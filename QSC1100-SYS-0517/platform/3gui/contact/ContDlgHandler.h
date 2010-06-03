#ifndef __CONTDLGHANDLER_H__
#define __CONTDLGHANDLER_H__

/*=============================================================================
  FILE: ContDlgHandler.h

  SERVICES: Contact Applet Dialog Handlers

  GENERAL DESCRIPTION:
      Declaration of all contact applet dialog handlers which are used
      by the IContactApp to bring up various Contact Applet screens
      to the user

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright © 2002 - 2003 QUALCOMM Incorporated. All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=============================================================================
  $Header: //depot/asic/msmshared/apps/Contact/ContDlgHandler.h#13 $
  $Author: aruns $
  $Change: 218254 $
                    EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------
07/14/05     aps     Support to handle contacts with name field empty.
01/17/05     BW      Added USIM support - ability to add and view UIM fields
11/10/04     BW      Added support for SDN
07/22/04     PA      Added new menu for saving existing contact to UIM2
06/23/04     PA      Support for enumeration of available contacts
04/07/04     PA      Check maximum number of fields allowed for a contact and
                     select an alternate number type if required. 
08/07/03     jds     Added support for editing FDN.
06/03/03     PA      Changed FEATURE_RUIM_PHONEBOOK to FEATURE_MMGSDI
05/15/03     PA      Support for retraining voice tags
05/08/03     PA      Support for Voice Tags
02/06/03     kar     Code cleanup
11/15/02     pm      Port Contact App to QCT
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES FOR MODULE
===========================================================================*/
#include "AEEAddrBookExt.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/
////
// MenuId for the 'Options' item
#define OPTIONS_MENU_ID 0xFFFF

//ID for the search on RUIM category
#define STR_CONTACTLIST_RUIM_ADN 0x8000

// NOTE: These number types are an addition to the
// ones in the DLG_NUMBERTYPE dialog. We need to
// make sure that they do not overlap
//Number type for the phone numbers on RUIM
//Will be used to show the RUIM icon
#define NUMBERTYPE_RUIM       0x8000

#define NUMBERTYPE_SPEEDDIAL  0x8001

/* The number is a Speed Dial entry with a Voice Tag ID */
#define NUMBERTYPE_VOICETAG   0x8002

/* Number type for numbers written by Softkey UI (or anybody) which
   does not belong to any type that Contact App knows of */
#define NUMBERTYPE_OTHER      0x8003


//ID for MAX NUMBERS in the speed dial 2 to 99
//index 1 is reserved for voice mail number
#define MAX_SPEED_DIAL_NUMS 98

//Speed dial slot empty indicator. Intially all the
//slots are marked empty and we mark the slot the with th
//record ids it is alloted
#define SPEED_DIAL_EMPTY    (-1)

/*===========================================================================
                               MACRO DEFINITIONS
===========================================================================*/

#define FREE_STRING(pStr)   \
   if ((pStr) != NULL) {    \
      FREE(pStr);           \
      (pStr) = NULL;        \
   }
#define FREE_CONTACTINFO(ci)    \
   FREE_STRING( (ci).m_name )

#define FREE_FIELDINFO(fi)     \
   FREE_STRING( (fi).m_name )

#define FREE_AEEAddrField(cf)      \
   if ((cf).pBuffer != NULL) { \
      FREE((cf).pBuffer); \
      (cf).pBuffer = NULL; \
   }

/*===========================================================================
                              TYPE DECLARATIONs
===========================================================================*/
//Avoid including the whole IContactApp.h header file
struct _CContactApp;

typedef boolean (*CONT_DIALOG_EVENTHANDLER)(struct _CContactApp *pMe,
                                            AEEEvent        eCode,
                                            uint16          wParam,
                                            uint32          dwParam);

////
// List of values that a dialog may return
//
typedef enum _DlgResultEnum {
   DLGRET_CONT_UNHANDLED = -1,    // The default 'unhandled' response,
                             // this is not necessarily an error as this may
                             // occur when the applet is suspended and then
                             // resumed later

   DLGRET_CONT_CANCELED = 0,      // General dialog response
   DLGRET_CONT_OK,                // General dialog response
   DLGRET_CONT_NO,                // Returned by YESNO dialog
   DLGRET_CONT_YES,               // Returned by YESNO dialog

   ////
   // DLG_MAIN specific return values
   DLGRET_MAIN_FINDNAME,
   DLGRET_MAIN_ALLCONTACTS,
   DLGRET_MAIN_BIZCONTACTS,
   DLGRET_MAIN_PERCONTACTS,
   DLGRET_MAIN_ADDNEW,
#ifdef FEATURE_MMGSDI
   DLGRET_MAIN_ADDTORUIM,
   DLGRET_CONT_MAIN_RUIM_ADN,
   DLGRET_CONT_MAIN_RUIM_FDN,
   DLGRET_MAIN_ADDTOFDN,
   DLGRET_CONT_MAIN_RUIM_SDN,
#endif /* FEATURE_MMGSDI */
   DLGRET_MAIN_SPEEDDIAL,

   ////
   //  DLG_CONTACTLIST specific return values
   DLGRET_CONTACTLIST_SENDKEY,

   ////
   // DLG_CONTACT specific return values
   DLGRET_CONTACT_OPTS,
   DLGRET_CONTACT_FIELDSEL,
   DLGRET_CONTACT_SENDKEY,

   ////
   // DLG_CONTACTOPTIONS specific return values
   DLGRET_CONTACTOPTS_ADDNEW,
   DLGRET_CONTACTOPTS_EDITNAME,
   DLGRET_CONTACTOPTS_EDITCLASS,
   DLGRET_CONTACTOPTS_DELETE,

   ////
   // DLG_FIELDOPTIONS specific return values
   DLGRET_FIELDOPTS_VIEWADDR,
   DLGRET_FIELDOPTS_NUMBERTYPE,
   DLGRET_FIELDOPTS_EDIT,
   DLGRET_FIELDOPTS_ADDTOSPEEDDIAL,
   DLGRET_FIELDOPTS_ADDVOICETAG,
   DLGRET_FIELDOPTS_RETRAIN,
#ifdef FEATURE_MMGSDI
   DLGRET_FIELDOPTS_PHONE,
   DLGRET_FIELDOPTS_RUIM,
#ifdef FEATURE_MMGSDI_DUAL_SLOT
   DLGRET_FIELDOPTS_RUIM2,
#endif
#endif
   DLGRET_FIELDOPTS_DELETE,

   DLGRET_VR_DONE,
   DLGRET_VR_FAILED,
   DLGRET_VR_ABORTED

#ifdef FEATURE_MMGSDI
   ,
   ////
   // DLG_RUIM return values
   DLGRET_RUIM_ASSIGN,
   DLGRET_RUIM_REMOVE,
   DLGRET_RUIM_ADDNEW,
   DLGRET_RUIM_CONTACTOPTS,
   DLGRET_RUIM_CONTACTLIST_SENDKEY,
   DLGRET_RUIM_EMPTY_ADDNEW

#endif

}DlgResultEnum;

typedef enum _ContEvent {
   CONT_EVENT_DIALOG_TIMEOUT,
   CONT_EVENT_REDRAW,
   CONT_EVENT_VR_DONE,
   CONT_EVENT_VR_RUNNING,
   CONT_EVENT_VR_ABORTED,
   CONT_EVENT_VR_FAILED,
   CONT_EVENT_RUIM_EMPTY
} ContEventType;

/*===========================================================================
                              FUNCTION DECLARATIONs
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
void ContDlgHandler_UpdateActiveDialogInfo(struct _CContactApp *pMe,
                                           uint16               wParam,
                                           uint32               dwParam);

/*=============================================================================
FUNCTION: ContDlgHandler_ShowDialog

DESCRIPTION:
   Creates the dialog with dlgResID

PARAMETERS:
   *pMe: ContactAppDialogHandler Object pointer
   dlgResId: ID of the dialog to be shown
   retState: return State. SUCCESS if successful else EFAILED/ENOMEMORY

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ContDlgHandler_ShowDialog(struct _CContactApp *pMe,
                               uint16               dlgResId,
                               ContactAppState retState);

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
void ContDlgHandler_ShowMsgBox(struct _CContactApp *pMe,
                               uint16               msgTextResId,
                               ContactAppState retState);

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
boolean ContDlgHandler_RouteDialogEvent(struct _CContactApp *pMe,
                                        AEEEvent             eCode,
                                        uint16               wParam,
                                        uint32               dwParam);

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
int ContDlgHandler_GetNumFields(IAddrRec *rec, AEEAddrFieldID fID);

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
int ContDlgHandler_AddField(struct _CContactApp *pMe);


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
boolean ContDlgHandler_SaveField(struct _CContactApp *pMe);

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
boolean ContDlgHandler_DeleteField(struct _CContactApp *pMe);

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
boolean ContDlgHandler_CreateContact(struct _CContactApp *pMe);

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
boolean ContDlgHandler_SaveContactInfo(struct _CContactApp *pMe);

/*=============================================================================
FUNCTION: ContDlgHandler_DeleteContact

DESCRIPTION:
   Delete the current contact (pMe->m_contactSelId)

PARAMETERS:
   *pMe: pointer to CContactApp object

RETURN VALUE:
   boolean: TRUE if successful otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ContDlgHandler_DeleteContact(struct _CContactApp *pMe);


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
boolean ContDlgHandler_LoadAvailContactList(struct _CContactApp *pMe);

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
AECHAR * ContDlgHandler_GetContactName(struct _CContactApp *pMe, IAddrRec *addrRec);

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
boolean ContDlgHandler_LoadContactList(struct _CContactApp *pMe);

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
boolean ContDlgHandler_LoadSpeedDialList(struct _CContactApp *pMe);

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
struct _FieldInfo *ContDlgHandler_AEEAddrField2FieldInfo(
                                                    struct _CContactApp *pMe,
                                                    AEEAddrField *cf);

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
boolean ContDlgHandler_LoadAEEAddrFields(struct _CContactApp *pMe);

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
void ContDlgHandler_ReleaseCDBRec(struct _CContactApp *pMe);
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
void ContDlgHandler_FreeAEEAddrFields(struct _CContactApp *pMe);

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
void ContDlgHandler_FreeContactList(struct _CContactApp *pMe,
                                    IVector  *pIV);


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
int ContDlgHandler_RUIM_Clear(struct _CContactApp *pMe);

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
int ContDlgHandler_RUIM_Set(struct _CContactApp *pMe,
                            AECHAR *name,
                            AECHAR *phone);


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
int ContDlgHandler_RUIM_Get(struct _CContactApp *pMe,
                            AECHAR *name,
                            AECHAR *phone);

#endif /*FEATURE_MMGSDI*/
/*=============================================================================
FUNCTION: ContDlgHandler_SpeedDial_Set

DESCRIPTION:
   Sets the name,phone number and index for the speed dial on the Database

PARAMETERS:
   pMe: pointer to Contact applet
   name: name of the contact to store
   phone: phone number of the contact
   wSlotID : slot index range 2 to 99

RETURN VALUE:
   SUCCESS: If it successfully added to the speed dial list
   EFAILED: If not successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int ContDlgHandler_SpeedDial_Set(struct _CContactApp *pMe,
                                 AECHAR *name,
                                 AECHAR *phone,
                                 uint16  wSlotID,
                                 boolean voiceTag);


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
int ContDlgHandler_VoiceTag_Create(struct _CContactApp *pMe);

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
int ContDlgHandler_VoiceTag_Remove(struct _CContactApp *pMe, 
                                                    AEEVRUtterance wRWId);

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
int ContDlgHandler_RemoveRec_VoiceTag(struct _CContactApp *pMe, 
                                                      IAddrRec *pAddrRec);
#endif /* FEATURE_PHONE_VR */

#endif // end __CONTDLGHANDLER_H__

