#ifndef AEEANNUNCIATOR_H
#define AEEANNUNCIATOR_H
/*=====================================================================
FILE:  AEEAnnunciator.h

SERVICES:  AEE Annunciator Interface

DESCRIPTION:  Base level definitions, typedefs, etc. for AEE Annunciator

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEEAnnunciator.h#13 $

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
#define ANNUN_FIELD_OPERATOR  0   /* PLMN in GSM; other network info in CDMA */
#define ANNUN_FIELD_MSG       1
#define ANNUN_FIELD_NET       2
#define ANNUN_FIELD_ALARM     3
#define ANNUN_FIELD_SECURITY  4
#define ANNUN_FIELD_MODE      5
#define ANNUN_FIELD_ROAM      6
#define ANNUN_FIELD_DOMAIN    7
#define ANNUN_FIELD_RSSI      8
#define ANNUN_FIELD_INUSE     9
#define ANNUN_FIELD_BATT      10
#define ANNUN_FIELD_TIME      11
#define ANNUN_FIELD_BANNER    12
#define ANNUN_FIELD_MMS       13
#define ANNUN_FIELD_HDR_RSSI  14
#define ANNUN_FIELD_WLAN_RSSI 15
#define ANNUN_FIELD_VT        16
#define ANNUN_FIELD_HSDPA     17  //Field to indicate HSDPA/HSUPA service
#define ANNUN_FIELD_BCMCS     18  //BCMCS annunciator


// Possible State of each annunciator field
#define ANNUN_STATE_OFF       0
#define ANNUN_STATE_ON        1

// Mask to get/set the blink bit in the state
#define ANNUN_STATE_BLINK         0x80000000

// Mask to strip the blink bit from the state
#define ANNUN_STATE_BLINK_UNMASK  0x7FFFFFFF

// Field specific custom states

// Mode field
#define ANNUN_STATE_ANALOG          1
#define ANNUN_STATE_CDMA            2
#define ANNUN_STATE_GSM             3
#define ANNUN_STATE_WCDMA           4
#define ANNUN_STATE_HDR             5
#define ANNUN_STATE_HYBRID          6
#define ANNUN_STATE_HDR_REV_A       7
#define ANNUN_STATE_HYBRID_REV_A    8
#define ANNUN_STATE_SIMULTANEOUS    9
#define ANNUN_STATE_GPS             10 //Not used currently
#define ANNUN_STATE_DIGITAL         11

// Domain field
#define ANNUN_STATE_CS        1
#define ANNUN_STATE_PS        2
#define ANNUN_STATE_CS_PS     3

// RSSI and HDR fields
#define ANNUN_STATE_RSSI_0        1
#define ANNUN_STATE_RSSI_1        2
#define ANNUN_STATE_RSSI_2        3
#define ANNUN_STATE_RSSI_3        4
#define ANNUN_STATE_RSSI_4        5

// In Use field
#define ANNUN_STATE_IN_USE    1
#define ANNUN_STATE_NO_SVC    2

// Alarm field
#define ANNUN_STATE_ALERT_ON  1
#define ANNUN_STATE_SILENCE   2

// Message field

// Battery field
#define ANNUN_STATE_BATT_LOW  1
#define ANNUN_STATE_BATT_1    2
#define ANNUN_STATE_BATT_2    3
#define ANNUN_STATE_BATT_3    4
#define ANNUN_STATE_BATT_FULL 5

//Network field
#define ANNUN_STATE_DORMANT   0
#define ANNUN_STATE_ACTIVE    1

//HSDPA/HSUPA SERVICE field 
#define ANNUN_STATE_HSDPA       1
#define ANNUN_STATE_HSUPA       2
#define ANNUN_STATE_HSDPA_HSUPA 3

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
   void    (*EnableBlink) (IAnnunciator * pMe, boolean bEnable);
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
#define IANNUNCIATOR_EnableBlink(p,b) AEEGETPVTBL((p),IAnnunciator)->EnableBlink((p),(b))

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


=======================================================================

IANNUNCIATOR_EnableBlink()

Description:
    This function enables or disables the blinking of annunciator images.

Prototype:
    int IANNUNCIATOR_EnableBlink(IAnnunciator * pIAnnunciator, boolean bEnable)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object
    bOn           : TRUE/Enable annunciator blink feature.

Return Value:

Comments:
    None

Side Effects:

See Also:


======================================================================= */

#endif /* ifndef AEEANNUNCIATOR_H */
