/*=============================================================================
  FILE: MPEqualizerPanel.cpp

  SERVICES: MPEqualizerPanel class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPEqualizerPanel.cpp#10 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "AEEStdLib.h"

#include "MPEqualizerPanel.h"
#ifdef FEATURE_MP_EQ_UI
#include "MPEqualizerFilterPanel.h"

CMPEqualizerPanel::CMPEqualizerPanel(void* pOwner):CMPSymbol(pOwner), 
  m_pSubPanel(NULL), m_pFilterPanel(NULL), m_nSliderID(0), m_nMin(0), m_nMax(0)
{
  SETAEERECT(&m_FilterPanelRc, 0, 0, 0, 0);
  SETAEERECT(&m_SubPanelRc, 0, 0, 0, 0);
 
}
CMPEqualizerPanel::~CMPEqualizerPanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  Cleanup();
  DBGPRINTF_LOW("MP: end deleting CMPEqualizerFilterPanel.");
  /*lint +e1551*/
}

void CMPEqualizerPanel::Cleanup()
{
  if(m_pSubPanel)
    delete m_pSubPanel;
  m_pSubPanel = NULL;
}
boolean CMPEqualizerPanel::Init(AEERect rc, int min, int max)
{
  uint32 charHight;
  boolean ret = TRUE;
  IDisplay* pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return FALSE;

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return FALSE;

  // save panel rect
  SetFrameRect(rc.x, rc.y, rc.dx, rc.dy);
  m_nMin = min;
  m_nMax = max;

  // set panel colors
  GetBkColor()->SetColor(212, 255, 255);
  GetFgColor()->SetColor(0, 0, 0);
  GetSdColor()->SetColor(0, 150, 150);
  GetFlColor()->SetColor(230, 230, 230);


  charHight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;
  DBGPRINTF_MED("MP: IDISPLAY_GetFontMetrics returns %d", charHight);

  //menu rect   
  m_FilterPanelRc.x = rc.x;
  m_FilterPanelRc.y = rc.y;
  m_FilterPanelRc.dy = rc.dy;

  // for small screen, e.g.(128x128) rc.dx is 124 = 128-4, 
  if(rc.dx < 130)
  {
    // limited space, shorten it
    m_FilterPanelRc.dx = 57;
  }
  else
    m_FilterPanelRc.dx = 65;

  // sub panel rc
  m_SubPanelRc = rc;
  m_SubPanelRc.x = m_FilterPanelRc.dx + 1;
  m_SubPanelRc.dx = rc.dx - m_SubPanelRc.x;

  // create filter panel
  CMPEqualizerFilterPanel* pEqFilterPanel;
  pEqFilterPanel = new CMPEqualizerFilterPanel(GetOwnerPtr());
  if(pEqFilterPanel && pEqFilterPanel->Init(m_FilterPanelRc))
  {
    pEqFilterPanel->SetID(MP_EQ_FILTERSETTING);
    pEqFilterPanel->SetScrollBarOnLeft(TRUE);
    pEqFilterPanel->SetFocus(TRUE);
    m_pFilterPanel = pEqFilterPanel;
  }

  rc = m_SubPanelRc;
  (void)CreateSliderPanel(rc);
  SetFocus(TRUE);

  if(pEqFilterPanel)
    pEqFilterPanel->SetFocus(TRUE);
  ret = TRUE;

  return ret;
}
/*
void CMPEqualizerPanel::SetFilter(int nFilterID)
{
  CMPEqualizerFilterPanel* pFilterPanel = (CMPEqualizerFilterPanel*)m_pFilterPanel;
  boolean ret;

	if(pFilterPanel && nFilterID >= pFilterPanel->GetItemCount())
		return;

  pFilterPanel->SetSelectItem(nFilterID);
  ret = CreateSubPanel(MP_EQ_SLIDER);
  if(ret && m_pSubPanel)
  {
    // set focus back to filter menu, just in case CreateSubPanel() changed it
    m_pSubPanel->SetFocus(FALSE);
    pFilterPanel->SetFocus(TRUE);
  }
}
int CMPEqualizerPanel::GetFilter()
{
	if(m_pFilterPanel)
		return m_pFilterPanel->GetSelectItem();
	return 0;
}
*/
boolean CMPEqualizerPanel::Draw(IGraphics *pg)
{
  if(m_pFilterPanel)
    (void)m_pFilterPanel->Draw(pg);
  if(m_pSubPanel)
    (void)m_pSubPanel->Draw(pg);
  return TRUE;
}
// set forcus
void CMPEqualizerPanel::SetFocus(boolean mode)
{
  if(mode == FALSE)
  {
    // clear all focus
    m_pFilterPanel->SetFocus(mode);
    m_pSubPanel->SetFocus(mode);
  }
  else
  {
    // alwayse set focus to EqPanel
    m_pFilterPanel->SetFocus(mode);
    m_pSubPanel->SetFocus(FALSE);
  }
  CMPSymbol::SetFocus(mode);
}

boolean CMPEqualizerPanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  boolean ret = FALSE;

  if(!GetFocus())
    return ret;

  switch(eCode)
  {
    case EVT_KEY_PRESS:
    switch(wParam)
    {
     case AVK_SELECT:
      ret = OnSelect(eCode, wParam, dwParam);
      break;
     case AVK_UP:
       ret = OnUp(eCode, wParam, dwParam);
       break;
     case AVK_DOWN:
       ret = OnDown(eCode, wParam, dwParam);
       break;
     case AVK_LEFT:
      ret = OnLeft(eCode, wParam, dwParam);
      break;
     case AVK_RIGHT:
      ret = OnRight(eCode, wParam, dwParam);
      break;
     default:
      break;
    }
    break;
  default:
    break;
  }
  return ret;
}

boolean CMPEqualizerPanel::CreateSubPanel(MPEQPANELTYPE type )
{
  AEERect rc;
  if(m_pSubPanel)
    delete m_pSubPanel;
  m_pSubPanel = NULL;

  switch(type)
  {
  case MP_EQ_SLIDER:
    // resize a little bit
    rc = m_SubPanelRc;
    return CreateSliderPanel(rc);
  case MP_EQ_FILTERSETTING:
    return CreateEqualizerFilterPanel(m_SubPanelRc);
  case MP_EQ_EXIT:
    return Exit();
  default:
    break;
  }
  return FALSE;
}
boolean CMPEqualizerPanel::OnSelect(AEEEvent /*eCode*/, uint16 /*wParam*/, uint32 /*dwParam*/)
{
  boolean ret = TRUE;
  return ret;
}
boolean CMPEqualizerPanel::OnDown(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  int sel = -1;
  boolean ret = FALSE;

  if(m_pFilterPanel && m_pFilterPanel->GetFocus())
  {
    CMPEqualizerFilterPanel* pFilterPanel = (CMPEqualizerFilterPanel*)m_pFilterPanel;
    // move to next, if end, select the first one
    if(!pFilterPanel->SetNext())
      pFilterPanel->SetFirst();

    // make selection on focusing item
    sel = pFilterPanel->GetItem();
    m_pFilterPanel->SetSelectItem(sel);

    ret = CreateSubPanel(MP_EQ_SLIDER);
    if(ret && m_pSubPanel)
    {
      // set focus back to main menu, just in case CreateSubPanel() changed it
      m_pSubPanel->SetFocus(FALSE);
      pFilterPanel->SetFocus(TRUE);
    }

  }
  else
  {
    // if sub panel has focus, pass on event
    if(m_pSubPanel)
    {
      (void)m_pSubPanel->OnEvent(eCode, wParam, dwParam);
      ret = TRUE;
    }
  }
  return ret;
}
boolean CMPEqualizerPanel::OnUp(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  int sel = -1;
  boolean ret = FALSE;

  if(m_pFilterPanel && m_pFilterPanel->GetFocus())
  {
    CMPEqualizerFilterPanel* pFilterPanel = (CMPEqualizerFilterPanel*)m_pFilterPanel;
    // move to next, if end, select the first one
    if(!pFilterPanel->SetPrevious())
      pFilterPanel->SetLast();

    // make selection on focusing item
    sel = m_pFilterPanel->GetItem();
    pFilterPanel->SetSelectItem(sel);
    
    ret = CreateSubPanel(MP_EQ_SLIDER);
    if(ret && m_pSubPanel)
    {
      // set focus back to main menu, just in case CreateSubPanel() changed it
      m_pSubPanel->SetFocus(FALSE);
      m_pFilterPanel->SetFocus(TRUE);
    }
  }
  else
  {
    if(m_pSubPanel)
    {
      (void)m_pSubPanel->OnEvent(eCode, wParam, dwParam);
      ret = TRUE;
    }
  }
  return ret;
}
boolean CMPEqualizerPanel::OnRight(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  if(!m_pFilterPanel)
    return FALSE;

  if(m_pFilterPanel->GetFocus())
  {  
    //switch focus from filter panel to sub panel
    if(m_pSubPanel)
    {
      m_pFilterPanel->SetFocus(FALSE);
      m_pSubPanel->SetFocus(TRUE);
    }
  }
  else
  {
    // stay in sub panel
    if(m_pSubPanel)
    {
      if(!m_pSubPanel->OnEvent(eCode, wParam, dwParam))
      {
        m_pFilterPanel->SetFocus(TRUE);
        m_pSubPanel->SetFocus(FALSE);
      }
    }
  }

  return TRUE;
}
boolean CMPEqualizerPanel::OnLeft(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  if(!m_pFilterPanel)
    return FALSE;

  if( m_pFilterPanel->GetFocus())
  {  
    // switch focus from Main menu panel to subpanel
    m_pFilterPanel->SetFocus(FALSE);
    m_pSubPanel->SetFocus(TRUE);
    // for slider panel, set focus on the right most
    if(m_pSubPanel->GetID() == MP_EQ_SLIDER)
      m_pSubPanel->SetItem(m_pSubPanel->GetItemCount()-1);
  }
  else
  {
    // stay in sub panel
    if(m_pSubPanel)
    {
      if(!m_pSubPanel->OnEvent(eCode, wParam, dwParam))
      {
        m_pFilterPanel->SetFocus(TRUE);
        m_pSubPanel->SetFocus(FALSE);
      }
    }
  }
  return TRUE;
}

boolean CMPEqualizerPanel::CreateSliderPanel(AEERect& rc)
{
  boolean ret = FALSE;
  CMPEqualizerSliderPanel* pSliderPane;

  pSliderPane = new CMPEqualizerSliderPanel(GetOwnerPtr());
  if(pSliderPane)
  {
    m_pSubPanel = pSliderPane;

    // when rc width smaller than 
    // 80 = (THUMB_WIDTH+1)*NUM_OF_BAR see MPSliderPanel.cpp for detail
    // the size of thumb and slider bar will be reduced.
    // 90 looks like a good threshold
    if(rc.dx > 90)
    {
      rc.x += 4;
      rc.dx -= 5;
    }
    else
    {
      // less margin with limited screen space
      rc.x += 2;
      rc.dx -= 3;
    }

    pSliderPane->SetID(MP_EQ_SLIDER);
    pSliderPane->SetSelectItem(m_nSliderID);
    pSliderPane->SetFocus(FALSE);
    if(pSliderPane->Init(rc, m_nMin, m_nMax))
      ret = TRUE;
  }
  return ret;
}

boolean CMPEqualizerPanel::CreateEqualizerFilterPanel(AEERect& rc)
{
  boolean ret = FALSE;
  CMPEqualizerFilterPanel* pEqFilterPanel;
  pEqFilterPanel = new CMPEqualizerFilterPanel(GetOwnerPtr());
  m_pSubPanel = (CMPPanel*)pEqFilterPanel;
  
  if(pEqFilterPanel && pEqFilterPanel->Init(rc))
  {
    pEqFilterPanel->SetID(MP_EQ_FILTERSETTING);
    pEqFilterPanel->SetScrollBarOnLeft(FALSE);
    ret = TRUE;
  }
  return ret;
}

boolean CMPEqualizerPanel::Exit()
{

  return TRUE;
}

#endif // FEATURE_MP_EQ_UI
