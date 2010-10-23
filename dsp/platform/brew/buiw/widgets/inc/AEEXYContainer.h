/*
  ========================================================================

  FILE:  AEEXYContainer.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Defines a container that positions its widgets
  using absolute coordinates

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEXYCONTAINER_H__
#define __AEEXYCONTAINER_H__

#include "AEEContainer.h"

#include "bid\AEEIID_XYCONTAINER.bid"

#include "bid\AEECLSID_XYCONTAINER.bid"

/////////////////////////////////////////////////////////////////////////////
// IXYContainer interface


typedef struct WidgetPos WidgetPos;
struct WidgetPos {
   int      x;          // x position in container relative coordinates
   int      y;          // y position in container relative coordinates
   boolean  bVisible;   // widget visibility flag
};

typedef struct IXYContainer IXYContainer;
#define INHERIT_IXYContainer(iname) \
   INHERIT_IContainer(iname);\
   int      (*SetPos)      (iname *, IWidget *pw, IWidget *piwBefore, const WidgetPos *ppos);\
   int      (*GetPos)      (iname *, IWidget *pw, WidgetPos *ppos)
 

AEEINTERFACE(IXYContainer) {
   INHERIT_IXYContainer(IXYContainer);
};


#define IXYCONTAINER_AddRef(p)              AEEGETPVTBL(p,IXYContainer)->AddRef(p)
#define IXYCONTAINER_Release(p)             AEEGETPVTBL(p,IXYContainer)->Release(p)
#define IXYCONTAINER_QueryInterface(p,i,p2) AEEGETPVTBL(p,IXYContainer)->QueryInterface(p,i,p2)
#define IXYCONTAINER_Invalidate(p,pw,prc,f) AEEGETPVTBL(p,IXYContainer)->Invalidate(p,pw,prc,f)
#define IXYCONTAINER_Locate(p,pw,ppr,prc)   AEEGETPVTBL(p,IXYContainer)->Locate(p,pw,ppr,prc)
#define IXYCONTAINER_Remove(p,pw)           AEEGETPVTBL(p,IXYContainer)->Remove(p,pw)         
#define IXYCONTAINER_SetPos(p,pw,pb,ppos)   AEEGETPVTBL(p,IXYContainer)->SetPos(p,pw,pb,ppos) 
#define IXYCONTAINER_GetPos(p,pw,ppos)      AEEGETPVTBL(p,IXYContainer)->GetPos(p,pw,ppos)
#define IXYCONTAINER_GetWidget(p,pw,d,w)    AEEGETPVTBL(p,IXYContainer)->GetWidget(p,pw,d,w) 
static __inline int IXYCONTAINER_Insert(IXYContainer *po, IWidget *pw, IWidget *pb, const WidgetPos *ppos) {
   return AEEGETPVTBL(po,IXYContainer)->Insert(po,pw,pb,(const void *)ppos);
}

#define IXYCONTAINER_GetFirstWidget(p)      AEEGETPVTBL(p,IXYContainer)->GetWidget(p,NULL,1,0)
#define IXYCONTAINER_GetLastWidget(p)       AEEGETPVTBL(p,IXYContainer)->GetWidget(p,NULL,0,0)

static __inline IContainer *IXYCONTAINER_TO_ICONTAINER(IXYContainer *me) {
   return (IContainer *)(void*)me;
}


/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
XY Container Interface and Class IDs

Description:
   Each class and interface within the widget framework is identified by a unique
   ID.  These IDs are used throughout the framework to distinguish between various
   types of objects, such as containers and widgets.

   The XY container supports both an interface and the creation of class object
   instances.  The XY container interface or a new instance of the XY container
   class object may be created by calling ISHELL_CreateInstance() and passing in
   the desired class id of the object.  The object that is created is, itself, a
   container and may utilize those APIs defined for either the container or XY
   container interfaces.

   The XY container recognizes the following set of class and interface IDs.

Definition:

Identifier             Definition
----------             -----------------------------------
AEECLSID_XYCONTAINER:  Identifies the class ID of the XY container
AEEIID_XYCONTAINER:    Identifies the interface ID of the XY container

Comments:
   None

See Also:
   IXYContainer Interface
   ISHELL_CreateInstance()
   
=============================================================================

WidgetPos

Description:
   This data type defines special attributes for any widget that is to be managed by
   an XY container.  These attributes identify whether or not the widget should be
   factored into the container's layout calculations and -- if it should -- where the
   widget will be placed in the container.  Widgets in an XY container are laid out
   relative to the coordinates of the enclosing container.
   
Definition:
   typedef struct {
      int      x;
      int      y;
      boolean  bVisible;
   } WidgetPos;

Members:
===pre>
   x:         Specifies the horizontal layout position of the widget within the XY
              container.  This value is expressed in coordinates relative to the
              container, with location (0,0) being the origin of the XY container.
   y:         Specifies the vertical layout position of the widget within the XY
              container.  This value is expressed in coordinates relative to the
              container, with location (0,0) being the origin of the XY container.
   bVisible:  Boolean value specifying whether or not this widget should be displayed.
===/pre>
Comments:
   None

See Also:
   IXYCONTAINER_SetPos()
   IXYCONTAINER_GetPos()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IXYContainer Interface

Description:
   The XY container is derived from the base container object and is used to manage the
   widgets within a container, where those widgets are positioned within the container
   at absolute (x, y) locations.  The coordinates of each object managed by an XY container
   are expressed relative to the enclosing container.  Therefore, a widget placed at (2, 5)
   within an XY container would be placed 2 pixels from the container's left edge, and 5
   pixels from the top.  Objects are positioned by specifying their point of origin, with
   the object extent entirely under the control of the object itself (unlike objects
   placed in card, prop or constraint containers, where the container grants an extent to
   each object).
   
   Objects within an XY container may overlap one another with the widgets at the top of
   the widget stack being those that appear to lay "on top" of lower objects. 

Supported Events:
   The XY container does not supply its own event handler, deferring all event handling to
   the base container from which it derives.  As such, all events will first be passed to
   the border event handler where border property events may be handled.  The base container
   will then attempt to process events that set or move the focus, and pass all other events
   to the widget within the container that currently holds the focus.
   
Comments:
   None
   
See Also:
   None
   
===========================================================================

IXYCONTAINER_AddRef()

Description:

   This function increments the reference count of the IXYContainer interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IXYCONTAINER_AddRef(IXYContainer *p);

Parameters:
===pre>
   p:  Pointer to the IXYContainer interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IXYContainer interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IXYCONTAINER_Release()
   
=======================================================================

IXYCONTAINER_Release()

Description:

   This function decrements the reference count of the IXYContainer interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IXYCONTAINER_Release(IXYContainer *p);

Parameters:
===pre>
   p:  Pointer to the IXYContainer interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IXYContainer interface object.  If the
   returned reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IXYCONTAINER_AddRef()

=======================================================================

IXYCONTAINER_QueryInterface()

Description: 
   This function queries an interface for an alternate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IXYCONTAINER_QueryInterface(IXYContainer *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IXYContainer interface object.
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

IXYCONTAINER_Invalidate()

Description: 
   This function will invalidate all or part of a widget within an XY container.
   The application identifies the container and widget, and may also specify a rectangle
   within that widget in order to invalidate a portion of the widget.  By default, the
   entire extent of the widget will be invalidated.

Prototype:

   void IXYCONTAINER_Invalidate(IXYContainer *p, IWidget *pw, const AEERect *prc, uint32 f);

Parameters:
===pre>
   p:    Pointer to the IXYContainer interface object that serves as the parent to the widget
         to invalidate.
   pw:   Pointer to the IWidget object to be invalidated, or a constant that implies a particular
         widget within the XY container.  The constants used to identify a relative position
         for the widget to be invalidated, are defined as follows:
                              
             WIDGET_ZTOPMOST     -  Requests that the widget at the top of the container's
                                    widget stack should be invalidated.
             WIDGET_ZBOTTOMMOST  -  Requests that the widget at the bottom of the container's
                                    widget stack should be invalidated.
             WIDGET_ZNORMAL      -  Requests that the widget placed at the container's most
                                    normal layout location be invalidated.  The XY container
                                    treats the WIDGET_ZTOPMOST position as the "normal" location.
   
   prc:  Pointer to the rectangle that defines the area to be invalidated.  This rectangle is
         expressed relative to the coordinates of the specified widget.  When this parameter
         is NULL, the interface will use the full extent of the 'pw' widget as the invalidation
         rectangle.
   f:    A set of flags that control the the calculation of the invalidation rectangle.
         The defined validation flags are:
             ICIF_EXTENT  -  When this flag is ON, it is an indication that the widget has
                             changed its extent.
===/pre>

Return Value:
   None

Comments:
   Calling IXYCONTAINER_Invalidate() will trigger the invalidation to travel up through the
   container's hierarchy, first to its parent, then to its parent, etc. so that the
   invalidation rectangle may be applied to each object in the chain.
   
   Callers should set the ICIF_EXTENT flag of the 'f' parameter when calling the function
   following a change to the widget's extent.  

Side Effects: 
   None

See Also:
   IXYContainer Interface
   IWidget Interface
   ICONTAINER_Invalidate()
   IWIDGET_Invalidate()

=======================================================================

IXYCONTAINER_Locate()

Description: 
   This function will find the location of a particular widget, returning that widget's
   rectangle relative to the widget's root container.  The function will also, optionally,
   return a pointer to the root container.

Prototype:

   int IXYCONTAINER_Locate(IXYContainer *p, IWidget *pw, IContainer **ppr, AEERect *prc)

Parameters:
===pre>
   p:    Pointer to the IXYContainer interface object that serves as the parent to the widget
         to locate.
   pw:   Pointer to the IWidget object to be located.
   ppr:  Pointer to an IContainer pointer, into which the function will return a pointer to the
         container's root container.  This parameter may be specified as NULL if the caller does not
         wish to get the root container.
   prc:  Pointer to an AEERect.  On input, this rectangle will specify the current relative
         offset of the 'pw' object relative to its child.  On output, this rectangle specifies
         the layout position of the object relative to the root container.  Therefore, the
         function should initially be called by passing in a rectangle with origin at (0, 0) --
         otherwise, the returned rectangle will be expressed relative to the initial offset.
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget 'pw' was found and the information requested was successfully returned
   EFAILED - If the widget 'pw' was not found in the container.
===/pre>
   
   The rectangle occupied by the widget is returned in 'prc'.

   A pointer to the widget's root container will be returned in 'ppr', which will increment the
   reference count of the root container.

Comments:
   The rectangle referenced by 'prc' MUST be initialized prior to calling IXYCONTAINER_Locate(),
   otherwise the results will be undefined.

   The pointer passed in the 'ppr' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   IXYContainer Interface
   IWidget Interface
   ICONTAINER_Locate()


=======================================================================

IXYCONTAINER_Insert()

Description:
   This function allows an application to insert a widget into the XY container's widget
   stack -- at the top of the stack, at the bottom, or some place inbetween.  By default,
   widgets will be inserted at the top of the stack.  However, an application may choose
   to instead insert widgets anywhere within the container's widget stack.  For example,
   consider an application that periodically wishes to flash subliminal messages to the
   user behind the current focus of the user's attention.  Such an application might want
   to create an image widget, then insert this widget directly behind the frontmost widget,
   displaying a quick image of Happy Couples sitting at a bar drinking frosty beverages
   (just for an instant) immediately behind the widget in the foreground, then remove that
   widget from the display before the user even knows what hit them (call your local broker
   now and start loading up on frosty beverage stocks).  Where the widget is inserted in
   the XY container's widget stack depends on the value passed in the 'pb' parameter.
   
Prototype:
   int IXYCONTAINER_Insert(IXYContainer *p, IWidget *pw, IWidget *pb, const WidgetPos *ppos);

Parameters:
===pre>   
   p:     A pointer to an IXYContainer object.
   pw:    Pointer to the widget to be inserted into the XY container's widget stack.
   pb:    Indicates where in the XY container's widget stack that the new widget
          should be inserted.  This value may be expressed as either a pointer to a 
          widget already in the widget stack, in which case the new widget will be
          inserted immediately before the widget pointed to by 'pb' -- or as  a
          constant that identifies a relative position within the widget stack, as
          follows:
              WIDGET_ZNORMAL     -  Allow BREW to place the inserted widget at the
                                    default location.  For an XY container, this
                                    places the new widget at the top of container's
                                    widget stack.
              WIDGET_ZTOPMOST    -  The new widget should be inserted as the topmost
                                    widget in the container's widget stack.
              WIDGET_ZBOTTOMMOST -  The new widget should be inserted at the bottommost
                                    widget in the container's widget stack.
   ppos:  Pointer to a WidgetPos data structure, specifying the location where the
          widget should be displayed within the container, as well as its visibility attribute
          for this widget.  If this parameter is set to NULL, pw will be inserted at (0,0), and
          visibility will be set to TRUE.  
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully inserted into the XY container's widget stack.
   EBADPARM  --  The widget pointer passed in 'pb' either does not exist, or refers to a widget
                 that is not in the XY container's widget stack.
   ENOMEMORY --  The widget stack could not grow to accomodate a new widget.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
   IXYContainer Interface
   IWidget Interface
   WidgetPos
   IXYCONTAINER_Remove()

===========================================================================

IXYCONTAINER_Remove()

Description:
   This function allows an application to remove a widget from the XY container's widget
   stack.  The widget to be removed may be specified explicitly by its widget pointer, or
   implicitly by a constant that identifies the widget's relative position within the
   container's widget stack.
   
Prototype:
   int IXYCONTAINER_Remove(IXYContainer *p, IWidget *pw);

Parameters:
===pre>   
   p:   A pointer to an IXYContainer object.
   pw:  Identifies the widget that should be removed from the XY container's widget stack.
        This widget may be expressed explicitly as a widget pointer, or implicitly as  a
        constant that identifies a relative position within the widget stack, as follows:
             WIDGET_ZNORMAL      - Remove the default widget, which BREW interprets to be
                                   the widget at the top of the widget stack.
             WIDGET_ZTOPMOST     - Remove the widget at the top of the widget stack.
             WIDGET_ZBOTTOMMOST  - Remove the widget at the bottom of the widget stack.

===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully removed from the XY container's widget stack.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a widget
                 that is not in the XY container's widget stack.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    IXYContainer Interface
    IWidget Interface
    IXYCONTAINER_Insert()

===========================================================================

IXYCONTAINER_SetPos()

Description:
   This function will change the layout position of a given widget within the XY container,
   moving this widget to a new (x, y) location.  The function may also, optionally, relocate
   that widget to a different spot in the XY container's widget stack -- effectively changing
   the order of the stack.  The new layout position and visibility attributes are passed to
   the function in a WidgetPos structure, pointed to by the 'ppos' parameter.  The widget to
   receive the new attributes may be specified explicitly by its widget pointer, or
   implicitly by a constant that identifies the widget's relative position within the XY
   container's widget stack.  The 'pb' parameter controls whether or not the widget's order
   within the stack will move.  Passing the constant WIDGET_ZNORMAL leaves the widget where
   it currently stands, without altering the order of the stack.  Passing WIDGET_ZTOPMOST or
   WIDGET_ZBOTTOMMOST will move the widget to the top or bottom of the stack, while passing
   an explicit widget pointer will move the target widget to the stack location before the
   widget passed in 'pb'.
   
Prototype:
   int IXYCONTAINER_SetPos(IXYContainer *p, IWidget *pw, IWidget *pb, const WidgetPos *ppos);

Parameters:
===pre>   
   p:     A pointer to an IXYContainer object.
   pw:    Identifies the widget in the XY container's widget stack that will be the
          target of the layout position change and (optionally) stack order relocation.
          This widget may be expressed explicitly as a widget pointer, or implicitly as
          a constant that identifies a relative position within the widget stack, as
          follows:
               WIDGET_ZNORMAL      - The target widget is the default widget, which BREW
                                     interprets to be the widget at the top of the widget
                                     stack.
               WIDGET_ZTOPMOST     - The target widget is the widget at the top of the
                                     widget stack.
               WIDGET_ZBOTTOMMOST  - The target widget is the widget at the bottom of the
                                     widget stack.
   pb:     Identifies a widget in the XY container's widget stack that will serve as
           the relocation spot for the target widget.  When this parameter is passed as an
           explicit pointer to a widget in the container's widget stack, the target widget
           will be moved to the stack location that is _before_ this widget.  The new
           location for the target widget may be passed implicitly as a relative location
           in the stack by passing any of the following constants:
               WIDGET_ZNORMAL      - Specifies that the target widget should NOT be moved
                                     and should instead remain at its current location in
                                     the widget stack.
               WIDGET_ZTOPMOST     - The target widget should be moved to the top of the
                                     widget stack.
               WIDGET_ZBOTTOMMOST  - The target widget should be moved to the bottom of
                                     the widget stack.
   
   ppos:   Pointer to a WidgetPos data structure, containing the new (x, y) location for
           the target widget, as well as its visibility attribute.

===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The layout position of the widget was successfully changed.
   EBADPARM  --  The widget pointers passed in 'pw' and/or 'pb' either do not exist, or
                 refer to widgets that are not in the XY container's widget stack.
===/pre>   

Comments:
   If IXYCONTAINER_SetPos() is being called to change the location of the widget within the
   XY container's widget stack without changing the layout position of the widget, the caller
   MUST still pass in a pointer to the correct layout position in the 'ppos' parameter.  In
   such cases, the application should first call IXYCONTAINER_GetPos() to retrieve the current
   layout position, then call IXYCONTAINER_SetPos() passing back the results received from
   IXYCONTAINER_GetPos().

Side Effects: 
   None
   
See Also:
    IXYContainer Interface
    IWidget Interface
    WidgetPos
    IXYCONTAINER_GetPos()

===========================================================================

IXYCONTAINER_GetPos()

Description:
   This function retrieves a widget's layout position within the XY container, returning a
   copy of the layout position and visibility attribute in the WidgetPos data structure
   pointed to by 'ppos'.
   
Prototype:
   int IXYCONTAINER_GetPos(IXYContainer *p, IWidget *pw, WidgetPos *ppos);

Parameters:
===pre>   
   p:     A pointer to an IXYContainer object.
   pw:    Identifies the widget in the XY container's widget stack whose layout
          position should be retrieved.  This widget may be expressed explicitly as a
          widget pointer, or implicitly as  a constant that identifies a relative
          position within the widget stack, as follows:
               WIDGET_ZNORMAL      - Retrieve the layout position for the default
                                     widget, which BREW interprets to be the widget at
                                     the top of the widget stack.
               WIDGET_ZTOPMOST     - Retrieve the layout position for the widget at the
                                     top of the  widget stack.
               WIDGET_ZBOTTOMMOST  - Retrieve the layout position for the widget at the
                                     bottom of the  widget stack.
   ppos:   Pointer to a WidgetPos data structure, into which the function will copy the
           current layout position of the widget.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The layout position of the widget was successfully retrieved.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a
                 widget that is not in the XY container's widget stack.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    IXYContainer Interface
    IWidget Interface
    WidgetPos
    IXYCONTAINER_SetPos()

===========================================================================

IXYCONTAINER_GetWidget()

Description:
   This function provides facilities that allow an application to retrieve the widgets stored
   in an XY container's widget stack.  The function may be instructed to return the widget from
   an explicit location, such as the top or bottom of the stack, or from some position relative
   to another widget in the widget stack -- for example, returning the widget that appears before
   or after another widget in the stack.  An application can easily iterate over all of the widgets
   in the XY container by repeatedly calling IXYCONTAINER_GetWidget() and passing in the widget
   pointer returned from the previous call.
   
Prototype:
   IWidget *IXYCONTAINER_GetWidget(IXYContainer *p, IWidget *pw, boolean d, boolean w);

Parameters:
===pre>   
   p:   A pointer to an IXYContainer object.
   pw:  Identifies the widget within the XY container's widget stack that will serve as a
        reference point for the widget to be retrieved.  This widget may be expressed explicitly
        as a widget pointer, or implicitly as a constant that identifies a relative position
        within the widget stack, as follows:
            WIDGET_ZNORMAL      - Use the widget at the XY container's default location as the
                                  retrieval reference point.  BREW interprets this to be the
                                  widget at the top of the widget stack.
            WIDGET_ZTOPMOST     - Use the widget at the top of the widget stack as the retrieval
                                  reference point.
            WIDGET_ZBOTTOMMOST  - Use the widget at the bottom of the widget stack as the retrieval
                                  reference point.
        When 'pw' is NULL, the function will return either the top or bottom widget on the stack,
        depending on the value of 'd'.  When 'pw' is NULL and 'd' is TRUE, IXYCONTAINER_GetWidget()
        will return the bottommost widget.  When 'pw' is NULL and 'd' is FALSE, the function will
        return the topmost widget.  
   d:   Specifies whether the function should retrieve the next widget in the widget stack, or the
        previous widget on the stack -- always relative to the reference widget determined by the
        value passed in the 'pw' parameter.  When 'd' is TRUE the function will retrieve the next
        widget on the stack (i.e. the widget closer to the top of the stack).  When 'd' is FALSE
        the function will retrieve the previous widget on the stack (i.e. the widget closer to the
        bottom of the stack).  Note, however, that when 'pw' is passed as NULL, the 'd' parameter
        explicitly indicates either the topmost or bottommost widget, as described above.
   w:   Specifies whether or not the function should wrap when attempting to retrieve the next or
        previous widget from the top or bottom of the XY container's widget stack.  When 'w' is
        TRUE, the function will wrap at either end of the stack, otherwise the function will return
        NULL indicating that there are no more widgets to be retrieved.
===/pre>   
   
Return Value:
   A pointer to a widget in the XY container's widget stack, or NULL when there is no widget to
   be returned. 

Comments:
   Though IXYCONTAINER_GetWidget() function returns a pointer to a widget, the function does not
   add a reference to the returned IWidget for the caller.

   An application may iterate over the entire widget stack -- from bottom to top -- with the
   following code:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = IXYCONTAINER_GetWidget (pXYContainer, pWidget, TRUE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once IXYCONTAINER_GetWidget() returns NULL
===/pre>   
   
   To iterate over all the widgets on the widget stack from top to bottom, the application's
   code would look like the following:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = IXYCONTAINER_GetWidget (pXYContainer, pWidget, FALSE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once IXYCONTAINER_GetWidget() returns NULL
===/pre>   

Side Effects: 
  None

See Also:
   IXYContainer Interface
   IWidget Interface

===========================================================================

IXYCONTAINER_GetFirstWidget()

Description:
   This function retrieves a pointer to the first widget in the XY container's widget stack.
   This is, effectively, the widget at the top of the widget stack.  The widget on the top 
   of the stack is the first one drawn onscreen and is the bottommost with regard to
   front-to-back ordering within the container.
   
Prototype:
   IWidget *IXYCONTAINER_GetFirstWidget(IXYContainer *p);

Parameters:
===pre>   
   p:   A pointer to an IXYContainer object.
===/pre>   
   
Return Value:
   A pointer to the widget at the top of the XY container's widget stack, or NULL when the
   widget stack is empty. 

Comments:
   Though IXYCONTAINER_GetFirstWidget() function returns a pointer to a widget, the function
   does not add a reference to the returned IWidget for the caller.

Side Effects: 
  None

See Also:
   IXYContainer Interface
   IWidget Interface
   IXYCONTAINER_GetWidget()
   IXYCONTAINER_GetLastWidget()

===========================================================================

IXYCONTAINER_GetLastWidget()

Description:
   This function retrieves a pointer to the last widget in the XY container's widget stack.
   This is, effectively, the widget at the bottom of the widget stack.  The widget on the 
   bottom of the stack is the last one drawn onscreen and is the topmost with regard to 
   front-to-back ordering within the container.
   
Prototype:
   IWidget *IXYCONTAINER_GetLastWidget(IXYContainer *p);

Parameters:
===pre>   
   p:   A pointer to an IXYContainer object.
===/pre>   
   
Return Value:
   A pointer to the widget at the bottom of the XY container's widget stack, or NULL when the
   widget stack is empty. 

Comments:
   Though IXYCONTAINER_GetLastWidget() function returns a pointer to a widget, the function
   does not add a reference to the returned IWidget for the caller.

Side Effects: 
  None

See Also:
   IXYContainer Interface
   IWidget Interface
   IXYCONTAINER_GetWidget()
   IXYCONTAINER_GetFirstWidget()

===========================================================================

IXYCONTAINER_TO_ICONTAINER()

Description:
   This function provides type safe casting of an IXYContainer pointer to an IContainer
   pointer and should be used when passing an IXYContainer pointer to functions that
   require a pointer to an IContainer.  This is safer than a simple cast, since the compiler
   will verify the pointer type.

Prototype:
   IContainer *IXYCONTAINER_TO_ICONTAINER(IXYContainer *me)

Parameters:
===pre>
   me:  Pointer to an IXYContainer interface object.
===/pre>

Return Value:
   Returns 'me' cast to an IContainer pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IContainer Interface
   IXYContainer Interface

*/
#endif /* __AEEXYCONTAINER_H__ */
