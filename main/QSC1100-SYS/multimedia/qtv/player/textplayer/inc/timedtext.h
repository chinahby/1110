#ifndef _PVTEXT_H_
#define _PVTEXT_H_
/* =======================================================================
                              timedtext.h
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

$Header: //source/qcom/qct/multimedia/qtv/player/textplayer/main/latest/inc/timedtext.h#7 $
$DateTime: 2008/05/08 12:55:47 $
$Change: 656328 $


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
#include "fileMedia.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
// Remainder of file included only if this feature is defined
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT

// Sync window size - unit: ms
#define TEXT_SYNC_WINDOW 10

/* -----------------------------------------------------------------------
** Type Declarations
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
class AVSync;
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
  StyleRecordStruct

DESCRIPTION
  Definition of the StyleRecord structure

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
class StyleRecordStruct
{
public:
  uint16  startChar;
  uint16  endChar;
  uint16  fontId;
  uint8   faceStyleFlags;
  uint8   fontSize;
  uint8   textColorRGBA[4];
};

/* ======================================================================
CLASS
  TimedText

DESCRIPTION
  Interface class to retrieve 3GPP text samples and metadata

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
class TimedText
{
public:

  enum TimedTextState
  {
    ETTInit,
    ETTGetText,
    ETTSyncText,
    ETTRenderText,
    ETTPause,
    ETTPauseDone,
    ETTStop,
    ETTStopDone,
    ETTCleanup
  };

  TimedText();
  ~TimedText();

  bool Create(void *pContext, AVSync* pAVSync);
  void ResetData();
  bool Prep(int const playbackID, bool bRestart, Media* pMpeg4In);
  bool Start(bool &bError, bool bRestart);
  bool Pause(bool &bError);
  bool Suspend(bool &bError);
  bool Resume(bool &bError,long stop) ;
  bool Stop(bool &bError);
  long GetElapsedTime();
  bool GetIsLastSample() const;
  void Destroy();
  bool IsDone();
  bool IsPlaying();
  bool IsPaused();
  void TimingUpdate();
  void SetPlayTimes(long play,long stop);
  bool TrackHasEnded();

#ifdef FEATURE_MP4_UNUSED_CODE
#error code not present
#endif /* FEATURE_MP4_UNUSED_CODE */

  // Returns pointer to buffer containing next text sample
  static unsigned char const *GetText();
  // Returns number of bytes in buffer containing next text sample
  static uint32 GetTextBufSize();
  // Returns the display duration of the next text sample
  static int32 GetSampleDuration();
  static int32 GetBeginTime(); //Returns the timestamp of the current text sample
  // Returns the various text sample modifiers
  static uint32 GetDisplayFlags();
  static int8 GetHorzJustification();
  static int8 GetVertJustification();
  static uint8 const *GetBackgroundColorRGBA();
  static int16 GetBoxTop();
  static int16 GetBoxLeft();
  static int16 GetBoxBottom();
  static int16 GetBoxRight();
  static uint16 GetStartChar();
  static uint16 GetEndChar();
  static uint16 GetFontID();
  static uint8 GetFontStyleFlags();
  static uint8 GetFontSize();
  static uint8 const *GetTextColorRGBA();
  static uint16 GetFontListSize();
  static FontRecord const * GetFontRecordAt(uint16 idx);
  static uint16 GetSampleModifierSize();
  static uint8 const *GetSampleModifierBuffer();

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  bool SetupNextTelopElement();
  static QtvPlayer::ReturnT GetNextTelopElement(QtvPlayer::TelopElementT &telopElement);
  static const QtvPlayer::TelopSubStringT* GetTelopSubString(int index);
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

  static uint8* GetTextFormatString();

  // Registers a callback function with the TimedText thread
  static bool RegisterForCallback(QtvPlayer::CallbackFuncT callback, void *pClientData);
  static bool RegisterForCallback(QtvPlayer::CallbackInstanceFuncT callback, 
     void *pClientData, void *pUserData, QtvPlayer::InstanceHandleT handl);

  //Callback data
  static QtvPlayer::CallbackFuncT callbackFunction;
  static QtvPlayer::CallbackInstanceFuncT callbackInstanceFunction;
  static QtvPlayer::InstanceHandleT pCallbackHandle;
  static void *pClientCallbackData;
  static void *pUserCallbackData;

  ///////////////////////////
  // OS-related routines.
  ///////////////////////////

  //Used to notify player thread of events during playback.
  void Notify(TimedTextState newState, Common::TimedTextStatusCode notify);

  //Main thread routine.
  void TimedTextThread();
    static void TheTimedTextThread(unsigned long param);

  bool PauseTextFromApp();
  bool ResumeTextFromApp();

  void EnableTimeCheck( unsigned long timeValue );

#ifdef FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY
  // Sets the playback speed.
  void SetPlaybackSpeed(Common::PlaybackSpeedType pbSpeed);
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY */

int32    nMaxTextDataLead;
int32    nMinTextDataLead;

private:

  //Pointer to the AV sync object shared by all the media types
  AVSync* m_pAVSync;

  // elapsed time check for Mpeg4Player
  bool checkTimeFlag;
  unsigned long checkTimeValue;

  // Content of a 'tx3g' atom
  //-------------------------
  uint32  iDisplayFlags;
  int8    iHorizontalJustification;
  int8    iVerticalJustification;
  uint8   iBackgroundColorRGBA[PVTEXT_MAX_TEXT_COLORS];
  // box record
  int16   iBoxTop;
  int16   iBoxLeft;
  int16   iBoxBottom;
  int16   iBoxRight;
  // default style record
  uint16  iStartChar;
  uint16  iEndChar;
  uint16  iFontID;
  uint8   iFaceStyleFlags;
  uint8   iFontSize;
  uint8   iTextColorRGBA[PVTEXT_MAX_TEXT_COLORS];
  // font table 'ftab' box
  uint16  iFontListSize;
  FontTableAtom * iFontTable;

  // Sample Modifiers
  uint16  iSampleModifiersSize;
  uint8*  iSampleModifiersBuffer;

  uint8*  iBackgroundColorPtr;
  uint8*  iTextColorPtr;


  int32 iRemainingDuration;
  int32 nElapsedTime;     /* time stamp of last rendered frame */
  int32 nLastRenderTime;  /* clk time at which last frame was rendered */
  uint32  iTextTrackTimescale;

  uint32  m_iTextTrackID;   // Max one Text track per MP4 file for 3GPP
  uint32  m_iSampleDescIndex;
  uint32  m_iTextTrackTimescale;
  uint32  m_iTextSampleSize;
  int32   m_iTimeStamp;

  unsigned int  m_iMaxTextBufSize;
  unsigned char *m_pTextBuffer;
  int32         m_iSampleDuration;

#if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES)
  TelopElement  *m_pTelopElement;
  unsigned int  m_iNumTelopSubStrings;
  uint32        m_telopBackgroundColor;
  uint32        m_telopDefaultFontColor;

  enum CPVMode
  {
    EPVMode3GPP
    ,EPVModeTelop
    ,EPVModeISDBText
  };
  CPVMode m_Mode;
  
  static const char *TextFormatString[];

#endif /* if defined(FEATURE_MP4_KDDI_TELOP_TEXT) || defined(FEATURE_QTV_ISDB_SUBTITLES) */

  bool beingRendered;	/* text sample is being rendered */
  TimedTextState iState;
  TimedTextState iTextResumeState;

  int32 m_textStreamId;
  bool  m_bIsFirstTextSample;
  bool  m_IsLastSample;
  bool  m_bTextRepos;    /* text repositioning has happened */

  int32 iTimeAtPause;
  int32 iTimeAtStart;
  int32 iTimePaused;

  //Playback ID
  int iCurrentPlaybackID;

  //Media interface
  Media *pMedia;

  //Media Status
  Media::MediaStatus mediaStatus;

  Mpeg4Player *pMp4Player;

  //Thread control data
  QCThreadControl TimedTextTC;
  QCConditionType wakeupSync, responseSync;

  // Calls the registerd callback function
  void CallCallback(QtvPlayer::PlayerStatusT status);

  //Commands from the AV player to the TimedText thread
  enum Command {NONE, PLAY, PAUSE, STOP, SUSPEND, RESUME};

  Command command;

  bool bAbort;
  bool bPlayingNotifyPending;

  // If the clip has no audio, m_playbackSpeedNum and m_plyabckSpeedDen 
  // contain the playback speed ratio.
  // If audio is present m_playbackSpeedNum = m_playbackSpeedDen = 1 and
  // since audio is the master, playing audio faster will automatically force
  // text to playback faster.
  uint32 m_playbackSpeedNum;
  uint32 m_playbackSpeedDen;

  void ProcessCommands();
  void CommandComplete();
  bool SendCommand(Command newCommand, unsigned long timeout);
  bool StartThread();
  void AbortTextOutput();
  void StopTextOutput();
  void PauseTextOutput();
  void ProcessNextTextSample(int &sleepTime);
  int32 GetClockMilliSec();
  void SetCurrentState(TimedTextState newCurrentState);
  void SetResumeState(TimedTextState newResumeState);
  void ShowState();
  void InitDefaults();
  void Cleanup();
  bool CanStart(bool bRestart);
  bool CanPause();
  bool CanSuspend();
  bool CanResume();
  bool CanSetAVSyncOffSet();
  #ifdef FEATURE_QTV_DRM_DCF
      bool m_bIsTextTaskRegisteredForIPC;
  #endif


};

#endif  //#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#endif  //#ifndef _PVTEXT_H_

