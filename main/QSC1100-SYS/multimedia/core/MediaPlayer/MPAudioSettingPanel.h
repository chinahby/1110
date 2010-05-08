/*=============================================================================
  FILE: MPAudioSettingPanel.h

  SERVICES: this file defines CMPAudioSettingPanel class objects
 
  GENERAL DESCRIPTION:


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPAudioSettingPanel.h#6 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPAUDIOSETTINGPANEL_H
#define MPAUDIOSETTINGPANEL_H

#include "MPDefines.h"

#ifdef __cplusplus

#include "MPListWin.h"
#include "MPSymbols.h"

class CMPAudioSettingPanel : public CMPSymbol
{
public:
typedef enum MP_AUDIO_SETTING_TYPE
{
  MP_AUDIO_SETTING_TEMPO          = 0,
  MP_AUDIO_SETTING_QSYNTH_HYBRID,
  MP_AUDIO_SETTING_PAN,
  MP_AUDIO_SETTING_SPEC_INFO,
  MP_AUDIO_SETTING_MAX
}MP_AUDIO_SETTING_TYPE;

  CMPAudioSettingPanel(void* pOwner);
  virtual ~CMPAudioSettingPanel();

  boolean Init(AEERect rc);

  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);

protected:
  AECHAR*                 m_pNameString;
  CMPListWin<CMPSymbol>*  m_pListWin;

  void Cleanup();
  int  GetMaxLabelLength();
  void OnSelect(AEEEvent eCode, uint16 wParam, uint32 dwParam);
  void OnLeftRight(AEEEvent eCode, uint16 wParam, uint32 dwParam);
  boolean CanUpdateNoteSetting();
};

#endif //__cplusplus
#endif //MPAUDIOSETTINGPANEL_H
