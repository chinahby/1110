/*===========================================================================
FILE: SSOperations_DialogCallDetails.c

SERVICES: SS Operations Dialog that displays Call Details

GENERAL DESCRIPTION:
    
PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================== */
#include "AEEShell.h"         // AEE Shell Services
#include "AEEText.h"          // AEE Text Services
#include "AEETime.h"
#include "AEESound.h"
#include "AEEStdLib.h"        // AEE StdLib Services

#include "GSM1xPhone.h"

#include "AutomationLogging.h"

#include "SSMsg.h"
#include "SSOperations_DialogCommon.h"
#include "SSOperations_DialogHandler.h"
#include "SSOperations_UserEvents.h"
#include "SSOperations_CommonMessageDefinitions.h"
#include "SSOperations_CommonEventDefinitions.h"


/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// Time in milliseconds between SSOperationsDialog_HandleCallDetailsTimer() callbacks
#define TIMEOUT_MS_CONV_TIMER                  (TIMEOUT_MS_ONE_SECOND)
#define TIMEOUT_STATIC_TEXT_SCROLLING          (3*TIMEOUT_MS_ONE_SECOND)

#define CALL_STATUS_TEXT_MAX_LENGTH 40
#define STATUS_TEXT_MAX_LENGTH  20
#define FORMAT_TEXT_MAX_LENGTH 20

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/

// Macro used to test if the CallDetails call info can be inspected/is available
#define IsCallDetailsCallInfoAvailable()                                   \
    SSCallDatabase_IsCallPresent(pSelf->m_pCallDatabase, pSelf->m_ForegroundCallIndex) 

// Macro used to get the CallDetails dialog call info
#define GetCallDetailsCallInfo()                                           \
    SSOperationsDialogHandler_GetCallInfo(pSelf, pSelf->m_ForegroundCallIndex)

// Macro used to get the CallDetails dialog call string
#define GetCallDetailsCallString()                                          \
    SSOperationsDialogHandler_GetCallString(pSelf, pSelf->m_ForegroundCallIndex)

// Macro used to get the CallDetails dialog call string
#define GetCallDetailsCallStartTime()                                       \
    SSOperationsDialogHandler_GetCallStartTime(pSelf, pSelf->m_ForegroundCallIndex) 

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

static void SSOperationsDialog_SetConvPhoneNumber(
    CSSOperationsDialogHandler *pSelf,
    IStatic                    *pNumberCtl
    );

static void SSOperationsDialog_SetConvCallStatus(  
    CSSOperationsDialogHandler *pSelf,
    ITextCtl                   *pStatusCtl
    );

static boolean SSOperationsDialog_SetConvCallName(  
    CSSOperationsDialogHandler *pSelf,
    IStatic                    *pNameCtl
    );

static void SSOperationsDialog_SetConvSoftkeyMenu(
    CSSOperationsDialogHandler *pSelf,
    IMenuCtl                   *pSkMenu
    );

static void SSOperationsDialog_SetupCallAudio(
    CSSOperationsDialogHandler *pSelf
    );

static void SSOperationsDialog_GetStatusStringRepresentation(
    SSCall_Status_type  callStatus,
    boolean             isMPTY,
    boolean             isHeld,
    IShell             *pShell, 
    AECHAR             *pText,
    uint8               textSize
    );
   
static boolean SSOperationsDialog_DoesCall1HaveHigherPriority(
    boolean            isCall1MPTY,
    boolean            isCall1Held,
    SSCall_Status_type call1Status,
    boolean            isCall2MPTY,
    boolean            isCall2Held,
    SSCall_Status_type call2Status
    );

static uint8 SSOperationsDialog_GetCallStatusPriority(
    boolean            isCallMPTY,
    boolean            isCallHeld,
    SSCall_Status_type callStatus
    );

static boolean SSOperationsDialog_CalculateCallInfo(
    CSSOperationsDialogHandler *pSelf
    );

// CallDetails Dialog timer callback
static void SSOperationsDialog_HandleCallDetailsTimer(void *pUser);

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialog_ConvHandleEvent

DESCRIPTION: Call Details dialog event handler (IDD_CALL_DETAILS)

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_ConvHandleEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
   switch (eCode) 
   {
   case EVT_DIALOG_INIT:
   {
      return TRUE;
   }

   case EVT_DIALOG_START:
   {
      ITextCtl *pTimeTextCtl;
      ITimeCtl *pTimerCtl;
      ITextCtl *pStatusCtl;
      IMenuCtl *pSkMenu;
      AEERect   rect;
      int       bOffset;
      int       bWidth;

      pTimeTextCtl   = (ITextCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                     IDDC_CALL_DETAILS_TIME_TEXT);
      pTimerCtl      = (ITimeCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                     IDDC_CALL_DETAILS_TIMER);
      pStatusCtl     = (ITextCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                     IDDC_CALL_DETAILS_STATUS);
      pSkMenu        = (IMenuCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                     IDDC_CALL_DETAILS_SK);
      //
      // TIMER TEXT
      //

      // Position time text on first line
      bWidth = IDISPLAY_MeasureText(
                  pSelf->m_pDisplay,
                  AEE_FONT_NORMAL,
                  ITEXTCTL_GetTextPtr(pTimeTextCtl)
                  );

      ITEXTCTL_SetSoftKeyMenu(pTimeTextCtl, NULL);

      SETAEERECT(&rect,
                 0,
                 0,
                 bWidth,
                 pSelf->m_LineHeight);

      ITEXTCTL_SetRect(pTimeTextCtl, &rect);

      // Position call timer on first line next to text.
      // Offset for Timer is width of Time Text
      bOffset = bWidth;

      SETAEERECT(&rect,
                 bOffset,
                 0,
                 (pSelf->m_Rect.dx - bOffset),
                 pSelf->m_LineHeight);

      ITIMECTL_SetRect(pTimerCtl, &rect);

      ITIMECTL_SetProperties(pTimerCtl, TP_NO_MSECONDS);

      //
      //  CALL STATUS
      //
      ITEXTCTL_SetSoftKeyMenu(pStatusCtl, NULL);

      // Position on second line
      SETAEERECT(&rect,
                 0,
                 pSelf->m_LineHeight,
                 pSelf->m_Rect.dx,
                 pSelf->m_LineHeight*2);

      ITEXTCTL_SetRect(pStatusCtl, &rect);

      //
      //  PHONE BOOK MATCH
      //

      // Position on third line
      SETAEERECT(&rect,
                 0,
                 pSelf->m_LineHeight*2,
                 pSelf->m_Rect.dx,
                 pSelf->m_LineHeight);

      ISTATIC_SetRect(pSelf->m_pNameCtl, &rect);

      //
      //  SOFTKEY MENU
      //
      SSOperations_SetDefaultSoftkeyLook(pSelf->m_pShell, pSkMenu);

      (void) SS_PostEvent(
                pSelf->m_pShell,
                AEECLSID_SS_OPERATIONS,
                EVT_USER_DRAW,
                0,
                0
                );
      return TRUE;
   }

   case EVT_USER_UPDATE_DIALOG:
   {
      GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:Conv EVT_USER_UPDATE_DIALOG evt", 0, 0, 0);

      // call the same event handler code again with a different event code
      return SSOperationsDialog_ConvHandleEvent(pSelf, EVT_USER_DRAW, 0, 0);
   }

   case EVT_USER_DRAW:
   {
      ITextCtl *pStatusCtl;
      IMenuCtl *pSkMenu;
      ITimeCtl *pTimerCtl;

      // KLUDGE:
      // The text control objects overwrite their rectangles on the bottom
      // by 6 pixels.  Redraw the text controls from the top of the screen
      // down to ensure there are no display problems.

      pStatusCtl = (ITextCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                 IDDC_CALL_DETAILS_STATUS);
      pSkMenu    = (IMenuCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                 IDDC_CALL_DETAILS_SK);
      pTimerCtl  = (ITimeCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                 IDDC_CALL_DETAILS_TIMER);

      if (SSOperationsDialog_CalculateCallInfo(pSelf) == TRUE)
      {
          // There must be at least a foreground call
          assert(IsCallDetailsCallInfoAvailable());

          // Update call timer
          ITIMECTL_SetTime(
             pTimerCtl,
             ISHELL_GetUpTimeMS(pSelf->m_pShell) -
             GetCallDetailsCallStartTime()
             );
          (void) ITIMECTL_Redraw(pTimerCtl);

          //
          //  CALL STATUS
          //
          SSOperationsDialog_SetConvCallStatus(pSelf, pStatusCtl);
          (void) ITEXTCTL_Redraw(pStatusCtl);

          //
          //  CALL NAME
          //
          if (SSOperationsDialog_SetConvCallName(pSelf, pSelf->m_pNameCtl) == TRUE)
          {
              (void) ISTATIC_Redraw(pSelf->m_pNameCtl);
              IStaticScroller_SetIStatic(pSelf->m_pNameScroller, pSelf->m_pNameCtl);
              IStaticScroller_StartScrolling(
                  pSelf->m_pNameScroller, 
                  TIMEOUT_STATIC_TEXT_SCROLLING
                  );
          }
          else
          {
              //
              //  PHONE NUMBER
              //

              // There is no space left to show both the call's name and
              // the phone number! call name is given priority so if it
              // was set successfully then do not set and draw the phone number.
              SSOperationsDialog_SetConvPhoneNumber(pSelf, pSelf->m_pNumberCtl);
              (void) ISTATIC_Redraw(pSelf->m_pNumberCtl);

              IStaticScroller_SetIStatic(pSelf->m_pNumberScroller ,pSelf->m_pNumberCtl);
              IStaticScroller_StartScrolling(
                  pSelf->m_pNumberScroller, 
                  TIMEOUT_STATIC_TEXT_SCROLLING
                  );
          }

          SSOperationsDialog_SetConvSoftkeyMenu(pSelf, pSkMenu);
          (void) IMENUCTL_Redraw(pSkMenu);
          (void)IDIALOG_SetFocus(pSelf->m_pDialog, IDDC_CALL_DETAILS_SK);

          // Start dialog update timer
          CALLBACK_Init(
             &pSelf->m_UIDialogTimerCB, 
             SSOperationsDialog_HandleCallDetailsTimer, 
             (void*)pSelf
             );

          (void) ISHELL_SetTimerEx(
                    pSelf->m_pShell,
                    TIMEOUT_MS_CONV_TIMER,
                    &pSelf->m_UIDialogTimerCB
                    );
      }

      return TRUE;
   }

   case EVT_DIALOG_END:
   {
      CALLBACK_Cancel(&pSelf->m_UIDialogTimerCB); // cancel timer callback

      IStaticScroller_SetIStatic(pSelf->m_pNumberScroller, NULL);
      IStaticScroller_SetIStatic(pSelf->m_pNameScroller, NULL);

      ISTATIC_Reset(pSelf->m_pNumberCtl);
      ISTATIC_Reset(pSelf->m_pNameCtl);

      return TRUE;
   }

   case EVT_ENTRY_MENU_HOLD_REQUESTED:
      // simulate hold user selection
      SSOperationsDialog_ConvHandleEvent(pSelf, EVT_COMMAND, IDDC_SK_LI_HOLD, 0);
      return TRUE;

   case EVT_COMMAND:
   {
      IMenuCtl *pSkMenu;

      // Assumes the EVT_KEY event is immediately processed after the dialog got it
      // (synchronously) and isn't delegated for later (asynchronously).
      // In this case there is no chance for the dialog to be changed.
      assert(pSelf->m_DialogID == IDD_CALL_DETAILS);
      assert(pSelf->m_PopupStatus == SS_NO_POPUP);

      pSkMenu = (IMenuCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                              IDDC_CALL_DETAILS_SK);
      switch (wParam) 
      {
         case IDDC_SK_LI_MUTE:
         case IDDC_SK_LI_UNMUTE:
         {
            pSelf->m_CallMuted = !pSelf->m_CallMuted;
            SSOperationsDialog_SetupCallAudio(pSelf);

            (void) SS_PostEvent(
                      pSelf->m_pShell,
                      AEECLSID_SS_OPERATIONS,
                      EVT_USER_UPDATE_DIALOG,
                      0,
                      0
                      );

            return TRUE;
         }
         break;

         case IDDC_SK_LI_END_CALL:
         {
            SSEventData *pEventData;

            if (pSelf->m_IsForegroundCallMPTY)
            {
                // TBD - send new MPTY call disconnect primitive !!!
                assert(!"not supported yet");
            }
            else
            {
                pEventData = 
                    SSOperationsDialog_CreateTransactionWiseEventData(
                        pSelf,
                        pSelf->m_ForegroundCallIndex,
                        NULL
                        );

                if (pEventData == NULL) return TRUE;

                (void) SS_PostEvent(
                          pSelf->m_pShell,
                          AEECLSID_SS_OPERATIONS,
                          UIMN_DISC_REQ,
                          0,
                          (uint32) pEventData
                          );

                pSelf->m_EndingCallDialogCallIndex = pSelf->m_ForegroundCallIndex;

                SET_ENDING_CALL_PREV_DIALOG_SETUP_TO_CURRENT;
                SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupEndingCallDialog );
            }
            return TRUE;
         }
         break;

         case IDDC_SK_LI_END_OTHER_CALL:
         {
            SSEventData *pEventData;

            if (pSelf->m_IsBackgroundCallMPTY)
            {
                // TBD - send new MPTY call disconnect primitive !!!
                assert(!"not supported yet");
            }
            else
            {
                pEventData = 
                    SSOperationsDialog_CreateTransactionWiseEventData(
                        pSelf,
                        pSelf->m_BackgroundCallIndex,
                        NULL
                        );

                if (pEventData == NULL) return TRUE;
                
                (void) SS_PostEvent(
                          pSelf->m_pShell,
                          AEECLSID_SS_OPERATIONS,
                          UIMN_DISC_REQ,
                          0,
                          (uint32) pEventData
                          );
            }
            return TRUE;
         }
         break;

         case IDDC_SK_LI_HOLD:
         {
            SSEventData *pEventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_CALLWISE_MSG);
            if (pEventData == NULL) return TRUE;

            pEventData->callwiseMsgData.callIndex1 = pSelf->m_ForegroundCallIndex;

            (void) SS_PostEvent(
                      pSelf->m_pShell,
                      AEECLSID_SS_OPERATIONS,
                      UIMN_HOLD_REQ,
                      0,
                      (uint32)pEventData
                      );
            return TRUE;
         }
         break;

         case IDDC_SK_LI_RETRIEVE:
         {
            SSEventData *pEventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_CALLWISE_MSG);
            if (pEventData == NULL) return TRUE;

            pEventData->callwiseMsgData.callIndex1 = pSelf->m_ForegroundCallIndex;

            (void) SS_PostEvent(
                      pSelf->m_pShell,
                      AEECLSID_SS_OPERATIONS,
                      UIMN_RETRIEVE_REQ,
                      0,
                      (uint32)pEventData
                      );
            return TRUE;
         }
         break;

         case IDDC_SK_LI_SWAP:
         {
            SSEventData *pEventData = 
                SSOEvent_AllocateEventData(SS_EVENT_DATA_CALLWISE_MSG);
            if (pEventData == NULL) return TRUE;

            pEventData->callwiseMsgData.callIndex1 = pSelf->m_ForegroundCallIndex;
            pEventData->callwiseMsgData.callIndex2 = pSelf->m_BackgroundCallIndex;

            (void) SS_PostEvent(
                      pSelf->m_pShell,
                      AEECLSID_SS_OPERATIONS,
                      UIMN_TOGGLE_REQ,
                      0,
                      (uint32) pEventData
                      );
             
            SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupSwappingCallsDialog );
            return TRUE;
         }
         break;

         case IDDC_SK_LI_BUILD_MPTY:
         {
            SSCall_Index_type        toCall;
            SSCall_Index_type        fromCall;

            SSEventData *pEventData = 
                SSOEvent_AllocateEventData(SS_EVENT_DATA_CALLWISE_MSG);
            if (pEventData == NULL) return TRUE;

            if (pSelf->m_IsForegroundCallMPTY)
            {
                toCall = pSelf->m_ForegroundCallIndex;
                fromCall = pSelf->m_BackgroundCallIndex;
            }
            else
            {
                if (pSelf->m_IsBackgroundCallMPTY)
                {
                    toCall = pSelf->m_BackgroundCallIndex;
                    fromCall = pSelf->m_ForegroundCallIndex;
                }
                else
                {
                    // arbitrarily choose to build to the foreground call
                    toCall = pSelf->m_ForegroundCallIndex;
                    fromCall = pSelf->m_BackgroundCallIndex;
                }
            }

            pEventData->callwiseMsgData.callIndex1 = toCall;
            pEventData->callwiseMsgData.callIndex2 = fromCall;

            (void) SS_PostEvent(
                      pSelf->m_pShell,
                      AEECLSID_SS_OPERATIONS,
                      UIMN_BUILD_MPTY_REQ,
                      0,
                      (uint32) pEventData
                      );
            
            pSelf->m_BuildingMPTYCallDialogCallIndex = fromCall;
            SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupBuildingMPTYDialog );
            return TRUE;
         }
         break;

         case IDDC_SK_LI_SHOW_MPTY_CALL_DETAILS:
         {
            SSCall_Index_type MPTYCallIndex;

            if (pSelf->m_IsForegroundCallMPTY)
            {
                MPTYCallIndex = pSelf->m_ForegroundCallIndex;
            }
            else
            {
                if (pSelf->m_IsBackgroundCallMPTY)
                {
                    MPTYCallIndex = pSelf->m_BackgroundCallIndex;
                }
                else
                {
                    assert(!"unknown error occurred");
                    return TRUE;
                }
            }
            
            pSelf->m_MPTYCallDetailsDialogCallIndex = MPTYCallIndex;
            SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupMPTYCallDetailsDialog );
            return TRUE;
         }
         break;
      }
   }
   break;

   case EVT_KEY_PRESS:
      switch ((AVKType)wParam) 
      {
      case AVK_UP:
      case AVK_VOLUME_UP:

         SET_VOLUME_PREV_DIALOG_SETUP_TO_CURRENT;
         SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallVolumeDialog )         
         return TRUE;

      case AVK_DOWN:
      case AVK_VOLUME_DOWN:
         
         SET_VOLUME_PREV_DIALOG_SETUP_TO_CURRENT;
         SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallVolumeDialog );         
         return TRUE;
      }
      break;

   case EVT_KEY:
      switch ((AVKType)wParam) 
      {

#if (defined U_TEST && defined AEE_SIMULATOR)

      // for unit testing 
      // ----------------
      case AVK_0:
      {
          SSMsg_int_msg_type* pMsg = 
              SSOMessage_AllocateMsg(SSMSG_MSG_TYPE_SETUP);

          pMsg->setup.hdr.pd = SSMSG_PD_CC;
          pMsg->setup.hdr.tiflag = 0;
          pMsg->setup.hdr.tivalue = 1;
          pMsg->setup.hdr.msg_type = SSMSG_MSG_TYPE_SETUP;
          pMsg->setup.signal.hdr.present = TRUE;
          pMsg->setup.signal.signal_value = SSMSG_SIGNAL_VALUE_CALL_WAITING_TONE_ON;
          pMsg->setup.calling_party_bcd_number.hdr.present = TRUE;
          //pMsg->setup.calling_party_bcd_number.pi =
          //    SSMSG_PI_NUMBER_NOT_AVAILABLE_DUE_TO_INTERWORKING;
          pMsg->setup.calling_party_bcd_number.pi = SSMSG_PI_PRESENTATION_ALLOWED;
              
          MEMCPY(
             pMsg->setup.calling_party_bcd_number.number,
             "0485065062048506506123456789", 
             28
             );
          pMsg->setup.calling_party_bcd_number.number_len = 28;

          SSOperations_TriggerCCEventWithMsg(
             pSelf, pMsg, FALSE, 1
             );
      }
      break;

      case AVK_1:
          SSOperations_TriggerCCEvent(
              pSelf, SSMSG_MSG_TYPE_HOLD_ACKNOWLEDGE, FALSE, 
              pSelf->m_ForegroundCallIndex
              );
          break;

      case AVK_2:
          SSOperations_TriggerCCEvent(
              pSelf, SSMSG_MSG_TYPE_RETRIEVE_ACKNOWLEDGE, FALSE, 
              pSelf->m_ForegroundCallIndex
              );
          break;

      case AVK_3:
          SSOperations_TriggerCCEvent(
              pSelf, SSMSG_MSG_TYPE_HOLD_REJECT, FALSE, 
              pSelf->m_ForegroundCallIndex
              );
          break;

      case AVK_4:
          SSOperations_TriggerCCEvent(
              pSelf, SSMSG_MSG_TYPE_RETRIEVE_REJECT, FALSE, 
              pSelf->m_ForegroundCallIndex
              );
          break;

      case AVK_5:
          SSOperations_TriggerCCEvent(
              pSelf, SSMSG_MSG_TYPE_RELEASE, FALSE, 
              pSelf->m_ForegroundCallIndex
              );
          break;

#else // ! (U_TEST && AEE_SIMULATOR)

      case AVK_0:
      case AVK_1:
      case AVK_2:
      case AVK_3:
      case AVK_4:
      case AVK_5:
#endif // U_TEST && AEE_SIMULATOR
      case AVK_6:
      case AVK_7:
      case AVK_8:
      case AVK_9:
      case AVK_STAR:
      case AVK_POUND:
      {
         // Pass the digit on to the dialer dialog
         char          digit[2];

         digit[0] = SSOperationsDialog_AVKType2ASCII((AVKType)wParam);
         digit[1] = '\0';

         STRTOWSTR(digit,
                     pSelf->m_DialString,
                     sizeof(pSelf->m_DialString));

#ifdef PHONE_DOES_NOT_AUTOMATICALLY_SEND_DTMF_WHILE_IN_TRAFFIC
         // start continuous DTMF
         (void) IPHONE_StartDTMF(pSelf->m_pPhone,
                                 *(byte*)digit);
         /*
         (void) IPHONE_BurstDTMF(pSelf->m_pPhone,
                                 SS_DTMF_ON_150,
                                 SS_DTMF_OFF_100,
                                 (byte*)digit,
                                 1);
         */
#endif //PHONE_DOES_NOT_AUTOMATICALLY_SEND_DTMF_WHILE_IN_TRAFFIC

         SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupDialerDialog );
         return TRUE;
      }

      case AVK_CLR:
         // do nothing!
         return TRUE;
      }
      break;
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_ConvHandleNonDialogEvent

DESCRIPTION: Call Details dialog non dialog event handler

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
   eCode: BREW event code
   wParam:
   dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_ConvHandleNonDialogEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    switch (eCode) 
    {
        case UIMN_SETUP_IND:
        {
            SSOperationsDialog_MoveToTheIncomingDialog(
               pSelf, 
               (SSEventData*)dwParam
               );
            return TRUE;
        }

        case UIMN_SETUP_COMPLETE_IND:
        {
            SSEventData* pEventData = (SSEventData*)dwParam;

            assert(pEventData != NULL);
            assert(pEventData->commonData.type == SS_EVENT_DATA_CALLWISE_MSG);

            SSOEvent_FreeEventData(pEventData);

            (void) SS_PostEvent(
                      pSelf->m_pShell,
                      AEECLSID_SS_OPERATIONS,
                      EVT_USER_UPDATE_DIALOG,
                      0,
                      0
                      );
             return TRUE;
        }
    }

    return FALSE;
}

/*===========================================================================
FUNCTION: SSOperationsDialog_SetupCallDetailsDialog

DESCRIPTION:
    This function creates the Call Details dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog
    
RETURN VALUE:
   None

===========================================================================*/
void SSOperationsDialog_SetupCallDetailsDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( SSOperationsDialog_ConvHandleEvent );        
        SHOW_DIALOG( IDD_CALL_DETAILS );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( SSOperationsDialog_ConvHandleNonDialogEvent );    
}

/*=============================================================================
FUNCTION: SSOperationsDialog_HandleCallDetailsTimer

DESCRIPTION: Timer callback used by the CallDetails dialog to update
             the call timer every second.

PARAMETERS:
   *pUser: CSSOperationsDialogHandler object pointer

RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_HandleCallDetailsTimer(
    void *pUser
    )
{
    CSSOperationsDialogHandler *pSelf = (CSSOperationsDialogHandler *)pUser;
    ITimeCtl                   *pTimerCtl;
    uint32                      elapsedCallTime;

    // Assume that the timeout will only happen when the CallDetails dialog
    // is the active dialog. this is because when the dialog is closed the timer
    // is canceled
    assert(pSelf->m_DialogID == IDD_CALL_DETAILS);
    assert(pSelf->m_PopupStatus == SS_NO_POPUP);

    if (IsCallDetailsCallInfoAvailable())
    {
        elapsedCallTime = ISHELL_GetUpTimeMS(pSelf->m_pShell) - 
                           GetCallDetailsCallStartTime();

        pTimerCtl = (ITimeCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                                  IDDC_CALL_DETAILS_TIMER);

        ITIMECTL_SetTime(pTimerCtl, (int32)elapsedCallTime);
    }

    // Restart dialog update timer
    (void) ISHELL_SetTimerEx(
              pSelf->m_pShell,
              TIMEOUT_MS_CONV_TIMER,
              &pSelf->m_UIDialogTimerCB
              );
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetConvCallStatus

DESCRIPTION: Used by the Call Details dialog to set the call status that
             is displayed.

PARAMETERS:
   *pSelf          [in]: CSSOperationsDialogHandler object pointer
   *pStatusCtl [in+out]: IStaticCtl to use

RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_SetConvCallStatus(  
   CSSOperationsDialogHandler *pSelf,
   ITextCtl                   *pStatusCtl
   )
{
    AECHAR szTextBuffer[CALL_STATUS_TEXT_MAX_LENGTH+1];
    AECHAR szStatusText[STATUS_TEXT_MAX_LENGTH+1];
    AECHAR szFormatText[FORMAT_TEXT_MAX_LENGTH+1];

    SSOperationsDialog_GetStatusStringRepresentation(
        pSelf->m_ForegroundCallStatus,
        pSelf->m_IsForegroundCallMPTY,
        pSelf->m_IsForegroundCallHeld,
        pSelf->m_pShell,
        szStatusText,
        sizeof(szStatusText)
        );

    if (pSelf->m_IsForegroundCallMPTY)
    {
        (void) ISHELL_LoadResString(
            pSelf->m_pShell,
            SSOPERATIONS_RES_FILE,
            IDS_DIALOG_CONV_MPTY_STATUS_FORMAT,
            szFormatText,
            sizeof(szFormatText)
            );

        WSPRINTF(
            szTextBuffer,
            sizeof(szTextBuffer),
            szFormatText,
            szStatusText
            );
    }
    else
    {
        (void) ISHELL_LoadResString(
            pSelf->m_pShell,
            SSOPERATIONS_RES_FILE,
            IDS_DIALOG_CONV_CALL_STATUS_FORMAT,
            szFormatText,
            sizeof(szFormatText)
            );

        WSPRINTF(
            szTextBuffer,
            sizeof(szTextBuffer),
            szFormatText, 
            SSOperationsDialogHandler_GetCallTInfoIndex(
                pSelf,
                pSelf->m_ForegroundCallIndex
                ),
            szStatusText
            );
    }

    (void) ITEXTCTL_SetText(pStatusCtl, szTextBuffer, -1);

    AUTOMATION_LOG_WSTR("%s",szTextBuffer);
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetConvPhoneNumber

DESCRIPTION: Used by the Call Details dialog to set the phone number that
             is displayed.

PARAMETERS:
   *pSelf          [in]: CSSOperationsDialogHandler object pointer
   *pNumberCtl [in+out]: IStaticCtl to use

RETURN VALUE:
    None

COMMENTS:
   The current value of CSSOperationsDialogHandler::m_CallString
   is used as the phone number.

=============================================================================*/
static void SSOperationsDialog_SetConvPhoneNumber(
    CSSOperationsDialogHandler *pSelf,
    IStatic                    *pNumberCtl
    )
{
   AEERect rect;
   int     bOffset;
   AECHAR *pCallNumberString;

   assert(IsCallDetailsCallInfoAvailable());

   pCallNumberString = GetCallDetailsCallString();

   // Position on last line, to the right
   bOffset = IDISPLAY_MeasureText(
                 pSelf->m_pDisplay,
                 AEE_FONT_NORMAL,
                 pCallNumberString
                 );

   if (bOffset > pSelf->m_Rect.dx) 
   {
      bOffset = pSelf->m_Rect.dx;
   }

   SETAEERECT(&rect,
              pSelf->m_Rect.dx - bOffset,
              pSelf->m_Rect.dy - 2*pSelf->m_LineHeight,
              pSelf->m_Rect.dx,
              pSelf->m_LineHeight);

   ISTATIC_SetRect(pNumberCtl, &rect);

   (void) ISTATIC_SetText(
             pNumberCtl,
             NULL,
             pCallNumberString,
             AEE_FONT_NORMAL,
             AEE_FONT_NORMAL
             );

   AUTOMATION_LOG_WSTR("N:%s", pCallNumberString);
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetConvCallName

DESCRIPTION: Used by the Call Details dialog to set the call name that
             is displayed.

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
   *pNameCtl: TextCtl to use

RETURN VALUE:
    TRUE if a name was set, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_SetConvCallName(
    CSSOperationsDialogHandler *pSelf,
    IStatic                    *pNameCtl
    )
{
    if (!pSelf->m_IsForegroundCallMPTY)
    {
        SSCall_TInfo_type *pCallTInfo =
            SSOperationsDialogHandler_GetCallTInfo(
                pSelf,
                pSelf->m_ForegroundCallIndex
                );

        if (pCallTInfo->callName[0] != (AECHAR)'\0')
        {
            (void) ISTATIC_SetText(
                     pNameCtl,
                     NULL,
                     pCallTInfo->callName,
                     AEE_FONT_NORMAL,
                     AEE_FONT_NORMAL
                     );

            AUTOMATION_LOG_WSTR("N:%s", pCallTInfo->callName);
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetConvSoftkeyMenu

DESCRIPTION: Used by the Call Details dialog to set the softkey options

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
   *pSkMenu [in+out]: IMenuCtl interface pointer
   
RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_SetConvSoftkeyMenu(
    CSSOperationsDialogHandler *pSelf,
    IMenuCtl                   *pSkMenu
    )
{
    boolean showHoldSoftkey = FALSE;
    boolean showRetrieveSoftkey = FALSE;
    boolean showSwapSoftkey = FALSE;
    boolean showBuildMPTYSoftkey = FALSE;
    boolean showMPTYCallDetails = FALSE;
    boolean showEndCallSoftkey = FALSE;
    boolean showEndOtherCallSoftkey = FALSE;

    (void) IMENUCTL_DeleteAll(pSkMenu);

    if (pSelf->m_CallMuted) 
    {
        (void) IMENUCTL_AddItem(pSkMenu,
                                SSOPERATIONS_RES_FILE,
                                IDS_UNMUTE,
                                IDDC_SK_LI_UNMUTE,
                                (AECHAR*)NULL,
                                (uint32)NULL);
    } 
    else 
    {
        (void) IMENUCTL_AddItem(pSkMenu,
                                SSOPERATIONS_RES_FILE,
                                IDS_MUTE,
                                IDDC_SK_LI_MUTE,
                                (AECHAR*)NULL,
                                (uint32)NULL);
    }

    if (pSelf->m_NumberOfCalls == 1)
    {
        if (pSelf->m_IsForegroundCallHeld)
        {
            showRetrieveSoftkey = TRUE;
        }
        else
        {
            if ((pSelf->m_IsForegroundCallMPTY) ||
                (pSelf->m_ForegroundCallStatus == SS_CALL_STATUS_ACTIVE))
            {
                showHoldSoftkey = TRUE;
            }

            // temp hack for release that does not support hold MPTY
            // TBD: remove next line in next release
            if (pSelf->m_IsForegroundCallMPTY) showHoldSoftkey = FALSE;
        }

        if (pSelf->m_IsForegroundCallMPTY)
        {
            showMPTYCallDetails = TRUE;
        }
    }
    else  // two calls ...
    {
        assert(pSelf->m_NumberOfCalls == 2);

        if (pSelf->m_IsForegroundCallHeld)
        {
           showRetrieveSoftkey = TRUE;
        }
        else
        {
            if ((pSelf->m_IsForegroundCallMPTY) || 
                (pSelf->m_ForegroundCallStatus == SS_CALL_STATUS_ACTIVE))
            {
                if (pSelf->m_IsBackgroundCallHeld)
                {
                    showSwapSoftkey = TRUE;
                    showBuildMPTYSoftkey = TRUE;
                }
                else
                {
                    showHoldSoftkey = TRUE;
                }
            }
        }

        if ((pSelf->m_IsForegroundCallMPTY) || 
            (pSelf->m_IsForegroundCallHeld) ||
            (pSelf->m_ForegroundCallStatus == SS_CALL_STATUS_ACTIVE))
        {
            showEndCallSoftkey = TRUE;
        }

        // temp hack for release that does not support hold MPTY
        // TBD: remove next line in next release
        if (pSelf->m_IsForegroundCallMPTY) showEndCallSoftkey = FALSE;

        if ((pSelf->m_IsBackgroundCallMPTY) || 
            (pSelf->m_IsBackgroundCallHeld) ||
            (pSelf->m_BackgroundCallStatus == SS_CALL_STATUS_ACTIVE))
        {
            showEndOtherCallSoftkey = TRUE;
        }

        // temp hack for release that does not support hold MPTY
        // TBD: remove next line in next release
        if (pSelf->m_IsBackgroundCallMPTY) showEndOtherCallSoftkey = FALSE;

        if ((pSelf->m_IsForegroundCallMPTY) || (pSelf->m_IsBackgroundCallMPTY))
        {
            showMPTYCallDetails = TRUE;
        }
    }

    // add command softkeys ...
    if (showHoldSoftkey)
    {
        (void) IMENUCTL_AddItem(pSkMenu,
                                SSOPERATIONS_RES_FILE,
                                IDS_HOLD,
                                IDDC_SK_LI_HOLD,
                                (AECHAR*)NULL,
                                (uint32)NULL);
    }

    if (showRetrieveSoftkey)
    {
        assert(!showHoldSoftkey);
        (void) IMENUCTL_AddItem(pSkMenu,
                                SSOPERATIONS_RES_FILE,
                                IDS_RETRIEVE,
                                IDDC_SK_LI_RETRIEVE,
                                (AECHAR*)NULL,
                                (uint32)NULL);
    }

    if (showSwapSoftkey)
    {
        assert((!showHoldSoftkey) && (!showRetrieveSoftkey));
        (void) IMENUCTL_AddItem(pSkMenu,
                                SSOPERATIONS_RES_FILE,
                                IDS_SWAP,
                                IDDC_SK_LI_SWAP,
                                (AECHAR*)NULL,
                                (uint32)NULL);
    }

    if (showBuildMPTYSoftkey)
    {
        (void) IMENUCTL_AddItem(pSkMenu,
                                SSOPERATIONS_RES_FILE,
                                IDS_BUILD_MPTY,
                                IDDC_SK_LI_BUILD_MPTY,
                                (AECHAR*)NULL,
                                (uint32)NULL);
    }

    if (showMPTYCallDetails)
    {
        (void) IMENUCTL_AddItem(pSkMenu,
                                SSOPERATIONS_RES_FILE,
                                IDS_SHOW_MPTY_CALL_DETAILS,
                                IDDC_SK_LI_SHOW_MPTY_CALL_DETAILS,
                                (AECHAR*)NULL,
                                (uint32)NULL);
    }

    if (showEndCallSoftkey)
    {        
        (void) IMENUCTL_AddItem(pSkMenu,
                                SSOPERATIONS_RES_FILE,
                                IDS_END_CALL,
                                IDDC_SK_LI_END_CALL,
                                (AECHAR*)NULL,
                                (uint32)NULL);
    }

    if (showEndOtherCallSoftkey)
    {
        (void) IMENUCTL_AddItem(pSkMenu,
                                SSOPERATIONS_RES_FILE,
                                IDS_END_OTHER_CALL,
                                IDDC_SK_LI_END_OTHER_CALL,
                                (AECHAR*)NULL,
                                (uint32)NULL);
    }

    (void) IMENUCTL_Redraw(pSkMenu);
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetupCallAudio

DESCRIPTION:
   Sets the current sound path based on the current values of:
      pSelf->m_CallVolume
      pSelf->m_CallMuted

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

=============================================================================*/
static void SSOperationsDialog_SetupCallAudio(
    CSSOperationsDialogHandler *pSelf
    )
{
   AEESoundInfo soundData;

   (void) ISOUND_Get(pSelf->m_pSound, &soundData);

   soundData.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;

   if (pSelf->m_CallMuted) 
   {
      soundData.eMicMuteCtl = AEE_SOUND_MUTECTL_MUTED;
   } 
   else 
   {
      soundData.eMicMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
   }

   (void) ISOUND_Set(pSelf->m_pSound, &soundData);

   ISOUND_SetDevice(pSelf->m_pSound);
}

/*=============================================================================
FUNCTION: SSOperationsDialog_GetStatusStringRepresentation

DESCRIPTION: Returns the text string representation of a call status value.

PARAMETERS:
    call status [in]: call status
    isMPTY [in] : MPTY call indicator
    isHeld [in] : Held call indicator
    pShell [in]: IShell interface instance
   *text [out]: string representation text
    textSize [in]: output string size

RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_GetStatusStringRepresentation(
   SSCall_Status_type  callStatus,
   boolean             isMPTY,
   boolean             isHeld,
   IShell             *pShell, 
   AECHAR             *pText,
   uint8               textSize
   )
{
    int16 resourceID;

    if (isMPTY)
    {
        if (isHeld)
        {
            resourceID = IDS_CALL_STATUS_HELD_MPTY;
        }
        else
        {
            resourceID = IDS_CALL_STATUS_MPTY;
        }
    }
    else
    {
        if (isHeld) 
        {
            resourceID = IDS_CALL_STATUS_HELD;
        }
        else
        {
            switch (callStatus)
            {
                case SS_CALL_STATUS_SETUP:
                    resourceID = IDS_CALL_STATUS_SETUP;
                    break;

                case SS_CALL_STATUS_CONNECTED:
                    resourceID = IDS_CALL_STATUS_CONNECTED;
                    break;

                case SS_CALL_STATUS_ACTIVE:
                    resourceID = IDS_CALL_STATUS_ACTIVE;
                    break;

                case SS_CALL_STATUS_DISCONNECTED:
                    resourceID = IDS_CALL_STATUS_DISCONNECTED;
                    break;

                default:
                    assert(!"unreachable code");
                    return;
            }
        }
    }

    ISHELL_LoadResString(
       pShell,
       SSOPERATIONS_RES_FILE,
       resourceID,
       pText,
       textSize
       );
}

/*=============================================================================
FUNCTION: SSOperationsDialog_GetCallStatusPriority

DESCRIPTION: Determine the priority value for a specific call status

PARAMETERS:
    isCallMPTY [in]: MPTY call indicator
    isCallHeld [in]: Held call indicator
    callStatus [in]: call status enum

RETURN VALUE:
    priority value

COMMENTS:
    The larger the returned value the higher the priority.
=============================================================================*/
static uint8 SSOperationsDialog_GetCallStatusPriority(
    boolean            isCallMPTY,
    boolean            isCallHeld,
    SSCall_Status_type callStatus
    )
{
    // An Active call or MPTY call has the highest priority
    // and after that a Held call
    if (isCallMPTY)
    {
        if (isCallHeld)
        {
            return 4;
        }
        else
        {
            return 8;
        }
    }
    else
    {
        if (isCallHeld)
        {
            return 4;
        }

        switch (callStatus)
        {
            case SS_CALL_STATUS_SETUP:
                return 1;

            case SS_CALL_STATUS_CONNECTED:
                return 1;

            case SS_CALL_STATUS_ACTIVE:
                return 8;

            case SS_CALL_STATUS_DISCONNECTED:
                return 1;

            default:
                assert(!"unknown enum value");
                return 0xff;
        }
    }

    return 0; // dummy 
}

/*=============================================================================
FUNCTION: SSOperationsDialog_DoesCall1HaveHigherPriority

DESCRIPTION: Determine if the first call status has a higher priority 
             then the second one

PARAMETERS:
    isCall1MPTY [in]: first call MPTY indicator
    isCall1Held [in]: first call Held indicator
    call1Status [in]: first call status
    isCall2MPTY [in]: second call MPTY indicator
    isCall2Held [in]: second call Held indicator
    call2Status [in]: second call status
    
RETURN VALUE:
    True if first call status has a higher priority then the second call status,
    and False otherwise.
=============================================================================*/
static boolean SSOperationsDialog_DoesCall1HaveHigherPriority(
    boolean            isCall1MPTY,
    boolean            isCall1Held,
    SSCall_Status_type call1Status,
    boolean            isCall2MPTY,
    boolean            isCall2Held,
    SSCall_Status_type call2Status
    )
{
    uint8 firstCallPriority, secondCallPriority;

    firstCallPriority = 
       SSOperationsDialog_GetCallStatusPriority(
           isCall1MPTY, 
           isCall1Held, 
           call1Status
           );

    secondCallPriority = 
       SSOperationsDialog_GetCallStatusPriority(
           isCall2MPTY, 
           isCall2Held, 
           call2Status
           );

    if (firstCallPriority > secondCallPriority) 
    {
        return TRUE;
    }
    else 
    {
        return FALSE;
    }
}

/*=============================================================================
FUNCTION: SSOperationsDialog_CalculateCallInfo

DESCRIPTION:
    Find out which information to display in "Call Details".

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer

RETURN VALUE:
    TRUE if at least one call info was found, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_CalculateCallInfo(
    CSSOperationsDialogHandler *pSelf
    )
{
    SSCall_Info_type  *pCallInfo;
    SSCall_Index_type  callIndex;
    boolean            aCallWasFound = FALSE;
    boolean            success;

    for (callIndex=0; callIndex < MAX_DATABASE_CALL_ENTRIES; ++callIndex)
    {
        success = SSCallDatabase_GetCallInfo(
                     pSelf->m_pCallDatabase, 
                     callIndex, 
                     &pCallInfo
                     );

        if (success)
        {
            SSCall_TInfo_type *pCallTInfo;

            if (aCallWasFound == FALSE)
            {
                pSelf->m_NumberOfCalls = 1;

                success =
                    SSCallDatabase_GetTransactionInfo(
                       pSelf->m_pCallDatabase, 
                       pCallInfo->transactionIndices[0],
                       &pCallTInfo
                       );
                assert(success == TRUE);

                pSelf->m_IsForegroundCallMPTY = pCallInfo->isMPTY;
                pSelf->m_IsForegroundCallHeld = pCallInfo->isHeld;
                pSelf->m_ForegroundCallStatus = pCallTInfo->callStatus;
                pSelf->m_ForegroundCallIndex = callIndex;

                aCallWasFound = TRUE;
            }
            else
            {
                pSelf->m_NumberOfCalls = 2;
                success =
                    SSCallDatabase_GetTransactionInfo(
                       pSelf->m_pCallDatabase, 
                       pCallInfo->transactionIndices[0],
                       &pCallTInfo
                       );
                assert(success == TRUE);

                if (SSOperationsDialog_DoesCall1HaveHigherPriority(
                       pCallInfo->isMPTY,
                       pCallInfo->isHeld,
                       pCallTInfo->callStatus,
                       pSelf->m_IsForegroundCallMPTY,
                       pSelf->m_IsForegroundCallHeld,
                       pSelf->m_ForegroundCallStatus
                       ))
                { 
                    // new call has a higher priority - set the 'old' foreground
                    // call data as the background call data
                    // and put the new call in the foreground call data.
                    pSelf->m_IsBackgroundCallMPTY = pSelf->m_IsForegroundCallMPTY;
                    pSelf->m_IsBackgroundCallHeld = pSelf->m_IsForegroundCallHeld;
                    pSelf->m_BackgroundCallStatus = pSelf->m_ForegroundCallStatus;
                    pSelf->m_BackgroundCallIndex  = pSelf->m_ForegroundCallIndex;

                    pSelf->m_IsForegroundCallMPTY = pCallInfo->isMPTY;
                    pSelf->m_IsForegroundCallHeld = pCallInfo->isHeld;
                    pSelf->m_ForegroundCallStatus = pCallTInfo->callStatus;
                    pSelf->m_ForegroundCallIndex  = callIndex;                    
                }
                else
                {
                    pSelf->m_IsBackgroundCallMPTY = pCallInfo->isMPTY;
                    pSelf->m_IsBackgroundCallHeld = pCallInfo->isHeld;
                    pSelf->m_BackgroundCallStatus = pCallTInfo->callStatus;
                    pSelf->m_BackgroundCallIndex  = callIndex;
                }          
            }
        }
    }

    // A background call must be either Held or not a MPTY call
    // and not an Active call
    assert( (pSelf->m_IsBackgroundCallHeld) ||
            ( (!pSelf->m_IsBackgroundCallMPTY) && 
              (pSelf->m_BackgroundCallStatus != SS_CALL_STATUS_ACTIVE) )
          );

    // temp hack for release that does not support hold MPTY
    // there can be only one MPTY call or two regular calls
    // TBD: remove next line in next release
    assert( !(pSelf->m_NumberOfCalls == 2) ||
             ( (!pSelf->m_IsForegroundCallMPTY) && 
               (!pSelf->m_IsBackgroundCallMPTY) )
          );

    return aCallWasFound;
}
