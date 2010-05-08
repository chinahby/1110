
#ifndef __CONTACTAPPSTATEMACHINE_H__
#define __CONTACTAPPSTATEMACHINE_H__

/*=============================================================================
  FILE: ContStateMachine.h

  SERVICES: Contact Applet State Machine Handlers
 
  GENERAL DESCRIPTION:
      This file defines all Contact Applet state mahine handlers which are used
      by the Contact applet to bring up various contacts screens to the user. 
  
  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright © 2002 - 2003 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=============================================================================
  $Header: //depot/asic/msmshared/apps/Contact/ContStateMachine.h#9 $
  $Author: johns $
  $Change: 175442 $
                    EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------
03/16/05     JAS     Fixing featurization of state defns to match table
01/17/05     BW      Added USIM support - ability to add and view UIM fields
06/23/04     PA      Support for enumeration of available contacts
08/07/03     jds     Added support for editing FDN.
06/03/03     PA      Changed FEATURE_RUIM_PHONEBOOK to FEATURE_MMGSDI
05/08/03     PA      Support for Voice Tags
02/06/03     kar     Code cleanup
11/15/02     pm      Port Contact App to QCT
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES FOR MODULE
===========================================================================*/

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

////
// List of states that the applet can be in.  The main applet logic
// is controled by a finite state machine.
//
typedef enum _ContactAppState {
   STATE_CONT_NULL = 0,

   STATE_CONT_INIT,

   STATE_CONT_MAIN,
   STATE_MAIN_HANDLE,

   STATE_FINDNAME,
   STATE_FINDNAME_HANDLE,

   STATE_ADDNEW,
   STATE_ADDNEW_HANDLE,
   STATE_ADDNEW_NUMBER,
   STATE_ADDNEW_NUMBER_HANDLE,
   STATE_ADDNEW_ADDRESS,
   STATE_ADDNEW_ADDRESS_HANDLE,
   STATE_ADDNEW_CONTACT,
   STATE_ADDNEW_CONTACT_HANDLE,
   STATE_ADDNEW_FINISHED,
#ifdef FEATURE_MMGSDI
   STATE_ADDNEW_SAVEONRUIM,
   STATE_PIN2,
   STATE_PIN2_HANDLE,
#endif

   STATE_CONTACTLIST,
   STATE_CONTACTLIST_RELOAD,
   STATE_CONTACTLIST_AVAIL_RELOAD,
   STATE_CONTACTLIST_RESUME,
   STATE_CONTACTLIST_ERROR,
   STATE_CONTACTLIST_HANDLE,

   STATE_CONTACT,
   STATE_CONTACT_REFRESH,
   STATE_CONTACT_RESUME,
   STATE_CONTACT_HANDLE,

   STATE_CONTACTOPTS,
   STATE_CONTACTOPTS_RESUME,
   STATE_CONTACTOPTS_HANDLE,

   STATE_CONTACTOPTS_ADDNEW,
   STATE_CONTACTOPTS_ADDNEW_HANDLE,
   STATE_CONTACTOPTS_ADDNEW_NUMBER,
   STATE_CONTACTOPTS_ADDNEW_NUMBER_HANDLE,
   STATE_CONTACTOPTS_ADDNEW_FINISHED_NUMBER,
   STATE_CONTACTOPTS_ADDNEW_ADDRESS,
   STATE_CONTACTOPTS_ADDNEW_ADDRESS_HANDLE,
   STATE_CONTACTOPTS_ADDNEW_FINISHED_ADDR,
#ifdef FEATURE_MMGSDI
   STATE_CONTACTOPTS_ADDNEW_SAVEONRUIM,
#endif

   STATE_CONTACTOPTS_EDITNAME,
   STATE_CONTACTOPTS_EDITNAME_HANDLE,
   STATE_CONTACTOPTS_EDITCLASS,
   STATE_CONTACTOPTS_EDITCLASS_HANDLE,
   STATE_CONTACTOPTS_DELETE,
   STATE_CONTACTOPTS_DELETE_HANDLE,

   STATE_FIELDOPTS,
   STATE_FIELDOPTS_HANDLE,
   STATE_FIELDOPTS_NUMBERTYPE,
   STATE_FIELDOPTS_NUMBERTYPE_HANDLE,
   STATE_FIELDOPTS_EDIT,
   STATE_FIELDOPTS_EDIT_HANDLE,
   STATE_FIELDOPTS_VIEWADDR,
   STATE_FIELDOPTS_DELETE,
   STATE_FIELDOPTS_DELETE_HANDLE,

   //Speed Dial states 
   STATE_SPEEDDIAL_SELSLOT,
   STATE_SPEEDDIAL_SELSLOT_RESUME,
   STATE_SPEEDDIAL_SELSLOT_HANDLE,
   STATE_SPEEDDIAL_OVERWRITE,
   STATE_SPEEDDIAL_OVERWRITE_HANDLE,

#ifdef FEATURE_PHONE_VR
   // Voice Tag states
   STATE_VOICETAG_ADD,
   STATE_VOICETAG_ADD_HANDLE,
   STATE_VOICETAG_TRAIN_INTRO,
   STATE_VOICETAG_TRAIN,
   STATE_VOICETAG_TRAIN_HANDLE,
   STATE_VOICETAG_SAVE,
#endif /* FEATURE_PHONE_VR */

   STATE_SAVENUMBER,
   STATE_SAVENUMBER_NUMBER,
   STATE_SAVENUMBER_NUMBER_HANDLE,
   STATE_SAVENUMBER_NEWOREXISTING,
   STATE_SAVENUMBER_NEWCONTACT,
   STATE_SAVENUMBER_NEWCONTACT_HANDLE,
   STATE_SAVENUMBER_FINISH_NEW,

   STATE_SAVENUMBER_SELCONTACT,
   STATE_SAVENUMBER_SELCONTACT_HANDLE,
   STATE_SAVENUMBER_FINISH_EXISTING,
   STATE_SAVENUMBER_VIEWCONTACT,
   STATE_SELECTFIELD_HANDLE,


#ifdef FEATURE_MMGSDI
   STATE_SAVENUMBER_FINISH_SAVEONRUIM,
   STATE_CONT_RUIM,
   STATE_RUIM_RESUME,
   STATE_RUIM_HANDLE,
   STATE_RUIM_SELCONTACT_REFRESH,
   STATE_RUIM_SELCONTACT_RESUME,
   STATE_RUIM_SELCONTACT_HANDLE,
   STATE_RUIM_CONTACTOPTS,
   STATE_RUIM_CONTACTOPTS_RESUME,
   STATE_RUIM_CONTACTOPTS_HANDLE,
   STATE_RUIM_CONTACTOPTS_EDITNAME,
   STATE_RUIM_CONTACTOPTS_EDITNAME_HANDLE,
   STATE_RUIM_CONTACTOPTS_ADDNEW,
   STATE_RUIM_CONTACTOPTS_ADDNEW_HANDLE,
   STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER,
   STATE_RUIM_CONTACTOPTS_ADDNEW_NUMBER_HANDLE,
   STATE_RUIM_CONTACTOPTS_ADDNEW_EMAIL,
   STATE_RUIM_CONTACTOPTS_ADDNEW_EMAIL_HANDLE,
   STATE_RUIM_CONTACTOPTS_ADDNEW_NAME,
   STATE_RUIM_CONTACTOPTS_ADDNEW_NAME_HANDLE,
   STATE_RUIM_CONTACTOPTS_ADDNEW_ID,
   STATE_RUIM_CONTACTOPTS_ADDNEW_ID_HANDLE,
   STATE_RUIM_CONTACTOPTS_ADDNEW_FINISHED_HANDLE,
   STATE_RUIM_FIELDOPTS_DELETE,
   STATE_RUIM_FIELDOPTS_DELETE_HANDLE,
   STATE_RUIM_ADDNEW,
   STATE_RUIM_ADDNEW_HANDLE,
   STATE_RUIM_ADDNEW_NUMBER,
   STATE_RUIM_ADDNEW_NUMBER_HANDLE,
   STATE_RUIM_ADDNEW_ADDNUMBER,
   STATE_RUIM_ADDNEW_ADDNUMBER_HANDLE,
   STATE_RUIM_ADDNEW_CONTACT,
   STATE_RUIM_ADDNEW_CONTACT_HANDLE,
   STATE_RUIM_ADDNEW_ID,
   STATE_RUIM_ADDNEW_ID_HANDLE,
   STATE_RUIM_ADDNEW_EMAIL,       
   STATE_RUIM_ADDNEW_EMAIL_HANDLE,
   STATE_RUIM_ADDNEW_NAME,       
   STATE_RUIM_ADDNEW_NAME_HANDLE,
   STATE_RUIM_FIELDOPTS,
   STATE_RUIM_FIELDOPTS_HANDLE,
   STATE_RUIM_FIELDOPTS_EDIT,
   STATE_RUIM_FIELDOPTS_EDIT_HANDLE,
   STATE_RUIM_DELETE,
   STATE_RUIM_DELETE_HANDLE,
   STATE_RUIM_CONTACTLIST,
   STATE_RUIM_CONTACTLIST_RELOAD,
   STATE_RUIM_CONTACTLIST_RESUME,
   STATE_RUIM_CONTACTLIST_ERROR,
   STATE_RUIM_CONTACTLIST_HANDLE,

   STATE_RUIM_CONTACT,
   STATE_RUIM_CONTACT_REFRESH,
   STATE_RUIM_CONTACT_RESUME,
   STATE_RUIM_CONTACT_HANDLE,
   STATE_RUIM_CONTACT_NOTANUMBER,

   STATE_RUIM_ASSIGN,

#endif
   
   STATE_EXITAPPLET,

   // Error during applet initialization
   STATE_ERROR_INIT,

   // This state is entered when a dialog fails to open
   STATE_CONT_ERROR_DLGOPEN,

   // This state is entered when saving to the database fails
   STATE_ERROR_DATABASE
} ContactAppState;


/*===========================================================================
                              TYPE DECLARATIONs
===========================================================================*/
//Avoid including the whole IContactApp.h header file
struct _CContactApp;
/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: ContactAppStateMachine_ProcessState

DESCRIPTION: Process a single FSM state

PARAMETERS:
   *pMe: CContactApp Object pointer

RETURN VALUE:
void: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ContStateMachine_ProcessState(struct _CContactApp *pMe);


#endif // end __CONTACTAPPSTATEMACHINE_H__

