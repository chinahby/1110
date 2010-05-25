/*=============================================================================
  FILE: MPBcastSettingsPanel.cpp

  SERVICES: CMPBcastSettingsPanel class implementation
 
  GENERAL DESCRIPTION: 
    Panel containing controls for viewing and setting various Video Broadcast
    settings.

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/mediaplayer/MPBcastSettingsPanel.cpp#8 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPBcastSettingsPanel.h"
#ifdef FEATURE_QTV_GENERIC_BCAST

static const char* pBcastSettingsLabelNames[] = {
  "Reposition Timestamp (ms)",  //MP_BCAST_SETTINGS_TIMESTAMP
  "Seek Frame Offset",          //MP_BCAST_SETTINGS_OFFSET
  NULL
};

CMPBcastSettingsPanel::CMPBcastSettingsPanel(void* pOwner):
  CMPSymbol(pOwner), m_nRowHeight(0), m_nOffset(0), m_nWidth(0), m_pLabels(NULL), m_pListWin(NULL)
{
  m_nNumofItems = MP_BCAST_SETTINGS_MAX;
}

CMPBcastSettingsPanel::~CMPBcastSettingsPanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  if(m_pLabels)
    delete[] m_pLabels;
  m_pLabels = NULL;
  if(m_pListWin)
    delete m_pListWin;
  m_pListWin = NULL;
  DBGPRINTF_LOW("MP: end deleting CMPBcastSettingsPanel.");
  /*lint +e1551*/
}

void CMPBcastSettingsPanel::Destroy()
{
  delete this;
}

boolean CMPBcastSettingsPanel::Init(AEERect rc)
{
  boolean     bRet;
  MPSetting*  pSetting;
  IDisplay*   pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
  {
    DBGPRINTF_ERROR("MP: pDeviceInfo is NULL");
    return FALSE;
  }

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
  {
    DBGPRINTF_ERROR("MP: pDisplay is NULL");
    return FALSE;
  }

  pSetting = pDeviceInfo->m_pSetting;
  if(!pSetting)
  {
    DBGPRINTF_ERROR("MP: pSetting is NULL");
    return FALSE;
  }

  // set colors
  GetBkColor()->SetColor(212, 255, 255);
  GetFgColor()->SetColor(0, 212, 212);
  GetSdColor()->SetColor(0, 150, 150);

  if(m_pLabels)
    delete[] m_pLabels;
  m_pLabels = NULL;

  m_pListWin = new CMPListWin<CMPNumEdit>(pDeviceInfo->m_pShell, pDisplay, rc); 
  if(!m_pListWin)
  {
    DBGPRINTF_ERROR("MP: Error creating m_pListWin.");
    return FALSE;
  }

  // the height of the row should be the char height + frame border
  m_nRowHeight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;
  // calculate offset
  m_nOffset = GetMaxLabelLength()+ 2;
  // string width for the edit ctl
  m_nWidth = GetStringWidth("9999999");

  // init list with 1 col and MP_BCAST_SETTING_MAX rows
  if(!m_pListWin->Init(m_nRowHeight, 1, m_nNumofItems))
  {
    delete m_pListWin;
    m_pListWin = NULL;
    DBGPRINTF_ERROR("MP: Unable to initialize m_pListWin.");
    return FALSE;
  }

    // create labels 
  m_pLabels = new CMPNumEdit[m_nNumofItems];
  if(!m_pLabels)
  {
    delete m_pListWin;
    m_pListWin = NULL;
    DBGPRINTF_ERROR("MP: Error on creating m_pLabels.");
    return FALSE;
  }

  SetFrameRect(rc.x, rc.y, rc.dx, rc.dy);

  m_pListWin->GetBkColor()->SetColor(212, 255, 255);
  m_pListWin->GetFgColor()->SetColor(0, 0, 0);
  m_pListWin->GetSdColor()->SetColor(150, 150, 150);

  m_pListWin->SetLeftMargin(2);
  m_pListWin->SetScrollBarOnLeft(FALSE);

  // Create Reposition Timestamp numeric edit box
  bRet = TRUE;
  if (bRet)
  {
    bRet = CreateNumEdit(0, 999999, 1, MP_BCAST_SETTINGS_TIMESTAMP, 
                         (char *)pBcastSettingsLabelNames[MP_BCAST_SETTINGS_TIMESTAMP]);
  }
  // Create Frame Offset numeric edit box
  if (bRet)
  {
    bRet = CreateNumEdit(0, 999999, 1, MP_BCAST_SETTINGS_OFFSET, 
                         (char *)pBcastSettingsLabelNames[MP_BCAST_SETTINGS_OFFSET]);
  }

  CMPNumEdit* pTimestamp = m_pListWin->GetItem(MP_BCAST_SETTINGS_TIMESTAMP);
  if (!pTimestamp)
  {
    DBGPRINTF_ERROR("MP: Error getting Timestamp item");
    return FALSE;
  }

  CMPNumEdit* pOffset = m_pListWin->GetItem(MP_BCAST_SETTINGS_OFFSET);
  if (!pOffset)
  {
    DBGPRINTF_ERROR("MP: Error getting Offset item");
    return FALSE;
  }

  // set initial values
  pTimestamp->SetValue(pSetting->m_BcastData.nTimestamp);
  pOffset->SetValue(pSetting->m_BcastData.nOffset);

  m_pLabels[0].SetFocus(TRUE);
  m_pLabels[0].SetSelection(TRUE);
  SetFocus(TRUE);

  return TRUE;
}

boolean CMPBcastSettingsPanel::CreateNumEdit(int iMinVal, int iMaxVal, int iStepVal, int id, char* pName)
{
  DBGPRINTF_MED("MP: Creating NumEdit for %s", pName);

  // save owner into label, so that we can pass the screen info
  m_pLabels[id].SetLabelName(pName);
  m_pLabels[id].SetOwnerPtr(GetOwnerPtr());
  m_pLabels[id].SetID(id);
  m_pLabels[id].SetRectOffset(m_nOffset);
  m_pLabels[id].SetWidth(m_nWidth);
  
  m_pLabels[id].GetBkColor()->SetColor(212, 255, 255);
  m_pLabels[id].GetFgColor()->SetColor(0, 212, 212);
  m_pLabels[id].GetSdColor()->SetColor(0, 150, 150);
  m_pLabels[id].GetTextColor()->SetColor(0, 0, 0);
  m_pLabels[id].SetRange(iMinVal, iMaxVal);

  m_pLabels[id].SetIncStep(iStepVal);

  m_pListWin->Add(&m_pLabels[id]);

  return TRUE;
}


// this function returns max string length for all labels
int CMPBcastSettingsPanel::GetMaxLabelLength()
{
  int       i;
  int       max = 0;
  int       length;

  for (i=0; i < m_nNumofItems; i++)
  {
    length = GetStringWidth(pBcastSettingsLabelNames[i]);
    if (length > max)
      max = length;
  }
  return max;
}

// this function returns the width of given string
int CMPBcastSettingsPanel::GetStringWidth(const char* pstr)
{
  int       width = 0;
  IDisplay* pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if (!pDeviceInfo)
  {
    DBGPRINTF_ERROR("MP: pDeviceInfo is NULL");
    return 0;
  }

  pDisplay = pDeviceInfo->m_pDisplay;
  if (!pDisplay)
  {
    DBGPRINTF_ERROR("MP: pDisplay is NULL");
    return 0;
  }

  AECHAR* pAEChar = MP_ToAEEChar(pstr);
  if (pAEChar)
  {
    width = IDISPLAY_MeasureText(pDisplay, AEE_FONT_NORMAL, pAEChar);
    delete[] pAEChar;
  }
  return width;
}

boolean CMPBcastSettingsPanel::Draw(IGraphics *pg)
{
  return m_pListWin->Draw(pg);
}

boolean CMPBcastSettingsPanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  boolean bRet = FALSE;
  int idx;
  CMPNumEdit* pEdit;

  if(!GetFocus())
    return bRet;

  if(!m_pListWin)
  {
    DBGPRINTF_ERROR("MP: m_pListWin is NULL");
    return FALSE;
  }

  idx = m_pListWin->GetItem();
  pEdit = m_pListWin->GetItem(idx);
  if(!pEdit)
  {
    DBGPRINTF_ERROR("MP: pEdit is NULL");
    return FALSE;
  }

  (void)(pEdit->GetEditMode());
  
  switch(eCode)
  {
  case EVT_KEY_PRESS:
    switch(wParam)
    {
    case AVK_SELECT:
      bRet = pEdit->OnEvent(eCode, wParam, dwParam);
      break;

    case AVK_UP:
    case AVK_DOWN:
      if(!pEdit->OnEvent(eCode, wParam, dwParam))
      {
        bRet = m_pListWin->OnEvent(eCode, wParam, dwParam);
        if(bRet && pEdit)
        {
        // turn off edit mode for previous item
          pEdit->SetEditMode(FALSE);
        }
      }
      bRet = TRUE;
      break;

    case AVK_LEFT:
    case AVK_RIGHT:
      bRet = pEdit->OnEvent(eCode, wParam, dwParam);
    break;

    default:
      if(pEdit && pEdit->GetEditMode())
        bRet = pEdit->OnEvent(eCode, wParam, dwParam);
    break;
    }
  break;

  default:
      bRet = pEdit->OnEvent(eCode, wParam, dwParam);
    break;
  }

  if(pEdit->GetDirty())
  {
    UpdateBcastSettingsCtl();
    pEdit->SetDirty(FALSE);
  }
  return bRet;
}

void CMPBcastSettingsPanel::UpdateBcastSettingsCtl()
{
  MPSetting* pSetting;
  MPDeviceInfo * pDeviceInfo = NULL;

  if(!m_pListWin) 
  {
    DBGPRINTF_ERROR("MP: m_pListWin is NULL");
	  return;
  }

  CMPNumEdit* pTimestamp = m_pListWin->GetItem(MP_BCAST_SETTINGS_TIMESTAMP);
  if (!pTimestamp)
  {
    DBGPRINTF_ERROR("MP: Error getting Timestamp item");
    return;
  }

  CMPNumEdit* pOffset = m_pListWin->GetItem(MP_BCAST_SETTINGS_OFFSET);
  if (!pOffset)
  {
    DBGPRINTF_ERROR("MP: Error getting Offset item");
    return;
  }

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo) 
  {
    DBGPRINTF_ERROR("MP: pDeviceInfo is NULL");
	  return;
  }

  pSetting = pDeviceInfo->m_pSetting;
  if (pSetting)
  {
    pSetting->m_BcastData.nTimestamp = pTimestamp->GetValue();
    pSetting->m_BcastData.nOffset    = pOffset->GetValue();
  }
}

#endif // FEATURE_QTV_GENERIC_BCAST
