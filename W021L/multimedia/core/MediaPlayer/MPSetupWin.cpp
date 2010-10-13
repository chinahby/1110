/*=============================================================================
  FILE: MPSetupWin.cpp

  SERVICES: CMPSetupWin class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPSetupWin.cpp#10 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPSetupWin.h"

#ifdef FEATURE_MP_EQ_UI

//-------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------
CMPSetupWin* MPSetupWinCreate(IShell* pShell, IDisplay* pDisplay, MPSetting* pSetting, AEERect rc, int min, int max)
{
  return (CMPSetupWin*)CMPSetupWin::Create(pShell, pDisplay, pSetting, rc, min, max);
}
void MPSetupWinDelete(CMPSetupWin* obj)
{
  obj->Destroy();
}
void MPSetupWinSetEnable(CMPSetupWin* pObj, boolean bMode)
{
  if(pObj)
    pObj->Enable(bMode);
}
boolean MPSetupWinGetEnable(CMPSetupWin* pObj)
{
  if(pObj)
    return pObj->Enable();
  return FALSE;
}
void MPSetupWinSetFocus(CMPSetupWin* pObj, boolean bFocus)
{
  if(pObj)
    pObj->SetFocus(bFocus);
}
boolean MPSetupWinGetFocus(CMPSetupWin* pObj)
{
  if(pObj)
    return pObj->ShowSettingPanel();
  return FALSE;
}
void MPSetupWinSetSpectrumData(CMPSetupWin* pObj, int16* pdata, int length)
{
  if(pObj)
    pObj->SetData(pdata, length);
}
boolean MPSetupWinDraw(CMPSetupWin* pObj, IGraphics *pg, boolean bHasVideo)
{
  if(pObj)
    return pObj->Draw(pg, bHasVideo);
  return FALSE;
}

boolean MPSetupWinDrawAnalyzer(CMPSetupWin* pObj, IGraphics *pg)
{
  if(pObj)
    return pObj->DrawAnalyzer(pg);
  return FALSE;
}

int MPSetupWinOnEvent(CMPSetupWin* pObj, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  if(pObj)
    return pObj->OnEvent(eCode, wParam, dwParam);
  return FALSE;
}

void MPSetupWinSetMaxAnalyzerData(CMPSetupWin* pObj, int nValue)
{
  if(pObj)
    pObj->SetMaxAnalyzerDataValue(nValue);
}

void MPSetupWinRefreshAudioSettings(CMPSetupWin* pObj)
{
  if(pObj)
    pObj->RefreshAudioSettings();
}

boolean MPSetupWinGetSettingPanelRect(CMPSetupWin* pObj, AEERect* pRc)
{
  if(pObj)
    return pObj->GetSettingPanelRect(pRc);
  return FALSE;
}
//-------------------------------------------------------------------------------------------
//   CMPSetupWin
//-------------------------------------------------------------------------------------------
CMPSetupWin::CMPSetupWin():
  m_pAnalyzerPanel(NULL), m_pSetupPanel(NULL),
  m_bObjStatus(TRUE)
{
  if(!BuildFilterSpec())
    m_bObjStatus = FALSE;
}
CMPSetupWin::CMPSetupWin(IShell* pShell, IDisplay* pDisplay, MPSetting* pSetting, AEERect& rc, int min, int max):
  m_pAnalyzerPanel(NULL), m_pSetupPanel(NULL), m_bEnable(TRUE), m_bObjStatus(TRUE)
{
  // filters have to be created before others
  if(!BuildFilterSpec())
  {
    m_bObjStatus = FALSE;
    return;
  }

  // save device info
  m_MPInfo.m_pShell = pShell;
  m_MPInfo.m_pDisplay = pDisplay;
#ifdef FEATURE_MP_EQ_UI
  m_MPInfo.m_pSetting = pSetting;
#endif

  if(!Init(rc, min, max))
  {
    Cleanup();
    m_bObjStatus = FALSE;
    DBGPRINTF_ERROR("MP: failed to initialize CMPSetupWin.");
    return;
  }
}

boolean CMPSetupWin::BuildFilterSpec()
{
#ifdef FEATURE_MP_EQ
  m_MPInfo.m_pFilters = new CMPEQData;
  if(!m_MPInfo.m_pFilters)
  {
    DBGPRINTF_ERROR("MP: failed to initialize CMPEQData.");
    return FALSE;
  }
#endif //FEATURE_MP_EQ

  return TRUE;
}
CMPSetupWin::~CMPSetupWin()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  Cleanup();
  m_bObjStatus = FALSE;
  /*lint +e1551*/
}

void CMPSetupWin::Cleanup()
{
  DBGPRINTF_LOW("MP: begin CMPSetupWin cleaning up.");
  if(m_pAnalyzerPanel)
    delete m_pAnalyzerPanel;

  if(m_pSetupPanel)
    delete m_pSetupPanel;

  m_pAnalyzerPanel = NULL;
  m_pSetupPanel = NULL;

#ifdef FEATURE_MP_EQ
  if(m_MPInfo.m_pFilters)
  {
    delete m_MPInfo.m_pFilters;
    m_MPInfo.m_pFilters = NULL;
  }
#endif //FEATURE_MP_EQ
  DBGPRINTF_LOW("MP: end CMPSetupWin cleaning up.");
}
CMPSetupWin* CMPSetupWin::Create(IShell* pShell, IDisplay* pDisplay, MPSetting* pSetting, AEERect& rc, int min, int max)
{
  CMPSetupWin* peq;
  
  peq = new CMPSetupWin(pShell, pDisplay, pSetting, rc, min, max);
  if(peq && peq->GetCreationStatus()== TRUE)
  {
    DBGPRINTF_MED("MP: CMPSetupWin created successfully.");
    return peq;
  }
  // Unable to create Eq object
  DBGPRINTF_ERROR("MP: Unable to create CMPSetupWin object.");

  if(peq)
    delete peq;
  peq = NULL;
  return NULL;
}

boolean CMPSetupWin::GetCreationStatus()
{
  return m_bObjStatus;
}
void CMPSetupWin::Destroy()
{
  delete this;
}

boolean CMPSetupWin::Init(AEERect rcPanel, int min, int max)
{
  AEERect rc;
  int sliderPanelHeight = 72;
  int nRowHeight;

  // save panel rect
  SetFrameRect(rcPanel.x, rcPanel.y, rcPanel.dx, rcPanel.dy);

  // shrink rc by 2 on each side
  rcPanel.x += 2;
  rcPanel.y += 2;
  rcPanel.dx -= 4;
  rcPanel.dy -= 4;

  nRowHeight = IDISPLAY_GetFontMetrics(m_MPInfo.m_pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;

  // for optimizing the usage of the screen size, we set sliderPanelHeight 
  // to different value.
  if(rcPanel.dy < 130)  // SC1X (128x128)
    sliderPanelHeight = 50;  //slider range(-12 to 12)x 2 = 2*24 + 2= 50
  else if(rcPanel.dy < 150) // QIF 144x760
    sliderPanelHeight = 72;
  else  //QVGA 240x320 and above
    sliderPanelHeight = nRowHeight*(CMPSetupPanel::MP_PANEL_MAX - 1) + 4; 

  rc = rcPanel;
  m_pAnalyzerPanel = new CMPAnalyzerPanel(this);
  // analyzer panel on top
  rc.dy = rcPanel.dy - sliderPanelHeight;
  // save analyzer rc
  m_AnalyzerRc = rc;
  if(m_pAnalyzerPanel && m_pAnalyzerPanel->Init(rc) == FALSE)
  {
    DBGPRINTF_ERROR("MP: Unable to initialize m_pAnalyzerPanel.");
    delete m_pAnalyzerPanel;
    m_pAnalyzerPanel = NULL;
    return FALSE;
  }

  //slider height = 48
  rc.dy = sliderPanelHeight;
  rc.y = rcPanel.y + rcPanel.dy - rc.dy;
  // save equalizer panel rc
  m_SetupPanelRc = rc;

  // set panel colors
  GetBkColor()->SetColor(153, 204, 204);
  GetFgColor()->SetColor(0, 212, 212);
  GetSdColor()->SetColor(0, 150, 150);
  GetFlColor()->SetColor(230, 230, 230);
 
 // test expander panel 
  m_pSetupPanel = new CMPSetupPanel(this);
  if(m_pSetupPanel && m_pSetupPanel->Init(m_SetupPanelRc, min, max) == FALSE)
  {
    DBGPRINTF_ERROR("MP: Unable to initialize m_pSetupPanel.");
    delete m_pSetupPanel;
    m_pSetupPanel = NULL;
  }

  if(m_pSetupPanel)
    m_pSetupPanel->SetFocus(FALSE);

  return (m_pSetupPanel || m_pAnalyzerPanel)? TRUE:FALSE;
}

boolean CMPSetupWin::Draw(IGraphics *pg, boolean bHasVideo)
{
  CMPRect rc;
  AEERect aeeRc;

  boolean bfillMode;
  CMPColor  dwColor;
  CMPColor  flwColor;
  AEERect   clipRc;

  // draw only when enabled
  if(!m_bEnable)
    return FALSE;
  //IDISPLAY_EraseRgn

  // we may need to draw analyzer graph when not having focus
  if(!GetFocus() && !m_MPInfo.m_pSetting->m_bShowAnalyzer)
    return FALSE;

  // do not show analyzer graph when media contains video
  if(!GetFocus() && m_MPInfo.m_pSetting->m_bShowAnalyzer && bHasVideo)
    return FALSE;

  (void)GetFrameRect(&rc);
  aeeRc = *rc.GetRect();

	//if(GetFocus())
  //  MP_DrawFrame(pg, &aeeRc, AEE_FT_LOWERED, MENU8_SELECT_BACKGROUND);
  //else
  (void)MP_DrawFrame(pg, &m_SetupPanelRc, AEE_FT_RAISED, GetFlColor()->GetRGB());

  // set clip rect
  IDISPLAY_GetClipRect(m_MPInfo.m_pDisplay, &clipRc);
  IDISPLAY_SetClipRect(m_MPInfo.m_pDisplay, &aeeRc);

  bfillMode = IGRAPHICS_SetFillMode(pg, TRUE);

  dwColor = IGRAPHICS_SetColor(pg, GetFlColor()->GetR(), GetFlColor()->GetG(), GetFlColor()->GetB(), 1);
  flwColor = IGRAPHICS_SetFillColor(pg, GetFlColor()->GetR(), GetFlColor()->GetG(), GetFlColor()->GetB(), 1);

  // draw analyzer graph
  if(!bHasVideo)
    (void)DrawAnalyzer(pg);

  if(m_pSetupPanel && GetFocus())
    (void)m_pSetupPanel->Draw(pg);

  // restore default settings
  IDISPLAY_SetClipRect(m_MPInfo.m_pDisplay, &clipRc);
  (void)IGRAPHICS_SetFillMode(pg, bfillMode);
  (void)IGRAPHICS_SetColor(pg, dwColor.GetR(), dwColor.GetG(), dwColor.GetB(), 1);
  (void)IGRAPHICS_SetFillColor(pg, flwColor.GetR(), flwColor.GetG(), flwColor.GetB(), 1);

  return TRUE;
}

boolean CMPSetupWin::Draw(IGraphics *pg)
{
  CMPRect rc;
  AEERect aeeRc;

  boolean bfillMode;
  CMPColor  dwColor;
  CMPColor  flwColor;
  AEERect   clipRc;

  // draw only when enabled
  if(!m_bEnable)
    return FALSE;
  //IDISPLAY_EraseRgn

  if(!m_MPInfo.m_pSetting->m_bShowAnalyzer && !GetFocus())
    return FALSE;

  (void)GetFrameRect(&rc);
  aeeRc = *rc.GetRect();

	//if(GetFocus())
  //  MP_DrawFrame(pg, &aeeRc, AEE_FT_LOWERED, MENU8_SELECT_BACKGROUND);
  //else
  (void)MP_DrawFrame(pg, &m_SetupPanelRc, AEE_FT_RAISED, GetFlColor()->GetRGB());

  // set clip rect
  IDISPLAY_GetClipRect(m_MPInfo.m_pDisplay, &clipRc);
  IDISPLAY_SetClipRect(m_MPInfo.m_pDisplay, &aeeRc);

  bfillMode = IGRAPHICS_SetFillMode(pg, TRUE);

  dwColor = IGRAPHICS_SetColor(pg, GetFlColor()->GetR(), GetFlColor()->GetG(), GetFlColor()->GetB(), 1);
  flwColor = IGRAPHICS_SetFillColor(pg, GetFlColor()->GetR(), GetFlColor()->GetG(), GetFlColor()->GetB(), 1);

  if(m_pAnalyzerPanel && m_MPInfo.m_pSetting->m_bShowAnalyzer)
    (void)m_pAnalyzerPanel->Draw(pg);

  if(m_pSetupPanel && GetFocus())
    (void)m_pSetupPanel->Draw(pg);

  // restore default settings
  IDISPLAY_SetClipRect(m_MPInfo.m_pDisplay, &clipRc);
  (void)IGRAPHICS_SetFillMode(pg, bfillMode);
  (void)IGRAPHICS_SetColor(pg, dwColor.GetR(), dwColor.GetG(), dwColor.GetB(), 1);
  (void)IGRAPHICS_SetFillColor(pg, flwColor.GetR(), flwColor.GetG(), flwColor.GetB(), 1);

  return TRUE;
}

boolean CMPSetupWin::DrawAnalyzer(IGraphics *pg)
{
  // draw only when enabled
  if(!m_bEnable)
    return FALSE;

  if(m_pAnalyzerPanel && m_MPInfo.m_pSetting->m_bShowAnalyzer)
  {
     if(m_pSetupPanel && GetFocus())
     {
       CMPRect rc(m_AnalyzerRc);
       m_pAnalyzerPanel->SetFrameRect(rc);
     }
     else
     {
       CMPRect rc;
       (void)GetFrameRect(&rc);
       // when setup panel is not enabled, draw entire rect
       m_pAnalyzerPanel->SetFrameRect(rc);
     }
    return m_pAnalyzerPanel->Draw(pg);
  }
  return FALSE;
}

void CMPSetupWin::Enable(boolean bMode)
{
  m_bEnable = bMode;
}
boolean CMPSetupWin::Enable()
{
  return m_bEnable;
}
void CMPSetupWin::SetFocus(boolean mode)
{
  AEERect* pRC;
  CMPRect rcPanel;
  if(mode == FALSE)
  {
    (void)GetFrameRect(&rcPanel);
    pRC = rcPanel.GetRect();
    // shrink rc by 2 on each side
    pRC->x += 2;
    pRC->y += 2;
    pRC->dx -= 4;
    pRC->dy -= 4;
    m_pAnalyzerPanel->Resize(*pRC);

    // clear all focus
    if(m_pAnalyzerPanel)
    {
      m_pAnalyzerPanel->SetFocus(mode);
    }
    if(m_pSetupPanel)
      m_pSetupPanel->SetFocus(mode);
  }
  else
  {
    if(m_pSetupPanel)
      m_pSetupPanel->SetFocus(mode);

    if(m_pAnalyzerPanel)
    {
      m_pAnalyzerPanel->Resize(m_AnalyzerRc);
      m_pAnalyzerPanel->SetFocus(FALSE);
    }
  }
  CMPSymbol::SetFocus(mode);
}

boolean CMPSetupWin::ShowSettingPanel()
{
  if(m_pSetupPanel)
    return m_pSetupPanel->GetFocus();
  return FALSE;
}

boolean CMPSetupWin::OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  if(!GetFocus())
    return FALSE;

  if(m_pSetupPanel)
    return m_pSetupPanel->OnEvent(eCode, wParam, dwParam);
  return FALSE;
}

void CMPSetupWin::SetData(int16* pdata, int length)
{
  if(m_pAnalyzerPanel)
    m_pAnalyzerPanel->SetData(pdata, length);
}

void CMPSetupWin::SetMaxAnalyzerDataValue(int nValue)
{
  if(m_pAnalyzerPanel)
    m_pAnalyzerPanel->SetMaxValue(nValue);
}

void CMPSetupWin::RefreshAudioSettings()
{
  if(m_pSetupPanel)
    m_pSetupPanel->RefreshAudioSettings();
}
boolean CMPSetupWin::GetSettingPanelRect(AEERect* pRc)
{
  CMPRect rc;
  if(m_pSetupPanel)
  {  
    (void)m_pSetupPanel->GetFrameRect(&rc);
    (void)rc.GetRect(&pRc->x, &pRc->y, &pRc->dx, &pRc->dy);
    return TRUE;
  }
  return FALSE;
}
#endif //FEATURE_MP_EQ_UI
