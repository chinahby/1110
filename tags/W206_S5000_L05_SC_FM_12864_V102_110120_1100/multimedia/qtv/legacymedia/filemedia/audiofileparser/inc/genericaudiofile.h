#ifndef __GenericAudioFile_H__ 
#define __GenericAudioFile_H__ 
/* =======================================================================
                              GenericAudioFile.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/audiofileparser/main/latest/inc/genericaudiofile.h#12 $

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
#include "Video.h"
#include "timedtext.h"
#include "udtaatoms.h"

#include "dcmdUserDataAtom.h"

#ifdef __cplusplus
extern "C" {
#endif
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
  GenericAudioFile

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
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
class GenericAudioFile : public FileBase, public Parentable
{

public:

  GenericAudioFile(); // Default Constructor
  GenericAudioFile(  OSCL_STRING filename,
                     Mpeg4Player *pMpeg4Player,
                     unsigned char *pFileBuf=NULL,
                     uint32 bufSize=0,
                     bool bPlayVideo = false,
                     bool bPlayAudio = false,
                     bool bPlayText  = false,
                     AudioDataFormatType audioFormatType = AUDIO_UNKNOWN
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#if defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) 
                    ,bool bHttpStream     = false
                    ,uint32 wBufferOffset = 0
                    ,uint32 wStartupTime  = 0
                    ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize = NULL
                    ,QtvPlayer::FetchBufferedDataT FetchBufferedData = NULL
                    ,QtvPlayer::InstanceHandleT handle = NULL
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
                  ); // local file playback Constructor

//#ifdef FEATURE_QTV_DRM_DCF
//  GenericAudioFile( const IxStream* inputStream,
//                    bool bPlayVideo = false,
//                    bool bPlayAudio = false,
//                    bool bPlayText  = false
//                  ); // DRM-file-media playback Constructor
//#endif

  virtual ~GenericAudioFile();
  virtual bool isMp4File(void) {return true;};

  virtual int32 getNextGenericAudioMediaSample(uint32 id, 
                                               uint8 *buf, 
                                               uint32 size, 
                                               uint32 offset, 
                                               uint32 &index);
  virtual   int32 getNextMediaSample(uint32, uint8 *, uint32, uint32 &){return 0;};
  virtual uint8  getFramesPerSample(uint32){return 0;};
  virtual uint16  getTotalNumberOfFrames(uint32){return 0;};
  virtual uint32 getMediaTimestampForCurrentSample(uint32 id);
  virtual bool resetMediaPlayback(uint32 id);  /* reset playback for a particular track */
  virtual uint32 GetTimeStampFromMedia();

  virtual void resetPlayback();
  virtual uint32 resetPlayback(uint32 pos, 
                               uint32 id, 
                               bool bSetToSyncSample, 
                               bool *bError, 
                               uint32 currentPosTimeStamp);

#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME  
  virtual uint32 skipNSyncSamples(int, uint32, bool *, uint32 );
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

    // From Movie
  virtual int32 getNumTracks();
  virtual uint32 getTrackWholeIDList(uint32 *ids);

  // RETRIEVAL METHODS
  // Methods to get the sample rate (i.e. timescales) for the streams and
  virtual uint32 getMovieDuration() const;
  virtual uint32 getMovieTimescale() const;
  
  // From Track
  virtual int16 getTrackContentVersion(uint32 id);
  virtual uint8 trackRandomAccessDenied(uint32 id);
  virtual float  getTrackVideoFrameRate(uint32 id); // from 'vinf' atom at track level

  virtual uint32 getTrackVideoFrameWidth(uint32 id); // from pvti atom at file level
  virtual uint32 getTrackVideoFrameHeight(uint32 id);

  virtual unsigned long GetNumAudioChannels(int id); // from qtv_windows_media


  // From TrackReference
  virtual uint32  trackDependsOn(uint32 id);

  // From MediaHeader
  virtual uint32 getTrackMediaDuration(uint32 id);
  virtual uint32 getTrackMediaTimescale(uint32 id);
  virtual uint32 getTrackAudioSamplingFreq(uint32 id);

  virtual uint32 getAudioSamplesPerFrame(uint32 id);
  virtual uint32 peekCurSample(uint32 trackid, file_sample_info_type *pSampleInfo);

  virtual uint8  getTrackOTIType(uint32 id); // Based on OTI value
  virtual uint8  getTrackAudioFormat(uint32 id); /* based on VideoFMT enum */

  virtual int32 getTrackMaxBufferSizeDB(uint32 id);
  
  virtual bool  isAudioPresentInClip();

  virtual int32 getTrackAverageBitrate(uint32 id);
  virtual int32 getTrackMaxBitrate(uint32 id);
  virtual int32 setTrackAverageBitrate(uint32 id);
  virtual void setRepositioningAllowedFlag(bool repositioningAllowed);
  virtual bool IsGenericAudioFileMediaInitialized();
  virtual bool IsGenericAudioFileRepositioned();

  virtual uint8 getAllowAudioOnly();
  virtual uint8 getAllowVideoOnly();
  virtual bool isGenericAudioFileInstance();

  // Operations
public:
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  virtual void parseFirstFragment();
#endif
  virtual uint32 getLargestFrameSize (uint32 id);
  void SetTotalTime();
  void GetAudioSpec();
  void static GetToatalTimeDataRequestCallback(uint32 position,
                                              uint32 max_len,
                                              cmx_client_req_cb_func_ptr_type data_req_cb,
                                              const void *server_client_data,
                                              const void *client_data
                                              );

  void static GetTotalTimeCallback(cmx_status_type status,
                                   const void * client_data,
                                   uint32 time_ms
                                  );
  
  void static GetAudioSpecDataRequestCallback(uint32 position,
                                              uint32 max_len,
                                              cmx_client_req_cb_func_ptr_type data_req_cb,
                                              const void *server_client_data,
                                              const void *client_data
                                              );
  void static GetAudioSpecCallback(cmx_status_type status, 
	                               const void * client_data,
                                   /* Audio spec of requested file */
                                   cmx_af_audio_spec_type *audio_spec   
                                   );
  void static GenericSetAudioSpec(const void * client_data, cmx_af_audio_spec_type * psd);

  void static SeekCallback(cmx_status_type status,
                           const void * client_data
                          );

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  virtual bool CanPlayTracks(uint32 pbTime);  
  void sendHTTPStreamUnderrunEvent(void);
  virtual bool parseHTTPStream( void );
  boolean initializeVideoFMT(void);
  void sendParserEvent(Common::ParserStatusCode status);
  virtual int32 setFileSize(uint32 id);
  virtual void updateBufferWritePtr(uint32 writeOffset);
  void sendParseHTTPStreamEvent( void );
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

protected:

	// These variables are used to communicate between the class and the
  // callback function registered with the video format services.
  bool  m_hasAudio;
  bool  m_playAudio;
  
  /* 
  * Variables to indicate whether clip has audio/video/text tracks.
  * These are useful in reporting UNSUPPORTED_FORMAT notifications if we don't end up selecting
  * any of the following media tracks but clip has corresponding tracks.
  */
  bool  m_bAudioPresentInClip;
  bool  m_bVideoPresentInClip;
  bool  m_bTextPresentInClip;
  
  /* Flag to mark if the current parsing position is end of file */
  AudioDataFormatType  m_audioFormatType;
  int m_bitRate;
  
  OSCL_FILE*  m_parseFilePtr;
  OSCL_FILE*  m_streamFilePtr[VIDEO_FMT_MAX_MEDIA_STREAMS];
  OSCL_STRING m_filename;
  uint32      m_fileSize;
  bool m_seekingAllowed;
  bool m_repositioningAllowed;
  bool m_repositioned;

  /* only one of "m_pFileBuf" or "m_filename" can be non-zero */
  unsigned char *m_pFileBuf;  /* pointer to buffer for playback from memory */

  bool m_bMediaInitialized; 

  virtual void resetInitialization();

//#ifdef FEATURE_QTV_DRM_DCF
//  const IxStream* m_inputStream;
//#endif


#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
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
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
   virtual int32 getSampleInfoError(video_fmt_stream_info_type *) { return 0; }
  
private:
  Mpeg4Player *m_pMpeg4Player;
  cmx_handle_type handle;
  uint32 m_totalPbTime;
  uint32 m_nDataOffsetWhenUnderRun;
  uint32 m_nPreRollDataBytes;
  uint32 m_timeStamp;
  QCConditionType genericAudio_wakeupSync;
  uint8 *m_buffer;
  virtual void InitData();
};

#endif  /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

#endif  // __GenericAudioFile_H__

