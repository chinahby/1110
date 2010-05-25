/*
  ========================================================================

  FILE:  AEEListForm.h
  
  SERVICES: 

  GENERAL DESCRIPTION: A form with a list in it

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEELISTFORM_H__
#define __AEELISTFORM_H__

#include "AEEForm.h"

#include "bid/AEECLSID_LISTFORM.bid"
#include "bid/AEECLSID_LISTFORM_SCROLL.bid"


/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
List Form Class ID

Description:
   Each object in the forms interface is identified by a unique interface ID.  These
   IDs are used throughout the widget and forms framework to distinguish between the
   various object and interface types, such as forms, widgets and containers.  An
   application creates a list form by creating a new instance of the List Form class,
   passing the ID of the class to ISHELL_CreateInstance().  The object that is created
   is, itself, a form and may utilize those APIs defined for the IForm interface.
   
   The class ID used to identify the list form is defined below.

Definition:

Identifier                 Definition
----------                 -----------------------------------
AEECLSID_LISTFORM:         Identifies the class ID of the list form
AEECLSID_LISTFORM_SCROLL:  Identifies the class ID of the scrollbar list form
   
Comments:
   None

See Also:
   IForm Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

List Form Class

Description:
   The list form object is derived from the base form interface and is intended to provide
   applications with a standard and consistent mechanism for displaying forms that contain
   lists of items.  For example, an application that tracks baseball statistics may wish
   to display screens that contain lists of teams and lists of players, each on a separate
   screen.  The list forms could be used to display the contents of each of these screens.
   
   The list form is a specialized derivation of the base form object.  As such, the list form
   supports each of the common user interface elements found within the context of a form --
   a title area, a content area, and a pair of softkeys that provide interaction with the
   information selected in the list form's content area.  The list form class expands on
   these common UI elements by providing additional widget and form capabilities.  Notably,
   the list form will include a list widget for displaying the list contents, and will also
   provide a selection handler the application may use to manage user interaction with items
   selected in the list.
   
   By default, the list widget is created with a list widget, but _without_ a list item widget.
   The application is responsible for creating and assigning the list item widget (by first
   retrieving the WID_LIST property, then setting the WID_LISTITEM property).  The application
   may also choose to replace the list widget entirely by setting the list form's WID_LIST
   property. 
   
   List forms share all of the usual characteristics of a form (background image, properties,
   etc) and maintain the size and shape of the form's root container.  The list widget
   supplied upon creating the list form has identical capabilities to those described in the
   List Widget class.
   
   The list form is created with the following default characteristics:
   
===pre>
           Size and position:  Identical to the root container
                Border width:  0
         List property flags:  LWF_WRAP  (continuous selection wrap)
===/pre>

   To create a list form, an application would do the following:

===pre>
       1. Call ISHELL_CreateInstance() with a class ID of either:
               - AEECLSID_LISTFORM to create a list form
               - AEECLSID_LISTFORM_SCROLL to create a list form with scrollbar
          ISHELL_CreateInstance() will return a reference counted instance of the desired
          form type.
       2. Set the form's FID_TITLE property to initialize the text to be displayed in the
          title area of the list form
       3. If the form is to support softkeys, set the dialog's FID_SOFTKEY1 and FID_SOFTKEY2
          properties to initialize the softkey text.
       4. Create the list item widget that will be responsible for displaying items within
          the list.
       5. Retrieve the form's list widget, then call IDECORATOR_SetWidget() to wrap the list
          widget around the item widget.
       6. Call IFORM_SetHandler() on the list form to hook an event handler to process user
          actions.
===/pre>

Supported Events:
   The list form processes events in two passes.  In the first pass the list form will
   attempt to process events that get and set various properties of the form or its
   widgets.  Events not handled during this pass are then passed on to the base form
   event handler.
  
   The list form handles the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The list form responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The list form
                      allows the following properties to be set:
                      
                          WID_LIST         --   Sets the list widget to be used to display
                                                the form's list contents.  A pointer to the
                                                list widget is passed with the event in the
                                                'dwParam' parameter.
                          WID_LISTITEM      --  Sets the widget that will be used to
                                                display each item within the form's list.  A
                                                pointer to the item widget is passed with the
                                                event in the 'dwParam' parameter.  Setting
                                                this property will trigger the list to be
                                                redrawn.
                          FID_LISTMODEL     --  Sets the model used by the form's list widget.
                                                A pointer to the list model is passed with the
                                                event in the 'dwParam' parameter.
                          FID_LISTINDEX     --  Sets the list item that should have focus.
                          FID_SELECTHANDLER --  Sets the selection handler to be used by the
                                                list form, to be the handler passed with the
                                                event in the 'dwParam' parameter.
                          FID_THEME         --  Sets the theme file to be used by the list
                                                form and applies this theme to the form.
                      
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The list form responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter, passing the value of
                      the property back to the querying object in a pointer passed in the
                      'dwParam' parameter.  The list form allows the following properties
                      to be retrieved: 
                      
                          WID_LIST       --  Retrieves a pointer to the form's list widget.  
                                             Retrieving this property increments the reference
                                             count of the list widget.
                          WID_LISTITEM   --  Retrieves a pointer to the widget used to
                                             display each item in the list. Retrieving this 
                                             property increments the reference count of the 
                                             list item.
                          WID_DECORATOR  --  Retrieves a pointer to the decorator associated
                                             with the list form.  Normally this is the list
                                             widget.  Retrieving this property increments the
                                             reference count of the decorator.
                          FID_VIEWMODEL  --  Retrieves a pointer to the view model utilized by
                                             the list form's list widget.  Retrieving this 
                                             property increments the reference count of the 
                                             view model.
                          FID_LISTMODEL  --  Retrieves a pointer to the list model utilized by
                                             the list form's list widget.  Retrieving this 
                                             property increments the reference count of the 
                                             list model.
                          FID_LISTINDEX  --  Retrieves the index of the list item that
                                             currently has focus.
                      
                      These properties are discussed below in greater detail.
===/pre>
   
Properties:
Property            Description
--------            ------------------------------------------------------------
WID_LIST:           This property contains a pointer to the form's list widget.  It is important
                    to note that the list widget is provided in _addition_ to the primary widget
                    associated with the base form from which the list form is derived.  

                    If this property is customized (set), the application must make sure it is 
                    equal to or a child of the form's primary widget, WID_FORM.
                             
                        Property Value:  IWidget *

WID_LISTITEM:       This property contains a pointer to the widget the form's list widget will
                    use to display each item within the list widget.  Each item in the list widget
                    is rendered using a single widget, pointed to by this item.  Though a
                    list form's list widget is created when the form is created, the list item
                    widget is not, leaving it up to the application to create the list item widget
                    that is most appropriate for the list contents.  A list form could use any
                    widget of its choosing to render its list, with a pointer to this  widget
                    being stored in the WID_LISTITEM property.  This property is, effectively, the
                    widget wrapped within the list form's WID_LIST property.  The list item widget
                    is managed by the list form itself -- independent of the primary widget
                    managed at the form level by the WID_FORM property.
                             
                        Property Value:  IWidget *

WID_FORM:           This property contains a pointer to the list form's primary widget.  If replaced, 
                    the application must make sure WID_LIST is equal to or a child of this widget.

                        Property Value:  IWidget *

FID_LISTINDEX:      This property contains the zero-based index of the list item that possesses the
                    current focus in a list form's list widget.  When setting this property the
                    index is passed in the 'dwParam' of the event and -- if the list is active --
                    will cause the list to be redrawn with the focused item scrolled into view.

                        Property Value:  int

FID_SELECTHANDLER:  This property contains a pointer to the select handler used by the list form to
                    process items selected from its list.  A pointer to a selection function is
                    stored in the SelectHandler data structure pointed to by this property, which
                    may be set but not retrieved -- i.e. this property is used as a mechanism for
                    registering the list form's selection callback and does not contain data that
                    would be used by a client of the list form.
                             
                        Property Value:  SelectHandler *

FID_THEME:          This property identifies the name of the theme file to be used by the list form.
                    Upon setting the theme file, BREW will load and apply the theme to the form.
                             
                        Property Value:  char *

WID_DECORATOR:      This property contains a pointer to the list form's decorator, which (for the
                    list form) is the list widget.  This property can only be retrieved (not set).

                        Property Value:  IDecorator *

FID_VIEWMODEL:      This property contains a pointer to the view model that is attached to the form's
                    list widget.  The property returns that pointer in an IModel pointer passed with
                    the 'dwParam' parameter of an EVT_WDG_GETPROPERTY event.

                        Property Value:  IModel *

FID_LISTMODEL:      This property contains a pointer to the list model that is attached to the form's
                    list widget.  The list model serves as the base model for the list form.

                        Property Value:  IModel *
                        
Comments:
   None
   
See Also:
   ISHELL_CreateInstance()
   IFORM_SetHandler()
   IDECORATOR_SetWidget()
   Form Events
   Model Events
   List Widget Class

*/
#endif /* __AEELISTFORM_H__ */
