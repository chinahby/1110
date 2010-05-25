#ifndef AEEDATE_H
#define AEEDATE_H
/*=====================================================================
FILE:  AEEDate.h

SERVICES:  AEE Date Control Interface

DESCRIPTION: Base level definitions, typedefs, etc. for AEE Date Control

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
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
#include "AEEDisp.h"

//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------

typedef struct _IDateCtl      IDateCtl;

enum {
   DOW_SUNDAY =    0,
   DOW_MONDAY,
   DOW_TUESDAY,
   DOW_WEDNESDAY,
   DOW_THURSDAY,
   DOW_FRIDAY,
   DOW_SATURDAY
};
#ifdef CUST_EDITION	
#if defined( FEATURE_JEWISH_CALENDAR)
typedef struct
{
	uint16 year;
	uint16 month;
	uint16 day;
	uint16 weekday;
	byte   holiday;
	boolean leap;

} JewishType;
#endif
#endif /*CUST_EDITION*/
// Date format flags for IDateCtl::GetDateString()

// This group is mutually exclusive
#define DFMT_SLASH_SEP           0x00000010     // "10/15/1999"
#define DFMT_DASH_SEP            0x00000020     // "10-15-1999"
#define DFMT_SPACE_SEP           0x00000030     // "10 15 1999"

#define DFMT_SEP_MASK            0x000000F0

// This group is mutually exclusive
#define DFMT_DD_MONTH_YYYY       0x00000001     // "15 October 1999"
#define DFMT_DD_MON_YYYY         0x00000002     // "15 Oct 1999"
#define DFMT_DD_MON_YY           0x00000003     // "15 Oct '99"
#define DFMT_MONTH_DD_YYYY       0x00000004     // "October 15, 1999"
#define DFMT_MON_DD_YYYY         0x00000005     // "Oct 15, 1999"
#define DFMT_MON_DD_YY           0x00000006     // "Oct 15, '99"
#define DFMT_MM_DD_YYYY          0x00000007     // "10/15/1999"  
#define DFMT_DD_MM_YYYY          0x00000008     // "15/10/1999
#define DFMT_YYYY_MM_DD          0x00000009     // "2001/6/1"
#define DFMT_INT_YYYY_MM_DD      0x0000000a     // "2001.6.1"

#define DFMT_FORMAT_MASK         0x0000000F

#define ABBREV_LEN   3

//---------------------------------------------------------------------
// Global Constant Declarations
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// Global Data Declarations
//---------------------------------------------------------------------

#define DP_CENTERED    (0x00010000L)  // Center date in rect

//=====================================================================
// FUNCTION DECLARATIONS AND INLINE FUNCTION DEFINITIONS
//=====================================================================
//---------------------------------------------------------------------
// IDateCtl Interface
//---------------------------------------------------------------------

QINTERFACE(IDateCtl)
{
   DECLARE_IBASE(IDateCtl)

   DECLARE_ICONTROL(IDateCtl)

   boolean        (*SetTitle)(IDateCtl * pIDateCtl, const char * pszResFile, uint16 wResID, AECHAR * pText);
   boolean        (*SetJulianDay)(IDateCtl * pIDateCtl, int32 lJulianDate);
   int32          (*GetJulianDay)(IDateCtl * pIDateCtl);
   boolean        (*SetDate)(IDateCtl * pIDateCtl, int nYear, int nMonth, int nDay);
   boolean        (*GetDate)(IDateCtl * pIDateCtl, int * pnYear, int * pnMonth, int * pnDay);
   uint16         (*GetDayOfWeek)(IDateCtl * pIDateCtl);    // returns 0=Sunday...6=Saturday
   AECHAR *       (*GetDayString)(IDateCtl * pIDateCtl, AECHAR * pBuffer, int nMaxChars, int * pnChars);
   AECHAR *       (*GetMonthString)(IDateCtl * pIDateCtl, AECHAR * pBuffer, int nMaxChars, int * pnChars);
   boolean        (*GetDateString)(IDateCtl * pIDateCtl, AECHAR * pBuffer, int nMaxChars, int * pnChars, uint32 dwDateFormat);
   void           (*EnableCommand)(IDateCtl * pIDateCtl, boolean bEnable, uint16 nCmdId);
   void           (*SetActiveDayMask)(IDateCtl * pIDateCtl, uint32 dwMask);
   void           (*SetFont)(IDateCtl * po, AEEFont fntText, AEEFont fntTitle);
   boolean        (*GetFont)(IDateCtl * po, AEEFont *pfntText, AEEFont *pfntTitle);
   void           (*SizeToFit)(IDateCtl * po, AEERect *prc);
#ifdef CUST_EDITION	   
   void           (*SetToday)(IDateCtl * pme, int32 lJulDate);
   boolean        (*SetJRange)(IDateCtl * po, int32 StartJulDate, int32 EndJulDate);
   void           (*EnabeRange)(IDateCtl * po, boolean ActRange);
   
#if defined( FEATURE_JEWISH_CALENDAR)
   void           (*GetJewishDate)(IDateCtl * pIDateCtl, JewishType* pjewish);
#endif
#endif /*CUST_EDITION*/

};

//---------------------------------------------------------------------
// Access Macros
//---------------------------------------------------------------------

#define IDATECTL_AddRef(p)                            GET_PVTBL((p),IDateCtl)->AddRef((p))
#define IDATECTL_Release(p)                           GET_PVTBL((p),IDateCtl)->Release((p))
#define IDATECTL_HandleEvent(p,ec,wp,dw)              GET_PVTBL((p),IDateCtl)->HandleEvent((p),(ec),(wp),(dw))
#define IDATECTL_Redraw(p)                            GET_PVTBL((p),IDateCtl)->Redraw((p))
#define IDATECTL_SetActive(p,b)                       GET_PVTBL((p),IDateCtl)->SetActive((p),(b))
#define IDATECTL_IsActive(p)                          GET_PVTBL((p),IDateCtl)->IsActive((p))
#define IDATECTL_SetRect(p,prc)                       GET_PVTBL((p),IDateCtl)->SetRect((p),(prc))
#define IDATECTL_GetRect(p,prc)                       GET_PVTBL((p),IDateCtl)->GetRect((p),(prc))
#define IDATECTL_SetProperties(p,props)               GET_PVTBL((p),IDateCtl)->SetProperties((p),(props))
#define IDATECTL_GetProperties(p)                     GET_PVTBL((p),IDateCtl)->GetProperties((p))
#define IDATECTL_Reset(p)                             GET_PVTBL((p),IDateCtl)->Reset((p))
#define IDATECTL_SetTitle(p,pr,id,psz)                GET_PVTBL((p),IDateCtl)->SetTitle((p),(pr),(id),(psz))
#define IDATECTL_SetJulianDay(p,d)                    GET_PVTBL((p),IDateCtl)->SetJulianDay((p),(d))
#define IDATECTL_GetJulianDay(p)                      GET_PVTBL((p),IDateCtl)->GetJulianDay((p))
#define IDATECTL_SetDate(p,y,m,d)                     GET_PVTBL((p),IDateCtl)->SetDate((p),(y),(m),(d))
#define IDATECTL_GetDate(p,py,pm,pd)                  GET_PVTBL((p),IDateCtl)->GetDate((p),(py),(pm),(pd))
#define IDATECTL_GetDayOfWeek(p)                      GET_PVTBL((p),IDateCtl)->GetDayOfWeek((p))
#define IDATECTL_GetDayString(p,psz,max,pc)           GET_PVTBL((p),IDateCtl)->GetDayString((p),(psz),(max),(pc))
#define IDATECTL_GetMonthString(p,psz,max,pc)         GET_PVTBL((p),IDateCtl)->GetMonthString((p),(psz),(max),(pc))
#define IDATECTL_GetDateString(p,psz,max,pc,format)   GET_PVTBL((p),IDateCtl)->GetDateString((p),(psz),(max),(pc),(format))
#define IDATECTL_EnableCommand(p,ben,id)              GET_PVTBL((p),IDateCtl)->EnableCommand((p),(ben),(id))
#define IDATECTL_SetActiveDayMask(p,dw)               GET_PVTBL((p),IDateCtl)->SetActiveDayMask((p),(dw))
#define IDATECTL_SetFont(p,f1,f2)                     GET_PVTBL((p),IDateCtl)->SetFont((p),(f1),(f2))
#define IDATECTL_GetFont(p,f1,f2)                     GET_PVTBL((p),IDateCtl)->GetFont((p),(f1),(f2))
#define IDATECTL_SizeToFit(p,f1)                      GET_PVTBL((p),IDateCtl)->SizeToFit((p),(f1))
#ifdef CUST_EDITION	
#define IDATECTL_SetToday(p,f1)                       GET_PVTBL(p,IDateCtl)->SetToday(p,f1)
#define IDATECTL_SetDateRange(p,f1,f2)                GET_PVTBL(p,IDateCtl)->SetJRange(p,f1,f2)
#define IDATECTL_EnableDateRange(p,f1)                GET_PVTBL(p,IDateCtl)->EnabeRange(p,f1)

#if defined( FEATURE_JEWISH_CALENDAR)
#define IDATECTL_GetJewishDate(p,pj)            GET_PVTBL((p),IDateCtl)->GetJewishDate((p),(pj))
#endif
#endif /*CUST_EDITION*/
#endif    // AEEDATE_H

/*=============================================================================
   INTERFACE DOCUMENTATION
===============================================================================

IDateCtl Interface

Date controls are used in interfaces that require the device user to choose a date.
There are two types of date controls (you choose the type you want by specifying
its ClassID when you create an instance of the date control):

 -    A standard date control (ClassID AEECLSID_DATECTL) allows the device user
to use the UP, DOWN, LEFT, and RIGHT keys to choose the specific month, day, and year.

 -    A date pick control (ClassID AEECLSID_DATEPICKCTL) displays a monthly calendar;
the arrow keys can be used to choose the day of the month, or to scroll to the next
or previous month.

IDATECTL_HandleEvent() handles the AVK_SELECT, AVK_UP, AVK_DOWN, AVK_LEFT and AVK_RIGHT
keys. When it receives AVK_SELECT, the date control sends an EVT_COMMAND to signal
your application that the device user has selected a date, provided you have enabled
command sending (see later in this section); then use one of the IDateCtl functions
to retrieve the selected date value. For standard date controls, control tabbing
events (EVT_CTL_TAB) are sent when the device user presses AVK_LEFT while selecting
the month or AVK_RIGHT while selecting the year. Use these events to allow the device
user to navigate between controls in a multi-control screen (dialogs handle EVT_CTL_TAB
events and change control focus as needed).

At present, there are no properties specific to date controls, so the functions
IDATECTL_SetProperties() and IDATECTL_GetProperties() are not used.

IDateCtl Interface implements several functions in addition to those in the IControl
Interface.

 -    IDATECTL_SetTitle() specifies a title that appears above the date control.

 -    IDATECTL_SetDate() sets the date stored in the control to the value specified
in the function's integer month, day, and date parameters.

 -    IDATECTL_GetDate() retrieves the control's current date in the same format.

 -    IDATECTL_SetJulianDay() and IDATECTL_GetJulianDay() are similar, except that
the control's date is set and retrieved in number of days since the first Sunday
before January 1, 1970.

 -    IDATECTL_GetDayOfWeek() returns the day of the week corresponding to the control's
current date.

 -    The date control's current date, day, or month can also be stored into a character
string with the functions IDATECTL_GetDateString(), IDATECTL_GetDayString(), and
IDATECTL_GetMonthString(). The date string can be in any of several different formats
(for example, specify the order of the date, month, and year, and whether the full
month name or a three-letter abbreviation is used).

 -    For date pick controls, IDATECTL_SetActiveDayMask() causes the specified days
of the month to be displayed with a dot (.) at the upper left hand corner;
this can be used to designate holidays, For example, the function is called with
a 32-bit parameter that specifies which of the days of the month is displayed in
this way.

 -    IDATECTL_EnableCommand() allows the disabling or enabling of command sending
when the device user presses the AVK_SELECT key while the date control is active
(command sending is disabled by default). This function also lets you specify the
command ID (for example, the wParam value that is sent along with the EVT_COMMAND
when your application's IAPPLET_HandleEvent() function is called).

===H2>
To use a date control:
===/H2>
===p>
1.   Call ISHELL_CreateInstance() to create an instance of a date control, specifying
the ClassID of either a standard or date pick date control.

2.   Call IDATECTL_SetRect() to specify the screen rectangle that contains the control.

3.   Call IDATECTL_SetDate() or IDATECTL_SetJulianDay() to specify an initial date
value for the control if necessary (if you do not specify one, the control's date
fields appear blank initially).

4.   Call IDATECTL_SetActive() to make the control active so that it can receive
key events generated as the device user chooses a date value. Your application must
send the control these events using the IDATECTL_HandleEvent() while the control
is active.

5.   When the device user has chosen a date, call one of the date retrieval functions
mentioned above to access its value in the specified format. The device user's selection
of a date may be signaled through an EVT_COMMAND event if command sending is enabled,
or by the device user exiting the screen that contains the date control.

6.   When you no longer need the date control, call IDATECTL_Release() to release
it.

The following header file is required:

AEEDate.h

=============================================================================

IDATECTL_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

See Also:
    IDATECTL_Release()

=============================================================================

IDATECTL_EnableCommand()

Description:
    This function is used to enable the date control object to send a user-defined
command to the active applet. If bEnable is TRUE, upon receiving the event generated
by press of select key, the date control object sends nCmdId as EVT_COMMAND to the
active applet. Handling of this event is applet-specific and is user-defined.

Prototype:
    void IDATECTL_EnableCommand
    (
    IDateCtl * pIDateCtl,
    boolean bEnable,
    uint16 nCmdId
    )

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    bEnable:  Boolean value for enable flag.
    nCmdId:  Command ID.

Return Value:
    None

Comments:
    This function can be used to send a user-defined command from the date control
object to the active applet.

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

IDATECTL_GetDate()

Description:
    This function gets the date from the date control object.

Prototype:
    boolean IDATECTL_GetDate
    (
    IDateCtl * pIDateCtl,
    unsigned int * pnYear,
    unsigned int * pnMonth,
    unsigned int * pnDay
    )

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    pnYear:  Placeholder for year [YYYY] for example, 2000.
    pnMonth:  Placeholder for month [MM] for example, 12.
    pnDay:  Placeholder for day [DD] for example, 31.

Return Value:
    TRUE, if successful.

    FALSE, If unsuccessful.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_SetDate()

=============================================================================

IDATECTL_GetDateString()

Description:
    This function gets the date string in the specified format.

Prototype:
    boolean IDATECTL_GetDateString
    (
    IDateCtl * pIDateCtl,
    AECHAR * pBuffer,
    unsigned int nMaxSize,
    unsigned int * pnChars,
    uint32 dwDateFormat
    )

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    pBuffer:  Placeholder for date string.
    nMaxSize:  Size of the input buffer (pBuffer) in bytes
    pnChars:  Placeholder for the number of characters written in pBuffer.
    dwDateFormat:  Format of the date string. Use one of the date string formats
    given -
===pre>
      DFMT_DD_MONTH_YYYY:  18 July 2000
      DFMT_DD_MON_YYYY  :  18 Jul 2000
      DFMT_DD_MON_YY    :  18 Jul ‘00
      DFMT_MONTH_DD_YYYY:  July 18, 2000
      DFMT_MON_DD_YYYY  :  Jul. 18, 2000
      DFMT_MON_DD_YY    :  Jul 18, ‘00
      DFMT_MM_DD_YYYY   :  10/15/2000 
      DFMT_DD_MM_YYYY   :  15/10/2001
      DFMT_YYYY_MM_DD   :  2001/6/1
      DFMT_INT_YYYY_MM_DD:  2001.06.01
===/pre>
Return Value:
    TRUE, if successful.

    FALSE, if unsuccessful.

Comments:
    The date string formats for parameter dwDateFormat are mutually exclusive.

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_GetDayString()
    IDATECTL_GetMonthString()

=============================================================================

IDATECTL_GetDayOfWeek()

Description:
    This function gets the day of week from the date control object.

Prototype:
    uint16 IDATECTL_GetDayOfWeek(IDateCtl * pIDateCtl)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.

Return Value:
    One of the following:~
~
    DOW_SUNDAY~
    DOW_MONDAY~
    DOW_TUESDAY~
    DOW_WEDNESDAY~
    DOW_THURSDAY~
    DOW_FRIDAY~
    DOW_SATURDAY
*

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

IDATECTL_GetDayString()

Description:
    This function gets the name of the day corresponding to the date control object’s
current date.

Prototype:
    AECHAR * IDATECTL_GetDayString
    (
    IDateCtl * pIDateCtl,
    AECHAR * pBuffer,
    unsigned int nMaxSize,
    unsigned int * pnChars
    )

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    pBuffer:  Placeholder for day name.
    nMaxSize:  Size of the input buffer (pBuffer) in bytes.
    pnChars:  Placeholder for number of characters written in pBuffer.

Return Value:
    Pointer, to the end of day string in pBuffer, if successful.

    NULL, if unsuccessful.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_GetMonthString()
    IDATECTL_GetDateString()

=============================================================================

IDATECTL_GetFont()

Description:
    This method allows the caller to get the title and text fonts.

Prototype:
    boolean IDATECTL_GetFont
    (
    IDateCtl * pIDateCtl, 
    AEEFont *pfntText, 
    AEEFont *pfntTitle
    )

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object
    pfntText:  Pointer to location to set to text font
    pfntTitle:  Pointer to location to set to title font

Return Value:
    TRUE, if successful.

    FALSE, if unsuccessful

Comments:
    The following changes were made to IDisplay:
         1) IDisplay multiple instances:: For BREW 2.1, IDisplay supports multiple
instances. This means that each call to ISHELL_CreateInstance() with AEECLSID_DISPLAY
will return an application-global IDisplay instance which maintains state information
such as fonts, colors, destination bitmap and a clipping rectangle (new). Furthermore,
IDisplay's can be cloned, and then modified, so that different parts of an applet
can draw to separate IDisplay's without the need to restore state data after every
draw operation. To support this behavior, IDISPLAY_Clone() was added to the IDisplay
interface. In addition, a new ClassID, AEECLSID_DISPLAYCLONE, was added to produce
a copy of the current application-global IDisplay.
         2) IDisplay clipping rectangle:: IDISPLAY_SetClipRect() was added to
set a clipping rectangle that all IDisplay drawing functions will honor. To reset
the clipping rectangle to the full screen, pass NULL in the (AEERect*) argument.

Version:
   Introduced BREW Client 2.0

See Also:
    IDATECTL_SetFont()

=============================================================================

IDATECTL_GetJulianDay()

Description:
    This function gets the Julian day value of the specified date control object.

Prototype:
    int32 IDATECTL_GetJulianDay(IDateCtl * pIDateCtl)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.

Return Value:
    Julian day value of date control object

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_SetJulianDay()

=============================================================================

IDATECTL_GetMonthString()

Description:
    This function gets the name of the month of the date control object’s current
date.

Prototype:
    AECHAR * IDATECTL_GetMonthString
    (
    IDateCtl * pIDateCtl,
    AECHAR * pBuffer,
    unsigned int nMaxSize,
    unsigned int * pnChars
    )

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    pBuffer:  Placeholder for month name.
    nMaxSize:  Size of the input buffer (pBuffer) in bytes.
    pnChars:  Placeholder for number of characters written in pBuffer.

Return Value:
    Pointer to the end of month name string in pBuffer, if successful.

    NULL, if unsuccessful.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_GetDayString()
    IDATECTL_GetDateString()

=============================================================================

IDATECTL_GetProperties()

Description:
    This function returns the date control-specific properties or flags. Presently
there are no date control-specific properties, and this function always returns
0 (zero).

Prototype:
    uint32 IDATECTL_GetProperties(IDateCtl * pIDateCtl)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.

Return Value:
    0 (zero) under all conditions.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_SetProperties()

=============================================================================

IDATECTL_GetRect()

Description:
    This function fills the given pointer to an AEERect structure with the coordinates
of the current bounding rectangle of the date control object. This is particularly
useful after a control is created to determine its optimal/default size and position.

Prototype:
    void IDATECTL_GetRect(IDateCtl * pIDateCtl, AEERect * prc)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    prc:  Rectangle to be filled with the coordinates of the date control object.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    AEERect
    IDATECTL_SetRect()

=============================================================================

IDATECTL_HandleEvent()

Description:
    This function is used to handle the events received by the date control object.
A date control object handles events received by it only if it is active. The events
processed by the date control object are by the pressing of UP, DOWN, LEFT and RIGHT
keys. If command sending is enabled for the date control object, upon receiving
event generated by the press of the SELECT key, it sends the command specified by
IDATECTL_EnableCommand function as command event to the active applet.

Prototype:
    boolean IDATECTL_HandleEvent
    (
    IDateCtl * pIDateCtl,
    AEEEvent evt,
    uint16 wp,
    uint32 dwp
    )

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    evt:  Event code.
    wp:  [AVK_UP|AVK_DOWN|AVK_LEFT|AVK_RIGHT|AVK_SELECT
    dwp:  Key symbol string corresponding to the key type specified by wp.

Return Value:
    TRUE, if the event was processed by the date control.~
    FALSE, if otherwise.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_EnableCommand()

=============================================================================

IDATECTL_IsActive()

Description:
    This function returns whether the date control object is active or not. The
active state is indicated by a return value of TRUE whereas the inactive state is
indicated by a return value of FALSE.

Prototype:
    boolean IDATECTL_IsActive(IDateCtl * pIDateCtl)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.

Return Value:
    TRUE, if the date control is active.

    FALSE, if otherwise.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_SetActive()

=============================================================================

IDATECTL_Redraw()

Description:
    This function instructs the date control object to redraw its contents. The
Date control object does not redraw its contents every time the underlying data
of the date control changes. This allows several data updates to occur while minimizing
screen flashes.

Prototype:
    boolean IDATECTL_Redraw(IDateCtl * pIDateCtl)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.

Return Value:
    TRUE, if the date control was redrawn.

    FALSE, if otherwise.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

IDATECTL_Release()

Description:
    This function is inherited from IBASE_Release(). 

See Also:
	IDATECTL_AddRef()

=============================================================================

IDATECTL_Reset()

Description:
    This function instructs the date control to reset (free/delete) its contents
and to immediately leave active/focus mode.

Prototype:
    void IDATECTL_Reset(IDateCtl * pIDateCtl)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.

Return Value:
    None

Comments:
    This function makes the control inactive. An inactive control doesn’t handle
the events sent to it.

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_SetActive()

=============================================================================

IDATECTL_SetActive()

Description:
    This function is used to make a date control object active. Only an active date
control object handles the events sent to it. An inactive date control object just
ignores the events.

Prototype:
    void IDATECTL_SetActive(IDateCtl * pIDateCtl, boolean bActive)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    bActive:  Boolean flag that specifies whether to activate (TRUE) or deactivate
                (FALSE) the date control

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_IsActive()

=============================================================================

IDATECTL_SetActiveDayMask()

Description:
    This function sets a new active day mask. The active day mask is a 32-bit integer
in which each of 0 (zero) to 30 bits specifies active status of a day. An active
day, having bit corresponding to it set, is drawn with a dot (.) at the upper left
hand corner.

Prototype:
    void IDATECTL_SetActiveDayMask(IDateCtl * pIDateCtl, uint32 dwMask)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    dwMask:  New active day mask.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

IDATECTL_SetDate()

Description:
    This function assigns the specified date to the date control object.

Prototype:
    boolean IDATECTL_SetDate
    (
    IDateCtl * pIDateCtl,
    unsigned int nYear,
    unsigned int nMonth,
    unsigned int nDay
    )

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    nYear:  Year [YYYY] for example, 2000.
    nMonth:  Month [MM] for example, 12.
    nDay:  Day [DD] for example, 31.

Return Value:
    TRUE, if successful.
    FALSE, if unsuccessful.

Comments:
    Minimal error checking: Any day that is between 1 and 31 is valid and any month
that is between 1 and 12 is valid. An input of 4/31/98 becomes 5/1/98.

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_GetDate()

=============================================================================

IDATECTL_SetFont()

Description:
    This method allows the caller to set the title and text fonts.

Prototype:
    void IDATECTL_SetFont
    (
    IDateCtl * pIDateCtl, 
    AEEFont fntText, 
    AEEFont fntTitle
    )

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object
    fntText:  Font to use for the text (if 0, normal font is used)
    fntTitle:  Font to use for the title (if 0, bold font is used)

Return Value:
    None

Comments:
    For invalid font values the function will return with no changes.

Version:
   Introduced BREW Client 2.0

See Also:
    IDATECTL_GetFont()

=============================================================================

IDATECTL_SetJulianDay()

Description:
    This function assigns the specified Julian day to the date control object.

Prototype:
    boolean IDATECTL_SetJulianDay(IDateCtl * pIDateCtl, int32 lJulDate)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    lJulDate:  Julian day to be assigned.

Return Value:
    TRUE, if Julian day input is valid.

    FALSE, if Julian day input is invalid.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_GetJulianDay()

=============================================================================

IDATECTL_SetProperties()

Description:
    This function sets date control-specific properties or flags. The lower 8 bits
of the properties mask specify date format for the control. The default is DFMT_DD_MON_YY.

Prototype:
    void IDATECTL_SetProperties(IDateCtl * pIDateCtl, uint32 dwProps)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    dwProps:  32-bit set of flags/properties.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IDATECTL_GetProperties()

=============================================================================

IDATECTL_SetRect()

Description:
    This function can be used to set the specified rectangle as the bounding rectangle
of the date control object. If the date control object is in month view, the specified
rectangle is also used to determine the grid used for drawing days.

Prototype:
    void IDATECTL_SetRect(IDateCtl * pIDateCtl, const AEERect * prc)

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    prc:  Bounding rectangle for the date control object.

Return Value:
    None

Comments:
    By default, entire device screen is set as the bounding rectangle of the date
control object.

Version:
   Introduced BREW Client 1.0

See Also:
    AEERect
    IDATECTL_GetRect()

=============================================================================

IDATECTL_SetTitle()

Description:
    This function is used to set title of a date control object. If pText is not
NULL, it sets the string specified by pText as the title of the date control object.
If pText is NULL, it reads the title string corresponding to the given resource
identifier from the resource file and sets it as the title of the date control object.

Prototype:
    boolean IDATECTL_SetTitle
    (
    IDateCtl * pIDateCtl,
    const char * pszResFile,
    uint16 wResID, 
    AECHAR * pText
    )

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object.
    pszResFile:  NULL-terminated string containing resource file name.
    wResID:  String resource identifier.
    pText:  NULL-terminated title string.

Return Value:
    TRUE, if successful

    FALSE, if unsuccessful

Comments:
    None

Side Effects:
    If pText is NULL and pszResFile and wResID are valid, this function assigns
the date control object title string to pText.

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

IDATECTL_SizeToFit()

Description:
    This method allows the caller to size the rectangle to the date text

Prototype:
    void IDATECTL_SizeToFit(IDateCtl * pIDateCtl, AEERect *prc);

Parameters:
    pIDateCtl:  Pointer to the IDateCtl Interface object
    prc:  Pointer to location to set to bounding rectangle

Return Value:
    none

Comments:
    This method is implemented only for AEECLSID_DATECTL and has no effect on AEECLSID_DATEPICKCTL.

Version:
   Introduced BREW Client 2.0

See Also:
    AEERect

===========================================================================*/
