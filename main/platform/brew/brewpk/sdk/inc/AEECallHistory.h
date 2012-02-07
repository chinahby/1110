#ifndef AEECALLHISTORY_H
#define AEECALLHISTORY_H
/*======================================================
FILE:  AEECallHistory.h

SERVICES:  BREW Call History Interface

GENERAL DESCRIPTION:
   Interface definitions, data structures, etc. for BREW Call History 
     Interface

Copyright © 1999,2002-2004,2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

/*--- Include Files ---*/
#include "AEE.h"
#include "AEEError.h"

/*--- Constant Definitions ---*/

// Pre-defined Call types
#define  AEECALLHISTORY_CALL_TYPE_TO                1
#define  AEECALLHISTORY_CALL_TYPE_FROM              2
#define  AEECALLHISTORY_CALL_TYPE_MISSED            3
#ifdef CUST_EDITION	
#define  AEECALLHISTORY_CALL_TYPE_ALL                4
#endif /*CUST_EDITION*/
// Pre-defined Number types
#define  AEECALLHISTORY_NUMBER_TYPE_INTERNATIONAL   1
#define  AEECALLHISTORY_NUMBER_TYPE_NATIONAL        2
#define  AEECALLHISTORY_NUMBER_TYPE_NETWORK         3
#define  AEECALLHISTORY_NUMBER_TYPE_SUBSCRIBER      4
#define  AEECALLHISTORY_NUMBER_TYPE_ABBREVIATED     5
#define  AEECALLHISTORY_NUMBER_TYPE_QCHAT           6

// Pre-defined Number plans
#define  AEECALLHISTORY_NUMBER_PLAN_TELEPHONY       1
#define  AEECALLHISTORY_NUMBER_PLAN_DATA            2
#define  AEECALLHISTORY_NUMBER_PLAN_TELEX           3
#define  AEECALLHISTORY_NUMBER_PLAN_PRIVATE         4

// Pre-defined Technology types
#define  AEECALLHISTORY_TECH_TYPE_VOICE             0
#define  AEECALLHISTORY_TECH_TYPE_VIDEO             1

// Pre-defined Call History Entry Field IDs. OEMs can add more entries to it. 
#define  AEECALLHISTORY_FIELD_NONE               (0)
#define  AEECALLHISTORY_FIELD_CALL_TYPE          (AEECALLHISTORY_FIELD_NONE + 1)
#define  AEECALLHISTORY_FIELD_NUMBER_TYPE        (AEECALLHISTORY_FIELD_NONE + 2)
#define  AEECALLHISTORY_FIELD_NUMBER_PLAN        (AEECALLHISTORY_FIELD_NONE + 3)
#define  AEECALLHISTORY_FIELD_DATE_TIME          (AEECALLHISTORY_FIELD_NONE + 4)
#define  AEECALLHISTORY_FIELD_CALL_DURATION      (AEECALLHISTORY_FIELD_NONE + 5)
#define  AEECALLHISTORY_FIELD_NUMBER             (AEECALLHISTORY_FIELD_NONE + 6)
#define  AEECALLHISTORY_FIELD_NAME               (AEECALLHISTORY_FIELD_NONE + 7)
#define  AEECALLHISTORY_FIELD_HANDLER_CLSID      (AEECALLHISTORY_FIELD_NONE + 8)
#define  AEECALLHISTORY_FIELD_NUMBER_EX          (AEECALLHISTORY_FIELD_NONE + 9)
#define  AEECALLHISTORY_FIELD_PI                 (AEECALLHISTORY_FIELD_NONE + 10)
#define  AEECALLHISTORY_FIELD_ID                 (AEECALLHISTORY_FIELD_NONE + 11)
#define  AEECALLHISTORY_FIELD_CALLACTIVE         (AEECALLHISTORY_FIELD_NONE + 12)
#define  AEECALLHISTORY_FIELD_DURATIONKNOWN      (AEECALLHISTORY_FIELD_NONE + 13)
#define  AEECALLHISTORY_FIELD_NOTVIEWED          (AEECALLHISTORY_FIELD_NONE + 14) 
#define  AEECALLHISTORY_FIELD_TECH_TYPE          (AEECALLHISTORY_FIELD_NONE + 15)
#ifdef CUST_EDITION	
#define  AEECALLHISTORY_FIELD_COUNTER            (AEECALLHISTORY_FIELD_NONE + 16)

// ×Ö¶ÎID×î´ó±àºÅ
#define  AEECALLHISTORY_FIELD_MAX                AEECALLHISTORY_FIELD_COUNTER
#endif /*CUST_EDITION*/
// Define PTT call types for AEECALLHISTORY_FIELD_PTT_CALL_TYPE field
#define AEECALLHISTORY_PTT_CALL_TYPE_NONE          0
#define AEECALLHISTORY_PTT_CALL_TYPE_DIRECT        1
#define AEECALLHISTORY_PTT_CALL_TYPE_ALERT         2
#define AEECALLHISTORY_PTT_CALL_TYPE_GROUP         3

// Define PTT call subtypes for AEECALLHISTORY_FIELD_PTT_CALL_TYPE field.
// The AEECALLHISTORY_FIELD_PTT_CALL_TYPE is two bytes long. The lsb
// contains one of the values above and the msb contains one of the 
// subtype values listed below.
// (i.e. a restricted group call would have a call type of 0x0203)
#define AEECALLHISTORY_PTT_CALL_SUBTYPE_NONE       0
#define AEECALLHISTORY_PTT_CALL_SUBTYPE_FAILED     1
#define AEECALLHISTORY_PTT_CALL_SUBTYPE_RESTRICTED 2

// First OEM-defined field name
#define AEECALLHISTORY_FIELD_OEM (AEECALLHISTORY_FIELD_NONE + 0x100)

// Associated type is uint16
#define AEECALLHISTORY_FIELD_PTT_CALL_TYPE         (AEECALLHISTORY_FIELD_OEM + 0)

// Associated data type is null-terminated comma-separated AECHAR* list
#define AEECALLHISTORY_FIELD_USER_ADDRESS          (AEECALLHISTORY_FIELD_OEM + 1)

// Associated data type is uint8[]
#define AEECALLHISTORY_FIELD_CONFERENCE_ID         (AEECALLHISTORY_FIELD_OEM + 2)

// Associated data type is uint8
#define AEECALLHISTORY_FIELD_ALERT_MSG_INDEX       (AEECALLHISTORY_FIELD_OEM + 3)

// Associated data type is null-terminated AECHAR*
#define AEECALLHISTORY_FIELD_ALERT_MSG_TEXT        (AEECALLHISTORY_FIELD_OEM + 4)

// Associated data type is uint8[]
#define AEECALLHISTORY_FIELD_ALT_CONFERENCE_ID     (AEECALLHISTORY_FIELD_OEM + 5)

// Reserver fields. Associated type will be defined in corresponding header file for reserved fields.
#define AEECALLHISTORY_FIELD_BROWSER_1             (AEECALLHISTORY_FIELD_OEM + 6)
#define AEECALLHISTORY_FIELD_BROWSER_2             (AEECALLHISTORY_FIELD_OEM + 7)
#define AEECALLHISTORY_FIELD_BROWSER_3             (AEECALLHISTORY_FIELD_OEM + 8)
#define AEECALLHISTORY_FIELD_BROWSER_4             (AEECALLHISTORY_FIELD_OEM + 9)
#ifdef CUST_EDITION	
// define max number of items
// Max number of digits in a call history entry 
#define AEECALLHISTORY_MAXDIGITS             (83+2)//(33+2)  //two bytes for number count  //modify by pyuangui
// Max number of letters of an call history entry 
// (including the NULL byte)
#define AEECALLHISTORY_MAXLETTERS           (33)

#define AEECALLHISTORY_MAX_CALLS            (60)
//end added
#endif /*CUST_EDITION*/
/*--- Data Types ---*/

/* Definition of Call History entry Field. An entry in the address book is 
   a collection of one or more fields */
typedef struct AEECallHistoryField
{
   AEECLSID    ClsId;      // ClsId of app defining wID
   uint16      wID;        // Field ID (ex AEECALLHISTORY_FIELD_NAME)
   uint16      wDataLen;   // Data Length
   void*       pData;      // Data (form and length varies according to 
                           // ClsId and wID)
} AEECallHistoryField;

typedef struct AEECallHistoryEntry
{
   AEECallHistoryField *pFields;
   uint16               wNumFields;
} AEECallHistoryEntry;

typedef struct ICallHistory ICallHistory;

/*
   Call History  Interface
*/

AEEINTERFACE(ICallHistory)
{
   INHERIT_IQueryInterface(ICallHistory);

   int (*AddEntry)(ICallHistory *pich, const AEECallHistoryEntry *pche);
   int (*EnumInit)(ICallHistory *pich);
   const AEECallHistoryEntry *(*EnumNext)(ICallHistory *pich, int *pnErr);
   int (*UpdateEntry)(ICallHistory *pich, const AEECallHistoryEntry *pche);
   int (*Clear)(ICallHistory *pich);
   int (*EnumInitByCallType)(ICallHistory *pich, uint16 type);
   int (*ClearEntry)(ICallHistory *pich);
   int (*Notify)(ICallHistory *pich, AEECallback *pcb);
#ifdef CUST_EDITION	   
   int (*GetRecCountByType)(ICallHistory *pich, uint16 type);
   AECHAR *(*GetLastRecNumByType)(ICallHistory *pich, uint16 type);
#endif /*CUST_EDITION*/   
};

#define ICALLHISTORY_AddRef(p)                   AEEGETPVTBL((p),ICallHistory)->AddRef((p))
#define ICALLHISTORY_Release(p)                  AEEGETPVTBL((p),ICallHistory)->Release((p))
#define ICALLHISTORY_QueryInterface(p,i,o)       AEEGETPVTBL((p),ICallHistory)->QueryInterface((p),(i),(o))
#define ICALLHISTORY_AddEntry(p, e)              AEEGETPVTBL((p),ICallHistory)->AddEntry((p),(e))
#define ICALLHISTORY_EnumInit(p)                 AEEGETPVTBL((p),ICallHistory)->EnumInit((p))
#define ICALLHISTORY_EnumInitByCallType(p, t)    AEEGETPVTBL((p),ICallHistory)->EnumInitByCallType((p),(t))
#define ICALLHISTORY_EnumNext(p, r)              AEEGETPVTBL((p),ICallHistory)->EnumNext((p),(r))
#define ICALLHISTORY_UpdateEntry(p, e)           AEEGETPVTBL((p),ICallHistory)->UpdateEntry((p),(e))
#define ICALLHISTORY_Clear(p)                    AEEGETPVTBL((p),ICallHistory)->Clear((p))
#define ICALLHISTORY_ClearEntry(p)               AEEGETPVTBL((p),ICallHistory)->ClearEntry((p))
#define ICALLHISTORY_Notify(p,c)                 AEEGETPVTBL((p),ICallHistory)->Notify((p),(c))
#ifdef CUST_EDITION	
#define ICALLHISTORY_GetRecCountByType(p, t)    AEEGETPVTBL((p),ICallHistory)->GetRecCountByType((p), (t))
#define ICALLHISTORY_GetLastRecNumByType(p, t)    AEEGETPVTBL((p),ICallHistory)->GetLastRecNumByType((p), (t))
#endif /*CUST_EDITION*/
// This is for compilation backwards compatability with older versions of
// this file with a misspelling.
#define AEECALLHISTORY_NUMBER_TYPE_ABREVIATED AEECALLHISTORY_NUMBER_TYPE_ABBREVIATED

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

=======================================================================
AEECallHistoryField

Description:
 This struct contains the definition of each Call History entry Field. 
 A Call History entry is a collection of one or more of these Fields.

Definition:

typedef struct AEECallHistoryField
{
   AEECLSID   ClsId
   uint16     wID;      
   uint16     wDataLen; 
   void      *pData;     

} AEECallHistoryField;

Members:

   ClsId:     Class ID associated with the field ID.  For predefined fields 
              (those defined in AEECallHistory.h), use a ClsID of 0. 
   wID:       Field ID (ex AEECALLHISTORY_FIELD_NAME).  For user-defined fields,
              this value is left up to the developer.
   wDataLen:  Data Length
   pData:     Data (form and length varies according to wID and ClsId)

Comments:

See Also:
  AEECallHistoryEntry

=======================================================================
AEECallHistoryEntry

Description:
   This struct contains the definition of each Call History entry Field. 
   A Call History entry is a collection of one or more of these Fields.

Definition:

typedef struct AEECallHistoryEntry
{
   AEECallHistoryField *pFields;
   uint16               wNumFields;
} AEECallHistoryEntry;


Members: 
   pFields:     the array of fields
   wNumFields:  number of fields in the array

Comments:
   None

See Also:
   AEECallHistoryField
   ICALLHISTORY_EnumNext()

=======================================================================

======================================================================= 
  INTERFACES   DOCUMENTATION
======================================================================= 

ICallHistory Interface

The ICallHistory interface provides applications with the ability to add an 
entry to the Call History list, erase all the entries, enumerate the entries 
on the list, and update entries in that list.

Call History entries are made up of one or more Call History fields.  Call 
History fields can be broken up into two types: predefined and user-defined.  
Predefined fields are defined in AEECallHistory.h and are strictly defined so 
that all BREW applications and the native user interface can understand the 
contents.  OEM implementations will verify the contents of predefined fields 
at runtime to ensure that valid data is entered.  User-defined fields allow 
an application to store arbitrary binary data into the Call History database, 
so that applications wishing to store information associated with a call that 
is not covered by the predefined fields can do so.

OEMs may not store all field types. If an entry to be added/updated contains 
field types that the OEM cannot store, the function will return SUCCESS, and 
only those fields that are supported by the OEM will be available to be 
retrieved via the enumeration functions.

In addition, OEMs may require that the developer include certain predefined
field types so that they can properly handle the entry.  Developers attempting 
to add or update an entry that does not contain these field types will get an 
error code from the add/update function.

BREW developers should not expect that the entry when read back via 
enumeration will exactly match the entry they entered.  In particular, OEMs 
are free to reorder the fields and/or add additional fields as they see fit.

Detailled Call History information specific to a device can be found in the
Data Sheets on the BREW Developer Extranet.

The ICallHistory Interface functions include:

 -     ICALLHISTORY_AddEntry() adds a new Call History entry to the device's
       Call History list.

 -     ICALLHISTORY_Clear() erases the entire Call History list

 -     ICALLHISTORY_ClearEntry() erases one entry from the Call History list

 -     ICALLHISTORY_EnumInit() and ICALLHISTORY_EnumNext() provide a mechanism
       for a BREW application to read the contents of the Call History list.

 -     ICALLHISTORY_EnumInitByCallType() and ICALLHISTORY_EnumNext() provide
       a mechanism for a BREW application to read out all entries of a 
       specific type (i.e. Incoming, Outgoing, or Missed) from the Call
       History.

 -     ICALLHISTORY_UpdateEntry() modifies an existing entry, replacing the 
       selected entry with a new one.

 -     ICALLHISTORY_Notify() informs applications when the Call History
       has been modified by.

Predefined Field Types:
  An entry into the CallHistory is made up of one or more fields.  Each
  field is composed of a ClassID and FieldID which together indicate
  the type of data contained by the field.  BREW has several predefined
  field types that represent information commonly associated with voice
  calls.  For all of the predefined fields, the ClsId parameter should
  be set to 0.  

  User applications should not attempt to overload the predefined field
  types, but rather make an extended type associated with their own ClassID
  (and possibly approximate the value with the predefined equivalent), so
  that applications that are not aware of the extended data type can
  continue to operate correctly.

Predefined Fields:
===pre>
  AEECALLHISTORY_FIELD_CALL_TYPE (uint16):
    This field is a required field when adding an entry, else the add and update fail. 
    The call type must be one of the following values:
      AEECALLHISTORY_CALL_TYPE_TO
      AEECALLHISTORY_CALL_TYPE_FROM
      AEECALLHISTORY_CALL_TYPE_MISSED

  AEECALLHISTORY_FIELD_NUMBER_TYPE (uint16):
    The number type must be one of the following values:
      AEECALLHISTORY_NUMBER_TYPE_INTERNATIONAL
      AEECALLHISTORY_NUMBER_TYPE_NATIONAL
      AEECALLHISTORY_NUMBER_TYPE_NETWORK
      AEECALLHISTORY_NUMBER_TYPE_SUBSCRIBER
      AEECALLHISTORY_NUMBER_TYPE_ABREVIATED

  AEECALLHISTORY_FIELD_NUMBER_PLAN (uint16):
    The number plan must be one of the following values:
      AEECALLHISTORY_NUMBER_PLAN_TELEPHONY 
      AEECALLHISTORY_NUMBER_PLAN_DATA
      AEECALLHISTORY_NUMBER_PLAN_TELEX
      AEECALLHISTORY_NUMBER_PLAN_PRIVATE

  AEECALLHISTORY_FIELD_DATE_TIME (uint32):
    The date and time of the origination of the phone call expressed as
    the number of seconds since Jan 6, 1980 00:00:00 GMT.  This is the
    same format as the GETTIMESECONDS() helper function.

  AEECALLHISTORY_FIELD_CALL_DURATION (uint32):
    The duration of the call, in seconds  

  AEECALLHISTORY_FIELD_NUMBER (ASCII string):
    This is the number dialed.  Valid characters for the string include
    the ASCII digits '0' - '9', '#', '*', and ','.  The comma represents
    and OEM-defined soft pause.  OEMs are free to truncate the number
    field as needed by their implementations.  The length of this field
    refers to the string length only and does not include the terminating
    null character.
    
  AEECALLHISTORY_FIELD_NUMBER_EX (AECHAR string):
    This is an extended version of the AEECALLHISTORY_FIELD_NUMBER field.
    The format of this string is OEM-defined, however valid characters for
    the AEECALLHISTORY_FIELD_NUMBER field (with the possible exception of
    the pause character) are valid for this string.  For the purpose of 
    backward compatibility a AEECALLHISTORY_FIELD_NUMBER field (legacy) 
    will always be returned whether or not a AEECALLHISTORY_FIELD_NUMBER 
    field was present when the record was added.  In the case that the Call
    History only contains an AEECALLHISTORY_FIELD_NUMBER_EX field (extended)
    then the OEM will make a best-effort to convert the extended number to a
    legacy number fields and return both when enumerated.  If both the 
    legacy and extended versions of the field are present during an 
    ICALLHISTORY_AddEntry() or ICALLHISTORY_UpdateEntry() the legacy number 
    will always take precedence and will overwrite the extended field.  This
    means that when adding an extended field the user must remove the legacy 
    field first. OEMs are free to truncate the number field as needed by 
    their implementations.  The length of this field refers to the string 
    length only and does not include the terminating null character.  The 
    above rules imply that only one unique phone number is allowed per Call 
    History entry and two fields are different representations of this 
    unique number.

  AEECALLHISTORY_FIELD_NAME (AECHAR string):
    This is the text description of the call history entry.  It should be
    considered mutable by applications adding entries, and should
    therefore not be used to hold connection information.  For custom call
    types (that is, entries without a AEECALLHISTORY_FIELD_NUMBER), this value
    will be displayed to describe the call.  The length of this field
    refers to the string length only and does not include the terminating
    null character.

   AEECALLHISTORY_FIELD_PI  (AEETPI enum defined in AEETelephoneTypes.h):
      This field is used to indicate if the calling party's number is available and 
      whether presentation of the number is allowed.  The three options are 
      "presentation allowed", "presentation restricted", and "number not available".

      The field value for the three options are defined in the  AEETPI enum in AEETelephoneTypes.h. 
      See AEETelephoneTypes.h for the enum values of AEETPI.
      
   AEECALLHISTORY_FIELD_ID  (AECHAR string):
      This field stores the unique string ID assigned to each entry
      when the entry is added to the call history database.
      This field is generated by the call history application, and is not
      auto-generated by BREW. BREW does not specify uniqueness or a 
      length limit for this field.
      The length of this field refers to the string length only and does not 
      include the terminating null character. 
      

   AEECALLHISTORY_FIELD_CALLACTIVE  (boolean):
      This field is used to indicate whether the call 
      associated with this entry is still active(TRUE) or inactive(FALSE).
   
   AEECALLHISTORY_FIELD_DURATIONKNOWN (boolean):
      This field is used to indicate whether the call duration is 
      known(TRUE) or unknown(FALSE).

   AEECALLHISTORY_FIELD_NOTVIEWED (boolean):
      This field is used to indicate whether the entry is marked 
      as viewed (value = FALSE) or not viewed (value = TRUE). 
      It is mainly used for missed call entries so that entries marked 
      as viewed will no longer appear in the subsequent missed call notes.   
      If this field is not present, the entry should be considered viewed. 

   AEECALLHISTORY_FIELD_TECH_TYPE (uint16):
      This field is used to indicate whether the call is a voice call or video call. 
      It can be extended to indicate other type of calls if needed.
      The technology call type must be one of the following values:
      AEECALLHISTORY_TECH_TYPE_VOICE
      AEECALLHISTORY_TECH_TYPE_VIDEO

  AEECALLHISTORY_FIELD_PTT_CALL_TYPE (uint16):
     The lsb of the call type must be one of the following values:
       AEECALLHISTORY_PTT_CALL_TYPE_NONE
       AEECALLHISTORY_PTT_CALL_TYPE_DIRECT
       AEECALLHISTORY_PTT_CALL_TYPE_ALERT
       AEECALLHISTORY_PTT_CALL_TYPE_GROUP
     The msb of the call type must be one of the following values:
       AEECALLHISTORY_PTT_CALL_SUBTYPE_NONE       
       AEECALLHISTORY_PTT_CALL_SUBTYPE_FAILED     
       AEECALLHISTORY_PTT_CALL_SUBTYPE_RESTRICTED

  AEECALLHISTORY_FIELD_USER_ADDRESS (Unicode null-terminated comma-separated string list):
    This is the field storing PTT call user address(es).
  AEECALLHISTORY_FIELD_CONFERENCE_ID (ASCII null-terminated string):
    This is the field storing the PTT call conference ID.

  AEECALLHISTORY_FIELD_ALERT_MSG_INDEX (uint8):
    This is the field storing the alert message index in PTT alert.

  AEECALLHISTORY_FIELD_ALERT_MSG_TEXT (Unicode null-terminated string):
    This is the field storing the alert message text in PTT alert.

  AEECALLHISTORY_FIELD_ALT_CONFERENCE_ID (ASCII null-terminated string):
    This is the field storing the alternate conference ID of a PTT call.    
===/pre>


The following header file is required: 

AEECallHistory.h
=====================================================
ICALLHISTORY_AddRef()

Description:
	This function is inherited from IBASE_AddRef(). 

=====================================================
ICALLHISTORY_Release()

Description:
	This function is inherited from IBASE_Release().  
	
=====================================================
ICALLHISTORY_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface(). 
	
=====================================================
Function: ICALLHISTORY_AddEntry()

Description: 
   This function adds a new entry to the Call History list. If the maximum 
   number of entries is reached, the function will typically delete the oldest 
   entry in the list before adding the new entry.

   In an effort to be consistent with their native user interfaces, some
   OEMs may delete identical or similiar entries before adding a new entry.
   For instance, if a BREW developer adds an outgoing call with the same 
   phone number as an existing entry, that existing entry may be deleted if 
   the native user interface does not display duplicates.

Prototype:
   int ICALLHISTORY_AddEntry(ICallHistory *pich,
                             const AEECallHistoryEntry *pche)

Parameters:
   pich: [in]:   Pointer to an ICallHistory interface object
   pche: [in]:   New entry

Return Value:
   SUCCESS, if successful

   EBADPARM, if the user specifies invalid data for a predefined field type

   ENOMEMORY, if the implementation cannot allocate memory internally for the
      operation.

   EFAILED, for other errors~

Comments:  
   The memory specified in pche is copied by the ICALLHISTORY implementation,
   and need not be maintained by the caller after the call to AddEntry()

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1
  
See Also: 
   None
   
======================================================================= 
Function: ICALLHISTORY_EnumInit()

Description: 
  This function initializes or resets the enumeration in the Call History list.

Prototype:
   int ICALLHISTORY_EnumInit(ICallHistory *pich)
                                               
Parameters:
   pich: [in]:   Pointer to an ICallHistory interface object

  
Return Value:
   SUCCESS, if successful

   ENOMEMORY, if the implementation cannot allocate memory internally for the
      operation.

   EFAILED, for other errors~

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1
  
See Also: 
   ICALLHISTORY_EnumNext()

======================================================================= 
Function: ICALLHISTORY_EnumInitByCallType()

Description: 
   This function initializes or resets the enumeration in the Call 
   History list.  Only entries of the specified type will be returned
   when enumerated until ICALLHISTORY_EnumInit is called at which time
   enumeration will return to a best-effort reverse chronilogical order.

   Enumeration of the Call History is defined to be done, in best-effort
   reverse chronological order.  (that is, returned in order from newest 
   to oldest).
   
Prototype:
   const int ICALLHISTORY_EnumInitByCallType(ICallHistory *pche,
                                                              uint16 type);

Parameters:
   pich:  [in]:    Pointer to an ICallHistory interface object
   type:  [in]:    Call type to enumerate

Return Value:
   SUCCESS, if successful

   ENOMEMORY, if the implementation cannot allocate memory internally for the
      operation.

   EFAILED, for other errors~
  
Comments:  
   The memory pointed to by the returned AEECallHistoryEntry is owned by
   the ICALLHISTORY object.  Its contents will stay valid until the next call
   to the ICallHistory interface.  The contents of the returned pointer 
   *must not* be modified by the caller.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1
  
See Also: 
   None

======================================================================= 
Function: ICALLHISTORY_EnumNext()

Description: 
   This function retrieves the next entry in the Call History list. 
   Either ICALLHISTORY_EnumInit() or ICALLHISTORY_EnumInitByCallType must be 
   called before any successive calls to this function. 

   Enumeration of the Call History is defined to be done, in best-effort
   reverse chronological order.  (that is, returned in order from newest 
   to oldest).

Prototype:
   const AEECallHistoryEntry* ICALLHISTORY_EnumNext(ICallHistory *pche,
                                                    int *pnErr);

Parameters:
   pche:  [in]   Pointer to an ICallHistory interface object
   pnErr: [out]  Pointer to an integer to hold any error value, set to
                     SUCCESS if successful or at end of enumeration
                     EFAILED or another BREW error if an error occurs

Return Value:
    The "next" entry in the Call History list, if there is an entry to return.
   
	 NULL if we're at the last entry or an error occurs
  
Comments:  
   The memory pointed to by the returned AEECallHistoryEntry is owned by
   the ICALLHISTORY object.  Its contents will stay valid until the next call
   to ICALLHISTORY_EnumNext(), ICALLHISTORY_EnumInit() or 
   ICALLHISTORY_Release().  The contents of the returned pointer *must not*
   be modified by the caller.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1
  
See Also: 
   None

======================================================================= 
Function: ICALLHISTORY_UpdateEntry()

Description: 
   This function replaces the current call history entry with the one 
   provided.  The current entry is defined as the entry that was returned 
   during the most recent call to ICALLHISTORY_EnumNext().

   Updating an entry may change its order when enumerating.
   
   Updating an entry may not be supported on all platforms, and some platforms
   may not allow all fields to be updated.

Prototype:
   int ICALLHISTORY_UpdateEntry(ICallHistory *pich, 
                                const AEECallHistoryEntry *pche);
   
Parameters:
    pich: [in]:  Pointer to an ICallHistory interface object
    pche: [in]:  New data to replace existing entry

Return Value:
   SUCCESS, if successful

   EBADPARM, if the user specifies invalid data for a predefined field type

   ENOMEMORY, if the implementation cannot allocate memory internally for the
      operation.

   EFAILED, or some other BREW error if failed to update entries

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1
  
See Also: 
   None

======================================================================= 

Function: ICALLHISTORY_Clear()

Description: 
   This function deletes all entries from the Call History list in the
   current storage.

Prototype:
   int ICALLHISTORY_Clear(ICallHistory *pich)
   
Parameters:
   pich: [in]:  Pointer to an ICallHistory interface object

Return Value:
   SUCCESS, if successful

   ENOMEMORY, if the implementation cannot allocate memory internally for the
      operation.

   EFAILED, if otherwise unsuccessful

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1
  
See Also: 
   None

======================================================================= 

Function: ICALLHISTORY_ClearEntry()

Description: 
   This function deletes the last enumerated call history entry.
 
Prototype:
   int ICALLHISTORY_ClearEntry(ICallHistory *pich)
   
Parameters:
   pich: [in]:  Pointer to an ICallHistory interface object

Return Value:
   SUCCESS, if successful

   ENOMEMORY, if the implementation cannot allocate memory internally for the
      operation.

   EFAILED, if otherwise unsuccessful

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1
  
See Also: 
   None

=======================================================================

Function: ICALLHISTORY_Notify()

Description: 
   This function sets a notification function that calls back the user
   if the Call History Changes.  The callback will continue to execute
   until canceled by the user.  Not all OEMs will support this function.
 
Prototype:
   int ICALLHISTORY_Notify(ICallHistory *pich, AEECallback *pcb)
   
Parameters:
   pich: [in]:  Pointer to an ICallHistory interface object
   pcb:  [in/out]:  Pointer to an ICallHistory interface object

Return Value:
   SUCCESS, if successful

   ENOMEMORY, if the implementation cannot allocate memory internally for the
      operation.

   EUNSUPPORTED, if the OEM does not support call notifications

   EFAILED, if otherwise unsuccessful

Comments:  
   Unlike other usages of the AEECallback structure, the notification
   callback remains valid unless canceled.  (i.e. the callbacks are not
   "one-shot").  Applications no longer wishing for callback notifications
   should call CALLBACK_Cancel() on their registered callback structure.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1
  
See Also: 
   None

=====================================================================*/
#endif      // AEECALLHISTORY_H



