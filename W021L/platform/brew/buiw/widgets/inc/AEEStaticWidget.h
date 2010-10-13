/*
  ========================================================================

  FILE:  AEEStaticWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Defines a static, label-like widget

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "AEERichTextModel.h"
#include "AEEWidget.h"
#include "AEEWProperties.h"


#ifndef __AEESTATICWIDGET_H__
#define __AEESTATICWIDGET_H__

#include "bid/AEECLSID_STATICWIDGET.bid"

#define SWF_WRAPTEXT          0x00100000                 // wrap text to fit in widget rect
#define SWF_NOSHORTENTEXT     0x00200000                 // don't shorten text that doesn't fit
#define SWF_WRAPNOBREAK       0x00400000                 // wrap text but don't break on white space
#define SWF_MULTILINESHORTEN (0x00800000|SWF_WRAPTEXT)   // wrap and truncate text with an ellipsis in a multiline
#define SWF_FLAGSMASK         0x000FFFFF                 // isolate the IDF_xxx drawing flags


static __inline int IWIDGET_SetText(IWidget *piWidget, const AECHAR *txt, boolean bFree)
{  
   int nErr = EFAILED;
   IValueModel *pivm;
   IRichTextModel *pirtm;

   if (piWidget) {

      // check the widget for a RichTextModel.
      nErr = IWIDGET_GetModel(piWidget, AEEIID_RICHTEXTMODEL, (IModel **)&pirtm);
      if (pirtm && (SUCCESS == nErr)) {
         IRICHTEXTMODEL_SetMarkupText(pirtm, txt, (txt == 0 ? 0 : WSTRLEN(txt)));
         IRICHTEXTMODEL_Release(pirtm);
      } else {
         // NULL model
         nErr = EFAILED;
      }

      // check the widget for a value model.
      if (SUCCESS != nErr) {
         nErr = IWIDGET_GetModel(piWidget, AEEIID_VALUEMODEL, (IModel **)&pivm);
         if (pivm && (SUCCESS == nErr)) {
            IVALUEMODEL_SetValue(pivm, (void *)txt, 
                        (txt == 0 ? 0 : WSTRLEN(txt)), 
                        (bFree ? WidgetAEEFree : 0));
            IVALUEMODEL_Release(pivm);
         } else {
            // NULL model
            nErr = EFAILED;
         }
      }
   }

   return nErr;
}

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Static Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   static widget by creating a new instance of the Static Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the static widget is defined below.

Definition:

Identifier              Definition
----------              -----------------------------------
AEECLSID_STATICWIDGET:  Identifies the class ID of the static widget
   
Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Static Widget Class

Description:
   The static widget is a user interface element that displays static (i.e. read-only)
   text on the display.  For example, instructions for a game, the text of a short poem,
   or the labels of a softkey widget could all be presented within a static widget.
   However, while the static widget can easily manage the text for a title, label, or
   short message, it is important to note that the static widget alone is not sufficient
   for supporting the user interface for long sequences of text.  For example, an
   application that wishes to display lengthy passages of text would likely wrap the
   static widget in a viewport widget, with the viewport widget wrapped in a scrollbar
   widget.
   
   The static widget is displayed within a border -- though this border, by default, is
   hidden.  Applications may, of course, alter the properties of the border so that the
   static text is displayed within some type of decorative frame.
   
   The static widget provided facilities for managing how the text should be wrapped and
   how the widget should display text that would otherwise flow well beyond the boundaries
   of the widget's extent.  Two property flags are used to manage how the text will be
   displayed.  SWF_WRAPTEXT indicates that the text should be wrapped to fit within the
   widget's extent.  To illustrate, consider the following text:
 
===pre>
          Once upon a time there was an old woman who lived in a shoe.  Actually, the
          shoe was a Doc Marten boot, with fine leather walls and windows where you
          would expect to find the eyelets.  The shoe did not have laces, though graceful
          vines of bright green ivy looped from window to window, creating the general
          effect of laces on the giant Doc Marten boot.  One day, the old woman decided
          that she no longer wanted to live in a shoe, so she ran off with the pool boy
          to live in a high rise condo off the coast of Florida.  She eventually went
          through some kind of rejuvenation surgery that made her look like a 20 year
          old belly dancer... and that's exactly what she became!  Of course, none of
          the patrons who attended her shows at the Moroccan restaurant where she worked
          had any idea that they were applauding the gyrations of a 97 year old woman, but
          they did so with great enthusiasm.
          
          In the meantime, her 17 children, 31 grandchildren, 28 great grandchildren, 4
          great great grandchildren and one great great GREAT grandchild (whose name was
          Manny The Wonder Child) were left without a home, as the old woman had sold her
          former home to a real estate developer who leveled the shoe-home and in its
          place built a lovely high rise Bermuda shorts condo.  The old woman's family
          took up residence under a freeway off ramp and made their living, forever more,
          holding up cardboard signs offering to "work for food".  Unfortunately, very
          few opportunities arose for work, and therefore there was very little food to
          be eaten, so the 17 children, 31 grandchildren, 28 great grandchildren, 4 great
          great grandchildren were left wondering what to do with themselves.  Ah!  But
          Manny The Wonder Child (being the illegitimate offspring of a Nobel Prize
          winner) had the good sense to invest one dollar in a Powerball drawing and --
          having used the numbers 17, 31, 28, 4, 1 and a few other numbers I've since
          forgotten -- won a cash prize of $304,000,000!!!  The family was joyous!!
          
          Alas, Manny -- though very, very bright for a wee child of 14 months -- had
          purchased his winning ticket from a convenience store that did not bother to
          check his age, and young Manny did not meet the "18 years or older to play"
          minimum requirement, so the family forfeited its winning ticket and the cash
          prize was later claimed by a high tech multimillionaire who didn't even need
          the money, but accepted the prize anyway so that he could woo a beautiful
          belly dancer he had spotted during the floor show at a Moroccan restaurant.
===/pre>

   When SWF_WRAPTEXT is set, this text will be flowed to wrap horizontally within the
   widget's boundary rectangle, clipping only at the bottom, like so:
   
===pre>
         +-----------------------------------+
         | Once upon a time there was an old |
         | woman who lived in a shoe.        |
         | Actually, the shoe was a Doc      |
         | Marten boot, with fine leather    |
         | walls and windows where you would |
         | expect to find the eyelets.  The  |
         +-----------------------------------+
===/pre>   

   Or, like so, depending on the dimensions of the widget's extent:
   
===pre>
         +--------------------------------------------------------+
         | Once upon a time there was an old woman who lived in a |
         | shoe.  Actually, the shoe was a Doc Marten boot, with  |
         | fine leather walls and windows where you would expect  |
         | to find the eyelets.  The shoe did not have laces,     |
         | though graceful vines of bright green ivy looped from  |
         | window to window, creating the general effect of laces |
         | on the giant Doc Marten boot.  One day, the old woman  |
         | decided that she no longer wanted to live in a shoe,   |
         | so she ran off with the pool boy to live in a high     |
         | rise condo off the coast of Florida.  She eventually   |
         +--------------------------------------------------------+
===/pre>   

   When SWF_WRAPTEXT is NOT set, the text will be clipped to the widget's horizontal
   extent, like so:
   
===pre>
                                                  The text below is clipped horizontally
                                                  - - - - - - - - - - - - - - - - - - - 
         +-----------------------------------+
         | Once upon a time there was an old |  woman who lived in a shoe.  Actually, the
         |                                   |
         |                                   |
         |                                   |
         |                                   |
         |                                   |
         +-----------------------------------+
===/pre>   

   Or, like so, depending on the dimensions of the widget's extent:
   
===pre>
                                                                       The text below is
                                                                      clipped horizontally
                                                                      - - - - - - - - - - 
         +--------------------------------------------------------+
         | Once upon a time there was an old woman who lived in a |  shoe.  Actually, the
         |                                                        |
         |                                                        |
         |                                                        |
         |                                                        |
         |                                                        |
         |                                                        |
         |                                                        |
         |                                                        |
         |                                                        |
         +--------------------------------------------------------+
===/pre>   

   Applications may also control how the text longer than a static widget's extent will
   displayed when clipped.  The static widget will, by default, shorten text that does
   not fit the horizontal extent of the widget.  When shortened, an ellipsis (...)
   character will be displayed at the point of horizontal clipping, like so:
   
===pre>
         +-----------------------------------+
         | Once upon a time there was an...  |
         |                                   |
         |                                   |
         |                                   |
         |                                   |
         |                                   |
         +-----------------------------------+
===/pre>   

   With the ellipsis providing visual indication to the user that the text is continued,
   but not visible.

   Applications can defeat this behavior by setting the SWF_NOSHORTENTEXT flag, which
   will result in the normal truncation of the text by clipping, without the ellipsis.

   Applications may wish to wrap text within a static widget but display an ellipsis
   at the end of the visible text indicating that the text was trunctated.  This may be 
   desirable for multi-line static widgets that don't scroll.  To do this, applications
   should set the SWF_MULTILINESHORTEN flag.  This will cause the text to be wrapped 
   within the static widget and truncated at the last character with an ellipsis 
   character.  Like so:
   
===pre>
         +-----------------------------------+
         | Once upon a time there was an old |
         | woman who lived in a shoe.        |
         | Actually, the shoe was a Doc      |
         | Marten boot, with fine leather    |
         | walls and windows where you would |
         | expect to find the eyelets.  Th...|
         +-----------------------------------+
===/pre>   
   
   To create a static widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_STATICWIDGET to
          create a static widget.  ISHELL_CreateInstance() will retrieve a reference
          counted instance of the class that manages the widget.
       2. Call IWIDGET_SetText() to specify the text to be displayed within the static
          widget.
       3. Call various IWIDGET_Setxxxxxx() functions to override any of  the default
          visual characteristics of the widget.  For example, if the text should be
          displayed in red, or if the border is to be displayed, rather than hidden.
       4. Call IWIDGET_SetFlags() to specify how the text is to be aligned or wrapped.
       5. Call IWIDGET_SetExtent() to set the size of the rectangle enclosing the text.
===/pre>

   The static widget, by default, will be created with the following characteristics:

===pre>
         Active border width:  0 pixels  (i.e. the border is hidden)
       Inactive border width:  0 pixel
                  Text color:  RGB_BLACK
            Background color:  RGB_NONE
                Border color:  RGB_NONE
        Text alignment Flags:  Middle/Left
                   Text Font:  AEECLSID_FONTSYSNORMAL
                Extent Width:  30
               Extent Height:  12
===/pre>
        
   The text, by default, will not wrap and will be clipped, rather than truncated with an
   ellipsis.

   Text that is too long to display in the static widget can be scrolled right to left.  
   To turn text scrolling on or off, call IWIDGET_SetProperty() with the PROP_ANIMATE property.  
   To set scrolling behavior, call IWIDGET_SetProperty() with the PROP_SCROLLTEXT property.

Supported Events:
   The static widget processes events in two passes.  In the first pass the static widget
   will pass all events to the widget's border, and return TRUE if the border accepts and
   processes the event.  If the event is not handled, the static widget will attempt to
   process the event itself, handling the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The static widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The static widget
                      allows the following properties to be set:
                      
                          PROP_FONT              --  Sets the font to be used when displaying the
                                                     text.
                          PROP_FONT_CLASS        --  Sets the font class to be used when displaying
                                                     text
                          PROP_FONT_OUTLINEWIDTH --  Sets the font's outline width.  Must first
                                                     use PROP_FONT to set the font to a font
                                                     object that understands this property, no
                                                     effect otherwise.
                          PROP_FONT_OUTLINECOLOR --  Sets the font's outline color.  Must first
                                                     use PROP_FONT to set the font to a font
                                                     object that understands this property, no
                                                     effect otherwise.
                          PROP_LINEGAP           --  Sets the spacing between lines of text.
                          PROP_FLAGS             --  Set various flags that effect how the text
                                                     will be aligned.
                          PROP_FGCOLOR           --  Sets the text color to be used when the
                                                     widget is selected, unselected, active or
                                                     inactive.
                          PROP_SELECTED_FGCOLOR  --  Sets the text color to be used when the
                                                     widget is selected.
                          PROP_SACTIVE_FGCOLOR   --  Sets the text color to be used when the
                                                     widget is selected and active.
                          PROP_SINACTIVE_FGCOLOR --  Sets the text color to be used when the
                                                     widget is selected and inactive.
                          PROP_ACTIVE_FGCOLOR    --  Sets the text color to be used when the
                                                     widget is active.
                          PROP_INACTIVE_FGCOLOR  --  Sets the text color to be used when the
                                                     widget is inactive.
                          PROP_HINT_WIDTH        --  Sets the preferred extent width of the
                                                     widget.
                          PROP_HINT_ROWS         --  Sets the preferred number of rows to
                                                     display.
                          PROP_SCROLLTEXT        --  Sets the text scrolling properties.
                          PROP_ANIMATE           --  Sets text scrolling on or off.
                          PROP_ANIMATE_FLAGS     --  Sets the animation flags for the widget.
                          PROP_DEFAULTTEXTALIGNMENT
                                                 --  Sets the default text alignment.
                          PROP_ELLIPSIS          --  Allows caller to override what character
                                                     is used for ellipsis truncation.  
                      
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The static widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  The static widget
                      allows the following properties to be retrieved:
                      
                          PROP_FONT              --  Retrieves the font used to display the text.
                          PROP_FONT_OUTLINEWIDTH --  Retrieves the current outline width of the
                                                     display text if the font supports this
                                                     property.
                          PROP_FONT_OUTLINECOLOR --  Retrieves the current outline color of the
                                                     display text if the font supports this
                                                     property.
                          PROP_LINEGAP           --  Retrieves the spacing between lines of text.
                          PROP_FLAGS             --  Retrieves the flags that dictate how the
                                                     text will be aligned.
                          PROP_FGCOLOR           --  Retrieves the text color that will be used
                                                     when the widget is active.
                          PROP_SELECTED_FGCOLOR  --  Retrieves the text color that will be used
                                                     when the widget is selected.
                          PROP_SACTIVE_FGCOLOR   --  Retrieves the text color that will be used
                                                     when the widget is selected and active.
                          PROP_SINACTIVE_FGCOLOR --  Retrieves the text color that will be used
                                                     when the widget is selected and inactive.
                          PROP_ACTIVE_FGCOLOR    --  Retrieves the text color that will be used
                                                     when the widget is active.
                          PROP_INACTIVE_FGCOLOR  --  Retrieves the text color that will be used
                                                     when the widget is inactive.
                          PROP_HINT_WIDTH        --  Retrieves the preferred extend width of the
                                                     widget.
                          PROP_HINT_ROWS         --  Retrieves the preferred number of rows to
                                                     display.
                          PROP_ANIMATE_FLAGS     --  Retrieves animation-specific flags.
                          PROP_DEFAULTTESTALIGNMENT  
                                                 --  Retrieves the default text alignment.
                          PROP_ELLIPSIS          --  Retrieves the character used to indicate
                                                     text has been truncated.

===/pre>   

Properties:
Property                 Description
--------                 ------------------------------------------------------------
PROP_FONT:               This property contains a pointer to the IFont that will be used to render
                         the text displayed within the static widget.
                                
                             Property Value:  IFont *

PROP_FONT_CLASS:         This property sets the class id to be used when rendering text displayed 
                         with the static widget. It is a shortcut for PROP_FONT as it saves having
                         to create the font and passing it in.

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

PROP_FLAGS:              This property contains a set of  flags that dictate the layout and
                         alignment of the text displayed by the static widget.  In addition to
                         those flags traditionally used to position and align text (see the
                         various IDF_xxx flags in AEEDisp.h), the static widget defines the
                         following widget specific flags:
                              
                         SWF_WRAPTEXT
                         ------------
                         When this flag is set the text displayed by the static widget will be
                         wrapped within the widget's horizontal extent, and clipped at the vertical
                         extent.  This flag takes priority over SWF_NOSHORTENTEXT -- i.e. if
                         SWF_WRAPTEXT, the text will wrap and not shortened, regardless of whether
                         or not SWF_NOSHORTENTEXT is set.
                              
                         SWF_NOSHORTENTEXT
                         -----------------
                         When this flag is set the text displayed by the static widget will be
                         truncatated by clipping without an ellipsis character.  This flag
                         can be used to turn off the default behavior of truncating text and
                         adding an ellipsis to text that extends beyond the horizontal width
                         of a single line static widget.
                         

                         SWF_MULTILINESHORTEN
                         --------------------
                         When this flag is set the text displayed by the static widget will
                         be wrapped to fit within the widget and truncated at the last character 
                         visible with an ellipsis (...) displayed as the trailing character to 
                         indicate additional text is present but not displayed.  If all the text 
                         fits within the extent of the widget, the text is wrapped and displayed 
                         without an ellipsis.  

                         Note, a single line static text widget defaults to truncating text
                         when necessary and displaying an ellipsis character.  The
                         SWF_NOSHORTENTEXT flag must be set to turn this default property off.  
                         A multi-line static widget defaults to the opposite - text is not 
                         truncated with an ellipsis. The SWF_MULTILINESHORTEN flag is used to 
                         truncate text and draw the ellipsis.  Note, SWF_MULTILINESHORTEN takes 
                         priority over all others, and an ellipsis will be drawn even when 
                         SWF_WRAPTEXT is set.

PROP_FGCOLOR:            This property contains the color the static widget will use to draw
                         its text.  Setting PROP_FGCOLOR will set the text color for all
                         combinations of widget selection and activation, as indicated by the
                         following table:
                             
                                                                       Text color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         PROP_FGCOLOR
                               Unselected         Active           PROP_FGCOLOR
                               Selected           Inactive         PROP_FGCOLOR
                               Selected           Active           PROP_FGCOLOR
                             
                         Though setting the PROP_FGCOLOR will effect _all_ combinations of
                         selection and activation, getting the value of the property will return
                         the text color that will be applied when drawing the widget in its active,
                         unselected state.
                             
                             Property Value:  RGBVal

PROP_SELECTED_FGCOLOR:   This property contains the color the static widget should use to draw its
                         text when the widget is selected, regardless of whether or not the widget
                         is active or inactive, as indicated by the following table:
                          
                                                                       Text color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         PROP_SELECTED_FGCOLOR
                               Selected           Active           PROP_SELECTED_FGCOLOR
                             
                         Though setting the PROP_SELECTED_FGCOLOR will effect both the active and
                         inactive states of a selected widget, getting the value of the property
                         will return the text color that will be applied when drawing the widget
                         in its selected, active state.
                             
                             Property Value:  RGBVal

PROP_SACTIVE_FGCOLOR:    This property contains the color the static widget should use to draw
                         its text when the widget is selected and active -- but neither unselected
                         or inactive, as indicated by the following table:
                          
                                                                       Text color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         Unchanged
                               Selected           Active           PROP_SACTIVE_FGCOLOR
                             
                         Getting the value of the PROP_SACTIVE_FGCOLOR property will return the
                         text color that will be applied when drawing the widget in its active,
                         selected state.
                             
                             Property Value:  RGBVal

PROP_SINACTIVE_FGCOLOR:  This property contains the color the static widget should  use to draw
                         its text when the widget is selected and inactive -- but neither
                         unselected or active, as indicated by the following table:
                          
                                                                       Text color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         PROP_SINACTIVE_FGCOLOR
                               Selected           Active           Unchanged
                             
                         Getting the value of the PROP_SINACTIVE_FGCOLOR property will return the
                         text color that will be applied when drawing the widget in its inactive,
                         selected state.
                             
                             Property Value:  RGBVal

PROP_ACTIVE_FGCOLOR:     This property contains the color the static widget should use to draw
                         its text when the widget is active and unselected, as indicated by the
                         following table:
                          
                                                                       Text color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           PROP_ACTIVE_FGCOLOR
                               Selected           Inactive         Unchanged
                               Selected           Active           Unchanged
                             
                         Getting the value of the PROP_ACTIVE_FGCOLOR property will return the
                         text color that will be applied when drawing the widget in its active,
                         unselected state.
                             
                             Property Value:  RGBVal
   
PROP_INACTIVE_FGCOLOR:   This property contains the color the static widget should use to draw
                         its text when the widget is inactive and unselected, as indicated by the
                         following table:
                          
                                                                       Text color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         PROP_INACTIVE_FGCOLOR
                               Unselected         Active           Unchanged
                               Selected           Inactive         Unchanged
                               Selected           Active           Unchanged
                             
                         Getting the value of the PROP_INACTIVE_FGCOLOR property will return the
                         text color that will be applied when drawing the widget in its inactive,
                         unselected state.
                             
                             Property Value:  RGBVal
   
PROP_HINT_WIDTH:         This property contains the preferred width of the static widget, which
                         represents the width of the widget under ideal circumstances.  This
                         property should be set when an application has enabled the SWF_WRAPTEXT
                         property flag to specify the width that will be returned by
                         IWIDGET_GetPreferredExtent().

                             Property Value:  int

PROP_HINT_ROWS:          This property contains the preferred number of rows to display, which
                         represents the height of the widget under ideal circumstances.  The 
                         height is determined by the number of rows, the font size, and the line
                         gap. This property should be set when an application has enabled the 
                         SWF_WRAPTEXT property flag to specify the height that will be returned by
                         IWIDGET_GetPreferredExtent().

                             Property Value:  int

PROP_SCROLLTEXT:         This property contains the scroll text properties including step size and
                         scroll delays.

                             Property Value:  struct ScrollText {

                                                uint16	   nScrollAmount; // step size (pixels)
                                                uint16      nScrollDelay;  // scroll speed (msec)
                                                uint16      nStartDelay;	// start delay (msec)
                                                uint16      nEndDelay;		// end delay (msec)
                                                uint16      nRepeat;       // number of times to repeat
                                                                           // (zero means infinite)

                                              }

PROP_ANIMATE:            This property contains a boolean flag to turn text scrolling on or off.

                             Property Value:  boolean

PROP_ANIMATE_FLAGS:      This property holds the flags related to static widget's animation.

                         AF_ENABLE_EVT_STEP
                         ------------------
                         This flag allows the user to enable StepEvent to be sent through the
                         widget's ViewModel.  When static widget animates, StepEvent will only
                         be passed to model listeners if this flag is set to true.
                       
                         Property Value:  uint32

PROP_DEFAULTTEXTALIGNMENT:
                         This property holds the default text alignment flag.  It is used to 
                         determine the text alignment when the PROP_FLAGS contains the 
                         horizontal alignment flag IDF_ALIGN_NONE.  Valid values to this
                         property are IDF_ALIGN_NONE, IDF_ALIGN_RIGHT, and IDF_ALIGN_LEFT.
                         This property defaults to IDF_ALIGN_NONE.  When this property is
                         set to IDF_ALIGN_NONE, the text alignment is determined using
                         the unicode characters contained in the IValueModel.
                        
                         Property Value:  uint32

PROP_ELLIPSIS:           This property contains the character to be used to indicate when 
                         text has been truncated within a static widget.  If not set, or set 
                         to NULL, the static widget will make its best guess at determining 
                         the code point for the ellipsis character based on the device encoding.  
                         Applications may set this property to override this behavior and force 
                         the widget to use a particular character as the ellipsis, such as 
                         0x2026, the Unicode code point for the ellipsis.  This may be useful 
                         for applications that use Unicode fonts that include the ellipsis
                         character but run on a device with a non-Unicode device encoding.

                             Property Value:  AECHAR

 
Required Model:
   IValueModel 
   
Model Data:
===pre>   
   AECHAR *modelData;
===/pre>
   
   The static widgets rely on a value model that contains an AEChar representing the text to
   be displayed.  The model should return a pointer to this text, as well as the text length
   in response to data queries from a client widget.
   
   A default model is created and assigned to the widget when the widget is created with 
   ISHELL_CreateInstance(). The client can replace the widget's model at any time with 
   the method: IWIDGET_SetModel(), as long as the supplied model interface is of the type 
   that the widget expects.   
   
   The data supplied in the value model must be a pointer to type AEChar.

Comments:
   None
   
See Also:
   Model Events
   ISHELL_CreateInstance()
   IWIDGET_SetText()
   IWIDGET_SetFlags()
   IWIDGET_SetExtent()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_SetHintWidth()

Description:
   Sets the preferred width of the static widget.  The preferred width is the size
   that the widget would choose to be under ideal circumstances.  For example, a
   static widget might set its preferred width to the maximum width of the text to
   be displayed.  However, the actual widget of the widget might be constrained by
   any number of factors -- the width of the display, the size of the container into
   which the text is to be displayed, the layout of the widget in relation to other
   objects on the display, etc.  BREW will attempt to honor the width passed in
   IWIDGET_SetHintWidth(), but may choose a different size depending on these
   constraints.
   
Prototype:
   int IWIDGET_SetHintWidth(IWidget *po, int nWidth);
   
Parameters:
===pre>   
   po:      Pointer to the IWidget object.
   nWidth:  The preferred width for the widget.
===/pre>

Return Value:
===pre>   
   SUCCESS - If the static widget is able to successfully set the preferred width.
   EFAILED - If the preferred width could not be set.
===/pre>   
   
Comments:
   Applications that specify that the static widget should wrap its text (by enabling
   the SWF_WRAPTEXT property flag) should call IWIDGET_SetHintWidth() to set the
   preferred width that will be returned by IWIDGET_GetPreferredExtent().

Side Effects: 
   Calling IWIDGET_SetHintWidth() will send an EVT_WDG_SETPROPERTY event to the
   static widget requesting to set the PROP_HINT_WIDTH property.

See Also: 
    WExtent
    IWidget
    IWIDGET_GetPreferredExtent()
    IWIDGET_SetPreferredExtent()
    IWIDGET_GetExtent()

===========================================================================

IWIDGET_SetText()

Description:
   Sets the text to be displayed by the static widget.  The widget can contain
   either a text model or a value model.
   A value model stores a pointer to the text memory, which may be owned by 
   the caller, or if 'bFree' is TRUE, by the value model, in which case it 
   will be freed when the value model is destroyed.
   A text model copies the display text into the model.  No pointers 
   are stored within a text model and the caller is responsible for freeing
   all memory associated with its pointers.  The bFree parameter is has 
   no effect.
   
Prototype:
   int IWIDGET_SetText(IWidget *piWidget, const AECHAR *txt, boolean bFree);
   
Parameters:
===pre>   
   piWidget:  Pointer to the IWidget object.
   txt:       Pointer to the text that is to be displayed in the widget.  This pointer
              will be stored in the value model.
   bFree:     Indicates whether or not the widget's value model is responsible for
              freeing the text. When 'bFree' is TRUE, the value model will free the
              text memory when the model is destroyed, or when it is replaced with new 
              text. When FALSE, the caller is responsible for managing the text memory.
              This parameter is only effective for widgets that contain a value model.
              The parameter is ignore for widgets with text models.
===/pre>

Return Value:
===pre>   
   SUCCESS - If the static widget is able to successfully set the text.
   EFAILED - If the text could not be set.
===/pre>   
   
Comments:
   None

Side Effects: 
   Calling IWIDGET_SetText() will change the value model and trigger an update of any
   attached objects, including the widget associated with the text.

See Also: 
    IWidget


*/

#endif /* __AEESTATICWIDGET_H__ */
