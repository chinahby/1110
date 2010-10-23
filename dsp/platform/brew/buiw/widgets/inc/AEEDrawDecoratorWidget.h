/*
  ========================================================================

  FILE:  AEEDrawDecoratorWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEDRAWDECORATORWIDGET_H__
#define __AEEDRAWDECORATORWIDGET_H__

#include "AEEDecorator.h"
#include "AEEDrawHandler.h"
#include "AEEWProperties.h"

#include "bid\AEECLSID_DRAWDECORATORWIDGET.bid"

// See AEEDrawHandler.h for definitions of:
//    PFNDRAWHANDLER 
//    DrawHandlerDesc
//    DRAWHANDLERDESC_Init 
//    DRAWHANDLERDESC_Call
//    DRAWHANDLERDESC_Free


typedef struct IDrawDecorator IDrawDecorator;
#define INHERIT_IDrawDecorator(iname) \
   INHERIT_IDecorator(iname); \
   void     (*SetDraw)(iname *, DrawHandlerDesc *pDesc)

AEEINTERFACE(IDrawDecorator) {
   INHERIT_IDrawDecorator(IDrawDecorator);
};


#define IDRAWDECORATOR_AddRef(p)                    AEEGETPVTBL((p),IDrawDecorator)->AddRef(p)
#define IDRAWDECORATOR_Release(p)                   AEEGETPVTBL((p),IDrawDecorator)->Release(p)
#define IDRAWDECORATOR_QueryInterface(p,i,p2)       AEEGETPVTBL((p),IDrawDecorator)->QueryInterface(p,i,p2)
#define IDRAWDECORATOR_SetWidget(p,pw)              AEEGETPVTBL((p),IDrawDecorator)->SetWidget((p),(pw))
#define IDRAWDECORATOR_GetWidget(p,ppw)             AEEGETPVTBL((p),IDrawDecorator)->GetWidget((p),(ppw))
#define IDRAWDECORATOR_HandleEvent(p,e,w,d)         AEEGETPVTBL((p),IDrawDecorator)->HandleEvent((p),(e),(w),(d))
#define IDRAWDECORATOR_SetHandler(p,pd)             AEEGETPVTBL((p),IDrawDecorator)->SetHandler(p,pd)
#define IDRAWDECORATOR_GetPreferredExtent(p,x)      AEEGETPVTBL((p),IDrawDecorator)->GetPreferredExtent((p),(x))
#define IDRAWDECORATOR_GetExtent(p,x)               AEEGETPVTBL((p),IDrawDecorator)->GetExtent((p),(x))
#define IDRAWDECORATOR_SetExtent(p,x)               AEEGETPVTBL((p),IDrawDecorator)->SetExtent((p),(x))
#define IDRAWDECORATOR_GetParent(p,ppc)             AEEGETPVTBL((p),IDrawDecorator)->GetParent((p),(ppc))
#define IDRAWDECORATOR_SetParent(p,c)               AEEGETPVTBL((p),IDrawDecorator)->SetParent((p),(c))
#define IDRAWDECORATOR_Draw(p,d,x,y)                AEEGETPVTBL((p),IDrawDecorator)->Draw((p),(d),(x),(y))
#define IDRAWDECORATOR_IntersectOpaque(p,rd,ri)     AEEGETPVTBL((p),IDrawDecorator)->IntersectOpaque((p),(rd),(ri))
#define IDRAWDECORATOR_GetModel(p,i,ppm)            AEEGETPVTBL((p),IDrawDecorator)->GetModel((p),(i),(ppm))
#define IDRAWDECORATOR_SetModel(p,m)                AEEGETPVTBL((p),IDrawDecorator)->SetModel((p),(m))
#define IDRAWDECORATOR_GetController(p,ppc)         AEEGETPVTBL((p),IDrawDecorator)->GetController(p,ppc)
#define IDRAWDECORATOR_SetController(p,pc)          AEEGETPVTBL((p),IDrawDecorator)->SetController(p,pc) 
#define IDRAWDECORATOR_SetDraw(p,pd)                AEEGETPVTBL((p),IDrawDecorator)->SetDraw(p,pd) 

static __inline IDecorator *IDRAWDECORATOR_TO_IDECORATOR(IDrawDecorator *p) {
   return (IDecorator *)(void*)p;
}

static __inline IWidget *IDRAWDECORATOR_TO_IWIDGET(IDrawDecorator *p) {
   return IDECORATOR_TO_IWIDGET(IDRAWDECORATOR_TO_IDECORATOR(p));
}

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Draw Decorator Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   draw decorator widget by creating a new instance of the Draw Decorator Widget class,
   passing the ID of the class to ISHELL_CreateInstance().  The object that is created
   is, itself, a widget and a decorator and may utilize those APIs defined for the
   IWidget and IDecorator interfaces.
   
   The class ID used to identify the draw decorator widget is defined below.

Definition:

Identifier                     Definition
----------                     -----------------------------------
AEECLSID_DRAWDECORATORWIDGET:  Identifies the class ID of the draw decorator widget

Comments:
   None

See Also:
   IWidget Interface
   IDecorator Interface
   ISHELL_CreateInstance()

=======================================================================

PFNDRAWHANDLER

Description:
   This type definition specifies the prototype for the function pointer an application
   may register as the draw handler for a draw decorator widget.  The draw handler allows
   an allocation to hook a custom drawing routine into the default draw operation normally
   invoked for a decorator.  For example, an application could choose to enhance the display
   of a decorator by drawing a translucent winking eyeball in the background (ooooo! ahhhh!).
   This custom draw handler will be called by the draw decorator whenever the decorator is
   asked to draw itself.  The handler is NOT intended to override and replace an object's
   normal draw handler.  Rather, it is intended to perform additional drawing and should
   itself call DRAWHANDLERDESC_Call() to continue the chain of drawing operations.
   
   The widgets framework will call the PFNDRAWHANDLER with the same parameters normally
   passed to IDECORATOR_Draw(), and additionally passes an additional parameter that
   identifies a pointer to private data owned and managed by the application, which the
   application provides to the draw decorator at the time the draw handler is registered.

Definition:
    typedef void (*PFNDRAWHANDLER)(void *pCxt, ICanvas *piCanvas, int x, int y);

Members:
===pre>
   pCxt:      A pointer to private data allocated and managed by the caller that
              has registered the draw handler with the draw decorator.  This data
              pointer is passed into the draw handler, providing the callback with
              access to this private storage.
   piCanvas:  Pointer to the ICanvas interface that will provide drawing services
              to the decorator.
   x:         Horizontal position of the left side of the decorator, offset from
              the left side of the ICanvas object.
   y:         Vertical position of the top of the decorator, offset from the left
              side of the ICanvas object
===/pre>

Comments:
    The draw handler is registered with a draw decorator widget by initializing the 'pfn'
    field of a DrawHandlerDesc data structure and calling IDRAWDECORATOR_SetDraw() to
    hook the draw handler into the decorator's normal drawing process.  Upon registering
    the draw handler, IDRAWDECORATOR_SetDraw() will return a pointer to a DrawHandlerDesc
    data structure that contains a pointer to the draw handler that would have been called
    had the new handler not been registered.  The PFNDRAWHANDLER function should pass this
    DrawHandlerDesc pointer to the old draw handler when calling DRAWHANDLERDESC_Call() in
    order to ensure that normal decorator drawing still takes place. 

See Also:
   ICanvas Interface
   DrawHandlerDesc
   IDRAWDECORATOR_SetDraw()
   DRAWHANDLERDESC_Call()

=============================================================================

DrawHandlerDesc

Description:
   This data type defines the callback handlers a draw decorator will use to draw itself
   and -- when the draw decorator is finally released -- free any private storage allocated
   by the draw decorator.

Definition:
   typedef struct {
      PFNDRAWHANDLER    pfn;
      void             *pCxt;
      PFNFREEHANDLER    pfnFree;
   } DrawHandlerDesc;

Members:
===pre>
   pfn:      Pointer to the callback function that BREW will call when drawing
             the decorator.  This routine will override the default drawing routine
             for the decorator, allowing an application to apply a custom rendering
             technique to the widget wrapped by the draw decorator object.
   pCxt:     A pointer to private data allocated and managed by the caller that
             has registered the draw handler with the draw decorator.  This data
             pointer is passed into the draw handler, as well as the free handler,
             providing each of these callbacks with access to this private storage.
   pfnFree:  Pointer to the callback function that BREW will call when freeing the
             memory associated with a draw decorator upon releasing the object.
             This gives the draw decorator the opportunity to release any private
             "deep" storage that may have been allocated by the object.
===/pre>
Comments:
   A pointer to a DrawHandlerDesc data structure is passed to IDRAWDECORATOR_SetDraw() to
   register the callbacks a draw decorator will use to perform custom drawing and memory
   free routines.

See Also:
   PFNDRAWHANDLER
   PFNFREEHANDLER
   IDRAWDECORATOR_SetDraw()

===================================================================================
   MACRO DOCUMENTATION
===================================================================================

DRAWHANDLERDESC_Init()

Description:
   This macro initializes each of the fields of a DrawHandlerDesc data structure.  An
   application implementing a draw decorator that provides its own draw or free callback
   routine should call DRAWHANDLERDESC_Init() after creating an instance of the draw
   decorator object.

Prototype:
   DRAWHANDLERDESC_Init(DrawHandlerDesc *pdesc, PFNDRAWHANDLER ph, void *pv, PFNFREEHANDLER pf);

Parameters:
===pre>
   pdesc:  Pointer to a DrawHandlerDesc data structure, into which the macro will
           copy the remaining parameters.
   ph:     Pointer to the callback function that BREW will call when drawing the
           the decorator.  This routine will override the default drawing routine
           for the decorator, allowing an application to apply a custom rendering
           technique to the widget wrapped by the draw decorator object.
   pv:     A pointer to private data allocated and managed by the caller that
           has registered the draw handler with the draw decorator.  This data
           pointer is passed into the draw handler, as well as the free handler,
           providing each of these callbacks with access to this private storage.
   pf:     Pointer to the callback function that BREW will call when freeing the
           memory associated with a draw decorator upon releasing the object.
           This gives the draw decorator the opportunity to release any private
           "deep" storage that may have been allocated by the object.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   DrawHandlerDesc
   PFNDRAWHANDLER
   PFNFREEHANDLER

===================================================================================

DRAWHANDLERDESC_Call()

Description:
   This macro issues the drawing callback referenced by the draw handler descriptor
   passed to the function in the 'pdesc' parameter.  An application that overrides a
   decorator's default draw routine by supplying a custom draw handler should ALWAYS
   call DRAWHANDLERDESC_Call() within its draw routine, passing in a pointer to the
   draw handler descriptor returned by IDRAWDECORATOR_SetDraw() when the current
   draw handler was initially registered.  The application providing a custom draw
   decorator consequently has the option of applying its own drawing operations prior
   to, or after, the default draw operation, depending on the needs of that particular
   draw decorator.

Prototype:
   DRAWHANDLERDESC_Call(DrawHandlerDesc *pdesc, ICanvas *canv, int x, int y);

Parameters:
===pre>
   pdesc:  Pointer to a DrawHandlerDesc data structure that contains a pointer
           to the draw handler BREW should execute.
   canv:   Pointer to the ICanvas object onto which the draw handler will perform
           its drawing operations
   x:      Horizontal position of the left side of the draw decorator, offset
           from the left side of the ICanvas object.
   y:      Vertical position of the top of the draw decorator, offset from the
           top of the ICanvas object.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   DrawHandlerDesc
   IDRAWDECORATOR_SetDraw()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IDrawDecorator Interface


Description:
   The widgets framework provides several built-in decorators such as the border and blend
   widgets, and the gradient decorator that perform very specific drawing operations on wrapped
   objects.   The draw decorator provides applications with the ability to perform customized
   drawing operations to the objects wrapped by a decorator.  For example, an application that
   wishes to display a border that contains itty bitty teeny tiny bitmap images of flesh eating
   zombies could create a draw decorator widget, wrap this widget around some other content, then
   supply a custom draw handler that would be called each time the draw decorator is asked to
   draw itself.  The custom draw handler would then render the flesh eating zombies in all their
   grotesque and stomach churning glory.
   
   It is important to note that a draw decorator widget is intended to enhance -- rather than
   replace -- the normal drawing activity associated with a decorator.  The widget framework
   maintains what could be viewed as a chain of draw handlers for each object.  Usually, this 
   conceptual drawing chain will only include the draw handler for the widget wrapped by the
   decorator, like so:
   
===pre>
       +-------------------------+
       |                         |
       |        Decorator        |     +---------------+
       |      Draw Handler     +-----> |    Widget     |
       |                         |     +---------------+
       +-------------------------+
===/pre>
   
   A decorator such as a blend widget or border widget hooks into this chain by applying its
   own draw handler to render its specific visual effect.  The border widget, for example, will
   hook its draw handler before the widget it wraps is drawn, like so:

===pre>
             +-------------------------+
             |                         |
             |     Border Widget       |     +---------------+
         +------+   Draw Handler     +-----> |    Border     |    drawn first
         |   |                         |     +---------------+
         |   +-------------------------+
         |
         |
         |   +-------------------------+
         +-->|                         |
             |        Decorator        |     +---------------+
             |      Draw Handler     +-----> |    Widget     |    drawn second
             |                         |     +---------------+
             +-------------------------+
===/pre>
   
   So, as illustrated the border is drawn first, followed by the widget it wraps.
   
   Hooking a custom draw handler to a draw decorator has the same effect, with the new
   handler inserting itself into the calling chain, either before or after the default
   handler supplied by the decorator.  The figure below illustrates how a custom draw
   handler is linked into the drawing chain.

===pre>   
             +-------------------------+
             |                         |
             |         Custom          |     +---------------+
         +------+   Draw Handler     +-----> |    Custom     |    drawn first
         |   |                         |     |   Drawing     |
         |   +-------------------------+     +---------------+
         |
         |
         |   +-------------------------+
         +-->|                         |
             |        Decorator        |     +---------------+
             |      Draw Handler     +-----> |    Widget     |    drawn second
             |                         |     +---------------+
             +-------------------------+
===/pre>
   
   An application  wishing to hook its own custom draw handler into the decorator drawing
   chain must first register its draw handler by calling IDRAWDECORATOR_SetDraw().  The call
   to IDRAWDECORATOR_SetDraw() returns a pointer to a data structure containing the decorator's
   current draw handler (i.e. the draw handler that would be called if a custom handler had not
   been attached to the decorator).  The application should save this pointer and pass it to
   the decorator interface in a subsequent call to DRAWHANDLERDESC_Call() within the custom
   draw handler itself to insure that the drawing chain continues.  The application may choose
   to perform its drawing either prior to or following the call to DRAWHANDLERDESC_Call().
   For example, our previous example of the flesh eating zombie custom draw handler would, like
   a border widget, likely choose to draw all of those scary zombies as a frame around the
   decorator, then call DRAWHANDLERDESC_Call() to allow the widget it wraps to draw itself.
   A custom decorator draw handler that wishes to superimpose a logo over the top of another
   object (sort of like station branding on cable TV broadcasts), might choose to call
   DRAWHANDLERDESC_Call() first, then perform it's own custom rendering.
   
Supported Events:
   The draw decorator passes all received events to its child widget.

See Also:
   IDecorator Interface

===========================================================================

IDRAWDECORATOR_AddRef()

Description:

   This function inherits from the IDecorator interface and increments the reference count
   of the IDrawDecorator interface object, thereby allowing the interface to be shared by multiple
   callers.  The object is freed when the reference count reaches 0 (zero).
   
   IDRAWDECORATOR_AddRef() inherits wholly from IDECORATOR_AddRef().

Prototype:

   uint32 IDRAWDECORATOR_AddRef(IDrawDecorator *p);

Parameters:
===pre>
   p:  Pointer to the IDrawDecorator interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IDrawDecorator interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IDRAWDECORATOR_Release()
   IDECORATOR_AddRef()
   
=======================================================================

IDRAWDECORATOR_Release()

Description:

   This function decrements the reference count of the IDrawDecorator interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IDRAWDECORATOR_Release(IDrawDecorator *p);

Parameters:
===pre>
   p:  Pointer to the IDrawDecorator interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IDrawDecorator interface object.  If the
   returned reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IDRAWDECORATOR_AddRef()

=======================================================================

IDRAWDECORATOR_QueryInterface()

Description: 
   This function queries an interface for an alternate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

   IDRAWDECORATOR_QueryInterface() inherits wholly from IDECORATOR_QueryInterface().

Prototype:

   int IDRAWDECORATOR_QueryInterface(IDrawDecorator *p, AEECLSID c, void **p2)

Parameters:
===pre>
   p:   Pointer to the IDrawDecorator interface object.
   c:   A globally unique id that identifies the interface to be queried.
   p2:  Pointer to a pointer returned by the function to reference the
        interface requested by the 'c' parameter.  If the queried item is not
        available, this pointer is returned as NULL.
===/pre>

Return Value:
===pre>
   SUCCESS          - The requested interface was found and has been returned
                      in the 'p2' parameter.
   ECLASSNOTSUPPORT - The requested interface is not supported.
===/pre>

Comments:
   The pointer passed in the 'p2' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper
   level of indirection is used in calling the function.

Side Effects: 
   Upon successful retrieval of an interface pointer, that interface is returned with
   its reference count incremented.  The caller is responsible for releasing this object
   at some point in the future.

See Also:
   IDECORATOR_QueryInterface()
   IQI_QueryInterface()

=======================================================================

IDRAWDECORATOR_SetWidget()

Description: 
   This function wraps the draw decorator around a target widget, and makes the target widget
   the child of the draw decorator.  Once wrapped, the draw decorator may use a custom draw
   handler to enhance the visual display of the widget.  For example, a draw decorator might
   be created to draw a wavy frame around a widget that displays bitmap images.  The application
   would call IDRAWDECORATOR_SetWidget() specifying both the draw decorator and the bitmap
   widget to make the decorator/widget binding, then hook a custom "wiggly frame" draw handler
   to the draw decorator to achieve the visual desired effect.
   
Prototype:

   void IDRAWDECORATOR_SetWidget(IDrawDecorator *p, IWidget *pw)

Parameters:
===pre>
   p:   Pointer to the IDrawDecorator object that will be wrapped around the target widget.
   pw:  Pointer to the target widget that will be managed by the draw decorator, or NULL if
        the decorator is to be completely detached from any child widgets.
===/pre>

Return Value:
   None

Comments:
   The reference count of the target widget 'pw' will be incremented.
   
Side Effects: 
   Calling IDRAWDECORATOR_SetWidget() will set the extent of the draw decorator to be the
   exact same size as the widget it wraps.  Therefore, a draw decorator that intends to
   decorate the widget "outside the lines" (i.e. a frame or border that draws around the
   widget), should subsequently call IDRAWDECORATOR_SetExtent() to force the extent of the
   draw decorator to be greater than that of the child widget.  Conversely, the application
   could call IWIDGET_SetExtent() to force the extent of the wrapped widget to be smaller
   than the draw decorator.

See Also:
   IDrawDecorator Interface
   IWidget Interface
   IDRAWDECORATOR_GetWidget()
   IDRAWDECORATOR_SetExtent()
   IWIDGET_SetExtent()

=======================================================================

IDRAWDECORATOR_GetWidget()

Description: 
   This function retrieves the child widget that is wrapped by the draw decorator, returning
   this object as a pointer to an IWidget.  Once retrieved, an application may use this pointer
   to directly manipulate the widget wrapped by the draw decorator.  For example, calling this
   function on a tab widget (itself a decorator) would return a pointer to the content widget
   wrapped by the tab widget.

   IDRAWDECORATOR_GetWidget() inherits wholly from IDECORATOR_GetWidget().

Prototype:

   void IDRAWDECORATOR_GetWidget(IDrawDecorator *p, IWidget **ppw)

Parameters:
===pre>
   p:    Pointer to the IDrawDecorator object from which the wrapped widget is to be
         retrieved.
   ppw:  Pointer to an IWidget pointer that will be returned by the function, providing
         access to the widget that is wrapped by the draw decorator.
===/pre>

Return Value:
   A pointer to the widget that is wrapped by the draw decorator is returned in 'ppw'.

Comments:
   The reference count of the returned widget pointed to by 'ppw' will be incremented.

   The pointer passed in the 'ppw' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   IDrawDecorator Interface
   IWidget Interface
   IDRAWDECORATOR_SetWidget()
   IDECORATOR_GetWidget()
   
=======================================================================

IDRAWDECORATOR_HandleEvent()

Description:
   This function receives and handles events passed to the draw decorator from its owner.
   The draw decorator will pass all events directly to the child widget that it wraps.

   IDRAWDECORATOR_HandleEvent() inherits wholly from IDECORATOR_HandleEvent().

Prototype:
   boolean IDRAWDECORATOR_HandleEvent(IDrawDecorator *p, AEEEvent e, uint16 w, uint32 d);

Parameters:
===pre>   
   p:  A pointer to an IDrawDecorator object.
   e:  The event code being sent to the IDrawDecorator object.
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
    IDrawDecorator Interface
    IDECORATOR_HandleEvent()

=======================================================================

IDRAWDECORATOR_SetHandler()

Description:
   This function allows a draw decorator implementation to hook its own event handler into
   the normal event handling mechanism for decorators.  For example, an object that derives
   from the base IDecorator Interface could insert its own event handler that will be called
   each time an event is processed.
   
   The event handler is stored in a HandlerDesc data structure as a PFNHANDLER, along with
   a pointer to a block of memory allocate and owned by the draw decorator implementation.
   This pointer will be passed into the draw decorator's event handler with each event to be
   processed.  The HandlerDesc data structure also contains a PFNFREEHANDLER that will be
   called when the draw decorator is released so that the decorator may free the private
   memory allocated and passed to the event handler.
   
   IDRAWDECORATOR_SetHandler() associates a given HandlerDesc data structure with a particular
   draw decorator.

   IDRAWDECORATOR_SetHandler() inherits wholly from IDECORATOR_SetHandler().

Prototype:
   void IDRAWDECORATOR_SetHandler(IDrawDecorator *p, HandlerDesc *pd);

Parameters:
===pre>   
   p:    A pointer to an IDrawDecorator object.
   phd:  Pointer to the data structure that contains the event handler, private memory
         pointer, and the callback used to free the private memory.
===/pre>   
   
Return Value:
   None

Comments:
   Passing NULL for the HandlerDesc will reset the draw decorator's event handler to use the
   base widget's default event handler -- without private memory or the need to free privately
   allocated storage.
   
   The handler installed by a draw decorator to process events should itself remember to call
   HANDLERDESC_Call() so that the implementations that themselves derive from this decorator
   may also to hook into the draw decorator's normal event handling mechanism.

   An application that hooks into both a draw decorator's event handler and draw handler (by
   calling IDRAWDECORATOR_SetDraw()), should exercise care in managing the private data
   released by the PFNFREEHANDLER stored in the HandlerDesc data structure.  Event handlers
   and draw handlers both manage data structures that contain pointers to private storage,
   as well as a pointer to a function that will be used to release this storage -- which
   could be different... or could be the same!  Each of these PFNFREEHANDLER routines will
   be called when the draw decorator is released.  Therefore, an application that uses the
   same block of private storage for both its custom event and draw handling procedures
   should be careful to check the validity of the storage to be freed before actually
   attempting to release the memory.
   
Side Effects: 
   None
   
See Also:
   IDrawDecorator Interface
   PFNHANDLER
   PFNFREEHANDLER
   HandlerDesc
   IDRAWDECORATOR_SetDraw()
   IDECORATOR_SetHandler()
   
===========================================================================

IDRAWDECORATOR_GetPreferredExtent()

Description:
   Returns the preferred extent of the draw decorator.  The preferred extent is the
   size that the draw decorator would choose to be under ideal circumstances.  For
   example, let's say a draw decorator wishes to adorn the outside of a bit mapped
   image by drawing a fancy, ornate gold and gilded frame (think oil paintings by
   Flemish masters hanging in the upper floors of the Metropolitan Museum of Art in
   New York).  Ideally, this frame would be impressively drawn, as large as possible
   to distract the viewer from the sameness of yet-another austere portrait of a
   European nobleman.  Here, the preferred extent might be returned as the full
   dimensions of the screen.  Though this is the preferred extent of the object, the
   draw decorator will automatically size itself, at creation, to be the same extent
   as the object it wraps.  In the above example, that ostentatious, gaudy gold frame
   will be shrunk to be the exact same size as the bitmap image it wraps.  However,
   when the application calls IDRAWDECORATOR_GetPreferredExtent() the full glorious
   size of the draw decorator would be returned.

Prototype:
   void IDRAWDECORATOR_GetPreferredExtent(IDrawDecorator *p, WExtent *x);
   
Parameters:
===pre>   
   p:  Pointer to the IDrawDecorator object.
   x:  Pointer to the WExtent struct that, on output, will contain the preferred extent
       of the draw decorator.
===/pre> 

Return Value:
   The preferred extent for the draw decorator is returned in 'x'.
   
Comments:
   A draw decorator that is not wrapped around a widget (i.e. one without any child
   widgets) will return an extent of (0, 0).

Side Effects: 
   None

See Also:
    WExtent
    IDrawDecorator Interface
    IWIDGET_SetPreferredExtent()
    IDRAWDECORATOR_GetExtent()
    IDRAWDECORATOR_SetExtent()
        
===========================================================================

IDRAWDECORATOR_GetExtent()

Description:
   Returns the horizontal and vertical extent of the draw decorator.  The extent
   is better thought of as the relative size of an object's bounding rectangle,
   independent of where that widget may be placed on the display.  The extent is
   returned in a WExtent data structure defining the width and height of the
   decorator.  Note that this is the _actual_ extent of the draw decorator as it
   will appear on the display, as opposed to the preferred extent, which is the
   size the object would choose to be under ideal circumstances.
   
Prototype:
   void IDRAWDECORATOR_GetExtent(IDrawDecorator *p, WExtent *x);
   
Parameters:
===pre>   
   p:  Pointer to the IDrawDecorator object.
   x:  Pointer to the WExtent struct that, on output, will contain the current
       extent of the draw decorator.
===/pre>   

Return Value:
   The actual extent for the draw decorator is returned in 'x'.
   
Comments:
   None

Side Effects: 
   None

See Also:
    WExtent
    IDrawDecorator Interface
    IDRAWDECORATOR_GetPreferredExtent()
    IDRAWDECORATOR_SetExtent()
    IWIDGET_SetPreferredExtent()

===========================================================================

IDRAWDECORATOR_SetExtent()

Description:
   Sets the horizontal and vertical extent of the draw decorator, independent of its
   preferred size.  IDRAWDECORATOR_SetExtent() tells the draw decorator the size it
   has been allocated on the display, and provides an opportunity for it to adjust
   its content as best suited for the allotted size.  For example, a draw decorator
   that wraps a tab widget would likely cause that widget to reformat its contents to
   better suit the updated size of the draw decorator.  As the extent of the draw
   decorator changes, so shall the layout, formatting and arrangement of all the
   objects held within.  The new size assigned to the draw decorator MUST be honored
   by the draw decorator, its child widget, and all the children, grandchildren and
   the entire hierarchy of objects that may be contained within the child widget.
   
Prototype:
   void IDRAWDECORATOR_SetExtent(IDrawDecorator *p, WExtent *x);
   
Parameters:
===pre>   
   p:  Pointer to the IDrawDecorator object.
   x:  Pointer to the WExtent struct that provides the new extent of the draw
       decorator.
===/pre>

Return Value:
   None
   
Comments:
   Subsequent call to IDRAWDECORATOR_GetExtent() should return the same extent value
   sent in the call to IDRAWDECORATOR_SetExtent().

Side Effects: 
   None

See Also: 
    WExtent
    IDrawDecorator Interface
    IDRAWDECORATOR_GetPreferredExtent()
    IDRAWDECORATOR_GetExtent()
    IWIDGET_SetPreferredExtent()

===========================================================================

IDRAWDECORATOR_GetParent()

Description:
   This function retrieves a pointer to the container object that serves as the
   parent to the draw decorator.  Objects within the widget framework maintain a
   hierarchy of parent/child relationships, wherein a container may enclose and
   manage one or more child objects.  The children of a parent container may be
   widgets, or additional containers and decorators, with each child receiving
   events and being managed by its parent.

   IDRAWDECORATOR_GetParent() inherits wholly from IDECORATOR_GetParent().
   
Prototype:
   void IDRAWDECORATOR_GetParent(IDrawDecorator *p, IContainer **ppc);
   
Parameters:
===pre>
   p:    Pointer to the IDrawDecorator object.
   ppc:  Pointer to an IContainer pointer, into which IDRAWDECORATOR_GetParent()
         will return a pointer to the container that serves as the decorator's
         parent.
===/pre>
   
Return Value:
   A pointer to the draw decorator's parent container is returned in 'ppwc', or
   NULL if the draw decorator does not have a parent container.
   
Comments:
   The pointer passed in the 'ppc' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also: 
   IDrawDecorator Interface
   IContainer Interface
   IDRAWDECORATOR_SetParent()
   IDECORATOR_GetParent()
   IWIDGET_GetParent()

===========================================================================

IDRAWDECORATOR_SetParent()

Description:
   This function allows a container to identify itself as the parent of a draw
   decorator, or remove the draw decorator as one of its children.  Objects within
   the widget framework maintain a hierarchy of parent/child relationships, wherein
   a container may enclose and manage one or more child objects.  The children of a
   parent container may be widgets, or additional containers and decorators, with
   each child receiving events and being managed by its parent.

   IDRAWDECORATOR_SetParent() inherits wholly from IDECORATOR_SetParent().
   
Prototype:
   void IDRAWDECORATOR_SetParent(IDrawDecorator *p, IContainer *c);
   
Parameters:
===pre>
   p:  Pointer to the IDrawDecorator object.
   c:  Pointer to the IContainer object that will act as the draw decorator's
       parent, or NULL if the draw decorator is being orphaned.
===/pre>
         
Return Value:
   none
   
Comments:
   Removing a parent/child relationship by passing NULL as a draw decorator's parent
   in the 'c' field does NOT move the object up one level in the object hierarchy.
   In other words, if an object is the grandchild of a container and the object's
   parent is removed, the object does NOT become a child of the grandparent.
   
   IDRAWDECORATOR_SetParent() should only be called by a container that is managing
   the draw decorator.
   
   Draw decorators must not refcount their parents.  It is the parent's responsibility
   to set the parent pointer of all child objects to NULL prior to the parent being
   deleted.

Side Effects: 
   None

See Also:
   IDrawDecorator Interface
   IContainer Interface
   IDRAWDECORATOR_GetParent()
   IDECORATOR_SetParent()
   IWIDGET_SetParent()

===========================================================================
   
IDRAWDECORATOR_Draw()

Description:
   This function renders a draw decorator at a specific position within on the device
   display, drawing onto an ICanvas object within the bounds defined by the draw
   decorator's current extent.  The function specifies the horizontal and vertical
   position of the upper left hand corner of the bounding rectangle that will enclose
   the draw decorator.
   
   IDRAWDECORATOR_Draw() starts off a chain of drawing, calling the draw handler at the
   beginning of the chain.  Each draw handler that hooks into the draw decorator is
   expected (though not required) to call DRAWHANDLERDESC_Call() to continue the chain
   and insure that all child objects are drawn.
   
Prototype:   
   void IDRAWDECORATOR_Draw(IDrawDecorator *p, ICanvas *d, int x, int y);
                        
Parameters:
===pre>
   p:  Pointer to the IDrawDecorator object to draw.
   d:  Pointer to the ICanvas object that will provide drawing services to
       the draw decorator.
   x:  Horizontal position of the left side of the draw decorator, offset from
       the left side of the ICanvas object.
   y:  Vertical position of the top of the draw decorator, offset from the top
       of the ICanvas object.
===/pre>
      
Return Value:
   none
   
Comments:
   IDRAWDECORATOR_Draw () effectively draws the child widget wrapped by the draw
   decorator.

Side Effects: 
   None

See Also:
   IDrawDecorator Interface
   ICanvas Interface
   DRAWHANDLERDESC_Call()

===========================================================================

IDRAWDECORATOR_IntersectOpaque()

Description:
   This function retrieves the intersection of a given rectangle with the largest opaque
   region of a given draw decorator.  To illustrate, consider the diagram below:
   
===pre>   
   +--------------------+
   |                    |
   |       source     -----------+ result of IDRAWDECORATOR_IntersectOpaque()
   |     rectangle   |  |
   |                 V  |
   |            +---------------------------------------+
   |            |/ / / /|                               |
   |            | / / / |    target draw decorator      |
   |            |/ / / /|                               |
   +------------|-------                                |
                |                                       |
                +---------------------------------------+
===/pre>  
   
   The function determines where the extent of the target draw decorator and the
   source rectangle overlap -- subtracting out, however, areas that lack opacity.
   So, for example, if the draw decorator itself was transparent (i.e. it has no
   children or the background of the child widget has been set to be transparent),
   the rectangle that enclosed those transparent areas would not included in the
   calculated result.
   
   If the rectangles intersect, IDRAWDECORATOR_IntersectOpaque() will return
   TRUE, otherwise the function returns FALSE and the resultant rectangle is empty.

   IDRAWDECORATOR_IntersectOpaque() inherits wholly from IDECORATOR_IntersectOpaque().
   
Prototype:
   boolean IDRAWDECORATOR_IntersectOpaque(IDrawDecorator *p, AEERect *rd, const AEERect *ri);
   
Parameters:
===pre>
   p:   Pointer to the IDrawDecorator object to be used as a target for intersection.
   rd:  Pointer to an AEERect, into which the function will return the intersection
        of the draw decorator's largest opaque rectangle and the rectangle passed to
        the function in 'ri'.
   ri:  Pointer to the AEERect that is to be intersected with the draw decorator's
        largest opaque rectangle.  This rectangle should be expressed in the target
        widget's coordinate system, with (0,0) being the top-left corner of the widget.
===/pre>
      
Return Value:
===pre>
   TRUE   -  If 'ri' intersects with the draw decorator's largest opaque rectangle.
   FALSE  -  If 'ri' does not intersect with the draw decorator largest opaque rectangle.
===/pre>
   
   The intersection of 'ri' and the draw decorator's largest opaque rectangle will be
   returned in 'rd', or returns the empty rectangle (0, 0, 0, 0) if the decorator is
   not wrapped around a widget (making the decorator, essentially, transparent).
   
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
   IDrawDecorator Interface
   IDECORATOR_IntersectOpaque()
   
===========================================================================

IDRAWDECORATOR_GetModel()

Description:
  This function retrieves the model that provides data to a given draw decorator
  implementation. The model that is returned will implement the interface for the
  class ID passed to the function in the
  'i' parameter

  IDRAWDECORATOR_GetModel() inherits wholly from IDECORATOR_GetModel().
   
Prototype:
   int IDRAWDECORATOR_GetModel(IDrawDecorator *p, AEECLSID i, IModel **ppm);
   
Parameters:
===pre>
   p:    Pointer to the IDrawDecorator object.
   i:    The class id of the model the draw decorator will use as a source for data.
   ppm:  Pointer to an IModel pointer into which the function will return a
         pointer to the IModel interface that supports the passed class id.
===/pre>
      
Return Value:
===pre>
   SUCCESS - The function was able to successfully locate the model interface
             identified by the 'i' parameter.
   EFAILED - The requested model interface is not currently declared.
===/pre>

   A pointer to the model interface associated with the draw decorator implementation
   is returned in 'ppm', or NULL if the draw decorator implementation does utilize a
   model interface.
   
Comments:
   The pointer passed in the 'ppm' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   IDrawDecorator Interface
   IModel Interface
   IDECORATOR_GetModel()

===========================================================================

IDRAWDECORATOR_SetModel()

Description:
   This function allows a draw decorator implementation to associate itself with a
   specific model interface.  To make this association, an application would first
   retrieve an IModel pointer for the desired interface class ID, then attach this
   model to the draw decorator implementation by passing the model pointer and draw
   decorator pointer to IDRAWDECORATOR_SetModel().
   
   Once the draw decorator implementation has associated itself with a given model
   interface, it should register as a listener for that model.  As a listener, the
   draw decorator implementation will receive notifications each time the model's
   data changes, thereby providing the opportunity for the draw decorator implementation
   to update its display or act in some other meaningful way to reflect the current
   data.
   
  IDRAWDECORATOR_SetModel() inherits wholly from IDECORATOR_SetModel().

Prototype:
   int IDRAWDECORATOR_SetModel(IDrawDecorator *p, IModel *pm);
   
Parameters:
===pre>
   p:  Pointer to the IDrawDecorator object.
   m:  Pointer to the model interface that the draw decorator implementation
       should use as the basis for accessing its data.
===/pre>
      
Return Value:
===pre>
   SUCCESS          - The function was able to successfully associate the draw
                      decorator with the specified model interface.
   ECLASSNOTSUPPORT - The model interface passed to the function does not support
                      a model interface class ID that is known to the draw decorator.
===/pre>
   
Comments:
   None

Side Effects: 
   None

See Also:
   IDrawDecorator Interface
   IModel Interface
   IDRAWDECORATOR_GetModel()
   IDECORATOR_SetModel()
   IMODEL_AddListener()


===========================================================================

IDRAWDECORATOR_SetModel()

Description:
   This function allows a draw decorator implementation to associate itself with a
   specific model interface.  To make this association, an application would first
   retrieve an IModel pointer for the desired interface class ID, then attach this
   model to the draw decorator implementation by passing the model pointer and draw
   decorator pointer to IDRAWDECORATOR_SetModel().
   
   Once the draw decorator implementation has associated itself with a given model
   interface, it should register as a listener for that model.  As a listener, the
   draw decorator implementation will receive notifications each time the model's
   data changes, thereby providing the opportunity for the draw decorator implementation
   to update its display or act in some other meaningful way to reflect the current
   data.
   
  IDRAWDECORATOR_SetModel() inherits wholly from IDECORATOR_SetModel().

Prototype:
   int IDRAWDECORATOR_SetModel(IDrawDecorator *p, IModel *pm);
   
Parameters:
===pre>
   p:  Pointer to the IDrawDecorator object.
   m:  Pointer to the model interface that the draw decorator implementation
       should use as the basis for accessing its data.
===/pre>
      
Return Value:
===pre>
   SUCCESS          - The function was able to successfully associate the draw
                      decorator with the specified model interface.
   ECLASSNOTSUPPORT - The model interface passed to the function does not support
                      a model interface class ID that is known to the draw decorator.
===/pre>
   
Comments:
   None

Side Effects: 
   None

See Also:
   IDrawDecorator Interface
   IModel Interface
   IDRAWDECORATOR_GetModel()
   IDECORATOR_SetModel()
   IMODEL_AddListener()

=======================================================================

IDRAWDECORATOR_SetDraw()

Description:
   This function allows an application to hook its own custom draw handler into the
   drawing chain for a draw decorator.  By default, a when asked to draw itself, a
   draw decorator will draw the object it wraps.  Usually this object is another
   container managing one or more child objects, each of which will draw be drawn as
   the effective result of drawing the draw decorator.  In this default state, the
   draw decorator really does not "draw" anything at all!!  However, an application
   wishing to provide a customized appearance to an object may wrap that object in
   a draw decorator and provide its own customized drawing routine to decorate that
   object by calling IDRAWDECORATOR_SetDraw() and providing a PFNDRAWHANDLER in a
   DrawHandlerDesc data structure to that draw decorator.  The customized draw handler
   will then be called instead of the normal default drawing procedure allowing the
   application to perform whatever rendering magic is appropriate.  For example, an
   application could choose to install its own custom draw procedure that renders all
   of its child widgets into an offscreen bitmap, then transformed that bitmap and
   finally blitted the image onto the display to create the effect of looking at the
   display from the inside out (neato!).
   
   The custom draw handler is stored in a DrawHandlerDesc data structure as a
   PFNDRAWHANDLER, along with a pointer to a block of memory allocate and owned by the
   application.  This pointer will be passed into the draw handler each time the draw
   decorator is asked to draw itself, providing access to private application storage.
   The DrawHandlerDesc data structure also contains a PFNFREEHANDLER that will be
   called when the draw decorator is released so that the decorator may free the private
   memory allocated and passed to the event handler.
   
Prototype:
   void IDRAWDECORATOR_SetDraw(IDrawDecorator *p, DrawHandlerDesc *pd);

Parameters:
===pre>   
   p:   A pointer to an IDrawDecorator object.
   pd:  Pointer to the data structure that contains the custom draw handler,
        private memory pointer, and the callback used to free the private memory.
===/pre>   
   
Return Value:
   None

Comments:
   Passing NULL for the DrawHandlerDesc will reset the draw decorator's draw handler to
   use the draw decorator's default draw handler -- without private memory or the need to
   free privately allocated storage (but capable of only drawing the wrapped object without
   any decoration).
   
   The draw handler installed by a draw decorator to perform custom drawing should itself
   remember to call DRAWHANDLERDESC_Call() so that the drawing chain will continue -- most
   notably, so that the object wrapped by the draw decorator will be drawn.  The draw handler
   may call DRAWHANDLERDESC_Call() at any point -- either before it performs its own drawing,
   after, or someplace in the middle.
   
   An application that hooks into both a draw decorator's draw handler and event handler (by
   calling IDRAWDECORATOR_SetHandler()), should exercise care in managing the private data
   released by the PFNFREEHANDLER stored in the DrawHandlerDesc data structure.  Draw handlers
   and event handlers both manage data structures that contain pointers to private storage,
   as well as a pointer to a function that will be used to release this storage -- which
   could be different... or could be the same!  Each of these PFNFREEHANDLER routines will
   be called when the draw decorator is released.  Therefore, an application that uses the
   same block of private storage for both its custom event and draw handling procedures
   should be careful to check the validity of the storage to be freed before actually
   attempting to release the memory.
   
Side Effects: 
   None
   
See Also:
   IDrawDecorator Interface
   PFNHANDLER
   PFNFREEHANDLER
   DrawHandlerDesc
   DRAWHANDLERDESC_Call()
   IDRAWDECORATOR_SetHandler()
   
===========================================================================

IDRAWDECORATOR_TO_IDECORATOR()

Description:
   This function provides type safe casting of an IDrawDecorator pointer to an
   IDecorator pointer and should be used when passing an IDrawDecorator pointer to
   functions that require a pointer to an IDecorator.  This is safer than a simple
   cast, since the compiler will verify the pointer type.

Prototype:
   IDecorator *IDRAWDECORATOR_TO_IDECORATOR(IDrawDecorator *p)

Parameters:
===pre>
   p:  Pointer to an IDrawDecorator interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IDecorator pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IDrawDecorator Interface
   IDecorator Interface

===========================================================================

IDRAWDECORATOR_TO_IWIDGET()

Description:
   This function provides type safe casting of an IDrawDecorator pointer to an
   IWidget pointer and should be used when passing an IDrawDecorator pointer to
   functions that require a pointer to an IWidget.  This is safer than a simple
   cast, since the compiler will verify the pointer type.

Prototype:
   IWidget *IDRAWDECORATOR_TO_IWIDGET(IDrawDecorator *p)

Parameters:
===pre>
   p:  Pointer to an IDrawDecorator interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IWidget pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IDrawDecorator Interface
   IWidget Interface

*/


#endif /* __AEEDRAWDECORATORWIDGET_H__ */
