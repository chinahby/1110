/*=============================================================================
  FILE: MPDlg.cpp

  SERVICES: CMPDlg class implementation
 
  GENERAL DESCRIPTION: 

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPDlg.cpp#2 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPDlg.h"

typedef struct _BT_NAME
{
  unsigned int   nBtType;
  AECHAR*        pszBtname;
} MP_BtName;

MP_BtName  BtName[] = {
  {MP_DLG_OK,        (AECHAR*)L"Ok"      }      
  ,{MP_DLG_YES,      (AECHAR*)L"Yes"     }
  ,{MP_DLG_NO,       (AECHAR*)L"No"      } 
  ,{MP_DLG_CANCEL,   (AECHAR*)L"Cancel"  }
};

//static ButtonHandlerT buttonHandler[MP_DLG_MAX];

CMPDlg* MPDlgCreate(void* m_pOwner, IShell* pShell, IDisplay* pDisplay, AEERect* rc)
{
  return (CMPDlg*)CMPDlg::Create(m_pOwner, pShell, pDisplay, rc);
}
void MPDlgDelete(CMPDlg* pObj)
{
  if(NULL != pObj)
  {
    pObj->Destroy();
  }
}
boolean MPDlgIsDismiss(CMPDlg* pObj)
{
  if(pObj)
  {
    return pObj->IsDismiss();
  }
  return FALSE;
}
boolean MPDlgUpdate(CMPDlg* pObj, IGraphics* pg)
{
  if(NULL != pObj)
  {
    return pObj->Draw(pg);
  }
  return FALSE;
}
boolean MPDlgHandleEvent(CMPDlg* pObj, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  if(NULL != pObj)
  {
    return pObj->OnEvent(eCode, wParam, dwParam);
  }
  return FALSE;
}

boolean MPDlgSetText(CMPDlg* pObj, const char* pstrTitle, const char* pstrText)
{
  if(NULL != pObj)
  {
    return pObj->SetText(pstrTitle, pstrText);
  }
  return FALSE;
}
boolean MPDlgAddButton(CMPDlg* pObj, MP_DLG_BUTTON_TYPE type, ButtonHandlerT pfunc)
{
  if(NULL != pObj || NULL != pfunc)
  {
    if(pObj->AddButton(type))
    {
      return pObj->RegisterButtonhandler(pfunc, type);
    }
  }
  return FALSE;
}

boolean MPDlgSetButtonFocus(CMPDlg* pObj, MP_DLG_BUTTON_TYPE type)
{
  if(NULL != pObj)
  {
    return pObj->SetButtonFocus(type);
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------
CMPDlg::CMPDlg():m_bObjStatus(FALSE),m_bDismiss(FALSE),m_pTextPan(NULL)
{
}
CMPDlg::CMPDlg(void* pOwner, IShell* pShell, IDisplay* pDisplay, AEERect* rc):m_bDismiss(FALSE), m_pTextPan(NULL)
{
  int i;
  m_pOwner = pOwner;

  SetOwnerPtr(this);
  m_MPInfo.m_pShell = pShell;
  m_MPInfo.m_pDisplay = pDisplay;

  for(i=0; i < MP_BUTTON_COUNT; i++)
  {
    m_pBt[i] = NULL;
  }

  m_bObjStatus = Init(rc);
}
CMPDlg::~CMPDlg()
{
  int i;
  if(m_pTextPan)
  {
    delete m_pTextPan;
    m_pTextPan = NULL;
  }

  for(i=0; i < MP_BUTTON_COUNT; i++)
  {
    if(m_pBt[i] != NULL)
    {
      delete m_pBt[i];
      m_pBt[i] = NULL;
    }
  }
}

CMPDlg* CMPDlg::Create(void* pOwner, IShell* pShell, IDisplay* pDisplay, AEERect* rc)
{
  CMPDlg* dlg;
  
  dlg = new CMPDlg(pOwner, pShell, pDisplay, rc);
  if(dlg && dlg->GetCreationStatus() == TRUE)
  {
    DBGPRINTF_MED("MP: CMPDlg created successfully.");
    return dlg;
  }
  // Unable to create object
  DBGPRINTF_ERROR("MP: Unable to create CMPDlg object.");

  if(dlg)
    delete dlg;
  dlg = NULL;
  return NULL;
}

boolean CMPDlg::RegisterButtonhandler(ButtonHandlerT pfunc, MP_DLG_BUTTON_TYPE type)
{
  if(type < MP_DLG_MAX)
  {
    buttonHandler[type] = pfunc;
    return TRUE;
  }
  else
  {
    DBGPRINTF_ERROR("MP: failed to register button handler type = %d.", type);
  }
  return FALSE;
}

boolean CMPDlg::GetCreationStatus()
{
  return m_bObjStatus;
}
void CMPDlg::Destroy()
{
  delete this;
}

boolean CMPDlg::Init(AEERect* rc)
{
  int charHeight;
  IDisplay* pDisplay;
  MPDeviceInfo* pDeviceInfo;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
  {
    DBGPRINTF_ERROR("MP: pDeviceInfo is NULL");
    return FALSE;
  }

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
  {
    DBGPRINTF_ERROR("MP: m_pDisplay is NULL");
    return FALSE;
  }

  SetFrameRect(rc->x, rc->y, rc->dx, rc->dy);
  // set colors
  GetBkColor()->SetColor(212, 255, 255);
  GetFgColor()->SetColor(0, 212, 212);
  GetSdColor()->SetColor(0, 150, 150);
  GetFlColor()->SetColor(200, 200, 200);

  //Create text panel
  m_pTextPan = new CMPTextPanel(GetOwnerPtr());

  //calculate button height
  charHeight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;

  // offset and set rect
  m_pTextPan->SetRect(rc->x + 10, rc->y + 5, rc->dx - 20, rc->dy - charHeight - 15);
  m_pTextPan->GetBkColor()->SetColor(212, 255, 255);
  m_pTextPan->GetFlColor()->SetColor(0, 212, 212);
  m_pTextPan->GetFgColor()->SetColor(0, 150, 150);
  m_pTextPan->SetTextColor(0, 0, 0);

  SetFocus(TRUE);
  return TRUE;
}

boolean CMPDlg::IsDismiss()
{
  return m_bDismiss;
}

boolean CMPDlg::Draw(IGraphics *pg)
{
  int i;
  AEERect* paeeRc;
  CMPRect  rc;
  IDisplay* pDisplay;
  boolean fillmode;
  MPDeviceInfo* pDeviceInfo;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
  {
    DBGPRINTF_ERROR("MP: pDeviceInfo is NULL");
    return FALSE;
  }

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
  {
    DBGPRINTF_ERROR("MP: m_pDisplay is NULL");
    return FALSE;
  }

  (void)GetFrameRect(&rc);
  paeeRc = rc.GetRect();

  fillmode = IGRAPHICS_SetFillMode(pg, TRUE);
  (void)IDISPLAY_DrawFrame(pDisplay, paeeRc, AEE_FT_RAISED, GetFlColor()->GetRGB());

  //IDISPLAY_SetColor(pDisplay, CLR_USER_BACKGROUND, GetBkColor()->GetRGB());
  (void)m_pTextPan->Draw(pg);

  for(i=0; i < MP_BUTTON_COUNT; i++)
  {
    if(m_pBt[i])
    {
      m_pBt[i]->Draw(pg);
    }
  }

  (void)IGRAPHICS_SetFillMode(pg, fillmode);
  return TRUE;
}

int CMPDlg::GetStringWidth(const AECHAR* pwstr)
{
  int width = 0;
  int defaultWidth = 0;
  IDisplay* pDisplay;
  MPDeviceInfo* pDeviceInfo;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
  {
    DBGPRINTF_ERROR("MP: pDeviceInfo is NULL");
    return 0;
  }

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
  {
    DBGPRINTF_ERROR("MP: m_pDisplay is NULL");
    return 0;
  }

  if(pwstr)
  {
    width = IDISPLAY_MeasureText(pDisplay, AEE_FONT_NORMAL, pwstr);
    defaultWidth = IDISPLAY_MeasureText(pDisplay, AEE_FONT_NORMAL, (AECHAR*)L"Cancel");
    if(width < defaultWidth)
    {
      width = defaultWidth;
    }
  }
  return width;
}

boolean CMPDlg::SetText(const char* pstrTitle, const char* pstrText)
{
  return m_pTextPan->SetText(pstrTitle, pstrText);
}

boolean CMPDlg::AddButton(MP_DLG_BUTTON_TYPE type)
{
  int i;
  int width = 0;
  int charHeight = 0;
  CMPLabel* pbt;
  IDisplay* pDisplay;
  MPDeviceInfo* pDeviceInfo;

  if(type >= MP_BUTTON_COUNT)
    return FALSE;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
  {
    DBGPRINTF_ERROR("MP: pDeviceInfo is NULL");
    return FALSE;
  }

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
  {
    DBGPRINTF_ERROR("MP: m_pDisplay is NULL");
    return FALSE;
  }

  for(i=0; i < MP_BUTTON_COUNT; i++)
  {
    if(NULL == m_pBt[i])
    {
      pbt = new CMPLabel(GetOwnerPtr());
      if(NULL == pbt)
      {
        DBGPRINTF_ERROR("MP: error on creating CMPLabel type = %d.", (int)type);
        return FALSE;
      }

      pbt->SetID(i);
      pbt->SetData(type);
      pbt->SetName((AECHAR*)GetButtonName(type));

      pbt->GetBkColor()->SetColor(153, 204, 204);
      pbt->GetFgColor()->SetColor(0, 212, 212);
      pbt->GetSdColor()->SetColor(0, 150, 150);
      pbt->GetTextColor()->SetColor(0, 0, 0);

      charHeight = IDISPLAY_GetFontMetrics(pDisplay, AEE_FONT_NORMAL, NULL, NULL) + 4;
      width = GetStringWidth(pbt->GetName());
      if(width == 0)
      {
        delete pbt;
        DBGPRINTF_ERROR("MP: failed to retrieve button width.");
        return FALSE;
      }

      width += 10;
      pbt->SetFrameRect(0, 0, width, charHeight);
      pbt->SetAllignText(IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER);

      if(i == 0)
      {
        pbt->SetFocus(TRUE);
      }
      LayoutButton(pbt);
      m_pBt[i] = pbt;
      return TRUE;
    }
  }

  return FALSE;
}

// This function place a button on the bottom right of frame rect next to the 
// previous button. the order is from right to left
void CMPDlg::LayoutButton(CMPLabel* pBt)
{
  int i;
  AEERect* paeeRc;
  CMPRect  rc;
  CMPRect  rcBt;

  if(pBt == NULL)
  {
    DBGPRINTF_ERROR("MP: NULL ptr.");
    return;
  }

  (void)GetFrameRect(&rc);
  paeeRc = rc.GetRect();
  if(pBt)
  {
    i = pBt->GetID();
  }

  pBt->GetFrameRect(&rcBt);
  if(i == 0)
  {
    // First button
    rcBt.GetRect()->x = (paeeRc->x + paeeRc->dx) - rcBt.GetRect()->dx - 10; 
    rcBt.GetRect()->y = (paeeRc->y + paeeRc->dy) - rcBt.GetRect()->dy - 5;
    pBt->SetFrameRect(rcBt);
  }
  else if (i > 0 && i < MP_BUTTON_COUNT)
  {
    CMPLabel* pPrevBt = m_pBt[i - 1];
    CMPRect   rcBtPrev;
    AEERect*  paeeRcPrev;
    if(pPrevBt)
    {
      pPrevBt->GetFrameRect(&rcBtPrev);
      paeeRcPrev = rcBtPrev.GetRect();

      rcBt.GetRect()->x = paeeRcPrev->x  - rcBt.GetRect()->dx; 
      rcBt.GetRect()->y = (paeeRc->y + paeeRc->dy) - rcBt.GetRect()->dy - 5;
      pBt->SetFrameRect(rcBt);
    }
  }
  else
  {
    DBGPRINTF_ERROR("MP: invalid button id.");
  }
}

const AECHAR* CMPDlg::GetButtonName(MP_DLG_BUTTON_TYPE type)
{
  int i;

  for(i=0; i < MP_BUTTON_COUNT; i++)
  {
    if(type == BtName[i].nBtType)
    {
      return BtName[i].pszBtname;
    }
  }

  DBGPRINTF_ERROR("MP: unable to find button name with type = %d.", type);
  return NULL;
}

CMPLabel* CMPDlg::GetButton(MP_DLG_BUTTON_TYPE type)
{
  int i;
  CMPLabel* pBt;
  for (i = 0; i < MP_BUTTON_COUNT; i++)
  {
    pBt = m_pBt[i];
    if(pBt->GetData() == type)
    {
      return pBt;
    }
  }

  DBGPRINTF_ERROR("MP: unable to find button with type = %d.", type);
  return NULL;
}

boolean CMPDlg::SetButtonFocus(MP_DLG_BUTTON_TYPE type)
{
  int i;
  CMPLabel* pBt;

  if(NULL == (pBt = GetButton(type)))
  {
    DBGPRINTF_ERROR("MP: failed to get button.");
    return FALSE;
  }

  for (i = 0; i < MP_BUTTON_COUNT; i++)
  {
    if(pBt == m_pBt[i])
    {
      // set focus to button
      pBt->SetFocus(TRUE);
    }
    else
    {
      // reset rest of buttons
      if(NULL != m_pBt[i])
      {
        m_pBt[i]->SetFocus(FALSE);
      }
    }
  }
  return TRUE;  
}

// event handler, 
// Return TRUE is handled. Otherwise, return FALSE
boolean CMPDlg::OnEvent(AEEEvent eCode, uint16 wParam, uint32 /*dwParam*/)
{
  boolean bRet = FALSE;

  if(!GetFocus())
    return FALSE;
   
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
          bRet = TRUE; 
          break;
        case AVK_LEFT:
          OnLeft();
          bRet = TRUE;
          break;
        case AVK_RIGHT:
          OnRight();
          bRet = TRUE;
          break;
        case AVK_CLR:
         m_bDismiss = TRUE;
         return FALSE;  
       default:
         break;
      }
    break;

    case EVT_KEY:
      if(AVK_CLR == wParam)
      {
        m_bDismiss = TRUE;
        return FALSE;  
      }
      else
        return TRUE;// return TRUE for all others
  default:
    break;
  }
  return bRet;
}

// This function will find the button with focus and call the 
// callback function ptr attached.
void CMPDlg::OnSelect()
{
  int i;
  CMPLabel* pBt;
  MP_DLG_BUTTON_TYPE type;

  for (i = 0; i < MP_BUTTON_COUNT; i++)
  {
    pBt = m_pBt[i];
    if(pBt && pBt->GetFocus())
    {
      type = (MP_DLG_BUTTON_TYPE)pBt->GetData();
      if(type < MP_DLG_MAX)
      {
        buttonHandler[type](m_pOwner);
      }
      break;
    }
  }
}

// This function set focus to the button on left side,
// If the current button is left most, focus will be unchanged
void CMPDlg::OnLeft()
{
  int i;
  CMPLabel* pBt;

  for (i = 0; i < MP_BUTTON_COUNT; i++)
  {
    pBt = m_pBt[i];
    if(pBt && pBt->GetFocus())
    {
      if((i+1) < MP_BUTTON_COUNT)
      {
        if(m_pBt[i+1] != NULL)
        {
          m_pBt[i+1]->SetFocus(TRUE);
          m_pBt[i]->SetFocus(FALSE);
        }
      }
      break;
    }
  }
}

// This function set focus to the button on right side,
// If the current button is right most, focus will be unchanged
void CMPDlg::OnRight()
{
  int i;
  CMPLabel* pBt;

  for (i = 0; i < MP_BUTTON_COUNT; i++)
  {
    pBt = m_pBt[i];
    if(pBt && pBt->GetFocus())
    {
      if((i-1) >= 0)
      {
        if(m_pBt[i-1] != NULL)
        {
          m_pBt[i-1]->SetFocus(TRUE);
          m_pBt[i]->SetFocus(FALSE);
        }
      }
      break;
    }
  }
}


