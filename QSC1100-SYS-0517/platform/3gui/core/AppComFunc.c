/*============================================================================^

FILE: AppComFunc.c

SERVICES: Provides common functions used in several of the BREW applets.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
void ShowBusyIcon(IShell *ps, IDisplay *pd, AEERect *rc);
void SetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm);
void SetDefaultMenuLook(IShell *ps, IMenuCtl *pm);
void SetDefaultIconLook(IShell *ps, IMenuCtl *pm);
void InitMenuIcons(IMenuCtl *pm);
void SetMenuIcon(IMenuCtl *pm, uint16 listId, boolean bSel);
void DoNumberLookup(IAddrBook *pa, AECHAR *number, AECHAR *name, uint32 size);
void DoSpeedDialLookup(IAddrBook *pa, AECHAR *speedialindex, AECHAR *number, uint32 size);
AECHAR *FormatPhoneNumberForDisplay(AECHAR const *number);
void GetTimeString(IConfig *cfg, ITimeCtl *tctl, IShell *ish,
                          uint32 timeVal, AECHAR *timeStr);


INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
                   All Rights Reserved.
                   QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================

  $Header: //depot/asic/msmshared/apps/Core/AppComFunc.c#73 $
$DateTime: 2009/04/22 00:18:30 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
04/17/09   Gopi      Added back IsCallActive, ClearBlockUITask, BlockUITask, 
                      UnblockUITask
03/11/09   Gopi      Moved IsCallActive, ClearBlockUITask, BlockUITask, 
                      UnblockUITask to uiutils.c for Mango UI Integration
03/26/08     cvs     Remove deprecated functions
02/23/07     jas     Refactoring for HDK builds
02/12/07     mjv     Featurized parts of file with !FEATURE_UIONE_HDK
01/23/07     ypmw    Move WLAN application from Core App to AUX Settings
09/19/06     jas     Moving a local AppComFunc structure definition to the
                     .c file and fixing a compile error.
09/19/06     jas     Adding band value to AEECMWLANPref structure.
07/02/06     joe     Fixed bug in set check box item.
06/15/06     cvs     Fix Klocwork errors
05/24/06     JD      Added SelectFont function to increase font on VGA displays.
04/21/06     jas     Featurizing font selection for BREW versions lacking
                     the standard font support.
07/28/05     AT      Added functions to block the UI task when waiting for
                     an OEM layer command.
07/27/05     AT      Removing compiler warning for unused variable.
05/11/05     ak      Added a different Decoding Scheme
05/09/05     ak      Using the packed string Decoding
04/15/05     jas     Correcting resource file use
03/10/05     ak      Added CPHS EONS Support
01/01/05     jas     Added QVGA resource file
01/25/05     sun     Allow other Apps to Change Backlight setting via CoreApp
12/13/04     ssc     Changed include file name case to match the Linux names 
                     of the files. Mismatches in the case break AMSS builds 
                     on Linux servers.
10/08/04     SUN     Added function to Display Data from other Apps
09/20/04     PA      Merged changes from BTG to use canonical path for resource
08/20/04     JG      IADDRBOOK_EnumRecInitEx() is defined in BREW 3.1. 
                     Rename the QCT IADDRBOOK_EnumRecInitEx() to 
                     IADDRBOOKEXT_EnumRecInitEx
07/28/04     PMD     Changed SetIdleMenuText() to post event when pszText = NULL
07/18/04     sun     Added broadcast to idle menu while the app is suspended
07/08/04     sun     Incorrect call state
06/28/04     jas     Added support for dual-display targets
05/13/04     sun     Modified Directory for Brew 3.0
02/23/04     sun     Fixed parameters being passed to ICM_GetActiveCallIDs
01/25/04     PMD     Added function OriginateMessage()
01/08/04     sun     "Unavailable" was not being displayed.
12/09/03     ram     Removed lint errors.
11/25/03     ram     Add broadcast text to idle menu.
11/25/03     BPW     Modified AnswerVoiceCall to reject the incoming call as well
11/21/03     ak      Rolled back IADDRBOOK_EnumRecInit change. IADDRBOOK_EnumRecInitEx() 
                     is now defined in OEMAddrbook.h of BREW 3.0
11/20/03     kar     Fixed merge error
11/19/03     ak      BREW 3.0 changes
11/06/03     BPW     Added AnswerVoiceCall function
10/23/03     PA      Changed ICM CLSID to AEECLSID_CM
10/21/03     BPW     Moved malloc in OriginateVoiceCall to Dialer App
10/16/03     kar     Call dialer function to originate voice call
10/15/03     BPW     Changed DoNumberLookup to be exact
09/30/03     tml     Reflected changes in ICM API
09/24/03     kar     Added new function to return if in call
09/12/03     kar     Fixed compile warning
09/10/03     BPW     Added error checking to the addr book lookup funtions
08/12/03     ram     Added Tab controller functionality for multi-control dialogs
08/08/03     sun     Added functions to initialize and set check boxes
05/30/03     PA      Display "Unavailable" when phone number is absent
05/23/03     kar     Originate voice call now goes automatically to SK UI
05/12/03     kar     Added originatevoicecall
05/08/03     PA      Added DoVoiceTagLookup, GetVoiceTagByNumber and
                     GetVoiceTagList
04/25/03     kar     Fixed compile warnings
04/24/03     kar     Added FormatPhoneNumberForDisplay and GetTimeString
04/17/03     PA/AT   Fixed DoSpeedDialLookup to use ADDRFIELD_INDEX
02/10/03     kar     Added AVKType2ASCII
01/28/03     kar     Redraw menu control after icon change
01/27/03     kar     Added functions to initialize and set menu icons
01/22/03     kar     Tweaked softkey properties
01/20/03     kar     Fixed compiler warnings
01/10/03     kar     Fixed softkey properties
01/08/03     kar     Removed unused functions
01/07/03     kar     Removed unwanted functions
01/07/03     kar     Initial revision
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#include "assert.h"
#endif
#include "err.h"
#include "OEMFeatures.h"
#include "AEEShell.h"
#include "AEEMenu.h"
#include "AEEText.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEControls_res.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AppComFunc.h"
#include "AEEAddrBookExt.h"
#include "AEEConfig.h"
#include "OEMDisp.h"
#include "BREWVersion.h"

#include "CoreApp.BID"
#include "CoreUserEvents.h"

#ifdef FEATURE_UIONE_HDK
#include "AUXMenu.h"
#else
#include "CoreApp.h"
#include "CoreApp_res.h"
#include "CoreApp_qvga_res.h"
#endif

#ifdef FEATURE_APP_DIALER
#include "DialerApp.h"
#endif /* FEATURE_APP_DIALER */

#include "task.h"
#include "ui.h"
#include "ui_base.h"

#ifdef FEATURE_WMS_APP
#include "WmsApp.h"
#endif /* FEATURE_WMS_APP */

#if MIN_BREW_VERSIONEx(3,1,4)
#include "AEEFontsStandard.BID"
#endif

#ifdef FEATURE_MANGO_BREW
#include "AEETelephone.h"
#endif

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#ifndef FEATURE_MANGO_UI
#define CLOCKCTL_MAX_TIMESTR_LEN 20


static const uint8 AppCom_GSMDefault_To_Ascii_Table[] =
{
 '@', '#', '$', 'Y', 'e', 'e', 'u', 'i', 'o', 'C',0x0A, 'O', 'o',0x0D, 'A', 'a',
 'D', ' ', '?', '?', 'L', 'O', 'P', '?', 'S', 'T', '?', ' ', 'A', 'a', 's', 'E',
 ' ', '!',0x22, '#', '?', '%', '&',0x27, '(', ')', '*', '+', ',', '-', '.', '/',
 '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
 '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'A', 'O', 'N', 'U', 'S',
 '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'a', 'o', 'n', 'u', 'a'
};

/*===========================================================================

 Tab Controller functions, forward declarations

=========================================================================== */
static boolean TabController_IsTabEvent(DlgTabCtl* pDlgTabCtl, uint16 keyCode);
static void TabController_SetAllInactive(DlgTabCtl *pDlgTabCtl);
static void TabController_End(DlgTabCtl** pDlgTabCtl);
static void TabController_ProcessKeyPress(DlgTabCtl *pDlgTabCtl, uint16 wParam);
static boolean TabController_ProcessKeyRelease(DlgTabCtl *pDlgTabCtl);
static void TabController_SetMenuSel(IMenuCtl *pIMenuCtl, int dir);
/*===========================================================================

 Extern function declarations.

=========================================================================== */
#ifndef FEATURE_UIONE_HDK
extern boolean CDialerApp_AnswerVoiceCall(IShell *pIShell, boolean bAnswer);
extern boolean CDialerApp_StartApp_OriginateVoiceCall(IShell *pIShell, const char *pszDialString);
#endif /* !FEATURE_UIONE_HDK */
extern byte    b_unpackb ( byte buf[ ], word pos, word len );
//lint -save -e740
/*===========================================================================

FUNCTION ShowBusyIcon

DESCRIPTION

PROTOTYPE:


PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
void ShowBusyIcon(IShell *ps, IDisplay *pd, AEERect *rc, boolean bClear)
{
   IImage       *busy;
   AEEImageInfo  ii;

   busy = ISHELL_LoadResImage(ps,
                              AEECONTROLS_RES_FILE,
                              AEE_IDB_BUSY);

   if (bClear) {
     /*lint -e620*/
      IDISPLAY_ClearScreen(pd);
     /*lint +e620*/
   }

   if (busy != NULL) {
      IIMAGE_GetInfo(busy, &ii);
      IIMAGE_SetParm(busy, IPARM_ROP, (int) AEE_RO_MASK, 0);
      IIMAGE_Draw(busy,
                  rc->x + (rc->dx - ii.cx) / 2,
                  rc->y + (rc->dy - ii.cy) / 2);
      IIMAGE_Release(busy);
   }

   IDISPLAY_UpdateEx(pd, FALSE);
}

/*===========================================================================

FUNCTION SetDefaultSoftkeyLook

DESCRIPTION

PROTOTYPE:


PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
void SetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm)
{
  AEEItemStyle sstyle, ustyle;
  AEEMenuColors col;

  ISHELL_GetItemStyle( ps, AEE_IT_MENU, &ustyle, &sstyle);


  /* selection style */
  sstyle.ft = AEE_FT_RAISED;
  sstyle.roImage = AEE_RO_TRANSPARENT;
  ustyle.ft = AEE_FT_NONE;
  ustyle.roImage = AEE_RO_TRANSPARENT;
  IMENUCTL_SetStyle(pm, &ustyle, &sstyle);

  /* colors */
  col.cSelText= RGB_BLACK;
  col.wMask = MC_BACK | MC_SEL_BACK| MC_SEL_TEXT;
  col.cBack = MAKE_RGB(255, 255, 204);

  col.cSelBack = MAKE_RGB(153, 204, 204);

  IMENUCTL_SetColors(pm, &col);
}

/*===========================================================================

FUNCTION SetDefaultMenuLook

DESCRIPTION

PROTOTYPE:


PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
void SetDefaultMenuLook( IShell *ps, IMenuCtl *pm)
{
  AEEItemStyle sstyle;
  AEEItemStyle ustyle;
  AEEMenuColors col;
  //uint32 dwProperties = 0;

  ISHELL_GetItemStyle( ps, AEE_IT_MENU, &ustyle, &sstyle );

  /* selection style */
  sstyle.ft = AEE_FT_RAISED;
  sstyle.xOffset = 0;
  sstyle.yOffset = 0;
  sstyle.roImage = AEE_RO_TRANSPARENT;
  ustyle.ft = AEE_FT_NONE;
  ustyle.xOffset = 2;
  ustyle.yOffset = 0;
  ustyle.roImage = AEE_RO_TRANSPARENT;
  IMENUCTL_SetStyle(pm, &ustyle, &sstyle);

  /* colors */
  col.cSelText= RGB_BLACK;
  col.wMask = MC_BACK | MC_SEL_BACK| MC_SEL_TEXT;
  col.cBack = MAKE_RGB(255, 255, 204);

  col.cSelBack = MAKE_RGB(153, 204, 204);

  //dwProperties |= IMENUCTL_GetProperties(pm);
  //IMENUCTL_SetProperties( pm, MP_UNDERLINE_TITLE );

  IMENUCTL_SetColors(pm, &col);

}

/*===========================================================================

FUNCTION SetDefaultIconLook

DESCRIPTION

PROTOTYPE:


PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
void SetDefaultIconLook( IShell *ps, IMenuCtl *pm)
{
  AEEItemStyle normal, selected;

  ISHELL_GetItemStyle( ps, AEE_IT_MENU, &normal, &selected );

  normal.ft = AEE_FT_3D_EMPTY;
  normal.xOffset = normal.yOffset = 3;
  normal.roImage = AEE_RO_MASK;

  selected = normal;
  selected.ft = AEE_FT_RAISED;

  IMENUCTL_SetStyle( pm, &normal, &selected );
  IMENUCTL_SetProperties( pm, MP_UNDERLINE_TITLE );
}

/*=============================================================================
FUNCTION:  GetResourcePath

DESCRIPTION: Prepares the appropriate path using canonical form depending on
 the version of BREW

PARAMETERS:
  None

RETURN VALUE:
   Valid char * if sucessful. 
   NULL: If unsucessful

COMMENTS:
  The pointer returned needs to be FREE'ed by the caller.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
char * GetResourcePath()
{
#ifdef FEATURE_UIONE_HDK
  // CoreApp is not available.  Reuse the AUXMenu resource file for now.
  return AUXGetResourcePath(); 
#else
  char   *     pszResFile;
  AEEDeviceInfo   di;
  char        szCoreClsID[20];
  IShell  *    pIShell; 

  di.wStructSize = sizeof(AEEDeviceInfo);
  pIShell = (IShell *)  AEE_GetShell();
  if(!pIShell)
  {
      MSG_ERROR("Could not get IShell", 0, 0, 0);
      return NULL;
  }

  ISHELL_GetDeviceInfo(pIShell, &di);
  pszResFile = (char*) MALLOC(di.wMaxPath);
  if(!pszResFile)
  {
      MSG_ERROR("Malloc failed", 0, 0, 0);
      return NULL;
  }


  #if MIN_BREW_VERSION(3,0)
  // Prepare the path for the resource file to be fs:/~COREAPP_CLASSID/coreapp.bar
  // This is needed because the context of the application calling this method might
  // not be the same as Core App.
  SNPRINTF(szCoreClsID, sizeof(szCoreClsID), "%d",AEECLSID_CORE_APP);
  STRLCPY(pszResFile, "fs:/~", di.wMaxPath);
  STRLCAT(pszResFile, szCoreClsID, di.wMaxPath);
  STRLCAT(pszResFile, "/", di.wMaxPath);
  STRLCAT(pszResFile, gpszResourceFile, di.wMaxPath);
  #else
  STRLCPY(pszResFile, gpszResourceFile, di.wMaxPath);
  #endif // MIN_BREW_VERSION(3,0) 

  return pszResFile;
#endif /* !FEATURE_UIONE_HDK */
}

/*=============================================================================
FUNCTION:  InitMenuIcons

DESCRIPTION: Adds IDB_RADIO_UNFILLED images to every menu item in the
             specified menu.

PARAMETERS:
   *pm:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void InitMenuIcons(IMenuCtl *pm)
{
   CtlAddItem cai;
   int        i;
   char *    pszResFile;
   
   pszResFile = GetResourcePath();
   
   if(!pszResFile)
     return;

   (void) MEMSET(&cai, 0, sizeof(cai));
   cai.pszResImage = pszResFile;
   cai.wImage = IDB_RADIO_UNFILLED;

   for (i = (IMENUCTL_GetItemCount(pm)-1); i >= 0; i--) {
      (void) IMENUCTL_SetItem(pm,
                              IMENUCTL_GetItemID(pm, i),
                              MSIF_IMAGE,
                              &cai);
   }

   FREEIF(pszResFile);
   pszResFile = NULL;


   /* IMENUCTL_SetItem() does not recalculate the new maximum height of
    * the menu control's items when a menu item is changed.  This causes a
    * problem if an image is added to a menu item that is larger than the
    * current maximum height of the menu items.
    *
    * For example, say the current maximum menu item height is 10 pixels and
    * the display height is 40 pixels.  The menu control can show 4 menu items
    * at a time.  Now IMENUCTL_SetItem() is used to add a 20 pixel high image
    * to one of the menu items.  The menu control can now only display three
    * items on the display at one time.  However it will still try to display
    * four items.  If the fourth item in the menu is currently selected, the
    * menu selection will not be visible.
    */
   // The workaround for this bug is to call an IMenuCtl method that
   // will indirectly cause the menu control to recalculate the maximum height
   // of the menu items (as there is no IMenuCtl method to do this directly).
   // Invoking IMENUCTL_SetStyle() will NULL style parameters works nicely
   // because it has no effect besides performing the necessary recalculation.
   IMENUCTL_SetStyle(pm, NULL, NULL);
}


/*=============================================================================
FUNCTION:  SetMenuIcon

DESCRIPTION: Sets Radio Buttons on/off

PARAMETERS:
   *pm:
   listId:
   bSel:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SetMenuIcon(IMenuCtl   *pm,
                 uint16      listId,
                 boolean     bSel)
{
   CtlAddItem cai;
   char *    pszResFile;

   pszResFile = GetResourcePath();

   if(!pszResFile)
     return;

   (void) MEMSET(&cai, 0, sizeof(cai));
   cai.pszResImage = pszResFile;
   cai.wImage = bSel ? IDB_RADIO_FILLED : IDB_RADIO_UNFILLED;

   (void) IMENUCTL_SetItem(pm, listId, MSIF_IMAGE, &cai);
   FREEIF(pszResFile);
   pszResFile = NULL;
   
   if (bSel) {
     IMENUCTL_SetSel (pm, listId);
   }
   IMENUCTL_SetStyle(pm, NULL, NULL);
   IMENUCTL_Redraw (pm);
}
/*=============================================================================
FUNCTION:  InitCheckBox

DESCRIPTION: Adds IDB_CHECK_OFF images to every menu item in the
             specified menu.

PARAMETERS:
   *pm:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void InitCheckBox(IMenuCtl *pm)
{
   CtlAddItem cai;
   int        i;
   char *    pszResFile;

   pszResFile = GetResourcePath();

   if(!pszResFile)
      return;

   (void) MEMSET(&cai, 0, sizeof(cai));
   cai.pszResImage = pszResFile;
   cai.wImage = IDB_CHECK_OFF;

   for (i = (IMENUCTL_GetItemCount(pm) -1); i >= 0; i--) {
      (void) IMENUCTL_SetItem(pm,
                              IMENUCTL_GetItemID(pm, i),
                              MSIF_IMAGE,
                              &cai);
      cai.dwData = 0;
      (void) IMENUCTL_SetItem(pm,
                              IMENUCTL_GetItemID(pm, i),
                              MSIF_DATA,
                              &cai);
   }

   FREEIF(pszResFile);
   pszResFile = NULL;

   IMENUCTL_SetStyle(pm, NULL, NULL);
}

/*=============================================================================
FUNCTION:  SetCheckBoxItem

DESCRIPTION: Sets Check Box Items on/off

PARAMETERS:
   *pm:
   listId:
   bSel:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SetCheckBoxItem(IMenuCtl   *pm,
                 uint16      listId,
                 boolean     bSel)
{
   CtlAddItem cai;
   char *    pszResFile;

   pszResFile = GetResourcePath();

   if(!pszResFile)
     return;

   (void) MEMSET(&cai, 0, sizeof(cai));

   /* Use the same resource file as the rest of CoreApp */
   cai.pszResImage = pszResFile;
   cai.wImage = bSel ? IDB_CHECK_ON : IDB_CHECK_OFF;

   (void) IMENUCTL_SetItem(pm, listId, MSIF_IMAGE, &cai);

   cai.dwData = bSel ? 1:0;
   (void) IMENUCTL_SetItem(pm, listId, MSIF_DATA, &cai);

   IMENUCTL_SetStyle(pm, NULL, NULL);
   IMENUCTL_Redraw (pm);

   FREEIF(pszResFile);
   pszResFile = NULL;

}

/*=============================================================================
FUNCTION:  GetCheckBoxVal

DESCRIPTION: Sets Check Box Items on/off

PARAMETERS:
   *pm:
   listId:
   bSel:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean GetCheckBoxVal(IMenuCtl   *pm,
                      uint16      listId)
{
   CtlAddItem cai;

   IMENUCTL_GetItem(pm, listId, &cai);
   if(cai.dwData == 1)
   {
     return TRUE;
   }

   return FALSE;

}

/*=============================================================================
FUNCTION: AVKType2ASCII

DESCRIPTION:
   Converts a {AVK_0..AVK_9,AVK_STAR,AVK_POUND} to the
   ASCII equivalent

PARAMETERS:
   key: BREW keycode

RETURN VALUE:
   char: ASCII value

COMMENTS:
   Returns the NULL character if the input value is out of range

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
char AVKType2ASCII(AVKType key)
{
   if (AVK_STAR == key) {
      return '*';
   }

   if (AVK_POUND == key) {
      return '#';
   }

   if (key < AVK_0 || key > AVK_9) {
      return '\0';
   }

   return '0' + (char)(key - AVK_0); /*lint !e656*/
}

/*=============================================================================
FUNCTION: DoNumberLookup

DESCRIPTION:  Searches the Contacts database for a exact name match for the
              specified phone number.

PARAMETERS:
   *pa     [ni]:  IAddrBook interface pointer
   *number [in]:  Phone number to lookup
   *name: [out]:  Destination buffer for the name
   size:   [in]:  Size of name in bytes

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void DoNumberLookup(IAddrBook *pa,
                    AECHAR    *number,
                    AECHAR    *name,
                    uint32     size)
{
   IAddrRec *addrRec;
   int       wTotalFields;
   int       j;

   if((NULL == name) || (number == NULL)) {
      return;
   }

   if(WSTRLEN(number) == 0) {
     return;
   }

   name[0] = (AECHAR) '\0';

   if (NULL == pa) {
      return;
   }


   //Start searching the address book with the search string
   if (SUCCESS == IADDRBOOKEXT_EnumRecInitEx(pa,
                                             AEE_ADDR_CAT_NONE,
                                             AEE_ADDRFIELD_NONE,
                                             number,
                                             (uint16) WSTRSIZE(number),
                                             ADDRREC_SEARCHTYPE_EXACT |
                                             ADDRREC_FIELDID_PHONE)) {

      for (;;) {
         addrRec = IADDRBOOK_EnumNextRec(pa);

         if (NULL == addrRec) {
            // No more matching records found, exit loop
            break;
         }

         //Get the name of the Contact from the record
         wTotalFields = IADDRREC_GetFieldCount(addrRec);

         for (j = 0; j < wTotalFields; j++) {
            AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);

            if (addrField &&
                (addrField->fID == AEE_ADDRFIELD_NAME) &&
                (addrField->pBuffer != NULL) &&
                (addrField->wDataLen > 0) ) {

               (void) WSTRNCOPYN(name,
                          (int)(size / sizeof(AECHAR)),
                          addrField->pBuffer,
                          -1);

               // Found one, bail out immediately
               //
               // NOTE:
               //    There could be more than one contact database match,
               //    but for simplicity just take the first one that was
               //    found.
               //
               IADDRREC_Release(addrRec);
               return;
            }
         }

         IADDRREC_Release(addrRec);
      }
   }
}

/*=============================================================================
FUNCTION: DoSpeedDialLookup

DESCRIPTION:  Searches the Contacts database in the speed dial list category
              for a contact number match for the the speed dial index.

PARAMETERS:
   *pa             [in]:  IAddrBook interface pointer
   *speeddialindex [in]:  index number in the speed dial list range 1 - 99
   *number         [out]:  Phone number to lookup
   size:           [in]:  Size of name in bytes

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void DoSpeedDialLookup(IAddrBook *pa,
                       AECHAR *speeddialindex,
                       AECHAR *number,
                       uint32 size)
{
   IAddrRec *addrRec;
   int       wTotalFields;
   int       j;
   uint16    wIndex; //will hold integer value of speeddialindex
   boolean   bFound;

   if ((NULL == number) || (NULL == speeddialindex )) {
      return;
   }

   if(WSTRLEN(speeddialindex) == 0) {
      return;
   }

   number[0] = (AECHAR) '\0';

   if (NULL == pa) {
      return;
   }

   wIndex = (uint16)WSTRTOFLOAT(speeddialindex);

   //Start searching the address book with the search string
   if (SUCCESS == IADDRBOOK_EnumRecInit(pa,
                                        AEE_ADDR_CAT_SPEEDDIAL,
                                        AEE_ADDRFIELD_NONE,
                                        NULL,
                                        0)) {


      for (;;) {
         bFound = FALSE;
         addrRec = IADDRBOOK_EnumNextRec(pa);

         if (NULL == addrRec) {
            // No more matching records found, exit loop
            break;
         }

         //Get the name of the Contact from the record
         wTotalFields = IADDRREC_GetFieldCount(addrRec);

         for (j = 0; j < wTotalFields; j++) {
            AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);

            if (addrField && (addrField->fID == AEE_ADDRFIELD_INDEX)) {
               ASSERT(addrField->pBuffer != NULL);
               if(wIndex == *((uint16 *)addrField->pBuffer)){
                  bFound = TRUE;
               }
            }
         }

         /* Should not assume that order of fields within the record
            will remain the same. If that is a BREW requirement, we need
            to change handling of AEE_ADDRFIELD_INDEX in OEM layer */
         if(bFound==TRUE){
            for (j = 0; j < wTotalFields; j++) {
               AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);

               if (addrField &&
                           ((addrField->fID == AEE_ADDRFIELD_PHONE_OTHER) ||
                            (addrField->fID == AEE_ADDRFIELD_PHONE_GENERIC)) ) {
                  //if the slot id has already matched
                  (void) WSTRNCOPYN(number,
                             (int)(size / sizeof(AECHAR)),
                             addrField->pBuffer,
                             -1);

                  // Found one, bail out immediately
                  //
                  // NOTE:
                  //    There could be more than one contact database match,
                  //    but for simplicity just take the first one that was
                  //    found.
                  //
                  IADDRREC_Release(addrRec);
                  return;
               }
            }
         }

         IADDRREC_Release(addrRec);
      }
   }
}

/*=============================================================================
FUNCTION: FormatPhoneNumberForDisplay

DESCRIPTION:  Formats a phone number for display.

PARAMETERS:
  *number  [in]: A purely numeric phone number  (ie, "8585551234")

RETURN VALUE:
   AECHAR const *: A formatted phone number for display (ie. "858-555-1234")

COMMENTS:
   The returned string is contained in a dynamically allocated buffer, that the
   caller must release using FREE().

   Note: Hyphenation will stop at the first non-digit character.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AECHAR *FormatPhoneNumberForDisplay(AECHAR const *number)
{
   AECHAR       *buf;
   int           bufLen;
   int           len;
   int           destLen;
   AECHAR const *srcEnd;
   AECHAR        unavail[24];
   boolean       unavailable = FALSE;

   if (NULL == number) {
      return NULL;
   }


   if (*number == '\0') {
      STRTOWSTR("Unavailable", unavail, sizeof(unavail));
      number = unavail;
      unavailable = TRUE; 
   }

   len = WSTRLEN(number);

   // Since we mostly add a hyphen every three digits, we can expect to need an
   // extra 1/3 * len for the hyphens in the destination buffer.
   bufLen = ( 4 * (len+1) ) / 3;

   buf = MALLOC( (uint32) bufLen * sizeof(AECHAR));
   if (NULL == buf) {
      return NULL;
   }

   if(unavailable){
      (void) WSTRLCPY(buf, unavail, bufLen);
   }


   
   
   // Find the first non-digit or NULL
   for (srcEnd = number;
        (*srcEnd >= (AECHAR) '0') && (*srcEnd <= (AECHAR) '9');
        srcEnd++)
      ;

   // Start with a NULL at the end of the buffer
   destLen = bufLen - 1;
   buf[destLen] = (AECHAR) '\0';

   // If the input number contains some non-digits, copy them to the destination
   // buffer immediately
   if (*srcEnd != (AECHAR) '\0') {
      int extraLen = WSTRLEN(srcEnd);

      destLen -= extraLen;
      len     -= extraLen;

      (void) MEMCPY(&buf[destLen], srcEnd, (uint32) extraLen * sizeof(AECHAR));
   }

   // The following block will format the number according to the commonly used
   // North American convention: "8585551234" => "858-555-1234"
   //
   {
      boolean blockOfFour = TRUE;
      int     numRunCount = 0;

      // The numbers are processed backwards, starting from the last digit in
      // the number.  Each iteration of this loop processes the next digit:
      while (len > 0) {

         if (destLen <= 0) {
            ERR("Static buffer is too small for input", 0, 0, 0);
            return buf;
         }

         if (blockOfFour && (numRunCount == 4)) {
            blockOfFour = FALSE;

            destLen--;
            buf[destLen] = (AECHAR) '-';

            numRunCount = 0;

         } else if (!blockOfFour && (numRunCount == 3)) {
            destLen--;
            buf[destLen] = (AECHAR) '-';

            numRunCount = 0;

         } else {
            destLen--;
            len--;
            buf[destLen] = number[len];

            numRunCount++;
         }
      }
   }

   (void) MEMMOVE(buf,
           &buf[destLen],
           ((uint32) (bufLen - destLen) + 1) * sizeof(AECHAR));

   return buf;
}

/*=============================================================================
FUNCTION: GetTimeString

DESCRIPTION: Handles a key event for the ClockCtl object

PARAMETERS:
   *cfg: [in] IConfig object
   *tctl: [in] ITIMECtl object
   *ish: [in] ISHELL object
   timeVal: [in] The value of time in milliseconds
   *timeStr: [out] The output contaning the time string

RETURN VALUE:
   boolean:  TRUE if the key was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GetTimeString(IConfig *cfg, ITimeCtl *tctl, IShell *ish,
                   uint32 timeVal, AECHAR *timeStr)
/*lint -esym(715,cfg)*/
/*lint -esym(715,ish)*/
{
    int               strLen;

   // first use Time Control to get the time string in AM/PM format
   ITIMECTL_GetTimeString(tctl,
                          timeVal,
                          timeStr,
                          CLOCKCTL_MAX_TIMESTR_LEN,
                          // time string should be in accordance with the
                          // time format
                          GTS_AMPM);
   strLen = WSTRLEN(timeStr);
   timeStr[strLen] = 0;
}


#ifdef FEATURE_PHONE_VR
/*=============================================================================
FUNCTION: DoVoiceTagLookup

DESCRIPTION:  Searches the Contacts database in the speed dial list category
              for the given VoiceTag.

PARAMETERS:
   *pa             [in]:  IAddrBook interface pointer
   wUtt            [in]:  Voice Tag to be searched for
   *szName         [out]: Name of contact
   namesize:       [in]:  Size of szName in bytes
   *szNum          [out]: Phone number of contact
   numsize:        [in]:  Size of szNum in bytes

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean DoVoiceTagLookup(IAddrBook *pa,
                      AEEVRUtterance wUtt,
                      AECHAR *szName,
                      uint32 nameSize,
                      AECHAR *szNum,
                      uint32 numSize)
{
   IAddrRec *addrRec;
   int       wTotalFields;
   int       j;
   boolean   gotName = FALSE;
   boolean   gotNum  = FALSE;
   uint32    flags = 0x0;

   if (wUtt == AEEVRUTTERANCE_NONE) {
      return FALSE;
   }

   if (NULL == pa) {
      return FALSE;
   }

   flags = ADDRREC_DATATYPE_BYTE;

   if (SUCCESS != IADDRBOOKEXT_EnumRecInitEx(pa,
                                             AEE_ADDR_CAT_SPEEDDIAL,
                                             AEE_ADDRFIELD_VOICETAG,
                                             &wUtt,
                                             sizeof(AEEVRUtterance),
                                             flags)) {
       return FALSE;
   }

   if ((addrRec = IADDRBOOK_EnumNextRec(pa)) != NULL ) {
      //Get the name of the Contact from the record
      wTotalFields = IADDRREC_GetFieldCount(addrRec);

      for (j = 0; j < wTotalFields; j++) {
          AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);

          if (addrField &&
              (addrField->pBuffer != NULL) &&
              (addrField->wDataLen > 0) ) {

              if (addrField->fID == AEE_ADDRFIELD_NAME) {
                  (void) WSTRNCOPYN(szName,
                          (int)(nameSize / sizeof(AECHAR)),
                          addrField->pBuffer,
                          -1);
                  gotName = TRUE;
              }

              if (addrField->fType == AEEDB_FT_PHONE) {
                  (void) WSTRNCOPYN(szNum,
                          (int)(numSize / sizeof(AECHAR)),
                          addrField->pBuffer,
                          -1);
                  gotNum = TRUE;
              }
          }

          if (gotName && gotNum) {
            IADDRREC_Release(addrRec);
            return TRUE;
          }
      }

      IADDRREC_Release(addrRec);
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: GetVoiceTagByNumber

DESCRIPTION:  Searches the Contacts database in the speed dial list category
              for the given Number

PARAMETERS:
   *pa             [in]:  IAddrBook interface pointer
   *szNum          [in]:  Phone number to be searched
   pUtt            [out]: Voice Tag
   *szName         [out]: Name of contact
   namesize:       [in]:  Size of szName in bytes

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean GetVoiceTagByNumber(IAddrBook *pa,
                            AECHAR *szNum,
                            AEEVRUtterance *pUtt,
                            AECHAR *szName,
                            uint32 nameSize)
{
   IAddrRec *addrRec;
   int       wTotalFields;
   int       j;
   boolean   gotName = FALSE;
   boolean   gotVoice  = FALSE;

   if (NULL == pa || szNum == NULL) {
      return FALSE;
   }

   if (SUCCESS != IADDRBOOKEXT_EnumRecInitEx(pa,
                                             AEE_ADDR_CAT_SPEEDDIAL,
                                             AEE_ADDRFIELD_NONE,
                                             szNum,
                                             (uint16) WSTRSIZE(szNum),
                                             ADDRREC_SEARCHTYPE_CONTAINS |
                                             ADDRREC_FIELDID_PHONE)) {
       return FALSE;
   }


   while ((addrRec = IADDRBOOK_EnumNextRec(pa)) != NULL ) {
      //Get the name of the Contact from the record
      wTotalFields = IADDRREC_GetFieldCount(addrRec);

      for (j = 0; j < wTotalFields; j++) {
          AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);

          if (addrField &&
              (addrField->pBuffer != NULL) &&
              (addrField->wDataLen > 0) ) {

              if (addrField->fID == AEE_ADDRFIELD_NAME) {
                  (void) WSTRNCOPYN(szName,
                          (int)(nameSize / sizeof(AECHAR)),
                          addrField->pBuffer,
                          -1);
                  gotName = TRUE;
              }

              if (addrField->fID == AEE_ADDRFIELD_VOICETAG) {
                  *pUtt = *((AEEVRUtterance *)addrField->pBuffer);
                  gotVoice = TRUE;
              }
          }

          if (gotName && gotVoice) {
            IADDRREC_Release(addrRec);
            return TRUE;
          }
      }

      IADDRREC_Release(addrRec);
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: GetVoiceTagList

DESCRIPTION:  Gets the list of all voice tags

PARAMETERS:
   *pa             [in]:  IAddrBook interface pointer
   list            [out]: Voice tags
   listsize        [in]:  Size of Voice Tag array in bytes

RETURN VALUE:
   Number of voice tags found

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16 GetVoiceTagList(IAddrBook *pa,
                       AEEVRUtterance *list,
                       uint32 listsize)
{
  IAddrRec     *addrRec;
  int          wTotalFields;
  int          j;
  uint16       count = 0;
  uint32       maxUtt = listsize/sizeof(AEEVRUtterance);

  if (list == NULL || listsize == 0)
    return 0;

  if (SUCCESS != IADDRBOOKEXT_EnumRecInitEx(pa,
                                            AEE_ADDR_CAT_SPEEDDIAL,
                                            AEE_ADDRFIELD_VOICETAG,
                                            NULL,
                                            0,
                                            0)) {
       return 0;
  }


  while (((addrRec = IADDRBOOK_EnumNextRec(pa)) != NULL) && (count < maxUtt)) {
    wTotalFields = IADDRREC_GetFieldCount(addrRec);

    for (j = 0; j < wTotalFields; j++) {
      AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);

      if (addrField && (addrField->pBuffer != NULL) &&
                       (addrField->wDataLen > 0) &&
                       (addrField->fID == AEE_ADDRFIELD_VOICETAG)) {
          list[count] = *((AEEVRUtterance *)addrField->pBuffer);
          count++;
      }
    }

    IADDRREC_Release(addrRec);

  }
  return count;
}
#endif /* FEATURE_PHONE_VR */

/*=============================================================================
FUNCTION: OriginateVoiceCall

DESCRIPTION:  Originate voice call

PARAMETERS:
   number - number to be dialed

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
  This function will be called from another apps context.
  The number buffer must be NULL terminated.

SIDE EFFECTS:
  None

=============================================================================*/
int OriginateVoiceCall (const char* number)
{
#ifndef FEATURE_UIONE_HDK
  IShell* pIShell=NULL;
  uint32 len;

  pIShell = AEE_GetShell();

  if (pIShell == NULL) {
    return EFAILED;
  }

  if (number == NULL) {
    return EFAILED;
  }

  len = STRLEN(number);

  if (len == 0) {
    return EFAILED;
  }

  // Call dialer function to originate voice call
  if(CDialerApp_StartApp_OriginateVoiceCall(pIShell, number) == FALSE)
  {
    return EFAILED;
  }

  return SUCCESS;
#else
  return EFAILED;
#endif
}

/*=============================================================================
FUNCTION: AnswerVoiceCall

DESCRIPTION:  Answers voice call if there is one to answer and does nothing if
              not

PARAMETERS:
  boolean: TRUE will answer call and FALSE will reject

RETURN VALUE:
   SUCCESS - if successfully sent to Dialer.  Does not mean it was ansewered.

COMMENTS:
  This function will be called from another apps context.

SIDE EFFECTS:
  None

=============================================================================*/
int AnswerVoiceCall(boolean bAnswer)
{
#ifndef FEATURE_UIONE_HDK
  IShell *pIShell = AEE_GetShell();

  if (pIShell == NULL) 
  {
    return EFAILED;
  }

  // Call dialer function to answer voice call
  if(CDialerApp_AnswerVoiceCall(pIShell, bAnswer) == FALSE)
  {
    return EFAILED;
  }

  return SUCCESS;
#else
  return EFAILED;
#endif
}

#ifdef FEATURE_WMS_APP
/*=============================================================================
FUNCTION: OriginateMessage

DESCRIPTION:  Originate a Message (SMS) to a phone number or EMail Address

PARAMETERS:
   isEmail - boolean If Email Address
   Address - Phone Number or Email Address

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
  This function will be called from another apps context.
  The Address buffer must be NULL terminated.

SIDE EFFECTS:
  None

=============================================================================*/
int OriginateMessage (const boolean isEmail,
                      const char* Address)
{
  IShell* pIShell=NULL;
  int len;
  char *pBuf=NULL;
  
  pIShell = AEE_GetShell();

  if (pIShell == NULL) {
    return EFAILED;
  }

  len = STRLEN(Address);

  if (len == 0) {
    return EFAILED;
  }
    
  /* The Buffer allocated here will be freed in WmsApp_HandleEvent while */
  /* handling EVT_WMSAPP_MESSAGE_COMPOSE */
  pBuf = MALLOC (len+1);
  if (pBuf == NULL) {
    return EFAILED;
  }

  MEMCPY (pBuf, Address, len+1);

  /* Post event to WMSAPP */
  ISHELL_PostEventEx (pIShell, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                     EVT_WMSAPP_MESSAGE_COMPOSE, isEmail,(uint32)pBuf);
  return SUCCESS;
}
#endif /* FEATURE_WMS_APP */

/* Tab Controller functions */
/*===========================================================================
FUNCTION TabController_HandleEvent

DESCRIPTION
   The event handler for the TabController.

The valid set of events.

    1. List Control :   Up/Down is tab and Left/Right moves Up/Down in  the menu.
    2. SK Control   :   Up/Down is tab.
    3. Text Control :   Up/Down is tab and Select moves to next control.
    4. Menu Control :   Up/Down moves inside the menu control and Left/Right is tab.

    All other events are ignored.

    The "tab-direction" is stored in Key_Press event and executed in Key_Release event.
    This storage is required for correct operation of the text controls.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void TabController_HandleEvent(DlgTabCtl** pDlgTabCtl, AEEEvent evt, uint16 wParam)
{
    switch(evt)
    {
    case EVT_KEY_PRESS:
      TabController_ProcessKeyPress(*pDlgTabCtl, wParam);
      break;

    case EVT_KEY_RELEASE:
        //Change focus implementation.
      (void) TabController_ProcessKeyRelease(*pDlgTabCtl);
      break;

    case EVT_DIALOG_END:
      TabController_End(pDlgTabCtl);
      break;

    default:
      break;
    }
}


/*===========================================================================
FUNCTION TabController_SetActiveEx

DESCRIPTION
 Set a particular control active and all other controls inactive(Read: Set Active Exclusive")
 To be called after the controller is initialized, the controls added and you want
 a particular control to be in focus at the start.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void TabController_SetActiveEx(DlgTabCtl *pDlgTabCtl, int index)
{
    TabController_SetAllInactive(pDlgTabCtl);
    ICONTROL_SetActive(pDlgTabCtl->ctls[index], TRUE);
}

/*===========================================================================
FUNCTION TabController_AddControl

DESCRIPTION
 Add control to the controller. Must have called TabController_Init function with
 correct number of controls before addition.


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void TabController_AddControl(DlgTabCtl *pDlgTabCtl, int CtlType, IControl *pIControl)
{
    pDlgTabCtl->ctls[pDlgTabCtl->numCtls] = pIControl;
    pDlgTabCtl->ctlType[pDlgTabCtl->numCtls] = CtlType;
    pDlgTabCtl->numCtls++;
}

/*===========================================================================
FUNCTION TabController_Init

DESCRIPTION
 Init function, initialized the controller, must pass the correct value of the
 number of controls to be controlled.


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void TabController_Init(DlgTabCtl **pDlgTabCtl, uint16 numControls)//, IShell *pIShell, AEECLSID clsId)
{
    if(*pDlgTabCtl != NULL)
    {
        DBGPRINTF("Possibility of TabController overwritten..Initialize to NULL/ Use new control for multilevel dialog tab control");
    }
    *pDlgTabCtl = (DlgTabCtl*) MALLOC(sizeof(DlgTabCtl));
    (*pDlgTabCtl)->numCtls = 0;
    (*pDlgTabCtl)->currentActiveCtl = 0;
    (*pDlgTabCtl)->ctls = (IControl**) MALLOC(sizeof(IControl*) * numControls);
    (*pDlgTabCtl)->ctlType = (int*) MALLOC(sizeof(int) * numControls);
    (*pDlgTabCtl)->tabDirection = 0;


}

/*===============================================================================
Static functions.
===============================================================================*/


/*===========================================================================
FUNCTION TabController_IsTabEvent

DESCRIPTION
   Determine if a particular event is a tab event
   Currently supported events

    1. List Control :   Up/Down is tab and Left/Right moves Up/Down in  the menu.
    2. SK Control   :   Up/Down is tab.
    3. Text Control :   Up/Down is tab and Select moves to next control.
    4. Menu Control :   Up/Down moves inside the menu control and Left/Right is tab.

    Also stored is the correct "tab direction"
    1 => Move down ,-1 => Move up, 0 => Not a tabbing operation.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

static boolean TabController_IsTabEvent(DlgTabCtl* pDlgTabCtl, uint16 keyCode)
{

    // Set the tabDirection field here too.

    int k = pDlgTabCtl->currentActiveCtl;

    pDlgTabCtl->tabDirection = 0;

    switch(pDlgTabCtl->ctlType[k])
    {
    case TEXT_CTL:
        if(keyCode == AVK_UP)
        {
            pDlgTabCtl->tabDirection = -1;
            return TRUE;
        }
        else if(keyCode == AVK_DOWN)
        {
            pDlgTabCtl->tabDirection = 1;
            return TRUE;
        }
        else if(keyCode == AVK_SELECT)
        {
            pDlgTabCtl->tabDirection = 1;
            return TRUE;
        }
        break;

    case MENU_CTL:
        if(keyCode == AVK_LEFT)
        {
            pDlgTabCtl->tabDirection = -1;
            return TRUE;
        }
        else if(keyCode == AVK_RIGHT)
        {
            pDlgTabCtl->tabDirection = 1;
            return TRUE;
        }
        break;

    case LIST_CTL:

        if(keyCode == AVK_UP)
        {
            pDlgTabCtl->tabDirection = -1;
            return TRUE;
        }
        else if(keyCode == AVK_DOWN)
        {
            pDlgTabCtl->tabDirection = 1;
            return TRUE;
        }
        else if(keyCode == AVK_LEFT)
        {
            TabController_SetMenuSel((IMenuCtl*)pDlgTabCtl->ctls[k], -1);
            return TRUE;
        }
        else if(keyCode == AVK_RIGHT)
        {
            TabController_SetMenuSel((IMenuCtl*)pDlgTabCtl->ctls[k], 1);
            return TRUE;
        }
        break;

    case SK_CTL:
        if(keyCode == AVK_UP)
        {
            pDlgTabCtl->tabDirection = -1;
            return TRUE;
        }
        else if(keyCode == AVK_DOWN)
        {
            pDlgTabCtl->tabDirection = 1;
            return TRUE;
        }
        break;

    default:
        break;
    }
    return FALSE;

}


/*===========================================================================
FUNCTION TabController_SetAllInactive

DESCRIPTION
 Sets all the controllers inactive
 Is required because , if not all the controls are set to inactive
 Dialog itself will do tabbing, and mess up.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void TabController_SetAllInactive(DlgTabCtl *pDlgTabCtl)
{
    int i;
    for(i = 0; i < pDlgTabCtl->numCtls; i++)
    {
        ICONTROL_SetActive(pDlgTabCtl->ctls[i], FALSE);
    }
}


/*===========================================================================
FUNCTION TabController_ProcessKeyPress

DESCRIPTION
 Key Press event. "tab direction" is stored in "IsTabEvent" function/

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void TabController_ProcessKeyPress(DlgTabCtl *pDlgTabCtl, uint16 wParam)
{
    if(TabController_IsTabEvent(pDlgTabCtl, wParam))
    {
        TabController_SetAllInactive(pDlgTabCtl);
    }
}


/*===========================================================================
FUNCTION TabController_ProcessKeyRelease

DESCRIPTION
 Key Release event. If the "tab direction" is set , set the correct control to be active

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean TabController_ProcessKeyRelease(DlgTabCtl *pDlgTabCtl)
{
    int dir = (int) pDlgTabCtl->tabDirection;

    if(dir!=0)
    {
        int i;
        int k = pDlgTabCtl->currentActiveCtl;

        for(i = 0; i < pDlgTabCtl->numCtls; i++)
        {
            ICONTROL_SetActive(pDlgTabCtl->ctls[i], FALSE);
        }

        pDlgTabCtl->currentActiveCtl = (k + dir + pDlgTabCtl->numCtls)%pDlgTabCtl->numCtls;
        ICONTROL_SetActive(pDlgTabCtl->ctls[pDlgTabCtl->currentActiveCtl], TRUE);
        pDlgTabCtl->tabDirection = 0;
        return TRUE;
    }
    else
    {
        ICONTROL_SetActive(pDlgTabCtl->ctls[pDlgTabCtl->currentActiveCtl], TRUE);
    }
    return FALSE;
}


/*===========================================================================
FUNCTION TabController_End

DESCRIPTION
 Free the memory used by the tabcontroller.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void TabController_End(DlgTabCtl** pDlgTabCtl)
{
    if ((pDlgTabCtl == NULL) || (*pDlgTabCtl == NULL))
    {
      return;
    }

    FREEIF((*pDlgTabCtl)->ctls);
    FREEIF((*pDlgTabCtl)->ctlType);
    FREEIF((*pDlgTabCtl));
}

/*===========================================================================
FUNCTION TabController_SetMenuSel

DESCRIPTION
  Set the selection for the menuItem, based on the direction stored during
  IsTabEvent..Left is up and Right is down.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void TabController_SetMenuSel(IMenuCtl *pIMenuCtl, int dir)
{
    int numItems = IMENUCTL_GetItemCount(pIMenuCtl);
    uint16 curSel = IMENUCTL_GetSel(pIMenuCtl);
    int indexSel = 0;
    int i;

    if(numItems == 0)
        return;

    for(i=0; i<numItems; i++)
    {
        if(IMENUCTL_GetItemID(pIMenuCtl, i) == curSel)
        {
            indexSel = i;
            break;
        }
    }
    indexSel = (indexSel + numItems + dir) % numItems;
    IMENUCTL_SetSel(pIMenuCtl, IMENUCTL_GetItemID(pIMenuCtl, indexSel));
}
/* End of Tab Controller functions */
/*=============================================================================
FUNCTION: SetIdleMenuText

DESCRIPTION:  Sets text in the static control of the idle menu.

PARAMETERS:
   AECHAR * to the text to be displayed

RETURN VALUE:
   TRUE if event was sent to CoreApp. (Not if the text was set)
   FALSE if otherwise.

COMMENTS:
   None

SIDE EFFECTS:
  Suspends any other app and brings up Idle Menu if non-null text.

=============================================================================*/
boolean SetIdleMenuText (AECHAR *pszText)
{
 IShell *pIShell;
 AECHAR *pszBuf = NULL; 

 pIShell = AEE_GetShell() ;
 if(pIShell == NULL)
 {
   MSG_ERROR("pIShell NULL", 0, 0, 0);
   return FALSE;
 }
 if(pszText != NULL)
 {
   pszBuf = (AECHAR *) MALLOC ((MAX_BROADCAST_MSG_SIZE + 1)*sizeof(AECHAR));
   if(pszBuf == NULL)
   {
    MSG_ERROR("Malloc Failed", 0, 0, 0);
    return FALSE;
   }
 
   (void) WSTRNCOPYN(pszBuf, (MAX_BROADCAST_MSG_SIZE + 1), pszText, WSTRLEN(pszText));
   pszBuf[MAX_BROADCAST_MSG_SIZE] = '\0';
 }
 ISHELL_PostEventEx(pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP, EVT_USER, 
    EVT_CORE_SET_IDLETEXT, (uint32) pszBuf);

 return TRUE;
}

/*=============================================================================
FUNCTION: SetIdleMenuTextInBkgrnd

DESCRIPTION:  Sets text in the static control of the idle menu.

PARAMETERS:
   AECHAR * to the text to be displayed

RETURN VALUE:
   TRUE if event was sent to CoreApp. (Not if the text was set)
   FALSE if otherwise.

COMMENTS:
   None

SIDE EFFECTS:
  Updates the Idle menu dlg only if the text is not null

=============================================================================*/
boolean SetIdleMenuTextInBkgrnd (AECHAR *pszText)
{
 IShell *pIShell;
 AECHAR *pszBuf = NULL; 

 pIShell = AEE_GetShell() ;
 if(pIShell == NULL)
 {
   MSG_ERROR("pIShell NULL", 0, 0, 0);
   return FALSE;
 }
 if(pszText != NULL)
 {
   pszBuf = (AECHAR *) MALLOC ((MAX_BROADCAST_MSG_SIZE + 1)*sizeof(AECHAR));
   if(pszBuf == NULL)
   {
    MSG_ERROR("Malloc Failed", 0, 0, 0);
    return FALSE;
   }
 
   (void) WSTRNCOPYN(pszBuf, (MAX_BROADCAST_MSG_SIZE + 1), pszText, WSTRLEN(pszText));
   pszBuf[MAX_BROADCAST_MSG_SIZE] = '\0';
 }
 ISHELL_PostEventEx(pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP, EVT_USER, 
    EVT_CORE_SET_TEXT_WHEN_APP_SUSPENDED, (uint32) pszBuf);

 return TRUE;
}

/*=========================================================================
FUNCTION
  AppCom_ConvertGSMDefaultToASCII

DESCRIPTION
  Convert 7-bit GSM Default alphabet string to ASCII.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void AppCom_ConvertGSMDefaultToASCII
(
  const uint8   *gsmdef,
  uint32        len,
  uint8         *ascii
)
{
  uint32  i, j;

  for( i = 0; i < len; i ++ )
  {
    j = gsmdef[i];
    ascii[i] = AppCom_GSMDefault_To_Ascii_Table[j];
  }
  ascii[i] = '\0';
} /* AppCom_ConvertGSMDefaultToASCII() */

/*=========================================================================
FUNCTION
  AppCom_ConvertGSMDefaultToASCII

DESCRIPTION
  Convert 7-bit packed array into a NUL Terminated ASCII string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
byte AppCom_ConvertPacked7BitToASCII
(
  const uint8 *ud,             /* IN */
  uint32      ud_len,          /* IN */
  uint32      buf_len,         /* IN */
  uint8       *buf             /* OUT */
)
{
  byte i;
  word bit_pos;

  for( i=0, bit_pos=0; i<buf_len && bit_pos<ud_len*8; i++, bit_pos+=7 )
  {
    buf[i] = b_unpackb( (byte*)ud, bit_pos, 7 );
  }

  buf[i] = 0;

  return i;

} /* AppCom_ConvertPacked7BitToASCII() */

/*=========================================================================
FUNCTION
  AppCom_ConvertPacked7BitToGSMDefault

DESCRIPTION
  Unpack raw data to GSM 7-bit characters.

DEPENDENCIES
  None

RETURN VALUE
  Number of 7-bit charactes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint32 AppCom_ConvertPacked7BitToGSMDefault
(
  const uint8       *in,
  uint32            in_len,        /* Number of 7-bit input characters to be decoded */
  uint32            out_len,       /* output buffer length */
  uint16            shift,         /* offset from which the data will start */
  uint8             *out
)
{
  int      i=0;
  uint32   pos=0;

  /*If the number of fill bits != 0, then it would cause an additional shift*/
  if (shift != 0)
   pos = pos+1;

  if (shift ==7)
  {
    out[0] = in[0] >> 1; /*7 fillbits*/
    shift =0;            /*First Byte is a special case*/
    i=1;
  }



  for( i=i;
       i < out_len && i< in_len;
       i++, pos++ )
  {
    out[i] = ( in[pos] << shift ) & 0x7F;

    if( pos != 0 )
    {
      /* except the first byte, a character contains some bits
      ** from the previous byte.
      */
      out[i] |= in[pos-1] >> (8-shift);
    }

    shift ++;

    if( shift == 7 )
    {
      shift = 0;

      /* a possible extra complete character is available */
      i ++;
      out[i] = in[pos] >> 1;

    }
  }

  return pos;

} /* AppCom_ConvertPacked7BitToGSMDefault() */
/*=============================================================================
FUNCTION: DisplayString

DESCRIPTION:  displays the string on Dialer

PARAMETERS:
   pszBuf - string to be displayed

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
  This function will be called from another apps context.
  The number buffer must be NULL terminated.

SIDE EFFECTS:
  None

=============================================================================*/
int DisplayString (const AECHAR* pszBuf)
{
  IShell* pIShell=NULL;
  uint32 len;

  pIShell = AEE_GetShell();

  if (pIShell == NULL) {
    return EFAILED;
  }

  if (pszBuf == NULL) {
    return EFAILED;
  }

  len = WSTRLEN(pszBuf);

  if (len == 0) {
    return EFAILED;
  }

#ifndef FEATURE_UIONE_HDK
  // Call dialer function to display string
  if(CDialerApp_StartApp_DisplayMsg(pIShell, pszBuf) == FALSE)
  {
    return EFAILED;
  }

  return SUCCESS;
#else
  return EFAILED;
#endif
}


/*=============================================================================
FUNCTION: SetBacklight

DESCRIPTION:  Sets the backlight on or off

PARAMETERS:
   onOff - backlight setting

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
  This function will be called from another apps context.
  After the backlight turns off, the backlight duration value willn go back to the original NV value.

SIDE EFFECTS:
  None

=============================================================================*/
int SetBacklight(boolean onOff)
{
  IShell *pIShell;
  
  pIShell = AEE_GetShell() ;
  if(pIShell == NULL)
  {
    MSG_ERROR("pIShell NULL", 0, 0, 0);
    return FALSE;
  }
  ISHELL_PostEvent(pIShell, AEECLSID_CORE_APP, EVT_USER, 
     EVT_SET_BACKLIGHT, (boolean) onOff);
  
  return TRUE;


}

/*=============================================================================
FUNCTION: GetPrimaryScreenSize

DESCRIPTION:  Determines the size of the primary display

PARAMETERS:
   None
   onOff - backlight setting

RETURN VALUE:
   One of the following values.
     COREAPP_VGA_RES
     COREAPP_QVGA_RES
     COREAPP_SUB_QVGA_RES
   If the screen size cannot be determined, the default value will be
   COREAPP_QVGA_RES.

COMMENTS:
  None

SIDE EFFECTS:
  None

=============================================================================*/
uint32 GetPrimaryScreenSize(void)
{
  AEEDeviceInfo  sDevInfo;
  int            nScreenSize = APPCOM_QVGA_RES;
  IShell        *pIShell = NULL;

  pIShell = AEE_GetShell() ;
  if (! pIShell) {
    MSG_MED("Failed to get shell pointer.", 0, 0, 0);
    return nScreenSize;
  }

  /* Get the dimensions of the primary display */
  ISHELL_GetDeviceInfo(pIShell, &sDevInfo);

  /* Check the display size in both landscape and portrait orientations. */
  if ((((sDevInfo.cxScreen + BREW_OFFSET_X >= APPCOM_VGA_DIM1)) &&
       ((sDevInfo.cyScreen + BREW_OFFSET_Y) >= APPCOM_VGA_DIM2)) ||
      (((sDevInfo.cxScreen + BREW_OFFSET_X) >= APPCOM_VGA_DIM2) &&
       ((sDevInfo.cyScreen + BREW_OFFSET_Y) >= APPCOM_VGA_DIM1))) {
    /* The primary display is VGA or larger. */
    nScreenSize = APPCOM_VGA_RES;
  }
  else if ((((sDevInfo.cxScreen + BREW_OFFSET_X >= APPCOM_QVGA_DIM1)) &&
           ((sDevInfo.cyScreen + BREW_OFFSET_Y) >= APPCOM_QVGA_DIM2)) ||
          (((sDevInfo.cxScreen + BREW_OFFSET_X) >= APPCOM_QVGA_DIM2) &&
            ((sDevInfo.cyScreen + BREW_OFFSET_Y) >= APPCOM_QVGA_DIM1))) {
    /* The primary display is at least QVGA. */
    nScreenSize = APPCOM_QVGA_RES;
  }
  else {
    /* The primary display is smaller than QVGA. */
    nScreenSize = APPCOM_SUB_QVGA_RES;
  }
  return nScreenSize;
}

#if MIN_BREW_VERSIONEx(3,1,4)
/*===========================================================================
FUNCTION AppComFunc_CleanupFontData

DESCRIPTION
  Restores the original display font settings and releases resources
  that were allocated for setting up the fonts.

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static void AppComFunc_CleanupFontData(IDisplay* pIDisp, IFont** ppOldNormalFont,
				       IFont** ppOldBoldFont, IFont** ppOldLargeFont)
{

  IFont  *pIFont;
  IFont* pOldNormalFont = NULL;
  IFont* pOldBoldFont = NULL;
  IFont* pOldLargeFont = NULL;

  if(ppOldNormalFont != NULL)
    pOldNormalFont = *ppOldNormalFont;
  if(ppOldBoldFont != NULL)
    pOldBoldFont   = *ppOldBoldFont;
  if(ppOldLargeFont != NULL)
    pOldLargeFont  = *ppOldLargeFont;
  

  /* For each of the fonts, if we were able to set it (i.e. if the "old font"
   * pointer is not NULL, we need to revert to the old font and release
   * the font we created.
   */
  if (pOldNormalFont) {
    pIFont = NULL;
    pIFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_NORMAL,
                              pOldNormalFont);
    if (! pIFont) {
      MSG_MED("Set font failed.", 0, 0, 0);
    }
    else {
      (void)IFONT_Release(pIFont);
    }
  }
      
  if (pOldBoldFont) {
    pIFont = NULL;
    pIFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_BOLD,
                              pOldBoldFont);
    if (! pIFont) {
      MSG_MED("Set font failed.", 0, 0, 0);
    }
    else {
      (void)IFONT_Release(pIFont);
    }
  }

  if (pOldLargeFont) {
    pIFont = NULL;
    pIFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_LARGE,
                              pOldLargeFont);
    if (! pIFont) {
      MSG_MED("Set font failed.", 0, 0, 0);
    }
    else {
      (void)IFONT_Release(pIFont);
    }
  }
 
  *ppOldNormalFont = NULL;
  *ppOldBoldFont = NULL;
  *ppOldLargeFont = NULL;
}
#endif // MIN_BREW_VERSIONEx(3,1,4)

/*===========================================================================  
FUNCTION AppComFunc_SelectFonts

DESCRIPTION
  Sets up the application font data and sets the correct fonts for the
  primary display

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
void AppComFunc_SelectFonts(IDisplay* pIDisp) {

  // Only targets with brew 3.1.4 or greater have these capabilities
#if MIN_BREW_VERSIONEx(3,1,4)
  IFont    *pIFont;
  int      nErrCode;
  boolean  bFontsWereSet = FALSE;
  IFont    *pOldNormalFont = NULL;
  IFont    *pOldBoldFont = NULL;
  IFont    *pOldLargeFont = NULL;
  IShell   *pIShell = NULL;  
  
  /* We want to set the fonts for the entire display.  If that fails,
   * we'll revert back to the old font values.
   */

  if(!pIDisp) { /* No IDisplay means we can't do anything. */    
    MSG_MED("No IDisplay passed to AppComFunc_SelectFonts.", 0, 0, 0);
    return;
  }

  if(GetPrimaryScreenSize() != APPCOM_VGA_RES) /* Only upgrade fonts on VGA */
    return;
  
  do {
    pIShell = (IShell*)AEE_GetShell();
    if(!pIShell) {
      MSG_HIGH("AEE_GetShell failed.", 0, 0, 0);
      return;
    }
      
    pIFont = NULL;
    nErrCode = ISHELL_CreateInstance(pIShell, AEECLSID_FONT_STANDARD23,
                                     (void **)&pIFont);
    if ((nErrCode != SUCCESS) || (! pIFont)) {
      MSG_MED("Failed to create a font.", 0, 0, 0);
      break;
    }
    pOldNormalFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_NORMAL, pIFont);
    if (!pOldNormalFont) {
      IFONT_Release(pIFont);
      MSG_MED("Failed to set a font.", 0, 0, 0);
      break;
    }

    pIFont = NULL;
    nErrCode = ISHELL_CreateInstance(pIShell, AEECLSID_FONT_STANDARD23B,
                                     (void **)&pIFont);
    if ((nErrCode != SUCCESS) || (! pIFont)) {   
      MSG_MED("Failed to create a font.", 0, 0, 0);
      break;
    }
    pOldBoldFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_BOLD, pIFont);
    if (!pOldBoldFont) {     
      IFONT_Release(pIFont);
      MSG_MED("Failed to set a font.", 0, 0, 0);
      break;
    }
    
    pIFont = NULL;
    nErrCode = ISHELL_CreateInstance(pIShell, AEECLSID_FONT_STANDARD36,
                                     (void **)&pIFont);
    if ((nErrCode != SUCCESS) || (! pIFont)) {
      MSG_MED("Failed to create a font.", 0, 0, 0);
      break;
    }
    pOldLargeFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_LARGE, pIFont);
    if (!pOldLargeFont) {
      IFONT_Release(pIFont);
      MSG_MED("Failed to set a font.", 0, 0, 0);
      break;
    }

    bFontsWereSet = TRUE;

  } while (0); /*lint -save -e506 "constant value boolean" */

  if (bFontsWereSet == FALSE) {
    AppComFunc_CleanupFontData(pIDisp, &pOldNormalFont, &pOldBoldFont, &pOldLargeFont);
  }
  else {
    /* If successful, release all the old fonts, as no one has access to them anymore. */
    IFONT_Release(pOldNormalFont);
    IFONT_Release(pOldBoldFont);
    IFONT_Release(pOldLargeFont);
  }

#else  
  return;
#endif
}

/*===========================================================================  
FUNCTION AppComFunc_DisplaySelectNetworkDialog

DESCRIPTION
  Signals coreapp to launch the select network dialog.

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
void AppComFunc_DisplaySelectNetworkDialog(IShell* pIShell, uint32 dwParam)
{
  ISHELL_PostEvent(pIShell, AEECLSID_CORE_APP, EVT_USER, 
                   EVT_SELECT_NETWORK, dwParam);
}

#endif //ifndef FEATURE_MANGO_UI
/*=============================================================================
FUNCTION: ClearBlockUITask

DESCRIPTION:  Clears the signal we are going to block on.

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   None.

SIDE EFFECTS:
  None

=============================================================================*/
void ClearBlockUITask(void)
{
  MSG_HIGH("UI Task UI_OEM_SIG being cleared", 0, 0, 0);
  rex_clr_sigs(&ui_tcb, UI_OEM_SIG);
}

/*=============================================================================
FUNCTION: BlockUITask

DESCRIPTION:  Blocks the UI Task until a signal is received.

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
  This function blocks the entire UI task waiting for UnBlockUITask
  to be called.

SIDE EFFECTS:
  None

=============================================================================*/
void BlockUITask(void)
{
  MSG_HIGH("UI Task Waiting on UI_OEM_SIG being set", 0, 0, 0);

  ui_wait (UI_OEM_SIG);
  rex_clr_sigs(&ui_tcb, UI_OEM_SIG);
}

/*=============================================================================
FUNCTION: UnblockUITask

DESCRIPTION:  Sets the signal to free the UI task.

PARAMETERS:
   onOff - backlight setting

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
  This function will be called from another tasks context.
  After the UI task is blocked, we need this signal to unblock it.

SIDE EFFECTS:
  None

=============================================================================*/
void UnblockUITask(void)
{
  MSG_HIGH("UI Task UI_OEM_SIG being set", 0, 0, 0);

  rex_set_sigs(&ui_tcb, UI_OEM_SIG);
}

/*=============================================================================
FUNCTION: IsCallActive

DESCRIPTION:  Returns if UI is in a call

PARAMETERS:
   call type - type of call being queried
   Passing in AEECM_CALL_TYPE_NONE
   for call type queries for all call types

RETURN VALUE:
   TRUE - if in a call
   FALSE - otherwise

COMMENTS:
   None

SIDE EFFECTS:
  None

=============================================================================*/
boolean IsCallActive (AEECMCallType calltype)
{  
  IShell *pIShell=NULL;
  int nCalls = 0;
  pIShell = AEE_GetShell();
  if (!pIShell)
    return FALSE;
#ifndef FEATURE_MANGO_BREW
  {
     ICM *pICM=NULL;
     ISHELL_CreateInstance(pIShell, AEECLSID_CM, (void **) &pICM);
     if (!pICM)
        return FALSE;
     nCalls = ICM_GetActiveCallIDs(pICM, calltype, AEECM_CALL_STATE_NONE,NULL, 0);
     ICM_Release (pICM);
  }
#else
  {
    AEETCalls po;
    ITelephone *pTelephone;
    if(SUCCESS != ISHELL_CreateInstance(pIShell, AEECLSID_TELEPHONE, (void **) &pTelephone))
       return FALSE;
    if(SUCCESS != ITELEPHONE_GetCalls(pTelephone, &po, sizeof(po)) )
       return FALSE;
    ITELEPHONE_Release(pTelephone);
    nCalls = po.dwCount;
  }
#endif
  if (nCalls > 0)
    return TRUE;
  return FALSE;
}