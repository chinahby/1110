#ifndef AEETIME_H
#define AEETIME_H
/*=====================================================================
FILE:  AEETime.h

SERVICES:  AEE Time Control Interface

DESCRIPTION:  Base level definitions, typedefs, etc. for AEE Time Control

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

typedef struct _ITimeCtl   ITimeCtl;

enum {ITF_HOUR, ITF_MIN, ITF_SEC};
#ifdef WIN32
typedef unsigned ITField;
#else
typedef int8 ITField;
#endif

#define TP_AUTOREDRAW      0x00000001     // Redraw on SetActive
#define TP_NO_SECONDS      0x00000002     // Do not show the seconds in the COUNTDOWN control..
#define TP_NO_MSECONDS     0x00000004     // Do not show milliseconds in STOPWATCH...
#define TP_NOEDIT_AMPM     0x00000008     // Do not allow them to edit AM/PM...
#define TP_LEFT_JUSTIFY    0x00000010     // Left Justify the Time
#define TP_24_FORMAT       0x00000020


#define TP_OEM_COUNTDOWNCTL_EDITABLE    0x00000001
#define TP_OEM_COUNTDOWNCTL_12_FORMAT   0x00000002

#define TP_OEM_CUSTOM_BG_COLOR   0x00000004

#define GTS_SECS        0x0001
#define GTS_AMPM        0x0002
#define GTS_MSECS       0x0004

//---------------------------------------------------------------------
// Global Constant Declarations
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// Global Data Declarations
//---------------------------------------------------------------------

//=====================================================================
// FUNCTION DECLARATIONS AND INLINE FUNCTION DEFINITIONS
//=====================================================================
//---------------------------------------------------------------------
// ITimeCtl Interface
//---------------------------------------------------------------------

QINTERFACE(ITimeCtl)
{
   DECLARE_IBASE(ITimeCtl)

   DECLARE_ICONTROL(ITimeCtl)

   boolean     (*SetTime)      (ITimeCtl * pITimeCtl, int32 dwMSecs,boolean bIncUpdate);  // Calls ->Redraw...
   int32       (*GetTime)      (ITimeCtl * pITimeCtl);                 // Return MSECS tod..
   void        (*EnableCommand)(ITimeCtl * pITimeCtl, boolean bEnable, uint16 nCmdId);
   void        (*GetTimeString)(ITimeCtl * pITimeCtl, uint32 dwMSecs, AECHAR * pDest, int nSize, uint16 wFlags);
   void        (*SetIncrement) (ITimeCtl * pITimeCtl, uint16 wMins);
   void        (*SetEditField) (ITimeCtl * pITimeCtl, ITField field);
   void        (*SetFont)      (ITimeCtl * pITimeCtl, AEEFont fntNum, AEEFont fntLetter);
   boolean     (*GetFont)      (ITimeCtl * pITimeCtl, AEEFont *pfntNum, AEEFont *pfntLetter);
   void        (*SizeToFit)    (ITimeCtl * pITimeCtl, AEERect *prc);

    void    (*SetOemProperties) (ITimeCtl * po, uint32 dwProp);
    void     (*SetBgColor) (ITimeCtl * po, RGBVAL nBgColor);
};

//---------------------------------------------------------------------
// Access Macros
//---------------------------------------------------------------------

#define ITIMECTL_AddRef(p)                        GET_PVTBL(p,ITimeCtl)->AddRef(p)
#define ITIMECTL_Release(p)                       GET_PVTBL(p,ITimeCtl)->Release(p)
#define ITIMECTL_HandleEvent(p,ec,wp,dw)          GET_PVTBL(p,ITimeCtl)->HandleEvent(p,ec,wp,dw)
#define ITIMECTL_Redraw(p)                        GET_PVTBL(p,ITimeCtl)->Redraw(p)
#define ITIMECTL_SetActive(p,b)                   GET_PVTBL(p,ITimeCtl)->SetActive(p,b)
#define ITIMECTL_IsActive(p)                      GET_PVTBL(p,ITimeCtl)->IsActive(p)
#define ITIMECTL_SetRect(p,prc)                   GET_PVTBL(p,ITimeCtl)->SetRect(p,prc)
#define ITIMECTL_GetRect(p,prc)                   GET_PVTBL(p,ITimeCtl)->GetRect(p,prc)
#define ITIMECTL_SetProperties(p,v)               GET_PVTBL(p,ITimeCtl)->SetProperties(p,v)
#define ITIMECTL_GetProperties(p)                 GET_PVTBL(p,ITimeCtl)->GetProperties(p)
#define ITIMECTL_Reset(p)                         GET_PVTBL(p,ITimeCtl)->Reset(p)
#define ITIMECTL_SetTime(p,d)                     GET_PVTBL(p,ITimeCtl)->SetTime(p,d,FALSE)
#define ITIMECTL_SetTimeEx(p,d,b)                 GET_PVTBL(p,ITimeCtl)->SetTime(p,d,b)
#define ITIMECTL_GetTime(p)                       GET_PVTBL(p,ITimeCtl)->GetTime(p)
#define ITIMECTL_EnableCommand(p,b,id)            GET_PVTBL(p,ITimeCtl)->EnableCommand(p,b,id)
#define ITIMECTL_GetTimeString(p,dw,pDest,n,wf)   GET_PVTBL(p,ITimeCtl)->GetTimeString(p,dw,pDest,n,wf)
#define ITIMECTL_SetIncrement(p, w)               GET_PVTBL(p,ITimeCtl)->SetIncrement(p,w)
#define ITIMECTL_SetEditField(p, f)               GET_PVTBL(p, ITimeCtl)->SetEditField(p,f)
#define ITIMECTL_SetFont(p, f1, f2)               GET_PVTBL(p, ITimeCtl)->SetFont(p,f1,f2)
#define ITIMECTL_GetFont(p, f1, f2)               GET_PVTBL(p, ITimeCtl)->GetFont(p,f1,f2)
#define ITIMECTL_SizeToFit(p, f1)                 GET_PVTBL(p, ITimeCtl)->SizeToFit(p,f1)

#define ITIMECTL_SetOemProperties(p,v)            GET_PVTBL(p,ITimeCtl)->SetOemProperties(p,v)
#define ITIMECTL_SetBgColor(p,n)                   GET_PVTBL(p,ITimeCtl)->SetBgColor(p,n)

#endif    // AEETIME_H
/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================
ITField

Description:
Field in the time control. There are three fields in each time control, shown as
following.

Definition:
typedef enum {ITF_HOUR, ITF_MIN, ITF_SEC} ITField;

Members:

ITF_HOUR : The hour field of the time control
ITF_MIN : The minute field of the time control
ITF_SEC : The second field of the time control


Comments:
None

See Also:
None

=======================================================================
AEE ITimeCtl Properties

Description:
These are the properties defined for ITimeCtl Interface.

Definition:
TP_AUTOREDRAW:    if set, redraw on SetActive
TP_NO_SECONDS:    if set, do not show the seconds in the COUNTDOWN control
TP_NO_MSECONDS:   if set, do not show milliseconds in STOPWATCH
TP_NOEDIT_AMPM:   if set, do not allow to edit AM/PM
TP_LEFT_JUSTIFY:  if set, text will be drawn left justified (default is center)

Members:
None

Comments:
None

See Also:
ITIMECTL_SetProperties() ,
ITIMECTL_GetProperties()

=============================================================================

    CtlValChange

Description:
    This data structure is passed as dwParam along with the event EVT_CTL_CHANGING.
This event is sent to the application when a specific control is changing. This
allows the application to redraw any portions of the screen.

Definition:
    typedef struct _CtlValChange {
    IControl * pc;
    uint32 dwParam;
    boolean bValid;
    } CtlValChange;

Members:
    pc :  Pointer to the control that is changing
    dwParam :  This is control-specific data. In the case of the ITimeCtl Interface,
               this specifies the current time in milliseconds
    bValid :  Parameter that can be set by the application on returning from this
              event. If bValid is set to FALSE by the application, the control will
              not be redrawn based on the new value

Comments:
    This event is sent by specific controls only; for example, TimeCtl. See the specific
controls for more information on this event

See Also:
    None

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

ITimeCtl Interface

Time controls allow the device user to enter a time value in several different formats.
Time controls can also be used to display a time value to the device user. When
entering a time value, the device user presses the LEFT and RIGHT keys to select
the time field that is to be modified (that is, hours, minutes, or seconds), and
presses the UP and DOWN keys to increase or decrease the value of the edited field.
When the device user presses the SELECT key and command sending is enabled (see
later in this section), an EVT_COMMAND event is sent to the application or dialog
that created the time control, which signals to your application that the device
user has completed entry of a time value. You can also specify the time value that
is displayed in the time control. This feature can be used to repeatedly update
the control to reflect a changing time value. For example, to implement a stopwatch,
the time control does not measure time itself by setting timers or accessing the
current time of day, so you must obtain the necessary time values in your code
using functions like GETTIMEMS().

This control sends the EVT_CTL_CHANGING event to the current application when the
value of the time in the ITimeCtl changes. The dwParam of this event points to a
CtlValChange structure.

Following are the three types of time controls, each of which displays the time
in a different format (you select the type you want by specifying its ClassID when
you create an instance of the time control):

 -    A clock control (ClassID AEECLSID_CLOCKCTL) displays the number of hours and
minutes along with an AM/PM indicator. This type of control can be used to specify
the time at which an alarm clock is to go off.

 -    A countdown control (ClassID AEECLSID_COUNTDOWNCTL) displays the number of
hours, minutes, and seconds. This display format is useful for a countdown timer
that allows the device user to set the timer duration and then displays the time
remaining until the timer expires.

 -    A stopwatch control (ClassID AEECLSID_STOPWATCHCTL) displays the number of
hours, minutes, seconds and hundredths of seconds. It can be used to implement a
stopwatch.

A time control sends a control tabbing event (EVT_CTL_TAB) when the device user
presses the LEFT and RIGHT keys while editing the LEFT-most and RIGHT-most fields
in the time control, respectively. Use control tabbing to move between controls
in a multi-control screen (if your time control is part of a dialog, the dialog intercepts
the control tabbing events and changes control focus appropriately).

The device user selects the field to be edited with the LEFT and RIGHT arrow keys,
so ITIMECTL_SetEditField() can be used to select the initial field to be edited.
===H2>
To create and use a time control
===/H2>

1.   Call ISHELL_CreateInstance() to create the time control instance and obtain
an interface pointer to it, specifying which of the three types of time control
you desire by its ClassID.

2.   Call ITIMECTL_SetTime() to specify an initial time value for the time control
if one is specified.

3.   Call ITIMECTL_SetRect() to define the screen rectangle in which the time control
is to be drawn.

4.   Call ITIMECTL_SetProperties() to set any of the time control properties. Call
ITIMECTL_SetIncrement() or ITIMECTL_SetEditField() to change the default minutes
increment or initial edit field.

5.   When you have completely specified the contents and properties of the time
control, call ITIMECTL_SetActive() to draw the control on the screen and (if your
control supports editing by the device user) enable it to receive key events from
the device user to select a time value. While the time control is active, your application's
IAPPLET_HandleEvent() function must call ITIMECTL_HandleEvent() to pass all handled
key events to the time control for processing.

6.   To display a changing time value to the device user, call ITIMECTL_SetTime()
or ITIMECTL_SetTimeEx() to update the display each time the value changes.

7.   To access the time value entered by the device user, call ITIMECTL_GetTime()
when the device user has entered the value (the device user can signal completion
by pressing the SELECT key or dismissing the screen that contains the time control).
Call ITIMECTL_GetTimeString() to convert the number of milliseconds into a printable
form.

8.   When you no longer need the time control, call ITIMECTL_Release() to free it.

The following header file is required:~
AEETime.h

=============================================================================

ITIMECTL_AddRef()

Description:
    This function is inherited from IBASE_AddRef().

Version:
   Introduced BREW Client 1.0

See Also:
   ITIMECTL_Release()

=============================================================================

ITIMECTL_EnableCommand()

Description:
    This function is used to enable sending of specified command by the time control
object to the AEE Shell upon receiving the event generated by pressing the center key.

Prototype:
    void ITIMECTL_EnableCommand
    (
    ITimeCtl * pITimeCtl,
    boolean bEnable,
    uint16 nCmdId
    )

Parameters:
    pITimeCtl  :  Pointer to I the ITimeCtl Interface object.
    bEnable  :  Boolean value for enable/disable flag.
    nCmdId  :  Command ID.

Return Value:
    None

Comments:
    This function is not supported for Stopwatch view of ITimeCtl Interface.

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITIMECTL_GetFont()

Description:
    This method allows the caller to get the time number and letter fonts.

Prototype:
    boolean ITimeCTL_GetFont
    (
    ITimeCtl * pITimeCtl,
    AEEFont *pfntNum,
    AEEFont *pfntLetter
    )

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object
    pfntNum :  pointer to location to set to number font
    pfntLetter :  pointer to location to set to letter (AM/PM) font

Return Value:
    TRUE: if successful
    FALSE: if unsuccessful

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    AEEFont
    ITIMECTL_SetFont()

=============================================================================

ITIMECTL_GetProperties()

Description:
    This function returns the time control-specific properties or flags.

Prototype:
    uint32 ITIMECTL_GetProperties(ITimeCtl * pITimeCtl)

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.

Return Value:
    32-bit properties for the time control.~
    Following properties are returned by time control object:~
~
    TP_AUTOREDRAW, if set, control redraws on ITIMECTL_SetActive().~
    TP_NO_SECONDS, if set, control doesn’t show the seconds in the COUNTDOWN control.~
    TP_NO_MSECONDS, if set, control doesn’t show milliseconds in STOPWATCH.~
    TP_NOEDIT_AMPM, if set, control doesn’t allow to edit AM/PM.~
    TP_LEFT_JUSTIFY, If set, text will be drawn left justified (default is center).~
*

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    ITIMECTL_SetProperties()

=============================================================================

ITIMECTL_GetRect()

Description:
    This function fills the given pointer to AEERect with the coordinates of the bounding
rectangle of the time control object. This is particularly useful after a control
is created to determine its optimal/default size and position.

Prototype:
    void ITIMECTL_GetRect(ITimeCtl * pITimeCtl, AEERect * prc)

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.
    prc  :  Rectangle to be filled with the coordinates of the time control.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    AEERect
    ITIMECTL_SetRect()

=============================================================================

ITIMECTL_GetTime()

Description:
    This function gets the time value from the time control object. The time value
is in milliseconds.

Prototype:
    int32 ITIMECTL_GetTime(ITimeCtl * pITimeCtl)

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.

Return Value:
    Time in milliseconds elapsed since 00:00:00 for this object.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITIMECTL_GetTimeString()

Description:
    This function converts the time in milliseconds to a string in the specified
format.

Prototype:
    void ITIMECTL_GetTimeString
    (
    ITimeCtl * pITimeCtl,
    uint32 dwSecs,
    AECHAR * pDest,
    unsigned int nSize,
    uint16 wFlags
    )

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.
    dwSecs  :  Time in milliseconds to be converted into a string.
    pDest  :  Converted time in a string.
    nSize  :  Size Of pDest In Bytes.
    wFlags  :  Bitmap that specifies the time format to use for the string conversion
the following are supported:
~
     GTS_MSECS: use centiseconds, and display 2 digit centiseconds.~
     GTS_SECS: use seconds.~
     GTS_AMPM: use AM or PM.~
*

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITIMECTL_HandleEvent()

Description:
    This function is used to handle the events received by a time control object.
A time control object handles events received by it only if it is active. The events
processed by the time control object are the pressing the UP, DOWN, LEFT and RIGHT
keys. If command sending is enabled for the time control object, upon receiving
event generated by the press of center key, it sends the command specified by ITIMECTL_EnableCommand()
function as command event to the AEE Shell.

Prototype:
    boolean ITIMECTL_HandleEvent
    (
    ITimeCtl * pITimeCtl,
    AEEEvent evt,
    uint16 wp,
    uint32 dwp
    )

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.
    evt  :  Event code.
    wp  :  16-bit event data.
    dwp  :  32-bit event data.

Return Value:
    TRUE: if the event was processed by the time control.
    FALSE: if otherwise.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITIMECTL_IsActive()

Description:
    This function returns the active/inactive state of the ITimeCtl Interface object.

Prototype:
    boolean ITIMECTL_IsActive(ITimeCtl * pITimeCtl)

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.

Return Value:
    TRUE: if the time control is active.
    FALSE: if otherwise.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITIMECTL_Redraw()

Description:
    This function instructs the time control object to redraw its contents. The
time control object does not redraw its contents every time the underlying data
behind the time control changes. This allows several data updates to occur while
minimizing screen flashes. For example, several changes can be made to the contents
of the time control object with no visible effect until the Redraw function is called.

Prototype:
    boolean ITIMECTL_Redraw(ITimeCtl * pITimeCtl)

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.

Return Value:
    TRUE: if the time control was redrawn.~
    FALSE: if otherwise.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITIMECTL_Release()

Description:
    This function is inherited from IBASE_Release().

Version:
   Introduced BREW Client 1.0

See Also:
   ITIMECTL_AddRef()

=============================================================================

ITIMECTL_Reset()

Description:
    This function instructs the time control to reset (free/delete) its contents
and to immediately leave active/focus mode.

Prototype:
    void ITIMECTL_Reset(ITimeCtl * pITimeCtl)

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    ITIMECTL_SetActive()

=============================================================================

ITIMECTL_SetActive()

Description:
    This function is used to make a time control object active. Only an active time
control object handles the event sent to it. An inactive time control object just
ignores the events.

Prototype:
    void ITIMECTL_SetActive(ITimeCtl * pITimeCtl, boolean bActive)

Parameters:
    pITimeCtl  :  Pointer to t the ITimeCtl Interface object.
    bActive  :  Boolean flag that indicates-
    TRUE- to activate the time control object.
    FALSE- to deactivate the time control object.

Return Value:
    None

Comments:
    The Stopwatch class is always inactive. The Clock and Countdown classes can
use ITIMECTL_SetActive().

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITIMECTL_SetEditField()

Description:
    This function sets specified field for editing.

Prototype:
    void ITIMECTL_SetEditField(ITimeCtl * pITimeCtl, ITField field)

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object
    field  :  Field to be set for editing. Field can be one of the following:
~
    ITF_HOUR~
    ITF_MIN~
    ITF_SEC
*

Return Value:
    None

Comments:
    This function is not supported for Stopwatch view of ITimeCtl Interface. The
Clock view doesn't contain seconds, so do not set ITF_SEC for a CLock view.

Version:
   Introduced BREW Client 1.0

See Also:
    ITField

=============================================================================

ITIMECTL_SetFont()

Description:
    This method allows the caller to set the title and text fonts.

Prototype:
    void ITimeCtl_SetFont
    (
    ITimeCtl * pITimeCtl,
    AEEFont fntNum,
    AEEFont fntLetter
    )

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object
    fntNum :  Font to use for the numbers in the time. If this parameter is set
to NULL then default font of AEE_FONT_LARGE will be assumed
    fntLetter :  Font to use for the non-number portions of the time (AM/PM). If
this parameter is set to NULL then default font of AEE_FONT_BOLD will be assumed

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    AEEFont
    ITIMECTL_GetFont()

=============================================================================

ITIMECTL_SetIncrement()

Description:
    This function sets a new time increment value in minutes. The default is 1.

Prototype:
    void ITIMECTL_SetIncrement(ITimeCtl * pITimeCtl, uint16 wMins)

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.
    wMins  :  New time increment value in minutes.

Return Value:
    None

Comments:
    The increment for only the minute field can be set. All other fields cannot
be set. This function is not supported for Stopwatch view of ITimeCtl Interface.

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITIMECTL_SetProperties()

Description:
    This function sets time control-specific properties.

Prototype:
    void ITIMECTL_SetProperties(ITimeCtl * pITimeCtl, uint32 dwProps)

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.
    dwProps  :  32-bit set of flags/properties. 

The following properties are usedfor time control object -

    TP_AUTOREDRAW - If set, redraw on ITIMECTL_SetActive().
    TP_NO_SECONDS - If set, do not show the seconds in the COUNTDOWN control.
    TP_NO_MSECONDS - If set, do not show milliseconds in STOPWATCH.
    TP_NOEDIT_AMPM - If set, do not allow to edit AM/PM.
    TP_LEFT_JUSTIFY - If set, text will be drawn left justified
    (default is center).

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    ITIMECTL_GetProperties()

=============================================================================

ITIMECTL_SetRect()

Description:
    This function can be used to set the coordinates specified by prc as the control
rectangle of the time control object.

Prototype:
    void ITIMECTL_SetRect(ITimeCtl * pITimeCtl, const AEERect * prc)

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.
    prc  :  Bounding rectangle for the time control.

Return Value:
    None

Comments:
    By default, entire device screen is set as the control rectangle of the time
control object.

Version:
   Introduced BREW Client 1.0

See Also:
    AEERect
    ITIMECTL_GetRect()

=============================================================================

ITIMECTL_SetTime()

Description:
    This function can be used to set given time and redraw time string on device
screen. It is the same as calling ITIMECTL_SetTimeEx with the third parameter to
be FALSE.

Prototype:
    boolean ITIMECTL_SetTime(ITimeCtl * pITimeCtl, int32 tod)

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.
    tod  :  Time in milliseconds expired since 00:00:00. Negative value is ignored.

Return Value:
    TRUE: if input time is valid.~
    FALSE: if input time is invalid.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITIMECTL_SetTimeEx()

Description:
    This function can be used to set a given time and redraw a time string on a
device screen.

Prototype:
    boolean ITIMECTL_SetTimeEx
    (
    ITimeCtl * pITimeCtl,
    int32 tod,
    boolean bIncUpdate
    )

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object.
    tod  :  Time in milliseconds expired since 00:00:00, where negative values are
ignored.
    bIncUpdate  :  Boolean flag:
     TRUE means to save the time string in the buffer of time control object and
draw on screen.
     FALSE means to just draw on screen and don’t save the time string.
    Whether TRUE or FALSE, the time integer is saved.

Return Value:
    TRUE: if input time is valid.~
    FALSE: if input time is invalid.

Comments:
    The bIncUpdate parameter affects only the time string of the ITimeCtl Interface
object. Time integer is always saved.

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITIMECTL_SizeToFit()

Description:
    This method allows the caller to size the rectangle to the time text.

Prototype:
    void ITimeCTL_SizeToFit(ITimeCtl * pITimeCtl, AEERect *prc);

Parameters:
    pITimeCtl  :  Pointer to the ITimeCtl Interface object
    prc :  Pointer to location to set to bounding rectangle

Return Value:
    none

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    AEERect

===========================================================================*/
