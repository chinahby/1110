/*=============================================================================
  FILE: MPSetupPanel.h

  SERVICES: This file defines CMPSetupPanel class
 
  GENERAL DESCRIPTION: The panel contains a list of check box and buttons, which
    can be set by user.


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPSetupPanel.h#8 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPSETUPPANEL_H
#define MPSETUPPANEL_H

#include "MPDefines.h"

#ifdef FEATURE_MP_EQ_UI
#include "MPListWin.h"
#include "MPSymbols.h"


#ifdef __cplusplus
class CMPSetupPanel : public CMPSymbol
{
public:
typedef enum MP_LABEL_TYPE
{
  MP_LABEL_ENABLE_SPA = 0,
  MP_LABEL_SHOW_SPA,
  MP_LABEL_ENABLE_EQ,
  MP_LABEL_SETUP_EQ,
  MP_LABEL_ENABLE_EXP,
  MP_LABEL_SETUP_EXP,
  MP_LABEL_SETUP_QCONCERT_PRESET,
  MP_LABEL_ENABLE_ANGLE_MIXING,
  MP_LABEL_SETUP_ANGLE_MIXING,
  MP_LABEL_AUDIO_SETTINGS,
  MP_LABEL_TRACKS,
  MP_LABEL_QTV_TEST,
  MP_LABEL_ENABLE_BCAST,
  MP_LABEL_BCAST_SETTINGS,
  MP_PANEL_MAX
}MPLABELTYPE;

typedef enum INDENT_TYPE {
  INDENT_0 = 0,
  INDENT_1,
  INDENT_2,
  INDENT_3
}MPINDENT_TYPE; 

//enum Items { nCheckBoxs = 4, nLabels =4 };
public:
  CMPSetupPanel(void* pOwner);
  virtual ~CMPSetupPanel();

  boolean Init(AEERect rc, int nSliderMin, int nSliderMax);
	virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);
  
  void RefreshAudioSettings();
protected:
  static int            m_nCheckBoxs;
  static int            m_nLabels;

  AECHAR*               m_NameString[MP_PANEL_MAX];
  CMPLabel*             m_pLabels[MP_PANEL_MAX];
  CMPListWin<CMPLabel>* m_pListWin;
 
  CMPSymbol*            m_pSubPanel;            // sub panel
  boolean               m_bShowAnalyzer;
  int                   m_nSliderMin;
  int                   m_nSliderMax;

  // Create and initialize labeled checkbox control 
  boolean CreateCheckbox(AEERect rc, int iRow, int indent, int id, AECHAR* pName, boolean bState);
  // Create and initialize labeled button control 
  boolean CreateButton(AEERect rc, int iRow, int indent, int id, AECHAR* pName);

  // create sub panel by type
  CMPSymbol* CreatePanel(MPLABELTYPE type);
  boolean OnSelect(AEEEvent eCode, uint16 wParam, uint32 dwParam);
  boolean OnClear(AEEEvent eCode, uint16 wParam, uint32 dwParam);

  // toggle a label state between select and unselect.
  // note, there are some rules will be applied for the child label
  boolean ToggleLabelState(CMPLabel* pLabel);
  // enable/disable a label according to the selection status of the control label
  void    ApplyRules(MPLABELTYPE controlType, MPLABELTYPE labelType);
  // retrieve a liabel onject from give ID type
  CMPLabel* GetLabelFromID(MPLABELTYPE id);
  // rebuild list according to the item enable/disable status
  void ReBuildList();
  void ClearLabels();

  void SetFilter();
  void SetAudioExpander();
  void SetQConcertPreset();
  void SetAudioAngledMixing();
  void SetBcastSettings();
};

#endif //__cplusplus
#endif // FEATURE_MP_EQ_UI
#endif //MPSETUPPANEL_H
