/*=============================================================================
  FILE: MPQTVSettingPanel.cpp

  SERVICES: CMPQTVSettingPanel class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPQTVSettingPanel.cpp#5 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPQTVSettingPanel.h"
#include "MPSettings.h"
#include "MediaPlayer.h"

#define LB_SUSPEND_NAME       "Post suspend event"
#define LB_RESUME_NAME        "Post resume event"


CMPQTVSettingPanel::CMPQTVSettingPanel(void* pOwner): CMPSymbol(pOwner),
  m_pNoteString(NULL), m_pListWin(NULL)
{
  int i;
  for(i=0; i< MP_QTV_SETTING_MAX; i++)
  {
    m_pSymbol[i] = NULL;
    m_NameString[i] = NULL;
  }
}

CMPQTVSettingPanel::~CMPQTVSettingPanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  Cleanup();
  /*lint +e1551*/
}

void CMPQTVSettingPanel::Cleanup()
{
  int i;
  for(i=0; i < MP_QTV_SETTING_MAX; i++)
  {
    if(m_NameString[i])
      delete m_NameString[i];
    if(m_pSymbol[i])
      delete m_pSymbol[i];

    m_pSymbol[i] = NULL;
    m_NameString[i] = NULL;
  }

  if(m_pNoteString)
    delete m_pNoteString;
  m_pNoteString = NULL;

  if(m_pListWin)
    delete m_pListWin;
  m_pListWin = NULL;
}

boolean CMPQTVSettingPanel::Init(AEERect rc)
{
  int i;
  uint32 nRowHeight;
  AEERect rcPanel = rc;
  IDisplay* pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return FALSE;

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return FALSE;

  Cleanup();

  // set colors
  GetBkColor()->SetColor(153, 204, 204);
  GetFgColor()->SetColor(0, 212, 212);
  GetSdColor()->SetColor(0, 150, 150);

  SetFrameRect(rc.x, rc.y, rc.dx, rc.dy);

  // the height of the row should be the char height + frame border
  nRowHeight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;

  // create a note label to display warning
  m_NoteLabel.SetOwnerPtr(GetOwnerPtr());
  m_NoteLabel.SetFrameRect(rcPanel.x + 2, rcPanel.y + 2, rcPanel.dx - 4, nRowHeight + 2);

  m_NoteLabel.GetBkColor()->SetColor(0, 0, 160); //212, 255, 255
  m_NoteLabel.GetFgColor()->SetColor(0, 212, 212);
  m_NoteLabel.GetSdColor()->SetColor(0, 150, 150);
  m_NoteLabel.GetTextColor()->SetColor(255, 0, 0);

  m_pNoteString = MP_ToAEEChar("Use only when QTV is able to PAUSE!!!");
  m_NoteLabel.SetName(m_pNoteString);

  // create list win now
  rcPanel.y += nRowHeight + 3;
  m_pListWin = new CMPListWin<CMPSymbol>(pDeviceInfo->m_pShell, pDeviceInfo->m_pDisplay, rcPanel); 
  if(!m_pListWin)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pListWin.");
    return FALSE;
  }

  // init list with 1 col and MP_AUDIO_SETTING_MAX rows
  if(!m_pListWin->Init(nRowHeight, 1, MP_QTV_SETTING_MAX))
  {
    delete m_pListWin;
    m_pListWin = NULL;
    DBGPRINTF_ERROR("MP: Unable to initialize m_pListWin.");
    return FALSE;
  }

  m_pListWin->GetBkColor()->SetColor(153, 204, 204);
  m_pListWin->GetFgColor()->SetColor(0, 0, 0);
  m_pListWin->GetSdColor()->SetColor(150, 150, 150);

  // label names in list win
  m_NameString[MP_QTV_SETTING_SUSPEND] = MP_ToAEEChar(LB_SUSPEND_NAME);
	m_NameString[MP_QTV_SETTING_RESUME] = MP_ToAEEChar(LB_RESUME_NAME);

  CMPCheckBox* pCheckBox;
  for(i=0; i < MP_QTV_SETTING_MAX; i++)
  {
    if(i == MP_QTV_SETTING_SUSPEND ||
       i == MP_QTV_SETTING_RESUME)
    {
      pCheckBox = new CMPCheckBox;
      if(!pCheckBox)
      {
        DBGPRINTF_ERROR("MP: error on creating m_pSymbol %d.", i);
        Cleanup();
        return FALSE;
      }
    }
    else
    {
      continue;
    }

    rc.x += 1;
    rc.y += 1;
    // save owner into label, so that we can pass the screen info
    pCheckBox->SetOwnerPtr(GetOwnerPtr());
    pCheckBox->SetID(i);
    pCheckBox->SetFrameRect(rc.x + 1, rc.y + i*nRowHeight, rc.dx/3, nRowHeight);

    pCheckBox->GetBkColor()->SetColor(153, 204, 204); //212, 255, 255
    pCheckBox->GetFgColor()->SetColor(0, 212, 212);
    pCheckBox->GetSdColor()->SetColor(0, 150, 150);
    pCheckBox->GetTextColor()->SetColor(0, 0, 0);
    pCheckBox->SetName(m_NameString[i]);

    m_pSymbol[i] = pCheckBox;
    m_pListWin->Add(m_pSymbol[i]);
  }

  m_pListWin->SetItem(0);
  SetFocus(TRUE);
  return TRUE;
}

boolean CMPQTVSettingPanel::Draw(IGraphics *pg)
{
  (void)m_NoteLabel.Draw(pg);
  return m_pListWin->Draw(pg);
}

// key event handler
boolean CMPQTVSettingPanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  boolean bRet;

  if(!GetFocus())
    return FALSE;
   
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
         bRet = TRUE;
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

void CMPQTVSettingPanel::OnSelect()
{
  CMPSymbol* pItem;
  MPSetting* pSetting;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;
   
  // get current focused item
  pItem = m_pListWin->GetItem(m_pListWin->GetItem());
  if(pItem)
  {
    pSetting = pDeviceInfo->m_pSetting;
    switch(pItem->GetID())
    {
    case MP_QTV_SETTING_SUSPEND:
      MPSetSuspend(pSetting);
      break;
    case MP_QTV_SETTING_RESUME:
      MPSetResume(pSetting);
      break;
    default:
      DBGPRINTF_MED("MP: Unexpected item ID = %d", pItem->GetID());
      break;
    }
  }
}
