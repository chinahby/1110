/*=============================================================================
  FILE: MPMediaTrackPanel.h

  SERVICES: this file defines MPMediaTrackPanel class objects
 
  GENERAL DESCRIPTION:


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPMediaTrackPanel.h#1 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPMEDIATRACKSPANEL_H
#define MPMEDIATRACKSPANEL_H

#include "MPDefines.h"

#ifdef __cplusplus

#include "MPListWin.h"
#include "MPSymbols.h"


class CMPMediaTracksPanel : public CMPSymbol
{
public:

typedef enum MP_LABEL_TYPE
{
  MP_LABEL_TRACK_SELECT = 0,
  MP_LABEL_TRACK_MUTE,
  MP_LABEL_TRACK_MAX
}MPLABELTYPE;

  CMPMediaTracksPanel(void* pOwner);
  virtual ~CMPMediaTracksPanel();

  boolean Init(AEERect rc);

  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);

protected:

  AECHAR*                 m_NameString[MP_LABEL_TRACK_MAX];
  CMPSymbol*              m_pSymbol[MP_LABEL_TRACK_MAX];
  CMPListWin<CMPSymbol>*  m_pListWin;        

  CMPSymbol*              m_pSelectTrack;   //point to track selection panel
  CMPSymbol*              m_pMuteTrack;     // point to track mute panel
  void Cleanup();
  void OnSelect();
};

#endif //__cplusplus
#endif //MPMEDIATRACKSPANEL_H
