/* =======================================================================
                              asffile.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/asfparser/main/latest/src/asffile.cpp#55 $
$DateTime: 2010/11/03 00:06:10 $
$Change: 1502826 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"

#ifdef FEATURE_QTV_WINDOWS_MEDIA

#include "asffile.h"
#include "wmfparse.h"
#include "wmfdec.h"
#include "Events.h"
#include "media.h"
#include "AEEstd.h"

#include "Mpeg4Player.h"
#if defined(FEATURE_QTV_WMA_PRO_DECODER) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
  #define WMA_PRO_TRACK_VERSION 3
  #define ENCOPT3_FRM_SIZE_MOD  0x0006
  #define FORMAT_WMAPRO           0x162
  #define WMAPRO_FRAME_IN_PACKET  1
#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

  #define SEEKABLE_BIT  0x08
  #define SPLICE_BIT    0x04
  #define WMA_DSP_PACKET_HEADER_LEN 6

#if defined (FEATURE_QTV_DRM_DCF) && defined (FEATURE_SME_DRMMS)
  #include "IxStreamMedia.h"
#endif

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

//This is req. to satisfy our WMA DSP. We need to send dummy bytes when there is no more data to decode
//before reporting end of audio media.
#define WMA_TOTAL_DUMMY_BYTES_TO_SEND 7000
#define WMA_DUMMY_BYTE 0xff
#define MAX_AUDIO_BIT_RATE_ALLOWED 256000

#if( defined (T_MSM6275) || defined (T_MSM6280) || defined (T_MSM6550) )
 /* block any video greater than 384KBPS */
 #define MAX_VIDEO_BIT_RATE_ALLOWED 384000

#elif ( defined (T_MSM6250) || defined(T_MSM6500) )
#error code not present
#endif

#ifdef FEATURE_QTV_FILE_IO_CACHING
#define FILE_READ_BUFFER_SIZE_FOR_WM 10240
#endif

//This feature is to be defined when we want to reject WMV track based on
//profile and bit rate.By default, this is turned OFF.

//#define CHECK_WMV_FOR_PROFILE_AND_BITRATE

//This feature is to be defined when we want to reject WMA track based on bit-rate.
//#define CHECK_WMA_FOR_BITRATE

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
/*
* Since we have 2.4 sec data lead for audio, make sure our minimum pre-roll is somewhat > 2.4 seconds.
* This is because, some clips have 1 second as pre-roll in meta-data, so we will run out of data as soon as we start playing,
* so, we will move to PLAYING and back to BUFFERING as soon as playback is started
*/
#define MIN_PREROLL_REQUIRED 3400
#endif

#define UNICODE_TO_UTF8 3

#define BYTE_1_REP          0x80    /* if <, will be represented in 1 byte */
#define BYTE_2_REP          0x800   /* if <, will be represented in 2 bytes */

#define SURROGATE_MIN       0xd800
#define SURROGATE_MAX       0xdfff

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
extern "C" tWMCDecStatus WMCDecGetNextIFrameTime( HWMCDECODER hWMCDec, U16_WMC wStreamId,
                                                  U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn,
                                                  U32_WMC* pdwPackets, U32_WMC* pdwRemainingPackets,
                                                  U64_WMC* pFirstAudioPTime, U32_WMC* pdwSCR );

extern "C" tWMCDecStatus WMCDecGetPreviousIFrameTime( HWMCDECODER hWMCDec, U16_WMC wStreamId,
                                                  U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn,
                                                  U32_WMC* pdwPackets, U32_WMC* pdwRemainingPackets,
                                                  U64_WMC* pFirstAudioPTime, U32_WMC* pdwSCR );


extern "C" tWMCDecStatus WMCDecGetIFrameTime (HWMCDECODER hWMCDec, U16_WMC wStreamId,
                                              U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn,
                                              U32_WMC* pdwFirstPacketOffset, U32_WMC* pdwPacketSize,
                                              U32_WMC* pdwPackets, U64_WMC* pu64FirstPayloadPresTime,
                                              U32_WMC * pFirstPayloadOffset, U8_WMC * pFirstPayloadOID,
                                              U32_WMC * pSendTime, U64_WMC *pFirstAudioPTime,
                                              U8_WMC * pFirstAudioPayloadOID);

extern "C" tWMCDecStatus WMCDecSeekToPrevI (HWMCDECODER hWMCDec, U16_WMC wStreamId, U64_WMC* iFrameTime);

extern "C" tWMCDecStatus WMCDecSeekToPrevIFrame (HWMCDECODER hWMCDec, U16_WMC wStreamId,U64_WMC* u64IFrameTimeReturn );

extern "C" tWMCDecStatus WMCDecGetLastBufferedAudioTS(HWMCDECODER hWMCDec, U64_WMC* audioTS);

extern "C" tWMCDecStatus WMCDecGetLastBufferedVideoTS(HWMCDECODER hWMCDec, U64_WMC* videoTS);

extern "C" tWMCDecStatus WMCDecHasGivenAnyOutput(HWMCDECODER hWMCDec, tMediaType_WMC Type);

extern "C" tWMCDecStatus WMCDecResetToFirstPacket(HWMCDECODER hWMCDec);
extern "C" tWMCDecStatus WMCDecGetPreviousFrameTime (HWMCDECODER hWMCDec, 
                                                     U16_WMC wStreamId, 
                                                     U64_WMC u64TimeRequest, 
                                                     U64_WMC* pu64TimeReturn);


static INLINE int32 ROUNDF(float f)
{
  if (f < 0.0f)
      return (int32) (f - 0.5f);
  else
      return (int32) (f + 0.5f);
}

static INLINE int32 LOG2(uint32 i)
{ // returns n where n = log2(2^n) = log2(2^(n+1)-1)
  uint32 iLog2 = 0;
  while ((i >> iLog2) > 1)
      iLog2++;
  return iLog2;
}

#if 0 /* for audio testing only */
int AudioFrameHeaderIndex = 0;
#endif

/* ======================================================================
FUNCTION
  WMCDecCBGetData

DESCRIPTION
  Its a callback function from the SDK to get the ASF data for decoding.
  This is not implemented by the SDK. It should be implemented by the app that calls the SDK.

ARGUMENTS
  hWMCDec                 Decoder object HWMCDECODER.
  U64_WMC nOffset         Offset of the requested data (from beginning),
  nNumBytesRequest        Size of the requested data (in bytes).
  ppData                  Pointer to the buffer for filling in the ASF data
  u32UserData             Extra info From App. Given by user in WMCDecCreate().

DEPENDENCIES
  Used by WM Parser.

RETURN VALUE
  The number of bytes actually being filled in

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
U32_WMC WMCDecCBGetData ( HWMCDECODER /*hWMCDec*/,
                          U64_WMC nOffset,
                          U32_WMC nNumBytesRequest/*, U32_WMC* pnNumBytesReturn*/,
                          U8_WMC **ppData,
                          U32_WMC u32UserData )
{
  if(u32UserData)
  {
    ASFFile *pAsfFile = (ASFFile *)u32UserData;
    return pAsfFile->FileGetData((uint32)nOffset, nNumBytesRequest, ppData);
  }
  return 0;
}

/* ======================================================================
FUNCTION:
 WMDRMDecrypt

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
#ifdef FEATURE_QTV_WM_DRM_API
boolean WMDRMDecrypt(uint32 wTrackID,              /* to which track this data belong to */
                     void *pEncryptedDataBuf,      /* pointer to encrypted data buffer, which has to be decrypted */
                     void *pDecryptedDataBuf,      /* pointer to destination buffer to copy decrypted data,
                                                      OEM is resposible for copying the decrypted data  */
                     uint32 wEncryptedDataSize,    /* encrypted data buffer size */
                     uint32 *pDecryptedDataSize,   /* pointer to decrypted data buffer size,
                                                      OEM is resposible for copying the decrypted data size  */
                     U32_WMC u32UserData)          /* holds ASFFile instance pointer */
#else
boolean WMDRMDecrypt(uint32 ,              /* to which track this data belong to */
                     void *,      /* pointer to encrypted data buffer, which has to be decrypted */
                     void *,      /* pointer to destination buffer to copy decrypted data,
                                                      OEM is resposible for copying the decrypted data  */
                     uint32 ,    /* encrypted data buffer size */
                     uint32 *,   /* pointer to decrypted data buffer size,
                                                      OEM is resposible for copying the decrypted data size  */
                     U32_WMC )          /* holds ASFFile instance pointer */
#endif /*FEATURE_QTV_WM_DRM_API*/
{
#ifdef FEATURE_QTV_WM_DRM_API
  if(!u32UserData)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "WMDRMDecrypt u32UserData is NULL");
    return false;
  }
  ASFFile *pAsfFile = (ASFFile *)u32UserData;
  return pAsfFile->DRMDecryptSwitch(wTrackID, pEncryptedDataBuf, pDecryptedDataBuf, wEncryptedDataSize, pDecryptedDataSize);
#else
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "FEATURE_QTV_WM_DRM_API is not enabled");
  return false;
#endif
}
/* ======================================================================
FUNCTION:
  ASFFile::FileGetData

DESCRIPTION:
  default constructor

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::FileGetData(  uint32 nOffset,
                              uint32 nNumBytesRequest,
                              uint8 **ppData  )
{
  uint32 nRead = 0;
  if (m_AsfFilePtr != NULL)
  {
    if( (m_pMpeg4Player) && (m_pMpeg4Player->m_bMediaAbort) )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Failed to readFile:User aborted playback..!!");
    }
    else
    {
      //Before reading from file, check that the data is in range. If size on disk (regardless
      //of actual size of file) is less than the data requested, this will abort. This check is
      //only in case of local file playback.
      if(m_filesize >= nOffset + nNumBytesRequest )
        nRead = FileBase::readFile(m_AsfFilePtr, m_ReadBuffer, nOffset, MIN(nNumBytesRequest,ASF_READ_BUFFER_SIZE));
    }
    if( (!nRead)
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                && (!bHttpStreaming)
#endif
      )
    {
      //Need to do only for local playback.
      //It is possible to have 0 bytes being read in PD,PS.
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Failed to readFile");
      //Set the error code to let player know the exact failure.
      //This will help to stop/abort the playback.
       _fileErrorCode = (int32)READ_FAILED;
    }
  }
  else if (m_dataSource != NULL)
  {
    nRead = m_dataSource->readData(m_ReadBuffer, nOffset, nNumBytesRequest);
  }
  *ppData = m_ReadBuffer;
  return nRead;
}

/* ======================================================================
FUNCTION:
  ASFFile::getPrerollTime

DESCRIPTION:
  Returns the preroll time for the stream.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Preroll time.

SIDE EFFECTS:
  None.
======================================================================*/

uint32 ASFFile::getPrerollTime()
{
    return m_strHeaderInfo.u32Preroll;
}

/* ==============================================================================
FUNCTION:
  ASFFile::getFileSize

DESCRIPTION:
  Returns the size of the file

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  File size

SIDE EFFECTS:
  None.
==============================================================================*/
uint64 ASFFile::getFileSize()
{
  return m_strHeaderInfo.u64TotalSize;
}

/* =============================================================================
FUNCTION:
  ASFFile::getCodecName

DESCRIPTION:
  Returns the name of the codec

INPUT/OUTPUT PARAMETERS:
  codecName          Buffer to copy the codec name into
  bufLen             Length of buffer to store the codec name
  mediaType          Specifies which codec name to get, audio or video

RETURN VALUE:
  Return the length of the copied string.

SIDE EFFECTS:
  None
==============================================================================*/
int32 ASFFile::getCodecName
(
  char *codecName,
  int32 bufLen,
  tMediaType_WMC mediaType
)
{
  QTV_NULL_PTR_CHECK(codecName, -1);

  if (bufLen <= 0)
  {
    return -1;
  }

  WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);

  for (unsigned int i = 0; i < pDecoder->m_dwNumCodec; i++)
  {
    if (pDecoder->m_pCodecEntry[i].m_wCodecType == mediaType)
    {
      WideCharToChar( (const wchar_t *)pDecoder->m_pCodecEntry[i].m_pwCodecName,
                      -1,
                      codecName,
                      bufLen);

      /* Change spaces to underscore */

      int j = 0;
      while (true)
      {
        if (codecName[j] == '\0')
        {
          return j;
        }

        if (codecName[j] == ' ')
        {
          codecName[j] = '_';
        }
        j++;
      }
    }
  }

  return -1;
}

/* ======================================================================
FUNCTION:
  ASFFile::ASFFile

DESCRIPTION:
  default constructor

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
ASFFile::ASFFile() // Default Constructor
{
  InitData();
}
/* ======================================================================
FUNCTION:
  ASFFile::InitData()

DESCRIPTION:
  Initializes class members

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 None

SIDE EFFECTS:
  None.
======================================================================*/
void ASFFile::InitData()
{
  _success = false;
  _fileErrorCode = DEFAULT_ERROR;
  m_corruptFile = false;
  m_bVideoReposPending = false;
  m_bAudioReposPending = false;
  m_nNumStreams = 0;
  m_ptMediaTypeStreams = NULL;
  m_pStreamDecodePattern = NULL;
  m_hASFDecoder = NULL;
  m_pContentDesc = NULL;
  m_pStreamDecodePattern = NULL;
  m_pasfDataBuffer = NULL;
  //Initialize this to a large value here. Unless we know the file size for sure, we
  //will use this value.
  m_filesize = 0x7FFFFFFF;
  m_isDSPWMA_pro = false;
  #if 0 /* for audio testing only */
    AudioFrameHeaderIndex = 0;
  #endif

  #ifdef FEATURE_QTV_DRM_DCF
    m_inputStream = NULL;
  #endif

  #if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    bHttpStreaming = FALSE;
    bGetMetaDataSize = TRUE;
    bIsMetaDataParsed = FALSE;
    m_minOffsetRequired = 0;
    m_HttpDataBufferMinOffsetRequired.Offset = 0;
    m_HttpDataBufferMinOffsetRequired.bValid = FALSE;
    parserState = Common::PARSER_IDLE;
    m_startupTime = HTTP_DEFAULT_STARTUP_TIME;
    memset(m_maxPlayableTime, 0, sizeof(m_maxPlayableTime));
    m_fpFetchBufferedDataSize = NULL;
    m_fpFetchBufferedData = NULL;
  #endif //#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

  #ifdef FEATURE_QTV_WMA_PRO_DECODER
    m_bUseARMDecoder   = false;
    memset(&m_sWAVEFormatEx,0,sizeof(m_sWAVEFormatEx));
  #endif

  #ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
    pwfioOut = NULL;
  #endif

  #ifdef FEATURE_MEASURE_TIMING
    startTimeGetMetadata = 0;
    endTimeGetMetadata = 0;
  #endif

  #if defined (FEATURE_QTV_DRM_DCF) || defined (FEATURE_QTV_WM_DRM_API)
    pDRMClientData = NULL;
  #endif

  m_pMpeg4Player = NULL;
  m_dataSource = NULL;
  m_AsfFilePtr = NULL;

  m_playAudio = false;
  m_playVideo = false;
  m_playText = false;
  m_filename = "";
  m_pFileBuf = NULL;
  m_FileBufSize = 0;
  m_bStreaming = FALSE;

  m_VidProfile = UNKNOWN_PROFILE;
  m_pAudioPacketBuffer = NULL;
  m_nAudioPacketBufferSize = 0;
  m_nWmaNumPrevFrameBits = 0;
  m_nTotalAudioBytesSent = 0;
  m_bAllowWmaPackets = true;   /* default value is allow WMA packets */

  /*
  * m_bWMADecodeDone is set to true when there is no more WMA data to decode.
  * This is done to satisfy our WMA DSP.
  */
  m_bWMADecodeDone = false;
  m_nWMATotalDummyBytesSent = 0;

  m_bIsDummyBytesStart = false;

  /*
  * Estimated value for each ASF audio packet duration.
  * This is used in calculating how much data is buffered.
  */
  m_nASFAudioPacketDuration = 0;
  m_nCntPrvBytes = 0;
  m_nCntCurBytes = 0;
  m_bBufferValid = false;

  //Initialized audio, video and binary streamIDs.
  m_nSelectedAudioStreamId = -1;
  m_nSelectedVideoStreamId = -1;
  m_nSelectedBinaryStreamId = -1;

  //Initialized ASF DATA Buffer size to 0;
  m_nAsfDataBufferSize = 0;

  #if 0 /* for audio testing only */
    AudioFrameHeaderIndex = 0;
  #endif

  memset(&m_wmStreamingInfo,0,sizeof(m_wmStreamingInfo));
  memset(m_sampleInfo, 0, sizeof(m_sampleInfo));
  memset(m_nDecodedDataSize, 0, sizeof(m_nDecodedDataSize));
  memset(m_nLargestFrame, 0, sizeof(m_nLargestFrame));
  memset(m_ReadBuffer, 0, sizeof(m_ReadBuffer));
  memset(&m_strHeaderInfo, 0, sizeof(m_strHeaderInfo));
  memset(&m_strAudioInfo, 0, sizeof(m_strAudioInfo));
  memset(&m_strVideoInfo, 0, sizeof(m_strVideoInfo));
  memset(&m_strBinaryInfo, 0, sizeof(m_strBinaryInfo));
  memset(m_prvSampleInfo,0,sizeof(m_prvSampleInfo));

  //Variables used to detect packet drop
  m_nPreviousAudioPacketNum = 0;
  m_nPreviousAudioPacketFramesNum = 0;
  m_bIsPacketDrop = false;
}

#ifdef FEATURE_QTV_DRM_DCF
/* ======================================================================
FUNCTION:
  ASFFile::ASFFile

DESCRIPTION:
  Constructor for supporting DCF media playback

INPUT/OUTPUT PARAMETERS:
  ->inputStream: IxStream created in Mpeg4Player
  ->bPlayVideo: Indicating whether this is Video instance
  ->bPlayAudio: Indicating whether this is Audio instance

RETURN VALUE:
 None

SIDE EFFECTS:
  None.
======================================================================*/
ASFFile::ASFFile( dcf_ixstream_type inputStream, Mpeg4Player *pMpeg4Player, bool bPlayVideo, bool bPlayAudio)
{
  InitData();
  m_pMpeg4Player= pMpeg4Player;
  m_playAudio = bPlayAudio;
  m_playVideo = bPlayVideo;
  m_playText = false;
  m_inputStream =  inputStream;
  if(!inputStream)
  {
    return;
  }
  IxStream* pStream = (IxStream* )m_inputStream;
  #if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    if(pStream->IsProgressive())
    {
      bHttpStreaming = true;
      bGetMetaDataSize = TRUE;
      bIsMetaDataParsed = FALSE;
      m_HttpDataBufferMinOffsetRequired.Offset = 0;
      m_HttpDataBufferMinOffsetRequired.bValid = FALSE;
      m_FileBufSize = 0x7FFFFFFF;
      m_bStreaming = FALSE;//no rtsp streaming
      parserState = Common::PARSER_IDLE;
      memset(m_maxPlayableTime, 0, sizeof(m_maxPlayableTime));
      //Get from IxStream C interface
      m_fpFetchBufferedDataSize = (QtvPlayer::FetchBufferedDataSizeT) ixstream_length;
    }
    else
  #endif //#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    {
      //Get the actual file size when doing local file playback
      (void)pStream->Size(&m_filesize);
    }

  /*
  * Currently we handle audio and video streams only so
  * don't create file instance and decoder for other type of streams.
  */

  if(!(m_playAudio || m_playVideo))
  {
    _success= true;
    m_pStreamDecodePattern = NULL;
    m_hASFDecoder = NULL;
    m_pAudioPacketBuffer = NULL;
    m_AsfFilePtr = NULL;
    #if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      if(bHttpStreaming)
      {
        sendParserEvent(Common::PARSER_READY);
      }
    #endif
    return;
  }
    m_isDSPWMA_pro  = false;
    m_AsfFilePtr = OSCL_FileOpen(m_inputStream);

  if(m_AsfFilePtr || m_bStreaming)
  {
       QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "ASFFILE constructor for dcf_ixstream_type, calling OSCL_FileTell");
       OSCL_FileTell(  m_AsfFilePtr );
    m_hASFDecoder = WMCDecCreate ((U32_WMC)this);
       if(m_hASFDecoder != NULL )
    {
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      if (bHttpStreaming)
      {
        parseHTTPStream();
        _success= true;
        return;
      }
#endif
         if( ParseMetaData() == WMCDec_Succeeded )
         {
           _success = true;
      }
       } /* end of if(m_hASFDecoder != NULL ) */
  } /* end of if(m_AsfFilePtr || m_bStreaming) */
}
#endif /*FEATURE_QTV_DRM_DCF*/

/* ======================================================================
FUNCTION:
  ASFFile::ASFFile

DESCRIPTION:
  local file playback Constructor

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
ASFFile::ASFFile( const OSCL_STRING &filename,
                  Mpeg4Player *pMpeg4Player,
                  unsigned char *pFileBuf,
                  uint32 bufSize,
                  IDataSource *dataSource,
                  bool bPlayVideo,
                  bool bPlayAudio
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                 ,bool bHttpStream
                 ,uint32
                 ,uint32 wStartupTime
                 ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize
                 ,QtvPlayer::FetchBufferedDataT FetchBufferedData
                 ,QtvPlayer::InstanceHandleT handle
#endif //#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                      )
{
  InitData();

  m_pMpeg4Player= pMpeg4Player;
  m_dataSource = dataSource;
  m_playAudio = bPlayAudio;
  m_playVideo = bPlayVideo;

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      bHttpStreaming = bHttpStream;
      m_startupTime = wStartupTime;
      m_fpFetchBufferedDataSize = FetchBufferedDataSize;
      m_fpFetchBufferedData = FetchBufferedData;
      m_QtvInstancehandle = handle;
#endif //#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

  /*
  Currently we handle audio and video streams only so
  don't create file instance and decoder for other type of streams.
  */

  if(!(m_playAudio || m_playVideo))
  {
    _success= true;
    m_pStreamDecodePattern = NULL;
    m_hASFDecoder = NULL;
    m_pAudioPacketBuffer = NULL;
    m_AsfFilePtr = NULL;

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    if(bHttpStreaming)
    {
      sendParserEvent(Common::PARSER_READY);
    }
#endif
    return;
  }

  if (pFileBuf && bufSize)
  {
    m_pFileBuf = pFileBuf;
    m_FileBufSize = bufSize;
    m_filename = "";
    m_dataSource = NULL;
    m_bStreaming = FALSE;
    m_AsfFilePtr = OSCL_FileOpen (pFileBuf, bufSize);
  }
  else if (m_dataSource != NULL)
  {
    m_bStreaming = TRUE;
    m_filename = "";
    m_pFileBuf = NULL;
    m_FileBufSize = 0;
    m_AsfFilePtr = NULL;
  }
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  else if (m_fpFetchBufferedDataSize)
  {
     //Out here we dont know the file size
     m_FileBufSize = 0xFFFFFFFF;
     m_bStreaming = FALSE;
     //Using default trackId 0 for parsing..
     m_AsfFilePtr = OSCL_FileOpen(NULL, 0, m_fpFetchBufferedData, 0, m_QtvInstancehandle);
  }
#endif //#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  else
  {
    m_filename = filename;
    m_pFileBuf = NULL;
    m_FileBufSize = 0;
    m_bStreaming = FALSE;
#ifdef FEATURE_QTV_FILE_IO_CACHING
    /* Calling with 10K cache  buffer size */
    m_AsfFilePtr = OSCL_FileOpen(filename, (OSCL_TCHAR *) _T("rb"), FILE_READ_BUFFER_SIZE_FOR_WM );
#else
    m_AsfFilePtr = OSCL_FileOpen(filename, (OSCL_TCHAR *) _T("rb"));
#endif
    m_filesize = OSCL_FileSize( filename );
  }

  if(m_AsfFilePtr || m_bStreaming)
  {
    m_hASFDecoder = WMCDecCreate ((U32_WMC)this);
    if(m_hASFDecoder != NULL )
    {
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      if (bHttpStreaming)
      {
        bGetMetaDataSize = TRUE;
        parseHTTPStream();
        _success= true;
        return;
      }
#endif
      if( ParseMetaData() == WMCDec_Succeeded )
      {
         _success = true;
      }
    } /* end of if(m_hASFDecoder != NULL ) */

  } /* end of if(m_AsfFilePtr || m_bStreaming) */

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif
}

#ifdef FEATURE_QTV_WM_SWITCHING_THINNING
/* ======================================================================
FUNCTION:
  ASFFile::MarkVideoTrackAsValid

DESCRIPTION:
  For an MBR clip it marks the valid video track to fetch data from.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
void ASFFile::MarkVideoTrackAsValid(uint32 id)
{
  if(m_playVideo)
  {
    for (int i=0; i<(int)m_nNumStreams ; i++ )
    {
      switch(m_ptMediaTypeStreams[i].MediaType)
      {
        case Video_WMC:
        {
          if(m_pStreamDecodePattern[i].wStreamId == id)
          {
            m_pStreamDecodePattern[i].tPattern = Compressed_WMC;
          }
          else
          {
            m_pStreamDecodePattern[i].tPattern = Discard_WMC;
          }
        }
      }
    }
    tWMCDecStatus wmerr = WMCDecSetDecodePatternForStreams (&m_hASFDecoder, m_pStreamDecodePattern);
    if (wmerr != WMCDec_Succeeded)
    {
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "MarkVideoTrackAsValid failed.. id=%d",id);
    }
  }
  return;
}
#endif //FEATURE_QTV_WM_SWITCHING_THINNING

/* ======================================================================
FUNCTION:
  ASFFile::~ASFFile

DESCRIPTION:
  destructor

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
ASFFile::~ASFFile()
{
  if (m_AsfFilePtr != NULL)
  {
    OSCL_FileClose(m_AsfFilePtr);
    m_AsfFilePtr = NULL;
  }

  if (m_dataSource != NULL)
  {
    QTV_Delete(m_dataSource);
    m_dataSource = NULL;
  }

  if (m_pStreamDecodePattern != NULL)
  {
    QTV_Free(m_pStreamDecodePattern);
    m_pStreamDecodePattern = NULL;
  }

  if (m_pAudioPacketBuffer != NULL)
  {
    QTV_Free(m_pAudioPacketBuffer);
    m_pAudioPacketBuffer = NULL;
  }

  if (m_pasfDataBuffer != NULL)
  {
    QTV_Free(m_pasfDataBuffer);
    m_pasfDataBuffer = NULL;
  }

  if (m_hASFDecoder != NULL)
  {
    WMCDecClose (&m_hASFDecoder);
    m_hASFDecoder = NULL;
  }
#ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
  if(pwfioOut)
  {
    wfioClose(pwfioOut);
    wfioDelete (pwfioOut);
    pwfioOut = NULL;
}
#endif
}

/* ======================================================================
FUNCTION:
  ASFFile::getNextMediaSample

DESCRIPTION:
  gets next sample of the given track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 size of sample

SIDE EFFECTS:
  None.
======================================================================*/
int32 ASFFile::getNextMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 &index)
{
  tMediaType_WMC streamType;
  U32_WMC nOutDataSize = 0;
  tWMCDecStatus nRet = WMADec_Failed;

#ifdef FEATURE_QTV_WMA_PRO_DECODER
    U32_WMC nSamples = 0;
#endif

#if 0
  //For simulating audio repositioning..Audio Testing only..
  bool test_error = false;
  int cnt = 0;
  int nFramesBeforeReposition = 20;
  int repos_count =0;
  uint32 newtime = 0;
 #ifdef FEATURE_QTV_WMA_PRO_DECODER
    //When PCM output is taken out 'pcm_output_count' times,
    //Reposiiton will be done.
    int pcm_output_count = 50;
  #endif
#endif

  nRet = GetMediaTypeForStreamID(&streamType, id);
  if(nRet == WMCDec_Succeeded)  {
    switch(streamType)
    {
      case Video_WMC:
        do
        {
          nRet = GetVideoFrame(id, streamType, buf, size, &nOutDataSize, m_bVideoReposPending);
        }while( (nRet==WMCDec_Succeeded) && (nOutDataSize==0) );

        break;
      case Audio_WMC:
#if 0  /* for audio testing */
        {
          FILE *fp = NULL;
          char outFile[256];
          int start_packet=0;
          const char *inFile = m_filename.get_cstr();
          uint16 u16Temp;
          int i;
          for(i=0; (inFile[i]!='.')&&(inFile[i]!='\0'); i++)
          {
            outFile[i] = inFile[i];
          }
#ifdef FEATURE_QTV_WMA_PRO_DECODER
            if((m_bUseARMDecoder)&&(!m_bWMADecodeDone))
            {
              #ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
                std_strlcpy(outFile+i, ".wav", (sizeof(outFile) - i));
                pwfioOut = wfioNew ();
                if(wfioOpen (pwfioOut, outFile, &m_sWAVEFormatEx, sizeof(m_sWAVEFormatEx), 1) != 0)
                {
                  if(pwfioOut)
                  {
                    free(pwfioOut);
                    pwfioOut = NULL;
                  }
                }
              #endif//#ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
            }
            else if(!m_bWMADecodeDone)
            {
          std_strlcpy(outFile+i, ".strm", (sizeof(outFile) - i));
          fp = fopen(outFile, "w");
            }
          #else
            {
          std_strlcpy(outFile+i, ".strm", (sizeof(outFile) - i));
          fp = fopen(outFile, "w");
           }
          #endif//#ifdef FEATURE_QTV_WMA_PRO_DECODER
          if( (fp)
                  #ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
                    ||(pwfioOut)
                  #endif
            )
          {
            /* read first packet number from where we will start writing in file */
            FILE *fp1;
            char outFile1[256];
            const char *inFile1 = m_filename.get_cstr();
            for(i=0; (inFile1[i]!='.')&&(inFile1[i]!='\0'); i++)
            {              outFile1[i] = inFile1[i];
            }
            std_strlcpy(outFile1+i, ".txt", (sizeof(outFile1) - i));
            fp1 = fopen(outFile1, "r");
            if(fp1)
            {
              int nRet = fread(buf, 1, 4, fp1);
              buf[nRet] = '\0';
              start_packet = atoi((char*)buf);
              fclose(fp1);
            }

#if 0
            #ifdef FEATURE_QTV_WMA_PRO_DECODER
             if(!m_bUseARMDecoder)
            #endif
            {
            /* first write DSP ADEC CONFIG command */
            /* first two bytes are 0s */
            buf[0] = 0;
            buf[1] = 0;
            /* next byte is version */
            buf[2] = (uint8)getTrackContentVersion(id);
            /* next byte is number of channels */
            buf[3] = (uint8)GetNumAudioChannels(id);
            /* next two bytes (next word) is bytes per second */
            u16Temp = htons(((uint16)(getTrackAverageBitrate(id)/8)));
            buf[4] = *((uint8*)&u16Temp);
            buf[5] = *(((uint8*)&u16Temp)+1);
            //*((uint16*)(buf+4)) = htons(((uint16)(getTrackAverageBitrate(id)/8)));
            /* next two bytes (next word) is sampling frequency */
            u16Temp = htons((uint16)getTrackAudioSamplingFreq(id));
            buf[6] = *((uint8*)&u16Temp);
            buf[7] = *(((uint8*)&u16Temp)+1);
            //*((uint16*)(buf+6)) = htons((uint16)getTrackAudioSamplingFreq(id));
            /* next two bytes (next word) is encoder options */
            u16Temp = htons(GetAudioStreamInfo(id)->nEncodeOpt);
            buf[8] = *((uint8*)&u16Temp);
            buf[9] = *(((uint8*)&u16Temp)+1);
            //*((uint16*)(buf+8)) = htons(GetAudioStreamInfo(id)->nEncodeOpt);
            fwrite(buf, 1, 10, fp);
            }
#endif
            /* now write all the audio stream */
            i=0;
            do
            {
              nOutDataSize=0;
              #ifdef FEATURE_QTV_WMA_PRO_DECODER
                if(m_bUseARMDecoder)
                {
                  nRet = GetAudioDecodedSamples(id, streamType, buf,size, &nOutDataSize,&nSamples);
                }
                else
              #endif
              {
                  nRet = GetAudioFrame(id, streamType, buf, size, &nOutDataSize);
              }
              if(nOutDataSize)
              {
                 if( (i==start_packet) && (AudioFrameHeaderIndex == 0)
                     #ifdef FEATURE_QTV_WMA_PRO_DECODER
                                      && (!m_bUseARMDecoder)
                     #endif
                  )
                {
                  /* this is the first packet to write, make sure header starts at
                     even byte */
                  if( (buf[AudioFrameHeaderIndex]==0xff) && (buf[AudioFrameHeaderIndex+1]==0xf0) )
                  {

                    //Do not output to the file before repositioning
                    //if((cnt >= nFramesBeforeReposition )&&(repos_count == 6))
                      fwrite(buf+AudioFrameHeaderIndex, 1, nOutDataSize-AudioFrameHeaderIndex, fp);
                  }
                }
                else
                {
                  #ifdef FEATURE_QTV_WMA_PRO_DECODER
                    if(m_bUseARMDecoder)
                    {
                      #ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
                        //Do not output to the file if pcm_output_count > 0.
                        //This is used to simulate audio repositioning.
                        //if(pcm_output_count == 0)
                        {
                          int nbytesWrote = wfioWrite (pwfioOut, (U8_WMC*) buf, nOutDataSize);
                          QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "nOutDataSize %d nbytesWrote %d total# bytes dumped so far %d",nOutDataSize,nbytesWrote,m_nTotalAudioBytesSent );
                        }
                       /* if(!repos_count)
                        {
                          pcm_output_count--;
                        }*/
                      #endif
                    }
                    else
                  #endif
                    {
                  //Do not output to the file before repositioning
                  //if((cnt >= nFramesBeforeReposition )&&(repos_count == 6))
                    fwrite(buf, 1, nOutDataSize, fp);
                }
              }
              }
              cnt++;
              i++;

              /*#ifdef FEATURE_QTV_WMA_PRO_DECODER
                if( (m_bUseARMDecoder) && (pcm_output_count == 0) && (repos_count == 0) )
                {
                  newtime = resetPlayback((newtime+40000),id,false,&test_error,newtime);
                  repos_count++;
                }
              #endif*/
                /*{
              if((cnt >= nFramesBeforeReposition )&& (repos_count<6))
              {
               newtime = resetPlayback((newtime+10000),id,false,&test_error,newtime);
               repos_count++;
              }
             }*/
            }
            while(nRet == WMCDec_Succeeded);
            #ifdef FEATURE_QTV_WMA_PRO_DECODER
              if(m_bUseARMDecoder)
              {
                 #ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
                   if( (pwfioOut) && (nRet == WMCDec_DecodeComplete) )
                   {
                     wfioClose(pwfioOut);
                     wfioDelete (pwfioOut);
                     pwfioOut = NULL;
                   }
                 #endif
              }
              else
            #endif
            {
              fclose(fp);
            }
            nOutDataSize = 0;
          }
        }
#else
  #if 0     /* insert DSP ADEC CONFIG command before first frame for testing CMX */
            /* first two bytes are 0s */
        {
            if(AudioFrameHeaderIndex ==0)
            {
              uint16 u16Temp;
              buf[0] = 0;
              buf[1] = 0;
              /* next byte is version */
              buf[2] = (uint8)getTrackContentVersion(id);
              /* next byte is number of channels */
              buf[3] = (uint8)GetNumAudioChannels(id);
              /* next two bytes (next word) is bytes per second */
              u16Temp = htons(((uint16)(getTrackAverageBitrate(id)/8)));
              buf[4] = *((uint8*)&u16Temp);
              buf[5] = *(((uint8*)&u16Temp)+1);
              //*((uint16*)(buf+4)) = htons(((uint16)(getTrackAverageBitrate(id)/8)));
              /* next two bytes (next word) is sampling frequency */
              u16Temp = htons((uint16)getTrackAudioSamplingFreq(id));
              buf[6] = *((uint8*)&u16Temp);
              buf[7] = *(((uint8*)&u16Temp)+1);
              //*((uint16*)(buf+6)) = htons((uint16)getTrackAudioSamplingFreq(id));
              /* next two bytes (next word) is encoder options */
              u16Temp = htons(GetAudioStreamInfo(id)->nEncodeOpt);
              buf[8] = *((uint8*)&u16Temp);
              buf[9] = *(((uint8*)&u16Temp)+1);
              //*((uint16*)(buf+8)) = htons(GetAudioStreamInfo(id)->nEncodeOpt);
              buf = buf + 10;
              nRet = GetAudioFrame(id, streamType, buf, size-10, &nOutDataSize);
              if(nRet == WMCDec_Succeeded)
              {
                nOutDataSize = nOutDataSize + 10;
                AudioFrameHeaderIndex=1;
              }
            }
            else
            {
              nRet = GetAudioFrame(id, streamType, buf, size, &nOutDataSize);
            }
        }
  #else
        do
        {
          #ifdef FEATURE_QTV_WMA_PRO_DECODER
          if(m_bUseARMDecoder && isWmaProDecoderNeeded(id) )
          {
            nRet = GetAudioDecodedSamples(id, streamType, buf, size, &nOutDataSize,&nSamples);

            #ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
              if( (pwfioOut==NULL)    &&
                  (!m_bWMADecodeDone) &&
                  (!m_bStreaming)
                  #if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                                      && (!bHttpStreaming)
                  #endif
                )
              {
                char outFile[256];
                const char *inFile = m_filename.get_cstr();
                int i;
                for(i=0; (inFile[i]!='.')&&(inFile[i]!='\0'); i++)
                {
                  outFile[i] = inFile[i];
                }
                std_strlcpy(outFile+i, ".wav", (sizeof(outFile) - i));
                pwfioOut = wfioNew ();
                if(wfioOpen (pwfioOut, outFile, &m_sWAVEFormatEx, sizeof(m_sWAVEFormatEx), 1) != 0)
                {
                  if(pwfioOut)
                  {
                    free(pwfioOut);
                    pwfioOut = NULL;
                  }
                }
              }
              if( (pwfioOut) && (nRet == WMCDec_DecodeComplete) )
              {
                wfioClose(pwfioOut);
                wfioDelete (pwfioOut);
                pwfioOut = NULL;
              }
              else if( (pwfioOut) && (nRet == WMCDec_Succeeded) )
              {
                int nbytesWrote = wfioWrite (pwfioOut, (U8_WMC*) buf, nOutDataSize);
                QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "nOutDataSize %d nbytesWrote %d total# bytes dumped so far %d",nOutDataSize,nbytesWrote,m_nTotalAudioBytesSent );
              }
            #endif//#ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
          }
          else
          #endif
          {
#ifndef OUTPUT_STANDARD_WMA_BITSTREAM
            nRet = GetAudioFrame(id, streamType, buf, size, &nOutDataSize);
#else
#error code not present
#endif

          }
        }
        while( (nRet==WMCDec_Succeeded) && (nOutDataSize==0) );
  #endif
#endif
        break;
      case Binary_WMC:
      default:
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "getNextMediaSample: unsupported stream type=%d", streamType);
        nRet = WMCDec_ImproperOutputType;
        break;
    }
  }
  index = 0;
  if(nRet == WMCDec_Succeeded)
    return nOutDataSize;
  else if (nRet == WMCDec_BufferTooSmall )
    return INSUFFICIENT_BUFFER_SIZE;
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  else if (bHttpStreaming && (nRet == WMCDec_HttpBufferUnderRun) )
  {
    QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "HTTP Stream Buffer Underrun m_playAudio = %d, m_playVideo = %d, m_playText = %d", m_playAudio,m_playVideo,m_playText);
    m_HttpDataBufferMinOffsetRequired.bValid = FALSE;
    sendHTTPStreamUnderrunEvent();
    return INSUFFICIENT_DATA;
  }
#endif//#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

  if( m_bStreaming && (nRet == WMCDec_DecodeComplete) && (m_dataSource != NULL))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Calling IDataSource::SetEOS..");
    m_dataSource->setEOS(true);
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::GetVideoFrame

DESCRIPTION:
  gets next sample of the video stream. If SyncFrame is set, it will give next
  I-Frame;

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 size of sample

SIDE EFFECTS:
  None.
======================================================================*/
tWMCDecStatus ASFFile::GetVideoFrame( uint32 id, tMediaType_WMC streamType, uint8 *buf,
                                      uint32 size, U32_WMC * pOutDataSize, bool bSyncFrame )
{
  U32_WMC nStreamIdReady = 0xff;
  U32_WMC nDecodedDataSize;
  tWMCDecStatus nRet = WMADec_Failed;
  I64_WMC PresentationTime=0;
  Bool_WMC IsKeyFrame=0;
  uint32 StreamNum;

  if(streamType!=Video_WMC)
  {
    return WMADec_Failed;
  }

  *pOutDataSize = 0;
  nRet = GetStreamNumForID(&StreamNum, id);
  if(nRet == WMCDec_Succeeded)
  {
    /* loop till you find a Sync frame, if bSyncFrame is TRUE */
    do
    {
      m_nDecodedDataSize[StreamNum] = 0;

      /* loop till you decode some sample of desired stream */
      while( (nRet==WMCDec_Succeeded) && (m_nDecodedDataSize[StreamNum]==0) )
      {
        nDecodedDataSize = 0;
        nRet = WMCDecDecodeData (m_hASFDecoder, &nStreamIdReady, &nDecodedDataSize, (-1));
        if( (nDecodedDataSize) && (nRet==WMCDec_Succeeded))
        {
          uint32 localStreamNum;
          GetStreamNumForID(&localStreamNum, nStreamIdReady);
          m_nDecodedDataSize[localStreamNum] = nDecodedDataSize;
        }
#ifdef FEATURE_QTV_WM_SWITCHING_THINNING
        if((m_bStreaming) && nDecodedDataSize)
            break;
#endif //FEATURE_QTV_WM_SWITCHING_THINNING
      }

      if(nRet != WMCDec_Succeeded)
      {
        break;
      }

      /* if we don't have enough buffer, save largest frame size and return error */
      if(m_nDecodedDataSize[StreamNum] > size)
      {
        m_nLargestFrame[StreamNum] = MAX(m_nDecodedDataSize[StreamNum], m_nLargestFrame[StreamNum]);
        return WMCDec_BufferTooSmall;
      }

      /* read the sample */
      nRet = WMCDecGetVideoOutput( m_hASFDecoder, buf, size, pOutDataSize,
                                     &PresentationTime, &IsKeyFrame, WMC_DispRotate0 );

      if(bSyncFrame)
      {
        if(IsKeyFrame)
        {
          m_bVideoReposPending = false;
          if(m_bStreaming)
            m_wmStreamingInfo.bVideoValid = false;
          break;
        }
      }
      else
      {
        break;
      }
    }while(nRet==WMCDec_Succeeded);

    if(bSyncFrame && !IsKeyFrame)
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "GetVideoFrame: Sync frame not found.");
    }

    if(nRet==WMCDec_Succeeded)
    {
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "GetVideoFrame: PresentationTime is %ld",PresentationTime);

      /* for repositioning we don't update delta as difference between this and last
         frame will be huge */

      if((!bSyncFrame)&&(!m_wmStreamingInfo.bVideoValid))
        m_sampleInfo[StreamNum].delta = (uint32)(PresentationTime - m_sampleInfo[StreamNum].time);

      m_sampleInfo[StreamNum].sync = IsKeyFrame;
      m_sampleInfo[StreamNum].size = *pOutDataSize;
      m_sampleInfo[StreamNum].num_frames = 1; /* we only get one video frame from ASF parser */
      m_sampleInfo[StreamNum].time = (uint32)PresentationTime;

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      m_maxPlayableTime[StreamNum] = MAX( m_maxPlayableTime[StreamNum],m_sampleInfo[StreamNum].time );
#endif
    }
  } /* end of if (GetStreamNumForID == Success) */
  return nRet;
}
#ifdef FEATURE_QTV_WMA_PRO_DECODER
/* ======================================================================
FUNCTION:
  ASFFile::PCMFormat2WaveFormatEx

DESCRIPTION:
  prepares WaveFormatEx from PCM format.

INPUT/OUTPUT PARAMETERS:
PCMFormat* pFormat: PCM format to be used for preparing WAVEFORMATEX.
WAVEFORMATEX* wfx : wfx is populated using PCM format structure mentioned above.
RETURN VALUE:
 size of sample

SIDE EFFECTS:
  None.
======================================================================*/
void ASFFile::PCMFormat2WaveFormatEx(PCMFormat* pFormat,
                                     WAVEFORMATEX* wfx)
{
    wfx->cbSize = 0;
    if (pFormat->pcmData == PCMDataPCM)
        wfx->wFormatTag = WAVE_FORMAT_PCM;
    else if (pFormat->pcmData == PCMDataIEEE_FLOAT)
        wfx->wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx->nChannels  = (U16_WMC)pFormat->nChannels;
    wfx->nSamplesPerSec  = pFormat->nSamplesPerSec;
    wfx->wBitsPerSample = (U16_WMC)pFormat->nValidBitsPerSample;
    wfx->nBlockAlign = wfx->nChannels * ((wfx->wBitsPerSample + 7) / 8);
    wfx->nAvgBytesPerSec = wfx->nBlockAlign * wfx->nSamplesPerSec;
}
/* ======================================================================
FUNCTION:
  ASFFile::GetAudioDecodedSamples

DESCRIPTION:
  Gets bunch of PCM samples for the given audio track.

INPUT/OUTPUT PARAMETERS:
INPUT:
uint32 id                : Track id identifying the audio track for which PCM samples will be retrieved.
tMediaType_WMC streamType: Stream type(audio/video/text).It has to be AUDIO_WMC.
U8_WMC * buf             :Buffer to which PCM samples will be written.
U32_WMC size             :Max buffer size available.
U32_WMC * pOutDataSize   :Total size of PCM samples retrieved in bytes.
U32_WMC * pnSamples      :Actual number of PCM samples retrieved.

OUTPUT:
'tWMCDecStatus': Returns WMCDec_Succeeded if PCM sample retrieval is successful otherwise
                 returns appropriate error code.
RETURN VALUE:
 size of sample

SIDE EFFECTS:
  None.
======================================================================*/
tWMCDecStatus ASFFile::GetAudioDecodedSamples(uint32 id,
                                              tMediaType_WMC streamType,
                                              U8_WMC * buf,
                                              U32_WMC size,
                                              U32_WMC * pOutDataSize,
                                              U32_WMC * pnSamples)
{
  U32_WMC nStreamIdReady = 0xff;
  U32_WMC nDecodedDataSize;
  tWMCDecStatus nRet = WMADec_Failed;
  I64_WMC PresentationTime = 0;
  uint32 StreamNum;
  U32_WMC nReadPacketSize=0;

  if( (streamType!=Audio_WMC) || (!buf) || (!size) || (!pOutDataSize) ||(!pnSamples) )
  {
    return WMADec_Failed;
  }

  *pOutDataSize = 0;
  *pnSamples = 0;
  nRet = GetStreamNumForID(&StreamNum, id);

  if(nRet == WMCDec_Succeeded)
  {
    int sizeOfSamples = 0;
    /* loop till you decode some sample of desired stream */
    while( (nRet==WMCDec_Succeeded) && (m_nDecodedDataSize[StreamNum]==0) )
    {
      nDecodedDataSize = 0;
      nRet = WMCDecDecodeData (m_hASFDecoder, &nStreamIdReady, &nDecodedDataSize, (-1));
      if(nDecodedDataSize)
      {
        uint32 localStreamNum;
        GetStreamNumForID(&localStreamNum, nStreamIdReady);
        m_nDecodedDataSize[localStreamNum] = nDecodedDataSize;
      }
    }
    //m_nDecodedDataSize[StreamNum] is number of PCM samples available for retrieval.

    /* if we have samples for this track */
    if(m_nDecodedDataSize[StreamNum])
    {
      //Calculate the MIN buffer size we need before pulling in PCM samples.
      U32_WMC minBufSizeNeeded =
        m_nDecodedDataSize[StreamNum] * m_sWAVEFormatEx.Format.nChannels * m_sWAVEFormatEx.Format.wBitsPerSample/8;

      //Make sure we have sufficient buffer.
      if( minBufSizeNeeded > size)
      {
        //Buffer size needed > what's being passed in.
        //Return failure and print error message.
        //Hopefully caller will allocate bigger buffer in next call.
        m_nLargestFrame[StreamNum] = minBufSizeNeeded;
        QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "ASF-GetAudioDecodedSamples minBufSizeNeeded %d > size %d",
                      minBufSizeNeeded,size);
        return WMCDec_BufferTooSmall;
      }
      nRet = WMCDec_Succeeded;

      /* read PCM samples */
      //m_nDecodedDataSize[StreamNum] is number of PCM samples available for retrieval.
      nRet = WMCDecGetAudioOutput( m_hASFDecoder,
                                  (I16_WMC *)buf,
                                  MAX_DECODED_BUFFER_SIZE,
                                   NULL,
                                   m_nDecodedDataSize[StreamNum],
                                   &nReadPacketSize,
                                   &PresentationTime );
      if( nRet==WMCDec_Succeeded )
      {
        //nReadPacketSize gives actual number of samples retrieved.
        //It should be same as m_nDecodedDataSize[StreamNum]
        if(m_nDecodedDataSize[StreamNum] > nReadPacketSize)
        {
               //If we fail to read in all available PCM samples and WMCDecGetAudioOutput returns  WMCDec_Succeeded,
               //most likely it's because BUFFER provided is not sufficient.
               //Rather than ASSERT, exit gracefully.

            QTV_MSG_PRIO2(QTVDIAG_FILE_OPS,
                    QTVDIAG_PRIO_FATAL,
                    "ASF-GetAudioDecodedSamples could not retrieve PCM samples-BUFFER not large enough!!!#PCM available %d, #Retrieved %d",
                    m_nDecodedDataSize[StreamNum],nReadPacketSize);
              *pOutDataSize = 0;
               return WMCDec_Fail;
          }
        sizeOfSamples = m_nDecodedDataSize[StreamNum] * m_sWAVEFormatEx.Format.nChannels * m_sWAVEFormatEx.Format.wBitsPerSample/8;
      }
    }

    if( nRet==WMCDec_Succeeded )
    {
      *pOutDataSize = sizeOfSamples;
      *pnSamples = nReadPacketSize;
      m_nTotalAudioBytesSent = m_nTotalAudioBytesSent + *pOutDataSize;
      m_nDecodedDataSize[StreamNum] = m_nDecodedDataSize[StreamNum] -nReadPacketSize;

      QTV_MSG_PRIO3(QTVDIAG_FILE_OPS,
                    QTVDIAG_PRIO_HIGH,
                    "ASF-GetAudioDecodedSamples size %d, #samples %d time %d",
                    (*pOutDataSize),(*pnSamples),PresentationTime);

      //Update sample information//

      m_sampleInfo[StreamNum].size = *pOutDataSize;
      m_sampleInfo[StreamNum].sync = 1;

      //1 frame is deocoded at a time.
      m_sampleInfo[StreamNum].num_frames = 1;

      // toal #of samples obtained so far //
      m_sampleInfo[StreamNum].sample = m_sampleInfo[StreamNum].sample++;
      m_sampleInfo[StreamNum].delta = (uint32)(PresentationTime -m_sampleInfo[StreamNum].time);
      m_sampleInfo[StreamNum].time = (uint32)PresentationTime;

      //Need to calculate duration as we used it in BYTE BASED AV SYNC to estimate audio data lead.
      if(m_nASFAudioPacketDuration == 0)
      {
        m_nASFAudioPacketDuration = m_prvSampleInfo[StreamNum].delta;
      }
      else
      {
        m_nASFAudioPacketDuration += m_sampleInfo[StreamNum].delta;
        m_nASFAudioPacketDuration/=2;
      }
    }

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      m_maxPlayableTime[StreamNum] = MAX( m_maxPlayableTime[StreamNum],m_sampleInfo[StreamNum].time );
#endif
  }
  /*if( m_wmStreamingInfo.bAudioValid && m_bStreaming )
  {
    m_wmStreamingInfo.bAudioValid = false;
  }*/

  if (!(m_wmStreamingInfo.bAudioValid && m_bStreaming))
  {
    m_bAudioReposPending = false;
  }
  if(nRet == WMCDec_DecodeComplete)
  {
    //Do not need to send dummy bytes when using ARM decoder for WMA.
    //It's safe to set the following flags so that other pieces of
    //code above 'asffile' will not have to change anything.
    m_bWMADecodeDone = true;
    m_bIsDummyBytesStart = true;
  }
  return nRet;
}
#endif//#ifdef FEATURE_QTV_WMA_PRO_DECODER
/* ======================================================================
FUNCTION:
  ASFFile::GetAudioFrame

DESCRIPTION:
  gets next sample of the audio stream.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 size of sample

SIDE EFFECTS:
  None.
======================================================================*/
tWMCDecStatus ASFFile::GetAudioFrame( uint32 id, tMediaType_WMC streamType,
                                      U8_WMC * buf, U32_WMC size, U32_WMC * pOutDataSize )
{
  U32_WMC nStreamIdReady = 0xff;
  U32_WMC nDecodedDataSize;
  tWMCDecStatus nRet = WMADec_Failed;
  I64_WMC PresentationTime;
  uint32 StreamNum;
  U32_WMC nReadPacketSize=0;
  uint8   cPacketNum = 0, cNumFrm = 0;
  uint16  wNumPrevFrameBitsValue;
  uint32  wNumFrmInPacket;
  int     i;
  uint16 u16Temp;
  uint32 blockAlign = GetFixedAsfAudioPacketSize(id);

  int nFrameSize = 0;
  int nMin = 0;
  int pktsPassed = 0;

  if(streamType!=Audio_WMC)
  {
    return WMADec_Failed;
  }

  *pOutDataSize = 0;
  nRet = GetStreamNumForID(&StreamNum, id);

  if( (nRet==WMCDec_Succeeded) && m_bWMADecodeDone)
  {
    if(m_nWMATotalDummyBytesSent >= WMA_TOTAL_DUMMY_BYTES_TO_SEND)
    {
      /*
      There is no WMA data to decode and we have already sent all the dummy bytes.
       Reset dummy bytes counter and return  WMCDec_DecodeComplete to trigger END of AUDIO.
       We should not Reset m_bIsDummyBytesStart and m_bWMADecodeDone here, as in some cases,
       ASFFILE may finish all dummy bytes but playback is not yet finished. In such case,
       isStartOfDummyBytes will return false;
       */

       m_nWMATotalDummyBytesSent = 0;
       QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"ASF-GetAudioFrame:returning WMCDec_DecodeComplete...");
      return WMCDec_DecodeComplete;
    }
    else /* if(m_nWMATotalDummyBytesSent < WMA_TOTAL_DUMMY_BYTES_TO_SEND) */
    {
      //Keep sending dummy data until m_nWMATotalDummyBytesSent == WMA_TOTAL_DUMMY_BYTES_TO_SEND
      //This is req. to satisfy WMA DSP.
      nMin = ((WMA_TOTAL_DUMMY_BYTES_TO_SEND - m_nWMATotalDummyBytesSent)< size)?
            (WMA_TOTAL_DUMMY_BYTES_TO_SEND - m_nWMATotalDummyBytesSent):size;

      memset(buf, WMA_DUMMY_BYTE, nMin);
      m_nWMATotalDummyBytesSent+=nMin;
      *pOutDataSize = nMin;
      m_nTotalAudioBytesSent = m_nTotalAudioBytesSent + nMin;

      m_sampleInfo[StreamNum].sync = 1;
      m_sampleInfo[StreamNum].size = *pOutDataSize;
      m_sampleInfo[StreamNum].num_frames = 1;

      m_bIsDummyBytesStart = true;
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"ASF-GetAudioFrame:Finished Sending %d DUMMY bytes...",m_nWMATotalDummyBytesSent);

    }
    m_bAudioReposPending = false;
    return WMCDec_Succeeded;
  }

  if(nRet == WMCDec_Succeeded)
  {
    /* loop till you decode some sample of desired stream */
    while( (nRet==WMCDec_Succeeded) && (m_nDecodedDataSize[StreamNum]==0) )
    {
      nDecodedDataSize = 0;
      nRet = WMCDecDecodeData (m_hASFDecoder, &nStreamIdReady, &nDecodedDataSize, (-1));
      if(nRet == WMCDec_DRMFail)
      {
        return nRet;
      }
      if(nDecodedDataSize)
      {
        uint32 localStreamNum;
        GetStreamNumForID(&localStreamNum, nStreamIdReady);
        m_nDecodedDataSize[localStreamNum] = nDecodedDataSize;
      }
      if ( m_bStreaming && (nRet != WMCDec_Succeeded) && (m_dataSource != NULL) &&
           m_dataSource->isEOS())
      {
        nRet=WMCDec_DecodeComplete;
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                     "ASF-GetAudioFrame:Detected EOS in data source...");
      }
    }

    if(nRet==WMCDec_DecodeComplete)
    {
      //Data from the last ASF packet will be in buffer, so retrieve it before starting dummy bytes.
      if(m_bBufferValid)
      {
        memmove(buf,m_pasfDataBuffer,m_nCntCurBytes);
        nFrameSize = nFrameSize + m_nCntCurBytes;
        m_nTotalAudioBytesSent += m_nCntCurBytes;
        m_bBufferValid = false;

        //Get the TS from the last buffered Audio Data packet, which is retrieved when we get WMCDec_DecodeComplete for the first time.
        m_sampleInfo[StreamNum].time = m_prvSampleInfo[StreamNum].time;

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
       m_maxPlayableTime[StreamNum] = MAX( m_maxPlayableTime[StreamNum],m_sampleInfo[StreamNum].time );
#endif
        //Get number of frames from last buffered Audio Data packet, which is retrieved when we get WMCDec_DecodeComplete for the first time.
        m_sampleInfo[StreamNum].num_frames = m_prvSampleInfo[StreamNum].num_frames;
      }

      //End of audio media is reached, but WMA DSP req. dummy bytes.
      //Make sure we write frame header on word boundary with length = 0.
      if((m_nTotalAudioBytesSent) % 2 )
      {
        buf[nFrameSize] = 0;
        nFrameSize++;
      }
      //Prepare frame header with length = 0. This is done to satisfy our DSP.
      u16Temp = htons((uint16)0xfff0);  /* sync + lost bits */
      buf[nFrameSize] = *((uint8*)&u16Temp);
      buf[nFrameSize+1] = *(((uint8*)&u16Temp)+1);
      nFrameSize = nFrameSize + 2;

      /* size of packet in words */
      buf[nFrameSize] = 0;
      buf[nFrameSize+1] = 0;
      nFrameSize = nFrameSize + 2;

      /* number of frames in this packet */
      buf[nFrameSize] = 0;
      nFrameSize = nFrameSize + 1;

      /* start bit */
      buf[nFrameSize] = 0;
      nFrameSize = nFrameSize + 1;

      *pOutDataSize = *pOutDataSize + nFrameSize;
      m_nTotalAudioBytesSent = m_nTotalAudioBytesSent + *pOutDataSize;

      m_bWMADecodeDone = true;
      m_nWMATotalDummyBytesSent = 0;

      m_sampleInfo[StreamNum].sync = 1;
      m_sampleInfo[StreamNum].size = *pOutDataSize;
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"ASF-GetAudioFrame:Started DUMMY HEADER...");
      return WMCDec_Succeeded;
    }

    /* if we have more samples for this track */
    if(m_nDecodedDataSize[StreamNum])
    {
      /* if we don't have enough buffer, save largest frame size and return error.
         To satisfy our DSP, for low bit rate audio (m_bAllowWmaPackets==FALSE), we need to
         put frame header after every frame (for higher bit rate, we put after every packet).
         We can possibly have number of frames = m_nDecodedDataSize[StreamNum]/blockAlign, so
         make sure we have enough bytes for packet and frame headers */
      if( (m_nDecodedDataSize[StreamNum]+(m_nDecodedDataSize[StreamNum]/blockAlign)*10) > size)
      {
        m_nLargestFrame[StreamNum] = MAX(m_nDecodedDataSize[StreamNum]+(m_nDecodedDataSize[StreamNum]/blockAlign)*10,
                                         m_nLargestFrame[StreamNum]);
        if(m_pAudioPacketBuffer)
          QTV_Free(m_pAudioPacketBuffer);
        m_pAudioPacketBuffer = (uint8*)QTV_Malloc(m_nLargestFrame[StreamNum]);
        if (m_pAudioPacketBuffer == NULL)
        {
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, 
                        "m_pAudioPacketBuffer malloc failed size = %d",
                        m_nLargestFrame[StreamNum]);
          return WMCDec_BadMemory;
        }
        m_nAudioPacketBufferSize = m_nLargestFrame[StreamNum];
        return WMCDec_BufferTooSmall;
      }

      /* read the sample */
      nRet = WMCDecGetAudioOutput( m_hASFDecoder, (I16_WMC *)m_pAudioPacketBuffer, m_nAudioPacketBufferSize, NULL,
                                   m_nAudioPacketBufferSize, &nReadPacketSize, &PresentationTime );
      /* make sure we have not overwritten m_pAudioPacketBuffer */
      if (nReadPacketSize > m_nAudioPacketBufferSize)
      {
        QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, 
                      "nReadPacketSize = %d is > m_nAudioPacketBufferSize = %d",
                      nReadPacketSize,
                      m_nAudioPacketBufferSize);
        return WMCDec_Fail;
      }
      m_nDecodedDataSize[StreamNum] = m_nDecodedDataSize[StreamNum] - nReadPacketSize;
      /* we should have read all the decoded data in one shot */
      if (m_nDecodedDataSize[StreamNum] != 0)
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, 
          "m_nDecodedDataSize[StreamNum] is  ! 0 !!");
        return WMCDec_Fail;
      }
    }

    if( (nRet==WMCDec_Succeeded) && nReadPacketSize )
    {
      /* pre-parse audio payload (packet level decoding) */

      if(m_bAllowWmaPackets)
      {
        int nSrcBitOffset = 0, nDestOffset = 0, nPrevBytes, nPrevExtraBits;

        int nBlockAlignBoundaryCount = nReadPacketSize/blockAlign;
        int nCurrentReadIndex = 0;
        cNumFrm = 0;
        wNumFrmInPacket = 0;
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
        int nSpillOverBits = 0;
        uint8 two_bits_in_header = 0;
#endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
        int bits_left = 0;
        uint8 seekable_frame_in_packet = 0;
        uint8 spliced_packet = 0;
        uint32 remainingBytes = 0;

        if (m_nAsfDataBufferSize < (2 * nReadPacketSize))
        {
          QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, 
                        "m_nAsfDataBufferSize is (%d)< 2 * nReadPacketSize(%d)",
                        m_nAsfDataBufferSize,
                        (2 * nReadPacketSize));
          return WMCDec_Fail;
        }

        for(int itr = 0;itr < nBlockAlignBoundaryCount ; itr++)
        {
          nFrameSize = 0;
          if(!m_isDSPWMA_pro)
          {
            /* first 4 bits are number of frames in packet */
            cNumFrm = (m_pAudioPacketBuffer[nCurrentReadIndex] & 0x0F);
            wNumFrmInPacket = wNumFrmInPacket + cNumFrm;

            /* next 4 bits are packet number */
            cPacketNum = (m_pAudioPacketBuffer[nCurrentReadIndex] & 0xF0) >> 4;
          }
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
            else
          {
            cNumFrm = WMAPRO_FRAME_IN_PACKET;
         /* first 4 bits are number of packet number */
            cPacketNum = (m_pAudioPacketBuffer[nCurrentReadIndex] & 0xF0) >> 4;
            /* packet_number - 4 bits */
            /* seekable_frame_in_packet - 1 bit */
            /* spliced_packet - 1 bit */
            /* seek_offset_in_bits  - */
            seekable_frame_in_packet = (m_pAudioPacketBuffer[nCurrentReadIndex] & 0x08) >> 3;
            spliced_packet           = (m_pAudioPacketBuffer[nCurrentReadIndex] & 0x04) >> 2;
            two_bits_in_header       = (m_pAudioPacketBuffer[nCurrentReadIndex] & 0x03);
            nSpillOverBits           = two_bits_in_header;
          }
#endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
          /* next m_nWmaNumPrevFrameBits bits have number of bits of previous frame in packet */
          wNumPrevFrameBitsValue = 0;

#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
          /* In case of WMA_PRO this m_nWmaNumPrevFrameBits calculation
             logic would be different */
          if(m_isDSPWMA_pro)
          {
            if (nReadPacketSize == 0)
            {
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "nReadPacketSize is 0!!");
              return WMCDec_BadData;
            }
            m_nWmaNumPrevFrameBits =  LOG2(nReadPacketSize * 8 ) + 1;
          }
#endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
          for(i=0; i<(int)m_nWmaNumPrevFrameBits/8; i++)
          {
            wNumPrevFrameBitsValue = wNumPrevFrameBitsValue << 8;
            wNumPrevFrameBitsValue = wNumPrevFrameBitsValue | (m_pAudioPacketBuffer[nCurrentReadIndex+1+i]);
          }

          bits_left = (m_nWmaNumPrevFrameBits%8);
          if(m_nWmaNumPrevFrameBits%8)
          {
            uint8 cTemp = m_pAudioPacketBuffer[nCurrentReadIndex+1+i] >> (8 - bits_left);
            wNumPrevFrameBitsValue = wNumPrevFrameBitsValue << bits_left;
            wNumPrevFrameBitsValue = wNumPrevFrameBitsValue | cTemp;
          }
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
          if(m_isDSPWMA_pro)
          {
            /* Here 2 is from 6 bits in the first byte go to the header
               and remaining 2 bits contribute to the spillover calculation */
            nSpillOverBits <<= (m_nWmaNumPrevFrameBits - 2);
            wNumPrevFrameBitsValue >>= 2;
            wNumPrevFrameBitsValue |=nSpillOverBits;
          }
#endif  /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
          if(!m_isDSPWMA_pro)
          {
            nSrcBitOffset = m_nWmaNumPrevFrameBits + 8 + nSrcBitOffset;
          }
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
          else
          {
            nSrcBitOffset = m_nWmaNumPrevFrameBits + 6 + nSrcBitOffset;
          }
#endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
          /* first copy previous frame bits */
          nPrevBytes = wNumPrevFrameBitsValue/8;
          nPrevExtraBits = wNumPrevFrameBitsValue%8;

          ///////////////Determine if there is a packet drop/////////////////
          pktsPassed = (int)(cPacketNum - m_nPreviousAudioPacketNum);
          if(!m_nTotalAudioBytesSent)
          {
           m_nPreviousAudioPacketNum = cPacketNum;
          }
          else
          {
            if( ((pktsPassed < 0) && (pktsPassed != -15)) ||
                                               (pktsPassed > 1) ||
                                                        //( (pktsPassed == 1) && ((PresentationTime - m_prvSampleInfo[StreamNum].time) > (2*m_nASFAudioPacketDuration) ) )||
                ((!pktsPassed) && (m_prvSampleInfo[StreamNum].time != PresentationTime)) )
            {
              m_bIsPacketDrop = true;
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS,QTVDIAG_PRIO_HIGH,"DETECTED PACKET LOSS:: m_nPreviousAudioPacketNum %d curr pkt.no %ld",m_nPreviousAudioPacketNum, cPacketNum);

              if(!m_bStreaming)
                QTV_MSG_PRIO2(QTVDIAG_FILE_OPS,QTVDIAG_PRIO_HIGH,"PLEASE CHECK AS DETECTED PACKET LOSS IN LOCAL PLAYBACK.. m_nPreviousAudioPacketNum %d curr pkt.no %ld",m_nPreviousAudioPacketNum, cPacketNum);
            }
            else
            {
              m_bIsPacketDrop = false;
            }
          }

          /* Detect and Reject Duplicate packets */
          if( (!m_bAudioReposPending) && (PresentationTime > 0) )
          {
            if( (!pktsPassed) && (m_prvSampleInfo[StreamNum].time == PresentationTime) )
            {
              QTV_MSG_PRIO3(QTVDIAG_FILE_OPS,QTVDIAG_PRIO_HIGH,"DETECTED AND REJECTED DUPLICATE PACKET.. Current Packet Time %d,Previous Packet Time %d, curr pkt.no %ld",PresentationTime,m_prvSampleInfo[StreamNum].time,cPacketNum);

              if(!m_bStreaming)
              {
                QTV_MSG_PRIO3(QTVDIAG_FILE_OPS,QTVDIAG_PRIO_HIGH,"PLEASE CHECK, AS DETECTED AND REJECTED DUPLICATE PACKET IN LOCAL PLAYBACK.. Current Packet Time %d,Previous Packet Time %d, curr pkt.no %ld",PresentationTime,m_prvSampleInfo[StreamNum].time, cPacketNum);
              }

              *pOutDataSize = 0;
              return WMCDec_Succeeded;
            }

            if( m_prvSampleInfo[StreamNum].time > PresentationTime )
            {
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS,QTVDIAG_PRIO_HIGH,"WIERED TIME STAMP, REJECTING PACKET.. Current Packet Time %d,Previous Packet Time %d",PresentationTime,m_prvSampleInfo[StreamNum].time );

              if(!m_bStreaming)
              {
                QTV_MSG_PRIO2(QTVDIAG_FILE_OPS,QTVDIAG_PRIO_HIGH,"PLEASE CHECK, AS WIERED TIME STAMP, REJECTING PACKET IN LOCAL PLAYBACK.. Current Packet Time %d,Previous Packet Time %d",PresentationTime,m_prvSampleInfo[StreamNum].time );
              }

              *pOutDataSize = 0;
              return WMCDec_Succeeded;
            }

          }

          ///////////////////////////////////////////////////////////////////////

          //If bBufferValid is true, pull the buffered data from previous ASF packet.
          if(m_bBufferValid)
          {
            memmove(buf+(*pOutDataSize),m_pasfDataBuffer,m_nCntCurBytes);
            nFrameSize = nFrameSize + m_nCntCurBytes;
            nDestOffset = nDestOffset + m_nCntCurBytes;
            //Don't make m_bBufferValid to false here;We might have to subtract number of frames
            //if buffer was valid and packet drop is detected.
            //m_bBufferValid = false;
          }
          for(i=0; i<nPrevBytes; i++)
          {
            //Don't copy prv frame bits if audio reposition is pending
            if( (!m_bAudioReposPending)&&(!m_bIsPacketDrop) )
            {
              GetByteFromBitStream(buf+nDestOffset, m_pAudioPacketBuffer, nSrcBitOffset, 8);
              nDestOffset = nDestOffset + 1;
              nFrameSize++;
            }
            else if(m_bIsPacketDrop && m_bBufferValid)
            {
              //decrement number of frames from previous ASF packet by 1 and break;
              *(buf+4) = *(buf+4) - 1;
              QTV_MSG_PRIO1(QTVDIAG_FILE_OPS,QTVDIAG_PRIO_HIGH,"current #frames %ld,decrementing it by 1 as packet drop is detected",*(buf+4));
              nSrcBitOffset += (nPrevBytes*8);
              break;
            }
            nSrcBitOffset = nSrcBitOffset + 8;
          }

          if(nPrevExtraBits)
          {
            //Don't copy prv frame bits if audio reposition is pending

            /* we can still read 8 bits and only nPrevExtraBits will be used by DSP */
            if( (!m_bAudioReposPending)&&(!m_bIsPacketDrop) )
            {
              GetByteFromBitStream(buf+nDestOffset, m_pAudioPacketBuffer, nSrcBitOffset, 8);
              nDestOffset = nDestOffset + 1;
              nFrameSize++;
            }
            else if(m_bIsPacketDrop && m_bBufferValid)
            {
              //decrement number of frames from previous ASF packet by 1 and break;
              ///???
            }
            nSrcBitOffset = nSrcBitOffset + nPrevExtraBits;
          }
            m_bBufferValid = false;

          /* we have to start DSP frame header from WORD boundary */
          if( (m_nTotalAudioBytesSent+nFrameSize) % 2 )
          {
            buf[nDestOffset] = 0;
            nDestOffset++;
            nFrameSize++;
          }
          m_nCntPrvBytes = nFrameSize;

          /* now prepare frame header in next four destination bytes */
#if 0
          AudioFrameHeaderIndex = nDestOffset;
#endif

          //update *pOutDataSize.
          //This gives number of bytes that form one complete audio frame,including spill over data.
          *pOutDataSize = *pOutDataSize + m_nCntPrvBytes;

          //if number of frames > 0 , then only append frame header..
          if(cNumFrm)
          {
            //Push everything into asfDataBuffer. This will be pulled when we start with next packet.
            u16Temp = htons((uint16)0xfff0);  /* sync + lost bits */
            m_pasfDataBuffer[0] = *((uint8*)&u16Temp);
            m_pasfDataBuffer[1] = *(((uint8*)&u16Temp)+1);

            nFrameSize = nFrameSize + 2;

            /* size of frame in words with extra byte to make it word aligned */
            u16Temp = htons((blockAlign+1)/2);  /* size of packet in words */

            m_pasfDataBuffer[2] = *((uint8*)&u16Temp);
            m_pasfDataBuffer[3] = *(((uint8*)&u16Temp)+1);
            nFrameSize = nFrameSize + 2;
            if(!m_isDSPWMA_pro)
            {
            *(m_pasfDataBuffer+4) = cNumFrm;  /* number of frames in this packet */
            }
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
            else
            {
              *(m_pasfDataBuffer+4) = WMAPRO_FRAME_IN_PACKET; /* number of frames in this packet */
            }
#endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */

            nFrameSize = nFrameSize + 1;
            *(m_pasfDataBuffer+5) = (nSrcBitOffset%8)<<4;  /* start bit */

            if(seekable_frame_in_packet)
                 m_pasfDataBuffer[5] |= SEEKABLE_BIT;

            if(spliced_packet)
                 m_pasfDataBuffer[5] |= SPLICE_BIT;

            //This seems to be redundant..
            //nFrameSize = nFrameSize + 1;

            //BE CAREFUL!!!
            //Following statement will introduce
            //one byte error for every
            //ASF packet whenever blockAlign is not same
            //as virtual packet length.

            //nDestOffset = nDestOffset + 1;



            nFrameSize = nFrameSize + 1;
            uint32 padBits = 0;
            if(m_isDSPWMA_pro)
            {
              padBits = 6;
            }
            else
            {
              padBits = 8;
            }

            /* now copy rest of the data */

            //(blockAlign - ((m_nWmaNumPrevFrameBits+8+(nPrevBytes*8) + nPrevExtraBits)/8)
            //gives number of bytes to copy.
            if ( (blockAlign <=((m_nWmaNumPrevFrameBits+padBits+nPrevExtraBits)/8 +nPrevBytes)) )
            {
              QTV_MSG_PRIO(QTVDIAG_FILE_OPS,QTVDIAG_PRIO_HIGH,"blockAlign <=((m_nWmaNumPrevFrameBits+6+nPrevExtraBits)/8 +nPrevBytes)");
              remainingBytes = 0;
            }
            else
            {
              remainingBytes = blockAlign - ((m_nWmaNumPrevFrameBits+padBits+(nPrevBytes*8) + nPrevExtraBits)/8);
            }

            memmove(m_pasfDataBuffer+(WMA_DSP_PACKET_HEADER_LEN), m_pAudioPacketBuffer+(nSrcBitOffset/8),
                                                remainingBytes );


            if(remainingBytes)
            {
              m_bBufferValid = true;
              //nFrameSize is total number of bytes copied including previous frame bytes if available,
              //and current frame header bytes.
              m_nCntCurBytes = (WMA_DSP_PACKET_HEADER_LEN) + remainingBytes;
              m_nTotalAudioBytesSent = m_nTotalAudioBytesSent + *pOutDataSize;
            }
            else
            {
              m_bBufferValid = false;
            }
          }
          else
          {
            m_nTotalAudioBytesSent = m_nTotalAudioBytesSent + nFrameSize;
            m_bBufferValid = false;
          }

          //Update nSrcBitOffset.
          //(nCurrentReadIndex+blockAlign-(nSrcBitOffset/8)) gives number of bytes copied
          //(nSrcBitOffset%8) if non zero, gives extra bits that are counted twice, so we need to subtract extra bits.
          nSrcBitOffset = nSrcBitOffset + (8 * (nCurrentReadIndex+blockAlign-(nSrcBitOffset/8)) )- (nSrcBitOffset%8);

          //Done with reading/processing blockAlign bytes, update the read index.
          nCurrentReadIndex += blockAlign;

          if( nBlockAlignBoundaryCount > 1)
           {
            QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"ASF-GetAudioFrame NumFrm=%d, pktNum=%d Time=%d", wNumFrmInPacket, cPacketNum, PresentationTime);
            UpdateSamplesInformation(cPacketNum,cNumFrm,StreamNum,(uint32)PresentationTime,*pOutDataSize,wNumFrmInPacket);
           }//if( nBlockAlignBoundaryCount > 1)
        }
      }
      else
      {
        int nSrcOffset, nDestOffset;

        /* if m_bAllowWmaPackets is FALSE, then we have complete multiple audio frames in one ASF packet.
           We can get number of frames = nReadPacketSize/blockAlign. To make sizes of these packet fixed
           (== blockAlign), MS encoder adds padding at the end of these audio frames. Our current DSP is
           not able to skip these padding without SYNC marker. So we need to add SYNC marker after every
           frame */

        cPacketNum = (uint8)m_sampleInfo[StreamNum].sample + 1;
        wNumFrmInPacket = cNumFrm = (uint8)(nReadPacketSize/blockAlign);
        nSrcOffset = 0;
        nDestOffset = 0;
        for(i=0; i<cNumFrm; i++)
        {
          /* now prepare frame header in next four destination bytes */
          u16Temp = htons((uint16)0xfff0);  /* sync + lost bits */
          buf[nDestOffset] = *((uint8*)&u16Temp);
          buf[nDestOffset+1] = *(((uint8*)&u16Temp)+1);
          nDestOffset = nDestOffset + 2;

          /* size of frame in words with extra byte to make it word aligned */
          u16Temp = htons( (blockAlign+1) / 2);

          buf[nDestOffset] = *((uint8*)&u16Temp);
          buf[nDestOffset+1] = *(((uint8*)&u16Temp)+1);
          nDestOffset = nDestOffset + 2;

          *(buf+nDestOffset) = 1;  /* number of frames for this header is one */
          nDestOffset = nDestOffset + 1;
          *(buf+nDestOffset) = 0;  /* start bit offset is zero */
          nDestOffset = nDestOffset + 1;

          /* now copy one frame data */
          memcpy(buf+nDestOffset, m_pAudioPacketBuffer+nSrcOffset, blockAlign);

          nSrcOffset = nSrcOffset + blockAlign;
          nDestOffset = nDestOffset + blockAlign;

          /*one extra byte if Block Align is odd to make it even */
          if(blockAlign%2)
          {
            buf[nDestOffset] = 0;
            nDestOffset++;
          }

          *pOutDataSize = *pOutDataSize + (blockAlign+(blockAlign%2))+ 6/*SYNC header length*/;
          m_nTotalAudioBytesSent = m_nTotalAudioBytesSent + blockAlign + (blockAlign%2) + 6;
        }
      }
      if((!m_bAllowWmaPackets)||((nReadPacketSize/blockAlign) == 1))
      {
        UpdateSamplesInformation(cPacketNum,cNumFrm,StreamNum,(uint32)PresentationTime,*pOutDataSize,wNumFrmInPacket);
        QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"ASF-GetAudioFrame NumFrm=%d, pktNum=%d Time=%d", wNumFrmInPacket, cPacketNum, PresentationTime);
      }//if( (nReadPacketSize/blockAlign) == 1)

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      m_maxPlayableTime[StreamNum] = MAX( m_maxPlayableTime[StreamNum],m_sampleInfo[StreamNum].time );
#endif

      //If there is no spill over, then simply copy previous sampleinfo into m_sampleInfo
      if(!m_bAllowWmaPackets)
        memcpy(&m_sampleInfo,&m_prvSampleInfo,sizeof(m_sampleInfo));

      QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"ASF-GetAudioFrame NumFrm=%d, pktNum=%d Time=%d", wNumFrmInPacket, cPacketNum, PresentationTime);

    }
  } /* end of if (GetStreamNumForID == Success) */
  if( m_wmStreamingInfo.bAudioValid && m_bStreaming )
  {
    m_wmStreamingInfo.bAudioValid = false;
  }

  if (!(m_wmStreamingInfo.bAudioValid && m_bStreaming))
    m_bAudioReposPending = false;
  return nRet;
}
/* ======================================================================
FUNCTION:
  ASFFile::GetStandardAudioFrame

DESCRIPTION:
  gets next sample of the audio stream.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 size of sample

SIDE EFFECTS:
  None.
======================================================================*/
tWMCDecStatus  ASFFile::GetStandardAudioFrame( uint32 id, tMediaType_WMC streamType,
                                               U8_WMC * buf, U32_WMC size, 
                                               U32_WMC * pOutDataSize )
{
  tWMCDecStatus nRet = WMADec_Failed;
  uint32 StreamNum;
  U32_WMC nDecodedDataSize; 
  U32_WMC nStreamIdReady = 0xff;   
  I64_WMC PresentationTime;  
  U32_WMC nReadPacketSize=0;

  if(streamType!=Audio_WMC)
  {
    nRet = WMADec_Failed;
  }
  else
  {
    *pOutDataSize = 0;
    nRet = GetStreamNumForID(&StreamNum, id);

    if(nRet == WMCDec_Succeeded)
    {
      /* loop till you decode some sample of desired stream */
      while( (nRet==WMCDec_Succeeded) && (m_nDecodedDataSize[StreamNum]==0) )
      {
        nDecodedDataSize = 0;      
        nRet = WMCDecDecodeData (m_hASFDecoder, &nStreamIdReady, &nDecodedDataSize, (-1));      
        if(nDecodedDataSize)
        {
          uint32 localStreamNum;
          GetStreamNumForID(&localStreamNum, nStreamIdReady);
          m_nDecodedDataSize[localStreamNum] = nDecodedDataSize;
        }
        if ( m_bStreaming && (nRet != WMCDec_Succeeded) && (m_dataSource != NULL) &&
             m_dataSource->isEOS())
        {
          nRet = WMCDec_DecodeComplete;
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                       "ASF-GetStandardAudioFrame:Detected EOS in data source...");
        }
      }
      if(nRet==WMCDec_DecodeComplete)
      {             
        //End of audio media is reached
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                     "ASF-GetStandardAudioFrame:Detected end of WMA track!!!");
      }
      /* if we have more samples for this track */
      if(m_nDecodedDataSize[StreamNum])
      {
        /*Make sure we have large enough buffer to copy the payload*/
        if(m_nDecodedDataSize[StreamNum] > size)
        {
          m_nLargestFrame[StreamNum] = m_nDecodedDataSize[StreamNum];         
          return WMCDec_BufferTooSmall;
        }      
        /* read the sample */
        nRet = WMCDecGetAudioOutput( m_hASFDecoder, (I16_WMC*)buf, size, NULL,
                                     size, &nReadPacketSize, &PresentationTime );
        /* make sure we have not overwritten m_pAudioPacketBuffer */
        //ASSERT(nReadPacketSize <= m_nAudioPacketBufferSize); //to do surbhi
        m_nDecodedDataSize[StreamNum] = m_nDecodedDataSize[StreamNum] - nReadPacketSize;
        /* we should have read all the decoded data in one shot */
        //ASSERT(m_nDecodedDataSize[StreamNum] == 0); //to do surbhi
        m_sampleInfo[StreamNum].time = (uint32)PresentationTime;
        m_sampleInfo[StreamNum].sample++;
        m_sampleInfo[StreamNum].size = nReadPacketSize;
        m_sampleInfo[StreamNum].sync = 1;
        *pOutDataSize = nReadPacketSize;
        m_sampleInfo[StreamNum].delta = ((WMFDecoderEx *)m_hASFDecoder)->tHeaderInfo.ppex.wDuration;
      }
    }//if(nRet == WMCDec_Succeeded)
    if( m_wmStreamingInfo.bAudioValid && m_bStreaming )
    {
      m_wmStreamingInfo.bAudioValid = false;
    }
    if (!(m_wmStreamingInfo.bAudioValid && m_bStreaming))
    {
      m_bAudioReposPending = false;
    }
  }//end of else of if(streamType!=Audio_WMC)
  return nRet;
}
/* ======================================================================
FUNCTION:
  ASFFile::getMediaTimestampForCurrentSample

DESCRIPTION:
  gets time stamp of current sample of the track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 time stamp in track time scale unit

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getMediaTimestampForCurrentSample(uint32 id)
{
  uint32 StreamNum;
  if(GetStreamNumForID(&StreamNum, id) == WMCDec_Succeeded)
  {
    return m_sampleInfo[StreamNum].time;
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::getMediaTimestampDeltaForCurrentSample

DESCRIPTION:
  gets duration of current sample of the track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 time stamp in track time scale unit

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getMediaTimestampDeltaForCurrentSample(uint32 id)
{
  uint32 StreamNum;
  if(GetStreamNumForID(&StreamNum, id) == WMCDec_Succeeded)
  {
    return m_sampleInfo[StreamNum].delta;
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::resetPlayback

DESCRIPTION:
  resets the playback time/sample to zero.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
void ASFFile::resetPlayback()
{
}

/* ======================================================================
FUNCTION:
  ASFFile::resetPlayback

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
uint32 ASFFile::resetPlayback(  uint32 repos_time, uint32 id, bool /*bSetToSyncSample*/,
                                bool *bError, uint32 currentPosTimeStamp)
{
  uint32 streamNum;
  *bError = FALSE;


  if( GetStreamNumForID(&streamNum, id) != WMCDec_Succeeded )
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "resetPlayback, unknown track id = %d", id);
    *bError = TRUE;
    return 0;
  }

  uint32 modTS = repos_time; // initial value

  //While repositioning audio, isRewind will always be FALSE, as currentPosTimeStamp passed
  //to ASFFile::resetPlayback is always 0;This is OK for video as currentPosTimeStamp passed is  correct.

  //Determine if user is attempting RW or FWD.

  boolean isRewind = (currentPosTimeStamp > repos_time)?1:0;

  if ( SetTimeStampedSample (id, repos_time, &modTS, isRewind) )
  {
     //This flag when true prevents copying previous frame bits.
    tMediaType_WMC Type;
    if( (GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded) &&
        (Type==Audio_WMC) )
    {
      m_bAudioReposPending = true;
    }
    if( (GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded) &&
        (Type==Video_WMC) &&
        (repos_time>0) )
    {
      m_bVideoReposPending = true;
    }


    //When repositioning, as a resule of flush, start all over to keep track of audio bytes sent.
    m_nTotalAudioBytesSent = 0;

    //reset variables to record packet drops.
    m_nPreviousAudioPacketNum = 0;
    m_bIsPacketDrop = false;

    /*
      m_bWMADecodeDone is TRUE then ASFFILE has finished all the dummy bytes.
      In such case, if it's RW, reset all the variables as there will be some data to decode.
      Otherwise, if m_bWMADecodeDone is TRUE and user is attempting FWD, it means seek was done to the EOF.

      if m_bWMADecodeDone is FALSE, reset everything as there will be data to decode.
    */

    if ( (!m_bWMADecodeDone) ||
         (m_bWMADecodeDone && isRewind) )
    {
      m_nWMATotalDummyBytesSent = 0;
      m_bIsDummyBytesStart = false;
      m_bWMADecodeDone = false;
      m_nWMATotalDummyBytesSent = 0;
      m_nCntCurBytes = 0;
      m_nCntPrvBytes = 0;
      m_bBufferValid = false;
    }
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Reposition failed for track id = %d", id);
    // this is to avoid unnecessary disturbance, when repositioning can not be done
    *bError = TRUE;
    modTS = (uint32) ( (float)m_sampleInfo[streamNum].time / (float)ASF_STREAM_TIME_SCALE * (float)TIMESCALE_BASE);
  }
  return modTS;
}
/* ======================================================================
 FUNCTION:
   ASFFile::updateASFStreamingRepositioningInfo()

 DESCRIPTION:
   Records the last timestamp of the audio and video frame parsed inside WM parser.

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 void  ASFFile::updateASFStreamingRepositioningInfo(U32_WMC reposTime,int id)
 {
  uint32 StreamNum;
  tMediaType_WMC Type;
  tWMCDecStatus retStatus;

  if(m_bStreaming)
  {
    if(GetMediaTypeForStreamID(&Type, id) != WMCDec_Succeeded)
    {
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Could not determine Media type for trackId %d",id);
      return;
    }

    // Even if we haven't output anything yet, allow the operation to
    // succeed, so that if we start at a non-zero position in the
    // clip, we are able to complete the first frame.
    //if(WMCDecHasGivenAnyOutput(m_hASFDecoder,Type) == WMCDec_Succeeded)
    {
      if(GetStreamNumForID(&StreamNum, id) == WMCDec_Succeeded)
      {
        switch(Type)
        {
          case Video_WMC:
            m_wmStreamingInfo.bVideoValid = true;
            retStatus = WMCDecResetToFirstPacket(m_hASFDecoder);
            if(retStatus == WMCDec_Succeeded)
            {
          //This assumes that first video frame is key frame. if come across a scenario
          //where first frame is not key frame (??) or server doesn't start from key frame
          //uncomment following if block.
              if(reposTime > 0)
                m_bVideoReposPending = true;
            }
            break;

          case Audio_WMC:
            m_wmStreamingInfo.bAudioValid = true;
               retStatus = WMCDecResetToFirstPacket(m_hASFDecoder);
               if(retStatus == WMCDec_Succeeded)
               {
                 m_bAudioReposPending = true;

              //When repositioning, as a resule of flush,
              //start from start to keep track of audio bytes sent.
              m_nTotalAudioBytesSent = 0;

              //Reset m_bWMADecodeDone and m_nWMATotalDummyBytesSent sent,
              //as after reposition, there may be some data available to decode.
              m_bWMADecodeDone = false;
              m_bIsDummyBytesStart = false;

           //reset variables to record packet drops.
           m_nPreviousAudioPacketNum = 0;
           m_bIsPacketDrop = false;

           //Reset m_nWMATotalDummyBytesSent sent, as after reposition,
           //there may be some data available to decode.
           m_nCntCurBytes = 0;
           m_nCntPrvBytes = 0;
           m_bBufferValid = false;
                }
            break;
          default:
            QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Unknown track type in updateASFStreamingRepositioningInfo..");
          break;
        }
      }
    }
  }
 }

/* ======================================================================
FUNCTION:
  ASFFile::resetPlayback

DESCRIPTION:
  resets the playback time to zero for a track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
bool ASFFile::resetMediaPlayback(uint32 /*id*/)
{
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::randomAccessDenied

DESCRIPTION:
  gets if repositioning is allowed or not.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 ASFFile::randomAccessDenied()
{
  uint8 nRet = FALSE; /* by default random access is allowed */
  U32_WMC dwFlags = 0;

  if(m_hASFDecoder)
  {
    dwFlags = ((WMFDecoderEx *)m_hASFDecoder)->tHeaderInfo.dwFlags;
    /* Check if the file level random acccess is allowed. i.e."Seekable" flag is set or not */
    if( (dwFlags & 0x2) == 0 )
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"Reposition is not allowed for this clip, Seekable flag is set to FALSE");
      nRet = TRUE;
    }
  }
  return nRet;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTitle

DESCRIPTION:
  gets title

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING ASFFile::getTitle() const
{
  if(m_pContentDesc && m_pContentDesc->uiTitle_len)
  {
    OSCL_STRING sTitle;
    uint8* pStr = NULL;
    /* Here we are allocating these many bytes because we are coverting this
    unicode string to UTF8 at max we may require three times of the uinicode 
    string length */
    uint32 destLen = (m_pContentDesc->uiTitle_len - 1)* UNICODE_TO_UTF8; 
    pStr = (uint8 *)QTV_Malloc(destLen + 1);    
	if(pStr)
	{
	    ConvertUnicodeToUTF8(pStr, destLen + 1, m_pContentDesc->pchTitle, m_pContentDesc->uiTitle_len -1);    
	    sTitle = (char*)pStr;
	    QTV_Free(pStr);
	    return sTitle;
	}
  }
  return NULL;
}

/* ======================================================================
FUNCTION:
  ASFFile::getAuthor

DESCRIPTION:
  gets Author

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING ASFFile::getAuthor() const
{
  if(m_pContentDesc && m_pContentDesc->uiAuthor_len)
  {
    OSCL_STRING sAuthor;
    uint8* pStr = NULL;
    /* Here we are allocating these many bytes because we are coverting this
    unicode string to UTF8 at max we may require three times of the uinicode 
    string length */
    uint32 destLen = (m_pContentDesc->uiAuthor_len - 1)* UNICODE_TO_UTF8; 
    pStr = (uint8 *)QTV_Malloc(destLen + 1);
	if(pStr)
	{
	    ConvertUnicodeToUTF8(pStr, destLen + 1, m_pContentDesc->pchAuthor, m_pContentDesc->uiAuthor_len -1);    
	    sAuthor = (char*)pStr;
	    QTV_Free(pStr);
	    return sAuthor; 
	}
  }
  return NULL;
}

/* ======================================================================
FUNCTION:
  ASFFile::getDescription

DESCRIPTION:
  gets description

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING ASFFile::getDescription() const
{
  if(m_pContentDesc && m_pContentDesc->uiDescription_len)
  {
    OSCL_STRING sDescription;
    uint8* pStr = NULL;
    /* Here we are allocating these many bytes because we are coverting this
    unicode string to UTF8 at max we may require three times of the uinicode 
    string length */
    uint32 destLen = (m_pContentDesc->uiDescription_len - 1)* UNICODE_TO_UTF8; 
    pStr = (uint8 *)QTV_Malloc(destLen + 1);
	if(pStr)
	{
	    ConvertUnicodeToUTF8(pStr, destLen + 1, m_pContentDesc->pchDescription, m_pContentDesc->uiDescription_len -1);        
	    sDescription = (char*)pStr;
	    QTV_Free(pStr);
	    return sDescription;
	}
  }
  return NULL;
}

/* ======================================================================
FUNCTION:
  ASFFile::getRating

DESCRIPTION:
  gets rating

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING ASFFile::getRating() const
{
  if(m_pContentDesc && m_pContentDesc->uiRating_len)
  {
    OSCL_STRING sRating;
    uint8* pStr = NULL;
    /* Here we are allocating these many bytes because we are coverting this
    unicode string to UTF8 at max we may require three times of the uinicode 
    string length */
    uint32 destLen = (m_pContentDesc->uiRating_len - 1)* UNICODE_TO_UTF8;
	if(pStr)
	{
	    pStr = (uint8 *)QTV_Malloc(destLen + 1);
	    ConvertUnicodeToUTF8(pStr, destLen + 1, m_pContentDesc->pchRating, m_pContentDesc->uiRating_len -1);           
	    sRating = (char*)pStr;
	    QTV_Free(pStr);
	    return sRating;
	}
  }
  return NULL;
}

/* ======================================================================
FUNCTION:
  ASFFile::getCopyright

DESCRIPTION:
  gets Copyright

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING ASFFile::getCopyright() const
{
  if(m_pContentDesc && m_pContentDesc->uiCopyright_len)
  {
    OSCL_STRING sCopyright;
    uint8* pStr = NULL;
    /* Here we are allocating these many bytes because we are coverting this
    unicode string to UTF8 at max we may require three times of the uinicode 
    string length */
    uint32 destLen = (m_pContentDesc->uiCopyright_len - 1)* UNICODE_TO_UTF8; 
    pStr = (uint8 *)QTV_Malloc(destLen + 1);
	if(pStr)
	{
	    ConvertUnicodeToUTF8(pStr, destLen + 1, m_pContentDesc->pchCopyright, m_pContentDesc->uiCopyright_len -1);               
	    sCopyright = (char*)pStr;
	    QTV_Free(pStr);
		return sCopyright;
	}
  }
  return NULL;
}

/* ======================================================================
FUNCTION:
  ASFFile::getVersion

DESCRIPTION:
  gets version

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING ASFFile::getVersion() const
{
  return NULL;
}

/* ======================================================================
FUNCTION:
  ASFFile::getCreationDate

DESCRIPTION:
  gets creation date

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING ASFFile::getCreationDate() const
{
  return NULL;
}

/* ======================================================================
FUNCTION:
  ASFFile::getMovieDuration

DESCRIPTION:
  gets movie duration in movie timescale unit.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getMovieDuration() const
{
  /* ASF time is given in MS and Time scale is 1000 */
  if(m_strHeaderInfo.u32PlayDuration)
  {
    return (uint32)(m_strHeaderInfo.u32PlayDuration - m_strHeaderInfo.u32Preroll);
  }
  else
  {
    return 0;
  }
}

/* ======================================================================
FUNCTION:
  ASFFile::getMovieTimescale

DESCRIPTION:
  gets movie timescale

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getMovieTimescale() const
{
  return ASF_MOVIE_TIME_SCALE;
}

/* ======================================================================
FUNCTION:
  ASFFile::getMovieDurationMsec

DESCRIPTION:
  gets movie duration in milli secs

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getMovieDurationMsec() const
{
  /* ASF time is given in MS. */
  return m_strHeaderInfo.u32PlayDuration - m_strHeaderInfo.u32Preroll;
}


/* ======================================================================
FUNCTION:
  ASFFile::getNumTracks

DESCRIPTION:
  gets number of track in file

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 number of tracks

SIDE EFFECTS:
  None.
======================================================================*/
int32 ASFFile::getNumTracks()
{
  return m_nNumStreams;
}
/* ======================================================================
FUNCTION:
  ASFFile::GetMaximumBitRateForTrack(uint32 trackId)

DESCRIPTION:
  Returns the max bitrate for the track identified by trackId

INPUT/OUTPUT PARAMETERS:
  TrackId

RETURN VALUE:
 Max Data Bit Rate

SIDE EFFECTS:
  None.
======================================================================*/

uint32 ASFFile::GetMaximumBitRateForTrack(uint32 trackId)
{
  tMediaType_WMC streamType;
  WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);
  tWMCDecStatus nRet = WMCDec_Fail;

  //If pDecoder is NULL, something is very bad..
  if(pDecoder == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "ASFFile::GetMaximumBitRateForTrack: pDecoder is NULL.");
     return 0;
  }

  //Get Total number of streams to iterate
  int strm_cnt = pDecoder->tHeaderInfo.wNoOfStreams;

  nRet = GetMediaTypeForStreamID(&streamType, trackId);

  if(nRet != WMCDec_Succeeded)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "ASFFile::GetMaximumBitRateForTrack:Failed to get StreamType..");
    return 0;
  }

  //Check if bit rate information is available

  /*
  * For Audio streams, we have to return MAX bit rate rather than AVG. bit rate.
  * If BHasBitRateInfo is set to TRUE, then most likely it's not the AVG. bit rate
  * as it gets populated when we parse stream bitrate properties object, which kind of gives
  * MAX bit rate for each stream.
  */

  if( (pDecoder->tHeaderInfo.bHasBitRateInfo) &&  (streamType == Audio_WMC) )
  {
    //Bit rate information is available via stream bitrate preperties object
    for( int itr = 0;itr<strm_cnt;itr++)
    {
      if(pDecoder->tHeaderInfo.tBitRateInfo[itr].wStreamId == trackId)
      {
        //Return MAX of AVG. BIT RATE and MAX. BIT RATE found in the clip information.
        //For Some clips, maximum bitrate information is not available,
        //in such cases, if average bit-rate is greater than the allowed bitrate
        //we should not select such stream for playback;
        uint32 avgBitRate = (uint32)getTrackAverageBitrate(trackId);
        return ZMAX(avgBitRate,pDecoder->tHeaderInfo.tBitRateInfo[itr].dwBitRate);
      }
    }
  }
  else
  {
    //See is bitrate information is available via extended stream properties object.
    for(int itr = 0; itr<strm_cnt;itr++)
    {
      nRet = GetMediaTypeForStreamID(&streamType, trackId);
      if( (nRet==WMCDec_Succeeded) && (streamType == Audio_WMC) )
      {
        AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(trackId);
        if(pAudioStreamInfo)
        {
         /*
         * Return MAX of AVG. BIT RATE and MAX. BIT RATE found in the clip information.
         * For Some clips, maximum bitrate information is not available,
         * in such cases, if average bit-rate is greater than the allowed bitrate
         * we should not select such stream for playback; For Audio,mAX bit rate if available,
         * should be less than or equal to allowed bit rate; This is not the case for video.
         */

         if(pAudioStreamInfo->pExtnStreamProperties)
         {
           uint32 avgBitRate = (uint32)getTrackAverageBitrate(trackId);
           return ZMAX(avgBitRate,pAudioStreamInfo->pExtnStreamProperties->u32AltDataBitrate);
         }

         return (uint32)getTrackAverageBitrate(trackId);
        }
        return 0;
      }//end of if( (nRet==WMCDec_Succeeded) && (streamType == Audio_WMC) )
      else if( (nRet==WMCDec_Succeeded) && (streamType == Video_WMC) )
      {
        /*
         * For Video stream, we just need to return AVG bit rate and not the MAX bit rate available in meata-data.
         * It is OK for Video if AVG. bit rate is within the allowed limit but MAX bit rate is above the allowed limit.
         * If AVG. bit rate is not present in extended stream properties object,
         * getTrackAverageBitrate will eventually call WMCDecGetVideoInfo, which estimates the bit rate.
         */
         return (uint32)getTrackAverageBitrate(trackId);

      }//end of if( (nRet==WMCDec_Succeeded) && (streamType == Video_WMC) )
    }
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackWholeIDList

DESCRIPTION:
  gets list of track IDs

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getTrackWholeIDList(uint32 *ids)
{
  int32 i;

  if(ids)
  {
    for (i = 0; i < (int32)m_nNumStreams; i++)
    {
      (*ids) = m_ptMediaTypeStreams[i].wStreamId;
      ids++;
    }
  }
  return m_nNumStreams;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackContentVersion

DESCRIPTION:
  gets content version number

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
int16 ASFFile::getTrackContentVersion(uint32 id)
{
  tMediaType_WMC streamType;
  tWMCDecStatus nRet = GetMediaTypeForStreamID(&streamType, id);
  if( (nRet==WMCDec_Succeeded) && (streamType==Audio_WMC))
  {
    AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
    if(pAudioStreamInfo)
    {
      return (int16)( pAudioStreamInfo->nVersion );
    }
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::trackRandomAccessDenied

DESCRIPTION:
  gets repositioning permission for the track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 ASFFile::trackRandomAccessDenied(uint32 id)
{
  uint8 nRet = FALSE; /* by default random access is allowed */

  EXTENDEDSTREAMPROPERTIES *tempExtnStreamProperties = NULL;

  if(m_hASFDecoder)
  {
    for(uint32 i=0; (i<FILE_MAX_MEDIA_STREAMS); i++)
    {
      tempExtnStreamProperties = ((WMFDecoderEx *)m_hASFDecoder)->tExtnStreamProperties[i];
      if(tempExtnStreamProperties)
      {
        if( tempExtnStreamProperties->u16StreamId == id )
        {
          /* Check if the random acccess is allowed for this track. i.e."Seekable" flag is set or not */
          if( (tempExtnStreamProperties->u32Flags & 0x2) == 0 )
          {
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"Reposition is not allowed for this track, trackid %u, Seekable flag is set to FALSE", id);
            nRet = TRUE;
          }
          break;
        }
      }
    }
  }
  return nRet;
}
/* ======================================================================
FUNCTION:
  ASFFile::UpdateSamplesInformation

DESCRIPTION:
  Copies over previous sample information into
  current sample information.

INPUT/OUTPUT PARAMETERS:
  INPUT
       :current packet number
       :current numnber of frames
       :current stream number
       :Presentation time from current packet
       :Data size in bytes from current packet
       :total number of frames in packet

  OUTPUT:
    None

RETURN VALUE:
 void

SIDE EFFECTS:
  None.
======================================================================*/

void ASFFile::UpdateSamplesInformation(uint8 cPacketNum, uint8 cNumFrm,
                                       uint32 StreamNum, uint32 PresentationTime,
                                       uint32 size,uint32 wNumFrmInPacket)
{
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"UpdateSamplesInformation");

  m_nPreviousAudioPacketFramesNum = cNumFrm;
  m_nPreviousAudioPacketNum = cPacketNum;

  // update current frame info //
  memcpy(&m_sampleInfo,&m_prvSampleInfo,sizeof(m_sampleInfo));
  m_sampleInfo[StreamNum].size = size;

  // this is not frame number //
  m_prvSampleInfo[StreamNum].sample = cPacketNum;

  if( (m_prvSampleInfo[StreamNum].size > 0)&&(!m_bIsPacketDrop)&&(!m_bAudioReposPending) )
  {
       if( (PresentationTime - m_prvSampleInfo[StreamNum].time)  > 0)
          {
      m_prvSampleInfo[StreamNum].delta = (uint32)(PresentationTime - m_prvSampleInfo[StreamNum].time);

      if(m_nASFAudioPacketDuration == 0)
      {
        m_nASFAudioPacketDuration = m_prvSampleInfo[StreamNum].delta;
      }
      else
      {
        m_nASFAudioPacketDuration += m_prvSampleInfo[StreamNum].delta;
        m_nASFAudioPacketDuration/=2;
      }
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"adjusted m_nASFAudioPacketDuration %ld",m_nASFAudioPacketDuration);
    }
     }
  m_prvSampleInfo[StreamNum].sync = 1;
  m_prvSampleInfo[StreamNum].size = size;

  m_prvSampleInfo[StreamNum].num_frames = wNumFrmInPacket;
  m_prvSampleInfo[StreamNum].time = (uint32)PresentationTime;
  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"m_sampleInfo[StreamNum].delta %ld",m_prvSampleInfo[StreamNum].delta);

  //If there is no spill over, then simply copy previous sampleinfo into m_sampleInfo
  if(!m_bAllowWmaPackets)
    memcpy(&m_sampleInfo,&m_prvSampleInfo,sizeof(m_sampleInfo));
}
/* ======================================================================
FUNCTION:
  ASFFile::getMSecValueForFrameRateCalculation

DESCRIPTION:
  Convert AvTimePerFrame from meta-data to MSec to be used in frame rate calculation.

INPUT/OUTPUT PARAMETERS:
  INPUT: avg. time per frame measured in 100 nanosecond units.(Read in from meta-data while parsing the file)
  OUTPUT:avg. time per frame value in milliseconds.

RETURN VALUE:
 unsigned long

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long ASFFile::getMSecValueForFrameRateCalculation(U64_WMC AvTimePerFrame)
{
 /*
 * According to ASF SPEC, AvTimePerFrame gives avg. time per frame measured in 100 nano-second units.
 * To convert it to msec, we need to do ((AvTimePerFrame * 100)/1000000) which is same as doing (AvTimePerFrame / 10000);
 */
 return (unsigned long)(AvTimePerFrame / 10000);
}
/* ======================================================================
FUNCTION:
  ASFFile::getTrackVideoFrameRate

DESCRIPTION:
  gets track video (if video) frame rate

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
float ASFFile::getTrackVideoFrameRate(uint32 id)
{
  tMediaType_WMC Type;

  float nFrameRate=0.0;

  VIDEOSTREAMINFO *pVideoStreamInfo = NULL;
  unsigned long valInMsec = 0;

  if(GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded)
  {
    switch(Type)
    {
      case Video_WMC:
        pVideoStreamInfo = GetVideoStreamInfo(id);
        if(pVideoStreamInfo && pVideoStreamInfo->pExtnStreamProperties)
        {
          if(pVideoStreamInfo->pExtnStreamProperties[0].u64AvTimePerFrame == 0)
          {
            nFrameRate = 0.0;
            QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "ASFFile::getTrackVideoFrameRate will be 0 as u64AvTimePerFrame is 0 in Meta-Data..");
          }

          if(pVideoStreamInfo->pExtnStreamProperties[0].u64AvTimePerFrame > 999)
          {
            valInMsec = getMSecValueForFrameRateCalculation(pVideoStreamInfo->pExtnStreamProperties[0].u64AvTimePerFrame);
            nFrameRate = (float)1000 / (float)valInMsec;
          }
          else
          {
            nFrameRate = ((float)1000/(float)pVideoStreamInfo->pExtnStreamProperties[0].u64AvTimePerFrame);
          }
        }
        break;

      case Binary_WMC:
      case Audio_WMC:
      default:
        break;
    }
  }
  return nFrameRate;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackVideoFrameWidth

DESCRIPTION:
  returns video track's frame width.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getTrackVideoFrameWidth(uint32 id)
{
  tMediaType_WMC Type;
  if(GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded)
  {
    {
      if(Type == Video_WMC)
      {
        strVideoInfo_WMC strVideoInfo;
        if( WMCDecGetVideoInfo(m_hASFDecoder, id, &strVideoInfo)== WMCDec_Succeeded )
        {
          return (uint32)strVideoInfo.i32Width;
        }
      }
    }
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackVideoFrameHeight

DESCRIPTION:
  returns video track's frame height.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getTrackVideoFrameHeight(uint32 id)
{
  tMediaType_WMC Type;
  if(GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded)
  {
    {
      if(Type == Video_WMC)
      {
        strVideoInfo_WMC strVideoInfo;
        if( WMCDecGetVideoInfo(m_hASFDecoder, id, &strVideoInfo)== WMCDec_Succeeded )
        {
          return (uint32)strVideoInfo.i32Height;
        }
      }
    }
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackMediaDuration

DESCRIPTION:
  gets track duration in track time scale unit

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getTrackMediaDuration(uint32 id)
{
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {
    if(pAudioStreamInfo->pExtnStreamProperties)
    {
      if(pAudioStreamInfo->pExtnStreamProperties->u64StartTime &&
         pAudioStreamInfo->pExtnStreamProperties->u64EndTime)
      {
         return (uint32)(pAudioStreamInfo->pExtnStreamProperties->u64EndTime -
                         pAudioStreamInfo->pExtnStreamProperties->u64StartTime);
      }
      else if(m_strHeaderInfo.u32PlayDuration)
      {
        return (uint32)(m_strHeaderInfo.u32PlayDuration - m_strHeaderInfo.u32Preroll);
      }
    }
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackMediaTimescale

DESCRIPTION:
  gets track time scale

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getTrackMediaTimescale(uint32 /*id*/)
{
  return ASF_STREAM_TIME_SCALE;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackAudioSamplingFreq

DESCRIPTION:
  gets audio track's sampling frequency

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getTrackAudioSamplingFreq(uint32 id)
{
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {
    #ifdef FEATURE_QTV_WMA_PRO_DECODER
      if(m_bUseARMDecoder && isWmaProDecoderNeeded(id) )
      {
        /*
        * Even though WMA track might have > 48 KHZ,
        * While configuring decoder output PCM format,we down-sample it to 44.1 or 48 KHZ.
        * So, always return sampling frequency from output PCM format.
        *
        */
        return ((WMFDecoderEx*)m_hASFDecoder)->sPCMFormat.nSamplesPerSec;
      }
    #endif
    return (uint32)( pAudioStreamInfo->nSamplesPerSec );
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::getAudioSamplesPerFrame

DESCRIPTION:
  gets audio track's number of samples per frame.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getAudioSamplesPerFrame(uint32 id)
{
  uint32 dwSamplesPerFrame = 0;
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);

  if(!pAudioStreamInfo)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Audio Stream Info not present");
    return dwSamplesPerFrame;
  }

  if(pAudioStreamInfo->nVersion == 1)
  {
    if( pAudioStreamInfo->nSamplesPerSec<=16000 )
    {
      dwSamplesPerFrame = 512;
    }
    else if(pAudioStreamInfo->nSamplesPerSec <= 32000 )
    {
      dwSamplesPerFrame = 1024;
    }
    else if(pAudioStreamInfo->nSamplesPerSec > 32000 )
    {
      dwSamplesPerFrame = 2048;
    }
  }
  else if (pAudioStreamInfo->nVersion == 2)
  {
    if( pAudioStreamInfo->nSamplesPerSec<=16000 )
    {
      dwSamplesPerFrame = 512;
    }
    else if(pAudioStreamInfo->nSamplesPerSec <= 22050 )
    {
      dwSamplesPerFrame = 1024;
    }
    else if(pAudioStreamInfo->nSamplesPerSec > 22050 )
    {
      dwSamplesPerFrame = 2048;
    }
  }
#if defined(FEATURE_QTV_WMA_PRO_DECODER) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
  else if( pAudioStreamInfo->nVersion == 3 )
  {
    if (pAudioStreamInfo->nSamplesPerSec <= 16000)
    {
        dwSamplesPerFrame = 512;
    }
    else if (pAudioStreamInfo->nSamplesPerSec <= 22050)
    {
        dwSamplesPerFrame = 1024;
    }
    else if (pAudioStreamInfo->nSamplesPerSec <=  48000)
    {
      dwSamplesPerFrame = 2048;
    }
    else if (pAudioStreamInfo->nSamplesPerSec <= 96000)
    {
        dwSamplesPerFrame = 4096;
    }
    else if (pAudioStreamInfo->nSamplesPerSec >= 192000)
    {
        dwSamplesPerFrame = 8192;// cap to 8192.
    }

    //Traditional sampling rate-based frame size multiplier
    U16_WMC iFrmSizeModifier = pAudioStreamInfo->nEncodeOpt & ENCOPT3_FRM_SIZE_MOD;
    if (iFrmSizeModifier == 2)
    {
      dwSamplesPerFrame *= 2;
    }
    else if (iFrmSizeModifier == 4)
    {
      dwSamplesPerFrame /= 2;
    }
    else if (iFrmSizeModifier == 6)
    {
      dwSamplesPerFrame /= 4;
    }
  }
#endif /* #if defined(FEATURE_QTV_WMA_PRO_DECODER) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

  if(!dwSamplesPerFrame)
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "unsupported Version=%d, SamplingRate=%d",
              pAudioStreamInfo->nVersion, pAudioStreamInfo->nSamplesPerSec  );
  }
  return dwSamplesPerFrame;
}

/* ======================================================================
FUNCTION:
  ASFFile::GetAudioBitsPerSample

DESCRIPTION:
  returns bits per sample

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long ASFFile::GetAudioBitsPerSample(int id)
{
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {
    #ifdef FEATURE_QTV_WMA_PRO_DECODER
      if(m_bUseARMDecoder && isWmaProDecoderNeeded(id) )
      {
        /*
        * Even though WMA track might have more than 16 bits/sample,
        * CMX can't play PCM samples if number of bits/sample is > 16.
        * While configuring decoder output PCM format, we down-sample it to 16 bits/sample.
        * So, always return number of bits/sample from output PCM format.
        */
        return ((WMFDecoderEx*)m_hASFDecoder)->sPCMFormat.nValidBitsPerSample;
      }
    #endif
    return (unsigned long)( pAudioStreamInfo->nBitsPerSample );
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::GetNumAudioChannels

DESCRIPTION:
  returns number of audio channels

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long ASFFile::GetNumAudioChannels(int id)
{
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {
    #ifdef FEATURE_QTV_WMA_PRO_DECODER
      if(m_bUseARMDecoder && isWmaProDecoderNeeded(id) )
      {
        /*
        * Even though WMA track might have more than 2 channels,
        * CMX can't play PCM samples if number of channels > 2.
        * While configuring decoder output PCM format, we set it stereo if channels > 2.
        * So, always return number of channels from output PCM format.
        *
        */
        return ((WMFDecoderEx*)m_hASFDecoder)->sPCMFormat.nChannels;
      }
    #endif
    return (unsigned long)( pAudioStreamInfo->nChannels );
  }
  return 0;
}
/* ======================================================================
FUNCTION:
  ASFFile::GetFormatTag

DESCRIPTION:
  returns format tag for the given track id

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint16 ASFFile::GetFormatTag(int id)
{
  uint16 formattag = 0;
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {  
    formattag = pAudioStreamInfo->wFormatTag;
  }
  return formattag;
}
/* ======================================================================
FUNCTION:
  ASFFile::GetBlockAlign

DESCRIPTION:
  returns block align for the given track id

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::GetBlockAlign(int id)
{
  uint32 blockalign = 0;
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {  
    blockalign = pAudioStreamInfo->nBlockAlign;
  }
  return blockalign;
}

/* ======================================================================
FUNCTION:
  ASFFile::GetFixedAsfAudioPacketSize

DESCRIPTION:
  returns the size of Audio ASF packet.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long ASFFile::GetFixedAsfAudioPacketSize(int id)
{
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {
    return (unsigned long)( pAudioStreamInfo->nBlockAlign );
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::GetAudioEncoderOptions

DESCRIPTION:
  returns the encoder options.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long ASFFile::GetAudioEncoderOptions(int id)
{
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {
    return (unsigned long)( pAudioStreamInfo->nEncodeOpt );
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::peekCurSample

DESCRIPTION:
  gets information about current sample of a track

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::peekCurSample(uint32 trackid, file_sample_info_type *pSampleInfo)
{
  uint32 StreamNum;
  if (pSampleInfo == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "pSampleInfo is NULL !!");
    return DEFAULT_ERROR;
  }
  if(GetStreamNumForID(&StreamNum, trackid) != WMCDec_Succeeded)
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "peekCurSample, unknown track id = %d", trackid);
    return DEFAULT_ERROR;
  }

  *pSampleInfo = m_sampleInfo[StreamNum];
  return EVERYTHING_FINE;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackOTIType

DESCRIPTION:
  gets track OTI value

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 ASFFile::getTrackOTIType(uint32 id)
{
  uint8 format = 0xFF;
  tMediaType_WMC Type;

  if( GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded )
  {
    switch(Type)
    {
      case Audio_WMC:
        #ifdef FEATURE_QTV_WMA_PRO_DECODER
          if(m_bUseARMDecoder && isWmaProDecoderNeeded(id, &format) )
          {
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "WMA Track Format %d", format);
          }
          else
        #endif /* FEATURE_QTV_WMA_PRO_DECODER */
        #ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
          if(isWmaProDecoderNeeded(id, &format))
          {
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "WMA Track Format %d", format);
          }
          else
        #endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
        format = WM_AUDIO;
        break;

      case Video_WMC:

        if(GetWMVCompressionType()== FOURCC_WMV1)
          format = WM_VIDEO_7;

        if(GetWMVCompressionType()== FOURCC_WMV3)
          format = WM_VIDEO_9;

        if(GetWMVCompressionType()== FOURCC_WMV2)
          format = WM_VIDEO_8;
        break;

      case Binary_WMC:
      default:
        break;
    }
  }
  return format;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackAudioFormat

DESCRIPTION:
  gets track audio format based on VideoFMT enum

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 ASFFile::getTrackAudioFormat(uint32 /*id*/)
{
  return 0xFF;  /* not valid for WMA CODEC */
}

/* ======================================================================
FUNCTION:
ASFFile::getLargestFrameSize

DESCRIPTION:
  gets the largest frame size in the given track
INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getLargestFrameSize(uint32 /*id*/)
{
  return 0;  /* not valid for WMV/WMA CODEC */
}

/* ======================================================================
FUNCTION:
  ASFFile::getFramesPerSample

DESCRIPTION:
  gets the number frames per sample of given track in video_fmt_stream_audio_subtype type.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 ASFFile::getFramesPerSample(uint32 /*id*/)
{
  return 0x00;  /* not valid for WMA CODEC */
}

/* ======================================================================
FUNCTION:
  ASFFile::getTotalNumberOfFrames

DESCRIPTION:
  gets total number of frames in a given track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint16 ASFFile::getTotalNumberOfFrames(uint32 /*id*/)
{
  return 0x00;  /* not valid for WMA CODEC */
}


/* ======================================================================
FUNCTION:
  ASFFile::getTrackMaxBufferSizeDB

DESCRIPTION:
  gets maximum buffer size to play the track

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
int32 ASFFile::getTrackMaxBufferSizeDB(uint32 id)
{
  tMediaType_WMC Type;
  uint32 StreamNum;
  uint32 nBufSize=0;
  if(GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded)
  {
    StreamNum = GetStreamNumForID(&StreamNum, id);
    /*
    * m_nLargestFrame is declared as
    * uint32 m_nLargestFrame[FILE_MAX_MEDIA_STREAMS];
    * Make sure StreamNum that we get from GetStreamNumForID
    * is within the limits.
    */
    if(StreamNum >= FILE_MAX_MEDIA_STREAMS)
    {
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS,
                    QTVDIAG_PRIO_ERROR,
                    "Error:StreamNum(%d) >= FILE_MAX_MEDIA_STREAMS(%d)",StreamNum,FILE_MAX_MEDIA_STREAMS);
      return 0;
    }

    switch(Type)
    {
      case Audio_WMC:
        {
          AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
          if(pAudioStreamInfo)
          {
            #ifdef FEATURE_QTV_WMA_PRO_DECODER
              if(m_bUseARMDecoder && isWmaProDecoderNeeded(id) )
              {
                nBufSize = MAX_DECODED_BUFFER_SIZE;
              }
              else
            #endif
              {
                nBufSize = MAX(pAudioStreamInfo->nBlockAlign, 1024);
                nBufSize = 2*MAX(m_nLargestFrame[StreamNum],nBufSize );
              }
          }
        }
        break;

      case Video_WMC:
        {
          VIDEOSTREAMINFO *pVideoStreamInfo = GetVideoStreamInfo(id);
          if(pVideoStreamInfo)
            nBufSize = MAX(m_nLargestFrame[StreamNum], pVideoStreamInfo->dwVideoBufSize);
        }
        break;

      case Binary_WMC:
        {
          BINARYSTREAMINFO *pBinaryStreamInfo = GetBinaryStreamInfo(id);
          if(pBinaryStreamInfo)
            nBufSize = MAX(m_nLargestFrame[StreamNum], pBinaryStreamInfo->dwBinaryBufSize);
        }
        break;

      default:
        break;
    }
  }
  return (int32)nBufSize;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackAverageBitrate

DESCRIPTION:
  gets track's average bitrate

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 average bitrate

SIDE EFFECTS:
  None.
======================================================================*/
int32 ASFFile::getTrackAverageBitrate(uint32 id)
{
 tMediaType_WMC Type;
 int32 nBitRate = 0;
 AUDIOSTREAMINFO *pAudioStreamInfo = NULL;
 VIDEOSTREAMINFO *pVideoStreamInfo = NULL;

 if(GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded)
 {
   switch(Type)
   {
     case Audio_WMC:
       pAudioStreamInfo = GetAudioStreamInfo(id);
       if(pAudioStreamInfo)
         nBitRate = pAudioStreamInfo->nAvgBytesPerSec * 8;
       break;

     case Video_WMC:
       pVideoStreamInfo = GetVideoStreamInfo(id);
       if(pVideoStreamInfo)
       {
         if(pVideoStreamInfo->pExtnStreamProperties)
           nBitRate = (int32)pVideoStreamInfo->pExtnStreamProperties->u32AvDataBitrate;
         else
         {
           WMCDecGetVideoInfo (m_hASFDecoder, id, &m_strVideoInfo);
           nBitRate = (int32)m_strVideoInfo.u32BitsPerSecond;
         }
       }

       break;

     case Binary_WMC:
       nBitRate = 0;
       break;

     default:
       break;
   }
 }
 return nBitRate;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackMaxBitrate

DESCRIPTION:
  gets track's max bitrate

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
int32 ASFFile::getTrackMaxBitrate(uint32 id)
{
  return getTrackAverageBitrate(id);
}
/* ======================================================================
  FUNCTION:
    ASFFile::GetWMVCompressionType()

  DESCRIPTION:
    returns compression type;This is used to initialize decoder.

  INPUT/OUTPUT PARAMETERS:
    Compression type defined in wmf_types.h

  RETURN VALUE:
   none

  SIDE EFFECTS:
    None.
  ======================================================================*/
  U32_WMC ASFFile::GetWMVCompressionType()
  {
    WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);
    int cnt;

    if(pDecoder == NULL)
      return 0;

    cnt = pDecoder->tHeaderInfo.wNoOfVideoStreams;
    if(cnt > 0)
    {
      if(pDecoder->tVideoStreamInfo[0])
      {
        return pDecoder->tVideoStreamInfo[0]->biCompression;
      }
    }
   return 0;
  }


/* ======================================================================
FUNCTION:
  ASFFile::GetAudioAdvancedEncodeOptions

DESCRIPTION:
  returns the Advanced encoder options.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint16 ASFFile::GetAudioAdvancedEncodeOptions(int id)
{
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {
    return (uint16)(pAudioStreamInfo->nAdvancedEncodeOpt);
  }
#endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
  (void)id;
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::GetAudioAdvancedEncodeOptions2

DESCRIPTION:
  returns the Advanced encoder options.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::GetAudioAdvancedEncodeOptions2(int id)
{
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {
    return (uint32)(pAudioStreamInfo->nAdvancedEncodeOpt2);
  }
#endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
  (void)id;
  return 0;
}


/* ======================================================================
FUNCTION:
  ASFFile::GetAudioChannelMask

DESCRIPTION:
  returns the Advanced encoder options.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint16 ASFFile::GetAudioChannelMask(int id)
{
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {
    return (uint16)(pAudioStreamInfo->dwChannelMask);
  }
  return 0;
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
bool ASFFile::UpdateTrackIdInFilePointer()
{
  AUDIOSTREAMINFO *pAudioStreamInfo = NULL;
  VIDEOSTREAMINFO *pVideoStreamInfo = NULL;
  
  if (m_playAudio)
  {
      pAudioStreamInfo = ((WMFDecoderEx *)m_hASFDecoder)->tAudioStreamInfo[0];
      if(pAudioStreamInfo)
      {
	    m_AsfFilePtr->pullBuf.trackId = pAudioStreamInfo->wStreamId;
        return true;
      }
  }
  else if (m_playVideo)
  {
      pVideoStreamInfo = ((WMFDecoderEx *)m_hASFDecoder)->tVideoStreamInfo[0];
      if(pVideoStreamInfo)
      {
	    m_AsfFilePtr->pullBuf.trackId = pVideoStreamInfo->wStreamId;
        return true;
      }    
  }

  return false;
}
/* ======================================================================
FUNCTION:
  ASFFile::GetAudioArmDataReqThr

DESCRIPTION:
  returns the Advanced encoder options.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint16 ASFFile::GetAudioArmDataReqThr(int id)
{
  (void) id;
  return 0;
}


/* ======================================================================
FUNCTION:
  ASFFile::GetAudioValidBitsperSample

DESCRIPTION:
  returns the Advanced encoder options.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 ASFFile::GetAudioValidBitsperSample(int id)
{
  AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {
    return (uint8)(pAudioStreamInfo->wValidBitsPerSample);
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::isIndexObjectPresent

DESCRIPTION:
  returns true if index object is present

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
bool ASFFile::isIndexObjectPresent()
{
  WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);

  if(m_bStreaming)
    return false;

  if(!pDecoder)
    return false;

  if(pDecoder->tIndexInfo && pDecoder->tIndexInfo->num_entries)
    return true;

  return false;
}
/* ======================================================================
FUNCTION:
  ASFFile::isVideoCodecSupported

DESCRIPTION:
  returns true if video codec is supported else returns false;

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
bool ASFFile::isVideoCodecSupported()
{
  char U8Value[META_DATA_INFO_LEN];
  WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);
  int cnt = 0;
  if(pDecoder == NULL)
    return false;

  cnt = pDecoder->tHeaderInfo.wNoOfVideoStreams;
  if(cnt > 0)
  {
    if(pDecoder->tVideoStreamInfo[0])
    {

      m_VidProfile = UNKNOWN_PROFILE;
      /*
      * Try to determine the video profile, if available.
      * If decoding is not done in ARM, reject the bit-stream if profile is not SIMPLE.
      * Currently, DSP supports only SIMPLE PROFILE.
      */
      if(pDecoder->tMetaDataEntry.m_wDescRecordsCount > 0)
      {
        int itr = 0;
        while( itr < pDecoder->tMetaDataEntry.m_wDescRecordsCount)
        {
          U8Value[0]='\0';

          if(pDecoder->tMetaDataEntry.pDescRec[itr].wStreamNumber == pDecoder->tVideoStreamInfo[0]->wStreamId)
          {
            WideCharToChar((const wchar_t *)pDecoder->tMetaDataEntry.pDescRec[itr].pwName,
                       -1, U8Value,META_DATA_INFO_LEN -1);


            if( !std_stricmp(U8Value,PROFILE_TEMPLATE) )
            {
              WideCharToChar((const wchar_t *)pDecoder->tMetaDataEntry.pDescRec[itr].pData,
                       -1, U8Value, META_DATA_INFO_LEN-1);

              if( ZUtils::StrncmpI( U8Value, S_PROFILE,strlen(S_PROFILE)) )
              {
                m_VidProfile = SIMPLEPROFILE;
                QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_MED, "Video:SIMPLE PROFILE");
                break;
              }
              else if( (strlen(U8Value))                                         &&
                       (ZUtils::StrncmpI( U8Value, M_PROFILE,strlen(M_PROFILE))) &&
                       (pDecoder->tVideoStreamInfo[0]->biCompression == FOURCC_WMV3)
                     )
              {
                //Do not check for profile,as we will use ARM solution if DSP does not support it.
                //Customer can enable this feature, if they want.
#ifdef CHECK_WMV_FOR_PROFILE_AND_BITRATE
                //Reject main profile video track only if video is WMV9
                //For WMV7 and WMV8 it's OK to have MP.
                _fileErrorCode = TRACK_VIDEO_UNSUPPORTED_PROFILE;
                m_VidProfile = UNSUPPORTED_PROFILE;
                QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Error:Unsupported Video Profile");
                return false;
#else
                m_VidProfile = MAINPROFILE;
                QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_MED, "Video:MAIN PROFILE");
                break;
#endif
              }
            }//if( U8Value && (!strcmpi(U8Value,PROFILE_TEMPLATE)) )

          }//if(pDecoder->tMetaDataEntry.pDescRec[itr].wStreamNumber == pDecoder->tVideoStreamInfo[0]->wStreamId)

          itr++;

        }//while( itr < pDecoder->tMetaDataEntry.m_wDescRecordsCount)

      }//if(pDecoder->tMetaDataEntry.m_wDescRecordsCount > 0)

      /*
      * Make sure video bit frate is within allowed limit, else return false;
      */
#if(defined (MAX_VIDEO_BIT_RATE_ALLOWED) )
      //Do not check for video bit-rate, as we should try to play irrespective of bit-rate.
      //Customer can enable this feature if want to limit based on bit-rate.
#ifdef CHECK_WMV_FOR_PROFILE_AND_BITRATE
      uint32 MaxBitrate = 0;
      MaxBitrate = GetMaximumBitRateForTrack(pDecoder->tVideoStreamInfo[0]->wStreamId);

               if( MaxBitrate > MAX_VIDEO_BIT_RATE_ALLOWED )
      {
        _fileErrorCode = TRACK_VIDEO_UNSUPPORTED_BITRATE;
        QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Error:Unsupported Max/Avg Video BitRate %ld (MAX Allowed is %ld)",MaxBitrate,MAX_VIDEO_BIT_RATE_ALLOWED);
        return false;
      }
#else
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_MED, "Max/Avg Video BitRate %ld ",GetMaximumBitRateForTrack(pDecoder->tVideoStreamInfo[0]->wStreamId));
#endif

#endif

      /*
      * Currently, we support only WMV7/WMV8/WMV9
      */
      if( (pDecoder->tVideoStreamInfo[0]->biCompression == FOURCC_WMV1)||
          (pDecoder->tVideoStreamInfo[0]->biCompression == FOURCC_WMV2)||
          (pDecoder->tVideoStreamInfo[0]->biCompression == FOURCC_WMV3))
        return true;
    }
  }
  return false;
}

 /* ======================================================================
FUNCTION:
  ASFFile::GetAudioASFPacketDuration

DESCRIPTION:
  returns packet duration in msec for each audio ASF packet.
  This value gets updated after every two consecutive ASF packets.

INPUT/OUTPUT PARAMETERS:
  trackid

RETURN VALUE:
 packet duration
SIDE EFFECTS:
  None.
======================================================================*/
long ASFFile::GetAudioASFPacketDuration(int trackid)
{
  WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);

  if(m_nASFAudioPacketDuration > 0)
    return m_nASFAudioPacketDuration;

  if(pDecoder == NULL)
    return 0;

  if(pDecoder->tHeaderInfo.cPackets > 0)
  {
    m_nASFAudioPacketDuration = (long) getTrackMediaDuration((uint)trackid)/pDecoder->tHeaderInfo.cPackets;
  }
  else
  {
    m_nASFAudioPacketDuration = 0;
  }

  return m_nASFAudioPacketDuration;
}


/* ======================================================================
 FUNCTION:
   ASFFile::GetTotalNumberOfAudioStreams()

 DESCRIPTION:
   returns total number of audio streams.

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 U16_WMC ASFFile::GetTotalNumberOfAudioStreams()
 {
   WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);
   if(pDecoder == NULL)
     return 0;
   return pDecoder->tHeaderInfo.wNoOfAudioStreams;
 }
 /* ======================================================================
 FUNCTION:
   ASFFile::GetTotalNumberOfVideoStreams()

 DESCRIPTION:
   returns total number of video streams.

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 U16_WMC ASFFile::GetTotalNumberOfVideoStreams()
 {
    WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);
    if(pDecoder == NULL)
     return 0;
    return pDecoder->tHeaderInfo.wNoOfVideoStreams;
 }
 /* ======================================================================
 FUNCTION:
   ASFFile::GetTotalNumberOfBinaryStreams()

 DESCRIPTION:
   returns total number of binary streams.

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 U16_WMC ASFFile::GetTotalNumberOfBinaryStreams()
 {
   WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);
   if(pDecoder == NULL)
     return 0;
   return pDecoder->tHeaderInfo.wNoOfBinaryStreams;
 }

  /* ======================================================================
 FUNCTION:
   ASFFile::GetFirstPacketOffset()

 DESCRIPTION:
   returns total number of binary streams.

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 U32_WMC ASFFile::GetFirstPacketOffset()
 {
   WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);
   if(pDecoder == NULL)
     return 0;
   return pDecoder->tHeaderInfo.cbFirstPacketOffset;
 }

   /* ======================================================================
 FUNCTION:
   ASFFile::GetLastPacketOffset()

 DESCRIPTION:
   returns total number of binary streams.

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 U32_WMC ASFFile::GetLastPacketOffset()
 {
   WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);
   if(pDecoder == NULL)
     return 0;
   return pDecoder->tHeaderInfo.cbLastPacketOffset;
 }

    /* ======================================================================
 FUNCTION:
   ASFFile::GetAsfPacketSize()

 DESCRIPTION:
   returns total number of binary streams.

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 U32_WMC ASFFile::GetAsfPacketSize()
 {
   WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);
   if(pDecoder == NULL)
     return 0;
   return pDecoder->tHeaderInfo.cbPacketSize;
 }

/* ======================================================================
FUNCTION:
  ASFFile::isStartOfDummyBytes

DESCRIPTION:
  returns true if we have started sending DUMMY bytes else returns false.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
bool ASFFile::isStartOfDummyBytes(int /*id*/)
{
  return m_bIsDummyBytesStart;
}
/* ======================================================================
 FUNCTION:
   ASFFile::isVideoInstance()

 DESCRIPTION:
   returns true if this is video instance

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 bool ASFFile::isVideoInstance()
 {
     return m_playVideo;
 }
 /* ======================================================================
 FUNCTION:
   ASFFile::isAudioInstance()

 DESCRIPTION:
   returns true if this is audio instance

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 bool ASFFile::isAudioInstance()
 {
     return m_playAudio;
 }
 /* ======================================================================
 FUNCTION:
   ASFFile::isTextInstance()

 DESCRIPTION:
   returns true if this is text instance

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 bool ASFFile::isTextInstance()
 {
     return m_playText;
 }

/* ======================================================================
FUNCTION:
  ASFFile::GetAudioVirtualPacketSize

DESCRIPTION:
  returns virtual packet size

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint16 ASFFile::GetAudioVirtualPacketSize(int id)
{
  AUDIOSTREAMINFO *pAudioStreamInfo;
  unsigned int nFreq = 0;

  pAudioStreamInfo= GetAudioStreamInfo(id);

  if( (pAudioStreamInfo) && (pAudioStreamInfo->wVirtualPacketLength > 0))
  {
    return (pAudioStreamInfo->wVirtualPacketLength);
  }

  nFreq = getTrackAudioSamplingFreq(id);
  if(nFreq < 16000)
  {
          return 900;
  }

  if( (16000 <= nFreq) && (nFreq <= 32000) )
  {
          return 4600;
  }

  if(nFreq > 32000)
  {
          return 6560;
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::getAllowAudioOnly

DESCRIPTION:
  gets if audio only playback is allowed.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 ASFFile::getAllowAudioOnly()
{
  return 1; /* by default allow */
}

/* ======================================================================
FUNCTION:
  ASFFile::getAllowVideoOnly

DESCRIPTION:
  gets if video only playback is allowed.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 ASFFile::getAllowVideoOnly()
{
  return 1; /* by default allow */
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackDecoderSpecificInfoContent

DESCRIPTION:
  this returns Sequence (VOL) Header.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 *ASFFile::getTrackDecoderSpecificInfoContent(uint32 id)
{
  tMediaType_WMC Type;
  if(GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded)
  {
    if(Type == Video_WMC)
    {
      VIDEOSTREAMINFO *pVStreamInfo = GetVideoStreamInfo(id);
      if(pVStreamInfo)
      {
        return pVStreamInfo->bAdditionalInfo;
      }
    }
  }
  return NULL;
}

/* ======================================================================
FUNCTION:
  ASFFile::getTrackDecoderSpecificInfoSize

DESCRIPTION:
  this returns Sequence (VOL) Header size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 ASFFile::getTrackDecoderSpecificInfoSize(uint32 id)
{
  tMediaType_WMC Type;
  if(GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded)
  {
    if(Type == Video_WMC)
    {
      VIDEOSTREAMINFO *pVStreamInfo = GetVideoStreamInfo(id);
      if(pVStreamInfo)
      {
        return pVStreamInfo->biSize - sizeof (BITMAPINFOHEADER);
      }
    }
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  ASFFile::SetTimeStampedSample

DESCRIPTION:
  gets closest sample's info of the closest frame of given time.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
bool ASFFile::SetTimeStampedSample(uint32 id, uint32 TimeStamp, uint32 *newTimeStamp, boolean isRewind)
{
  U64_WMC u64TimeReturn = 0;
  tWMCDecStatus Ret;
  tMediaType_WMC Type;
  U64_WMC u64TempTimeReturn = 0;
  uint32 StreamNum;
  U64_WMC TimeStamp2 = 0;

  if(GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded)
  {
    if( TimeStamp && (Type==Video_WMC) )
    {
      if(isRewind)
      {
        /* Now we can handle RW with/without Simple index object */
        Ret = WMCDecGetPreviousIFrameTime (m_hASFDecoder, (U16_WMC)id, TimeStamp, &u64TempTimeReturn, NULL, NULL, NULL, NULL );
      }
      else
      {
        /* Now we can handle FF with/without Simple index object */
        Ret = WMCDecGetNextIFrameTime (m_hASFDecoder, (U16_WMC)id, TimeStamp, &u64TempTimeReturn, NULL, NULL, NULL, NULL );
      }
      if(Ret == WMCDec_Succeeded)
      {
        Ret = WMCDecSeek (m_hASFDecoder, u64TempTimeReturn, &u64TimeReturn);
      }

      if(Ret == WMCDec_Succeeded)
      {
        QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "SetTimeStampedSample, Time IN=%d, OUT=%d", TimeStamp,u64TimeReturn);
        *newTimeStamp = (uint32)u64TimeReturn;
        return true;
      }
    }//end of if( TimeStamp && (Type==Video_WMC) )
    else
    {
     if(GetStreamNumForID(&StreamNum, id) == WMCDec_Succeeded)
      {
        if( (m_bWMADecodeDone) && (!isRewind) )
        {
          //If audio is already done and user is attempting FWD, fail the repositioning.
          *newTimeStamp = m_sampleInfo[StreamNum].time;
          m_sampleInfo[StreamNum].size = 0;
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"SetTimeStampedSample, Seek failed for track  %d as WMA track has ended.",id);
          return false;
        }
      }
      TimeStamp2 = (U64_WMC)TimeStamp;
      if (Type == Audio_WMC && isRewind && TimeStamp != 0 )
      {
        Ret = WMCDecGetPreviousFrameTime (m_hASFDecoder, (U16_WMC)id, TimeStamp, &TimeStamp2);
        if (Ret == WMCDec_Succeeded)
        {
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"Audio Rewind, Seek to previous frame TS %d",TimeStamp);
        }
      }
      Ret = WMCDecSeek (m_hASFDecoder, TimeStamp2, &u64TimeReturn);
      if( (Ret == WMCDec_Succeeded)|| (Ret == WMCDec_DecodeComplete) )
      {
        if(Ret == WMCDec_DecodeComplete)
        {
          *newTimeStamp = (uint32)TimeStamp;
          if(GetStreamNumForID(&StreamNum, id) == WMCDec_Succeeded)
          {
               m_sampleInfo[StreamNum].time = (uint32)TimeStamp;
            m_bWMADecodeDone = true;
            m_nWMATotalDummyBytesSent = WMA_TOTAL_DUMMY_BYTES_TO_SEND;
            m_sampleInfo[StreamNum].size = 0;
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"SetTimeStampedSample, Seek to EOF TS %d",TimeStamp);
          }
        }//end of if(Ret == WMCDec_DecodeComplete)
        else
        {
          *newTimeStamp = (uint32)u64TimeReturn;
        }
        QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"SetTimeStampedSample, Time IN=%d, OUT=%d", TimeStamp, *newTimeStamp);
        return true;
      }
    }
  }
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "SetTimeStampedSample failed");
  return false;
}

/* ======================================================================
FUNCTION:
  ASFFile::GetMediaTypeForStreamID

DESCRIPTION:
  gets sample at the given timestamp

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
tWMCDecStatus ASFFile::GetMediaTypeForStreamID(tMediaType_WMC *pType, uint32 wStreamId)
{
  int i;
  tWMCDecStatus err = WMCDec_InValidArguments;
  for(i=0; i<(int)m_nNumStreams; i++)
  {
    if( m_ptMediaTypeStreams[i].wStreamId == (U16_WMC)wStreamId )
    {
      *pType = m_ptMediaTypeStreams[i].MediaType;
      err = WMCDec_Succeeded;
    }
  }
  return err;
}

/* ======================================================================
FUNCTION:
  ASFFile::GetStreamNumForID

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
tWMCDecStatus ASFFile::GetStreamNumForID(uint32 *pStreamNum, uint32 wStreamId)
{
  int i;
  tWMCDecStatus err = WMCDec_InValidArguments;
  for(i=0; i<(int)m_nNumStreams; i++)
  {
    if( m_ptMediaTypeStreams[i].wStreamId == (U16_WMC)wStreamId )
    {
      *pStreamNum = i;
      err = WMCDec_Succeeded;
    }
  }
  return err;
}

/* ======================================================================
FUNCTION:
  ASFFile::GetAudioStreamInfo

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
AUDIOSTREAMINFO *ASFFile::GetAudioStreamInfo(uint32 wStreamId)
{
  int i;
  AUDIOSTREAMINFO *pAudioStreamInfo;
  for(i=0; (i<FILE_MAX_MEDIA_STREAMS); i++)
  {
    pAudioStreamInfo = ((WMFDecoderEx *)m_hASFDecoder)->tAudioStreamInfo[i];
    if(!pAudioStreamInfo)
      break;

    if(pAudioStreamInfo->wStreamId == wStreamId)
    {
      return pAudioStreamInfo;
    }
  }
  return NULL;
}

/* ======================================================================
FUNCTION:
  ASFFile::GetVideoStreamInfo

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
VIDEOSTREAMINFO *ASFFile::GetVideoStreamInfo(uint32 wStreamId)
{
  int i;
  VIDEOSTREAMINFO *pVideoStreamInfo;
  for(i=0; (i<FILE_MAX_MEDIA_STREAMS); i++)
  {
    pVideoStreamInfo = ((WMFDecoderEx *)m_hASFDecoder)->tVideoStreamInfo[i];
    if(!pVideoStreamInfo)
      break;

    if(pVideoStreamInfo->wStreamId == wStreamId)
    {
      return pVideoStreamInfo;
    }
  }
  return NULL;
}

/* ======================================================================
FUNCTION:
  ASFFile::GetBinaryStreamInfo

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
BINARYSTREAMINFO *ASFFile::GetBinaryStreamInfo(uint32 wStreamId)
{  int i;
  BINARYSTREAMINFO *pBinaryStreamInfo;
  for(i=0; i<FILE_MAX_MEDIA_STREAMS; i++)
  {
    pBinaryStreamInfo = ((WMFDecoderEx *)m_hASFDecoder)->tBinaryStreamInfo[i];
    if(!pBinaryStreamInfo)
      break;

    if(pBinaryStreamInfo->wStreamId == wStreamId)
    {
      return pBinaryStreamInfo;
    }
  }
  return NULL;
}

/* ======================================================================
FUNCTION:
  ConvertUnicodeToAscii

DESCRIPTION:
  Converts Unicode String to ASCII string by skipping second byte
  of Unicode char.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
void ConvertUnicodeToAscii(char *pDest, uint16 *pSrc , uint32 size)
{
  for( ; *pSrc && size; size--, pDest++,  pSrc++)
  {
    *pDest = *((char *)pSrc);
  }
}

/* ======================================================================
FUNCTION:
  ConvertUnicodeToUTF8

DESCRIPTION:
  Converts Unicode String to UTF8 string and appends NULL at the end.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
void ConvertUnicodeToUTF8(uint8 *pDest,  uint32 destLen, uint16 *pSrc , uint32 srcLen) 
{
  uint8 ch_tmp_byte;
  int32 i_cur_output=0; 
  uint32 i;
  for ( i=0; i<srcLen; i++ )
  {	 
    if ( BYTE_1_REP > pSrc[i] ) /* 1 byte utf8 representation */
    {
      if ( i_cur_output+1<destLen )
        pDest[i_cur_output++]=(uint8)pSrc[i];
      else
        return; /* ERROR_INSUFFICIENT_BUFFER */
    }
    else if ( BYTE_2_REP > pSrc[i] ) /* 2 byte utf8 representation */
    {
      if ( i_cur_output+2<destLen )
      {
        pDest[i_cur_output++]=(int8)(pSrc[i] >> 6 | 0xc0);
        pDest[i_cur_output++]=(int8)(pSrc[i] & 0x3f | 0x80);
      }
      else
      {
        return; /* ERROR_INSUFFICIENT_BUFFER */
      }
    }
    else if ( SURROGATE_MAX > pSrc[i] && SURROGATE_MIN < pSrc[i] )
    {        /* 4 byte surrogate pair representation */
      if ( i_cur_output+4<destLen )
      {
        ch_tmp_byte = (int8)(((pSrc[i] & 0x3c0) >> 6) + 1);
        pDest[i_cur_output++]=(int8)(ch_tmp_byte >> 2 | 0xf0); 
        pDest[i_cur_output++]=(int8)((ch_tmp_byte & 0x03 | 0x80) | (pSrc[i] & 0x3e) >> 2);
      }
      else
      {
        return; /* ERROR_INSUFFICIENT_BUFFER */
      }
    }
    else /* 3 byte utf8 representation */
    {
      if ( i_cur_output+3<destLen )
      {
        pDest[i_cur_output++]=(int8)(pSrc[i] >> 12 | 0xe0);
        pDest[i_cur_output++]=(int8)(pSrc[i] >> 6  & 0x3f | 0x80);
        pDest[i_cur_output++]=(int8)(pSrc[i] & 0x3f | 0x80);
      }
      else
      {
        return; /* ERROR_INSUFFICIENT_BUFFER */
      }
    } 
  }  
  pDest[i_cur_output] = '\0';
}

/*===========================================================================
FUNCTION:
  GetByteFromBitStream

DESCRIPTION:
  This function gets given number of bits from the given bit of source in the
  destination byte.

INPUT/OUTPUT PARAMETERS:
  uint8   *pByte      destination byte
  uint8   *pSrc       source stream of bits
  int      nFromBit   from which bit of source
  int      nBits      number of bits to copy in byte (it should not be more than 8)

  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
===========================================================================*/
void GetByteFromBitStream(uint8 *pByte, uint8 *pSrc, int nFromBit, int nBits)
{
  int a, b, i;
  uint8 temp;

  if(nBits > 8)
    return;

  *pByte = 0;       /* reset all the bits */

  for( i=0; i<nBits; i++)
  {
    a = nFromBit/8;
    b = nFromBit%8;
    *pByte = *pByte << 1;     /* make space for next bit */
    temp = pSrc[a] << b;
    *pByte |= temp >> 7;      /* OR after masking all other bits */
    nFromBit++;
  }
}
#ifdef FEATURE_QTV_WMA_PRO_DECODER
/*===========================================================================

FUNCTION  ASFFile::UpdateDecoderTrackInfo

DESCRIPTION
  If any of the audio track in given WM clip is WMA PRO,
  or QTV is being configured to use ARM based decoder for all WMA,
  updates 'm_bUseARMDecoder' to be 'true'
  Once 'm_bUseARMDecoder' is marked as TRUE, WMV can't be played.

===========================================================================*/
void ASFFile::UpdateDecoderTrackInfo()
{
  if(m_hASFDecoder)
  {
    int nAudioTracks = ((WMFDecoderEx *)m_hASFDecoder)->tHeaderInfo.wNoOfAudioStreams;

    //No WMA PRO/PRO+ for streaming. Make sure we don't select such track while streaming.
    for(int j = 0; j < nAudioTracks; j++)
    {
      //Check version/format to see if the track is PRO track

      if( ( ((WMFDecoderEx *)m_hASFDecoder)->tAudioStreamInfo[j])->nVersion == WMA_PRO_TRACK_VERSION)
      {
        m_bUseARMDecoder = true;
        break;
      }
      #ifdef FEATURE_QTV_ONLY_ARM_BASED_WMA
      {
        //When FEATURE_QTV_ONLY_ARM_BASED_WMA is defined, we have to use ARM decoder for all WMA.
        m_bUseARMDecoder = true;
        break;
      }
      #endif
    }
    #ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
      pwfioOut = NULL;
    #endif
  }
}
#endif /* FEATURE_QTV_WMA_PRO_DECODER */

#if defined(FEATURE_QTV_WMA_PRO_DECODER) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
/*===========================================================================

FUNCTION  ASFFile::isWmaProDecoderNeeded

DESCRIPTION
  Returns true if given trackId represents PRO/Pro+ track
  or QTV is being configured to use ARM based decoder for all WMA, Otherwise,returns false.

===========================================================================*/
bool ASFFile::isWmaProDecoderNeeded(uint32 trackid,uint8* format)
{
  if(m_hASFDecoder)
  {
    int nAudioTracks = ((WMFDecoderEx *)m_hASFDecoder)->tHeaderInfo.wNoOfAudioStreams;

    for(int j = 0; j < nAudioTracks; j++)
    {
      if(
          ((((WMFDecoderEx*)m_hASFDecoder)->tAudioStreamInfo[j])->wStreamId == trackid) &&
          ( ( ((WMFDecoderEx *)m_hASFDecoder)->tAudioStreamInfo[j])->nVersion == WMA_PRO_TRACK_VERSION)
        )
      {
        if(format)
        {
          *format = (uint8)WM_PRO_AUDIO;
#ifdef FEATURE_QTV_WMA_PRO_DECODER
           m_isDSPWMA_pro = false;
#elif defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
           m_isDSPWMA_pro = true;
#endif
          //One we have the content, set format based on FORMAT TAG.

          /*if( ( ((WMFDecoderEx *)m_hASFDecoder)->tAudioStreamInfo[j])->nVersion == WMA_PRO_TRACK_VERSION)
          format = (uint8)WM_PRO_AUDIO;
          else
          format = (uint8)WM_PRO_PLUS_AUDIO;*/
        }
        return true;
      }
      #ifdef FEATURE_QTV_ONLY_ARM_BASED_WMA
      //Make sure track id passed in is valid audio track id.
      if((((WMFDecoderEx*)m_hASFDecoder)->tAudioStreamInfo[j])->wStreamId == trackid)
      {
        //When FEATURE_QTV_ONLY_ARM_BASED_WMA is defined, we have to use ARM decoder for all WMA.
        if(format)
        {
          //*format = (uint8)WM_PRO_AUDIO;
        }
        return true;
      }
      #endif
    }
  }
  return false;
}
#endif /* defined(FEATURE_QTV_WMA_PRO_DECODER) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */
/*===========================================================================

FUNCTION  ParseMetaData

DESCRIPTION
  Public method used to parse the meta-data.

===========================================================================*/
tWMCDecStatus ASFFile::ParseMetaData ( void )
{
  uint32 dwAudioSamplesPerFrame = 0;
  bool bAudioStreamSelected = false;
  bool bVideoStreamSelected = false;
  tWMCDecStatus wmerr = WMCDec_Succeeded;

#ifdef CHECK_WMA_FOR_BITRATE
  uint32 MaxBitrate = 0;
#endif

#ifdef FEATURE_QTV_WMA_PRO_DECODER
  m_bUseARMDecoder   = false;
#endif


  //Don't read SIMPLE INDEX OBJECT for audio instance or when streaming.
  //This will reduce the parsing time significantly.
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  wmerr =  WMCDecInit (&m_hASFDecoder, RGB24_WMV, (m_bStreaming || bHttpStreaming || m_playAudio) ? TRUE_WMC : FALSE_WMC, FALSE_WMC, FALSE_WMC);
  if(bHttpStreaming)
  {
    //For Pull interface, register the OEM function.
    ((WMFDecoderEx *)m_hASFDecoder)->fpFetchBufferedDataSize = m_fpFetchBufferedDataSize;
    ((WMFDecoderEx *)m_hASFDecoder)->pQtvPlayerhandle = m_QtvInstancehandle;
  }
  #ifdef FEATURE_QTV_DRM_DCF
    ((WMFDecoderEx *)m_hASFDecoder)->nUserData = (U32_WMC)m_inputStream;
  #else
    ((WMFDecoderEx *)m_hASFDecoder)->nUserData = 0;
  #endif
#else
  wmerr =  WMCDecInit (&m_hASFDecoder, RGB24_WMV, (m_bStreaming || m_playAudio) ? TRUE_WMC : FALSE_WMC, FALSE_WMC, FALSE_WMC);
#endif

      if(wmerr == WMCDec_Succeeded)
      {
        /* get file header information */
        if(WMCDecGetHeaderInfo (m_hASFDecoder, &m_strHeaderInfo) == WMCDec_Succeeded)
        {
          /* get basic stream information */
          if( WMCDecGetStreamInfo(m_hASFDecoder, &m_nNumStreams, &m_ptMediaTypeStreams)==WMCDec_Succeeded )
          {
            /* initialize stream pattern and also limit max munber of streams */
            m_pStreamDecodePattern = (tStreamIdPattern_WMC*) QTV_Malloc(m_nNumStreams*sizeof(tStreamIdPattern_WMC));
            if( m_pStreamDecodePattern == NULL)
            {
                QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Memory allocation is failed for m_pStreamDecodePattern" );
                _success = false;
                wmerr = WMCDec_Fail;
                return wmerr;
            }

            #ifdef FEATURE_QTV_WMA_PRO_DECODER
            //m_bUseARMDecoder is set to TRUE/FALSE in following function.
            UpdateDecoderTrackInfo();
            #endif

            for (uint32 i=0; i<(int)m_nNumStreams; i++ )
            {
              switch(m_ptMediaTypeStreams[i].MediaType)
              {
                case Audio_WMC:
                  WMCDecGetAudioInfo (m_hASFDecoder, m_ptMediaTypeStreams[i].wStreamId, &m_strAudioInfo);

                  m_pStreamDecodePattern[i].wStreamId = m_ptMediaTypeStreams[i].wStreamId;
                  dwAudioSamplesPerFrame = getAudioSamplesPerFrame(m_ptMediaTypeStreams[i].wStreamId);

                  #ifdef CHECK_WMA_FOR_BITRATE
                    MaxBitrate = GetMaximumBitRateForTrack(m_ptMediaTypeStreams[i].wStreamId);
                  #endif

                if ( (((!bAudioStreamSelected)&&(m_playAudio)) || ((GetTotalNumberOfAudioStreams()) &&(!GetTotalNumberOfVideoStreams())))
                       #ifdef CHECK_WMA_FOR_BITRATE
                         && (MaxBitrate < MAX_AUDIO_BIT_RATE_ALLOWED)
                       #endif
                     )
                  {
                    if( (i<FILE_MAX_MEDIA_STREAMS) && (dwAudioSamplesPerFrame > 0)&&(!bAudioStreamSelected) )
                    {
                      #ifdef FEATURE_QTV_WMA_PRO_DECODER
                        //We want to select the PRO audio track if available.
                        if(m_bUseARMDecoder )
                        {
                          if( isWmaProDecoderNeeded(m_ptMediaTypeStreams[i].wStreamId) )
                          {
                            //Current audio track is WMA PRO,
                            //select and mark as Decompressed to use ARM base decoder.
                            m_pStreamDecodePattern[i].tPattern = Decompressed_WMC;
                            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Using ARM Decoder for WMA TrackId %d",m_ptMediaTypeStreams[i].wStreamId);
                            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Max Bit-Rate %ld",GetMaximumBitRateForTrack(m_ptMediaTypeStreams[i].wStreamId));
                          }
                          else
                          {
                            //This will happen if clip has more than 1 audio tracks
                            //and one of them is PRO. Do not select non pro track in such case.
                            continue;
                          }
                        }
                        else
                        {
                          m_pStreamDecodePattern[i].tPattern = Compressed_WMC;
                        }
                      #else
                        m_pStreamDecodePattern[i].tPattern = Compressed_WMC;
                      #endif

                      bAudioStreamSelected = true;

                      //Always set the correct stream ID when the stream is selected for playback.
                      //For some clips, even though clip has only one stream,stream number is not one.
                      //CR86660//
                      m_nSelectedAudioStreamId = (int16)m_pStreamDecodePattern[i].wStreamId;

                      //Do not need the buffer if we will be using ARM based decoder.
                      if(m_pStreamDecodePattern[i].tPattern == Compressed_WMC)
                      {
#ifndef OUTPUT_STANDARD_WMA_BITSTREAM
                        m_nAudioPacketBufferSize = getTrackMaxBufferSizeDB(m_ptMediaTypeStreams[i].wStreamId);
                        m_pAudioPacketBuffer = (uint8*)QTV_Malloc(m_nAudioPacketBufferSize);
#endif
                      }
                      /* calculate Num of prev bit length (sec 3.6/3.7 of WMA doc) */
                      float bits_per_sample = ((float)m_strAudioInfo.u32BitsPerSecond) /
                                        (m_strAudioInfo.u32SamplesPerSecond*m_strAudioInfo.u16NumChannels);

                      uint32 round_value    = ROUNDF( bits_per_sample *
                                                          getAudioSamplesPerFrame(m_ptMediaTypeStreams[i].wStreamId)
                                                      / 8.0F );
                      if (round_value == 0)
                      {
                        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "round_value is 0!!");
                        return WMCDec_BadData;
                      }

                      m_nWmaNumPrevFrameBits = LOG2(round_value) + 2 + 3;
                       AUDIOSTREAMINFO* pAudioStrmPtr = GetAudioStreamInfo(m_ptMediaTypeStreams[i].wStreamId);
                       m_bAllowWmaPackets = (pAudioStrmPtr && (pAudioStrmPtr->nEncodeOpt & 0x02))?true:false;
                       #ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
                         uint8 format = 0xFF;
                         // Sets m_isDSPWMA_pro to true if wma pro clips are decoded using DSP
                         (void)isWmaProDecoderNeeded(m_ptMediaTypeStreams[i].wStreamId, &format);
                       #endif
                       if(m_isDSPWMA_pro)
                       {
                           m_bAllowWmaPackets = true;
                       }
                    }
                    else
                      m_pStreamDecodePattern[i].tPattern = Discard_WMC;
                  }
                  else
                  {
                    m_pStreamDecodePattern[i].tPattern = Discard_WMC;
                    #ifdef CHECK_WMA_FOR_BITRATE
                      if( MaxBitrate > MAX_AUDIO_BIT_RATE_ALLOWED )
                    {
                         _fileErrorCode = TRACK_AUDIO_UNSUPPORTED_BITRATE;
                        QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Error:Unsupported Max/Avg Audio BitRate %ld (MAX Allowed is %ld)",MaxBitrate,MAX_AUDIO_BIT_RATE_ALLOWED);
                        return false;
                    }
                    #endif
                  }
                  break;

                case Video_WMC:
                  WMCDecGetVideoInfo (m_hASFDecoder, m_ptMediaTypeStreams[i].wStreamId, &m_strVideoInfo);
                  m_pStreamDecodePattern[i].wStreamId = m_ptMediaTypeStreams[i].wStreamId;

                   if( ( ((!bVideoStreamSelected)&&(m_playVideo))||
                         ((!GetTotalNumberOfAudioStreams()) && (GetTotalNumberOfVideoStreams())) ) )

                  {
                    //We don't want to select any video track if we will be using ARM based decoder.
                    if( (i<FILE_MAX_MEDIA_STREAMS) && (!bVideoStreamSelected)
                          #ifdef FEATURE_QTV_WMA_PRO_DECODER
                                                   && (! m_bUseARMDecoder)
                          #endif
                      )
                    {
                      if(!isVideoCodecSupported())
                      {
                         _success = false;
                        wmerr = WMCDec_Fail;
                        return wmerr;
                      }
                      m_pStreamDecodePattern[i].tPattern = Compressed_WMC;
                      bVideoStreamSelected = true;

                      //Always set the correct stream ID when the stream is selected for playback.
                      //For some clips, even though clip has only one stream,stream number is not one.
                      //CR86660//
                      m_nSelectedVideoStreamId = (int16)m_pStreamDecodePattern[i].wStreamId;

                    }
                    else
                      m_pStreamDecodePattern[i].tPattern = Discard_WMC;
                  }
                  else
                  {
                    m_pStreamDecodePattern[i].tPattern = Discard_WMC;
                  }
                  break;

                case Binary_WMC:
                  WMCDecGetBinaryInfo (m_hASFDecoder, m_ptMediaTypeStreams[i].wStreamId, &m_strBinaryInfo);
                  m_pStreamDecodePattern[i].wStreamId = m_ptMediaTypeStreams[i].wStreamId;

                  //Always set the correct stream ID when the stream is selected for playback.
                  //For some clips, even though clip has only one stream,stream number is not one.
                  //CR86660//
                  m_nSelectedBinaryStreamId = (int16)m_pStreamDecodePattern[i].wStreamId;

                  /* fall through to default */
                default:
                  m_pStreamDecodePattern[i].tPattern = Discard_WMC;
                  break;
              } /* end of switch(m_ptMediaTypeStreams[i].MediaType) */
            } /* end of for (i=0; i<(int)m_nNumStreams; i++ ) */

            if(m_nNumStreams > FILE_MAX_MEDIA_STREAMS)
            {
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Clip has %d streams. Limiting to %d", m_nNumStreams, FILE_MAX_MEDIA_STREAMS);
              m_nNumStreams = FILE_MAX_MEDIA_STREAMS;
            }

            wmerr = WMCDecSetDecodePatternForStreams (&m_hASFDecoder, m_pStreamDecodePattern);
            if (wmerr == WMCDec_Succeeded)
            {
              if(WMCDecGetContentDesc (m_hASFDecoder, &m_pContentDesc)==WMCDec_Succeeded)
              {
                _success = true;
                _fileErrorCode = EVERYTHING_FINE;
                wmerr = WMCDec_Succeeded;
              }
            }
          } /* end of if( WMCDecGetStreamInfo(...)==WMCDec_Succeeded ) */
        } /* end of if(WMCDecGetHeaderInfo (...) == WMCDec_Succeeded) */
      } /* end of if(WMCDecInit == WMCDec_Succeeded) */

    //This buffer will be used to send full complete data for all audio frames
    //in a given ASF packet. We assume that last frame data can be spilled only in to the next ASF packet.
    //Allocate memory only for audio instance.
    if(m_playAudio)
    {
      if(m_nSelectedAudioStreamId != -1)
      {
         #ifdef FEATURE_QTV_WMA_PRO_DECODER
        if(m_bUseARMDecoder)
        {
            PCMFormat2WaveFormatEx(&(((WMFDecoderEx*)m_hASFDecoder)->sPCMFormat),
                                   &(m_sWAVEFormatEx.Format));
        }
        else
        #endif
        {
#ifndef OUTPUT_STANDARD_WMA_BITSTREAM
          m_nAsfDataBufferSize = 2 * GetAsfPacketSize();
          m_pasfDataBuffer = (U8_WMC*)QTV_Malloc( m_nAsfDataBufferSize);
          if (m_pasfDataBuffer == NULL)
          {
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, 
                          "m_pasfDataBuffer malloc failed size = %ld",m_nAsfDataBufferSize);
            return WMCDec_BadMemory;
          }

          memset(m_pasfDataBuffer,0,m_nAsfDataBufferSize);
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "m_nAsfDataBufferSize %ld",m_nAsfDataBufferSize);
#endif
        }
      }
      else
      {
        m_pasfDataBuffer = NULL;
      }
    }
    else
      m_pasfDataBuffer = NULL;
  if( wmerr == WMCDec_Succeeded )
     {
       #if defined (FEATURE_QTV_DRM_DCF) && defined (FEATURE_SME_DRMMS)
      if (m_inputStream)
      {
        if( ((IxStreamMedia*)m_inputStream)->IsProtected() )
        {
          #ifdef FEATURE_QTV_WM_DRM_API
            ((WMFDecoderEx *)(m_hASFDecoder))->pDRMDecryptFunction = WMDRMDecrypt;
          #endif
        }
      }
    #endif /*FEATURE_SME_DRMMS*/
  }
     return wmerr;

}

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
/*===========================================================================

FUNCTION  updateBufferWritePtr

DESCRIPTION
  Public method used to update the write buffer offset during Http streaming.

===========================================================================*/
void ASFFile::updateBufferWritePtr ( uint32 writeOffset )
{
  WMFDecoderEx *pDecoder = (WMFDecoderEx *)m_hASFDecoder;
  //Executing in the UI thread context.

  if(pDecoder)
  {
    pDecoder->wHttpDataBuffer.Offset = writeOffset;
    pDecoder->wHttpDataBuffer.bValid = TRUE;
  }

  if((parserState == Common::PARSER_PAUSE) || (parserState == Common::PARSER_RESUME))
  {
     //check if we got sufficient data to start parsing the
     //meta data.
      sendParseHTTPStreamEvent();
  }
}
/*===========================================================================

FUNCTION  sendParserEvent

DESCRIPTION
  Public method used send parser events

===========================================================================*/
void ASFFile::sendParserEvent(Common::ParserStatusCode status)
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

/*===========================================================================

FUNCTION  getMetaDataSize

DESCRIPTION
  Public method used to determine the meta-data size of the fragment.

===========================================================================*/
tWMCDecStatus ASFFile::getMetaDataSize ( void )
{
  tWMCDecStatus wmerr = WMCDec_Fail;
  uint32 nHttpDownLoadBufferOffset = 0;
  boolean bEndOfData = false;
  boolean bHttpDownLoadBufferOffsetValid = GetHTTPStreamDownLoadedBufferOffset(&nHttpDownLoadBufferOffset,bEndOfData );
  WMFDecoderEx *pDecoder = (WMFDecoderEx *)m_hASFDecoder;
  U32_WMC nAsfHeaderSize = 0;
  if( pDecoder && bHttpDownLoadBufferOffsetValid && (nHttpDownLoadBufferOffset > (MIN_OBJECT_SIZE + sizeof(U32_WMC) + 2*sizeof(U8_WMC)) ) )
  {
    wmerr = GetAsfHeaderSize(&m_hASFDecoder,&nAsfHeaderSize );
  }
  if(wmerr == WMCDec_Succeeded)
  {
    m_HttpDataBufferMinOffsetRequired.Offset = nAsfHeaderSize;
    bGetMetaDataSize = FALSE;
  }
  else
  {
    bGetMetaDataSize = TRUE;
  }
  return wmerr;
}

/*===========================================================================

FUNCTION  CanPlayTracks

DESCRIPTION
  Public method used to switch contexts and notify the player about buffer-underrun.

===========================================================================*/
bool ASFFile::CanPlayTracks(uint32 nTotalPBTime)
{
  uint32 nMinBufferingTime = 0;
  uint32 nMediaMaxPlayableTime = 0;
  uint32  nTotalAvgBitRate = 0;
  uint32 nHttpDownLoadBufferOffset = 0;
  boolean bEndOfData = false;
  boolean bHttpDownLoadBufferOffsetValid = GetHTTPStreamDownLoadedBufferOffset(&nHttpDownLoadBufferOffset,bEndOfData);

  if (bEndOfData)
  {
     return true;
  }
  if( (m_HttpDataBufferMinOffsetRequired.bValid == FALSE) && GetTotalAvgBitRate( &nTotalAvgBitRate ) )
  {
    /* Compute Required Buffering/Rebuffering time */

    (void)GetMediaMaxPlayableTime( &nMediaMaxPlayableTime );

    if(nTotalPBTime > nMediaMaxPlayableTime)
    {
      nMinBufferingTime = nTotalPBTime - nMediaMaxPlayableTime;
    }
    else
    {
      nMinBufferingTime = 0;
    }

    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "nMinBufferingTime %d", nMinBufferingTime);
    if( !nMinBufferingTime )
    {
      /* nBufferingTime from Mpeg4Plyer/OEM is '0', then take it as Preroll time */
      nMinBufferingTime = getPrerollTime();
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Taking Preroll as Rebuffering Time: nTotalPBTime= %d, nMinBufferingTime= %d", nTotalPBTime, nMinBufferingTime);
    }
    nMinBufferingTime = ZMAX(MIN_PREROLL_REQUIRED,nMinBufferingTime);
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Updated nMinBufferingTime after taking ZMAX to %d", nMinBufferingTime);

    /* Estimate approximate minimum offset required */
    m_HttpDataBufferMinOffsetRequired.Offset = nHttpDownLoadBufferOffset +  (nTotalAvgBitRate * nMinBufferingTime)/8000;
    m_HttpDataBufferMinOffsetRequired.bValid = TRUE;
    QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "calculated m_HttpDataBufferMinOffsetRequired.Offset %d m_HttpDataBufferMinOffsetRequired.bValid %d", m_HttpDataBufferMinOffsetRequired.Offset,m_HttpDataBufferMinOffsetRequired.bValid);

    //When broadcast flag is set, last packet offset is not valid, so we should not use it
    //in calculating required data offset.
    if( (m_hASFDecoder) && (!( ((WMFDecoderEx *)(m_hASFDecoder))->tHeaderInfo.dwFlags & 0x1)) )
    {
      if( m_HttpDataBufferMinOffsetRequired.Offset > (GetLastPacketOffset() + GetAsfPacketSize()) )
      {
         m_HttpDataBufferMinOffsetRequired.Offset =  (GetLastPacketOffset() + GetAsfPacketSize());
         QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "updated m_HttpDataBufferMinOffsetRequired.Offset %d", m_HttpDataBufferMinOffsetRequired.Offset);
      }
    }

  }

  if((nHttpDownLoadBufferOffset >= m_HttpDataBufferMinOffsetRequired.Offset) &&
      bHttpDownLoadBufferOffsetValid && m_HttpDataBufferMinOffsetRequired.bValid )
  {
    return true;
  }
  return false;
}

/*===========================================================================

FUNCTION  parseHTTPStream

DESCRIPTION
  Public method used to parse the Http Stream.

===========================================================================*/
bool ASFFile::parseHTTPStream ( void )
{

  tWMCDecStatus wmerr = WMCDec_Succeeded;
  bool returnStatus = true;
  uint32 nHttpDownLoadBufferOffset = 0;

  bool bPLaybackStarted = true;

  //GetHTTPStreamDownLoadedBufferOffset returns amount of data downloaded so far.
  //With PULL interface, value is fetched with fetch data function, for PULL interface, it is same as pDecoder->wHttpDataBuffer.Offset
  boolean bEndOfData = false;
  boolean bHttpDownLoadBufferOffsetValid = GetHTTPStreamDownLoadedBufferOffset(&nHttpDownLoadBufferOffset,bEndOfData);

  if(bGetMetaDataSize)
  {
     wmerr = getMetaDataSize();
     //When getMetaDataSize returns TRUE,
     //m_HttpDataBufferMinOffsetRequired.Offset is updated with return of GetAsfHeaderSize().
  }
  if(wmerr != WMCDec_Succeeded)
  {
    //QTV_PS_PARSER_STATUS_PAUSED
    sendParserEvent(Common::PARSER_PAUSE);
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "getMetaDataSize failed, notifying Common::PARSER_PAUSE");
    returnStatus = false;
  }
  else if(wmerr == WMCDec_Succeeded)
  {
     if((nHttpDownLoadBufferOffset >= m_HttpDataBufferMinOffsetRequired.Offset)&&
        (bHttpDownLoadBufferOffsetValid)                                       &&
        (m_HttpDataBufferMinOffsetRequired.bValid || (!bIsMetaDataParsed)) )
     {
       if( !bIsMetaDataParsed )
       {

         //If meta-data is not parsed, parsed it and set the flag.
         //This will be set once for each playback and will never be reset.

         if(ParseMetaData() == WMCDec_Succeeded)
         {
           bIsMetaDataParsed = TRUE;
           bPLaybackStarted = false;
           //Even though we have meta-data, we may not have sufficient bytes to begin the playback.
           //This will be checked via CanPlayTracks. Please see below.
           UpdateTrackIdInFilePointer();
         }
            else
            {
           //Data not sufficient: Notify
           //QTV_PS_PARSER_STATUS_PAUSED
           sendParserEvent(Common::PARSER_PAUSE);
           bPLaybackStarted = false;
           QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "not enough data to parse meta-data,notifying Common::PARSER_PAUSE");
           returnStatus = false;
            }
       }
     }
     //Check if we have sufficient data. When m_startupTime is 0,
     //we use Pre-roll time of the clip.
     if(CanPlayTracks(m_startupTime))
     {
       if(!bPLaybackStarted)
       {
         //We are starting for the first time for given clip.
         QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "parseHTTPStream, canPlayTracks() returned TRUE bPLaybackStarted %d",bPLaybackStarted);
         sendParserEvent(Common::PARSER_RESUME);
       }
       if(parserState == Common::PARSER_PAUSE)
       {
         QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "parseHTTPStream, canPlayTracks() returned TRUE sending Common::PARSER_RESUME");
         sendParserEvent(Common::PARSER_RESUME);
       }
       if(parserState == Common::PARSER_RESUME)
       {
         QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "parseHTTPStream, canPlayTracks() returned TRUE sending Common::PARSER_READY");
         sendParserEvent(Common::PARSER_READY);
       }
       returnStatus = true;
     }
     else
     {
       if((!bPLaybackStarted)||(!bIsMetaDataParsed))
       {
         /*
         * bIsMetaDataParsed is initialized to FALSE in the ASFFILE constructor.
         * When we parse the meta-data for the first time, we set it to TRUE.
         * This will remain TRUE for rest of the CLIP.
         *
         * bPLaybackStarted is initialized to 'true' when we enter ASFFILE::ParseHTTPStream.
         * If we have not parsed the meta-data yet(bIsMetaDataParsed is FALSE), bPLaybackStarted is set to FALSE.
         *
         * It is possible that even though we have the clip meta-data, it is not sufficient to start playing based on
         * pre-roll time. AsfFile::CanPlayTracks will determine whether we have sufficient data or not.
         * When there is no sufficient data and we haven't started playback yet,(bPLaybackStarted is FALSE),
         * we need to report Common::PARSER_PAUSE;
         */
         sendParserEvent(Common::PARSER_PAUSE);
         QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "bPLaybackStarted is FALSE, notifying Common::PARSER_PAUSE before returing FALSE from AsfFile::CanPlayTracks");
       }
       returnStatus = false;
     }
  }
  return returnStatus;
}

/*===========================================================================

FUNCTION  GetHTTPStreamDownLoadedBufferOffset

DESCRIPTION
  Public method used to switch contexts and notify the player about buffer-underrun.

===========================================================================*/
boolean ASFFile::GetHTTPStreamDownLoadedBufferOffset(U32_WMC * pOffset, boolean &bEod)
{
  WMFDecoderEx *pDecoder = (WMFDecoderEx *)m_hASFDecoder;

  if(pDecoder && pOffset)
  {
    if(m_fpFetchBufferedDataSize)
    {
      boolean bEndOfData = false;
      U32_WMC userData = 0;
      #ifdef FEATURE_QTV_DRM_DCF
        userData = (U32_WMC)m_inputStream;
      #endif
      //Pull interface so pull dnld data size from OEM
      m_fpFetchBufferedDataSize( userData ,&(pDecoder->wHttpDataBuffer.Offset),&bEndOfData, m_QtvInstancehandle);
      pDecoder->wHttpDataBuffer.bValid = TRUE;
      bEod = bEndOfData;
    }
    if( pDecoder->wHttpDataBuffer.bValid )
    {
      *pOffset = pDecoder->wHttpDataBuffer.Offset;
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION  GetTotalAvgBitRate

DESCRIPTION
  Public method used to switch contexts and notify the player about buffer-underrun.

===========================================================================*/
bool ASFFile::GetTotalAvgBitRate(U32_WMC * pBitRate)
{
  if( !m_nNumStreams || !m_ptMediaTypeStreams)
  {
    return false;
  }

  *pBitRate = 0;

  for(uint8 index = 0; index < m_nNumStreams; index++)
  {
     /* the Movie average bitrate from sum of all the tacks average bitrate */
     if(m_ptMediaTypeStreams)
     {
       *pBitRate = *pBitRate + getTrackAverageBitrate(m_ptMediaTypeStreams[index].wStreamId);
     }
  }
  return true;
}
/* ======================================================================
FUNCTION:
  ASFFile::GetMediaMaxTimeStampPlayable

DESCRIPTION:
  gets time stamp of current sample of the track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 time stamp in track time scale unit

SIDE EFFECTS:
  None.
======================================================================*/
tWMCDecStatus ASFFile::GetMediaMaxPlayableTime(U32_WMC *nMaxPBTime)
{
  uint32 nMaxPlayableTime = 0;
  tWMCDecStatus err = WMCDec_InValidArguments;

  if( (m_pStreamDecodePattern == NULL) || (nMaxPBTime == NULL) )
  {
    return err;
  }
  for(uint16 i=0; i<(int)m_nNumStreams; i++)
  {
    if(m_pStreamDecodePattern[i].tPattern  != Discard_WMC)
    {
      if(!nMaxPlayableTime)
      {
        /* initialize with valid track sample time */
        nMaxPlayableTime = m_maxPlayableTime[i];
        err = WMCDec_Succeeded;
        continue;
      }
      /* Take the MIN value to make sure all tracks are playable atleast nMaxPlayableTime */
      nMaxPlayableTime = MIN(m_maxPlayableTime[i],nMaxPlayableTime);
    }
  }
  if(err == WMCDec_Succeeded)
  {
    *nMaxPBTime = nMaxPlayableTime;
  }
  return err;
}
#endif//#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
/*===========================================================================

FUNCTION  ASFFile::parsePseudoStream

DESCRIPTION
  Called from Mpeg4Player when-ever data is pushed or need to be PULLED.

===========================================================================*/
bool ASFFile::parsePseudoStream( void )
{
  return parseHTTPStream();
}
#endif//#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)



#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD ||defined FEATURE_QTV_PSEUDO_STREAM
/*===========================================================================

FUNCTION  sendHTTPStreamUnderrunEvent

DESCRIPTION
  Public method used to switch contexts and notify the player about buffer-underrun.

===========================================================================*/
void ASFFile::sendHTTPStreamUnderrunEvent(void)
{
  sendParserEvent(Common::PARSER_PAUSE);
}
/*===========================================================================

FUNCTION  sendParseHTTPStreamEvent

DESCRIPTION
  Public method used to switch contexts and call the parseHttpStream.

===========================================================================*/
void ASFFile::sendParseHTTPStreamEvent(void)
{
  QTV_HTTP_BUFFER_UPDATE_type *pEvent = QCCreateMessage(QTV_HTTP_BUFFER_UPDATE, m_pMpeg4Player);

  if (pEvent)
  {
    pEvent->bHasAudio = m_playAudio ;
    pEvent->bHasVideo = m_playVideo;
    pEvent->bHasText  = m_playText;
    QCUtils::PostMessage(pEvent, 0, NULL);
  }
}
#endif //#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

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
bool ASFFile::RegisterDRMDecryptMethod(QtvPlayer::DRMDecryptMethodT pDRMDecryptFunction, void *pClientData)
{
  WMFDecoderEx *pDecoder = (WMFDecoderEx *)(m_hASFDecoder);

  if(pDecoder)
  {
    pDRMClientData = pClientData;
    pDRMDecryptFunction = (QtvPlayer::DRMDecryptMethodT)pDRMDecryptFunction;
    pDecoder->pDRMDecryptFunction = WMDRMDecrypt;
    return true;
  }
  return false;
}
/* ======================================================================
FUNCTION
  ASFFile::DRMDecryptSwitch

DESCRIPTION
  Decides which Decrypt function to call between IxStreamMedia or OEMs Decrypt Functions.

DEPENDENCIES

RETURN VALUE
  TRUE if successful, else FALSE

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
#if defined (FEATURE_QTV_DRM_DCF) && defined (FEATURE_SME_DRMMS)
bool ASFFile::DRMDecryptSwitch(uint32 wTrackID,           /* to which track this data belong to */
                               void *pEncryptedDataBuf,   /* pointer to encrypted data buffer, which has to be decrypted */
                               void *pDecryptedDataBuf,   /* pointer to destination buffer to copy decrypted data,
                                                             OEM is resposible for copying the decrypted data  */
                               uint32 wEncryptedDataSize, /* encrypted data buffer size */
                               uint32 *pDecryptedDataSize)/* pointer to decrypted data buffer size,
                                                             OEM is resposible for copying the decrypted data size  */
#elif defined FEATURE_QTV_WM_DRM_API
bool ASFFile::DRMDecryptSwitch(uint32 wTrackID,                   /* to which track this data belong to */
                               void *pEncryptedDataBuf,   /* pointer to encrypted data buffer, which has to be decrypted */
                               void *pDecryptedDataBuf,   /* pointer to destination buffer to copy decrypted data,
                                                             OEM is resposible for copying the decrypted data  */
                               uint32 wEncryptedDataSize, /* encrypted data buffer size */
                               uint32 *pDecryptedDataSize)/* pointer to decrypted data buffer size,
                                                             OEM is resposible for copying the decrypted data size  */
#else
bool ASFFile::DRMDecryptSwitch(uint32 ,                   /* to which track this data belong to */
                               void *,                    /* pointer to encrypted data buffer, which has to be decrypted */
                               void *,                    /* pointer to destination buffer to copy decrypted data,
                                                             OEM is resposible for copying the decrypted data  */
                               uint32 ,                   /* encrypted data buffer size */
                               uint32 *)                  /* pointer to decrypted data buffer size,
                                                             OEM is resposible for copying the decrypted data size  */
#endif
{

  {
    #if defined (FEATURE_QTV_DRM_DCF) && defined (FEATURE_SME_DRMMS)
    if(m_inputStream)
    {
      IxErrnoType result = ((IxStreamMedia*)(m_inputStream))->Decrypt(wTrackID,
                                                             (uint8*)pEncryptedDataBuf,
                                                             wEncryptedDataSize,
                                                             (uint8*)pDecryptedDataBuf,
                                                             pDecryptedDataSize);
      if(result == E_SUCCESS)
      {
        return true;
      }
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Decrypt on IxStreamMedia failed result %d",result);
      return false;
    }
    else
    #endif/*FEATURE_QTV_DRM_DCF && FEATURE_SME_DRMMS */
    if(pDRMDecryptFunction)
    {
      #ifdef FEATURE_QTV_WM_DRM_API
    return (pDRMDecryptFunction(wTrackID,pEncryptedDataBuf, pDecryptedDataBuf, wEncryptedDataSize, pDecryptedDataSize, pDRMClientData )!=0 );
      #endif
    }
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Weird case in ASFFile::DRMDecryptSwitch");
    return false;
  }
}
#endif /* FEATURE_QTV_WM_DRM_API */
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
