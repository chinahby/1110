/*=============================================================================
  FILE: MPAudioExpanderPanel.h

  SERVICES: defines CMPAudioExpanderPanel class
 
  GENERAL DESCRIPTION: The panel is used to set 3D audio expander


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPAudioExpanderPanel.h#6 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPAUDIOEXPANDERPANEL_H
#define MPAUDIOEXPANDERPANEL_H

#include "MPListWin.h"
#include "MPSymbols.h"

#ifdef __cplusplus
class CMPAudioExpanderPanel : public CMPSymbol
{
public:
typedef enum MP_LABEL_TYPE
{
  MP_LABEL_MODE = 0,
  MP_LABEL_GAIN,
  MP_LABEL_SPREAD,
  MP_LABEL_GEOMETRY,
  MP_BUTTON_COMMIT,
  MP_LABEL_MAX
}MPLABELTYPE;

  CMPAudioExpanderPanel(void* pOwner);
  virtual ~CMPAudioExpanderPanel();
  void Destroy();

  boolean Init(AEERect rc);
  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);

protected:
  AECHAR*                    m_pwszNameString;
  int                        m_nNumofEditItems;  // number of labels
  CMPNumEdit*                m_pLabels;          // pts to all labels
  CMPLabel*                  m_pCommit;          //commit button
  CMPListWin<CMPSymbol>*     m_pListWin;         // list win ptr, contain all labels
  
  // return the max length of labels in pixel
  int  GetMaxLabelLength();
  // calculate the width of a given string
  int  GetStringWidth(const char* pstr);
  // update data
  void Commit();
};
#endif //__cplusplus

#endif //MPAUDIOEXPANDERPANEL_H
