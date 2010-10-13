/*=============================================================================
  FILE: MPColor.cpp

  SERVICES: CMPColor class Implementation
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPColor.cpp#2 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPColor.h"

CMPColor::CMPColor()
{
  m_nCol = MAKE_RGB(255,255,255);
}

CMPColor::CMPColor(RGBVAL clr)
{
  m_nCol = clr;
}
CMPColor::CMPColor(uint8 r, uint8 g, uint8 b)
{
  m_nCol = MAKE_RGB(r, g, b);
}
CMPColor::~CMPColor()
{
}
RGBVAL CMPColor::GetRGB()
{
  return m_nCol;
}
uint8 CMPColor::GetR() const
{
  uint32 c = m_nCol;
  c <<= 16;
  c >>= 24;
  return (uint8) c;
}

uint8 CMPColor::GetG() const
{
  uint32 c = m_nCol;
  c <<= 8;
  c >>= 24;
  return (uint8) c;
}

uint8 CMPColor::GetB() const
{
  uint32 c = m_nCol;
  c >>= 24;
  return (uint8) c;
}

void CMPColor::SetColor(RGBVAL clr)
{
  m_nCol = clr;
}
void CMPColor::SetColor(uint8 r, uint8 g, uint8 b)
{
  m_nCol = MAKE_RGB(r, g, b);
}
RGBVAL CMPColor::MakeColor(uint8 r, uint8 g, uint8 b)
{
  return MAKE_RGB(r, g, b);
}

