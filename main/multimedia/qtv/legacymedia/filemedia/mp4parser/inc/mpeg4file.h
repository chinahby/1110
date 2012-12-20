#ifndef __Mpeg4File_H__ 
#define __Mpeg4File_H__ 
/* =======================================================================
                              mpeg4file.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/mpeg4file.h#21 $
$DateTime: 2010/01/07 03:31:52 $
$Change: 1129999 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_file_io.h"
#include "qcplayer_oscl_utils.h"
#include "filebase.h"
#include "parentable.h"
#include "Events.h"
#include "timedtext.h"

#ifdef FEATURE_MP4_KDDI_META_DATA
  #include "kddiuserdataatom.h"
#endif /* FEATURE_MP4_KDDI_META_DATA */

#include "udtaatoms.h"
#include "pdcfatoms.h"

#include "dcmdUserDataAtom.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "videofmt.h"
#include "msg.h"
#include "cmx.h"
#ifdef __cplusplus
} // extern "C"
#endif

#include "ztl.h"

//HEADER FILES REQD FOR MULTIPLE SAMPLE RETRIEVAL API
#include "oscl_media_data.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_MP4_AMR
const int16 AMRModeSetMask[8] =
{
    0x0001, 0x0002, 0x0004, 0x0008,
    0x0010, 0x0020, 0x0040, 0x0080
};

const int32 AMRBitRates[8] =
{
    4750, 5150,  5900,  6700,
    7400, 7950, 10200, 12200
};
#endif  // FEATURE_MP4_AMR

/* to break the VideoFMT read loop, if videoFMT hangs */
#define MPEG4_VIDEOFMT_MAX_LOOP 50000

static const uint32 AAC_FORMAT_UNK = 0;
static const uint32 AAC_FORMAT_ADTS = 1;
static const uint32 AAC_FORMAT_NON_ADTS = 2;


#define QTV_MPEG4_COARSE_REPOS_LIMIT 5*60*1000       //5min
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
static const uint8 maskByte[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
#endif /* FEATURE_QTV_BSAC */
#define DURATION_OF_AMR_FRAME_BLOCK 20

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
class Mpeg4Player;
class AudioPlayer;
class VideoPlayer;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
class TimedText;
#endif


/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Class Declarations
** ======================================================================= */

/* ======================================================================
CLASS
  Mpeg4File

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
class Mpeg4File : public FileBase, public Parentable
{

public:

  Mpeg4File(); // Default Constructor

  Mpeg4File(  OSCL_STRING filename,
              Mpeg4Player *pMpeg4Player,
              unsigned char *pFileBuf=NULL,
              uint32 bufSize=0,
              bool bPlayVideo = false,
              bool bPlayAudio = false,
              bool bPlayText  = false
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#if defined (FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
              ,bool bHttpStream     = false
              ,uint32 wBufferOffset = 0
              ,uint32 wStartupTime  = 0
              ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize = NULL
              ,QtvPlayer::FetchBufferedDataT FetchBufferedData = NULL
              ,QtvPlayer::InstanceHandleT handle = NULL
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
            ); // local file playback Constructor

#ifdef FEATURE_QTV_DRM_DCF
  // DRM-file-media playback Constructor
    Mpeg4File( dcf_ixstream_type inputStream,
               Mpeg4Player *pMpeg4Player,
               bool bPlayVideo = false,
               bool bPlayAudio = false,
               bool bPlayText  = false);
#endif

  virtual ~Mpeg4File();
  virtual bool isMp4File(void) {return true;};

  /* method to find if mpeg4 file is fragmented or non-fragmented */
  virtual bool isFileFragmented(void);

  virtual int32 getNextMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 &index);
  virtual uint32 getMediaTimestampForCurrentSample(uint32 id);
  virtual uint32 getMediaTimestampDeltaForCurrentSample(uint32 id);

#ifdef FEATURE_H264_DECODER
  uint32 m_nextSeqSample;
  uint32 m_nextPicSample;
  virtual uint32 getNumPicParamSet(uint32 trackId);
  virtual uint32 getNumSeqParamSet(uint32 trackId);
  virtual int32 getNextParamSetNAL(uint32 trackId, uint8 *buf, uint32 size);
  virtual void  resetParamSetNAL(uint32 trackId);
  virtual int  GetSizeOfNALLengthField(uint32 trackId);
#endif /* FEATURE_H264_DECODER */

  virtual uint32 GetData(QtvPlayer::DataT dType, uint8 *pBuf, uint32 size, uint32 offset);
  virtual bool resetMediaPlayback(uint32 id);  /* reset playback for a particular track */
  uint32 fragmentNumber;
  //Return MPEG VOL header information
  virtual uint8 *getTrackDecoderSpecificInfoContent(uint32 id);
  virtual MP4_ERROR_CODE getTrackDecoderSpecificInfoContent(uint32 id, uint8* buf, uint32 *pbufSize);
  virtual uint32 getTrackDecoderSpecificInfoSize(uint32 id);

  virtual uint8 *getTrackDecoderSpecificInfoAtSDI(uint32 trackID, uint32 index);

  virtual void resetPlayback();
  virtual uint32 resetPlayback(uint32 pos, uint32 id, bool bSetToSyncSample, bool *bError, uint32 currentPosTimeStamp);

#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
  virtual uint32 skipNSyncSamples(int offset, uint32 id, bool *bError, uint32 currentPosTimeStamp);
#endif /* FEATURE_QTV_REPOSITION_SYNC_FRAME */

  // from 'rand' atom at top level
  virtual uint8 randomAccessDenied();

  // pvmm
  virtual OSCL_STRING getTitle() const;
  virtual OSCL_STRING getAuthor() const;
  virtual OSCL_STRING getDescription() const;
  virtual OSCL_STRING getRating() const;
  virtual OSCL_STRING getCopyright() const;
  virtual OSCL_STRING getVersion() const;
  virtual OSCL_STRING getCreationDate() const;
  virtual OSCL_STRING getPerf()const;
  virtual OSCL_STRING getGenre()const;
  virtual OSCL_STRING getClsf()const;
  virtual OSCL_STRING getKywd()const;
  virtual OSCL_STRING getLoci()const;
  virtual OSCL_STRING getAlbum()const;
  virtual OSCL_STRING getYrrc()const;

  // from 'ftyp' atom
  virtual uint32 getCompatibiltyMajorBrand();
  virtual uint32 getCompatibiltyMajorBrandVersion();
  virtual ZArray<uint32> *getCompatibiltyList();

  // From Movie
  virtual int32 getNumTracks();
  virtual uint32 getTrackWholeIDList(uint32 *ids);

  // RETRIEVAL METHODS
  // Methods to get the sample rate (i.e. timescales) for the streams and
  // the overall Mpeg-4 presentation
  virtual uint32 getMovieDuration() const;
  virtual uint32 getMovieTimescale() const;

  uint32 getMovieDurationMsec() const;

  // From Track
  virtual int16 getTrackContentVersion(uint32 id);
  virtual uint8 trackRandomAccessDenied(uint32 id);
  virtual float  getTrackVideoFrameRate(uint32 id); // from 'vinf' atom at track level

  virtual uint32 getTrackVideoFrameWidth(uint32 id); // from pvti atom at file level
  virtual uint32 getTrackVideoFrameHeight(uint32 id);

  virtual unsigned long GetNumAudioChannels(int id); // from qtv_windows_media
  virtual OSCL_STRING getAudioTrackLanguage(uint32 id);


  // From TrackReference
  virtual uint32  trackDependsOn(uint32 id);

  // From MediaHeader
  virtual uint32 getTrackMediaDuration(uint32 id);
  virtual uint32 getTrackMediaTimescale(uint32 id);
  virtual uint32 getTrackAudioSamplingFreq(uint32 id);

  virtual uint32 getAudioSamplesPerFrame(uint32 id);
  virtual uint32 peekCurSample(uint32 trackid, file_sample_info_type *pSampleInfo);

  /* Mp4 Clip may has meta data */
  virtual bool HasMetaData() {return true;};
  virtual bool   isADTSHeader();
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT

  /* 3GPP timed text related APIs */
  virtual TextSampleEntry *getTextSampleEntryAt (uint32 trackid, uint32 index);
  virtual void Parse3GPPTimedTextAtom(video_fmt_tx3g_data_type *tx3g_atom,OSCL_FILE* localFilePtr);
  virtual uint32 GetNumTX3GAtom();
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT

//KDDI Telop related APIs
  virtual bool IsTelopPresent();
  virtual uint32 getTelopTrackDuration();
  virtual TelopElement *getNextTelopElement();
  virtual TelopHeader *getTelopHeader();
  virtual uint32 resetTelopPlayback(uint32 startPos);
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_MP4_KDDI_META_DATA

  /*KDDI Meta Data Related APIs*/

  // From CopyGaurdAtom
  virtual uint32 getCopyProhibitionFlag();
  virtual uint32 getValidityEffectiveDate();
  virtual uint32 getValidityPeriod();
  virtual uint32 getNumberofAllowedPlayBacks();

  // From Content Property Atom
  virtual OSCL_STRING getContentPropertyTitle();
  virtual OSCL_STRING getContentPropertyCopyRight();
  virtual OSCL_STRING getContentPropertyAuthor();
  virtual OSCL_STRING getContentPropertyMemo();
  virtual uint32      getAuthorDLLVersion();

  //From Movie Mail Atom
  virtual uint32 getEditFlags();
  virtual uint8  getRecordingMode();
  virtual uint32 getRecordingDate();

  //From Encoder Information Atom
  virtual uint8*  getDeviceName() const;
  virtual uint8*  getModelName() const;
  virtual uint8*  getEncoderInformation() const;
  virtual uint8*  getMuxInformation() const;

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  virtual uint8*  getTelopInformationString() const;
  virtual uint32  getTelopInformationSize();
#endif

  //KDDI GPS Atom
  virtual uint16 getGPSByteOrder();
  virtual uint32  getGPSVersionID();
  virtual uint32  getGPSLatitudeRef();
  virtual uint32  getGPSLongitudeRef();
  virtual uint32  getGPSAltitudeRef();
  virtual uint64 *getGPSLatitudeArray();
  virtual uint64 *getGPSLongitudeArray();
  virtual uint64 getGPSAltitude();
  virtual uint64 *getGPSTimeArray();
  virtual OSCL_STRING getGPSSurveyData();
  virtual OSCL_STRING getPositoningMethod();
  virtual OSCL_STRING getPositioningName();
  virtual OSCL_STRING getGPSDate();
  virtual uint64 getGPSExtensionMapScaleInfo();

#endif /* FEATURE_MP4_KDDI_META_DATA */

  virtual uint8  getTrackOTIType(uint32 id); // Based on OTI value
  virtual uint8  getTrackAudioFormat(uint32 id); /* based on VideoFMT enum */
  virtual uint8  getFramesPerSample(uint32 id);
  virtual uint16  getTotalNumberOfFrames(uint32 id);

  virtual int32 getTrackMaxBufferSizeDB(uint32 id);

  virtual bool  isAudioPresentInClip();
  virtual void setAudioPlayerData(const void *client_data);
  virtual void setVideoPlayerData(const void *client_data);
  virtual void setTextPlayerData(const void *client_data);
  AudioPlayer *AudioPlayerPtr;
  VideoPlayer *VideoPlayerPtr;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  TimedText   *TextPlayerPtr;
#endif

  virtual int32 getTrackAverageBitrate(uint32 id);
  virtual int32 getTrackMaxBitrate(uint32 id);
  virtual bool isGenericAudioFileInstance(){return false;};
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  virtual int32 setTrackAverageBitrate(uint32){return 0;};  
  virtual uint32 GetTimeStampFromMedia(){return 0;};
  virtual int32 getNextGenericAudioMediaSample(uint32, uint8 *, uint32, uint32, uint32 &) { return 0;};  
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
  

  virtual uint8 getAllowAudioOnly();
  virtual uint8 getAllowVideoOnly();

// Operations
public:
  static void mp4ParseStatusCallback (video_fmt_status_type status,
                                      void *client_data,
                                      video_fmt_status_cb_info_type *info,
                                      video_fmt_end_cb_func_type end);

  void mp4ParseStatus ( video_fmt_status_type status,
                        video_fmt_status_cb_info_type *info,
                        video_fmt_end_cb_func_type end);

  static void mp4ReadStatusCallback (video_fmt_status_type status,
                                     void *client_data,
                                     video_fmt_status_cb_info_type *info,
                                     video_fmt_end_cb_func_type end);

  void mp4ReadStatus (uint32 streamNum,
                      video_fmt_status_type status,
                      video_fmt_status_cb_info_type *info,
                      video_fmt_end_cb_func_type end);

  static void mp4SyncStatusCallback (video_fmt_status_type status,
                                     void *client_data,
                                     video_fmt_status_cb_info_type *info,
                                     video_fmt_end_cb_func_type end);

  void mp4SyncStatus (uint32 streamNum,
                      video_fmt_status_type status,
                      video_fmt_status_cb_info_type *info,
                      video_fmt_end_cb_func_type end);

  video_fmt_stream_info_type *getTrackInfoForID (uint32 id);

  uint32 getTrackIdFromStreamNum (uint32 streamNum);

  int32 getSampleInfo (uint32 streamNum,
                       uint32 startingSample,
                       uint32 sampleCount,
                       video_fmt_sample_info_type *buffer);
  int32 getSample (uint32 streamNum,
                   video_fmt_data_unit_type unitDef,
                   uint32 startingUnit,
                   uint32 unitCount,
                   uint8 *buffer);
  bool getSyncSampleInfo (uint32 streamNum,
                          uint32 sampleNum,
                          bool reverse,
                          video_fmt_sample_info_type *buffer);

  virtual bool getTimestampedSampleInfo(video_fmt_stream_info_type *p_track,
                                uint32                      TimeStamp,
                                video_fmt_sample_info_type *sampleInfo,
                                uint32                     *newTimeStamp,
                                bool                        bSetToSyncSample,
                                uint32                      currentPosTimeStamp);

  virtual  bool getSampleAtTimestamp(video_fmt_stream_info_type *p_track,
                            uint32                      timestamp,
                            bool                        rewind,
                            video_fmt_sample_info_type *sampleInfo);

  virtual void parseFirstFragment();
  virtual uint32 getLargestFrameSize (uint32 id);

#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD ||defined FEATURE_QTV_PSEUDO_STREAM
  virtual bool CanPlayTracks(uint32 pbTime);
  virtual void updateBufferWritePtr(uint32 writeOffset);
  virtual bool parseHTTPStream( void );
  virtual bool ParseStream();
  virtual boolean initializeVideoFMT(void);
  void sendParseHTTPStreamEvent( void );
  void sendHTTPStreamUnderrunEvent(void);
  void sendParserEvent(Common::ParserStatusCode status);
  bool parseMetaData (void);
  bool peekMetaDataSize (uint32 fragment_num);
  bool getMetaDataSize ( void );
  uint32 getSampleAbsOffset (uint32 streamNum,
                             uint32 sampleOffset,
                             uint32 sampleSize);
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  virtual int32 setFileSize(uint32){return 0;};
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */  

#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#if defined(FEATURE_MP4_KDDI_META_DATA) || defined(FEATURE_QTV_SKT_MOD_MIDI)
  uint32 getNumUUIDAtom();
  ZArray<Atom *> UUIDatomEntryArray;
  uint32 UUIDatomEntryCount;
#endif
    virtual void resetPlaybackPos(uint32 tId);

    const video_fmt_info_type& getVideoFmtInfo( void ) const { return m_videoFmtInfo; };
#ifdef FEATURE_QTV_PDCF
    virtual QtvPlayer::EncryptionTypeT getEncryptionType();
    virtual QtvPlayer::EncryptionTypeT getEncryptionType(uint32 track_id);  
    virtual bool RegisterDRMDecryptMethod(QtvPlayer::DRMDecryptMethodT pDRMDecriptFunction, void *pClientData);  
#endif /* FEATURE_QTV_PDCF */
    
protected:

  //Qtv mpeg4 default value for coarse repositioning
  uint32 m_defaultCoarseJumpLimit;

  // These variables are used to communicate between the class and the
  // callback function registered with the video format services.
  struct videoFMTClientData
  {
    Mpeg4File * Mp4FilePtr;
    int streamNum;
  };

  video_fmt_status_type             m_mp4ParseLastStatus;
  bool                              m_hasAudio, m_hasVideo, m_hasText;
  bool                              m_allSyncVideo; /* all video frames are SYNC frames (STSS not present) */

  uint32                            m_parseIODoneSize;
  video_fmt_continue_cb_func_type   m_mp4ParseContinueCb;

  bool                              m_playAudio;
  bool                              m_playVideo;
  bool                              m_playText;
  bool                              m_isFragmentedFile;

  /*
  * Variables to indicate whether clip has audio/video/text tracks.
  * These are useful in reporting UNSUPPORTED_FORMAT notifications if we don't end up selecting
  * any of the following media tracks but clip has corresponding tracks.
  */
  bool                              m_bAudioPresentInClip;
  bool                              m_bVideoPresentInClip;
  bool                              m_bTextPresentInClip;

  /* Flag to mark if the current parsing position is end of fragment */
  bool                              m_parsedEndofFragment;
  /* Flag to mark if the current parsing position is end of file */
  bool                              m_parsedEndofFile;

  bool                              m_corruptFile;
  uint32                            m_iodoneSize[VIDEO_FMT_MAX_MEDIA_STREAMS];
  /* using this member variable to distinguish between udta vs. kddi 'titl' atom */
  bool                              m_bUdtaAtomPresent;


  uint32                            m_absFileOffset[VIDEO_FMT_MAX_MEDIA_STREAMS];

  videoFMTClientData                m_clientData[VIDEO_FMT_MAX_MEDIA_STREAMS];

  video_fmt_status_type             m_mp4ReadLastStatus[VIDEO_FMT_MAX_MEDIA_STREAMS];
  video_fmt_continue_cb_func_type   m_mp4ReadContinueCb[VIDEO_FMT_MAX_MEDIA_STREAMS];
  void                             *m_mp4ReadServerData[VIDEO_FMT_MAX_MEDIA_STREAMS];

  video_fmt_status_type             m_mp4SyncLastStatus[VIDEO_FMT_MAX_MEDIA_STREAMS];
  video_fmt_continue_cb_func_type   m_mp4SyncContinueCb[VIDEO_FMT_MAX_MEDIA_STREAMS];
  void                             *m_mp4SyncServerData[VIDEO_FMT_MAX_MEDIA_STREAMS];

  // This is a copy of the stream information returned by the video format
  // services after parsing an MP4 file.
  video_fmt_end_cb_func_type        m_mp4ParseEndCb;
  void                              *m_mp4ParseServerData;
  video_fmt_info_type               m_videoFmtInfo;

  video_fmt_sample_info_type  m_sampleInfo [VIDEO_FMT_MAX_MEDIA_STREAMS];
  uint32  m_nextSample [VIDEO_FMT_MAX_MEDIA_STREAMS];
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
  uint8  m_reposStreamPending;
#endif /* FEATURE_QTV_BSAC */
  uint32 m_nextReposSample [VIDEO_FMT_MAX_MEDIA_STREAMS];

  video_fmt_stream_info_type* m_track[VIDEO_FMT_MAX_MEDIA_STREAMS];
  uint32 m_trackCount;

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  ZArray<TextSampleEntry *> textSampleEntryArray;
  uint32 textSampleEntryCount;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  OSCL_FILE*  m_parseFilePtr;
  OSCL_FILE*  m_streamFilePtr[VIDEO_FMT_MAX_MEDIA_STREAMS];
  OSCL_STRING m_filename;
  uint32      m_fileSize;
  boolean     m_fileSizeFound;

  /* only one of "m_pFileBuf" or "m_filename" can be non-zero */
  unsigned char *m_pFileBuf;  /* pointer to buffer for playback from memory */

#ifdef FEATURE_QTV_DRM_DCF
  //member variable to store IxStream
  dcf_ixstream_type m_inputStream;
#endif

#ifdef FEATURE_QTV_PDCF
/* This is to hold the each pdcf atom */
  typedef struct
  {
   uint32 track_id;
   QtvPlayer::EncryptionTypeT encryptionType;   
  }Track_Encryption_Type;
    
  Track_Encryption_Type   m_EncryptionType[VIDEO_FMT_MAX_MEDIA_STREAMS];
  uint8                   m_OffsetinEncryptionTypeArray;
  void                    *m_pDRMClientData;           /* client data provided by OEM when registering callback */

  boolean (*m_pDRMDecryptFunction)
  (
    uint32      TrackId,          /* track type */
    void      *pEncryptedDataBuf,   /* pointer to encrypted data buffer, which has to be decrypted */
    void      *pDecryptedDataBuf,   /* pointer to destination buffer to copy decrypted data, 
                                       OEM is resposible for copying the decrypted data  */
    uint32    wEncryptedDataSize,   /* encrypted data buffer size */
    uint32    *pDecryptedDataSize,   /* pointer to decrypted data buffer size, 
                                       OEM is resposible for copying the decrypted data size  */
    void      *pDRMClientData       /* client data provided by OEM when registering callback */
  ); 
  PdcfAtom * _pdcfAtom;
  uint8 *m_pEncryptedDataBuffer;
#endif /* FEATURE_QTV_PDCF */


#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   uint32 m_minOffsetRequired;
   bool bHttpStreaming;
   uint32 m_wBufferOffset;        //valid downloaded bytes [0..m_wBufferOffset)
   uint32 m_currentParseFragment;
   boolean bGetMetaDataSize;
   boolean bDataIncomplete;
   boolean bQtvPlayerPaused;
   boolean bsendParseFragmentCmd;
   QCCritSectType videoFMT_Access_CS;
   Common::ParserStatusCode parserState;
   uint32 m_pbTime;
   uint32 m_startupTime;
   QtvPlayer::FetchBufferedDataSizeT m_fpFetchBufferedDataSize;
   QtvPlayer::FetchBufferedDataT m_fpFetchBufferedData;
   QtvPlayer::InstanceHandleT m_QtvInstancehandle;
   uint32 mdat_size;
   boolean Initialized;
   boolean Parsed;
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */

#ifdef FEATURE_MEASURE_TIMING
  uint32 startTimeGetMetadata;
  uint32 endTimeGetMetadata;
#endif

#ifdef FEATURE_MP4_KDDI_META_DATA
  KDDIDrmAtom                *_kddiDRMAtom;
  KDDIContentPropertyAtom    *_kddiContentPropertyAtom;
  KDDIMovieMailAtom          *_kddiMovieMailAtom;
  KDDIEncoderInformationAtom *_kddiEncoderInformationAtom;
  KDDIGPSAtom                *_kddiGPSAtom;
#endif /* FEATURE_MP4_KDDI_META_DATA */

#ifdef FEATURE_QTV_SKT_MOD_MIDI
  UdtaMidiAtom * _midiAtom;
  virtual bool IsMidiDataPresent();
  virtual uint32 GetMidiDataSize();
  virtual uint32 GetMidiData(uint8 *pBuf, uint32 size, uint32 offset);

  UdtaLinkAtom * _linkAtom;
  virtual bool IsLinkDataPresent();
  virtual uint32 GetLinkDataSize();
  virtual uint32 GetLinkData(uint8 *pBuf, uint32 size);

#endif /* FEATURE_QTV_SKT_MOD_MIDI */

  FtypAtom * _ftypAtom; /* File Type atom*/
  UdtaCprtAtom * _cprtAtom; /* Copyright atom*/
  UdtaAuthAtom * _authAtom; /* Author atom */
  UdtaTitlAtom * _titlAtom; /* Title atom */
  UdtaDscpAtom * _dscpAtom; /* Description atom */
  UdtaRtngAtom * _rtngAtom; /* Rating atom */
  UdtaGnreAtom * _gnreAtom; /* Genre atom */
  UdtaPerfAtom * _perfAtom; /* Performance atom */
  UdtaClsfAtom * _clsfAtom; /* Classification atom */
  UdtaKywdAtom * _kywdAtom; /* Keyword */
  UdtaLociAtom * _lociAtom; /* Location atom */
  UdtaMetaAtom * _metaAtom; /* Meta atom*/
  DcmdDrmAtom * _dcmdAtom; /* DRM atom */
  UdtaAlbumAtom * _albumAtom; /* Album atom */
  UdtaYrrcAtom * _yrrcAtom; /* yrcc atom */

  virtual bool IsDataPresent(QtvPlayer::DataT dType, uint32 track_id);
  virtual uint32 GetDataSize(QtvPlayer::DataT dType, uint32 offset);
#if defined(FEATURE_MP4_KDDI_META_DATA) || defined(FEATURE_QTV_SKT_MOD_MIDI)
  void mp4ParseUUIDAtom(video_fmt_uuid_data_type *pAtomInfo,OSCL_FILE* localFilePtr);
#endif

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  KDDITelopAtom              *_kddiTelopElement;
  TsmlAtom                   *_pTsmlAtom;
#endif

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  void process_kddi_telop_text();
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

#if defined(FEATURE_QTV_SKT_MOD_MIDI)
  void process_mod_midi_atom();
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

  virtual int32 getSampleInfoError(video_fmt_stream_info_type *) { return 0; }

  virtual void process_video_fmt_info(video_fmt_status_type status,
                                      video_fmt_status_cb_info_type *info);

private:
  virtual void InitData();

protected:
  Mpeg4Player *m_pMpeg4Player;


public:
  uint32  aac_data_type;
};

#endif  // __Mpeg4File_H__

