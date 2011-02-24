/*=============================================================================
  FILE: MPEqualizerFilterPanel.h

  SERVICES: defines CMPEqualizerFilterPanel class
 
  GENERAL DESCRIPTION: allows user to set gain value for filter


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPEqualizerFilterPanel.h#6 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPEQUALIZERFILTERPANEL_H
#define MPEQUALIZERFILTERPANEL_H

#include "MPDefines.h"
#ifdef FEATURE_MP_EQ_UI
#include "MPSymbols.h"
#include "MPListWin.h"

#ifdef FEATURE_MP_EQ
#include "AEESoundCtl.h"
#endif

#ifdef __cplusplus

class CMPEqualizerFilterPanel : public CMPPanel
{
public:
  CMPEqualizerFilterPanel(void* pOwner);
  virtual ~CMPEqualizerFilterPanel();
  
  virtual boolean Init(AEERect rc);
  virtual boolean Init(int x, int y, int dx, int dy);

  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);

  void SetScrollBarOnLeft(boolean bLeft);

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

  // set focus to first the item
  void    SetFirst();
  // set focus to the last item
  void    SetLast();
protected:
  CMPLabel*             m_pLabels;
  int                   m_nNumofItems;
  CMPListWin<CMPLabel>* m_pListWin;

  void fSet(int i);
#ifdef FEATURE_MP_EQ
  void SetFilter(int i);
#endif //FEATURE_MP_EQ
};
#endif //__cplusplus
#endif //FEATURE_MP_EQ_UI
#endif //MPEQUALIZERFILTERPANEL_H
