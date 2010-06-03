/*=============================================================================
  FILE: MPSettings.c

  SERVICES: implementation file
 
  GENERAL DESCRIPTION:
    ISoundCtl helper classes/functions implementation 
  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPSettings.c#40 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPSettings.h"
#include "MediaPlayer.h"

#ifdef FEATURE_MP_EQ_UI
#include "MediaPlayer.h"
#include "AEEStdLib.h"
#include "MPSetupWin.h"

#define MP_TRACK_STATE_SELECTED          0x01 
#define MP_TRACK_STATE_MUTED             0x02 

#ifdef FEATURE_MP_EQ
static char* MPAudioGetSoundCtlStatusString(int16 status, char* pBuf, int size);
#endif //FEATURE_MP_EQ

#ifdef FEATURE_QTV_GENERIC_BCAST
static boolean MPGetTrackID(AEETrackList* pTrack, int* pBitMask, AEETrackID* pTrackID);
static char* GetTrackTypeSting(AEEQtvTrackType type);
#endif // FEATURE_QTV_GENERIC_BCAST

#ifdef FEATURE_MP_EQ
// default flat filter settings
#define DEFAULT_FILTER_NAME "flat"
AEESoundCtlEqualizerFilter FilterDefault[] = {
  0,  60,   1, 362, sizeof(AEESoundCtlEqualizerFilter),
  0,  170,  1, 362, sizeof(AEESoundCtlEqualizerFilter),
  0,  310,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  0,  600,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  0, 1000,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  0, 3000,  3, 362, sizeof(AEESoundCtlEqualizerFilter),
  0, 6000,  2, 362, sizeof(AEESoundCtlEqualizerFilter),
  0, 12000, 2, 362, sizeof(AEESoundCtlEqualizerFilter),
};

// defalut audio expender settings
AEESoundCtlAudioExpander AudioExpander= {
  AEE_SOUNDCTL_EXPANDER_HEADPHONE,
  16502,  //0x4076
  4096,   //0x1000,
  -1,
  sizeof(AEESoundCtlAudioExpander)
};
#endif //FEATURE_MP_EQ

void MPSettingRelease(MPSetting* psetting)
{
  if(psetting == NULL)
    return;
  
#ifdef FEATURE_MP_EQ
  // free sountctl interface
  if(psetting->m_pSoundCtl)
  {
    (void)ISOUNDCTL_EnableEqualizer(psetting->m_pSoundCtl, FALSE);
    (void)ISOUNDCTL_EnableAudioExpander(psetting->m_pSoundCtl, FALSE);
    (void)ISOUNDCTL_EnableSpectrumAnalyzer(psetting->m_pSoundCtl, FALSE);
    MP_RELEASEIF(psetting->m_pSoundCtl);
  }
#endif //FEATURE_MP_EQ

#ifdef FEATURE_QTV_GENERIC_BCAST

  FREEIF(((CMediaPlayer*)psetting->m_pOwner)->m_pTrackList);
  ((CMediaPlayer*)psetting->m_pOwner)->m_nTrackCount = 0;

#endif //FEATURE_QTV_GENERIC_BCAST

  if(psetting->m_pSetupWin)
  {
    MPSetupWinDelete(psetting->m_pSetupWin);
  }
  psetting->m_pSetupWin = NULL;
}

#ifdef FEATURE_MP_EQ
void GetDefaultExpanderData(stAudioExp* pdata)
{
  if(pdata)
  {
    pdata->AudioExp.dwMode = AEE_SOUNDCTL_EXPANDER_HEADPHONE;
    pdata->AudioExp.nGain = 16502;
    pdata->AudioExp.nSpread = 4096;
    pdata->AudioExp.nGeometry = -1;
    pdata->AudioExp.dwStructSize = sizeof(AEESoundCtlAudioExpander);
    pdata->curPreset = 0;
    pdata->curLevel = 10;
    pdata->numLevels = 10;
    pdata->idx = 0;
    (void)MEMSET(pdata->Name, 0, MAX_BUFFER_LENGTH*sizeof(AECHAR));
  }
}

void GetDefaultEqData(stEqualizer* pdata)
{
  if(pdata)
  {
    char  sz[32];
    (void)MEMCPY(&pdata->Filter, FilterDefault, sizeof(AEESoundCtlEqualizerFilter));
    pdata->nBands = 8;
    (void)SNPRINTF(sz, sizeof(sz), "%d", DEFAULT_FILTER_NAME);
    (void)STRTOWSTR(sz, pdata->Name, sizeof(pdata->Name));
  }
}

#ifdef MP_FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif//MP_FEATURE_AUDIO_QENSEMBLE

#endif

#ifdef FEATURE_QTV_GENERIC_BCAST
void GetDefaultBcastSettingsData(stBcastSettings* pdata)
{
  if(pdata)
  {
    pdata->nTimestamp = 0;
    pdata->nOffset = 0;
  }
}
#endif // FEATURE_QTV_GENERIC_BCAST

void MPSettingInit(MPSetting* psetting, void* pOwner)
{
  if(!psetting)
  {
    DBGPRINTF_ERROR("MP: psetting is NULL");
    return;
  }

  psetting->m_bEnableAnalyzer = FALSE;
  psetting->m_bEnableEqualizer = FALSE;
  psetting->m_bEnableAudioExpander = FALSE;
  psetting->m_bEnableAngledMixing = FALSE;
  psetting->m_bShowAnalyzer = FALSE;
  psetting->m_bEnableBcastSettings = FALSE;
  psetting->m_nFilterID = 0;
  psetting->m_nSliderID = 0;

  psetting->m_pSetupWin = NULL;
  psetting->m_bEqualizerHasFocus = FALSE;
  psetting->m_pOwner = pOwner;
  psetting->m_bEditMode = FALSE;

#ifdef FEATURE_MP_EQ
  psetting->m_pSoundCtl = NULL;
  GetDefaultEqData(&psetting->m_EqData);
  GetDefaultExpanderData(&psetting->m_AudioExpanderData);

#ifdef MP_FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif //MP_FEATURE_AUDIO_QENSEMBLE

#endif //FEATURE_MP_EQ
#ifdef FEATURE_QTV_GENERIC_BCAST
  GetDefaultBcastSettingsData(&psetting->m_BcastData);
#endif // FEATURE_QTV_GENERIC_BCAST
}

#ifdef FEATURE_MP_EQ
/*===========================================================================
   This function converts SoundCtl status string from id.
===========================================================================*/
char* MPAudioGetSoundCtlStatusString(int16 status, char* pBuf, int size)
{
  if(pBuf == NULL)
    return pBuf;

  switch(status)
  {
  case AEE_SOUNDCTL_SUCCESS:
     (void)STRLCPY(pBuf, "SUCCESS", size);
    break;
  case AEE_SOUNDCTL_FAILURE:
     (void)STRLCPY(pBuf, "FAILURE", size);
    break;
  case AEE_SOUNDCTL_UNKNOWN:
     (void)STRLCPY(pBuf, "UNKNOWN", size);
    break;
  case AEE_SOUNDCTL_UNSUPPORTED:
     (void)STRLCPY(pBuf, "UNSUPPORTED", size);
    break;
  default:
     (void)STRLCPY(pBuf, "unknown", size);
    break;
  }
  return pBuf;
}
#endif // FEATURE_MP_EQ

int MPAudioEnableEqualizer(MPSetting* psetting, boolean bEnable)
{
  int ret = EUNSUPPORTED; 

#ifdef FEATURE_MP_EQ

  char   szStatus[64] = {'\0'};

  if(!psetting)
  {
    DBGPRINTF_ERROR("MP: psetting is NULL");
    return EFAILED;
  }

  if(!psetting->m_pSoundCtl)
  {
    DBGPRINTF_ERROR("MP: psetting->m_pSoundCtl is NULL");
    return EFAILED;
  }

  ret = ISOUNDCTL_EnableEqualizer(psetting->m_pSoundCtl, bEnable);
  if(SUCCESS != ret)
  {
    DBGPRINTF_ERROR("MP: ISOUNDCTL_EnableEqualizer return error: %s", 
                      MPAudioGetSoundCtlStatusString(ret, szStatus, 64));
    return EFAILED;
  }
  else
    DBGPRINTF_LOW("MP: Enable equalizer: %s", (bEnable)?"On":"Off");
  psetting->m_bEnableEqualizer = bEnable;

#endif //FEATURE_MP_EQ

  return ret;
}

int MPAudioEnableExpander(MPSetting* psetting, boolean bEnable)
{
  int ret = EUNSUPPORTED;
  
#ifdef FEATURE_MP_EQ
  char   szStatus[64] = {'\0'};

  if(!psetting)
  {
    DBGPRINTF_ERROR("MP: psetting is NULL");
    return EFAILED;
  }

  if(!psetting->m_pSoundCtl)
  {
    DBGPRINTF_ERROR("MP: psetting->m_pSoundCtl is NULL");
    return EFAILED;
  }

  ret = ISOUNDCTL_EnableAudioExpander(psetting->m_pSoundCtl, bEnable);
  if(SUCCESS != ret)
  {
      DBGPRINTF_ERROR("MP: ISOUNDCTL_EnableAudioExpander return error: %s", 
                      MPAudioGetSoundCtlStatusString(ret, szStatus, 64));
      return EFAILED;
  }
  else
    DBGPRINTF_LOW("MP: Enable audio expander: %s", (bEnable)?"On":"Off");
  psetting->m_bEnableAudioExpander = bEnable;
#endif //FEATURE_MP_EQ  

  return ret;
}

int MPEnableBcastSettings(MPSetting* psetting, boolean bEnable)
{
  int ret = EUNSUPPORTED;
  
#ifdef FEATURE_QTV_GENERIC_BCAST
  if(!psetting)
  {
    DBGPRINTF_ERROR("MP: NULL ptr, psetting 0x%x", psetting);
    ret = EFAILED;
  }
  else
  {
    psetting->m_bEnableBcastSettings = bEnable;
    ret = SUCCESS;
  }
#endif // FEATURE_QTV_GENERIC_BCAST  

  return ret;
}

int MPAudioEnableSpectrumAnalyzer(MPSetting* psetting, boolean bEnable)
{
  int ret = EUNSUPPORTED;
#ifdef FEATURE_MP_EQ
  char   szStatus[64] = {'\0'};
  int32  nMaxDataPeriod;
  int32  nMinDataPeriod;
  int32  nDataValue;

  if(!psetting)
  {
    DBGPRINTF_ERROR("MP: psetting is NULL");
    return EFAILED;
  }

  if(!psetting->m_pSoundCtl)
  {
    DBGPRINTF_ERROR("MP: psetting->m_pSoundCtl is NULL");
    return EFAILED;
  }

  ret = ISOUNDCTL_EnableSpectrumAnalyzer(psetting->m_pSoundCtl, bEnable);
  if(SUCCESS != ret)
  {
      DBGPRINTF_ERROR("MP: ISOUNDCTL_EnableSpectrumAnalyzer return error: %s", 
                      MPAudioGetSoundCtlStatusString(ret, szStatus, 64));
      return EFAILED;
  }
  else
    DBGPRINTF_LOW("MP: Enable spectrum analyzer: %s", (bEnable)?"On":"Off");

  if(bEnable)
  {
    nDataValue = MP_SPECTRUM_DATA_PERIOD;
    // verify analyzer callback period
    if(ISOUNDCTL_GetParm(psetting->m_pSoundCtl, AEE_SOUNDCTL_PARM_SPECTRUM_DATA_PERIOD, 
                                               &nMinDataPeriod, &nMaxDataPeriod) == SUCCESS)
    {
      if(nDataValue < nMinDataPeriod) 
      {      
        nDataValue = nMinDataPeriod;
      }
      else if(nDataValue > nMaxDataPeriod)
      {
        nDataValue = nMaxDataPeriod;
      }
    }

    // set spectrum data period to 6*10 ms
    (void)MPAudioSpectrumAnalyzerCallbackPeriod(psetting, nDataValue);

    // retrieve max data value
    if(ISOUNDCTL_GetParm(psetting->m_pSoundCtl, AEE_SOUNDCTL_PARM_SPECTRUM_DATA_MAX, 
                                               &nDataValue, NULL) == SUCCESS)
    {
      // save it
      MPSetupWinSetMaxAnalyzerData(psetting->m_pSetupWin, nDataValue);
    }
  }

  psetting->m_bEnableAnalyzer = bEnable;  
#endif //FEATURE_MP_EQ
  return ret;
}


int MPAudioEnabledAngledMixing(MPSetting* psetting, boolean bEnable)
{
  int ret = EUNSUPPORTED;
  
#if (defined(FEATURE_MP_EQ) && defined(MP_FEATURE_AUDIO_QENSEMBLE))
#error code not present
#endif //#if (defined(FEATURE_MP_EQ) && defined(MP_FEATURE_AUDIO_QENSEMBLE))  

  return ret;

}

#ifdef FEATURE_MP_EQ
int MPAudioSetEqualizer(MPSetting* psetting, AEESoundCtlEqualizerFilter* pfilter, int32 nBands, AECHAR* pName)
{
  int ret = EUNSUPPORTED;
  int i;
  char   szStatus[64] = {'\0'};
  if(psetting && pfilter)
  {
    if(!psetting->m_pSoundCtl)
    {
      DBGPRINTF_ERROR("MP: m_pSoundCtl is NULL");
      return EFAILED;
    }
    ret = ISOUNDCTL_SetEqualizer( psetting->m_pSoundCtl, pfilter, nBands);
    if(SUCCESS != ret)
    {
      DBGPRINTF_ERROR("MP: ISOUNDCTL_SetEqualizer return error: %s", 
                       MPAudioGetSoundCtlStatusString(ret, szStatus, 64));
      return EFAILED;
    }
    else
    {
      for(i=0; i < nBands; i++)
      {
       if(i < MAX_NUM_OF_BANDS)
       {
          psetting->m_EqData.Filter[i].dwType = pfilter[i].dwType;
          psetting->m_EqData.Filter[i].nFreq = pfilter[i].nFreq;
          psetting->m_EqData.Filter[i].nGain = pfilter[i].nGain;
          psetting->m_EqData.Filter[i].nQF = pfilter[i].nQF;
          psetting->m_EqData.Filter[i].dwStructSize = pfilter[i].dwStructSize;
        
          DBGPRINTF_LOW("MP: Set equalizer, Bands:%d, i:%d, gain:%d, freq:%d, type:%d, qf:%d",
                nBands, i, pfilter[i].nGain, pfilter[i].nFreq, pfilter[i].dwType, pfilter[i].nQF);
        }
      }
      psetting->m_EqData.Name[0] = '\0';
      if(pName != NULL)
          (void)WSTRLCPY(psetting->m_EqData.Name, pName, MAX_BUFFER_LENGTH -1);
      psetting->m_EqData.nBands = nBands;
    }
  }
  return ret;
}
#endif //FEATURE_MP_EQ 

int MPAudioSetEqualizerGain(MPSetting* psetting, int16 nBand, int16 nGain)
{
  int ret = EUNSUPPORTED;
#ifdef FEATURE_MP_EQ  
  char   szStatus[64] = {'\0'};
  if(psetting)
  {
    if(!psetting || !psetting->m_pSoundCtl)
    {
      DBGPRINTF_ERROR("MP: NULL ptr, psetting 0x%x, psetting->m_pSoundCtl 0x%x",
                      psetting, psetting->m_pSoundCtl);
      return EFAILED;
    }
    ret = ISOUNDCTL_SetEqualizerGain( psetting->m_pSoundCtl, nBand, nGain );
    if(SUCCESS != ret)
    {
      DBGPRINTF_ERROR("MP: ISOUNDCTL_SetEqualizerGain return error: %s", 
                       MPAudioGetSoundCtlStatusString(ret, szStatus, 64));
      return EFAILED;
    }
    else
    {
      DBGPRINTF_LOW("MP: Set equalizer, band:%d, nGain:%d", nBand, nGain);
    }
  }
#endif //FEATURE_MP_EQ  
  return ret;
}

int MPAudioSpectrumAnalyzerCallbackPeriod(MPSetting* psetting, int nPeriod)
{
  int ret = EUNSUPPORTED;
#ifdef FEATURE_MP_EQ  
  char   szStatus[64] = {'\0'};

  if(!psetting)
  {
    DBGPRINTF_ERROR("MP: psetting is NULL");
    return EFAILED;
  }

  if(!psetting->m_pSoundCtl)
  {
    DBGPRINTF_ERROR("MP: psetting->m_pSoundCtl is NULL");
    return EFAILED;
  }

  ret = ISOUNDCTL_SetSpectrumAnalyzer(psetting->m_pSoundCtl, nPeriod);
  if(SUCCESS != ret)
  {
    DBGPRINTF_ERROR("MP: ISOUNDCTL_SetSpectrumAnalyzer return error: %s", 
                       MPAudioGetSoundCtlStatusString(ret, szStatus, 64));
    return EFAILED;
  }
  else
  {
    //time = nPeriod*10 ms
    DBGPRINTF_LOW("MP: Set spectrumAnalyzer callback period: %dms", nPeriod*10);
  }
#endif //FEATURE_MP_EQ  
  return ret;
}

#ifdef FEATURE_MP_EQ 
int MPAudioSetExpander(MPSetting* psetting, AEESoundCtlAudioExpander* pAuExp)
{
  int ret = EUNSUPPORTED;
  char   szStatus[64] = {'\0'};

  if(!psetting)
  {
    DBGPRINTF_ERROR("MP: psetting is NULL");
    return EFAILED;
  }

  if(!psetting->m_pSoundCtl)
  {
    DBGPRINTF_ERROR("MP: psetting->m_pSoundCtl is NULL");
    return EFAILED;
  }

  if(!pAuExp)
  {
    DBGPRINTF_ERROR("MP: pAuExp is NULL");
    return EFAILED;
  }

  ret = ISOUNDCTL_SetAudioExpander(psetting->m_pSoundCtl, pAuExp);
  if(SUCCESS != ret)
  {
    DBGPRINTF_ERROR("MP: ISOUNDCTL_SetAudioExpander return error: %s", 
                       MPAudioGetSoundCtlStatusString(ret, szStatus, 64));
    return EFAILED;
  }
  else
  {
    DBGPRINTF_LOW("MP: Set AudioExpander, Mode:%d, Gain:%d, Spread:%d, Geometry:%d, sz:%d", 
                 pAuExp->dwMode,
                 pAuExp->nGain,
                 pAuExp->nSpread,
                 pAuExp->nGeometry,
                 pAuExp->dwStructSize);
  }
  
  psetting->m_AudioExpanderData.AudioExp.dwMode       = pAuExp->dwMode;
  psetting->m_AudioExpanderData.AudioExp.nGain        = pAuExp->nGain;
  psetting->m_AudioExpanderData.AudioExp.nSpread      = pAuExp->nSpread;
  psetting->m_AudioExpanderData.AudioExp.nGeometry    = pAuExp->nGeometry;
  psetting->m_AudioExpanderData.AudioExp.dwStructSize = pAuExp->dwStructSize;
  return ret;
}

int MPQConcertSetPreset(MPSetting* psetting, short preset)
{
  int ret = EUNSUPPORTED;

  if(!psetting)
  {
    DBGPRINTF_ERROR("MP: psetting is NULL");
    return EFAILED;
  }

  if(!psetting->m_pSoundCtl)
  {
    DBGPRINTF_ERROR("MP: psetting->m_pSoundCtl is NULL");
    return EFAILED;
  }

  DBGPRINTF_MED("MP: Call QConcert Set Preset API: %d", preset);
#ifdef FEATURE_AUDFMT_QCONCERT_REVERB
  {
    AEESoundCtlAudioQconcertPlusReverb cmd;
    if((preset < AUD_DEF_QCONCERT_REVERB_PRESET_MODE_GENERIC) ||
       (preset >= AUD_DEF_QCONCERT_REVERB_PRESET_MODE_MAX))
      preset = AUD_DEF_QCONCERT_REVERB_PRESET_MODE_GENERIC;
    cmd.nCmd = AUDDEF_CMD_SET_PRESET;
    cmd.nData = (uint16)preset;
    ret = ISOUNDCTL_SetQconcertPlusReverbParams(psetting->m_pSoundCtl, &cmd);
    if(SUCCESS != ret)
    {
      char   szStatus[64] = {'\0'};
      DBGPRINTF_ERROR("MP: ISOUNDCTL_SetQconcertPlusReverbParams AUDDEF_CMD_SET_PRESET return error: %s", 
                         MPAudioGetSoundCtlStatusString(ret, szStatus, 64));
      return EFAILED;
    }
    else
    {
      DBGPRINTF_LOW("MP: Set QConcert Preset %d", preset);
    }
  }
#endif
  psetting->m_AudioExpanderData.curPreset = preset;
  return ret;
}

int MPQConcertSetLevel(MPSetting* psetting, short level)
{
  int ret = EUNSUPPORTED;

  if(!psetting)
  {
    DBGPRINTF_ERROR("MP: psetting is NULL");
    return EFAILED;
  }

  if(!psetting->m_pSoundCtl)
  {
    DBGPRINTF_ERROR("MP: psetting->m_pSoundCtl is NULL");
    return EFAILED;
  }

  DBGPRINTF_MED("MP: Call QConcert Set Level API: %d", level);
#ifdef FEATURE_AUDFMT_QCONCERT_REVERB
  {
    AEESoundCtlAudioQconcertPlusReverb cmd;
    cmd.nCmd = AUDDEF_CMD_SET_QCPR_CURRENT_LEVEL;
    cmd.nData = (uint16)level;
    ret = ISOUNDCTL_SetQconcertPlusReverbParams(psetting->m_pSoundCtl, &cmd);
    if(SUCCESS != ret)
    {
      char   szStatus[64] = {'\0'};
      DBGPRINTF_ERROR("MP: ISOUNDCTL_SetQconcertPlusReverbParams AUDDEF_CMD_SET_EXPANSION_CURRENT_LEVEL return error: %s", 
                         MPAudioGetSoundCtlStatusString(ret, szStatus, 64));
      return EFAILED;
    }
    else
    {
      DBGPRINTF_LOW("MP: Set QConcert Level %d", level);
    }
  }
#endif
  psetting->m_AudioExpanderData.curLevel = level;
  return ret;
}

int MPQConcertSetNumLevels(MPSetting* psetting, short numLevels)
{
  int ret = EUNSUPPORTED;

  if(!psetting)
  {
    DBGPRINTF_ERROR("MP: psetting is NULL");
    return EFAILED;
  }

  if(!psetting->m_pSoundCtl)
  {
    DBGPRINTF_ERROR("MP: psetting->m_pSoundCtl is NULL");
    return EFAILED;
  }

  DBGPRINTF_MED("MP: Call QConcert Set Num Levels API: %d", numLevels);
#ifdef FEATURE_AUDFMT_QCONCERT_REVERB
  {
    AEESoundCtlAudioQconcertPlusReverb cmd;
    cmd.nCmd = AUDDEF_CMD_SET_QCPR_MAX_NUM_OF_LEVELS;
    cmd.nData = (uint16)numLevels;
    ret = ISOUNDCTL_SetQconcertPlusReverbParams(psetting->m_pSoundCtl, &cmd);
    if(SUCCESS != ret)
    {
      char   szStatus[64] = {'\0'};
      DBGPRINTF_ERROR("MP: ISOUNDCTL_SetQconcertPlusReverbParams AUDDEF_CMD_SET_MAX_NUM_OF_LEVELS return error: %s", 
                         MPAudioGetSoundCtlStatusString(ret, szStatus, 64));
      return EFAILED;
    }
    else
    {
      DBGPRINTF_LOW("MP: Set QConcert Num Levels %d", numLevels);
    }
  }
#endif
  psetting->m_AudioExpanderData.numLevels = numLevels;
  return ret;
}

#ifdef MP_FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif//MP_FEATURE_AUDIO_QENSEMBLE

#endif //FEATURE_MP_EQ

boolean MPShowSetupWin(MPSetting* psetting, boolean bShow)
{
  if(psetting && psetting->m_pSetupWin)
  {
    MPSetupWinSetEnable(psetting->m_pSetupWin, bShow);
    MPSetupWinSetFocus(psetting->m_pSetupWin, bShow);
    psetting->m_bEqualizerHasFocus = bShow;
    return TRUE;
  }
  return FALSE;
}

boolean MPSetupWinIsShow(MPSetting* psetting)
{
  if(psetting && psetting->m_pSetupWin)
  {
    if(MPSetupWinGetEnable(psetting->m_pSetupWin) == FALSE)
      return FALSE;
    return MPSetupWinGetFocus(psetting->m_pSetupWin);
  }
  return FALSE;
}
boolean MPUpdateSetupWin(MPSetting* psetting,  IGraphics* pg, boolean bHasVideo)
{
  if(psetting && psetting->m_pSetupWin && pg)
  {
    (void)MPSetupWinDraw(psetting->m_pSetupWin, pg, bHasVideo);
    IGRAPHICS_Update(pg);
    return TRUE;
  }
  return FALSE;
}
boolean MPUpdateAnalyzerGraph(MPSetting* psetting,  IGraphics* pg)
{
  if(psetting && psetting->m_pSetupWin && pg)
  {
     (void)MPSetupWinDraw(psetting->m_pSetupWin, pg, TRUE);
    IGRAPHICS_Update(pg);
    return TRUE;
  }
  return FALSE;
}

boolean MPSetupFocus(MPSetting* psetting, boolean bMode)
{
  if(psetting && psetting->m_pSetupWin)
  {
    if(bMode != psetting->m_bEqualizerHasFocus)
    {
      psetting->m_bEqualizerHasFocus = bMode;
      MPSetupWinSetFocus(psetting->m_pSetupWin, bMode);
      return TRUE;
    }
  }
  return FALSE;
}

int MPSetupHandleEvent(MPSetting* psetting, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  if(psetting && psetting->m_bEqualizerHasFocus && psetting->m_pSetupWin)
  {
    return MPSetupWinOnEvent(psetting->m_pSetupWin, eCode, wParam, dwParam);
  }
  return FALSE;
}

#ifdef FEATURE_MULTISEQUENCER
/*---------------------------------------------------------------------------------------------
This function sets number of simultaneously voiced notes into IMedia object. Zero is the highest
quality playback mode. Notes can only be set when media is in MM_STATE_READY stat.

The note conversion table is defined at OEMSynthQuality.h and OEMMediaCMX.c as following
#define AEE_SYNTH_MODE_HP_ALL        0
#define AEE_SYNTH_MODE_HP_HI         1
#define AEE_SYNTH_MODE_HP_HI_MED     2   
#define AEE_SYNTH_MODE_HP_MED        3
#define AEE_SYNTH_MODE_HQ_MED        4
#define AEE_SYNTH_MODE_HQ_HI_MED     5
#define AEE_SYNTH_MODE_HQ_HI         6
#define AEE_SYNTH_MODE_HQ_ALL        7

Number of notes                 Synth mode
    57 - 76                     AEE_SYNTH_MODE_HP_ALL       
    47 - 56                     AEE_SYNTH_MODE_HP_HI        
    39 ?46                     AEE_SYNTH_MODE_HP_HI_MED 
    31 ?38                     AEE_SYNTH_MODE_HP_MED
    21 - 30                     AEE_SYNTH_MODE_HQ_MED 
    13 ?20                     AEE_SYNTH_MODE_HQ_HI_MED    
     1 ?12                     AEE_SYNTH_MODE_HQ_HI      
        0                       AEE_SYNTH_MODE_HQ_ALL   

Since the slider bar range is from 0 to 7 and the step is 1, we can convert our input to above table 
value by *9. 
---------------------------------------------------------------------------------------------*/
void MPSetQsynthHybridMode(MPSetting* psetting, int nNote)
{
  if(psetting && psetting->m_pOwner)
  {
    CMediaPlayer* pme = (CMediaPlayer*)psetting->m_pOwner;
    if(pme->m_eActiveWin == MPW_SEQUENCE_PLAYER)
    {
      CSequencePlayerWin *pwin = (CSequencePlayerWin *)pme->m_pWin;
      if(pwin)
      {
        //The note can be set into just one IMedia object, for the entire sequence 
        //by specifying the total # of notes.
        if(pme->m_MSFiles.m_wNumSequences > 0)
        {
          // since the slider bar range is from 0 to 7 and the step is 1, we can 
          // convert to above table value by *9
          nNote = nNote * 9;
          DBGPRINTF_MED ("MP: Set notes to sequencer, note %d ",  nNote);
#ifdef MM_PARM_NOTES
          if(pwin->m_sSequenceFile[0].m_pMedia)
            (void)IMEDIA_SetMediaParm(pwin->m_sSequenceFile[0].m_pMedia, MM_PARM_NOTES, nNote, 0);
#endif //#ifdef MM_PARM_NOTES
        }
      }
    }
  }
}
#endif // FEATURE_MULTISEQUENCER

void MPSetReplayTempo(MPSetting* psetting, int nValue)
{
  if(psetting && psetting->m_pOwner)
  {
    CMediaPlayer* pme = (CMediaPlayer*)psetting->m_pOwner;    
#ifdef MM_PARM_TEMPO
    int ret = SUCCESS;
    ret = IMEDIA_SetMediaParm(((CPlayerWin *)(pme->m_pWin))->m_pMedia,
                          MM_PARM_TEMPO, (uint32)nValue,0);
    DBGPRINTF_MED("MP: Set tempo to the new value, %d with return of %d", nValue, ret);
#else
    DBGPRINTF_HIGH("MP: MM_PARM_TEMPO unavailable, not setting tempo");
#endif //#ifdef MM_PARM_TEMPO
  }
}

int MPGetSavedPan(MPSetting* psetting)
{
  CMediaPlayer* pme;
  uint16 panVal = MM_MAX_PAN/2;

  if (psetting == NULL)
  {
    DBGPRINTF_HIGH("MP: Unexpected psetting = NULL!");
    return panVal;
  }
  if (psetting->m_pOwner == NULL)
  {
    DBGPRINTF_HIGH("MP: Unexpected psetting->m_pOwner = NULL!");
    return panVal;
  }
  pme = (CMediaPlayer*)psetting->m_pOwner;
  DBGPRINTF_MED("MP: MPGetSavedPan returning m_nAudioPanValue = %d", pme->m_nAudioPanValue);
  return pme->m_nAudioPanValue;
}

int MPGetPan(MPSetting* psetting)
{
  CMediaPlayer* pme;
  int nRet;
  int32 panVal=MM_MAX_PAN/2, val2=0;

  if (psetting == NULL)
  {
    DBGPRINTF_HIGH("MP: Unexpected psetting = NULL!");
    return panVal;
  }
  if (psetting->m_pOwner == NULL)
  {
    DBGPRINTF_HIGH("MP: Unexpected psetting->m_pOwner = NULL!");
    return panVal;
  }
  pme = (CMediaPlayer*)psetting->m_pOwner;
  if (pme->m_eActiveWin == MPW_PLAYER)
  {
    CPlayerWin* pwin = (CPlayerWin*)pme->m_pWin;
    if (pwin)
    {
      if (pwin->m_pMedia == NULL)
      {
        DBGPRINTF_HIGH("MP: Unexpected m_pMedia = NULL!");
        return panVal;
      }
      nRet = IMEDIA_GetMediaParm(pwin->m_pMedia, MM_PARM_PAN, &panVal, &val2);
      DBGPRINTF_MED("MP: IMEDIA_GetMediaParm returns %d = %s", nRet, MP_ErrorStateToString(nRet));
    }
  }
#ifdef FEATURE_MULTISEQUENCER
  else if (pme->m_eActiveWin == MPW_SEQUENCE_PLAYER)
  {
    CSequencePlayerWin *pwin = (CSequencePlayerWin *)pme->m_pWin;
    if (pwin)
    {
      if (pwin->m_sSequenceFile[0].m_pMedia == NULL)
      {
        DBGPRINTF_HIGH("MP: Unexpected sequence m_pMedia = NULL!");
        return panVal;
      }
      nRet = IMEDIA_GetMediaParm(pwin->m_sSequenceFile[0].m_pMedia, MM_PARM_PAN, &panVal, &val2);
      DBGPRINTF_MED("MP: IMEDIA_GetMediaParm returns %d = %s", nRet, MP_ErrorStateToString(nRet));
    }
  }
#endif //FEATURE_MULTISEQUENCER
  else
  {
    DBGPRINTF_MED("MP: MPGetPan takes no action, m_eActiveWin = %d", pme->m_eActiveWin);
  }
  DBGPRINTF_MED("MP: MPGetPan returning pan = %d", panVal);
  return panVal;
}

int MPSetPan(MPSetting* psetting, int nValue)
{
  CMediaPlayer* pme;
  int nRet = EFAILED;

  if (psetting == NULL)
  {
    DBGPRINTF_HIGH("MP: Unexpected psetting = NULL!");
    return nRet;
  }
  if (psetting->m_pOwner == NULL)
  {
    DBGPRINTF_HIGH("MP: Unexpected psetting->m_pOwner = NULL!");
    return nRet;
  }
  pme = (CMediaPlayer*)psetting->m_pOwner;
  if (pme->m_eActiveWin == MPW_PLAYER)
  {
    CPlayerWin* pwin = (CPlayerWin*)pme->m_pWin;
    if (pwin)
    {
      if (pwin->m_pMedia == NULL)
      {
        DBGPRINTF_HIGH("MP: Unexpected m_pMedia = NULL!");
        return nRet;
      }
      nRet = IMEDIA_SetPan(pwin->m_pMedia, (uint16)nValue);
      DBGPRINTF_MED("MP: IMEDIA_SetPan(%d) returns %d = %s", nValue, nRet, MP_ErrorStateToString(nRet));
      if (nRet == SUCCESS)
      {
        pme->m_nAudioPanValue = (uint16)nValue;
      }
    }
  }
#ifdef FEATURE_MULTISEQUENCER
  else if (pme->m_eActiveWin == MPW_SEQUENCE_PLAYER)
  {
    CSequencePlayerWin *pwin = (CSequencePlayerWin *)pme->m_pWin;
    if (pwin)
    {
      if (pwin->m_sSequenceFile[0].m_pMedia == NULL)
      {
        DBGPRINTF_HIGH("MP: Unexpected sequence m_pMedia = NULL!");
        return nRet;
      }
      nRet = IMEDIA_SetPan(pwin->m_sSequenceFile[0].m_pMedia, (uint16)nValue);
      DBGPRINTF_MED("MP: IMEDIA_SetPan(%d) returns %d = %s", nValue, nRet, MP_ErrorStateToString(nRet));
      if (nRet == SUCCESS)
      {
        pme->m_nAudioPanValue = (uint16)nValue;
      }
    }
  }
#endif //FEATURE_MULTISEQUENCER
  else
  {
    DBGPRINTF_MED("MP: MPSetPan takes no action, m_eActiveWin = %d", pme->m_eActiveWin);
  }

  return nRet;
}

void MPSetSuspend(MPSetting* psetting)
{
  if(psetting && psetting->m_pOwner)
  {
    CMediaPlayer* pme = (CMediaPlayer*)psetting->m_pOwner;
    // post event to suspend
    (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, ID_EVENT_SUSPEND, 0);
    DBGPRINTF_MED("MP: post ID_EVENT_SUSPEND event.");
  }
}
void MPSetResume(MPSetting* psetting)
{
  if(psetting && psetting->m_pOwner)
  {
    CMediaPlayer* pme = (CMediaPlayer*)psetting->m_pOwner;
    // post event to resume
    (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, ID_EVENT_RESUME, 0);
    DBGPRINTF_MED("MP: post ID_EVENT_RESUME event.");
  }
}

void MPSetShowAudioSpecInfoFlag(MPSetting* psetting, boolean bShow)
{
  if(psetting && psetting->m_pOwner)
  {
    CMediaPlayer* pme = (CMediaPlayer*)psetting->m_pOwner;
    pme->m_bShowAudioSpec = bShow;

    if(bShow)
      DBGPRINTF_MED("MP: enabled show audio spec info flag.");
    else
      DBGPRINTF_MED("MP: disabled show audio spec info flag.");

    if (pme->m_eActiveWin == MPW_PLAYER)
    {
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_INFO, 0);
    }
  }
}
boolean MPGetShowAudioSpecInfoFlag(MPSetting* psetting)
{
  if(psetting && psetting->m_pOwner)
  {
    CMediaPlayer* pme = (CMediaPlayer*)psetting->m_pOwner;
    return pme->m_bShowAudioSpec;
  }
  return 0;
}

#ifdef FEATURE_QTV_GENERIC_BCAST
int  MPReadTrackInfo(MPSetting* psetting)
{
  int nRet = EUNSUPPORTED;
  int i;
  if(psetting && psetting->m_pOwner)
  {
    CMediaPlayer* pme = (CMediaPlayer*)psetting->m_pOwner;
    if (pme->m_eActiveWin == MPW_PLAYER)
    {
      CPlayerWin* pwin = (CPlayerWin*)pme->m_pWin;
      if (pwin)
      {
        if (pwin->m_pMedia == NULL)
        {
          DBGPRINTF_HIGH("MP: Unexpected m_pMedia = NULL!");
          return nRet;
        }
        
        // free old track list
        FREEIF(pme->m_pTrackList);
        pme->m_nTrackCount = 0;

        // read track count
        nRet = IMEDIA_GetMediaParm(pwin->m_pMedia, MM_MP4_PARM_READ_TRACK, (int32 *)(&pme->m_nTrackCount), (int32 *)NULL);
        if(SUCCESS == nRet && pme->m_nTrackCount > 0)
        {
          DBGPRINTF_HIGH("MP: READ_TRACK return track count = %d.",pme->m_nTrackCount);
          
          pme->m_pTrackList = MALLOC(pme->m_nTrackCount*sizeof(AEETrackList));
          if(pme->m_pTrackList)
          {
            // read track information
            nRet = IMEDIA_GetMediaParm(pwin->m_pMedia, MM_MP4_PARM_READ_TRACK, 
                                      (int32*)(&pme->m_nTrackCount), (int32 *)(pme->m_pTrackList));
            if(SUCCESS != nRet)
            {
              DBGPRINTF_ERROR("MP: failed on retrieve track information, nRet = %d.", nRet);
            }
            else
            {
              // print track list for dbg
              for(i = 0; i < pme->m_nTrackCount; i++)
              {
                AEETrackList* ptrack = &pme->m_pTrackList[i];
                DBGPRINTF_HIGH("MP: Tracklist i = %d, nTrackID = %d, TrackType = %d, MediaType = %d, bmTrackState = 0X%X",
                                i, ptrack->nTrackID, ptrack->TrackType, ptrack->MediaType, ptrack->bmTrackState);
              }
            }
          }
          else
          {
            DBGPRINTF_ERROR("MP: failed to allocate memory.");
            nRet = EFAILED;
          }
        }
        else
        {
          if(SUCCESS != nRet)
          {
            DBGPRINTF_ERROR("MP: failed to retrieve track count, nRet = %d.", nRet);
          }
          else if(pme->m_nTrackCount == 0)
          {
            DBGPRINTF_ERROR("MP: IMEDIA_GetMediaParm with MM_MP4_PARM_READ_TRACK, return 0 track count.");
          }
        }
      }
    }
  }
  return nRet;
}

#endif //FEATURE_QTV_GENERIC_BCAST  

#ifdef FEATURE_QTV_GENERIC_BCAST
int MPUnselectAllTracks(MPSetting* psetting)
{
  int nRet = EUNSUPPORTED;

  int i;
  CMediaPlayer* pme = NULL;
  CPlayerWin* pwin = NULL;
  int BitMask;
  if(psetting)
  {
    pme = (CMediaPlayer*)psetting->m_pOwner;
    if (pme && pme->m_eActiveWin == MPW_PLAYER)
    {
      pwin = (CPlayerWin*)pme->m_pWin;
    }
  }

  if(!pwin || !pwin->m_pMedia)
  {
    DBGPRINTF_ERROR("MP: NULL ptr, pme = 0x%X, pMedia = 0x%X", pwin, (pwin) ? pwin->m_pMedia : 0);
    return EFAILED;
  }

  for(i = 0; i < pme->m_nTrackCount; i++)
  {
    AEETrackList* ptrack = &pme->m_pTrackList[i];
    AEETrackID trackID;
    if(MPGetTrackID(ptrack, &BitMask, &trackID))
    {
      BitMask = 0;
      nRet = IMEDIA_SetMediaParm(pwin->m_pMedia, MM_MP4_PARM_SELECT_PB, (int32)BitMask, (int32)&trackID);
      DBGPRINTF_HIGH("MP: MPUnselectAllTracks MM_MP4_PARM_SELECT_PB, BitMask = 0x%X, trackID = %d, nRet = %d", 
                        BitMask, trackID, nRet);
    }
  }
  return nRet;
}
#endif// #ifdef FEATURE_QTV_GENERIC_BCAST

int MPSelectTrack(MPSetting* psetting, int nTrackIdx)
{
  int nRet = EUNSUPPORTED;

#ifdef FEATURE_QTV_GENERIC_BCAST
  int i;
  CMediaPlayer* pme = NULL;
  CPlayerWin* pwin = NULL;

  if(psetting)
  {
    pme = (CMediaPlayer*)psetting->m_pOwner;
    if (pme && pme->m_eActiveWin == MPW_PLAYER)
    {
      pwin = (CPlayerWin*)pme->m_pWin;
    }
  }

  if(!pwin || !pwin->m_pMedia)
  {
    DBGPRINTF_ERROR("MP: NULL ptr, pme = 0x%X, pMedia = 0x%X", pwin, (pwin) ? pwin->m_pMedia : 0);
    return EFAILED;
  }

  for(i=0; i < pme->m_nTrackCount; i++)
  {
    // find track
    if(pme->m_pTrackList && pme->m_pTrackList[i].nTrackID == nTrackIdx)
    {
      int BitMask;
      AEETrackID trackID;

      if(MPGetTrackID(&pme->m_pTrackList[i], &BitMask, &trackID))
      {
        nRet = IMEDIA_SetMediaParm(pwin->m_pMedia, MM_MP4_PARM_SELECT_PB, (int32)BitMask, (int32)&trackID);
        DBGPRINTF_HIGH("MP: MPSelectTrack, MM_MP4_PARM_SELECT_PB, BitMask = 0x%X, trackID = %d, nRet = %d", 
                        BitMask, trackID, nRet);

        break;
      }
      else
      {
        DBGPRINTF_ERROR("MP: MPGetTrackID failed.");
      }
    }
  }


#endif// #ifdef FEATURE_QTV_GENERIC_BCAST

  return nRet;
}

int MPMuteTrack(MPSetting* psetting, int nTrackIdx, boolean bMute)
{
  int nRet = EUNSUPPORTED;

#ifdef FEATURE_QTV_GENERIC_BCAST
  int i;
  CMediaPlayer* pme = NULL;
  CPlayerWin* pwin = NULL;

  if(psetting)
  {
    pme = (CMediaPlayer*)psetting->m_pOwner;
    if (pme && pme->m_eActiveWin == MPW_PLAYER)
    {
      pwin = (CPlayerWin*)pme->m_pWin;
    }
  }

  if(!pwin || !pwin->m_pMedia)
  {
    DBGPRINTF_ERROR("MP: NULL ptr, pme = 0x%X, pMedia = 0x%X", pwin, (pwin) ? pwin->m_pMedia : 0);
    return EFAILED;
  }

  for(i=0; i < pme->m_nTrackCount; i++)
  {
    if(pme->m_pTrackList && pme->m_pTrackList[i].nTrackID == nTrackIdx)
    {
      int BitMask;
      AEETrackID trackID;

      if(MPGetTrackID(&pme->m_pTrackList[i], &BitMask, &trackID))
      {
        // set mute on current slected tracks
        nRet = IMEDIA_SetMediaParm(pwin->m_pMedia, MM_MP4_PARM_MUTE_TRACK, (int32)BitMask, (int32)bMute);
        DBGPRINTF_HIGH("MP: MPMuteTrack, MM_MP4_PARM_MUTE_TRACK, BitMask = 0x%X, bMute = %d, nRet = %d", 
                        BitMask, bMute, nRet);
      }
      else
      {
        DBGPRINTF_ERROR("MP: MPGetTrackID failed.");
      }
    }
  }

#endif //#ifdef FEATURE_QTV_GENERIC_BCAST

  return nRet;
}

int MPIsTrackSelected(MPSetting* psetting, int nTrackIdx, boolean* bSelect)
{
  int nRet = EUNSUPPORTED;

#ifdef FEATURE_QTV_GENERIC_BCAST
  int i;
  CMediaPlayer* pme = NULL;
  CPlayerWin* pwin = NULL;
  uint32 trackState;

  nRet = EFAILED;
  if(psetting)
  {
    pme = (CMediaPlayer*)psetting->m_pOwner;
    if (pme && pme->m_eActiveWin == MPW_PLAYER)
    {
      pwin = (CPlayerWin*)pme->m_pWin;
    }
  }

  if(!pwin || !pwin->m_pMedia)
  {
    DBGPRINTF_ERROR("MP: NULL ptr, pme = 0x%X, pMedia = 0x%X", pwin, (pwin) ? pwin->m_pMedia : 0);
    return nRet;
  }

  for(i=0; i < pme->m_nTrackCount; i++)
  {
    if(pme->m_pTrackList && pme->m_pTrackList[i].nTrackID == nTrackIdx)
    {
      trackState = pme->m_pTrackList[i].bmTrackState;
      // mask pme->m_pTrackList[i].bmTrackState for selection status
      *bSelect = ((trackState & MP_TRACK_STATE_SELECTED) == MP_TRACK_STATE_SELECTED);
      DBGPRINTF_HIGH("MP: isTrackSelected, nTrackID = %d, state = 0x%X, bSelect = %d", nTrackIdx, trackState, *bSelect);
      nRet = SUCCESS;
      break;
    }
  }

#endif //#ifdef FEATURE_QTV_GENERIC_BCAST
  return nRet;
}
// this function detected if a given track is selected
int MPIsTrackMuted(MPSetting* psetting, int nTrackIdx, boolean* bMute)
{
  int nRet = EUNSUPPORTED;

#ifdef FEATURE_QTV_GENERIC_BCAST
  int i;
  CMediaPlayer* pme = NULL;
  CPlayerWin* pwin = NULL;
  uint32 trackState;

  nRet = EFAILED;
  if(psetting)
  {
    pme = (CMediaPlayer*)psetting->m_pOwner;
    if (pme && pme->m_eActiveWin == MPW_PLAYER)
    {
      pwin = (CPlayerWin*)pme->m_pWin;
    }
  }

  if(!pwin || !pwin->m_pMedia)
  {
    DBGPRINTF_ERROR("MP: NULL ptr, pme = 0x%X, pMedia = 0x%X", pwin, (pwin) ? pwin->m_pMedia : 0);
    return nRet;
  }

  for(i=0; i < pme->m_nTrackCount; i++)
  {
    if(pme->m_pTrackList && pme->m_pTrackList[i].nTrackID == nTrackIdx)
    {
      // mask pme->m_pTrackList[i].bmTrackState for mute status
      trackState = pme->m_pTrackList[i].bmTrackState;
      *bMute = ((trackState & MP_TRACK_STATE_MUTED) == MP_TRACK_STATE_MUTED);

      DBGPRINTF_HIGH("MP: isTrackMuted, nTrackID = %d, state = 0x%X, bMute = %d", nTrackIdx, trackState, *bMute);
      nRet = SUCCESS;
      break;
    }
  }

#endif //#ifdef FEATURE_QTV_GENERIC_BCAST
  return nRet;
}

#ifdef FEATURE_QTV_GENERIC_BCAST
static boolean MPGetTrackID(AEETrackList* pTrack, int* pBitMask, AEETrackID* pTrackID)
{
  if(!pTrack || !pBitMask || !pTrackID)
  {
    DBGPRINTF_ERROR("MP: Null ptr.");
    return FALSE;
  }

  switch(pTrack->TrackType)
  {
    case TRACK_AUDIO_EVRC:
    case TRACK_AUDIO_QCELP:
    case TRACK_AUDIO_AMR:
    case TRACK_AUDIO_MP4A_LATM:
    case TRACK_WM_AUDIO:
    case TRACK_REAL_AUDIO_G2_MONO:
    case TRACK_REAL_AUDIO_G2_DUAL_MONO:
    case TRACK_REAL_AUDIO_8_STEREO:
    case TRACK_AUDIO_AMR_WB:
    case TRACK_AUDIO_AMR_WB_PLUS:
      *pBitMask = MM_QTV_AUDIO_TRACK;
      pTrackID->nAudioTrackID = pTrack->nTrackID;
      pTrackID->nVideoTrackID = 0;
      pTrackID->nTextTrackID = 0;
      break;
    case TRACK_VIDEO_MP4V_ES:
    case TRACK_VIDEO_H263_2000:
    case TRACK_VIDEO_H264:
    case TRACK_WM1_VIDEO:
    case TRACK_WM2_VIDEO:
    case TRACK_WM3_VIDEO:
    case TRACK_VIDEO_OSCAR:
    case TRACK_REAL_VIDEO_8:
    case TRACK_REAL_VIDEO_9:
      *pBitMask = MM_QTV_VIDEO_TRACK;
      pTrackID->nAudioTrackID = 0;
      pTrackID->nVideoTrackID = pTrack->nTrackID;
      pTrackID->nTextTrackID = 0;
      break;
    case TRACK_3GPP_TIMED_TEXT:
      *pBitMask = MM_QTV_TEXT_TRACK;
      pTrackID->nAudioTrackID = 0;
      pTrackID->nVideoTrackID = 0;
      pTrackID->nTextTrackID = pTrack->nTrackID;
      break;
    case TRACK_UNKNOWN:
    default:
      *pBitMask = 0;
      pTrackID->nAudioTrackID = 0;
      pTrackID->nVideoTrackID = 0;
      pTrackID->nTextTrackID = 0;

      DBGPRINTF_ERROR("MP: MPGetTrackID received unknown media type. 0x%X", pTrack->TrackType);  
      return FALSE;
  }

  return TRUE;
}
//static boolean MPGetTrackID(AEETrackList* pTrack, int* pBitMask, AEETrackID* pTrackID)
//{
//  if(!pTrack || !pBitMask || !pTrackID)
//  {
//    DBGPRINTF_ERROR("MP: Null ptr.");
//    return FALSE;
//  }
//
//  switch(pTrack->MediaType)
//  {
//    case MM_MPEG4_AUDIO_ONLY:
//      *pBitMask = MM_QTV_AUDIO_TRACK;
//      pTrackID->nAudioTrackID = pTrack->nTrackID;
//      pTrackID->nVideoTrackID = 0;
//      pTrackID->nTextTrackID = 0;
//      break;
//
//    case MM_MPEG4_VIDEO_ONLY:
//    case MM_MPEG4_VIDEO_STILL_IMAGE:
//      *pBitMask = MM_QTV_VIDEO_TRACK;
//      pTrackID->nAudioTrackID = 0;
//      pTrackID->nVideoTrackID = pTrack->nTrackID;
//      pTrackID->nTextTrackID = 0;
//      break;
//
//    case MM_MPEG4_AUDIO_VIDEO:
//    case MM_MPEG4_AUDIO_VIDEO_STILL_IMAGE:
//      *pBitMask = MM_QTV_VIDEO_TRACK & MM_QTV_AUDIO_TRACK;
//      pTrackID->nAudioTrackID = pTrack->nTrackID;
//      pTrackID->nVideoTrackID = pTrack->nTrackID;
//      pTrackID->nTextTrackID = 0;
//      break;
//
//    case MM_MPEG4_VIDEO_TEXT:
//    case MM_MPEG4_VIDEO_STILL_IMAGE_TEXT:
//      *pBitMask = MM_QTV_VIDEO_TRACK & MM_QTV_TEXT_TRACK;
//      pTrackID->nAudioTrackID = 0;
//      pTrackID->nVideoTrackID = pTrack->nTrackID;
//      pTrackID->nTextTrackID = pTrack->nTrackID;
//      break;
//
//    case MM_MPEG4_AUDIO_TEXT:
//    case MM_MPEG4_AUDIO_VIDEO_STILL_IMAGE_TEXT:
//      *pBitMask = MM_QTV_AUDIO_TRACK & MM_QTV_TEXT_TRACK;
//      pTrackID->nAudioTrackID = pTrack->nTrackID;
//      pTrackID->nVideoTrackID = 0;
//      pTrackID->nTextTrackID = pTrack->nTrackID;
//      break;
//
//    case MM_MPEG4_AUDIO_VIDEO_TEXT:
//      *pBitMask = MM_QTV_AUDIO_TRACK & MM_QTV_VIDEO_TRACK & MM_QTV_TEXT_TRACK;
//      pTrackID->nAudioTrackID = pTrack->nTrackID;
//      pTrackID->nVideoTrackID = pTrack->nTrackID;
//      pTrackID->nTextTrackID = pTrack->nTrackID;
//      break;
//
//    case MM_MPEG4_TEXT:
//      *pBitMask = MM_QTV_TEXT_TRACK;
//      pTrackID->nAudioTrackID = 0;
//      pTrackID->nVideoTrackID = 0;
//      pTrackID->nTextTrackID = pTrack->nTrackID;
//      break;
//
//    case MM_MPEG4_INVALID:
//    default:
//      *pBitMask = 0;
//      pTrackID->nAudioTrackID = 0;
//      pTrackID->nVideoTrackID = 0;
//      pTrackID->nTextTrackID = 0;
//
//      DBGPRINTF_ERROR("MP: invalid media type.");  
//      return FALSE;
//  }
//  return TRUE;
//}

#endif //FEATURE_QTV_GENERIC_BCAST

#ifdef FEATURE_QTV_GENERIC_BCAST
// this function will compose a string with requested track information
// nTrackIdx is the track list index, 
// user need to delete memory after using
char* MPGetTrackInfoString(MPSetting* psetting, int nTrackIdx)
{
  char* pString = NULL;
  AEETrackList* pTrack;
  CMediaPlayer* pme = NULL;

  if(!psetting || !psetting->m_pOwner)
  {
    DBGPRINTF_ERROR("MP: NULL ptr, psetting = 0x%X, psetting->m_pOwner = 0x%X", psetting, (psetting) ? psetting->m_pOwner : 0);
    return NULL;
  }
  pme = (CMediaPlayer*)psetting->m_pOwner;

  if(nTrackIdx >= pme->m_nTrackCount)
  {
    DBGPRINTF_ERROR("MP: idx is larger than track count");
    return pString;
  }

  pTrack = &pme->m_pTrackList[nTrackIdx];
  // 64 should be more than enough for string length
  pString = MALLOC(64*sizeof(char));
  if(pString)
  {
#ifdef MP_USE_OLD_AEETRACKLIST
    (void)SNPRINTF(pString, 64*sizeof(char), "Id(%d),%s,0x%02X", 
                  pTrack->nTrackID, 
                  GetTrackTypeSting(pTrack->TrackType), 
                  pTrack->bmTrackState);
#else
    (void)SNPRINTF(pString, 64*sizeof(char), "Id(%d),L(%s),%s,0x%02X", 
                  pTrack->nTrackID, 
                  pTrack->language, 
                  GetTrackTypeSting(pTrack->TrackType), 
                  pTrack->bmTrackState);
#endif
  }
  else
  {
    DBGPRINTF_ERROR("MP: failed to allocate memory.");
  }
  return pString;
}
#endif //#ifdef FEATURE_QTV_GENERIC_BCAST

#ifdef FEATURE_QTV_GENERIC_BCAST
static char* GetTrackTypeSting(AEEQtvTrackType type)
{
  switch (type)
  {
    case TRACK_UNKNOWN:
      return("Unknown");
    case TRACK_AUDIO_EVRC:
      return("AUD_EVRC");
    case TRACK_AUDIO_QCELP:
      return("AUD_QCELP");
    case TRACK_AUDIO_AMR:
      return("AUD_AMR");
    case TRACK_AUDIO_MP4A_LATM:
      return("AUD_AAC");
    case TRACK_VIDEO_MP4V_ES:
      return("VOD_MPEG4");
    case TRACK_VIDEO_H263_2000:
      return("VOD_H263");
    case TRACK_VIDEO_H264:
      return("VOD_H264");
    case TRACK_3GPP_TIMED_TEXT:
      return("TXT_3GPP");
    case TRACK_WM_AUDIO:
      return("AUD_WM");
    case TRACK_WM1_VIDEO:
      return("VOD_WM_7");
    case TRACK_WM2_VIDEO:
      return("VOD_WM_8");
    case TRACK_WM3_VIDEO:
      return("VOD_WM_9");
    case TRACK_VIDEO_OSCAR:
      return("VOD_OSCAR");
    case TRACK_REAL_AUDIO_G2_MONO:
      return("R_AUD_MONO");
    case TRACK_REAL_AUDIO_G2_DUAL_MONO:
      return("R_AUD_DU_MONO");
    case TRACK_REAL_AUDIO_8_STEREO:
      return("R_AUD_ST");
    case TRACK_REAL_VIDEO_8:
      return("R_VOD_8");
    case TRACK_REAL_VIDEO_9:
      return("R_VOD_9");
    case TRACK_AUDIO_AMR_WB:
      return("R_AUD_AMR_9");
    case TRACK_AUDIO_AMR_WB_PLUS:
      return("R_AUD_AMR_WM_PLUS");
    default:
      break;
  }

  DBGPRINTF_ERROR("MP: GetTrackTypeSting() received unknown track type 0x%X", type);
  return("Unknown");
}
#endif //#ifdef FEATURE_QTV_GENERIC_BCAST

#endif //FEATURE_MP_EQ_UI
