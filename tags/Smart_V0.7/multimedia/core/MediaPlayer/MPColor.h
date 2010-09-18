/*==========================================================================
  FILE: MPColor.h

  SERVICES: Declaration of CMPColor class
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPColor.h#4 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPCOLOR_H
#define MPCOLOR_H

#include "AEEDisp.h"

#define COLOR_WHITE      MAKE_RGB(255, 255, 255)
#define COLOR_BLACK      MAKE_RGB(  0,   0,   0)
#define COLOR_LT_GREY    MAKE_RGB(200, 200, 200)
#define COLOR_DRK_GERY   MAKE_RGB(128, 128, 128)

#ifdef __cplusplus
class CMPColor  
{
public:
	// Constructors
	// default constructor will set default white color
  CMPColor();
  CMPColor(RGBVAL clr);
  CMPColor(uint8 r, uint8 g, uint8 b);
  ~CMPColor();

  // set color
  void SetColor(RGBVAL clr);
  void SetColor(uint8 r, uint8 g, uint8 b);
  // get color RGB value
  RGBVAL GetRGB();

  uint8 GetB() const;
  uint8 GetG() const;
  uint8 GetR() const;

  static RGBVAL MakeColor(uint8 r, uint8 g, uint8 b);
private:
  RGBVAL m_nCol;
};
#else
  typedef struct CMPColor CMPColor;
#endif // __cplusplus

#endif //MPCOLOR_H
