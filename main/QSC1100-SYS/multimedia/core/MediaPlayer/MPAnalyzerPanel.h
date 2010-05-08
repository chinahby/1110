/*=============================================================================
  FILE: MPAnalyzerPanel.h

  SERVICES: MediaPlayer Spectrum Analyzer Graph Handler
 
  GENERAL DESCRIPTION:
  This file defines spectrum analyzer panel object which contains 
	bar graph used to show data value

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPAnalyzerPanel.h#5 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPANALYZERPANEL_H
#define MPANALYZERPANEL_H

#include "MPDefines.h"
#ifdef FEATURE_MP_EQ_UI

#include "MPSymbols.h"

#ifdef __cplusplus
class CMPAnalyzerPanel : public CMPSymbol
{
public:
  CMPAnalyzerPanel(void* pOwner);
	virtual ~CMPAnalyzerPanel();

	// set new data
  void SetData(int16* pData, int length);	
	virtual boolean Draw(IGraphics *pg);

  boolean Init(AEERect rc);
	// resize will take effict after calling on Draw() and SetData()
	//   SetData will recalculate new bar graph size
	//   if draw is called before SetData, display new fram without bar graph
  void Resize(AEERect& rc);
  // show/hide graph
  void Enable(boolean bMode);
  // set max data value for the graph
  void SetMaxValue(int nValue);
  void Clean();
protected:
  CMPBar*   m_pBars;       // bar objects
  int       m_MaxVal;
  int       m_nCount;      // bar count
  boolean   m_bEnable; 
  AEERect   m_rcFrameSave; // requested frame rc
	AEERect   m_rcResize;    // new rc

	boolean LayoutScreen(AEERect& rcPanel, int nBarCount);
	// get physical y value
  int GetY(int y);
};
#endif //__cplusplus
#endif //FEATURE_MP_EQ_UI
#endif
