/*=============================================================================
  FILE: MPAudioExpanderPanel.cpp

  SERVICES: CMPAudioExpanderPanel class implementation
 
  GENERAL DESCRIPTION: 


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPAudioExpanderPanel.cpp#14 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPAudioExpanderPanel.h"
#ifdef FEATURE_MP_EQ_UI

CMPAudioExpanderPanel::CMPAudioExpanderPanel(void* pOwner):CMPSymbol(pOwner),
  m_pwszNameString(NULL), m_nNumofEditItems(0), m_pLabels(NULL), m_pCommit(NULL), m_pListWin(NULL)
{
}

CMPAudioExpanderPanel::~CMPAudioExpanderPanel()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  if(m_pLabels)
    delete[] m_pLabels;
  m_pLabels = NULL;

  if(m_pCommit)
    delete m_pCommit;
  m_pCommit = NULL;
  
  if(m_pListWin)
    delete m_pListWin;
  m_pListWin = NULL;
  
  if(m_pwszNameString)
    delete [] m_pwszNameString;
  m_pwszNameString = NULL;

  DBGPRINTF_LOW("MP: end deleting CMPAudioExpanderPanel.");
  /*lint +e1551*/
}

void CMPAudioExpanderPanel::Destroy()
{
  delete this;
}
boolean CMPAudioExpanderPanel::Init(AEERect rc)
{
  int i;
  int offset;
  int stringWidth;
  uint32 charHight;
  IDisplay* pDisplay = NULL;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo )
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

  GetBkColor()->SetColor(212, 255, 255);
  GetFgColor()->SetColor(0, 212, 212);
  GetSdColor()->SetColor(0, 150, 150);

  if(m_pLabels)
    delete[] m_pLabels;
  m_pLabels = NULL;

  m_nNumofEditItems = MP_LABEL_MAX - 1;

  charHight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;
  m_pListWin = new CMPListWin<CMPSymbol>(pDeviceInfo->m_pShell, pDisplay, rc); 
  if(!m_pListWin)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pListWin.");
    return FALSE;
  }

  if(!m_pListWin->Init(charHight, 1, MP_LABEL_MAX))
  {
    delete m_pListWin;
    m_pListWin = NULL;
    DBGPRINTF_ERROR("MP: Unable to initialize m_pListWin.");
    return FALSE;
  }

  // create labes 
  m_pLabels = new CMPNumEdit[m_nNumofEditItems];
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

  // calculate the max row length needed, a row contains a label, a texteditor and a scrollbar
  // the max length of label string is "Spread(-32,32): "
  // the texteditor width is set to length of "9999999"
  // the scrollbar + leading and trailing space is about 16
  stringWidth = GetStringWidth("Spread(-32,32): ") + GetStringWidth("9999999") + 16;

  // set label names   
  if(rc.dx >= stringWidth)
  {
    m_pLabels[0].SetLabelName("Mode(0,3): ");
    m_pLabels[1].SetLabelName("Gain(0,32767): ");
    m_pLabels[2].SetLabelName("Spread(0,32767): ");
    m_pLabels[3].SetLabelName("Delay(-32,32): ");
  }
  else
  {
    // with limited screen space, use following texts
    m_pLabels[0].SetLabelName("Mode: ");
    m_pLabels[1].SetLabelName("Gain: ");
    m_pLabels[2].SetLabelName("Spread: ");
    m_pLabels[3].SetLabelName("Delay: ");
  }
  // calculate max offset
  offset = GetMaxLabelLength();
  // string width for the edit ctl
  stringWidth = GetStringWidth("9999999");

  for(i=0; i < m_nNumofEditItems; i++)
  {
    // save owner into label, so that we can pass the screen info
    m_pLabels[i].SetOwnerPtr(GetOwnerPtr());
    m_pLabels[i].SetID(i);
    m_pLabels[i].SetRectOffset(offset);
    if(stringWidth)
      m_pLabels[i].SetWidth(stringWidth);

    m_pLabels[i].GetBkColor()->SetColor(212, 255, 255);
    m_pLabels[i].GetFgColor()->SetColor(0, 212, 212);
    m_pLabels[i].GetSdColor()->SetColor(0, 150, 150);
    m_pLabels[i].GetTextColor()->SetColor(0, 0, 0);

    m_pListWin->Add(&m_pLabels[i]);
  }

  m_pLabels[0].SetRange(0, 3);
  m_pLabels[0].SetIncStep(1);
  m_pLabels[1].SetRange(0, 32767);
  m_pLabels[1].SetIncStep(2724);
  m_pLabels[2].SetRange(0, 32767);
  m_pLabels[2].SetIncStep(3276);
  m_pLabels[3].SetRange(-32, 32);
  m_pLabels[3].SetIncStep(2);

#ifdef FEATURE_MP_EQ
  MPSetting* pSetting;
  pSetting = pDeviceInfo->m_pSetting;
  if(pSetting)
  {
    CMPNumEdit* pItem;

    // set initial values
    pItem = ((CMPNumEdit*)m_pListWin->GetItem(MP_LABEL_MODE));
    if(!pItem)
    {
      DBGPRINTF_ERROR("MP: pItem is NULL");
      return FALSE;
    }
    pItem->SetValue(pSetting->m_AudioExpanderData.AudioExp.dwMode);

    pItem = ((CMPNumEdit*)m_pListWin->GetItem(MP_LABEL_GAIN));
    if(!pItem)
    {
      DBGPRINTF_ERROR("MP: pItem is NULL");
      return FALSE;
    }
    pItem->SetValue(pSetting->m_AudioExpanderData.AudioExp.nGain);

    pItem = ((CMPNumEdit*)m_pListWin->GetItem(MP_LABEL_SPREAD));
    if(!pItem)
    {
      DBGPRINTF_ERROR("MP: pItem is NULL");
      return FALSE;
    }
    pItem->SetValue(pSetting->m_AudioExpanderData.AudioExp.nSpread);

    pItem = ((CMPNumEdit*)m_pListWin->GetItem(MP_LABEL_GEOMETRY));
    if(!pItem)
    {
      DBGPRINTF_ERROR("MP: pItem is NULL");
      return FALSE;
    }
    pItem->SetValue(pSetting->m_AudioExpanderData.AudioExp.nGeometry);
  }
#endif //FEATURE_MP_EQ

  // create commit button
  m_pCommit = new CMPLabel;
  if(!m_pCommit)
  {
    delete m_pListWin;
    m_pListWin = NULL;
    delete [] m_pLabels;
    m_pLabels = NULL;

    DBGPRINTF_ERROR("MP: filed to create button.");
    return FALSE;
  }

  // save owner into label, so that we can pass the screen info
  m_pCommit->SetOwnerPtr(GetOwnerPtr());
  m_pCommit->SetID(MP_BUTTON_COMMIT);
  m_pwszNameString = MP_ToAEEChar("Commit");
  m_pCommit->SetName(m_pwszNameString);
  m_pCommit->SetAllignText(IDF_ALIGN_CENTER);

  m_pCommit->GetBkColor()->SetColor(212, 255, 255);
  m_pCommit->GetFgColor()->SetColor(0, 212, 212);
  m_pCommit->GetSdColor()->SetColor(0, 150, 150);
  m_pCommit->GetTextColor()->SetColor(0, 0, 0);
  m_pListWin->Add(m_pCommit);
  
  m_pLabels[0].SetFocus(TRUE);
  m_pLabels[0].SetSelection(TRUE);
  SetFocus(TRUE);
  return TRUE;
}

// this function returns max string length for all labels
int CMPAudioExpanderPanel::GetMaxLabelLength()
{
  int i;
  int max = 0;
  int length;
  IDisplay* pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
  {
    return max;
  }

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
  {
    return max;
  }

  for(i=0; i < m_nNumofEditItems; i++)
  {
    length = IDISPLAY_MeasureText(pDisplay,
                            AEE_FONT_NORMAL, 
                            m_pLabels[i].GetLabelName());
    if(length > max)
      max = length;
  }
  return max;
}
// this function returns the width of given string
int CMPAudioExpanderPanel::GetStringWidth(const char* pstr)
{
  int width = 0;
  IDisplay* pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
  {
    return width;
  }

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
  {
    return width;
  }

  AECHAR* pAEChar = MP_ToAEEChar(pstr);
  if(pAEChar)
  {
    width = IDISPLAY_MeasureText(pDisplay, AEE_FONT_NORMAL, pAEChar);
    delete[] pAEChar;
  }
  return width;
}

boolean CMPAudioExpanderPanel::Draw(IGraphics *pg)
{
  return m_pListWin->Draw(pg);
}

boolean CMPAudioExpanderPanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  boolean ret = FALSE;
  int idx;
  CMPSymbol* pSymbol;

  if(!GetFocus() || !m_pListWin)
    return ret;

  idx = m_pListWin->GetItem();
  pSymbol = m_pListWin->GetItem(idx);
  if(NULL == pSymbol)
  {
    DBGPRINTF_ERROR("MP: NULL symbol.");
    return FALSE;
  }

  switch(eCode)
  {
    case EVT_KEY_PRESS:
    {
      switch(wParam)
      {
       case AVK_SELECT:
         if(MP_BUTTON_COMMIT  == idx)
         {
            // commit the changes
           (void)Commit();
         }
         else
         {
            // the focus is in edit ctl, pass the event
           ret = ((CMPNumEdit*)pSymbol)->OnEvent(eCode, wParam, dwParam);
         }
         break;

       case AVK_UP:
         if(MP_BUTTON_COMMIT != idx)
         {
          // turn off edit mode
           ((CMPNumEdit*)pSymbol)->SetEditMode(FALSE);
         }

         if(!m_pListWin->SetPrevious())
         {
           //top, move focus to last item
           m_pListWin->SetLast();
         }
         ret = TRUE;
         break;

       case AVK_DOWN:
         if(MP_BUTTON_COMMIT != idx)
         {
          // turn off edit mode
           ((CMPNumEdit*)pSymbol)->SetEditMode(FALSE);
         }
         if(!m_pListWin->SetNext())
         {
           //top, move focus to first item
           m_pListWin->SetFirst();
         }
         ret = TRUE;
         break;

       case AVK_LEFT:
         ret = pSymbol->OnEvent(eCode, wParam, dwParam);
        break;

       case AVK_RIGHT:
         ret = pSymbol->OnEvent(eCode, wParam, dwParam);
        break;

       default:
         ret = pSymbol->OnEvent(eCode, wParam, dwParam);
        break;
      }
      break;
    }
    default:
      ret = pSymbol->OnEvent(eCode, wParam, dwParam);
      break;
  }

  if(MP_BUTTON_COMMIT != idx)
  {
    MPSetting*    pSetting;
    MPDeviceInfo* pDeviceInfo = NULL;

    pDeviceInfo = MP_GetInfo(this);
    if (!pDeviceInfo)
    {
      DBGPRINTF_ERROR("MP: pDeviceInfo is NULL");
      return FALSE;
    }

    pSetting = pDeviceInfo->m_pSetting;
    if(!pSetting)
    {
      DBGPRINTF_ERROR("MP: pSetting is NULL");
      return FALSE;
    }

    // save edit mode into setting object, so that MP can 
    // use this information
    pSetting->m_bEditMode = ((CMPNumEdit*)pSymbol)->GetEditMode();
  }

  return ret;
}

// this function save the current changes into setting object, and also
// set into OEM
void CMPAudioExpanderPanel::Commit()
{
  MPSetting* pSetting;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
  {
    DBGPRINTF_ERROR("MP: pDeviceInfo is NULL");
    return;
  }

  pSetting = pDeviceInfo->m_pSetting;
  if(pSetting)
  {
#ifdef FEATURE_MP_EQ
    AEESoundCtlAudioExpander pAudioExp;
    CMPNumEdit* pItem;

    pItem = ((CMPNumEdit*)m_pListWin->GetItem(MP_LABEL_MODE));
    if(!pItem)
    {
      DBGPRINTF_ERROR("MP: pItem is NULL");
      return;
    }
    pAudioExp.dwMode = pItem->GetValue();

    pItem = ((CMPNumEdit*)m_pListWin->GetItem(MP_LABEL_GAIN));
    if(!pItem)
    {
      DBGPRINTF_ERROR("MP: pItem is NULL");
      return;
    }
    pAudioExp.nGain = pItem->GetValue();

    pItem = ((CMPNumEdit*)m_pListWin->GetItem(MP_LABEL_SPREAD));
    if(!pItem)
    {
      DBGPRINTF_ERROR("MP: pItem is NULL");
      return;
    }
    pAudioExp.nSpread = pItem->GetValue();

    pItem = ((CMPNumEdit*)m_pListWin->GetItem(MP_LABEL_GEOMETRY));
    if(!pItem)
    {
      DBGPRINTF_ERROR("MP: pItem is NULL");
      return;
    }
    pAudioExp.nGeometry = pItem->GetValue();

    pAudioExp.dwStructSize = sizeof(AEESoundCtlAudioExpander);
    (void)MPAudioSetExpander(pSetting, &pAudioExp);
#endif
  }
}

#endif // FEATURE_MP_EQ_UI
