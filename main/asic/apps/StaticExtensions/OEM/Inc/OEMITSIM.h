#ifndef OEMITSIM_H
#define OEMITSIM_H
/*=====================================================================
FILE:  OEMITSIM.h

SERVICES:  OEM TOUCH SCREEN INPUT METHOD INTERFACE

DESCRIPTION: Base level definitions, typedefs, etc. for OEM ITSIM Control

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright ?2004-2006 TCL Incorporated.
               All Rights Reserved.
            TCL Proprietary/GTDR
=====================================================================*/
/*=============================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
01/06/2004    ZP     CREATE THIS FILE.Use it in CD01 project
=============================================================================*/

//---------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------

#include "AEE.h"
#include "AEEMenu.h"
#include "AEEText.h"
#include "AEEPen.h"
#include "AEEGraphics.h"
#include "AEEVirtualkey.h"
#include "AEE_OEM.h"
#include "AEEConfig.h"
#include "AEEClipboard.h"

//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------

typedef struct _ITSIM ITSIM;   //the struct of ITSIM interface

#define SEL_BUFFER_SIZE      (512)

#define MAX_TSIM_MODE_SIZE   (5)       // Maximum TOUCH SCREEN INPUT mode name size
#define FONTLEADING_HEIGHT   (7)       // the height of the nFontleading.

typedef enum
{
    TSIM_NONE = 0x00,      //the touch screen mode is none  
    TSIM_HANSTROKE,        //the touch screen hanstroke input method
    TSIM_PINYIN,           //the touch screen pinyin input method
    TSIM_THAI,
    TSIM_MYANMRA,
    TSIM_LETTERS,          //the touch screen letter input method
    TSIM_NUM,
    TSIM_SYMBOLS           //the touch screen symbol input method
}OEMTSIMInputMode;

#define TSIM_MAX          TSIM_SYMBOLS   //the max number of the touch screen inputmodes
#define MAX_TSM_TYPE      TSIM_MAX

//Maximum length of the Mode string. This is used in the Menu.
//#define MAX_MODE_STR      15

// special flag to mark the text control to track properties for.
#define TP_DISPLAY_COUNT  0x10000000

//wait for this time to send the handwrite parameters to hs task.
#define SENDPACKET_WAIT_TIME  1000 

//wait for this time to invoke scrollbar callback function
#define SCROLL_START_WAIT_TIME      600
#define SCROLL_RATE_WAIT_TIME       200

//the number of the symbols in the handwrite inputmode
#define FULLSCREEN_SYMBOL_NUM       11
#define ONERECT_SYMBOL_NUM          9

//the information of the TSIM inputmodes.
typedef struct _OEMTSIMInputModeInfo
{
    OEMTSIMInputMode  tmMode;
    AECHAR            modeString[MAX_TSIM_MODE_SIZE];
}OEMTSIMInputModeInfo;

//enum the function title types.
typedef enum
{
    FUNCTITLE_ONE = 0X00,
    FUNCTITLE_TWO,
    FUNCTITLE_THREE,
    FUNCTITLE_FOUR,
    FUNCTITLE_FIVE,
    FUNCTITLE_SIX,
    FUNCTITLE_BLANK ,
    FUNCTITLE_DELETE,
    FUNCTITLE_BACKSPACE
}Function_type;

#define MAX_FN_TYPE   (9)

//enum the states of the pinyin inputmode.
typedef enum
{
    PY_INPUT_MODE = 0x00,
    PY_SELECT_MODE,
    PY_ASSOC_MODE
}PYstate_type;

//the struct of the coordinate range.
typedef struct
{
    int16 xmin, ymin;
    int16 xmax, ymax;
}CoordiRange;

//the struct of the storke parmet
//add by ydc
typedef struct
{
	unsigned short	wMode;		        // 识别模式
	uint32          FixedColor;         // 颜色

}HanStrokeSet;

// add by Decai.Yang start
#define STROKE_END_X                    (-1)
#define STROKE_END_Y                    (0)
//#define STROKE_WIDTH                    1       //笔划线宽,目前最大值为5
//add end

//add by Decai Yang
AECHAR             *TSIM_TextGet(ITSIM * pITSIM);

#define SETCoordiRange(range, xleft, ytop, xright, ybottom)\
         (range)->xmin = (int16)(xleft);\
         (range)->ymin = (int16)(ytop);\
         (range)->xmax = (int16)(xright);\
         (range)->ymax = (int16)(ybottom);
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
// ITextCtl Interface
//---------------------------------------------------------------------

QINTERFACE(ITSIM)
{
    DECLARE_IBASE(ITSIM)

    DECLARE_ICONTROL(ITSIM)

    boolean           (*SetText)(ITSIM * pITSIM, const AECHAR * psz, int cch);
    boolean           (*GetText)(ITSIM * pITSIM, AECHAR * pBuffer, int nMaxChars);
    AECHAR *          (*GetTextPtr)(ITSIM * pITSIM);
    void              (*SetMaxSize)(ITSIM * pITSIM, uint16 nMaxSize);
    OEMTSIMInputMode  (*SetInputMode)(ITSIM * po, OEMTSIMInputMode m);
    int32             (*GetCursorPos)(ITSIM *po);
    void              (*SetCursorPos)(ITSIM *po, int32 nOffset);
    OEMTSIMInputMode  (*GetInputMode)(ITSIM *po, OEMTSIMInputModeInfo * pmInfo);
    boolean           (*GetSelText)(ITSIM * pITSIM, AECHAR * pBuffer, int nchars);
    boolean           (*SetSelText)(ITSIM * pITSIM, AECHAR * psz, int nchars);
};

//---------------------------------------------------------------------
// Access Macros
//---------------------------------------------------------------------

#define ITSIM_AddRef(p)                            GET_PVTBL(p,ITSIM)->AddRef(p)
#define ITSIM_Release(p)                           GET_PVTBL(p,ITSIM)->Release(p)
#define ITSIM_HandleEvent(p,ec,wp,dw)              GET_PVTBL(p,ITSIM)->HandleEvent(p,ec,wp,dw)
#define ITSIM_Redraw(p)                            GET_PVTBL(p,ITSIM)->Redraw(p)
#define ITSIM_SetActive(p,b)                       GET_PVTBL(p,ITSIM)->SetActive(p,b)
#define ITSIM_IsActive(p)                          GET_PVTBL(p,ITSIM)->IsActive(p)
#define ITSIM_SetRect(p,prc)                       GET_PVTBL(p,ITSIM)->SetRect(p,prc)
#define ITSIM_GetRect(p,prc)                       GET_PVTBL(p,ITSIM)->GetRect(p,prc)
#define ITSIM_SetProperties(p,props)               GET_PVTBL(p,ITSIM)->SetProperties(p,props)
#define ITSIM_GetProperties(p)                     GET_PVTBL(p,ITSIM)->GetProperties(p)
#define ITSIM_Reset(p)                             GET_PVTBL(p,ITSIM)->Reset(p)
#define ITSIM_SetText(p,psz,cch)                   GET_PVTBL(p,ITSIM)->SetText(p,psz,cch)
#define ITSIM_GetText(p,pbuf,max)                  GET_PVTBL(p,ITSIM)->GetText(p,pbuf,max)
#define ITSIM_GetTextPtr(p)                        GET_PVTBL(p,ITSIM)->GetTextPtr(p)
#define ITSIM_SetMaxSize(p,s)                      GET_PVTBL(p,ITSIM)->SetMaxSize(p,s)
#define ITSIM_SetInputMode(p,m)                    GET_PVTBL(p,ITSIM)->SetInputMode(p,m)
#define ITSIM_GetCursorPos(p)                      GET_PVTBL(p,ITSIM)->GetCursorPos(p)
#define ITSIM_SetCursorPos(p,n)                    GET_PVTBL(p,ITSIM)->SetCursorPos(p,n)
#define ITSIM_GetInputMode(p,m)                    GET_PVTBL(p,ITSIM)->GetInputMode(p,m)
#define ITSIM_GetSelText(p,pbuf,nchars)            GET_PVTBL(p,ITSIM)->GetSelText(p,pbuf,nchars)
#define ITSIM_SetSelText(p,psz,nchars)             GET_PVTBL(p,ITSIM)->SetSelText(p,psz,nchars)

#endif// OEMITSIM_H
/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================
OEMTSIMInputMode

Description:
This enumerated type specifies the touch screen input modes that can be used to enter ITSIM
into a ITSIM control. The function ITSIM_SetInputMode() is used to select the touch screen
input mode that is used for a particular ITSIM control instance.

Definition:
typedef enum 
{
   TSIM_NONE
   TSIM_LETTERS
   TSIM_SYMBOLS
   TSIM_PINYIN
   TSIM_HANSTROKE
} OEMTSIMInputMode;

Members:
 TSIM_NONE:the NULL touch screen inputmode just for the TSIM limit
 TSIM_LETTERS:the touch screen letter input method
 TSIM_SYMBOLS:the touch screen symbol input method
 TSIM_PINYIN:the touch screen pinyin input method
 TSIM_HANSTROKE:the touch screen hanstroke input method

Comments:
The available touch screen input modes differ with each BREW-enabled device.

See Also:
ITSIM_SetInputMode()

=====================================================================
OEMTSIMInputModeInfo

Description:
This structure contains the an OEMTSIMInputMode and a buffer to hold the 
string that is associated with that mode to be filled when using the 
ITSIM_GetInputMode API.

Definition:
typedef struct _OEMTSIMInputModeInfo
{
   OEMTSIMInputMode  tmMode;
   AECHAR            modeString[MAX_TSIM_MODE_SIZE];
} OEMTSIMInputModeInfo;

Members:
   tmMode;               touch screen input Mode enum entry
   modeString;           String that is associated with the mode

Comments:
The available touch screen input modes differ with each BREW-enabled device.

See Also:
ITSIM_GetInputMode(), OEMTSIMInputMode

=====================================================================
=====================================================================
   INTERFACE DOCUMENTATION
=======================================================================

ITSIM Interface

A ITSIM control enables the device user to enter a string of ITSIM using the keys
on the device. The ITSIM control consists of an optional title and a rectangular
window containing one or more lines in which the entered ITSIM is displayed to the
device user. The ITSIM control handles the translation of device user PEN down
into screen. Your application only needs to pass keypress events to the ITSIM
control while it is active and retrieve the ITSIM from the control when device user
ITSIM entry has completed. The translation process depends on the ITSIM entry modes
the device supports (for example, the standard multi-tap mode in which the device
user selects from the characters mapped to each key, and Tegic's T9 predictive ITSIM
input mode). If more than one ITSIM entry mode is supported, your application makes
it possible for the device user to select the specified mode while the ITSIM control
is active. The ITSIM control allows you to specify a Soft Key menu that is used for
this purpose. While the ITSIM control is active, your application must send all keypress
events to it by calling ITSIM_HandleEvent().

While the Soft Key menu is active, the device user can press the UP key to return
to edit mode without making a menu selection.
===H2>
To use a ITSIM control in your application
===/H2>

1.   Call ISHELL_CreateInstance() to create an instance of the ITSIM control.

2.   Call ITSIM_SetRect() to specify the screen rectangle that contains the ITSIM
control.

3.   If specified, call ITSIM_SetText() to specify the initial value of its ITSIM
string.

4.   Call ITSIM_SetProperties() to set any ITSIM control properties.

5.   Call ITSIM_SetActive() to activate the ITSIM control and draw its contents
on the screen.

6.   While the ITSIM control is active, call ITSIM_HandleEvent() to pass it any
key events generated by the user.

7.   When the device user has completed entering ITSIM, call ITSIM_GetText() or
ITSIM_GetTextPtr() to retrieve the ITSIM the device user has entered. (If you
are using a Soft Key menu, the device user may signal the completion of ITSIM entry
with a "Done" item in the menu, or by pressing the SELECT or other key if no Soft
Key menu is present).

9.   Call ITSIM_Release() to free the ITSIM control when you no longer need it.


The following header file is required for ITSIM 

OEMITSIM.h

=============================================================================

ITSIM_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

Version:
   Introduced TCL CDMA CD01 project

See Also:
   ITSIM_Release()

=============================================================================

ITSIM_GetCursorPos()

Description:
    This function gets the position of a cursor in a ITSIM control object.

Prototype:
    int32 ITSIM_GetCursorPos(ITSIM * pme)

Parameters:
    pme  :  Pointer to the ITSIM Interface object

Return Value:
    Absolute position of cursor in ITSIM control

    If you set the character position between an element of a grapheme, the position
    will be updated such that it is in the next available character position after the 
    complete grapheme. This means that a call to this will not necessarily match the value
    passed to a previous call to ITSIM_SetCursorPos(). A grapheme is all the characters 
    that compose a complete symbol in a language such as Thai.

Comments:
    None

Version:
   Introduced TCL CDMA CD01 project

See Also:
    ITSIM_SetCursorPos()

=============================================================================

ITSIM_GetInputMode()

Description:
    This function allows the caller to get the selected ITSIM input mode and the
string associated with it.

Prototype:
    OEMTSIMInputMode ITSIM_GetInputMode
    (
    ITSIM *pme,
    OEMTSIMInputModeInfo * pmInfo
    )

Parameters:
    pme  :  [in] :  Pointer to the ITSIM Interface object
    pmInfo  :  [in/out] :  Input: a pointer to a OEMTSIMInputModeInfo Info structure
to be filled OR can be NULL, so as to not fill a structure and return current mode.
    Output: If a valid pointer is given it is filled with the current mode and the
string associated with that mode.

Return Value:
    An enum of type OEMTSIMInputModeInfo to indicate the input mode set.

Comments:
    If a OEMTSIMInputModeInfo pointer is given the tmMode field, it will match the
return value of this function. The pmInfo field is not required if the callee is
just checking the OEMTSIMInputModeInfo and does not need the string associated with
it.

Version:
   Introduced TCL CDMA CD01 project

See Also:
    OEMTSIMInputModeInfo

=============================================================================

ITSIM_GetProperties()

Description:
    This function returns the ITSIM control-specific properties or flags.

Prototype:
    uint32 ITSIM_GetProperties(ITSIM * pme)

Parameters:
    pme  :  Pointer to the ITSIM Interface object.

Return Value:
    32-bit properties for the ITSIM control.
    Following properties are returned by the ITSIM control object:~
~
    TP_MULTILINE, if set, text control object is multiple line control.~
    TP_FRAME, if set, text control object has a frame.~
    TP_T9_MODE, if set, text control object is in T9 mode.~
    TP_FIXSETRECT, if set, the actual height more closely represents requested height.~
*

Comments:
    None

Version:
   Introduced TCL CDMA CD01 project

See Also:
    ITSIM_SetProperties()

=============================================================================

ITSIM_GetRect()

Description:
    This function fills given pointer to AEERect with the coordinates of the current
bounding rectangle size only for ITSIM, not title. This is particularly useful after
a control is created to determine its optimal/default size and position.
    NOTE:
    If the property TP_FIXSETRECT is set, this function fills the AEERect with the
actual bounding rectangle of the control, which is not necessarily the rectangle
passed in ITSIM_SetRect().
    If the property TP_FIXSETRECT is NOT set, this function returns the rectangle
that was passed in to ITSIM_SetRect().

Prototype:
    void ITSIM_GetRect(ITSIM * pme, AEERect * prc)

Parameters:
    pme :  Pointer to the ITSIM Interface object.
    prc  :  Rectangle to be filled with the coordinates of the ITSIM control object.

Return Value:
    None

Comments:

    None
Version:
   Introduced TCL CDMA CD01 project

See Also:
    AEERect
    ITSIM_SetRect()

=============================================================================

ITSIM_GetText()

Description:
    This function is used to read text associated with the ITSIM Interface object
in the given buffer subject to the maximum of nMaxChars.

Prototype:
    boolean ITSIM_GetText
    (
    ITSIM * pme,
    AECHAR * pBuffer,
    unsigned int nMaxChars
    )

Parameters:
    pme  :  Pointer to the ITSIM Interface object.
    pBuffer  :  Placeholder for the text.
    nMaxChars  :  Maximum number of characters to be read.

Return Value:
    TRUE, if successful.~
    FALSE, if unsuccessful.

Comments:
    None

Version:
   Introduced TCL CDMA CD01 project

See Also:
    ITSIM_GetTextPtr()

=============================================================================

ITSIM_GetTextPtr()

Description:
    It returns the pointer to the text maintained by the ITSIM object. The difference
between this function and GetText is that latter copies the content to a destination
buffer, and the former just returns the pointer to the text inside the ITSIM
object.

Prototype:
    AECHAR * ITSIM_GetTextPtr(ITSIM * pme)

Parameters:
    pme  :  Pointer to the ITSIM Interface object.

Return Value:
    Pointer to the text buffer of the test control object

Comments:
    None

Version:
   Introduced TCL CDMA CD01 project

See Also:
    ITSIM_GetText()

=============================================================================

ITSIM_HandleEvent()

Description:
    This function is used to handle the events received by ITSIM control object.
If the ITSIM control object is in non edit mode, it processes only set title, set
text, and the pressing the UP and DOWN key events. In text edit mode, it processes
various events like key up, key down, key held, set title, set text, command event
from the soft key menu.

Prototype:
    boolean ITSIM_HandleEvent
    (
    ITSIM * pme,
    AEEEvent evt,
    uint16 wp,
    uint32 dwp
    )

Parameters:
    pme  :  Pointer to the ITSIM Interface object.
    evt  :  Event code.
    wp  :  16-bit event data.
    dwp  :  32-bit event data.

Return Value:
    TRUE, if the event was processed by the ITSIM control.~
    FALSE, if otherwise.

Comments:
    If a grapheme is present in the text a key navigation event may cause the cursor to 
    move more than one position to align itself per grapheme boundary.

Version:
   Introduced TCL CDMA CD01 project

See Also:
    None

=============================================================================

ITSIM_IsActive()

Description:
    This function returns the active state of the ITSIM control object.

Prototype:
    boolean ITSIM_IsActive(ITSIM * pme)

Parameters:
    pme  :  Pointer to the ITSIM Interface object.

Return Value:
    TRUE, if the ITSIM control is active.~
    FALSE, if otherwise.

Comments:
    None

Version:
   Introduced TCL CDMA CD01 project

See Also:
    None

=============================================================================

ITSIM_Redraw()

Description:
    This function instructs the ITSIM control object to redraw its contents. The
ITSIM Interface object does not redraw its contents every time the underlying
data behind the ITSIM control changes. This allows several data updates to occur
while minimizing screen flashes. For example, several changes can be made to the
contents of the ITSIM control object with no visible effect until ITSIM_Redraw()
function is called.

Prototype:
    boolean ITSIM_Redraw(ITSIM * pme)

Parameters:
    pme  :  Pointer to the ITSIM Interface object.

Return Value:
    TRUE, if the text control was redrawn.
    FALSE, if otherwise.

Comments:
    None

Version:
   Introduced TCL CDMA CD01 project

See Also:
    None

=============================================================================

ITSIM_Release()

Description:
    This function is inherited from IBASE_Release(). 

Version:
   Introduced TCL CDMA CD01 project

See Also:
   ITSIM_AddRef()

=============================================================================

ITSIM_Reset()

Description:
    This function instructs the ITSIM control to reset (free/delete) its contents
and to immediately leave active/focus mode.

Prototype:
    void ITSIM_Reset(ITSIM * pme)

Parameters:
    pme  :  Pointer to the ITSIM Interface object.

Return Value:
    None

Comments:
    None

Version:
   Introduced TCL CDMA CD01 project

See Also:
    ITSIM_SetActive()

=============================================================================

ITSIM_SetActive()

Description:
    This function is used to make a ITSIM control object active. Only an active ITSIM
control object handles the event sent to it. Inactive ITSIM control object just ignores
the events. Also an inactive ITSIM control object does not draw its frame.

Prototype:
    void ITSIM_SetActive(ITSIM * pme, boolean bActive)

Parameters:
    pme  :  Pointer to the ITSIM Interface object.
    bActive  :  Boolean flag that specifies:
    TRUE: to activate the text control object.
    FALSE: to deactivate the text control object.

Return Value:
    None

Comments:
    None

Version:
   Introduced TCL CDMA CD01 project

See Also:
    None

=============================================================================

ITSIM_SetCursorPos()

Description:
    This function is used to set the position of a cursor in a ITSIM control object.
You can use the following defines for nOffset to place the text at the start or
end.

Prototype:
    void ITSIM_SetCursorPos(ITSIM * pme, int32 nOffset)

Parameters:
    pme  :  Pointer to the ITSIM Interface object
    nOffset  :  Placement of the ITSIM object
    TC_CURSOREND - Place the cursor at the end of the text.
    TC_CURSORSTART - Place the cursor at the beginning of the text.

Return Value:
    None

Comments:
    If nOffset is < 0 the cursor is placed at the beginning of the text.
    If nOffset is > the Length of the text, the cursor is placed at the end of the
text

  If you set the character position between an element of a grapheme, the position
  will be updated such that it is in the next available character position after the 
  complete grapheme. This means that a subsequent call to ITSIM_GetCursorPos() will not 
  necessarily match the value passed to this function. A grapheme is all the characters 
  that compose a complete symbol in a language such as Thai.

Version:
   Introduced TCL CDMA CD01 project

See Also:
    ITSIM_GetCursorPos()

=============================================================================

ITSIM_SetInputMode()

Description:
    This function allows the caller to set the selected touch screen input mode.

Prototype:
    OEMTSIMInputMode ITSIM_SetInputMode
    (
    ITSIM * pme,
    OEMTSIMInputMode wMode
    )

Parameters:
    pme  :  Pointer to the ITSIM Interface object.
    wMode :  Touch screen input mode.

Return Value:
    An enum of type OEMTSIMInputMode to indicate the input mode set.

Comments:
    None

Version:
   Introduced TCL CDMA CD01 project

See Also:
    OEMTSIMInputMode

=============================================================================

ITSIM_SetMaxSize()

Description:
    This function is used to set the maximum text size supported by the ITSIM control
object. If the size being set is more than the size already set, this leads to the
freeing up of the memory associated with the previous size and allocation of the
memory per the new size.

Prototype:
    void ITSIM_SetMaxSize (ITSIM * pme, uint16 nMaxSize)

Parameters:
    pme  :  Pointer to the ITSIM Interface object.
    nMaxSize  :  Maximum text size in AECHAR characters excluding NULL and if 0
(zero) then no effect.

Return Value:
    None

Comments:
    The implementation of this function may vary between devices. Some devices may
allow text to be entered beyond the maximum size set by this function.

Version:
   Introduced TCL CDMA CD01 project

See Also:
    None

=============================================================================

ITSIM_SetProperties()

Description:
    This function sets ITSIM control-specific properties or flags.

Prototype:
    void ITSIM_SetProperties(ITSIM * pme, uint32 dwProps)

Parameters:
    pme  :  Pointer to the ITSIM Interface object.
    dwProps  :  32-bit set of flags/properties.
    Following properties are used for ITSIM control object:
    TP_MULTILINE, if set, ITSIM control object is multiple line control.
    TP_FRAME, if set, ITSIM control object has a frame.
    TP_T9_MODE, if set, ITSIM control object is in T9 mode.
    TP_FIXSETRECT, if set, the actual height more closely represents requested height.


Return Value:
    None

Comments:
    None

Side Effects:
    It deactivates the ITSIM control.

Version:
   Introduced TCL CDMA CD01 project

See Also:
    ITSIM_GetProperties()

=============================================================================

ITSIM_SetRect()

Description:
    This function fills the AEERect data structure with the coordinates of the current
bounding rectangle to determining the size of the text, not the title. This is particularly
useful after a control is created to determine its optimal/default size and position.
    NOTE:
    If the property TP_FIXSETRECT is set, this function fills the AEERect data structure
with the actual bounding rectangle of the control, which may not be the rectangle
set using ITSIM_SetRect().
    If the property TP_FIXSETRECT is NOT set, this function returns the AEERect
data structure which contains the coordinates of rectangle set using ITSIM_SetRect().

Prototype:
    void ITSIM_SetRect(ITSIM * pme, const AEERect * prc)

Parameters:
    pme  :  Pointer to the ITSIM Interface object.
    prc  :  Bounding rectangle for the ITSIM control object.

Return Value:
    None

Comments:
    By default, the ITSIM control rectangle of the ITSIM control object has a device screen
width as width and (device screen height - text height) as height starting from
the upper left corner.

Version:
   Introduced TCL CDMA CD01 project

See Also:
    AEERect
    ITSIM_GetRect()

=============================================================================

ITSIM_SetText()

Description:
    This function is used to assign given string as text of the ITSIM control object.

Prototype:
    boolean ITSIM_SetText
    (
    ITSIM * pme, 
    const AECHAR * psz,
    int cch
    )

Parameters:
    pme  :  Pointer to the ITSIM Interface object.
    psz  :  Text string to be set.
    cch  :  Number of AECHAR characters to be assigned from the string to the text
of the ITSIM control object. If cch is negative or greater than the length of psz
string, then the length of string is used. 

Return Value
    None

Comments:
    None

Version:
   Introduced TCL CDMA CD01 project

See Also:
    None

=============================================================================*/


