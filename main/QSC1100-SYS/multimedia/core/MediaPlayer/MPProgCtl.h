/*===========================================================================
  FILE: CMPProgCtl.h

  CLASS: CMPProgCtl

  DESCRIPTION:
  This class manages objects that contain and display information  
  about the current playback session.  This information is displayed in the
  progress bar area and consists of a possible icon, message text, and 
  progress bar.

  Copyright @2007 QUALCOMM Incorporated. All Rights Reserved.
  QUALCOMM Proprietary/GTDR

========================================================================== */

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 Revision:
 $Header: //depot/asic/msmshared/apps/mediaplayer/MPProgCtl.h#4 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MP_PROG_CTL
#define MP_PROG_CTL

#include "AEEComdef.h"
#include "AEE.h"
#include "AEEBitmap.h"
#include "AEEDisp.h"
#include "AEEShell.h"

#define MP_PROGCTL_BAR_DX_DIV     3        // dx is third of main rectangle
#define MP_PROGCTL_BAR_DY_DIV     2        // dy is half of main rectangle
#define MP_PROGCTL_BAR_DX_OFFSET  4        // dx is reduced by offset
#define MP_CLR_PROG_FILL          MAKE_RGB(0, 128, 192)
#define MP_MSG_MAX_STRLEN         64
#define MP_MSG_TIMER_DEFAULT      2500     // Show timed message for 2.5 secs
/*
 * Minimum Rectangle sizes
 */
#define MP_PROGCTL_ICON_MIN_HEIGHT 10
#define MP_PROGCTL_ICON_MIN_WIDTH  10
#define MP_PROGCTL_BAR_MIN_HEIGHT   5
#define MP_PROGCTL_BAR_MIN_WIDTH   20
#define MP_PROGCTL_MSG_MIN_HEIGHT  10
#define MP_PROGCTL_MSG_MIN_WIDTH   (MP_PROGCTL_BAR_MIN_WIDTH*(MP_PROGCTL_BAR_DX_DIV - 1) - MP_PROGCTL_ICON_MIN_WIDTH)
#define MP_PROGCTL_MIN_HEIGHT      (MP_PROGCTL_MSG_MIN_HEIGHT + 2)
#define MP_PROGCTL_MIN_WIDTH       (MP_PROGCTL_MSG_MIN_WIDTH + MP_PROGCTL_BAR_MIN_WIDTH + MP_PROGCTL_ICON_MIN_WIDTH)

#ifdef __cplusplus

class CMPProgCtl
{
public:
  CMPProgCtl(IShell *pIShell, IDisplay *pIDisplay, void *pOverlay, AEERect rectInfo);
  virtual ~CMPProgCtl(void);
  void Update(IBitmap *pIcon, bool bShowIcon, 
              AECHAR *szMessageText, bool bIsTimedMsg, int nPct);
  void UpdateMessage(AECHAR *szMessageText, bool bIsTimedMsg);
  void UpdateProgBar(int nProgBarPct);
  void UpdateIcon(IBitmap *pIcon, bool bShowIcon);
  void MessageTimerCancel();
  void MessageTimerNotify();
  bool GetCreationStatus();

private:
  void TimedMessageText(AECHAR *msg);
  void MessageText(AECHAR *msg);
  void ShowMessage(AECHAR *msg);
  void DrawIcon();
  void EraseIcon();
  void DrawProgBar();
  void UpdateDisplay();
  void MessageTimerStart();
  // char *DbgGetMessageStr(AECHAR *szMessageText);

private:
  /*
   * Progress control: includes progress bar, message, and record status
   */
  bool        m_bObjStatus;       // true = Object successfully created
  IShell     *m_pIShell;
  IDisplay   *m_pIDisplay;
  void       *m_pOverlay;
  AEERect     m_rectMain;         // Containing rectangle
  AEERect     m_rectIcon;         // Area for Record Icon or Timed Msg Icons
  IBitmap    *m_pIcon;            // Bitmap for icon to be displayed
  bool        m_bShowIcon;
  AEERect     m_rectMessage;      // Message text rectangle
  IStatic    *m_pMessage;
  AECHAR      m_szMessageText[MP_MSG_MAX_STRLEN];
  AECHAR      m_szTimedMessageText[MP_MSG_MAX_STRLEN];
  bool        m_bShowingTimedMessage;
  AEERect     m_rectProgBar;      // Progress Bar rectangle
  int         m_nProgBarPct;      // Percent complete
};

#else
  typedef struct CMPProgCtl CMPProgCtl;
#endif


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

 "C" language interface functions

========================================================================== */

/*---------------------------------------------------------------------

Function: MPProgCtl_Create

Description: This function creates an instance of the MPProgCtl object and
    returns a ptr to the object.

Parameters:
    pIShell    - Ptr to IShell object to be used by CMPProgCtl instance
    pIDisplay  - Ptr to IDisplay to be used by CMPProgCtl instance
    pOverlay   - Ptr to IMMOverlay object to be used by CMPProgCtl, if Overlays are enabled.
    rectInfo   - Rectangle describing screen area for CMPProgCtl display info.

Return:
    Ptr to CMPProgCtl object created.

Side Effects:
    ISTATIC object created for Message area text.

-----------------------------------------------------------------------*/
extern CMPProgCtl *MPProgCtl_Create(IShell *pIShell, IDisplay *pIDisplay, void *pOverlay, AEERect rectInfo);

/*---------------------------------------------------------------------

Function: MPProgCtl_Release

Description: This function deletes the CMPProgCtl object and resets its
    ptr to NULL. 

Parameters:
    pMPProgCtl - Ptr to  CMPProgCtl object to be released

Return:
    None.

Side Effects:
    pMPProgCtl - Ptr to CMPProgCtl set to NULL.
    Message timer for timed messages is canceled.
    ISTATIC message text object is released.

-----------------------------------------------------------------------*/
extern void MPProgCtl_Release(CMPProgCtl **pMPProgCtl);

/*---------------------------------------------------------------------

Function: MPProgCtl_Update

Description: This function updates the information shown in the Progress Bar
    area and then updates the display.

Parameters:
    pMPProgCtl   - Ptr to ptr to CMPProgCtl object to be released
    pRecIcon     - Ptr to IBitmap object to be shown in Icon area
    bShowRecIcon - TRUE = show icon.  FALSE = hide icon.
    szMessageText- Message to display in message area.
    bIsTimedMsg  - TRUE = timed message.  FALSE = message is not timed.
    nPct         - Percentage completed to be shown in progress bar.

Return:
    None.

Side Effects:
    Progess bar area contents updated and displayed.
    Message timer started for timed messages.

-----------------------------------------------------------------------*/
extern void MPProgCtl_Update(CMPProgCtl *pMPProgCtl, IBitmap *m_pRecIcon, boolean bShowRecIcon, 
                             AECHAR *szTimerText, boolean bIsTimedMsg, int m_wPct);

/*---------------------------------------------------------------------

Function: MPProgCtl_TimedMessage

Description: This function displays specified message for default (3 secs) 
    time period.

Parameters:
    pMPProgCtl   - Ptr to ptr to CMPProgCtl object to be released
    szMessageText- Message to display in message area.

Return:
    None.

Side Effects:
    Message timer started or restarted.

-----------------------------------------------------------------------*/
extern void MPProgCtl_TimedMessage(CMPProgCtl *pMPProgCtl, AECHAR *szMessageText);

/*---------------------------------------------------------------------

Function: MPProgCtl_MessageTimerCancel

Description: This function cancels the timer for Timed Messages.  Subsequent
    non-timed messages will then be immediately shown.

Parameters:
    pMPProgCtl - Ptr to ptr to CMPProgCtl object to be released

Return:
    None.

Side Effects:
    Message timer canceled.

-----------------------------------------------------------------------*/
extern void MPProgCtl_MessageTimerCancel(CMPProgCtl *pMPProgCtl);

/*---------------------------------------------------------------------

Function: MPProgCtl_MessageTimerNotify

Description: This is the callback function for the Message Timer.

Parameters:
    pUser   - Ptr to CMPProgCtl object

Return:
    None.

Side Effects:
    None.

-----------------------------------------------------------------------*/
extern void MPProgCtl_MessageTimerNotify(void *pUser);

#ifdef __cplusplus
}
#endif

#endif // MP_PROG_CTL
