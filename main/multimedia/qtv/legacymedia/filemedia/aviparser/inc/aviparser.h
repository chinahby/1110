#ifndef AVI_PARSER_H
#define AVI_PARSER_H

/* =======================================================================
                              aviParser.h
DESCRIPTION

Copyright 2004 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             PERFORCE HEADER
$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/aviparser/main/latest/inc/aviparser.h#22 $
========================================================================== */
#include "customer.h"

#ifdef FEATURE_QTV_AVI

#include "aviHeaders.h"
#include "aviErrors.h"
#include "qtvsystem.h"
#include "qtv_msg.h"

/*
* Enabling following feature will print lots of debug messages from
* AVI parser APIs.
*/
//#define AVI_PARSER_DEBUG

/*
* When this feature is defined, parser will not load the indexing information during intial parsing.
* Thus, playback will start as quickly as possible.
* 
* Undefining this feature will make parser cache in entire idx1 and prepare key-frame table for video
* and indexing table for all audio streams. This will delay the intial parsing and will also
* consume additional memory. Size of memory depends on size of idx1.
*/
#define AVI_PARSER_FAST_START_UP

//Debug purpose
#ifdef  AVI_PARSER_FAST_START_UP
#define AVI_PARSER_SEEK_SANITY_TEST
#endif
/*
* When defined, all index tables will be printed while parsing.
* Can be used for debugging when indexing/repositioning has a bug.
*/
#ifndef AVI_PARSER_FAST_START_UP
//#define WALK_INDEX_TABLES
#endif

/*
*Callback function used by parser for reading the file data.
*Parser does not implement this function, it's the responsibility
*of the APP to implement it.
*/
extern avi_int32 AVICallbakGetData (avi_int32 nOffset,
                                    avi_int32 nNumBytesRequest,
                                    unsigned char* pData,
                                    avi_int32  nMaxSize,
                                    avi_uint32  u32UserData );
/*
*aviParser class.
*/
class aviParser
{
  /*=======================================================================
  * Public Members                                                        *
  * ======================================================================*/

  public:
  //aviParser APIs
                         aviParser(avi_uint32 pUData,
                                   avi_uint64 fsize,
                                   bool bDiscardAudIndex = false,
                                   bool bDiscardVidIndex = false);

  virtual                ~aviParser();
  virtual avi_uint32     GetTotalNumberOfTracks(void){return m_hAviSummary.n_streams;}
  virtual avi_uint32     GetTotalNumberOfAudioTracks(void){return m_hAviSummary.n_audio_tracks;}
  virtual avi_uint32     GetTotalNumberOfVideoTracks(void){return m_hAviSummary.n_video_tracks;}

  virtual aviErrorType   StartParsing(void);
  virtual aviErrorType   GetNumOfRiff(avi_uint64 nOffset, int* m_nNumOfRiff);
  virtual aviErrorType   GetAVIHeader(avi_mainheader_avih* pAviHdrPtr);
  virtual aviErrorType   GetAudioInfo(avi_uint8 trackId,avi_audio_info* pAudioInfo);
  virtual aviErrorType   GetVideoInfo(avi_uint8 trackId,avi_video_info* pVideoInfo);
  virtual avi_uint64     GetTrackDuration(avi_uint8 trackId);
  virtual avi_uint64     GetClipDurationInMsec();

  virtual avi_uint8*     GetAVIVolHeader(avi_uint8   trackId);
  virtual avi_uint32     GetAVIVolHeaderSize(avi_uint8 trackId);

  virtual aviErrorType   GetCurrentSample(avi_uint8 trackId,
                                          avi_uint8* dataBuffer,
                                          avi_uint32 nMaxBufSize,
                                          avi_uint32 nBytesNeeded);

  virtual aviErrorType   GetNextSampleInfo(avi_uint8 trackId,
                                           avi_sample_info* sampleInfo,                                           
                                           avi_uint32 nMaxBufSize,
                                           avi_uint16* trackIdFound);

  virtual aviErrorType   GetTrackChunkType(avi_uint8 trackId,CHUNK_t* type);
  virtual aviErrorType   GetAudioTrackSummaryInfo(avi_uint8 trackId,
                                                  avi_audiotrack_summary_info*);
  virtual avi_uint32     GetTrackWholeIDList(avi_uint32*);
  virtual aviErrorType   Seek(avi_uint8,
                              avi_uint64,
                              avi_uint32,
                              avi_sample_info*,
                              bool canSyncToNonKeyFrame=false,                              
                              int  nSyncFramesToSkip = 0);
  virtual void           SetIDX1Cache(void*);
  virtual void*          GetIDX1Cache();

  virtual avi_uint8*     GetDRMInfo(int*);
  virtual bool           IsDRMProtection(){return m_bDRMProtection;};
  bool                   getDataFromINFO(char*, char*, avi_uint16*);
  
  #ifdef WALK_INDEX_TABLES
    virtual void         WalkIDX1Table();
  #endif

  /*=======================================================================
  * Private Members                                                       *
  * ======================================================================*/

  private:
  //Private members/variables used by parser.

  //User callback data
  avi_uint32       m_pUserData;

  //This is the offset in 'movi' used for retrieval of audio/video samples.
  avi_uint64       m_nCurrOffset;

  //Current audio/video sample size to be retrieved.
  avi_uint64       m_nCurrentChunkDataSize;

  //Start/size of 'movi' chunk
  avi_uint64       m_nMoviOffset;
  avi_uint64       m_nStartOfMovi;
  avi_uint64       m_nMoviSize;

  int              m_nNumOfRiff;
  bool             m_bisAVIXpresent;
  avi_riff_info*   m_pMultipleRiff;

  //Start/size of 'idx1' chunk
  avi_uint64       m_nIdx1Offset;
  avi_uint64       m_nIdx1Size;

  //Start of MOVI and IDX1 Offset in case of JUNK at the beginning
  avi_uint64       m_nAdjustedMoviOffset;
  avi_uint64       m_nAdjustedIdx1Offset;

  //Total file size
  avi_uint64       m_nFileSize;

  //Current parser state
  aviParserState   m_CurrentParserState;
  avi_uint8        m_ReadBuffer[AVI_READ_BUFFER_SIZE];


  //Points to the current sample being retrieved in 'idx1' chunk.
  //Used to seek backward/forward. This eliminates the need to parse
  //IDX1 at the begining during reposition.
  avi_uint64       m_nCurrentSampleInfoOffsetInIdx1;
  avi_uint32       m_nCurrAudioSampleInIdx1;
  avi_uint32       m_nCurrVideoSampleInIdx1;

  avi_summary_info m_hAviSummary;
  avi_uint32       m_nCurrVideoFrameCount[AVI_MAX_VIDEO_TRACKS];
  avi_uint32       m_nCurrAudioPayloadSize[AVI_MAX_AUDIO_TRACKS];
  avi_uint32       m_nCurrAudioFrameCount[AVI_MAX_AUDIO_TRACKS];
  avi_uint64       m_nParserAudSampleEndTime[AVI_MAX_AUDIO_TRACKS];

  bool             m_bDiscardAudioIndex;
  bool             m_bDiscardVideoIndex;
  bool             m_bDRMProtection;
  bool             m_bByteAdjustedForMOVI;
  avi_info_struct  m_AviClipMetaInfo;
  avi_uint32       m_nBytesToBeAdjustedForMOVI;
  avi_uint16       m_VolSize;

  //Stores the start offset for each indx chunk.
  avi_uint32       m_start_data_offset_base_indx_tbl[AVI_MAX_TRACKS];

  //Stores AVI Base Index('indx'), if available.
  avi_indx_tbl     m_base_indx_tbl[AVI_MAX_TRACKS];

  memory_struct*   m_AviVOLHeader[AVI_MAX_TRACKS];

  avi_parser_seek_buffer_cache* m_pIdx1SeekCache;

  avi_header_strd  m_AviSTRDHeader;

  /*
  * AVI_INDEX_OF_CHUNKS table can have
  * an AVI Standard Index Chunks or AVI Field Index Chunk('ix##')
  * with IndexType Set to AVI_INDEX_OF_CHUNKS and IndexSubType set to
  * 0x0 or AVI_INDEX_2FIELD.
  */
  avi_uint32       m_start_data_offset_avi_index_of_chunks_tbl[AVI_MAX_TRACKS];
  avi_std_indx_tbl m_avi_index_of_chunks_tbl[AVI_MAX_TRACKS];

  //Private functions for parsing various AVI headers.
  aviErrorType     parseAVIH(avi_uint64*);
  aviErrorType     parseSTRL(avi_uint64*,avi_uint32);
  aviErrorType     parseVideoSTRH(avi_uint64*,avi_uint64,avi_video_info*);
  aviErrorType     parseAudioSTRH(avi_uint64*,avi_uint64);
  aviErrorType     parseAudioSTRF(avi_uint64*);
  aviErrorType     parseVideoSTRF(avi_uint64*,avi_video_info*);
  aviErrorType     parseVideoSTRN(avi_uint64*,avi_video_info*);
  aviErrorType     parseAudioSTRN(avi_uint64*);
  aviErrorType     parseODML(avi_uint64);
  aviErrorType     parseJUNK(avi_uint64*);
  aviErrorType     parseMOVI(avi_uint64);
  aviErrorType     parseHDRL(avi_uint64*);
  aviErrorType     parseINFO(avi_uint64 offset,int);
  avi_uint64       skipToNextValidMediaChunk(avi_uint64);
  aviErrorType     parseIDX1(avi_uint64*);
  aviErrorType     cacheIDX1(avi_uint64*,avi_uint32);
  aviErrorType     updateSampleRetrievedOffset(CHUNK_t,avi_uint8);
  aviErrorType     parseINDX(avi_uint64,fourCC_t);
  aviErrorType     parseIX(avi_uint64,int*);
  aviErrorType     getNextFourCC(avi_uint64,avi_uint32*);
  aviErrorType     updateInfoChunkInfo(fourCC_t,avi_uint32,int);
  avi_info_chunk*  getInfoChunkHandle(fourCC_t);
  aviErrorType     parseSTRD(avi_uint64*);

aviErrorType       searchIDX1Cache(avi_uint32,
                                   avi_uint64,
                                   avi_idx1_entry*,
                                   bool,
                                   CHUNK_t,
                                   int nSyncFramesToSkip,
                                   bool* endOfFileReached);

  bool             isVOLReadDone(avi_uint8,int,avi_int8*,bool*,avi_uint8* membuf=NULL);

  aviErrorType     seekInIDX1(avi_uint32,
                              avi_uint64,
                              avi_idx1_entry*,
                              bool,
                              CHUNK_t,
                              int nSyncFramesToSkip,
                              bool* endOfFileReached);

  avi_uint32       readFromIdx1SeekCache(avi_uint64 nOffset, 
                                         avi_int32 nNumBytesRequest,
                                         avi_int32 nMaxSize,
                                         unsigned char *ppData);

  void             flushIdx1SeekCache(void);
  bool             doSanityCheckBeforeSeek(avi_uint8,CHUNK_t,aviParserState);
  avi_int64        getCurrentPlaybackTime(avi_uint8);
avi_uint32         isCurrentFrameEncrypted(avi_uint8/*trackid*/,
                                           avi_uint64/*current offset in idx1*/,
                                           avi_uint64/*current offset in MOVI*/);
};
#endif//#ifdef FEATURE_QTV_AVI

#endif
