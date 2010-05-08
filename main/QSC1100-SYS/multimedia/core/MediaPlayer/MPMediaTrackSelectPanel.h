/*=============================================================================
  FILE: MPMediaTrackSelectPanel.h

  SERVICES: this file defines CMPMediaTrackSelectPanel class objects
 
  GENERAL DESCRIPTION:


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPMediaTrackSelectPanel.h#3 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPMEDIATRACKSELECTPANEL_H
#define MPMEDIATRACKSELECTPANEL_H

#include "MPDefines.h"

#ifdef __cplusplus

#include "MPListWin.h"
#include "MPSymbols.h"

class CMPMediaTrackSelectPanel : public CMPSymbol
{
public:

  CMPMediaTrackSelectPanel(void* pOwner);
  virtual ~CMPMediaTrackSelectPanel();

  boolean Init(AEERect rc);
  boolean CreateTrackItems(int nCount);
  void RemoveTrackItems();
  boolean AddTrackItem(int id, int nTrackID, boolean bSelect, char* pDisp);

  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);

protected:
  class CTrackItem
  {
    public:
    CTrackItem():m_nID(0), m_pNameString(NULL),m_pLabels(NULL){}
    ~CTrackItem(){
      if(m_pNameString)
        delete[] m_pNameString;
      if(m_pLabels)
        delete m_pLabels;
    }

    boolean Add(int id, char* pstr){
      m_nID = id;
      if(m_pNameString)
        delete[] m_pNameString;
      m_pNameString = MP_ToAEEChar(pstr);

      if(!m_pLabels){
        m_pLabels = new CMPCheckBox;
        if(!m_pLabels)
          return FALSE;

        m_pLabels->SetName(m_pNameString);

      }
      return TRUE;
    };

    int        m_nID; 
    AECHAR*    m_pNameString;
    CMPLabel*  m_pLabels;
  };

  int           m_nItemCount;
  CTrackItem*   m_pItem;

  CMPListWin<CMPLabel>*  m_pListWin;

  void Cleanup();
  void OnSelect();
  void SaveSelections();
};

#endif //__cplusplus
#endif //MPMEDIATRACKSPANEL_H
