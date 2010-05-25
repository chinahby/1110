/*=============================================================================
  FILE: MPAudioAngledMixing.h

  SERVICES: defines CMPAudioAngledMixing class
 
  GENERAL DESCRIPTION: The panel is used to set 3D audio expander


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPAudioAngledMixing.h#2 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPAUDIOPANGLEDMIXING_H
#define MPAUDIOPANGLEDMIXING_H

#include "MPListWin.h"
#include "MPSymbols.h"

#ifdef __cplusplus
class CMPAudioAngledMixing : public CMPSymbol
{
public:
typedef enum MP_LABEL_TYPE
{
  MP_EDIT_CLASS = 0,
  MP_EDIT_GAIN,
  MP_EDIT_ANGLE,
  MP_EDIT_TIME,
  MP_BUTTON_COMMIT,
  MP_LABEL_MAX
}MPLABELTYPE;

  CMPAudioAngledMixing(void* pOwner);
  virtual ~CMPAudioAngledMixing();
  void Destroy();

  boolean Init(AEERect rc);
  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);

protected:
  AECHAR*                 m_pwszNameString;
  int                     m_nNumofEditItems;  // number of labels
  CMPNumEdit*             m_pLabels;          // pts to all labels
  CMPLabel*               m_pCommit;          //commit button
  CMPListWin<CMPSymbol>*  m_pListWin;          // list win ptr, contain all labels
  
  // return the max length of labels in pixel
  int  GetMaxLabelLength();
  // calculate the width of a given string
  int  GetStringWidth(const char* pstr);
  // update data
  void Commit();
};
#endif //__cplusplus

#endif //MPAUDIOPANGLEDMIXING_H
