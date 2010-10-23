/*
  ========================================================================

  FILE:       AEEWidget.h

  SERVICES:   AEE Widget interfaces (UI Controls)

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEWIDGET_H__
#define __AEEWIDGET_H__

#include "AEE.h"
#include "AEEWModel.h"
#include "AEEDisp.h"
#include "AEEShell.h"
#include "AEEVCodes.h"
#include "AEEHandler.h"
#include "AEECanvas.h"

#include "bid\AEEIID_WIDGET.bid"
#include "bid\AEEIID_CONTROLLER.bid"


/////////////////////////////////////////////////////////////////////////////
// IWidget interface

typedef struct IWidget IWidget;
typedef struct IContainer IContainer;
typedef struct IController IController;

// Widget extent struct
typedef struct {
   int   width;
   int   height;
} WExtent;

#define SETWEXTENT(pwe,w,h) (pwe)->width=(w), (pwe)->height=(h)

// Widget Events
//
// HandleEvent() handles all tasks that would be associated with the
// controller in an MVC framework.
//

#define EVT_WDG_SETFOCUS      0x700    // wparam is focus state (1=focused, 0=not focused)
#define EVT_WDG_HASFOCUS      0x701    // dwParam is ptr to boolean to fill
#define EVT_WDG_CANTAKEFOCUS  0x702    // dwParam is ptr to boolean to fill
#define EVT_WDG_GETPROPERTY   0x800    // wParam PROP_ constant, dwParam is ptr to prop dest
#define EVT_WDG_SETPROPERTY   0x801    // wParam PROP_ constant, dwParam is prop value
#define EVT_WDG_SETLAYOUT     0x802    // wParam is layout flags (0=disable, 1=enable, 2=force)

//
// Trigplayer events (where should these really go?)
//

// TODO tidy up these names & numbers

// Request for next explicit focus widget. wParam holds the direction
// (same as key), dwParam is a pointer to an IWidget
#define EVT_TRIGML_GETNEXTFOCUS 0x7003

// Trigplayer-specific events that are sent by the GridContainer (to itself)

// Trigplayer-specific move focus event. dwParam holds a pointer to an
// IWidget
#define EVT_TRIGPLAYER_MOVEFOCUS 0x7004

// Sent when the data index is changed. dwParam holds the new first index
#define EVT_TRIGPLAYER_DATAINDEXCHANGED 0x700a
/////////////////////////////////////////////////////////////////////////////
// IWidget interface

#define INHERIT_IWidget(iname) \
   INHERIT_IHandler(iname);\
   void     (*GetPreferredExtent)   (iname *, WExtent *pwePreferred);\
   void     (*GetExtent)            (iname *, WExtent *pWExtent);\
   void     (*SetExtent)            (iname *, WExtent *pWExtent);\
   void     (*GetParent)            (iname *, IContainer **ppwc);\
   void     (*SetParent)            (iname *, IContainer *pwc);\
   void     (*Draw)                 (iname *, ICanvas *piCanvas, int x, int y);\
   boolean  (*IntersectOpaque)      (iname *, AEERect *prcDest, const AEERect *prcIn);\
   int      (*GetModel)             (iname *, AEECLSID id, IModel **ppm);\
   int      (*SetModel)             (iname *, IModel *pm)


AEEINTERFACE(IWidget) {
   INHERIT_IWidget(IWidget);
};


#define IWIDGET_AddRef(p)                    AEEGETPVTBL((p),IWidget)->AddRef(p)
#define IWIDGET_Release(p)                   AEEGETPVTBL((p),IWidget)->Release(p)
#define IWIDGET_QueryInterface(p,i,p2)       AEEGETPVTBL((p),IWidget)->QueryInterface(p,i,p2)
#define IWIDGET_HandleEvent(p,e,w,d)         AEEGETPVTBL((p),IWidget)->HandleEvent(p,e,w,d)
#define IWIDGET_SetHandler(p,pd)             AEEGETPVTBL((p),IWidget)->SetHandler(p,pd)
#define IWIDGET_GetPreferredExtent(p,x)      AEEGETPVTBL((p),IWidget)->GetPreferredExtent(p,x)
#define IWIDGET_GetExtent(p,x)               AEEGETPVTBL((p),IWidget)->GetExtent(p,x)
#define IWIDGET_SetExtent(p,x)               AEEGETPVTBL((p),IWidget)->SetExtent(p,x)
#define IWIDGET_GetParent(p,ppc)             AEEGETPVTBL((p),IWidget)->GetParent(p,ppc)
#define IWIDGET_SetParent(p,c)               AEEGETPVTBL((p),IWidget)->SetParent(p,c)
#define IWIDGET_Draw(p,d,x,y)                AEEGETPVTBL((p),IWidget)->Draw(p,d,x,y)
#define IWIDGET_IntersectOpaque(p,rd,ri)     AEEGETPVTBL((p),IWidget)->IntersectOpaque(p,rd,ri)
#define IWIDGET_GetModel(p,i,ppm)            AEEGETPVTBL((p),IWidget)->GetModel(p,i,ppm)
#define IWIDGET_SetModel(p,m)                AEEGETPVTBL((p),IWidget)->SetModel(p,m)


static __inline int IWIDGET_GetProperty(IWidget *p, uint16 propId, uint32 *pVal) {
   return ((0 != IWIDGET_HandleEvent(p, EVT_WDG_GETPROPERTY, propId, (uint32)pVal)) ? SUCCESS : EFAILED);
}

static __inline int IWIDGET_SetProperty(IWidget *p, uint16 propId, uint32 val) {
   return ((0 != IWIDGET_HandleEvent(p, EVT_WDG_SETPROPERTY, propId, val)) ? SUCCESS : EFAILED);
}

static __inline int IWIDGET_GetPropBool(IWidget *po, uint16 wProp, boolean *pbVal)
{
   uint32 dwTemp = 0;
   int nErr = IWIDGET_GetProperty(po, wProp, &dwTemp);
   *pbVal = (boolean)dwTemp;
   return nErr;
}

static __inline int IWIDGET_GetPropInt(IWidget *po, uint16 wProp, int *pnVal) 
{
   uint32 dwTemp = 0;
   int nErr = IWIDGET_GetProperty(po, wProp, &dwTemp);
   *pnVal = (int)dwTemp;
   return nErr;
}

static __inline int IWIDGET_GetPropRGB(IWidget *po, uint16 wProp, RGBVAL *prgb)
{
   uint32 dwTemp = 0;
   int nErr = IWIDGET_GetProperty(po, wProp, &dwTemp);
   *prgb = (RGBVAL)dwTemp;
   return nErr;
}

static __inline int IWIDGET_GetPropPtr(IWidget *po, uint16 wProp, void **pp)
{
   uint32 dwTemp = 0;
   int nErr = IWIDGET_GetProperty(po, wProp, &dwTemp);
   *pp = (void*)dwTemp;
   return nErr;
}

static __inline void IWIDGET_SetPreferredExtent(IWidget *po, WExtent *pe)
{
   WExtent we;

   if (pe)
      IWIDGET_SetExtent(po, pe);
   IWIDGET_GetPreferredExtent(po, &we);
   IWIDGET_SetExtent(po, &we);
}

static __inline void IWIDGET_SetFocus(IWidget* po, boolean bFocus) {
   // SetFocus does not return a meaningful value. Use the functions
   // below to determine if a widget can take focus or has focus
   IWIDGET_HandleEvent(po, EVT_WDG_SETFOCUS, (uint16)bFocus, 0);
}

static __inline int IWIDGET_HasFocus(IWidget* po, boolean *pbFocus) {
   return ((0 != IWIDGET_HandleEvent(po, EVT_WDG_HASFOCUS, 0, (uint32)pbFocus)) ? SUCCESS : EFAILED);
}

static __inline int IWIDGET_CanTakeFocus(IWidget* po, boolean *pbFocus) {
   return ((0 != IWIDGET_HandleEvent(po, EVT_WDG_CANTAKEFOCUS, 0, (uint32)pbFocus)) ? SUCCESS : EFAILED);
}


/////////////////////////////////////////////////////////////////////////////////
// IController - used in widgets that dynamically instantiate their controller, 
// i.e., TextWidget :: TextController


#define INHERIT_IController(iname) \
   INHERIT_IHandler(iname); \
   int      (*SetWidget)   (iname *, IWidget *piWidget)   
   
   

AEEINTERFACE(IController) {
   INHERIT_IController(IController);
};


#define ICONTROLLER_AddRef(p)                AEEGETPVTBL((p),IController)->AddRef(p)
#define ICONTROLLER_Release(p)               AEEGETPVTBL((p),IController)->Release(p)
#define ICONTROLLER_QueryInterface(p,i,p2)   AEEGETPVTBL((p),IController)->QueryInterface(p,i,p2)
#define ICONTROLLER_HandleEvent(p,e,w,d)     AEEGETPVTBL((p),IController)->HandleEvent(p,e,w,d)
#define ICONTROLLER_SetHandler(p,pd)         AEEGETPVTBL((p),IController)->SetHandler(p,pd)
#define ICONTROLLER_SetWidget(p,piw)         AEEGETPVTBL((p),IController)->SetWidget(p,piw)

/* ICONTROLLER_SetWidget returns SUCCESS or 
   EFAILED if the passed widget does not have a valid model 
*/

/////////////////////////////////////////////////////////////////////////////

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Widget Interface IDs

Description:
   Each object in the IWidget interface is identified by a unique interface ID.
   These IDs are used throughout the widget framework to distinguish between the
   various object and interface types, such as containers, decorators and controllers.

   The widget interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as the base interface for all widgets.

   The recognized widget interfaces are defined by the list of interface IDs below.

Definition:

Interface ID        Definition
------------        -----------------------------------
AEEIID_WIDGET:      Identifies the base widget interface
AEEIID_CONTROLLER:  Identifies the controller interface

Comments:
   None

See Also:
   IWidget Interface
   
=============================================================================
Widget Events

Description:
   The widget framework defines a small set of BREW events used to notify objects of
   various state or property changes involving a widget object.  These events are
   sent to objects in addition to the normal set of BREW interface events such as
   key press events and other UI related events.  A widget's event handler should be
   prepared to handle these events in whatever manner is appropriate for the behavior
   of that object.  For example, the EVT_WDG_GETPROPERTY event will be sent to an
   widget to retrieve attributes or characteristics that apply to that particular
   object.  To illustrate, a widget that contains a display banner of some sort
   would likely respond to an EVT_WDG_GETPROPERTY event specifying WID_TITLE as the
   requested property type, by returning the text of the widget's display banner.

   The table below lists the AEE event codes supported by the widget framework,
   identifying the data structure that will be sent with the notification.

Definition:
AEE Event Code                Description
--------------                ------------------------------------------------------
EVT_WDG_SETFOCUS:             The focus for this widget has changed or has been cleared.
                              The 'wParam' parameter of the event contains a boolean value
                              indicating whether or not the new focus item should become
                              active.  The widget receiving this event should return TRUE
                              if it accepts the focus, FALSE if it does not, and may choose
                              to invalidate itself or respond to the focus change in some
                              other meaningful way.  

EVT_WDG_CANTAKEFOCUS:         The widget is being asked if it can take focus. The 'dwParam'
                              should be a pointer to a boolean in memory, which will be
                              set to either TRUE (if the widget can take focus) or FALSE
                              (if the widget is unable to take focus).  The helper function,
                              IWIDGET_CanTakeFocus(), is also available.
                              
EVT_WDG_HASFOCUS:             The widget is being asked if it currently has focus.  The
                              'dwParam' should be a pointer to a boolean in memory, which will
                              be set to either TRUE (if the widget currently has focus) or
                              FALSE (if the widget does not currently have focus).  The
                              helper function, IWIDGET_HasFocus(), is also available.

EVT_WDG_GETPROPERTY:          The widget is being asked to return a particular property, as
                              specified in the 'wParam' parameter, returning the value of the
                              property in the buffer pointed to by the 'dwParam' parameter.
                              Widget properties are defined in AEEWProperties.h and include
                              attributes such as color, border width, shadowing, etc.  The
                              widget receiving this event should return TRUE if it is able to
                              retrieve the requested property, FALSE if no such property exists
                              for the widget.
                              
EVT_WDG_SETPROPERTY:          The widget is being asked to set the value for a particular
                              property, as specified in the 'wParam' parameter.  The 'dwParam'
                              parameter of the event will contain the property's new value.
                              Widget properties are defined in AEEWProperties.h and include
                              attributes such as color, border width, shadowing, etc.  The
                              widget receiving this event should return TRUE if it is able to
                              successfully set the specified property, FALSE if no such property
                              exists for the widget.

Members:
   None
   
Comments:
   None

See Also:
   Border Widget Class
   
=============================================================================

WExtent

Description:
   This data type specifies the geometric structure of a widget, in terms of height
   and width, without defining the bounds or placement of the widget.  The WExtent
   data structure, in a sense, defines the size of a widget.

Definition:
   typedef struct WExtent {
      int   width;
      int   height;
   } WExtent;

Members:
   width:   The width of the widget's bounding rectangle.
   height:  The height of the widget's bounding rectangle.

Comments:
   The WExtent of a widget can be derived from a widget's bounding rectangle with
   the 'width' being equal to the right coordinate of the bounding rectangle minus
   the left, and the 'height' being the bottom coordinate minus the top.

See Also:
   None


===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IWidget Interface

Description:
   IWidget is the interface for describing user interface elements of a wide variety of
   types and capabilities including containers, forms, controls such as checkboxes and
   radio button, static and editable text, decorators and many other UI objects.  The
   IWidget interface provides many common controls, and can used as a base class for new
   custom widgets and UI objects.  Widgets, in general, possess characteristics of both
   display and behavior.  For example, a checkbox maintains a visual appearance on
   the device display depending on its current state, and also acts in a particular way
   when the user checks or unchecks the control.  Other widgets may not invite operator
   interaction and possess only display characteristics.  For example, a decorator
   widget that adds a shadow effect to a popup menu, will support interfaces to draw
   itself on the display, but will not expect to receive operator events such as key
   presses.
      
   The following APIs are provided to support an object reference counting mechanism for
   objects to manage their own memory instances.
===pre>
       IWIDGET_AddRef()
       IWIDGET_Release()
===/pre>
      
   The following APIs are provided to support event handling to the object:
===pre>
       IWIDGET_HandleEvent()
       IWIDGET_SetHandler()
===/pre>

   The following APIs are provided to retrieve or set a widget's extent (the relative
   width and height of a widget's bounding rectangle).
===pre>
       IWIDGET_GetPreferredExtent()
       IWIDGET_SetPreferredExtent()
       IWIDGET_GetExtent()
       IWIDGET_SetExtent()
===/pre>

   The following APIs are provided to retrieve or set a widget's container object.
===pre>
       IWIDGET_GetParent()
       IWIDGET_SetParent()
===/pre>

   The following APIs are provided to manage the drawing of a widget's contents.
===pre>
       IWIDGET_Draw()
       IWIDGET_IntersectOpaque()
===/pre>

   The following APIs are provided to retrieve or set the model that manages the value of
   the data represented by a widget.
===pre>
       IWIDGET_GetModel()
       IWIDGET_SetModel()
===/pre>

Required header files:
===pre>
   AEE.h
   AEEModel.h
   AEEDisp.h
   AEEShell.h
   AEEVCodes.h
===/pre>

===========================================================================

IWIDGET_HandleEvent()

Description:
   This function receives and handles events passed to the widget

Prototype:
   boolean IWIDGET_HandleEvent(IWidget *p, AEEEvent evt, uint16 wParam, uint32 dwParam);

Parameters:
===pre>   
   p:        A pointer to an IWidget object.
   evt:      The event code being sent to the IWidget object.
   wParam:   16-bit event-specific parameter.
   dwParam:  32-bit event-specific parameter.
===/pre>   
   
Return Value:
===pre>
   TRUE, if the event was handled by the object.
   FALSE, if the event was not handled.
===/pre>

Comments:
   None

Side Effects: 
   None
   
See Also:
    Widget Events
    IWidget Interface

===========================================================================

IWIDGET_SetHandler()

Description:
   This function allows a widget to hook its own event handler into the normal widget
   event handling mechanism.  For example, a widget that derives from one of the standard
   widget interfaces could insert its own event handler that will be called by the base
   widget object each time an event is processed.
   
   The event handler is stored in a HandlerDesc data structure as a PFNHANDLER, along with
   a pointer to a block of memory allocate and owned by the widget.  This pointer will be
   passed into the widget's event handler with each event to be processed.  The  HandlerDesc
   data structure also contains a PFNFREEHANDLER that will be called when the widget is
   released so that the widget may free the private memory allocated and passed to the event
   handler.
   
   IWIDGET_SetHandler() associates a given HandlerDesc data structure with a particular
   widget.

Prototype:
   void IWIDGET_SetHandler(IWidget *p, HandlerDesc *phd);

Parameters:
===pre>   
   p:    A pointer to an IWidget object.
   phd:  Pointer to the data structure that contains the event handler, private memory
         pointer, and the callback used to free the private memory.
===/pre>   
   
Return Value:
   None

Comments:
   Passing NULL for the HandlerDesc will reset the widget's event handler to use the base
   widget's default event handler -- without private memory or the need to free privately
   allocated storage.
   
   The handler installed by a widget to process events should itself remember to call
   HANDLERDESC_Call() so that widgets that themselves derive from this widget may also
   hook into the widget's event handler.

Side Effects: 
   None
   
See Also:
   IWidget Interface
   PFNHANDLER
   PFNFREEHANDLER
   HandlerDesc
   
===========================================================================

IWIDGET_GetPreferredExtent()

Description:
   Returns the preferred extent of the widget.  The preferred extent is the size
   that the widget would choose to be under ideal circumstances.  For example, a
   widget that displays a bit mapped image may have a preferred size of 128 x 128
   pixels to best display the detail of each image, and would return a preferred
   extent with 'width' and 'height' both equal to 128.  However, the widget may be
   constrained by any number of factors (the size of the device display, the size
   of the container object, etc) and may need to render the widget in a much
   smaller region.

Prototype:
   void IWIDGET_GetPreferredExtent(IWidget *p, WExtent *pwePreferred);
   
Parameters:
===pre>   
   p:             Pointer to the IWidget object.
   pwePreferred:  Pointer to the WExtent struct that, on output, will contain the
                  preferred extent of the widget.
===/pre> 

Return Value:
   The preferred extent for the widget is returned in 'pwePreferred'.
   
Comments:
   Controls with natural increments in sizes (for example, text controls) should
   return their size in 'pwePreferred' to be the largest natural size that fits the
   available width and height for the control's contents.  To illustrate, a bitmap
   control that contains an image of 640 x 480 pixels, though is displayed in a
   much smaller viewer, should return a preferred extent of the full natural size
   of the image to be displayed.

Side Effects: 
   None

See Also:
    WExtent
    IWidget Interface
    IWIDGET_SetPreferredExtent()
    IWIDGET_GetExtent()
    IWIDGET_SetExtent()
        
===========================================================================

IWIDGET_SetPreferredExtent()

Description:
   This is an inline function that gets the preferred extent of a widget,
   and then sets the extent of the widget to that preferred extent.
   It takes an optional extent that gives a basis for the widget in order
   to calculate the preferred extent.

Prototype:
   void IWIDGET_SetPreferredExtent(IWidget *p, WExtent *pe);
   
Parameters:
===pre>   
   p:   Pointer to the IWidget object.
   pe:  Optional pointer to an extent used to first set the widget extent
        before calculating the preferred extent.
===/pre> 

Return Value:
   None
   
Comments:
   None

Side Effects: 
   None

See Also:
    IWidget Interface
    IWIDGET_GetPreferredExtent()
    IWIDGET_GetExtent()
    IWIDGET_SetExtent()
        
===========================================================================

IWIDGET_GetExtent()

Description:
   Returns the horizontal and vertical extent of the widget.  The extent is better
   thought of as the relative size of an object's bounding rectangle, independent
   of where that widget may be placed on the display.  The extent is returned in
   a WExtent data structure defining the width and height of the widget.
   
Prototype:
   void IWIDGET_GetExtent(IWidget *p, WExtent *pWExtent);
   
Parameters:
===pre>   
   p:         Pointer to the IWidget object.
   pWExtent:  Pointer to the WExtent struct that, on output, will contain the
              current extent of the widget.
===/pre>   

Return Value:
   none
   
Comments:
   None

Side Effects: 
   None

See Also:
    WExtent
    IWidget Interface
    IWIDGET_GetPreferredExtent()
    IWIDGET_SetPreferredExtent()
    IWIDGET_SetExtent()

         
===========================================================================

IWIDGET_SetExtent()

Description:
   Sets the horizontal and vertical extent of the widget, independent of the widget's
   preferred size.  IWIDGET_SetExtent() communicates to the widget the size it has
   been allocated on the display, and provides an opportunity for the widget to adjust
   its content as best suited for the allotted size.  For example, a checkbox widget
   might choose to truncate, reformat or realign its text label when the extent has
   shrunk.  The new size MUST be honored by the widget, as the parent object will
   clip any subsequent attempt to draw "outside the lines".
   
Prototype:
   void IWIDGET_SetExtent(IWidget *p, WExtent *pWExtent);
   
Parameters:
===pre>   
   p:         Pointer to the IWidget object.
   pWExtent:  Pointer to the WExtent struct that provides the new extent of the widget.
===/pre>

Return Value:
   None
   
Comments:
   The extent of a widget often changes as the size of the owning object changes.
   For example, as the size of a container object changes, the container may choose
   to resize each of the enclosed objects to best manage the available real estate
   on the display.  The container would call IWIDGET_SetExtent() for any widget that
   needs to be resized.

   Subsequent call to IWIDGET_GetExtent() should return the same extent value sent
   in the call to IWIDGET_SetExtent().

Side Effects: 
   None

See Also: 
    WExtent
    IWidget Interface
    IWIDGET_GetPreferredExtent()
    IWIDGET_SetPreferredExtent()
    IWIDGET_GetExtent()

===========================================================================

IWIDGET_GetParent()

Description:
   This function retrieves a pointer to the container object that owns a widget.
   Objects within the widget framework maintain a hierarchy of parent/child
   relationships, wherein a container may enclose and manage one or more child
   objects.  The children of a parent container may be widgets, or additional
   containers, with each child receiving events and being managed by its parent.
   
   To illustrate, consider the following hierarchy of objects in which a display
   will contain a checkbox, a text entry field and three radio buttons:

===pre>   
       + Object #1 (checkbox widget)
       + Object #2 (container)
            |
            |
            |---+ Object #3 (text widget)
            |---+ Object #4 (container)
                     |
                     |
                     |---+ Object #5 (radio button widget)
                     |---+ Object #6 (radio button widget)
                     |---+ Object #7 (radio button widget)
===/pre>

   The checkbox is at the top level of the object hierarchy along with a container
   that acts as the parent to a pair of additional objects, a text widget as well
   as a second container, object 4.  Object 4 is in turn the parent of a group
   of three radio button widgets.  Events sent to object 2 could be dispatched to
   each of the child objects 3 and 4, and -- since object 4 is another container --
   to the three radio button widgets at the bottom of the hierarchy.
   
   In the above example, IWIDGET_GetParent() on objects 5, 6 and 7 would return a
   pointer to object 4.  Called for objects 3 or 4, IWIDGET_GetParent() would return
   object 2.  Both object 1 and object 2, being at the top of the hierarchy and
   without any container serving as a parent, would yield NULL when passed into
   IWIDGET_GetParent().
   
Prototype:
   void IWIDGET_GetParent(IWidget *p, IContainer **ppwc);
   
Parameters:
===pre>
   p:     Pointer to the IWidget object.
   ppwc:  Pointer to an IContainer pointer, into which IWIDGET_GetParent() will return
          a pointer to the container that serves as the widget's parent.
===/pre>
   
Return Value:
   A pointer to the widget's parent container is returned in 'ppwc', or NULL if the
   widget does not have a parent container.
   
Comments:
   None

Side Effects: 
   IWidget
   IContainer
   IWIDGET_SetParent()

===========================================================================

IWIDGET_SetParent()

Description:
   This function allows a container to identify itself as the parent of a widget,
   or remove the widget as one of its children.  Objects within the widget framework
   maintain a hierarchy of parent/child relationships, wherein a container may
   enclose and manage one or more child objects.  The children of a parent container
   may be widgets, or additional containers, with each child receiving events and
   being managed by its parent.
   
   To illustrate, consider the following hierarchy of objects in which a display
   contains a pair of checkboxes,

===pre>   
       - Object #1 (checkbox widget)
       - Object #2 (checkbox widget)
 ===/pre>

   Both checkboxes are at the top level of the object hierarchy, neither of which are
   managed by a container object.
   
   To this hierarchy an application wishes to add three radio buttons, and could do
   so by creating three radio button widgets, each enclosed by a container object.

===pre>   
       - Object #1 (checkbox widget)
       - Object #2 (checkbox widget)
       - Object #3 (container)
            |
            |
            |---- Object #4 (radio button widget)
            |---- Object #5 (radio button widget)
            |---- Object #6 (radio button widget)
===/pre>   

   To "attach" the radio buttons to the existing hierarchy, object 3 -- the container
   object -- would call IWIDGET_SetParent() for each of the three radio buttons,
   specifying itself (the container) as the parent in 'pwc'. 
   
   To remove an object from the hierarchy, a container would call IWIDGET_SetParent()
   on one of its children, passing NULL in 'pwc' to remove itself as the object's
   parent.

   Widgets must not refcount their parents.  It is the parent's responsibility
   to set the parent pointer to NULL if the parent is deleted.
   
Prototype:
   void IWIDGET_SetParent(IWidget *p, IContainer *pwc);
   
Parameters:
===pre>
   p:     Pointer to the IWidget object.
   ppwc:  Pointer to the IContainer object that will act as the widget's parent, or
          NULL if the widget is being orphaned.
===/pre>
         
Return Value:
   none
   
Comments:
   Widgets must not refcount their parents.  It is the parent's responsibility
   to set the parent pointer to NULL if the parent is deleted.
   
   Removing a parent/child relationship by passing NULL as a widget's parent in the
   'pwc' field does NOT move the object up one level in the object hierarchy.  In
   other words, if an object is the grandchild of a container and the object's
   parent is removed, the object does NOT become a child of the grandparent.
   
   IWIDGET_SetParent() should only be called by a container that is managing the
   widget.
   
   Widgets must not refcount their parents.  It is the parent's responsibility
   to set the parent pointer of all child objects to NULL prior to the parent
   being deleted.

Side Effects: 
   None

See Also:
   IWidget Interface
   IContainer Interface
   IWIDGET_GetParent()

===========================================================================
   
IWIDGET_Draw()

Description:
   This function draws a widget at a specific position on the device display.
   The widget will draw itself within the bounds defined by its current extent,
   clipped to the clipRect defined in ICanvas.  If the widget does any drawing
   through interfaces other than ICanvas, it is the widget's responsibility to
   ensure that these operations are clipped appropriately (i.e. by calling
   ICANVAS_GetClipRect() and drawing only within the bounds of the rectangle
   returned from ICanvas).
   
   The function specifies the horizontal and vertical position of the upper left
   hand corner of the bounding rectangle that will contain the widget.
   
Prototype:   
   void IWIDGET_Draw(IWidget *p, ICanvas *piCanvas, int x, int y);
                        
Parameters:
===pre>
   p:          Pointer to the IWidget object.
   piCanvas :  Pointer to the ICanvas interface that will provide drawing services
               to the widget.
   x:          Horizontal position of the left side of the widget.
   y:          vertical position of the top of the widget.
===/pre>
      
Return Value:
   none
   
Comments:
   None

Side Effects: 
   None

See Also:
   IWidget Interface
   ICanvas Interface

===========================================================================

IWIDGET_IntersectOpaque()

Description:
   This function retrieves the intersection of the given rectangle with the largest
   opaque region of a given widget.  To illustrate, a widget that defines the plane
   upon which other widgets will be rendered (akin to a window on a desktop GUI)
   would likely provide an opaque background (i.e. when in the foreground, you would
   not be able to see interface objects laying behind).  Another object might want
   to calculate to what degree this widget would be obscuring the object's display,
   and so would pass its bounding rectangle and a pointer to the target widget to
   IWIDGET_IntersectOpaque().  The widget will then determine its largest opaque
   region, taking into account any non-opaque portions (shadows, blends, transparent
   decorators, etc), then -- if the rectangles intersect -- calculate the intersection
   of the source rectangle and the target's opaque rectangle. 
===pre>   
   +--------------------+
   |                    |
   |       source     -----------+ result of IWIDGET_IntersectOpaque()
   |     rectangle   |  |
   |                 V  |
   |            +---------------------------------------+
   |            |/ / / /|                               |
   |            | / / / |       target widget           |
   |            |/ / / /|                               |
   +------------|-------                                |
                |                                       |
                +---------------------------------------+
===/pre>  
   
   If the rectangle's intersect, IWIDGET_IntersectOpaque() will return TRUE, otherwise
   the function returns FALSE and the resultant rectangle is empty.
   
Prototype:
   boolean IWIDGET_IntersectOpaque(IWidget *p, AEERect *prcOut, const AEERect *prcIn);
   
Parameters:
===pre>
   p:       Pointer to the IWidget object to be used as a target for intersection.
   prcOut:  Pointer to an AEERect, into which the function will return the intersection
            of the widget's largest opaque rectangle and the rectangle passed to the
            function in 'prcIn'.
   prcIn:   Pointer to the AEERect that is to be intersected with the widget's largest
            opaque rectangle.  This rectangle should be expressed in the target widget's
            coordinate system, with 0,0 being the top-left corner of the widget.
===/pre>
      
Return Value:
===pre>
   TRUE   -  If 'prcIn' intersects with the widget's largest opaque rectangle.
   FALSE  -  If 'prcIn' does not intersect.
===/pre>
   
   The intersection of 'prcIn' and the widget's largest opaque rectangle will be returned
   in 'prcOut'.
   
Comments:
   By definition, the rectangle returned in 'prcOut' can be no larger than the rectangle
   passed to the function in 'prcIn'.

Side Effects: 
   None

See Also:
   IWidget Interface
   
===========================================================================

IWIDGET_GetModel()

Description:
  This function retrieves the model that provides data to a given widget.
  The model that is returned will implement the interface for the given
  class id.  To illustrate, a widget that displays television signals might
  get its data from an HDTV model, and as such could retrieve a pointer to
  the appropriate model interface by calling IWIDGET_GetModel(), passing in
  a pointer to the "television widget", and the class ID of the HDTV model
  interface.  The function would then return a reference to the appropriate
  model interface used to source the television widget.
   
Prototype:
   int IWIDGET_GetModel(IWidget *p, AEECLSID id, IModel **ppm);
   
Parameters:
===pre>
   p:    Pointer to the IWidget object.
   id:   The class id of the interface model the widget will use as a source
         for data.
   ppm:  Pointer to an IModel pointer into which the function will return a
         pointer to the IModel interface that supports the passed class id.
===/pre>
      
Return Value:
===pre>
   SUCCESS - The function was able to successfully locate a model interface for
             the indicated 'id'.
   EFAILED - The requested model interface is not currently declared.
===/pre>

   A pointer to the model interface associated with the widget is returned in
   'ppm', or NULL if the widget does utilize a model interface.
   
Comments:
   None

Side Effects: 
   None

See Also:
   IWidget Interface
   IModel Interface

===========================================================================

IWIDGET_SetModel()

Description:
   This function allows a widget to associate itself with a specific model
   interface.  For example, a widget that wishes to mimic the behavior of an
   FM radio tuner might wish to utilize source data from a model that provides
   the frequency of the currently tuned station, so that it can display the
   frequency and call letters ("...91.3 KCPR...").  The widget would first
   retrieve an IModel pointer for the desired interface class ID, then
   associate this model with a widget by pass the model pointer and widget
   pointer to IWIDGET_SetModel().
   
   Once the widget has associated itself with a given model interface, it should
   register as a listener for that model.  As a listener, the widget will receive
   notifications each time the model's data changes, thereby providing the
   opportunity for the widget to update its display or act in some other meaning-
   ful way to reflect the current data.

   All provided widgets will send out an EVT_MDL_SETMODEL event
   through their ViewModel when this function is called.
   
Prototype:
   int IWIDGET_SetModel(IWidget *p, IModel *pm);
   
Parameters:
===pre>
   p:   Pointer to the IWidget object.
   pm:  Pointer to the model interface that the widget should use as the basis
        for accessing its data.
===/pre>
      
Return Value:
===pre>
   SUCCESS          - The function was able to successfully associate the widget
                      with the specified model interface.
   ECLASSNOTSUPPORT - The model interface passed to the function does not support
                      a model interface class ID that is known to the widget.
                      For example, the button widget expects to be used with the
                      value model, and will verify that the model pointer in 'pm'
                      supports the value model class ID.
===/pre>
   
Comments:
   None

Side Effects: 
   A EVT_MDL_SETMODEL event is sent through the ViewModel of the widget to indicate
   the change of model

See Also:
   IWidget Interface
   IModel Interface
   IWIDGET_GetModel()
   IMODEL_AddListener()

===========================================================================

IWIDGET_GetProperty()

Description:
   This function retrieves a specific property value for the given widget. 
   Widgets may possess any number of properties, as defined in AEEWProperties.h,
   and include attributes such as color, border width, shadowing, etc.  The
   property value is returned in a pointer passed into the function, with the
   function returning SUCCESS or EFAILED depending on whether or not the requested
   property was able to be retrieved from the widget.


Prototype:
   int IWIDGET_GetProperty(IWidget *p, uint16 propId, uint32 *pVal); 

Parameters:
===pre>
   p:       Pointer to the IWidget interface object.
   propId:  Identifies the property to be retrieved.
   pVal:    A pointer to a 32-bit unsigned integer into which the widget will copy the
            value of the requested property.
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>
   
   The value of the requested property is returned in 'pVal'.

Comments:
   Calling IWIDGET_GetProperty() has the same effect as processing an EVT_WDG_GETPROPERTY
   event.

Side Effects: 
   None

See Also:
   Widget Events
   IWidget Interface
   IWIDGET_SetProperty()

==========================================================================

IWIDGET_SetProperty()

Description:
   This function sets the value of a specific property for the given widget.
   Widgets may possess any number of properties, as defined in AEEWProperties.h,
   and include attributes such as color, border width, shadowing, etc.

Prototype:
   int IWIDGET_SetProperty(IWidget *p, uint16 propId, uint32 val); 

Parameters:
===pre>
   p:       Pointer to the IWidget interface object.
   propId:  Identifies the property whose value is to be set.
   val:     The new value for the widget's 'propID' property, expressed as a 32-bit
            unsigned integer.
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to set the requested property.
   EFAILED - If the property could not be set.
===/pre>

Comments:
   Calling IWIDGET_SetProperty() has the same effect as processing an EVT_WDG_SETPROPERTY
   event.

Side Effects: 
   None

See Also:
   IWidget
   IWIDGET_GetProperty()

==========================================================================

IWIDGET_GetPropBool()

Description:
   This function is used to retrieve widget properties that are stored as boolean values,
   copying the value into the 'pbVal' pointer passed to the function as a parameter. The
   function will return SUCCESS or EFAILED depending on whether or not the widget was able
   to retrieve the requested property.

Prototype:
   int IWIDGET_GetPropBool(IWidget *po, uint16 wProp, boolean *pbVal); 

Parameters:
===pre>
   p:      Pointer to the IWidget interface object.
   wProp:  Identifies the property to be retrieved.
   pbVal:  A pointer to a boolean into which the widget will copy the value of the
           requested property.
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>

   The value of the requested boolean property is returned in 'pbVal'.

Comments:
   Widgets may possess any number of properties, as defined in AEEWProperties.h, and include
   attributes such as color, border width, shadowing, etc.  PROP_SELECTED is an example of a
   widget property expressed as a boolean value.  

Side Effects: 
   IWIDGET_GetPropBool() calls IWIDGET_GetProperty() and has the net effect of 
   processing an EVT_WDG_GETPROPERTY event.

See Also:
   Widget Events
   IWidget Interface
   IWIDGET_GetProperty()

==========================================================================

IWIDGET_GetPropInt()

Description:
   This function is used to retrieve widget properties that are stored as integer values,
   copying the value into the 'pnVal' pointer passed to the function as a parameter. The
   function will return SUCCESS or EFAILED depending on whether or not the widget was able
   to retrieve the requested property.

Prototype:
   int IWIDGET_GetPropInt(IWidget *po, uint16 wProp, int *pnVal); 

Parameters:
===pre>
   p:      Pointer to the IWidget interface object.
   wProp:  Identifies the property to be retrieved.
   pnVal:  A pointer to an int into which the widget will copy the value of the requested
           property.
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>
   
   The value of the requested integer property is returned in 'pnVal'.

Comments:
   Widgets may possess any number of properties, as defined in AEEWProperties.h, and include
   attributes such as color, border width, shadowing, etc.  Examples of widget properties
   stored as an int include PROP_ACTIVE_BORDERWIDTH, PROP_BORDERSTYLE and many others.

Side Effects: 
   IWIDGET_GetPropInt() calls IWIDGET_GetProperty() and has the net effect of 
   processing an EVT_WDG_GETPROPERTY event.

See Also:
   Widget Events
   IWidget Interface
   IWIDGET_GetProperty()

==========================================================================

IWIDGET_GetPropRGB()

Description:
   This function is used to retrieve widget properties that are stored as RGB values,
   copying the value into the 'prgb' pointer passed to the function as a parameter. The
   function will return SUCCESS or EFAILED depending on whether or not the widget was able
   to retrieve the requested property.

Prototype:
   int IWIDGET_GetPropRGB(IWidget *po, uint16 wProp, RGBVAL *prgb); 

Parameters:
===pre>
   p:      Pointer to the IWidget interface object.
   wProp:  Identifies the property to be retrieved.
   prgb:   A pointer to a 32-bit RGBVAL into which the widget will copy the value of the
           requested property.
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>
   
   The value of the requested RGB property is returned in 'prgb'.

Comments:
   Widgets may possess any number of properties, as defined in AEEWProperties.h, and include
   attributes such as color, border width, shadowing, etc.  Examples of widget properties
   stored as an RGBVAL include PROP_ACTIVE_BORDERCOLOR, PROP_SELECTED_BORDERCOLOR and many
   others.

   RGB values may contain standard red, green and blue components, or may include an alpha
   component in addition to the standard color values.  See the definition of RGBVAL in
   AEEBipmap.h for additional information.

Side Effects: 
   IWIDGET_GetPropRGB() calls IWIDGET_GetProperty() and has the net effect of 
   processing an EVT_WDG_GETPROPERTY event.

See Also:
   Widget Events
   IWidget Interface
   IWIDGET_GetProperty()

==========================================================================

IWIDGET_GetPropPtr()

Description:
   This function is used to retrieve widget properties that are stored as pointers to
   more complex data structures such as model interfaces or font records.  For example,
   the PROP_VIEWMODEL property is stored as a pointer to a view model interface. This
   pointer would be the value copied into the function's 'pp' parameter if 
   IWIDGET_GetPropPtr() is called with PROP_VIEWMODEL as the requested property.  The
   function will return SUCCESS or EFAILED depending on whether or not the widget was able
   to retrieve the requested property.

   Widgets may possess any number of properties, as defined in AEEWProperties.h, and include
   attributes such as color, border width, shadowing, etc.  Examples of widget properties stored
   as an RGBVAL include PROP_ACTIVE_BORDERCOLOR, PROP_SELECTED_BORDERCOLOR and many others.  The
   function will return SUCCESS or EFAILED depending on whether or not the widget was able
   to retrieve the requested property.

Prototype:
   int IWIDGET_GetPropPtr(IWidget *po, uint16 wProp, void **pp); 

Parameters:
===pre>
   p:      Pointer to the IWidget interface object.
   wProp:  Identifies the property to be retrieved.
   pp:     A pointer to a pointer that provides reference to the actual property data.  For
           example, a request for a widet's PROP_FONT property would pass back a pointer to
           an IFont pointer, with that pointer pointing to the font information used by the
           widget.
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>
   
   The value of the requested RGB property is returned in 'prgb'.

Comments:
   Widgets may possess any number of properties, as defined in AEEWProperties.h, and include
   attributes such as color, border width, shadowing, etc.  Examples of widget properties stored
   as a pointer to more complex data include PROP_FONT, PROP_VIEWMODEL and many others.

Side Effects: 
   IWIDGET_GetPropPtr() calls IWIDGET_GetProperty() and has the net effect of 
   processing an EVT_WDG_GETPROPERTY event.

See Also:
   Widget Events
   IWidget Interface
   IWIDGET_GetProperty()

==========================================================================

IWIDGET_SetFocus()

Description:
   This helper function sets focus on a widget by calling the widget's
   handle event function with the event EVT_WDG_SETFOCUS.  

Prototype:
   void IWIDGET_SetFocus(IWidget* po, boolean bFocus);

Parameters:
===pre>
   po:      Pointer to the IWidget interface object.
   bFocus:  A boolean set to TRUE if the widget should have focus or FALSE
            if it should not have focus.
===/pre>

Return Value:
   None.

Comments:
   IWIDGET_SetFocus() does not return a meaningful value.  Use IWIDGET_HasFocus()
   to determine if focus was set properly.  

Side Effects: 
   None.

See Also:
   Widget Events
   IWidget Interface
   IWidget_HasFocus()
   IWidget_CanTakeFocus()

==========================================================================

IWIDGET_CanTakeFocus()

Description:
   This helper function will return whether or not the widget can take 
   focus.  It passes the event EVT_WDG_CANTAKEFOCUS to the widget's handle
   event function.

Prototype:
   int IWIDGET_CanTakeFocus(IWidget* po, boolean *pbFocus);

Parameters:
===pre>
   po:      Pointer to the IWidget interface object.
   pbFocus: Pointer to a boolean in memory which will be set to TRUE if the
            widget can take focus or FALSE if it cannot.
===/pre>

Return Value:
===pre>
   SUCCESS - The event was handled by the widget, and pbFocus was set.
   EFAILED - The event was not handled by the widget, and pbFocus was not set.
===/pre>

Comments:
   None.
  
Side Effects: 
   None.

See Also:
   Widget Events
   IWidget Interface
   IWidget_HasFocus()
   IWidget_SetFocus()

==========================================================================

IWIDGET_HasFocus()

Description:
   This helper function determines if the widget has focus.  It passes the
   EVT_WDG_HASFOCUS event to the widget's handle event function.

Prototype:
   int IWIDGET_HasFocus(IWidget* po, boolean *pbFocus);

Parameters:
===pre>
   po:      Pointer to the IWidget interface object.
   pbFocus: Pointer to a boolean in memory which will be set to TRUE if the 
            widget has focus or FALSE if the widget does not have focus.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to handle the event, and pbFocus was set.
   EFAILED - The widget was unable to handle the event, and pbFocus was not set.
===/pre>
   
Comments:
   None.

Side Effects: 
   None.

See Also:
   Widget Events
   IWidget Interface
   IWidget_SetFocus()
   IWidget_CanTakeFocus()

==========================================================================

IWIDGET_SetFlags()

Description:
   This helper function allows the caller to turn on a set of flags in 
   a widget that will change its behavior.  The caller should bitwise
   OR the flags it wishes to set together and pass the result in the
   dwFlags parameter.  
   
   The set of flags passed to IWIDGET_SetFlags() relpaces the flags 
   currently set in the widget.  That is, IWIDGET_SetFlags() turns off 
   all flags set in the widget, then turns on the ones specified in the 
   dwFlags parameter.

   IWIDGET_SetFlags() calls IWIDGET_SetProperty(), setting the PROP_FLAGS
   property to the set of flags specified in dwFlags.  Calling 
   IWIDGET_SetFlags() on a widget that doesn't support PROP_FLAGS will
   result in an error.  To determine if a specific type of widget 
   supports PROP_FLAGS and, more importantly, what set of flags the 
   widget understands, consult the widget's documentation.
   
   For example, the text widget defines the TWF_MULTILINE, 
   TWF_VERTICALNAV and TWF_PASSWORD flags.  You can consult the Text 
   Widget documentation for specific details on these flags.  To tell a 
   text widget to display text on multiple lines and allow vertical 
   cursor navigation, an application would call:

   IWIDGET_SetFlags(pTextWidget, TWF_MULTILINE|TWF_VERTICALNAV);

Prototype:
   int IWIDGET_SetFlags(IWidget *po, uint32 dwFlags);

Parameters:
===pre>
   po:      Pointer to the IWidget interface object.
   dwFlags: The set of logically ORed flags to enable in the widget.  
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to handle the event, and the internal
             flags were set to the set specified..
   EFAILED - The widget was unable to handle the event, and the internal
             flags were not changed..
===/pre>
   
Comments:
   If the application wishes to enable specific flags without disabling
   the current set of internal flags, it should call IWIDGET_AddFlags()
   instead.

   IWIDGET_SetFlags(0) can be used to turn all internal widget flags off.

Side Effects: 
   Depending on the widget implementation and what flags were set, the
   widget's behavior will change.  Refer to the widget's documentation
   to see what specific flags it supports and what effect they have.

See Also:
   Widget Events
   IWidget Interface
   IWIDGET_GetProperty()
   IWIDGET_GetFlags()
   IWIDGET_AddFlags()

==========================================================================

IWIDGET_GetFlags()

Description:
   This helper function allows the application to query the widget for the
   flags that are set internally.  This routine places the logically ORed 
   set of flags in the pdwFlags parameter.  The application may bitwise 
   AND this result with the flags the widget supports to determine which
   options are enabled or not.
   
   IWIDGET_GetFlags() calls IWIDGET_GetProperty() to retrieve the 
   PROP_FLAGS property.  Calling IWIDGET_SGetFlags() on a widget that 
   doesn't support PROP_FLAGS will result in an error.  To determine if a 
   specific type of widget supports PROP_FLAGS and what set of flags the 
   widget understands, consult the widget's documentation.
   
   For example, the Text Widget defines and understands the TWF_MULTILINE, 
   TWF_VERTICALNAV and TWF_PASSWORD flags. Each of these causes a text 
   widget to behave differently.  To determine if a specific flag is set, 
   an application would call:

   IWIDGET_GetFlags(pTextWidget, &dwFlags);

   The application can bitwise AND the result to determine which
   flags are set and which behavior the text widget currently exhibits:

   bTextIsMultiline = (dwFlags&TWF_MULTILINE);
   bVerticalNavAllowed = (dwFlags&TWF_VERTICALNAV);
   bInPasswordMode = (dwFlags&TWF_PASSWORD);

Prototype:
   int IWIDGET_GetFlags(IWidget *po, uint32 *pdwFlags);

Parameters:
===pre>
   po:         Pointer to the IWidget interface object.
   pdwFlags:   Pointer a uint32 that will be set to the logically ORed 
               set of flags that are turned on in the widget.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to handle the event, and the set of
             enabled flags was returned.
   EFAILED - The widget was unable to handle the event, and nothing
             valid was returned.
===/pre>
   
Comments:
   Use this routine to see all flags that are set internally in the
   widget by default or the last call to IWIDGET_SetFlags().

Side Effects: 
   None.

See Also:
   Widget Events
   IWidget Interface
   IWIDGET_GetProperty()
   IWIDGET_SetFlags()

==========================================================================

  IWIDGET_AddFlags()

Description:
   This helper function allows the application to enable specific flags
   in a widget without disabling any of the flags already set.  Unlike
   IWIDGET_SetFlags(), this routine switches the specified flags on
   while leaving all others untouched.

   The application should pass the set of flags to be enabled as a
   logically ORed set in the dwFlagsToAdd parameter.  These will be
   switched on internally in addition to whatever else was enabled 
   before the call.

   This helper function calls IWIDGET_GetFlags() to retrieve the current
   set of enabled flags, ORs in the set of flags to be turned on, then
   uses IWIDGET_SetFlags() to enable the new set along with the old.
   If the widget doesn't support the PROP_FLAGS parameter, an error
   code will be returned.  To determine if a specific type of widget
   supports PROP_FLAGS and what flags can be set, refer to that widget's
   documentation.

   For example, the Text Widget defines and understands the TWF_MULTILINE, 
   TWF_VERTICALNAV and TWF_PASSWORD flags. Each of these causes a text 
   widget to behave differently.  To tell a multiline text widget to
   allow vertical navigation, an application may call

   IWIDGET_AddFlags(pTextWidget, TWF_VERTICALNAV);

   This is equivalent to the following, which enables vertical
   navigation and retains the multiline behavior:

   IWIDGET_GetFlags(pTextWidget, &dwCurFlags);
   dwCurFlags |= TWF_VERTICALNAV;
   IWIDGET_SetFlags(pTextWidget, dwCurFlags);

Prototype:
   int IWIDGET_AddFlags(IWidget *po, uint32 dwFlagsToAdd);

Parameters:
===pre>
   po:            Pointer to the IWidget interface object.
   dwFlagsToAdd:  Set of logically ORed flags to turn on in the widget
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to handle the event, and the specified
             flags were enabled.
   EFAILED - The widget was unable to handle the event, and the widget's
             flags remain unchanged.
===/pre>
   
Comments:
   Use this routine to turn on certain flags without disabling others.
   IWIDGET_AddFlags(piw, dwFlags) is equivalent to:

   IWIDGET_GetFlags(piw, &dwCurFlags);
   dwCurFlags |= dwFlags;
   IWIDGET_SetFlags(piw, dwCurFlags);

   Use IWIDGET_RemoveFlags() to switch off specific flags.

Side Effects: 
   Depending on the widget implementation and what flags were set, the
   widget's behavior will change.  Refer to the widget's documentation
   to see what specific flags it supports and what effect they have.

See Also:
   Widget Events
   IWidget Interface
   IWidget_SetFlags()
   IWIDGET_GetFlags()
   IWIDGET_RemoveFlags()

==========================================================================

  IWIDGET_RemoveFlags()

Description:
   This helper function allows the application to disable specific flags
   in a widget without disabling any of the flags already set.  Unlike
   IWIDGET_SetFlags(0), this routine switches only the specified flags off
   while leaving all others enabled.

   The application should pass the set of flags to be disabled as a
   logically ORed set in the dwFlagsToRemove parameter.  These will be
   switched off internally while leaving whatever else was enabled 
   earlier alone.

   This helper function retrieves the set of flags that are enabled in
   the current widget using IWIDGET_GetFlags(), strips out the set
   of flags to be disabled, then uses IWIDGET_SetFlags() to turn them
   off while leaving the others switched on.  If the widget doesn't 
   support the PROP_FLAGS parameter, an error code will be returned.  To 
   determine if a specific type of widget supports PROP_FLAGS and what 
   flags can be set, refer to that widget's documentation.

   For example, the Text Widget defines and understands the TWF_MULTILINE, 
   TWF_VERTICALNAV and TWF_PASSWORD flags. Each of these causes a text 
   widget to behave differently.  To tell a multiline text widget to no
   longer allow vertical navigation, an application may call

   IWIDGET_RemoveFlags(pTextWidget, TWF_VERTICALNAV);

   This is equivalent to the following, which disables vertical
   navigation but retains the multiline behavior:

   IWIDGET_GetFlags(pTextWidget, &dwCurFlags);
   dwCurFlags &= ~TWF_VERTICALNAV;
   IWIDGET_SetFlags(pTextWidget, dwCurFlags);

Prototype:
   int IWIDGET_RemoveFlags(IWidget *po, uint32 dwFlagsToRemove);

Parameters:
===pre>
   po:               Pointer to the IWidget interface object.
   dwFlagsToRemove:  Set of logically ORed flags to turn off in the widget
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to handle the event, and only the 
             specified flags were disabled.
   EFAILED - The widget was unable to handle the event, and the widget's
             flags remain unchanged
===/pre>
   
Comments:
   IWidget_SetFlags(piw, 0) may be used to turn off all flags in a widget.
   Use this routine to quickly disable specific flags rather than all
   of them.  IWIDGET_RemoveFlags(piw, dwFlags) is equivalent to:

   IWIDGET_GetFlags(piw, &dwCurFlags);
   dwCurFlags &= ~dwFlags;
   IWIDGET_SetFlags(piw, dwCurFlags);

   Use IWIDGET_AddFlags() to switch on specific flags.

Side Effects: 
   Depending on the widget implementation and what flags were unset, the
   widget's behavior will change.  Refer to the widget's documentation
   to see what specific flags it supports and what effect they have.

See Also:
   Widget Events
   IWidget Interface
   IWidget_SetFlags()
   IWIDGET_GetFlags()
   IWIDGET_AddFlags()

==========================================================================

*/


#endif /* __AEEWIDGET_H__ */
