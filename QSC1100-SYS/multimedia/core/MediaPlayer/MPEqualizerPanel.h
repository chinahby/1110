/*=============================================================================
  FILE: MPEqualizerPanel.h

  SERVICES: The top level of equalizer panels
 
  GENERAL DESCRIPTION: The panel contains a filter list win and slider panel


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPEqualizerPanel.h#5 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPEQUALIZERPANEL_H
#define MPEQUALIZERPANEL_H

#include "MPDefines.h"
#ifdef FEATURE_MP_EQ_UI
#include "MPListWin.h"
#include "MPSymbols.h"
#include "MPEqualizerSliderPanel.h"
#include "MPEqualizerFilterPanel.h"

#ifdef __cplusplus
class CMPEqualizerPanel : public CMPSymbol
{
public:
typedef enum MP_EQ_PANEL_TYPE
{
  MP_EQ_SLIDER,
  MP_EQ_FILTERSETTING,
  MP_EQ_EXIT,
  MP_EQ_LABEL_MAX
}MPEQPANELTYPE;

  CMPEqualizerPanel(void* pOwner);
  virtual ~CMPEqualizerPanel();

  boolean Init(AEERect rc, int min, int max);

//	void    SetFilter(int nFilterID);
//	int     GetFilter();
  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);
  // set/unset the forcus to this panel
  virtual void SetFocus(boolean mode);
protected:
  CMPPanel*     m_pSubPanel;   // current subpanel pointer
  CMPPanel*     m_pFilterPanel;
  AEERect       m_FilterPanelRc;
  AEERect       m_SubPanelRc;

  int           m_nSliderID;  // previouse selected slider id when switch subpanels
  int           m_nMin;       // slider min value
  int           m_nMax;       // slider max value

  void    Cleanup();
  boolean Exit();
  int     GetItemCount();

  boolean OnSelect(AEEEvent eCode, uint16 wParam, uint32 dwParam);
  boolean OnDown(AEEEvent eCode, uint16 wParam, uint32 dwParam);
  boolean OnUp(AEEEvent eCode, uint16 wParam, uint32 dwParam);
  boolean OnLeft(AEEEvent eCode, uint16 wParam, uint32 dwParam);
  boolean OnRight(AEEEvent eCode, uint16 wParam, uint32 dwParam);

  boolean CreateSubPanel(MPEQPANELTYPE type );
  boolean CreateSliderPanel(AEERect& rc);
  boolean CreateEqualizerFilterPanel(AEERect& rc);
};

#endif //__cplusplus
#endif // FEATURE_MP_EQ_UI
#endif //MPEQUALIZERPANEL_H
