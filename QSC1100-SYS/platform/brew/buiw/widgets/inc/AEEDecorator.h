/*
  ========================================================================

  FILE:  AEEDecorator.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I define an abstract widget/container that
  decorates exactly one child. Concrete examples of me are List,
  Border and Blend

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEEDECORATOR_H__
#define __AEEDECORATOR_H__

#include "AEEContainer.h"

#include "bid/AEEIID_DECORATOR.bid"

typedef struct IDecorator IDecorator;
#define INHERIT_IDecorator(iname) \
   INHERIT_IWidget(iname); \
   void (*SetWidget)(iname *po, IWidget *piWidget);\
   void (*GetWidget) (iname *po, IWidget **ppWidget)

AEEINTERFACE(IDecorator) {
   INHERIT_IDecorator(IDecorator);
};


#define IDECORATOR_AddRef(p)                    AEEGETPVTBL((p),IDecorator)->AddRef(p)
#define IDECORATOR_Release(p)                   AEEGETPVTBL((p),IDecorator)->Release(p)
#define IDECORATOR_QueryInterface(p,i,p2)       AEEGETPVTBL((p),IDecorator)->QueryInterface(p,i,p2)
#define IDECORATOR_SetWidget(p,pw)              AEEGETPVTBL((p),IDecorator)->SetWidget((p),(pw))
#define IDECORATOR_GetWidget(p,ppw)             AEEGETPVTBL((p),IDecorator)->GetWidget((p),(ppw))
#define IDECORATOR_HandleEvent(p,e,w,d)         AEEGETPVTBL((p),IDecorator)->HandleEvent((p),(e),(w),(d))
#define IDECORATOR_SetHandler(p,pd)             AEEGETPVTBL((p),IDecorator)->SetHandler(p,pd)
#define IDECORATOR_GetPreferredExtent(p,x)      AEEGETPVTBL((p),IDecorator)->GetPreferredExtent((p),(x))
#define IDECORATOR_GetExtent(p,x)               AEEGETPVTBL((p),IDecorator)->GetExtent((p),(x))
#define IDECORATOR_SetExtent(p,x)               AEEGETPVTBL((p),IDecorator)->SetExtent((p),(x))
#define IDECORATOR_GetParent(p,ppc)             AEEGETPVTBL((p),IDecorator)->GetParent((p),(ppc))
#define IDECORATOR_SetParent(p,c)               AEEGETPVTBL((p),IDecorator)->SetParent((p),(c))
#define IDECORATOR_Draw(p,d,x,y)                AEEGETPVTBL((p),IDecorator)->Draw((p),(d),(x),(y))
#define IDECORATOR_IntersectOpaque(p,rd,ri)     AEEGETPVTBL((p),IDecorator)->IntersectOpaque((p),(rd),(ri))
#define IDECORATOR_GetModel(p,i,ppm)            AEEGETPVTBL((p),IDecorator)->GetModel((p),(i),(ppm))
#define IDECORATOR_SetModel(p,m)                AEEGETPVTBL((p),IDecorator)->SetModel((p),(m))

static __inline IWidget *IDECORATOR_TO_IWIDGET(IDecorator *p) {
   return (IWidget *)(void*)p;
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Decorator Interface ID

Description:
   Each class and interface within the widget framework is identified by a unique
   ID.  These IDs are used throughout the framework to distinguish between various
   types of objects, such as containers and widgets.

   The decorator interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as the base interface for all decorators.

   The decorator interface is identified by the interface ID below.

Definition:

Identifier         Definition
----------         -----------------------------------
AEEIID_DECORATOR:  Identifies the decorator interface

Comments:
   None

See Also:
   IDecorator Interface


===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IDecorator Interface

Description:
   IDecorator is an abstract interface meant to be a base from which more complex
   widget implementations are based.  A decorator is, essentially, a container that
   manages one and only one widget.  Widget implementations of the decorator widget
   typically function as a "visual enhancers" for the user interface.  For example,
   the list, grid and pick widgets all lend unique organization and layout to the
   information displayed by a list widget.  The blend widget -- also a decorator --
   provides a color mixing filter to the widgets it wraps.  The scrollbar is used as
   a list decorator to show relative position as the user moves through the widget
   that the scrollbar widget wraps (or, decorates).  The border widget (yes, another
   decorator) manages the visual properties of another widget's physical border.
   
   Decorators are wrapped around another widget and function as the parent to that
   widget.  To illustrate, consider the decorator implementation of the tab widget
   and the content widget it will decorate.  Visually, a tabbed interface looks like
   the following:
   
===pre>
                                                                        
        +-----------------+ +-----------------+ +-----------------+
        |     Tab One     | |     Tab Two     | |    Tab Three    | <---- Tab Widget
        |                 | +-----------------+ +-----------------+
      +-+                 +-------------------------------------------+
      |                                                               |
      |                                                               |
      |                                                               | <--- Wrapped
      |                      Content for Tab One                      |      Content
      |                                                               | 
      |                                                               |
      |                                                               |
      +---------------------------------------------------------------+
      
                  A typical tab widget with attached content
===/pre>  

   The tab widget itself consists of only the tabs, with this decorator acting as the
   parent to the content widget it wraps.  On the display, while the tabs and content
   appear to be one unified object, conceptually there are really two separate objects
   with a well defined parent/child relationship working together to create the illusion
   of a single interface object.

===pre>
         Parent
   +------------------+        
   |                  |            Child
   |    Decorator   +------> +----------------+
   |                  |      |                |
   +------------------+      |     Widget     | 
        Tab Widget           |                |
                             +----------------+
                               Content Widget
===/pre>  
   
   The decorator is the parent of the widget it wraps and manages various aspects of that
   widget's display and behavior.
   

Supported Events:
   The base decorator passes all received events to its child widget.

See Also:
   Blend Widget Class
   Border Widget Class
   Image Widget Class
   List Widget Class
   Scroll Widget Class
   Tab Widget Class
   Title Widget Class
   Viewport Widget Class
    
===========================================================================

IDECORATOR_AddRef()

Description:

   This function increments the reference count of the IDecorator interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).
   
   IDECORATOR_AddRef() inherits wholly from IWIDGET_AddRef().

Prototype:

   uint32 IDECORATOR_AddRef(IDecorator *p);

Parameters:
===pre>
   p:  Pointer to the IDecorator interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IDecorator interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IDECORATOR_Release()
   
=======================================================================

IDECORATOR_Release()

Description:

   This function decrements the reference count of the IDecorator interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IDECORATOR_Release(IDecorator *p);

Parameters:
===pre>
   p:  Pointer to the IDecorator interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IDecorator interface object.  If the
   returned reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IDECORATOR_AddRef()

=======================================================================

IDECORATOR_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IDECORATOR_QueryInterface(IDecorator *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IDecorator interface object.
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

IDECORATOR_SetWidget()

Description: 
   This function wraps the decorator around a target widget, and makes the target widget the
   child of the decorator.  Once wrapped, the decorator will provide management and visual
   services to the wrapped widget.  For example, an application that wishes display stylized
   black and white photos of various Hollywood starlets (think, raven haired beauties like
   Audrey Hepburn, Winona Ryder, Brenda Marshall and Rachel Leigh Cook) with flashy, blinking
   marquee lighting around the frame of each photo.  The application could create a special
   implementation of IDecorator -- the FlashyBlinkingMarquee Widget -- which it could then
   wrap around the StylizedBlackAndWhitePhoto Widget, by calling IDECORATOR_SetWidget() and
   passing in both the source decorator and the target widget.  Now, each time that fetching
   photo of Vanessa Paradis pops onto the display, it will do so with Hollywood panache,
   surrounded by flashing, twinkling (I mean, blinking) lights.

Prototype:

   void IDECORATOR_SetWidget(IDecorator *p, IWidget *pw)

Parameters:
===pre>
   p:   Pointer to the IDecorator object that will be wrapped around the target widget.
   pw:  Pointer to the target widget that will be managed by the decorator, or NULL if
        the decorator is to be completely detached from any child widgets.
===/pre>

Return Value:
   None

Comments:
   The reference count of the target widget 'w' will be incremented.

Side Effects: 
   None

See Also:
   IDecorator Interface
   IWidget Interface
   IDECORATOR_GetWidget()

=======================================================================

IDECORATOR_GetWidget()

Description: 
   This function retrieves the child widget that is wrapped by the decorator, returning this
   object as a pointer to an IWidget.  Once retrieved, an application may use this pointer to
   directly manipulate the widget wrapped by the decorator.  For example, calling this function
   on a tab widget (itself a decorator) would return a pointer to the content widget wrapped by
   the tab widget.

Prototype:

   void IDECORATOR_GetWidget(IDecorator *p, IWidget **ppw)

Parameters:
===pre>
   p:    Pointer to the IDecorator object from which the wrapped widget is to be retrieved.
   ppw:  Pointer to an IWidget pointer that will be returned by the function, providing
         access to the widget that is wrapped by the decorator.
===/pre>

Return Value:
   A pointer to the widget that is wrapped by the decorator is returned in 'ppw'.

Comments:
   The reference count of the returned widget pointed to by 'ppw' will be incremented.

   The pointer passed in the 'ppw' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   IDecorator Interface
   IWidget Interface
   IDECORATOR_SetWidget()

=======================================================================

IDECORATOR_HandleEvent()

Description:
   This function receives and handles events passed to the decorator from its owner.
   The decorator will pass all events directly to the child widget that it wraps.

Prototype:
   boolean IDECORATOR_HandleEvent(IDecorator *p, AEEEvent e, uint16 w, uint32 d);

Parameters:
===pre>   
   p:  A pointer to an IDecorator object.
   e:  The event code being sent to the IDecorator object.
   w:  16-bit event-specific parameter.
   d:  32-bit event-specific parameter.
===/pre>   
   
Return Value:
===pre>
   TRUE  --  If the event was handled by the object.
   FALSE --  If the event was not handled.
===/pre>

Comments:
   None

Side Effects: 
   None
   
See Also:
    Widget Events
    IDecorator Interface
   
=======================================================================

IDECORATOR_SetHandler()

Description:
   This function allows a decorator implementation to hook its own event handler into the
   normal event handling mechanism for decorators.  For example, an object that derives
   from the base IDecorator Interface could insert its own event handler that will be called
   each time an event is processed.
   
   The event handler is stored in a HandlerDesc data structure as a PFNHANDLER, along with
   a pointer to a block of memory allocate and owned by the decorator implementation.  This
   pointer will be passed into the decorator's event handler with each event to be processed.
   The  HandlerDesc data structure also contains a PFNFREEHANDLER that will be called when
   the decorator is released so that the decorator may free the private memory allocated and
   passed to the event handler.
   
   IDECORATOR_SetHandler() associates a given HandlerDesc data structure with a particular
   decorator.

Prototype:
   void IDECORATOR_SetHandler(IDecorator *p, HandlerDesc *pd);

Parameters:
===pre>   
   p:    A pointer to an IDecorator object.
   phd:  Pointer to the data structure that contains the event handler, private memory
         pointer, and the callback used to free the private memory.
===/pre>   
   
Return Value:
   None

Comments:
   Passing NULL for the HandlerDesc will reset the decorator's event handler to use the base
   widget's default event handler -- without private memory or the need to free privately
   allocated storage.
   
   The handler installed by a decorator to process events should itself remember to call
   HANDLERDESC_Call() so that the implementations that themselves derive from this decorator
   may also to hook into the decorator's event handler.

Side Effects: 
   None
   
See Also:
   IDecorator Interface
   HandlerDesc
   
===========================================================================

IDECORATOR_GetPreferredExtent()

Description:
   Returns the preferred extent of the decorator.  The preferred extent is the size
   that the decorator would choose to be under ideal circumstances.  For example,
   let's say decorator that manages the border for a row of icons wants to be 128
   pixels wide.  However, the actual size of the decorator could be constrained by
   any number of factors (the size of the device display, the size of the container
   object, etc) and may need to render the widget in a limited extent.  The widget
   framework distinguishes between the extent that an object prefers, and the
   extent that object is actually granted.

Prototype:
   void IDECORATOR_GetPreferredExtent(IDecorator *p, WExtent *x);
   
Parameters:
===pre>   
   p:  Pointer to the IDecorator object.
   x:  Pointer to the WExtent struct that, on output, will contain the preferred extent
       of the decorator.
===/pre> 

Return Value:
   The preferred extent for the decorator is returned in 'x'.
   
Comments:
   A decorator that is not wrapped around a widget (i.e. one without any child widgets)
   will return an extent of (0, 0).

Side Effects: 
   None

See Also:
    WExtent
    IDecorator Interface
    IWIDGET_SetPreferredExtent()
    IDECORATOR_GetExtent()
    IDECORATOR_SetExtent()
        
===========================================================================

IDECORATOR_GetExtent()

Description:
   Returns the horizontal and vertical extent of the decorator.  The extent is
   better thought of as the relative size of an object's bounding rectangle,
   independent of where that widget may be placed on the display.  The extent is
   returned in a WExtent data structure defining the width and height of the
   decorator.  Note that this is the _actual_ extent of the widget as it will
   appear on the display, as opposed to the preferred extent, which is the size
   the object would choose to be under ideal circumstances.
   
Prototype:
   void IDECORATOR_GetExtent(IDecorator *p, WExtent *x);
   
Parameters:
===pre>   
   p:  Pointer to the IDecorator object.
   x:  Pointer to the WExtent struct that, on output, will contain the current
       extent of the decorator.
===/pre>   

Return Value:
   The actual extent for the decorator is returned in 'x'.
   
Comments:
   None

Side Effects: 
   None

See Also:
    WExtent
    IDecorator Interface
    IDECORATOR_GetPreferredExtent()
    IDECORATOR_SetExtent()
    IWIDGET_SetPreferredExtent()

===========================================================================

IDECORATOR_SetExtent()

Description:
   Sets the horizontal and vertical extent of the decorator, independent of the
   decorator's preferred size.  IDECORATOR_SetExtent() communicates to the decorator
   the size it has been allocated on the display, and provides an opportunity for
   the decorator to adjust its content as best suited for the allotted size.  For
   example, a decorator that wraps a list widget would likely cause that widget to
   reformat its contents to better suit the new decorator extent.  As the size of
   the decorator changes, so shall the layout, formatting and arrangement of all the
   objects held within.  The new size assigned to the decorator MUST be honored by
   the decorator, its child widget, and all the children, grandchildren and the entire
   hierarchy of objects that may be contained within the child widget.
   
Prototype:
   void IDECORATOR_SetExtent(IDecorator *p, WExtent *x);
   
Parameters:
===pre>   
   p:  Pointer to the IDecorator object.
   x:  Pointer to the WExtent struct that provides the new extent of the decorator.
===/pre>

Return Value:
   None
   
Comments:
   Subsequent call to IDECORATOR_GetExtent() should return the same extent value sent
   in the call to IDECORATOR_SetExtent().

Side Effects: 
   None

See Also: 
    WExtent
    IDecorator Interface
    IDECORATOR_GetPreferredExtent()
    IDECORATOR_GetExtent()
    IWIDGET_SetPreferredExtent()

===========================================================================

IDECORATOR_GetParent()

Description:
   This function retrieves a pointer to the container object that serves as the
   parent to the decorator.  Objects within the widget framework maintain a
   hierarchy of parent/child relationships, wherein a container may enclose and
   manage one or more child objects.  The children of a parent container may be
   widgets, or additional containers and decorators, with each child receiving
   events and being managed by its parent.

   IDECORATOR_GetParent() inherits wholly from IWIDGET_GetParent().
   
Prototype:
   void IDECORATOR_GetParent(IDecorator *p, IContainer **ppc);
   
Parameters:
===pre>
   p:    Pointer to the IDecorator object.
   ppc:  Pointer to an IContainer pointer, into which IDECORATOR_GetParent() will
         return a pointer to the container that serves as the decorator's parent.
===/pre>
   
Return Value:
   A pointer to the decorator's parent container is returned in 'ppwc', or NULL if the
   decorator does not have a parent container.
   
Comments:
   The pointer passed in the 'ppc' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also: 
   IDecorator Interface
   IContainer Interface
   IDECORATOR_SetParent()
   IWIDGET_GetParent()

===========================================================================

IDECORATOR_SetParent()

Description:
   This function allows a container to identify itself as the parent of a decorator,
   or remove the decorator as one of its children.  Objects within the widget framework
   maintain a hierarchy of parent/child relationships, wherein a container may
   enclose and manage one or more child objects.  The children of a parent container
   may be widgets, or additional containers and decorators, with each child receiving
   events and being managed by its parent.

   IDECORATOR_SetParent() inherits wholly from IWIDGET_SetParent().
   
Prototype:
   void IDECORATOR_SetParent(IDecorator *p, IContainer *c);
   
Parameters:
===pre>
   p:  Pointer to the IDecorator object.
   c:  Pointer to the IContainer object that will act as the decorator's parent, or
       NULL if the decorator is being orphaned.
===/pre>
         
Return Value:
   none
   
Comments:
   Removing a parent/child relationship by passing NULL as a decorator's parent in
   the 'c' field does NOT move the object up one level in the object hierarchy.  In
   other words, if an object is the grandchild of a container and the object's
   parent is removed, the object does NOT become a child of the grandparent.
   
   IDECORATOR_SetParent() should only be called by a container that is managing the
   decorator.
   
   Decorator must not refcount their parents.  It is the parent's responsibility
   to set the parent pointer of all child objects to NULL prior to the parent
   being deleted.

Side Effects: 
   None

See Also:
   IDecorator Interface
   IContainer Interface
   IDECORATOR_GetParent()
   IWIDGET_SetParent()

===========================================================================
   
IDECORATOR_Draw()

Description:
   This function draws a decorator at a specific position on the device display.
   Drawing will occur within the bounds defined by the decorator's current extent,
   clipped to the clipRect defined in IDisplay.  If the decorator does any drawing
   through interfaces other than IDisplay, it is the decorator's responsibility to
   ensure that these operations are clipped appropriately (i.e. by calling
   IDISPLAY_GetClipRect() and drawing only within the bounds of the rectangle
   returned from IDisplay).
   
   The function specifies the horizontal and vertical position of the upper left
   hand corner of the bounding rectangle that will enclose the decorator.
   
Prototype:   
   void IDECORATOR_Draw(IDecorator *p, IDisplay *piDisplay, int x, int y);
                        
Parameters:
===pre>
   p:  Pointer to the IDecorator object to draw.
   d:  Pointer to the IDisplay interface that will provide drawing services
       to the decorator.
   x:  Horizontal position of the left side of the decorator, offset from the left
       side of the IDisplay object.
   y:  Vertical position of the top of the decorator, offset from the left side of
       the IDisplay object
===/pre>
      
Return Value:
   none
   
Comments:
   IDECORATOR_Draw () effectively draws the child widget wrapped by the decorator.

Side Effects: 
   None

See Also:
   IDecorator Interface
   IDisplay Interface

===========================================================================

IDECORATOR_IntersectOpaque()

Description:
   This function retrieves the intersection of a given rectangle with the largest opaque
   region of a given decorator.  To illustrate, consider the diagram below:
   
===pre>   
   +--------------------+
   |                    |
   |       source     -----------+ result of IDECORATOR_IntersectOpaque()
   |     rectangle   |  |
   |                 V  |
   |            +---------------------------------------+
   |            |/ / / /|                               |
   |            | / / / |      target decorator         |
   |            |/ / / /|                               |
   +------------|-------                                |
                |                                       |
                +---------------------------------------+
===/pre>  
   
   The function determines where the extent of the target decorator and the source
   rectangle overlap -- subtracting out, however, areas that lack opacity.  So, for
   example, if the decorator itself was transparent (i.e. it has no children or the
   background of the child widget has been set to be transparent), the rectangle
   that enclosed those transparent areas would not included in the result.
   
   If the rectangle's intersect, IDECORATOR_IntersectOpaque() will return TRUE,
   otherwise the function returns FALSE and the resultant rectangle is empty.
   
Prototype:
   boolean IDECORATOR_IntersectOpaque(IDecorator *p, AEERect *rd, const AEERect *ri);
   
Parameters:
===pre>
   p:   Pointer to the IDecorator object to be used as a target for intersection.
   rd:  Pointer to an AEERect, into which the function will return the intersection
        of the decorator's largest opaque rectangle and the rectangle passed to the
        function in 'prcIn'.
   ri:  Pointer to the AEERect that is to be intersected with the decorator's largest
        opaque rectangle.  This rectangle should be expressed in the target widget's
        coordinate system, with 0,0 being the top-left corner of the widget.
===/pre>
      
Return Value:
===pre>
   TRUE   -  If 'ri' intersects with the decorator's largest opaque rectangle.
   FALSE  -  If 'ri' does not intersect with the decorator largest opaque rectangle.
===/pre>
   
   The intersection of 'ri' and the decorator's largest opaque rectangle will be returned
   in 'rd', or returns the empty rectangle (0, 0, 0, 0) if the decorator is not wrapped
   around a widget (making the decorator, essentially, transparent).
   
Comments:
   By definition, the rectangle returned in 'rd' can be no larger than the rectangle
   passed to the function in 'ri'.
   
   The largest opaque rectangle for an object is defined to be the largest rectangle
   required to capture all opaque points and regions within an object.  So, for example,
   an object that looks something like this:
   
===pre>
      +-------------------------------------------------------+
      |                                                       |
      |                                                       |
      |      #######                                          |
      |      #######                                          |
      |      #######                                          |
      |                              ##                       |
      |                              ##                       |
      |                  #######                              |
      |                   #######                             |
      |                    #######                            |  # indicates opaque
      |                     #######                           |    regions within the
      |                      #######                          |    object.  All other
      |            ##                                         |    area is transparent
      |           ####                                        |
      |          ######                                       |
      |         ########                                      |
      |        ##########                                     |
      |       ############                                    |
      |                                             ##        |
      |                                            ####       |
      |                                             ##        |
      |                                                       |
      |                                                       |
      +-------------------------------------------------------+
===/pre>

   When expressed as its "largest opaque rectangle", the following rectangle
   would result (and subsequently compared to the source rectangle passed into
   the function to determine the intersecting rectangle):
   
===pre>
      +-------------------------------------------------------+
      |                                                       |
      |                                                       |
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |  # indicates area
      |      ##########################################       |    enclosed by the
      |      ##########################################       |    largest opaque
      |      ##########################################       |    rectangle
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |
      |      ##########################################       |
      |                                                       |
      |                                                       |
      +-------------------------------------------------------+
===/pre>

Side Effects: 
   None

See Also:
   IDecorator Interface
   
===========================================================================

IDECORATOR_GetModel()

Description:
  This function retrieves the model that provides data to a given decorator
  implementation (such as the image widget, etc). The model that is returned
  will implement the interface for the class ID passed to the function in the
  'i' parameter

  IDECORATOR_GetModel() inherits wholly from IWIDGET_GetModel().
   
Prototype:
   int IDECORATOR_GetModel(IDecorator *p, AEECLSID i, IModel **ppm);
   
Parameters:
===pre>
   p:    Pointer to the IDecorator object.
   i:    The class id of the model the decorator will use as a source for data.
   ppm:  Pointer to an IModel pointer into which the function will return a
         pointer to the IModel interface that supports the passed class id.
===/pre>
      
Return Value:
===pre>
   SUCCESS - The function was able to successfully locate the model interface
             identified by the 'i' parameter.
   EFAILED - The requested model interface is not currently declared.
===/pre>

   A pointer to the model interface associated with the decorator implementation
   is returned in 'ppm', or NULL if the decorator implementation does utilize a
   model interface.
   
Comments:
   The pointer passed in the 'ppm' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   IDecorator Interface
   IModel Interface

===========================================================================

IDECORATOR_SetModel()

Description:
   This function allows a decorator implementation to associate itself with a
   specific model interface.  To make this association, an application would first
   retrieve an IModel pointer for the desired interface class ID, then attach this
   model to the decorator implementation by passing the model pointer and decorator
   pointer to IDECORATOR_SetModel().
   
   Once the decorator implementation has associated itself with a given model
   interface, it should register as a listener for that model.  As a listener,
   the decorator implementation will receive notifications each time the model's
   data changes, thereby providing the opportunity for the decorator implementation
   to update its display or act in some other meaningful way to reflect the current
   data.
   
Prototype:
   int IDECORATOR_SetModel(IDecorator *p, IModel *pm);
   
Parameters:
===pre>
   p:  Pointer to the IDecorator object.
   m:  Pointer to the model interface that the decorator implementation should use
       as the basis for accessing its data.
===/pre>
      
Return Value:
===pre>
   SUCCESS          - The function was able to successfully associate the decorator
                      with the specified model interface.
   ECLASSNOTSUPPORT - The model interface passed to the function does not support
                      a model interface class ID that is known to the decorator.
===/pre>
   
Comments:
   None

Side Effects: 
   None

See Also:
   IDecorator Interface
   IModel Interface
   IDECORATOR_GetModel()
   IMODEL_AddListener()

===========================================================================

IDECORATOR_TO_IWIDGET()

Description:
   This function provides type safe casting of an IDecorator pointer to an IWidget
   pointer and should be used when passing an IDecorator pointer to functions that
   require a pointer to an IWidget.  This is safer than a simple cast, since the
   compiler will verify the pointer type.

Prototype:
   IWidget *IDECORATOR_TO_IWIDGET(IDecorator *p)

Parameters:
===pre>
   p:  Pointer to an IDecorator interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IWidget pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IDecorator Interface
   IWidget Interface

*/

#endif /* __AEEDECORATOR_H__ */
