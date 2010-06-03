/*=============================================================================
  FILE: MPSetupPanel.cpp

  SERVICES: CMPSetupPanel class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPSetupPanel.cpp#18 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPSetupPanel.h"
#include "MediaPlayer.h"
#ifdef FEATURE_MP_EQ_UI
#include "MPEqualizerPanel.h"
#include "MPAudioExpanderPanel.h"
#include "MPQConcertPresetPanel.h"
#include "MPAudioAngledMixing.h"
#include "MPSettings.h"
#include "MPAudioSettingPanel.h"
#include "MPBcastSettingsPanel.h"
#include "MPQTVSettingPanel.h"
#include "MPMediaTrackPanel.h"

const char* pSetupPanelLabelNames[] = {
  "Enable Spectrum Analyzer",  //MP_LABEL_ENABLE_SPA
  "Show Spectrum Analyzer",    //MP_LABEL_SHOW_SPA
  "Enable Equalizer",          //MP_LABEL_ENABLE_EQ
  "Set up Equalizer",          //MP_LABEL_SETUP_EQ
  "Enable 3D Expander",        //MP_LABEL_ENABLE_EXP
  "Set up 3D Expander",        //MP_LABEL_SETUP_EXP
  "Set up QConcert Presets",   //MP_LABEL_SETUP_QCONCERT_PRESET
  "Enable AngledMixing",       //MP_LABEL_ENABLE_ANGLE_MIXING
  "Set up AngledMixing",       //MP_LABEL_SETUP_ANGLE_MIXING
  "Audio Settings",            //MP_LABEL_AUDIO_SETTINGS
  "Media Tracks",              //MP_LABEL_TRACKS
  "QTV Test",                  //MP_LABEL_QTV_TEST
  "Enable Bcast Settings",     //MP_LABEL_ENABLE_BCAST
  "Bcast Settings",            //MP_LABEL_BCAST_SETTINGS
  NULL
};

CMPSetupPanel::CMPSetupPanel(void* pOwner):
  CMPSymbol(pOwner), m_pListWin(NULL), m_pSubPanel(NULL), m_bShowAnalyzer(TRUE), m_nSliderMin(0), m_nSliderMax(0)
{
  
  DBGPRINTF_MED("MP: new CMPSetupPanel");

  int i;
  for(i=0; i< MP_PANEL_MAX; i++)
  {
    m_pLabels[i] = NULL;
    m_NameString[i] = NULL;
  }
}

CMPSetupPanel::~CMPSetupPanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  ClearLabels();

  if(m_pSubPanel)
    delete m_pSubPanel;
  m_pSubPanel = NULL;

  if(m_pListWin)
    delete m_pListWin;
  m_pListWin = NULL;

  DBGPRINTF_LOW("MP: end deleting CMPSetupPanel.");
  /*lint +e1551*/
}

boolean CMPSetupPanel::Init(AEERect rc, int nSliderMin, int nSliderMax)
{
  boolean    bRet = TRUE;
  int        iRow = 0;
  uint32     iRowHeight;
  IDisplay*  pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return FALSE;

  DBGPRINTF_MED("MP: CMPSetupPanelInit");

  m_nSliderMin = nSliderMin;
  m_nSliderMax = nSliderMax;

  GetBkColor()->SetColor(212, 255, 255);
  GetFgColor()->SetColor(0, 212, 212);
  GetSdColor()->SetColor(0, 150, 150);

  ClearLabels();
 
  for(int i=0; i < MP_PANEL_MAX; i++)
  {
    m_NameString[i] = MP_ToAEEChar(pSetupPanelLabelNames[i]);
  }

  SetFrameRect(rc.x, rc.y, rc.dx, rc.dy);

  m_pListWin = new CMPListWin<CMPLabel>(pDeviceInfo->m_pShell, pDeviceInfo->m_pDisplay, rc); 
  if(!m_pListWin)
  {
    DBGPRINTF_ERROR("MP: Error creating m_pListWin.");
    return FALSE;
  }
  else
  {
    DBGPRINTF_MED("MP: Setup Window, m_pListWin created.");
  }

  pDisplay = pDeviceInfo->m_pDisplay;
  iRowHeight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;

  if(!m_pListWin->Init(iRowHeight, 1, 3))
  {
    delete m_pListWin;
    m_pListWin = NULL;
    DBGPRINTF_ERROR("MP: Failed to Init m_pListWin.");
    return FALSE;
  }
  else
  {
    DBGPRINTF_MED("MP: Setup Window, m_pListWin Initialized.");
  }

  m_pListWin->SetLeftMargin(2);
  m_pListWin->SetScrollBarOnLeft(FALSE);

  m_pListWin->GetBkColor()->SetColor(153, 204, 204);
  m_pListWin->GetFgColor()->SetColor(0, 0, 0);
  m_pListWin->GetSdColor()->SetColor(150, 150, 150);

  DBGPRINTF_MED("MP: Setup Window margins and colors.");

#ifdef FEATURE_MP_EQ
  boolean    bState;
  MPSetting* pSetting;
  pSetting = pDeviceInfo->m_pSetting;

  if (bRet)
  {
    bState = pSetting->m_bEnableAnalyzer;
    bRet = CreateCheckbox(rc, iRow++, INDENT_0, MP_LABEL_ENABLE_SPA, 
                          m_NameString[MP_LABEL_ENABLE_SPA], bState);
  }
  if (bRet)
  {
    bState = pSetting->m_bShowAnalyzer && pSetting->m_bEnableAnalyzer;
    bRet = CreateCheckbox(rc, iRow++, INDENT_1, MP_LABEL_SHOW_SPA, 
                          m_NameString[MP_LABEL_SHOW_SPA], bState);
  }
  if (bRet)
  {
    bState = pSetting->m_bEnableEqualizer;
    bRet = CreateCheckbox(rc, iRow++, INDENT_0, MP_LABEL_ENABLE_EQ, 
                          m_NameString[MP_LABEL_ENABLE_EQ], bState);
  }
  if (bRet)
  {
    bRet = CreateButton(rc, iRow++, INDENT_1, MP_LABEL_SETUP_EQ, 
                        m_NameString[MP_LABEL_SETUP_EQ]);
  }
  if (bRet)
  {
    bState = pSetting->m_bEnableAudioExpander;
    bRet = CreateCheckbox(rc, iRow++, INDENT_0, MP_LABEL_ENABLE_EXP, 
                          m_NameString[MP_LABEL_ENABLE_EXP], bState );
  }
  if (bRet)
  {
    bRet = CreateButton(rc, iRow++, INDENT_1, MP_LABEL_SETUP_EXP, 
                        m_NameString[MP_LABEL_SETUP_EXP]);
  }
#ifdef FEATURE_AUDFMT_QCONCERT_REVERB
  if (bRet)
  {
    bRet = CreateButton(rc, iRow++, INDENT_1, MP_LABEL_SETUP_QCONCERT_PRESET, 
                        m_NameString[MP_LABEL_SETUP_QCONCERT_PRESET]);
  }
#endif

//----------------------------------------------------------------------
  if (bRet)
  {
    bState = pSetting->m_bEnableAngledMixing;
    bRet = CreateCheckbox(rc, iRow++, INDENT_0, MP_LABEL_ENABLE_ANGLE_MIXING, 
                          m_NameString[MP_LABEL_ENABLE_ANGLE_MIXING], bState );
  }
  if (bRet)
  {
    bRet = CreateButton(rc, iRow++, INDENT_1, MP_LABEL_SETUP_ANGLE_MIXING, 
                        m_NameString[MP_LABEL_SETUP_ANGLE_MIXING]);
  }


#endif //FEATURE_MP_EQ

  if (bRet)
  {
    bRet = CreateButton(rc, iRow++, INDENT_0, MP_LABEL_AUDIO_SETTINGS, 
                        m_NameString[MP_LABEL_AUDIO_SETTINGS]);
  }
  if (bRet)
  {
    bRet = CreateButton(rc, iRow++, INDENT_0, MP_LABEL_QTV_TEST, 
                        m_NameString[MP_LABEL_QTV_TEST]);
  }
  if (bRet)
  {
    bRet = CreateButton(rc, iRow++, INDENT_0, MP_LABEL_TRACKS, 
                        m_NameString[MP_LABEL_TRACKS]);
  }

#ifdef FEATURE_QTV_GENERIC_BCAST
  if (bRet)
  {
    bRet = CreateCheckbox(rc, iRow++, INDENT_0, MP_LABEL_ENABLE_BCAST, 
                          m_NameString[MP_LABEL_ENABLE_BCAST], FALSE);
  }
  if (bRet)
  {
    bRet = CreateButton(rc, iRow++, INDENT_1, MP_LABEL_BCAST_SETTINGS, 
                        m_NameString[MP_LABEL_BCAST_SETTINGS]);
  }
#endif // FEATURE_QTV_GENERIC_BCAST

  if (!bRet)
  {
      DBGPRINTF_ERROR("MP: Error initializing Setup Panel");
      ClearLabels();
      if (m_pListWin)
      {
        delete m_pListWin;
        m_pListWin = NULL;
        return FALSE;
      }
  }

#ifdef FEATURE_MP_EQ
  // sync button states
  ApplyRules(MP_LABEL_ENABLE_EQ, MP_LABEL_SETUP_EQ);
  ApplyRules(MP_LABEL_ENABLE_EXP, MP_LABEL_SETUP_EXP);
#ifdef FEATURE_AUDFMT_QCONCERT_REVERB
  ApplyRules(MP_LABEL_ENABLE_EXP, MP_LABEL_SETUP_QCONCERT_PRESET);
#endif
  ApplyRules(MP_LABEL_ENABLE_ANGLE_MIXING, MP_LABEL_SETUP_ANGLE_MIXING);
#endif //FEATURE_MP_EQ

  ReBuildList();
  SetFocus(TRUE);
  return TRUE;
}

boolean CMPSetupPanel::CreateCheckbox(AEERect rc, int iRow, int indent, int id, AECHAR* pName, boolean bState)
{
  int          iRowHeight;  
  CMPCheckBox* pCheckBox;
  IDisplay*    pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return FALSE;

  pCheckBox = new CMPCheckBox;
  if (!pCheckBox)
  {
    DBGPRINTF_ERROR("MP: Error creating checkbox for id %d", id);
    return FALSE;
  }
  else
  {
    DBGPRINTF_MED("MP: Creating checkbox for id %d", id);
  }

  m_pLabels[id] = pCheckBox;
  // save owner into label, so that we can pass the screen info
  // owner info and id should be set before other actions
  m_pLabels[id]->SetOwnerPtr(GetOwnerPtr());
  m_pLabels[id]->SetID(id);

  m_pLabels[id]->SetName(pName);
  m_pLabels[id]->SetIndentLevel(indent);
  pCheckBox->SetSelection(!bState);
  (void)ToggleLabelState(pCheckBox);

  pDisplay = pDeviceInfo->m_pDisplay;
  iRowHeight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;

  m_pLabels[id]->SetFrameRect(rc.x + 1, rc.y + iRow*iRowHeight, rc.dx/3, iRowHeight);

  m_pLabels[id]->GetBkColor()->SetColor(153, 204, 204); //212, 255, 255
  m_pLabels[id]->GetFgColor()->SetColor(0, 212, 212);
  m_pLabels[id]->GetSdColor()->SetColor(0, 150, 150);
  m_pLabels[id]->GetTextColor()->SetColor(0, 0, 0);

  return TRUE;
}

boolean CMPSetupPanel::CreateButton(AEERect rc, int iRow, int indent, int id, AECHAR* pName)
{
  int          iRowHeight;  
  CMPButton*   pButton;
  IDisplay*    pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return FALSE;

  pButton = new CMPButton;
  if(!pButton)
  {
    DBGPRINTF_ERROR("MP: Error creating button for id %d", id);
    return FALSE;
  }
  else
  {
    DBGPRINTF_MED("MP: Creating button for id %d", id);
  }

  m_pLabels[id] = pButton;
  m_pLabels[id]->SetName(pName);
  m_pLabels[id]->SetIndentLevel(indent);

  pDisplay = pDeviceInfo->m_pDisplay;
  iRowHeight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;

// save owner into label, so that we can pass the screen info
  m_pLabels[id]->SetOwnerPtr(GetOwnerPtr());
  m_pLabels[id]->SetID(id);
  m_pLabels[id]->SetFrameRect(rc.x + 1, rc.y + iRow*iRowHeight, rc.dx/3, iRowHeight);

  m_pLabels[id]->GetBkColor()->SetColor(153, 204, 204); //212, 255, 255
  m_pLabels[id]->GetFgColor()->SetColor(0, 212, 212);
  m_pLabels[id]->GetSdColor()->SetColor(0, 150, 150);
  m_pLabels[id]->GetTextColor()->SetColor(0, 0, 0);

  return TRUE;
}

void CMPSetupPanel::ReBuildList()
{
  int i;
  m_pListWin->Empty();

#ifdef FEATURE_MP_EQ
  for(i=0; i <= MP_LABEL_SETUP_ANGLE_MIXING; i++)
  {
    if(m_pLabels[i] != NULL)
    {
      // remove this MP_LABEL_SHOW_SPA feature for now
      if (MP_LABEL_SHOW_SPA == m_pLabels[i]->GetID())
        continue;

      if (MP_LABEL_SHOW_SPA == m_pLabels[i]->GetID() ||
          MP_LABEL_SETUP_EQ == m_pLabels[i]->GetID() ||
          MP_LABEL_SETUP_EXP== m_pLabels[i]->GetID() ||
#ifdef FEATURE_AUDFMT_QCONCERT_REVERB
          MP_LABEL_SETUP_QCONCERT_PRESET == m_pLabels[i]->GetID() ||
#endif
          MP_LABEL_SETUP_ANGLE_MIXING == m_pLabels[i]->GetID())

      {
        if (m_pLabels[i]->Enable())
        {
          m_pListWin->Add(m_pLabels[i]);
        }
      }
      else
      {
        m_pListWin->Add(m_pLabels[i]);
      }
    }
  }
#endif //FEATURE_MP_EQ

  for (i = MP_LABEL_AUDIO_SETTINGS; i <= MP_LABEL_QTV_TEST; i++)
  {
    if(m_pLabels[i] != NULL)
    {
      m_pListWin->Add(m_pLabels[i]);
    }
  }

#ifdef FEATURE_QTV_GENERIC_BCAST
  if (m_pLabels[MP_LABEL_ENABLE_BCAST] && m_pLabels[MP_LABEL_ENABLE_BCAST]->Enable())
  {
    m_pListWin->Add(m_pLabels[MP_LABEL_ENABLE_BCAST]);
  }
  if(m_pLabels[MP_LABEL_BCAST_SETTINGS])
  {
    m_pListWin->Add(m_pLabels[MP_LABEL_BCAST_SETTINGS]);
  }
#endif // FEATURE_QTV_GENERIC_BCAST
}

CMPSymbol* CMPSetupPanel::CreatePanel(MPLABELTYPE type)
{
  CMPSymbol* pPanel = NULL;
  CMPRect rc;

  (void)GetFrameRect(&rc);
  switch(type)
  {
  case MP_LABEL_SETUP_EQ:
    pPanel = new CMPEqualizerPanel(GetOwnerPtr());
    if(pPanel && ((CMPEqualizerPanel*)pPanel)->Init(*rc.GetRect(), m_nSliderMin, m_nSliderMax) == FALSE)
    {
      delete pPanel;
      pPanel = NULL;
      DBGPRINTF_ERROR("MP: Error on initializing CMPEqualizerPanel");
    }
    else if(pPanel == NULL)
    {
      DBGPRINTF_ERROR("MP: Error on creating CMPEqualizerPanel");
    }
    break;

  case MP_LABEL_SETUP_EXP:
    pPanel = new CMPAudioExpanderPanel(GetOwnerPtr());
    if(pPanel && ((CMPAudioExpanderPanel*)pPanel)->Init(*rc.GetRect()) == FALSE)
    {
      delete pPanel;
      pPanel = NULL;
      DBGPRINTF_ERROR("MP: Error on initializing CMPAudioExpanderPanel");
    }
    else if(pPanel == NULL)
    {
      DBGPRINTF_ERROR("MP: Error on creating CMPAudioExpanderPanel");
    }
    break;

  case MP_LABEL_SETUP_QCONCERT_PRESET:
#ifdef FEATURE_AUDFMT_QCONCERT_REVERB
    pPanel = new CMPQConcertPresetPanel(GetOwnerPtr());
    if(pPanel && ((CMPQConcertPresetPanel*)pPanel)->Init(*rc.GetRect()) == FALSE)
    {
      delete pPanel;
      pPanel = NULL;
      DBGPRINTF_ERROR("MP: Error on initializing CMPQConcertPresetPanel");
    }
    else if(pPanel == NULL)
    {
      DBGPRINTF_ERROR("MP: Error on creating CMPQConcertPresetPanel");
    }
#endif
    break;

  case MP_LABEL_SETUP_ANGLE_MIXING:
    pPanel = new CMPAudioAngledMixing(GetOwnerPtr());
    if(pPanel && ((CMPAudioAngledMixing*)pPanel)->Init(*rc.GetRect()) == FALSE)
    {
      delete pPanel;
      pPanel = NULL;
      DBGPRINTF_ERROR("MP: Error on initializing CMPAudioAngledMixing");
    }
    else if(pPanel == NULL)
    {
      DBGPRINTF_ERROR("MP: Error on creating CMPAudioAngledMixing");
    }
    break;

  case MP_LABEL_AUDIO_SETTINGS:
    pPanel = new CMPAudioSettingPanel(GetOwnerPtr());
    if(pPanel && ((CMPAudioSettingPanel*)pPanel)->Init(*rc.GetRect()) == FALSE)
    {
      delete pPanel;
      pPanel = NULL;
      DBGPRINTF_ERROR("MP: Error on initializing CMPAudioSettingPanel");
    }
    else if(pPanel == NULL)
    {
      DBGPRINTF_ERROR("MP: Error on creating CMPAudioSettingPanel");
    }
    break;

  case MP_LABEL_TRACKS:
#ifdef FEATURE_QTV_GENERIC_BCAST
    pPanel = new CMPMediaTracksPanel(GetOwnerPtr());
    if(pPanel && ((CMPMediaTracksPanel*)pPanel)->Init(*rc.GetRect()) == FALSE)
    {
      delete pPanel;
      pPanel = NULL;
      DBGPRINTF_ERROR("MP: Error on initializing CMPMediaTracksPanel");
    }
    else if(pPanel == NULL)
    {
      DBGPRINTF_ERROR("MP: failed to create CMPMediaTracksPanel");
    }
    //if(pPanel)
    //{
    //  int i;
    //  char* ps;
    //  MPSetting* pSetting;
    //  boolean bMute = FALSE;

    //  pSetting = MP_GetInfo(this)->m_pSetting;
    //  if(SUCCESS == MPReadTrackInfo(pSetting))
    //  {
    //    if(pSetting->m_nTrackCount)
    //    {
    //      (void)((CMPMediaTracksPanel*)pPanel)->CreateTrackItems(pSetting->m_nTrackCount);
    //      for(i = 0; i < pSetting->m_nTrackCount; i ++)
    //      {
    //        if(SUCCESS != MPIsTrackMuted(pSetting, pSetting->m_pTrackList[i].nTrackID, &bMute))
    //        {
    //          bMute = FALSE;
    //        }
    //        ps = MPGetTrackInfoString(pSetting, i);
    //        ((CMPMediaTracksPanel*)pPanel)->AddTrackItem(i, 
    //                 pSetting->m_pTrackList[i].nTrackID, 
    //                 !bMute,
    //                 ps);
    //        if( ps)
    //        {
    //          FREE(ps);
    //        }
    //      }
    //    }
    //  }
    //}
    break;    
#else
    {
      IShell* pIShell;
      MPDeviceInfo * pDeviceInfo = NULL;

      pDeviceInfo = MP_GetInfo(this);
      if(!pDeviceInfo)
          return NULL;

      pIShell = pDeviceInfo->m_pShell;
      (void)ISHELL_Beep(pIShell, BEEP_ERROR, FALSE);
      (void)ISHELL_Beep(pIShell, BEEP_ERROR, FALSE);
      break;
    }
#endif //FEATURE_QTV_GENERIC_BCAST

  case MP_LABEL_QTV_TEST:
    pPanel = new CMPQTVSettingPanel(GetOwnerPtr());
    if(pPanel && ((CMPQTVSettingPanel*)pPanel)->Init(*rc.GetRect()) == FALSE)
    {
      delete pPanel;
      pPanel = NULL;
      DBGPRINTF_ERROR("MP: Error on initializing CMPQTVSettingPanel");
    }
    else if(pPanel == NULL)
    {
      DBGPRINTF_ERROR("MP: Error on creating CMPQTVSettingPanel");
    }
    break;

#ifdef FEATURE_QTV_GENERIC_BCAST
  case MP_LABEL_BCAST_SETTINGS:
    pPanel = new CMPBcastSettingsPanel(GetOwnerPtr());
    if(pPanel && ((CMPBcastSettingsPanel*)pPanel)->Init(*rc.GetRect()) == FALSE)
    {
      delete pPanel;
      pPanel = NULL;
      DBGPRINTF_ERROR("MP: Error on initializing CMPBcastSettingPanel");
    }
    else if(pPanel == NULL)
    {
      DBGPRINTF_ERROR("MP: Error on creating CMPBcastSettingPanel");
    }
    break;
#endif //FEATURE_QTV_GENERIC_BCAST

  default:
    DBGPRINTF_ERROR("MP: unknown label type");
    break;
  }

  return pPanel;
}

boolean CMPSetupPanel::Draw(IGraphics *pg)
{
  DBGPRINTF_MED("MP: CMPSetupPanel Draw");
  if(pg == NULL)
  {
    DBGPRINTF_MED("MP: CMPSetupPanel pg=NULL");
    return FALSE;
  }

  if(m_pSubPanel)
  {
    DBGPRINTF_MED("MP: CMPSetupPanel Draw subpanel");
    return m_pSubPanel->Draw(pg);
  }

  return m_pListWin->Draw(pg);
}

boolean CMPSetupPanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  boolean ret = FALSE;

  DBGPRINTF_MED("MP: CMPSetupPanel OnEvent, %d", eCode);

  if(!GetFocus() || !m_pListWin)
    return ret;

  switch(eCode)
  {
    case EVT_KEY_PRESS:
    {
      switch(wParam)
      {
        case AVK_SELECT:
          DBGPRINTF_MED("MP: CMPSetupPanel OnEvent AVK_SELECT");
          ret = OnSelect(eCode, wParam, dwParam);
          break;

        case AVK_UP:
          DBGPRINTF_MED("MP: CMPSetupPanel OnEvent AVK_UP");
          if(m_pSubPanel)
          {
            ret = m_pSubPanel->OnEvent(eCode, wParam, dwParam);
          }
          else
          {
            ret = m_pListWin->SetPrevious();
            if(ret == FALSE)
            {
              ret = m_pListWin->SetLast();
            }
          }
          break;

        case AVK_DOWN:
          DBGPRINTF_MED("MP: CMPSetupPanel OnEvent AVK_DOWN");
          if(m_pSubPanel)
          {
            ret = m_pSubPanel->OnEvent(eCode, wParam, dwParam);
          }
          else
          {
            ret = m_pListWin->SetNext();
            if(ret == FALSE)
            {
              ret = m_pListWin->SetFirst();
            }
          }
          break;

        case AVK_CLR:
          DBGPRINTF_MED("MP: CMPSetupPanel OnEvent AVK_CLR");
          ret = OnClear(eCode, wParam, dwParam);
          break;

        case EVT_KEY:
          if(AVK_CLR == wParam)
            ret = OnClear(eCode, wParam, dwParam);    
          else
            return TRUE;// return TRUE for all others
          break;

        case AVK_LEFT:
        case AVK_RIGHT:
        default:
          DBGPRINTF_MED("MP: CMPSetupPanel OnEvent LEFT/RIGHT/default");
          if(m_pSubPanel)
          {
            ret = m_pSubPanel->OnEvent(eCode, wParam, dwParam);
          }
          else
          {
            ret = m_pListWin->OnEvent(eCode, wParam, dwParam);
            if(ret == TRUE)
            {
  /* 
              idx = m_pListWin->GetItem();
              bSelect = m_pCheckBoxs->GetSelection();
              m_pListWin->SetItemSelection(idx);
              if(bSelect)
                m_pCheckBoxs->SetSelection(bSelect);
  */
            }
          }
          break;
      }
      break;
    }
    //case EVT_KEY:
    //  if(AVK_CLR == wParam)
    //    ret = OnClear(eCode, wParam, dwParam);    
    //  else
    //    return TRUE;// return TRUE for all others
    //  break;
  }
  return ret;
}

boolean CMPSetupPanel::OnSelect(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  CMPSymbol* pSubPanel;
  //CMPEqualizer* pOwner;
  CMPLabel* pLabel;

  if(m_pSubPanel)
  {
    (void)m_pSubPanel->OnEvent(eCode, wParam, dwParam);
  }
  else
  {
    pLabel = m_pListWin->GetItem(m_pListWin->GetItem());
    if(pLabel)
    {
      if(pLabel->GetID() == MP_LABEL_SETUP_EQ || 
         pLabel->GetID() == MP_LABEL_SETUP_EXP||
#ifdef FEATURE_AUDFMT_QCONCERT_REVERB
         pLabel->GetID() == MP_LABEL_SETUP_QCONCERT_PRESET||
#endif
         pLabel->GetID() == MP_LABEL_AUDIO_SETTINGS ||
         pLabel->GetID() == MP_LABEL_QTV_TEST ||
         pLabel->GetID() == MP_LABEL_TRACKS ||
         pLabel->GetID() == MP_LABEL_BCAST_SETTINGS || 
         pLabel->GetID() == MP_LABEL_SETUP_ANGLE_MIXING)
      {
        pSubPanel = CreatePanel((MPLABELTYPE)pLabel->GetID());
        if(pSubPanel)
        {
          m_pSubPanel = pSubPanel;
        }
      }
      else
      {
        // clicked on checkbox for enabling/disabling
        (void)ToggleLabelState(pLabel);
      }
    }
  }
  return TRUE;
}

boolean CMPSetupPanel::OnClear(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  if(m_pSubPanel)
  {
    if(m_pSubPanel->OnEvent(eCode, wParam, dwParam) == FALSE)
    {
      delete m_pSubPanel;
      m_pSubPanel = NULL;
    }
    return TRUE;
  }
  return FALSE;
}

// This functions will toggle the state between select and unselect of a label.
// note, there are some rules will be applied for the child label
//  MP_LABEL_ENABLE_SPA
//    select/unselect -> apply to this label
//    select          -> will enable MP_LABEL_SHOW_SPA
//    unselect        -> will unselect and disable MP_LABEL_SHOW_SPA 
//  MP_LABEL_SHOW_SPA,
//    select/unselect -> only apply to this label
//  MP_LABEL_ENABLE_EQ,
//    select/unselect -> apply to this label
//    select          -> will enable MP_LABEL_SETUP_EQ
//    unselect        -> will disable MP_LABEL_SETUP_EQ 
//  MP_LABEL_SETUP_EQ,
//    select/unselect -> only apply to this label
//  MP_LABEL_ENABLE_EXP,
//    select/unselect -> apply to this label
//    select          -> will enable MP_LABEL_SETUP_EXP
//    unselect        -> will disable MP_LABEL_SETUP_EXP 
//  MP_LABEL_SETUP_EXP,
//    select/unselect -> only apply to this label
boolean CMPSetupPanel::ToggleLabelState(CMPLabel* pLabel)
{
  boolean    bEnable;
  MPSetting* pSetting;
  boolean    ret = FALSE;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return FALSE;

  pSetting = pDeviceInfo->m_pSetting;
  if(pLabel && pSetting)
  {
    bEnable = !pLabel->GetSelection();
    if(pLabel->GetID() == MP_LABEL_ENABLE_SPA)
    {
      // enable/disable spectrum analyzer
      if (SUCCESS == MPAudioEnableSpectrumAnalyzer(pSetting, bEnable))
      {
        pLabel->SetSelection(bEnable);
        // enable/disable analyzer graph
        pSetting->m_bShowAnalyzer = bEnable;

    // remove this MP_LABEL_SHOW_SPA feature for now
/*
        pLabel->SetSelection(bEnable);
        // enable/disable analyzer graph
        ApplyRules(MP_LABEL_ENABLE_SPA, MP_LABEL_SHOW_SPA);
        
        // if the analyzer is disabled, unselect show analyzer graph also
        if(!bEnable)
        {
          CMPLabel* pLab = GetLabelFromID(MP_LABEL_SHOW_SPA);
          if(pLab)
          {
            pLab->SetSelection(FALSE);
            pSetting->m_bShowAnalyzer = bEnable;
          }
        }
*/
        ret = TRUE;
      }
      else 
      {
        // turn off selection
        pLabel->SetSelection(FALSE);
        // enable/disable analyzer graph
        pSetting->m_bShowAnalyzer = FALSE;
      }
    }
    else if(pLabel->GetID() == MP_LABEL_SHOW_SPA)
    {
      // show/hide spectrum analyzer graph
      pLabel->SetSelection(bEnable);
      pSetting->m_bShowAnalyzer = bEnable;
      ret = TRUE;
    }
    else if(pLabel->GetID() == MP_LABEL_ENABLE_EQ)
    {
      // enable/disable equalizer
      if( SUCCESS == MPAudioEnableEqualizer(pSetting, bEnable))
      {
        pLabel->SetSelection(bEnable);
        ApplyRules(MP_LABEL_ENABLE_EQ, MP_LABEL_SETUP_EQ);
      }
      else
      {
        // failed to set MPAudioEnableEqualizer
        // we need to disable this option
        pLabel->SetSelection(FALSE);
        ApplyRules(MP_LABEL_ENABLE_EQ, MP_LABEL_SETUP_EQ);
      }
      ret = TRUE;
    }
    else if(pLabel->GetID() == MP_LABEL_ENABLE_EXP)
    {
      // enable/disable audio expander
      if( SUCCESS == MPAudioEnableExpander(pSetting, bEnable))
      {
        pLabel->SetSelection(bEnable);
        ApplyRules(MP_LABEL_ENABLE_EXP, MP_LABEL_SETUP_EXP);
#ifdef FEATURE_AUDFMT_QCONCERT_REVERB
        ApplyRules(MP_LABEL_ENABLE_EXP, MP_LABEL_SETUP_QCONCERT_PRESET);
#endif
      }
      else
      {
        // failed to set MPAudioEnableExpander
        // we need to disable this option
        pLabel->SetSelection(FALSE);
        ApplyRules(MP_LABEL_ENABLE_EXP, MP_LABEL_SETUP_EXP);
#ifdef FEATURE_AUDFMT_QCONCERT_REVERB
        ApplyRules(MP_LABEL_ENABLE_EXP, MP_LABEL_SETUP_QCONCERT_PRESET);
#endif
      }
      ret = TRUE;
    }
    else if(pLabel->GetID() == MP_LABEL_ENABLE_ANGLE_MIXING)
    {
      // enable/disable Angle Mixing
      if( SUCCESS == MPAudioEnabledAngledMixing(pSetting, bEnable))
      {
        pLabel->SetSelection(bEnable);
        ApplyRules(MP_LABEL_ENABLE_ANGLE_MIXING, MP_LABEL_SETUP_ANGLE_MIXING);
      }
      else
      {
        // failed to set MPAudioEnabledAngledMixing
        // we need to disable this option
        pLabel->SetSelection(FALSE);
        ApplyRules(MP_LABEL_ENABLE_ANGLE_MIXING, MP_LABEL_SETUP_ANGLE_MIXING);

        //(void)ISHELL_Beep(((CMediaPlayer*)pSetting->m_pOwner)->a.m_pIShell, BEEP_ERROR, FALSE);
        //(void)ISHELL_Beep(((CMediaPlayer*)pSetting->m_pOwner)->a.m_pIShell, BEEP_ERROR, FALSE);
      }
      ret = TRUE;
    }
    else if(pLabel->GetID() == MP_LABEL_ENABLE_BCAST)
    {
      // enable/disable equalizer
      if( SUCCESS == MPEnableBcastSettings(pSetting, bEnable))
      {
        pLabel->SetSelection(bEnable);
        ApplyRules(MP_LABEL_ENABLE_BCAST, MP_LABEL_BCAST_SETTINGS);
      }
      else
      {
        // failed to set MPBcastSettings
        // disable this option
        pLabel->SetSelection(FALSE);
        ApplyRules(MP_LABEL_ENABLE_BCAST, MP_LABEL_BCAST_SETTINGS);
      }
      ret = TRUE;
    }
  }

  if(ret)
    ReBuildList();
  return ret;
}

void CMPSetupPanel::RefreshAudioSettings()
{
  CMPCheckBox* pCheckBox;
  MPSetting* pSetting;
  boolean bEnable;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  pSetting = pDeviceInfo->m_pSetting;
  if(NULL == pSetting)
  {
    return;
  }

  pCheckBox = (CMPCheckBox*)GetLabelFromID(MP_LABEL_ENABLE_SPA);
  if(pCheckBox)
  {
    bEnable = pCheckBox->GetSelection();
    (void)MPAudioEnableSpectrumAnalyzer(pSetting, bEnable);
  }

  pCheckBox = (CMPCheckBox*)GetLabelFromID(MP_LABEL_ENABLE_EQ);
  if(pCheckBox)
  {
    bEnable = pCheckBox->GetSelection();
    if(MPAudioEnableEqualizer(pSetting, bEnable) != EFAILED)
    {    
      ApplyRules(MP_LABEL_ENABLE_EQ, MP_LABEL_SETUP_EQ);
      if(bEnable)
        SetFilter();
    }
  }

  pCheckBox = (CMPCheckBox*)GetLabelFromID(MP_LABEL_ENABLE_EXP);
  if(pCheckBox)
  {
    bEnable = pCheckBox->GetSelection();
    if(MPAudioEnableExpander(pSetting, bEnable) != EFAILED)
    {
      ApplyRules(MP_LABEL_ENABLE_EXP, MP_LABEL_SETUP_EXP);
#ifdef FEATURE_AUDFMT_QCONCERT_REVERB
      ApplyRules(MP_LABEL_ENABLE_EXP, MP_LABEL_SETUP_QCONCERT_PRESET);
#endif
      if(bEnable)
      {
        SetAudioExpander();
        SetQConcertPreset();
      }
    }
  }

  pCheckBox = (CMPCheckBox*)GetLabelFromID(MP_LABEL_ENABLE_ANGLE_MIXING);
  if(pCheckBox)
  {
    bEnable = pCheckBox->GetSelection();
    if(MPAudioEnabledAngledMixing(pSetting, bEnable) != EFAILED)
    {
      ApplyRules(MP_LABEL_ENABLE_ANGLE_MIXING, MP_LABEL_SETUP_ANGLE_MIXING);
      if(bEnable)
      {
        SetAudioAngledMixing();
      }
    }
  }

  pCheckBox = (CMPCheckBox*)GetLabelFromID(MP_LABEL_ENABLE_BCAST);
  if(pCheckBox)
  {
    bEnable = pCheckBox->GetSelection();
    if(MPEnableBcastSettings(pSetting, bEnable) != EFAILED)
    {
      ApplyRules(MP_LABEL_ENABLE_BCAST, MP_LABEL_BCAST_SETTINGS);
      if(bEnable)
      {
        SetBcastSettings();
      }
    }
  }
}

void CMPSetupPanel::SetFilter()
{
  MPSetting* pSetting;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  pSetting = pDeviceInfo->m_pSetting;
  if(pSetting)
  {
#ifdef FEATURE_MP_EQ
    DBGPRINTF_LOW("MP: restore equalizer filter.");
    stEqualizer* pFilter = &pSetting->m_EqData;
    (void)MPAudioSetEqualizer(pSetting, pFilter->Filter, pFilter->nBands, pFilter->Name);
#endif //FEATURE_MP_EQ
  }
}

void CMPSetupPanel::SetAudioExpander()
{
  MPSetting* pSetting;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  pSetting = pDeviceInfo->m_pSetting;
  if(pSetting)
  {
#ifdef FEATURE_MP_EQ
    AEESoundCtlAudioExpander* pAudioExp = &pSetting->m_AudioExpanderData.AudioExp;
    (void)MPAudioSetExpander(pSetting, pAudioExp);
#endif
  }
}

void CMPSetupPanel::SetQConcertPreset()
{
  MPSetting* pSetting;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  pSetting = pDeviceInfo->m_pSetting;
  if(pSetting)
  {
#ifdef FEATURE_MP_EQ
    (void)MPQConcertSetNumLevels(pSetting, pSetting->m_AudioExpanderData.numLevels);
    (void)MPQConcertSetPreset(pSetting, pSetting->m_AudioExpanderData.curPreset);
    (void)MPQConcertSetLevel(pSetting, pSetting->m_AudioExpanderData.curLevel);
#endif
  }
}

void CMPSetupPanel::SetAudioAngledMixing()
{
  MPSetting* pSetting;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  pSetting = pDeviceInfo->m_pSetting;
  if(pSetting)
  {
#if (defined(FEATURE_MP_EQ) && defined(MP_FEATURE_AUDIO_QENSEMBLE))
#error code not present
#endif //defined(FEATURE_MP_EQ) && defined(MP_FEATURE_AUDIO_QENSEMBLE))
  }
}

void CMPSetupPanel::SetBcastSettings()
{
  MPSetting* pSetting;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  pSetting = pDeviceInfo->m_pSetting;
  if(pSetting)
  {
#ifdef FEATURE_QTV_GENERIC_BCAST
    // Nothing to be done here.  Bcast parameters are set
    // when apprpriate player window buttons are pressed.
#endif // FEATURE_QTV_GENERIC_BCAST
  }
}

// enable/disable a child label according to the selection status of the control label
void CMPSetupPanel::ApplyRules(MPLABELTYPE controlType, MPLABELTYPE labelType)
{
  CMPLabel* pLabelCtl = NULL;
  CMPLabel* pLabel = NULL;
  int i;

  // find controller and child
  for(i=0; i < MP_PANEL_MAX; i++)
  {
    if(m_pLabels[i] && (controlType == m_pLabels[i]->GetID()))
    {
      pLabelCtl = m_pLabels[i];
    }
    else if(m_pLabels[i] && (labelType == m_pLabels[i]->GetID()))
    {
      pLabel = m_pLabels[i];
    }
  }

  if(pLabelCtl && pLabel)
  {
    if(pLabelCtl->GetSelection())
      pLabel->Enable(TRUE);
    else
      pLabel->Enable(FALSE);
  }
}

CMPLabel* CMPSetupPanel::GetLabelFromID(MPLABELTYPE id)
{
  int i;
  for(i=0; i < MP_PANEL_MAX; i++)
  {
    if(m_pLabels[i] && (m_pLabels[i]->GetID() == id))
      return m_pLabels[i];
  }
  return NULL;
}

void CMPSetupPanel::ClearLabels()
{ 
  for(int i=0; i < MP_PANEL_MAX; i++)
  {
    if(m_NameString[i])
      delete m_NameString[i];
    if(m_pLabels[i])
      delete m_pLabels[i];
    m_pLabels[i] = NULL;
    m_NameString[i] = NULL;
  }
}

#endif //FEATURE_MP_EQ_UI
