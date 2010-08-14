/*=============================================================================
  FILE: MPEqualizerFilterPanel.cpp

  SERVICES: CMPEqualizerFilterPanel class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPEqualizerFilterPanel.cpp#9 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPEqualizerFilterPanel.h"

#ifdef FEATURE_MP_EQ_UI
#include "MPEqualizerData.h"
#include "MPSettings.h"

#ifdef FEATURE_MP_EQ
#include "AEESoundCtl.h"
#endif //FEATURE_MP_EQ

#define NUM_OF_FILTERS 3

CMPEqualizerFilterPanel::CMPEqualizerFilterPanel(void* pOwner):
  CMPPanel(pOwner), m_pLabels(NULL), m_nNumofItems(0), m_pListWin(NULL)
{
}
CMPEqualizerFilterPanel::~CMPEqualizerFilterPanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  if(m_pLabels)
    delete[] m_pLabels;
  if(m_pListWin)
    delete m_pListWin;
  m_pListWin = NULL;
  DBGPRINTF_LOW("MP: end deleting CMPEqualizerFilterPanel.");
  /*lint +e1551*/
}

boolean CMPEqualizerFilterPanel::Init(int x, int y, int dx, int dy)
{
  AEERect rc;
  rc.x = x;
  rc.y = y;
  rc.dx = dx;
  rc.dy = dy;

  return Init(rc);
}

boolean CMPEqualizerFilterPanel::Init(AEERect rc)
{
  int i;
  uint32 charHight;
  AEERect rcPanel = rc;
  IDisplay* pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return FALSE;

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return FALSE;

  GetBkColor()->SetColor(153, 204, 204);
  GetFgColor()->SetColor(0, 212, 212);
  GetSdColor()->SetColor(0, 150, 150);

  rcPanel.x += 1;
  rcPanel.dx -= 2;
  charHight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;
  m_pListWin = new CMPListWin<CMPLabel>(pDeviceInfo->m_pShell, pDisplay, rcPanel); 
  if(!m_pListWin)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pListWin.");
    return FALSE;
  }

  if(!m_pListWin->Init(charHight, 1,17))
  {
    delete m_pListWin;
    m_pListWin = NULL;
    DBGPRINTF_ERROR("MP: Unable to initialize m_pListWin.");
    return FALSE;
  }

  m_pListWin->SetLeftMargin(4);

  m_pListWin->GetBkColor()->SetColor(153, 204, 204);
  m_pListWin->GetFgColor()->SetColor(0, 0, 0);
  m_pListWin->GetSdColor()->SetColor(150, 150, 150);

  // create labels eq to num of filters
  if(m_pLabels)
    delete[] m_pLabels;
  m_pLabels = NULL;

#ifdef FEATURE_MP_EQ
  m_nNumofItems = pDeviceInfo->m_pFilters->GetCount();
#else
  m_nNumofItems = 0;
#endif

  if(m_nNumofItems > 0)
  {
    m_pLabels = new CMPLabel[m_nNumofItems];
    if(!m_pLabels)
    {
      DBGPRINTF_ERROR("MP: Error on creating m_pLabels.");
    }

    if(!m_pLabels)
    {
      delete m_pListWin;
      m_pListWin = NULL;
      DBGPRINTF_ERROR("MP: No Filters.");
      return FALSE;
    }

    SetFrameRect(rc.x, rc.y, rc.dx, rc.dy);

    //  lbHeight = (rc.dy - 2)/3 + 2;
    rc.x += 1;
    rc.y += 1;
    for(i=0; i < m_nNumofItems; i++)
    {
      // save owner into label, so that we can pass the screen info
      m_pLabels[i].SetOwnerPtr(GetOwnerPtr());
      m_pLabels[i].SetID(i);
      m_pLabels[i].SetFrameRect(rc.x + 1, rc.y + i*charHight, rc.dx/3, charHight);
  #ifdef FEATURE_MP_EQ
      equalizer* peq = pDeviceInfo->m_pFilters->GetEqualizer((int16)i);
      if(peq)
        m_pLabels[i].SetName(peq->name);
  #endif

      m_pLabels[i].GetBkColor()->SetColor(153, 204, 204);
      m_pLabels[i].GetFgColor()->SetColor(0, 212, 212);
      m_pLabels[i].GetSdColor()->SetColor(0, 150, 150);
      m_pLabels[i].GetTextColor()->SetColor(0, 0, 0);

      m_pListWin->Add(&m_pLabels[i]);
    }

  #ifdef FEATURE_MP_EQ
    m_pLabels[pDeviceInfo->m_pFilters->GetIdx()].SetFocus(TRUE);
    m_pLabels[pDeviceInfo->m_pFilters->GetIdx()].SetSelection(TRUE);
  #endif

    MPSetting* pSetting = pDeviceInfo->m_pSetting;
    // set selection and default filter
    SetSelectItem(pSetting->m_nFilterID);

    SetFocus(TRUE);
  }

  return TRUE;
}

// set focus for the panel
// when panel lose focus, nothing will be shown
void CMPEqualizerFilterPanel::SetFocus(boolean mode)
{
  m_pListWin->SetFocus(mode);
  CMPSymbol::SetFocus(mode);
}
void CMPEqualizerFilterPanel::SetItem(int idx)
{
  m_pListWin->SetItem(idx);
  //m_pListWin->SetItemSelection(idx);
}
int CMPEqualizerFilterPanel::GetItem()
{
  return m_pListWin->GetItem();
}
boolean CMPEqualizerFilterPanel::SetNext()
{
  return m_pListWin->SetNext();
}
boolean CMPEqualizerFilterPanel::SetPrevious()
{
  return m_pListWin->SetPrevious();
}

void CMPEqualizerFilterPanel::SetSelectItem(int idx)
{
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  if(m_pListWin->SetItemSelection(idx))
  {
    // save idx
    MPSetting* pSetting = pDeviceInfo->m_pSetting;
    if(pSetting)
      pSetting->m_nFilterID = idx;
    // set this filter now
    fSet(idx);
  }
}
int CMPEqualizerFilterPanel::GetSelectItem()
{
  return m_pListWin->GetSelectedItem();
}
int CMPEqualizerFilterPanel::GetItemCount()
{
  return m_pListWin->GetItemCount();
}

void CMPEqualizerFilterPanel::SetScrollBarOnLeft(boolean bLeft)
{
  if(m_pListWin)
    m_pListWin->SetScrollBarOnLeft(bLeft);
}

// denote for filter set
void CMPEqualizerFilterPanel::fSet(int i)
{
  if(m_pLabels[i].Enable())
  {
#ifdef FEATURE_MP_EQ
    SetFilter(i);
#endif //FEATURE_MP_EQ
  }
}
#ifdef FEATURE_MP_EQ
void CMPEqualizerFilterPanel::SetFilter(int i)
{
  equalizer* peq;
  int16      idx;
  MPSetting* pSetting;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(pDeviceInfo && pDeviceInfo->m_pFilters)
  {
    // save current eq(usr may made change), this is not been implemented yet. 
    idx = pDeviceInfo->m_pFilters->GetIdx();
    pDeviceInfo->m_pFilters->Save(pDeviceInfo->m_pFilters->GetEqualizer(idx));

    // load and set new eq
    peq = pDeviceInfo->m_pFilters->GetEqualizer(i);
    if(peq)
    {
      DBGPRINTF_LOW("MP: set equalizer filter idx = %d.", i);
      pSetting = pDeviceInfo->m_pSetting;
      (void)MPAudioSetEqualizer(pSetting, peq->filter, peq->band, peq->name);
    
      pDeviceInfo->m_pFilters->SetIdx(i);
    }
  }
}
#endif //FEATURE_MP_EQ

boolean CMPEqualizerFilterPanel::Draw(IGraphics *pg)
{
  return m_pListWin->Draw(pg);
}

// set focus to first the item
void CMPEqualizerFilterPanel::SetFirst()
{
  if(m_pListWin)
    (void)m_pListWin->SetFirst();
}
// set focus to the last item
void CMPEqualizerFilterPanel::SetLast()
{
  if(m_pListWin)
    (void)m_pListWin->SetLast();
}

boolean CMPEqualizerFilterPanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  if(!GetFocus())
    return FALSE;

  return m_pListWin->OnEvent(eCode, wParam, dwParam);
}
#endif // FEATURE_MP_EQ_UI
