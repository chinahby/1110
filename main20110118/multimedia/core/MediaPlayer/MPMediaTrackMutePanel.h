/*=============================================================================
  FILE: MPMediaTrackMutePanel.h

  SERVICES: this file defines CMPMediaTrackMutePanel class objects
 
  GENERAL DESCRIPTION:


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPMediaTrackMutePanel.h#3 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPMEDIATRACKMUTEPANEL_H
#define MPMEDIATRACKMUTEPANEL_H

#include "MPDefines.h"

#ifdef __cplusplus

#include "MPListWin.h"
#include "MPSymbols.h"

//void* operator new[](UINT size){return MALLOC(size);}

class CMPMediaTrackMutePanel : public CMPSymbol
{
public:

  CMPMediaTrackMutePanel(void* pOwner);
  virtual ~CMPMediaTrackMutePanel();

  boolean Init(AEERect rc);
  boolean CreateTrackItems(int nCount);
  void RemoveTrackItems();
  boolean AddTrackItem(int id, int nTrackID, boolean bMute, char* pDisp, boolean bSelect);
  // this function will set focus to the first selected item.
  void SetItemFocus();

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
  // Number of selections made by user
  // bounded by m_nItemCount
  int           m_nSelectionCount; 
  // Array to hold the selections made
  int           *m_aSelections; 

  CMPListWin<CMPLabel>*  m_pListWin;

  void Cleanup();
  void OnSelect();
  void SaveMute();
};

#endif //__cplusplus
#endif //MPMEDIATRACKMUTEPANEL_H
