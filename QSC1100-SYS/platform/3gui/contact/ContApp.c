/*===========================================================================

FILE: ContApp.c

SERVICES: Contact Manager Applet

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
IContactApp

INITIALIZATION AND SEQUENCING REQUIREMENTS:


         Copyright © 2002-2008 QUALCOMM Incorporated.
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/Contact/ContApp.c#18 $
$DateTime: 2008/03/28 14:44:05 $
  $Author: carls $
  $Change: 633816 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
03/26/08     cvs     Remove deprecated functions
05/24/06     JD      Added call to SelectFonts to increase size on VGA
11/11/05     aps     Fixed Critical Lint errors.
05/18/05     aps     CContactApp->m_tmpName initialization
04/19/05     jas     Replace sizing info with display bmp size
01/18/05     BW      Upgraded to contactapp.brx file from the bri version
12/13/04     ssc     Changed include file name case to match the Linux names 
                     of the files. Mismatches in the case break AMSS builds 
                     on Linux servers.
10/20/04     JAS     Release the ICARD object when app exits
06/23/04     PA      Support for enumeration of available contacts
1/26/04      AT      Fix off by one error on max digits phone numbers.
05/15/03     PA      Support for originating calls
05/09/03     PA      Handle EVT_APP_RESUME event
05/08/03     PA      Create instance of IVR interface on startup
02/06/03     kar     Code cleanup
11/15/02     pm      Port Contact App to QCT
=============================================================================*/


// Suppress "ANSI limit of 511 'external identifiers' exceeded"
//lint -save -"esym(793,external identifiers)"

// Suppress "Suspcious cast" and "Unusual pointer cast"
//lint -save -e611 -e740


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimfeatures.h"  // Simulator Feature Definitions
#endif

#include "OEMFeatures.h"  // ???? QCB -- PM: Moved this up here to grab #define FEATURE_APP_CONTACT
                          //                 before ifdef FEATURE_APP_CONTACT

#ifdef FEATURE_APP_CONTACT

#if defined(BREW_STATIC_APP)
#include "OEMClassIDs.h"
#endif

#include "ContApp.h"
#include "contactapp.brh"        // Applet Resource Header
#include "AEEVector.h"

#include "AEE_OEM.h"


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

/*===========================================================================

                    MACRO DEFINITIONS

===========================================================================*/
/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/
/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

//-----------------------------------------------------------------------------
// Public Function Declarations
//-----------------------------------------------------------------------------

#if defined(BREW_STATIC_APP)
//This function will be used to get the module information
PFNMODENTRY ContactMod_GetModInfo(IShell      *ps,
                                  AEECLSID   **ppClasses,
                                  AEEAppInfo **pApps,
                                  uint16      *pnApps,
                                  uint16      *pwMinPriv);

#endif


//Will be called when  loading the contacts module
/*static*/ int ContactMod_Load(IShell   *pIShell,
                           void     *ph,
                           IModule **ppMod);

int ContactMod_New(int16            nSize,
                   IShell          *pIShell,
                   void            *ph,
                   IModule        **ppMod,
                   PFNMODCREATEINST pfnMC,
                   PFNFREEMODDATA   pfnMF);

//-----------------------------------------------------------------------------
// Module Method Declarations
//-----------------------------------------------------------------------------

static uint32 ContactMod_AddRef(IModule *po);
static uint32 ContactMod_Release(IModule *po);

static int ContactMod_CreateInstance(IModule *po,
                                     IShell  *pIShell,
                                     AEECLSID ClsId,
                                     void   **ppObj);

static void ContactMod_FreeResources(IModule  *po,
                                     IHeap    *ph,
                                     IFileMgr *pfm);

//-----------------------------------------------------------------------------
// Applet Method Declarations
//-----------------------------------------------------------------------------

static int CContactApp_New(IShell       *ps,
                           IModule      *pIModule,
                           IContactApp **ppObj);

static uint32  CContactApp_AddRef(IContactApp *pi);
static uint32  CContactApp_Release(IContactApp *pi);

static boolean CContactApp_HandleEvent(IContactApp *pi,
                                       AEEEvent    eCode,
                                       uint16      wParam,
                                       uint32      dwParam);

// Alternate entry points
static int CContactApp_SaveNumber(IContactApp        *pi,
                                  AECHAR             *number,
                                  AppletStartMethod   method,
                                  PhoneNumberSaveType saveType);
static int CContactApp_ViewAll(IContactApp *pi);
static int CContactApp_FindNumber(IContactApp *pi,
                                  AECHAR      *searchStr);
static int CContactApp_SelectField(IContactApp          *pi,
                                   PFNSELECTEDCONTACTCB  fnSelectedCallback);

//-----------------------------------------------------------------------------
// Internal Applet Method Declarations
//-----------------------------------------------------------------------------

static void CContactApp_InitAppData(CContactApp *pMe);
static void CContactApp_FreeAppData(CContactApp *pMe);

////
// Applet state methods
////
static void CContactApp_RunFSM(CContactApp *pMe);

////
// Misc
////

static char *CContactApp_BuildStartArgs(AppletStartMethod     method,
                                        void                 *saveType,
                                        void                 *arg);
static void CContactApp_DecodeStartArgs(CContactApp *pMe, 
                                        char        *args);

/*===========================================================================

                    GLOBAL DATA

===========================================================================*/

// IMPORTANT NOTE: g_dwAEEStdLibEntry global variable is defined for SDK ONLY!
// This variable should NOT BE (1) overwritten and/or (2) USED DIRECTLY
// by BREW SDK users.
// It is used as an entry point to AEEStdLib.
// DO NOT REMOVE next three lines.
#ifdef AEE_SIMULATOR
uint32 g_dwAEEStdLibEntry;
#endif


/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

static ContactMod gContactMod = {0};

static const VTBL(IModule) gModFuncs =
{
   ContactMod_AddRef,
   ContactMod_Release,
   ContactMod_CreateInstance,
   ContactMod_FreeResources
};


// there is only ever one CContactApp
static CContactApp gContactApp = {0};

static const VTBL(IContactApp) gContactAppMethods =
{
   CContactApp_AddRef,
   CContactApp_Release,
   CContactApp_HandleEvent,
   CContactApp_SaveNumber,
   CContactApp_ViewAll,
   CContactApp_FindNumber,
   CContactApp_SelectField
};

#if defined(BREW_STATIC_APP)
static const AEEAppInfo gaiContactApp = {
   AEECLSID_CONTACT_APP,
   CONTACTAPP_RES_FILE,
   STR_APP_CONTACTS,
   0,
   0,
   0,
   0,
   AFLAG_TOOL|AFLAG_HIDDEN
};
#endif

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

#if defined(BREW_STATIC_APP)

/*=============================================================================
FUNCTION: ContactMod_GetModInfo

DESCRIPTION:
   Get the Module information
   
PARAMETERS:
   *ps:          pointer to IShell object
   **ppClasses:  pointer to AEEClSID object
   **pApps:      pointer to AEEAppInfo
   *pnApps:
   *pwMinPriv:

RETURN VALUE:
PFNMODENTRY:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
PFNMODENTRY ContactMod_GetModInfo(IShell      *ps,
                                  AEECLSID   **ppClasses,
                                  AEEAppInfo **pApps,
                                  uint16      *pnApps,
                                  uint16      *pwMinPriv)
{
   PARAM_NOT_REF(ps)
   PARAM_NOT_REF(pwMinPriv)

   *ppClasses = NULL;
   *pApps = (AEEAppInfo *)&gaiContactApp;
   *pnApps = 1;
   return((PFNMODENTRY)ContactMod_Load);
}

#endif


/*=============================================================================
FUNCTION: ContactMod_Load

DESCRIPTION: Module load function

PARAMETERS:
   *pIShell: pointer to IShell object
   *ph:
   **ppMod:  pointert to IModule Object

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#ifdef AEE_SIMULATOR
__declspec(dllexport) int AEEMod_Load(IShell   *pIShell,
                                      void     *ph,
                                      IModule **ppMod)
#else
/*static*/ int ContactMod_Load(IShell   *pIShell,
                           void     *ph,
                           IModule **ppMod)
#endif
   {
   // For dynamic apps, verify the version of the AEE STD library to ensure
   // that it matches with the version that this module was built with.
   // No need to do this check under AEE_SIMULATOR
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
   if (GET_HELPER_VER() != AEESTDLIB_VER)
      return EVERSIONNOTSUPPORT;
#endif

   return ContactMod_New(sizeof(ContactMod),
                         pIShell,
                         ph,
                         ppMod,
                         NULL,
                         NULL);
}



/*=============================================================================
FUNCTION: ContactMod_New

DESCRIPTION:
     Called when a new contact module is created
     
PARAMETERS:
   nSize:
   *pIShell: pointer to IShell Object
   *ph:
   **ppMod:  pointer to IModule Object
   pfnMC:    function pointer
   pfnMF:    function pointer

RETURN VALUE:
   int: AEE_SUCCESS if successful else EFAILED

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int ContactMod_New(int16            nSize,
                   IShell          *pIShell,
                   void            *ph,
                   IModule        **ppMod,
                   PFNMODCREATEINST pfnMC,
                   PFNFREEMODDATA   pfnMF)
{
   PARAM_NOT_REF(nSize)
   PARAM_NOT_REF(ph)
   PARAM_NOT_REF(pfnMC)
   PARAM_NOT_REF(pfnMF)

   if (!ppMod){
      return EFAILED;
   }
   *ppMod = NULL;

// IMPORTANT NOTE: g_dwAEEStdLibEntry global variable is defined for SDK ONLY!
// This variable should NOT BE (1) overwritten and/or (2) USED DIRECTLY
// by BREW SDK users.
// It is used as an entry point to AEEStdLib.
// DO NOT REMOVE next three lines.
#ifdef AEE_SIMULATOR
   g_dwAEEStdLibEntry = (uint32)ph;
   if (!pIShell || !ph){
      return EFAILED;
   }
#else
   if (!pIShell) {
      return EFAILED;
   }
#endif

   MEMSET(&gContactMod,
          0,
          sizeof(ContactMod));

   INIT_VTBL(&gContactMod,
             IModule,
             gModFuncs);

   gContactMod.m_nRefs = 1;

   *ppMod = (IModule *)&gContactMod;

   return AEE_SUCCESS;
}



//-----------------------------------------------------------------------------
// Module Method Definitions
//-----------------------------------------------------------------------------

/*=============================================================================
FUNCTION: ContactMod_AddRef

DESCRIPTION:

PARAMETERS:
   *po:  pointer to IModule

RETURN VALUE:
   uint32: current reference number

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 ContactMod_AddRef(IModule *po)
{
   return(++((ContactMod *)po)->m_nRefs);
}

/*=============================================================================
FUNCTION: ContactMod_Release

DESCRIPTION:

PARAMETERS:
   *po:  pointer to IModule

RETURN VALUE:
   uint32: current reference number

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 ContactMod_Release(IModule *po)
{
   return(--((ContactMod *)po)->m_nRefs);
}


/*=============================================================================
FUNCTION: ContactMod_CreateInstance

DESCRIPTION:
   Create the instance of contact object
   
PARAMETERS:
   *po:       pointer to IModule object
   *pIShell: pointer to IShell Object
   ClsId:    class ID
   **ppObj:

RETURN VALUE:
   int: SUCCESS if successful else EFAILED

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int ContactMod_CreateInstance(IModule *po,
                                     IShell  *pIShell,
                                     AEECLSID ClsId,
                                     void   **ppObj)
{
   *ppObj = NULL;

   if (ClsId != AEECLSID_CONTACT_APP) {
      return EFAILED;
   }

   if (CContactApp_New(pIShell, po, (IContactApp**)ppObj) != SUCCESS) {
      return EFAILED;
   }

   return SUCCESS;
}


/*=============================================================================
FUNCTION: ContactMod_FreeResources

DESCRIPTION:
   Cleanup the resources 
   
PARAMETERS:
   *po:  pointer to IModule 
   *ph:   pointer to IHeap
   *pfm: pointer to IFileMgr

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void ContactMod_FreeResources(IModule  *po,
                                     IHeap    *ph,
                                     IFileMgr *pfm)
{
   PARAM_NOT_REF(po)
   PARAM_NOT_REF(ph)
   PARAM_NOT_REF(pfm)
}



//-----------------------------------------------------------------------------
//  Applet Method Definitions
//-----------------------------------------------------------------------------

/*=============================================================================
FUNCTION: CContactApp_New

DESCRIPTION:
    Called when new Contact Applet is created
    
PARAMETERS:
   *ps:       pointer to IShell
   *pIModule: pointer to IModule
   **ppObj:   pointer to IContactApp

RETURN VALUE:
   int: SUCCESS if successful else EFAILED

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CContactApp_New(IShell       *ps,
                           IModule      *pIModule,
                           IContactApp **ppObj)
{
   if (0 == gContactApp.m_nRefs) {
      // Must initialize the object
      MEMSET(&gContactApp,
             0,
             sizeof(gContactApp));

      INIT_VTBL(&gContactApp,
                IContactApp,
                gContactAppMethods);

      //Intialise all the generic applet variables
      gContactApp.m_nRefs     = 0;
      gContactApp.m_pShell    = ps;
      gContactApp.m_pModule   = pIModule;

      (void) ISHELL_AddRef(ps);
      (void) IMODULE_AddRef(pIModule);

      //Initialize the applet specific variables
      CContactApp_InitAppData(&gContactApp);
   }


   ++gContactApp.m_nRefs;

   *ppObj = (IContactApp*)&gContactApp;

   return SUCCESS;
}


/*=============================================================================
FUNCTION: CContactApp_AddRef

DESCRIPTION:

PARAMETERS:
   *p: pointer to IContactApp

RETURN VALUE:
   uint32: current reference number

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CContactApp_AddRef(IContactApp *p)
{
   register CContactApp *pMe = (CContactApp*)p;

   ASSERT(pMe != NULL);
   ASSERT(pMe->m_nRefs > 0);

   return ++pMe->m_nRefs;
}


/*=============================================================================
FUNCTION: CContactApp_Release

DESCRIPTION:

PARAMETERS:
   *p:

RETURN VALUE:
   int32: current reference number

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CContactApp_Release(IContactApp *p)
{
   register CContactApp *pMe = (CContactApp*)p;

   ASSERT(pMe != NULL);
   if (0 == pMe->m_nRefs) {
      return 0;
   }

   if (--pMe->m_nRefs) {
      return pMe->m_nRefs;
   }
   //Free all the applet specific data
   CContactApp_FreeAppData(pMe);

   (void) ISHELL_Release(pMe->m_pShell);
   (void) IMODULE_Release(pMe->m_pModule);

   // NOTE: pMe is allocated statically so there's no need to FREE() it
   return 0;
}


/*=============================================================================
FUNCTION: CContactApp_HandleEvent

DESCRIPTION:
   This is the EventHandler for this app. All events to this app are handled
   in this function.

PARAMETERS:
   pi [in]: Pointer to the SMS Applet structure. This structure contains
   information specific to this applet.


    
   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE: If the app has processed the event.
   FALSE: If the app did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CContactApp_HandleEvent(IContactApp *pi,
                                       AEEEvent     eCode,
                                       uint16       wParam,
                                       uint32       dwParam)
{
   CContactApp *pMe        = (CContactApp*)pi;
   IImage      *pPersImage = NULL;
   AEEImageInfo ii;
   IShell *pIShell = AEE_GetShell();

   if (pMe == NULL)
   {
      MSG_ERROR("CContactApp_HandleEvent : NULL pMe", 0, 0, 0);
      ISHELL_CloseApplet(pIShell,FALSE);
      return FALSE;
   }
   switch (eCode) {
      case EVT_APP_START:
         {
            AEEAppStart  *as;
            IBitmap      *pDisplayBmp;
            int           nErrCode;

            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;
            
            pMe->m_bAddNewMenu = FALSE;
            pMe->m_suspending  = FALSE;
            pMe->m_isActive    = TRUE;

#ifdef FEATURE_CONTACTNAME_PREDICT
            pMe->m_nSpeedDialIndexesEmpty = NULL;
#endif

            if (pMe->m_pDisplay != NULL) {
               (void) IDISPLAY_Release(pMe->m_pDisplay);
               pMe->m_pDisplay = NULL;
            }

            pMe->m_pDisplay = as->pDisplay;
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);

            pDisplayBmp = IDISPLAY_GetDestination(pMe->m_pDisplay);
            if (! pDisplayBmp)
            {
              MSG_ERROR("Unable to get display bmp", 0, 0, 0);
              (void) IDISPLAY_Release(pMe->m_pDisplay);
              return FALSE;
            }

            nErrCode = IBITMAP_GetInfo(pDisplayBmp, &(pMe->m_BmpInfo),
                                       sizeof(pMe->m_BmpInfo));
            if (nErrCode != SUCCESS)
            {
              MSG_ERROR("Unable to get bmp info", 0, 0, 0);
              (void) IDISPLAY_Release(pMe->m_pDisplay);
              (void) IBITMAP_Release(pDisplayBmp);
              return FALSE;
            }
            (void) IBITMAP_Release(pDisplayBmp);


            /* The rectangle received from BREW in the AEEAppStart structure
             * is incorrect, so we need to explicitly get the rectangle
             * from the bitmap info.
             */
            pMe->m_rc.x = 0;
            pMe->m_rc.y = 0;
            pMe->m_rc.dx = (int16)pMe->m_BmpInfo.cx;
            pMe->m_rc.dy = (int16)pMe->m_BmpInfo.cy;

	    // This will increase the font size on VGA displays.
	    AppComFunc_SelectFonts(pMe->m_pDisplay);
	    
            //We will get the line height from the icon height if it is
            //greater than font height, else we will get it from font.
            //We take the icon height from PERSONAL icon and assume that all
            //icons for this applet will be same size
            pMe->m_lineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                                                        AEE_FONT_NORMAL,
                                                        NULL,
                                                        NULL);

            //number of lines if only text is shown
            pMe->m_numTextOnlyLines = (uint16)(pMe->m_rc.dy / pMe->m_lineHeight);

            pPersImage = ISHELL_LoadResImage(pMe->m_pShell,
                                             CONTACTAPP_RES_FILE,
                                             ICON_CONTACTCLASS_PERS);
            if (pPersImage != NULL) {
               IIMAGE_GetInfo(pPersImage,&ii);
               IIMAGE_Release(pPersImage);
               if (pMe->m_lineHeight < ii.cy) {
                  pMe->m_lineHeight = ii.cy;
               }
            }

            pMe->m_numLines = (uint16)(pMe->m_rc.dy / pMe->m_lineHeight);

            CContactApp_DecodeStartArgs(pMe, (char *) as->pszArgs);

            // reset the FSM
            pMe->m_currState = STATE_CONT_NULL;
            pMe->m_nextState = STATE_CONT_INIT;
            pMe->m_dlgRetState = STATE_CONT_NULL;

#ifdef FEATURE_PHONE_VR
            if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, 
                                                 AEECLSID_VR, 
                                                 (void **) &pMe->m_pVR)) {
              pMe->m_pVR = NULL;
            } else {
               OEMVRSound sndArray[] = {
                              { AEEVRUTTERANCE_ID_NAME_PLEASE, NULL },
                              { AEEVRUTTERANCE_NONE, NULL} };
               OEMVR_SetPrompt(pMe->m_pVR, OEMVRPROMPT_TRAIN_INITIAL, sndArray);

               if (IVR_SetVoiceTagDB(pMe->m_pVR, "VoiceDB/All") != SUCCESS) {
                  IVR_Release(pMe->m_pVR);
                  pMe->m_pVR = NULL;
               }
            }
#endif

            CContactApp_RunFSM(pMe);
         }
         return TRUE;

      case EVT_APP_STOP:
         pMe->m_startMethod = STARTMETHOD_NORMAL;
         pMe->m_isActive = FALSE;
         pMe->m_suspending = TRUE;

         if (pMe->m_pDisplay != NULL) {
            (void) IDISPLAY_Release(pMe->m_pDisplay);
            pMe->m_pDisplay = NULL;
         }

#ifdef FEATURE_PHONE_VR
         if (pMe->m_pVR != NULL) {
            (void) IVR_Release(pMe->m_pVR);
            pMe->m_pVR = NULL;
         }
#endif
         return TRUE;

      case EVT_APP_SUSPEND:
         pMe->m_suspending = TRUE;
         ASSERT(TRUE == pMe->m_isActive);
#ifdef FEATURE_PHONE_VR
         if (pMe->m_pVR != NULL) {
            pMe->m_userAbort = FALSE;
            (void) IVR_AbortOperation(pMe->m_pVR);
         }
#endif
         return TRUE;

      case EVT_APP_RESUME:
         {
            AEEAppStart  *as;
            IBitmap      *pDisplayBmp;
            int           nErrCode;

            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;

            pMe->m_suspending = FALSE;
            ASSERT(TRUE == pMe->m_isActive);

            if (pMe->m_pDisplay != NULL) {
               (void) IDISPLAY_Release(pMe->m_pDisplay);
               pMe->m_pDisplay = NULL;
            }

            pMe->m_pDisplay = as->pDisplay;
            (void) IDISPLAY_AddRef(pMe->m_pDisplay);

            pDisplayBmp = IDISPLAY_GetDestination(pMe->m_pDisplay);
            if (! pDisplayBmp)
            {
              MSG_ERROR("Unable to get display bmp", 0, 0, 0);
              (void) IDISPLAY_Release(pMe->m_pDisplay);
              return FALSE;
            }

            nErrCode = IBITMAP_GetInfo(pDisplayBmp, &(pMe->m_BmpInfo),
                                       sizeof(pMe->m_BmpInfo));
            if (nErrCode != SUCCESS)
            {
              MSG_ERROR("Unable to get bmp info", 0, 0, 0);
              (void) IDISPLAY_Release(pMe->m_pDisplay);
              (void) IBITMAP_Release(pDisplayBmp);
              return FALSE;
            }
            (void) IBITMAP_Release(pDisplayBmp);

            /* The rectangle received from BREW in the AEEAppStart structure
             * is incorrect, so we need to explicitly get the rectangle
             * from the bitmap info.
             */
            pMe->m_rc.x = 0;
            pMe->m_rc.y = 0;
            pMe->m_rc.dx = (int16)pMe->m_BmpInfo.cx;
            pMe->m_rc.dy = (int16)pMe->m_BmpInfo.cy;


            CContactApp_RunFSM(pMe);
         }
         return TRUE;

      case EVT_DIALOG_INIT:
      // Update the active dialog info in the one and only ContactApp object.
         ContDlgHandler_UpdateActiveDialogInfo(pMe,
                                               wParam,
                                               dwParam);

         return ContDlgHandler_RouteDialogEvent(pMe,
                                                eCode,
                                                wParam,
                                                dwParam);

      case EVT_DIALOG_START:
         return ContDlgHandler_RouteDialogEvent(pMe,
                                                eCode,
                                                wParam,
                                                dwParam);

      case EVT_DIALOG_END:
         if (0 == wParam) return TRUE;

         (void) ContDlgHandler_RouteDialogEvent(pMe,
                                                eCode,
                                                wParam,
                                                dwParam);

         pMe->m_activeDlg = NULL;
         pMe->m_activeDlgHandleEvent = NULL;

         pMe->m_nextState = pMe->m_dlgRetState;
         pMe->m_dlgRetState = STATE_CONT_NULL;

         // Don't run the FSM if we are suspending (EVT_SUSPEND is
         // sent before EVT_DIALOG_END).  Wait until we resume...
         if (FALSE == pMe->m_suspending) {
            CContactApp_RunFSM(pMe);
         }
         return TRUE;

      case EVT_BUSY:
      // the applet is never 'busy'
         break;

      default:
         return ContDlgHandler_RouteDialogEvent(pMe,
                                                eCode,
                                                wParam,
                                                dwParam);
   }

   return FALSE;
} // CContactApp_HandleEvent()


/*=============================================================================
FUNCTION: CContactApp_SaveNumber

DESCRIPTION:
   Starts the contact applet to Save the number. Called generally by other 
   applets. We increment the Reference count as work around since we want
   contact applet running even after the function returns.
   
PARAMETERS:
   *pi: Pointer to Contacts Applet 
   *number: Number to be saved
   *method : Type of save number method used to start this applet
   *saveType : Phone number save Type ( see PhoneNumberSaveType)

RETURN VALUE:
   int: SUCCESS if the contacts applet was started successfully 
        otherwise EFAILED or ENOMEMORY

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CContactApp_SaveNumber(IContactApp        *pi,
                                  AECHAR             *number,
                                  AppletStartMethod   method,
                                  PhoneNumberSaveType saveType)
{
   CContactApp *pMe = (CContactApp*)pi;
   char        *args;
   int          ret;

   if (TRUE == pMe->m_isActive) {
      // Contact app is already running
      return EFAILED;
   }

   if (NULL == number || WSTRLEN(number) <= 0) {
      return EFAILED;
   }

   args = CContactApp_BuildStartArgs(method,
                                     &saveType,
                                     number);

   ret = ISHELL_StartAppletArgs(pMe->m_pShell,
                                AEECLSID_CONTACT_APP,
                                args);
   if (args) {
      FREE(args);
   }
   return ret;
}


/*=============================================================================
FUNCTION: CContactApp_ViewAll

DESCRIPTION:
   Lists all the contacts on the Phone EFS and RUIM. Called by another applet.
   We increment the Reference count as work around since we want contact 
   applet running even after the function returns.
   
PARAMETERS:
   *pi: Pointer to Contact Applet

RETURN VALUE:
   int: SUCCESS if the contacts applet was started successfully 
        otherwise EFAILED or ENOMEMORY

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CContactApp_ViewAll(IContactApp *pi)
{
   CContactApp *pMe = (CContactApp*)pi;
   char        *args;
   int          ret;

   if (TRUE == pMe->m_isActive) {
      // Contact app is already running
      return EFAILED;
   }

   args = CContactApp_BuildStartArgs(STARTMETHOD_VIEWALL,
                                     NULL,
                                     NULL);

   ret = ISHELL_StartAppletArgs(pMe->m_pShell,
                                AEECLSID_CONTACT_APP,
                                args);
   if (args) {
      FREE(args);
   }
   return ret;
}


/*=============================================================================
FUNCTION: CContactApp_FindNumber

DESCRIPTION:
   Finds the contacts with matching the numbe passed. Called by another applet.
   We increment the Reference count as work around since we want contact 
   applet running even after the function returns.

PARAMETERS:
   *pi: Pointer to Contact Applet
   *searchStr: search string consisting the number to be found

RETURN VALUE:
   int: SUCCESS if the contacts applet was started successfully 
        otherwise EFAILED or ENOMEMORY

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CContactApp_FindNumber(IContactApp *pi,
                                  AECHAR      *searchStr)
{
   CContactApp *pMe = (CContactApp*)pi;
   char        *args;
   int          ret;

   if (TRUE == pMe->m_isActive) {
      // Contact app is already running
      return EFAILED;
   }

   if (NULL == searchStr || WSTRLEN(searchStr) <= 0) {
      return EFAILED;
   }

   args = CContactApp_BuildStartArgs(STARTMETHOD_FINDNUMBER, 
                                     NULL,
                                     searchStr);
   pMe->m_startMethod = STARTMETHOD_FINDNUMBER;

   ret = ISHELL_StartAppletArgs(pMe->m_pShell,
                                AEECLSID_CONTACT_APP,
                                args);
   if (args) {
      FREE(args);
   }
   return ret;
}


/*=============================================================================
FUNCTION: CContactApp_SelectField

DESCRIPTION:
   Selects a field of contact. Called by another applet.
   We increment the Reference count as work around since we want contact 
   applet running even after the function returns.

PARAMETERS:
   *pi: Pointer to Contact Applet
   fnSelectedCallback: Callback function

RETURN VALUE:
int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CContactApp_SelectField(IContactApp          *pi,
                                   PFNSELECTEDCONTACTCB  fnSelectedCallback)
{
   CContactApp *pMe = (CContactApp*)pi;
   char        *args;
   int          ret;

   if (TRUE == pMe->m_isActive) {
      // Contact app is already running
      return EFAILED;
   }

   args = CContactApp_BuildStartArgs(STARTMETHOD_SELECTFIELD, 
                                     NULL,
                                     (void *) (uint32) fnSelectedCallback);

   ret = ISHELL_StartAppletArgs(pMe->m_pShell,
                                AEECLSID_CONTACT_APP,
                                args);
   if (args) {
      FREE(args);
   }
   return ret;
}


//-----------------------------------------------------------------------------
// Internal Applet Method Declarations
//-----------------------------------------------------------------------------

/*=============================================================================
FUNCTION: CContactApp_InitAppData

DESCRIPTION:
   Initializes the application data

PARAMETERS:
   *pMe: CContactApp Object pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CContactApp_InitAppData(CContactApp *pMe)
{

   pMe->m_addr_book  = NULL;
   pMe->m_rec = NULL;
   pMe->m_contact_iv = NULL;
   pMe->m_speeddial_iv = NULL;
   pMe->m_fldrecs_iv = NULL;

   pMe->m_currState = STATE_CONT_NULL;
   pMe->m_nextState = STATE_CONT_INIT;
   pMe->m_dlgRetState = STATE_CONT_NULL;

   pMe->m_searchStr = NULL;

   pMe->m_activeDlg = NULL;
   pMe->m_activeDlgHandleEvent = NULL;

   pMe->m_mainDlg_menuSel = MAIN_MENUITEM_FINDNAME;

   pMe->m_startMethod = STARTMETHOD_NORMAL;
   pMe->m_isActive = FALSE;

   pMe->m_fieldInfo.m_name = NULL;
   pMe->m_fieldInfo.m_type = STR_FIELDINPUT_NUMBER;
   pMe->m_fieldInfo.m_numberType = NUMBERTYPE_MENUITEM_WORK;

   pMe->m_contactInfo.m_name = NULL;
   pMe->m_contactInfo.m_class = AEE_ADDR_CAT_PERSONAL;
   pMe->m_tmpName = NULL;      

   pMe->m_msgBox_IStatic = NULL;

   pMe->m_ruimDlg_menuSel = 0;

   pMe->m_availRecInit = FALSE;

#ifdef FEATURE_CONTACTNAME_PREDICT
   pMe->m_bContListNeedsReload = FALSE;
#endif

}



/*=============================================================================
FUNCTION: CContactApp_FreeAppData

DESCRIPTION:
   Frees up all the data before exiting the applet

PARAMETERS:
   *pMe: CContactApp Object pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CContactApp_FreeAppData(CContactApp *pMe)
{
   if (NULL != pMe->m_msgBox_IStatic) {
      (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
      pMe->m_msgBox_IStatic = NULL;
   }

   FREE_CONTACTINFO(pMe->m_contactInfo);
   FREE_FIELDINFO(pMe->m_fieldInfo);

   FREE_STRING(pMe->m_searchStr);

#ifdef FEATURE_CONTACTNAME_PREDICT
   if(pMe->m_nSpeedDialIndexesEmpty != NULL){
      FREE(pMe->m_nSpeedDialIndexesEmpty);
      pMe->m_nSpeedDialIndexesEmpty = NULL;
   }
#endif

   if (NULL != pMe->m_addr_book) {
      (void)IADDRBOOK_Release(pMe->m_addr_book);
      pMe->m_addr_book = NULL;
   }

   ContDlgHandler_ReleaseCDBRec(pMe);

   if (NULL != pMe->m_fldrecs_iv) {
      ContDlgHandler_FreeAEEAddrFields(pMe);
      (void) IVector_Release(pMe->m_fldrecs_iv);
      pMe->m_fldrecs_iv = NULL;
   }

   if (NULL != pMe->m_contact_iv) {
      ContDlgHandler_FreeContactList(pMe,pMe->m_contact_iv);
      (void) IVector_Release(pMe->m_contact_iv);
      pMe->m_contact_iv = NULL;
   }
   
   if (NULL != pMe->m_speeddial_iv) {
      ContDlgHandler_FreeContactList(pMe,pMe->m_speeddial_iv);
      (void) IVector_Release(pMe->m_speeddial_iv);
      pMe->m_speeddial_iv = NULL;
   }

   if (pMe->m_pDisplay != NULL) {
      (void) IDISPLAY_Release(pMe->m_pDisplay);
      pMe->m_pDisplay = NULL;
   }

#ifdef FEATURE_MMGSDI
   if (pMe->m_pICard != NULL) {
      (void) ICARD_Release(pMe->m_pICard);
      pMe->m_pICard = NULL;
   }
#endif // FEATURE_MMGSDI

}

/*=============================================================================
FUNCTION: CContactApp_RunFSM

DESCRIPTION: Run the finite state machine

PARAMETERS:
   *pMe: CContactApp Object pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CContactApp_RunFSM(CContactApp *pMe)
{
   for (;;) {
      ASSERT(pMe->m_nextState != STATE_CONT_NULL);

      pMe->m_currState = pMe->m_nextState;
      pMe->m_nextState = STATE_CONT_NULL;

      ContStateMachine_ProcessState(pMe);

      if (STATE_CONT_NULL == pMe->m_nextState) {
         break;
      }
   }
}

/*=============================================================================
FUNCTION: CContactApp_BuildStartArgs

DESCRIPTION: Builds a startup argument string

PARAMETERS:
   method: desired 'startup method'.  
   saveType: Phone number save Type(Only useful in case method is
     STARTMETHOD_SAVENUMBER_NUMBER_EDIT or STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT
     All other methods should pass this as null  
   *arg: optional startup method specific argument

RETURN VALUE:
    char *:  Encoded startup argument string that may be used with
             ISHELL_StartAppletArgs().  

             If non-NULL, this string must be deallocated by the caller

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
   CContactApp_SaveNumber()
   CContactApp_ViewAll()
   CContactApp_FindNumber()
   CContactApp_SelectField()

=============================================================================*/
static char *CContactApp_BuildStartArgs(AppletStartMethod    method,
                                        void                *saveType,
                                        void                *arg)
{
   char *str = NULL;

   switch (method) {
      case STARTMETHOD_NORMAL:
         break;

      case STARTMETHOD_SAVENUMBER_NUMBER_EDIT:
      case STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT:
         if ((arg != NULL)&& (saveType!=NULL)) {
            uint32 len;
            PhoneNumberSaveType *phoneSaveType = (PhoneNumberSaveType*)saveType;

            len = (uint32) WSTRLEN((AECHAR*) arg);

            // Note:UTF8 encoding can occupy anything from 0.5x to 4x more space
            //       than Unicode encoding of the same string.   We take the
            //       paranoid (and wasteful!) approach and always assume it will 
            //       take 4x the space
            //In the first char we will store the method and the second char
            // we will store the save type. Hence we have 2 in the begining
            str = MALLOC(2 + len * 4);

            if (str) {
               //store the method in the first character
               if (STARTMETHOD_SAVENUMBER_NUMBER_EDIT == method) {
                  *str = STARTARGPREFIX_SAVENUMBER_NUMBEREDIT;
               } else {
                  *str = STARTARGPREFIX_SAVENUMBER_NONUMBEREDIT;
               }
               //store the saveType in the second character
               switch(*phoneSaveType){
                  case SAVE_TO_EXISTING:
                     str[1] = STARTARGPREFIX_SAVETYPE_SAVETOEXISTING;
                     break;
                     
                  case SAVE_TO_RUIM:
                     str[1] = STARTARGPREFIX_SAVETYPE_SAVETORUIM;
                     break;
                  
                  case SAVE_NEW:
                  default:
                     str[1] = STARTARGPREFIX_SAVETYPE_SAVENEW;
                     break;

               }
               
               (void) WSTRTOUTF8((AECHAR *) arg, 
                                 (int) len,
                                 (byte *) &str[2],
                                 (int)( len * 4 ));
            }
         }
         break;

      case STARTMETHOD_FINDNUMBER:
         if (arg != NULL) {
            uint32 len;

            len = (uint32) WSTRLEN((AECHAR*) arg);

         // Note: UTF8 encoding can occupy anything from 0.5x to 4x more space
         //       than Unicode encoding of the same string.   We take the
         //       paranoid (and wasteful!) approach and always assume it will 
         //       take 4x the space
            str = MALLOC(1 + len * 4);

            if (str) {
               *str = STARTARGPREFIX_FINDNUMBER;
               
               (void) WSTRTOUTF8((AECHAR *) arg, 
                                 (int) len,
                                 (byte *) &str[1],
                                 (int)( len * 4 ));
            }
         }
         break;

      case STARTMETHOD_SELECTFIELD:
         if (arg != NULL) {
            char buf[16];

            SNPRINTF(buf, sizeof(buf), "%c%d", STARTARGPREFIX_SELECTFIELD, (uint32) arg);

            str = STRDUP(buf);
         }
         break;

      case STARTMETHOD_VIEWALL:
         str = MALLOC(sizeof(char[2]));
         if (str != NULL) {
            str[0] = STARTARGPREFIX_VIEWALL;
            str[1] = '\0';
         }
         break;

      default:
         ASSERT_NOT_REACHABLE;
         break;
   }
   return str;
}


/*=============================================================================
FUNCTION: CContactApp_DecodeStartArgs

DESCRIPTION:  Decodes a startup argument string passed into EVT_APP_START

PARAMETERS:
   *pMe:  CContactApp object pointer
   *args: startup argument string

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   Sets the CContactApp::m_startMethod field and possibly other fields
   depending on the contents of the startup argument string.

SEE ALSO:

=============================================================================*/
static void CContactApp_DecodeStartArgs(CContactApp *pMe, 
                                        char        *args)
{
   uint32  len;
   AECHAR *wStr;

   if (NULL == args) {
      return;
   }

   len = STRLEN(args);
   if (len < 1) {
      return;
   }

   // Default to 'normal start method'
   pMe->m_startMethod = STARTMETHOD_NORMAL;

   switch (*args) {
      case STARTARGPREFIX_SAVENUMBER_NUMBEREDIT:
      case STARTARGPREFIX_SAVENUMBER_NONUMBEREDIT:
         if (len < 2) {
         // invalid argument length
            break;
         }

         len = STRLEN(&args[2]) + 1;

         // Note: UTF8 encoding can occupy anything from 0.5x to 4x more space
         //       than Unicode encoding of the same string.   We take the
         //       paranoid (and wasteful!) approach and always assume that the 
         //       Unicode string will take 2x the length of the UTF8 string
         wStr = MALLOC(len * sizeof(AECHAR)); 

         if (NULL == wStr) {
            break;
         }

         (void) UTF8TOWSTR((byte *) &args[2], 
                           (int) len,
                           wStr, 
                           (int)( len * sizeof(AECHAR) ) );


         //decode the first character where the method is stored
         if (STARTARGPREFIX_SAVENUMBER_NUMBEREDIT == *args) {
            pMe->m_startMethod = STARTMETHOD_SAVENUMBER_NUMBER_EDIT;
         } else {
            pMe->m_startMethod = STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT;
         }
         
         //decode the second character where the PhoneNumberSaveType is stored
         switch(args[1]){
            case STARTARGPREFIX_SAVETYPE_SAVETOEXISTING:
               pMe->m_phoneSaveType = SAVE_TO_EXISTING;
               break;
            case STARTARGPREFIX_SAVETYPE_SAVETORUIM:
               pMe->m_phoneSaveType = SAVE_TO_RUIM;
               break;

            case STARTARGPREFIX_SAVETYPE_SAVENEW:
            default:
               pMe->m_phoneSaveType = SAVE_NEW;
               break;
         }

         FREE_STRING(pMe->m_fieldInfo.m_name);
         pMe->m_fieldInfo.m_name = wStr;
         pMe->m_fieldInfo.m_type = STR_FIELDINPUT_NUMBER;
         pMe->m_fieldInfo.m_numberType = NUMBERTYPE_MENUITEM_WORK;
         break;

      //If the start method is find number
      case STARTARGPREFIX_FINDNUMBER:
         if (len < 2) {
         // invalid argument length
            break;
         }

         len = STRLEN(&args[1]) + 1;

      // Note: UTF8 encoding can occupy anything from 0.5x to 4x more space
      //       than Unicode encoding of the same string.   We take the
      //       paranoid (and wasteful!) approach and always assume that the 
      //       Unicode string will take 2x the length of the UTF8 string
         wStr = MALLOC(len * sizeof(AECHAR)); 

         if (NULL == wStr) {
            break;
         }

         (void) UTF8TOWSTR((byte *) &args[1], 
                           (int) len,
                           wStr, 
                           (int)( len * sizeof(AECHAR) ) );


         pMe->m_startMethod = STARTMETHOD_FINDNUMBER;

         FREE_STRING(pMe->m_searchStr);
         pMe->m_searchStr = wStr;
         pMe->m_contactListType = STR_CONTACTLIST_FINDNUMBER;
         break;

      case STARTARGPREFIX_VIEWALL:
         pMe->m_startMethod = STARTMETHOD_VIEWALL;
         break;

      case STARTARGPREFIX_SELECTFIELD:
         pMe->m_startMethod = STARTMETHOD_SELECTFIELD;
         pMe->m_selNumberCallback = (PFNSELECTEDCONTACTCB) ATOI(args+1);
         break;

      default:
         break;
   }
}


/*=============================================================================
FUNCTION: CContactApp_CallNumber

DESCRIPTION:
   Calls the number indicated in the AEEAddrField. Uses idle applet to do this.
   
PARAMETERS:
   *pMe: pointer to CContactApp object
   *cf ; pointer to AEEAddrField Object

RETURN VALUE:
   boolean: TRUE if successful otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CContactApp_CallNumber(CContactApp *pMe,
                               AEEAddrField *cf)
{
#ifdef FEATURE_IDLE_CALL
   FieldInfo *fi = NULL;
   boolean ret = FALSE;
   char szNumber[1+FLDLEN_NUMBER];

   if (cf == NULL)
      return FALSE;

   if ((fi = ContDlgHandler_AEEAddrField2FieldInfo(pMe, cf)) == NULL)
      return FALSE;


   if (STR_FIELDINPUT_NUMBER == fi->m_type) {
      WSTRTOSTR((AECHAR *)cf->pBuffer, szNumber, sizeof(szNumber));

      if (OriginateVoiceCall(szNumber) == SUCCESS)
         ret = TRUE;

   }

   if (fi != NULL) {
      FREE_FIELDINFO(*fi);
      FREE(fi);
   }

   return ret;
#else
   return FALSE;
#endif /* FEATURE_IDLE_CALL */
}

//lint -restore

#endif /*FEATURE_APP_CONTACT*/

