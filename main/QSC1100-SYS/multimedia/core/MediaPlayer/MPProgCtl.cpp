/*===========================================================================
  FILE: CMPProgCtl.cpp

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
 $Header: //depot/asic/msmshared/apps/mediaplayer/MPProgCtl.cpp#6 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPProgCtl.h"
#include "MPUtils.h"
#include "MPDefines.h"

#ifdef FEATURE_MMOVERLAY
#include "AEEMMOverlay.h"
#endif // FEATURE_MMOVERLAY

/*===========================================================================

                  CMPProgCtl "C" Interface functions

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
    Ptr to CMPProgCtl object = NULL if failed to create.

Side Effects:
    ISTATIC object created for Message area text.

-----------------------------------------------------------------------*/

CMPProgCtl *MPProgCtl_Create(IShell *pIShell, IDisplay *pIDisplay, void *pOverlay, AEERect rectInfo)
{
  //DBGPRINTF_MED("MP: MPProgCtl_Create");

  CMPProgCtl *pMPProgCtl = new CMPProgCtl(pIShell, pIDisplay, pOverlay, rectInfo);

  if (pMPProgCtl->GetCreationStatus() == true)
  {
    return pMPProgCtl;
  }
  else
  {
    delete pMPProgCtl;
    return NULL;
  }
}

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
    ISTAtic message text object is released.

-----------------------------------------------------------------------*/
void  MPProgCtl_Release(CMPProgCtl **ppMPProgCtl)
{
  //DBGPRINTF_MED("MP: MPProgCtl_Release, ppMPProgCtl=0x%x, *ppMPProgCtl=0x%x",
  //               ppMPProgCtl, *ppMPProgCtl);

  if (ppMPProgCtl && *ppMPProgCtl) 
  {
    delete (*ppMPProgCtl);
    *ppMPProgCtl = NULL;
  }
}

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
void MPProgCtl_Update(CMPProgCtl *pMPProgCtl, IBitmap *pIcon, boolean bShowIcon, 
                      AECHAR *szMessageText, boolean bIsTimedMsg, int nPct)
{
  //DBGPRINTF_MED("MP: MPProgCtl_Update: pMPProgCtl=0x%x, pIcon=0x%x, bShowIcon=%d, szMessageText=0x%x, bIsTimedMsg=%d, nPct=%d",
  //               pMPProgCtl, pIcon, bShowIcon, szMessageText, bIsTimedMsg, nPct);

  bool bIcon = (bShowIcon == TRUE);
  bool bTimedMsg = (bIsTimedMsg == TRUE);

  if (pMPProgCtl) 
  {
    pMPProgCtl->Update(pIcon, bIcon, szMessageText, bTimedMsg, nPct);
  }
}

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
void MPProgCtl_TimedMessage(CMPProgCtl *pMPProgCtl, AECHAR *szMessageText)
{
  //DBGPRINTF_MED("MP: MPProgCtl_TimedMessage: pMPProgCtl=0x%x, szMessageText=0x%x",
  //              pMPProgCtl, szMessageText);

  bool bIsTimedMessage = true;

  if (pMPProgCtl) 
  {
    pMPProgCtl->UpdateMessage(szMessageText, bIsTimedMessage);
  }
}

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
void  MPProgCtl_MessageTimerCancel(CMPProgCtl *pMPProgCtl)
{
  //DBGPRINTF_MED("MP: MPProgCtl_MessageTimerCancel, pMPProgCtl=0x%x", pMPProgCtl);

  if (pMPProgCtl) 
  {
    pMPProgCtl->MessageTimerCancel();
  }
}

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
void  MPProgCtl_MessageTimerNotify(void *pUser)
{
  CMPProgCtl *pMPProgCtl;

  pMPProgCtl = (CMPProgCtl *)pUser;

  //DBGPRINTF_MED("MP: MPProgCtl_MessageTimerNotify, pMPProgCtl=0x%x", pMPProgCtl);

  if (pMPProgCtl) 
  {
    pMPProgCtl->MessageTimerNotify();
  }
}

/*===========================================================================

                     CMPProgCtl Methods

========================================================================== */

/*---------------------------------------------------------------------

Method: CMPProgCtl 

Description: This constructor creates and initializes an instance of CMPProgCtl.
    CMPProgCtl contains four rectangles: the main containing rectangle and within
    it, the icon rectangle, the message rectangle, and the progress bar rectangle.

Parameters:
    pIShell     Ptr to IShell object to be used by CMPProgCtl instance
    pIDisplay   Ptr to IDisplay to be used by CMPProgCtl instance
    pOverlay    Ptr to IMMOverlay object to be used by CMPProgCtl, if Overlays are enabled.
    rectInfo    Rectangle describing screen area for CMPProgCtl display info.

Return:
    CMPProgCtl object.

Side Effects:
    ISTATIC object created for Message area text.  Must be released by destructor.

-----------------------------------------------------------------------*/
CMPProgCtl::CMPProgCtl(IShell *pIShell, IDisplay *pIDisplay, void *pOverlay, AEERect rectInfo)
{
  int       x, y, dx, dy;
  int       nRet;

  //DBGPRINTF_MED("MP: CMPProgCtl: pIShell=0x%x, pIDisplay=0x%x, pOverlay=0x%x, rectInfo: x=%d, y=%d, dx=%d, dy=%d",
  //              pIShell, pIDisplay, pOverlay, rectInfo.x, rectInfo.y, rectInfo.dx, rectInfo.dy);

  m_bObjStatus = true;
  m_pIShell = pIShell;
  m_pIDisplay = pIDisplay;
  m_pOverlay = pOverlay;

  /*
   * Verify that rectangle is valid.
   * If not, set object creation status to false
   */
  if (!MP_isValidRect(rectInfo, MP_PROGCTL_MIN_HEIGHT, MP_PROGCTL_MIN_WIDTH))
  {
    m_bObjStatus = false;
    DBGPRINTF_ERROR("MP: Invalid Rectangle rectInfo: x=%d, y=%d, dx=%d, dy=%d, minHeight=%d, minWidth=%d",
                    rectInfo.x, rectInfo.y, rectInfo.dx, rectInfo.dy, MP_PROGCTL_MIN_HEIGHT, MP_PROGCTL_MIN_WIDTH);
    return;
  }
            
  /*
   * Set rectangle for Progress Bar
   */
  (void)MEMCPY(&m_rectMain, &rectInfo, sizeof(AEERect));
  dx = m_rectMain.dx / MP_PROGCTL_BAR_DX_DIV;
  dy = m_rectMain.dy / MP_PROGCTL_BAR_DY_DIV;
  x = m_rectMain.x + m_rectMain.dx - dx + 1;
  y = m_rectMain.y + (m_rectMain.dy/2) - (dy/2);

  if ((dx - MP_PROGCTL_BAR_DX_OFFSET) < MP_PROGCTL_BAR_MIN_WIDTH)
  {
    dx = MP_PROGCTL_BAR_MIN_WIDTH;
  }
  SETAEERECT(&m_rectProgBar, x, y, dx, dy);
  //DBGPRINTF_MED("MP: CMPProgCtl: m_rectProgBar: x=%d, y=%d, dx=%d, dy=%d",
  //              m_rectProgBar.x, m_rectProgBar.y, m_rectProgBar.dx, m_rectProgBar.dy);

  /*
   * Initialize icon object:  
   * Set rectangle for Icon, making it square, based on the progress bar height.  
   */
  m_pIcon = NULL;
  SETAEERECT(&m_rectIcon, m_rectMain.x, m_rectMain.y, m_rectMain.dy, m_rectMain.dy);
  //DBGPRINTF_MED("MP: CMPProgCtl: m_rectIcon: x=%d, y=%d, dx=%d, dy=%d",
  //              m_rectIcon.x, m_rectIcon.y, m_rectIcon.dx, m_rectIcon.dy);

  /*
   * Initialize Message object:
   *   Create Static message object for display.
   *   Set initial values for Messages. 
   *   Set display rectangle for message.
   */
  nRet = ISHELL_CreateInstance(pIShell, AEECLSID_STATIC, (void **)&m_pMessage);
  if (SUCCESS != nRet)
  {
    m_pMessage = NULL;
    m_bObjStatus = false;
    DBGPRINTF_ERROR("MP: ISHELL_CreateInstance() returns %d = %s", nRet, MP_ErrorStateToString(nRet));
    return;
  }

  for (int i=0; i<MP_MSG_MAX_STRLEN; i++)
  {
    m_szMessageText[i] = (AECHAR) 0;
    m_szTimedMessageText[i] = (AECHAR) 0;
  }
  m_bShowingTimedMessage = FALSE;

  if (m_pMessage)
  {
    SETAEERECT(&m_rectMessage, m_rectMain.x + m_rectIcon.dx, m_rectMain.y + 1, 
               m_rectMain.dx - dx - m_rectIcon.dx, m_rectMain.dy - 1);
    //DBGPRINTF_MED("MP: CMPProgCtl: m_rectMessage: x=%d, y=%d, dx=%d, dy=%d",
    //              m_rectMessage.x, m_rectMessage.y, m_rectMessage.dx, m_rectMessage.dy);
    ISTATIC_SetRect(m_pMessage, &m_rectMessage);
    ISTATIC_SetProperties(m_pMessage, ST_CENTERTEXT | ST_NOSCROLL);
  }
}

/*---------------------------------------------------------------------

Method: ~CMPProgCtl 

Description: This destructor deletes instance of CMPProgCtl.  It also makes sure
    that the Message Timer is stopped and it releases the Message STATIC test object.

Parameters:
    None

Return:
    CMPProgCtl object.

Side Effects:
    ISTATIC object created for Message area text.  Must be released by destructor.

-----------------------------------------------------------------------*/
CMPProgCtl::~CMPProgCtl(void)
{
  MessageTimerCancel();
  if (m_pMessage)
  {
    ISTATIC_Release(m_pMessage);
    m_pMessage = NULL;
  }
}

/*---------------------------------------------------------------------

Method: Update 

Description: This function updates the information shown in the Progress Bar
    area and then updates the display.

Parameters:
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
void
CMPProgCtl::Update(IBitmap *pIcon, bool bShowIcon, AECHAR *szMessageText, bool bIsTimedMsg, int nPct)
{
  //DBGPRINTF_MED("MP: CMPProgCtl::Update");

  UpdateMessage(szMessageText, bIsTimedMsg);
  UpdateProgBar(nPct);
  UpdateIcon(pIcon, bShowIcon);

  // Draw frame around progress bar area 
  (void)MP_FrameRect(m_pIDisplay, &m_rectMain);

  UpdateDisplay();
}

/*---------------------------------------------------------------------

Method: UpdateMessage 

Description: This method updates the Message content and starts the message 
    timer if needed.

Parameters:
    szMessageText- Message to be displayed
    bIsTimedMsg  - true = Message is a timed message.

Return:
    None.

Side Effects:
    Message timer started if timed message.

-----------------------------------------------------------------------*/void
CMPProgCtl::UpdateMessage(AECHAR *szMessageText, bool bIsTimedMsg)
{
  //DBGPRINTF_MED("MP: CMPProgCtl::UpdateMessage: szMessageText='%s', bIsTimedMsg=%d", 
  //               DbgGetMessageStr(szMessageText), bIsTimedMsg);

  if ((szMessageText != NULL) && (m_pMessage != NULL))
  { 
    if (bIsTimedMsg)
    {
      // Update Timed Message text, restart timer, and show message
      MessageTimerCancel();
      TimedMessageText(szMessageText);
      ShowMessage(m_szTimedMessageText);
      MessageTimerStart();
      m_bShowingTimedMessage = true;
    }
    else
    {
      // Update Normal Message text and show message 
      // if not currently showing timed message
      MessageText(szMessageText);
      if (!m_bShowingTimedMessage)
      {
        ShowMessage(m_szMessageText);
      }
    }
  }
}

/*---------------------------------------------------------------------

Method: MessageText

Description: This method copies the specified message text to the 
    m_szMessageText member variable.

Parameters:
    msg - Message text to be copied

Return:
    None.

Side Effects:
    m_szMessageText updated.

-----------------------------------------------------------------------*/
void 
CMPProgCtl::MessageText(AECHAR *msg)
{
  (void)WSTRLCPY(m_szMessageText, msg, sizeof(m_szMessageText));
}

/*---------------------------------------------------------------------

Method: TimedMessageText

Description: This method copies the specified timed message text to the 
    m_szTimedMessageText member variable.

Parameters:
    msg - Message text to be copied

Return:
    None.

Side Effects:
    m_szTimedMessageText updated.

-----------------------------------------------------------------------*/
void 
CMPProgCtl::TimedMessageText(AECHAR *msg)
{
  (void)WSTRLCPY(m_szTimedMessageText, msg, sizeof(m_szTimedMessageText));
}

/*---------------------------------------------------------------------

Method: ShowMessage

Description: This method sizes the STATIC text display object for the 
    Message Text and then updates the Message display.

Parameters:
    szMessageText - Message text to be displayed.

Return:
    None.

Side Effects:
    m_pMessage - STATIC text object updated with specified text

-----------------------------------------------------------------------*/
void
CMPProgCtl::ShowMessage(AECHAR *szMessageText)
{
  if ((m_pIDisplay != NULL) && (m_pMessage != NULL))
  {
    IDISPLAY_EraseRect(m_pIDisplay, &m_rectMessage);
    (void)MP_FitStaticTextNormal(m_pIDisplay, m_pMessage, szMessageText);
    (void)ISTATIC_Redraw(m_pMessage);
  }
}

/*---------------------------------------------------------------------

Method: UpdateProgBar

Description: This method updates the progress percentage and then updates
   the progress bar display.

Parameters:
    nProgBarPct - Completion percentage.

Return:
    None.

Side Effects:
    m_nProgBarPct - Updated with new completion percentage.

-----------------------------------------------------------------------*/
void
CMPProgCtl::UpdateProgBar(int nProgBarPct)
{
  //DBGPRINTF_MED("MP: CMPProgCtl::UpdateProgBar: nProgBarPct=%d", nProgBarPct);

  /*
   * Make sure Progress Bar Percentage is in range
   */
  if (nProgBarPct < 0)
  {
    nProgBarPct = 0;
  }
  if (nProgBarPct > 100)
  {
    nProgBarPct = 100;
  }
  m_nProgBarPct = nProgBarPct;

  /* 
   * If valid rectangle, draw progress bar 
   */
  if ((m_pIDisplay != NULL) && (MP_isValidRect(m_rectProgBar, MP_PROGCTL_BAR_MIN_HEIGHT, MP_PROGCTL_BAR_MIN_WIDTH)))
  {
    AEERect   rc;

    (void)MEMCPY(&rc, &m_rectProgBar, sizeof(AEERect));
    IDISPLAY_EraseRect(m_pIDisplay, &rc);
    (void)IDISPLAY_DrawFrame(m_pIDisplay, &rc, AEE_FT_RAISED, CLR_SYS_SCROLLBAR);
    rc.dx = (rc.dx *m_nProgBarPct) / 100;
    IDISPLAY_FillRect(m_pIDisplay, &rc, MP_CLR_PROG_FILL);
  }
}

/*---------------------------------------------------------------------

Method: UpdateIcon

Description: This method updates the m_pIcon member with the specified
    icon bitmap and then displays the icon if enabled.

Parameters:
    pIcon     - Ptr to IBitmap object
    bShowIcon - True = Display this icon

Return:
    None.

Side Effects:
    m_pIcon     - Updated with specified bitmap
    m_bShowIcon - Set to value input.

-----------------------------------------------------------------------*/
void
CMPProgCtl::UpdateIcon(IBitmap *pIcon, bool bShowIcon) 
{
  //DBGPRINTF_MED("MP: CMPProgCtl::UpdateIcon: pIcon=0x%x, bShowIcon=%d", pIcon, bShowIcon);

  m_pIcon = pIcon;
  m_bShowIcon = bShowIcon;

  if ((m_pIcon != NULL) && (MP_isValidRect(m_rectIcon, MP_PROGCTL_ICON_MIN_HEIGHT, MP_PROGCTL_ICON_MIN_WIDTH)))
  {
    if (m_bShowIcon)
    {
      DrawIcon();
    }
    else
    {
      EraseIcon();
    }
  }
}

/*---------------------------------------------------------------------

Method: DrawIcon

Description: This method centers tee icon in the icon region of the display.  
    If the icon bitmap is larger than the available space, the icon is clipped.

Parameters:
    None

Return:
    None.

Side Effects:
    Display is updated.

-----------------------------------------------------------------------*/
void
CMPProgCtl::DrawIcon()
{
  AEERect       srcRect, dstRect;
  AEEBitmapInfo bi;

  if (NULL == m_pIcon)
  {
      DBGPRINTF_MED("MP: No icon to draw");
  }  
  else
  {
    // Get size info for the icon bitmap
    (void)IBITMAP_GetInfo(m_pIcon, &bi, sizeof(bi));

    // Center the icon in the icon field.
    // If the icon is larger than the icon field, clip it.
    if (m_rectIcon.dx >= bi.cx)
    {
      dstRect.dx = bi.cx;
      dstRect.x = m_rectIcon.x + (m_rectIcon.dx - bi.cx)/2;
      srcRect.x = 0;
    }
    else
    {
      dstRect.dx = m_rectIcon.dx;
      dstRect.x = m_rectIcon.x;
      srcRect.x = (bi.cx - m_rectIcon.dx)/2;
    }

    if (m_rectIcon.dy >= bi.cy)
    {
      dstRect.dy = bi.cy;
      dstRect.y = m_rectIcon.y + (m_rectIcon.dy - bi.cy)/2;
      srcRect.y = 0;
    }
    else
    {
      dstRect.dy = m_rectIcon.dy;
      dstRect.y = m_rectIcon.y;
      srcRect.y = (bi.cy - m_rectIcon.dy)/2;
    }

    if (m_pIDisplay != NULL)
    {
      IDISPLAY_BitBlt(m_pIDisplay, dstRect.x, dstRect.y, dstRect.dx, dstRect.dy, 
                      m_pIcon, srcRect.x, srcRect.y, AEE_RO_COPY);
    }
  }
}
 
/*---------------------------------------------------------------------

Method: EraseIcon

Description: This method erases the current Icon rectangle.

Parameters:
    None

Return:
    None.

Side Effects:
    Display is updated.

-----------------------------------------------------------------------*/
void
CMPProgCtl::EraseIcon()
{
  //DBGPRINTF_MED("MP: CMPProgCtl::EraseIcon");

  if (m_pIDisplay != NULL)
  {
    IDISPLAY_EraseRect(m_pIDisplay, &m_rectIcon);
  }
}

/*---------------------------------------------------------------------

Method: UpdateDisplay

Description: This method updates the display.  If Overlays are enabled,
    IMMOverlay methods are used.  Otherwise, IDisplayUpdate() is used.

Parameters:
    None

Return:
    None.

Side Effects:
    Display updated.

-----------------------------------------------------------------------*/
void
CMPProgCtl::UpdateDisplay(void)
{
  //DBGPRINTF_MED("MP: CMPProgCtl::UpdateDisplay");
  /*
   * When overlay mode is enabled,
   * use IMMOverlay_Update to update display.
   */
  if (m_pOverlay != NULL) 
  {
#ifdef FEATURE_MMOVERLAY
    uint32 nRet;

    nRet = IMMOVERLAY_UpdateAsync((IMMOverlay *)m_pOverlay, NULL, 0);
    if (nRet == MP_ERR_IMMOVERLAY)
    {
      DBGPRINTF_ERROR("MP: IMMOVERLAY_Update Failed, nRet=%d", nRet);
    }
#endif // FEATURE_MMOVERLAY
  }
  else if (m_pIDisplay != NULL)
  {
    IDISPLAY_Update(m_pIDisplay);
  }
}


/*===========================================================================
                      Methods for Message Timer
===========================================================================*/

/*---------------------------------------------------------------------

Method: MessageTimerStart

Description: This function starts the timer for Timed Messages.

Parameters:
    None

Return:
    None.

Side Effects:
    None.

-----------------------------------------------------------------------*/
void 
CMPProgCtl::MessageTimerStart()
{
  //DBGPRINTF_MED("MP: MessageTimerStart");

  if (m_pIShell != NULL)
  {
    (void)ISHELL_SetTimer(m_pIShell, MP_MSG_TIMER_DEFAULT, (PFNNOTIFY)MPProgCtl_MessageTimerNotify, this);
  }
}

/*---------------------------------------------------------------------

Method: MessageTimerCancel

Description: This function cancels the timer for Timed Messages.  Subsequent
    non-timed messages will then be immediately shown.

Parameters:
    None

Return:
    None.

Side Effects:
    m_bShowingTimedMessage set to false.

-----------------------------------------------------------------------*/
void 
CMPProgCtl::MessageTimerCancel()
{
  //DBGPRINTF_MED("MP: MessageTimerCancel");

  if (m_pIShell != NULL)
  {
    (void)ISHELL_CancelTimer(m_pIShell, (PFNNOTIFY)MPProgCtl_MessageTimerNotify, this);
  }
  m_bShowingTimedMessage = false;
}

/*---------------------------------------------------------------------

Method: MessageTimerNotify

Description: This method is called when the message timer expires. When this
    method is called, the Timed Message is replace with the current non-timed
    message.

Parameters:
    None

Return:
    None.

Side Effects:
    m_bShowingTimedMessage set to false.
    Display is updated to show current non-timed message.

-----------------------------------------------------------------------*/
void
CMPProgCtl::MessageTimerNotify()
{
  //DBGPRINTF_MED("MP: MessageTimerNotify");

  m_bShowingTimedMessage = false;
  if (m_szMessageText)
  {
    UpdateMessage(m_szMessageText, false);
    UpdateDisplay();
  }
}

/*---------------------------------------------------------------------

Method: GetCreationStatus

Description: This method returns status of object's creation.

Parameters:
    None

Return:
    true = Object created successfully.

Side Effects:
    None

-----------------------------------------------------------------------*/
bool 
CMPProgCtl::GetCreationStatus()
{
  return m_bObjStatus;
}

/*---------------------------------------------------------------------

Method: dbgGetMessageStr

Description: This method converts wide string into a standard string
    and returns a ptr to it.

Parameters:
    szWStr - Null terminated Wide string.

Return:
    char * - Ptr to null terminated standard string.  Max length is MP_MSG_MAX_STRLEN.

Side Effects:
    Returned string may be truncated if it is longer than MP_MSG_MAX_STRLEN.

-----------------------------------------------------------------------*/
//static char DbgMessageStr[MP_MSG_MAX_STRLEN];
//char *
//CMPProgCtl::DbgGetMessageStr(AECHAR *szWStr)
//{
//  WSTRTOSTR(szWStr, DbgMessageStr, sizeof(DbgMessageStr));
//  return DbgMessageStr;
//}
