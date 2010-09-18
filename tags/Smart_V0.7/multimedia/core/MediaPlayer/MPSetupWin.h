/*=============================================================================
  FILE: MPSetupWin.h

  SERVICES: CMPSetupWin class object
 
  GENERAL DESCRIPTION: This panel contains panel or panels to show spectrum analyzer
	data graph and/or setting panel


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPSetupWin.h#6 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPSETUPWIN_H
#define MPSETUPWIN_H

#include "MPDefines.h"
#ifdef FEATURE_MP_EQ_UI
#include "MPSymbols.h"
#include "MPEqualizerPanel.h"
#include "MPAnalyzerPanel.h"
#include "MPSetupPanel.h"

#include "MPSettings.h"

#ifdef __cplusplus

class CMPSetupWin : public CMPObject
{
public:
  CMPSetupWin();
  CMPSetupWin(IShell* pShell, IDisplay* pDisplay, MPSetting* pSetting, AEERect& rc, int min, int max);
  virtual ~CMPSetupWin();

  // create CMPSetupWin object
  static CMPSetupWin* Create(IShell* pShell, IDisplay* pDisplay, MPSetting* pSetting, AEERect& rc, int min, int max);
  void Destroy();

  // set analyzer data
  void SetData(int16* pdata, int length);
	// hide/show UI
  void Enable(boolean bMode);
  boolean Enable();

  // draw analyzer panel
  boolean DrawAnalyzer(IGraphics *pg);    
  // draw panels
  boolean Draw(IGraphics *pg, boolean bDrawAnalyzer);
  virtual boolean Draw(IGraphics *pg);
  // set forcus
  virtual void SetFocus(boolean mode);
  // event handler
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);

  // set max data value for spectrum analyzer graph
  void SetMaxAnalyzerDataValue(int nValue);
  void RefreshAudioSettings();

  boolean GetSettingPanelRect(AEERect* pRc);

  boolean ShowSettingPanel();
protected:
  //CMPEqualizerPanel* m_pEqPanel;       // equalizer sub panel
  CMPAnalyzerPanel*  m_pAnalyzerPanel; // analyzer sub panel
  CMPSetupPanel*   m_pSetupPanel;

  boolean          m_bEnable;          // Hide/Show drawing
  boolean          m_bObjStatus;       //CMPSetupWin object creation status

  AEERect          m_SetupPanelRc;        //equalizer sub panel rc
  AEERect          m_AnalyzerRc;       //analyzer sub panel rc
  
  boolean Init(AEERect rc, int min, int max);
  boolean BuildFilterSpec();
  boolean GetCreationStatus();
  void Cleanup();
};

#else
  typedef struct CMPSetupWin CMPSetupWin;
#endif

#ifdef __cplusplus
extern "C" {
#endif

// MPSetupWinCreate
//  pShell, brew shell ptr
//  pDisplay, brew display ptr
//  pSetting, point to MPSetting object
//  rc, setup screen rect
//  min, min value of EQ slider
//  max, max value of EQ slider
extern CMPSetupWin* MPSetupWinCreate(IShell* pShell, IDisplay* pDisplay, MPSetting* pSetting, AEERect rc, int min, int max);
extern void MPSetupWinDelete(CMPSetupWin* obj);
// enable/disable setup screen, screen will be displayed only when it is enabled
extern void MPSetupWinSetEnable(CMPSetupWin* pObj, boolean bMode);
extern boolean MPSetupWinGetEnable(CMPSetupWin* pObj);

// set/unset input key focus to setup screen
extern void MPSetupWinSetFocus(CMPSetupWin* pObj, boolean bFocus);
extern boolean MPSetupWinGetFocus(CMPSetupWin* pObj);

// set or update spectrum analyzer data
extern void MPSetupWinSetSpectrumData(CMPSetupWin* pObj, int16* pdata, int length);
// update analyzer graph drawing
extern boolean MPSetupWinDrawAnalyzer(CMPSetupWin* pObj, IGraphics *pg);

// redraw setup screen
extern boolean MPSetupWinDraw(CMPSetupWin* pObj, IGraphics *pg, boolean bHasVideo);
// handle key event, return FALSE for not handled
extern int MPSetupWinOnEvent(CMPSetupWin* pObj, AEEEvent eCode, uint16 wParam, uint32 dwParam);
// set max data value for analyzer graph
extern void MPSetupWinSetMaxAnalyzerData(CMPSetupWin* pObj, int nValue);
// load defalut filter and audio expender settings
extern void MPSetupWinRefreshAudioSettings(CMPSetupWin* pObj);
// return setting panel rectangle
extern boolean MPSetupWinGetSettingPanelRect(CMPSetupWin* pObj, AEERect* pRc);
#ifdef __cplusplus
}
#endif

#endif // FEATURE_MP_EQ_UI
#endif //MPSETUPWIN_H
