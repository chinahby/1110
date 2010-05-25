#ifndef _GENERIC_AUDIOMGR_H_
#define _GENERIC_AUDIOMGR_H_
/* =======================================================================
                               genericaudioMgr.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.
  
EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.
 
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/genericaudiomgr.h#7 $
$DateTime: 2008/05/08 08:09:22 $
$Change: 656074 $

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
#include "qcplayer.h"
#include "media.h"
#include "QtvPlayer.h"
#include "audiocmx.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

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

========================================================================== */

/* =======================================================================
**                        Class Declarations
** ======================================================================= */

/* ======================================================================
CLASS 
  AudioMgr

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
class genericAudioMgr : public AudioMgr
{
public: 
	
  genericAudioMgr();
  virtual ~genericAudioMgr();
  virtual bool Prep(
    Media * pMpeg4In,       //media pointer
    long nTimingUpdateMsec, //requested timing update interval.
    bool bRestart
    );
  virtual bool PlaySomeAudio(
  long &nWaitMsec,  //output: time calling thread should sleep before calling again.
  bool &bError,   //output: error occurred.
  Media::MediaStatus &dataStateOut, //output: data state
  long &cmxDataLead);               // output: CMX data lead
  virtual bool FillAudioBuffer(const uint32 max_len, const uint32 offset) ;
  virtual void PredictEndDelay(long &tEnd);
  virtual long GetElapsedTime();
  virtual void DataIsAvailable();
  
private:
    //Fixed parameters for this clip.
  struct ClipInfo
  {
    int nFramesPerBuffer;
    int nTimingUpdateIntervalBuffers;
    float fFramesPerMsec;
  };
  ClipInfo clipInfo;
  //Elapsed time stuff.
  struct ElapsedTime
  {
    long nLastTimestamp;
    long nAmtBuffered;
    bool bPlaying;
  };
  ElapsedTime elapsed;

//Data transfer info.
  struct XferInfo
  {
    int nBuffersSentInInterval;
    long tStartInterval;
    long nLastTimestampSent;
    long nLastFrameSent;
    bool bTimingUpdatedInInterval;
    TimePoint startOfInterval, endOfInterval;
    int nBytesSent;
  };
  XferInfo xferInfo;

  //Data related to sample processing.
  struct MediaInfo
  {
    //current frame number
    unsigned long nFramesProcessed ;

    //current state of data stream.
    Media::MediaStatus dataState;
  };
  MediaInfo mediaInfo;
//stop time
  struct StopTime
  {
    unsigned long nStop;
    bool bStop;
  };
  StopTime stopTime;
  //start time.
  long nStartTime;
  Media * pMpeg4;
  uint32 m_endDelay;
  /* m_faked_resume, this flag to help in ::prep not to reset m_bMediaInitialized 
     even  though the Media DATA_STATE is DATA_END. 
  */
  bool m_faked_resume;

  virtual void DestroyAudioInterface();
  virtual bool CreateAudioInterface(Media::CodecType codec);  
  virtual void SetAudioSpec();
  virtual void SetEndDelay();
  virtual bool Resume(bool bRestart);
  virtual Media::MediaStatus GetMediaStatus()
  {
    return mediaInfo.dataState;
  }
};
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

#endif //_GENERIC_AudioMgr_H_
