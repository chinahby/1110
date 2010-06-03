/*===========================================================================
  FILE: ContDlgHandler.c

  SERVICES: Contact Applet Dialog Handlers and Database manipulation
            functions

  GENERAL DESCRIPTION:
      This file implements all contact applet dialog handlers which are used
      by the IContactApp to bring up various Contact Applet screens
      to the user

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright © 2003-2008 QUALCOMM Incorporated. All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=============================================================================
  $Header: //depot/asic/msmshared/apps/Contact/ContDlgHandler.c#58 $

                    EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------
03/26/08     cvs     Remove deprecated functions
05/24/07     cvs     Only sort the contact list when it changes.
09/18/06     cvs     Integrate brew 4.0 changes from BTG.
07/27/06     slw     Fix menu items displayed when adding FDNs & removed 
                     "addnew" from FDN and speed dial
07/12/06     JAS     Compiler warning cleanup for RVCT
06/06/06     aps     Fix for '+' prefix added back to a previously
                     international number, even after it was edited
                     out to make it a local one.
05/19/06     aps     Fixed wrong handling of fType in address book
                     records in ContDlgHandler_GetFirstNumberType
08/10/05     JAS     Fix handling of Clear key in name entry dialog
08/10/05     AT      Added a new text property to validate the phone number
                     on input to ensure it contains valid dialed digits.
                     Fixed bugs related to writing the GSM_TONNPI.
07/26/05     AT      Fixed issue with Speed Dial, improved name finding
                     efficiency.
07/23/05     BW      Fix compile error with using a variable (ret) that was not 
                     declared.
07/14/05     aps     Support to handle contacts with name field empty.
06/22/05     AT      Turning FONT_MORPHING and AUTO-HYPENTATION back on.
06/10/05     RI      Fixed freeing of buffer SS is stored.
05/31/05     RI      Set TON/NPI when SS strings are stored for GSM/UMTS.
04/19/05     JAS     Replace sizing info with display bmp size
02/17/05     SUN     Fixed Link Errors
01/18/05     BW      Upgraded to contactapp.brx file from the bri version
01/17/05     BW      Added USIM support - ability to add and view UIM fields
01/12/05     jas     Removing ASSERTs
12/13/04     ssc     Changed include file name case to match the Linux names 
                     of the files. Mismatches in the case break AMSS builds 
                     on Linux servers.
12/10/04     AT      Allowed # key to terminate the PIN code
12/02/04     SUN     Fixed Compile Error
11/10/04     BW      Added support for SDN
10/05/04     AT      Changed OEM_TM_DIALED_DIGITS to AEE_TM_DIALED_DIGITS
                     on phones with Brew 3.1.
09/25/04     AT      Added Search for existing name to list that gets all
                     contacts.
08/27/04     NG      Removed assert and replaced it with closing dialog.
08/20/04     JG      IADDRBOOK_EnumRecInitEx() is defined in BREW 3.1.
                     Rename the QCT IADDRBOOK_EnumRecInitEx() to
                     IADDRBOOKEXT_EnumRecInitEx
08/09/04     PA      Fix assertion in ContDlgHandler_LoadContactList
07/27/04     PA      Fix compilation error/warning.
07/22/04     PA      Do not display FDN in View All
                     Added new menu for saving exsting contact to UIM2
06/23/04     PA      Support for enumeration of available contacts
06/07/04     AT      BREW 2.1.3 changes what the dwparam is for EVT_COMMANDS.
                     Change the SW not to use the dwparam as we will not be
                     able to use the value it returns.
04/07/04     PA      Check maximum number of fields allowed for a contact and
                     select an alternate number type if required.
                     Fixed Mutitap issues.
03/11/04     PA      If SIM 1 is not present, do not show relevant menus
1/21/04      AT      Featurized the Changes from December so that this
                     can build in all targets.
1/19/04      AT      Added error message when UIM not ready.  Fixed problem
                     saving to UIM from non main menu saving options.
12/16/03     BPW     Added Brew 3.0 and UK field changes
11/21/03     ak      Rolled back IADDRBOOK_EnumRecInit change. IADDRBOOK_EnumRecInitEx()
                     is now defined in OEMAddrbook.h of BREW 3.0
11/19/03     ak      IADDRBOOK_EnumRecInit() instead of IADDRBOOK_EnumRecInitEx()
                     in BREW 3.0 builds.
11/18/03     AT      Changed the field type for VR to DWORD.
09/10/03     adm     Added support for SIM2 (ADN and FDN)
08/12/03     jds     Changed AEE_TM_NUMBERS to OEM_TM_DIALED_DIGITS on phone
                     number edit screen to allow '*' and '#' characters
08/07/03     jds     Added support for editing FDN.
06/03/03     PA      Changed FEATURE_RUIM_PHONEBOOK to FEATURE_MMGSDI
05/15/03     PA      Speed dial index is now part of ContactInfo and removed
                     logic that stores it as part of name string.
                     Support for creating/retraining voice tags from speed
                     dial list.
05/09/03     PA      Return TRUE while closing dialog on fail
05/08/03     PA      Support for Voice Tags
04/17/03     PA/AT   Use category in addition to field type to determine
                     number types of GENERIC and OTHER fields.
                     Added NUMBERTYPE_OTHER for Softkey UI support.
03/26/03     pa      Enable up and down arrows to switch between text and
                     softkey controls. After "Pause", selection now goes back
                     to text control.
03/05/03     pa      Fix to set correct input mode after setting properties
02/23/03     kar     Fixes to get around textctl issues
02/06/03     kar     Code cleanup
11/15/02     pm      Port Contact App to QCT
=============================================================================*/
//lint -save -e611 -e740  Suppress unusual/suspicious cast
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


#include "ContApp.h"           // Main Applet Header

#include "contactapp.brh"        // Applet Resource Header

#include "AEEStdLib.h"        // AEE StdLib Services
#include "AEE_OEM.h"
#include "OEMText.h"
#include "AppComFunc.h"
#include "uiutils.h"

/*===========================================================================
                           DEFINITIONS AND CONSTANTS
===========================================================================*/
// Time (in ms) before a MsgBox will close automatically
#define MSGBOX_TIMEOUT 2000

// Max number of characters allowed in a MsgBox title
#define MSGBOX_MAXTITLELEN 64

// Max number of characters allowed in the MsgBox text
#define MSGBOX_MAXTEXTLEN  256

// Time (in ms) before a YesNo Box will close automatically
#define YESNO_TIMEOUT 6000

// Time (in ms) before a speed dial menu jumps to m_indexSpeedDial key location
#define SPEEDDIAL_MENUKEY_TIMEOUT  500

//Used in CContactApp_ShowNextNumberType to iterate the fields
//m_fldrecs_iv in forward direction or backward direction
#define FORWARD_DIRECTION   1
#define BACKWARD_DIRECTION  2

/*===========================================================================
                               MACRO DEFINITIONS
===========================================================================*/
// Macro used at lot of places to close the dialog (after setting the
// dialog result).
#define CLOSE_DIALOG(dlgResult)  {                                           \
                                    pMe->m_dlgResult = dlgResult;            \
                                    (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }

/*===========================================================================
                              TYPE DECLARATIONs
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/
//Dialog helper functions
static void    ContDlgHandler_DialogTimeout_Callback(void *pUser);

#ifdef FEATURE_PHONE_VR
static void   ContDlgHandler_VoiceTag_Callback(void *pCContactApp,
                                             AEEVRStatus status,
                                             AEEVRFailure failure,
                                             AEEVRUtterance wUtt);
#endif

static boolean ContDlgHandler_ShowNextNumberType(CContactApp *pMe,
                                                 IMenuCtl    *pCtrl,
                                                 uint16       wDirection);

static boolean ContDlgHandler_ShowFirstNumberType(CContactApp *pMe,
                                                  IMenuCtl    *pCtrl);

static boolean ContDlgHandler_GetFirstNumberType(CContactApp *pMe,
                                                 ContactInfo *ci,
                                                 IAddrRec    *addrRec);

static void ContDlgHandler_PopulateContactListMenu(CContactApp *pMe,
                                                   IMenuCtl    *ctrl,
                                                   uint16       startIdx,
                                                   uint16       count,
                                                   boolean      clearMenu);

static void ContDlgHandler_PopulateSpeedDialListMenu(CContactApp *pMe,
                                                     IMenuCtl    *ctrl,
                                                     uint16       startIdx,
                                                     uint16       count,
                                                     boolean      clearMenu);

static void ContDlgHandler_SetMenuStyle(IShell *shell, IMenuCtl *menu);

static void ContDlgHandler_SpeedDialMenuKeyTimer(void *pUser);


#ifdef FEATURE_CONTACTNAME_PREDICT
static boolean ContDlgHandler_IsPredictMatch(AECHAR ch, AVKType key);

static boolean ContDlgHandler_DoContactNamePredict(CContactApp *pMe,
                                                   uint16      *idx,
                                                   boolean      bFromCurPos);

#endif /* FEATURE_CONTACTNAME_PREDICT */

////
// Database manipulation methods
// Local
static boolean ContDlgHandler_FindCDBRec(CContactApp *pMe);

static boolean ContDlgHandler_ContactInfo2AEEAddrField(CContactApp *pMe,
                                                       AEEAddrField    *cf,
                                                       boolean    mallocData);
static boolean ContDlgHandler_FieldInfo2AEEAddrField(FieldInfo   *fi,
                                                     AEEAddrField    *cf,
                                                     boolean      mallocData);

static boolean ContDlgHandler_UpdateAEEAddrFields(CContactApp *pMe);

//Miscellanous
static boolean ContDlgHandler_WStrStr(AECHAR *haystack,
                                      AECHAR *needle);
////
// Dialog event handlers
////
static boolean ContDlgHandler_MainDialogEvent(CContactApp *pMe,
                                              AEEEvent     eCode,
                                              uint16       wParam,
                                              uint32       dwParam);

static boolean ContDlgHandler_FindNameDialogEvent(CContactApp *pMe,
                                                  AEEEvent     eCode,
                                                  uint16       wParam,
                                                  uint32       dwParam);

static boolean ContDlgHandler_ContactListDialogEvent(CContactApp *pMe,
                                                     AEEEvent     eCode,
                                                     uint16       wParam,
                                                     uint32       dwParam);

static boolean ContDlgHandler_AddNewDialogEvent(CContactApp *pMe,
                                                AEEEvent     eCode,
                                                uint16       wParam,
                                                uint32       dwParam);

static boolean ContDlgHandler_AddNewRuimDialogEvent(CContactApp *pMe,
                                                AEEEvent     eCode,
                                                uint16       wParam,
                                                uint32       dwParam);

static boolean ContDlgHandler_FieldInputDialogEvent(CContactApp *pMe,
                                                    AEEEvent     eCode,
                                                    uint16       wParam,
                                                    uint32       dwParam);

static boolean ContDlgHandler_NumberTypeDialogEvent(CContactApp *pMe,
                                                    AEEEvent     eCode,
                                                    uint16       wParam,
                                                    uint32       dwParam);

static boolean ContDlgHandler_ContactNameDialogEvent(CContactApp *pMe,
                                                     AEEEvent     eCode,
                                                     uint16       wParam,
                                                     uint32       dwParam);

static boolean ContDlgHandler_ContactClassDialogEvent(CContactApp *pMe,
                                                      AEEEvent     eCode,
                                                      uint16       wParam,
                                                      uint32       dwParam);

static boolean ContDlgHandler_ContactDialogEvent(CContactApp *pMe,
                                                 AEEEvent     eCode,
                                                 uint16       wParam,
                                                 uint32       dwParam);

static boolean ContDlgHandler_ContactOptionsDialogEvent(CContactApp *pMe,
                                                        AEEEvent     eCode,
                                                        uint16       wParam,
                                                        uint32       dwParam);

static boolean ContDlgHandler_FieldOptionsDialogEvent(CContactApp *pMe,
                                                      AEEEvent     eCode,
                                                      uint16       wParam,
                                                      uint32       dwParam);

static boolean ContDlgHandler_YesNoDialogEvent(CContactApp *pMe,
                                               AEEEvent     eCode,
                                               uint16       wParam,
                                               uint32       dwParam);

static boolean ContDlgHandler_MsgBoxDialogEvent(CContactApp *pMe,
                                                AEEEvent     eCode,
                                                uint16       wParam,
                                                uint32       dwParam);

static boolean ContDlgHandler_ViewAddressDialogEvent(CContactApp *pMe,
                                                     AEEEvent     eCode,
                                                     uint16       wParam,
                                                     uint32       dwParam);

static boolean ContDlgHandler_RUIMDialogEvent(CContactApp *pMe,
                                              AEEEvent     eCode,
                                              uint16       wParam,
                                              uint32       dwParam);

static boolean ContDlgHandler_SpeedDialDialogEvent(CContactApp *pMe,
                                                   AEEEvent     eCode,
                                                   uint16       wParam,
                                                   uint32       dwParam);

static boolean ContDlgHandler_VoiceTagDialogEvent(CContactApp *pMe,
                                                   AEEEvent     eCode,
                                                   uint16       wParam,
                                                   uint32       dwParam);

static boolean ContDlgHandler_PIN2DialogEvent(CContactApp *pMe,
                                              AEEEvent     eCode,
                                              uint16       wParam,
                                              uint32       dwParam);

/*===========================================================================
                              GLOBAL DATA
===========================================================================*/
/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

////
// List of dialog handlers indexed by dialog resource ID
//
// NOTE!  Dialog resource IDs are assumed to start at 1 and
//        must not have gaps between successive IDs (ie. 1,2,4,5).
//
static CONT_DIALOG_EVENTHANDLER gDialogHandlers[] =
{
/* DLG_MAIN */           ContDlgHandler_MainDialogEvent,
/* DLG_FINDNAME */       ContDlgHandler_FindNameDialogEvent,
/* DLG_CONTACTLIST */    ContDlgHandler_ContactListDialogEvent,
/* DLG_ADDNEW */         ContDlgHandler_AddNewDialogEvent,
/* DLG_N_FIELDINPUT */   ContDlgHandler_FieldInputDialogEvent,
/* DLG_NUMBERTYPE */     ContDlgHandler_NumberTypeDialogEvent,
/* DLG_CONTACTNAME */    ContDlgHandler_ContactNameDialogEvent,
/* DLG_CONTACTCLASS */   ContDlgHandler_ContactClassDialogEvent,
/* DLG_CONTACT */        ContDlgHandler_ContactDialogEvent,
/* DLG_CONTACTOPTIONS */ ContDlgHandler_ContactOptionsDialogEvent,
/* DLG_FIELDOPTIONS */   ContDlgHandler_FieldOptionsDialogEvent,
/* DLG_YESNO */          ContDlgHandler_YesNoDialogEvent,
/* DLG_MSGBOX */         ContDlgHandler_MsgBoxDialogEvent,
/* DLG_NEWCONTACTNAME */ ContDlgHandler_ContactNameDialogEvent,
/* DLG_A_FIELDINPUT */   ContDlgHandler_FieldInputDialogEvent,
/* DLG_E_FIELDINPUT */   ContDlgHandler_FieldInputDialogEvent,
/* DLG_U_FIELDINPUT */   ContDlgHandler_FieldInputDialogEvent,
/* DLG_VIEWADDRESS  */   ContDlgHandler_ViewAddressDialogEvent,
/* DLG_RUIM */           ContDlgHandler_RUIMDialogEvent,
/* DLG_SPEEDDIAL*/       ContDlgHandler_SpeedDialDialogEvent,
/* DLG_VOICETAG */       ContDlgHandler_VoiceTagDialogEvent,
/* DLG_PIN2 */           ContDlgHandler_PIN2DialogEvent,
/* DLG_ADDNEW_RUIM */    ContDlgHandler_AddNewRuimDialogEvent,
/* DLG_G_FIELDINPUT */   ContDlgHandler_FieldInputDialogEvent,
/* DLG_SN_FIELDINPUT */  ContDlgHandler_FieldInputDialogEvent,
};

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/
/*=============================================================================
FUNCTION: ContDlgHandler_UpdateActiveDialogInfo

DESCRIPTION: Updates the key dialog parameters in the SMSApp object.

PARAMETERS:
   pi [in]: Pointer to the Contact Applet structure. This structure contains
            information specific to this applet.

   wParam, dwParam [in]: Active dialog information

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:
   The updated contact applet dialog attributes are later used to route the
   dialog events to thier respective handlers.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ContDlgHandler_UpdateActiveDialogInfo(CContactApp       *pMe,
                                           uint16             wParam,
                                           uint32             dwParam)
{
   if (pMe) {
      if (
         (wParam==0) ||
         (wParam > sizeof(gDialogHandlers)/sizeof(gDialogHandlers[0]))
         ) {
         // Hmmm! our range check failed. Return right away.
         return;
      }

      pMe->m_activeDlg = (IDialog*)dwParam;
      pMe->m_activeDlgHandleEvent = gDialogHandlers[wParam-1];
   }
}

/*=============================================================================
FUNCTION: ContDlgHandler_ShowDialog

DESCRIPTION:
   Creates the dialog with dlgResID

PARAMETERS:
   *pMe: CContactApp Object pointer
   dlgResId: ID of the dialog to be shown
   retState: return State. SUCCESS if successful else EFAILED/ENOMEMORY

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ContDlgHandler_ShowDialog(CContactApp         *pMe,
                               uint16               dlgResId,
                               ContactAppState      retState)
{
   int iRet;

   pMe->m_dlgRetState = retState;
   pMe->m_dlgResult = DLGRET_CONT_UNHANDLED;

   // at most one dialog open at once
   if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
   {
      MSG_MED("Currently active dialog = %d", dlgResId, 0, 0);
      ISHELL_EndDialog(pMe->m_pShell);
   }

   iRet = ISHELL_CreateDialog(pMe->m_pShell,
                              CONTACTAPP_RES_FILE,
                              dlgResId,
                              NULL);

   if (iRet != SUCCESS) {
      pMe->m_nextState = STATE_CONT_ERROR_DLGOPEN;
   }
}

/*=============================================================================
FUNCTION: ContDlgHandler_ShowMsgBox

DESCRIPTION:
    Shows the message in the messagebox.

PARAMETERS:
   *pMe: CContactApp Object pointer
   msgTextResId: String Resource ID of the string to be displayed
   retState: return State. SUCCESS if successful else EFAILED/ENOMEMORY

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ContDlgHandler_ShowMsgBox(CContactApp         *pMe,
                               uint16               msgTextResId,
                               ContactAppState retState)
{
   pMe->m_msgBox_textId = msgTextResId;

   ContDlgHandler_ShowDialog(pMe, DLG_MSGBOX, retState);
}


/*=============================================================================
FUNCTION: ContDlgHandler_RouteDialogEvent

DESCRIPTION: Route a BREW event to the active dialog handler

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ContDlgHandler_RouteDialogEvent(CContactApp *pMe,
                                        AEEEvent     eCode,
                                        uint16       wParam,
                                        uint32       dwParam)
{
   if ( (NULL == pMe->m_activeDlg) ||
        (NULL == pMe->m_activeDlgHandleEvent) ) {
      return FALSE;
   }

   return pMe->m_activeDlgHandleEvent(pMe,
                                      eCode,
                                      wParam,
                                      dwParam);
} // ContDlgHandler_RouteDialogEvent()

/*=============================================================================
FUNCTION: ContDlgHandler_DialogTimeout_Callback

DESCRIPTION:
   Callback function passed to the BREW layer so that whenever the timer that
   we have set in the dialog handler expires, we can post a timeout event to the
   dialog.

PARAMETERS:
   *pUser: pUser is assumed to be a CContactApp pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void ContDlgHandler_DialogTimeout_Callback(void *pUser)
{
   CContactApp *pMe = (CContactApp*)pUser;

   // Post a message to the dialog telling it to close
   (void) ISHELL_PostEvent(pMe->m_pShell,
                           AEECLSID_CONTACT_APP,
                           EVT_USER,
                           CONT_EVENT_DIALOG_TIMEOUT,
                           0);
}


/*=============================================================================
FUNCTION: ContDlgHandler_ShowNextNumberType

DESCRIPTION:
    This function  first finds the next number type in the m_fldrecs_iv of the
    current contact info. If it reaches end/beginging, it scrolls over to
    end/begining to search for the next number type. Then it updates the icon
    in the menu control to reflect the new number type

PARAMETERS:
   *pMe: contacts applet
   *ci: contact info


RETURN VALUE:
    TRUE if successful
    FALSE if not successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_ShowNextNumberType(CContactApp *pMe,
                                                 IMenuCtl    *pCtrl,
                                                 uint16       wDirection)
{
   int           wTotalFields = 0;
   int           nIndex = 0;
   AEEAddrField *addrField = NULL;
   boolean       retVal   = FALSE;
   ContactInfo  *ci = NULL;


   if ((wDirection != FORWARD_DIRECTION) && (wDirection!=BACKWARD_DIRECTION)) {
      return FALSE;
   }
   wTotalFields = (int)IVector_Size(pMe->m_fldrecs_iv);
   if (wTotalFields <= 0) {
      return FALSE;
   }

   nIndex = pMe->m_contactListFieldID;

   for ( ; ;) {
      if (wDirection == FORWARD_DIRECTION) {
         nIndex++;
         //index is 0 based
         if (nIndex > wTotalFields-1) {
            nIndex = 0;
         }
      } else if (wDirection == BACKWARD_DIRECTION) {
         nIndex--;
         //index is 0 based
         if (nIndex < 0) {
            nIndex = wTotalFields-1 ;
         }
      }
      //check if we did one whole cycle and did not find any
      //other phone type
      if (nIndex == pMe->m_contactListFieldID) {
         retVal = FALSE;
         break;
      }

      ci = &pMe->m_contactInfo;

      addrField = IVector_ElementAt(pMe->m_fldrecs_iv,
                                    (uint16)nIndex);
      if (addrField == NULL) {
         return FALSE;
      }
      if (addrField->fType != AEEDB_FT_PHONE) {
         continue;
      } else {

         CtlAddItem pai;
         (void)MEMSET(&pai, 0, sizeof(pai));

         switch (addrField->fID) {
            case AEE_ADDRFIELD_PHONE_WORK:
               pai.wImage = ICON_WORKNUMBER;
               break;

            case AEE_ADDRFIELD_PHONE_CELL:
               pai.wImage = ICON_MOBILENUMBER;
               break;
            case AEE_ADDRFIELD_PHONE_PAGER:
               pai.wImage = ICON_PAGERNUMBER;
               break;
               //If it is on RUIM
            case AEE_ADDRFIELD_PHONE_GENERIC:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
              switch(ci->m_class)
              {
                case AEE_ADDR_CAT_SIM_ADN:
                case AEE_ADDR_CAT_SIM_FDN:
                  pai.wImage = ICON_RUIM;
                  break;
                case AEE_ADDR_CAT_SIM_ADN2:
                case AEE_ADDR_CAT_SIM_FDN2:
                  pai.wImage = ICON_RUIM2;
                  break;
                case AEE_ADDR_CAT_SIM_SDN:
                  pai.wImage = ICON_CONTACTCLASS_SDN;
                  break;
                case AEE_ADDR_CAT_SIM_SDN2:
                  pai.wImage = ICON_CONTACTCLASS_SDN2;
                  break;
                default:
                  break;
              }
              break;
#else
              switch(ci->m_class)
              {
                case AEE_ADDR_CAT_SIM_SDN:
                  pai.wImage = ICON_CONTACTCLASS_SDN;
                  break;
                default:
                  pai.wImage = ICON_RUIM;
                  break;
              }
              break;
#endif

            case AEE_ADDRFIELD_PHONE_HOME:
               pai.wImage = ICON_HOMENUMBER;
               break;
            case AEE_ADDRFIELD_PHONE_OTHER:
               pai.wImage = 0;
               break;
            default:
               MSG_ERROR("Unrecognized address field, %d.", addrField->fID, 0, 0);
               continue;
         }
         pai.pszResImage =  CONTACTAPP_RES_FILE;

         if (IMENUCTL_SetItem(pCtrl,
                              pMe->m_contactSelId,
                              MSIF_IMAGE,
                              &pai)==FALSE) {
            retVal = FALSE;
            break;
         }

         (void)IMENUCTL_Redraw(pCtrl);

         pMe->m_contactListFieldID = (uint16)nIndex;
         retVal = TRUE;
         break;
      }
   }
   return retVal;
}

/*=============================================================================
FUNCTION: ContDlgHandler_ShowFirstNumberType

DESCRIPTION:
    This function  gets the first number type for the current contact and show
    the icon

PARAMETERS:
   *pMe: contacts applet
   *ci: contact info

RETURN VALUE:
    TRUE if successful
    FALSE if not successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_ShowFirstNumberType(CContactApp *pMe,
                                                  IMenuCtl    *pCtrl)
{
   boolean      retVal   = FALSE;
   CtlAddItem   pai;
   ContactInfo *ci;

   if ((! pMe) || (! pCtrl))
   {
      MSG_ERROR("Unexpected NULL ptr.", 0, 0, 0);
      return FALSE;
   }

   ci = IVector_ElementAt(pMe->m_contact_iv,
                          pMe->m_contactSelId);

   if (! ci)
   {
      MSG_ERROR("NULL contact info at index %d.", pMe->m_contactSelId,
                0, 0);
      return FALSE;
   }

   //Check all the fields and find the first field which is a phone number
   //of any type
   if (ContDlgHandler_GetFirstNumberType(pMe,ci,NULL)==FALSE) {
      return FALSE;
   }

   (void)MEMSET(&pai, 0, sizeof(pai));

   switch (ci->m_fldNumberType) {
      case NUMBERTYPE_MENUITEM_WORK:
         pai.wImage = ICON_WORKNUMBER;
         break;

      case NUMBERTYPE_MENUITEM_MOBILE:
         pai.wImage = ICON_MOBILENUMBER;
         break;
         //If it is on RUIM
      case NUMBERTYPE_RUIM:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
         switch(ci->m_class)
         {
          case AEE_ADDR_CAT_SIM_ADN:
          case AEE_ADDR_CAT_SIM_FDN:
            pai.wImage = ICON_RUIM;
            break;
          case AEE_ADDR_CAT_SIM_ADN2:
          case AEE_ADDR_CAT_SIM_FDN2:
            pai.wImage = ICON_RUIM2;
            break;
          case AEE_ADDR_CAT_SIM_SDN:
            pai.wImage = ICON_CONTACTCLASS_SDN;
            break;
          case AEE_ADDR_CAT_SIM_SDN2:
            pai.wImage = ICON_CONTACTCLASS_SDN2;
            break;
          default:
            break;
         }
         break;
#else
         switch (ci->m_class)
         {
           case AEE_ADDR_CAT_SIM_SDN:
             pai.wImage = ICON_CONTACTCLASS_SDN;
             break;
           default:
             pai.wImage = ICON_RUIM;
             break;
         }
         break;
#endif
      case NUMBERTYPE_SPEEDDIAL:
         pai.wImage = ICON_SPEEDDIAL;
         break;

      case NUMBERTYPE_VOICETAG:
         pai.wImage = ICON_VOICETAG;
         break;

      case NUMBERTYPE_OTHER:
         pai.wImage = ICON_NUMBER;
         break;

      case NUMBERTYPE_MENUITEM_HOME:
         pai.wImage = ICON_HOMENUMBER;
         break;

      case NUMBERTYPE_MENUITEM_PAGER:
         pai.wImage = ICON_PAGERNUMBER;
         break;

      default:
         pai.wImage = ICON_NOPHONE;
         break;
   }
   pai.pszResImage =  CONTACTAPP_RES_FILE;

   if (IMENUCTL_SetItem(pCtrl,
                        pMe->m_contactSelId,
                        MSIF_IMAGE,
                        &pai)==FALSE) {
      retVal = FALSE;
   } else {
      (void)IMENUCTL_Redraw(pCtrl);
      retVal = TRUE;
   }
   return retVal;
}

/*=============================================================================
FUNCTION: ContDlgHandler_GetFirstNumberType

DESCRIPTION:
    This function finds the first phone number type of the contact from
    the record ID in the contact info and copies in m_fldNumberType member of ci

PARAMETERS:
   *pMe: contacts applet
   *ci: contact info
*addrRec: If non-NULL, use this IAddrRec interface instead of using
          ci->m_id to load a new one

RETURN VALUE:
    TRUE if successful
    FALSE if no phone number was found was this record id

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_GetFirstNumberType(CContactApp *pMe,
                                                 ContactInfo *ci,
                                                 IAddrRec    *addrRec)
{
   int           wTotalFields;
   uint16        j;
   AEEAddrField *addrField = NULL;
   boolean       bRelease = FALSE;
   AEEAddrCat    cat;
   boolean       hasVoiceTag = FALSE;
   boolean       gotPhone = FALSE;

   if (NULL == addrRec) {
      bRelease = TRUE;
      addrRec = IADDRBOOK_GetRecByID(pMe->m_addr_book,
                                     ci->m_id);
   }

   if (addrRec==NULL) {
      return FALSE;
   }

   cat = IADDRREC_GetCategory(addrRec);

   //Go throught all the fields and find the first field which is a
   //phone number type
   wTotalFields = IADDRREC_GetFieldCount(addrRec);
   for (j=0;j< wTotalFields;j++) {
      addrField = IADDRREC_GetField(addrRec,j);
      if (addrField) {
         if ((addrField->fType == AEEDB_FT_PHONE) && (gotPhone == FALSE)) {
            gotPhone = TRUE;
            ci->m_fldNumberId = (uint16)j;
            pMe->m_contactListFieldID = (uint16)j;

            switch (addrField->fID) {
               case AEE_ADDRFIELD_PHONE_WORK:
                  ci->m_fldNumberType = NUMBERTYPE_MENUITEM_WORK;
                  break;
               case AEE_ADDRFIELD_PHONE_CELL:
                  ci->m_fldNumberType = NUMBERTYPE_MENUITEM_MOBILE;
                  break;
               case AEE_ADDRFIELD_PHONE_PAGER:
                  ci->m_fldNumberType = NUMBERTYPE_MENUITEM_PAGER;
                  break;
               case AEE_ADDRFIELD_PHONE_HOME:
                  ci->m_fldNumberType = NUMBERTYPE_MENUITEM_HOME;
                  break;
               case AEE_ADDRFIELD_PHONE_OTHER:
               case AEE_ADDRFIELD_PHONE_GENERIC:
                  switch(cat) {
                    case AEE_ADDR_CAT_SPEEDDIAL:
                      ci->m_fldNumberType = NUMBERTYPE_SPEEDDIAL;
                      break;
                    case AEE_ADDR_CAT_SIM_ADN:
                    case AEE_ADDR_CAT_SIM_ADN2:
                      ci->m_fldNumberType = NUMBERTYPE_RUIM;
                      break;
                    case AEE_ADDR_CAT_SIM_FDN:
                    case AEE_ADDR_CAT_SIM_FDN2:
                      ci->m_fldNumberType = NUMBERTYPE_RUIM;
                      break;
                    case AEE_ADDR_CAT_SIM_SDN:
                    case AEE_ADDR_CAT_SIM_SDN2:
                      ci->m_fldNumberType = NUMBERTYPE_RUIM;
                      break;
                    default:
                      ci->m_fldNumberType = NUMBERTYPE_OTHER;
                      break;
                  }
                  break;
               default:
                 gotPhone = FALSE;
                 break;
            }
         }
         if (addrField->fID == AEE_ADDRFIELD_VOICETAG)
            hasVoiceTag = TRUE;
         if (hasVoiceTag && gotPhone)
            break;
      }
   }

   if ((ci->m_fldNumberType == NUMBERTYPE_SPEEDDIAL) && hasVoiceTag)
      ci->m_fldNumberType = NUMBERTYPE_VOICETAG;

   if (bRelease) {
      IADDRREC_Release(addrRec);
   }

   if (gotPhone)
      return TRUE;

   ci->m_fldNumberId = (uint16)0;
   ci->m_fldNumberType = 0;

   return FALSE;
}

/*=============================================================================
FUNCTION: ContDlgHandler_Sort_by_Name

DESCRIPTION: Sort function that compares two records and returns which 
             is name is greater.  The comparison is done case insensitively.

PARAMETERS:
   ContactInfo *ci_a;
   ContactInfo *ci_b;

RETURN VALUE:
   < 0 if ci_a has a lower name than ci_b
   > 0 if ci_a has a higher name than ci_b

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int ContDlgHandler_Sort_by_Name (ContactInfo *ci_a, ContactInfo *ci_b) 
{
  AECHAR *name_a;
  AECHAR *name_b;
  int ret_val = 0;

  if ((ci_a == NULL) || (ci_b == NULL))
    return 0;

  /* Make a copy of the names so we can modify them if need be. */
  name_a = WSTRDUP(ci_a->m_name);
  name_b = WSTRDUP(ci_b->m_name);

  /* If the name's both were copied, we convert them to lower case
   * and then do a comparison.
   */
  if ((name_a != NULL) && (name_b != NULL))
  {
    WSTRLOWER(name_a);
    WSTRLOWER(name_b);
    ret_val = WSTRCMP(name_a, name_b);
  }

  /* Free the names we created. */
  FREEIF(name_a);
  FREEIF(name_b);

  return ret_val;
}

/*=============================================================================
FUNCTION: ContDlgHandler_Sort_by_Index

DESCRIPTION: Sort function that compares two records and returns which index
             is greater.

PARAMETERS:
   ContactInfo *ci_a;
   ContactInfo *ci_b;

RETURN VALUE:
   < 0 if ci_a has a lower index than ci_b
   > 0 if ci_a has a greater index than ci_b

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int ContDlgHandler_Sort_by_Index (ContactInfo *ci_a, ContactInfo *ci_b) 
{
  if ((ci_a == NULL) || (ci_b == NULL))
    return 0;

  return (ci_a->m_speedDialIndex - ci_b->m_speedDialIndex);
}

/*=============================================================================
FUNCTION: ContDlgHandler_PopulateContactListMenu

DESCRIPTION: Adds contact names into the specified menu.

PARAMETERS:
   *pMe: CContactApp object pointer
   *ctrl: IMenuCtl interface pointer
startIdx: the contact index to start at (this is an index into the
                                         CContactApp::m_contact_iv IVector)
   count: number of contact names to add
   clearMenu: TRUE if the current menu contents should be erased before
              adding the new items

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void ContDlgHandler_PopulateContactListMenu(CContactApp *pMe,
                                                   IMenuCtl    *ctrl,
                                                   uint16       startIdx,
                                                   uint16       count,
                                                   boolean      clearMenu)
{
   CtlAddItem   ai;
   ContactInfo *ci;
   uint16       i;
   AECHAR       szUsedFormatStr[FLDLEN_NAME+1];
   AECHAR       szMenuItem[FLDLEN_NAME+1];

   // init common fields
   ai.pImage = NULL;
   ai.pszResText = NULL;
   ai.pszResImage =  CONTACTAPP_RES_FILE;
   ai.wText = 0;
   ai.wFont = 0;
   ai.dwData = 0;

   szUsedFormatStr[0] = 0;
   if(STR_CONTACTLIST_SPEEDDIAL==pMe->m_contactListType){
      // Load the format string for speed dials menu items...
      (void) ISHELL_LoadResString(pMe->m_pShell,
                                  CONTACTAPP_RES_FILE,
                                  STR_SPEEDDIAL_MENU_ASSIGNED,
                                  szUsedFormatStr,
                                  sizeof(szUsedFormatStr));
   }

   IMENUCTL_SetProperties(ctrl,
                          IMENUCTL_GetProperties(ctrl) | MP_NO_REDRAW);

   if (clearMenu) {
      (void) IMENUCTL_DeleteAll(ctrl);
   }

   for (i = 0; i < count; i++) {
      ci = IVector_ElementAt(pMe->m_contact_iv, i + startIdx);
      if (NULL == ci) {
         // Probably at end of contacts
         break;
      }

      //Get the first number type for this contact info to show that icon
      (void)ContDlgHandler_GetFirstNumberType(pMe, ci, NULL);

      if (STR_CONTACTLIST_SPEEDDIAL==pMe->m_contactListType){
         WSPRINTF(szMenuItem,
                     sizeof(szMenuItem),
                     szUsedFormatStr,
                     ci->m_speedDialIndex,
                     ci->m_name);
      } else {
         (void)WSTRLCPY(szMenuItem, ci->m_name, ARR_SIZE(szMenuItem));
      }


      ai.pText = szMenuItem;

      switch (ci->m_fldNumberType) {
         case NUMBERTYPE_MENUITEM_WORK:
            ai.wImage = ICON_WORKNUMBER;
            break;
         case NUMBERTYPE_MENUITEM_HOME:
            ai.wImage = ICON_HOMENUMBER;
            break;
         case NUMBERTYPE_MENUITEM_PAGER:
            ai.wImage = ICON_PAGERNUMBER;
            break;
         case NUMBERTYPE_RUIM:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
           switch (ci->m_class)
           {
            case AEE_ADDR_CAT_SIM_ADN:
            case AEE_ADDR_CAT_SIM_FDN:
              ai.wImage = ICON_RUIM;
              break;
            case AEE_ADDR_CAT_SIM_ADN2:
            case AEE_ADDR_CAT_SIM_FDN2:
              ai.wImage = ICON_RUIM2;
              break;
            case AEE_ADDR_CAT_SIM_SDN:
              ai.wImage = ICON_CONTACTCLASS_SDN;
              break;
            case AEE_ADDR_CAT_SIM_SDN2:
              ai.wImage = ICON_CONTACTCLASS_SDN2;
              break;
            default :
              break;
           }
           break;
#else
           switch (ci->m_class)
           {
             case AEE_ADDR_CAT_SIM_SDN:
               ai.wImage = ICON_CONTACTCLASS_SDN;
               break;
             default:
               ai.wImage = ICON_RUIM;
               break;
            }
            break;
#endif
         case NUMBERTYPE_SPEEDDIAL:
            ai.wImage = ICON_SPEEDDIAL;
            break;
         case NUMBERTYPE_VOICETAG:
            ai.wImage = ICON_VOICETAG;
            break;
         case NUMBERTYPE_OTHER:
            ai.wImage = ICON_NUMBER;
            break;
         case NUMBERTYPE_MENUITEM_MOBILE:
            ai.wImage = ICON_MOBILENUMBER;
            break;
         default:
            ai.wImage= ICON_NOPHONE;
            break;
      }
      ai.wItemID = i + startIdx;
      //if speed dial list then we do not show the icon.
      if(ai.wImage != 0){
         (void) IMENUCTL_AddItemEx(ctrl, &ai);
      } else {
         (void) IMENUCTL_AddItem(ctrl,
                        NULL,
                        0,
                        ai.wItemID,
                        ci->m_name,
                        ci->m_fldNumberId);
      }
   }

   if (clearMenu) {
      IMENUCTL_SetSel(ctrl, pMe->m_contactSelId);
   }

   IMENUCTL_SetProperties(ctrl,
                          IMENUCTL_GetProperties(ctrl) & ~MP_NO_REDRAW);

   (void) IMENUCTL_Redraw(ctrl);
}

/*=============================================================================
FUNCTION: ContDlgHandler_PopulateSpeedDialListMenu

DESCRIPTION: Adds speed dial list contact names into the menu. First it marks
   all of the items as empty. Then iterates through the contact list in
   m_speeddial_iv and modifies the proper item.

PARAMETERS:
   *pMe: CContactApp object pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void ContDlgHandler_PopulateSpeedDialListMenu(CContactApp *pMe,
                                                     IMenuCtl    *ctrl,
                                                     uint16       startIdx,
                                                     uint16       count,
                                                     boolean      clearMenu)

{
   ContactInfo *ci = NULL;
   uint16       i;
   AECHAR       szEmptyFormatStr[16];
   AECHAR       szUsedFormatStr[FLDLEN_NAME+1];
   AECHAR       szMenuItem[FLDLEN_NAME+1];
   int8         wIndex=-1;

   szEmptyFormatStr[0] = 0;
   (void) ISHELL_LoadResString(pMe->m_pShell,
                               CONTACTAPP_RES_FILE,
                               STR_SPEEDDIAL_MENU_EMPTY,
                               szEmptyFormatStr,
                               sizeof(szEmptyFormatStr));
   szUsedFormatStr[0] = 0;
   (void) ISHELL_LoadResString(pMe->m_pShell,
                               CONTACTAPP_RES_FILE,
                               STR_SPEEDDIAL_MENU_ASSIGNED,
                               szUsedFormatStr,
                               sizeof(szUsedFormatStr));

   //if we were unable to load string or the
   //characters array representing the slots are null
   if ((0 == szEmptyFormatStr[0])|| (0 == szUsedFormatStr[0]) ||
       (pMe->m_nSpeedDialIndexesEmpty == NULL)) {
      return ;
   }

   IMENUCTL_SetProperties(ctrl,
                          IMENUCTL_GetProperties(ctrl) | MP_NO_REDRAW);

   if (clearMenu) {
      (void) IMENUCTL_DeleteAll(ctrl);
   }
   //if these are the last numbers, then show only the available
   if(count+startIdx > MAX_SPEED_DIAL_NUMS){
      count = MAX_SPEED_DIAL_NUMS - startIdx;
   }
   for (i = 0; i < count; i++) {
      ci = NULL;
      wIndex = pMe->m_nSpeedDialIndexesEmpty[i+startIdx];
      //if it is empty
      if(wIndex == SPEED_DIAL_EMPTY){

         WSPRINTF(szMenuItem,
                  FLDLEN_NAME+1,
                  szEmptyFormatStr,
                  i + (startIdx+SPEED_DIAL_USER_START_INDEX));

         (void) IMENUCTL_AddItem(ctrl,
                                 NULL,
                                 0,
                                 i + startIdx,
                                 szMenuItem,
                                 i + startIdx);

      }else{
         if(wIndex>=0){
            ci = IVector_ElementAt(pMe->m_speeddial_iv,
                                   (uint32)wIndex); /*lint !e571*/
         }
         if (NULL == ci) {
            // Probably at end of contacts
            break;
         }

         WSPRINTF(szMenuItem,
                     sizeof(szMenuItem),
                     szUsedFormatStr,
                     ci->m_speedDialIndex,
                     ci->m_name);

         (void) IMENUCTL_AddItem(ctrl,
                                 NULL,
                                 0,
                                 i + startIdx,
                                 szMenuItem,
                                 ci->m_fldNumberId);
      }
   }

   if (clearMenu) {
      IMENUCTL_SetSel(ctrl, pMe->m_speedDialSelId);
   }

   IMENUCTL_SetProperties(ctrl,
                          IMENUCTL_GetProperties(ctrl) & ~MP_NO_REDRAW);

   (void) IMENUCTL_Redraw(ctrl);

}

/*=============================================================================
FUNCTION: ContDlgHandler_SetMenuStyle

DESCRIPTION:
   Sets the menu style for the menu control. This menu style is consistenet
   across all the controls in contacts applet

PARAMETERS:
   *menu:  Pointer to the IMenuCtl object

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void ContDlgHandler_SetMenuStyle(IShell *shell, IMenuCtl *menu)
{
   SetDefaultMenuLook(shell, menu);
}

/*=============================================================================
FUNCTION: ContDlgHandler_SpeedDialMenuKeyTimer

DESCRIPTION:
   Called when the timer for the keys in the speed dial menu expires. We look
   at the key in the cache and jump to that location in the menu

PARAMETERS:
   *pUser: pointer to CContactApp object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void ContDlgHandler_SpeedDialMenuKeyTimer(void *pUser)
{
   CContactApp *pMe = (CContactApp*)pUser;
   uint16       wIndex = 0;
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  SPEEDDIAL_MENU_ENTRIES);
   if (NULL == menu) {
      return;
   }
   //calculate the index based on the last two keys in cache
   wIndex = (uint16)(((int)(pMe->m_indexSpeedDial[0] - AVK_0)*10)+/*lint !e656*/
                      (int)(pMe->m_indexSpeedDial[1] - AVK_0)- /*lint !e656*/
                       SPEED_DIAL_USER_START_INDEX);

   if(wIndex > MAX_SPEED_DIAL_NUMS){
      return;
   }

   pMe->m_speedDialSelId = wIndex;

   //Jump to ht index location
#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
   pMe->m_speedDialListMenuStart = wIndex;
   ContDlgHandler_PopulateSpeedDialListMenu(pMe,
                                            menu,
                                            pMe->m_speedDialListMenuStart,
                                            pMe->m_numTextOnlyLines-1,
                                            TRUE);
#else

   IMENUCTL_SetSel(menu, pMe->m_speedDialSelId);

#endif /* defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW) */
   //reset the predict keys cache
   pMe->m_indexSpeedDial[0] = AVK_0;
   pMe->m_indexSpeedDial[1] = AVK_0;

}


#ifdef FEATURE_CONTACTNAME_PREDICT

/*=============================================================================
FUNCTION:  ContDlgHandler_IsPredictMatch

DESCRIPTION:  Determines if the specified character could have been
              entered by the specificed keycode. (ie, the '2' key can
              enter the characters "ABCabc2")

PARAMETERS:
   ch: AECHAR character
   key: BREW key code

RETURN VALUE:
   boolean:  TRUE if the character and keycode match

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_IsPredictMatch(AECHAR ch, AVKType key)
{
   static const AECHAR sszMultitap1[] = {
      '.',',','-','\'','@',':','!','?','/','1','\0'
   };

   static const AECHAR sszMultitap2[] = {
      'A','a','B','b','C','c','2','\0'
   };

   static const AECHAR sszMultitap3[] = {
      'D','d','E','e','F','f','3','\0'
   };

   static const AECHAR sszMultitap4[] = {
      'G','g','H','h','I','i','4','\0'
   };

   static const AECHAR sszMultitap5[] = {
      'J','j','K','k','L','l','5','\0'
   };

   static const AECHAR sszMultitap6[] = {
      'M','m','N','n','O','o','6','\0'
   };

   static const AECHAR sszMultitap7[] = {
      'P','p','Q','q','R','r','S','s','7','\0'
   };

   static const AECHAR sszMultitap8[] = {
      'T','t','U','u','V','v','8','\0'
   };

   static const AECHAR sszMultitap9[] = {
      'W','w','X','x','Y','y','Z','z','9','\0'
   };

   static const AECHAR sszMultitap0[] = {
      '0','\0'
   };


   AECHAR const *chTable;

   switch (key) {
      case AVK_0:
         chTable = sszMultitap0;
         break;
      case AVK_1:
         chTable = sszMultitap1;
         break;
      case AVK_2:
         chTable = sszMultitap2;
         break;
      case AVK_3:
         chTable = sszMultitap3;
         break;
      case AVK_4:
         chTable = sszMultitap4;
         break;
      case AVK_5:
         chTable = sszMultitap5;
         break;
      case AVK_6:
         chTable = sszMultitap6;
         break;
      case AVK_7:
         chTable = sszMultitap7;
         break;
      case AVK_8:
         chTable = sszMultitap8;
         break;
      case AVK_9:
         chTable = sszMultitap9;
         break;
      default:
         MSG_ERROR("Unexpected AVKType: %d", key, 0, 0);
         return FALSE;
   }

   for (; *chTable != '\0'; chTable++) {
      if (ch == *chTable) {
         return TRUE;
      }
   }
   return FALSE;
}


/*=============================================================================
FUNCTION:  ContDlgHandler_DoContactNamePredict

DESCRIPTION: Given the current keystroke data in the
             CContactApp::m_namePredictData[] array, determine the most
             likely contact name match.

PARAMETERS:
   *pMe: [in] CContactApp object pointer
 curIdx: [in] the current index into CContactApp::m_contact_iv
         [out] the next predicted index (if the return value is TRUE,
                                         otherwise unchanged
bFromCurPos: [in] TRUE if the search should start from the current menu
                  selection.  FALSE if the search should start from the
                  first menu item.

RETURN VALUE:
   uint16: the new index (may be the same as curIdx)

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_DoContactNamePredict(CContactApp *pMe,
                                                   uint16      *idx,
                                                   boolean      bFromCurPos)
{
   uint16 i;
   int    len,
      charIdx;

   if (pMe->m_namePredictLen > 0) {

      for (i = (bFromCurPos ? *idx : 0);
          i < IVector_Size(pMe->m_contact_iv);
          i++) {
         ContactInfo *ci = IVector_ElementAt(pMe->m_contact_iv, i);

         if (NULL == ci) {
            break;
         }

         len = WSTRLEN(ci->m_name);
         if (len >= pMe->m_namePredictLen) {
            charIdx = 0;
            while (ContDlgHandler_IsPredictMatch(ci->m_name[charIdx],
                                            pMe->m_namePredictData[charIdx])) {

               charIdx++;
               if (charIdx == pMe->m_namePredictLen) {
                  // Found a match
                  *idx = i;
                  return TRUE;
               }
            }
         }
      }

   }

   // No match, return the current index
   return FALSE;
}

#endif /* FEATURE_CONTACTNAME_PREDICT */


/*=============================================================================
FUNCTION: ContDlgHandler_FindCDBRec

DESCRIPTION:
   Loads pMe->m_rec with the IAddrRec for the currently selected
   record (pMe->m_contactSelId).  Returns TRUE on success

PARAMETERS:
   *pMe: ContactApp object pointer

RETURN VALUE:
   boolean: TRUE if successful else FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_FindCDBRec(CContactApp *pMe)
{
   ContactInfo *ci;

   ci = IVector_ElementAt(pMe->m_contact_iv,
                          pMe->m_contactSelId);

   if (NULL == ci) {
      return FALSE;
   }

   // Already loaded?
   if (pMe->m_rec != NULL)
      ContDlgHandler_ReleaseCDBRec(pMe);

   pMe->m_rec = IADDRBOOK_GetRecByID(pMe->m_addr_book,
                                     ci->m_id);

   if (NULL == pMe->m_rec) {
      return FALSE;
   }

   return TRUE;
}


/*=============================================================================
FUNCTION: ContDlgHandler_ReleaseCDBRec

DESCRIPTION:
   Releases the IADDRREC pointer

PARAMETERS:
   *pMe: CContactApp object pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ContDlgHandler_ReleaseCDBRec(CContactApp *pMe)
{
   if (pMe->m_rec != NULL) {
      (void) IADDRREC_Release(pMe->m_rec);
      pMe->m_rec = NULL;
   }
}


/*=============================================================================
FUNCTION: ContDlgHandler_ContactInfo2AEEAddrField

DESCRIPTION:
   Initializes a AEEAddrField structure with the contents of
   the pMe->m_contactinfo ContactInfo structure.  Set
   mallocData to TRUE if you want to duplicate the data buffer.

   Returns TRUE on success.  The method will never return FALSE
   if mallocData == FALSE

PARAMETERS:
   *pMe: CContactApp object pointer
   *cf: AEEAddrField object pointer
   mallocData: boolean variable indicates if data should be allocated

RETURN VALUE:
   boolean: TRUE if successful else FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_ContactInfo2AEEAddrField(CContactApp *pMe,
                                                       AEEAddrField    *cf,
                                                       boolean      mallocData)
{
   cf->fID = AEE_ADDRFIELD_NAME;
   cf->fType = AEEDB_FT_STRING;
   cf->wDataLen = (uint16) WSTRSIZE(pMe->m_contactInfo.m_name);

   if (TRUE == mallocData) {
      cf->pBuffer = MALLOC(cf->wDataLen);
      if (NULL == cf->pBuffer) {
         return FALSE;
      }

      (void)MEMCPY(cf->pBuffer, pMe->m_contactInfo.m_name, cf->wDataLen);
   } else {
      cf->pBuffer = pMe->m_contactInfo.m_name;
   }

   return TRUE;
}


/*=============================================================================
FUNCTION: ContDlgHandler_FieldInfo2AEEAddrField

DESCRIPTION:
   Initializes a AEEAddrField structure with the contents of
   a FieldInfo structure.

   Set mallocData to true if the data buffer should be
   duplicated.

   Returns TRUE if successful.

PARAMETERS:
   *fi: FieldInfo object pointer
   *cf: AEEAddrField Object pointer
   mallocData: boolean variable indicates if data should be allocated

RETURN VALUE:
   boolean: TRUE if successful else FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_FieldInfo2AEEAddrField(FieldInfo    *fi,
                                                     AEEAddrField *cf,
                                                     boolean       mallocData)
{
  char szId[FLDLEN_GROUPID+1];
  uint16 grpId;


  if (fi->m_type == STR_FIELDINPUT_ID)
  {
    (void)WSTRTOSTR(fi->m_name, szId, sizeof(szId));
    grpId = ATOI(szId);
    cf->wDataLen = sizeof(grpId);
    cf->pBuffer = MALLOC(cf->wDataLen);
    if (NULL == cf->pBuffer) {
      return FALSE;
    }
    (void)MEMCPY(cf->pBuffer, &grpId, cf->wDataLen);
  }
  else
  {

    cf->wDataLen = (uint16) WSTRSIZE(fi->m_name);

    if (TRUE == mallocData) {
      cf->pBuffer = MALLOC(cf->wDataLen);
      if (NULL == cf->pBuffer) {
        return FALSE;
      }
      (void)MEMCPY(cf->pBuffer, fi->m_name, cf->wDataLen);
    } else {
      cf->pBuffer = fi->m_name;
    }
  }

  switch (fi->m_type) {
      case STR_FIELDINPUT_NUMBER:
         cf->fID = AEE_ADDRFIELD_PHONE_HOME;
         cf->fType = AEEDB_FT_PHONE;

         switch (fi->m_numberType) {
            case NUMBERTYPE_MENUITEM_WORK:
               cf->fID = AEE_ADDRFIELD_PHONE_WORK;
               break;
            case NUMBERTYPE_MENUITEM_MOBILE:
               cf->fID = AEE_ADDRFIELD_PHONE_CELL;
               break;
            case NUMBERTYPE_MENUITEM_PAGER:
               cf->fID = AEE_ADDRFIELD_PHONE_PAGER;
               break;

            case NUMBERTYPE_RUIM:
               cf->fID = AEE_ADDRFIELD_PHONE_GENERIC;
               break;

            case NUMBERTYPE_SPEEDDIAL:
               cf->fID = AEE_ADDRFIELD_PHONE_OTHER;
               break;

            case NUMBERTYPE_VOICETAG:
               cf->fID = AEE_ADDRFIELD_PHONE_OTHER;
               break;

            case NUMBERTYPE_MENUITEM_HOME:
            default:
               cf->fID = AEE_ADDRFIELD_PHONE_HOME;
               break;
         }
         break;

      case STR_FIELDINPUT_ADDRESS:
         cf->fID   = AEE_ADDRFIELD_ADDRESS;
         cf->fType = AEEDB_FT_STRING;
         break;

      case STR_FIELDINPUT_EMAIL:
         cf->fID   = AEE_ADDRFIELD_EMAIL;
         cf->fType = AEEDB_FT_STRING;
         break;

      case STR_FIELDINPUT_URL:
         cf->fID   = AEE_ADDRFIELD_URL;
         cf->fType = AEEDB_FT_STRING;
         break;

      case STR_FIELDINPUT_SECNAME:
         cf->fID   = AEE_ADDRFIELD_OTHERNAME;
         cf->fType = AEEDB_FT_STRING;
         break;

      case STR_FIELDINPUT_ADDNUMBER:
         cf->fID   = AEE_ADDRFIELD_ADDITIONAL_NUMBER;
         cf->fType = AEEDB_FT_PHONE;
         break;

      case STR_FIELDINPUT_ID:
         cf->fID   = AEE_ADDRFIELD_GRPNAME_ID;
         cf->fType = AEEDB_FT_WORD;
         break;

      default:
         MSG_ERROR("Unrecognized field type, %d.", fi->m_type, 0, 0);
         return FALSE;
  }

  return TRUE;
}

/*=============================================================================
FUNCTION: ContDlgHandler_Itoa

DESCRIPTION:
   Converts integer to ascii

PARAMETERS:
   
RETURN VALUE:
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

char* ContDlgHandler_Itoa(int i, char *chrptr, int base)
{
  if (base == 10)
  {
    (void)SPRINTF(chrptr, "%d", i);
    return chrptr;
  }
  else if (base == 16)
  {
    (void)SPRINTF(chrptr, "%x", i);
    return chrptr;
  }

  return NULL;
}

/*=============================================================================
FUNCTION: ContDlgHandler_AEEAddrField2FieldInfo

DESCRIPTION:
   Initializes a FieldInfo structure with the specified AEEAddrField
   structure.  Returns a newly allocated FieldInfo structure on
   success, NULL if the data in AEEAddrField could not be
   converted to a FieldInfo structure.

   The caller is responsible for deallocation

PARAMETERS:
   *cf: pointer to AEEAddrField object

RETURN VALUE:
    FieldInfo *: FieldInfo object pointer

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
FieldInfo *ContDlgHandler_AEEAddrField2FieldInfo(CContactApp *pMe,
                                                 AEEAddrField *cf)
{
   FieldInfo  fi;
   FieldInfo *pfi;
   char szId[FLDLEN_GROUPID+1];
   uint16 grpId;

   if (cf == NULL)
   {
      MSG_ERROR("Invalid field", 0, 0, 0);
      return NULL;
   }

   if (pMe == NULL)
   {
      MSG_ERROR("Unexpected Error", 0, 0, 0);
      return NULL;
   }
   // Make sure it is a field we know about...
   switch (cf->fID) {
      case AEE_ADDRFIELD_NAME:
         return NULL;

      case AEE_ADDRFIELD_PHONE_WORK:
         fi.m_type = STR_FIELDINPUT_NUMBER;
         fi.m_numberType = NUMBERTYPE_MENUITEM_WORK;
         break;

      case AEE_ADDRFIELD_PHONE_CELL:
         fi.m_type = STR_FIELDINPUT_NUMBER;
         fi.m_numberType = NUMBERTYPE_MENUITEM_MOBILE;
         break;

      case AEE_ADDRFIELD_PHONE_HOME:
         fi.m_type = STR_FIELDINPUT_NUMBER;
         fi.m_numberType = NUMBERTYPE_MENUITEM_HOME;
         break;

      case AEE_ADDRFIELD_PHONE_GENERIC:
         fi.m_type = STR_FIELDINPUT_NUMBER;
         if (pMe->m_fldrecs_is_sd)
            fi.m_numberType = NUMBERTYPE_SPEEDDIAL;
         else
            fi.m_numberType = NUMBERTYPE_RUIM;
         break;
      
      case AEE_ADDRFIELD_OTHERNAME:
         fi.m_type = STR_FIELDINPUT_SECNAME;
        break;
      
      case AEE_ADDRFIELD_ADDITIONAL_NUMBER:
         fi.m_type = STR_FIELDINPUT_ADDNUMBER;
         fi.m_numberType = NUMBERTYPE_RUIM;
        break;
      
      case AEE_ADDRFIELD_GRPNAME_ID:
         fi.m_type = STR_FIELDINPUT_ID;
        break;
      
      case AEE_ADDRFIELD_PHONE_OTHER:
         fi.m_type = STR_FIELDINPUT_NUMBER;
         if (pMe->m_fldrecs_has_vt)
            fi.m_numberType = NUMBERTYPE_VOICETAG;
         else
            fi.m_numberType = NUMBERTYPE_SPEEDDIAL;
         break;

      case AEE_ADDRFIELD_PHONE_PAGER:
         fi.m_type = STR_FIELDINPUT_NUMBER;
         fi.m_numberType = NUMBERTYPE_MENUITEM_PAGER;
         break;

      case AEE_ADDRFIELD_ADDRESS:
         fi.m_type = STR_FIELDINPUT_ADDRESS;
         break;

      case AEE_ADDRFIELD_EMAIL:
         fi.m_type = STR_FIELDINPUT_EMAIL;
         break;

      case AEE_ADDRFIELD_URL:
         fi.m_type = STR_FIELDINPUT_URL;
         break;

         // Don't care/know about any other field type
      default:
         return NULL;
   }

   // Make sure the field type is something we can handle...
   switch (cf->fType) {
      case AEEDB_FT_STRING:
      case AEEDB_FT_PHONE:
         break;
      case AEEDB_FT_WORD:
        pfi = MALLOC(sizeof(FieldInfo));
        if (NULL == pfi) {
            FREE_FIELDINFO(fi);
            return NULL;
        }
        (void)MEMCPY(&grpId, cf->pBuffer, cf->wDataLen);

        ContDlgHandler_Itoa(grpId, szId, 10);
        fi.m_name = MALLOC(sizeof(AECHAR) * sizeof(szId));
        if (NULL == fi.m_name) {
            return NULL;
        }
        (void)STRTOWSTR(szId, fi.m_name, sizeof(AECHAR)* sizeof(szId));

        *pfi = fi;
        return pfi;

      default:
         return NULL;
   }

   fi.m_name = MALLOC(cf->wDataLen);
   if (NULL == fi.m_name) {
      return NULL;
   }

   pfi = MALLOC(sizeof(*pfi));
   if (NULL == pfi) {
      FREE_FIELDINFO(fi);
      return NULL;
   }

   (void)MEMCPY(fi.m_name, cf->pBuffer, cf->wDataLen);

   *pfi = fi;

   return pfi;
}


/*=============================================================================
FUNCTION: ContDlgHandler_LoadAEEAddrFields

DESCRIPTION:
   Retrieve all the AEEAddrField fields from rec and store them
   in pMe->m_fldrecs_iv

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
boolean: TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ContDlgHandler_LoadAEEAddrFields(CContactApp *pMe)
{
   AEEAddrField   fld;
   byte          *fldBuff;
   AEEAddrField  *pfld = NULL;
   boolean        ret = TRUE;
   int            j,wTotalFields;

   ContDlgHandler_FreeAEEAddrFields(pMe);

   if (FALSE == ContDlgHandler_FindCDBRec(pMe)) {
      return FALSE;
   }

   if (IADDRREC_GetCategory(pMe->m_rec) == AEE_ADDR_CAT_SPEEDDIAL)
      pMe->m_fldrecs_is_sd = TRUE;
   else
      pMe->m_fldrecs_is_sd = FALSE;

   //First get the field count and itereate through the
   //fields to get the data
   wTotalFields = IADDRREC_GetFieldCount(pMe->m_rec);

   pMe->m_fldrecs_has_vt = FALSE;

   for (j=0; j<wTotalFields; j++) {
      AEEAddrField *addrField = IADDRREC_GetField(pMe->m_rec,j);

      if (addrField->fID == AEE_ADDRFIELD_VOICETAG) {
         pMe->m_fldrecs_has_vt = TRUE;
#ifdef FEATURE_PHONE_VR
         pMe->m_fldrecs_vt = *((AEEVRUtterance *)addrField->pBuffer);
#endif
      }

      if (addrField->fID == AEE_ADDRFIELD_INDEX) {
         pMe->m_fldrecs_sdindex = *((uint16 *)addrField->pBuffer);
      }

      (void)MEMCPY(&fld,addrField,sizeof(AEEAddrField));

      fldBuff= fld.pBuffer;
      if (NULL == fldBuff) {
         break;
      }

      // Duplicate the field
      pfld = MALLOC(sizeof(*pfld));
      if (NULL == pfld) {
         ret = FALSE;
         break;
      }

      *pfld = fld;
      if (fld.pBuffer != NULL) {
         pfld->pBuffer = MALLOC(fld.wDataLen);
         if (NULL == pfld->pBuffer) {
            ret = FALSE;
            break;
         }
         (void)MEMCPY(pfld->pBuffer, fld.pBuffer, fld.wDataLen);
      }

      if (IVector_AddElement(pMe->m_fldrecs_iv, pfld) != SUCCESS) {
         ret = FALSE;
         break;
      }

      pfld = NULL;
   }

   if (pfld != NULL) {
      if (pfld->pBuffer != NULL) {
         FREE(pfld->pBuffer);
      }
      FREE(pfld);
   }

   return ret;
}


/*=============================================================================
FUNCTION: ContDlgHandler_UpdateAEEAddrFields

DESCRIPTION:
   Update rec with the current fields in pMe->m_fldrecs_iv

   Assumes that pMe->m_rec is valid

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
   boolean: TRUE if successful otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_UpdateAEEAddrFields(CContactApp *pMe)
{
   AEEAddrField *fArray;
   AEEAddrField *cf;
   uint32    count;
   int       ret;
   int i;
   int       retCatUpdate= SUCCESS;

   /* Delete the field that stores number type information, if the
      number was edited, the number type will be determined again  */
   for(i=0; i< IVector_Size(pMe->m_fldrecs_iv); i++) {
     cf = IVector_ElementAt(pMe->m_fldrecs_iv,i);
     if((cf->fType == AEEDB_FT_BYTE) &&
        (cf->fID == AEE_ADDRFIELD_GSM_TONNPI)) {
       cf = IVector_RemoveElementAt(pMe->m_fldrecs_iv,i);
       if (NULL == cf) {
         return FALSE;
       }
       
       FREE_AEEAddrField(*cf);
       FREE(cf);
       break;
     }
     
   }
   count = IVector_Size(pMe->m_fldrecs_iv);

   if (0 == count) {
      // Can't update with no fields
      return FALSE;
   }

   fArray = (AEEAddrField*)IVector_BuildBuffer(pMe->m_fldrecs_iv,
                                               sizeof(*fArray));

   if (NULL == fArray) {
      return FALSE;
   }

   if (NULL == pMe->m_rec)
   {
      MSG_ERROR("NULL Address record.", 0, 0, 0);
      return FALSE;
   }

   // Update all the fields of this record in address book
   ret = IADDRREC_UpdateAllFields(pMe->m_rec,
                                  fArray,
                                  (int)count);
   // Update the category of this record, only if category is not
   // AEE_ADDRCAT_RUIM_ADN
#ifdef FEATURE_MMGSDI
   if ((IADDRREC_GetCategory(pMe->m_rec) != AEE_ADDR_CAT_SIM_ADN) &&
       (IADDRREC_GetCategory(pMe->m_rec) != AEE_ADDR_CAT_SIM_ADN2) &&
       (IADDRREC_GetCategory(pMe->m_rec) != AEE_ADDR_CAT_SIM_FDN) &&
       (IADDRREC_GetCategory(pMe->m_rec) != AEE_ADDR_CAT_SIM_FDN2)
      )
#endif /*FEATURE_MMGSDI*/
   {
      retCatUpdate = IADDRREC_SetCategory(pMe->m_rec,
                                          pMe->m_contactInfo.m_class);
   }
   FREE(fArray);

   if ((SUCCESS == ret) && (SUCCESS == retCatUpdate)) {
      return TRUE;
   }
   return FALSE;
}


/*=============================================================================
FUNCTION: ContDlgHandler_FreeAEEAddrFields

DESCRIPTION:
   Release the heap used by the current fields in pMe->m_fldrecs_iv

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ContDlgHandler_FreeAEEAddrFields(CContactApp *pMe)
{
   uint32    i;
   AEEAddrField *f;

   // free the data first
   for (i = 0; i < IVector_Size(pMe->m_fldrecs_iv); i++) {
      f = (AEEAddrField*)IVector_ElementAt(pMe->m_fldrecs_iv, i);

      if (NULL == f)
      {
         MSG_ERROR("NULL address field, %d.", i, 0, 0);
         continue;
      }
      FREE_AEEAddrField(*f);
      FREE(f);

   }

   IVector_RemoveAllElements(pMe->m_fldrecs_iv);
}

/*=============================================================================
FUNCTION: ContDlgHandler_GetNumFields

DESCRIPTION:
   This function returns the number of fields matching the given
   field ID in a record.

PARAMETERS:
   *rec: Record
   fID:  Field ID

RETURN VALUE:
   int: Number of fields with specified field ID.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int ContDlgHandler_GetNumFields(IAddrRec *rec, AEEAddrFieldID fID)
{
   int i = 0;
   int cnt = 0;
   AEEAddrField * pField;

   for (i = 0; i < IADDRREC_GetFieldCount(rec); i++) {
      pField = IADDRREC_GetField(rec, i);
      if (pField && pField->fID == fID)
         cnt++;
   }

   return cnt;
}

/*=============================================================================
FUNCTION: ContDlgHandler_AddField

DESCRIPTION:
   This function will add a new field to the current contact based on
   the following CContactApp fields:

     m_fieldInfo.m_name      -- field data
     m_fieldInfo.m_type  -- type of field
     m_fieldInfo.m_numberType -- if number, the type of number
     m_contactSelId    -- the ID of the contact to add the field to

   Returns: TRUE on success, FALSE on failure

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
  int: EUNSUPPORTED: If field is not supported
       ENOMEMORY:    If the contact already has the maximum number of fields
       SUCCESS:      If successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int ContDlgHandler_AddField(CContactApp *pMe)
{
   AEEAddrField     field;
   int              retval;
   AEEAddrFieldInfo field_info;
   boolean          bSupported = FALSE;
   int              i;
   AEEAddrFieldID   alt_fID = FALSE;
   boolean          alt_found = FALSE;
   boolean          req_checked = FALSE;
   AEEAddrFieldID   choices[] = { AEE_ADDRFIELD_PHONE_HOME,
                                  AEE_ADDRFIELD_PHONE_WORK,
                                  AEE_ADDRFIELD_PHONE_CELL,
                                  AEE_ADDRFIELD_PHONE_PAGER
                                 };

   if (pMe == NULL)
   {
     return FALSE;
   }

   if (NULL == pMe->m_fieldInfo.m_name) {
      return FALSE;
   }

   if (FALSE == ContDlgHandler_FindCDBRec(pMe)) {
      return FALSE;
   }

   (void) ContDlgHandler_FieldInfo2AEEAddrField(&pMe->m_fieldInfo,
                                                &field,
                                                FALSE);


   retval = IADDRBOOK_EnumFieldsInfoInit(pMe->m_addr_book,
                                         IADDRREC_GetCategory(pMe->m_rec));
   if ( retval != AEE_SUCCESS)
      return EFAILED;

   /* Check maximum number of fields allowed per record */
   while(IADDRBOOK_EnumNextFieldsInfo(pMe->m_addr_book, &field_info)) {

      if (field_info.fieldID == field.fID) {
          req_checked = TRUE;
          if (ContDlgHandler_GetNumFields(pMe->m_rec, field.fID) <
              field_info.nMaxItems) {
             bSupported = TRUE;
             break; /* Success */
          }

          if (pMe->m_fieldInfo.m_type != STR_FIELDINPUT_NUMBER)
             break; /* Nothing more to look for */
      }

      if (pMe->m_fieldInfo.m_type == STR_FIELDINPUT_NUMBER) {
         /* Check current enumeration against alternate choices */
         if (!alt_found) {
            for (i = 0; i < ARR_SIZE(choices); i++) {
               if ((field_info.fieldID == choices[i]) &&
                   (ContDlgHandler_GetNumFields(pMe->m_rec,
                               field_info.fieldID) < field_info.nMaxItems)) {
                  /* Found an alternate number type */
                  alt_fID = choices[i];
                  alt_found = TRUE;
                  break; /* out of for loop */
               }
            }
         }
      }

      if (req_checked && alt_found)
         break;
   } /* while: end of enumeration */

   if (!bSupported) {
      if (alt_found) {
         field.fID = alt_fID;
      } else {
         if (req_checked)
            return ENOMEMORY;
         else
            return EUNSUPPORTED;
      }
   }

   //Add this new field to the record
   retval = IADDRREC_AddField(pMe->m_rec, &field);
   if (retval == SUCCESS) {
      pMe->m_new_rec_id = IADDRREC_GetRecID(pMe->m_rec);
   }

   return retval;
}


/*=============================================================================
FUNCTION: ContDlgHandler_SaveField

DESCRIPTION:
   Saves the info stored in pMe->m_fieldInfo into the
   IAddrRec at pMe->m_fldrecs_iv[pMe->m_fieldSelId]

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
   boolean: TRUE if successful otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ContDlgHandler_SaveField(CContactApp *pMe)
{
   AEEAddrField  *pcf;
   AEEAddrField   cf;
   boolean    ret;
   if (FALSE == ContDlgHandler_FindCDBRec(pMe)) {
      return FALSE;
   }

   pcf = IVector_ElementAt(pMe->m_fldrecs_iv,
                           pMe->m_fieldSelId);
   if (NULL == pcf) {
      return FALSE;
   }

   if (FALSE == ContDlgHandler_FieldInfo2AEEAddrField(&pMe->m_fieldInfo,
                                                      &cf,
                                                      TRUE)) {
      return FALSE;
   }

   FREE_AEEAddrField(*pcf);
   *pcf = cf;

   ret = ContDlgHandler_UpdateAEEAddrFields(pMe);
   return ret;
}


/*=============================================================================
FUNCTION: ContDlgHandler_DeleteField

DESCRIPTION:
   Deletes the field pMe->m_fieldSelId from the current record

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
   boolean: TRUE if successful otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ContDlgHandler_DeleteField(CContactApp *pMe)
{
   AEEAddrField  *cf;
   boolean    ret;

   if (pMe == NULL)
   {
     return FALSE;
   }

   if (FALSE == ContDlgHandler_FindCDBRec(pMe)) {
      return FALSE;
   }

   cf = IVector_RemoveElementAt(pMe->m_fldrecs_iv, pMe->m_fieldSelId);

   if (NULL == cf) {
      return FALSE;
   }

   FREE_AEEAddrField(*cf);
   FREE(cf);

   ret = ContDlgHandler_UpdateAEEAddrFields(pMe);
   return ret;
}


/*=============================================================================
FUNCTION: ContDlgHandler_CreateContact

DESCRIPTION:
   This function will create a new contact based on
   the following CContactApp fields:
     m_contactInfo.m_name    -- contact name
     m_contactInfo.m_class   -- contact classification

   And will add a field to the new contact based on
   the following CContactApp fields:
     m_fieldInfo.m_name      -- field data
     m_fieldInfo.m_type  -- type of field
     m_fieldInfo.m_numberType -- if number, the type of number

   Returns: TRUE on success, FALSE on failure

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
   boolean: TRUE if successful otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ContDlgHandler_CreateContact(CContactApp *pMe)
{
   AEEAddrField fields[3];
   IAddrRec *rec;

   int  num_fields = 0;

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
   int nStrLen;
   int nSSRetVal;
   int nOperation;
   ISUPPSVC *pISUPPSVC = NULL;
   uint8  ton_npi;

#endif /* (FEATURE_GSM) ||  (FEATURE_WCDMA) */

   if ( (NULL == pMe->m_contactInfo.m_name) ||
        (NULL == pMe->m_fieldInfo.m_name) ) {
      return FALSE;
   }

   //Copy the contact name in the first field
   (void) ContDlgHandler_ContactInfo2AEEAddrField(pMe,
                                                  &fields[0],
                                                  FALSE);
   //copy the other field ie. phone number, email,etc. in the
   //second field
   (void) ContDlgHandler_FieldInfo2AEEAddrField(&pMe->m_fieldInfo,
                                                &fields[1],
                                                FALSE);

   /* two fields are to be copied */
   num_fields = 2;

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
   if (pMe->m_fieldInfo.m_type == STR_FIELDINPUT_NUMBER)
   {
     nStrLen = WSTRLEN(pMe->m_fieldInfo.m_name);
     if(nStrLen != 0) 
     {
       nSSRetVal = ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_SUPPSVC,
                                         (void **)&pISUPPSVC );
       if (( nSSRetVal != SUCCESS) || (pISUPPSVC == NULL))
       {
         MSG_ERROR("Unable to create ISUPPSVC %d", nSSRetVal, 0, 0);
       }
       else
       {
         nSSRetVal = ISUPPSVC_StringCheck(pISUPPSVC, nStrLen,
                                          pMe->m_fieldInfo.m_name, &nOperation);

         (void) ISUPPSVC_Release(pISUPPSVC);

         if(nSSRetVal == AEESUPPSVC_IS_SS) 
         {
           ton_npi = 0xFF;

           fields[num_fields].fID = AEE_ADDRFIELD_GSM_TONNPI;
           fields[num_fields].fType = AEEDB_FT_BYTE; 
           fields[num_fields].wDataLen = sizeof(ton_npi);
           fields[num_fields].pBuffer = &ton_npi;

           num_fields++;
         }
       }
     }
   }
#endif /* (FEATURE_GSM) ||  (FEATURE_WCDMA)*/

   //Create Record in the address book
   rec =   IADDRBOOK_CreateRec(pMe->m_addr_book,
                               pMe->m_contactInfo.m_class,
                               fields,
                               num_fields);

   if (NULL == rec) {
      return FALSE;
   }
   //copy the newly added record id. It will used
   //by the view contact state
   pMe->m_new_rec_id = IADDRREC_GetRecID(rec);

   (void) IADDRREC_Release(rec);

   return TRUE;
}

/*=============================================================================
FUNCTION: ContDlgHandler_SaveContactInfo

DESCRIPTION:
   Save the contact name and classification using the information
   from the following fields:
     m_contactInfo.m_id      -- contact id
     m_contactInfo.m_name    -- contact name
     m_contactInfo.m_class   -- contact classification

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
   boolean: TRUE if successful otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ContDlgHandler_SaveContactInfo(CContactApp *pMe)
{
   int           i;
   AEEAddrField *pfield;
   AEEAddrField  field;
   boolean       ret;

   if (NULL == pMe->m_contactInfo.m_name)
   {
      MSG_ERROR("Empty name in contact info.", 0, 0, 0);
      return FALSE;
   }

   if (FALSE == ContDlgHandler_FindCDBRec(pMe)) {
      return FALSE;
   }

   if (FALSE == ContDlgHandler_LoadAEEAddrFields(pMe)) {
      return FALSE;
   }

   // Find the FULLNAME field and alter it
   i = 0;
   for (;;) {
      pfield = (AEEAddrField *)IVector_ElementAt(pMe->m_fldrecs_iv,
                                                 (uint32)i);
      i++;
      if (NULL == pfield) {
         break;
      }

      // Looking for the full name field...
      if (pfield->fID != AEE_ADDRFIELD_NAME) {
         continue;
      }

      if (FALSE == ContDlgHandler_ContactInfo2AEEAddrField(pMe,
                                                           &field,
                                                           TRUE)) {
         return FALSE;
      }
      // Replace the old name with the new one
      FREE(pfield->pBuffer);
      *pfield = field;

      break;
   }

   // Didn't find a fullname field so create one
   // (this will probably never be necessary but be paranoid)
   if (NULL == pfield) {

      pfield = MALLOC(sizeof(*pfield));
      if (NULL == pfield) {
         return FALSE;
      }

      if (FALSE == ContDlgHandler_ContactInfo2AEEAddrField(pMe,
                                                           &field,
                                                           TRUE)) {
         FREE(pfield);
         return FALSE;
      }

      *pfield = field;

      if (SUCCESS != IVector_AddElement(pMe->m_fldrecs_iv, pfield)) {
         FREE(pfield->pBuffer);
         FREE(pfield);
         return FALSE;
      }

   }
   ret = ContDlgHandler_UpdateAEEAddrFields(pMe);
   return ret;
}


/*=============================================================================
FUNCTION: ContDlgHandler_DeleteContact

DESCRIPTION:
   Delete the current contact (pMe->m_contactSelId)

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
   boolean: TRUE if successful otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ContDlgHandler_DeleteContact(CContactApp *pMe)
{
   int          ret;
   ContactInfo *ci;

   if (FALSE == ContDlgHandler_FindCDBRec(pMe)) {
      return FALSE;
   }

#ifdef FEATURE_PHONE_VR
   ContDlgHandler_RemoveRec_VoiceTag(pMe, pMe->m_rec);
#endif

   //Remove the record from the database
   ret = IADDRREC_RemoveRec(pMe->m_rec);
   pMe->m_rec = NULL;  // IIAddrRec_Remove also frees the record

   if (ret != SUCCESS) {
      return FALSE;
   }

   ci = IVector_ElementAt(pMe->m_contact_iv,
                          pMe->m_contactSelId);

   if (NULL == ci)
   {
      /* Don't fail if the cached contact doesn't exist, since we've
       * accomplished the purpose of the function.
       */
      MSG_ERROR("Cached contact empty at index %d.", pMe->m_contactSelId, 0, 0);
      return FALSE;
   }
   else
   {
      //remove this element from the cache
      (void) IVector_RemoveElementAt(pMe->m_contact_iv, pMe->m_contactSelId);
      FREE_CONTACTINFO(*ci);
   }

   return TRUE;
}


/*=============================================================================
FUNCTION: ContDlgHandler_LoadAvailContactList

DESCRIPTION:
   Loads all available contacts

   Returns TRUE on success.  FALSE if an error occured (out of memory)

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
   boolean: TRUE if successful otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ContDlgHandler_LoadAvailContactList(CContactApp *pMe)
{
  pMe->m_availRecInit = TRUE;

  return ContDlgHandler_LoadContactList(pMe);
}

/*=============================================================================
FUNCTION: ContDlgHandler_GetContactName

DESCRIPTION:
   Returns Contact Name

   Usually returns the AEE_ADDRFIELD_NAME. If this field is empty,
   goes through other fields to construct a non-empty contact name
   field for ContactApp.

PARAMETERS:
   *pMe: pointer to CContactApp object
   *addrRec: pointer to IAddrRec

RETURN VALUE:
   AEECHAR *: Pointer to buffer in corresponding field in addrRec.

COMMENTS:
   Used in ContDlgHandler_LoadContactList( ).

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AECHAR * ContDlgHandler_GetContactName(CContactApp *pMe, IAddrRec *addrRec)
{
  

  AEEAddrFieldID addrFieldList[] = {
             AEE_ADDRFIELD_NAME,
             AEE_ADDRFIELD_FIRSTNAME,
             AEE_ADDRFIELD_LASTNAME,
             AEE_ADDRFIELD_NICKNAME,
             AEE_ADDRFIELD_PHONE_GENERIC,
             AEE_ADDRFIELD_PHONE_WORK,
             AEE_ADDRFIELD_PHONE_HOME,
             AEE_ADDRFIELD_PHONE_CELL,
             AEE_ADDRFIELD_PHONE_PAGER,
             AEE_ADDRFIELD_EMAIL };
  uint32 usedAddrFieldListIndex = ARR_SIZE(addrFieldList); /* init to high value */
  int fieldIndex = -1;
  int i,j;
  int wTotalFields;
  AEEAddrField        *addrField = NULL;


  wTotalFields = IADDRREC_GetFieldCount(addrRec);
  
  /* While there are still fields to search, and still something
   * better out there, keep searching. */
  for (i=0; (i < wTotalFields) && (usedAddrFieldListIndex != 0); i++)
  {
    addrField = IADDRREC_GetField(addrRec,i);

    /* If we got a field, look at the contents for a name. */
    if(addrField)
    {
      /* First check to see if this field is in the list above. */
      for (j=0; j < usedAddrFieldListIndex; j++)
      {
        /* If its in the list, save it as fieldIndex for later
         * as well as in usedAddrFieldListIndex for its priority. */
        if (addrField->fID == addrFieldList[j])
        {
          usedAddrFieldListIndex = j; /* This breaks us out of the for loop */
          fieldIndex = i;             /* Saves the field index to use. */
        }
      }

      /* If we didn't yet find a field Index, see if this field can be one */
      if ((fieldIndex != -1) &&
          ((addrField->fType == AEEDB_FT_STRING) ||
           (addrField->fType == AEEDB_FT_PHONE)))
      {
        /* not yet found a field */
        /* store information about any other string field */
        fieldIndex = i; /* Without anything better, any string will do. */
      }
    }
  }

  /* If we found a field, get it and use it. */
  if (fieldIndex != -1) {
    addrField = IADDRREC_GetField(addrRec,fieldIndex);
    return (AECHAR *)addrField->pBuffer;
  }

  /* If not, return NULL */
  MSG_HIGH("Contact name relevent fields are empty",0,0,0);
  return (AECHAR *)NULL;
}

/*=============================================================================
FUNCTION: ContDlgHandler_LoadContactList

DESCRIPTION:
   Loads a contact list, based on the current value of pMe->m_contactListType
   into the IVector pMe->m_contact_iv.

   Returns TRUE on success.  FALSE if an error occured (out of memory)

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
   boolean: TRUE if successful otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ContDlgHandler_LoadContactList(CContactApp *pMe)
{
   boolean              addMe;
   boolean              OOM = FALSE;  // out of memory
   ContactInfo         *ci = NULL;
   AECHAR              *searchStr = NULL;
   uint16               searchStrLen = 0;
   AEEAddrField        *addrField = NULL;
   AEEAddrFieldID       wFieldID = AEE_ADDRFIELD_NONE;
   IAddrRec            *addrRec=NULL;
   AEEAddrCat           category = AEE_ADDR_CAT_NONE;
   AEEAddrCat           searchCat = AEE_ADDR_CAT_NONE;
   AECHAR              *contact_name = NULL;
   uint16               wSpeedDialSlotIndex = 0;
   int                  j;
   int                  wTotalFields;
   boolean              hasVoiceTag = FALSE;
   uint32               flags = 0x0;
   AECHAR               szNameUnknown[FLDLEN_NAME + 1];

   if (pMe->m_availRecInit) {
      flags |= ADDRREC_ENUMERATE_AVAILABLE;
      pMe->m_availRecInit = FALSE;
   }

   ContDlgHandler_FreeContactList(pMe,pMe->m_contact_iv);

   (void) IVector_EnsureCapacity(pMe->m_contact_iv,
                                 IADDRBOOK_GetNumRecs(pMe->m_addr_book));

   switch (pMe->m_contactListType) {
      case STR_CONTACTLIST_SEARCH :
         searchStr = WSTRDUP(pMe->m_searchStr);
         if (NULL == searchStr) {
            return FALSE;
         }
         WSTRLOWER(searchStr);
         //Search on name field
         wFieldID = AEE_ADDRFIELD_NAME;
         break;
      case STR_CONTACTLIST_PERSONAL:
         searchCat = AEE_ADDR_CAT_PERSONAL;
         break;
      case STR_CONTACTLIST_BUSINESS:
         searchCat = AEE_ADDR_CAT_BUSINESS;
         break;

#ifdef FEATURE_MMGSDI
      case STR_CONTACTLIST_RUIM_ADN:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        if (pMe->m_uimSlot1)
         searchCat = AEE_ADDR_CAT_SIM_ADN;
        else
         searchCat = AEE_ADDR_CAT_SIM_ADN2;
#else
         searchCat = AEE_ADDR_CAT_SIM_ADN;
#endif
         break;

      case STR_CONTACTLIST_RUIM_FDN:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        if (!(pMe->m_uimSlot1))
         searchCat = AEE_ADDR_CAT_SIM_FDN2;
        else
#endif
         searchCat = AEE_ADDR_CAT_SIM_FDN;
         break;
      case STR_CONTACTLIST_SDN:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        if (!(pMe->m_uimSlot1))
         searchCat = AEE_ADDR_CAT_SIM_SDN2;
        else
#endif
         searchCat = AEE_ADDR_CAT_SIM_SDN;
         break;
#endif /* FEATURE_MMGSDI */

      case STR_CONTACTLIST_SPEEDDIAL:
         searchCat = AEE_ADDR_CAT_SPEEDDIAL;
         break;
      case STR_CONTACTLIST_FINDNUMBER :
         searchStr = WSTRDUP(pMe->m_searchStr);
         if (NULL == searchStr) {
            return FALSE;
         }
         wFieldID = AEE_ADDRFIELD_NONE;
         flags = ADDRREC_SEARCHTYPE_CONTAINS | ADDRREC_FIELDID_PHONE;
         break;

         case STR_CONTACTLIST_SELECTCONTACT:
      case STR_CONTACTLIST_ALLCONTACTS:
        searchCat = AEE_ADDR_CAT_NONE;
        break;

      default:
         MSG_ERROR("Unrecognized contact list type %d", pMe->m_contactListType,
                                                         0, 0);
         return FALSE;
   }

   MSG_MED("Loading contact list", 0, 0, 0);

   if (searchStr!=NULL) {
      searchStrLen=(uint16)WSTRSIZE(searchStr);
   }

   //Start searching the address book with the search string
   //and search field.
   if ((pMe->m_RecInitRetVal = IADDRBOOKEXT_EnumRecInitEx(pMe->m_addr_book,searchCat,
                                                          wFieldID,searchStr,
                                                          searchStrLen, flags)) ==SUCCESS) {
      addrRec = IADDRBOOK_EnumNextRec(pMe->m_addr_book);
   }

   while (addrRec != NULL) {
      addMe    = FALSE;
      category = AEE_ADDR_CAT_NONE;
      wSpeedDialSlotIndex = 0;
      hasVoiceTag = FALSE;

      //Get the category of the current record
      category = IADDRREC_GetCategory(addrRec);

      //
      // Ignore FDN records except when viewing the FDN category
      //
      if (((AEE_ADDR_CAT_SIM_FDN == category) || (AEE_ADDR_CAT_SIM_FDN2 == category)) &&
        (STR_CONTACTLIST_RUIM_FDN != pMe->m_contactListType) ) {
         // free the current record and get the next record
         IADDRREC_Release(addrRec);
         addrRec = NULL;
         addrRec = IADDRBOOK_EnumNextRec(pMe->m_addr_book);
         continue;
      }

      //Get the name of the Contact from the record
      contact_name = ContDlgHandler_GetContactName(pMe, addrRec);
      if (contact_name == NULL) {
        /* no appropriate string was found */
        (void) ISHELL_LoadResString(pMe->m_pShell,
                                    CONTACTAPP_RES_FILE,
                                    STR_CONTACTLIST_NONAME,
                                    szNameUnknown,
                                    sizeof(szNameUnknown));
        contact_name = szNameUnknown;
      }
      
      wTotalFields = IADDRREC_GetFieldCount(addrRec);
      
      //Speed Dial has Index and VT that we care about.
      if(STR_CONTACTLIST_SPEEDDIAL == pMe->m_contactListType){
        for (j=0;j< wTotalFields;j++) {
          addrField = IADDRREC_GetField(addrRec,j);
          if (addrField) {
            if (addrField->fID == AEE_ADDRFIELD_INDEX) {
              wSpeedDialSlotIndex = *((uint16 *)addrField->pBuffer);
              continue;
            }
            if (addrField->fID == AEE_ADDRFIELD_VOICETAG) {
              hasVoiceTag = TRUE;
            }
          }
        }
      }

      switch (pMe->m_contactListType) {
         case STR_CONTACTLIST_SEARCH:
         case STR_CONTACTLIST_FINDNUMBER:
         case STR_CONTACTLIST_ALLCONTACTS:
            if(AEE_ADDR_CAT_SPEEDDIAL != category){
               addMe = TRUE;
            }
            break;

         case STR_CONTACTLIST_SELECTCONTACT:
#ifdef FEATURE_MMGSDI
#ifdef FEATURE_MMGSDI_DUAL_SLOT
            if( (AEE_ADDR_CAT_SIM_ADN != category) &&
                (AEE_ADDR_CAT_SIM_ADN2 != category) &&
                (AEE_ADDR_CAT_SPEEDDIAL != category))
#else
            if( (AEE_ADDR_CAT_SIM_ADN != category) &&
                (AEE_ADDR_CAT_SPEEDDIAL != category))
#endif // FEATURE_MMGSDI_DUAL_SLOT
#else
            if(AEE_ADDR_CAT_SPEEDDIAL != category)
#endif /* FEATURE_MMGSDI*/
            {
               addMe = TRUE;
            }
            break;

         case STR_CONTACTLIST_PERSONAL:
            if (AEE_ADDR_CAT_PERSONAL == category) {
               addMe = TRUE;
            }
            break;

         case STR_CONTACTLIST_BUSINESS:
            if (AEE_ADDR_CAT_BUSINESS== category) {
               addMe = TRUE;
            }
            break;

#ifdef FEATURE_MMGSDI
         case STR_CONTACTLIST_RUIM_ADN:
         if ((AEE_ADDR_CAT_SIM_ADN == category) || (AEE_ADDR_CAT_SIM_ADN2 == category)) {
               addMe = TRUE;
            }
            break;

         case STR_CONTACTLIST_RUIM_FDN:
         if ((AEE_ADDR_CAT_SIM_FDN == category) || (AEE_ADDR_CAT_SIM_FDN2 == category)) {
               addMe = TRUE;
            }
            break;
         case STR_CONTACTLIST_SDN:
            if ((AEE_ADDR_CAT_SIM_SDN == category) || (AEE_ADDR_CAT_SIM_SDN2 == category)) 
            {
               addMe = TRUE;
            }
            break;
#endif /* FEATURE_MMGSDI */

         case STR_CONTACTLIST_SPEEDDIAL:
            if (AEE_ADDR_CAT_SPEEDDIAL== category) {
               addMe = TRUE;
            }
            break;

         default:
            break;
      }

      if (TRUE == OOM) {
         break;
      }

      if (FALSE == addMe) {
         contact_name = NULL;

         // free the current record and get the next record
         IADDRREC_Release(addrRec);
         addrRec = NULL;
         addrRec = IADDRBOOK_EnumNextRec(pMe->m_addr_book);
         continue;
      }

      // If it is contact list number search, look in address record returned
      // by address book for type of phone and if more than one type
      // of phone matches the search string
      if (STR_CONTACTLIST_FINDNUMBER == pMe->m_contactListType) {
         for (j=0;j< wTotalFields;j++) {
            addrField = IADDRREC_GetField(addrRec,j);
            if (addrField == NULL) {
               // No more fields...
               break;
            }
            if ((addrField->fType !=AEEDB_FT_PHONE ) ||
                (!ContDlgHandler_WStrStr(addrField->pBuffer, searchStr))) {
               continue;
            }

            // Found a new contact, create a ContactInfo structure for it
            // and add it to the contact list (pMe->m_contact_iv)

            ci = MALLOC(sizeof(*ci));
            if (NULL == ci) {
               OOM = TRUE;
               break;
            }

            ci->m_name = WSTRDUP(contact_name);

            if (NULL == ci->m_name && NULL != contact_name) {
               OOM = TRUE;
               break;
            }

            ci->m_id = IADDRREC_GetRecID(addrRec);
            ci->m_class = category;
            ci->m_fldNumberId = (uint16)j;

            switch (addrField->fID) {
               case AEE_ADDRFIELD_PHONE_WORK:
                  ci->m_fldNumberType = NUMBERTYPE_MENUITEM_WORK;
                  break;
               case AEE_ADDRFIELD_PHONE_CELL:
                  ci->m_fldNumberType = NUMBERTYPE_MENUITEM_MOBILE;
                  break;

               case AEE_ADDRFIELD_PHONE_GENERIC:
               case AEE_ADDRFIELD_PHONE_OTHER:
                  switch(category) {
                    case AEE_ADDR_CAT_SIM_ADN:
                    case AEE_ADDR_CAT_SIM_FDN:
                    case AEE_ADDR_CAT_SIM_ADN2:
                    case AEE_ADDR_CAT_SIM_FDN2:
                    case AEE_ADDR_CAT_SIM_SDN:
                    case AEE_ADDR_CAT_SIM_SDN2:
                      ci->m_fldNumberType = NUMBERTYPE_RUIM;
                      break;
                    case AEE_ADDR_CAT_SPEEDDIAL:
                      ci->m_fldNumberType = NUMBERTYPE_SPEEDDIAL;
                      break;
                    default:
                      ci->m_fldNumberType = NUMBERTYPE_OTHER;
                      break;
                  }
                  break;

               case AEE_ADDRFIELD_PHONE_PAGER:
                  ci->m_fldNumberType = NUMBERTYPE_MENUITEM_PAGER;
                  break;

               default:
               case AEE_ADDRFIELD_PHONE_HOME:
                  ci->m_fldNumberType = NUMBERTYPE_MENUITEM_HOME;
                  break;
            }

            if (SUCCESS != IVector_AddElement(pMe->m_contact_iv, ci)) {
               OOM = TRUE;
               break;
            }
         }

         // if out of memory then no point in going on. Break the outer
         // while loop
         if (OOM==TRUE) {
            break;
         }

      } else {

         // Found a new contact, create a ContactInfo structure for it
         // and add it to the contact list (pMe->m_contact_iv)
         ci = MALLOC(sizeof(*ci));
         if (NULL == ci) {
            OOM = TRUE;
            break;
         }

         //if it is the speed dial list then show the index slot also in the
         //contact list
         if (STR_CONTACTLIST_SPEEDDIAL == pMe->m_contactListType) {

            ci->m_name = WSTRDUP(contact_name);
            ci->m_speedDialIndex = wSpeedDialSlotIndex;

            if (hasVoiceTag)
               ci->m_fldNumberType = NUMBERTYPE_VOICETAG;
            else
               ci->m_fldNumberType = NUMBERTYPE_SPEEDDIAL;

         } else {
            // If we are viewing the FDN numbers, we want to display
            // the actual FDN number rather than the associated alpha
            // tag.
            //
            // We currently don't allow the user to edit the alpha tag so
            // it will probably be empty anyway.
            //
            if (STR_CONTACTLIST_RUIM_FDN == pMe->m_contactListType) {
              for (j=0; j < wTotalFields; j++) {
                addrField = IADDRREC_GetField(addrRec,j);
                if (!addrField) {
                  break;
                }

                if (addrField->fID == AEE_ADDRFIELD_PHONE_GENERIC) {
                  contact_name = ContDlgHandler_GetContactName(pMe, addrRec);
                  if (contact_name == NULL) {
                    /* no appropriate string was found */
                    (void) ISHELL_LoadResString(pMe->m_pShell,
                                                CONTACTAPP_RES_FILE,
                                                STR_CONTACTLIST_NONAME,
                                                szNameUnknown,
                                                sizeof(szNameUnknown));
                    contact_name = szNameUnknown;
                  }
                  break;
                }
              }
            }

            ci->m_name = WSTRDUP(contact_name);
         }

         if (NULL == ci->m_name && NULL != contact_name) {
            OOM = TRUE;
            break;
         }
         ci->m_id = IADDRREC_GetRecID(addrRec);
         ci->m_class = category;

         if (SUCCESS != IVector_AddElement(pMe->m_contact_iv, ci)) {
            OOM = TRUE;
            break;
         }
      }

      ci = NULL;
      contact_name = NULL;

      // free the current record and get the next record
      IADDRREC_Release(addrRec);
      addrRec = NULL;
      addrRec = IADDRBOOK_EnumNextRec(pMe->m_addr_book);
   }

   FREE_STRING(searchStr);
   contact_name = NULL;

   if (TRUE == OOM) {
      if (ci != NULL) {
         if (ci->m_name != NULL) {
            FREE(ci->m_name);
         }
         FREE(ci);
      }
      // free the current record
      if (addrRec) {
         IADDRREC_Release(addrRec);
      }
      return FALSE;
   }

   if (STR_CONTACTLIST_SPEEDDIAL==pMe->m_contactListType)
   {
     IVector_Sort(pMe->m_contact_iv,
                  (IVectorSortCB) ContDlgHandler_Sort_by_Index);
   }
   else
   {
     IVector_Sort(pMe->m_contact_iv,
                  (IVectorSortCB) ContDlgHandler_Sort_by_Name);
   }

   if (pMe->m_RecInitRetVal == SUCCESS)
     return TRUE;
   else
     return FALSE;
}

/*=============================================================================
FUNCTION: ContDlgHandler_LoadSpeedDialList

DESCRIPTION:
   Loads speed dial lists from the phone book into IVector pMe->m_speeddial_iv.

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
   boolean: TRUE if successful otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ContDlgHandler_LoadSpeedDialList(struct _CContactApp *pMe)
{
   boolean              OOM = FALSE;  // out of memory
   ContactInfo         *ci = NULL;
   AEEAddrField        *addrField = NULL;
   IAddrRec            *addrRec=NULL;
   AECHAR              *contact_name = NULL;
   uint16               wSpeedDialSlotIndex = 0;
   int                  wTotalFields=0;
   int                  j;
   int8                 nContactsCount=0;
   boolean              hasVoiceTag;

   //initialize the pointer to char. we will store MAX_SPEED_DIAL_NUMS chars
   //which denotes availablity of each slot
   if(pMe->m_nSpeedDialIndexesEmpty != NULL){
      FREE(pMe->m_nSpeedDialIndexesEmpty);
      pMe->m_nSpeedDialIndexesEmpty=NULL;
   }

   pMe->m_nSpeedDialIndexesEmpty = MALLOC(MAX_SPEED_DIAL_NUMS);
   if(pMe->m_nSpeedDialIndexesEmpty==NULL){
      //should not happen
      return FALSE;
   }else {
      (void)MEMSET(pMe->m_nSpeedDialIndexesEmpty, SPEED_DIAL_EMPTY,
                   MAX_SPEED_DIAL_NUMS);
   }
   ContDlgHandler_FreeContactList(pMe,pMe->m_speeddial_iv);

   (void) IVector_EnsureCapacity(pMe->m_speeddial_iv,
                                 MAX_SPEED_DIAL_NUMS);


   //Start searching the address book with the search string
   //and search field.
   if ((pMe->m_RecInitRetVal = IADDRBOOK_EnumRecInit(pMe->m_addr_book,
                             AEE_ADDR_CAT_SPEEDDIAL,
                             AEE_ADDRFIELD_NONE,
                             NULL,
                             0)) ==SUCCESS) {
      addrRec = IADDRBOOK_EnumNextRec(pMe->m_addr_book);
   }

   while (addrRec != NULL) {

      //Get the name of the Contact from the record
      contact_name = NULL;
      wSpeedDialSlotIndex = 0;
      wTotalFields = IADDRREC_GetFieldCount(addrRec);

      hasVoiceTag = FALSE;

      for (j=0;j< wTotalFields;j++) {
         addrField = IADDRREC_GetField(addrRec,j);
         if (addrField) {
            if (addrField->fID == AEE_ADDRFIELD_NAME) {
               contact_name = (AECHAR *) addrField->pBuffer;
            }
            if (addrField->fID == AEE_ADDRFIELD_INDEX) {
               wSpeedDialSlotIndex = *((uint16 *)addrField->pBuffer);
            }
            if (addrField->fID == AEE_ADDRFIELD_VOICETAG) {
               hasVoiceTag = TRUE;
            }
         }
      }

      //we start at index 1.
      if(wSpeedDialSlotIndex >=  SPEED_DIAL_USER_START_INDEX) {
         ci = MALLOC(sizeof(*ci));
         if (NULL == ci) {
            OOM = TRUE;
            break;
         }

         ci->m_name = WSTRDUP(contact_name);
         ci->m_speedDialIndex = wSpeedDialSlotIndex;

         if (NULL == ci->m_name) {
            OOM = TRUE;
            break;
         }

         ci->m_id          = IADDRREC_GetRecID(addrRec);
         ci->m_fldNumberId = wSpeedDialSlotIndex;

         if (hasVoiceTag)
            ci->m_fldNumberType = NUMBERTYPE_VOICETAG;
         else
            ci->m_fldNumberType = NUMBERTYPE_SPEEDDIAL;

         if( nContactsCount < MAX_SPEED_DIAL_NUMS) {
            uint16 nIndex = wSpeedDialSlotIndex-SPEED_DIAL_USER_START_INDEX;
            pMe->m_nSpeedDialIndexesEmpty[nIndex]=nContactsCount;
         }

         if (SUCCESS != IVector_AddElement(pMe->m_speeddial_iv, ci)) {
            OOM = TRUE;
            break;
         }
         /* Increment count only if we are adding it to the vector */
         nContactsCount++;
      }

      //ci = NULL;
      contact_name = NULL;

      // free the current record and get the next record
      IADDRREC_Release(addrRec);
      addrRec = NULL;
      addrRec = IADDRBOOK_EnumNextRec(pMe->m_addr_book);
   }

   if (TRUE == OOM) {
      if (ci != NULL) {
         if (ci->m_name != NULL) {
            FREE(ci->m_name);
         }
         FREE(ci);
      }
      // free the current record
      if (addrRec) {
         IADDRREC_Release(addrRec);
      }
      return FALSE;
   }
   return TRUE;
}



/*=============================================================================
FUNCTION: ContDlgHandler_FreeContactList

DESCRIPTION:
   Deletes all the contacts in the contacts list in Cache.

PARAMETERS:
   *pMe: pointer to CContactApp object
   *pIV : pointer to the Vector which holds the contact info

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ContDlgHandler_FreeContactList(CContactApp *pMe,
                                    IVector     *pIV)
{
   ContactInfo *ci;

   if ((pMe == NULL) || (pIV == NULL))
   {
      return;
   }

   // free the data first
   ci = IVector_RemoveElementAt(pIV, 0);
   while (NULL != ci)
   {
      FREE_CONTACTINFO(*ci);
      FREE(ci);
      ci = IVector_RemoveElementAt(pIV, 0);
   }

}

#ifdef FEATURE_MMGSDI
/*=============================================================================
FUNCTION: ContDlgHandler_RUIM_Clear

DESCRIPTION:
   Delete the record from the RUIM card. the record to be deleted is got from
   the pMe->m_ruimDlg_menuSel variable which is index in m_contact_iv

PARAMETERS:
   pMe: pointer to Contact applet


RETURN VALUE:
   SUCCESS: If it successfully deleted the contact on RUIM
   EFAILED: If not successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int ContDlgHandler_RUIM_Clear(CContactApp *pMe)

{
   //Get the record from the address book and delete it.
   ContactInfo *ci;
   IAddrRec *addrRec= NULL;

   ci = IVector_ElementAt(pMe->m_contact_iv,
                          pMe->m_ruimDlg_menuSel);
   if (ci == NULL) {
      return EFAILED;
   }

   //Remove the record from the RUIM
   addrRec = IADDRBOOK_GetRecByID(pMe->m_addr_book,
                                  ci->m_id);
   if (addrRec != NULL) {
      return IADDRREC_RemoveRec(addrRec);
   }
   return EFAILED;
}

/*=============================================================================
FUNCTION: ContDlgHandler_RUIM_Set

DESCRIPTION:
   Sets the name and phone number on the RUIM

PARAMETERS:
   pMe: pointer to Contact applet
   name: name of the contact to store on RUIM
   phone: phone number of the contact to store on RUIM

RETURN VALUE:
   SUCCESS: If it successfully added the contact on RUIM
   EFAILED: If not successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int ContDlgHandler_RUIM_Set(CContactApp *pMe,
                            AECHAR *name,
                            AECHAR *phone)
{
   AEEAddrField fields[3];
   IAddrRec    *addrRec = NULL;
   boolean      retVal = TRUE;

   int          num_fields = 0;

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
   int nStrLen;
   int nSSRetVal;
   int nOperation;
   ISUPPSVC *pISUPPSVC = NULL;
   uint8 ton_npi;

#endif /* FEATURE_GSM || FEATURE_WCDMA */

   //Copy the contact name in the first field
   retVal = ContDlgHandler_ContactInfo2AEEAddrField(pMe,
                                                  &fields[0],
                                                  FALSE);
   if (retVal == FALSE)
   {
     return EFAILED;
   }
   //copy the other field ie. phone number, email,etc. in the
   //second field
   retVal =  ContDlgHandler_FieldInfo2AEEAddrField(&pMe->m_fieldInfo,
                                                &fields[1],
                                                FALSE);
   if (retVal == FALSE)
   {
     return EFAILED;
   }
 
   if (pMe->m_fieldInfo.m_type == STR_FIELDINPUT_NUMBER)
   {
      fields[1].fID = AEE_ADDRFIELD_PHONE_GENERIC;
   }

   num_fields = 2;

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
   if (pMe->m_fieldInfo.m_type == STR_FIELDINPUT_NUMBER)
   {
     nStrLen = WSTRLEN(pMe->m_fieldInfo.m_name);
     if(nStrLen != 0) 
     {
       nSSRetVal = ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_SUPPSVC,
                                         (void **)&pISUPPSVC );
       if (( nSSRetVal != SUCCESS) || (pISUPPSVC == NULL))
       {
         MSG_ERROR("Unable to create ISUPPSVC %d", nSSRetVal, 0, 0);
       }
       else
       {
         nSSRetVal = ISUPPSVC_StringCheck(pISUPPSVC, nStrLen,
                                          pMe->m_fieldInfo.m_name, &nOperation);

         (void) ISUPPSVC_Release(pISUPPSVC);

         if(nSSRetVal == AEESUPPSVC_IS_SS) 
         {
           ton_npi = 0xFF;

           fields[num_fields].fID = AEE_ADDRFIELD_GSM_TONNPI;
           fields[num_fields].fType = AEEDB_FT_BYTE; 
           fields[num_fields].wDataLen = sizeof(ton_npi);
           fields[num_fields].pBuffer = &ton_npi;

           num_fields++;
         }
       }
     }
   }
 #endif /* FEATURE_GSM || FEATURE_WCDMA */

   //create the record
#ifdef FEATURE_MMGSDI_DUAL_SLOT
   if (pMe->m_contactListType == STR_CONTACTLIST_RUIM_FDN) {
      if(pMe->m_uimSlot1)
         addrRec = IADDRBOOK_CreateRec(pMe->m_addr_book,
                                       AEE_ADDR_CAT_SIM_FDN,
                                       fields,
                                       num_fields);
      else
         addrRec = IADDRBOOK_CreateRec(pMe->m_addr_book,
                                       AEE_ADDR_CAT_SIM_FDN2,
                                       fields,
                                       num_fields);

   } else {
      if(pMe->m_uimSlot1)
         addrRec = IADDRBOOK_CreateRec(pMe->m_addr_book,
                                       AEE_ADDR_CAT_SIM_ADN,
                                       fields,
                                       num_fields);
      else
         addrRec = IADDRBOOK_CreateRec(pMe->m_addr_book,
                                       AEE_ADDR_CAT_SIM_ADN2,
                                       fields,
                                       num_fields);
   }
#else
   if (pMe->m_contactListType == STR_CONTACTLIST_RUIM_FDN) {
      addrRec = IADDRBOOK_CreateRec(pMe->m_addr_book,
                                    AEE_ADDR_CAT_SIM_FDN,
                                    fields,
                                    num_fields);
   } else {
      addrRec = IADDRBOOK_CreateRec(pMe->m_addr_book,
                                    AEE_ADDR_CAT_SIM_ADN,
                                    fields,
                                    num_fields);
   }
#endif
   if (addrRec == NULL) {
      return EFAILED;
   }
   //initialise the newly added record so that it can used in the
   //view contact screen
   pMe->m_new_rec_id = IADDRREC_GetRecID(addrRec);
   IADDRREC_Release(addrRec);

   return SUCCESS;
}

/*=============================================================================
FUNCTION: ContDlgHandler_RUIM_Get

DESCRIPTION:
   Gets the name and phone number on the RUIM

PARAMETERS:
   pMe: pointer to Contact applet
   name: name of the contact retrieved from RUIM
   phone: phone number of the contact retrieved from RUIM

RETURN VALUE:
   SUCCESS: If it successfully retrieved the contact on RUIM
   EFAILED: If not successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int ContDlgHandler_RUIM_Get(CContactApp *pMe,
                            AECHAR *name,
                            AECHAR *phone)
{
   uint16        j=0;
   int           wTotalFields = 0;
   AEEAddrField *addrField = NULL;
   IAddrRec     *addrRec = NULL;
   ContactInfo  *ci;

   name  = NULL;
   phone = NULL;

   //Get the record from the address.

   ci = IVector_ElementAt(pMe->m_contact_iv,
                          pMe->m_ruimDlg_menuSel);
   if (ci == NULL) {
      return EFAILED;
   }

   addrRec = IADDRBOOK_GetRecByID(pMe->m_addr_book,
                                  ci->m_id);
   if (addrRec == NULL) {
      return EFAILED;
   }

   //Get the name of the Contact from the record
   wTotalFields = IADDRREC_GetFieldCount(addrRec);

   for (j=0;j< wTotalFields;j++) {
      addrField = IADDRREC_GetField(addrRec,j);
      if (addrField) {
         if (addrField->fID == AEE_ADDRFIELD_NAME) {
            name = MALLOC(addrField->wDataLen);
            if (name==NULL) {
               return(ENOMEMORY);
            }
            MEMCPY(name,addrField->pBuffer,addrField->wDataLen);
         }
         if (addrField->fID == AEE_ADDRFIELD_PHONE_GENERIC) {
            phone = MALLOC(addrField->wDataLen);
            if (phone==NULL) {
               return(ENOMEMORY);
            }
            MEMCPY(phone,addrField->pBuffer,addrField->wDataLen);
         }
      }
   }

   if ((name==NULL) ||(phone==NULL)) {
      return(ENOMEMORY);
   }
   IADDRREC_Release(addrRec);
   return SUCCESS;
}

#endif /*FEATURE_MMGSDI*/

#ifdef FEATURE_PHONE_VR
/*=============================================================================
FUNCTION: ContDlgHandler_VoiceTag_Create

DESCRIPTION:
   Creates a Voice Tag

PARAMETERS:
   pMe: pointer to Contact applet

RETURN VALUE:
   SUCCESS: If training of Voice Tag was successful
   EFAILED: If not successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int ContDlgHandler_VoiceTag_Create(CContactApp *pMe)
{
  if (pMe->m_pVR == NULL)
    return EFAILED;

  IVR_RegisterNotify( pMe->m_pVR,
                      (PFNVRSTATUS) ContDlgHandler_VoiceTag_Callback,
                      pMe );

  IVR_TrainVoiceTag(pMe->m_pVR, NULL);

  return SUCCESS;
}

/*=============================================================================
FUNCTION: ContDlgHandler_VoiceTag_Remove

DESCRIPTION:
   Removes Voice Tag from IVR database

PARAMETERS:
   pMe: pointer to Contact applet
   wUtt: Utterance (Vocie Tag) to be removed.

RETURN VALUE:
   SUCCESS: If successful
   EFAILED: If not successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int ContDlgHandler_VoiceTag_Remove(CContactApp *pMe, AEEVRUtterance wUtt)
{
  if (pMe->m_pVR == NULL)
    return EFAILED;

  return IVR_RemoveVoiceTag(pMe->m_pVR, wUtt);
}

/*=============================================================================
FUNCTION: ContDlgHandler_RecRemove_VoiceTag

DESCRIPTION:
    Checks if the record contains any field which is a voice tag. If so,
    removes the voice tag from the IVR database. Called when the record
    is about to be removed.

PARAMETERS:
   pMe: pointer to Contact applet
   pAddrRec: pointer to the record to be removed.

RETURN VALUE:
   SUCCESS: If voice tag not found, or
            found and remove was successful
   EFAILED: Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int ContDlgHandler_RemoveRec_VoiceTag(CContactApp *pMe, IAddrRec *pAddrRec)
{
  int     wTotalFields;
  uint16  j;
  AEEAddrField *addrField = NULL;

  wTotalFields = IADDRREC_GetFieldCount(pAddrRec);
  for ( j = 0; j < wTotalFields; j++) {
    addrField = IADDRREC_GetField(pAddrRec, j);
    if (addrField && (addrField->fID == AEE_ADDRFIELD_VOICETAG))
      return ContDlgHandler_VoiceTag_Remove(pMe,
                                *((AEEVRUtterance *)addrField->pBuffer));
  }

  return SUCCESS;
}

/*=============================================================================
FUNCTION: ContDlgHandler_VoiceTag_Callback

DESCRIPTION:
   Callback function registered using IVR_RegisterNotify to get the result
   of the asynchronous IVR_TrainVoiceTag function.

PARAMETERS:
   pMe: pointer to Contact applet
   status: status of IVR function
   failure: failure type
   wUtt: Utterance if training is successful.

RETURN VALUE:
   NONE

COMMENTS:
   The events generated by this callback function is handled by
   ContDlgHandler_VoiceTagDialogEvent.

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
static void ContDlgHandler_VoiceTag_Callback(void *pCContactApp,
                                             AEEVRStatus status,
                                             AEEVRFailure failure,
                                             AEEVRUtterance wUtt)
{
  CContactApp *pMe = (CContactApp *) pCContactApp;

  switch (status) {
    case AEEVRSTATUS_DONE:
      if (wUtt != AEEVRUTTERANCE_NONE) {
         pMe->m_voiceTag = wUtt;
         (void) ISHELL_PostEvent(pMe->m_pShell,
                           AEECLSID_CONTACT_APP,
                           EVT_USER,
                           CONT_EVENT_VR_DONE,
                           0);
      }
      break;

    case AEEVRSTATUS_RUNNING:
      (void) ISHELL_PostEvent(pMe->m_pShell,
                           AEECLSID_CONTACT_APP,
                           EVT_USER,
                           CONT_EVENT_VR_RUNNING,
                           0);
      break;

    case AEEVRSTATUS_FAILED:
      (void) ISHELL_PostEvent(pMe->m_pShell,
                           AEECLSID_CONTACT_APP,
                           EVT_USER,
                           CONT_EVENT_VR_FAILED,
                           0);
      break;

    case AEEVRSTATUS_ABORTED:
      if (!pMe->m_userAbort) {
         (void) ISHELL_PostEvent(pMe->m_pShell,
                           AEECLSID_CONTACT_APP,
                           EVT_USER,
                           CONT_EVENT_VR_ABORTED,
                           0);
      }
      break;

    default:
      break;
  }
//lint -save -esym(715, failure) Suppress unusued arg
}
//lint +esym(715, failure)

#endif /* FEATURE_PHONE_VR */

/*=============================================================================
FUNCTION: ContDlgHandler_SpeedDial_Set

DESCRIPTION:
   Sets the name,phone number and index for the speed dial on the Database

PARAMETERS:
   pMe: pointer to Contact applet
   name: name of the contact to store
   phone: phone number of the contact
   wSlotID : slot index range 1 to 99

RETURN VALUE:
   SUCCESS: If it successfully added to the speed dial list
   EFAILED: If not successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int ContDlgHandler_SpeedDial_Set(CContactApp *pMe,
                                 AECHAR *name,
                                 AECHAR *phone,
                                 uint16  wSlotID,
                                 boolean hasVoiceTag)
{
   AEEAddrField fields[4];
   IAddrRec    *addrRec = NULL;
   boolean      OOM = FALSE;

   //Convert name and address to AEEAddrField
   fields[0].fID = AEE_ADDRFIELD_NAME;
   fields[0].fType = AEEDB_FT_STRING;
   fields[0].wDataLen = (uint16) WSTRSIZE(name);
   fields[0].pBuffer = MALLOC(fields[0].wDataLen);
   if(fields[0].pBuffer!=NULL){
      (void)MEMCPY(fields[0].pBuffer,name, fields[0].wDataLen);
   } else {
      OOM = TRUE;
   }


   fields[1].fID = AEE_ADDRFIELD_INDEX;
   fields[1].fType = AEEDB_FT_WORD;
   fields[1].wDataLen = sizeof(wSlotID);
   fields[1].pBuffer = MALLOC(fields[1].wDataLen);
   if(fields[1].pBuffer!=NULL){
      (void)MEMCPY(fields[1].pBuffer, &wSlotID, fields[1].wDataLen);
   } else {
      OOM = TRUE;
   }

   fields[2].fID = AEE_ADDRFIELD_PHONE_OTHER;
   fields[2].fType = AEEDB_FT_PHONE;
   fields[2].wDataLen = (uint16) WSTRSIZE(phone);
   fields[2].pBuffer = MALLOC(fields[2].wDataLen);
   if(fields[2].pBuffer!=NULL){
      (void)MEMCPY(fields[2].pBuffer,phone, fields[2].wDataLen);
   } else {
      OOM = TRUE;
   }

#ifdef FEATURE_PHONE_VR
   if (hasVoiceTag) {
      fields[3].fID = AEE_ADDRFIELD_VOICETAG;
      fields[3].fType = AEEDB_FT_DWORD;
      fields[3].wDataLen = sizeof(pMe->m_voiceTag);
      fields[3].pBuffer = MALLOC(fields[3].wDataLen);
      if (fields[3].pBuffer!=NULL){
         MEMCPY(fields[3].pBuffer, &pMe->m_voiceTag, fields[3].wDataLen);
      } else {
         OOM = TRUE;
      }
   }
#endif

   //create the record
   if(OOM !=TRUE){
      addrRec = IADDRBOOK_CreateRec(pMe->m_addr_book,
                                    AEE_ADDR_CAT_SPEEDDIAL,
                                    fields,
                                    hasVoiceTag?4:3);
   }
   if (addrRec == NULL) {
      return EFAILED;
   }
   IADDRREC_Release(addrRec);
   return SUCCESS;
}

/*=============================================================================
FUNCTION: ContDlgHandler_WStrStr

DESCRIPTION:
   Simple widestring strstr() function.  Returns true if needle was
   found in haystack

PARAMETERS:
   *haystack: string in which needle string is searched
   *needle:  string to be searched

RETURN VALUE:
boolean: TRUE if is match is found other wise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_WStrStr(AECHAR *haystack,
                                      AECHAR *needle)
{
   AECHAR *n,*j;

   if ((NULL == needle) || (NULL == haystack))
   {
      MSG_ERROR("NULL pointer argument.", 0, 0, 0);
      return FALSE;
   }

   for (;;) {
      n = needle;
      j = haystack;

      if ((AECHAR)0 == *haystack) {
         return FALSE;
      }

      while ((*j == *n)&&(*j!=(AECHAR)0)) {
         n++;
         j++;
      }
      if ((AECHAR)0 == *n) {
         return TRUE;
      }
      haystack++;
   }
}

#ifdef FEATURE_MMGSDI
/*=============================================================================
FUNCTION: ContDlgHandler_IsCardAvailable

DESCRIPTION:
   Checks if there is a valid card available in the specified UIM slot.

PARAMETERS:
   pMe   : Contact App object pointer
   nSlot : UIM slot

RETURN VALUE:
   boolean
         TRUE  : If there is a valid card
         FALSE : Otherwise

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_IsCardAvailable(CContactApp *pMe,
                                              uint8 nSlot)
{
   uint8 cardStatus;

   if (!pMe || !pMe->m_pICard)
      return FALSE;

   if (SUCCESS != ICARD_GetCardStatusSlot(pMe->m_pICard, nSlot, &cardStatus)) {
      MSG_ERROR("Unable to obtain card status for slot %d", nSlot, 0, 0);
      return FALSE;
   }

   switch (cardStatus) {
      case AEECARD_NO_CARD:
      case AEECARD_INVALID_CARD:
      case AEECARD_NOT_INIT:
      case AEECARD_CARD_REINIT:
      case AEECARD_NOT_READY:
         return FALSE;

      case AEECARD_AVAIL_CARD:
      case AEECARD_VALID_CARD:
         return TRUE;

      default:
         return FALSE;
   }
}
#endif /* FEATURE_MMGSDI */

/*=============================================================================
                        DIALOG HANDLERS START HERE
=============================================================================*/


/*=============================================================================
FUNCTION: ContDlgHandler_MainDialogEvent

DESCRIPTION:
   Dialog events Handler for the main dialog of contacts applet

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_MainDialogEvent(CContactApp *pMe,
                                              AEEEvent     eCode,
                                              uint16       wParam,
                                              uint32       dwParam)
{
   PARAM_NOT_REF(dwParam)
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  MAIN_MENU);
   if (NULL == menu) {
      return FALSE;
   }

   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
#ifndef FEATURE_MMGSDI_DUAL_SLOT
        {
         (void) IMENUCTL_DeleteItem(menu,
                                    MAIN_MENUITEM_RUIM_FDN2);
         (void) IMENUCTL_DeleteItem(menu,
                                    MAIN_MENUITEM_ADDTOFDN2);
         (void) IMENUCTL_DeleteItem(menu,
                                    MAIN_MENUITEM_RUIM_ADN2);
         (void) IMENUCTL_DeleteItem(menu,
                                    MAIN_MENUITEM_ADDTORUIM_2);
         (void) IMENUCTL_DeleteItem(menu,
                                    MAIN_MENUITEM_RUIM_SDN2);
         }
#else
        {
           if (!ContDlgHandler_IsCardAvailable(pMe, AEECARD_SLOT2)) {
               (void) IMENUCTL_DeleteItem(menu,
                                          MAIN_MENUITEM_RUIM_FDN2);
               (void) IMENUCTL_DeleteItem(menu,
                                          MAIN_MENUITEM_ADDTOFDN2);
               (void) IMENUCTL_DeleteItem(menu,
                                          MAIN_MENUITEM_RUIM_ADN2);
               (void) IMENUCTL_DeleteItem(menu,
                                          MAIN_MENUITEM_ADDTORUIM_2);
               (void) IMENUCTL_DeleteItem(menu,
                                          MAIN_MENUITEM_RUIM_SDN2);
           }
        }
#endif
#ifndef FEATURE_MMGSDI
         (void) IMENUCTL_DeleteItem(menu,
                                    MAIN_MENUITEM_RUIM_ADN);
         (void) IMENUCTL_DeleteItem(menu,
                                    MAIN_MENUITEM_ADDTORUIM);
         (void) IMENUCTL_DeleteItem(menu,
                                    MAIN_MENUITEM_RUIM_FDN);
         (void) IMENUCTL_DeleteItem(menu,
                                    MAIN_MENUITEM_ADDTOFDN);
         (void) IMENUCTL_DeleteItem(menu,
                                    MAIN_MENUITEM_RUIM_SDN);

#else
        {
           if (!ContDlgHandler_IsCardAvailable(pMe, AEECARD_SLOT1)) {
              (void) IMENUCTL_DeleteItem(menu,
                                        MAIN_MENUITEM_RUIM_ADN);
              (void) IMENUCTL_DeleteItem(menu,
                                        MAIN_MENUITEM_ADDTORUIM);
              (void) IMENUCTL_DeleteItem(menu,
                                        MAIN_MENUITEM_RUIM_FDN);
              (void) IMENUCTL_DeleteItem(menu,
                                        MAIN_MENUITEM_ADDTOFDN);
              (void) IMENUCTL_DeleteItem(menu,
                                        MAIN_MENUITEM_RUIM_SDN);
           }
        }
#endif /* !FEATURE_MMGSDI */

         ContDlgHandler_SetMenuStyle(pMe->m_pShell, menu);
         IMENUCTL_SetSel(menu, pMe->m_mainDlg_menuSel);
         return TRUE;

      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            pMe->m_mainDlg_menuSel = IMENUCTL_GetSel(menu);
         }
         return TRUE;

      case EVT_COMMAND:
         pMe->m_mainDlg_menuSel = IMENUCTL_GetSel(menu);

         switch (wParam)
         {
           case MAIN_MENUITEM_FINDNAME:
             CLOSE_DIALOG(DLGRET_MAIN_FINDNAME)
             break;

           case MAIN_MENUITEM_ALLCONTACTS:
             CLOSE_DIALOG(DLGRET_MAIN_ALLCONTACTS)
             break;

           case MAIN_MENUITEM_BUSINESSCONTACTS:
             CLOSE_DIALOG(DLGRET_MAIN_BIZCONTACTS)
             break;

           case MAIN_MENUITEM_PERSONALCONTACTS:
             CLOSE_DIALOG(DLGRET_MAIN_PERCONTACTS)
             break;

           case MAIN_MENUITEM_ADDNEW:
             CLOSE_DIALOG(DLGRET_MAIN_ADDNEW)
             break;

           case MAIN_MENUITEM_SPEEDDIAL:
             CLOSE_DIALOG(DLGRET_MAIN_SPEEDDIAL)
             break;

#ifdef FEATURE_MMGSDI
           case MAIN_MENUITEM_ADDTORUIM:
           {
#ifdef FEATURE_MMGSDI_DUAL_SLOT
             //set simState to TRUE indicating SIM 1 is being used
             pMe->m_uimSlot1 = TRUE;
#endif
             CLOSE_DIALOG(DLGRET_MAIN_ADDTORUIM)
             break;
           }

           case MAIN_MENUITEM_RUIM_ADN:
           {
#ifdef FEATURE_MMGSDI_DUAL_SLOT
             //set simState to TRUE indicating SIM 1 is being used
             pMe->m_uimSlot1 = TRUE;
#endif
             CLOSE_DIALOG(DLGRET_CONT_MAIN_RUIM_ADN)
             break;
           }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
           case MAIN_MENUITEM_ADDTORUIM_2:
           {
             // set simState to FALSE indicating SIM2 is being used
             pMe->m_uimSlot1 = FALSE;
             CLOSE_DIALOG(DLGRET_MAIN_ADDTORUIM)
             break;
           }

           case MAIN_MENUITEM_RUIM_ADN2:
           {
             // set simState to FALSE indicating SIM2 is being used
             pMe->m_uimSlot1 = FALSE;
             CLOSE_DIALOG(DLGRET_CONT_MAIN_RUIM_ADN)
             break;
           }
#endif

           case MAIN_MENUITEM_ADDTOFDN:
           {
#ifdef FEATURE_MMGSDI_DUAL_SLOT
             //set simState to TRUE indicating SIM 1 is being used
             pMe->m_uimSlot1 = TRUE;
#endif
             CLOSE_DIALOG(DLGRET_MAIN_ADDTOFDN)
             break;
           }

           case MAIN_MENUITEM_RUIM_FDN:
           {
#ifdef FEATURE_MMGSDI_DUAL_SLOT
             //set simState to TRUE indicating SIM 1 is being used
             pMe->m_uimSlot1 = TRUE;
#endif
             CLOSE_DIALOG(DLGRET_CONT_MAIN_RUIM_FDN)
             break;
           }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
           case MAIN_MENUITEM_ADDTOFDN2:
           {
             // set simState to FALSE indicating SIM2 is being used
             pMe->m_uimSlot1 = FALSE;
             CLOSE_DIALOG(DLGRET_MAIN_ADDTOFDN)
             break;
           }

           case MAIN_MENUITEM_RUIM_FDN2:
           {
             // set simState to FALSE indicating SIM2 is being used
             pMe->m_uimSlot1 = FALSE;
             CLOSE_DIALOG(DLGRET_CONT_MAIN_RUIM_FDN)
             break;
           }
#endif
           case MAIN_MENUITEM_RUIM_SDN:
           {
#ifdef FEATURE_MMGSDI_DUAL_SLOT
             //set simState to TRUE indicating SIM 1 is being used
             pMe->m_uimSlot1 = TRUE;
#endif
             CLOSE_DIALOG(DLGRET_CONT_MAIN_RUIM_SDN)
             break;
           }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
           case MAIN_MENUITEM_RUIM_SDN2:
           {
             // set simState to FALSE indicating SIM2 is being used
             pMe->m_uimSlot1 = FALSE;
             CLOSE_DIALOG(DLGRET_CONT_MAIN_RUIM_SDN)
             break;
           }
#endif
#endif /* FEATURE_MMGSDI */

           default:
             MSG_ERROR("Invalid main menu selection, %d.", wParam, 0, 0);
             break;
         }
         return TRUE;

      default:
         break;
   }

   return FALSE;
//lint -save -esym(715, dwParam)  Suppress unused arg
} // ContDlgHandler_MainDialogEvent()
//lint +esym(715, dwParam)



/*=============================================================================
FUNCTION: ContDlgHandler_FindNameDialogEvent

DESCRIPTION:
  Dialog events handler for the Find name dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_FindNameDialogEvent(CContactApp *pMe,
                                                  AEEEvent     eCode,
                                                  uint16       wParam,
                                                  uint32       dwParam)
{
   IMenuCtl *menu;
   ITextCtl *ctrl;

   ctrl = (ITextCtl*)IDIALOG_GetControl(pMe->m_activeDlg, FINDNAME_TEXTINPUT);
   if (NULL == ctrl) {
      return FALSE;
   }

   menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg, FINDNAME_SOFTKEYS);
   if (menu == NULL) {
      return FALSE;
   }

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   switch (eCode) {
      case EVT_DIALOG_INIT:
         pMe->bMultiTap = FALSE;
         return TRUE;


      case EVT_DIALOG_START:
         {
            AEERect textRect, menuRect;
            IMenuCtl *pskMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                           FINDNAME_SOFTKEYS);

            CtlAddItem ai;

            // Build the softkey menu manually.
            ai.pText = NULL;
            ai.pImage = NULL;
            ai.pszResText = ai.pszResImage = CONTACTAPP_RES_FILE;
            ai.wFont = 0;
            ai.dwData = 0;

            (void) IMENUCTL_DeleteAll(pskMenu);
            (void) IMENUCTL_Redraw(pskMenu);

            ai.wText = STR_FINDNAME_SOFTKEY_FIND;
            ai.wImage = ICON_FIND;
            ai.wItemID = FINDNAME_SOFTKEY_FIND;
            (void) IMENUCTL_AddItemEx(pskMenu,
                                      &ai);
            ai.wText = STR_FINDNAME_SOFTKEY_CANCEL;
            ai.wImage = ICON_CANCEL;
            ai.wItemID = FINDNAME_SOFTKEY_CANCEL;
            (void) IMENUCTL_AddItemEx(pskMenu,
                                      &ai);

            ITEXTCTL_SetSoftKeyMenu(ctrl, pskMenu);

            //Remove the multiline property
            ITEXTCTL_SetProperties(ctrl,
                                   ITEXTCTL_GetProperties(ctrl)& ~TP_MULTILINE);
            SetDefaultSoftkeyLook (pMe->m_pShell, pskMenu);

            // Set the text input to the valid of the previous
            // search string (if it exists)
            if (pMe->m_searchStr != NULL) {
               uint32 oldProps = ITEXTCTL_GetProperties(ctrl);
               ITEXTCTL_SetProperties(ctrl, oldProps | TP_NODRAW);

               (void) ITEXTCTL_SetText(ctrl,
                                       pMe->m_searchStr,
                                       -1 /* use string len */);

               ITEXTCTL_SetProperties(ctrl, oldProps);
            }
            else {
               (void) ITEXTCTL_SetText(ctrl,
                                       NULL,
                                       0);
            }
            //set the rect so that it does not clip, the menu control.
            IMENUCTL_GetRect(pskMenu,&menuRect);
            ITEXTCTL_GetRect(ctrl, &textRect);
            textRect.dy = pMe->m_rc.dy - menuRect.dy;
            ITEXTCTL_SetRect(ctrl, &textRect);

            return TRUE;
         }

      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            FREE_STRING(pMe->m_searchStr);
            pMe->m_searchStr = WSTRDUP(ITEXTCTL_GetTextPtr(ctrl));
         }
         return TRUE;

      case EVT_KEY_PRESS:
         pMe->wKeyPressParam = wParam;
         switch ((AVKType)wParam) {
            case AVK_CLR:
               if (WSTRLEN(ITEXTCTL_GetTextPtr(ctrl)) <= 0) {
                  pMe->m_fieldWasEmpty = TRUE;
               } else {
                  pMe->m_fieldWasEmpty = FALSE;
               }
               if (pMe->bMultiTap)
                  pMe->bMultiTap = FALSE;
               break;
            case AVK_UP:
               if (!pMe->bMultiTap) {
                  IMENUCTL_SetActive(menu, FALSE);
                  ITEXTCTL_SetActive(ctrl, FALSE);
                  return TRUE;
               }
               break;
            case AVK_DOWN:
               if (!pMe->bMultiTap) {
                  ITEXTCTL_SetActive(ctrl, FALSE);
                  IMENUCTL_SetActive(menu, FALSE);
                  return TRUE;
               }
               break;
            case AVK_SELECT:
               if (pMe->bMultiTap)
                  pMe->bMultiTap = FALSE;
               break;
            default:
               break;
         }
         return FALSE;

      case EVT_KEY_RELEASE:
         switch ((AVKType)pMe->wKeyPressParam) {
            case AVK_UP:
               if (!pMe->bMultiTap) {
                  IMENUCTL_SetActive(menu, FALSE);
                  ITEXTCTL_SetActive(ctrl, TRUE);
                  return TRUE;
               }
               break;
            case AVK_DOWN:
               if (!pMe->bMultiTap) {
                  ITEXTCTL_SetActive(ctrl, FALSE);
                  IMENUCTL_SetActive(menu, TRUE);
                  return TRUE;
               }
               break;
            default:
               break;
         }
         return FALSE;

      case EVT_COMMAND:
         switch (wParam)
         {
           case FINDNAME_SOFTKEY_CANCEL:
             CLOSE_DIALOG(DLGRET_CONT_CANCELED)
             return TRUE;

           case FINDNAME_SOFTKEY_FIND:
             // save the search string
             FREE_STRING(pMe->m_searchStr);
             pMe->m_searchStr = WSTRDUP(ITEXTCTL_GetTextPtr(ctrl));
             CLOSE_DIALOG(DLGRET_CONT_OK)
             return TRUE;

           default:
             MSG_ERROR("Unexpected EVT_COMMAND, %d.", wParam, 0, 0);
             break;
         }
         return FALSE;

      case EVT_CTL_MENU_OPEN:
         pMe->bMultiTap = TRUE;
         break;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_FindNameDialogEvent()


/*=============================================================================
FUNCTION: ContDlgHandler_ContactListDialogEvent

DESCRIPTION:
    Dialog events handler for the Contacts list dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_ContactListDialogEvent(CContactApp *pMe,
                                                     AEEEvent     eCode,
                                                     uint16       wParam,
                                                     uint32       dwParam)
{
   uint16    wNumLines = 0;
   IMenuCtl *ctrl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  CONTACTLIST_MENU);
#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

#ifndef FEATURE_PHONE_VR
   if (STR_CONTACTLIST_SPEEDDIAL == pMe->m_contactListType) {
      wNumLines = pMe->m_numTextOnlyLines;
   } else {
      wNumLines = pMe->m_numLines;
   }
#else
   wNumLines = pMe->m_numLines;
#endif

   if (NULL == ctrl) {
      return FALSE;
   }

   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         {
#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
            AEERect rc;

            (void) IMENUCTL_SetTitle(ctrl,
                                     CONTACTAPP_RES_FILE,
                                     pMe->m_contactListType,
                                     NULL);
            ContDlgHandler_SetMenuStyle(pMe->m_pShell, ctrl);

            // Hide the scrollbar off the right of the screen.  Since
            // we aren't loading all the menu items it won't be
            // an accurate representation of the current menu position
            IMENUCTL_GetRect(ctrl, &rc);
            rc.dx += 3;
            IMENUCTL_SetRect(ctrl, &rc);

            if (pMe->m_contactSelId > 0) {

               if (pMe->m_contactSelId+1 < IVector_Size(pMe->m_contact_iv)) {
                  // If in the middle of the menu list, start at one
                  // item above the selected item
                  pMe->m_contactListMenuStart = pMe->m_contactSelId - 1;
               } else {
                  if (pMe->m_contactSelId >= IVector_Size(pMe->m_contact_iv)) {
                     pMe->m_contactSelId = 0;
                     pMe->m_contactListMenuStart = 0;
                  } else {
                     int menuStart =
                        (pMe->m_contactSelId - (wNumLines-1)) + 1;

                     if (menuStart < 0) {
                        menuStart = 0;
                     }

                     pMe->m_contactListMenuStart = (uint16)menuStart;
                  }
               }
            } else {
               pMe->m_contactListMenuStart = 0;
            }

            ContDlgHandler_PopulateContactListMenu(pMe,
                                                   ctrl,
                                                   pMe->m_contactListMenuStart,
                                                   wNumLines-1,
                                                   TRUE);

#else
            uint32     i;
            CtlAddItem ai;

            (void) IMENUCTL_SetTitle(ctrl,
                                     CONTACTAPP_RES_FILE,
                                     pMe->m_contactListType,
                                     NULL);


            // init common fields
            ai.pImage = NULL;
            ai.pszResText = NULL;
            ai.pszResImage =  CONTACTAPP_RES_FILE;
            ai.wText = 0;
            ai.wFont = 0;
            ai.dwData = 0;

            // Show the hourglass if we are loading a
            // 'large' number of contacts...IMENUCTL_AddItemEx()
            // takes O(n) time to add a new item (where n is
            // the number of items currently in the menu)
            if (IVector_Size(pMe->m_contact_iv) > 10) {
               ShowBusyIcon(pMe->m_pShell,
                            pMe->m_pDisplay,
                            &pMe->m_rc,
                            FALSE);
            }

            for (i = 0; i < IVector_Size(pMe->m_contact_iv); i++) {
               ContactInfo *ci;

               ci = IVector_ElementAt(pMe->m_contact_iv, i);
               if (NULL == ci)
               {
                  MSG_ERROR("NULL contact info at index %d.", i, 0, 0);
                  continue;
               }

               ai.pText = ci->m_name;

               if (STR_CONTACTLIST_FINDNUMBER == pMe->m_contactListType) {

                  switch (addrField->fID) {
                     case AEE_ADDRFIELD_PHONE_WORK:
                        ai.wImage = ICON_WORKNUMBER;
                        break;
                     case AEE_ADDRFIELD_PHONE_CELL:
                        ai.wImage = ICON_MOBILENUMBER;
                        break;
                     case AEE_ADDRFIELD_PHONE_PAGER:
                        ai.wImage = ICON_PAGERNUMBER;
                        break;
                        //If it is on RUIM
                     case AEE_ADDRFIELD_PHONE_GENERIC:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                      switch(ci->m_class)
                      {
                        case AEE_ADDR_CAT_SIM_ADN:
                        case AEE_ADDR_CAT_SIM_FDN:
                          pai.wImage = ICON_RUIM;
                          break;
                        case AEE_ADDR_CAT_SIM_ADN2:
                        case AEE_ADDR_CAT_SIM_FDN2:
                          pai.wImage = ICON_RUIM2;
                          break;
                        case AEE_ADDR_CAT_SIM_SDN:
                          pai.wImage = ICON_CONTACTCLASS_SDN;
                          break;
                        case AEE_ADDR_CAT_SIM_SDN2:
                          pai.wImage = ICON_CONTACTCLASS_SDN2;
                          break;
                        default:
                          break;
                      }
                      break;
#else
                      switch (ci->m_class)
                      {
                        case AEE_ADDR_CAT_SIM_SDN:
                          ai.wImage = ICON_CONTACTCLASS_SDN;
                          break;
                        default:
                          ai.wImage = ICON_RUIM;
                          break;
                      }
                      break;
#endif
                     case AEE_ADDRFIELD_PHONE_HOME:
                        ai.wImage = ICON_HOMENUMBER;
                        break;
                     default:
                        MSG_ERROR("Unexpected address field, %d.",
                                  addrField->fID, 0, 0);
                        continue;
                  }

               } else {
                  //Get the first number type for this contact info to show that icon
                  (void)ContDlgHandler_GetFirstNumberType(pMe, ci, NULL);

                  switch (ci->m_fldNumberType) {
                     case NUMBERTYPE_MENUITEM_WORK:
                        ai.wImage = ICON_WORKNUMBER;
                        break;
                     case NUMBERTYPE_MENUITEM_HOME:
                        ai.wImage = ICON_HOMENUMBER;
                        break;
                     case NUMBERTYPE_MENUITEM_PAGER:
                        ai.wImage = ICON_PAGERNUMBER;
                        break;
                     case NUMBERTYPE_RUIM:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                      switch(ci->m_class)
                      {
                        case AEE_ADDR_CAT_SIM_ADN:
                        case AEE_ADDR_CAT_SIM_FDN:
                          pai.wImage = ICON_RUIM;
                          break;
                        case AEE_ADDR_CAT_SIM_ADN2:
                        case AEE_ADDR_CAT_SIM_FDN2:
                          pai.wImage = ICON_RUIM2;
                          break;
                        case AEE_ADDR_CAT_SIM_SDN:
                          pai.wImage = ICON_CONTACTCLASS_SDN;
                          break;
                        case AEE_ADDR_CAT_SIM_SDN2:
                          pai.wImage = ICON_CONTACTCLASS_SDN2;
                          break;
                        default:
                          break;
                      }
                      break;
#else
                      switch (ci->m_class)
                      {
                        case AEE_ADDR_CAT_SIM_SDN:
                          ai.wImage = ICON_CONTACTCLASS_SDN;
                          break;
                        default:
                          ai.wImage = ICON_RUIM;
                          break;
                      }
                        break;
#endif
                     case NUMBERTYPE_SPEEDDIAL:
                        ai.wImage = ICON_SPEEDIAL;
                        break;
                     case NUMBERTYPE_VOICETAG:
                        ai.wImage = ICON_SPEEDIAL;
                        break;
                     case NUMBERTYPE_OTHER:
                        ai.wImage = ICON_NUMBER;
                        break;
                     case NUMBERTYPE_MENUITEM_MOBILE:
                        ai.wImage = ICON_MOBILENUMBER;
                        break;
                     default:
                        ai.wImage= ICON_NOPHONE;
                        break;
                  }
               }
               ai.wItemID = (uint16)i;
               if(ai.wImage != NULL){
                  (void) IMENUCTL_AddItemEx(ctrl, &ai);
               } else {
                  (void) IMENUCTL_AddItem(ctrl,
                                          NULL,
                                          0,
                                          ai.wItemID,
                                          ci->m_name,
                                          ci->m_fldNumberId);
               }
            }

            ContDlgHandler_SetMenuStyle(pMe->m_pShell, ctrl);

            IMENUCTL_SetSel(ctrl, pMe->m_contactSelId);
#endif /* defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW) */

#ifdef FEATURE_CONTACTNAME_PREDICT
            pMe->m_namePredictLen = 0;
#endif /* FEATURE_CONTACTNAME_PREDICT */

            //if not search on find number list, populate the m_fld_recs now
            if (STR_CONTACTLIST_FINDNUMBER != pMe->m_contactListType) {
               ContactInfo *ci = NULL;

               (void)ContDlgHandler_LoadAEEAddrFields(pMe);
               ci = IVector_ElementAt(pMe->m_contact_iv,
                                      pMe->m_contactSelId);
               if (ci!=NULL) {
                  pMe->m_contactListFieldID = ci->m_fldNumberId;
               }
            }

            return TRUE;
         }

      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            pMe->m_contactSelId = IMENUCTL_GetSel(ctrl);
         }
         return TRUE;

      case EVT_KEY_PRESS:
         switch ((AVKType)wParam) {
#ifdef FEATURE_CONTACTNAME_PREDICT
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
            case AVK_POUND:
               {
                  uint16  idx;
                  boolean match;

                  idx = IMENUCTL_GetSel(ctrl);

                  if (AVK_POUND == (AVKType)wParam) {
                     idx++;
                     match = ContDlgHandler_DoContactNamePredict(pMe,
                                                                 &idx,
                                                                 TRUE);

                     if (FALSE == match) {
                        // No match, try starting over from the top of the menu
                        match = ContDlgHandler_DoContactNamePredict(pMe,
                                                                    &idx,
                                                                    FALSE);
                     }

                  } else {
                     if (pMe->m_namePredictLen <
                         sizeof(pMe->m_namePredictData)) {
                        pMe->m_namePredictData[pMe->m_namePredictLen++] =
                           (AVKType) wParam;
                     }

                     match = ContDlgHandler_DoContactNamePredict(pMe,
                                                                 &idx,
                                                                 FALSE);
                  }

                  if (match) {
#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
                     pMe->m_contactListMenuStart = idx;
                     ContDlgHandler_PopulateContactListMenu(pMe,
                                                   ctrl,
                                                   pMe->m_contactListMenuStart,
                                                   wNumLines-1,
                                                   TRUE);
                     (void) IMENUCTL_Redraw(ctrl);
#else
                     IMENUCTL_SetSel(ctrl,
                                     idx);
#endif /* defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW) */
                     //load the fields  for the selected contact so that
                     //left and right keys can start working
                     pMe->m_contactSelId = IMENUCTL_GetSel(ctrl);
                     (void)ContDlgHandler_LoadAEEAddrFields(pMe);
                  } else {
                     // No match, reset the keystroke list
                     pMe->m_namePredictLen = 0;
                  }
                  return TRUE;
               }

            case AVK_STAR:
               // Reset the keystroke list
               pMe->m_namePredictLen = 0;
               break;


#endif /* FEATURE_CONTACTNAME_PREDICT */

            case AVK_UP:
#ifdef FEATURE_CONTACTNAME_PREDICT
               pMe->m_namePredictLen = 0;
#endif /* FEATURE_CONTACTNAME_PREDICT */

#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
               pMe->m_contactSelId = IMENUCTL_GetSel(ctrl);

               if ( (pMe->m_contactSelId > 0) &&
                    (pMe->m_contactSelId == pMe->m_contactListMenuStart) ) {
                  pMe->m_contactListMenuStart--;
                  ContDlgHandler_PopulateContactListMenu(pMe,
                                                ctrl,
                                                pMe->m_contactListMenuStart,
                                                wNumLines-1,
                                                TRUE);
               }
#endif /* defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW) */
               return TRUE;

            case AVK_DOWN:
#ifdef FEATURE_CONTACTNAME_PREDICT
               pMe->m_namePredictLen = 0;
#endif /* FEATURE_CONTACTNAME_PREDICT */

#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
               {
                  uint32 menuEnd;
                  uint16 nOldContactSelId = pMe->m_contactSelId;

                  pMe->m_contactSelId = IMENUCTL_GetSel(ctrl);

                  menuEnd = (pMe->m_contactListMenuStart +
                             (wNumLines-1)) - 1;
                  if (menuEnd < IVector_Size(pMe->m_contact_iv)) {
                     if ((uint32)pMe->m_contactSelId > menuEnd)
                     {
                        /* Restore the old selection and error */
                        pMe->m_contactSelId = nOldContactSelId;
                        MSG_ERROR("Selection out of range, %d.",
                                  (uint32)pMe->m_contactSelId, 0, 0);
                        return TRUE;
                     }

                     if (((uint32)pMe->m_contactSelId == menuEnd) &&
                         ((uint32)pMe->m_contactSelId+1 <
                          IVector_Size(pMe->m_contact_iv))) {
                        (void)IMENUCTL_DeleteItem(ctrl,
                                                  pMe->m_contactListMenuStart);

                        pMe->m_contactListMenuStart++;

                        ContDlgHandler_PopulateContactListMenu(pMe,
                                                              ctrl,
                                                              (uint16)menuEnd+1,
                                                              1,
                                                              FALSE);
                     }
                  }
               }
#endif /* defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW) */
               return TRUE;

            case AVK_LEFT:
               if (STR_CONTACTLIST_FINDNUMBER != pMe->m_contactListType) {
                  (void)ContDlgHandler_ShowNextNumberType(pMe,
                                                          ctrl,
                                                          BACKWARD_DIRECTION);
               }
               break;

            case AVK_RIGHT:
               if (STR_CONTACTLIST_FINDNUMBER != pMe->m_contactListType) {
                  (void)ContDlgHandler_ShowNextNumberType(pMe,
                                                          ctrl,
                                                          FORWARD_DIRECTION);
               }
               break;

            default:
               break;
         }
         break;

      case EVT_KEY_RELEASE:
         switch ((AVKType)wParam) {

            //If we are at the top of contact list then scroll over to the last
            //contact
            case AVK_UP:
               if (STR_CONTACTLIST_FINDNUMBER != pMe->m_contactListType) {
                  //restore the first number type icon for the current selected
                  //contact
                  (void)ContDlgHandler_ShowFirstNumberType(pMe,ctrl);
               }

#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
               if (pMe->m_contactSelId == 0) {
                  pMe->m_contactSelId=(uint16)IVector_Size(pMe->m_contact_iv)-1;

                  //If total number of contacts are more than number of lines
                  // that can be displayed then repopulate the list else
                  //just change the current selection
                  if ((uint16)IVector_Size(pMe->m_contact_iv) > wNumLines-1) {
                     pMe->m_contactListMenuStart = (pMe->m_contactSelId -
                                                    (wNumLines-1))+1;
                     ContDlgHandler_PopulateContactListMenu(pMe,
                                                   ctrl,
                                                   pMe->m_contactListMenuStart,
                                                   wNumLines-1,
                                                   TRUE);
                     (void) IMENUCTL_Redraw(ctrl);
                  } else {
                     IMENUCTL_SetSel(ctrl, pMe->m_contactSelId);
                  }
               }
#else
               pMe->m_contactSelId = IMENUCTL_GetSel(ctrl);
               if (pMe->m_contactSelId == 0) {
                  pMe->m_contactSelId=(uint16)IVector_Size(pMe->m_contact_iv)-1;
                  IMENUCTL_SetSel(ctrl, pMe->m_contactSelId);
               }
#endif /* defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW) */
               //without this ContDlgHandler_ShowNextNumberType
               //if not search on find number list, populate the m_fld_recs now
               if (STR_CONTACTLIST_FINDNUMBER != pMe->m_contactListType) {
                  ContactInfo *ci = NULL;
                  pMe->m_contactSelId = IMENUCTL_GetSel(ctrl);
                  (void)ContDlgHandler_LoadAEEAddrFields(pMe);
                  ci = IVector_ElementAt(pMe->m_contact_iv,
                                         pMe->m_contactSelId);
                  if (ci!=NULL) {
                     pMe->m_contactListFieldID = ci->m_fldNumberId;
                  }
               }
               break;

               //If we are bottom of the the contacts list then scroll
               //over to top of //the list
            case AVK_DOWN:
               if (STR_CONTACTLIST_FINDNUMBER != pMe->m_contactListType) {
                  //restore the first number type icon for the current selected
                  //contact
                  (void)ContDlgHandler_ShowFirstNumberType(pMe,ctrl);
               }

#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
               if (pMe->m_contactSelId==
                   (uint16)IVector_Size(pMe->m_contact_iv)-1) {
                  pMe->m_contactSelId = 0;

                  //If total number of contacts are more than number of lines
                  // that can be displayed then repopulate the list else
                  //just change the current selection
                  if ((uint16)IVector_Size(pMe->m_contact_iv) > wNumLines-1) {
                     pMe->m_contactListMenuStart = 0;
                     ContDlgHandler_PopulateContactListMenu(pMe,
                                                   ctrl,
                                                   pMe->m_contactListMenuStart,
                                                   wNumLines - 1,
                                                   TRUE);
                     (void) IMENUCTL_Redraw(ctrl);
                  } else {
                     IMENUCTL_SetSel(ctrl, pMe->m_contactSelId);
                  }
               }
#else
               pMe->m_contactSelId = IMENUCTL_GetSel(ctrl);
               if (pMe->m_contactSelId ==
                   (uint16)IVector_Size(pMe->m_contact_iv)-1) {
                  pMe->m_contactSelId = 0;
                  IMENUCTL_SetSel(ctrl, pMe->m_contactSelId);
               }
#endif /* defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW) */
               //without this ContDlgHandler_ShowNextNumberType will not work
               //if not search on find number list, populate the m_fld_recs now
               if (STR_CONTACTLIST_FINDNUMBER != pMe->m_contactListType) {
                  ContactInfo *ci = NULL;
                  pMe->m_contactSelId = IMENUCTL_GetSel(ctrl);
                  (void)ContDlgHandler_LoadAEEAddrFields(pMe);
                  ci = IVector_ElementAt(pMe->m_contact_iv,
                                         pMe->m_contactSelId);
                  if (ci!=NULL) {
                     pMe->m_contactListFieldID = ci->m_fldNumberId;
                  }
               }
               break;

            default:
               break;
         }
         break;

      case EVT_KEY:
         switch (wParam) {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CONT_CANCELED)
               return TRUE;

#ifdef FEATURE_IDLE_CALL
            case AVK_SEND:
               // Don't allow the user to dial an FDN number as it is
               // not necessarily a valid number (ie, it can contain
               // wildcard characters)
               if (STR_CONTACTLIST_RUIM_FDN != pMe->m_contactListType)
               {
                  pMe->m_contactSelId = IMENUCTL_GetSel(ctrl);

                  CLOSE_DIALOG(DLGRET_CONTACTLIST_SENDKEY)
               }
               return TRUE;
#endif /* FEATURE_IDLE_CALL */

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // Don't allow the user to move to the 'Contact dialog' when
         // viewing the FDN numbers.  They are read-only so there isn't
         // really anything useful the user could do in that dialog
         if (STR_CONTACTLIST_RUIM_FDN != pMe->m_contactListType)
         {
            pMe->m_contactSelId = IMENUCTL_GetSel(ctrl);
            CLOSE_DIALOG(DLGRET_CONT_OK)
         }
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_ContactListDialogEvent()


/*=============================================================================
FUNCTION: ContDlgHandler_AddNewDialogEvent

DESCRIPTION:
   Dialog events handler for the Add new contact dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_AddNewDialogEvent(CContactApp *pMe,
                                                AEEEvent     eCode,
                                                uint16       wParam,
                                                uint32       dwParam)
{
#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         {
            IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                           ADDNEW_MENU);
            if (NULL == menu) {
               return FALSE;
            }

            ContDlgHandler_SetMenuStyle(pMe->m_pShell, menu);

            // Select the menu title to use
            switch (pMe->m_fieldInfo.m_type) {
               case STR_FIELDINPUT_EMAIL:
                  IMENUCTL_SetSel(menu,
                                  ADDNEW_MENUITEM_EMAIL);
                  break;

               case STR_FIELDINPUT_ADDRESS:
                  IMENUCTL_SetSel(menu,
                                  ADDNEW_MENUITEM_ADDRESS);
                  break;

               case STR_FIELDINPUT_URL:
                  IMENUCTL_SetSel(menu,
                                  ADDNEW_MENUITEM_URL);
                  break;

               case STR_FIELDINPUT_NUMBER:
                  IMENUCTL_SetSel(menu,
                                  ADDNEW_MENUITEM_NUMBER);
                  break;

               default:
                  MSG_ERROR("Unrecognized address field type, %d.",
                            pMe->m_fieldInfo.m_type, 0, 0);
                  break;
            }

            (void) IMENUCTL_Redraw(menu);
            return TRUE;
         }

      case EVT_DIALOG_END:
         return TRUE;

      case EVT_KEY:
         switch (wParam) {
            case AVK_CLR:
            case AVK_LEFT:
               CLOSE_DIALOG(DLGRET_CONT_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         switch (wParam) {
            case ADDNEW_MENUITEM_NUMBER:
               pMe->m_fieldInfo.m_type = STR_FIELDINPUT_NUMBER;
               break;
            case ADDNEW_MENUITEM_EMAIL:
               pMe->m_fieldInfo.m_type = STR_FIELDINPUT_EMAIL;
               break;
            case ADDNEW_MENUITEM_ADDRESS:
               pMe->m_fieldInfo.m_type = STR_FIELDINPUT_ADDRESS;
               break;
            case ADDNEW_MENUITEM_URL:
               pMe->m_fieldInfo.m_type = STR_FIELDINPUT_URL;
               break;
            default:
               MSG_ERROR("Unrecognized EVT_COMMAND, %d.", wParam, 0, 0);
               return FALSE;
         }
         CLOSE_DIALOG(DLGRET_CONT_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_AddNewDialogEvent()


/*=============================================================================
FUNCTION: ContDlgHandler_AddNewRuimDialogEvent

DESCRIPTION:
   Dialog events handler for the Add new RUIM contact dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_AddNewRuimDialogEvent(CContactApp *pMe,
                                                AEEEvent     eCode,
                                                uint16       wParam,
                                                uint32       dwParam)
{
#ifndef FEATURE_MMGSDI
  PARAM_NOT_REF(pMe)
  PARAM_NOT_REF(eCode)
  PARAM_NOT_REF(wParam)
  PARAM_NOT_REF(dwParam)
  
  return FALSE;
  
  #else
  int retVal;
  int delItemCount = 0;
  int itemCount = 0;
  AEEAddrFieldInfo fieldInfo;
  int accessedMenuItems[5] = {0,0,0,0,0};
  
  #ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
  return FALSE;
  }
  #endif
  
  switch (eCode) {
    case EVT_DIALOG_INIT:
      return TRUE;
  
    case EVT_DIALOG_START:
    {       
      IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                         ADDNEW_RUIM_MENU);
      if (NULL == menu) {
        return FALSE;
      }
      
      itemCount = IMENUCTL_GetItemCount(menu);
  
      if (pMe->m_contactListType == STR_CONTACTLIST_RUIM_FDN) {
        retVal = IADDRBOOK_EnumFieldsInfoInit(pMe->m_addr_book, AEE_ADDR_CAT_SIM_FDN);
      }
      else {
        retVal = IADDRBOOK_EnumFieldsInfoInit(pMe->m_addr_book, AEE_ADDR_CAT_SIM_ADN);
      }
                      
      if ( retVal != AEE_SUCCESS)
        return FALSE;
      // find out what menuitems are valid for this record.
      if (pMe->m_isExisting == TRUE)
      {             
        /* Check maximum number of fields allowed per record */
        while(IADDRBOOK_EnumNextFieldsInfo(pMe->m_addr_book, &fieldInfo)) {
          switch (fieldInfo.fieldID)
          {
            case AEE_ADDRFIELD_PHONE_GENERIC:
              accessedMenuItems[0] = 1;
              if (ContDlgHandler_GetNumFields(pMe->m_rec, fieldInfo.fieldID) >=
                      fieldInfo.nMaxItems) {
                //then delete the menuitem
                (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_NUMBER);
                delItemCount++;
              }
              break;
            case AEE_ADDRFIELD_EMAIL:
              accessedMenuItems[1] = 1;
              if (ContDlgHandler_GetNumFields(pMe->m_rec, fieldInfo.fieldID) >=
                      fieldInfo.nMaxItems) {
                //then delete the menuitem
                (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_EMAIL);
                delItemCount++;
              }
              break;
            
            case AEE_ADDRFIELD_OTHERNAME:
              accessedMenuItems[2] = 1;
              if (ContDlgHandler_GetNumFields(pMe->m_rec, fieldInfo.fieldID) >=
                      fieldInfo.nMaxItems) {
                //then delete the menuitem
                (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_SECNAME);
                delItemCount++;
              }
              break;
            
            case AEE_ADDRFIELD_ADDITIONAL_NUMBER:
              accessedMenuItems[3] = 1;
              if (ContDlgHandler_GetNumFields(pMe->m_rec, fieldInfo.fieldID) >=
                      fieldInfo.nMaxItems) {
                //then delete the menuitem
                (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_ADDNUMBER);
                delItemCount++;
              }
              break;
            
            case AEE_ADDRFIELD_GRPNAME_ID:
              accessedMenuItems[4] = 1;
              if (ContDlgHandler_GetNumFields(pMe->m_rec, fieldInfo.fieldID) >=
                      fieldInfo.nMaxItems) {
                //then delete the menuitem
                (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_GROUPID);
                delItemCount++;
              }
              break;
            
            default:
              break;
          }
        }   
      }
      // if record doesnt currently exist
      else { 
        while(IADDRBOOK_EnumNextFieldsInfo(pMe->m_addr_book, &fieldInfo)) {
          switch (fieldInfo.fieldID)
          {
            case AEE_ADDRFIELD_PHONE_GENERIC:
              accessedMenuItems[0] = 1;
              if (fieldInfo.nMaxItems == 0) {
                //then delete the menuitem
                (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_NUMBER);
                delItemCount++;
              }
              break;
            case AEE_ADDRFIELD_EMAIL:
              accessedMenuItems[1] = 1;
              if (fieldInfo.nMaxItems == 0) {
                //then delete the menuitem
                (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_EMAIL);
                delItemCount++;
              }
              break;
            
            case AEE_ADDRFIELD_OTHERNAME:
              accessedMenuItems[2] = 1;
              if (fieldInfo.nMaxItems == 0) {
                //then delete the menuitem
                (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_SECNAME);
                delItemCount++;
              }
              break;
            
            case AEE_ADDRFIELD_ADDITIONAL_NUMBER:
              accessedMenuItems[3] = 1;
              if (fieldInfo.nMaxItems == 0) {
                //then delete the menuitem
                (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_ADDNUMBER);
                delItemCount++;
              }
              break;
            
            case AEE_ADDRFIELD_GRPNAME_ID:
              accessedMenuItems[4] = 1;
              if (fieldInfo.nMaxItems == 0) {
                //then delete the menuitem
                (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_GROUPID);
                delItemCount++;
              }
              break;
            
            default:
              break;
          }
        }             
      }
      // make sure that all possible menu fields have been accessed, as part of
      // IADDRBOOK_EnumNextFieldsInfo(), or delete them
    if (accessedMenuItems[0] != 1) 
      (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_NUMBER);
    if (accessedMenuItems[1] != 1) 
      (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_EMAIL);
    if (accessedMenuItems[2] != 1) 
      (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_SECNAME);
    if (accessedMenuItems[3] != 1) 
      (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_ADDNUMBER);
    if (accessedMenuItems[4] != 1) 
      (void)IMENUCTL_DeleteItem(menu, ADDNEW_RUIM_MENUITEM_GROUPID);       
       
      // if all items are deleted then we need to tell our dialog state 
      // handler that we have a message to show.
      if (delItemCount == itemCount)
      {
        return ISHELL_PostEvent(pMe->m_pShell,
                          AEECLSID_CONTACT_APP,
                          EVT_USER,
                          CONT_EVENT_RUIM_EMPTY,
                          0);      
      }
      
      ContDlgHandler_SetMenuStyle(pMe->m_pShell, menu);
      
      IMENUCTL_SetSel(menu, 0);
      
      (void) IMENUCTL_Redraw(menu);
      return TRUE;
    }
    
    case EVT_USER:
      // tell the handler that we have an empty screen
      if ((ContEventType)wParam == CONT_EVENT_RUIM_EMPTY) {
        CLOSE_DIALOG(DLGRET_RUIM_EMPTY_ADDNEW)
        return TRUE;
      }
    
    case EVT_DIALOG_END:
      return TRUE;
    
    case EVT_KEY:
      switch (wParam) {
        case AVK_CLR:
        case AVK_LEFT:
          CLOSE_DIALOG(DLGRET_CONT_CANCELED)
          return TRUE;
    
      default:
        break;
    }
    break;
    
    case EVT_COMMAND:
    switch (wParam) {
      case ADDNEW_RUIM_MENUITEM_NUMBER:
        pMe->m_fieldInfo.m_type = STR_FIELDINPUT_NUMBER;
        break;
      case ADDNEW_RUIM_MENUITEM_ADDNUMBER:
        pMe->m_fieldInfo.m_type = STR_FIELDINPUT_ADDNUMBER;
        break;
      case ADDNEW_RUIM_MENUITEM_EMAIL:
        pMe->m_fieldInfo.m_type = STR_FIELDINPUT_EMAIL;
        break;
      case ADDNEW_RUIM_MENUITEM_GROUPID:
        pMe->m_fieldInfo.m_type = STR_FIELDINPUT_ID;
        break;
      case ADDNEW_RUIM_MENUITEM_SECNAME:
        pMe->m_fieldInfo.m_type = STR_FIELDINPUT_SECNAME;
        break;
      default:
        MSG_ERROR("Unrecognized field type, %d.", pMe->m_fieldInfo.m_type,
               0, 0);
        return FALSE;
      }
      CLOSE_DIALOG(DLGRET_CONT_OK)
      return TRUE;
      
    default:
      break;
    }
  
  return FALSE;
  #endif //#ifndef FEATURE_MMGSDI
} // ContDlgHandler_AddNewRuimDialogEvent()

/*=============================================================================
FUNCTION: ContApp_NVCheck_IsAutoHyphen

=============================================================================*/
boolean ContApp_NVCheck_IsAutoHyphen(void)
{
  nv_item_type nvi;

  if(ui_get_nv(NV_AUTO_HYPHEN_I, &nvi) == NV_DONE_S)
  {
    return(nvi.auto_hyphen);
  }
  else
  {
    // default
    return TRUE;
  }
}

/*=============================================================================
FUNCTION: ContDlgHandler_FieldInputDialogEvent

DESCRIPTION:
    Dialog events handler for the Field Input dialog for the contact

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.



   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.


COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_FieldInputDialogEvent(CContactApp *pMe,
                                                    AEEEvent     eCode,
                                                    uint16       wParam,
                                                    uint32       dwParam)
{
   ITextCtl *ctrl;
   IMenuCtl *menu;
   int16    mId;
   int16    tId;
   uint16   wFieldLen =  FLDLEN_ADDRESS;

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   switch (pMe->m_fieldInfo.m_type) {
      case STR_FIELDINPUT_NUMBER:
         tId = N_FIELDINPUT_TEXTINPUT;
         mId = N_FIELDINPUT_SOFTKEYS;
         wFieldLen = FLDLEN_NUMBER;
         break;
      case STR_FIELDINPUT_ADDNUMBER:
         tId = N_FIELDINPUT_TEXTINPUT;
         mId = N_FIELDINPUT_SOFTKEYS;
         wFieldLen = FLDLEN_NUMBER;
         break;
      case STR_FIELDINPUT_ADDRESS:
         tId = A_FIELDINPUT_TEXTINPUT;
         mId = A_FIELDINPUT_SOFTKEYS;
         wFieldLen = FLDLEN_ADDRESS;
         break;
      case STR_FIELDINPUT_EMAIL:
         tId = E_FIELDINPUT_TEXTINPUT;
         mId = E_FIELDINPUT_SOFTKEYS;
         wFieldLen = FLDLEN_EMAIL;
         break;
      case STR_FIELDINPUT_URL:
         tId = U_FIELDINPUT_TEXTINPUT;
         mId = U_FIELDINPUT_SOFTKEYS;
         wFieldLen = FLDLEN_URL;
         break;
      case STR_FIELDINPUT_ID:
         tId = G_FIELDINPUT_IDINPUT;
         mId = G_FIELDINPUT_SOFTKEY;
         wFieldLen = FLDLEN_GROUPID;
         break;
      case STR_FIELDINPUT_SECNAME:
         tId = SN_FIELDINPUT_NAMEINPUT;
         mId = SN_FIELDINPUT_SOFTKEY;
         wFieldLen = FLDLEN_NAME;
         break;
      default:
         MSG_ERROR("Unrecognized field type, %d.", pMe->m_fieldInfo.m_type,
                   0, 0);
         return FALSE;
   }

   ctrl = (ITextCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                        tId);

   menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                        mId);

   if (NULL == ctrl || NULL == menu) {
      return FALSE;
   }


   switch (eCode) {
      case EVT_DIALOG_INIT:
         pMe->bMultiTap = FALSE;
         return TRUE;

      case EVT_DIALOG_START:
         {
            AEERect    textRect, menuRect;
            CtlAddItem ai;
            uint32 dwTextProps = 0;

            ITEXTCTL_SetMaxSize(ctrl,wFieldLen);

            // Remove the multitap softkey as soon as possible
            (void) IMENUCTL_DeleteAll(menu);
            (void) IMENUCTL_Redraw(menu);

            // Build the softkey menu manually.
            ai.pText = NULL;
            ai.pImage = NULL;
            ai.pszResText = ai.pszResImage = CONTACTAPP_RES_FILE;
            ai.wFont = 0;
            ai.dwData = 0;

            if (TRUE == pMe->m_isAddNew) {
               ai.wText = STR_FIELDINPUT_SOFTKEY_NEXT;
               ai.wImage = ICON_NEXT;
               ai.wItemID = FIELDINPUT_SOFTKEY_NEXT;
               (void) IMENUCTL_AddItemEx(menu,
                                         &ai);

               if (STARTMETHOD_SAVENUMBER_NUMBER_EDIT == pMe->m_startMethod) {
                  ai.wText = STR_FIELDINPUT_SOFTKEY_CANCEL;
                  ai.wImage = ICON_CANCEL;
                  ai.wItemID = FIELDINPUT_SOFTKEY_CANCEL;
                  (void) IMENUCTL_AddItemEx(menu,
                                            &ai);
               } else {
                  ai.wText = STR_FIELDINPUT_SOFTKEY_BACK;
                  ai.wImage = ICON_BACK;
                  ai.wItemID = FIELDINPUT_SOFTKEY_BACK;
                  (void) IMENUCTL_AddItemEx(menu,
                                            &ai);
               }
            } else {
               ai.wText = STR_FIELDINPUT_SOFTKEY_SAVE;
               ai.wImage = ICON_SAVE;
               ai.wItemID = FIELDINPUT_SOFTKEY_SAVE;
               (void) IMENUCTL_AddItemEx(menu,
                                         &ai);

               ai.wText = STR_FIELDINPUT_SOFTKEY_CANCEL;
               ai.wImage = ICON_CANCEL;
               ai.wItemID = FIELDINPUT_SOFTKEY_CANCEL;
               (void) IMENUCTL_AddItemEx(menu,
                                         &ai);
            }

            //set the rect so that it does not clip, the menu control.
            IMENUCTL_GetRect(menu,&menuRect);
            ITEXTCTL_GetRect(ctrl, &textRect);
            textRect.dy = pMe->m_rc.dy - menuRect.dy;
            ITEXTCTL_SetRect(ctrl, &textRect);

            if ((STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) ||
                (STR_FIELDINPUT_ADDNUMBER == pMe->m_fieldInfo.m_type)) {
               // Use the large font when editing numbers
               dwTextProps = TP_MULTILINE | TP_FRAME ;
#if MIN_BREW_VERSION(3,1)
               dwTextProps |= TP_FONTMORPH;
#ifdef TP_VALIDATE_INPUT
               dwTextProps |= TP_VALIDATE_INPUT;
#endif
               if (ContApp_NVCheck_IsAutoHyphen() == TRUE)
               {
                 dwTextProps |= TP_AUTOHYPHEN;
               }
               ITEXTCTL_SetProperties(ctrl, dwTextProps);
               (void) ITEXTCTL_SetInputMode(ctrl, AEE_TM_DIALED_DIGITS);
#else
               dwTextProps |= OEM_TP_FONTMORPH;
               if (ContApp_NVCheck_IsAutoHyphen() == TRUE)
               {
                 dwTextProps |= OEM_TP_AUTOHYPHEN;
               }
               ITEXTCTL_SetProperties(ctrl, dwTextProps);
               (void) ITEXTCTL_SetInputMode(ctrl, OEM_TM_DIALED_DIGITS);
#endif
            } else if (STR_FIELDINPUT_ID == pMe->m_fieldInfo.m_type) {
               (void) ITEXTCTL_SetInputMode(ctrl, AEE_TM_NUMBERS );
            } else {
               if ((STR_FIELDINPUT_EMAIL == pMe->m_fieldInfo.m_type) ||
                   (STR_FIELDINPUT_SECNAME == pMe->m_fieldInfo.m_type) ||
                   (STR_FIELDINPUT_URL == pMe->m_fieldInfo.m_type)) {
                  // In case of email and web address set the input method
                  // to multitap, even in Chinese mode
                  dwTextProps = TP_MULTILINE | TP_FRAME;
                  ITEXTCTL_SetProperties(ctrl, dwTextProps);
                  (void) ITEXTCTL_SetInputMode(ctrl, AEE_TM_LETTERS);
               }

               // Add Multi-tap menu
               ITEXTCTL_SetSoftKeyMenu(ctrl, menu);
            }

            SetDefaultSoftkeyLook (pMe->m_pShell, menu);
            if (NULL != pMe->m_fieldInfo.m_name) {
               uint32 oldProps = ITEXTCTL_GetProperties(ctrl);
               AEETextInputMode oldMode = ITEXTCTL_GetInputMode(ctrl, NULL);

               ITEXTCTL_SetProperties(ctrl, oldProps | TP_NODRAW);
               (void) ITEXTCTL_SetText(ctrl,
                                       pMe->m_fieldInfo.m_name,
                                       -1);

               ITEXTCTL_SetProperties(ctrl, oldProps);
               ITEXTCTL_SetInputMode(ctrl, oldMode);
            }
            else {
               (void) ITEXTCTL_SetText(ctrl, NULL, 0);
            }
            return TRUE;
         }

      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            FREE_STRING(pMe->m_fieldInfo.m_name);
            pMe->m_fieldInfo.m_name = WSTRDUP(ITEXTCTL_GetTextPtr(ctrl));
         }
         return TRUE;

      case EVT_KEY_PRESS:
         pMe->wKeyPressParam = wParam;
         switch ((AVKType)wParam) {
            case AVK_CLR:
               if (WSTRLEN(ITEXTCTL_GetTextPtr(ctrl)) <= 0) {
                  pMe->m_fieldWasEmpty = TRUE;
               } else {
                  pMe->m_fieldWasEmpty = FALSE;
               }
               if (pMe->bMultiTap)
                  pMe->bMultiTap = FALSE;
               break;
            case AVK_UP:
               if (!pMe->bMultiTap) {
                  IMENUCTL_SetActive(menu, FALSE);
                  ITEXTCTL_SetActive(ctrl, FALSE);
                  return TRUE;
               }
               break;
            case AVK_DOWN:
               if (!pMe->bMultiTap) {
                  ITEXTCTL_SetActive(ctrl, FALSE);
                  IMENUCTL_SetActive(menu, FALSE);
                  return TRUE;
               }
               break;
            case AVK_SELECT:
               if (pMe->bMultiTap)
                  pMe->bMultiTap = FALSE;
               break;
            default:
               break;
         }

         return FALSE;

      case EVT_KEY:
         switch((AVKType) wParam) {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CONT_CANCELED)
               return TRUE;

#ifdef FEATURE_IDLE_CALL
            case AVK_SEND:
               if ((STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) ||
                   (STR_FIELDINPUT_ADDNUMBER == pMe->m_fieldInfo.m_type)){
                  if(pMe->m_fieldInfo.m_name!=NULL){
                     FREE(pMe->m_fieldInfo.m_name);
                  }
                  pMe->m_fieldInfo.m_name = WSTRDUP(ITEXTCTL_GetTextPtr(ctrl));

                  CLOSE_DIALOG(DLGRET_CONTACT_SENDKEY)
                  return TRUE;

               }else{
                  break;
               }
#endif/*FEATURE_IDLE_CALL*/

             default :
               break;
         }
         break;

      case EVT_KEY_RELEASE:
         switch ((AVKType)pMe->wKeyPressParam) {
            case AVK_UP:
               if (!pMe->bMultiTap) {
                  IMENUCTL_SetActive(menu, FALSE);
                  ITEXTCTL_SetActive(ctrl, TRUE);
                  return TRUE;
               }
               break;
            case AVK_DOWN:
               if (!pMe->bMultiTap) {
                  ITEXTCTL_SetActive(ctrl, FALSE);
                  IMENUCTL_SetActive(menu, TRUE);
                  return TRUE;
               }
               break;
            default:
               break;
         }
         return FALSE;

      case EVT_COMMAND:
         switch (wParam)
         {
           case FIELDINPUT_SOFTKEY_BACK:
           case FIELDINPUT_SOFTKEY_CANCEL:
             CLOSE_DIALOG(DLGRET_CONT_CANCELED)
             return TRUE;

           case FIELDINPUT_SOFTKEY_NEXT:
           case FIELDINPUT_SOFTKEY_SAVE:
             // save the entered text
             FREE_STRING(pMe->m_fieldInfo.m_name);
             pMe->m_fieldInfo.m_name = WSTRDUP(ITEXTCTL_GetTextPtr(ctrl));
             CLOSE_DIALOG(DLGRET_CONT_OK)
             return TRUE;

           default:
             MSG_ERROR("Unexpected EVT_COMMAND, %d.", wParam, 0, 0);
             break;
         }
         return FALSE;

      case EVT_CTL_MENU_OPEN:
         pMe->bMultiTap = TRUE;
         break;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_FieldInputDialogEvent()


/*=============================================================================
FUNCTION: ContDlgHandler_NumberTypeDialogEvent

DESCRIPTION:
    Dialog events handler for the Number Type Dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_NumberTypeDialogEvent(CContactApp *pMe,
                                                    AEEEvent     eCode,
                                                    uint16       wParam,
                                                    uint32       dwParam)
{
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  NUMBERTYPE_MENU);
#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   if (NULL == menu) {
      return FALSE;
   }

   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         ContDlgHandler_SetMenuStyle(pMe->m_pShell, menu);

         IMENUCTL_SetSel(menu,
                         pMe->m_fieldInfo.m_numberType);
         return TRUE;

      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            pMe->m_fieldInfo.m_numberType = IMENUCTL_GetSel(menu);
         }
         return TRUE;

      case EVT_KEY:
         switch (wParam) {
            case AVK_CLR:
            case AVK_LEFT:
               CLOSE_DIALOG(DLGRET_CONT_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         pMe->m_fieldInfo.m_numberType = IMENUCTL_GetSel(menu);

         CLOSE_DIALOG(DLGRET_CONT_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_NumberTypeDialogEvent


/*=============================================================================
FUNCTION: ContDlgHandler_ContactNameDialogEvent

DESCRIPTION:
   Dialog events handler for the contact name dialog.

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_ContactNameDialogEvent(CContactApp *pMe,
                                                     AEEEvent     eCode,
                                                     uint16       wParam,
                                                     uint32       dwParam)
{
   ITextCtl *ctrl;
   IMenuCtl *menu;
   //Get the pointers to the text and menu control

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   if (TRUE == pMe->m_isAddNew) {
      ctrl = (ITextCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                           NEWCONTACTNAME_TEXTINPUT);
      menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                           NEWCONTACTNAME_SOFTKEYS);
   } else {
      ctrl = (ITextCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                           CONTACTNAME_TEXTINPUT);
      menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                           CONTACTNAME_SOFTKEYS);
   }

   if (NULL == ctrl || NULL == menu) {
      return FALSE;
   }

   switch (eCode) {
      case EVT_DIALOG_INIT:
         pMe->bMultiTap = FALSE;
         return TRUE;

      case EVT_DIALOG_START:
         {

            AEERect textRect, menuRect;

            // Build the softkey menu manually.  This is necessary to
            // remove the item that the ITextCtl adds when entering a phone number
            CtlAddItem ai;

            ai.pText = NULL;
            ai.pImage = NULL;
            ai.pszResText = ai.pszResImage = CONTACTAPP_RES_FILE;
            ai.wFont = 0;
            ai.dwData = 0;

            (void) IMENUCTL_DeleteAll(menu);
            (void) IMENUCTL_Redraw(menu);

            if (TRUE == pMe->m_isAddNew) {
               ai.wText = STR_CONTACTNAME_SOFTKEY_NEXT;
               ai.wImage = ICON_NEXT;
               ai.wItemID = CONTACTNAME_SOFTKEY_NEXT;
               (void) IMENUCTL_AddItemEx(menu,
                                         &ai);
               ai.wText = STR_CONTACTNAME_SOFTKEY_BACK;
               ai.wImage = ICON_BACK;
               ai.wItemID = CONTACTNAME_SOFTKEY_BACK;
               (void) IMENUCTL_AddItemEx(menu,
                                         &ai);
            } else {
               ai.wText = STR_CONTACTNAME_SOFTKEY_SAVE;
               ai.wImage = ICON_SAVE;
               ai.wItemID = CONTACTNAME_SOFTKEY_SAVE;
               (void) IMENUCTL_AddItemEx(menu,
                                         &ai);

               ai.wText = STR_CONTACTNAME_SOFTKEY_CANCEL;
               ai.wImage = ICON_CANCEL;
               ai.wItemID = CONTACTNAME_SOFTKEY_CANCEL;
               (void) IMENUCTL_AddItemEx(menu,
                                         &ai);
            }

            ITEXTCTL_SetMaxSize(ctrl,
                                FLDLEN_NAME);

            ITEXTCTL_SetProperties(ctrl,
                                   ITEXTCTL_GetProperties(ctrl)& ~TP_MULTILINE);

            if (pMe->m_contactInfo.m_name != NULL) {
               uint32 oldProps = ITEXTCTL_GetProperties(ctrl);

               ITEXTCTL_SetProperties(ctrl, oldProps | TP_NODRAW);
               (void) ITEXTCTL_SetText(ctrl,
                                       pMe->m_contactInfo.m_name,
                                       -1);
               ITEXTCTL_SetProperties(ctrl, oldProps);
            }
            else {
               (void) ITEXTCTL_SetText(ctrl,
                                       NULL,
                                       0);
            }

            ITEXTCTL_SetSoftKeyMenu(ctrl, menu);
            //set the rect so that it does not clip, the menu control.
            IMENUCTL_GetRect(menu,&menuRect);
            ITEXTCTL_GetRect(ctrl, &textRect);
            textRect.dy = pMe->m_rc.dy - menuRect.dy;
            ITEXTCTL_SetRect(ctrl, &textRect);
            SetDefaultSoftkeyLook (pMe->m_pShell, menu);
            return TRUE;
         }

      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            FREE_STRING(pMe->m_contactInfo.m_name);
            pMe->m_contactInfo.m_name = WSTRDUP(ITEXTCTL_GetTextPtr(ctrl));
         }
         return TRUE;


      case EVT_KEY_PRESS:
         pMe->wKeyPressParam = wParam;
         switch ((AVKType)wParam) {
            case AVK_CLR:
               if (WSTRLEN(ITEXTCTL_GetTextPtr(ctrl)) <= 0) {
                  pMe->m_fieldWasEmpty = TRUE;
               } else {
                  pMe->m_fieldWasEmpty = FALSE;
               }
               if (pMe->bMultiTap)
                  pMe->bMultiTap = FALSE;
               break;
            case AVK_UP:
               if (!pMe->bMultiTap) {
                  IMENUCTL_SetActive(menu, FALSE);
                  ITEXTCTL_SetActive(ctrl, FALSE);
                  return TRUE;
               }
               break;
            case AVK_DOWN:
               if (!pMe->bMultiTap) {
                  ITEXTCTL_SetActive(ctrl, FALSE);
                  IMENUCTL_SetActive(menu, FALSE);
                  return TRUE;
               }
               break;
            case AVK_SELECT:
               if (pMe->bMultiTap)
                  pMe->bMultiTap = FALSE;
               break;
            default:
               break;
         }

         return FALSE;

      case EVT_KEY:
         if (AVK_CLR == (AVKType) wParam) {
            CLOSE_DIALOG(DLGRET_CONT_CANCELED)
            return TRUE;
         }
         break;

      case EVT_KEY_RELEASE:
         switch ((AVKType)pMe->wKeyPressParam) {
            case AVK_UP:
               if (!pMe->bMultiTap) {
                  IMENUCTL_SetActive(menu, FALSE);
                  ITEXTCTL_SetActive(ctrl, TRUE);
                  return TRUE;
               }
               break;
            case AVK_DOWN:
               if (!pMe->bMultiTap) {
                  ITEXTCTL_SetActive(ctrl, FALSE);
                  IMENUCTL_SetActive(menu, TRUE);
                  return TRUE;
               }
               break;
            default:
               break;
         }
         return FALSE;

      case EVT_COMMAND:
         switch (wParam)
         {

           case CONTACTNAME_SOFTKEY_BACK:
           case CONTACTNAME_SOFTKEY_CANCEL:
             CLOSE_DIALOG(DLGRET_CONT_CANCELED)
             return TRUE;

           case CONTACTNAME_SOFTKEY_NEXT:
           case CONTACTNAME_SOFTKEY_SAVE:
             FREE_STRING(pMe->m_contactInfo.m_name);
             pMe->m_contactInfo.m_name = WSTRDUP(ITEXTCTL_GetTextPtr(ctrl));
             CLOSE_DIALOG(DLGRET_CONT_OK)
             return TRUE;

           default:
             MSG_ERROR("Invalid EVT_COMMAND, %d, %d.", wParam, dwParam, 0);
             break;
         }
         return FALSE;

      case EVT_CTL_MENU_OPEN:
         pMe->bMultiTap = TRUE;
         break;

      default:
         break;

   }

   return FALSE;
} // ContDlgHandler_ContactNameDialogEvent


/*=============================================================================
FUNCTION: ContDlgHandler_ContactClassDialogEvent

DESCRIPTION:
    Dialog events handler for the Contact Class Dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.


COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_ContactClassDialogEvent(CContactApp *pMe,
                                                      AEEEvent     eCode,
                                                      uint16       wParam,
                                                      uint32       dwParam)
{
   uint16   nItemID=0;//Item Id of the Contact class menu control

   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  CONTACTCLASS_MENU);
#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   if (NULL == menu) {
      return FALSE;
   }

   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         ContDlgHandler_SetMenuStyle(pMe->m_pShell, menu);
         //Find the current class category and select that one in the menu
         switch (pMe->m_contactInfo.m_class) {
            case AEE_ADDR_CAT_BUSINESS:
               nItemID = CONTACTCLASS_MENUITEM_BUSINESS;
               break;
            case AEE_ADDR_CAT_PERSONAL:
               nItemID = CONTACTCLASS_MENUITEM_PERSONAL;
               break;
            case AEE_ADDR_CAT_NONE:
               nItemID = CONTACTCLASS_MENUITEM_NEITHER;
               break;
            default:
               pMe->m_contactInfo.m_class = AEE_ADDR_CAT_PERSONAL;
               nItemID = CONTACTCLASS_MENUITEM_PERSONAL;
               break;
         }

         IMENUCTL_SetSel(menu,
                         nItemID);
         return TRUE;

      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            nItemID = IMENUCTL_GetSel(menu);
            switch (nItemID) {
               case CONTACTCLASS_MENUITEM_BUSINESS:
                  pMe->m_contactInfo.m_class = AEE_ADDR_CAT_BUSINESS;
                  break;
               case CONTACTCLASS_MENUITEM_PERSONAL:
                  pMe->m_contactInfo.m_class = AEE_ADDR_CAT_PERSONAL;
                  break;
               case CONTACTCLASS_MENUITEM_NEITHER:
                  pMe->m_contactInfo.m_class = AEE_ADDR_CAT_NONE;
                  break;
               default:
                  pMe->m_contactInfo.m_class = AEE_ADDR_CAT_PERSONAL;
                  break;
            }
         }
         return TRUE;

      case EVT_KEY:
         switch (wParam) {
            case AVK_CLR:
            case AVK_LEFT:
               CLOSE_DIALOG(DLGRET_CONT_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         //Get the selected class category so that we can change in it address
         //book
         nItemID = IMENUCTL_GetSel(menu);
         switch (nItemID) {
            case CONTACTCLASS_MENUITEM_BUSINESS:
               pMe->m_contactInfo.m_class = AEE_ADDR_CAT_BUSINESS;
               break;
            case CONTACTCLASS_MENUITEM_PERSONAL:
               pMe->m_contactInfo.m_class = AEE_ADDR_CAT_PERSONAL;
               break;
            case CONTACTCLASS_MENUITEM_NEITHER:
               pMe->m_contactInfo.m_class = AEE_ADDR_CAT_NONE;
               break;
            default:
               pMe->m_contactInfo.m_class = AEE_ADDR_CAT_PERSONAL;
               break;
         }

         CLOSE_DIALOG(DLGRET_CONT_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_ContactClassDialogEvent()



/*=============================================================================
FUNCTION: ContDlgHandler_ContactDialogEvent

DESCRIPTION:
    Dialog events handler for the Contact Dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_ContactDialogEvent(CContactApp *pMe,
                                                 AEEEvent     eCode,
                                                 uint16       wParam,
                                                 uint32       dwParam)
{
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  CONTACT_MENU);

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   if (NULL == menu) {
      return FALSE;
   }

   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         {
            CtlAddItem   ai;
            ContactInfo *ci;
            uint32       i;
            AECHAR       szUsedFormatStr[FLDLEN_NAME+1];
            AECHAR       szMenuItem[FLDLEN_NAME+1];

            ci = IVector_ElementAt(pMe->m_contact_iv,
                                   pMe->m_contactSelId);
            if (NULL == ci)
            {
               MSG_ERROR("NULL contact info at index %d.", pMe->m_contactSelId,
                         0, 0);
               return FALSE;
            }

            if (ci->m_name != NULL) {
               szUsedFormatStr[0] = 0;
               if (STR_CONTACTLIST_SPEEDDIAL==pMe->m_contactListType){
                   // Load the format string for speed dials menu items...
                   (void) ISHELL_LoadResString(pMe->m_pShell,
                                      CONTACTAPP_RES_FILE,
                                      STR_SPEEDDIAL_MENU_ASSIGNED,
                                      szUsedFormatStr,
                                      sizeof(szUsedFormatStr));

                    WSPRINTF(szMenuItem,
                         sizeof(szMenuItem),
                         szUsedFormatStr,
                         ci->m_speedDialIndex,
                         ci->m_name);
               } else {
                   (void)WSTRLCPY(szMenuItem, ci->m_name, ARR_SIZE(szMenuItem));
               }
               (void) IMENUCTL_SetTitle(menu,
                                        NULL,
                                        0,
                                        szMenuItem);
            } else {
               (void) IMENUCTL_SetTitle(menu,
                                        CONTACTAPP_RES_FILE,
                                        STR_CONTACT_MENUTITLE_EMPTY,
                                        NULL);
            }

            ContDlgHandler_SetMenuStyle(pMe->m_pShell, menu);

            ai.pImage = NULL;
            ai.pszResText = ai.pszResImage = CONTACTAPP_RES_FILE;
            ai.wText = 0;
            ai.wFont = 0;
            ai.dwData = 0;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
            if ((ci->m_class != AEE_ADDR_CAT_SIM_SDN) &&
               (ci->m_class != AEE_ADDR_CAT_SIM_SDN2))
            {
#else
            if (ci->m_class != AEE_ADDR_CAT_SIM_SDN)
            {
#endif //ifdef FEATURE_MMGSDI_DUAL_SLOT
               if (FALSE == pMe->m_selectingContact) {
                 // Options menu item
                 ai.pText = NULL;
                 ai.wText = STR_CONTACT_MENUITEM_OPTIONS;
                 ai.wImage = ICON_OPTIONS;
                 ai.wItemID = OPTIONS_MENU_ID;

                 (void) IMENUCTL_AddItemEx(menu,
                                         &ai);
               }
             }

#ifdef FEATURE_PHONE_VR
            if (pMe->m_fldrecs_has_vt) {
               IVR_RegisterNotify( pMe->m_pVR, NULL, pMe);
               IVR_PlayUtterance( pMe->m_pVR, pMe->m_fldrecs_vt);
            }
#endif

            // Load the fields
            for (i = 0; i < IVector_Size(pMe->m_fldrecs_iv); i++) {
               FieldInfo *fi;
               AEEAddrField  *cf;
               boolean        bSuccess = TRUE;

               cf = IVector_ElementAt(pMe->m_fldrecs_iv,
                                      i);
               if (NULL == cf)
               {
                  MSG_ERROR("NULL address field at index %d.", i, 0, 0);
                  continue;
               }

               fi = ContDlgHandler_AEEAddrField2FieldInfo(pMe, cf);
               if (NULL == fi) {
                  continue;
               }

               // select the correct icon
               switch (fi->m_type) {
                  case STR_FIELDINPUT_NUMBER:
                     switch (fi->m_numberType) {
                        case NUMBERTYPE_MENUITEM_WORK:
                           ai.wImage = ICON_WORKNUMBER;
                           break;
                        case NUMBERTYPE_MENUITEM_HOME:
                           ai.wImage = ICON_HOMENUMBER;
                           break;
                        case NUMBERTYPE_MENUITEM_MOBILE:
                           ai.wImage = ICON_MOBILENUMBER;
                           break;
                        case NUMBERTYPE_MENUITEM_PAGER:
                           ai.wImage = ICON_PAGERNUMBER;
                           break;
                        case NUMBERTYPE_SPEEDDIAL:
                           ai.wImage = ICON_SPEEDDIAL;
                           break;
                        case NUMBERTYPE_VOICETAG:
                           ai.wImage = ICON_VOICETAG;
                           break;
                        case NUMBERTYPE_OTHER:
                           ai.wImage = ICON_NUMBER;
                           break;
                        case NUMBERTYPE_RUIM:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                          switch(ci->m_class)
                          {
                            case AEE_ADDR_CAT_SIM_ADN:
                            case AEE_ADDR_CAT_SIM_FDN:
                              ai.wImage = ICON_RUIM;
                              break;
                            case AEE_ADDR_CAT_SIM_ADN2:
                            case AEE_ADDR_CAT_SIM_FDN2:
                              ai.wImage = ICON_RUIM2;
                              break;
                            case AEE_ADDR_CAT_SIM_SDN:
                              ai.wImage = ICON_CONTACTCLASS_SDN;
                              break;
                            case AEE_ADDR_CAT_SIM_SDN2:
                              ai.wImage = ICON_CONTACTCLASS_SDN2;
                              break;
                            default:
                              break;
                          }
                          break;
#else
                          switch (ci->m_class)
                          {
                            case AEE_ADDR_CAT_SIM_SDN:
                              ai.wImage = ICON_CONTACTCLASS_SDN;
                              break;
                            default:
                              ai.wImage = ICON_RUIM;
                              break;
                           }
                           break;
#endif
                        default:
                           MSG_ERROR("Unexpected number type, %d.",
                                     fi->m_numberType, 0, 0);
                           bSuccess = FALSE;
                           break;
                     }
                     break;
                  case STR_FIELDINPUT_SECNAME:
                    ai.wImage = ICON_ADDNEW;
                    break;
                  case STR_FIELDINPUT_ADDNUMBER:
                    ai.wImage = ICON_RUIM;
                    break;
                  case STR_FIELDINPUT_ID:
                    ai.wImage = ICON_ADDNEW;
                    break;
                  case STR_FIELDINPUT_ADDRESS:
                     ai.wImage = ICON_ADDRESS;
                     break;
                  case STR_FIELDINPUT_EMAIL:
                     ai.wImage = ICON_EMAIL;
                     break;
                  case STR_FIELDINPUT_URL:
                     ai.wImage = ICON_URL;
                     break;
                  default:
                     MSG_ERROR("Unexpected field type, %d.", fi->m_type, 0, 0);
                     bSuccess = FALSE;
                     break;
               }

               if (bSuccess)
               {
                  ai.pText = fi->m_name;
                  ai.wItemID = (uint16)i;
                  (void) IMENUCTL_AddItemEx(menu, &ai);
               }

               FREE_FIELDINFO(*fi);
               FREE(fi);
            }

            IMENUCTL_SetSel(menu, pMe->m_fieldSelId);
            return TRUE;
         }

      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            pMe->m_fieldSelId = IMENUCTL_GetSel(menu);
         }
         return TRUE;


      case EVT_KEY:
         switch (wParam) {
            case AVK_CLR:
            case AVK_LEFT:
               CLOSE_DIALOG(DLGRET_CONT_CANCELED)
               return TRUE;

            case AVK_RIGHT:
               // Send ourselves a command message
               (void) ISHELL_PostEvent(pMe->m_pShell,
                                       0,
                                       EVT_COMMAND,
                                       0,
                                       0);
               break;

#ifdef FEATURE_IDLE_CALL
            case AVK_SEND:
               {
                  uint16 curSel = IMENUCTL_GetSel(menu);

                  if (OPTIONS_MENU_ID == curSel) {
                     break;
                  }

                  pMe->m_fieldSelId = curSel;

                  if (TRUE == pMe->m_selectingContact) {
                     CLOSE_DIALOG(DLGRET_CONTACT_FIELDSEL)
                     return TRUE;
                  } else {
                     CLOSE_DIALOG(DLGRET_CONTACT_SENDKEY)
                     return TRUE;
                  }
               }
#endif /* FEATURE_IDLE_CALL */

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // Note: we don't use wParam for the current menu selection
         //       because the EVT_COMMAND event may have been synthesized
         //       (ie. by an AVK_RIGHT keypress) and wParam would be invalid
         //       in that case.
         pMe->m_fieldSelId = IMENUCTL_GetSel(menu);

         // Check for 'Options'
         if (OPTIONS_MENU_ID == pMe->m_fieldSelId) {
            CLOSE_DIALOG(DLGRET_CONTACT_OPTS)
            return TRUE;
         }
         //if it is speed dial then we do not want to show any
         //field options
         if (STR_CONTACTLIST_SPEEDDIAL != pMe->m_contactListType) {
            CLOSE_DIALOG(DLGRET_CONTACT_FIELDSEL)
         }
#ifdef FEATURE_PHONE_VR
         else {
            if (pMe->m_fldrecs_has_vt)
               IVR_PlayUtterance( pMe->m_pVR, pMe->m_fldrecs_vt);

            CLOSE_DIALOG(DLGRET_CONTACT_FIELDSEL)
         }
#endif
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_ContactDialogEvent()


/*=============================================================================
FUNCTION: ContDlgHandler_ContactOptionsDialogEvent

DESCRIPTION:
   Dialog events handler for the Contact options dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_ContactOptionsDialogEvent(CContactApp *pMe,
                                                        AEEEvent     eCode,
                                                        uint16       wParam,
                                                        uint32       dwParam)
{
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  CONTACTOPTIONS_MENU);

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   if (NULL == menu) {
      return FALSE;
   }

   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         {
            ContactInfo *ci;

            ci = IVector_ElementAt(pMe->m_contact_iv,
                                   pMe->m_contactSelId);
            if (NULL == ci)
            {
               MSG_ERROR("NULL contact info at index %d.", 
                         pMe->m_contactSelId, 0, 0);
               return FALSE;
            }

            //if this contact is on RUIM or speed dial
            //then show different options
#ifdef FEATURE_MMGSDI
            if ((ci->m_class == AEE_ADDR_CAT_SIM_ADN)||
                (ci->m_class == AEE_ADDR_CAT_SIM_FDN)||
                (ci->m_class == AEE_ADDR_CAT_SIM_ADN2)||
                (ci->m_class == AEE_ADDR_CAT_SIM_FDN2)||
                (ci->m_class == AEE_ADDR_CAT_SPEEDDIAL)) 
            {

                /* You can't delete an entry if it hasn't been stored to the
                * UIM yet.  So, we turn off the Delete option under ADDNEW */
                if (pMe->m_pinRetState == STATE_RUIM_ADDNEW)
                {
                    (void)IMENUCTL_DeleteItem(menu,
                                          CONTACTOPTIONS_MENUITEM_DELETE);
                }
                if ((ci->m_class == AEE_ADDR_CAT_SIM_FDN)||
                    (ci->m_class == AEE_ADDR_CAT_SIM_FDN2)||
                    (ci->m_class == AEE_ADDR_CAT_SPEEDDIAL)) 
                {
                    (void)IMENUCTL_DeleteItem(menu,CONTACTOPTIONS_MENUITEM_ADDNEW);
                }
                (void)IMENUCTL_DeleteItem(menu,CONTACTOPTIONS_MENUITEM_CLASS);
            }
#else
            if (ci->m_class == AEE_ADDR_CAT_SPEEDDIAL) {
               (void)IMENUCTL_DeleteItem(menu,CONTACTOPTIONS_MENUITEM_ADDNEW);
               (void)IMENUCTL_DeleteItem(menu,CONTACTOPTIONS_MENUITEM_CLASS);
            }
#endif /* FEATURE_MMGSDI*/
            if(ci->m_class == AEE_ADDR_CAT_SPEEDDIAL){
               (void)IMENUCTL_DeleteItem(menu,CONTACTOPTIONS_MENUITEM_EDITNAME);
            }
            ContDlgHandler_SetMenuStyle(pMe->m_pShell, menu);

            IMENUCTL_SetSel(menu, pMe->m_contactOptDlg_menuSel);
            return TRUE;
         }
      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            pMe->m_contactOptDlg_menuSel = IMENUCTL_GetSel(menu);
         }
         return TRUE;

      case EVT_KEY:
         switch (wParam) {
            case AVK_CLR:
            case AVK_LEFT:
               CLOSE_DIALOG(DLGRET_CONT_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         pMe->m_contactOptDlg_menuSel = IMENUCTL_GetSel(menu);

         switch (wParam) {
            case CONTACTOPTIONS_MENUITEM_ADDNEW:
               CLOSE_DIALOG(DLGRET_CONTACTOPTS_ADDNEW)
               return TRUE;

            case CONTACTOPTIONS_MENUITEM_EDITNAME:
               CLOSE_DIALOG(DLGRET_CONTACTOPTS_EDITNAME)
               return TRUE;

            case CONTACTOPTIONS_MENUITEM_CLASS:
               CLOSE_DIALOG(DLGRET_CONTACTOPTS_EDITCLASS)
               return TRUE;

            case CONTACTOPTIONS_MENUITEM_DELETE:
               CLOSE_DIALOG(DLGRET_CONTACTOPTS_DELETE)
               return TRUE;

            default:
               MSG_ERROR("Unrecognized EVT_COMMAND, %d.", wParam, 0, 0);
         }
         break;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_ContactOptionsDialogEvent()


/*=============================================================================
FUNCTION: ContDlgHandler_FieldOptionsDialogEvent

DESCRIPTION:
   Dialog events handler for the Field options dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_FieldOptionsDialogEvent(CContactApp *pMe,
                                                      AEEEvent     eCode,
                                                      uint16       wParam,
                                                      uint32       dwParam)
{
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  FIELDOPTIONS_MENU);

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   if (NULL == menu) {
      return FALSE;
   }

   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         {
            ContactInfo *ci;

            ci = IVector_ElementAt(pMe->m_contact_iv,
                                   pMe->m_contactSelId);
            if (NULL == ci)
            {
               MSG_ERROR("NULL contact info at index %d.", 
                         pMe->m_contactSelId, 0, 0);
               return FALSE;
            }

            ContDlgHandler_SetMenuStyle(pMe->m_pShell, menu);

            if ((STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) ||
                (STR_FIELDINPUT_ADDNUMBER == pMe->m_fieldInfo.m_type)) {
               (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_VIEWADDRESS);

#ifdef FEATURE_PHONE_VR
               if (ci->m_class == AEE_ADDR_CAT_SPEEDDIAL) {
                  if (pMe->m_fldrecs_has_vt) {
                    (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_ADDVOICETAG);
                  } else {
                    (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_RETRAIN);
                  }
                  (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_ADDTOSPEEDDIAL);

                  (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_NUMBERTYPE);
                  (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_EDIT);
                  (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_DELETE);
                  (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_RUIM);
                  (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_RUIM2);
               } else {
                  (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_RETRAIN);
               }
#else
               (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_ADDVOICETAG);
               (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_RETRAIN);
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_MMGSDI
               //if this contact is on RUIM then only edit,save on phone
               // and Add to Speed Dial is valid,so remove all the other items

               if ((ci->m_class == AEE_ADDR_CAT_SIM_ADN) ||
                   (ci->m_class == AEE_ADDR_CAT_SIM_ADN2) ||
                   (ci->m_class == AEE_ADDR_CAT_SIM_FDN2)||
                   (ci->m_class == AEE_ADDR_CAT_SIM_FDN) ||
                   (ci->m_class == AEE_ADDR_CAT_SIM_SDN2)|| 
                   (ci->m_class == AEE_ADDR_CAT_SIM_SDN))
               {
                  (void) IMENUCTL_DeleteItem(menu,
                                             FIELDOPTIONS_MENUITEM_NUMBERTYPE);
                  (void) IMENUCTL_DeleteItem(menu,
                                             FIELDOPTIONS_MENUITEM_RUIM);
                  (void) IMENUCTL_DeleteItem(menu,
                                             FIELDOPTIONS_MENUITEM_RUIM2);

                  if ((ci->m_class == AEE_ADDR_CAT_SIM_FDN) ||
                    (ci->m_class == AEE_ADDR_CAT_SIM_FDN2)) {
                     (void) IMENUCTL_DeleteItem(menu,
                                         FIELDOPTIONS_MENUITEM_ADDTOSPEEDDIAL);
                     (void) IMENUCTL_DeleteItem(menu,
                                         FIELDOPTIONS_MENUITEM_PHONE);
                     (void) IMENUCTL_DeleteItem(menu,
                                         FIELDOPTIONS_MENUITEM_ADDVOICETAG);
                  }
                  // if this contact is SDN then only speeddial and voice
                  // tag options are valid since SDN is not editable
                  // remove all other options
                 if ((ci->m_class == AEE_ADDR_CAT_SIM_SDN) ||
                    (ci->m_class == AEE_ADDR_CAT_SIM_SDN2)) 
                 {
                        (void) IMENUCTL_DeleteItem(menu,
                                         FIELDOPTIONS_MENUITEM_VIEWADDRESS);
                        (void) IMENUCTL_DeleteItem(menu,
                                         FIELDOPTIONS_MENUITEM_EDIT);
                        (void) IMENUCTL_DeleteItem(menu,
                                         FIELDOPTIONS_MENUITEM_PHONE);
                 }
               } else
#endif /* FEATURE_MMGSDI */
               {
                  (void) IMENUCTL_DeleteItem(menu,
                                             FIELDOPTIONS_MENUITEM_PHONE);
#ifdef FEATURE_MMGSDI
                  /* Delete Save to UIM menu if that UIM is not present */
                  if (!ContDlgHandler_IsCardAvailable(pMe, AEECARD_SLOT1)) {
                     (void) IMENUCTL_DeleteItem(menu,
                                                FIELDOPTIONS_MENUITEM_RUIM);
                  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                  if (!ContDlgHandler_IsCardAvailable(pMe, AEECARD_SLOT2)) {
                     (void) IMENUCTL_DeleteItem(menu,
                                                FIELDOPTIONS_MENUITEM_RUIM2);
                  }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
#endif /* FEATURE_MMGSDI */

               }
            } else {
               (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_ADDTOSPEEDDIAL);

               (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_ADDVOICETAG);

               (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_RETRAIN);

               (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_NUMBERTYPE);

               // only phone numbers can be saved on the RUIM
               (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_RUIM);

               (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_RUIM2);

               (void) IMENUCTL_DeleteItem(menu,
                                          FIELDOPTIONS_MENUITEM_PHONE);
               if ((STR_FIELDINPUT_SECNAME == pMe->m_fieldInfo.m_type) ||
                   (STR_FIELDINPUT_ID == pMe->m_fieldInfo.m_type))
               {
                  (void) IMENUCTL_DeleteItem(menu,
                                             FIELDOPTIONS_MENUITEM_VIEWADDRESS);
                  pMe->m_fieldOptsDlg_menuSel = FIELDOPTIONS_MENUITEM_EDIT;
               }
               else if (FIELDOPTIONS_MENUITEM_NUMBERTYPE == pMe->m_fieldOptsDlg_menuSel) {
                  pMe->m_fieldOptsDlg_menuSel = FIELDOPTIONS_MENUITEM_VIEWADDRESS;
               }
            }

#ifndef FEATURE_MMGSDI
            // Remove this option if no RUIM support
            (void) IMENUCTL_DeleteItem(menu,
                                       FIELDOPTIONS_MENUITEM_RUIM);
#endif

#ifndef FEATURE_MMGSDI_DUAL_SLOT
            (void) IMENUCTL_DeleteItem(menu,
                                       FIELDOPTIONS_MENUITEM_RUIM2);
#endif

            IMENUCTL_SetSel(menu,
                            pMe->m_fieldOptsDlg_menuSel);

            (void) IMENUCTL_Redraw(menu);
            return TRUE;
         }

      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            pMe->m_fieldOptsDlg_menuSel = IMENUCTL_GetSel(menu);
         }
         return TRUE;

      case EVT_KEY:
         switch (wParam) {
            case AVK_CLR:
            case AVK_LEFT:
               CLOSE_DIALOG(DLGRET_CONT_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         pMe->m_fieldOptsDlg_menuSel = IMENUCTL_GetSel(menu);

         switch (wParam)
         {
           case FIELDOPTIONS_MENUITEM_VIEWADDRESS:
             CLOSE_DIALOG(DLGRET_FIELDOPTS_VIEWADDR)
             return TRUE;

           case FIELDOPTIONS_MENUITEM_NUMBERTYPE:
             CLOSE_DIALOG(DLGRET_FIELDOPTS_NUMBERTYPE)
             return TRUE;

           case FIELDOPTIONS_MENUITEM_EDIT:
             CLOSE_DIALOG(DLGRET_FIELDOPTS_EDIT)
             return TRUE;

           case FIELDOPTIONS_MENUITEM_ADDTOSPEEDDIAL:
             CLOSE_DIALOG(DLGRET_FIELDOPTS_ADDTOSPEEDDIAL)
             return TRUE;

#ifdef FEATURE_PHONE_VR
           case FIELDOPTIONS_MENUITEM_ADDVOICETAG:
             CLOSE_DIALOG(DLGRET_FIELDOPTS_ADDVOICETAG)
             return TRUE;

           case FIELDOPTIONS_MENUITEM_RETRAIN:
             CLOSE_DIALOG(DLGRET_FIELDOPTS_RETRAIN)
             return TRUE;
#endif

#ifdef FEATURE_MMGSDI
           case FIELDOPTIONS_MENUITEM_RUIM:
             CLOSE_DIALOG(DLGRET_FIELDOPTS_RUIM)
             return TRUE;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
           case FIELDOPTIONS_MENUITEM_RUIM2:
             CLOSE_DIALOG(DLGRET_FIELDOPTS_RUIM2)
             return TRUE;
#endif

           case FIELDOPTIONS_MENUITEM_PHONE:
             CLOSE_DIALOG(DLGRET_FIELDOPTS_PHONE)
             return TRUE;
#endif

           case FIELDOPTIONS_MENUITEM_DELETE:
             CLOSE_DIALOG(DLGRET_FIELDOPTS_DELETE)
             return TRUE;

           default:
             MSG_ERROR("Unrecognized EVT_COMMAND, %d.", wParam, 0, 0);
             break;
         }
         return FALSE;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_FieldOptionsDialogEvent()


/*=============================================================================
FUNCTION: ContDlgHandler_YesNoDialogEvent

DESCRIPTION:
   Dialog events handler for the Yes no Dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.



   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_YesNoDialogEvent(CContactApp *pMe,
                                               AEEEvent     eCode,
                                               uint16       wParam,
                                               uint32       dwParam)
{
#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
 #endif

   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         {
            AEERect rc;
            AECHAR  szText[MSGBOX_MAXTEXTLEN];
            IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                           YESNO_MENU);
            if (NULL == menu) {
               return FALSE;
            }

            IMENUCTL_GetRect(menu,
                             &rc);

            rc.dy = rc.y;
            rc.dx = (int16)pMe->m_BmpInfo.cx;
            rc.y = 0;
            rc.x = 0;

            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        CONTACTAPP_RES_FILE,
                                        pMe->m_yesnoDialogTitle,
                                        szText,
                                        sizeof(szText));

            (void) ISHELL_CreateInstance(pMe->m_pShell,
                                         AEECLSID_STATIC,
                                         (void**)&pMe->m_msgBox_IStatic);

            ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

            ISTATIC_SetProperties(pMe->m_msgBox_IStatic,
                                  ST_CENTERTEXT | ST_MIDDLETEXT);

            (void) ISTATIC_SetText(pMe->m_msgBox_IStatic,
                                   NULL,
                                   szText,
                                   AEE_FONT_NORMAL,
                                   AEE_FONT_NORMAL);

            if (pMe->m_yesnoTimeout) {
               IMENUCTL_SetSel(menu, YESNO_MENUITEM_NO);
               (void) ISHELL_SetTimer(pMe->m_pShell,
                                      YESNO_TIMEOUT,
                                      ContDlgHandler_DialogTimeout_Callback,
                                      pMe);
            }

            // KLUDGE!  Need to wait for the Dialog to draw itself before
            //          the ISTATIC_Redraw() command can be issued or the
            //          Dialog will overwrite the IStatic control
            return ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_CONTACT_APP,
                                    EVT_USER,
                                    CONT_EVENT_REDRAW,
                                    0);
         }

      case EVT_USER:
         if ((ContEventType)wParam == CONT_EVENT_REDRAW) {
            (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
            return TRUE;
         } else {
            if ((ContEventType)wParam == CONT_EVENT_DIALOG_TIMEOUT) {
               CLOSE_DIALOG(DLGRET_CONT_CANCELED)
               return TRUE;
            } else {
               return FALSE;
            }
         }

      case EVT_DIALOG_END:
         (void) ISHELL_CancelTimer(pMe->m_pShell,
                                   ContDlgHandler_DialogTimeout_Callback,
                                   pMe);

         if (NULL != pMe->m_msgBox_IStatic) {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;


      case EVT_KEY:
         switch (wParam) {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CONT_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         switch (wParam)
         {
           case YESNO_MENUITEM_NO:
             CLOSE_DIALOG(DLGRET_CONT_NO)
             return TRUE;

           case YESNO_MENUITEM_YES:
             CLOSE_DIALOG(DLGRET_CONT_YES)
             return TRUE;

           default:
             MSG_ERROR("Unrecognized EVT_COMMAND, %d.", wParam, 0, 0);
             break;
         }
         return FALSE;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_YesNoDialogEvent



/*=============================================================================
FUNCTION: ContDlgHandler_MsgBoxDialogEvent

DESCRIPTION:
   Dialog events handler for the Yes no Dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.



   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_MsgBoxDialogEvent(CContactApp *pMe,
                                                AEEEvent     eCode,
                                                uint16       wParam,
                                                uint32       dwParam)
{
#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   switch (eCode) {
      case EVT_DIALOG_INIT:
         {
            AEERect rc;
            AECHAR  szText[MSGBOX_MAXTEXTLEN];

            SETAEERECT(&rc,
                       0,
                       0,
                       pMe->m_BmpInfo.cx,
                       pMe->m_BmpInfo.cy);


            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        CONTACTAPP_RES_FILE,
                                        pMe->m_msgBox_textId,
                                        szText,
                                        sizeof(szText));

            (void) ISHELL_CreateInstance(pMe->m_pShell,
                                         AEECLSID_STATIC,
                                         (void**)&pMe->m_msgBox_IStatic);

            ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

            ISTATIC_SetProperties(pMe->m_msgBox_IStatic,
                                  ST_CENTERTEXT | ST_MIDDLETEXT);

            (void) ISTATIC_SetText(pMe->m_msgBox_IStatic,
                                   NULL,
                                   szText,
                                   AEE_FONT_NORMAL,
                                   AEE_FONT_NORMAL);


            (void) ISHELL_SetTimer(pMe->m_pShell,
                                   MSGBOX_TIMEOUT,
                                   ContDlgHandler_DialogTimeout_Callback,
                                   pMe);
            return TRUE;
         }

      case EVT_DIALOG_START:
         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         return ISHELL_PostEvent(pMe->m_pShell,
                                 AEECLSID_CONTACT_APP,
                                 EVT_USER,
                                 CONT_EVENT_REDRAW,
                                 0);

      case EVT_USER:
         if ((ContEventType)wParam == CONT_EVENT_REDRAW) {
            (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
            return TRUE;
         } else {
            if ((ContEventType)wParam == CONT_EVENT_DIALOG_TIMEOUT) {
               CLOSE_DIALOG(DLGRET_CONT_OK)
               return TRUE;
            } else {
               return FALSE;
            }
         }

      case EVT_DIALOG_END:
         (void) ISHELL_CancelTimer(pMe->m_pShell,
                                   ContDlgHandler_DialogTimeout_Callback,
                                   pMe);

         if (NULL != pMe->m_msgBox_IStatic) {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY:
         CLOSE_DIALOG(DLGRET_CONT_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_MsgBoxDialogEvent()


/*=============================================================================
FUNCTION: ContDlgHandler_ViewAddressDialogEvent

DESCRIPTION:
   Dialog events handler for the View Address dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_ViewAddressDialogEvent(CContactApp *pMe,
                                                     AEEEvent     eCode,
                                                     uint16       wParam,
                                                     uint32       dwParam)
{
#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   switch (eCode) {
      case EVT_DIALOG_INIT:
         {
            AEERect  rc;
            AECHAR   szTitle[MSGBOX_MAXTITLELEN];
            AECHAR  *name;

            (void) ISHELL_CreateInstance(pMe->m_pShell,
                                         AEECLSID_STATIC,
                                         (void**)&pMe->m_msgBox_IStatic);

            SETAEERECT(&rc,
                       0,
                       0,
                       pMe->m_BmpInfo.cx,
                       pMe->m_BmpInfo.cy - (uint32)pMe->m_lineHeight);

            ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

            szTitle[0] = 0;
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        CONTACTAPP_RES_FILE,
                                        pMe->m_fieldInfo.m_type,
                                        szTitle,
                                        sizeof(szTitle));

            if (pMe->m_viewAddressStr != NULL) {
               name = (AECHAR*)pMe->m_viewAddressStr;
            } else {
               name = pMe->m_fieldInfo.m_name;
            }

            (void) ISTATIC_SetText(pMe->m_msgBox_IStatic,
                                   szTitle,
                                   name,
                                   AEE_FONT_BOLD,
                                   AEE_FONT_LARGE);

            return TRUE;
         }

      case EVT_DIALOG_START:
         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         return ISHELL_PostEvent(pMe->m_pShell,
                                 AEECLSID_CONTACT_APP,
                                 EVT_USER,
                                 CONT_EVENT_REDRAW,
                                 0);

      case EVT_USER:
         if ((ContEventType)wParam == CONT_EVENT_REDRAW) {
            (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
            return TRUE;
         } else {
            return FALSE;
         }


      case EVT_DIALOG_END:
         if (NULL != pMe->m_msgBox_IStatic) {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY_PRESS:
         // Send Up/Down key events to the ISTATIC control to allow
         // the user to scroll the address if necessary
         //
         // Note that we use the EVT_KEY_PRESS event instead of EVT_KEY
         // because there is an active IMenuCtl on the screen.  Active
         // IMenuCtl's currently always handle the Up/Down EVT_KEY events
         // so the applet will never see them.
         if ((AVK_UP   == (AVKType)wParam) ||
             (AVK_DOWN == (AVKType)wParam)) {
            return ISTATIC_HandleEvent(pMe->m_msgBox_IStatic,
                                       EVT_KEY,
                                       wParam,
                                       dwParam);
         }
         break;

      case EVT_KEY:
         if (AVK_CLR == (AVKType)wParam) {
            CLOSE_DIALOG(DLGRET_CONT_CANCELED)
            return TRUE;
         }
         break;

      case EVT_COMMAND:
         CLOSE_DIALOG(DLGRET_CONT_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // ContDlgHandler_ViewAddressDialogEvent()


/*=============================================================================
FUNCTION: ContDlgHandler_RUIMDialogEvent

DESCRIPTION:
   Dialog events handler for the RUIM Dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_RUIMDialogEvent(CContactApp *pMe,
                                              AEEEvent     eCode,
                                              uint16       wParam,
                                              uint32       dwParam)
{
#ifndef FEATURE_MMGSDI
   PARAM_NOT_REF(pMe)
   PARAM_NOT_REF(eCode)
   PARAM_NOT_REF(wParam)
   PARAM_NOT_REF(dwParam)

   return FALSE;

#else
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  RUIM_MENU);

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

   if (NULL == menu) {
      return FALSE;
   }

   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         {
            uint16 count =0;

            count = (uint16)IVector_Size(pMe->m_contact_iv);

            if (count>0) {
               ContDlgHandler_PopulateContactListMenu(pMe,
                                                      menu,
                                                      0,
                                                      count,
                                                      TRUE);
            }

            ContDlgHandler_SetMenuStyle(pMe->m_pShell, menu);

            if (pMe->m_ruimDlg_menuSel >= count) {
               pMe->m_ruimDlg_menuSel = 0;
            }

            IMENUCTL_EnableCommand(menu, TRUE);
            IMENUCTL_SetSel(menu, pMe->m_contactSelId);

            (void) IDIALOG_SetFocus(pMe->m_activeDlg,
                                    RUIM_MENU);

            return TRUE;
         }

      case EVT_DIALOG_END:
         pMe->m_ruimDlg_menuSel = IMENUCTL_GetSel(menu);
         pMe->m_contactSelId = IMENUCTL_GetSel(menu);
         return TRUE;

      case EVT_KEY_PRESS:
         switch ((AVKType)wParam) {
            case AVK_UP:
            case AVK_DOWN:
               pMe->m_contactSelId = IMENUCTL_GetSel(menu);
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_KEY:
         switch ((AVKType)wParam) {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CONT_CANCELED);
               return TRUE;

#ifdef FEATURE_IDLE_CALL
            case AVK_SEND:
               CLOSE_DIALOG(DLGRET_RUIM_CONTACTLIST_SENDKEY)
               return TRUE;
#endif/*FEATURE_IDLE_CALL*/

            case AVK_UP:
            case AVK_DOWN:
               (void) IDIALOG_SetFocus(pMe->m_activeDlg,
                                       RUIM_MENU);
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         pMe->m_ruimDlg_menuSel = IMENUCTL_GetSel(menu);
         pMe->m_contactSelId    = IMENUCTL_GetSel(menu);

         CLOSE_DIALOG(DLGRET_RUIM_CONTACTOPTS);
         return TRUE;

      case EVT_KEY_RELEASE:
         switch ((AVKType)wParam) {

            //If we are at the top of contact list then scroll over to the last
            //contact
            case AVK_UP:
               if (pMe->m_contactSelId == 0) {
                  pMe->m_contactSelId=(uint16)IVector_Size(pMe->m_contact_iv)-1;
                  IMENUCTL_SetSel(menu, pMe->m_contactSelId);
               }
               break;

            //If we are bottom of the the contacts list then scroll
            //over to top of //the list
            case AVK_DOWN:
               if (pMe->m_contactSelId ==
                  (uint16)IVector_Size(pMe->m_contact_iv)-1) {
                  pMe->m_contactSelId = 0;
                  IMENUCTL_SetSel(menu, pMe->m_contactSelId);
               }
               break;

            default:
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
#endif /*FEATURE_MMGSDI*/
//lint -save -esym(715, pMe) -esym(715, wParam)  Suppress unused arg
//lint -save -esym(715, dwParam) -esym(715, eCode)
} // ContDlgHandler_RUIMDialogEvent()
//lint +esym(715, pMe) +esym(715, wParam)
//lint +esym(715, dwParam) +esym(715, eCode)

/*=============================================================================
FUNCTION: ContDlgHandler_SpeedDialDialogEvent

DESCRIPTION:
   Dialog events handler for the Speed Dial Dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_SpeedDialDialogEvent(CContactApp *pMe,
                                                   AEEEvent     eCode,
                                                   uint16       wParam,
                                                   uint32       dwParam)
{
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  SPEEDDIAL_MENU_ENTRIES);

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   if (NULL == menu) {
      return FALSE;
   }

   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         pMe->m_speedDialSelId = 0;
         pMe->m_speedDialListMenuStart = 0;
         //initialize the predict keys
         pMe->m_indexSpeedDial[0] = AVK_0;
         pMe->m_indexSpeedDial[1] = AVK_0;

#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
         ContDlgHandler_SetMenuStyle(pMe->m_pShell, menu);

         if (pMe->m_speedDialSelId > 0) {

            if (pMe->m_speedDialSelId+1 < MAX_SPEED_DIAL_NUMS) {
               // If in the middle of the menu list, start at one
               // item above the selected item
               pMe->m_speedDialListMenuStart = pMe->m_speedDialSelId - 1;
            } else {
               if (pMe->m_speedDialSelId >= MAX_SPEED_DIAL_NUMS) {
                  pMe->m_speedDialSelId = 0;
                  pMe->m_speedDialListMenuStart = 0;
               } else {
                  int menuStart=
                     (pMe->m_speedDialSelId - (pMe->m_numTextOnlyLines-1)) + 1;

                  if (menuStart < 0) {
                     menuStart = 0;
                  }
                  pMe->m_speedDialListMenuStart = (uint16)menuStart;
               }
            }
         } else {
            pMe->m_speedDialListMenuStart = 0;
         }
         // Populate all the contacts with the numbers
         // and empty or the contact name string
         ContDlgHandler_PopulateSpeedDialListMenu(pMe,
                                                menu,
                                                pMe->m_speedDialListMenuStart,
                                                pMe->m_numTextOnlyLines-1,
                                                TRUE);
#else
         ContDlgHandler_SetMenuStyle(pMe->m_pShell, menu);
         // Populate all the contacts with the numbers
         // and empty or the contact name string
         ContDlgHandler_PopulateSpeedDialListMenu(pMe,
                                               menu,
                                               0,
                                               MAX_SPEED_DIAL_NUMS,
                                               TRUE);
#endif

         return TRUE;

      case EVT_KEY_PRESS:
         switch ((AVKType)wParam) {

            case AVK_UP:
               //initialize the predict keys
               pMe->m_indexSpeedDial[0] = AVK_0;
               pMe->m_indexSpeedDial[1] = AVK_0;

#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
               pMe->m_speedDialSelId = IMENUCTL_GetSel(menu);

               if ( (pMe->m_speedDialSelId > 0) &&
                    (pMe->m_speedDialSelId == pMe->m_speedDialListMenuStart) ) {
                  pMe->m_speedDialListMenuStart--;
                  ContDlgHandler_PopulateSpeedDialListMenu(pMe,
                                                menu,
                                                pMe->m_speedDialListMenuStart,
                                                pMe->m_numTextOnlyLines-1,
                                                TRUE);
               }
#endif /* defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW) */
               return TRUE;

            case AVK_DOWN:
               //initialize the predict keys
               pMe->m_indexSpeedDial[0] = AVK_0;
               pMe->m_indexSpeedDial[1] = AVK_0;

#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
               {
                  uint32 menuEnd;
                  uint16 nOldSpeedDialSelId = pMe->m_speedDialSelId;

                  pMe->m_speedDialSelId = IMENUCTL_GetSel(menu);

                  menuEnd = (pMe->m_speedDialListMenuStart +
                             (pMe->m_numTextOnlyLines-1)) - 1;
                  if (menuEnd < MAX_SPEED_DIAL_NUMS) {
                     if ((uint32)pMe->m_speedDialSelId > menuEnd)
                     {
                        /* Restore the old selection and error. */
                        pMe->m_speedDialSelId = nOldSpeedDialSelId;
                        MSG_ERROR("Selection out of range, %d.",
                                  (uint32)pMe->m_speedDialSelId, 0, 0);
                        return TRUE;
                     }

                     if (((uint32)pMe->m_speedDialSelId == menuEnd) &&
                         ((uint32)pMe->m_speedDialSelId+1 <
                          MAX_SPEED_DIAL_NUMS)) {
                        (void)IMENUCTL_DeleteItem(menu,
                                                 pMe->m_speedDialListMenuStart);

                        pMe->m_speedDialListMenuStart++;

                        ContDlgHandler_PopulateSpeedDialListMenu(pMe,
                                                              menu,
                                                              (uint16)menuEnd+1,
                                                              1,
                                                              FALSE);
                     }
                  }
               }
#endif /* defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW) */
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_KEY_RELEASE:
         switch ((AVKType)wParam) {

            //If we are at the top of contact list then scroll over to the last
            //contact
            case AVK_UP:
#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
               if (pMe->m_speedDialSelId == 0) {
                  pMe->m_speedDialSelId=(uint16)MAX_SPEED_DIAL_NUMS-1;

                  //If total number of contacts are more than number of lines
                  // that can be displayed then repopulate the list else
                  //just change the current selection
                  if ((uint16)MAX_SPEED_DIAL_NUMS > pMe->m_numTextOnlyLines-1) {
                     pMe->m_speedDialListMenuStart = (pMe->m_speedDialSelId -
                                                 (pMe->m_numTextOnlyLines-1))+1;
                     ContDlgHandler_PopulateSpeedDialListMenu(pMe,
                                                   menu,
                                                   pMe->m_speedDialListMenuStart,
                                                   pMe->m_numTextOnlyLines-1,
                                                   TRUE);
                     (void) IMENUCTL_Redraw(menu);
                  } else {
                     IMENUCTL_SetSel(menu, pMe->m_speedDialSelId);
                  }
               }
#else
               pMe->m_speedDialSelId = IMENUCTL_GetSel(menu);
               if (pMe->m_speedDialSelId == 0) {
                  pMe->m_speedDialSelId=MAX_SPEED_DIAL_NUMS-1;
                  IMENUCTL_SetSel(menu, pMe->m_speedDialSelId);
               }
#endif /* defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW) */
               break;

               //If we are bottom of the the contacts list then scroll
               //over to top of //the list
            case AVK_DOWN:
#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
               if (pMe->m_speedDialSelId==MAX_SPEED_DIAL_NUMS-1) {
                  pMe->m_speedDialSelId = 0;

                  //If total number of contacts are more than number of lines
                  // that can be displayed then repopulate the list else
                  //just change the current selection
                  if (MAX_SPEED_DIAL_NUMS > pMe->m_numTextOnlyLines-1) {
                     pMe->m_speedDialListMenuStart = 0;
                     ContDlgHandler_PopulateSpeedDialListMenu(pMe,
                                                  menu,
                                                  pMe->m_speedDialListMenuStart,
                                                  pMe->m_numTextOnlyLines-1,
                                                  TRUE);
                     (void) IMENUCTL_Redraw(menu);
                  } else {
                     IMENUCTL_SetSel(menu, pMe->m_speedDialSelId);
                  }
               }
#else
               pMe->m_speedDialSelId = IMENUCTL_GetSel(menu);
               if (pMe->m_speedDialSelId ==MAX_SPEED_DIAL_NUMS-1) {
                  pMe->m_speedDialSelId = 0;
                  IMENUCTL_SetSel(menu, pMe->m_speedDialSelId);
               }
#endif /* defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW) */
               break;

            default:
               break;
         }
         break;


      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            pMe->m_speedDialSelId = IMENUCTL_GetSel(menu);
         }
         (void) ISHELL_CancelTimer(pMe->m_pShell,
                                   ContDlgHandler_SpeedDialMenuKeyTimer,
                                   pMe);
         return TRUE;

      case EVT_KEY:
         switch ((AVKType)wParam) {
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
               //initialize the predict keys
               pMe->m_indexSpeedDial[0] = pMe->m_indexSpeedDial[1];
               pMe->m_indexSpeedDial[1] = (AVKType)wParam;
               (void) ISHELL_SetTimer((IShell *) pMe->m_pShell,
                                       SPEEDDIAL_MENUKEY_TIMEOUT,
                                       ContDlgHandler_SpeedDialMenuKeyTimer,
                                       pMe);

               break;

            case AVK_CLR:
            case AVK_LEFT:
               CLOSE_DIALOG(DLGRET_CONT_CANCELED);
               return TRUE;
            default:
               break;
         }
         break;

      case EVT_COMMAND:
         pMe->m_speedDialSelId = IMENUCTL_GetSel(menu);
         CLOSE_DIALOG(DLGRET_CONT_OK)
         break;

      default:
         break;
   }
   return FALSE;
} // ContDlgHandler_SpeedDialDialogEvent()

/*=============================================================================
FUNCTION: ContDlgHandler_VoiceTagDialogEvent

DESCRIPTION:
   Dialog events handler for the Voice Tag ISTATIC Dialog

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:
   Reports any status change provided by the callback function registered
   using IVR_RegisterNotify.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_VoiceTagDialogEvent(CContactApp *pMe,
                                                   AEEEvent     eCode,
                                                   uint16       wParam,
                                                   uint32       dwParam)
{
#ifdef FEATURE_PHONE_VR

   AECHAR  szText[MSGBOX_MAXTEXTLEN];
   AECHAR  szTitle[MSGBOX_MAXTITLELEN];

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   switch (eCode) {
      case EVT_DIALOG_INIT:
         {
            ContactInfo *ci;
            AEERect rc;

            ci = IVector_ElementAt(pMe->m_contact_iv,
                                   pMe->m_contactSelId);
            if (NULL == ci)
            {
               MSG_ERROR("NULL contact info at index %d.",
                         pMe->m_contactSelId, 0, 0);
               return FALSE;
            }

            SETAEERECT(&rc,
                       0,
                       0,
                       pMe->m_BmpInfo.cx,
                       pMe->m_BmpInfo.cy);


            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        CONTACTAPP_RES_FILE,
                                        STR_VOICETAG_TRAIN_TEXT,
                                        szText,
                                        sizeof(szText));

            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        CONTACTAPP_RES_FILE,
                                        STR_VOICETAG_TRAIN_TITLE,
                                        szTitle,
                                        sizeof(szTitle));

            (void) ISHELL_CreateInstance(pMe->m_pShell,
                                         AEECLSID_STATIC,
                                         (void**)&pMe->m_voiceTag_IStatic);

            ISTATIC_SetRect(pMe->m_voiceTag_IStatic, &rc);

            ISTATIC_SetProperties(pMe->m_voiceTag_IStatic,
                                  ST_CENTERTEXT | ST_MIDDLETEXT);

            WSTRLCAT(szText, ci->m_name, ARR_SIZE(szText));

            (void) ISTATIC_SetText(pMe->m_voiceTag_IStatic,
                                   szTitle,
                                   szText,
                                   AEE_FONT_BOLD,
                                   AEE_FONT_NORMAL);
            return TRUE;
         }

      case EVT_DIALOG_START:
         ISHELL_PostEvent(pMe->m_pShell,
                          AEECLSID_CONTACT_APP,
                          EVT_USER,
                          CONT_EVENT_REDRAW,
                          0);
         if (ContDlgHandler_VoiceTag_Create(pMe) != SUCCESS) {
            ISHELL_PostEvent(pMe->m_pShell,
                          AEECLSID_CONTACT_APP,
                          EVT_USER,
                          CONT_EVENT_VR_FAILED,
                          0);
         }
         return TRUE;


      case EVT_DIALOG_END:
         if (pMe->m_voiceTag_IStatic) {
            ISTATIC_Reset(pMe->m_voiceTag_IStatic);
            (void) ISTATIC_Release(pMe->m_voiceTag_IStatic);
            pMe->m_voiceTag_IStatic = NULL;
         }
         return TRUE;

      case EVT_USER:
         switch((ContEventType)wParam) {

            case CONT_EVENT_VR_DONE:
               CLOSE_DIALOG(DLGRET_VR_DONE);
               return TRUE;

            case CONT_EVENT_VR_RUNNING:
               (void) ISHELL_LoadResString(pMe->m_pShell,
                                           CONTACTAPP_RES_FILE,
                                           STR_VOICETAG_TRAIN_TEXT_AGAIN,
                                           szText,
                                           sizeof(szText));

               (void) ISHELL_LoadResString(pMe->m_pShell,
                                           CONTACTAPP_RES_FILE,
                                           STR_VOICETAG_TRAIN_TITLE,
                                           szTitle,
                                           sizeof(szTitle));

               (void) ISTATIC_SetText(pMe->m_voiceTag_IStatic,
                                      szTitle,
                                      szText,
                                      AEE_FONT_BOLD,
                                      AEE_FONT_NORMAL);

               (void) ISTATIC_Redraw(pMe->m_voiceTag_IStatic);

               return TRUE;

            case CONT_EVENT_VR_FAILED:
               CLOSE_DIALOG(DLGRET_VR_FAILED);
               return TRUE;

            case CONT_EVENT_VR_ABORTED:
               CLOSE_DIALOG(DLGRET_VR_ABORTED);
               return TRUE;

            case CONT_EVENT_REDRAW:
               (void) ISTATIC_Redraw(pMe->m_voiceTag_IStatic);
               return TRUE;

            default:
               return FALSE;
         }

      case EVT_KEY:
         if (AVK_CLR == (AVKType) wParam) {
            if (pMe->m_pVR) {
               pMe->m_userAbort = TRUE;
               IVR_AbortOperation(pMe->m_pVR);
            }
            CLOSE_DIALOG(DLGRET_CONT_CANCELED);
            return TRUE;
         }
         break;

      default:
         break;
   }
#endif /* FEATURE_PHONE_VR */
   return FALSE;
} // ContDlgHandler_VoiceTagDialogEvent()


/*=============================================================================
FUNCTION: ContDlgHandler_PIN2DialogEvent

DESCRIPTION:
   Dialog events handler for the PIN2 dialog.

PARAMETERS:
   pi [in]: Pointer to the Contacts Applet structure. This structure contains
            information specific to this applet.

   ecode [in]: Specifies the Event sent to this applet

   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContDlgHandler_PIN2DialogEvent(CContactApp *pMe,
                                                     AEEEvent     eCode,
                                                     uint16       wParam,
                                                     uint32       dwParam)
{
   ITextCtl *ctrl;

#ifdef FEATURE_BREW_3_0
   // do not want to handle autorepeat key events
   if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
   {
     return FALSE;
   }
#endif

   /* Get the pointers to the text and menu control */
   ctrl = (ITextCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                        PIN2_REQUEST);

   /* Exit if we don't have valid a pointer */
   if (NULL == ctrl) {
      return FALSE;
   }

   /* Check the event code */
   switch (eCode) {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START: /* Build the screen at start */
         {

            //AEERect textRect, menuRect;

            /*
             * Build the softkey menu manually.  This is necessary to
             * remove the item that the ITextCtl adds when entering a
             * phone number
             */

            /* Limit the number of digits to be entered */
            ITEXTCTL_SetMaxSize(ctrl, FLDLEN_NUMBER);

            /* Turn off Multiline, and turn on Password
             * (so we don't display digits) */
            ITEXTCTL_SetProperties(ctrl,
                              (ITEXTCTL_GetProperties(ctrl)& ~TP_MULTILINE) |
                              TP_FRAME | TP_PASSWORD);

            (void) ITEXTCTL_SetText(ctrl, NULL, 0);

            /* set the rect so that it does not clip, the menu control. */
            //ITEXTCTL_GetRect(ctrl, &textRect);
            //textRect.dy = pMe->m_rc.dy - menuRect.dy;
            //ITEXTCTL_SetRect(ctrl, &textRect);

            /* Allow only numbers for PINs */
            (void) ITEXTCTL_SetInputMode(ctrl, AEE_TM_NUMBERS);

            return TRUE;
         }

      case EVT_DIALOG_END:
         if (pMe->m_suspending) {
            FREE_STRING(pMe->m_contactInfo.m_name);
            pMe->m_contactInfo.m_name = WSTRDUP(ITEXTCTL_GetTextPtr(ctrl));
         }
         return TRUE;


      case EVT_KEY_PRESS: /* Standard key presses */
        switch ((AVKType)wParam) {
          case AVK_CLR:
            if (WSTRLEN(ITEXTCTL_GetTextPtr(ctrl)) <= 0) {
               pMe->m_fieldWasEmpty = TRUE;
            } else {
               pMe->m_fieldWasEmpty = FALSE;
            }
            break;

          case AVK_POUND:
          case AVK_SELECT:
               FREE_STRING(pMe->m_contactInfo.m_name);
               pMe->m_contactInfo.m_name = WSTRDUP(ITEXTCTL_GetTextPtr(ctrl));

               CLOSE_DIALOG(DLGRET_CONT_OK)
               return TRUE;

          default:
            break;
        }

        return FALSE;

      case EVT_KEY:
         if (AVK_CLR == (AVKType) wParam) {
            return TRUE;
         }
         break;

      case EVT_COMMAND:
         return TRUE;

      default:
         break;

   }

   return FALSE;
} // ContDlgHandler_PIN2DialogEvent

#endif /*FEATURE_APP_CONTACT*/
//lint -restore
