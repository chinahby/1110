#ifndef __Mp4FragmentFile_H__
#define __Mp4FragmentFile_H__  
/* =======================================================================
                              mp4FragmentFile.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/mp4fragmentfile.h#15 $
$DateTime: 2010/01/07 03:31:52 $
$Change: 1129999 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "mpeg4file.h"
#include "videofmt_mp4r.h"

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
class Mpeg4Player;
class AudioPlayer;
#define QTV_MPEG4_MIN_TFRA_REW_LIMIT 2000       //2sec
#define DEFAULTFRAGMENTCOUNT 5000

typedef struct
{
    uint32 fragment_number;
    uint32 fragment_offset;
    boolean fragment_i_frame[VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32  frames[VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32  bytes[VIDEO_FMT_MAX_MEDIA_STREAMS];
    uint32  timestamp[VIDEO_FMT_MAX_MEDIA_STREAMS];
} fragment_info_type;


class VideoPlayer; /* forward decl */

class Mp4FragmentFile : public Mpeg4File
{

public:

  Mp4FragmentFile();
  Mp4FragmentFile(  OSCL_STRING filename,
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
                    ,bool bPseudoStream  = false
                    ,uint32 wBufferOffset = 0
                    ,uint32 wStartupTime = 0
                    ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize = 0
                    ,QtvPlayer::FetchBufferedDataT FetchBufferedData = NULL
                    ,QtvPlayer::InstanceHandleT handle = NULL
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
    ); // local file playback Constructor

#ifdef FEATURE_QTV_DRM_DCF
   // DCF-file-media playback Constructor
    Mp4FragmentFile(  dcf_ixstream_type inputStream,
                      Mpeg4Player *pMpeg4Player,
                      bool bPlayVideo = false,
                      bool bPlayAudio = false,
                    bool bPlayText  = false);
#endif

  virtual ~Mp4FragmentFile();

  virtual int32 getNextFragTrackMaxBufferSizeDB(uint32 id);

  virtual
  bool getTimestampedSampleInfo(video_fmt_stream_info_type *p_track,
                                uint32                      TimeStamp,
                                video_fmt_sample_info_type *sampleInfo,
                                uint32                     *newTimeStamp,
                                bool                        bSetToSyncSample,
                                uint32                      currentPosTimeStamp);

  virtual
  bool getSampleAtTimestamp(video_fmt_stream_info_type *p_track,
                            uint32                      timestamp,
                            bool                        rewind,
                            video_fmt_sample_info_type *sampleInfo);

#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
  virtual uint32 skipNSyncSamples(int offset, uint32 id, bool *bError, uint32 currentTimeStamp);
#endif /* FEATURE_QTV_REPOSITION_SYNC_FRAME */    

  bool parsePseudoStreamLocal( void );
  virtual uint16 getParseFragmentNum( void );
  virtual uint16 getReadFragmentNum( void );

  virtual uint32 repositionAccessPoint( int32 skipNumber, uint32 id, bool &bError ,uint32 currentPosTimeStampMsec);
  bool getAccessPointSampleInfo(video_fmt_stream_info_type *p_track,
                                int32                      skipNumber,
                                video_fmt_sample_info_type *sampleInfo,
                                uint32                     *newTimeStamp,
                                uint32                      currentPosTimeStampMsec);

  bool getTfraEntryInfo        (uint32                     streamNum,
                                int32                      skipNumber,
                                bool                       rewind,
                                video_fmt_tfra_entry_type  *tfraEntry,
                                uint32                      currentPosTimeStamp);

  bool findSampleFromTfra      (video_fmt_stream_info_type *input_track,
                                bool                       rewind,
                                uint32                     &reqSampleNum,
                                video_fmt_tfra_entry_type  *tfraEntry,
                                boolean                   &fragmentRepositioned);

  /*
    If a user does repeated Rew. and the skipNumber is set to 1, we would always keep going
    to the same previous sync point. In order to avoid this, search for the sync point after a
    pre-defined limit (in the reverse direction) from the current playback pos.
  */
  static uint16 m_minTfraRewindLimit;


  int32 processFragmentBoundary(video_fmt_stream_info_type *track);

  bool findiFrameFragment(video_fmt_stream_info_type *input_track,
                          uint32                     reqSampleNum,
                          bool                       rewind,
                          boolean                    findiFrame,
                          boolean                    &fragmentParsed);

  virtual uint32 getMovieDuration() const;

private:

  boolean parseUntilSampleFound( video_fmt_stream_info_type *track );
  void postMessage(QCMessageType *pEvent);
  void locateStreamData(
    video_fmt_stream_info_type  **p_stream_info,
    video_fmt_mp4r_stream_type  **p_stream,
    video_fmt_stream_info_type  *input_track);

  void reinitializeFragmentData(video_fmt_stream_info_type  *input_track,
                                uint32                      fragment_infoindex,
                                uint32                      reqSampleNum,
                                bool                        rewind);
  void reinitializeFragmentStream(video_fmt_mp4r_stream_type  *input_stream,
                                  fragment_info_type          *fragment_info,
                                  uint32                      fragment_infoindex,
                                  uint32                      stream_num,
                                  bool                        rewind);
  void reinitializeFragmentStreamInfo(video_fmt_stream_info_type  *input_streaminfo,
                                      fragment_info_type          *fragment_info,
                                      uint32                      stream_num);

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  video_fmt_status_type             m_mp4InitialParseStatus;
#endif

  QCCritSectType m_trackwrite_CS, pause_CS;

  virtual void setAudioPlayerData(const void *client_data);
  virtual void setVideoPlayerData(const void *client_data);
  virtual void setTextPlayerData(const void *client_data);
  AudioPlayer *AudioPlayerPtr;
  VideoPlayer *VideoPlayerPtr;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  TimedText     *TextPlayerPtr;
#endif

  ZArray<fragment_info_type *> fragmentInfoArray;
  uint32 fragmentinfoCount;
private:

  virtual void InitData();

  virtual int32 getSampleInfoError(video_fmt_stream_info_type *p_track);

  virtual void process_video_fmt_info(video_fmt_status_type status,
                              video_fmt_status_cb_info_type *info);

  boolean setMainFragmentBytes();

};

#endif //__Mp4FragmentFile_H__

