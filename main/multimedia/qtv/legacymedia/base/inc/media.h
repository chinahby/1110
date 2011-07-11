#ifndef _MEDIA_H_
#define _MEDIA_H_
/* =======================================================================
                              media.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/base/main/latest/inc/media.h#35 $
$DateTime: 2010/04/20 22:35:42 $
$Change: 1273913 $


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
#include "qtvsystem.h"
#include "qcplayer.h"
#include "filebase.h"


#ifdef __cplusplus
extern "C" {
#endif
#include "cmx.h"
#ifdef __cplusplus
} // extern "C"
#endif
#include "QtvConfig.h"
#include "ReferenceCountedPointer.h"

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
class PVDL;
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define MAX_MIME_TYPE_STRING_LENGTH 32

//conversion factor to get msec
#define TIMESCALE_BASE 1000

//duration of the frame block in audio packet (for AMR, EVRC, QCELP)
#define DURATION_OF_FRAME_BLOCK 20

#define EVRC_SAMPLE_HEADER_SIZE 1
#define QCELP_SAMPLE_HEADER_SIZE 1
#define AMR_SAMPLE_HEADER_SIZE 1
#define AMRWBP_SAMPLE_HEADER_SIZE 8
#define AAC_SAMPLES_PER_FRAME 1024

//Content version is 1 byte for major version, 1 byte for minor version (3.0)
#define PLAYABLE_CONTENT_VERSION 0x0400
//----------------------------------------------------------------------
// CAI & WMF Constant Definitions
//----------------------------------------------------------------------
#define GSM_AMR_CONTINUATION_FRAME      0x80
#define GSM_AMR_QUALITY_BIT                     0x04
#define GSM_AMR_FRAME_TYPE_POSITION     3

#define FRAGMENT_BOUNDARY -1
#define INSUFFICIENT_BUFFER_SIZE -2
#define FRAGMENT_CORRUPT -4
#define INSUFFICIENT_DATA -5
#define READ_FAILURE -6
#define FAILED_TO_DECRYPT -7

#define ZERO_SAMPLE_SIZE (-3)
#define MAX_AUDIO_BSAC_TRACKS  32
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
class ITrackList;
class ITrackSelectionPolicy;
class Mpeg4Player;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

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
  Media

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
class Media
{
public:

  Media();

  virtual ~Media(); // it's virtual so the derived class destructor gets called

  //Media source
  enum MediaSource
  {
    FILE_SOURCE
    , STREAM_SOURCE
    , DOWNLOAD_SOURCE
    , BCAST_ISDB_SOURCE
    , BCAST_FLO_SOURCE
  };

  //Media components
  enum MediaFileType {
    CONTENT_AUDIO,
    CONTENT_VIDEO,
    CONTENT_AUDIO_VIDEO,
    CONTENT_VIDEO_STILL_IMAGE,
    CONTENT_AUDIO_VIDEO_STILL_IMAGE,
    CONTENT_VIDEO_TEXT,
    CONTENT_AUDIO_TEXT,
    CONTENT_AUDIO_VIDEO_TEXT,
    CONTENT_VIDEO_STILL_IMAGE_TEXT,
    CONTENT_AUDIO_VIDEO_STILL_IMAGE_TEXT,
    CONTENT_TEXT,
    CONTENT_UNKNOWN
  };

//Status of media data fetches
enum MediaStatus {
    DATA_OK,
    DATA_END,
    DATA_UNDERRUN,
    DATA_NULLFILL,
    DATA_ERROR,
    DATA_INIT,
    DATA_FRAGMENT,
    DATA_REALLOCATE_BUFFER,
    DATA_SKIP,
    DATA_INSUFFICIENT
};

  //If any one is adding any new codec here then they should update this
  //buffer AudioMgr::NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER in audioMgr.cpp
  //otherwise if anybody try to play with that codec then leads to crash.
  //Type of codec
  enum CodecType {
    UNKNOWN_CODEC,
    EVRC_CODEC,
    EVRC_NB_CODEC,
    EVRC_WB_CODEC,
    QCELP_CODEC,
    AAC_CODEC,
    GSM_AMR_CODEC,
    MPEG4_CODEC,
    H263_CODEC,
    H264_CODEC,
    STILL_IMAGE_CODEC,
    TIMED_TEXT_CODEC,
    GENERIC_TEXT_CODEC,
    JPEG_CODEC,
    MP3_CODEC,
    WMA_CODEC,
    WMV1_CODEC,
    WMV2_CODEC,
    WMV3_CODEC,
    WMA_PRO_CODEC,
    WMA_PRO_PLUS_CODEC,
    OSCAR_CODEC,
    CONC_CODEC, // audio codec of concurrent image not available for Qtv
    COOK_CODEC,
    SIPR_CODEC,
    RV30_CODEC,
    RV40_CODEC,
    AMR_WB_CODEC,
    AMR_WB_PLUS_CODEC,
    NONMP4_MP3_CODEC,
    QCP_CODEC,
    MIDI_CODEC,
    NONMP4_AAC_CODEC,
    NONMP4_AMR_CODEC,
    DIVX311_CODEC,
    BSAC_CODEC,
    FGV_CODEC_NB,
    FGV_CODEC_WB,
    AC3_CODEC,
    PCM_CODEC,
    STILL_IMAGE_H263_CODEC,	
    MAX_CODEC
  };

  enum AACDataFormatType
  {
    AAC_DATA_FORMAT_ADTS,
    AAC_DATA_FORMAT_RAW,
    AAC_DATA_FORMAT_PSEUDO_RAW,
    BSAC_DATA_FORMAT_ADTS,
    AAC_DATA_FORMAT_LOAS,
    AAC_DATA_FORMAT_UNKNOWN /*Please add all the valid format information above this line*/
  };

  typedef struct
  {
    uint32          trackID;
    uint32          actualTrackID;
    CodecType       codec;
    int32           bitrate;
    int32           numAudioChannels;
    int             dependsOnID;
    int16           contentVersion;
    int32           frameWidth;
    int32           frameHeight;
    bool            isPlayable;
    bool            isTrackSelected;
    QtvPlayer::EncryptionTypeT encryptionType;
    char            language[QTV_MAX_LANGUAGE_BYTES];
  } TrackInfo;

  //Audio parameters
  static const int EVRC_SAMPLES_PER_FRAME;
  static const int AMR_SAMPLES_PER_FRAME;
  static int       QCELP_SAMPLES_PER_FRAME; // This is not a constant on purpose !! (It can vary)
  static const int MP3_SAMPLES_PER_FRAME;
  static const int AMR_WB_SAMPLES_PER_FRAME;
  static const int AMR_WB_PLUS_SAMPLES_PER_FRAME;
  static const int EVRC_NB_SAMPLES_PER_FRAME;
  static const int EVRC_WB_SAMPLES_PER_FRAME;
  //Static methods to create a media data interface

  // Open local file stream
#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
  static Media *OpenFile(  const OSCL_STRING &name,
                           Mpeg4Player *pMpeg4Player,
                           bool bPlayVideo = false,
                           bool bPlayAudio = false,
                           bool bPlayText = false );
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_QTV_DRM_DCF
  // To create media instance to support DCF playback
  static Media * OpenFile( dcf_ixstream_type inputStream,
                          QtvPlayer::URNTypeT urnType,
                          Mpeg4Player *pMpeg4Player,
                          bool bPlayVideo,
                          bool bPlayAudio,
                          bool bPlayText);

#endif

  // Open buffer stream
  static Media * OpenFile(  unsigned char * pBuf, uint32 bufSize,
                            Mpeg4Player *pMpeg4Player,
                            bool bPlayVideo = false,
                            bool bPlayAudio = false,
                            bool bPlayText = false
#if (defined (FEATURE_QTV_PSEUDO_STREAM) || \
defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
                  ,bool bPseudoStream = false
                  ,uint32 wBufferOffset = 0
                  ,uint32 wStartupTime = 0
                  ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize = NULL
                  ,QtvPlayer::FetchBufferedDataT FetchBufferedData = NULL
                  ,QtvPlayer::InstanceHandleT handle = NULL
#endif /* FEATURE_QTV_PSEUDO_STREAM */
  /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
                          );

  virtual bool isHTTPStreaming ( void ) { return false; }

#ifdef FEATURE_QTV_PSEUDO_STREAM
  virtual void UpdatePSBufferOffset(uint32) { }
  virtual bool ParsePseudoStream() { return true; }
#endif
#ifdef FEATURE_FILE_FRAGMENTATION
  virtual uint16 GetParseFragmentNum(void) { return 1; }
#endif /* FEATURE_FILE_FRAGMENTATION */
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  virtual void UpdateHTTPBufferOffset( uint32 wBufferOffset )
  {
    QTV_USE_ARG1(wBufferOffset);
  }
  virtual bool ParseHTTPStream( void ) { return false; }
  virtual bool HTTPCanPlayTracks(uint32 pbTime)
  {
    QTV_USE_ARG1(pbTime);
    return false;
  }
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  virtual unsigned long SetFileSize(uint32 idx);
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#ifdef FEATURE_QTV_WM_DRM_API
  virtual bool RegisterDRMDecryptMethod(QtvPlayer::DRMDecryptMethodT , void *) { return false; }
#endif /* FEATURE_QTV_WM_DRM_API */

#ifdef FEATURE_QTV_GENERIC_BCAST
  static Media* ConstructGenericBcastMedia(void* pContext,
                Common::GenericBcastMediaSourceID eID);

  // Gets the Media Information. This  API just passes key information necessary to identify the information
  // Which need to flow back to the application. Media source should use the asynchrnous mechanism to send
  // the result back.
#endif /* FEATURE_QTV_GENERIC_BCAST */

       // Set AAC ADTS Configuration Data
       virtual void SetAACADTSConfigData(unsigned int,unsigned int) { return; }

  // Gets the Media Information
  virtual bool   GetMediaInfo(void **,unsigned int *) { return true; }

  // Sets  the Media Information
  virtual bool   SetMediaInfo(void *,unsigned int) { return true; }

#ifdef FEATURE_QTV_BCAST_FLO
#error code not present
#endif /* FEATURE_QTV_BCAST_FLO */

  virtual int FillCMXAACConfig(cmx_af_aac_spec_type *) { return SUCCESS; }

  //Method to get information about this particular interface.
  virtual MediaSource GetSource() const = 0;
  virtual FileBase *GetFile() const { return NULL; }

  // Streaming specific functions
  virtual void SetBufferDuration(int duration) = 0;
  virtual long GetBufferDuration() = 0;
  virtual int getCMXMaxAheadInMsec()
  {
      return 0;
  }
  virtual void SetMinBufferDuration(int duration)
  {
    QTV_USE_ARG1(duration);
  }

  //INFORMATION METHODS

  // Retrieve the list of preselected tracks. Put the result in *ppTrackList.
  // The track list is automatically AddRefed, so remember to Release() it
  // when finished.
  virtual bool GetSelectedTrackList(ITrackList **ppTrackList /* out */) = 0;

  virtual bool SetTrackSelectionPolicy(ITrackSelectionPolicy *policy /* in */);

  virtual bool GetTitle(OSCL_STRING &oTitle) = 0;
  virtual bool GetAuthor(OSCL_STRING &oAuthor) = 0;
  virtual bool GetRating(OSCL_STRING &oRating) = 0;
  virtual bool GetCopyright(OSCL_STRING &oCopyright) = 0;
  virtual bool GetDescription(OSCL_STRING &oDescription) = 0;
  virtual bool GetOwner(OSCL_STRING &oOwner) = 0;
  virtual bool GetVersion(OSCL_STRING &oVersion) = 0;
  virtual bool GetCreationDate(OSCL_STRING &oCreationDate) = 0;

  virtual bool GetArtist(OSCL_STRING &oArtist) = 0;
  virtual bool GetSTitle(OSCL_STRING &oTitle) = 0;
  virtual bool GetAlbum(OSCL_STRING &oAlbum) = 0;
  virtual bool GetInfo(OSCL_STRING &oInfo) = 0;

  virtual bool GetPerf(OSCL_STRING &oPerformance) = 0;
  virtual bool GetGenre(OSCL_STRING &oGenre) = 0;
  virtual bool GetClsf(OSCL_STRING &oClsf) = 0;
  virtual bool GetKywd(OSCL_STRING &oKywd) = 0;
  virtual bool GetLoci(OSCL_STRING &oLoci) = 0;
  /*Not making this a pure virtual, as there could be
    problems with the instantiation of stream media,
    which does not have an implementation of Yrrc*/
  virtual bool GetYrrc(OSCL_STRING &oYrrc)
  {
    oYrrc = "";
    return false;
  };

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  virtual int64 ConvertTextTimestamp(unsigned long timestamp,int idx);
  virtual long GetTextTimescale(int idx=0) {return textTimescale[idx];}
  virtual bool HasClipMetaData() { return false; }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  virtual bool IsLive() { return false; }

#ifdef FEATURE_QTV_MCAST
  virtual bool IsMcast() { return false; }
#endif /* FEATURE_QTV_MCAST */

  virtual bool IsRepositioningAllowed() { return false; }
  virtual bool IsVideoOnlyPlaybackAllowed() { return true; }
  virtual bool IsAudioOnlyPlaybackAllowed() { return true; }
  virtual int GetDurationMsec() = 0;
  virtual MediaFileType GetFileType() = 0;

  // Note! This is not implemented correctly except for in streamMedia!
  virtual bool IsWindowsMedia() { return false; }

  virtual bool ShouldEndPlaybackImmediately() { return false; }

  virtual int GetNumberOfAudioTracks() {return (int) numAudioTracks;}
  virtual int GetNumberOfVideoTracks() {return (int) numVideoTracks;}
  virtual void SetAACSamplesPerFrame(unsigned int SamplesPerFrame) { audioSamplesPerFrame = aacSamplesPerFrame = SamplesPerFrame; }

  virtual bool IsAudioPresentInClip() { return false; }
  virtual bool IsVideoPresentInClip() { return false; }

#ifdef FEATURE_QTV_PDCF
  virtual QtvPlayer::EncryptionTypeT getEncryptionType() {return QtvPlayer::ENCRYPT_NONE;};
  virtual QtvPlayer::EncryptionTypeT getEncryptionType(uint32) {return QtvPlayer::ENCRYPT_NONE;};
#endif /* FEATURE_QTV_PDCF */

#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  virtual void SetAacFrameMarkerNeeded(bool needed) { aacFrameMarkerNeeded = needed; }
  virtual bool GetAacFrameMarkerNeeded() { return aacFrameMarkerNeeded; }
#endif /* FEATURE_MP4_AAC_FRAME_MARKER */

  virtual bool GetTrackList(ITrackList **ppTrackList);

  virtual CodecType GetAudioCodecType() {return audioCodecType;}
  virtual CodecType GetVideoCodecType() {return videoCodecType;}
  virtual void setAACChannelMode(int mode) {AACChannelMode = mode;}
  virtual int  getAACChannelMode() { return AACChannelMode; }
  virtual long GetAudioTimescale(int idx=0) {idx = 0; return audioTimescale[idx];}
  virtual long GetVideoTimescale(int idx=0) {idx = 0; return videoTimescale[idx];}

  virtual long GetAudioSamplesPerFrame() {return audioSamplesPerFrame;}
  virtual long GetAudioSamplingFreq(int idx=0) {return audioSamplingFreq[idx];}
  virtual void SetAudioSamplingFreq(uint32 Freq, int idx=0) {audioSamplingFreq[idx]=Freq;}

  virtual unsigned long GetNumAudioChannels(int idx=0);
  virtual unsigned long GetAudioBitRate(int idx=0);
  virtual float GetVideoFrameRate(int idx=0);
  virtual unsigned long GetVideoBitRate(int idx=0);

  virtual unsigned long GetAvgAudioBitRate(int idx=0);
  virtual unsigned long GetAvgVideoBitRate(int idx=0);
  virtual bool GetAudioTrackLanguage(OSCL_STRING &, uint32);
  virtual unsigned int UserCompare(bool &, int, int, int){return 0;}
  virtual unsigned long GetAudioBitsPerSample(int idx=0);
#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
  /* use these functions only for windows media audio */
  virtual unsigned long GetAudioCodecVersion(int idx=0);
  virtual unsigned long GetFixedAsfAudioPacketSize(int idx=0);
  virtual unsigned long GetAudioEncoderOptions(int idx=0);

  //Returns audio virtual packet size in bytes.
  virtual uint16        GetAudioVirtualPacketSize(int){return 0;}

  //Returns true if asffile has started sending dummy bytes.
  virtual bool          isStartOfDummyBytes(int){return false;}

  //Returns estimated AUDIO ASF packet duration in msec.
  virtual long          GetAudioASFPacketDuration(int){return 0;}
  virtual uint16 GetAudioAdvancedEncodeOptions(int idx = 0);
  virtual uint32 GetAudioAdvancedEncodeOptions2(int idx = 0);
  virtual uint16 GetAudioChannelMask(int idx = 0);
  virtual uint16 GetAudioArmDataReqThr(int idx = 0);
  virtual uint8 GetAudioValidBitsperSample(int idx = 0);

#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef FEATURE_QTV_AVI
  virtual long  getAudioFrameDuration(int){return 0;}
  virtual void  SetIDX1Cache(void*){}
  virtual void* GetIDX1Cache(){return NULL;}
#endif

  virtual void resetInitialization(){};
  virtual bool IsAudioAvailable(bool *bufUnderrun = NULL) = 0;

  virtual uint32 GetAudioDurationAvailable() = 0;
  virtual uint32 GetVideoDurationAvailable() = 0;

  virtual bool IsVideoAvailable() = 0;
  virtual bool IsMinAudioBuffered(void) { return true; }

  virtual void AbortAudio(bool bError = true) { QTV_USE_ARG1(bError); }
  virtual void AbortVideo(bool bError = true) { QTV_USE_ARG1(bError); }

  virtual unsigned long GetMaxVideoBufferSizeDB(int idx = 0) = 0;
  virtual unsigned long GetMaxAudioBufferSizeDB(int idx = 0) = 0;
  virtual unsigned long GetMaxVideoFrameSize(int idx = 0);
  virtual unsigned long GetMaxAudioFrameSize(int idx = 0);
  virtual unsigned long GetMaxTextFrameSize(int idx = 0);

#ifdef FEATURE_FILE_FRAGMENTATION
  virtual unsigned long GetNextFragMaxAudioBufferSizeDB(int idx = 0);
  virtual unsigned long GetNextFragMaxVideoBufferSizeDB(int idx = 0);
  virtual unsigned long GetNextFragMaxTextBufferSizeDB(int idx = 0);
  virtual void setPausedVideo(boolean) { }
  virtual void setPausedAudio(boolean) { }
  virtual void setPausedText(boolean) { }
  virtual boolean getPausedVideo( void ) { return false; }
  virtual boolean getPausedAudio( void ) { return false; }
  virtual boolean getPausedText( void ) { return false; }
  virtual void resumeMedia( void ) {  }
#endif /*FEATURE_FILE_FRAGMENTATION*/

  virtual int GetAudioDurationMsec() = 0;

  //Timestamp conversion
  virtual int64 ConvertAudioTimestamp(unsigned long timestamp, bool convert=true, int idx=0);
  virtual int64 ConvertVideoTimestamp(unsigned long timestamp,int idx);

  virtual uint32 SetVideoPosition(unsigned long pos, bool &bError, uint32 inputTimeStamp) = 0;
  virtual uint32 SetAudioPosition(unsigned long pos, bool &bError, uint32 currentPosTimeStamp) = 0;

#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
  virtual uint32 seekToSync(int offset, bool &bError, uint32 currentTimeStamp)
  {
    QTV_USE_ARG1(offset);
    QTV_USE_ARG1(bError);
    QTV_USE_ARG1(currentTimeStamp);

    return 0;
  }
#endif /* FEATURE_QTV_REPOSITION_SYNC_FRAME */

  virtual void ResetVideoPlaybackPos() { }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  virtual bool IsTextAvailable(MediaStatus *status = NULL) = 0;
virtual uint32 SetTextPosition(unsigned long pos,bool &bError,uint32 currentPosTimeStamp) = 0;
  virtual void GetCurTextSampleInfo(uint32 *timeStamp, uint32 *timeDuration, uint32 *sampleDescIndex) = 0;
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  virtual bool IsTelopPresent() { return false; }
  virtual TelopHeader *GetTelopHeader() { return NULL; }
  virtual TelopElement *GetNextTelopElementPtr() { return NULL; }
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

  virtual unsigned long GetMaxTextBufferSizeDB(int idx) = 0;
  virtual uint32 GetNextTextSample(MediaStatus &status, int idx, unsigned char *buf, int size) = 0;
  virtual TextSampleEntry *getTextSampleEntryAt(uint32 trackId, uint32 index) = 0;
  int GetNumberOfTextTracks() {return (int) numTextTracks;}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */


#ifdef FEATURE_QTV_SKT_MOD_MIDI
  virtual bool IsMidiDataPresent() { return false; }
  virtual uint32 GetMidiDataSize() { return 0; }
  virtual uint32 GetMidiData(uint8 *pBuf, uint32 size, uint32 offset)
  {
    QTV_USE_ARG1(pBuf);
    QTV_USE_ARG1(size)
    QTV_USE_ARG1(offset);

   return 0;
  }

  virtual bool IsLinkDataPresent() { return false; }
  virtual uint32 GetLinkDataSize() { return 0; }
  virtual uint32 GetLinkData(uint8 *, uint32) { return 0; }
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

  virtual bool IsDataPresent(QtvPlayer::DataT, uint32) {return false;}
  virtual uint32 GetDataSize(QtvPlayer::DataT, uint32){return 0;}
  virtual uint32 GetData(QtvPlayer::DataT, uint8 *, uint32, uint32)
  {return 0;}
  virtual bool isGenericAudioFileInstance() { return false; }

  virtual bool isAviFileInstance() { return false;}
#ifdef FEATURE_QTV_DIVX_DRM
  virtual void CopyDRMContextInfo(void*){}
  virtual void GetClipDrmInfo(void*){}
  virtual bool IsDRMProtection(){return false;}
  virtual bool CommitDivXPlayback(){ return false;}
#endif

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  virtual unsigned long SetAudioBitRate(uint32 idx);
  virtual void setRepositioningAllowed(bool) { }
  virtual bool IsGenericAudioFileMediaInitialized(){ return false; }
  virtual bool IsGenericAudioFileRepositioned(){ return false; };
  virtual uint32 GetTimeStampFromMedia();

  virtual int GetNextGenericAudioSample(MediaStatus &status,
    unsigned char *buf, int size, uint32 offset, int *numSample = NULL,
    unsigned long idx = 0, int channel = 0)
  {
    QTV_USE_ARG1(buf);
    QTV_USE_ARG1(size);
    QTV_USE_ARG1(offset);
    QTV_USE_ARG1(numSample);
    QTV_USE_ARG1(idx);
    QTV_USE_ARG1(channel);

    status = DATA_ERROR;
    return 0;
  }

#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT*/


  virtual void SetAudioPlayerData (const void *client_data)
  {
    QTV_USE_ARG1(client_data);
  }

  virtual void SetVideoPlayerData (const void *client_data)
  {
    QTV_USE_ARG1(client_data);
  }

  virtual void SetTextPlayerData  (const void *client_data)
  {
    QTV_USE_ARG1(client_data);
  }


#ifdef FEATURE_FILE_FRAGMENTATION
virtual uint32 RepositionVideoAccessPoint(int32 skipNumber, bool &bError ,uint32 currentPosTimeStampMsec);
virtual uint32 RepositionAudioAccessPoint(int32 skipNumber, bool &bError,uint32 currentPosTimeStampMsec);
#endif /*FEATURE_FILE_FRAGMENTATION*/

//VIDEO SAMPLE METHODS

  virtual int GetNextLayeredVideoSample(MediaStatus &status, int idx,
                                        unsigned char * buf, int size,
                                        uint32 buf_pos = 0, void **userdata = NULL) = 0;
  virtual bool ResetLayeredVideoPlayback(int idx) = 0;
  virtual unsigned long GetTimestampForCurrentLayeredVideoSample(int idx = 0) = 0;
  virtual unsigned long GetTimestampDeltaForCurrentLayeredVideoSample(int layer = 0);
  virtual unsigned long GetTimestampDeltaForCurrentAudioSample(int idx = 0);
  virtual unsigned char *GetVOLHeader(int idx, int &size) = 0;
  virtual bool GetVideoDimensions(uint32 *pWidth, uint32 *pHeight, uint32 trackIdx=0) = 0;
  virtual void GetCurVideoSampleInfo(uint32 track_id, uint32 *timeStamp, uint32 *timeDuration) = 0;

#ifdef FEATURE_H264_DECODER
  virtual int GetNextParamSetNAL(MediaStatus &status, int idx, unsigned char * buf, int size) = 0;
  virtual bool IsParamSetAvailable(int idx) = 0;
  virtual bool ResetParamSetNAL(int idx) = 0;
  virtual int  GetSizeOfNALLengthField(int idx) = 0;
  // Accessor method to obtain the start code detection flag of the H264 media
  virtual bool  GetH264StartCodeDetectFlag(int idx) = 0;
#endif /* FEATURE_H264_DECODER */

//AUDIO SAMPLE METHODS

// Returns the next audio sample in presentation order
  //WZeng, add "int channel" to support aac silence insertion when streaming, 06/05/02
  virtual int GetNextAudioSample(MediaStatus &status, unsigned char * buf,
    int size, int *numSample=NULL, unsigned long idx = 0, int channel = 0) = 0;

  virtual unsigned long GetTimestampForCurrentAudioSample(unsigned long idx = 0) = 0; //Get the error code from the previous request.
  virtual unsigned char *GetAACHeader(int idx, int &size) = 0;

//ERROR REPORT  METHODS

  //Get the error code from the previous request.  The caller inputs
  //the context for logging purposes.
  virtual bool CheckError(const Common::Mp4ErrorContext context) = 0;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  virtual bool setPDS2DloadObj( PVDL *) { return false; };
  virtual void setVideoPos(uint32 ){};
  virtual void setAudioPos(uint32 ){};
#endif /*  FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

  // In the case of streaming video-only clips, if the delay between
  // frames is too big, this probably indicates missing frames due to
  // packet loss, so adjust the delay so that the next frame can be
  // displayed earlier.
  virtual long AdjustInterFrameDelay(long delay, int32) { return delay; }

#ifdef FEATURE_QTV_STREAM_RECORD
#ifdef FEATURE_QTV_TEXT_RECORD
  virtual void initTextTrackRecording(QtvPlayer::ClipInfoT &info,
                                      uint32 tscale)
  {
    memset( &info, 0, sizeof(info) );

    QTV_USE_ARG1(tscale);
  }

  virtual void setRecordingStreamNo(bool, bool, bool, bool) { }
#else
  virtual void setRecordingStreamNo(bool, bool, bool) { }
#endif
  virtual void initAudioTrackRecording(QtvPlayer::ClipInfoT& info,
    uint32 audiortpts)
  {
    memset( &info, 0, sizeof(info) );

    QTV_USE_ARG1(audiortpts);
  }

  virtual void initVideoTrackRecording(QtvPlayer::ClipInfoT& info, uint32 tscale)
  {
    memset( &info, 0, sizeof(info) );

    QTV_USE_ARG1(tscale);
  }

  virtual void initEnhancedVideoTrackRecording(QtvPlayer::ClipInfoT& info, uint32 tscale)
  {
    memset( &info, 0, sizeof(info) );

    QTV_USE_ARG1(tscale);
  }

  virtual bool qtvio_start_file_writer(char *, bool,
                                       void(*cb)(int,void*),
                                       void* client_data)
  {
    QTV_USE_ARG1(cb);
    QTV_USE_ARG1(client_data);

    return false;
  }

  virtual bool qtvio_stop_file_writer() { return false; }
  virtual void qtvio_pause_file_writer() { }

  virtual void recordVideoFrame(bool isIframe, unsigned long timestamp)
  {
    QTV_USE_ARG1(isIframe);
    QTV_USE_ARG1(timestamp);
  }

  virtual void recordEnhLayerVideoFrame(bool isIframe, unsigned long timestamp)
  {
    QTV_USE_ARG1(isIframe);
    QTV_USE_ARG1(timestamp);
  }

  virtual void SetStrRecEFSReservedSpace(uint32 resSpaceInBytes)
  {
    QTV_USE_ARG1(resSpaceInBytes);
  }

  virtual uint32 getRecordedClipDuration(void)
  {
    return 0;
  }
#endif /* FEATURE_QTV_STREAM_RECORD */

  virtual bool GetLaunchURL(OSCL_STRING&)
  {
    return false;
  }

  virtual EndOfClipActionT GetEndOfClipAction() { return NO_END_OF_CLIP_ACTION; }
  // Determine whether the player supports this track.
  static bool CanPlay(const TrackInfo *pTrackInfo);

#ifdef FEATURE_QTV_GENERIC_BCAST
  static bool CanCodecPlay(TrackInfo *pTrackInfo);
#endif

  // Determine if the codec is audio.
  static bool IsAudioCodec(CodecType codec);
  // Determine if the codec is video.
  static bool IsVideoCodec(CodecType codec);
  // Determine if the codec is text.
  static bool IsTextCodec(CodecType codec);

#ifdef FEATURE_QTV_WINDOWS_MEDIA
  // Determine if the codec is Windows Media audio or video.
  static bool IsWindowsMediaCodec(CodecType codec);
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

  virtual uint32 GetAudioTrackID(int index);
  virtual uint32 GetVideoTrackID(int index);
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  virtual uint32 GetTextTrackID(int index);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  bool bLoopTrackFlag;
  /* Used to store the timestamp of the last video frame in the video track.
      This value is then used to offset the time stamp of successive video frame
      (i.e. first video frame with timestamp 0) when the playback restarts.*/
  uint32 videoLoopTrackTimeOffset;
  uint32 audioLoopTrackTimeOffset;
  uint32 textLoopTrackTimeOffset;
  virtual void SetLoopTrackFlag(bool bLoop) {bLoopTrackFlag = bLoop;}

  virtual AACDataFormatType GetAACDataFormat(void);

  //enum containing the possible request from Media layer
  typedef enum
  {
    LOG_CUMULATIVE_CLIP_STATS = 1,
    SET_PREROLL_RENDERER = 2,
    LOG_FRAME_INFO = 3,
    OEM_LAYER_INFO = 4,
    AUDIO_CHANNEL_SWITCH_FRAME = 5
    //Add more request on need
  } MediaLayerRequest;

  //Function pointer to pass Media Layer request to Mpeg4Player
  typedef void (*pfnMediaPlayerCb)(MediaLayerRequest reqType, Mpeg4Player *pMpeg4Player,
                                   uint32 reqData1, uint32 reqData2);

  virtual void SetMediaPlayerCb(pfnMediaPlayerCb MediaPlayerCb);

  Mpeg4Player *m_pMpeg4Player;

protected:

  void BaseInitData();

  // Mark tracks selected.
  // Use the user-supplied policy if present, otherwise use the fallback
  // policy.
  bool SelectTracks(ITrackList *trackList /* in */,
                    ITrackSelectionPolicy *fallbackPolicy = NULL /* in */);

  static MediaFileType GetFileTypeByComponents(bool hasAudio, bool hasVideo,
                                               bool bHasStillImage
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
                                               , bool hasText = false
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
                                   );

  //Audio sample durations.
  unsigned int aacSamplesPerFrame;
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  bool aacFrameMarkerNeeded;
#endif /* FEATURE_MP4_AAC_FRAME_MARKER */

  CodecType audioCodecType;
  CodecType videoCodecType;
  int       AACChannelMode;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  CodecType textCodecType;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

int audioSamplesPerFrame;
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
  uint32 numAudioTracks;
  uint32 numVideoTracks;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  uint32 numTextTracks;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  uint32 audioTrackIDs[COMMON_MAX_LAYERS];
  uint32 audioTimescale[COMMON_MAX_LAYERS];
  uint32 audioSamplingFreq[COMMON_MAX_LAYERS];
  uint32 videoTrackIDs[COMMON_MAX_LAYERS];
  uint32 videoTimescale[COMMON_MAX_LAYERS];
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  uint32 textTrackIDs[COMMON_MAX_LAYERS];
  uint32 textTimescale[COMMON_MAX_LAYERS];
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  //The next fields are for the player to keep track of which track
  //is being selected.

  //List of track descriptions.
  ReferenceCountedPointer<ITrackList> m_trackList;

  ReferenceCountedPointer<ITrackSelectionPolicy> m_trackSelectionPolicy;

  //Callback funtion..called by Media layer
  pfnMediaPlayerCb MediaPlayerCbFn;
#ifdef FEATURE_WINCE
#error code not present
#endif
};
#endif /* _MEDIA_H_ */

