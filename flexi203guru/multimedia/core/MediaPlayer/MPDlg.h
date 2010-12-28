/*=============================================================================
  FILE: MPDlg.h

  SERVICES: this file defines CMPDlg class
 
  GENERAL DESCRIPTION:


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright @2008 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPDlg.h#1 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPDLG_H
#define MPDLG_H
#include "AEEGraphics.h"

typedef void (*ButtonHandlerT)(void*);

typedef enum MP_DLG_BUTTON_TYPE
{
  MP_DLG_OK         = 0,
  MP_DLG_YES,
  MP_DLG_NO,
  MP_DLG_CANCEL,
  MP_DLG_MAX
}MP_DLG_BUTTON_TYPE;

#ifdef __cplusplus

#include "MPDefines.h"
#include "MPSymbols.h"

#define MP_BUTTON_COUNT    MP_DLG_MAX

class CMPDlg : public CMPObject
{
public:

  CMPDlg();
  CMPDlg(void* pOwner, IShell* pShell, IDisplay* pDisplay, AEERect* rc);
  virtual ~CMPDlg();

 // create CMPDlg object
  static CMPDlg* Create(void* pOwner, IShell* pShell, IDisplay* pDisplay, AEERect* rc);
  void Destroy();
  boolean GetCreationStatus();

  boolean Init(AEERect* rc);
  boolean IsDismiss();

  boolean AddButton(MP_DLG_BUTTON_TYPE bt);
  boolean SetButtonFocus(MP_DLG_BUTTON_TYPE type);
  boolean RegisterButtonhandler(ButtonHandlerT pfunc, MP_DLG_BUTTON_TYPE type);
  boolean SetText(const char* pstrTitle, const char* pstrText);

  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);
protected:
  void      *m_pOwner;       // owner ptr
  ButtonHandlerT buttonHandler[MP_DLG_MAX]; // button callback function ptr
  boolean    m_bObjStatus;   // object creation status
  boolean    m_bDismiss;     // object should be deleted

  CMPTextPanel *m_pTextPan;
  CMPLabel     *m_pBt[MP_BUTTON_COUNT];

  CMPLabel* GetButton(MP_DLG_BUTTON_TYPE type);
  const AECHAR* GetButtonName(MP_DLG_BUTTON_TYPE type);
  void LayoutButton(CMPLabel* pBt);
  int GetStringWidth(const AECHAR* pwstr);

  void OnSelect();
  void OnLeft();
  void OnRight();
};

#else
  typedef struct CMPDlg CMPDlg;
#endif //__cplusplus

#ifdef __cplusplus
extern "C" {
#endif

// Create/delete CMPDlg
extern CMPDlg* MPDlgCreate(void* pOwner, IShell* pShell, IDisplay* pDisplay, AEERect* rc);
extern void MPDlgDelete(CMPDlg* obj);

// Return the status if the dlg should be deleted
extern boolean MPDlgIsDismiss(CMPDlg* pObj);
// Redraw
extern boolean MPDlgUpdate(CMPDlg* pObj, IGraphics* pg);
// Return TRUE if message is handled, otherwise return FALSE
extern boolean MPDlgHandleEvent(CMPDlg* pObj, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// Set dlg text
extern boolean MPDlgSetText(CMPDlg* pObj, const char* pstrTitle, const char* pstrText);
// Add button and callback function handler with given type. 
// The order is from right to left on bottom of panel
extern boolean MPDlgAddButton(CMPDlg* pObj, MP_DLG_BUTTON_TYPE type, ButtonHandlerT pfunc);
// Set focus on a button. this will also remove focus from other if there is any
extern boolean MPDlgSetButtonFocus(CMPDlg* pObj, MP_DLG_BUTTON_TYPE type);
#ifdef __cplusplus
}
#endif

#endif // MPDLG_H

