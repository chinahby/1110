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

(c) COPYRIGHT 2002 - 2003 QUALCOMM Incorporated.
                   All Rights Reserved.
                   QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================

  $Header: //depot/asic/qsc60x0/apps/core/AppComFunc.c#1 $
$DateTime: 2006/03/17 13:20:59 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
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

#include "AEEShell.h"
#include "AEEMenu.h"
#include "AEEText.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AppComFunc.h"
#include "AEEAddrBookExt.h"
#include "AEEConfig.h"
#include "OEMDisp.h"

//#include "CoreApp.h"
//#include "CoreApp_res.h"
#include "AEE_OEM.h"
#include "OEMClassIDs.h"
#include "Appscommon.h"

#include "appscommon.brh"
#include "appscommonimages.brh"

#ifdef WIN32
#include "oemhelperfunctype.h"
#endif//WIN32

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

#define CLOCKCTL_MS_IN_HOUR      (60 * 60 * 1000)
#define CLOCKCTL_MS_IN_MIN       (60 * 1000)
#define CLOCKCTL_MS_IN_DAY       (24 * 60 * 60 * 1000)
#define CLOCKCTL_MAX_TIMESTR_LEN 20

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
      IDISPLAY_ClearScreen(pd);
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

   MEMSET(&cai, 0, sizeof(cai));
   //cai.pszResImage = COREAPP_RES_FILE;
   cai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
   cai.wImage = IDB_RADIO_UNFILLED;

   for (i = IMENUCTL_GetItemCount(pm); i >= 0; i--) {
      (void) IMENUCTL_SetItem(pm,
                              IMENUCTL_GetItemID(pm, i),
                              MSIF_IMAGE,
                              &cai);
   }

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

   MEMSET(&cai, 0, sizeof(cai));

   //cai.pszResImage = COREAPP_RES_FILE;
   cai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
   cai.wImage = bSel ? IDB_RADIO_FILLED : IDB_RADIO_UNFILLED;

   (void) IMENUCTL_SetItem(pm, listId, MSIF_IMAGE, &cai);
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
   //CtlAddItem cai;
   int        i;

   if(!pm)
   {
      return;
   }

   //MEMSET(&cai, 0, sizeof(cai));
   //cai.pszResImage = COREAPP_RES_FILE;

   for (i = IMENUCTL_GetItemCount(pm); i >= 0; i--) {
#if 0
      /*(void) IMENUCTL_SetItem(pm,
        IMENUCTL_GetItemID(pm, i),
        MSIF_IMAGE,
        &cai);*/
      cai.dwData = 0;
      (void) IMENUCTL_SetItem(pm,
            IMENUCTL_GetItemID(pm, i),
            MSIF_DATA,
            &cai);
#else
      IMENUCTL_SetSelEx(pm, IMENUCTL_GetItemID(pm, i), FALSE);
#endif//if 0
   }

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
#if 0
   CtlAddItem cai;

   MEMSET(&cai, 0, sizeof(cai));

   //cai.pszResImage = COREAPP_RES_FILE;
   //cai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE; //wuuquan.tang delete 20081111
   //cai.wImage = bSel ? IDB_CHECK_ON : IDB_CHECK_OFF; //wuuqan.tnag delete 20081111


   cai.dwData = bSel ? 1:0;
   (void) IMENUCTL_SetItem(pm, listId, MSIF_DATA, &cai);
#else
   if(!pm)
   {
      return;
   }
   IMENUCTL_SetSelEx(pm, listId, bSel);
#endif//if 0

   IMENUCTL_SetStyle(pm, NULL, NULL);
   IMENUCTL_Redraw (pm);
}

/*=============================================================================
FUNCTION:  GetCheckBoxVal

DESCRIPTION: Gets Check Box Items on/off

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
#if 0
   CtlAddItem cai;

   IMENUCTL_GetItem(pm, listId, &cai);
   if(cai.dwData == 1)
   {
      return TRUE;
   }

   return FALSE;
#else
   uint16 wID;
   if(!pm)
   {
      return FALSE;
   }

   IMENUCTL_EnumSelInit(pm);
   while(1)
   {
      wID =  IMENUCTL_EnumNextSel(pm);
      if((uint16)-1 !=wID)
      {
         if(wID == listId)
            return TRUE;
      }
      else
      {
         return FALSE;
      }
   }
#endif//if 0
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

DESCRIPTION:  Searches the Contacts database for a name match for the
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
#if MIN_BREW_VERSION(3,1)
   AEEAddrEnumRecInfo   searchInfo;
   
   MEMSET(&searchInfo, 0, sizeof(AEEAddrEnumRecInfo));

#endif

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

#if MIN_BREW_VERSION(3,1)
   searchInfo.pData = number;
   if (NULL == searchInfo.pData) {
      return;
   }
   searchInfo.dwDataSize = (uint16)WSTRSIZE(searchInfo.pData);

   searchInfo.pCatList = NULL;
   searchInfo.nCategories = 0;
   searchInfo.pFieldList = (AEEAddrFieldID *) MALLOC(sizeof(AEEAddrFieldID) * 9);
   searchInfo.pFieldList[0] = AEE_ADDRFIELD_PHONE_WORK;
   searchInfo.pFieldList[1] = AEE_ADDRFIELD_PHONE_HOME;
   searchInfo.pFieldList[2] = AEE_ADDRFIELD_PHONE_FAX;
   searchInfo.pFieldList[3] = AEE_ADDRFIELD_PHONE_VOICE;
   searchInfo.pFieldList[4] = AEE_ADDRFIELD_PHONE_PREF;
   searchInfo.pFieldList[5] = AEE_ADDRFIELD_PHONE_CELL;
   searchInfo.pFieldList[6] = AEE_ADDRFIELD_PHONE_PAGER;
   searchInfo.pFieldList[7] = AEE_ADDRFIELD_PHONE_GENERIC;
   searchInfo.pFieldList[8] = AEE_ADDRFIELD_PHONE_OTHER;
   searchInfo.nFields = 9;
   searchInfo.flags = ABSF_MATCH_EQUAL;
#endif

   //Start searching the address book with the search string
#if MIN_BREW_VERSION(3,1)
   if (SUCCESS == IADDRBOOK_EnumRecInitEx(pa, &searchInfo)) {
#else
   if (SUCCESS == IADDRBOOK_EnumRecInitEx(pa,
                                        AEE_ADDR_CAT_NONE,
                                        AEE_ADDRFIELD_NONE,
                                        number,
                                        (uint16) WSTRSIZE(number),
                                        ADDRREC_SEARCHTYPE_CONTAINS |
                                        ADDRREC_FIELDID_PHONE)) {

#endif

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

               WSTRNCOPYN(name,
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
#if !defined( AEE_SIMULATOR)
               ASSERT(addrField->pBuffer != NULL);
#endif
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
                  WSTRNCOPYN(number,
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


#if !defined( AEE_SIMULATOR)

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
#if !MIN_BREW_VERSION(3,1)
   uint32    flags = 0x0;
#endif

   if (wUtt == AEEVRUTTERANCE_NONE) {
      return FALSE;
   }

   if (NULL == pa) {
      return FALSE;
   }

#if !MIN_BREW_VERSION(3,1)
   flags = ADDRREC_DATATYPE_BYTE;

   if (SUCCESS != IADDRBOOK_EnumRecInitEx(pa,
                                        AEE_ADDR_CAT_SPEEDDIAL,
                                        AEE_ADDRFIELD_VOICETAG,
                                        &wUtt,
                                        sizeof(AEEVRUtterance),
                                        flags)) {
#else
   if (SUCCESS != IADDRBOOK_EnumRecInit(pa,
                                        AEE_ADDR_CAT_SPEEDDIAL,
                                        AEE_ADDRFIELD_VOICETAG,
                                        &wUtt,
                                        sizeof(AEEVRUtterance))) {
#endif
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
                  WSTRNCOPYN(szName,
                          (int)(nameSize / sizeof(AECHAR)),
                          addrField->pBuffer,
                          -1);
                  gotName = TRUE;
              }

              if (addrField->fType == AEEDB_FT_PHONE) {
                  WSTRNCOPYN(szNum,
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
#if MIN_BREW_VERSION(3,1)
   AEEAddrEnumRecInfo   searchInfo;

   MEMSET(&searchInfo, 0, sizeof(AEEAddrEnumRecInfo));
#endif

   if (NULL == pa || szNum == NULL) {
      return FALSE;
   }

#if MIN_BREW_VERSION(3,1)
   searchInfo.pData = szNum;
   if (NULL == searchInfo.pData) {
      return FALSE;
   }
   searchInfo.dwDataSize = (uint16)WSTRSIZE(searchInfo.pData);

   searchInfo.pCatList = (AEEAddrCat *) MALLOC(sizeof(AEEAddrCat));
   searchInfo.nCategories = 1;
   *(searchInfo.pCatList) = AEE_ADDR_CAT_SPEEDDIAL;
   searchInfo.pFieldList = (AEEAddrFieldID *) MALLOC(sizeof(AEEAddrFieldID) * 9);
   searchInfo.pFieldList[0] = AEE_ADDRFIELD_PHONE_WORK;
   searchInfo.pFieldList[1] = AEE_ADDRFIELD_PHONE_HOME;
   searchInfo.pFieldList[2] = AEE_ADDRFIELD_PHONE_FAX;
   searchInfo.pFieldList[3] = AEE_ADDRFIELD_PHONE_VOICE;
   searchInfo.pFieldList[4] = AEE_ADDRFIELD_PHONE_PREF;
   searchInfo.pFieldList[5] = AEE_ADDRFIELD_PHONE_CELL;
   searchInfo.pFieldList[6] = AEE_ADDRFIELD_PHONE_PAGER;
   searchInfo.pFieldList[7] = AEE_ADDRFIELD_PHONE_GENERIC;
   searchInfo.pFieldList[8] = AEE_ADDRFIELD_PHONE_OTHER;
   searchInfo.nFields = 9;
   searchInfo.flags = ABSF_MATCH_CONTAINS;

   if (SUCCESS != IADDRBOOK_EnumRecInitEx(pa, &searchInfo)) {
#else
   if (SUCCESS != IADDRBOOK_EnumRecInitEx(pa,
                                        AEE_ADDR_CAT_SPEEDDIAL,
                                        AEE_ADDRFIELD_NONE,
                                        szNum,
                                        (uint16) WSTRSIZE(szNum),
                                        ADDRREC_SEARCHTYPE_CONTAINS |
                                        ADDRREC_FIELDID_PHONE)) {
#endif
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
                  WSTRNCOPYN(szName,
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
  uint16       maxUtt = listsize/sizeof(AEEVRUtterance);
#if MIN_BREW_VERSION(3,1)
  AEEAddrEnumRecInfo   searchInfo;
   
  MEMSET(&searchInfo, 0, sizeof(AEEAddrEnumRecInfo));
#endif


  if (list == NULL || listsize == 0)
    return 0;

#if MIN_BREW_VERSION(3,1)
   searchInfo.pData = NULL;
   searchInfo.dwDataSize = 0;

   searchInfo.pCatList = (AEEAddrCat *) MALLOC(sizeof(AEEAddrCat));
   searchInfo.nCategories = 1;
   *(searchInfo.pCatList) = AEE_ADDR_CAT_SPEEDDIAL;
   searchInfo.pFieldList = (AEEAddrFieldID *) MALLOC(sizeof(AEEAddrFieldID) * 1);
   searchInfo.pFieldList[0] = AEE_ADDRFIELD_VOICETAG;
   searchInfo.nFields = 1;
   searchInfo.flags = 0;


  if (SUCCESS != IADDRBOOK_EnumRecInitEx(pa, &searchInfo)) {
#else
  if (SUCCESS != IADDRBOOK_EnumRecInitEx(pa,
                                          AEE_ADDR_CAT_SPEEDDIAL,
                                          AEE_ADDRFIELD_VOICETAG,
                                          NULL,
                                          0,
                                          0)) {
#endif
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
  IShell* pIShell=NULL;
  int len;
  AECHAR *pBuf=NULL;

  pIShell = AEE_GetShell();

  if (pIShell == NULL) {
    return EFAILED;
  }

  len = STRLEN(number);

  if (len == 0) {
    return EFAILED;
  }
  
  /* The Buffer allocated here will be freed in CoreApp_HandleEvent while */
  /* handling EVT_ORIGINATE_CALL */
  pBuf = MALLOC (sizeof(AECHAR)*(len+1));
  if (pBuf == NULL) {
    return EFAILED;
  }

  STRTOWSTR (number, pBuf, sizeof(AECHAR)*(len+1));

  /* Post event to coreapp */
  ISHELL_PostEventEx (pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                     EVT_USER, EVT_ORIGINATE_CALL, (uint32) pBuf);
  return SUCCESS;

}

#endif //if !defined( AEE_SIMULATOR)

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
        TabController_ProcessKeyRelease(*pDlgTabCtl);
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
#ifdef FEATURE_ICM
boolean IsCallActive (AEECMCallType calltype)
#else
boolean IsCallActive (AEETCallType calltype)
#endif
{  
  IShell *pIShell=NULL;
  int nCalls = 0;
  pIShell = AEE_GetShell();
  if (!pIShell)
    return FALSE;
//#ifndef FEATURE_MANGO_BREW
#ifndef FEATURE_USES_LOWMEM
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
