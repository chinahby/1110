/*=============================================================================
  FILE: MPBcastSettingsPanel.h

  SERVICES: defines CMPBcastSettingsPanel class
 
  GENERAL DESCRIPTION: 
    Panel containing controls for viewing and setting various Video Broadcast
    settings.

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPBcastSettingsPanel.h#5 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPBCASTSETTINGSPANEL_H
#define MPBCASTSETTINGSPANEL_H

#include "MPListWin.h"
#include "MPSymbols.h"

#ifdef __cplusplus

class CMPBcastSettingsPanel : public CMPSymbol
{
public:

  typedef enum MP_LABEL_TYPE
  {
    MP_BCAST_SETTINGS_TIMESTAMP = 0,
    MP_BCAST_SETTINGS_OFFSET,
    MP_BCAST_SETTINGS_MAX
  } MPLABELTYPE;

  CMPBcastSettingsPanel(void* pOwner);
  virtual ~CMPBcastSettingsPanel();
  void Destroy();

  boolean Init(AEERect rc);
	virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);

protected:
  int                     m_nNumofItems;      // number of labels
  int                     m_nRowHeight;  
  int                     m_nOffset;
  int                     m_nWidth;

  CMPNumEdit*             m_pLabels;          // pts to all labels
  CMPListWin<CMPNumEdit>* m_pListWin;         // list win ptr, contain all labels

  // return the max length of labels in pixels
  int  GetMaxLabelLength();
  // calculate the width of a given string
  int  GetStringWidth(const char* pstr);
  // update data
  void UpdateBcastSettingsCtl();

private:
  void    Cleanup();  
  boolean CreateNumEdit(int iMinVal, int iMaxVal, int iStepVal, int id, char* pName);
};
#endif //__cplusplus

#endif //MPBCASTSETTINGSPANEL_H
