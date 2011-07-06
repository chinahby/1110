/*=============================================================================
  FILE: MPListWin.h

  SERVICES: list control
 
  GENERAL DESCRIPTION:
  This file defines a list panel which services as a container for 
	graph objects such as buttons, labels and so on

	PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPListWin.h#8 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPLISTWIN_H
#define MPLISTWIN_H

#include "MPSymbols.h"
#include "AEEVector.h"
#include "AEEShell.h"
#include "AEEStdLib.h"     // needed for DBGPRINTF

#ifdef __cplusplus
//---------------------------------------------------------------------------
// class T
//   SetFrameRect(int x, int y, int dx, int dy);
//   SetFocus(boolean bMode);
//   SetSelection(boolean bMode);
//   Draw(IGraphics* pg);
//---------------------------------------------------------------------------

template <class T>
class CMPListWin : public CMPSymbol
{
public:
  CMPListWin(IShell* pShell, IDisplay* pDisp, AEERect& rc);
  virtual ~CMPListWin();

  virtual boolean Draw(IGraphics* pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);
  // set focus to this window
  virtual void SetFocus(boolean mode);

  // row height, number of columns, initial count of items
  boolean Init(int nRowHeight, int cols, int size);

  // add an item
  void    Add(T* p);
  // delete all items
  void    DeleteAll();
  // remove all items from vectory
  void    Empty();

  // enable/disable multiselection, the selected item may not have focus
  void    SetMultiSelection(boolean bMode);
  // set focus to an item, the focused item has selection
  void    SetItem(int idx);
  // get current focused item
  int     GetItem();
  // set focus to next item
  boolean SetNext();
  // set focus to previous item
  boolean SetPrevious();
  // set focus to first the item
  boolean SetFirst();
  // set focus to the last item
  boolean SetLast();
  // mark selection to an item
  boolean SetItemSelection(int idx);
  // return idx for selected item
  int     GetSelectedItem();

  T*      GetItem(int idx);
  int     GetItemCount();
  
  //left margin space
  void    SetLeftMargin(int x);
  // set scroll bar on left
  void    SetScrollBarOnLeft(boolean bLeft);

protected:
  IShell*   m_pShell;           // shell pointer
  IDisplay* m_pDisplay;         // display pointer
  IVector*  m_pVector;          // contains items

  boolean   m_bMultiSelection;  // allow multiple item selection
  boolean   m_bBarLeft;         // scroll bar on left
  int       m_nLeftMargin;      // left margin
  int       m_nCols;            // num of cols
  int       m_nPos;             // The first item pos in display page

  int       m_nRowHeight;       // row height
  int       m_nDispRows;        // visible rows
  int       m_SelectedCol;      // selected row
  
  // draw scroll bar
  void    DrawScrollBar(IGraphics* pg, CMPRect& rc, int pos);
  boolean IsFirstPage(int pos);
  boolean IsLastPage(int pos);
  void    SetPage(int pos);      // set given item pos into disply page

  // event actions
  boolean OnSelect();
  boolean OnLeft();
  boolean OnRight();
  boolean OnUp();
  boolean OnDown();
};

template <class T> CMPListWin<T>::CMPListWin(IShell* pShell, IDisplay* pDisp, AEERect& rc):
  m_pShell(pShell),m_pDisplay(pDisp),
  m_bMultiSelection(FALSE), m_bBarLeft(TRUE), m_nLeftMargin(5), m_nCols(1), m_nPos(0),
  m_nRowHeight(1), m_nDispRows(1), m_SelectedCol(0)
{
  if(!pShell || !m_pDisplay)
    DBGPRINTF_ERROR("MP: failed to create ListWin, wrong args input.");
  // create vector 
  if(ISHELL_CreateInstance(m_pShell, AEECLSID_VECTOR, (void **) &m_pVector) != SUCCESS)
  {
    DBGPRINTF_ERROR("MP: failed to create VECTOR.");
    m_pVector = NULL;
  }
  
  CMPSymbol::SetFrameRect(rc.x, rc.y, rc.dx, rc.dy);
}

template <class T> CMPListWin<T>::~CMPListWin()
{
  if(m_pVector != NULL){
    IVector_Release(m_pVector);
    m_pVector = NULL;
  }
}
// Initialize list
// list size can be reallocated latter
template <class T> boolean CMPListWin<T>::Init(int nRowHeight, int cols, int size)
{
  CMPRect rc;
  if(cols == 0 || nRowHeight == 0)
    return FALSE;

  // init the vector, the size is actually doesn't matter
  if(NULL == m_pVector  || IVector_Init(m_pVector, size) != SUCCESS)
  {
    DBGPRINTF("VECTOR Init Failed");
    return FALSE;
  }
  
  m_nCols = cols;
  m_nRowHeight = nRowHeight;
  (void)CMPSymbol::GetFrameRect(&rc);
  m_nDispRows = rc.GetRect()->dy / m_nRowHeight; 
  return TRUE;
}
// delete all items from vector
template <class T> void CMPListWin<T>::DeleteAll()
{
  T* p;
  int i, size;
  if(m_pVector){
    size = IVector_Size(m_pVector);
    for(i=0; i<size; i++){
      p = (T*)IVector_ElementAt(m_pVector, i);
      delete p;
      p = NULL;
    }
  }
}

template <class T> void CMPListWin<T>::Empty()
{
  IVector_RemoveAllElements(m_pVector);
}
// set left margin space
template <class T> void CMPListWin<T>::SetLeftMargin(int x)
{
  m_nLeftMargin = x;
}
// add an item into vector
template <class T> void CMPListWin<T>::Add(T* p)
{
  IVector_AddElement(m_pVector, (void*)p);
}
// set TRUE to enable multiple item selection
template <class T> void CMPListWin<T>::SetMultiSelection(boolean bMode)
{
  m_bMultiSelection = bMode;
}
// draw screen
template <class T> boolean CMPListWin<T>::Draw(IGraphics* pg)
{
  int size;
  int i;
  int col;
  T* p;
  CMPRect  frameRc;
  AEERect* pAEERc;
  AEERect rc;
  int  cnt;
  int offset;
  int x_level, dx_level;
  int x, y;

  size = IVector_Size(m_pVector);

  (void)CMPSymbol::GetFrameRect(&frameRc);
  pAEERc = frameRc.GetRect();

  // move 1 pixel away from left edge
  offset = 1;

  // Prevent divide by zero 
  if (m_nCols == 0)
    return FALSE;

  // display panel rect
  rc.x  = pAEERc->x + offset;
  rc.y  = pAEERc->y;
  rc.dx = (pAEERc->dx - offset)/m_nCols;
  rc.dy = m_nRowHeight;

  col = 0;
  for(i = m_nPos, cnt = 0; cnt < m_nDispRows*m_nCols && i< size; i++)
  {
    cnt++;
    p = (T*)IVector_ElementAt(m_pVector, i);
    if(col >= m_nCols)
    {
     col = 0;
     rc.y = rc.y + rc.dy;
    }

    if(!col)
      rc.x = pAEERc->x + col*rc.dx + m_nLeftMargin; // first col with margin offset
    else
      rc.x = pAEERc->x + col*rc.dx;
    col++;

    // calculate offset according to the label level
    x_level = rc.x + (rc.dy - 2)*(p->GetIndentLevel());
    dx_level = rc.dx - (rc.dy - 2)*(p->GetIndentLevel());

    // leave some offside on both sides for scroll bars
    p->SetFrameRect(x_level + 1, rc.y, dx_level - 4, rc.dy);
    (void)p->Draw(pg);
  }

  (void)frameRc.GetPos(&x, &y);
  frameRc.SetPos(x - 1, y);
  DrawScrollBar(pg, frameRc, m_nPos);
  return TRUE;
}
template <class T> void CMPListWin<T>::DrawScrollBar(IGraphics* pg, CMPRect& frameRc, int pos)
{
  AEETriangle tr;
  AEERect* pAEERc;
  AEERect rc;
  boolean fillmode;
  uint8 r, g, b, alpha;

  boolean fp = IsFirstPage(pos);
  boolean lp = IsLastPage(pos);

  pAEERc = frameRc.GetRect();
  fillmode = IGRAPHICS_GetFillMode(pg);
  DBGPRINTF_MED("MP: IGRAPHICS_GetFillMode returns %d", fillmode);
  IGRAPHICS_GetColor(pg, &r, &g, &b, &alpha);

  (void)IGRAPHICS_SetFillMode(pg, TRUE);
  // set draw and fill color same as text color
  (void)IGRAPHICS_SetColor(pg, 
         CMPSymbol::GetFgColor()->GetR(),
         CMPSymbol::GetFgColor()->GetG(),
         CMPSymbol::GetFgColor()->GetB(),
         1);
  (void)IGRAPHICS_SetFillColor(pg, 
         CMPSymbol::GetFgColor()->GetR(),
         CMPSymbol::GetFgColor()->GetG(),
         CMPSymbol::GetFgColor()->GetB(),
         1);

  if(m_bBarLeft)
  {
    if(fp && !lp)
    {
      // down
      tr.x0 = pAEERc->x;
      tr.y0 = pAEERc->y + pAEERc->dy - 7;
      tr.x1 = pAEERc->x + 3;
      tr.y1 = pAEERc->y + pAEERc->dy;
      tr.x2 = pAEERc->x + 5;
      tr.y2 = pAEERc->y + pAEERc->dy - 7;
      (void)IGRAPHICS_DrawTriangle(pg, &tr);

      rc.x = pAEERc->x + 2;
      rc.y = pAEERc->y;
      rc.dx = 2;
      rc.dy = pAEERc->dy - 7;
      (void)IGRAPHICS_DrawRect(pg, &rc);
    }
    else if(fp && lp)
    {
      // do nothing
    }
    else if(!fp && !lp)
    {
      // up
      tr.x0 = pAEERc->x + 3;
      tr.y0 = pAEERc->y;
      tr.x1 = pAEERc->x;
      tr.y1 = pAEERc->y + 7;
      tr.x2 = pAEERc->x + 5;
      tr.y2 = pAEERc->y + 7;
      (void)IGRAPHICS_DrawTriangle(pg, &tr);

      // down
      tr.x0 = pAEERc->x;
      tr.y0 = pAEERc->y + pAEERc->dy - 7;
      tr.x1 = pAEERc->x + 3;
      tr.y1 = pAEERc->y + pAEERc->dy;
      tr.x2 = pAEERc->x + 5;
      tr.y2 = pAEERc->y + pAEERc->dy - 7;
      IGRAPHICS_DrawTriangle(pg, &tr);

      rc.x = pAEERc->x + 2;
      rc.y = pAEERc->y + 7;
      rc.dx = 2;
      rc.dy = pAEERc->dy - 14;
      (void)IGRAPHICS_DrawRect(pg, &rc);
    }
    else if(!fp && lp)
    {
      // up
      tr.x0 = pAEERc->x + 3;
      tr.y0 = pAEERc->y;
      tr.x1 = pAEERc->x;
      tr.y1 = pAEERc->y + 7;
      tr.x2 = pAEERc->x + 5;
      tr.y2 = pAEERc->y + 7;
      (void)IGRAPHICS_DrawTriangle(pg, &tr);

      rc.x = pAEERc->x + 2;
      rc.y = pAEERc->y + 7;
      rc.dx = 2;
      rc.dy = pAEERc->dy - 7;
      (void)IGRAPHICS_DrawRect(pg, &rc);
    }
  }
  else
  {
    // draw bar on right side of rect
    if(fp && !lp)
    {
      // down
      tr.x0 = pAEERc->x + pAEERc->dx - 5;
      tr.y0 = pAEERc->y + pAEERc->dy - 7;
      tr.x1 = pAEERc->x + pAEERc->dx - 3;
      tr.y1 = pAEERc->y + pAEERc->dy;
      tr.x2 = pAEERc->x + pAEERc->dx;
      tr.y2 = pAEERc->y + pAEERc->dy - 7;
      (void)IGRAPHICS_DrawTriangle(pg, &tr);

      rc.x = pAEERc->x + pAEERc->dx - 3;
      rc.y = pAEERc->y;
      rc.dx = 2;
      rc.dy = pAEERc->dy - 7;
      (void)IGRAPHICS_DrawRect(pg, &rc);
    }
    else if(fp && lp)
    {
      // do nothing
    }
    else if(!fp && !lp)
    {
      // up
      tr.x0 = pAEERc->x + pAEERc->dx - 3;
      tr.y0 = pAEERc->y;
      tr.x1 = pAEERc->x + pAEERc->dx - 5;
      tr.y1 = pAEERc->y + 7;
      tr.x2 = pAEERc->x + pAEERc->dx;
      tr.y2 = pAEERc->y + 7;
      IGRAPHICS_DrawTriangle(pg, &tr);

      // down
      tr.x0 = pAEERc->x + pAEERc->dx - 5;
      tr.y0 = pAEERc->y + pAEERc->dy - 7;
      tr.x1 = pAEERc->x + pAEERc->dx - 3;
      tr.y1 = pAEERc->y + pAEERc->dy;
      tr.x2 = pAEERc->x + pAEERc->dx;
      tr.y2 = pAEERc->y + pAEERc->dy - 7;
      IGRAPHICS_DrawTriangle(pg, &tr);

      rc.x = pAEERc->x + pAEERc->dx  - 3;
      rc.y = pAEERc->y + 7;
      rc.dx = 2;
      rc.dy = pAEERc->dy - 14;
      IGRAPHICS_DrawRect(pg, &rc);
    }
    else if(!fp && lp)
    {
      // up
      tr.x0 = pAEERc->x + pAEERc->dx - 3;
      tr.y0 = pAEERc->y;
      tr.x1 = pAEERc->x + pAEERc->dx - 5;
      tr.y1 = pAEERc->y + 7;
      tr.x2 = pAEERc->x + pAEERc->dx;
      tr.y2 = pAEERc->y + 7;
      IGRAPHICS_DrawTriangle(pg, &tr);

      rc.x = pAEERc->x + pAEERc->dx  - 3;
      rc.y = pAEERc->y + 7;
      rc.dx = 2;
      rc.dy = pAEERc->dy - 7;
      IGRAPHICS_DrawRect(pg, &rc);
    }
  }
}
template <class T> void CMPListWin<T>::SetScrollBarOnLeft(boolean bLeft)
{
  m_bBarLeft = bLeft;
}
// set focus for the panel
// when panel lose focus, nothing will be shown
template <class T> void CMPListWin<T>::SetFocus(boolean mode)
{
  int idx;
  if(CMPSymbol::GetFocus())
  {
    if(mode == FALSE)
    {
      // clear focus
      SetItem(-1);
      CMPSymbol::SetFocus(mode);
    }
  }
  else
  {
    if(mode == TRUE)
    {
      // set focus to current selected item, and set page
      idx = GetSelectedItem();
      SetItem(idx);
      SetPage(idx);
      CMPSymbol::SetFocus(mode);
    }
    else
    {
      SetItem(-1);
      CMPSymbol::SetFocus(mode);
    }
  }
}

// set focus to item
template <class T> void CMPListWin<T>::SetItem(int idx)
{
  int i, size;
  T* p;
  if(!m_pVector)
    return;

  size = GetItemCount();
  if(idx >=0 && idx < size)
  {
    for(i=0; i<size; i++)
    {
      p = (T*)GetItem(i);
      if(p)
      {
        // calculate col from idx
        if(i == idx)
        {
          if(idx < m_nCols - 1)
            m_SelectedCol = idx;
          else
            m_SelectedCol = idx - (idx/m_nCols)*m_nCols;
          p->SetFocus(TRUE);
        }
        else
          p->SetFocus(FALSE);
      }
    }
  }
  else
  {
    // clear all focus
    for(i=0; i<size; i++)
    {
      p = (T*)GetItem(i);
      if(p)
        p->SetFocus(FALSE);
    }
  }  
}

//get index of an item, which has focus
template <class T> int CMPListWin<T>::GetItem()
{
  int i, size;
  T* p;

  size = GetItemCount();
  for(i=0; i<size; i++)
  {
    p = (T*)GetItem(i);
    if(p && p->GetFocus())
      return i;
  }
  return -1;
}

// set focus to next item
template <class T> boolean CMPListWin<T>::SetNext()
{
  int sel, size;

  size = GetItemCount();
  sel = GetItem();
  if(sel < size - 1)
  {
    SetItem(++sel);
    SetPage(sel);
    return TRUE;
  }
  return FALSE;
}

// set focus to previous item
template <class T> boolean CMPListWin<T>::SetPrevious()
{
  int sel;
  sel = GetItem();
  if(sel > 0)
  {
    SetItem(--sel);
    SetPage(sel);
    return TRUE;
  }
  return FALSE;
}
// set forcus to first item
template <class T> boolean CMPListWin<T>::SetFirst()
{
  int i;
  i = GetItemCount();
  if(i > 0)
	{
		i = 0;
    SetItem(i);
		SetPage(i);
		return TRUE;
	}
	return FALSE;
}
// set focus to last item
template <class T> boolean CMPListWin<T>::SetLast()
{
  int i;
  i = GetItemCount();
  if(i > 0)
	{
    SetItem(--i);
		SetPage(i);
		return TRUE;
	}
	return FALSE;
}

// set selection to an item
template <class T> boolean CMPListWin<T>::SetItemSelection(int idx)
{
  int i, size;
  T* p;
  boolean  ret = FALSE;

  if(!m_pVector)
    return ret;

  size = GetItemCount();
  if(idx >=0 && idx < size)
  {
    for(i=0; i<size; i++)
    {
      p = (T*)GetItem(i);
      if(p)
      {
        if(i == idx)
        {
          p->SetSelection(TRUE);
          ret = TRUE;
        }
        else if(m_bMultiSelection == FALSE)
          p->SetSelection(FALSE);
      }
    }
  }
  else
  { 
    // out of focus range
    if(m_bMultiSelection == FALSE)
    {
      // if not on multiselection mode, clear all selections
      for(i=0; i<size; i++)
      {
        p = (T*)GetItem(i);
        if(p)
          p->SetSelection(FALSE);
      }
    }
  }  
  return ret;
}
// get selected item
template <class T> int CMPListWin<T>::GetSelectedItem()
{
  int i, size;
  T* p;

  size = GetItemCount();
  for(i=0; i<size; i++)
  {
    p = (T*)GetItem(i);
    if(p && p->GetSelection())
      return i;
  }
  return -1;
}
// Get an item with given index
template <class T> T* CMPListWin<T>::GetItem(int idx)
{
  if(m_pVector)
    return (T*)IVector_ElementAt(m_pVector, idx);
  else
    return NULL;
}
// return number of items
template <class T> int CMPListWin<T>::GetItemCount()
{
  if(m_pVector)
    return IVector_Size(m_pVector);
  else
    return 0;
}
// given item to display page
template <class T> void CMPListWin<T>::SetPage(int pos)
{
  int i, size, pages;
  i = 0;
  pages = 0;
  size = GetItemCount();
  if(m_nDispRows && m_nCols)
    pages = size/(m_nDispRows*m_nCols) + 1;
  while(i < pages)
  {
    // scan page to find where is pos located
    if( pos >= m_nDispRows*m_nCols*i && pos < m_nDispRows*m_nCols*(i+1))
    {
      // set the first display item pos
      m_nPos = m_nDispRows*m_nCols*i;
      break;
    }
    i++;
  }
}
// return TRUE if it is in first page, otherwise return FASLE
template <class T> boolean CMPListWin<T>::IsFirstPage(int pos)
{
  if( pos >= 0 && pos < m_nDispRows*m_nCols)
    return TRUE;
  return FALSE;
}
// return TRUE if it is in last page, otherwise return FASLE
template <class T> boolean CMPListWin<T>::IsLastPage(int pos)
{
  int i, size, pages;
  i = 0;
  pages = 0;
  size = GetItemCount();
  if(m_nDispRows && m_nCols)
    pages = size/(m_nDispRows*m_nCols) + 1;

  while(i < pages)
  {
    if( pos >= m_nDispRows*m_nCols*i && pos < m_nDispRows*m_nCols*(i+1))
    {
      if( size >= m_nDispRows*m_nCols*i && size <= m_nDispRows*m_nCols*(i+1))
        return TRUE;
      else
        return FALSE;
    }
    i++;
  }
  return FALSE;
}
// set selection on focus item
template <class T> boolean CMPListWin<T>::OnSelect()
{
  return SetItemSelection(GetItem());
}

template <class T> boolean CMPListWin<T>::OnLeft()
{
  int sel;
  sel = GetItem();

  if(m_SelectedCol == 0)
    return FALSE;
  
  sel--;
  if(sel >= 0)
  {
    SetItem(sel);
    SetPage(sel);
    return TRUE;
  }
  return FALSE;
}
template <class T> boolean CMPListWin<T>::OnRight()
{
  int sel, size;
  size = GetItemCount();
  sel = GetItem();

  if(m_SelectedCol == m_nCols - 1)
    return FALSE;

  sel++;
  if(sel < size)
  {
    SetItem(sel);
    SetPage(sel);
    return TRUE;
  }
  return FALSE;
}
template <class T> boolean CMPListWin<T>::OnUp()
{
  int sel, size;

  size = GetItemCount();
  sel = GetItem();

  sel -= m_nCols;
  if(sel>=0 && sel < size)
  {
    SetItem(sel);
    SetPage(sel);
    return TRUE;
  }
  return FALSE;
}
template <class T> boolean CMPListWin<T>::OnDown()
{
  int sel, size;

  size = GetItemCount();
  sel = GetItem();

  sel += m_nCols;
  if(sel < size)
  {
    SetItem(sel);
    SetPage(sel);
    return TRUE;
  }
  return FALSE;
}
template <class T> boolean CMPListWin<T>::OnEvent(AEEEvent eCode, uint16 wParam, uint32 /*dwParam*/)
{
  int sel;
  boolean ret = FALSE;

  sel = GetItem();
  DBGPRINTF_MED("MP: GetItem returns %d", sel);
  switch(eCode)
  {
    case EVT_KEY_PRESS:
    switch(wParam)
    {
      case AVK_SELECT:
        ret = OnSelect();
        break;
      case AVK_UP:
        ret = OnUp();
        break;
      case AVK_DOWN:
        ret = OnDown();
        break;
      case AVK_LEFT:
        ret = OnLeft();
        break;
      case AVK_RIGHT:
        ret = OnRight();
        break;
     default:
       break;
    }
    break;
  default:
    break;
  }
  return ret;
}
#endif //__cplusplus

#endif //MPLISTWIN_H
