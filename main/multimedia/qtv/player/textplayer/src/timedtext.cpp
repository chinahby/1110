/* =======================================================================
                              timedtext.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

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

$Header: //source/qcom/qct/multimedia/qtv/player/textplayer/main/latest/src/timedtext.cpp#17 $
$DateTime: 2010/02/19 00:32:23 $
$Change: 1184937 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"             /* Target specific definitions             */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT

#include "timedtext.h"
#include "media.h"
#include "ITrackList.h"
#include "ReferenceCountedPointer.h"
#include "Mpeg4Player.h"
#include "AVSync.h"
#include "qtv_log.h"

#ifdef FEATURE_QTV_DRM_DCF
#include "zrex.h"
#include "rex.h"
#include "ixipc.h"
#endif

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
//#define KTIMETHRESHOLD_GOTO_UNDERRUN   500

// debug support
#define SHOW_TIMING

// don't wait
#define NO_SLEEP (-1)
// wait for default time - COMMON_IDLE_WAIT_TIMEOUT_MSEC
#define DEFAULT_SLEEP (0)
const uint32 COMMON_TEXT_DATA_POLL_TIME_MSEC = 10;
const uint32 MAX_TEXT_DATA_LEAD_TIME_MSEC = 100;
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

// Number of instances of player
static int nInstance = 0;

// Callback data
QtvPlayer::CallbackFuncT TimedText::callbackFunction = NULL;
QtvPlayer::CallbackInstanceFuncT TimedText::callbackInstanceFunction = NULL;
QtvPlayer::InstanceHandleT TimedText::pCallbackHandle = NULL;
void *TimedText::pClientCallbackData = NULL;
void *TimedText::pUserCallbackData = NULL;

#if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES)
const char * TimedText::TextFormatString[] =
{
  "3gpp timed text"         /* For 3GPP Timed Text */
  ,"telop text"             /* For Telop Text */
  ,"ISDB-T subtitles"       /* For ISDB-T subtitles */
};
#endif /* if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */

// Context pointer for use by static routines
static TimedText *pThis = NULL;

// Thread Event Table

// Source
EVENT_POOL( CPVTEXT_POOL, 50 )
  //to player
  EVENT_SOURCE( CPVTEXT_STATUS, CPVTEXT_POOL )

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION
  TimedText::TimedText

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
TimedText::TimedText()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::TimedText" );

  TimedTextTC.threadHandle = NULL;
  TimedTextTC.pThreadFunction = (QCThreadFuncType)TheTimedTextThread;
  TimedTextTC.stackSize = COMMON_STACK_SIZE;
  QCUtils::SetThreadName(&TimedTextTC,Common::CPVTEXT_THREAD_NAME);

  m_pTextBuffer = NULL;
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  m_pTelopElement = NULL;
  m_iNumTelopSubStrings = 0;
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */
  beingRendered = false;
  m_bTextRepos = false;
  nElapsedTime = 0;
  nLastRenderTime = 0;
  bPlayingNotifyPending = false;

  nMaxTextDataLead = 0;
  nMinTextDataLead = 0;
  checkTimeFlag=true;
  checkTimeValue=0;
  iDisplayFlags=0;
  iHorizontalJustification=0;
  iVerticalJustification=0;
  for(int i=0;i<PVTEXT_MAX_TEXT_COLORS;i++)
  {
    iBackgroundColorRGBA[i]=0;
  }
 /*iBackgroundColorRGBA[PVTEXT_MAX_TEXT_COLORS]={0,0,0,0};*/
  iBoxTop=0;
  iBoxLeft=0;
  iBoxBottom=0;
  iBoxRight=0;
  iStartChar=0;
  iEndChar=0;
  iFontID=0;
  iFaceStyleFlags=0;
  iFontSize=0;
  for(int i=0;i<PVTEXT_MAX_TEXT_COLORS;i++)
  {
    iTextColorRGBA[i]=0;
  }

 /* iTextColorRGBA[PVTEXT_MAX_TEXT_COLORS]={0,0,0,0};*/
  iFontListSize=0;
  iFontTable=NULL;
  iSampleModifiersSize=0;
  iSampleModifiersBuffer=NULL;
  iBackgroundColorPtr=NULL;
  iTextColorPtr=NULL;
  iRemainingDuration=0;
  nElapsedTime=0;
  iTextTrackTimescale=0;
  m_iTextTrackID=0;
  m_iSampleDescIndex=0;
  m_iTextTrackTimescale=1000;
  m_iTextSampleSize=0;
  m_iTimeStamp=0;
  m_iMaxTextBufSize=0;
  m_iSampleDuration=0;
  m_telopBackgroundColor=0;
  m_telopDefaultFontColor=0;
  memset(&m_Mode,0,sizeof(CPVMode));
  memset(&iState,0,sizeof(TimedTextState));
  memset(&iTextResumeState,0,sizeof(TimedTextState));
  memset(&wakeupSync,0,sizeof(wakeupSync));
  memset(&responseSync,0,sizeof(responseSync));
  m_textStreamId=0;
  m_bIsFirstTextSample=true;
  m_IsLastSample=true;
  iTimeAtStart=0;
  iTimePaused=0;
  iCurrentPlaybackID=0;
  pMedia=NULL;
  m_pAVSync = NULL;
  pMp4Player = NULL;
  memset(&mediaStatus,0,sizeof(MediaStatus));
  iTimeAtPause=0;
  memset(&command,0,sizeof(Command));
  bAbort=true;

  #ifdef FEATURE_QTV_DRM_DCF
    //reset variable to indicate TEXT task is not registered with NS
    // and have no IPC signal associated with it.
    m_bIsTextTaskRegisteredForIPC = false;
  #endif
}


/* ======================================================================
FUNCTION
  TimedText::~TimedText

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
TimedText::~TimedText()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::~TimedText" );
  iFontTable=NULL;
  // Not needed, the Destroy routine will be used
  iSampleModifiersBuffer=NULL;
  iBackgroundColorPtr=NULL;
  iTextColorPtr=NULL;
  pMedia=NULL;
  m_pTextBuffer=NULL;
  m_pTelopElement=NULL;
  m_pAVSync = NULL;
  pMp4Player = NULL;

  #ifdef FEATURE_QTV_DRM_DCF
    //reset variable to indicate TEXT task is not registered with NS
    // and have no IPC signal associated with it.
    m_bIsTextTaskRegisteredForIPC = false;
  #endif
}


/* ======================================================================
FUNCTION
  TimedText::Create

DESCRIPTION
  Create the text player

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool TimedText::Create(void *pContext, AVSync* pAVSync)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::Create" );

  // Prevent multiple instances
  if (nInstance)
  {
    Destroy();
  }
  nInstance++;

  // Save the context.
  pMp4Player = (Mpeg4Player *)pContext;
  m_pAVSync = pAVSync;

  QCUtils::CreateCondition(&wakeupSync,false,false);
  QCUtils::CreateCondition(&responseSync,false,false);

  // Start the thread
  if (!StartThread())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText::Create failed");
    return false;
  }
  return true;
}

/* ======================================================================
FUNCTION
  TimedText::ResetData

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
void TimedText::ResetData()
{
  // Init statistics here and any other data that the text player can return
  nElapsedTime = 0;
}

/* ======================================================================
FUNCTION
  TimedText::Prep

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
bool TimedText::Prep(int const playbackID, bool bRestart, Media *pMpeg4In)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::Prep" );

  if (!bRestart)
  {
    //save playback ID
    iCurrentPlaybackID = playbackID;

    //save media pointer
    pMedia = pMpeg4In;

    mediaStatus = Media::DATA_INIT;

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
    /*
    ** GRB 1/13/04
    **
    ** It's possible that Prep() is being called in a 'replay' scenario, where
    ** we're preparing to replay a clip or play a new clip without the
    ** player being fully destructed between plays, and therefore without
    ** our destructor firing.  We need to check m_pTelopElement to see if
    ** there are 'leftovers' from the previous playback and clean them up
    ** if required.
    */
    if ( m_pTelopElement != NULL )
    {
      QTV_Delete( m_pTelopElement );
      m_pTelopElement = NULL;
    }
    m_iNumTelopSubStrings = 0;
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

    m_bIsFirstTextSample = true;
    m_textStreamId = 0;
    iRemainingDuration = 0;
    nElapsedTime = 0;
    nLastRenderTime = 0;
    iFontListSize = 0;
    iTimeAtStart = 0;
    iTimeAtPause = 0;
    iTimePaused = 0;
    m_iTimeStamp = 0;
    m_iSampleDescIndex = 0;

#if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES)
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
    if (pMedia && pMedia->IsTelopPresent())
    {
      m_Mode = EPVModeTelop;
    }
    else
#endif /*ifdef FEATURE_MP4_KDDI_TELOP_TEXT*/
    {
      ReferenceCountedPointer<ITrackList> trackList;

      if (pMedia && !pMedia->GetTrackList(MakeOutPointer(trackList)) || 
          (trackList == NULL))
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                    "ERROR: TimedText trackList is NULL");
        return false;
      }
      
      // Determine Text track ID (max one Text track per MP4 file for 3GPP)
      for (int32 i = 0; i < trackList->GetNumTracksAvailable(); i++)
      {
        Media::CodecType codecType = trackList->GetCodecType(i);

        // Always select Text track if present
        if (codecType == Media::TIMED_TEXT_CODEC)
        {
          m_Mode = EPVMode3GPP;
          m_iTextTrackID = trackList->GetTrackID(i);
        }
        if (codecType == Media::GENERIC_TEXT_CODEC)            
        {
          /* Theoretically "GENERIC_TEXT_CODEC" can support all the text modes/formats, the default mode
             is "EPVModeISDBText". Going forward we get should get this info from media */ 
             
          m_Mode = EPVModeISDBText;
          m_iTextTrackID = trackList->GetTrackID(i);
        }
      }
    }
#endif /* if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */


    if (pMedia)
    {
       pMedia->SetTextPlayerData(this);
    }


#if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES)
    if ( (m_Mode == EPVMode3GPP)||(m_Mode == EPVModeISDBText) )
    {
#endif /* if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */


      /* Just in case subsequent fragments have bigger samples than main fragment.
       * ading 1 for null termination.
       */
      if (pMedia)
      {  
         m_iMaxTextBufSize = pMedia->GetMaxTextBufferSizeDB(0) + 1;
      }

      if (m_pTextBuffer)
      {
          QTV_Free(m_pTextBuffer);
          m_pTextBuffer = NULL;
      }
      m_pTextBuffer = (unsigned char*)QTV_Malloc(m_iMaxTextBufSize);
      if(m_pTextBuffer == NULL)
      {
        m_iMaxTextBufSize = 0;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText buffer allocation failed");
        return false;
      }

#if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES)
    }
#endif /* if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */

    iDisplayFlags = 0;
    iHorizontalJustification = 0;
    iVerticalJustification = 0;
    for (int j=0; j<PVTEXT_MAX_TEXT_COLORS; j++)
    {
      iBackgroundColorRGBA[j] = 0;
      iTextColorRGBA[j] = 0;
    }
    iBackgroundColorPtr = &iBackgroundColorRGBA[0];
    iTextColorPtr = &iTextColorRGBA[0];
    iBoxTop = 0;
    iBoxLeft = 0;
    iBoxBottom = 0;
    iBoxRight = 0;
    iStartChar = 0;
    iEndChar = 0;
    iFontID = 0;
    iFaceStyleFlags = 0;
    iFontSize = 0;
    iFontListSize = 0;
  }

  m_IsLastSample = false;
  //beingRendered  = false;
  m_iTimeStamp   = 0;
  m_iSampleDuration = 0;
  SetCurrentState(ETTStopDone);
  SetResumeState(ETTStopDone);
  bPlayingNotifyPending = 0;

  return true;
}

/* ======================================================================
FUNCTION
  TimedText::Start

DESCRIPTION
  Start playing, return TRUE if success.
  Assume Prep was already called for this clip.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool TimedText::Start(bool &bError, bool bRestart)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::Start" );

  bError = false;

  if (CanStart(bRestart))
  {
    if (!SendCommand(PLAY, COMMON_CPVTEXT_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: Text Start response timeout");
      bError = true;
    }
    return true;
  }

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText::Start failed");
  return false;
}

/* ======================================================================
FUNCTION
  TimedText::Pause

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
bool TimedText::Pause(bool &bError)
{
  bool bOk = true; 
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::Pause" );

  bError = false;

  if (CanPause())
  {
    bPlayingNotifyPending = false;
    if (!SendCommand(PAUSE, COMMON_CPVTEXT_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: Text Pause response timeout");
      bError = true;
    }    
  }
  /* If the text state is not changed to paused then return false. */
  if(iState != ETTPause )
  {
    bOk = false;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText::Pause failed");
  }
  return bOk;
}

/* ======================================================================
FUNCTION
  TimedText::Suspend

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
bool TimedText::Suspend(bool &bError)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::Suspend" );

  bError = false;

  if (CanSuspend())
  {
    bPlayingNotifyPending = false;
    if (!SendCommand(SUSPEND, COMMON_CPVTEXT_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: Text Suspend response timeout");
      bError = true;
    }
    return true;
  }

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText::Suspend failed");
  return false;
}

/* ======================================================================
FUNCTION
  TimedText::Resume

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
bool TimedText::Resume(bool &bError, long)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::Resume" );

  bError = false;

  if (CanResume())
  {
    if (!SendCommand(RESUME, COMMON_CPVTEXT_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: Text Resume response timeout");
      bError = true;
    }
    return true;
  }

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText::Resume failed");
  return false;
}

/* ======================================================================
FUNCTION
  TimedText::Stop

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
bool TimedText::Stop(bool &bError)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::Stop" );

  bError = false;

  bPlayingNotifyPending = false;
  if (!SendCommand(STOP, COMMON_CPVTEXT_RESPONSE_TIMEOUT_MSEC))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: Text Stop response timeout");
    bError = true;
  }
  return true;
}

/* ======================================================================
FUNCTION
  TimedText::PauseTextFromApp

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
  bool TimedText::PauseTextFromApp()
{
  //if the thread is in the display sleep we want to
  //wake it up so it will be paused.
  //wakeup video thread.
  command=PAUSE;
  QCUtils::SetCondition(&wakeupSync);
  return true;
}

/* ======================================================================
FUNCTION
  TimedText::ResumeTextFromApp

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
bool TimedText::ResumeTextFromApp()
{
  //if the thread is in the display sleep we want to
  //wake it up so it will be Resumed.
  //wakeup video thread.
  command=PLAY;
  QCUtils::SetCondition(&wakeupSync);
  return true;
}

/* ======================================================================
FUNCTION
  TimedText::GetElapsedTime

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
long TimedText::GetElapsedTime()
{
  /*
   * When TEXT PLAYER is not PLAYING, we should not return nElapsedTime based on clock and offset calculation.
   * This will increment the playback time during pause state as clock is ticking.
   *
   */
   if(!IsPlaying())
   {
     QTV_MSG_PRIO1(QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_LOW, "TimedText::GetElapsedTime():(!IsPlaying()) nElapsedTime %d",nElapsedTime);
     return nElapsedTime;
   }

   uint32 timeOffset = 0;
   if(m_pAVSync)
   {
     timeOffset = m_pAVSync->ReadPlaybackOffset(  AVSync::TextAV, ZUtils::Clock(),
                                             nElapsedTime, CanSetAVSyncOffSet() );
   }

   long iTime = ZUtils::Clock();

   /* During fast playback the progress bar needs to be updated at the rate at  
   ** which playback occurs, so we need to multiply nElapsedTime with the
   ** playback speed ratio. 
   */
   nElapsedTime = ((iTime - timeOffset)*m_playbackSpeedNum)/m_playbackSpeedDen;

#ifdef FEATURE_QTV_AFE_CLK_BASED_AV_SYNC
#error code not present
#endif // #ifdef FEATURE_QTV_AFE_CLK_BASED_AV_SYNC

  /* Calculate and store the Max and Min Text data lead time */
  int32 lTextDataLead = 0;     
  lTextDataLead = (m_iTimeStamp + m_iSampleDuration) - nElapsedTime;
  if (nMaxTextDataLead < lTextDataLead){
       nMaxTextDataLead = lTextDataLead;
  }
  else if (lTextDataLead < nMinTextDataLead){
      nMinTextDataLead = lTextDataLead;
  }

   QTV_MSG_PRIO3(QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_LOW, "TimedText::GetElapsedTime():time %d timeOffset %d nElapsedTime %d",iTime,timeOffset,nElapsedTime);

   return nElapsedTime;
}

/* ======================================================================
FUNCTION
  TimedText::GetIsLastSample

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
bool TimedText::GetIsLastSample() const
{
  return m_IsLastSample;
}

/* ======================================================================
FUNCTION
  TimedText::Destroy

DESCRIPTION
  Force an immediate shutdown and cleanup

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void TimedText::Destroy()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::Destroy" );

  QCUtils::SetCondition(&wakeupSync);
  QCUtils::SetCondition(&responseSync);

  QCUtils::DestroyCondition(&wakeupSync);
  QCUtils::DestroyCondition(&responseSync);

  // Kill the thread
  QCUtils::KillThread(&TimedTextTC);

  // Cleanup
  Cleanup();

  // Reset instance counter
  nInstance = 0;

// callback reset
  callbackFunction = NULL;
  callbackInstanceFunction = NULL;

  // Reset context to effectively disable player
  pMp4Player = NULL;
}


/* ======================================================================
FUNCTION
  TimedText::IsDone

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
bool TimedText::IsDone()
{
  return (iState==ETTInit);
}

/* ======================================================================
FUNCTION
  TimedText::IsPlaying

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
bool TimedText::IsPlaying()
{
  if ((iState == ETTGetText) ||
    (iState == ETTRenderText) ||
    (iState == ETTPauseDone))
  {
    return true;
  }
  else
  {
    return false;
  }
}

/* ======================================================================
FUNCTION
  TimedText::IsPaused

DESCRIPTION
//
// tell if text is paused.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool TimedText::IsPaused()
{
  return(iState == ETTPause) ;
}

/* ======================================================================
FUNCTION:
  TimedText::TimingUpdate

DESCRIPTION:
  AV player calls this when audio thread has made a timing update

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void TimedText::TimingUpdate()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::TimingUpdate" );

  if (command == NONE)
  {
    //if the thread is in the display sleep we want to
    //wake it up so it will recompute the sleep time.
    if ((iState == ETTGetText) ||
      (iState == ETTSyncText)  ||
      (iState == ETTPauseDone) ||
      (iState == ETTRenderText))
      {
        //wakeup text thread.
        QCUtils::SetCondition(&wakeupSync);
      }
  }
}

#ifdef FEATURE_MP4_UNUSED_CODE
#error code not present
#endif /* FEATURE_MP4_UNUSED_CODE */

/* ======================================================================
FUNCTION
  TimedText::GetText

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
unsigned char const *TimedText::GetText()
{
  // Everytime the text is gotten, print it out to QxDM
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Timed text: ");
#ifndef FEATURE_WINCE
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, (const char *)(pThis->m_pTextBuffer + 2) );
#endif

  return pThis->m_pTextBuffer;
}

/* ======================================================================
FUNCTION
  TimedText::GetTextBufSize

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
uint32 TimedText::GetTextBufSize()
{
  return pThis->m_iTextSampleSize;
}

/* ======================================================================
FUNCTION
  TimedText::GetSampleDuration

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
int32 TimedText::GetSampleDuration()
{
  return pThis->m_iSampleDuration;
}

/* ======================================================================
FUNCTION
  TimedText::GetBeginTime

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
int32 TimedText::GetBeginTime()
{
  return pThis->m_iTimeStamp;
}


/* ======================================================================
FUNCTION
  TimedText::GetDisplayFlags

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
uint32 TimedText::GetDisplayFlags()
{
  return pThis->iDisplayFlags;
}

/* ======================================================================
FUNCTION
  TimedText::GetHorzJustification

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
int8 TimedText::GetHorzJustification()
{
    return pThis->iHorizontalJustification;
}

/* ======================================================================
FUNCTION
  TimedText::GetVertJustification

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
int8 TimedText::GetVertJustification()
{
  return pThis->iVerticalJustification;
}

/* ======================================================================
FUNCTION
  TimedText::GetBackgroundColorRGBA

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
uint8 const *TimedText::GetBackgroundColorRGBA()
{
    return pThis->iBackgroundColorPtr;
}

/* ======================================================================
FUNCTION
  TimedText::GetBoxTop

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
int16 TimedText::GetBoxTop()
{
    return pThis->iBoxTop;
}

/* ======================================================================
FUNCTION
  TimedText::GetBoxLeft

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
int16 TimedText::GetBoxLeft()
{
    return pThis->iBoxLeft;
}

/* ======================================================================
FUNCTION
  TimedText::GetBoxBottom

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
int16 TimedText::GetBoxBottom()
{
    return pThis->iBoxBottom;
}

/* ======================================================================
FUNCTION
  TimedText::GetBoxRight

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
int16 TimedText::GetBoxRight()
{
  return pThis->iBoxRight;
}

/* ======================================================================
FUNCTION
  TimedText::GetStartChar

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
uint16 TimedText::GetStartChar()
{
    return pThis->iStartChar;
}

/* ======================================================================
FUNCTION
  TimedText::GetEndChar

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
uint16 TimedText::GetEndChar()
{
  return pThis->iEndChar;
}

/* ======================================================================
FUNCTION
  TimedText::GetFontID

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
uint16 TimedText::GetFontID()
{
  return pThis->iFontID;
}

/* ======================================================================
FUNCTION
  TimedText::GetFontStyleFlags

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
uint8 TimedText::GetFontStyleFlags()
{
    return pThis->iFaceStyleFlags;
}

/* ======================================================================
FUNCTION
  TimedText::GetFontSize

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
uint8 TimedText::GetFontSize()
{
  return pThis->iFontSize;
}

/* ======================================================================
FUNCTION
  TimedText::GetTextColorRGBA

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
const uint8* TimedText::GetTextColorRGBA()
{
  return pThis->iTextColorPtr;
}

/* ======================================================================
FUNCTION
  TimedText::GetFontListSize

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
uint16 TimedText::GetFontListSize()
{
  return pThis->iFontListSize;
}

/* ======================================================================
FUNCTION
  TimedText::GetFontRecordAt

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
FontRecord const * TimedText::GetFontRecordAt(uint16 idx)
{
  return pThis->iFontTable->getFontRecordAt(idx);
}

/* ======================================================================
FUNCTION
  TimedText::GetSampleModifierSize

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
uint16 TimedText::GetSampleModifierSize()
{
    return pThis->iSampleModifiersSize;
}

/* ======================================================================
FUNCTION
  TimedText::GetSampleModifierBuffer

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
uint8 const *TimedText::GetSampleModifierBuffer()
{
  return pThis->iSampleModifiersBuffer;
}

/* ======================================================================
FUNCTION
  TimedText::RegisterForCallback

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
bool TimedText::RegisterForCallback(QtvPlayer::CallbackFuncT callback, void *pClientData)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::RegisterForCallback" );

  callbackFunction    = callback;
  pClientCallbackData = pClientData;
  return true;
}

bool TimedText::RegisterForCallback(QtvPlayer::CallbackInstanceFuncT callback, void *pClientData, 
                                    void *pUserData, QtvPlayer::InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::RegisterForCallback" );

  callbackInstanceFunction    = callback;
  pClientCallbackData = pClientData;
  pUserCallbackData = pUserData;
  pCallbackHandle = handle;
  return true;
}

/* ======================================================================
FUNCTION
  TimedText::Notify

DESCRIPTION
  Notify player thread of updated status and do a state transition.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void TimedText::Notify(TimedTextState newState, Common::TimedTextStatusCode notify)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::Notify" );

  // The order is important.  First save the playback id,
  // then change state, then send the notification.

  int playbackID = iCurrentPlaybackID;

  SetCurrentState(newState);

  // Notify player.
  if ( pMp4Player )
  {
    if ( pMp4Player->IsThreadActive() )
    {
      CPVTEXT_STATUS_type*pEvent =QCCreateMessage(CPVTEXT_STATUS, pMp4Player);
      if ( pEvent )
      {
        pEvent->playbackID = playbackID;
        pEvent->status = notify;
        pMp4Player->PostMessage(pEvent);
      }
    }
  }
  notify = (Common::TimedTextStatusCode) 0; /* Fix compiler warning */
}


/* ======================================================================
FUNCTION
  TimedText::TimedTextThread

DESCRIPTION
  Internal text thread routine.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void TimedText::TimedTextThread()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "TimedText::TimedTextThread");

  int sleepTime;

  for ( ; ; )
  {
    // Execute the command only if needed
    // Otherwise sometimes actions are missed
    if ( bAbort ||  (command != NONE)) 
    { 
      ProcessCommands();
    }

    // elapsed time check for mpeg4player
    if (checkTimeFlag)
    {
      if( (unsigned long)GetElapsedTime() >= checkTimeValue)
      {
        Notify(iState, Common::CPVTEXT_ELAPSE_TIME);
        checkTimeFlag = false;
      }
    }

    sleepTime = DEFAULT_SLEEP;
    switch (iState)
    {
      case ETTInit:
        break;

      case ETTSyncText:
      case ETTGetText:
      case ETTPauseDone:
      case ETTStopDone:
      case ETTRenderText:

         /**
		  * For DCF playback, Calling task needs to register an IPC signal. This signal is set by IxStream on completion
		  * of an asynchronous operation. IPC handler for the calling task will make sure
		  * to invoke IxStreamDispatchMsg which eventually invokes the callback provided by QTV in calling task context.
		  */
		#ifdef FEATURE_QTV_DRM_DCF
		  if(m_bIsTextTaskRegisteredForIPC == false)
		  {
		   /*
			 * Before reading any text data from IxStream,
			 * TEXT task needs to have an IPC signal and to be registered with ixipc.
			 * Without this, read rom TEXT task will get blocked foreever on DRM suite.
		   */
			 ixipc_init(0,QTV_DRM_DCF_IPC_SIG) ;
			 m_bIsTextTaskRegisteredForIPC = true;
		  }
    #endif

        ProcessNextTextSample(sleepTime);
        break;

      case ETTPause:
      case ETTStop:
      case ETTCleanup:
      default:

      #ifdef FEATURE_QTV_DRM_DCF
		  if( (m_bIsTextTaskRegisteredForIPC == true) && (iState == ETTStop) )
		  {		    
			  //when done, de-register with ixipc.
        ixipc_terminate();
			  m_bIsTextTaskRegisteredForIPC = false;
		  }
      #endif
        break;
    }

    // Wait for next text sample retrieval or incoming command
    if (sleepTime >= DEFAULT_SLEEP)
    {
      if (sleepTime == DEFAULT_SLEEP)
      {
        /* check if we have read and rendered the last frame and slept for last frame duration */
        if( (mediaStatus==Media::DATA_END) &&
            (m_IsLastSample==true) &&
            (iState!=ETTInit) )
        {
          QTV_MSG_PRIO(QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_HIGH, "Text Track Done");
          Notify(ETTInit, Common::CPVTEXT_DONE);
        }
        sleepTime = COMMON_IDLE_WAIT_TIMEOUT_MSEC;
      }
      (void)QCUtils::WaitForCondition(&wakeupSync,(unsigned long)sleepTime);
    }
  }
}


/* ======================================================================
FUNCTION
  SAMPLE_FUNC

DESCRIPTION
  Static text thread routine.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void TimedText::TheTimedTextThread(unsigned long param)
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "TimedText::TheTimedTextThread param 0x%x",param);

  pThis = (TimedText *)param;

  if (pThis)
  {
    pThis->TimedTextThread();
  }
  pThis = NULL;
}

/* ======================================================================
FUNCTION
  TimedText::CallCallback

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
void TimedText::CallCallback(QtvPlayer::PlayerStatusT status)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::CallCallback" );

  // Dummy PlayerStateRecord (not used for this type of callback)
  QtvPlayer::PlayerStateRecordT ps = {0};

  if (callbackFunction)
  {
    (callbackFunction)(status, pClientCallbackData, ps);
  }
  else if (callbackInstanceFunction)
  {
     (callbackInstanceFunction)(status, pClientCallbackData, ps, pCallbackHandle, pUserCallbackData);
  }
  return;
}


/* ======================================================================
FUNCTION
  TimedText::ProcessCommands

DESCRIPTION
  Process commands from AV player or data callback

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void TimedText::ProcessCommands()
{
  // Catch the abort signal.  This takes priority over commands.
  if (bAbort)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "TimedText Abort signal detected");
    AbortTextOutput();
    if (iState != ETTInit)
    {
      Notify(ETTInit,Common::CPVTEXT_ERROR_ABORT);
    }
    return;
  }

  // Process commands from the AV player.
  switch (command)
  {
  case STOP:
    StopTextOutput();
    Notify(ETTStop,Common::CPVTEXT_STOPPED);
    break;

  case SUSPEND:
    if (CanSuspend())
    {
      if (iState != ETTStop)
      {
        PauseTextOutput();
        SetResumeState(ETTSyncText);
        Notify(ETTPause,Common::CPVTEXT_SUSPENDED);
      }
    }
    break;

  case PAUSE:
    if (CanPause())
    {
      if (iState != ETTPause)
      {
        PauseTextOutput();
        Notify(ETTPause,Common::CPVTEXT_PAUSED);
      }
    }
    break;

  case PLAY:
  case RESUME:
    switch (iState)
    {
      case ETTInit:
      case ETTPauseDone:
      case ETTStop:
      case ETTStopDone:
        // Start buffering.
        Notify(ETTGetText,Common::CPVTEXT_BUFFERING);
        SetCurrentState(ETTSyncText);
        break;

      case ETTGetText:
      case ETTRenderText:
        // Just get next text sample or render it
        break;

      case ETTPause:
        if (CanResume())
        {
          /* record duration of PAUSE */
          iTimePaused = iTimePaused + (int32)ZUtils::Clock() - iTimeAtPause;

#ifdef SHOW_TIMING
          QTV_MSG_PRIO3( QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_MED, "text paused %u(%u to %u)",
                          iTimePaused, iTimeAtPause, ZUtils::Clock() );
#endif
          /* for text only clip, set AV Sync offset */
          if(CanSetAVSyncOffSet())
          {
            /* take care of time for which last frame was already displayed before pause */
            int32 nLastFrameDisplayed = iTimeAtPause - nLastRenderTime;
            QTV_MSG_PRIO1(QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_MED, "LastFrameDisplayed for %d, doing timeadjust", nLastFrameDisplayed );

            /*
            * nElapsedTime will now increment with the clock, so no need to
            * do any other adjustments.
            */

            if(m_pAVSync)
            {
              m_pAVSync->SetPlaybackOffset(AVSync::TextAV, (int)ZUtils::Clock(), nElapsedTime);
            }
          }
          iTimeAtPause = 0;
          Notify(ETTGetText,Common::CPVTEXT_BUFFERING);
          SetCurrentState(iTextResumeState);
        }
        break;
      default:
        break;
    };
    // resume text output
    bPlayingNotifyPending = true;
    break;

  default:
    QTV_MSG_PRIO1(QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_ERROR, "unrecognized command %d.", command);
    break;
  };

  // Handshake with the calling thread.
  if (command != NONE)
  {
    CommandComplete();
  }
}


/* ======================================================================
FUNCTION
  TimedText::CommandComplete

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
void TimedText::CommandComplete()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::CommandComplete" );
  QCUtils::SetCondition(&responseSync);
}


/* ======================================================================
FUNCTION
  TimedText::SendCommand

DESCRIPTION
  Send a command, wait on it to be consumed, return TRUE if
  it was consumed, false if we timed out.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool TimedText::SendCommand(Command newCommand, unsigned long timeout)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::SendCommand" );

  // Clear any old abort signal that has been processed.
  if (bAbort && (iState==ETTInit || iState==ETTStop || iState==ETTStopDone))
  {
    bAbort = false;
  }

  // Set the global command variable.
  command = newCommand;

  // Handshake with the thread.
  QCUtils::ResetCondition(&responseSync);
  QCUtils::SetCondition(&wakeupSync);
  bool bOK = QCUtils::WaitForCondition(&responseSync,timeout);

  // If a stop failed, set the abort signal so the text will
  // stop eventually.
  if (!bOK && command==STOP)
  {
    bAbort = true;
  }

  // Cleanup
  command = NONE;
  QCUtils::ResetCondition(&wakeupSync);

  if (!bOK)
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "TimedText::SendCommand failed");
  return bOK;
}


/* ======================================================================
FUNCTION
  TimedText::StartThread

DESCRIPTION
  Start the thread in case it has not yet been started
  or if it died due to hitting an exception.  Also resets
  the state to ETTInit.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool TimedText::StartThread()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::StartThread " );

  // Start the TimedText thread.  Only one
  // thread can be active at a time.
  if (!QCUtils::IsThreadActive(&TimedTextTC))
  {
    QCUtils::ResetCondition(&wakeupSync);

    // Initialize object state and defaults
    InitDefaults();

    if (pMp4Player)
    {
      TimedTextTC.priority = QTVTimedTextPriority;
      if (!QCUtils::CreateThread(&TimedTextTC, (unsigned long)(TimedText *)this, false))
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Error: TimedText::StartThread failed CreateThread");
        return false;
      }
    }
  }

  if (!QCUtils::IsThreadActive(&TimedTextTC))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText::StartThread failed IsThreadActive");
    return false;
  }
  return true;
}


/* ======================================================================
FUNCTION
  TimedText::AbortTextOutput

DESCRIPTION
  Stop, but don't handshake with it.  Use this for
  error cases where we're not sure we are playing.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void TimedText::AbortTextOutput()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "TimedText::AbortTextOutput" );

  bAbort = true;
}


/* ======================================================================
FUNCTION
  TimedText::StopTextOutput

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
void TimedText::StopTextOutput()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::StopTextOutput" );

  SetResumeState(ETTStop);
  SetCurrentState(ETTStop);
}


/* ======================================================================
FUNCTION
  TimedText::PauseTextOutput

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
void TimedText::PauseTextOutput()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::PauseTextOutput" );

  // Record system time when Pause command received
  if (iTimeAtPause == 0)
  {
     iTimeAtPause = (int32)ZUtils::Clock();
  }

  SetCurrentState(ETTPause);
  SetResumeState(ETTRenderText);
}


/* ======================================================================
FUNCTION
  TimedText::ProcessNextTextSample

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
void TimedText::ProcessNextTextSample(int &sleepTime)
{
  // support text player diag
  static int revCount = 0, renderCount = 0;

  int timeToSleep = DEFAULT_SLEEP;
  int timeNow = (int)ZUtils::Clock();

  sleepTime = DEFAULT_SLEEP;
  if (bAbort) return;

  int32 timeOffset = 0;   // shared by ETTGetText & ETTRenderText

  switch(iState)
  {
  case ETTPauseDone :
    // We don't want to sleep at all, so set sleepTime negative
    sleepTime = NO_SLEEP;
    SetCurrentState(ETTSyncText);
    SetResumeState(ETTSyncText);
    break;

  case ETTSyncText:
    if (m_IsLastSample)
    {
      // if done with text, goes to play state
      sleepTime = NO_SLEEP;
      SetCurrentState(ETTGetText);
      SetResumeState(ETTGetText);
    }
    else
    {
       bool dataFlag = false;
       Media::MediaStatus status;
       if (pMedia)
       {
          dataFlag = pMedia->IsTextAvailable(&status);
          if( !dataFlag && status == Media::DATA_NULLFILL )
          {
             dataFlag = true;
          }
       }

       if (dataFlag)
       {
         // resume text output
         bPlayingNotifyPending = true;
         /* record duration of REBUFFERING */
         if( iTimeAtPause > 0 )
         {
            iTimePaused += ((int32)ZUtils::Clock() - iTimeAtPause);

#ifdef SHOW_TIMING
            QTV_MSG_PRIO3( QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_MED, "text paused(buf) %u(%u to %u)",
              iTimePaused, iTimeAtPause, ZUtils::Clock() );
#endif

            timeNow = (int)ZUtils::Clock(); /* get time again to be more accurate */

            /* To enable fast playback the timestamps need to be shrunk, i.e. 
            ** PTS/playbackSpeed. We represent playback speed as a ratio, this will 
            ** translate to PTS/(Num/Den) = PTS*Den/Num. Presentation time is 
            ** unmodified for 1x speed.
            */
            uint32 iAdjustedTimeStamp = (m_iTimeStamp*m_playbackSpeedDen)/m_playbackSpeedNum;

            if(m_pAVSync)
            {
              timeOffset = m_pAVSync->ReadPlaybackOffset(AVSync::TextAV, 
                                                         timeNow,
                                                         iAdjustedTimeStamp, 
                                                         CanSetAVSyncOffSet());
            }

            /* iAdjustedTimeStamp is the same as m_iTimeStamp for 1x playback
            ** but is modified for fast playback. Use this value to calculate
            ** sleepTime.
            */
            if ( (iAdjustedTimeStamp + timeOffset + m_iSampleDuration) > iTimeAtPause)
            {
              sleepTime = (iAdjustedTimeStamp + timeOffset + m_iSampleDuration) - iTimeAtPause;
              SetCurrentState(ETTRenderText);
              SetResumeState(ETTGetText);
            }
            else
            {
              sleepTime = NO_SLEEP;
              SetCurrentState(ETTGetText);
              SetResumeState(ETTGetText);
            }
            iTimeAtPause = 0;
         }
         else
         {
           sleepTime = NO_SLEEP;
           SetCurrentState(ETTGetText);
           SetResumeState(ETTGetText);
         }
       }
       else
       {
          // sleep some time before polling again
          sleepTime = COMMON_BUFFERING_POLL_TIME_MSEC;
       }
    }
    break;

  case ETTGetText:
    if (m_IsLastSample)
    {
       StopTextOutput();
       break;
    }

    if(bPlayingNotifyPending)
    {
      QTV_MSG_PRIO(QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_MED, "Posting CPVTEXT_PLAYING");
      Notify(iState, Common::CPVTEXT_PLAYING);
      bPlayingNotifyPending = false;
    }

    // text track hasn't finished
    timeToSleep = NO_SLEEP;  // default - no sleep

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
    if (m_Mode == EPVModeTelop)
    {
      // Telop Timed Text mode
      if (SetupNextTelopElement() && m_pTelopElement)
      {
        m_iTimeStamp = (uint32)(m_pTelopElement->GetBeginTime());
        mediaStatus = Media::DATA_OK;
      }
      else
      {
        mediaStatus = Media::DATA_END;
      }
    }
    else
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */
    {
      // 3GPP Timed Text mode/ISDBT text mode
      /* get all other info about current sample */
      if (pMedia)
      {
        m_iTextSampleSize = pMedia->GetNextTextSample( mediaStatus,
                                                      m_textStreamId,
                                                      m_pTextBuffer,
                                                      m_iMaxTextBufSize);
      }

      if (mediaStatus == Media::DATA_OK)
      {
         // Suppose we should trust m_iTextSampleSize <= m_iMaxTextBufSize
         // For now, do extra safety checking here
         if (m_iTextSampleSize >= m_iMaxTextBufSize)
         {
            m_pTextBuffer[m_iMaxTextBufSize - 1] = '\0';
         }
         else
         {
            m_pTextBuffer[m_iTextSampleSize] = '\0';
         }

         /* get all other info about current sample */
         if (pMedia)
         {
            pMedia->GetCurTextSampleInfo((uint32*)&m_iTimeStamp, (uint32*)&m_iSampleDuration, &m_iSampleDescIndex);
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif
         }

         // Sample Modifier Content Retrieval
         uint8* byteBuffer = m_pTextBuffer;
         // byteBuffer may not be aligned properly, needs to use PACKED
         uint16 textSampleSize = ntohs( *(PACKED uint16*)byteBuffer );
         textSampleSize += 2;  // add the 2 bytes that contain the length
         byteBuffer += textSampleSize;
         iSampleModifiersBuffer = byteBuffer;
         iSampleModifiersSize = (uint16)(m_iTextSampleSize-textSampleSize);
      }
    }

    if (mediaStatus == Media::DATA_OK && m_bIsFirstTextSample)
    {
      m_bIsFirstTextSample = false;
#if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES)
      if ( (m_Mode == EPVMode3GPP||m_Mode == EPVModeISDBText) && pMedia)
      {
        m_iTextTrackTimescale = (uint32)pMedia->GetTextTimescale(0);
      }
      else
      {
        m_iTextTrackTimescale = 1000;  //always milliseconds for telop
        // We knows that TELOP timestamp always start from 0. Set the
        // playback offset if necessary. If master tracks (audio/video) exist,
        // they will set playback offset correctly.
				if(m_pAVSync)
				{
					if (!m_pAVSync->IsOffsetValid(AVSync::TextAV))
					{
						m_pAVSync->SetPlaybackOffset(AVSync::TextAV, (long)ZUtils::Clock(), 0);
					}
				}
      }
#else /* FEATURE_MP4_KDDI_TELOP_TEXT */
      if (pMedia)
      {
         m_iTextTrackTimescale = (uint32)pMedia->GetTextTimescale(0);
      }
#endif /* if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */

      // normally start time is set by SetPlayTimes function. If not, set it here */
      if(!iTimeAtStart)
      {
        iTimeAtStart = (int32)ZUtils::Clock();
        QTV_MSG_PRIO1(QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_MED, "Text player: started", iTimeAtStart);
        // reset debugging counter
        revCount = 0, renderCount = 0;
      }
    }

    switch( mediaStatus )
    {
    case Media::DATA_END:
      // Handle end of timed text track
      // inform Mpeg4Player only once
      if(!m_IsLastSample)
      {
        Notify(iState, Common::CPVTEXT_MEDIA_END);
      }
      m_IsLastSample = true;
      /* special case, if there was no sample in text track */
      if(m_bIsFirstTextSample)
      {
        /* this will help to end the text track */
        beingRendered = true;
      }
      StopTextOutput();
      timeToSleep = DEFAULT_SLEEP;
      break;
    case Media::DATA_NULLFILL:
    case Media::DATA_FRAGMENT:
       // no text data available, reiterate to check data
       timeToSleep = COMMON_BUFFERING_POLL_TIME_MSEC;
       break;
    case Media::DATA_UNDERRUN:
      // buffering/rebuffering, adjust pause time
        if (nMaxTextDataLead <= 0)
      	{
           PauseTextOutput();
           SetCurrentState(ETTSyncText);
           SetResumeState(ETTSyncText);
           timeToSleep = NO_SLEEP;
      	}
	else
	{
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Ignoring the media status Media::DATA_UNDERRUN, since we have data to decode/display & maxTextDataLead : %d ",nMaxTextDataLead);
          if( nMaxTextDataLead > MAX_TEXT_DATA_LEAD_TIME_MSEC )
          {
            timeToSleep = COMMON_BUFFERING_POLL_TIME_MSEC;
          }
          else
          {
            timeToSleep = COMMON_TEXT_DATA_POLL_TIME_MSEC;
          }
	}	  	
      break;
    case Media::DATA_ERROR:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText Media Data Error");
      AbortTextOutput();
      timeToSleep = NO_SLEEP;
      break;
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    case Media::DATA_INSUFFICIENT:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "DATA_INSUFFICIENT: Text track waiting for next sample");
      SetCurrentState(ETTPause);
      SetResumeState(ETTGetText);
      timeToSleep = NO_SLEEP;
      break;
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    case Media::DATA_REALLOCATE_BUFFER:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Text track sample at FragmentBoundary");

      uint32 bufferSize;
      bufferSize = 0;
      // adding 1 for null termination
      if(pMedia)
        bufferSize = pMedia->GetMaxTextBufferSizeDB(0) + 1;

      if(bufferSize > m_iMaxTextBufSize)
      {
        m_iMaxTextBufSize = bufferSize;

        if (m_pTextBuffer)
        {
          QTV_Free(m_pTextBuffer);
          m_pTextBuffer = NULL;
        }
        m_pTextBuffer = (unsigned char*)QTV_Malloc(m_iMaxTextBufSize);
        if(m_pTextBuffer == NULL)
        {
          m_iMaxTextBufSize = 0;
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText buffer allocation failed");
          AbortTextOutput();
        }
      }
      /* try again */
      timeToSleep = NO_SLEEP;
      break;

    case Media::DATA_OK:
      {
  #if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES)
      if ( (m_Mode == EPVMode3GPP)||(m_Mode == EPVModeISDBText) )
      {
  #endif /* if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */
        QTV_MSG_PRIO3( QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_MED, "Get text sample %u: size=%u, ts=%d", revCount++,
           m_iTextSampleSize - iSampleModifiersSize -2, m_iTimeStamp);
        QTV_MSG_PRIO4( QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_MED, "Sample content: ts=%d, ...0x%x,0x%x,0x%x",
           m_iTimeStamp, m_pTextBuffer[m_iTextSampleSize-1],
           m_pTextBuffer[m_iTextSampleSize-2], m_pTextBuffer[m_iTextSampleSize-3]);

#if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES)
         if( m_Mode == EPVMode3GPP )
#endif /* if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */
          {
						// Get Text Sample Modifiers corresponding to this 3GPP text sample
                                                TextSampleEntry *entry = NULL;
						if (pMedia)
                                                {
                                                   entry = pMedia->getTextSampleEntryAt(m_iTextTrackID, m_iSampleDescIndex);
                                                }
						if (entry)
						{
							QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "getTextSampleEntryAt sampleDescIndex %d",m_iSampleDescIndex);

							iDisplayFlags = entry->getDisplayFlags();
							iHorizontalJustification = entry->getHorzJustification ();
							iVerticalJustification = entry->getVertJustification ();

							iBackgroundColorPtr = entry->getBackgroundColourRGBA();

							iBoxTop = entry->getBoxTop ();
							iBoxLeft = entry->getBoxLeft ();
							iBoxBottom = entry->getBoxBottom ();
							iBoxRight = entry->getBoxRight ();
							iStartChar = entry->getStartChar ();
							iEndChar = entry->getEndChar ();
							iFontID = entry->getFontID ();
							iFaceStyleFlags = entry->getFontStyleFlags ();
							iFontSize = entry->getfontSize ();
							iFontTable = entry->getFontTable();

							iTextColorPtr = entry->getTextColourRGBA ();

							iFontListSize = entry->getFontListSize ();

							// For debug only, display results
							QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "displayFlags %d, hJust %d, vJust %d",iDisplayFlags,iHorizontalJustification,iVerticalJustification);
							QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "*bkRgba %d, top %d, left %d",*iBackgroundColorPtr,iBoxTop,iBoxLeft);
							QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "bottom %d, right %d, startChar %d",iBoxBottom,iBoxRight,iStartChar);
							QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "endChar %d, fontID %d, fontStyleFlags %d",iEndChar,iFontID,iFaceStyleFlags);
							if(iTextColorPtr)
							{
								QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "fontSize %d, *tRgba %d, fontListSize %d",iFontSize,*iTextColorPtr,iFontListSize);
							}
						}
         }
    #if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES)
      }
      else
      {
        // Telop Timed Text mode
        if(m_pTelopElement)
        {
          m_iSampleDuration = m_pTelopElement->GetDuration();
        }
        else
        {
          m_iSampleDuration = 0;
        }
      }
    #endif /* if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */

      /* convert TimeStamp and SampleDuration to MSec */
      if (m_iTimeStamp > 0)
      {
        m_iTimeStamp = (int32)((float)m_iTimeStamp*1000/m_iTextTrackTimescale);
      }
      else
      {
        m_iTimeStamp = 0;
      }

      if (m_iSampleDuration > 0)
      {
        m_iSampleDuration = (uint32)((float)m_iSampleDuration*1000/m_iTextTrackTimescale);
      }
      else
      {
        m_iSampleDuration = 0;
      }

      /* To enable fast playback the sample duration need to be shrunk, i.e. 
      ** SampleDuration/playbackSpeed. We represent playback speed as a ratio, 
      ** this will translate to SampleDuration/(Num/Den) = SampleDuration*Den/Num.
      ** SampleDuration is unmodified for 1x speed.
      */
      m_iSampleDuration = (m_iSampleDuration*m_playbackSpeedDen)/m_playbackSpeedNum;

      // move to render-text state
      timeToSleep = NO_SLEEP;
      iState = ETTRenderText;
      beingRendered = false;
      m_bTextRepos = false;
      break;
      }// Media::DATA_OK
    default:
      //error ending
      StopTextOutput();
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Text Data Error: status=%u", mediaStatus);
      timeToSleep = DEFAULT_SLEEP;
      break;
    }// switch( mediaStatus )

    sleepTime = timeToSleep;
    break;

  case ETTRenderText:
    {

    if(bPlayingNotifyPending)
    {
      QTV_MSG_PRIO(QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_MED, "Posting CPVTEXT_PLAYING");
      Notify(iState, Common::CPVTEXT_PLAYING);
      bPlayingNotifyPending = false;
    }

    // double check is it the time to render text, in case it is
    // waked up by AUDIO_TIMING event
    timeNow = (int)ZUtils::Clock();

    /* To enable fast playback the timestamps need to be shrunk, i.e. 
    ** PTS/playbackSpeed. We represent playback speed as a ratio, this will 
    ** translate to PTS/(Num/Den) = PTS*Den/Num. Presentation time is 
    ** unmodified for 1x speed.
    */
    uint32 iAdjustedTimeStamp = (m_iTimeStamp*m_playbackSpeedDen)/m_playbackSpeedNum; 

    if(m_pAVSync)
    {
      timeOffset = m_pAVSync->ReadPlaybackOffset(AVSync::TextAV, 
                                                 timeNow, 
                                                 iAdjustedTimeStamp,
                                                 CanSetAVSyncOffSet() );
    }

    if (!beingRendered)
    {
#ifdef SHOW_TIMING
       QTV_MSG3(QTVDIAG_TEXT_TASK, "prepare text sample: ts=%d,offset=%ld,t=%u",
          m_iTimeStamp, timeOffset, timeNow);
#endif

      // If needs to wait, adjust sleep time
      // Always rendering text without drop
      
      /* iAdjustedTimeStamp is the same as m_iTimeStamp for 1x playback
      ** but is modified for fast playback. Use this value to calculate
      ** delay.
      */
      int delay = (int)iAdjustedTimeStamp + (int)timeOffset - timeNow;
      if (delay >= TEXT_SYNC_WINDOW)
      {
         timeToSleep = delay;
      }
      else
      {
         timeToSleep = NO_SLEEP;
      }

      // needs to wait for the render-time to come?
      if (timeToSleep != NO_SLEEP)
      {
         sleepTime = timeToSleep;
         break;
      }

      // NOTE: Deal with sample duration unavailable case in streaming mode.
      //       The temporary solution is using default duration.
      //       Since from this state, text player will move to ETTGetText
      //       state immediately if duration is zero,
      //       this value is actually the duration before moving to ETTGetText
      //       state and start to retrieve next sample. In other word, it is
      //       the minimum grace period, within which the above level should
      //       retrieve text sample info after get
      //       QTV_PLAYER_STATUS_NEXT_TEXT_READY notification.
      //       By using the default duration, the side-effect is that real
      //       text sample rate should not greater than 10 per second. But
      //       that sounds reasonable in reality.
      if (m_iSampleDuration <= 0)
      {
         m_iSampleDuration = 100; // default to 100ms duration
      }

      // rendering text sample
      QTV_MSG_PRIO4(QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_MED, "Render text %u now:%u, duration %d, ts=%d",
         renderCount++, ZUtils::Clock(), m_iSampleDuration, m_iTimeStamp);

      /* set elaspe time before posting event to OEM layer */
      nElapsedTime = m_iTimeStamp;
      nLastRenderTime = timeNow;

      /* if delay is more negative than sample duration, we should drop it to
         catchup just like video, ignore this check incase of ISDB-T text  */
      if( (delay > (-m_iSampleDuration))
#ifdef FEATURE_QTV_ISDB_SUBTITLES
#error code not present
#endif
      )
      {
#if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES)
        QtvPlayer::PlayerStatusT callbackStatus;
        if (m_Mode == EPVMode3GPP)
          callbackStatus = QtvPlayer::QTV_PLAYER_STATUS_NEXT_TEXT_READY;
        else if(m_Mode == EPVModeISDBText)
          callbackStatus = QtvPlayer::QTV_PLAYER_STATUS_NEXT_GENERIC_TEXT_READY;
        else
          callbackStatus = QtvPlayer::QTV_PLAYER_STATUS_NEXT_TELOP_READY;
        CallCallback(callbackStatus);
#else /* if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */
        //Call client's callback function then sleep for duration of text sample
        CallCallback(QtvPlayer::QTV_PLAYER_STATUS_NEXT_TEXT_READY);
#endif /* if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */
      }
      else
      {
        QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Dropping Textsample=%d, delay=%d, SampleDuration=%d",
                  m_iTimeStamp, delay, m_iSampleDuration );
      }

      /* calculate sleep time before reading next sample */
      timeNow = (int)ZUtils::Clock(); /* get time again to be more accurate */

      // log timed text rendering
      TIMED_TEXT_LOG_INFO timed_text_log_info;
      timed_text_log_info.render_time = timeNow;
      timed_text_log_info.duration    = m_iSampleDuration;
      timed_text_log_info.timestamp   = (uint32)m_iTimeStamp;
      log_timed_text_msg( (int)m_iTextSampleSize - 2, (char*)(m_pTextBuffer + 2),
         timed_text_log_info, LOG_QTV_PLAYER_TIMED_TEXT_C);

      beingRendered = true;
    }// beingRendered?

    /* iAdjustedTimeStamp is the same as m_iTimeStamp for 1x playback
    ** but is modified for fast playback. Use this value to calculate
    ** sleepTime.
    */
    if ((iAdjustedTimeStamp + timeOffset + m_iSampleDuration) > timeNow)
    {
      timeToSleep = (iAdjustedTimeStamp + timeOffset + m_iSampleDuration) - timeNow;
    }
    else
    {
      timeToSleep = NO_SLEEP;   /* don't sleep */
    }

    if(timeToSleep > 0)
        iState = ETTRenderText;
    else
        iState = ETTGetText;

    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"timeNow %d, timeToSleep %d, m_iTimeStamp %d",timeNow,timeToSleep,m_iTimeStamp);
    //Return the amount of time to sleep until next text sample ready to be retrieved
    sleepTime = timeToSleep;
    } // case ETTRenderText:

    break;

    default:
    break;
  }
}

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
/* ======================================================================
FUNCTION
  TimedText::GetNextTelopElement

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
QtvPlayer::ReturnT TimedText::GetNextTelopElement(QtvPlayer::TelopElementT &telopElement)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::GetNextTelopElement" );

  telopElement.BackgroundColor  = (int32)pThis->m_telopBackgroundColor;
  telopElement.DefaultFontColor = (int32)pThis->m_telopDefaultFontColor;

  if (pThis->m_pTelopElement)
  {
    telopElement.TelopSize          = pThis->m_pTelopElement->GetTelopSize();
    telopElement.BeginTime          = pThis->m_pTelopElement->GetBeginTime();
    telopElement.Duration           = pThis->m_pTelopElement->GetDuration();
    telopElement.WrapFlag           = pThis->m_pTelopElement->GetWrapFlag();

    telopElement.NumberOfSubStrings = (int32)pThis->m_iNumTelopSubStrings;
    return QtvPlayer::QTV_RETURN_OK;
  }
  return QtvPlayer::QTV_RETURN_DATA_NOT_AVAILABLE;
}

/* ======================================================================
FUNCTION
  TimedText::GetTelopSubString

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
const QtvPlayer::TelopSubStringT* TimedText::GetTelopSubString(int index)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::GetTelopSubString" );
  static QtvPlayer::TelopSubStringT returnedSubStringStruct;
  static QtvPlayer::TelopSubStringT* const returnedSubStringPtr = &returnedSubStringStruct;
  SubStrings *pSubStrings;

  if ((index<0) || (index>=(int)pThis->m_iNumTelopSubStrings))
  {
    return NULL;
  }

  if (pThis->m_pTelopElement)
  {
    pSubStrings = pThis->m_pTelopElement->GetSubStringStructAt(index);
    if (!pSubStrings)
    {
      return NULL;
    }

    // Log Telop Substring to QxDM
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TelopSubString: ");
#ifndef FEATURE_WINCE
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, pSubStrings->textSubString );
#endif

    returnedSubStringStruct.LineFeed      = pSubStrings->oLineFeed;
    returnedSubStringStruct.Underline     = pSubStrings->oUnderLine;
    returnedSubStringStruct.Reversal      = pSubStrings->oReversal;
    returnedSubStringStruct.Linking       = pSubStrings->oLinking;
    returnedSubStringStruct.FontColor     = pSubStrings->fontColor;
    returnedSubStringStruct.TextSampleSize= pSubStrings->sizeofTextSampleInBytes;
    returnedSubStringStruct.LinkSize      = pSubStrings->linkSize;
    returnedSubStringStruct.TextSubString = pSubStrings->textSubString;
    returnedSubStringStruct.LinkValue     = pSubStrings->linkValue;
    return returnedSubStringPtr;
  }
  return NULL;
}

/* ======================================================================
FUNCTION
  TimedText::SetupNextTelopElement

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
bool TimedText::SetupNextTelopElement()
{
  static int32 newResult;

  if(!pMedia)
    return false;

  if (m_pTelopElement)
  {
    QTV_Delete( m_pTelopElement );
    m_pTelopElement = NULL;
  }

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::SetupNextTelopElement" );

  TelopHeader *m_pTelopHeader = NULL;
  TelopElement *telopFF = NULL;
  if (pMedia)
  {
     m_pTelopHeader = pMedia->GetTelopHeader();
     if (m_pTelopHeader)
     {
        m_telopBackgroundColor      = (uint32)m_pTelopHeader->backGroundColor;
        m_telopDefaultFontColor     = (uint32)m_pTelopHeader->defaultFontColor;
     }
     telopFF = pMedia->GetNextTelopElementPtr();
  }
  if (telopFF)
  {
    // Must always create local copy
    m_pTelopElement = QTV_New_Args( TelopElement, (*telopFF,&newResult) );
    if (m_pTelopElement)
    {
      if (!newResult)
      {
        m_iNumTelopSubStrings = (unsigned int)m_pTelopElement->GetNumSubStrings();
        return true;
      }
      // Here, we got a good pointer but the copy constructor failed
      QTV_Delete( m_pTelopElement );
      m_pTelopElement = NULL;
    }
  }
  return false;
}
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

/* ======================================================================
FUNCTION
  TimedText::GetTextFormatString

DESCRIPTION
  Returns the current system time (in milliseconds) adjusted by the 
  constant delta between the system clock and the clip's starting timestamp.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
uint8* TimedText::GetTextFormatString()
{
#if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES)
  return (uint8*)TextFormatString[pThis->m_Mode];
#else /* #if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */
  return NULL;
#endif 
}

/* ======================================================================
FUNCTION 
  TimedText::GetClockMilliSec

DESCRIPTION
  Returns the current system time (in milliseconds) adjusted by the
  constant delta between the system clock and the clip's starting timestamp.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int32 TimedText::GetClockMilliSec()
{
  return (int32)ZUtils::Clock();
}

/* ======================================================================
FUNCTION
  TimedText::SetCurrentState

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
void TimedText::SetCurrentState(TimedTextState newCurrentState)
{
  iState = newCurrentState;
  ShowState();
}

/* ======================================================================
FUNCTION
  TimedText::SetResumeState

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
void TimedText::SetResumeState(TimedTextState newResumeState)
{
  iTextResumeState = newResumeState;
  ShowState();
}

/* ======================================================================
FUNCTION
  TimedText::ShowState

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
void TimedText::ShowState()
{
  switch (iState)
  {
  case ETTInit:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText state ETTInit %d",ZUtils::Clock()); break;
  case ETTGetText:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText state ETTGetText %d", ZUtils::Clock()); break;
  case ETTSyncText:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText state ETTSyncText %d", ZUtils::Clock()); break;
  case ETTRenderText:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText state ETTRenderText %d", ZUtils::Clock()); break;
  case ETTPause:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText state ETTPause %d", ZUtils::Clock()); break;
  case ETTPauseDone:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText state ETTPauseDone %d", ZUtils::Clock()); break;
  case ETTStop:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText state ETTStop %d", ZUtils::Clock()); break;
  case ETTStopDone:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText state ETTStopDone %d", ZUtils::Clock()); break;
  case ETTCleanup:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText state ETTCleanup %d", ZUtils::Clock()); break;
  default: break;
  }

  switch (iTextResumeState)
  {
  case ETTInit:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText resume state ETTInit %d",ZUtils::Clock()); break;
  case ETTGetText:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText resume state ETTGetText %d", ZUtils::Clock()); break;
  case ETTSyncText:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText resume state ETTSyncText %d", ZUtils::Clock()); break;
  case ETTRenderText:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText resume state ETTRenderText %d", ZUtils::Clock()); break;
  case ETTPause:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText resume state ETTPause %d", ZUtils::Clock()); break;
  case ETTPauseDone:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText resume state ETTPauseDone %d", ZUtils::Clock()); break;
  case ETTStop:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText resume state ETTStop %d", ZUtils::Clock()); break;
  case ETTStopDone:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText resume state ETTStopDone %d", ZUtils::Clock()); break;
  case ETTCleanup:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText resume state ETTCleanup %d", ZUtils::Clock()); break;
  default: break;
  }

}

/* ======================================================================
FUNCTION
  TimedText::InitDefaults

DESCRIPTION
  Init data for each time player is created.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void TimedText::InitDefaults()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::InitDefaults" );

  SetCurrentState(ETTInit);
  SetResumeState(ETTInit);

    m_iTextTrackID = 0;
  m_iTextTrackTimescale = 0;
  m_iTextSampleSize = 0;
  m_iMaxTextBufSize = 0;
  m_pTextBuffer = 0;
    m_iSampleDuration = 0;
  iCurrentPlaybackID = 0;
  pMedia = 0;
  command = NONE;
  bAbort = false;

  checkTimeFlag = false;

  beingRendered = false;
  nElapsedTime = 0;

  m_playbackSpeedNum = 1;
  m_playbackSpeedDen = 1;

  nMaxTextDataLead = 0;
  nMinTextDataLead = 0;
}


/* ======================================================================
FUNCTION
  TimedText::Cleanup

DESCRIPTION
  Cleanup after playback

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void TimedText::Cleanup()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "TimedText::Cleanup" );

  if (m_pTextBuffer)
  {
        QTV_Free(m_pTextBuffer);
    m_pTextBuffer = 0;
  }

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  if (m_pTelopElement)
  {
    QTV_Delete( m_pTelopElement );
    m_pTelopElement = NULL;
  }
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */
  return;
}

/* ======================================================================
FUNCTION
  TimedText::CanStart

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
bool TimedText::CanStart(bool)
{
  return true;
}

/* ======================================================================
FUNCTION
  TimedText::CanPause

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
bool TimedText::CanPause()
{
  if(iState == ETTInit)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText::CanPause failed");
    return false;
  }
  return true;
}

/* ======================================================================
FUNCTION
  TimedText::TrackHasEnded

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
bool TimedText::TrackHasEnded()
{
  if((iState == ETTInit ) && ( mediaStatus == Media::DATA_END))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText::TrackHasEnded");
    return true;
  }
  return false;
}


/* ======================================================================
FUNCTION 
  TimedText::CanSuspend

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
bool TimedText::CanSuspend()
{
  if((iState == ETTInit ) && ( mediaStatus == Media::DATA_END))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: TimedText::CanPause failed");
    return false;
  }
  return true;
}

/* ======================================================================
FUNCTION
  TimedText::CanResume

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
bool TimedText::CanResume()
{
  return true;
}

/* ======================================================================
FUNCTION
  TimedText::SetPlayTimes

DESCRIPTION
  set start & stop times.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void TimedText::SetPlayTimes(long play,long)
{
  iTimeAtStart = ZUtils::Clock() - play;
  iTimeAtPause = 0;
  m_bTextRepos = true;
  m_IsLastSample = false;
  if(CanSetAVSyncOffSet())
  {
    QTV_MSG_PRIO( QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_MED, "INFO: Text SetPlayTimes caused timeadjust" );
    if(m_pAVSync)
    {
      m_pAVSync->SetPlaybackOffset(AVSync::TextAV, (int)ZUtils::Clock(), play);
    }
  }
  if( play >= 0)
  {
    nElapsedTime = 0;
  }

  QTV_MSG_PRIO2(QTVDIAG_TEXT_TASK, QTVDIAG_PRIO_HIGH, "Set Text PlayTime=%d, StartTime %d", play, iTimeAtStart);
}

/* ======================================================================
FUNCTION
  TimedText::EnableTimeCheck

DESCRIPTION
  Enable elapse time check

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void TimedText::EnableTimeCheck( unsigned long timeValue )
{
   checkTimeFlag = true;
   checkTimeValue = timeValue;
}

/* ======================================================================
FUNCTION
  TimedText::CanSetAVSyncOffSet

DESCRIPTION
  Can text task can set AV Sync offset. It can set only if audio and video
  tracks are not playing (either not present or ended due to some reason).

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool TimedText::CanSetAVSyncOffSet()
{
  /* if loop track flag is set, then text should always set AV Sync.
     Also, if Audio and Video are not playing, then text should set AV Sync */
  /* we get the pMp4Player pointer from the TimedText::Create*/
  if( (pMp4Player->IsAudioPlaying() || pMp4Player->IsVideoPlaying()) && !pMp4Player->GetLoopTrackFlag())
    return false;
  else
    return true;
}

#ifdef FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY
/* ======================================================================
FUNCTION
  TimedText::SetPlaybackSpeed

DESCRIPTION
  Sets the playback speed.

PARAMETERS
  Common::PlaybackSpeedT pbSpeed
    The desired playback speed.

RETURN VALUE
  None.
========================================================================== */
void TimedText::SetPlaybackSpeed(Common::PlaybackSpeedType pbSpeed)
{
  switch (pbSpeed)
  {
    case Common::PLAYBACK_SPEED_NORMAL:
    {
      m_playbackSpeedNum = 1;
      m_playbackSpeedDen = 1;
      break;
    }

    case Common::PLAYBACK_SPEED_1P3X:
    {
      m_playbackSpeedNum = 4;
      m_playbackSpeedDen = 3;
      break;
    }

    case Common::PLAYBACK_SPEED_NO_CHANGE:
    default:
    {
      /* Nothing to do */
      break;
    }
  } /* switch (pbSpeed) */
}
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY */

/* ======================================================================
FUNCTION:
  log_timed_text_msg

DESCRIPTION:
  Logs the timed text message

INPUT/OUTPUT PARAMETERS:
  cnt
  buffer
  log_code

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void log_timed_text_msg(int cnt, char * buffer, TIMED_TEXT_LOG_INFO& info, uint16 log_code)
{

   // sanity check
   if( buffer == NULL || cnt <=0 )
   {
      return;
   }

   // get timed text log size;
   unsigned int log_length = FPOS(TIMED_TEXT_LOG, content);
   log_length += (unsigned int)cnt;
   // allocate timed text log item
   TIMED_TEXT_LOG *timed_text_log = (TIMED_TEXT_LOG *)log_alloc(log_code, log_length);
   if (timed_text_log != NULL)
   {
      timed_text_log->timed_text_info = info;
      timed_text_log->size = (uint16)cnt;
      memcpy(timed_text_log->content, buffer, (unsigned int)cnt);
      log_commit(timed_text_log);
   }
}

#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
