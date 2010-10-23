/*
  ========================================================================

  FILE:  AEERootContainer.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I define the rootcontainer. Only one of me
  should ever be instantiated for a particular area of the display
  since I am the root of the drawning order

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEROOTCONTAINER_H__
#define __AEEROOTCONTAINER_H__

#include "AEEXYContainer.h"

#include "bid\AEEIID_ROOTCONTAINER.bid"
#include "bid\AEECLSID_ROOTCONTAINER.bid"

/////////////////////////////////////////////////////////////////////////////
// IRootContainer interface

typedef struct IRootContainer IRootContainer;
#define INHERIT_IRootContainer(iname) \
   INHERIT_IXYContainer(iname);\
   void      (*SetCanvas) (iname *, ICanvas *piCanvas, const AEERect *prc);\
   int       (*GetCanvas) (iname *, ICanvas **ppiCanvas, AEERect *prc)


AEEINTERFACE(IRootContainer) {
   INHERIT_IRootContainer(IRootContainer);
};

#define IROOTCONTAINER_AddRef(p)               AEEGETPVTBL(p,IRootContainer)->AddRef(p)
#define IROOTCONTAINER_Release(p)              AEEGETPVTBL(p,IRootContainer)->Release(p)
#define IROOTCONTAINER_QueryInterface(p,i,p2)  AEEGETPVTBL(p,IRootContainer)->QueryInterface(p,i,p2)
#define IROOTCONTAINER_Invalidate(p,pw,prc,f)  AEEGETPVTBL(p,IRootContainer)->Invalidate(p,pw,prc,f)
#define IROOTCONTAINER_Locate(p,pw,ppr,prc)    AEEGETPVTBL(p,IRootContainer)->Locate(p,pw,ppr,prc)
#define IROOTCONTAINER_Remove(p,pw)            AEEGETPVTBL(p,IRootContainer)->Remove(p,pw)         
#define IROOTCONTAINER_SetPos(p,pw,pb,ppos)    AEEGETPVTBL(p,IRootContainer)->SetPos(p,pw,pb,ppos) 
#define IROOTCONTAINER_GetPos(p,pw,ppos)       AEEGETPVTBL(p,IRootContainer)->GetPos(p,pw,ppos)
#define IROOTCONTAINER_GetWidget(p,pw,d,w)     AEEGETPVTBL(p,IRootContainer)->GetWidget(p,pw,d,w) 
#define IROOTCONTAINER_SetCanvas(p,pic,prc)    AEEGETPVTBL(p,IRootContainer)->SetCanvas(p,pic,prc)
static __inline int IROOTCONTAINER_Insert(IRootContainer *po, IWidget *pw, IWidget *pb, WidgetPos *ppos) {
   return IXYCONTAINER_Insert((IXYContainer *)(void*)po,pw,pb,ppos);
}

#define IROOTCONTAINER_GetFirstWidget(p)        AEEGETPVTBL(p,IRootContainer)->GetWidget(p,NULL,1,0)
#define IROOTCONTAINER_GetLastWidget(p)         AEEGETPVTBL(p,IRootContainer)->GetWidget(p,NULL,0,0)

static __inline IContainer *IROOTCONTAINER_TO_ICONTAINER(IRootContainer *me) {
   return (IContainer *)(void*)me;
}

static __inline IXYContainer *IROOTCONTAINER_TO_IXYCONTAINER(IRootContainer *p) {
   return (IXYContainer*)(void*) p;
}

// Introduced in BUIT Version 1.2
#define IROOTCONTAINER_GetCanvas(p,ppi,prc)    AEEGETPVTBL(p,IRootContainer)->GetCanvas(p,ppi,prc)



/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Root Container Interface and Class IDs

Description:
   Each class and interface within the widget framework is identified by a unique
   ID.  These IDs are used throughout the framework to distinguish between various
   types of objects, such as containers and widgets.

   The root container supports both an interface and the creation of class object
   instances.  The root container interface or a new instance of the root container
   class object may be created by calling ISHELL_CreateInstance() and passing in the
   desired class id of the object.  The object that is created is, itself, a
   container and may utilize those APIs defined for either the container or root
   container interfaces.

   The root container recognizes the following set of class and interface IDs.

Definition:

Identifier               Definition
----------               -----------------------------------
AEECLSID_ROOTCONTAINER:  Identifies the class ID of the root container
AEEIID_ROOTCONTAINER:    Identifies the interface ID of the root container
   
Comments:
   None

See Also:
   IRootContainer Interface
   ISHELL_CreateInstance()
   
===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IRootContainer Interface

Description:
   The root container is derived from the XY container object and is the highest level
   container object defined for a single display.  Each display will have, at its base,
   one and only one root container, into which all other objects (including containers)
   will be placed.  The root container is laid out in absolute (x, y) coordinates,
   occupying the entire extent of the display.  It is at the base of all other drawing.
   To illustrate the relationship of the root container to other containers and objects
   placed on a display, consider the following illustration:
   
===pre>
   +---------------------------------------------+
   | Root Container                              |
   | +-----------------------------------------+ |
   | | Constraint Container                    | |
   | | +-------------------------------------+ | |  + Root Container
   | | |            Static Widget            | | |    ----+---------
   | | +-------------------------------------+ | |        |
   | |                                         | |        + Constraint Container
   | |                                         | |          ----+---------------
   | |                                         | |              |
   | |                                         | |              + Static Widget
   | |                                         | |              |
   | |                                         | |              + Softkey Widget
   | |                                         | |              |
   | |                                         | |              + Softkey Widget
   | |                                         | |
   | |                                         | |
   | |                                         | |
   | |                                         | |
   | | +-------------------------------------+ | |
   | | |  Softkey Widget  |  Softkey Widget  | | |
   | | +-------------------------------------+ | |
   | +-----------------------------------------+ | <------ Same extent as the display
   +---------------------------------------------+
===/pre>

   Here, the root container is the grand daddy of all objects.  It is king, ruler, and
   omnipotent despot.  It is at the base of every other object to be shown on the display.
   Within the root container, a constraint container has been created which is responsible
   for managing the layout of a static widget and two softkeys.  The object hierarchy is
   illustrated to the right of the diagram, with the root container at the base, and all
   other objects being "contained" within this root container.
   
   As the ultimate background for everything else appearing on a display, the root container
   is created with a white background, though this color may be changed by setting the
   PROP_BGCOLOR for the container.

Supported Events:
   The root container processes events in two passes.  The room container will first attempt to
   process property events involving the containers background color.  All other events will be
   passed to the base container event handler where border, focus and widget events will all be
   processed.
   
   The root container will accept the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The root container responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The root container
                      allows the following properties to be set:

                          PROP_BGCOLOR  --  Sets the background color that will be used to
                                            render the background of the container.

                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The root container responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  The root container allows
                      the following properties to be retrieved, returning the value of the
                      property in the pointer pointed to by 'dwParam':

                          PROP_BGCOLOR  --  Gets the background color that will be used to
                                            render the background of the container.

                      These properties are discussed below in greater detail.
===/pre>   

Properties:
Property       Description
--------       ------------------------------------------------------------
PROP_BGCOLOR:  This property contains the color the root container will use to draw
               its background. 
               
                   Property Value:  RGBVAL
                    
Required Model:
   None 
   
Model Data:
   None

Comments:
   None
   
See Also:
   IContainer Interface
   ICardContainer Interface
   IConstraintContainer Interface
   IPropContainer Interface
   IXYContainer Interface
    
===========================================================================

IROOTCONTAINER_AddRef()

Description:

   This function increments the reference count of the IRootContainer interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IROOTCONTAINER_AddRef(IRootContainer *p);

Parameters:
===pre>
   p:  Pointer to the IRootContainer interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IRootContainer interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IROOTCONTAINER_Release()
   
=======================================================================

IROOTCONTAINER_Release()

Description:

   This function decrements the reference count of the IRootContainer interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IROOTCONTAINER_Release(IRootContainer *p);

Parameters:
===pre>
   p:  Pointer to the IRootContainer interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IRootContainer interface object.  If the
   returned reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IROOTCONTAINER_AddRef()

=======================================================================

IROOTCONTAINER_QueryInterface()

Description: 
   This function queries an interface for an alternate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IROOTCONTAINER_QueryInterface(IRootContainer *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IRootContainer interface object.
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

IROOTCONTAINER_Invalidate()

Description: 
   This function will invalidate all or part of a widget within a root container.
   The application identifies the container and widget, and may also specify a rectangle
   within that widget in order to invalidate a portion of the widget.  By default, the
   entire extent of the widget will be invalidated.
   
Prototype:

   void IROOTCONTAINER_Invalidate(IRootContainer *p, IWidget *pw, const AEERect *prc, uint32 f);

Parameters:
===pre>
   p:    Pointer to the IRootContainer interface object that serves as the parent to the widget
         to invalidate.
   pw:   Pointer to the IWidget object to be invalidated or NULL if the entire extent of the
         root container is to be the target for invalidation.  When 'pw' is supplied, the
         widget to be invalidated may be passed either explicitly as a pointer to an IWidget,
         or implicitly as a constant that identifies a particular widget within the root
         container.  The constants used to identify a relative position for the widget to be
         invalidated, are defined as follows:
                              
             WIDGET_ZTOPMOST     -  Requests that the widget at the top of the container's
                                    widget stack should be invalidated.
             WIDGET_ZBOTTOMMOST  -  Requests that the widget at the bottom of the container's
                                    widget stack should be invalidated.
             WIDGET_ZNORMAL      -  Requests that the widget placed at the container's most
                                    normal layout location be invalidated.  The root container
                                    treats the WIDGET_ZTOPMOST position as the "normal" location.
   
   prc:  Pointer to the rectangle that defines the area to be invalidated.  This rectangle
         is expressed relative to the coordinates of the widget specified in 'pw'.  When this
         parameter is NULL, the interface will use the full extent of the 'pw' widget as the
         invalidation rectangle.
   f:    A set of flags that control the the calculation of the invalidation rectangle.
         The defined validation flags are:
             ICIF_EXTENT  -  When this flag is ON, it is an indication that the widget has
                             changed its extent.
===/pre>

Return Value:
   None

Comments:
   Callers should set the ICIF_EXTENT flag of the 'f' parameter when calling the function
   following a change to the 'pw' widget's extent.  

Side Effects: 
   None

See Also:
   IRootContainer Interface
   IWidget Interface
   ICONTAINER_Invalidate()
   IWIDGET_Invalidate()

=======================================================================

IROOTCONTAINER_Locate()

Description: 
   This function will find the location of a particular widget, returning that widget's
   rectangle relative to the root container.  The function will also, optionally, return
   a pointer to the root container (which will actually be the same pointer passed into
   the function in the 'p' parameter).

Prototype:

   int IROOTCONTAINER_Locate(IRootContainer *p, IWidget *pw, IContainer **ppr, AEERect *prc)

Parameters:
===pre>
   p:    Pointer to the IRootContainer interface object that serves as the parent to the
         widget to locate.
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

   A pointer to the widget's root container will be returned in 'ppr', which will increment
   the reference count of the root container.

Comments:
   The rectangle referenced by 'prc' MUST be initialized prior to calling IROOTCONTAINER_Locate(),
   otherwise the results will be undefined.
   
   Since the root container is the container making the location request the pointer
   returned in 'ppr' will be the same pointer passed into the function in the 'p' parameter.

   The pointer passed in the 'ppr' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   IRootContainer Interface
   IWidget Interface
   ICONTAINER_Locate()

=======================================================================

IROOTCONTAINER_Insert()

Description:
   This function allows an application to insert a widget into the root container's widget
   stack -- at the top of the stack, at the bottom, or some place inbetween.  By default,
   widgets will be inserted at the top of the stack.  However, an application may choose
   to instead insert widgets anywhere within the root container's widget stack by specifying
   a widget already in the stack as a reference position for placing the new widget.

Prototype:
   int IROOTCONTAINER_Insert(IRootContainer *p, IWidget *pw, IWidget *pb, const WidgetPos *ppos);

Parameters:
===pre>   
   p:     A pointer to an IRootContainer object.
   pw:    Pointer to the widget to be inserted into the root container's widget stack.
   pb:    Indicates where in the root container's widget stack that the new widget
          should be inserted.  This value may be expressed as either a pointer to a 
          widget already in the widget stack, in which case the new widget will be
          inserted immediately before the widget pointed to by 'pb' -- or as  a
          constant that identifies a relative position within the widget stack, as
          follows:
              WIDGET_ZNORMAL     -  Allow BREW to place the inserted widget at the
                                    default location.  For the root container, this
                                    places the new widget at the top of container's
                                    widget stack.
              WIDGET_ZTOPMOST    -  The new widget should be inserted as the topmost
                                    widget in the root container's widget stack.
              WIDGET_ZBOTTOMMOST -  The new widget should be inserted at the bottommost
                                    widget in the root container's widget stack.
   ppos:  Pointer to a WidgetPos data structure, specifying the location where the
          widget should be displayed within the root container.   This data structure
          also defines whether or not the widget is currently visible.  If ppos is NULL,
          pw will be inserted at (0,0) with visibility set to TRUE.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully inserted into the root container's widget stack.
   EBADPARM  --  The widget pointer passed in 'pb' either does not exist, or refers to a
                 widget that is not in the root container's widget stack.
   ENOMEMORY --  The widget stack could not grow to accommodate a new widget.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
   IRootContainer Interface
   IWidget Interface
   WidgetPos
   IROOTCONTAINER_Remove()

===========================================================================

IROOTCONTAINER_Remove()

Description:
   This function allows an application to remove a widget from the root container's widget
   stack.  The widget to be removed may be specified explicitly by its widget pointer, or
   implicitly by a constant that identifies the widget's relative position within the
   container's widget stack.

Prototype:
   int IROOTCONTAINER_Remove(IRootContainer *p, IWidget *pw);

Parameters:
===pre>   
   p:   A pointer to an IRootContainer object.
   pw:  Identifies the widget that should be removed from the root container's widget stack.
        This widget may be expressed explicitly as a widget pointer, or implicitly as  a
        constant that identifies a relative position within the widget stack, as follows:
             WIDGET_ZNORMAL      - Remove the default widget, which BREW interprets to be
                                   the widget at the top of the widget stack.
             WIDGET_ZTOPMOST     - Remove the widget at the top of the widget stack.
             WIDGET_ZBOTTOMMOST  - Remove the widget at the bottom of the widget stack.

===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully removed from the root container's widget stack.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a
                 widget that is not in the root container's widget stack.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    IRootContainer Interface
    IWidget Interface
    IROOTCONTAINER_Insert()

===========================================================================

IROOTCONTAINER_SetPos()

Description:
   This function will change the layout position of a given widget within the root container,
   moving this widget to a new (x, y) location.  The function may also, optionally, relocate
   that widget to a different spot in the root container's widget stack -- effectively changing
   the order of the stack.  The new layout position and visibility attributes are passed to
   the function in a WidgetPos structure, pointed to by the 'ppos' parameter.  The widget to
   receive the new attributes may be specified explicitly, by its widget pointer, or
   implicitly by a constant that identifies the widget's relative position within the root
   container's widget stack.  The 'pb' parameter controls whether or not the widget's order
   within the stack will move.  Passing the constant WIDGET_ZNORMAL leaves the widget where
   it currently stands, without altering the order of the stack.  Passing WIDGET_ZTOPMOST or
   WIDGET_ZBOTTOMMOST will move the widget to the top or bottom of the stack, while passing
   an explicit widget pointer will move the target widget to the stack location before the
   widget passed in 'pb'.
   
Prototype:
   int IROOTCONTAINER_SetPos(IRootContainer *p, IWidget *pw, IWidget *pb, const WidgetPos *ppos);

Parameters:
===pre>   
   p:     A pointer to an IRootContainer object.
   pw:    Identifies the widget in the root container's widget stack that will be the
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
   pb:     Identifies a widget in the root container's widget stack that will serve as
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
                 refer to widgets that are not in the root container's widget stack.
===/pre>   

Comments:
   If IROOTCONTAINER_SetPos() is being called to change the location of the widget within the
   root container's widget stack without changing the layout position of the widget, the caller
   MUST still pass in a pointer to the correct layout position in the 'ppos' parameter.  In
   such cases, the application should first call IROOTCONTAINER_GetPos() to retrieve the current
   layout position, then call IROOTCONTAINER_SetPos() passing back the results received from
   IROOTCONTAINER_GetPos().

Side Effects: 
   None
   
See Also:
    IRootContainer Interface
    IWidget Interface
    WidgetPos
    IROOTCONTAINER_GetPos()

===========================================================================

IROOTCONTAINER_GetPos()

Description:
   This function retrieves a widget's layout position within the root container, returning
   a copy of that widget's layout position and visibility attribute in the WidgetPos data
   structure pointed to by 'ppos'.

Prototype:
   int IROOTCONTAINER_GetPos(IRootContainer *p, IWidget *pw, WidgetPos *ppos);

Parameters:
===pre>   
   p:     A pointer to an IRootContainer object.
   pw:    Identifies the widget in the root container's widget stack whose layout
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
                 widget that is not in the root container's widget stack.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    IRootContainer Interface
    IWidget Interface
    WidgetPos
    IROOTCONTAINER_SetPos()

===========================================================================

IROOTCONTAINER_GetWidget()

Description:
   This function provides facilities that allow an application to retrieve the widgets stored
   in the root container's widget stack.  The function may be instructed to return the widget
   from an explicit location, such as the top or bottom of the stack, or from some position
   relative to another widget in the widget stack -- for example, returning the widget that
   appears before or after another widget in the stack.  An application can easily iterate
   over all of the widgets in the root container by repeated calls to IROOTCONTAINER_GetWidget(),
   passing in the widget pointer returned from the previous call.

Prototype:
   IWidget *IROOTCONTAINER_GetWidget(IRootContainer *p, IWidget *pw, boolean d, boolean w);

Parameters:
===pre>   
   p:   A pointer to an IRootContainer object.
   pw:  Identifies the widget within the root container's widget stack that will serve as a
        reference point for the widget to be retrieved.  'pw' may be expressed explicitly
        as a widget pointer, or implicitly as a constant that identifies a relative position
        within the widget stack, as follows:
            WIDGET_ZNORMAL      - Use the widget at the root container's default location as
                                  the retrieval reference point.  BREW interprets this to be
                                  the widget at the top of the widget stack.
            WIDGET_ZTOPMOST     - Use the widget at the top of the widget stack as the
                                  retrieval reference point.
            WIDGET_ZBOTTOMMOST  - Use the widget at the bottom of the widget stack as the
                                  retrieval reference point.
        When 'pw' is NULL, the function will return either the top or bottom widget on the
        stack, depending on the value of 'd'.  When 'pw' is NULL and 'd' is TRUE, the function
        will return the bottommost widget.  When 'pw' is NULL and 'd' is FALSE, the function
        will return the topmost widget.  
   d:   Specifies whether the function should retrieve the next widget in the widget stack, or
        the previous widget on the stack -- always relative to the reference widget determined
        by the value passed in the 'pw' parameter.  When 'd' is TRUE the function will retrieve
        the next widget on the stack (i.e. the widget closer to the top of the stack).  When 'd'
        is FALSE the function will retrieve the previous widget on the stack (i.e. the widget
        closer to the bottom of the stack).  Note, however, that when 'pw' is passed as NULL,
        the 'd' parameter explicitly indicates either the topmost or bottommost widget, as
        described above.
   w:   Specifies whether or not the function should wrap when attempting to retrieve the next
        or previous widget from the top or bottom of the root container's widget stack.  When 'w'
        is  TRUE, the function will wrap at either end of the stack, otherwise the function
        will return NULL indicating that there are no more widgets to be retrieved.
===/pre>   
   
Return Value:
   A pointer to a widget in the root container's widget stack, or NULL when there is no widget
   to be returned. 

Comments:
   Though IROOTCONTAINER_GetWidget() function returns a pointer to a widget, the function does
   not add a reference to the returned IWidget for the caller.

   An application may iterate over the root container's entire widget stack -- from bottom to
   top -- with the following code:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = IROOTCONTAINER_GetWidget (pRootContainer, pWidget, TRUE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once IROOTCONTAINER_GetWidget() returns NULL
===/pre>   
   
   To iterate over all the widgets on the root container's widget stack from top to bottom,
   the application's code would look like the following:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = IROOTCONTAINER_GetWidget (pRootContainer, pWidget, FALSE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once IROOTCONTAINER_GetWidget() returns NULL
===/pre>   

Side Effects: 
  None

See Also:
   IRootContainer Interface
   IWidget Interface

===========================================================================

IROOTCONTAINER_SetCanvas

Description:
   The root container is the top-most entity in the widget/container hierarchy and is
   responsible for drawing all of its contained widgets. The ICanvas interface provides
   a drawing surface for widgets and other components to draw upon. This function is
   used to specify the ICanvas and the position and extent of the root container.

Prototype:
   void IROOTCONTAINER_SetCanvas(IRootContainer *p, ICanvas *piCanvas, const AEERect *prc);

Parameters:
===pre>   
   p:          A pointer to an IRootContainer object.
   piCanvas:   Pointer to an ICanvas object to set
   prc:        Pointer to an AEERect structure that specifies the location and 
               extent of the root container.
===/pre>   
   
Return Value:
   None

Comments:
   None   
   
Side Effects: 
  None

See Also:
   ICanvas interface

===========================================================================

IROOTCONTAINER_GetFirstWidget()

Description:
   This function retrieves a pointer to the first widget in the root container's widget stack.
   This is, effectively, the widget at the top of the widget stack.  The widget on the top of 
   the stack is the first one drawn onscreen and is the bottommost with regard to front-to-back 
   ordering within the container.
   
Prototype:
   IWidget *IROOTCONTAINER_GetFirstWidget(IRootContainer *p);

Parameters:
===pre>   
   p:   A pointer to an IRootContainerainer object.
===/pre>   
   
Return Value:
   A pointer to the widget at the top of the root container's widget stack, or NULL when the
   widget stack is empty. 

Comments:
   Though IROOTCONTAINER_GetFirstWidget() function returns a pointer to a widget, the function
   does not add a reference to the returned IWidget for the caller.

Side Effects: 
  None

See Also:
   IRootContainer Interface
   IWidget Interface
   IROOTCONTAINER_GetWidget()
   IROOTCONTAINER_GetLastWidget()

===========================================================================

IROOTCONTAINER_GetLastWidget()

Description:
   This function retrieves a pointer to the last widget in the root container's widget stack.
   This is, effectively, the widget at the bottom of the widget stack.  The widget on the bottom 
   of the stack is the last one drawn onscreen and is the topmost with regard to front-to-back 
   ordering within the container.
   
Prototype:
   IWidget *IROOTCONTAINER_GetLastWidget(IRootContainer *p);

Parameters:
===pre>   
   p:   A pointer to an IRootContainer object.
===/pre>   
   
Return Value:
   A pointer to the widget at the bottom of the root container's widget stack, or NULL when the
   widget stack is empty. 

Comments:
   Though IROOTCONTAINER_GetLastWidget() function returns a pointer to a widget, the function
   does not add a reference to the returned IWidget for the caller.

Side Effects: 
  None

See Also:
   IRootContainer Interface
   IWidget Interface
   IROOTCONTAINER_GetWidget()
   IROOTCONTAINER_GetFirstWidget()

===========================================================================

IROOTCONTAINER_TO_ICONTAINER()

Description:
   This function provides type safe casting of an IRootContainer pointer to an IContainer
   pointer and should be used when passing an IRootContainer pointer to functions that
   require a pointer to an IContainer.  This is safer than a simple cast, since the compiler
   will verify the pointer type.

Prototype:
   IContainer *IROOTCONTAINER_TO_ICONTAINER(IRootContainer *me)

Parameters:
===pre>
   me:  Pointer to an IRootContainer interface object.
===/pre>

Return Value:
   Returns 'me' cast to an IContainer pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IContainer Interface
   IRootContainer Interface

===========================================================================

IROOTCONTAINER_TO_IXYCONTAINER()

Description:
   This function provides type safe casting of an IRootContainer pointer to an IXYContainer
   pointer and should be used when passing an IRootContainer pointer to functions that
   require a pointer to an IXYContainer.  This is safer than a simple cast, since the
   compiler will verify the pointer type.

Prototype:
   IXYContainer *IROOTCONTAINER_TO_IXYCONTAINER(IRootContainer *p)

Parameters:
===pre>
   me:  Pointer to an IRootContainer interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IXYContainer pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IXYContainer Interface
   IRootContainer Interface
   
===========================================================================

IROOTCONTAINER_GetCanvas

Description:
   The root container is the top-most entity in the widget/container hierarchy and is
   responsible for drawing all of its contained widgets. The ICanvas interface provides
   a drawing surface upon which widgets and other components draw. This function is
   used to get the ICanvas and the position and extent of the canvas itself.

Prototype:
   int IROOTCONTAINER_GetCanvas(IRootContainer *p, ICanvas **pic, const AEERect *prc);

Parameters:
===pre>   
   p:          A pointer to an IRootContainer object.
   ppi:        A pointer to an ICanvas pointer which will be set
   prc:        Pointer to an AEERect structure that specifies the location and 
               extent of the root container.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS  --  The canvas was successfully retrieved.
   EFAILED  --  The canvas was NULL
===/pre>   

Comments:
   This function does an Add_Ref on the returned ICanvas. 
  
Side Effects: 
  None

Version:
   Introduced in BUIT Version 1.2.

See Also:
   IROOTCONTAINER_SetCanvas
   ICanvas interface
*/

#endif /* __AEEROOTCONTAINER_H__ */
