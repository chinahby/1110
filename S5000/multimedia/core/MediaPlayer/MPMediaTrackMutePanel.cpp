/*=============================================================================
  FILE: MPMediaTrackMutePanel.cpp

  SERVICES: CMPMediaTrackMutePanel class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPMediaTrackMutePanel.cpp#9 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPMediaTrackMutePanel.h"
#include "MPSettings.h"
#include "MediaPlayer.h"


CMPMediaTrackMutePanel::CMPMediaTrackMutePanel(void* pOwner): CMPSymbol(pOwner),
   m_nItemCount(0), m_pItem(NULL), m_nSelectionCount(0),m_aSelections(NULL),m_pListWin(NULL)
{

}

CMPMediaTrackMutePanel::~CMPMediaTrackMutePanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  Cleanup();
  /*lint +e1551*/
}

void CMPMediaTrackMutePanel::Cleanup()
{
  RemoveTrackItems();
  if(m_pListWin)
    delete m_pListWin;
  m_pListWin = NULL;
}

boolean CMPMediaTrackMutePanel::Init(AEERect rc)
{
  uint32 charHight;
  IDisplay* pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return FALSE;

  GetBkColor()->SetColor(212, 255, 255);
  GetFgColor()->SetColor(0, 212, 212);
  GetSdColor()->SetColor(0, 150, 150);

  SetFrameRect(rc.x, rc.y, rc.dx, rc.dy);

  pDisplay = pDeviceInfo->m_pDisplay;
  charHight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;
  m_pListWin = new CMPListWin<CMPLabel>(pDeviceInfo->m_pShell, pDeviceInfo->m_pDisplay, rc); 
  if(!m_pListWin)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pListWin.");
    return FALSE;
  }

  if(!m_pListWin->Init(charHight, 1,3))
  {
    delete m_pListWin;
    m_pListWin = NULL;
    DBGPRINTF_ERROR("MP: Failed to create listwin.");
    return FALSE;
  }

  m_pListWin->SetLeftMargin(2);
  m_pListWin->SetScrollBarOnLeft(FALSE);

  m_pListWin->GetBkColor()->SetColor(153, 204, 204);
  m_pListWin->GetFgColor()->SetColor(0, 0, 0);
  m_pListWin->GetSdColor()->SetColor(150, 150, 150);


  SetFocus(TRUE);
  return TRUE;
}

boolean CMPMediaTrackMutePanel::CreateTrackItems(int nCount)
{
  if(m_nItemCount)
  {
    RemoveTrackItems();
  }
  m_pItem = new CTrackItem[nCount];
  m_aSelections = new int [nCount];
  if(m_pItem && m_aSelections)
  {
    m_nItemCount = nCount;
    m_nSelectionCount = 0; 
  }
  else
  {
    if(m_pItem)
    { 
      delete [] m_pItem; 
      m_pItem = NULL; 
    }
    if(m_aSelections)
    { 
      delete [] m_aSelections; 
      m_aSelections = NULL; 
    }
    DBGPRINTF_ERROR("MP: failed to allocate memory.");
    return FALSE;
  }
  return TRUE;
}
void CMPMediaTrackMutePanel::RemoveTrackItems()
{
  delete[] m_pItem;
  delete[] m_aSelections;
  m_pItem = NULL;
  m_aSelections = NULL; 
  m_nItemCount = 0;
  m_nSelectionCount = 0; 
}

boolean CMPMediaTrackMutePanel::AddTrackItem(int id, int nTrackID, boolean bMute, char* pDisp, boolean bSelect)
{
  boolean bRet = FALSE;

  if(id < m_nItemCount)
  {
    int charHight;
    CMPRect rc;
    AEERect* pRC;
    IDisplay* pDisplay;
    MPDeviceInfo * pDeviceInfo = NULL;

    pDeviceInfo = MP_GetInfo(this);
    if(!pDeviceInfo)
        return FALSE;

    (void)GetFrameRect(&rc);
    pRC = rc.GetRect();

    pDisplay = pDeviceInfo->m_pDisplay;
    charHight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;

    bRet = m_pItem[id].Add(nTrackID,pDisp);
    m_pItem[id].m_pLabels->SetOwnerPtr(GetOwnerPtr());
    m_pItem[id].m_pLabels->SetID(id);
    m_pItem[id].m_pLabels->SetFrameRect(pRC->x + 1, pRC->y + id*charHight, pRC->dx/3, charHight);
    m_pItem[id].m_pLabels->GetBkColor()->SetColor(153, 204, 204); //212, 255, 255
    m_pItem[id].m_pLabels->GetFgColor()->SetColor(0, 212, 212);
    m_pItem[id].m_pLabels->GetSdColor()->SetColor(0, 150, 150);
    m_pItem[id].m_pLabels->GetTextColor()->SetColor(0, 0, 0);
    m_pItem[id].m_pLabels->SetData(nTrackID);
    m_pItem[id].m_pLabels->SetSelection(bMute);

    m_pListWin->Add(m_pItem[id].m_pLabels);

    if(bSelect)
    {    
      m_pItem[id].m_pLabels->Enable(TRUE);
    }
    else
    {
      m_pItem[id].m_pLabels->Enable(FALSE);
    }
    m_pItem[id].m_pLabels->SetFocus(FALSE);
  }


  return bRet;
}
void CMPMediaTrackMutePanel::SetItemFocus()
{
  CMPLabel* pItem;
  int nCnt;
  int id;
  boolean bSet = FALSE;

  nCnt = m_pListWin->GetItemCount();
  for(id = 0; id < nCnt; id++)
  {
    pItem = m_pListWin->GetItem(id);
    if(NULL == pItem)
      continue;

    if(FALSE == bSet)
    {
      // set focus to first enabled item
      if(pItem->Enable())
      {
        pItem->SetFocus(TRUE);
        bSet = TRUE;
      }
      else
      {
        pItem->SetFocus(FALSE);
      }
    }
    else
    {
      pItem->SetFocus(FALSE);
    }
  }
  SetFocus(TRUE);
}

boolean CMPMediaTrackMutePanel::Draw(IGraphics *pg)
{
  return m_pListWin->Draw(pg);
}

// key event handler
boolean CMPMediaTrackMutePanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 /*dwParam*/)
{
  boolean bRet = FALSE;

  if(!GetFocus())
    return FALSE;
   
 // bRet = m_pListWin->OnEvent(eCode, wParam, dwParam);
  switch(eCode)
  {
    case EVT_KEY_PRESS:
      switch(wParam)
      {
        case AVK_SELECT:
          OnSelect();
          bRet = TRUE; 
          break;
        case AVK_UP:
          bRet = m_pListWin->SetPrevious();
          if(bRet == FALSE)
            bRet = m_pListWin->SetLast();
          break;
        case AVK_DOWN:
          bRet = m_pListWin->SetNext();
          if(bRet == FALSE)
            bRet = m_pListWin->SetFirst();
          break;
        case AVK_LEFT:
        case AVK_RIGHT:
           bRet = TRUE;
         break;
        case AVK_CLR:
         SaveMute();
         return FALSE;  
       default:
         break;
      }
    break;

    case EVT_KEY:
      if(AVK_CLR == wParam)
        return FALSE;  
      else
        return TRUE;// return TRUE for all others
  default:
    break;
  }
  return bRet;
}

void CMPMediaTrackMutePanel::OnSelect()
{
#ifdef FEATURE_QTV_GENERIC_BCAST
  CMPLabel* pItem;
  int id;
  int i=0; 

  // get current focused item
  id = m_pListWin->GetItem();
  pItem = (CMPLabel*)m_pListWin->GetItem(id);
  if(pItem && pItem->Enable())
  {
    pItem->SetSelection(!pItem->GetSelection());
    for(;(i<m_nSelectionCount) && (m_aSelections[i] != id);i++)
    {
      ; 
    }
  
    if((i==m_nSelectionCount) && (m_nSelectionCount < m_nItemCount))
    {
      m_aSelections[m_nSelectionCount++] = id; 
    }
  }
  else
  {
    IShell* pIShell;
    MPDeviceInfo * pDeviceInfo = NULL;

    pDeviceInfo = MP_GetInfo(this);
    if(!pDeviceInfo)
        return;

    pIShell = pDeviceInfo->m_pShell;
    (void)ISHELL_Beep(pIShell, BEEP_ERROR, FALSE);
    (void)ISHELL_Beep(pIShell, BEEP_ERROR, FALSE);
  }
#endif//#ifdef FEATURE_QTV_GENERIC_BCAST
}

void CMPMediaTrackMutePanel::SaveMute()
{
#ifdef FEATURE_QTV_GENERIC_BCAST
  CMPSymbol* pItem;
  MPSetting* pSetting;
  int nRet;
  int id;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  pSetting = pDeviceInfo->m_pSetting;

  // sync all selections
  for(id = 0; id < m_nSelectionCount; id++)
  {
    pItem = m_pListWin->GetItem(m_aSelections[id]);
    if (!pItem) 
    {
        return;
    }
    nRet = MPMuteTrack(pSetting, pItem->GetData(), pItem->GetSelection());
    if(SUCCESS != nRet && MM_PENDING != nRet)
    {
      // failed, set back to old
      pItem->SetSelection(!pItem->GetSelection());
      DBGPRINTF_ERROR("MP: failed to mute track, id = %d", pItem->GetData());
    }   
  }
  m_nSelectionCount = 0;
#endif //#ifdef FEATURE_QTV_GENERIC_BCAST
}

