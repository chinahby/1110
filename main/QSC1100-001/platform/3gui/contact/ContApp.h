#ifndef __CONTAPP_H__
#define __CONTAPP_H__
/*=============================================================================

FILE: ContApp.h

SERVICES: Contact Applet

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
IContactApp

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002-2004 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/Contact/ContApp.h#23 $
  $DateTime: 2005/07/18 15:45:26 $
  $Author: aruns $
  $Change: 218254 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/18/05   aps     added a temperory variable to CContactApp structure
06/22/05   AT      Changed to match largest number than PBM can save.
04/19/05   jas     Replace sizing info with display bmp size
03/24/05   jas     Consolidating all the VR-related class object members
01/17/05   BW      Added USIM support - ability to add and view UIM fields
12/13/04   ssc     Changed include file name case to match the Linux names 
                   of the files. Mismatches in the case break AMSS builds 
                   on Linux servers.
11/15/04   PA      Fix max data length
07/22/04   PA      Changed m_simState to m_uimSlot1
06/23/04   PA      Support for enumeration of available contacts
04/07/04   PA      Added new variables in CContactApp for fixing multitap
01/23/04   tml     Added FEATURE_NEW_ICARD
12/22/03   tml     ICARD API Changes
01/19/04   AT      Added support for saving error messages. 
09/10/03   adm     Added a field m_simState to ContactApp to lend dual slot support.
08/07/03   jds     Added support for editing FDN.
06/03/03   PA      Changed FEATURE_RUIM_PHONEBOOK to FEATURE_MMGSDI
05/15/03   PA      Support for creating and retraining voice tags from speed 
                   dial list
05/08/03   PA      Support for Voice Tags
04/17/03   PA/AT   Changed value of SPEED_DIAL_USER_START_INDEX
02/06/03   kar     Code cleanup
11/15/02   pm      Port Contact App to QCT
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimfeatures.h"  // Simulator Feature Definitions
#endif


#if !defined(BREW_STATIC_APP)
   // Use the .bid file if compiling as dynamic applet
   #include "IContactApp.bid"       
#else
   #define AEECLSID_CONTACT_APP     0x01011e61
#endif

#include "AEE.h"
#include "AEEShell.h"           // Shell interface definitions
#include "AEEStdLib.h"
#include "AEEModGen.h"          // Module interface definitions
#include "AEEMenu.h"
#include "AEEText.h"
#include "AEEStdLib.h"        // AEE StdLib Services

#include "AEEVector.h"

#ifdef FEATURE_PHONE_VR
#include "AEEVR.h"
#include "OEMVR_priv.h"
#endif 

#include "AEEAddrBookExt.h"

#include "AppComFunc.h"

#include "ContStateMachine.h"
#include "ContDlgHandler.h"

// Uncomment this #define if you want the Contacts app to use
// the Idle applet to start calls when the Send key is pressed
// while viewing a contact
#define FEATURE_IDLE_CALL

#define FEATURE_CONTACTNAME_PREDICT

#define WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW

#ifdef FEATURE_IDLE_CALL
#include "CoreApp.h"
#endif


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

////
// All the ways that somebody can start the contacts app

typedef enum _AppletStartMethod {
   STARTMETHOD_NORMAL,                   //ISHELL_StartApplet()
   STARTMETHOD_SAVENUMBER_NUMBER_EDIT,   //Save number with phone edit option
   STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT,//Save number with no phone edit option
   STARTMETHOD_VIEWALL,                  //View all service called by other app
   STARTMETHOD_FINDNUMBER,               //find number called by other app
   STARTMETHOD_SELECTFIELD               //Select Field called by other app
}AppletStartMethod;

//These are different type of save option in STARTMETHOD_SAVENUMBER_NUMBER_EDIT
// and STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT.
typedef enum _PhoneNumberSaveType{
   SAVE_NEW,          //Add a new contact with this phone number on Phone(EFS)
   SAVE_TO_EXISTING,  //Add this number to a existing contact
   SAVE_TO_RUIM       //Add a new contact with this phone number on RUIM
}PhoneNumberSaveType;

////
// Valid first characters for startup arguments.
//
// See the CContactApp_DecodeStartArgs()
//
#define STARTARGPREFIX_SAVENUMBER_NUMBEREDIT    ((char) 'E')
#define STARTARGPREFIX_SAVENUMBER_NONUMBEREDIT  ((char) 'N')
#define STARTARGPREFIX_VIEWALL                  ((char) 'V')
#define STARTARGPREFIX_FINDNUMBER               ((char) '?')
#define STARTARGPREFIX_SELECTFIELD              ((char) 'F')

////
// The dialer pause character
#define DIALER_PAUSE_CHAR    ((char)'P')
#define DIALER_PAUSE_AECHAR  ((AECHAR)'P')


////
// Valid second characters for startup arguments in case of
// start method is STARTARGPREFIX_SAVENUMBER_NUMBEREDIT
// or STARTARGPREFIX_SAVENUMBER_NONUMBEREDIT
//
// See the CContactApp_DecodeStartArgs()
//
#define STARTARGPREFIX_SAVETYPE_SAVENEW         ((char) 'W')
#define STARTARGPREFIX_SAVETYPE_SAVETOEXISTING  ((char) 'X')
#define STARTARGPREFIX_SAVETYPE_SAVETORUIM      ((char) 'R')

//Maximum length of characters that can be used  in predicting
// the contact
#ifdef FEATURE_36_DIGIT_DIAL_STRING
#define MAX_PREDICT_DATA_LEN                    36
#else
#define MAX_PREDICT_DATA_LEN                    32
#endif

//Maximum characters that will be used in the speed dial.
//We only use 0 -99, so 2 digits is enough
#define MAX_SPEED_DIAL_LEN                      2

//index at which the speed dial slot number is available to the user
// 1 to 99 is available for the to allocate
#define SPEED_DIAL_USER_START_INDEX             1

//field lengths for different types of field input
#define FLDLEN_NAME        255   /* 256 - NULL */
#define FLDLEN_NUMBER      40    /* Maximum Number Length - Limit PBM can save*/
#define FLDLEN_EMAIL       255   /* 256 - NULL */
#define FLDLEN_ADDRESS     255   /* 256 - NULL */
#define FLDLEN_URL         255   /* 256 - NULL */
#define FLDLEN_GROUPID     3     /* largest number that can be entered is 255 */

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/
typedef struct _IContactApp IContactApp;

////
// Callback function type, used with IContactApp_SelectField()
//
// selRec  == the selected IAddrRec. Don't forget to use
//            IADDRREC_AddRef() if this pointer will be saved.
// selItem == the selected AEEAddrField from selRec.
//
// Returns TRUE if the contact app should exit
typedef boolean (*PFNSELECTEDCONTACTCB)(IAddrRec *selRec, AEEAddrField *selItem);

//Class definition
QINTERFACE(IContactApp)
{
   INHERIT_IApplet(IContactApp);
   int (*SaveNumber)(IContactApp *p, AECHAR *number,
                     AppletStartMethod   method,
                     PhoneNumberSaveType saveType);
   int (*ViewAll)(IContactApp *p);
   int (*FindNumber)(IContactApp *p, AECHAR *searchStr);
   int (*SelectField)(IContactApp *p, PFNSELECTEDCONTACTCB fnSelectedCallback);
};

//Contact Module required by BREW
typedef struct _ContactMod
{
   DECLARE_VTBL(IModule)

   uint32   m_nRefs;
} ContactMod;


//Contacts info structures. It used to store the names in the contact list,
//adding new contact or when you select a contact.
typedef struct _ContactInfo
{
   uint16      m_id;    // Database ID of this contact
   AECHAR     *m_name;  // Contact name
   AEEAddrCat  m_class; // Contact classification, one of
                        // {AEE_ADDR_CAT_PERSONAL,
                        // AEE_ADDR_CAT_BUSINESS,
                        // AEE_ADDR_CAT_NONE}

   // used in the FindNumber stuff, viewing contacts and speed dial list.
   //In case of findnumber it will hold field id number and its type
   //In case of view all, personal, business it will hold the primary
   //number field id and its type.
   //In case of speed dial list, m_fldNumberId will hold the speed dial
   //index number of the contact
   uint16  m_fldNumberId;   // field id of the found number
   uint16  m_fldNumberType; // stores the number type, one of:
                            //   {NUMBERTYPE_MENUITEM_WORK,
                            //    NUMBERTYPE_MENUITEM_HOME,
                            //    NUMBERTYPE_MENUITEM_PAGER
                            //    NUMBERTYPE_MENUITEM_MOBILE}
   uint16  m_speedDialIndex; // Speed Dial Index (only for speed dial entries)

} ContactInfo;

//Temporary holds the information about the address field.
typedef struct _FieldInfo
{
   // field type, one of {STR_FIELDINPUT_NUMBER, STR_FIELDINPUT_ADDRESS,
   //                     STR_FIELDINPUT_EMAIL, STR_FIELDINPUT_URL}
   uint16 m_type;

   // stores the number type if m_type == STR_FIELDINPUT_NUMBER, one of:
   //   {NUMBERTYPE_MENUITEM_WORK, NUMBERTYPE_MENUITEM_HOME,
   //    NUMBERTYPE_MENUITEM_PAGER, NUMBERTYPE_MENUITEM_MOBILE, NUMBERTYPE_RUIM}
   uint16 m_numberType;

   AECHAR *m_name;  // field data
} FieldInfo;

//struct _CContactApp;

typedef struct _CContactApp
{
   DECLARE_VTBL(IContactApp)

   uint32    m_nRefs;        // Reference count
   IModule  *m_pModule;      // pointer to IModule
   IDisplay *m_pDisplay;     // Display interface
   IShell   *m_pShell;       // Shell interface
   AEERect   m_rc;

   AEEBitmapInfo m_BmpInfo;  // Display bitmap info

   IAddrBook  *m_addr_book;  //IADDRBOOK interface

   IAddrRec   *m_rec;        // Current record

   // IVector interface that holds a list of ContactInfo structures
   IVector    *m_contact_iv;

   // IVector interface that holds a list of ContactInfo structures
   // which represents the speed dial contacts
   IVector    *m_speeddial_iv;

   // IVector interfaces that holds a list of AEEAddrField records
   IVector    *m_fldrecs_iv;

   // True if any of the fields in m_fldrecs_iv contain a voicetag.
   boolean    m_fldrecs_has_vt;

#ifdef FEATURE_PHONE_VR
   // The voicetag if m_fldrecs_has_vt is TRUE
   AEEVRUtterance m_fldrecs_vt;

   // Ask "create voice tag ?" only for "Add to Speed Dial".
   boolean m_isAddVoiceTag;

   // Pointer to the ISTATIC control used by the VoiceTag dialog
   IStatic *m_voiceTag_IStatic;

   // This is set to the Voice Tag after training
   AEEVRUtterance m_voiceTag;

   // IVR interface
   IVR  *m_pVR;

   // True if user aborts VR
   boolean  m_userAbort;

   // the state to return to if a voice tag creation is cancelled
   ContactAppState m_voiceTagSavedState;
#endif

   // True if m_fldrecs_iv belongs to Speed Dial Category
   boolean    m_fldrecs_is_sd;

   // Speed Dial index for the contact loaded in m_fldrecs_iv
   // Also used to store index selected after STATE_SPEEDDIAL_SELSLOT_RESUME
   uint16     m_fldrecs_sdindex;

   // Current applet state
   ContactAppState m_currState;

   // The 'next' applet state
   ContactAppState m_nextState;

   // The state to return to once the active dialog closes
   ContactAppState m_dlgRetState;

   // Result code returned when the active dialog closes
   DlgResultEnum  m_dlgResult;


   // The search string that the user has just entered in
   AECHAR *m_searchStr;

   // pointer to the currently active dialog
   IDialog *m_activeDlg;

   // event handler for the active dialog
   CONT_DIALOG_EVENTHANDLER m_activeDlgHandleEvent;

   // the saved menu selection for DLG_MAIN
   uint16 m_mainDlg_menuSel;

   // the saved menu selection for DLG_CONTACTOPTIONS
   uint16 m_contactOptDlg_menuSel;

   // the saved menu selection for DLG_FIELDOPTIONS
   uint16 m_fieldOptsDlg_menuSel;

   // the saved menu selection for DLG_RUIM
   uint16 m_ruimDlg_menuSel;

   // this is used by the DLG_FIELDINPUT and DLG_CONTACTNAME dialogs
   // to adjust the softkey menu items depending on whether the user is
   // adding or editing.
   boolean m_isAddNew;
   //to specify that a record already exists and we will be manipulating it
   boolean m_isExisting;

   // The resource id that the YesNo dialog box uses for it's title
   // (this should be set before creating a YesNo dialog box)
   uint16 m_yesnoDialogTitle;

   // True if the yesno dialog should timeout after time
   // (returns DLGRET_CANCELED)
   boolean  m_yesnoTimeout;

   // This should be set to one of
   //   {STR_CONTACTLIST_ALLCONTACTS, STR_CONTACTLIST_PERSONAL,
   //    STR_CONTACTLIST_BUSINESS, STR_CONTACTLIST_SEARCH,
   //    STR_CONTACTLIST_SELECTCONTACT, STR_CONTACTLIST_FINDNUMBER}
   // before a DLG_CONTACTLIST is created
   uint16 m_contactListType;

   // This is set by DLG_CONTACTLIST to the ID when the user
   // selects a contact (it is an index into m_contact_iv)
   uint16 m_contactSelId;

   // This is set by Speed Dial list to the ID when the user
   // selects a contact (it is an index into m_speeddial_iv)
   uint16 m_speedDialSelId;

   //Phone number save type. (see enum PhoneNumberSaveType).
   //will be used in case start  method is save number.
   //This indicates whether the number should be added to the new contact
   // or existing contact or RUIM.
   PhoneNumberSaveType m_phoneSaveType;

#if defined(WORKAROUND_IMENUCTL_ADDITEMEX_IS_SLOW)
   // The first index that is currently being displayed
   // in the Contact list menu.  Since IMENUCTL_AddItemEx()
   // is slow, we only load a partial list of Contact names
   uint16 m_contactListMenuStart;

   // The first index that is currently being displayed
   // in the Speed dial list menu.  Since IMENUCTL_AddItemEx()
   // is slow, we only load a partial list of Contact names
   uint16 m_speedDialListMenuStart;

#endif


   // ID of the currently selected field (index into m_fldrecs_iv)
   uint16 m_fieldSelId;

   // Contains information about the field that the user
   // is currently manipulating (adding,editing,deleting,etc)
   FieldInfo m_fieldInfo;

   // Contains information about the contact that the user is
   // current manipulating
   ContactInfo m_contactInfo;

   // Contains information about the speed dial entry currently selected
   ContactInfo *m_speedDialInfo;

   // Pointer to the ISTATIC control used by the MsgBox dialog
   IStatic *m_msgBox_IStatic;

   // String resource to use for the Msgbox text
   uint16 m_msgBox_textId;

   // How did we startup?
   AppletStartMethod m_startMethod;

   // True if EVT_SUSPEND as been received
   boolean m_suspending;

   // True if the Contacts applet is currently running
   boolean m_isActive;

   // flag used in some cases to determine if AVK_CLR should close
   // the current dialog
   boolean m_fieldWasEmpty;

   // height of a line of text in the current font
   int     m_lineHeight;

   // number of lines of text on the screen in the current font, if they
   // shown along with bitmaps
   uint16  m_numLines;

   // number of lines of text on the screen in the current font, if only text
   //is shown i.e with no bitmaps
   uint16  m_numTextOnlyLines;

   // callback function used with 968IContactApp_SelectField()
   PFNSELECTEDCONTACTCB  m_selNumberCallback;

   ////
   // If this is not NULL, the DLG_VIEWADDRESS dialog will
   // use it instead of pMe->m_fieldInfo.m_name
   // (the buffer pointed to by this field will not be FREE()ed
   AECHAR const         *m_viewAddressStr;

   ////
   // Setting this to TRUE causes some slight behaviour changes
   // in the DLG_CONTACT dialog.  It's used when DLG_CONTACT is
   // being used to simply select a contact, and disables the
   // 'Options' menu item and the 'Send key' dialing
   boolean               m_selectingContact;

   //This varibale will be used to keep track of the current field
   //ID of the number type in contact list while viewing contacts.
   //It will used to show the icons rotating
   uint16                m_contactListFieldID;

   // the state to return when clear is pressed on View contact screen
   ContactAppState m_viewContactSavedState;

   // the state to return to when an entry is saved on the Speeddial
   ContactAppState m_speedDialSavedState;

   //will used to save the newly added record ID in the add new state.
   //This id will be later used when showing the view contact screen.
   uint16          m_new_rec_id;

   //will be used in the different states of the contact options to know
   //which state to go back. Now we can have contact options from Addnew
   //menu as well as the contact list. If this flag is set we know that
   //we had choosed one of  the add menu.
   boolean         m_bAddNewMenu;

#ifdef FEATURE_MMGSDI
   ////
   // These fields are used to control the STATE_RUIM_* states
   ////

   // the state to return to when STATE_RUIM_* is canceled
   ContactAppState m_ruimCancelState;

   // the state to return to when an entry is saved on the RUIM
   ContactAppState m_ruimSavedState;

   // TRUE if the 'Assign' softkey should allow the user to select a number
   // from the list of current contacts, FALSE if it should immediately
   // save to the RUIM
   boolean              m_ruimListContacts;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
   // TRUE if the ContactApp is accessing SIM 1 and FALSE if the app is 
   // accessing SIM 2. The default is set to SIM 1

   boolean          m_uimSlot1;
#endif

   // Added for FDN PIN2 or FDN PIN2_SLOT2 validation
   // Needed to create an instance of the ICard interface
#ifdef FEATURE_NEW_ICARD
   ICard            *m_pICard;

   // Used to store PIN2 or PIN2_SLOT2 to later pass to ICARD_VerifyPin
   AEECardPinCode       m_sPIN;
   AEECardPinCmdStatus m_sPinActionStatus;
#else
   ICARD            *m_pICard;
   AEEPINCbData     m_sPin_rsp_data;
   AEEPINCode       m_sPIN;
#endif /*FEATURE_NEW_ICARD */

   AEECallback      m_sCallback;
   ContactAppState  m_pinRetState;
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_CONTACTNAME_PREDICT

   // Keeps track of the keys the user has entered, in an attempt to predict
   // which contact name they're interested in.
   AVKType              m_namePredictData[MAX_PREDICT_DATA_LEN];

   // Number of keystrokes in m_namePredictData[]
   uint16               m_namePredictLen;
   //Whenever you are saving a phone number from phone to RUIM or vice versa
   //set this flag to true so the contact list resume will know if needs to
   //reload all the contacts.
   boolean              m_bContListNeedsReload;

   //keep track of the keys that are entered for the speed dial jump
   AVKType              m_indexSpeedDial[MAX_SPEED_DIAL_LEN];

   //pointer to character. Character denotes the availability of the slot. Will
   //be used to get the confirmation message to overwrite.If the character is -1
   //denotes that it is empty otherwise it denotes the index number of the
   //contact info in m_speeddial_iv;
   int8                *m_nSpeedDialIndexesEmpty;

#endif /* FEATURE_CONTACTNAME_PREDICT */
   int8                 m_RecInitRetVal;

   /* Set to TRUE when doing enumeration of all available records */
   boolean              m_availRecInit;

   /* Set to TRUE when Multitap menu is open. Used to decide whether to
    * do the workaround on up and down keys for switching between text ctrl
    * and softkey */
   boolean              bMultiTap;

   /* Store key press param for use in key release event */
   uint16               wKeyPressParam;

   /* temperory storage to store m_contactInfo.m_name when FDN
      verification is being performed */
   AECHAR *             m_tmpName;
} CContactApp;


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/


/*=============================================================================
FUNCTION: IContactApp_AddRef

DESCRIPTION:

PARAMETERS:
    *p: IContactApp interface pointer

RETURN VALUE:
uint32:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#define IContactApp_AddRef(p)          GET_PVTBL(p,IContactApp)->AddRef(p)


/*=============================================================================
FUNCTION: IContactApp_Release

DESCRIPTION:

PARAMETERS:
    *p: IContactApp interface pointer

RETURN VALUE:
uint32:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#define IContactApp_Release(p)         GET_PVTBL(p,IContactApp)->Release(p)


/*=============================================================================
FUNCTION: IContactApp_HandleEvent

DESCRIPTION:

PARAMETERS:
    *pi: IContactApp interface pointer
    eCode:
    wParam:
    dwParam:

RETURN VALUE:
boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#define IContactApp_HandleEvent(p,ec,wp,dw) \
           GET_PVTBL(p,IContactApp)->HandleEvent(p,ec,wp,dw)


/*=============================================================================
FUNCTION: IContactApp_SaveNumber

DESCRIPTION:
   Use this method to start the save number screen from
   a different applet


PARAMETERS:
    *pi: IContactApp interface pointer
    *number: The number to save (AECHAR)
   *method : method used to start this applet ( see AppletStartMethod)
   *saveType : Phone number save Type ( see PhoneNumberSaveType)
RETURN VALUE:
   int: SUCCESS on success

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#define IContactApp_SaveNumber(p,number,method,saveType) \
           GET_PVTBL(p,IContactApp)->SaveNumber(p,number,method,saveType)


/*=============================================================================
FUNCTION: IContactApp_ViewAll

DESCRIPTION:
   Invoke the 'View All' entry point

PARAMETERS:
    *pi: IContactApp interface pointer

RETURN VALUE:
   int: SUCCESS on success

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#define IContactApp_ViewAll(p)         GET_PVTBL(p,IContactApp)->ViewAll(p)


/*=============================================================================
FUNCTION: IContactApp_FindNumber

DESCRIPTION:
   Invoke the 'FindNumber' entry point

PARAMETERS:
    *pi: IContactApp interface pointer
    *searchStr: The string to search for (AECHAR)

RETURN VALUE:
   int: SUCCESS on success

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#define IContactApp_FindNumber(p,s)    GET_PVTBL(p,IContactApp)->FindNumber(p,s)


/*=============================================================================
FUNCTION: IContactApp_SelectField

DESCRIPTION:
   This entry point can be used to select a field from the contact database.
   When a field is selected, the callback function will be invoked and the
   Contacts applet will exit (if the callback returns TRUE).

PARAMETERS:
    *pi: IContactApp interface pointer
    fnSelectedCallback: callback function

RETURN VALUE:
   int: SUCCESS on success

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#define IContactApp_SelectField(p,cb)    \
           GET_PVTBL(p,IContactApp)->SelectField(p,cb)

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
boolean CContactApp_CallNumber(struct _CContactApp *pMe, AEEAddrField *cf);


#endif
