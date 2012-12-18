/*===========================================================================
                              OEMAppFuncs.c

  OEM REFERENCE APPLICATION CONTROL ROUTINES

  This file contains a reference implementation of a standard AEE application
  control interfaces.

        Copyright ? 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "OEMFeatures.h"


#if defined(OEMAPPFUNCS)

#include "err.h"
#include "OEMConfig.h"
#include "OEMNotify.h"
#include "OEMSVC.h"
#include "OEMCFGI.h"

#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEE_OEMDownload.h"
#include "AEEDownload.h"
#include "AEEStdLib.h"


static AEECallback   gCBResetBREW;
static boolean       gbInResetBREW = FALSE;
#include "OEMVersion.h"

#ifdef FEATURE_OEMOMH
extern void OEMOMH_SetProfile(AEECLSID cls, AEEEvent evt);
#endif

#ifdef CUST_EDITION
#include "OEMClassIDs.h"
#else
#ifdef FEATURE_UI_CORE
#include "CoreApp.BID"
#endif /* FEATURE_UI_CORE */

#ifndef FEATURE_BREW_LITE
#include "AEERUIM.h"       // Sample RUIM support...
#include "AEEDownload.h"
#ifdef FEATURE_PROVISION
#include "OEMProvisionItems.h"
#endif // FEATURE_PROVISION
#endif // FEATURE_BREW_LITE

#ifdef FEATURE_DEBUG_EVENT
#include "AEEDebugEvent.h"
#endif
#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
#include "clkregim.h"
#endif

#ifdef FEATURE_UI_CORE
#ifdef FEATURE_WMS_APP
#include "WMSAPP.BID"
#endif
#ifdef FEATURE_APP_CONTACT
#include "ContApp.h"
#endif
#ifdef FEATURE_APP_RECENTCALLS
#include "recentcalls.bid"
#endif
#ifdef FEATURE_APP_DIALER
#include "DialerApp.bid"
#endif
#ifdef FEATURE_APP_DATASTATS
#include "DATASTATSAPP.BID"
#endif
#ifdef FEATURE_APP_BLUETOOTH
#include "BTApp.bid"
#endif
#ifdef FEATURE_APP_CATAPP
#include "catapp.bid"
#endif
#ifdef FEATURE_APP_FLDDBG
//#include "FLDDBGAPP.BID"
#endif
#ifdef FEATURE_GRAPHICS_APP_GIGAMON
#include "gigamon.bid"
#endif
#ifdef FEATURE_GPSAPP
#include "gpsApp.bid"
#endif
#ifdef FEATURE_APP_GUNIMON
#include "gunimon.bid"
#endif
#ifdef FEATURE_APP_MEDIAPLAYER
#include "MediaPlayer.bid"
#endif
#ifdef FEATURE_APP_MMS
#include "MMSApp.BID"
#endif
#ifdef FEATURE_APP_WAP
#include "WAPApp.BID"
#endif
#ifdef FEATURE_APP_MOBIMON
#include "Mobimon.bid"
#endif
#ifdef FEATURE_APP_PUREVOICE
#include "purevoice.h"
#endif
#ifdef FEATURE_APP_QCAMCORDER
#include "Qcamcorder.bid"
#endif
#if defined(FEATURE_APP_QCAMERA)
#include "qcamera.bid"
#if defined(FEATURE_APP_QALBUM)
#include "qalbum.bid"
#endif // FEATURE_APP_QALBUM
#endif
#ifdef FEATURE_APP_RECENTCALLS
#include "recentcalls.bid"
#endif
#ifdef FEATURE_APP_SVCPRG
//#include "SVCPRGAPP.BID"
#endif
#ifdef FEATURE_WMS_APP
#include "WMSAPP.BID"
#endif
#ifdef FEATURE_MMGPS
#include "lcaapp.bid"
#endif

#ifdef FEATURE_QVPHONE_APP
#include "QVIDEOPHONE.BID"
#endif

#ifdef FEATURE_SECMSM_APPS
#include "mmcapp.BID"
#include "mystufff.bid"
#include "PREPROVISION.BID"
#include "RightsMgr.bid"
#include "drmapp.bid"
#endif /* FEATURE_SECMSM_APPS */

#ifdef FEATURE_AUXSETTINGS_APP
#include "AUXSettings.bid"
#endif /*FEATURE_AUXSETTINGS_APP*/

#ifdef FEATURE_NETSETTINGS_APP
#include "NetSettings.bid"
#endif /*FEATURE_NETSETTINGS_APP*/
#endif

#ifdef FEATURE_IMS
#ifdef FEATURE_IMS_ENABLERPRESENCE
#include "IMS_PRESENCEMANAGER.BID"
#endif
#ifdef FEATURE_IMS_VIDEOSHARE
#include "IMS_VIDEOSHARE.BID"
#endif //FEATURE_IMS_VIDEOSHARE
#include "IMS_SETTINGS.BID"
#include "IMS_REGMANAGER.BID"
#endif //FEATURE_IMS

#ifdef FEATURE_UIONE_HDK
#include "TestApp_SafeMode.h"
#include "PhoneApp.bid"
#endif

#ifdef FEATURE_APP_PBPRINT
#include "pbprint.bid"
#endif
#endif
#ifdef FEATURE_APP_FLDDBG
//#include "FLDDBGAPP.BID"
#endif

#ifdef FEATURE_APP_SVCPRG
//#include "SVCPRGAPP.BID"
#endif

#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
#include "clkregim.h"
static boolean registered_for_cpu = TRUE; /* registered for high CPU ? */
#endif

static AEECallback   gCBReset;
static boolean       gbReset = FALSE;
static AEECallback   gCBStartCoreOEMApp;

/*==================================================================
Function: OEM_AuthorizeDownload

Description:

 This function is called before each download.  The OEM must fire the callback before the IDOWNLOAD_Acquire
 operation will be completed.  The OEM can use the IDownload instance class instance and the item/price IDs
 to query information about the item if desired.  In addition, the IDownload pointer can be used to
 set the HTTP headers sent to the ADS.  This is performed via the IDOWNLOAD_SetHeaders method.

 pUser - User data to be passed to the callback.
 iID - Item ID
 iPrice - Item Price Handle
 pItem - Pointer to information structure for the item
 nErr - Error code.  If non-zero, the download will abort and this error will be reported.

Prototype:
   void OEM_AuthorizeDownload(IDownload * pd, DLITEMID iID, DLPRICEID iPrice,PFNCHECKDLCB pfn, void * pUser);

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_AuthorizeDownload(IDownload * pd, DLITEMID iID, DLPRICEID iPrice,DLItem * pItem,PFNCHECKDLCB pfn, void * pUser)
{
   pfn(pUser,iID,iPrice,0);
}


/*==================================================================
Function: OEM_SimpleBeep

Description:
   This function plays a standard OEM beep tone and vibration given
the duration, and returns TRUE if successful.

Prototype:
   boolean OEM_SimpleBeep(BeepType nBeepType, boolean bLoud);

Parameters:
   nBeepType: beep type which can be one of the following:
      - BEEP_OFF: stop playback of the current beep or vibration
      - BEEP_ALERT: alert beep tone
      - BEEP_REMINDER: reminder beep tone
      - BEEP_MSG: message beep tone
      - BEEP_ERROR: error beep tone
      - BEEP_VIBRATE_ALERT: alert vibration
      - BEEP_VIBRATE_REMIND: reminder vibration
   bLoud: boolean flag that sets the playback volume high or low

Return Value:
   TRUE if successfuly played or stopped the tone or vibration;
   FALSE otherwise/unsupported.

Comments:
   None

Side Effects:
   None

See Also:
   None
==================================================================*/
boolean OEM_SimpleBeep(BeepType nBeepType, boolean bLoud)
{
   return OEM_SVCSimpleBeep(nBeepType, bLoud);
}

/*==============================================================
==============================================================*/
static void WakeReset(void * pUnused)
{
   AEEOS_ResetDevice(" Resetting...", 0);
}

/*==============================================================
Function: WakeCoreOEMApp

Description: Starts OEM's core BREW app that represents the idle state.
             This is useful when the device UI etc. are BREW apps.
==============================================================*/
static void WakeCoreOEMApp( void *pUnused )
{
   IShell *pShell = AEE_GetShell();
   if(NULL != pShell)
   {
#ifdef FEATURE_UIONE_HDK
      ISHELL_PostURL(pShell, "tel:Top");
#else
      ISHELL_StartApplet( pShell, AEECLSID_CORE_APP );
#endif // FEATURE_UIONE_HDK
   }
}

/*==============================================================
Exit and re-init BREW
==============================================================*/
static void ResetBREW(void * pUnused)
{
#ifdef CUST_EDITION
   WakeReset(pUnused);
#else
   //Set this bool. Calling AEE_Exit() will result in NTF_IDLE. We do not want AutoStartApp to
   //be started there.
   gbInResetBREW = TRUE;

   AEE_Exit();

   gbInResetBREW = FALSE;

   //AEE_Init will also take care of starting AutoStart App
   AEE_Init(0);
#endif
}

/*==============================================================
Function: OEM_Notify

Description: Notification to the device

==============================================================*/
int OEM_Notify(OEMNotifyEvent evt, uint32 dw)
{
   OEMAppEvent * pae = (OEMAppEvent *)dw;

#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
  boolean high_cpu_clk_needed = FALSE;
#endif

   switch(evt){
      case OEMNTF_MODLOAD:
      {
         OEMNotifyModLoad  *pML = (OEMNotifyModLoad *) dw;

         if ((void *)0 == pML->pLocation) {
            return EFAILED;
         }
         return SUCCESS;
      }

      case OEMNTF_IDLE:

         if(gbInResetBREW)
            break;

         if(gbReset){
            gbReset = FALSE;
            CALLBACK_Init(&gCBReset, WakeReset, NULL);
            AEE_ResumeCallback(&gCBReset,0);
         }
         else {
            CALLBACK_Init(&gCBStartCoreOEMApp, WakeCoreOEMApp, NULL);
            AEE_ResumeCallback(&gCBStartCoreOEMApp,0);
         }
         break;

      case OEMNTF_RESET_BREW:
         CALLBACK_Init(&gCBResetBREW, ResetBREW, NULL);
         AEE_ResumeCallback(&gCBResetBREW,NULL);
         break;

      case OEMNTF_RESET:
         CALLBACK_Init(&gCBReset, WakeReset, NULL);
         AEE_ResumeCallback(&gCBReset,0);
         break;
      case OEMNTF_APP_EVENT:
#ifdef FEATURE_OEMOMH
	  	 OEMOMH_SetProfile(pae->cls, pae->evt); 
#endif
         if( pae->evt == EVT_APP_START   ||
             pae->evt == EVT_APP_STOP    ||
             pae->evt == EVT_APP_SUSPEND ||
             pae->evt == EVT_APP_RESUME ){
            DBGEVENT( pae->evt, pae->cls );
         }

#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
         if( pae) {
           if (pae->evt == EVT_APP_START ||
               pae->evt == EVT_APP_RESUME )
           {
#ifdef FEATURE_UI_CORE
             if (pae->cls != AEECLSID_CORE_APP &&
#ifdef FEATURE_WMS_APP
                 pae->cls != AEECLSID_WMSAPP &&
#endif
#ifdef FEATURE_APP_CONTACT
                 pae->cls != AEECLSID_APP_CONTACT &&
#endif
#ifdef FEATURE_APP_RECENTCALLS
                 pae->cls != AEECLSID_RECENTCALLSAPP &&
#endif
#ifdef FEATURE_APP_DATASTATS
                 pae->cls != AEECLSID_DATASTATS &&
#endif
#ifdef FEATURE_APP_DIALER
                 pae->cls != AEECLSID_DIALER &&
#endif
#ifdef FEATURE_APP_MEDIAPLAYER
                 pae->cls != AEECLSID_MEDIAPLAYER &&
#endif
#ifdef FEATURE_APP_QCAMERA
                 pae->cls != AEECLSID_QCAMERA &&
   #ifdef FEATURE_APP_QALBUM
                 pae->cls != AEECLSID_QALBUM &&
   #endif
#endif
#ifdef FEATURE_APP_QCAMCORDER
                 pae->cls != AEECLSID_QCAMCORDER &&
#endif
#ifdef FEATURE_QVPHONE_APP
                 pae->cls != AEECLSID_QVIDEOPHONE &&
#endif

#ifdef FEATURE_APP_PBPRINT
                 pae->cls != AEECLSID_PBPRINT &&
#endif //FEATURE_APP_PBPRINT

#ifdef FEATURE_UIONE_HDK
                 pae->cls != AEECLSID_PHONEAPP &&
#endif

#ifndef CUST_EDITION
                 pae->cls != AEECLSID_CORE_STARTAPP&&
#endif                 
                 
                 1)
                
             {
               high_cpu_clk_needed = TRUE;
             }
             else
#endif /* FEATURE_UI_CORE */
             {
               high_cpu_clk_needed = FALSE;
             }

            if (high_cpu_clk_needed  && !registered_for_cpu)
            {
              clk_regime_register_for_cpu_resource(CLKRGM_NON_CDMA_APPS);
              registered_for_cpu = TRUE;
            }
            else if (!high_cpu_clk_needed && registered_for_cpu)
            {
              clk_regime_deregister_for_cpu_resource(CLKRGM_NON_CDMA_APPS);
              registered_for_cpu = FALSE;
            }
         }
       }
#endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */
         break;
      case OEMNTF_APP_CTXT_NEW:    // fall through
      case OEMNTF_APP_CTXT_DELETE: // fall through
         // OEM to handle the context notifications for OEM's app
         // context here.
         break;

      case OEMNTF_APP_CTXT_SWITCH:
         // OEM to handle the context notifications for OEM's app
         // context here - context switch between app.
         break;
      default:
         break;
   }
   return(0);
}

/*==============================================================
Function: OEM_IsClsOKInSafeMode

Description: The OEMs can maintain a list of class IDs for apps
             that they do not want to throttle when in Safe mode.
             For example, the OEM will not throttle the UI app.
             in devices where the UI is implemented using BREW.

==============================================================*/
boolean OEM_IsClsOKInSafeMode(uint32 clsid)
{
#ifdef FEATURE_UIONE_HDK
  {
     int result = TestApp_SafeMode_OEM_IsClsOKInSafeMode(clsid);

     if (result >= 0) {
        return (result > 0) ? TRUE : FALSE;
     }
  }
#endif

  switch(clsid)
  {
     case AEECLSID_APPMANAGER:
   #ifdef FEATURE_UI_CORE
     case AEECLSID_CORE_APP:
#ifndef CUST_EDITION
     case AEECLSID_CORE_STARTAPP:
#endif        
   #endif
#if 0

   #ifdef FEATURE_APP_BLUETOOTH
      case  AEECLSID_BLUETOOTH_APP:
   #endif
#endif

   #ifdef FEATURE_APP_CATAPP
     case AEECLSID_CATAPP:
   #endif

   #ifdef FEATURE_APP_CONTACT
     case AEECLSID_APP_CONTACT:
   #endif

   #ifdef FEATURE_APP_DATASTATS
    case AEECLSID_DATASTATS:
   #endif

#ifndef CUST_EDITION   
   #ifdef FEATURE_APP_DIALER
     case AEECLSID_DIALER:
   #endif

#endif
  
   #ifdef FEATURE_APP_FLDDBG
     case AEECLSID_FIELDDEBUGAPP:
   #endif

   #ifdef FEATURE_GRAPHICS_APP_GIGAMON
     case AEECLSID_GIGA_TEST_APP:
   #endif

   #ifdef FEATURE_GPSAPP
       case AEECLSID_GPSONE:
   #endif

   #ifdef FEATURE_APP_GUNIMON
       case AEECLSID_GUNIMON:
   #endif

   #ifdef FEATURE_APP_MEDIAPLAYER
         case AEECLSID_MEDIAPLAYER:
   #endif

   #ifdef FEATURE_APP_MMS
     case AEECLSID_TMT_MMS_APP:
   #endif

   #ifdef FEATURE_APP_WAP
     case AEECLSID_TMT_WAP_APP:
   #endif

   #ifdef FEATURE_APP_MOBIMON
       case AEECLSID_MOBIMON:
   #endif

   #ifdef FEATURE_APP_PUREVOICE
      case AEECLSID_PUREVOICE:
   #endif

   #ifdef FEATURE_APP_QCAMCORDER
      case AEECLSID_QCAMCORDER:
   #endif

   #if defined(FEATURE_APP_QCAMERA)
      case AEECLSID_QCAMERA:
   #if defined(FEATURE_APP_QALBUM)
      case AEECLSID_QALBUM:
   #endif // FEATURE_APP_QALBUM
   #endif

#ifndef CUST_EDITION
   #ifdef FEATURE_APP_RECENTCALLS
      case AEECLSID_RECENTCALLSAPP:
   #endif
#endif

   #ifdef FEATURE_APP_SVCPRG
    case AEECLSID_SVCPRGAPP:
   #endif

   #ifdef FEATURE_WMS_APP
     case AEECLSID_WMSAPP:
   #endif

   #ifdef FEATURE_MMGPS
     case AEECLSID_LCAAPP:
   #endif

   #ifdef FEATURE_QVPHONE_APP
     case AEECLSID_QVIDEOPHONE:
   #endif

   #ifdef FEATURE_SECMSM_APPS
     case AEECLSID_MMCAPP:
     case AEECLSID_AEECLSID_MYSTUFFF:
     case AEECLSID_PREPROVISION:
     case AEECLSID_DRMAPP:
     case AEECLSID_RIGHTSMGR:
   #endif /* FEATURE_SECMSM_APPS */

   #ifdef FEATURE_AUXSETTINGS_APP
     case AEECLSID_AUXSETTINGS:
   #endif

   #ifdef FEATURE_NETSETTINGS_APP
     case AEECLSID_NETSETTINGS:
   #endif

#ifdef FEATURE_IMS
    case AEECLSID_IMS_SETTINGS:
    case AEECLSID_IMS_REGMANAGER:
#ifdef FEATURE_IMS_VIDEOSHARE
    case AEECLSID_IMS_VIDEOSHARE:
#endif
#ifdef FEATURE_IMS_ENABLERPRESENCE
	case AEECLSID_IMS_PRESENCEMANAGER:
#endif
#endif /* FEATURE_IMS */
         return TRUE;

     default:
       {
       IShell *pShell = AEE_GetShell();
       if (ISHELL_AppIsInGroup(pShell, clsid, PL_SYSTEM) == AEE_SUCCESS) {
          return TRUE;
       } else {
          AEEAppInfo ai;

          if (ISHELL_QueryClass(pShell, clsid, &ai)) {
             // If this is a static applet, it is ok to start
             if (ai.wFlags & AFLAG_STATIC) {
                return TRUE;
             }
          }
       }
       return FALSE;
     }
   }
}

boolean OEM_IsCPUHigh(void)
{
#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
  return registered_for_cpu;
#else
   return FALSE;
#endif
}

boolean   OEM_GetLCDSetting()
{
  return OEM_GetCachedLCDSetting();
}


/*==============================================================
Function: OEM_GetUpdateVersion

Description: Returns the update version information.

==============================================================*/
int OEM_GetUpdateVersion(char *pszVersion, int *pnLen)
{
   int nUpdateVerLen = 0;

   if(0 == pnLen)
   {
      return EFAILED;
   }

   nUpdateVerLen = STRLEN(BREW_UPDATE_VERSION) + 1;

   if (0 == pszVersion)
   {
      *pnLen = nUpdateVerLen - 1; // not include null termination.
      return SUCCESS;
   }

   if(*pnLen < nUpdateVerLen)
   {
      *pnLen = nUpdateVerLen - 1; // not include null termination.
      *pszVersion = 0;
      return EFAILED;
   }

   STRCPY(pszVersion, BREW_UPDATE_VERSION);
   *pnLen = nUpdateVerLen - 1;  // not include null termination.

   return SUCCESS;
}

#endif
