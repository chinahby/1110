/*=============================================================================
  FILE: MPSymbols.cpp

  SERVICES: CMPSymbol and other base object class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPSymbols.cpp#22 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPSymbols.h"
#include "AEEVCodes.h"
#include "AEEStdLib.h"

#include <string.h>
#include <stdio.h>

#define MP_DEFAULT_BUFFER_SIZE    10
#define NUM_EDIT_MAX_LEN          9


AECHAR* MP_ToAEEChar(const char* pName)
{
  AECHAR * pstr = NULL;
  if(pName)
  {
    pstr = (AECHAR *) new AECHAR[(STRLEN(pName)+1)*sizeof(AECHAR)];
    if(pstr)
      (void)STRTOWSTR(pName, pstr, (STRLEN(pName)+1)*sizeof(AECHAR));
  }
  return pstr;
}

boolean MP_DrawFrame(IGraphics* pg, AEERect* pRc, AEEFrameType type, RGBVAL clr)
{
  AEELine  line;
  boolean  ret = TRUE;
  CMPColor color(clr);
  boolean fillmode;
  uint8 r, g, b, alpha;
  uint8 fr, fg, fb, falpha;

  switch(type)
  {
  case AEE_FT_RAISED:

    IGRAPHICS_GetColor(pg, &r, &g, &b, &alpha);
    IGRAPHICS_GetFillColor(pg, &fr, &fg, &fb, &falpha);

    fillmode = IGRAPHICS_SetFillMode(pg, TRUE);
    (void)IGRAPHICS_SetFillColor(pg, color.GetR(), color.GetG(), color.GetB(), 1);
    (void)IGRAPHICS_SetColor(pg, 240,240,240, 1);
    (void)IGRAPHICS_DrawRect(pg, pRc);

    // lower left to upper left
    line.sx = pRc->x;
    line.sy = pRc->y + pRc->dy;
    line.ex = pRc->x;
    line.ey = pRc->y;
    (void)IGRAPHICS_DrawLine(pg, &line);

    // upper left to upper right
    line.sx = pRc->x;
    line.sy = pRc->y;
    line.ex = pRc->x + pRc->dx;
    line.ey = pRc->y;
    (void)IGRAPHICS_DrawLine(pg, &line);

    (void)IGRAPHICS_SetColor(pg, 16,16,16, 1);
    // upper right to lower right
    line.sx = pRc->x + pRc->dx;
    line.sy = pRc->y;
    line.ex = pRc->x + pRc->dx;
    line.ey = pRc->y + pRc->dy;
    (void)IGRAPHICS_DrawLine(pg, &line);

    // upper right to lower right
    line.sx = pRc->x + pRc->dx;
    line.sy = pRc->y + pRc->dy;
    line.ex = pRc->x;
    line.ey = pRc->y + pRc->dy;
    (void)IGRAPHICS_DrawLine(pg, &line);

    (void)IGRAPHICS_SetColor(pg, r, g, b, alpha);
    (void)IGRAPHICS_SetFillColor(pg, fr, fg, fb, falpha);
    (void)IGRAPHICS_SetFillMode(pg, fillmode);
    break;
  case AEE_FT_LOWERED:

    (void)IGRAPHICS_GetColor(pg, &r, &g, &b, &alpha);
    (void)IGRAPHICS_GetFillColor(pg, &fr, &fg, &fb, &falpha);

    fillmode = IGRAPHICS_SetFillMode(pg, TRUE);
    (void)IGRAPHICS_SetFillColor(pg, color.GetR(), color.GetG(), color.GetB(), 1);
    (void)IGRAPHICS_SetColor(pg, 240,240,240, 1);
    (void)IGRAPHICS_DrawRect(pg, pRc);

    (void)IGRAPHICS_SetColor(pg, 16,16,16, 1);
    // lower left to upper left
    line.sx = pRc->x;
    line.sy = pRc->y + pRc->dy;
    line.ex = pRc->x;
    line.ey = pRc->y;
    (void)IGRAPHICS_DrawLine(pg, &line);

    // upper left to upper right
    line.sx = pRc->x;
    line.sy = pRc->y;
    line.ex = pRc->x + pRc->dx;
    line.ey = pRc->y;
    (void)IGRAPHICS_DrawLine(pg, &line);

    (void)IGRAPHICS_SetColor(pg, 200,200,200, 1);
    // upper right to lower right
    line.sx = pRc->x + pRc->dx;
    line.sy = pRc->y;
    line.ex = pRc->x + pRc->dx;
    line.ey = pRc->y + pRc->dy;
    (void)IGRAPHICS_DrawLine(pg, &line);

    // upper right to lower right
    line.sx = pRc->x + pRc->dx;
    line.sy = pRc->y + pRc->dy;
    line.ex = pRc->x;
    line.ey = pRc->y + pRc->dy;
    (void)IGRAPHICS_DrawLine(pg, &line);

    (void)IGRAPHICS_SetColor(pg, r, g, b, alpha);
    (void)IGRAPHICS_SetFillColor(pg, fr, fg, fb, falpha);
    (void)IGRAPHICS_SetFillMode(pg, fillmode);
    break;
  default:
    ret = FALSE;
    break;
  }
  return ret;
}

CMPSymbol::CMPSymbol():
  m_pOwner(NULL), m_nID(-1), m_data(0), m_byIntLevel(0), m_FrameRect(0,0,0,0), 
  m_bkClr(COLOR_WHITE), m_fgClr(COLOR_BLACK), m_fillClr(m_fgClr), m_sdClr(m_fgClr), 
  m_Focus(FALSE), m_bSelect(FALSE) 
{
}
CMPSymbol::CMPSymbol(void* pOwner):
  m_pOwner(pOwner), m_nID(-1), m_data(0), m_byIntLevel(0), m_FrameRect(0,0,0,0), 
  m_bkClr(COLOR_WHITE), m_fgClr(COLOR_BLACK), m_fillClr(m_fgClr), m_sdClr(m_fgClr), 
  m_Focus(FALSE), m_bSelect(FALSE) 
{
}
CMPSymbol::CMPSymbol(void* pOwner, CMPRect& rc):
  m_pOwner(pOwner), m_nID(-1), m_data(0), m_byIntLevel(0), m_FrameRect(rc),
  m_bkClr(COLOR_WHITE), m_fgClr(COLOR_BLACK), m_fillClr(m_fgClr), m_sdClr(m_fgClr), 
  m_Focus(FALSE), m_bSelect(FALSE) 
{
}

CMPSymbol::~CMPSymbol()
{
  m_pOwner = NULL;
}

void CMPSymbol::SetFrameRect(CMPRect& rc)
{
  m_FrameRect = rc;
}

boolean CMPSymbol::GetFrameRect(CMPRect* rc)
{
  return m_FrameRect.Get(rc);
}

void CMPSymbol::SetFrameRect(int x, int y, int dx, int dy)
{
  m_FrameRect.Set(x, y, dx, dy);
}
void CMPSymbol::GetFrameRect(int* x, int* y, int* dx, int* dy)
{
  *x = m_FrameRect.GetRect()->x;
  *y = m_FrameRect.GetRect()->y;
  *dx = m_FrameRect.GetRect()->dx;
  *dy = m_FrameRect.GetRect()->dy;
}

void CMPSymbol::SetPos(int x, int y)
{
  m_FrameRect.SetPos(x, y);  
}
boolean CMPSymbol::GetPos(int* x, int* y)
{
  return m_FrameRect.GetPos(x, y);
}

void CMPSymbol::SetSize(int w, int h)
{
  m_FrameRect.SetSize(w, h);
}

boolean CMPSymbol::GetSize(int* w, int* h)
{
  return m_FrameRect.GetSize(w, h);
}

CMPColor* CMPSymbol::GetBkColor()
{
  return &m_bkClr;
}
CMPColor* CMPSymbol::GetFgColor()
{
  return &m_fgClr;
}
CMPColor* CMPSymbol::GetFlColor()
{
  return &m_fillClr;
}
CMPColor* CMPSymbol::GetSdColor()
{
  return &m_sdClr;
}

void CMPSymbol::SetFocus(boolean bMode)
{
  m_Focus = bMode;
}
boolean CMPSymbol::GetFocus()
{
  return m_Focus;
}

void CMPSymbol::SetSelection(boolean bMode)
{
  m_bSelect = bMode;
}
boolean CMPSymbol::GetSelection()
{
  return m_bSelect;
}

void CMPSymbol::SetOwnerPtr(void* pOwner)
{
  m_pOwner = pOwner;
}
void* CMPSymbol::GetOwnerPtr()
{
  return   m_pOwner;
}
void CMPSymbol::SetID(int16 id)
{
  m_nID = id;
}
int16 CMPSymbol::GetID()
{
  return m_nID;
}

void CMPSymbol::SetIndentLevel(int16 nlevel)
{
  m_byIntLevel = nlevel;
}
int16 CMPSymbol::GetIndentLevel()
{
  return m_byIntLevel;
}

void  CMPSymbol::SetData(int32 data)
{
  m_data = data;
}
int32 CMPSymbol::GetData()
{
  return m_data;
}

//--------------------------------------------------------------
//  CMPLabel
//--------------------------------------------------------------
CMPLabel::CMPLabel(): 
  CMPSymbol(NULL), m_strName(NULL), m_bEnable(TRUE), m_TextClr(COLOR_BLACK), m_nAlignMode(-1)
{
}
CMPLabel::CMPLabel(void* pOwner) : 
  CMPSymbol(pOwner), m_strName(NULL), m_bEnable(TRUE), m_TextClr(COLOR_BLACK), m_nAlignMode(-1)
{
}
CMPLabel::~CMPLabel()
{
  // I am not 100% sure that we don't want to free here
  // but I don't think we do.
  m_strName = NULL;
}

void CMPLabel::Enable(boolean bEnable)
{
  m_bEnable = bEnable;
}
boolean CMPLabel::Enable()
{
  return m_bEnable;
}

void CMPLabel::SetName(AECHAR* name)
{
  m_strName = name; 
}

AECHAR* CMPLabel::GetName()
{
  return m_strName;
}

CMPColor* CMPLabel::GetTextColor()
{
  return &m_TextClr;
}

void CMPLabel::SetAllignText(int nAlignMode)
{
  m_nAlignMode = nAlignMode;
}

boolean CMPLabel::Draw(IGraphics *pg)
{
  boolean rval = TRUE;
  AEERect* pAEERc;
  CMPRect  rc;
  RGBVAL clrText;
  boolean fillmode;
  IDisplay* pDisplay;
  MPDeviceInfo* pDeviceInfo;

  if(!m_bEnable)
    return FALSE;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
    return FALSE;

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return FALSE;

  // save default fill mode
  fillmode = IGRAPHICS_GetFillMode(pg);

  (void)GetFrameRect(&rc);
  pAEERc = rc.GetRect();
  if(GetFocus())
  {  
    // draw raised frame for selected item, filled with bk color
    // note, pAEERc will be modified 2 pixels smaller on each side
    (void)IDISPLAY_DrawFrame(pDisplay, pAEERc, AEE_FT_RAISED, GetBkColor()->GetRGB());
  }
  else
  {
    // resize rc to make it same as one with selection
    rc.SetSize(pAEERc->dx - 4, pAEERc->dy - 4);
    if(GetSelection())
    {
      (void)IGRAPHICS_SetFillMode(pg, TRUE);
      (void)IGRAPHICS_SetFillColor(pg, 64, 128, 128, 1); 
      (void)IGRAPHICS_DrawRect(pg, rc.GetRect());
    }
   // else
   // {
   //   (void)IGRAPHICS_SetFillMode(pg, TRUE);
   //   (void)IGRAPHICS_SetFillColor(pg, GetBkColor()->GetR(), GetBkColor()->GetG(), GetBkColor()->GetB(), 1);
   //   (void)IGRAPHICS_DrawRect(pg, rc.GetRect());
   // }
  }

  if(m_strName)
  {
    // set text color
    clrText = IDISPLAY_SetColor(pDisplay,CLR_USER_TEXT, GetTextColor()->GetRGB());

    if(m_nAlignMode >= 0)
    {
      (void)IDISPLAY_DrawText(pDisplay,
                                     AEE_FONT_NORMAL, 
                                     m_strName,
                                     -1,
                                     pAEERc->x + 1, 
                                     pAEERc->y, 
                                     pAEERc, 
                                     IDF_TEXT_TRANSPARENT | m_nAlignMode);
    }
    else
    {
      (void)IDISPLAY_DrawText(pDisplay,
                                     AEE_FONT_NORMAL, 
                                     m_strName,
                                     -1,
                                     pAEERc->x + 1, 
                                     pAEERc->y, 
                                     pAEERc, 
                                     IDF_TEXT_TRANSPARENT);
    }

    if(clrText != RGB_NONE)
    {
      (void)IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, clrText);
    }
  }

  // set back to default
  (void)IGRAPHICS_SetFillMode(pg, fillmode);
  return rval;
}


// ------------------------------------------------------------------------
//   CMPTextPanel
//--------------------------------------------------------------------------
CMPTextPanel::CMPTextPanel(): 
  CMPSymbol(NULL), m_pIText(NULL), m_pwstrTitle(NULL), m_pwstrText(NULL), m_TextClr(COLOR_BLACK)
{
}
CMPTextPanel::CMPTextPanel(void* pOwner) : 
  CMPSymbol(pOwner), m_pIText(NULL), m_pwstrTitle(NULL), m_pwstrText(NULL), m_TextClr(COLOR_BLACK)
{
}
CMPTextPanel::~CMPTextPanel()
{
  if(m_pwstrTitle)
  {
    delete m_pwstrTitle;
    m_pwstrTitle = NULL;
  }

  if(m_pwstrText)
  {  
    delete m_pwstrText;
    m_pwstrText = NULL;
  }

  if(m_pIText)
  {  
    ISTATIC_Release(m_pIText);
    m_pIText = NULL;
  }
}

boolean CMPTextPanel::SetText(const char* pszTitle, const char* pszText, AEEFont titleFnt, AEEFont textFnt)
{
  MPDeviceInfo* pDeviceInfo;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
    return FALSE;

  if(pszTitle == NULL && pszText == NULL)
    return FALSE;

  if(m_pwstrTitle != NULL)
  {
    delete m_pwstrTitle;
    m_pwstrTitle = NULL;
  }
  if(m_pwstrText != NULL)
  {
    delete m_pwstrText;
    m_pwstrText = NULL;
  }

  m_pwstrTitle = MP_ToAEEChar(pszTitle);
  m_pwstrText  = MP_ToAEEChar(pszText);
  if(m_pwstrTitle || m_pwstrText)
  {
    IShell *pShell;

    pShell = pDeviceInfo->m_pShell;
    if(NULL == m_pIText && pShell)
    {
      if (SUCCESS != ISHELL_CreateInstance(pShell, AEECLSID_STATIC, (void **)&m_pIText))
      {
        DBGPRINTF_ERROR("MP: failed to create AEECLSID_STATIC.");
        return FALSE;
      }
    }
    
    if(!ISTATIC_SetText(m_pIText, m_pwstrTitle, m_pwstrText, titleFnt, textFnt))
    {
      DBGPRINTF_ERROR("MP: ISTATIC_SetText failed.");
      return FALSE;
    }

    return TRUE;
  }
  else
  {
    DBGPRINTF_ERROR("MP: afailed to allocate memory.");
  }

  return FALSE;
}

void CMPTextPanel::SetTextColor(uint8 r, uint8 g, uint8 b)
{
  m_TextClr.SetColor(r, g, b);
}

CMPColor* CMPTextPanel::GetTextColor()
{
  return &m_TextClr;
}

void CMPTextPanel::SetRect(int x, int y, int dx, int dy)
{
  CMPRect       Rc;
  AEERect*      paeRc;
  IShell*       pShell;
  MPDeviceInfo* pDeviceInfo;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
    return;

  (void)SetFrameRect(x, y, dx, dy);
  (void)GetFrameRect(&Rc);
  paeRc = Rc.GetRect();

  pShell = pDeviceInfo->m_pShell;
  if(NULL == m_pIText && pShell)
  {
    if (SUCCESS != ISHELL_CreateInstance(pShell, AEECLSID_STATIC, (void **)&m_pIText))
    {
      DBGPRINTF_ERROR("MP: failed to create AEECLSID_STATIC.");
      return;
    }
  }

  //set size and location of ctrl
  ISTATIC_SetRect(m_pIText, paeRc);
}

boolean CMPTextPanel::Draw(IGraphics *pg)
{
  AEERect* pAEERc;
  CMPRect  rc;
  IDisplay* pDisplay;
  boolean fillmode;
  MPDeviceInfo* pDeviceInfo;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
    return FALSE;

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return FALSE;

  if(NULL == m_pIText)
    return FALSE;

  (void)GetFrameRect(&rc);
  pAEERc = rc.GetRect();


  // save default fill mode
  fillmode = IGRAPHICS_GetFillMode(pg);

  (void)IGRAPHICS_SetFillMode(pg, TRUE);
  (void)IGRAPHICS_SetFillColor(pg, GetBkColor()->GetR(), GetBkColor()->GetG(), GetBkColor()->GetB(), 1); 
  (void)IGRAPHICS_SetColor(pg, GetFgColor()->GetR(), GetFgColor()->GetG(), GetFgColor()->GetB(), 1);
  (void)IGRAPHICS_DrawRect(pg, pAEERc);
  
  (void)ISTATIC_Redraw(m_pIText);

  (void)IGRAPHICS_SetFillMode(pg, fillmode);  
  return TRUE;
}
// ------------------------------------------------------------------------
//   CMPButton
//--------------------------------------------------------------------------

CMPButton::CMPButton()
{
}
CMPButton::CMPButton(void* pOwner):CMPLabel(pOwner)
{
}
CMPButton::~CMPButton()
{
}
boolean CMPButton::Draw(IGraphics *pg)
{
  boolean rval = TRUE;
  AEERect* pAEERc;
  AEELine  line;
  CMPRect  rc;
  AEERect  SelectRc;
  RGBVAL clrText;
  boolean fillmode;
  uint8 r, g, b, alpha;
  
  AEEPolygon  gp;
  AEEPoint    pt[4];

  IDisplay* pDisplay;
  MPDeviceInfo* pDeviceInfo;

  if(!m_bEnable)
    return FALSE;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
    return FALSE;

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return FALSE;

  // save default fill mode
  fillmode = IGRAPHICS_GetFillMode(pg);
  IGRAPHICS_GetColor(pg, &r, &g, &b, &alpha);

  (void)GetFrameRect(&rc);
  pAEERc = rc.GetRect();

  if(GetFocus())
  {  
    // draw raised frame for selected item, filled with bk color
    // note, pAEERc will be modified 2 pixels smaller on each side
    (void)IDISPLAY_DrawFrame(pDisplay, pAEERc, AEE_FT_RAISED, GetBkColor()->GetRGB());
  }
  else
  {
    // resize rc to make it same as one with selection
    rc.SetSize(pAEERc->dx - 4, pAEERc->dy - 4);
  }

  // set draw and fill color same as text color
  (void)IGRAPHICS_SetColor(pg, 
         GetTextColor()->GetR(),
         GetTextColor()->GetG(),
         GetTextColor()->GetB(),
         1);

  SelectRc.x = pAEERc->x + 1;
  SelectRc.y = pAEERc->y + 1;
  SelectRc.dx = pAEERc->dy - 2;
  SelectRc.dy = SelectRc.dx;
 

  pt[0].x = SelectRc.x;
  pt[0].y = SelectRc.y;
  pt[1].x = SelectRc.x + SelectRc.dx/2;
  pt[1].y = SelectRc.y + SelectRc.dy/2;
  pt[2].x = SelectRc.x;
  pt[2].y = SelectRc.y + SelectRc.dy;
  pt[3].x = SelectRc.x;
  pt[3].y = SelectRc.y;

  gp.len = 4;
  gp.points = pt;

 // (void)IGRAPHICS_SetFillMode(pg, TRUE);
 // IGRAPHICS_DrawPolygon(pg,&gp);


    (void)IGRAPHICS_SetFillMode(pg, TRUE);
 //   (void)IGRAPHICS_SetFillColor(pg, color.GetR(), color.GetG(), color.GetB(), 1);
 
    if(GetFocus())
    {
      (void)IGRAPHICS_SetColor(pg, 180, 180, 180, 1);
    }
    else
    {
      (void)IGRAPHICS_SetColor(pg, 180, 180, 180, 1);
    }
    (void)IGRAPHICS_DrawPolygon(pg,&gp);

    if(GetFocus())
    {  
      (void)IGRAPHICS_SetColor(pg, 120, 120, 120, 1);
    }
    else
      (void)IGRAPHICS_SetColor(pg, 180, 180, 180, 1);

    // lower left to upper left
    line.sx = SelectRc.x;
    line.sy = SelectRc.y + SelectRc.dy;
    line.ex = SelectRc.x;
    line.ey = SelectRc.y;
    (void)IGRAPHICS_DrawLine(pg, &line);

    // upper left to mid right
    line.sx = SelectRc.x;
    line.sy = SelectRc.y;
    line.ex = SelectRc.x + SelectRc.dx/2;
    line.ey = SelectRc.y + SelectRc.dy/2;
    (void)IGRAPHICS_DrawLine(pg, &line);

    if(GetFocus())
    {  
      (void)IGRAPHICS_SetColor(pg, 32, 32, 32, 1);
    }
    else
      (void)IGRAPHICS_SetColor(pg, 64, 64, 64, 1);
    
    // mid right to lower left
    line.sx = SelectRc.x + SelectRc.dx/2;
    line.sy = SelectRc.y + SelectRc.dy/2;
    line.ex = SelectRc.x;
    line.ey = SelectRc.y + SelectRc.dy;
    (void)IGRAPHICS_DrawLine(pg, &line);


 // (void)IGRAPHICS_SetFillMode(pg, FALSE);
//  (void)IGRAPHICS_DrawRect(pg, &SelectRc);
 // (void)MP_DrawFrame(pg, &SelectRc, AEE_FT_LOWERED, MAKE_RGB(180, 180, 180));

/*
  if(GetSelection())
  {
    line.sx = SelectRc.x + 2;
    line.sy = SelectRc.y + 2;
    line.ex = SelectRc.x + SelectRc.dx - 4;
    line.ey = SelectRc.y + SelectRc.dy - 4;

    (void)IGRAPHICS_DrawLine(pg, &line);

    line.sx = SelectRc.x + 2;
    line.sy = SelectRc.y + SelectRc.dy - 4;
    line.ex = SelectRc.x + SelectRc.dx - 4;
    line.ey = SelectRc.y + 2;

    (void)IGRAPHICS_DrawLine(pg, &line);
  }
*/
  if(m_strName)
  {
    // set text color
    clrText = IDISPLAY_SetColor(pDisplay,CLR_USER_TEXT, GetTextColor()->GetRGB());

    if(m_nAlignMode >= 0)
    {
      (void)IDISPLAY_DrawText(pDisplay,
                              AEE_FONT_NORMAL, 
                              m_strName,
                              -1,
                              pAEERc->x + pAEERc->dy/2 + 3, // move
                              pAEERc->y, 
                              pAEERc, 
                              IDF_TEXT_TRANSPARENT | m_nAlignMode);
    }
    else
    {
      (void)IDISPLAY_DrawText(pDisplay,
                              AEE_FONT_NORMAL, 
                              m_strName,
                              -1,
                              pAEERc->x + pAEERc->dy/2 + 3, // move
                              pAEERc->y, 
                              pAEERc, 
                              IDF_TEXT_TRANSPARENT);
    }
    if(clrText != RGB_NONE)
    {
      (void)IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, clrText);
    }
  }

  // set back to default
  (void)IGRAPHICS_SetFillMode(pg, fillmode);
  (void)IGRAPHICS_SetColor(pg, r, g, b, alpha);
  return rval;
}

// ------------------------------------------------------------------------
//   CMPRadioButton
//--------------------------------------------------------------------------
CMPRadioButton::CMPRadioButton()
{
}
CMPRadioButton::CMPRadioButton(void* pOwner):CMPLabel(pOwner)
{
}
CMPRadioButton::~CMPRadioButton()
{
}

boolean CMPRadioButton::Draw(IGraphics *pg)
{
  boolean rval = TRUE;
  AEERect* pAEERc;
  CMPRect  rc;
  AEECircle circle;
  RGBVAL clrText;
  boolean fillmode;
  uint8 r, g, b, alpha;
  IDisplay* pDisplay;
  MPDeviceInfo* pDeviceInfo;

  if(!m_bEnable)
    return FALSE;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
    return FALSE;

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return FALSE;

  // save default fill mode
  fillmode = IGRAPHICS_GetFillMode(pg);
  IGRAPHICS_GetColor(pg, &r, &g, &b, &alpha);

  (void)GetFrameRect(&rc);
  pAEERc = rc.GetRect();

  if(GetFocus())
  {  
    // draw raised frame for selected item, filled with bk color
    // note, pAEERc will be modified 2 pixels smaller on each side
    (void)IDISPLAY_DrawFrame(pDisplay, pAEERc, AEE_FT_RAISED, GetBkColor()->GetRGB());
  }
  else
  {
    // resize rc to make it same as one with selection
    rc.SetSize(pAEERc->dx - 4, pAEERc->dy - 4);
  }

  // set draw and fill color same as text color
  (void)IGRAPHICS_SetColor(pg, 
         GetTextColor()->GetR(),
         GetTextColor()->GetG(),
         GetTextColor()->GetB(),
         1);
  (void)IGRAPHICS_SetFillColor(pg, 
         GetTextColor()->GetR(),
         GetTextColor()->GetG(),
         GetTextColor()->GetB(),
         1);

  circle.cx = pAEERc->x + pAEERc->dy/2 + 1;
  circle.cy = pAEERc->y + pAEERc->dy/2;
  circle.r  = pAEERc->dy/2 - 1;
  if(GetSelection())
  {
    // draw a smaller solid circle for selected item
    AEECircle circle1 = circle;
    circle1.r -= 2;
    (void)IGRAPHICS_SetFillMode(pg, TRUE);
    (void)IGRAPHICS_DrawCircle(pg, &circle1);
  }

  // draw circle
  (void)IGRAPHICS_SetFillMode(pg, FALSE);
  (void)IGRAPHICS_DrawCircle(pg, &circle);

  if(m_strName)
  {
    // set text color
    clrText = IDISPLAY_SetColor(pDisplay,CLR_USER_TEXT, GetTextColor()->GetRGB());
    (void)IDISPLAY_DrawText(pDisplay,
                            AEE_FONT_NORMAL, 
                            m_strName,
                            -1,
                            pAEERc->x + pAEERc->dy + 3, // move
                            pAEERc->y, 
                            pAEERc, 
                            IDF_TEXT_TRANSPARENT);
    if(clrText != RGB_NONE)
      (void)IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, clrText);
  }

  // set back to default
  (void)IGRAPHICS_SetFillMode(pg, fillmode);
  (void)IGRAPHICS_SetColor(pg, r, g, b, alpha);
  return rval;
}

//----------------------------------------------------------------------
//    CheckBoxLabel
//----------------------------------------------------------------------
CMPCheckBox::CMPCheckBox()
{
}
CMPCheckBox::CMPCheckBox(void* pOwner):CMPLabel(pOwner)
{
}
CMPCheckBox::~CMPCheckBox()
{
}

boolean CMPCheckBox::Draw(IGraphics *pg)
{
  boolean rval = TRUE;
  AEERect* pAEERc;
  CMPRect  rc;
  AEERect  SelectRc;
  AEELine  line;
  RGBVAL clrText;
  boolean fillmode;
  uint8 r, g, b, alpha;
  IDisplay* pDisplay;
  MPDeviceInfo* pDeviceInfo;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
    return FALSE;

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return FALSE;

  // save default fill mode
  fillmode = IGRAPHICS_GetFillMode(pg);
  IGRAPHICS_GetColor(pg, &r, &g, &b, &alpha);

  (void)GetFrameRect(&rc);
  pAEERc = rc.GetRect();

  if(GetFocus())
  {  
    // draw raised frame for selected item, filled with bk color
    // note, pAEERc will be modified 2 pixels smaller on each side
    (void)IDISPLAY_DrawFrame(pDisplay, pAEERc, AEE_FT_RAISED, GetBkColor()->GetRGB());
  }
  else
  {
    // resize rc to make it same as one with selection
    rc.SetSize(pAEERc->dx - 4, pAEERc->dy - 4);
  }

  // set draw and fill color same as text color
  (void)IGRAPHICS_SetColor(pg, 
         GetTextColor()->GetR(),
         GetTextColor()->GetG(),
         GetTextColor()->GetB(),
         1);

  SelectRc.x = pAEERc->x + 1;
  SelectRc.y = pAEERc->y + 1;
  SelectRc.dx = pAEERc->dy - 2;
  SelectRc.dy = SelectRc.dx;
 
  (void)IGRAPHICS_SetFillMode(pg, FALSE);
  // draw selection frame only when it is enabled
  if(m_bEnable)
  {
//  (void)IGRAPHICS_DrawRect(pg, &SelectRc);
    (void)MP_DrawFrame(pg, &SelectRc, AEE_FT_LOWERED, MAKE_RGB(212, 212, 212));
  }
 
  // new, shrink rect by 2 pixels.
  SelectRc.x += 2;
  SelectRc.dx -= 3;
  SelectRc.y += 2;
  SelectRc.dy -= 4;
  if(GetSelection())
  {
    // left mid to (bottom 1/3-1)
    line.sx = SelectRc.x;
    line.sy = SelectRc.y + SelectRc.dy/2;
    line.ex = SelectRc.x + SelectRc.dx/3;
    line.ey = SelectRc.y + SelectRc.dy - 1;
    (void)IGRAPHICS_DrawLine(pg, &line);
    
    // left (mid=1) to bottom 1/3
    line.sx = SelectRc.x;
    line.sy = SelectRc.y + SelectRc.dy/2 + 1;
    line.ex = SelectRc.x + SelectRc.dx/3;
    line.ey = SelectRc.y + SelectRc.dy;
    (void)IGRAPHICS_DrawLine(pg, &line);
 
    // (bottom 1/3-1) to right top
    line.sx = SelectRc.x + SelectRc.dx/3;
    line.sy = SelectRc.y + SelectRc.dy - 1;
    line.ex = SelectRc.x + SelectRc.dx;
    line.ey = SelectRc.y;
    (void)IGRAPHICS_DrawLine(pg, &line);
 
    // bottom 1/3 to (right top +1)
    line.sx = SelectRc.x + SelectRc.dx/3;
    line.sy = SelectRc.y + SelectRc.dy;
    line.ex = SelectRc.x + SelectRc.dx;
    line.ey = SelectRc.y + 1;
    (void)IGRAPHICS_DrawLine(pg, &line);
  }

  if(m_strName)
  {
    // set text color
    clrText = IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, GetTextColor()->GetRGB());
    (void)IDISPLAY_DrawText(pDisplay,
                            AEE_FONT_NORMAL, 
                            m_strName,
                            -1,
                            pAEERc->x + pAEERc->dy + 3, // move
                            pAEERc->y, 
                            pAEERc, 
                            IDF_TEXT_TRANSPARENT);
    if(clrText != RGB_NONE)
      (void)IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, clrText);
  }

  // set back to default
  (void)IGRAPHICS_SetFillMode(pg, fillmode);
  (void)IGRAPHICS_SetColor(pg, r, g, b, alpha);
  return rval;
}
//----------------------------------------------------------------------
//    CMPSrollBar
//----------------------------------------------------------------------
CMPSrollBar::CMPSrollBar()
{
}
CMPSrollBar::CMPSrollBar(void* pOwner):CMPLabel(pOwner)
{
}
CMPSrollBar::~CMPSrollBar()
{
}
boolean CMPSrollBar::Draw(IGraphics *pg)
{
  CMPRect  FrameRc; 
  AEERect* pAEERc;
  
  AEEPolygon  gp;
  AEEPoint    pt[4];
  IDisplay*   pDisplay;
  MPDeviceInfo* pDeviceInfo;

  if(!m_bEnable)
    return FALSE;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
    return FALSE;

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return FALSE;

  (void)GetFrameRect(&FrameRc);
  pAEERc = FrameRc.GetRect();

  (void)IGRAPHICS_SetFillMode(pg, FALSE);

  (void)IDISPLAY_DrawFrame(pDisplay, pAEERc, AEE_FT_BOX, GetBkColor()->GetRGB());

  gp.len = 4;
  gp.points = pt;
  (void)IGRAPHICS_SetColor(pg, 0, 0, 0, 1);
  (void)IGRAPHICS_SetFillColor(pg, 0, 0, 0, 1);
  (void)IGRAPHICS_SetFillMode(pg, TRUE);

  pt[0].x = pAEERc->x;
  pt[0].y = pAEERc->y + pAEERc->dy/3;
  pt[1].x = pAEERc->x + pAEERc->dx/2;
  pt[1].y = pAEERc->y;
  pt[2].x = pAEERc->x + pAEERc->dx;
  pt[2].y = pAEERc->y + pAEERc->dy/3;
  pt[3].x = pAEERc->x;
  pt[3].y = pAEERc->y + pAEERc->dy/3;

  (void)IGRAPHICS_DrawPolygon(pg,&gp);

  pt[0].x = pAEERc->x;
  pt[0].y = pAEERc->y + pAEERc->dy - pAEERc->dy/3;
  pt[1].x = pAEERc->x + pAEERc->dx;
  pt[1].y = pAEERc->y + pAEERc->dy - pAEERc->dy/3;
  pt[2].x = pAEERc->x + pAEERc->dx/2;
  pt[2].y = pAEERc->y + pAEERc->dy;
  pt[3].x = pAEERc->x;
  pt[3].y = pAEERc->y + pAEERc->dy - pAEERc->dy/3;

  (void)IGRAPHICS_DrawPolygon(pg,&gp);
  return TRUE;
}

//----------------------------------------------------------------------
//    CMPNumEdit
//----------------------------------------------------------------------

CMPNumEdit::CMPNumEdit():m_clrBkEdit(COLOR_WHITE), 
                   m_nMaxLength(NUM_EDIT_MAX_LEN), m_nCurrentPos(0), 
                   m_bEdit(FALSE), m_strLabelName(NULL), 
                   m_nValue(0), m_nRectOffset(0), m_nRectWidth(0), m_bDirty(FALSE),
                   m_nSavedData(0), m_previousEvent(0), m_nStep(1)
{
  m_strName = (AECHAR *) new AECHAR[m_nMaxLength];
  memset(m_strName, 0, m_nMaxLength*sizeof(AECHAR));

  m_nMax = 2147483647;      //Maximum (signed) int32 value 
  m_nMin = -2147483647;     //Minimum (signed) int32 value

}

CMPNumEdit::CMPNumEdit(void* pOwner):CMPLabel(pOwner), m_clrBkEdit(COLOR_WHITE), 
                              m_nMaxLength(NUM_EDIT_MAX_LEN), m_nCurrentPos(0), m_bEdit(FALSE),
                              m_strLabelName(NULL), 
                              m_nValue(0), m_nRectOffset(0), m_nRectWidth(0), m_bDirty(FALSE),
                              m_nSavedData(0), m_previousEvent(0), m_nStep(1)
{
  m_strName = (AECHAR *) new AECHAR[m_nMaxLength];
  memset(m_strName, 0, m_nMaxLength*sizeof(AECHAR));

  m_nMax = 2147483647;      //Maximum (signed) int32 value 
  m_nMin = -2147483647;     //Minimum (signed) int32 value
}

CMPNumEdit::~CMPNumEdit()
{
  /*lint -e1551*/ /*Suppressing Lint Warning: Function may throw exception '...' in destructor*/
  if(m_strName)
    delete[] m_strName;
  if(m_strLabelName)
    delete[] m_strLabelName;
  /*lint +e1551*/
}

void CMPNumEdit::SetLabelName(const char* strName)
{
  if(NULL == strName)
    return;

  if(m_strLabelName)
  {
    delete[] m_strLabelName;
  }
  m_strLabelName = MP_ToAEEChar(strName);
}

const AECHAR* CMPNumEdit::GetLabelName()
{
  return m_strLabelName;
}

void CMPNumEdit::SetValue(int nValue)
{
  char buf[NUM_EDIT_MAX_LEN];

  m_nValue = nValue;
  (void)SNPRINTF(buf, sizeof(buf), "%d", m_nValue);
  (void)STRTOWSTR(buf, m_strName, m_nMaxLength*sizeof(AECHAR));
}

int CMPNumEdit::GetValue()
{
  return m_nValue;
}

void CMPNumEdit::SetDirty(boolean bMode)
{
  m_bDirty = bMode;
}

boolean CMPNumEdit::GetDirty()
{
  return m_bDirty;
}

void CMPNumEdit::SetRectOffset(int nOffset)
{
  m_nRectOffset = nOffset;
}

int  CMPNumEdit::GetRectOffset()
{
  return m_nRectOffset;
}

void CMPNumEdit::SetWidth(int nWidth)
{
  m_nRectWidth = nWidth;
}

boolean CMPNumEdit::Draw(IGraphics *pg)
{
  boolean rval = TRUE;
  AEERect* pAEERc;
  CMPRect  rc;
  CMPRect  textRc;
  RGBVAL   clrText;
  boolean  fillmode;
  uint8 r, g, b, alpha;
  IDisplay* pDisplay;
  MPDeviceInfo* pDeviceInfo;

  if(!m_bEnable)
    return FALSE;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
    return FALSE;

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return FALSE;

  // high light bar color
  CMPColor HLTClr(110, 204, 204);

  // save default fill mode
  fillmode = IGRAPHICS_GetFillMode(pg);
  IGRAPHICS_GetColor(pg, &r, &g, &b, &alpha);

  (void)GetFrameRect(&textRc);
  pAEERc = textRc.GetRect();
  pAEERc->dx = m_nRectOffset;
  pAEERc->x += 2;
  pAEERc->dx -= 2;

  // set text color to black
  (void)IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, COLOR_BLACK);
  if(m_strLabelName)
  {
    // display label name
    (void)IDISPLAY_DrawText(pDisplay,
                            AEE_FONT_NORMAL, 
                            m_strLabelName,
                            -1,
                            pAEERc->x, 
                            pAEERc->y, 
                            pAEERc, 
                            IDF_TEXT_TRANSPARENT);
  }

  (void)GetFrameRect(&rc);
  pAEERc = rc.GetRect();
  pAEERc->x  += (m_nRectOffset + 2);
  pAEERc->dx -= (m_nRectOffset + 4);

  textRc = rc;
  pAEERc = textRc.GetRect();
  pAEERc->x  += 12;
  pAEERc->dx -= 24;

  if((GetFocus() && !m_bEdit))
  {
    (void)IGRAPHICS_SetFillMode(pg, TRUE);
    // draw frame on focused edit area
    (void)IDISPLAY_DrawFrame(pDisplay, pAEERc, AEE_FT_NONE, HLTClr.GetRGB());
  }
  else if((GetFocus() && m_bEdit))
  {
    (void)IGRAPHICS_SetFillMode(pg, FALSE);
    // draw frame on focused edit area with eidt white bk color
    (void)IDISPLAY_DrawFrame(pDisplay, pAEERc, AEE_FT_BOX, COLOR_WHITE);
  }

  if(m_strName)
  {
    // set text color
    clrText = IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, GetTextColor()->GetRGB());
    (void)IDISPLAY_DrawText(pDisplay,
                            AEE_FONT_NORMAL, 
                            m_strName,
                            -1,
                            pAEERc->x, // move
                            pAEERc->y, 
                            pAEERc, 
                            IDF_TEXT_TRANSPARENT);
    if(clrText != RGB_NONE)
      (void)IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, clrText);
  }

  if(m_bEdit)
  {
    DrawCursor(pg, pAEERc);
  }

  if(GetFocus())
  {
    DrawTickBar(pg, rc);
  }
 
  // set back to default
  (void)IGRAPHICS_SetFillMode(pg, fillmode);
  (void)IGRAPHICS_SetColor(pg, r, g, b, alpha);

  return rval;
}

void CMPNumEdit::DrawTickBar(IGraphics *pg, CMPRect& rc)
{
  CMPRect  FrameRc; 
  AEERect* pAEERc;
  
  AEEPolygon  gp;
  AEEPoint    pt[4];
  IDisplay*   pDisplay;
  int         nTriangleWidth;
  int         nHeight;
  int         nCanter;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;
 
  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return;

  (void)IGRAPHICS_SetFillColor(pg, 64, 64, 64, 1);

  rc.Get(&FrameRc);
  pAEERc = FrameRc.GetRect();
  nTriangleWidth = (pAEERc->dy * 2)/3;
  nCanter = pAEERc->y + pAEERc->dy/2;
  nHeight = pAEERc->dy/2 - 2;
  
  if(nHeight <= 0)
  {
    nHeight = pAEERc->dy/2;	   
  }


  pAEERc->dx = nTriangleWidth;
  (void)IGRAPHICS_SetFillMode(pg, FALSE);

  gp.len = 4;
  gp.points = pt;

  // draw left side of triangle
  (void)IGRAPHICS_SetFillMode(pg, TRUE);
  pt[0].x = pAEERc->x + 2;
  pt[0].y = pAEERc->y + pAEERc->dy/2;

  pt[1].x = pAEERc->x + pAEERc->dx - 2;
  pt[1].y = nCanter - nHeight;

  pt[2].x = pAEERc->x + pAEERc->dx - 2;
  pt[2].y = nCanter + nHeight;

  pt[3].x = pAEERc->x + 2;
  pt[3].y = pAEERc->y + pAEERc->dy/2;

  (void)IGRAPHICS_DrawPolygon(pg,&gp);


  // draw right side of triangle
  rc.Get(&FrameRc);
  pAEERc = FrameRc.GetRect();
  pAEERc->x = pAEERc->x + pAEERc->dx - nTriangleWidth;
  pAEERc->dx = nTriangleWidth;

  pt[0].x = pAEERc->x + 2;
  pt[0].y = nCanter - nHeight;;

  pt[1].x = pAEERc->x + pAEERc->dx - 2;
  pt[1].y = pAEERc->y + pAEERc->dy/2;

  pt[2].x = pAEERc->x + 2;
  pt[2].y = nCanter + nHeight;;

  pt[3].x = pAEERc->x + 2;
  pt[3].y = nCanter - nHeight;

  (void)IGRAPHICS_DrawPolygon(pg,&gp);
}

void CMPNumEdit::DrawCursor(IGraphics *pg, AEERect* pAEERc)
{
  AEELine   line;
  AEELine   lineV;
  int       nPos;
  IDisplay* pDisplay;
  MPDeviceInfo * pDeviceInfo = NULL;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
      return;

  pDisplay = pDeviceInfo->m_pDisplay;

  nPos = IDISPLAY_MeasureTextEx(pDisplay,
                                AEE_FONT_NORMAL, 
                                m_strName, 
                                m_nCurrentPos, 
                                -1, 
                                NULL);

  (void)IGRAPHICS_SetColor(pg, 0, 0, 0, 1);
  // draw vertical line
  lineV.sx = pAEERc->x + nPos;
  lineV.sy = pAEERc->y + 1;
  lineV.ex = pAEERc->x + nPos;
  lineV.ey = pAEERc->y + pAEERc->dy - 2;
  (void)IGRAPHICS_DrawLine(pg, &lineV);

  // top left
  line.sx = lineV.sx - 1;
  line.sy = lineV.sy;
  line.ex = lineV.sx;
  line.ey = lineV.sy;
  (void)IGRAPHICS_DrawLine(pg, &line);

  // top right
  line.sx = lineV.sx + 1;
  line.sy = lineV.sy;
  line.ex = lineV.sx;
  line.ey = lineV.sy;
  (void)IGRAPHICS_DrawLine(pg, &line);

  // bottom left
  line.sx = lineV.sx - 1;
  line.sy = lineV.ey;
  line.ex = lineV.sx;
  line.ey = lineV.ey;
  (void)IGRAPHICS_DrawLine(pg, &line);

  // bottom right
  line.sx = lineV.sx + 1;
  line.sy = lineV.ey;
  line.ex = lineV.sx;
  line.ey = lineV.ey;
  (void)IGRAPHICS_DrawLine(pg, &line);
}

boolean CMPNumEdit::OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  boolean ret = FALSE;
  int     nLength;

  if(!GetFocus())
    return ret;

  switch(eCode)
  {
  case EVT_KEY_PRESS:
    switch(wParam)
    {
     case AVK_UP:
       break;

     case AVK_DOWN:
       break;

     case AVK_LEFT:
       if(!m_bEdit)
       {
         if(m_nValue - m_nStep >= m_nMin)
         {
           SetValue(m_nValue - m_nStep);
           SetDirty(TRUE);
         }
         else if(m_nValue > m_nMin)
         {
           SetValue(m_nMin);
           SetDirty(TRUE);
         }
         else
         {
           SetValue(m_nMax);
           SetDirty(TRUE);
         }
         m_previousEvent = wParam;
       }
       else
       {
         if(m_nCurrentPos > 0)
         {
           m_nCurrentPos--;
         }
       }
       ret = TRUE;
       break;

     case AVK_RIGHT:
       if(!m_bEdit)
       {
         if(m_nValue + m_nStep <= m_nMax)
         {
           SetValue(m_nValue + m_nStep);
           SetDirty(TRUE);
         }
         else if(m_nValue < m_nMax)
         {
           SetValue(m_nMax);
           SetDirty(TRUE);
         }
         else
         {
           SetValue(m_nMin);
           SetDirty(TRUE);
         }
         m_previousEvent = wParam;
       }
       else
       {
         nLength = WSTRLEN(m_strName);
         if(m_nCurrentPos < nLength  && m_nCurrentPos < m_nMaxLength)
           m_nCurrentPos++;
       }
       ret = TRUE;
       break;

     case AVK_SELECT:
       // click on select key to enable edit mode
       m_bEdit = TRUE;
       m_nSavedData = m_nValue;
       m_nCurrentPos = WSTRLEN(m_strName);
       ret = TRUE;
       break;

     case AVK_CLR:
       if(m_bEdit && m_nCurrentPos > 0)
       {       
         (void)DeleteChar();
         ret = TRUE;
       }
       else
       {
         if(m_bEdit)
         {
           SetValue(m_nSavedData);
           m_bEdit = FALSE;
           ret = TRUE;
         }
       }
       m_previousEvent = wParam;
       break;

     default:
       return OnKeyEvent(eCode, wParam, dwParam);
    }
    break;

  case EVT_KEY:
    if(AVK_CLR == wParam && m_nCurrentPos)
    {
      (void)DeleteChar();
      ret = TRUE;
    }
    else
    {
      if(m_bEdit)
      {
        if(m_previousEvent == AVK_CLR)
        {
          SetValue(m_nSavedData);
        }
        m_bEdit = FALSE;
        ret = TRUE;
      }
    }
    m_previousEvent = wParam;
    break;

  default:
    //return OnKeyEvent(eCode, wParam, dwParam);
    break;
  }

  return ret;
}
boolean CMPNumEdit::OnKeyEvent(AEEEvent /*eCode */, uint16 wParam, uint32 /*dwParam*/)
{
  boolean bRet = FALSE;

  if(FALSE == m_bEdit && (wParam >= AVK_0 && wParam <= AVK_9))
  {
    m_nSavedData = m_nValue;
    memset(m_strName, 0, m_nMaxLength*sizeof(AECHAR)); 
    // set cursor to the end
    m_nCurrentPos = 1;
    m_bEdit = TRUE;
  }

  switch (wParam)
  {
  case AVK_0:
    (void)AddChar("0");
    bRet = TRUE;
    break;
  case AVK_1:
    (void)AddChar("1");
    bRet = TRUE;
    break;
  case AVK_2:
    (void)AddChar("2");
    bRet = TRUE;
    break;
  case AVK_3:
    (void)AddChar("3");
    bRet = TRUE;
    break;
  case AVK_4:
    (void)AddChar("4");    
    bRet = TRUE;
    break;
  case AVK_5:
    (void)AddChar("5");
    bRet = TRUE;
    break;
  case AVK_6:
    (void)AddChar("6");
    bRet = TRUE;
    break;
  case AVK_7:
    (void)AddChar("7");
    bRet = TRUE;
    break;
  case AVK_8:
    (void)AddChar("8");
    bRet = TRUE;
    break;
  case AVK_9:
    (void)AddChar("9");
    bRet = TRUE;
    break;
  case AVK_STAR:
    bRet = TRUE;
    break;
  case AVK_POUND:
    bRet = TRUE;
    break;
  default:
    bRet = FALSE;
    break;
  }

  if(TRUE == bRet && (wParam >= AVK_0 && wParam <= AVK_9))
  {
    m_previousEvent = wParam;
  }
  return bRet;
}
boolean CMPNumEdit::AddChar(const char* pChar)
{
  int    value;
  int    pos;
  char   strBuf[NUM_EDIT_MAX_LEN];
  int    nLength;

  if(m_strName == NULL)
  {
    DBGPRINTF_ERROR("MP: Uninitialized buffer.");
    return FALSE;
  }

  (void)WSTRTOSTR(m_strName, strBuf, sizeof(strBuf));
  // remove leading 0s if there is any
  value = ATOI(strBuf);
  (void)SNPRINTF(strBuf, sizeof(strBuf), "%d", value);
  
  nLength = STRLEN(strBuf);
  if(nLength >= (m_nMaxLength - 1)) // need one for end of string
  {
    // reach to max
    DBGPRINTF_ERROR("MP: buffer full.");
    return FALSE;
  }

  pos = m_nCurrentPos;
  if(pChar != NULL)
  {
    if(strBuf[0] == '0' && nLength == 1 || m_nMax < 10)
    {
      // current value is 0
      strBuf[0] = pChar[0];
      strBuf[1] = '\0'; 
    }
    else if(pos < nLength)
    {
      // inserting, we don't insert 0 at the first place
      if((pChar[0] == '0' && pos > 0)|| pChar[0] != '0' ) //&& pos > 0)|| (pChar[0] |= '0')
      {
        // move string to leave 1 char space for inserting
        (void)MEMMOVE(&strBuf[pos + 1], &strBuf[pos], (nLength - pos)*sizeof(char));
        // insert new char
        strBuf[pos] = pChar[0];
        // advance current pos by 1
        pos++;

      }
      else if(pChar[0] == '0' && m_nCurrentPos == 0)
      {
       // if 0 and current pos is in 0, change value to negative
        value = -m_nValue;
        if(value >= m_nMin && value <= m_nMax)
        {
          m_nValue = value;
          (void)SNPRINTF(strBuf, sizeof(strBuf), "%d", m_nValue);
          (void)STRTOWSTR(strBuf, m_strName, m_nMaxLength*sizeof(AECHAR));
          SetDirty(TRUE);
          return TRUE;
        }
      }
    }
    else
    {
      if (pos < (m_nMaxLength - 1))
      {
        //append at end
        strBuf[pos] = pChar[0];
        // advance current pos by 1
        pos++;
      }
    }
  
    // make sure NULL at end of string
    strBuf[nLength+1] = '\0';   
    value = ATOI(strBuf);
    if(value >= m_nMin && value <= m_nMax)
    {
      m_nValue = value;
      m_nCurrentPos = pos;
      (void)STRTOWSTR(strBuf, m_strName, m_nMaxLength*sizeof(AECHAR));
      SetDirty(TRUE);
    }

    return TRUE;
  }
  
  return FALSE;
}
boolean CMPNumEdit::DeleteChar()
{
  char   strBuf[NUM_EDIT_MAX_LEN];
  int    nLength;

  // get current string length
  nLength = WSTRLEN(m_strName);
  if(nLength == 0)
    return FALSE;

  if(m_nCurrentPos < nLength -1)
  { 
    //move strings to leave 1 char room for inserting
    (void)MEMMOVE(&m_strName[m_nCurrentPos - 1], &m_strName[m_nCurrentPos], (nLength - m_nCurrentPos)*sizeof(AECHAR));
  }
  m_nCurrentPos--;
  m_strName[nLength-1] = 0; 
  (void)WSTRTOSTR(m_strName, strBuf, sizeof(strBuf));
  //save current value
  m_nValue = ATOI(strBuf);

  SetDirty(TRUE);
  return TRUE;
}
boolean CMPNumEdit::GetEditMode()
{
  return m_bEdit;
}
void CMPNumEdit::SetEditMode(boolean bMode)
{
  m_bEdit = bMode;
}

void CMPNumEdit::SetRange(int nMin, int nMax)
{
  m_nMin = nMin;
  m_nMax = nMax;
}
void CMPNumEdit::GetRange(int* nMin, int* nMax)
{
  *nMin = m_nMin;
  *nMax = m_nMax;
}
void CMPNumEdit::SetIncStep(int nValue)
{
  m_nStep = nValue;
}


//----------------------------------------------------------------
//  CMPBar
//----------------------------------------------------------------
CMPBar::CMPBar()
{
  m_nIndex = 0;
  m_nMaxValueIndex = 0;
  m_nMaxValue = 0;
  m_nBufferSize = MP_DEFAULT_BUFFER_SIZE; // since spectrum data is 100ms, this buffer will hold 1 sec data. 
  m_nReqBufferSize = m_nBufferSize;
  m_pDataBuffer = new int32[m_nBufferSize];
}
CMPBar::CMPBar(void* pOwner): CMPSymbol(pOwner)
{
  m_nIndex = 0;
  m_nMaxValueIndex = 0;
  m_nMaxValue = 0;
  m_nBufferSize = MP_DEFAULT_BUFFER_SIZE; // since spectrum data is 100ms, this buffer will hold 1 sec data.
  m_nReqBufferSize = m_nBufferSize;
  m_pDataBuffer = new int32[m_nBufferSize];
}
CMPBar::~CMPBar()
{
  if(m_pDataBuffer)
  {
    DBGPRINTF_HIGH("MP: delete m_pDataBuffer.");
    delete [] m_pDataBuffer;
  }
  m_pDataBuffer = NULL;
  m_nBufferSize = 0;
}

boolean CMPBar::SetBufSize(int nSize)
{
  if(nSize > 0)
  {
    int32* pbuf = new int32[nSize];
    if(pbuf)
    {
      DBGPRINTF_HIGH("MP: new buffer allocated, 0x%X, %d", pbuf, nSize);
      if(m_pDataBuffer)
      {
        DBGPRINTF_HIGH("MP: delete m_pDataBuffer.");
        delete [] m_pDataBuffer;
      }
      m_pDataBuffer = pbuf;
      m_nBufferSize = nSize;
      m_nReqBufferSize = m_nBufferSize;
      return TRUE;
    }
  }
  else
  {
    DBGPRINTF_ERROR("MP: wrong size = %d", nSize);
  }
  return FALSE;
}
int CMPBar::GetBufSize()
{
  return m_nBufferSize;
}

void CMPBar::CleanPeakHold()
{
  if(m_pDataBuffer)
  {
    int i;
    for(i=0; i < m_nBufferSize; i++)
    {
      m_pDataBuffer[i] = 0;
    }
    m_nIndex = 0;
    m_nMaxValueIndex = 0;
    m_nMaxValue = 0;
  }
}
void CMPBar::SetData(int32 data)
{
  UINT index = GetNextIndex();
  CMPSymbol::SetData(data);
  
  if(NULL == m_pDataBuffer)
  {
    DBGPRINTF_ERROR("MP: NULL ptr.");
    return;
  }
  if(m_nReqBufferSize != m_nBufferSize || index >= m_nBufferSize)
  {
    DBGPRINTF_ERROR("MP: Invalid buffer size, m_nReqBufferSize = %d, m_nBufferSize = %d, index = %d",
                    m_nReqBufferSize, m_nBufferSize, index);
    return;
  }
  
  if(data < m_nMaxValue)
  {
    m_pDataBuffer[index] = data;
    // the old MaxValue happened be the one replaced, recompute
    if(index == m_nMaxValueIndex)
    {
      // recompute Max Value
      ComputeMaxValue();
    }
  }
  else
  {
    // new value is the max
    m_pDataBuffer[index] = data;
    m_nMaxValue = data;
    m_nMaxValueIndex = index;
  }
}

int CMPBar::GetNextIndex()
{
  if(++m_nIndex >= m_nBufferSize)
  {
    m_nIndex = 0;
  }
  return m_nIndex;
}
void CMPBar::ComputeMaxValue()
{
  int i;
  int32 fMax=0;
  int MaxIndex = 0;

  for(i=0; i < m_nBufferSize; i++)
  {
    if(m_pDataBuffer[i] > fMax)
    {
      fMax = m_pDataBuffer[i];
      MaxIndex = i;
    }
  }
  m_nMaxValue = fMax;
  m_nMaxValueIndex = MaxIndex;
}

boolean CMPBar::Draw(IGraphics *pg)
{
  boolean rval = TRUE;
  CMPRect rc;
  AEERect* prc;
  AEELine  line;

  (void)GetFrameRect(&rc);
  prc = rc.GetRect();

  prc->y = prc->y + prc->dy - m_data;
  prc->dy = m_data;

  (void)IGRAPHICS_SetColor(pg, GetFgColor()->GetR(), GetFgColor()->GetG(), GetFgColor()->GetB(), 1);
  (void)IGRAPHICS_DrawRect(pg, prc);

  // set peakhold bar to red
  (void)IGRAPHICS_SetColor(pg, 255, 0, 0, 1);
  (void)GetFrameRect(&rc);
  prc = rc.GetRect();

  // draw peakhold bar
  if(m_nMaxValue)
  {
    line.sx = prc->x;
    line.sy = prc->y + prc->dy - m_nMaxValue;
    line.ex = prc->x + prc->dx - 1;
    line.ey = line.sy;
    (void)IGRAPHICS_DrawLine(pg, &line);
  }
  return rval;
}

//-----------------------------------------------------------------
//  CMPThumb
//----------------------------------------------------------------
CMPThumb::CMPThumb(void* pOwner):CMPSymbol(pOwner)
{
}
CMPThumb::~CMPThumb()
{
}

boolean CMPThumb::Draw(IGraphics *pg)
{
  boolean rval = TRUE;
  CMPRect Rect;
  
  (void)GetFrameRect(&Rect);

  (void)IGRAPHICS_SetColor(pg, GetFgColor()->GetR(), GetFgColor()->GetG(), GetFgColor()->GetB(), 1);
 // IGRAPHICS_SetFillColor(pg, GetBkColor()->GetR(), GetBkColor()->GetG(), GetBkColor()->GetB(), 1);

  Rect.GetRect()->dx -= 1;
  Rect.GetRect()->dy -= 1;
  //IGRAPHICS_DrawRect(pg, (AEERect*)&Rect);
  (void)MP_DrawFrame(pg, Rect.GetRect(), AEE_FT_RAISED, GetFgColor()->GetRGB());
//  IGRAPHICS_SetBackground(pg,0,0,128);
//  IGRAPHICS_ClearViewport(pg);
//  IGRAPHICS_SetFillMode(pg, TRUE);
//  IGRAPHICS_SetPaintMode(pg, AEE_PAINT_COPY);
//  IGRAPHICS_SetFillColor(pg,255,0,0,100); 
  
//  rval = (IGRAPHICS_DrawRect(pg, &m_FrameRect) == SUCCESS);
  return rval;
}

void CMPThumb::SetThumbSize(int dx, int dy)
{
  int x, y, dx1, dy1;
  (void)GetFrameRect(&x, &y, &dx1, &dy1);
  SetFrameRect(x, y, dx, dy);
}

//-----------------------------------------------------------------
//  CMPSlider
//----------------------------------------------------------------
CMPSlider::CMPSlider():CMPSymbol(NULL), m_Thumb(NULL),
  m_bOrientation(VERTICAL), m_nThombPos(0), m_nUpperBound(0), m_nLowerBound(0)
{
}
CMPSlider::CMPSlider(void* pOwner):CMPSymbol(pOwner), m_Thumb(pOwner),
  m_bOrientation(VERTICAL), m_nThombPos(0), m_nUpperBound(0), m_nLowerBound(0)
{
}
/*
CMPSlider::CMPSlider(CMPRect rc, Orientation nOrientation):
  CMPSymbol(rc), m_bOrientation(nOrientation)
{
}
*/
CMPSlider::~CMPSlider()
{
}

boolean CMPSlider::Draw(IGraphics *pg)
{
  boolean rval = TRUE;
  CMPRect rc;
  
  (void)GetFrameRect(&rc);

  (void)IGRAPHICS_SetFillMode(pg, TRUE);
  if(GetFocus())
  {
    // draw slider border frame
    (void)MP_DrawFrame(pg, rc.GetRect(), AEE_FT_LOWERED, MAKE_RGB(0, 64, 64));
    m_Thumb.GetFgColor()->SetColor(0, 150, 150);
  }
  else
  {
    (void)MP_DrawFrame(pg, rc.GetRect(), AEE_FT_LOWERED, MAKE_RGB(128, 128, 128));
     m_Thumb.GetFgColor()->SetColor(120, 120, 120);
  }

  // draw thumb box
  (void)m_Thumb.Draw(pg);
/*
  // draw slider border frame
  (void)MP_DrawFrame(pg, rc.GetRect(), AEE_FT_LOWERED, MAKE_RGB(0, 64, 64));

  // draw thumb box
  (void)m_Thumb.Draw(pg);

  IGRAPHICS_SetFillMode(pg, FALSE);
  if(GetFocus())
  {  
    aeeRc = *rc.GetRect();
    aeeRc.x  -= 3;
    aeeRc.dx += 6;
    aeeRc.y  -= 3;
    aeeRc.dx += 6;

    IGRAPHICS_SetColor(pg, 64, 64, 64, 1);
  //  IGRAPHICS_DrawRect(pg, &aeeRc);
  }
  IGRAPHICS_SetFillMode(pg, TRUE);
*/
  return rval;
}

void CMPSlider::SetThumbSize(int w, int h)
{
  m_Thumb.SetSize(w, h);
}

boolean CMPSlider::SetThumbPos(int value)
{
  int x, y;
  if(value < m_nLowerBound)
    value = m_nLowerBound; 
  else if(value > m_nUpperBound) 
    value = m_nUpperBound; 

//  m_nThombPos = m_nUpperBound - value + m_nLowerBound;
  m_nThombPos = value;
  // get current slider bar center pos
  (void)GetPos(&x, &y);

  // convert to physical value
  if(m_bOrientation == VERTICAL)
  {
    y = GetThumbPhysicalPos(m_nThombPos, m_bOrientation);
  }
  else if(m_bOrientation == HORZIONTAL)
  {
    x = GetThumbPhysicalPos(m_nThombPos, m_bOrientation);
  }
  else
    return FALSE;

  m_Thumb.SetPos(x, y);
  return TRUE;
}
int CMPSlider::GetThumbPos()
{
  return m_nThombPos;
}

void CMPSlider::SetUpperBound(int value)
{
  m_nUpperBound = value;
}
int CMPSlider::GetUpperBound()
{
  return m_nUpperBound;
}
void CMPSlider::SetLowerBound(int value)
{
  m_nLowerBound = value;
}
int CMPSlider::GetLowerBound()
{
  return m_nLowerBound;
}
void CMPSlider::SetOrientation(int nOrientation)
{
  m_bOrientation = nOrientation;
}
int CMPSlider::GetThumbPhysicalPos(int log, int Or)
{
  CMPRect rc;
  int l = m_nUpperBound - m_nLowerBound;

  (void)GetFrameRect(&rc);

  if(l)
  {
    if(Or == VERTICAL)
      return (log - m_nLowerBound)*rc.GetRect()->dy/l + rc.GetRect()->y;
    else if(Or == HORZIONTAL)
      return (log - m_nLowerBound)*rc.GetRect()->dx/l + rc.GetRect()->x;
  }
  return 0;
}


int CMPSlider::GetThumbLogicalPos(int log, int Or)
{
  int value = 0;
  CMPRect rc;
  int l = m_nUpperBound - m_nLowerBound;

  (void)GetFrameRect(&rc);

  if(l)
  {
    if(Or == VERTICAL)
    {
      value = (log - rc.GetRect()->y)*l/rc.GetRect()->dy + m_nLowerBound;
    }
    else if(Or == HORZIONTAL)
    {
      value = (log - rc.GetRect()->x)*l/rc.GetRect()->dx + m_nLowerBound;
    }
  }
  else
    return m_nLowerBound;

  value = m_nUpperBound - value + m_nLowerBound;
  return value;
}

CMPThumb* CMPSlider::GetThumb()
{
  return &m_Thumb;
}

void CMPSlider::SetOwnerPtr(void* ptr)
{
  m_Thumb.SetOwnerPtr(ptr);
  CMPSymbol::SetOwnerPtr(ptr);
}

CMPSliderH::CMPSliderH(void* pOwner):CMPPanel(pOwner), m_pNameString(NULL), m_bForceSliderOffset(FALSE),
m_nSliderOffset(0), m_nStep(VALUE_STEP),m_SliderHight(SLIDERBAR_HIGH), m_szThumbW(THUMB_WIDTH), m_szThumbH(THUMB_HIGHT)
{
  m_Label.SetOwnerPtr(pOwner);
  m_pSlider.SetOwnerPtr(pOwner);
}
CMPSliderH::~CMPSliderH()
{
  if(m_pNameString)
    delete m_pNameString;
}
boolean CMPSliderH::Init(int x, int y, int dx, int dy)
{
  int       labelWidth;
  IDisplay* pDisplay;
  MPDeviceInfo* pDeviceInfo;

  pDeviceInfo = MP_GetInfo(this);
  if(!pDeviceInfo)
    return FALSE;

  pDisplay = pDeviceInfo->m_pDisplay;
  if(!pDisplay)
    return FALSE;

  SetFrameRect(x, y, dx, dy);

  if(m_nSliderOffset)
    labelWidth = m_nSliderOffset;
  else
    labelWidth = IDISPLAY_MeasureText(pDisplay, AEE_FONT_NORMAL, m_pNameString) + 4;

  m_Label.SetOwnerPtr(GetOwnerPtr());
  m_Label.SetID(0);
  m_Label.SetName(m_pNameString);
  m_Label.SetFrameRect(x, y, labelWidth, dy);
  m_Label.GetBkColor()->SetColor(153, 204, 204);
  m_Label.GetFgColor()->SetColor(0, 212, 212);
  m_Label.GetSdColor()->SetColor(0, 150, 150);
  m_Label.GetTextColor()->SetColor(0, 0, 0);


  m_pSlider.SetOrientation(CMPSlider::HORZIONTAL);
  m_pSlider.SetOwnerPtr(GetOwnerPtr());
  m_pSlider.SetID(m_nStep);
  // make slider bar SLIDERBAR_WIDTH pixels height
  m_pSlider.SetFrameRect(x + labelWidth, 
                        (y + dy/2) - m_SliderHight/2,  
                         dx - (x + labelWidth), 
                         m_SliderHight);

  m_pSlider.SetThumbSize(m_szThumbW, m_szThumbH);

  m_pSlider.GetBkColor()->SetColor(GetBkColor()->GetRGB());
  m_pSlider.GetFgColor()->SetColor(GetFgColor()->GetRGB());
  m_pSlider.GetSdColor()->SetColor(GetSdColor()->GetRGB());
  m_pSlider.GetFlColor()->SetColor(GetFlColor()->GetRGB());

  m_pSlider.GetThumb()->GetBkColor()->SetColor(GetBkColor()->GetRGB());
  m_pSlider.GetThumb()->GetFgColor()->SetColor(GetFgColor()->GetRGB());
  m_pSlider.GetThumb()->GetSdColor()->SetColor(GetSdColor()->GetRGB());
  m_pSlider.GetThumb()->GetFlColor()->SetColor(GetFlColor()->GetRGB());

  SetFocus(FALSE);
  return TRUE;
}
AECHAR* CMPSliderH::GetNameString()
{
  return m_pNameString;
}
CMPLabel* CMPSliderH::GetLabel()
{
  return &m_Label;
}
CMPSlider* CMPSliderH::GetSlider()
{
  return &m_pSlider;
}

// this function will force the slider to be in given offset
// set it back to 0, will removing the forcing mode, and the 
// offset will be calculated base on the label string length
// Init() should be called to take effect after setting
void CMPSliderH::SetSliderOffset(int x)
{
  m_nSliderOffset = x;
}

void CMPSliderH::SetLabelName(const char* pszSting)
{
//  CMPRect rc;
  if(m_pNameString)
    delete m_pNameString;
  m_pNameString = MP_ToAEEChar(pszSting);

  // since the new label, we need to reinit 
 // GetFrameRect(&rc);
 // (void)Init((rc.GetRect())->x, (rc.GetRect())->y, (rc.GetRect())->dx, (rc.GetRect())->dy);
}
void CMPSliderH::SetSliderStep(int nStep)
{
  m_nStep = nStep;
}
void CMPSliderH::SetSliderBarHight(int nHight)
{
  m_SliderHight = nHight;
}
void CMPSliderH::SetSliderThumbSize(int w, int h)
{
  m_szThumbW = w;
  m_szThumbH = h;
}

void CMPSliderH::SetFocus(boolean bMode)
{
  m_pSlider.SetFocus(bMode);
}
boolean CMPSliderH::GetFocus()
{
  return m_pSlider.GetFocus();
}

void CMPSliderH::SetFrameRect(CMPRect& rc)
{
  int16 x, y, dx, dy;
  (void)rc.GetRect(&x,&y,&dx,&dy);
  SetFrameRect(x,y,dx,dy);
}

void CMPSliderH::SetFrameRect(int x, int y, int dx, int dy)
{
  int nPos_x, nPos_y;
  int x1, y1;
  CMPSymbol::SetFrameRect(x, y, dx, dy);
 
  // reset label and thumb position
  // we only need to do vertical, due to the container (listWin) 
  // move it only up and down. will do horizontal latter
  (void)GetPos(&nPos_x, &nPos_y);
  (void)m_Label.GetPos(&x1, &y1);
  m_Label.SetPos(x1, nPos_y);

  (void)m_pSlider.GetPos(&x1, &y1);
  m_pSlider.SetPos(x1, nPos_y);

  (void)m_pSlider.SetThumbPos(m_pSlider.GetThumbPos());

}
boolean CMPSliderH::Draw(IGraphics *pg)
{
  (void)m_Label.Draw(pg);
  (void)m_pSlider.Draw(pg);
  return TRUE;
}

int CMPSliderH::GetThumbValue()
{
  return m_pSlider.GetThumbPos();
}

boolean CMPSliderH::OnEvent(AEEEvent eCode, uint16 wParam, uint32 /*dwParam*/)
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
        bRet = TRUE;
        break;
      case AVK_UP:
      case AVK_DOWN:
        break;
      case AVK_LEFT:
        {
          int pos;
          pos = m_pSlider.GetThumbPos() - m_nStep;
          (void)m_pSlider.SetThumbPos(pos);
        }
        bRet = TRUE;
        break;
      case AVK_RIGHT:
        {
          int pos;
          pos = m_pSlider.GetThumbPos() + m_nStep;
          (void)m_pSlider.SetThumbPos(pos);
        }
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


