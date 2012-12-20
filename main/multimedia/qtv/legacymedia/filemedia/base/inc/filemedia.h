#ifndef _FILEMEDIA_H_
#define _FILEMEDIA_H_
/* =======================================================================
                              filemedia.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/base/main/latest/inc/filemedia.h#19 $
$DateTime: 2010/02/21 21:35:19 $
$Change: 1187322 $


========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"

#include "media.h"
#include "filebase.h"

#include "ITrackSelectionPolicy.h"
#include "DefaultTrackSelectionPolicy.h"
#include "ReferenceCountedPointer.h"

#ifdef FEATURE_QTV_GENERIC_BCAST
#include "qtvmediatracklist.h"
#endif /* FEATURE_QTV_GENERIC_BCAST */

class Mpeg4Player;

class FileMedia : public Media
{
public:

#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
  FileMedia(  const OSCL_STRING &name,
              Mpeg4Player *pMpeg4Player,
              bool bPlayVideo =  false,
              bool bPlayAudio = false,
              bool bPlayText = false );
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_QTV_DRM_DCF
  // constructor for supporting DCF playback
  FileMedia(  dcf_ixstream_type inputStream,
              QtvPlayer::URNTypeT urnType,
              Mpeg4Player *pMpeg4Player,
              bool bPlayVideo =  false,
              bool bPlayAudio = false,
              bool bPlayText = false );

#endif

  FileMedia(unsigned char *pBuf, uint32 bufSize, Mpeg4Player *pMpeg4Player,
             bool bPlayVideo =  false, bool bPlayAudio = false,
             bool bPlayText = false
#if  defined (FEATURE_QTV_PSEUDO_STREAM) || \
     defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
           ,bool bPseudoStream = false
           ,uint32 wBufferOffset = 0
           ,uint32 wStartupTime = 0
           ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize = NULL
           ,QtvPlayer::FetchBufferedDataT FetchBufferedData = NULL
           ,QtvPlayer::InstanceHandleT handle = NULL
#endif  /* FEATURE_QTV_PSEUDO_STREAM ||
           FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
            );

  virtual ~FileMedia();

  virtual MediaSource GetSource() const
  {
    return FILE_SOURCE;
  }
  virtual FileBase * GetFile() const
  {
    return pBaseFile;
  }

  virtual void SetBufferDuration(int duration)
  {
    preroll = duration;
  }
  virtual long GetBufferDuration()
  {
    return preroll;
  }

  virtual uint32 GetDurationBuffered()
  {
    return 0;
  }

  // INFORMATION METHODS

  virtual bool GetSelectedTrackList(ITrackList **ppTrackList /* out */);

  virtual bool GetTitle(OSCL_STRING &oTitle) ;
  virtual bool GetAuthor(OSCL_STRING &oAuthor) ;
  virtual bool GetRating(OSCL_STRING &oRating) ;
  virtual bool GetCopyright(OSCL_STRING &oCopyright) ;
  virtual bool GetDescription(OSCL_STRING &oDescription) ;
  virtual bool GetPerf(OSCL_STRING &oPerformance);
  virtual bool GetGenre(OSCL_STRING &oGenre);
  virtual bool GetClsf(OSCL_STRING &oClsf);
  virtual bool GetKywd(OSCL_STRING &oKywd);
  virtual bool GetLoci(OSCL_STRING &oLoci);
  virtual bool GetOwner(OSCL_STRING &oOwner)
  {
    oOwner=NULL;
    return false;
  } //not available.
  virtual bool GetVersion(OSCL_STRING &oVersion) ;
  virtual bool GetCreationDate(OSCL_STRING &oCreationDate) ;

  virtual bool GetArtist(OSCL_STRING &oArtist);
  virtual bool GetSTitle(OSCL_STRING &oTitle);
  virtual bool GetAlbum(OSCL_STRING &oAlbum);
  virtual bool GetYrrc(OSCL_STRING &oYrrc);
  virtual bool GetInfo(OSCL_STRING &oInfo);
  virtual bool HasClipMetaData();

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#ifdef FEATURE_MP4_KDDI_META_DATA
  virtual unsigned long GetCopyProhibitionFlag() ;
  virtual unsigned long GetValidityEffectiveDate() ;
  virtual unsigned long GetValidityPeriod() ;
  virtual unsigned long GetNumberOfAllowedPlayBacks() ;
  virtual bool GetContentPropertyTitle(OSCL_STRING &oContentPropertyTitle) ;
  virtual bool GetContentPropertyCopyRight(OSCL_STRING &oContentPropertyCopyright) ;
  virtual bool GetContentPropertyAuthor(OSCL_STRING &oContentPropertyAuthor) ;
  virtual bool GetContentPropertyMemo(OSCL_STRING &oContentPropertyMemo) ;
  virtual unsigned long GetAuthorDLLVersion() ;
  virtual unsigned long GetEditFlags() ;
  virtual unsigned int GetRecordingMode() ;
  virtual unsigned long GetRecordingDate() ;
  virtual void GetDeviceName(uint8** oDeviceName) ;
  virtual void GetModelName(uint8** oModelName) ;
  virtual void GetEncoderInformation(uint8** oEncoderInformation) ;
  virtual void GetMuxInformation(uint8** oMuxInformation) ;
  // The following methods are for KDDI GPS atom support
  virtual uint16 GetGPSByteOrder();
  virtual uint32  getGPSVersionID();
  virtual uint32  getGPSLatitudeRef();
  virtual uint32  getGPSLongitudeRef();
  virtual uint32  getGPSAltitudeRef();
  virtual uint64 *GetGPSLatitudeArray();
  virtual uint64 *GetGPSLongitudeArray();
  virtual uint64 GetGPSAltitude();
  virtual uint64 *GetGPSTimeArray();
  virtual bool   GetGPSSurveyData(OSCL_STRING &oGPSSurveyData);
  virtual bool   GetPositioningMethod(OSCL_STRING &oGPSPositioningMethod);
  virtual bool   GetPositioningName(OSCL_STRING &oGPSPositioningName);
  virtual bool   GetGPSDate(OSCL_STRING &oGPSDate);
  virtual uint64 GetGPSExtensionMapScaleInfo();
#endif

  // TIMED TEXT SAMPLE METHODS
  // Returns the next timed text sample in presentation order
  virtual uint32 GetNextTextSample(MediaStatus &status, int idx, unsigned char *buf, int size);
  virtual unsigned long GetTimestampForCurrentTextSample(unsigned long track = 0);
  virtual void GetCurTextSampleInfo(uint32 *timeStamp, uint32 *timeDuration, uint32 *sampleDescIndex);
  virtual TextSampleEntry *getTextSampleEntryAt(uint32 trackId, uint32 index);
  virtual bool IsTextAvailable(MediaStatus* status = NULL)  ;
  virtual unsigned long GetMaxTextBufferSizeDB(int idx) ;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  // Returns true if text track is KDDI telop text type, false if 3GPP type
  virtual bool IsTelopPresent();
  // The following methods are for KDDI Telop Timed Text only:
  // Returns a pointer to the next telop element
  virtual TelopElement *GetNextTelopElementPtr();
  virtual TelopHeader *GetTelopHeader();
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

#ifdef FEATURE_QTV_SKT_MOD_MIDI
  virtual bool IsMidiDataPresent();
  virtual uint32 GetMidiDataSize();
  virtual uint32 GetMidiData(uint8 *pBuf, uint32 size, uint32 offset);

  virtual bool IsLinkDataPresent();
  virtual uint32 GetLinkDataSize();
  virtual uint32 GetLinkData(uint8 *pBuf, uint32 size);
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

  virtual bool IsDataPresent(QtvPlayer::DataT dType, uint32 track_id);
  virtual uint32 GetDataSize(QtvPlayer::DataT dType, uint32 offset);
  virtual uint32 GetData(QtvPlayer::DataT dType, uint8 *pBuf, uint32 size, uint32 offset);

   virtual bool isGenericAudioFileInstance();
   virtual bool isAviFileInstance();

#ifdef FEATURE_QTV_DIVX_DRM
  virtual void CopyDRMContextInfo(void*);
  virtual void GetClipDrmInfo(void*);
  virtual bool IsDRMProtection();
  virtual bool CommitDivXPlayback();
#endif

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  virtual unsigned long SetAudioBitRate(uint32 idx);
  virtual void setRepositioningAllowed(bool repositioningNotAllowed);
  virtual bool IsGenericAudioFileMediaInitialized();
  virtual bool IsGenericAudioFileRepositioned();
  virtual uint32 GetTimeStampFromMedia();
  virtual int GetNextGenericAudioSample
    (MediaStatus &status, unsigned char * buf, int size, uint32 offset, int *numSample=NULL, unsigned long track = 0, int channel=0);
  #endif /* #ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT */
  virtual bool IsRepositioningAllowed() ;
  virtual bool IsVideoOnlyPlaybackAllowed() ;
  virtual bool IsAudioOnlyPlaybackAllowed() ;

  virtual int GetDurationMsec();

  virtual MediaFileType GetFileType();

  virtual bool IsAudioAvailable(bool* bufUnderrun = NULL);
  virtual bool IsVideoAvailable();
  virtual uint32 GetAudioDurationAvailable();
  virtual uint32 GetVideoDurationAvailable();

  /*These 2 originally from windows media*/
  virtual unsigned long GetNumAudioChannels(int idx=0);
  virtual unsigned long GetAudioBitRate(int idx);
  virtual bool GetAudioTrackLanguage(OSCL_STRING &, uint32);
  virtual unsigned int UserCompare(bool &bError, int nAudID, int nVidID, int nTextID);

  virtual float GetVideoFrameRate(int idx);
  virtual unsigned long GetVideoBitRate(int idx);
  virtual unsigned long GetAudioBitsPerSample(int idx);

#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
  /* use these functions only for windows media audio */
  virtual unsigned long GetAudioCodecVersion(int idx);
  virtual unsigned long GetFixedAsfAudioPacketSize(int idx);
  virtual unsigned long GetAudioEncoderOptions(int idx);

  //Returns audio virtual packet size in bytes.
  virtual uint16        GetAudioVirtualPacketSize( int idx);

  //Returns true if asffile has started sending dummy bytes.
  virtual bool          isStartOfDummyBytes(int idx);

  //Returns estimated AUDIO ASF packet duration in msec.
  virtual long          GetAudioASFPacketDuration(int);

virtual uint16 GetAudioAdvancedEncodeOptions(int idx);
   virtual uint32 GetAudioAdvancedEncodeOptions2(int idx);
   virtual uint16 GetAudioChannelMask(int idx);
   virtual uint16 GetAudioArmDataReqThr(int idx);
   virtual uint8 GetAudioValidBitsperSample(int idx);
#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef FEATURE_QTV_AVI
  virtual long getAudioFrameDuration(int);
  virtual void  SetIDX1Cache(void*);
  virtual void* GetIDX1Cache();
#endif

  virtual void resetInitialization();
  virtual unsigned long GetMaxVideoBufferSizeDB(int trackNum = 0) ;
  virtual unsigned long GetMaxAudioBufferSizeDB(int trackNum = 0) ;
  virtual unsigned long GetMaxVideoFrameSize(int idx);
  virtual unsigned long GetMaxAudioFrameSize(int idx);
  virtual unsigned long GetMaxTextFrameSize(int idx);
#ifdef FEATURE_FILE_FRAGMENTATION
  virtual unsigned long GetNextFragMaxAudioBufferSizeDB(int trackNum = 0);
  virtual unsigned long GetNextFragMaxVideoBufferSizeDB(int trackNum = 0);
  virtual unsigned long GetNextFragMaxTextBufferSizeDB(int trackNum = 0);
  virtual void setPausedVideo( boolean bPauseVideo );
  virtual void setPausedAudio( boolean bPauseAudio );
  virtual void setPausedText( boolean bPauseText );
  virtual boolean getPausedVideo( void );
  virtual boolean getPausedAudio( void );
  virtual boolean getPausedText( void );
  virtual void resumeMedia( void );
#endif /* FEATURE_FILE_FRAGMENTATION */

  virtual int GetAudioDurationMsec() ;

  virtual uint32 SetVideoPosition(unsigned long pos, bool &bError, uint32 currentPosTimeStamp);
  virtual uint32 SetAudioPosition(unsigned long pos, bool &bError,uint32 currentPosTimeStamp);

#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
  virtual uint32 seekToSync(int, bool &, uint32 );
#endif /* FEATURE_QTV_REPOSITION_SYNC_FRAME */

  virtual void ResetVideoPlaybackPos();
  virtual Media::AACDataFormatType GetAACDataFormat(void);

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  virtual void DownloadDone();
#endif

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  virtual uint32 SetTextPosition(unsigned long pos,bool &bError,uint32 currentPosTimeStamp);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_FILE_FRAGMENTATION
  virtual bool ParseFragment( void );
  virtual uint16 GetParseFragmentNum( void );
  virtual uint16 GetReadFragmentNum( void );
#endif

#ifdef FEATURE_QTV_PSEUDO_STREAM
  virtual void UpdatePSBufferOffset( uint32 wBufferOffset);
  virtual bool ParsePseudoStream( void );
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  virtual void UpdateHTTPBufferOffset( uint32 wBufferOffset );
  virtual bool ParseHTTPStream( void );
  virtual bool HTTPCanPlayTracks(uint32 pbTime);
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  virtual unsigned long SetFileSize(uint32 idx);
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#ifdef FEATURE_FILE_FRAGMENTATION
  virtual uint32 RepositionVideoAccessPoint(int32 skipNumber, bool &bError ,uint32 currentPosTimeStampMsec);
  virtual uint32 RepositionAudioAccessPoint(int32 skipNumber, bool &bError,uint32 currentPosTimeStampMsec);
#endif /*FEATURE_FILE_FRAGMENTATION*/

  // VIDEO SAMPLE METHODS

  virtual int GetNextLayeredVideoSample(MediaStatus &status, int layer,
                                        unsigned char * buf, int size,
                                        uint32 bit_pos = 0, void **userdata = NULL);
  virtual unsigned long GetTimestampForCurrentLayeredVideoSample(int layer = 0);
  virtual unsigned long GetTimestampDeltaForCurrentLayeredVideoSample(int layer = 0);
  virtual unsigned long GetTimestampDeltaForCurrentAudioSample(int idx);

  virtual unsigned char *GetVOLHeader(int idx, int &size) ;
  virtual bool ResetLayeredVideoPlayback(int idx);
  virtual bool GetVideoDimensions(uint32 *pWidth, uint32 *pHeight, uint32 trackIdx=0);
  virtual void GetCurVideoSampleInfo(uint32 idx, uint32 *timeStamp, uint32 *timeDuration);

#ifdef FEATURE_H264_DECODER
  virtual int GetNextParamSetNAL(MediaStatus &status, int idx, unsigned char * buf, int size);
  virtual bool IsParamSetAvailable(int) { return true; }
  virtual bool ResetParamSetNAL(int trackIdx=0);
  virtual int  GetSizeOfNALLengthField(int trackIdx=0);
  virtual bool GetH264StartCodeDetectFlag(int trackIdx=0)
  {
    (void)trackIdx;
    return false; /* file media does not use H264 byte stream format */
  }
#endif /* FEATURE_H264_DECODER */

  // AUDIO SAMPLE METHODS

  // Returns the next audio sample in presentation order
  //WZeng, add "int channel" to support aac silence insertion when streaming, not used here, 06/05/02
  virtual int GetNextAudioSample
    (MediaStatus &status, unsigned char * buf, int size, int *numSample=NULL, unsigned long track = 0, int channel=0);


  virtual void SetAudioPlayerData( const void *client_data);
  virtual void SetVideoPlayerData( const void *client_data);
  virtual void SetTextPlayerData( const void *client_data);


  virtual unsigned long GetTimestampForCurrentAudioSample(unsigned long track = 0);
  virtual unsigned char *GetAACHeader(int idx, int &size);

  //ERROR REPORT  METHODS

  //Get the error code from the previous request.  The caller inputs
  //the context for logging purposes.
  virtual bool CheckError(const Common::Mp4ErrorContext context) ;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  virtual bool setPDS2DloadObj( PVDL *pPDS2Obj);
  virtual void setVideoPos(uint32 videoPos);
  virtual void setAudioPos(uint32 audioPos);
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

  virtual void SetLoopTrackFlag(bool bLoop);

  virtual bool isAudioPresentInClip();
#ifdef FEATURE_QTV_PDCF
  virtual QtvPlayer::EncryptionTypeT getEncryptionType();
  virtual QtvPlayer::EncryptionTypeT getEncryptionType(uint32);
#endif /* FEATURE_QTV_PDCF */
#ifdef FEATURE_QTV_WM_DRM_API
  virtual bool RegisterDRMDecryptMethod(QtvPlayer::DRMDecryptMethodT pDRMDecriptFunction, void *pClientData);
#endif /* FEATURE_QTV_WM_DRM_API */

private:

  void HandleSelectedTrackList();

  FileBase *pBaseFile;
  int nMp4Error;
  Media::AACDataFormatType  m_AACDataFormatType;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  long nTimestampA,nTimestampV,nTimestampT;
#else
  long nTimestampA,nTimestampV;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  int32 curAudioSDI;
  char evrcFrameType;
  char amrFrameType;
  bool lockTrackList;
  enum MediaReqType
  {
    VIDEO_REQUEST,
    AUDIO_REQUEST,
    TEXT_REQUEST
  };

  OSCL_STRING m_fileName;
  unsigned char *m_pFileBuf;
  uint32 m_dwFileBufSize;
  bool m_bPlayVideo;
  bool m_bPlayAudio;
  bool m_bPlayText;
  ReferenceCountedPointer<DefaultTrackSelectionPolicy>
    m_defaultTrackSelectionPolicy;

  bool m_bMediaInitialized;
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  bool bAACPaddingSent;
#endif

#if defined (FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  bool m_bPseudoStream;
  virtual bool isHTTPStreaming( void )
  {
	 return m_bPseudoStream;
  }
#endif /* FEATURE_QTV_PSEUDO_STREAM ||
          FEATURE_QTV_3GPP_PROGRESSIVE_DNLD  */

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

  const void *AudioPlayerDataPtr;
  const void *VideoPlayerDataPtr;
  const void *TextPlayerDataPtr;
  int32 getNextMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 &index, MediaReqType type);
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  int32 getNextGenericAudioMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 offset, uint32 &index, MediaReqType type);
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
  int preroll;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    // Minumum duration of time (msec) accumulated in buffer required to
	// return true when calling IsVideo/AudioAvailable.
  static const int32 MIN_BUFFER_DURATION;

	// States:
	// Play - Downloaded data is above minimum time interval of MIN_BUFFER_DURATION (msec).
	// Buffer - Do we need to buffer data?
  enum State {Play, Buffer};
  State state;
  PVDL *pPVDL;
  int audioPos;
  int videoPos;
  bool bDownloadDone;
  bool bCheckRate;
  bool CheckDataRate(int playtime);
#endif

  void SetError();
  void FileInitData();
  int MapTrackToIndex(int nTrackID);
};

#endif /* _FILEMEDIA_H_ */

