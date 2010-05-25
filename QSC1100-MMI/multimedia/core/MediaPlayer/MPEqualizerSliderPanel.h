/*=============================================================================
  FILE: MPEqualizerSliderPanel.h

  SERVICES: this file defines CMPEqualizerSliderPanel slider panel objects
 
  GENERAL DESCRIPTION: The panel contains sliders


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPEqualizerSliderPanel.h#5 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPEQUALIZERSLIDERPANEL_H
#define MPEQUALIZERSLIDERPANEL_H

#include "MPDefines.h"

#ifdef FEATURE_MP_EQ_UI

#ifdef FEATURE_MP_EQ
#include "AEESoundCtl.h"
#endif //FEATURE_MP_EQ

#include "MPSymbols.h"

#define NUM_OF_BAR   8
#define THUMB_WIDTH  9
#define THUMB_HEIGHT 7
#define BAR_WIDTH    5

#ifdef __cplusplus

class CMPEqualizerSliderPanel : public CMPPanel
{
public:
  CMPEqualizerSliderPanel(void* pOwner);
  CMPEqualizerSliderPanel(AEERect& rc, int min, int max);
  virtual ~CMPEqualizerSliderPanel();

  static CMPEqualizerSliderPanel* Create(AEERect& rc, int min, int max);
  void Destroy();

  virtual boolean Init(AEERect /*rc*/) { return FALSE; }
  virtual boolean Init(int /*x*/, int /*y*/, int /*dx*/, int /*dy*/) { return FALSE; }
  virtual boolean Init(AEERect rc, int min, int max);
  void SetDrawingPanelRect(int x, int y, int dx, int dy);

  // set focus to this panel
  virtual void    SetFocus(boolean mode);
  // set focus to an item
  virtual void    SetItem(int idx);
  // return item id which has focus
  virtual int     GetItem();
  // set focus to next item
  virtual boolean SetNext();
  // set focus to previous item
  virtual boolean SetPrevious();
  // set selection on an item
  virtual void    SetSelectItem(int idx);
  // return item id which has selection
  virtual int     GetSelectItem();
  // return item count
  virtual int     GetItemCount();

  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);
protected:
  CMPRect        m_rcDwPanel;   // this is the drawing panel rect
  CMPSlider*     m_pBar;        // slider objects
  int            m_nNumofItems; // number of sliders
  int            m_nMax;        // max value of sliders
  int            m_nMin;        // min value of sliders

  byte           m_byBarWidth;
  byte           m_byThumbWidth;
  byte           m_byThumbHeight;

  void SetRange(int min, int max);
  int GetMinValue();
  int GetMaxValue();
  
  CMPSlider* GetSlider(int idx);

#ifdef FEATURE_MP_EQ
  void SetEqualizer(int16 gain);
#endif // FEATURE_MP_EQ

  boolean Inc();
  boolean Dec();
};

#endif //__cplusplus

#endif // FEATURE_MP_EQ_UI
#endif // MPEQUALIZERSLIDERPANEL_H
