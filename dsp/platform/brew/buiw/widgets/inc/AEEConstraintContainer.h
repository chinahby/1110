/*
  ========================================================================

  FILE:  AEEConstraintContainer.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I define a container that constrains its 
  children according to particular constraint rules

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEECONSTRAINTCONTAINER_H__
#define __AEECONSTRAINTCONTAINER_H__

#include "AEE.h"
#include "AEEWidget.h"
#include "AEEContainer.h"

#include "bid\AEECLSID_CONSTRAINTCONTAINER.bid"
#include "bid\AEEIID_CONSTRAINTCONTAINER.bid"

#define CONSTRAINT_PARENT_LEFT      0x0100
#define CONSTRAINT_PARENT_RIGHT     0x0101
#define CONSTRAINT_PARENT_TOP       0x0102
#define CONSTRAINT_PARENT_BOTTOM    0x0103

#define CONSTRAINT_PARENT_WIDTH     0x0104
#define CONSTRAINT_PARENT_HEIGHT    0x0105

#define CONSTRAINT_PARENT_HCENTER     0x0106
#define CONSTRAINT_PARENT_VCENTER     0x0107

#define WIDGET_CENTER_SIZE_TO_FIT     0x0120

#define CONSTRAINT_PREV_LEFT        0x0200
#define CONSTRAINT_PREV_RIGHT       0x0201
#define CONSTRAINT_PREV_TOP         0x0202
#define CONSTRAINT_PREV_BOTTOM      0x0203


typedef struct  
{
   uint16 type;
   int16 offset;
} ConstraintElem;

typedef struct 
{
   ConstraintElem left;
   ConstraintElem right;
   ConstraintElem top;
   ConstraintElem bottom;
} Constraint;

typedef struct 
{
   Constraint constraint;
   boolean bVisible;
} WidgetConstraint;

typedef struct IConstraintContainer IConstraintContainer;
#define INHERIT_IConstraintContainer(iname) \
   INHERIT_IContainer(iname);\
   int      (*SetConstraint) (iname *, IWidget *pw, IWidget *piwBefore, const WidgetConstraint *pwc);\
   int      (*GetConstraint) (iname *, IWidget *pw, WidgetConstraint *pwc)

AEEINTERFACE(IConstraintContainer) {
   INHERIT_IConstraintContainer(IConstraintContainer);
};

#define ICONSTRAINTCONTAINER_AddRef(p)                 AEEGETPVTBL(p,IConstraintContainer)->AddRef(p)
#define ICONSTRAINTCONTAINER_Release(p)                AEEGETPVTBL(p,IConstraintContainer)->Release(p)
#define ICONSTRAINTCONTAINER_QueryInterface(p,i,p2)    AEEGETPVTBL(p,IConstraintContainer)->QueryInterface(p,i,p2)
#define ICONSTRAINTCONTAINER_Invalidate(p,pw,prc,f)    AEEGETPVTBL(p,IConstraintContainer)->Invalidate(p,pw,prc,f)
#define ICONSTRAINTCONTAINER_Locate(p,pw,ppr,prc)      AEEGETPVTBL(p,IConstraintContainer)->Locate(p,pw,ppr,prc)
#define ICONSTRAINTCONTAINER_Remove(p,pw)              AEEGETPVTBL(p,IConstraintContainer)->Remove(p,pw)         
#define ICONSTRAINTCONTAINER_SetConstraint(p,pw,pb,pc) AEEGETPVTBL(p,IConstraintContainer)->SetConstraint(p,pw,pb,pc) 
#define ICONSTRAINTCONTAINER_GetConstraint(p,pw,pc)    AEEGETPVTBL(p,IConstraintContainer)->GetConstraint(p,pw,pc)
#define ICONSTRAINTCONTAINER_GetWidget(p,pw,d,w)       AEEGETPVTBL(p,IConstraintContainer)->GetWidget(p,pw,d,w) 
static __inline int ICONSTRAINTCONTAINER_Insert(IConstraintContainer *po, IWidget *pw ,IWidget *pib, const WidgetConstraint *pc) {
   return AEEGETPVTBL(po,IConstraintContainer)->Insert(po,pw,pib,(const void *)pc);
}

static __inline IContainer *ICONSTRAINTCONTAINER_TO_ICONTAINER(IConstraintContainer *me) {
   return (IContainer *)(void*)me;
}


/////////////////////////////////////////////////////////////////////////////

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Constraint Container Interface and Class IDs

Description:
   Each class and interface within the widget framework is identified by a unique
   ID.  These IDs are used throughout the framework to distinguish between various
   types of objects, such as containers and widgets.

   The constraint container supports both an interface and the creation of class
   object instances.  The constraint container interface or a new instance of the
   constraint container class object may be created by calling ISHELL_CreateInstance()
   and passing in the desired class id of the object.  The object that is created is,
   itself, a container and may utilize those APIs defined for either the container or
   constraint container interfaces.

   The constraint container recognizes the following set of class and interface IDs.

Definition:

Identifier                     Definition
----------                     -----------------------------------
AEECLSID_CONSTRAINTCONTAINER:  Identifies the class ID of the constraint container
AEEIID_CONSTRAINTCONTAINER:    Identifies the interface ID of the constraint container

Comments:
   None

See Also:
   IConstraintContainer Interface
   ISHELL_CreateInstance()
      
=============================================================================
Constraint types

Description:
   The constraint container defines a small set of constants that are used to establish
   points of constraint for determining a widget's size or position.  These points of
   constraint allow an object's size or position to be expressed in terms relative to
   another object.  For example, the layout of an object could be constrained to the
   placement of other objects using the following vocabulary:
   
===pre>   
      - Place the left edge 2 pixels to the right of the right side of the previous object
      - Place the right edge 2 pixels to the left of the right side of the parent object
      - Place the top edge at 50% of the height of the parent object
      - Place the bottom edge 2 pixels above the bottom of the parent object
===/pre>   
   
   These types of constraint relationships, in conjunctions with the Constraint and
   ConstraintElem data types, may be expressed by using the constants listed in the table
   below:

===pre>
Definition                 Description
--------------             -------------------------------------------------------
CONSTRAINT_PARENT_BOTTOM:  Indicates that an object is to be positioned relative to the
                           bottom of its parent.  For example, the bottom of a widget
                           could be placed 'x' pixels from the bottom of the parent's
                           bounding rectangle -- effectively anchoring the widget to
                           the bottom of its container.

CONSTRAINT_PARENT_TOP:     Indicates that an object is to be positioned relative to the
                           top of its parent.  For example, the top of a widget could
                           be placed 'x' pixels from the top of the parent's bounding
                           rectangle -- effectively anchoring the widget to the top of
                           its container.

CONSTRAINT_PARENT_LEFT:    Indicates that an object is to be positioned relative to the
                           left side of its parent.  For example, the left side of a
                           widget could be placed 'x' pixels from the left side of the
                           parent's bounding rectangle -- effectively anchoring the widget
                           to the left side of its container.

CONSTRAINT_PARENT_RIGHT:   Indicates that an object is to be positioned relative to the
                           right side of its parent.  For example, the right side of a
                           widget could be placed 'x' pixels from the right side of the
                           parent's bounding rectangle -- effectively anchoring the widget
                           to the right side of its container.

CONSTRAINT_PARENT_WIDTH:   Indicates that an object is to be positioned relative to a
                           percentage of the width of its parent.  For example, the left
                           side of a widget could be placed at a position that is at 50%
                           of the container object's extent -- i.e. the left edge is
                           aligned at the middle of the container.

CONSTRAINT_PARENT_HEIGHT:  Indicates that an object is to be positioned relative to a
                           percentage of the height of its parent.  For example, the bottom
                           of a widget could be placed at a position that is at 33%
                           of the container object's extent -- i.e. the bottom edge is
                           aligned one third of the way down from the top of the container.
                              
CONSTRAINT_PARENT_HCENTER: Indicates that an object is to be positioned horizontally
                           centered within its parent. For example, an 40 pixel wide
                           object would be positioned 20 pixels from the left of an
                           80 pixel wide parent.

CONSTRAINT_PARENT_VCENTER: Indicates that an object is to be positioned vertically
                           centered within its parent. For example, an 40 pixel high
                           object would be positioned 20 pixels from the top of an
                           80 pixel high parent.
                              
CONSTRAINT_PREV_BOTTOM:    Indicates that an object is to be positioned relative to the
                           bottom edge of the previous visible object in the widget list.
                           For example, the top of a widget could be placed 'x' pixels from
                           the bottom of the widget that was just previously declared.

CONSTRAINT_PREV_TOP:       Indicates that an object is to be positioned relative to the
                           top edge of the previous visible object in the widget list. 
                           For example, the bottom of a widget could be placed 'x' pixels
                           from the top of the widget that was just previously declared.

CONSTRAINT_PREV_LEFT:      Indicates that an object is to be positioned relative to the
                           left edge of the previous visible object in the widget list.
                           For example, the right side of a widget could be placed 'x'
                           pixels from the left side of the widget that was just previously
                           declared.

CONSTRAINT_PREV_RIGHT:     Indicates that an object is to be positioned relative to the
                           right edge of the previous visible object in the widget list.
                           For example, the left side of a widget could be placed 'x'
                           pixels from the right side of the widget that was just previously
                           declared.

WIDGET_CENTER_SIZE_TO_FIT: Indicates that an object is to be positioned relative to the 
                           exact center of the container.  If this constraint is set for either
                           the left or right constraint, the widget will be centered to the 
                           midpoint of the container's width, and the widget will be sized to
                           its preferred width.  The same is true for the top and bottom 
                           constraint except the widget will be centered to the container's
                           height, and the widget will be sized to its preferred height.
                           If this constraint type is chosen for all constraints, the widget
                           will sized to its preferred extent and will be placed in the dead
                           center of the container.  
===/pre>

Members:
   None
   
Comments:
   None

See Also:
   ConstraintElem
   Constraint
   
=============================================================================

ConstraintElem

Description:
   This data type identifies a single constraint element for a widget, which defines how
   the position of that widget is to be constrained by another object.  A ConstraintElem
   merely defines a reference point for a single aspect of a widget's position -- for
   example, a reference point for placing the right side of a bitmap widget, or the top
   edge of a checkbox widget.  A single positional relationship will often suffice where
   an object's preferred extent has statically been defined.  However, to truly constrain
   the dimensions of an object, multiple ConstraintElem are required to specify the
   location of the object's left, right, top and bottom coordinates, as defined by the
   Constraint data structure.
   
   The 'type' and 'offset' fields have a binding relationship in which they together define
   a point of constraint for a target object.  For example, a ConstraintElem with 'type'
   equal to CONSTRAINT_PARENT_BOTTOM and 'offset' equal to -2 would define a coordinate that
   is two pixels up from the bottom of the object's parent.  As part of a Constraint data
   structure, this relationship might be used to constrain the position of a widget's bottom
   coordinate.

Definition:
   typedef struct {
      uint16 type;
      int16 offset;
   } ConstraintElem;

Members:
===pre>
   type:    This constant describes either:
                i) The type of positional relationship the constraint element will have
                   against a coordinate of another object (CONSTRAINT_PARENT_BOTTOM,
                   CONSTRAINT_PARENT_TOP, CONSTRAINT_PREV_LEFT, etc).  For example,
                   when 'type' is equal to CONSTRAINT_PARENT_RIGHT, the ConstraintElem
                   defines a constraint point relative to the right side of the widget's
                   parent (generally, the container object).  Or...
               ii) How the coordinate being described by the ConstraintElem should be
                   sized in relation to the object's own extent, as defined by one of
                   the following constants:
                              
                      WIDGET_SIZE_TO_FIT  - The constraint container will grant an
                                            extent to this object, which is sized to fit
                                            the widget's preferred extent.  For example,
                                            if the ConstraintElem is describing the top
                                            coordinate of an object and 'type' is equal to
                                            WIDGET_SIZE_TO_FIT, the widget will be laid
                                            out in the container with a height equal to the
                                            preferred vertical extent of the widget.
                      WIDGET_KEEP_EXTENT  - The constraint container will grant an
                                            extent to this object, which is exactly the
                                            same size as the widget's extent.  For example,
                                            if the ConstraintElem is describing the left
                                            coordinate of an object and 'type' is equal to
                                            WIDGET_KEEP_EXTENT, the widget will be laid
                                            out in the container with a width equal to the
                                            existing horizontal extent of the widget.

   offset:  The distance in pixels from a single coordinate of the reference object, as
            defined by the 'type' field.  Positive values mean to the right and down from
            the reference object, and negative values mean to the left and up from the
            reference object.
===/pre>

Comments:
   None

See Also:
   Constraint types
   Constraint
   
=============================================================================

Constraint

Description:
   This data type defines constraint elements for each coordinate of an object -- top,
   bottom, left and right -- thereby allowing the position and size of that object to
   be fully determined in terms relative to other objects or itself.  To illustrate a
   very basic example, consider an widget whose position and size is entirely determined
   by the dimensions of the display.  Regardless of the device, the desire is to always
   place this object centered on the display, inset by 4 pixels on all sides, like so:
   
===pre>
                                        +------------------+
                                        |    display #2    |
                                        |  +------------+  |
   +----------------------------+       |  |            |  |
   |         display #1         |       |  |            |  |
   |  +----------------------+  |       |  |            |  |
   |  |                      |  |       |  |            |  |
   |  |                      |  |       |  |            |  |
   |  |                      |  |       |  |            |  |
   |  |        widget        |  |       |  |   widget   |  |
   |  |                      |  |       |  |            |  |
   |  |                      |  |       |  |            |  |
   |  |                      |  |       |  |            |  |
   |  +----------------------+  |       |  |            |  |
   |                            |       |  |            |  |
   +----------------------------+       |  |            |  |
                                        |  +------------+  |
                                        |                  |
                                        +------------------+
===/pre>   
   
   To accomplish this, the application would create a constraint container with the
   exact dimensions of the device display, then create the widget and specify
   constraints on all four sides relative to the parent container, in the following
   manner:
   
===pre>
       Widget Coordinate       type                       offset
       -----------------       ----                       ------
             top               CONSTRAINT_PARENT_TOP         4
             bottom            CONSTRAINT_PARENT_BOTTOM     -4
             left              CONSTRAINT_PARENT_LEFT        4
             right             CONSTRAINT_PARENT_RIGHT      -4
===/pre>   

   The collection of all four constraint coordinates together define the Constraint
   data structure.

Definition:
   typedef struct {
      ConstraintElem left;
      ConstraintElem right;
      ConstraintElem top;
      ConstraintElem bottom;
   } Constraint;

Members:
===pre>
   left:    Defines how the left coordinate of the object should be constrained by
            another object or itself.
   right:   Defines how the right coordinate of the object should be constrained by
            another object or itself.
   top:     Defines how the top coordinate of the object should be constrained by
            another object or itself.
   bottom:  Defines how the bottom coordinate of the object should be constrained by
            another object or itself.
===/pre>

Comments:
   To indicate that a particular constraint coordinate should be ignored, the 'type'
   field of a ConstraintElem should be set to WIDGET_KEEP_EXTENT.  For example, a
   radio button may have constraints defined for its left, right and top bounds, but
   the bottom coordinate would be determined by the existing height of the widget.

See Also:
   Constraint types
   ConstraintElem
   
   
=============================================================================

WidgetConstraint

Description:
   This data type defines special attributes for any widget that is to be managed by a
   constraint container.  These attributes identify whether or not the widget should be
   factored into the container's layout calculations and -- if it should -- how the
   widget will be constrained by other objects when it is laid out in the container.
   
Definition:
   typedef struct {
      Constraint  constraint;
      boolean     bVisible;
   } WidgetConstraint;

Members:
===pre>
   constraint:  Specifies how the widget should be laid out in the container as a complete
                set of constraint points, each defining an element of constraint for one of
                the widget's edges.
                                        
   bVisible:    Boolean value specifying whether or not this widget should be displayed.
                If the widget is not displayed, it is also not factored into layout
                calculations for the container.  For example, say you have 3 widgets in a
                constraint container, and the first and third widgets are visible, but the
                second is not visible.  If the third widget has a constraint type based on
                the previous widget (e.g. CONSTRAINT_PREV_BOTTOM), then it will use the
                first widget's position to calculate its relative position, and not the
                second widget's position because the second widget is not visible.
===/pre>
Comments:
   None

See Also:
   Constraint types
   ConstraintElem
   Constraint
   ICONSTRAINTCONTAINER_SetConstraint()
   ICONSTRAINTCONTAINER_GetConstraint()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IConstraintContainer Interface

Description:
   The constraint container is derived from the base container object and is used to manage
   the widgets within a container where those widgets are laid out relative to either the
   container itself, or to each of the other objects held in the container.  Objects are laid
   out in the order that the widgets appear in the container's widget stack, with the position
   of each object determined by the set of constraint elements -- one element per object
   coordinate.  Constraint elements describe the location of a particular object coordinate,
   relative to the placement of the object's parent, the previous visible object in
   the constraint container's widget stack, or the object's existing or preferred size.
   For example, the left coordinate of an object may be described as being "2 pixels from the
   right coordinate of the previous object", or, the bottom of an object might be described as
   "0 pixels from the bottom coordinate of the object's parent".  As the size and location of
   the constraint container changes, so do the sizes and locations of the objects held within.
   Likewise, as objects are added or removed from the container, or as individual constraint
   points are modified, the dimensions and placement of other objects will change.

   For example, an application may wish to layout a pair of pushbuttons, side-by-side
   and anchored to the bottom of the display, like so: 
   
===pre>
                    :   
   |                :                 |
   |                                  |
   |            Container             |
   |                                  |
   |+---------------++---------------+|
   ||   button 1    ||   button 2    ||
   |+---------------++---------------+|
   +----------------------------------+
===/pre>

   Regardless of the dimensions of the container, the objects within will always be
   sized according to the defined layout for these objects, relative to the positions
   of other objects, or the constraint container itself.  So, for example, if the
   constraint container in the above example were to change size, button1 and button 2
   could maintain their relative size and position, like so:
   
===pre>
                              :             
   |                          :                           |
   |                                                      |
   |                 Constraint Container                 |
   |                                                      |
   |+-------------------------++-------------------------+|
   ||        button 1         ||        button 2         ||
   |+-------------------------++-------------------------+|
   +------------------------------------------------------+
===/pre>

   The constraint container provides a loose vocabulary for defining the points of
   constraint for determining a widget's size or position.  These points of constraint
   allow an object's size or position to be expressed in terms relative to the size or
   position to another object.  Continuing the above example, the bottom of button 1
   could be defined to be "0 pixels from the bottom of its parent", and its right side
   could be defined to be "50% of the parent's width".
   
   In defining a point of constraint, each constant identifies the object from which
   relevance should be established -- this object being the parent of the object
   whose position is being determined, the object prior to that object, or the object
   itself.  Consider a constraint container that is to manage a pushbutton, a checkbox,
   two radio buttons and a bitmap -- all created in exactly that order.  The figure
   below illustrates the relationship between the container and each of the widgets
   created inside.

===pre>   
   Constraint
    Container ---+                              NULL <---+
                 |                                       |
                 |---- pushbutton      +---- previous ---+ <---+
                 |                                             |
                 |---- checkbox        +---- previous ---------+ <---+
                 |                                                   |
                 |---- radio button 1  +---- previous ---------------+ <---+
                 |                                                         |
                 |---- radio button 2  +---- previous ---------------------+ <---+
                 |                                                               |
                 +---- bitmap          +---- previous ---------------------------+
===/pre>   

   The constraint container is the parent of each of the 5 widgets.  The widgets
   themselves are stored as a linked list of widgets, with a pointer back to the
   previous widget.  Therefore, since the pushbutton is the first widget created,
   the previous widget is NULL.  The checkbox, being created next, points back to
   the pushbutton, the first radio button points to the checkbox, the second radio
   button to the first, and so on.  To declare, in the above example, that the
   checkbox is to be positioned relative to the top of the previous object, would be
   to indicate that it is to be positioned relative to the pushbutton, since the
   pushbutton is the object declared previous to the checkbox in the linked list.
   
   Note, however, that objects are always laid out relative to other VISIBLE objects.
   So, if a constraint point has been defined relative to the previous object, and that
   object is invisible, BREW will traverse over the widget stack until it finds the
   first visible widget, and base the constraint point on that widget's layout.  In the
   above example, let's say that the top of the bitmap widget is to be laid out relative
   to the bottom of the previous widget -- ordinarily, the second radio button.  Now,
   let's say that both of the radio buttons are invisible.  In this case, the previous
   visible object would be the checkbox, and the top of the bitmap widget will be laid
   out relative to the checkbox, rather than either radio button.

Supported Events:
   The constraint container passes all received events to the base container's event handler,
   where border, focus and widget events will be processed (see the description of supported
   events in AEEContainer.h for more information).  If, in the process of handling the event,
   the constraint container's client rectangle changes, the layout of objects within the
   container will be rearranged to accommodate the new container size.

Comments:
   None
   
See Also:
   Constraint types
   ConstraintElem
   Constraint

===========================================================================

ICONSTRAINTCONTAINER_AddRef()

Description:

   This function increments the reference count of the IConstraintContainer interface object,
   allowing the interface to be shared by multiple callers.  The object is freed when the
   reference count reaches 0 (zero).

Prototype:

   uint32 ICONSTRAINTCONTAINER_AddRef(IConstraintContainer *p);

Parameters:
===pre>
   p:  Pointer to the IConstraintContainer interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IConstraintContainer interface object.

Comments:
   None

Side Effects:
   None

See Also:
   ICONSTRAINTCONTAINER_Release()
   
=======================================================================

ICONSTRAINTCONTAINER_Release()

Description:

   This function decrements the reference count of the IConstraintContainer interface object.
   The object is freed from memory and is no longer valid once its reference count reaches 0
   (zero).

Prototype:

   uint32 ICONSTRAINTCONTAINER_Release(IConstraintContainer *p);

Parameters:
===pre>
   p:  Pointer to the IConstraintContainer interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IConstraintContainer interface object.  If
   the returned reference count is 0 (zero), the object has been freed and will no longer be
   valid.

Comments:
   None

Side Effects:
   None

See Also:
   ICONSTRAINTCONTAINER_AddRef()

=======================================================================

ICONSTRAINTCONTAINER_QueryInterface()

Description: 
   This function queries an interface for an alternate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int ICONSTRAINTCONTAINER_QueryInterface(IConstraintContainer *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IConstraintContainer interface object.
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

ICONSTRAINTCONTAINER_Invalidate()

Description: 
   This function will invalidate all or part of a widget within a constraint container.
   The application identifies the container and widget, and may also specify a rectangle
   within that widget in order to invalidate a portion of the widget.  By default, the
   entire extent of the widget will be invalidated.
   
Prototype:

   void ICONSTRAINTCONTAINER_Invalidate(IConstraintContainer *p, IWidget *pw, const AEERect *prc, uint32 f)

Parameters:
===pre>
   p:    Pointer to the IConstraintContainer interface object that serves as the parent to the
         widget to invalidate.
   pw:   Pointer to the IWidget object to be invalidated, or a constant that implies a particular
         widget within the constraint container.  The constants used to identify a relative position
         for the widget to be invalidated, are defined as follows:
                              
             WIDGET_ZTOPMOST     -  Requests that the widget at the top of the container's
                                    widget stack should be invalidated.
             WIDGET_ZBOTTOMMOST  -  Requests that the widget at the bottom of the container's
                                    widget stack should be invalidated.
             WIDGET_ZNORMAL      -  Requests that the widget placed at the container's most
                                    normal layout location be invalidated.  The constraint
                                    container treats the WIDGET_ZTOPMOST position as the "normal"
                                    location.
   
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
   Calling ICONSTRAINTCONTAINER_Invalidate() will trigger the invalidation to travel up through
   container's hierarchy, first to its parent, then to its parent, etc. so that the
   invalidation rectangle may be applied to each object in the chain.
   
   Callers should set the ICIF_EXTENT flag of the 'f' parameter when calling the function
   following a change to the widget's extent.  

Side Effects: 
   Calling ICONSTRAINTCONTAINER_Invalidate() with the ICIF_EXTENT flag on will cause the layout
   of the objects within the constraint container to change, using the widget's new extent.

See Also:
   IConstraintContainer Interface
   IWidget Interface
   ICONTAINER_Invalidate()
   IWIDGET_Invalidate()

=======================================================================

ICONSTRAINTCONTAINER_Locate()

Description: 
   This function will find the location of a particular widget, returning that widget's
   rectangle relative to the widget's root container.  The function will also, optionally,
   return a pointer to the root container.

Prototype:

   int ICONSTRAINTCONTAINER_Locate(IConstraintContainer *p, IWidget *pw, IContainer **ppr, AEERect *prc)

Parameters:
===pre>
   p:    Pointer to the IConstraintContainer interface object that serves as the parent to the widget
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
   The rectangle referenced by 'prc' MUST be initialized prior to calling ICONSTRAINTCONTAINER_Locate(),
   otherwise the results will be undefined.

   The pointer passed in the 'ppr' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   IConstraintContainer Interface
   IWidget Interface
   ICONTAINER_Locate()


=======================================================================

ICONSTRAINTCONTAINER_Insert()

Description:
   This function allows an application to insert a widget into the constraint container's
   widget stack -- at the top of the stack, at the bottom, or some place inbetween.  By
   default, widgets will be inserted at the top of the stack.  However, an application may
   choose to instead insert widgets anywhere within the container's widget stack.  For
   example, consider an application that wishes to keep one widget always at the top of the
   container's widget stack so that the layout of this widget is always based on the location
   of the topmost visible widget.  As new widgets are added to the screen, the application
   wants to make sure that the fixed widget remains on the top of the stack, so each time a
   new widget is added it will be inserted before the fixed widget, passing a pointer to the
   fixed widget in the function's 'pb' parameter.
   
Prototype:
   int ICONSTRAINTCONTAINER_Insert(IConstraintContainer *p, IWidget *pw, IWidget *pb, const WidgetConstraint *pwc);

Parameters:
===pre>   
   p:   A pointer to an IConstraintContainer object.
   pw:  Pointer to the widget to be inserted into the constraint container's widget stack.
   pb:  Indicates where in the constraint container's widget stack that the new widget
        should be inserted.  This value may be expressed as either a pointer to a 
        widget already in the widget stack, in which case the new widget will be
        inserted immediately before the widget pointed to by 'pb' -- or as  a
        constant that identifies a relative position within the widget stack, as
        follows:
            WIDGET_ZNORMAL     -  Allow BREW to place the inserted widget at the
                                  default location.  For a constraint container,
                                  this places the new widget at the top of container's
                                  widget stack.
            WIDGET_ZTOPMOST    -  The new widget should be inserted as the topmost
                                  widget in the container's widget stack.
            WIDGET_ZBOTTOMMOST -  The new widget should be inserted at the bottommost
                                  widget in the container's widget stack.
   pwc: Pointer to a WidgetConstraint data structure, containing the constraint value for
        the inserted widget, as well as its visibility attribute.  If this parameter is set to NULL,
        pw will be inserted with WIDGET_KEEP_EXTENT and an offset of zero for all constraints, and
        visibility will be set to TRUE.  
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully inserted into the constraint container's widget stack.
   EBADPARM  --  The widget pointer passed in 'pb' either does not exist, or refers to a widget
                 that is not in the constraint container's widget stack.
   ENOMEMORY --  The widget stack could not grow to accommodate a new widget.
===/pre>   

Comments:
   None

Side Effects: 
   Calling ICONSTRAINTCONTAINER_Insert() will cause the constraint container to adjust the object
   layout to include the newly added widget.
   
See Also:
   IConstraintContainer Interface
   IWidget Interface
   WidgetConstraint
   ICONSTRAINTCONTAINER_Remove()

===========================================================================

ICONSTRAINTCONTAINER_Remove()

Description:
   This function allows an application to remove a widget from the constraint container's
   widget stack.  The widget to be removed may be specified explicitly, by its widget pointer,
   or implicitly by a constant that identifies the widget's relative position within the
   container's widget stack.
   
Prototype:
   int ICONSTRAINTCONTAINER_Remove(IConstraintContainer *p, IWidget *pw);

Parameters:
===pre>   
   p:   A pointer to an IConstraintContainer object.
   pw:  Identifies the widget that should be removed from the constraint container's widget
        stack. This widget may be expressed explicitly as a widget pointer, or implicitly as 
        a constant that identifies a relative position within the widget stack, as follows:
             WIDGET_ZNORMAL      - Remove the default widget, which BREW interprets to be
                                   the widget at the top of the widget stack.
             WIDGET_ZTOPMOST     - Remove the widget at the top of the widget stack.
             WIDGET_ZBOTTOMMOST  - Remove the widget at the bottom of the widget stack.

===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully removed from the constraint container's widget
                 stack.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a widget
                 that is not in the constraint container's widget stack.
===/pre>   

Comments:
   None

Side Effects: 
   Calling ICONSTRAINTCONTAINER_Remove() will cause the constraint container to adjust the
   object layout to consume the space surrendered by the removed widget.
   
See Also:
    IConstraintContainer Interface
    IWidget Interface
    ICONSTRAINTCONTAINER_Insert()

===========================================================================

ICONSTRAINTCONTAINER_SetConstraint()

Description:
   This function will change the constraint values or visible attributes of a given widget,
   and will optionally relocate that widget to a different location in the constraint
   container's widget stack.  The new constraint and visibility values are passed to the
   function in a WidgetConstraint structure, pointed to by the 'pwc' parameter.  The widget
   to receive the new values may be specified explicitly, by its widget pointer, or implicitly
   by a constant that identifies the widget's relative position within the container's widget
   stack.  The 'pb' parameter controls whether or not the widget's location in the stack will
   move.  Passing the constant WIDGET_ZNORMAL leaves the widget where it currently stands,
   without altering the order of the stack.  Passing WIDGET_ZTOPMOST or WIDGET_ZBOTTOMMOST
   will move the widget to the top or bottom of the stack, while passing an explicit widget
   pointer will move the target widget to the stack location before the widget passed in 'pb'.
   
Prototype:
   int ICONSTRAINTCONTAINER_SetConstraint(IConstraintContainer *p, IWidget *pw, IWidget *pb, const WidgetConstraint *pwc);

Parameters:
===pre>   
   p:   A pointer to an IConstraintContainer object.
   pw:  Identifies the widget in the constraint container's widget stack that will be
        the target of the constraint change and (optionally) relocation.  This widget
        may be expressed explicitly as a widget pointer, or implicitly as  a constant
        that identifies a relative position within the widget stack, as follows:
             WIDGET_ZNORMAL      - The target widget is the default widget, which BREW
                                   interprets to be the widget at the top of the widget
                                   stack.
             WIDGET_ZTOPMOST     - The target widget is the widget at the top of the
                                   widget stack.
             WIDGET_ZBOTTOMMOST  - The target widget is the widget at the bottom of the
                                   widget stack.
   pb:  Identifies a widget in the  constraint container's widget stack that will serve
        as the relocation spot for the target widget.  When this parameter is passed as
        an explicit pointer to a widget in the container's widget stack, the target
        widget will be moved to the stack location that is _before_ this widget.  The
        new location for the target widget may be passed implicitly as a relative
        location in the stack by passing any of the following constants:
            WIDGET_ZNORMAL      - Specifies that the target widget should NOT be moved
                                  and should instead remain at its current location in
                                  the widget stack.
            WIDGET_ZTOPMOST     - The target widget should be moved to the top of the
                                  widget stack.
            WIDGET_ZBOTTOMMOST  - The target widget should be moved to the bottom of
                                  the widget stack.
   
   pwc: Pointer to a WidgetConstraint data structure, containing the new constraint
        value for the target widget, as well as its visibility attribute.

===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The constraint attributes of the widget were successfully changed.
   EBADPARM  --  The widget pointers passed in 'pw' and/or 'pb' either do not exist, or
                 refer to widgets that are not in the constraint container's widget
                 stack.
===/pre>   

Comments:
   If ICONSTRAINTCONTAINER_SetConstraint() is being called to change the location of the
   widget within the constraint container's widget stack without changing the constraint
   attributes of the widget, the caller MUST still pass in a pointer to the correct
   constraint attributes in the 'pwc' parameter.  In such cases, the application should
   first call ICONSTRAINTCONTAINER_GetConstraint() to retrieve the current constraint
   attributes, then call ICONSTRAINTCONTAINER_SetConstraint() passing back the results
   received from ICONSTRAINTCONTAINER_GetConstraint().

Side Effects: 
   Calling ICONSTRAINTCONTAINER_SetConstraint() will cause the constraint container to adjust
   the object layout to accommodate the altered constraints or reordering of the objects in the
   container's widget stack.
   
See Also:
    IConstraintContainer Interface
    IWidget Interface
    WidgetConstraint
    ICONSTRAINTCONTAINER_GetConstraint()


===========================================================================

ICONSTRAINTCONTAINER_GetConstraint()

Description:
   This function retrieves a widget's constraint values, returning a copy of those
   values in the WidgetConstraint data structure pointed to by 'pwc'.  The data returned
   in 'pwc' will include the widget's constraint layout value, as well as its current
   visibility attribute.
   
Prototype:
   int ICONSTRAINTCONTAINER_GetConstraint(IConstraintContainer *p, IWidget *pw, WidgetConstraint *pwc);

Parameters:
===pre>   
   p:   A pointer to an IConstraintContainer object.
   pw:  Identifies the widget in the constraint container's widget stack whose
        constraint values should be retrieved.  This widget may be expressed
        explicitly as a widget pointer, or implicitly as  a constant that identifies
        a relative position within the widget stack, as follows:
             WIDGET_ZNORMAL     -  Retrieve the constraint values for the default
                                   widget, which BREW interprets to be the widget at
                                   the top of the widget stack.
             WIDGET_ZTOPMOST     - Retrieve the constraint values for the widget at
                                   the top of the  widget stack.
             WIDGET_ZBOTTOMMOST  - Retrieve the constraint values for the widget at
                                   the bottom of the  widget stack.
   pwc: Pointer to a WidgetConstraint data structure, into which the function will copy
        the current constraint values of the widget.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The constraint values of the widget were successfully retrieved.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a
                 widget that is not in the constraint container's widget stack.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    IConstraintContainer Interface
    IWidget Interface
    WidgetConstraint
    ICONSTRAINTCONTAINER_SetConstraint()

===========================================================================

ICONSTRAINTCONTAINER_GetWidget()

Description:
   This function provides facilities that allow an application to retrieve the widgets stored
   in a constraint container's widget stack.  The function may be instructed to return the widget
   from an explicit location, such as the top or bottom of the stack, or from some position relative
   to another widget in the widget stack -- for example, returning the widget that appears before
   or after another widget in the stack.  An application can easily iterate over all of the widgets
   in the constraint container by repeatedly calling ICONSTRAINTCONTAINER_GetWidget() and passing in
   the widget pointer returned from the previous call.
   
Prototype:
   IWidget *ICONSTRAINTCONTAINER_GetWidget(IConstraintContainer *p, IWidget *pw, boolean d, boolean w);

Parameters:
===pre>   
   p:   A pointer to an IConstraintContainer object.
   pw:  Identifies the widget within the constraint container's widget stack that will serve as a
        reference point for the widget to be retrieved.  This widget may be expressed explicitly as
        a widget pointer, or implicitly as a constant that identifies a relative position within
        the widget stack, as follows:
            WIDGET_ZNORMAL      - Use the widget at the constraint container's default location as
                                  the retrieval reference point.  BREW interprets this to be the
                                  widget at the top of the widget stack.
            WIDGET_ZTOPMOST     - Use the widget at the top of the widget stack as the retrieval
                                  reference point.
            WIDGET_ZBOTTOMMOST  - Use the widget at the bottom of the widget stack as the retrieval
                                  reference point.
        When 'pw' is NULL, the function will return either the top or bottom widget on the stack,
        depending on the value of 'd'.  When 'pw' is NULL and 'd' is TRUE, the function will return
        the bottommost widget.  When 'pw' is NULL and 'd' is FALSE, the function will return the
        topmost widget.  
   d:   Specifies whether the function should retrieve the next widget in the widget stack, or the
        previous widget on the stack -- always relative to the reference widget determined by the
        value passed in the 'pw' parameter.  When 'd' is TRUE the function will retrieve the next
        widget on the stack (i.e. the widget closer to the top of the stack).  When 'd' is FALSE
        the function will retrieve the previous widget on the stack (i.e. the widget closer to the
        bottom of the stack).  Note, however, that when 'pw' is passed as NULL, the 'd' parameter
        explicitly indicates either the topmost or bottommost widget, as described above.
   w:   Specifies whether or not the function should wrap when attempting to retrieve the next or
        previous widget from the top or bottom of the constraint container's widget stack.  When
        'w' is TRUE, the function will wrap at either end of the stack, otherwise the function
        will return NULL indicating that there are no more widgets to be retrieved.
===/pre>   
   
Return Value:
   A pointer to a widget in the constraint container's widget stack, or NULL when there is no
   widget to be returned. 

Comments:
   Though the ICONSTRAINTCONTAINER_GetWidget() function returns a pointer to a widget, the
   function does not add a reference to the returned IWidget for the caller.

   An application may iterate over the entire widget stack -- from bottom to top -- with the
   following code:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = ICONSTRAINTCONTAINER_GetWidget (pConstraintContainer, pWidget, TRUE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once ICONSTRAINTCONTAINER_GetWidget() returns NULL
===/pre>   
   
   To iterate over all the widgets on the widget stack from top to bottom, the application's
   code would look like the following:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = ICONSTRAINTCONTAINER_GetWidget (pConstraintContainer, pWidget, FALSE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once ICONSTRAINTCONTAINER_GetWidget() returns NULL
===/pre>   

Side Effects: 
  None

See Also:
   IConstraintContainer Interface
   IWidget Interface

===========================================================================

ICONSTRAINTCONTAINER_TO_ICONTAINER()

Description:
   This function provides type safe casting of an IConstraintContainer pointer to an IContainer
   pointer and should be used when passing an IConstraintContainer pointer to functions that
   require a pointer to an IContainer.  This is safer than a simple cast, since the compiler
   will verify the pointer type.

Prototype:
   IContainer *ICONSTRAINTCONTAINER_TO_ICONTAINER(IConstraintContainer *me)

Parameters:
===pre>
   me:  Pointer to an IConstraintContainer interface object.
===/pre>

Return Value:
   Returns 'me' cast to an IConstraintContainer pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IContainer Interface
   IConstraintContainer Interface
   
*/

#endif /* __AEECONSTRAINTCONTAINER_H__ */
