/*=============================================================================
  FILE: MPQConcertPresetPanel.h

  SERVICES: defines CMPQConcertPresetPanel class
 
  GENERAL DESCRIPTION: The panel is used to set 3D audio expander


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/mediaplayer/MPQConcertPresetPanel.h#3 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPQCONCERTPRESETPANEL_H
#define MPQCONCERTPRESETPANEL_H

#include "MPListWin.h"
#include "MPSymbols.h"

#ifdef __cplusplus
class CMPQConcertPresetPanel : public CMPSymbol
{
public:
typedef enum MP_LABEL_TYPE
{
  MP_LABEL_PRESET_HEADER = 0,
  MP_LABEL_PRESET_EDIT,
  MP_LABEL_NUMLEVELS_HEADER,
  MP_LABEL_NUMLEVELS_EDIT,
  MP_LABEL_LEVEL_HEADER,
  MP_LABEL_LEVEL_EDIT,
  MP_BUTTON_COMMIT,
  MP_LABEL_MAX
}MPLABELTYPE;

  CMPQConcertPresetPanel(void* pOwner);
  virtual ~CMPQConcertPresetPanel();
  void Destroy();

  boolean Init(AEERect rc);
  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);

protected:
  CMPLabel*                  m_pPresetHeader;    // Header text for preset information
  CMPNumEdit*                m_pPresetEdit;      // Num edit to select preset option
  CMPLabel*                  m_pLevelHeader;     // Header text for expansion strength
  CMPNumEdit*                m_pLevelEdit;       // Num edit to expansion strength
  CMPLabel*                  m_pCommit;          //commit button
  CMPListWin<CMPSymbol>*     m_pListWin;         // list win ptr, contain all labels
  AECHAR*                    m_pwszPresetHeader;
  AECHAR*                    m_pwszLevelHeader;
  AECHAR*                    m_pwszCommit;

  CMPLabel*                  m_pNumLevelsHeader; // Header text for number of levels
  CMPNumEdit*                m_pNumLevelsEdit;   // Num edit to number of levels
  AECHAR*                    m_pwszNumLevelsHeader;
  
  // update data
  void Commit();
  void CMPQConcertPresetCleanup();
};
#endif //__cplusplus

#endif //MPQCONCERTPRESETPANEL_H
