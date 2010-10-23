/*
  ========================================================================

  FILE:  AEECursorWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Implements a "wait" cursor 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEECURSORWIDGET_H__
#define __AEECURSORWIDGET_H__

#include "AEE.h"
#include "AEEWidget.h"
#include "AEEXYContainer.h"
#include "AEEWProperties.h"

#include "bid\AEECLSID_CURSORWIDGET.bid"


typedef struct {
   int x;                       // x position of cursor in container
   int y;                       // x position of cursor in container
   IXYContainer *piContainer;   // container for cursor
} CursorShow;

#define CURSORSHOW_Init(d,x,y,c) \
   (d)->x = (x), (d)->y = (y), (d)->piContainer = (c)

static __inline int IWIDGET_CursorShow(IWidget *po, IXYContainer *piContainer, int x, int y) {
   CursorShow pos;
   CURSORSHOW_Init(&pos, x, y, piContainer);
   return IWIDGET_SetProperty(po, PROP_CURSOR_SHOW, (uint32)&pos);
}

static __inline int IWIDGET_CursorDismiss(IWidget *po) {
   return IWIDGET_SetProperty(po, PROP_CURSOR_DISMISS, 0);
}

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Cursor Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   cursor widget by creating a new instance of the Cursor Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the cursor widget is defined below.

Definition:

Interface ID            Definition
------------            -----------------------------------
AEECLSID_CURSORWIDGET:  Identifies the class ID of the cursor widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

=============================================================================

CursorShow

Description:
   This data type specifies the where the cursor will be displayed, relative to
   an enclosing container.
   
Definition:
   typedef struct {
      int           x;
      int           y;
      IXYContainer  *piContainer;
   } CursorShow;

Members:
===pre>
   x:            The horizontal position of the cursor, relative to the left edge of
                 the container specified in 'piContainer'.
   y:            The vertical position of the cursor, relative to the top edge of
                 the container specified in 'piContainer'.
   piContainer:  Pointer to the container in which the cursor will be displayed.
===/pre>

Comments:
   When showing a cursor, the 'x' and 'y' coordinates of the CursorShow data structure
   specify where the origin of the bitmap representing the cursor will be displayed.

See Also:
   IXYContainer Interface

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Cursor Widget Class

Description:
   The cursor widget is derived from the decorator object and is used to show or hide
   an element on screen (the cursor) typically used in BREW as an overlay to show
   progress (i.e. a spinning beach ball) or a period to wait (i.e. a watch or hour
   glass).
   
   Note, however, that the cursor widget could also be used to track user directed
   movements of a visual pointer on the display.  For example, a user interface could
   be created that uses the bitmap of an arrow to indicate a pointer on screen, with
   the user moving the pointer up and down, left and right by pressing the navigation
   keys on a keypad.

   The bitmap used to represent the default cursor is stored as resource ID 100 in
   the "widgets.mif" file.  The default bitmap could be overridden by retrieving an
   alternate bitmap and assigning it to the cursor widget by calling IWIDGET_SetBitmap().

   To create a cursor widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_CURSORWIDGET to
          retrieve a reference counted instance of the cursor widget.
       2. If a bitmap other than the default cursor is to be displayed, the application
          should call IWIDGET_SetBitmap() on the cursor widget, passing in a pointer to
          the desired graphic.
===/pre>

Supported Events:
   The cursor widget handles events in two passes, first attempting too handle events that
   will set the widget's small set of properties, then -- if the event has not been handled --
   passing it on to the bitmap the widget will display as its cursor.

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The cursor widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The cursor widget
                      allows the following properties to be set:
                      
                          PROP_CURSOR_SHOW    --  Setting this property will display
                                                  the cursor.
                      
                          PROP_CURSOR_DISMISS --  Setting this property will remove the
                                                  cursor widget from its container, and
                                                  releases the container object, effect-
                                                  ively removing the cursor from the display.
===/pre>   

Properties:
Property              Description
--------              ------------------------------------------------------------
PROP_CURSOR_SHOW:     This property contains a pointer to a CursorShow data structure that
                      specifies where this cursor widget is to be displayed.  Setting this
                      property will, in fact, display the cursor widget.
                             
                      Property Value:  CursorShow *

PROP_CURSOR_DISMISS:  This property is used as a command trigger to remove the cursor widget
                      from its container, and release the container object.  There is no value
                      associated with the PROP_CURSOR_DISMISS property.
                             
                      Property Value:  None

Required Model:
   None
      
Comments:
   None

See Also:
   Model Events
   ISHELL_CreateInstance()
   IWIDGET_SetBitmap()

===================================================================================
   MACRO DOCUMENTATION
===================================================================================
CURSORSHOW_Init()

Description:
   This macro initializes each of the fields of a CursorShow data structure.

Prototype:
   CURSORSHOW_Init(CursorShow *d, int x, int y, IXYContainer *c)

Parameters:
===pre>
   d:  Pointer to the CursorShow data structure to be initialized
   x:  The value of the horizontal position where the cursor should be displayed.
   y:  The value of the vertical position where the cursor should be displayed.
   c:  Pointer to the container in which the cursor will be displayed.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   CursorShow
   IXYContainer Interface

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_CursorShow()

Description:
   This function will show a widget cursor at a specific position within a
   specified container.

Prototype:
   int IWIDGET_CursorShow(IWidget *po, IXYContainer *piContainer, int x, int y); 

Parameters:
===pre>
   po:           Pointer to the IWidget interface object.
   piContainer:  Pointer to the container in which the cursor will be displayed.
   x:            The horizontal position (relative to the origin of the enclosing
                 container) where the cursor should be displayed.
   y:            The vertical position (relative to the origin of the enclosing
                 container) where the cursor should be displayed
===/pre>

Return Value:
===pre>
   TRUE  - If BREW was able to successfully show the cursor.
   FALSE - If the cursor could not be displayed.
===/pre>

Comments:
   None

Side Effects: 
   Calling IWIDGET_CursorShow() effectively sets the PROP_CURSOR_SHOW property on the
   widget cursor.

See Also:
   IXYContainer Interface
   PROP_CURSOR_SHOW

==========================================================================

IWIDGET_CursorDismiss()

Description:
   This function removed a cursor widget from its parent container and removes and
   releases the container from the object list.

Prototype:
   int IWIDGET_CursorDismiss(IWidget *po); 

Parameters:
===pre>
   po:  Pointer to the IWidget interface object.
===/pre>

Return Value:
===pre>
   TRUE  - If BREW was able to successfully remove the cursor.
   FALSE - If the cursor could not be removed.
===/pre>

Comments:
   A failure generally indicates that the cursor widget had never been inserted into a
   valid container, or the attempt to remove the container failed.  Note, however, that
   while the attempt to "remove" the container might fail, BREW will still proceed to
   release the container.  

Side Effects: 
   Calling IWIDGET_CursorDismiss() effectively set the PROP_CURSOR_DISMISS property on
   the widget cursor.
   
See Also:
   PROP_CURSOR_DISMISS

*/


#endif //__AEECURSORWIDGET_H__
