#ifndef __ASFFile_H__ 
#define __ASFFile_H__ 
/* =======================================================================
                              asffile.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/asfparser/main/latest/inc/asffile.h#23 $
$DateTime: 2010/03/21 22:20:53 $
$Change: 1230509 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#ifdef FEATURE_QTV_WINDOWS_MEDIA
/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "oscl_file_io.h"
#include "qcplayer_oscl_utils.h"
#include "QCUtils.h"
#include "filebase.h"
#include "wmc_type.h"
#include "wmcdecode.h"
#include "wmcguids.h"

#ifdef FEATURE_QTV_WMA_PRO_DECODER

/*
* Define following if want to dump PCM output to WAV file on EFS
* This will make the playback choppy as writing to EFS makes PCM samples retrieval slow.
* Should not be defined, only for debug purpose.
*/
//#define DUMP_PCM_OUTPUT_TO_WAV_FORMAT
#include "wmadecS_api.h"
#include "wmawfxdefs.h"
 #ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
    #include "wavfileexio.h"
 #endif
#endif

#include "wmftypes.h"
#include "wmfdec.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define ASF_READ_BUFFER_SIZE 1024       // file i/o buffer size (reading input file)
#define ASF_MOVIE_TIME_SCALE 1000       // WM Code has Milli Sec interface
#define ASF_STREAM_TIME_SCALE 1000      // WM Code has Milli Sec interface

#define PROFILE_TEMPLATE "DeviceConformanceTemplate"
#define S_PROFILE "SP"
#define M_PROFILE "MP"
#define META_DATA_INFO_LEN 56


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
class Mpeg4Player;
/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Function Declaration
** ======================================================================= */
void ConvertUnicodeToAscii(char *pDest, uint16 *pSrc , uint32 size);
void ConvertUnicodeToUTF8(uint8 *pDest,  uint32 destLen, uint16 *pSrc , uint32 srcLen); 
void GetByteFromBitStream(uint8 *pByte, uint8 *pSrc, int nFromBit, int nBits);

/* =======================================================================
**                        Class Declarations
** ======================================================================= */

/* ======================================================================
CLASS
  ASFFile

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
class ASFFile : public FileBase, public Parentable
{

public:

  class IDataSource 
  {
    public:    
    virtual ~IDataSource() { }
    virtual uint32 readData(uint8 *buffer, uint32 pos, uint32 size) = 0;
    virtual bool isEOS() = 0;
    virtual void setEOS(bool flag) = 0;
  };

  ASFFile(); // Default Constructor
  ASFFile(  const OSCL_STRING &filename,
            Mpeg4Player *pMpeg4Player,
            unsigned char *pFileBuf = NULL,
            uint32 bufSize = 0,
            IDataSource *dataSource = NULL,
            bool bPlayVideo = true,
            bool bPlayAudio = true
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
            ,bool bHttpStream = false
            ,uint32 wBufferOffset = 0
            ,uint32 wStartupTime = 0
            ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize = NULL
            ,QtvPlayer::FetchBufferedDataT FetchBufferedData = NULL
            ,QtvPlayer::InstanceHandleT handle = NULL
#endif //#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

	 ); // Constructor

#if defined (FEATURE_QTV_DRM_DCF) || defined (FEATURE_QTV_WM_DRM_API)
 /*
  * ASFFile constructor for DCF playback.
  * dcf_ixstream_type is actually IxStream*
  */
  void    *pDRMClientData;
	    
  QtvPlayer::DRMDecryptMethodT  pDRMDecryptFunction;

  ASFFile( dcf_ixstream_type inputStream, Mpeg4Player *pMpeg4Player, bool bPlayVideo, bool bPlayAudio);
#endif


  virtual ~ASFFile();

  virtual bool isAsfFile(void) {return true;};

  virtual int32 getNextMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 &index);
  virtual uint32 getMediaTimestampForCurrentSample(uint32 id);

  virtual uint32 getMediaTimestampDeltaForCurrentSample(uint32 id);

  virtual void resetPlayback();
  virtual uint32 resetPlayback(uint32 repos_time, uint32 id, bool bSetToSyncSample,
                               bool *bError, uint32 currentPosTimeStamp);
  virtual bool resetMediaPlayback(uint32 id);  /* reset playback for a particular track */

#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
  virtual uint32 skipNSyncSamples(int , uint32 , bool* , uint32  ){return 0;}
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
  virtual int32 getNumTracks();
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
  virtual uint8  getTrackAudioFormat(uint32 id); /* based on VideoFMT enum */
  virtual uint8  getFramesPerSample(uint32 id);
  virtual uint16  getTotalNumberOfFrames(uint32 id);
  virtual int32 getTrackMaxBufferSizeDB(uint32 id);
  virtual int32 getTrackAverageBitrate(uint32 id);
  virtual int32 getTrackMaxBitrate(uint32 id);
  virtual uint32 getLargestFrameSize(uint32 id);

  virtual uint16 GetAudioVirtualPacketSize(int id);

  virtual bool   isStartOfDummyBytes(int id);

  /* use these functions only for windows media audio, other formats may not implement it */
  virtual unsigned long GetAudioBitsPerSample(int id);
  virtual unsigned long GetNumAudioChannels(int id);
  virtual unsigned long GetFixedAsfAudioPacketSize(int id);
  virtual unsigned long GetAudioEncoderOptions(int id);
  virtual uint16 GetAudioAdvancedEncodeOptions(int id);
  virtual uint32 GetAudioAdvancedEncodeOptions2(int id);
  virtual uint16 GetAudioChannelMask(int id);
  virtual uint16 GetAudioArmDataReqThr(int id);
  virtual bool   UpdateTrackIdInFilePointer();
  virtual uint8 GetAudioValidBitsperSample(int id);
  virtual uint16 GetFormatTag(int id);
  virtual uint32 GetBlockAlign(int id);


  /* this returns Sequence (VOL) Header and its size */
  virtual uint8 *getTrackDecoderSpecificInfoContent(uint32 id);
  virtual uint32 getTrackDecoderSpecificInfoSize(uint32 id);

  virtual uint8 getAllowAudioOnly();
  virtual uint8 getAllowVideoOnly();
  virtual bool isGenericAudioFileInstance(){return false;};
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  virtual int32 setTrackAverageBitrate(uint32){return 0;};
  virtual uint32 GetTimeStampFromMedia(){return 0;};  
  virtual int32 getNextGenericAudioMediaSample(uint32, uint8 *, uint32, uint32, uint32 &){ return 0;};
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

  bool SetTimeStampedSample(uint32 id, uint32 TimeStamp, uint32 *newTimeStamp, boolean isRewind);

  uint32 FileGetData(  uint32 nOffset,
                       uint32 nNumBytesRequest,
                       uint8 **ppData  );

  uint32 getPrerollTime();
  uint64 getFileSize();
  int32 getCodecName(char *codecName, int32 bufLen, tMediaType_WMC mediaType);

  void   updateASFStreamingRepositioningInfo(U32_WMC reposTime,int id);

  U16_WMC           GetTotalNumberOfAudioStreams();
  U16_WMC           GetTotalNumberOfVideoStreams();
  U16_WMC           GetTotalNumberOfBinaryStreams();
  U32_WMC           GetFirstPacketOffset();
  U32_WMC           GetLastPacketOffset();
  U32_WMC           GetAsfPacketSize();
  U32_WMC           GetWMVCompressionType();
  bool              isVideoInstance();
  bool              isAudioInstance();
  bool              isTextInstance();
  uint32            GetMaximumBitRateForTrack(uint32 trackId);


#ifdef FEATURE_QTV_WM_SWITCHING_THINNING
  void              MarkVideoTrackAsValid(uint32 id);
#endif //FEATURE_QTV_WM_SWITCHING_THINNING
  virtual long              GetAudioASFPacketDuration(int);

#ifdef FEATURE_QTV_WM_DRM_API
  virtual bool DRMDecryptSwitch(uint32 wTrackID,             /* to which track this data belong to */
                               void *pEncryptedDataBuf,     /* pointer to encrypted data buffer, which has to be decrypted */
                               void *pDecryptedDataBuf,     /* pointer to destination buffer to copy decrypted data, 
                                                               OEM is resposible for copying the decrypted data  */
                               uint32 wEncryptedDataSize,   /* encrypted data buffer size */
                               uint32 *pDecryptedDataSize); /* pointer to decrypted data buffer size, 
                                                               OEM is resposible for copying the decrypted data size  */
#endif


private:

  bool                              m_playAudio;
  bool                              m_playVideo;
  bool                              m_playText;
  bool                              m_corruptFile;
  bool                              m_bVideoReposPending;
  bool                              m_bAudioReposPending;
  bool                              m_bStreaming; /* if we are streaming, rather than playing locally */
  bool                              m_bWMADecodeDone;
  unsigned int                      m_nWMATotalDummyBytesSent;
  bool                              m_bIsDummyBytesStart;
  long                              m_nASFAudioPacketDuration;


  file_sample_info_type   m_sampleInfo[FILE_MAX_MEDIA_STREAMS];
  file_sample_info_type   m_prvSampleInfo[FILE_MAX_MEDIA_STREAMS];
  uint32                  m_nDecodedDataSize[FILE_MAX_MEDIA_STREAMS];
  uint32                  m_nLargestFrame[FILE_MAX_MEDIA_STREAMS];

  //The size of the file. This is used to prevent data underruns, when the file size on
  //disk is less than the actual file size.
  uint32                  m_filesize;

  /* only one of "m_pFileBuf" or "m_filename" can be non-zero */
  OSCL_STRING     m_filename;     /* EFS file path */
  unsigned char  *m_pFileBuf;  /* pointer to buffer for playback from memory */
  uint32          m_FileBufSize;

  OSCL_FILE     *m_AsfFilePtr;
  uint8          m_ReadBuffer[ASF_READ_BUFFER_SIZE];    // file i/o buffer for our input content file
  IDataSource   *m_dataSource;

  /* WM Parser/Decoder related variables */
  HWMCDECODER                       m_hASFDecoder;
  U32_WMC                           m_nNumStreams;
  strHeaderInfo_WMC                 m_strHeaderInfo;
  tStreamIdnMediaType_WMC          *m_ptMediaTypeStreams;
  const WMCContentDescription      *m_pContentDesc;
  tStreamIdPattern_WMC             *m_pStreamDecodePattern;
  strAudioInfo_WMC                  m_strAudioInfo;
  strVideoInfo_WMC                  m_strVideoInfo;
  strBinaryInfo_WMC                 m_strBinaryInfo;
  bool                              m_bAllowWmaPackets;  /* audio stream is in packet format */

  /* variables needed to decode audio packets into chunk of frames
     which microsoft calls "packet layer decoding */
  uint8 *m_pAudioPacketBuffer;  /* we read audio packet into this buffer */
  uint32 m_nAudioPacketBufferSize;
  uint32 m_nWmaNumPrevFrameBits;

  //Total number of bytes generated from ASFFILE
  uint32 m_nTotalAudioBytesSent;

  //Variables used to determine packet drop
  uint16 m_nPreviousAudioPacketNum;
  uint16 m_nPreviousAudioPacketFramesNum;
  bool   m_bIsPacketDrop;

  int16  m_nSelectedAudioStreamId;
  int16  m_nSelectedVideoStreamId;
  int16  m_nSelectedBinaryStreamId;
  uint32 m_nAsfDataBufferSize;


  /* internal helper functions */
  tWMCDecStatus   ParseMetaData ( void );
  tWMCDecStatus   GetMediaTypeForStreamID(tMediaType_WMC *pType, uint32 wStreamId);
  tWMCDecStatus   GetStreamNumForID(uint32 *pStreamNum, uint32 wStreamId);
  AUDIOSTREAMINFO *GetAudioStreamInfo(uint32 wStreamId);
  VIDEOSTREAMINFO *GetVideoStreamInfo(uint32 wStreamId);
  BINARYSTREAMINFO *GetBinaryStreamInfo(uint32 wStreamId);
  tWMCDecStatus     GetVideoFrame( uint32 id, tMediaType_WMC streamType, uint8 *buf,
                                   uint32 size, U32_WMC * pOutDataSize, bool bSyncFrame );
  tWMCDecStatus     GetAudioFrame( uint32 id, tMediaType_WMC streamType,
                                   U8_WMC * buf, U32_WMC size, U32_WMC * pOutDataSize );
  
  tWMCDecStatus    GetStandardAudioFrame( uint32 id, tMediaType_WMC streamType,
                                          U8_WMC * buf, U32_WMC size, U32_WMC * pOutDataSize );

#ifdef FEATURE_QTV_WMA_PRO_DECODER
  //To pull PCM samples out of WMA decoder
  tWMCDecStatus GetAudioDecodedSamples(uint32 id, tMediaType_WMC streamType,
                                       U8_WMC * buf, U32_WMC size,
                                       U32_WMC * pOutDataSize,
                                       U32_WMC * pnSamples);
  //Prepare WAV file format from PCM format.
  void PCMFormat2WaveFormatEx(PCMFormat* pFormat, WAVEFORMATEX* wfx);
  
  /*
  * Scans all audio track and sets 'm_bUseARMDecoder' to true if
  * we need to use ARM based WMA decoder.
  */
  void          UpdateDecoderTrackInfo();
  
  
  //@see above for UpdateDecoderTrackInfo.
  bool          m_bUseARMDecoder;

  WAVEFORMATEXTENSIBLE  m_sWAVEFormatEx;
 
  #ifdef DUMP_PCM_OUTPUT_TO_WAV_FORMAT
    WavFileIO *pwfioOut;
  #endif
#endif

  bool m_isDSPWMA_pro;


#if defined(FEATURE_QTV_WMA_PRO_DECODER) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
  //Returns true if we will be using ARM based decoder for playback of given trackid.
  bool          isWmaProDecoderNeeded(uint32 trackid,uint8* format=NULL);
#endif /* defined(FEATURE_QTV_WMA_PRO_DECODER) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

  bool isVideoCodecSupported();
  bool isIndexObjectPresent();

  unsigned long getMSecValueForFrameRateCalculation(U64_WMC);


  typedef struct m_asfStreamingInfo
  {
    bool bAudioValid;
    bool bVideoValid;
  }ASFSTREAMINGINFO;

  ASFSTREAMINGINFO m_wmStreamingInfo;


#ifdef FEATURE_MEASURE_TIMING
  uint32 startTimeGetMetadata;
  uint32 endTimeGetMetadata;
#endif
  //Variables used to process 2 ASF packets. This will make sure we send full complete data for all audio frames
  //in a given ASF packet. This kind of assumes that last frame data can be spilled only in to the next ASF packet.
  U8_WMC* m_pasfDataBuffer;
  int m_nCntPrvBytes;
  int m_nCntCurBytes;
  bool m_bBufferValid;

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   uint32 m_minOffsetRequired;
   bool    bHttpStreaming;
   boolean bGetMetaDataSize;
   boolean bIsMetaDataParsed;
   Common::ParserStatusCode parserState;
   uint32  m_startupTime;
   U32_WMC m_maxPlayableTime[FILE_MAX_MEDIA_STREAMS];

   struct tHttpDataOffset
   {
      U32_WMC Offset;
      boolean bValid;
   } m_HttpDataBufferMinOffsetRequired;
   QtvPlayer::FetchBufferedDataSizeT m_fpFetchBufferedDataSize;
   QtvPlayer::FetchBufferedDataT m_fpFetchBufferedData;
   QtvPlayer::InstanceHandleT m_QtvInstancehandle;

   void sendParserEvent(Common::ParserStatusCode status);   
   virtual void updateBufferWritePtr ( uint32 writeOffset );
   tWMCDecStatus getMetaDataSize ( void );
   virtual bool CanPlayTracks(uint32 pbTime);
   virtual bool parseHTTPStream ( void );   
   boolean GetHTTPStreamDownLoadedBufferOffset(U32_WMC *pOffset, boolean &bEod);
   bool GetTotalAvgBitRate(U32_WMC * pBitRate);
   tWMCDecStatus GetMediaMaxPlayableTime(U32_WMC *nMaxPBTime);


  virtual void sendParseHTTPStreamEvent(void);      
  virtual void sendHTTPStreamUnderrunEvent(void);
  virtual bool parsePseudoStream( void );

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
   virtual int32 setFileSize(uint32){return 0;};
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

#endif //#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

#ifdef FEATURE_QTV_WM_DRM_API
   virtual bool RegisterDRMDecryptMethod(QtvPlayer::DRMDecryptMethodT pDRMDecriptFunction, void *pClientData);
#endif /* FEATURE_QTV_WM_DRM_API */

inline void UpdateSamplesInformation(uint8 cPacketNum, uint8 cNumFrm,
                                     uint32 StreamNum, uint32 PresentationTime,
                                     uint32 size, uint32 wNumFrmInPacket);
typedef enum Profile
{
  UNKNOWN_PROFILE,
  SIMPLEPROFILE,
  MAINPROFILE,
  UNSUPPORTED_PROFILE
}WMProfile;

WMProfile m_VidProfile;

 void InitData();

 #ifdef FEATURE_QTV_DRM_DCF
  /*
  * Class members for supporting DCF playback
  */
  dcf_ixstream_type m_inputStream;  
 #endif

  Mpeg4Player *m_pMpeg4Player;

};

#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#endif  /* __ASFFile_H__ */

