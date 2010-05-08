/*=============================================================================
  FILE: MPMediaTrackSelectPanel.cpp

  SERVICES: CMPMediaTrackSelectPanel class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPMediaTrackSelectPanel.cpp#11 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPMediaTrackSelectPanel.h"
#include "MPSettings.h"
#include "MediaPlayer.h"


CMPMediaTrackSelectPanel::CMPMediaTrackSelectPanel(void* pOwner): CMPSymbol(pOwner),
   m_nItemCount(0), m_pItem(NULL), m_pListWin(NULL)
{

}

CMPMediaTrackSelectPanel::~CMPMediaTrackSelectPanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  Cleanup();
  /*lint +e1551*/
}

void CMPMediaTrackSelectPanel::Cleanup()
{
  RemoveTrackItems();
  if(m_pListWin)
    delete m_pListWin;
  m_pListWin = NULL;
}

boolean CMPMediaTrackSelectPanel::Init(AEERect rc)
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

boolean CMPMediaTrackSelectPanel::CreateTrackItems(int nCount)
{
  if(m_nItemCount)
  {
    RemoveTrackItems();
  }
  m_pItem = new CTrackItem[nCount];
  if(m_pItem)
  {
    m_nItemCount = nCount;
  }
  else
  {
    DBGPRINTF_ERROR("MP: failed to allocate memory.");
    return FALSE;
  }
  return TRUE;
}
void CMPMediaTrackSelectPanel::RemoveTrackItems()
{
  delete[] m_pItem;
  m_pItem = NULL;
  m_nItemCount = 0;
}

boolean CMPMediaTrackSelectPanel::AddTrackItem(int id, int nTrackID, boolean bSelect, char* pDisp)
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

    bRet = m_pItem[id].Add(nTrackID, pDisp);
    m_pItem[id].m_pLabels->SetOwnerPtr(GetOwnerPtr());
    m_pItem[id].m_pLabels->SetID(id);
    m_pItem[id].m_pLabels->SetFrameRect(pRC->x + 1, pRC->y + id*charHight, pRC->dx/3, charHight);
    m_pItem[id].m_pLabels->GetBkColor()->SetColor(153, 204, 204); //212, 255, 255
    m_pItem[id].m_pLabels->GetFgColor()->SetColor(0, 212, 212);
    m_pItem[id].m_pLabels->GetSdColor()->SetColor(0, 150, 150);
    m_pItem[id].m_pLabels->GetTextColor()->SetColor(0, 0, 0);
    m_pItem[id].m_pLabels->SetData(nTrackID);
    m_pItem[id].m_pLabels->SetSelection(bSelect);

    m_pListWin->Add(m_pItem[id].m_pLabels);

    m_pItem[0].m_pLabels->SetFocus(TRUE);
  }

  return bRet;
}
boolean CMPMediaTrackSelectPanel::Draw(IGraphics *pg)
{
  return m_pListWin->Draw(pg);
}

// key event handler
boolean CMPMediaTrackSelectPanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 /*dwParam*/)
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
          // exit from track selection dlg
          // go through the list, and set to QTV
          SaveSelections();
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

void CMPMediaTrackSelectPanel::OnSelect()
{
#ifdef FEATURE_QTV_GENERIC_BCAST
  CMPSymbol* pItem;

  // get current focused item
  pItem = m_pListWin->GetItem(m_pListWin->GetItem());
  if(pItem)
  {
    pItem->SetSelection(!pItem->GetSelection());
  }
#endif //#ifdef FEATURE_QTV_GENERIC_BCAST
}

void CMPMediaTrackSelectPanel::SaveSelections()
{
#ifdef FEATURE_QTV_GENERIC_BCAST
  CMPSymbol* pItem;
  MPSetting* pSetting;
  int nRet;
  int nCnt;
  int id;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return; //FALSE

  pSetting = pDeviceInfo->m_pSetting;
  nCnt = m_pListWin->GetItemCount();

  (void)MPUnselectAllTracks(pSetting);
  // sync all selections
  for(id = 0; id < nCnt; id++)
  {
    pItem = m_pListWin->GetItem(id);
    if(pItem && pItem->GetSelection())
    {
      // set selection to OEM
      nRet = MPSelectTrack(pSetting, pItem->GetData());
      if(SUCCESS != nRet && MM_PENDING != nRet)
      {
        // failed, set back
        pItem->SetSelection(!pItem->GetSelection());
        DBGPRINTF_ERROR("MP: failed to set select on track, id = %d", pItem->GetData());
        return;
      }
    }
  }
#endif //#ifdef FEATURE_QTV_GENERIC_BCAST
}
