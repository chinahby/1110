#ifndef __AVIFile_H__
#define __AVIFile_H__
/* =======================================================================
                              avifile.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.

Copyright 2004 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             PERFORCE HEADER

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/aviparser/main/latest/inc/avifile.h#22 $
========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */
#ifdef FEATURE_QTV_AVI
/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

#include "oscl_file_io.h"
#include "qcplayer_oscl_utils.h"
#include "QCUtils.h"
#include "filebase.h"

#include "aviHeaders.h"
#include "aviErrors.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

//Need to update this value based on MOBILE PROFILE
#define AVI_STREAM_TIME_SCALE 1000      // AVI Code has Milli Sec interface

#define AVI_MAX_VIDEO_FRAMES_READ_AHEAD 4
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
class Mpeg4Player;
class aviParser;
/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_MPEG4_B_FRAMES
typedef struct video_sample
{
  AVI_VOP_TYPE vop_type;
  uint8 bVopCount;
  uint32 size;
  file_sample_info_type   m_sampleInfo;
  uint32 buff_Size;
  uint8 * buff;
}avi_video_sample;

/* This is available only for AVI MPEG4 */
typedef struct read_ahead_buffer
{
  bool                   firstVideoFrame;  // To know if we are reading the first frame  
  uint8                  currentSampleIndex; // current frame that we need to send 
  uint8                  validSampleIndex;  // index to read the next video frame from file 
  bool                   allocatedForReadAhead;  // TO allocate only once the read ahead buffer to hold frames
}avi_read_ahead_buffer;
#endif /* FEATURE_MPEG4_B_FRAMES */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Function Declaration
** ======================================================================= */

/* =======================================================================
**                        Class Declarations
** ======================================================================= */

/* ======================================================================
CLASS
  AVIFile

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
class AVIFile : public FileBase, public Parentable
{

public:
  
  AVIFile(); 
  AVIFile(  const OSCL_STRING &filename
            ,Mpeg4Player *pMpeg4Player
            ,unsigned char *pFileBuf = NULL
            ,uint32 bufSize = 0            
            ,bool bPlayVideo = true
            ,bool bPlayAudio = true            
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
            ,bool bHttpStream = false
            ,uint32 wBufferOffset = 0
            ,uint32 wStartupTime = 0
            ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize = NULL
            ,QtvPlayer::FetchBufferedDataT FetchBufferedData = NULL
            ,QtvPlayer::InstanceHandleT handle = NULL
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

	       ); // Constructor

  virtual ~AVIFile();

  virtual bool isDivxFile(void) {return false;};
  virtual bool isAviFile(void) {return true;};
  virtual bool isasffile(void) {return false;};

  virtual int32 getNextMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 &index);
  virtual uint32 getMediaTimestampForCurrentSample(uint32 id);
  virtual uint32 getMediaTimestampDeltaForCurrentSample(uint32 id);

  virtual void resetPlayback();
  virtual uint32 resetPlayback(uint32 repos_time, uint32 id, bool bSetToSyncSample,
                               bool *bError, uint32 currentPosTimeStamp);
  virtual bool resetMediaPlayback(uint32);  // reset playback for a particular track //

#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
  virtual uint32 skipNSyncSamples(int , uint32 , bool* , uint32 );
#endif /* FEATURE_QTV_REPOSITION_SYNC_FRAME */

  virtual uint8 randomAccessDenied();

  virtual OSCL_STRING getTitle() const;
  virtual OSCL_STRING getAuthor() const;
  virtual OSCL_STRING getDescription() const;
  virtual OSCL_STRING getRating() const;
  virtual OSCL_STRING getCopyright() const;
  virtual OSCL_STRING getVersion() const;
  virtual OSCL_STRING getCreationDate() const;

  virtual OSCL_STRING getPerf()const{return 0;}; //not available.
  virtual OSCL_STRING getGenre()const{return 0;};//not available.
  virtual OSCL_STRING getClsf()const{return 0;};//not available.
  virtual OSCL_STRING getKywd()const{return 0;};//not available.
  virtual OSCL_STRING getLoci()const{return 0;};//not available.
  virtual OSCL_STRING getAlbum()const{return 0;};//not available.
  virtual OSCL_STRING getYrrc()const{return 0;};//not available.

  // Methods to get the sample rate (i.e. timescales) for the streams and
  // the overall Movie presentation
  virtual uint32 getMovieDuration() const;
  virtual uint32 getMovieTimescale() const;
  virtual uint32 getMovieDurationMsec() const;

  // From Track
  virtual int32 getNumTracks(){return m_nNumStreams;}
  virtual uint32 getTrackWholeIDList(uint32 *ids);
  virtual int16 getTrackContentVersion(uint32 id);
  virtual uint8 trackRandomAccessDenied(uint32 id);
  virtual float  getTrackVideoFrameRate(uint32 id);
  virtual uint32 getTrackVideoFrameWidth(uint32 id);
  virtual uint32 getTrackVideoFrameHeight(uint32 id);

  // From MediaHeader
  virtual uint32 getTrackMediaDuration(uint32 id);
  virtual uint32 getTrackMediaTimescale(uint32 id);
  virtual uint32 getTrackAudioSamplingFreq(uint32 id);

  virtual uint32 getAudioSamplesPerFrame(uint32 id);
  virtual uint32 peekCurSample(uint32 trackid, file_sample_info_type *pSampleInfo);

  virtual uint8  getTrackOTIType(uint32 id); // Based on OTI value
  virtual uint8  getTrackAudioFormat(uint32 id); // based on VideoFMT enum //
  virtual uint8  getFramesPerSample(uint32 id);
  virtual uint16 getTotalNumberOfFrames(uint32 id);
  virtual int32  getTrackMaxBufferSizeDB(uint32 id);
  virtual int32  getTrackAverageBitrate(uint32 id);
  virtual int32  getTrackMaxBitrate(uint32 id);
  virtual uint32 getLargestFrameSize(uint32 id); 
  virtual long   getAudioFrameDuration(int);
  virtual void   SetIDX1Cache(void*);
  virtual void*  GetIDX1Cache();

   
  // use these functions only for windows media audio, other formats may not implement it //
  virtual unsigned long GetAudioBitsPerSample(int id);
  virtual unsigned long GetNumAudioChannels(int id);  
  virtual unsigned long GetAudioEncoderOptions(int id);

  // this returns Sequence (VOL) Header and its size //
  virtual uint8 *getTrackDecoderSpecificInfoContent(uint32 id);
  virtual uint32 getTrackDecoderSpecificInfoSize(uint32 id);

  virtual uint8 getAllowAudioOnly();
  virtual uint8 getAllowVideoOnly();
  virtual bool isGenericAudioFileInstance(){return false;}; 
  virtual bool isAviFileInstance(){return true;}
  virtual bool isADTSHeader();

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  virtual int32 setTrackAverageBitrate(uint32){return 0;};
  virtual uint32 GetTimeStampFromMedia(){return 0;};  
  virtual int32 getNextGenericAudioMediaSample(uint32, uint8 *, uint32, uint32, uint32 &){ return 0;};
#endif // FEATURE_QTV_GENERIC_AUDIO_FORMAT //

  bool SetTimeStampedSample(uint32 id, uint32 TimeStamp, uint32 *newTimeStamp, boolean isRewind);
  
  uint32 FileGetData(  uint32 nOffset, uint32 nNumBytesRequest, avi_int32 nMaxSize, uint8 *ppData  );
 
  uint64 getFileSize();
  void getCodecName(char *codecName,uint32 bufLen,uint32 trackId);

  int           GetTotalNumberOfAudioStreams();
  int           GetTotalNumberOfVideoStreams();
  int           GetTotalNumberOfTextStreams();
     
  bool          isVideoInstance(){return m_playVideo;}
  bool          isAudioInstance(){return m_playAudio;}
  bool          isTextInstance(){return m_playText;}

  uint32        GetMaximumBitRateForTrack(uint32 trackId);
 
#ifdef FEATURE_QTV_DIVX_DRM 
  virtual bool        IsDRMProtection();
  virtual void        SetDRMContextInfo(avi_uint8*,avi_uint32);
  virtual avi_uint8*  GetDRMContextInfo(avi_uint32*);
  virtual void        CopyDRMContextInfo(void*); 
  virtual void        GetClipDrmInfo(void*);
  virtual bool        CommitDivXPlayback();
#endif


private:  
  
  bool                              m_playAudio;
  bool                              m_playVideo;
  bool                              m_playText;
  bool                              m_corruptFile;
  
  bool                              m_bVideoReposPending;
  bool                              m_bAudioReposPending;

  // if we are streaming, rather than playing locally //
  bool                              m_bStreaming; 
  uint32                            m_nNumStreams;

  // These variable are added to know the largest size of audio and video samples respectively//
  avi_uint32                        m_audioLargestSize;
  avi_uint32                        m_videoLargestSize;

  file_sample_info_type   m_sampleInfo[FILE_MAX_MEDIA_STREAMS];
#ifdef FEATURE_MPEG4_B_FRAMES  
  //These are required to read ahead to know any B-Frames are following for Mpeg4 
  avi_video_sample       m_avi_video_samples[AVI_MAX_VIDEO_FRAMES_READ_AHEAD];
  avi_read_ahead_buffer  m_videoFramesReadAhead;
#endif /* FEATURE_MPEG4_B_FRAMES */

  uint32                  m_nDecodedDataSize[FILE_MAX_MEDIA_STREAMS];
  uint32                  m_nLargestFrame[FILE_MAX_MEDIA_STREAMS];

  uint8                   m_TrackDecoderSpecificInfo[2]; // used for AAC Header preparation

  //only one of "m_pFileBuf" or "m_filename" can be non-zero 
  OSCL_STRING     m_filename;  // EFS file path //
  unsigned char  *m_pFileBuf;  // pointer to buffer for playback from memory //
  uint32          m_FileBufSize;
  uint32          m_fileSize;

  OSCL_FILE     *m_AviFilePtr;
    
  // AVI parser handle
  aviParser*                     m_pAVIParser;
  
#ifdef FEATURE_QTV_DIVX_DRM
  //DRM context info.
  avi_uint8 *drmContext;  
  avi_uint32 drmContextLength;
  avi_uint8  drm_frame_info[DIVX_DRM_FRAME_DRM_INFO_SIZE];
#endif     

  uint8 *m_pAudioPacketBuffer;  // we read audio into this buffer //
  uint32 m_nAudioPacketBufferSize;      

  int16  m_nSelectedAudioStreamId;
  int16  m_nSelectedVideoStreamId;
  int16  m_nSelectedTextStreamId;

  //function to begin parsing avi file and update media information such as
  //total number of tracks, if drm is present etc.
  bool ParseMetaData();
  
#ifdef FEATURE_MEASURE_TIMING
  uint32 startTimeGetMetadata;
  uint32 endTimeGetMetadata;
#endif

 
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   uint32 m_minOffsetRequired;
   bool    bHttpStreaming;
   boolean bGetMetaDataSize;
   boolean bIsMetaDataParsed;
   Common::ParserStatusCode parserState;
   uint32  m_startupTime;
   uint32 m_maxPlayableTime[FILE_MAX_MEDIA_STREAMS];

   struct tHttpDataOffset
   {
      uint32 Offset;
      boolean bValid;
   } m_HttpDataBufferMinOffsetRequired;
   QtvPlayer::FetchBufferedDataSizeT m_fpFetchBufferedDataSize;
   QtvPlayer::FetchBufferedDataT m_fpFetchBufferedData;
   QtvPlayer::InstanceHandleT m_QtvInstancehandle;

   void   UpdateTrackIdInFilePointer();
   void sendParserEvent(Common::ParserStatusCode status);
   void sendParseHTTPStreamEvent(void);
   // virtual void updateBufferWritePtr ( uint32 writeOffset );
   //tWMCDecStatus getMetaDataSize ( void );
   virtual bool CanPlayTracks(uint32 pbTime);
   // virtual bool parseHTTPStream ( void );
   void sendHTTPStreamUnderrunEvent(void);
   boolean GetHTTPStreamDownLoadedBufferOffset(uint32 *pOffset);
   bool GetTotalAvgBitRate(uint32 * pBitRate);
   //tWMCDecStatus GetMediaMaxPlayableTime(U32_WMC *nMaxPBTime);

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
   virtual int32 setFileSize(uint32){return 0;};
#endif // FEATURE_QTV_GENERIC_AUDIO_FORMAT //

#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#ifdef FEATURE_QTV_DIVX_DRM
  virtual bool        initDivXDrmSystem();
  virtual avi_uint8*  GetDRMInfo(int*);
  void*        m_pClipDrmInfo;
#endif  
#ifdef FEATURE_MPEG4_B_FRAMES
  AVI_VOP_TYPE whichVop(uint8*, int, uint8 *);
#endif /* FEATURE_MPEG4_B_FRAMES */
  int32 getNextAVIMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 &index);
  Mpeg4Player *m_pMpeg4Player;  
};
#endif //FEATURE_QTV_AVI

#endif  /* __AVIFile_H__ */

