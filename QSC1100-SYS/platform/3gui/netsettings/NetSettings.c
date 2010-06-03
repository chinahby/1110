/*=============================================================================

FILE: NetSettings.c

SERVICES: Network settings menus

GENERAL DESCRIPTION:
Creates and handles all network related menus

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:
None.

(c) COPYRIGHT 2002 - 2006 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/NetSettings/NetSettings.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/06   RT      Added support to increase font size for VGA displays
09/14/06   jas     Fixing an incorrect use of NET_FATAL
09/08/06   jas     Fixing case-sensitivity in include file for L4
06/19/06   joe     Initial Revision

===========================================================================*/

/*===========================================================================

                      INCLUDES

===========================================================================*/
#include "OEMFeatures.h"
#include "comdef.h"

#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEModTable.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEEClipboard.h"

#include "AppComFunc.h"
#include "NetSettings.h"
#include "NetMenu.h"
#include "netsettings.brh"
#include "NetSettings.bid"
#include "NetUtils.h"
/*===========================================================================

                      Constant Definitions

===========================================================================*/
/* Dialog Event Handler function pointer structure */
typedef boolean (*PFNDLGHANDLER) (
                                 void *pUser,
                                 AEEEvent evt,
                                 uint16 w,
                                 uint32 dw );

char *gpNetResourceFile = NETSETTINGS_RES_FILE;


/*===========================================================================

                      STATIC DATA

===========================================================================*/

static const PFNDLGHANDLER NetDlgEventHandlers[] = {
  NULL,
  NetRadioButtonDlgEventHandler,              /* 01 IDD_NETWORK_DATA_FAX */
  NetRadioButtonDlgEventHandler,              /* 02 IDD_NETWORK_SET_NAM */
  NetRadioButtonDlgEventHandler,              /* 03 IDD_NETWORK_AUTO_NAM */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  ForceModeSlotRadioButtonDlgEventHandler,    /* 04 IDD_NETWORK_FORCE_MODE */
#else
  NetRadioButtonDlgEventHandler,              /* 04 IDD_NETWORK_FORCE_MODE */
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
  NetRadioButtonDlgEventHandler,              /* 05 IDD_NETWORK_SELECT */
  NetRadioButtonDlgEventHandler,              /* 06 IDD_NETWORK_HYBR_PREF */
  NetRadioButtonDlgEventHandler,              /* 07 IDD_NETWORK_ACQ_PREF */
  NetRadioButtonDlgEventHandler,              /* 08 IDD_NETWORK_DOMAIN_PREF */
  NetRadioButtonDlgEventHandler,              /* 09 IDD_NETWORK_ROAM_PREF */
  NetCheckBoxDlgEventHandler,                 /* 10 IDD_NETWORK_BAND_PREF */
  NetMenuDlgEventHandler,                     /* 11 IDD_NETWORK */
  NetMenuDlgEventHandler,                     /* 12 IDD_NETWORK_M51 */
  NetRadioButtonDlgEventHandler,              /* 13 IDD_NETWORK_M511 */
  NetRadioButtonDlgEventHandler,              /* 14 IDD_NETWORK_M512 */
  NetRadioButtonDlgEventHandler,              /* 15 IDD_NETWORK_M513 */
  NetStaticDlgEventHandler,                   /* 16 IDD_NETWORK_AVOID_SERVING_SYS*/
  NetStaticDlgEventHandler,                   /* 17 IDD_NETWORK_WAKEUP_1X*/
  NetStaticDlgEventHandler,                   /* 18 IDD_NETWORK_WAKEUP_HDR*/
  NetMessageDlgEventHandler                   /* 19 IDD_GENERAL_MESSAGE */
}; /* End DlgEventHandlers[] */

#ifdef AEE_STATIC
static AEEAppInfo gaiNetSettings = {AEECLSID_NETSETTINGS,
                    NETSETTINGS_RES_FILE,
                    IDS_NETAPP_TITLE,
                    0,0,0,0 , AFLAG_PHONE | AFLAG_STATIC | AFLAG_HIDDEN};
#endif /* AEE_STATIC */

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION NetSettings_FreeAppData

DESCRIPTION
  Release the application data

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
void NetSettings_FreeAppData(IApplet* po)
{
  CNetApp *pMe = (CNetApp*)po;
  ASSERT( pMe != NULL );   /* error checking */

  if (pMe->m_pIConfig) {
    ICONFIG_Release(pMe->m_pIConfig);
    pMe->m_pIConfig = NULL;
  }

  /* ICM */
  if (pMe->m_pICM) {
    ISHELL_RegisterNotify(pMe->a.m_pIShell,
      AEECLSID_NETSETTINGS,
      AEECLSID_CM_NOTIFIER,
      0 );

    ICM_Release(pMe->m_pICM);
    pMe->m_pICM = NULL;
  }

} /* End NetSettings_FreeAppData */

/*===========================================================================
FUNCTION NetSettings_InitAppData

DESCRIPTION
  Initializing the application data

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSettings_InitAppData(IApplet* po)
{
  CNetApp *pMe = (CNetApp*)po;
  int nReturnStatus;

  /* Create the IConfig Serivce object. */
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                               AEECLSID_CONFIG,
                               (void **) &pMe->m_pIConfig);

  if(CheckAEEReturnStatus(nReturnStatus) == FALSE || pMe->m_pIConfig == NULL)
  {
    NET_ERR("Failed to create IConfig, %d", nReturnStatus, 0, 0);
    pMe->m_pIConfig = NULL;
  }

  /* Create the Call Manager object. */
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                               AEECLSID_CM,
                               (void **) &pMe->m_pICM);

  if(nReturnStatus != SUCCESS) {
    (void)CheckAEEReturnStatus(nReturnStatus); /* error checking */
    NetSettings_FreeAppData((IApplet*)pMe);
    return FALSE;
  }

  //This will increase the font size on VGA displays
  AppComFunc_SelectFonts(pMe->a.m_pIDisplay);
  return TRUE;

} /* End NetSettings_InitAppData */

/*===========================================================================

FUNCTION NetSettings_HandleEvent

DESCRIPTION
   This is the EventHandler for this app. All events to this app are handled in this
   function. All APPs must supply an Event Handler.

   NetSettings creates a menu dialogue in the EVT_APP_START case, and therefore this handler
   is only the active event handler during NetSettings startup.
   
PROTOTYPE:
   boolean NetSettings_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

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
static boolean NetSettings_HandleEvent(CNetApp * pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  ASSERT( pMe != NULL ); /* error checking */

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (eCode)
  {
    case EVT_APP_START:
    {
      // Start the network dialog
      NET_MSG ("Got EVT_APP_START", 0, 0, 0);
      if(SUCCESS != ISHELL_CreateDialog(pMe->a.m_pIShell, gpNetResourceFile,
                                        IDD_NETWORK, NULL))
      {
        NET_ERR("Unable to create Dialog", 0, 0, 0);
        return FALSE;
      }
      return TRUE;
    }
    case EVT_DIALOG_INIT:
    {
      NET_MSG ("Got EVT_DIALOG_INIT", 0, 0, 0);
      NET_MSG ("Creating Dialog %d", wParam, 0, 0);

      if(wParam >= ARR_SIZE(NetDlgEventHandlers))
        ERR_FATAL("Index out of bounds in NetSettings_HandleEvent",0,0,0);

      if(wParam < ARR_SIZE(NetDlgEventHandlers))
      { /* check for array bound */
        if (NetDlgEventHandlers[wParam] != NULL)
        {
          IDIALOG_SetEventHandler((IDialog *) dwParam,
                                  NetDlgEventHandlers[wParam], pMe);
          IDIALOG_SetProperties((IDialog *) dwParam, DLG_HANDLE_ALL_EVENTS );
          
          return TRUE;
        }
      }
    }
    case EVT_APP_STOP:
    {
      NET_MSG ("Got EVT_APP_STOP", 0, 0, 0);
      return TRUE;
    }
    case EVT_APP_SUSPEND:
    {
      NET_MSG ("Got EVT_APP_SUSPEND", 0, 0, 0);
      return TRUE;
    }
    case EVT_APP_RESUME:
    {
      NET_MSG ("Got EVT_APP_RESUME", 0, 0, 0);
      return TRUE;
    }
    case EVT_COMMAND:
    {
      NET_MSG ("Got EVT_COMMAND", 0, 0, 0);
      return TRUE;
    }
    case EVT_NOTIFY:
    {
      NET_MSG ("Got EVT_NOTIFY", 0, 0, 0);
      return TRUE;
    }
    case EVT_USER:
    {
      NET_MSG ("Got EVT_USER", 0, 0, 0);
      return TRUE;
    }
    case EVT_KEY:
    {
      NET_MSG ("Got EVT_KEY", 0, 0, 0);
      return TRUE;
    }
    default:
    {
      break;
    }
  }
  
  return FALSE;
  
}  /* End NetSettings_HandleEvent */



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
#if defined(AEE_STATIC)
int NetSettings_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
#endif
{
   *ppObj = NULL;

   if(AEEApplet_New( sizeof(CNetApp),                    /* Size of our private class */
                     ClsId,                               /* Our class ID */
                     pIShell,                             /* Shell interface */
                     pMod,                                /* Module instance */
                     (IApplet**)ppObj,                    /* Return object */
                     (AEEHANDLER)NetSettings_HandleEvent,     /* Our event handler */
                     (PFNFREEAPPDATA)NetSettings_FreeAppData))/* Cleanup function */
   {
     if(NetSettings_InitAppData((IApplet*)*ppObj)) {
       return(AEE_SUCCESS);
     }
     //Release the applet. This will free the memory allocated for the applet when
     IAPPLET_Release(((IApplet*)*ppObj));
   }
  return (EFAILED);
} /* End NetSettings_CreateInstance / AEEClsCreateInstance */



#ifdef AEE_STATIC
/*=============================================================================
FUNCTION: NetSettings_Load

DESCRIPTION:
    This function loads the application.

PROTOTYPE:
static int NetSettings_Load(IShell *ps, void * pHelpers, IModule ** pMod)

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
int NetSettings_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,
          NetSettings_CreateInstance,NULL));
} /* End NetSettings_Load */



/*=============================================================================
FUNCTION: NetSettings_GetModInfo

DESCRIPTION:
    Gets the NetSettings Info since it is a static application.

PROTOTYPE:
    PFNMODENTRY NetSettings_GetModInfo(IShell * ps, AEECLSID ** ppClasses,
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
PFNMODENTRY NetSettings_GetModInfo(IShell * ps, AEECLSID ** ppClasses,
                               AEEAppInfo ** pApps,
                               uint16 * pnApps,uint16 * pwMinPriv)
/*lint -esym(715,ps)*/
/*lint -esym(715,ppClasses)*/
{
   *pApps = (AEEAppInfo *)&gaiNetSettings;
   *pnApps = 1;
   *pwMinPriv = PL_SYSTEM;   /* NetSettings has system priviliges */
   return((PFNMODENTRY)NetSettings_Load);
} /* End NetSettings_GetModInfo */

//lint -restore
#endif  /* AEE_STATIC */
