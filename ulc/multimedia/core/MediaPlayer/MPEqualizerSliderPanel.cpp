/*=============================================================================
  FILE: MPSliderPanel.cpp

  SERVICES: CMPEqualizerSliderPanel class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPEqualizerSliderPanel.cpp#4 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPEqualizerSliderPanel.h"

#ifdef FEATURE_MP_EQ_UI
#include "MPSetupWin.h"
#include "MPSetupWin.h"
#include "MPSettings.h"

//----------------------------------------------------------
//   CMPEqualizerSliderPanel
//
//-----------------------------------------------------------

CMPEqualizerSliderPanel::CMPEqualizerSliderPanel(void* pOwner):CMPPanel(pOwner), m_pBar(NULL),m_nNumofItems(NUM_OF_BAR),
                               m_nMax(0), m_nMin(0), 
                               m_byBarWidth(BAR_WIDTH), m_byThumbWidth(THUMB_WIDTH), m_byThumbHeight(THUMB_HEIGHT)
{
}
CMPEqualizerSliderPanel::CMPEqualizerSliderPanel(AEERect& /*rc*/, int /*min*/, int /*max*/):m_pBar(NULL),m_nNumofItems(NUM_OF_BAR),
                              m_nMax(0), m_nMin(0),
                              m_byBarWidth(BAR_WIDTH), m_byThumbWidth(THUMB_WIDTH), m_byThumbHeight(THUMB_HEIGHT)
{
}
CMPEqualizerSliderPanel::~CMPEqualizerSliderPanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  if(m_pBar)
    delete [] m_pBar;
  DBGPRINTF_LOW("MP: end deleting CMPEqualizerSliderPanel.");
  /*lint +e1551*/
}

CMPEqualizerSliderPanel* CMPEqualizerSliderPanel::Create(AEERect& rc, int min, int max)
{
  CMPEqualizerSliderPanel* peq;
  peq = new CMPEqualizerSliderPanel(rc, min, max);
  return peq;
}

void CMPEqualizerSliderPanel::Destroy()
{
  delete this;
}

boolean CMPEqualizerSliderPanel::Init(AEERect rc, int min, int max)
{
  int i;
  int w;
  int x, y, dx, dy;
  CMPSlider* pSliderBar;
  int pos = 0;
#ifdef FEATURE_MP_EQ
  equalizer* peq;
#endif

#ifdef FEATURE_MP_EQ
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return FALSE;

  peq = pDeviceInfo->m_pFilters->GetEqualizer();
  if(peq)
    m_nNumofItems = peq->band;
#endif

  // verify slider bar and thumb size
  // when rc.dx is smaller than 80 = (THUMB_WIDTH+1)*NUM_OF_BAR, resize thumb and bar
  if(rc.dx < (m_byThumbWidth + 1)*(NUM_OF_BAR))
  {
    m_byThumbWidth = rc.dx/(NUM_OF_BAR) - 1;
    if(m_byThumbWidth <= 3)
    {
      // if thumb width is smaller than 3 pixels, set it to 3
      m_byThumbWidth = 3;
      m_byThumbHeight = 2;
      m_byBarWidth = 1;
    }
    else 
      m_byThumbHeight = m_byThumbWidth - 2;

    if(m_byThumbHeight < 2)
      m_byThumbHeight = 2;
    m_byBarWidth = m_byThumbWidth/2 + 1;
  }


  if(!m_nNumofItems)
    m_nNumofItems = NUM_OF_BAR;

  if(m_pBar)
    delete[] m_pBar;
  m_pBar = new CMPSlider[m_nNumofItems];
  if(!m_pBar)
    return FALSE;

  m_nMin = min;
  m_nMax = max;

  // save to drawingRc if it is not been set yet
  if(!m_rcDwPanel.IsInitialize())
    m_rcDwPanel.Set(rc.x, rc.y, rc.dx, rc.dy);
  // save drawing panel rect
  SetFrameRect(rc.x, rc.y, rc.dx, rc.dy);

  w = m_byBarWidth;
  y = rc.y + 2;
  dy = rc.dy - 4;
  // dx is the thumbWidth plus gap
  dx = (rc.dx - m_byThumbWidth*m_nNumofItems)/(m_nNumofItems - 1);
  // move start pt to 1/2 of gap width
  x = rc.x + dx/2;

  CMPSetupWin* pOwnerPtr;
  pOwnerPtr = (CMPSetupWin*)GetOwnerPtr();

  GetBkColor()->SetColor(pOwnerPtr->GetBkColor()->GetRGB());
  GetFgColor()->SetColor(pOwnerPtr->GetFgColor()->GetRGB());
  GetSdColor()->SetColor(pOwnerPtr->GetSdColor()->GetRGB());
  GetFlColor()->SetColor(pOwnerPtr->GetFlColor()->GetRGB());


  for(i=0; i < m_nNumofItems; i++)
  {
    pSliderBar = (CMPSlider*)(&m_pBar[i]); 

    pSliderBar->SetOwnerPtr(pOwnerPtr);
    pSliderBar->SetUpperBound(m_nMax);
    pSliderBar->SetLowerBound(m_nMin);
    pSliderBar->SetFrameRect(x, y + 2, w, dy - 4);
    pSliderBar->SetThumbSize(m_byThumbWidth, m_byThumbHeight);

#ifdef FEATURE_MP_EQ
    if(peq)
      pos = peq->filter[i].nGain;
    
    // reverse it
    pos = m_nMax - pos + m_nMin;
#endif
    (void)pSliderBar->SetThumbPos(pos);

    pSliderBar->GetBkColor()->SetColor(GetBkColor()->GetRGB());
    pSliderBar->GetFgColor()->SetColor(GetFgColor()->GetRGB());
    pSliderBar->GetSdColor()->SetColor(GetSdColor()->GetRGB());
    pSliderBar->GetFlColor()->SetColor(GetFlColor()->GetRGB());

    pSliderBar->GetThumb()->GetBkColor()->SetColor(GetBkColor()->GetRGB());
    pSliderBar->GetThumb()->GetFgColor()->SetColor(GetFgColor()->GetRGB());
    pSliderBar->GetThumb()->GetSdColor()->SetColor(GetSdColor()->GetRGB());
    pSliderBar->GetThumb()->GetFlColor()->SetColor(GetFlColor()->GetRGB());

    x = x + dx + m_byThumbWidth;
  }

  SetItem(0);
  return TRUE;
}
boolean CMPEqualizerSliderPanel::Draw(IGraphics *pg)
{
  int32 n;
  CMPSlider* pSliderBar;
  CMPRect rc;

  //IDISPLAY_EraseRgn
//  GetFrameRect(&rc);

  rc = m_rcDwPanel;
  (void)IGRAPHICS_SetFillColor(pg, GetFlColor()->GetR(), GetFlColor()->GetG(), GetFlColor()->GetB(), 1);

//  (void)IGRAPHICS_SetViewport(pg, rc.GetRect(), AEE_GRAPHICS_CLEAR);

  (void)IGRAPHICS_SetFillMode(pg, TRUE);
  (void)IGRAPHICS_SetColor(pg, GetFlColor()->GetR(), GetFlColor()->GetG(), GetFlColor()->GetB(), 1);

  (void)IGRAPHICS_DrawRect(pg, (AEERect*)rc.GetRect());

  if(m_pBar)
  {
    for(n=0; n < m_nNumofItems; n++)
    {
      pSliderBar = (CMPSlider*)(&m_pBar[n]); 
      (void)pSliderBar->Draw(pg);
    }
  }

  (void)IGRAPHICS_SetFillMode(pg, TRUE);
  return TRUE;
}

void CMPEqualizerSliderPanel::SetDrawingPanelRect(int x, int y, int dx, int dy)
{
  m_rcDwPanel.Set(x, y, dx, dy);
}

int CMPEqualizerSliderPanel::GetMinValue()
{
  return m_nMin;
}
int CMPEqualizerSliderPanel::GetMaxValue()
{
  return m_nMax;
}


void CMPEqualizerSliderPanel::SetFocus(boolean mode)
{
  if(GetFocus())
  {
    if(mode == FALSE)
    {
      SetItem(-1);
      CMPSymbol::SetFocus(mode);
    }
  }
  else
  {
    if(mode == TRUE)
    {
      SetItem(0);
      CMPSymbol::SetFocus(mode);
    }
    else
    {
      SetItem(-1);
      CMPSymbol::SetFocus(mode);
    }
  }
}

void CMPEqualizerSliderPanel::SetItem(int idx)
{
  int i;
  
  if(!m_pBar)
    return;

  if(idx >=0 && idx < m_nNumofItems)
  {
    for(i=0; i<m_nNumofItems; i++)
    {
      if(i == idx)
        m_pBar[i].SetFocus(TRUE);
      else
        m_pBar[i].SetFocus(FALSE);
    }
  }
  else
  {
    for(i=0; i<m_nNumofItems; i++)
    {
      m_pBar[i].SetFocus(FALSE);
    }
  }
}
int CMPEqualizerSliderPanel::GetItem()
{
  int i;
  for(i=0; i<m_nNumofItems; i++)
  {
    if(m_pBar && m_pBar[i].GetFocus())
      return i;
  }
  return -1;
}

void CMPEqualizerSliderPanel::SetRange(int min, int max)
{
  m_nMin = min;
  m_nMax = max;
}
boolean CMPEqualizerSliderPanel::SetNext()
{
  int sel;
  sel = GetItem();
  if(sel < m_nNumofItems - 1)
  {
    SetItem(++sel);
    return TRUE;
  }
  return FALSE;
}
boolean CMPEqualizerSliderPanel::SetPrevious()
{
  int sel;
  sel = GetItem();
  if(sel > 0)
  {
    SetItem(--sel);
    return TRUE;
  }
  return FALSE;
}
void CMPEqualizerSliderPanel::SetSelectItem(int idx)
{
  int i;
//  CMPEqualizer* peq;
  MPSetting* pSetting;
  MPDeviceInfo * pDeviceInfo = NULL;

  if(!m_pBar)
    return;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  pSetting = pDeviceInfo->m_pSetting;
  if(idx >=0 && idx < m_nNumofItems)
  {
    for(i=0; i<m_nNumofItems; i++)
    {
      if(i == idx)
      {
        m_pBar[i].SetSelection(TRUE);
        // save idx
        pSetting->m_nSliderID = idx;
        break;
      }
      else
        m_pBar[i].SetSelection(FALSE);
    }
  }
  else
  {
    for(i=0; i<m_nNumofItems; i++)
    {
      m_pBar[i].SetSelection(FALSE);
    }
  }  
}

int CMPEqualizerSliderPanel::GetSelectItem()
{
  int i;
  for(i=0; i<m_nNumofItems; i++)
  {
    if(m_pBar && m_pBar[i].GetSelection())
      return i;
  }
  return -1;
}

int CMPEqualizerSliderPanel::GetItemCount()
{
  return m_nNumofItems;
}


boolean CMPEqualizerSliderPanel::Inc()
{
  int sel;
  int pos;
  CMPSlider* pSliderBar;
  sel = GetItem();
  if(sel >=0)
  {
    pSliderBar = (CMPSlider*)&m_pBar[sel];
    pos = pSliderBar->GetThumbPos() + 1;
    (void)pSliderBar->SetThumbPos(pos);

#ifdef FEATURE_MP_EQ
    SetEqualizer(pos);
#endif // FEATURE_MP_EQ
    return TRUE;
  }
  return FALSE;
}
boolean CMPEqualizerSliderPanel::Dec()
{
  int sel;
  int pos;
  CMPSlider* pSliderBar;
  sel = GetItem();
  if(sel >=0)
  {
    pSliderBar = (CMPSlider*)&m_pBar[sel];
    pos = pSliderBar->GetThumbPos() - 1;
    (void)pSliderBar->SetThumbPos(pos);

#ifdef FEATURE_MP_EQ
    SetEqualizer(pos);
#endif // FEATURE_MP_EQ
    return TRUE;
  }
  return FALSE;
}
CMPSlider* CMPEqualizerSliderPanel::GetSlider(int idx)
{
  return &m_pBar[idx];
}

#ifdef FEATURE_MP_EQ
void CMPEqualizerSliderPanel::SetEqualizer(int16 gain)
{
  int ret = FALSE;
  MPSetting* pSetting;
  MPDeviceInfo * pDeviceInfo = NULL;
  int16 sel = GetItem();

  if(gain <= m_nMin || gain >= m_nMax)
    return;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  equalizer* peq = pDeviceInfo->m_pFilters->GetEqualizer();
  if (!peq) 
  {
      return;
  }

  gain = m_nMax - gain + m_nMin;
  // save new gain value
  peq->filter[sel].nGain = gain;
  
  // sel(band) index???
  pSetting = pDeviceInfo->m_pSetting;
  ret = MPAudioSetEqualizerGain(pSetting, sel, gain);

  DBGPRINTF_LOW("MP: Set EQ Gain, band:%d, gain:%d, ret: %d", sel, gain, ret);
}
#endif

boolean CMPEqualizerSliderPanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 /*dwParam*/)
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

      ret = TRUE;
      break;
     case AVK_UP:
       (void)Dec();
       ret = TRUE;
       break;
     case AVK_DOWN:
       (void)Inc();
       ret = TRUE;
       break;
     case AVK_LEFT:
      ret = SetPrevious();
      break;
     case AVK_RIGHT:
      ret = SetNext();
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

#endif //FEATURE_MP_EQ_UI
