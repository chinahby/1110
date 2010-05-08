/*===========================================================================

FILE: CoreStartApp.c

DESCRIPTION
   This file contains the functions required to start the CoreApp.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:



       Copyright © 2002-2007 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/core/CoreStartApp.c#31 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
04/06/07   YZ     Removing launch of WMSAPP, which now launches itself
04/06/07   jas    Featurize launch of catapp to help legacy catapp versions
02/21/07   sun    Removing launch of Catapp, which now launches itself
01/15/07   jas    Removing launch of FieldDebug, which now launches itself
03/27/06   ng     Featurized VT app
9/16/05    ak     Make GPSApp backgrounded for all targets
12/01/04   jas    Removed duplicate startup of Core App
09/16/04   PA     Changed send event to post event for starting Apps
09/07/04   jas    Start apps one at a time, rather than queueing multiple reqs
07/22/04   sun    Made BVluetooth app background
05/27/04   sbs    Added LCAApp
03/18/04   sun    Backed out GPSAPP
02/04/04   is      Background gpsApp support
12/09/03   ram    Removed lint errors.
11/20/03   kar    Start coreapp on EVT_APP_RESUME
11/17/03   BPW    Do nothing on EVT_APP_RESUME
11/17/03   kar    BREW 3.0 changes
11/05/03   NNG    Added support for VT.
08/29/03   BW     Added support for Dialer App
03/04/03    lz    Removed starting MMS App.
02/25/03   tml    Fixed start args for Cat app
02/23/03   kar    Fixed start args for WMS app
02/18/03   AP     Added support for FEATURE_WMS_APP.
01/29/03    lz    Replaced ISHELL_StartApplet with ISHELL_StartAppletArgs
                  for MMS App.
01/28/03   tml    Added CATApp related feature
01/10/03   kar    Filled missing element in mod info
08/16/02   lz     Initial version
===========================================================================*/

#include "comdef.h"
#include "AEE.h"
#include "msg.h"
#include "CoreStartApp.h"
#ifdef FEATURE_APP_DIALER
#include "DialerApp.h"
#endif
#include "CoreApp.h"
#ifdef FEATURE_CORE_START_BG_APPS
#ifdef FEATURE_APP_CATAPP
#include "catapp.bid"
#endif
#endif /* FEATURE_CORE_START_BG_APPS */
#ifdef FEATURE_QVPHONE_APP
#include "QVIDEOPHONE.BID"
#endif /* FEATURE_QVPHONE_APP */
#ifdef FEATURE_MMGPS
#include "lcaapp.bid"
#include "lcaapp.h"
#endif

#ifdef FEATURE_GPSAPP
#include "gpsApp.bid"
#endif

#ifdef FEATURE_APP_BLUETOOTH
#include "BTApp.h"
#endif
//lint -save -e740

/*===========================================================================

                      DEFINITIONS

===========================================================================*/
/* The following macros can be used to populate the global structure
 * that is used to initialize the background applets.  Parameters are
 * as follows:
 *  pme:       A pointer to the CCoreStartApp structure.  Note that the
 *             macros increment the m_nNumAppsToStart member.
 *  cls:       The AEECLSID of the applet to be started
 *  pszArgs:   For applets started with args, the arg string to be passed
 *             to the applet upon startup
 *  evt:       For applets started with an event, the event ID to be sent
 *  wp:        For applets started with an event, the 16-bit param to be sent
 *  dwp:       For applets started with an event, the 32-bit param to be sent
 */
#define CORESTARTAPP_STARTAPPSIMPLE(pme,cls) \
    { \
        if ((pme)->m_nNumAppsToStart < CORESTARTAPP_NUM_APP_STARTS) \
        { \
          (pme)->m_AppStartInfo[(pme)->m_nNumAppsToStart].ClassId = (cls); \
          (pme)->m_AppStartInfo[(pme)->m_nNumAppsToStart].StartAction = \
                                                     COREAPP_START_SIMPLE; \
          ((pme)->m_nNumAppsToStart)++; \
        } \
    }
#define CORESTARTAPP_STARTAPPARGS(pme,cls,pszArgs) \
    { \
        if ((pme)->m_nNumAppsToStart < CORESTARTAPP_NUM_APP_STARTS) \
        { \
          (pme)->m_AppStartInfo[(pme)->m_nNumAppsToStart].ClassId = (cls); \
          (pme)->m_AppStartInfo[(pme)->m_nNumAppsToStart].StartAction = \
                                                  COREAPP_START_WITH_ARGS; \
          (pme)->m_AppStartInfo[(pme)->m_nNumAppsToStart].StartData.sArgs.pszStartArgs = (pszArgs); \
          ((pme)->m_nNumAppsToStart)++; \
        } \
    }
#define CORESTARTAPP_POSTEVENT(pme,cls,evt,wp,dwp) \
    { \
        if ((pme)->m_nNumAppsToStart < CORESTARTAPP_NUM_APP_STARTS) \
        { \
          (pme)->m_AppStartInfo[(pme)->m_nNumAppsToStart].ClassId = (cls); \
          (pme)->m_AppStartInfo[(pme)->m_nNumAppsToStart].StartAction = \
                                                    COREAPP_POST_EVENT; \
          (pme)->m_AppStartInfo[(pme)->m_nNumAppsToStart].StartData.sEvent.EventId = (evt); \
          (pme)->m_AppStartInfo[(pme)->m_nNumAppsToStart].StartData.sEvent.wParam = (wp); \
          (pme)->m_AppStartInfo[(pme)->m_nNumAppsToStart].StartData.sEvent.dwParam = (dwp); \
          ((pme)->m_nNumAppsToStart)++; \
        } \
    }


/*===========================================================================

                      STATIC DATA

===========================================================================*/

#ifdef AEE_STATIC
static const AEEAppInfo gaiCoreStartApp = {AEECLSID_CORE_STARTAPP, 0, 0, 0,
                                            0, 0, 0, AFLAG_STATIC | AFLAG_HIDDEN};
#endif /* AEE_STATIC */

/* This flag indicates whether Core App has already been started or not.
 * It is used in case Core Start App gets a resume and tries to start
 * Core App while we're still doing the initial background app startup
 * and before Core App has been started the first time.
 */
boolean                       gbAllAppsStarted = FALSE;

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION StartOneAppCB

DESCRIPTION
  A callback function that starts one background app in the list of apps to
  starts and then schedules itself as a callback again to start the next app in
  the list.

PROTOTYPE:
   void StartOneAppCB(void *pUser);

PARAMETERS:
   void *pUser:   Pointer to the callback data, which is a
                  CCoreStartApp_StartCBData structure.

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None

===========================================================================*/
static void StartOneAppCB(void *pUser)
{
    CCoreStartApp        *pMe = (CCoreStartApp *)pUser;
    AEECLSID              ClassId;        // Class id of the app to start

    if (! pMe)
    {
        MSG_ERROR("Bad data received in start app callback.", 0, 0, 0);
        return;
    }

    if (pMe->m_nNextAppToStart >= pMe->m_nNumAppsToStart)
    {
        // If we've reached or passed the end of the list, stop.
        MSG_ERROR("Start app callback invoked past the end of the list.",
                  0, 0, 0);
        gbAllAppsStarted = TRUE;
        return;
    }

    ClassId = pMe->m_AppStartInfo[pMe->m_nNextAppToStart].ClassId;
    switch (pMe->m_AppStartInfo[pMe->m_nNextAppToStart].StartAction)
    {
        case COREAPP_START_SIMPLE:
        {
            int                    nStartSuccess;

            nStartSuccess = ISHELL_StartApplet(pMe->a.m_pIShell, ClassId);
            if (AEE_SUCCESS != nStartSuccess)
            {
                MSG_ERROR("Unable to do simple start on app 0x%x, error code 0x%x.",
                          ClassId, nStartSuccess, 0);
            }
            break;
        }

        case COREAPP_START_WITH_ARGS:
        {
            int                         nStartSuccess;
            CCoreStartApp_AppStartArgs *pArgs =
                &(pMe->m_AppStartInfo[pMe->m_nNextAppToStart].StartData.sArgs);

            nStartSuccess = ISHELL_StartAppletArgs(pMe->a.m_pIShell, ClassId,
                                                   pArgs->pszStartArgs);
            if (AEE_SUCCESS != nStartSuccess)
            {
                MSG_ERROR("Unable to do start with args on app 0x%x, error code 0x%x .",
                          ClassId, nStartSuccess, 0);
            }
            break;
        }

        case COREAPP_POST_EVENT:
        {
            boolean                      bStartSuccess;
            CCoreStartApp_AppStartEvent *pEventInfo =
               &(pMe->m_AppStartInfo[pMe->m_nNextAppToStart].StartData.sEvent);

            bStartSuccess = ISHELL_PostEvent(pMe->a.m_pIShell, ClassId,
                                             pEventInfo->EventId,
                                             pEventInfo->wParam,
                                             pEventInfo->dwParam);
            if (! bStartSuccess)
            {
                MSG_ERROR("Unable to do \"post event\" start on app 0x%x, event id %d.",
                          ClassId, pEventInfo->EventId, 0);
            }
            break;
        }

        default:
            break;

    }

    // Initialize the callback to start the next applet, if there are any more
    pMe->m_nNextAppToStart++;
    if (pMe->m_nNextAppToStart < pMe->m_nNumAppsToStart)
    {
        /* The callback data is identical, so no need to re-init the
         * callback structure.  Just schedule it...
         */
        ISHELL_Resume(pMe->a.m_pIShell, &(pMe->m_sStartCB));
    }
    else
    {
        gbAllAppsStarted = TRUE;
    }

    return;

}

/*===========================================================================
FUNCTION STARTALLAPPS

DESCRIPTION
  Starts all background apps

DEPENDENCIES
  None

SIDE EFFECTS
  None

COMMENTS
  The global array supports a limited number of entries for applets to start.
  The size of the array is CORESTARTAPP_NUM_APP_STARTS.  If more applets are
  going to be started, that value must be increased.  There's nothing magic
  about its value, so it can be increased as needed (within reason).

===========================================================================*/
static void StartAllApps(AEEApplet *pMe)
{
  CCoreStartApp *pCoreStartApp = (CCoreStartApp *)pMe;

  if (! pCoreStartApp)
  {
    MSG_ERROR("NULL Applet pointer. Unable to start apps.", 0, 0, 0);
    return;
  }

  pCoreStartApp->m_nNumAppsToStart = 0;

//lint -save -e774   Suppress complaint that boolean expr is always TRUE
#ifdef FEATURE_CORE_START_BG_APPS
#ifdef FEATURE_APP_CATAPP
  CORESTARTAPP_STARTAPPARGS(pCoreStartApp, AEECLSID_CATAPP,
                            CORE_NO_START_ARGS);
#endif
#endif /* FEATURE_CORE_START_BG_APPS */
#ifdef FEATURE_APP_DIALER
  CORESTARTAPP_STARTAPPARGS(pCoreStartApp, AEECLSID_DIALER,
                            CORE_NO_START_ARGS);
#endif /* FEATURE_APP_DIALER */
#ifdef FEATURE_QVPHONE_APP
  CORESTARTAPP_STARTAPPARGS(pCoreStartApp, AEECLSID_QVIDEOPHONE,
                            CORE_NO_START_ARGS);
#endif /* FEATURE_QVPHONE_APP */

#ifdef FEATURE_GPSAPP
  CORESTARTAPP_STARTAPPARGS(pCoreStartApp, AEECLSID_GPSONE,
                            CORE_NO_START_ARGS);
#endif /* FEATURE_GPSAPP */

#ifdef FEATURE_MMGPS
  CORESTARTAPP_POSTEVENT(pCoreStartApp, AEECLSID_LCAAPP,
                         EVT_LCAAPP_START_BACKGROUND, 0, 0);
#endif /* FEATURE_MMGPS */

#if defined(FEATURE_APP_BLUETOOTH) && defined(EVT_FIRST_LAUNCH)
  CORESTARTAPP_POSTEVENT(pCoreStartApp, AEECLSID_BLUETOOTH_APP,
                         EVT_FIRST_LAUNCH, 0, 0);
#endif /* FEATURE_APP_BLUETOOTH */

  CORESTARTAPP_STARTAPPSIMPLE(pCoreStartApp, AEECLSID_CORE_APP);

  if (pCoreStartApp->m_nNumAppsToStart >= CORESTARTAPP_NUM_APP_STARTS)
  {
    MSG_ERROR("Reached the limit of supported background apps. %d apps.",
              pCoreStartApp->m_nNumAppsToStart, 0, 0);
  }
  else if (pCoreStartApp->m_nNumAppsToStart > 0)
  {
    // Initialize the callback to start the first applet.
    pCoreStartApp->m_nNextAppToStart = 0;
    CALLBACK_Init(&(pCoreStartApp->m_sStartCB), StartOneAppCB,
                  pCoreStartApp);
    ISHELL_Resume(pCoreStartApp->a.m_pIShell, &(pCoreStartApp->m_sStartCB));
  }

//lint -restore -e774   Restore complaint that boolean expr is always TRUE

  return;
} /* End StartAllApps */

/*===========================================================================
FUNCTION STARTCOREAPP

DESCRIPTION
  Starts the CoreApp using ISHELL_StartApplet().

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void StartCore(AEEApplet *pMe)
{
  ISHELL_StartApplet(pMe->m_pIShell, AEECLSID_CORE_APP);
  return;
} /* End StartCore */

/*===========================================================================

FUNCTION CoreStartApp_HandleEvent

DESCRIPTION
   This is the EventHandler for this app. All events to this app are handled in this
   function. All APPs must supply an Event Handler.

   Note - The switch statement in the routine is to demonstrate how event handlers are
   generally structured.  However, realizing the simplicity of the example, the code could
   have been reduced as follows:

   if(eCode == EVT_APP_START){
      IDISPLAY_DrawText();
      IDISPLAY_Update();
      return(TRUE);
   }
   return(FALSE);

   However, while doing so would have demonstrated how BREW apps can be written in about 8
   lines of code (including the app creation function), it might have confused those who wanted
   a bit more practical example.

   Also note that the use of "szText" below is provided only for demonstration purposes.  As
   indicated in the documentation, a more practical approach is to load text resources
   from the applicaton's resource file.

   Finally, note that the ONLY event that an applet must process is EVT_APP_START.  Failure
   to return TRUE from this event will indicate that the app cannot be started and BREW
   will close the applet.

PROTOTYPE:
   boolean CoreStartApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

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
static boolean CoreStartApp_HandleEvent(AEEApplet * pMe, AEEEvent eCode,
                                        uint16 wParam, uint32 dwParam)
/*lint -esym(715, wParam)*/
/*lint -esym(715, dwParam)*/
{
  ASSERT(pMe != NULL);

  switch (eCode) {
    case EVT_APP_START:
      /* Start the Core App */
      ISHELL_SetTimer(pMe->m_pIShell,
                      100,
                     (PFNNOTIFY) StartAllApps,
                     (uint32*) pMe);
      return TRUE;

    case EVT_APP_STOP:
      return TRUE;

    case EVT_APP_SUSPEND:
      return TRUE;

    case EVT_APP_RESUME:
      /* Only "restart" the Core App if we've already started all the
       * background apps.  Otherwise, Core App will just start normally at
       * the end of background app init.
       */
      if (gbAllAppsStarted)
      {
        ISHELL_SetTimer(pMe->m_pIShell, 100, (PFNNOTIFY) StartCore,
                        (uint32*) pMe);
      }
      return TRUE;

    default:
       return FALSE;
  }
} /* End CoreStartApp_HandleEvent */

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
  int AEEAppCreateInstance(AEECLSID clsID, IShell* pIShell, IModule* pIModule,IApplet** ppApplet)

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

/* Create the core start app (dummy app) */
#if defined(AEE_STATIC)
int CoreStartApp_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
#endif
{
   *ppObj = NULL;

   if(AEEApplet_New( sizeof(CCoreStartApp),        /* Size of private class */
                     ClsId,                        /* Our class ID */
                     pIShell,                      /* Shell interface */
                     pMod,                         /* Module instance */
                     (IApplet**)ppObj,             /* Return object */
                     (AEEHANDLER)CoreStartApp_HandleEvent, /* Event handler */
                     NULL))                        /* Cleanup function */
   {
     return(AEE_SUCCESS);
   }
  return (EFAILED);
} /* End CoreStartApp_CreateInstance / AEEClsCreateInstance */

#ifdef AEE_STATIC
/*=============================================================================
FUNCTION: COREStartAPP_LOAD

DESCRIPTION:
    This function loads the application.

PROTOTYPE:
static int CoreStartApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)

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
int CoreStartApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,
          CoreStartApp_CreateInstance,NULL));
} /* End CoreStartApp_Load */

/*=============================================================================
FUNCTION: CORESTARTAPP_GETMODINFO

DESCRIPTION:
    Gets the CoreStartApp Info since it is a static application.

PROTOTYPE:
    PFNMODENTRY CoreStartApp_GetModInfo(IShell * ps, AEECLSID ** ppClasses,
                                        AEEAppInfo ** pApps, uint16 * pnApps,
                                        uint16 * pwMinPriv)

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
PFNMODENTRY CoreStartApp_GetModInfo(IShell * ps, AEECLSID ** ppClasses,
                                    AEEAppInfo ** pApps,
                                    uint16 * pnApps,uint16 * pwMinPriv)
/*lint -esym(715, ps)*/
/*lint -esym(715, ppClasses)*/
/*lint -esym(715, pwMinPriv)*/
{
   *pApps = (AEEAppInfo *)&gaiCoreStartApp;
   *pnApps = 1;
   return((PFNMODENTRY)CoreStartApp_Load);
} /* End CoreStartApp_GetModInfo */
#endif /* AEE_STATIC */
//lint -restore
