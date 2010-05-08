/*===========================================================================
  FILE: ContStateMachine.c

  SERVICES: Contact Applet State Machine Handlers

  GENERAL DESCRIPTION:
      This file implements all contact applet dialog handlers which are used
      by the IContactApp to bring up various Contact Applet screens
      to the user

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright © 2003-2007 QUALCOMM Incorporated. All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=============================================================================
  $Header: //depot/asic/msmshared/apps/Contact/ContStateMachine.c#39 $

                    EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------
06/08/07     cvs     Fix issue related to incorrectly initialized callback
05/24/07     cvs     Only sort the contact list when it changes.
11/14/05     jas     Fix lint issues
08/23/05     bw      Fix memory problems when selecting "save to phone" on a
                     UIM entry.
07/18/05     aps     fix to save contact name when FDN pin
                     verification is performed
03/24/05     JAS     One more featurization fix for VR.
03/08/05     SUN     Fixed Compiler Error
02/25/05     jkl     Fixed featurization over VR.
01/18/05     BW      Upgraded to contactapp.brx file from the bri version
01/17/05     BW      Added USIM support - ability to add and view UIM fields
12/13/04     ssc     Changed include file name case to match the Linux names 
                     of the files. Mismatches in the case break AMSS builds 
                     on Linux servers.
12/07/04     JAS     Replacing break;s in if.. with return
12/01/04     JAS     Fixing typo in error message
11/30/04     AT      Removing the ASSERTS from the code.
11/19/04     BW      Fix crash when Add To... is selected and there are no
                     efs entries.
11/10/04     BW      Added support for SDN
10/19/04     JAS     Janitorial work; code cleanup
10/04/04     PA      Update category in Field Options state
08/27/04     NG      Added missing else statement to fix contact app crash.
07/22/04     PA      Added new menu to save existing contact to UIM2
06/23/04     PA      Support for enumeration of available contacts
04/07/04     PA      Check maximum number of fields allowed for a contact and
                     select an alternate number type if required.
03/11/04     PA      More support for SIM related errors
01/26/04     AT      Fixed off by one error on max digit numbers.
01/23/04     tml     Added FEATURE_NEW_ICARD feature
01/21/04     AT      Featurized the ICARD changes from December
                     to be able to build in all targets.
01/19/04     AT      Added New Error messages when UIM not ready or memory
                     is full.
12/22/03     tml     ICARD API Changes
12/17/03     ADM     Added code to check and verify the status of card
                     in slot2 before adding to or viewing numbers in slot2
10/10/03     adm     Replaced ASSERTS in Init code to ifs' to fix crashes
                     when instances of addrbook and vectors already existed.
08/12/03     jds     Added FEATURE_MMGSDI around var to fix compile error
08/07/03     jds     Added support for editing FDN.
06/03/03     PA      Changed FEATURE_RUIM_PHONEBOOK to FEATURE_MMGSDI
05/15/03     PA      Support for creating/retraining voice tags from speed dial
                     list. Support for originating voice calls.
05/09/03     PA      Handle DLGRET_CONT_UNHANDLED in dialog handlers
05/08/03     PA      Support for Voice Tags
02/06/03     kar     Code cleanup
11/15/02     pm      Port Contact App to QCT
=============================================================================*/
// Suppress "ANSI limit of 511 'external identifiers' exceeded"
//lint -save -"esym(793,external identifiers)"

// Suppress "Suspicious cast" and "Unusual pointer cast"
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


#include "contactapp.brh"        // Applet Resource Header
#include "ContApp.h"           // Main Applet Header

#include "AEEStdLib.h"        // AEE StdLib Services
#include "AEE_OEM.h"

/*===========================================================================
                           DEFINITIONS AND CONSTANTS
===========================================================================*/


/*===========================================================================
                               MACRO DEFINITIONS
===========================================================================*/
   #define MOVE_TO_STATE(pMe, toState) {                                 \
                                   (pMe)->m_nextState = (toState); \
                                       }

/*===========================================================================
                              TYPE DECLARATIONs
===========================================================================*/
typedef void (*STATE_CONT_HANDLER)(CContactApp *pMe);


/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/
static uint16 ContStateMachine_SelFieldInputDlg(uint16 type);
static boolean ContStateMachine_CallFieldNumber(CContactApp *pMe);

//Function handler for different states of contact applet

//Just place holder for their corresponding states
static void HdlrStateContNull(CContactApp *pMe);
static void HdlrStateContInit(CContactApp *pMe);
static void HdlrStateContMain(CContactApp *pMe);
static void HdlrStateMainHandle(CContactApp *pMe);
static void HdlrStateFindName(CContactApp *pMe);
static void HdlrStateFindNameHandle(CContactApp *pMe);
static void HdlrStateAddNew(CContactApp *pMe);
static void HdlrStateAddNewHandle(CContactApp *pMe);
static void HdlrStateAddNewNumber(CContactApp *pMe);
static void HdlrStateAddNewNumberHandle(CContactApp *pMe);
static void HdlrStateAddNewAddress(CContactApp *pMe);
static void HdlrStateAddNewAddressHandle(CContactApp *pMe);
static void HdlrStateAddNewContact(CContactApp *pMe);
static void HdlrStateAddNewContactHandle(CContactApp *pMe);
static void HdlrStateAddNewFinished(CContactApp *pMe);

#ifdef FEATURE_MMGSDI
static void HdlrStateAddNewSaveOnRUIM(CContactApp *pMe);
static void HdlrStatePIN2(CContactApp *pMe);
static void HdlrStatePIN2Handle(CContactApp *pMe);
#endif

static void HdlrStateContactList(CContactApp *pMe);
static void HdlrStateContactListReload(CContactApp *pMe);
static void HdlrStateContactListAvailReload(CContactApp *pMe);
static void HdlrStateContactListResume(CContactApp *pMe);
static void HdlrStateContactListError(CContactApp *pMe);
static void HdlrStateContactListHandle(CContactApp *pMe);

static void HdlrStateContact(CContactApp *pMe);
static void HdlrStateContactRefresh(CContactApp *pMe);
static void HdlrStateContactResume(CContactApp *pMe);
static void HdlrStateContactHandle(CContactApp *pMe);

static void HdlrStateContactOpts(CContactApp *pMe);
static void HdlrStateContactOptsResume(CContactApp *pMe);
static void HdlrStateContactOptsHandle(CContactApp *pMe);

static void HdlrStateContactOptsAddNew(CContactApp *pMe);
static void HdlrStateContactOptsAddNewHandle(CContactApp *pMe);
static void HdlrStateContactOptsAddNewNumber(CContactApp *pMe);
static void HdlrStateContactOptsAddNewNumberHandle(CContactApp *pMe);
static void HdlrStateContactOptsAddNewFinishedNumber(CContactApp *pMe);
static void HdlrStateContactOptsAddNewAddress(CContactApp *pMe);
static void HdlrStateContactOptsAddNewAddressHandle(CContactApp *pMe);
static void HdlrStateContactOptsAddNewFinishedAddr(CContactApp *pMe);

#ifdef FEATURE_MMGSDI
static void HdlrStateContactOptsAddNewSaveOnRUIM(CContactApp *pMe);
#endif

static void HdlrStateContactOptsEditName(CContactApp *pMe);
static void HdlrStateContactOptsEditNameHandle(CContactApp *pMe);
static void HdlrStateContactOptsEditClass(CContactApp *pMe);
static void HdlrStateContactOptsEditClassHandle(CContactApp *pMe);
static void HdlrStateContactDelete(CContactApp *pMe);
static void HdlrStateContactDeleteHandle(CContactApp *pMe);

static void HdlrStateFieldOpts(CContactApp *pMe);
static void HdlrStateFieldOptsHandle(CContactApp *pMe);
static void HdlrStateFieldOptsNumberType(CContactApp *pMe);
static void HdlrStateFieldOptsNumberTypeHandle(CContactApp *pMe);
static void HdlrStateFieldOptsEdit(CContactApp *pMe);
static void HdlrStateFieldOptsEditHandle(CContactApp *pMe);
static void HdlrStateFieldOptsViewAddr(CContactApp *pMe);
static void HdlrStateFieldOptsDelete(CContactApp *pMe);
static void HdlrStateFieldOptsDeleteHandle(CContactApp *pMe);

static void HdlrStateSpeedDialSelSlot(CContactApp *pMe);
static void HdlrStateSpeedDialSelSlotResume(CContactApp *pMe);
static void HdlrStateSpeedDialSelSlotHandle(CContactApp *pMe);
static void HdlrStateSpeedDialOverwrite(CContactApp *pMe);
static void HdlrStateSpeedDialOverwriteHandle(CContactApp *pMe);

#ifdef FEATURE_PHONE_VR
static void HdlrStateVoiceTagAdd(CContactApp *pMe);
static void HdlrStateVoiceTagAddHandle(CContactApp *pMe);
static void HdlrStateVoiceTagTrainIntro(CContactApp *pMe);
static void HdlrStateVoiceTagTrain(CContactApp *pMe);
static void HdlrStateVoiceTagTrainHandle(CContactApp *pMe);
static void HdlrStateVoiceTagSave(CContactApp *pMe);
#endif

static void HdlrStateSaveNumber(CContactApp *pMe);
static void HdlrStateSaveNumberNumber(CContactApp *pMe);
static void HdlrStateSaveNumberNumberHandle(CContactApp *pMe);
static void HdlrStateSaveNumberNewOrExisting(CContactApp *pMe);
static void HdlrStateSaveNumberNewContact(CContactApp *pMe);
static void HdlrStateSaveNumberNewContactHandle(CContactApp *pMe);
static void HdlrStateSaveNumberFinishNew(CContactApp *pMe);
static void HdlrStateSaveNumberSelContact(CContactApp *pMe);
static void HdlrStateSaveNumberSelContactHandle(CContactApp *pMe);
static void HdlrStateSaveNumberFinishExisting(CContactApp *pMe);
static void HdlrStateSaveNumberViewContact(CContactApp *pMe);

static void HdlrStateSelectFieldHandle(CContactApp *pMe);

#ifdef FEATURE_MMGSDI
static void HdlrStateSaveNumberFinishSaveOnRUIM(CContactApp *pMe);
static void HdlrStateContRUIM(CContactApp *pMe);
static void HdlrStateRUIMResume(CContactApp *pMe);
static void HdlrStateRUIMHandle(CContactApp *pMe);
static void HdlrStateRUIMSelContactRefresh(CContactApp *pMe);
static void HdlrStateRUIMSelContactResume(CContactApp *pMe);
static void HdlrStateRUIMSelContactHandle(CContactApp *pMe);
static void HdlrStateRUIMContactOpts(CContactApp *pMe);
static void HdlrStateRUIMContactOptsResume(CContactApp *pMe);
static void HdlrStateRUIMContactOptsHandle(CContactApp *pMe);
static void HdlrStateRUIMContactOptsEditName(CContactApp *pMe);
static void HdlrStateRUIMContactOptsEditNameHandle(CContactApp *pMe);
static void HdlrStateRUIMContactOptsAddNew(CContactApp *pMe);
static void HdlrStateRUIMContactOptsAddNewHandle(CContactApp *pMe);
static void HdlrStateRUIMContactOptsAddNewNumber(CContactApp *pMe);
static void HdlrStateRUIMContactOptsAddNewNumberHandle(CContactApp *pMe);
static void HdlrStateRUIMContactOptsAddNewEmail(CContactApp *pMe);
static void HdlrStateRUIMContactOptsAddNewEmailHandle(CContactApp *pMe);
static void HdlrStateRUIMContactOptsAddNewName(CContactApp *pMe);
static void HdlrStateRUIMContactOptsAddNewNameHandle(CContactApp *pMe);
static void HdlrStateRUIMContactOptsAddNewId(CContactApp *pMe);
static void HdlrStateRUIMContactOptsAddNewIdHandle(CContactApp *pMe);
static void HdlrStateRUIMContactOptsAddNewFinishedHandle(CContactApp *pMe);
static void HdlrStateRUIMAddNew(CContactApp *pMe);
static void HdlrStateRUIMAddNewHandle(CContactApp *pMe);
static void HdlrStateRUIMAddNewNumber(CContactApp *pMe);
static void HdlrStateRUIMAddNewNumberHandle(CContactApp *pMe);
static void HdlrStateRUIMAddNewAddNumber(CContactApp *pMe);
static void HdlrStateRUIMAddNewAddNumberHandle(CContactApp *pMe);
static void HdlrStateRUIMAddNewContact(CContactApp *pMe);
static void HdlrStateRUIMAddNewContactHandle(CContactApp *pMe);
static void HdlrStateRUIMFieldOpts(CContactApp *pMe);
static void HdlrStateRUIMFieldOptsHandle(CContactApp *pMe);
static void HdlrStateRUIMFieldOptsEdit(CContactApp *pMe);
static void HdlrStateRUIMFieldOptsEditHandle(CContactApp *pMe);
static void HdlrStateRUIMFieldOptsDelete(CContactApp *pMe);
static void HdlrStateRUIMFieldOptsDeleteHandle(CContactApp *pMe);
static void HdlrStateRUIMDelete(CContactApp *pMe);
static void HdlrStateRUIMDeleteHandle(CContactApp *pMe);
static void HdlrStateRUIMAddNewId(CContactApp *pMe);
static void HdlrStateRUIMAddNewIdHandle(CContactApp *pMe);
static void HdlrStateRUIMAddNewEmail(CContactApp *pMe);
static void HdlrStateRUIMAddNewEmailHandle(CContactApp *pMe);
static void HdlrStateRUIMAddNewName(CContactApp *pMe);
static void HdlrStateRUIMAddNewNameHandle(CContactApp *pMe);

static void HdlrStateRUIMContactList(CContactApp *pMe);
static void HdlrStateRUIMContactListReload(CContactApp *pMe);
static void HdlrStateRUIMContactListResume(CContactApp *pMe);
static void HdlrStateRUIMContactListError(CContactApp *pMe);
static void HdlrStateRUIMContactListHandle(CContactApp *pMe);

static void HdlrStateRUIMContact(CContactApp *pMe);
static void HdlrStateRUIMContactRefresh(CContactApp *pMe);
static void HdlrStateRUIMContactResume(CContactApp *pMe);
static void HdlrStateRUIMContactHandle(CContactApp *pMe);
static void HdlrStateRUIMContactNotANumber(CContactApp *pMe);
static void HdlrStateRUIMAssign(CContactApp *pMe);

#endif

static void HdlrStateExitApplet(CContactApp *pMe);
static void HdlrStateErrorInit(CContactApp *pMe);
static void HdlrStateContErrorDlgOpen(CContactApp *pMe);
static void HdlrStateErrorDatabase(CContactApp *pMe);

static boolean CContactApp_CanSaveNumber(CContactApp  *pMe,
                                         AECHAR const *number);

int ContDlgHandler_Sort_by_Name (ContactInfo *ci_a, ContactInfo *ci_b); 
int ContDlgHandler_Sort_by_Index (ContactInfo *ci_a, ContactInfo *ci_b);

/*===========================================================================
                              GLOBAL DATA
===========================================================================*/

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

// List of state handlers indexed by state ID (enum smsAppState)
// NOTE!  State IDs starts at 0 and must not have gaps between successive IDs
// (ie. 0,1,2,4,5). While adding new states or removing old states, pay extra
// attention and maintain the same order as "enum ContactAppState" in which the
//Contact applet states are defined.

static STATE_CONT_HANDLER gStateHandlers[] =
{
   /* STATE_CONT_NULL */                         HdlrStateContNull,
   /* STATE_CONT_INIT  */                        HdlrStateContInit,
   /* STATE_CONT_MAIN  */                        HdlrStateContMain,
   /* STATE_MAIN_HANDLE */                       HdlrStateMainHandle,
   /* STATE_FINDNAME   */                        HdlrStateFindName,
   /* STATE_FINDNAME_HANDLE */                   HdlrStateFindNameHandle,
   /* STATE_ADDNEW     */                        HdlrStateAddNew,
   /* STATE_ADDNEW_HANDLE */                     HdlrStateAddNewHandle,
   /* STATE_ADDNEW_NUMBER */                     HdlrStateAddNewNumber,
   /* STATE_ADDNEW_NUMBER_HANDLE */              HdlrStateAddNewNumberHandle,
   /* STATE_ADDNEW_ADDRESS */                    HdlrStateAddNewAddress,
   /* STATE_ADDNEW_ADDRESS_HANDLE */             HdlrStateAddNewAddressHandle,
   /* STATE_ADDNEW_CONTACT */                    HdlrStateAddNewContact,
   /* STATE_ADDNEW_CONTACT_HANDLE */             HdlrStateAddNewContactHandle,
   /* STATE_ADDNEW_FINISHED */                   HdlrStateAddNewFinished,

#ifdef FEATURE_MMGSDI
   /* STATE_ADDNEW_SAVEONRUIM */                 HdlrStateAddNewSaveOnRUIM,
   /* STATE_PIN2 */                              HdlrStatePIN2,
   /* STATE_PIN2_HANDLE */                       HdlrStatePIN2Handle,
#endif
   /* STATE_CONTACTLIST */                       HdlrStateContactList,
   /* STATE_CONTACTLIST_RELOAD */                HdlrStateContactListReload,
   /* STATE_CONTACTLIST_AVAIL_RELOAD */          HdlrStateContactListAvailReload,
   /* STATE_CONTACTLIST_RESUME */                HdlrStateContactListResume,
   /* STATE_CONTACTLIST_ERROR  */                HdlrStateContactListError,
   /* STATE_CONTACTLIST_HANDLE */                HdlrStateContactListHandle,

   /* STATE_CONTACT            */                HdlrStateContact,
   /* STATE_CONTACT_REFRESH    */                HdlrStateContactRefresh,
   /* STATE_CONTACT_RESUME     */                HdlrStateContactResume,
   /* STATE_CONTACT_HANDLE     */                HdlrStateContactHandle,

   /* STATE_CONTACTOPTS        */                HdlrStateContactOpts,
   /* STATE_CONTACTOPTS_RESUME */                HdlrStateContactOptsResume,
   /* STATE_CONTACTOPTS_HANDLE */                HdlrStateContactOptsHandle,

   /* STATE_CONTACTOPTS_ADDNEW */                HdlrStateContactOptsAddNew,
   /* STATE_CONTACTOPTS_ADDNEW_HANDLE */      HdlrStateContactOptsAddNewHandle,
   /* STATE_CONTACTOPTS_ADDNEW_NUMBER */      HdlrStateContactOptsAddNewNumber,
   /* STATE_CONTACTOPTS_ADDNEW_NUMBER_HANDLE */
                                     HdlrStateContactOptsAddNewNumberHandle,
   /* STATE_CONTACTOPTS_ADDNEW_FINISHED_NUMBER */
                                     HdlrStateContactOptsAddNewFinishedNumber,
   /* STATE_CONTACTOPTS_ADDNEW_ADDRES */
                                     HdlrStateContactOptsAddNewAddress,
   /* STATE_CONTACTOPTS_ADDNEW_ADDRESS_HANDLE */
                                     HdlrStateContactOptsAddNewAddressHandle,
   /* STATE_CONTACTOPTS_ADDNEW_FINISHED_ADDR */
                                     HdlrStateContactOptsAddNewFinishedAddr,
#ifdef FEATURE_MMGSDI
/* STATE_CONTACTOPTS_ADDNEW_SAVEONRUIM */ HdlrStateContactOptsAddNewSaveOnRUIM,
#endif

   /* STATE_CONTACTOPTS_EDITNAME */  HdlrStateContactOptsEditName,
   /* STATE_CONTACTOPTS_EDITNAME_HANDLE */
                                     HdlrStateContactOptsEditNameHandle,
   /* STATE_CONTACTOPTS_EDITCLASS */ HdlrStateContactOptsEditClass,
   /* STATE_CONTACTOPTS_EDITCLASS_HANDLE */
                                     HdlrStateContactOptsEditClassHandle,
   /* STATE_CONTACTOPTS_DELETE */    HdlrStateContactDelete,
   /* STATE_CONTACTOPTS_DELETE_HANDLE */   HdlrStateContactDeleteHandle,

   /* STATE_FIELDOPTS */                   HdlrStateFieldOpts,
   /* STATE_FIELDOPTS_HANDLE */            HdlrStateFieldOptsHandle,
   /* STATE_FIELDOPTS_NUMBERTYPE */        HdlrStateFieldOptsNumberType,
   /* STATE_FIELDOPTS_NUMBERTYPE_HANDLE */ HdlrStateFieldOptsNumberTypeHandle,
   /* STATE_FIELDOPTS_EDIT */              HdlrStateFieldOptsEdit,
   /* STATE_FIELDOPTS_EDIT_HANDLE */       HdlrStateFieldOptsEditHandle,
   /* STATE_FIELDOPTS_VIEWADDR */          HdlrStateFieldOptsViewAddr,
   /* STATE_FIELDOPTS_DELETE */            HdlrStateFieldOptsDelete,
   /* STATE_FIELDOPTS_DELETE_HANDLE */     HdlrStateFieldOptsDeleteHandle,

   /* STATE_SPEEDDIAL_SELSLOT */           HdlrStateSpeedDialSelSlot,
   /* STATE_SPEEDDIAL_SELSLOT_RESUME */    HdlrStateSpeedDialSelSlotResume,
   /* STATE_SPEEDDIAL_SELSLOTHANLDE */     HdlrStateSpeedDialSelSlotHandle,
   /* STATE_SPEEDDIAL_OVERWRITE */         HdlrStateSpeedDialOverwrite,
   /* STATE_SPEEDDIAL_OVERWRITE_HANDLE */  HdlrStateSpeedDialOverwriteHandle,

#ifdef FEATURE_PHONE_VR
   /* STATE_VOICETAG_ADD */                HdlrStateVoiceTagAdd,
   /* STATE_VOICETAG_ADD_HANDLE */         HdlrStateVoiceTagAddHandle,
   /* STATE_VOICETAG_TRAIN_INTRO */        HdlrStateVoiceTagTrainIntro,
   /* STATE_VOICETAG_TRAIN */              HdlrStateVoiceTagTrain,
   /* STATE_VOICETAG_TRAIN_HANDLE */       HdlrStateVoiceTagTrainHandle,
   /* STATE_VOICETAG_SAVE */               HdlrStateVoiceTagSave,
#endif

   /* STATE_SAVENUMBER */                  HdlrStateSaveNumber,
   /* STATE_SAVENUMBER_NUMBER */           HdlrStateSaveNumberNumber,
   /* STATE_SAVENUMBER_NUMBER_HANDLE */    HdlrStateSaveNumberNumberHandle,

   /* STATE_SAVENUMBER_NEWOREXISTING */    HdlrStateSaveNumberNewOrExisting,
   /* STATE_SAVENUMBER_NEWCONTACT  */      HdlrStateSaveNumberNewContact,
   /* STATE_SAVENUMBER_NEWCONTACT_HANDLE */HdlrStateSaveNumberNewContactHandle,
   /* STATE_SAVENUMBER_FINISH_NEW */       HdlrStateSaveNumberFinishNew,

   /* STATE_SAVENUMBER_SELCONTACT */       HdlrStateSaveNumberSelContact,
   /* STATE_SAVENUMBER_SELCONTACT_HANDLE */HdlrStateSaveNumberSelContactHandle,
   /* STATE_SAVENUMBER_FINISH_EXISTING */  HdlrStateSaveNumberFinishExisting,
   /* STATE_SAVENUMBER_VIEWCONTACT     */  HdlrStateSaveNumberViewContact,
   /* STATE_SELECTFIELD_HANDLE */          HdlrStateSelectFieldHandle,

#ifdef FEATURE_MMGSDI
   /* STATE_SAVENUMBER_FINISH_SAVEONRUIM */HdlrStateSaveNumberFinishSaveOnRUIM,
   /* STATE_CONT_RUIM */                   HdlrStateContRUIM,
   /* STATE_RUIM_RESUME */                 HdlrStateRUIMResume,
   /* STATE_RUIM_HANDLE */                 HdlrStateRUIMHandle,
   /* STATE_RUIM_SELCONTACT_REFRESH */     HdlrStateRUIMSelContactRefresh,
   /* STATE_RUIM_SELCONTACT_RESUME */      HdlrStateRUIMSelContactResume,
   /* STATE_RUIM_SELCONTACT_HANDLE */      HdlrStateRUIMSelContactHandle,
   /* STATE_RUIM_CONTACTOPTS */            HdlrStateRUIMContactOpts,
   /* STATE_RUIM_CONTACTOPTS_RESUME */     HdlrStateRUIMContactOptsResume,
   /* STATE_RUIM_CONTACTOPTS_HANDLE */     HdlrStateRUIMContactOptsHandle,
   /* STATE_RUIM_CONTACTOPTS_EDITNAME */   HdlrStateRUIMContactOptsEditName,
   /* STATE_RUIM_CONTACTOPTS_EDITNAME_HANDLE */
                                           HdlrStateRUIMContactOptsEditNameHandle,
   /* STATE_RUIM_CONTACTOPTS_ADDNEW */     HdlrStateRUIMContactOptsAddNew,
   /* STATE_RUIM_CONTACTOPTS_ADDNEW_HANDLE */
                                           HdlrStateRUIMContactOptsAddNewHandle,
   /* STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER */   
                                           HdlrStateRUIMContactOptsAddNewNumber,
   /* STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER_HANDLE */
                                           HdlrStateRUIMContactOptsAddNewNumberHandle,
   /* STATE_RUIM_CONTACTOPTS_ADDNEW_EMAIL */   
                                           HdlrStateRUIMContactOptsAddNewEmail,
   /* STATE_RUIM_CONTACTOPTS_ADDNEW_EMAIL_HANDLE */
                                           HdlrStateRUIMContactOptsAddNewEmailHandle,
   /* STATE_RUIM_CONTACTOPTS_ADDNEW_NAME */   
                                           HdlrStateRUIMContactOptsAddNewName,
   /* STATE_RUIM_CONTACTOPTS_ADDNEW_NAME_HANDLE */
                                           HdlrStateRUIMContactOptsAddNewNameHandle,
   /* STATE_RUIM_CONTACTOPTS_ADDNEW_ID */   
                                           HdlrStateRUIMContactOptsAddNewId,
   /* STATE_RUIM_CONTACTOPTS_ADDNEW_ID_HANDLE */
                                           HdlrStateRUIMContactOptsAddNewIdHandle,
   /* STATE_RUIM_CONTACTOPTS_ADDNEW_FINISHED_HANDLE */
                                           HdlrStateRUIMContactOptsAddNewFinishedHandle,
   /* STATE_RUIM_FIELDOPTS_DELETE */       HdlrStateRUIMFieldOptsDelete,
   /* STATE_RUIM_FIELDOPTS_DELETE_HANDLE */     
                                           HdlrStateRUIMFieldOptsDeleteHandle,
   /* STATE_RUIM_ADDNEW */                 HdlrStateRUIMAddNew,
   /* STATE_RUIM_ADDNEW_HANDLE */          HdlrStateRUIMAddNewHandle,
   /* STATE_RUIM_ADDNEW_NUMBER */          HdlrStateRUIMAddNewNumber,
   /* STATE_RUIM_ADDNEW_NUMBER_HANDLE */   HdlrStateRUIMAddNewNumberHandle,
   /* STATE_RUIM_ADDNEW_ADDNUMBER */       HdlrStateRUIMAddNewAddNumber,
   /* STATE_RUIM_ADDNEW_ADDNUMBER_HANDLE */   
                                           HdlrStateRUIMAddNewAddNumberHandle,
   /* STATE_RUIM_ADDNEW_CONTACT */         HdlrStateRUIMAddNewContact,
   /* STATE_RUIM_ADDNEW_CONTACT_HANDLE */  HdlrStateRUIMAddNewContactHandle,
   /* STATE_RUIM_ADDNEW_ID */              HdlrStateRUIMAddNewId,
   /* STATE_RUIM_ADDNEW_ID_HANDLE */       HdlrStateRUIMAddNewIdHandle,
   /* STATE_RUIM_ADDNEW_EMAIL */           HdlrStateRUIMAddNewEmail,
   /* STATE_RUIM_ADDNEW_EMAIL_HANDLE */  HdlrStateRUIMAddNewEmailHandle,
   /* STATE_RUIM_ADDNEW_NAME */            HdlrStateRUIMAddNewName,
   /* STATE_RUIM_ADDNEW_NAME_HANDLE */     HdlrStateRUIMAddNewNameHandle,
   /* STATE_RUIM_FIELDOPTS */              HdlrStateRUIMFieldOpts,
   /* STATE_RUIM_FIELDOPTS_HANDLE */       HdlrStateRUIMFieldOptsHandle,
   /* STATE_RUIM_FIELDOPTS_EDIT */         HdlrStateRUIMFieldOptsEdit,
   /* STATE_RUIM_FIELDOPTS_EDIT_HANDLE */  HdlrStateRUIMFieldOptsEditHandle,
   /* STATE_RUIM_DELETE */                 HdlrStateRUIMDelete,
   /* STATE_RUIM_DELETE_HANDLE */          HdlrStateRUIMDeleteHandle,

   /* STATE_RUIM_CONTACTLIST */            HdlrStateRUIMContactList,
   /* STATE_RUIM_CONTACTLIST_RELOAD */     HdlrStateRUIMContactListReload,
   /* STATE_RUIM_CONTACTLIST_RESUME */     HdlrStateRUIMContactListResume,
   /* STATE_RUIM_CONTACTLIST_ERROR */      HdlrStateRUIMContactListError,
   /* STATE_RUIM_CONTACTLIST_HANDLE */     HdlrStateRUIMContactListHandle,

   /* STATE_RUIM_CONTACT */                HdlrStateRUIMContact,
   /* STATE_RUIM_CONTACT_REFRESH */        HdlrStateRUIMContactRefresh,
   /* STATE_RUIM_CONTACT_RESUME */         HdlrStateRUIMContactResume,
   /* STATE_RUIM_CONTACT_HANDLE */         HdlrStateRUIMContactHandle,
   /* STATE_RUIM_CONTACT_NOTANUMBER */     HdlrStateRUIMContactNotANumber,
   /* STATE_RUIM_ASSIGN */                 HdlrStateRUIMAssign,

#endif
   /* STATE_EXITAPPLET */                  HdlrStateExitApplet,
   /* STATE_ERROR_INIT */                  HdlrStateErrorInit,
   /* STATE_CONT_ERROR_DLGOPEN */          HdlrStateContErrorDlgOpen,
   /* STATE_ERROR_DATABASE */              HdlrStateErrorDatabase

};


/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

/*=============================================================================
FUNCTION: ContStateMachine_ProcessState

DESCRIPTION: Process a single FSM state

PARAMETERS:
   *pMe: CContactApp Object pointer

RETURN VALUE:
void:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ContStateMachine_ProcessState(CContactApp *pMe)
{
   if (pMe) {
      // Invoke the state handler depending upon the current state of the
      // Contact applet.
      gStateHandlers[pMe->m_currState](pMe);
   }

} // ContStateMachine_ProcessState()


/*=============================================================================
FUNCTION: HdlrStateContNull

DESCRIPTION:
   Called in response to the STATE_CONT_NULL state being set. This funtion
   does not do anything.It is just a place holder for the corresponding state.
   This function will never be called

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContNull(CContactApp *pMe)
{
   if (!pMe) {
      return;
   }
}

/*=============================================================================
FUNCTION: HdlrStateContInit

DESCRIPTION:
   Called in response to the STATE_CONT_INIT state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContInit(CContactApp *pMe)
{
   //Create all the interfaces here.
   boolean initFailed = FALSE;

   //Create the instance of the Address book
   if (NULL == pMe->m_addr_book)
   if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                        AEECLSID_ADDRBOOK,
                                        (void**)&pMe->m_addr_book)) {
      initFailed = TRUE;
   }

#ifdef FEATURE_MMGSDI
   //Create an instance of ICard
   if (NULL == pMe->m_pICard) {
      if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                           AEECLSID_CARD,
                                           (void**)&pMe->m_pICard)) {
         initFailed = TRUE;
      }
   }

   /* Create Space for the PIN */
   pMe->m_sPIN.code = (AECHAR*)MALLOC((FLDLEN_NUMBER+1)*sizeof(AECHAR));
#endif /* FEATURE_MMGSDI */

   //create the instance of the vector for contacts used in the populating
   //contact list
   if (NULL == pMe->m_contact_iv) {
      if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                           AEECLSID_VECTOR,
                                           (void**)&pMe->m_contact_iv)) {
         initFailed = TRUE;
      }
   }

   if (NULL == pMe->m_speeddial_iv) {
      if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                           AEECLSID_VECTOR,
                                           (void**)&pMe->m_speeddial_iv)) {
         initFailed = TRUE;
      }
   }

   if (NULL == pMe->m_fldrecs_iv) {
      if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                           AEECLSID_VECTOR,
                                           (void**)&pMe->m_fldrecs_iv)) {
         initFailed = TRUE;
      }
   }

   if (TRUE == initFailed) {
      MOVE_TO_STATE(pMe, STATE_ERROR_INIT)
   } else {
      pMe->m_selectingContact = FALSE;
      switch (pMe->m_startMethod) {
         //if the applet started normally by clicking contacts applet
         case STARTMETHOD_NORMAL:
            MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
            break;

         case STARTMETHOD_SAVENUMBER_NUMBER_EDIT:
         case STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT:
            MOVE_TO_STATE(pMe, STATE_SAVENUMBER)
            break;

         case STARTMETHOD_SELECTFIELD:
            pMe->m_selectingContact = TRUE;

         /*lint -fallthrough*/
         case STARTMETHOD_VIEWALL:
            pMe->m_contactListType = STR_CONTACTLIST_ALLCONTACTS;

         /*lint -fallthrough*/
         case STARTMETHOD_FINDNUMBER:
            MOVE_TO_STATE(pMe, STATE_CONTACTLIST)
            break;

         default:
            MSG_ERROR("Invalid start method %d", pMe->m_startMethod, 0, 0);
            MOVE_TO_STATE(pMe, STATE_ERROR_INIT)
            break;
      }
   }
}

/*=============================================================================
FUNCTION: HdlrStateContMain

DESCRIPTION:
   Called in response to the STATE_CONT_MAIN state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContMain(CContactApp *pMe)
{
   //show the main dialog with all the menus here.
   pMe->m_selectingContact = FALSE;
   if (pMe->m_startMethod != STARTMETHOD_NORMAL) {
      MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
   } else {
      pMe->m_bContListNeedsReload= FALSE;
      ContDlgHandler_ShowDialog(pMe,
                                DLG_MAIN,
                                STATE_MAIN_HANDLE);
   }
}


#ifdef FEATURE_MMGSDI
/*=============================================================================
FUNCTION: validate_PIN2

DESCRIPTION:
   Called in response to the an attemp to edit FDN.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void validate_PIN2(CContactApp *pMe)
{
#ifdef FEATURE_NEW_ICARD
   AEECardPinStatus pin2_status;  /* PIN2 status */
#else
   AEEPinStatus pin2_status;  /* PIN2 status */
#endif /*FEATURE_NEW_ICARD */

   int nReturnStatus;

#ifdef FEATURE_MMGSDI_DUAL_SLOT

  // Let's go get the status of PIN2 or PIN2_SLOT2 depending on which SIM
  // the app is communicating with.
  if(pMe->m_uimSlot1)
    nReturnStatus = ICARD_GetPinStatus(pMe->m_pICard,
                                     AEECARD_PIN2,
                                     &pin2_status);
  else
    nReturnStatus = ICARD_GetPinStatus(pMe->m_pICard,
                                       AEECARD_PIN2_SLOT2,
                                       &pin2_status);
#else
  // Since we have only one SIM, we can just go access that one.
  nReturnStatus = ICARD_GetPinStatus(pMe->m_pICard,
                                     AEECARD_PIN2,
                                     &pin2_status);
#endif


  /* Check status */
  if (nReturnStatus != SUCCESS) {
    ContDlgHandler_ShowMsgBox(pMe,
                              STR_PIN2_ERROR_MSG,
                              STATE_PIN2);
    return;
  }

  switch (pin2_status.lock_state) {
    case AEECARD_PIN_DISABLED:
      MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW)
      break;

    case AEECARD_PIN_ENABLED:
    case AEECARD_PIN_VERIFIED:
      MOVE_TO_STATE(pMe, STATE_PIN2)
      break;

    case AEECARD_PIN_UNKNOWN:
      ContDlgHandler_ShowMsgBox(pMe,
                                STR_CANNOT_EDIT_FDN_MSG,
                                STATE_CONT_MAIN);
      break;

    case AEECARD_PIN_PERM_DISABLED:
    case AEECARD_PIN_NOT_VALID:
      ContDlgHandler_ShowMsgBox(pMe,
                                STR_PIN2_SVC_PROVIDER_MSG,
                                STATE_CONT_MAIN);
      break;

    case AEECARD_PIN_BLOCKED:
      ContDlgHandler_ShowMsgBox(pMe,
                                STR_PIN2_BLOCKED_MSG,
                                STATE_CONT_MAIN);
      break;

    default:
      break;

  }
}
#endif /* FEATURE_MMGSDI */


/*=============================================================================
FUNCTION: HdlrStateMainHandle

DESCRIPTION:
   Called in response to the STATE_MAIN_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateMainHandle(CContactApp *pMe)
{
   //Initialise this variable
   pMe->m_bAddNewMenu = FALSE;
#ifdef FEATURE_MMGSDI
   pMe->m_pinRetState = STATE_CONT_NULL;
#endif /* FEATURE_MMGSDI */

   //handler for the return result of the Main Dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_MAIN_FINDNAME:
         MOVE_TO_STATE(pMe, STATE_FINDNAME)
         break;

      case DLGRET_MAIN_ALLCONTACTS:
         pMe->m_contactListType = STR_CONTACTLIST_ALLCONTACTS;
         MOVE_TO_STATE(pMe, STATE_CONTACTLIST)
         break;

      case DLGRET_MAIN_BIZCONTACTS:
         pMe->m_contactListType = STR_CONTACTLIST_BUSINESS;
         MOVE_TO_STATE(pMe, STATE_CONTACTLIST)
         break;

      case DLGRET_MAIN_PERCONTACTS:
         pMe->m_contactListType = STR_CONTACTLIST_PERSONAL;
         MOVE_TO_STATE(pMe, STATE_CONTACTLIST)
         break;

      case DLGRET_MAIN_ADDNEW:
         pMe->m_contactListType = STR_CONTACTLIST_ALLCONTACTS;
         pMe->m_bAddNewMenu = TRUE;
         pMe->m_fieldInfo.m_type = STR_FIELDINPUT_NUMBER;
         MOVE_TO_STATE(pMe, STATE_ADDNEW)
         break;

#ifdef FEATURE_MMGSDI
      case DLGRET_MAIN_ADDTORUIM:
         pMe->m_bAddNewMenu = TRUE;
         pMe->m_contactListType = STR_CONTACTLIST_RUIM_ADN;
         pMe->m_fieldInfo.m_type = STR_FIELDINPUT_NUMBER;
         pMe->m_ruimSavedState = STATE_CONT_MAIN;
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW)
         break;

      case DLGRET_CONT_MAIN_RUIM_ADN:
         pMe->m_contactListType = STR_CONTACTLIST_RUIM_ADN;
         pMe->m_ruimCancelState = STATE_CONT_MAIN;
         pMe->m_ruimSavedState = STATE_CONT_MAIN;
         pMe->m_ruimListContacts = TRUE;
         MOVE_TO_STATE(pMe, STATE_CONT_RUIM)
         break;

      case DLGRET_MAIN_ADDTOFDN:

         pMe->m_bAddNewMenu = TRUE;
         pMe->m_contactListType = STR_CONTACTLIST_RUIM_FDN;
         pMe->m_fieldInfo.m_type = STR_FIELDINPUT_NUMBER;
         pMe->m_ruimSavedState = STATE_CONT_MAIN;
         /* Set state to return to after PIN verification */
         pMe->m_pinRetState = STATE_RUIM_ADDNEW;
         validate_PIN2(pMe);
         break;

      case DLGRET_CONT_MAIN_RUIM_FDN:
         pMe->m_contactListType = STR_CONTACTLIST_RUIM_FDN;
         pMe->m_ruimCancelState = STATE_CONT_MAIN;
         pMe->m_ruimSavedState = STATE_CONT_MAIN;
         pMe->m_ruimListContacts = TRUE;
         MOVE_TO_STATE(pMe, STATE_CONT_RUIM)
         break;
      case DLGRET_CONT_MAIN_RUIM_SDN:
         pMe->m_contactListType = STR_CONTACTLIST_SDN;
         pMe->m_ruimCancelState = STATE_CONT_MAIN;
         pMe->m_ruimSavedState = STATE_CONT_MAIN;
         pMe->m_ruimListContacts = TRUE;
         MOVE_TO_STATE(pMe, STATE_CONT_RUIM)
         break;
#endif /* FEATURE_MMGSDI */

      case DLGRET_MAIN_SPEEDDIAL:
         pMe->m_contactListType = STR_CONTACTLIST_SPEEDDIAL;
         MOVE_TO_STATE(pMe, STATE_CONTACTLIST)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
         break;
   }
}


/*=============================================================================
FUNCTION: HdlrStateFindName

DESCRIPTION:
   Called in response to the STATE_FINDNAME state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateFindName(CContactApp *pMe)
{
   //show the find name dialog here
   ContDlgHandler_ShowDialog(pMe,
                             DLG_FINDNAME,
                             STATE_FINDNAME_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateFindNameHandle

DESCRIPTION:
   Called in response to the STATE_FINDNAME_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateFindNameHandle(CContactApp *pMe)
{
   //handler for the return result of find name dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_searchStr || WSTRLEN(pMe->m_searchStr) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FINDNAME_NOTEXT_MSG,
                                      STATE_FINDNAME);
         } else {
            pMe->m_contactListType = STR_CONTACTLIST_SEARCH;
            MOVE_TO_STATE(pMe, STATE_CONTACTLIST)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_FINDNAME)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}
/*=============================================================================
FUNCTION: HdlrStateAddNew

DESCRIPTION:
   Called in response to the STATE_ADDNEW state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateAddNew(CContactApp *pMe)
{
   //show the add new dialog here
   pMe->m_isAddNew = TRUE;
   ContDlgHandler_ShowDialog(pMe,
                             DLG_ADDNEW,
                             STATE_ADDNEW_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateAddNewHandle

DESCRIPTION:
   Called in response to the STATE_ADDNEW_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateAddNewHandle(CContactApp *pMe)
{
   //handler for the return result of add new dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_OK:
         FREE_STRING(pMe->m_fieldInfo.m_name);

         if (STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_ADDNEW_NUMBER)
         } else {
            MOVE_TO_STATE(pMe, STATE_ADDNEW_ADDRESS)
         }
         break;

      case  DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_ADDNEW)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateAddNewNumber

DESCRIPTION:
   Called in response to the STATE_ADDNEW_NUMBER state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

============================================================================*/
static void HdlrStateAddNewNumber(CContactApp *pMe)
{
   //show the add new number dialog
   ContDlgHandler_ShowDialog(pMe,
                             DLG_N_FIELDINPUT,
                             STATE_ADDNEW_NUMBER_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateAddNewNumberHandle

DESCRIPTION:
   Called in response to the STATE_ADDNEW_NUMBER_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateAddNewNumberHandle(CContactApp *pMe)
{
   //Process the add new number dialog return state
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_ADDNEW)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_ADDNEW_NUMBER);
         } else {
            if (!CContactApp_CanSaveNumber(pMe, pMe->m_fieldInfo.m_name)) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_MSGBOX_DATABASE_ERR_TEXT,
                                         STATE_ADDNEW_NUMBER);
            } else {
               pMe->m_fieldInfo.m_numberType = NUMBERTYPE_MENUITEM_HOME;
               MOVE_TO_STATE(pMe, STATE_ADDNEW_CONTACT)
            }
         }
         break;

      //If you are middle of editing and pressed send key
      case DLGRET_CONTACT_SENDKEY:
         {
            boolean   ret= FALSE;

            if (NULL == pMe->m_fieldInfo.m_name ||
                WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_FIELDINPUT_NOTEXT_MSG,
                                         STATE_FIELDOPTS_EDIT);
            } else {
               //call the number in the text box
               ret=ContStateMachine_CallFieldNumber(pMe);
               if (FALSE == ret) {
                  MOVE_TO_STATE(pMe, STATE_ADDNEW_NUMBER)
               } else {
                  MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
               }
            }
            break;
         }

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_ADDNEW_NUMBER)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateAddNewAddress

DESCRIPTION:
   Called in response to the STATE_ADDNEW_ADDRESS state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateAddNewAddress(CContactApp *pMe)
{
   //show the add new address dialog
   ContDlgHandler_ShowDialog(pMe,
                     ContStateMachine_SelFieldInputDlg(pMe->m_fieldInfo.m_type),
                     STATE_ADDNEW_ADDRESS_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateAddNewAddressHandle

DESCRIPTION:
   Called in response to the STATE_ADDNEW_ADDRESS_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateAddNewAddressHandle(CContactApp *pMe)
{
   //Process the add new address dialog result
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_ADDNEW)
         break;
      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
            WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {

            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_ADDNEW_ADDRESS);

         } else {
            FREE_STRING(pMe->m_contactInfo.m_name);
            MOVE_TO_STATE(pMe, STATE_ADDNEW_CONTACT)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_ADDNEW_ADDRESS)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateAddNewContact

DESCRIPTION:
   Called in response to the STATE_ADDNEW_CONTACT state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateAddNewContact(CContactApp *pMe)
{
   //if contact name has some stale string delete it since
   //we are adding new contact
   if(pMe->m_contactInfo.m_name != NULL){
      FREE_STRING(pMe->m_contactInfo.m_name);
      pMe->m_contactInfo.m_name=NULL;
   }
   //show the add new contact dialog here
   ContDlgHandler_ShowDialog(pMe,
                             DLG_NEWCONTACTNAME,
                             STATE_ADDNEW_CONTACT_HANDLE);
}
/*=============================================================================
FUNCTION: HdlrStateAddNewContactHandle

DESCRIPTION:
   Called in response to the STATE_ADDNEW_CONTACT_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateAddNewContactHandle(CContactApp *pMe)
{
   //handle the add new contact dialog result here
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         if (STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_ADDNEW_NUMBER)
         } else {
            MOVE_TO_STATE(pMe, STATE_ADDNEW_ADDRESS)
         }
         break;
      case DLGRET_CONT_OK:
         if (NULL == pMe->m_contactInfo.m_name ||
            WSTRLEN(pMe->m_contactInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_CONTACTNAME_NOTEXT_MSG,
                                      STATE_ADDNEW_CONTACT);
         } else {
            pMe->m_contactInfo.m_class = AEE_ADDR_CAT_NONE;
            MOVE_TO_STATE(pMe, STATE_ADDNEW_FINISHED)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_ADDNEW_CONTACT)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;
   }
}

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_NEW_ICARD
/*===========================================================================
FUNCTION ContPINVerify_cb

DESCRIPTION

  ContPINVerify_cb is called in response to ICARD_VerifyPin completing, and
  will check the status of pin status(either success or fail).

  Fail:    MOVE_TO_STATE(pMe, STATE_PIN2)
  Success: MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW)

  return TRUE;


DEPENDENCIES
  None.


RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
void  ContPINVerify_cb( void *pNotifyData)
{
  AEECardPinStatus pin2_status;  /* PIN2 status */

  CContactApp * pMe = (CContactApp *) pNotifyData;

  if (NULL == pMe) { 
     ERR("NULL pointer", 0, 0, 0);
     return;
  }

  /* Did the PIN entered by the user match the PIN on the ID card? */
  if (pMe->m_sPinActionStatus.nCmdStatus == SUCCESS) {
    ContDlgHandler_ShowMsgBox(pMe, STR_PIN_VERIFIED_MSG, pMe->m_pinRetState);
  } else { /* verify pin fails */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if(!(pMe->m_uimSlot1))
      (void) ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN2_SLOT2, &pin2_status);
    else
#endif
    (void) ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN2, &pin2_status);

    if (pin2_status.lock_state == AEECARD_PIN_BLOCKED)
      ContDlgHandler_ShowMsgBox(pMe, STR_PIN2_BLOCKED_MSG, STATE_CONT_MAIN);
    else
      ContDlgHandler_ShowMsgBox(pMe, STR_PINS_DONT_MATCH, STATE_PIN2);
  }
  return;
}
#else
/*===========================================================================
FUNCTION ContPINVerify_cb

DESCRIPTION

  ContPINVerify_cb is called in response to ICARD_VerifyPin completing, and
  will check the status of pin status(either success or fail).

  Fail:    MOVE_TO_STATE(STATE_PIN2)
  Success: MOVE_TO_STATE(STATE_RUIM_ADDNEW)

  return TRUE;


DEPENDENCIES
  None.


RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
void  ContPINVerify_cb(AEEPINCbData *pin_cb_rsp)
{
  AEEPinStatus pin2_status;  /* PIN2 status */

  CContactApp * pMe = (CContactApp *) (pin_cb_rsp->user_data);

  if (NULL == pMe) { 
     ERR("NULL pointer", 0, 0, 0);
     break;
  }

  /* Did the PIN entered by the user match the PIN on the ID card? */
  if (pin_cb_rsp->pin_cmd_status == SUCCESS) {
    ContDlgHandler_ShowMsgBox(pMe, STR_PIN_VERIFIED_MSG, pMe->m_pinRetState);
  } else { /* verify pin fails */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if(!(pMe->m_uimSlot1))
      (void) ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN2_SLOT2, &pin2_status);
    else
#endif
      (void) ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN2, &pin2_status);

    if (pin2_status.lock_state == AEECARD_PIN_BLOCKED)
      ContDlgHandler_ShowMsgBox(pMe, STR_PIN2_BLOCKED_MSG, STATE_CONT_MAIN);
    else
      ContDlgHandler_ShowMsgBox(pMe, STR_PINS_DONT_MATCH, STATE_PIN2);
  }
  return;
}
#endif /*FEATURE_NEW_ICARD */

/*=============================================================================
FUNCTION: HdlrStatePIN2

DESCRIPTION:
   Called in response to the STATE_PIN2 state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStatePIN2(CContactApp *pMe)
{
   /* Always start PIN2 verification by clearing the input string */
   if(pMe->m_contactInfo.m_name != NULL){
      FREE_STRING(pMe->m_contactInfo.m_name);
      pMe->m_contactInfo.m_name=NULL;
   }
   /* Show the PIN2 dialog here */
   ContDlgHandler_ShowDialog(pMe,
                             DLG_PIN2_REQUEST,
                             STATE_PIN2_HANDLE);
}


/*=============================================================================
FUNCTION: HdlrStatePIN2Handle

DESCRIPTION:
   Called in response to the STATE_PIN2_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStatePIN2Handle(CContactApp *pMe)
{
  int nReturnStatus;

  //handle the PIN2 dialog result here
  switch (pMe->m_dlgResult) {
     case DLGRET_CONT_CANCELED:
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;

     case DLGRET_CONT_OK:   /* The user selected "NEXT" */
        /* We don't want to interfere with any other data entry screens
         * so let's copy the PIN information here and reset m_name */
        (void)WSTRCPY(pMe->m_sPIN.code, pMe->m_contactInfo.m_name);
        pMe->m_sPIN.code_len = WSTRLEN(pMe->m_sPIN.code);
        pMe->m_contactInfo.m_name[0] = (AECHAR)'\0';

        /* Okay, The user pressed SELECT and there *is* data in the
         * m_sPIN.code string so now we need to call ICARD_VerifyPin
         * and wait for the cb. */

        /* Populate the m_sCallback structure */
#ifdef FEATURE_NEW_ICARD
        CALLBACK_Init(&pMe->m_sCallback, (PFNNOTIFY) ContPINVerify_cb,pMe);
#else
        pMe->m_sPin_rsp_data.user_data = (void*)pMe;
        CALLBACK_Init(&pMe->m_sCallback, (PFNNOTIFY) ContPINVerify_cb,
                  (void*)&(pMe->m_sPin_rsp_data));
#endif /*FEATURE_NEW_ICARD */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
        if(!(pMe->m_uimSlot1))
#ifdef FEATURE_NEW_ICARD
          nReturnStatus = ICARD_VerifyPin(pMe->m_pICard,      // ICARD obj
                                          AEECARD_PIN2_SLOT2, // Which PIN
                                          &pMe->m_sPIN,       // String
                                          &pMe->m_sPinActionStatus,
                              &pMe->m_sCallback); // cb function
#else
          nReturnStatus = ICARD_VerifyPin(pMe->m_pICard,      // ICARD obj
                                          AEECARD_PIN2_SLOT2, // Which PIN
                                          &pMe->m_sPIN,       // String
                                                              &pMe->m_sCallback); // cb function
#endif /*FEATURE_NEW_ICARD */
        else
#endif
#ifdef FEATURE_NEW_ICARD
        nReturnStatus = ICARD_VerifyPin(pMe->m_pICard,      // ICARD obj
                                        AEECARD_PIN2,       // Which PIN
                                        &pMe->m_sPIN,       // String
                                        &pMe->m_sPinActionStatus,
                                        &pMe->m_sCallback); // cb function
#else
          nReturnStatus = ICARD_VerifyPin(pMe->m_pICard,      // ICARD obj
                                          AEECARD_PIN2,       // Which PIN
                                          &pMe->m_sPIN,       // String
                                          &pMe->m_sCallback); // cb function
#endif /*FEATURE_NEW_ICARD */

        if (nReturnStatus != SUCCESS) {
          /* Display Error Message, then return to Contact App Menu */
          ContDlgHandler_ShowMsgBox(pMe, STR_PIN2_ERROR_MSG, STATE_CONT_MAIN);
        }

        /* Note, we *aren't* setting the MOVE_TO_STATE here, because the
         * ContPINVerify_cb will set the state based on Success or Fail */
        break;

     case DLGRET_CONT_UNHANDLED: // Restart the dialog
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;

     default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;
  }
}
#endif /* FEATURE_MMGSDI */


/*=============================================================================
FUNCTION: HdlrStateAddNewFinished

DESCRIPTION:
   Called in response to the STATE_ADDNEW_FINISHED state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/


static void HdlrStateAddNewFinished(CContactApp *pMe)
{
   ContactInfo *ci = NULL;
   //Initialise this to default value. After we add the record this will
   // changed in the create contact function
   pMe->m_new_rec_id = 0xffff;
   //Create the contact with all the information collected in the
   //previous dialogs
   if (TRUE == ContDlgHandler_CreateContact(pMe)) {
      //Free the contact info vector and add newly added contact
      //to the vector. Now the Vector will have only one contact.
      //View Contact state uses this vector to show the contact.
      ContDlgHandler_FreeContactList(pMe,
                                     pMe->m_contact_iv);
      ci = MALLOC(sizeof(*ci));
      if((ci!=NULL) && (pMe->m_new_rec_id!= 0xffff)) {
         ci->m_id = pMe->m_new_rec_id;
         ci->m_class = AEE_ADDR_CAT_NONE;

         ci->m_name = WSTRDUP(pMe->m_contactInfo.m_name);

         if((ci->m_name != NULL)&&
            (SUCCESS == IVector_AddElement(pMe->m_contact_iv, ci))) {

            //set the contact select to the first one, anyways we have only one
            pMe->m_contactSelId = 0;

            //set the return state from view contact to main menu
            pMe->m_viewContactSavedState= STATE_CONT_MAIN;

            MOVE_TO_STATE(pMe, STATE_CONTACT);
         } else{
            MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
         }
      } else{
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
      }
   } else {
      MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
   }

}

#ifdef FEATURE_MMGSDI
/*=============================================================================
FUNCTION: HdlrStateAddNewSaveOnRUIM

DESCRIPTION:
   Called in response to the STATE_ADDNEW_SAVEON_RUIM state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateAddNewSaveOnRUIM(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateAddNewSaveOnRUIM: NULL pMe", 0, 0, 0);
      return;
   }
   //Here we save the the contact on RUIM.
   pMe->m_ruimCancelState = STATE_CONT_MAIN;
   pMe->m_ruimSavedState = STATE_CONT_MAIN;
   pMe->m_ruimListContacts = FALSE;

   if (NULL == pMe->m_contactInfo.m_name ||
       WSTRLEN(pMe->m_contactInfo.m_name) <= 0) {
      //Show the message and go back to Contact Main screen
      ContDlgHandler_ShowMsgBox(pMe,
                                STR_CONTACTNAME_NOTEXT_MSG,
                                STATE_CONT_MAIN);
   } else {
      if (SUCCESS == ContDlgHandler_RUIM_Set(pMe,
                                             pMe->m_contactInfo.m_name,
                                             pMe->m_fieldInfo.m_name)) {
         ContDlgHandler_ShowMsgBox(pMe,
                                   STR_MSGBOX_SAVEDONRUIM_TEXT,
                                   STATE_ADDNEW);
      } else {
         MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
      }
   }

}
#endif

/*=============================================================================
FUNCTION: HdlrStateContactList

DESCRIPTION:
   Called in response to the STATE_CONTACTLIST state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactList(CContactApp *pMe)
{
   // clear the currently selected contact id
   pMe->m_contactSelId = 0;
   MOVE_TO_STATE(pMe, STATE_CONTACTLIST_RELOAD)

}

/*=============================================================================
FUNCTION: HdlrStateContactListReload

DESCRIPTION:
   Called in response to the STATE_CONTACTLIST_RELOAD state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateContactListReload(CContactApp *pMe)
{
   //reload all the contacts from the database.
   ShowBusyIcon(pMe->m_pShell,
                pMe->m_pDisplay,
                &pMe->m_rc,
                FALSE);

   if (TRUE == ContDlgHandler_LoadContactList(pMe)) {
      pMe->m_bContListNeedsReload = FALSE;
      if (IVector_Size(pMe->m_contact_iv) == 0) {
         // No records found -- return to main
         ContDlgHandler_ShowMsgBox(pMe,
                                   STR_MSGBOX_NOCONTACTS_TEXT,
                                   STATE_CONT_MAIN);

      } else {
         MOVE_TO_STATE(pMe, STATE_CONTACTLIST_RESUME)
      }
   } else {
      MOVE_TO_STATE(pMe, STATE_CONTACTLIST_ERROR)
   }
}

/*=============================================================================
FUNCTION: HdlrStateContactListAvailReload

DESCRIPTION:
   Called in response to the STATE_CONTACTLIST_AVAIL_RELOAD state being set.
   Similar to STATE_CONTACTLIST_RELOAD except for the messages. The actual
   error message has already been displayed before getting here.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateContactListAvailReload(CContactApp *pMe)
{
   /* reload all the contacts from the database. */
   ShowBusyIcon(pMe->m_pShell,
                pMe->m_pDisplay,
                &pMe->m_rc,
                FALSE);

   /* Try loading available contacts */
   if (TRUE == ContDlgHandler_LoadAvailContactList(pMe)) {
      pMe->m_bContListNeedsReload = FALSE;
      if (IVector_Size(pMe->m_contact_iv) == 0) {
         /* No records found -- return to main */
         ContDlgHandler_ShowMsgBox(pMe,
                                   STR_MSGBOX_NOAVAILCONTACTS_TEXT,
                                   STATE_CONT_MAIN);

      } else {
         ContDlgHandler_ShowMsgBox(pMe,
                                   STR_MSGBOX_AVAILCONTACTS_TEXT,
                                   STATE_CONTACTLIST_RESUME);
      }
   } else {
      MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
   }
}

/*=============================================================================
FUNCTION: HdlrStateContactResume

DESCRIPTION:
   Called in response to the STATE_CONTACTLIST_RESUME state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateContactListResume(CContactApp *pMe)
{
   //if the list needs to be reloaded, the go retrieve
   //all the records to that it will be refreshed.
   if(pMe->m_bContListNeedsReload == TRUE){
      MOVE_TO_STATE(pMe, STATE_CONTACTLIST_RELOAD)
   }else{
      //show the contact list dialog withe all the contacts
      //retrieved from database
      ContDlgHandler_ShowDialog(pMe,
                                DLG_CONTACTLIST,
                                STATE_CONTACTLIST_HANDLE);
   }

}

/*=============================================================================
FUNCTION: HdlrStateContactListError

DESCRIPTION:
   Called in response to the STATE_CONTACTLIST_ERROR state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactListError(CContactApp *pMe)
{
   switch (pMe->m_RecInitRetVal) {
     case EITEMBUSY:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_UIMNOTREADY_ERR_TEXT,
                                 STATE_CONTACTLIST_AVAIL_RELOAD);
       break;
     case ENOSUCH:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_NOSIMFILE_ERR_TEXT,
                                 STATE_CONTACTLIST_AVAIL_RELOAD);
       break;
     case EBADSTATE:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_RESTRICTED_ERR_TEXT,
                                 STATE_CONTACTLIST_AVAIL_RELOAD);
       break;
     case ENEEDMORE:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_NEEDMORE_ERR_TEXT,
                                 STATE_CONTACTLIST_AVAIL_RELOAD);
       break;
     default:
        // NOTE:  This error condition is treated a little differently
       //        than most of the other errors.  Suppose that
       //        ContDlgHandler_LoadContactList() failed to load contact 100
       //        due to lack of memory, we want the user to be
       //        able to manipulate contacts 0..99 but still notify them
       //        that something isn't quite right.
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_LOADCONTACTLIST_ERR_TEXT,
                                 STATE_CONTACTLIST_RESUME);
     break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateContactListHandle

DESCRIPTION:
   Called in response to the STATE_CONTACTLIST_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactListHandle(CContactApp *pMe)
{
   //handler for the return state of contact list dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_OK:
         pMe->m_viewContactSavedState = STATE_CONTACTLIST_RESUME;
      // Note: pMe->m_contactSelId contains the selected contact
         MOVE_TO_STATE(pMe, STATE_CONTACT)
         break;

      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_CONTACTLIST_RESUME)
         break;

      case DLGRET_CONTACTLIST_SENDKEY:
         {
            ContactInfo *ci;
            AEEAddrField *cf;
            boolean resume = TRUE;

            ci = IVector_ElementAt(pMe->m_contact_iv,
                                   pMe->m_contactSelId);
            if (ci != NULL) {
               if (TRUE == ContDlgHandler_LoadAEEAddrFields(pMe)) {
                  //if we are in the find number list
                  if (STR_CONTACTLIST_FINDNUMBER == pMe->m_contactListType) {
                     cf = (AEEAddrField*)IVector_ElementAt(pMe->m_fldrecs_iv,
                                                           ci->m_fldNumberId);
                  } else {
                     cf =(AEEAddrField*)IVector_ElementAt(pMe->m_fldrecs_iv,
                                                     pMe->m_contactListFieldID);
                  }
                  if (TRUE == CContactApp_CallNumber(pMe, cf)) {
                     resume = FALSE;
                  }
               }
            }

            if (TRUE == resume) {
               MOVE_TO_STATE(pMe, STATE_CONTACTLIST_RESUME)
            } else {
               MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
            }
            break;
         }

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
        break;
   }
}

/*=============================================================================
FUNCTION: HdlrStateContact

DESCRIPTION:
   Called in response to the STATE_CONTACT state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContact(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      ERR("Invalid pMe variables", 0, 0, 0);
      return;
   }
   pMe->m_fieldSelId = OPTIONS_MENU_ID; // Select 'Options' by default
   if (STR_CONTACTLIST_FINDNUMBER == pMe->m_contactListType) {
      ContactInfo *ci;

      ci = (ContactInfo*)IVector_ElementAt(pMe->m_contact_iv,
                                           pMe->m_contactSelId);
      if (ci != NULL)
        pMe->m_fieldSelId = ci->m_fldNumberId;
      else
      {
        ERR("Could not find vector", 0, 0, 0);
        pMe->m_fieldSelId = OPTIONS_MENU_ID; // Select 'Options' by default
      }
   }

   MOVE_TO_STATE(pMe, STATE_CONTACT_REFRESH)

}

/*=============================================================================
FUNCTION: HdlrStateContactRefresh

DESCRIPTION:
   Called in response to the STATE_CONTACT_REFRESH state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactRefresh(CContactApp *pMe)
{
   //Refreshes all the fields from the database
   // for the selectd contact
   (void) ContDlgHandler_LoadAEEAddrFields(pMe);

   if (pMe->m_fieldSelId >= IVector_Size(pMe->m_fldrecs_iv)) {
      pMe->m_fieldSelId = OPTIONS_MENU_ID;
   }
   MOVE_TO_STATE(pMe, STATE_CONTACT_RESUME)

}

/*=============================================================================
FUNCTION: HdlrStateContactResume

DESCRIPTION:
   Called in response to the STATE_CONTACT_RESUME state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactResume(CContactApp *pMe)
{
   //show the contact dialog here
   ContDlgHandler_ShowDialog(pMe,
                             DLG_CONTACT,
                             STATE_CONTACT_HANDLE);

}
/*=============================================================================
FUNCTION: HdlrStateContactHandle

DESCRIPTION:
   Called in response to the STATE_CONTACT_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactHandle(CContactApp *pMe)
{
   //handler for the Contact dialog return state
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, pMe->m_viewContactSavedState)
         break;

      case DLGRET_CONTACT_OPTS:
         pMe->m_contactOptDlg_menuSel= CONTACTOPTIONS_MENUITEM_ADDNEW;
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS)
         break;

      case DLGRET_CONTACT_FIELDSEL:
         if (pMe->m_startMethod != STARTMETHOD_SELECTFIELD) {
            pMe->m_fieldOptsDlg_menuSel = FIELDOPTIONS_MENUITEM_NUMBERTYPE;
            MOVE_TO_STATE(pMe, STATE_FIELDOPTS)
         } else {
            MOVE_TO_STATE(pMe, STATE_SELECTFIELD_HANDLE)
         }
         break;

      case DLGRET_CONTACT_SENDKEY:
         {
            AEEAddrField *cf;
            boolean   ret;

            cf = (AEEAddrField*)IVector_ElementAt(pMe->m_fldrecs_iv,
                                                  pMe->m_fieldSelId);

            ret = CContactApp_CallNumber(pMe, cf);

            if (FALSE == ret) {
               MOVE_TO_STATE(pMe, STATE_CONTACT_RESUME)
            } else {
               MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
            }
            break;
         }

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_CONTACT_RESUME)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateContactOpts

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOpts(CContactApp *pMe)
{
  ContactInfo *ci;

  if (NULL == pMe) { 
    ERR("NULL pointer", 0, 0, 0);
    return;
  }

  if (NULL == (ci = IVector_ElementAt(pMe->m_contact_iv, pMe->m_contactSelId)))
  {
    ERR("NULL pointer for element %d", pMe->m_contactSelId, 0, 0);
    return;
  }
  pMe->m_contactInfo.m_id = ci->m_id;
  pMe->m_contactInfo.m_class = ci->m_class;

  FREE_STRING(pMe->m_contactInfo.m_name);
  pMe->m_contactInfo.m_name = WSTRDUP(ci->m_name);

  MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_RESUME)

}
/*=============================================================================
FUNCTION: HdlrStateContactOptsResume

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_RESUME state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsResume(CContactApp *pMe)
{
   //show the contact options dialog
   ContDlgHandler_ShowDialog(pMe,
                             DLG_CONTACTOPTIONS,
                             STATE_CONTACTOPTS_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateContactOptsHandle

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsHandle(CContactApp *pMe)
{
#ifdef FEATURE_MMGSDI
   ContactInfo *ci;
#endif //FEATURE_MMGSDI

   //handle the contact options dialog return state
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONTACT_RESUME)
         break;

      case DLGRET_CONTACTOPTS_ADDNEW:
#ifdef FEATURE_MMGSDI

         //if we have a view all then we get into this state
         // even when we are dealing with UIM entries

         ci = IVector_ElementAt(pMe->m_contact_iv,
                                pMe->m_contactSelId);
         if (ci==NULL)
         {
           MSG_ERROR("Invalid Contact", 0, 0, 0);
           MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
           return;
         }
         if ((ci->m_class == AEE_ADDR_CAT_SIM_ADN)||
             (ci->m_class == AEE_ADDR_CAT_SIM_ADN2)) {
           MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW)
         } else {
           pMe->m_fieldInfo.m_type = STR_FIELDINPUT_NUMBER;
           MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW)
         }
#else
           pMe->m_fieldInfo.m_type = STR_FIELDINPUT_NUMBER;
           MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW)
#endif  //FEATURE_MMGSDI
         break;

      case DLGRET_CONTACTOPTS_EDITNAME:
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_EDITNAME)
         break;

      case DLGRET_CONTACTOPTS_EDITCLASS:
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_EDITCLASS)
         break;

      case DLGRET_CONTACTOPTS_DELETE:
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_DELETE)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_RESUME)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateContactOptsAddNew

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_ADDNEW state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsAddNew(CContactApp *pMe)
{
   //show the add new option dialog of contact.
   //Allow you to enter new fields for the selectd contact
   pMe->m_isAddNew = TRUE;
   ContDlgHandler_ShowDialog(pMe,
                             DLG_ADDNEW,
                             STATE_CONTACTOPTS_ADDNEW_HANDLE);

}

static void HdlrStateContactOptsAddNewHandle(CContactApp *pMe)
{
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_OK:
         FREE_STRING(pMe->m_fieldInfo.m_name);

         if (STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW_NUMBER)
         } else {
            MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW_ADDRESS)
         }
         break;

      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateContactOptsAddNewNumber

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_ADDNEW_NUMBER state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsAddNewNumber(CContactApp *pMe)
{
   //show the add new  number dialog.
   //Here we can take the phone number input to store
   ContDlgHandler_ShowDialog(pMe,
                             DLG_N_FIELDINPUT,
                             STATE_CONTACTOPTS_ADDNEW_NUMBER_HANDLE);
}

/*=============================================================================
FUNCTION: HdlrStateContactOptsAddNewNumberHandle

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_ADDNEW_NUMBER_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsAddNewNumberHandle(CContactApp *pMe)
{

   //handles the return result of the contact options- add new number dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_CONTACTOPTS_ADDNEW_NUMBER);
         } else {
            if (!CContactApp_CanSaveNumber(pMe, pMe->m_fieldInfo.m_name)) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_MSGBOX_DATABASE_ERR_TEXT,
                                         STATE_CONTACTOPTS_ADDNEW_NUMBER);
            } else {
               pMe->m_fieldInfo.m_numberType = NUMBERTYPE_MENUITEM_HOME;
               MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW_FINISHED_NUMBER)
            }
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW_NUMBER)
         break;

      //If you are middle of editing and pressed send key
      case DLGRET_CONTACT_SENDKEY:
         {
            boolean   ret= FALSE;

            if (NULL == pMe->m_fieldInfo.m_name ||
                WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_FIELDINPUT_NOTEXT_MSG,
                                         STATE_FIELDOPTS_EDIT);
            } else {
               ret=ContStateMachine_CallFieldNumber(pMe);
               if (FALSE == ret) {
                  MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW_NUMBER)
               } else {
                  MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
               }
            }
            break;
         }

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateContactOptsAddNewFinishedNumber

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_ADDNEW_FINISHED_NUMBER state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsAddNewFinishedNumber(CContactApp *pMe)
{
   int retval;

   //We save the field in the database.
   retval = ContDlgHandler_AddField(pMe);
   switch (retval) {
      case AEE_SUCCESS:
         MOVE_TO_STATE(pMe, STATE_CONTACT);
         break;
      case ENOMEMORY:
         ContDlgHandler_ShowMsgBox(pMe,
                             STR_MSGBOX_LIMIT_REACHED_ERR_TEXT,
                             STATE_CONTACT);
         break;
      default:
         MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateContactOptsAddNewAddress

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_ADDNEW_ADDRESS state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsAddNewAddress(CContactApp *pMe)
{
   //show the contact options add new address dialog
   ContDlgHandler_ShowDialog(pMe,
                     ContStateMachine_SelFieldInputDlg(pMe->m_fieldInfo.m_type),
                     STATE_CONTACTOPTS_ADDNEW_ADDRESS_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateContactOptsAddNewAddressHandle

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_ADDNEW_ADDRERESS_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateContactOptsAddNewAddressHandle(CContactApp *pMe)
{
   //handler for the contacat options - add new address dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_CONTACTOPTS_ADDNEW_ADDRESS);
         } else {
            MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW_FINISHED_ADDR)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_ADDNEW_ADDRESS)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateContactOptsAddNewFinishedNumberAddr

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_ADDNEW_FINISHED_ADDR state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsAddNewFinishedAddr(CContactApp *pMe)
{
   int retval;

   //save the address field in the address book
   retval = ContDlgHandler_AddField(pMe);

   switch (retval) {
      case AEE_SUCCESS:
         ContDlgHandler_ShowMsgBox(pMe,
                                STR_MSGBOX_ADDRESSSAVED_TEXT,
                                STATE_CONTACT);
         break;
      case ENOMEMORY:
         ContDlgHandler_ShowMsgBox(pMe,
                             STR_MSGBOX_LIMIT_REACHED_ERR_TEXT,
                             STATE_CONTACT);
         break;
      default:
         MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
         break;
   }

}

#ifdef FEATURE_MMGSDI
/*=============================================================================
FUNCTION: HdlrStateContactOptsAddNewSaveOnRUIM

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_ADDNEW_SAVEON_RUIM state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsAddNewSaveOnRUIM(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateContactOptsAddNewSaveOnRUIM: NULL pMe", 0, 0, 0);
      return;
   }
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_YES:
         pMe->m_ruimCancelState = STATE_CONTACT;
         pMe->m_ruimSavedState = STATE_CONTACT;
         pMe->m_ruimListContacts = FALSE;

         MOVE_TO_STATE(pMe, STATE_CONT_RUIM)
         break;

      case DLGRET_CONT_CANCELED:
      case DLGRET_CONT_NO:
      default:
         MOVE_TO_STATE(pMe, STATE_CONTACT)
         break;
   }

}
#endif


/*=============================================================================
FUNCTION: HdlrStateContactOptsEditName

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_EDITNAME state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateContactOptsEditName(CContactApp *pMe)
{
   //show the edit name dialog
   pMe->m_isAddNew = FALSE;
   ContDlgHandler_ShowDialog(pMe,
                             DLG_CONTACTNAME,
                             STATE_CONTACTOPTS_EDITNAME_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateContactOptsEditNameHande

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_EDITNAME_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsEditNameHandle(CContactApp *pMe)
{
   //handler for the return result of edit name dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_contactInfo.m_name ||
             WSTRLEN(pMe->m_contactInfo.m_name) <= 0) {
            //go back to  the edit name screen if the name is blank
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_CONTACTNAME_NOTEXT_MSG,
                                      STATE_CONTACTOPTS_EDITNAME);
         } else {
            if (TRUE == ContDlgHandler_SaveContactInfo(pMe)) {
               ContactAppState stateToReturn = STATE_CONTACTLIST_RELOAD;

               if((pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NUMBER_EDIT)||
                 (pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT)){
                  stateToReturn = STATE_EXITAPPLET;

               } else if (pMe->m_bAddNewMenu ==TRUE){
                 stateToReturn = STATE_CONT_MAIN;
               }

               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_MSGBOX_CONTACTSAVED_TEXT,
                                         stateToReturn);
            } else {
               MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
            }
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_EDITNAME)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateContactOptsEditClass

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_EDITCLASS state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsEditClass(CContactApp *pMe)
{
   //show the edit class dialog
   ContDlgHandler_ShowDialog(pMe,
                             DLG_CONTACTCLASS,
                             STATE_CONTACTOPTS_EDITCLASS_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateContactOptsEditClasHandle

DESCRIPTION:
   Called in response to the STATE_CONTACTOPTS_EDITCLASS_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactOptsEditClassHandle(CContactApp *pMe)
{
   //handler for the return result of edit class dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS)
         break;

      case DLGRET_CONT_OK:
         if (TRUE == ContDlgHandler_SaveContactInfo(pMe)) {
            //Find the state to return to after showing the message
            ContactAppState stateToReturn = STATE_CONTACTLIST;

            if((pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NUMBER_EDIT)||
              (pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT)){
               stateToReturn = STATE_EXITAPPLET;
            }else if (pMe->m_bAddNewMenu ==TRUE){
               stateToReturn = STATE_CONT_MAIN;
            }

            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_MSGBOX_CONTACTSAVED_TEXT,
                                      stateToReturn);
         } else {
            MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_EDITCLASS)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }
}

/*=============================================================================
FUNCTION: HdlrStateContactDelete

DESCRIPTION:
   Called in response to the STATE_CONTACT_DELETE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactDelete(CContactApp *pMe)
{
   //show the contact delete dialog.
   //We take the confirmation in this dialog before actuallly deleting it from
   //the address book
   pMe->m_yesnoTimeout = FALSE;
   pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_DELETECONTACT;
   ContDlgHandler_ShowDialog(pMe,
                             DLG_YESNO,
                             STATE_CONTACTOPTS_DELETE_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateContactDeleteHandle

DESCRIPTION:
   Called in response to the STATE_CONTACT_DELETE_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContactDeleteHandle(CContactApp *pMe)
{
   //handler for the return result of contact delete dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_YES:
         //Delete the contact
         if (TRUE == ContDlgHandler_DeleteContact(pMe)) {
            ContactAppState stateToReturn = STATE_CONTACTLIST;

            if((pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NUMBER_EDIT)||
              (pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT)){
               stateToReturn = STATE_EXITAPPLET;
            }else if (pMe->m_bAddNewMenu ==TRUE){
               stateToReturn = STATE_CONT_MAIN;
            }

            //show the message box and return to that state
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_MSGBOX_CONTACTDELETED_TEXT,
                                      stateToReturn);
         } else {
            MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
         }
         break;

      case DLGRET_CONT_CANCELED:
      case DLGRET_CONT_NO:
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateFieldOpts

DESCRIPTION:
   Called in response to the STATE_FIELDOPTS state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateFieldOpts(CContactApp *pMe)
{
   // Set pMe->m_fieldInfo.m_type, pMe->m_fieldInfo.m_numberType and
   // mMe->fieldInput to the correct values for the selected field
   FieldInfo *fi;
   AEEAddrField  *cf;
   ContactInfo *ci;

   if (!pMe) {
      MSG_ERROR("HdlrStateFieldOpts: NULL pMe", 0, 0, 0);
      return;
   }

   ci = IVector_ElementAt(pMe->m_contact_iv,
                                       pMe->m_contactSelId);

   if (ci) {
      pMe->m_contactInfo.m_id = ci->m_id;
      pMe->m_contactInfo.m_class = ci->m_class;

      FREE_STRING(pMe->m_contactInfo.m_name);
      pMe->m_contactInfo.m_name = WSTRDUP(ci->m_name);
   } else {
      MSG_ERROR("Contact not found", 0, 0, 0);
      MOVE_TO_STATE(pMe, STATE_ERROR_INIT)
      return;
   }

   cf = (AEEAddrField*)IVector_ElementAt(pMe->m_fldrecs_iv,
                                         pMe->m_fieldSelId);
   if (cf == NULL)
   {
      MSG_ERROR("HdlrStateFieldOpts: NULL cf", 0, 0, 0);
      return;
   }

   fi = ContDlgHandler_AEEAddrField2FieldInfo(pMe, cf);
   if (fi == NULL)
   {
      MSG_ERROR("HdlrStateFieldOpts: NULL fi", 0, 0, 0);
      return;
   }

   FREE_FIELDINFO(pMe->m_fieldInfo);
   pMe->m_fieldInfo = *fi;
   FREE(fi);

   ContDlgHandler_ShowDialog(pMe,
                             DLG_FIELDOPTIONS,
                             STATE_FIELDOPTS_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateFieldOptsHandle

DESCRIPTION:
   Called in response to the STATE_FIELDOPTS_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateFieldOptsHandle(CContactApp *pMe)
{
   //handler for the return result of field options dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONTACT_RESUME)
         break;

      case DLGRET_FIELDOPTS_VIEWADDR:
         MOVE_TO_STATE(pMe, STATE_FIELDOPTS_VIEWADDR)
         break;

      case DLGRET_FIELDOPTS_NUMBERTYPE:
         MOVE_TO_STATE(pMe, STATE_FIELDOPTS_NUMBERTYPE)
         break;

      case DLGRET_FIELDOPTS_EDIT:
         MOVE_TO_STATE(pMe, STATE_FIELDOPTS_EDIT)
         break;

      case DLGRET_FIELDOPTS_ADDTOSPEEDDIAL:
         pMe->m_speedDialSavedState = STATE_FIELDOPTS;
#ifdef FEATURE_PHONE_VR
         pMe->m_isAddVoiceTag = FALSE;
#endif
         MOVE_TO_STATE(pMe, STATE_SPEEDDIAL_SELSLOT)
         break;

#ifdef FEATURE_PHONE_VR
      case DLGRET_FIELDOPTS_ADDVOICETAG:
      case DLGRET_FIELDOPTS_RETRAIN:
         pMe->m_isAddVoiceTag = TRUE;
         if (STR_CONTACTLIST_SPEEDDIAL == pMe->m_contactListType) {
            pMe->m_speedDialSavedState = STATE_CONTACTLIST_RELOAD;
            pMe->m_voiceTagSavedState = STATE_CONTACT_RESUME;
            pMe->m_speedDialInfo = IVector_ElementAt(pMe->m_contact_iv,
                                        pMe->m_contactSelId);
            MOVE_TO_STATE(pMe, STATE_VOICETAG_TRAIN_INTRO)
         } else {
            pMe->m_speedDialSavedState = STATE_FIELDOPTS;
            pMe->m_voiceTagSavedState = STATE_SPEEDDIAL_SELSLOT_RESUME;
            MOVE_TO_STATE(pMe, STATE_SPEEDDIAL_SELSLOT)
         }
         break;
#endif

#ifdef FEATURE_MMGSDI
      case DLGRET_FIELDOPTS_RUIM:
         pMe->m_ruimCancelState = STATE_FIELDOPTS;
         pMe->m_ruimSavedState = STATE_FIELDOPTS;
         pMe->m_ruimListContacts = FALSE;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
         pMe->m_uimSlot1 = TRUE;
#endif

         MOVE_TO_STATE(pMe, STATE_CONT_RUIM)
         break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case DLGRET_FIELDOPTS_RUIM2:
         pMe->m_ruimCancelState = STATE_FIELDOPTS;
         pMe->m_ruimSavedState = STATE_FIELDOPTS;
         pMe->m_ruimListContacts = FALSE;
         pMe->m_uimSlot1 = FALSE;

         MOVE_TO_STATE(pMe, STATE_CONT_RUIM)
         break;
#endif

      case DLGRET_FIELDOPTS_PHONE:
         {
            ContactInfo *ci;
            boolean      ret;
            uint16       wOldCat;
            uint16       wOldNumberType;

            ci = IVector_ElementAt(pMe->m_contact_iv,
                                   pMe->m_contactSelId);
            if ((ci == NULL) ||
                (STR_FIELDINPUT_NUMBER != pMe->m_fieldInfo.m_type) ||
                (pMe->m_fieldInfo.m_name == NULL))
            {
              ERR("Invalid item found %x %x %x", ci, pMe->m_fieldInfo.m_type,
                  pMe->m_fieldInfo.m_name);
              return;
            }

            //work around. temporary modify the category and numbertype before
            //creating the record and restore when we are done with it
            wOldCat = pMe->m_contactInfo.m_class;
            pMe->m_contactInfo.m_class    = AEE_ADDR_CAT_NONE;
            wOldNumberType = pMe->m_fieldInfo.m_numberType;
            pMe->m_fieldInfo.m_numberType = NUMBERTYPE_MENUITEM_HOME;
            //create the contact
            ret= ContDlgHandler_CreateContact(pMe);
            pMe->m_contactInfo.m_class    = wOldCat;
            pMe->m_fieldInfo.m_numberType = wOldNumberType;

            if (TRUE ==ret) {
               pMe->m_bContListNeedsReload = TRUE;
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_MSGBOX_CONTACTADDED_TEXT,
                                         STATE_FIELDOPTS);

            } else {
               MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
            }
            break;
         }
#endif

      case DLGRET_FIELDOPTS_DELETE:
         MOVE_TO_STATE(pMe, STATE_FIELDOPTS_DELETE)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_FIELDOPTS)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateFieldOptsNumberType

DESCRIPTION:
   Called in response to the STATE_FIELDOPTS_NUMBERTYPE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateFieldOptsNumberType(CContactApp *pMe)
{
   //show the field option - number type dialog
   ContDlgHandler_ShowDialog(pMe,
                             DLG_NUMBERTYPE,
                             STATE_FIELDOPTS_NUMBERTYPE_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateFieldOptsNumberTypeHandle

DESCRIPTION:
   Called in response to the STATE_FIELDOPTS_NUMBERTYPE_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateFieldOptsNumberTypeHandle(CContactApp *pMe)
{
   //handler for the return result of field options - Number type dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_OK:
         if (TRUE == ContDlgHandler_SaveField(pMe)) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_MSGBOX_NUMBERTYPESAVED_TEXT,
                                      STATE_CONTACT_REFRESH);
         } else {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_MSGBOX_LIMIT_REACHED_ERR_TEXT,
                                      STATE_CONTACT_REFRESH);
         }
         break;

      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_FIELDOPTS)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_FIELDOPTS_NUMBERTYPE)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }
}

/*=============================================================================
FUNCTION: HdlrStateFieldOptsEdit

DESCRIPTION:
   Called in response to the STATE_FIELDOPTS_EDIT state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateFieldOptsEdit(CContactApp *pMe)
{
   //show edit dialog depending on the field
   pMe->m_isAddNew = FALSE;
   ContDlgHandler_ShowDialog(pMe,
                     ContStateMachine_SelFieldInputDlg(pMe->m_fieldInfo.m_type),
                     STATE_FIELDOPTS_EDIT_HANDLE);
}

/*=============================================================================
FUNCTION: HdlrStateFieldOptsEditHandle

DESCRIPTION:
   Called in response to the STATE_FIELDOPTS_EDIT_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateFieldOptsEditHandle(CContactApp *pMe)
{
   //handler for return result of  Field options - Edit dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_FIELDOPTS_EDIT);
         } else {
            if ( (STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) &&
                 (!CContactApp_CanSaveNumber(pMe, pMe->m_fieldInfo.m_name)) ) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_MSGBOX_DATABASE_ERR_TEXT,
                                         STATE_FIELDOPTS_EDIT);
            } else {
               if (TRUE == ContDlgHandler_SaveField(pMe)) {
                  if (STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) {
                     ContDlgHandler_ShowMsgBox(pMe,
                                               STR_MSGBOX_NUMBERSAVED_TEXT,
                                               STATE_CONTACT_REFRESH);
                  } else {
                     ContDlgHandler_ShowMsgBox(pMe,
                                               STR_MSGBOX_ADDRESSSAVED_TEXT,
                                               STATE_CONTACT_REFRESH);
                  }
               } else {
                  MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
               }
            }
         }
         break;

      //If you are middle of editing and pressed send key
      case DLGRET_CONTACT_SENDKEY:
         {
            boolean   ret= FALSE;

            if (NULL == pMe->m_fieldInfo.m_name ||
                WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_FIELDINPUT_NOTEXT_MSG,
                                         STATE_FIELDOPTS_EDIT);
            } else {
               //call the number in the text box
               ret = ContStateMachine_CallFieldNumber(pMe);
               if (FALSE == ret) {
                  MOVE_TO_STATE(pMe, STATE_FIELDOPTS_EDIT)
               } else {
                  MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
               }
            }
            break;
         }

      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_FIELDOPTS)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_FIELDOPTS_EDIT)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateFieldOptsViewAddr

DESCRIPTION:
   Called in response to the STATE_FIELDOPTS_VIEWADDR state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateFieldOptsViewAddr(CContactApp *pMe)
{
   //show the view address dialog.
   pMe->m_viewAddressStr = NULL;
   ContDlgHandler_ShowDialog(pMe,
                             DLG_VIEWADDRESS,
                             STATE_FIELDOPTS);

}

/*=============================================================================
FUNCTION: HdlrStateFieldOptsDelete

DESCRIPTION:
   Called in response to the STATE_FIELDOPTS_DELETE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateFieldOptsDelete(CContactApp *pMe)
{
   //show the field options delete dialog.
   pMe->m_yesnoTimeout = FALSE;
   if (STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) {
      pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_DELETENUMBER;
   } else {
      pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_DELETEADDRESS;
   }

   ContDlgHandler_ShowDialog(pMe,
                             DLG_YESNO,
                             STATE_FIELDOPTS_DELETE_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateFieldOptsDeleteHandle

DESCRIPTION:
   Called in response to the STATE_FIELDOPTS_DELETE_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateFieldOptsDeleteHandle(CContactApp *pMe)
{
   //handler for the return result of field options - delete dialog
   if (DLGRET_CONT_YES == pMe->m_dlgResult) {
      if (TRUE == ContDlgHandler_DeleteField(pMe)) {

         if (STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_MSGBOX_NUMBERDELETED_TEXT,
                                      STATE_CONTACT);
         } else {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_MSGBOX_ADDRESSDELETED_TEXT,
                                      STATE_CONTACT);
         }
      } else {
         MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
      }

   } else {
      MOVE_TO_STATE(pMe, STATE_FIELDOPTS)
   }

}
/*=============================================================================
FUNCTION: HdlrStateSpeedDialSelSlot

DESCRIPTION:
   Called in response to the STATE_SPEEDDIAL_SELSLOT state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSpeedDialSelSlot(CContactApp *pMe)
{
   //show the busy icon
   ShowBusyIcon(pMe->m_pShell,
                pMe->m_pDisplay,
                &pMe->m_rc,
                FALSE);

   //Load the contacts list from the database
   (void)ContDlgHandler_LoadSpeedDialList(pMe);
   MOVE_TO_STATE(pMe, STATE_SPEEDDIAL_SELSLOT_RESUME)

}
/*=============================================================================
FUNCTION: HdlrStateSpeedDialSelSlotResume

DESCRIPTION:
   Called in response to the STATE_SPEEDDIAL_SELSLOT_RESUME state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSpeedDialSelSlotResume(CContactApp *pMe)
{
   ContDlgHandler_ShowDialog(pMe,
                             DLG_SPEEDDIAL,
                             STATE_SPEEDDIAL_SELSLOT_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateSpeedDialSelSlotHandle

DESCRIPTION:
   Called in response to the STATE_SPEEDDIAL_SELSLOT_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSpeedDialSelSlotHandle(CContactApp *pMe)
{
   if (NULL == pMe) { 
     ERR("NULL pointer", 0, 0, 0);
     return;
   }
   //process return result of save number - save on dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, pMe->m_speedDialSavedState)
         break;

      case DLGRET_CONT_OK:
         {
#ifndef FEATURE_PHONE_VR
            ContactInfo *ciContactList = NULL;
#endif

            if (pMe->m_nSpeedDialIndexesEmpty == NULL)
            {
              ERR ("Invalid SpeedDialIndexesEmpty variable", 0, 0, 0);
              MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
              return;
            }

            //if this slot is empty then just overwrite it
            //else you will have to get the confirmation
            if( pMe->m_nSpeedDialIndexesEmpty[pMe->m_speedDialSelId] ==
                SPEED_DIAL_EMPTY){
#ifdef FEATURE_PHONE_VR
               pMe->m_fldrecs_sdindex =
                    pMe->m_speedDialSelId + SPEED_DIAL_USER_START_INDEX;
               pMe->m_speedDialInfo = NULL;

               if (pMe->m_isAddVoiceTag) {
                  MOVE_TO_STATE(pMe, STATE_VOICETAG_TRAIN_INTRO);
               } else {
                  MOVE_TO_STATE(pMe, STATE_VOICETAG_ADD);
               }
#else

               uint16 nSlot;

               ciContactList = IVector_ElementAt(pMe->m_contact_iv,
                                                 pMe->m_contactSelId);
               if ((ciContactList == NULL) ||
                   (pMe->m_fieldInfo.m_name == NULL) ||
                   (pMe->m_speedDialSelId >= MAX_SPEED_DIAL_NUMS))
               {
                 ERR ("Invalid contact information %x %x %x", ciContactList,
                      pMe->m_fieldInfo.m_name, pMe->m_speedDialSelId);
                 MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
                 return;
               }
                 

               nSlot=pMe->m_speedDialSelId+SPEED_DIAL_USER_START_INDEX;

                  //Add the number in the speed dial
               if (SUCCESS == ContDlgHandler_SpeedDial_Set(pMe,
                                                       ciContactList->m_name,
                                                       pMe->m_fieldInfo.m_name,
                                                       nSlot,
                                                       FALSE)){
                  ContDlgHandler_ShowMsgBox(pMe,
                                            STR_MSGBOX_CONTACTADDED_TEXT,
                                            pMe->m_speedDialSavedState);

               } else {
                  MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
               }
#endif /* FEATURE_PHONE_VR */
            }else{
               MOVE_TO_STATE(pMe, STATE_SPEEDDIAL_OVERWRITE)
            }
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, pMe->m_speedDialSavedState)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}
/*=============================================================================
FUNCTION: HdlrStateSpeedDialOverwrite

DESCRIPTION:
   Called in response to the STATE_SPEEDDIAL_OVERWRITE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSpeedDialOverwrite(CContactApp *pMe)
{
   pMe->m_yesnoTimeout = FALSE;
   pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_OVERWRITERUIM;
   ContDlgHandler_ShowDialog(pMe,
                             DLG_YESNO,
                             STATE_SPEEDDIAL_OVERWRITE_HANDLE);
}

/*=============================================================================
FUNCTION: HdlrStateSpeedDialOverwriteHandle

DESCRIPTION:
   Called in response to the STATE_SPEEDDIAL_OVERWRITE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSpeedDialOverwriteHandle(CContactApp *pMe)
{
   switch (pMe->m_dlgResult) {
   case DLGRET_CONT_YES:
      {
         int8         wIndex=0;
         wIndex = pMe->m_nSpeedDialIndexesEmpty[pMe->m_speedDialSelId];

        //check if it is empty
         if (wIndex == SPEED_DIAL_EMPTY)
         {
           MSG_ERROR("wIndex invalid", 0, 0, 0);
           MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
           return;
         }

#ifdef FEATURE_PHONE_VR
         pMe->m_fldrecs_sdindex =
                    pMe->m_speedDialSelId + SPEED_DIAL_USER_START_INDEX;
         pMe->m_speedDialInfo = IVector_ElementAt(pMe->m_speeddial_iv,
                                          (uint32)wIndex);/*lint !e571*/
         if ((pMe->m_speedDialInfo == NULL) ||
             (pMe->m_speedDialSelId >= MAX_SPEED_DIAL_NUMS))
         {
           MSG_ERROR("Data out of bounds %x %x", pMe->m_speedDialInfo,
                     pMe->m_speedDialSelId, 0);
           MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
           return;
         }

         if (pMe->m_isAddVoiceTag) {
            MOVE_TO_STATE(pMe, STATE_VOICETAG_TRAIN_INTRO);
         } else {
            MOVE_TO_STATE(pMe, STATE_VOICETAG_ADD);
         }
#else
         {
           uint16       nSlot;
           ContactInfo *ciContactList = NULL;
           IAddrRec    *pAddrRec = NULL;
           boolean      bContactAdded = FALSE;

           if(wIndex>=0){
              pMe->m_speedDialInfo = IVector_ElementAt(pMe->m_speeddial_iv,
                                              (uint32)wIndex);/*lint !e571*/
           }

           if (pMe->m_speedDialInfo == NULL)
           {
             MSG_ERROR("Unexpected NULL pointer", 0, 0, 0);
             MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
             return;
           }
           ciContactList = IVector_ElementAt(pMe->m_contact_iv,
                                             pMe->m_contactSelId);

           if ((ciContactList == NULL) ||
               (pMe->m_fieldInfo.m_name == NULL) ||
               (pMe->m_speedDialSelId >= MAX_SPEED_DIAL_NUMS))
           {
             MSG_ERROR("Unexpected NULL pointer", 0, 0, 0);
             MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
             return;
           }

           //remove the old record from database
           pAddrRec = IADDRBOOK_GetRecByID(pMe->m_addr_book,
                                           pMe->m_speedDialInfo->m_id);
           if(pAddrRec !=NULL){
              if (IADDRREC_RemoveRec(pAddrRec)==AEE_SUCCESS){
                 //add the new record in the database
                 nSlot=pMe->m_speedDialSelId+SPEED_DIAL_USER_START_INDEX;
                 if (SUCCESS == ContDlgHandler_SpeedDial_Set(pMe,
                                                        ciContactList->m_name,
                                                        pMe->m_fieldInfo.m_name,
                                                        nSlot,
                                                        FALSE)){
                    ContDlgHandler_ShowMsgBox(pMe,
                                              STR_MSGBOX_CONTACTADDED_TEXT,
                                              pMe->m_speedDialSavedState);
                    bContactAdded = TRUE;

                 }
              }
           }

           if(bContactAdded == FALSE) {
              MOVE_TO_STATE(pMe, pMe->m_speedDialSavedState)
           }
         }
#endif /* FEATURE_PHONE_VR */
      }
      break;

   case DLGRET_CONT_CANCELED:
   case DLGRET_CONT_NO:
   case DLGRET_CONT_UNHANDLED:
      MOVE_TO_STATE(pMe, STATE_SPEEDDIAL_SELSLOT_RESUME);
      break;

   default:
      MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
      MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
      break;
   }
}
#ifdef FEATURE_PHONE_VR
/*=============================================================================
FUNCTION: HdlrStateVoiceTagAdd

DESCRIPTION:
   Called in response to the STATE_VOICETAG_ADD state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateVoiceTagAdd(CContactApp *pMe)
{
   pMe->m_yesnoTimeout = FALSE;
   pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_VOICETAG_ADD;
   ContDlgHandler_ShowDialog(pMe,
                             DLG_YESNO,
                             STATE_VOICETAG_ADD_HANDLE);
}

/*=============================================================================
FUNCTION: HdlrStateVoiceTagAddHandle

DESCRIPTION:
   Called in response to the STATE_VOICETAG_ADD_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateVoiceTagAddHandle(CContactApp *pMe)
{

  switch (pMe->m_dlgResult) {
    case DLGRET_CONT_YES:
      MOVE_TO_STATE(pMe, STATE_VOICETAG_TRAIN_INTRO);
      break;

    case DLGRET_CONT_CANCELED:
    case DLGRET_CONT_UNHANDLED:
      MOVE_TO_STATE(pMe, STATE_SPEEDDIAL_SELSLOT_RESUME)
      break;

    case DLGRET_CONT_NO:
    {
      uint16       nSlot;
      ContactInfo *ciSpeedDial = NULL;
      ContactInfo *ciContactList = NULL;
      IAddrRec    *pAddrRec = NULL;
      int8         wIndex=0;

      wIndex = pMe->m_nSpeedDialIndexesEmpty[pMe->m_speedDialSelId];

      ciContactList = IVector_ElementAt(pMe->m_contact_iv,
                                         pMe->m_contactSelId);

      if ((ciContactList == NULL) ||
          (pMe->m_fieldInfo.m_name == NULL) ||
          (pMe->m_speedDialSelId >= MAX_SPEED_DIAL_NUMS))
      {
        MSG_ERROR("Invalid Contact Info %x %x %x", ciContactList,
                  pMe->m_fieldInfo.m_name, pMe->m_speedDialSelId);
        MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
        break;
      }

      if (wIndex != SPEED_DIAL_EMPTY) {
        ciSpeedDial = IVector_ElementAt(pMe->m_speeddial_iv,
                                           (uint32)wIndex);/*lint !e571*/
        if (ciSpeedDial == NULL)
        {
          MSG_ERROR("Invalid Contact Info %x %x %x", ciSpeedDial, 0, 0);
          MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
          break;
        }

        //remove the old record from database
        pAddrRec = IADDRBOOK_GetRecByID(pMe->m_addr_book,
                                         ciSpeedDial->m_id);
        if (pAddrRec !=NULL){
          (void)ContDlgHandler_RemoveRec_VoiceTag(pMe, pAddrRec);
          if (IADDRREC_RemoveRec(pAddrRec) != AEE_SUCCESS) {
            MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
            break;
          }
        }
      }

      /* Set Speed Dial Entry */
      nSlot=pMe->m_speedDialSelId+SPEED_DIAL_USER_START_INDEX;
      if (SUCCESS == ContDlgHandler_SpeedDial_Set(pMe,
                                              ciContactList->m_name,
                                              pMe->m_fieldInfo.m_name,
                                              nSlot,
                                              FALSE)){
        ContDlgHandler_ShowMsgBox(pMe,
                                     STR_MSGBOX_CONTACTADDED_TEXT,
                                     pMe->m_speedDialSavedState);
      } else {
        MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
      }
      break;
    }

    default:
      MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
      MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
      break;
  }
}

/*=============================================================================
FUNCTION: HdlrStateVoiceTagTrainIntro

DESCRIPTION:
   Called in response to the STATE_VOICETAG_TRAIN_INTRO state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateVoiceTagTrainIntro(CContactApp *pMe)
{
  ContDlgHandler_ShowMsgBox(pMe,
                            STR_VOICETAG_TRAIN_INTRO,
                            STATE_VOICETAG_TRAIN);
}

/*=============================================================================
FUNCTION: HdlrStateVoiceTagTrain

DESCRIPTION:
   Called in response to the STATE_VOICETAG_TRAIN state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateVoiceTagTrain(CContactApp *pMe)
{
   ContDlgHandler_ShowDialog(pMe,
                             DLG_VOICETAG,
                             STATE_VOICETAG_TRAIN_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateVoiceTagTrainHandle

DESCRIPTION:
   Called in response to the STATE_VOICETAG_TRAIN_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateVoiceTagTrainHandle(CContactApp *pMe)
{
  switch (pMe->m_dlgResult) {
    case DLGRET_VR_DONE:
      (void)IVR_RegisterNotify(pMe->m_pVR, NULL, pMe);
      (void)IVR_PlayUtterance(pMe->m_pVR,
                              AEEVRUTTERANCE_ID_TRAINING_WAS_SUCCESSFUL);
      ContDlgHandler_ShowMsgBox(pMe,
                                STR_VOICETAG_SUCCESS,
                                STATE_VOICETAG_SAVE);

      break;

    case DLGRET_VR_FAILED:
      {
        OEMVRSound SayFailed[] = {
                                  { AEEVRUTTERANCE_ID_CANCELLED, NULL },
                                  { AEEVRUTTERANCE_NONE, NULL } };
        (void)OEMVR_PlaySounds(pMe->m_pVR, SayFailed);
        ContDlgHandler_ShowMsgBox(pMe,
                             STR_VOICETAG_FAILED,
                             STATE_CONT_MAIN);
      }
      break;

    case DLGRET_VR_ABORTED:
      if (!pMe->m_isAddVoiceTag) {
        ContDlgHandler_ShowMsgBox(pMe,
                                  STR_VOICETAG_ABORTED,
                                  pMe->m_speedDialSavedState);
      } else {
        MOVE_TO_STATE(pMe, pMe->m_speedDialSavedState);
      }
      break;

    case DLGRET_CONT_CANCELED:
    case DLGRET_CONT_UNHANDLED:
      MOVE_TO_STATE(pMe, pMe->m_voiceTagSavedState)
      break;

    default:
      MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
      break;
  }
}

/*=============================================================================
FUNCTION: HdlrStateVoiceTagSave

DESCRIPTION:
   Called in response to the STATE_VOICETAG_SAVE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateVoiceTagSave(CContactApp *pMe)
{
   uint16       nSlot;
   ContactInfo *ciContactList = NULL;
   IAddrRec    *pAddrRec = NULL;

   if (NULL == pMe) { 
     ERR("NULL pointer", 0, 0, 0);
     return;
   }
   ciContactList = IVector_ElementAt(pMe->m_contact_iv,
                                         pMe->m_contactSelId);

   if ((ciContactList == NULL) ||
       (pMe->m_fieldInfo.m_name == NULL))
   {
     MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
     return;
   }

   if (pMe->m_speedDialInfo != NULL) {
      //remove the old record from database
      pAddrRec = IADDRBOOK_GetRecByID(pMe->m_addr_book,
                                       (pMe->m_speedDialInfo)->m_id);
      if (pAddrRec !=NULL){
        (void)ContDlgHandler_RemoveRec_VoiceTag(pMe, pAddrRec);
        if (IADDRREC_RemoveRec(pAddrRec) != AEE_SUCCESS) {
           (void)ContDlgHandler_VoiceTag_Remove(pMe, pMe->m_voiceTag);
           MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
           return;
        }
      }
   }

   /* Set Speed Dial Entry */
   nSlot=pMe->m_fldrecs_sdindex;
   if (SUCCESS == ContDlgHandler_SpeedDial_Set(pMe,
                                        ciContactList->m_name,
                                        pMe->m_fieldInfo.m_name,
                                        nSlot,
                                        TRUE)) {
      ContDlgHandler_ShowMsgBox(pMe,
                               STR_MSGBOX_CONTACTADDED_TEXT,
                               pMe->m_speedDialSavedState);
   } else {
      (void)ContDlgHandler_VoiceTag_Remove(pMe, pMe->m_voiceTag);
      MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
   }
}
#endif
/*=============================================================================
FUNCTION: HdlrStateSaveNumber

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSaveNumber(CContactApp *pMe)
{
   pMe->m_isAddNew = TRUE;
   pMe->m_fieldInfo.m_numberType = NUMBERTYPE_MENUITEM_HOME;
   pMe->m_fieldInfo.m_type = STR_FIELDINPUT_NUMBER;

#ifdef FEATURE_MMGSDI
   //if we are saving on RUIM
   if(pMe->m_phoneSaveType == SAVE_TO_RUIM){

      pMe->m_ruimSavedState = STATE_EXITAPPLET;
      if(pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NUMBER_EDIT){
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_NUMBER)
      } else {
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_CONTACT)
      }

   }
   else
#endif /*FEATURE_MMGSDI*/
   { //if we are saving on Phone EFS
      //set it to default type
      pMe->m_fieldInfo.m_numberType = NUMBERTYPE_MENUITEM_HOME;

      if(pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NUMBER_EDIT){
         MOVE_TO_STATE(pMe, STATE_SAVENUMBER_NUMBER)
      } else {
         MOVE_TO_STATE(pMe, STATE_SAVENUMBER_NEWOREXISTING)
      }
   }
}

/*=============================================================================
FUNCTION: HdlrStateSaveNumberNumber

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER_NUMBER state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSaveNumberNumber(CContactApp *pMe)
{
   //show the save number-number  dialog
   ContDlgHandler_ShowDialog(pMe,
                             DLG_N_FIELDINPUT,
                             STATE_SAVENUMBER_NUMBER_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateSaveNumberNumberHandle

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER_NUMBER state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSaveNumberNumberHandle(CContactApp *pMe)
{
   //handler for the return result of save number - number dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_SAVENUMBER_NUMBER);
         } else if (!CContactApp_CanSaveNumber(pMe, pMe->m_fieldInfo.m_name)) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_MSGBOX_DATABASE_ERR_TEXT,
                                      STATE_SAVENUMBER_NUMBER);
         } else {
            MOVE_TO_STATE(pMe, STATE_SAVENUMBER_NEWOREXISTING)
         }
         break;

      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_SAVENUMBER_NUMBER)
         break;

      //If you are middle of editing and pressed send key
      case DLGRET_CONTACT_SENDKEY:
         {
            boolean   ret= FALSE;

            if (NULL == pMe->m_fieldInfo.m_name ||
                WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_FIELDINPUT_NOTEXT_MSG,
                                         STATE_FIELDOPTS_EDIT);
            } else {
               //call the number in the text box
               ret= ContStateMachine_CallFieldNumber(pMe);
               if (FALSE == ret) {
                  MOVE_TO_STATE(pMe, STATE_SAVENUMBER_NUMBER)
               } else {
                  MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
               }
            }
            break;
         }

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}
/*=============================================================================
FUNCTION: HdlrStateSaveNumberNewOrExisting

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER_NEWOREXISTING state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateSaveNumberNewOrExisting(CContactApp *pMe)
{
   //if the save phone type is new
   if (pMe->m_phoneSaveType == SAVE_NEW)
   {
      MOVE_TO_STATE(pMe, STATE_SAVENUMBER_NEWCONTACT)
   } else {
      MOVE_TO_STATE(pMe, STATE_SAVENUMBER_SELCONTACT)
   }
}

/*=============================================================================
FUNCTION: HdlrStateSaveNumberNewContact

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER_NEWCONTACT state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSaveNumberNewContact(CContactApp *pMe)
{
   //show the savenumber - new contact dialog
   ContDlgHandler_ShowDialog(pMe,
                             DLG_NEWCONTACTNAME,
                             STATE_SAVENUMBER_NEWCONTACT_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateSaveNumberNewContactHandle

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER_NEWCONTACT_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateSaveNumberNewContactHandle(CContactApp *pMe)
{
   //handler for the return result of Save number - new contact dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         if(pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NUMBER_EDIT){
            MOVE_TO_STATE(pMe, STATE_SAVENUMBER_NUMBER)
         } else{
            MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
         }
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_contactInfo.m_name ||
             WSTRLEN(pMe->m_contactInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_CONTACTNAME_NOTEXT_MSG,
                                      STATE_SAVENUMBER_NEWCONTACT);
         } else {
            pMe->m_contactInfo.m_class = AEE_ADDR_CAT_NONE;
            MOVE_TO_STATE(pMe, STATE_SAVENUMBER_FINISH_NEW)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_SAVENUMBER_NEWCONTACT)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateSaveNumberFinishNew

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER_FINISH_NEW state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSaveNumberFinishNew(CContactApp *pMe)
{
   //Initialse the new_rec_id
   pMe->m_new_rec_id= 0xffff;
   //create the contact on the database
   if (TRUE == ContDlgHandler_CreateContact(pMe)) {
      MOVE_TO_STATE(pMe, STATE_SAVENUMBER_VIEWCONTACT);
   } else {
      MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
   }

}

/*=============================================================================
FUNCTION: HdlrStateSaveNumberSelContact

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER_SELCONTACT state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateSaveNumberSelContact(CContactApp *pMe)
{
   pMe->m_contactListType = STR_CONTACTLIST_SELECTCONTACT;
   /* Try loading available contacts */
   if (TRUE == ContDlgHandler_LoadAvailContactList(pMe)) {
      if (IVector_Size(pMe->m_contact_iv) == 0) {
         // No records found, so save number to a new contact
         pMe->m_phoneSaveType = SAVE_NEW;
         ContDlgHandler_ShowMsgBox(pMe,
                                   STR_MSGBOX_NOCONTACTS_TEXT,
                                   STATE_SAVENUMBER_NEWCONTACT);

      } else {
         ContDlgHandler_ShowDialog(pMe,
                                   DLG_CONTACTLIST,
                                   STATE_SAVENUMBER_SELCONTACT_HANDLE);
      }
   } else {
      /* No existing contacts, save number to a new contact */
      pMe->m_phoneSaveType = SAVE_NEW;
      ContDlgHandler_ShowMsgBox(pMe,
                                STR_MSGBOX_LOADCONTACTLIST_ERR_TEXT,
                                STATE_SAVENUMBER_NEWCONTACT);
   }

}

/*=============================================================================
FUNCTION: HdlrStateSaveNumberNumberSelContactHandle

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER_SELCONTACT_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSaveNumberSelContactHandle(CContactApp *pMe)
{
   //handle the return result of select contact dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_OK:
         MOVE_TO_STATE(pMe, STATE_SAVENUMBER_FINISH_EXISTING)
         break;

      case DLGRET_CONT_CANCELED:
         if(pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NUMBER_EDIT){
            MOVE_TO_STATE(pMe, STATE_SAVENUMBER_NUMBER)
         } else{
            MOVE_TO_STATE(pMe, STATE_EXITAPPLET);
         }

         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_SAVENUMBER_SELCONTACT)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }
}
/*=============================================================================
FUNCTION: HdlrStateSaveFinish Existing

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER_FINISH_EXISTING state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSaveNumberFinishExisting(CContactApp *pMe)
{
   int      ret;
   pMe->m_new_rec_id = 0xffff;
   do {
      ret = ContDlgHandler_LoadAEEAddrFields(pMe);
      if (FALSE == ret) {
         break;
      }
      ret = ContDlgHandler_AddField(pMe);
   } while (0); /*lint !e717*/

   if (AEE_SUCCESS == ret) {
      MOVE_TO_STATE(pMe, STATE_SAVENUMBER_VIEWCONTACT);
   } else {
      // Ack! An error occured while saving the data
      switch(ret) {
         case ENOMEMORY:
            ContDlgHandler_ShowMsgBox(pMe,
                                STR_MSGBOX_LIMIT_REACHED_ERR_TEXT,
                                STATE_EXITAPPLET);
            break;

         default:
            ContDlgHandler_ShowMsgBox(pMe,
                                STR_MSGBOX_DATABASE_ERR_TEXT,
                                STATE_EXITAPPLET);
            break;
      }
   }

}
/*=============================================================================
FUNCTION: HdlrStateSaveNumberViewContact Existing

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER_VIEWCONTACT state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSaveNumberViewContact(CContactApp *pMe)
{
   ContactInfo *ci = NULL;
   //if we were saving to the existing contact then get the
   //name of the contact to which we added.
   if(pMe->m_phoneSaveType == SAVE_TO_EXISTING){
      ci = IVector_ElementAt(pMe->m_contact_iv,
                             pMe->m_contactSelId);
      if (ci==NULL)
      {
        ERR("ci invalid pointer", 0, 0, 0);
        MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
        return;
      }
      FREE_STRING(pMe->m_contactInfo.m_name);
      pMe->m_contactInfo.m_name = WSTRDUP(ci->m_name);

   }

   ci = NULL;
   //Free the contact info vector and add newly added contact
   //to the vector. Now the Vector will have only one contact.
   //View Contact state uses this vector to show the contact.
   ContDlgHandler_FreeContactList(pMe,
                                  pMe->m_contact_iv);
   ci = MALLOC(sizeof(*ci));
   if((ci!=NULL) && (pMe->m_new_rec_id!= 0xffff)) {
      ci->m_id = pMe->m_new_rec_id;
      ci->m_name = WSTRDUP(pMe->m_contactInfo.m_name);

      if((ci->m_name != NULL)&&
         (SUCCESS == IVector_AddElement(pMe->m_contact_iv, ci))) {

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

         //set the contact select to the first one, anyways we have only one
         pMe->m_contactSelId = 0;

         //set the return state from view contact to main menu
         pMe->m_viewContactSavedState= STATE_EXITAPPLET;

         MOVE_TO_STATE(pMe, STATE_CONTACT);
      } else{
         MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
      }
   } else {
      MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
   }
}

#ifdef FEATURE_MMGSDI
/*=============================================================================
FUNCTION: HdlrStateSaveNumberFinishedSaveOnRUIM

DESCRIPTION:
   Called in response to the STATE_SAVENUMBER_FINISH_SAVEONRUIM state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateSaveNumberFinishSaveOnRUIM(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateSaveNumberFinishSaveOnRUIM: NULL pMe", 0, 0, 0);
      return;
   }
   //Save the phone number on RUIM
   pMe->m_ruimCancelState = STATE_EXITAPPLET;
   pMe->m_ruimSavedState = STATE_EXITAPPLET;
   pMe->m_ruimListContacts = FALSE;

   if (NULL == pMe->m_contactInfo.m_name ||
       WSTRLEN(pMe->m_contactInfo.m_name) <= 0) {
      ContDlgHandler_ShowMsgBox(pMe,
                                STR_CONTACTNAME_NOTEXT_MSG,
                                STATE_EXITAPPLET);
   } else {
      if (SUCCESS == ContDlgHandler_RUIM_Set(pMe,
                                             pMe->m_contactInfo.m_name,
                                             pMe->m_fieldInfo.m_name)) {
         ContDlgHandler_ShowMsgBox(pMe,
                                   STR_MSGBOX_CONTACTADDED_TEXT,
                                   STATE_EXITAPPLET);
      } else {
         MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
      }
   }

}
#endif

/*=============================================================================
FUNCTION: HdlrStateSelectFieldHandle

DESCRIPTION:
   Called in response to the STATE_SELECTFIELD_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateSelectFieldHandle(CContactApp *pMe)
{
   AEEAddrField *cf;
   boolean exitApp;

   if (NULL == pMe) { 
      ERR("NULL pointer", 0, 0, 0);
      return;
   }

   if (pMe->m_startMethod != STARTMETHOD_SELECTFIELD)
   {
     ERR("Invalid start method %x", pMe->m_startMethod, 0, 0);
     MOVE_TO_STATE(pMe, STATE_CONTACT_REFRESH);
     return;
   }

   if (pMe->m_selNumberCallback != NULL) {

      cf = (AEEAddrField*)IVector_ElementAt(pMe->m_fldrecs_iv,
                                            pMe->m_fieldSelId);

      //call the function passed as the call back function
      exitApp = pMe->m_selNumberCallback(pMe->m_rec, cf);
   } else {
      exitApp = TRUE;
   }


   if (exitApp) {
      MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
   } else {
      MOVE_TO_STATE(pMe, STATE_CONTACT_REFRESH)
   }

}

#ifdef FEATURE_MMGSDI
/*=============================================================================
FUNCTION: HdlrStateContRUIM

DESCRIPTION:
   Called in response to the STATE_CONT_RUIM state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContRUIM(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateContRUIM: NULL pMe", 0, 0, 0);
      return;
   }

   if (FALSE == pMe->m_ruimListContacts) {
      //If we are saving the phone number from the EFS contacts,
      //get the currently selected contact info and use that as the contact
      //name.
      ContactInfo *ci;
      ci = IVector_ElementAt(pMe->m_contact_iv,
                             pMe->m_contactSelId);

      if ((ci == NULL) ||
          (pMe->m_fieldInfo.m_name == NULL))
      {
        MSG_ERROR("Invalid information for contact %x %x %x", ci,
                  pMe->m_fieldInfo.m_type, pMe->m_fieldInfo.m_name);
        return;
      }

      if (SUCCESS == ContDlgHandler_RUIM_Set(pMe,
                                             ci->m_name,
                                             pMe->m_fieldInfo.m_name)) {
         //if a contact is added then set this flag so that it will reload
         //the contact list
         pMe->m_bContListNeedsReload = TRUE;
         ContDlgHandler_ShowMsgBox(pMe,
                                   STR_MSGBOX_CONTACTADDED_TEXT,
                                   pMe->m_ruimSavedState);

      } else {
         MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
      }
   } else {
      // Ensure that m_contactInfo is empty
      FREE_CONTACTINFO(pMe->m_contactInfo);

      pMe->m_ruimDlg_menuSel = 0;
      MOVE_TO_STATE(pMe, STATE_RUIM_RESUME);
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMResume

DESCRIPTION:
   Called in response to the STATE_RUIM_RESUME state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMResume(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMResume: NULL pMe", 0, 0, 0);
      return;
   }

   pMe->m_searchStr       = NULL;

   if (TRUE == ContDlgHandler_LoadContactList(pMe)) {
      if (IVector_Size(pMe->m_contact_iv) == 0) {
         // No records found -- return to main
         ContDlgHandler_ShowMsgBox(pMe,
                                   STR_MSGBOX_NOCONTACTS_TEXT,
                                   STATE_CONT_MAIN);

      } else {
         //show the RUIM dialog here
         ContDlgHandler_ShowDialog(pMe,
                                   DLG_RUIM,
                                   STATE_RUIM_HANDLE);
      }
   } else {
      MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTLIST_ERROR)
   }

}
/*=============================================================================
FUNCTION: HdlrStateRUIMHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return result of RUIM dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, pMe->m_ruimCancelState)
         break;

      case DLGRET_RUIM_ASSIGN:
         if (pMe->m_ruimListContacts) {
            MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTLIST)
         } else {
           MSG_ERROR("Unreachable case reached", 0, 0, 0);
           MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
           break;
         }
         break;

      case DLGRET_RUIM_CONTACTOPTS:
         pMe->m_viewContactSavedState=STATE_RUIM_RESUME;
         MOVE_TO_STATE(pMe, STATE_RUIM_SELCONTACT_REFRESH)
         break;

      case DLGRET_RUIM_ADDNEW:
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW)
         break;

      case DLGRET_RUIM_CONTACTLIST_SENDKEY:
         {
            boolean resume = TRUE;
            ContactInfo *ci;
            AEEAddrField *cf;

            ci = IVector_ElementAt(pMe->m_contact_iv,
                                   pMe->m_contactSelId);
            if (ci != NULL) {
               //load all the fields for this contact
               if (TRUE == ContDlgHandler_LoadAEEAddrFields(pMe)) {

                  cf =(AEEAddrField*)IVector_ElementAt(pMe->m_fldrecs_iv,
                                                     pMe->m_contactListFieldID);
                  if (TRUE == CContactApp_CallNumber(pMe, cf)) {
                     resume = FALSE;
                  }
               }
            }

            if (TRUE == resume) {
               MOVE_TO_STATE(pMe, STATE_RUIM_RESUME)
            } else {
               MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
            }
            break;
         }

      case DLGRET_RUIM_REMOVE:
         MOVE_TO_STATE(pMe, STATE_RUIM_DELETE)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_RESUME)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMSelContactRefresh

DESCRIPTION:
   Called in response to the STATE_RUIM_SELCONTACT_REFRESH state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMSelContactRefresh(CContactApp *pMe)
{
   //reload the fields of the selected contact from the database
   (void) ContDlgHandler_LoadAEEAddrFields(pMe);

   if (pMe->m_fieldSelId >= IVector_Size(pMe->m_fldrecs_iv)) {
      pMe->m_fieldSelId = OPTIONS_MENU_ID;
   }
   MOVE_TO_STATE(pMe, STATE_RUIM_SELCONTACT_RESUME)

}

/*=============================================================================
FUNCTION: HdlrStateRUIMSelContactResume

DESCRIPTION:
   Called in response to the STATE_RUIM_SELCONTACT_RESUME state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

============================================================================*/
static void HdlrStateRUIMSelContactResume(CContactApp *pMe)
{
   //show the contact dialog with the fields for the selected contact
   pMe->m_selectingContact = FALSE;
   ContDlgHandler_ShowDialog(pMe,
                             DLG_CONTACT,
                             STATE_RUIM_SELCONTACT_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMSelContactHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_SELCONTACT_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMSelContactHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMSelContactHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return result of Ruim selected contact dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, pMe->m_viewContactSavedState)
         break;

      case DLGRET_CONTACT_OPTS:
         pMe->m_contactOptDlg_menuSel= CONTACTOPTIONS_MENUITEM_ADDNEW;
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS)
         break;

      case DLGRET_CONTACT_FIELDSEL:
         pMe->m_fieldOptsDlg_menuSel = FIELDOPTIONS_MENUITEM_NUMBERTYPE;
         if (pMe->m_contactListType == STR_CONTACTLIST_RUIM_FDN) {
           /* Set state to return to after PIN verification */
           pMe->m_pinRetState = STATE_RUIM_FIELDOPTS;
           validate_PIN2(pMe);
         } else {
           MOVE_TO_STATE(pMe, STATE_RUIM_FIELDOPTS)
         }
         break;

      case DLGRET_CONTACT_SENDKEY:
         {
            AEEAddrField *cf;
            boolean   ret;

            cf = (AEEAddrField*)IVector_ElementAt(pMe->m_fldrecs_iv,
                                                  pMe->m_fieldSelId);
            //call the phone number and go to the Idle applet
            ret = CContactApp_CallNumber(pMe, cf);

            if (FALSE == ret) {
               MOVE_TO_STATE(pMe, STATE_RUIM_RESUME)
            } else {
               MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
            }
            break;
         }

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_RESUME)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOpts

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOpts(CContactApp *pMe)
{
   ContactInfo *ci;

   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactOpts: NULL pMe", 0, 0, 0);
      return;
   }

   //Get the current selected contact
   ci = IVector_ElementAt(pMe->m_contact_iv,
                          pMe->m_contactSelId);
   if (ci == NULL)
   {
     MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
     ERR("NULL pointer", 0, 0, 0);
     return;
   }
   pMe->m_contactInfo.m_id = ci->m_id;
   pMe->m_contactInfo.m_class = ci->m_class;

   FREE_STRING(pMe->m_contactInfo.m_name);
   /* save in temperory location, incase FDN pin verification is going
      to be performed*/
   pMe->m_tmpName = WSTRDUP(ci->m_name); 

   if (pMe->m_contactListType == STR_CONTACTLIST_RUIM_FDN) {
     /* Set state to return to after PIN verification */
     pMe->m_pinRetState = STATE_RUIM_CONTACTOPTS_RESUME;
     validate_PIN2(pMe);
   } else {
     MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_RESUME)
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsResume

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_RESUME state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsResume(CContactApp *pMe)
{
  /* see if anything was saved to temperory name location and if so
     copy it back to m_contactInfo.m_name*/
  if (pMe->m_tmpName != NULL) {
    pMe->m_contactInfo.m_name = pMe->m_tmpName;
    pMe->m_tmpName = NULL;
  }
  //show the contact options dialog
  ContDlgHandler_ShowDialog(pMe,
			    DLG_CONTACTOPTIONS,
			    STATE_RUIM_CONTACTOPTS_HANDLE);
  
}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactOptsHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //Handle for the return result of RUIM- Contact options dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_SELCONTACT_RESUME)
         break;
      
      case DLGRET_CONTACTOPTS_ADDNEW:
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW)
         break;

      case DLGRET_CONTACTOPTS_EDITNAME:
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_EDITNAME)
         break;

      case DLGRET_CONTACTOPTS_DELETE:
         MOVE_TO_STATE(pMe, STATE_RUIM_DELETE)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_SELCONTACT_RESUME)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsEditName

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_EDITNAME state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsEditName(CContactApp *pMe)
{
   //show the RUIM - edit name dialog
   pMe->m_isAddNew = FALSE;

   ContDlgHandler_ShowDialog(pMe,
                             DLG_CONTACTNAME,
                             STATE_RUIM_CONTACTOPTS_EDITNAME_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsEditNameHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_EDITNAME_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsEditNameHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactOptsEditNameHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return result of RUIM - edit name dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_RESUME)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_contactInfo.m_name ||
             WSTRLEN(pMe->m_contactInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_CONTACTNAME_NOTEXT_MSG,
                                      STATE_RUIM_CONTACTOPTS_EDITNAME);
         } else {
            if (TRUE == ContDlgHandler_SaveContactInfo(pMe)) {
               //find the return state after we show the message
               ContactAppState stateToReturn = STATE_RUIM_RESUME;

               if((pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NUMBER_EDIT)||
                 (pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT)){
                  stateToReturn = STATE_EXITAPPLET;
               }else if (pMe->m_bAddNewMenu == TRUE){
                 stateToReturn = STATE_CONT_MAIN;
               }

               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_MSGBOX_CONTACTSAVED_TEXT,
                                         stateToReturn);
            } else {
               MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
            }
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_CONTACTOPTS_EDITNAME)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsAddNew

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_ADDNEW state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsAddNew(CContactApp *pMe)
{
   //show the add new option dialog of contact.
   //Allow you to enter new fields for the selectd contact
   pMe->m_isAddNew = TRUE;
   // the record already exists and we are adding a new field to it
   pMe->m_isExisting = TRUE;
   ContDlgHandler_ShowDialog(pMe,
                             DLG_ADDNEW_RUIM,
                             STATE_RUIM_CONTACTOPTS_ADDNEW_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsAddNewHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_ADDNEW_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsAddNewHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactOptsAddNewHandle: NULL pMe", 0, 0, 0);
      return;
   }

   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_OK:
         FREE_STRING(pMe->m_fieldInfo.m_name);

         if ((STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) ||
                (STR_FIELDINPUT_ADDNUMBER == pMe->m_fieldInfo.m_type)) {
            MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER)
         } else if (STR_FIELDINPUT_ID == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_ID)
         } else if (STR_FIELDINPUT_SECNAME == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_NAME)
         } else {
            MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_EMAIL)
         }
         break;

      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW)
         break;

      case DLGRET_RUIM_EMPTY_ADDNEW:
         ContDlgHandler_ShowMsgBox(pMe,
                                   STR_MSGBOX_RUIM_EMPTY_TEXT,
                                   STATE_RUIM_CONTACTOPTS);
         break;
      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }
}

/*=============================================================================
FUNCTION: HdlrStateRUIMAddNew

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNew(CContactApp *pMe)
{
   //show the add new dialog here
   pMe->m_isAddNew = TRUE;
   pMe->m_isExisting = FALSE;
   ContDlgHandler_ShowDialog(pMe,
                             DLG_ADDNEW_RUIM,
                             STATE_RUIM_ADDNEW_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateRUIMAddNewHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMAddNewHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return result of add new dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_OK:
         FREE_STRING(pMe->m_fieldInfo.m_name);

         if (STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_NUMBER)
         } else if (STR_FIELDINPUT_ADDNUMBER == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_ADDNUMBER)
         } else if (STR_FIELDINPUT_ID == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_ID)
         } else if (STR_FIELDINPUT_SECNAME == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_NAME)
         } else {
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_EMAIL)
         }
         break;

      case  DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;
   }

}


/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewNumber

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_NUMBER state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewNumber(CContactApp *pMe)
{
   //show the RUIM - add new number dialog
   pMe->m_fieldInfo.m_type = STR_FIELDINPUT_NUMBER;
   pMe->m_isAddNew = TRUE;
   //if it is normal start then clean up the field info
   if (pMe->m_startMethod == STARTMETHOD_NORMAL) {
      FREE_STRING(pMe->m_fieldInfo.m_name);
   }
   ContDlgHandler_ShowDialog(pMe,
                             DLG_N_FIELDINPUT,
                             STATE_RUIM_ADDNEW_NUMBER_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewNumberHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_NUMBER_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewNumberHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMAddNewNumberHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return result of RUIM - add new number dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_RUIM_ADDNEW_NUMBER);
         } else {
            if (!CContactApp_CanSaveNumber(pMe, pMe->m_fieldInfo.m_name)) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_MSGBOX_DATABASE_ERR_TEXT,
                                         STATE_RUIM_ADDNEW_NUMBER);
            } else {
               MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_CONTACT)
            }
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_NUMBER)
         break;

      //If you are middle of editing and pressed send key
      case DLGRET_CONTACT_SENDKEY:
         {
            boolean   ret= FALSE;

            if (NULL == pMe->m_fieldInfo.m_name ||
                WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_FIELDINPUT_NOTEXT_MSG,
                                         STATE_FIELDOPTS_EDIT);
            } else {
               //Call the number in the text box
               ret=ContStateMachine_CallFieldNumber(pMe);
               if (FALSE == ret) {
                  MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW)
               } else {
                  MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
               }
            }
            break;
         }

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewAddNumber

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_ADDNUMBER state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewAddNumber(CContactApp *pMe)
{
   //show the RUIM - add new number dialog
   pMe->m_fieldInfo.m_type = STR_FIELDINPUT_ADDNUMBER;
   pMe->m_isAddNew = TRUE;
   //if it is normal start then clean up the field info
   if (pMe->m_startMethod == STARTMETHOD_NORMAL) {
      FREE_STRING(pMe->m_fieldInfo.m_name);
   }
   ContDlgHandler_ShowDialog(pMe,
                             DLG_N_FIELDINPUT,
                             STATE_RUIM_ADDNEW_ADDNUMBER_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewAddNumberHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_ADDNUMBER_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewAddNumberHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMAddNewAddNumberHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return result of RUIM - add new number dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_RUIM_ADDNEW_ADDNUMBER);
         } else {
            if (!CContactApp_CanSaveNumber(pMe, pMe->m_fieldInfo.m_name)) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_MSGBOX_DATABASE_ERR_TEXT,
                                         STATE_RUIM_ADDNEW_ADDNUMBER);
            } else {
               MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_CONTACT)
            }
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_ADDNUMBER)
         break;

      //If you are middle of editing and pressed send key
      case DLGRET_CONTACT_SENDKEY:
         {
            boolean   ret= FALSE;

            if (NULL == pMe->m_fieldInfo.m_name ||
                WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_FIELDINPUT_NOTEXT_MSG,
                                         STATE_FIELDOPTS_EDIT);
            } else {
               //Call the number in the text box
               ret=ContStateMachine_CallFieldNumber(pMe);
               if (FALSE == ret) {
                  MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW)
               } else {
                  MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
               }
            }
            break;
         }

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewContact

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_CONTACT state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewContact(CContactApp *pMe)
{
   pMe->m_isAddNew = TRUE;
   FREE_STRING(pMe->m_contactInfo.m_name);
   //show the RUIM - add new contact dialog
   ContDlgHandler_ShowDialog(pMe,
                             DLG_NEWCONTACTNAME,
                             STATE_RUIM_ADDNEW_CONTACT_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewContactHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_CONTACT_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewContactHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMAddNewContactHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return result of  RUIM - add new contact dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         if(pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT){
            MOVE_TO_STATE(pMe, STATE_EXITAPPLET);
         }else if (STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_NUMBER)
         }else if (STR_FIELDINPUT_ADDNUMBER == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_ADDNUMBER)
         } else if (STR_FIELDINPUT_ID == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_ID)
         } else if (STR_FIELDINPUT_SECNAME == pMe->m_fieldInfo.m_type) {
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_NAME)
         } else {
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_EMAIL)
         }
         break;

      case DLGRET_CONT_OK:
         //if the text box is empty, then
         if (NULL == pMe->m_contactInfo.m_name ||
             WSTRLEN(pMe->m_contactInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_CONTACTNAME_NOTEXT_MSG,
                                      STATE_RUIM_ADDNEW_CONTACT);
         } else {
            ContactInfo *ci = NULL;
            // Initialise this to default value. After we add the record this
            // will be changed in the create contact function
            pMe->m_new_rec_id = 0xffff;

            if (SUCCESS == ContDlgHandler_RUIM_Set(pMe,
                                                   pMe->m_contactInfo.m_name,
                                                   pMe->m_fieldInfo.m_name)) {

               //Free the contact info vector and add newly added contact
               //to the vector. Now the Vector will have only one contact.
               //View Contact state uses this vector to show the contact.
               ContDlgHandler_FreeContactList(pMe,
                                              pMe->m_contact_iv);
               ci = MALLOC(sizeof(*ci));
               if((ci!=NULL) && (pMe->m_new_rec_id!= 0xffff)) {
                  ci->m_id = pMe->m_new_rec_id;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
                  if(pMe->m_uimSlot1)
                    ci->m_class = AEE_ADDR_CAT_SIM_ADN;
                  else
                    ci->m_class = AEE_ADDR_CAT_SIM_ADN2;
#else
                  ci->m_class = AEE_ADDR_CAT_SIM_ADN;
#endif
                  ci->m_name = WSTRDUP(pMe->m_contactInfo.m_name);

                  if((ci->m_name != NULL)&&
                     (SUCCESS == IVector_AddElement(pMe->m_contact_iv, ci))) {

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

                     //set the contact select to the first one, anyways we
                     //have only one
                     pMe->m_contactSelId = 0;

                     //set the return state from view contact to main menu
                     pMe->m_viewContactSavedState= pMe->m_ruimSavedState;

                     MOVE_TO_STATE(pMe, STATE_RUIM_SELCONTACT_REFRESH);
                  } else{
                     MOVE_TO_STATE(pMe, pMe->m_ruimSavedState)
                  }
               } else{
                  MOVE_TO_STATE(pMe, pMe->m_ruimSavedState)
               }
            } else {
               MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
            }
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_RESUME)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMFieldOpts

DESCRIPTION:
   Called in response to the STATE_RUIM_FIELDOPTS state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMFieldOpts(CContactApp *pMe)
{
   // Set pMe->m_fieldInfo.m_type, pMe->m_fieldInfo.m_numberType and
   // mMe->fieldInput to the correct values for the selected field
   FieldInfo *fi;
   AEEAddrField  *cf;
   ContactInfo *ci;

   if (!pMe) {
      MSG_ERROR("HdlrStateRUIMFieldOpts: NULL pMe", 0, 0, 0);
      return;
   }

   ci = IVector_ElementAt(pMe->m_contact_iv,
                                       pMe->m_contactSelId);

   if (ci) {
      /* Update the category of the contact */
      pMe->m_contactInfo.m_id = ci->m_id;
      pMe->m_contactInfo.m_class = ci->m_class;

      FREE_STRING(pMe->m_contactInfo.m_name);
      pMe->m_contactInfo.m_name = WSTRDUP(ci->m_name);
   } else {
      MSG_ERROR("Contact not found", 0, 0, 0);
      MOVE_TO_STATE(pMe, STATE_ERROR_INIT)
      return;
   }

   cf = (AEEAddrField*)IVector_ElementAt(pMe->m_fldrecs_iv,
                                         pMe->m_fieldSelId);
   if (cf == NULL)
   {
     MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
     ERR("NULL pointer", 0, 0, 0);
     return;
   }

   fi = ContDlgHandler_AEEAddrField2FieldInfo(pMe, cf);
   if (fi == NULL)
   {
     MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
     ERR("NULL pointer", 0, 0, 0);
     return;
   }

   FREE_FIELDINFO(pMe->m_fieldInfo);
   pMe->m_fieldInfo = *fi;
   FREE(fi);

   ContDlgHandler_ShowDialog(pMe,
                             DLG_FIELDOPTIONS,
                             STATE_RUIM_FIELDOPTS_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMFieldOptsHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_FIELDOPTS_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMFieldOptsHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMFieldOptsHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return result of RUIM - field options dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_SELCONTACT_REFRESH)
         break;

      case DLGRET_FIELDOPTS_EDIT:
         MOVE_TO_STATE(pMe, STATE_RUIM_FIELDOPTS_EDIT)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_FIELDOPTS)
         break;

      case DLGRET_FIELDOPTS_ADDTOSPEEDDIAL:
#ifdef FEATURE_PHONE_VR
         pMe->m_isAddVoiceTag = FALSE;
#endif
         pMe->m_speedDialSavedState = STATE_RUIM_FIELDOPTS;
         MOVE_TO_STATE(pMe, STATE_SPEEDDIAL_SELSLOT)
         break;

#ifdef FEATURE_PHONE_VR
      case DLGRET_FIELDOPTS_ADDVOICETAG:
         pMe->m_isAddVoiceTag = TRUE;
         pMe->m_speedDialSavedState = STATE_RUIM_FIELDOPTS;
         pMe->m_voiceTagSavedState = STATE_SPEEDDIAL_SELSLOT_RESUME;
         MOVE_TO_STATE(pMe, STATE_SPEEDDIAL_SELSLOT)
         break;
#endif

      case DLGRET_FIELDOPTS_PHONE:
         {
            ContactInfo *ci;
            boolean      ret;
            uint16       wOldCat;
            uint16       wOldNumberType;

            ci = IVector_ElementAt(pMe->m_contact_iv,
                                   pMe->m_contactSelId);
            if ((ci == NULL) ||
                ((STR_FIELDINPUT_NUMBER != pMe->m_fieldInfo.m_type) &&
                (STR_FIELDINPUT_ADDNUMBER != pMe->m_fieldInfo.m_type)) ||
                (pMe->m_fieldInfo.m_name == NULL))
            {
              MSG_ERROR("Invalid information %x %x %x", ci,
                        pMe->m_fieldInfo.m_type,pMe->m_fieldInfo.m_name);
              MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
              return;
            }

            //work around. temporary modify the category and numbertype before
            //creating the record and restore when we are done with it
            wOldCat = pMe->m_contactInfo.m_class;
            pMe->m_contactInfo.m_class    = AEE_ADDR_CAT_NONE;
            wOldNumberType = pMe->m_fieldInfo.m_numberType;
            pMe->m_fieldInfo.m_numberType = NUMBERTYPE_MENUITEM_HOME;
            //create the contact in the database
            ret= ContDlgHandler_CreateContact(pMe);
            pMe->m_contactInfo.m_class    = wOldCat;
            pMe->m_fieldInfo.m_numberType = wOldNumberType;

            if (TRUE ==ret) {
               pMe->m_bContListNeedsReload = TRUE;
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_MSGBOX_CONTACTADDED_TEXT,
                                         STATE_FIELDOPTS);

            } else {
               MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
            }
            break;
         }

      case DLGRET_FIELDOPTS_DELETE:
         MOVE_TO_STATE(pMe, STATE_RUIM_FIELDOPTS_DELETE)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMFieldOptsEdit

DESCRIPTION:
   Called in response to the STATE_RUIM_FIELDOPTS_EDIT state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMFieldOptsEdit(CContactApp *pMe)
{
   //show the field options edit dialog
   pMe->m_isAddNew = FALSE;
   ContDlgHandler_ShowDialog(pMe,
                     ContStateMachine_SelFieldInputDlg(pMe->m_fieldInfo.m_type),
                     STATE_RUIM_FIELDOPTS_EDIT_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMFieldOptsEditHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_FIELDOPTS_EDIT_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMFieldOptsEditHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMFieldOptsEditHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return result of RUIM - field opts dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_RUIM_FIELDOPTS_EDIT);
         } else {
            if ( ((STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) ||
                    (STR_FIELDINPUT_ADDNUMBER == pMe->m_fieldInfo.m_type))&&
                 (!CContactApp_CanSaveNumber(pMe, pMe->m_fieldInfo.m_name)) ) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_MSGBOX_DATABASE_ERR_TEXT,
                                         STATE_RUIM_FIELDOPTS_EDIT);
            } else {
               pMe->m_fieldInfo.m_numberType = NUMBERTYPE_RUIM;
               if (TRUE == ContDlgHandler_SaveField(pMe)) {
                  //RUIM will only store number.
                  if ((STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) ||
                        (STR_FIELDINPUT_ADDNUMBER == pMe->m_fieldInfo.m_type)) {
                     ContDlgHandler_ShowMsgBox(pMe,
                                               STR_MSGBOX_NUMBERSAVED_TEXT,
                                               STATE_RUIM_CONTACTOPTS_RESUME);
                  }
               } else {
                  MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
               }
            }
         }
         break;

      //If you are middle of editing and pressed send key
      case DLGRET_CONTACT_SENDKEY:
         {
            boolean   ret= FALSE;

            if (NULL == pMe->m_fieldInfo.m_name ||
                WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_FIELDINPUT_NOTEXT_MSG,
                                         STATE_FIELDOPTS_EDIT);
            } else {
               //Call the phone number in the text box
               ret=ContStateMachine_CallFieldNumber(pMe);
               if (FALSE == ret) {
                  MOVE_TO_STATE(pMe, STATE_RUIM_FIELDOPTS_EDIT)
               } else {
                  MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
               }
            }
            break;
         }

      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_FIELDOPTS)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_FIELDOPTS_EDIT)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMDelete

DESCRIPTION:
   Called in response to the STATE_RUIM_DELETE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMDelete(CContactApp *pMe)
{
   //Get the confirmationa again before deleting the contact from RUIM
   pMe->m_yesnoTimeout = FALSE;
   pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_DELETERUIM;
   ContDlgHandler_ShowDialog(pMe,
                             DLG_YESNO,
                             STATE_RUIM_DELETE_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMDeleteHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_DELETE_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMDeleteHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMDeleteHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return return of RUIM delete dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_YES:
         if (SUCCESS == ContDlgHandler_RUIM_Clear(pMe)) {
            //save the state
            ContactAppState stateToReturn = STATE_RUIM_RESUME;

            if((pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NUMBER_EDIT)||
              (pMe->m_startMethod == STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT)){
               stateToReturn = STATE_EXITAPPLET;
            } else if (pMe->m_bAddNewMenu ==TRUE){
               stateToReturn = STATE_CONT_MAIN;
            }

            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_MSGBOX_REMOVEDFROMRUIM_TEXT,
                                      stateToReturn);
            break;
         }

      /*lint -fallthrough*/

      case DLGRET_CONT_CANCELED:
      case DLGRET_CONT_NO:
      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_RESUME);
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactList

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTLIST state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactList(CContactApp *pMe)
{
   pMe->m_contactListType = STR_CONTACTLIST_ALLCONTACTS;
   pMe->m_selectingContact = TRUE;
   pMe->m_contactSelId = 0;
   MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTLIST_RELOAD)
}
/*=============================================================================
FUNCTION: HdlrStateRUIMContactListReload

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTLIST_RELOAD state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateRUIMContactListReload(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactListReload: NULL pMe", 0, 0, 0);
      return;
   }

   //load all the contacts on the RUIM
   if (TRUE == ContDlgHandler_LoadContactList(pMe)) {
      if (IVector_Size(pMe->m_contact_iv) == 0) {
         // No records found -- return to main
         ContDlgHandler_ShowMsgBox(pMe,
                                   STR_MSGBOX_NOCONTACTS_TEXT,
                                   STATE_CONT_RUIM);

      } else {
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTLIST_RESUME)
      }
   } else {
      MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTLIST_ERROR)
   }
}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactListResume

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTLIST_RESUME state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateRUIMContactListResume(CContactApp *pMe)
{
   //show the contact list dialog here
   ContDlgHandler_ShowDialog(pMe,
                             DLG_CONTACTLIST,
                             STATE_RUIM_CONTACTLIST_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactListError

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTLIST_ERROR state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactListError(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactListError: NULL pMe", 0, 0, 0);
      return;
   }

   switch (pMe->m_RecInitRetVal) {
     case EITEMBUSY:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_UIMNOTREADY_ERR_TEXT,
                                 STATE_CONT_MAIN);
       break;
     case ENOSUCH:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_NOSIMFILE_ERR_TEXT,
                                 STATE_CONT_MAIN);
       break;
     case EBADSTATE:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_RESTRICTED_ERR_TEXT,
                                 STATE_CONT_MAIN);
       break;
     case ENEEDMORE:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_NEEDMORE_ERR_TEXT,
                                 STATE_CONT_MAIN);
       break;
     default:
      // NOTE:  This error condition is treated a little differently
      //        than most of the other errors.  Suppose that
      //        ContDlgHandler_LoadContactList() failed to load contact 100
      //        due to lack of memory, we want the user to be
      //        able to manipulate contacts 0..99 but still notify them
      //        that something isn't quite right.
        ContDlgHandler_ShowMsgBox(pMe,
                                  STR_MSGBOX_LOADCONTACTLIST_ERR_TEXT,
                                  STATE_RUIM_CONTACTLIST_RESUME);

   }
}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactListHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTLIST_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateRUIMContactListHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactListHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return result of contact list dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_OK:
      case DLGRET_CONTACTLIST_SENDKEY:
      // Note: pMe->m_contactSelId contains the selected contact
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACT)
         break;

      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_CONT_RUIM)
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTLIST_RESUME)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContact

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACT state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContact(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContact: NULL pMe", 0, 0, 0);
      return;
   }

   pMe->m_fieldSelId = OPTIONS_MENU_ID; // Select 'Options' by default
   MOVE_TO_STATE(pMe, STATE_RUIM_CONTACT_REFRESH)

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactRefresh

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTAC_REFRESH state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactRefresh(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactRefresh: NULL pMe", 0, 0, 0);
      return;
   }

   //reload the contact fields
   (void) ContDlgHandler_LoadAEEAddrFields(pMe);

   if (pMe->m_fieldSelId >= IVector_Size(pMe->m_fldrecs_iv)) {
      pMe->m_fieldSelId = OPTIONS_MENU_ID;
   }
   MOVE_TO_STATE(pMe, STATE_RUIM_CONTACT_RESUME)

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactResume

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACT_RESUME state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactResume(CContactApp *pMe)
{
   ContDlgHandler_ShowDialog(pMe,
                             DLG_CONTACT,
                             STATE_RUIM_CONTACT_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACT_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the return result of RUIM - contact dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONTACT_OPTS:
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTLIST_RESUME)
         break;

      case DLGRET_CONTACT_FIELDSEL:
      case DLGRET_CONTACT_SENDKEY:
         {
            FieldInfo *fi;
            AEEAddrField  *cf;

            cf = (AEEAddrField*)IVector_ElementAt(pMe->m_fldrecs_iv,
                                                  pMe->m_fieldSelId);
            if (cf == NULL)
            {
              MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
              ERR("NULL pointer", 0, 0, 0);
              return;
            }

            fi = ContDlgHandler_AEEAddrField2FieldInfo(pMe, cf);
            if (fi == NULL)
            {
              MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
              ERR("NULL pointer", 0, 0, 0);
              return;
            }

            FREE_FIELDINFO(pMe->m_fieldInfo);
            pMe->m_fieldInfo = *fi;
            FREE(fi);

            if ((pMe->m_fieldInfo.m_type != STR_FIELDINPUT_NUMBER) &&
                  (pMe->m_fieldInfo.m_type != STR_FIELDINPUT_ADDNUMBER)) {
               MOVE_TO_STATE(pMe, STATE_RUIM_CONTACT_NOTANUMBER)
            } else {
               MOVE_TO_STATE(pMe, STATE_RUIM_ASSIGN)
            }
            break;
         }

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACT_RESUME)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactNotANUMBER

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACT_NOTANUMBER state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactNotANumber(CContactApp *pMe)
{
   // show the RUIM - not a number dialog
   ContDlgHandler_ShowMsgBox(pMe,
                             STR_MSGBOX_ONLYNUMBERSONRUIM_TEXT,
                             STATE_RUIM_CONTACT_RESUME);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMAssign

DESCRIPTION:
   Called in response to the STATE_RUIM_ASSIGN state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAssign(CContactApp *pMe)
{
   int          ret;
   ContactInfo *ci;

   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMAssign: NULL pMe", 0, 0, 0);
      return;
   }

   if (pMe->m_contactInfo.m_name != NULL) {
      ci = &pMe->m_contactInfo;
   } else {
      ci = IVector_ElementAt(pMe->m_contact_iv,
                             pMe->m_contactSelId);
   }

   if ((ci == NULL) ||
       (pMe->m_fieldInfo.m_name == NULL))
   {
     MSG_ERROR("Invalid input %x %x %x", ci,
               pMe->m_fieldInfo.m_type, pMe->m_fieldInfo.m_name);
     MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
     return;
   }
   //store the
   ret = ContDlgHandler_RUIM_Set(pMe,
                                 ci->m_name,
                                 pMe->m_fieldInfo.m_name);

   if (SUCCESS == ret) {
      ContDlgHandler_ShowMsgBox(pMe,
                                STR_MSGBOX_SAVEDONRUIM_TEXT,
                                STATE_RUIM_RESUME);

   } else {
      MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE);
   }

}
#endif

/*=============================================================================
FUNCTION: HdlrStateExitApplet

DESCRIPTION:
   Called in response to the STATE_EXITAPPLET state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateExitApplet(CContactApp *pMe)
{
   //Close the contact applet
   (void) ISHELL_CloseApplet(pMe->m_pShell,
                             FALSE);
}

/*=============================================================================
FUNCTION: HdlrStateErrorInit

DESCRIPTION:
   Called in response to the STATE_ERROR_INIT state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateErrorInit(CContactApp *pMe)
{
   //show the initializaiton error text and exit appplet
   ContDlgHandler_ShowMsgBox(pMe,
                             STR_MSGBOX_INIT_ERR_TEXT,
                             STATE_EXITAPPLET);

}

/*=============================================================================
FUNCTION: HdlrStateContErrorDlgOpen

DESCRIPTION:
   Called in response to the STATE_CONT_ERROR_DLGOPEN state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateContErrorDlgOpen(CContactApp *pMe)
{
   MOVE_TO_STATE(pMe, STATE_EXITAPPLET);
}

/*=============================================================================
FUNCTION: HdlrStateErrorDatabase

DESCRIPTION:
   Called in response to the STATE_ERROR_DATABASE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateErrorDatabase(CContactApp *pMe)
{
   int last_error = EFAILED;

   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateErrorDatabase: NULL pMe", 0, 0, 0);
      return;
   }

   last_error=IADDRBOOK_GetLastError(pMe->m_addr_book);

   //show the database error message.
   switch (last_error)
   {
     case ENOMEMORY:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_MEMFULL_ERR_TEXT,
                                 STATE_CONT_MAIN);
       break;
     case EITEMBUSY:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_UIMNOTREADY_ERR_TEXT,
                                 STATE_CONT_MAIN);
       break;
     case ENOSUCH:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_NOSIMFILE_ERR_TEXT,
                                 STATE_CONT_MAIN);
       break;
     case EBADSTATE:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_RESTRICTED_ERR_TEXT,
                                 STATE_CONT_MAIN);
       break;
     case ENEEDMORE:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_NEEDMORE_ERR_TEXT,
                                 STATE_CONT_MAIN);
       break;
     default:
       ContDlgHandler_ShowMsgBox(pMe,
                                 STR_MSGBOX_DATABASE_ERR_TEXT,
                                 STATE_CONT_MAIN);
       break;
   }

}

/*=============================================================================
FUNCTION: ContStateMachine_SelFieldInputDlg

DESCRIPTION:
   Selects the dialog based on the type of input field.

PARAMETERS:
   type: type of input field eg. STR_FIELDINPUT_NUMBER, STR_FIELDINPUT_ADDRESS

RETURN VALUE:
   uint16: ID of the dialog to be displayed

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint16 ContStateMachine_SelFieldInputDlg(uint16 type)
{
   //select which dialog should be should
   //depending on field type
   switch (type) {
      case STR_FIELDINPUT_NUMBER:
         return DLG_N_FIELDINPUT;
      case STR_FIELDINPUT_ADDNUMBER:
         return DLG_N_FIELDINPUT;
      case STR_FIELDINPUT_ADDRESS:
         return DLG_A_FIELDINPUT;
      case STR_FIELDINPUT_EMAIL:
         return DLG_E_FIELDINPUT;
      case STR_FIELDINPUT_URL:
         return DLG_U_FIELDINPUT;
      case STR_FIELDINPUT_ID:
         return DLG_G_FIELDINPUT;
      case STR_FIELDINPUT_SECNAME:
         return DLG_SN_FIELDINPUT;
      default:
        MSG_ERROR("Unreachable case reached for input %d", type, 0, 0);
        break;
   }

   return DLG_N_FIELDINPUT;
}

/*=============================================================================
FUNCTION: ContStateMachine_CallFieldNumber

DESCRIPTION:
   Creates the instance of Idle applet and calls the number in the m_fieldinfo.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:
   boolean - TRUE if successful otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ContStateMachine_CallFieldNumber(CContactApp *pMe)
{
   char szNumber[1+FLDLEN_NUMBER];

   (void)WSTRTOSTR(pMe->m_fieldInfo.m_name, szNumber, sizeof(szNumber));

   if (OriginateVoiceCall(szNumber) == SUCCESS)
      return TRUE;

   return FALSE;
}


/*=============================================================================
FUNCTION: CContactApp_CanSaveNumber

DESCRIPTION:
   Checks if the number is allowed to be saved to the contact database.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.
*number [in]: Number that the user has entered

RETURN VALUE:
   boolean - TRUE if the number can be saved.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CContactApp_CanSaveNumber(CContactApp  *pMe,
                                         AECHAR const *number)
{
   boolean ret = TRUE;
#ifdef FEATURE_OTASP
      // TO DO
      // OTASP number are not allowed to be saved to the contact database or
      // R-UIM card!

#endif /* FEATURE_OTASP */

   return ret;
}

#ifdef FEATURE_MMGSDI
/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewEmail

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_EMAIL state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewEmail(CContactApp *pMe)
{
   //show the add new email dialog
   ContDlgHandler_ShowDialog(pMe,
                     ContStateMachine_SelFieldInputDlg(pMe->m_fieldInfo.m_type),
                     STATE_RUIM_ADDNEW_EMAIL_HANDLE);

}


/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewEmailHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_EMAIL_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewEmailHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMAddNewEmailHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //Process the add new email dialog result
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW)
         break;
      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
            WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {

            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_RUIM_ADDNEW_EMAIL);

         } else {
            FREE_STRING(pMe->m_contactInfo.m_name);
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_CONTACT)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_EMAIL)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewName

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_NAME state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewName(CContactApp *pMe)
{
   //show the add new name dialog
   ContDlgHandler_ShowDialog(pMe,
                     ContStateMachine_SelFieldInputDlg(pMe->m_fieldInfo.m_type),
                     STATE_RUIM_ADDNEW_NAME_HANDLE);

}


/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewNameHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_NAME_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewNameHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMAddNewNameHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //Process the add new name dialog result
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW)
         break;
      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
            WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {

            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_RUIM_ADDNEW_NAME);

         } else {
            FREE_STRING(pMe->m_contactInfo.m_name);
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_CONTACT)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_NAME)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;
   }

}
/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewId

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_ID state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewId(CContactApp *pMe)
{
   //show the add new group id dialog
   ContDlgHandler_ShowDialog(pMe,
                     ContStateMachine_SelFieldInputDlg(pMe->m_fieldInfo.m_type),
                     STATE_RUIM_ADDNEW_ID_HANDLE);

}


/*=============================================================================
FUNCTION: HdlrStateRUIMAddNewIdHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_ADDNEW_ID_HANDLE state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMAddNewIdHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMAddNewIdHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //Process the add new group id dialog result
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW)
         break;
      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
            WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {

            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_RUIM_ADDNEW_ID);

         } else {
            FREE_STRING(pMe->m_contactInfo.m_name);
            MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_CONTACT)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_ADDNEW_ID)
         break;

      default:
        MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
        MOVE_TO_STATE(pMe, STATE_CONT_MAIN)
        break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsAddNewNumber

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER state being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsAddNewNumber(CContactApp *pMe)
{
   //show the add new  number dialog.
   //Here we can take the phone number input to store
   ContDlgHandler_ShowDialog(pMe,
                             DLG_N_FIELDINPUT,
                             STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER_HANDLE);
}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsAddNewNumberHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsAddNewNumberHandle(CContactApp *pMe)
{

   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactOptsAddNewNumberHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handles the return result of the contact options- add new number dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER);
         } else {
            if (!CContactApp_CanSaveNumber(pMe, pMe->m_fieldInfo.m_name)) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_MSGBOX_DATABASE_ERR_TEXT,
                                         STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER);
            } else {
               pMe->m_fieldInfo.m_numberType = NUMBERTYPE_RUIM;
               MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_FINISHED_HANDLE)
            }
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER)
         break;

      //If you are middle of editing and pressed send key
      case DLGRET_CONTACT_SENDKEY:
         {
            boolean   ret= FALSE;

            if (NULL == pMe->m_fieldInfo.m_name ||
                WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
               ContDlgHandler_ShowMsgBox(pMe,
                                         STR_FIELDINPUT_NOTEXT_MSG,
                                         STATE_FIELDOPTS_EDIT);
            } else {
               ret=ContStateMachine_CallFieldNumber(pMe);
               if (FALSE == ret) {
                  MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER)
               } else {
                  MOVE_TO_STATE(pMe, STATE_EXITAPPLET)
               }
            }
            break;
         }

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}


/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsAddNewEmail

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_ADDNEW_EMAIL state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsAddNewEmail(CContactApp *pMe)
{
   //show the contact options add new email dialog
   ContDlgHandler_ShowDialog(pMe,
                     ContStateMachine_SelFieldInputDlg(pMe->m_fieldInfo.m_type),
                     STATE_RUIM_CONTACTOPTS_ADDNEW_EMAIL_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsAddNewEmailHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_ADDNEW_EMAIL_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateRUIMContactOptsAddNewEmailHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactOptsAddNewEmailHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the contacat options - add new email dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_RUIM_CONTACTOPTS_ADDNEW_EMAIL);
         } else {
            MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_FINISHED_HANDLE)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_EMAIL)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsAddNewName

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_ADDNEW_NAME state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsAddNewName(CContactApp *pMe)
{
   //show the contact options add new name dialog
   ContDlgHandler_ShowDialog(pMe,
                     ContStateMachine_SelFieldInputDlg(pMe->m_fieldInfo.m_type),
                     STATE_RUIM_CONTACTOPTS_ADDNEW_NAME_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsAddNewNameHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_ADDNEW_NAME_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateRUIMContactOptsAddNewNameHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactOptsAddNewNameHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the contacat options - add new name dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_RUIM_CONTACTOPTS_ADDNEW_NAME);
         } else {
            MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_FINISHED_HANDLE)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_NAME)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsAddNewFinishedHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_ADDNEW_FINISHED_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsAddNewFinishedHandle(CContactApp *pMe)
{
   int retval;

   //save the address field in the address book
   retval = ContDlgHandler_AddField(pMe);

   switch (retval) {
      case AEE_SUCCESS:
         ContDlgHandler_ShowMsgBox(pMe,
                                STR_MSGBOX_ENTRYSAVED_TEXT,
                                STATE_RUIM_SELCONTACT_REFRESH);
         break;
      case ENOMEMORY:
         ContDlgHandler_ShowMsgBox(pMe,
                             STR_MSGBOX_LIMIT_REACHED_ERR_TEXT,
                             STATE_RUIM_SELCONTACT_REFRESH);
         break;
      default:
         MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsAddNewId

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_ADDNEW_ID state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMContactOptsAddNewId(CContactApp *pMe)
{
   //show the contact options add new group id dialog
   ContDlgHandler_ShowDialog(pMe,
                     ContStateMachine_SelFieldInputDlg(pMe->m_fieldInfo.m_type),
                     STATE_RUIM_CONTACTOPTS_ADDNEW_ID_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMContactOptsAddNewIdHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_CONTACTOPTS_ADDNEW_ID_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void HdlrStateRUIMContactOptsAddNewIdHandle(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMContactOptsAddNewIdHandle: NULL pMe", 0, 0, 0);
      return;
   }

   //handler for the contacat options - add new group id dialog
   switch (pMe->m_dlgResult) {
      case DLGRET_CONT_CANCELED:
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW)
         break;

      case DLGRET_CONT_OK:
         if (NULL == pMe->m_fieldInfo.m_name ||
             WSTRLEN(pMe->m_fieldInfo.m_name) <= 0) {
            ContDlgHandler_ShowMsgBox(pMe,
                                      STR_FIELDINPUT_NOTEXT_MSG,
                                      STATE_RUIM_CONTACTOPTS_ADDNEW_ID);
         } else {
            MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_FINISHED_HANDLE)
         }
         break;

      case DLGRET_CONT_UNHANDLED: // Restart the dialog
         MOVE_TO_STATE(pMe, STATE_RUIM_CONTACTOPTS_ADDNEW_ID)
         break;

      default:
         MSG_ERROR("Unreachable case reached for input %d", pMe->m_dlgResult, 0, 0);
         MOVE_TO_STATE(pMe, STATE_CONT_MAIN);
         break;
   }

}

/*=============================================================================
FUNCTION: HdlrStateRUIMFieldOptsDelete

DESCRIPTION:
   Called in response to the STATE_RUIM_FIELDOPTS_DELETE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMFieldOptsDelete(CContactApp *pMe)
{
   if (pMe == NULL)
   {
      MSG_ERROR("HdlrStateRUIMFieldOptsDelete: NULL pMe", 0, 0, 0);
      return;
   }

   //show the field options delete dialog.
   pMe->m_yesnoTimeout = FALSE;
   if (STR_FIELDINPUT_NUMBER == pMe->m_fieldInfo.m_type) {
      pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_DELETENUMBER;
   } else if (STR_FIELDINPUT_ADDNUMBER == pMe->m_fieldInfo.m_type) {
      pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_DELETEADDNUMBER;
   } else if (STR_FIELDINPUT_EMAIL == pMe->m_fieldInfo.m_type) {
      pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_DELETEEMAIL;
   } else if (STR_FIELDINPUT_SECNAME == pMe->m_fieldInfo.m_type) {
      pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_DELETESECNAME;
   } else if (STR_FIELDINPUT_ID == pMe->m_fieldInfo.m_type) {
      pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_DELETEID;
   } else if (STR_FIELDINPUT_ADDRESS == pMe->m_fieldInfo.m_type) {
      pMe->m_yesnoDialogTitle = STR_YESNO_MENUTITLE_DELETEADDRESS;
   }

   ContDlgHandler_ShowDialog(pMe,
                             DLG_YESNO,
                             STATE_RUIM_FIELDOPTS_DELETE_HANDLE);

}

/*=============================================================================
FUNCTION: HdlrStateRUIMFieldOptsDeleteHandle

DESCRIPTION:
   Called in response to the STATE_RUIM_FIELDOPTS_DELETE_HANDLE state
   being set.

PARAMETERS:
   *pMe [in]: Pointer to the Contact Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void HdlrStateRUIMFieldOptsDeleteHandle(CContactApp *pMe)
{
   //handler for the return result of field options - delete dialog
   if (DLGRET_CONT_YES == pMe->m_dlgResult) {
      if (TRUE == ContDlgHandler_DeleteField(pMe)) {

        ContDlgHandler_ShowMsgBox(pMe,
                                  STR_MSGBOX_ENTRYDELETED_TEXT,
                                  STATE_RUIM_SELCONTACT_REFRESH);
      } else {
         MOVE_TO_STATE(pMe, STATE_ERROR_DATABASE)
      }

   } else {
      MOVE_TO_STATE(pMe, STATE_FIELDOPTS)
   }

}
#endif //#ifdef FEATURE_MMGSDI

//lint -restore

#endif /*FEATURE_APP_CONTACT*/
