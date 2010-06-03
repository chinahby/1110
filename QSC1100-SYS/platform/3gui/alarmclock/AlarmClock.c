/*=============================================================================

FILE: AlarmClock.c

SERVICES: AlarmClock App

GENERAL DESCRIPTION:
This application provides Alarm Clock functionality for users to set an alarm
on one time or daily basis. The Application utilizes the the IALARMGR 
interface to notify the app of when apps have occurred.  The IALERT interface
is used to provide the audible alarm for any alarms that have expired.


PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:
None.

(c) COPYRIGHT 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   clm     Initial Revision

===========================================================================*/

/*===========================================================================

                      INCLUDES

===========================================================================*/
#include "BREWVersion.h"
// IAlarmMgr is available only in BREW 3.1.4 and later version
#if MIN_BREW_VERSIONEx(3,1,4)
#include "OEMFeatures.h"
#include "AlarmClock.h"

#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEModTable.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEEText.h"
#include "AppComFunc.h"

#include "err.h"

#include "ALARMMGR.BID"

#include "AlarmClock.h"

#include "alarmclock.brh"
#include "AlarmClock.bid"

#include "AEEDeviceNotifier.h"  // for NMASK_DEVICENOTIFIER_TIMEACQUIRED 


/*===========================================================================

                               STATIC FUNCTIONS

===========================================================================*/
static void    AlarmClock_FreeAppData(IApplet *po);
static boolean AlarmClock_InitAppData(IApplet *po);
static boolean AlarmClock_HandleEvent(AlarmClockApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean AlarmClock_HandleAppStartArgs(AlarmClockApp *pMe, AEEAppStart *pAStart);
static boolean AlarmClock_OnNotify(AlarmClockApp *pMe, AEENotify *pNotify);
static int     AlarmClock_GetAlarmByUserCode(IVector *pAlarmList, uint16 uCode, Alarm **ppAlarm);
static boolean AlarmClock_OnAlarm(AlarmClockApp *pMe, uint16 uCode);
static int     AlarmClock_CreateDefaultAlarms(AlarmClockApp *pMe);
static int     AlarmClock_NewAlarm(Alarm **ppAlarm);
static void    AlarmClock_FreeAlarm(Alarm *pAlarm);
static int     AlarmClock_SetAlarm(AlarmClockApp *pMe, Alarm *pAlarm);
static int     AlarmClock_CancelAlarm(AlarmClockApp *pMe, Alarm *pAlarm);
static void    AlarmClock_SetExpirationTimeToNextRepeat(Alarm *pAlarm);
static int     AlarmClock_SaveAlarms(AlarmClockApp *pMe);
static int     AlarmClock_LoadAlarms(AlarmClockApp *pMe);
static int     AlarmClock_PopulateAlarmStringField(IFile *pFile, AECHAR **ppField);
static int     AlarmClock_SaveAlarmStringField(IFile *pFile, AECHAR *pField);
static boolean AlarmClock_HasActiveAlarm(AlarmClockApp *pMe);
static int     AlarmClock_GetNumDaysToNearestRecurrentDay(Alarm *pAlarm);
static void    AlarmClock_FreeListData(AlarmClockApp *pMe);
static boolean AlarmClock_ListDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
static boolean AlarmClock_EditDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
static boolean AlarmClock_AlertDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
static boolean AlarmClock_OptionsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
static boolean AlarmClock_DescDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
static void    AlarmClock_EndCurrentDlg(AlarmClockApp *pMe);
static void    AlarmClock_PopulateListMenu(AlarmClockApp *pMe, IMenuCtl *ctrl);
static void    AlarmClock_UpdateAnnun(AlarmClockApp *pMe);
static int     AlarmClock_RefreshDialog(AlarmClockApp *pMe);
static void    AlarmClock_AlertTimerCB(void *pUser);
static void    AlarmClock_FormatAlertStr(AlarmClockApp *pMe, Alarm *pAlarm, AECHAR *pBuf, 
                                         uint16 nBufLen, char nSeperator);

#if 0  //Snooze infrastructure is in place, but is not fully implemented
static boolean AlarmClock_CanSnooze(AlarmClockApp *pMe, Alarm *pAlarm);
#endif

/*===========================================================================

                      Constant Definitions

===========================================================================*/
/* Dialog Event Handler function pointer structure */
typedef boolean (PFNDLGHANDLER) (void    *pUser,
                                 AEEEvent evt,
                                 uint16   w,
                                 uint32   dw);

char *gpAlarmClockResourceFile = ALARMCLOCK_RES_FILE;

const char ALARMCLOCK_Alert[] = "ALERT";

#define ALARMCLOCK_FLD_LEN        31

#define ALARMCLOCK_ALERT_FLD_LEN  127

#define ALARMCLOCK_ARG_LEN        31

#define ALARMCLOCK_NUM_OF_ALARMS  8     //Total number of configurable alarms.

#define ALARMCLOCK_ALERT_TIMEOUT  120000 //2 Minute timeout

// Alarm info file name        
#define ALARMCLOCK_ALARMINFO_FILENAME                 "alarminfo.dat"

// Default snooze time duration in seconds
#define ALARMCLOCK_DEFAULT_SNOOZEDURATION             300 // 5 min 

// Invalid expiration time for uninitialized alarm
#define ALARMCLOCK_INVALID_EXPIRATIONTIME             0xffffffff

#define ALARMCLOCK_SECS_PER_DAY 86400

#ifdef MSG_SSID_APPS
  #define ALARM_CLK_MSG(s,p1,p2,p3)        MSG_3(MSG_SSID_APPS,MSG_LEGACY_MED,s,p1,p2,p3)
  #define ALARM_CLK_ERR(s,p1,p2,p3)        MSG_3(MSG_SSID_APPS,MSG_LEGACY_ERROR,s,p1,p2,p3)
#else /* no specific MSG_SSID_APPS ID */
  #define ALARM_CLK_MSG(s,p1,p2,p3)        MSG_MED(s,p1,p2,p3)
  #define ALARM_CLK_ERR(s,p1,p2,p3)        MSG_ERROR(s,p1,p2,p3)
#endif /* MSG_SSID_APPS */

/*===========================================================================

                      STATIC DATA

===========================================================================*/
typedef struct
{
  uint16         wDialogID;  /* Current Dialog ID */
  PFNDLGHANDLER *pDlgHandler; /* Function pointer to Dialog Event Handler */
} DlgEvtHandlerType;

static const DlgEvtHandlerType AlarmClockDlgEventHandlers[] = {
  {IDD_DIALOG_ALARM_LIST,    AlarmClock_ListDlgEventHandler},
  {IDD_DIALOG_ALARM_EDIT,    AlarmClock_EditDlgEventHandler},
  {IDD_DIALOG_ALARM_ALERT,   AlarmClock_AlertDlgEventHandler},
  {IDD_DIALOG_ALARM_OPTIONS, AlarmClock_OptionsDlgEventHandler},
  {IDD_DIALOG_ALARM_DESC,    AlarmClock_DescDlgEventHandler}
}; /* End AlarmClockDlgEventHandlers[] */


#ifdef AEE_STATIC
static AEEAppInfo gaiAlarmClock = {AEECLSID_ALARMCLOCK,
                                   ALARMCLOCK_RES_FILE,
                                   IDS_STRING_ALARMS,
                                   0,0,0,0 , AFLAG_PHONE | AFLAG_STATIC | AFLAG_HIDDEN};
#endif /* AEE_STATIC */

/*=============================================================================

                       DATA STRUCTURE

=============================================================================*/
// Enum for the days in a week 
typedef enum {
   CLOCK_MONDAY    = 0,
   CLOCK_TUESDAY   = 1,
   CLOCK_WEDNESDAY = 2,
   CLOCK_THURSDAY  = 3,
   CLOCK_FRIDAY    = 4,
   CLOCK_SATURDAY  = 5,
   CLOCK_SUNDAY    = 6
} Days;

/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/

#define RELEASEIF(p) do { if (p) { IBASE_Release((IBase*) (void *) (p)); (p) = NULL; } } while (0)

/*=============================================================================
FUNCTION: AlarmClock_IsRepeatingAlarm

DESCRIPTION:  Check whether the alarm is a repeating alarm

PARAMETERS:
  *pAlarm  [in]: Alarm struct

RETURN VALUE:
   boolean:  

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*lint -e{818} suppress could be const */
static __inline boolean AlarmClock_IsRepeatingAlarm(Alarm *pAlarm) {
   return (pAlarm != NULL) ? (pAlarm->recurrence != CLOCK_ONE_TIME) : FALSE;
}

/*===========================================================================
FUNCTION AlarmClock_FreeAppData

DESCRIPTION
  Release the application data

PARAMETERS:
  *po  [in]: IApplet

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static void AlarmClock_FreeAppData(IApplet *po)
{
  AlarmClockApp *pMe = (AlarmClockApp*)(void*)po;

  if (NULL == pMe)
  {
    ALARM_CLK_ERR("pMe NULL", 0, 0, 0);
    return;
  }

  RELEASEIF(pMe->pFileMgr);

  RELEASEIF(pMe->pAlarmMgr);

  RELEASEIF(pMe->pIAnn);

  RELEASEIF(pMe->pIAlert);

  if (NULL != pMe->pAlarmList)
  {
    //Free IVector list data
    AlarmClock_FreeListData(pMe);
  }
  RELEASEIF(pMe->pAlarmList);
} /* End AlarmClock_FreeAppData */

/*===========================================================================
FUNCTION AlarmClock_InitAppData

DESCRIPTION
  Initializing the application data

PARAMETERS:
  *po  [in]: IApplet

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static boolean AlarmClock_InitAppData(IApplet *po)
{
  int            nReturnStatus;
  int            nResult;
  AEEDeviceInfo  di;
  AlarmClockApp *pMe = (AlarmClockApp*)(void*)po;

  if (NULL == pMe)
  {
    ALARM_CLK_ERR("pMe NULL", 0, 0, 0);
    return FALSE;
  }

  pMe->bAppIsRunning = FALSE;
  pMe->wAlarmSelIdx = 0;
  pMe->wAlarmExpiredIdx = 0;

  // Initializing cxScreen
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &di);
  pMe->wCxScreen = di.cxScreen;

  do
  {
    nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                          AEECLSID_ALARMMGR,
                                          (void**) &pMe->pAlarmMgr);
    if (SUCCESS != nReturnStatus)
    {
      break;
    }

    nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                          AEECLSID_FILEMGR,
                                          (void**) &pMe->pFileMgr);
    if (SUCCESS != nReturnStatus)
    {
      break;
    }

    nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                          AEECLSID_VECTOR,
                                          (void**) &pMe->pAlarmList);
    if (SUCCESS != nReturnStatus)
    {
      break;
    }

    nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                          AEECLSID_ANNUNCIATOR,
                                          (void**) &pMe->pIAnn);
    if (SUCCESS != nReturnStatus)
    {
      break;
    }

    nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                          AEECLSID_ALERT,
                                          (void**) &pMe->pIAlert);
    if (SUCCESS != nReturnStatus)
    {
      break;
    }

    // Load the alarms from EFS storage to the alarm list
    nResult = AlarmClock_LoadAlarms(pMe);
    if (nResult != SUCCESS)
    {
      // default settings
      pMe->uSnoozeDuration = ALARMCLOCK_DEFAULT_SNOOZEDURATION;
      pMe->uSysTimeAcquiredTime = GETTIMESECONDS();
      nReturnStatus = AlarmClock_CreateDefaultAlarms(pMe);
    }

    pMe->bTimeValid = (boolean) ISTIMEVALID();
  } while (0);

  if (SUCCESS != nReturnStatus)
  {
    AlarmClock_FreeAppData(po);
    return FALSE;
  }
  //This will increase the font size on VGA displays
  AppComFunc_SelectFonts(pMe->a.m_pIDisplay);

  return TRUE;
} /* End AlarmClock_InitAppData */

/*===========================================================================

FUNCTION AlarmClock_HandleEvent

DESCRIPTION
   This is the EventHandler for this app. All events to this app are handled in this
   function. All APPs must supply an Event Handler.

   AlarmClock creates a menu dialogue in the EVT_APP_START case, and therefore this handler
   is only the active event handler during AlarmClock startup.
   
PROTOTYPE:
   boolean AlarmClock_HandleEvent(IApplet *pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
   pi: Pointer to the AEEApplet structure. This structure contains information specific
   to this applet. It was initialized during the AppCreateInstance() function.

   ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
   none

RETURN VALUE
   TRUE: If the app has processed the event
   FALSE: If the app did not process the event

SIDE EFFECTS
   none

===========================================================================*/
static boolean AlarmClock_HandleEvent(AlarmClockApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{ 
  int nResult;

  if (pMe == NULL)
  {
    ALARM_CLK_ERR("pMe NULL", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if ((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (eCode)
  {
    case EVT_ALARM:
      {
        return(AlarmClock_OnAlarm(pMe, wParam));
      }
    case EVT_APP_START:
      {
        AEEAppStart *pAStart = (AEEAppStart*)dwParam;

        if (pAStart == NULL)
        {
          ALARM_CLK_ERR("pAStart NULL", 0, 0, 0);
          return FALSE;
        }

        if (FALSE == AlarmClock_HandleAppStartArgs(pMe, pAStart))
        {
          //No app start arguments, so just display default alarm list
          nResult = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAlarmClockResourceFile,
                                         IDD_DIALOG_ALARM_LIST, NULL);
          if (nResult != SUCCESS)
          {
            ALARM_CLK_ERR("Unable to create Dialog %d: %d", IDD_DIALOG_ALARM_LIST, nResult, 0);
            return FALSE;
          }
        }

        // Start the AlarmClock Configuration dialog
        ALARM_CLK_MSG("Got EVT_APP_START", 0, 0, 0);

        pMe->bAppIsRunning = TRUE;
        return TRUE;
      }
    case EVT_DIALOG_INIT:
      {
        uint32 i;

        ALARM_CLK_MSG("Got EVT_DIALOG_INIT", 0, 0, 0);
        ALARM_CLK_MSG("Creating Dialog %d", wParam, 0, 0);

        for (i=0; i<ARR_SIZE(AlarmClockDlgEventHandlers); i++)
        {
          if (AlarmClockDlgEventHandlers[i].wDialogID == wParam)
          {
            IDIALOG_SetEventHandler((IDialog *) dwParam, (*AlarmClockDlgEventHandlers[i].pDlgHandler), pMe);
            IDIALOG_SetProperties((IDialog *) dwParam, DLG_HANDLE_ALL_EVENTS);
            return TRUE;
          }
        }
        ERR_FATAL("Dialog Handler not found in AlarmClock_HandleEvent",0,0,0);
        break;
      }
    case EVT_APP_STOP:
      {
        ALARM_CLK_MSG("Got EVT_APP_STOP", 0, 0, 0);
        pMe->bAppIsRunning = FALSE;
        return TRUE;
      }
    case EVT_APP_SUSPEND:
      {
        AEESuspendInfo *pSuspendInfo = (AEESuspendInfo*)dwParam;

        pSuspendInfo->bCloseDialogs = FALSE;

        ALARM_CLK_MSG("Got EVT_APP_SUSPEND", 0, 0, 0);
        return TRUE;
      }
    case EVT_APP_RESUME:
      {
        boolean bRetVal = AlarmClock_HandleAppStartArgs(pMe, (AEEAppStart*)dwParam);
        ALARM_CLK_MSG("Got EVT_APP_RESUME", 0, 0, 0);
        return bRetVal;
      }
    case EVT_COMMAND:
      {
        ALARM_CLK_MSG("Got EVT_COMMAND", 0, 0, 0);
        return TRUE;
      }
    case EVT_NOTIFY:
      { 
        AEENotify *notif = (AEENotify *) dwParam;
        boolean    bRetVal = AlarmClock_OnNotify(pMe, notif);

        ALARM_CLK_MSG("Got EVT_NOTIFY ClsID 0x%x, Mask 0x%x", notif->cls, notif->dwMask, 0);
        return bRetVal;
      }
    case EVT_USER:
      {
        ALARM_CLK_MSG("Got EVT_USER", 0, 0, 0);
        break;
      }
    case EVT_KEY:
      {
        ALARM_CLK_MSG("Got EVT_KEY %d", wParam, 0, 0);
        return TRUE;
      }
    default:
      {
        break;
      }
  }

  return FALSE;

}  /* End AlarmClock_HandleEvent */

/*=============================================================================
FUNCTION: AlarmClock_HandleAppStartArgs

DESCRIPTION: Handle app start/resume arguments
 
PARAMETERS:
   *pMe         [in]: AlarmClockApp object
   *AEEAppStart [in]: AEEAppStart object

RETURN VALUE:
   boolean: TRUE if the event was handled.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*lint -e{818} suppress could be const */
static boolean AlarmClock_HandleAppStartArgs(AlarmClockApp *pMe, AEEAppStart *pAStart)
{
  boolean bRetVal = FALSE;
  int     nResult;

  if (pAStart == NULL)
  {
    ALARM_CLK_ERR("pAStart NULL", 0, 0, 0);
    return bRetVal;
  }

  if ((pAStart->pszArgs != NULL) && 
      STRNCMP(pAStart->pszArgs, 
              ALARMCLOCK_Alert, 
              STRLEN((char*)ALARMCLOCK_Alert)) == 0)
  {
    //ALERT alarm went off, so display alarm dialog
    char *psz = (char*)pAStart->pszArgs + STRLEN((char*)ALARMCLOCK_Alert);

    if (':' == psz[0])
    {
      psz++;
      pMe->wAlarmExpiredIdx = (uint16)ATOI(psz);
    }

    nResult = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAlarmClockResourceFile,
                                   IDD_DIALOG_ALARM_ALERT, NULL);
    if (nResult != SUCCESS)
    {
      ALARM_CLK_ERR("Unable to create Dialog %d: %d", IDD_DIALOG_ALARM_ALERT, nResult, 0);
      return bRetVal;
    }
    bRetVal = TRUE;
  }
  return bRetVal;
}/* End AlarmClock_HandleAppStartArgs */

/*=============================================================================
FUNCTION: AlarmClock_OnNotify

DESCRIPTION: Event handler for EVT_NOTIFY event
 
PARAMETERS:
   *pMe     [in]: AlarmClockApp object
   *pNotify [in]: AEENotify object

RETURN VALUE:
   boolean: TRUE if the event was handled.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean AlarmClock_OnNotify(AlarmClockApp *pMe, AEENotify *pNotify)
{
   boolean  bHandled = FALSE;

   if ((NULL == pMe) || (NULL == pNotify))
   {
     ALARM_CLK_ERR("NULL arguments %d,%d", pMe, pNotify, 0);
     return FALSE;
   }

   if (AEECLSID_SHELL == pNotify->cls &&
      NMASK_SHELL_INIT == pNotify->dwMask) {
      // When the phone powers up, set the time acquired valid system time as 
      // the powerup time if the time is valid.
      if (pMe->bTimeValid) {
         pMe->uSysTimeAcquiredTime = GETTIMESECONDS();
      }
      bHandled = TRUE;
   } 
   else if ((AEECLSID_DEVICENOTIFIER == pNotify->cls) &&
            (NMASK_DEVICENOTIFIER_TIMEACQUIRED == pNotify->dwMask))
   {
      Alarm *pAlarm = NULL;
      uint32 i;

      pMe->bTimeValid = TRUE;
      pMe->uSysTimeAcquiredTime = GETTIMESECONDS();
      bHandled = TRUE;

      for (i = 0; i < IVector_Size(pMe->pAlarmList); i++)
      {
        pAlarm = (Alarm *)IVector_ElementAt(pMe->pAlarmList, i);
        //Check to see if the alarm is still valid
        if (((pAlarm->state == ALARM_STATE_RUNNING) || 
             (pAlarm->state == ALARM_STATE_SNOOZE)) &&
            (!IALARMMGR_IsCodeReserved(pMe->pAlarmMgr, pAlarm->wCode)))
        {
          ALARM_CLK_ERR("AlarmClock_OnNotify Alarm %d is not running", 
                        i, 0, 0);
          pAlarm->state = ALARM_STATE_READY;
          pAlarm->bAlarmActive = FALSE;
          pAlarm->bExpired = TRUE;
        }
      }
   }

   if (bHandled && pMe->bTimeValid)
   {
      //Update annunciator
      AlarmClock_UpdateAnnun(pMe);
     // Save sysTimeAqcuiredTime to file
     (void) AlarmClock_SaveAlarms(pMe);
   }

   return bHandled;
}

/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
  This function is invoked while the app is being loaded. All Modules must provide this
  function. Ensure to retain the same name and parameters for this function.
  In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
  that has been provided in AEEAppGen.c.

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to InitAppData(). This is done as follows:
   InitAppData() is called to initialize AppletData instance. It is app developers
   responsibility to fill-in app data initialization code of InitAppData(). App developer
   is also responsible to release memory allocated for data contained in AppletData -- this can be
   done in FreeAppData().

PROTOTYPE:
  int AEEAppCreateInstance(AEECLSID clsID, IShell *pIShell, IModule *pIModule, IApplet **ppApplet)

PARAMETERS:
  clsID: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell interface.

  pIModule: pin]: Contains pointer to the IModule interface to the current module to which
  this app belongs

  ppApplet: [out]: On return, *ppApplet must point to a valid AEEApplet structure. Allocation
  of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was successful
  EFAILED: If the app does not need to be loaded or if errors occurred in AEEApplet_New().
  If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(AEE_STATIC)
int AlarmClock_CreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMod, void **ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMod, void **ppObj)
#endif
{
  *ppObj = NULL;

  if (AEEApplet_New( sizeof(AlarmClockApp),                  /* Size of our private class */
                     ClsId,                                  /* Our class ID */
                     pIShell,                                /* Shell interface */
                     pMod,                                   /* Module instance */
                     (IApplet**)ppObj,                       /* Return object */
                     (AEEHANDLER)AlarmClock_HandleEvent,     /* Our event handler */
                     (PFNFREEAPPDATA)AlarmClock_FreeAppData))/* Cleanup function */
  {
    if (AlarmClock_InitAppData((IApplet*)*ppObj))
    {
      return(AEE_SUCCESS);
    }
    //Release the applet. This will free the memory allocated for the applet when
    (void)IAPPLET_Release(((IApplet*)*ppObj));
  }
  return(EFAILED);
} /* End AlarmClock_CreateInstance / AEEClsCreateInstance */



#ifdef AEE_STATIC
/*=============================================================================
FUNCTION: AlarmClock_Load

DESCRIPTION:
    This function loads the application.

PROTOTYPE:
static int AlarmClock_Load(IShell *ps, void *pHelpers, IModule **pMod)

PARAMETERS:
--> ps: Pointer to ISHELL interface
--> pHelpers:
--> pMod:

RETURN VALUE:


COMMENTS:


SIDE EFFECTS:
none

SEE ALSO:
none
=============================================================================*/
int AlarmClock_Load(IShell *ps, void *pHelpers, IModule **pMod)
{
  return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,
                          AlarmClock_CreateInstance,NULL));
} /* End AlarmClock_Load */



/*=============================================================================
FUNCTION: AlarmClock_GetModInfo

DESCRIPTION:
    Gets the AlarmClock Info since it is a static application.

PROTOTYPE:
    PFNMODENTRY AlarmClock_GetModInfo(IShell *ps, AEECLSID **ppClasses,
                                      AEEAppInfo **pApps, uint16 *pnApps,
                                      uint16 *pwMinPriv)

PARAMETERS:
--> ps: Pointer to ISHELL interface
--> ppClasses:
--> pApps:
--> pnApps:
--> pwMinPriv:

RETURN VALUE:
 A pointer to the function to load the Application.

COMMENTS:


SIDE EFFECTS:
none

SEE ALSO:
none
=============================================================================*/
/*lint -e{818} -e{715} suppress param could be const, param not referenced */
PFNMODENTRY AlarmClock_GetModInfo(IShell *ps, AEECLSID **ppClasses,
                                  AEEAppInfo **pApps,
                                  uint16 *pnApps,uint16 *pwMinPriv)
{
  *pApps = (AEEAppInfo *)&gaiAlarmClock;
  *pnApps = 1;
  *pwMinPriv = PL_SYSTEM;   /* AlarmClock has system priviliges */
  return((PFNMODENTRY)AlarmClock_Load);
} /* End AlarmClock_GetModInfo */

#endif  /* AEE_STATIC */

/*=============================================================================
FUNCTION: AlarmClock_GetAlarmByUserCode

DESCRIPTION:  Search the alarm list to find the alarm that has the 
              specified user code.

PARAMETERS:
   *pAlarmList [in]: Alarm list
    uCode      [in]: Specified user code
  **ppAlarm   [out]: Pointing to the matched alarm if found.

RETURN VALUE:
   int: the index of the matched alarm in the alarm list; -1 if no matching
        is found or error occurs.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int AlarmClock_GetAlarmByUserCode(IVector *pAlarmList, 
                                         uint16 uCode, 
                                         Alarm **ppAlarm)
{
  uint32 i;
  Alarm *pAlarm = NULL;

  if (NULL == pAlarmList)
  {
    return -1;
  }

  for (i = 0; i < IVector_Size(pAlarmList); i++)
  {
    pAlarm = IVector_ElementAt(pAlarmList, i);
    if ((NULL != pAlarm) && (uCode == pAlarm->wCode))
    {
      if (ppAlarm != NULL)
      {
        *ppAlarm = pAlarm;
      }
      return (int)i;
    }
  }
  return -1; 
}

/*=============================================================================
FUNCTION: AlarmClock_OnAlarm

DESCRIPTION: Event handler for EVT_ALARM event
 
PARAMETERS:
   *pMe     [in]: AlarmClockApp object
   uCode    [in]: User code used to identify BREW alarm

RETURN VALUE:
   boolean: TRUE if the event was handled.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean AlarmClock_OnAlarm(AlarmClockApp *pMe, uint16 uCode)
{
  int    nResult     = SUCCESS;
  Alarm *pAlarm      = NULL;
  int    nAlarmIndex = -1;
  char   szArgsStr[ALARMCLOCK_ARG_LEN+1] = {0};

  do
  {
    // Check if one of our alarms expires
    nAlarmIndex = AlarmClock_GetAlarmByUserCode(pMe->pAlarmList, uCode, &pAlarm);
    if (nAlarmIndex < 0 || !pAlarm->bAlarmActive)
    {
      nResult = EFAILED;
      break;
    }

    ALARM_CLK_MSG("Alarm Clock Alarm %d", nAlarmIndex, 0, 0);

    // Check if the expiration occurs before valid sys time is acquired
    if (pAlarm->uExpirationTime < pMe->uSysTimeAcquiredTime)
    {
      pAlarm->bExpiredBeforeValidTime = TRUE;
    } 
    else
    {
      pAlarm->bExpiredBeforeValidTime = FALSE;
    }

    // Set the lastExpirationTime to the current time if this is 
    // the snooze expiration.
    if (ALARM_STATE_SNOOZE == pAlarm->state)
    {
      pAlarm->uLastExpirationTime = GETTIMESECONDS();
    } 
    else
    {
      pAlarm->uLastExpirationTime = pAlarm->uExpirationTime;
    }
    pAlarm->bAlarmActive = FALSE;
    pAlarm->bExpired = TRUE;
    pAlarm->state = ALARM_STATE_EXPIRED;

    // Reset the alarm if it is repeating
    if (AlarmClock_IsRepeatingAlarm(pAlarm))
    {
      nResult = AlarmClock_SetAlarm(pMe, pAlarm);
      if (SUCCESS == nResult)
      {
        pAlarm->state = ALARM_STATE_RUNNING;
      }
    }

    if (pMe->bAppIsRunning)
    {
      pMe->wAlarmExpiredIdx = (uint16)nAlarmIndex;
      nResult = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAlarmClockResourceFile,
                                    IDD_DIALOG_ALARM_ALERT, NULL);
      if (nResult != SUCCESS)
      {
        ALARM_CLK_ERR("Unable to create Dialog %d: %d", IDD_DIALOG_ALARM_ALERT, nResult, 0);
        break;
      }
    }
    else
    {
      SNPRINTF(szArgsStr, sizeof(szArgsStr), "%s:%u", ALARMCLOCK_Alert, nAlarmIndex);

      nResult = ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_ALARMCLOCK, 
                                       szArgsStr);
      if (nResult != SUCCESS)
      {
        ALARM_CLK_ERR("Unable to start applet: %d", nResult, 0, 0);
        break;
      }
    }

    //Update annunciator
    AlarmClock_UpdateAnnun(pMe);

    // Save alarms in the alarm list to EFS storage
    (void) AlarmClock_SaveAlarms(pMe);

  } while (0);

  return(SUCCESS == nResult);
}

/*=============================================================================
FUNCTION: AlarmClock_CreateDefaultAlarms

DESCRIPTION:  Create a set of new default alarms for the Alarm Clock App.

PARAMETERS:
   *pMe     [in]: AlarmClockApp object

RETURN VALUE:
   int:  BREW result code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int AlarmClock_CreateDefaultAlarms(AlarmClockApp *pMe)
{
  uint32 i;
  Alarm *pAlarm = NULL;
  int    nResult = EBADPARM;

  if (NULL == pMe)
  {
    return nResult;
  }

  if (NULL == pMe->pAlarmList)
  {
    return nResult;
  }

  for (i=0; i < ALARMCLOCK_NUM_OF_ALARMS; i++)
  {
    //Create default alarms
    nResult = AlarmClock_NewAlarm(&pAlarm);
    if (SUCCESS == nResult)
    {
      //Add to IVector list
      nResult = IVector_AddElement(pMe->pAlarmList, pAlarm);
    }

    if (SUCCESS != nResult)
    {
      break;
    }
  }

  if (SUCCESS != nResult)
  {
    AlarmClock_FreeListData(pMe);
    ALARM_CLK_ERR("Cannot create default alarms Err: %d", nResult, 0, 0);
  }
  else
  {
    //Write out file with alarms in default state
    nResult = AlarmClock_SaveAlarms(pMe);
  }

  return nResult;
}

/*=============================================================================
FUNCTION: AlarmClock_NewAlarm

DESCRIPTION:  Create a new alarm struct and populate its other fields with 
              default values.

PARAMETERS:
  **ppAlarm   [out]:  Newly created Alarm struct 

RETURN VALUE:
   int:  BREW result code.

COMMENTS:
   Caller is responsible for free-ing *ppAlarm

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int AlarmClock_NewAlarm(Alarm **ppAlarm) 
{
  Alarm *pAlarm = NULL;
  int    nResult = ENOMEMORY;

  if (NULL == ppAlarm)
  {
    return EMEMPTR;
  }

  *ppAlarm = NULL;

  do
  {
    pAlarm = MALLOC(sizeof(Alarm));
    if (NULL == pAlarm)
    {
      break;
    }

    pAlarm->pDescription = WSTRDUP(L"");
    if (NULL == pAlarm->pDescription)
    {
      break;
    }

    pAlarm->pRingerID = WSTRDUP(L"");
    if (NULL == pAlarm->pRingerID)
    {
      break;
    }
    // Initialize other alarm fields 
    pAlarm->state = ALARM_STATE_INIT;
    pAlarm->bExpired = FALSE;
    pAlarm->bAlarmActive = FALSE;
    pAlarm->bExpiredBeforeValidTime = FALSE;
    pAlarm->uExpirationTime = ALARMCLOCK_INVALID_EXPIRATIONTIME;
    pAlarm->uLastExpirationTime = 0;
    pAlarm->recurrence = CLOCK_ONE_TIME;
    pAlarm->wCode = 0;

    nResult = SUCCESS;
  } while (0);

  if (SUCCESS != nResult && pAlarm != NULL)
  {
    AlarmClock_FreeAlarm(pAlarm);
    pAlarm = NULL; //lint !e423   this is not a memory leak
  }

  *ppAlarm = pAlarm;

  return nResult;

}

/*=============================================================================
FUNCTION: AlarmClock_FreeAlarm

DESCRIPTION:  Free the memory allocated for the Alarm struct.

PARAMETERS:
   *pAlarm   [in]:  Alarm struct  

RETURN VALUE:
   None

COMMENTS:


SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AlarmClock_FreeAlarm(Alarm *pAlarm) 
{
  if (pAlarm != NULL)
  {
    FREEIF(pAlarm->pRingerID);
    FREEIF(pAlarm->pDescription);
    FREE(pAlarm);
  }
}


/*=============================================================================
FUNCTION: AlarmClock_SetAlarm

DESCRIPTION:  Set the alarm to expire at the specified expiration time. 
              If the specified alarm expiration time is in the past and the 
              alarm is repeating, the expiration time is readjusted to the next 
              recurrence.

PARAMETERS:
  *pMe    [in]: AlarmActor object
  *pAlarm [in]: Alarm struct

RETURN VALUE:
   int:  BREW result code. 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*lint -e{818} suppress could be const */
static int AlarmClock_SetAlarm(AlarmClockApp *pMe, Alarm *pAlarm)
{
  uint32 uCurrentTime = GETTIMESECONDS();
  int    nResult = EFAILED;
  int    nInterval;

  if (NULL == pAlarm)
  {
    return EMEMPTR;
  }

  nInterval = (int)(pAlarm->uExpirationTime - uCurrentTime);

  // Readjust alarm expiration time
  // if the current expiration time is in the past
  if (nInterval <= 0)
  {
    AlarmClock_SetExpirationTimeToNextRepeat(pAlarm);
    nInterval = (int)(pAlarm->uExpirationTime - uCurrentTime);
  }

  // Expiration time is in the future.
  if (nInterval > 0)
  {
    uint32 uCode32 = IALARMMGR_SetUserCode(pAlarm->wCode, FALSE, TRUE);
    nResult = IALARMMGR_SetAlarm(pMe->pAlarmMgr, 0, &uCode32,
                                 pAlarm->uExpirationTime);

    if (SUCCESS == nResult)
    {
      // Save the user code generated by the BREW associated with this alarm
      pAlarm->wCode = IALARMMGR_GetUserCode(uCode32);
      pAlarm->bAlarmActive = TRUE;
      pAlarm->state = ALARM_STATE_RUNNING;
    }
  }

  return nResult;
}

/*=============================================================================
FUNCTION: AlarmClock_CancelAlarm

DESCRIPTION:  Cancel the pending IALARMGR associated with the specified Alarm
              struct

PARAMETERS:
   *pMe     [in]:  AlarmActor object
   *pAlarm  [in]:  Alarm struct
    
RETURN VALUE:
   int:  BREW result code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
/*lint -e{818} suppress could be const */
static int AlarmClock_CancelAlarm(AlarmClockApp *pMe, Alarm *pAlarm) 
{
  int nResult = EFAILED;

  if (pAlarm != NULL && pAlarm->bAlarmActive)
  {
    nResult = IALARMMGR_CancelAlarm(pMe->pAlarmMgr, 0, pAlarm->wCode);
    if (SUCCESS == nResult)
    {
      pAlarm->bAlarmActive = FALSE;
      pAlarm->state = ALARM_STATE_READY;
    }
  }
  return nResult;
}


/*=============================================================================
FUNCTION: AlarmClock_SetExpirationTimeToNextRepeat

DESCRIPTION: Move the expiration time of the specified alarm forward until it 
             is in the future.

PARAMETERS:
  *pAlarm   [in]: Alarm struct

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AlarmClock_SetExpirationTimeToNextRepeat(Alarm *pAlarm)
{
  uint32 uCurrentTime = GETTIMESECONDS();
  int    nAdjustment;

  if (NULL == pAlarm)
  {
    return;
  }

  // Adjust the expiration date to be today
  // (expiration time remains the same).
  pAlarm->uExpirationTime = (pAlarm->uExpirationTime % ALARMCLOCK_SECS_PER_DAY) +
                           (GETTIMESECONDS() / ALARMCLOCK_SECS_PER_DAY) * ALARMCLOCK_SECS_PER_DAY; 

  // Adjust the expiration date to the nearest recurrent day
  // if the recurrence is not one-time
  if (CLOCK_ONE_TIME != pAlarm->recurrence)
  {
    nAdjustment = AlarmClock_GetNumDaysToNearestRecurrentDay(pAlarm);
    if (nAdjustment > 0)
    {
      pAlarm->uExpirationTime += ALARMCLOCK_SECS_PER_DAY * (uint32)nAdjustment;
    }
  } 

  // Make sure that the expiration time is indeed in the future
  if (pAlarm->uExpirationTime <= uCurrentTime)
  {
    // For one-time and daily alarm
    pAlarm->uExpirationTime += ALARMCLOCK_SECS_PER_DAY; 
    // Move to the nearest recurrent day in the future for other
    // recurrence
    if (CLOCK_ONE_TIME != pAlarm->recurrence)
    {
      nAdjustment = AlarmClock_GetNumDaysToNearestRecurrentDay(pAlarm);
      if (nAdjustment > 0)
      {
        pAlarm->uExpirationTime += ALARMCLOCK_SECS_PER_DAY * (uint32)nAdjustment;
      }
    } 
  }
}


/*=============================================================================
FUNCTION: AlarmClock_SaveAlarms

DESCRIPTION: Save all the alarms in EFS storage
 
PARAMETERS:
   *pMe  [in]:  AlarmActor object

RETURN VALUE:
   int:  BREW result code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:


=============================================================================*/
/*lint -e{818} suppress could be const */
static int AlarmClock_SaveAlarms(AlarmClockApp *pMe) 
{
  IFile *pFile;
  int    nResult = SUCCESS;

  if (NULL == pMe->pFileMgr || NULL == pMe->pAlarmList)
  {
    return EMEMPTR;
  }

  // Note: OpenFile will return an error if the file exists and the 
  // _OFM_CREATE flag is used, so we first remove the file.
  (void) IFILEMGR_Remove(pMe->pFileMgr, ALARMCLOCK_ALARMINFO_FILENAME);

  pFile = IFILEMGR_OpenFile(pMe->pFileMgr, ALARMCLOCK_ALARMINFO_FILENAME, _OFM_CREATE);
  if (NULL == pFile)
  {
    return EFAILED;
  }

  do
  {
    uint32 uAlarmIndex;
    uint32 uAlarmCount = IVector_Size(pMe->pAlarmList);
    uint32 uSize = sizeof(uint32);

    // Save the time when the valid sys time was acquired
    if (uSize != IFILE_Write(pFile, (void *) &pMe->uSysTimeAcquiredTime, uSize))
    {
      nResult = EFAILED;
      break;
    }

    // Save the snooze duration
    if (uSize != IFILE_Write(pFile, (void *) &pMe->uSnoozeDuration, uSize))
    {
      nResult = EFAILED;
      break;
    }

    // Save the alarm count
    if (uSize != IFILE_Write(pFile, (void *) &uAlarmCount, uSize))
    {
      nResult = EFAILED;
      break;
    }

    // Save every alarm in the alarm list
    for (uAlarmIndex = 0; uAlarmIndex < uAlarmCount; uAlarmIndex++)
    {
      Alarm *pAlarm;

      pAlarm = IVector_ElementAt(pMe->pAlarmList, uAlarmIndex);
      if (NULL == pAlarm)
      {
        nResult = EFAILED;
        break;
      }

      uSize = sizeof(Alarm);
      // Save the alarm struct
      if (uSize != IFILE_Write(pFile, (void *) pAlarm, uSize))
      {
        nResult = EFAILED;
        break;
      }

      // Save the size and the content of the alarm ringer ID
      nResult = AlarmClock_SaveAlarmStringField(pFile, pAlarm->pRingerID);
      if (SUCCESS != nResult)
      {
        break;
      }

      // Save the size and the content of the alarm description
      nResult = AlarmClock_SaveAlarmStringField(pFile, pAlarm->pDescription);
      if (SUCCESS != nResult)
      {
        break;
      }
    } /* for (alarmIndex = 0; alarmIndex < alarmCount; alarmIndex++) */ 

  } while (0);

  RELEASEIF(pFile);

  return nResult;
}

/*=============================================================================
FUNCTION: AlarmClock_SaveAlarmStringField

DESCRIPTION: Writes pField buffer size and pField string to pFile.
 
PARAMETERS:
   *pFile    [in]:  IFile object
   *pField   [in]:  Pointer to an AECHAR string.

RETURN VALUE:
   int:  BREW result code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*lint -e{818} suppress could be const */
static int AlarmClock_SaveAlarmStringField(IFile *pFile, AECHAR *pField)
{
  int    nResult = EFAILED;
  uint32 uSize;

  if (NULL == pFile)
  {
    return EBADPARM;
  }

  do
  {
    // Save the size and the content of the pField
    if (pField != NULL)
    {
      uSize = (uint32) (WSTRLEN(pField) + 1) * sizeof(AECHAR);
      if (sizeof(uint32) != IFILE_Write(pFile, (void *) &uSize, sizeof(uint32)))
      {
        nResult = EFAILED;
        break;
      }
      if (uSize != IFILE_Write(pFile, (void *) pField, uSize))
      {
        nResult = EFAILED;
        break;
      }
      nResult = SUCCESS;
    }
  } while (FALSE);

  return nResult;
}

/*=============================================================================
FUNCTION: AlarmClock_LoadAlarms

DESCRIPTION: Load the alarms stored in EFS storage to memory
 
PARAMETERS:
   *pMe  [in]:  AlarmClockApp object

RETURN VALUE:
   int:  BREW result code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:


=============================================================================*/
static int  AlarmClock_LoadAlarms(AlarmClockApp *pMe) 
{
  IFile *pFile = NULL;   
  uint32 uSize = sizeof(uint32);
  uint32 uAlarmCount = 0;
  uint32 uAlarmIndex;
  int    nResult = SUCCESS;

  if (NULL == pMe->pFileMgr || NULL == pMe->pAlarmList)
  {
    return EMEMPTR;
  }

  // Try to open the alarm info file
  pFile = IFILEMGR_OpenFile(pMe->pFileMgr, ALARMCLOCK_ALARMINFO_FILENAME, _OFM_READ);
  if (NULL == pFile)
  {
    return EFILENOEXISTS;
  }
  do
  {
    // Retrieve the time when last system time was acquired
    if (uSize != (uint32) IFILE_Read(pFile, (void *) &pMe->uSysTimeAcquiredTime, uSize))
    {
      nResult = EFAILED;
      break;
    }

    // Retrieve the snooze duration
    if (uSize != (uint32) IFILE_Read(pFile, (void *) &pMe->uSnoozeDuration, uSize))
    {
      nResult = EFAILED;
      break;
    }

    // Retrieve the alarm count
    if (uSize != (uint32) IFILE_Read(pFile, (void *) &uAlarmCount, uSize))
    {
      uAlarmCount = 0;
      nResult = EFAILED;
      break;
    }

  } while (0);

  // Retrieve all the saved alarms
  for (uAlarmIndex = 0; uAlarmIndex < uAlarmCount; uAlarmIndex++)
  {
    Alarm *pAlarm = NULL;

    do
    {
      // Create a new alarm and intialized with the saved values
      uSize = sizeof(Alarm);
      pAlarm = MALLOC(uSize);
      if (NULL == pAlarm)
      {
        nResult = ENOMEMORY;
        break;
      }

      if (uSize != (uint32) IFILE_Read(pFile, (void *) pAlarm, uSize))
      {
        nResult = EFAILED;
        break;
      }

      // Retrieve ringerID   
      nResult = AlarmClock_PopulateAlarmStringField(
                                                  pFile, 
                                                  &pAlarm->pRingerID
                                                  );
      if (nResult != SUCCESS)
      {
        break;
      }

      // Retrieve description   
      nResult = AlarmClock_PopulateAlarmStringField(
                                                  pFile, 
                                                  &pAlarm->pDescription
                                                  );
    } while (0);

    if (SUCCESS == nResult && pAlarm != NULL)
    {
      // Append the newly created alarm to the alarm list
      nResult = IVector_AddElement(pMe->pAlarmList, pAlarm);
    } 
    else
    {
      if (pAlarm != NULL)
      {
        AlarmClock_FreeAlarm(pAlarm);
      }
      break;
    }

  }  /* for (alarmIndex = 0; alarmIndex < alarmCount; alarmIndex++) */

  if (nResult != SUCCESS)
  {
    AlarmClock_FreeListData(pMe);
  }

  RELEASEIF(pFile);

  return nResult;
}

/*=============================================================================
FUNCTION: AlarmClock_PopulateAlarmStringField

DESCRIPTION: Allocate memory and copy the content of the specified alarm string 
             field in the EFS to memory.
 
PARAMETERS:
   *pFile     [in]:  IFile object
  **ppField  [out]:  Pointer to an AECHAR string.

RETURN VALUE:
   int:  BREW result code.

COMMENTS:
   Caller is responsible for free-ing memory *ppField.

SIDE EFFECTS:

SEE ALSO:


=============================================================================*/
static int AlarmClock_PopulateAlarmStringField(IFile *pFile, AECHAR **ppField)
{
  int    nResult = SUCCESS;
  uint32 uSize = 0;

  if (NULL == pFile || NULL == ppField)
  {
    return EMEMPTR;
  }

  *ppField = NULL;

  do
  {
    // Get the size of the string field
    if (sizeof(uint32) != (uint32) IFILE_Read(pFile, &uSize, sizeof(uint32)))
    {
      nResult = EFAILED;
      break;
    }

    // Allocate memory to store the string
    *ppField = MALLOC(uSize);
    if (NULL == *ppField)
    {
      nResult = ENOMEMORY;
      break;
    }

    // Copy the string from the EFS storage
    if (uSize != (uint32)IFILE_Read(pFile, (void *) *ppField, uSize))
    {
      nResult = EFAILED;
    }
  } while (0);

  return nResult;
}

/*=============================================================================
FUNCTION: AlarmClock_HasActiveAlarm

DESCRIPTION: Whether there is an active IALARMGR alarm running in the alarm list.
 
PARAMETERS:
   *pMe  [in]:  AlarmClockApp object

RETURN VALUE:
   None

COMMENTS:
   boolean:

SIDE EFFECTS:

SEE ALSO:


=============================================================================*/
/*lint -e{818} suppress could be const */
static boolean AlarmClock_HasActiveAlarm(AlarmClockApp *pMe) 
{
  uint32 i;

  for (i = 0; i < IVector_Size(pMe->pAlarmList); i++)
  {
    Alarm *pAlarm = NULL;

    pAlarm = IVector_ElementAt(pMe->pAlarmList, i);
    if ((NULL != pAlarm) && pAlarm->bAlarmActive)
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*=============================================================================
FUNCTION: AlarmClock_GetNumDaysToNearestRecurrentDay

DESCRIPTION: Get the number of days to the nearest recurrent day for a
             repeating alarm
 
PARAMETERS:
   *pAlarm   [in]:  Specified alarm

RETURN VALUE:
   int: the number of days to the nearest recurrent day, or -1 if the alarm
        is not recurrent or pAlarm is null.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:


=============================================================================*/
/*lint -e{818} suppress could be const */
static int AlarmClock_GetNumDaysToNearestRecurrentDay(Alarm *pAlarm) 
{
  int        nNumDaysToNearestRecurrentDay = 0;
  JulianType jt;
  Days       day;
  uint32     uMask = 1;

  if (NULL == pAlarm || !AlarmClock_IsRepeatingAlarm(pAlarm))
  {
    return -1;
  }

  GETJULIANDATE(pAlarm->uExpirationTime, &jt);
  // Start with the recurrent day to the Sunday of that week to find out
  // what is the nearest recurrent day specified in the alarm recurrence.
  for (day = (Days)jt.wWeekDay; day <= CLOCK_SUNDAY; day++)
  {
    if (pAlarm->recurrence & (uMask << (uint16)day))
    {
      return nNumDaysToNearestRecurrentDay;
    }
    nNumDaysToNearestRecurrentDay++;
  }

  // If we have not found the nearest recurrent day in that week,
  // try the following week starting with Monday.
  for (day = CLOCK_MONDAY; day < (Days)jt.wWeekDay; day++)
  {
    if (pAlarm->recurrence & (uMask << (uint16)day))
    {
      break;
    }
    nNumDaysToNearestRecurrentDay++;
  }

  return nNumDaysToNearestRecurrentDay;
}

/*=============================================================================
FUNCTION: AlarmClock_FreeListData

DESCRIPTION:
   Release the heap used by the current fields in pMe->pAlarmList

PARAMETERS:
   *pMe: pointer to AlarmClockApp object

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*lint -e{818} suppress could be const */
static void AlarmClock_FreeListData(AlarmClockApp *pMe)
{
  uint32  i;
  Alarm  *pAlarm = NULL;

  // free the data first
  for (i = 0; i < IVector_Size(pMe->pAlarmList); i++)
  {
    pAlarm = (Alarm*)IVector_ElementAt(pMe->pAlarmList, i);

    if (NULL == pAlarm)
    {
      ALARM_CLK_ERR("NULL Alarm entry, %d.", i, 0, 0);
      continue;
    }
    FREEIF(pAlarm->pDescription);
    FREEIF(pAlarm->pRingerID);
    FREE(pAlarm);
  }

  IVector_RemoveAllElements(pMe->pAlarmList);
}

/*===========================================================================
FUNCTION AlarmClockListDlgEventHandler

DESCRIPTION
  handle AlarmClock List menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean AlarmClock_ListDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AlarmClockApp *pMe = (AlarmClockApp*)pUser;

  if (pMe == NULL)
  { /* error checking */
    ALARM_CLK_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if ((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (evt)
  {    
    case EVT_DIALOG_START:
      ALARM_CLK_MSG("MenuDlg: EVT_DIALOG_START", 0, 0, 0);
      if (SUCCESS == AlarmClock_RefreshDialog(pMe))
      {
        return TRUE;
      }
      ALARM_CLK_ERR("MenuDlg: EVT_DIALOG_START Error", 0,0,0);
      return FALSE;

    case EVT_KEY:
      ALARM_CLK_MSG("Got EVT_KEY %d", w, 0, 0);
      switch (w)
      {
        case AVK_CLR:
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          return TRUE;

        case AVK_END:
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          return TRUE;

        /* digit keys */
        case AVK_0:
        case AVK_1:
        case AVK_2:
        case AVK_3:
        case AVK_4:
        case AVK_5:
        case AVK_6:
        case AVK_7:
        case AVK_8:
        case AVK_9:
        case AVK_STAR:
        case AVK_POUND:
        /* navigation keys */
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
        case AVK_SELECT:
        /* send key */
        case AVK_SEND:
        case AVK_MESSAGE:
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE;

        default:
          return FALSE;
      }

    case EVT_COMMAND:
      {
        int nResult = SUCCESS;

        ALARM_CLK_MSG("MenuDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
        pMe->wAlarmSelIdx = w;

        nResult = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAlarmClockResourceFile,
                                      IDD_DIALOG_ALARM_OPTIONS, NULL);
        if (SUCCESS != nResult)
        {
          ALARM_CLK_ERR("MenuDlg: EVT_COMMAND, Cannot create dialog w=%d, dw=%d", w, dw, 0);
        }
      }
      return TRUE;

    case EVT_DIALOG_END:

      ALARM_CLK_MSG("MenuDlg: EVT_DIALOG_END", 0, 0, 0);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      break;
  }

  return FALSE;
} /* End AlarmClockListDlgEventHandler */

/*===========================================================================
FUNCTION AlarmClockOptionsDlgEventHandler

DESCRIPTION
  handle AlarmClock alert menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean AlarmClock_OptionsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AlarmClockApp *pMe = (AlarmClockApp*) pUser;
  IMenuCtl      *pICurrentMenu;

  if (pMe == NULL)
  { /* error checking */
    ALARM_CLK_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if ((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (evt)
  {
    case EVT_DIALOG_START:
      ALARM_CLK_MSG("MenuDlg: EVT_DIALOG_START", 0, 0, 0);
      if (SUCCESS == AlarmClock_RefreshDialog(pMe))
      {
        return TRUE;
      }
      ALARM_CLK_ERR("MenuDlg: EVT_DIALOG_START error", 0,0,0);
      return FALSE;

    case EVT_KEY:
      ALARM_CLK_MSG("Got EVT_KEY %d", w, 0, 0);
      switch (w)
      {
        case AVK_CLR:
          AlarmClock_EndCurrentDlg(pMe);
          return TRUE;

        case AVK_END:
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          return TRUE;

        /* digit keys */
        case AVK_0:
        case AVK_1:
        case AVK_2:
        case AVK_3:
        case AVK_4:
        case AVK_5:
        case AVK_6:
        case AVK_7:
        case AVK_8:
        case AVK_9:
        case AVK_STAR:
        case AVK_POUND:
        /* navigation keys */
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
        case AVK_SELECT:
        /* send key */
        case AVK_SEND:
        case AVK_MESSAGE:
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE;

        default:
          return FALSE;
      }

    case EVT_COMMAND:
      {
        IDialog *pIActiveDialog = NULL;
        int      nResult = SUCCESS;
        Alarm   *pAlarm = NULL;

        ALARM_CLK_MSG("MenuDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

        pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if (pIActiveDialog == NULL)
        { /* error checking */
          ALARM_CLK_ERR("Null pointer", 0,0,0);
          return FALSE;
        }

        pICurrentMenu = (IMenuCtl*)(void*)IDIALOG_GetControl((IDialog*)pIActiveDialog, (int16) IDC_OPTIONS_MENU);
        if (pICurrentMenu == NULL)
        { /* error checking */
          ALARM_CLK_ERR("Null pointer", 0,0,0);
          return FALSE;
        }

        pAlarm = (Alarm*)IVector_ElementAt(pMe->pAlarmList, pMe->wAlarmSelIdx);
        if (pAlarm == NULL)
        { /* error checking */
          ALARM_CLK_ERR("Null pointer", 0,0,0);
          return FALSE;
        }

        switch (w)
        {
          case IDC_CONTROL_ALARM_EDIT:
            nResult = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAlarmClockResourceFile,
                                          IDD_DIALOG_ALARM_EDIT, NULL);
            break;
          case IDC_CONTROL_ALARM_TURN_ON:
            nResult = AlarmClock_SetAlarm(pMe, pAlarm);
            if (SUCCESS != nResult)
            {
              return FALSE;
            }
            (void)IMENUCTL_DeleteItem(pICurrentMenu, IDC_CONTROL_ALARM_TURN_ON);
            (void)IMENUCTL_AddItem(pICurrentMenu, gpAlarmClockResourceFile,
                                   IDS_STRING_TURN_OFF, IDC_CONTROL_ALARM_TURN_OFF,
                                   NULL, 0);
            IMENUCTL_SetFocus(pICurrentMenu, IDC_CONTROL_ALARM_TURN_OFF);
            (void)IMENUCTL_Redraw(pICurrentMenu);
            ALARM_CLK_MSG("TURN ON ALARM %d", pMe->wAlarmSelIdx, 0, 0);
            AlarmClock_UpdateAnnun(pMe);
            AlarmClock_SaveAlarms(pMe);
            break;
          case IDC_CONTROL_ALARM_TURN_OFF:
            nResult = AlarmClock_CancelAlarm(pMe, pAlarm);
            if (SUCCESS != nResult)
            {
              return FALSE;
            }
            (void)IMENUCTL_DeleteItem(pICurrentMenu, IDC_CONTROL_ALARM_TURN_OFF);
            (void)IMENUCTL_AddItem(pICurrentMenu, gpAlarmClockResourceFile,
                                   IDS_STRING_TURN_ON, IDC_CONTROL_ALARM_TURN_ON,
                                   NULL, 0);
            IMENUCTL_SetFocus(pICurrentMenu, IDC_CONTROL_ALARM_TURN_ON);
            (void)IMENUCTL_Redraw(pICurrentMenu);
            ALARM_CLK_MSG("TURN OFF ALARM %d", pMe->wAlarmSelIdx, 0, 0);
            AlarmClock_UpdateAnnun(pMe);
            AlarmClock_SaveAlarms(pMe);
            break;
          case IDC_CONTROL_ALARM_DESC:
            nResult = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAlarmClockResourceFile,
                                          IDD_DIALOG_ALARM_DESC, NULL);
            break;
          default:
            ALARM_CLK_MSG("Unhandled EVT_COMMAND", 0, 0, 0);
            break;
        }
      }
      return TRUE;

    case EVT_DIALOG_END:
      (void)AlarmClock_RefreshDialog(pMe);
      ALARM_CLK_MSG("MenuDlg: EVT_DIALOG_END", 0, 0, 0);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      break;
  }

  return FALSE;
} /* End AlarmClockOptionsDlgEventHandler */

/*===========================================================================
FUNCTION AlarmClockEditDlgEventHandler

DESCRIPTION
  handle AlarmClock setting menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean AlarmClock_EditDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AlarmClockApp *pMe = (AlarmClockApp*) pUser;
  ITimeCtl      *pITimeCtl = NULL;
  IMenuCtl      *pICurrentMenu = NULL;
  IMenuCtl      *pISoftKeyMenu = NULL;

  if (pMe == NULL)
  { /* error checking */
    ALARM_CLK_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if ((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (evt)
  {
    case EVT_DIALOG_START:
      ALARM_CLK_MSG("MenuDlg: EVT_DIALOG_START", 0, 0, 0);
      pITimeCtl =     (ITimeCtl*)(void*)IDIALOG_GetControl((IDialog*)dw, (int16) IDC_CLOCK_ALARM_SETTING);
      pICurrentMenu = (IMenuCtl*)(void*)IDIALOG_GetControl((IDialog*)dw, (int16) IDC_LIST_ALARM_SETTING);
      pISoftKeyMenu = (IMenuCtl*)(void*)IDIALOG_GetControl((IDialog*)dw, (int16) IDC_SOFTKEY_ALARM_SETTING);

      if (pITimeCtl && pICurrentMenu)
      {
        Alarm  *pAlarm = NULL;

        SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
        SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);

        if (NULL != (pAlarm = (Alarm*)IVector_ElementAt(pMe->pAlarmList, pMe->wAlarmSelIdx)))
        {
          JulianType julian_time;
          int32      nTimeCtlDispTime;
          uint16     wItem = IDC_CONTROL_ITEM_ONCE;
          AEERect    rcMenu = {0};
          AEERect    rcTime = {0};
          uint32     uTimeCtlProps;

          if (pAlarm->uExpirationTime != ALARMCLOCK_INVALID_EXPIRATIONTIME)
          {
            GETJULIANDATE(pAlarm->uExpirationTime, &julian_time);
            julian_time.wHour   = 0;
            julian_time.wMinute = 0;
            julian_time.wSecond = 0;

            nTimeCtlDispTime = (int32)((pAlarm->uExpirationTime - JULIANTOSECONDS(&julian_time)) * 1000);
          }
          else
          {
            nTimeCtlDispTime = 0;
          }

          if (CLOCK_EVERY_DAY == pAlarm->recurrence)
          {
            wItem = IDC_CONTROL_ITEM_DAILY;
          }

          //Layout Menu Items
          rcMenu.x  = 2;
          rcMenu.y  = 20;
          rcMenu.dx = (int16)pMe->wCxScreen - 2;
          rcMenu.dy = 30;
          rcTime.x  = 2;
          rcTime.y  = rcMenu.dy + 30;
          rcTime.dx = (int16)pMe->wCxScreen - 2;
          rcTime.dy = 30;
          IMENUCTL_SetRect(pICurrentMenu, &rcMenu);
          ITIMECTL_SetRect(pITimeCtl, &rcTime);

          uTimeCtlProps = ITIMECTL_GetProperties(pITimeCtl);
          uTimeCtlProps |= TP_AUTOREDRAW;
          ITIMECTL_SetProperties(pITimeCtl, uTimeCtlProps);

          (void)ITIMECTL_SetTime(pITimeCtl, nTimeCtlDispTime);
          IMENUCTL_SetSel(pICurrentMenu, wItem);

          (void)IDIALOG_SetFocus((IDialog*)dw, IDC_LIST_ALARM_SETTING);
        }
        return TRUE;
      }
      ALARM_CLK_ERR("Null pointer", 0,0,0);
      return FALSE;

    case EVT_KEY:
      ALARM_CLK_MSG("Got EVT_KEY %d", w, 0, 0);
      switch (w)
      {
        case AVK_CLR:
          AlarmClock_EndCurrentDlg(pMe);
          return TRUE;

        case AVK_END:
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          return TRUE;

        /* digit keys */
        case AVK_0:
        case AVK_1:
        case AVK_2:
        case AVK_3:
        case AVK_4:
        case AVK_5:
        case AVK_6:
        case AVK_7:
        case AVK_8:
        case AVK_9:
        case AVK_STAR:
        case AVK_POUND:
        /* navigation keys */
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
        case AVK_SELECT:
        /* send key */
        case AVK_SEND:
        case AVK_MESSAGE:
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE;

        default:
          return FALSE;
      }

    case EVT_COMMAND:
      {
        IDialog *pIActiveDialog = NULL;
        uint16   wItem;
        ALARM_CLK_MSG("MenuDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

        switch (w)
        {
          case IDC_SOFTKEY_ITEM_SAVE:

            pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
            if (pIActiveDialog == NULL)
            { /* error checking */
              ALARM_CLK_ERR("Null pointer", 0,0,0);
              return FALSE;
            }

            pITimeCtl = (ITimeCtl*)(void*)IDIALOG_GetControl((IDialog*)pIActiveDialog, 
                                                             (int16) IDC_CLOCK_ALARM_SETTING);
            pICurrentMenu = (IMenuCtl*)(void*)IDIALOG_GetControl((IDialog*)pIActiveDialog, 
                                                                 (int16) IDC_LIST_ALARM_SETTING);

            if (pITimeCtl && pICurrentMenu)
            {
              Alarm *pAlarm = NULL;
              int32  nTimeCtlDispTime = ITIMECTL_GetTime(pITimeCtl);

              if (NULL != (pAlarm = (Alarm*)IVector_ElementAt(pMe->pAlarmList, pMe->wAlarmSelIdx)))
              {
                // Cancel any pending shell alarm
                (void)AlarmClock_CancelAlarm(pMe, pAlarm);

                // Adjust the expiration date to be today
                // (expiration time remains the same).
              
                pAlarm->uExpirationTime = (uint32)(nTimeCtlDispTime / 1000) +
                  (GETTIMESECONDS() / ALARMCLOCK_SECS_PER_DAY) * ALARMCLOCK_SECS_PER_DAY;

                wItem = IMENUCTL_GetSel(pICurrentMenu);
                if (IDC_CONTROL_ITEM_ONCE == wItem)
                {
                  pAlarm->recurrence = CLOCK_ONE_TIME;
                }
                else
                {
                  pAlarm->recurrence = CLOCK_EVERY_DAY;
                }

                pAlarm->state = ALARM_STATE_READY;
              }

              AlarmClock_UpdateAnnun(pMe);
              AlarmClock_SaveAlarms(pMe);
            }
            AlarmClock_EndCurrentDlg(pMe);
            break;

          case IDC_SOFTKEY_ITEM_CANCEL:
            AlarmClock_EndCurrentDlg(pMe);
            break;
          default:
            ALARM_CLK_MSG("Unhandled EVT_COMMAND", 0, 0, 0);
            break;
        }
      }
      return TRUE;

    case EVT_DIALOG_END:
      (void)AlarmClock_RefreshDialog(pMe);
      ALARM_CLK_MSG("MenuDlg: EVT_DIALOG_END", 0, 0, 0);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      break;
  }

  return FALSE;
} /* End AlarmClockEditDlgEventHandler */

/*===========================================================================
FUNCTION AlarmClockAlertDlgEventHandler

DESCRIPTION
  handle AlarmClock alert menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean AlarmClock_AlertDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AlarmClockApp *pMe = (AlarmClockApp*) pUser;
  IMenuCtl      *pICurrentMenu = NULL;
  IStatic       *pIStatic = NULL;

  if (pMe == NULL)
  { /* error checking */
    ALARM_CLK_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if ((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (evt)
  {  
    case EVT_DIALOG_START:
      ALARM_CLK_MSG("MenuDlg: EVT_DIALOG_START", 0, 0, 0);
      pICurrentMenu = (IMenuCtl*)(void*)IDIALOG_GetControl((IDialog*)dw, (int16) IDC_SOFTKEY_ALARM_MSG);
      pIStatic = (IStatic*)(void*)IDIALOG_GetControl((IDialog*)dw, (int16) IDC_STATIC_ALARM_MSG);

      if (pICurrentMenu && pIStatic)
      {
        Alarm      *pAlarm = NULL;

        SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);

        pAlarm = IVector_ElementAt(pMe->pAlarmList, pMe->wAlarmExpiredIdx);

        if (NULL != pAlarm)
        {
          AECHAR szAlertText[ALARMCLOCK_ALERT_FLD_LEN+1];

          AlarmClock_FormatAlertStr(pMe, pAlarm, szAlertText, sizeof(szAlertText), '\n');
     
          ISTATIC_SetProperties(pIStatic, ST_CENTERTEXT | ST_CENTERTITLE | ST_MIDDLETEXT);
          ISTATIC_SetText(pIStatic, NULL, szAlertText, AEE_FONT_BOLD, AEE_FONT_BOLD);
          IDIALOG_Redraw ((IDialog*)dw);

          (void) ISHELL_CancelTimer(pMe->a.m_pIShell, AlarmClock_AlertTimerCB, (void*)pMe);
          (void) ISHELL_SetTimer(pMe->a.m_pIShell, ALARMCLOCK_ALERT_TIMEOUT, 
                                 AlarmClock_AlertTimerCB, (void*)pMe);

          (void) IALERT_StartAlerting(pMe->pIAlert, AEEALERT_CALLTYPE_VOICE, NULL, 
                                      AEEALERT_ALERT_NORMAL);
        }

        return TRUE;
      }

      ALARM_CLK_ERR("Null pointer", 0,0,0);
      return FALSE;

    case EVT_KEY:
      ALARM_CLK_MSG("Got EVT_KEY %d", w, 0, 0);
      switch (w)
      {
        case AVK_CLR:
          AlarmClock_EndCurrentDlg(pMe);
          return TRUE;

        case AVK_END:
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          return TRUE;

        /* digit keys */
        case AVK_0:
        case AVK_1:
        case AVK_2:
        case AVK_3:
        case AVK_4:
        case AVK_5:
        case AVK_6:
        case AVK_7:
        case AVK_8:
        case AVK_9:
        case AVK_STAR:
        case AVK_POUND:
        /* navigation keys */
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
        case AVK_SELECT:
        /* send key */
        case AVK_SEND:
        case AVK_MESSAGE:
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE;

        default:
          return FALSE;
      }

    case EVT_COMMAND:
      {
        IDialog *pIActiveDialog = NULL;
        ALARM_CLK_MSG("MenuDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

        if (IDC_SOFTKEY_ITEM_DISMISS == w)
        {
          pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          if (pIActiveDialog == NULL)
          { /* error checking */
            ALARM_CLK_ERR("Null pointer", 0,0,0);
            return FALSE;
          }
          AlarmClock_EndCurrentDlg(pMe);
        }
      }
      return TRUE;

    case EVT_DIALOG_END:
      {
        ALARM_CLK_MSG("MenuDlg: EVT_DIALOG_END", 0, 0, 0);
        (void) ISHELL_CancelTimer(pMe->a.m_pIShell, AlarmClock_AlertTimerCB, (void*)pMe);
        (void) IALERT_StopAlerting(pMe->pIAlert);
        if (SUCCESS != AlarmClock_RefreshDialog(pMe))
        {
          ALARM_CLK_MSG("Close App all dialogs have ended.", 0, 0, 0);
          ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
        }
      }
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      break;
  }

  return FALSE;
} /* End AlarmClockAlertDlgEventHandler */

/*===========================================================================
FUNCTION AlarmClock_DescDlgEventHandler

DESCRIPTION
  handle AlarmClock Description menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean AlarmClock_DescDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AlarmClockApp *pMe = (AlarmClockApp*) pUser;
  IMenuCtl      *pISKMenu = NULL;
  ITextCtl      *pIText  = NULL;

  if (pMe == NULL)
  { /* error checking */
    ALARM_CLK_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if ((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (evt)
  {
    case EVT_DIALOG_START:
      ALARM_CLK_MSG("MenuDlg: EVT_DIALOG_START", 0, 0, 0);
      pISKMenu = (IMenuCtl*)(void*)IDIALOG_GetControl((IDialog*)dw, (int16) IDC_SOFTKEY_ALARM_DESC);
      pIText   = (ITextCtl*)(void*)IDIALOG_GetControl((IDialog*)dw, (int16) IDC_TEXT_ALARM_DESC);

      if (pISKMenu && pIText)
      {
        Alarm *pAlarm = NULL;

        SetDefaultMenuLook(pMe->a.m_pIShell, pISKMenu);
        pAlarm = IVector_ElementAt(pMe->pAlarmList, pMe->wAlarmSelIdx);

        if (NULL != pAlarm)
        {
          ITEXTCTL_SetMaxSize(pIText, ALARMCLOCK_ALERT_FLD_LEN);
          (void)ITEXTCTL_SetText(pIText, pAlarm->pDescription, -1);
          (void)IDIALOG_SetFocus((IDialog*)dw, IDC_TEXT_ALARM_DESC);
        }
        return TRUE;
      }

      ALARM_CLK_ERR("Null pointer", 0,0,0);
      return FALSE;

    case EVT_KEY:
      ALARM_CLK_MSG("Got EVT_KEY %d", w, 0, 0);
      switch (w)
      {
        case AVK_CLR:
          AlarmClock_EndCurrentDlg(pMe);
          return TRUE;

        case AVK_END:
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          return TRUE;

        /* digit keys */
        case AVK_0:
        case AVK_1:
        case AVK_2:
        case AVK_3:
        case AVK_4:
        case AVK_5:
        case AVK_6:
        case AVK_7:
        case AVK_8:
        case AVK_9:
        case AVK_STAR:
        case AVK_POUND:
        /* navigation keys */
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
        case AVK_SELECT:
        /* send key */
        case AVK_SEND:
        case AVK_MESSAGE:
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE;

        default:
          return FALSE;
      }

    case EVT_COMMAND:
      {
        IDialog *pIActiveDialog = NULL;
        ALARM_CLK_MSG("MenuDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

        if (IDC_ALARM_DESC_SAVE == w)
        {
          pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          if (pIActiveDialog == NULL)
          { /* error checking */
            ALARM_CLK_ERR("Null pointer", 0,0,0);
            return FALSE;
          }

          pIText = (ITextCtl*)(void*)IDIALOG_GetControl((IDialog*)pIActiveDialog, (int16) IDC_TEXT_ALARM_DESC);

          if (pIText)
          {
            Alarm *pAlarm = NULL;

            pAlarm = IVector_ElementAt(pMe->pAlarmList, pMe->wAlarmSelIdx);
            if (NULL != pAlarm)
            {
              AECHAR *pText = ITEXTCTL_GetTextPtr(pIText);

              if (NULL != pText)
              {
                FREEIF(pAlarm->pDescription);
                pAlarm->pDescription = WSTRDUP(pText);
                if (NULL == pAlarm->pDescription)
                {
                  ALARM_CLK_ERR("NO MEMORY", 0,0,0);
                  return FALSE;
                }
                (void) AlarmClock_SaveAlarms(pMe);
                AlarmClock_EndCurrentDlg(pMe);
              }
            }
            return TRUE;
          }
        }
        else if (IDC_ALARM_DESC_CANCEL == w)
        {
          AlarmClock_EndCurrentDlg(pMe);
        }
      }
      return TRUE;

    case EVT_DIALOG_END:
      {
        ALARM_CLK_MSG("MenuDlg: EVT_DIALOG_END", 0, 0, 0);
        (void)AlarmClock_RefreshDialog(pMe);
      }
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      break;
  }

  return FALSE;
} /* End AlarmClock_DescDlgEventHandler */

/*===========================================================================
FUNCTION AlarmClockEndCurrentDlg

DESCRIPTION
  End the current dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -e{818} suppress could be const */
static void AlarmClock_EndCurrentDlg(AlarmClockApp *pMe)
{
  int      nReturnStatus;
  IDialog *pIActiveDialog;
  uint16   wDlgID;

  if (pMe == NULL)
  {
    ALARM_CLK_ERR("pMe NULL", 0, 0, 0);
    return;
  }

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog != NULL)
  {
    wDlgID = IDIALOG_GetID(pIActiveDialog);
    if (wDlgID == 0)
    { /* error */
      return;
    }

    nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
    if (nReturnStatus != SUCCESS)
    {
      ALARM_CLK_ERR("Fail to end dialog: %d", nReturnStatus, 0, 0);
    }
  }
} /*End AlarmClockEndCurrentDlg */

/*=============================================================================
FUNCTION: AlarmClockDlgHandler_PopulateListMenu

DESCRIPTION: Populate menu with Alarm List.

PARAMETERS:
   *pMe: AlarmClockApp object pointer
   *ctrl: IMenuCtl interface pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*lint -e{818} suppress could be const */
static void AlarmClock_PopulateListMenu(AlarmClockApp *pMe,
                                        IMenuCtl      *ctrl)
{
  CtlAddItem   ai;
  Alarm       *pAlarm = NULL;
  uint16       i;
  AECHAR       szMenuItem[ALARMCLOCK_FLD_LEN+1];

  // init common fields
  ai.pImage = NULL;
  ai.pszResText = NULL;
  ai.pszResImage =  gpAlarmClockResourceFile;
  ai.wText = 0;
  ai.wFont = 0;
  ai.dwData = 0;

  IMENUCTL_SetProperties(ctrl, IMENUCTL_GetProperties(ctrl) | MP_NO_REDRAW);
  (void) IMENUCTL_DeleteAll(ctrl);

  for (i = 0; i < (uint16)IVector_Size(pMe->pAlarmList); i++)
  {
    pAlarm = IVector_ElementAt(pMe->pAlarmList, i);
    if (NULL == pAlarm)
    {
      // Probably at end of alarms
      break;
    }

    if ((pAlarm->state == ALARM_STATE_INIT) || (pAlarm->state == ALARM_STATE_INVALID))
    {
      //Display Unassigned resource string
      (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  gpAlarmClockResourceFile,
                                  IDS_STRING_UNASSIGNED,
                                  szMenuItem,
                                  sizeof(szMenuItem));
    } 
    else
    {
      AlarmClock_FormatAlertStr(pMe, pAlarm, szMenuItem, sizeof(szMenuItem), ' ');
    }

    ai.pText = szMenuItem;

    switch (pAlarm->state)
    {
      case ALARM_STATE_READY:
        ai.wImage = IDI_READY_STATE;
        break;
      case ALARM_STATE_RUNNING:
        ai.wImage = IDI_RUNNING_STATE;
        break;
      case ALARM_STATE_EXPIRED:
        ai.wImage = IDI_EXPIRED_STATE;
        break;
      case ALARM_STATE_SNOOZE:
        ai.wImage = IDI_SNOOZE_STATE;
        break;
      default:
        ai.wImage = IDI_INIT_STATE;
        break;
    }

    ai.wItemID = i;

    (void) IMENUCTL_AddItemEx(ctrl, &ai);
  }

  IMENUCTL_SetSel(ctrl, pMe->wAlarmSelIdx);
  IMENUCTL_SetProperties(ctrl, IMENUCTL_GetProperties(ctrl) & ~MP_NO_REDRAW);
  (void) IMENUCTL_Redraw(ctrl);
}

/*=============================================================================
FUNCTION: AlarmClockUpdateAnnun

DESCRIPTION: Update alarm clock annunciator.

PARAMETERS:
   *pMe: AlarmClockApp object pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AlarmClock_UpdateAnnun(AlarmClockApp *pMe)
{
  uint32 uAnnunState = ANNUN_STATE_OFF;

  if (NULL == pMe)
  {
    return;
  }

  if (AlarmClock_HasActiveAlarm(pMe))
  {
    uAnnunState = ANNUN_STATE_ON;
  }

  IANNUNCIATOR_SetField(pMe->pIAnn, ANNUN_FIELD_ALARM, uAnnunState);
}

/*=============================================================================
FUNCTION: AlarmClockRefreshDialog

DESCRIPTION: Refresh active dialog.

PARAMETERS:
   *pMe: AlarmClockApp object pointer

RETURN VALUE:
   int:  BREW result code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int AlarmClock_RefreshDialog(AlarmClockApp *pMe)
{
  IDialog *pIActiveDialog = NULL;
  uint16   wDlgId;
  int      nResult = EFAILED;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (NULL == pIActiveDialog)
  {
    ALARM_CLK_MSG("AlarmClock_RefreshDialog Root Dialog Dismissed", 0, 0, 0);
    return nResult;
  }

  wDlgId = IDIALOG_GetID(pIActiveDialog);
  switch(wDlgId)
  {
    case IDD_DIALOG_ALARM_LIST:
      {
        IMenuCtl *pICurrentMenu = (IMenuCtl*)(void*)IDIALOG_GetControl((IDialog*)pIActiveDialog, 
                                                                       (int16) IDC_ALARM_LIST_MENU);
        if (pICurrentMenu)
        {
          SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
          AlarmClock_PopulateListMenu(pMe, pICurrentMenu);
          nResult = SUCCESS;
        }
      }
      break;

    case IDD_DIALOG_ALARM_OPTIONS:
      {
        IMenuCtl *pICurrentMenu = (IMenuCtl*)(void*)IDIALOG_GetControl((IDialog*)pIActiveDialog, 
                                                                (int16) IDC_OPTIONS_MENU);
        if (pICurrentMenu)
        {
          Alarm  *pAlarm = NULL;

          SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
          //Clear out the options in order to figure out which ones to put up.
          (void)IMENUCTL_DeleteItem(pICurrentMenu, IDC_CONTROL_ALARM_TURN_ON);
          (void)IMENUCTL_DeleteItem(pICurrentMenu, IDC_CONTROL_ALARM_TURN_OFF);

          if (NULL != (pAlarm = (Alarm*)IVector_ElementAt(pMe->pAlarmList, pMe->wAlarmSelIdx)))
          {
            switch (pAlarm->state)
            {
              case ALARM_STATE_READY:
              case ALARM_STATE_EXPIRED:
                (void)IMENUCTL_AddItem(pICurrentMenu, gpAlarmClockResourceFile,
                                       IDS_STRING_TURN_ON, IDC_CONTROL_ALARM_TURN_ON,
                                       NULL, 0);
                break;

              case ALARM_STATE_RUNNING:
              case ALARM_STATE_SNOOZE:
                (void)IMENUCTL_AddItem(pICurrentMenu, gpAlarmClockResourceFile,
                                       IDS_STRING_TURN_OFF, IDC_CONTROL_ALARM_TURN_OFF,
                                       NULL, 0);
                break;

              case ALARM_STATE_INIT:
              case ALARM_STATE_INVALID:
              default:
                //Don't display anything except edit option.
                break;
            }
            nResult = SUCCESS;
          }
        }
      }
      break;

    default:
      ALARM_CLK_ERR("AlarmClock_RefreshDialog Unhandled Dialog ID %d", wDlgId, 0, 0);
      nResult = SUCCESS;
  }
  return nResult;
}

/*=============================================================================
FUNCTION: AlarmClock_AlertTimerCB

DESCRIPTION: Callback function for ISHELL_SetTimer().  Cancel IAlert after 
             timeout.

PARAMETERS:
   *pUser: AlarmClockApp object pointer

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AlarmClock_AlertTimerCB(void *pUser)
{
  AlarmClockApp *pMe = (AlarmClockApp*)pUser;

  if ((NULL == pMe) || (NULL == pMe->pIAlert))
  {
    ALARM_CLK_ERR("NULL PTR", 0, 0, 0);
    return;
  }

  (void) IALERT_StopAlerting(pMe->pIAlert);
}

/*=============================================================================
FUNCTION: AlarmClock_FormatAlertStr

DESCRIPTION: Format Alert String.  Will be formatted as follows:
             hh:mm[ap]m[seperator character]<alarm description>

PARAMETERS:
   *pMe:        AlarmClockApp object pointer
   *pAlarm:     Alarm struct pointer
   *pBuf:       Buffer to write string to.
    nBufLen:    pBuf length in bytes
    cSeperator: Seperating character in format string

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*lint -e{818} suppress could be const */
static void AlarmClock_FormatAlertStr(AlarmClockApp *pMe, Alarm *pAlarm, AECHAR *pBuf, 
                                      uint16 nBufSize, char cSeperator)
{
  AECHAR     szUsedFormatStr[ALARMCLOCK_FLD_LEN+1];
  JulianType julianDate;
  uint16     wHour;

  if ((NULL == pMe) || (NULL == pAlarm) || (NULL == pBuf))
  {
    ALARM_CLK_ERR("NULL PTR", 0, 0, 0);
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                              gpAlarmClockResourceFile,
                              IDS_STRING_ALARM_FMT,
                              szUsedFormatStr,
                              sizeof(szUsedFormatStr));
  GETJULIANDATE(pAlarm->uExpirationTime, &julianDate);

  wHour = (julianDate.wHour > 12)?(julianDate.wHour-12):(julianDate.wHour);
  if (wHour == 0)
  {
    //Display correct time for 12:00am hour
    wHour = 12;
  }
  WSPRINTF(pBuf,
           nBufSize,
           szUsedFormatStr,
           wHour,
           julianDate.wMinute,
           (julianDate.wHour > 12)?'p':'a',
           cSeperator);

  //WSPRINTF does not properly handle %S, risk of buffer overrun.
  WSTRLCAT(pBuf, pAlarm->pDescription, nBufSize/sizeof(AECHAR));
}

#if 0   //Snooze infrastructure is in place, but is not fully implemented
/*=============================================================================
FUNCTION: AlarmClock_CanSnooze

DESCRIPTION: Check if the specified alarm can be put in snooze mode.
 
PARAMETERS:
   *pMe      [in]:  AlarmClockApp object
   *pAlarm   [in]:  Alarm struct

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:


=============================================================================*/
static boolean AlarmClock_CanSnooze(AlarmClockApp *pMe, Alarm *pAlarm) 
{
  boolean bCanSnooze = FALSE;

  if (pMe->uSnoozeDuration > 0)
  {
    // repeating alarm
    if (AlarmClock_IsRepeatingAlarm(pAlarm))
    {
      if ((ALARM_STATE_RUNNING == pAlarm->state || 
           ALARM_STATE_EXPIRED == pAlarm->state) && 
          (GETTIMESECONDS() + pMe->uSnoozeDuration  < 
           pAlarm->uExpirationTime))
      {
        bCanSnooze = TRUE;
      }
      // one-time alarm
    } 
    else if (ALARM_STATE_EXPIRED == pAlarm->state)
    {
      bCanSnooze = TRUE;
    }
  }

  return bCanSnooze;
}
#endif

#endif // MIN_BREW_VERSIONEx(3,1,4)
