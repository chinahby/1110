/*=============================================================================
  FILE: MPQTVSettingPanel.h

  SERVICES: this file defines CMPQTVSettingPanel class objects
 
  GENERAL DESCRIPTION:


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPQTVSettingPanel.h#2 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPQTVSETTINGPANEL_H
#define MPQTVSETTINGPANEL_H

#include "MPDefines.h"

#ifdef __cplusplus

#include "MPListWin.h"
#include "MPSymbols.h"

class CMPQTVSettingPanel : public CMPSymbol
{
public:
typedef enum MP_AUDIO_SETTING_TYPE
{
  MP_QTV_SETTING_SUSPEND       = 0,
  MP_QTV_SETTING_RESUME,
  MP_QTV_SETTING_MAX
}MP_QTV_SETTING_TYPE;

  CMPQTVSettingPanel(void* pOwner);
  virtual ~CMPQTVSettingPanel();

  boolean Init(AEERect rc);

  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);

protected:
  CMPLabel                m_NoteLabel;
  AECHAR*                 m_pNoteString;

  AECHAR*                 m_NameString[MP_QTV_SETTING_MAX];
  CMPSymbol*              m_pSymbol[MP_QTV_SETTING_MAX];
  CMPListWin<CMPSymbol>*  m_pListWin;

  void Cleanup();
  void OnSelect();
};

#endif //__cplusplus
#endif //MPQTVSETTINGPANEL_H
