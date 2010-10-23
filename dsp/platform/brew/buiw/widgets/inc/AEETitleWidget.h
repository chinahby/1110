/*
  ========================================================================

  FILE:  AEETitleWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I decorate any given widget with a title area

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEETITLEWIDGET_H__
#define __AEETITLEWIDGET_H__

#include "AEEWProperties.h"
#include "AEEStaticWidget.h"

#include "bid\AEECLSID_TITLEWIDGET.bid"


static __inline int IWIDGET_SetTitle(IWidget *piw, AECHAR *pwText, boolean bFree) {
   IWidget *piwStatic = 0;
   int nErr = IWIDGET_GetProperty(piw, PROP_TITLEWIDGET, (uint32*)&piwStatic);
   if (!nErr) {
      IWIDGET_SetText(piwStatic, pwText, bFree);
      IWIDGET_Release(piwStatic);
   }
   return nErr;
}


/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Title Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   title widget by creating a new instance of the Title Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the title widget is defined below.

Definition:

Identifier             Definition
----------             -----------------------------------
AEECLSID_TITLEWIDGET:  Identifies the class ID of the title widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Title Widget Class

Description:
   The title widget is derived from the decorator widget and provides a user interface
   element that is, essentially, a decorator that displays a text label above the widget
   that it wraps.  For example, a title widget that is wrapped around a container could
   be used to create a title element for a display screen, like so:
   
===pre>
     Take me to your leader, monkey boy   <--- Title widget wrapped around the
   +------------------------------------+           random widget below
   |                                    |
   |               _..._                |
   |             .'     '.              |
   |            /'-.---.-'\             |
   |            |/`     `\|             |
   |           ( \  0 0  / )            |
   |            |/   V   \|             | <--- Some random widget that knows how to
   |             \ '-:-' /              |         display ASCII images of mutant
   |            .'`-----'`.             |                  monkey people.
   |          /`           `\           |
   |         /  /         \  \          |
   |        /._/|         |\_.\         |
   |        |  )\._     _./(  |         |
   |        \_)_/_   Y   _\_(_/         |
   |          / ` ) / \ / ` \           |
   |          \   \/   \|   /           |
   |           '._/     \_.'            |
   |                                    |
   |                                    |
   +------------------------------------+
   
===/pre>
   The text displayed by the title widget is, itself, a static widget and responds to
   all of the property and display characteristics common to text objects.  For example,
   the title could be displayed using a specific font, or rendered in a particular color
   or with a certain level of transparency.  Clients that wish to directly manipulate
   the text or text characteristics of a title widget would first need to obtain a pointer
   to the static widget by requesting the PROP_TITLEWIDGET from the title widget, then
   manipulate this widget (or the value model associated with the static widget) directly.
   
   Though the title widget by default will bind itself to a static widget as the object
   displayed as the wrapped widget's decorator, an application may choose to replace the
   static widget with a more complex element such as an image static, a bitmap, or something
   much more elaborate.  For example, an application that displays tabs with images and
   text may choose to carry this theme forward by creating forms containing title widgets
   that are drawn using an image static to match the icons and text appearing on various
   tabs.

   To create a title widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_TITLEWIDGET to
          create a title widget.  ISHELL_CreateInstance() will retrieve a reference
          counted instance of the class that manages the widget.
       2. Make any desired modifications to the title widget's border.  For example,
          setting the extent, defining the border color (if a visible border is
          desired), etc.
       3. Retrieve the PROP_TITLEWIDGET property from the title widget to gain access
          to the static widget used to manage the title's text.
       4. Make any desired modifications to the static widget.  For example, setting
          the font, text color and position of the text.
       5. Create the widget that will be wrapped by the title widget.
       6. Call IDECORATOR_SetWidget() to wrap the title widget around the widget it
          decorates
===/pre>

   The title widget, by default, will be created with the following characteristics:

===pre>
              Text alignment:  Middle Left
                   Text Font:  AEECLSID_FONTSYSBOLD
                Border width:  1 pixel
                Border color:  RGB_BLACK
===/pre>

   The title widget processes events in three passes.  In the first pass the widget will
   attempt to process any property events related to the widget border.  If the event is
   handled during this first pass, the widget will invalidate itself and be redrawn, using
   any updated border property values.  In the second pass, the title widget will attempt to
   handle events that may be directly intended for the title widget itself.  Finally, events
   not handled during either of the first two passes will be passed to the parent decorator.
  
   In addition to the full compliment of border and decorator event, the title widget handles
   the following title-widget-specific events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_GETPROPERTY:  The title widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  These properties are
                      discussed below in greater detail.
                      
                          PROP_TITLEWIDGET  --  Retrieves a pointer to the widget that
                                                manages the title widget text, incrementing 
                                                the reference count of the widget.
                                                
EVT_WDG_SETPROPERTY:  The title widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  These properties are
                      discussed below in greater detail.
                      
                          PROP_TITLEWIDGET  --  Sets a new widget which will manage the 
                                                title widget text. The original widget is 
                                                released.
                                                
===/pre>

Properties:
Property          Description
--------          ------------------------------------------------------------
PROP_TITLEWIDGET  This property contains a pointer to the IWidget used to render the title
                  portion of the title widget.  Though the title widget, by default, is
                  bound to a static widget, an application could override this binding and
                  render the title using a much more complex object.  
                                
                    Property Value:  IWidget *
 
Required Model:
===desc>   
   The title widget does not, itself, rely on a particular model.  However, the widget bound
   within the title widget that is used to render the actual title _does_ rely on a model, as
   dictated by the documentation for that particular widget.  For example, the static widget
   which provides a text title to the title widget will rely on the text model for its data,
   and will expect the data stored in the model to formatted as a TextInfo data structure, as
   detailed in AEEStaticWidget.h.  Likewise, if an image static widget has been bound to the
   title widget, the image static widget will rely on the value model and will expect that
   model to store a pointer to an ImageStaticInfo data structure as described in AEEImageStatic.h.
===/desc>
   
Model Data:
   Depends on the widget used to display the title.  (see AEEStaticWidget.h for the default)
   
Comments:
   None
   
See Also:
   Model Events
   IWidget Interface
   ISHELL_CreateInstance()
   IDECORATOR_SetWidget()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_SetTitle()

Description:
   Sets the title text to be displayed by the title widget. The caller may specify
   whether the memory containing the title text is to be owned and freed by the title 
   widget.
   
Prototype:
   int IWIDGET_SetTitle(IWidget *piWidget, const AECHAR *pwText, boolean bFree);
   
Parameters:
===pre>   
   piWidget:  Pointer to the IWidget object.
   pwText:    Pointer to the text that is to be displayed in the widget.  This pointer
              will be stored in the value model.
   bFree:     Indicates whether or not the value model of the title widget should 
              free the memory of the text that is being set.  When TRUE, the value 
              model will free the memory pointed to by 'pwText' when the model
              is released. When FALSE, the model will leave memory management to 
              the calling application.
===/pre>

Return Value:
===pre>   
   SUCCESS - If the title widget is able to successfully set the text.
   EFAILED - If the text could not be set.
===/pre>   
   
Comments:
   None

Side Effects: 
   Calling IWIDGET_SetTitle() will change the value model and trigger a notify of any
   attached model listeners.

See Also: 
    IWidget


*/

#endif /* __AEETITLEWIDGET_H__ */
