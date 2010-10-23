/*
  ========================================================================

  FILE:  AEECardContainer.h
  
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
#ifndef __AEECARDCONTAINER_H__
#define __AEECARDCONTAINER_H__

#include "AEEWidget.h"
#include "AEEContainer.h"

#include "bid\AEECLSID_CARDCONTAINER.bid"
#include "bid\AEEIID_CARDCONTAINER.bid"

typedef struct ICardContainer ICardContainer;

#define INHERIT_ICardContainer(iname) \
   INHERIT_IContainer(iname); \
   int      (*Activate)    (iname *, IWidget *pw); \
   IWidget* (*GetActive)   (iname *)


AEEINTERFACE(ICardContainer) {
   INHERIT_ICardContainer(ICardContainer);
};

#define ICARDCONTAINER_AddRef(p)                AEEGETPVTBL(p,ICardContainer)->AddRef(p)
#define ICARDCONTAINER_Release(p)               AEEGETPVTBL(p,ICardContainer)->Release(p)
#define ICARDCONTAINER_QueryInterface(p,i,p2)   AEEGETPVTBL(p,ICardContainer)->QueryInterface(p,i,p2)
#define ICARDCONTAINER_Invalidate(p,pw,prc,f)   AEEGETPVTBL(p,ICardContainer)->Invalidate(p,pw,prc,f)
#define ICARDCONTAINER_Locate(p,pw,ppr,prc)     AEEGETPVTBL(p,ICardContainer)->Locate(p,pw,ppr,prc)
#define ICARDCONTAINER_Remove(p,pw)             AEEGETPVTBL(p,ICardContainer)->Remove(p,pw)         
#define ICARDCONTAINER_GetActive(p)             AEEGETPVTBL(p,ICardContainer)->GetActive(p)           
#define ICARDCONTAINER_Activate(p,pw)           AEEGETPVTBL(p,ICardContainer)->Activate(p,pw)          
#define ICARDCONTAINER_GetWidget(p,pw,d,w)      AEEGETPVTBL(p,ICardContainer)->GetWidget(p,pw,d,w)
static __inline int  ICARDCONTAINER_Insert(ICardContainer *po, IWidget *pw, IWidget *pb) {
   return AEEGETPVTBL(po,ICardContainer)->Insert(po,pw,pb, NULL);
}

static __inline IContainer *ICARDCONTAINER_TO_ICONTAINER(ICardContainer *me) {
   return (IContainer *)(void*)me;
}
/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Card Container Interface and Class IDs

Description:
   Each class and interface within the widget framework is identified by a unique
   ID.  These IDs are used throughout the framework to distinguish between various
   types of objects, such as containers and widgets.

   The card container supports both an interface and the creation of class object
   instances.  The card container interface or a new instance of the card container
   class object may be created by calling ISHELL_CreateInstance() and passing in
   the desired class id of the object.  The object that is created is, itself, a
   container and may utilize those APIs defined for either the container or card
   container interfaces.

   The card container recognizes the following set of class and interface IDs.

Definition:

Identifier               Definition
----------               -----------------------------------
AEECLSID_CARDCONTAINER:  Identifies the class ID of the card container
AEEIID_CARDCONTAINER:    Identifies the interface ID of the card container

Comments:
   None

See Also:
   ICardContainer Interface
   ISHELL_CreateInstance()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

ICardContainer Interface

Description:
   The card container is derived from the container object and is used to manage collections of
   widgets that are organized as one would expect to view cards in a deck -- i.e. one card at the
   top of the deck, with all the other cards laying below, one after another.  At any point in time,
   the card container shows only the topmost widget and all underlying widgets are hidden,
   regardless of the top widget's size, shape, position or opacity.  However, note that the
   single visible widget could be implemented as another container, which itself could manage a
   collection of many widgets, all of which would be visible depending on the type of container
   being used.

   The diagram below, illustrates how the widgets managed by a card container are related to one
   another.  The topmost widget is the one that will occupy the entire extent of the container,
   and will be the widget that has the current focus (if it can take focus).  However, it is possible
   to activate a widget that cannot take focus, but note that this will cause the container to have
   no focused widget as focus usually goes to the activated widget.  The bottom line is there are two 
   ways to set the activated widget (1) move focus to a widget which can take focus or (2) activate
   a non-focuable widget using ICARDCONTAINER_Activate().  Remember in the latter case, the container
   will not have a focused widget if the newly activated widget is unable to take focus.  
===pre>

          Container                    Widget Stack
                                       
     +----------------+             +----------------+  <--------- Topmost widget (has the focus)
     |                |             |                |
     |                |             |                |--+  <--------- Next widget
     |                |<---------------+             |  |
     |                |   overlay   |                |  |--+  <--------- Next widget
     |       +        |             |                |  |  |
     |       |        |             |                |  |  |--+  <--------- Last widget
     +-------|--------+             +----------------+  |  |  |
             |                         |                |  |  |
             |                         +----------------+  |  |
             |                            |                |  |
             | Focus                      +----------------+  |
             | Changes                       |                |
             |                               +----------------+
             |                               
             V                               
     +----------------+
     |                |
     | View Model  +----------> Event notifications
     |                |
     +----------------+
                          
===/pre>
   The widgets within a card container are all sized to the full extent of the card container,
   whether they require the full amount of space or not.  Once given the focus, it is up to the
   widget to determine how best to draw itself within the extent it has been granted.
   
   The card container is attached to a view model, which is used as a conduit through which focus
   changes are broadcast to other objects.  As the focus changes from widget to widget, the card
   container will notify listening objects of the change by issuing an EVT_MDL_TAB_CHANGE model
   event, specifying which widget (neigh, "card") now commands the focus.

Supported Events:
   The card container processes events in two passes.  In the first pass the card container will
   attempt to process focus change events, and those property events related to the card container's
   attached view model.  Events not handled during this first pass are then passed down to the base
   container object.
   
   The card container will accept the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_MOVEFOCUS:    The focus has been moved to a particular widget within the container.
                      The 'dwParam' parameter identifies the new focus item, either explicitly
                      as a pointer to an IWidget managed by the card container, or implicitly
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
                    
                      Once granted the focus, the card container will invalidate itself to trigger
                      the newly focused widget to become the activated widget and redraw.  The card
                      container will return TRUE in response to an EVT_WDG_MOVEFOCUS event. 

EVT_WDG_SETPROPERTY:  The card container responds to this event by attempting to set the property
                      identified by the 'wParam' parameter.  The card container allows the following
                      properties to be set:

                          PROP_VIEWMODEL  --  Sets the view model to be used by the container.

                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The card container responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  The card container allows the
                      following properties to be retrieved, returning a pointer to the specified
                      property in the pointer pointed to by 'dwParam':

                          PROP_VIEWMODEL  --  Gets the view model attached to the container.

                      These properties are discussed below in greater detail.
===/pre>   

Properties:
Property         Description
--------         ------------------------------------------------------------
PROP_VIEWMODEL:  This property contains a pointer to the view model that facilitates notification
                 of card container focus change events to listening objects.  As new cards come
                 to the foreground and are granted the focus, the view model is used as the
                 notification agent for attached listeners.
                             
                     Property Value:  IModel *
                    
Required Model:
   IViewModel 
   
Model Data:
   The card container relies on a view model that it simply uses as a conduit for sending focus
   changed events to listening objects.

Comments:
   None
   
See Also:
   Model Events
   IContainer Interface
   IModel Interface


===========================================================================

ICARDCONTAINER_AddRef()

Description:
   This function increments the reference count of the ICardContainer interface object,
   allowing the interface to be shared by multiple callers.  The object is freed when the
   reference count reaches 0 (zero).

Prototype:

   uint32 ICARDCONTAINER_AddRef(ICardContainer *p);

Parameters:
===pre>
   p:  Pointer to the ICardContainer interface object
===/pre>

Return Value:
   Returns the incremented reference count for the ICardContainer interface object.

Comments:
   None

Side Effects:
   None

See Also:
   ICARDCONTAINER_Release()
   
=======================================================================

ICARDCONTAINER_Release()

Description:

   This function decrements the reference count of the ICardContainer interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 ICARDCONTAINER_Release(ICardContainer *p);

Parameters:
===pre>
   p:  Pointer to the ICardContainer interface object
===/pre>

Return Value:
   Returns the decremented reference count for the ICardContainer interface object.  If the
   returned reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   ICARDCONTAINER_AddRef()

=======================================================================

ICARDCONTAINER_QueryInterface()

Description: 
   This function queries an interface for an alternate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int ICARDCONTAINER_QueryInterface(ICardContainer *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the ICardContainer interface object.
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

ICARDCONTAINER_Invalidate()

Description: 
   This function will invalidate all or part of a widget within a card container -- but only
   if the specified widget is visible.  For a card container, only the topmost visible widget
   may be invalidated, since every widget managed by the card container is considered a "card",
   stacked one atop another).  Attempting to invalidate a widget that is beneath the topmost
   visible widget will yield no change to the invalidation region of the display. By default, 
   ICARDCONTAINER_Invalidate() will invalidate the entire extent of the widget.  However, if the
   caller wishes to invalidate only a portion of this widget (for example, a widget that displays
   many visual attributes, any of which could change independent of the others), a specific
   rectangle may be identified to limit the invalidation region within the widget.
   
Prototype:

   void ICARDCONTAINER_Invalidate(ICardContainer *p, IWidget *pw, const AEERect *prc, uint32 f)

Parameters:
===pre>
   p:    Pointer to the ICardContainer interface object that serves as the parent to the widget
         to invalidate.
   pw:   Pointer to the IWidget object to be invalidated.  Invalidating any widget beside the
         topmost visible widget within the card container will have no effect.
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
   Calling ICARDCONTAINER_Invalidate() will trigger the invalidation to travel up through the
   container's hierarchy, first to its parent, then to its parent, etc. so that the
   invalidation rectangle may be applied to each object in the chain.
   
   Callers should set the ICIF_EXTENT flag of the 'f' parameter when calling the function
   following a change to the widget's extent.

Side Effects: 
   None

See Also:
   ICardContainer Interface
   IWidget Interface
   ICONTAINER_Invalidate()
   IWIDGET_Invalidate()

=======================================================================

ICARDCONTAINER_Locate()

Description: 
   This function will find the location of a particular widget, returning that widget's
   rectangle relative to the widget's root container.  The function will also, optionally,
   return a pointer to the root container.  Because each of the widgets managed by the
   card container are treated as "cards" of equal size, ICARDCONTAINER_Locate() will
   return a rectangle equal to the size of the card container's extent.

Prototype:

   int ICARDCONTAINER_Locate(ICardContainer *p, IWidget *pw, IContainer **ppr, AEERect *prc)

Parameters:
===pre>
   p:    Pointer to the IContainer interface object that serves as the parent to the widget
         to locate.
   pw:   Pointer to the IWidget object to be located.
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
   The rectangle referenced by 'prc' MUST be initialized prior to calling ICARDCONTAINER_Locate(),
   otherwise the results will be undefined.

   The pointer passed in the 'ppr' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   ICardContainer Interface
   IWidget Interface
   ICONTAINER_Locate()

=======================================================================

ICARDCONTAINER_Insert()

Description:
   This function allows an application to insert a widget into the card container's widget
   stack -- at the top of the stack, at the bottom, or someplace in-between.  For example,
   an application might choose to initially create the widget that will be topmost in
   the card container, allow that widget to be quickly displayed, then delay the creation
   of underlying widgets, creating each at some position behind the topmost widget.  Likewise,
   an application may choose to create a new widget, on the fly, in response to a particular
   event, then insert that widget at some specific place within the card container based on
   some other criteria.  ICARDCONTAINER_Insert() provides facilities for controlling where
   in the card container's widget stack a widget will be placed.
   
   A widget inserted at the top of a card container will become the active widget in the
   container, and -- if the card container has the focus -- will become the new focus item.
   
Prototype:
   int ICARDCONTAINER_Insert(ICardContainer *p, IWidget *pw, IWidget *pb);

Parameters:
===pre>   
   p:   A pointer to an ICardContainer object.
   pw:  Pointer to the widget to be inserted into the card container's widget stack.
   pb:  Indicates where in the card container's widget stack that the new widget should be
        inserted.  This value may be expressed as either a pointer to a widget already in the
        widget stack, in which case the new widget will be inserted immediately before
        the widget pointed to by 'pb' -- or as  a constant that identifies a relative position
        within the widget stack, as follows:
            WIDGET_ZNORMAL     -  Allow BREW to place the inserted widget at the default location.
                                  For a card container, this places the new widget at the top of
                                  container's widget stack.
            WIDGET_ZTOPMOST    -  The new widget should be inserted as the topmost widget in the
                                  container's widget stack.
            WIDGET_ZBOTTOMMOST -  The new widget should be inserted at the bottommost widget in
                                  the container's widget stack.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully inserted into the card container's widget stack.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a widget
                 that is not in the card container's widget stack.
   ENOMEMORY --  The widget stack could not grow to accommodate a new widget.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
   ICardContainer Interface
   IWidget Interface
   ICARDCONTAINER_Remove()

===========================================================================

ICARDCONTAINER_Remove()

Description:
   This function allows an application to remove a widget at a specific position within the
   card container's widget stack -- either at the top of the stack, at the bottom, or someplace
   in-between.  For example, let's say a card container is implemented to REALLY function like
   a deck of cards, with each "card" containing a different widget. This container is managed
   by a special widget that will shuffle all the cards in the deck.  Ah!  But this special
   widget doesn't really shuffle... it's all a trick designed to randomly throw away cards
   (widgets).  So, when the unsuspecting user selects "shuffle", the application would randomly
   choose one of the widgets from the card container and call ICARDCONTAINER_Remove() on that
   widget.
   
   Removing the topmost visible widget from a card container -- i.e. the widget that has the
   current focus -- the focus will be shifted to the next focusable widget within the container.
   
Prototype:
   int ICARDCONTAINER_Remove(ICardContainer *p, IWidget *w);

Parameters:
===pre>   
   p:  A pointer to an ICardContainer object.
   w:  Identifies the widget that should be removed from the card container's widget stack.
       This widget may be expressed explicitly as a widget pointer, or implicitly as  a constant
       that identifies a relative position  within the widget stack, as follows:
            WIDGET_ZNORMAL     -  Remove the default widget, which BREW interprets to be the widget
                                  at the top of the widget stack.
            WIDGET_ZTOPMOST     - Remove the widget at the top of the widget stack.
            WIDGET_ZBOTTOMMOST  - Remove the widget at the bottom of the widget stack.

===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully removed from the card container's widget stack.
   EBADPARM  --  The widget pointer passed in 'w' either does not exist, or refers to a widget
                 that is not in the card container's widget stack.
===/pre>   

Comments:
   None

Side Effects: 
   Removing the widget from the card container that has the focus will invalidate the region
   occupied by the widget.
   
See Also:
    ICardContainer Interface
    IWidget Interface
    ICARDCONTAINER_Insert()

===========================================================================

ICARDCONTAINER_GetActive()

Description:
   This function returns a pointer to the card container's active widget, or NULL if the
   card container is not active.  The function will also return NULL if the card container
   is active, but does not currently contain an active widget.
   
Prototype:
   IWidget *ICARDCONTAINER_GetActive(ICardContainer *p);

Parameters:
===pre>   
   p:  A pointer to an ICardContainer object.
===/pre>   
   
Return Value:
   Pointer to the card container's active widget.  The function will return NULL if the
   card container is inactive, or if the widget stack does not contain an active widget.

Comments:
   None

Side Effects: 
   None
   
See Also:
   ICardContainer Interface
   IWidget Interface
   ICARDCONTAINER_Activate()

===========================================================================

ICARDCONTAINER_Activate()

Description:
   This function allows an application to activate a specific widget within the card container's
   widget stack, thereby granting focus to that widget if it can take focus.  There are two cases to
   keep in mind: (1) activating a widget which can take focus and (2) activating a widget which CANNOT
   take focus.  In the latter case, the widget will still become active (redrawn and visible), however,
   the container will not have a focused widget.   
   
Prototype:
   int ICARDCONTAINER_Activate(ICardContainer *p, IWidget *pw);

Parameters:
===pre>   
   p:  A pointer to an ICardContainer object.
   w:  Identifies the widget within the card container's widget stack that should be activated.
       This widget may be expressed explicitly as a widget pointer, or implicitly as  a constant
       that identifies a relative position  within the widget stack, as follows:
            WIDGET_ZNORMAL     -  Activate the default widget, which BREW interprets to be the
                                  widget at the top of the widget stack.
            WIDGET_ZTOPMOST     - Activate the widget at the top of the widget stack.
            WIDGET_ZBOTTOMMOST  - Activate the widget at the bottom of the widget stack.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully activated.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a widget
                 that is not in the card container's widget stack.
===/pre>   

Comments:
   Though it is possible to remove the focus from an otherwise active card container, this function
   should not be called with 'pw' set to NULL as a means of removing the focus.  To remove the focus
   an application should instead generate an EVT_WDG_MOVEFOCUS event, specifying WIDGET_FOCUS_NONE
   as the new focus item. 

Side Effects: 
  Calling ICARDCONTAINER_Activate() will cause BREW to send an EVT_WDG_MOVEFOCUS to the card
  container, specifying 'pw' as the new focus item.

   
See Also:
   ICardContainer Interface
   IWidget Interface
   ICARDCONTAINER_GetActive()

===========================================================================

ICARDCONTAINER_GetWidget()

Description:
   This function provides facilities that allow an application to retrieve the widgets stored
   in a card container's widget stack.  The function may be instructed to return the widget from
   an explicit location, such as the top or bottom of the stack, or from some position relative
   to another widget in the widget stack -- for example, returning the widget that appears before
   or after another widget in the stack.  An application can easily iterate over all of the widgets
   in the card container by repeatedly calling ICARDCONTAINER_GetWidget() and passing in the widget
   pointer returned from the previous call.
   
Prototype:
   IWidget *ICARDCONTAINER_GetWidget(ICardContainer *p, IWidget *pw, boolean d, boolean w);

Parameters:
===pre>   
   p:   A pointer to an ICardContainer object.
   pw:  Identifies the widget within the card container's widget stack that will serve as a
        reference point for the widget to be retrieved.  This widget may be expressed explicitly as
        a widget pointer, or implicitly as a constant that identifies a relative position within
        the widget stack, as follows:
            WIDGET_ZNORMAL     -  Use the widget at the card container's default location as the
                                  retrieval reference point.  BREW interprets this to be the widget
                                  at the top of the widget stack.
            WIDGET_ZTOPMOST     - Use the widget at the top of the widget stack as the retrieval
                                  reference point.
            WIDGET_ZBOTTOMMOST  - Use the widget at the bottom of the widget stack as the retrieval
                                  reference point.
        When 'pw' is NULL, the function will return either the top or bottom widget on the stack,
        depending on the value of 'd'.  When 'pw' is NULL and 'd' is TRUE, ICARDCONTAINER_GetWidget()
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
        previous widget from the top or bottom of the card container's widget stack.  When 'w' is
        TRUE, the function will wrap at either end of the stack, otherwise the function will return
        NULL indicating that there are no more widgets to be retrieved.
===/pre>   
   
Return Value:
   A pointer to a widget in the card container's widget stack, or NULL when there is no widget to
   be returned. 

Comments:
   Though ICARDCONTAINER_GetWidget() function returns a pointer to a widget, the function does not
   add a reference to the returned IWidget for the caller.

   An application may iterate over the entire widget stack -- from bottom to top -- with the
   following code:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = ICARDCONTAINER_GetWidget (pCardContainer, pWidget, TRUE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once ICARDCONTAINER_GetWidget() returns NULL
===/pre>   
   
   To iterate over all the widgets on the widget stack from top to bottom, the application's
   code would look like the following:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = ICARDCONTAINER_GetWidget (pCardContainer, pWidget, FALSE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once ICARDCONTAINER_GetWidget() returns NULL
===/pre>   

Side Effects: 
  None

See Also:
   ICardContainer Interface
   IWidget Interface

==========================================================================

ICARDCONTAINER_TO_ICONTAINER()

Description:
   This function provides type safe casting of an ICardContainer pointer to an IContainer
   pointer and should be used when passing an ICardContainer pointer to functions that
   require a pointer to an IContainer.  This is safer than a simple cast, since the compiler
   will verify the pointer type.

Prototype:
   IContainer *ICARDCONTAINER_TO_ICONTAINER(ICardContainer *me)

Parameters:
===pre>
   me:  Pointer to an ICardContainer interface object.
===/pre>

Return Value:
   Returns 'me' cast to an IContainer pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IContainer Interface
   ICardContainer Interface

*/

#endif /* __AEECARDCONTAINER_H__ */
