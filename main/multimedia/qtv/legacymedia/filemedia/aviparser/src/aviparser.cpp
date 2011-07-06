
/* =======================================================================
                              aviParser.cpp
DESCRIPTION

Copyright 2004 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             PERFORCE HEADER
$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/aviparser/main/latest/src/aviparser.cpp#73 $
========================================================================== */

#include "aviParser.h"

#ifdef FEATURE_QTV_AVI

#include "qcplayer_oscl_utils.h"

#define HTONS(x) \
        ((unsigned short)((((unsigned short)(x) & 0x00ff) << 8) | \
                         (((unsigned short)(x) & 0xff00) >> 8)))
/* =============================================================================
FUNCTION:
 ascii_2_short_int


DESCRIPTION:
 Helper function to convert ascii characters into short int.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
avi_uint16 ascii_2_short_int(avi_uint16* ptr, bool *bResult = NULL)
{
  avi_uint16 no = 0;
  char* data = (char*)ptr;
  bool bOK = false;
  if(bResult)
  {
    *bResult = false;
  }

  for(int i = 0; (data!= 0) && i < sizeof(avi_uint16);i++)
  {
    if((data[i] >= '0')&&(data[i]<='9'))
    {
      no = ( (10*no) + (data[i]-'0'));
      if(bResult)
      {
        *bResult = true;
      }
      bOK = true;
    }
  }
  if(!bOK)
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,"ascii_2_short_int could not locate ASCII CHARS *ptr %x",(*ptr));
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,"trackId returned not correct,PLEASE CHECK..");
    no = (avi_uint16)-1;
  }
  return no;
}

/* =============================================================================
FUNCTION:
  aviParser::aviParser

DESCRIPTION:
  Class constructor

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviParser::aviParser(avi_uint32 pUData,avi_uint64 fileSize,bool bDiscardAudIndex,bool bDiscardVidIndex)
{
#ifdef AVI_PARSER_DEBUG
  QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                "aviParser::aviParser fileSize %d bDiscardAudIndex %d bDiscardVidIndex %d",
                fileSize,bDiscardAudIndex,bDiscardVidIndex);
#endif
  m_bDRMProtection = false;
  m_nNumOfRiff = 0;
  m_pUserData = pUData;
  m_nCurrentChunkDataSize = m_nCurrOffset = m_nMoviOffset = m_nIdx1Offset = m_nStartOfMovi = 0;
  m_CurrentParserState = AVI_PARSER_IDLE;
  m_nFileSize = fileSize;
  m_bDiscardAudioIndex = bDiscardAudIndex;
  m_bDiscardVideoIndex = bDiscardVidIndex;
  memset(&m_hAviSummary,0,sizeof(avi_summary_info));
  m_nBytesToBeAdjustedForMOVI = 0;
  m_bByteAdjustedForMOVI = m_bisAVIXpresent = false;
  m_VolSize = 0;
  memset(&m_AviClipMetaInfo,0,sizeof(avi_info_struct));
  memset(&m_AviSTRDHeader,0,sizeof(m_AviSTRDHeader));
  m_nMoviSize = m_nIdx1Size = 0;
  m_nAdjustedMoviOffset = 0;
  m_nAdjustedIdx1Offset = 0;
  m_nCurrentSampleInfoOffsetInIdx1 = 0;
  m_nCurrAudioSampleInIdx1 = 0;
  m_nCurrVideoSampleInIdx1 = 0;
  m_pIdx1SeekCache = NULL;
  m_pMultipleRiff = NULL;

  for(int k = 0; k < AVI_MAX_TRACKS;k++)
  {
    memset(&m_hAviSummary.stream_index[k],0,sizeof(m_hAviSummary.stream_index[k]));
    memset(&m_base_indx_tbl[k],0,sizeof(m_base_indx_tbl[k]));
    memset(&m_start_data_offset_avi_index_of_chunks_tbl[k],0,sizeof(m_start_data_offset_avi_index_of_chunks_tbl[k]));

    memset(&m_start_data_offset_base_indx_tbl[k],0,sizeof(m_start_data_offset_base_indx_tbl[k]));
    memset(&m_avi_index_of_chunks_tbl[k],0,sizeof(m_avi_index_of_chunks_tbl[k]));

    m_AviVOLHeader[k] = (memory_struct*)QTV_Malloc(sizeof(memory_struct));
    if(m_AviVOLHeader[k])
    {
      memset(m_AviVOLHeader[k],0,sizeof(memory_struct));
    }
  }

  for(int j = 0; j < AVI_MAX_VIDEO_TRACKS;j++)
  {
    memset(&m_nCurrVideoFrameCount[j],0,sizeof(m_nCurrVideoFrameCount[j]));
    memset(&m_hAviSummary.video_info[j],0,sizeof(m_hAviSummary.video_info[j]));
  }
  for(int k = 0; k < AVI_MAX_AUDIO_TRACKS; k++)
  {
    memset(&m_nCurrAudioPayloadSize[k],0,sizeof(m_nCurrAudioPayloadSize[k]));
    memset(&m_nCurrAudioFrameCount[k] ,0,sizeof(m_nCurrAudioFrameCount[k]));
    memset(&m_hAviSummary.audio_info[k] ,0,sizeof(m_hAviSummary.audio_info[k]));
    memset(&m_nParserAudSampleEndTime[k],0,sizeof(m_nParserAudSampleEndTime[k]));
  }
}
/* =============================================================================
FUNCTION:
 aviParser::~aviParser

DESCRIPTION:
  Class destructor

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviParser::~aviParser()
{
#ifdef AVI_PARSER_DEBUG
  if(m_hAviSummary.pIdx1Table)
  {
    QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                  "aviParser::~aviParser #Audio entries allocated %d #Video entries allocated %d",
                  m_hAviSummary.pIdx1Table->nAudioEntriesAllocated,m_hAviSummary.pIdx1Table->nVideoEntriesAllocated);
  }
#endif
  if(m_hAviSummary.pIdx1Table)
  {
    if( m_hAviSummary.pIdx1Table->pAudioEntries )
    {
      QTV_Free(m_hAviSummary.pIdx1Table->pAudioEntries);
      m_hAviSummary.pIdx1Table->pAudioEntries = NULL;
    }
    if( m_hAviSummary.pIdx1Table->pKeyVideoEntries )
    {
      QTV_Free(m_hAviSummary.pIdx1Table->pKeyVideoEntries);
      m_hAviSummary.pIdx1Table->pKeyVideoEntries = NULL;
    }
    QTV_Free(m_hAviSummary.pIdx1Table);
    m_hAviSummary.pIdx1Table = NULL;
  }
  for(int i =0; i < AVI_MAX_VIDEO_TRACKS;i++)
  {
    if(m_hAviSummary.video_info[i].strnVideo.streamName)
    {
      QTV_Free(m_hAviSummary.video_info[i].strnVideo.streamName);
      m_hAviSummary.video_info[i].strnVideo.streamName = NULL;
    }
  }
  for(int i =0; i < AVI_MAX_AUDIO_TRACKS;i++)
  {
    if(m_hAviSummary.audio_info[i].strnAudio.streamName)
    {
      QTV_Free(m_hAviSummary.audio_info[i].strnAudio.streamName);
      m_hAviSummary.audio_info[i].strnAudio.streamName = NULL;
    }
    if(m_hAviSummary.audio_info[i].strfAudio.extra)
    {
      QTV_Free(m_hAviSummary.audio_info[i].strfAudio.extra);
      m_hAviSummary.audio_info[i].strfAudio.extra = NULL;
    }
  }
  for(int k = 0; k < AVI_MAX_TRACKS; k++)
  {
    if(m_AviVOLHeader[k])
    {
      if(m_AviVOLHeader[k]->pMemory)
      {
        QTV_Free(m_AviVOLHeader[k]->pMemory);
      }
      QTV_Free(m_AviVOLHeader[k]);
    }
  }
  if(m_pIdx1SeekCache && m_pIdx1SeekCache->pMemory)
  {
    QTV_Free(m_pIdx1SeekCache->pMemory);
    QTV_Free(m_pIdx1SeekCache);
  }
  if(m_pMultipleRiff)
  {
    QTV_Free(m_pMultipleRiff);
  }
  if(m_hAviSummary.avih)
  {
    QTV_Free(m_hAviSummary.avih);
    m_hAviSummary.avih = NULL;
  }
  if(m_AviSTRDHeader.drm_info)
  {
    QTV_Free(m_AviSTRDHeader.drm_info);
  }
  if(m_AviClipMetaInfo.ArchLocn.Ptr){
      QTV_Free(m_AviClipMetaInfo.ArchLocn.Ptr);
  }
  if(m_AviClipMetaInfo.Artist.Ptr){
      QTV_Free(m_AviClipMetaInfo.Artist.Ptr);
  }
  if(m_AviClipMetaInfo.Commissioned.Ptr){
    QTV_Free(m_AviClipMetaInfo.Commissioned.Ptr);
  }
  if(m_AviClipMetaInfo.Comments.Ptr){
    QTV_Free(m_AviClipMetaInfo.Comments.Ptr);
  }
  if(m_AviClipMetaInfo.Copyright.Ptr){
    QTV_Free(m_AviClipMetaInfo.Copyright.Ptr);
  }
  if(m_AviClipMetaInfo.CreateDate.Ptr){
    QTV_Free(m_AviClipMetaInfo.CreateDate.Ptr);
  }
  if(m_AviClipMetaInfo.Genre.Ptr){
    QTV_Free(m_AviClipMetaInfo.Genre.Ptr);
  }
  if(m_AviClipMetaInfo.Keyword.Ptr){
    QTV_Free(m_AviClipMetaInfo.Keyword.Ptr);
  }
  if(m_AviClipMetaInfo.Name.Ptr){
    QTV_Free(m_AviClipMetaInfo.Name.Ptr);
  }
  if(m_AviClipMetaInfo.Product.Ptr){
    QTV_Free(m_AviClipMetaInfo.Product.Ptr);
  }
  if(m_AviClipMetaInfo.Subject.Ptr){
    QTV_Free(m_AviClipMetaInfo.Subject.Ptr);
  }
  if(m_AviClipMetaInfo.Software.Ptr){
    QTV_Free(m_AviClipMetaInfo.Software.Ptr);
  }
  if(m_AviClipMetaInfo.Source.Ptr){
    QTV_Free(m_AviClipMetaInfo.Source.Ptr);
  }
  memset(&m_AviClipMetaInfo,0,sizeof(m_AviClipMetaInfo));
  m_CurrentParserState = AVI_PARSER_IDLE;
}
/* =============================================================================
FUNCTION:
  aviParser::parseAVIH

DESCRIPTION:
  Reads and parses AVI header.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseAVIH(avi_uint64* offset)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint32 nSize = 0;

  aviErrorType retError = AVI_READ_FAILURE;
  avi_uint64 offsetAtTheEndOfAVIH = 0;

#ifdef AVI_PARSER_DEBUG
  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                "parseAVIH Offset to start %d",*offset);
#endif
  if(!AVICallbakGetData(*offset,2 * sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseAVIH AVI_READ_FAILURE");
    return retError;
  }
  *offset+= (2 *sizeof(fourCC_t));
  if(!memcmp(byteData,AVI_AVIH_FOURCC,sizeof(fourCC_t)))
  {
    memcpy(&nSize,byteData+4,sizeof(fourCC_t));
    offsetAtTheEndOfAVIH = (*offset + nSize);
    //Read and parse AVIH
    if(!AVICallbakGetData(*offset,10 * sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "parseAVIH AVI_READ_FAILURE");
      return AVI_READ_FAILURE;
    }
    *offset+= (10 * sizeof(avi_uint32));
    if(m_hAviSummary.avih)
    {
      //Ideally,this should never happen.Print F3 for debug purpose.
      QTV_Free(m_hAviSummary.avih);
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                   "parseAVIH freeing existing avih..");
    }
    m_hAviSummary.avih = (avi_mainheader_avih*)QTV_Malloc(sizeof(avi_mainheader_avih));
    if(!m_hAviSummary.avih)
    {
       QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "parseAVIH Malloc failed..AVI_OUT_OF_MEMORY");
      return AVI_OUT_OF_MEMORY;
    }
    memset(m_hAviSummary.avih,0,sizeof(avi_mainheader_avih));

    memcpy(&m_hAviSummary.avih->dwMicroSecPerFrame,byteData,sizeof(avi_uint32));
    memcpy(&m_hAviSummary.avih->dwMaxBytesPerSec,byteData+4,sizeof(avi_uint32));
    memcpy(&m_hAviSummary.avih->dwPaddingGranularity,byteData+8,sizeof(avi_uint32));
    memcpy(&m_hAviSummary.avih->dwFlags,byteData+12,sizeof(avi_uint32));
    memcpy(&m_hAviSummary.avih->dwTotalFrames,byteData+16,sizeof(avi_uint32));
    memcpy(&m_hAviSummary.avih->dwInitialFrames,byteData+20,sizeof(avi_uint32));
    memcpy(&m_hAviSummary.avih->dwStreams,byteData+24,sizeof(avi_uint32));
    memcpy(&m_hAviSummary.avih->dwSuggestedBufferSize,byteData+28,sizeof(avi_uint32));
    memcpy(&m_hAviSummary.avih->dwWidth,byteData+32,sizeof(avi_uint32));
    memcpy(&m_hAviSummary.avih->dwHeight,byteData+36,sizeof(avi_uint32));

    for(int i = 0; i < 4; i++)
    {
      if(!AVICallbakGetData(*offset,
                            sizeof(avi_uint32),
                            m_ReadBuffer,AVI_READ_BUFFER_SIZE,
                            m_pUserData))
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "parseAVIH AVI_READ_FAILURE");
        return AVI_READ_FAILURE;
      }
      memcpy(&m_hAviSummary.avih->dwReserved[i],byteData,sizeof(avi_uint32));
      *offset+= sizeof(avi_uint32);
    }
    retError = AVI_SUCCESS;
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseAVIH AVI_CORRUPTED_FILE");
    retError = AVI_CORRUPTED_FILE;
  }
  if(*offset != offsetAtTheEndOfAVIH)
  {
    *offset = offsetAtTheEndOfAVIH;
  }
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::GetAudioTrackSummaryInfo

DESCRIPTION:
  Returns audio track summary info for given trackId.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::GetAudioTrackSummaryInfo(avi_uint8 trackId,avi_audiotrack_summary_info* info)
{
  aviErrorType retError = AVI_INVALID_USER_DATA;
  if( (!info) || (m_hAviSummary.n_streams <= trackId) )
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "GetAudioTrackSummaryInfo AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  for(int i = 0; i< AVI_MAX_TRACKS; i++)
  {
    if( (m_hAviSummary.stream_index[i].index == trackId)&&
        (m_hAviSummary.stream_index[i].type == AVI_CHUNK_AUDIO))
    {
      info->audioBytesPerSec = m_hAviSummary.audio_info[m_hAviSummary.stream_index[i].audioIndex].strfAudio.nAvgBytesPerSec;
      info->audioFrequency=m_hAviSummary.audio_info[m_hAviSummary.stream_index[i].audioIndex].strfAudio.nSamplesPerSec;
      info->nBlockAlign=m_hAviSummary.audio_info[m_hAviSummary.stream_index[i].audioIndex].strfAudio.nBlockAlign;
      if((info->nBlockAlign == 1152) || (info->nBlockAlign == 576))
      {
        info->isVbr = true;
      }
      else
      {
        info->isVbr = false;
      }

      info->nChannels=m_hAviSummary.audio_info[m_hAviSummary.stream_index[i].audioIndex].strfAudio.nChannels;
      info->trackID=m_hAviSummary.stream_index[i].index;
      info->wFormatTag=m_hAviSummary.audio_info[m_hAviSummary.stream_index[i].audioIndex].strfAudio.wFormatTag;
      info->nbitsPerAudioSample = m_hAviSummary.audio_info[m_hAviSummary.stream_index[i].audioIndex].strfAudio.wBitsPerSample;
      return AVI_SUCCESS;
    }
  }
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseSTRD

DESCRIPTION:
 Reads and parse STRD header.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseSTRD(avi_uint64* offset)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint32 nSize = 0;
  avi_uint32 nVersion = 0;
  avi_uint32 length = 0;
  avi_uint32 drmOffset = 0;
  aviErrorType retError = AVI_READ_FAILURE;
  bool isDivxCodec = false;

#ifdef FEATURE_QTV_AVI_DIVX_PARSER
  if ( (!memcmp(&m_hAviSummary.video_info->strhVideo.fccHandler,"divx",sizeof(fourCC_t))) ||
       (!memcmp(&m_hAviSummary.video_info->strhVideo.fccHandler,"DIVX",sizeof(fourCC_t))) ||
       (!memcmp(&m_hAviSummary.video_info->strhVideo.fccHandler,"dx50",sizeof(fourCC_t))) ||
       (!memcmp(&m_hAviSummary.video_info->strhVideo.fccHandler,"DX50",sizeof(fourCC_t))) ||
       (!memcmp(&m_hAviSummary.video_info->strhVideo.fccHandler,"div3",sizeof(fourCC_t))) ||
       (!memcmp(&m_hAviSummary.video_info->strhVideo.fccHandler,"DIV3",sizeof(fourCC_t))) ||
       (!memcmp(&m_hAviSummary.video_info->strhVideo.fccHandler,"div4",sizeof(fourCC_t))) ||
       (!memcmp(&m_hAviSummary.video_info->strhVideo.fccHandler,"DIV4",sizeof(fourCC_t)))
       )
  {
    isDivxCodec = true;
  }
#endif

#ifdef AVI_PARSER_DEBUG
  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                "parseSTRD Offset to start %d",*offset);
#endif

  if(isDivxCodec)
  {
    m_bDRMProtection = true;
    if(!AVICallbakGetData(*offset,3 * sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseSTRD AVI_READ_FAILURE");
      return retError;
    }
    *offset+= (3 *sizeof(avi_uint32));
    memcpy(&length,byteData,sizeof(avi_uint32));
    memcpy(&nVersion,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
    memcpy(&nSize,byteData+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
    drmOffset = *offset;

    m_AviSTRDHeader.version = nVersion;
	  m_AviSTRDHeader.drm_size = nSize;
	  m_AviSTRDHeader.drm_offset = drmOffset;
    if(m_AviSTRDHeader.drm_info)
    {
      QTV_Free(m_AviSTRDHeader.drm_info);
    }
    m_AviSTRDHeader.drm_info = (avi_uint8*)QTV_Malloc(nSize);
    if(!m_AviSTRDHeader.drm_info)
    {
      return AVI_OUT_OF_MEMORY;
    }
    if(!AVICallbakGetData(*offset,nSize,m_AviSTRDHeader.drm_info,m_AviSTRDHeader.drm_size,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseSTRD AVI_READ_FAILURE");
      return retError;
    }
    retError = AVI_SUCCESS;
  }
  else
  {
    *offset+= sizeof(avi_uint32);
    memcpy(&nSize,byteData,sizeof(avi_uint32));
    retError = AVI_SUCCESS;
  }
  *offset+= nSize;
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseSTRL

DESCRIPTION:
 Reads and parse STRL header.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseSTRL(avi_uint64* offset,avi_uint32 nListSize)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint32 nSize = 0;
  fourCC_t fccType = 0;
  aviErrorType retError = AVI_READ_FAILURE;

  /*
  * We would have already read STRL fourCC as that makes us call parseSTRL.
  * Hence subtract fourCC_t size from the total size.
  * This is used if we encounter any error in parsing STRL as we can skip entire
  * STRL with out parsing its children.
  */
  avi_uint64 offsetAtTheEndOfSTRL = (*offset + (nListSize - sizeof(fourCC_t)) );

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                "parseSTRL Offset to start %d",*offset);
  #endif
  if(!AVICallbakGetData(*offset,3 * sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseSTRL AVI_READ_FAILURE");
    return retError;
  }
  *offset+= (3 *sizeof(fourCC_t));
  if(memcmp(byteData,AVI_STRH_FOURCC,sizeof(fourCC_t)))
  {
    retError = AVI_CORRUPTED_FILE;
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseSTRL AVI_CORRUPTED_FILE");
    return retError;
  }
  memcpy(&nSize,byteData+4,sizeof(fourCC_t));
  memcpy(&fccType,byteData+8,sizeof(fourCC_t));

  if(!memcmp(&fccType,"vids",sizeof(fourCC_t)) && (m_hAviSummary.n_streams < AVI_MAX_TRACKS) )
  {
    m_hAviSummary.n_streams++;
    if(m_hAviSummary.n_video_tracks >= AVI_MAX_VIDEO_TRACKS)
    {
       QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "Not parsing Video/Subtitle Track as # video tracks > %d",AVI_MAX_VIDEO_TRACKS);
       //Skip rest of the STRL and continue parsing..
       *offset = offsetAtTheEndOfSTRL;
       return AVI_SUCCESS;
    }

    /*
    * Video and Caption track are marked as "vids".
    * Need to look at "biCompression" to distinguish.
    * For time being, we will treat it as video track and
    * once we have biCompression, we will adjust the stream information.
    */
    avi_video_info tempInfo;
    memset(&tempInfo,0,sizeof(avi_video_info));
    retError = parseVideoSTRH(offset,nSize,&tempInfo);
    if(retError == AVI_SUCCESS)
    {
        m_hAviSummary.n_video_tracks++;
        m_hAviSummary.stream_index[m_hAviSummary.nNextAvailTrackIndex].videoIndex =
        m_hAviSummary.nCurrVideoTrackInfoIndex;
        m_hAviSummary.stream_index[m_hAviSummary.nNextAvailTrackIndex].bParsed = true;
        m_hAviSummary.nCurrVideoTrackInfoIndex++;

        #ifdef AVI_PARSER_DEBUG
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "parseSTRL VIDEO STRL");
        #endif
        m_hAviSummary.stream_index[m_hAviSummary.nNextAvailTrackIndex].index =
        m_hAviSummary.nNextAvailTrackIndex;
        m_hAviSummary.stream_index[m_hAviSummary.nNextAvailTrackIndex].type=   AVI_CHUNK_VIDEO;
        m_hAviSummary.nNextAvailTrackIndex++;
    }//if(retError == AVI_SUCCESS)
    while ((*offset < m_nFileSize) && (retError == AVI_SUCCESS))
    {
      if(!AVICallbakGetData(*offset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
          "parseSTRL AVI_READ_FAILURE in vids");
        return AVI_READ_FAILURE;
      }
      *offset+=sizeof(fourCC_t);
      if(!memcmp(byteData,AVI_STRF_FOURCC,sizeof(fourCC_t) ) )
      {
        retError = parseVideoSTRF(offset,&tempInfo);
        if (retError != AVI_SUCCESS)
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "parseSTRL VIDEO STRF Failed..");
        }
        else
        {
          //check if we really have video track or subtitle track
          if( !memcmp(&tempInfo.strfVideo.biCompression,"DXSB",sizeof(avi_int32)) )
          {
            //The one we thought as VIDEO is actually sub-title.
            //Update our records to reflect this information.
            uint8 tempId = m_hAviSummary.nNextAvailTrackIndex -1;
            m_hAviSummary.stream_index[tempId].type = AVI_CHUNK_BITMAP_CAPTION;
            m_hAviSummary.stream_index[tempId].videoIndex = 0;
            m_hAviSummary.stream_index[tempId].bParsed = false;
            m_hAviSummary.nCurrVideoTrackInfoIndex--;
            m_hAviSummary.n_video_tracks--;
            #ifdef AVI_PARSER_DEBUG
              QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"parseSTRL AVI_CHUNK_BITMAP_CAPTION STRL");
            #endif
          }
          else
          {
            //It's a Video track, copy over strh/strf/strd/strn if any.
            memcpy(&m_hAviSummary.video_info[m_hAviSummary.nCurrVideoTrackInfoIndex-1], &tempInfo,sizeof(avi_video_info));
          }
        }
      }
      else if(!memcmp(byteData,AVI_STRN_FOURCC,sizeof(fourCC_t) ) )
      {
        retError = parseVideoSTRN(offset,&tempInfo);
        if (retError != AVI_SUCCESS)
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "parseSTRL VIDEO STRN Failed..");
        }
        else
        {
          if( (tempInfo.strnVideo.streamNameSize && tempInfo.strnVideo.streamName)&&
              (memcmp(&tempInfo.strfVideo.biCompression,"DXSB",sizeof(avi_int32))!= 0) )
          {
            //It's a Video track, copy over strh/strf/strd/strn if any.
            memcpy(&m_hAviSummary.video_info[m_hAviSummary.nCurrVideoTrackInfoIndex-1], &tempInfo,sizeof(avi_video_info));
          }
        }
      }
      else if(!memcmp(byteData,AVI_STRD_FOURCC,sizeof(fourCC_t)))
      {
        //Clip has 'strd' chunk,meaning it's DRM protected.
        if( (retError = parseSTRD(offset))!= AVI_SUCCESS )
        {
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
            "StartParsing parseSTRD failed..retError %d",retError);
          return retError;
        }
#ifdef AVI_PARSER_DEBUG
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"StartParsing done parsing STRD");
#endif
      }
      else
      {
        *offset-=sizeof(fourCC_t);
        break;
      }
    }//while ((offset < m_nFileSize) && (retError == AVI_SUCCESS))
  }//if(!memcmp(&fccType,"vids",sizeof(fourCC_t)))
  else if(!memcmp(&fccType,"auds",sizeof(fourCC_t)) && (m_hAviSummary.n_streams < AVI_MAX_TRACKS) )
  {
    m_hAviSummary.n_streams++;
    m_hAviSummary.n_audio_tracks++;
    if(m_hAviSummary.n_audio_tracks > AVI_MAX_AUDIO_TRACKS)
    {
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "Not parsing Audio Track as # audio tracks > %d",AVI_MAX_AUDIO_TRACKS);
      //Skip rest of the STRL and continue parsing..
      *offset = offsetAtTheEndOfSTRL;
      return AVI_SUCCESS;
    }
    else
    {
      m_hAviSummary.stream_index[m_hAviSummary.nNextAvailTrackIndex].audioIndex =
      m_hAviSummary.nCurrAudioTrackInfoIndex;
      m_hAviSummary.stream_index[m_hAviSummary.nNextAvailTrackIndex].bParsed = true;

      retError = parseAudioSTRH(offset,nSize);

    }
    m_hAviSummary.stream_index[m_hAviSummary.nNextAvailTrackIndex].index = m_hAviSummary.nNextAvailTrackIndex;
    m_hAviSummary.stream_index[m_hAviSummary.nNextAvailTrackIndex].type= AVI_CHUNK_AUDIO;
    m_hAviSummary.nNextAvailTrackIndex++;

    while((*offset < m_nFileSize) && (retError == AVI_SUCCESS))
    {
      if(!AVICallbakGetData(*offset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseSTRL AVI_READ_FAILURE in auds");
        return AVI_READ_FAILURE;
      }
	    *offset+=sizeof(fourCC_t);
      if(!memcmp(byteData,AVI_STRF_FOURCC,sizeof(fourCC_t) ) )
      {
        retError = parseAudioSTRF(offset);
        if (retError != AVI_SUCCESS)
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "parseSTRL AUDIO STRF Failed..");
        }
      }
      else if(!memcmp(byteData,AVI_STRN_FOURCC,sizeof(fourCC_t) ) )
      {
        retError = parseAudioSTRN(offset);
        if (retError != AVI_SUCCESS)
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "parseSTRL AUDIO STRN Failed..");
        }
      }
      else if(!memcmp(byteData,AVI_STRD_FOURCC,sizeof(fourCC_t)))
      {
        //Clip has 'strd' chunk,meaning it's DRM protected.
        if( (retError = parseSTRD(offset))!= AVI_SUCCESS )
        {
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "StartParsing parseSTRD failed..retError %d",retError);
          return retError;
        }
        #ifdef AVI_PARSER_DEBUG
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"StartParsing done parsing STRD");
        #endif
      }
	    else
	    {
        *offset-=sizeof(fourCC_t);
		    break;
	    }
    }//while ((offset < m_nFileSize) && (retError == AVI_SUCCESS))
    if(AVI_SUCCESS == retError)
    {
      /*
      * FOR CBR AUDIO, we need nAvgBytesPerSec to be nonzero.
      * If it's zero, signal the failure here itself as we won't be able
      * to calculate time stamp correctly.
      *
      * FOR VBR, we need dwScale,dwRate and nBlockAlign to be non-zero.
      * if any one of them is 0,signal the failure here itself as we won't be able
      * to calculate time stamp correctly.
      */
      retError = AVI_CORRUPTED_FILE;
      int audid = m_hAviSummary.stream_index[m_hAviSummary.nNextAvailTrackIndex-1].index;
      avi_audiotrack_summary_info audsuminfo;
      memset(&audsuminfo,0,sizeof(avi_audiotrack_summary_info));
      if(GetAudioTrackSummaryInfo(audid,&audsuminfo) == AVI_SUCCESS)
      {
        if((audsuminfo.isVbr) &&
            ((!m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwScale) ||
             (!m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwRate)  ||
             (!m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.nBlockAlign))
          )
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "VBR AUDIO:AVI_CORRUPTED_FILE can't get dwScale|dwRate|nBlockAlign");
        }
        else if( (!audsuminfo.isVbr) &&
                 (!m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.nAvgBytesPerSec) )
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "CBR AUDIO:AVI_CORRUPTED_FILE can't get nAvgBytesPerSec");
        }
        else
        {
          retError = AVI_SUCCESS;
          m_hAviSummary.nCurrAudioTrackInfoIndex++;
          #ifdef AVI_PARSER_DEBUG
            QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "parseSTRL AUDIO STRL");
          #endif
        }
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "parseSTRL AUDIO STRL Failed..");
    }
    return retError;
  }
  else
  {
    if(m_hAviSummary.n_streams < AVI_MAX_TRACKS)
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                   "parseSTRL encountered non 'auds'/'vids'");
      m_hAviSummary.stream_index[m_hAviSummary.nNextAvailTrackIndex].index =
      m_hAviSummary.nNextAvailTrackIndex;
      m_hAviSummary.stream_index[m_hAviSummary.nNextAvailTrackIndex].type = AVI_CHUNK_UNKNOWN;
      m_hAviSummary.nNextAvailTrackIndex++;
    }
    //Skip rest of the STRL and continue parsing..
    *offset = offsetAtTheEndOfSTRL;
    retError = AVI_SUCCESS;
  }
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseVideoSTRH

DESCRIPTION:
 Reads and parse video track STRH header.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseVideoSTRH(avi_uint64* offset,
                                       avi_uint64 nMaxSize,
                                       avi_video_info* streaminfo)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint64 offsetAtStartOfVideoSTRH = *offset;
  aviErrorType retError = AVI_READ_FAILURE;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                  "parseVideoSTRH Offset to start %d",*offset);
  #endif
  if((!streaminfo)||(!offset))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseVideoSTRH AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  if(!AVICallbakGetData(*offset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseVideoSTRH AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=sizeof(fourCC_t);

  memcpy(&(streaminfo->strhVideo.fccType),"vids",sizeof(fourCC_t));
  memcpy(&(streaminfo->strhVideo.fccHandler),byteData,sizeof(fourCC_t));

  if(!AVICallbakGetData(*offset,sizeof(avi_int32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseVideoSTRH AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=sizeof(avi_int32);
  memcpy(&(streaminfo->strhVideo.dwFlags),byteData,sizeof(avi_int32));

  if(!AVICallbakGetData(*offset,(2*sizeof(avi_int16)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseVideoSTRH AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(2*sizeof(avi_int16));
  memcpy(&(streaminfo->strhVideo.wPriority),byteData,sizeof(avi_int16));
  memcpy(&(streaminfo->strhVideo.wLanguage),byteData+sizeof(avi_int16),sizeof(avi_int16));

  if(!AVICallbakGetData(*offset,(8*sizeof(avi_int32)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseVideoSTRH AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(8*sizeof(avi_int32));
  int tempOffset = 0;

  memcpy(&(streaminfo->strhVideo.dwInitialFrames),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);

  memcpy(&(streaminfo->strhVideo.dwScale),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);

  memcpy(&(streaminfo->strhVideo.dwRate),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);

  memcpy(&(streaminfo->strhVideo.dwStart),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);
  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
               "parseVideoSTRH dwStart %d",streaminfo->strhVideo.dwStart);

  memcpy(&(streaminfo->strhVideo.dwLength),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);
  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
               "parseVideoSTRH dwLength %d",streaminfo->strhVideo.dwLength);

  memcpy(&(streaminfo->strhVideo.dwSuggestedBufferSize),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);

  memcpy(&(streaminfo->strhVideo.dwQuality),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);

  memcpy(&(streaminfo->strhVideo.dwSampleSize),
         byteData+tempOffset,sizeof(avi_int32));

  if(!AVICallbakGetData(*offset,(4*sizeof(avi_int16)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseVideoSTRH AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(4*sizeof(avi_int16));
  tempOffset = 0;

  memcpy(&(streaminfo->strhVideo.rcFrame_left),
         byteData+tempOffset,sizeof(avi_int16));
  tempOffset+=sizeof(avi_int16);

  memcpy(&(streaminfo->strhVideo.rcFrame_top),
         byteData+tempOffset,sizeof(avi_int16));
  tempOffset+=sizeof(avi_int16);

  memcpy(&(streaminfo->strhVideo.rcFrame_right),
         byteData+tempOffset,sizeof(avi_int16));
  tempOffset+=sizeof(avi_int16);

  memcpy(&(streaminfo->strhVideo.rcFrame_bottom),
         byteData+tempOffset,sizeof(avi_int16));
  streaminfo->strhVideo.present = 1;

  retError = AVI_SUCCESS;

  if(*offset != (offsetAtStartOfVideoSTRH + nMaxSize - sizeof(fourCC_t) ))
  {
    *offset = (offsetAtStartOfVideoSTRH + nMaxSize - sizeof(fourCC_t));
  }
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseAudioSTRH

DESCRIPTION:
 Reads and parse audio track STRH header.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseAudioSTRH(avi_uint64* offset,avi_uint64 nMaxSize)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint64 offsetAtStartOfAudioSTRH = *offset;
  aviErrorType retError = AVI_READ_FAILURE;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                "parseAudioSTRH Offset to start %d",*offset);
  #endif
  if(!AVICallbakGetData(*offset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseAudioSTRH AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=sizeof(fourCC_t);

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.fccType),
         "auds",sizeof(fourCC_t));
  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.fccHandler),
         byteData,sizeof(fourCC_t));

  if(!AVICallbakGetData(*offset,sizeof(avi_int32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseAudioSTRH AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=sizeof(avi_int32);
  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwFlags),
         byteData,sizeof(avi_int32));

  if(!AVICallbakGetData(*offset,(2*sizeof(avi_int16)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseAudioSTRH AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(2*sizeof(avi_int16));
  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.wPriority),
         byteData,sizeof(avi_int16));
  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.wLanguage),
         byteData+sizeof(avi_int16),sizeof(avi_int16));

  if(!AVICallbakGetData(*offset,(8*sizeof(avi_int32)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseAudioSTRH AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(8*sizeof(avi_int32));
  int tempOffset = 0;

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwInitialFrames),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);

  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
               "parseAudioSTRH dwInitialFrames %d",
               m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwInitialFrames);

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwScale),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwRate),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwStart),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);
  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
               "parseAudioSTRH dwStart %d",
               m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwStart);

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwLength),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);

  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
               "parseAudioSTRH dwLength %d",
               m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwLength);

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwSuggestedBufferSize),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwQuality),
         byteData+tempOffset,sizeof(avi_int32));
  tempOffset+=sizeof(avi_int32);

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.dwSampleSize),
         byteData+tempOffset,sizeof(avi_int32));

  if(!AVICallbakGetData(*offset,(4*sizeof(avi_int16)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseAudioSTRH AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(4*sizeof(avi_int16));
  tempOffset = 0;

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.rcFrame_left),
         byteData+tempOffset,sizeof(avi_int16));
  tempOffset+=sizeof(avi_int16);

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.rcFrame_top),
         byteData+tempOffset,sizeof(avi_int16));
  tempOffset+=sizeof(avi_int16);

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.rcFrame_right),
         byteData+tempOffset,sizeof(avi_int16));
  tempOffset+=sizeof(avi_int16);

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.rcFrame_bottom),
         byteData+tempOffset,sizeof(avi_int16));
  m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strhAudio.present = 1;

  if(*offset != (offsetAtStartOfAudioSTRH + nMaxSize - sizeof(fourCC_t) ))
  {
    *offset = (offsetAtStartOfAudioSTRH + nMaxSize - sizeof(fourCC_t));
  }

  retError = AVI_SUCCESS;
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseAudioSTRF

DESCRIPTION:
 Reads and parse audio track STRF header.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseAudioSTRF(avi_uint64* offset)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint32 nSize = 0;
  aviErrorType retError = AVI_CORRUPTED_FILE;
  avi_uint64 offsetAtTheEndOfAudioSTRF = 0;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                  "parseAudioSTRF Offset to start %d",*offset);
  #endif
  if(!AVICallbakGetData(*offset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseAudioSTRF AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=sizeof(avi_uint32);

  memcpy(&nSize,byteData,sizeof(avi_uint32));
  offsetAtTheEndOfAudioSTRF = (*offset + nSize);

  if(!AVICallbakGetData(*offset,(2*sizeof(avi_int16)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseAudioSTRF AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(2*sizeof(avi_int16));

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.wFormatTag),
        byteData,sizeof(avi_int16));

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.nChannels),
        byteData+sizeof(avi_int16),sizeof(avi_int16));

  if(!AVICallbakGetData(*offset,(2*sizeof(avi_int32)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseAudioSTRF AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(2*sizeof(avi_int32));

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.nSamplesPerSec),
        byteData,sizeof(avi_int32));

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.nAvgBytesPerSec),
        byteData+sizeof(avi_int32),sizeof(avi_int32));


  if(!AVICallbakGetData(*offset,(2*sizeof(avi_int16)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseAudioSTRF AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(2*sizeof(avi_int16));

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.nBlockAlign),
        byteData,sizeof(avi_int16));

  memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.wBitsPerSample),
        byteData+sizeof(avi_int16),sizeof(avi_int16));

  if(nSize > MIN_AUDIO_STRF_HDR_SIZE)
  {
		if(!AVICallbakGetData(*offset,sizeof(avi_int16),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseAudioSTRF AVI_READ_FAILURE");
      return AVI_READ_FAILURE;
    }
    *offset+=sizeof(avi_int16);

    memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.cbSize),
            byteData,sizeof(avi_int16));

    if(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.cbSize)
    {
      m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.extra = (avi_uint8*)QTV_Malloc((sizeof(avi_uint8))*(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.cbSize));

      //Check if it is a valid pointer
      if(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.extra)
      {
        for(int i=0; i < m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.cbSize; i++)
        {
          if(!AVICallbakGetData(*offset,sizeof(avi_uint8),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
          {
            QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                          "parseAudioSTRF AVI_READ_FAILURE");
            return AVI_READ_FAILURE;
          }
          *offset+=sizeof(avi_uint8);
          memcpy(&(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.extra[i]),
                 byteData,sizeof(avi_uint8));
		    }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                     "parseAudioSTRF Malloc failed");
        return AVI_OUT_OF_MEMORY;
      }
    }
  }
  else
  {
    m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strfAudio.cbSize = 0;
  }
  retError = AVI_SUCCESS;
  *offset = offsetAtTheEndOfAudioSTRF;
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseVideoSTRF

DESCRIPTION:
 Reads and parse video track STRF header.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseVideoSTRF(avi_uint64* offset,avi_video_info* streaminfo)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint32 nSize = 0;
  aviErrorType retError = AVI_CORRUPTED_FILE;
  avi_uint64 offsetAtTheEndOfVideoSTRF = 0;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                  "parseVideoSTRF Offset to start %d",*offset);
  #endif

  if((!streaminfo)||(!offset))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseVideoSTRF AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  if(!AVICallbakGetData(*offset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseVideoSTRF AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=sizeof(avi_uint32);

  memcpy(&nSize,byteData,sizeof(avi_uint32));
  offsetAtTheEndOfVideoSTRF = (*offset + nSize);

  if(!AVICallbakGetData(*offset,(2*sizeof(avi_int32)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseVideoSTRF AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(2*sizeof(avi_int32));

  streaminfo->strfVideo.biSize = nSize;

  memcpy(&(streaminfo->strfVideo.biWidth),byteData,sizeof(avi_int32));

  memcpy(&(streaminfo->strfVideo.biHeight),
        byteData+sizeof(avi_int32),sizeof(avi_int32));

  if(!AVICallbakGetData(*offset,(2*sizeof(avi_int16)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseVideoSTRF AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(2*sizeof(avi_int32));

  memcpy(&(streaminfo->strfVideo.biPlanes),byteData,sizeof(avi_int16));

  memcpy(&(streaminfo->strfVideo.biBitCount),
        byteData+sizeof(avi_int16),sizeof(avi_int16));


  if(!AVICallbakGetData(*offset,(6*sizeof(avi_int32)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseVideoSTRF AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=(6*sizeof(avi_int32));

  memcpy(&(streaminfo->strfVideo.biCompression),byteData,sizeof(avi_int32));

  memcpy(&(streaminfo->strfVideo.biSizeImage),
        byteData+sizeof(avi_int32),sizeof(avi_int32));

  memcpy(&(streaminfo->strfVideo.biXPelsPerMeter),
        byteData+(2*sizeof(avi_int32)),sizeof(avi_int32));
  memcpy(&(streaminfo->strfVideo.biYPelsPerMeter),
        byteData+(3*sizeof(avi_int32)),sizeof(avi_int32));
  memcpy(&(streaminfo->strfVideo.biClrUsed),
        byteData+(4*sizeof(avi_int32)),sizeof(avi_int32));
  memcpy(&(streaminfo->strfVideo.biClrImportant),
        byteData+(5*sizeof(avi_int32)),sizeof(avi_int32));
  retError = AVI_SUCCESS;
  *offset = offsetAtTheEndOfVideoSTRF;
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseVideoSTRN

DESCRIPTION:
 Reads and parse video track STRN header.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseVideoSTRN(avi_uint64* offset,avi_video_info* streaminfo)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint32 nSize = 0;
  aviErrorType retError = AVI_CORRUPTED_FILE;
  avi_uint64 offsetAtTheEndOfVideoSTRN = 0;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                  "parseVideoSTRN Offset to start %d",*offset);
  #endif

  if((!streaminfo)||(!offset))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "parseVideoSTRN AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  if(!AVICallbakGetData(*offset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseVideoSTRN AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=sizeof(avi_uint32);
  memcpy(&nSize,byteData,sizeof(avi_uint32));
  /* make sure nSize is even number (16 bits boundary) */
  nSize = nSize + (nSize %2);
  offsetAtTheEndOfVideoSTRN = (*offset + nSize);

  if( memcmp(&(streaminfo->strfVideo.biCompression),"DXSB",sizeof(avi_int32)) != 0)
  {
    #ifdef AVI_PARSER_DEBUG
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                      "parseVideoSTRN nSize %d",nSize);
    #endif

    retError = AVI_SUCCESS;
    if(nSize > 0)
    {
      if(streaminfo->strnVideo.streamName)
      {
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                      "parseVideoSTRN existing streamName is not NULL video trackid %d",
                       m_hAviSummary.nCurrVideoTrackInfoIndex);
        QTV_Free(streaminfo->strnVideo.streamName);
      }
      streaminfo->strnVideo.streamName =
        (avi_uint8*)QTV_Malloc(nSize+1);
      if(!streaminfo->strnVideo.streamName)
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "parseVideoSTRN AVI_OUT_OF_MEMORY");
        return AVI_OUT_OF_MEMORY;
      }
      if(!AVICallbakGetData(*offset,nSize,streaminfo->strnVideo.streamName,nSize,m_pUserData))
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "parseVideoSTRN AVI_READ_FAILURE");
        return AVI_READ_FAILURE;
      }
      memcpy(streaminfo->strnVideo.streamName+nSize,"\0",1);
      streaminfo->strnVideo.streamNameSize = nSize;

     // *offset+=nSize;
      retError = AVI_SUCCESS;
    }//if(nSize > 0)
  }//if(!memcmp(&streaminfo.strfVideo.biCompression,"DXSB",sizeof(avi_int32)))
  else
  {
    retError = AVI_SUCCESS;
  }
  *offset = offsetAtTheEndOfVideoSTRN;
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseAudioSTRN

DESCRIPTION:
 Reads and parse audio track STRN header.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseAudioSTRN(avi_uint64* offset)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint32 nSize = 0;
  aviErrorType retError = AVI_CORRUPTED_FILE;
  avi_uint64 offsetAtTheEndOfAudioSTRN = 0;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                  "parseAudioSTRN Offset to start %d",*offset);
  #endif
  if(!AVICallbakGetData(*offset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "parseAudioSTRN AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset+=sizeof(avi_uint32);
  memcpy(&nSize,byteData,sizeof(avi_uint32));
  /* make sure nSize is even number (16 bits boundary) */
  nSize = nSize + (nSize %2);
  offsetAtTheEndOfAudioSTRN = (*offset + nSize);
  retError = AVI_SUCCESS;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                    "parseAudioSTRN nSize %d",nSize);
  #endif

  if(nSize > 0)
  {
    if(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strnAudio.streamName)
    {
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                    "parseAudioSTRN existing streamName is not NULL audio trackid %d",
                    m_hAviSummary.nCurrAudioTrackInfoIndex);
      QTV_Free(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strnAudio.streamName);
    }
    m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strnAudio.streamName =
      (avi_uint8*)QTV_Malloc(nSize+1);

    if(!m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strnAudio.streamName)
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseAudioSTRN AVI_OUT_OF_MEMORY");
      return AVI_OUT_OF_MEMORY;
    }
    if(!AVICallbakGetData(*offset,
                          nSize,
                          m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strnAudio.streamName,
                          nSize,
                          m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseAudioSTRN AVI_READ_FAILURE");
      return AVI_READ_FAILURE;
    }
    memcpy(m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strnAudio.streamName+nSize,
      "\0",1);
    m_hAviSummary.audio_info[m_hAviSummary.nCurrAudioTrackInfoIndex].strnAudio.streamNameSize = nSize;

    //*offset+=nSize;
    retError = AVI_SUCCESS;
  }//if(nSize > 0)
  *offset = offsetAtTheEndOfAudioSTRN;
  return retError;
}
#ifdef WALK_INDEX_TABLES
/* =============================================================================
FUNCTION:
  aviParser::WalkIDX1Table

DESCRIPTION:
 Prints IDX1 table.Used only for debugging;

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
void aviParser::WalkIDX1Table()
{
  if(!m_hAviSummary.pIdx1Table)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,"m_hAviSummary.pIdx1Table is NULL");
    return;
  }
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "............................<START-WalkIDX1Table>.............................");

  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "...Summary....");
  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "#Bytes in IDX1(excluding initial 8 bytes HDR)%d",m_hAviSummary.pIdx1Table->nTotalSize);

  QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "total#Entries %d #AudioEntries %d #VideoEntries %d",
                m_hAviSummary.pIdx1Table->nTotalEntriesInIDX1,
                m_hAviSummary.pIdx1Table->nAudioEntriesInIDX1,
                m_hAviSummary.pIdx1Table->nVideoEntriesInIDX1);
  QTV_MSG_PRIO4(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "nKeyVideoyEntriesInIDX1 %d nAudioEntriesLoaded %d nAudioEntriesAllocated %d nVideoEntriesAllocated %d",
                m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1,
                m_hAviSummary.pIdx1Table->nCurrParsedAudioEntriesInIDX1,
                m_hAviSummary.pIdx1Table->nAudioEntriesAllocated,
                m_hAviSummary.pIdx1Table->nVideoEntriesAllocated);

  QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "chunkType AVI_CHUNK_AUDIO is %d chunkType AVI_CHUNK_VIDEO is %d",
                AVI_CHUNK_AUDIO,
                AVI_CHUNK_VIDEO);

  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "...Summary<DONE>....");

  if(m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1)
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "Now Dumping Video Key Entries.. %d",m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1);
  }

  for(int i = 0;
      (i < m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1) &&
      (m_hAviSummary.pIdx1Table->pKeyVideoEntries); i++)
  {
    QTV_MSG_PRIO4(QTVDIAG_FILE_OPS,
                  QTVDIAG_PRIO_HIGH,
                  "index#%d trackId %d chunkType %d dwFlags %x",
                  i,
                  m_hAviSummary.pIdx1Table->pKeyVideoEntries[i].trackId,
                  m_hAviSummary.pIdx1Table->pKeyVideoEntries[i].chunkType,
                  m_hAviSummary.pIdx1Table->pKeyVideoEntries[i].dwFlags);

    QTV_MSG_PRIO5(QTVDIAG_FILE_OPS,
                  QTVDIAG_PRIO_HIGH,
                  "Offset %d dwSize %d #FromMOVI %d TS(in MS) %d dwOffsetInIDX1 %d",
                  m_hAviSummary.pIdx1Table->pKeyVideoEntries[i].dwOffset,
                  m_hAviSummary.pIdx1Table->pKeyVideoEntries[i].dwSize,
                  m_hAviSummary.pIdx1Table->pKeyVideoEntries[i].dwVidFrameCount,
                  m_hAviSummary.pIdx1Table->pKeyVideoEntries[i].nTimeStamp,
                  m_hAviSummary.pIdx1Table->pKeyVideoEntries[i].dwOffsetInIDX1);
  }
  if(m_hAviSummary.pIdx1Table->nCurrParsedAudioEntriesInIDX1)
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "Now Dumping loaded Audio Entries..%d",m_hAviSummary.pIdx1Table->nCurrParsedAudioEntriesInIDX1);
  }
  for(int i = 0;
      (i < m_hAviSummary.pIdx1Table->nCurrParsedAudioEntriesInIDX1) &&
      (m_hAviSummary.pIdx1Table->pAudioEntries); i++)
  {
    QTV_MSG_PRIO4(QTVDIAG_FILE_OPS,
                  QTVDIAG_PRIO_HIGH,
                  "index#%d trackId %d chunkType %d dwFlags %x",
                  i,
                  m_hAviSummary.pIdx1Table->pAudioEntries[i].trackId,
                  m_hAviSummary.pIdx1Table->pAudioEntries[i].chunkType,
                  m_hAviSummary.pIdx1Table->pAudioEntries[i].dwFlags);

    QTV_MSG_PRIO5(QTVDIAG_FILE_OPS,
                  QTVDIAG_PRIO_HIGH,
                  "Offset %d dwSize %d #FromMOVI %d TS(in MS)%d dwOffsetInIDX1 %d",
                  m_hAviSummary.pIdx1Table->pAudioEntries[i].dwOffset,
                  m_hAviSummary.pIdx1Table->pAudioEntries[i].dwSize,
                  m_hAviSummary.pIdx1Table->pAudioEntries[i].dwAudFrameCount,
                  m_hAviSummary.pIdx1Table->pAudioEntries[i].nTimeStamp,
                  m_hAviSummary.pIdx1Table->pAudioEntries[i].dwOffsetInIDX1);
  }
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "............................<END-WalkIDX1Table>.............................");
}
#endif
/* =============================================================================
FUNCTION:
  aviParser::parseINDX

DESCRIPTION:
 Reads and parse index header

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseINDX(avi_uint64 offset,fourCC_t fourcc)
{
  aviErrorType retError = AVI_SUCCESS;

  //int failed = 0;
	avi_int32		cb = 0;
  avi_uint16	wLongsPerEntry = 0; // size of each entry in aIndex array
  char		bIndexSubType = '\0';   // must be 0
  char		bIndexType = '\0';      // one of AVI_INDEX_* codes
  avi_uint32	nEntriesInUse = 0;  //
  avi_uint32	dwChunkId = 0;      // '##dc' OR '##db' OR ...
  avi_uint32	dwReserved[3] = {0};
	//char str[5] = {0};
	//int	file_offset;
	//int stream_id = -1;
	//fourCC_t	fourCC = {0};
  unsigned char* byteData = m_ReadBuffer;
  int nCount = 0;
  int baseOffset = offset;

  if(!AVICallbakGetData(offset,sizeof(avi_int32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseINDX AVI_READ_FAILURE");
    retError = AVI_READ_FAILURE;
    return AVI_READ_FAILURE;
  }
  memcpy(&cb,byteData,sizeof(avi_int32));
  offset += sizeof(avi_int32);
  nCount += sizeof(avi_int32);

  if(!AVICallbakGetData(offset,sizeof(avi_uint16),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseINDX AVI_READ_FAILURE");
    retError = AVI_READ_FAILURE;
    return AVI_READ_FAILURE;
  }
  memcpy(&wLongsPerEntry,byteData,sizeof(avi_uint16));
  offset += sizeof(avi_uint16);
  nCount += sizeof(avi_uint16);

  if(!AVICallbakGetData(offset,2* sizeof(char),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseINDX AVI_READ_FAILURE");
    retError = AVI_READ_FAILURE;
    return AVI_READ_FAILURE;
  }
  memcpy(&bIndexSubType,byteData,sizeof(char));
  memcpy(& bIndexType,byteData+sizeof(char),sizeof(char));
  offset += (2* sizeof(char));
  nCount += (2* sizeof(char));

  //Format of Base index table and Super Index table is identical.
  //Need to re-write some of the following stuff when 'indx' is really
  //an AVI_INDEX_OF_CHUNKS.
  QTV_ASSERT(bIndexType!=AVI_INDEX_OF_CHUNKS);

  if(!AVICallbakGetData(offset,5* sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseINDX AVI_READ_FAILURE");
    retError = AVI_READ_FAILURE;
    return AVI_READ_FAILURE;
  }
  memcpy(&nEntriesInUse,byteData,sizeof(avi_uint32));
  memcpy(&dwChunkId,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
  memcpy(&dwReserved[0],byteData+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
  memcpy(&dwReserved[1],byteData+(3*sizeof(avi_uint32)),sizeof(avi_uint32));
  memcpy(&dwReserved[2],byteData+(4*sizeof(avi_uint32)),sizeof(avi_uint32));
  offset += (5* sizeof(avi_uint32));
  nCount += (5* sizeof(avi_uint32));

  avi_uint16 trackId,cType;

  memcpy(&trackId,(unsigned char*)&dwChunkId,sizeof(avi_uint16));
  trackId = ascii_2_short_int(&trackId);
  memcpy(&cType,((unsigned char*)&dwChunkId)+2,sizeof(avi_uint16));

  if(trackId < AVI_MAX_TRACKS)
  {
    m_start_data_offset_base_indx_tbl[trackId] = nCount + baseOffset;
    m_base_indx_tbl[trackId].fcc = fourcc;
    m_base_indx_tbl[trackId].cb = cb;
    m_base_indx_tbl[trackId].wLongsPerEntry = wLongsPerEntry;
	  m_base_indx_tbl[trackId].bIndexSubType = bIndexSubType;
	  m_base_indx_tbl[trackId].bIndexType = bIndexType;
	  m_base_indx_tbl[trackId].nEntriesInUse = nEntriesInUse;
	  m_base_indx_tbl[trackId].dwChunkId = dwChunkId;
	  m_base_indx_tbl[trackId].dwReserved[0] = dwReserved[0];
	  m_base_indx_tbl[trackId].dwReserved[1] = dwReserved[1];
	  m_base_indx_tbl[trackId].dwReserved[2] = dwReserved[2];
  }
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseIX

DESCRIPTION:
 Reads and parse indx chunk.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseIX(avi_uint64 offset,int* ixsize)
{
  aviErrorType retError = AVI_SUCCESS;

  //int failed = 0;
	avi_int32		cb = 0;
  avi_uint16	wLongsPerEntry = 0; // size of each entry in aIndex array
  char		bIndexSubType = '\0';   // must be 0
  char		bIndexType = '\0';      // one of AVI_INDEX_* codes
  avi_uint32	nEntriesInUse = 0;  //
  avi_uint32	dwChunkId = 0;      // '##dc' OR '##db' OR ...
  avi_uint32	dwReserved = 0;
  avi_uint32	dwReserved_arr[3] = {0};
	//char str[5] = {0};
	//int	file_offset;
	//int stream_id = -1;
  unsigned char* byteData = m_ReadBuffer;
  int nCount = 0;
  int baseOffset = offset;
  avi_uint64 qwBaseOffset;

  fourCC_t fourcc;

  if(!ixsize)
  {
    return AVI_INVALID_USER_DATA;
  }
  *ixsize = 0;
  if(!AVICallbakGetData(offset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseIX AVI_READ_FAILURE");
    retError = AVI_READ_FAILURE;
    return AVI_READ_FAILURE;
  }
  memcpy(&fourcc,byteData,sizeof(fourCC_t));
  offset += sizeof(fourCC_t);
  nCount += sizeof(fourCC_t);

  if(!AVICallbakGetData(offset,sizeof(avi_int32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseIX AVI_READ_FAILURE");
    retError = AVI_READ_FAILURE;
    return AVI_READ_FAILURE;
  }
  memcpy(&cb,byteData,sizeof(avi_int32));
  offset += sizeof(avi_int32);
  nCount += sizeof(avi_int32);

  if(!AVICallbakGetData(offset,sizeof(avi_uint16),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseIX AVI_READ_FAILURE");
    retError = AVI_READ_FAILURE;
    return AVI_READ_FAILURE;
  }
  memcpy(&wLongsPerEntry,byteData,sizeof(avi_uint16));
  offset += sizeof(avi_uint16);
  nCount += sizeof(avi_uint16);

  if(!AVICallbakGetData(offset,2* sizeof(char),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseIX AVI_READ_FAILURE");
    retError = AVI_READ_FAILURE;
    return AVI_READ_FAILURE;
  }
  memcpy(&bIndexSubType,byteData,sizeof(char));
  memcpy(& bIndexType,byteData+sizeof(char),sizeof(char));
  offset += (2* sizeof(char));
  nCount += (2* sizeof(char));

  if(bIndexType == AVI_INDEX_OF_INDEXES)
  {
    if(!AVICallbakGetData(offset,5* sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "parseIX AVI_READ_FAILURE");
      retError = AVI_READ_FAILURE;
      return AVI_READ_FAILURE;
    }
    memcpy(&nEntriesInUse,byteData,sizeof(avi_uint32));
    memcpy(&dwChunkId,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
    memcpy(&dwReserved_arr[0],byteData+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
    memcpy(&dwReserved_arr[1],byteData+(3*sizeof(avi_uint32)),sizeof(avi_uint32));
    memcpy(&dwReserved_arr[2],byteData+(4*sizeof(avi_uint32)),sizeof(avi_uint32));

    offset += (5* sizeof(avi_uint32));
    nCount += (5* sizeof(avi_uint32));
  }
  else
  {
    //Either Standard AVI Index of Chunks or Index of Fields.
    if(!AVICallbakGetData(offset,2* sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "parseIX AVI_READ_FAILURE");
      retError = AVI_READ_FAILURE;
      return AVI_READ_FAILURE;
    }
    memcpy(&nEntriesInUse,byteData,sizeof(avi_uint32));
    memcpy(&dwChunkId,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
    offset += (2* sizeof(avi_uint32));
    nCount += (2* sizeof(avi_uint32));

    if(!AVICallbakGetData(offset,sizeof(avi_uint64),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "parseIX AVI_READ_FAILURE");
      retError = AVI_READ_FAILURE;
      return AVI_READ_FAILURE;
    }
    memcpy(&qwBaseOffset,byteData,sizeof(avi_uint64));
    offset += (sizeof(avi_uint64));
    nCount += (sizeof(avi_uint64));

    if(!AVICallbakGetData(offset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "parseIX AVI_READ_FAILURE");
      retError = AVI_READ_FAILURE;
      return AVI_READ_FAILURE;
    }
    memcpy(&dwReserved,byteData,sizeof(avi_uint32));
    offset += (sizeof(avi_uint32));
    nCount += (sizeof(avi_uint32));
  }

  avi_uint16 trackId,cType;

  memcpy(&trackId,(unsigned char*)&dwChunkId,sizeof(avi_uint16));
  trackId = ascii_2_short_int(&trackId);
  memcpy(&cType,((unsigned char*)&dwChunkId)+2,sizeof(avi_uint16));

  /*
  * We do not store Field Index as of now.
  */
  if( (trackId < AVI_MAX_TRACKS)&&(bIndexSubType !=AVI_INDEX_2FIELD) )
  {
    //Store Super Index for given stream#.
    if(bIndexType == AVI_INDEX_OF_INDEXES)
    {
      m_start_data_offset_base_indx_tbl[trackId] = nCount + baseOffset;
      m_base_indx_tbl[trackId].wLongsPerEntry = wLongsPerEntry;
	    m_base_indx_tbl[trackId].bIndexSubType = bIndexSubType;
	    m_base_indx_tbl[trackId].bIndexType = bIndexType;
	    m_base_indx_tbl[trackId].nEntriesInUse = nEntriesInUse;
	    m_base_indx_tbl[trackId].dwChunkId = dwChunkId;
	    m_base_indx_tbl[trackId].dwReserved[0] = dwReserved_arr[0];
	    m_base_indx_tbl[trackId].dwReserved[1] = dwReserved_arr[1];
	    m_base_indx_tbl[trackId].dwReserved[2] = dwReserved_arr[2];
    }
    //Store Standard Index of Chunks for given stream#.
    if(bIndexType == AVI_INDEX_OF_CHUNKS)
    {
      m_avi_index_of_chunks_tbl[trackId].fcc = fourcc;
      m_avi_index_of_chunks_tbl[trackId].cb = cb;
      m_avi_index_of_chunks_tbl[trackId].wLongsPerEntry = wLongsPerEntry;
	    m_avi_index_of_chunks_tbl[trackId].bIndexSubType = bIndexSubType;
	    m_avi_index_of_chunks_tbl[trackId].bIndexType = bIndexType;
	    m_avi_index_of_chunks_tbl[trackId].nEntriesInUse = nEntriesInUse;
	    m_avi_index_of_chunks_tbl[trackId].dwChunkId = dwChunkId;
      m_avi_index_of_chunks_tbl[trackId].qwBaseOffset = qwBaseOffset;
	    m_avi_index_of_chunks_tbl[trackId].dwReserved = dwReserved;

      avi_int32 dwOffset,dwSize;
      #ifdef WALK_INDEX_TABLES
        for(int i = 0; i < nEntriesInUse; i++)
        {
      #endif
          if(!AVICallbakGetData(offset,(2*sizeof(avi_uint32)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
          {
            QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "parseIX AVI_READ_FAILURE");
            retError = AVI_READ_FAILURE;
            return AVI_READ_FAILURE;
          }
          offset += (2*sizeof(avi_uint32));
          nCount += (2*sizeof(avi_uint32));
          memcpy(&dwOffset,byteData,sizeof(avi_int32));
          //bit 31 is set if this is not a key frame. Mask 31st bit to get actual size.
          memcpy(&dwSize,byteData+sizeof(avi_int32),sizeof(avi_int32));
          bool bisKey = dwSize & 0x8000;
          dwSize = dwSize & 0x7FFFF;

          /*
          * dwOffset+qwBaseOffset points to actual media data.
          * Need to go back by sizeof(fourCC) + sizeof(uint32)to point to
          * data chunk header(for example ##dc,##db,##wb etc.)
          */
        #ifndef WALK_INDEX_TABLES
          m_start_data_offset_avi_index_of_chunks_tbl[trackId] = (dwOffset+qwBaseOffset) - ((sizeof(fourCC_t)+sizeof(avi_uint32)));
          QTV_MSG_PRIO4(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "parseIX:AVI Standard Index Chunk Entry %ld offset %ld size %ld KeyFrame %ld",0,m_start_data_offset_avi_index_of_chunks_tbl[trackId],dwSize,bisKey);
        #else
          if(i == 0)
          {
            m_start_data_offset_avi_index_of_chunks_tbl[trackId] = (dwOffset+qwBaseOffset) - ((sizeof(fourCC_t)+sizeof(avi_uint32)));
          }
          QTV_MSG_PRIO4(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "parseIX:AVI Standard Index Chunk Entry %ld offset %ld size %ld KeyFrame %ld",i,m_start_data_offset_avi_index_of_chunks_tbl[trackId],dwSize,bisKey);
        #endif
      #ifdef WALK_INDEX_TABLES
        }//for(int i = 0; i < nEntriesInUse; i++)
      #endif
    }//if(bIndexType == AVI_INDEX_OF_CHUNKS)
    //'cb' does not include #bytes used in fcc and cb itself.
    //So, Need to add those sizes to determine total bytes to jump for this 'ix##'.
  }//if( (trackId >= 0)&&(bIndexSubType !=AVI_INDEX_2FIELD) )
  *ixsize = cb+sizeof(fourCC_t)+sizeof(avi_int32);
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::updateSampleRetrievedOffset

DESCRIPTION:
 Update the offset in IDX1 to match the current audio/video sample retrieved.
 This is used when seeking the stream.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::updateSampleRetrievedOffset(CHUNK_t chunktype,avi_uint8 trid)
{
  aviErrorType retError = AVI_SUCCESS;
  unsigned char* byteData = m_ReadBuffer;
  //avi_uint32 nSize = 0;

  avi_uint16 trackId = 0;
  avi_uint16 cType = 0;
  avi_uint32 dwFlags = 0;
  avi_uint32 dwOffset = 0;
  avi_uint32 dwSize = 0;

  if(!m_nCurrentSampleInfoOffsetInIdx1)
  {
     QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "updateSampleRetrievedOffset IDX1 does not exist!!");
     return retError;
  }
  while(true)
  {
    if(!AVICallbakGetData(m_nCurrentSampleInfoOffsetInIdx1,4 * sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "updateSampleRetrievedOffset AVI_READ_FAILURE");
      retError = AVI_READ_FAILURE;
      return retError;
    }
    #ifdef AVI_PARSER_DEBUG
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                    "updateSampleRetrievedOffset m_nCurrentSampleInfoOffsetInIdx1 %d",
                     m_nCurrentSampleInfoOffsetInIdx1);
    #endif
    m_nCurrentSampleInfoOffsetInIdx1 += (4 *sizeof(avi_uint32));
    memcpy(&trackId,byteData,sizeof(avi_uint16));
    trackId = ascii_2_short_int(&trackId);
    memcpy(&cType,byteData+2,sizeof(avi_uint16));
    memcpy(&dwFlags,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
    memcpy(&dwOffset,byteData+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
    memcpy(&dwSize,byteData+(3*sizeof(avi_uint32)),sizeof(avi_uint32));

    if(!m_bByteAdjustedForMOVI)
    {
      if( m_nMoviOffset != dwOffset )
      {
        m_nBytesToBeAdjustedForMOVI = dwOffset;
        m_bByteAdjustedForMOVI = true;
        #ifdef AVI_PARSER_DEBUG
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                        "updateSampleRetrievedOffset adjusted m_nBytesToBeAdjustedForMOVI %d",
                        m_nBytesToBeAdjustedForMOVI);
        #endif
      }
    }
    if(trackId == trid)
    {
      if(chunktype == AVI_CHUNK_VIDEO)
      {
        m_nCurrVideoSampleInIdx1++;
        #ifdef AVI_PARSER_DEBUG
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                        "updateSampleRetrievedOffset m_nCurrVideoSampleInIdx1%d", m_nCurrVideoSampleInIdx1);
        #endif
        if(m_nCurrVideoFrameCount[trid] == m_nCurrVideoSampleInIdx1)
        {
          #ifdef AVI_PARSER_DEBUG
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                          "updateSampleRetrievedOffset located current VIDEO sample in IDX1 %d", m_nCurrVideoSampleInIdx1);
          #endif
          break;
        }
      }//if(chunktype == AVI_CHUNK_VIDEO)
      if(chunktype == AVI_CHUNK_AUDIO)
      {
        m_nCurrAudioSampleInIdx1++;
        #ifdef AVI_PARSER_DEBUG
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                        "updateSampleRetrievedOffset m_nCurrAudioSampleInIdx1%d", m_nCurrAudioSampleInIdx1);
        #endif
        if(m_nCurrAudioFrameCount[trid] == m_nCurrAudioSampleInIdx1)
        {
          #ifdef AVI_PARSER_DEBUG
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                          "updateSampleRetrievedOffset located current AUDIO sample in IDX1 %d", m_nCurrAudioSampleInIdx1);
          #endif
          break;
        }
      }//if(chunktype == AVI_CHUNK_AUDIO)
    }//if(trackId == trid)
  }//while(true)
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::SkipToNextValidMediaChunk

DESCRIPTION:
 Scans IDX1 table to locate first valid media chunk

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
avi_uint64 aviParser::skipToNextValidMediaChunk(avi_uint64 offset)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint16 cType = 0;
  avi_uint16 trackId = 0;
  avi_uint32 dwFlags = 0;
  avi_uint32 dwOffset = 0;
  avi_uint32 dwSize = 0;
  avi_uint64 nOffsetRet;
  int nItr = 0;

  do
  {
    if(!AVICallbakGetData(offset,4 * sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "SkipToNextValidMediaChunk AVI_READ_FAILURE");
      return AVI_READ_FAILURE;
    }
    memcpy(&trackId,byteData,sizeof(avi_uint16));
    trackId = ascii_2_short_int(&trackId);
    memcpy(&cType,byteData+2,sizeof(avi_uint16));
    memcpy(&dwFlags,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
    memcpy(&dwOffset,byteData+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
    memcpy(&dwSize,byteData+(3*sizeof(avi_uint32)),sizeof(avi_uint32));
    offset += (4 *sizeof(avi_uint32));
    nItr++;
  }while (!( (!memcmp(&cType,"wb",sizeof(avi_uint16))) || (!memcmp(&cType,"dc",sizeof(avi_uint16))) || (!memcmp(&cType,"db",sizeof(avi_uint16))) || (!memcmp(&cType,"dd",sizeof(avi_uint16))) ));
  if(nItr > 1)
  {
    m_nCurrentSampleInfoOffsetInIdx1 = m_nIdx1Offset + ( (nItr-1) * (4 * sizeof(avi_uint32)) );
    m_nAdjustedIdx1Offset = m_nCurrentSampleInfoOffsetInIdx1;
  }
  else
  {
    m_nCurrentSampleInfoOffsetInIdx1 = m_nIdx1Offset;
    m_nAdjustedIdx1Offset = 0;
  }
  if(m_bByteAdjustedForMOVI)
  {
    nOffsetRet = m_nStartOfMovi + dwOffset - m_nBytesToBeAdjustedForMOVI;
  }
  else
  {
    nOffsetRet = dwOffset;
  }

  QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "SkipToNextValidMediaChunk nOffsetRet %d m_nCurrOffset %d", nOffsetRet,m_nCurrOffset);

   QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "SkipToNextValidMediaChunk m_nCurrentSampleInfoOffsetInIdx1 %d m_nIdx1Offset %d",
                m_nCurrentSampleInfoOffsetInIdx1,m_nIdx1Offset);

   QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "SkipToNextValidMediaChunk m_nAdjustedIdx1Offset %d m_nIdx1Offset %d",
                m_nAdjustedIdx1Offset,m_nIdx1Offset);
  return nOffsetRet;
}
/* =============================================================================
FUNCTION:
  aviParser::parseIDX1

DESCRIPTION:
 Reads and parse index header

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseIDX1(avi_uint64* offset)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint32 nSize = 0;
  avi_uint16 cType = 0;
  avi_uint16 trackId = 0;
  avi_uint32 dwFlags = 0;
  avi_uint32 dwOffset = 0;
  avi_uint32 dwSize = 0;

  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "Entered parseIDX1");
  if(!AVICallbakGetData(*offset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "parseIDX1 AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  *offset += sizeof(avi_uint32);
  memcpy(&nSize,byteData,sizeof(avi_uint32));
  if(m_bDiscardAudioIndex && m_bDiscardVideoIndex)
  {
    //Non audio/video instance.Right now,there is no support for such streams.
    //No need to parse further.
    m_nIdx1Offset = *offset;
    m_nCurrentSampleInfoOffsetInIdx1 = m_nIdx1Offset;
    m_nIdx1Size = nSize;
    *offset += nSize;
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                    "Not parsing IDX1 as m_bDiscardAudioIndex && m_bDiscardVideoIndex is TRUE..");
    return AVI_SUCCESS;
  }

    bool bOK = false;
    bool bTkID = false;
    bool bFirst = true;
    CHUNK_t Type;

    while(!bOK)
    {
      if(!AVICallbakGetData(*offset,4 * sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
      {
         QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "parseIDX1 AVI_READ_FAILURE");
        return AVI_READ_FAILURE;
      }
      memcpy(&trackId,byteData,sizeof(avi_uint16));
      trackId = ascii_2_short_int(&trackId, &bTkID);

      if(GetTrackChunkType(trackId,&Type)!=AVI_SUCCESS)
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "parseIDX1::unknown chunk type");
      }

      memcpy(&cType,byteData+2,sizeof(avi_uint16));
      memcpy(&dwFlags,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
      memcpy(&dwOffset,byteData+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
      memcpy(&dwSize,byteData+(3*sizeof(avi_uint32)),sizeof(avi_uint32));

      if(bFirst)
      {
        if(( ( (!memcmp(&cType,"dc",sizeof(avi_uint16))) || (!memcmp(&cType,"db",sizeof(avi_uint16))) ||
              (!memcmp(&cType,"wb",sizeof(avi_uint16))) || (!memcmp(&cType,"dd",sizeof(avi_uint16)))
            ) && (((Type == AVI_CHUNK_VIDEO)||(Type ==AVI_CHUNK_AUDIO)) && (bTkID == true))) ||
            (!memcmp(byteData,AVI_JUNK_FOURCC,sizeof(fourCC_t))) ||
            (!memcmp(byteData,AVI_RES_FOURCC,3*sizeof(char))))
        {
            if(!m_bByteAdjustedForMOVI)
            {
              if( m_nStartOfMovi != dwOffset )
              {
                m_nBytesToBeAdjustedForMOVI = dwOffset;
                m_bByteAdjustedForMOVI = true;
                #ifdef FEATURE_FILESOURCE_AVI_PARSER_DEBUG
#error code not present
                #endif
              }
            }
        }
        bFirst = false;
      }

      if( ( (!memcmp(&cType,"dc",sizeof(avi_uint16))) || (!memcmp(&cType,"db",sizeof(avi_uint16))) ||
            (!memcmp(&cType,"wb",sizeof(avi_uint16))) || (!memcmp(&cType,"dd",sizeof(avi_uint16)))
          ) && (((Type == AVI_CHUNK_VIDEO)||(Type ==AVI_CHUNK_AUDIO)) && (bTkID == true))
        )
      {
        bOK = true;
      }
      else if( (!memcmp(byteData,AVI_JUNK_FOURCC,sizeof(fourCC_t))) || (!memcmp(byteData,AVI_RES_FOURCC,3*sizeof(char))) )
      {
        *offset += (4 *sizeof(avi_uint32));
        nSize -= (4 *sizeof(avi_uint32));
        continue;
      }
      else
      {
        *offset += sizeof(fourCC_t);
        nSize -= sizeof(fourCC_t);
      }
    }

    for(int i = 0; i< AVI_MAX_TRACKS; i++)
    {
      if(m_hAviSummary.stream_index[i].type == AVI_CHUNK_VIDEO)
      {
        (void)GetAVIVolHeader(m_hAviSummary.stream_index[i].index);
      }
    }
    //nOffset is pointing to first entry in IDX1
    m_nIdx1Offset = *offset;
    /*
    * Initialized to first entry in IDX1.
    * Variable gets updated inline with current sample being retrieved.
    * This helps to avoid scannig IDX1 from the begining when seeking.
    */
    m_nCurrentSampleInfoOffsetInIdx1 = m_nIdx1Offset;
    m_nIdx1Size = nSize;

    *offset += nSize;


    #ifdef AVI_PARSER_DEBUG
      QTV_MSG_PRIO4(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                    "StartParsing done parsing IDX1 at offset %d size %d m_nCurrentSampleInfoOffsetInIdx1 %d m_nIdx1Size %d",
                    *offset,nSize,m_nCurrentSampleInfoOffsetInIdx1,m_nIdx1Size);
    #endif

#ifndef AVI_PARSER_FAST_START_UP
  (void)cacheIDX1(&m_nIdx1Offset,nSize);
#else
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"Not parsing rest of the IDX1 for FAST_START_UP!!");
#endif
    return AVI_SUCCESS;
}
/* =============================================================================
FUNCTION:
  aviParser::SetIDX1Cache

DESCRIPTION:
 Copies the given idx1 cache pointer into this cache pointer

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
void  aviParser::SetIDX1Cache(void* ptr)
{
  avi_idx1_table* table_ptr = (avi_idx1_table*)ptr;
  //bool bContinue = true;
  //aviErrorType retError = AVI_SUCCESS;

  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "SetIDX1Cache");
  if(table_ptr && table_ptr->pAudioEntries)
  {
    m_hAviSummary.pIdx1Table = (avi_idx1_table*)QTV_Malloc(sizeof(avi_idx1_table));
    if(!m_hAviSummary.pIdx1Table)
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "SetIDX1Cache:Malloc Failed for pIdx1Table-AVI_OUT_OF_MEMORY");
		  //bContinue = false;
		  //retError = AVI_OUT_OF_MEMORY;
    }
	  else
	  {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "SetIDX1Cache pIdx1Table is allocated");
      memset(m_hAviSummary.pIdx1Table,0,sizeof(avi_idx1_table));

      m_hAviSummary.pIdx1Table->nTotalSize = table_ptr->nTotalSize;
      m_hAviSummary.pIdx1Table->nStartOffset = table_ptr->nStartOffset;
      m_hAviSummary.pIdx1Table->pAudioEntries = (avi_idx1_entry*)QTV_Malloc(sizeof(avi_idx1_entry) * table_ptr->nAudioEntriesAllocated);
      m_hAviSummary.pIdx1Table->nTotalAudioBytes = table_ptr->nTotalAudioBytes;
      if(m_hAviSummary.pIdx1Table->pAudioEntries)
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "SetIDX1Cache pIdx1Table->pAudioEntries is allocated");
        memset(m_hAviSummary.pIdx1Table->pAudioEntries, 0, sizeof(avi_idx1_entry) * table_ptr->nAudioEntriesAllocated);
        m_hAviSummary.pIdx1Table->nAudioEntriesAllocated = table_ptr->nAudioEntriesAllocated;
        m_hAviSummary.pIdx1Table->nCurrParsedAudioEntriesInIDX1 = table_ptr->nCurrParsedAudioEntriesInIDX1;
        m_hAviSummary.pIdx1Table->nTotalEntriesInIDX1= table_ptr->nTotalEntriesInIDX1;
        m_hAviSummary.pIdx1Table->nAudioEntriesInIDX1= table_ptr->nAudioEntriesInIDX1;
        memcpy(m_hAviSummary.pIdx1Table->pAudioEntries,
               table_ptr->pAudioEntries,
               (sizeof(avi_idx1_entry) * table_ptr->nAudioEntriesAllocated) );
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "SetIDX1Cache pAudioEntries set up successful!!");
        table_ptr->nAudioEntriesAllocated = 0;
        table_ptr->nCurrParsedAudioEntriesInIDX1 = 0;
        table_ptr->nAudioEntriesInIDX1 = 0;
        QTV_Free(table_ptr->pAudioEntries);
        table_ptr->pAudioEntries = NULL;
      }
      else
      {
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "SetIDX1Cache:Malloc Failed for pAudioEntries #entries %d",table_ptr->nAudioEntriesAllocated);
        QTV_Free(m_hAviSummary.pIdx1Table);
        m_hAviSummary.pIdx1Table = NULL;
        table_ptr->nAudioEntriesAllocated = 0;
        table_ptr->nCurrParsedAudioEntriesInIDX1 = 0;
        table_ptr->nAudioEntriesInIDX1 = 0;
        QTV_Free(table_ptr->pAudioEntries);
        table_ptr->pAudioEntries = NULL;
      }
    }
  }
}
/* =============================================================================
FUNCTION:
  aviParser::GetIDX1Cache

DESCRIPTION:
 Returns this cache pointer

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
void* aviParser::GetIDX1Cache()
{
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "GetIDX1Cache");
  return (void*)m_hAviSummary.pIdx1Table;
}
/* =============================================================================
FUNCTION:
  aviParser::cacheIDX1

DESCRIPTION:
 Reads and cache in entire idx1 table to reduce seek delay.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
#ifndef AVI_PARSER_FAST_START_UP
aviErrorType aviParser::cacheIDX1(avi_uint64* offset,avi_uint32 idx1size)
#else
aviErrorType aviParser::cacheIDX1(avi_uint64*,avi_uint32)
#endif
{
aviErrorType retError = AVI_SUCCESS;
#ifndef AVI_PARSER_FAST_START_UP
  avi_uint32 nSize = 0;
  avi_uint16 cType = 0;
  avi_uint16 trackId = 0;
  avi_uint32 dwFlags = 0;
  avi_uint32 dwOffset = 0;
  avi_uint32 dwSize = 0;
  avi_uint32 nTotalEntries = 0;
  avi_uint32 nAudioIndex = 0;
  avi_uint32 nVideoIndex = 0;
  avi_uint64 audioBytesFromStart = 0;
  avi_uint64 tsAudio = 0;
  avi_uint64 prevTSAudio  = 0;
  bool bContinue = true;
  avi_uint64 tmpoffset = *offset;
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "Entered cacheIDX1");
  avi_uint32 readIndex = 0;

  int nTSIndexToStore = 0;
  int videotrackidtoCache = -1;
  int audiotrackidtoCache = -1;

  if(m_bDiscardAudioIndex == false)
  {
    //we don't parse idx1 in audio instance.
    //It will be parsed in video instance and will be pushed into aviparser audio instance.
    return retError;
  }
  if(m_hAviSummary.pIdx1Table)
  {
    //Weird,this should never happen.Print F3 for debug and Free the memory.
    QTV_Free(m_hAviSummary.pIdx1Table);
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "Deleting m_hAviSummary.pIdx1Table and allocating...");
  }
	//Allocate the memory for idx1 table
  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "trying to allocate memory for idx1 table %d",(sizeof(avi_idx1_table)) );
  m_hAviSummary.pIdx1Table = (avi_idx1_table*)QTV_Malloc(sizeof(avi_idx1_table));
  if(!m_hAviSummary.pIdx1Table)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "Malloc Failed for pIdx1Table-AVI_OUT_OF_MEMORY");
		bContinue = false;
		retError = AVI_OUT_OF_MEMORY;
  }
	else
	{
	  //Record the start offset for the idx1 read above and it's total size.
    memset(m_hAviSummary.pIdx1Table,0,sizeof(avi_idx1_table));
    m_hAviSummary.pIdx1Table->nTotalSize = idx1size;
    m_hAviSummary.pIdx1Table->nStartOffset = *offset;

		//Each idx1 entry has a constant size.Calculate total number of entries that exist in idx1.
    nTotalEntries = idx1size/AVI_IDX1_ENTRY_SIZE;
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "nTotalEntries calculated %d",nTotalEntries);
		//Make sure there are non zero entries in idx1
    if(!nTotalEntries)
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "nTotalEntries is 0");
		  bContinue = false;
    }
    /*
		* Allocate memory for storing audio index table only if clip has audio and parser is told not to
		* discard audio index entries.(AUDIO INSTANCE).
		* To save memory, we don't need to store video entries in audio instance.
		*/
    if( (m_hAviSummary.n_audio_tracks)            &&
        (!m_hAviSummary.pIdx1Table->pAudioEntries)&&
		 	  (bContinue)
      )
    {
      /*
      * We always play the first audio track from AVI/DIVX FILE.
      * Storing each audio entry consumes too much memory.
      * We save the audio sample at every AUDIO_SAMPLE_CACHING_INTERVAL to reduce memory requirement.
      * Get the duration of the first audio track
      */
      for(int i =0; i< (int)m_hAviSummary.n_streams; i++)
      {
        if(m_hAviSummary.stream_index[i].type == AVI_CHUNK_AUDIO)
        {
          audiotrackidtoCache = i;
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "cacheIDX1 caching audio trackid %d",i);
          break;
        }
      }
      if(audiotrackidtoCache == -1)
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "Failed to locate audio track id when n_audio_tracks > 0");
        bContinue = false;
        retError = AVI_PARSE_ERROR;
      }
      else
      {
        avi_uint64 duration = GetTrackDuration(audiotrackidtoCache);
        int nAudioEntries = (int)(duration / AUDIO_SAMPLE_CACHING_INTERVAL)+1;
		  if(nAudioEntries > 0)
		  {
          int size = sizeof(avi_idx1_entry);
          QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "trying to allocate memory for storing AudioEntries %d %d",nAudioEntries,(sizeof(avi_idx1_entry) * nAudioEntries));
			  //make sure there are non zero audio entries to cache.
        m_hAviSummary.pIdx1Table->pAudioEntries =
			    (avi_idx1_entry*)QTV_Malloc(sizeof(avi_idx1_entry) * nAudioEntries);

			  if(!m_hAviSummary.pIdx1Table->pAudioEntries)
        {
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "Malloc failed for audio IDX1 entries %d",nAudioEntries);
          retError = AVI_OUT_OF_MEMORY;
			    bContinue = false;
        }
			  else
			  {
          memset(m_hAviSummary.pIdx1Table->pAudioEntries, 0, sizeof(avi_idx1_entry) * nAudioEntries);
          m_hAviSummary.pIdx1Table->nAudioEntriesAllocated = nAudioEntries;
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "ParseIDX1 nAudioEntries %d",nAudioEntries);
			    }
			  }
      }
    }
    /*
		* Allocate only if clip has video and parser is told not to
		* discard video index entries.(VIDEO INSTANCE)
		*/
    if((m_hAviSummary.n_video_tracks)                &&
       (!m_hAviSummary.pIdx1Table->pKeyVideoEntries) &&
		   (bContinue)
      )
    {
		  for(int i =0; i< (int)m_hAviSummary.n_streams; i++)
      {
        if(m_hAviSummary.stream_index[i].type == AVI_CHUNK_VIDEO)
        {
          videotrackidtoCache = i;
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "cacheIDX1 caching video trackid %d",i);
          break;
        }
      }
      int nVideoEntries = 0;
      if(videotrackidtoCache != -1)
      {
        nVideoEntries = m_hAviSummary.video_info[videotrackidtoCache].strhVideo.dwLength /1000;
      }
  	  //Make sure there are non zero video entries to cache
      if(nVideoEntries)
		  {
        QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "trying to allocate memory for storing VideoKeyEntries %d %d,",nVideoEntries,(sizeof(avi_idx1_entry) * nVideoEntries));
        m_hAviSummary.pIdx1Table->pKeyVideoEntries = (avi_idx1_entry*)QTV_Malloc(sizeof(avi_idx1_entry) * nVideoEntries);
			  if(!m_hAviSummary.pIdx1Table->pKeyVideoEntries)
        {
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "Malloc failed for video IDX1 entries %d",nVideoEntries);
			    retError = AVI_OUT_OF_MEMORY;
			    bContinue = false;
			  }
			  else
			  {
			    memset(m_hAviSummary.pIdx1Table->pKeyVideoEntries,0,sizeof(avi_idx1_entry) * nVideoEntries);
          m_hAviSummary.pIdx1Table->nVideoEntriesAllocated = nVideoEntries;
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "ParseIDX1 nVideoEntries %d",nVideoEntries);
        }
      }
    }
    //check if any error has occured above(bContinue will be false) or memory allocation fail.
    if( (!bContinue)                                         ||
		    ( (!m_hAviSummary.pIdx1Table->pKeyVideoEntries)&&
        (!m_hAviSummary.pIdx1Table->pAudioEntries) )
		  )
    {
      //No need to cache, just print the message
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "cacheIDX1 aborting caching!!!");
    }

    while( (tmpoffset < (*offset + idx1size)) && (bContinue) )
    {
      readIndex = 0;
		  if(!AVICallbakGetData(tmpoffset,(4*sizeof(avi_uint32)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "cacheIDX1 AVI_READ_FAILURE");
        bContinue = false;
      }
		  //read in idx1 entry
      memcpy(&trackId,m_ReadBuffer+readIndex,sizeof(avi_uint16));
      trackId = ascii_2_short_int(&trackId);
		  readIndex+=sizeof(avi_uint16);

      memcpy(&cType,m_ReadBuffer+readIndex,sizeof(avi_uint16));
		  readIndex+=sizeof(avi_uint16);

      memcpy(&dwFlags,m_ReadBuffer+readIndex,sizeof(avi_uint32));
		  readIndex+=sizeof(avi_uint32);

      memcpy(&dwOffset,m_ReadBuffer+readIndex,sizeof(avi_uint32));
		  readIndex+=sizeof(avi_uint32);

      memcpy(&dwSize,m_ReadBuffer+readIndex,sizeof(avi_uint32));
		  readIndex+=sizeof(avi_uint32);
		  tmpoffset += (4*sizeof(avi_uint32));

		  //If idx1 entry denotes video chunk, check if it's a key frame
      if(  ( (!memcmp(&cType,"dc",sizeof(avi_uint16))) ||(!memcmp(&cType,"db",sizeof(avi_uint16))) ) &&
           (m_hAviSummary.pIdx1Table->pKeyVideoEntries ) && (bContinue) && (videotrackidtoCache == trackId) )
      {
        if(dwFlags & AVI_KEY_FRAME_MASK)
        {
			    //video chunk contains key-frame.Make sure there is room for storage,otherwise, do the re-alloc.
          if( nVideoIndex >= m_hAviSummary.pIdx1Table->nVideoEntriesAllocated )
          {
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "cacheIDX1 doing realloc for video curr #entries ",
                            m_hAviSummary.pIdx1Table->nVideoEntriesAllocated);
            avi_idx1_entry* pTemp = NULL;
            pTemp = (avi_idx1_entry*)realloc(m_hAviSummary.pIdx1Table->pKeyVideoEntries,
                                             (sizeof(avi_idx1_entry) * (m_hAviSummary.pIdx1Table->nVideoEntriesAllocated * 2)) );
            if(!pTemp)
            {
              QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                              "cacheIDX1 realloc failed for video curr #entries ",
                               m_hAviSummary.pIdx1Table->nVideoEntriesAllocated);
              QTV_Free(m_hAviSummary.pIdx1Table->pKeyVideoEntries);
              m_hAviSummary.pIdx1Table->pKeyVideoEntries = NULL;
              m_hAviSummary.pIdx1Table->nVideoEntriesAllocated = 0;
              retError = AVI_OUT_OF_MEMORY;
				      bContinue = false;
            }
				    else
				    {
              m_hAviSummary.pIdx1Table->pKeyVideoEntries = pTemp;
              m_hAviSummary.pIdx1Table->nVideoEntriesAllocated *= 2;
            }
          }
			    //Check for pointer as re-alloc can fail.
			    if(m_hAviSummary.pIdx1Table->pKeyVideoEntries)
			    {
            m_hAviSummary.pIdx1Table->pKeyVideoEntries[nVideoIndex].chunkType = AVI_CHUNK_VIDEO;
            m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1++;
            m_hAviSummary.pIdx1Table->pKeyVideoEntries[nVideoIndex].dwVidFrameCount =
                                       m_hAviSummary.pIdx1Table->nVideoEntriesInIDX1;
            m_hAviSummary.pIdx1Table->pKeyVideoEntries[nVideoIndex].dwFlags = dwFlags;

			      if(m_bByteAdjustedForMOVI)
            {
              m_hAviSummary.pIdx1Table->pKeyVideoEntries[nVideoIndex].dwOffset =
				  	  (avi_uint32)(m_nStartOfMovi + dwOffset - m_nBytesToBeAdjustedForMOVI);
            }
			      else
			      {
              m_hAviSummary.pIdx1Table->pKeyVideoEntries[nVideoIndex].dwOffset = dwOffset;
            }
            m_hAviSummary.pIdx1Table->pKeyVideoEntries[nVideoIndex].dwSize = dwSize;
            m_hAviSummary.pIdx1Table->pKeyVideoEntries[nVideoIndex].trackId = trackId;
            m_hAviSummary.pIdx1Table->pKeyVideoEntries[nVideoIndex].dwOffsetInIDX1 = (tmpoffset - (4*sizeof(avi_uint32)) );

            avi_video_info vinfo;
            if( (GetVideoInfo(trackId,&vinfo)==AVI_SUCCESS) && (vinfo.strhVideo.dwRate) )
            {
              //dwScale/dwRate gives framerate in fps.
              m_hAviSummary.pIdx1Table->pKeyVideoEntries[nVideoIndex].nTimeStamp =
              ((float)vinfo.strhVideo.dwScale/(float)vinfo.strhVideo.dwRate)*
              (float)m_hAviSummary.pIdx1Table->pKeyVideoEntries[nVideoIndex].dwVidFrameCount;
              //Convert to time in milliseconds.
              m_hAviSummary.pIdx1Table->pKeyVideoEntries[nVideoIndex].nTimeStamp*=1000;
            }
            nVideoIndex++;
          }
        }
        m_hAviSummary.pIdx1Table->nVideoEntriesInIDX1++;
      }//if video entry
      else if( (!memcmp(&cType,"wb",sizeof(avi_uint16))) &&
               (m_hAviSummary.pIdx1Table->pAudioEntries) &&
               (bContinue)                               &&
               (trackId == audiotrackidtoCache) )
		  {
		    //make sure there is a room to store audio entry.If not do the re-alloc.
        if( nAudioIndex >= m_hAviSummary.pIdx1Table->nAudioEntriesAllocated )
        {
          #ifdef AVI_PARSER_DEBUG
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "parseIDX1 doing realloc for audio curr #entries ",
                            m_hAviSummary.pIdx1Table->nAudioEntriesAllocated);
          #endif
          avi_idx1_entry* pTemp = NULL;
          pTemp = (avi_idx1_entry*)realloc(m_hAviSummary.pIdx1Table->pAudioEntries,
                                          (sizeof(avi_idx1_entry)*(m_hAviSummary.pIdx1Table->nAudioEntriesAllocated * 2)) );
          if(!pTemp )
          {
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "parseIDX1 realloc failed for audio curr #entries ",
                            m_hAviSummary.pIdx1Table->nAudioEntriesAllocated);
            QTV_Free(m_hAviSummary.pIdx1Table->pAudioEntries);
            m_hAviSummary.pIdx1Table->pAudioEntries = NULL;
            m_hAviSummary.pIdx1Table->nAudioEntriesAllocated = 0;
            bContinue = false;
				    retError = AVI_OUT_OF_MEMORY;
			    }
			    else
			    {
            m_hAviSummary.pIdx1Table->pAudioEntries = pTemp;
            m_hAviSummary.pIdx1Table->nAudioEntriesAllocated *= 2;
          }
        }
        avi_audiotrack_summary_info ainfo;
			  //check for pointer as re-alloc can fail
			  if(m_hAviSummary.pIdx1Table->pAudioEntries)
			  {
          //This should always succeed, if not there is a problem in initial parsing.
          if(GetAudioTrackSummaryInfo(trackId,&ainfo)==AVI_SUCCESS)
          {
            if( (!ainfo.isVbr)&&(ainfo.audioBytesPerSec) )
            {
              tsAudio = (avi_uint32)((float)audioBytesFromStart /
                (float)ainfo.audioBytesPerSec * 1000.0f);

              prevTSAudio = tsAudio;

              #ifdef WALK_INDEX_TABLES
                QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                               "parseIDX1 AUDIO TRACK IS CBR TS %d Chunk# %d ",
                               tsAudio,m_hAviSummary.pIdx1Table->nAudioEntriesInIDX1);
              #endif
            }
            else if( ainfo.isVbr && ainfo.audioFrequency )
            {
			        avi_audio_info audinfo;
				      if(GetAudioInfo(trackId,&audinfo)==AVI_SUCCESS)
				      {
                if(audinfo.strhAudio.dwSampleSize > 0)
				        {
                  prevTSAudio = tsAudio;
				          double val1 = ceil( ( (double)dwSize / ainfo.nBlockAlign) );
                  double val2 =  ((double)audinfo.strhAudio.dwScale/audinfo.strhAudio.dwRate)* 1000.f;
                  avi_uint32 duration  =  (uint32)(val1 * val2);
                  tsAudio +=  duration;
                }
				        else
				        {
                  tsAudio = (avi_uint64)(( ( (float)audinfo.strhAudio.dwScale/
                                             (float)audinfo.strhAudio.dwRate) * 1000.f) * (float)m_hAviSummary.pIdx1Table->nAudioEntriesInIDX1);                  \
                  prevTSAudio = tsAudio;
                }
                }
              }
            if( (prevTSAudio / AUDIO_SAMPLE_CACHING_INTERVAL) >= nTSIndexToStore)
            {
              m_hAviSummary.pIdx1Table->pAudioEntries[nAudioIndex].nTimeStamp =
                (float)prevTSAudio;
			        m_hAviSummary.pIdx1Table->pAudioEntries[nAudioIndex].chunkType =
                AVI_CHUNK_AUDIO;
              m_hAviSummary.pIdx1Table->pAudioEntries[nAudioIndex].dwFlags =
                dwFlags;
			        if(m_bByteAdjustedForMOVI)
              {
                m_hAviSummary.pIdx1Table->pAudioEntries[nAudioIndex].dwOffset =
			              (avi_uint32)(m_nStartOfMovi + dwOffset - m_nBytesToBeAdjustedForMOVI);
            }
			        else
			        {
                m_hAviSummary.pIdx1Table->pAudioEntries[nAudioIndex].dwOffset = dwOffset;
          }
              m_hAviSummary.pIdx1Table->pAudioEntries[nAudioIndex].dwOffsetInIDX1 =
                (tmpoffset - (4 * sizeof(avi_uint32)) );
              m_hAviSummary.pIdx1Table->pAudioEntries[nAudioIndex].dwSize = dwSize;
              m_hAviSummary.pIdx1Table->pAudioEntries[nAudioIndex].trackId = trackId;
              m_hAviSummary.pIdx1Table->pAudioEntries[nAudioIndex].dwAudFrameCount =
                              m_hAviSummary.pIdx1Table->nAudioEntriesInIDX1;
              m_hAviSummary.pIdx1Table->pAudioEntries[nAudioIndex].nTotalSizeInBytes =
                audioBytesFromStart;

              nTSIndexToStore = (prevTSAudio / AUDIO_SAMPLE_CACHING_INTERVAL);
              nTSIndexToStore++;
          nAudioIndex++;
          m_hAviSummary.pIdx1Table->nCurrParsedAudioEntriesInIDX1 = nAudioIndex;

              #ifdef WALK_INDEX_TABLES
                QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,"cacheIDX1, AUDIO TS indexed %d nAudioIndex %d",prevTSAudio,nAudioIndex);
              #endif
            }//if( (tsAudio / AUDIO_SAMPLE_CACHING_INTERVAL) == nTSIndexToStore)
          }//if(GetAudioTrackSummaryInfo(trackId,&ainfo)==AVI_SUCCESS)
        }//if(m_hAviSummary.pIdx1Table->pAudioEntries)
          audioBytesFromStart+=dwSize;
			  m_hAviSummary.pIdx1Table->nAudioEntriesInIDX1++;
      }//else if( (!memcmp(&cType,"wb",sizeof(avi_uint16))) && (m_hAviSummary.pIdx1Table->pAudioEntries) && (bContinue) )

      m_hAviSummary.pIdx1Table->nTotalEntriesInIDX1++;
      #ifdef WALK_INDEX_TABLES
       QTV_MSG_PRIO6(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "parseIDX1 nAudioEntriesInIDX1 %d nAudioEntriesLoaded %d nVideoEntriesInIDX1 %d #KeyFrames %d nTotalEntriesInIDX1 %d nSize %d",
                        m_hAviSummary.pIdx1Table->nAudioEntriesInIDX1,
                        m_hAviSummary.pIdx1Table->nCurrParsedAudioEntriesInIDX1,
                        m_hAviSummary.pIdx1Table->nVideoEntriesInIDX1,
                        m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1,
                        m_hAviSummary.pIdx1Table->nTotalEntriesInIDX1,nSize);
      #endif
    }//while( (tmpoffset < (*offset + idx1size)) && (bContinue) )
    //store the total num of audio bytes.
    m_hAviSummary.pIdx1Table->nTotalAudioBytes = audioBytesFromStart;
	}//end of else of if(!m_hAviSummary.pIdx1Table)
  if(!bContinue)
  {
    //error occured while caching the entries.
    //free up the memory, if any,allocated above.
	  if(m_hAviSummary.pIdx1Table->pAudioEntries)
	  {
	    QTV_Free(m_hAviSummary.pIdx1Table->pAudioEntries);
      m_hAviSummary.pIdx1Table->pAudioEntries = NULL;
	  }
	  if(m_hAviSummary.pIdx1Table->pKeyVideoEntries)
	  {
	    QTV_Free(m_hAviSummary.pIdx1Table->pKeyVideoEntries);
      m_hAviSummary.pIdx1Table->pKeyVideoEntries = NULL;
	  }
	  if(m_hAviSummary.pIdx1Table)
	  {
	    QTV_Free(m_hAviSummary.pIdx1Table);
      m_hAviSummary.pIdx1Table = NULL;
	  }
  }
  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "leaving cacheIDX1 retError %d",retError);
#endif
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseODML

DESCRIPTION:
 Reads and parse ODML header

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseODML(avi_uint64 offset)
{
  int nFrames = 0;
  int nSize=0;
  unsigned char* byteData = m_ReadBuffer;
  #ifdef AVI_PARSER_DEBUG
  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                "parseODML Offset to start %d",offset);
  #endif
  if(!AVICallbakGetData(offset,(2*sizeof(avi_uint32)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                 "parseODML AVI_READ_FAILURE");
    return AVI_READ_FAILURE;
  }
  offset+=(2*sizeof(avi_uint32));
  memcpy(&nSize,byteData,sizeof(avi_uint32));
  memcpy(&nFrames,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
  return AVI_SUCCESS;
}
/* =============================================================================
FUNCTION:
  aviParser::parseINFO

DESCRIPTION:
 Reads and parse INFO header

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::parseINFO(avi_uint64 offset,int size)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint32 nSize = 0;
  fourCC_t fourcc;

  #ifdef AVI_PARSER_DEBUG
  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                "parseINFO Offset to start %d",offset);
  #endif

  while(size > 0)
  {
    //Read INFO Chunk FourCC and Size
    if(!AVICallbakGetData(offset,(sizeof(fourCC_t)+ sizeof(avi_uint32)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "parseINFO AVI_READ_FAILURE");
      return AVI_READ_FAILURE;
    }
    offset+= ((sizeof(fourCC_t)) +(sizeof(avi_uint32)));

    memcpy(&fourcc,byteData,sizeof(fourCC_t));
    memcpy(&nSize,byteData+sizeof(fourCC_t),sizeof(avi_uint32));

    (void)updateInfoChunkInfo(fourcc,nSize,offset);

    if(nSize %2)
    {
      //Must align to 16-bit boundary
      nSize++;
    }
    offset+= nSize;

    size -= (sizeof(uint32)+sizeof(fourCC_t)+nSize);
  }
  return AVI_SUCCESS;
}
/* =============================================================================
FUNCTION:
  aviParser::getDataFromInfo

DESCRIPTION:
 Retrieves required meta data from INFO chunk

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
bool aviParser::getDataFromINFO(char* fourcc, char* ptr, avi_uint16* size)
{
  bool isAvailable = false;
  if((size != NULL) && (fourcc != NULL) )
  {
    if(!memcmp(fourcc,INFO_NAME,sizeof(fourCC_t)))
    {
      *size = m_AviClipMetaInfo.Name.nSize;
      if((ptr != NULL) && (*size>0))
      {
        memcpy(ptr,m_AviClipMetaInfo.Name.Ptr,*size);
      }
      if (*size>0)
      {
        isAvailable = true;
      }
    }
    else if(!memcmp(fourcc,INFO_ARTIST,sizeof(fourCC_t)))
    {
      *size = m_AviClipMetaInfo.Artist.nSize;
      if((ptr != NULL)&& (*size>0))
      {
        memcpy(ptr,m_AviClipMetaInfo.Artist.Ptr,*size);
      }
      if (*size>0)
      {
        isAvailable = true;
      }
    }
    else if(!memcmp(fourcc,INFO_COMMENTS,sizeof(fourCC_t)))
    {
      *size = m_AviClipMetaInfo.Comments.nSize;
      if((ptr != NULL)&& (*size>0))
      {
        memcpy(ptr,m_AviClipMetaInfo.Comments.Ptr,*size);
      }
      if (*size>0)
      {
        isAvailable = true;
      }
    }
    else if(!memcmp(fourcc,INFO_COPYRIGHT,sizeof(fourCC_t)))
    {
      *size = m_AviClipMetaInfo.Copyright.nSize;
      if((ptr != NULL)&& (*size>0))
      {
        memcpy(ptr,m_AviClipMetaInfo.Copyright.Ptr,*size);
      }
      if (*size>0)
      {
        isAvailable = true;
      }
    }
    else if(!memcmp(fourcc,INFO_CREATION_DATE,sizeof(fourCC_t)))
    {
      *size = m_AviClipMetaInfo.CreateDate.nSize;
      if((ptr != NULL)&& (*size>0))
      {
        memcpy(ptr,m_AviClipMetaInfo.CreateDate.Ptr,*size);
      }
      if (*size>0)
      {
        isAvailable = true;
      }
    }
    else
      isAvailable = false;
  }
  else
  {
    isAvailable = false;
  }
  return isAvailable;
}
/* =============================================================================
FUNCTION:
  aviParser::updateInfoChunkInfo

DESCRIPTION:
 Stores the info for chunk identified by fourcc.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::updateInfoChunkInfo(fourCC_t fourcc,avi_uint32 nsize,int offset)
{

  #ifdef AVI_PARSER_DEBUG
  QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                "updateInfoChunkInfo fourcc %d nsize %d",fourcc,nsize);
  #endif

  if(nsize > 0)
  {
    avi_info_chunk* pChunkInfo = getInfoChunkHandle(fourcc);
    if(!pChunkInfo)
    {
      //Error occured or parser does not support given fourcc.
      //Continue but just flag the message.
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                     "updateInfoChunkInfo getInfoChunkHandle return NULL");
    }
    else
    {
      if(pChunkInfo->Ptr)
      {
        QTV_Free(pChunkInfo->Ptr);
      }
      pChunkInfo->Ptr   = (char*)QTV_Malloc(nsize);
      if(!pChunkInfo->Ptr)
      {
        return AVI_OUT_OF_MEMORY;
      }
      //Read 'nsize' data from file.
      if(!AVICallbakGetData(offset,nsize,(unsigned char*)pChunkInfo->Ptr,nsize,m_pUserData))
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                     "updateInfoChunkInfo AVI_READ_FAILURE");
        return AVI_READ_FAILURE;
      }
      pChunkInfo->nSize = nsize;
    }
  }
  return AVI_SUCCESS;
}
/* =============================================================================
FUNCTION:
  aviParser::getInfoChunkHandle

DESCRIPTION:
 Returns the chunk info handle for given chunk id.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
avi_info_chunk* aviParser::getInfoChunkHandle(fourCC_t fourcc)
{
   if(!memcmp(&fourcc,INFO_ARCHIVAL_LOCATION,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.ArchLocn;
    }
    else if(!memcmp(&fourcc,INFO_ARTIST,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.Artist;
    }
    else if(!memcmp(&fourcc,INFO_COMMISIONED,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.Commissioned;
    }
    else if(!memcmp(&fourcc,INFO_COMMENTS,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.Comments;
    }
    else if(!memcmp(&fourcc,INFO_COPYRIGHT,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.Copyright;
    }
    else if(!memcmp(&fourcc,INFO_CREATION_DATE,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.CreateDate;
    }
    else if(!memcmp(&fourcc,INFO_GENRE,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.Genre;
    }
    else if(!memcmp(&fourcc,INFO_KEYWORD,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.Keyword;
    }
    else if(!memcmp(&fourcc,INFO_NAME,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.Name;
    }
    else if(!memcmp(&fourcc,INFO_PRODUCT,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.Product;
    }
    else if(!memcmp(&fourcc,INFO_SUBJECT,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.Subject;
    }
    else if(!memcmp(&fourcc,INFO_SOURCE,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.Software;
    }
    else if(!memcmp(&fourcc,INFO_SOFTWARE,sizeof(fourCC_t))){
      return &m_AviClipMetaInfo.Source;
    }
    return NULL;
 }
/* =============================================================================
FUNCTION:
  aviParser::StartParsing

DESCRIPTION:
 Reads and parse clip meta data such as AVIH/STRL/STRF/STRN/INDX1 etc.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::StartParsing(void)
{
  unsigned char* byteData = m_ReadBuffer;
  bool bFoundStartRiff = false;
  aviErrorType retError = AVI_CORRUPTED_FILE;
  fourCC_t nCurrFourCC = 0;
  int nFoundRiff = 0;
  if(!m_pUserData)
  {
     QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "StartParsing AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  avi_uint64 nOffset = 0;
  avi_uint64 inMoviOffset = 0;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "StartParsing");
  #endif

  if(!bFoundStartRiff)
  {
    if(AVICallbakGetData(nOffset,12,m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      nOffset+=12;
      if(!memcmp(byteData,AVI_START_BYTES,sizeof(fourCC_t)))
      {
        if(!memcmp((byteData+8),AVI_SIGNATURE_BYTES,sizeof(fourCC_t)))
        {
          bFoundStartRiff = true;
          #ifdef AVI_PARSER_DEBUG
            QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "StartParsing located RIFF AVI");
          #endif
        }
      }
    }
    if(!bFoundStartRiff)
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "StartParsing AVI_CORRUPTED_FILE");
      return AVI_CORRUPTED_FILE;
    }
  }
  if(bFoundStartRiff)
  {
    m_CurrentParserState = AVI_PARSER_INIT;
    retError = AVI_SUCCESS;
    while( (nOffset + sizeof(fourCC_t) < m_nFileSize) && (retError == AVI_SUCCESS) )
    {
      if(!AVICallbakGetData(nOffset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
      {
        m_CurrentParserState = AVI_PARSER_READ_FAILED;
        retError = AVI_READ_FAILURE;
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "StartParsing AVI_PARSER_READ_FAILED");
        break;
      }
      nOffset+=sizeof(fourCC_t);

      if(!memcmp(byteData,AVI_LIST_FOURCC,sizeof(fourCC_t)))
      {
        //Get the list size
        if(!AVICallbakGetData(nOffset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
        {
          m_CurrentParserState = AVI_PARSER_READ_FAILED;
          retError = AVI_READ_FAILURE;
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                       "StartParsing AVI_READ_FAILURE");
          return AVI_READ_FAILURE;
        }
        nOffset+=sizeof(avi_uint32);
        avi_uint32 nListSize = 0;
        memcpy(&nListSize,byteData,sizeof(avi_uint32));

        //get the next fourCC code
        if(!AVICallbakGetData(nOffset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
        {
          m_CurrentParserState = AVI_PARSER_READ_FAILED;
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                       "StartParsing AVI_READ_FAILURE");
          retError = AVI_READ_FAILURE;
          return AVI_READ_FAILURE;
        }
        nOffset+=sizeof(fourCC_t);
        memcpy(&nCurrFourCC,byteData,sizeof(fourCC_t));

        /*
        * PARSING is STRICT,meaning when an error is reported
        * while parsing any of the following headers,playback will error out.
        * Not updating retError with return value will make it non STRICT,if needed.
        * Need to find out which error can be ignored while parsing following AVI headers.
        */

        if(!memcmp(byteData,AVI_HDRL_FOURCC,sizeof(fourCC_t)))
        {
          retError = parseHDRL(&nOffset);
        }
        else if(!memcmp(byteData,AVI_STRL_FOURCC,sizeof(fourCC_t)))
        {
          retError = parseSTRL(&nOffset,nListSize);
        }
        else if(!memcmp(byteData,AVI_ODML_FOURCC,sizeof(fourCC_t)))
        {
          retError = parseODML(nOffset);
          nOffset+=nListSize-sizeof(fourCC_t);
        }
        else if(!memcmp(byteData,AVI_MOVI_FOURCC,sizeof(fourCC_t)))
        {
          inMoviOffset = nOffset;
          m_nStartOfMovi = inMoviOffset;
          retError = parseMOVI(nOffset);
          nOffset = nOffset + (nListSize - (nOffset - inMoviOffset)) - sizeof(fourCC_t);
        }
        else if(!memcmp(byteData,AVI_INFO_FOURCC,sizeof(fourCC_t)))
        {
          retError = parseINFO(nOffset,(nListSize-sizeof(fourCC_t)));
          nOffset+=nListSize-sizeof(fourCC_t);
        }
        else
        {
          //Any un-known LIST chunk should be skipped.
          fourCC_t unknownFourCC;
          memcpy(&unknownFourCC,byteData,sizeof(fourCC_t));
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS,
                        QTVDIAG_PRIO_LOW,
                        "StartParsing encountered unknown LIST FOURCC %x",
                        unknownFourCC);
          //Skip size to go to next FOURCC.
          //nOffset+=nListSize;
          nOffset+=nListSize-sizeof(fourCC_t);//ajit added on 10/03/07.
        }
      }//if(!memcmp(byteData,AVI_LIST_FOURCC,sizeof(fourCC_t)))
      else if(!memcmp(byteData,AVI_JUNK_FOURCC,sizeof(fourCC_t)))
      {
        retError = parseJUNK(&nOffset);
        continue;
      }
      else if(!memcmp(byteData,AVI_IDX1_FOURCC,sizeof(fourCC_t)))
      {
        //IDX1 is the last RIFF as per AVI std.
        if( (retError = parseIDX1(&nOffset))!= AVI_SUCCESS )
        {
          /*
          * IDX1 is used for repositioning and calculating audio/video
          * timestamp once seek is done.
          * So, abort the playback if IDX1 parsing fails.
          */
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                       "StartParsing parseIDX1 failed..retError %d",retError);
          return retError;
        }

        /*
        * As per AVI specification, IDX1 is the last chunk in the file.
        * So, we can safely break from here. However, there could be few clips
        * that do not follow the specification,so continue parsing without breaking.
        */
      }
      else if(!memcmp(byteData,AVI_INDX_FOURCC,sizeof(fourCC_t)))
      {
        //Clip has 'indx' chunk.
        if( (retError = parseINDX(nOffset,*((fourCC_t*)(byteData)) ) )!= AVI_SUCCESS )
        {
          /*
          * INDX is used for repositioning and calculating audio/video
          * timestamp once seek is done and during playback.
          * So, abort the playback if IDX1 parsing fails.
          */
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                       "StartParsing parseINDX failed..retError %d",retError);
          return retError;
        }
        /*
        * Done parsing INDX;Read in size now and go over it.
        */
        if(!AVICallbakGetData(nOffset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
        {
          m_CurrentParserState = AVI_PARSER_READ_FAILED;
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                       "StartParsing AVI_READ_FAILURE");
          retError = AVI_READ_FAILURE;
          return AVI_READ_FAILURE;
        }
        nOffset+=sizeof(avi_uint32);
        int size = 0;
        memcpy(&size,byteData,sizeof(avi_uint32));
        nOffset+=size;
        #ifdef AVI_PARSER_DEBUG
          QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                        "StartParsing done parsing INDX at offset %d size %d",
                        nOffset - sizeof(avi_uint32),size);
        #endif
      }
      else if(!memcmp(byteData,AVI_START_BYTES,sizeof(fourCC_t)))
      {
        m_bisAVIXpresent = true;
        if(!m_nNumOfRiff)
        {
          retError = GetNumOfRiff(nOffset, &m_nNumOfRiff);
          if( (retError == AVI_SUCCESS) && (m_nNumOfRiff) )
          {
            m_pMultipleRiff = (avi_riff_info*)QTV_Malloc((sizeof(avi_riff_info))*(m_nNumOfRiff));
          }
          else
          {
            QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                             "StartParsing GetNumOfRiff ERROR");
          }
        }
        if((m_pMultipleRiff) && (nFoundRiff<= m_nNumOfRiff))
        {
          if(!AVICallbakGetData(nOffset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
          {
            m_CurrentParserState = AVI_PARSER_READ_FAILED;
            QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                         "StartParsing AVI_READ_FAILURE");
            retError = AVI_READ_FAILURE;
            return AVI_READ_FAILURE;
          }
          nOffset+=sizeof(avi_uint32);
          m_pMultipleRiff[nFoundRiff].startOffset = nOffset;
          memcpy(&m_pMultipleRiff[nFoundRiff].size,byteData,sizeof(avi_uint32));
          //Allign to 16-bit boundary
          m_pMultipleRiff[nFoundRiff].size = m_pMultipleRiff[nFoundRiff].size + ((m_pMultipleRiff[nFoundRiff].size) %2);
          nOffset+=m_pMultipleRiff[nFoundRiff].size;
          nFoundRiff++;
        }
      }
      else
      {
        //Any unknown FOURCC should be skipped...
        fourCC_t unknownFourCC;
        memcpy(&unknownFourCC,byteData,sizeof(fourCC_t));

        if( (nOffset+sizeof(avi_uint32)) >= m_nFileSize)
        {
          //Some times, a file might be missing chunk size after a fourCC.
          //If this is the last chunk, break out as no need flag an error while
          //parsing last unknown chunk.
          #ifdef AVI_PARSER_DEBUG
            QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                          "StartParsing Breaking from within the loop ..nOffset %d m_nFileSize %d",nOffset,m_nFileSize);
          #endif
          break;
        }

        //read in size to skip to next FOURCC
        if(!AVICallbakGetData(nOffset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
        {
          m_CurrentParserState = AVI_PARSER_READ_FAILED;
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                       "StartParsing AVI_READ_FAILURE");
          retError = AVI_READ_FAILURE;
          return AVI_READ_FAILURE;
        }
        nOffset+=sizeof(avi_uint32);
        avi_uint32 size = 0;
        memcpy(&size,byteData,sizeof(avi_uint32));
        size = size + (size %2);
        nOffset+=size;
        QTV_MSG_PRIO3(QTVDIAG_FILE_OPS,
                      QTVDIAG_PRIO_LOW,
                      "StartParsing encountered unknown FOURCC %x nsize %d offset %d",
                      unknownFourCC,size,(nOffset-8));
      }
    }//while( (nOffset < m_nFileSize) && (retError == AVI_SUCCESS) )

    if( (retError == AVI_SUCCESS) && (m_nCurrOffset > 0) )
    {
      avi_uint64 tempOffset = m_nCurrOffset;
      if(m_nIdx1Offset)
        tempOffset = skipToNextValidMediaChunk(m_nIdx1Offset);
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                      "StartParsing current m_nCurrOffset is %d",m_nCurrOffset);
      if(tempOffset != m_nCurrOffset)
      {
        m_nCurrOffset = tempOffset;
        m_nAdjustedMoviOffset = m_nCurrOffset;
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                      "StartParsing updated m_nCurrOffset based on SkipToNextValidMediaChunk to %d",m_nCurrOffset);
      }
      else
      {
        m_nAdjustedMoviOffset = 0;
      }
      m_CurrentParserState = AVI_PARSER_READY;
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                      "StartParsing m_nAdjustedMoviOffset %d m_nAdjustedIdx1Offset %d",m_nAdjustedMoviOffset, m_nAdjustedIdx1Offset);
      QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                      "StartParsing m_nIdx1Offset %d m_nMoviOffset %d m_nCurrOffset %d",m_nIdx1Offset, m_nMoviOffset,m_nCurrOffset);
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "StartParsing parsing done successfully!!");
    }
    else
    {
      QTV_MSG_PRIO3(QTVDIAG_FILE_OPS,
                    QTVDIAG_PRIO_LOW,
                    "StartParsing parsing failed.. nOffset %d m_nFileSize %d retError %d",
                    nOffset,m_nFileSize,retError);
    }
  }//if(bFoundStartRiff)
  return retError;
}

/* =============================================================================
FUNCTION:
  aviParser::GetNumOfRiff

DESCRIPTION:
 parseHDRL

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::GetNumOfRiff(avi_uint64 offset, int* m_nNumOfRiff)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint64 riffOffset = offset - sizeof(fourCC_t);
  avi_uint32 riffSize = 0;
  aviErrorType retError = AVI_CORRUPTED_FILE;

  while(riffOffset + 2*sizeof(avi_uint32) < m_nFileSize)
  {
    if(!AVICallbakGetData(riffOffset,2*sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      m_CurrentParserState = AVI_PARSER_READ_FAILED;
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                     "GetNumOfRiff AVI_READ_FAILURE");
      retError = AVI_READ_FAILURE;
      break;
    }
    riffOffset += 2*sizeof(avi_uint32);
    if(!memcmp(byteData,AVI_START_BYTES,sizeof(fourCC_t)))
    {
      memcpy(&riffSize,byteData+4,sizeof(avi_uint32));
      (*m_nNumOfRiff)++;
      riffSize = riffSize + (riffSize %2); //Allign to 16-bit
      riffOffset += riffSize;
      retError = AVI_SUCCESS;
    }
    else
    {
		//Any unknown FOURCC should be skipped
		fourCC_t unknownFourCC;
		memcpy(&unknownFourCC,byteData,sizeof(fourCC_t));
		avi_uint32 size = 0;
		memcpy(&size,byteData + sizeof(fourCC_t),sizeof(avi_uint32));
		size = size + (size %2);
		QTV_MSG_PRIO3(QTVDIAG_FILE_OPS,
			  QTVDIAG_PRIO_LOW,
			  "GetNumOfRiff encountered unknown FOURCC %x nsize %d offset %d, Skipping it!",
			  unknownFourCC,size,(riffOffset-8));
		riffOffset += size;
    }
  }
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseHDRL

DESCRIPTION:
 parseHDRL

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/

aviErrorType aviParser::parseHDRL(avi_uint64* offset)
{
  unsigned char* byteData = m_ReadBuffer;
  aviErrorType retError = AVI_CORRUPTED_FILE;

  fourCC_t nCurrFourCC = 0;

  retError = parseAVIH(offset);

  while( (*offset < m_nFileSize) && (retError == AVI_SUCCESS) )
  {
    if(!AVICallbakGetData(*offset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      m_CurrentParserState = AVI_PARSER_READ_FAILED;
      retError = AVI_READ_FAILURE;
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "StartParsing AVI_PARSER_READ_FAILED");
    }
    *offset+=sizeof(fourCC_t);

    if(!memcmp(byteData,AVI_LIST_FOURCC,sizeof(fourCC_t)))
    {
      //Get the list size
      if(!AVICallbakGetData(*offset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
      {
        m_CurrentParserState = AVI_PARSER_READ_FAILED;
        retError = AVI_READ_FAILURE;
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "StartParsing AVI_READ_FAILURE");
        return AVI_READ_FAILURE;
      }
      *offset+=sizeof(avi_uint32);

      avi_uint32 nListSize = 0;
      memcpy(&nListSize,byteData,sizeof(avi_uint32));

      //get the next fourCC code
      if(!AVICallbakGetData(*offset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
      {
        m_CurrentParserState = AVI_PARSER_READ_FAILED;
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "StartParsing AVI_READ_FAILURE");
        retError = AVI_READ_FAILURE;
        return AVI_READ_FAILURE;
      }
      *offset+=sizeof(fourCC_t);
      memcpy(&nCurrFourCC,byteData,sizeof(fourCC_t));

      if(!memcmp(byteData,AVI_STRL_FOURCC,sizeof(fourCC_t)))
      {
        retError = parseSTRL(offset,nListSize);
      }
      else
      {
        *offset-=12;
        break;
      }
    }
    else
    {
      *offset-=sizeof(fourCC_t);
      break;
    }
  }
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::parseMOVI

DESCRIPTION:
 parseMOVI

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/

aviErrorType aviParser::parseMOVI(avi_uint64 offset)
{
  aviErrorType retError = AVI_CORRUPTED_FILE;
  unsigned char* byteData = m_ReadBuffer;

  /*
  * Look ahead to see if there is any additional index information available.
  */
  if(!AVICallbakGetData(offset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    m_CurrentParserState = AVI_PARSER_READ_FAILED;
    retError = AVI_READ_FAILURE;
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "StartParsing AVI_PARSER_READ_FAILED");
    //break;
  }

  if(!memcmp(byteData,AVI_IX_FOURCC,sizeof(avi_uint16)))
  {
    //Clip has 'ix##' chunk(s).
    //Keep parsing untill all ix## chunks are parsed.

    while( memcmp(byteData,AVI_IX_FOURCC,sizeof(avi_uint16)) == 0)
    {
      //Need to read fourCC again as it contains streamId as well.
      //This will be read again and parsed in parseIX.

      int ixsize = 0;
      if( (retError = parseIX(offset,&ixsize))!= AVI_SUCCESS )
      {
        /*
        * IX is used for repositioning and calculating audio/video
        * timestamp once seek is done and during playback.
        * So, abort the playback if IX parsing fails.
        */
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "StartParsing parseIX failed..retError %d",retError);
        return retError;
      }
      offset += ixsize;
      if(!AVICallbakGetData(offset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
      {
        m_CurrentParserState = AVI_PARSER_READ_FAILED;
        retError = AVI_READ_FAILURE;
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "StartParsing AVI_PARSER_READ_FAILED");
        break;
      }
    }//while( memcmp(byteData,AVI_IX_FOURCC,sizeof(avi_uint16)) != 0)
  }
  else //check if we have any junk before audio and video frames start and skip them
  {
    bool bOK = false;
    while(!bOK)
    {
      avi_uint16 tkID, cType = 0;
      bool bTkID = false;
      CHUNK_t Type;

      memcpy(&tkID,byteData,sizeof(avi_uint16));
      tkID = ascii_2_short_int(&tkID, &bTkID);

      if(GetTrackChunkType(tkID,&Type)!=AVI_SUCCESS)
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "parseMOVI::unknown chunk type");
      }

      memcpy(&cType,byteData+2,sizeof(avi_uint16));
      if( ( (!memcmp(&cType,"dc",sizeof(avi_uint16))) || (!memcmp(&cType,"db",sizeof(avi_uint16))) ||
            (!memcmp(&cType,"wb",sizeof(avi_uint16))) || (!memcmp(&cType,"dd",sizeof(avi_uint16)))
           ) && (((Type == AVI_CHUNK_VIDEO)||(Type ==AVI_CHUNK_AUDIO)) && (bTkID == true))
         )
        {
          bOK = true;
        }
        else if( (!memcmp(byteData,AVI_JUNK_FOURCC,sizeof(avi_uint16))) ||
                 (!memcmp(byteData,AVI_RES_FOURCC,3*sizeof(char))) )
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "parseMOVI::JUNK atom encountered, skipping it");
          offset += sizeof(fourCC_t);
          retError = parseJUNK(&offset);
          if(!AVICallbakGetData(offset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
          {
            m_CurrentParserState = AVI_PARSER_READ_FAILED;
            retError = AVI_READ_FAILURE;
            QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                          "StartParsing AVI_READ_FAILURE");
            return AVI_READ_FAILURE;
          }
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "parseMOVI::Non A/V data encountered, skipping 4 bytes");
          offset += sizeof(fourCC_t);
          if(!AVICallbakGetData(offset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
          {
            m_CurrentParserState = AVI_PARSER_READ_FAILED;
            retError = AVI_READ_FAILURE;
            QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                          "StartParsing AVI_READ_FAILURE");
            return AVI_READ_FAILURE;
          }
        }
     }
  }
  m_nCurrOffset = offset;
  m_nMoviOffset = m_nCurrOffset;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                  "StartParsing encountered MOVI at offset %d",
                    m_nMoviOffset);
  #endif
  /*
  * For MOVI, size of LIST includes 'MOVI' and all media payloads.
  * Since we have already read in MOVI, while adjusting the offset for next FOURCC,
  * we need to subtract 'MOVI' bytes, which is, sizeof(fourCC_t) from length.
  * This length is inclusive of any IX if available.
  */
  //offset+=nListSize-sizeof(fourCC_t);
  retError = AVI_SUCCESS;
  return retError;
 }

/* =============================================================================
FUNCTION:
  aviParser::parseJUNK

DESCRIPTION:
 parseJUNK

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/

aviErrorType aviParser::parseJUNK(avi_uint64* offset)
{
  unsigned char* byteData = m_ReadBuffer;
  aviErrorType retError = AVI_CORRUPTED_FILE;
  avi_uint32 size;
  /*
  * Read in JUNK size to skip it as there is nothing to parse in JUNK.
  */
  if(!AVICallbakGetData(*offset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    m_CurrentParserState = AVI_PARSER_READ_FAILED;
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                 "parseJUNK AVI_READ_FAILURE");
    retError = AVI_READ_FAILURE;
    return AVI_READ_FAILURE;
  }
  *offset+=sizeof(avi_uint32);
  memcpy(&size,byteData,sizeof(avi_uint32));

  size = size + (size %2); // Allign to 16 bit boundary

  *offset+=size;
  retError = AVI_SUCCESS;
  return retError;
}
/* =============================================================================
FUNCTION:
  aviParser::GetTrackChunkType

DESCRIPTION:
 Returns track type(audio/video) for given track trackId.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::GetTrackChunkType(avi_uint8 trackId,CHUNK_t* type)
{
  if(trackId < m_hAviSummary.n_streams)
  {
    *type = m_hAviSummary.stream_index[trackId].type;
    return AVI_SUCCESS;
  }
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
               "GetTrackChunkType AVI_INVALID_USER_DATA");
	return AVI_INVALID_USER_DATA;
}
/* =============================================================================
FUNCTION:
  aviParser::GetClipDurationInMsec

DESCRIPTION:
 Returns clip duration from the current parsed meta-data.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
avi_uint64 aviParser::GetClipDurationInMsec()
{
  avi_uint64 nDuration = 0;
  avi_uint64 maxDuration = 0;

  for (int i =0;i<m_hAviSummary.n_streams;i++)
  {
    nDuration = GetTrackDuration(m_hAviSummary.stream_index[i].index);
    maxDuration = MAX(maxDuration, nDuration);
  }
  return maxDuration;
}
/* =============================================================================
FUNCTION:
  aviParser::GetTrackDuration

DESCRIPTION:
 Returns duration for given track.
 APP can take MAX across all available tracks to get the clip duration.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
avi_uint64 aviParser::GetTrackDuration(avi_uint8 trackId)
{
  avi_video_info vinfo;
  avi_audiotrack_summary_info ainfo;
  avi_audio_info audtrackInfo;
  CHUNK_t Type;
  avi_uint64 nDuration = 0;
  if(GetTrackChunkType(trackId,&Type)==AVI_SUCCESS)
  {
    switch(Type)
    {
      case AVI_CHUNK_VIDEO:
        if(GetVideoInfo(trackId,&vinfo)==AVI_SUCCESS)
        {
          if(vinfo.strhVideo.dwRate > 0)
          {
            float frate = (float)vinfo.strhVideo.dwScale/(float)vinfo.strhVideo.dwRate;
            float length = (float)vinfo.strhVideo.dwLength;
            nDuration = (avi_uint64)(frate*length*1000);
            #ifdef AVI_PARSER_DEBUG
              QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "GetTrackDuration video duration %d",nDuration);
            #endif
          }
        }
        break;
      case AVI_CHUNK_AUDIO:
        if( (GetAudioTrackSummaryInfo(trackId,&ainfo)==AVI_SUCCESS) &&
            (GetAudioInfo(trackId,&audtrackInfo)==AVI_SUCCESS) )
        {
          if(!ainfo.isVbr && (ainfo.audioBytesPerSec))
          {
#ifdef AVI_PARSER_FAST_START_UP
            nDuration = (avi_uint64)((float) (audtrackInfo.strhAudio.dwLength*8)/
                                     (float)ainfo.audioBytesPerSec * 1000.0f);
            #ifdef AVI_PARSER_DEBUG
              QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "GetTrackDuration CBR Audio:nDuration %d",nDuration);
            #endif
            float framerate =  ( (float)audtrackInfo.strhAudio.dwScale /
                                             (float)audtrackInfo.strhAudio.dwRate );
            nDuration = (avi_uint64)((framerate * 1000)* audtrackInfo.strhAudio.dwLength);
#else
			//Calculate the duration using the total nummber of audio bytes in the track.
			//This will result in the duration being returned as 0 till the idx1 cache is completed.
              if(m_hAviSummary.pIdx1Table)
              {
			nDuration = (avi_uint32)((float)m_hAviSummary.pIdx1Table->nTotalAudioBytes /
                (float)ainfo.audioBytesPerSec * 1000.0f);
              }
#endif

            #ifdef AVI_PARSER_DEBUG
              QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "GetTrackDuration CBR Audio:nDuration %d",nDuration);
            #endif
          }
          else if(ainfo.isVbr && ainfo.audioFrequency)
          {
              // Duration = nBlockAlign * 1000 / audioFrequency
              // MAX AUDIO TS = Duration * NumberofAudioChunks;
              nDuration = (avi_uint64)( (float)(ainfo.nBlockAlign * 1000 /ainfo.audioFrequency) *
                                        ((float)audtrackInfo.strhAudio.dwLength + 0.5) );

              nDuration = (avi_uint64)(( ( (float)audtrackInfo.strhAudio.dwScale/
                                        (float)audtrackInfo.strhAudio.dwRate) * 1000.f) * (float)audtrackInfo.strhAudio.dwLength);

              #ifdef AVI_PARSER_DEBUG
                QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                              "GetTrackDuration VBR Audio:nDuration %d",nDuration);
              #endif
          }
        }
        break;
        default:
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                       "GetTrackDuration returing 0 for non audio/video track");
        break;
    }
  }
  return nDuration;
}
/* =============================================================================
FUNCTION:
 aviParser::GetAVIHeader

DESCRIPTION:
Returns AVIH from the clip.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::GetAVIHeader(avi_mainheader_avih* pAviHdrPtr)
{
  if(!pAviHdrPtr)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
               "GetAVIHeader AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  if(!m_hAviSummary.avih)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
               "GetAVIHeader,NULL AVI Header,AVI_PARSE_ERROR");
    return AVI_PARSE_ERROR;
  }
  memset(pAviHdrPtr,0,sizeof(avi_mainheader_avih));
  memcpy(pAviHdrPtr,m_hAviSummary.avih,sizeof(avi_mainheader_avih));
  return AVI_SUCCESS;
}
/* =============================================================================
FUNCTION:
 aviParser::GetAudioInfo

DESCRIPTION:
Returns audio track info for given trackId.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::GetAudioInfo(avi_uint8 trackId,avi_audio_info* pAudioInfo)
{
  aviErrorType retError = AVI_INVALID_USER_DATA;
  if(!pAudioInfo)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
               "GetAudioInfo AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  memset(pAudioInfo,0,sizeof(avi_audio_info));
  if(m_hAviSummary.n_streams <= trackId)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                 "GetAudioInfo AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  for(int i = 0; i< AVI_MAX_TRACKS; i++)
  {
    if( (m_hAviSummary.stream_index[i].index == trackId)&&
        (m_hAviSummary.stream_index[i].type == AVI_CHUNK_AUDIO))
    {
      memcpy(pAudioInfo,&(m_hAviSummary.audio_info[m_hAviSummary.stream_index[i].audioIndex]),sizeof(avi_audio_info));
      retError = AVI_SUCCESS;
    }
  }
  return retError;
}
/* =============================================================================
FUNCTION:
 aviParser::GetVideoInfo

DESCRIPTION:
Returns video track info for given trackId.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::GetVideoInfo(avi_uint8 trackId,avi_video_info* pVideoInfo)
{
  aviErrorType retError = AVI_INVALID_USER_DATA;
  if(!pVideoInfo)
  {
     QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "GetVideoInfo AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  memset(pVideoInfo,0,sizeof(avi_video_info));
  if(m_hAviSummary.n_streams <= trackId)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                 "GetVideoInfo AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  for(int i = 0; i< AVI_MAX_TRACKS; i++)
  {
    if( (m_hAviSummary.stream_index[i].index == trackId)&&
        (m_hAviSummary.stream_index[i].type == AVI_CHUNK_VIDEO))
    {
      memcpy(pVideoInfo,&(m_hAviSummary.video_info[m_hAviSummary.stream_index[i].videoIndex]),sizeof(avi_video_info));
      retError = AVI_SUCCESS;
    }
  }
  return retError;
}
/* =============================================================================
FUNCTION:
 aviParser::GetDRMInfo

DESCRIPTION:
Returns DRM header for and the size of the header.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
avi_uint8* aviParser::GetDRMInfo(int* size)
{
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "GetDRMInfo");
  if( (!size)||(!m_bDRMProtection)||(!m_AviSTRDHeader.drm_info) )
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "GetDRMInfo Error,returning NULL");
    return NULL;
  }
  *size = m_AviSTRDHeader.drm_size;
  return m_AviSTRDHeader.drm_info;
}
/* =============================================================================
FUNCTION:
 aviParser::GetAVIVolHeader

DESCRIPTION:
Returns VOL header for given video trackId.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
avi_uint8* aviParser::GetAVIVolHeader(avi_uint8   trackId)
{
  unsigned char* byteData = m_ReadBuffer;
  bool bOk = false;
  avi_int8 volhdrbuff[MAX_VOL_HDR_SIZE];
  int writeOffset = 0;
  avi_uint8* retBuffer = NULL;
  avi_video_info vInfo;
  int bdivxfourcc = 1;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "GetVolHeader");
  #endif
  if( m_hAviSummary.n_streams <= trackId )
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "GetVolHeader AVI_INVALID_USER_DATA");
    return retBuffer;
  }
  if(m_AviVOLHeader[trackId]->pMemory)
  {
    return m_AviVOLHeader[trackId]->pMemory;
  }

  for(int i = 0; i< AVI_MAX_TRACKS; i++)
  {
    if( (m_hAviSummary.stream_index[i].index == trackId)   &&
        (m_hAviSummary.stream_index[i].type == AVI_CHUNK_VIDEO))
    {
      bOk = true;
      break;
    }
  }
  if(!bOk)
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                   "GetVolHeader was called for non Video track %d",trackId);
    return retBuffer;
  }
  bOk = false;
  avi_uint64 offset = m_nMoviOffset;
  avi_uint32 dwChunkId,dwSize;
  avi_uint16 tkId,cType;

  //Now locate the video frame for given track-id.
  while(1)
  {
    if(!AVICallbakGetData(offset,(2*sizeof(avi_uint32)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "GetAVIVolHeader AVI_READ_FAILURE");
      return retBuffer;
    }

    offset += (2*sizeof(avi_uint32));
    memcpy(&dwChunkId,byteData,sizeof(avi_uint32));
    memcpy(&dwSize,byteData+sizeof(avi_uint32),sizeof(avi_uint32));

    memcpy(&tkId,(avi_uint8*)&dwChunkId,sizeof(avi_uint16));
    tkId = ascii_2_short_int(&tkId);
    memcpy(&cType,((unsigned char*)&dwChunkId)+2,sizeof(avi_uint16));

    if ( ((!memcmp(&cType,"dc",sizeof(avi_uint16))) || (!memcmp(&cType,"db",sizeof(avi_uint16))))&&
         (trackId==tkId) && (dwSize) )
    {
      bOk = true;
      #ifdef AVI_PARSER_DEBUG
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                      "GetVolHeader located corrected video track id %d",tkId);
      #endif
      break;
    }
    else if ( (!memcmp(&cType,"wb",sizeof(avi_uint16))) ||
              (!memcmp(&cType,"dd",sizeof(avi_uint16))) ||
              (!memcmp(&cType,"sb",sizeof(avi_uint16))) ||
              (!memcmp(byteData,AVI_JUNK_FOURCC,sizeof(fourCC_t))) ||
              (!memcmp(byteData,AVI_RES_FOURCC,3*sizeof(char)))
            )
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                      "GetVolHeader located an audio frame");
      offset += dwSize;
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                      "GetVolHeader located bad data");
      offset -= (2*sizeof(avi_uint32));   //Undoing what we read earlier
      offset += sizeof(fourCC_t);         //Going to next 4 bytes to read fourcc
    }
    if(dwSize %2)
    {
      //Must align to 16-bit boundary
      offset++;
    }
  }

  if(!bOk)
  {
     //Could not locate video frame for given track-id.
     //Something is wrong or there is blank video track(??)
     QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                   "GetVolHeader Failed to locate VOL header for track %d",trackId);
     return retBuffer;
  }

  if(GetVideoInfo(trackId,&vInfo)==AVI_SUCCESS)
  {
    if( (!memcmp(&vInfo.strhVideo.fccHandler,"XVID",sizeof(fourCC_t))) ||
        (!memcmp(&vInfo.strhVideo.fccHandler,"xvid",sizeof(fourCC_t))) ||
        (!memcmp(&vInfo.strhVideo.fccHandler,"FMP4",sizeof(fourCC_t))) ||
        (!memcmp(&vInfo.strhVideo.fccHandler,"fmp4",sizeof(fourCC_t))) ||
        (!memcmp(&vInfo.strhVideo.fccHandler,"yv12",sizeof(fourCC_t))) ||
        (!memcmp(&vInfo.strhVideo.fccHandler,"YV12",sizeof(fourCC_t)))
      )
    {
      bdivxfourcc = 0;
    }
    else
    {
      bdivxfourcc = 1;
    }
  }

  if(bdivxfourcc == 1)
  {
    if(!AVICallbakGetData(offset,(2*sizeof(avi_uint32)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "GetAVIVolHeader AVI_READ_FAILURE");
        return retBuffer;
    }
    offset += (2*sizeof(avi_uint32));

    int error = memcmp(byteData,&IVOP_START_CODE,sizeof(avi_uint32) );
    error += memcmp(byteData+sizeof(avi_uint32),&IVOP_VOL_START_CODE,sizeof(avi_uint32));

    if(error )
    {
     //Not sure how to locate VOL header so signal the failure.
     QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                   "GetVolHeader track %d does not start with SEQUENCE_START_CODE || START_CODE",trackId);
    }
    else
    {
      /*
      * Dump VISUAL_OBJECT_SEQUENCE_START_CODE,
      * MPEG4_PROFILE_AND_LEVEL_INDICATION,
      * MPEG4_VISUAL_OBJECT_START_CODE,
      * MPEG4_VIDEO_OBJECT_IDEN_VIDEO_SIG_TYPE
      * IVOP_START_CODE
      * IVOP_VOL_START_CODE into output buffer.
      */
      memcpy(volhdrbuff+writeOffset,&MPEG4_VISUAL_OBJECT_SEQUENCE_START_CODE,sizeof(avi_uint32));
      writeOffset += sizeof(avi_uint32);

      memcpy(volhdrbuff+writeOffset,&MPEG4_PROFILE_AND_LEVEL_INDICATION,sizeof(avi_uint8));
      writeOffset += sizeof(avi_uint8);

      memcpy(volhdrbuff+writeOffset,&MPEG4_VISUAL_OBJECT_START_CODE,sizeof(avi_uint32));
      writeOffset += sizeof(avi_uint32);

      memcpy(volhdrbuff+writeOffset,&MPEG4_VIDEO_OBJECT_IDEN_VIDEO_SIG_TYPE,sizeof(avi_uint8));
      writeOffset += sizeof(avi_uint8);

      memcpy(volhdrbuff+writeOffset,&IVOP_START_CODE,sizeof(avi_uint32));
      writeOffset += sizeof(avi_uint32);
      memcpy(volhdrbuff+writeOffset,&IVOP_VOL_START_CODE,sizeof(avi_uint32));
      writeOffset += sizeof(avi_uint32);
    }
  }

  bool bError = false;
  avi_int8 val = 0;

  while( (!isVOLReadDone(trackId,offset,&val,&bError,NULL)) && (!bError) )
  {
    memcpy(volhdrbuff+writeOffset,&val,sizeof(avi_int8));
    writeOffset += sizeof(avi_uint8);
    offset++;
    if(writeOffset >= MAX_VOL_HDR_SIZE)
    {
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "GetVolHeader size > %ld",MAX_VOL_HDR_SIZE);
      return retBuffer;
    }
  }
  //memcpy(volhdrbuff+writeOffset,&MPEG4_VOP_HEADER_CODE,(4*sizeof(avi_uint8)));
  //writeOffset+=(4*sizeof(avi_uint8));

  m_VolSize = writeOffset;
  if(m_AviVOLHeader[trackId])
  {
    m_AviVOLHeader[trackId]->pMemory = (avi_uint8*)QTV_Malloc(m_VolSize);
    if(!m_AviVOLHeader[trackId]->pMemory)
    {
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "GetVolHeader failed to allocate memory for %ld bytes",m_VolSize);
      return retBuffer;
    }
    memcpy(m_AviVOLHeader[trackId]->pMemory,volhdrbuff,m_VolSize);
    m_AviVOLHeader[trackId]->nSize = m_VolSize;
    retBuffer = m_AviVOLHeader[trackId]->pMemory;
  }
  return retBuffer;
}
/* =============================================================================
FUNCTION:
 aviParser::isVOLReadDone

DESCRIPTION:
Reads a byte for VOL header untill VOP HEADER CODE is encountered.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
bool aviParser::isVOLReadDone(avi_uint8 trackId, int offset, avi_int8* val,bool* bError,avi_uint8* membuf)
{
  unsigned char* byteData = m_ReadBuffer;
  bool bRet = false;
  avi_video_info vInfo;

  if( (!val)||(!bError) )
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "isVOLReadDone invalid parameter!!");
    return true;
  }
  *bError = false;
  if(membuf == NULL)
  {
    if(!AVICallbakGetData(offset,(4 *sizeof(avi_uint8)),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "isVOLReadDone AVI_READ_FAILURE");
      *bError = true;
      return true;
    }
  }
  if(GetVideoInfo(trackId,&vInfo)==AVI_SUCCESS)
  {
    if(
        (!memcmp(&vInfo.strhVideo.fccHandler,"FMP4",sizeof(fourCC_t)))  ||
        (!memcmp(&vInfo.strhVideo.fccHandler,"fmp4",sizeof(fourCC_t)))
      )
      {
        MPEG4_VOP_HEADER_CODE = 0xB2010000;   //FMP4 clips have VolHeader ending with this code
      }
    if(!memcmp(byteData,&MPEG4_VOP_HEADER_CODE,(4 *sizeof(avi_uint8))))
    {
      bRet = true;
      #ifdef AVI_PARSER_DEBUG
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"isVOLReadDone located VOP_HEADER_CODE");
      #endif
      return bRet;
    }
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                   "GetVideoInfo failed to return video info for the given trackId %d",trackId);
  }

  memcpy(val,byteData,sizeof(avi_int8));
  return bRet;
}
/* =============================================================================
FUNCTION:
 aviParser::GetAVIVolHeaderSize

DESCRIPTION:
Returns VOL headersize for given trackID.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
avi_uint32 aviParser::GetAVIVolHeaderSize(avi_uint8 trackId)
{
#ifdef AVI_PARSER_DEBUG
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "GetAVIVolHeaderSize");
#endif
  if(m_AviVOLHeader[trackId])
  {
     return m_AviVOLHeader[trackId]->nSize;
  }
  return 0;
}
/* =============================================================================
FUNCTION:
 aviParser::GetCurrentSample

DESCRIPTION:
Returns current sample for the given trackId.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::GetCurrentSample(avi_uint8 trackId,avi_uint8* dataBuffer,
                                         avi_uint32 nMaxBufSize,
                                         avi_uint32 nBytesNeeded)
{
  //avi_uint32 nSize = 0;
  //avi_uint16 cType = AVI_CHUNK_UNKNOWN;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                        "GetCurrentSample trackId %d nMaxBufSize %d nBytesNeeded %d",
                        trackId,nMaxBufSize,nBytesNeeded);
  #endif
  if( (!dataBuffer) || (!nMaxBufSize) || (!m_hAviSummary.n_streams) || (!nBytesNeeded) )
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS,
                 QTVDIAG_PRIO_HIGH,
                 "GetCurrentSample AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  if(trackId < m_hAviSummary.n_streams)
  {
    if(m_CurrentParserState != AVI_PARSER_CHUNK_DATA_START)
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,
      "INVALID PARSER STATE.Please retrieve sampleInfo first before retrieving actual sample!!");
      return AVI_FAILURE;
    }
    if(nMaxBufSize < m_nCurrentChunkDataSize)
    {
      //Buffer size provided is < the chunk data size,
      //report failure and print an error message.
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,
                    "Buffer size provided %d  < the current chunk data size %d",nMaxBufSize,m_nCurrentChunkDataSize);
      return AVI_INVALID_USER_DATA;
    }
    if(!AVICallbakGetData(m_nCurrOffset,nBytesNeeded,dataBuffer,nMaxBufSize,m_pUserData))
    {
      m_CurrentParserState = AVI_PARSER_READ_FAILED;
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS,
                   QTVDIAG_PRIO_FATAL,
                   "GetCurrentSample AVI_READ_FAILURE");
      return AVI_READ_FAILURE;
    }
    m_nCurrOffset+=nBytesNeeded;
    if (nBytesNeeded % 2)
    {
		  // must align to 16-bit boundary
		  m_nCurrOffset++;
	  }
    m_CurrentParserState = AVI_PARSER_CHUNK_HEADER_START;
    return AVI_SUCCESS;
  }
   QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                "GetCurrentSample AVI_INVALID_USER_DATA");
  return AVI_INVALID_USER_DATA;
}
/* =============================================================================
FUNCTION:
 aviParser::GetNextSampleInfo

DESCRIPTION:
Returns next available sample info for given trackId.
APP needs to call GetNextSampleInfo before retrieving the media sample.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::GetNextSampleInfo(avi_uint8 trackId,
                                          avi_sample_info* sampleInfo,
                                          avi_uint32 nMaxBufSize,
                                          avi_uint16* trackIdFound)
{
  unsigned char* byteData = m_ReadBuffer;
  avi_uint32 nSize = 0;
  avi_uint16 cType = AVI_CHUNK_UNKNOWN;
  aviErrorType retError = AVI_FAILURE;
  CHUNK_t chkType;
  bool bSampleOffsetAdjusted = false;
  avi_uint32 nSizeToBeAdjusted = 0;
  bool bEndTrack = false;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                  "GetNextSampleInfo trackId %d",trackId);
  #endif
  if( (!sampleInfo) || (!trackIdFound) || (!m_hAviSummary.n_streams) )
  {
   QTV_MSG_PRIO(QTVDIAG_FILE_OPS,
                QTVDIAG_PRIO_FATAL,
                "GetNextSampleInfo AVI_INVALID_USER_DATA");
    return AVI_INVALID_USER_DATA;
  }
  memset(sampleInfo,0,sizeof(avi_sample_info));
  *trackIdFound = 0;

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                "GetNextSampleInfo m_nCurrOffset %d m_nFileSize %d m_nIdx1Offset %d",
                m_nCurrOffset,m_nFileSize,m_nIdx1Offset);
  #endif

  if(trackId < m_hAviSummary.n_streams)
  {
    if(m_CurrentParserState == AVI_PARSER_CHUNK_DATA_START)
    {
       QTV_MSG_PRIO2(QTVDIAG_FILE_OPS,
                     QTVDIAG_PRIO_LOW,
                     "Previous sample not retrieved before reading next sampleInfo!!current m_nCurrOffset %d ADJUSTING TO %d",
                     m_nCurrOffset,(m_nCurrOffset-8));
      m_nCurrOffset -= 8;
      nSizeToBeAdjusted = m_nCurrentChunkDataSize;
      m_nCurrentChunkDataSize = 0;
      m_CurrentParserState = AVI_PARSER_CHUNK_HEADER_START;
      bSampleOffsetAdjusted = true;
      //Since we went back by a sample,adjust IDX1 offset location
      if(m_nCurrentSampleInfoOffsetInIdx1 > (4 * sizeof(avi_uint32)))
      {
        m_nCurrentSampleInfoOffsetInIdx1 -= ( 4 * sizeof(avi_uint32) );
      }
      else
      {
        //If we reach here, something has gone bad, be on lookout!!!
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "GetNextSampleInfo detected invalid m_nCurrentSampleInfoOffsetInIdx1 %d ",
                      m_nCurrentSampleInfoOffsetInIdx1);
      }
    }
    if( (m_CurrentParserState != AVI_PARSER_CHUNK_HEADER_START) &&
        (m_CurrentParserState != AVI_PARSER_READY) )
    {
      /*
      * PARSER needs to be in AVI_PARSER_CHUNK_HEADER_START or AVI_PARSER_READY
      * when retrieving samples. If not, we should bail out immediately.
      */
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,"INVALID PARSER STATE %d ",m_CurrentParserState);
      return AVI_PARSE_ERROR;
    }
    while(1)
    {
      m_CurrentParserState = AVI_PARSER_CHUNK_HEADER_START;

      fourCC_t fourCCVal = 0;
      if(!AVICallbakGetData(m_nCurrOffset,sizeof(fourCC_t),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
      {
        m_CurrentParserState = AVI_PARSER_READ_FAILED;
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS,
                     QTVDIAG_PRIO_FATAL,
                     "GetNextSampleInfo AVI_READ_FAILURE");
        return AVI_READ_FAILURE;
      }
      memcpy(&fourCCVal,byteData,sizeof(fourCC_t));
      m_nCurrOffset+=sizeof(fourCC_t);

      memcpy(trackIdFound,(avi_uint8*)&fourCCVal,sizeof(avi_uint16));
      *trackIdFound = ascii_2_short_int(trackIdFound);
      memcpy(&cType,((avi_uint8*)(&fourCCVal))+2,sizeof(avi_uint16));

      if(!AVICallbakGetData(m_nCurrOffset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
      {
        m_CurrentParserState = AVI_PARSER_READ_FAILED;
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS,
                       QTVDIAG_PRIO_FATAL,
                       "GetNextSampleInfo AVI_READ_FAILURE");
        return AVI_READ_FAILURE;
      }
      memcpy(&nSize,byteData,sizeof(avi_uint32));

      /* We will end track based on trackDuration only for AUDIO since it is of higher priority
      But once we go over the IDX1Offset,we should end track for both AUDIO and VIDEO */
      if( ( (getCurrentPlaybackTime(trackId) >= GetTrackDuration(trackId) )&&
            (GetTrackDuration(trackId) > 0) &&
            (GetTrackChunkType(trackId,&chkType) == AVI_SUCCESS) &&
            (chkType == AVI_CHUNK_AUDIO) ) ||
            ( (m_nCurrOffset >= m_nIdx1Offset) && (!m_bisAVIXpresent) && (m_nIdx1Offset) ) )
      {
        bEndTrack = true;
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                  "GetNextSampleInfo setting bEndTrack TRUE for trackid %d",trackId);
        break;
      }

      if( (!memcmp(&fourCCVal,AVI_JUNK_FOURCC,sizeof(fourCC_t))) ||
          (!memcmp(&fourCCVal,AVI_RES_FOURCC,3*sizeof(char)))  ||
          (!memcmp(&fourCCVal,AVI_IX_FOURCC,2*sizeof(char))) ||
          ((!memcmp(&fourCCVal,AVI_IDX1_FOURCC,sizeof(fourCC_t))) && (m_bisAVIXpresent) ))
      {
        m_nCurrOffset+=sizeof(avi_uint32);
        sampleInfo->nSampleSize = nSize;
        m_nCurrOffset+=sampleInfo->nSampleSize;
        if (sampleInfo->nSampleSize % 2)
        {
		      // must align to 16-bit boundary
		      m_nCurrOffset++;
	      }
        m_nCurrentSampleInfoOffsetInIdx1 += ( 4 * sizeof(avi_uint32) );
      }
      else if (
         (memcmp(&cType,"dc",sizeof(avi_uint16))) &&
         (memcmp(&cType,"db",sizeof(avi_uint16))) &&
         (memcmp(&cType,"wb",sizeof(avi_uint16))) &&
         (memcmp(&cType,"dd",sizeof(avi_uint16))) &&
         (memcmp(&cType,"sb",sizeof(avi_uint16))) &&
         ((nSize) && (m_nCurrOffset+nSize <= m_nFileSize) &&
         (memcmp(&fourCCVal,"RIFF",sizeof(fourCC_t))) )
         )
      {
        m_nCurrOffset+=sizeof(avi_uint32);
        sampleInfo->nSampleSize = nSize;
        m_nCurrOffset+=sampleInfo->nSampleSize;
        if (sampleInfo->nSampleSize % 2)
        {
	  // must align to 16-bit boundary
          m_nCurrOffset++;
	}
        continue;
      }
      else if(!memcmp(&fourCCVal,"RIFF",sizeof(fourCC_t)) )
      {
        for(int i = 0;i<m_nNumOfRiff;i++)
        {
          //Check which RIFF atom is immediately next from the current offset
          if( (m_nCurrOffset + sizeof(avi_uint32) ) == m_pMultipleRiff[i].startOffset)
          {
            //Skip AVIX/LIST/MOVI/SIZE chunks till we start getting audio/video frames again
            m_nCurrOffset = m_pMultipleRiff[i].startOffset + (4*sizeof(avi_uint32));
          }
        }
      }
      else
      {

        if(!AVICallbakGetData(m_nCurrOffset,sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
        {
          m_CurrentParserState = AVI_PARSER_READ_FAILED;
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS,
                      QTVDIAG_PRIO_FATAL,
                      "GetNextSampleInfo AVI_READ_FAILURE");
          return AVI_READ_FAILURE;
        }
        memcpy(&nSize,byteData,sizeof(avi_uint32));
        m_nCurrOffset+=sizeof(avi_uint32);

        sampleInfo->nSampleSize = nSize;

        /*
        * Go to next entry in IDX1 as we have read in the sample header.
        * Since IDX1 has all the samples from MOVI, number of samples
        * skipped here should match the current retrieved audio/video sample.
        * Although we are interested only in audio/video,
        * we need to do it here otherwise, we will have to do file read to
        * bypass non audio/video samples. This makes audio playback choppy.
        */

        m_nCurrentSampleInfoOffsetInIdx1 += ( 4 * sizeof(avi_uint32) );

        /*
        * If we encounter any chunk which is not expected or not known,
        * we should just continue parsing by skipping such un-known/unexpected chunks.
        */
        bool bChunkOk = false;

        if( (!memcmp(&cType,"dc",sizeof(avi_uint16))) ||
            (!memcmp(&cType,"db",sizeof(avi_uint16))) ||
            (!memcmp(&cType,"wb",sizeof(avi_uint16))) ||
            (!memcmp(&cType,"dd",sizeof(avi_uint16))) )
        {
          bChunkOk = true;
        }
        else if ( (!memcmp(&cType,"sb",sizeof(avi_uint16))) )
        {
          bChunkOk = false;
        }

        if( (*trackIdFound != trackId) || (bChunkOk == false) || GetTotalNumberOfTracks() < (*trackIdFound))
        {
          /*
          * Any unknown chunk might yield invalid track-id as it may not have track-id in it.
          * Thus, if we have found some media chunk(audio/video/caption/drm) and if track id is >
          * total number of tracks, flag an error and abort.
          */
          if( GetTotalNumberOfTracks() < (*trackIdFound) && (bChunkOk) )
          {
            /*
            *Something is wrong, either seek resulted in wrong offset,
            *parser screwed up during playback or somebode else
            *might have corrupted the memory.
            */
            QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                          "getNextSampleInfo *trackIdFound %d > total number of tracks %d ",(*trackIdFound),GetTotalNumberOfTracks());
            m_nCurrOffset += sizeof(avi_uint32);
            continue;
          }
          if(memcmp(&cType,"dd",sizeof(avi_uint16))) //Making sure its not a DRM chunk
          {
            //Skip un-known chunk and continue parsing
            m_nCurrOffset+=sampleInfo->nSampleSize;
            if (sampleInfo->nSampleSize % 2)
            {
		          // must align to 16-bit boundary
		          m_nCurrOffset++;
	          }
            continue;
          }
        }

        if( (!memcmp(&cType,"dc",sizeof(avi_uint16))) ||
            (!memcmp(&cType,"db",sizeof(avi_uint16)))   )
        {
          sampleInfo->chunkType = AVI_CHUNK_VIDEO;
          if (nSize>nMaxBufSize)
          {
            m_nCurrOffset -= sizeof(fourCC_t);
            m_nCurrOffset -= sizeof(avi_uint32);
            m_nCurrentSampleInfoOffsetInIdx1 -= ( 4 * sizeof(avi_uint32) );
            return AVI_INSUFFICIENT_BUFFER;
          }
        }
        if(!memcmp(&cType,"wb",sizeof(avi_uint16)))
        {
          sampleInfo->chunkType = AVI_CHUNK_AUDIO;
          if (nSize>nMaxBufSize)
          {
            m_nCurrOffset -= sizeof(fourCC_t);
            m_nCurrOffset -= sizeof(avi_uint32);
            m_nCurrentSampleInfoOffsetInIdx1 -= ( 4 * sizeof(avi_uint32) );
            return AVI_INSUFFICIENT_BUFFER;
          }
        }
        if(!memcmp(&cType,"dd",sizeof(avi_uint16)))
        {
          sampleInfo->chunkType = AVI_CHUNK_DRM;
          if (nSize>nMaxBufSize)
          {
            m_nCurrOffset -= sizeof(fourCC_t);
            m_nCurrOffset -= sizeof(avi_uint32);
            m_nCurrentSampleInfoOffsetInIdx1 -= ( 4 * sizeof(avi_uint32) );
            return AVI_INSUFFICIENT_BUFFER;
          }
          retError = AVI_SUCCESS;
          m_nCurrentChunkDataSize = sampleInfo->nSampleSize;
          m_CurrentParserState = AVI_PARSER_CHUNK_DATA_START;
          return retError;
        }

        if(GetTrackChunkType(*trackIdFound,&chkType)==AVI_SUCCESS)
        {
          switch(chkType)
          {
            case AVI_CHUNK_AUDIO:
              avi_audiotrack_summary_info ainfo;
            avi_audio_info audinfo;
              if( bSampleOffsetAdjusted == true)
              {
                //Since we went back by a sample,adjust IDX1 sample count,
                //Retrieved sample count and bytes count.
                if( m_nCurrAudioSampleInIdx1 > 0 )
                {
                  m_nCurrAudioSampleInIdx1--;
                }
                if(m_nCurrAudioPayloadSize[*trackIdFound] > nSizeToBeAdjusted)
                {
                  m_nCurrAudioPayloadSize[*trackIdFound]-=nSizeToBeAdjusted;
                }
                if(m_nCurrAudioFrameCount[*trackIdFound] > 0)
                {
                  m_nCurrAudioFrameCount[*trackIdFound]--;
                }
                bSampleOffsetAdjusted = false;
                nSizeToBeAdjusted = 0;
              }
              m_nCurrAudioSampleInIdx1++;
              #ifdef AVI_PARSER_DEBUG
                QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                              "Audio->GetNextSampleInfo->m_nCurrAudioSampleInIdx1 %ld",
                              m_nCurrAudioSampleInIdx1);
              #endif

              if(GetAudioTrackSummaryInfo(*trackIdFound,&ainfo)==AVI_SUCCESS)
              {
                if( (!ainfo.isVbr)&&(ainfo.audioBytesPerSec) )
                {
                  sampleInfo->nTimeStamp = (avi_uint64)((float)m_nCurrAudioPayloadSize[*trackIdFound] /
                  (float)ainfo.audioBytesPerSec * 1000.0f);

                 sampleInfo->nDuration = (avi_uint32)((avi_uint64)((float)(m_nCurrAudioPayloadSize[*trackIdFound]+sampleInfo->nSampleSize) /

                (float)ainfo.audioBytesPerSec * 1000.0f) - sampleInfo->nTimeStamp);

                  m_nParserAudSampleEndTime[*trackIdFound] = sampleInfo->nTimeStamp+sampleInfo->nDuration;

                 QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                                "Audio->GetNextSampleInfo->CBR:nTimeStamp %d nDuration %d",
                                sampleInfo->nTimeStamp,sampleInfo->nDuration);
                }
                else if(ainfo.isVbr &&
                      ainfo.audioFrequency &&
                      (GetAudioInfo(*trackIdFound,&audinfo)== AVI_SUCCESS) )
                {
                QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AUDIO TRACK IS VBR");

                if(audinfo.strhAudio.dwSampleSize > 0 )
                  {
                  //Multiple samples will be grouped together in single chunk
                    sampleInfo->nTimeStamp = m_nParserAudSampleEndTime[*trackIdFound];
                    double val1 = ceil( ( (double)sampleInfo->nSampleSize / ainfo.nBlockAlign) );
                    double val2 =  ((double)audinfo.strhAudio.dwScale/audinfo.strhAudio.dwRate)* 1000.f;
                    sampleInfo->nDuration =  (uint32)(val1 * val2);
                    m_nParserAudSampleEndTime[*trackIdFound] = sampleInfo->nTimeStamp+sampleInfo->nDuration;
                }
                else
                {
                  sampleInfo->nTimeStamp = (avi_uint64)(( ( (float)audinfo.strhAudio.dwScale/
                                          (float)audinfo.strhAudio.dwRate) * 1000.f) * (float)m_nCurrAudioFrameCount[*trackIdFound]);
                  avi_uint64 nextAudioTS = (avi_uint64)(( ( (float)audinfo.strhAudio.dwScale/
                                          (float)audinfo.strhAudio.dwRate) * 1000.f) * (float)(m_nCurrAudioFrameCount[*trackIdFound]+1) );
                  sampleInfo->nDuration = (avi_uint32) (nextAudioTS - sampleInfo->nTimeStamp);
                }
                QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "Audio->GetNextSampleInfo->VBR:nTimeStamp %d nDuration %d",
                                sampleInfo->nTimeStamp,sampleInfo->nDuration);
                  }
                }
              m_nCurrAudioPayloadSize[*trackIdFound]+=sampleInfo->nSampleSize;
              m_nCurrAudioFrameCount[*trackIdFound]++;
              #ifdef AVI_PARSER_DEBUG
                QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                              "Audio->GetNextSampleInfo->m_nCurrAudioPayloadSize[*trackIdFound] %d m_nCurrAudioFrameCount[*trackIdFound] %ld",
                              m_nCurrAudioPayloadSize[*trackIdFound],m_nCurrAudioFrameCount[*trackIdFound]);
              #endif

              if( sampleInfo->nSampleSize == 0 )
              {
                QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Audio->GetNextSampleInfo encountered zero size sample");
                continue;
              }
              //(void)updateSampleRetrievedOffset(AVI_CHUNK_AUDIO,*trackIdFound);
              break;
            case AVI_CHUNK_VIDEO:
              avi_video_info vinfo;
              if( bSampleOffsetAdjusted == true)
              {
                //Since we went back by a sample,adjust IDX1 sample count,
                //Retrieved sample count and bytes count.
                if( m_nCurrVideoSampleInIdx1 > 0 )
                {
                  m_nCurrVideoSampleInIdx1--;
                }
                if(m_nCurrVideoFrameCount[*trackIdFound] > 0)
                {
                  m_nCurrVideoFrameCount[*trackIdFound]--;
                }
                bSampleOffsetAdjusted = false;
                nSizeToBeAdjusted = 0;
              }
              m_nCurrVideoSampleInIdx1++;
              #ifdef AVI_PARSER_DEBUG
                QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                              "Video->GetNextSampleInfo->m_nCurrVideoSampleInIdx1 %d",
                              m_nCurrVideoSampleInIdx1);
              #endif

              if( (GetVideoInfo(*trackIdFound,&vinfo)==AVI_SUCCESS) && (vinfo.strhVideo.dwRate) )
              {
                sampleInfo->nTimeStamp =
                  (avi_uint64)( ( (float)vinfo.strhVideo.dwScale/(float)vinfo.strhVideo.dwRate)*
                                (float)m_nCurrVideoFrameCount[*trackIdFound]*1000.0f);
                sampleInfo->nDuration =
                  (avi_uint32) ( ( ((float)vinfo.strhVideo.dwScale / (float)vinfo.strhVideo.dwRate) ) * 1000.0f );
                //#ifdef AVI_PARSER_DEBUG
                QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                              "Video->GetNextSampleInfo:nTimeStamp %d nDuration %d",
                              sampleInfo->nTimeStamp,sampleInfo->nDuration);
                //#endif
              }
              m_nCurrVideoFrameCount[*trackIdFound]++;
              #ifdef AVI_PARSER_DEBUG
                QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                              "Video->GetNextSampleInfo->m_nCurrVideoFrameCount[*trackIdFound] %d",
                              m_nCurrVideoFrameCount[*trackIdFound]);
              #endif
              if( sampleInfo->nSampleSize == 0 )
              {
                QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Video->GetNextSampleInfo encountered zero size sample");
                continue;
              }
              //(void)updateSampleRetrievedOffset(AVI_CHUNK_VIDEO,*trackIdFound);
              break;
            default:
              break;
          }
        }
        retError = AVI_SUCCESS;
        m_nCurrentChunkDataSize = sampleInfo->nSampleSize;
        m_CurrentParserState = AVI_PARSER_CHUNK_DATA_START;
        break;
      }
    }
    if( (m_nFileSize && (m_nCurrOffset >= m_nFileSize) ) || (bEndTrack == true) )
    {
      //check if we are done reading 'MOVI'
      #ifdef FEATURE_FILESOURCE_AVI_PARSER_DEBUG
#error code not present
      #endif
      return AVI_END_OF_FILE;
    }
  }
  else
  {
    retError = AVI_INVALID_USER_DATA;
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS,
                     QTVDIAG_PRIO_FATAL,
                     "GetNextSampleInfo AVI_INVALID_USER_DATA");
  }
  return retError;
}
/* =============================================================================
FUNCTION:
 aviParser::GetTrackWholeIDList

DESCRIPTION:
Returns trackId list for all the tracks in given clip.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
avi_uint32 aviParser::GetTrackWholeIDList(avi_uint32* idList)
{
  if(!idList)
  {
    return 0;
  }
  for (avi_uint32 i = 0; i < m_hAviSummary.n_streams; i++)
  {
    (*idList) = m_hAviSummary.stream_index[i].index;
    idList++;
  }
  return m_hAviSummary.n_streams;
}
/* =============================================================================
FUNCTION:
 aviParser::doSanityCheckBeforeSeek

DESCRIPTION:
 To be used only for debugging purpose.
 Does quick sanity testing by comparing
 audio/video sample count retrieved so far against audio/video sample count
 in IDX1. Also compares the offset being used by parser against
 sample offset in IDX1.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
bool aviParser::doSanityCheckBeforeSeek(avi_uint8 tid,CHUNK_t tracktype,aviParserState state)
{
  QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                   "doSanityCheckBeforeSeek tid %d tracktype %d",tid,tracktype);
  bool bok = true;
  unsigned char* byteData = m_ReadBuffer;
  avi_uint16 cType = 0;
  avi_uint16 trackId = 0;
  avi_uint32 dwFlags = 0;
  avi_uint32 dwOffset = 0;
  avi_uint32 dwSize = 0;

  //This read should always be xxwb,xxdc for audio/video.
  if(!AVICallbakGetData(m_nCurrentSampleInfoOffsetInIdx1,4 * sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                 "doSanityCheckBeforeSeek AVI_READ_FAILURE");
    return false;
  }
  memcpy(&trackId,byteData,sizeof(avi_uint16));
  trackId = ascii_2_short_int(&trackId);
  memcpy(&cType,byteData+2,sizeof(avi_uint16));
  memcpy(&dwFlags,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
  memcpy(&dwOffset,byteData+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
  if(m_bByteAdjustedForMOVI)
  {
    dwOffset = (m_nStartOfMovi + dwOffset - m_nBytesToBeAdjustedForMOVI);
  }
  memcpy(&dwSize,byteData+(3*sizeof(avi_uint32)),sizeof(avi_uint32));

  //State and Offset/chunk size Sanity.
  if(state == AVI_PARSER_CHUNK_DATA_START)
  {
    if(m_nCurrentChunkDataSize != dwSize)
    {
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "doSanityCheckBeforeSeek dwSize %ld did not match m_nCurrentChunkDataSize %ld",
                   dwSize,m_nCurrentChunkDataSize);
      bok = false;
    }
    if( m_nCurrOffset != (dwOffset - AVI_SIZEOF_FOURCC_PLUS_LENGTH) )
    {
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "doSanityCheckBeforeSeek m_nCurrOffset %ld did not match dwOffset %ld",
                   m_nCurrOffset,(dwOffset - AVI_SIZEOF_FOURCC_PLUS_LENGTH));
      bok = false;
    }
  }
  else if(state == AVI_PARSER_CHUNK_HEADER_START)
  {
    if(m_nCurrOffset != dwOffset)
    {
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "doSanityCheckBeforeSeek m_nCurrOffset %ld did not match dwOffset %ld",
                   m_nCurrOffset,dwOffset);
      bok = false;
    }
  }
  else
  {
    //Parser is in BAD STATE
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "doSanityCheckBeforeSeek m_CurrentParserState in wrong state %d",
                   state);
    bok = false;
  }

  //Sample Count Sanity
  if(tracktype == AVI_CHUNK_AUDIO)
  {
    if(m_nCurrAudioSampleInIdx1 != m_nCurrAudioFrameCount[tid])
    {
       QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "doSanityCheckBeforeSeek AUDIO m_nCurrAudioFrameCount did not match!!!");
       bok = false;
    }
  }
  else if(tracktype == AVI_CHUNK_VIDEO)
  {
    if(m_nCurrVideoSampleInIdx1 != m_nCurrVideoFrameCount[tid])
    {
       QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "doSanityCheckBeforeSeek VIDEO m_nCurrVideoFrameCount did not match!!!");
        bok = false;
    }
  }
  if(bok)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"doSanityCheckBeforeSeek Successful!!");
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,"PLEASE CHECK as doSanityCheckBeforeSeek Failed..");
  }
  return bok;
}

/* =============================================================================
FUNCTION:
 aviParser::getCurrentPlaybackTime

DESCRIPTION:
Returns the current playback time for given trackid.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:
Current Playback time in msec.

SIDE EFFECTS:
  None.
=============================================================================*/
avi_int64 aviParser::getCurrentPlaybackTime(avi_uint8 trackid)
{
  avi_audiotrack_summary_info ainfo;
  avi_video_info vinfo;
  avi_int64 nCurrentTime = 0;
  CHUNK_t cType;

  if(GetTrackChunkType(trackid,&cType)==AVI_SUCCESS)
  {
    switch(cType)
    {
      case AVI_CHUNK_AUDIO:
        if(GetAudioTrackSummaryInfo(trackid,&ainfo)==AVI_SUCCESS)
        {
          if( (!ainfo.isVbr)&&(ainfo.audioBytesPerSec) )
          {
            nCurrentTime = (avi_int64)((float)m_nCurrAudioPayloadSize[trackid] /
                                       (float)ainfo.audioBytesPerSec * 1000.0f);
          }
          else if(ainfo.isVbr && ainfo.audioFrequency)
          {
            avi_audio_info audinfo;
            if(GetAudioInfo(trackid,&audinfo) == AVI_SUCCESS)
            {
              if(audinfo.strhAudio.dwSampleSize > 0)
              {
          nCurrentTime =  m_nParserAudSampleEndTime[trackid];
        }
              else
              {
                nCurrentTime = (avi_uint64)(( ( (float)audinfo.strhAudio.dwScale/
                (float)audinfo.strhAudio.dwRate) * 1000.f) * (float)m_nCurrAudioFrameCount[trackid] );
              }
            }
          }//else if(ainfo.isVbr && ainfo.audioFrequency)
        }
        break;
      case AVI_CHUNK_VIDEO:
        if( (GetVideoInfo(trackid,&vinfo)==AVI_SUCCESS) && (vinfo.strhVideo.dwRate) )
        {
          nCurrentTime = (avi_int64)( ( (float)vinfo.strhVideo.dwScale/(float)vinfo.strhVideo.dwRate)*
                                        (float)m_nCurrVideoFrameCount[trackid]*1000.0f);
        }
        break;
      default:
        break;
    }
  }
  return nCurrentTime;
}
/* =============================================================================
FUNCTION:
 aviParser::Seek

DESCRIPTION:
 Seek the track identified by given trackid.

INPUT/OUTPUT PARAMETERS:
 trackid: Identifies the track to be repositioned.
 nReposTime:Target time to seek to
 nCurrPlayTime: Current playback time
 sample_info: Sample Info to be filled in if seek is successful
 canSyncToNonKeyFrame: When true, video can be repositioned to non key frame
 nSyncFramesToSkip:Number of sync samples to skip. Valid only if > or < 0.

RETURN VALUE:
 AVI_SUCCESS if successful otherwise returns appropriate error code.

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType  aviParser::Seek(avi_uint8 trackid,
                              avi_uint64 nReposTime,
                              avi_uint32 nCurrPlayTime,
                              avi_sample_info* sample_info,
                              bool canSyncToNonKeyFrame,
                              int  nSyncFramesToSkip)
{
  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                  "Seek trackId %d nReposTime %d canSyncToNonKeyFrame %d",
                  trackid,nReposTime,canSyncToNonKeyFrame);
  #endif
  aviErrorType retError = AVI_FAILURE;
  CHUNK_t cType;
  avi_int64 nCurrentTime = -1;
  bool bSearchForward = false;
  aviParserState prvParserState = m_CurrentParserState;

  avi_audiotrack_summary_info ainfo;
  avi_video_info vinfo;

  if(!sample_info)
  {
     QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                   "Seek sample_info is NULL..");
    return AVI_INVALID_USER_DATA;
  }
  memset(sample_info,0,sizeof(avi_sample_info));
  memset(&ainfo,0,sizeof(avi_audiotrack_summary_info));
  memset(&vinfo,0,sizeof(avi_video_info));

  //Make sure parser is in valid state.
  if( (m_CurrentParserState == AVI_PARSER_READ_FAILED)          ||
      (m_CurrentParserState == AVI_PARSER_FAILED_CORRUPTED_FILE)||
      (m_CurrentParserState == AVI_PARSER_SEEK))
  {
     QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                   "Seek Invalid m_CurrentParserState %d",m_CurrentParserState);
     return AVI_FAILURE;
  }
  //Make sure given trackid is valid.

  if(trackid < m_hAviSummary.n_streams)
  {
    //Make sure duration is valid..
    avi_uint64 max_duration = GetTrackDuration(trackid);
    if((max_duration < nReposTime) && (nReposTime != 0) && (max_duration > 0) &&
       (GetTrackChunkType(trackid,&cType) == AVI_SUCCESS) && (cType == AVI_CHUNK_AUDIO) )
    {
      QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                     "Seek Time correction TrackDuration %d <= Reposition time %d for trackid %d",max_duration,nReposTime,trackid);
      nReposTime = max_duration;
    }
    if(GetTrackChunkType(trackid,&cType)==AVI_SUCCESS)
    {
      m_CurrentParserState = AVI_PARSER_SEEK;

      /*
      * When nSyncFramesToSkip is 0,we need to seek based on
      * nReposTime otherwise based on number of sync frames to skip.
      * Thus, when seeking based on target seek time, nSyncFramesToSkip should be 0.
      */
      if( (nReposTime == 0 ) && ( nSyncFramesToSkip == 0 ) )
      {
        if(m_nAdjustedIdx1Offset == 0)
        {
          m_nCurrentSampleInfoOffsetInIdx1 = m_nIdx1Offset;
        }
        else
        {
          m_nCurrentSampleInfoOffsetInIdx1 = m_nAdjustedIdx1Offset;
           QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                    "Seek updated m_nCurrentSampleInfoOffsetInIdx1 to use m_nAdjustedIdx1Offset %d",m_nAdjustedIdx1Offset);
        }
        m_nCurrAudioSampleInIdx1 = 0;
        m_nCurrVideoSampleInIdx1 = 0;

        nCurrentTime = 0;
        if(m_nAdjustedMoviOffset == 0)
        {
          m_nCurrOffset = m_nMoviOffset;
        }
        else
        {
          m_nCurrOffset = m_nAdjustedMoviOffset;
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                    "Seek updated m_nCurrOffset to use m_nAdjustedMoviOffset %d",m_nAdjustedMoviOffset);
        }
        m_nCurrentChunkDataSize = 0;
        m_CurrentParserState = AVI_PARSER_CHUNK_HEADER_START;
        sample_info->nTimeStamp =  nCurrentTime;
        sample_info->chunkType = cType;
        retError = AVI_SUCCESS;
        if(sample_info->chunkType == AVI_CHUNK_AUDIO)
        {
          //AUDIO
          m_nCurrAudioPayloadSize[trackid] = 0;
          m_nCurrAudioFrameCount[trackid] = 0;
          m_nParserAudSampleEndTime[trackid] = 0;
        }
        if(sample_info->chunkType == AVI_CHUNK_VIDEO)
        {
          //VIDEO
          m_nCurrVideoFrameCount[trackid] = 0;
        }
        return retError;
      }
      if(!m_nIdx1Offset)
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
        "Seek Failed, no IDX1 available");
        return AVI_FAILURE;
      }
      nCurrentTime =  nCurrPlayTime;

      QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                    "Seek nCurrPlayTime %d nReposTime %ld nSyncFramesToSkip %d",
                    nCurrPlayTime,nReposTime,nSyncFramesToSkip);

      switch(cType)
      {
        case AVI_CHUNK_AUDIO:
            if(nCurrentTime >= 0)
            {
              bSearchForward = (nReposTime > nCurrentTime)?true:false;
              QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Seek AUDIO bSearchForward %d",bSearchForward);
                  avi_idx1_entry matchedEntry;
                  memset(&matchedEntry,0,sizeof(avi_idx1_entry));
	                aviErrorType result = AVI_PARSE_ERROR;
                  bool endOfFileReached = false;
                  #ifdef AVI_PARSER_SEEK_SANITY_TEST
                    //Debug purpose
                    (void)doSanityCheckBeforeSeek(trackid,AVI_CHUNK_AUDIO,prvParserState);
                  #endif
#ifndef AVI_PARSER_FAST_START_UP
	            if( (m_hAviSummary.pIdx1Table)&&
                  (m_hAviSummary.pIdx1Table->pAudioEntries) &&
                  (nSyncFramesToSkip == 0) )
      	    {
		          result = searchIDX1Cache(trackid,
                                       nReposTime,
                                       &matchedEntry,
                                       bSearchForward,
                                       AVI_CHUNK_AUDIO,
                                       nSyncFramesToSkip,
                                       &endOfFileReached);
	          }
      	    else
#endif
            {
              result = seekInIDX1(trackid,
                                  nReposTime,
                                  &matchedEntry,
                                  bSearchForward,
                                  AVI_CHUNK_AUDIO,
                                  nSyncFramesToSkip,
                                  &endOfFileReached);
            }
                  if( ((result == AVI_SUCCESS) || (endOfFileReached)) && (matchedEntry.chunkType == AVI_CHUNK_AUDIO))
                  {
                    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                                  "Matched Audio index entry TS %d",matchedEntry.nTimeStamp);

                    m_nCurrOffset = matchedEntry.dwOffset;
                    m_nCurrentChunkDataSize = 0;
                    m_nCurrAudioPayloadSize[trackid] = matchedEntry.nTotalSizeInBytes;
                    m_nCurrAudioFrameCount[trackid] = matchedEntry.dwAudFrameCount;
                    m_nCurrentSampleInfoOffsetInIdx1 = matchedEntry.dwOffsetInIDX1;

                    m_CurrentParserState = AVI_PARSER_CHUNK_HEADER_START;

                    QTV_MSG_PRIO6(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                                  "m_nCurrOffset %d trackid %d m_nCurrAudioPayloadSize[trackid] %d Chunk# %d m_CurrentParserState %d m_nCurrentSampleInfoOffsetInIdx1 %d",
                                  m_nCurrOffset, trackid, m_nCurrAudioPayloadSize[trackid], m_nCurrAudioFrameCount[trackid],m_CurrentParserState,m_nCurrentSampleInfoOffsetInIdx1);

                    sample_info->chunkType  = AVI_CHUNK_AUDIO;
                    sample_info->nSampleSize= matchedEntry.dwSize;
                    sample_info->nTimeStamp = (avi_uint64)matchedEntry.nTimeStamp;
                    if(GetAudioTrackSummaryInfo(trackid,&ainfo)== AVI_SUCCESS)
                    {
                      if( (!ainfo.isVbr) && (ainfo.audioBytesPerSec) )
                      {
                        sample_info->nDuration = (avi_uint32)(sample_info->nTimeStamp - (avi_int64)((float)(m_nCurrAudioPayloadSize[trackid]+ sample_info->nSampleSize)/
                                                 (float)ainfo.audioBytesPerSec * 1000.0f));
                        m_nParserAudSampleEndTime[trackid] = sample_info->nTimeStamp;
                      }
                      else if(ainfo.isVbr && ainfo.audioFrequency)
                      {
                        avi_audio_info audinfo;
                        if( (GetAudioInfo(trackid,&audinfo) == AVI_SUCCESS) && (audinfo.strhAudio.dwRate) )
                        {
                          m_nParserAudSampleEndTime[trackid] = sample_info->nTimeStamp;
                          sample_info->nDuration = (avi_uint32)
                                ( ((double)audinfo.strhAudio.dwScale/audinfo.strhAudio.dwRate)* 1000.f);
                        }
                      }
                    }
                    retError = AVI_SUCCESS;
                  }
                  break;
            }//if(nCurrentTime >= 0)
            break;
        case AVI_CHUNK_VIDEO:
            if(nCurrentTime >= 0)
            {
              bSearchForward = (nReposTime > nCurrentTime)?true:false;
              QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "Seek VIDEO bSearchForward %d",bSearchForward);
                avi_idx1_entry matchedEntry;
                memset(&matchedEntry,0,sizeof(avi_idx1_entry));
                aviErrorType result = AVI_PARSE_ERROR;
                bool endOfFileReached = false;
                #ifdef AVI_PARSER_SEEK_SANITY_TEST
                  //Debug purpose
                  (void)doSanityCheckBeforeSeek(trackid,AVI_CHUNK_VIDEO,prvParserState);
                #endif
#ifndef AVI_PARSER_FAST_START_UP
	            if( (m_hAviSummary.pIdx1Table)&&
                  (m_hAviSummary.pIdx1Table->pKeyVideoEntries) &&
                  (nSyncFramesToSkip == 0) )
	            {
		            result = searchIDX1Cache(trackid,
                                         nReposTime,
                                         &matchedEntry,
                                         bSearchForward,
                                         AVI_CHUNK_VIDEO,
                                         nSyncFramesToSkip,
                                         &endOfFileReached);
              }
	            else
#endif
              {
                result = seekInIDX1(trackid,
                                    nReposTime,
                                    &matchedEntry,
                                    bSearchForward,
                                    AVI_CHUNK_VIDEO,
                                    nSyncFramesToSkip,
                                    &endOfFileReached);
              }
                if( ( (result == AVI_SUCCESS) || (endOfFileReached) ) && (matchedEntry.chunkType == AVI_CHUNK_VIDEO) )
                {
                  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                                "Matched Video index entry TS %d",matchedEntry.nTimeStamp);

                  m_nCurrOffset = matchedEntry.dwOffset;
                  m_nCurrentChunkDataSize = 0;
                  m_nCurrVideoFrameCount[trackid] = matchedEntry.dwVidFrameCount;
                  m_CurrentParserState = AVI_PARSER_CHUNK_HEADER_START;
                  m_nCurrentSampleInfoOffsetInIdx1 = matchedEntry.dwOffsetInIDX1;

                  QTV_MSG_PRIO5(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                                "m_nCurrOffset %d trackid %d m_nCurrVideoFrameCount[trackid] %d m_CurrentParserState %d m_nCurrentSampleInfoOffsetInIdx1 %d",
                                 m_nCurrOffset, trackid, m_nCurrVideoFrameCount[trackid],m_CurrentParserState,m_nCurrentSampleInfoOffsetInIdx1);

                  sample_info->chunkType  = AVI_CHUNK_VIDEO;
                  sample_info->nSampleSize= matchedEntry.dwSize;
                  sample_info->nTimeStamp = (avi_uint64)matchedEntry.nTimeStamp;
                  if(GetVideoInfo(trackid,&vinfo)== AVI_SUCCESS)
                  {
                    sample_info->nDuration  = (avi_int64)( ( (float)vinfo.strhVideo.dwScale/(float)vinfo.strhVideo.dwRate)*
                                                           (float)1000.0f );
                  }
                  /*
                  * If clip is encrypted, we need to locate corresponding DRM chunk for the matched entry, if any.
                  * If yes, need to adjust IDX1/MOVI offset so that parser will parse DRM chunk before retrieving matched video sample.
                  */
                  int nDrmChunkSize = 0;
                  nDrmChunkSize = isCurrentFrameEncrypted(trackid,m_nCurrentSampleInfoOffsetInIdx1,m_nCurrOffset);

                  if( nDrmChunkSize )
                  {
                    m_nCurrentSampleInfoOffsetInIdx1 -= (4 * sizeof(avi_uint32) );
                    m_nCurrOffset -= (nDrmChunkSize+sizeof(fourCC_t)+sizeof(avi_uint32));

                    QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                                "nDrmChunkSize %ld updated m_nCurrOffset %ld updated m_nCurrentSampleInfoOffsetInIdx1 %ld",
                                 nDrmChunkSize, m_nCurrOffset, m_nCurrentSampleInfoOffsetInIdx1);
                  }
                  retError = AVI_SUCCESS;
                }
                break;
            }//if(nCurrentTime >= 0)
            break;
        default:
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Can't seek trackId %d",trackid);
        break;
      }
    }//if(GetTrackChunkType(trackid,&cType)==AVI_SUCCESS)
  }//if(trackid < m_hAviSummary.n_streams)
  if( retError != AVI_SUCCESS)
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,"Reposition failed for trackid %d",trackid);
    m_CurrentParserState = prvParserState;
  }
  return  retError;
}
/* =============================================================================
FUNCTION:
 aviParser::isCurrentFrameEncrypted

DESCRIPTION:
Determine if current sample is encrypted or not.

trackid      : Identifies the track
offsetinidx1 : Offset of sample in idx1
movioffset   : sample offset in MOVI

INPUT/OUTPUT PARAMETERS:
  @See above.

RETURN VALUE:
 DRM chunk size for the sample identified by given trackid and offsetinidx1/movioffset

SIDE EFFECTS:
  None.
=============================================================================*/
avi_uint32 aviParser::isCurrentFrameEncrypted(avi_uint8 trid, avi_uint64 offsetinidx1, avi_uint64 movioffset)
{
  avi_uint32 drm_chunk_size = 0;
  unsigned char* byteData = m_ReadBuffer;

  avi_uint16 trackId = 0;
  avi_uint16 cType = 0;
  avi_uint32 dwFlags = 0;
  avi_uint32 dwOffset = 0;
  avi_uint32 dwSize = 0;
  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO3(QTVDIAG_FILE_OPS,
                  QTVDIAG_PRIO_HIGH,"isCurrentFrameEncrypted trid %ld offsetinidx1 %ld movioffset %ld",
                  trid,offsetinidx1,movioffset);
  #endif

  offsetinidx1 = offsetinidx1 - (4 * sizeof(avi_uint32));

  if(!AVICallbakGetData(offsetinidx1,4 * sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,"isCurrentFrameEncrypted AVI_READ_FAILURE");
    return 0;
  }
  memcpy(&trackId,byteData,sizeof(avi_uint16));
  trackId = ascii_2_short_int(&trackId);
  memcpy(&cType,byteData+2,sizeof(avi_uint16));
  memcpy(&dwFlags,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
  memcpy(&dwOffset,byteData+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
  if(m_bByteAdjustedForMOVI)
  {
    dwOffset = (m_nStartOfMovi + dwOffset - m_nBytesToBeAdjustedForMOVI);
  }
  memcpy(&dwSize,byteData+(3*sizeof(avi_uint32)),sizeof(avi_uint32));

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO4(QTVDIAG_FILE_OPS,
                  QTVDIAG_PRIO_HIGH,"isCurrentFrameEncrypted trackId %d cType %d dwFlags %d dwOffset %d",
                  trackId,cType,dwFlags,dwOffset);
  #endif
  if( (!memcmp(&cType,"dd",sizeof(avi_uint16))) && (trackId == trid) )
  {
    drm_chunk_size = dwSize;
    #ifdef AVI_PARSER_DEBUG
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                    "isCurrentFrameEncrypted Matched sample is encrypted,located corresponding DRM chunk drm_chunk_size %d",drm_chunk_size);
    #endif
    if( (dwOffset + drm_chunk_size + sizeof(fourCC_t)+sizeof(avi_uint32)) != movioffset )
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "isCurrentFrameEncrypted matched DRM chunk offset mismatch with current MOVI offset..");
      QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "isCurrentFrameEncrypted dwOffset %ld drm_chunk_size %ld movioffset %ld",
                    dwOffset, drm_chunk_size, movioffset);
    }
  }
  return drm_chunk_size;
}
/* =============================================================================
FUNCTION:
 aviParser::flushIdx1SeekCache

DESCRIPTION:
 Cache management routine to flush out IDX1 Seek cache.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
 NOne

 SIDE EFFECTS:
  None.
=============================================================================*/
void aviParser::flushIdx1SeekCache(void)
{
  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"flushIdx1SeekCache");
  #endif

  if(m_pIdx1SeekCache && m_pIdx1SeekCache->pMemory)
  {
    #ifdef AVI_PARSER_DEBUG
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"flushIdx1SeekCache resetting Cache structure..");
    #endif
    m_pIdx1SeekCache->nReadOffset = 0;
    m_pIdx1SeekCache->nSize = 0;
    m_pIdx1SeekCache->nStartOffset = 0;
    m_pIdx1SeekCache->nEndOffset = 0;
  }
}
/* =============================================================================
FUNCTION:
 aviParser::readFromIdx1SeekCache

DESCRIPTION:
 Cache management routine to be used to carry out Seek especially in RW
 as OSCL cache works in forward direction. We want caching to be
 in backward direction when performing Rewind to reduce the delay.
 Note: Currently, this routine can be used as a cache only for reading IDX1 in backward direction.
       To use it for general purpose, need to change bound checking on tmpEndOffset and tmpStartOffset.
       To be able to use in both direction, we need to add the direction as well.
       Don't free up the memory once RW is done. Frequent allocation/free up might result in
       Heap fragmentation subsequently as user won't be able to RW.

 nOffset          :Offset to read from.
 nNumBytesRequest : Number of bytes to read.
 ppData           : Buffer to read in data.

INPUT/OUTPUT PARAMETERS:
  @See above.

RETURN VALUE:
 Number of bytes read.

SIDE EFFECTS:
  None.
=============================================================================*/
avi_uint32 aviParser::readFromIdx1SeekCache(avi_uint64 nOffset,
                                            avi_int32 nNumBytesRequest,
                                            avi_int32 nMaxSize,
                                            unsigned char *pData)
{
  avi_uint32 nRead = 0;
  avi_uint64 tmpSize = 0;
  avi_uint64 tmpStartOffset = 0;
  avi_uint64 tmpEndOffset = 0;

#ifdef AVI_PARSER_DEBUG
  QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                "readFromIdx1SeekCache nOffset %d nNumBytesRequest %d",
                nOffset,nNumBytesRequest);
#endif

  if(!m_pIdx1SeekCache)
  {
    m_pIdx1SeekCache = (avi_parser_seek_buffer_cache*)
                       QTV_Malloc(sizeof(avi_parser_seek_buffer_cache));
    if(!m_pIdx1SeekCache)
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "readFromIdx1SeekCache failed to allocate memory for m_pIdx1SeekCache");
      return 0;
    }
    memset(m_pIdx1SeekCache,0,sizeof(avi_parser_seek_buffer_cache));
  }

  if(!m_pIdx1SeekCache->pMemory)
  {
    m_pIdx1SeekCache->nReadOffset = -1;
    m_pIdx1SeekCache->pMemory = (avi_uint8*)
                       QTV_Malloc(AVI_PARSER_IDX1_SEEK_BUFFER_CACHE_SIZE);
    if(!m_pIdx1SeekCache->pMemory)
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                   "readFromIdx1SeekCache failed to allocate memory for m_pIdx1SeekCache->pMemory");
      QTV_Free(m_pIdx1SeekCache);
      m_pIdx1SeekCache = NULL;
      return 0;
    }
  }


    if( (nOffset < m_pIdx1SeekCache->nStartOffset)                 ||
        ((nOffset+nNumBytesRequest) > m_pIdx1SeekCache->nEndOffset)||
        (m_pIdx1SeekCache->nReadOffset < 0) )
  {
    //Time to refill cache, flush curent cache state.
    flushIdx1SeekCache();
    m_pIdx1SeekCache->nReadOffset = -1;
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                 "readFromIdx1SeekCache Refilling cache...");
  }

  if(m_pIdx1SeekCache->nReadOffset < 0)
  {
    //Cache is empty, fill it up.

    //Find the range to cache.
    tmpEndOffset = nOffset + nNumBytesRequest;

    //Validate end offset
    if( (m_nIdx1Offset + m_nIdx1Size) < tmpEndOffset)
    {
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                    "readFromIdx1SeekCache tmpEndOffset %d > (m_nIdx1Offset + m_nIdx1Size) %d",
                    tmpEndOffset,(m_nIdx1Offset + m_nIdx1Size));
      tmpEndOffset = m_nIdx1Offset + m_nIdx1Size;

      if(tmpEndOffset < nOffset)
      {
        QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "readFromIdx1SeekCache adjusted tmpEndOffset %d < nOffset %d",
                      tmpEndOffset,nOffset);
        //We should never come here, print error message and bail out.
        return 0;
      }
    }

    tmpStartOffset = tmpEndOffset - AVI_PARSER_IDX1_SEEK_BUFFER_CACHE_SIZE;

    #ifdef AVI_PARSER_DEBUG
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                    "readFromIdx1SeekCache tmpStartOffset %d tmpEndOffset %d",
                    tmpStartOffset,tmpEndOffset);
    #endif

    if(tmpStartOffset < m_nIdx1Offset)
    {
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                    "readFromIdx1SeekCache tmpStartOffset %d < m_nIdx1Offset %d",tmpStartOffset,m_nIdx1Offset);
      tmpStartOffset = m_nIdx1Offset;
      if(tmpStartOffset > nOffset)
      {
        QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                      "readFromIdx1SeekCache adjusted tmpStartOffset %d > nOffset %d",
                      tmpStartOffset,nOffset);
        //We should never come here, print error message an bail out.
        return 0;
      }
    }
    #ifdef AVI_PARSER_DEBUG
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                    "readFromIdx1SeekCache tmpStartOffset %d tmpEndOffset %d",tmpStartOffset,tmpEndOffset);
    #endif

    tmpSize = tmpEndOffset - tmpStartOffset;

    #ifdef AVI_PARSER_DEBUG
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                    "readFromIdx1SeekCache tmpSize %d MAX SIZE %d",tmpSize,AVI_PARSER_IDX1_SEEK_BUFFER_CACHE_SIZE);
    #endif
    if(tmpSize > AVI_PARSER_IDX1_SEEK_BUFFER_CACHE_SIZE)
    {
      //Something is very bad, print an fatal error mesage and bail out.
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                 "readFromIdx1SeekCache tmpSize %d > MAX SIZE %d ",tmpSize,AVI_PARSER_IDX1_SEEK_BUFFER_CACHE_SIZE);
      return 0;
    }

    if(!AVICallbakGetData(tmpStartOffset,
                          (avi_int32)tmpSize,
                          m_pIdx1SeekCache->pMemory,
                          AVI_PARSER_IDX1_SEEK_BUFFER_CACHE_SIZE,
                          m_pUserData))
    {
      //Read failed, free up memory and return 0.
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                 "readFromIdx1SeekCache READ FAILED nOffset %d nBytes %d ",nOffset,AVI_PARSER_IDX1_SEEK_BUFFER_CACHE_SIZE);
      flushIdx1SeekCache();
      QTV_Free(m_pIdx1SeekCache->pMemory);
      QTV_Free(m_pIdx1SeekCache);
      m_pIdx1SeekCache = NULL;
      return 0;
    }
    m_pIdx1SeekCache->nStartOffset = tmpStartOffset;
    m_pIdx1SeekCache->nEndOffset   = tmpEndOffset;
    m_pIdx1SeekCache->nSize        = tmpSize;
    m_pIdx1SeekCache->nReadOffset  = tmpStartOffset;

    //Make sure offset to read belongs to our cached region
    if( (nOffset >= m_pIdx1SeekCache->nStartOffset) &&
        (nOffset <= m_pIdx1SeekCache->nEndOffset) )
    {
      avi_uint64 offdiff = nOffset - m_pIdx1SeekCache->nStartOffset;
      memcpy(pData,m_pIdx1SeekCache->pMemory+offdiff,nNumBytesRequest);
      //*ppData = (m_pIdx1SeekCache->pMemory+offdiff);
      nRead = nNumBytesRequest;
      m_pIdx1SeekCache->nReadOffset = nOffset;
    }
    else
    {
      //Something is terrible, print fatal message and bail out.
       QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                    "readFromIdx1SeekCache nOffset not in cached region nOffset %d m_pIdx1SeekCache->nStartOffset %d m_pIdx1SeekCache->nEndOffset %d",
                    nOffset, m_pIdx1SeekCache->nStartOffset, m_pIdx1SeekCache->nEndOffset);
      flushIdx1SeekCache();
      return 0;
    }
  }
  else
  {
    avi_uint64 offdiff = nOffset - m_pIdx1SeekCache->nStartOffset;
    memcpy(pData,m_pIdx1SeekCache->pMemory+offdiff,nNumBytesRequest);
    //*ppData = (m_pIdx1SeekCache->pMemory+offdiff);
    nRead = nNumBytesRequest;
    m_pIdx1SeekCache->nReadOffset = nOffset;
  }
  return nRead;
}
/* =============================================================================
FUNCTION:
 aviParser::searchIDX1Cache

DESCRIPTION:
 Seek the track identified by given trackid based on IDX1 cache

INPUT/OUTPUT PARAMETERS:
  trackid          : Identifies the track to be repositioned.
  nReposTime       : Target time to seek to.
  outputEntry      : Fills in idx1 entry information once the seek is complete.
  bSearchForward   : Direction to search in IDX1
  chunkType        : Track type
  nSyncFramesToSkip: Number of sync frames to skip in forward or backward direction.

Please note that nReposTime will be used to carry out SEEK only when nSyncFramesToSkip is 0.

RETURN VALUE:
  AVI_SUCCESS if successful otherwise returns appropriate error code.

SIDE EFFECTS:
  None.
=============================================================================*/
#ifndef AVI_PARSER_FAST_START_UP
aviErrorType aviParser::searchIDX1Cache(avi_uint32 trid,
                                       avi_uint64 nReposTime,
                                       avi_idx1_entry* outputEntry,
                                       bool bSearchForward,
                                       CHUNK_t chunkType,
                                       int  nSyncFramesToSkip,
                                       bool* endOfFileReached)
#else
aviErrorType aviParser::searchIDX1Cache(avi_uint32, avi_uint64, avi_idx1_entry*,
                                       bool, CHUNK_t, int, bool*)
#endif
{
	aviErrorType retError = AVI_PARSE_ERROR;
#ifndef AVI_PARSER_FAST_START_UP
	bool bOkToRepos = true;
	int entryIndex = 0;

  avi_uint16 cType = 0;
  avi_uint16 trackId = 0;
  avi_uint32 dwFlags = 0;
  avi_uint64 dwOffset = 0;
  avi_uint32 dwSize = 0;

  avi_uint32 audioChunksFromStart = 0;
  avi_uint64 audioBytesFromStart = 0;
  avi_uint64 tsAudio = 0;

	QTV_MSG_PRIO4(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache trackid %d bsearchfwd %d nReposTime %d nSyncFramesToSkip %d",
		trid,bSearchForward,nReposTime,nSyncFramesToSkip);

	if(!outputEntry)
	{
		bOkToRepos = false;
	}
	if((chunkType == AVI_CHUNK_AUDIO)&&(bOkToRepos))
	{
	  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache seeking audio track");
	  memset(outputEntry,0,sizeof(avi_idx1_entry));
      if( (!m_hAviSummary.pIdx1Table->pAudioEntries) ||
          (!m_hAviSummary.pIdx1Table->nCurrParsedAudioEntriesInIDX1) )
      {
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"No Audio Entries exist for trackid %d",trid);
        bOkToRepos = false;
	  }
    avi_uint32 nCacheIndex = (avi_uint32)(nReposTime / AUDIO_SAMPLE_CACHING_INTERVAL);
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache nCacheIndex %d",nCacheIndex);
    if(nCacheIndex > m_hAviSummary.pIdx1Table->nCurrParsedAudioEntriesInIDX1)
    {
      //Go to last entry if nCacheIndex is outside current parsed audio entries.
      nCacheIndex = m_hAviSummary.pIdx1Table->nCurrParsedAudioEntriesInIDX1 -1;
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache updated nCacheIndex %d",nCacheIndex);
    }
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache nCacheIndex %d",nCacheIndex);
    if( m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].nTimeStamp > nReposTime )
    {
      while(m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].nTimeStamp > nReposTime)
      {
        nCacheIndex--;
      }
    }
    if(!bSearchForward)
      nCacheIndex++;
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache updated nCacheIndex for REWIND CASE %d",nCacheIndex);
    bool bFirstEntry = true;
	  if(bSearchForward && bOkToRepos)
	  {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache seeking in FORWARD dirn");
      for(avi_uint64 itr = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].dwOffsetInIDX1;
          itr < (m_nIdx1Offset+m_nIdx1Size) ; )
      {
        if(!AVICallbakGetData(itr,4 * sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                         "searchIDX1Cache AVI_READ_FAILURE");
          retError = AVI_READ_FAILURE;
          return retError;
        }
        itr += (4 *sizeof(avi_uint32));
        if(bFirstEntry)
        {
          trackId = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].trackId;
          cType = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].chunkType;
          dwFlags = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].dwFlags;
          dwOffset= m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].dwOffset;
          audioChunksFromStart = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].dwAudFrameCount;
          audioBytesFromStart = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].nTotalSizeInBytes;
          tsAudio = (avi_uint64)m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].nTimeStamp;
          dwSize = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].dwSize;
        }
        else
        {
          memcpy(&trackId,m_ReadBuffer,sizeof(avi_uint16));
          trackId = ascii_2_short_int(&trackId);
          memcpy(&cType,m_ReadBuffer+2,sizeof(avi_uint16));
          memcpy(&dwFlags,m_ReadBuffer+sizeof(avi_uint32),sizeof(avi_uint32));
          memcpy(&dwOffset,m_ReadBuffer+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
          if(m_bByteAdjustedForMOVI)
          {
            dwOffset = (m_nStartOfMovi + dwOffset - m_nBytesToBeAdjustedForMOVI);
          }
          memcpy(&dwSize,m_ReadBuffer+(3*sizeof(avi_uint32)),sizeof(avi_uint32));
        }
        if( (trackId == trid) && ( (!memcmp(&cType,"wb",sizeof(avi_uint16)))||(bFirstEntry) ) )
        {
          avi_audio_info audstrminfo;
          avi_audiotrack_summary_info ainfo;
          memset(&audstrminfo,0,sizeof(avi_audio_info));
          memset(&ainfo,0,sizeof(avi_audiotrack_summary_info));

          if( (GetAudioTrackSummaryInfo(trackId,&ainfo)==AVI_SUCCESS) &&
              (GetAudioInfo(trackId,&audstrminfo) == AVI_SUCCESS) )
          {
            if(audstrminfo.strhAudio.dwSampleSize > 0)
            {
              if(audstrminfo.strfAudio.nBlockAlign && audstrminfo.strhAudio.dwRate)
              {
                double val1 = ceil( ( (double)dwSize / audstrminfo.strfAudio.nBlockAlign) );
                double val2 =  ((double)audstrminfo.strhAudio.dwScale/audstrminfo.strhAudio.dwRate)* 1000.f;
                tsAudio += (uint32)(val1 * val2);
              }
            }//if(audstrminfo.strhAudio.dwSampleSize > 0)
            else
            {
              tsAudio = (avi_uint64)(( ( (float)audstrminfo.strhAudio.dwScale/
                                      (float)audstrminfo.strhAudio.dwRate) * 1000.f) * (float)audioChunksFromStart);
            }
          }//if( (GetAudioTrackSummaryInfo(trackId,&ainfo)==AVI_SUCCESS) &&...
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache tsAudio %d",tsAudio);

          outputEntry->trackId = trid;
          outputEntry->dwFlags = dwFlags;
          outputEntry->dwOffset = dwOffset;
          outputEntry->dwSize = dwSize;
          outputEntry->dwOffsetInIDX1 = (itr - (4 *sizeof(avi_uint32)));
          outputEntry->dwAudFrameCount = audioChunksFromStart;
          outputEntry->nTimeStamp = (float)tsAudio;
          outputEntry->nTotalSizeInBytes = audioBytesFromStart;
          outputEntry->chunkType = AVI_CHUNK_AUDIO;
          m_nCurrAudioSampleInIdx1 = outputEntry->dwAudFrameCount;

          if( (tsAudio >= nReposTime) )
          {
            retError = AVI_SUCCESS;
            /*
            * Located the correct audio entry/chunk.
            * Load IDX1 entry values into output idx1 entry and return SUCCESS;
            */

            QTV_MSG_PRIO5(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                            "searchIDX1Cache Matched Audio Entry: trackid %d dwFlags %d dwOffset %d dwSize %d dwOffsetInIDX1 %d",
                            trid,dwFlags,dwOffset,dwSize,outputEntry->dwOffsetInIDX1);

            QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                          "searchIDX1Cache Matched Audio Entry: dwAudFrameCount %d TS %d nTotalSizeInBytes %d",
                          outputEntry->dwAudFrameCount,outputEntry->nTimeStamp,audioBytesFromStart);

            return retError;
          }//if(tsAudio >= nReposTime)
          else
	  {
            audioBytesFromStart += dwSize;
            audioChunksFromStart++;
            bFirstEntry = false;
          }//end of else of if(tsAudio >= nReposTime)
        }//if( (trackId == trid) && ( (!memcmp(&cType,"wb",sizeof(avi_uint16)))||(bFirstEntry) ) )
      }//for(avi_uint64 itr = offset; itr < (m_nIdx1Offset+m_nIdx1Size) ; )
      *endOfFileReached = true;
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache end of file reached while finding matched entry");
	  }//if(bSearchForward && bOkToRepos)
	  else if( (!bSearchForward)&& bOkToRepos )
	  {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache seeking in BACKWARD dirn");
      for(avi_uint64 itr = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].dwOffsetInIDX1;
          itr > m_nIdx1Offset; )
          {
        if(!AVICallbakGetData(itr,4 * sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
			{
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                         "searchIDX1Cache AVI_READ_FAILURE");
          retError = AVI_READ_FAILURE;
          return retError;
			}
        itr -= (4 *sizeof(avi_uint32));
        if(bFirstEntry)
            {
          trackId = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].trackId;
          cType = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].chunkType;
          dwFlags = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].dwFlags;
          dwOffset= m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].dwOffset;
          audioChunksFromStart = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].dwAudFrameCount;
          audioBytesFromStart = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].nTotalSizeInBytes;
          tsAudio = (avi_uint64)m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].nTimeStamp;
          dwSize = m_hAviSummary.pIdx1Table->pAudioEntries[nCacheIndex].dwSize;
			}
	  else
	  {
          memcpy(&trackId,m_ReadBuffer,sizeof(avi_uint16));
          trackId = ascii_2_short_int(&trackId);
          memcpy(&cType,m_ReadBuffer+2,sizeof(avi_uint16));
          memcpy(&dwFlags,m_ReadBuffer+sizeof(avi_uint32),sizeof(avi_uint32));
          memcpy(&dwOffset,m_ReadBuffer+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
          if(m_bByteAdjustedForMOVI)
          {
            dwOffset = (m_nStartOfMovi + dwOffset - m_nBytesToBeAdjustedForMOVI);
          }
          memcpy(&dwSize,m_ReadBuffer+(3*sizeof(avi_uint32)),sizeof(avi_uint32));
        }

        if( (trackId == trid) && ( (!memcmp(&cType,"wb",sizeof(avi_uint16)))||(bFirstEntry) ) )
        {
          avi_audio_info audstrminfo;
          avi_audiotrack_summary_info ainfo;
          memset(&audstrminfo,0,sizeof(avi_audio_info));
          memset(&ainfo,0,sizeof(avi_audiotrack_summary_info));
          if(!bFirstEntry)
		{
            audioBytesFromStart -= dwSize;
            audioChunksFromStart--;
            if( (GetAudioTrackSummaryInfo(trackId,&ainfo)==AVI_SUCCESS)&&
                (GetAudioInfo(trackId,&audstrminfo) == AVI_SUCCESS) )
            {
              if( (!ainfo.isVbr)&&(ainfo.audioBytesPerSec) )
              {
                tsAudio = (avi_uint32)((float)audioBytesFromStart /
                                      (float)ainfo.audioBytesPerSec * 1000.0f);
              }
              if( ainfo.isVbr )
              {
                if(audstrminfo.strhAudio.dwSampleSize > 0)
          {
                  if(audstrminfo.strfAudio.nBlockAlign && audstrminfo.strhAudio.dwRate)
            {
                    double val1 = ceil( ( (double)dwSize / audstrminfo.strfAudio.nBlockAlign) );
                    double val2 =  ((double)audstrminfo.strhAudio.dwScale/audstrminfo.strhAudio.dwRate)* 1000.f;
                    tsAudio -=  (uint32)(val1 * val2);
                  }
			}
		else
		{
                  tsAudio = (avi_uint64)(( ( (float)audstrminfo.strhAudio.dwScale/
                                          (float)audstrminfo.strhAudio.dwRate) * 1000.f) * (float)audioChunksFromStart);
                }
              }
            }//if( (GetAudioTrackSummaryInfo(trackId,&ainfo)==AVI_SUCCESS)&&..
          }//if(!bFirstEntry)
          bFirstEntry = false;
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,"searchIDX1Cache tsAudio %d",tsAudio);

          if(tsAudio <= nReposTime)
            {
            retError = AVI_SUCCESS;
            /*
            * Located the correct audio entry/chunk.
            * Load IDX1 entry values into output idx1 entry and return SUCCESS;
            */
            outputEntry->trackId = trid;
            outputEntry->dwFlags = dwFlags;
            outputEntry->dwOffset = dwOffset;
            outputEntry->dwSize = dwSize;
            outputEntry->dwOffsetInIDX1 = (itr + (4 *sizeof(avi_uint32)));

            outputEntry->dwAudFrameCount = audioChunksFromStart;
            outputEntry->nTimeStamp = (float)tsAudio;
            outputEntry->nTotalSizeInBytes = audioBytesFromStart;
            outputEntry->chunkType = AVI_CHUNK_AUDIO;
		         m_nCurrAudioSampleInIdx1 = outputEntry->dwAudFrameCount;
            QTV_MSG_PRIO5(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                            "searchIDX1Cache Matched Audio Entry: trackid %d dwFlags %d dwOffset %d dwSize %d dwOffsetInIDX1 %d",
                            trid,dwFlags,dwOffset,dwSize,outputEntry->dwOffsetInIDX1);

            QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                          "searchIDX1Cache Matched Audio Entry: dwAudFrameCount %d TS %d nTotalSizeInBytes %d",
                          outputEntry->dwAudFrameCount,outputEntry->nTimeStamp,audioBytesFromStart);

            return retError;
          }//if(tsAudio <= nReposTime)
        }//if( (trackId == trid) && ( (!memcmp(&cType,"wb",sizeof(avi_uint16)))||(bFirstEntry) ) )
      }//for(avi_uint64 itr = offset; itr < (m_nIdx1Offset+m_nIdx1Size) ; )
    }//end of else if( (!bSearchForward)&& bOkToRepos )
	}//if((chunkType == AVI_CHUNK_AUDIO)&&(bOkToRepos))

	if((chunkType == AVI_CHUNK_VIDEO)&&(bOkToRepos))
	{
	  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache seeking video track");
	  memset(outputEntry,0,sizeof(avi_idx1_entry));
      if( (!m_hAviSummary.pIdx1Table->pKeyVideoEntries) ||
          (!m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1) )
      {
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"No Video Entries exist for trackid %d",trid);
        bOkToRepos = false;
	  }
	  if(bSearchForward)
	  {
		//user wants to do FWD
	    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache seeking video track FFW");
		avi_uint64 distanceFromLastEntry  = (avi_uint64)
		m_hAviSummary.pIdx1Table->pKeyVideoEntries[m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1-1].nTimeStamp - nReposTime;
		avi_uint64 distanceFromFirstEntry = nReposTime - (avi_uint64)m_hAviSummary.pIdx1Table->pKeyVideoEntries[0].nTimeStamp;

		if(distanceFromFirstEntry <= distanceFromLastEntry)
		{
		  //we have to start searching in fwd direction from first video frame
		  entryIndex = 0;
		    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache seeking video track searching in fwd dirn from first entry");
		  for(avi_uint32 j = entryIndex; (j < (m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1-1)) && (bOkToRepos);j++)
          {
            memcpy(outputEntry,&(m_hAviSummary.pIdx1Table->pKeyVideoEntries[j]),sizeof(avi_idx1_entry));
            if( (trid == m_hAviSummary.pIdx1Table->pKeyVideoEntries[j].trackId)&&
			    (m_hAviSummary.pIdx1Table->pKeyVideoEntries[j].nTimeStamp >= nReposTime))
            {
		        QTV_MSG_PRIO6(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
			              "searchIDX1Cache matched video entry %d video frame# %d dwOffset %d dwOffsetInIDX1 %d dwSize %d TS %d",
			              j,
						  outputEntry->dwVidFrameCount,
						  outputEntry->dwOffset,
						  outputEntry->dwOffsetInIDX1,
						  outputEntry->dwSize,
						  outputEntry->nTimeStamp);
		       m_nCurrVideoSampleInIdx1 = outputEntry->dwVidFrameCount;
		       retError = AVI_SUCCESS;
		       break;
			}
		  }//for(avi_uint32 j = entryIndex; (j < (m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1-1)) && (bOkToRepos);j++)
      if(retError != AVI_SUCCESS)
      {
        *endOfFileReached = true;
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache end of file reached while finding matched entry");
      }
		}//if(distanceFromFirstEntry <= distanceFromLastEntry)
		else
		{
		  //we have to start searching in the backward dirn from the last entry
		  entryIndex = m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1-1;
		    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache seeking video track searching in backward dirn from last entry");
		  for(int j = entryIndex; (j >= 0) && (bOkToRepos);j--)
          {
            memcpy(outputEntry,&(m_hAviSummary.pIdx1Table->pKeyVideoEntries[j]),sizeof(avi_idx1_entry));
		    if((j == 0)&& (trid == m_hAviSummary.pIdx1Table->pKeyVideoEntries[j].trackId))
			{
			  //This is weird as nReposTime > current playback time, so unless there is some error,
			  //we will never hit this condition.print a message and seek to 0th audio sample.
			        QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
			                  "searchIDX1Cache unexpected condition reached entryIndex %d , nReposTime %d",
							  entryIndex,nReposTime);
		          QTV_MSG_PRIO6(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
			              "searchIDX1Cache matched video entry %d video frame# %d dwOffset %d dwOffsetInIDX1 %d dwSize %d TS %d",
			              j,
						  outputEntry->dwVidFrameCount,
						  outputEntry->dwOffset,
						  outputEntry->dwOffsetInIDX1,
						  outputEntry->dwSize,
						  outputEntry->nTimeStamp);
		       m_nCurrVideoSampleInIdx1 = outputEntry->dwVidFrameCount;
		       retError = AVI_SUCCESS;
		       break;
			}
            else if( (trid == m_hAviSummary.pIdx1Table->pKeyVideoEntries[j].trackId)          &&
			         (m_hAviSummary.pIdx1Table->pKeyVideoEntries[j].nTimeStamp >= nReposTime) &&
					 (m_hAviSummary.pIdx1Table->pKeyVideoEntries[j-1].nTimeStamp < nReposTime)
				   )
            {
		        QTV_MSG_PRIO6(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
			              "searchIDX1Cache matched video entry %d video frame# %d dwOffset %d dwOffsetInIDX1 %d dwSize %d TS %d",
			              j,
						  outputEntry->dwVidFrameCount,
						  outputEntry->dwOffset,
						  outputEntry->dwOffsetInIDX1,
						  outputEntry->dwSize,
						  outputEntry->nTimeStamp);
		       m_nCurrVideoSampleInIdx1 = outputEntry->dwVidFrameCount;
		       retError = AVI_SUCCESS;
		       break;
			}
		  }//for(int j = entryIndex; (j >= 0) && (bOkToRepos);j--)
      if(retError != AVI_SUCCESS)
      {
        *endOfFileReached = true;
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache end of file reached while finding matched entry");
      }
		}//end of else of if(distanceFromFirstEntry <= distanceFromLastEntry)
	  }//if(bSearchForward)
	  else
	  {
		//user wants to do RWD
	    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache seeking video track RW");
		avi_uint64 distanceFromlastEntry = (avi_uint64)
		m_hAviSummary.pIdx1Table->pKeyVideoEntries[m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1-1].nTimeStamp - nReposTime;
		avi_uint64 distanceFromFirstEntry = nReposTime - (avi_uint64)m_hAviSummary.pIdx1Table->pKeyVideoEntries[0].nTimeStamp;

		if(distanceFromlastEntry <= distanceFromFirstEntry)
		{
		  //we have to start searching in backward direction from last video entry
		    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache seeking video track searching from last entry in backward dirn");
		  entryIndex = m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1-1;
		  for(int j = entryIndex; (j >= 0) && (bOkToRepos);j--)
          {
		    if( (trid == m_hAviSummary.pIdx1Table->pKeyVideoEntries[j].trackId)&&
			    (m_hAviSummary.pIdx1Table->pKeyVideoEntries[j].nTimeStamp <= nReposTime))
            {
		      memcpy(outputEntry,&(m_hAviSummary.pIdx1Table->pKeyVideoEntries[j]),sizeof(avi_idx1_entry));
		        QTV_MSG_PRIO6(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
			              "searchIDX1Cache matched video entry %d video frame# %d dwOffset %d dwOffsetInIDX1 %d dwSize %d TS %d",
			              j,
						  outputEntry->dwVidFrameCount,
						  outputEntry->dwOffset,
						  outputEntry->dwOffsetInIDX1,
						  outputEntry->dwSize,
						  outputEntry->nTimeStamp);
		       m_nCurrVideoSampleInIdx1 = outputEntry->dwVidFrameCount;
		       retError = AVI_SUCCESS;
		       break;
			}
		  }//for(int j = entryIndex; (j >= 0) && (bOkToRepos);j--)
		}//if(distanceFromlastEntry <= distanceFromFirstEntry)
		else
		{
			//we have to start searching from the first entry in forward direction
			  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"searchIDX1Cache seeking video track searching from first entry in fwd dirn");
			entryIndex =  0;
			for(avi_uint32 j = entryIndex; (j < (m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1-1)) && (bOkToRepos);j++)
            {
              if( (trid == m_hAviSummary.pIdx1Table->pKeyVideoEntries[j].trackId)          &&
			      (m_hAviSummary.pIdx1Table->pKeyVideoEntries[j].nTimeStamp <= nReposTime) &&
				  (m_hAviSummary.pIdx1Table->pKeyVideoEntries[j+1].nTimeStamp > nReposTime) )
              {
		        memcpy(outputEntry,&(m_hAviSummary.pIdx1Table->pKeyVideoEntries[j]),sizeof(avi_idx1_entry));
		        QTV_MSG_PRIO6(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
			              "searchIDX1Cache matched video entry %d video frame# %d dwOffset %d dwOffsetInIDX1 %d dwSize %d TS %d",
			              j,
						  outputEntry->dwVidFrameCount,
						  outputEntry->dwOffset,
						  outputEntry->dwOffsetInIDX1,
						  outputEntry->dwSize,
						  outputEntry->nTimeStamp);
		         m_nCurrVideoSampleInIdx1 = outputEntry->dwVidFrameCount;
		         retError = AVI_SUCCESS;
		         break;
			  }
			}//for(avi_uint32 j = entryIndex; (j < (m_hAviSummary.pIdx1Table->nKeyVideoyEntriesInIDX1-1)) && (bOkToRepos);j++)
		}//end of else of if(distanceFromlastEntry <= distanceFromFirstEntry)
	  }//end of else of if(bSearchForward)
	}//if((chunkType == AVI_CHUNK_VIDEO)&&(bOkToRepos))
#endif
	return retError;
}
/* =============================================================================
FUNCTION:
 aviParser::seekInIDX1

DESCRIPTION:
 Seek the track identified by given trackid.

INPUT/OUTPUT PARAMETERS:
  trackid          : Identifies the track to be repositioned.
  nReposTime       : Target time to seek to.
  outputEntry      : Fills in idx1 entry information once the seek is complete.
  bSearchForward   : Direction to search in IDX1
  chunkType        : Track type
  nSyncFramesToSkip: Number of sync frames to skip in forward or backward direction.

Please note that nReposTime will be used to carry out SEEK only when nSyncFramesToSkip is 0.

RETURN VALUE:
  AVI_SUCCESS if successful otherwise returns appropriate error code.

SIDE EFFECTS:
  None.
=============================================================================*/
aviErrorType aviParser::seekInIDX1(avi_uint32 trid,
                                   avi_uint64 nReposTime,
                                   avi_idx1_entry* outputEntry,
                                   bool bSearchForward,
                                   CHUNK_t chunkType,
                                   int  nSyncFramesToSkip,
                                   bool* endOfFileReached)
{
  aviErrorType retError = AVI_PARSE_ERROR;
  unsigned char* byteData = m_ReadBuffer;

  avi_uint16 cType = 0;
  avi_uint16 trackId = 0;
  avi_uint32 dwFlags = 0;
  avi_uint64 dwOffset = 0;
  avi_uint32 dwSize = 0;

  avi_uint32 audioChunksFromStart = 0;
  avi_uint64 audioBytesFromStart = 0;
  avi_uint64 tsAudio = 0;

  avi_uint32 videoChunksFromStart = 0;
  avi_uint64 tsVideo = 0;


  avi_uint64 offset = m_nCurrentSampleInfoOffsetInIdx1;
  avi_audiotrack_summary_info ainfo;
  avi_audio_info audinfo;
  avi_video_info vinfo;
  int nVideoSyncSkipped = 0;
  int nAudioSyncSkipped = 0;

  if(chunkType == AVI_CHUNK_AUDIO)
  {
    audioChunksFromStart = m_nCurrAudioFrameCount[trid];
    audioBytesFromStart  = m_nCurrAudioPayloadSize[trid];
  }
  if(chunkType == AVI_CHUNK_VIDEO)
  {
    videoChunksFromStart = m_nCurrVideoFrameCount[trid];
  }

  #ifdef AVI_PARSER_DEBUG
    QTV_MSG_PRIO4(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                 "seekInIDX1 trackid %d nReposTime %d nSyncFramesToSkip %d m_nCurrentSampleInfoOffsetInIdx1 %d",
                 trid,nReposTime,nSyncFramesToSkip,m_nCurrentSampleInfoOffsetInIdx1);
    QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                 "seekInIDX1 m_nCurrAudioSampleInIdx1 %d m_nCurrVideoSampleInIdx1 %d bSearchForward %d",
                 m_nCurrAudioSampleInIdx1,m_nCurrVideoSampleInIdx1,bSearchForward);
    QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                 "seekInIDX1 m_nCurrAudioFrameCount[trid] %d m_nCurrAudioPayloadSize[trid] %d m_nCurrVideoFrameCount[trid] %d",
                 m_nCurrAudioFrameCount[trid],m_nCurrAudioPayloadSize[trid],m_nCurrVideoFrameCount[trid]);
  #endif

  //Make sure Parser is in valid state to carry out Seek.
  if(m_CurrentParserState != AVI_PARSER_SEEK)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                 "seekInIDX1 called when parser is not in AVI_PARSER_SEEK!!");
    return AVI_FAILURE;
  }
  if(!outputEntry)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "seekInIDX1 Failure!!");
    return AVI_FAILURE;
  }
  memset(outputEntry,0,sizeof(avi_idx1_entry));

  /*
  * bSearchForward is based on Current playback time and Reposition time.
  * Thus, is valid only when nSyncFramesToSkip is 0.
  * When nSyncFramesToSkip > 0 and nReposTime is 0,we need skip in forward direction.
  */
  if( (bSearchForward && (nSyncFramesToSkip == 0)) ||
      ((nSyncFramesToSkip > 0)&&(nReposTime == 0)) )
  {
    /*
    * In case of VBR audio, we keep record of the end time stamp of
    * the sample being consumed.
    * That time stamp becomes the start time of the sample pointed by idx1 entry
    * in following iteration.
    */
    bool bRetrieveEndTSFromSampleConsumed = true;
    for(avi_uint64 itr = offset; itr < (m_nIdx1Offset+m_nIdx1Size) ; )
    {
      bool bAdjustVideo = false;
      bool bAdjustAudio = false;

      if(!AVICallbakGetData(offset,4 * sizeof(avi_uint32),m_ReadBuffer,AVI_READ_BUFFER_SIZE,m_pUserData))
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                     "seekInIDX1 AVI_READ_FAILURE");
        retError = AVI_READ_FAILURE;
        return retError;
      }
      offset += (4 *sizeof(avi_uint32));

      memcpy(&trackId,byteData,sizeof(avi_uint16));
      trackId = ascii_2_short_int(&trackId);
      memcpy(&cType,byteData+2,sizeof(avi_uint16));
      memcpy(&dwFlags,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
      memcpy(&dwOffset,byteData+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
      if(m_bByteAdjustedForMOVI)
      {
        dwOffset = (m_nStartOfMovi + dwOffset - m_nBytesToBeAdjustedForMOVI);
      }
      memcpy(&dwSize,byteData+(3*sizeof(avi_uint32)),sizeof(avi_uint32));

      if(  ( (!memcmp(&cType,"dc",sizeof(avi_uint16))) ||(!memcmp(&cType,"db",sizeof(avi_uint16))) ) &&
           (trackId == trid) )
      {
        if( (GetVideoInfo(trid,&vinfo)==AVI_SUCCESS) && (vinfo.strhVideo.dwRate) )
        {
          bAdjustVideo = true;
          tsVideo =    (avi_uint64)( ( (float)vinfo.strhVideo.dwScale/(float)vinfo.strhVideo.dwRate)*
                                     (float)videoChunksFromStart*1000.0f);

          #ifdef AVI_PARSER_DEBUG
            QTV_MSG_PRIO4(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"seekInIDX1 VIDEO chunk# %d TS %d Size %d dwFlags %u",
                          videoChunksFromStart,tsVideo,dwSize,dwFlags);
          #endif

          if(dwFlags & AVI_KEY_FRAME_MASK)
          {
            nVideoSyncSkipped++;
            #ifdef AVI_PARSER_DEBUG
              QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"seekInIDX1 nVideoSyncSkipped %ld", nVideoSyncSkipped);
            #endif
            m_nCurrVideoSampleInIdx1 = videoChunksFromStart;
            outputEntry->trackId = trid;
            outputEntry->dwFlags = dwFlags;
            outputEntry->dwOffset = dwOffset;
            outputEntry->dwSize = dwSize;
            /*
            * OSCL Cache works in fordward direction.
            * Thus, after consuming an entry, 'offset' is
            * pointing to an entry which will be consumed in next iteration.
            * We need to subtract size of(idx1 entry) from 'offset' to
            * pick the one which is consumed.
            */
            outputEntry->dwOffsetInIDX1 = (offset - (4 *sizeof(avi_uint32)));
            outputEntry->dwVidFrameCount = videoChunksFromStart;
            outputEntry->nTimeStamp = (float)tsVideo;
            outputEntry->chunkType = AVI_CHUNK_VIDEO;

            if( (tsVideo >= nReposTime)                                            ||
                ((nVideoSyncSkipped == nSyncFramesToSkip)&&(nSyncFramesToSkip > 0))   )
            {
              /*
              * Located the correct video entry/chunk.
              * Load IDX1 entry values into output idx1 entry and return SUCCESS;
              */
              retError = AVI_SUCCESS;

              QTV_MSG_PRIO5(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
              "seekInIDX1 Matched Video Entry: trackid %d dwFlags %d dwOffset %d dwSize %d dwOffsetInIDX1 %d",
              trid,dwFlags,dwOffset,dwSize,outputEntry->dwOffsetInIDX1);

              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
              "seekInIDX1 Matched Video Entry: dwVidFrameCount %d TS %d",
              outputEntry->dwVidFrameCount,outputEntry->nTimeStamp);

              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                            "seekInIDX1 nVideoSyncSkipped %d nSyncFramesToSkip %d",
                            nVideoSyncSkipped,nSyncFramesToSkip);

              return retError;
            }//if(tsVideo >= nReposTime)
          }//if(dwFlags & AVI_KEY_FRAME_MASK)
        }//if( (GetVideoInfo(trid,&vinfo)==AVI_SUCCESS) && (vinfo.strhVideo.dwRate) )
        else
        {
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                        "seekInIDX1 GetVideoInfo failed for trackid %d",trid);
          return AVI_PARSE_ERROR;
        }
      }//if video chunk
      else if( (!memcmp(&cType,"wb",sizeof(avi_uint16))) && (trackId == trid) )
      {
        if( (GetAudioTrackSummaryInfo(trackId,&ainfo)==AVI_SUCCESS) &&
            (GetAudioInfo(trackId,&audinfo)==AVI_SUCCESS) )
        {
          bAdjustAudio = true;
          nAudioSyncSkipped++;
          #ifdef AVI_PARSER_DEBUG
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"seekInIDX1 nAudioSyncSkipped %ld", nAudioSyncSkipped);
          #endif

          if( (!ainfo.isVbr)&&(ainfo.audioBytesPerSec) )
          {
            tsAudio = (avi_uint32)((float)audioBytesFromStart / (float)ainfo.audioBytesPerSec * 1000.0f);
            #ifdef AVI_PARSER_DEBUG
              QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                            "seekInIDX1 AUDIO TRACK IS CBR TS %d Chunk# %d Size %d",
                            tsAudio,audioChunksFromStart,dwSize);
            #endif
          }
          else if( ainfo.isVbr && ainfo.audioFrequency )
          {
            if(audinfo.strhAudio.dwSampleSize > 0)
            {
            if(bRetrieveEndTSFromSampleConsumed)
            {
              //Retrieve the end time timestamp of the sample consumed during playback
              //which is same as start time stamp of the sample pointed by idx1 entry
              //read above
              tsAudio = m_nParserAudSampleEndTime[trid];
              bRetrieveEndTSFromSampleConsumed = false;
            }
            }
            else
            {
              tsAudio = (avi_uint64)(( ( (float)audinfo.strhAudio.dwScale/
                                      (float)audinfo.strhAudio.dwRate) * 1000.f) * (float)audioChunksFromStart );
            }
            if(audinfo.strhAudio.dwSampleSize > 0)
            {
              if(audinfo.strfAudio.nBlockAlign && audinfo.strhAudio.dwRate)
              {
                double val1 = ceil( ( (double)dwSize / audinfo.strfAudio.nBlockAlign) );
                double val2 =  ((double)audinfo.strhAudio.dwScale/audinfo.strhAudio.dwRate)* 1000.f;
                tsAudio += (uint32)(val1 * val2);
              }
            }
            #ifdef AVI_PARSER_DEBUG
              QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                            "seekInIDX1 AUDIO TRACK IS VBR TS %d Chunk# %d Size %d",
                            tsAudio,audioChunksFromStart,dwSize);
            #endif
          }
          m_nCurrAudioSampleInIdx1 = audioChunksFromStart;
          outputEntry->trackId = trid;
          outputEntry->dwFlags = dwFlags;
          outputEntry->dwOffset = dwOffset;
          outputEntry->dwSize = dwSize;
          /*
          * OSCL Cache works in fordward direction.
          * Thus, after consuming an entry, 'offset' is
          * pointing to an entry which will be consumed in next iteration.
          * We need to subtract size of(idx1 entry) from 'offset' to
          * pick the one which is consumed.
          */
          outputEntry->dwOffsetInIDX1 = (offset - (4 *sizeof(avi_uint32)));
          outputEntry->dwAudFrameCount = audioChunksFromStart;
          outputEntry->nTimeStamp = (float)tsAudio;
          outputEntry->nTotalSizeInBytes = audioBytesFromStart;
          outputEntry->chunkType = AVI_CHUNK_AUDIO;

          if( (tsAudio >= nReposTime) ||
              ((nAudioSyncSkipped == nSyncFramesToSkip)&&(nSyncFramesToSkip > 0))   )
          {
            /*
            * Located the correct audio entry/chunk.
            * Load IDX1 entry values into output idx1 entry and return SUCCESS;
            */

            retError = AVI_SUCCESS;

            QTV_MSG_PRIO5(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
            "seekInIDX1 Matched Audio Entry: trackid %d dwFlags %d dwOffset %d dwSize %d dwOffsetInIDX1 %d",
            trid,dwFlags,dwOffset,dwSize,outputEntry->dwOffsetInIDX1);

            QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
            "seekInIDX1 Matched Audio Entry: dwAudFrameCount %d TS %d nTotalSizeInBytes %d",
            outputEntry->dwAudFrameCount,outputEntry->nTimeStamp,audioBytesFromStart);

            QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                          "seekInIDX1 nAudioSyncSkipped %d nSyncFramesToSkip %d",
                          nAudioSyncSkipped,nSyncFramesToSkip);
            return retError;
          }
        }
        else
        {
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                        "seekInIDX1 GetAudioTrackSummaryInfo failed for trackid %d ",trid);
          return AVI_PARSE_ERROR;
        }
      }
      if(bAdjustVideo)
      {
        videoChunksFromStart++;
      }
      if(bAdjustAudio)
      {
        audioChunksFromStart++;
        audioBytesFromStart += dwSize;
      }
      itr+=(4 *sizeof(avi_uint32));
    }//for(avi_uint64 itr = offset; itr < (m_nIdx1Offset+m_nIdx1Size) ; )
    *endOfFileReached = true;
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"seekInIDX1 end of file reached while finding matched entry");
  }
  else if( ((bSearchForward == false)&&(nSyncFramesToSkip == 0)) ||
           ((nSyncFramesToSkip < 0)&&(nReposTime == 0)) )
  {
    /*
    * bSearchForward is based on Current playback time and Reposition time.
    * Thus, is valid only when nSyncFramesToSkip is 0.
    * When nSyncFramesToSkip < 0 and nReposTime is 0,we need skip in backward direction.
    */

    /*
    * m_nCurrentSampleInfoOffsetInIdx1 incremented once the
    * sample info/sample is retrieved to point to the next IDX1 entry.
    * Since we read in backward direction when doing rewind,we need to
    * adjust video/audio frame count.
    * The first TS that we will encounter below while doing rewind for audio/video,
    * should be the one that parser has already spitted out.
    * So go to previous entry to match with current audio/video frame count.
    */
    offset -= (4 *sizeof(avi_uint32));
    #ifdef AVI_PARSER_DEBUG
      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                    "bInitialBackWardDirection is TRUE, old offset %ld updated offset %ld",
                    (offset+(4 *sizeof(avi_uint32))),offset);
    #endif

    bool bRetrieveEndTSFromSampleConsumed = true;
    for(avi_uint64 itr = offset; itr >= m_nIdx1Offset ; )
    {
      if(!readFromIdx1SeekCache(offset,4 * sizeof(avi_uint32),AVI_READ_BUFFER_SIZE,m_ReadBuffer))
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                     "seekInIDX1 AVI_READ_FAILURE");
        retError = AVI_READ_FAILURE;
        flushIdx1SeekCache();
        return retError;
      }
      offset -= (4 *sizeof(avi_uint32));

      memcpy(&trackId,byteData,sizeof(avi_uint16));
      trackId = ascii_2_short_int(&trackId);
      memcpy(&cType,byteData+2,sizeof(avi_uint16));
      memcpy(&dwFlags,byteData+sizeof(avi_uint32),sizeof(avi_uint32));
      memcpy(&dwOffset,byteData+(2*sizeof(avi_uint32)),sizeof(avi_uint32));
      if(m_bByteAdjustedForMOVI)
      {
        dwOffset = (m_nStartOfMovi + dwOffset - m_nBytesToBeAdjustedForMOVI);
      }
      memcpy(&dwSize,byteData+(3*sizeof(avi_uint32)),sizeof(avi_uint32));

      if(  ( (!memcmp(&cType,"dc",sizeof(avi_uint16))) ||(!memcmp(&cType,"db",sizeof(avi_uint16))) ) &&
           (trackId == trid) )
      {
        if(videoChunksFromStart)
        {
          /*As we go backwards in RW, we should decrement videoChunksFromStart since it is already incremented
          in GetNextSampleInfo during playback.*/
          videoChunksFromStart--;
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"SeekInIDX1 videoChunksFromStart is 0 ");
        }
        if( (GetVideoInfo(trid,&vinfo)==AVI_SUCCESS) && (vinfo.strhVideo.dwRate) )
        {
          tsVideo =    (avi_uint64)( ( (float)vinfo.strhVideo.dwScale/(float)vinfo.strhVideo.dwRate)*
                                     (float)videoChunksFromStart*1000.0f);

          #ifdef AVI_PARSER_DEBUG
            QTV_MSG_PRIO4(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"seekInIDX1 VIDEO chunk# %d TS %d Size %d dwFlags %u",
                          videoChunksFromStart,tsVideo,dwSize,dwFlags);
          #endif

          /*
          * For some clips, first video frame is not marked as KEY Frame.
          * So, don't check for any flag if it's a first video frame.
          */
          if( (dwFlags & AVI_KEY_FRAME_MASK)||(videoChunksFromStart == 0) )
          {
            nVideoSyncSkipped--;
            #ifdef AVI_PARSER_DEBUG
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"seekInIDX1 nVideoSyncSkipped %ld TS %ld", nVideoSyncSkipped,tsVideo);
            #endif

            if( (tsVideo <= nReposTime)                                            ||
                ((nVideoSyncSkipped == nSyncFramesToSkip)&&(nSyncFramesToSkip < 0))   )
            {
              flushIdx1SeekCache();
              m_nCurrVideoSampleInIdx1 = videoChunksFromStart;

              /*
              * Located the correct video entry/chunk.
              * Load IDX1 entry values into output idx1 entry and return SUCCESS;
              */
              outputEntry->trackId = trid;
              outputEntry->dwFlags = dwFlags;
              outputEntry->dwOffset = dwOffset;
              outputEntry->dwSize = dwSize;

              /*
              * Parser Cache works in backward direction when doing REWIND.
              * Thus, after consuming an entry, 'offset' is
              * pointing to an entry which will be consumed in next iteration as
              * opposed to FORWARD case where we need to subtract
              * size of(idx1 entry) from 'offset' to pick the one which is consumed.
              * Here we need to increment it.
              */
              outputEntry->dwOffsetInIDX1 = (offset + (4 *sizeof(avi_uint32)) );

              outputEntry->dwVidFrameCount = videoChunksFromStart;
              outputEntry->nTimeStamp = (float)tsVideo;
              outputEntry->chunkType = AVI_CHUNK_VIDEO;
              retError = AVI_SUCCESS;

              QTV_MSG_PRIO5(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
              "seekInIDX1 Matched Video Entry: trackid %d dwFlags %d dwOffset %d dwSize %d dwOffsetInIDX1 %d",
              trid,dwFlags,dwOffset,dwSize,outputEntry->dwOffsetInIDX1);

              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
              "seekInIDX1 Matched Video Entry: dwVidFrameCount %d TS %d",
              outputEntry->dwVidFrameCount,outputEntry->nTimeStamp);
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                            "seekInIDX1 nVideoSyncSkipped %d nSyncFramesToSkip %d",
                            nVideoSyncSkipped,nSyncFramesToSkip);

              return retError;
            }//if(tsVideo >= nReposTime)
          }//if(dwFlags & AVI_KEY_FRAME_MASK)
        }//if( (GetVideoInfo(trid,&vinfo)==AVI_SUCCESS) && (vinfo.strhVideo.dwRate) )
        else
        {
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                        "seekInIDX1 GetVideoInfo failed for trackid %d",trid);
          return AVI_PARSE_ERROR;
        }
      }//if video chunk
      else if( (!memcmp(&cType,"wb",sizeof(avi_uint16))) && (trackId == trid) )
      {
        /*As we go backwards in RW, we should decrement audioBytesFromStart and audioChunksFromStart
        since it is already incremented in GetNextSampleInfo during playback.*/
        if(audioBytesFromStart >= dwSize)
        {
          audioBytesFromStart -= dwSize;
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"SeekInIDX1 audioBytesFromStart < dwSize ");
        }
        if(audioChunksFromStart)
        {
          audioChunksFromStart--;
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"SeekInIDX1 audioChunksFromStart is 0 ");
        }
        if( (GetAudioTrackSummaryInfo(trackId,&ainfo)==AVI_SUCCESS) &&
            (GetAudioInfo(trackId,&audinfo) == AVI_SUCCESS) )
        {
          nAudioSyncSkipped--;
          #ifdef AVI_PARSER_DEBUG
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,"seekInIDX1 nAudioSyncSkipped %ld", nAudioSyncSkipped);
          #endif

          if( (!ainfo.isVbr)&&(ainfo.audioBytesPerSec) )
          {
            tsAudio = (avi_uint32)((float)audioBytesFromStart / (float)ainfo.audioBytesPerSec * 1000.0f);
            #ifdef AVI_PARSER_DEBUG
              QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                            "seekInIDX1 AUDIO TRACK IS CBR TS %d Chunk# %d Size %d",
                            tsAudio,audioChunksFromStart,dwSize);
            #endif
          }
          else if( ainfo.isVbr && ainfo.audioFrequency )
          {
            if(audinfo.strhAudio.dwSampleSize > 0)
            {
            if(bRetrieveEndTSFromSampleConsumed)
            {
              //Get the end time stamp of the sample that corresponds
              //to idx1 entry read above
              tsAudio = m_nParserAudSampleEndTime[trid];
              bRetrieveEndTSFromSampleConsumed = false;
            }
            //Now subtract duration based on current sample's size
            //to get this sample's start time stamp
              if(audinfo.strfAudio.nBlockAlign && audinfo.strhAudio.dwRate)
            {
                double val1 = ceil( ( (double)dwSize / audinfo.strfAudio.nBlockAlign) );
                double val2 =  ((double)audinfo.strhAudio.dwScale/audinfo.strhAudio.dwRate)* 1000.f;
                uint32 sampleduration =  (uint32)(val1 * val2);
                if(tsAudio > sampleduration)
                {
                  tsAudio -= sampleduration;
                }
              }
            }
            else
            {
              tsAudio = (avi_uint64)(( ( (float)audinfo.strhAudio.dwScale/
                                       (float)audinfo.strhAudio.dwRate) * 1000.f) * (float)audioChunksFromStart );
            }
            QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                            "seekInIDX1 AUDIO TRACK IS VBR TS %d Chunk# %d Size %d",
                            tsAudio,audioChunksFromStart,dwSize);
          }
          if( (tsAudio <= nReposTime)                                            ||
              ((nAudioSyncSkipped == nSyncFramesToSkip)&&(nSyncFramesToSkip < 0))   )
          {
            m_nCurrAudioSampleInIdx1 = audioChunksFromStart;
            flushIdx1SeekCache();
            /*
            * Located the correct audio entry/chunk.
            * Load IDX1 entry values into output idx1 entry and return SUCCESS;
            */
            outputEntry->trackId = trid;
            outputEntry->dwFlags = dwFlags;
            outputEntry->dwOffset = dwOffset;
            outputEntry->dwSize = dwSize;

            /*
            * Parser Cache works in backward direction when doing REWIND.
            * Thus, after consuming an entry, 'offset' is
            * pointing to an entry which will be consumed in next iteration as
            * opposed to FORWARD case where we need to subtract
            * size of(idx1 entry) from 'offset' to pick the one which is consumed.
            * Here we need to increment it.
            */
            outputEntry->dwOffsetInIDX1 = (offset + (4 *sizeof(avi_uint32)) );

            outputEntry->dwAudFrameCount = audioChunksFromStart;
            outputEntry->nTimeStamp = (float)tsAudio;
            outputEntry->nTotalSizeInBytes = audioBytesFromStart;

            outputEntry->chunkType = AVI_CHUNK_AUDIO;
            retError = AVI_SUCCESS;

            QTV_MSG_PRIO5(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
            "seekInIDX1 Matched Audio Entry: trackid %d dwFlags %d dwOffset %d dwSize %d dwOffsetInIDX1 %d",
            trid,dwFlags,dwOffset,dwSize,outputEntry->dwOffsetInIDX1);

            QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
            "seekInIDX1 Matched Audio Entry: dwAudFrameCount %d TS %d nTotalSizeInBytes %d",
            outputEntry->dwAudFrameCount,outputEntry->nTimeStamp,audioBytesFromStart);
            QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                          "seekInIDX1 nAudioSyncSkipped %d nSyncFramesToSkip %d",
                          nAudioSyncSkipped,nSyncFramesToSkip);

            return retError;
          }
        }
        else
        {
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                        "seekInIDX1 GetAudioTrackSummaryInfo failed for trackid %d ",trid);
          return AVI_PARSE_ERROR;
        }
      }
      itr-=(4 *sizeof(avi_uint32));
    }//for(avi_uint64 itr = offset; itr < (m_nIdx1Offset+m_nIdx1Size) ; )
    flushIdx1SeekCache();
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,"seekInIDX1 WEIRD CASE...PLEASE CHECK..");
    QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "bSearchForward %ld nSyncFramesToSkip %ld nReposTime %ld",
                  bSearchForward,nSyncFramesToSkip,nReposTime);
    return AVI_FAILURE;
  }
  return retError;
}
#endif//FEATURE_QTV_AVI
