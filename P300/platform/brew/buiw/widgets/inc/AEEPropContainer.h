/*
  ========================================================================

  FILE:  AEEPropContainer.h
  
  SERVICES: 

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEEPROPCONTAINER_H__
#define __AEEPROPCONTAINER_H__

#include "AEEContainer.h"

#include "bid/AEECLSID_PROPCONTAINER.bid"
#include "bid/AEEIID_PROPCONTAINER.bid"


typedef struct WidgetProp WidgetProp;
struct WidgetProp 
{
   uint16  prop;
   boolean bVisible;
};


typedef struct IPropContainer IPropContainer;
#define INHERIT_IPropContainer(iname) \
   INHERIT_IContainer(iname);\
   int      (*SetProp)     (iname *, IWidget *pw, IWidget *piwBefore, const WidgetProp *pProp);\
   int      (*GetProp)     (iname *, IWidget *pw, WidgetProp *pProp);\
   int      (*SetPadding)  (iname *, IWidget *pw, int32 dwBefore, int32 dwAfter);\
   int      (*GetPadding)  (iname *, IWidget *pw, int32 *pdwBefore, int32 *pdwAfter)

AEEINTERFACE(IPropContainer) {
   INHERIT_IPropContainer(IPropContainer);
};


#define IPROPCONTAINER_AddRef(p)               AEEGETPVTBL(p,IPropContainer)->AddRef(p)
#define IPROPCONTAINER_Release(p)              AEEGETPVTBL(p,IPropContainer)->Release(p)
#define IPROPCONTAINER_QueryInterface(p,i,p2)  AEEGETPVTBL(p,IPropContainer)->QueryInterface(p,i,p2)
#define IPROPCONTAINER_Invalidate(p,pw,prc,f)  AEEGETPVTBL(p,IPropContainer)->Invalidate(p,pw,prc,f)
#define IPROPCONTAINER_Locate(p,pw,ppr,prc)    AEEGETPVTBL(p,IPropContainer)->Locate(p,pw,ppr,prc)
#define IPROPCONTAINER_SetProp(p,pw,pb,pp)     AEEGETPVTBL(p,IPropContainer)->SetProp(p,pw,pb,pp) 
#define IPROPCONTAINER_GetProp(p,pw,pp)        AEEGETPVTBL(p,IPropContainer)->GetProp(p,pw,pp)
#define IPROPCONTAINER_GetWidget(p,pw,d,w)     AEEGETPVTBL(p,IPropContainer)->GetWidget(p,pw,d,w) 
#define IPROPCONTAINER_Remove(p,pw)            AEEGETPVTBL(p,IPropContainer)->Remove(p,pw)         

static __inline  int IPROPCONTAINER_Insert(IPropContainer *po, IWidget *pw, IWidget *pb, const WidgetProp *pProp) {
   return AEEGETPVTBL(po,IPropContainer)->Insert(po,pw,pb,(const void *)pProp);
}


static __inline IContainer *IPROPCONTAINER_TO_ICONTAINER(IPropContainer *me) {
   return (IContainer *)(void*)me;
}

// Introduced in BUIT Version 1.2
static __inline  int IPROPCONTAINER_SetPadding(IPropContainer *po, IWidget *pw, int16 wBefore, int16 wAfter) {
   return AEEGETPVTBL(po,IPropContainer)->SetPadding(po,pw,(int32)wBefore,(int32)wAfter);
}

static __inline  int IPROPCONTAINER_GetPadding(IPropContainer *po, IWidget *pw, int16 *pwBefore, int16 *pwAfter) {
   int32 dwBefore, dwAfter;
   int dwResult = AEEGETPVTBL(po,IPropContainer)->GetPadding(po,pw,&dwBefore,&dwAfter);
   if (pwBefore) {
      *pwBefore = (int16)dwBefore;
   }
   if (pwAfter) {
      *pwAfter = (int16)dwAfter;
   }
   return dwResult;
}

#define IPROPCONTAINER_SetPaddingExact(p,pw,b,a)         IPROPCONTAINER_SetPadding((p),(pw),(-b),(-a))
#define IPROPCONTAINER_SetPaddingProportional(p,pw,b,a)  IPROPCONTAINER_SetPadding((p),(pw),(b),(a))

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Prop Container Interface and Class IDs

Description:
   Each class and interface within the widget framework is identified by a unique
   ID.  These IDs are used throughout the framework to distinguish between various
   types of objects, such as containers and widgets.

   The prop container supports both an interface and the creation of class object
   instances.  The prop container interface or a new instance of the prop container
   class object may be created by calling ISHELL_CreateInstance() and passing in the
   desired class id of the object.  The object that is created is, itself, a
   container and may utilize those APIs defined for either the container or prop
   container interfaces.

   The prop container recognizes the following set of class and interface IDs.

Definition:

Identifier               Definition
----------               -----------------------------------
AEECLSID_PROPCONTAINER:  Identifies the class ID of the prop container
AEEIID_PROPCONTAINER:    Identifies the interface ID of the prop container

Comments:
   None

See Also:
   IPropContainer Interface
   ISHELL_CreateInstance()
   
=============================================================================

WidgetProp

Description:
   This data type defines special attributes for any widget that is to be managed by a
   prop container.  These attributes identify whether or not the widget should be
   factored into the container's layout calculations and -- if it should -- how the
   widget will be proportioned when laid out in the container.
   
Definition:
   typedef struct {
      uint16  prop;
      boolean bVisible;
   } WidgetProp;

Members:
===pre>
   prop:      Specifies how the widget should be laid out in the container, relative either
              to its own extent, or relative to the extent of other objects being managed by
              the prop container.
          
              When specifying the widget's proportion in relationship to other objects, 'prop'
              contains the numerator of the fraction of this widget's extent to the combined
              extents of all other proportional objects managed by the container.  So, for
              example, if a prop container contains three widgets with proportional values of
              1, 2 and 2, the widget with 'prop' of 1 would be granted an extent that is
              one-fifth (1 of 1 + 2 + 2) of the total variable extent, whereas each of the
              widgets with a 'prop' value of 2 would be granted an extent that is two-fifths
              (2 of 1 + 2 + 2) of the total variable extent.

              When expressing the widget's proportion in relation to its own extent, 'prop'
              is set to one of the following defined constants:
                              
                  WIDGET_SIZE_TO_FIT  - The prop container will grant an extent to this
                                        object which is sized to fit the widget's preferred
                                        extent.  For example, if the prop container is being
                                        laid out horizontally, an object with 'prop' equal to
                                        WIDGET_SIZE_TO_FIT will be granted a horizontal extent
                                        equal to the preferred horizontal extent of the widget.
                  WIDGET_KEEP_EXTENT  - The prop container will grant an extent to this
                                        object which is exactly the same size as the widget's
                                        extent.  For example, if the prop container is being
                                        laid out vertically, an object with 'prop' equal to
                                        WIDGET_KEEP_EXTENT will be granted a vertical extent
                                        equal to the existing vertical extent of the widget.
                                        
   bVisible:  Boolean value specifying whether or not this widget should be displayed.
              If the widget is not displayed, it is also not factored into layout
              calculations for the container.
===/pre>
Comments:
   None

See Also:
   IPROPCONTAINER_SetProp()
   IPROPCONTAINER_GetProp()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IPropContainer Interface

Description:
   The prop container is derived from the base container object and is used to manage the
   widgets within a container where those widgets are laid out (in proportion to one another)
   along the same vertical or horizontal direction.  Having derived from the base container,
   the prop container maintains each of the characteristics common to the container object,
   while an additional attribute describes the direction of the objects are to be laid out
   within the container.
   
   Objects are laid out left to right or top to bottom in the order that the widgets appear
   in the container's widget stack, with the prop container dictating the extent granted to
   each object.  The prop container will determine each object's extent based on that object's
   proportional value -- a value that is expressed either explicitly of as a ratio component
   that is measured in relationship to all other objects within that container, or implicitly
   as a constant that will determine the size of the object independent of other objects held
   within the container.  Therefore, depending on the requirements of each object held in the
   container, the relative size of the objects within that container may vary as the size of
   the container changes.
   
   To illustrate, consider for a moment a prop container laid out horizontally that contains
   one object that MUST maintain a constant width, but two additional objects that don't really
   care how about their width -- but they do want to be of equal size.  On one display, these
   objects might be laid out as follows:
   
===pre>   
                        Prop Container
   +----------------------------------------------------+
   |+-------------------+ +------------+ +------------+ |
   ||                   | |            | |            | |
   ||                   | |            | |            | |
   ||      Object       | |   Object   | |   Object   | |
   ||   that must be    | | that does  | | that does  | |
   ||   exactly this    | |  not care  | |  not care  | |
   ||      width        | |            | |            | |
   ||                   | |            | |            | |
   ||                   | |            | |            | |
   ||                   | |            | |            | |
   ||                   | |            | |            | |
   |+-------------------+ +------------+ +------------+ |
   +----------------------------------------------------+
===/pre>   
   
   The first object has requested that it be displayed using a constant width, no matter what,
   so it will always be the same horizontal extent.  The other two objects will adjust the width
   of their extent based on whatever space is leftover once the first object has consumed all
   the space it needs.  Again, to illustrate how the objects will adjust depending on the size
   of the prop container, let's make the container wider and see what happens, okay?
   
===pre>   
                                         Wider Prop Container
   +--------------------------------------------------------------------------------------+
   |+-------------------+ +-----------------------------+ +-----------------------------+ |
   ||                   | |                             | |                             | |
   ||                   | |                             | |                             | |
   ||      Object       | |            Object           | |            Object           | |
   ||   that must be    | |          that does          | |          that does          | |
   ||   exactly this    | |           not care          | |           not care          | |
   ||      width        | |                             | |                             | |
   ||                   | |                             | |                             | |
   ||                   | |                             | |                             | |
   ||                   | |                             | |                             | |
   ||                   | |                             | |                             | |
   |+-------------------+ +-----------------------------+ +-----------------------------+ |
   +--------------------------------------------------------------------------------------+
===/pre>   

   Oh my, look at that!  Each of the objects that were willing to grow to the size of the
   container have, in fact, grown!  And yet, they are still the same size.  This is because
   each of these objects have requested proportions of equal values -- so their ratio would
   be 1:1 (all things being equal).  But what if that were not the case?  What if one of those
   objects wanted to be twice as big as the other "adjustable" object?  In that case, that
   object would set its proportional value to be double that of the other object, resulting
   in a ratio of 2:1 and a distribution of objects that would look like this:
   
===pre>   
                                         Wider Prop Container
   +--------------------------------------------------------------------------------------+
   |+-------------------+ +-------------------+ +---------------------------------------+ |
   ||                   | |                   | |                                       | |
   ||                   | |                   | |                                       | |
   ||      Object       | |      Object       | |                 Object                | |
   ||   that must be    | |  of normal size   | |             of double size            | |
   ||   exactly this    | |   at ratio 1:2    | |              at ratio 2:1             | |
   ||      width        | |                   | |                                       | |
   ||                   | |                   | |                                       | |
   ||                   | |                   | |                                       | |
   ||                   | |                   | |                                       | |
   ||                   | |                   | |                                       | |
   |+-------------------+ +-------------------+ +---------------------------------------+ |
   +--------------------------------------------------------------------------------------+
===/pre>   
   
   Notice in each of the above horizontal layout examples that the vertical extent of each
   object extends to be the full height of the prop container.  When laying out the objects
   within a container, the prop container applies its layout algorithm only in the direction
   of the container's layout, and will provide an "opposite direction" extent that is the
   maximum value that the container will accommodate.  So, when laying out objects in the
   horizontal direction, the vertical extent of each object will be set to the vertical
   extent of the container.  Likewise, when laying out objects in the vertical direction,
   the horizontal extent of each object will be set to the horizontal extent of the container.
   
   Next, let's look at an example of a vertical prop container layout that illustrates how
   fixed and proportional objects can be mixed within the same container.  This container will
   include three objects of a fixed vertical extent, as well as two variable height objects that
   will all be of equal proportions:
   
===pre>
         Order the objects will be laid out
        ------------------------------------
           1. Set height object
           2. Variable height object (1:1 ratio)
           3. Set height object
           4. Variable height object (1:1 ratio)
           5. Set height object
===/pre>   
   
   When laid out by the prop container, the 1st, 3rd and 5th objects all require a set
   amount of space, regardless of the size of the container.  The 2nd and 4th objects
   get what space is left after the set objects have had their space reservations filled,
   and will divide that space equally.  All five objects will still appear in the container
   in the order that they are managed by the container's object stack, but will be sized as
   follows:
   
===pre>   
            Prop Container                          Smaller Container
   +-------------------------------+        +-------------------------------+
   |+-----------------------------+|        |+-----------------------------+|
   ||                             ||        ||                             ||
   ||          Object #1          ||        ||          Object #1          ||
   ||                             ||        ||                             ||
   |+-----------------------------+|        |+-----------------------------+|
   |+-----------------------------+|        |+-----------------------------+|
   ||                             ||        ||          Object #2          ||
   ||                             ||        |+-----------------------------+|
   ||                             ||        |+-----------------------------+|
   ||                             ||        ||                             ||
   ||          Object #2          ||        ||          Object #3          ||
   ||                             ||        ||                             ||
   ||                             ||        |+-----------------------------+|
   ||                             ||        |+-----------------------------+|
   ||                             ||        ||          Object #4          ||
   |+-----------------------------+|        |+-----------------------------+|
   |+-----------------------------+|        |+-----------------------------+|
   ||                             ||        ||          Object #5          ||
   ||          Object #3          ||        |+-----------------------------+|
   ||                             ||        +-------------------------------+
   |+-----------------------------+|
   |+-----------------------------+|
   ||                             ||
   ||                             ||
   ||                             ||
   ||                             ||
   ||          Object #4          ||
   ||                             ||
   ||                             ||
   ||                             ||
   ||                             ||
   |+-----------------------------+|
   |+-----------------------------+|
   ||          Object #5          ||
   |+-----------------------------+|
   +-------------------------------+
===/pre>

   Note in the above example that the three set objects (1, 3 and 5) all maintain the same
   vertical extent when the container size is made smaller, while the variable size objects
   (2 and 4) maintain their 1:1 relational ratio, but require a smaller vertical extent in
   the smaller prop container.
   
   Once granted an extent by the prop container, it is then up to each object to draw itself
   within the space it has been allotted, which will depend on the drawing and layout
   capabilities of that particular object.

Supported Events:
   The prop container processes events in two passes.  The prop container first passes all events
   to the base container, which will handle events relating to the container's focus.  Events not
   handled during this first pass will then be handled by the prop container itself.
   
   The prop container will accept the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The prop container responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The prop container
                      allows the following properties to be set:

                          PROP_LAYOUTSTYLE  --  Sets the direction that objects managed by
                                                the container should be laid out.

                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The prop container responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  The prop container allows
                      the following properties to be retrieved, returning the value of the
                      property in the pointer pointed to by 'dwParam':

                          PROP_LAYOUTSTYLE  --  Gets the direction that objects managed by
                                                the container should be laid out.

                      These properties are discussed below in greater detail.
===/pre>   

Properties:
Property           Description
--------           ------------------------------------------------------------
PROP_LAYOUTSTYLE:  This property contains one of two constants that specify the layout direction
                   for objects managed by the prop container.  When objects are to be laid out
                   from top to bottom, the property will be stored as LAYOUT_VERT.  Containers
                   that lay out their objects from left to right specify LAYOUT_HORZ as the
                   property value.  The default value of this property is LAYOUT_VERT.

                       Property Value:  int

Required Model:
   None 
   
Model Data:
   None

Comments:
   None
   
See Also:
   IContainer Interface

===========================================================================

IPROPCONTAINER_AddRef()

Description:
   This function increments the reference count of the IPropContainer interface object,
   allowing the interface to be shared by multiple callers.  The object is freed when the
   reference count reaches 0 (zero).

Prototype:

   uint32 IPROPCONTAINER_AddRef(IPropContainer *p);

Parameters:
===pre>
   p:  Pointer to the IPropContainer interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IPropContainer interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IPROPCONTAINER_Release()
   
=======================================================================

IPROPCONTAINER_Release()

Description:

   This function decrements the reference count of the IPropContainer interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IPROPCONTAINER_Release(IPropContainer *p);

Parameters:
===pre>
   p:  Pointer to the IPropContainer interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IPropContainer interface object.  If the
   returned reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IPROPCONTAINER_AddRef()

=======================================================================

IPROPCONTAINER_QueryInterface()

Description: 
   This function queries an interface for an alternate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IPROPCONTAINER_QueryInterface(IPropContainer *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IPropContainer interface object.
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

IPROPCONTAINER_Invalidate()

Description: 
   This function will invalidate all or part of a widget within a prop container.  The
   application identifies the container and widget, and may also specify a rectangle
   within that widget in order to invalidate a portion of the widget.  By default, the
   entire extent of the widget will be invalidated.
   
Prototype:

   void IPROPCONTAINER_Invalidate(IPropContainer *p, IWidget *pw, const AEERect *prc, uint32 f)

Parameters:
===pre>
   p:    Pointer to the IPropContainer interface object that serves as the parent to the widget
         to invalidate.
   pw:   Pointer to the IWidget object to be invalidated, or a constant that implies a particular
         widget within the prop container.  The constants used to identify a relative position
         for the widget to be invalidated, are defined as follows:
                              
             WIDGET_ZTOPMOST     -  Requests that the widget at the top of the container's
                                    widget stack should be invalidated.
             WIDGET_ZBOTTOMMOST  -  Requests that the widget at the bottom of the container's
                                    widget stack should be invalidated.
             WIDGET_ZNORMAL      -  Requests that the widget placed at the container's most
                                    normal layout location be invalidated.  The base container
                                    treats the WIDGET_ZTOPMOST position as the "normal" location.
   
   prc:  Pointer to the rectangle that defines the area to be invalidated.  This rectangle is
         expressed relative to the coordinates of the specified widget.  When this parameter
         is NULL, the interface will use the full extent of the 'pw' widget as the invalidation
         rectangle.
   f:    A set of flags that control the the calculation of the invalidation rectangle.
         The defined validation flags are:
             ICIF_EXTENT  -  When this flag is ON, it is an indication that the widget has
                             changed its extent.
             ICIF_REDRAW  -  When this flag is ON, it is an indication that the widget has 
                             changed its contents, but its preferred extent is still the same.
                             The widget simply wants to be redrawn.
             ICIF_DEFER   -  When this flag is ON, it indicates that the widget's layout is
                             disabled, but it is still passing an invalidate up to 
                             notify its ancestors that an invalidate occurred but no action
                             was taken as layout was disabled.  When RootContainer receives
                             an invalidate with the ICIF_DEFER flag appended, it will not cause
                             a draw to be scheduled. 
===/pre>

Return Value:
   None

Comments:
   Calling IPROPCONTAINER_Invalidate() will trigger the invalidation to travel up through the
   container's hierarchy, first to its parent, then to its parent, etc. so that the
   invalidation rectangle may be applied to each object in the chain.
   
   Callers should set the ICIF_EXTENT flag of the 'f' parameter when calling the function
   following a change to the widget's extent.  

Side Effects: 
   Calling IPROPCONTAINER_Invalidate() with the ICIF_EXTENT flag on will cause the layout of the
   objects within the prop container to change, using the widget's new extent.

See Also:
   IPropContainer Interface
   IWidget Interface
   ICONTAINER_Invalidate()
   IWIDGET_Invalidate()

=======================================================================

IPROPCONTAINER_Locate()

Description: 
   This function will find the location of a particular widget, returning that widget's
   rectangle relative to the widget's root container.  The function will also, optionally,
   return a pointer to the root container.

Prototype:

   int IPROPCONTAINER_Locate(IPropContainer *p, IWidget *pw, IContainer **ppr, AEERect *prc)

Parameters:
===pre>
   p:    Pointer to the IPropContainer interface object that serves as the parent to the widget
         to locate.
   pw:   Pointer to the IWidget object to be located.
   ppr:  Pointer to an IContainer pointer, into which the function will return a pointer to the
         container's root container.  This parameter may be specified as NULL if the caller does not
         wish to get the root container.
   prc:  Pointer to an AEERect.  On input, this rectangle will specify the current relative
         offset of the 'pw' object relative to its child.  On output, this rectangle specifies
         the position of the object relative to the root container.  Therefore, the function
         should initially be called by passing in a rectangle with origin at (0, 0) -- otherwise,
         the returned rectangle will be expressed relative to the initial offset.
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
   The rectangle referenced by 'prc' MUST be initialized prior to calling IPROPCONTAINER_Locate(),
   otherwise the results will be undefined.

   The pointer passed in the 'ppr' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   IPropContainer Interface
   IWidget Interface
   ICONTAINER_Locate()


=======================================================================

IPROPCONTAINER_Insert()

Description:
   This function allows an application to insert a widget into the prop container's widget
   stack -- at the top of the stack, at the bottom, or someplace in between.  By default,
   widgets will be created in the order that they will be laid out within the container
   (left to right, or top to bottom).  However, a widget may be inserted anywhere in
   the widget stack depending on the needs of the application and container.  An example
   would be an application that breeds widgets.  Imagine an application that looks like
   the following:

===pre>   
       +-----------------------------------+
       |           Widget Breeder          |
       +-----------------------------------+
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       +-----------------------------------+
       |   Press any key to breed widgets  |
       +-----------------------------------+
===/pre>
   
   This prop container is initially laid out vertically with the following three widgets:
===pre>   
       1. Static widget with 'prop' = WIDGET_KEEP_EXTENT
       2. Dummy widget with 'prop' = 1
       3. Static widget with 'prop' = WIDGET_KEEP_EXTENT
===/pre>
   
   The net result is the screen above -- two WIDGET_KEEP_EXTENT sized widgets with a
   variable sized widget in between that will occupy all the space not required by the
   WIDGET_KEEP_EXTENT widgets.
   
   When the user presses a key, a new widget will be created and inserted into the widget
   stack before widget 3 (we'd also likely call IPROPCONTAINER_Remove() to get rid of the
   dummy widget we'd no longer need).  With each subsequent key press, additional widgets
   would be inserted, all prior to the static widget located at the bottom of the screen,
   like so:

===pre>   
                                                             Following
                    Initial State                      several key presses
       +-----------------------------------+   +-----------------------------------+
       |           Widget Breeder          |   |           Widget Breeder          |
       +-----------------------------------+   +-----------------------------------+
       |                                   |   |+---------------------------------+|
       |                                   |   ||                                 ||
       |                                   |   ||  A baby widget has been born!   ||
       |                                   |   ||                                 ||
       |                                   |   |+---------------------------------+|
       |                                   |   ||                                 ||
       |                                   |   ||  A baby widget has been born!   ||
       |                                   |   ||                                 ||
       |                                   |   |+---------------------------------+|
       |                                   |   ||                                 ||
       |                                   |   ||  A baby widget has been born!   ||
       |                                   |   ||                                 ||
       |                                   |   |+---------------------------------+|
       +-----------------------------------+   +-----------------------------------+
       |   Press any key to breed widgets  |   |   Press any key to breed widgets  |
       +-----------------------------------+   +-----------------------------------+
===/pre>

   At this point, the widget stack within the prop container would appear as follows:
===pre>   
       1. Static widget with 'prop' = WIDGET_KEEP_EXTENT
       2. Oldest baby widget
       3. 2nd Oldest baby widget
       4. 3rd Oldest baby widget
       5. Static widget with 'prop' = WIDGET_KEEP_EXTENT
===/pre>
   
Prototype:
   int IPROPCONTAINER_Insert(IPropContainer *p, IWidget *pw, IWidget *pb, const WidgetProp *pProp);

Parameters:
===pre>   
   p:        A pointer to an IPropContainer object.
   pw:       Pointer to the widget to be inserted into the prop container's widget stack.
   pb:       Indicates where in the prop container's widget stack that the new widget
             should be inserted.  This value may be expressed as either a pointer to a 
             widget already in the widget stack, in which case the new widget will be
             inserted immediately before the widget pointed to by 'pb' -- or as  a
             constant that identifies a relative position within the widget stack, as
             follows:
                 WIDGET_ZNORMAL     -  Allow BREW to place the inserted widget at the
                                       default location.  For a prop container, this
                                       places the new widget at the top of container's
                                       widget stack.
                 WIDGET_ZTOPMOST    -  The new widget should be inserted as the topmost
                                       widget in the container's widget stack.
                 WIDGET_ZBOTTOMMOST -  The new widget should be inserted at the bottommost
                                       widget in the container's widget stack.
   pibppos:  Pointer to a WidgetProp data structure, containing the proportional value for
             this widget, as well as its visibility attribute.  If this parameter is NULL,
             pw will be inserted with WIDGET_KEEP_EXTENT for its proportion, and 
             visibility will be set to TRUE.  
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully inserted into the prop container's widget stack.
   EBADPARM  --  The widget pointer passed in 'pb' either does not exist, or refers to a widget
                 that is not in the prop container's widget stack.
   ENOMEMORY --  The widget stack could not grow to accommodate a new widget.
===/pre>   

Comments:
   None

Side Effects: 
   Calling IPROPCONTAINER_Insert() will cause the prop container to adjust the object layout
   to include the newly added widget.
   
See Also:
   IPropContainer Interface
   IWidget Interface
   WidgetProp
   IPROPCONTAINER_Remove()

===========================================================================

IPROPCONTAINER_Remove()

Description:
   This function allows an application to remove a widget from the prop container's widget
   stack.  The widget to be removed may be specified explicitly, by its widget pointer, or
   implicitly by a constant that identifies the widget's relative position within the
   container's widget stack.
   
Prototype:
   int IPROPCONTAINER_Remove(IPropContainer *p, IWidget *pw);

Parameters:
===pre>   
   p:   A pointer to an IPropContainer object.
   pw:  Identifies the widget that should be removed from the prop container's widget stack.
        This widget may be expressed explicitly as a widget pointer, or implicitly as  a
        constant that identifies a relative position within the widget stack, as follows:
             WIDGET_ZNORMAL     -  Remove the default widget, which BREW interprets to be
                                   the widget at the top of the widget stack.
             WIDGET_ZTOPMOST     - Remove the widget at the top of the widget stack.
             WIDGET_ZBOTTOMMOST  - Remove the widget at the bottom of the widget stack.

===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully removed from the prop container's widget stack.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a widget
                 that is not in the prop container's widget stack.
===/pre>   

Comments:
   None

Side Effects: 
   Calling IPROPCONTAINER_Remove() will cause the prop container to adjust the object layout
   to consume the space surrendered by the removed widget.
   
See Also:
    IPropContainer Interface
    IWidget Interface
    IPROPCONTAINER_Insert()

===========================================================================

IPROPCONTAINER_SetProp()

Description:
   This function will change the proportional or visible attributes of a given widget, and
   will optionally relocate that widget to a different location in the prop container's
   widget stack.  The new proportion and visibility attributes are passed to the function
   in a WidgetProp structure, pointed to by the 'pProp' parameter.  The widget to receive
   the new attributes may be specified explicitly by its widget pointer, or implicitly by
   a constant that identifies the widget's relative position within the container's widget
   stack.  The 'pb' parameter controls whether or not the widget's location in the stack
   will move.  Passing the constant WIDGET_ZNORMAL leaves the widget where it currently
   stands, without altering the order of the stack.  Passing WIDGET_ZTOPMOST or
   WIDGET_ZBOTTOMMOST will move the widget to the top or bottom of the stack, while passing
   an explicit widget pointer will move the target widget to the stack location before the
   widget passed in 'pb'.
   
Prototype:
   int IPROPCONTAINER_SetProp(IPropContainer *p, IWidget *pw, IWidget *pb, const WidgetProp *pProp);

Parameters:
===pre>   
   p:     A pointer to an IPropContainer object.
   pw:    Identifies the widget in the prop container's widget stack that will be the
          target of the proportion change and (optionally) relocation.  This widget may
          be expressed explicitly as a widget pointer, or implicitly as  a constant that
          identifies a relative position within the widget stack, as follows:
               WIDGET_ZNORMAL      - The target widget is the default widget, which BREW
                                     interprets to be the widget at the top of the widget
                                     stack.
               WIDGET_ZTOPMOST     - The target widget is the widget at the top of the
                                     widget stack.
               WIDGET_ZBOTTOMMOST  - The target widget is the widget at the bottom of the
                                     widget stack.
   pb:     Identifies a widget in the  prop container's widget stack that will serve as
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
   
   pProp:  Pointer to a WidgetProp data structure, containing the new proportional value
           for the target widget, as well as its visibility attribute.

===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The proportional attributes of the widget were successfully changed.
   EBADPARM  --  The widget pointers passed in 'pw' and/or 'pb' either do not exist, or
                 refer to widgets that are not in the prop container's widget stack.
===/pre>   

Comments:
   If IPROPCONTAINER_SetProp() is being called to change the location of the widget within the
   prop container's widget stack without changing the proportional attributes of the widget,
   the caller MUST still pass in a pointer to the correct proportional attributes in the 'pProp'
   parameter.  In such cases, the application should first call IPROPCONTAINER_GetProp() to
   retrieve the current proportional attributes, then call IPROPCONTAINER_SetProp() passing
   back the results received from IPROPCONTAINER_GetProp().

Side Effects: 
   Calling IPROPCONTAINER_SetProp() will cause the prop container to adjust the object layout
   to accommodate the changed proportions or ordering of the objects in the container's widget
   stack.
   
See Also:
    IPropContainer Interface
    IWidget Interface
    WidgetProp
    IPROPCONTAINER_GetProp()

===========================================================================

IPROPCONTAINER_GetProp()

Description:
   This function retrieves a widget's proportional attributes, returning a copy of those
   values in the WidgetProp data structure pointed to by 'pProp'.  The data returned in
   'pProp' will include the widget's proportional layout value, as well as its current
   visibility attribute.
   
Prototype:
   int IPROPCONTAINER_GetProp(IPropContainer *p, IWidget *pw, WidgetProp *pProp);

Parameters:
===pre>   
   p:     A pointer to an IPropContainer object.
   pw:    Identifies the widget in the prop container's widget stack whose proportional
          attributes should be retrieved.  This widget may be expressed explicitly as
          a widget pointer, or implicitly as  a constant that identifies a relative
          position within the widget stack, as follows:
               WIDGET_ZNORMAL      - Retrieve the proportional attributes for the default
                                     widget, which BREW interprets to be the widget at
                                     the top of the widget stack.
               WIDGET_ZTOPMOST     - Retrieve the proportional attributes for the widget
                                     at the top of the  widget stack.
               WIDGET_ZBOTTOMMOST  - Retrieve the proportional attributes for the widget
                                     at the bottom of the  widget stack.
   pProp:  Pointer to a WidgetProp data structure, into which the function will copy the
           current proportional attributes of the widget.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The proportional attributes of the widget were successfully retrieved.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a
                 widget that is not in the prop container's widget stack.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    IPropContainer Interface
    IWidget Interface
    WidgetProp
    IPROPCONTAINER_SetProp()

===========================================================================

IPROPCONTAINER_GetWidget()

Description:
   This function provides facilities that allow an application to retrieve the widgets stored
   in a prop container's widget stack.  The function may be instructed to return the widget from
   an explicit location, such as the top or bottom of the stack, or from some position relative
   to another widget in the widget stack -- for example, returning the widget that appears before
   or after another widget in the stack.  An application can easily iterate over all of the widgets
   in the prop container by repeatedly calling IPROPCONTAINER_GetWidget() and passing in the widget
   pointer returned from the previous call.
   
Prototype:
   IWidget *IPROPCONTAINER_GetWidget(IPropContainer *p, IWidget *pw, boolean d, boolean w);

Parameters:
===pre>   
   p:   A pointer to an IPropContainer object.
   pw:  Identifies the widget within the prop container's widget stack that will serve as a
        reference point for the widget to be retrieved.  This widget may be expressed explicitly as
        a widget pointer, or implicitly as a constant that identifies a relative position within
        the widget stack, as follows:
            WIDGET_ZNORMAL      - Use the widget at the prop container's default location as the
                                  retrieval reference point.  BREW interprets this to be the widget
                                  at the top of the widget stack.
            WIDGET_ZTOPMOST     - Use the widget at the top of the widget stack as the retrieval
                                  reference point.
            WIDGET_ZBOTTOMMOST  - Use the widget at the bottom of the widget stack as the retrieval
                                  reference point.
        When 'pw' is NULL, the function will return either the top or bottom widget on the stack,
        depending on the value of 'd'.  When 'pw' is NULL and 'd' is TRUE, IPROPCONTAINER_GetWidget()
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
        previous widget from the top or bottom of the prop container's widget stack.  When 'w' is
        TRUE, the function will wrap at either end of the stack, otherwise the function will return
        NULL indicating that there are no more widgets to be retrieved.
===/pre>   
   
Return Value:
   A pointer to a widget in the prop container's widget stack, or NULL when there is no widget to
   be returned. 

Comments:
   Though IPROPCONTAINER_GetWidget() function returns a pointer to a widget, the function does not
   add a reference to the returned IWidget for the caller.

   An application may iterate over the entire widget stack -- from bottom to top -- with the
   following code:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = IPROPCONTAINER_GetWidget (pPropContainer, pWidget, TRUE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once IPROPCONTAINER_GetWidget() returns NULL
===/pre>   
   
   To iterate over all the widgets on the widget stack from top to bottom, the application's
   code would look like the following:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = IPROPCONTAINER_GetWidget (pPropContainer, pWidget, FALSE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once IPROPCONTAINER_GetWidget() returns NULL
===/pre>   

Side Effects: 
  None

See Also:
   IPropContainer Interface
   IWidget Interface

===========================================================================

  IPROPCONTAINER_GetPadding()

Description:
   This function retrieves the padding that will be applied before
   and/or after the given widget.  The values returned are either the
   proportional layout value or the exact number of pixels to be applied
   before and/or after the widget during layout.  Positive values are to
   be interpreted as proportional layout values - the numerator of
   the fraction of this spacer's extent to the combined extents of 
   all other proportional objects managed by the container.  Negative 
   values are to be interpreted as the exact pixel width to be left 
   clear ahead and/or behind the widget.
   
Prototype:
   int IPROPCONTAINER_GetPadding(
      IPropContainer *p, 
      IWidget *pw, 
      int32 *pdwBefore, 
      int32 *pdwAfter);

Parameters:
===pre>   
   p:          A pointer to an IPropContainer object.
   pw:         Identifies the widget within the prop container's widget 
               stack that will serve as a reference point for the widget to 
               be retrieved.  This widget may be expressed explicitly as a 
               widget pointer, or implicitly as a constant that identifies 
               a relative position within the widget stack, as follows:
                  WIDGET_ZNORMAL      - Use the widget at the prop 
                                        container's  default location as the 
                                        retrieval reference point.  BREW 
                                        interprets  this to be the widget at 
                                        the top of the widget stack.
                  WIDGET_ZTOPMOST     - Use the widget at the top of the 
                                        widget stack  as the retrieval 
                                        reference   point.
                  WIDGET_ZBOTTOMMOST  - Use the widget at the bottom of the 
                                        widget stack as the retrieval reference 
                                        point.
               When 'pw' is NULL, the function will return either the top or 
               bottom widget on the stack, depending on the value of 'd'.  
               When 'pw' is NULL and 'd' is TRUE, IPROPCONTAINER_GetWidget() 
               will return the bottommost widget.  When 'pw' is NULL and 'd' 
               is FALSE, the function will return  the topmost widget.  
   pdwBefore:  Contains the padding value to be applied ahead of the widget.  
               If this value is positive, it indicates a proportional layout 
               value.  If this value is negative, it indicates an exact pixel 
               value.
   pdwAfter:   Contains the padding value to be applied behind the widget.  
               If this value is positive, it indicates a proportional layout 
               value.  If this value is negative, it indicates an exact pixel 
               value.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The spacing around the widget was successfully retrieved.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or 
                 refers to a widget that is not in the prop container's widget 
                 stack.
===/pre>  

Comments:
   Assuming a PropContainer has been set up with a number of widgets, the 
   following code can be used to obtain the padding before and after a certain 
   widget:
   
===pre>   
       IWidget *pWidget = NULL;
       int32 dwBefore, dwAfter;

       if (SUCCESS == IPROPCONTAINER_GetPadding (
                        pPropContainer, 
                        pWidget, 
                        &dwBefore,
                        &dwBefore)) {
          
         if (dwBefore > 0) {
            // this is a proportional layout value
         } else {dwBefore < 0)
            // -dwBefore is the number of pixels to be 
            // left clear ahead of this widget
         } else {
            // No leading padding in front of this widget
         }

         if (dwAfter > 0) {
            // this is a proportional layout value
         } else {dwAfter < 0)
            // -dwAfter is the number of pixels to be left 
            // clear behind of this widget
         } else {
            // No leading padding behind of this widget
         }

       }
===/pre>   

Side Effects: 
  None

See Also:
   IPROPCONTAINER_SetPadding()

===========================================================================

  IPROPCONTAINER_SetPadding()

Description:
   This function is used to specify the padding that will be applied 
   before and/or after a given widget in a PropContainer.  If the 
   PropContainer is laid out vertically, padding is applied above
   and/or below the specified widget.  If the PropContainer is laid 
   out horizontally, the padding  is applied to the left and/or right 
   of the given widget.
   
   The values specified for the padding may be positive or negative.
   Positive values tell the PropContainer to add a proportional spacer
   ahead of the widget.  Positive values specify the spacers's 
   proportion in relationship to other objects.  It is the numerator 
   of the fraction of this spacer's extent to the combined extents of 
   all other proportional objects managed by the container.  Negative 
   values are interpreted as exact pixel values for spacers.  To add a 
   spacer of a fixed size ahead or behind of a widget, pass in -x where 
   x is the width of the desired spacer in pixels.

   Note, the IPROPCONTAINER_SetPaddingExact() and 
   IPROPCONTAINER_SetPaddingProportional() macros should normally be 
   used to set spacing before and after a widget.  You'll need to use 
   this routine to set a proportional and an exact width spacer around 
   a given widget, though.

Prototype:
   int IPROPCONTAINER_SetPadding(
      IPropContainer *p, 
      IWidget *pw, 
      int32 dwBefore, 
      int32 dwAfter);

Parameters:
===pre>   
   p:          A pointer to an IPropContainer object.
   pw:         Identifies the widget within the prop container's widget 
               stack that will serve as a reference point for the widget to 
               be retrieved.  This widget may be expressed explicitly as a 
               widget pointer, or implicitly as a constant that identifies 
               a relative position within the widget stack, as follows:
                  WIDGET_ZNORMAL      - Use the widget at the prop 
                                        container's  default location as the 
                                        retrieval reference point.  BREW 
                                        interprets  this to be the widget at 
                                        the top of the widget stack.
                  WIDGET_ZTOPMOST     - Use the widget at the top of the 
                                        widget stack  as the retrieval 
                                        reference   point.
                  WIDGET_ZBOTTOMMOST  - Use the widget at the bottom of the 
                                        widget stack as the retrieval reference 
                                        point.
               When 'pw' is NULL, the function will return either the top or 
               bottom widget on the stack, depending on the value of 'd'.  
               When 'pw' is NULL and 'd' is TRUE, IPROPCONTAINER_GetWidget() 
               will return the bottommost widget.  When 'pw' is NULL and 'd' 
               is FALSE, the function will return  the topmost widget.  
   dwBefore:   Specifies the padding value to be applied ahead of the widget. 
               If this value is positive, it indicates a proportional layout 
               value.  If this value is negative, it indicates an exact pixel 
               value.
   dwAfter:    Specifies the padding value to be applied behind the widget. 
               If this value is positive, it indicates a proportional layout 
               value.  If this value is negative, it indicates an exact pixel 
               value.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The spacing around the widget was successfully set.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or 
                 refers to a widget that is not in the prop container's widget 
                 stack.
===/pre>  


Comments:
   Although you should normally call IPROPCONTAINER_SetPaddingExact() and 
   IPROPCONTAINER_SetPaddingProportional(), he following code demonstrates how 
   to add spacers around a given widget in a PropContainer:
   
===pre>   
       IWidget *pWidget = NULL;
       
       pWidget = IPROPCONTAINER_GetWidget (
                  pPropContainer, 
                  WIDGET_ZTOPMOST, 
                  TRUE, 
                  FALSE)) {
       if (pWidget) {
          
         // add a proportinal spacer of 1 part ahead 
         // of this widget, and an exact-width spacer 
         // of 5 pixels behind it.
         dwResult = IPROPCONTAINER_SetPadding(
                     pPropContainer, 
                     pWidget, 
                     1, 
                     -5);

       }
===/pre>   

Side Effects: 
   Calling IPROPCONTAINER_SetPadding() will cause the PropContainer to 
   adjust the object layout to include the newly added spacing.

See Also:
   IPROPCONTAINER_GetPadding()
   IPROPCONTAINER_SetPaddingExact()
   IPROPCONTAINER_SetPaddingProportional()

===========================================================================

  IPROPCONTAINER_SetPaddingExact()

Description:
   This function is used to specify the number of pixels worth of padding
   that will be applied before and/or after a widget in a PropContainer.  
   
Prototype:
   int IPROPCONTAINER_SetPaddingExact(
      IPropContainer *p, 
      IWidget *pw, 
      int32 dwBefore, 
      int32 dwAfter);

Parameters:
===pre>   
   p:          A pointer to an IPropContainer object.
   pw:         Identifies the widget within the prop container's widget 
               stack that will serve as a reference point for the widget to 
               be retrieved.  This widget may be expressed explicitly as a 
               widget pointer, or implicitly as a constant that identifies 
               a relative position within the widget stack, as follows:
                  WIDGET_ZNORMAL      - Use the widget at the prop 
                                        container's  default location as the 
                                        retrieval reference point.  BREW 
                                        interprets  this to be the widget at 
                                        the top of the widget stack.
                  WIDGET_ZTOPMOST     - Use the widget at the top of the 
                                        widget stack  as the retrieval 
                                        reference   point.
                  WIDGET_ZBOTTOMMOST  - Use the widget at the bottom of the 
                                        widget stack as the retrieval reference 
                                        point.
               When 'pw' is NULL, the function will return either the top or 
               bottom widget on the stack, depending on the value of 'd'.  
               When 'pw' is NULL and 'd' is TRUE, IPROPCONTAINER_GetWidget() 
               will return the bottommost widget.  When 'pw' is NULL and 'd' 
               is FALSE, the function will return  the topmost widget.  
   dwBefore:   Specifies the exact number of pixels worth of padding to be 
               applied ahead of the widget.
   dwAfter:    Specifies the exact number of pixels worth of padding to be 
               applied after of the widget.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The spacing around the widget was successfully set.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or 
                 refers to a widget that is not in the prop container's widget 
                 stack.
===/pre>  

Comments:
   Use this routine to add a spacer of a given width before and/or after
   a widget in the PropContainer.

Side Effects: 
   Calling IPROPCONTAINER_SetPaddingExact() will cause the PropContainer
   to adjust the  object layout to include the newly added spacing.

See Also:
   IPROPCONTAINER_GetPadding()
   IPROPCONTAINER_SetPadding()
   IPROPCONTAINER_SetPaddingProportional()

===========================================================================

  IPROPCONTAINER_SetPaddingProportional()

Description:
   This function is used to set a proportional spacer ahead and/or behind
   a given widget in a PropContainer.  Values passed as the leading
   or trailing padding are interpreted as the numerator of the fraction 
   of this spacers's extent to the combined extents of all other proportional 
   objects managed by the container.
   
Prototype:
   int IPROPCONTAINER_SetPaddingProportional(
      IPropContainer *p, 
      IWidget *pw, 
      int32 dwBefore, 
      int32 dwAfter);

Parameters:
===pre>   
   p:          A pointer to an IPropContainer object.
   pw:         Identifies the widget within the prop container's widget 
               stack that will serve as a reference point for the widget to 
               be retrieved.  This widget may be expressed explicitly as a 
               widget pointer, or implicitly as a constant that identifies 
               a relative position within the widget stack, as follows:
                  WIDGET_ZNORMAL      - Use the widget at the prop 
                                        container's  default location as the 
                                        retrieval reference point.  BREW 
                                        interprets  this to be the widget at 
                                        the top of the widget stack.
                  WIDGET_ZTOPMOST     - Use the widget at the top of the 
                                        widget stack  as the retrieval 
                                        reference   point.
                  WIDGET_ZBOTTOMMOST  - Use the widget at the bottom of the 
                                        widget stack as the retrieval reference 
                                        point.
               When 'pw' is NULL, the function will return either the top or 
               bottom widget on the stack, depending on the value of 'd'.  
               When 'pw' is NULL and 'd' is TRUE, IPROPCONTAINER_GetWidget() 
               will return the bottommost widget.  When 'pw' is NULL and 'd' 
               is FALSE, the function will return  the topmost widget.  
   dwBefore:   Specifies the proportional layout value of the padding to be 
               applied ahead of the widget.
   dwAfter:    Specifies the proportional layout value of the padding to be 
               applied after of the widget.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The spacing around the widget was successfully set.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or 
                 refers to a widget that is not in the prop container's widget 
                 stack.
===/pre>  

Comments:
   Use this routine to add a proportionally-sized spacer before and/or after
   a widget in the PropContainer.

Side Effects: 
   Calling IPROPCONTAINER_SetPaddingProportional() will cause the 
   PropContainer to adjust the  object layout to include the newly added spacing.

See Also:
   IPROPCONTAINER_GetPadding()
   IPROPCONTAINER_SetPadding()
   IPROPCONTAINER_SetPaddingExact()

===========================================================================

IPROPCONTAINER_TO_ICONTAINER()

Description:
   This function provides type safe casting of an IPropContainer pointer to an IContainer
   pointer and should be used when passing an IPropContainer pointer to functions that
   require a pointer to an IContainer.  This is safer than a simple cast, since the compiler
   will verify the pointer type.

Prototype:
   IContainer *IPROPCONTAINER_TO_ICONTAINER(IPropContainer *me)

Parameters:
===pre>
   me:  Pointer to an IPropContainer interface object.
===/pre>

Return Value:
   Returns 'me' cast to an IContainer pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IContainer Interface
   IPropContainer Interface

*/
#endif /* __AEEPROPCONTAINER_H__ */
