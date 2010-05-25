/*=============================================================================
  FILE: MPQConcertPresetPanel.cpp

  SERVICES: CMPQConcertPresetPanel class implementation
 
  GENERAL DESCRIPTION: 


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/mediaplayer/MPQConcertPresetPanel.cpp#6 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPQConcertPresetPanel.h"
#ifdef FEATURE_MP_EQ_UI

// This table really should be maintained by the QConcert level
// Note: these labels need to match order in QConcerts presets enum
const char *PresetLabels[] = 
{
 "Generic",
 "Small Room",
 "Medium Room",
 "Medium Hall",
 "Concert Hall",
 "Surround",
 "Warm Stage",
 "Crystal",
 "Living Room",
 NULL
};
int numPresets = 9;

#define MPQCONCERT_NUMLEVELS_MIN  1
#define MPQCONCERT_NUMLEVELS_MAX  100

#define MPQCONCERT_LEVEL_MIN      1

#define MPQCONCERT_INCSTEP_PRESET    1
#define MPQCONCERT_INCSTEP_NUMLEVELS 1
#define MPQCONCERT_INCSTEP_LEVEL     1

#define MPQCONCERT_EDIT_WIDTH  6


CMPQConcertPresetPanel::CMPQConcertPresetPanel(void* pOwner):CMPSymbol(pOwner),
  m_pPresetHeader(NULL), m_pPresetEdit(NULL), 
  m_pLevelHeader(NULL), m_pLevelEdit(NULL), m_pCommit(NULL), m_pListWin(NULL),
  m_pwszPresetHeader(NULL), m_pwszLevelHeader(NULL), m_pwszCommit(NULL),
  m_pNumLevelsHeader(NULL), m_pNumLevelsEdit(NULL), m_pwszNumLevelsHeader(NULL)
{
}

CMPQConcertPresetPanel::~CMPQConcertPresetPanel()
{
  CMPQConcertPresetCleanup();
}

void CMPQConcertPresetPanel::Destroy()
{
  delete this;
}
boolean CMPQConcertPresetPanel::Init(AEERect rc)
{
  uint32 charHeight;
  IDisplay* pDisplay = NULL;
  MPDeviceInfo * pDeviceInfo = NULL;
  char labelText[64];
  short curPreset;
  short curLevel;
  short numLevels;

  AEERect tmpRC;
  tmpRC.x  = rc.x;
  tmpRC.dx = rc.dx;
  tmpRC.y  = rc.y + 50;
  tmpRC.dx = rc.dy - 50;

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

  // ** Create the ListWin object to contain panel ** //
  charHeight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;
  m_pListWin = new CMPListWin<CMPSymbol>(pDeviceInfo->m_pShell, pDisplay, tmpRC); 
  if(!m_pListWin)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pListWin.");
    goto InitError;
  }
  if(!m_pListWin->Init(charHeight, 1, MP_LABEL_MAX))
  {
    DBGPRINTF_ERROR("MP: Unable to initialize m_pListWin.");
    goto InitError;
  }

  SetFrameRect(tmpRC.x, tmpRC.y, tmpRC.dx, tmpRC.dy);
  m_pListWin->GetBkColor()->SetColor(212, 255, 255);
  m_pListWin->GetFgColor()->SetColor(0, 0, 0);
  m_pListWin->GetSdColor()->SetColor(150, 150, 150);
  m_pListWin->SetLeftMargin(2);
  m_pListWin->SetScrollBarOnLeft(FALSE);


  // ** Create the QConcert Preset label text ** //
  curPreset = 0;

#ifdef FEATURE_MP_EQ
  if(pDeviceInfo->m_pSetting)
  {
    if((pDeviceInfo->m_pSetting->m_AudioExpanderData.curPreset < 0) ||
       (pDeviceInfo->m_pSetting->m_AudioExpanderData.curPreset > numPresets-1))
      pDeviceInfo->m_pSetting->m_AudioExpanderData.curPreset = 0;
    curPreset = pDeviceInfo->m_pSetting->m_AudioExpanderData.curPreset;
  }
#endif // FEATURE_MP_EQ

  if(m_pPresetHeader)
    delete m_pPresetHeader;
  m_pPresetHeader = new CMPLabel;
  if(!m_pPresetHeader)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pPresetHeader.");
    goto InitError;
  }

  // save owner into label, so that we can pass the screen info
  m_pPresetHeader->SetOwnerPtr(GetOwnerPtr());
  m_pPresetHeader->SetID(MP_LABEL_PRESET_HEADER);

  STRLCPY(labelText, " QConcert Presets: ", sizeof(labelText));
  STRLCAT(labelText, PresetLabels[curPreset], sizeof(labelText));
  m_pwszPresetHeader = MP_ToAEEChar(labelText);
  m_pPresetHeader->SetName(m_pwszPresetHeader);
  m_pPresetHeader->SetAllignText(IDF_ALIGN_LEFT);

  m_pPresetHeader->GetBkColor()->SetColor(212, 255, 255);
  m_pPresetHeader->GetFgColor()->SetColor(0, 212, 212);
  m_pPresetHeader->GetSdColor()->SetColor(0, 150, 150);
  m_pPresetHeader->GetTextColor()->SetColor(0, 0, 0);
  m_pListWin->Add(m_pPresetHeader);
  

  // ** Create the QConcert Preset NumEdit control ** //
  if(m_pPresetEdit)
    delete m_pPresetEdit;
  m_pPresetEdit = new CMPNumEdit;
  if(!m_pPresetEdit)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pPresetEdit.");
    goto InitError;
  }

  m_pPresetEdit->SetLabelName("Preset Val: ");

  // save owner into label, so that we can pass the screen info
  m_pPresetEdit->SetOwnerPtr(GetOwnerPtr());
  m_pPresetEdit->SetID(MP_LABEL_PRESET_EDIT);

  m_pPresetEdit->GetBkColor()->SetColor(212, 255, 255);
  m_pPresetEdit->GetFgColor()->SetColor(0, 212, 212);
  m_pPresetEdit->GetSdColor()->SetColor(0, 150, 150);
  m_pPresetEdit->GetTextColor()->SetColor(0, 0, 0);
  m_pListWin->Add(m_pPresetEdit);
  m_pPresetEdit->SetWidth(MPQCONCERT_EDIT_WIDTH);
  m_pPresetEdit->SetAllignText(IDF_ALIGN_CENTER);
  m_pPresetEdit->SetRange(0, numPresets-1);
  m_pPresetEdit->SetIncStep(MPQCONCERT_INCSTEP_PRESET);
  m_pPresetEdit->SetValue(curPreset);


  // ** Create the QConcert Num Levels label text ** //
  if(m_pNumLevelsHeader)
    delete m_pNumLevelsHeader;
  m_pNumLevelsHeader = new CMPLabel;
  if(!m_pNumLevelsHeader)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pNumLevelsHeader.");
    goto InitError;
  }

  // save owner into label, so that we can pass the screen info
  m_pNumLevelsHeader->SetOwnerPtr(GetOwnerPtr());
  m_pNumLevelsHeader->SetID(MP_LABEL_NUMLEVELS_HEADER);
  m_pwszNumLevelsHeader = MP_ToAEEChar(" QConcert Num Levels:");
  m_pNumLevelsHeader->SetName(m_pwszNumLevelsHeader);
  m_pNumLevelsHeader->SetAllignText(IDF_ALIGN_LEFT);

  m_pNumLevelsHeader->GetBkColor()->SetColor(212, 255, 255);
  m_pNumLevelsHeader->GetFgColor()->SetColor(0, 212, 212);
  m_pNumLevelsHeader->GetSdColor()->SetColor(0, 150, 150);
  m_pNumLevelsHeader->GetTextColor()->SetColor(0, 0, 0);
  m_pListWin->Add(m_pNumLevelsHeader);


  // ** Create the QConcert Num Levels Strength NumEdit control ** //
  m_pNumLevelsEdit = new CMPNumEdit;
  if(!m_pNumLevelsEdit)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pNumLevelsEdit.");
    goto InitError;
  }
  m_pNumLevelsEdit->SetLabelName("Num Levels: ");

  // save owner into label, so that we can pass the screen info
  m_pNumLevelsEdit->SetOwnerPtr(GetOwnerPtr());
  m_pNumLevelsEdit->SetID(MP_LABEL_NUMLEVELS_EDIT);

  m_pNumLevelsEdit->GetBkColor()->SetColor(212, 255, 255);
  m_pNumLevelsEdit->GetFgColor()->SetColor(0, 212, 212);
  m_pNumLevelsEdit->GetSdColor()->SetColor(0, 150, 150);
  m_pNumLevelsEdit->GetTextColor()->SetColor(0, 0, 0);
  m_pListWin->Add(m_pNumLevelsEdit);
  m_pNumLevelsEdit->SetWidth(MPQCONCERT_EDIT_WIDTH);
  m_pNumLevelsEdit->SetAllignText(IDF_ALIGN_CENTER);
  m_pNumLevelsEdit->SetRange(MPQCONCERT_NUMLEVELS_MIN, MPQCONCERT_NUMLEVELS_MAX);
  m_pNumLevelsEdit->SetIncStep(MPQCONCERT_INCSTEP_NUMLEVELS);

  numLevels = MPQCONCERT_NUMLEVELS_MIN;
#ifdef FEATURE_MP_EQ
  if(pDeviceInfo->m_pSetting)
    numLevels = pDeviceInfo->m_pSetting->m_AudioExpanderData.numLevels;
#endif // FEATURE_MP_EQ
  m_pNumLevelsEdit->SetValue(numLevels);
 
  // ** Create the QConcert Level Strength label text ** //
  if(m_pLevelHeader)
    delete m_pLevelHeader;
  m_pLevelHeader = new CMPLabel;
  if(!m_pLevelHeader)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pLevelHeader.");
    goto InitError;
  }

  // save owner into label, so that we can pass the screen info
  m_pLevelHeader->SetOwnerPtr(GetOwnerPtr());
  m_pLevelHeader->SetID(MP_LABEL_LEVEL_HEADER);
  m_pwszLevelHeader = MP_ToAEEChar(" QConcert Level:");
  m_pLevelHeader->SetName(m_pwszLevelHeader);
  m_pLevelHeader->SetAllignText(IDF_ALIGN_LEFT);

  m_pLevelHeader->GetBkColor()->SetColor(212, 255, 255);
  m_pLevelHeader->GetFgColor()->SetColor(0, 212, 212);
  m_pLevelHeader->GetSdColor()->SetColor(0, 150, 150);
  m_pLevelHeader->GetTextColor()->SetColor(0, 0, 0);
  m_pListWin->Add(m_pLevelHeader);
  

  // ** Create the QConcert Level Strength NumEdit control ** //
  m_pLevelEdit = new CMPNumEdit;
  if(!m_pLevelEdit)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pLevelEdit.");
    goto InitError;
  }

  m_pLevelEdit->SetLabelName("Strength: ");

  // save owner into label, so that we can pass the screen info
  m_pLevelEdit->SetOwnerPtr(GetOwnerPtr());
  m_pLevelEdit->SetID(MP_LABEL_LEVEL_EDIT);

  m_pLevelEdit->GetBkColor()->SetColor(212, 255, 255);
  m_pLevelEdit->GetFgColor()->SetColor(0, 212, 212);
  m_pLevelEdit->GetSdColor()->SetColor(0, 150, 150);
  m_pLevelEdit->GetTextColor()->SetColor(0, 0, 0);
  m_pListWin->Add(m_pLevelEdit);
  m_pLevelEdit->SetWidth(MPQCONCERT_EDIT_WIDTH);
  m_pLevelEdit->SetAllignText(IDF_ALIGN_CENTER);
  m_pLevelEdit->SetRange(1, numLevels);
  m_pLevelEdit->SetIncStep(MPQCONCERT_INCSTEP_LEVEL);

  curLevel = MPQCONCERT_LEVEL_MIN;
#ifdef FEATURE_MP_EQ
  if(pDeviceInfo->m_pSetting)
  {
    if((pDeviceInfo->m_pSetting->m_AudioExpanderData.curLevel < MPQCONCERT_LEVEL_MIN) ||
       (pDeviceInfo->m_pSetting->m_AudioExpanderData.curLevel > pDeviceInfo->m_pSetting->m_AudioExpanderData.numLevels))
    {
      pDeviceInfo->m_pSetting->m_AudioExpanderData.curLevel = MPQCONCERT_LEVEL_MIN;
    }
    curLevel = pDeviceInfo->m_pSetting->m_AudioExpanderData.curLevel;
  }
#endif // FEATURE_MP_EQ
  m_pLevelEdit->SetValue(curLevel);


  // ** Create the QConcert Commit control ** //
  m_pCommit = new CMPLabel;
  if(!m_pCommit)
  {
    DBGPRINTF_ERROR("MP: Error on creating m_pCommit.");
    goto InitError;
  }

  // save owner into label, so that we can pass the screen info
  m_pCommit->SetOwnerPtr(GetOwnerPtr());
  m_pCommit->SetID(MP_BUTTON_COMMIT);
  m_pwszCommit = MP_ToAEEChar("Commit");
  m_pCommit->SetName(m_pwszCommit);
  m_pCommit->SetAllignText(IDF_ALIGN_CENTER);

  m_pCommit->GetBkColor()->SetColor(212, 255, 255);
  m_pCommit->GetFgColor()->SetColor(0, 212, 212);
  m_pCommit->GetSdColor()->SetColor(0, 150, 150);
  m_pCommit->GetTextColor()->SetColor(0, 0, 0);
  m_pListWin->Add(m_pCommit);
  
  m_pPresetEdit[0].SetFocus(TRUE);
  m_pPresetEdit[0].SetSelection(TRUE);
  SetFocus(TRUE);
  return TRUE;

InitError:
  CMPQConcertPresetCleanup();
  return FALSE;
}

void CMPQConcertPresetPanel::CMPQConcertPresetCleanup()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  if(m_pListWin)
    delete m_pListWin;
  m_pListWin = NULL;

  if(m_pPresetHeader)
    delete m_pPresetHeader;
  m_pPresetHeader = NULL;

  if(m_pPresetEdit)
    delete m_pPresetEdit;
  m_pPresetEdit = NULL;

  if(m_pLevelHeader)
    delete m_pLevelHeader;
  m_pLevelHeader = NULL;

  if(m_pLevelEdit)
    delete m_pLevelEdit;
  m_pLevelEdit = NULL;
  
  if(m_pNumLevelsEdit)
    delete m_pNumLevelsEdit;
  m_pNumLevelsEdit = NULL;
  
  if(m_pNumLevelsHeader)
    delete m_pNumLevelsHeader;
  m_pNumLevelsHeader = NULL;

  if(m_pCommit)
    delete m_pCommit;
  m_pCommit = NULL;
  
  if(m_pwszPresetHeader)
    delete m_pwszPresetHeader;
  m_pwszPresetHeader = NULL;

  if(m_pwszLevelHeader)
    delete m_pwszLevelHeader;
  m_pwszLevelHeader = NULL;

  if(m_pwszNumLevelsHeader)
    delete m_pwszNumLevelsHeader;
  m_pwszNumLevelsHeader = NULL;

  if(m_pwszCommit)
    delete m_pwszCommit;
  m_pwszCommit = NULL;

  DBGPRINTF_LOW("MP: end deleting CMPQConcertPresetPanel.");
  /*lint +e1551*/
}

boolean CMPQConcertPresetPanel::Draw(IGraphics *pg)
{
  return m_pListWin->Draw(pg);
}

boolean CMPQConcertPresetPanel::OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
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
         // turn off edit mode
         if((idx == MP_LABEL_PRESET_EDIT) || (idx == MP_LABEL_LEVEL_EDIT) || 
            (idx == MP_LABEL_NUMLEVELS_EDIT))
           ((CMPNumEdit*)pSymbol)->SetEditMode(FALSE);

         // Set prev, if fail then goto last
         if(!m_pListWin->SetPrevious())
           m_pListWin->SetLast();
         idx = m_pListWin->GetItem();

         while((idx != MP_BUTTON_COMMIT) && (idx != MP_LABEL_PRESET_EDIT) &&
               (idx != MP_LABEL_NUMLEVELS_EDIT) && (idx != MP_LABEL_LEVEL_EDIT))
         {
           // Set prev, if fail then goto last
           if(!m_pListWin->SetPrevious())
             m_pListWin->SetLast();
           idx = m_pListWin->GetItem();
         }
         ret = TRUE;
         break;

       case AVK_DOWN:
         // turn off edit mode
         if(MP_BUTTON_COMMIT != idx)
           ((CMPNumEdit*)pSymbol)->SetEditMode(FALSE);

         // Set next, if fail then goto first
         if(!m_pListWin->SetNext())
           m_pListWin->SetFirst();
         idx = m_pListWin->GetItem();

         while((idx != MP_BUTTON_COMMIT) && (idx != MP_LABEL_PRESET_EDIT) &&
               (idx != MP_LABEL_NUMLEVELS_EDIT) && (idx != MP_LABEL_LEVEL_EDIT))
         {
           // Set next, if fail then goto first
           if(!m_pListWin->SetNext())
             m_pListWin->SetFirst();
           idx = m_pListWin->GetItem();
         }
         ret = TRUE;
         break;

       case AVK_LEFT:
         ret = pSymbol->OnEvent(eCode, wParam, dwParam);
         if(idx == MP_LABEL_PRESET_EDIT)
         {
           char labelText[64];
           short curPreset;
            if(m_pwszPresetHeader)
              delete m_pwszPresetHeader;
           curPreset = m_pPresetEdit->GetValue();

           STRLCPY(labelText, " QConcert Presets: ", sizeof(labelText));
           STRLCAT(labelText, PresetLabels[curPreset], sizeof(labelText));
           m_pwszPresetHeader = MP_ToAEEChar(labelText);
           m_pPresetHeader->SetName(m_pwszPresetHeader);
         }
         else if(idx == MP_LABEL_NUMLEVELS_EDIT)
         {
           short numLevels;
           short curLevel;
           numLevels = m_pNumLevelsEdit->GetValue();
           curLevel = m_pLevelEdit->GetValue();
           if(curLevel > numLevels)
             m_pLevelEdit->SetValue(numLevels);
           m_pLevelEdit->SetRange(1, numLevels);
         }
        break;

       case AVK_RIGHT:
         ret = pSymbol->OnEvent(eCode, wParam, dwParam);
         if(idx == MP_LABEL_PRESET_EDIT)
         {
           char labelText[64];
           short curPreset;
           if(m_pwszPresetHeader)
              delete m_pwszPresetHeader;
           curPreset = m_pPresetEdit->GetValue();

           STRLCPY(labelText, " QConcert Presets: ", sizeof(labelText));
           STRLCAT(labelText, PresetLabels[curPreset], sizeof(labelText));
           m_pwszPresetHeader = MP_ToAEEChar(labelText);
           if (m_pwszPresetHeader)
              m_pPresetHeader->SetName(m_pwszPresetHeader);
         }
         else if(idx == MP_LABEL_NUMLEVELS_EDIT)
         {
           short numLevels;
           short curLevel;
           numLevels = m_pNumLevelsEdit->GetValue();
           curLevel = m_pLevelEdit->GetValue();
           if(curLevel > numLevels)
             m_pLevelEdit->SetValue(numLevels);
           m_pLevelEdit->SetRange(1, numLevels);
         }
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

  if((idx == MP_LABEL_PRESET_EDIT) || (idx == MP_LABEL_LEVEL_EDIT))
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
void CMPQConcertPresetPanel::Commit()
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
    pSetting->m_AudioExpanderData.curPreset = m_pPresetEdit->GetValue();
    pSetting->m_AudioExpanderData.curLevel = m_pLevelEdit->GetValue();
    pSetting->m_AudioExpanderData.numLevels = m_pNumLevelsEdit->GetValue();

    (void)MPQConcertSetNumLevels(pSetting, pSetting->m_AudioExpanderData.numLevels);
    (void)MPQConcertSetPreset(pSetting, pSetting->m_AudioExpanderData.curPreset);
    (void)MPQConcertSetLevel(pSetting, pSetting->m_AudioExpanderData.curLevel);
#endif
  }
}

#endif // FEATURE_MP_EQ_UI
