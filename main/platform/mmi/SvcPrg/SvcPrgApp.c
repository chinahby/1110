/*=============================================================================

FILE: SvcPrg.c
 
SERVICES: Service Programming applet
 
GENERAL DESCRIPTION:
 
PUBLIC CLASSES AND STATIC FUNCTIONS:
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
 
(c) COPYRIGHT 2003 - 2006 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/qsc60x0/apps/SvcPrg/SvcPrgApp.c#2 $
$DateTime: 2006/07/19 12:53:23 $

                      EDIT HISTORY FOR FILE
 
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/20/04   sun     GetItemData was assuming wrong input parameters
02/12/04   ram     Added CDMA/HDR Rx diversity control menus.
01/09/04   sun     Removed static from prototype
01/05/04   kar     Fixed compile warnings
11/24/03   kar     Changes to support AKEY entry
10/23/03   PA      Changed ICM CLSID to AEECLSID_CM
10/16/03   kar     Made app backwards compatible with non ICM targets
09/30/03   tml     Reflected changes in ICM API
09/25/03   tml     Remove UI dependencies, add ICM supports
01/31/03   kar     Moved macros to AppComFunc.h
01/30/03   kar     Removed UIM related support
01/27/03   kar     Used AppComFunc API to set radio buttons
01/18/03   kar     Initial revision
 
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#if !defined(BREW_STATIC_APP)
//#include "SVCPRGAPP.BID"
#else
#include "OEMClassIds.h"      // Applet Ids
#endif

#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEHeap.h"
#include "AEEMenu.h"
#include "AEEText.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AppComFunc.h"
#include "AEEConfig.h"
#include "svcprg.brh"
#include "Appscommon.h"
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "uixcm.h"
#include "uistate.h"
#endif

#include "OEMCFGI.h"

#ifdef FEATURE_AUTH
#include "auth.h"
#endif

#include "msg.h"
#include "UTK_priv.h"
#include "ui.h"
#include "OEMNV.h"
#include "Voccal.h"
#include "AEEDownload.h"
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#define  AEE_SVCPRG_RES_FILE (AEE_RES_LANGDIR SVCPRG_RES_FILE)
// When an IMenuCtl itemID has this flag set, it is part of a 'range'
// (the remainder of the itemID is the idx into the range)
#define RANGE_ITEMID_FLAG ((uint16) 0x8000)

// When an IMenuCtl itemID has this flag set, it is part of a 'sid/nid' list
// (the remainder of the itemID is the idx into the 'sid/nid' list)
#define SIDNID_ITEMID_FLAG ((uint16) 0x4000)

// Time in milliseconds before a message dialog closes automatically 
#define MESSAGEDIALOG_TIMEOUT_MS  (3 * 1000)

#define WORKAROUND_IMENUCTL_DISABLE_UPDOWN_KEY_EVENT

/* AKEY constants */
#define MIN_AKEY_DIGITS 6
#ifdef FEATURE_AUTH
#define MAX_AKEY_DIGITS AUTH_A_KEY_DIGITS
#else
#define MAX_AKEY_DIGITS 26
#endif
#define FEATURE_LOCK
#define debug DBGPRINTF
#define MENU_SETBOTTOMBAR(p, t)                     \
{                                                   \
    IMENUCTL_SetBottomBarType((p), (t));            \
}
/*===========================================================================

                    MACRO DEFINITIONS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

typedef enum {
   // DT_UNINITIALIZED isn't used explictly anywhere the code to Lint
   // complains...
   /*lint -esym(749,DT_UNINITIALIZED)*/
   DT_UNINITIALIZED = 0, 
   
   DT_ESN, 
   DT_WSTR,        // A varible length string with number input mode only 
   DT_WSTR_FIXED,  // A fixed length string with number input mode only
   DT_BYTE,
   DT_WORD,
   DT_DWORD,
   DT_BOOLEAN,  // Assumed to be a boolean C type
   DT_RANGE,    // Assumed to be an uint8 C type
   DT_SIDNIDLIST,
   DT_AKEY,
   DT_SUBMENU
   ,DT_ALPHA       // A varible length string with Alpha input mode only 
#ifdef FEATURE_NV_VOICE_PARAMETER   
   ,DT_BOOLEAN_VOCCAL
   ,DT_WORD_VOCCAL
#endif //FEATURE_NV_VOICE_PARAMETER   
} ItemType;


typedef struct _RangeItemType {
   uint16  resId;
   uint16  value;  // Note that only the first 8-bits may be used
                   // for the value.
                   //
                   // The 16th bit is reserved for the flag RANGE_ITEMID_FLAG
} RangeItemType;


typedef struct _MenuItemType {
   uint16        title;  // Resource Id for this item.

   ItemType      itemType; // The type of this item.

   // Extra data required for some of the item types
   union {
      // If DT_RANGE, this points to a list of range items.
      //
      // The last range item in the list will be all zero
      RangeItemType *rangeData;

      // If DT_SUBMENU type, this points to a list of MenuItemTypes
      struct _MenuItemType *subMenu;

      // If DT_WSTR, this it the max length of the string (including
      // a terminating NULL word).
      //
      // If DT_WSTR_FIXED, this it the fixed length of the string (including
      // a terminating NULL word).
      uint8                 strLen;

      // If DT_SIDNID,
      struct {
         // this is the total number of SID/NID pairs
         uint8              count;

         // Used to store the index of the SID/NID pair that is currently 
         // being edited.
         uint16             currIndex;
      } sidnid;

   } typeData;


   AEEConfigItem cfgItem;  // IConfig item to use for this item.  This is
                           // ignored for DT_SUBMENU types.

   flg isEditable:1; // Is this item editable?
   flg isLast:1;     // TRUE if this is the last MenuItemType in the
                     // current menu.
#ifdef FEATURE_NV_VOICE_PARAMETER
   nv_items_enum_type nvitem;
#endif //#ifdef FEATURE_NV_VOICE_PARAMETER
} MenuItemType;
   


typedef struct _CSvcPrgApp {
   AEEApplet     a;
   AEERect       m_rc; // Applet rectangle

   IDisplay     *m_pDisplay;
   IConfig      *m_pConfig;

   boolean       m_resetOnExit;
   
   MenuItemType *m_mainMenu;
   
   MenuItemType *m_lockMenu;
   
   int           m_ShowMainMenu;
   
   IAnnunciator *m_pIAnn;

#ifdef FEATURE_ENABLE_OTKSL
   // TRUE if access to the Service Programming applet was gained
   // using the OTKSL code.
   boolean       m_accessByOTKSL;
#endif /* FEATURE_ENABLE_OTKSL */
} CSvcPrgApp;

#ifdef FEATURE_NV_VOICE_PARAMETER
extern nv_voc_pcm_path_cal_type nv_voc_pcm_on_chip_0_cal_default;
extern nv_voc_pcm_path_cal_type nv_voc_pcm_on_chip_1_cal_default;
extern nv_voc_pcm_path_cal_type nv_voc_pcm_on_chip_speaker_cal_default;
extern nv_qdsp_cmd_ec_params_type nv_voc_cal_ec_params_esec_default;
extern nv_qdsp_cmd_ec_params_type nv_voc_cal_ec_params_headset_default;
extern nv_qdsp_cmd_ec_params_type nv_voc_cal_ec_params_aec_default;
extern nv_qdsp_cmd_ec_params_type nv_voc_cal_ec_params_speaker_default;
#endif //FEATURE_NV_VOICE_PARAMETER

/*===========================================================================

                       STATIC/LOCAL FUNCTION DECLARATIONS

===========================================================================*/

#if defined(AEE_STATIC)

int     CSvcPrgMod_Load(IShell   *ps, 
                               void     *pHelpers, 
                               IModule **pMod);

static int     CSvcPrgMod_CreateInstance(AEECLSID  ClsId,
                                         IShell   *pIShell,
                                         IModule  *po,
                                         void    **ppObj);
#endif /* defined(AEE_STATIC) */


static boolean CSvcPrg_HandleEvent(CSvcPrgApp  *pMe, 
                                   AEEEvent     eCode, 
                                   uint16       wParam, 
                                   uint32       dwParam);

static void CSvcPrg_FreeAppData(CSvcPrgApp *pMe);

// Event handler functions
static boolean CSvcPrg_OnAppStart(CSvcPrgApp  *pMe, 
                                  uint16       wParam, 
                                  uint32       dwParam);

static boolean CSvcPrg_OnDialogStart(CSvcPrgApp  *pMe, 
                                     uint16       wParam, 
                                     uint32       dwParam);

static boolean CSvcPrg_OnCommand(CSvcPrgApp  *pMe, 
                                 uint16       wParam, 
                                 uint32       dwParam);

static boolean CSvcPrg_OnCommand_SecCodeOK(CSvcPrgApp *pMe, 
                                           IDialog    *pd);

static boolean CSvcPrg_OnCommand_Edit(IDialog      *pd,
                                      MenuItemType *pItem);

static boolean CSvcPrg_OnCommand_Save(CSvcPrgApp      *pMe, 
                                      IDialog         *pd,
                                      MenuItemType    *pItem);

static boolean CSvcPrg_OnCommand_AKEYEntry(CSvcPrgApp      *pMe, 
                                      IDialog         *pd,
                                      MenuItemType    *pItem);

static boolean CSvcPrg_OnCommand_YesNo(CSvcPrgApp      *pMe, 
                                       IDialog         *pd,
                                       MenuItemType    *pItem,
                                       boolean          bYes);

static boolean CSvcPrg_OnCommand_SID_OK(CSvcPrgApp      *pMe, 
                                        IDialog         *pd,
                                        MenuItemType    *pItem);

static boolean CSvcPrg_OnCommand_NID_OK(CSvcPrgApp      *pMe, 
                                        IDialog         *pd,
                                        MenuItemType    *pItem);

static boolean CSvcPrg_OnRangeItemSelect(CSvcPrgApp    *pMe, 
                                         uint16         itemId,
                                         MenuItemType  *pItem);

static boolean CSvcPrg_OnSidNidItemSelect(CSvcPrgApp    *pMe, 
                                          uint16         itemId,
                                          MenuItemType  *pItem);

#ifdef WORKAROUND_IMENUCTL_DISABLE_UPDOWN_KEY_EVENT
static boolean CSvcPrg_OnKeyPress(CSvcPrgApp  *pMe, 
                                  AVKType      key);
#endif /* WORKAROUND_IMENUCTL_DISABLE_UPDOWN_KEY_EVENT */

static boolean CSvcPrg_OnKey(CSvcPrgApp  *pMe, 
                             AVKType      key);

// Helper functions
static boolean CSvcPrg_BuildMenuList(CSvcPrgApp *pMe);

static boolean CSvcPrg_BuildLockMenuList(CSvcPrgApp *pMe);
static MenuItemType *CSvcPrg_CreateMenuList(uint16 numItems);

static void CSvcPrg_FreeMenuList(MenuItemType *itemList);

static RangeItemType *CSvcPrg_CreateRange(uint16 numItems);

static boolean CSvcPrg_DisplayItem(CSvcPrgApp      *pMe, 
                                   MenuItemType    *item);

static void CSvcPrg_LoadTextCtlWithItemVal(CSvcPrgApp      *pMe, 
                                           ITextCtl        *pt, 
                                           MenuItemType    *item);

static void CSvcPrg_SaveItemValue(CSvcPrgApp      *pMe,
                                  MenuItemType    *item,
                                  AECHAR          *wNewVal);

static IDialog *CSvcPrg_CreateDialog(CSvcPrgApp *pMe, uint16 dlgId);

static void CSvcPrg_EndDialog(CSvcPrgApp *pMe, boolean bEndKeyPressed);

static boolean CSvcPrg_DisplaySecCodeDialog(CSvcPrgApp *pMe);

static boolean CSvcPrg_DisplayMessageDialog(CSvcPrgApp *pMe, uint16 resId);

//static boolean CSvcPrg_CheckSecCode(CSvcPrgApp *pMe, AECHAR const *code);

static int CSvcPrg_CheckLockCode(CSvcPrgApp *pMe, AECHAR const *code);

static void CSvcPrg_MessageDialog_TimerCB(CSvcPrgApp *pMe);

static void CSvcPrg_DisplayTextMsg(CSvcPrgApp *pMe, uint16 resId);

static boolean CSvcPrg_SaveSIDNIDPair(CSvcPrgApp      *pMe, 
                                      MenuItemType    *pItem, 
                                      uint16           sid, 
                                      uint16           nid);

extern void hw_reset( void );
extern void OEMRTC_Phone_Reset(void *ppp);

/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

#if defined(AEE_STATIC)

/*=============================================================================
FUNCTION: CSvcPrgMod_Load

DESCRIPTION:  Load the module into memory

PARAMETERS:
   *ps: 
   *pHelpers: 
   **pMod: 

RETURN VALUE:
   int: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CSvcPrgMod_Load(IShell   *ps, 
                           void     *pHelpers, 
                           IModule **pMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),
                           ps,
                           pHelpers,
                           pMod,
                           (PFNMODCREATEINST) CSvcPrgMod_CreateInstance,
                           (PFNFREEMODDATA) NULL);
}

#endif /* defined(AEE_STATIC) */


/*=============================================================================
FUNCTION: CSvcPrgMod_CreateInstance

DESCRIPTION:  Called by BREW to create an instance of an object contained
              in this module.

PARAMETERS:
   ClsId: 
   *pIShell: 
   *po: 
   **ppObj: 

RETURN VALUE:
   int: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#if defined(AEE_STATIC)
static int CSvcPrgMod_CreateInstance(AEECLSID  ClsId,
                                     IShell   *pIShell,
                                     IModule  *po,
                                     void    **ppObj)
#else
int AEEClsCreateInstance(AEECLSID  ClsId,
                         IShell   *pIShell,
                         IModule  *po,
                         void    **ppObj)
#endif
{
   *ppObj = NULL;
   do {
         CSvcPrgApp *pMe = (CSvcPrgApp *) *ppObj;
   		if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
   		{
		   	return EFAILED;
		}
   	}while (0);

   if (AEEApplet_New(sizeof(CSvcPrgApp), 
                      ClsId, 
                      pIShell,
                      po,
                      (IApplet**)ppObj,
                      (AEEHANDLER) CSvcPrg_HandleEvent,
                      (PFNFREEAPPDATA) CSvcPrg_FreeAppData)) {

      do {
         CSvcPrgApp *pMe = (CSvcPrgApp *) *ppObj;

         if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                  AEECLSID_CONFIG,
                                                  (void **) &pMe->m_pConfig)) {
            break;
         }

         return AEE_SUCCESS;
      } while (0); /*lint !e717*/ // suppress 'Info 717: do .. while (0);'

      IAPPLET_Release(((IApplet *) *ppObj));
      *ppObj = NULL;
   }
   return ENOMEMORY;
}


/*=============================================================================
FUNCTION: CSvcPrg_HandleEvent

DESCRIPTION:  Service Programming Applet event handler

PARAMETERS:
   *pMe: CSvcPrgApp object pointer
   eCode: Event code
   wParam: word event parameter
   dwParam: dword event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_HandleEvent(CSvcPrgApp *pMe, 
                                   AEEEvent    eCode, 
                                   uint16      wParam, 
                                   uint32      dwParam)
{
   switch (eCode) {
   case EVT_APP_START:
   	  IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);   
      return CSvcPrg_OnAppStart(pMe, wParam, dwParam);

   case EVT_APP_STOP:
      return TRUE;

   case EVT_DIALOG_INIT:
      return TRUE;

   case EVT_DIALOG_START:
      return CSvcPrg_OnDialogStart(pMe, wParam, dwParam);

   case EVT_DIALOG_END:
      return TRUE;
   
   case EVT_COMMAND:
      return CSvcPrg_OnCommand(pMe, wParam, dwParam);

#ifdef WORKAROUND_IMENUCTL_DISABLE_UPDOWN_KEY_EVENT
   case EVT_KEY_PRESS:
      return CSvcPrg_OnKeyPress(pMe, (AVKType) wParam);
#endif /* WORKAROUND_IMENUCTL_DISABLE_UPDOWN_KEY_EVENT */

   case EVT_KEY:
    if(wParam == AVK_ENDCALL)
    {
       if (pMe->m_resetOnExit) {
	  EnableUIKeys(FALSE);
        CSvcPrg_DisplayTextMsg(pMe, IDS_PHONE_RESETTING);
        OEMRTC_Phone_Reset(pMe->a.m_pIShell);
      } 
     else {
      (void) ISHELL_CloseApplet(pMe->a.m_pIShell, TRUE);
      }	   
	return TRUE;

   }
      return CSvcPrg_OnKey(pMe, (AVKType) wParam);
      
   case EVT_BUSY:
      // Return TRUE to EVT_BUSY once we are offline to prevent any other 
      // applets from starting up
      return pMe->m_resetOnExit;

   default:
      break;
   }
   return FALSE;
}


/*=============================================================================
FUNCTION: CSvcPrg_FreeAppData

DESCRIPTION:  Free applet resources

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CSvcPrg_FreeAppData(CSvcPrgApp *pMe)
{
   CSvcPrg_FreeMenuList(pMe->m_mainMenu);
   CSvcPrg_FreeMenuList(pMe->m_lockMenu);

   if (pMe->m_pConfig) {
      ICONFIG_Release(pMe->m_pConfig);
   }

   if (pMe->m_pDisplay) {
      IDISPLAY_Release(pMe->m_pDisplay);
   }
   if(pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    }
}


/*=============================================================================
FUNCTION: CSvcPrg_OnAppStart

DESCRIPTION:  EVT_APP_START event handler

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   wParam: word event parameter
   dwParam: dword event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnAppStart(CSvcPrgApp  *pMe, 
                                  uint16       wParam, 
                                  uint32       dwParam)
{
   PARAM_NOT_REF(wParam)
   AEEAppStart  *a = (AEEAppStart *) dwParam;

   pMe->m_rc = a->rc;

   pMe->m_pDisplay = a->pDisplay;
   IDISPLAY_AddRef(pMe->m_pDisplay);
   

   return CSvcPrg_DisplaySecCodeDialog(pMe);
}


/*=============================================================================
FUNCTION: CSvcPrg_OnDialogStart

DESCRIPTION:  EVT_DIALOG_START event handler

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   wParam: word event parameter
   dwParam: dword event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnDialogStart(CSvcPrgApp  *pMe, 
                                     uint16       wParam, 
                                     uint32       dwParam)
{
   if (IDD_GENERICTEXTSK == wParam) {
      // Get rid of the annoying Multi-tap softkey and the automatic
      // association between the softkey and text controls.  
      IMenuCtl *pm;
      ITextCtl *pt;
      
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam, 
                                           IDC_GENERICTEXTSK_SK);
      pt = (ITextCtl *) IDIALOG_GetControl((IDialog *) dwParam, 
                                           IDC_GENERICTEXTSK_TEXT);
      if (!pm || !pt) {
         return FALSE;
      }

      (void) IMENUCTL_DeleteAll(pm);
      ITEXTCTL_SetSoftKeyMenu(pt, NULL);


   } else if (IDD_GENERICMENU == wParam) {
      IMenuCtl *pm;
      
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam, 
                                           IDC_GENERICMENU_MENU);
      if (!pm) {
         return FALSE;
      }
      
   }
   return TRUE;
}


/*=============================================================================
FUNCTION: CSvcPrg_OnCommand

DESCRIPTION:  EVT_COMMAND event handler

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   wParam: word event parameter
   dwParam: dword event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnCommand(CSvcPrgApp  *pMe, 
                                 uint16       wParam, 
                                 uint32       dwParam)
{
   IMenuCtl     *pCmdMenu;
   IDialog      *pDialog;
   MenuItemType *pItem;
   uint32 wDlgID;

  
   // There should be nobody but IMenuCtl objects sending us
   // EVT_COMMANDS so this typecast should be safe.
   pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if (pDialog == NULL) { /* error */
     return FALSE;
   }
   wDlgID = IDIALOG_GetID(pDialog);
  
   switch(wDlgID)
   {
     case IDD_GENERICMENU:
      pCmdMenu = (IMenuCtl *) IDIALOG_GetControl(pDialog, IDC_GENERICMENU_MENU);
      break;
     case IDD_GENERICTEXTSK:
       pCmdMenu = (IMenuCtl *) IDIALOG_GetControl(pDialog, IDC_GENERICTEXTSK_SK);
     break;
     case IDD_SECCODE:
       pCmdMenu = (IMenuCtl*) IDIALOG_GetControl(pDialog, IDC_SECCODE_TEXT);
       break;
     default:
       MSG_ERROR("Bad Dlg",0,0,0);
       return FALSE;
   }

   // The item data from the IMenuCtl object is always a MenuItemType (assuming it's not NULL)
   if (!IMENUCTL_GetItemData(pCmdMenu, 
                             wParam, 
                             (uint32 *) (void *)&pItem)) {
      return FALSE;
   }

   // Retrieve an interface to the active dialog 
   pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if (!pDialog) {
      return FALSE;
   }


   switch (wParam) {
   case IDS_CMD_SECCODE_OK:
      return CSvcPrg_OnCommand_SecCodeOK(pMe, pDialog);

   case IDS_CMD_EDIT:
      return CSvcPrg_OnCommand_Edit(pDialog, pItem);

   case IDS_CMD_SAVE:
      return CSvcPrg_OnCommand_Save(pMe, pDialog, pItem);

   case IDS_CMD_YES:
      return CSvcPrg_OnCommand_YesNo(pMe, pDialog, pItem, TRUE);

   case IDS_CMD_NO:
      return CSvcPrg_OnCommand_YesNo(pMe, pDialog, pItem, FALSE);

   case IDS_CMD_SID_OK:
      return CSvcPrg_OnCommand_SID_OK(pMe, pDialog, pItem);

   case IDS_CMD_NID_OK:
      return CSvcPrg_OnCommand_NID_OK(pMe, pDialog, pItem);

   case IDS_CMD_NID_CANCEL:
      // Need to close two dialogs (the NID and then the SID dialog)
      CSvcPrg_EndDialog(pMe, FALSE);
      //lint -fallthrough

   case IDS_CMD_OK:
   case IDS_CMD_CANCEL:
      // OK and Cancel softkey simply exit the current dialog and
      // return to the previous one.
      CSvcPrg_EndDialog(pMe, FALSE);
      return TRUE;

   case IDS_CMD_AKEY:
     // AKEY entry 
     return CSvcPrg_OnCommand_AKEYEntry (pMe, pDialog, pItem);

   default:
      if (wParam & RANGE_ITEMID_FLAG) {
         return CSvcPrg_OnRangeItemSelect(pMe,  
                                      wParam ^ RANGE_ITEMID_FLAG, 
                                      pItem);

      } else if (wParam & SIDNID_ITEMID_FLAG) {
         return CSvcPrg_OnSidNidItemSelect(pMe, 
                                       wParam ^ SIDNID_ITEMID_FLAG, 
                                       pItem);

      } else {
         // Any other EVT_COMMAND the applet receives will be a generic 
         // menu command...  
         return CSvcPrg_DisplayItem(pMe, pItem);
      }
   }
}


/*=============================================================================
FUNCTION: CSvcPrg_OnCommand_SecCodeOK

DESCRIPTION:  Command event handler for IDS_CMD_SECCODE_OK

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   *pd: IDialog interface for the active dialog 

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnCommand_SecCodeOK(CSvcPrgApp *pMe, 
                                           IDialog    *pd)
{
   ITextCtl *pt; 
#ifdef FEATURE_ICM
   //ICM *pICM = NULL;
   //int nReturnStatus;
#endif

   if (IDIALOG_GetID(pd) != IDD_SECCODE) {
      return FALSE;
   }

   pt = (ITextCtl *) IDIALOG_GetControl(pd,
                                        IDC_SECCODE_TEXT);
   if (!pt) {
      return FALSE;
   }
   pMe->m_ShowMainMenu = CSvcPrg_CheckLockCode(pMe, ITEXTCTL_GetTextPtr(pt));
   if (pMe->m_ShowMainMenu == 1) {
      MenuItemType  fakeItem;

      // Build the main menu
      if (!CSvcPrg_BuildMenuList(pMe)) {
         return FALSE;
      }

      // Set the phone offline
/*
#ifdef FEATURE_ICM
      // Create the Call Manager object. 
      nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                AEECLSID_CM,
                                (void **) &pICM);

      if(nReturnStatus != SUCCESS) {
        return FALSE;
      }

      if (pICM == NULL) { 
        return FALSE;
      }
      ICM_SetOperatingMode(pICM, AEECM_OPRT_MODE_OFFLINE);
      ICM_Release(pICM);
      pICM = NULL;
#else
      (void) ui_set_ph_oprt_mode (SYS_OPRT_MODE_OFFLINE);
#endif
*/
      pMe->m_resetOnExit = TRUE;

      // Close the security code dialog
      (void) ISHELL_EndDialog(pMe->a.m_pIShell);

      // Display the main menu...
      MEMSET(&fakeItem, 0, sizeof(fakeItem));

      fakeItem.title            = IDS_SERVICE_OPTIONS; 
      fakeItem.itemType         = DT_SUBMENU;
      fakeItem.typeData.subMenu = pMe->m_mainMenu;
      fakeItem.isLast           = TRUE;

      return CSvcPrg_DisplayItem(pMe, &fakeItem);
   }
   else if(pMe->m_ShowMainMenu == 2) 
   {
   	  MenuItemType  lockItem;

      // Build the main menu
      if (!CSvcPrg_BuildLockMenuList(pMe)) {
         return FALSE;
      }
      pMe->m_resetOnExit = TRUE;

      // Close the security code dialog
      (void) ISHELL_EndDialog(pMe->a.m_pIShell);

      // Display the main menu...
      MEMSET(&lockItem, 0, sizeof(lockItem));

      lockItem.title            = IDS_OPERATOR_OPTIONS; 
      lockItem.itemType         = DT_SUBMENU;
      lockItem.typeData.subMenu = pMe->m_lockMenu;
      lockItem.isLast           = TRUE;

      return CSvcPrg_DisplayItem(pMe, &lockItem);
   	
   	}
    else {

      // Reset the controls for the next time...
      (void) ITEXTCTL_SetText(pt, NULL, -1);
      (void) IDIALOG_SetFocus(pd, IDC_SECCODE_TEXT);

      // Display the Invalid Security Code error message
      return CSvcPrg_DisplayMessageDialog(pMe, IDS_INVALID_SECCODE);
   }
}


/*=============================================================================
FUNCTION: CSvcPrg_OnCommand_Edit

DESCRIPTION:  Command event handler for IDS_CMD_EDIT

PARAMETERS:
   *pd: IDialog interface for the active dialog 
   *pItem: menu item

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnCommand_Edit(IDialog      *pd,
                                      MenuItemType *pItem)
{
   IMenuCtl *pm;
   ITextCtl *pt;

   if (IDIALOG_GetID(pd) != IDD_GENERICTEXTSK) {
      return FALSE;
   }

   pm = (IMenuCtl *) IDIALOG_GetControl(pd,
                                        IDC_GENERICTEXTSK_SK);
   pt = (ITextCtl *) IDIALOG_GetControl(pd,
                                        IDC_GENERICTEXTSK_TEXT);
   if (!pm || !pt) {
      return FALSE;
   }

   // Add the association between the softkey and the text control
   // _before_ deleting all the menu items so the 'Multitap' item
   // is removed too
   ITEXTCTL_SetSoftKeyMenu(pt, pm);
   // 有些设置项需输入字母，在这里设置不便控制，改在文本控件初始化时设置
   //(void) ITEXTCTL_SetInputMode(pt, AEE_TM_NUMBERS);

   (void) IMENUCTL_DeleteAll(pm);
   (void) IMENUCTL_AddItem(pm, 
                           AEE_SVCPRG_RES_FILE,
                           IDS_CMD_SAVE,
                           IDS_CMD_SAVE,
                           NULL,
                           (uint32) pItem);
   (void) IMENUCTL_AddItem(pm, 
                           AEE_SVCPRG_RES_FILE,
                           IDS_CMD_CANCEL,
                           IDS_CMD_CANCEL,
                           NULL,
                           (uint32) pItem);

   // Set the focus to the text control object
   (void) IDIALOG_SetFocus(pd,
                           IDC_GENERICTEXTSK_TEXT);
   return TRUE;
}


/*=============================================================================
FUNCTION: CSvcPrg_OnCommand_Save

DESCRIPTION: Command event handler for IDS_CMD_SAVE

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   *pd: IDialog interface for the active dialog 
   *pItem: menu item

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnCommand_Save(CSvcPrgApp      *pMe, 
                                      IDialog         *pd,
                                      MenuItemType    *pItem)
{
   ITextCtl *pt;
   AECHAR   *pText;
   int       len;

   if (NULL == pItem) {
      return FALSE;
   }

   if (IDIALOG_GetID(pd) != IDD_GENERICTEXTSK) {
      return FALSE;
   }

   pt = (ITextCtl *) IDIALOG_GetControl(pd,
                                        IDC_GENERICTEXTSK_TEXT);
   if (!pt) {
      return FALSE;
   }

   pText = ITEXTCTL_GetTextPtr(pt);

   len = WSTRLEN(pText);
   
   if (0 == len) {
      // Exit immediately if the text control is empty.
      return CSvcPrg_DisplayMessageDialog(pMe, IDS_NOTHING_ENTERED);
   }

   if ( (DT_WSTR_FIXED == pItem->itemType) &&
        (len != pItem->typeData.strLen-1) ) {
      // The user has not entered enough characters yet
      return CSvcPrg_DisplayMessageDialog(pMe, IDS_TOO_SHORT);
   }

   // "MIN Lock" just for Venezuela MOVILNET
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
      if(pItem->cfgItem == CFGI_IMSI_S)
      {
         char temp[15];
         WSTRTOSTR(pText, temp, 15);
         
         temp[3] = 0;
         if (!((STRCMP(temp, "158") == 0) ||
                (STRCMP(temp, "199") == 0)))
          {
                   (void) ITEXTCTL_SetText(pt, NULL, -1);
                   (void) IDIALOG_SetFocus(pd, IDC_GENERICTEXTSK_TEXT);
                   return CSvcPrg_DisplayMessageDialog(pMe, IDS_INVALID_MIN);
         }
      }
#endif //FEATURE_CARRIER_VENEZUELA_MOVILNET

   // Save the new value
   CSvcPrg_SaveItemValue(pMe, 
                     pItem,
                     pText);

   // Return to the previous dialog
   CSvcPrg_EndDialog(pMe, FALSE);
   return TRUE;
}


/*=============================================================================
FUNCTION: CSvcPrg_OnCommand_AKEYEntry

DESCRIPTION: Command event handler for IDS_AKEY_SUBMIT

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   *pd: IDialog interface for the active dialog 
   *pItem: menu item

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnCommand_AKEYEntry(CSvcPrgApp      *pMe, 
                                      IDialog         *pd,
                                      MenuItemType    *pItem)
{
   ITextCtl *pt;
   AECHAR   *pText;
   int       len;
#if defined (FEATURE_AUTH)
   char     pszBuf[MAX_AKEY_DIGITS+1];
   int i,j;
#endif
   int nam;
#ifdef FEATURE_ICM
   int nReturnStatus;
   ICM  *pICM;
   AEECMPhInfo phInfo;
#endif

   if (NULL == pItem) {
      return FALSE;
   }

   if (IDIALOG_GetID(pd) != IDD_GENERICTEXTSK) {
      return FALSE;
   }

   pt = (ITextCtl *) IDIALOG_GetControl(pd,
                                        IDC_GENERICTEXTSK_TEXT);
   if (!pt) {
      return FALSE;
   }

   pText = ITEXTCTL_GetTextPtr(pt);

   len = WSTRLEN(pText);
   
   if (0 == len) {
      // Exit immediately if the text control is empty.
      return CSvcPrg_DisplayMessageDialog(pMe, IDS_NOTHING_ENTERED);
   }

#ifdef FEATURE_ICM
      /* Create the Call Manager object. */
      nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                AEECLSID_CM,
                                (void **) &pICM);

      if(nReturnStatus != SUCCESS) {
        return FALSE;
      }

      if (pICM == NULL) { /* error */
        return FALSE;
      }
      ICM_GetPhoneInfo(pICM, &phInfo, sizeof(phInfo));
      nam = phInfo.curr_nam;
      ICM_Release(pICM);
      pICM = NULL;
#else
      nam = ui.nam;
#endif

#if defined (FEATURE_AUTH)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (!nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    if ( len >= MIN_AKEY_DIGITS )
    { 
      WSTRTOSTR (pText, pszBuf, ARR_SIZE(pszBuf));
      /* min digits */
      /* Fill with leading zeros */
      for ( i=len-1,j=MAX_AKEY_DIGITS-1; j >= 0 && i<(MAX_AKEY_DIGITS - 1); )
      {
        if ( i>= 0)
        {
          pszBuf[j--] = pszBuf[i--]; /* shift digits */
        }
        else
        {
          pszBuf[j--] = '0';       /* fill with leading zeros */
        }
      } /* Fill with leading zeros */
      pszBuf[MAX_AKEY_DIGITS]=0; /* null termination */

      /* Verify AKEY entry */
      if ( auth_validate_a_key( (byte*)pszBuf ) )
      {
        auth_cmd_type *auth_ptr;
        /* valid AKEY */
        if( ( auth_ptr = (auth_cmd_type *)(q_get( &auth_free_q ))) == NULL )
        {
          /* could not get a buffer : try again */
          MSG_ERROR( " ... couldn't store AKEY", 0, 0, 0 );
          // Exit immediately if the text control is empty.
          return CSvcPrg_DisplayMessageDialog(pMe, IDS_AKEY_FAILED);
        }
        /* tell auth to update AKEY */
        auth_ptr->hdr.command = AUTH_UPDATE_A_KEY_F;
        auth_ptr->hdr.cmd_hdr.task_ptr = NULL;
        MEMCPY( auth_ptr->a_key.a_key, pszBuf, MAX_AKEY_DIGITS );
        auth_ptr->a_key.nam = nam;
        auth_ptr->a_key.rpt_function = NULL;
        auth_cmd ( auth_ptr );
        // Save the new value
        CSvcPrg_EndDialog (pMe, FALSE);
        return CSvcPrg_DisplayMessageDialog(pMe, IDS_AKEY_SUCCESS); 
      } /* if ( auth_validate_a_key( akey_num.buf ) ) */
      /* if AKEY is not validated we fall through */
    } /* if ( pszBuf >= MIN_AUTH_DIGITS ) */
  }
#endif /* F_AUTH */
  /* Fall through if invalid AKEY */
  return CSvcPrg_DisplayMessageDialog(pMe, IDS_AKEY_FAILED); 
}


/*=============================================================================
FUNCTION: CSvcPrg_OnCommand_YesNo

DESCRIPTION: Command event handler for IDS_CMD_YES and IDS_CMD_NO

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   *pd: IDialog interface for the active dialog 
   *pItem: menu item
   bYes: TRUE if IDS_CMD_YES, FALSE if IDS_CMD_NO

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnCommand_YesNo(CSvcPrgApp      *pMe, 
                                       IDialog         *pd,
                                       MenuItemType    *pItem,
                                       boolean          bYes)
{
   if (NULL == pItem) {
      return FALSE;
   }
   
   if (IDIALOG_GetID(pd) != IDD_GENERICMENU) {
      return FALSE;
   }

   if ((pItem->itemType != DT_BOOLEAN)
#ifdef FEATURE_NV_VOICE_PARAMETER    
    && (pItem->itemType != DT_BOOLEAN_VOCCAL)
#endif //FEATURE_NV_VOICE_PARAMETER   
    ) 
    {
      return FALSE;
   }

   if(pItem->itemType == DT_BOOLEAN)
   {
       (void) ICONFIG_SetItem(pMe->m_pConfig, 
                              pItem->cfgItem,
                              &bYes,
                              sizeof(bYes));
   }
#ifdef FEATURE_NV_VOICE_PARAMETER    
   else if(pItem->itemType == DT_BOOLEAN_VOCCAL)
   {
        nv_item_type nvi;
        
        if (NV_DONE_S != OEMNV_Get(pItem->nvitem,&nvi))
        {
            ERR("Unable to retrieve config item: %d", 
                (int) pItem->nvitem, 0, 0);
            return FALSE;            
        }
         
        if(IDS_VOCCAL_USE_NVMODE == pItem->title)
        {
            switch(pItem->nvitem)
            {
                case NV_VOC_PCM_ON_CHIP_0_CAL_I:
                    nvi.nv_voc_pcm_on_chip_0_cal.use_nvmode = bYes;
                    if(!bYes)
                    {
                        nvi.nv_voc_pcm_on_chip_0_cal = nv_voc_pcm_on_chip_0_cal_default;
                    }
                    break;

                case NV_VOC_PCM_ON_CHIP_1_CAL_I:
                    nvi.nv_voc_pcm_on_chip_1_cal.use_nvmode = bYes;
                    if(!bYes)
                    {
                        nvi.nv_voc_pcm_on_chip_1_cal = nv_voc_pcm_on_chip_1_cal_default;
                    }                        
                    break;

                case NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I:
                    nvi.nv_voc_pcm_on_chip_speaker_cal.use_nvmode = bYes;
                    if(!bYes)
                    {
                        nvi.nv_voc_pcm_on_chip_speaker_cal = nv_voc_pcm_on_chip_speaker_cal_default;
                    }                        
                    break;   

                case NV_VOC_CAL_EC_PARAMS_ESEC_I:
                    nvi.nv_voc_cal_ec_params_esec.use_nvmode = bYes;
                    if(!bYes)
                    {
                        nvi.nv_voc_cal_ec_params_esec = nv_voc_cal_ec_params_esec_default;
                    }                        
                    break;                       

                case NV_VOC_CAL_EC_PARAMS_HEADSET_I:
                    nvi.nv_voc_cal_ec_params_headset.use_nvmode = bYes;
                    if(!bYes)
                    {
                        nvi.nv_voc_cal_ec_params_headset = nv_voc_cal_ec_params_headset_default;
                    }                        
                    break;    
                    
                case NV_VOC_CAL_EC_PARAMS_AEC_I:
                    nvi.nv_voc_cal_ec_params_aec.use_nvmode = bYes;
                    if(!bYes)
                    {
                        nvi.nv_voc_cal_ec_params_aec = nv_voc_cal_ec_params_aec_default;
                    }                        
                    break; 

                case NV_VOC_CAL_EC_PARAMS_SPEAKER_I:
                    nvi.nv_voc_cal_ec_params_speaker.use_nvmode = bYes;
                    if(!bYes)
                    {
                        nvi.nv_voc_cal_ec_params_speaker = nv_voc_cal_ec_params_speaker_default;
                    }                        
                    break; 

                default:
                    break;  
            }
           
            if (NV_DONE_S != OEMNV_Put(pItem->nvitem,&nvi))
            {
                ERR("Unable to save config item: %d", 
                    (int) pItem->nvitem, 0, 0);
            }  
        }
   }
#endif //FEATURE_NV_VOICE_PARAMETER 

   CSvcPrg_EndDialog(pMe, FALSE);
   return TRUE;
}


/*=============================================================================
FUNCTION: CSvcPrg_OnCommand_SID_OK

DESCRIPTION:  Command event handler for IDS_CMD_SID_OK

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   *pd: IDialog interface for the active dialog 
   *pItem: menu item

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnCommand_SID_OK(CSvcPrgApp      *pMe, 
                                        IDialog         *pd,
                                        MenuItemType    *pItem)
{
   IMenuCtl *pm;
   ITextCtl *pt;

   ASSERT(DT_SIDNIDLIST == pItem->itemType);

   if (IDIALOG_GetID(pd) != IDD_GENERICTEXTSK) {
      return FALSE;
   }

   pt = (ITextCtl *) IDIALOG_GetControl(pd,
                                        IDC_GENERICTEXTSK_TEXT);
   if (!pt) {
      return FALSE;
   }

   // Return immediately if the SID length is zero, the 
   // user needs to enter something first
   if (0 == WSTRLEN(ITEXTCTL_GetTextPtr(pt))) {
      return CSvcPrg_DisplayMessageDialog(pMe, IDS_NOTHING_ENTERED);
   }
#if defined FEATURE_LOCK 
   if ( pItem->cfgItem == CFGI_LOCK_MCCMNC_LIST )
   {
      if ( WSTRLEN(ITEXTCTL_GetTextPtr(pt)) > 3 ) 
      {
         return CSvcPrg_DisplayMessageDialog(pMe, IDS_MCC_LIMIT);
      }
   }
#endif   
   //
   // Display Edit NID Dialog
   //
   pd = CSvcPrg_CreateDialog(pMe, IDD_GENERICTEXTSK);
   if (NULL == pd) {
      return FALSE;
   }

   pm = (IMenuCtl *) IDIALOG_GetControl(pd,IDC_GENERICTEXTSK_SK);
   pt = (ITextCtl *) IDIALOG_GetControl(pd,IDC_GENERICTEXTSK_TEXT);
   if (!pm || !pt) {
      return FALSE;
   }
#if defined FEATURE_LOCK
   if ( pItem->cfgItem == CFGI_LOCK_MCCMNC_LIST )
   {
      (void) ITEXTCTL_SetTitle(pt,
                                AEE_SVCPRG_RES_FILE,
                                IDS_EDIT_MNC,
                                NULL);
   }
   else
#endif
   {
       (void) ITEXTCTL_SetTitle(pt,
                                AEE_SVCPRG_RES_FILE,
                                IDS_EDIT_NID,
                                NULL);
    }
    
   ITEXTCTL_SetSoftKeyMenu(pt, pm);
   (void) ITEXTCTL_SetInputMode(pt, AEE_TM_NUMBERS);
   (void) IMENUCTL_DeleteAll(pm);

   (void) IMENUCTL_AddItem(pm, 
                           AEE_SVCPRG_RES_FILE,
                           IDS_CMD_NID_OK,
                           IDS_CMD_NID_OK,
                           NULL,
                           (uint32) pItem);

   (void) IMENUCTL_AddItem(pm, 
                           AEE_SVCPRG_RES_FILE,
                           IDS_CMD_NID_CANCEL,
                           IDS_CMD_NID_CANCEL,
                           NULL,
                           0);

   (void) IDIALOG_SetFocus(pd,
                           IDC_GENERICTEXTSK_TEXT);

   (void) ITEXTCTL_Redraw(pt);
   (void) IMENUCTL_Redraw(pm);
   return TRUE;
}

/*=============================================================================
FUNCTION: CSvcPrg_OnCommand_NID_OK

DESCRIPTION: Command event handler for IDS_CMD_NID_OK

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   *pd: IDialog interface for the active dialog 
   *pItem: menu item

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnCommand_NID_OK(CSvcPrgApp      *pMe, 
                                        IDialog         *pd,
                                        MenuItemType    *pItem)
{
   AECHAR wNID[6],   // 6 = max number of digits in a 16-bit
          wSID[6];   //     decimal number plus a NULL character
   char   szBuf[6];
   uint16 nid, 
          sid;
   int    i;

   ITextCtl *pt;

   ASSERT(DT_SIDNIDLIST == pItem->itemType);

   wNID[0] = (AECHAR) 0; // pacify Lint by initializing these vars
   wSID[0] = (AECHAR) 0; // even though it's not required.

   //
   // Get the new NID and SID values
   // 
   for (i = 0; i < 2; i++) {
      
      // Already have the active dialog the first time around this loop
      // (ie. the value passed into this function)
      if (i != 0) {
         pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
         if (!pd) {
            return FALSE;
         }
      }

      // This better be a 'generic text' dialog
      if (IDIALOG_GetID(pd) != IDD_GENERICTEXTSK) {
         return FALSE;
      }

      pt = (ITextCtl *) IDIALOG_GetControl(pd,
                                           IDC_GENERICTEXTSK_TEXT);
      if (!pt) {
         return FALSE;
      }

      if (0 == i) {
         (void) ITEXTCTL_GetText(pt, 
                                 wNID, 
                                 sizeof(wNID)/sizeof(wNID[0]));

         // Return immediately if the NID length is zero, the 
         // user needs to enter something first
         if (0 == WSTRLEN(wNID)) {
            return CSvcPrg_DisplayMessageDialog(pMe, 
                                            IDS_NOTHING_ENTERED);
         }
#if defined FEATURE_LOCK
         if ( pItem->cfgItem == CFGI_LOCK_MCCMNC_LIST )
         {
            if ( WSTRLEN(ITEXTCTL_GetTextPtr(pt)) > 2 ) 
            {
               return CSvcPrg_DisplayMessageDialog(pMe, IDS_MNC_LIMIT);
            }
         }
#endif
      } else {
         (void) ITEXTCTL_GetText(pt, 
                                 wSID, 
                                 sizeof(wSID)/sizeof(wSID[0]));
      }

      // Close the NID dialog (when i == 0) or SID dialog (when i == 1)
      CSvcPrg_EndDialog(pMe, FALSE);
   }

   WSTR_TO_STR(wSID, szBuf, sizeof(szBuf));
   sid = (uint16) ATOI(szBuf);

   WSTR_TO_STR(wNID, szBuf, sizeof(szBuf));
   nid = (uint16) ATOI(szBuf);

   if (!CSvcPrg_SaveSIDNIDPair(pMe, pItem, sid, nid)) {
      return FALSE;
   }

   // Close the SID/NID menu list dialog and open it again
   // to reload the changed data...
   CSvcPrg_EndDialog(pMe, FALSE);
   return CSvcPrg_DisplayItem(pMe, pItem);
}


/*=============================================================================
FUNCTION: CSvcPrg_OnRangeItemSelect

DESCRIPTION:  Command event handler for a range selection (DT_RANGE type)

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   itemId: Selected item id
   *pItem: Range menu item

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnRangeItemSelect(CSvcPrgApp      *pMe, 
                                         uint16           itemId,
                                         MenuItemType    *pItem)
{
   uint8 val;

   if (NULL == pItem) {
      return FALSE;
   }
   if (pItem->itemType != DT_RANGE) {
      return FALSE;
   }

   val = (uint8) (itemId & 0xFF);
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
   if (pItem->cfgItem == CFGI_RTRE_CONFIGURATION)
   {
       nv_rtre_configuration_type rtre_config=itemId;
       (void) ICONFIG_SetItem(pMe->m_pConfig, 
                              pItem->cfgItem,
                              &rtre_config,
                              sizeof(rtre_config));
   }
   else
   {
       (void) ICONFIG_SetItem(pMe->m_pConfig, 
                              pItem->cfgItem,
                              &val,
                              sizeof(val));
   }
#else
   (void) ICONFIG_SetItem(pMe->m_pConfig, 
                          pItem->cfgItem,
                          &val,
                          sizeof(val));
#endif

   CSvcPrg_EndDialog(pMe, FALSE);

   return TRUE;  
}


/*=============================================================================
FUNCTION: CSvcPrg_OnSidNidItemSelect

DESCRIPTION:  Command event handler for a sid/nid list 
              selection (DT_SIDNIDLIST type)

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   itemId:  selected item id
   *pItem: SID/NID List menu item

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnSidNidItemSelect(CSvcPrgApp   *pMe, 
                                          uint16        itemId,
                                          MenuItemType *pItem)
{
   IDialog  *pd;
   IMenuCtl *pm;
   ITextCtl *pt;

   ASSERT(DT_SIDNIDLIST == pItem->itemType);

   // Remember the item Id.  This will come in very handy when CSvcPrg_SaveSIDNIDPair()
   // is called from CSvcPrg_OnCommand_NID_OK()
   pItem->typeData.sidnid.currIndex = itemId; 

   //
   // Display Edit SID Dialog
   //
   pd = CSvcPrg_CreateDialog(pMe, IDD_GENERICTEXTSK);
   if (NULL == pd) {
      return FALSE;
   }
   pm = (IMenuCtl *) IDIALOG_GetControl(pd,IDC_GENERICTEXTSK_SK);
   pt = (ITextCtl *) IDIALOG_GetControl(pd,IDC_GENERICTEXTSK_TEXT);
   if (!pm || !pt) {
      return FALSE;
   }
#if defined FEATURE_LOCK
   if ( pItem->cfgItem == CFGI_LOCK_MCCMNC_LIST )
   {
      (void) ITEXTCTL_SetTitle(pt,
                                AEE_SVCPRG_RES_FILE,
                                IDS_EDIT_MCC,
                                NULL);
   }
   else
#endif
    {
       (void) ITEXTCTL_SetTitle(pt,
                                AEE_SVCPRG_RES_FILE,
                                IDS_EDIT_SID,
                                NULL);
    }
   ITEXTCTL_SetSoftKeyMenu(pt, pm);
   (void) ITEXTCTL_SetInputMode(pt, AEE_TM_NUMBERS);
   (void) IMENUCTL_DeleteAll(pm);

   (void) IMENUCTL_AddItem(pm, 
                           AEE_SVCPRG_RES_FILE,
                           IDS_CMD_SID_OK,
                           IDS_CMD_SID_OK,
                           NULL,
                           (uint32) pItem);

   (void) IMENUCTL_AddItem(pm, 
                           AEE_SVCPRG_RES_FILE,
                           IDS_CMD_CANCEL,
                           IDS_CMD_CANCEL,
                           NULL,
                           0);

   (void) IDIALOG_SetFocus(pd,
                           IDC_GENERICTEXTSK_TEXT);


   (void) ITEXTCTL_Redraw(pt);
   (void) IMENUCTL_Redraw(pm);

   return TRUE;
}

#ifdef WORKAROUND_IMENUCTL_DISABLE_UPDOWN_KEY_EVENT
/*=============================================================================
FUNCTION: CSvcPrg_OnKeyPress

DESCRIPTION:  EVT_KEY_PRESS event handler

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
    key: The key that was pressed

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnKeyPress(CSvcPrgApp  *pMe, 
                                  AVKType      key)
{
   switch (key) {
   case AVK_UP:
   case AVK_DOWN:
      {
         IDialog *pd;

         pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

         if ((pd != NULL) && (IDD_GENERICTEXTSK == IDIALOG_GetID(pd))) {
            IMenuCtl *pm;

            pm = (IMenuCtl *) IDIALOG_GetControl(pd,
                                                 IDC_GENERICTEXTSK_SK);
            ASSERT(pm != NULL);

            // Make sure the first softkey is the 'OK' softkey.  
            // Presence of the 'OK' softkey implies the user is not 
            // editing anything (ie, it's a read-only dialog)
            if (IDS_CMD_OK == IMENUCTL_GetItemID(pm, 0)) {

               // Enable the no-redraw property 
               IMENUCTL_SetProperties(pm, 
                                      MP_NO_REDRAW | 
                                      IMENUCTL_GetProperties(pm));

               // Deactivate the menu control so it won't process the 
               // AVK_UP/AVK_DOWN on the EVT_KEY event.
               IMENUCTL_SetActive(pm, FALSE);
            }
         }
         return TRUE;
      }

   default:
      break;
   }

   return FALSE;
}
#endif /* WORKAROUND_IMENUCTL_DISABLE_UPDOWN_KEY_EVENT */


/*=============================================================================
FUNCTION: CSvcPrg_OnKey

DESCRIPTION:  EVT_KEY event handler

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
    key: The key that was pressed

RETURN VALUE:
   boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_OnKey(CSvcPrgApp  *pMe, 
                             AVKType      key)
{
   switch (key) {
  // case AVK_END:
   case AVK_CLR:
      // Pressing CLR will end the current dialog 
      // (and possibly the entire applet) 
      CSvcPrg_EndDialog(pMe, FALSE);
      return TRUE;

#ifdef WORKAROUND_IMENUCTL_DISABLE_UPDOWN_KEY_EVENT
   case AVK_UP:
   case AVK_DOWN:
      {
         IDialog *pd;

         pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

         if ((pd != NULL) && (IDD_GENERICTEXTSK == IDIALOG_GetID(pd))) {
            IMenuCtl *pm;

            pm = (IMenuCtl *) IDIALOG_GetControl(pd,
                                                 IDC_GENERICTEXTSK_SK);
            ASSERT(pm != NULL);

            // Make sure the first softkey is the 'OK' softkey.  
            // Presence of the 'OK' softkey implies the user is not 
            // editing anything (ie, it's a read-only dialog)
            if (IDS_CMD_OK == IMENUCTL_GetItemID(pm, 0)) {
               // Remove the no-redraw property 
               IMENUCTL_SetProperties(pm, 
                                      ~MP_NO_REDRAW &
                                      IMENUCTL_GetProperties(pm));

               // Reactivate the menu control 
               IMENUCTL_SetActive(pm, TRUE);
            }
         }
         return TRUE;
      }
#endif /* WORKAROUND_IMENUCTL_DISABLE_UPDOWN_KEY_EVENT */
   case AVK_SELECT:
         {
         IDialog *pd;

         pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
	  if(pd)
	  {
              (void) IDIALOG_SetFocus(pd, IDC_SECCODE_SK);
	   }
   	  }
     return TRUE;
     //    case AVK_END:
      // Keep ending dialogs until there are no more, at which point
      // CSvcPrg_EndDialog() will exit the applet for us.
   //       MSG_ERROR("88888888888888888888",0,0,0);
     //  CSvcPrg_EndDialog(pMe, FALSE);
    //  return TRUE;

   default:
      break;
   }

   return FALSE;
}


/*=============================================================================
FUNCTION: CSvcPrg_BuildMenuList

DESCRIPTION:  Builds the applet menu list

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 

RETURN VALUE:
   boolean: TRUE if successful

COMMENTS:

SIDE EFFECTS:
   CSvcPrgApp::m_mainMenu is updated to point to the topmost menu

SEE ALSO:

=============================================================================*/
#ifndef FEATURE_CARRIER_VENEZUELA_MOVILNET
static boolean CSvcPrg_BuildMenuList(CSvcPrgApp *pMe)
{
   MenuItemType *m;
   int numItems = 7; // Number of items in the main menu.
   int idxBrew = 0;
   
#ifdef FEATURE_NV_VOICE_PARAMETER
   int voccalitem;
   int echocancel;
#endif //#ifdef FEATURE_NV_VOICE_PARAMETER
   
   pMe->m_mainMenu = NULL;
   //
   // Build the Main menu
   //
#ifdef FEATURE_CDMA_RX_DIVERSITY
   numItems++;
#endif

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
   numItems++;
#endif

#ifdef FEATURE_NV_VOICE_PARAMETER
   numItems++;
   numItems++;
#endif //#ifdef FEATURE_NV_VOICE_PARAMETER

   numItems++;// 添加了 IDS_BREW_CONFIG

#ifdef FEATURE_HDR
#error code not present
#endif

   m = CSvcPrg_CreateMenuList(numItems);  // 7 elements in the Main menu

   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_ESN;
   m[0].itemType           = DT_ESN;
   m[0].cfgItem            = CFGI_ESN; 

   m[1].title              = IDS_NETWORK_OPTIONS;
   m[1].itemType           = DT_SUBMENU;
   m[1].typeData.subMenu   = NULL;
   
   m[2].title              = IDS_SMS_OPTIONS;
   m[2].itemType           = DT_SUBMENU;
   m[2].typeData.subMenu   = NULL;

   m[3].title              = IDS_DATA_OPTIONS;
   m[3].itemType           = DT_SUBMENU;
   m[3].typeData.subMenu   = NULL;
   
   m[4].title              = IDS_SLOTCYCLEINDEX;
   m[4].itemType           = DT_BYTE;
   m[4].cfgItem            = CFGI_SLOTINDEX; 
   m[4].isEditable         = TRUE;

   m[5].title              = IDS_SPCCODE;
   m[5].itemType           = DT_WSTR_FIXED;
   m[5].cfgItem            = CFGI_SECCODE; 
   m[5].typeData.strLen    = OEMNV_SECCODE_LENGTH;
   m[5].isEditable         = TRUE;

   m[6].title             = IDS_AKEY;
   m[6].itemType          = DT_AKEY;
   m[6].typeData.strLen   = MAX_AKEY_DIGITS+1; /* add 1 to fix textctl */

   numItems = 7;

#ifdef FEATURE_CDMA_RX_DIVERSITY
   m[numItems].title              = IDS_CDMA_RX_DIVERSITY_ENABLED;
   m[numItems].itemType           = DT_BOOLEAN;
   m[numItems].cfgItem            = CFGI_CDMA_RX_DIVERSITY_CTL;
   m[numItems].isEditable         = TRUE;
   numItems++;
#endif
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
   m[numItems].title             = IDS_RTRE_SELECT;
   m[numItems].itemType          = DT_RANGE;
   m[numItems].cfgItem           = CFGI_RTRE_CONFIGURATION; 
   m[numItems].isEditable        = TRUE;

   m[numItems].typeData.rangeData = CSvcPrg_CreateRange(3);
   if (NULL == m[numItems].typeData.rangeData) {
      return FALSE;
   }
   m[numItems].typeData.rangeData[0].resId = IDS_RTRE_SELECT_NVONLY;
   m[numItems].typeData.rangeData[0].value = NV_RTRE_CONFIG_NV_ONLY;
   m[numItems].typeData.rangeData[1].resId = IDS_RTRE_SELECT_RUIMONLY;
   m[numItems].typeData.rangeData[1].value = NV_RTRE_CONFIG_RUIM_ONLY;
   m[numItems].typeData.rangeData[2].resId = IDS_RTRE_SELECT_BOTH;
   m[numItems].typeData.rangeData[2].value = NV_RTRE_CONFIG_RUIM_OR_DROP_BACK;
   ERR("F numItems:= %d", numItems, 0, 0);
   numItems++;
   ERR("S numItems:= %d", numItems, 0, 0);
#endif

#ifdef FEATURE_NV_VOICE_PARAMETER
   m[numItems].title              = IDS_VOCCAL;
   m[numItems].itemType           = DT_SUBMENU;
   m[numItems].typeData.subMenu   = NULL;
   voccalitem = numItems;
   numItems++;   

   m[numItems].title              = IDS_ECHO_CANCEL;
   m[numItems].itemType           = DT_SUBMENU;
   m[numItems].typeData.subMenu   = NULL;
   echocancel = numItems;   
   numItems++;  
#endif //FEATURE_NV_VOICE_PARAMETER

   m[numItems].title       = IDS_BREW_CONFIG;
   m[numItems].itemType    = DT_SUBMENU;
   m[numItems].typeData.subMenu = NULL;
   idxBrew = numItems;
   numItems++;

#ifdef FEATURE_HDR
#error code not present
#endif

   // Store the Main menu pointer
   pMe->m_mainMenu = m;  

   
   //
   // Build the Network menu
   //
#ifdef FEATURE_ACP
   m = CSvcPrg_CreateMenuList(15); // 15 elements in the Network menu
#else  
   m = CSvcPrg_CreateMenuList(13); // 13 elements in the Network menu
#endif 
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_PHONE_NUMBER;
   m[0].itemType           = DT_WSTR;
   m[0].cfgItem            = CFGI_PHONE_NUMBER; 
   m[0].typeData.strLen    = OEMNV_PHONENUMBER_MAXLEN;
   m[0].isEditable         = TRUE;

   m[1].title              = IDS_HOME_SIDNID;
   m[1].itemType           = DT_SIDNIDLIST;
   m[1].cfgItem            = CFGI_HOME_SIDNID_LIST; 
   m[1].typeData.sidnid.count = OEMNV_HOME_SIDNID_ARRSIZE;
   m[1].isEditable         = TRUE;

   m[2].title              = IDS_LOCK_SIDNID;
   m[2].itemType           = DT_SIDNIDLIST;
   m[2].cfgItem            = CFGI_LOCK_SIDNID_LIST; 
   m[2].typeData.sidnid.count = OEMNV_LOCK_SIDNID_ARRSIZE;
   m[2].isEditable         = TRUE;

   m[3].title              = IDS_COUNTRY_CODE;
   m[3].itemType           = DT_WORD;
   m[3].cfgItem            = CFGI_IMSI_MCC;
   m[3].isEditable         = TRUE;

   m[4].title              = IDS_NETWORK_CODE;
   m[4].itemType           = DT_WORD;
   m[4].cfgItem            = CFGI_IMSI_11_12;
   m[4].isEditable         = TRUE;

   m[5].title              = IDS_IMSI_S;
   m[5].itemType           = DT_WSTR_FIXED;
   m[5].cfgItem            = CFGI_IMSI_S;
   m[5].typeData.strLen    = OEMNV_IMSI_S_LENGTH;
   m[5].isEditable         = TRUE;

   m[6].title              = IDS_PRL_ENABLED;
   m[6].itemType           = DT_BOOLEAN;
   m[6].cfgItem            = CFGI_PRL_ENABLED;
   m[6].isEditable         = TRUE;

   m[7].title              = IDS_PRI_CH_A;
   m[7].itemType           = DT_WORD;
   m[7].cfgItem            = CFGI_PRI_CH_A;
   m[7].isEditable         = TRUE;

   m[8].title              = IDS_PRI_CH_B;
   m[8].itemType           = DT_WORD;
   m[8].cfgItem            = CFGI_PRI_CH_B;
   m[8].isEditable         = TRUE;

   m[9].title              = IDS_SEC_CH_A;
   m[9].itemType           = DT_WORD;
   m[9].cfgItem            = CFGI_SEC_CH_A;
   m[9].isEditable         = TRUE;

   m[10].title             = IDS_SEC_CH_B;
   m[10].itemType          = DT_WORD;
   m[10].cfgItem           = CFGI_SEC_CH_B;
   m[10].isEditable        = TRUE;

   m[11].title             = IDS_REGISTRATION;
   m[11].itemType          = DT_SUBMENU;
   m[11].typeData.subMenu  = NULL;

   m[12].title             = IDS_AOC;
   m[12].itemType          = DT_BYTE;
   m[12].cfgItem           = CFGI_AOC;
   m[12].isEditable         = TRUE;

#ifdef FEATURE_ACP
   m[13].title              = IDS_MODE_PREF;
   m[13].itemType           = DT_RANGE;
   m[13].cfgItem            = CFGI_MODE_PREF;
   m[13].isEditable         = TRUE;
   m[13].typeData.rangeData = CSvcPrg_CreateRange(3);
   if (NULL == m[13].typeData.rangeData) {
      return FALSE;
   }
   m[13].typeData.rangeData[0].resId = IDS_MODE_AUTOMATIC;
   m[13].typeData.rangeData[0].value = OEMNV_MODE_AUTOMATIC;
   m[13].typeData.rangeData[1].resId = IDS_MODE_DIGITAL_ONLY;
   m[13].typeData.rangeData[1].value = OEMNV_MODE_DIGITAL_ONLY;
   m[13].typeData.rangeData[2].resId = IDS_MODE_ANALOG_ONLY;
   m[13].typeData.rangeData[2].value = OEMNV_MODE_ANALOG_ONLY;

   m[14].title              = IDS_ANALOG_OPTIONS;
   m[14].itemType           = DT_SUBMENU;
   m[14].typeData.subMenu   = NULL;

#endif /* FEATURE_ACP */
   
   // Store the Network menu in the Main menu
   ASSERT(IDS_NETWORK_OPTIONS == pMe->m_mainMenu[1].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[1].itemType);
   pMe->m_mainMenu[1].typeData.subMenu = m;

   //
   // Build the Registration submenu
   //

   m = CSvcPrg_CreateMenuList(3); // 3 elements in the Registration menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_HOME_SID_REG;
   m[0].itemType           = DT_BOOLEAN;
   m[0].cfgItem            = CFGI_HOME_SID_REG;
   m[0].isEditable         = TRUE;

   m[1].title              = IDS_FORN_SID_REG;
   m[1].itemType           = DT_BOOLEAN;
   m[1].cfgItem            = CFGI_FORN_SID_REG;
   m[1].isEditable         = TRUE;

   m[2].title              = IDS_FORN_NID_REG;
   m[2].itemType           = DT_BOOLEAN;
   m[2].cfgItem            = CFGI_FORN_NID_REG;
   m[2].isEditable         = TRUE;

   // Store the Registration menu pointer in the Network menu 
   ASSERT(IDS_NETWORK_OPTIONS == pMe->m_mainMenu[1].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[1].itemType);
   ASSERT(IDS_REGISTRATION == pMe->m_mainMenu[1].typeData.subMenu[11].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[1].typeData.subMenu[11].itemType);
   pMe->m_mainMenu[1].typeData.subMenu[11].typeData.subMenu = m;


#ifdef FEATURE_ACP
   //
   // Build the Analog Options submenu
   //

   m = CSvcPrg_CreateMenuList(3); // 3 elements in the Registration menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_AMPS_HOME_SID;
   m[0].itemType           = DT_WORD;
   m[0].cfgItem            = CFGI_AMPS_HOME_SID;
   m[0].isEditable         = TRUE;

   m[1].title              = IDS_AMPS_FIRSTCHP;
   m[1].itemType           = DT_WORD;
   m[1].cfgItem            = CFGI_AMPS_FIRSTCHP;
   m[1].isEditable         = TRUE;

   m[2].title              = IDS_AMPS_REG_TYPE;
   m[2].itemType           = DT_RANGE;
   m[2].cfgItem            = CFGI_AMPS_REG_TYPE;
   m[2].isEditable         = TRUE;

   m[2].typeData.rangeData = CSvcPrg_CreateRange(3);
   if (NULL == m[2].typeData.rangeData) {
      return FALSE;
   }
   m[2].typeData.rangeData[0].resId = IDS_AMPSREG_DISABLED;
   m[2].typeData.rangeData[0].value = OEMNV_AMPSREG_DISABLED;
   m[2].typeData.rangeData[1].resId = IDS_AMPSREG_WHEREABOUTS_KNOWN;
   m[2].typeData.rangeData[1].value = OEMNV_AMPSREG_WHEREABOUTS_KNOWN;
   m[2].typeData.rangeData[2].resId = IDS_AMPSREG_WHEREABOUTS_UNKNOWN;
   m[2].typeData.rangeData[2].value = OEMNV_AMPSREG_WHEREABOUTS_UNKNOWN;

   // Store the Analog Options menu pointer in the Network menu 
   ASSERT(IDS_NETWORK_OPTIONS == pMe->m_mainMenu[1].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[1].itemType);
   ASSERT(IDS_ANALOG_OPTIONS == pMe->m_mainMenu[1].typeData.subMenu[14].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[1].typeData.subMenu[14].itemType);
   pMe->m_mainMenu[1].typeData.subMenu[14].typeData.subMenu = m;
#endif /* FEATURE_ACP */

   //
   // Build the SMS Options menu
   //

   m = CSvcPrg_CreateMenuList(6); // 6 elements in the Network menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_SMS_TIMESTAMP;
   m[0].itemType           = DT_RANGE;
   m[0].cfgItem            = CFGI_SMS_TIMESTAMP; 
   m[0].isEditable         = TRUE;
   m[0].typeData.rangeData = CSvcPrg_CreateRange(2);
   if (NULL == m[0].typeData.rangeData) {
      return FALSE;
   }
   m[0].typeData.rangeData[0].resId = IDS_SMS_TIMESTAMP_ADJUST;
   m[0].typeData.rangeData[0].value = OEMNV_SMS_TIMESTAMP_ADJUST;
   m[0].typeData.rangeData[1].resId = IDS_SMS_TIMESTAMP_ASRECEIVED;
   m[0].typeData.rangeData[1].value = OEMNV_SMS_TIMESTAMP_ASRECEIVED;


   m[1].title              = IDS_MAX_MSG_PAYLOAD;
   m[1].itemType           = DT_DWORD;
   m[1].cfgItem            = CFGI_SMS_MAX_PAYLOAD_LENGTH;
   m[1].isEditable         = TRUE;

   m[2].title              = IDS_SMS_MO_ACCESS;
   m[2].itemType           = DT_RANGE;
   m[2].cfgItem            = CFGI_WMS_MO_CHANNEL_SELECT;//CFGI_SMS_MO_ON_ACCESS_CHANNEL;
   m[2].isEditable         = TRUE;
   m[2].typeData.rangeData = CSvcPrg_CreateRange(2);
   if (NULL == m[2].typeData.rangeData) {
      return FALSE;
   }

   m[2].typeData.rangeData[0].resId = IDS_SMS_MO_ACCESS_OFF;
   m[2].typeData.rangeData[0].value = 0;
   m[2].typeData.rangeData[1].resId = IDS_SMS_MO_ACCESS_ON;
   m[2].typeData.rangeData[1].value = 1;
   
   /*
   m[3].title              = IDS_SMS_MO_TRAFFIC;
   m[3].itemType           = DT_RANGE;
   m[3].cfgItem            = CFGI_SMS_MO_ON_TRAFFIC_CHANNEL;
   m[3].isEditable         = TRUE;
   m[3].typeData.rangeData = CSvcPrg_CreateRange(2);
   if (NULL == m[3].typeData.rangeData) {
      return FALSE;
   }
   m[3].typeData.rangeData[0].resId = IDS_SMS_MO_TRAFFIC_OFF;
   m[3].typeData.rangeData[0].value = 0;
   m[3].typeData.rangeData[1].resId = IDS_SMS_MO_TRAFFIC_ON;
   m[3].typeData.rangeData[1].value = 1;
  */
   m[4].title              = IDS_SMS_IS41_WORKAROUND;
   m[4].itemType           = DT_RANGE;
   m[4].cfgItem            = CFGI_SMS_IS41_WORKAROUND;
   m[4].isEditable         = TRUE;
   m[4].typeData.rangeData = CSvcPrg_CreateRange(2);
   if (NULL == m[4].typeData.rangeData) {
      return FALSE;
   }
   m[4].typeData.rangeData[0].resId = IDS_SMS_IS41_WORKAROUND_ON;
   m[4].typeData.rangeData[0].value = OEMNV_SMS_EMAIL_ADDRESSING_IS41;
   m[4].typeData.rangeData[1].resId = IDS_SMS_IS41_WORKAROUND_OFF;
   m[4].typeData.rangeData[1].value = OEMNV_SMS_EMAIL_ADDRESSING_STANDARD;


   m[5].title              = IDS_SMS_MO_ENCODING;
   m[5].itemType           = DT_RANGE;
   m[5].cfgItem            = CFGI_SMS_MO_ENCODING; 
   m[5].isEditable         = TRUE;
   m[5].typeData.rangeData = CSvcPrg_CreateRange(2);
   if (NULL == m[5].typeData.rangeData) {
      return FALSE;
   }
   m[5].typeData.rangeData[0].resId = IDS_SMS_MO_ENCODING_7BIT;
   m[5].typeData.rangeData[0].value = OEMNV_SMS_MO_ENCODING_7BIT;
#if defined(FEATURE_CARRIER_ANGOLA_MOVICEL) || defined(FEATURE_CARRIER_MAROC_WANA) || defined (FEATURE_CARRIER_ISRAEL_PELEPHONE)
   m[5].typeData.rangeData[1].resId = IDS_SMS_MO_ENCODING_OCTET;
   m[5].typeData.rangeData[1].value = OEMNV_SMS_MO_ENCODING_OCTET;
#else       
   m[5].typeData.rangeData[1].resId = IDS_SMS_MO_ENCODING_UNICODE;
   m[5].typeData.rangeData[1].value = OEMNV_SMS_MO_ENCODING_UNICODE;
#endif

   // Store the SMS Options menu in the Main menu
   ASSERT(IDS_SMS_OPTIONS == pMe->m_mainMenu[2].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[2].itemType);
   pMe->m_mainMenu[2].typeData.subMenu = m;


   //
   // Build the Data Options menu
   //

   m = CSvcPrg_CreateMenuList(2); // 2 elements in the Network menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_QNC_ENABLED;
   m[0].itemType           = DT_BOOLEAN;
   m[0].cfgItem            = CFGI_DATA_QNC_ENABLED;
   m[0].isEditable         = TRUE;


   m[1].title              = IDS_DATA_DIALSTRING;
   m[1].itemType           = DT_WSTR;
   m[1].cfgItem            = CFGI_DATA_DIALSTRING;
   m[1].typeData.strLen    = OEMNV_DATA_DIALSTRING_MAXLEN; 
   m[1].isEditable         = TRUE;

   // Store the Data Options submenu in the Main menu
   ASSERT(IDS_DATA_OPTIONS == pMe->m_mainMenu[3].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[3].itemType);
   pMe->m_mainMenu[3].typeData.subMenu = m;   

#ifdef FEATURE_NV_VOICE_PARAMETER   
   //build Voice parameter cal submenu
    m = CSvcPrg_CreateMenuList(3); // 2 elements in the Network menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_VOCCAL_HANDSET;
   m[0].itemType           = DT_SUBMENU;
   m[0].typeData.subMenu   = NULL;

   m[1].title              = IDS_VOCCAL_HEADSET;
   m[1].itemType           = DT_SUBMENU;
   m[1].typeData.subMenu   = NULL;

   m[2].title              = IDS_VOCCAL_SPEAKER;
   m[2].itemType           = DT_SUBMENU;
   m[2].typeData.subMenu   = NULL;

   // Store the SMS Options menu in the Main menu
   ASSERT(IDS_VOCCAL == pMe->m_mainMenu[voccalitem].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[voccalitem].itemType);
   pMe->m_mainMenu[voccalitem].typeData.subMenu = m;

   //
   // Build the Handset submenu
   //

   m = CSvcPrg_CreateMenuList(9); // 9 elements in the Registration menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_VOCCAL_USE_NVMODE;
   m[0].itemType           = DT_BOOLEAN_VOCCAL;
   m[0].nvitem             = NV_VOC_PCM_ON_CHIP_0_CAL_I;
   m[0].isEditable         = TRUE;   
   
   m[1].title              = IDS_VOCCAL_EC_MODE;
   m[1].itemType           = DT_WORD_VOCCAL;
   m[1].nvitem             = NV_VOC_PCM_ON_CHIP_0_CAL_I;
   m[1].isEditable         = TRUE;
   
   m[2].title              = IDS_VOCCAL_NS_ENABLE;
   m[2].itemType           = DT_WORD_VOCCAL;
   m[2].nvitem             = NV_VOC_PCM_ON_CHIP_0_CAL_I;
   m[2].isEditable         = TRUE;

   m[3].title              = IDS_VOCCAL_TX_GAIN;
   m[3].itemType           = DT_WORD_VOCCAL;
   m[3].nvitem             = NV_VOC_PCM_ON_CHIP_0_CAL_I;
   m[3].isEditable         = TRUE;

   m[4].title              = IDS_VOCCAL_DTMF_TX_GAIN;
   m[4].itemType           = DT_WORD_VOCCAL;
   m[4].nvitem             = NV_VOC_PCM_ON_CHIP_0_CAL_I;
   m[4].isEditable         = TRUE;

   m[5].title              = IDS_VOCCAL_CODEC_TX_GAIN;
   m[5].itemType           = DT_WORD_VOCCAL;
   m[5].nvitem             = NV_VOC_PCM_ON_CHIP_0_CAL_I;
   m[5].isEditable         = TRUE;

   m[6].title              = IDS_VOCCAL_CODEC_RX_GAIN;
   m[6].itemType           = DT_WORD_VOCCAL;
   m[6].nvitem             = NV_VOC_PCM_ON_CHIP_0_CAL_I;
   m[6].isEditable         = TRUE;   

   m[7].title              = IDS_VOCCAL_CODEC_ST_GAIN;
   m[7].itemType           = DT_WORD_VOCCAL;
   m[7].nvitem             = NV_VOC_PCM_ON_CHIP_0_CAL_I;
   m[7].isEditable         = TRUE;   

   m[8].title              = IDS_VOCCAL_RX_DBM_OFFSET;
   m[8].itemType           = DT_WORD_VOCCAL;
   m[8].nvitem             = NV_VOC_PCM_ON_CHIP_0_CAL_I;
   m[8].isEditable         = TRUE;   
   
   // Store the Registration menu pointer in the Network menu 
   ASSERT(IDS_VOCCAL == pMe->m_mainMenu[voccalitem].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[voccalitem].itemType);
   ASSERT(IDS_VOCCAL_HANDSET == pMe->m_mainMenu[voccalitem].typeData.subMenu[0].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[voccalitem].typeData.subMenu[0].itemType);
   pMe->m_mainMenu[voccalitem].typeData.subMenu[0].typeData.subMenu = m;   

   //
   // Build the Headset submenu
   //

   m = CSvcPrg_CreateMenuList(9); // 9 elements in the Registration menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_VOCCAL_USE_NVMODE;
   m[0].itemType           = DT_BOOLEAN_VOCCAL;
   m[0].nvitem             = NV_VOC_PCM_ON_CHIP_1_CAL_I;
   m[0].isEditable         = TRUE;   
   
   m[1].title              = IDS_VOCCAL_EC_MODE;
   m[1].itemType           = DT_WORD_VOCCAL;
   m[1].nvitem             = NV_VOC_PCM_ON_CHIP_1_CAL_I;
   m[1].isEditable         = TRUE;
   
   m[2].title              = IDS_VOCCAL_NS_ENABLE;
   m[2].itemType           = DT_WORD_VOCCAL;
   m[2].nvitem             = NV_VOC_PCM_ON_CHIP_1_CAL_I;
   m[2].isEditable         = TRUE;

   m[3].title              = IDS_VOCCAL_TX_GAIN;
   m[3].itemType           = DT_WORD_VOCCAL;
   m[3].nvitem             = NV_VOC_PCM_ON_CHIP_1_CAL_I;
   m[3].isEditable         = TRUE;

   m[4].title              = IDS_VOCCAL_DTMF_TX_GAIN;
   m[4].itemType           = DT_WORD_VOCCAL;
   m[4].nvitem             = NV_VOC_PCM_ON_CHIP_1_CAL_I;
   m[4].isEditable         = TRUE;

   m[5].title              = IDS_VOCCAL_CODEC_TX_GAIN;
   m[5].itemType           = DT_WORD_VOCCAL;
   m[5].nvitem             = NV_VOC_PCM_ON_CHIP_1_CAL_I;
   m[5].isEditable         = TRUE;

   m[6].title              = IDS_VOCCAL_CODEC_RX_GAIN;
   m[6].itemType           = DT_WORD_VOCCAL;
   m[6].nvitem             = NV_VOC_PCM_ON_CHIP_1_CAL_I;
   m[6].isEditable         = TRUE;   

   m[7].title              = IDS_VOCCAL_CODEC_ST_GAIN;
   m[7].itemType           = DT_WORD_VOCCAL;
   m[7].nvitem             = NV_VOC_PCM_ON_CHIP_1_CAL_I;
   m[7].isEditable         = TRUE;   

   m[8].title              = IDS_VOCCAL_RX_DBM_OFFSET;
   m[8].itemType           = DT_WORD_VOCCAL;
   m[8].nvitem             = NV_VOC_PCM_ON_CHIP_1_CAL_I;
   m[8].isEditable         = TRUE;   
   
   // Store the Registration menu pointer in the Network menu 
   ASSERT(IDS_VOCCAL == pMe->m_mainMenu[voccalitem].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[voccalitem].itemType);
   ASSERT(IDS_VOCCAL_HEADSET == pMe->m_mainMenu[voccalitem].typeData.subMenu[1].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[voccalitem].typeData.subMenu[1].itemType);
   pMe->m_mainMenu[voccalitem].typeData.subMenu[1].typeData.subMenu = m; 

   //
   // Build the Speaker submenu
   //

   m = CSvcPrg_CreateMenuList(9); // 9 elements in the Registration menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_VOCCAL_USE_NVMODE;
   m[0].itemType           = DT_BOOLEAN_VOCCAL;
   m[0].nvitem             = NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I;
   m[0].isEditable         = TRUE;   
   
   m[1].title              = IDS_VOCCAL_EC_MODE;
   m[1].itemType           = DT_WORD_VOCCAL;
   m[1].nvitem             = NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I;
   m[1].isEditable         = TRUE;
   
   m[2].title              = IDS_VOCCAL_NS_ENABLE;
   m[2].itemType           = DT_WORD_VOCCAL;
   m[2].nvitem             = NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I;
   m[2].isEditable         = TRUE;

   m[3].title              = IDS_VOCCAL_TX_GAIN;
   m[3].itemType           = DT_WORD_VOCCAL;
   m[3].nvitem             = NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I;
   m[3].isEditable         = TRUE;

   m[4].title              = IDS_VOCCAL_DTMF_TX_GAIN;
   m[4].itemType           = DT_WORD_VOCCAL;
   m[4].nvitem             = NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I;
   m[4].isEditable         = TRUE;

   m[5].title              = IDS_VOCCAL_CODEC_TX_GAIN;
   m[5].itemType           = DT_WORD_VOCCAL;
   m[5].nvitem             = NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I;
   m[5].isEditable         = TRUE;

   m[6].title              = IDS_VOCCAL_CODEC_RX_GAIN;
   m[6].itemType           = DT_WORD_VOCCAL;
   m[6].nvitem             = NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I;
   m[6].isEditable         = TRUE;   

   m[7].title              = IDS_VOCCAL_CODEC_ST_GAIN;
   m[7].itemType           = DT_WORD_VOCCAL;
   m[7].nvitem             = NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I;
   m[7].isEditable         = TRUE;   

   m[8].title              = IDS_VOCCAL_RX_DBM_OFFSET;
   m[8].itemType           = DT_WORD_VOCCAL;
   m[8].nvitem             = NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I;
   m[8].isEditable         = TRUE;   
   
   // Store the Registration menu pointer in the Network menu 
   ASSERT(IDS_VOCCAL == pMe->m_mainMenu[voccalitem].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[voccalitem].itemType);
   ASSERT(IDS_VOCCAL_SPEAKER == pMe->m_mainMenu[voccalitem].typeData.subMenu[2].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[voccalitem].typeData.subMenu[2].itemType);
   pMe->m_mainMenu[voccalitem].typeData.subMenu[2].typeData.subMenu = m; 



   //build Echo cancel submenu
    m = CSvcPrg_CreateMenuList(4); // 2 elements in the Network menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_ECHO_CANCEL_ESEC;
   m[0].itemType           = DT_SUBMENU;
   m[0].typeData.subMenu   = NULL;

   m[1].title              = IDS_ECHO_CANCEL_HEADSET;
   m[1].itemType           = DT_SUBMENU;
   m[1].typeData.subMenu   = NULL;

   m[2].title              = IDS_ECHO_CANCEL_AEC;
   m[2].itemType           = DT_SUBMENU;
   m[2].typeData.subMenu   = NULL;

   m[3].title              = IDS_ECHO_CANCEL_SPEAKER;
   m[3].itemType           = DT_SUBMENU;
   m[3].typeData.subMenu   = NULL;   

   // Store the SMS Options menu in the Main menu
   ASSERT(IDS_ECHO_CANCEL == pMe->m_mainMenu[echocancel].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[echocancel].itemType);
   pMe->m_mainMenu[echocancel].typeData.subMenu = m;

   //
   // Build the EC_ESEC submenu
   //

   m = CSvcPrg_CreateMenuList(6); // 9 elements in the Registration menu
   if (NULL == m) {
      return FALSE;
   }
   m[0].title              = IDS_VOCCAL_USE_NVMODE;
   m[0].itemType           = DT_BOOLEAN_VOCCAL;
   m[0].nvitem             = NV_VOC_CAL_EC_PARAMS_ESEC_I;
   m[0].isEditable         = TRUE; 

   m[1].title              = IDS_ECHO_CANCEL_FAREND;
   m[1].itemType           = DT_WORD_VOCCAL;
   m[1].nvitem             = NV_VOC_CAL_EC_PARAMS_ESEC_I;
   m[1].isEditable         = TRUE;   
   
   m[2].title              = IDS_ECHO_CANCEL_DOUBLETALK;
   m[2].itemType           = DT_WORD_VOCCAL;
   m[2].nvitem             = NV_VOC_CAL_EC_PARAMS_ESEC_I;
   m[2].isEditable         = TRUE;
   
   m[3].title              = IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER;
   m[3].itemType           = DT_WORD_VOCCAL;
   m[3].nvitem             = NV_VOC_CAL_EC_PARAMS_ESEC_I;
   m[3].isEditable         = TRUE;

   m[4].title              = IDS_ECHO_CANCEL_STARTUP_MUTE_MODE;
   m[4].itemType           = DT_WORD_VOCCAL;
   m[4].nvitem             = NV_VOC_CAL_EC_PARAMS_ESEC_I;
   m[4].isEditable         = TRUE;

   m[5].title              = IDS_ECHO_CANCEL_STARTUP_ERLE;
   m[5].itemType           = DT_WORD_VOCCAL;
   m[5].nvitem             = NV_VOC_CAL_EC_PARAMS_ESEC_I;
   m[5].isEditable         = TRUE;
   
   // Store the Registration menu pointer in the Network menu 
   ASSERT(IDS_ECHO_CANCEL == pMe->m_mainMenu[echocancel].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[echocancel].itemType);
   ASSERT(IDS_ECHO_CANCEL_ESEC == pMe->m_mainMenu[echocancel].typeData.subMenu[0].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[echocancel].typeData.subMenu[0].itemType);
   pMe->m_mainMenu[echocancel].typeData.subMenu[0].typeData.subMenu = m;   

   //
   // Build the EC_Headset submenu
   //

   m = CSvcPrg_CreateMenuList(6); // 9 elements in the Registration menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_VOCCAL_USE_NVMODE;
   m[0].itemType           = DT_BOOLEAN_VOCCAL;
   m[0].nvitem             = NV_VOC_CAL_EC_PARAMS_HEADSET_I;
   m[0].isEditable         = TRUE; 

   m[1].title              = IDS_ECHO_CANCEL_FAREND;
   m[1].itemType           = DT_WORD_VOCCAL;
   m[1].nvitem             = NV_VOC_CAL_EC_PARAMS_HEADSET_I;
   m[1].isEditable         = TRUE;   
   
   m[2].title              = IDS_ECHO_CANCEL_DOUBLETALK;
   m[2].itemType           = DT_WORD_VOCCAL;
   m[2].nvitem             = NV_VOC_CAL_EC_PARAMS_HEADSET_I;
   m[2].isEditable         = TRUE;
   
   m[3].title              = IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER;
   m[3].itemType           = DT_WORD_VOCCAL;
   m[3].nvitem             = NV_VOC_CAL_EC_PARAMS_HEADSET_I;
   m[3].isEditable         = TRUE;

   m[4].title              = IDS_ECHO_CANCEL_STARTUP_MUTE_MODE;
   m[4].itemType           = DT_WORD_VOCCAL;
   m[4].nvitem             = NV_VOC_CAL_EC_PARAMS_HEADSET_I;
   m[4].isEditable         = TRUE;

   m[5].title              = IDS_ECHO_CANCEL_STARTUP_ERLE;
   m[5].itemType           = DT_WORD_VOCCAL;
   m[5].nvitem             = NV_VOC_CAL_EC_PARAMS_HEADSET_I;
   m[5].isEditable         = TRUE;
   
   // Store the Registration menu pointer in the Network menu 
   ASSERT(IDS_ECHO_CANCEL == pMe->m_mainMenu[echocancel].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[echocancel].itemType);
   ASSERT(IDS_ECHO_CANCEL_HEADSET == pMe->m_mainMenu[echocancel].typeData.subMenu[1].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[echocancel].typeData.subMenu[1].itemType);
   pMe->m_mainMenu[echocancel].typeData.subMenu[1].typeData.subMenu = m;  

   //
   // Build the EC_AEC submenu
   //

   m = CSvcPrg_CreateMenuList(6); // 9 elements in the Registration menu
   if (NULL == m) {
      return FALSE;
   }
   
   m[0].title              = IDS_VOCCAL_USE_NVMODE;
   m[0].itemType           = DT_BOOLEAN_VOCCAL;
   m[0].nvitem             = NV_VOC_CAL_EC_PARAMS_AEC_I;
   m[0].isEditable         = TRUE; 

   m[1].title              = IDS_ECHO_CANCEL_FAREND;
   m[1].itemType           = DT_WORD_VOCCAL;
   m[1].nvitem             = NV_VOC_CAL_EC_PARAMS_AEC_I;
   m[1].isEditable         = TRUE;   
   
   m[2].title              = IDS_ECHO_CANCEL_DOUBLETALK;
   m[2].itemType           = DT_WORD_VOCCAL;
   m[2].nvitem             = NV_VOC_CAL_EC_PARAMS_AEC_I;
   m[2].isEditable         = TRUE;
   
   m[3].title              = IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER;
   m[3].itemType           = DT_WORD_VOCCAL;
   m[3].nvitem             = NV_VOC_CAL_EC_PARAMS_AEC_I;
   m[3].isEditable         = TRUE;

   m[4].title              = IDS_ECHO_CANCEL_STARTUP_MUTE_MODE;
   m[4].itemType           = DT_WORD_VOCCAL;
   m[4].nvitem             = NV_VOC_CAL_EC_PARAMS_AEC_I;
   m[4].isEditable         = TRUE;

   m[5].title              = IDS_ECHO_CANCEL_STARTUP_ERLE;
   m[5].itemType           = DT_WORD_VOCCAL;
   m[5].nvitem             = NV_VOC_CAL_EC_PARAMS_AEC_I;
   m[5].isEditable         = TRUE;
   
   // Store the Registration menu pointer in the Network menu 
   ASSERT(IDS_ECHO_CANCEL == pMe->m_mainMenu[echocancel].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[echocancel].itemType);
   ASSERT(IDS_ECHO_CANCEL_AEC == pMe->m_mainMenu[echocancel].typeData.subMenu[2].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[echocancel].typeData.subMenu[2].itemType);
   pMe->m_mainMenu[echocancel].typeData.subMenu[2].typeData.subMenu = m; 

   //
   // Build the EC_ESEC submenu
   //

   m = CSvcPrg_CreateMenuList(6); // 9 elements in the Registration menu
   if (NULL == m) {
      return FALSE;
   }
   m[0].title              = IDS_VOCCAL_USE_NVMODE;
   m[0].itemType           = DT_BOOLEAN_VOCCAL;
   m[0].nvitem             = NV_VOC_CAL_EC_PARAMS_SPEAKER_I;
   m[0].isEditable         = TRUE; 

   m[1].title              = IDS_ECHO_CANCEL_FAREND;
   m[1].itemType           = DT_WORD_VOCCAL;
   m[1].nvitem             = NV_VOC_CAL_EC_PARAMS_SPEAKER_I;
   m[1].isEditable         = TRUE;   
   
   m[2].title              = IDS_ECHO_CANCEL_DOUBLETALK;
   m[2].itemType           = DT_WORD_VOCCAL;
   m[2].nvitem             = NV_VOC_CAL_EC_PARAMS_SPEAKER_I;
   m[2].isEditable         = TRUE;
   
   m[3].title              = IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER;
   m[3].itemType           = DT_WORD_VOCCAL;
   m[3].nvitem             = NV_VOC_CAL_EC_PARAMS_SPEAKER_I;
   m[3].isEditable         = TRUE;

   m[4].title              = IDS_ECHO_CANCEL_STARTUP_MUTE_MODE;
   m[4].itemType           = DT_WORD_VOCCAL;
   m[4].nvitem             = NV_VOC_CAL_EC_PARAMS_SPEAKER_I;
   m[4].isEditable         = TRUE;

   m[5].title              = IDS_ECHO_CANCEL_STARTUP_ERLE;
   m[5].itemType           = DT_WORD_VOCCAL;
   m[5].nvitem             = NV_VOC_CAL_EC_PARAMS_SPEAKER_I;
   m[5].isEditable         = TRUE;
   
   // Store the Registration menu pointer in the Network menu 
   ASSERT(IDS_ECHO_CANCEL == pMe->m_mainMenu[echocancel].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[echocancel].itemType);
   ASSERT(IDS_ECHO_CANCEL_SPEAKER == pMe->m_mainMenu[echocancel].typeData.subMenu[3].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[echocancel].typeData.subMenu[3].itemType);
   pMe->m_mainMenu[echocancel].typeData.subMenu[3].typeData.subMenu = m;  
#endif //FEATURE_NV_VOICE_PARAMETER

   //
   // Build the BREW Configuration menu
   //
   // 14 elements in the BREW Configuration menu
   m = CSvcPrg_CreateMenuList(14); 
   if (NULL == m) 
   {
      return FALSE;
   }
   
   m[0].title              = IDS_BREWUSERNAME;
   m[0].itemType           = DT_ALPHA;
   m[0].cfgItem            = CFGI_BREW_USERNAME; 
   m[0].typeData.strLen    = BREW_USERNAME_LEN;
   m[0].isEditable         = TRUE;
   
   m[1].title              = IDS_BREWPASSWORD;
   m[1].itemType           = DT_ALPHA;
   m[1].cfgItem            = CFGI_BREW_PASSWORD; 
   m[1].typeData.strLen    = BREW_PASSWORD_LEN;
   m[1].isEditable         = TRUE;
   
   m[2].title              = IDS_ADSURL;
   m[2].itemType           = DT_ALPHA;
   m[2].cfgItem            = CFGI_BREW_SERVER; 
   m[2].typeData.strLen    = DL_MAX_SERVER;
   m[2].isEditable         = TRUE;
   
   m[3].title              = IDS_PRIM_DNS_IP;
   m[3].itemType           = DT_DWORD;
   m[3].cfgItem            = CFGI_DNS_IP1; 
   m[3].isEditable         = TRUE;
   
   m[4].title              = IDS_SECOND_DNS_IP;
   m[4].itemType           = DT_DWORD;
   m[4].cfgItem            = CFGI_DNS_IP2; 
   m[4].isEditable         = TRUE;
   
   m[5].title              = IDS_CARRIER_ID;
   m[5].itemType           = DT_DWORD;
   m[5].cfgItem            = CFGI_BREW_CARRIER_ID; 
   m[5].isEditable         = TRUE;
   
   m[6].title              = IDS_BKEY;
   m[6].itemType           = DT_WSTR;
   m[6].cfgItem            = CFGI_BREW_BKEY; 
   m[6].typeData.strLen    = NV_BREW_BKEY_SIZ;
   m[6].isEditable         = TRUE;
   
   m[7].title              = IDS_KEY_SPEC;
   m[7].itemType           = DT_BOOLEAN;
   m[7].cfgItem            = CFGI_BREW_USEAKEY;
   m[7].isEditable         = TRUE;
   
   m[8].title             = IDS_AUTHFLAG;
   m[8].itemType          = DT_SUBMENU;
   m[8].typeData.subMenu  = NULL;
   
   m[9].title              = IDS_AUTHPOLICY;
   m[9].itemType           = DT_RANGE;
   m[9].cfgItem            = CFGI_BREW_AUTH_POLICY;
   m[9].isEditable         = TRUE;
   m[9].typeData.rangeData = CSvcPrg_CreateRange(4);
   if (NULL == m[9].typeData.rangeData) 
   {
      return FALSE;
   }
   m[9].typeData.rangeData[0].resId = IDS_APOLICY_NONE;
   m[9].typeData.rangeData[0].value = APOLICY_NONE;
   m[9].typeData.rangeData[1].resId = IDS_APOLICY_SID;
   m[9].typeData.rangeData[1].value = APOLICY_SID;
   m[9].typeData.rangeData[2].resId = IDS_APOLICY_TEXT;
   m[9].typeData.rangeData[2].value = APOLICY_TEXT;
   m[9].typeData.rangeData[3].resId = IDS_APOLICY_NUM;
   m[9].typeData.rangeData[3].value = APOLICY_NUM;
   
   m[10].title              = IDS_APP_POLICY;
   m[10].itemType           = DT_RANGE;
   m[10].cfgItem            = CFGI_BREW_PRIVACY_POLICY;
   m[10].isEditable         = TRUE;
   m[10].typeData.rangeData = CSvcPrg_CreateRange(4);
   if (NULL == m[10].typeData.rangeData) 
   {
      return FALSE;
   }
   m[10].typeData.rangeData[0].resId = IDS_PPOLICY_BREW;
   m[10].typeData.rangeData[0].value = PPOLICY_BREW;
   m[10].typeData.rangeData[1].resId = IDS_PPOLICY_CARRIER;
   m[10].typeData.rangeData[1].value = PPOLICY_CARRIER;
   m[10].typeData.rangeData[2].resId = IDS_PPOLICY_BREW_AND_CARRIER;
   m[10].typeData.rangeData[2].value = PPOLICY_BREW_AND_CARRIER;
   m[10].typeData.rangeData[3].resId = IDS_PPOLICY_BREW_OR_CARRIER;
   m[10].typeData.rangeData[3].value = PPOLICY_BREW_OR_CARRIER;
   
   m[11].title              = IDS_BREWTESTOPT;
   m[11].itemType           = DT_BOOLEAN;
   m[11].cfgItem            = CFGI_BREW_TESTOPT;
   m[11].isEditable         = TRUE;
   
   m[12].title              = IDS_BREW_SID;
   m[12].itemType           = DT_WSTR;
   m[12].cfgItem            = CFGI_BREW_SUBSCRIBER_ID; 
   m[12].typeData.strLen    = NV_BREW_SID_SIZ;/* add 1 to fix textctl */
   m[12].isEditable         = FALSE;
   
   m[13].title              = IDS_PLATFORMID;
   m[13].itemType           = DT_DWORD;
   m[13].cfgItem            = CFGI_BREW_PLATFORM_ID; 
   m[13].isEditable         = TRUE;
   
   // Store the BREW Config menu in the Main menu
   ASSERT(IDS_BREW_CONFIG == pMe->m_mainMenu[idxBrew].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[idxBrew].itemType);
   pMe->m_mainMenu[idxBrew].typeData.subMenu = m;
   
   //
   // Build the User Authorization Flag submenu
   //
   if (IsRunAsUIMVersion())
   {
      m = CSvcPrg_CreateMenuList(7); 
   }
   else
   {
      m = CSvcPrg_CreateMenuList(6);
   }
   
   if (NULL == m) 
   {
      return FALSE;
   }
   m[0].title              = IDS_AUTOUPGRADE;
   m[0].itemType           = DT_BOOLEAN;
   m[0].cfgItem            = CFGI_BREW_AUTOUPGRADE_FLG;
   m[0].isEditable         = TRUE;
   
   m[1].title              = IDS_USE_MIN_FOR_SID;
   m[1].itemType           = DT_BOOLEAN;
   m[1].cfgItem            = CFGI_BREW_USEMINFORSID_FLG;
   m[1].isEditable         = TRUE;

   m[2].title              = IDS_PREPAY;
   m[2].itemType           = DT_BOOLEAN;
   m[2].cfgItem            = CFGI_BREW_PREPAY_FLG;
   m[2].isEditable         = TRUE;

   m[3].title              = IDS_NOAUTOACK;
   m[3].itemType           = DT_BOOLEAN;
   m[3].cfgItem            = CFGI_BREW_NOAUTOACK_FLG;
   m[3].isEditable         = TRUE;
   
   m[4].title              = IDS_SIDENCODE;
   m[4].itemType           = DT_BOOLEAN;
   m[4].cfgItem            = CFGI_BREW_SIDENCODE_FLG;
   m[4].isEditable         = TRUE;
   
   m[5].title              = IDS_SIDVALIDATAALL;
   m[5].itemType           = DT_BOOLEAN;
   m[5].cfgItem            = CFGI_BREW_SIDVALIDATAALL_FLG;
   m[5].isEditable         = TRUE;
   
   if (IsRunAsUIMVersion())
   {
      m[6].title              = IDS_RUIMDELETE;
      m[6].itemType           = DT_BOOLEAN;
      m[6].cfgItem            = CFGI_BREW_IDS_RUIMDELETE_FLG;
      m[6].isEditable         = TRUE;
   }

   // Store the User Authorization Flag menu pointer in the BREW Configuration menu 
   ASSERT(IDS_BREW_CONFIG == pMe->m_mainMenu[idxBrew].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[idxBrew].itemType);
   ASSERT(IDS_AUTHFLAG == pMe->m_mainMenu[idxBrew].typeData.subMenu[8].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[idxBrew].typeData.subMenu[8].itemType);
   pMe->m_mainMenu[idxBrew].typeData.subMenu[8].typeData.subMenu = m;

   return TRUE;    // Phew!  I'm sure glad that pain is over with.
}

#else

static boolean CSvcPrg_BuildMenuList(CSvcPrgApp *pMe)
{
   MenuItemType *m;
   int numItems = 7; // Number of items in the main menu.
   
   pMe->m_mainMenu = NULL;
   //
   // Build the Main menu
   //
#ifdef FEATURE_CDMA_RX_DIVERSITY
   numItems++;
#endif

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
   numItems++;
#endif

#ifdef FEATURE_HDR
#error code not present
#endif

   m = CSvcPrg_CreateMenuList(numItems);  // 7 elements in the Main menu

   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_ESN;
   m[0].itemType           = DT_ESN;
   m[0].cfgItem            = CFGI_ESN; 

   m[1].title              = IDS_NETWORK_OPTIONS;
   m[1].itemType           = DT_SUBMENU;
   m[1].typeData.subMenu   = NULL;
   
   m[2].title              = IDS_SMS_OPTIONS;
   m[2].itemType           = DT_SUBMENU;
   m[2].typeData.subMenu   = NULL;

   m[3].title              = IDS_DATA_OPTIONS;
   m[3].itemType           = DT_SUBMENU;
   m[3].typeData.subMenu   = NULL;
   
   m[4].title              = IDS_SLOTCYCLEINDEX;
   m[4].itemType           = DT_BYTE;
   m[4].cfgItem            = CFGI_SLOTINDEX; 
   m[4].isEditable         = TRUE;

   m[5].title              = IDS_SPCCODE;
   m[5].itemType           = DT_WSTR_FIXED;
   m[5].cfgItem            = CFGI_SECCODE; 
   m[5].typeData.strLen    = OEMNV_SECCODE_LENGTH;
   m[5].isEditable         = TRUE;

   m[6].title             = IDS_AKEY;
   m[6].itemType          = DT_AKEY;
   m[6].typeData.strLen   = MAX_AKEY_DIGITS+1; /* add 1 to fix textctl */

   numItems = 7;

#ifdef FEATURE_CDMA_RX_DIVERSITY
   m[numItems].title              = IDS_CDMA_RX_DIVERSITY_ENABLED;
   m[numItems].itemType           = DT_BOOLEAN;
   m[numItems].cfgItem            = CFGI_CDMA_RX_DIVERSITY_CTL;
   m[numItems].isEditable         = TRUE;
   numItems++;
#endif
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
   m[numItems].title             = IDS_RTRE_SELECT;
   m[numItems].itemType          = DT_RANGE;
   m[numItems].cfgItem           = CFGI_RTRE_CONFIGURATION; 
   m[numItems].isEditable        = TRUE;

   m[numItems].typeData.rangeData = CSvcPrg_CreateRange(3);
   if (NULL == m[numItems].typeData.rangeData) {
      return FALSE;
   }
   m[numItems].typeData.rangeData[0].resId = IDS_RTRE_SELECT_NVONLY;
   m[numItems].typeData.rangeData[0].value = NV_RTRE_CONFIG_NV_ONLY;
   m[numItems].typeData.rangeData[1].resId = IDS_RTRE_SELECT_RUIMONLY;
   m[numItems].typeData.rangeData[1].value = NV_RTRE_CONFIG_RUIM_ONLY;
   m[numItems].typeData.rangeData[2].resId = IDS_RTRE_SELECT_BOTH;
   m[numItems].typeData.rangeData[2].value = NV_RTRE_CONFIG_RUIM_OR_DROP_BACK;
   ERR("F numItems:= %d", numItems, 0, 0);
   numItems++;
   ERR("S numItems:= %d", numItems, 0, 0);
#endif

#ifdef FEATURE_HDR
#error code not present
#endif

   // Store the Main menu pointer
   pMe->m_mainMenu = m;  

   
   //
   // Build the Network menu
   //
#ifdef FEATURE_ACP
   m = CSvcPrg_CreateMenuList(14); // 14 elements in the Network menu
#else  
   m = CSvcPrg_CreateMenuList(12); // 12 elements in the Network menu
#endif 
   if (NULL == m) {
      return FALSE;
   }
/*
   m[0].title              = IDS_PHONE_NUMBER;
   m[0].itemType           = DT_WSTR;
   m[0].cfgItem            = CFGI_PHONE_NUMBER; 
   m[0].typeData.strLen    = OEMNV_PHONENUMBER_MAXLEN;
   m[0].isEditable         = TRUE;
*/
   m[0].title              = IDS_HOME_SIDNID;
   m[0].itemType           = DT_SIDNIDLIST;
   m[0].cfgItem            = CFGI_HOME_SIDNID_LIST; 
   m[0].typeData.sidnid.count = OEMNV_HOME_SIDNID_ARRSIZE;
   m[0].isEditable         = TRUE;

   m[1].title              = IDS_LOCK_SIDNID;
   m[1].itemType           = DT_SIDNIDLIST;
   m[1].cfgItem            = CFGI_LOCK_SIDNID_LIST; 
   m[1].typeData.sidnid.count = OEMNV_LOCK_SIDNID_ARRSIZE;
   m[1].isEditable         = TRUE;

   m[2].title              = IDS_COUNTRY_CODE;
   m[2].itemType           = DT_WORD;
   m[2].cfgItem            = CFGI_IMSI_MCC;
   m[2].isEditable         = TRUE;

   m[3].title              = IDS_NETWORK_CODE;
   m[3].itemType           = DT_WORD;
   m[3].cfgItem            = CFGI_IMSI_11_12;
   m[3].isEditable         = TRUE;

   m[4].title              = IDS_IMSI_S;
   m[4].itemType           = DT_WSTR_FIXED;
   m[4].cfgItem            = CFGI_IMSI_S;
   m[4].typeData.strLen    = OEMNV_IMSI_S_LENGTH;
   m[4].isEditable         = TRUE;

   m[5].title              = IDS_PRL_ENABLED;
   m[5].itemType           = DT_BOOLEAN;
   m[5].cfgItem            = CFGI_PRL_ENABLED;
   m[5].isEditable         = TRUE;

   m[6].title              = IDS_PRI_CH_A;
   m[6].itemType           = DT_WORD;
   m[6].cfgItem            = CFGI_PRI_CH_A;
   m[6].isEditable         = TRUE;

   m[7].title              = IDS_PRI_CH_B;
   m[7].itemType           = DT_WORD;
   m[7].cfgItem            = CFGI_PRI_CH_B;
   m[7].isEditable         = TRUE;

   m[8].title              = IDS_SEC_CH_A;
   m[8].itemType           = DT_WORD;
   m[8].cfgItem            = CFGI_SEC_CH_A;
   m[8].isEditable         = TRUE;

   m[9].title             = IDS_SEC_CH_B;
   m[9].itemType          = DT_WORD;
   m[9].cfgItem           = CFGI_SEC_CH_B;
   m[9].isEditable        = TRUE;

   m[10].title             = IDS_REGISTRATION;
   m[10].itemType          = DT_SUBMENU;
   m[10].typeData.subMenu  = NULL;

   m[11].title             = IDS_AOC;
   m[11].itemType          = DT_BYTE;
   m[11].cfgItem           = CFGI_AOC;
   m[11].isEditable         = TRUE;

#ifdef FEATURE_ACP
   m[12].title              = IDS_MODE_PREF;
   m[12].itemType           = DT_RANGE;
   m[12].cfgItem            = CFGI_MODE_PREF;
   m[12].isEditable         = TRUE;
   m[12].typeData.rangeData = CSvcPrg_CreateRange(3);
   if (NULL == m[12].typeData.rangeData) {
      return FALSE;
   }
   m[12].typeData.rangeData[0].resId = IDS_MODE_AUTOMATIC;
   m[12].typeData.rangeData[0].value = OEMNV_MODE_AUTOMATIC;
   m[12].typeData.rangeData[1].resId = IDS_MODE_DIGITAL_ONLY;
   m[12].typeData.rangeData[1].value = OEMNV_MODE_DIGITAL_ONLY;
   m[12].typeData.rangeData[2].resId = IDS_MODE_ANALOG_ONLY;
   m[12].typeData.rangeData[2].value = OEMNV_MODE_ANALOG_ONLY;

   m[13].title              = IDS_ANALOG_OPTIONS;
   m[13].itemType           = DT_SUBMENU;
   m[13].typeData.subMenu   = NULL;

#endif /* FEATURE_ACP */
   
   // Store the Network menu in the Main menu
   ASSERT(IDS_NETWORK_OPTIONS == pMe->m_mainMenu[1].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[1].itemType);
   pMe->m_mainMenu[1].typeData.subMenu = m;

   //
   // Build the Registration submenu
   //

   m = CSvcPrg_CreateMenuList(3); // 3 elements in the Registration menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_HOME_SID_REG;
   m[0].itemType           = DT_BOOLEAN;
   m[0].cfgItem            = CFGI_HOME_SID_REG;
   m[0].isEditable         = TRUE;

   m[1].title              = IDS_FORN_SID_REG;
   m[1].itemType           = DT_BOOLEAN;
   m[1].cfgItem            = CFGI_FORN_SID_REG;
   m[1].isEditable         = TRUE;

   m[2].title              = IDS_FORN_NID_REG;
   m[2].itemType           = DT_BOOLEAN;
   m[2].cfgItem            = CFGI_FORN_NID_REG;
   m[2].isEditable         = TRUE;

   // Store the Registration menu pointer in the Network menu 
   ASSERT(IDS_NETWORK_OPTIONS == pMe->m_mainMenu[1].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[1].itemType);
   ASSERT(IDS_REGISTRATION == pMe->m_mainMenu[1].typeData.subMenu[10].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[1].typeData.subMenu[10].itemType);
   pMe->m_mainMenu[1].typeData.subMenu[10].typeData.subMenu = m;


#ifdef FEATURE_ACP
   //
   // Build the Analog Options submenu
   //

   m = CSvcPrg_CreateMenuList(3); // 3 elements in the Registration menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_AMPS_HOME_SID;
   m[0].itemType           = DT_WORD;
   m[0].cfgItem            = CFGI_AMPS_HOME_SID;
   m[0].isEditable         = TRUE;

   m[1].title              = IDS_AMPS_FIRSTCHP;
   m[1].itemType           = DT_WORD;
   m[1].cfgItem            = CFGI_AMPS_FIRSTCHP;
   m[1].isEditable         = TRUE;

   m[2].title              = IDS_AMPS_REG_TYPE;
   m[2].itemType           = DT_RANGE;
   m[2].cfgItem            = CFGI_AMPS_REG_TYPE;
   m[2].isEditable         = TRUE;

   m[2].typeData.rangeData = CSvcPrg_CreateRange(3);
   if (NULL == m[2].typeData.rangeData) {
      return FALSE;
   }
   m[2].typeData.rangeData[0].resId = IDS_AMPSREG_DISABLED;
   m[2].typeData.rangeData[0].value = OEMNV_AMPSREG_DISABLED;
   m[2].typeData.rangeData[1].resId = IDS_AMPSREG_WHEREABOUTS_KNOWN;
   m[2].typeData.rangeData[1].value = OEMNV_AMPSREG_WHEREABOUTS_KNOWN;
   m[2].typeData.rangeData[2].resId = IDS_AMPSREG_WHEREABOUTS_UNKNOWN;
   m[2].typeData.rangeData[2].value = OEMNV_AMPSREG_WHEREABOUTS_UNKNOWN;

   // Store the Analog Options menu pointer in the Network menu 
   ASSERT(IDS_NETWORK_OPTIONS == pMe->m_mainMenu[1].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[1].itemType);
   ASSERT(IDS_ANALOG_OPTIONS == pMe->m_mainMenu[1].typeData.subMenu[13].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[1].typeData.subMenu[13].itemType);
   pMe->m_mainMenu[1].typeData.subMenu[13].typeData.subMenu = m;
#endif /* FEATURE_ACP */

   //
   // Build the SMS Options menu
   //

   m = CSvcPrg_CreateMenuList(6); // 6 elements in the Network menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_SMS_TIMESTAMP;
   m[0].itemType           = DT_RANGE;
   m[0].cfgItem            = CFGI_SMS_TIMESTAMP; 
   m[0].isEditable         = TRUE;
   m[0].typeData.rangeData = CSvcPrg_CreateRange(2);
   if (NULL == m[0].typeData.rangeData) {
      return FALSE;
   }
   m[0].typeData.rangeData[0].resId = IDS_SMS_TIMESTAMP_ADJUST;
   m[0].typeData.rangeData[0].value = OEMNV_SMS_TIMESTAMP_ADJUST;
   m[0].typeData.rangeData[1].resId = IDS_SMS_TIMESTAMP_ASRECEIVED;
   m[0].typeData.rangeData[1].value = OEMNV_SMS_TIMESTAMP_ASRECEIVED;


   m[1].title              = IDS_MAX_MSG_PAYLOAD;
   m[1].itemType           = DT_DWORD;
   m[1].cfgItem            = CFGI_SMS_MAX_PAYLOAD_LENGTH;
   m[1].isEditable         = TRUE;

   /*
   m[2].title              = IDS_SMS_MO_ACCESS;
   m[2].itemType           = DT_RANGE;
   m[2].cfgItem            = CFGI_SMS_MO_ON_ACCESS_CHANNEL;
   m[2].isEditable         = TRUE;
   m[2].typeData.rangeData = CSvcPrg_CreateRange(2);
   if (NULL == m[2].typeData.rangeData) {
      return FALSE;
   }

   m[2].typeData.rangeData[0].resId = IDS_SMS_MO_ACCESS_OFF;
   m[2].typeData.rangeData[0].value = 0;
   m[2].typeData.rangeData[1].resId = IDS_SMS_MO_ACCESS_ON;
   m[2].typeData.rangeData[1].value = 1;

   m[3].title              = IDS_SMS_MO_TRAFFIC;
   m[3].itemType           = DT_RANGE;
   m[3].cfgItem            = CFGI_SMS_MO_ON_TRAFFIC_CHANNEL;
   m[3].isEditable         = TRUE;
   m[3].typeData.rangeData = CSvcPrg_CreateRange(2);
   if (NULL == m[3].typeData.rangeData) {
      return FALSE;
   }
   m[3].typeData.rangeData[0].resId = IDS_SMS_MO_TRAFFIC_OFF;
   m[3].typeData.rangeData[0].value = 0;
   m[3].typeData.rangeData[1].resId = IDS_SMS_MO_TRAFFIC_ON;
   m[3].typeData.rangeData[1].value = 1;
  */
   m[4].title              = IDS_SMS_IS41_WORKAROUND;
   m[4].itemType           = DT_RANGE;
   m[4].cfgItem            = CFGI_SMS_IS41_WORKAROUND;
   m[4].isEditable         = TRUE;
   m[4].typeData.rangeData = CSvcPrg_CreateRange(2);
   if (NULL == m[4].typeData.rangeData) {
      return FALSE;
   }
   m[4].typeData.rangeData[0].resId = IDS_SMS_IS41_WORKAROUND_ON;
   m[4].typeData.rangeData[0].value = OEMNV_SMS_EMAIL_ADDRESSING_IS41;
   m[4].typeData.rangeData[1].resId = IDS_SMS_IS41_WORKAROUND_OFF;
   m[4].typeData.rangeData[1].value = OEMNV_SMS_EMAIL_ADDRESSING_STANDARD;


   m[5].title              = IDS_SMS_MO_ENCODING;
   m[5].itemType           = DT_RANGE;
   m[5].cfgItem            = CFGI_SMS_MO_ENCODING; 
   m[5].isEditable         = TRUE;
   m[5].typeData.rangeData = CSvcPrg_CreateRange(2);
   if (NULL == m[5].typeData.rangeData) {
      return FALSE;
   }
   m[5].typeData.rangeData[0].resId = IDS_SMS_MO_ENCODING_7BIT;
   m[5].typeData.rangeData[0].value = OEMNV_SMS_MO_ENCODING_7BIT;
#if defined(FEATURE_CARRIER_ANGOLA_MOVICEL) || defined(FEATURE_CARRIER_MAROC_WANA)
   m[5].typeData.rangeData[1].resId = IDS_SMS_MO_ENCODING_OCTET;
   m[5].typeData.rangeData[1].value = OEMNV_SMS_MO_ENCODING_OCTET;
#else       
   m[5].typeData.rangeData[1].resId = IDS_SMS_MO_ENCODING_UNICODE;
   m[5].typeData.rangeData[1].value = OEMNV_SMS_MO_ENCODING_UNICODE;
#endif

   // Store the SMS Options menu in the Main menu
   ASSERT(IDS_SMS_OPTIONS == pMe->m_mainMenu[2].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[2].itemType);
   pMe->m_mainMenu[2].typeData.subMenu = m;


   //
   // Build the Data Options menu
   //

   m = CSvcPrg_CreateMenuList(2); // 2 elements in the Network menu
   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_QNC_ENABLED;
   m[0].itemType           = DT_BOOLEAN;
   m[0].cfgItem            = CFGI_DATA_QNC_ENABLED;
   m[0].isEditable         = TRUE;


   m[1].title              = IDS_DATA_DIALSTRING;
   m[1].itemType           = DT_WSTR;
   m[1].cfgItem            = CFGI_DATA_DIALSTRING;
   m[1].typeData.strLen    = OEMNV_DATA_DIALSTRING_MAXLEN; 
   m[1].isEditable         = TRUE;

   // Store the Data Options submenu in the Main menu
   ASSERT(IDS_DATA_OPTIONS == pMe->m_mainMenu[3].title);
   ASSERT(DT_SUBMENU == pMe->m_mainMenu[3].itemType);
   pMe->m_mainMenu[3].typeData.subMenu = m;
      
   return TRUE;    // Phew!  I'm sure glad that pain is over with.
}
#endif //FEATURE_CARRIER_VENEZUELA_MOVILNET

/*=============================================================================
FUNCTION: CSvcPrg_CreateMenuList

DESCRIPTION:  Creates an array of MenuItemTypes

PARAMETERS:
   numItems:  number of elements in the array

RETURN VALUE:
    MenuItemType *:  Newly allocated array, NULL if unsuccessful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static MenuItemType *CSvcPrg_CreateMenuList(uint16 numItems)
{
   MenuItemType *m;

   m = MALLOC(sizeof(*m) * numItems);
   
   // NOTE: OEM layer zeros out newly allocated memory so there's no
   //       need to do it a second time here in the Applet.

   if (m != NULL) {
      // Mark the 'isLast' flag in the last menu item in the array
      m[numItems-1].isLast = TRUE;
   }
   
   return m;
}


/*=============================================================================
FUNCTION: CSvcPrg_FreeMenuList

DESCRIPTION:  Deallocates the specified menu item list

PARAMETERS:
   *item: Array of menu item to deallocate.  The last element in this
          array must have the MenuItemType::isLast flag set to TRUE.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CSvcPrg_FreeMenuList(MenuItemType *itemList)
{
   int i;

   if (NULL == itemList) {
      return;
   }

   // Use a do ... while() loop because if itemList only contains
   // a single item, we still want to examine itemList->itemType because
   // it may be a submenu or something
   i = -1;
   do {
      i++;

      switch (itemList[i].itemType) {
      case DT_SUBMENU:
         CSvcPrg_FreeMenuList(itemList[i].typeData.subMenu);
         break;
      case DT_RANGE:
         if (itemList[i].typeData.rangeData != NULL) {
            FREE(itemList[i].typeData.rangeData);
         }
         break;
      default:
         break;
      }

   } while (!itemList[i].isLast);

   FREE(itemList);
}


/*=============================================================================
FUNCTION: CSvcPrg_CreateRange

DESCRIPTION:  Creates an array of RangeItemTypes

PARAMETERS:
   numItems:  number of elements in the array

RETURN VALUE:
    RangeItemType *: Newly allocated array, NULL if unsuccessful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static RangeItemType *CSvcPrg_CreateRange(uint16 numItems)
{
   RangeItemType *r;

   r = MALLOC(sizeof(*r) * (numItems + 1));  // The extra item is used as 
                                             // a sentinal to indicate the
                                             // end of the range.

   // NOTE: OEM layer zeros out newly allocated memory so there's no
   //       need to do it a second time here in the Applet

   return r;
}


/*=============================================================================
FUNCTION: CSvcPrg_DisplayItem

DESCRIPTION:  Displays the specified menu item

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   *item:  menu item to display

RETURN VALUE:
   boolean: TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_DisplayItem(CSvcPrgApp      *pMe, 
                                   MenuItemType    *item)
{
   uint16    dlgID;
   IDialog  *pd = NULL;
   IMenuCtl *pm = NULL;
   ITextCtl *pt = NULL;
  
   // Determine what kind of dialog to display based on the item type...
   switch (item->itemType) {
   case DT_BOOLEAN:
   case DT_SUBMENU:
   case DT_RANGE:
   case DT_SIDNIDLIST:
#ifdef FEATURE_NV_VOICE_PARAMETER   
   case DT_BOOLEAN_VOCCAL:
#endif //FEATURE_NV_VOICE_PARAMETER  
      dlgID = IDD_GENERICMENU;
      break;

   case DT_AKEY:
   default:
      dlgID = IDD_GENERICTEXTSK;
      break;
   }

   pd = CSvcPrg_CreateDialog(pMe, dlgID);
   if (NULL == pd) {
      return FALSE;
   }

   if (dlgID != IDIALOG_GetID(pd)) {
      // Something is amiss!!!
      return FALSE;
   }

   // Get pointers to the dialog controls
   switch (dlgID) {
   case IDD_GENERICMENU:
      pm = (IMenuCtl *) IDIALOG_GetControl(pd, IDC_GENERICMENU_MENU);
      break;
   case IDD_GENERICTEXTSK:
      pm = (IMenuCtl *) IDIALOG_GetControl(pd, IDC_GENERICTEXTSK_SK);
      pt = (ITextCtl *) IDIALOG_GetControl(pd, IDC_GENERICTEXTSK_TEXT);
      break;
   default:
      ASSERT_NOT_REACHABLE
      return FALSE;
   }
         

   // Do any item type specific initialization..
   switch (item->itemType) {
   case DT_ALPHA:
   case DT_WSTR:
   case DT_WSTR_FIXED:
   case DT_BYTE:
   case DT_WORD:
   case DT_DWORD:
   case DT_ESN:
   case DT_AKEY:
#ifdef FEATURE_NV_VOICE_PARAMETER    
   case DT_WORD_VOCCAL:
#endif //FEATURE_NV_VOICE_PARAMETER  
      if (!pt || !pm) {
         return FALSE;
      }
      (void) ITEXTCTL_SetTitle(pt,
                               AEE_SVCPRG_RES_FILE,
                               item->title,
                               NULL);
      (void) ITEXTCTL_SetText(pt, NULL, -1);
      if (item->itemType == DT_AKEY) {
	  	
        (void) IMENUCTL_AddItem(pm, 
                                AEE_SVCPRG_RES_FILE,
                                IDS_CMD_AKEY,
                                IDS_CMD_AKEY,
                                NULL,
                                (uint32) item);
		(void) IMENUCTL_AddItem(pm, 
                                 AEE_SVCPRG_RES_FILE,
                                 IDS_CMD_CANCEL,
                                 IDS_CMD_CANCEL,
                                 NULL,
                                 (uint32) item);
                                
      }
      else {

		
	
        (void) IMENUCTL_AddItem(pm, 
                                AEE_SVCPRG_RES_FILE,
                                IDS_CMD_OK,
                                IDS_CMD_OK,
                                NULL,
                                (uint32) item);
	(void) IMENUCTL_AddItem(pm, 
                                 AEE_SVCPRG_RES_FILE,
                                 IDS_CMD_NID_CANCEL,
                                 IDS_CMD_NID_CANCEL,
                                 NULL,
                                 (uint32) item);
                                           
      }

      if ((item->itemType != DT_AKEY) && (item->isEditable)) {
	 IMENUCTL_DeleteItem (pm, IDS_CMD_NID_CANCEL);
	
     (void) IMENUCTL_AddItem(pm, 
                                 AEE_SVCPRG_RES_FILE,
                                 IDS_CMD_EDIT,
                                 IDS_CMD_EDIT,
                                 NULL,
                                 (uint32) item);
                              
         //   MENU_SETBOTTOMBAR(pm,BTBAR_CONTINU_DELETE); 
      }
      
      if (item->itemType != DT_AKEY) {
        CSvcPrg_LoadTextCtlWithItemVal(pMe, pt, item);
      }

      // The ESN item type requires two lines to display so make the 
      // Text Control a multiline control
      //if (DT_ESN == item->itemType) 
      if ((DT_ESN == item->itemType)  || (item->itemType == DT_ALPHA))
      {
         AEERect rcSK, rcText;
         
         ITEXTCTL_SetProperties(pt, 
                                TP_MULTILINE | ITEXTCTL_GetProperties(pt)|TP_FOCUS_NOSEL);

         // BREW Bug:
         // 
         // Even though the dialog startup code told the Text Control to use
         // the entire display minus the softkey area, it is currently
         // only using two lines (one for the Title, and one for the original
         // line of text).   
         //
         // The Text Control has forgotten that it can use the entire display 
         // for the text body and is only using a single line.  Calling 
         // ITEXTCTL_SetRect() a second time will refresh its' memory.
         //
         IMENUCTL_GetRect(pm, &rcSK);
         SETAEERECT(&rcText, 
                    pMe->m_rc.x, 
                    pMe->m_rc.y, 
                    pMe->m_rc.dx,
                    pMe->m_rc.dy - rcSK.dy);

         ITEXTCTL_SetRect(pt, &rcText);
      }

      if (item->itemType != DT_AKEY) {
        // Always start with focus on the softkey
        (void) IDIALOG_SetFocus(pd,
                                IDC_GENERICTEXTSK_SK);

        ITEXTCTL_SetActive(pt, FALSE);
      }
      else {
        //(void) ITEXTCTL_SetInputMode(pt, AEE_TM_NUMBERS);
        ITEXTCTL_SetMaxSize(pt, item->typeData.strLen-1);
        ITEXTCTL_SetActive(pt, TRUE);
      }
      if ((item->cfgItem == CFGI_DNS_IP1) ||
         (item->cfgItem == CFGI_DNS_IP2))
      {
          ITEXTCTL_SetProperties(pt, TP_MULTILINE | TP_STARKEY_SWITCH | ITEXTCTL_GetProperties(pt));
      }
      
      if (item->itemType == DT_ALPHA)
      {
          (void) ITEXTCTL_SetInputMode(pt, AEE_TM_LETTERS);
      }
      else
      {
          (void) ITEXTCTL_SetInputMode(pt, AEE_TM_NUMBERS);
      }
      
      (void) ITEXTCTL_Redraw(pt);
      (void) IMENUCTL_Redraw(pm);
      IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
      break;

   case DT_SUBMENU:
      {
         MenuItemType *m = item->typeData.subMenu;

         if (!pm) {
            return FALSE;
         }
         #if 0
         (void) IMENUCTL_SetTitle(pm, 
                                  AEE_SVCPRG_RES_FILE, 
                                  item->title,
                                  NULL);
		 #else
		  {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->a.m_pIShell,
                        AEE_SVCPRG_RES_FILE,                                
                        item->title,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		  }
		  #endif

     
         // Add in the menu options
         //
         ASSERT(m != NULL);
         for (;;) {
            (void) IMENUCTL_AddItem(pm, 
                                    AEE_SVCPRG_RES_FILE,
                                    m->title,
                                    m->title,
                                    NULL,
                                    (uint32) m);
            if (m->isLast) {
               break;
            }
            m++;
         }
         
         if (item->title == IDS_SERVICE_OPTIONS) {
#ifdef FEATURE_ENABLE_OTKSL
            // The service programming code may not be viewed/changed when access
            // to the applet was granted by using the OTKSL code.
           if (TRUE == pMe->m_accessByOTKSL) {
             IMENUCTL_DeleteItem (pm, IDS_SPCCODE);
           }
#endif /* FEATURE_ENABLE_OTKSL */

#ifndef FEATURE_AUTH
           /* Delete AKEY menu if feature not defined */
          IMENUCTL_DeleteItem (pm, IDS_AKEY);
#endif

         }
	   MENU_SETBOTTOMBAR(pm, BTBAR_SELECT_BACK);
         (void) IMENUCTL_Redraw(pm);
         break;
      }
      
   case DT_BOOLEAN:
      {
         boolean val;
         
         if (!pm) {
            return FALSE;
         }
         #if 0
         (void) IMENUCTL_SetTitle(pm, 
                                  AEE_SVCPRG_RES_FILE, 
                                  item->title,
                                  NULL);
		 #else
		  {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->a.m_pIShell,
                        AEE_SVCPRG_RES_FILE,                                
                        item->title,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		  }
		  #endif

         // booleans are assumed to always be editable
         ASSERT(item->isEditable);  

         if (IDS_KEY_SPEC == item->title)
         {
             (void) IMENUCTL_AddItem(pm, 
                                     AEE_SVCPRG_RES_FILE,
                                     IDS_USEAKEY,
                                     IDS_CMD_YES,
                                     NULL,
                                     (uint32) item);
         }
         else
         (void) IMENUCTL_AddItem(pm, 
                                 AEE_SVCPRG_RES_FILE,
                                 IDS_CMD_YES,
                                 IDS_CMD_YES,
                                 NULL,
                                 (uint32) item);

         if (IDS_KEY_SPEC == item->title)
         {
             (void) IMENUCTL_AddItem(pm, 
                                     AEE_SVCPRG_RES_FILE,
                                     IDS_USEBKEY,
                                     IDS_CMD_NO,
                                     NULL,
                                     (uint32) item);
         }
         else
         (void) IMENUCTL_AddItem(pm, 
                                 AEE_SVCPRG_RES_FILE,
                                 IDS_CMD_NO,
                                 IDS_CMD_NO,
                                 NULL,
                                 (uint32) item);

         if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                        item->cfgItem,
                                        &val,
                                        sizeof(val))) {
            ERR("Unable to retrieve config item: %d", 
                (int) item->cfgItem, 0, 0);
            val = FALSE;
         }

         SetMenuIcon(pm, IDS_CMD_YES, val);
         SetMenuIcon(pm, IDS_CMD_NO,  !val); /*lint !e730*/
                                             // 'boolean argument to function'
         MENU_SETBOTTOMBAR(pm, BTBAR_SELECT_BACK);
         (void) IMENUCTL_Redraw(pm);
         break;
      }
   
#ifdef FEATURE_NV_VOICE_PARAMETER
   case DT_BOOLEAN_VOCCAL:
      {
         boolean val=0;
         nv_item_type nvi; 
         
         if (!pm) {
            return FALSE;
         }
		 #if 0
         (void) IMENUCTL_SetTitle(pm, 
                                  AEE_SVCPRG_RES_FILE, 
                                  item->title,
                                  NULL);
		 #else
		  {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->a.m_pIShell,
                        AEE_SVCPRG_RES_FILE,                                
                        item->title,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		  }
		  #endif

         // booleans are assumed to always be editable
         ASSERT(item->isEditable);  

         (void) IMENUCTL_AddItem(pm, 
                                 AEE_SVCPRG_RES_FILE,
                                 IDS_CMD_YES,
                                 IDS_CMD_YES,
                                 NULL,
                                 (uint32) item);

         (void) IMENUCTL_AddItem(pm, 
                                 AEE_SVCPRG_RES_FILE,
                                 IDS_CMD_NO,
                                 IDS_CMD_NO,
                                 NULL,
                                 (uint32) item);

          if (NV_DONE_S != OEMNV_Get(item->nvitem,&nvi))
          {
          
             ERR("Unable to retrieve config item: %d", 
                 (int) item->nvitem, 0, 0);
             return FALSE;             
          }
          
          if(IDS_VOCCAL_USE_NVMODE == item->title)
          {
             switch(item->nvitem)
              {
                  case NV_VOC_PCM_ON_CHIP_0_CAL_I:
                      val = nvi.nv_voc_pcm_on_chip_0_cal.use_nvmode;
                      break;
     
                  case NV_VOC_PCM_ON_CHIP_1_CAL_I:
                      val = nvi.nv_voc_pcm_on_chip_1_cal.use_nvmode;
                      break;
     
                  case NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I:
                      val = nvi.nv_voc_pcm_on_chip_speaker_cal.use_nvmode;
                      break; 

                  case NV_VOC_CAL_EC_PARAMS_ESEC_I:
                      val = nvi.nv_voc_cal_ec_params_esec.use_nvmode;
                      break; 

                  case NV_VOC_CAL_EC_PARAMS_HEADSET_I:
                      val = nvi.nv_voc_cal_ec_params_headset.use_nvmode;
                      break; 

                  case NV_VOC_CAL_EC_PARAMS_AEC_I:
                      val = nvi.nv_voc_cal_ec_params_aec.use_nvmode;
                      break; 

                  case NV_VOC_CAL_EC_PARAMS_SPEAKER_I:
                      val = nvi.nv_voc_cal_ec_params_speaker.use_nvmode;
                      break;                       
   
                  default:
                      val = FALSE;
                      break;
              }
          }
         SetMenuIcon(pm, IDS_CMD_YES, val);
         SetMenuIcon(pm, IDS_CMD_NO,  !val); /*lint !e730*/
                                             // 'boolean argument to function'
         MENU_SETBOTTOMBAR(pm, BTBAR_SELECT_BACK);
         (void) IMENUCTL_Redraw(pm);
         break;
      }
#endif //FEATURE_NV_VOICE_PARAMETER

   case DT_RANGE:
      {
         RangeItemType *r;
         uint8          val;
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
         nv_rtre_configuration_type rtre_config;
#endif

         if (!pm) {
            return FALSE;
         }
         #if 0
         (void) IMENUCTL_SetTitle(pm, 
                                  AEE_SVCPRG_RES_FILE, 
                                  item->title,
                                  NULL);
		 #else
		  {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->a.m_pIShell,
                        AEE_SVCPRG_RES_FILE,                                
                        item->title,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		  }
		  #endif
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
         if (item->cfgItem == CFGI_RTRE_CONFIGURATION)
         {
             if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                            item->cfgItem,
                                            &rtre_config,
                                            sizeof(rtre_config))) 
             {
                ERR("Unable to retrieve config item: %d", 
                    (int) item->cfgItem, 0, 0);
                val = (uint8) item->typeData.rangeData[0].value;
             }
             else
             {
                val = (uint8) rtre_config;
             }
         }
         else
#endif
         if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                        item->cfgItem,
                                        &val,
                                        sizeof(val))) {
            ERR("Unable to retrieve config item: %d", 
                (int) item->cfgItem, 0, 0);
            val = (uint8) item->typeData.rangeData[0].value;
         }

         // Add in the menu options
         for (r = item->typeData.rangeData; 
              r->resId != 0 || r->value != 0; 
              r++) {
            (void) IMENUCTL_AddItem(pm, 
                                    AEE_SVCPRG_RES_FILE,
                                    r->resId,
                                    RANGE_ITEMID_FLAG | r->value,
                                    NULL,
                                    (uint32) item);
            
            SetMenuIcon(pm, 
                        RANGE_ITEMID_FLAG | r->value,
                        ( val == r->value ) ? TRUE : FALSE);
            
            // Select the current value (so it will scroll into
            // view when there are more than a screenful of options)
            if (val == r->value) {
               IMENUCTL_SetSel(pm, 
                               RANGE_ITEMID_FLAG | r->value);
            }
         }
         MENU_SETBOTTOMBAR(pm,BTBAR_SELECT_BACK); 
         (void) IMENUCTL_Redraw(pm);
         break;
      }

   case DT_SIDNIDLIST:
      {
         uint16                   i;
         AEEConfigSIDNIDPairType *p;

         if (!pm) {
            return FALSE;
         }

         p = MALLOC(sizeof(*p) * item->typeData.sidnid.count);
         if (NULL == p) {
            return FALSE;
         }

         if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                        item->cfgItem,
                                        (void *) p,
                                        sizeof(*p) * 
                                                item->typeData.sidnid.count)) {
            ERR("Unable to retrieve config item: %d", 
                (int) item->cfgItem, 0, 0);
            FREE((void *) p);
            return FALSE;
         }
		  #if 0
         (void) IMENUCTL_SetTitle(pm, 
                                  AEE_SVCPRG_RES_FILE, 
                                  item->title,
                                  NULL);
		  #else
		  {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->a.m_pIShell,
                        AEE_SVCPRG_RES_FILE,                                
                        item->title,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		  }
		  #endif

        
         // Add in the menu options
         //
         for (i = 0; i < item->typeData.sidnid.count; i++) {
            char   str[32];
            AECHAR wStr[32];
            
            SPRINTF(str, "%d/%d", p[i].sid, p[i].nid);
            STR_TO_WSTR(str, wStr, sizeof(wStr));

            (void) IMENUCTL_AddItem(pm, 
                                    NULL,
                                    0,
                                    SIDNID_ITEMID_FLAG | i,
                                    wStr,
                                    (uint32) item);
         }

         FREE((void *)p);
         MENU_SETBOTTOMBAR(pm, BTBAR_SELECT_BACK);
         (void) IMENUCTL_Redraw(pm);
         break;
      }

   default:
      ASSERT_NOT_REACHABLE
      return FALSE;
   }
   return TRUE;
}


/*=============================================================================
FUNCTION: CSvcPrg_LoadTextCtlWithItemVal

DESCRIPTION:  Loads the 'value' associated with a menu item into a 
              Text Control object.

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   *pt: destination text control
   *item: menu item

RETURN VALUE:
   None

COMMENTS:
   This function must not be used with the following menu item types:
      DT_BOOLEAN
      DT_SUBMENU
      DT_SIDNIDLIST
      DT_RANGE

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CSvcPrg_LoadTextCtlWithItemVal(CSvcPrgApp      *pMe, 
                                           ITextCtl        *pt, 
                                           MenuItemType    *item)
{
   PARAM_NOT_REF(pMe)

   char   szBuf[32];  // 32 seems like a reasonably large number
   AECHAR  wBuf[32];  
         
   szBuf[0] = '\0';
   wBuf[0]  = (AECHAR) 0;

   switch (item->itemType) {
   case DT_ALPHA:
   case DT_WSTR:
   case DT_WSTR_FIXED:
      {
         int maxLen;

         if (sizeof(wBuf) < (item->typeData.strLen) * sizeof(AECHAR)) {
            ERR("wBuf is too small!", 0, 0, 0);
            maxLen = sizeof(wBuf);
         } else {
            maxLen = (item->typeData.strLen) * sizeof(AECHAR);
         }

         if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                        item->cfgItem,
                                        (void *) wBuf,
                                        maxLen)) {

            ERR("Unable to retrieve config item: %d", 
                (int) item->cfgItem, 0, 0);
         }

         ITEXTCTL_SetMaxSize(pt, item->typeData.strLen-1);
         break;
      }
      

   case DT_BYTE:
      {
         uint8 b;

         if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                        item->cfgItem,
                                        &b,
                                        sizeof(b))) {
            ERR("Unable to retrieve config item: %d", 
                (int) item->cfgItem, 0, 0);
            b = 0;
         }

         ITEXTCTL_SetMaxSize(pt, 3 /* max chars in a 8-bit decimal number */);
         
         SNPRINTF(szBuf, sizeof(szBuf), "%u", b);
         STR_TO_WSTR(szBuf, wBuf, sizeof(wBuf));
         break;
      }

   case DT_WORD:
      {
         uint16 w;

         if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                        item->cfgItem,
                                        &w,
                                        sizeof(w))) {
            ERR("Unable to retrieve config item: %d", 
                (int) item->cfgItem, 0, 0);
            w = 0;
         }
         ITEXTCTL_SetMaxSize(pt, 5 /* max chars in a 16-bit decimal number*/);
         
         SNPRINTF(szBuf, sizeof(szBuf), "%u", w);
         STR_TO_WSTR(szBuf, wBuf, sizeof(wBuf));
         break;
      }

   case DT_DWORD:
      {
         uint32 dw;

         if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                        item->cfgItem,
                                        &dw,
                                        sizeof(dw))) {
            ERR("Unable to retrieve config item: %d", 
                (int) item->cfgItem, 0, 0);
            dw = 0;
         }
         if ((item->cfgItem == CFGI_DNS_IP1) ||
             (item->cfgItem == CFGI_DNS_IP2))
         {
            ITEXTCTL_SetMaxSize(pt, 15/* max chars in a IP string*/);
            if (!IPAdd_FromNetValueToString(dw, szBuf, sizeof(szBuf)))
            {
                STRTOWSTR("0.0.0.0", wBuf, sizeof(wBuf));
            }
            else
            {
                STRTOWSTR(szBuf, wBuf, sizeof(wBuf));
            }
         }
         else
         {
            ITEXTCTL_SetMaxSize(pt, 10/* max chars in a 16-bit decimal number*/);
    
            SNPRINTF(szBuf, sizeof(szBuf), "%u", dw);
            STR_TO_WSTR(szBuf, wBuf, sizeof(wBuf));
         }
         break;
      }
   
#ifdef FEATURE_NV_VOICE_PARAMETER
   case DT_WORD_VOCCAL:
      {  
         uint16 w = 0;
         nv_item_type nvi;

         if (NV_DONE_S != OEMNV_Get(item->nvitem,&nvi))
         {

            ERR("Unable to retrieve config item: %d", 
                (int) item->nvitem, 0, 0);
            w = 0;            
         }    
         else
         {
            switch(item->nvitem)
            {
                case NV_VOC_PCM_ON_CHIP_0_CAL_I:
                    switch (item->title)
                    {  
                        case IDS_VOCCAL_EC_MODE:  
                            w = nvi.nv_voc_pcm_on_chip_0_cal.ec_mode;
                            break;
                          
                        case IDS_VOCCAL_NS_ENABLE:
                            w = nvi.nv_voc_pcm_on_chip_0_cal.ns_enable;
                            break;

                        case IDS_VOCCAL_TX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_0_cal.tx_gain;
                            break;

                        case IDS_VOCCAL_DTMF_TX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_0_cal.dtmf_tx_gain;
                            break;

                        case IDS_VOCCAL_CODEC_TX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_0_cal.codec_tx_gain;
                            break;

                        case IDS_VOCCAL_CODEC_RX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_0_cal.codec_rx_gain;
                            break;

                        case IDS_VOCCAL_CODEC_ST_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_0_cal.codec_st_gain;
                            break;    

                        case IDS_VOCCAL_RX_DBM_OFFSET:
                            w = nvi.nv_voc_pcm_on_chip_0_cal.rx_dbm_offset;
                            break;  

                        default:
                            ASSERT_NOT_REACHABLE
                            break;  
                    }                       
                    break;


                case NV_VOC_PCM_ON_CHIP_1_CAL_I:
                    switch (item->title)
                    {  
                        case IDS_VOCCAL_EC_MODE:  
                            w = nvi.nv_voc_pcm_on_chip_1_cal.ec_mode;
                            break;
                          
                        case IDS_VOCCAL_NS_ENABLE:
                            w = nvi.nv_voc_pcm_on_chip_1_cal.ns_enable;
                            break;

                        case IDS_VOCCAL_TX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_1_cal.tx_gain;
                            break;

                        case IDS_VOCCAL_DTMF_TX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_1_cal.dtmf_tx_gain;
                            break;

                        case IDS_VOCCAL_CODEC_TX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_1_cal.codec_tx_gain;
                            break;

                        case IDS_VOCCAL_CODEC_RX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_1_cal.codec_rx_gain;
                            break;

                        case IDS_VOCCAL_CODEC_ST_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_1_cal.codec_st_gain;
                            break;    

                        case IDS_VOCCAL_RX_DBM_OFFSET:
                            w = nvi.nv_voc_pcm_on_chip_1_cal.rx_dbm_offset;
                            break;  

                        default:
                            ASSERT_NOT_REACHABLE
                            break;  
                    }                       
                    break;

                case NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I:
                    switch (item->title)
                    {  
                        case IDS_VOCCAL_EC_MODE:  
                            w = nvi.nv_voc_pcm_on_chip_speaker_cal.ec_mode;
                            break;
                          
                        case IDS_VOCCAL_NS_ENABLE:
                            w = nvi.nv_voc_pcm_on_chip_speaker_cal.ns_enable;
                            break;

                        case IDS_VOCCAL_TX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_speaker_cal.tx_gain;
                            break;

                        case IDS_VOCCAL_DTMF_TX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_speaker_cal.dtmf_tx_gain;
                            break;

                        case IDS_VOCCAL_CODEC_TX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_speaker_cal.codec_tx_gain;
                            break;

                        case IDS_VOCCAL_CODEC_RX_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_speaker_cal.codec_rx_gain;
                            break;

                        case IDS_VOCCAL_CODEC_ST_GAIN:
                            w = nvi.nv_voc_pcm_on_chip_speaker_cal.codec_st_gain;
                            break;    

                        case IDS_VOCCAL_RX_DBM_OFFSET:
                            w = nvi.nv_voc_pcm_on_chip_speaker_cal.rx_dbm_offset;
                            break;  

                        default:
                            ASSERT_NOT_REACHABLE
                            break;  
                    }                       
                    break;

                case NV_VOC_CAL_EC_PARAMS_ESEC_I:
                    switch (item->title)
                    {  
                        case IDS_ECHO_CANCEL_FAREND:  
                            w = nvi.nv_voc_cal_ec_params_esec.farend_hangover_thresh;
                            break;
                          
                        case IDS_ECHO_CANCEL_DOUBLETALK:
                            w = nvi.nv_voc_cal_ec_params_esec.doubletalk_hangover_thresh;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER:
                            w = nvi.nv_voc_cal_ec_params_esec.startup_mute_hangover_thresh;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_MUTE_MODE:
                            w = nvi.nv_voc_cal_ec_params_esec.startup_mute_mode;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_ERLE:
                            w = nvi.nv_voc_cal_ec_params_esec.startup_erle_thresh;
                            break;

                        default:
                            ASSERT_NOT_REACHABLE
                            break;  
                    }                       
                    break;                    

                case NV_VOC_CAL_EC_PARAMS_HEADSET_I:
                    switch (item->title)
                    {  
                        case IDS_ECHO_CANCEL_FAREND:  
                            w = nvi.nv_voc_cal_ec_params_headset.farend_hangover_thresh;
                            break;
                          
                        case IDS_ECHO_CANCEL_DOUBLETALK:
                            w = nvi.nv_voc_cal_ec_params_headset.doubletalk_hangover_thresh;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER:
                            w = nvi.nv_voc_cal_ec_params_headset.startup_mute_hangover_thresh;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_MUTE_MODE:
                            w = nvi.nv_voc_cal_ec_params_headset.startup_mute_mode;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_ERLE:
                            w = nvi.nv_voc_cal_ec_params_headset.startup_erle_thresh;
                            break;

                        default:
                            ASSERT_NOT_REACHABLE
                            break;  
                    }                       
                    break;  

                case NV_VOC_CAL_EC_PARAMS_AEC_I:
                    switch (item->title)
                    {  
                        case IDS_ECHO_CANCEL_FAREND:  
                            w = nvi.nv_voc_cal_ec_params_aec.farend_hangover_thresh;
                            break;
                          
                        case IDS_ECHO_CANCEL_DOUBLETALK:
                            w = nvi.nv_voc_cal_ec_params_aec.doubletalk_hangover_thresh;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER:
                            w = nvi.nv_voc_cal_ec_params_aec.startup_mute_hangover_thresh;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_MUTE_MODE:
                            w = nvi.nv_voc_cal_ec_params_aec.startup_mute_mode;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_ERLE:
                            w = nvi.nv_voc_cal_ec_params_aec.startup_erle_thresh;
                            break;

                        default:
                            ASSERT_NOT_REACHABLE
                            break;  
                    }                       
                    break;  

                case NV_VOC_CAL_EC_PARAMS_SPEAKER_I:
                    switch (item->title)
                    {  
                        case IDS_ECHO_CANCEL_FAREND:  
                            w = nvi.nv_voc_cal_ec_params_speaker.farend_hangover_thresh;
                            break;
                          
                        case IDS_ECHO_CANCEL_DOUBLETALK:
                            w = nvi.nv_voc_cal_ec_params_speaker.doubletalk_hangover_thresh;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER:
                            w = nvi.nv_voc_cal_ec_params_speaker.startup_mute_hangover_thresh;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_MUTE_MODE:
                            w = nvi.nv_voc_cal_ec_params_speaker.startup_mute_mode;
                            break;

                        case IDS_ECHO_CANCEL_STARTUP_ERLE:
                            w = nvi.nv_voc_cal_ec_params_speaker.startup_erle_thresh;
                            break;

                        default:
                            ASSERT_NOT_REACHABLE
                            break;  
                    }                       
                    break;                      

                default:
                    break;
            }
         }

         ITEXTCTL_SetMaxSize(pt, 5 /* max chars in a 16-bit decimal number*/);         
         SNPRINTF(szBuf, sizeof(szBuf), "%u", w);
         STR_TO_WSTR(szBuf, wBuf, sizeof(wBuf));
         
         break;
      }
#endif //FEATURE_NV_VOICE_PARAMETER   

   case DT_ESN:
      {
         uint32 esn;

         if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                        item->cfgItem,
                                        &esn,
                                        sizeof(esn))) {
            ERR("Unable to retrieve config item: %d", 
                (int) item->cfgItem, 0, 0);
            esn = 0;
         }
         
         SNPRINTF(szBuf, 
                  sizeof(szBuf), 
                  "%u\n%u",
                  (esn & 0xFF000000) >> 24,
                  esn & 0x00FFFFFF);

         STR_TO_WSTR(szBuf, wBuf, sizeof(wBuf));
         break;
      }


   // This function does not work with the following item types...
   case DT_BOOLEAN:
   case DT_SUBMENU:
   case DT_SIDNIDLIST:
   case DT_RANGE:
   case DT_AKEY:
#ifdef FEATURE_NV_VOICE_PARAMETER  
   case DT_BOOLEAN_VOCCAL:
#endif //FEATURE_NV_VOICE_PARAMETER  
      ASSERT_NOT_REACHABLE
      break;
      
   default:
      // Has somebody added a new item and not updated this switch()? 
      ASSERT_NOT_REACHABLE
      break;
   }

   (void) ITEXTCTL_SetText(pt, 
                           wBuf,
                           -1);
}


/*=============================================================================
FUNCTION: CSvcPrg_SaveItemValue

DESCRIPTION:  Saves a new value for the specific menu item.

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   *item: menu item
   *wNewVal: new value (in string form)

RETURN VALUE:
   None

COMMENTS:
   This function must not be used with the following menu item types:
      DT_BOOLEAN
      DT_SUBMENU
      DT_SIDNIDLIST
      DT_RANGE

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CSvcPrg_SaveItemValue(CSvcPrgApp      *pMe,
                                  MenuItemType    *item,
                                  AECHAR          *wNewVal)
{
   switch (item->itemType) {
   case DT_ALPHA:
   case DT_WSTR:
   case DT_WSTR_FIXED:
      if (SUCCESS != ICONFIG_SetItem(pMe->m_pConfig, 
                                     item->cfgItem,
                                     wNewVal,
                                     WSTRSIZE(wNewVal))) {
         ERR("Unable to save config item: %d", 
             (int) item->cfgItem, 0, 0);
      }
	else if(item->title == IDS_IMSI_S)
      {       
         if(wNewVal)
         {
            int i=0,j=0;

            unsigned long long val=0,key=0;
	    char *p;
			
            uint8 k;
            while(*wNewVal!='\0')
            {
               val = *wNewVal-48;
               ERR("val = %d !", val,0,0);
               for(j = 1; j< (20 - i); j++)
               {
                   val = val*10;
                   i++;
                   wNewVal++;
                   key+=val;
		  MSG_ERROR("88444444444%s,%d,%s",key,key,wNewVal);
                }
		    
		    MSG_ERROR("88444444444%s,%d,%s",key,key,wNewVal);
		  k=key%10;
			   
            }
		--wNewVal;	
	     p = (char*)wNewVal;	
	     k = (uint8)ATOI(p);	
            ICONFIG_SetItem(pMe->m_pConfig, 
                                     CFGI_AOC,
                                     &k,
                                     sizeof(k));
         }
      }
      break;

   case DT_BYTE:
      {
         uint8 b;
         char  szBuf[4];  // max chars in a 8-bit decimal number plus a NULL byte

         WSTR_TO_STR(wNewVal, szBuf, sizeof(szBuf));
         b = (uint8) ATOI(szBuf);

         if (SUCCESS != ICONFIG_SetItem(pMe->m_pConfig, 
                                        item->cfgItem,
                                        &b,
                                        sizeof(b))) {
            ERR("Unable to save config item: %d", 
                (int) item->cfgItem, 0, 0);
         }
         break;
      }

   case DT_WORD:
      {
         uint16 w;
         char   szBuf[6]; // max chars in a 16-bit decimal number plus a NULL byte

         WSTR_TO_STR(wNewVal, szBuf, sizeof(szBuf));
         w = (uint16) ATOI(szBuf);

         if (SUCCESS != ICONFIG_SetItem(pMe->m_pConfig, 
                                        item->cfgItem,
                                        &w,
                                        sizeof(w))) {
            ERR("Unable to save config item: %d", 
                (int) item->cfgItem, 0, 0);
         }
         break;
      }

   case DT_DWORD:
      {
         uint32 dw;
         char   szBuf[11]; // max chars in a 32-bit decimal number plus a NULL byte

         if ((item->cfgItem == CFGI_DNS_IP1) ||
             (item->cfgItem == CFGI_DNS_IP2))
         {
            char  strbuf[20];
            
            WSTRTOSTR(wNewVal, strbuf, sizeof(strbuf));
            IPAdd_FromStringToNetValue(&dw, strbuf);
         }
         else
         {
            WSTR_TO_STR(wNewVal, szBuf, sizeof(szBuf));
            dw = (uint32) ATOI(szBuf);
         }

         if (SUCCESS != ICONFIG_SetItem(pMe->m_pConfig, 
                                        item->cfgItem,
                                        &dw,
                                        sizeof(dw))) {
            ERR("Unable to save config item: %d", 
                (int) item->cfgItem, 0, 0);
         }
         break;
      }
   
#ifdef FEATURE_NV_VOICE_PARAMETER
   case DT_WORD_VOCCAL:
        {
         nv_item_type nvi;          
         uint16 w;
         char   szBuf[6]; // max chars in a 16-bit decimal number plus a NULL byte

         WSTR_TO_STR(wNewVal, szBuf, sizeof(szBuf));
         w = (uint16) ATOI(szBuf);

         if (NV_DONE_S != OEMNV_Get(item->nvitem,&nvi))
         {

            ERR("Unable to retrieve config item: %d", 
                (int) item->nvitem, 0, 0);   
            return;
         }
            
        switch(item->nvitem)
        {
            case NV_VOC_PCM_ON_CHIP_0_CAL_I:
                if(FALSE == nvi.nv_voc_pcm_on_chip_0_cal.use_nvmode)
                {
                    return;
                }                
                switch (item->title)
                {  
                    case IDS_VOCCAL_EC_MODE:  
                        if(w > 5) 
                        {
                            return;
                        }
                        nvi.nv_voc_pcm_on_chip_0_cal.ec_mode = w;
                        break;
                      
                    case IDS_VOCCAL_NS_ENABLE:
                         if(w > 2) 
                        {
                            return;
                        }                       
                        nvi.nv_voc_pcm_on_chip_0_cal.ns_enable = w;
                        break;

                    case IDS_VOCCAL_TX_GAIN:
                        nvi.nv_voc_pcm_on_chip_0_cal.tx_gain = w;
                        break;

                    case IDS_VOCCAL_DTMF_TX_GAIN:
                        nvi.nv_voc_pcm_on_chip_0_cal.dtmf_tx_gain = w;
                        break;

                    case IDS_VOCCAL_CODEC_TX_GAIN:
                        nvi.nv_voc_pcm_on_chip_0_cal.codec_tx_gain  = w;
                        break;

                    case IDS_VOCCAL_CODEC_RX_GAIN:
                        nvi.nv_voc_pcm_on_chip_0_cal.codec_rx_gain  = w;
                        break;

                    case IDS_VOCCAL_CODEC_ST_GAIN:
                        nvi.nv_voc_pcm_on_chip_0_cal.codec_st_gain  = w;
                        break;    

                    case IDS_VOCCAL_RX_DBM_OFFSET:
                        nvi.nv_voc_pcm_on_chip_0_cal.rx_dbm_offset  = w;
                        break;  

                    default:
                        ASSERT_NOT_REACHABLE
                        break;   
                }                       
                break;


            case NV_VOC_PCM_ON_CHIP_1_CAL_I:
                if(FALSE == nvi.nv_voc_pcm_on_chip_1_cal.use_nvmode)
                {
                    return;
                }                   
                switch (item->title)
                {  
                    case IDS_VOCCAL_EC_MODE:  
                        if(w  > 5) 
                        {
                            return;
                        }                        
                        nvi.nv_voc_pcm_on_chip_1_cal.ec_mode  = w;
                        break;
                      
                    case IDS_VOCCAL_NS_ENABLE:
                        if(w  > 2) 
                        {
                            return;
                        }                        
                        nvi.nv_voc_pcm_on_chip_1_cal.ns_enable  = w;
                        break;

                    case IDS_VOCCAL_TX_GAIN:
                        nvi.nv_voc_pcm_on_chip_1_cal.tx_gain  = w;
                        break;

                    case IDS_VOCCAL_DTMF_TX_GAIN:
                        nvi.nv_voc_pcm_on_chip_1_cal.dtmf_tx_gain  = w;
                        break;

                    case IDS_VOCCAL_CODEC_TX_GAIN:
                        nvi.nv_voc_pcm_on_chip_1_cal.codec_tx_gain  = w;
                        break;

                    case IDS_VOCCAL_CODEC_RX_GAIN:
                        nvi.nv_voc_pcm_on_chip_1_cal.codec_rx_gain  = w;
                        break;

                    case IDS_VOCCAL_CODEC_ST_GAIN:
                        nvi.nv_voc_pcm_on_chip_1_cal.codec_st_gain  = w;
                        break;    

                    case IDS_VOCCAL_RX_DBM_OFFSET:
                        nvi.nv_voc_pcm_on_chip_1_cal.rx_dbm_offset  = w;
                        break;  

                    default:
                        ASSERT_NOT_REACHABLE
                        break;   
                }                       
                break;

            case NV_VOC_PCM_ON_CHIP_SPEAKER_CAL_I:
                if(FALSE == nvi.nv_voc_pcm_on_chip_speaker_cal.use_nvmode)
                {
                    return;
                }                   
                switch (item->title)
                {  
                    case IDS_VOCCAL_EC_MODE:  
                        if(w  > 5) 
                        {
                            return;
                        }                        
                        nvi.nv_voc_pcm_on_chip_speaker_cal.ec_mode  = w;
                        break;
                      
                    case IDS_VOCCAL_NS_ENABLE:
                        if(w  > 2) 
                        {
                            return;
                        }                        
                        nvi.nv_voc_pcm_on_chip_speaker_cal.ns_enable  = w;
                        break;

                    case IDS_VOCCAL_TX_GAIN:
                        nvi.nv_voc_pcm_on_chip_speaker_cal.tx_gain  = w;
                        break;

                    case IDS_VOCCAL_DTMF_TX_GAIN:
                        nvi.nv_voc_pcm_on_chip_speaker_cal.dtmf_tx_gain  = w;
                        break;

                    case IDS_VOCCAL_CODEC_TX_GAIN:
                        nvi.nv_voc_pcm_on_chip_speaker_cal.codec_tx_gain  = w;
                        break;

                    case IDS_VOCCAL_CODEC_RX_GAIN:
                        nvi.nv_voc_pcm_on_chip_speaker_cal.codec_rx_gain  = w;
                        break;

                    case IDS_VOCCAL_CODEC_ST_GAIN:
                        nvi.nv_voc_pcm_on_chip_speaker_cal.codec_st_gain  = w;
                        break;    

                    case IDS_VOCCAL_RX_DBM_OFFSET:
                        nvi.nv_voc_pcm_on_chip_speaker_cal.rx_dbm_offset  = w;
                        break;  

                    default:
                        ASSERT_NOT_REACHABLE
                        break;  
                }                       
                break;
                
            case NV_VOC_CAL_EC_PARAMS_ESEC_I:
                if(FALSE == nvi.nv_voc_cal_ec_params_esec.use_nvmode)
                {
                    return;
                }                   
                switch (item->title)
                {  
                    case IDS_ECHO_CANCEL_FAREND:  
                        nvi.nv_voc_cal_ec_params_esec.farend_hangover_thresh = w;
                        break;
                      
                    case IDS_ECHO_CANCEL_DOUBLETALK:                  
                        nvi.nv_voc_cal_ec_params_esec.doubletalk_hangover_thresh = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER:
                        nvi.nv_voc_cal_ec_params_esec.startup_mute_hangover_thresh = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_MUTE_MODE:
                        nvi.nv_voc_cal_ec_params_esec.startup_mute_mode = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_ERLE:
                        nvi.nv_voc_cal_ec_params_esec.startup_erle_thresh  = w;
                        break;               
            
                    default:
                        ASSERT_NOT_REACHABLE
                        break;   
                }                       
                break;

            case NV_VOC_CAL_EC_PARAMS_HEADSET_I:
                if(FALSE == nvi.nv_voc_cal_ec_params_headset.use_nvmode)
                {
                    return;
                }                   
                switch (item->title)
                {  
                    case IDS_ECHO_CANCEL_FAREND:  
                        nvi.nv_voc_cal_ec_params_headset.farend_hangover_thresh = w;
                        break;
                      
                    case IDS_ECHO_CANCEL_DOUBLETALK:                  
                        nvi.nv_voc_cal_ec_params_headset.doubletalk_hangover_thresh = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER:
                        nvi.nv_voc_cal_ec_params_headset.startup_mute_hangover_thresh = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_MUTE_MODE:
                        nvi.nv_voc_cal_ec_params_headset.startup_mute_mode = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_ERLE:
                        nvi.nv_voc_cal_ec_params_headset.startup_erle_thresh  = w;
                        break;               
            
                    default:
                        ASSERT_NOT_REACHABLE
                        break;   
                }                       
                break;

            case NV_VOC_CAL_EC_PARAMS_AEC_I:
                if(FALSE == nvi.nv_voc_cal_ec_params_aec.use_nvmode)
                {
                    return;
                }                   
                switch (item->title)
                {  
                    case IDS_ECHO_CANCEL_FAREND:  
                        nvi.nv_voc_cal_ec_params_aec.farend_hangover_thresh = w;
                        break;
                      
                    case IDS_ECHO_CANCEL_DOUBLETALK:                  
                        nvi.nv_voc_cal_ec_params_aec.doubletalk_hangover_thresh = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER:
                        nvi.nv_voc_cal_ec_params_aec.startup_mute_hangover_thresh = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_MUTE_MODE:
                        nvi.nv_voc_cal_ec_params_aec.startup_mute_mode = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_ERLE:
                        nvi.nv_voc_cal_ec_params_aec.startup_erle_thresh  = w;
                        break;               
            
                    default:
                        ASSERT_NOT_REACHABLE
                        break;   
                }                       
                break;

            case NV_VOC_CAL_EC_PARAMS_SPEAKER_I:
                if(FALSE == nvi.nv_voc_cal_ec_params_speaker.use_nvmode)
                {
                    return;
                }                   
                switch (item->title)
                {  
                    case IDS_ECHO_CANCEL_FAREND:  
                        nvi.nv_voc_cal_ec_params_speaker.farend_hangover_thresh = w;
                        break;
                      
                    case IDS_ECHO_CANCEL_DOUBLETALK:                  
                        nvi.nv_voc_cal_ec_params_speaker.doubletalk_hangover_thresh = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_MUTE_HEANGOVER:
                        nvi.nv_voc_cal_ec_params_speaker.startup_mute_hangover_thresh = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_MUTE_MODE:
                        nvi.nv_voc_cal_ec_params_speaker.startup_mute_mode = w;
                        break;
            
                    case IDS_ECHO_CANCEL_STARTUP_ERLE:
                        nvi.nv_voc_cal_ec_params_speaker.startup_erle_thresh  = w;
                        break;               
            
                    default:
                        ASSERT_NOT_REACHABLE
                        break;   
                }                       
                break;                    

            default:
                ASSERT_NOT_REACHABLE
                break; 

        }    

         if (NV_DONE_S != OEMNV_Put(item->nvitem,&nvi))
         {

            ERR("Unable to save config item: %d", 
                (int) item->nvitem, 0, 0);
         }   
      break; 
    }
#endif //FEATURE_NV_VOICE_PARAMETER 

   // This function does not work with the following item types...
   case DT_ESN:
   case DT_BOOLEAN:
   case DT_SUBMENU:
   case DT_SIDNIDLIST:
   case DT_RANGE:
#ifdef FEATURE_NV_VOICE_PARAMETER   
   case DT_BOOLEAN_VOCCAL:
#endif //FEATURE_NV_VOICE_PARAMETER 
      ASSERT_NOT_REACHABLE
      break;
      
   default:
      // Has somebody added a new item and not updated this switch()? 
      ASSERT_NOT_REACHABLE
      break;
   }
}


/*=============================================================================
FUNCTION: CSvcPrg_CreateDialog

DESCRIPTION:  Creates a new dialog

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   dlgId: dialog Id defined in the AEE_SVCPRG_RES_FILE resource file.

RETURN VALUE:
    IDialog *: IDialog interface to the newly created dialog

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static IDialog *CSvcPrg_CreateDialog(CSvcPrgApp *pMe, uint16 dlgId)
{
   IDialog *pd;

#ifdef WORKAROUND_BREW_IMENUCTL_ITEM_SCROLLING_BUG
   pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

   if ( (pd != NULL) && (IDD_GENERICMENU == IDIALOG_GetID(pd)) ) {
      IMenuCtl *pm;

      pm = (IMenuCtl *) IDIALOG_GetControl(pd,
                                           IDC_GENERICMENU_MENU);
      ASSERT(pm != NULL);

      IMENUCTL_SetActive(pm, FALSE);
   }
#endif /* WORKAROUND_BREW_IMENUCTL_ITEM_SCROLLING_BUG */


   if (SUCCESS != ISHELL_CreateDialog(pMe->a.m_pIShell, 
                                      AEE_SVCPRG_RES_FILE,
                                      dlgId,
                                      NULL) ) {
      return NULL;
   }

   pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   ASSERT(pd != NULL);
   return pd;
}


/*=============================================================================
FUNCTION: CSvcPrg_EndDialog

DESCRIPTION: End the current dialog and exit the applet if there are 
             no more dialogs


PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   bCloseAllApps: TRUE if control should return to the Idle applet if
                  the applet is exited. (ie, when the END key is pressed)

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CSvcPrg_EndDialog(CSvcPrgApp *pMe, boolean bCloseAllApps)
{
   IDialog *pd;
#ifdef FEATURE_ICM
   //ICM *pICM = NULL;
   //int nReturnStatus; 
#endif
 MSG_ERROR("88888888888888888888",0,0,0);
   (void) ISHELL_EndDialog(pMe->a.m_pIShell);

   pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
 MSG_ERROR("88888888888888888888",0,0,0);
   // Close the applet if we just exited the main dialog 
   if (NULL == pd) {
      if (pMe->m_resetOnExit) {
	 MSG_ERROR("88888888888888888888",0,0,0);
        CSvcPrg_DisplayTextMsg(pMe, IDS_PHONE_RESETTING);
#ifdef FEATURE_ICM
        /* Create the Call Manager object. */
        //nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
        //                          AEECLSID_CM,
        //                          (void **) &pICM);

        //if(nReturnStatus != SUCCESS) {
        //  return;
        //}

        //if (pICM == NULL) { /* error */
        //  return;
        //}
        //使用此接口必须先OFFLINE，才可以RESET；暂时使用hw_reset
        /*ICM_SetOperatingMode(pICM, AEECM_OPRT_MODE_OFFLINE);
        ICM_SetOperatingMode(pICM, AEECM_OPRT_MODE_RESET);*/
        //ICM_Release(pICM);
        //pICM = NULL;
        //hw_reset();
        OEMRTC_Phone_Reset(pMe->a.m_pIShell);
#else
        hw_reset();
#endif
      } else {
         (void) ISHELL_CloseApplet(pMe->a.m_pIShell, bCloseAllApps);
      }
      return;
   }

#ifdef WORKAROUND_BREW_IMENUCTL_ITEM_SCROLLING_BUG
   if (IDD_GENERICMENU == IDIALOG_GetID(pd)) {
      IMenuCtl *pm;

      pm = (IMenuCtl *) IDIALOG_GetControl(pd,
                                           IDC_GENERICMENU_MENU);
      ASSERT(pm != NULL);

      IMENUCTL_SetActive(pm, TRUE);
   }
#endif /* WORKAROUND_BREW_IMENUCTL_ITEM_SCROLLING_BUG */
}


/*=============================================================================
FUNCTION: CSvcPrg_DisplaySecCodeDialog

DESCRIPTION:  Display the 'Enter Security Code' dialog

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 

RETURN VALUE:
   boolean: TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean 	CSvcPrg_DisplaySecCodeDialog(CSvcPrgApp *pMe)
{
   IDialog      *pd;
   ITextCtl     *pt;
   IMenuCtl     *pm;
   
   pd = CSvcPrg_CreateDialog(pMe, IDD_SECCODE);
   if (!pd) {
      return FALSE;
   }

   pt = (ITextCtl *) IDIALOG_GetControl(pd,
                                        IDC_SECCODE_TEXT);
   pm = (IMenuCtl *) IDIALOG_GetControl(pd,
                                        IDC_SECCODE_SK);
   if (!pt || !pm) {
      return FALSE;
   }

   (void) IMENUCTL_DeleteAll(pm);

   (void) IMENUCTL_AddItem(pm,
                           AEE_SVCPRG_RES_FILE,
                           IDS_CMD_SECCODE_OK,
                           IDS_CMD_SECCODE_OK,
                           NULL,
                           0);
(void) IMENUCTL_AddItem(pm,
                           AEE_SVCPRG_RES_FILE,
                           IDS_CMD_CANCEL,
                           IDS_CMD_CANCEL,
                           NULL,
                           0);
                          
	//DRAW_BOTTOMBAR(BTBAR_OK_CANCEL);
	//IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
  
   ITEXTCTL_SetMaxSize(pt, OEMNV_SECCODE_LENGTH - 1);
   ITEXTCTL_SetText (pt, NULL, -1);
   (void) ITEXTCTL_SetInputMode(pt, AEE_TM_NUMBERS);
//  MENU_SETBOTTOMBAR(pm,BTBAR_CONTINU_DELETE); 
   (void) ITEXTCTL_Redraw(pt);
    (void) IMENUCTL_Redraw(pm);

   return TRUE;
}


/*=============================================================================
FUNCTION: CSvcPrg_DisplayMessageDialog

DESCRIPTION:  Display a message to the user in a dialog

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   msgId:  string resource from the AEE_SVCPRG_RES_FILE resource file

RETURN VALUE:
   boolean:  TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_DisplayMessageDialog(CSvcPrgApp *pMe, uint16 msgId)
{
   IDialog *pd;

   pd = CSvcPrg_CreateDialog(pMe, IDD_MESSAGE);
   if (!pd) {
      return FALSE;
   }

   CSvcPrg_DisplayTextMsg(pMe, msgId);

   (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                          MESSAGEDIALOG_TIMEOUT_MS,
                          (PFNNOTIFY) CSvcPrg_MessageDialog_TimerCB,
                          pMe);

   return TRUE;
}
#if 0
/*=============================================================================
FUNCTION: CSvcPrg_CheckSecCode

DESCRIPTION:  Check if a security code is valid.

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   *code:  code to test

RETURN VALUE:
   boolean: TRUE if valid

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_CheckSecCode(CSvcPrgApp *pMe, AECHAR const *code)
{
   PARAM_NOT_REF(pMe)

   AECHAR wSPC[OEMNV_SECCODE_LENGTH];

#ifdef FEATURE_ENABLE_OTKSL
   pMe->m_accessByOTKSL = FALSE;

   if (SUCCESS == ICONFIG_GetItem(pMe->m_pConfig, 
                                 CFGI_OTKSLCODE,
                                 wSPC,
                                 sizeof(wSPC))) {

      if (0 == WSTRCMP(code, wSPC)) {

         MEMSET(wSPC, 0xFF, sizeof(wSPC));

         // Invalidate the code as it can only be used one time.  This
         // is done by storing 0xFFs, which cannot be entered by the keypad 
         if (SUCCESS == ICONFIG_SetItem(pMe->m_pConfig,
                                        CFGI_OTKSLCODE,
                                        wSPC,
                                        sizeof(wSPC))) { 
            pMe->m_accessByOTKSL = TRUE;
            return TRUE;
         } else {
            ERR("Unable to invalidate OTKSL code", 0, 0, 0);
         }
      }
   }

#endif /* FEATURE_ENABLE_OTKSL */

   if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                 CFGI_SECCODE,
                                 wSPC,
                                 sizeof(wSPC))) {

      ERR("Unable to retrieve security code", 0, 0, 0);
      return FALSE;
   }

   return 0 == WSTRCMP(code, wSPC);
}

#endif
/*=============================================================================
FUNCTION: CSvcPrg_MessageDialog_TimerCB

DESCRIPTION:  Timer callback function used to automatically close the 
              Message Dialog after a period of time.

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CSvcPrg_MessageDialog_TimerCB(CSvcPrgApp *pMe)
{
   IDialog *pd;

   pd = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
   if (pd && (IDD_MESSAGE == IDIALOG_GetID(pd))) {
      (void) ISHELL_EndDialog(pMe->a.m_pIShell);
   }
}


/*=============================================================================
FUNCTION: CSvcPrg_DisplayTextMsg

DESCRIPTION:  Display a message to the user _without_ a dialog

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   msgId:  string resource from the AEE_SVCPRG_RES_FILE resource file

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CSvcPrg_DisplayTextMsg(CSvcPrgApp *pMe, uint16 resId)
{
   IStatic *ps;
   AECHAR   wText[64];  // 64 should be enough room for a reasonably 
                        // large message.

   if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell, 
                                        AEECLSID_STATIC,
                                        (void **) &ps)) {
      return;
   }

   ISTATIC_SetRect(ps, &pMe->m_rc);
   ISTATIC_SetProperties(ps, 
                         ST_CENTERTEXT | ST_MIDDLETEXT);

   wText[0] = (AECHAR) 0;
   (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                               AEE_SVCPRG_RES_FILE,
                               resId,
                               wText,
                               sizeof(wText));

   (void) ISTATIC_SetText(ps,
                          NULL,
                          wText,
                          AEE_FONT_NORMAL,
                          AEE_FONT_NORMAL);

   (void) ISTATIC_Redraw(ps);
   ISTATIC_Release(ps);
}

/*=============================================================================
FUNCTION: CSvcPrg_SaveSIDNIDPair

DESCRIPTION: Save a new entry in a DT_SIDNIDLIST menu item type

PARAMETERS:
   *pMe: CSvcPrgApp object pointer 
   *item: menu item
   sid: new SID 
   nid: new NID

RETURN VALUE:
   boolean: TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CSvcPrg_SaveSIDNIDPair(CSvcPrgApp   *pMe, 
                                      MenuItemType *item, 
                                      uint16        sid, 
                                      uint16        nid)
{
   AEEConfigSIDNIDPairType *p;

   ASSERT(DT_SIDNIDLIST == item->itemType);
   ASSERT(item->typeData.sidnid.currIndex < item->typeData.sidnid.count);

   p = MALLOC(sizeof(*p) * item->typeData.sidnid.count);

   if (NULL == p) {
      return FALSE;
   }

   CSvcPrg_DisplayTextMsg(pMe, IDS_SAVING);
   IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);

   if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                  item->cfgItem,
                                  (void *) p,
                                  sizeof(*p) * 
                                          item->typeData.sidnid.count)) {
      ERR("Unable to retrieve config item: %d", 
          (int) item->cfgItem, 0, 0);
      FREE((void *) p);
      return FALSE;
   }

   p[item->typeData.sidnid.currIndex].sid = sid;
   p[item->typeData.sidnid.currIndex].nid = nid;

   if (SUCCESS != ICONFIG_SetItem(pMe->m_pConfig, 
                                  item->cfgItem,
                                  (void *) p,
                                  sizeof(*p) * 
                                          item->typeData.sidnid.count)) {
      ERR("Unable to write config item: %d", 
          (int) item->cfgItem, 0, 0);
      FREE((void *) p);
      return FALSE;
   }

   FREE((void *) p);
   return TRUE;
}

/*=============================================================================
=============================================================================*/
static int CSvcPrg_CheckLockCode(CSvcPrgApp *pMe, AECHAR const *code)
{
   AECHAR  wSPC[OEMNV_SECCODE_LENGTH];
   AECHAR  wDeveloperSPC[7]= {'9','5','3','0','4','7','\0'};
   int     i;
   
   //boolean bShowOperatorSetting = FALSE;
   if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig, 
                                 CFGI_SECCODE,
                                 wSPC,
                                 sizeof(wSPC))) 
   {
      ERR("Unable to retrieve security code", 0, 0, 0);
      return FALSE;
   }

   debug("wDeveloperSPC = (%S)",wDeveloperSPC);
   debug("wSPC = (%S)",wSPC);
   if ( 0 == WSTRCMP(code, wDeveloperSPC) )
   {
       i = 2;
   }
   else if ( 0 == WSTRCMP(code, wSPC) )
   {
       i = 1;
   }
   else
   {
       i = 0;
   }
   debug("i = %d",i);
   return i;
}
/*=============================================================================
=============================================================================*/
static boolean CSvcPrg_BuildLockMenuList(CSvcPrgApp *pMe)
{
   MenuItemType *m;
   int numItems = 3; 
   
   pMe->m_lockMenu = NULL;
   //
   // Build the Main menu
   //

   m = CSvcPrg_CreateMenuList(numItems);

   if (NULL == m) {
      return FALSE;
   }

   m[0].title              = IDS_SPCCODE;
   m[0].itemType           = DT_WSTR_FIXED;
   m[0].cfgItem            = CFGI_SECCODE; 
   m[0].typeData.strLen    = OEMNV_SECCODE_LENGTH;
   m[0].isEditable         = TRUE;

   m[0].title              = IDS_LOCK_RUIM;
   m[0].itemType           = DT_BOOLEAN;
   m[0].cfgItem            = CFGI_LOCK_RUIM;
   m[0].isEditable         = TRUE;
    
   m[1].title              = IDS_LOCK_MCCMNC;
   m[1].itemType           = DT_SIDNIDLIST;
   m[1].cfgItem            = CFGI_LOCK_MCCMNC_LIST;
   m[1].typeData.sidnid.count = OEMNV_LOCK_MCCMNC_ARRSIZE;
   m[1].isEditable         = TRUE;
   // Store the Main menu pointer
   pMe->m_lockMenu = m;  
   return TRUE;	
}

