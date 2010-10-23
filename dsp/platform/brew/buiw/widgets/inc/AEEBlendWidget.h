/*
  ========================================================================

  FILE:  AEEBlendWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: BlendWidget blends its child into the background

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEBLENDWIDGET_H__
#define __AEEBLENDWIDGET_H__

#include "bid\AEECLSID_BLENDWIDGET.bid"

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Blend Widget Class IDs

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   Blend widget by creating a new instance of the Blend Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The user interface object that is created
   is, itself, a widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the blend widget is defined below.

Definition:

Identifier             Definition
----------             -----------------------------------
AEECLSID_BLENDWIDGET:  Identifies the class ID of the blend widget
   
Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Blend Widget Class

Description:
   The blend widget is derived from the decorator object and is used as a visual
   filter through which a child widget will be blended into the display.  The effect
   is to make the child widget appear semi-transparent (i.e. "dimmed" or "faded").
   Typically, a user interface will display inactive objects in a distinctly subdued
   manner to lessen their prominence on the display.  For example, an application may
   choose to dim a bitmap image depending on some application state (off-line buddies,
   a dormant network connection, etc).
   
   A blend widget is created by calling ISHELL_CreateInstance() and passing
   AEECLSID_BLENDWIDGET as the class id.  If successful, a pointer to the blend
   widget object will be returned in the 'ppobj' parameter of ISHELL_CreateInstance.

   Applications may control the degree to which the underlying widget will be blended
   by setting the PROP_TRANSPARENCY property of the blend widget.  A PROP_TRANSPARENCY
   value of 0 would produce a fully opaque result.  A value of 1 would add a small
   degree of transparency to the widget, 2 a little more, 3 a little more... up to 255
   which would render the widget completely transparent (i.e. invisible!).  The default
   is to apply a completely transparent blend.
      
   To create a blend widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_BLENDWIDGET to
          retrieve a reference counted instance of the blend widget.
       2. If the blend widget is to be displayed with a degree of transparency, call
          IWIDGET_SetTransparency().
===/pre>
   
   Once a blend widget has been created, it can be used to affect the display of other
   widgets.  Continuing the example above, the newly created blend widget could be
   attached as a decorator to the child widget we wish to display with some level of
   transparency.

Supported Events:
   The blend widget handles events to get or set the properties that will control how
   blend effects should be applied to the widget that it decorates.  Events not identified
   below will be routed to the base decorator widget from which the blend widget is
   derived.

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The blend widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The blend widget
                      allows the following properties to be set:
                      
                          PROP_TRANSPARENCY   --  Sets the transparency level of the blend
                                                  widget.
                                                  
                      These properties are discussed below in greater detail.

                      The blend widget will route the EVT_WDG_SETPROPERTY event to the
                      base decorator widget if 'wParam' identifies a property that does
                      not appear in the list above.

EVT_WDG_GETPROPERTY:  The blend widget responds to this event by attempting to retrieve
                      the property identified by the 'wParam' parameter.  The blend widget
                      allows the following properties to be retrieved:
                      
                          PROP_TRANSPARENCY   --  Gets the transparency level of the blend
                                                  widget.
                                                  
                      These properties are discussed below in greater detail.

                      The blend widget will route the EVT_WDG_GETPROPERTY event to the
                      base decorator widget if 'wParam' identifies a property that does
                      not appear in the list above.
===/pre>   

Properties:
Property             Description
--------             ------------------------------------------------------------
PROP_TRANSPARENCY:   This property controls the degree to which the underlying widget will
                     be blended.  A value of 0 would produce a fully opaque result.  A value
                     of 1 would add a small degree of transparency to the widget, 2 a little
                     more, 3 a little more... up to 255 which would render the widget
                     completely transparent (i.e. invisible!).

Required Model:
   None
   
Comments:
   The blend widget accepts and handles the following events:

===pre>
       EVT_WDG_SETPROPERTY
       EVT_WDG_GETPROPERTY
===/pre>

   The blend widget will only set or get the values for PROP_TRANSPARENCY.  All other
   event and property requests will be routed to the decorator widget from which the
   blend widget has been derived.
   
Side Effects:   
   Setting the PROP_TRANSPARENCY on a blend widget will invalidate the widget's parent
   decorator widget.
   
See Also:
   Model Events
   Border Widget Class
   IWIDGET_SetExtent()
   IWIDGET_SetProperty()
   IWIDGET_SetModel()
   IWIDGET_SetTransparency()
   ISHELL_CreateInstance()
   
   
*/

#endif /* __AEEBLENDWIDGET_H__ */
