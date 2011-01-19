/*=============================================================================
  FILE: MPAnalyzerPanel.cpp

  SERVICES: MediaPlayer Spectrum Analyzer Graph 
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPAnalyzerPanel.cpp#6 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPAnalyzerPanel.h"
#ifdef FEATURE_MP_EQ_UI

CMPAnalyzerPanel::CMPAnalyzerPanel(void* pOwner):
    CMPSymbol(pOwner), m_pBars(NULL), m_MaxVal(32), m_nCount(0), m_bEnable(TRUE)
{
	m_rcFrameSave.dx = m_rcFrameSave.dy = 0;
  m_rcResize.dx =  m_rcResize.dy = 0;
}
CMPAnalyzerPanel::~CMPAnalyzerPanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  Clean();
  DBGPRINTF_LOW("MP: end deleting CMPAnalyzerPanel.");
  /*lint +e1551*/
}

void CMPAnalyzerPanel::Clean()
{
  if(m_pBars)
  {
    DBGPRINTF_HIGH("MP: deleting SPA drawing bars.");
      delete[] m_pBars;
  }
  m_pBars = NULL;
  m_nCount = 0;
}

boolean CMPAnalyzerPanel::Init(AEERect rc)
{
  // save rc
	m_rcFrameSave = rc;
	// add two pixels on each side
  rc.x += 2;
  rc.y += 2;
  rc.dx -= 4;
  rc.dy -= 4;
  SetFrameRect(rc.x, rc.y, rc.dx, rc.dy);

  GetBkColor()->SetColor(0, 0, 0);
  GetFgColor()->SetColor(255, 255, 0);
  GetFlColor()->SetColor(255, 255, 0);
  GetSdColor()->SetColor(125, 120, 120);
  return TRUE;
}
void CMPAnalyzerPanel::Resize(AEERect& rc)
{
  DBGPRINTF_HIGH("MP: Resize requested, x = %d, y = %d, dx = %d, dy = %d.", rc.x, rc.y, rc.dx, rc.dy);
  m_rcResize = rc;
}

void CMPAnalyzerPanel::SetData(int16* pData, int length)
{
  int i;

  if(pData == NULL || length == 0)
  {
    DBGPRINTF_ERROR("MP: Null or 0 size data, pData = 0x%X, length = %d",pData, length);
    return;
  }

  if(length != m_nCount || NULL == m_pBars)
  {  
    // data length changed
    if(!LayoutScreen(m_rcFrameSave, length))
    {
      DBGPRINTF_ERROR("MP: LayoutScreen failed.");
      return;
    }
  }
  else if(m_rcResize.dx && m_rcResize.dy)
  {
    // resize requested
    if(!LayoutScreen(m_rcResize, length))
    {
      DBGPRINTF_ERROR("MP: LayoutScreen failed.");
      return;
    }
    m_rcResize.dx = 0;
    m_rcResize.dy = 0;
  }

  // save new data
  for(i = 0; i < length; i++)
  {
    m_pBars[i].SetData(GetY((int)pData[i]));
  }
}

boolean CMPAnalyzerPanel::LayoutScreen(AEERect& rcPanel, int nBarCount)
{
  int i; 
  CMPRect rc;
  AEERect rcb;
  int orgwidth;
  int newwidth;
  int delta;
  int x, nBarWidth;

  if(rcPanel.dx != 0 && rcPanel.dy != 0 && nBarCount != 0)
  {
    // set frame panel size
    (void)Init(rcPanel);

    // if number of bars changed, delete the old one
    if(nBarCount != m_nCount)
    {
      if(m_pBars)
      {
        DBGPRINTF_HIGH("MP: counts changed, delete old.");
        delete[] m_pBars;
      }
      m_pBars = NULL;
      m_nCount = 0;
    }

    if(m_pBars == NULL)
    {
      m_pBars  = new CMPBar[nBarCount]; 
      if(m_pBars == NULL)
      {
        DBGPRINTF_ERROR("MP: Error on creating SpectrumAnalyzer bars.");
        return FALSE;
      }
      DBGPRINTF_HIGH("MP: memory allocated for SPA bars 0x%X, Count = %d", m_pBars, nBarCount);
      m_pBars->SetOwnerPtr(GetOwnerPtr());
    }
    m_nCount = nBarCount;

    (void)GetFrameRect(&rc);

    // calculate bar width, add one pixel space between bars
    if (m_nCount > 1)
    {
      nBarWidth = rc.GetRect()->dx/(m_nCount) - 1; //(rc.GetRect()->dx - (m_nCount-1))/(m_nCount);
    }
    else
    {
      /* should not happen, but assign sensible values */
      nBarWidth = 1;
      m_nCount = 1;
    }

    // readjust frame width and centralize bars
    orgwidth = rc.GetRect()->dx;
    newwidth = nBarWidth*m_nCount + (m_nCount-1);
    delta = (orgwidth - newwidth)/2;
    rc.GetRect()->x += delta;
    rc.GetRect()->dx -= 2*delta;

    // set new frame rect size
    SetFrameRect(rc.GetRect()->x, rc.GetRect()->y, rc.GetRect()->dx, rc.GetRect()->dy);
  
    x = rc.GetRect()->x;  
    for(i = 0; i < m_nCount; i++)
    {
      rcb.x = x;
      rcb.y = rc.GetRect()->y;
      rcb.dx = nBarWidth;
      rcb.dy = rc.GetRect()->dy;

      m_pBars[i].SetFrameRect(rcb.x, rcb.y, rcb.dx, rcb.dy);
      m_pBars[i].GetBkColor()->SetColor(GetBkColor()->GetRGB());
      m_pBars[i].GetFgColor()->SetColor(GetFgColor()->GetRGB());
      m_pBars[i].GetSdColor()->SetColor(GetSdColor()->GetRGB());
      m_pBars[i].GetFlColor()->SetColor(GetFlColor()->GetRGB());

      // 1 pixel gap
      x += nBarWidth + 1;
    }
    
    DBGPRINTF_HIGH("MP: LayoutScreen successfully.");
    return TRUE;
  }

  DBGPRINTF_ERROR("MP: LayoutScreen received bad parm width = %d, height = %d, count = %d.",
                  rcPanel.dx, rcPanel.dy, nBarCount);

  return FALSE;
}

void CMPAnalyzerPanel::Enable(boolean bMode)
{
  m_bEnable = bMode;
}

void CMPAnalyzerPanel::SetMaxValue(int nValue)
{
  if(nValue > 0)
  {
    m_MaxVal = nValue;
  }
}

boolean CMPAnalyzerPanel::Draw(IGraphics *pg)
{
  AEERect* prc;
  CMPRect rc;
  int i;
  boolean fillmode;
  uint8 r, g, b, alpha;
  uint8 fr, fg, fb, falpha;

  if(!m_bEnable)
    return TRUE;

  if(m_rcResize.dx && m_rcResize.dy && m_pBars)
  {
    // use resize rc. 
    rc = m_rcResize;
    //we don't want to draw old data if resizing
    for(i=0; i < m_nCount; i++)
    {
      m_pBars[i].CleanPeakHold();
    }
  }
  else
  {
    (void)GetFrameRect(&rc);
  }

  prc = rc.GetRect();

  // save default colors
  IGRAPHICS_GetColor(pg, &r, &g, &b, &alpha);
  IGRAPHICS_GetFillColor(pg, &fr, &fg, &fb, &falpha);
  fillmode = IGRAPHICS_SetFillMode(pg, TRUE);

  // draw bk with entire width
  prc->x = m_rcFrameSave.x;
  prc->dx = m_rcFrameSave.dx;
  // fill bk color
  (void)MP_DrawFrame(pg, rc.GetRect(), AEE_FT_LOWERED, GetBkColor()->GetRGB());

  if(m_pBars)
  {
    // do not draw if relayout is needed
    if(m_rcResize.dx ==0 && m_rcResize.dy == 0)
    {
      for(i=0; i < m_nCount; i++)
      {
        (void)m_pBars[i].Draw(pg);
      }
    }
  }

  // restore default color and fill mode
  (void)IGRAPHICS_SetColor(pg, r, g, b, alpha);
  (void)IGRAPHICS_SetFillColor(pg, fr, fg, fb, falpha);
  (void)IGRAPHICS_SetFillMode(pg, fillmode);

  return TRUE;
}
// this function will convert y value from logical to physical
int CMPAnalyzerPanel::GetY(int y)
{
  int y1  = 0;
  CMPRect rc;
  (void)GetFrameRect(&rc);

  if(y > m_MaxVal)
  {
    DBGPRINTF_ERROR("PB: SPA received data larger than Max = %d, data = %d.", m_MaxVal, y);
    y = m_MaxVal;
  }

  if(rc.GetRect()->dy)
  {
    y1 = rc.GetRect()->dy * y/m_MaxVal;
  }
  return y1;
}
#endif //FEATURE_MP_EQ_UI
