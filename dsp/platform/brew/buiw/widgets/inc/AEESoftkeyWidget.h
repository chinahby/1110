/*
  ========================================================================

  FILE:  AEESoftkeyWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: A softkeywidget shows a non-focusable
  horizontal strip with two labels that assign meaning to the physical
  softkeys on the device. Additionally the softkeywidget has a
  scrollindicator in its center which can be used to indicate scroll
  position for other elements on the screen.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEESOFTKEYWIDGET_H__
#define __AEESOFTKEYWIDGET_H__


#include "bid\AEECLSID_SOFTKEYWIDGET.bid"

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Softkey Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   softkey widget by creating a new instance of the Softkey Widget class, passing the
   ID of the class to ISHELL_CreateInstance().  The object that is created is, itself,
   a widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the softkey widget is defined below.

Definition:

Identifier               Definition
----------               -----------------------------------
AEECLSID_SOFTKEYWIDGET:  Identifies the class ID of the softkey widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Softkey Widget Class

Description:
   The softkey widget is a user interface element that provides software navigation to
   devices that possess a pair of hardware buttons directly below the display.  These
   buttons are commonly referred to as "softkey buttons" and are usually accompanied
   by a pair of commands placed on the display directly above each button -- one
   command for the left button, and one for the right.  Though the buttons remain
   fixed (well, duh, buttons are hardware), the commands they map to change -- making
   these keys "soft"... voila!  Softkeys!
   
   To illustrate, imagine a device with a pair of buttons -- completely separate from
   the rest of the keypad -- placed directly below the screen.  The illustration below
   depicts such an environment, using lovely ASCII text (oooo! ahhhh!)
   
===pre>
             |                                         |
             |      Do you _really_ want to send       |
             |     that mulletheaded photo of your     |
             |        boss to everyone at work?        |
             |                                         |
             +--------------------+--------------------+
             |       Cancel       |         OK         | <--- Softkey Widget
             +--------------------+--------------------+
                  ============         ============
                 |            |       |            |
Left Softkey --> |            |       |            | <-- Right Softkey
                  ============         ============
===/pre>
   
   In the above example, when the user presses the left softkey button, they would be
   selecting the command positioned immediately above -- "Cancel" -- which would be
   designated as softkey one in the softkey widget.  Pressing the right softkey would
   select the item on the right of the widget, softkey two.
   
   As the user navigates through an application, the commands presented in the softkey
   widget would change to reflect the current application context, as revealed through
   the main body of the display.

   Functionally, the softkey widget is a prop container consisting of a pair of embedded
   static text widgets:  softkey one and softkey two.  Though the layout and interaction
   of these widgets are handled together by the softkey widget, each is displayed and
   functions independent of the other.  For example, an application may choose to alter
   the color or font of one softkey to place emphasis on one button or the other.

   
   To create a softkey widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_SOFTKEYWIDGET to
          create a softkey widget.  ISHELL_CreateInstance() will retrieve a reference
          counted instance of the class that manages the widget.
       2. IWIDGET_SetText() to specify the text to be displayed within each of the
          softkeys.
       3. Call IWIDGET_SetExtent() to set the size of the softkey widget.
       4. As necessary, call various IWIDGET_Setxxxxxx() functions to override any of
          the default visual characteristics of the widget.  For example, if the
          text of one softkey should be bright blue, or if the border of the widget
          should possess a shadow. 
===/pre>

   The softkey widget, by default, will be created with the following characteristics:

===pre>
         Active border width:  0 pixels
       Inactive border width:  0 pixel
               Right Padding:  4 pixels
            Foreground color:  RGB_BLACK
            Background color:  RGB_NONE
                 Label Flags:  Middle/Left (Softkey One)
                               Middle/Right (Softkey Two)
                Extent Width:  50
               Extent Height:  10
===/pre>

Supported Events:
   The softkey widget, as a widget that embeds multiple objects in a prop container,
   will first pass all received events to the property container, returning TRUE if
   the event is accepted and handled by the container.  These events will include those
   that manipulate the border, shadow, color and other visual attributes of the widget.
   If the event is not handled by the container, the softkey widget will attempt to
   process the event itself.  Finally, if the event is handled by neither the container
   nor the softkey widget, it will be passed to each of the widgets embedded in the
   container.
   
   The softkey widget will accept and handle the following softkey-specific events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The softkey widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The softkey widget
                      recognizes those properties defined in the "Properties" section, below.

EVT_WDG_GETPROPERTY:  The softkey widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  The softkey widget
                      recognizes those properties defined in the "Properties" section, below.
===/pre>   

Properties:
Property              Description
--------              ------------------------------------------------------------
PROP_SOFTKEY1:        This property contains a pointer to the widget that will be displayed
                      in softkey one -- i.e. the static text widget on the left side of the
                      softkey widget.  When setting this property, BREW will retrieve and
                      release the existing widget assigned as softkey one, prior to adding
                      the new widget.
                             
                        Property Value:  IWidget **

PROP_SOFTKEY2:        This property contains a pointer to the widget that will be displayed
                      in softkey two -- i.e. the static text widget on the right side of the
                      softkey widget.  When setting this property, BREW will retrieve and
                      release the existing widget assigned as softkey two, prior to adding
                      the new widget.
                             
                        Property Value:  IWidget **

Required Model:
   None 
   
Model Data:
   None
   
Comments:
   None
   
See Also:
   Model Events
   ISHELL_CreateInstance()
   IWIDGET_SetText()
   IWIDGET_SetExtent()


*/
#endif /* __AEESOFTKEYWIDGET_H__ */
