/*
  ========================================================================

  FILE:  AEEContainer.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I define the abstract interface for all
  containers. A container can hold a number of widgets and may be a
  widget itself

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEECONTAINER_H__
#define __AEECONTAINER_H__

#include "AEEWidget.h"

#include "bid/AEEIID_CONTAINER.bid"

// Container event for setting focus to a particular widget
#define EVT_WDG_MOVEFOCUS     0x711    // dwParam is next focus item (IWidget*)
#define EVT_WDG_FOCUSCANCEL   0x712    // dwParam is the focus item (IWidget*) that is going away
#define EVT_WDG_GETFOCUS      0x713    // dwParam is ptr to widget ptr to fill
#define EVT_WDG_TRAVERSE      0x714    // dwParam is ptr to TraverseDesc struct

// special definitions for dwParam EVT_WDG_MOVEFOCUS
#define WIDGET_FOCUS_NONE     0  // cancel current focus
#define WIDGET_FOCUS_FIRST    1  // move focus to first focusable widget in the container
#define WIDGET_FOCUS_LAST     2  // move focus to last focusable widget in the container 
#define WIDGET_FOCUS_NEXT     3  // move focus to next focusable widget in the container  
#define WIDGET_FOCUS_PREV     4  // move focus to previous focusable widget in the container   


// Default insertion points for widgets
#define WIDGET_ZBOTTOMMOST    ((IWidget*) -1)   // set widget as bottommost
#define WIDGET_ZNORMAL        ((IWidget*) 0)    // normal z-order (leave unchanged in SetPos)
#define WIDGET_ZTOPMOST       ((IWidget*) 1)    // set widget as topmost

// Used to tell a container how to size an item
#define WIDGET_SIZE_TO_FIT ((uint16)-1)
#define WIDGET_KEEP_EXTENT ((uint16) 0)
 
// Container Broadcast flag
#define CWF_BROADCAST      0x00000001
#define CWF_NOFILTERKEYS   0x00000002     // disables key press/release filtering on focus changes
#define CWF_NOROUTEFOCUS   0x00000004     // disables routing of events to focused children


// IContainer invalidate flags
#define ICIF_EXTENT           (1 << 0)     // widget's extent changed
#define ICIF_REDRAW           (1 << 1)     // force redraw only, content has not changed
                                           // (this is an optimization flag)
#define ICIF_DEFER            (1 << 2)     // defer this widget's draw (used when layout is disabled)
#define ICIF_FORCEDRAW        (1 << 3)     // forces an immediate, synchronous draw cycle in rootcontainer



typedef boolean (*PFNVISIT)(void*, IWidget*);  


// traversal descriptor used by EVT_WDG_TRAVERSE
typedef struct {
   PFNVISIT       pfnVisit;      // callback function
   void *         pvVisit;       // function context
   uint16         wDepth;        // how many levels deep to descend (0 == all)
   uint16         wReserved;     // reserved for internal use
} TraverseDesc;



/////////////////////////////////////////////////////////////////////////////
// IContainer interface

#define INHERIT_IContainer(iname) \
   INHERIT_IQueryInterface(iname);\
   void     (*Invalidate)  (iname *, IWidget *pw, const AEERect *prcInWidget, uint32 dwInvalFlags);\
   int      (*Locate)      (iname *, IWidget *pw, IContainer **ppRoot, AEERect *prcRelative);\
   int      (*Insert)      (iname *, IWidget *pw, IWidget *piwBefore, const void *pDesc);\
   int      (*Remove)      (iname *, IWidget *pw);\
   IWidget* (*GetWidget)   (iname *, IWidget *pwRef, boolean bNext, boolean bWrap)


AEEINTERFACE(IContainer) {
   INHERIT_IContainer(IContainer);
};


#define ICONTAINER_AddRef(p)               AEEGETPVTBL(p,IContainer)->AddRef(p)
#define ICONTAINER_Release(p)              AEEGETPVTBL(p,IContainer)->Release(p)
#define ICONTAINER_QueryInterface(p,i,p2)  AEEGETPVTBL(p,IContainer)->QueryInterface(p,i,p2)
#define ICONTAINER_Invalidate(p,pw,prc,f)  AEEGETPVTBL(p,IContainer)->Invalidate(p,pw,prc,f)
#define ICONTAINER_Locate(p,pw,ppr,prc)    AEEGETPVTBL(p,IContainer)->Locate(p,pw,ppr,prc)
#define ICONTAINER_GetWidget(p,pw,d,w)     AEEGETPVTBL(p,IContainer)->GetWidget(p,pw,d,w) 
#define ICONTAINER_Insert(p,pw,pib,pc)     AEEGETPVTBL(p,IContainer)->Insert(p,pw,pib,pc)
#define ICONTAINER_Remove(p,pw)            AEEGETPVTBL(p,IContainer)->Remove(p,pw)         


static __inline int IWIDGET_MoveFocus(IWidget *p, IWidget *piwFocus) {
   return ((0 != IWIDGET_HandleEvent(p, EVT_WDG_MOVEFOCUS, 0, (uint32)piwFocus)) ? SUCCESS : EFAILED);
}

static __inline int IWIDGET_GetFocus(IWidget* po, IWidget **ppo) {
   return ((0 != IWIDGET_HandleEvent(po, EVT_WDG_GETFOCUS, 0, (uint32)ppo)) ? SUCCESS : EFAILED);
}

static __inline void IWIDGET_Invalidate(IWidget *p, AEERect *prc, uint32 dwFlags) {
   IContainer *piContainer = 0;
   IWIDGET_GetParent(p, &piContainer);
   if (piContainer) {
      ICONTAINER_Invalidate(piContainer, p, prc, dwFlags);
      ICONTAINER_Release(piContainer);
   }
}

static __inline int IWIDGET_EnableLayout(IWidget *po, boolean bEnable) {
   return ((0 != IWIDGET_HandleEvent(po, EVT_WDG_SETLAYOUT, (uint16)(bEnable ? 1 : 0), 0)) ? SUCCESS : EFAILED);
}

static __inline int IWIDGET_ForceLayout(IWidget *po) {
   return ((0 != IWIDGET_HandleEvent(po, EVT_WDG_SETLAYOUT, 2, 0)) ? SUCCESS : EFAILED); 
}

static __inline int IWIDGET_TraverseContainerTree(IWidget *po, PFNVISIT pfn, void *pv, uint16 wDepth) {
   TraverseDesc trav;
   trav.pfnVisit  = pfn;
   trav.pvVisit   = pv;
   trav.wDepth    = wDepth;
   return ((0 != IWIDGET_HandleEvent(po, EVT_WDG_TRAVERSE, 0, (uint32)&trav)) ? SUCCESS : EFAILED);
}


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Container Interface ID

Description:
   Each class and interface within the widget framework is identified by a unique
   ID.  These IDs are used throughout the framework to distinguish between various
   types of objects, such as containers and widgets.

   The container interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as the base interface for all containers.

   The container interface is identified by the interface class ID below.

Definition:

Identifier         Definition
----------         -----------------------------------
AEEIID_CONTAINER:  Identifies the container interface

Comments:
   None

See Also:
   IContainer Interface


=======================================================================

TraverseDesc

Description:
   This data type supplies the necessary information for the EVT_WDG_TRAVERSE
   event, which is used to walk the container tree 

Definition:
   typedef struct {
      PFNVISIT       pfnVisit;
      void *         pvVisit;
      uint16         wDepth;
      uint16         wReserved;
   } TraverseDesc;

Members:
   pfnVisit:      Pointer to a callback function to execute on each widget.
   pvVisit:       Callback function context pointer.
   wDepth:        The number of levels to descend on traversal. Use value 0 
                  to descend into the maximum depth of the tree. 
   wReserved:     This member is reserved for use by the EVT_WDG_TRAVERSE event.

Comments:
   This event performs a preorder traversal of the container tree. Starting at the
   target container, each widget is 'visited' by calling the callback function with
   the widget pointer as a parameter.  Then, if the 'wDepth' member allows for it, the 
   container's child widgets are each visited in turn, descending to the depth specified
   by 'wDepth'. 

   Example -- given the tree:
===pre>   
   
            a  ......... wDepth = 1
          /   \
        b       c ...... wDepth = 2
       / \     / \
      d   e   f   g .... wDepth = 3
   
===/pre>   
   Where 'a' is the eldest parent container widget which receives the EVT_WDG_TRAVERSE
   event, the nodes will be visited in the following order:  a, b, d, e, c, f, g.

   The eldest container widget is at depth one, so passing the value of one for
   'wDepth' will visit the same container only and will not descend into any
   children.  The value zero for 'wDepth' is treated as a special case and is used 
   to descend into all levels of the tree.

   Using this method to broadcast events and properties to child widgets is the
   preferred method.  For example, if 'a' wants to tell all his children to change their
   border color to blue, then pfnVisit should set each child's border color to blue as it
   visits each of them.  
   

See Also:
   PFNVISIT
   EVT_WDG_TRAVERSE
   IWIDGET_TraverseContainerTree()

=======================================================================

PFNVISIT

Description:
   Callback function for the EVT_WDG_TRAVERSE tree-walking event. 

Prototype:
   boolean Visit(
      void *      pvVisit,    // function context passed in TraverseDesc;
      IWidget *   piw         // pointer to widget to visit
   );

Parameters:
===pre>
   pvVisit:       function context pointer passed in TraverseDesc
   piw:           pointer to widget being visited
===/pre>


Return Value:
   0 - Continue traversal
   1 - Abort traversal


===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IContainer Interface

Description:
   IContainer is an abstract interface meant to be as a base from which more complex
   container interfaces (such as the card container or XY container interfaces) will
   inherit.  The widgets framework supplies several implementations of containers,
   all based on the IContainer interface.  These derived containers differ from one
   another in the manner with which each lays out their child widgets.  The derived
   container objects are:
   
===pre>
   Card Container
   --------------
   The card container organizes widgets much like a deck of cards -- one on top of
   another, managing which is visible at the top of the deck.  Widgets in card
   containers do not have 'x' and 'y' coordinates on the display -- only 'z' ordering,
   with one widget at each level of the display.  Only one of the widget's managed by
   the container will be visible on the display, with the others hidden from view as
   if they were the remaining cards in the deck.  The card container is described in
   greater detail in AEECardContainer.h.
       
   Prop Container
   --------------
   The prop container will lay out all of its widgets in a single direction, either
   vertically or horizontally, using widget extents and proportions relative to other
   widgets as the determining factor in placing the widget on the display.  For
   example, three widgets laid out vertically -- each of which has been assigned a
   different proportional value -- will be sized and positioned in the container
   based on the size of the container and vertical extent of each widget.  The prop
   container is described in greater detail in AEEPropContainer.h.
   
   XY Container
   ------------
   The XY container will layout all of its widgets in absolute (x, y) coordinates
   within the container -- i.e. widgets will be displayed exactly as they have been
   defined, regardless of container size or the relation of one widget to another.
   'x' is 'x', 'y' is 'y' and the world becomes very predictable and boring (though
   precise, well understood, and easy to debug).  The XY container is described in
   greater detail in AEEXYContainer.h.
   
   Constraint Container
   --------------------
   The constraint container is the Mad Scientist Experimentation Gizmo of all containers!
   It's a floor wax and a dessert topping!  Its glass is half-full and half-empty!  It's
   Norman Bates and his mother, all rolled into one!  In a constraint container, widgets
   are laid out relative to the placement of other widgets and/or the container.  For
   example, the top edge of a widget could be defined to always be 4 pixels from the top
   of its parent.  Likewise, the left edge of one widget might be defined to be located
   2 pixels from the left edge of the previous widget.  In all cases, the position and
   size of a widget will be determined by the size of the parent container, and the
   position and size of all the other widgets in the container.  As the size and shape of
   the container changes, so shall the size, shape and position of each of the widgets it
   manages.  Regardless of the size and shape of the container, the widgets it manages
   will maintain consistent appearance relative to one another.  The constraint container
   is described in greater detail in AEEConstraintContainer.h.
===/pre>

   Conceptually, a container is the parent to a collection of child widgets, managed as
   a stack within the container object.  This stack keeps track of the relative order of
   the widgets being managed and provides operational services such as drawing and focus
   management to those widgets.  For example, let's invent a new application called Art
   Auction.  This application will include a screen that includes a scrollable lists of
   paintings (as images, of course), along with a text control and a "Bid" softkey so
   that the user can place twenty million dollar bids on their favorite Picasso while
   riding the subway to work.  The widgets on this screen could all be collected in a
   container, which would be responsible for managing the order and drawing of each widget.
   The container knows which widget is created first, which is next and so on... and --
   most importantly -- dictates the order that the widgets will be drawn.
   
   Illustrating the default relationship of widgets within a container further:
   
===pre>
                              Container   
                         +----------------+
   Created last  ------> |    Widget n    | <--- Drawn last (topmost)
                         |----------------|
                         |        :       |
                         |----------------|
   Created 3rd   ------> |    Widget 3    | <--- Drawn 3rd
                         |----------------|
   Created 2nd   ------> |    Widget 2    | <--- Drawn 2nd
                         |----------------|  
   Created first ------> |    Widget 1    | <--- Drawn first (bottommost)
                         +----------------+
===/pre>
   
   By default widgets are added to a container as they would be added to a stack, i.e.
   new items are added to the "top".  When the container is drawn, the widgets will be
   drawn in the order that they appear in the container's widget stack.  Therefore, where
   widgets may overlap one another, widgets at the top of the stack will be drawn forward
   (i.e. on top) of widgets buried deeper within the stack.
   
   By default the widget stack within a container grows from bottom to top.  However, an
   application may choose to insert new widgets at a particular place within the stack.
   So, though the widgets may not appear in the container in the same order that they have
   been created, their management and display will still be determined by the stack order.
   
   Theoretically, the widgets collected within a container are "in" that container.  While
   this is an accurate accounting of the object relationship that exists between a container
   and the widgets it manages, those widget are not necessarily constrained by the
   container's bounding rectangle.  The widgets managed by a container are expressed in
   coordinates relative to the container's origin and may be wholly enclosed within the
   container's extent (widget's 1, 2 and 5 in the diagram below), partially enclosed
   (widget 4), or defined to be entirely outside of the container's extent (widget 3).  The
   container sets up the clipping rectangle before telling each widget to draw, and in cases
   where the widget is fully clipped (e.g. widget 3), will simply not call on it to draw
   itself at all.
===pre>
                                                            +- - - - - - - +
                          c o n t a i n e r                 :              :
                +-----------------------------------+       :   Widget 3   :
                |+---------------------------------+|       :              :
                ||            Widget 5             ||       + - - - - - - -+
                |+---------------------------------+|
                |                                   |
                |                                   |
                |                                   |
                |                                   |
                |                  +----------------|- - - - - - - - - +
                |                  |                |                  :
                |                  |                |                  :
                |                  |         Widget | 4                :
                |                  |                |                  :
                |                  |                |                  :
                |                  +----------------|- - - - - - - - - +
                |                                   |
                |                                   |
                |                                   |
                |                                   |
                |+---------------------------------+|
                ||   Widget 1     |   Widget 2     ||
                |+---------------------------------+|
                +-----------------------------------+
===/pre>

   Widgets held in the container's widget stack may be created as either visible or
   invisible.  An invisible widget is one that occupies a place in the container stack
   relative to other widgets, but is not factored into operations involving display.  So
   when the container objects are being drawn, an invisible widget will not be displayed.
   Likewise, an invisible object will not respond to operations that would calculate in
   an object's rectangle.  Though benign with regards to "immediate" display operations,
   at any time an invisible widget could be made visible -- at which time it would be
   factored into display operations at the spot it occupies in the widget stack.
   
   The objects stored in a container's widget stack may contain any combination of widgets
   and... other containers!  For example, a single widget that combines the services of two
   widgets -- say, a widget that has been created to combine a checkbox and a static text
   widget as a single entity -- would define a container around the checkbox and static
   text widgets.  An application that wishes to display this "checkbox with text" widget on
   the same screen as an image widget would create a container.  That container would in
   turn hold an image widget and the container that manages the "checkbox with text" widget.
   
   By combining multiple levels of containers and widgets, a container hierarchy is built
   with a single container at the hierarchy's base -- referred to as the "root container".
   The relationship between the root container and all other objects within its hierarchy
   can be illustrated by the following diagram:
   
===pre>
   +------------------+        
   |                  |
   | Root Container   |
   |        +         |
   +--------|---------+        
            |
            V
   +------------------+        
   |                  |
   | Prop Container +------> +----------------+
   |                  |      |                |
   +------------------+      |   Container  +--------> +----------------+
                             |                |        |                |
                             |----------------|        |     Widget     |
                             |                |        |                |
                             |     Widget     |        +----------------|
                             |                |
                             |----------------|
                             |                |
                             |   Container  +--------> +----------------+
                             |                |        |                |
                             |----------------|        |   Container  +--------> +----------------+
                             |                |        |                |        |                |
                             |     Widget     |        +----------------+        |     Widget     |
                             |                |        |                |        |                |
                             |----------------|        |     Widget     |        +----------------+
                             |                |        |                |
                             |     Widget     |        +----------------+
                             |                |        |                |
                             +----------------+        |     Widget     |
                                                       |                |
                                                       +----------------+
===/pre>
   
   In the above diagram, a single root container serves as the parent to a Prop Container
   (a special type of container object that manages the lays out of its children in a
   proportional fashion -- see AEEPropContainer.h for more information).  The prop container
   serves as a parent to 5 child objects -- three widgets and two containers.  One of those
   containers serves as the parent for a single widget, while the other is the parent to
   three objects -- two widget and yet another container.  Finally, this container is parent
   to one child widget.  Operations within the container/widget hierarchy (such as
   invalidating a portion of the display) will trickle up the object hierarchy.  For example,
   if the widget on the far right of the above diagram were to be invalidated, the rectangle
   occupied by that widget will be invalidated in the widget's parent container -- and so on
   up to that container's parent, up eventually to the root container.

   By default, containers will be created with the following characteristics:

===pre>
      Border width:  0 pixels
===/pre>

Supported Events:
   The widget framework defines a small set of events used to support focus changes in
   container objects.  These events are sent to containers in addition to the normal
   set of BREW events such as key presses or property queries.  A container that derives
   from the base container object should be prepared to handle these events in whatever
   manner is appropriate for the behavior of that specific type of container.
   
   The base container object processes events in three passes.  In the first pass the
   container will pass all events to the border event handler where border property
   events may be handled.  Events not handled during this first pass are then processed
   by the container itself -- specifically, those events that set or move the focus.
   Finally, if the event is not handled it will be passed on to the widget within the
   container that currently holds the focus.
   
   The base container will accept the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETFOCUS:     The base container responds to this event by accepting the focus and
                      effectively moving the focus to the first focusable widget held in the
                      container's widget stack.  Upon setting the focus, the base container
                      will pass an EVT_WDG_SETFOCUS event to the widget that will be accepting
                      the focus.  This event is also sent to a widget when it loses focus.

EVT_WDG_GETFOCUS:     The base container responds to this event by returning the widget that 
                      it currently maintains as the focused widget. This may be NULL. 
                      
EVT_WDG_MOVEFOCUS:    The focus has been moved to a particular widget within the container.
                      The 'dwParam' parameter identifies the new focus item, either explicitly
                      as a pointer to an IWidget in the container's widget stack, or implicitly
                      as a defined constant that expresses the relation of the new focus item
                      relative to the current focus.  The constants used to identify relative
                      focus changes, are defined as follows:
                              
                          WIDGET_FOCUS_NONE   - The current focus should be canceled and no
                                                widget will have the focus.
                          WIDGET_FOCUS_FIRST  - The focus has been moved to the first focusable
                                                widget in the container.
                          WIDGET_FOCUS_LAST   - The focus has been moved to the last focusable
                                                widget in the container.
                          WIDGET_FOCUS_NEXT   - The focus has been moved to the next focusable
                                                widget in the container.
                          WIDGET_FOCUS_PREV   - The focus has been moved to the previous 
                                                focusable widget in the container.
                              
                      The base container will return TRUE or FALSE depending on whether or not
                      it was able to change the focus as requested.  Upon accepting the focus
                      change, the base container will pass an EVT_WDG_SETFOCUS event to the
                      widget that will be accepting the focus. 

EVT_WDG_FOCUSCANCEL:  This event will be sent to a container when the currently focused child widget 
                      is being removed.  This allows containers to reset focus logic accordingly.

EVT_WDG_SETLAYOUT:    This event can be used to request a container to not auto-layout its children. 
                      This can be used when a number of widgets need to be added, removed or manipulated 
                      in a container. Multiple re-layouts and invalidations can be a performance problems. 
                      By turning of layout while manipulating a container all layouts and invalidations 
                      are turned of. When finished manipulating the container turn on layout to re-layout 
                      the container and invalidate itself. You can also force a re-layout by using 
                      IWIDGET_ForceLayout 

EVT_WDG_SETPROPERTY:  The container responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The container
                      allows the following properties to be set:
                      
                          PROP_FLAGS          --  Set various container flags.
                          PROP_VISIBLE        --  Sets the visibility of a child widget
                      
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The container responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  These properties are
                      discussed below in greater detail.
                      
                          PROP_FLAGS         --  Retrieves various image widget flags.
                          PROP_VISIBLE       --  Gets the visibility of a child widget

EVT_WDG_TRAVERSE:     The container responds to this event by executing a function on each 
                      widget in its containment tree, including itself. The 'dwParam' parameter
                      is a pointer to a TraverseDesc data structure containing a function
                      callback, a callback context pointer, and the number of levels to 
                      descend in the container hierarchy.  (See the TraverseDesc data structure.)

===/pre>   
   
===pre>
Properties:
Property             Description
--------             ------------------------------------------------------------
PROP_FLAGS:          This property contains a set of mutually exclusive flags that dictate how a
                     container will behave.  These flags are passed in the 'dwParam' of the
                     event.  The following flags are identified for Container:

                        CWF_NOFILTERKEYS
                        ----------------
                        This flag disables filtering of mismatched key events that may occur 
                        when the container's focus widget changes.  By default, containers 
                        will watch for EVT_KEY_PRESS events and only route subsequent 
                        EVT_KEY and EVT_KEY_RELEASE events on to the widget that had the 
                        focus when EVT_KEY_PRESS occurred.  When this flag is set, this key 
                        filtering is disabled.  This flag was added for backwards 
                        compatibility and for advanced applications that want to deal with 
                        non-filtered keys.  

                        Property Value:  uint32

                        CWF_NOROUTEFOCUS
                        ----------------
                        This flag disables the routing of events to the focused child widget.
                        Normally, containers will send events to their focused child if they are 
                        unable to handle them.  This flag will prevent any events the container
                        could not handle from being sent to the focused child.

                        Property Value:  uint32

                        
                         

PROP_VISIBLE         This property is used to determine the visibility of a child widget.  The 
                     'dwParam' of IWIDGET_HandleEvent() should point to a WidgetVis struct pointer.
                     WidgetVis has two members, a pointer to the widget (piw) whose visibility
                     we want to get/set and a boolean (bVisible) whose usage depends on whether
                     we are setting or getting.  WidgetVis is used for both setting and
                     getting this property.
                     
                     When requesting the visibility of a child widget, the 'pVal' param of
                     IWIDGET_GetProperty() will be a pointer to a WidgetVis struct.  The 'piw'
                     member should point to the child widget whose visibility is being requested.
                     The 'bVisible' member will be set to TRUE if the widget is visible and FALSE
                     if it is not visible.  IWIDGET_GetProperty() will return SUCCESS if 'piw'
                     is found to be a child widget of the container and 'bVisible' is successfully
                     set to TRUE or FALSE (depending on visibility).    

                     When setting the visibility of a child widget, the 'val' param of
                     IWIDGET_SetProperty() should be a pointer to a WidgetVis struct.
                     The 'piw' member of WidgetVis should point to the child widget whose
                     visibility is being set.  The 'bVisible' members should be set to
                     TRUE if the widget should be set to visible and FALSE if it should be
                     set to not visible.  IWIDGET_SetProperty() will return SUCCESS if 'piw'
                     is found to be a child widget of the container and its visibility
                     was successfully set.

                     There are also a number of helper functions available which use this
                     property to change visibility:
                        
                          IWIDGET_SetChildVisibility()
                          IWIDGET_GetChildVisibility()
                          IWIDGET_IsVisible()
                          IWIDGET_SetVisible()
                     
                       Property Value: WidgetVis*


PROP_RAISE           This property is used to "raise" a child widget of this container to 
                     an IXYContainer target.  Raising a widget means that a widget is 
                     added to the child list of an IXYContainer (called the raise target), 
                     in addition to being a child of its original container.  The original 
                     container is responsible for the layout or positioning of the widget, 
                     and the raise target is responsible for drawing the widget.  Since 
                     the raise target is doing the drawing, the widget will be clipped to 
                     the dimensions and z-order of the raise target instead of the 
                     original parent container.  

                     The property value is a pointer to a RaiseDesc data structure, which 
                     contains two members, the first describes the child widget to 
                     raise, and the second is an IXYContainer pointer which is the raise 
                     target.

                        typedef struct RaiseDesc {
                           IXYContainer * pixyTarget;    // xy container we are raising TO
                           IWidget *      piw;           // the widget we are raising
                        } RaiseDesc;

                     To "unraise" an already raised widget, set the 'pixyTarget' member of 
                     the RaiseDesc structure to NULL, and the 'piw' member to the raised 
                     child before setting the property. 

                     For a 'Get' property operation, set the 'piw' member of the RaiseDesc 
                     structure to one of the child widgets of the container, before 
                     getting the property. On return, the 'pixyTarget' will contain a 
                     pointer to the raise target container, or NULL if the widget is not 
                     raised.
                     

                        Property Value: RaiseDesc *
                       
===/pre>

Comments:
   None
   
See Also:
   ICardContainer Interface
   IRootContainer Interface
   IPropContainer Interface
   IXYContainer Interface
   IWIDGET_MoveFocus()
   IWIDGET_ForceLayout()
   IWIDGET_EnableLayout()

===========================================================================

ICONTAINER_AddRef()

Description:

   This function increments the reference count of the IContainer interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 ICONTAINER_AddRef(IContainer *p);

Parameters:
===pre>
   p:  Pointer to the IContainer interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IContainer interface object.

Comments:
   None

Side Effects:
   None

See Also:
   ICONTAINER_Release()
   
=======================================================================

ICONTAINER_Release()

Description:

   This function decrements the reference count of the IContainer interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 ICONTAINER_Release(IContainer *p);

Parameters:
===pre>
   p:  Pointer to the IContainer interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IContainer interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   ICONTAINER_AddRef()

=======================================================================

ICONTAINER_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int ICONTAINER_QueryInterface(IContainer *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IContainer interface object.
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

ICONTAINER_Invalidate()

Description: 
   This function will invalidate all or part of a widget within a specific container.  The
   application must specify the container and widget that will be used as the basis for the
   invalidation region.  By default, ICONTAINER_Invalidate() will invalidate the region of
   the entire widget.  However, if the caller wishes to invalidate only a portion of this
   widget (for example, a widget that possesses many visual attributes, any of which could
   change independent of the others), a specific rectangle may be identified to limit the
   invalidation region within the widget, as illustrated below:

===pre>
                          c o n t a i n e r
                +-----------------------------------+
                |                                   |
                |                                   |
                |                                   |
                |                                   |
                |            w i d g e t            |
                |  +-----------------------------+  |
                |  |                             |  |    # - Portion of the display
                |  |                             |  |        to be invalidated
                |  |    rectangle                |  |
                |  | +-------------+             |  |
                |  | |#############|             |  |
                |  | |#############|             |  |
                |  | +-------------+             |  |
                |  |                             |  |
                |  |                             |  |
                |  |                             |  |
                |  +-----------------------------+  |
                |                                   |
                +-----------------------------------+
===/pre>

   When invalidating a widget that lays behind other objects on the display, the container
   will subtract fully opaque objects that overlap the target widget on the display.  However,
   in doing so, the container will maintain the "rectangleness" of the invalidation region.
   To illustrate:
   
===pre>
       +-------------------------+
       |                         |
       |                         |
       |                         |
       |                         |---------------+
       |                         |               |
       |                         |               |
       |         Widget 1        |               |
       |                         |               |
       +-------------------------+               |
                     |                           |       
                     |                           |       
                     |          Widget 2         |       
                     |                           |       
                     +---------------------------+
===/pre>
                     
   In the above example, when widget 2 is to be invalidated, the container will first calculate
   the rectangle occupied by widget 2, then subtract out the rectangle occupied by widget 1
   (which we assume to be fully opaque, for the sake of our example.  Geometrically, the resultant
   region would first appear to look like so:

===pre>
                                 +---------------+
                                 |               |
                                 |               |
                                 |               |
                                 |               |
                     +-----------+               |
                     |                           |       
                     |       Theoretical         |       
                     |  Invalidation Rectangle   |       
                     |                           |       
                     +---------------------------+
===/pre>

   However, the container defines the invalidation region to be the rectangle required to include
   all portions of the resultant region.  Therefore, the actual invalidation rectangle in
   the above example would be:
   
===pre>
                     +---------------------------+
                     |                           |       
                     |                           |       
                     |                           |       
                     |                           |       
                     |                           |       
                     |                           |       
                     |          Actual           |       
                     |  Invalidation Rectangle   |       
                     |                           |       
                     +---------------------------+
===/pre>

   In this case, the exact same rectangle as that occupied by the widget to be invalidated.
   
   Looking at a much different example, the effect of subtracting out overlapping opaque
   widgets becomes a little more obvious:
   
===pre>
       +-------------------------+
       |                         |
       |         Widget 1        |
       |                         |
       |         - - - - - - - - |---------------+
       |        :                |               |
       |        :                |               |
       |  +-------------+        |               |
       |  |     :       |        |               |
       +--|     :       |--------+               |
          |     :       |                        |       
          |     :       |                        |       
          |     :       |       Widget 2         |       
          |     :       |                        |       
          |       - - - |------------------------+
          |             |
          |  Widget 3   |
          |             |
          +-------------+
===/pre>

   Here, both widgets 1 and 3 overlap widget 2, whose obscured region is shown via a
   series of dotted lines.  After subtracting out the rectangles for the overlapping
   objects, the theoretical invalidation rectangle for widget 2 becomes:

===pre>
                                 +---------------+
                                 |               |
                                 |               |
                                 |               |
                                 |               |
                        +--------+               |
                        |                        |       
                        |      Theoretical       |       
                        | Invalidation Rectangle |       
                        |                        |       
                        +------------------------+
===/pre>

   The actual invalidation rectangle becomes the following (which is less than the
   original rectangle specified for widget 2):

===pre>
                + - - - +------------------------+
                :       |                        |       
                :       |                        |       
                :       |                        |       
                :       |                        |       
                :       |                        |       
                :       |                        |       
                :       |         Actual         |       
                :       | Invalidation Rectangle |       
                :       |                        |       
                + - - - +------------------------+
===/pre>
   
Prototype:

   void ICONTAINER_Invalidate(IContainer *p, IWidget *pw, const AEERect *prc, uint32 f)

Parameters:
===pre>
   p:    Pointer to the IContainer interface object that serves as the parent to the widget
         to invalidate.
   pw:   Pointer to the IWidget object to be invalidated, or a constant that implies a particular
         widget within the container.  The constants used to identify a relative position for the
         widget to be invalidated, are defined as follows:
                              
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
   Calling ICONTAINER_Invalidate() will trigger the invalidation to travel up through the
   container's hierarchy, first to its parent, then to its parent, etc. so that the
   invalidation rectangle may be applied to each object in the chain.
   
   Callers should set the ICIF_EXTENT flag of the 'f' parameter when calling ICONTAINER_Invalidate()
   following a change to the widget's extent.

Side Effects: 
   None

See Also:
   IContainer Interface
   IWidget Interface
   IWIDGET_Invalidate()

=======================================================================

ICONTAINER_Locate()

Description: 
   This function will find the location of a particular widget, returning that widget's
   rectangle relative to the widget's root container.  The function will also, optionally,
   return a pointer to the root container.

Prototype:

   int ICONTAINER_Locate(IContainer *p, IWidget *pw, IContainer **ppr, AEERect *prc)

Parameters:
===pre>
   p:    Pointer to the IContainer interface object that serves as the parent to the widget
         to locate.
   pw:   Pointer to the IWidget object to be located, or a constant that implies a particular
         widget within the container.  The constants used to identify a relative position for the
         widget to be located, are defined as follows:
                              
             WIDGET_ZTOPMOST     -  Locate the widget at the top of the container's widget stack.
             WIDGET_ZBOTTOMMOST  -  Locate the widget at the bottom of the container's widget stack.
             WIDGET_ZNORMAL      -  Locate the widget placed at the container's most normal layout
                                    location.  The base container treats the WIDGET_ZTOPMOST position
                                    as the "normal" location.
   
   ppr:  Pointer to an IContainer pointer, into which the function will return a pointer to the
         container's root container.  This parameter may be specified as NULL if the caller does not
         wish to get the root container.
   prc:  Pointer to an AEERect.  On input, this rectangle will specify the current relative
         offset of the 'pw' object relative to its child.  On output, this rectangle specifies
         the position of the object relative to the root container.  Therefore, ICONTAINER_Locate()
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
   The rectangle referenced by 'prc' MUST be initialized prior to calling ICONTAINER_Locate(),
   otherwise the results will be undefined.

   The pointer passed in the 'ppr' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   IContainer Interface
   IWidget Interface

=======================================================================

ICONTAINER_Insert()

Description: 

   This function inserts a new widget into the container. Insertion
   behavior is defined by each container implementation specifically
   and may not be supported for some containers. This function should
   only be called from the function definitions in derived containers.

Prototype:

   int ICONTAINER_Insert(IContainer *p, IWidget *pw, IWIdget *pb, void *pDesc)

Parameters:
===pre>
   p:    Pointer to the IContainer interface object that serves as the parent to the widget
         to locate.
   pw:   Pointer to the widget to be inserted 
   pb:   Pointer to the widget to insert the new widget before. Constants can be used to identify a 
         relative position for the widget to be located, are defined as follows:
                              
             WIDGET_ZTOPMOST     -  Locate the widget at the top of the container's widget stack.
             WIDGET_ZBOTTOMMOST  -  Locate the widget at the bottom of the container's widget stack.
             WIDGET_ZNORMAL      -  Locate the widget placed at the container's most normal layout
                                    location.  The base container treats the WIDGET_ZTOPMOST position
                                    as the "normal" location.
   
   pDesc:  Pointer to a descriptor that can convey container specific information regarding the insertion.   
           Derived containers use this parameter to pass in information such as location, proportions or 
           constraints.  If NULL is passed in for this parameter, the derived container will insert the widget
           with a "default" pDesc determined by the derived container.  Please consult the derived container's
           insert function for information about its default behavior when this parameter is NULL.
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget 'pw' was inserted successfully 
   EFAILED - If the widget 'pw' was not found in the container.
   ENOMEMORY -  The widget stack could not grow to accommodate a new widget.
   EUNSUPPORTED - The widget does not support insertion

===/pre>
   
Comments:
   None

Side Effects: 
   Calling ICONTAINER_Insert() may cause derived containers to adjust their widgets layout

See Also:
   IContainer Interface
   ICONTAINER_Remove()
   IConstraintContainer Interface
   IPropContainer Interface
   IXYContainer Interface
   ICardContainer Interface
   IWidget Interface

===========================================================================

ICONTAINER_Remove()

Description:
   This function allows an application to remove a widget from the container's widget stack.  
   The widget to be removed may be specified explicitly, by its widget pointer, or implicitly by 
   a constant that identifies the widget's relative position within the
   container's widget stack. Some containers may not support removing widgets. 
   
Prototype:
   int ICONTAINER_Remove(IContainer *p, IWidget *pw);

Parameters:
===pre>   
   p:   A pointer to an IContainer object.
   pw:  Identifies the widget that should be removed from the container's widget stack. This widget 
        may be expressed explicitly as a widget pointer, or implicitly as a constant that identifies 
        a relative position within the widget stack, as follows:
             WIDGET_ZNORMAL      - Remove the default widget, is interpreted to be
                                   the widget at the top of the widget stack.
             WIDGET_ZTOPMOST     - Remove the widget at the top of the widget stack.
             WIDGET_ZBOTTOMMOST  - Remove the widget at the bottom of the widget stack.

===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully removed from the container's widget stack.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a widget
                 that is not in the container's widget stack.
   EUNSUPPORTED  --  The widget does not support removal of contained widgets. 
===/pre>   

Comments:
   None

Side Effects: 
   Calling ICONTAINER_Remove() may cause derived containers to adjust their widgets layout
   
See Also:
    IContainer Interface
    IWidget Interface
    ICONTAINER_Insert()


===========================================================================

ICONTAINER_GetWidget()

Description:

   This function provides facilities that allow an application to retrieve the widgets stored
   in a container's widget stack.  The function may be instructed to return the widget
   from an explicit location, such as the top or bottom of the stack, or from some position relative
   to another widget in the widget stack -- for example, returning the widget that appears before
   or after another widget in the stack.  An application can easily iterate over all of the widgets
   in the container by repeatedly calling ICONTAINER_GetWidget() and passing in
   the widget pointer returned from the previous call. Some container implementations may not support 
   GetWidget and will always return NULL.
   
Prototype:
   IWidget *ICONTAINER_GetWidget(IContainer *p, IWidget *pw, boolean d, boolean w);

Parameters:
===pre>   
   p:   A pointer to an IContainer object.
   pw:  Identifies the widget within the container's widget stack that will serve as a
        reference point for the widget to be retrieved.  This widget may be expressed explicitly as
        a widget pointer, or implicitly as a constant that identifies a relative position within
        the widget stack, as follows:
            WIDGET_ZNORMAL      - Use the widget at the container's default location as
                                  the retrieval reference point.  This is  interpreted as the
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
        previous widget from the top or bottom of the container's widget stack.  When
        'w' is TRUE, the function will wrap at either end of the stack, otherwise the function
        will return NULL indicating that there are no more widgets to be retrieved.
===/pre>   
   
Return Value:
   A pointer to a widget in the container's widget stack, or NULL when there is no
   widget to be returned. Some container implementations may not support GetWidget and 
   will always return NULL 

Comments:
   Though the ICONTAINER_GetWidget() function returns a pointer to a widget, the
   function does not add a reference to the returned IWidget for the caller.

   An application may iterate over the entire widget stack -- from bottom to top -- with the
   following code:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = ICONTAINER_GetWidget (pContainer, pWidget, TRUE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once ICONTAINER_GetWidget() returns NULL
===/pre>   
   
   To iterate over all the widgets on the widget stack from top to bottom, the application's
   code would look like the following:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = ICONTAINER_GetWidget (pContainer, pWidget, FALSE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once ICONTAINER_GetWidget() returns NULL
===/pre>   

Side Effects: 
  None

See Also:
   IContainer Interface
   IWidget Interface
   
===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_MoveFocus()

Description: 
   This function requests a change in focus within the specified widget, where this source
   widget -- which is passed to the function as a widget pointer in 'p' -- is either a proper
   container, or a widget that is itself a container (e.g. an image static widget).  The
   target for the new focus is passed in 'piwFocus' and may be expressed either explicitly,
   as a pointer to a specific widget, or implicitly as a constant that identifies how the focus
   should move relative to the current focus.  The constants used to identify relative focus
   changes, are defined as follows:
                              
===pre>
       WIDGET_FOCUS_NONE   -  The current focus should be cancelled and no widget will have
                              the focus.
       WIDGET_FOCUS_FIRST  -  The focus should be  moved to the first focusable widget in the
                              parent container.
       WIDGET_FOCUS_LAST   -  The focus should be moved to the last focusable widget in the
                              parent container.
       WIDGET_FOCUS_NEXT   -  The focus should be moved to the next focusable widget in the
                              parent container.
       WIDGET_FOCUS_PREV   -  The focus should be moved to the previous focusable widget in
                              the parent container.
===/pre>

   An application can use these constants in conjunction with IWIDGET_MoveFocus() to easily
   cycle the focus from one widget to the next within a given container.  
   
   To move the focus to the next focusable widget in the given container, for example,
   IWIDGET_MoveFocus(piwc, (IWidget*)WIDGET_FOCUS_NEXT) should be called.  As another example, 
   to move the focus to the last focusable widget in the container, the application should
   call IWIDGET_MoveFocus(piwc, (IWidget*)WIDGET_FOCUS_LAST).

Prototype:
   int IWIDGET_MoveFocus(IWidget *p, IWidget *piwFocus)   

Parameters:
===pre>
   p:         Pointer to the IWidget object that is responsible for managing the focus.  This
              is usually the parent container of the widget receiving the focus.
   piwFocus:  Identifies the new focus item, either explicitly as a pointer to an IWidget in
              the container's widget stack, or implicitly as one of the relative focus
              position constants defined above.
===/pre>
      
Return Value:
===pre>
   SUCCESS - If the focus move succeeded
   EFAILED - If the focus could not be moved
===/pre>
   
Comments:
   None

Side Effects: 
  Calling IWIDGET_MoveFocus() will cause an EVT_WDG_MOVEFOCUS to be sent to the specified
  container.

See Also:
   Model Events
   IWidget Interface

===========================================================================

IWIDGET_GetFocus()

Description: 
   This function requests the current focused widget from the container. 

Prototype:
   int IWIDGET_GetFocus(IWidget *p, IWidget **ppo)   

Parameters:
===pre>
   p:         Pointer to the IWidget object that is responsible for managing the focus.  This
              is usually the parent container of the widget receiving the focus.
   ppo:       Pointer to the address to store the focused (and AddRef'ed) widget. The stored address 
              will be NULL if no widget currently has focus. 
===/pre>
      
Return Value:
===pre>
   SUCCESS - If the focus retrieval succeeded
   EFAILED - If the focus could not be retrieved
===/pre>
   
Comments:
   None

See Also:
   Model Events
   IWidget Interface

===========================================================================

IWIDGET_Invalidate()

Description: 
   This is a helper function for invalidating a widget when you only have a pointer to the
   widget interface and not a pointer to the interface of the container of the widget.  See
   ICONTAINER_Invalidate() for details on invalidation.

Prototype:
   void IWIDGET_Invalidate(IWidget *p, AEERect *prc, uint32 dwFlags);
   
Parameters:
===pre>
   p:    Pointer to the IWidget to be invalidated, or a constant that implies a particular
         widget within the container.  The constants used to identify a relative position for the
         widget to be invalidated, are defined as follows:
                              
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
   None

Side Effects: 
   None

See Also:
   IContainer Interface
   IWidget Interface
   ICONTAINER_Invalidate()
   
==========================================================================

IWIDGET_EnableLayout()

Description:
   This function is used to set a container to not do automatic layout and invalidation when
   its extent changes or its children invalidate themselves. This can be used when many 
   children need to be added, removed or modified in a container and performance degradation
   is noticed. After manipulating the children, layout should be turned on and invalidated
   (with a IWDIDGET_ForceLayout) to cause the container to re-layout and invalidate correctly. 

Prototype:
   int IWIDGET_EnableLayout(IWidget *po, boolean bEnable); 

Parameters:
===pre>
   p:        Pointer to the IWidget interface object.
   bEnable:  Whether to turn on layout and invalidation or not
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to turn off layout and invalidation.
   EFAILED - If the property could not be retrieved.
===/pre>
   
Comments:

Side Effects: 

See Also:
   Widget Events
   IWidget Interface
   IWIDGET_ForceLayout()

==========================================================================

IWIDGET_ForceLayout()

Description:
   This function is used to force a re-layout and invalidation of a container. This is
   commonly used when turning on layout after a number of child widget manipulations with
   layout turned off. See IWIDGET_EnableLayout for an explanation of when to use layout
   disabling.

Prototype:
   int IWIDGET_ForceLayout(IWidget *po); 

Parameters:
===pre>
   p:        Pointer to the IWidget interface object.
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget was able to layout and invalidate itself.
   EFAILED - If the property could not be retrieved.
===/pre>
   
Comments:

Side Effects: 

See Also:
   Widget Events
   IWidget Interface
   IWIDGET_EnableLayout()

==========================================================================

IWIDGET_TraverseContainerTree()

Description:
   This function is used to execute a function on a container and some or all of
   its children.  This is similar to the broadcast functionality but allows more
   control and flexibility. The client passes in a callback function, function context
   and a depth value, and then the container widget performs a preorder traversal of 
   its containment tree. Each widget is visited in turn, starting with the top-level
   container that is the target for this function.  

Prototype:   
   int IWIDGET_TraverseContainerTree(IWidget *po, PFNVISIT pfn, void *pv, uint16 wDepth);

Parameters:
===pre>
   po:         Pointer to the IWidget interface object.
   pfn:        Callback function to execute on each widget in container hierarchy
   pv:         Context pointer for callback function 
   wDepth:     Number of levels to descend. (Use zero to descend into all levels).
===/pre>
   
Return Value:
   SUCCESS - If the target widget handles the EVT_WDG_TRAVERSE event.
   EFAILED - If the target widget does not handle the EVT_WDG_TRAVERSE event.

Comments:
   This function is a convenience inline for the EVT_WDG_TRAVERSE event.

Side Effects:
   None

See Also:
   EVT_WDG_TRAVERSE
   PFNVISIT
   TraverseDesc


==========================================================================
*/

#endif /* __AEECONTAINER_H__ */
