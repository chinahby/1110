#ifndef AEEANNUNCIATOR_H
#define AEEANNUNCIATOR_H
/*=====================================================================
FILE:  AEEAnnunciator.h

SERVICES:  AEE Annunciator Interface

DESCRIPTION:  Base level definitions, typedefs, etc. for AEE Annunciator

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable

        Copyright ?1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //depot/asic/qsc60x0/apps/StaticExtensions/Inc/AEEAnnunciator.h#1 $

=====================================================================*/

//=====================================================================
// INCLUDES AND VARIABLE DEFINITIONS
//=====================================================================

//---------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------

#include "AEE.h"

#define AEECLSID_ANNUNCIATOR            0x01011a5b /* real class ID from BREW */

//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------

typedef struct IAnnunciator IAnnunciator;

//---------------------------------------------------------------------
// Global Constant Declarations
//---------------------------------------------------------------------
// Annunciator Fields
/*fields that locate at the same place are merged, and use the states ID to dicide which icon to display*/

#if defined(FEATURE_VERSION_K212)
#define ANNUN_FIELD_RSSI                       0   /*Airplane Mode/RSSI*/
#define ANNUN_FIELD_HEADSET                       1   /*Wap/1x/Roam*/
#define ANNUN_FIELD_TCARD						2		//Add By zzg 2012_10_30
#define ANNUN_FIELD_CALL                      3   /*Loudspeaker/Mute/Emergency/Inuse/Missed Call*/
#define ANNUN_FIELD_SMS                       4   /*VMail/SMS/SMS Memory Full*/
#define ANNUN_FIELD_QQ                     5   /*QQ*/
#define ANNUN_FIELD_ALARM                    6   /*Timer/Alarm/Schedule*/
#define ANNUN_FIELD_RINGTONE              7   /*Ringtone*/
#define ANNUN_FIELD_BATT                     8   /*Battery*/

#define ANNUN_FIELD_FMRADIO                9   /*FMRadio/Headset*/
#define ANNUN_FIELD_BLUETOOTH            10   /*BT Trans/BT Headset/BT On*/
#define ANNUN_FIELD_LOCKSTATUS           11  /*Voice Privacy/Lockstatus*/
#define ANNUN_FIELD_MMS                       12   /*MMS Full/MMS Unread/MMS Unreceive/Push*/
#define ANNUN_FIELD_WAP                       13   /*Wap/1x/Roam*/
#else
#define ANNUN_FIELD_RSSI                       0   /*Airplane Mode/RSSI*/
#define ANNUN_FIELD_WAP                       1   /*Wap/1x/Roam*/
#define ANNUN_FIELD_LOCKSTATUS           2   /*Voice Privacy/Lockstatus*/
#define ANNUN_FIELD_CALL                      3   /*Loudspeaker/Mute/Emergency/Inuse/Missed Call*/
#define ANNUN_FIELD_SMS                       4   /*VMail/SMS/SMS Memory Full*/
#define ANNUN_FIELD_FMRADIO                5   /*FMRadio/Headset*/
//#define ANNUN_FIELD_QQ                     2   /*QQ*/
#define ANNUN_FIELD_BLUETOOTH            6   /*BT Trans/BT Headset/BT On*/
#define ANNUN_FIELD_ALARM                    7   /*Timer/Alarm/Schedule*/
#define ANNUN_FIELD_MMS                       8   /*MMS Full/MMS Unread/MMS Unreceive/Push*/
#define ANNUN_FIELD_RINGTONE              9   /*Ringtone*/
#define ANNUN_FIELD_BATT                     10   /*Battery*/
#define ANNUN_FIELD_3G_RSSI                    11    //Add By zzg 2013_09_06
#define ANNUN_FIELD_TCARD						2		//Add By zzg 2012_10_30
#endif


// Possible State of each annunciator field
#define ANNUN_STATE_OFF       0
#define ANNUN_STATE_ON        1

// Mask to get/set the blink bit in the state
#define ANNUN_STATE_BLINK         0x80000000
#define ANNUN_STATE_UNBLINK     0x7FFFFFFF

#if 1
// Mask to strip the blink bit from the state
#define ANNUN_STATE_BLINK_UNMASK  0x7FFFFFFF

/*Currently, the maxium number of different states in one field is 6*/
// Mask to get the highest priority state
#define ANNUN_FIRST_STATE   0x00000004
#define ANNUN_FIRST_STATE_OFF   0xFFFFFFFA

// Mask to get the second highest priority state
#define ANNUN_SECOND_STATE   0x00000008
#define ANNUN_SECOND_STATE_OFF   0xFFFFFFF6

// Mask to get the third highest priority state
#define ANNUN_THIRD_STATE   0x00000010
#define ANNUN_THIRD_STATE_OFF   0xFFFFFFEE

// Mask to get the forth highest priority state
#define ANNUN_FORTH_STATE   0x00000020
#define ANNUN_FORTH_STATE_OFF   0xFFFFFFDE

// Mask to get the fifth highest priority state
#define ANNUN_FIFTH_STATE   0x00000040
#define ANNUN_FIFTH_STATE_OFF   0xFFFFFFBE

// Mask to get the sixth highest priority state
#define ANNUN_SIXTH_STATE   0x00000080
#define ANNUN_SIXTH_STATE_OFF   0xFFFFFF7E

// Mask to get the seven highest priority state
#define ANNUN_SEVENTH_STATE   0x00000100
#define ANNUN_SEVENTH_STATE_OFF   0xFFFFFEFE
#ifdef FEATURE_VERSION_VG68
// Mask to get the eight highest priority state
#define ANNUN_EIGHTH_STATE   0x00000200
#define ANNUN_EIGHTH_STATE_OFF   0xFFFFFDFE
// Mask to get the nine highest priority state
#define ANNUN_NINTH_STATE   0x00000400
#define ANNUN_NINTH_STATE_OFF   0xFFFFFBFE
#endif

#ifdef FEATURE_VERSION_LM126C   //add by yangdecai
// Mask to get the eight highest priority state
#define ANNUN_EIGHTH_STATE   0x00000200
#define ANNUN_EIGHTH_STATE_OFF   0xFFFFFDFE

#endif

// Mask to know if the state is going to turn a state off
#define ANNUN_TURN_OFF_STATE    0x00000002

// Mask to know whether the buffer bits contains information
#define ANNUN_STATE_BUFFERED_MASK  0x000FFC00
// Mask to clear the buffer bits
#define ANNUN_STATE_BUFFERED_UNMASK  0x000003FF
#endif
//end added

// Field specific custom states

// Mode field
#define ANNUN_STATE_ANALOG          1
#define ANNUN_STATE_CDMA            2
#define ANNUN_STATE_GSM             3
#define ANNUN_STATE_WCDMA           4
#define ANNUN_STATE_HDR             5
#define ANNUN_STATE_HYBRID          6
#define ANNUN_STATE_SIMULTANEOUS    7
#define ANNUN_STATE_GPS             8 //Not used currently
#define ANNUN_STATE_DIGITAL         9

// Domain field
#define ANNUN_STATE_CS        1
#define ANNUN_STATE_PS        2
#define ANNUN_STATE_CS_PS     3

/*ANNUN_FIELD_RSSI*/
#define ANNUN_STATE_AIR_MODE_ON     ANNUN_FIRST_STATE
#define ANNUN_STATE_AIR_MODE_OFF   ANNUN_FIRST_STATE_OFF
#define ANNUN_STATE_RSSI_NO_SERV    ANNUN_SECOND_STATE
#define ANNUN_STATE_RSSI_0                ANNUN_THIRD_STATE
#define ANNUN_STATE_RSSI_1                ANNUN_FORTH_STATE
#define ANNUN_STATE_RSSI_2                ANNUN_FIFTH_STATE
#define ANNUN_STATE_RSSI_3                ANNUN_SIXTH_STATE
#define ANNUN_STATE_RSSI_4                ANNUN_SEVENTH_STATE
#ifdef FEATURE_VERSION_VG68
#define ANNUN_STATE_RSSI_5                ANNUN_EIGHTH_STATE
#define ANNUN_STATE_RSSI_6                ANNUN_NINTH_STATE
#endif

#ifdef FEATURE_VERSION_LM126C
#define ANNUN_STATE_RSSI_5                ANNUN_EIGHTH_STATE
#endif


//Add By zzg 2012_10_29
#define ANNUN_STATE_TCARD_ON    ANNUN_FIRST_STATE
#define ANNUN_STATE_TCARD_OFF   ANNUN_FIRST_STATE_OFF
//Add End

/*ANNUN_FIELD_WAP*/
#define ANNUN_STATE_WAP_ON      ANNUN_FIRST_STATE
#define ANNUN_STATE_WAP_OFF    ANNUN_FIRST_STATE_OFF
#define ANNUN_STATE_1X_ON         ANNUN_SECOND_STATE
#define ANNUN_STATE_1X_OFF       ANNUN_SECOND_STATE_OFF
#define ANNUN_STATE_ROAM_ON    ANNUN_THIRD_STATE
#define ANNUN_STATE_ROAM_OFF   ANNUN_THIRD_STATE_OFF

/*ANNUN_FIELD_LOCKSTATUS*/
#define ANNUN_STATE_LOCKSTATUS_ON    ANNUN_FIRST_STATE
#define ANNUN_STATE_LOCKSTATUS_OFF   ANNUN_FIRST_STATE_OFF
#define ANNUN_STATE_VOICE_PRIV_ON     ANNUN_SECOND_STATE
#define ANNUN_STATE_VOICE_PRIV_OFF   ANNUN_SECOND_STATE_OFF

/*ANNUN_FIELD_CALL*/
#define ANNUN_STATE_CALL_EMERGENCY_ON        ANNUN_FIRST_STATE
#define ANNUN_STATE_CALL_EMERGENCY_OFF      ANNUN_FIRST_STATE_OFF
#define ANNUN_STATE_CALL_MUTE_ON                  ANNUN_SECOND_STATE
#define ANNUN_STATE_CALL_MUTE_OFF                ANNUN_SECOND_STATE_OFF
#define ANNUN_STATE_CALL_LOUDSPEAKER_ON     ANNUN_THIRD_STATE
#define ANNUN_STATE_CALL_LOUDSPEAKER_OFF    ANNUN_THIRD_STATE_OFF
#define ANNUN_STATE_CALL_INUSE_ON                 ANNUN_FORTH_STATE
#define ANNUN_STATE_CALL_INUSE_OFF               ANNUN_FORTH_STATE_OFF
#define ANNUN_STATE_CALL_MISSEDCALL_ON       ANNUN_FIFTH_STATE
#define ANNUN_STATE_CALL_MISSEDCALL_OFF      ANNUN_FIFTH_STATE_OFF

/*ANNUN_FIELD_SMS*/
#define ANNUN_STATE_SMS_MAILFULL_ON    ANNUN_FIRST_STATE
#define ANNUN_STATE_SMS_MAILFULL_OFF  ANNUN_FIRST_STATE_OFF
#define ANNUN_STATE_SMS_VMAIL_ON        ANNUN_SECOND_STATE
#define ANNUN_STATE_SMS_VMAIL_OFF       ANNUN_SECOND_STATE_OFF
#define ANNUN_STATE_SMS_SMAIL_ON         ANNUN_THIRD_STATE
#define ANNUN_STATE_SMS_SMAIL_OFF       ANNUN_THIRD_STATE_OFF

/*ANNUN_FIELD_SPORT*/
#define ANNUN_STATE_SPORTS_ON     ANNUN_FIRST_STATE
#define ANNUN_STATE_SPORTS_OFF   ANNUN_FIRST_STATE_OFF

/*ANNUN_FIELD_FMRADIO*/
#define ANNUN_STATE_FMRADIO_ON    ANNUN_FIRST_STATE
#define ANNUN_STATE_FMRADIO_OFF  ANNUN_FIRST_STATE_OFF
#ifdef FEATURE_VERSION_K212
#define ANNUN_STATE_QQ_MSG_ON      ANNUN_FIRST_STATE
#define ANNUN_STATE_QQ_MSG_OFF     ANNUN_FIRST_STATE_OFF
#define ANNUN_STATE_QQ_ONLINE      ANNUN_SECOND_STATE
#define ANNUN_STATE_QQ_OFFLINE     ANNUN_SECOND_STATE_OFF
#define ANNUN_STATE_QQ_HIDING_ON   ANNUN_THIRD_STATE
#define ANNUN_STATE_QQ_HIDING_OFF  ANNUN_THIRD_STATE_OFF
#define ANNUN_STATE_QQ_LEAVE_ON    ANNUN_FORTH_STATE
#define ANNUN_STATE_QQ_LEAVE_OFF   ANNUN_FORTH_STATE_OFF
#define ANNUN_STATE_HEADSET_ON     ANNUN_FIRST_STATE 
#define ANNUN_STATE_HEADSET_OFF    ANNUN_FIRST_STATE_OFF
#else
#ifdef FEATURE_QQ_APP
#define ANNUN_STATE_QQ_MSG_ON      ANNUN_SECOND_STATE
#define ANNUN_STATE_QQ_MSG_OFF     ANNUN_SECOND_STATE_OFF
#define ANNUN_STATE_QQ_ONLINE      ANNUN_THIRD_STATE
#define ANNUN_STATE_QQ_OFFLINE     ANNUN_THIRD_STATE_OFF
#define ANNUN_STATE_QQ_HIDING_ON   ANNUN_FORTH_STATE
#define ANNUN_STATE_QQ_HIDING_OFF  ANNUN_FORTH_STATE_OFF
#define ANNUN_STATE_QQ_LEAVE_ON   ANNUN_FIFTH_STATE
#define ANNUN_STATE_QQ_LEAVE_OFF  ANNUN_FIFTH_STATE_OFF
#define ANNUN_STATE_HEADSET_ON    ANNUN_SIXTH_STATE
#define ANNUN_STATE_HEADSET_OFF   ANNUN_SIXTH_STATE_OFF
#else
#define ANNUN_STATE_HEADSET_ON   ANNUN_SECOND_STATE 
#define ANNUN_STATE_HEADSET_OFF  ANNUN_SECOND_STATE_OFF
#endif
#endif




/*ANNUN_FIELD_BLUETOOTH*/
#define ANNUN_STATE_BT_TRANS_ON       ANNUN_FIRST_STATE
#define ANNUN_STATE_BT_TRANS_OFF     ANNUN_FIRST_STATE_OFF
#define ANNUN_STATE_BT_HEADSET_ON   ANNUN_SECOND_STATE
#define ANNUN_STATE_BT_HEADSET_OFF  ANNUN_SECOND_STATE_OFF
#define ANNUN_STATE_BT_ON                  ANNUN_THIRD_STATE
#define ANNUN_STATE_BT_OFF                ANNUN_THIRD_STATE_OFF

/*ANNUN_FIELD_ALARM*/
#define ANNUN_STATE_TIMER_ON          ANNUN_FIRST_STATE
#define ANNUN_STATE_TIMER_OFF        ANNUN_FIRST_STATE_OFF
#define ANNUN_STATE_ALARM_ON         ANNUN_SECOND_STATE
#define ANNUN_STATE_ALARM_OFF       ANNUN_SECOND_STATE_OFF
#define ANNUN_STATE_SCHEDULE_ON    ANNUN_THIRD_STATE
#define ANNUN_STATE_SCHEDULE_OFF  ANNUN_THIRD_STATE_OFF

/*ANNUN_FIELD_MMS*/
#define ANNUN_MMS_FULL_ON              ANNUN_FIRST_STATE
#define ANNUN_MMS_FULL_OFF             ANNUN_FIRST_STATE_OFF
#define ANNUN_MMS_UNREAD_ON          ANNUN_SECOND_STATE
#define ANNUN_MMS_UNREAD_OFF        ANNUN_SECOND_STATE_OFF
#define ANNUN_MMS_UNRECEIVE_ON     ANNUN_THIRD_STATE
#define ANNUN_MMS_UNRECEIVE_OFF   ANNUN_THIRD_STATE_OFF
#define ANNUN_STATE_PUSH_ON           ANNUN_FORTH_STATE
#define ANNUN_STATE_PUSH_OFF          ANNUN_FORTH_STATE_OFF

/*ANNUN_FIELD_RINGTONE*/
#define ANNUN_STATE_RINGTONE_ALERT       ANNUN_FIRST_STATE
#define ANNUN_STATE_RINGTONE_SILENT      ANNUN_SECOND_STATE
#define ANNUN_STATE_RINGTONE_VIBRING    ANNUN_THIRD_STATE
#define ANNUN_STATE_RINGTONE_VIBRATOR  ANNUN_FORTH_STATE
/*
#ifdef FEATURE_VERSION_C316
#define ANNUN_STATE_RINGTONE_BLUETOOTH  ANNUN_FIFTH_STATE
#endif
*/

// In Use field
#define ANNUN_STATE_IN_USE    1
#define ANNUN_STATE_NO_SVC    2

// Alarm field
#define ANNUN_STATE_ALERT_ON  1
#define ANNUN_STATE_SILENCE   2

// Message field

// Battery field
#define ANNUN_STATE_BATT_LOW  ANNUN_FIRST_STATE
#define ANNUN_STATE_BATT_1       ANNUN_SECOND_STATE
#define ANNUN_STATE_BATT_2       ANNUN_THIRD_STATE
#define ANNUN_STATE_BATT_3       ANNUN_FORTH_STATE
#if /*defined(FEATURE_VERSION_LM126C)||*/defined(FEATURE_5_LEVEL_BATTERY) //xxzhen
#define ANNUN_STATE_BATT_4       ANNUN_FIFTH_STATE
#define ANNUN_STATE_BATT_FULL    ANNUN_SIXTH_STATE
#else
#define ANNUN_STATE_BATT_FULL    ANNUN_FIFTH_STATE
#endif
// QQ field
//#define ANNUN_STATE_QQ_MSG      ANNUN_FIRST_STATE
//#define ANNUN_STATE_QQ_ONLINE   ANNUN_SECOND_STATE

//Network field
#define ANNUN_STATE_DORMANT   0
#define ANNUN_STATE_ACTIVE    1

/* Annunciator types */
#define ANNUN_TYPE_IMAGE      0
#define ANNUN_TYPE_TEXT       1

//---------------------------------------------------------------------
// Global Data Declarations
//---------------------------------------------------------------------

//=====================================================================
// FUNCTION DECLARATIONS AND INLINE FUNCTION DEFINITIONS
//=====================================================================
//---------------------------------------------------------------------
// IAnnunciator Interface
//---------------------------------------------------------------------

AEEINTERFACE(IAnnunciator)
{
   INHERIT_IQueryInterface(IAnnunciator);

   int     (*GetField)   (IAnnunciator * pIAnnunciator, uint32 nAnnunID, uint32 * pnState);
   int     (*SetField)   (IAnnunciator * pIAnnunciator, uint32 nAnnunID, uint32 nState);
   int     (*SetFieldEx) (IAnnunciator * pMe, uint32 nAnnunID, void * pData);
   int     (*Redraw) (IAnnunciator *pMe);
   int     (*EnableAnnunciatorBar)   (IAnnunciator * pMe, AEECLSID clsid, boolean bOn);
   int     (*GetAnnunciatorBarSize)  (IAnnunciator * pMe, AEECLSID clsid, AEERect *pRec);
   int     (*SetUnblinkTimer) (IAnnunciator * pMe, uint32 nAnnunID, uint32 nState, uint32 nTimeMs);
   int     (*SetFieldIsActiveEx) (IAnnunciator * pMe,boolean bActive);   //add by yangdecai 2010-07-13
   int     (*SetFieldText)     (IAnnunciator * pMe,uint16 * cText, boolean bUpdate);    //add by yangdecai 2010-07-13
   int     (*SetHasTitleText)     (IAnnunciator * pMe, boolean hasTitleText);
   int     (*SetNotUpdate)     (IAnnunciator * pMe, boolean bNotUpdate);
};

//---------------------------------------------------------------------
// Access Macros
//---------------------------------------------------------------------

#define IANNUNCIATOR_AddRef(p)           AEEGETPVTBL((p),IAnnunciator)->AddRef((p))
#define IANNUNCIATOR_Release(p)          AEEGETPVTBL((p),IAnnunciator)->Release((p))
#define IANNUNCIATOR_QueryInterface(p,i,p2) AEEGETPVTBL((p),IAnnunciator)->QueryInterface((p),(i),(p2))
#define IANNUNCIATOR_GetField(p,id,ps)   AEEGETPVTBL((p),IAnnunciator)->GetField((p),(id),(ps))
#define IANNUNCIATOR_SetField(p,id,s)    AEEGETPVTBL(((p)),IAnnunciator)->SetField((p),(id),(s))
#define IANNUNCIATOR_SetFieldEx(p,id, pd) AEEGETPVTBL((p),IAnnunciator)->SetFieldEx((p),(id),(pd))
#define IANNUNCIATOR_Redraw(p) AEEGETPVTBL((p), IAnnunciator)->Redraw((p))
#define IANNUNCIATOR_EnableAnnunciatorBar(p,cls,b)   AEEGETPVTBL((p),IAnnunciator)->EnableAnnunciatorBar((p),(cls),(b))
#define IANNUNCIATOR_GetAnnunciatorBarSize(p,cls,pr) AEEGETPVTBL((p),IAnnunciator)->GetAnnunciatorBarSize((p),(cls),(pr))
#define IANNUNCIATOR_SetUnblinkTimer(p,id,s,n)   AEEGETPVTBL((p),IAnnunciator)->SetUnblinkTimer((p),(id),(s),(n))
#define IANNUNCIATOR_SetFieldIsActiveEx(p,b)       AEEGETPVTBL((p),IAnnunciator)->SetFieldIsActiveEx((p),(b))
#define IANNUNCIATOR_SetFieldText(p,c)           AEEGETPVTBL((p),IAnnunciator)->SetFieldText((p),(c),(TRUE))
#define IANNUNCIATOR_SetFieldTextEx(p,c,b)       AEEGETPVTBL((p),IAnnunciator)->SetFieldText((p),(c),(b))
#define IANNUNCIATOR_SetHasTitleText(p,c)           AEEGETPVTBL((p),IAnnunciator)->SetHasTitleText((p),(c))
#define IANNUNCIATOR_SetNotUpdate(p,c)           AEEGETPVTBL((p),IAnnunciator)->SetNotUpdate((p),(c))


/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================
=======================================================================
AEE IAnnunciator IDs

Description:
These are the predefined annunciator IDs for the IAnnunciator interface .

Definition:

   ANNUN_FIELD_MSG       - Voicemail icon
   ANNUN_FIELD_NET       - Network connection
   ANNUN_FIELD_ALARM     - Alarm clock
   ANNUN_FIELD_SECURITY  - Phone in secure mode/locked
   ANNUN_FIELD_MODE      - Mode (CDMA/WCDMA/GSM/Analog etc.)
   ANNUN_FIELD_ROAM      - Roaming status
   ANNUN_FIELD_DOMAIN    - Domain (Circuit Switched/Packet Switched/HDR etc.)
   ANNUN_FIELD_RSSI      - RSSI level
   ANNUN_FIELD_INUSE     - In use
   ANNUN_FIELD_BATT      - Battery level
   ANNUN_FIELD_TIME      - Date and time field
   ANNUN_FIELD_BANNER    - User defined banner

Members:
None

Comments:
None

See Also:
IANNUNCIATOR_EnumNext()
IANNUNCIATOR_GetField()
IANNUNCIATOR_SetField()

=======================================================================
AEE IAnnunciator States

Description:
These are the possible states of the annunciators in the IAnnunciator interface.

Definition:
   ANNUN_STATE_OFF    - Annunciator turned off
   ANNUN_STATE_ON     - Annunciator turned on

   Apart from the above, depending on the field, you could set it to specific states
   For e.g. ANNUN_FIELD_RSSI can be set to ANNUN_STATE_OFF, ANNUN_STATE_RSSI_0,
   ANNUN_STATE_RSSI_1, ... through ANNUN_STATE_RSSI_4. Moreover, you could make the
   field blink by enabling the blink mask.

   For e.g. IANNUNCIATOR_SetField (pMe, ANNUN_FIELD_RSSI, ANNUN_STATE_BLINK | ANNUN_STATE_RSSI_4);
   would set the RSSI field to level 4 and make that field blink.

   To disable blinking, you can either call IANNUNCIATOR_SetField with the blink mask off or
   set the state to ANNUN_STATE_OFF (which would also disable the annunciator).


Members:
None

Comments:
None

See Also:
IANNUNCIATOR_GetField()
IANNUNCIATOR_SetField()

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

Interface Name: IAnnunciator

Description:
The IAnnunciator interface provides services for applications to query the
various annunciators supported and methods to set and retrieve values of the
various annunciators it is allowed access to.

The IAnnunciator interfaces has a bunch of predefined annunciator IDs as
described below.

   ANNUN_FIELD_MSG       - Voicemail icon
   ANNUN_FIELD_NET       - Network connection
   ANNUN_FIELD_ALARM     - Alarm clock
   ANNUN_FIELD_SECURITY  - Phone in secure mode/locked
   ANNUN_FIELD_MODE      - Mode (CDMA/WCDMA/GSM/Analog etc.)
   ANNUN_FIELD_ROAM      - Roaming status
   ANNUN_FIELD_DOMAIN    - Domain (Circuit Switched/Packet Switched/HDR etc.)
   ANNUN_FIELD_RSSI      - RSSI level
   ANNUN_FIELD_INUSE     - In use
   ANNUN_FIELD_BATT      - Battery level
   ANNUN_FIELD_TIME      - Date and time field
   ANNUN_FIELD_BANNER    - User defined banner

An OEM may support all the above annunciators, support only some of the above
annunciators or maybe even none. Its upto the OEM to define which annunciators
can be accessed by third-party applications etc.

To use the IAnnunciator interface, perform the following steps:

    Call ISHELL_CreateInstance() to create the annunciator instance and
obtain an interface pointer to it.

    Call IANNUNCIATOR_SetField() to set an annunciator to a given value.

    When you no longer need the interface, call IANNUNCIATOR_Release()
to free it.

Functions in this interface include:
IANNUNCIATOR_AddRef()
IANNUNCIATOR_GetField()
IANNUNCIATOR_QueryInterface()
IANNUNCIATOR_Release()
IANNUNCIATOR_SetField()

=======================================================================

IANNUNCIATOR_AddRef()

Description:
    This function increments the reference count of the IAnnunciator Interface
    object. This allows the object to be shared by multiple callers. The
    object is freed when the reference count reaches 0 (zero).

Prototype:
    uint32 IANNUNCIATOR_AddRef(IAnnunciator * pIAnnunciator)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object

Return Value:
    Incremented reference count for the object.

Comments:
    A valid object returns a positive reference count.

Side Effects:
    None

See Also:
    IANNUNCIATOR_Release()

=======================================================================

IANNUNCIATOR_GetField()

Description:
    This function retrives the state of an annunciator.

Prototype:
    int IANNUNCIATOR_GetField(IAnnunciator * pIAnnunciator, uint32 nAnnunID, uint32 * pnState)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object
    nAnnunID      : ID of the annunciator to query
    pnState       : Placeholder to return annunciator state
                    The state of the annunciator can be the following
                       ANNUN_STATE_OFF    - Annunciator turned off
                       ANNUN_STATE_ON     - Annunciator turned on
                    Or, it can be a specific value depending on the field
                    (e.g. ANNUN_STATE_RSSI_4)
                    In addition, you can logically AND the state with ANNUN_STATE_BLINK
                    find out if the annunciator is blinking

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    None

Side Effects:
    None

See Also:
    IANNUNCIATOR_SetField()


==============================================================================
Function: IANNUNCIATOR_QueryInterface()

Description:
   This function retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary features.

   Upon a successful query, the interace is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:
   int IANNUNCIATOR_QueryInterface(IAnnunciator *po, AEECLSID id, void **p);

Parameters:
   po [in]: Pointer to IAnnunciator interface.

   id [in]: A globally unique id to identify the entity (interface or data)
            that we are trying to query.

   p [out]: Pointer to the data or interface that we want to retrieve.  If
            the value passed back is NULL, the interface or data that we
            query are not available.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments:
   On failure, *po should be set to NULL, but it is good form to explicitly
   set *po to NULL before calling QueryInterface().

   NOTE: This function currently only responds to AEECLSID_ANNUNCIATOR.

Side Effects:
   None

See Also:
   None

=======================================================================

IANNUNCIATOR_Release()

Description:
    This function decrements the reference count for the IAnnunciator Interface
    object and does appropriate cleanup if the reference count reaches 0 (zero).

Prototype:
    uint32 IANNUNCIATOR_Release(IAnnunciator * pIAnnunciator)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object whose reference
                    count needs to be decremented

Return Value:
    Updated reference count for the object.

Comments:
    None

Side Effects:
    None

See Also:
    None

=======================================================================

IANNUNCIATOR_SetField()

Description:
    This function sets the state of an annunciator.

Prototype:
    int IANNUNCIATOR_SetField(IAnnunciator * pIAnnunciator, uint32 nAnnunID, uint32 nState)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object
    nAnnunID      : Field ID of the annunciator to set
    nState        : State of the annunciator to set
                    The state of the annunciator can be the following
                       ANNUN_STATE_OFF    - Annunciator turned off
                       ANNUN_STATE_ON     - Annunciator turned on
                    It is also possible to set the state to a specific value depending on
                    the field.
                    The annunciator can be made to blink by a logical OR of nState
                    with ANNUN_STATE_BLINK. The blinking is stopped when a subsequent call
                    to SetField doesn't have the ANNUN_STATE_BLINK set or when the state is
                    ANNUN_STATE_OFF.

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    None

Side Effects:
    None

See Also:
    IANNUNCIATOR_GetField()


=======================================================================

IANNUNCIATOR_SetFieldEx()

Description:
    This function sets the value of an annunciator field to the specified data (for e.g. text)

Prototype:
    int IANNUNCIATOR_SetFieldEx(IAnnunciator * pIAnnunciator, int nAnnunID, void* pData)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object
    nAnnunID      : Field ID of the annunciator to set
    pData         : Pointer to data.

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    None

Side Effects:
    None

See Also:


=======================================================================

IANNUNCIATOR_Redraw()

Description:
    This function forces a redraw of the annunciator on all displays
    where it appears.

Prototype:
    int IANNUNCIATOR_Redraw(IAnnunciator *pMe)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful
    EBADPARM: If an argument was invalid

Comments:
    None

Side Effects:
    None

See Also:


=======================================================================

IANNUNCIATOR_EnableAnnunciatorBar()

Description:
    This function enables or disables the drawing of the annunciator bar to the specified screen.

Prototype:
    int IANNUNCIATOR_EnableAnnunciatorBar(IAnnunciator * pIAnnunciator, AEECLSID clsid, boolean bOn)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object
    clsid         : Class ID of the Screen to change the state for
    bOn           : TRUE/Enable annunciator bar, FALSE/Disable annunciator bar.

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    None

Side Effects:
    When reenabling annunciator bar, the annunciator bar bitmap will be copied to the display

See Also:


=======================================================================

IANNUNCIATOR_GetAnnunciatorBarSize()

Description:
    This function retrieves the size of the annunciator bar for the specified screen.
    The origin of the rectangle that is returned is (0,0) and the (dx, dy) of the
    rectangle describes the height/width of the annunciator bar.

Prototype:
    int IANNUNCIATOR_GetAnnunciatorBarSize(IAnnunciator * pIAnnunciator, AEECLSID clsid, AEERect *pRec)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object
    clsid         : Class ID of the Screen to get the size of the annunicator bar for
    pRec          : Pointer the rectangle describing the size of the annunciator bar.

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    None

Side Effects:
    None

See Also:


======================================================================= */


#endif /* ifndef AEEANNUNCIATOR_H */
