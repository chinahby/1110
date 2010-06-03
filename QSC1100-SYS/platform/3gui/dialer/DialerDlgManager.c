/*=============================================================================

FILE: DialerDlgManager.c

SERVICES: Dialer Applet 

GENERAL DESCRIPTION: Manages what dialogs can be displayed based on events

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerDlgManager.c#113 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
08/22/07   cvs     Add framework for dialogs
08/02/07   cvs     Featurize all secondary display support
04/05/07   alb     Do not display status dialog when SS call forward register 
                   conf is received.
11/22/06   ak      Release the call if Release record is sent from n/w
09/18/06   cvs     Fix compile warning
09/10/06   ypmw    Fix the error display message when it is in invalid_sim_state
04/24/06   alb     Change to handle 80 character dial strings plus the '+'
04/17/06   jas     Ignoring incoming SMS calls, which will not be displayed
03/06/06   jks     Added logging for all calls ended event
02/17/06   jks     Removed duplicate code
02/15/06   jas/sun Fixed a bad format string that was causing a data abort.
02/09/06   jas     Fixing bad handling of pointers
12/05/05   sun     Increased Display Buffer size for Alpha of Request Struct
11/16/05   jas     Support for disabling in-call display, per config item
10/18/05   cvs     Carkit doesn't print to display while in call
10/14/05   cvs     support for attaching carkit display mid call
10/12/05   cvs     Carkit display
09/19/05   RI      Fixed the autoanswer error if there there are more than
                   one voice call active.
07/28/05   AT      Fix compiler warnings.
07/11/05   aps     Added PI field lookup when Flashed W/ Info frm BS.
07/07/05   cvs     Zero pad seconds in call duration on secondary display
06/10/05   sun     Fixed Featurizations
06/09/05   sun     Enabled auto recall timer
06/07/05   sun     Added support for CCBS
05/31/05   RI      Modified USSD processing.
05/19/05   SUN     Fixed SS Code and SS operation values
05/10/05   AT      Cleared flag for waiting for origination on ORIG event.
                   Also changed status dialog to not poll for things to do.
04/13/05   SUN     Display SS Status correctly
03/31/05   RI      Added support for saving the NW initiated USSD DCS.
01/07/04   SUN     Added the CDA/CDS Data Services
10/14/04   SUN     Added new alert Signal
09/28/04   JAS     Corrected errors in status infor for secondary display
09/27/04   SUN     Added Support for CPHS
09/20/04   SUN     Fixed Mute and Vocoder settings
09/16/04   SUN     Fixed SS Strings
07/26/04   JAS     Added support for secondary display
07/08/04   PA      The call ended after emergency flash need not be displayed
07/08/04   SUN     Stop the call failed sound on incoming call event
07/02/04   SUN     End Dialogs on Call Error
06/24/04   SUN     Vocoder should not be turned on multiple times
05/20/04   SUN     Play the ringback only when the Progress Info is Received
04/29/04   SUN     Stop the Ringer on Call Answer
04/14/04   SUN     Stop AutoAnswer Timer when call is answered/ended
                   Do not display BSG  on SS Response
04/09/04   SUN     Fixed Lint Errors, Stop AutoAnswer Timer after call is 
                   answered
04/05/04   BPW     Alert changes for failed calls
03/18/04   SUN     Check for NULL Pointer
02/10/04   BPW     End Missed Call Dlg on connected event
01/29/04   BPW     Refresh dialog after the phone in use error message is suppresed
01/22/04   BPW     Optimized NV gets
01/21/04   BPW     Added ISuppSvc API changes 
01/21/04   BPW     Modified ISDN normal alert signal & fixed missed call time
01/20/04   BPW     DTMF and other memory cleanup and lint errors
12/19/03   BPW     USSD & MissedCall now uses own dialg info structure
12/18/03   BPW     Added more SS feedback for requesting and incomplete
12/09/03   BPW     Added code review comments and changes
12/04/03   BPW     Fix for display after call ops event
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/25/03   BPW     Linted for the Saber 6250 build
11/21/03   BPW     Suppress displaying of AEECM_CALL_ERROR_IN_USE_S error
11/19/03   BPW     Fixed the handling of AEECM_EVENT_CALL_CALLER_ID again
11/18/03   BPW     Fixed the handling of AEECM_EVENT_CALL_CALLER_ID
11/18/03   BPW     Play ringback sound for calling in GSM/WCDMA
11/13/03   BPW     Unmute handset for voice calls only
11/13/03   BPW     Made auto answer timer based
11/07/03   SUN     Fixed End Status Display 
10/17/03   SUN     Added Missing Display Notifications
10/16/03   RAM     Added CUGInfo notification display 
10/16/03   SUN     Fixed FWD Reason Display
10/15/03   RAM     Modified functionality for USSD NW Initiated messages.
10/15/03   BPW     Removed the memory for some SS messages
10/10/03   RAM     Added BSGList display,call restriction notification,code cleanup.
10/09/03   SUN     Fixed SS Complete Display   
10/06/03   SUN     Fixed USSD Display
09/04/03   SUN     Changes related to AEESUPPSVC
08/29/03   BW      Fixed compile warnings
08/29/03   BW      Finished SuppSvc related displays for completed events
08/22/03   BW      Added TimePauseDlg and functionality
08/19/03   BW      Added messages, param checking, and more code clean up
08/14/03   BW      Integration with both ICM and ISuppSvc
07/25/03   BW      Created DialerDlgManager Source File 

=============================================================================*/
//lint -save -e611
//lint -save -e740

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"
#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEShell.h"
#include "AEEAppGen.h"
#include "DialerApp.h"
#include "DialerNotif.h"
#include "DialerUtils.h"
#include "DialerDlgManager.h"
#include "dialerapp_res.h"
#include "OEMCFGI.h"
#include "AEEAnnunciator.h"
#include "disp.h"
#ifdef FEATURE_MDP
#include "mdp_api.h"
#endif

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static boolean  CDialerApp_DlgManager_PlaySignal(CDialerApp *pMe, AECHAR* pszPhoneNumber, AEECMSignal *pSignal);
static boolean  CDialerApp_DlgManager_DisplayCallError(CDialerApp *pMe, uint32 dwError);
static boolean  CDialerApp_DlgManager_DisplayCallEndCause(CDialerApp *pMe, uint32 dwError, uint32 dwOtherError);
static uint32   CDialerApp_DlgManager_CallErrorToIDS(uint32 dwCode);
static boolean  CDialerApp_DlgManager_DisplaySSError(CDialerApp *pMe, uint32 dwError);
static uint32   CDialerApp_DlgManager_SSErrorToIDS(uint32 dwErr);
#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
static void     CDialerApp_SecStatusCB(void * pUser);
#endif

#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM)
static boolean  CDialerApp_DlgManager_DisplayError(CDialerApp *pMe, AEESUPPSVC_EVENT_DATA *EventInfo );
static boolean  CDialerApp_DlgManager_DisplaySSCallBarringInfo(CDialerApp *pMe, AEESUPPSVC_EVENT_DATA *EventInfo);
static boolean  CDialerApp_DlgManager_DisplaySSForwardingInfo(CDialerApp *pMe, AEESUPPSVC_EVENT_DATA *EventInfo);
static boolean  CDialerApp_DlgManager_DisplaySSForwardingFeatureInfo(CDialerApp *pMe, AEESUPPSVC_EVENT_DATA *EventInfo);
static boolean  CDialerApp_DlgManager_DisplaySSCLIRestriction(CDialerApp *pMe, AEESUPPSVC_EVENT_DATA *EventInfo);
static boolean  CDialerApp_DlgManager_DisplayCUG(CDialerApp *pMe, AEESuppSvcCUGInfo *pCUGInfo);
static boolean  CDialerApp_DlgManager_DisplayBSGList(CDialerApp *pMe, AEESUPPSVC_EVENT_DATA *EventInfo);
static boolean  CDialerApp_DlgManager_DisplaySSStatus(CDialerApp *pMe, AEESUPPSVC_EVENT_DATA *EventInfo);
static uint32   CDialerApp_DlgManager_SSCodeETypeToIDS(uint32 dwCode);
static uint32   CDialerApp_DlgManager_SSBasicServiceETypeToIDS(uint32 dwCode);
static uint32   CDialerApp_DlgManager_SSCLIRstrctOptionETypeToIDS(uint32 dwCode);
static uint32   CDialerApp_DlgManager_SSFwdReasonsToIDS(uint32 dwCode);
static uint32   CDialerApp_DlgManager_SSSuccessOperationETypeToIDS(uint32 dwCode);
static uint32   CDialerApp_DlgManager_OperationETypeToIDS(uint32 dwCode);
static boolean  CDialerApp_DlgManager_DisplaySSData(CDialerApp *pMe, 
                                                   AEESUPPSVC_EVENT_DATA *EventInfo);
static boolean CDialerApp_DlgManager_DisplayCCBSSet(CDialerApp *pMe, 
                                                AEECMCallID callID);
static boolean CDialerApp_DlgManager_DisplaySSCCBSInfo(CDialerApp *pMe, 
                                 AEESUPPSVC_EVENT_DATA *EventInfo);
#endif

/*===========================================================================

STATIC and PUBLIC FUNCTIONS

===========================================================================*/
/* The following values are used to indicate whether to turn display(s)
 * on or off.  It is used for turning displays off during calls when the
 * user has selected the appropriate menu item.
 */
#define CDIALERAPP_DISPLAY_OFF   0
#define CDIALERAPP_DISPLAY_ON    1

/* Yes, this is a hack, but it will go away once we get the new
 * IOverlay stuff.
 */
#ifdef FEATURE_MDP
extern uint32 coreapp_bgImg_id;
#endif

/*=============================================================================
FUNCTION: CDialerApp_SetDisplayPower

DESCRIPTION: This function turns the display(s) on or off, according to
             an input flag.

PARAMETERS:
  *pMe:     CDialerApp object pointer
   nOnOff:  Flag indicating whether to turn the display(s) on or off.
            Valid values are CDIALERAPP_DISPLAY_OFF and CDIALERAPP_DISPLAY_ON.

RETURN VALUE:
   None

COMMENTS:
   This function makes "drip" calls to the handset task and the display
   drivers, as there is currently no other way to implement this functionality.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CDialerApp_SetDisplayPower(CDialerApp *pMe, int nOnOff)
{

  if (!pMe)
  {
    MSG_ERROR("NULL pMe.", 0, 0, 0);
    return;
  }

  if (nOnOff == CDIALERAPP_DISPLAY_OFF)
  {
    disp_off();
#if defined(FEATURE_SECONDARY_DISPLAY)
    disp_off2();
#endif
  }
  else
  {
#if defined(FEATURE_SECONDARY_DISPLAY)
    int       nReturnStatus;
    boolean   bIsFlipOpen;      // Whether flip is open on a flip phone
    int       nFlipOpenSize;    // Dummy for ISHELL_GetDeviceinfoEx call
    IBitmap  *pIBitmap = NULL;  // Bitmap for the display being turned on
    AEERect   DirtyRegion;      // Rectangle to invalidate for redisplay
    AEEBitmapInfo DirtyBMPInfo; // Bitmap info for the display being turned on
    IDisplay *pIDisplay2 = NULL; // Ptr to secondary display
    IAnnunciator *pIAnnun = NULL;  // Annunciator pointer for refresh
    IDisplay *pIDispToUpdate = NULL; // Display to update at the end

    // Use a do..while(0) to simplify error handling.
    do
    {

      nFlipOpenSize = sizeof(boolean);

      /* Check clamshell status before turning the display on.  If the
       * clamshell is closed, turn on the secondary display.  If not,
       * turn on the primary.
       */
      nReturnStatus = ISHELL_GetDeviceInfoEx(pMe->a.m_pIShell,
                                             AEE_DEVICESTATE_FLIP_OPEN,
                                             (void *)&bIsFlipOpen,
                                             &nFlipOpenSize);
      if (nReturnStatus != SUCCESS)
      {
        /* If we couldn't get the status, error and default to
         * clamshell open.
         */
        MSG_MED("Unable to get clamshell status: %d.", nReturnStatus, 0, 0);
        bIsFlipOpen = TRUE;
      }

      /* Now, we turn on the appropriate display and get the necessary
       * bitmap info for forcing the update.
       */
      if ((bIsFlipOpen == TRUE)
#ifdef T_FFA6550_SS
          || (hs_query_articulated_state() == TRUE)
#endif /* T_FFA6550_SS */
          )
      {
        // The flip is open. Use the primary display.
        disp_on();
        pIDispToUpdate = pMe->a.m_pIDisplay;
        nReturnStatus = IDISPLAY_GetDeviceBitmap(pMe->a.m_pIDisplay, &pIBitmap);
        if ((SUCCESS != nReturnStatus) || (!pIBitmap))
        {
          MSG_MED("Unable to get primary display bitmap.", 0, 0, 0);
          break;
        }
  
        nReturnStatus = IBITMAP_GetInfo(pIBitmap, &DirtyBMPInfo,
                                        sizeof(AEEBitmapInfo));
        if (SUCCESS != nReturnStatus)
        {
          MSG_MED("Unable to get bmp info.", 0, 0, 0);
          break;
        }
      }
      else
      {
  
        disp_on2();
  
        nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                              AEECLSID_DISPLAY2,
                                              (void **)&pIDisplay2);
        if ((SUCCESS != nReturnStatus) || (! pIDisplay2))
        {
          MSG_MED("Unable to create secondary display.", 0, 0, 0);
          break;
        }

        pIDispToUpdate = pIDisplay2;
    
        nReturnStatus = IDISPLAY_GetDeviceBitmap(pIDisplay2, &pIBitmap);
        if ((SUCCESS != nReturnStatus) || (!pIBitmap))
        {
          MSG_MED("Unable to get secondary display bitmap.", 0, 0, 0);
          break;
        }

        nReturnStatus = IBITMAP_GetInfo(pIBitmap, &DirtyBMPInfo,
                                        sizeof(AEEBitmapInfo));
        if (SUCCESS != nReturnStatus)
        {
          MSG_MED("Unable to get bmp info.", 0, 0, 0);
          break;
        }
      }
  
      /* Now we need to update the entire screen, which means:
       * 1) Invalidate the entire bitmap
       * 2) Refresh the annunciator
       * 3) Update the background image
       * 4) Tell the display to update
       */
      DirtyRegion.x = 0;
      DirtyRegion.y = 0;
      DirtyRegion.dx = (int16)DirtyBMPInfo.cx;
      DirtyRegion.dy = (int16)DirtyBMPInfo.cy;
      IBITMAP_Invalidate(pIBitmap, &DirtyRegion);
  
      nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)pIAnnun);
      if ((SUCCESS != nReturnStatus) || !pIAnnun)
      {
        MSG_MED("Unable to create annunciator, %d.", nReturnStatus, 0, 0);
        break;
      }
      else
      {
        // Redraw the Annunciator
        if (IANNUNCIATOR_Redraw(pIAnnun) != SUCCESS)
        {
          MSG_MED("Unable to update annunciator.", 0, 0, 0);
        }
      }
  
#ifdef FEATURE_MDP
        /* Update the background overlay image */
        mdp_update_ovimg(coreapp_bgImg_id);
#endif /* FEATURE_MDP */
  
        if (pIDispToUpdate)
        {
          IDISPLAY_Update(pIDispToUpdate);
          pIDispToUpdate = NULL;
        }
      } while (0);

      if (pIDisplay2)
      {
        IDISPLAY_Release(pIDisplay2);
      }
      if (pIBitmap)
      {
        IBITMAP_Release(pIBitmap);
      }
      if (pIAnnun)
      {
        IANNUNCIATOR_Release(pIAnnun);
      }
#else // FEATURE_SECONDARY_DISPLAY
      disp_on();
#endif // FEATURE_SECONDARY_DISPLAY
 
  }
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager

DESCRIPTION: Dialer Dialog Manager (based on user and notify events) 

PARAMETERS:
  *pUser: CDialerApp object pointer
  eCode: Event code
  wParam: word event parameter
  dwParam: dword event parameter

RETURN VALUE:
  boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_DlgManager(void *pUser, AEEEvent eCode, 
                              uint16 wParam, uint32 dwParam)
{
  //lint -save -esym(715, wParam)
  CDialerApp *pMe = (CDialerApp*) pUser;
  boolean     bRetVal = TRUE;
  boolean     bFinish = FALSE;                // Go to the end of a case
#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
  boolean     bInUseFlag;
#endif
  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  switch (eCode) 
  {
  case EVT_NOTIFY:
/***************************************************************
                  IAlert Notify Events
***************************************************************/
    if (((AEENotify *) dwParam)->cls == AEECLSID_ALERT_NOTIFIER)
    {
      switch(wParam)
      {
      case NMASK_ALERT_ONOFF: 
        DIALER_MSG("NMASK_ALERT_ONOFF", 0, 0, 0);
        if((boolean) dwParam == FALSE)
        {
          (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_AutoAnswer, (void*) pMe);
        }
        return TRUE;

      case NMASK_ALERT_MUTED:
        DIALER_MSG("NMASK_ALERT_MUTED", 0, 0, 0);
        return TRUE;

      default:
        DIALER_MSG("IALERT mask %d Not Handled", wParam, 0, 0);
        return FALSE;
      }
    }

/***************************************************************
                  ICallManager Notify Events - HANDLED
***************************************************************/
    if (((AEENotify *) dwParam)->cls == AEECLSID_CM_NOTIFIER)
    {
      AEECMNotifyInfo *EventInfo = ((AEENotify *)dwParam)->pData;
      uint8 byCMCallID = EventInfo->event_data.call.call_id;

      switch(EventInfo->event)
      {
      case AEECM_EVENT_CALL_ORIG:
        DIALER_MSG("AEECM_EVENT_CALL_ORIG", 0, 0, 0);

        /* Ignore SMS calls.  We don't want to display them, as they
         * are a distraction.
         */
        if (EventInfo->event_data.call.call_info.call_type ==
            AEECM_CALL_TYPE_SMS)
        {
          DIALER_MSG("Ignoring SMS call.", 0, 0, 0);
          return FALSE;
        }

        pMe->m_byCurrOrigCall = byCMCallID;
        pMe->m_bWaitingforOrigVoiceCall = FALSE;
        (void) MEMSET(pMe->m_pszDialString, 0, (sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE));
        
        // display or refresh calling dlg
        (void) CDialerApp_CreateDlg(pMe, IDD_CALLING, NULL, 0);

        // we no longer need these dialogs if they exist
        (void) CDialerApp_EndDlgEx(pMe, IDD_NUMEDIT);
        (void) CDialerApp_EndDlgEx(pMe, IDD_WILDCARD); 

        return TRUE;

      case AEECM_EVENT_CALL_ALL_CALLS_ENDED:
        DIALER_MSG("AEECM_EVENT_CALL_ALL_CALLS_ENDED", 0, 0, 0);
        return TRUE;

      case AEECM_EVENT_CALL_END:
        {
          AEEALERTType  CurrAlert;
          IConfig      *pIConfig;
          boolean       bDisableInCallDisp = FALSE;
          int           nReturnStatus;

          DIALER_MSG("AEECM_EVENT_CALL_END", 0, 0, 0);

          /* Ignore SMS calls. */
          if (EventInfo->event_data.call.call_info.call_type ==
              AEECM_CALL_TYPE_SMS)
          {
            DIALER_MSG("Ignoring SMS call.", 0, 0, 0);
            return FALSE;
          }

          /* If the last call just ended and the "Disable display while in
           * call" config item is set, we need to turn the display(s) back on.
           * If we can't find the value, assume the flag is off and leave
           * the display(s) alone.
           */
          nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                       AEECLSID_CONFIG, (void **)&pIConfig);
          if ((nReturnStatus == SUCCESS) && (pIConfig != NULL))
          {
             nReturnStatus = ICONFIG_GetItem(pIConfig,
                                             CFGI_DISABLE_IN_CALL_DISP,
                                             &bDisableInCallDisp,
                                             sizeof(bDisableInCallDisp));
             if (nReturnStatus != SUCCESS)
             {
                MSG_LOW("Failed to get in-call display setting, %d.",
                        nReturnStatus, 0, 0);
                bDisableInCallDisp = FALSE;
             }
             (void)ICONFIG_Release(pIConfig);
          }

          if ((AEECM_IS_ANYCALL_PRESENT(pMe->m_pICM) == FALSE) &&
              (bDisableInCallDisp == TRUE))
          {
            /* We need to turn the display(s) back on. */
            CDialerApp_SetDisplayPower(pMe, CDIALERAPP_DISPLAY_ON);
          }

          if(byCMCallID == pMe->m_byCurrOrigCall)
          {
            // stop playing the ringback for GSM/WCDMA
            uisnd_snd_stop(); 
          }

          if(byCMCallID == pMe->m_byCurrIncomCall)
          {
            // stop the ringer if it is playing
            (void) IALERT_StopAlerting(pMe->m_pIALERT);
            (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_AutoAnswer, (void*) pMe);
          }

          // turn off handset if no connected voice calls
          if(AEECM_IS_VOICECALL_PRESENT(pMe->m_pICM) == FALSE)
          {
            CDialerApp_SetVocoder(pMe, FALSE);
          }

          // check if we missed this call
          if(EventInfo->event_data.call.missed == TRUE)
          {
            CMissedCallDlgInfo *pDlgInfo;
            // call was missed
            pMe->m_wNumMissedCalls++;

            pDlgInfo = (CMissedCallDlgInfo*) MALLOC(sizeof(CMissedCallDlgInfo));
            if(pDlgInfo == NULL)
            {
              DIALER_ERR("Null Pointer", 0, 0, 0);
              bRetVal = FALSE;
              bFinish = TRUE;
            }

            if (! bFinish)
            {
//lint -save -e613 Suppress warning about pDlgInfo possibly being NULL
              (void) MEMCPY(&(pDlgInfo->MissedCallInfo),
                            &(EventInfo->event_data.call.call_info),
                            sizeof(AEECMCallInfo));
              pDlgInfo->MissedCallInfo.start_time = GETTIMESECONDS();
              (void) CDialerApp_CreateDlg(pMe, IDD_MISSEDCALL, pDlgInfo,
                                          sizeof(CMissedCallDlgInfo));
              FREE(pDlgInfo);
//lint -restore
            }
          } 
          else // call has ended and was not missed
          {
            (void) CDialerApp_EndDlgEx(pMe, IDD_NUMEDIT);

            // init array to hold call info
            if(pMe->m_EndedCalls[byCMCallID] != NULL)
            {
              FREEIF(pMe->m_EndedCalls[byCMCallID]);
              pMe->m_EndedCalls[byCMCallID] = NULL;
            }

            if (pMe->m_bEmergencyFlashed) 
            {
              /* Do not show ended calls if voice call was converted to
               * emergency */
              pMe->m_bEmergencyFlashed = FALSE;
            } 
            else 
            {
              // record info for ended call
              pMe->m_EndedCalls[byCMCallID] = (CEndedCall*) MALLOC(sizeof(CEndedCall));
              if(pMe->m_EndedCalls[byCMCallID] == NULL)
              {
                DIALER_ERR("No memory", 0,0,0);
                bRetVal = FALSE;
                bFinish = TRUE;
              }

              if (! bFinish)
              {
                (void) MEMCPY(&(pMe->m_EndedCalls[byCMCallID]->CallInfo),
                              &(EventInfo->event_data.call.call_info),
                              sizeof(AEECMCallInfo));
                pMe->m_EndedCalls[byCMCallID]->dwDisplayStart = 0;
                (void) CDialerApp_DlgManager_DisplayCallEndCause(pMe,
                         (unsigned)(int)(EventInfo->event_data.call.end_status),
                         EventInfo->event_data.call.end_additional_info);
              }
            }
          }
          if (! bFinish)
          {

            // remove call related dialogs
            (void) CDialerApp_EndCallRelatedDlgs(pMe, byCMCallID);

            // check to see if call waiting alert needs to be changed to a normal ring alert
            if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM) == FALSE)
            {
              if(ICM_GetActiveCallIDs(pMe->m_pICM, (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                                                 AEECM_CALL_TYPE_EMERGENCY|
                                                                 AEECM_CALL_TYPE_NON_STD_OTASP|
                                                                 AEECM_CALL_TYPE_STD_OTASP),
                (AEECMCallState)AEECM_CALL_STATE_INCOM, NULL, 0) > 0)
              {
                (void) IALERT_GetInfo(pMe->m_pIALERT, NULL, NULL, 0, &CurrAlert);

                if(CurrAlert == AEEALERT_ALERT_NONE)
                {
                  (void) IALERT_StartAlerting(pMe->m_pIALERT, AEEALERT_CALLTYPE_VOICE, NULL, AEEALERT_ALERT_NORMAL);
                  // check to see if auto answer is on
                  if(CDialerApp_NVCheck_IsAutoAnswer() == TRUE)
                  {
                    (void) ISHELL_SetTimer(pMe->a.m_pIShell, DIALERAPP_AUTOANSWER_TIME, 
                      (PFNNOTIFY) CDialerApp_AutoAnswer, (void*) pMe);
                  }
                }
              }
            }
          }

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
          if (bRetVal)
          {
            /* Only update the secondary display state if we were able to do
             * so for the primary display.  This update is done for the case
             * where we get an "END" for an incoming or originating call.
             */
            /* If we determined that this was a missed call earlier, don't go 
             * to status, as that will show a time in call dialog */
            if (pMe->m_DispState != DIALER_DISP_STATE_MISSEDCALL)
            {
              pMe->m_DispState = DIALER_DISP_STATE_STATUS;
            }
          }
#endif //FEATURE_UI_SECONDARY_DISP_SUPPORT

        }

        return bRetVal;

      case AEECM_EVENT_CALL_INCOM:
        {
          AEEALERTType AlertType;
          DIALER_MSG("AEECM_EVENT_CALL_INCOM", 0, 0, 0);

          /* Ignore SMS calls.  We don't want to display them, as they
           * are a distraction.
           */
          if (EventInfo->event_data.call.call_info.call_type ==
              AEECM_CALL_TYPE_SMS)
          {
            DIALER_MSG("Ignoring SMS call.", 0, 0, 0);
            return FALSE;
          }

          pMe->m_byCurrIncomCall = byCMCallID;

          if(pMe->m_bSuspended == TRUE)
          {
            pMe->m_bStartAlerting = TRUE;
          }
          else
          {
            (void) IALERT_GetInfo(pMe->m_pIALERT, NULL, NULL, 0, &AlertType);

            // start the ringer
            if(AlertType == (AEEALERTType)AEEALERT_ALERT_BUSY)
            {
              CDialerApp_StopCallFailedSound(pMe);
              AlertType = AEEALERT_ALERT_NONE;
            }
          
            if(AlertType == (AEEALERTType)AEEALERT_ALERT_NONE)
            {
              if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM) == TRUE)
              {
                // play call waiting ring
                (void) IALERT_StartAlerting(pMe->m_pIALERT, AEEALERT_CALLTYPE_VOICE, 
                  EventInfo->event_data.call.call_info.other_party_no, 
                  (AEEALERTType)AEEALERT_ALERT_CALLWAITING);
              }
              else
              {
                // play normal ring
                (void) IALERT_StartAlerting(pMe->m_pIALERT, AEEALERT_CALLTYPE_VOICE, 
                  EventInfo->event_data.call.call_info.other_party_no, 
                  AEEALERT_ALERT_NORMAL);
              }
            }
            // check to see if auto answer is on
            if(CDialerApp_NVCheck_IsAutoAnswer() == TRUE)
            {
              (void) ISHELL_SetTimer(pMe->a.m_pIShell, DIALERAPP_AUTOANSWER_TIME, 
                                                    (PFNNOTIFY) CDialerApp_AutoAnswer, (void*) pMe);
            }
          }
          (void) CDialerApp_CreateDlg(pMe, IDD_INCOMING, NULL, 0);
        }
        return TRUE;

      case AEECM_EVENT_CALL_CONNECT:
        {
          IConfig *pIConfig;
          boolean  bDisableInCallDisp = FALSE;
          int      nReturnStatus;

          DIALER_MSG("AEECM_EVENT_CALL_CONNECT", 0, 0, 0);
  
          /* Ignore SMS calls.  We don't want to display them, as they
           * are a distraction.
           */
          if (EventInfo->event_data.call.call_info.call_type ==
              AEECM_CALL_TYPE_SMS)
          {
            DIALER_MSG("Ignoring SMS call.", 0, 0, 0);
            return FALSE;
          }

          /* Determine if the "Disable display while in call" config item
           * is set.  If so, turn off the display.  Otherwise, leave
           * the display alone.  Default behavior is always to leave the
           * display on.
           */
          nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                       AEECLSID_CONFIG, (void **)&pIConfig);
          if ((nReturnStatus == SUCCESS) && (pIConfig != NULL))
          {
             nReturnStatus = ICONFIG_GetItem(pIConfig,
                                             CFGI_DISABLE_IN_CALL_DISP,
                                             &bDisableInCallDisp,
                                             sizeof(bDisableInCallDisp));
             if (nReturnStatus != SUCCESS)
             {
                MSG_LOW("Failed to get in-call display setting, %d.",
                        nReturnStatus, 0, 0);
                bDisableInCallDisp = FALSE;
             }
             (void)ICONFIG_Release(pIConfig);
          }

          if (bDisableInCallDisp == TRUE)
          {
            MSG_LOW("Turning off displays while in call.", 0, 0, 0);
            CDialerApp_SetDisplayPower(pMe, CDIALERAPP_DISPLAY_OFF);
          }
         
          if(byCMCallID == pMe->m_byCurrIncomCall)
          {
            // stop the ringer
           (void) IALERT_StopAlerting(pMe->m_pIALERT);
           (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_AutoAnswer, (void*) pMe);
          }
  
          
          if(AEECM_IS_CALLTYPE_VOICE(EventInfo->event_data.call.call_info.call_type) == TRUE )
          {
            CDialerApp_SetVocoder(pMe, TRUE);
          }
  
          // set NV var
          pMe->m_byNVDTMF = CDialerApp_NV_GetDTMF();
          
          // special case for time and hard pauses
          if(byCMCallID == pMe->m_byCurrOrigCall)
          {
            // stop playing the ringback for GSM/WCDMA
            uisnd_snd_stop(); 
  
            // check to see if we have more dtmfs to send
            if(WSTRLEN(pMe->m_pszTimePauseRemaining) > 0)
            {
              pMe->m_byCurrTimePauseCall = byCMCallID;
              (void) CDialerApp_EndCallRelatedDlgs(pMe, byCMCallID);
              (void) CDialerApp_CreateDlg(pMe, IDD_TIMEPAUSE, NULL, 0);
              bFinish = TRUE;
            }
          }
          
          if (! bFinish)
          {
            // remove the missed call dialog since it really is no longer important
            (void) CDialerApp_EndDlgEx(pMe, IDD_MISSEDCALL);
            (void) CDialerApp_EndCallRelatedDlgs(pMe, byCMCallID);
          }

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
          // update secondary display data
          pMe->m_DispState = DIALER_DISP_STATE_STATUS;
          pMe->m_bIsActiveCalls = TRUE;
#endif //FEATURE_UI_SECONDARY_DISP_SUPPORT
  
          (void) CDialerApp_RefreshDlg(pMe);  
        }
        return TRUE;
  
      case AEECM_EVENT_CALL_ERROR:     
        DIALER_MSG("AEECM_EVENT_CALL_ERROR: %d", EventInfo->event_data.call.call_error, 0, 0); 
        (void) CDialerApp_EndDlgEx(pMe, IDD_NUMEDIT);

        if(EventInfo->event_data.call.call_error == AEECM_CALL_ERROR_IN_USE_S)
        {
          // suppress displaying this error message
          DIALER_ERR("ICM ERROR: Can't perform command because phone is in use",0,0,0);
          (void) CDialerApp_RefreshDlg(pMe);
          return TRUE;
        }
        (void) CDialerApp_DlgManager_DisplayCallError(pMe, (uint32)EventInfo->event_data.call.call_error);
        return TRUE;

      case AEECM_CALL_EVENT_NSS_REL_REC:     
        DIALER_MSG("AEECM_CALL_EVENT_NSS_REL_REC CallId: %d", EventInfo->event_data.call.call_id, 0, 0); 
        return (CDialerApp_HandleNSSRelRecord(pMe, EventInfo->event_data.call.call_id));

       case AEECM_EVENT_CALL_OPS_ERROR:
         DIALER_MSG("AEECM_EVENT_CALL_OPS_ERROR: %d", EventInfo->event_data.call.callops_error, 0, 0);
         (void) CDialerApp_EndDlgEx(pMe, IDD_CALL_OPTIONS);
         (void) CDialerApp_DlgManager_DisplaySSError(pMe, (uint32)EventInfo->event_data.call.callops_error);
         return TRUE;

       case AEECM_EVENT_CALL_OPS:         
         DIALER_MSG("AEECM_EVENT_CALL_OPS", 0, 0, 0);
         // do not need these dialogs anymore
         (void) CDialerApp_EndDlgEx(pMe, IDD_CALL_OPTIONS);
         (void) CDialerApp_EndDlgEx(pMe, IDD_NUMEDIT);
         (void) CDialerApp_EndDlgEx(pMe, IDD_WILDCARD);
         if(byCMCallID == pMe->m_byCurrIncomCall)
         {
           (void) CDialerApp_EndDlgEx(pMe, IDD_FORWARDING);
           (void) CDialerApp_EndDlgEx(pMe, IDD_INCOM_OPTIONS);
           (void) CDialerApp_EndDlgEx(pMe, IDD_INCOMING);     
           (void) CDialerApp_EndDlgEx(pMe, IDD_SETUP_RECALL);    
         }
         (void) CDialerApp_RefreshDlg(pMe);
         return TRUE;

       case AEECM_EVENT_CALL_CALLER_ID:     /* Caller ID info was received from BS */
         DIALER_MSG("AEECM_EVENT_CALL_CALLER_ID", 0, 0, 0);
         if(EventInfo->event_data.call.call_info.call_state == AEECM_CALL_STATE_INCOM)
         {
           // new caller id info so refresh incoming screen
           (void) CDialerApp_RefreshDlg(pMe);
         }
         else
         {
           AEEALERTType AlertType;
           // this is call waiting notification
           pMe->m_byCurrIncomCall = byCMCallID;

           (void) IALERT_GetInfo(pMe->m_pIALERT, NULL, NULL, 0, &AlertType);

           if(AlertType == AEEALERT_ALERT_NONE)
           {
             // play call waiting ringer
             (void) IALERT_StartAlerting(pMe->m_pIALERT, AEEALERT_CALLTYPE_VOICE, 
               EventInfo->event_data.call.call_info.other_party_no, 
               AEEALERT_ALERT_CALLWAITING);
           }

           // copy call waiting info
	   //if Presentation Restriction is set, then overwrite nubmer buffer.
	   if(EventInfo->event_data.call.number.pi == AEECM_PI_RES) {
	     (void) ISHELL_LoadResString(pMe->a.m_pIShell,
					 DIALERAPP_RES_FILE,
					 IDS_RESTRICTED_CALL,
					 pMe->m_pszCallWaitingNum, 
					 AEECM_MAX_DIAL_STRING
					 *sizeof(AECHAR));
	   } else {
	     (void) WSTRNCOPY(pMe->m_pszCallWaitingNum, DIALERAPP_NUMBER_BUFFER_SIZE, EventInfo->event_data.call.number.buf);
	   }
	   
           (void) CDialerApp_CreateDlg(pMe, IDD_INCOMING, NULL, 0);      
         }
         return TRUE;

       case AEECM_EVENT_CALL_BARRED:         // call barred notification */
         DIALER_MSG("AEECM_EVENT_CALL_BARRED", 0, 0, 0);
         CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_CALL_BARRED, 
           DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
         return TRUE;

       case AEECM_EVENT_CALL_IS_WAITING:     // call is in waiting notification */
         DIALER_MSG("AEECM_EVENT_CALL_IS_WAITING", 0, 0, 0);
         CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_CALL_IS_WAITING, 
           DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);    
         return TRUE;

       case AEECM_EVENT_CALL_ON_HOLD:        // call on hold notification */ 
         DIALER_MSG("AEECM_EVENT_CALL_ON_HOLD", 0, 0, 0);
         return CDialerApp_RefreshDlg(pMe);  
          
       case AEECM_EVENT_CALL_RETRIEVED:      // call retrieved notification */
         DIALER_MSG("AEECM_EVENT_CALL_RETRIEVED", 0, 0, 0);
         return CDialerApp_RefreshDlg(pMe);     
          
       case AEECM_EVENT_CALL_ORIG_FWD_STATUS: // call may be forwarded (to forwarding MS)*/
         DIALER_MSG("AEECM_EVENT_CALL_ORIG_FWD_STATUS", 0, 0, 0);
         CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_CALL_FWD_ACTIVE, 
           DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
         return TRUE;
          
       case AEECM_EVENT_CALL_FORWARDED:      // call forwarded (to forwarding MS) */
         DIALER_MSG("AEECM_EVENT_CALL_FORWARDED", 0, 0, 0);
         CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_CALL_FWDED, 
           DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
         return TRUE;
          
       case AEECM_EVENT_CALL_BEING_FORWARDED:// call being forwarded (to calling MS) */
         DIALER_MSG("AEECM_EVENT_CALL_BEING_FORWARDED", 0, 0, 0);
         CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_CALL_BEING_FWD, 
           DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
         return TRUE;
    
       case AEECM_EVENT_CALL_INCOM_FWD_CALL: // Incoming forwarded call notification */
         DIALER_MSG("AEECM_EVENT_CALL_INCOM_FWD_CALL", 0, 0, 0);
         CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_FORWARDINCOMINGCALL, 
           DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
         return TRUE;
       case AEECM_EVENT_CALL_RESTRICTED: // call restricted notification */    
         DIALER_MSG("AEECM_EVENT_CALL_RESTRICTED", 0, 0, 0);
         CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_CALL_RESTRICTED, 
           DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
         return TRUE;

       case AEECM_EVENT_CALL_DEFLECTION:     // Call deflection notification */
         DIALER_MSG("AEECM_EVENT_CALL_DEFLECTION", 0, 0, 0);
         CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_CALL_DEFLECTED, 
           DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
         return TRUE;
          
       case AEECM_EVENT_CALL_TRANSFERRED:    // Call transfer notification */
         DIALER_MSG("AEECM_EVENT_CALL_TRANSFERRED", 0, 0, 0);
         CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_CALL_TRANSFERRED, 
           DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
         return TRUE;

       case AEECM_EVENT_CALL_DISPLAY:        // display info was received from BS. */
         DIALER_MSG("AEECM_EVENT_CALL_DISPLAY", 0, 0, 0);
         if(WSTRLEN(EventInfo->event_data.call.call_info.alpha) != 0)
         {
           // display alpha on the notif dlg
           CDialerApp_NotifDlg_DisplayMsgEx(pMe, EventInfo->event_data.call.call_info.alpha, 
             DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
           return TRUE;
         }
         (void) CDialerApp_RefreshDlg(pMe);
         return TRUE;

       case AEECM_EVENT_CALL_CALLED_PARTY:   // called party info was received from BS. */
         DIALER_MSG("AEECM_EVENT_CALL_CALLED_PARTY", 0, 0, 0);
         (void) CDialerApp_RefreshDlg(pMe);
         return TRUE;

       case AEECM_EVENT_CALL_CONNECTED_NUM:  // connected number info was received */
         DIALER_MSG("AEECM_EVENT_CALL_CONNECTED_NUM", 0, 0, 0);
         (void) CDialerApp_RefreshDlg(pMe);
         return TRUE;

       case AEECM_EVENT_CALL_CUG_INFO_RECEIVED:   // call forward CUG notification */
         DIALER_MSG("AEECM_EVENT_CALL_CUG_INFO_RECEIVED", 0, 0, 0);
         (void) CDialerApp_RefreshDlg(pMe);
         return TRUE;
          
       case AEECM_EVENT_CALL_CNAP_INFO_RECEIVED:  // caller name info notification */
         DIALER_MSG("AEECM_EVENT_CALL_CNAP_INFO_RECEIVED", 0, 0, 0);
         (void) CDialerApp_RefreshDlg(pMe);
         return TRUE;
          
       case AEECM_EVENT_CALL_EMERGENCY_FLASHED:   // voice call converted to emergency */
         DIALER_MSG("AEECM_EVENT_CALL_EMERGENCY_FLASHED", 0, 0, 0);
         (void) CDialerApp_RefreshDlg(pMe);
         return TRUE;
          
       case AEECM_EVENT_CALL_SRV_OPT:        // Service option changed */
         DIALER_MSG("AEECM_EVENT_CALL_SRV_OPT", 0, 0, 0);
         (void) CDialerApp_RefreshDlg(pMe);
         return TRUE;
          
       case AEECM_EVENT_CALL_PRIVACY:           // Privacy mode changed */  
         DIALER_MSG("AEECM_EVENT_CALL_PRIVACY", 0, 0, 0);
         (void) CDialerApp_RefreshDlg(pMe);
         return TRUE;
          
       case AEECM_EVENT_CALL_ENTER_DORMANT:     // for AEECM_CALL_EVENT_RAB_REL_IND ? */
         DIALER_MSG("AEECM_EVENT_CALL_ENTER_DORMANT", 0, 0, 0);
         (void) CDialerApp_RefreshDlg(pMe);
         return TRUE;
          
       case AEECM_EVENT_CALL_EXIT_DORMANT:      // for AEECM_CALL_EVENT_RAB_REESTAB_REQ */
         DIALER_MSG("AEECM_EVENT_CALL_EXIT_DORMANT", 0, 0, 0);
         (void) CDialerApp_RefreshDlg(pMe);
         return TRUE;

       case AEECM_EVENT_CALL_SIGNAL:            // signal info was received from BS */
         DIALER_MSG("AEECM_EVENT_CALL_SIGNAL", 0, 0, 0);
         if(EventInfo->event_data.call.signal.is_signal_info_avail == TRUE)
         {

            (void) CDialerApp_DlgManager_PlaySignal(pMe, EventInfo->event_data.call.call_info.other_party_no, 
                                             &(EventInfo->event_data.call.signal));
         }
         return TRUE;

       case AEECM_EVENT_CALL_PROGRESS_INFO_IND: // call origination progress indication */
         DIALER_MSG("AEECM_EVENT_CALL_PROGRESS_INFO_IND", 0, 0, 0);
         // set NV var
         CDialerApp_HandleProgressInfo(pMe, byCMCallID);
         pMe->m_byNVDTMF = CDialerApp_NV_GetDTMF();
         return TRUE;

       case AEECM_EVENT_CALL_ABRV_ALERT:        // CMDA/AMPS abbreviated alert. */
         DIALER_MSG("AEECM_EVENT_CALL_ABRV_ALERT", 0, 0, 0);
         CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_ABBR_ALERT);
         return TRUE;
          
       case AEECM_EVENT_CALL_ABRV_REORDER:      // AMPS abbreviated reorder. */
         DIALER_MSG("AEECM_EVENT_CALL_ABRV_REORDER", 0, 0, 0);
         CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_ABBR_REORDER);
         return TRUE;
          
       case AEECM_EVENT_CALL_ABRV_INTERCEPT:    // AMPS abbreviated intercept. */
         DIALER_MSG("AEECM_EVENT_CALL_ABRV_INTERCEPT", 0, 0, 0);
         CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_ABBR_INTERCEPT);
         return TRUE;

       case AEECM_EVENT_CALL_RECALL_IND:
         DIALER_MSG("AEECM_EVENT_CALL_RECALL_IND", 0, 0, 0);
         {
           CRecallDlgInfo *pDlgInfo;

           pMe->m_byCurrRecallID = byCMCallID;

           pDlgInfo = (CRecallDlgInfo*) MALLOC(sizeof(CRecallDlgInfo));
           if(pDlgInfo == NULL)
           {
             DIALER_ERR("Null Pointer", 0, 0, 0);
             return FALSE;
           }

           pDlgInfo->callID = byCMCallID;
           (void) CDialerApp_CreateDlg(pMe, IDD_RECALL, pDlgInfo,
                                          sizeof(CRecallDlgInfo));
           FREE(pDlgInfo);

           //check to see if auto answer is on
           if(CDialerApp_Check_IsCCBSAutoAccept(pMe) != OEM_CCBS_NONE)
           {
             (void) ISHELL_SetTimer(pMe->a.m_pIShell, DIALERAPP_AUTOANSWER_TIME, 
              (PFNNOTIFY) CDialerApp_CCBSAutoAccept, (void*) pMe);
           }
         }
         return TRUE;

      case AEECM_EVENT_CALL_CCBS_ALLOWED:
         DIALER_MSG("AEECM_EVENT_CALL_CCBS_ALLOWED", 0, 0, 0);
         {
           CRecallDlgInfo *pDlgInfo;

           pDlgInfo = (CRecallDlgInfo*) MALLOC(sizeof(CRecallDlgInfo));
           if(pDlgInfo == NULL)
           {
             DIALER_ERR("Null Pointer", 0, 0, 0);
             return FALSE;
           }

           pDlgInfo->callID = byCMCallID;
           (void) CDialerApp_CreateDlg(pMe, IDD_SETUP_RECALL, pDlgInfo,
                                          sizeof(CRecallDlgInfo));
           FREE(pDlgInfo);
         }
         return TRUE;


      case AEECM_EVENT_CALL_CCBS_SET:
#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM)
        DIALER_MSG("AEECM_EVENT_CALL_CCBS_SET", 0, 0, 0);
        CDialerApp_DlgManager_DisplayCCBSSet(pMe, EventInfo->event_data.call.call_id);
#endif
        return TRUE;
        
      case AEECM_EVENT_CALL_ANSWER:             // call was answered
        if(byCMCallID == pMe->m_byCurrIncomCall)
        {
          // stop the ringer
         (void) IALERT_StopAlerting(pMe->m_pIALERT);
         (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_AutoAnswer, (void*) pMe);
        }

        // unmute the handset for voice calls
        if(AEECM_IS_CALLTYPE_VOICE(EventInfo->event_data.call.call_info.call_type) == TRUE)
        {
          CDialerApp_SetVocoder(pMe, TRUE);
        }

        DIALER_MSG("AEECM_EVENT_CALL_ANSWER", 0, 0, 0);
        return TRUE;

      case AEECM_EVENT_PH_IN_USE_STATE:
        DIALER_MSG("AEECM_EVENT_PH_IN_USE_STATE", 0, 0, 0);
#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
        bInUseFlag = EventInfo->event_data.ph.is_in_use;
        if (bInUseFlag)
        {
          /* A call is starting. Set up the data to prepare for displaying
           * status info to secondary display.
           */

          // Set up the timestamps
          pMe->m_dwStartTS = GETUPTIMEMS();
          pMe->m_dwEndTS = 0;
          pMe->m_bIsActiveCalls = TRUE;

          /* Set the secondary display state to "status" if we're not displaying 
           * something else.  Otherwise, let the other display info (e.g. incoming
           * or originating call) take precedence.
           */
          if (DIALER_DISP_STATE_NONE == pMe->m_DispState ||
              DIALER_DISP_STATE_MISSEDCALL == pMe->m_DispState)
          {
            pMe->m_DispState = DIALER_DISP_STATE_STATUS;
          }

          // Set (periodic) callback to update "status" information
          (void)ISHELL_SetTimer(pMe->a.m_pIShell, DIALERAPP_SEC_STATUS_TIMEOUT,
                                CDialerApp_SecStatusCB, (void *)pMe);

        }
        else
        {
          /* The last call ended. Set up the data to display the total
           * time of the entire call session.
           */
          pMe->m_dwEndTS = GETUPTIMEMS();
          pMe->m_bIsActiveCalls = FALSE;
          if (pMe->m_DispState != DIALER_DISP_STATE_MISSEDCALL)
          {
            pMe->m_DispState = DIALER_DISP_STATE_STATUS;
          }
        }
#endif //FEATURE_UI_SECONDARY_DISP_SUPPORT
        (void) CDialerApp_RefreshDlg(pMe);  
 
        return TRUE;

  
/***************************************************************
                  ICallManager Notify Events - NOT HANDLED
***************************************************************/
        
  
      default:
        DIALER_MSG("ICM Event %d Not Handled", EventInfo->event, 0, 0);
        return FALSE;
      }
    }
    
/***************************************************************
                     ISuppSvc Notify Events
***************************************************************/
#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM)
    if (((AEENotify *) dwParam)->cls == AEECLSID_SUPPSVC_NOTIFIER)
    {
      AEESUPPSVC_EVENT_DATA *EventInfo = (((AEENotify *) dwParam)->pData);  

      switch (((AEENotify *) dwParam)->dwMask )
      {
      case NMASK_SS_END:
        {  
          DIALER_MSG("NMASK_SS_END", 0, 0, 0);

          // we no longer need these dialogs if they exist
          (void) CDialerApp_EndDlgEx(pMe, IDD_NUMEDIT);
          (void) CDialerApp_EndDlgEx(pMe, IDD_WILDCARD);
          (void) CDialerApp_EndDlgEx(pMe, IDD_SSREQUEST);

          switch(EventInfo->evt)
          {
          case AEESUPPSVC_EVENT_ERROR:
            DIALER_MSG("AEESUPPSVC_EVENT_ERROR", 0, 0, 0);
            CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_PLEASE_TRY_AGAIN, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
            return TRUE;

          case AEESUPPSVC_EVENT_RELEASE: 
            DIALER_MSG("AEESUPPSVC_EVENT_RELEASE", 0, 0, 0);
            //CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_PLEASE_TRY_AGAIN, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
            return TRUE;

          default:
            return FALSE;
          }      
        }
      

      case NMASK_SS_STARTED:
        {
          CSSRequestDlgInfo *DlgInfo;

          DIALER_MSG("NMASK_SS_STARTED", 0, 0, 0);

          DlgInfo = (CSSRequestDlgInfo*) MALLOC(sizeof(CSSRequestDlgInfo));
          if(DlgInfo == NULL)
          {
            DIALER_ERR("No Memory", 0, 0, 0);
            return TRUE;
          }

          if(EventInfo->alpha)
          {
            (void) WSTRNCOPY(DlgInfo->szAlpha, DIALERAPP_MAX_NOTIF_BUFFER_SIZE, EventInfo->alpha);
          }
          DlgInfo->byReqID = (uint8) EventInfo->requestId;
          DlgInfo->ssOper = (AEESuppSvcSSOperationType) EventInfo->ss_operation;
          DlgInfo->ssCode = (AEESuppSvcSSCodeType)EventInfo->ss_code;

          (void) CDialerApp_CreateDlg(pMe, IDD_SSREQUEST, DlgInfo, sizeof(CSSRequestDlgInfo));

          // we no longer need these dialogs if they exist
          (void) CDialerApp_EndDlgEx(pMe, IDD_NUMEDIT);
          (void) CDialerApp_EndDlgEx(pMe, IDD_WILDCARD);

          FREE(DlgInfo);

          return TRUE;
        }

      case NMASK_SS_NW_RESPONSE:
        DIALER_MSG("NMASK_SS_NW_RESPONSE", 0, 0, 0);
        // Check for Password Dlg, if it is up, then end it.
        (void) CDialerApp_EndDlgEx(pMe, IDD_USSD_INFO);
        (void) CDialerApp_EndDlgEx(pMe, IDD_USSD_PROMPT);
        (void) CDialerApp_EndDlgEx(pMe, IDD_USSD_PASSWORD);
        return TRUE;

      case NMASK_SS_NW_INITIATED:
      { 
        CUSSDDlgInfo *pDlgInfo;

        DIALER_MSG("NMASK_SS_NW_INITIATED", 0, 0, 0);

        pDlgInfo = MALLOC(sizeof(CUSSDDlgInfo));
        if(pDlgInfo == NULL)
        {
          DIALER_ERR("No memory", 0,0,0);
          return FALSE;
        }

        pDlgInfo->SSResponse.length = 0;
        pDlgInfo->SSResponse.user_input = NULL; 
        pDlgInfo->SSResponse.invoke_id = (unsigned)(int)EventInfo->invoke_id;

        switch(EventInfo->evt)
        { 
        case AEESUPPSVC_EVENT_GET_PASSWORD_IND: 
          pDlgInfo->ssOper = AEESUPPSVC_REL_GET_PWD_IND;
          switch(EventInfo->evt_data.ss_ind.pwd_type)
          {
          case AEESUPPSVC_ENTER_PWD:
            // Enter old Password
            (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_ENTERPASS, pDlgInfo->szNWInitiatedData, 
              ((AEESUPPSVC_MAX_USS_LENGTH)*sizeof(AECHAR)));
            break;

          case AEESUPPSVC_ENTER_NEWPWD:
            // Enter New Password
            (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_SS_SS_NEW_PASSWORD, pDlgInfo->szNWInitiatedData, 
              ((AEESUPPSVC_MAX_USS_LENGTH)*sizeof(AECHAR)));
            break;

          case AEESUPPSVC_ENTER_NEWPWD_AGAIN: 
            // Enter New Password Again
            (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_SS_SS_REENTER_PASSWORD, pDlgInfo->szNWInitiatedData, 
              ((AEESUPPSVC_MAX_USS_LENGTH)*sizeof(AECHAR)));
            break;

          case AEESUPPSVC_MAX_PWD:
          default:
            DIALER_ERR("Unknown pwd_type %d", EventInfo->evt_data.ss_ind.pwd_type, 0, 0);
            break;
          }

          switch(EventInfo->evt_data.ss_ind.pwd_type)
          {
          case AEESUPPSVC_ENTER_PWD:
            // Enter old Password
          case AEESUPPSVC_ENTER_NEWPWD:
            // Enter New Password
          case AEESUPPSVC_ENTER_NEWPWD_AGAIN: 
            // Enter New Password Again
            (void) CDialerApp_CreateDlg(pMe, IDD_USSD_PASSWORD, pDlgInfo, sizeof(CUSSDDlgInfo)); 
            break;

          default:
            DIALER_ERR("Unknown pwd_type %d", EventInfo->evt_data.ss_ind.pwd_type, 0, 0);
            FREE(pDlgInfo);
            return  FALSE;
          }
          break;


        case AEESUPPSVC_EVENT_USS_NOTIFY_IND:
          pDlgInfo->ssOper = AEESUPPSVC_REL_USS;
          pDlgInfo->SSResponse.resp_type = AEESUPPSVC_USS_NOTIFY_RES;
          (void) WSTRNCOPY(pDlgInfo->szNWInitiatedData, AEESUPPSVC_MAX_USS_LENGTH + 1, EventInfo->evt_data.ss_ind.uss_data.data);
          (void) CDialerApp_CreateDlg(pMe, IDD_USSD_PROMPT, pDlgInfo, sizeof(CUSSDDlgInfo));  
          break;


        case AEESUPPSVC_EVENT_USS_IND:
          pDlgInfo->ssOper = AEESUPPSVC_REL_PROCESS_USS;
          pDlgInfo->SSResponse.resp_type = AEESUPPSVC_USS_RES;
          pDlgInfo->SSResponse.USSDdcs = EventInfo->evt_data.ss_ind.USSDdcs;
          (void) WSTRNCOPY(pDlgInfo->szNWInitiatedData, AEESUPPSVC_MAX_USS_LENGTH + 1, EventInfo->evt_data.ss_ind.uss_data.data);
          (void) CDialerApp_CreateDlg(pMe, IDD_USSD_INFO, pDlgInfo, sizeof(CUSSDDlgInfo)); 
          break;

        default:
          FREE(pDlgInfo);
          return FALSE;
        } 

        FREE(pDlgInfo);
        return TRUE;
      }
 
      case NMASK_SS_COMPLETE:
        {
          DIALER_MSG("NMASK_SS_COMPLETE", 0, 0, 0);   

          // we no longer need these dialogs if they exist
          (void) CDialerApp_EndDlgEx(pMe, IDD_SSREQUEST);

          if(EventInfo->evt  == (int)AEESUPPSVC_EVENT_CHANGE_PIN || 
             EventInfo->evt  == (int)AEESUPPSVC_EVENT_UNBLOCK_PIN ||
             EventInfo->evt  == (int)AEESUPPSVC_EVENT_DISPLAY_IMEI )
          {
             CDialerApp_NotifDlg_DisplayMsgEx(pMe, EventInfo->evt_data.data, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
             return TRUE;
          } 
 
          else if((EventInfo->evt == (int)AEESUPPSVC_EVENT_REGISTER_CONF)     ||  
                  (EventInfo->evt == (int)AEESUPPSVC_EVENT_ACTIVATE_CONF)     ||
                  (EventInfo->evt == (int)AEESUPPSVC_EVENT_DEACTIVATE_CONF)   ||
                  (EventInfo->evt == (int)AEESUPPSVC_EVENT_ERASE_CONF)        ||
                  (EventInfo->evt == (int)AEESUPPSVC_EVENT_INTERROGATE_CONF)  ||
                  (EventInfo->evt == (int)AEESUPPSVC_EVENT_REG_PASSWORD_CONF) ||
                  (EventInfo->evt == (int)AEESUPPSVC_EVENT_PROCESS_USS_CONF))
          { 
             switch(EventInfo->evt_data.ss_complete.conf_type)
             {
             case AEESUPPSVC_SS_ERROR_INFO:
             case AEESUPPSVC_CC_CAUSE_INFO:
               DIALER_MSG("AEESUPPSVC_SS_ERROR_INFO", 0, 0, 0);
               //Add Failed to ssCode Buf and ssOpBuf
               //Display - SS Code Failed
               return CDialerApp_DlgManager_DisplayError(pMe, EventInfo);
     
             

              case AEESUPPSVC_CALL_BARRING_INFO:
                DIALER_MSG("AEESUPPSVC_CALL_BARRING_INFO", 0, 0, 0);
                return CDialerApp_DlgManager_DisplaySSCallBarringInfo(pMe, EventInfo);

              case AEESUPPSVC_FWD_INFO:
                DIALER_MSG("DialerDlgManager: AEESUPPSVC_FWD_INFO", 0, 0, 0);
                if (AEESUPPSVC_EVENT_REGISTER_CONF == EventInfo->evt)
                {
                   return FALSE;
                }
                else 
                {
                   return CDialerApp_DlgManager_DisplaySSForwardingInfo(pMe, EventInfo);
                }

              case AEESUPPSVC_FWD_FEATURE_LIST_INFO:
                DIALER_MSG("AEESUPPSVC_FWD_FEATURE_LIST_INFO", 0, 0, 0);
                return CDialerApp_DlgManager_DisplaySSForwardingFeatureInfo(pMe, EventInfo);

              case AEESUPPSVC_SS_STATUS_INFO:
                DIALER_MSG("AEESUPPSVC_SS_STATUS_INFO", 0, 0, 0);
                return CDialerApp_DlgManager_DisplaySSStatus(pMe, EventInfo);

              case AEESUPPSVC_CLI_RESTRICTION_INFO:
                DIALER_MSG("AEESUPPSVC_CLI_RESTRICTION_INFO", 0, 0, 0);
                return CDialerApp_DlgManager_DisplaySSCLIRestriction(pMe, EventInfo);
              
              case AEESUPPSVC_CUG_INFO:
                DIALER_MSG("AEESUPPSVC_CUG_INFO", 0, 0, 0);
                return CDialerApp_DlgManager_DisplayCUG(pMe, &(EventInfo->evt_data.ss_complete.complete.cug_info));

              case AEESUPPSVC_BSG_LIST_INFO:
                DIALER_MSG("AEESUPPSVC_BSG_LIST_INFO", 0, 0, 0);
                return CDialerApp_DlgManager_DisplayBSGList(pMe, EventInfo);

              case AEESUPPSVC_USS_DATA_INFO:
                DIALER_MSG("AEESUPPSVC_USS_DATA_INFO", 0, 0, 0);
                CDialerApp_NotifDlg_DisplayMsgEx(pMe, EventInfo->evt_data.ss_complete.complete.uss_data_info.data, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
                return TRUE;   

              case AEESUPPSVC_USSD_DATA_INFO:
                DIALER_MSG("AEESUPPSVC_USSD_DATA_INFO", 0, 0, 0);
                CDialerApp_NotifDlg_DisplayMsgEx(pMe, EventInfo->evt_data.ss_complete.complete.ussd_data_info.data, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
                return TRUE;

              case AEESUPPSVC_SS_DATA_INFO:
                DIALER_MSG("AEESUPPSVC_SS_DATA_INFO", 0, 0, 0);
                return CDialerApp_DlgManager_DisplaySSData(pMe, EventInfo);

              // Display The CPHS Info along with the Service Groups
              case AEESUPPSVC_CHECK_STATUS_INFO:
                pMe->m_bWaitForStatusResult = TRUE;
                MEMCPY(&pMe->m_ActiveData, &EventInfo->evt_data.ss_complete.complete.check_status,sizeof(AEESuppSvcActiveService));
                CDialerApp_CreateDlg(pMe, IDD_SERVICE_GROUP, NULL, 0); 
                return TRUE;

              case AEESUPPSVC_CCBS_INFO:
#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM)
                return CDialerApp_DlgManager_DisplaySSCCBSInfo(pMe, EventInfo);
#else
                return TRUE;
#endif

              default:
                if(EventInfo->evt_data.ss_complete.ss_success == TRUE)
                {
                  DIALER_MSG("SS SUCCESSFUL", 0, 0, 0);
                  CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_SS_CONFIRMED, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
                }
                else
                {
                  DIALER_MSG("SS_FAILED", 0, 0, 0);
                  CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_SS_FAILED, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY); 
                }
                return TRUE;
             }
          }
        }
        return FALSE;

      default:
        DIALER_MSG("ISUPPSVC Event %d Not Handled", EventInfo->evt, 0, 0);
        return FALSE;
      }
    }
#endif /* FEATURE_WCDMA || F_GSM */  
    DIALER_MSG("EVT_NOTIFY Not Handled: %d", wParam, 0, 0);
    return FALSE;

  default:
    DIALER_MSG("Event Not Handled: %d", eCode, 0, 0);
    return FALSE;
  } 
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_PlaySignal

DESCRIPTION:  Plays the signal info from network using IALERT

PARAMETERS:
  *pMe: CDialerApp object pointer
  bySignalType: Signal type from network
  bySignal: Signal from network
  byAlertPitch: Alert pitch from network

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_PlaySignal(CDialerApp *pMe, AECHAR* pszPhoneNumber,
                                                AEECMSignal *pSignal)
{
  AECHAR* pszPhNumber = NULL;
  AEEALERTType AlertType;

  PRINT_FUNCTION_NAME();

  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  if(WSTRLEN(pszPhoneNumber) > 0)
  {
    // give IALERT null if there is no phone number
    pszPhNumber = pszPhoneNumber;
  }

  switch(pSignal->signal_type)
  { 
  case AEECM_SIGNAL_CDMA_TONE:
    switch(pSignal->signal.cdma_tone)
    {
    case AEECM_CDMA_TONE_DIAL:
      AlertType = AEEALERT_ALERT_DIALTONE;
      break;

    case AEECM_CDMA_TONE_RING:
      AlertType = AEEALERT_ALERT_RINGBACK;
      break;

    case AEECM_CDMA_TONE_BUSY:
      AlertType = AEEALERT_ALERT_BUSY;
      break;

    case AEECM_CDMA_TONE_ANSWER:
      AlertType = AEEALERT_ALERT_ANSWER;
      break;

    case AEECM_CDMA_TONE_INTERCEPT:
      AlertType = AEEALERT_ALERT_INTERCEPT;
      break;

    case AEECM_CDMA_TONE_ABBR_INTERCEPT:
      AlertType = AEEALERT_ALERT_ABBR_INTERCEPT;
      break;

    case AEECM_CDMA_TONE_REORDER:
      AlertType = AEEALERT_ALERT_REORDER;
      break;

    case AEECM_CDMA_TONE_ABBR_REORDER:
      AlertType = AEEALERT_ALERT_ABBR_REORDER;
      break;

    case AEECM_CDMA_TONE_CONFIRM:
      AlertType = AEEALERT_ALERT_CONFIRM;
      break;

    case AEECM_CDMA_TONE_CALLWAIT:
      AlertType = AEEALERT_ALERT_CALLWAIT_SIGNAL;
      break;

    case AEECM_CDMA_TONE_PIP:
      AlertType = AEEALERT_ALERT_PIP;
      break;

    case AEECM_CDMA_TONE_OFF:
    default:
      (void) IALERT_StopAlerting(pMe->m_pIALERT);
      return TRUE;
    }
    break;

  case AEECM_SIGNAL_CDMA_ISDN:
    switch(pSignal->signal.cdma_isdn)
    {
    case AEECM_CDMA_ISDN_INTERGROUP:
      AlertType = AEEALERT_ALERT_ISDN_INTERGROUP;
      break;

    case AEECM_CDMA_ISDN_SPECIAL:
      AlertType = AEEALERT_ALERT_ISDN_SPECIAL;
      break;

    case AEECM_CDMA_ISDN_PING:
      AlertType = AEEALERT_ALERT_ISDN_PING;
      break;

    case AEECM_CDMA_ISDN_OFF:
      (void) IALERT_StopAlerting(pMe->m_pIALERT);
      return TRUE;

    case AEECM_CDMA_ISDN_NORMAL:
    default:
      // play the normal ringer since this is default
      AlertType = AEEALERT_ALERT_NORMAL;
      //AlertType = AEEALERT_ALERT_ISDN_NORMAL;
      break;
    }
    break;

  case AEECM_SIGNAL_CDMA_IS54B:
    {
      switch(pSignal->signal.cdma_is54b)
      {
      case AEECM_CDMA_IS54B_L:
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          // play the normal ringer since this is default
          AlertType = AEEALERT_ALERT_NORMAL;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS54B_LONG_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS54B_LONG_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;

      case AEECM_CDMA_IS54B_SS:
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          AlertType = AEEALERT_ALERT_IS54B_SS_M;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS54B_SS_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS54B_SS_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;      

      case AEECM_CDMA_IS54B_SSL: 
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          AlertType = AEEALERT_ALERT_IS54B_SSL_M;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS54B_SSL_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS54B_SSL_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;     

      case AEECM_CDMA_IS54B_SS2: 
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          AlertType = AEEALERT_ALERT_IS54B_SS2_M;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS54B_SS2_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS54B_SS2_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;    

      case AEECM_CDMA_IS54B_SLS: 
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          AlertType = AEEALERT_ALERT_IS54B_SLS_M;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS54B_SLS_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS54B_SLS_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;    

      case AEECM_CDMA_IS54B_SSSS: 
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          AlertType = AEEALERT_ALERT_IS54B_SSSS_M;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS54B_SSSS_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS54B_SSSS_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;    

      case AEECM_CDMA_IS54B_PBX_L:   
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          AlertType = AEEALERT_ALERT_IS54B_PBX_LONG_M;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS54B_PBX_LONG_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS54B_PBX_LONG_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;

      case AEECM_CDMA_IS54B_PBX_SS:  
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SS_M;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SS_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SS_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;

      case AEECM_CDMA_IS54B_PBX_SSL: 
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SSL_M;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SSL_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SSL_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;

      case AEECM_CDMA_IS54B_PBX_SLS:  
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SLS_M;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SLS_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SLS_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;

      case AEECM_CDMA_IS54B_PBX_SSSS: 
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SSSS_M;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SSSS_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS54B_PBX_SSSS_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;

      case AEECM_CDMA_IS54B_PPPP:
        switch(pSignal->signal_pitch)
        {
        case AEECM_PITCH_MED:
          AlertType = AEEALERT_ALERT_IS53A_PPPP_M;
          break;

        case AEECM_PITCH_HIGH:
          AlertType = AEEALERT_ALERT_IS53A_PPPP_H;
          break;

        case AEECM_PITCH_LOW:
          AlertType = AEEALERT_ALERT_IS53A_PPPP_L;
          break;

        default:
          DIALER_ERR("Invalid Alert Pitch: %d", pSignal->signal_pitch,0,0);
          return FALSE;
        }
        break;

      case AEECM_CDMA_IS54B_OFF:
        (void) IALERT_StopAlerting(pMe->m_pIALERT);
        return TRUE;

      default: 
        DIALER_ERR("Invalid Signal for IS54b: %d", pSignal->signal.cdma_is54b,0,0);
        return FALSE;
      }
    }
    break;

  case AEECM_SIGNAL_GW_TONE:
    switch(pSignal->signal.gw_tone)
    {
     case AEECM_GW_DIAL_TONE_ON:
      AlertType = AEEALERT_ALERT_DIALTONE;
      break;

    case AEECM_GW_RING_BACK_TONE_ON:
      AlertType = AEEALERT_ALERT_RINGBACK;
      break;

    case AEECM_GW_BUSY_TONE_ON:
      AlertType = AEEALERT_ALERT_BUSY;
      break;

    case AEECM_GW_ANSWER_TONE_ON:
      AlertType = AEEALERT_ALERT_ANSWER;
      break;

    case AEECM_GW_INTERCEPT_TONE_ON:
      AlertType = AEEALERT_ALERT_INTERCEPT;
      break;

    case AEECM_GW_CONFIRM_TONE_ON:
      AlertType = AEEALERT_ALERT_CONFIRM;
      break;

    case AEECM_GW_CALL_WAITING_TONE_ON:
      AlertType = AEEALERT_ALERT_CALLWAITING;
      break;

    case AEECM_GW_TONES_OFF:
    case AEECM_GW_NW_CONGESTION_TONE_ON:
    case AEECM_GW_OFF_HOOK_WARNING_TONE_ON:
    case AEECM_GW_ALERTING_TONE_OFF:
    default:
      (void) IALERT_StopAlerting(pMe->m_pIALERT);
      return TRUE;
    }
    break;

  default:
    DIALER_ERR("Invalid Signal Type: %d", pSignal->signal_type,0,0);
    return FALSE;
  }


  return (CheckAEEReturnStatus(IALERT_StartAlerting(pMe->m_pIALERT, AEEALERT_CALLTYPE_VOICE, pszPhNumber, AlertType)));
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplayCallError

DESCRIPTION:  Displays the call error

PARAMETERS:
  *pMe: CDialerApp object pointer
  dwError: error code

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_DisplayCallError(CDialerApp *pMe, 
                                                         uint32 dwError)
{
  AECHAR *pszError = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR  szFormat[] = {'%','s',':',' ','%','s','\0'};

  PRINT_FUNCTION_NAME();
  pszError = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszMessage == NULL) || (pszError == NULL) || (pszBuf == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszError);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_ERROR_CAPS, pszError, 
    ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)CDialerApp_DlgManager_CallErrorToIDS(dwError), 
    pszMessage, ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

  WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszError, pszMessage);

  CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

  FREE(pszError);
  FREE(pszMessage);
  FREE(pszBuf);

  return TRUE;
}


/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplayCallEndCause

DESCRIPTION:  Displays the end cause of the call if needed

PARAMETERS:
  *pMe: CDialerApp object pointer
  dwError: error code

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_DisplayCallEndCause(CDialerApp *pMe,
                                                         uint32 dwError, 
                                                         uint32 dwOtherError)
{
  AECHAR *pszError = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR  szFormat[] = {'%','s',':',' ','%','s','\0'};
  uint32  dwErrorPrinted = IDS_UNKNOWN;


  PRINT_FUNCTION_NAME();
  switch(dwError)
  {
  case AEECM_CALL_END_NO_SRV:
    // no service currently available
    dwErrorPrinted = IDS_CALL_FAIL_NO_SRV;
    break;

  case AEECM_CALL_END_FADE:
    CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_FADE_TONE);
    // please try again
    dwErrorPrinted = IDS_CALL_FAIL_TEMP;
    break;

  case AEECM_CALL_END_INTERCEPT:
    CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_INTERCEPT);
    // please try again
    dwErrorPrinted = IDS_CALL_FAIL_TEMP;
    break;

  case AEECM_CALL_END_REORDER:
    CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_REORDER);
    // please try again
    dwErrorPrinted = IDS_CALL_FAIL_TEMP;
    break;

  case AEECM_CALL_END_NO_FUNDS:
    // no funds available
    dwErrorPrinted = IDS_CALL_FAIL_NO_FUNDS;
    break;

  case AEECM_CALL_END_CDMA_LOCK: /* phone is CDMA locked until power cycle. CDMA only */
    // maintenance required
    dwErrorPrinted = IDS_CALL_FAIL_CDMA_LOCK;
    break;

  case AEECM_CALL_END_ACC_FAIL:
  case AEECM_CALL_END_ACC_IN_PROG:
  case AEECM_CALL_END_MC_ABORT:
  case AEECM_CALL_END_MAX_ACCESS_PROBE:
    // please try again
    dwErrorPrinted = IDS_CALL_FAIL_TEMP;
    break;

  case AEECM_CALL_END_UIM_NOT_PRESENT:
    CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_FADE_TONE);
    // R-UIM not present
    dwErrorPrinted = IDS_CALL_FAIL_UIM_NOT_PRESENT;
    break;

  case AEECM_CALL_END_RETRY_ORDER:
    CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_FADE_TONE);
    // retry order
    dwErrorPrinted = IDS_CALL_FAIL_RETRY_ORDER;
    break;

  case AEECM_CALL_END_LL_CAUSE:   /* rxd a reason from lower layer,look in cc_cause. WCDMA/GSM only*/ 
  case AEECM_CALL_END_NETWORK_END: /* network ended the call,look in cc_cause. WCDMA/GSM only*/
    switch(dwOtherError)
    {
    case AEECM_GSMEND_FACILITY_REJECTED:
      // TODO: ICM will add more end causes based on this event
      // Rejected by facility
      dwErrorPrinted = IDS_CALL_FAIL_REJECTED;
      break;

    case AEECM_GSMEND_OPERATOR_DETERMINED_BARRING:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_REORDER);
      // call barred
      dwErrorPrinted = IDS_CALL_FAIL_CALL_BARRED;
      break;

    case AEECM_GSMEND_USER_BUSY: 
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // user busy
      dwErrorPrinted = IDS_CALL_FAIL_USER_BUSY;
      break;

    case AEECM_GSMEND_UNASSIGNED_CAUSE:
    case AEECM_GSMEND_CHANNEL_UNACCEPTABLE:
    case AEECM_GSMEND_NON_SELECTED_USER_CLEARING:
    case AEECM_GSMEND_DESTINATION_OUT_OF_ORDER: 
    case AEECM_GSMEND_CALL_REJECTED:
    case AEECM_GSMEND_USER_ALERTING_NO_ANSWER:
    case AEECM_GSMEND_NO_ROUTE_TO_DEST: 
    case AEECM_GSMEND_NO_USER_RESPONDING: 
      CDialerApp_PlayCallFailedSound(pMe,(uint32) AEEALERT_ALERT_REORDER);
      // please try again  
      dwErrorPrinted = IDS_CALL_FAIL_TEMP;
      break;

    case AEECM_GSMEND_NUMBER_CHANGED:
      //number changed
      dwErrorPrinted = IDS_CALL_FAIL_NUMBER_CHANGED;
      break;

    case AEECM_GSMEND_INVALID_NUMBER_FORMAT:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_SPECIAL_INFO);
      // invalid number format
      dwErrorPrinted = IDS_CALL_FAIL_INVALID_NUMBER_FORMAT;
      break;

    case AEECM_GSMEND_REQUESTED_FACILITY_NOT_SUBSCRIBED:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_ABBR_INTERCEPT);
      // service not subscribed
      dwErrorPrinted = IDS_CALL_FAIL_SRV_NOT_SUB;
      break;

    case AEECM_GSMEND_INCOMING_CALL_BARRED_WITHIN_CUG: 
      // incoming calls barred within CUG
      dwErrorPrinted = IDS_CALL_FAIL_INCOMING_CALL_BARRED_WITHIN_CUG;
      break;

    case AEECM_GSMEND_REQUESTED_FACILITY_NOT_IMPLEMENTED:
    case AEECM_GSMEND_SERVICE_OR_OPTION_NOT_IMPLEMENTED:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_ABBR_INTERCEPT);
      // No service currently available
      dwErrorPrinted = IDS_CALL_FAIL_NO_SRV;
      break;

    case AEECM_GSMEND_USER_NOT_MEMBER_OF_CUG: 
      // user not in CUG
      dwErrorPrinted = IDS_CALL_FAIL_USER_NOT_MEMBER_OF_CUG;
      break;

    case AEECM_GSMREJ_IMSI_UNKNOWN_IN_HLR:
    case AEECM_GSMREJ_IMSI_UNKNOWN_IN_VLR:
    case AEECM_GSMREJ_IMEI_NOT_ACCEPTED:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // Unknown IMSI
      dwErrorPrinted = IDS_CALL_FAIL_UNKNOWN_IMSI;
      break;

    case AEECM_GSMREJ_ILLEGAL_MS:
    case AEECM_GSMREJ_ILLEGAL_ME:
      CDialerApp_PlayCallFailedSound(pMe,(uint32) AEEALERT_ALERT_BUSY);
      //Illegal Values
      dwErrorPrinted = IDS_CALL_FAIL_ILLEGAL_VALUES;
      break;

    case AEECM_GSMREJ_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK:
    case AEECM_GSMREJ_IE_NON_EXISTANT:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      //Unknown MS
      dwErrorPrinted = IDS_CALL_FAIL_UNKNOWN_MS;
      break;

    case AEECM_GSMREJ_GPRS_SERVICES_NOT_ALLOWED:
    case AEECM_GSMREJ_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
    case AEECM_GSMREJ_PLMN_NOT_ALLOWED:
    case AEECM_GSMREJ_LA_NOT_ALLOWED:
    case AEECM_GSMREJ_NATIONAL_ROAMING_NOT_ALLOWED:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // service not allowed
      dwErrorPrinted = IDS_CALL_FAIL_SRV_NOT_ALLOWED;
      break;

    case AEECM_GSMREJ_IMPLICITLY_DETACHED:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // Implicity Detached
      dwErrorPrinted = IDS_CALL_FAIL_IMPLICITLY_DETACHED;
      break;

    case AEECM_GSMREJ_NETWORK_FAILURE:
    case AEECM_GSMREJ_CONGESTTION:
    case AEECM_GSMEND_TEMPORARY_FAILURE:
    case AEECM_GSMEND_NETWORK_OUT_OF_ORDER:
    case AEECM_GSMEND_SWITCHING_EQUIPMENT_CONGESTION:
    case AEECM_GSMEND_NO_CIRCUIT_CHANNEL_AVAILABLE:
    case AEECM_GSMEND_ACCESS_INFORMATION_DISCARDED:
    case AEECM_GSMEND_REQUESTED_CIRCUIT_CHANNEL_NOT_AVAILABLE:
    case AEECM_GSMEND_RESOURCES_UNAVAILABLE_UNSPECIFIED:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // Please Try Again
      dwErrorPrinted = IDS_CALL_FAIL_TEMP;
      break;

    case AEECM_GSMREJ_SERVICE_OPTION_NOT_SUPPORTED:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // Service Not Supported
      dwErrorPrinted = IDS_CALL_FAIL_SRV_NOT_SUP;
      break;

    case AEECM_GSMREJ_REQ_SERV_OPT_NOT_SUBSCRIBED:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // Service Not Subscribed
      dwErrorPrinted = IDS_CALL_FAIL_SRV_NOT_SUB;
      break;

    case AEECM_GSMREJ_SERVICE_OPT__OUT_OF_ORDER:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // Service Option Out of Order
      dwErrorPrinted = IDS_CALL_FAIL_SRV_OUT_OF_ORDER;
      break;

    case AEECM_GSMREJ_CALL_CANNOT_BE_IDENTIFIED:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // Call Not Identified
      dwErrorPrinted = IDS_CALL_FAIL_CALL_NOT_IDENTIFIED;
      break;

    case AEECM_GSMREJ_NO_PDP_CONTEXT_ACTIVATED:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // No PDP Context
      dwErrorPrinted = IDS_CALL_FAIL_NO_PDP_CONTEXT;
      break;

    case AEECM_GSMREJ_RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MIN_VALUE:
    case AEECM_GSMREJ_RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MAX_VALUE:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // Rejected by facility
      dwErrorPrinted = IDS_CALL_FAIL_REJECTED;
      break;

    case AEECM_GSMREJ_SEMANTICALLY_INCORRECT_MSG:
    case AEECM_GSMREJ_INVALID_MANDATORY_INFO:
    case AEECM_GSMREJ_MESSAGE_TYPE_NON_EXISTANT:
    case AEECM_GSMREJ_MESSAGE_TYPE_NOT_COMP_PRT_ST:
      CDialerApp_PlayCallFailedSound(pMe, (uint32)AEEALERT_ALERT_BUSY);
      // Invalid Message
      dwErrorPrinted = IDS_CALL_FAIL_INVALID_MESSAGE;
      break;

    default:
      dwErrorPrinted = IDS_UNKNOWN;
      break;
    }    
    break;

  default:
    dwErrorPrinted = IDS_UNKNOWN;
    break;
  }


  if(dwErrorPrinted ==IDS_UNKNOWN)
  {
    // Do not print Unknown Values
    return TRUE;
  }

  pszError = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszMessage == NULL) || (pszError == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszError);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_CALLFAILED, pszError, 
    ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));


  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)dwErrorPrinted, pszMessage, 
    ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

  WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszError, pszMessage);

  CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, 2, DIALERAPP_MAX_DISPLAY_CALL_FAIL); // longer max time

  FREE(pszError);
  FREE(pszMessage);
  FREE(pszBuf);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplaySSError

DESCRIPTION:  Displays the SS error in the notif dialog

PARAMETERS:
  *pMe: CDialerApp object pointer
  dwError: error code

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_DisplaySSError(CDialerApp *pMe, 
                                                    uint32 dwError)
{
  AECHAR *pszError = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR  szFormat[] = {'%','s',':',' ','%','s','\0'};
  uint16 dwCode = IDS_PLEASE_TRY_AGAIN;

  PRINT_FUNCTION_NAME();
  pszError = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszError == NULL)  || (pszMessage == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszError);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }

  dwCode = CDialerApp_DlgManager_SSErrorToIDS(dwError);

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
                              dwCode, pszMessage, ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));


  if(dwCode != IDS_PLEASE_TRY_AGAIN)
  {
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_ERROR, pszError, 
    ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
  }


  WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszError, pszMessage);

  CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

  FREE(pszError);
  FREE(pszMessage);
  FREE(pszBuf);

  return TRUE;
}

#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM)

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplayCCBSSet

DESCRIPTION:  Displays that the CCBS has been set up

PARAMETERS:
  *pMe: CDialerApp object pointer
  dwError: error code

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_DisplayCCBSSet(CDialerApp *pMe, 
                                                AEECMCallID callID)
{
  AECHAR *pszBuf = NULL;
  AECHAR szFormat[] = {'%','s',' ','%','d','\0'};

  PRINT_FUNCTION_NAME();
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if( (pszBuf == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszBuf);
    return FALSE;
  }

  /*Will display - Recall Indication Set for  callID X */
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_RECALL_IND, pszBuf, 
    ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

  WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszBuf, callID);

  CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

  FREE(pszBuf);

  return TRUE;
}


/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplayError

DESCRIPTION: Displays the Error

PARAMETERS:
  *pMe: CDialerApp object pointer
  *EventInfo: AEESUPPSVC_EVENT_DATA pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_DisplayError(CDialerApp *pMe, 
                              AEESUPPSVC_EVENT_DATA *EventInfo )
{
  AECHAR *pszHeading = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR szFormat[] = {'%','s',':',' ','%','s','\0'};
  AECHAR pszSpace[] = { ' ' , '\0' };
  uint32 dwCode;
  
  PRINT_FUNCTION_NAME();
  pszHeading = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszMessage == NULL) || (pszHeading == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszHeading);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }
  
  dwCode = CDialerApp_DlgManager_OperationETypeToIDS(EventInfo->evt);
 
  if(dwCode != IDS_UNKNOWN)
  {
   // for eg. Call Forwarding Actvation failed
    CDialerApp_NotifDlg_DisplayMsg(pMe, (uint16)CDialerApp_DlgManager_SSCodeETypeToIDS((uint32)EventInfo->ss_code),
                                 DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);


    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
         dwCode, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
    (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);

    (void) WSTRLCAT(pszMessage, pszSpace, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);

    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
         IDS_FAILED, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
    (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);


    WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
    CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
  }
  else
  {
    // For Interrogation ,it will display  "Request not Completed"

    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
       IDS_REQUEST_NOT_COMPLETED, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
  
    CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
  }
    

  return  CDialerApp_DlgManager_DisplaySSError(pMe, (uint32)EventInfo->evt_data.ss_complete.complete.ss_error);
  
}
 

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplaySSCallBarringInfo

DESCRIPTION: Displays the call bearing info in notif dialog

PARAMETERS:
  *pMe: CDialerApp object pointer
  *EventInfo: AEESUPPSVC_EVENT_DATA pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_DisplaySSCallBarringInfo(CDialerApp *pMe, 
                              AEESUPPSVC_EVENT_DATA *EventInfo )
{
  AECHAR *pszHeading = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR szFormat[] = {'%','s',':',' ','%','s','\0'};
  uint32 dwCode;
  AEESuppSvcCallBarringInfo *pCallBarringInfo = &(EventInfo->evt_data.ss_complete.complete.call_barring_info);
  int i;

  PRINT_FUNCTION_NAME();
  pszHeading = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszMessage == NULL) || (pszHeading == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszHeading);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }

  //For eg. Call Barring for all voice
  CDialerApp_NotifDlg_DisplayMsg(pMe, (uint16)CDialerApp_DlgManager_SSCodeETypeToIDS((uint32)EventInfo->ss_code),
                                 DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
  dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);

  if(dwCode == IDS_UNKNOWN) // For Interrogation
  {
    for(i=0; i<(int)pCallBarringInfo->length; i++)
    {
      pszBuf[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
      pszMessage[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
      pszHeading[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
    
      if(pCallBarringInfo->call_barring_feature_list[i].ss_status.present == TRUE)
      {
        DIALER_MSG("Status Present %d %d %d ",pCallBarringInfo->call_barring_feature_list[i].ss_status.ss_provisioned,  
                                              pCallBarringInfo->call_barring_feature_list[i].ss_status.ss_registered, 
                                              pCallBarringInfo->call_barring_feature_list[i].ss_status.ss_active);
      
        DIALER_MSG("Status Operative :%d ",pCallBarringInfo->call_barring_feature_list[i].ss_status.ss_operative, 0, 0);
      
        if(pCallBarringInfo->call_barring_feature_list[i].ss_status.ss_operative == FALSE)
        {
          dwCode = IDS_NOT_OPERATIVE;
        }
        else if(pCallBarringInfo->call_barring_feature_list[i].ss_status.ss_provisioned == FALSE)
        {
          dwCode = IDS_NOT_PROVISIONED;
        }

        else if(pCallBarringInfo->call_barring_feature_list[i].ss_status.ss_registered == FALSE)
        {
          dwCode = IDS_NOT_REGISTERED;
        }
        else if(pCallBarringInfo->call_barring_feature_list[i].ss_status.ss_active == TRUE)
        {
          dwCode = IDS_ACTIVATED;
        }
        else 
        {
          dwCode = IDS_DEACTIVATED;
        }

      }
      else // ss_present
      {
        if(EventInfo->evt_data.ss_complete.ss_success)
        {
          dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);
        }
        else
          dwCode = IDS_ERASED;
      }

      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
         dwCode, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
      (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
      WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
      CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
     }
   }
   else
   {
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
         dwCode, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
      (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
      WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
      CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
  }

  FREEIF(pszHeading);
  FREEIF(pszMessage);
  FREEIF(pszBuf);

  return TRUE;
}
/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplaySSForwardingInfo

DESCRIPTION: Displays the forwarding feature info in notif dialog

PARAMETERS:
  *pMe: CDialerApp object pointer
   *EventInfo: AEESUPPSVC_EVENT_DATA pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_DisplaySSForwardingInfo(CDialerApp *pMe, 
                               AEESUPPSVC_EVENT_DATA *EventInfo)
{
  AECHAR *pszHeading = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR  szFormat[] = {'%','s',':',' ','%','s','\0'};
  AECHAR pszSpace[] = { ' ' , '\0' };
  int i;
  uint32 dwCode ;
  AEESuppSvcForwardingInfo *pForwardingInfo = &(EventInfo->evt_data.ss_complete.complete.fwd_info);
  uint32 dwServiceIDS =0;

  PRINT_FUNCTION_NAME();
  pszHeading = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszMessage == NULL) || (pszHeading == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszHeading);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }

  // For eg. Call Forwarding Unconditional Activation 
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)CDialerApp_DlgManager_SSCodeETypeToIDS((uint32)EventInfo->ss_code),
         pszHeading, ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 

  dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);
  if(dwCode != IDS_UNKNOWN)
  {
   (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
    dwCode, pszMessage,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
  }

  WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
  CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

  
  pszHeading[0] = '\0';
  pszBuf[0] = '\0';
  pszMessage[0] = '\0';

  
  for(i=0; i<(int)pForwardingInfo->fwd_feature_list.length; i++)
  {
    pszBuf[0] = '\0';
    pszMessage[0] = '\0';
    pszHeading[0] = '\0';

    if(EventInfo->evt == AEESUPPSVC_EVENT_INTERROGATE_CONF)
    {
      dwCode = IDS_UNKNOWN;
      
      if(pForwardingInfo->fwd_feature_list.fwd_feature[i].ss_status.present)
      {
        DIALER_MSG("Status Present %d %d %d ",pForwardingInfo->fwd_feature_list.fwd_feature[i].ss_status.ss_provisioned,
                                              pForwardingInfo->fwd_feature_list.fwd_feature[i].ss_status.ss_registered,
                                              pForwardingInfo->fwd_feature_list.fwd_feature[i].ss_status.ss_active);

        DIALER_MSG("Status Operative :%d ",pForwardingInfo->fwd_feature_list.fwd_feature[i].ss_status.ss_operative, 0, 0);

        if(pForwardingInfo->fwd_feature_list.fwd_feature[i].ss_status.ss_operative == FALSE)
        {
          dwCode = IDS_NOT_OPERATIVE;
        }
        else if(pForwardingInfo->fwd_feature_list.fwd_feature[i].ss_status.ss_provisioned == FALSE)
        {
          dwCode = IDS_NOT_PROVISIONED;
        }
        else if(pForwardingInfo->fwd_feature_list.fwd_feature[i].ss_status.ss_registered == FALSE)
        {
          dwCode = IDS_NOT_REGISTERED;
        }
        else if(pForwardingInfo->fwd_feature_list.fwd_feature[i].ss_status.ss_active == TRUE)
        {
          dwCode = IDS_ACTIVE;
        }
        else
        {
          dwCode = IDS_DEACTIVATED;
        }
      }
      else // ss_status
      {
        if(EventInfo->evt_data.ss_complete.ss_success)
        {
          dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);
        }
        else
          dwCode = IDS_ERASED;
      }


      if(dwCode != IDS_UNKNOWN)
      {
       (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
         dwCode, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
       (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
      }
    }
   
    // For eg. All Voice, All Fax etc
    dwServiceIDS = CDialerApp_DlgManager_SSBasicServiceETypeToIDS
                              ((uint32)pForwardingInfo->fwd_feature_list.fwd_feature[i].basic_service);

    if(dwServiceIDS != IDS_UNKNOWN)
    {
       (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
          IDS_FOR, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
       (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
       (void) WSTRLCAT(pszMessage, pszSpace, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
  
       (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
          dwServiceIDS, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
       (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
       (void) WSTRLCAT(pszMessage, pszSpace, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
    }
  
    pszBuf[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
    pszMessage[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';

    if(WSTRLEN(pszMessage) != 0)
      CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszMessage, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

    pszBuf[0] = '\0';
    pszMessage[0] = '\0';
    pszHeading[0] = '\0';

    if(pForwardingInfo->fwd_feature_list.fwd_feature[i].fwd_to_number.length != 0)
    {
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_FORWARDEDTO, pszHeading, 
        ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 
      WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pForwardingInfo->fwd_feature_list.fwd_feature[i].fwd_to_number.data);
      CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

    }

    if(pForwardingInfo->fwd_feature_list.fwd_feature[i].long_fwd_to_number.length != 0)
    {
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_FORWARDEDTO, pszHeading, 
        ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 
      WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pForwardingInfo->fwd_feature_list.fwd_feature[i].long_fwd_to_number.data);
      CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

      pszBuf[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
      pszHeading[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
    }
    
    if(pForwardingInfo->fwd_feature_list.fwd_feature[i].fwd_options.fwd_reason != AEESUPPSVC_FWD_REASON_NONE) {
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_REASON, pszHeading, 
        ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
        (uint16)CDialerApp_DlgManager_SSFwdReasonsToIDS((uint32)pForwardingInfo->fwd_feature_list.fwd_feature[i].fwd_options.fwd_reason), 
        pszMessage, ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
      WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
      CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
  
      if(pForwardingInfo->fwd_feature_list.fwd_feature[i].fwd_options.notification_to_cp == TRUE)
      {
        CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_CPNOTIFY, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);   
      }

      if(pForwardingInfo->fwd_feature_list.fwd_feature[i].fwd_options.notification_to_fp == TRUE)
      {
        CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_FPNOTIFY, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
      }
    }

    if(pForwardingInfo->fwd_feature_list.fwd_feature[i].fwd_to_subaddress.length > 0)
    {
      pszBuf[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
      pszHeading[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';

      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_FORWARDEDTOSUBADDRESS, pszHeading, 
        ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 
      WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pForwardingInfo->fwd_feature_list.fwd_feature[i].fwd_to_subaddress.data);
      CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
    }
  }

  FREEIF(pszHeading);
  FREEIF(pszMessage);
  FREEIF(pszBuf);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplaySSForwardingFeatureInfo

DESCRIPTION: Displays the forwarding feature info in notif dialog

PARAMETERS:
  *pMe: CDialerApp object pointer
  *EventInfo: AEESUPPSVC_EVENT_DATA pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_DisplaySSForwardingFeatureInfo(CDialerApp *pMe, 
                               AEESUPPSVC_EVENT_DATA *EventInfo)
{
  AECHAR *pszHeading = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR  szFormat[] = {'%','s',':',' ','%','s','\0'};
  AECHAR pszSpace[] = { ' ' , '\0' };
  int i;
  uint32 dwCode; 
  AEESuppSvcFwdFeatureList *pForwardingInfo = &EventInfo->evt_data.ss_complete.complete.fwd_feature_list;
  uint32 dwServiceIDS = IDS_UNKNOWN;

  PRINT_FUNCTION_NAME();
  pszHeading = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszMessage == NULL) || (pszHeading == NULL)) 
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszHeading);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)CDialerApp_DlgManager_SSCodeETypeToIDS((uint32)EventInfo->ss_code),
         pszHeading, ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 

  dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);
  if(dwCode != IDS_UNKNOWN)
  {
   (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
    dwCode, pszMessage,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
  }

  WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
  CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

  pszHeading[0] = '\0';
  pszBuf[0] = '\0';
  pszMessage[0] = '\0';

  
  for(i=0; i<(int)pForwardingInfo->length; i++)
  {
    pszBuf[0] = '\0';
    pszMessage[0] = '\0';
    pszHeading[0] = '\0';

    if(EventInfo->evt == AEESUPPSVC_EVENT_INTERROGATE_CONF)
    {
      if(pForwardingInfo->fwd_feature[i].ss_status.present)
      {
        DIALER_MSG("Status Present %d %d %d ",pForwardingInfo->fwd_feature[i].ss_status.ss_provisioned,
                                              pForwardingInfo->fwd_feature[i].ss_status.ss_registered,
                                              pForwardingInfo->fwd_feature[i].ss_status.ss_active);

        DIALER_MSG("Status Operative :%d ",pForwardingInfo->fwd_feature[i].ss_status.ss_operative, 0, 0);

        if(pForwardingInfo->fwd_feature[i].ss_status.ss_operative == FALSE)
        {
          dwCode = IDS_NOT_OPERATIVE;
        }
        else if(pForwardingInfo->fwd_feature[i].ss_status.ss_provisioned == FALSE)
        {
          dwCode = IDS_NOT_PROVISIONED;
        }
        else if(pForwardingInfo->fwd_feature[i].ss_status.ss_registered == FALSE)
        {
          dwCode = IDS_NOT_REGISTERED;
        }
        else if(pForwardingInfo->fwd_feature[i].ss_status.ss_active == TRUE)
        {
          dwCode = IDS_ACTIVE;
        }
        else
        {
          dwCode = IDS_DEACTIVATED;
        }
      }
      else
      {
        if(EventInfo->evt_data.ss_complete.ss_success)
        {
          dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);
        }
        else
          dwCode = IDS_ERASED;
      }

      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
         dwCode, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
      (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
      (void) WSTRLCAT(pszMessage, pszSpace, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
    }
   
    dwServiceIDS = CDialerApp_DlgManager_SSBasicServiceETypeToIDS
                                   ((uint32)pForwardingInfo->fwd_feature[i].basic_service);

    if(dwServiceIDS != IDS_UNKNOWN)  
    {
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
        IDS_FOR, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
       (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
       (void) WSTRLCAT(pszMessage, pszSpace, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
  
       (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
        dwServiceIDS, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
       (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
       (void) WSTRLCAT(pszMessage, pszSpace, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
    }
  
    pszBuf[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
    pszMessage[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
    pszHeading[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';

    WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
    CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

    pszBuf[0] = '\0';
    pszMessage[0] = '\0';
    pszHeading[0] = '\0';

    if(pForwardingInfo->fwd_feature[i].fwd_to_number.length != 0)
    {
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_FORWARDEDTO, pszHeading, 
        ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 
      WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pForwardingInfo->fwd_feature[i].fwd_to_number.data);
      CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

      pszBuf[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
      pszHeading[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
    }

    if(pForwardingInfo->fwd_feature[i].long_fwd_to_number.length != 0)
    {
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_FORWARDEDTO, pszHeading, 
        ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 
      WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pForwardingInfo->fwd_feature[i].long_fwd_to_number.data);
      CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

      pszBuf[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
      pszHeading[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
    }
    
    if(pForwardingInfo->fwd_feature[i].fwd_options.fwd_reason != AEESUPPSVC_FWD_REASON_NONE) {
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_REASON, pszHeading, 
        ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
        (uint16)CDialerApp_DlgManager_SSFwdReasonsToIDS((uint32)pForwardingInfo->fwd_feature[i].fwd_options.fwd_reason), 
        pszMessage, ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
      WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
      CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
  
      if(pForwardingInfo->fwd_feature[i].fwd_options.notification_to_cp == TRUE)
      {
        CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_CPNOTIFY, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);   
      }

      if(pForwardingInfo->fwd_feature[i].fwd_options.notification_to_fp == TRUE)
      {
        CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_FPNOTIFY, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
      }
    }

    if(pForwardingInfo->fwd_feature[i].fwd_to_subaddress.length > 0)
    {
      pszBuf[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
      pszHeading[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';

      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_FORWARDEDTOSUBADDRESS, pszHeading, 
        ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 
      WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pForwardingInfo->fwd_feature[i].fwd_to_subaddress.data);
      CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
    }
  }

  FREEIF(pszHeading);
  FREEIF(pszMessage);
  FREEIF(pszBuf);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplaySSCLIRestriction

DESCRIPTION: Displays the CLI Restriction Info in notif dialog

PARAMETERS:
  *pMe: CDialerApp object pointer
  *EventInfo: AEESUPPSVC_EVENT_DATA pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_DisplaySSCLIRestriction(CDialerApp *pMe, 
                                   AEESUPPSVC_EVENT_DATA *EventInfo)
{
  AECHAR *pszHeading = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR  szFormat[] = {'%','s',':',' ','%','s','\0'};
  AEESuppSvcCliRestrictInfo *pCLIInfo = &(EventInfo->evt_data.ss_complete.complete.cli_restriction);
  uint32 dwCode;

  PRINT_FUNCTION_NAME();
  pszHeading = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszMessage == NULL) || (pszHeading == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszHeading);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_CLIRESTRICTION, pszHeading, 
    ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

  dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);

  if(dwCode == IDS_UNKNOWN && pCLIInfo->ss_status.present)
  {
    DIALER_MSG("Status Present %d %d %d ",pCLIInfo->ss_status.ss_provisioned,
                                          pCLIInfo->ss_status.ss_registered,
                                          pCLIInfo->ss_status.ss_active);
  
    DIALER_MSG("Status Operative :%d ",pCLIInfo->ss_status.ss_operative, 0, 0);

    if(pCLIInfo->ss_status.ss_operative == FALSE)
    {
      dwCode = IDS_NOT_OPERATIVE;
    }
    else if(pCLIInfo->ss_status.ss_provisioned == FALSE)
    {
      dwCode = IDS_NOT_PROVISIONED;
    }
    else if(pCLIInfo->ss_status.ss_active == TRUE)
    {
      if(EventInfo->ss_code == AEESUPPSVC_CLIR)
      {
        dwCode = IDS_MY_CALLER_ID_SHOW_CALLER_ID;
      }
      else
	    {
	      dwCode = IDS_ACTIVATED;
      }
    }
    else 
    {
      dwCode = IDS_DEACTIVATED; 
    }

  }
  else
  {
    if(EventInfo->evt_data.ss_complete.ss_success)
    {
      dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);
    }
    else
      dwCode = IDS_ERASED;
  }

 (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
  dwCode, pszMessage,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

  WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);

  CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

  CDialerApp_NotifDlg_DisplayMsg(pMe, (uint16)CDialerApp_DlgManager_SSCLIRstrctOptionETypeToIDS((uint32)pCLIInfo->cli_restriction_option), DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

  FREE(pszHeading);
  FREE(pszMessage);
  FREE(pszBuf);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplayCUG

DESCRIPTION: Displays the CUG Info in notif dialog

PARAMETERS:
  *pMe: CDialerApp object pointer
  *pCUGInfo: AEESuppSvcCUGInfo pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean  CDialerApp_DlgManager_DisplayCUG(CDialerApp *pMe, AEESuppSvcCUGInfo *pCUGInfo)
{
  AECHAR *pszHeading = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR  szFormat[] = {'%','s',':',' ','%','s','\0'};
  AECHAR szComma[] = { ',' ,'\0'};
  int i;

  PRINT_FUNCTION_NAME();
  pszHeading = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszHeading == NULL) || (pszMessage == NULL)) 
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszHeading);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_CUG, pszHeading, 
    ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
  
  pszMessage[0] = '\0';

  
  for(i=0; i<(int)pCUGInfo->cug_list_length; i++)
  {
    (void) WSTRLCAT(pszMessage, pCUGInfo->cug_index[i], DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
    if(i != (int)(pCUGInfo->cug_list_length - 1))
    {
      (void) WSTRLCAT(pszMessage, szComma, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
    }
  }       
  if(pCUGInfo->cug_list_length > 0)
  {
    WSPRINTF(pszBuf, (DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR), szFormat, pszHeading, pszMessage);
  }
  else
  {
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_CUGNONE, pszBuf, 
      ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
  }
  CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

  FREEIF(pszHeading);
  FREEIF(pszMessage);
  FREEIF(pszBuf);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplayBSGList

DESCRIPTION: Displays the Basic Service Group List Info in notif dialog

PARAMETERS:
  *pMe: CDialerApp object pointer
  *EventInfo: AEESUPPSVC_EVENT_DATA pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean  CDialerApp_DlgManager_DisplayBSGList(CDialerApp *pMe, AEESUPPSVC_EVENT_DATA *EventInfo)
{
  AECHAR *pszHeading = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR  szFormat[] = {'%','s',':',' ','%','s','\0'};
  AECHAR pszSpace[] = { ' ' , '\0' };
  AECHAR pszSeparator[] = { ',' , '\0' };
  int i;
  AEESuppSvcBasicServiceGroupListInfo *pBSGListInfo = &(EventInfo->evt_data.ss_complete.complete.bsg_list_info);
  uint32 dwCode;

  PRINT_FUNCTION_NAME();
  pszHeading = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszMessage == NULL) || (pszHeading == NULL)) 
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszHeading);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }

 (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)CDialerApp_DlgManager_SSCodeETypeToIDS((uint32)EventInfo->ss_code),
         pszHeading, ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 

 dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);

 if(dwCode == IDS_UNKNOWN) // interrogation
 {
   dwCode = IDS_ACTIVATED;
 }
 (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
    dwCode, pszMessage,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
 
  WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
  CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

  pszHeading[0] = '\0';
  pszBuf[0] = '\0';
  pszMessage[0] = '\0';

  for(i = 0; i < pBSGListInfo->length; i++)
  {
    uint32 dwIDS = CDialerApp_DlgManager_SSBasicServiceETypeToIDS((uint32)pBSGListInfo->bsg_list[i]);
	  uint32 j=0;

    if(dwIDS == IDS_UNKNOWN)
		  continue;

    if( j== 0)
    {
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)IDS_FOR, pszBuf, 
      ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
  
      (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
	    (void) WSTRLCAT(pszMessage, pszSpace, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
    }
    else
	  {
	    (void) WSTRLCAT(pszMessage, pszSeparator, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
	  }

    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)dwIDS, pszBuf, 
      ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
  
    (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
    j++;
  }
  
  if(WSTRLEN(pszMessage) >0)
    CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszMessage, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);  

  FREEIF(pszHeading);
  FREEIF(pszMessage);
  FREEIF(pszBuf);

  return TRUE;
}
/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplaySSStatus

DESCRIPTION:  Displays the SS status on 

PARAMETERS:
  *pMe: CDialerApp object pointer
  *EventInfo: AEESUPPSVC_EVENT_DATA pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_DisplaySSStatus(CDialerApp *pMe, 
                                                      AEESUPPSVC_EVENT_DATA *EventInfo)
{
  AECHAR *pszHeading = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR szFormat[] = {'%','s',':',' ','%','s','\0'};
  uint32 dwCode = IDS_UNKNOWN;
  AEESuppSvcSSStatus *pStatus = &(EventInfo->evt_data.ss_complete.complete.ss_status);

  PRINT_FUNCTION_NAME();
  pszHeading = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszMessage == NULL) || (pszHeading == NULL)) 
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszHeading);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
                             (uint16)CDialerApp_DlgManager_SSCodeETypeToIDS((uint32)EventInfo->ss_code), pszHeading, 
    ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

  if(pStatus->present)
  {
    DIALER_MSG("Status Present %d %d %d ",pStatus->ss_provisioned,  pStatus->ss_registered, pStatus->ss_active);
  
    if(EventInfo->evt == AEESUPPSVC_EVENT_REGISTER_CONF ||
       EventInfo->evt == AEESUPPSVC_EVENT_ERASE_CONF)
    {
      if(pStatus->ss_operative == FALSE)
      {
        dwCode = IDS_NOT_OPERATIVE;
      }
      if(pStatus->ss_provisioned == FALSE)
      {
        dwCode = IDS_NOT_PROVISIONED;
      }
      else if(pStatus->ss_registered == FALSE)
      {
       dwCode = IDS_NOT_REGISTERED;
      }
    }
    else if(pStatus->ss_active == TRUE)
    {
      dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);
      if(dwCode == IDS_UNKNOWN) // For Interrogate
        dwCode =  IDS_ACTIVATED;
    }
    else
    {
      dwCode = IDS_DEACTIVATED;
  
      if(pStatus->ss_provisioned == FALSE)
        dwCode = IDS_NOT_PROVISIONED;
    }
  }
  else
  {
    dwCode = IDS_ERASED;
  }

  if(dwCode != IDS_UNKNOWN)
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)dwCode, pszMessage, 
       ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

  WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
  CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

  FREE(pszHeading);
  FREE(pszMessage);
  FREE(pszBuf);

  return TRUE;
}



/*=============================================================================
FUNCTION: CDialerApp_DlgManager_DisplaySSData

DESCRIPTION:  Displays the SS Data on 

PARAMETERS:
  *pMe: CDialerApp object pointer
  *EventInfo: AEESUPPSVC_EVENT_DATA pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DlgManager_DisplaySSData(CDialerApp *pMe, 
                                                   AEESUPPSVC_EVENT_DATA *EventInfo)
{
  AECHAR *pszHeading = NULL;
  AECHAR *pszMessage = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR szFormat[] = {'%','s',':',' ','%','s','\0'};
  AECHAR pszSpace[] = { ' ' , '\0' };
  AECHAR pszSeparator[] = { ',' , '\0' };
  AEESuppSvcSSDataInfo *pStatusData = &(EventInfo->evt_data.ss_complete.complete.ss_data);
  uint32 dwCode = IDS_UNKNOWN;
  uint8 i=0;

  PRINT_FUNCTION_NAME();
  pszHeading = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszMessage == NULL) || (pszHeading == NULL)) 
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszHeading);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
    return FALSE;
  }

  
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
                             (uint16)CDialerApp_DlgManager_SSCodeETypeToIDS((uint32)EventInfo->ss_code), pszHeading, 
                             ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

  if(pStatusData->ss_status.present)
  {
     DIALER_MSG("Status Present %d %d %d ",pStatusData->ss_status.ss_provisioned,  
                                           pStatusData->ss_status.ss_registered, 
                                           pStatusData->ss_status.ss_active);
     DIALER_MSG("Status Operative :%d ",pStatusData->ss_status.ss_operative, 0, 0);

    if(EventInfo->evt == AEESUPPSVC_EVENT_REGISTER_CONF ||
       EventInfo->evt == AEESUPPSVC_EVENT_ERASE_CONF)
    {
      if(pStatusData->ss_status.ss_operative == FALSE)
      {
        dwCode = IDS_NOT_OPERATIVE;
      }
      else if(pStatusData->ss_status.ss_provisioned == FALSE)
      {
        dwCode = IDS_NOT_PROVISIONED;
      }
      else if(pStatusData->ss_status.ss_registered == FALSE)
      {
       dwCode = IDS_NOT_REGISTERED;
      }
    }
    else if(pStatusData->ss_status.ss_active == TRUE)
    {
      dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);
    }
    else
    {
      dwCode = IDS_DEACTIVATED;
    }
  }
  else
  {
    if(EventInfo->evt_data.ss_complete.ss_success)
    {
      dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);
    }
    else
      dwCode = IDS_ERASED;
  }


  if(dwCode != IDS_UNKNOWN)
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)dwCode, pszMessage, 
        ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

  WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
  CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

  pszMessage[0] = '\0';
  pszBuf[0] = '\0';
  pszHeading[0] = '\0';

  for(i = 0; i < pStatusData->basic_service_group_list.length; i++)
  {
    int j=0;
    uint32 dwIDS = CDialerApp_DlgManager_SSBasicServiceETypeToIDS((uint32)pStatusData->basic_service_group_list.bsg_list[i]);

    if(dwIDS == IDS_UNKNOWN)
		continue;

	  if( j== 0)
    {
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)IDS_FOR, pszBuf, 
        ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
  
      (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
      (void) WSTRLCAT(pszMessage, pszSpace, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
    }
    else
  	{
	    (void) WSTRLCAT(pszMessage, pszSeparator, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
    }

    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)dwIDS, pszBuf, 
       ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

    (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
	  j++;
    
  }

  if(WSTRLEN(pszMessage) > 0)
  {
    CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszMessage, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
  }

  FREE(pszHeading);
  FREE(pszMessage);
  FREE(pszBuf);

  return TRUE;
}


  /*=============================================================================
  FUNCTION: CDialerApp_DlgManager_DisplaySSCCBSInfo
  
  DESCRIPTION: Displays the CCBS info in notif dialog
  
  PARAMETERS:
    *pMe: CDialerApp object pointer
     *EventInfo: AEESUPPSVC_EVENT_DATA pointer
  
  RETURN VALUE:
    boolean: returns TRUE if successful
  
  COMMENTS:
  
  SIDE EFFECTS:
  
  SEE ALSO:
  
  =============================================================================*/
  static boolean CDialerApp_DlgManager_DisplaySSCCBSInfo(CDialerApp *pMe, 
                                 AEESUPPSVC_EVENT_DATA *EventInfo)
  {
    AECHAR *pszHeading = NULL;
    AECHAR *pszMessage = NULL;
    AECHAR *pszBuf = NULL;
    AECHAR  szFormat[] = {'%','s',':',' ','%','s','\0'};
    AECHAR  szFormatInt[] = {'%','s',':',' ','%','d','\0'};
    AECHAR pszSpace[] = { ' ' , '\0' };
    int i;
    uint32 dwCode ;
    AEESuppSvcCCBSInfo *pCCBSInfo = &(EventInfo->evt_data.ss_complete.complete.ccbs_info);
    uint32 dwServiceIDS =0;
  
    PRINT_FUNCTION_NAME();
    pszHeading = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
    pszMessage = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
    pszBuf = MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
    if((pszBuf == NULL) || (pszMessage == NULL) || (pszHeading == NULL))
    {
      DIALER_ERR("No memory", 0,0,0);
      FREEIF(pszHeading);
      FREEIF(pszMessage);
      FREEIF(pszBuf);
      return FALSE;
    }
  
    // For eg. Call Forwarding Unconditional Activation 
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, (uint16)CDialerApp_DlgManager_SSCodeETypeToIDS((uint32)EventInfo->ss_code),
           pszHeading, ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 
  
    WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pszMessage);
    CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
  
    
    pszHeading[0] = '\0';
    pszBuf[0] = '\0';
    pszMessage[0] = '\0';
  
    dwCode = IDS_UNKNOWN;
        
    if(pCCBSInfo->ss_status.present)
    {
      DIALER_MSG("Status Present %d %d %d ",pCCBSInfo->ss_status.ss_provisioned,
                                           pCCBSInfo->ss_status.ss_registered,
                                           pCCBSInfo->ss_status.ss_active);
  
      DIALER_MSG("Status Operative :%d ",pCCBSInfo->ss_status.ss_operative, 0, 0);
  
      if(pCCBSInfo->ss_status.ss_operative == FALSE)
      {
         dwCode = IDS_NOT_OPERATIVE;
      }
      else if(pCCBSInfo->ss_status.ss_provisioned == FALSE)
      {
        dwCode = IDS_NOT_PROVISIONED;
      }
      else if(pCCBSInfo->ss_status.ss_registered == FALSE)
      {
        dwCode = IDS_NOT_REGISTERED;
      }
      else if(pCCBSInfo->ss_status.ss_active == TRUE)
      {
        dwCode = IDS_ACTIVE;
      }
      else
      {
        dwCode = IDS_DEACTIVATED;
      }
    }
    else // ss_status
    {
      if(EventInfo->evt_data.ss_complete.ss_success)
      {
        dwCode = CDialerApp_DlgManager_SSSuccessOperationETypeToIDS((uint32)EventInfo->evt);
      }
      else
        dwCode = IDS_ERASED;
    }
  
  
    if(dwCode != IDS_UNKNOWN)
    {
     (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
        dwCode, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
     (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
    }
    
    for(i=0; i<(int)pCCBSInfo->ccbs_feature_list_length; i++)
    {
      pszBuf[0] = '\0';
      pszMessage[0] = '\0';
      pszHeading[0] = '\0';

      if(pCCBSInfo->ccbs_feature_list[i].ccbsIndex != 0)
      {
        (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
                IDS_CCBS_INDEX, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
        pszBuf[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
        (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
        WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormatInt, pszHeading, pCCBSInfo->ccbs_feature_list[i].ccbsIndex);
        pszBuf[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
        (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
        pszMessage[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
        CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszMessage, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

        pszBuf[0] = '\0';
        pszMessage[0] = '\0';
        pszHeading[0] = '\0';        
      }
     

      // For eg. All Voice, All Fax etc
      dwServiceIDS = CDialerApp_DlgManager_SSBasicServiceETypeToIDS
                                ((uint32)pCCBSInfo->ccbs_feature_list[i].basicService);
  
      if(dwServiceIDS != IDS_UNKNOWN)
      {
         (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
            IDS_FOR, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
         (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
         (void) WSTRLCAT(pszMessage, pszSpace, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
    
         (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, 
            dwServiceIDS, pszBuf,((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
         (void) WSTRLCAT(pszMessage, pszBuf, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
         (void) WSTRLCAT(pszMessage, pszSpace, DIALERAPP_MAX_NOTIF_BUFFER_SIZE);
      }
    
      pszBuf[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
      pszMessage[DIALERAPP_MAX_NOTIF_BUFFER_SIZE-1] = '\0';
  
      if(WSTRLEN(pszMessage) != 0)
        CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszMessage, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
  
      pszBuf[0] = '\0';
      pszMessage[0] = '\0';
      pszHeading[0] = '\0';
  
      if(pCCBSInfo->ccbs_feature_list[i].subscriberAddress.length != 0)
      {
        (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_FORWARDEDTO, pszHeading, 
          ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 
        WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pCCBSInfo->ccbs_feature_list[i].subscriberAddress.data);
        CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
  
      }

      if(pCCBSInfo->ccbs_feature_list[i].subscriberNumber.length != 0)
      {
        (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_FORWARDEDTO, pszHeading, 
          ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR))); 
        WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszHeading, pCCBSInfo->ccbs_feature_list[i].subscriberNumber.data);
        CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
  
      }
     
    }
  
    FREEIF(pszHeading);
    FREEIF(pszMessage);
    FREEIF(pszBuf);
  
    return TRUE;
  }

/*=============================================================================

FUNCTION: CDialerApp_DlgManager_SSCodeETypeToIDS

DESCRIPTION: looks up the IDS for given code

PARAMETERS:
  nCode: code to lookup IDS for

RETURN VALUE:
  uint32: IDS for code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CDialerApp_DlgManager_SSCodeETypeToIDS(uint32 dwCode)
{
  PRINT_FUNCTION_NAME();
  switch(dwCode)
  {
  case AEESUPPSVC_CLIP: return(IDS_SUPPSYS_SS_CODE_E_TYPE_CLIP);                      
  case AEESUPPSVC_CLIR: return(IDS_SUPPSYS_SS_CODE_E_TYPE_CLIR);                      
  case AEESUPPSVC_COLP: return(IDS_SUPPSYS_SS_CODE_E_TYPE_COLP);                       
  case AEESUPPSVC_COLR: return(IDS_SUPPSYS_SS_CODE_E_TYPE_COLR);                       
  case AEESUPPSVC_ALL_FORWARDING_SS: return(IDS_SUPPSYS_SS_CODE_E_TYPE_ALL_FORWARDING_SS);         
  case AEESUPPSVC_CFU: return(IDS_SUPPSYS_SS_CODE_E_TYPE_CFU);                       
  case AEESUPPSVC_ALL_CONDFWD_SS: return(IDS_SUPPSYS_SS_CODE_E_TYPE_ALL_CONDFWD_SS);            
  case AEESUPPSVC_CFB: return(IDS_SUPPSYS_SS_CODE_E_TYPE_CFB);                        
  case AEESUPPSVC_CFNRY: return(IDS_SUPPSYS_SS_CODE_E_TYPE_CFNRY);                      
  case AEESUPPSVC_CFNRC: return(IDS_SUPPSYS_SS_CODE_E_TYPE_CFNRC);                      
  case AEESUPPSVC_CW: return(IDS_SUPPSYS_SS_CODE_E_TYPE_CW);                        
  case AEESUPPSVC_ALL_CALL_RSTRCT_SS: return(IDS_SUPPSYS_SS_CODE_E_TYPE_ALL_CALL_RSTRCT_SS);         
  case AEESUPPSVC_BARRING_OUTGOING_CALLS: return(IDS_SUPPSYS_SS_CODE_E_TYPE_BARRING_OUTGOING_CALLS);    
  case AEESUPPSVC_BAOC: return(IDS_SUPPSYS_SS_CODE_E_TYPE_BAOC);                      
  case AEESUPPSVC_BOIC: return(IDS_SUPPSYS_SS_CODE_E_TYPE_BOIC);                       
  case AEESUPPSVC_BOIC_EXHC: return(IDS_SUPPSYS_SS_CODE_E_TYPE_BOIC_EXHC);                 
  case AEESUPPSVC_BARRING_OF_INCOMING_CALLS: return(IDS_SUPPSYS_SS_CODE_E_TYPE_BARRING_OF_INCOMING_CALLS);  
  case AEESUPPSVC_BAIC: return(IDS_SUPPSYS_SS_CODE_E_TYPE_BAIC);                      
  case AEESUPPSVC_BICROAM: return(IDS_SUPPSYS_SS_CODE_E_TYPE_BICROAM);                    
  case AEESUPPSVC_CCBS: return (IDS_SUPPSYS_SS_CODE_E_TYPE_CCBS);
  default: 
    DIALER_ERR("Unknown Code E Type: %d", dwCode, 0, 0);
    return(IDS_UNKNOWN);
  }
}



/*=============================================================================

FUNCTION: CDialerApp_DlgManager_OperationETypeToIDS

DESCRIPTION: looks up the IDS for given code

PARAMETERS:
  nCode: code to lookup IDS for

RETURN VALUE:
  uint32: IDS for code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CDialerApp_DlgManager_OperationETypeToIDS(uint32 dwCode)
{
  PRINT_FUNCTION_NAME();
  switch(dwCode)
  {
    case AEESUPPSVC_EVENT_REGISTER_CONF: return(IDS_REGISTERATION);
    case AEESUPPSVC_EVENT_ERASE_CONF: return (IDS_ERASE);
    case AEESUPPSVC_EVENT_ACTIVATE_CONF:return (IDS_ACTIVATION);
    case AEESUPPSVC_EVENT_DEACTIVATE_CONF:return (IDS_DEACTIVATION);
    case AEESUPPSVC_EVENT_REG_PASSWORD_CONF:return(IDS_PASSWORD_CONFIRMATION);
    default: 
      DIALER_ERR("Unknown Code E Type: %d", dwCode, 0, 0);
      return(IDS_UNKNOWN);
  }
}


/*=============================================================================

FUNCTION: CDialerApp_DlgManager_SSSuccessOperationETypeToIDS

DESCRIPTION: looks up the IDS for given code

PARAMETERS:
  nCode: code to lookup IDS for

RETURN VALUE:
  uint32: IDS for code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CDialerApp_DlgManager_SSSuccessOperationETypeToIDS(uint32 dwCode)
{
  PRINT_FUNCTION_NAME();
  switch(dwCode)
  {
    case AEESUPPSVC_EVENT_REGISTER_CONF: return(IDS_REGISTERED);
    case AEESUPPSVC_EVENT_ERASE_CONF: return (IDS_ERASED);
    case AEESUPPSVC_EVENT_ACTIVATE_CONF:return (IDS_ACTIVATED);
    case AEESUPPSVC_EVENT_DEACTIVATE_CONF:return (IDS_DEACTIVATED);
    case AEESUPPSVC_EVENT_REG_PASSWORD_CONF:return(IDS_PASSWORD_CONFIRMED);
    default: 
      return(IDS_UNKNOWN);
  }
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_SSBasicServiceETypeToIDS

DESCRIPTION: looks up the IDS for given code

PARAMETERS:
  nCode: code to lookup IDS for

RETURN VALUE:
  uint32: IDS for code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CDialerApp_DlgManager_SSBasicServiceETypeToIDS(uint32 dwCode)
{
  PRINT_FUNCTION_NAME();
  switch(dwCode)
  {
    case AEESUPPSVC_ALL_SERVICES: return(IDS_SUPPSYS_BASIC_SERVICE_E_TYPE_ALL_SERVICES);
    case AEESUPPSVC_ALL_BEARER_SERVICES: return(IDS_SUPPSYS_BASIC_SERVICE_E_TYPE_ALL_BEARER_SERVICES);
    case AEESUPPSVC_ALL_ASYNCHRONOUS_SERVICES: return(IDS_SUPPSYS_BASIC_SERVICE_E_TYPE_ALL_ASYNCHRONOUS_SERVICES);
    case AEESUPPSVC_ALL_SYNCHRONOUS_SERVICES: return(IDS_SUPPSYS_BASIC_SERVICE_E_TYPE_ALL_SYNCHRONOUS_SERVICES);
    case AEESUPPSVC_ALL_TELESERVICES: return(IDS_SUPPSYS_BASIC_SERVICE_E_TYPE_ALL_TELESERVICES);
    case AEESUPPSVC_ALL_SPEECH_TRANSMISSION_SERVICES: return(IDS_SUPPSYS_BASIC_SERVICE_E_TYPE_ALL_SPEECH_TRANSMISSION_SERVCES);
    case AEESUPPSVC_ALL_SMS_SERVICES: return(IDS_SUPPSYS_BASIC_SERVICE_E_TYPE_ALL_SMS_SERVICES);
    case AEESUPPSVC_ALL_FAX_TRANSMISSION_SERVICES: return(IDS_SUPPSYS_BASIC_SERVICE_E_TYPE_ALL_FAX_TRANSMISSION_SERVICES);
    case AEESUPPSVC_ALL_DATA_CDA_SERVICES: return(IDS_SUPPSYS_BASIC_SERVICE_E_TYPE_ALL_DATA_CDA_SERVICES);
    case AEESUPPSVC_ALL_DATA_CDS_SERVICES: return (IDS_SUPPSYS_BASIC_SERVICE_E_TYPE_ALL_DATA_CDS_SERVICES);
    default: 
      DIALER_ERR("Unknown Basic Service E Type: %d", dwCode, 0, 0);
      return(IDS_UNKNOWN);
  }
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_SSCLIRstrctOptionETypeToIDS

DESCRIPTION: looks up the IDS for given code

PARAMETERS:
  nCode: code to lookup IDS for

RETURN VALUE:
  uint32: IDS for code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CDialerApp_DlgManager_SSCLIRstrctOptionETypeToIDS(uint32 dwCode)
{
  PRINT_FUNCTION_NAME();
  switch(dwCode)
  {
    case AEESUPPSVC_PERMANENT: return(IDS_SUPPSYS_CLI_RSTRCT_OPTION_E_TYPE_PERMANENT);
    case AEESUPPSVC_TEMPORARY_DEFAULT_RESTRICTED: return(IDS_SUPPSYS_CLI_RSTRCT_OPTION_E_TYPE_TEMPORARY_DEFAULT_RESTRICTED);
    case AEESUPPSVC_TEMPORARY_DEFAULT_ALLOWED: return(IDS_SUPPSYS_CLI_RSTRCT_OPTION_E_TYPE_TEMPORARY_DEFAULT_ALLOWED);
    default:
      DIALER_ERR("Unknown CLI Rstrct Option E Type: %d", dwCode, 0, 0);
      return(IDS_UNKNOWN);
  }
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_SSFwdReasonsToIDS

DESCRIPTION: looks up the IDS for given code

PARAMETERS:
  nCode: code to lookup IDS for

RETURN VALUE:
  uint32: IDS for code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CDialerApp_DlgManager_SSFwdReasonsToIDS(uint32 dwCode)
{
  PRINT_FUNCTION_NAME();
  switch(dwCode)
  {
    case AEESUPPSVC_MS_NOT_REACHABLE: return(IDS_SUPPSYS_FWD_REASONS_MS_NOT_REACHABLE);
    case AEESUPPSVC_MS_BUSY: return(IDS_SUPPSYS_FWD_REASONS_MS_BUSY);
    case AEESUPPSVC_NO_REPLY: return(IDS_SUPPSYS_FWD_REASONS_NO_REPLY);
    case AEESUPPSVC_UNCONDITIONAL: return(IDS_SUPPSYS_FWD_REASONS_UNCONDITIONAL);
    default:
      DIALER_ERR("Unknown CLI Rstrct Option E Type: %d", dwCode, 0, 0);
     return(IDS_UNKNOWN);
  }
}
#endif //defined(FEATURE_WCDMA) || defined (FEATURE_GSM)

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_SSErrorToIDS

DESCRIPTION: looks up the IDS for given error code

PARAMETERS:
  nErr: error code to lookup IDS for

RETURN VALUE:
  uint32: IDS for error code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CDialerApp_DlgManager_SSErrorToIDS(uint32 dwErr)
{
  PRINT_FUNCTION_NAME();
  // error checking
  if (dwErr == 0) 
  {
    DIALER_ERR("Bad Value", 0,0,0);
    return FALSE;
  }

  switch(dwErr)
  {
    case AEESUPPSVC_UNKNOWN_SUBR: return(IDS_ERROR_AEESYS_UNKNOWN_SUBR); 
    case AEESUPPSVC_ILLEGAL_SUBS: return(IDS_ERROR_AEESYS_ILLEGAL_SUBS);
    case AEESUPPSVC_BR_SRV_NOT_PRV: return(IDS_ERROR_AEESYS_BR_SRV_NOT_PRV);
    case AEESUPPSVC_TELE_SRV_NOT_PRV: return(IDS_ERROR_AEESYS_TELE_SRV_NOT_PRV);
    case AEESUPPSVC_ILLEGAL_EQUIP: return(IDS_ERROR_AEESYS_ILLEGAL_EQUIP);
    case AEESUPPSVC_CALL_BARRED: return(IDS_ERROR_AEESYS_CALL_BARRED);
    case AEESUPPSVC_ILLEGAL_SS_OP: return(IDS_ERROR_AEESYS_ILLEGAL_SS_OP);
    case AEESUPPSVC_SS_ERR_STATUS: return(IDS_ERROR_AEESYS_SS_ERR_STATUS);
    case AEESUPPSVC_SS_NOT_AVAIL: return(IDS_ERROR_AEESYS_SS_NOT_AVAIL);
    case AEESUPPSVC_SS_SUBR_VIOLATION: return(IDS_ERROR_AEESYS_SS_SUBR_VIOLATION);
    case AEESUPPSVC_SS_INCOMPATIBLE: return(IDS_ERROR_AEESYS_SS_INCOMPATIBLE);
    case AEESUPPSVC_FACILITY_NOT_SUPP: return(IDS_ERROR_AEESYS_FACILITY_NOT_SUPP);
    case AEESUPPSVC_ABSENT_SUBR: return(IDS_ERROR_AEESYS_FACILITY_NOT_SUPP);
    case AEESUPPSVC_SHORT_TERM_DENIAL: return(IDS_ERROR_AEESYS_SHORT_TERM_DENIAL);
    case AEESUPPSVC_LONG_TERM_DENIAL: return(IDS_ERROR_AEESYS_LONG_TERM_DENIAL);
    case AEESUPPSVC_SYSTEM_FAILURE: return(IDS_ERROR_AEESYS_SYSTEM_FAILURE);
    case AEESUPPSVC_DATA_MISSING: return(IDS_ERROR_AEESYS_DATA_MISSING);
    case AEESUPPSVC_UNEXPT_DATA_VALUE: return(IDS_ERROR_AEESYS_UNEXPT_DATA_VALUE);
    case AEESUPPSVC_PWD_REG_FAILURE: return(IDS_ERROR_AEESYS_PWD_REG_FAILURE);
    case AEESUPPSVC_NEGATIVE_PWD_CHECK: return(IDS_ERROR_AEESYS_NEGATIVE_PWD_CHECK);
    case AEESUPPSVC_MAX_PWD_ATTEMPTS: return(IDS_ERROR_AEESYS_MAX_PWD_ATTEMPTS);
    case AEESUPPSVC_POS_METHOD_FAILURE: return(IDS_ERROR_AEESYS_POS_METHOD_FAILURE);
    case AEESUPPSVC_UNKNOWN_ALPHABET: return(IDS_ERROR_AEESYS_UNKNOWN_ALPHABET);
    case AEESUPPSVC_USSD_BUSY: return(IDS_ERROR_AEESYS_USSD_BUSY);
    case AEESUPPSVC_REJ_BY_USER: return(IDS_ERROR_AEESYS_REJ_BY_USER);
    case AEESUPPSVC_REJ_BY_NETWORK: return(IDS_ERROR_AEESYS_REJ_BY_NETWORK);
    case AEESUPPSVC_DEF_TO_SRV_SUBR: return(IDS_ERROR_AEESYS_DEF_TO_SRV_SUBR);
    case AEESUPPSVC_SPECIAL_SRV_CODE: return(IDS_ERROR_AEESYS_SPECIAL_SRV_CODE);
    case AEESUPPSVC_INVALID_DEF_TO_NUM: return(IDS_ERROR_AEESYS_INVALID_DEF_TO_NUM);
    case AEESUPPSVC_MAX_MPTY_EXCEEDED: return(IDS_ERROR_AEESUSP_MAX_MPTY_EXCEEDED);
    case AEESUPPSVC_RES_NOT_AVAILABLE: return(IDS_ERROR_AEESYS_RES_NOT_AVAILABLE);
    case AEESUPPSVC_UNRECOGNISED_COMPONENT: return(IDS_ERROR_AEESYS_UNRECOGNISED_COMPONENT);  
    case AEESUPPSVC_MISTYPED_COMPONENT: return(IDS_ERROR_AEESYS_MISTYPED_COMPONENT);     
    case AEESUPPSVC_BADLY_STRUCTURED_COMPONENT: return(IDS_ERROR_AEESYS_BADLY_STRUCTURED_COMPONENT);
    case AEESUPPSVC_DUPLICATE_INVOKE_ID: return(IDS_ERROR_AEESYS_DUPLICATE_INVOKE_ID);
    case AEESUPPSVC_UNRECOGNISED_OPERATION: return(IDS_ERROR_AEESYS_UNRECOGNISED_OPERATION);
    case AEESUPPSVC_MISTYPED_PARAMETER: return(IDS_ERROR_AEESYS_MISTYPED_PARAMETER);
    case AEESUPPSVC_RESOURCE_LIMITATION: return(IDS_ERROR_AEESYS_RESOURCE_LIMITATION);
    case AEESUPPSVC_INITIATING_RELEASE: return(IDS_ERROR_AEESYS_INITIATING_RELEASE);
    case AEESUPPSVC_UNRECOGNISED_LINKED_ID: return(IDS_ERROR_AEESYS_UNRECOGNISED_LINKED_ID);
    case AEESUPPSVC_LINKED_RESPONSE_UNEXPECTED: return(IDS_ERROR_AEESYS_LINKED_RESPONSE_UNEXPECTED);
    case AEESUPPSVC_UNEXPECTED_LINKED_OPERATION: return(IDS_ERROR_AEESYS_UNEXPECTED_LINKED_OPERATION);
    case AEESUPPSVC_UNRECOGNISED_INVOKE_ID: return(IDS_ERROR_AEESYS_UNRECOGNISED_INVOKE_ID);
    case AEESUPPSVC_RETURN_RESULT_UNEXPECTED: return(IDS_ERROR_AEESYS_RETURN_RESULT_UNEXPECTED);
    case AEESUPPSVC_RETURN_RESULT_MISTYPED_PARAMETER: return(IDS_ERROR_AEESYS_RETURN_RESULT_MISTYPED_PARAMETER);
    case AEESUPPSVC_RETURN_ERROR_UNRECOGNISED_INVOKE_ID: return(IDS_ERROR_AEESYS_RETURN_ERROR_UNRECOGNISED_INVOKE_ID); 
    case AEESUPPSVC_RETURN_ERROR_UNEXPECTED: return(IDS_ERROR_AEESYS_RETURN_ERROR_UNEXPECTED);
    case AEESUPPSVC_UNRECOGNISED_ERROR: return(IDS_ERROR_AEESYS_UNRECOGNISED_ERROR);
    case AEESUPPSVC_UNEXPECTED_ERROR: return(IDS_ERROR_AEESYS_UNEXPECTED_ERROR);
    case AEESUPPSVC_RETURN_ERROR_MISTYPED_PARAMETER: return(IDS_ERROR_AEESYS_RETURN_ERROR_MISTYPED_PARAMETER);
    case AEESUPPSVC_MN_INSUFFICIENT_RESOURCES: return(IDS_ERROR_AEESYS_MN_INSUFFICIENT_RESOURCES);
    case AEESUPPSVC_MN_SPLIT_MPTY_REJ: return(IDS_ERROR_AEESYS_MN_SPLIT_MPTY_REJ);       
    case AEESUPPSVC_MN_CALL_HOLD_REJ: return(IDS_ERROR_AEESYS_MN_CALL_HOLD_REJ);        
    case AEESUPPSVC_MN_CALL_RETRIEVE_REJ: return(IDS_ERROR_AEESYS_MN_CALL_RETRIEVE_REJ);
    case AEESUPPSVC_MN_NETWORK_NOT_RESPONDING: return(IDS_ERROR_AEESYS_MN_NETWORK_NOT_RESPONDING);
    default: 
      DIALER_ERR("Unknown Error Code: %d", dwErr, 0, 0);
     return(IDS_PLEASE_TRY_AGAIN);
  }
}

/*=============================================================================
FUNCTION: CDialerApp_DlgManager_CallErrorToIDS

DESCRIPTION: looks up the IDS for given code

PARAMETERS:
  nCode: code to lookup IDS for

RETURN VALUE:
  uint32: IDS for code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CDialerApp_DlgManager_CallErrorToIDS(uint32 dwCode)
{
  PRINT_FUNCTION_NAME();
  switch(dwCode)
  {
    case AEECM_CALL_ERROR_OFFLINE_S: return(IDS_CALL_ERROR_OFFLINE_S);
    case AEECM_CALL_ERROR_CDMA_LOCK_S: return(IDS_CALL_ERROR_CDMA_LOCK_S);
    case AEECM_CALL_ERROR_FLASH_S: return(IDS_CALL_ERROR_FLASH_S);
    case AEECM_CALL_ERROR_ORIG_RES_S: return(IDS_CALL_ERROR_ORIG_RES_S);
    case AEECM_CALL_ERROR_NO_FUNDS_S: return(IDS_CALL_ERROR_NO_FUNDS_S);
    case AEECM_CALL_ERROR_EMERGENCY_MODE_S: return(IDS_CALL_ERROR_EMERGENCY_MODE_S);
    case AEECM_CALL_ERROR_IN_USE_S: return(IDS_CALL_ERROR_IN_USE_S);
    case AEECM_CALL_ERROR_SRV_TYPE_S: return(IDS_CALL_ERROR_SRV_TYPE_S);
    case AEECM_CALL_ERROR_CALL_TYPE_S: return(IDS_CALL_ERROR_CALL_TYPE_S);
    case AEECM_CALL_ERROR_CALL_STATE_S: return(IDS_CALL_ERROR_CALL_STATE_S);
    case AEECM_CALL_ERROR_SRV_STATE_S: return(IDS_CALL_ERROR_SRV_STATE_S);
    case AEECM_CALL_ERROR_NO_SO_S: return(IDS_CALL_ERROR_NO_SO_S);
    case AEECM_CALL_ERROR_ALERT_ANS_BYPASS_S: return(IDS_CALL_ERROR_ALERT_ANS_BYPASS_S);
    case AEECM_CALL_ERROR_UIM_NOT_PRESENT_S: return(IDS_CALL_ERROR_UIM_NOT_PRESENT_S);
    case AEECM_CALL_ERROR_BAD_PRIVACY_PREF_S: return(IDS_CALL_ERROR_BAD_PRIVACY_PREF_S);
    case AEECM_CALL_ERROR_NO_CALL_ID_S: return(IDS_CALL_ERROR_NO_CALL_ID_S);
    case AEECM_CALL_ERROR_CALL_ORIG_FAIL_S: return(IDS_CALL_ERROR_CALL_ORIG_FAIL_S);
    case AEECM_CALL_ERROR_INVALID_MODE_S: return(IDS_CALL_ERROR_INVALID_MODE_S);
    case AEECM_CALL_ERROR_INVALID_SIM_STATE_S: return (IDS_CALL_ERROR_INVALID_SIM_STATE_S);
    case AEECM_CALL_ERROR_NO_COLLOC_HDR: return(IDS_CALL_ERROR_NO_COLLOC_HDR);
    case AEECM_CALL_ERROR_OTHER: return(IDS_CALL_ERROR_OTHER);
    case AEECM_CALL_ERROR_CALL_CONTROL_REJECTED: return(IDS_CALL_ERROR_CALL_CONTROL_REJECTED);
    case AEECM_CALL_ERROR_LOWER_LAYER: return(IDS_CALL_ERROR_LOWER_LAYER);
    default: 
      DIALER_ERR("Unknown Call Error: %d", dwCode, 0, 0);
      return(IDS_UNKNOWN);
  }
}

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
/*=============================================================================
FUNCTION: CDialerApp_SecStatusCB

DESCRIPTION: Updates the secondary display with call status information.
             Performs necessary cleanup if we have been displaying ended
             call time for long enough that we now need to stop.  Sets a
             timer so that it will be re-invoked periodically.

PARAMETERS:
  void *pUser:  Callback data.  Just a pointer to the CDialerApp structure

RETURN VALUE:
  void

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CDialerApp_SecStatusCB(void *pUser)
{
  CDialerApp    *pMe = (CDialerApp *)pUser;
  AECHAR        *pszDispText;
  AECHAR        *pszFormatStr;
  AECHAR        *pszScratchStr;
  uint16        nStringID;
  int           nStrlen;
  uint32        dwCurTime;
  uint32        dwCallMinutes;           // Minutes portion of call duration
  uint32        dwCallSeconds;           // Seconds portion of call duration

  if (! pMe)
  {
    MSG_ERROR("2ndary display callback called with bad ptr; app ptr is 0x%x",
              pMe, 0, 0);
    return;
  }

  dwCurTime = GETUPTIMEMS();

  /* If we're displaying total time for an ended call session (as indicated
   * by the active calls flag being FALSE), and we've displayed the ended
   * call session time for long enough, reset the timestamps to indicate
   * that there is no more status to display.
   */
  if ((! pMe->m_bIsActiveCalls) &&
      ((dwCurTime - pMe->m_dwEndTS) >= DIALERAPP_MAX_ENDED_DISPLAY_TIME))
  {
    /* Reset our timestamps, regardless of state.  This will tell us that
     * there's no status info to display when the other state finishes.
     */
    pMe->m_dwStartTS = 0;
    pMe->m_dwEndTS = 0;

    /* Only destroy the Static if we are in Status state; otherwise,
     * another state (e.g. DIALER_DISP_STATE_MISSEDCALL) is using it.
     */
    if (DIALER_DISP_STATE_STATUS == pMe->m_DispState)
    {
      pMe->m_DispState = DIALER_DISP_STATE_NONE;
      if (pMe->m_pIDisplay2 && CDialerApp_GetSecText(pMe))
      {
        (void)ISTATIC_Release(pMe->m_pSecText);
        pMe->m_pSecText = NULL;

        // Update the display to remove the static text we just destroyed
        IDISPLAY_Update(pMe->m_pIDisplay2);
      }
      if (CDialerApp_IsAnyUSBCarkitConnected())
      {
        CDialerApp_PrintToCarkit(NULL);
      }
    }
    return;
  }
    
  /* We're not done displaying yet.  If no one else in Dialer is displaying
   * (as indicated by a state of "status"), display either the "in call"
   * time, or the "total" ended call session time.
   */

  // Get the string buffers
  pszDispText = (AECHAR *)MALLOC(DIALERAPP_MAX_SMALL_STR_SIZE *
                                 sizeof(AECHAR));
  pszFormatStr = (AECHAR *)MALLOC(DIALERAPP_MAX_SMALL_STR_SIZE *
                                   sizeof(AECHAR));
  pszScratchStr = (AECHAR *)MALLOC(DIALERAPP_MAX_SMALL_STR_SIZE *
                                   sizeof(AECHAR));

  if ((DIALER_DISP_STATE_STATUS == pMe->m_DispState) &&
      (NULL != pszDispText) &&
      (NULL != pszFormatStr))
  {
    if (pMe->m_bIsActiveCalls)
    {
      nStringID =IDS_TIMEINCALL;
      dwCallMinutes = (((dwCurTime - pMe->m_dwStartTS)/1000)/60);
      dwCallSeconds = (((dwCurTime - pMe->m_dwStartTS)/1000) -
                       (dwCallMinutes * 60));
    }
    else
    {
      nStringID =IDS_CALL_TIME;
      dwCallMinutes = (((pMe->m_dwEndTS - pMe->m_dwStartTS)/1000)/60);
      dwCallSeconds = (((pMe->m_dwEndTS - pMe->m_dwStartTS)/1000) -
                       (dwCallMinutes * 60));
    }

    nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                                   nStringID, pszScratchStr,
                                   (DIALERAPP_MAX_SMALL_STR_SIZE *
                                    sizeof(AECHAR)));
    if (0 != nStrlen)
    {
      (void)STRTOWSTR("%s: %d:%02d", pszFormatStr, DIALERAPP_MAX_SMALL_STR_SIZE);
      WSPRINTF(pszDispText,
               (int)(DIALERAPP_MAX_SMALL_STR_SIZE * sizeof(AECHAR)),
               pszFormatStr, pszScratchStr, dwCallMinutes, dwCallSeconds);
    }

    // Now, display the text
    if (pMe->m_pIDisplay2 && CDialerApp_GetSecText(pMe))
    {
      (void)ISTATIC_SetText(pMe->m_pSecText, NULL, pszDispText, AEE_FONT_LARGE,
                            AEE_FONT_LARGE);
      (void)ISTATIC_Redraw(pMe->m_pSecText);
    }

    //Don't display status info if not in app
    //Also, carkit can't handle data + audio at the same time, so don't do 
    //this in a call.
    if (CDialerApp_IsAnyUSBCarkitConnected() && 
        !pMe->m_bSuspended && 
        !pMe->m_bIsActiveCalls)
    {
      CDialerApp_PrintToCarkit(pszDispText);
    }
  }

  (void)ISHELL_SetTimer(pMe->a.m_pIShell, DIALERAPP_SEC_STATUS_TIMEOUT,
                        CDialerApp_SecStatusCB, (void *)pMe);

  FREEIF(pszDispText);
  FREEIF(pszFormatStr);
  FREEIF(pszScratchStr);
}
#endif  //FEATURE_UI_SECONDARY_DISP_SUPPORT


/*=============================================================================
FUNCTION: CDialerApp_GenericEventHandler

DESCRIPTION: Framework based dialog handler

PARAMETERS:
  *pUser: CDialerApp object pointer
  evt: event code
  w: word event param
  dw: dword event param

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_GenericEventHandler(void *pUser,
                                       AEEEvent evt,
                                       uint16 w,
                                       uint32 dw)
{
  //lint -save -esym(715, dw)
  CDlgHandler* pDlgHandlerEntry = (CDlgHandler*) pUser;
  CDialerApp* pMe;

  PRINT_FUNCTION_NAME();
  // error checking
  if(pDlgHandlerEntry == NULL || (pMe = pDlgHandlerEntry->pDialerApp) == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (evt)
  {
  case EVT_DIALOG_START:
    DIALER_MSG("EVT_DIALOG_START %d", pDlgHandlerEntry->wDlgID, 0, 0);
    if (pDlgHandlerEntry->DlgInitFunc != NULL)
    {
      return pDlgHandlerEntry->DlgInitFunc(pMe);
    }
    break;

  case EVT_KEY_PRESS:
  case EVT_KEY_RELEASE:
  case EVT_KEY:
    if (pDlgHandlerEntry->DlgKeyFunc != NULL)
    {
      DIALER_MSG("Handling key event %d, key %d, dialog %d", evt, w, pDlgHandlerEntry->wDlgID);
      return pDlgHandlerEntry->DlgKeyFunc(pMe, evt, w); //w == keycode
    }
    break;

  case EVT_COMMAND:
    if (pDlgHandlerEntry->DlgCmdFunc != NULL)
    {
      DIALER_MSG("Handling cmd event ctrl_id %d, dialog %d", w, pDlgHandlerEntry->wDlgID, 0);
      if (pDlgHandlerEntry->DlgCmdFunc(pMe, w) == FALSE) //w == control_id
      {
        DIALER_ERR("Command failed %d", w,0,0);
        return FALSE;
      }
      return TRUE;
    }
    break;
    

  case EVT_DIALOG_END:
    DIALER_MSG("EVT_DIALOG_END %d", pDlgHandlerEntry->wDlgID, 0, 0);
    if (pDlgHandlerEntry->DlgEndFunc != NULL)
    {
      return pDlgHandlerEntry->DlgEndFunc(pMe);
    }
    break;
  }
  return FALSE;
}

//lint -restore 
