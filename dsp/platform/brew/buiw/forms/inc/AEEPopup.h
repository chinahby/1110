/*
  ========================================================================

  FILE:  AEEPopup.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Standard popup and popupmenu types

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEPOPUP_H__
#define __AEEPOPUP_H__

#include "AEEForm.h"
#include "AEEMenuModel.h" // For MMF_* flags

#include "bid/AEEIID_POPUPMENUFORM.bid"

#include "bid/AEECLSID_POPUPFORM.bid"
#include "bid/AEECLSID_POPUPMENUFORM.bid"
#include "bid/AEECLSID_POPUPMENUFORM_SCROLL.bid"

//
// The PopupMenu's item widget may be replaced using WID_LISTITEM
// (the widget is expected to behave like an ImageStaticWidget, and can 
//  optionally listen to MMF_ flags).  
//
// The static aspect of the item widget may be replaced with WID_STATIC.
// 

typedef struct IPopupMenu  IPopupMenu;
#define INHERIT_IPopupMenu(iname) \
   INHERIT_IForm(iname); \
   int   (*LoadMenuItem)     (iname *, const char *pszResFile, uint16 idText, uint16 idMenuCmd, uint16 flags);\
   int   (*SetMenuItemFlags) (iname *, uint16 idCmd, uint16 flags, boolean bSet)

AEEINTERFACE(IPopupMenu) {
   INHERIT_IPopupMenu(IPopupMenu);
};


#define IPOPUPMENU_AddRef(p)                     AEEGETPVTBL(p,IPopupMenu)->AddRef(p)
#define IPOPUPMENU_Release(p)                    AEEGETPVTBL(p,IPopupMenu)->Release(p)
#define IPOPUPMENU_QueryInterface(p,c,d)         AEEGETPVTBL(p,IPopupMenu)->QueryInterface(p,c,d)
#define IPOPUPMENU_HandleEvent(p,e,w,d)          AEEGETPVTBL(p,IPopupMenu)->HandleEvent(p,e,w,d)
#define IPOPUPMENU_LoadMenuItem(p,psz,tr,c,f)    AEEGETPVTBL(p,IPopupMenu)->LoadMenuItem(p,psz,tr,c,f)
#define IPOPUPMENU_SetMenuItemFlags(p,c,f,b)     AEEGETPVTBL(p,IPopupMenu)->SetMenuItemFlags(p,c,f,b)

static __inline IForm *IPOPUPMENU_TO_IFORM(IPopupMenu *po) {
   return (IForm *)(void*)po;
}

static __inline int IPOPUPMENU_GetMenuModel(IPopupMenu *po, IMenuModel **ppiMenuModel) {
   return IFORM_GetProperty(IPOPUPMENU_TO_IFORM(po), FID_MENUMODEL, (uint32)ppiMenuModel);
}


/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Popup Form Interface and Class IDs

Description:
   Each object in the forms interface is identified by a unique interface ID.
   These IDs are used throughout the forms and widgets framework to distinguish
   between the various object and interface types, such as forms, widgets and
   containers.

   The popup and popup menu support both an interface and the creation of class
   object instances.  The popup menu form interface or a new instance of the popup
   or popup menu class objects may be created by calling ISHELL_CreateInstance()
   and passing in the desired class id of the object.
   
   The interfaces and class IDs used to identify the popup and popup menu forms are
   defined below.

Definition:

Identifier                    Definition
----------                    -----------------------------------
AEEIID_POPUPMENUFORM:         Identifies the popup menu form interface
AEECLSID_POPUPFORM:           Identifies the class ID of the popup form
AEECLSID_POPUPMENUFORM:       Identifies the class ID of the popup menu form
AEECLSID_POPUPMENUFORM_SCROLL Identifies the class ID of the popup menu form with scrollbar
   
Comments:
   None

See Also:
   IForm Interface
   IPopupMenu Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Popup Form Class

Description:
   Popups are specialized forms that provide the ability for an application to display a quick,
   transient screen to the user, either to display a message (such as an error or progress
   message) or provide some level of rapid user interaction (such as a popup menu or a simple
   dialog).  For example, an application that displays an "Options" softkey could choose to
   display a quick popup menu from which the user would select any of the available "options"
   provided by the app.
   
   Popups share all of the usual characteristics of a form (background image, properties, etc)
   and are drawn as small windows that appear to float on top of the display.  They also
   contain a backdrop widget which can be used to control the appearance of the form that is
   beneath the popup.  Alone, the popup class is actually the base object form from which more
   complex forms such as dialogs and popup menus will be derived.  Forms that derive from
   popups have control over how the popup will be sized and positioned.  For example, dialogs
   (which derive from the base popup form) center themselves horizontally and vertically within
   the form's root container.  Conversely, the popup menu, which also derives from the base
   popup form, will choose its size based on the number of items contained in its menu, and
   will position itself just above the softkey area of the root form.

   The base popup form is created with the following default characteristics:
===pre>
           Border width:  0
                 Shadow:  1 pixel
               Backdrop:  none
===/pre>
   
   After a popup is created, these or other characteristics common to popups or forms may be
   overridden.  For example, an object that drives from the popup class could choose to alter
   the background color or transparency to create the effect of the popup truly overlaying the
   display.
   
   To create a popup form, an application would do the following:

===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_POPUPFORM to
          retrieve a reference counted instance of the popup form.
       2. Set any specialized properties for the popup, such as the border, background,
          or backdrop properties.
       3. Create the content to be displayed on the popup
       4. If an application so chooses to do so, it may wrap the popup in a blender or
          fader to provide a transitional visual effect each time the popup appears or
          disappears from the screen.
===/pre>

Supported Events:
   The base popup form processes events in two passes.  In the first pass the popup form
   will attempt to process a small set of property events that the popup form will handle
   in a very specific manner.  Events not handled during this pass are then passed on to
   the base form event handler.
  
   The popup form handles the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The popup form responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The popup form
                      allows the following properties to be set:
                      
                          WID_BACKDROP --  Sets the widget used to render the popup's backdrop
                                           effect.
                          FID_ACTIVE   --  Sets the activation state of the popup form.
                          FID_VISIBLE  --  Sets the visible state of the popup form
                          FID_FADEMS   --  Sets the fadeout time for the backdrop if the popup
                                           has a backdrop
                      
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The popup form responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter, passing the value of
                      the property back to the querying object in a pointer passed in the
                      'dwParam' parameter.  The popup form allows the following properties
                      to be retrieved: 
                      
                          WID_BACKDROP  --  Retrieves a pointer to the widget used by the popup
                                            to render its backdrop effect.  Retrieving this
                                            property increments the reference count of the
                                            backdrop widget.
                          WID_DECORATOR --  Retrieves a pointer to the decorator
                                            associated with the popup form.  Retrieving
                                            this property increments the reference count
                                            of the decorator.
                          FID_FADEMS   --  Gets the fadeout time for the backdrop
===/pre>
   
Properties:
Property        Description
--------        ------------------------------------------------------------
FID_FADEMS:     This property maintains the fadeout time for the backdrop of the popup. If
                WID_BACKDROP is set a fader transition is automatically attached to it, with
                the given fadeout time. The fadeout transition is applied when the popup is
                cancelled. 

                    Property Value:  uint32

FID_ACTIVE:     This property contains the activation state of the popup form -- TRUE when
                the popup is active, FALSE when it is not.  Setting the activation state of
                the popup will trigger the popup form to invalidate itself and be redrawn. 
                             
                    Property Value:  boolean

FID_VISIBLE:    This property contains the visible state of the dialog -- TRUE when the
                popup may be all or partially visible, FALSE when it is not. Setting the 
                visible state of the popup may trigger an invalidation if the root form 
                client region has changed.                
                             
                    Property Value:  boolean

WID_BACKDROP:   This property contains a pointer to the widget used to render the backdrop for
                the popup.  The backdrop provides a visual effect for the popup, which makes
                it seem as though the popup has been placed "above" other content on the
                display.  The WID_BACKDROP property allows applications to access the backdrop
                widget that provides this visual feedback.  It is important to note that popups
                supply a backdrop widget in _addition_ to the primary widget provided by the base
                form and accessed through the WID_FORM property.  The backdrop widget is also
                distinguished from the background widget supplied by the root form and accessed
                by the WID_BACKGROUND property.
                            
                    Property Value:  IWidget *
                                    
WID_DECORATOR:  This property contains a pointer to the popup form's decorator.  The decorator
                allows forms that derive from the popup to manage some specialized visual
                characteristic of their display.  For example, a popup form that supports the
                ability to browse through a list of photographs, displaying a thumbnail of the
                current selection in an image viewer, might choose to return a pointer to the
                image viewer in response to queries to get the WID_DECORATOR property, while the 
                list widget used to browse the names of the images might be returned by the
                WID_LIST property.  The list is just a list, while the image viewer decorates
                that list.  This distinction is important in light of the prior example in which
                a pointer to a list widget is returned as the WID_DECORATOR property.

                    Property Value:  IDecorator *

Comments:
   None
   
See Also:
   ISHELL_CreateInstance()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IPopupMenu Interface

Description:
   The popup menu is derived from the base popup form and is intended to provide applications
   with popup menus that are standard in appearance and consistent in their behavior.  For
   example, an application that supports online ticket scalping might include a screen that
   presents the user with a list of upcoming events (Prince in Los Angeles, the San Diego Padres
   vs the evil San Francisco Giants, NCAA Final Four basketball, Oatmeal Wrestling at Denny's...).
   The application might also provide an "Options" softkey that, when pressed, displays a popup
   menu of commands that could be applied to the selected event -- "Buy Tickets", "Venue Details",
   "Available Seats", etc.
   
   Applications interact with the popup menu by defining an item selection function and
   registering this function with the popup menu by calling IFORM_SetSelectHandler().  The
   selection handler will be called, notifying the application of the menu item that has been
   selected, whenever the user presses the key mapped to AVK_SOFT1 -- usually the left softkey
   button.  Applications that wish to invoke item selection using a different key should
   provide their own key event handler to hook into the popup event handler.  For example,
   an application that uses the right softkey for actions and the left for canceling
   operations would need to define an event handler that swapped virtual keycodes normally
   generated by these keys, then hook this event handler into the popup menu event handler
   so that the popup menu's handler will "see" the left softkey as the right, and the right as
   the left. 
   
   Popup menus are automatically sized to fit the number of items to be displayed in the menu,
   and are placed centered horizontally and anchored near the bottom of the display.

   The base popup menu form is created with the following default characteristics:
   
===pre>
                Border width:  0
                      Shadow:  1 pixel
            Background color:  RGB_WHITE
            Foreground Color:  RGB_BLACK
   Selected Background Color:  Light grey
===/pre>
   
   The popup menu will also be wrapped within a fader widget to provide an effect of the
   popup gradually emerging onto the display, then gradually fading from the display when
   it is removed.
   
   After a popup menu is created, the default characteristics listed above, or any other
   characteristic native to popup menus, popups, forms or decorators may be overridden.  For
   example, the popup menu could choose to display vivid blue text over a hot pink background
   (horrifying though that may be to the naked eye).
   
   To create a popup menu, an application would do the following:

===pre>
       1. Call ISHELL_CreateInstance() with a class ID of either:
               - AEECLSID_POPUPMENUFORM to create a popup menu form
               - AEECLSID_POPUPMENUFORM_SCROLL  to create a popup menu form with scrollbar
          ISHELL_CreateInstance() will return a reference counted instance of the desired
          popup menu form interface.
       2. Call IFORM_SetThemeBaseName() on the popup menu if the your application is
          applying specific theme metrics to your forms.
       3. Call IFORM_SetSelectHandler() to install a selection handler to manage item
          selections from within the popup menu.
       4. Initialize the text that is to appear in the popup menu, which is easily
          accomplished through multiple calls to IPOPUPMENU_LoadMenuItem(), which will
          set the text for each menu item from strings stored in a resource file.
===/pre>

Supported Events:
   The popup menu form processes events in two passes.  In the first pass the popup form
   will attempt to process events itself -- looking for property events handled in specific
   ways by popup menus, specific key events and events intended to cancel the popup menu.
   Events not handled during the first pass are then passed on to the base popup event handler.
  
   The popup menu form handles the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_KEY:              The popup menu form will interpret presses of the AVK_SOFT1 virtual
                      key code as a selection from the popup menu, and will trigger the
                      popup menu's selection handler to be called, with the ID of the menu
                      item that has been selected.  Applications that wish to invoke item
                      selection using a different key should provide their own key event
                      handler to hook into the popup event handler, trapping for the virtual
                      keycode of their own choosing.

EVT_FORM_CANCEL:      The popup menu is being instructed to remove itself from the form stack
                      and will comply by initiating its fade out transition.  Popup menus
                      fade for a period of 200 ms prior to being popped off the form stack.
                      This time period is not currently configurable.

EVT_WDG_SETPROPERTY:  The popup menu form responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The popup menu form
                      allows the following properties to be set:
                      
                           WID_BACKDROP      --  Sets the widget used to render the popup menu's
                                                 backdrop effect.
                           WID_STATIC        --  Sets the widget that will be used to display each
                                                 image static item within the popup menu to be the
                                                 static text widget passed with the event in the
                                                 'dwParam' parameter.
                           WID_LISTITEM      --  Sets the widget that will be used to display each
                                                 item within the popup menu to be the widget passed
                                                 with the event in the 'dwParam' parameter.
                           FID_SELECTHANDLER --  Sets the selection handler to be used by the
                                                 popup menu, to be the handler passed with the event
                                                 in the 'dwParam' parameter.
                           FID_LISTMODEL     --  Sets the model used by the form's decorator to be
                                                 the list model passed with the event in the
                                                 'dwParam' parameter.
                           FID_ARRAYMODEL    --  Sets the model used by the form's decorator to be
                                                 the array model passed with the event in the
                                                 'dwParam' parameter.
                           FID_THEME         --  Sets the theme file to be used by the popup menu,
                                                 and applies this theme to the popup menu's form.
                           FID_ACTIVE        --  Sets the activation state of the popup menu form.
                                                 When the popup menu is being activated its form
                                                 will be sized, positioned and displayed.
                           FID_VISIBLE       --  Sets the visible state of the popup menu form.
                           FID_FADEMS        --  Sets the fadeout time for both the backdrop, if present,
                                                 and the menu itself. 
                                                 
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The popup menu responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter, passing the value of
                      the property back to the querying object in a pointer passed in the
                      'dwParam' parameter.  The popup menu allows the following properties
                      to be retrieved: 
                      
                          WID_BACKDROP   --  Retrieves a pointer to the widget used by the popup menu
                                             to render its backdrop effect.  Retrieving this
                                             property increments the reference count of the
                                             backdrop widget.
                          WID_LIST       --  Retrieves a pointer to the popup's decorator, incrementing 
                                             the reference count of the decorator.
                          WID_LISTITEM   --  Retrieves a pointer to the list item widget used as the display
                                             widget for each item in the popup menu, incrementing 
                                             the reference count of the list item.
                          FID_MENUMODEL  --  Retrieves a pointer to the menu model utilized by the
                                             popup menu's decorator, incrementing the reference count
                                             for the menu model.
                          FID_LISTMODEL  --  Retrieves a pointer to the list model utilized by the
                                             popup menu's decorator, incrementing the reference count 
                                             of the decorator.
                          FID_ARRAYMODEL --  Retrieves a pointer to the array model utilized by the
                                             popup menu's decorator, incrementing the reference count 
                                             of the array model.
                          FID_VIEWMODEL  --  Retrieves a pointer to the view model utilized by the
                                             popup menu's decorator, incrementing the reference count 
                                             of the view model.
                          FID_FADEMS     --  Gets the fadeout time for both the backdrop, if present,
                                             and the menu itself. 
===/pre>

Properties:
Property            Description
--------            ------------------------------------------------------------
WID_BACKDROP:       This property contains a pointer to the widget used to render the backdrop for
                    the popup menu.  The backdrop provides a visual effect for the popup menu, which makes
                    it seem as though the popup menu has been placed "above" other content on the
                    display.  The WID_BACKDROP property allows applications to access the backdrop
                    widget that provides this visual feedback.  It is important to note that popup menus
                    supply a backdrop widget in _addition_ to the primary widget provided by the base
                    form and accessed through the WID_FORM property.  The backdrop widget is also
                    distinguished from the background widget supplied by the root form and accessed
                    by the WID_BACKGROUND property.
                             
                        Property Value:  IWidget *
                                    
WID_STATIC:         This property contains a pointer to the static widget the popup will use to
                    display the text portion of each item within its menu.  Each item in the popup
                    menu is rendered using a single image static widget.  The static component of
                    this widget is stored in the popup menu's WID_STATIC property.  This widget is
                    managed by the popup menu itself -- independent of the primary widget managed
                    at the form level by the WID_FORM property.
                             
                        Property Value:  IWidget *

WID_LIST:           This property contains a pointer to the list widget utilized by the popup to display
                    and manage the items contained in the popup menu.  It is important to note that the
                    list widget is provided in _addition_ to the primary widget associated with the popup's
                    base form and accessed by the WID_FORM property.
                             
                        Property Value:  IWidget *
                                    
WID_LISTITEM:       This property contains a pointer to the decorator the popup will use to display
                    each item within its menu.    Each item in the popup menu is rendered using a
                    single widget, pointed to by this decorator.  Note, however, that this widget is
                    not, necessarily, an image static widget -- as would be believed by considering
                    the WID_STATIC property.  Popup menus, by default, are created utilizing an
                    image static as the widget to render each menu item.  A popup menu could, however,
                    use any widget of its choosing to render the menu items, with a pointer to this
                    widget being stored in the WID_LISTITEM property.  This property is, effectively,
                    the widget wrapped within the popup menu form's WID_LIST property.  The list item
                    widget is managed by the popup menu itself -- independent of the primary widget
                    managed at the form level by the WID_FORM property.
                             
                        Property Value:  IWidget *

FID_SELECTHANDLER:  This property contains a pointer to the select handler used by the popup to
                    process items selected from its menu.  A pointer to a selection function is stored
                    in the SelectHandler data structure pointed to by this property, which may be set
                    but not retrieved -- i.e. this property is used as a mechanism for registering the
                    popup menu's selection callback and does not contain data that would be used by a
                    client of the popup menu.
                             
                        Property Value:  SelectHandler *

FID_LISTMODEL:      This property contains a pointer to the list model that is attached to the list
                    widget that manages the content of the menu displayed in the popup.  The list model
                    serves as the base model for the popup menu.

                        Property Value:  IModel *

FID_ARRAYMODEL:     This property contains a pointer to the array model that is attached to the list
                    widget that manages the content of the menu displayed in the popup.  The array
                    model provides structure to the items to be displayed by the popup menu.

                        Property Value:  IModel *

FID_MENUMODEL:      This property contains a pointer to the menu model that is attached to the popup
                    menu -- typically, the list widget that forms the basis for the menu contents.  The
                    property returns that pointer in an IModel pointer passed in the 'dwParam'
                    parameter passed to the form in the EVT_WDG_GETPROPERTY event.
                    
                    If an application is using this property to add popup menu items 
                    (such as via IMENUMODEL_Add()), the application should also set its own free
                    function, via IMENUMODEL_SetPfnFree().

                    The popup menu does not support attempts to set the FID_MENUMODEL property.              

                        Property Value:  IModel *

FID_VIEWMODEL:      This property contains a pointer to the view model that is attached to the popup
                    menu's list widget.  The property returns that pointer in an IModel pointer passed with
                    the 'dwParam' parameter of an EVT_WDG_GETPROPERTY event.

                        Property Value:  IModel *

FID_THEME:          This property identifies the name of the theme file to be used by the popup menu.
                    Setting FID_THEME on a popup menu will load the theme background image to be used by
                    the popup and will notify listening objects of the theme change -- most notably, the
                    widget responsible for displaying the popup menu itself.  The popup menu will apply
                    the new theme to the widget used to draw each menu item, and cause the menu to be
                    drawn on the display.
                             
                        Property Value:  char *

FID_ROOT:           This property contains a pointer to the root form that manages the popup menu's form.
                    The root form controls the stacking order for a collection of forms and manages where
                    the popup will be placed relative to other forms.
                             
                        Property Value:  IRootForm *

FID_ACTIVE:         This property contains the activation state of the popup menu -- TRUE when the popup
                    is active, FALSE when it is not.  Setting the activation state of the popup will
                    trigger the popup's form to invalidate itself and be redrawn.
                             
                        Property Value:  boolean

FID_VISIBLE:        This property contains the visible state of the dialog -- TRUE when the
                    popup may be all or partially visible, FALSE when it is not. Setting the 
                    visible state of the popup may trigger an invalidation if the root form 
                    client region has changed.

                    See Also: FORMITEM_CLIENT
                             
                        Property Value:  boolean

FID_FADEMS:         This property contains the fadeout time for the popup menu and the backdrop 
                    (if present).
                             
                        Property Value:  uint32
   
Comments:
   None
   
See Also:
   Form Events
   ISHELL_CreateInstance()
   IFORM_SetSelectHandler()
   IFORM_SetThemeBaseName()
   Model Events

===========================================================================

IPOPUPMENU_AddRef()

Description:

   This function increments the reference count of the IPopupMenu interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IPOPUPMENU_AddRef(IPopupMenu *p);

Parameters:
===pre>
   p:  Pointer to the IPopupMenu interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IPopupMenu interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IFORM_Release()
   
=======================================================================

IPOPUPMENU_Release()

Description:

   This function decrements the reference count of the IPopupMenu interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IPOPUPMENU_Release(IPopupMenu *p);

Parameters:
===pre>
   p:  Pointer to the IPopupMenu interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IPopupMenu interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IPOPUPMENU_AddRef()

=======================================================================

IPOPUPMENU_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IPOPUPMENU_QueryInterface(IPopupMenu *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IPopupMenu interface object.
   c:  A globally unique id that identifies the interface to be queried.
   d:  Pointer to a pointer returned by the function to reference the
       interface requested by the 'c' parameter.  If the queried item is not
       available, this pointer is returned as NULL.
===/pre>

Return Value:
===pre>
   SUCCESS          - The requested interface was found and has been returned
                      in the 'd' parameter.
   ECLASSNOTSUPPORT - The requested interface is not supported.
===/pre>

Comments:  
   The pointer passed in the 'd' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper
   level of indirection is used in calling the function.

Side Effects: 
   Upon successful retrieval of an interface pointer, that interface is returned with
   its reference count incremented.  The caller is responsible for releasing this object
   at some point in the future.

See Also:
   IQI_QueryInterface()

=======================================================================

IPOPUPMENU_HandleEvent()

Description:
   This function receives and handles events passed to the popup menu from its owner.

Prototype:
   boolean IPOPUPMENU_HandleEvent(IPopupMenu *p, AEEEvent e, uint16 w, uint32 d);

Parameters:
===pre>   
   p:  A pointer to an IPopupMenu object.
   e:  The event code being sent to the IPopupMenu object.
   w:  16-bit event-specific parameter.
   d:  32-bit event-specific parameter.
===/pre>   
   
Return Value:
===pre>
   TRUE  - If the event was handled by the object.
   FALSE - If the event was not handled.
===/pre>

Comments:  
   None

Side Effects: 
   None
   
See Also:
    IPopupMenu Interface
    Form Events
    Widget Events


=======================================================================

IPOPUPMENU_LoadMenuItem()

Description:
   This function adds a new item to a popup menu, loading the text from a string resource and
   identifying the menu 

Prototype:
   int IPOPUPMENU_LoadMenuItem(IPopupMenu *po, const char *pszResFile, uint16 idResText, uint16 idMenuCmd, uint16 flags);

Parameters:
===pre>   
   po:          Pointer to an IPopupMenu object.
   pszResFile:  Identifies the name of the resource file that contains the string resource to be
                used as the menu item text.
   idResText:   Identifies the resource that contains the string to be used as the menu item text.
   idMenuCmd:   Specifies the command ID to be assigned to the new menu item.
   flags:       Bit flags identifying special characteristics for the new menu item.  The popup menu
                recognizes the following flags:
                    MMF_SUBMENU    -  The menu item is a submenu
                    MMF_SEPARATOR  -  The menu item should be drawn with a menu separator beneath the text
                    MMF_ENABLED    -  This menu item is currently enabled for selection
===/pre>   
   
Return Value:
===pre>   
   SUCCESS            -- The menu item was successfully added to the popup menu
   ENOMEMORY          --  The memory required to accommodate the new menu item could not be allocated.
   ERESOURCENOTFOUND  --  The resource identified by 'idResText' does not exist in the specified
                          resource file.
===/pre>   

Comments:  
   Popup menu also takes responsibility for freeing data associated with the loaded menu items, 
   so it is not recommended to manually manipulate the menu model if using this API.

Side Effects: 
   None
   
See Also:
    IPopupMenu Interface
    IPOPUPMENU_GetMenuModel()

=======================================================================

IPOPUPMENU_SetMenuItemFlags()

Description:
   This function sets the menu item flags for one or all of the menu items in a popup menu.  The
   caller can identify either a specific menu item by passing that item's ID in the 'idCmd'
   parameter, or the caller may indicate all of the menu items by passing MENUITEM_ALL for 'idCmd'.    

Prototype:
   int IPOPUPMENU_SetMenuItemFlags(IPopupMenu *po, uint16 idCmd, uint16 flags, boolean bSet);

Parameters:
===pre>   
   po:      Pointer to an IPopupMenu object.
   idCmd:   Specifies the command ID to of the menu item whose flags are to changed.
   flags:   Bit flags identifying special characteristics for the new menu item.  The popup menu
            recognizes the following flags:
                MMF_SUBMENU    -  The menu item is a submenu
                MMF_SEPARATOR  -  The menu item should be drawn with a menu separator beneath the text
                MMF_ENABLED    -  This menu item is currently enabled for selection
   bSet:    Boolean indicating whether the specified 'flags' should be set (TRUE) or cleared (FALSE).
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The menu item flags were successfully changed.
   EBADPARM  --  The menu command ID passed in 'idCmd' does not exist.
===/pre>   

Comments:  
   None

Side Effects: 
   Changing the menu item flags will cause a ListModelEvent specifying an EVT_MDL_LIST_CHANGE event to
   be sent to any objects listening to the popup menu's menu model.
   
See Also:
    IPopupMenu Interface
    ListModelEvent

=======================================================================

IPOPUPMENU_GetMenuModel()

Description:
   This function will retrieve a pointer to the menu model that is attached to the popup widget,
   returning that pointer in the pointer pointed to by the 'ppiMenuModel' parameter.

Prototype:
   int IPOPUPMENU_GetMenuModel(IPopupMenu *po, IMenuModel **ppiMenuModel);

Parameters:
===pre>   
   po:            Pointer to an IPopupMenu object.
   ppiMenuModel:  Pointer to an IMenuModel pointer, into which the function will return a pointer
                  to the menu model that is attached to the popup menu.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS - If a pointer to the menu model is successfully retrieved.
   EFAILED - If a pointer to the menu model could not be retrieved.
===/pre>   
   
   A pointer to the attached menu model is returned in 'ppiMenuModel'.

Comments:  
   The pointer passed in the 'ppiMenuModel' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level of
   indirection is used in calling the function.

   If an application is using this property to add popup menu items (such as via IMENUMODEL_Add()), 
   the application should also set its own free function for the model, via IMENUMODEL_SetPfnFree().
   By default popup menu frees menu item data upon destruction or item removal.

Side Effects: 
   Calling IPOPUPMENU_GetMenuModel() effectively queries the popup menu form to retrieve the
   FID_MENUMODEL property.
   
See Also:
    IPopupMenu Interface
    IMenuModel Interface

==========================================================================

IPOPUPMENU_TO_IFORM()

Description:
   This function provides type safe casting of an IPopupMenu pointer to an IForm pointer and
   should be used when passing an IPopupMenu pointer to functions that require a pointer to
   an IForm.  This is safer than a simple cast, since the compiler will verify the pointer
   type.

Prototype:
   IForm *IPOPUPMENU_TO_IFORM(IPopupMenu *po)

Parameters:
===pre>
   po:  Pointer to an IPopupMenu interface object.
===/pre>

Return Value:
   Returns 'po' cast to an IForm pointer.

Comments:  
   None

Side Effects: 
   None

See Also:
   IPopupMenu Interface
   IForm Interface


*/



#endif /* __AEEPOPUP_H__ */
