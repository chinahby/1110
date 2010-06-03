/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:      OEMText.h


SERVICES:  OEM Functions for Text Control support


DESCRIPTION: This file provides the data structures and prototypes of functions
that OEMs must provide in order to support the Text Control. This header file
shall be included by OEMTextCtl.c which provides sample implementation for
the functions mentioned here. The AEETextCtl API implementation makes down calls into
the functions mentioned here.


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable


        Copyright © 1999-2002,2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
#ifndef OEMTEXT_H
#define OEMTEXT_H

/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE_OEMComdef.h"
#include "AEEText.h"


/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

// User-defined text modes must begin from here

#define AEE_TEXT_MODE_USER    10

//Maximum length of the Mode string. This is used in the Menu.

#define MAX_MODE_STR 15

//Info holder for the text modes
typedef struct
{
        uint16 wID;        // ID of the Text Mode.
        AECHAR pszTitle[MAX_MODE_STR+1]; // Name of the mode. This name is used in the menu.
}AEETextMode;



//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------
#define TP_VALIDATE_INPUT 0x08000000 // Sets the AEE_TM_DIALED_DIGITS mode to validate the string.

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------


//===============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===============================================================================

OEMCONTEXT OEM_TextCreate(const IShell* pIShell,
                                                                  const IDisplay* pIDisplay,
                                                                  const AEERect *pRect,
                                                                  AEECLSID cls);

void OEM_TextDelete(OEMCONTEXT hTextField);


boolean OEM_TextSet(OEMCONTEXT hTextField, const AECHAR * pszText, int nChars);

AECHAR* OEM_TextGet(OEMCONTEXT hTextField);

void OEM_TextDraw(OEMCONTEXT hTextField);

void OEM_TextUpdate(OEMCONTEXT hTextField);

void OEM_TextGetModeString(OEMCONTEXT hTextField, AECHAR* szBuf, uint16 len);

void OEM_TextSetEdit(OEMCONTEXT hTextField, boolean bIsEditable, AEETextInputMode wmode);

AEETextInputMode OEM_TextGetCurrentMode(OEMCONTEXT hTextField);

void OEM_TextAddChar(OEMCONTEXT hTextField,AECHAR ch, boolean bOverStrike);

int OEM_TextQueryModes(AEETextMode** ppTextMode);

boolean OEM_TextKeyPress(OEMCONTEXT hTextField,
                         AEEEvent eCode,
                                                                 uint32 dwKeyCode,
                                                                 uint32 dwKeySyms);

boolean OEM_TextDialogEvt(OEMCONTEXT hTextField,
                          AEEEvent eCode,
                                                                  uint16 wDlgID,
                                                                  uint32 dwDlgPtr);

boolean OEM_TextHandlePenEvt(OEMCONTEXT hTextField,
                          AEEEvent eCode,
                                                                  uint16 wParam,
                                                                  uint32 dwParam);

uint32 OEM_TextGetProperties(OEMCONTEXT hTextField);

void OEM_TextSetProperties(OEMCONTEXT hTextField, uint32 dwProperties);

void OEM_TextGetRect(OEMCONTEXT hTextField, AEERect *pOutRect);

void OEM_TextSetRect(OEMCONTEXT hTextField, const AEERect *pInRect);

uint16 OEM_TextGetMaxChars(OEMCONTEXT hTextField);

void OEM_TextSetMaxChars(OEMCONTEXT hTextField, uint16 wMaxChars);

void OEM_TextSetSel(OEMCONTEXT hTextField, int iSelStart, int iSelEnd);

void OEM_TextGetSel(OEMCONTEXT hTextField, int *piSelStart, int *piSelEnd);

void OEM_TextEnumModesInit(void);

//Return true if this is a valid mode. Return false if we already reached end
boolean OEM_TextEnumMode(AEETextMode* pMode);

uint16 OEM_TextQuerySymbols(AECHAR *pszOut, uint16 size);

int32 OEM_TextGetCursorPos(OEMCONTEXT hTextField);
void  OEM_TextSetCursorPos(OEMCONTEXT hTextField, int32 nOffset);

void  OEM_TextDrawIMEDlg(OEMCONTEXT hTextField);

/*========================================================================
  INTERFACES DOCUMENTATION
==========================================================================

OEM_Text Interface

Description:
  This module provides a input text interface for the AEE. The reference
implementation can be modified to add more text input modes for foreign
languges or other input methods.

=======================================================================

Function: OEM_TextCreate()

Description:
   This function creates a dynamic text control object. It uses the given
   rectangle pRect to create the text control. The IShell and IDisplay
   pointers are saved in the newly created context to be used by the
   text control for notification, drawing, etc.

Prototype:
   OEMCONTEXT OEM_TextCreate(const IShell* pIShell,
                                                                  const IDisplay* pIDisplay,
                                                                  const AEERect *pRect,
                                                                  AEECLSID cls)

Parameters:
   pIShell:    const IShell *   Pointer to the IShell interface object

   pIDisplay:  const IDIsplay*  Pointer to the IDisplay interface object

   pRect:      const AEERect*   Pointer to the rectangle specifying the bounds
                                and location of the text control to be created.

   cls:        AEECLSID         Class ID to create. Should be stored to maintain
                                compatibility with older applets.


Return Value:
   If successful, a OEMCONTEXT which can be used as the handle to the
   newly created text control. If failed, returns NULL. If pIShell or
   pIDisplay or pRect is NULL, the function fails.

Comments:
   None

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextDelete()

Description:
   This function deletes a dynamic text control object. The text control
   must have been created successfully using OEM_TextCreate(). This function,
   also frees up memory and other resources (if any) associated with this
   text control.

Prototype:
   void OEM_TextDelete(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object that needs
                               to be deleted.

Return Value:
   None

Comments:
   If hTextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSet()

Description:
   This function sets the text of a given text control object.
   The text control must have been created successfully using
   OEM_TextCreate(). Once the text has been set, OEM_Textdraw()
   must be called to update the screen with the new text.

Prototype:
   boolean OEM_TextSet(OEMCONTEXT hTextField, const AECHAR *pszText, int nChars)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object
   pszText:       AECHAR *     Text string that is to be set into the text control
   nChars:        int          Number of characters to set

Return Value:
   None

Comments:
   If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGet()

Description:
   This function gets the text associated with a given text control object.
   It returns a pointer to the text present in the given text control object.

Prototype:
   AECHAR* OEM_TextGet(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object

Return Value:
   On Success, returns pointer to the Text string in the text control.
   On failure, returns NULL.

Comments:
   If hTextCtl is NULL, the function returns NULL.

Side Effects:
   None

See Also:
   None
==========================================================================

Function: OEM_TextDraw()

Description:
   This function draws the text associated with a given text control object
   on the screen. This function also draws the associated items such as
   Scroll Bar, Border, Cursor etc. if  necessary and if supported.

Prototype:
   void OEM_TextDraw(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object

Return Value:
   None.

Comments:
When the TP_PASSWORD property is set please display a text buffer of **** in place
of actual characters. You must maintain your original buffer of actual text.
Also when in multitap mode, please allow selection(ie. last character to be shown) while typing
Only * out the last character when it is committed to the text.  If hTextCtl is NULL, the
function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextUpdate()

Description:
   This function draws the text associated with a given text control object
   on the screen if the text control is modified.

Prototype:
   void OEM_TextUpdate(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetModeString()

Description:
   This function returns the wide string corresponding to the current mode of
   the text control specified by hTextField. The possible mode strings are:
   "Multitap", "Numbers" and "Symbols".

Prototype:
   void OEM_TextGetModeString(OEMCONTEXT hTextField, AECHAR* szBuf, uint16 len)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object [IN]
   szBuf:         AECHAR *     String corresponding to the mode of the text control [OUT]
   len:           uint16       Length of the mode string buffer [IN]

Return Value:
   None.

Comments:
   If hTextCtl or szBuf is NULL or if len is one or less, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetEdit()

Description:
   This function informs the text control whenever it goes into or out of focus.
   Typically, when the text control is in focus, the border and cursor are
   activated and when the text control goes out of focus, these items are
   de-activated. This function also informs the text control of the current
   text mode.

Prototype:
   void OEM_TextSetEdit(OEMCONTEXT hTextField,
                        boolean bIsEditable,
                        AEETextInputMode wmode
                       )

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   bIsEditable:   boolean           Flag to indicate if the text control
                                    object is in focus (is editable)
   wmode:         AEETextInputMode  Text input mode

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.
   If wmode is out of the valid range, please change the input mode to your
   default mode (reference code: AEE_TM_LETTERS)

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetCurrentMode()

Description:
   This function returns the current text mode of the text control specified
   by hTextField.

Prototype:
   AEETextInputMode OEM_TextGetCurrentMode(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object

Return Value:
  The current text mode for the text control specified.

Comments:
   None.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextAddChar()

Description:
   This function adds/overwrites a charecter at the current cursor location
   in the specified text control.

Prototype:
   void OEM_TextAddChar(OEMCONTEXT hTextField,AECHAR ch, boolean bOverStrike)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   ch:            AECHAR            Wide character to be added to the text control
   bOverStrike:   boolean           Overwrite the text at the cursor location.


Return Value:
   None.

Comments:
   Overstrike is only meaningful if there is an insertion point rather than a
   selection AND the insertion point is not at the very end of the text.  If
   hTextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextQueryModes()

Description:
   This function is invoked by the AEE to query the different text modes
   (T9, MULTITAP, etc) supported by the OEM. This information is used by
   the AEE for two purposes:
   1) To populate the pop-up menu containing selections for the
      different modes supported.
   2) To notify the text control (using OEM_TextSetEdit()) of the mode
      selected by the user.

   The OEM must populate the given data structure and return from this function.


Prototype:
   int OEM_TextQueryModes(AEETextMode** ppTextMode)

Parameters:
   ppTextMode:    AEETextMode**    On return, this contains a valid pointer
                                   to an array of AEE_TextMode containing
                                   information about the different modes
                                   supported by the OEM. The OEM must use the
                                   standard identifier OEM_TEXT_MODE_ SYMBOLS
                                   for symbols mode. Memory for this pointer
                                   must be allocated by the OEM.


Return Value:
   Returns the number of text modes supported by the OEM.

Comments:
   Text  Modes Support:
   Here is a brief description of how text modes are supported.

   ?The AEE platform invokes the OEM function OEM_TextQueryModes() to get
     information on the different text modes supported by the OEM.
   ?The information obtained above is used to populate the menu containing
     selection strings for the different modes.
   ?When the user selects a particular mode, the function OEM_TextSetEdit()
     is invoked and is passed the ID of the mode that has been selected.
     If the user has not changed the mode, the ID is set to OEM_TEXT_MODE_DEFAULT,
     informing the OEM to use the currently selected mode.
   ?The OEM must use the standard ID OEM_TEXT_MODE_SYMBOLS for supporting the
     symbol mode. All other IDs must be based out of OEM_TEXT_MODE_USER.


Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextKeyPress()

Description:
   This function handles the key press events  in a text control.
   Whenever any key is pressed while a text control is active,
   this function is invoked, passing information relating to the
   key that has been pressed. The OEM must handle the key event and
   process it appropriately.

Prototype:
   boolean OEM_TextKeyPress(OEMCONTEXT hTextField,
                         AEEEvent eCode,
                                                                 uint32 dwKeyCode,
                                                                 uint32 dwKeySyms)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   eCode:         AEEEvent          Event code for the key event
   dwKeyCode:     uint32            Key code of the key that has been pressed
   dwKeySyms      uint32


Return Value:
  The current text mode for the text control specified.

Comments:
   Ensure to handle key navigation presses on a grapheme boundary.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextDialogEvt()

Description:
   This function handles dialog events of any corresponding dialogs that are
   shown to obtain screen space when inline text editing is not possible due
   to UI constraints.

Prototype:
  boolean OEM_TextDialogEvt(OEMCONTEXT hTextField,
                          AEEEvent eCode,
                                                                  uint16 wDlgID,
                                                                  uint32 dwDlgPtr)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   eCode:         AEEEvent          Event code for the dialog event
   wDlgID:        uint32            The dialog ID that sent the event
   dwDlgPtr       uint32            IDialog * for the dialog


Return Value:
  TRUE   Dialog event handled
  FALSE  Dialog event not handled

Comments:
   You must handle only dialog events based on the dialogs created by this layer.

Side Effects:
   Your dialog will have the input focus, reference code sets all events to this layer
   until the dialog is dismissed. This is highly recommended behavior, so events are
   not mixed between dialog and application.

See Also:
   None

==========================================================================

Function: OEM_TextHandlePenEvt()

Description:
   This function will be called ot handle pen events. Please follow the guidelines
   for handling pen events in a cooperative manner to ensure compatibility when
   used in dialogs and applications.

Prototype:
  boolean OEM_TextHandlePenEvt(OEMCONTEXT hTextField,
                           AEEEvent eCode,
                           uint16 wParam,
                           uint32 dwParam);

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   eCode:         AEEEvent          Pen Event code
   wParam:        uint16            wParam for EVT_POINTER_* events
   dwParam:       uint32            dwParam for EVT_POINTER_* events


Return Value:
  TRUE   Pen event handled, see documentation for expectations of IControl with pen events
  FALSE  Pen event not handled, see documentation for expectations of IControl with pen events

Comments:
   You must handle the pen events as described in the expectations of Pens with IControl.

Side Effects:
   None.

See Also:
   None

==========================================================================

Function: OEM_TextGetProperties()

Description:
   This function returns the properties of the text control, such as frame type,
   multiline, or rapid entry (such as T9).

Prototype:
   uint32 OEM_TextGetProperties(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object

Return Value:
   Property of the text control, which could be combination of 1 or more
   of the following:


Comments:
Important properties
    TP_MULTILINE :  If set, text control object is multiple line control
    TP_FRAME :      If set, text control object has a frame
    TP_RAPID_MODE : If set, text control object is in rapid mode
    TP_PASSWORD :   If set, text control will display * characters in place of real characters
Safe to ignore:
    TP_NODRAW :     If set, text control object does not draw itself
    TP_NOUPDATE :   If set, text control object does not call IDIPLAY_Update when not needed

When using TP_PASSWORD in multitap mode, please allow selection(ie. last character to be shown) while typing
Only * out the last character when it is committed to the text

If hTextCtl is NULL, the function returns 0 ( zero ).

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetProperties()

Description:
   This function sets the properties of the text control, such as frame type,
   multiline, rapid entry (such as T9), or a combination thereof.

Prototype:
   void OEM_TextSetProperties(OEMCONTEXT hTextField, uint32 dwProperties)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   dwProperties   uint32            Properties (TP_FRAME, TP_MULTILINE,
                                    TP_RAPID_MODE, or a combination of these)

Return Value:
   None.

Comments:
Important properties
    TP_MULTILINE :  If set, text control object is multiple line control
    TP_FRAME :      If set, text control object has a frame
    TP_RAPID_MODE : If set, text control object is in rapid mode
    TP_PASSWORD :   If set, text control will display * characters in place of real characters
Safe to ignore:
    TP_NODRAW :     If set, text control object does not draw itself
    TP_NOUPDATE :   If set, text control object does not call IDIPLAY_Update when not needed

When using TP_PASSWORD in multitap mode, please allow selection(ie. last character to be shown) while typing
Only * out the last character when it is committed to the text

If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetRect()

Description:
   This function returns the rectangle correpsonding to the bounds of this
   text control.

Prototype:
   void OEM_TextGetRect(OEMCONTEXT hTextField, AEERect *pOutRect)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object[IN]
   pOuntRect:     AEERect *         Rectangle corresponding to the
                                    bounds of the text control[OUT]

Return Value:
   None.

Comments:
   If hTextCtl or pOutRect is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetRect()

Description:
   This function returns the rectangle correpsonding to the bounds of this
   text control.

Prototype:
   void OEM_TextSetRect(OEMCONTEXT hTextField, const AEERect *pInRect)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   pInRect:     AEERect *         New bounds for the text control

Return Value:
   None.

Comments:
   If htextField or pInRect is NULL, the function simply returns

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetMaxChars()

Description:
   This function gets the maximum number of characters that can be added to
   the specified text control.

Prototype:
   uint16 OEM_TextGetMaxChars(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object

Return Value:
   Max. number of characters for the text control specified by hTextField.

Comments:
   if hTextCtl is NULL, the function returns 0 (zero).

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetMaxChars()

Description:
   This function sets the maximum number of characters that can be added to
   the specified text control.

Prototype:
   void OEM_TextSetMaxChars(OEMCONTEXT hTextField, uint16 wMaxChars)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   wMaxChars:     uint16            New maximum number of chars in this
                                    text control

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetSel()

Description:
   This function gets the start and end locations for the selected text.

Prototype:
   void OEM_TextGetSel(OEMCONTEXT hTextField, int *piSelStart, int *piSelEnd)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object [IN]
   piSelStart:    int *             Start location of the text selection [OUT]
   piSelEnd:      int *             Ending location of the text selection [OUT]

Return Value:
   None.

Comments:
   If htextField is NULL, then
     if piSelStart is non-NULL, the first entry is set to zero.
     if piSelEnd is non-NULL, the first entry is set to zero.

   The selection start position must be preserved to what the applet had set to the
   nearest grapheme.
   This means, if you internally represent the selection start as a value less than
   selection end you must return the actual data set in OEM_TextSetSel().
   Examples:
   OEM_TextSetSel(p, 3, 6) -> OEM_TextGetSel() will return *piSelStart = 3, *piSelEnd = 6
   OEM_TextSetSel(p, 6, 3) -> OEM_TextGetSel() will return *piSelStart = 6, *piSelEnd = 3


Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetSel()

Description:
   This function sets the start and end locations for text selection.

Prototype:
   void OEM_TextSetSel(OEMCONTEXT hTextField, int iSelStart, int iSelEnd)

Parameters:
   htextField:   OEMCONTEXT       Handle for the text control object
   iSelStart:    int              Start location of the text selection
   iSelEnd:      int              Ending location of the text selection

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.

   This must update the cursor position to the place where iSelEnd is set.
   The selection start position must be preserved to what the applet had set to the
   nearest grapheme.
   This means, if you internally represent the selection start as a value less than
   selection end you must return the actual data set in OEM_TextSetSel().
   Examples:
   OEM_TextSetSel(p, 3, 6) -> OEM_TextGetSel() will return *piSelStart = 3, *piSelEnd = 6
   OEM_TextSetSel(p, 6, 3) -> OEM_TextGetSel() will return *piSelStart = 6, *piSelEnd = 3

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextEnumModesInit()

Description:
   This function initializes the enumeration mode. This function does not
   have an associated handle for the text control object.


Prototype:
   void OEM_TextEnumModesInit(void);

Parameters:
   None.

Return Value:
   None.

Comments:
   None.

Side Effects:
   None

See Also:
   None


==========================================================================

Function: OEM_TextEnumMode()

Description:
   This function gets the next text enumeration mode.


Prototype:
   boolean OEM_TextEnumMode(AEETextMode* pMode)

Parameters:
   pMode:    AEETextMode *     Pointer to the next text mode [IN/OUT]


Return Value:
   TRUE if the next mode is valid.
   FALSE if already at the end of the list.

Comments:
   If pMode is NULL, the function returns FALSE.
   Use the wID member of the AEETextMode structure to enumerate the next mode

Side Effects:
   None

See Also:
   None



==========================================================================

Function: OEM_TextQuerySymbols()

Description:
   This function gets the buffer pszOut with the symbols. The length of
   the buffer is specified by size.


Prototype:
   uint16 OEM_TextQuerySymbols(AECHAR *pszOut, uint16 size)

Parameters:
   pszOut:  AECHAR *     Symbols buffer [OUT]
   size:    uint16       Size of the buffer [IN]

Return Value:
   The number of symbols put into the query buffer. If pszOut is NULL,
   or if size is less than the number of OEM symbols, this function returns
   0.

Comments:
   If pszOut is NULL, the function returns zero. If you do not want the Symbols
   mode to be supported, simply fill the pszOut buffer with an empty string and
   return 0.

Side Effects:
   None

See Also:
   None


==========================================================================

Function: OEM_TextGetCursorPos()

Description:
   This function gets the absolute position of the cursor


Prototype:
   int32 OEM_TextGetCursorPos(OEMCONTEXT hTextField)

Parameters:
   hTextField:    OEMCONTEXT     Handle for the text control object

Return Value:
   The 0 based position of the cursor.
   For example:
   If you have the Text Hi and the cursor is given as |~
~
   |Hi would return 0~
   H|i would return 1~
   Hi| would return 2 ~
*

Comments:
  If you set the character position between an element of a grapheme, the position
  will be updated such that it is in the next available character position after the
  complete grapheme. This means that a call to this will not necessarily match the value
  passed to a previous call to OEM_TextGetCursorPos(). A grapheme is all the characters
  that compose a complete symbol in a language such as Thai.

Side Effects:
   None

See Also:
   None


==========================================================================

Function: OEM_TextSetCursorPos()

Description:
   This function gets the absolute position of the cursor


Prototype:
   int32 OEM_TextSetCursorPos(OEMCONTEXT hTextField, int32 nOffset)

Parameters:
   hTextField:    OEMCONTEXT     Handle for the text control object
   nOffset:       int32          Absolute Offset to move the cursor to

Return Value:
   None.

Comments:
   This function should move the cursor to the 0 based position of the cursor.
   If nOffset is > the length of the text, the cursor should be placed after the text.
   If nOffset is <= 0, the cursor should be placed at the begging of the text
   For example:
   If you have the Text Hi and | represents the cursor:~
~
   nOffset = 0    |Hi~
   nOffset = -1   |Hi~
   nOffset = 1    H|i ~
   nOffset = 2    Hi| ~
   nOffset = 100  Hi| ~
*

  If you set the character position between an element of a grapheme, the position
  must be updated such that it is in the next available character position after the
  complete grapheme. A grapheme is all the characters that compose a complete symbol
  in a language such as Thai.

Side Effects:
   Calling this does not guarentee a matching result in OEM_TextGetCursorPos().

See Also:
   None

==========================================================================

Function: OEM_TextDrawIMEDlg()

Description:
   This function redraws the IME dialog as needed.


Prototype:
   void  OEM_TextDrawIMEDlg(OEMCONTEXT hTextField);

Parameters:
   hTextField:    OEMCONTEXT     Handle for the text control object

Return Value:
   None.

Comments:
   This function should redraw the IME dialog when it is displayed. If one
   is not display this function should return without any operation.

Side Effects:
   None

See Also:
   None

==============================================================
AEEText

See the ITextCTL Interface in the BREW API Reference.

========================================================================*/

#endif    // OEMTEXTCTL_H