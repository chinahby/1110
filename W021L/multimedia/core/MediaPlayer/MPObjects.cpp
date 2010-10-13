/*=============================================================================
  FILE: MPObjects.cpp

  SERVICES: MPObjects class object Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPObjects.cpp#4 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPObjects.h"

//------------------------------------------------------------------------
//       CMPPoint
//------------------------------------------------------------------------
CMPPoint::CMPPoint()
{
  m_pt.x = 0; m_pt.y = 0;
}
CMPPoint::CMPPoint(int px, int py)
{
  m_pt.x = px;
  m_pt.y = py;
}
CMPPoint::CMPPoint(MPPOINT& pt)
{
  m_pt.x = pt.x;
  m_pt.y = pt.y;
}
CMPPoint::~CMPPoint()
{
}
void CMPPoint::Set(int x, int y)
{
  m_pt.x = x;
  m_pt.y = y;
}
int CMPPoint::GetX() const
{
  return m_pt.x;
}
int CMPPoint::GetY() const
{
  return m_pt.y;
}

const MPPOINT* CMPPoint::GetPoint()const
{
  return &m_pt;
}

//------------------------------------------------------------------------
//       CMPRect
//------------------------------------------------------------------------
CMPRect::CMPRect()
{
  m_rc.x = 0;
  m_rc.y = 0;
  m_rc.dx = 0;
  m_rc.dy = 0;
}
CMPRect::CMPRect(AEERect& rc)
{
  m_rc.x = rc.x; 
  m_rc.y = rc.y;
  m_rc.dx = rc.dx;
  m_rc.dy = rc.dy;
}
CMPRect::CMPRect(int left, int top, int width, int height)
{
  Set(left, top, width, height);
}
CMPRect::~CMPRect()
{
}

CMPRect& CMPRect::operator=(const AEERect& rc)
{
  m_rc.x = rc.x; 
  m_rc.y = rc.y;
  m_rc.dx = rc.dx;
  m_rc.dy = rc.dy;

  return *this;
}

void CMPRect::Set(int left, int top, int width, int height)
{
  m_rc.x = left; 
  m_rc.y = top;
  m_rc.dx = width;
  m_rc.dy = height;
}

void CMPRect::Set(CMPRect& rc)
{
  m_rc = *rc.GetRect();
}
boolean CMPRect::Get(CMPRect* rc)
{
  boolean ret = FALSE;
  if(rc)
  {
    rc->Set(m_rc.x, m_rc.y, m_rc.dx, m_rc.dy);
    ret = TRUE;
  }
  return ret;
}
AEERect* CMPRect::GetRect()
{
  return (AEERect*)&m_rc;
}
//boolean CMPRect::GetRect(AEERect* rc)
//{
//  *rc = m_rc;
//  return TRUE;
//}

boolean CMPRect::GetRect(int16* x, int16* y, int16* dx, int16* dy)
{
  *x = m_rc.x;
  *y = m_rc.y;
  *dx = m_rc.dx;
  *dy = m_rc.dy;
  return TRUE;
}
void CMPRect::SetPos(int x_pos, int y_pos)
{
  m_rc.x = x_pos - m_rc.dx/2;
  m_rc.y = y_pos - m_rc.dy/2;
}
boolean CMPRect::GetPos(int* x, int* y)
{
  if(x && y)
  {
    *x = m_rc.x + m_rc.dx/2;
    *y = m_rc.y + m_rc.dy/2;
    return TRUE;
  }
  return FALSE;
}

void CMPRect::SetSize(int w, int h)
{
  m_rc.x = (m_rc.x + m_rc.dx/2) - w/2;
  m_rc.y = (m_rc.y + m_rc.dy/2) - h/2;
  m_rc.dx = w;
  m_rc.dy = h;
}
boolean CMPRect::GetSize(int* w, int* h)
{
  if(w && h)
  {
    *w = (int)m_rc.dx;
    *h = (int)m_rc.dy;
    return TRUE;
  }
  return FALSE;
}

boolean CMPRect::IsInitialize()
{
  return (m_rc.x || m_rc.y || m_rc.dx || m_rc.dy);
}

void CMPRect::Resize(AEERect* pRc, int x, int h)
{
  if(pRc)
  {
    pRc->dx += x;
    pRc->dy += h;
  }
}

