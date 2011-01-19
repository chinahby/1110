/*=============================================================================
  FILE: MPMediaTracksPanel.cpp

  SERVICES: CMPMediaTracksPanel class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPMediaTrackPanel.cpp#7 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPMediaTrackPanel.h"
#include "MPSettings.h"
#include "MediaPlayer.h"
#include "MPMediaTrackSelectPanel.h"
#include "MPMediaTrackMutePanel.h"

#define LB_LABEL_SELECT_TRACK "Select Track"
#define LB_LABEL_MUTE_TRACK   "Mute Track"

CMPMediaTracksPanel::CMPMediaTracksPanel(void* pOwner): CMPSymbol(pOwner),
   m_pListWin(NULL), m_pSelectTrack(NULL), m_pMuteTrack(NULL) 
{
  int i;	
  for( i=0; i < MP_LABEL_TRACK_MAX; i++)
  {
    m_pSymbol[i] = NULL;
    m_NameString[i] = NULL;
  }
}

CMPMediaTracksPanel::~CMPMediaTracksPanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  Cleanup();
  /*lint +e1551*/
}

void CMPMediaTracksPanel::Cleanup()
{
  int i;

  if(m_pSelectTrack)
    delete m_pSelectTrack;
  if(m_pMuteTrack)
    delete m_pMuteTrack;
  m_pSelectTrack = NULL;
  m_pMuteTrack = NULL;

  for( i=0; i < MP_LABEL_TRACK_MAX; i++)
  {
    if(m_pSymbol[i])
      delete m_pSymbol[i];
    m_pSymbol[i] = NULL;

    if(m_NameString[i])
      delete m_NameString[i];
    m_NameString[i] = NULL;
  }

  if(m_pListWin)
    delete m_pListWin;
  m_pListWin = NULL;
}

boolean CMPMediaTracksPanel::Init(AEERect rc)
{
  int i;
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
  m_pListWin = new CMPListWin<CMPSymbol>(pDeviceInfo->m_pShell, pDeviceInfo->m_pDisplay, rc); 
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


  m_NameString[MP_LABEL_TRACK_SELECT] = MP_ToAEEChar(LB_LABEL_SELECT_TRACK);
  m_NameString[MP_LABEL_TRACK_MUTE] = MP_ToAEEChar(LB_LABEL_MUTE_TRACK);

  for( i=0; i < MP_LABEL_TRACK_MAX; i++)
  {
    m_pSymbol[i] = new CMPButton;
    if(!m_pSymbol[i])
    {
      DBGPRINTF_ERROR("MP: error on creating m_pSymbol %d.", i);
      Cleanup();
      return FALSE;
    }

    ((CMPButton*)m_pSymbol[i])->GetTextColor()->SetColor(0, 0, 0);
    ((CMPButton*)m_pSymbol[i])->SetName(m_NameString[i]);

    rc.x += 1;
    rc.y += 1;
  
    // save owner into label, so that we can pass the screen info
    m_pSymbol[i]->SetOwnerPtr(GetOwnerPtr());
    m_pSymbol[i]->SetID(i);
    m_pSymbol[i]->SetFrameRect(rc.x + 1, rc.y + i*charHight, rc.dx/3, charHight);
  
    m_pSymbol[i]->GetBkColor()->SetColor(153, 204, 204); //212, 255, 255
    m_pSymbol[i]->GetFgColor()->SetColor(0, 212, 212);
    m_pSymbol[i]->GetSdColor()->SetColor(0, 150, 150);

    m_pListWin->Add(m_pSymbol[i]);
  }

  // set focus into first item
  m_pSymbol[0]->SetFocus(TRUE);
  SetFocus(TRUE);
  return TRUE;
}

boolean CMPMediaTracksPanel::Draw(IGraphics *pg)
{
  if(m_pSelectTrack)
  {
    return m_pSelectTrack->Draw(pg);
  }
  else if(m_pMuteTrack)
  {
    return m_pMuteTrack->Draw(pg);
  }

  return m_pListWin->Draw(pg);
}

// key event handler
boolean CMPMediaTracksPanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  boolean bRet;

  if(!GetFocus())
    return FALSE;
   
  if(m_pSelectTrack)
  {
    bRet = m_pSelectTrack->OnEvent(eCode, wParam, dwParam);
    if(bRet)
      return bRet;
  }
  else if(m_pMuteTrack)
  {
    bRet = m_pMuteTrack->OnEvent(eCode, wParam, dwParam);
    if(bRet)
      return bRet;
  }


  bRet = m_pListWin->OnEvent(eCode, wParam, dwParam);
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
      case AVK_DOWN:
      case AVK_LEFT:
      case AVK_RIGHT:
       break;
      case AVK_CLR:
      {
       bRet = FALSE;
       if(m_pSelectTrack)
       {       
         delete m_pSelectTrack;
         m_pSelectTrack = NULL;
         bRet = TRUE;
       }
       if(m_pMuteTrack)
       {
         delete m_pMuteTrack;
         m_pMuteTrack = NULL;
         bRet = TRUE;
       }
       return bRet;  
     }
     default:
       break;
    }
    break;
  default:
    break;
  }
  return bRet;
}

void CMPMediaTracksPanel::OnSelect()
{
  CMPSymbol* pItem;
  MPSetting* pSetting;
  CMPRect rc;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  DBGPRINTF_HIGH("MP: CMPMediaTracksPanel::OnSelect().");

  (void)GetFrameRect(&rc);
  // get current focused item
  pItem = m_pListWin->GetItem(m_pListWin->GetItem());
  if(pItem)
  {
    pSetting = pDeviceInfo->m_pSetting;
    if(!pSetting)
      return;

    switch(pItem->GetID())
    {
    case MP_LABEL_TRACK_SELECT:
#ifdef FEATURE_QTV_GENERIC_BCAST
     {
        // detecting playing state
        boolean   bChg = 0;
        int       nState = 0;
        CMediaPlayer* pme = NULL;
        CPlayerWin* pwin = NULL;
        boolean bState = FALSE;

        pme = (CMediaPlayer*)pSetting->m_pOwner;
        if (pme && pme->m_eActiveWin == MPW_PLAYER)
        {
          pwin = (CPlayerWin*)pme->m_pWin;
        }

        if (pwin && pwin->m_pMedia)
        {
          nState = IMEDIA_GetState(pwin->m_pMedia, &bChg);
          bState = TRUE;
        }

        // exit if we are in playing state
        if (!bState || (bChg || (nState != MM_STATE_READY)))
        {
          if (pme)
          {
              (void)ISHELL_Beep(pme->a.m_pIShell, BEEP_ERROR, FALSE);
              (void)ISHELL_Beep(pme->a.m_pIShell, BEEP_ERROR, FALSE);
          }
          DBGPRINTF_HIGH("MP: Track can only be selected in ready state.");
          break;
        }
      }
            
      // delete old tracks
      if(m_pMuteTrack)
        delete m_pMuteTrack;
      m_pMuteTrack = NULL;
      if(m_pSelectTrack)
        delete m_pSelectTrack;
      m_pSelectTrack = NULL;

      // recreate track select panel
      m_pSelectTrack = new CMPMediaTrackSelectPanel(GetOwnerPtr());
      if(m_pSelectTrack && ((CMPMediaTrackSelectPanel*)m_pSelectTrack)->Init(*rc.GetRect()) == FALSE)
      {
        delete m_pSelectTrack;
        m_pSelectTrack = NULL;
        DBGPRINTF_ERROR("MP: error on initializing CMPMediaTrackSelectPanel");
      }
      else if(m_pSelectTrack == NULL)
      {
        DBGPRINTF_ERROR("MP: failed to create CMPMediaTrackSelectPanel");
      }
  
      if(m_pSelectTrack)
      {
        int i;
        char* ps;
        boolean bSelect = FALSE;
        CMediaPlayer* pme;

        pSetting = pDeviceInfo->m_pSetting;
        pme = (CMediaPlayer*)pSetting->m_pOwner;
        
        // read track info
        if(pme && SUCCESS == MPReadTrackInfo(pSetting))
        {
          if(pme->m_nTrackCount)
          {
            // add track items into track select panel
            (void)((CMPMediaTrackSelectPanel*)m_pSelectTrack)->CreateTrackItems(pme->m_nTrackCount);
            for(i = 0; i < pme->m_nTrackCount; i++)
            {
              // for each track, we need to check selection status, adn add into track panel
              if(SUCCESS != MPIsTrackSelected(pSetting, pme->m_pTrackList[i].nTrackID, &bSelect))
              {
                // can’t determine, set it to unselected
                bSelect = FALSE;
              }

              // get track name string for displaying
              ps = MPGetTrackInfoString(pSetting, i);
              if(ps)
              {
                // add item into panel now
                (void)((CMPMediaTrackSelectPanel*)m_pSelectTrack)->AddTrackItem(i,pme->m_pTrackList[i].nTrackID, bSelect, ps);
                FREE(ps);
              }
            }
          }
        }
        else
        {
          DBGPRINTF_ERROR("MP: MPReadTrackInfo failed.");
        }
      }
  
#endif //FEATURE_QTV_GENERIC_BCAST
      break;

    case MP_LABEL_TRACK_MUTE:
#ifdef FEATURE_QTV_GENERIC_BCAST

      if(m_pMuteTrack)
        delete m_pMuteTrack;
      m_pMuteTrack = NULL;
      if(m_pSelectTrack)
        delete m_pSelectTrack;
      m_pSelectTrack = NULL;
     
      // create mute track panel
      m_pMuteTrack = new CMPMediaTrackMutePanel(GetOwnerPtr());
      if(m_pMuteTrack && ((CMPMediaTrackMutePanel*)m_pMuteTrack)->Init(*rc.GetRect()) == FALSE)
      {
        delete m_pMuteTrack;
        m_pMuteTrack = NULL;
        DBGPRINTF_ERROR("MP: error on initializing CMPMediaTrackMutePanel");
      }
      else if(m_pMuteTrack == NULL)
      {
        DBGPRINTF_ERROR("MP: failed to create CMPMediaTrackMutePanel");
      }

      if(m_pMuteTrack)
      {
        int i;
        char* ps;
        //MPSetting* pSetting;
        boolean bMute = FALSE;
        CMediaPlayer* pme;
        boolean bSelect;

        pSetting = pDeviceInfo->m_pSetting;
        pme = (CMediaPlayer*)pSetting->m_pOwner;
     
        if(SUCCESS == MPReadTrackInfo(pSetting))
        {
          if(pme->m_nTrackCount)
          {
            (void)((CMPMediaTrackMutePanel*)m_pMuteTrack)->CreateTrackItems(pme->m_nTrackCount);
            for(i = 0; i < pme->m_nTrackCount; i ++)
            {
              if(SUCCESS != MPIsTrackSelected(pSetting, pme->m_pTrackList[i].nTrackID, &bSelect))
              {
                // can’t determine, set it to unselected
                bSelect = FALSE;
              }

              // selected track
              if(SUCCESS != MPIsTrackMuted(pSetting, pme->m_pTrackList[i].nTrackID, &bMute))
              {
                bMute = FALSE;
              }

              ps = MPGetTrackInfoString(pSetting, i);
              if( ps)
              {
                (void)((CMPMediaTrackMutePanel*)m_pMuteTrack)->AddTrackItem(i, pme->m_pTrackList[i].nTrackID, bMute, ps, bSelect);
                FREE(ps);
              }
            }
            // set focus to the first selected item 
            ((CMPMediaTrackMutePanel*)m_pMuteTrack)->SetItemFocus();
          }
          else
          {
            DBGPRINTF_HIGH("MP: 0 track count.");
          }
        }
      }
     
#endif //FEATURE_QTV_GENERIC_BCAST
      break;

    default:
      DBGPRINTF_MED("MP: Unexpected item ID = %d", pItem->GetID());
      break;
    }
  }
}

