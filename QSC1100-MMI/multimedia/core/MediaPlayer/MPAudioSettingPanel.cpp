/*=============================================================================
  FILE: MPAudioSettingPanel.cpp

  SERVICES: CMPAudioSettingPanel class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPAudioSettingPanel.cpp#11 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPAudioSettingPanel.h"
#include "MPSettings.h"
#include "MediaPlayer.h"

#define TEMPO         "Tempo   "
#define QSYNTH_HYBRID "Qsynth Hybrid   "
#define MP_PAN        "Pan   "

CMPAudioSettingPanel::CMPAudioSettingPanel(void* pOwner):
  CMPSymbol(pOwner), m_pNameString(NULL), m_pListWin(NULL)
{

}

CMPAudioSettingPanel::~CMPAudioSettingPanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  Cleanup();
  /*lint +e1551*/
}

void CMPAudioSettingPanel::Cleanup()
{
  if(m_pNameString)
    delete[] m_pNameString;
  m_pNameString = NULL;

  if(m_pListWin)
  {
    m_pListWin->DeleteAll();
    delete m_pListWin;
  }
  m_pListWin = NULL;
}

boolean CMPAudioSettingPanel::Init(AEERect rc)
{
  int i;
  uint32 nRowHeight;
  int nMaxStrWidth = 0;
  AEERect rcPanel = rc;
  CMPSliderH* pSlider;
  MPSetting* pSetting;
  IDisplay* pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
  {
    return FALSE;
  }

  pDisplay = pDeviceInfo->m_pDisplay;
  pSetting = pDeviceInfo->m_pSetting;
  if(!pDisplay || !pSetting)
  {
    return FALSE;
  }

  // set colors
  GetBkColor()->SetColor(153, 204, 204);
  GetFgColor()->SetColor(0, 212, 212);
  GetSdColor()->SetColor(0, 150, 150);

  SetFrameRect(rc.x, rc.y, rc.dx, rc.dy);

  m_pListWin = new CMPListWin<CMPSymbol>(pDeviceInfo->m_pShell, pDisplay, rcPanel); 
  if(!m_pListWin)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pListWin.");
    return FALSE;
  }

  // the height of the row should be the char height + frame border
  nRowHeight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;
  // init list with 1 col and MP_AUDIO_SETTING_MAX rows
  if(!m_pListWin->Init(nRowHeight, 1, MP_AUDIO_SETTING_MAX))
  {
    delete m_pListWin;
    m_pListWin = NULL;
    DBGPRINTF_ERROR("MP: Unable to initialize m_pListWin.");
    return FALSE;
  }

  m_pListWin->GetBkColor()->SetColor(153, 204, 204);
  m_pListWin->GetFgColor()->SetColor(0, 0, 0);
  m_pListWin->GetSdColor()->SetColor(150, 150, 150);

  m_pNameString = MP_ToAEEChar("Show Audio Spec Info");

  CMPCheckBox *pCheckBox;
  CMPSliderH  *pSliderPanel;
  for(i=0; i < MP_AUDIO_SETTING_MAX; i++)
  {

    if(i == MP_AUDIO_SETTING_SPEC_INFO)
    {
      pCheckBox = new CMPCheckBox(GetOwnerPtr());
      if(pCheckBox == NULL)
      {
        DBGPRINTF_ERROR("MP: error on creating m_pSymbol %d.", i);
        Cleanup();
        return FALSE;
      }
      pCheckBox->SetID(i);
      pCheckBox->SetName(m_pNameString);

      pCheckBox->GetBkColor()->SetColor(GetBkColor()->GetRGB());
      pCheckBox->GetFgColor()->SetColor(GetFgColor()->GetRGB());
      pCheckBox->GetSdColor()->SetColor(GetSdColor()->GetRGB());
      pCheckBox->GetFlColor()->SetColor(GetFlColor()->GetRGB());

      pCheckBox->SetSelection(MPGetShowAudioSpecInfoFlag(pSetting));
      m_pListWin->Add(pCheckBox);
    }
    else
    {
      // create horizontal slider bar
      pSliderPanel = new CMPSliderH(GetOwnerPtr());
      if(pSliderPanel == NULL)
      {
        DBGPRINTF_ERROR("MP: error on creating m_pSymbol %d.", i);
        Cleanup();
        return FALSE;
      }
      pSliderPanel->SetID(i);
      pSliderPanel->SetSliderBarHight(6);
      pSliderPanel->SetSliderThumbSize(6, 12);
      (void)pSliderPanel->Init(rc.x + 4, rc.y + i*nRowHeight, rc.dx - 4, nRowHeight);

      pSliderPanel->GetBkColor()->SetColor(GetBkColor()->GetRGB());
      pSliderPanel->GetFgColor()->SetColor(GetFgColor()->GetRGB());
      pSliderPanel->GetSdColor()->SetColor(GetSdColor()->GetRGB());
      pSliderPanel->GetFlColor()->SetColor(GetFlColor()->GetRGB());

      m_pListWin->Add(pSliderPanel);
    }

  }

  // set string name, and thumb value for TEMP
  pSlider = (CMPSliderH*)m_pListWin->GetItem(MP_AUDIO_SETTING_TEMPO);
  if(pSlider == NULL)
  {
    DBGPRINTF_ERROR("MP: pSlider is NULL");
    Cleanup();
    return FALSE;
  }
  pSlider->SetLabelName(TEMPO);
  pSlider->SetSliderStep(20);
  pSlider->GetSlider()->SetLowerBound(TEMPO_MIN);
  pSlider->GetSlider()->SetUpperBound(TEMPO_MAX);
  (void)pSlider->GetSlider()->SetThumbPos(TEMPO_DEFAULT);

  // set string name, and thumb value for QSYNTH_HYBRID
  pSlider = (CMPSliderH*)m_pListWin->GetItem(MP_AUDIO_SETTING_QSYNTH_HYBRID);
  if(pSlider == NULL)
  {
    DBGPRINTF_ERROR("MP: pSlider is NULL");
    Cleanup();
    return FALSE;
  }
  pSlider->SetLabelName(QSYNTH_HYBRID);
  pSlider->SetSliderStep(QSYNTH_HYBRID_STEP);
  pSlider->GetSlider()->SetLowerBound(QSYNTH_HYBRID_MIN);
  pSlider->GetSlider()->SetUpperBound(QSYNTH_HYBRID_MAX);
  (void)pSlider->GetSlider()->SetThumbPos(QSYNTH_HYBRID_MIN);

  // set string name, and thumb value for PAN
  pSlider = (CMPSliderH*)m_pListWin->GetItem(MP_AUDIO_SETTING_PAN);
  if(pSlider == NULL)
  {
    DBGPRINTF_ERROR("MP: pSlider is NULL");
    Cleanup();
    return FALSE;
  }
  pSlider->SetLabelName(MP_PAN);
  pSlider->SetSliderStep(1);
  pSlider->GetSlider()->SetLowerBound(0);
  pSlider->GetSlider()->SetUpperBound(MM_MAX_PAN);
  (void)pSlider->GetSlider()->SetThumbPos(MPGetSavedPan(pSetting));
  (void)MPSetPan(pSetting, MPGetSavedPan(pSetting));

  // now, all sliders have name string, we can find the Max width
  nMaxStrWidth = GetMaxLabelLength();

  CMPRect rcSliderPan;
  for(i=0; i < MP_AUDIO_SETTING_MAX; i++)
  {
    if(i != MP_AUDIO_SETTING_SPEC_INFO)
    {
      pSlider = (CMPSliderH*)m_pListWin->GetItem(i);
      if(pSlider == NULL)
      {
        DBGPRINTF_ERROR("MP: pSlider is NULL");
        Cleanup();
        return FALSE;
      }
      pSlider->SetSliderOffset(nMaxStrWidth);
      (void)pSlider->GetFrameRect(&rcSliderPan);

      // re call ini to have setting take effect
      (void)pSlider->Init((rcSliderPan.GetRect())->x, (rcSliderPan.GetRect())->y, 
                        (rcSliderPan.GetRect())->dx, (rcSliderPan.GetRect())->dy);
    }
  }

  m_pListWin->SetItem(0);
  SetFocus(TRUE);
  return TRUE;
}

// this function returns max string length
int CMPAudioSettingPanel::GetMaxLabelLength()
{
  int i;
  int max = 0;
  int length;
  IDisplay* pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
  {
    return max;
  }

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
  {
    return max;
  }

  for(i=0; i < MP_AUDIO_SETTING_MAX; i++)
  {
    if(i != MP_AUDIO_SETTING_SPEC_INFO)
    {
      CMPSliderH * pSlider = ((CMPSliderH*)m_pListWin->GetItem(i));
      if (!pSlider) 
      {
          return max;
      }
      length = IDISPLAY_MeasureText(pDisplay, 
                            AEE_FONT_NORMAL, 
                            pSlider->GetNameString());
      if(length > max)
        max = length;
    }
  }
  return max;
}

boolean CMPAudioSettingPanel::Draw(IGraphics *pg)
{
  return m_pListWin->Draw(pg);
}

// key event handler
boolean CMPAudioSettingPanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  boolean bRet;
  //CMPSymbol* pItem;
  if(!GetFocus())
    return FALSE;

  bRet = m_pListWin->OnEvent(eCode, wParam, dwParam);

  switch(eCode)
  {
    case EVT_KEY_PRESS:
    switch(wParam)
    {
      case AVK_SELECT:
        OnSelect(eCode, wParam, dwParam);
        bRet = TRUE; // we need to return TRUE here
        break;
      case AVK_UP:
      case AVK_DOWN:
        break;
      case AVK_LEFT:
      case AVK_RIGHT:
       {
         OnLeftRight(eCode, wParam, dwParam);
         bRet = TRUE;
       }
       break;
     default:
       break;
    }
    break;
  default:
    break;
  }
  return bRet;
}
void CMPAudioSettingPanel::OnSelect(AEEEvent /*eCode*/, uint16 /*wParam*/, uint32 /*dwParam*/)
{
  CMPSymbol* pItem;
  MPSetting* pSetting;
  CMPCheckBox *pCheckBox;   
  boolean    bSelection;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
  {
      return;
  }

  pSetting = pDeviceInfo->m_pSetting;
  if(!pSetting)
  {
      return;
  }

  // get current focused item
  pItem = m_pListWin->GetItem(m_pListWin->GetItem());
  if(pItem)
  {

    switch(pItem->GetID())
    {
    case MP_AUDIO_SETTING_SPEC_INFO:
      pCheckBox = (CMPCheckBox*)pItem;

      bSelection = !MPGetShowAudioSpecInfoFlag(pSetting);
      MPSetShowAudioSpecInfoFlag(pSetting, bSelection);
      pCheckBox->SetSelection(bSelection);
      break;
    default:
      DBGPRINTF_MED("MP: Unexpected item ID = %d", pItem->GetID());
      break;
    }
  }
}


// Handles left and right key event
// depends on the current item selection, a corresponding setting function
// will be called. 
void CMPAudioSettingPanel::OnLeftRight(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  CMPSymbol* pItem;
  MPSetting* pSetting;
  CMPSliderH* pSliderH;
  IShell* pIShell;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if (!pDeviceInfo) 
  {
      return;
  }

  pSetting = pDeviceInfo->m_pSetting;
  pIShell = pDeviceInfo->m_pShell;
  if (!pSetting || !pIShell) 
  {
      return;
  }

  // get current focused item
  pItem = m_pListWin->GetItem(m_pListWin->GetItem());
  if(pItem)
  {

    switch(pItem->GetID())
    {
    case MP_AUDIO_SETTING_TEMPO:
      pSliderH = (CMPSliderH*)pItem;
      // update slider bar
      (void)pSliderH->OnEvent(eCode, wParam, dwParam);
      MPSetReplayTempo(pSetting, pSliderH->GetThumbValue());
      break;
#ifdef FEATURE_MULTISEQUENCER
    case MP_AUDIO_SETTING_QSYNTH_HYBRID:
      pSliderH = (CMPSliderH*)pItem;
      if(CanUpdateNoteSetting())
      {
        // update slider bar
        (void)pSliderH->OnEvent(eCode, wParam, dwParam);
        MPSetQsynthHybridMode(pSetting, pSliderH->GetThumbValue());
      }
      else
      {
        DBGPRINTF_HIGH("MP: Notes can only be set when media is in MM_STATE_READY.");
        (void)ISHELL_Beep(pIShell, BEEP_ERROR, FALSE);
        (void)ISHELL_Beep(pIShell, BEEP_ERROR, FALSE);
      }
      break;
#endif
    case MP_AUDIO_SETTING_PAN:       
      pSliderH = (CMPSliderH*)pItem;
      // update slider bar
      (void)pSliderH->OnEvent(eCode, wParam, dwParam);
      if (MPSetPan(pSetting, pSliderH->GetThumbValue()) == EUNSUPPORTED)
      {
        // Pan setting not supported, reset slider
        (void)pSliderH->GetSlider()->SetThumbPos(MPGetSavedPan(pSetting));
      }
      break;
    default:
      DBGPRINTF_MED("MP: Unexpected item ID = %d", pItem->GetID());
      break;
    }
  }
}
// return TRUE if Sequencer win is in Ready state
boolean CMPAudioSettingPanel::CanUpdateNoteSetting()
{
#ifdef FEATURE_MULTISEQUENCER
  MPSetting* pSetting;
  //uint16     nIndex;
  boolean    bChg = 0;
  int        nState = 0;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if (!pDeviceInfo) 
  {
      return FALSE;
  }

  pSetting = pDeviceInfo->m_pSetting;
  if (!pSetting) 
  {
      return FALSE;
  }

  CMediaPlayer* pme = (CMediaPlayer*)pSetting->m_pOwner;
  if(pme && pme->m_eActiveWin == MPW_SEQUENCE_PLAYER)
  {
    CSequencePlayerWin *pwin = (CSequencePlayerWin *)pme->m_pWin;
    if(pwin && (pme->m_MSFiles.m_wNumSequences > 0))
     {
       if (pwin->m_sSequenceFile[0].m_pMedia)
         nState = IMEDIA_GetState(pwin->m_sSequenceFile[0].m_pMedia, &bChg);
       if(!bChg && MM_STATE_READY == nState)
       {
         return TRUE;
       }
       else
         DBGPRINTF_ERROR("MP: Sequencer is not in READY state.");
     }
  }
  else
    DBGPRINTF_ERROR("MP: Notes can only be set when playing sequence.");

#endif // FEATURE_MULTISEQUENCER
  return FALSE;
}
