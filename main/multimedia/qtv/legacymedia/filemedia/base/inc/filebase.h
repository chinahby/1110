#ifndef __FileBase_H__
#define __FileBase_H__
/* =======================================================================
                              FileBase.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/base/main/latest/inc/filebase.h#29 $
$DateTime: 2010/08/11 06:04:10 $
$Change: 1398024 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
//#include "oscl_file_io.h"
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "isucceedfail.h"
#include "ztl.h"

#include "sampleentry.h"

//HEADER FILES REQD FOR MULTIPLE SAMPLE RETRIEVAL API
#include "oscl_media_data.h"

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT

#include "videofmt_common.h"

#include "textsampleentry.h"
#include "fontrecord.h"
#include "kddiuserdataatom.h"

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
#include "telop.h"
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

//When following feature is defined,
//media type(mpeg4/WM) will be determined by analysing the file extension.
//This avoids File Open/Close once for each track.
//#define FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
class Mpeg4Player;

/* This is the maximum number of streams that can be accessed through video
** format services.
*/
#if defined(FEATURE_QTV_WINDOWS_MEDIA) && defined(FEATURE_QTV_WM_SWITCHING_THINNING)
#define FILE_MAX_MEDIA_STREAMS  12
#else
#define FILE_MAX_MEDIA_STREAMS  6
#endif

#define MPG4_FILE_SIGNATURE_BYTES 4
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* This structure contains information about a single sample (video frame or
** audio packet) in a stream.
*/
enum AudioDataFormatType
{
  AUDIO_UNKNOWN,
  AUDIO_MIDI,
  NONMP4_AUDIO_MP3,
  NONMP4_AUDIO_AAC,
  AUDIO_QCP,
  AUDIO_VND_QCELP,
  AUDIO_QCF,
  AUDIO_MMF,
  AUDIO_PHR,
  AUDIO_IMELODY,
  AUDIO_ADPCM,
  AUDIO_AAC,
  AUDIO_AMR,
  AUDIO_WMA,
  AUDIO_HVS,
  AUDIO_SAF,
  AUDIO_XMF,
  AUDIO_DLS,
  QCP_QLCM,
  VIDEO_PMD,
  AUDIO_AC3,
  AUDIO_PCM
};
typedef struct
{
  uint32 sample;            /* sample number (first sample is zero)                    */
  uint32 size;              /* size of sample in bytes                                 */
  uint32 offset;            /* offset of sample in stream, in bytes                    */
  uint32 time;              /* composition time of sample, in the media timescale      */
  uint32 delta;             /* difference between composition time of this sample and  */
                            /* the next sample, in the media timescale                 */
  uint32 sync;              /* Indication if sample is a random access point           */
                            /* (non-zero) or not (zero)                                */
  uint32 sample_desc_index; /* Sample Description retrieved from the stsc atom         */
  uint32 num_frames;        /* normally it is one, but ASF file can give several       */
                            /* frames in one sample (packet)                           */
} file_sample_info_type;

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
  FileBase

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
class FileBase : public ISucceedFail
{
public:

  virtual ~FileBase()
  {
  } // So the Mpeg4File destructor gets called when delete the interface

  // MEDIA DATA APIS

  /* method to find if it is a MP4 file */
  virtual bool isMp4File(void) {return false;};
  virtual bool isAsfFile(void) {return false;};

  /* method to find if a MP4 file is fragmented or non-fragmented*/
  virtual bool isFileFragmented(void) {return false;};

  /* Returns media samples for the requested tracks
     id: The track ID of the track from which the method is to retrieve the samples.
     buf:  A pointer to the buffer into which to place the sample.
     size: The size of the data buffer
     index:  An output parameter which is the index of the sample entry to which the returned sample refers.
     return: The size in bytes of the data placed into the provided buffer.  If the buffer is not large enough, the return value is the negative of the size that is needed.
  */
  virtual   int32 getNextMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 &index) = 0;


#ifdef FEATURE_H264_DECODER
  virtual uint32 getNumPicParamSet(uint32) {return 0;};
  virtual uint32 getNumSeqParamSet(uint32) {return 0;};
  virtual int32 getNextParamSetNAL(uint32, uint8 *, uint32) {return 0;};
  virtual void  resetParamSetNAL(uint32) {};
  virtual int  GetSizeOfNALLengthField(uint32) {return 0;};
#endif /* FEATURE_H264_DECODER */

  /* Returns the timestamp for the previously returned media samples from
  // the requested track
  id: The track ID of the track from which the method is to retrieve the sample timestamp.
  return: The timestamp of the most recently return media sample in the "media timescale"
  */
  virtual  uint32 getMediaTimestampForCurrentSample(uint32 id)= 0;
  virtual  uint32 getMediaTimestampDeltaForCurrentSample(uint32) {return 0;};

  /* resets the playback for given track */
  virtual bool resetMediaPlayback(uint32 id) = 0;

  // META DATA APIS

  // From RandomAccessAtom 'rand'
  virtual uint8  randomAccessDenied()= 0;   // return _randomAccessDenied flag

  // From PVUserDataAtom 'pvmm'
  virtual OSCL_STRING getTitle()const = 0;    // return _title string
  virtual OSCL_STRING getAuthor()const = 0;   // return _author string
  virtual OSCL_STRING getDescription()const = 0;  // return _description string
  virtual OSCL_STRING getRating()const = 0;   // return _rating string
  virtual OSCL_STRING getCopyright()const = 0;    // return _copyright string
  virtual OSCL_STRING getVersion()const = 0;    // return _version string
  virtual OSCL_STRING getCreationDate()const = 0; // return _creationDate string
  virtual OSCL_STRING getPerf()const=0;           // return _performance string
  virtual OSCL_STRING getGenre()const=0;          // return _genre string
  virtual OSCL_STRING getClsf()const=0;           // return _classification string
  virtual OSCL_STRING getKywd()const=0;           // return _keyword string
  virtual OSCL_STRING getLoci()const=0;           // return _location string
  virtual OSCL_STRING getAlbum()const = 0;    // return _album string
  virtual OSCL_STRING getYrrc()const = 0;    // return recording year
  // from 'ftyp' atom
  virtual uint32 getCompatibiltyMajorBrand(){return 0;};
  virtual uint32 getCompatibiltyMajorBrandVersion(){return 0;};
  virtual ZArray<uint32> *getCompatibiltyList(){return NULL;};

  // From Movie
  virtual int32 getNumTracks()= 0;
  virtual uint32 getTrackWholeIDList( uint32 *ids)= 0;
  virtual bool  isAudioPresentInClip(){return false;}


  // From MovieHeader
  virtual uint32 getMovieDuration() const= 0;
  virtual uint32 getMovieTimescale() const= 0;

  // From Track
  virtual int16 getTrackContentVersion(uint32 id)= 0; // from 'cver' atom at track level
  virtual uint8 trackRandomAccessDenied(uint32 id)= 0; // from 'rand' atom at track level
  virtual float getTrackVideoFrameRate(uint32 id)= 0; // from 'vinf' atom at track level

  virtual uint32 getTrackVideoFrameWidth(uint32 id) = 0; // from pvti atom at file level
  virtual uint32 getTrackVideoFrameHeight(uint32 id) = 0;

  // From TrackReference
  // Returns the track ID of the track on which this track depends
  virtual  uint32  trackDependsOn( uint32) {return 0;}; //tao more work

  // From MediaHeader
  virtual uint32 getTrackMediaDuration(uint32 id)= 0;
  virtual uint32 getTrackMediaTimescale(uint32 id)= 0;
  virtual uint32 getTrackAudioSamplingFreq(uint32 id)= 0;

  virtual uint32 getAudioSamplesPerFrame(uint32 id)= 0;

  virtual uint8  getTrackOTIType(uint32 id) =0;// Based on OTI value
  virtual uint8  getTrackAudioFormat(uint32 id) =0; /* based on VideoFMT enum */
  virtual uint8 getFramesPerSample(uint32 id) = 0;
  virtual uint16 getTotalNumberOfFrames(uint32 id) = 0;

  //virtual int32 getTrackOTIValue(uint32 id);
  virtual int32  getTrackMaxBufferSizeDB(uint32 id)= 0;
  virtual int32  getTrackAverageBitrate(uint32 id)= 0;
  virtual int32  getTrackMaxBitrate(uint32 id)= 0;
  virtual uint32  getLargestFrameSize(uint32 id)= 0;

  virtual unsigned long GetNumAudioChannels(int) {return 0;};
  virtual void resetInitialization(){m_bMediaInitialized = false;};
  virtual OSCL_STRING getAudioTrackLanguage(uint32 ) {return 0;};
  virtual unsigned int UserCompare(bool &, int, int, int) {return 0;}
  virtual bool   isADTSHeader(){return false;};
  virtual unsigned long GetAudioBitsPerSample(int) {return 0;};

#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
  /* use these functions only for windows media audio, other formats may not implement it */
  virtual unsigned long GetFixedAsfAudioPacketSize(int) {return 0;};
  virtual unsigned long GetAudioEncoderOptions(int) {return 0;}

  //Returns audio virtual packet size in bytes.
  virtual uint16        GetAudioVirtualPacketSize(int){return 0;}

  //Returns estimated AUDIO ASF packet duration in msec.
  virtual long          GetAudioASFPacketDuration(int){return 0;}

  //Returns true if asffile has started sending dummy bytes.
   virtual bool          isStartOfDummyBytes(int){return false;}

   virtual uint16 GetAudioAdvancedEncodeOptions(int) {return 0;}
   virtual uint32 GetAudioAdvancedEncodeOptions2(int) {return 0;}
   virtual uint16 GetAudioChannelMask(int) {return 0;}
   virtual uint16 GetAudioArmDataReqThr(int) {return 0;}
   virtual uint8 GetAudioValidBitsperSample(int) {return 0;}

#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef FEATURE_QTV_AVI
   virtual long getAudioFrameDuration(int){return 0;}
   virtual void  SetIDX1Cache(void*){}
   virtual void* GetIDX1Cache(){return NULL;}
#endif

#ifdef FEATURE_FILE_FRAGMENTATION
  virtual int32  getNextFragTrackMaxBufferSizeDB(uint32){return 0;};
  virtual void setPausedVideo(boolean){};
  virtual void setPausedAudio(boolean){};
  virtual void setPausedText(boolean){};
  virtual boolean getPausedVideo( void ){return FALSE;};
  virtual boolean getPausedAudio( void ){return FALSE;};
  virtual boolean getPausedText( void ){return FALSE;};
  virtual void resumeMedia( void ){};
  virtual uint16 getParseFragmentNum( void ){return 0;};
  virtual uint16 getReadFragmentNum( void ){return 0;};
#endif


#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  virtual void updateBufferWritePtr(uint32){};
  virtual bool parseHTTPStream( void ){return FALSE;}
  virtual bool CanPlayTracks(uint32) {return FALSE;}
#endif /* FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#ifdef FEATURE_QTV_PSEUDO_STREAM
  virtual bool parsePseudoStream( void ){return parseHTTPStream();};
#endif

#if defined FEATURE_QTV_GENERIC_AUDIO_FORMAT && defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  virtual int32  setFileSize(uint32 id)= 0;
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */


#ifdef FEATURE_FILE_FRAGMENTATION
  virtual uint32 repositionAccessPoint(int32, uint32, bool &bError ,uint32 )
  {
     bError = true;
     return 0;
  }
#endif /*FEATURE_FILE_FRAGMENTATION*/

  virtual uint8 *getTrackDecoderSpecificInfoContent(uint32){return NULL;};
  virtual uint32 getTrackDecoderSpecificInfoSize(uint32){return 0;};
  virtual uint8 *getTrackDecoderSpecificInfoAtSDI(uint32, uint32){return NULL;};

  // Static method to read in an MP4 file from disk and return the FileBase interface

#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
  static FileBase *openMediaFile(  OSCL_STRING filename,
                                   Mpeg4Player *pMpeg4Player,
                                   bool bPlayVideo,
                                   bool bPlayAudio,
                                   bool bPlayText);
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_QTV_DRM_DCF

  // DRM DCF file support
  static FileBase *openMediaFile(  dcf_ixstream_type inputStream,
                                   Mpeg4Player *pMpeg4Player,
                                   QtvPlayer::URNTypeT urnType,
                                   bool bPlayVideo,
                                   bool bPlayAudio,
                                   bool bPlayText);
#endif

  static bool MAKE_AAC_AUDIO_CONFIG(uint8* pBuffer,uint16 nAudObjectType,
                                  uint16 nSamplingFreq,uint16 nChannelsConfig,
                                  uint8* pConfigSize);

  static FileBase *openMediaFile(  unsigned char *pBuf,
                                   Mpeg4Player *pMpeg4Player,
                                   uint32 bufSize,
                                   bool bPlayVideo,
                                   bool bPlayAudio,
                                   bool bPlayText
#if defined(FEATURE_QTV_PSEUDO_STREAM) || \
    defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                                   ,bool bPseudoStream = false
                                   ,uint32 wBufferOffset = 0
                                   ,uint32 wStartupTime = 0
                                   ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize = NULL
                                   ,QtvPlayer::FetchBufferedDataT FetchBufferedData = NULL
                                   ,QtvPlayer::InstanceHandleT handle = NULL
#endif  /* FEATURE_QTV_PSEUDO_STREAM */
        /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
                                 );

#ifdef FEATURE_QTV_WM_DRM_API
  virtual bool RegisterDRMDecryptMethod(QtvPlayer::DRMDecryptMethodT pDRMDecryptFunction, void *pClientData)
    {
        pDRMDecryptFunction = NULL;
        pClientData = NULL;
        QTV_USE_ARG2(pDRMDecryptFunction,pClientData); //added to remove warning.
        return false;
    }
#endif /* FEATURE_QTV_WM_DRM_API */

  virtual void resetPlayback() = 0;
  virtual uint32 resetPlayback(uint32 pos, uint32 id,
                               bool bSetToSyncSample, bool *bError, uint32 currentPosTimeStamp) = 0;

#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
  virtual uint32 skipNSyncSamples(int, uint32, bool *, uint32 )= 0;
#endif /* FEATURE_QTV_REPOSITION_SYNC_FRAME */

  virtual void resetPlaybackPos(uint32){return;}

#ifdef FEATURE_FILE_FRAGMENTATION
  virtual bool parseFragment( void ){return FALSE;};
#endif
  virtual void setAudioPlayerData(const void *){};
  virtual void setVideoPlayerData(const void *){};
  virtual void setTextPlayerData(const void *){};


  virtual uint8 getAllowAudioOnly() = 0;
  virtual uint8 getAllowVideoOnly() = 0;

  virtual uint32 peekCurSample(uint32 trackid, file_sample_info_type *pSampleInfo)=0;

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  virtual void Parse3GPPTimedTextAtom(video_fmt_tx3g_data_type *,OSCL_FILE *){};
  virtual TextSampleEntry *getTextSampleEntryAt (uint32, uint32){return NULL;};

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  //KDDI Telop related APIs
  virtual bool IsTelopPresent(){return false;};
  virtual uint32 getTelopTrackDuration(){return 0;};
  virtual TelopElement *getNextTelopElement(){return NULL;};
  virtual TelopHeader *getTelopHeader(){return NULL;};
  virtual uint32 resetTelopPlayback(uint32){return 0;};
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

  virtual bool HasMetaData() {return false;};

#ifdef FEATURE_MP4_KDDI_META_DATA
  /*KDDI Meta Data Related APIs*/

  // From CopyGaurdAtom
  virtual uint32 getCopyProhibitionFlag(){return 0;};
  virtual uint32 getValidityEffectiveDate(){return 0;};
  virtual uint32 getValidityPeriod(){return 0;};
  virtual uint32 getNumberofAllowedPlayBacks(){return 0;};

  // From Content Property Atom
  virtual OSCL_STRING getContentPropertyTitle(){return NULL;};
  virtual OSCL_STRING getContentPropertyCopyRight(){return NULL;};
  virtual OSCL_STRING getContentPropertyAuthor(){return NULL;};
  virtual OSCL_STRING getContentPropertyMemo(){return NULL;};
  virtual uint32      getAuthorDLLVersion(){return 0;};

  //From Movie Mail Atom
  virtual uint32 getEditFlags(){return 0;};
  virtual uint8  getRecordingMode(){return 0;};
  virtual uint32 getRecordingDate(){return 0;};

  //From Encoder Information Atom
  virtual uint8*  getDeviceName() const {return NULL;};
  virtual uint8*  getModelName() const {return NULL;};
  virtual uint8*  getEncoderInformation() const {return NULL;};
  virtual uint8*  getMuxInformation() const {return NULL;};

  //From GPS Atom
  virtual uint16  getGPSByteOrder(){return 0;};
  virtual uint32  getGPSVersionID(){return 0;};
  virtual uint32  getGPSLatitudeRef(){return 0;};
  virtual uint32  getGPSLongitudeRef(){return 0;};
  virtual uint32  getGPSAltitudeRef(){return 0;};
  virtual uint64 *getGPSLatitudeArray(){return NULL;};
  virtual uint64 *getGPSLongitudeArray(){return NULL;};
  virtual uint64 getGPSAltitude(){return 0;};
  virtual uint64 *getGPSTimeArray(){return NULL;};
  virtual OSCL_STRING getGPSSurveyData(){return NULL;};
  virtual OSCL_STRING getPositoningMethod(){return NULL;};
  virtual OSCL_STRING getPositioningName(){return NULL;};
  virtual OSCL_STRING getGPSDate(){return NULL;};
  virtual uint64 getGPSExtensionMapScaleInfo(){return 0;};

#endif /* FEATURE_MP4_KDDI_META_DATA */

#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_QTV_SKT_MOD_MIDI
  virtual bool IsMidiDataPresent(){return false;};
  virtual uint32 GetMidiDataSize(){return 0;};
  virtual uint32 GetMidiData(uint8 *, uint32, uint32){return 0;};

  virtual bool IsLinkDataPresent() {return false;}
  virtual uint32 GetLinkDataSize() {return 0;}
  virtual uint32 GetLinkData(uint8 *, uint32) {return 0;}
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

  virtual bool IsDataPresent(QtvPlayer::DataT, uint32) {return false;}
  virtual uint32 GetDataSize(QtvPlayer::DataT, uint32){return 0;}
  virtual uint32 GetData(QtvPlayer::DataT, uint8 *, uint32, uint32)
  {return 0;}
  virtual bool isGenericAudioFileInstance() = 0;

  virtual bool isAviFileInstance(){return false;}
#ifdef FEATURE_QTV_DIVX_DRM
  virtual void CopyDRMContextInfo(void*){}
  virtual void GetClipDrmInfo(void*){}
  virtual bool IsDRMProtection(){return false;}
  virtual bool CommitDivXPlayback(){return false;}
#endif

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  static AudioDataFormatType isGenericAudioFile(uint8 * pBuf, uint32 size);
  static AudioDataFormatType isGenericAudioFile(QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize, QtvPlayer::FetchBufferedDataT FetchBufferedData, QtvPlayer::InstanceHandleT handle = NULL);
  static AudioDataFormatType audio_Format_DetectType(const void * cpBuf, uint32  pdwSize);
  virtual int32  setTrackAverageBitrate(uint32 id)= 0;
  virtual void setRepositioningAllowedFlag(bool){ return; };
  virtual bool IsGenericAudioFileMediaInitialized(){ return false; };
  virtual bool IsGenericAudioFileRepositioned(){ return false; };
  virtual uint32 GetTimeStampFromMedia() = 0;
  virtual   int32 getNextGenericAudioMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 offset, uint32 &index) = 0;
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#ifdef FEATURE_QTV_PDCF
  virtual QtvPlayer::EncryptionTypeT getEncryptionType() {return QtvPlayer::ENCRYPT_NONE;};
  virtual QtvPlayer::EncryptionTypeT getEncryptionType(uint32) {return QtvPlayer::ENCRYPT_NONE;};
#endif /* FEATURE_QTV_PDCF */

protected:

        /*
        * When a media/interface is created,
        * m_bMediaInitialized will be initialized to FALSE to indicate that
        * initialization is not yet done. This is particularly
        * useful when playing generic audio formats as CMX expects
        * certain commands only when their internal state is initialized.
        *
        * For generic audio formats, when posting Seek command,
        * Mpeg4Player will wait on this flag(inside GenericAudioFile::ResetPlayback)before issuing SEEK commands to CMX.
        */
        bool m_bMediaInitialized;

  #ifdef FEATURE_QTV_DRM_DCF
     /*
      * To read few bytes from IxStream to determine if it is
      * 3GP/WM.
      */
  static uint32 readFile(dcf_ixstream_type inputStream,
                         uint8 *buffer,
                         uint32 pos,
                         uint32 size );

  #endif

  static uint32 readFile( OSCL_STRING filename,
                          uint8 *buffer,
                          uint32 pos,
                          uint32 size );

  static uint32 readFile( OSCL_FILE *fp,
                          uint8 *buffer,
                          uint32 pos,
                          uint32 size );

  static int32 seekFile ( OSCL_FILE *fp,
                          uint32 offset,
                          uint32 origin = SEEK_SET);

#ifdef FEATURE_QTV_WINDOWS_MEDIA

#ifdef FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE
   static bool IsASFFile(OSCL_STRING filename);
   static bool IsGenericAudioFile(OSCL_STRING filename);
#endif

  static bool IsASFFile(uint8 * pBuf, uint32 size);
  static bool IsASFFile(QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize, QtvPlayer::FetchBufferedDataT FetchBufferedData, QtvPlayer::InstanceHandleT handle = NULL);
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#ifdef FEATURE_QTV_AVI
  static bool IsAVIFile(OSCL_STRING,uint8*,bool);
  static bool IsAVIFile(QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize, QtvPlayer::FetchBufferedDataT FetchBufferedData, QtvPlayer::InstanceHandleT handle = NULL);
#endif

  static bool isMp4Filebool(uint8* pBuf, uint32 mp4size);
  static bool IsMP4_3GPFile(uint8* pBuf, uint32 mp4size);
  static bool IsMP4_3GPFile(QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize, QtvPlayer::FetchBufferedDataT FetchBufferedData, QtvPlayer::InstanceHandleT handle = NULL);

};

#endif

