/*
  ========================================================================

  FILE:  AEETextWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I display and allow entry of text input. I
  require a controller to do my job and try to instantiate a default
  textcontroller if an OEM supplied one can not be created

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEETEXTWIDGET_H__
#define __AEETEXTWIDGET_H__

#include "AEETextModel.h"
#include "AEEWProperties.h"

#include "bid\AEECLSID_TEXTWIDGET.bid"

// text widget flags
#define TWF_MULTILINE            1
#define TWF_VERTICALNAV          2
#define TWF_PASSWORD             4
#define TWF_WRAPNOBREAK          8
#define TWF_SHORTENTEXT         16


static __inline int IWIDGET_GetTextWidgetText(IWidget *piWidget, AECHAR **ppo) {
   ITextModel *piModel;

   if (piWidget && IWIDGET_GetModel(piWidget, AEEIID_TEXTMODEL, (IModel **)&piModel) == 0) {
      TextInfo textInfo;
      
      ITEXTMODEL_GetTextInfo(piModel, &textInfo);
      *ppo = (AECHAR*)textInfo.pwText;
      ITEXTMODEL_Release(piModel);

      return SUCCESS;
   }
   return EFAILED;
}

static __inline int IWIDGET_SetTextWidgetText(IWidget *piWidget, const AECHAR *txt) {
   ITextModel *piModel;

   if (piWidget && IWIDGET_GetModel(piWidget, AEEIID_TEXTMODEL, (IModel **)&piModel) == 0) {
      ITEXTMODEL_SetSel(piModel, 0, -1);
      ITEXTMODEL_ReplaceSel(piModel, txt, (int)(txt ? -1 : 0));
      ITEXTMODEL_Release(piModel);
   }

   return SUCCESS;
}


static __inline int IWIDGET_GetTextLimit(IWidget *po, int *pnMaxChars) {
   return IWIDGET_GetPropInt(po, PROP_LIMITTEXT, pnMaxChars);
}
static __inline int IWIDGET_SetTextLimit(IWidget *po, int nMaxChars) {
   return IWIDGET_SetProperty(po, PROP_LIMITTEXT, (uint32)nMaxChars);
}

static __inline int IWIDGET_GetInputMode(IWidget *po, int *pnMode) {
   return IWIDGET_GetPropInt(po, PROP_INPUTMODE, pnMode);
}
static __inline int IWIDGET_SetInputMode(IWidget *po, int nMode) {
   return IWIDGET_SetProperty(po, PROP_INPUTMODE, (uint32)nMode);
}

static __inline int IWIDGET_GetTextController(IWidget *po, IController **pController) {
   return IWIDGET_GetProperty(po, PROP_TEXTCONTROLLER, (uint32 *) pController);
}

static __inline int IWIDGET_SetTextController(IWidget *po, IController *pController) {
   return IWIDGET_SetProperty(po, PROP_TEXTCONTROLLER, (uint32) pController);
}

static __inline int IWIDGET_GetCaretWidget(IWidget *po, IWidget **piCaret) {
   return IWIDGET_GetProperty(po, PROP_CARETWIDGET, (uint32 *) piCaret);
}

static __inline int IWIDGET_SetCaretWidget(IWidget *po, IWidget *piCaret) {
   return IWIDGET_SetProperty(po, PROP_CARETWIDGET, (uint32) piCaret);
}

static __inline int IWIDGET_GetTextControllerModeKey(IWidget *po, AVKType *pAvk) {
   return IWIDGET_GetProperty(po, PROP_TEXTCONTROLLERMODEKEY, (uint32*)pAvk);
}

static __inline int IWIDGET_SetTextControllerModeKey(IWidget *po, AVKType avk) {
   return IWIDGET_SetProperty(po, PROP_TEXTCONTROLLERMODEKEY, (uint32)avk);
}


/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Text Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   text widget by creating a new instance of the Text Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the text widget is defined below.

Definition:

Identifier            Definition
----------            -----------------------------------
AEECLSID_TEXTWIDGET:  Identifies the class ID of the softkey widget
   
Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Text Widget Class

Description:
   The text widget is a user interface element that provides text entry facilities to the
   widget framework.  The widget manages interaction between the application and OEM
   provided services for supporting various text entry methods (multitap, T9, etc),
   handles display of the text entry caret, and provides accessor functions for storing
   and retrieving entered text.  Text widgets are usually implemented in conjunction
   with a static widget, to provide data entry fields in an application, as illustrated
   below:

===pre>
        Favorite Color:  <------------------------ Static widget
          +-----------------------------+
          | Red                         |  <------ Text widget
          +-----------------------------+
          
        Favorite Toy:
          +-----------------------------+
          | Slinky                      |
          +-----------------------------+
          
        Favorite Inanimate Object:
          +-----------------------------+
          | |                           |  <-- Current field with caret ('|') at point
          +-----------------------------+      where next character will be entered.
===/pre>

   By default, the text widget is displayed within a border and the widget can be
   set to support either single or multiline text entry.  When configured to support
   multiline text entry, the text widget will set its preferred height to at least
   two lines.  The border abides to all the same properties as other borders in the
   widget framework (color, thickness, transparency, etc).
   
   The text widget is implemented as using a controller that is usually provided by
   the OEM as AEECLSID_TEXTCONTROLLER.  However, if the OEM has not provided a text
   controller, the text widget will use the default text controller provided by the
   widget framework.  Text entry is actually captured by and handled by the text
   controller, with that object modifying the text model tied to both the controller
   and the text widget.  The text widget listens for changes to the text model and
   will be notified of changes to the model, which in turn triggers the text widget
   to update what the user sees on the display in response to key presses.  The
   relationship between the text widget, controller and text model can be illustrated
   with the following diagram:
   
===pre>
     How the keys the user presses go from the user's thumb to the display:
     
                       +----------------+
     Key Press ------> |                |        +--------------------+
        'a'            |      Text    +--------> |                    |
                       |   Controller   |        |     Text Model     |
                       |                |        |         +          |
                       +----------------+        +---------|----------+
                                                           |
                                                           |
                                                           |
                                                           |
                                                           V
                                                 +--------------------------------+
                               Text Widget ----> | a|                             |
                                                 +--------------------------------+
===/pre>
   
   When the user presses the 'a' key, the text controller updates the text model,
   triggering the text widget (which is listening for changes in the text model)
   to retrieve the updated model contents and redraw itself, resulting in the letter
   'a' being displayed at the current caret location.  The caret, in turn, moves in
   the text direction (in this case, left to right) and awaits the next action from
   the user.
   
   The text widget is also attached to a view model, which will send scroll events
   (with 'evCode' set to EVT_MDL_SCROLL_CHANGE) to other objects registered with
   the text widget's view (i.e. scroll bars, etc). 
   
   To create a text widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_TEXTWIDGET to
          create a text widget.  ISHELL_CreateInstance() will retrieve a reference
          counted instance of the class that manages the widget.
       2. Call IWIDGET_GetModel() to retrieve the text model associated with the text
          widget and populate this model with any existing text.
       3. If the text widget is to contain multiple lines of text, call
          IWIDGET_SetFlags() with the TWF_MULTILINE flag set.
       4. Call IWIDGET_SetExtent() to establish the extent of the text widget
       5. If the widget is to support some indication that the text can be scrolled,
          create and attach a scrollbar by calling IDECORATOR_SetWidget(), making
          sure that the scrollbar and text widget share the same view model.
===/pre>

   The text widget, by default, will be created with the following characteristics:

===pre>
           Horizontal extent:  40 pixels
             Vertical extent:  16 pixels
     Background transparency:  255  (fully transparent)
      Border Padding (left) :  3
     Border Padding (right) :  2
       Border Padding (top) :  1
    Border Padding (bottom) :  1
            Background color:  RGB_WHITE
            Character height:  12 pixels
                  Text color:  (0, 0, 255) Blue
         Selected text color:  RGB_WHITE
             Selection color:  (0, 0, 128) Medium Blue
             Number of lines:  1  (i.e. multiline mode is turned off)
                        Font:  AEECLSID_FONTSYSNORMAL
     Password Mask Character:  *
===/pre>

Supported Events:
   The text widget processes events in three passes.  In the first pass the text widget
   will pass all events to the widget's controller, which will screen for key events and
   other events that pertain to controllers.  If the controller accepts and processes
   the event, the text widget will return TRUE.  If the event is not handled by the
   controller, the text widget will pass the event to its border for normal border
   event handling.  Note, however, that the text widget will NOT pass events that set
   the PROP_SELECTED_BGCOLOR property onto the border, instead choosing to handle this
   event as described below.  Events not handled by the border will be processed by the
   text widget itself, as follows:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETFOCUS:     The text widget responds to this event by accepting the focus, then
                      passes the event on to its border so that the border may be modified
                      as necessary to reflect the focus change.  Once handled, the text
                      widget returns TRUE to prevent any other objects from attempting to
                      claim the focus.
                      
EVT_WDG_SETPROPERTY:  The text widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The text widget
                      allows the following properties to be set:
                      
                          PROP_FONT              --  Sets the font to be used when displaying the
                                                     text.
                          PROP_FONT_CLASS        --  Sets the font class to be used when displaying
                                                     text.
                          PROP_FONT_OUTLINEWIDTH --  Sets the font's outline width.  Must first
                                                     use PROP_FONT to set the font to a font
                                                     object that understands this property, no
                                                     effect otherwise.
                          PROP_FONT_OUTLINECOLOR --  Sets the font's outline color.  Must first
                                                     use PROP_FONT to set the font to a font
                                                     object that understands this property, no
                                                     effect otherwise.
                          PROP_LINEGAP           --  Sets the spacing between lines of text.
                          PROP_FGCOLOR           --  Sets the color of the text.
                          PROP_SELECTED_BGCOLOR  --  Sets the selection color of the text
                          PROP_SELECTED_FGCOLOR  --  Sets the selected text color
                          PROP_FLAGS             --  Sets various flags that effect how the text
                                                     will be displayed.
                          PROP_PASSWORDMASKCHAR  --  Sets the replacement character that will be
                                                     displayed when the text widget has been
                                                     designated as a password field.
                          PROP_CARETWIDGET       --  Sets the caret widget.
                          PROP_TEXTCONTROLLERMODEKEY -- Sets the text controller's mode change key.
                      
EVT_WDG_GETPROPERTY:  The text widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  The text widget
                      allows the following properties to be retrieved:
                      
                          PROP_FONT              --  Retrieves the font used to display the text.
                          PROP_FONT_OUTLINEWIDTH --  Retrieves the current outline width of the
                                                     display text if the font supports this
                                                     property.
                          PROP_FONT_OUTLINECOLOR --  Retrieves the current outline color of the
                                                     display text if the font supports this
                                                     property.
                          PROP_LINEGAP           --  Retrieves the spacing between lines of text.
                          PROP_FGCOLOR           --  Retrieves the text color.
                          PROP_FLAGS             --  Retrieves the flags that dictate how the
                                                     text will be displayed.
                          PROP_VIEWMODEL         --  Retrieves the view model attached to the text widget, 
                                                     incrementing the reference count of the view model.
                          PROP_CARETPOS          --  Retrieves the current caret position.
                          PROP_CARETWIDGET       --  Retrieves the caret widget.
                          PROP_TEXTCONTROLLERMODEKEY -- Retrieves the text controller's mode change key.
                          PROP_TEXTRECTS         --  Retrieves an array of rectangles that represent the 
                                                     display location of a range of text.
===/pre>   

   The text widget also responds to and initiates several model events, as follows:
   
===pre>
Event                   Description
-----                   ------------------------------------------------------------
EVT_MDL_TEXT_CHANGE:    The text widget listens for changes in its attached text model and will
                        receive an EVT_MDL_TEXT_CHANGE model event when the text stored within
                        the text model changes.  The text widget responds to these changes by
                        invalidating its bounding rectangle and redrawing.
                      
EVT_MDL_TEXT_SELECT:    The text widget listens for changes in its attached text model and will
                        receive an EVT_MDL_TEXT_CHANGE model event when the selection state of
                        the text changes.  The text widget responds to these changes by
                        invalidating its bounding rectangle and redrawing.
                      
EVT_MDL_SCROLL_CHANGE:  The text widget generates EVT_MDL_SCROLL_CHANGE events to notify
                        listening objects of changes to the scroll state of the displayed text.
                        For example, in a multiline text widget, when the text has flowed past
                        the widget's vertical extent an EVT_MDL_SCROLL_CHANGE event will be
                        generated to inform other objects that the scroll state has changed.
                        An attached object such as a scrollbar could then redraw itself to
                        reflect the new scroll position.
===/pre>   
                      
Properties:
Property                 Description
--------                 ------------------------------------------------------------
PROP_FONT:               This property contains a pointer to the IFont that will be used to render
                         the text displayed within the text widget.
                                
                             Property Value:  IFont *

PROP_FONT_CLASS:         This property sets the class id to be used when rendering text displayed 
                         with the text widget. It is a shortcut for PROP_FONT as it saves having
                         to create the font and passing it in using PROP_FONT.

                             Property Value: AEECLSID

PROP_FONT_OUTLINEWIDTH:  This property sets the width of the outline to be applied when rendering
                         text within the static widget.  Setting this property only effects the
                         text if an IFont object (such as an IHFont)  that understands this 
                         property was set earlier via the PROP_FONT property.

                             Property Value: int

PROP_FONT_OUTLINECOLOR:  This property sets the color of the outline to be applied when rendering
                         text within the static widget.  Setting this property only effects the
                         text if an IFont object (such as an IHFont)  that understands this 
                         property was set earlier via the PROP_FONT property.

                             Property Value: RGBAVAL

PROP_FLAGS:              This property contains a set of flags that dictate the display
                         characteristics of the text being entered in the text widget.  
                         The text widget defines the following widget specific flags:
                              
                         TWF_MULTILINE
                         -------------
                         When this flag is set the text displayed by the text widget will wrap
                         at the widget's horizontal extent, thereby providing multiline text
                         entry.  When this flag is clear text entry is on a single line and does
                         not wrap -- however, the text may extend beyond the horizontal extent,
                         thereby providing an opportunity to track or manage horizontal scrolling
                         for this text.
                              
                         TWF_VERTICALNAV
                         ---------------
                         When this flag is set the text displayed by the text widget may be
                         navigated vertically via the AVK_UP and AVK_DOWN virtual key codes.
                         Events that indicate a press of either of these keys will move the
                         text entry caret to a new line, in the direction specified by the
                         pressed key.
                              
                         TWF_PASSWORD
                         ------------
                         When this flag is set, text entered into the text widget will
                         replaced by the character contained in the PROP_PASSWORDMASKCHAR
                         property, thereby lending a sense of on-screen privacy to the
                         password being entered by the user.  For example, if an application
                         wishes to provide a field into which the user enters credit card
                         information, they may wish to set the TWF_PASSWORD flag on the
                         text widget.  As the use keys in their credit card, the character
                         stored in the PROP_PASSWORDMASKCHAR property (by default, the '*"
                         character) will appear in the widget rather than the typed text.

                         TWF_SHORTENTEXT
                         ---------------
                         When this flag is set and the widget is no longer active, the widget
                         will display the start of the string.  The text that does not fit into
                         the display window will be truncated into an ellipsis.

                             Property Value:  uint32

PROP_FGCOLOR:            This property contains the color the text widget will use to draw
                         its text.  Note that the text widget maintains two separate text colors:
                         PROP_FGCOLOR, which provides the text color when entering text, and
                         PROP_SELECTED_FGCOLOR, which provides the text color of text when it is
                         selected.
                             
                             Property Value:  RGBVal

PROP_SELECTED_FGCOLOR:   This property contains the color the text widget will use to draw
                         selected text.  Note that the text widget maintains two separate text
                         colors:  PROP_SELECTED_FGCOLOR, which provides the text color of text
                         when it is selected, and PROP_FGCOLOR, which provides the text color when
                         entering text.
                             
                             Property Value:  RGBVal

PROP_SELECTED_BGCOLOR:   This property contains the color the text widget will use to draw
                         selected text.  This is NOT the color of the text, rather, it is the
                         background color upon which the selected text is drawn.  When text is
                         unselected, it is drawn over the normal background of the text widget
                         using PROP_FGCOLOR.  When the text is selected, it is drawn over a
                         PROP_SELECTED_BGCOLOR background, using PROP_SELECTED_FGCOLOR to render
                         the text.
                         
                             Property Value:  RGBVal
                             
PROP_VIEWMODEL:          This property contains a pointer to the view model that may be attached
                         to the text widget, returning that pointer in an IModel pointer passed
                         in the 'dwParam' parameter passed to the text widget with the
                         EVT_WDG_GETPROPERTY event.  The text widget uses this model as a
                         vehicle for notifying attached objects of scroll changes.  For example,
                         when the text entered into a single line text widget extends beyond the
                         widget's horizontal extent, a scroll event will be communicated to other
                         objects via the text widget's view model.
                              
                         The text widget does not support attempts to set the PROP_VIEWMODEL
                         property.

                             Property Value:  IModel *

PROP_CARETPOS:           This property contains the current position of the caret, relative to the
                         origin of the text widget.  The position is stored as a 32 bit value with
                         the horizontal position stored in the lower 16 bits, and the vertical
                         position stored in the upper 16 bits.

                             Property Value:  uint32

PROP_CARETWIDGET:        This property contains the widget used to draw the caret.  The text widget
                         creates a caret widget by default, but a custom caret can also be set. Setting
                         this property to NULL will cause no caret to be displayed.

                             Property Value:  IWidget *

PROP_STARTLINE           This property allows multiline text widgets to obtain the position of the
                         start of the line corresponding to the input position, This property is
                         used with IWIDGET_GetProperty only. The dwParam argument is an in/out 
                         parameter of type pointer to int. On entry, the value pointed to is a
                         position for which the line start position will be returned. On exit,
                         the returned value is the line start position.
                         
                             Property Value:  int *

PROP_PREVLINE            This property allows multiline text widgets to obtain the position of the
                         start of the previous line corresponding to the input position, This 
                         property is used with IWIDGET_GetProperty only. The dwParam argument is 
                         an in/out parameter of type pointer to int. On entry, the value pointed to 
                         is a position for which the previous line start position will be returned. 
                         On exit, the returned value is the previous line's start position.
                         
                             Property Value:  int *

PROP_NEXTLINE            This property allows multiline text widgets to obtain the position of the
                         start of the following line corresponding to the input position, This 
                         property is used with IWIDGET_GetProperty only. The dwParam argument is 
                         an in/out parameter of type pointer to int. On entry, the value pointed to 
                         is a position for which the next line's start position will be returned. 
                         On exit, the returned value is the next line's start position.
                         
                             Property Value:  int *

PROP_TEXTCONTROLLER      This property allows the caller to get the current text widget's controller,
                         or replace the current text widget's controller with an app-supplied controller.
                         
                             Property Value:  IController *

PROP_PASSWORDMASKCHAR    This property contains the replacement character to be used in password
                         entry fields.  When the TWF_PASSWORD property flag has been set on the
                         text widget, all text entered into that field will be displayed using
                         the character contained in PROP_PASSWORDMASKCHAR.
                         
                             Property Value:  AECHAR
                             
PROP_PASSWORDMASKDELAY   This property contains the time delay in milliseconds from the time a
                         character is typed and initially displayed until the time that it is
                         replaced by the mask character (PROP_PASSWORDMASKCHAR). Use a value of
                         zero to never display the typed character and only display the mask
                         character.
                         
                             Property Value: int


PROP_TEXTCONTROLLERMODEKEY  
                         This property contains the virtual key code that will be used by 
                         the text controller to cycle through the valid text input modes.  
                         Use AVK_UNDEFINED to disallow changing of the input mode by the user.

                             Property Value: AVKType

PROP_TEXTRECTS
                         This property gets an array of rectangles that represent the display 
                         location of a range of text.

                             Property Value: WidgetTextRects*

                         The WidgetTextRects structure is defined and used as follows:

                         typedef struct {
                              uint32   count;  // in: if pRects is not equal to NULL, then count 
                                               //     contains the number of AEERect allocated in
                                               //     pRects.
                                               // out: output contains the number of rects needed
                                               //      to locate the range of text,
                              uint32   flags;  // in: for future expansion, currently must be zero.
                              uint32   start;  // in: starting character index to range of text
                              uint32   end;    // in: ending character index to range of text
                              AEERect* pRects; // out: array of rects that locate the range of text.
                                               //      using the text widget coordinates.
                                               //      
                         } WidgetTextRects;

IWIDGET_GetTextRects

                             
Required Model:
   ITextModel Interface
   IModel Interface
   
Model Data:
===pre>   
   TextInfo modelData;
===/pre>
   
   The text widget relies on a text model that contains a TextInfo data structure that contains
   information about and a pointer to the text that is being entered.  The text widget uses this
   data to measure, position and draw the text.  The model should return a pointer to this data
   structure in response to data queries from a client widget.
   
   The text widget also relies on a view model that it simply uses as a conduit for sending scroll
   events to listening objects.
   
   The models used by the text widget are created by the widget itself as default implementations
   but may be replaced by any client that wishes to provide their own model.  However, clients that
   provide their own model interface to the text widget must abide by the data conventions described
   above, returning a pointer to a TextInfo data structure when queried for the model data.
   
Comments:
   None
   
See Also:
   Model Events
   IModel Interface
   ITextModel Interface
   ISHELL_CreateInstance()
   IWIDGET_GetModel()
   IWIDGET_SetFlags()
   IWIDGET_SetExtent()
   IDECORATOR_SetWidget()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_GetTextWidgetText()

Description:
   Retrieves the text stored in a text widget from that widget's text model.  A pointer
   to the text is returned in the 'ppo' parameter.
   
Prototype:
   int IWIDGET_GetTextWidgetText(IWidget *piWidget, AECHAR **ppo);
   
Parameters:
===pre>   
   piWidget:  Pointer to the IWidget object.
   ppo:       Pointer to an AECHAR pointer into which the function will return a pointer
              to the text currently contained in the text widget.
===/pre>

Return Value:
===pre>   
   SUCCESS - If the text widget is able to successfully return a pointer to the text.
   EFAILED - If the text could not be accessed and returned.
===/pre>   
   
Comments:
   The text pointer returned by IWIDGET_GetTextWidgetText() is the same pointer stored
   in the 'pwText' field of the TextInfo data structure maintained by the text model.

Side Effects: 
   None

See Also: 
   IWidget
   AECHAR
   IWIDGET_SetTextWidgetText()

===========================================================================

IWIDGET_SetTextWidgetText()

Description:
   Replaces _all_ the text currently displayed in the text widget with the text passed in
   'txt', regardless of the current selection range.
   
Prototype:
   int IWIDGET_SetTextWidgetText(IWidget *piWidget, AECHAR *txt);
   
Parameters:
===pre>   
   piWidget:  Pointer to the IWidget object.
   txt:       Pointer to the text that is to be placed into the text widget, or NULL is
              the text that currently appears in the text widget should be cleared.
===/pre>

Return Value:
===pre>   
   SUCCESS - If the text widget is able to successfully set the text
   EFAILED - If the text could not be set.
===/pre>   
   
Comments:
   None

Side Effects: 
   setting the text in a text widget by calling IWIDGET_SetTextWidgetText() will cause BREW to
   send an EVT_MDL_TEXT_CHANGE ModelEvent to any object that has registered as a listener
   with the widget's text model -- including the text widget itself, which of course triggers
   the text widget to be redrawn with the new text.

See Also: 
   ITextModel
   IWidget
   AECHAR
   IWIDGET_GetTextWidgetText()

===========================================================================

IWIDGET_GetTextLimit()

Description:
   This function is used to retrieve the maximum number of characters that can be entered
   into the given text widget.  For example, a text widget that invites the user to enter
   a 5-digit postal ZIP code would set its maximum number of characters to 5, and return
   this value in response to IWIDGET_GetTextLimit().  The limit, though set on the text
   widget, is actually observed by the attached controller as the user enters text.

Prototype:
   int IWIDGET_GetTextLimit(IWidget *po, int *pnMaxChars)

Parameters:
===pre>
   po:          Pointer to the text widget object.
   pnMaxChars:  A pointer to an int into which the text widget will copy the character
                limit to be observed by the text widget's controller.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully retrieve the text limit
   EFAILED - If the text limit could not be retrieved
===/pre>

Comments:
   None  

Side Effects: 
   IWIDGET_GetTextLimit() calls IWIDGET_GetPropInt() to retrieve the PROP_LIMITTEXT property.

See Also:
   IWidget
   IWIDGET_GetPropInt()
   IWIDGET_SetTextLimit()

==========================================================================

IWIDGET_SetTextLimit()

Description:
   This function is used to place a limit on the number of characters that may be entered
   into a text widget.  For example, a Magic Number Guessing application might provide a
   text widget that asks the user to guess a number between 0 and 100, thereby limiting the
   user's input to two characters.  The application would set this limit by passing 2 to
   IWIDGET_SetTextLimit() and the attached text controller would then limit the user's
   ability to enter more than just two characters.
   
   By the way, the number I was thinking of was 42.

Prototype:
   int IWIDGET_SetTextLimit(IWidget *po, int nMaxChars)

Parameters:
===pre>
   po:       Pointer to the text widget  object.
   nFrames:  Specifies the maximum number of characters that may be entered into the text
             widget.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the maximum number of characters for the
             text widget.
   EFAILED - If the maximum number of characters for the widget could not be set.
===/pre>

Comments:
   None  

Side Effects: 
   IWIDGET_SetTextLimit() calls IWIDGET_SetProperty() to set the PROP_LIMITTEXT property.
   This has the net effect of processing an EVT_WDG_SETPROPERTY event through the widget
   hierarchy.

See Also:
   EVT_WDG_SETPROPERTY
   IWidget
   IWIDGET_SetProperty()

===========================================================================

IWIDGET_GetInputMode()

Description:
   This function is used to retrieve the input mode of the associated text controller.
   See AEETextController.h for possible values.

Prototype:
   int IWIDGET_GetInputMode(IWidget *po, int *pnMode)

Parameters:
===pre>
   po:          Pointer to the text controller object.
   pnMode:      A pointer to an int to retrieve the input mode.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully retrieve the input mode.
   EFAILED - If the input mode could not be retrieved.
===/pre>

Comments:
   None  

Side Effects: 
   IWIDGET_GetInputMode() calls IWIDGET_GetPropInt() to retrieve the PROP_INPUTMODE property.

See Also:
   TextController
   IWIDGET_SetInputMode()

==========================================================================

IWIDGET_SetInputMode()

Description:
   This function is used to set the input mode of the associated text controller.
   See AEETextController.h for possible values.

Prototype:
   int IWIDGET_SetInputMode(IWidget *po, int nMode)

Parameters:
===pre>
   po:       Pointer to the text controller object.
   nMode:    Specifies the input mode.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the input mode.
   EFAILED - If the input mode could not be set.
===/pre>

Comments:
   None  

Side Effects: 
   IWIDGET_SetInputMode() calls IWIDGET_SetProperty() to set the PROP_INPUTMODE property.

See Also:
   TextController
   IWIDGET_GetInputMode()

===========================================================================

IWIDGET_GetTextController()

Description:
   This function is used to retrieve the text controller for the text widget object.  
   The caller is responsible for releasing the text controller when finished with it.

Prototype:
   int IWIDGET_GetTextController(IWidget *po, IController **pController)

Parameters:
===pre>
   po:           Pointer to the text widget object.
   pController:  Pointer to an IController pointer.  On return points to the
                 text controller.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully retrieve the text controller.
   EFAILED - If the text controller could not be retrieved.
===/pre>

Comments:
   None  

Side Effects: 
   IWIDGET_GetTextController() calls IWIDGET_GetProperty() to retrieve the 
   PROP_TEXTCONTROLLER property.

See Also:
   IController
   ICONTROLLER_Release()
   IWIDGET_SetTextController()

==========================================================================

IWIDGET_SetTextController()

Description:
   This function is used to set the text controller for the text widget object.
   It will also increment the text controller reference count.

Prototype:
   int IWIDGET_SetTextController(IWidget *po, IController *pController)

Parameters:
===pre>
   po:           Pointer to the text widget object.
   pController:  Pointer to the text controller.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the text controller.
   EFAILED - If the text controller could not be set.
===/pre>

Comments:
   None  

Side Effects: 
   IWIDGET_SetTextController() calls IWIDGET_SetProperty() to set the PROP_TEXTCONTROLLER property.

See Also:
   IController
   ICONTROLLER_AddRef()
   IWIDGET_GetTextController()

===========================================================================

IWIDGET_GetCaretWidget()

Description:
   This function is used to retrieve the caret widget for the text widget object.  
   The caller is responsible for releasing the caret widget when finished with it.

Prototype:
   int IWIDGET_GetCaretWidget(IWidget *po, IWidget **piCaret)

Parameters:
===pre>
   po:           Pointer to the text widget object.
   piCaret:      Pointer to an IWidget pointer.  On return points to the
                 caret widget.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully retrieve the caret widget.
   EFAILED - If the caret widget could not be retrieved.
===/pre>

Comments:
   None  

Side Effects: 
   IWIDGET_GetCaretWidget() calls IWIDGET_GetProperty() to retrieve the 
   PROP_CARETWIDGET property.

See Also:
   CaretWidget
   IWIDGET_Release()

===========================================================================

IWIDGET_SetCaretWidget()

Description:
   This function is used to set a custom caret widget for the text widget object.
   Setting the caret widget to NULL will cause no caret to be displayed.

Prototype:
   int IWIDGET_SetCaretWidget(IWidget *po, IWidget **piCaret)

Parameters:
===pre>
   po:           Pointer to the text widget object.
   piCaret:      Pointer to the caret widget object.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the caret widget.
   EFAILED - If the caret widget could not be set.
===/pre>

Comments:
   None  

Side Effects: 
   IWIDGET_SetCaretWidget() calls IWIDGET_SetProperty() to retrieve the 
   PROP_CARETWIDGET property.

See Also:
   CaretWidget
   IWIDGET_Release()


===========================================================================

IWIDGET_GetTextControllerModeKey

Description:
   This function is used to retrieve the virtual key code that the text controller 
   uses change the text input mode. 

Prototype:
   int IWIDGET_GetTextControllerModeKey(IWidget *po, AVKType *pAvk}
   
Return Value:
===pre>
   SUCCESS - If BREW was able to successfully retrieve mode key
   EFAILED - If the property was not handled
===/pre>

Comments:
   None  

Side Effects: 
   IWIDGET_GetTextControllerModeKey() calls IWIDGET_GetProperty() to retrieve the 
   PROP_TEXTCONTROLLERMODEKEY property.

See Also:
   IWIDGET_SetTextControllerModeKey
   PROP_TEXTCONTROLLERMODEKEY


===========================================================================

IWIDGET_SetTextControllerModeKey

Description:
   This function is used to set a virtual key code that the text controller 
   will respond to in order to change the text input mode. By default the 
   text controller uses AVK_STAR as the mode change key.  Use AVK_UNDEFINED
   to disable text input mode changes by the user.

Prototype:
   int IWIDGET_SetTextControllerModeKey(IWidget *po, AVKType avk}
   
Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the mode key
   EFAILED - If the property was not handled
===/pre>

Comments:
   None  

Side Effects: 
   IWIDGET_SetTextControllerModeKey() calls IWIDGET_SetProperty() to set the 
   PROP_TEXTCONTROLLERMODEKEY property.

See Also:
   IWIDGET_GetTextControllerModeKey
   PROP_TEXTCONTROLLERMODEKEY


===========================================================================


*/

#endif      /* __AEETEXTWIDGET_H__ */

