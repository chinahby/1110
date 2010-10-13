/*=============================================================================
  FILE: MPSettings.h

  SERVICES: This header defines the mediaplyer setup objects/functions, which are
  shared by both C++ and C codes.

  GENERAL DESCRIPTION:
  helper classes/functions for ISoundCtl 

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPSettings.h#23 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPSETTINGS_H
#define MPSETTINGS_H

#include "MPDefines.h"
#include "AEEMediaMPEG4.h"

#ifdef FEATURE_MP_EQ_UI

#include "AEEGraphics.h"
#include "AEEFile.h"

#ifdef FEATURE_MP_EQ
  #include "AEESoundCtl.h"
  #include "SoundCtl.bid"
#endif //FEATURE_MP_EQ

#define MP_EQUALIZER_GAIN_MAX 12
#define MP_EQUALIZER_GAIN_MIN -12
#define MP_SPECTRUM_DATA_PERIOD 10  // spectrum data period = 10*10 ms

#define TEMPO_MAX                500
#define TEMPO_MIN                0
#define TEMPO_DEFAULT            100

#define QSYNTH_HYBRID_MAX        7
#define QSYNTH_HYBRID_MIN        0
#define QSYNTH_HYBRID_STEP       1

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BUFFER_LENGTH 64
#define MAX_NUM_OF_BANDS   8

#ifdef FEATURE_MP_EQ
typedef struct {
  AEESoundCtlEqualizerFilter  Filter[MAX_NUM_OF_BANDS]; 
  int32                       nBands;                   // nBands shoud <= FEATURE_MP_EQ
  AECHAR                      Name[MAX_BUFFER_LENGTH];
} stEqualizer;

typedef struct {
  AEESoundCtlAudioExpander    AudioExp; 
  short                       idx;
  AECHAR                      Name[MAX_BUFFER_LENGTH];
  short                       curPreset;
  short                       curLevel;
  short                       numLevels;
}stAudioExp;

#endif //FEATURE_MP_EQ

#ifdef FEATURE_QTV_GENERIC_BCAST
typedef struct {
  uint32            nTimestamp;
  int32             nOffset;
}stBcastSettings;
#endif // FEATURE_QTV_GENERIC_BCAST

enum MP_ANGLEDMIXING_DATA
{
  MP_SOUNDCTL_CLASS_VOCODER = 0,
  MP_SOUNDCTL_CLASS_AUDIO,
  MP_SOUNDCTL_CLASS_MAX
};

typedef struct MPSetting
{
  boolean          m_bEnableAnalyzer;
  boolean          m_bShowAnalyzer;
  boolean          m_bEnableAudioExpander;
  boolean          m_bEnableEqualizer;
  boolean          m_bEnableBcastSettings;
  boolean          m_bEnableAngledMixing;

  byte             m_nFilterID;
  byte             m_nSliderID;

#ifdef FEATURE_MP_EQ
  ISoundCtl*       m_pSoundCtl;
  stEqualizer      m_EqData;
  stAudioExp       m_AudioExpanderData;

#ifdef MP_FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif //MP_FEATURE_AUDIO_QENSEMBLE

#endif //FEATURE_MP_EQ

#ifdef FEATURE_QTV_GENERIC_BCAST
  stBcastSettings  m_BcastData;
#endif // FEATURE_QTV_GENERIC_BCAST

  boolean          m_bEqualizerHasFocus; 
  boolean          m_bEditMode;  // ctl is in editing mode
  void*            m_pSetupWin;  // point to CMPSetupWin
  void*            m_pOwner;     // point to CMediaPlayer

} MPSetting;

void MPSettingInit(MPSetting* paudio, void* pOwner);
// release and cleanup setting object.
// note, this function is not going to delete psetting
void MPSettingRelease(MPSetting* psetting);

#ifdef FEATURE_MP_EQ
  void GetDefaultExpanderData(stAudioExp* pAudioData);
  void GetDefaultEqData(stEqualizer* pdata);
#ifdef MP_FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif //MP_FEATURE_AUDIO_QENSEMBLE
#endif
#ifdef FEATURE_QTV_GENERIC_BCAST
 void GetDefaultBcastSettingsData(stBcastSettings* pdata);
#endif // FEATURE_QTV_GENERIC_BCAST

// enable/disable functions
int MPAudioEnableEqualizer(MPSetting* paudio, boolean bEnable);
int MPAudioEnableExpander(MPSetting* paudio, boolean bEnable);
int MPAudioEnableSpectrumAnalyzer(MPSetting* paudio, boolean bEnable);
int MPAudioEnabledAngledMixing(MPSetting* paudio, boolean bEnable);
int MPEnableBcastSettings(MPSetting* psetting, boolean bEnable);

#ifdef FEATURE_MP_EQ
int MPAudioSetEqualizer(MPSetting* paudio, AEESoundCtlEqualizerFilter* pfilter, int32 nBands, AECHAR* pName);
#endif //FEATURE_MP_EQ
int MPAudioSetEqualizerGain(MPSetting* paudio, int16 nBand, int16 nGain);

int MPAudioSpectrumAnalyzerCallbackPeriod(MPSetting* paudio, int nPeriod);

#ifdef FEATURE_MP_EQ
int MPAudioSetExpander(MPSetting* paudio, AEESoundCtlAudioExpander* pAuExp);
int MPQConcertSetPreset(MPSetting* psetting, short preset);
int MPQConcertSetLevel(MPSetting* psetting, short level);
int MPQConcertSetNumLevels(MPSetting* psetting, short numLevels);

#ifdef MP_FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif //MP_FEATURE_AUDIO_QENSEMBLE

#endif //FEATURE_MP_EQ



boolean MPShowSetupWin(MPSetting* paudio, boolean bShow);
boolean MPSetupWinIsShow(MPSetting* paudio);

boolean MPUpdateSetupWin(MPSetting* psetting,  IGraphics* pg, boolean bHasVideo);
boolean MPUpdateAnalyzerGraph(MPSetting* paudio, IGraphics* pg);
// this function turn on/off setup screen focus.
// return TRUE, if focus changed, otherwise return FALSE
boolean MPSetupFocus(MPSetting* paudio, boolean bMode);

int MPSetupHandleEvent(MPSetting* paudio, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// read/write audio config file
void MPReadAudioConfig(IFile *pIFile,  MPSetting* paudio);
void MPWriteAudioConfig(IFile *pIFile, MPSetting* paudio);

#ifdef FEATURE_MULTISEQUENCER
void MPSetQsynthHybridMode(MPSetting* paudio, int nNote); 
#endif // FEATURE_MULTISEQUENCER

void MPSetReplayTempo(MPSetting* paudio, int nValue);

int MPGetSavedPan(MPSetting* psetting);
int MPGetPan(MPSetting* psetting);
int MPSetPan(MPSetting* paudio, int nValue);

void MPSetSuspend(MPSetting* psetting);
void MPSetResume(MPSetting* psetting);

void MPSetShowAudioSpecInfoFlag(MPSetting* psetting, boolean bShow);
boolean MPGetShowAudioSpecInfoFlag(MPSetting* psetting);

#ifdef FEATURE_QTV_GENERIC_BCAST
void    MPSetBcastTimestamp(MPSetting* psetting, uint32 nTimestamp);
uint32  MPGetBcastTimestamp(MPSetting* psetting);
void    MPSetBcastOffset(MPSetting* psetting, int32 nOffset);
int32   MPGetBcastOffset(MPSetting* psetting);
#endif // FEATURE_QTV_GENERIC_BCAST

#ifdef FEATURE_QTV_GENERIC_BCAST
int MPUnselectAllTracks(MPSetting* psetting);
int  MPReadTrackInfo(MPSetting* psetting);
char* MPGetTrackInfoString(MPSetting* psetting, int nTrackIdx);
int  MPSelectTrack(MPSetting* psetting, int nTrackIdx);
int MPMuteTrack(MPSetting* psetting, int nTrackIdx, boolean bMute);
int MPIsTrackSelected(MPSetting* psetting, int nTrackIdx, boolean* bSelect);
int MPIsTrackMuted(MPSetting* psetting, int nTrackIdx, boolean* bMute);
#endif //#ifdef FEATURE_QTV_GENERIC_BCAST

#ifdef __cplusplus
}
#endif

#endif //FEATURE_MP_EQ_UI

#endif //MPSETTINGS_H
