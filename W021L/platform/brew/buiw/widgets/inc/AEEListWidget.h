/*
  ========================================================================

  FILE:  AEEListWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: We define various list oriented widgets, such
  as List, Grid and Pick. We are defined as Decorators since we
  utilize the decorated widget as the itemwidget to draw all items in
  the list

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEELISTWIDGET_H__
#define __AEELISTWIDGET_H__

#include "AEEDecorator.h"
#include "AEEWProperties.h"

#include "bid/AEECLSID_LISTWIDGET.bid"
#include "bid/AEECLSID_PICKWIDGET.bid"
#include "bid/AEECLSID_GRIDWIDGET.bid"

// List/Pick/Grid Widget flags (set with PROP_FLAGS)
#define LWF_WRAP                 1  // wrap to/from first/last selection (all)
#define LWF_NOSELECT             2  // (mutually exclusive with and takes precedence over LWF_WRAP)
#define LWF_COLUMNWRAP           4  // column wrap in grid mode
#define LWF_ADVANCECOLUMNWRAP    8  // column wrap in grid mode w/ advance to next column
#define LWF_VARIABLEITEMSIZE    16  // items may be of variable size, use PROP_VARITEMSIZE 
                                    // during indexer callback to modify item size


typedef struct PropItemRectDesc {
   int         nIndex;
   AEERect *   prc;
} PropItemRectDesc;

static __inline int IWIDGET_GetItemRect(IWidget *po, int nIndex, AEERect *prc) {
   PropItemRectDesc ird;
   ird.nIndex  = nIndex;
   ird.prc     = prc;
   return IWIDGET_GetProperty(po, PROP_ITEMRECT, (uint32*)&ird);
}


// Inline functions for variable-size items, 
// Used with list/pick widgets with LWF_VARIABLEITEMSIZE flag
static __inline int IWIDGET_GetVarItemSize(IWidget *po, int *pnItemSize) {
   return IWIDGET_GetPropInt(po, PROP_VARITEMSIZE, pnItemSize);
}
static __inline int IWIDGET_SetVarItemSize(IWidget *po, uint8 nItemSize) {
   return IWIDGET_SetProperty(po, PROP_VARITEMSIZE, (int)nItemSize);
}

static __inline int IWIDGET_GetSelItemSize(IWidget *po, int *pnSelItemSize) {
   return IWIDGET_GetPropertyEx(po, PROPEX_SELITEMSIZE, (int)sizeof(*pnSelItemSize), pnSelItemSize);
}
static __inline int IWIDGET_SetSelItemSize(IWidget *po, int nSelItemSize) {
   return IWIDGET_SetPropertyEx(po, PROPEX_SELITEMSIZE, (int)sizeof(nSelItemSize), &nSelItemSize);
}


/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
List Widget Class IDs

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   list, pick or grid widget by creating a new instance of the List Widget class,
    passing the desired class ID to ISHELL_CreateInstance().  The object that is
   created is, itself, a widget and may utilize those APIs defined for the IWidget
   interface.
   
   The class IDs used to identify the list widget are defined below.

Definition:

Identifier            Definition
----------            -----------------------------------
AEECLSID_LISTWIDGET:  Identifies the class ID of the list widget
AEECLSID_PICKWIDGET:  Identifies the class ID of the pick widget
AEECLSID_GRIDWIDGET:  Identifies the class ID of the grid widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

=============================================================================

PropItemRectDesc

Description:
   This data type is used to pass an index and an AEERect pointer to the
   list, pick or grid widget, along with the property PROP_ITEMRECT.

Definition:
   typedef struct PropItemRectDesc {
      int      nIndex;
      AEERect *prc;
   } PropItemRectDesc;

Members:
===pre>
   nIndex:  Index of list item for which the bounding rectangle will be retrieved.
   prc:     Pointer to an AEERect structure which will be filled on return from
            IWIDGET_GetProperty() call.
===/pre>
Comments:
   This structure is used to pass multiple arguments in the 'pVal' parameter 
   of IWIDGET_GetProperty().

See Also:
   IWIDGET_GetItemRect()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

List Widget Class

Description:
   The list widget is a user interface object that displays multiple items that the
   user can view or select.  For example, a list widget may display a vertical
   list of names, or a horizontal row of icons, or even a palette of drawing tools
   laid out in a grid.  The concept is one of presenting many items, and providing
   an interface for interacting with these items.
   
   From an implementation standpoint, the list widget is actually a decorator that
   wraps itself around another widget.  As such, as a decorator, the purpose of the
   list widget is to modify the appearance of the widget it wraps.  Just as a blend
   widget or border widget are used to effect the shading, color or transparency of
   another widget, the list widget effects the display of the _contents_ of the
   widget that lays within.
   
   Though a list widget generally contains and manages what appear to be multiple items,
   there is only one widget wrapped within the list widget, and the list widget uses
   the services of that single widget (through the model/widget relationship of the widget
   framework) to draw each individual item in the list.
   
   To explain how the list widget manages this relationship, consider the following
   diagram:
   
===pre>
                                                                        +-----------------+
                                                                        |                 |
                                                                        |    List Model   |
                                                                        |        +        |
                                                                        +--------|--------+
                                                                                 |
                                                                                 |
               Display                                                           V
      +-------------------------+                                       +-----------------+
      |                         |                                       |                 |
      |                 ----------------------------------------------->|   List Widget   |<----
      |                |        |                                       |        +        |     |
      |  +-------------|-----+  |   Draw item                           +--------|--------+     |
      |  |  Item x     +     |<-----------------                                 |              |
      |  +-------------------+  |               |                                |              |
      |                         |               |                                |              |
      |                         |               |                                |              |
      |                         |               |                                |              |
      |                         |            +--|---------------+                V              |
      |                         |            |  +               |       +-----------------+     |
      |                         |            |                  |       |                 |     |
      |                         |            |   item widget    |<--------+ Value Model   |     |
      |                         |            |                  |       |                 |     |
      |                         |            |  +               |       +-----------------+     |
      |                         |            +--|---------------+                               |
      |                         |               |                                               |
      |                         |               |                                               |
      |                         |               |                                               |
      |  +-------------------+  |               |                                               |
      |  |  Item y     +     |<-----------------                                                |
      |  +-------------|-----+  |   Draw item                                                   |
      |                |        |                                                               |
      |                 ------------------------------------------------------------------------
      |                         |
      +-------------------------+
      
===/pre>
   See?  Now isn't that perfectly clear?
   
   To better explain the diagram one must consider the models that support both the
   list widget and the widget that will be used to render each item within the list.
   
   The list widget is attached to a list model, which -- as an abstract class -- could
   be realized as a vector model, an array model, or any model that manages indexed
   collections of data.  This model will contain all the data to be presented in the
   list.  It could be a simple list of strings, or it could be a complex data structure
   containing strings, icons or anything else an application may need to render items
   in a list.
   
   Likewise, the widget wrapped within the list widget is associated with a value model,
   and will draw itself, within the contents of its extent, using whatever data has
   been supplied in its attached model.
   
   The list widget, however, though possessing methods to "draw" the contents of the list,
   does not actually do any drawing itself.  It knows only of items and indices, and
   understands how to position these entities by adjusting its location and extent.  When
   it comes time to draw an item in the list, the list widget (as a decorator) will adjust
   these factors, fetch the appropriate item data from its associated list model, then set
   this data in the value model belonging to the wrapped item widget.  Finally the draw method
   on the item widget will be called and...
   
   voila!
   
   The item is drawn onto the display by the wrapped item widget, using the appropriate
   item data, at the location occupied by that item in the list.
   
   Simplifying this process a bit, these are the steps typically followed by the list
   widget to draw a single list item:

===pre>
      - List widget positions itself to display item x
      - List widget retrieves the data for item x from the list model
      - List widget then sets the value of the item widget's value model (if supported)
      - List widget calls the indexer function if provided 
      - List widget calls draw on the item widget
===/pre>

   Note that if the item widget does not support a valuemodel the list
   widget will not attempt to set the value in the item widget's value
   model. A common use of this is to have an item widget that has a
   different model and utilizing the indexer function to do the actual
   data transfer from the listmodel to the item widget's model
  
   To create a list widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of either:
                - AEECLSID_LISTWIDGET to create a list widget
                - AEECLSID_PICKWIDGET to create a pick widget
                - AEECLSID_GRIDWIDGET to create a grid widget
          ISHELL_CreateInstance() will retrieve a reference counted instance of the desired
          list, pick or grid widget.
       2. Call IWIDGET_SetModel() to attach the appropriate model to the list
       3. Create the item widget that will be wrapped by the list widget, and identify
          the value model that is to be associated with this widget
       4. Retrieve the height of the item widget by calling IWIDGET_GetPreferredExtent(),
          then set the height of the list widget by calling IWIDGET_SetItemHeight()
       5. Call IDECORATOR_SetWidget() to wrap the list widget around the item widget
===/pre>
      
   This document describes a trio of similar widgets -- the list, pick, and grid widgets -- 
   each of which is used to collect multiple, related items, either for view or for selection.  
   These widgets share common behaviors, such as how they handle events or interact with 
   their attached model, but differ in how they arrange the items they contain.  Descriptions 
   of each list widget flavor follow:

===h3>
List Widget
===/h3>
   The list widget lays out its items in a vertical list.  For example, an application
   that manages baseball statistics might choose to present to the user a list of player
   names in alphabetic order.  This list could be implemented using a list widget that
   displays each item as static text.  Visually, the items of a list widget might be laid
   out as follows:
   
===pre>
      +-----------------------+
      |    List item 1        |
      |-----------------------|
      |    List item 2        |
      |-----------------------|
      |    List item 3        |
      |-----------------------|
      |    List item 4        |
      |-----------------------|
      |    List item 5        |
      +-----------------------+
===/pre>

===h3>
Pick Widget
===/h3>
   The pick widget lays out its items horizontally, in much the same manner as a
   toolbar.  For example, a painting application that offers a palette of editing
   tools might choose to implement the presentation of these tools as a pick widget,
   with each item in the pick list representing a particular tool.  Visually, the
   items of a pick widget might be laid out as follows:

===pre>
      +-------------------------------------------------------------------------------+
      |  Pick item 1  |  Pick item 2  |  Pick item 3  |  Pick item 4  |  Pick item 5  |
      +-------------------------------------------------------------------------------+
===/pre>

   The grid widget also lays out its items horizontally, but wraps once it is out
   of horizontal space.  The net result is a widget that lays out its items in a
   grid with the appearance of a table (though the widget does not, technically,
   behave as a table).  The items are laid out left to right, then wrap
   
===h3>
Grid Widget
===/h3>
   The grid widget is ideal for implementations that call for
   the user to select from a long list of similarly shaped items -- for example, a
   virtual art gallery app might choose to display thumbnail images of great works
   of art laid out in a grid, allowing the user to scroll through this list to locate
   an image to view at full screen size.  The items are laid out left to right, then
   wrap, with the wrapped item left justified under the row above.  Visually, the items
   of a grid widget might be laid out as follows:

===pre>
      +-----------------------------------------------+
      |  Grid item 1  |  Grid item 2  |  Grid item 3  |
      |---------------+---------------+---------------|
      |  Grid item 4  |  Grid item 5  |  Grid item 6  |
      |---------------+---------------+---------------|
      |  Grid item 7  |  Grid item 8  |  Grid item 9  |
      |---------------+---------------+---------------|
      |  Grid item 10 |  Grid item 11 |  Grid item 12 |
      +-----------------------------------------------+
===/pre>

   The items are laid out left to right, then wrap, with the wrapped item left
   justified under the row above.  By default, the grid widget is set to contain
   three rows and three columns.


===h3>
Variable-sized List Items
===/h3>
   A special feature of both the list widget and pick widget is the ability to have
   list items that vary in size from one index to another, or change size when the
   item is selected. For list widgets the height may vary, and for pick widgets the
   width may vary. 
   
   There are two types of variable-sized lists; in the first type, every item in the
   list could be set to a unique size.  In the second type, all items are of equal
   size until they become selected, and then are displayed at a different size. 
      
   For the first type of variable-sized list, the flag LWF_VARIABLEITEMSIZE must be set
   on the widget, and an indexer callback function must be installed (see PROP_INDEXER). 
   In the indexer callback, the application can control the height (or width) by calling
   IWIDGET_SetVarItemSize() to set the new size of the item. This size may be based on
   the original size of the item, which may be retrieved by calling 
   IWIDGET_GetVarItemSize(). 
   
   For example, to create a list widget which sizes the list item based on the content
   of the item, the indexer callback might look like this:
===pre>
      static void ListIndexer(IWidget *piw, int nIndex, boolean bSelected)
      {
         // calculate the height based on the content
         int height = CalcNewHeight(piw, nIndex);
         IWIDGET_SetVarItemSize(piw, size);
      }
===/pre>

   For the second type of list, no special flag of indexer function is needed, just
   simply set the size of the selected item with the property: PROP_SELITEMSIZE and
   the list will take care of changing the size of the selected item when it is drawn.


Supported Events:
   The list widget processes events in three passes.  In the first pass the list widget
   will attempt to process any property events related to the widget's border.  If the
   event is handled during this first pass, the list widget will invalidate itself and
   be redrawn, using any updated border property values.  The second pass attempts to
   handle events that may be directly intended for the list widget, including key events
   for changing the list focus or selecting the focused item.  This pass also attempts
   to process list widget specific property events.  Finally, events not handled during
   either of the first two passes will be passed to the parent decorator.
  
   The list widget handles the following events in the described manner:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_KEY:              This list widget responds to the following EVT_KEY events:
                          AVK_UP
                          AVK_DOWN
                          AVK_LEFT
                          AVK_RIGHT  --  Each of these navigation events will attempt to
                                         scroll the contents of the list, moving the
                                         contents of the list to reveal additional items.
                                         As the contents of the list move, the focus may
                                         change, causing BREW to send a ModelEvent to any
                                         object that has registered as a listener, passing
                                         EVT_MDL_FOCUS_CHANGE as the 'evCode'. 
                                         
                          AVK_SELECT --  Causes BREW to send a ModelEvent to any object that
                                         has registered as a listener, passing
                                         EVT_MDL_FOCUS_SELECT as the 'evCode'.  Generally,
                                         an application interested in list selections will
                                         register a view model with a list widget by setting
                                         the widget's PROP_VIEWMODEL property.  The view
                                         model  will then be notified of item selections and
                                         focus changes, and may change contents or respond to
                                         these events as necessary. 
                                                        
                      The fields of the ModelEvent accompanying the EVT_MDL_FOCUS_SELECT or
                      EVT_MDL_FOCUS_CHANGE events will contain the  following information:
      
                          dwParam:  The index of the item that is currently selected in the list.

EVT_WDG_SETPROPERTY:  The list widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The list widget allows
                      the following properties to be set:
                      
                          PROP_INDEXER      --  Sets the indexer descriptor to be used to
                                                iterate through the items of the list.
                          PROP_FOCUSINDEX   --  Sets the list item that should have focus.
                          PROP_SELECTINDEX  --  Sets the list item that should be selected.
                          PROP_ITEMHEIGHT   --  Sets the height of each item in the list.
                          PROP_ITEMWIDTH    --  Sets the width of each item in the list.
                          PROP_ROWS         --  Sets the number of rows contained in the list.
                          PROP_COLS         --  Set the number of columns contained in the list.
                          PROP_FLAGS        --  Set various list widget flags.
                      
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The list widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  These properties are
                      discussed below in greater detail.
                      
                          PROP_FOCUSINDEX   --  Gets the index of the list item that has focus.
                          PROP_ITEMHEIGHT   --  Gets the height of each item in the list.
                          PROP_ITEMWIDTH    --  Gets the width of each item in the list.
                          PROP_ROWS         --  Gets the number of rows contained in the list.
                          PROP_COLS         --  Gets the number of columns contained in the list.
                          PROP_FLAGS        --  Gets various list widget flags.
                          PROP_VIEWMODEL    --  Gets the view model attached to the list widget.
                          PROP_ITEMRECT     --  Gets the rectangle of a list item in screen coordinates.

                      
                      These properties are discussed below in greater detail.

EVT_WDG_SETFOCUS:     The list widget responds to set focus events by automatically scrolling the
                      current focus item into view, then passes the event through to the single
                      child IWidget used to represent each list item.  The child widget may then
                      respond as necessary, generally redrawing itself to reflect that it now has
                      focus.
===/pre>   
   

Properties:
Property           Description
--------           ------------------------------------------------------------
PROP_INDEXER:      This property it used to install an indexer function that will be 
                   called for every item in the list when the list widget iterates 
                   through its items.  Typically this is done at draw time and when 
                   the preferred extent is calculated.  This allows the application 
                   logic to manipulate the item widget just after the list model data 
                   has been copied to the item widget's model, but just before drawing 
                   occurs.  The dwParam is a pointer to an IndexerDesc data structure, 
                   which contains a pointer to an indexer callback function and a 
                   context pointer.  

                       Property Value:  IndexerDesc *

PROP_TOPINDEX:     This property contains the zero-based index of the item at the top
                   of the list, or in the case of a pick widget, at the left-most
                   position.  When setting this property the index is passed in the 
                   'dwParam' of the event and -- if the list is active -- will cause 
                   the list to be redrawn with the top item scrolled into view.
                   
                       Property Value: int
                                                            
PROP_FOCUSINDEX:   This property contains the zero-based index of the list item that
                   possesses the current focus.  When setting this property the index
                   is passed in the 'dwParam' of the event and -- if the list is active
                   -- will cause the list to be redrawn with the focused item scrolled
                   into view.

                       Property Value:  int
                                                            
PROP_SELECTINDEX:  This property contains the zero-based index of the list item that
                   has been selected.  When setting this property the index of the
                   selected item is passed in the 'dwParam' of the event and will cause
                   BREW to send an EVT_MDL_FOCUS_SELECT event containing the index of
                   the selected item to any object registered as a listener of the focus
                   model.  Setting this property will also cause the list to be redrawn
                   with the selected item scrolled into view, if the list is currently
                   active.

                       Property Value:  int

PROP_ITEMHEIGHT:   This property contains the preferred height, in pixels, of a single
                   list item.  When setting this property the preferred  height is
                   passed in the 'dwParam' of the event.
                              
                   The preferred item height is the height that the widget has requested
                   for each of its items -- or zero if the widget would prefer that the
                   height be determined by the preferred extent of the decorator.
                              
                   Setting the item height of either the list widget or the grid widget
                   will also set the actual height to be used for each item in the list.
                   Setting the item height for the pick widget only affects the preferred
                   height.

PROP_ITEMWIDTH:    This property contains the preferred width, in pixels, of a single
                   list item.  When setting this property the preferred width is passed
                   in the 'dwParam' of the event.
                              
                   The preferred item width is the width that the widget has requested
                   for each of its items -- or zero if the widget would prefer that the
                   width be determined by the preferred extent of the decorator.
                              
                   Setting the item width of pick widget will also set the actual width
                   to be used for each item in the list.  Setting the item width for the
                   list or grid widgets only affects the preferred width.
                                                            
                       Property Value:  int

PROP_HINT_ROWS:    This property is used as a hint to IWIDGET_GetPreferredExtent() to
                   set the number of rows desired for a list or grid widget. The extent
                   height calculated and returned will be equal to the item height times
                   the PROP_HINT_ROWS value.
                       
                       Property Value:  int
                       
PROP_HINT_COLS:    This property is used as a hint to IWIDGET_GetPreferredExtent() to
                   set the number of columns desired for a pick or gride widget. The
                   extent width calculated and returned will be equal to the item width
                   times the PROP_HINT_COLS value.
                   
                       Property Value:  int

PROP_ROWS:         This property contains the number of rows contained in a list or grid
                   widget.  The pick widget ignores attempts to set this property and
                   passes handling on to the parent decorator.  When getting this property,
                   the number of rows is calculated, based on the height of each item.
                                                            
                       Property Value:  int

PROP_COLS:         This property contains the number of columns contained in a pick or grid
                   widget.  The list widget ignores attempts to set this property and passes
                   handling on to the parent decorator. When getting this property, the
                   number of columns is calculated, based on the width of each item.
                                                            
                       Property Value:  int

PROP_FLAGS:        This property contains a set of (mostly) mutually exclusive flags that
                   dictate layout and behavior characteristics for the list, pick and grid
                   widgets.  These flags are passed in the 'dwParam' of the event and
                   include:
                              
                   LWF_WRAP
                   --------
                   When this flag is set the list will wrap as one continuous loop of items.
                   For example, as the user scrolls to the bottom of a list, then attempts
                   to move one item lower, the first item in the list is the next that will
                   be displayed -- though only if the list contains more items than can be
                   displayed within the list boundary.  Likewise, when scrolling to the top
                   of a list, the next item to be displayed prior to the first item, will be
                   the item at the bottom.  When this flag is not set, any attempt to scroll
                   beyond the limits (top, bottom, left or right, depending on the type of
                   list) will stop right there without wrapping.
                              
                   LWF_NOSELECT
                   ------------
                   When this flag is set, the list does not support selection and will not
                   recognize or notify other objects of selection events.  This flag takes
                   priority over the LWF_WRAP flag and will clear that flag if an attempt is
                   made to set both flags (i.e. LWF_WRAP will be automatically cleared if
                   LWF_NOSELECT is set).  When selection is not enabled, the net effect is a
                   list that purposefully has a defined beginning and end.
                              
                   LWF_COLUMNWRAP
                   --------------
                   This flag effects how the grid widget will respond to key events that
                   attempt to move the current selection up or down.  When the flag is set,
                   up/down navigation over the items in the grid circle and wrap past the
                   items in a single column.  Therefore, a grid that has been laid out as,
                              
                           +----+----+----+
                           | 1  | 2  | 3  |
                           |    |    |    |
                           +----+----+----+
                           | 4  | 5  | 6  |
                           |    |    |    |
                           +----+----+----+
                           | 7  | 8  | 9  |
                           |    |    |    |
                           +----+----+----+

                   would see the focus cycling over items 1, 4 and 7, over and over again,
                   scrolling into view hidden items if the content extends beyond the
                   vertical dimensions of its boundary.  When this flag is clear, the focus
                   will stay within the current column, but will not wrap.  So, in the
                   example above, navigating down would move from item 1, to 4, then to 7
                   and stop there.
                              
                   LWF_ADVANCECOLUMNWRAP
                   ---------------------
                   Like LWF_COLUMNWRAP, this flag effects how the grid widget will respond
                   to key events that attempt to move the current selection up or down --
                   not limiting, however, the wrapping to a single column.  Therefore, when
                   the flag is set, up/down navigation over the same nine-item grid
                   displayed above would see the focus cycling as follows:
                              
                       1, 4, 7, 2, 5, 8, 3, 6, 9... then back to 1
                              
                   Items not displayed within the dimensions of the grid widget's boundary
                   will be scrolled into and out of view as necessary as the focus moves.
                                                            
                       Property Value:  uint32


                   LWF_VARIABLEITEMSIZE
                   --------------------
                   This flag is used for list or pick widgets to display items with different
                   heights (for lists) or widths (for pick widgets). This flag is not 
                   supported for grid widgets. The size of each item is set by the 
                   application in the installed indexer callback (see PROP_INDEXER). When
                   the indexer callback is called, the application may modify the size of 
                   the item by setting the property PROP_VARITEMSIZE. By default, the
                   item size will be set to the value as set by PROP_ITEMWIDTH or 
                   PROP_ITEMHEIGHT, which will also by the value returned when getting the
                   property PROP_VARITEMSIZE. 
                   
                   
PROP_VIEWMODEL:    This property contains a pointer to the view model that may be attached
                   to the list widget, returning that pointer in an IModel pointer passed
                   in the 'dwParam' parameter passed to the list widget with the
                   EVT_WDG_GETPROPERTY event.
                              
                   The list widget does not support attempts to set the PROP_VIEWMODEL
                   property.

                       Property Value:  IModel *


PROP_ITEMRECT:    This property retrieves the bounding rectangle of a visible list, pick or 
                  grid item in screen coordinates. The 'dwParam' member is a pointer to an 
                  AEERect data structure, which will be filled with the item rectangle on 
                  return. This property will only succeed for items that are visible, including 
                  partially visible items. A visible item is one that is scrolled to viewable 
                  position in the list, not necessarily actually displayed on screen.


PROP_VARITEMSIZE: This property sets or retrieves the value of the variable dimension of 
                  an item in a variable list or pick widget (see LWF_VARIABLEITEMSIZE). For
                  a get property operation, the 'dwParam' member is of type pointer to integer,
                  (int*), and points to an integer which will be filled with the default 
                  item size. For a set property operation, the 'dwParam' member contains 
                  the value of the new item size.
                  
                  
PROP_SELITEMSIZE: This property sets or retrieves the value of the variable dimension of
                  a selected item in a list or pick widget. For list widgets, this dimension 
                  is the height, and for pick widgets, it is the width.  This property is 
                  used where the size of the selected item is to be displayed with a 
                  different size than non-selected items.  Note that this can be used without 
                  setting the LWF_VARIABLEITEMSIZE flag or installing and indexer function.

PROP_EX:             This property sets or retrieves an extended property value using a
                     WidgetPropEx structure. The extended property value is specified in
                     the structure's property ID field.

                     This structure also contains a size field and buffer pointer field.
                     For a get operation, on input the size field contains the buffer size
                     in bytes and on output it contains the buffer's data size. For a set
                     operation, on input the size field contains the buffer's data size.

                     Possible values for the property ID field include:

                        PROPEX_ITEMHEIGHT
                        PROPEX_ITEMWIDTH
                        PROPEX_SELITEMSIZE

PROPEX_ITEMHEIGHT:   This extended property replaces the property PROP_ITEMHEIGHT which is
                     unable to properly set values larger than 255.

PROPEX_ITEMWIDTH:    This extended property replaces the property PROP_ITEMWIDTH which is
                     unable to properly set values larger than 255.

PROPEX_SELITEMSIZE:  This extended property replaces the property PROP_SELITEMSIZE which
                     is unable to properly set values larger than 255.

Required Model:
   IListModel (default)
   
Model Data:
   The list model is an abstract class that, in practice, will be implemented as a vector
   model, an array model, or any model that organizes its data as indexed collections of
   data.  This model will contain all the data to be presented in the list and will abide
   by the access functions provided by the list model interface.
   
   However, the data returned by this model MUST accommodate the data expected by the item
   widget that is wrapped by the list widget, since the list widget will retrieve this data
   from the list model, then set this data into the value model used by item widget.  To
   illustrate, if the list widget is wrapped around an image widget, the list model must be
   able to retrieve its indexed item data as a pointer to an IImage -- the model data
   required by the image widget.

   The list widget may be created with a specific list model identified, rather than the
   default list model assigned by the widget framework.  Clients that identify their own model
   interface to be attached to the list widget must abide by the data conventions described
   above, returning a pointer the data type expected by the wrapped item widget when queried
   for the model data. 
   
Comments:
   None
   
See Also:
   Model Events
   ModelEvent
   ISHELL_CreateInstance()
   IWIDGET_SetModel()
   IWIDGET_GetPreferredExtent()
   IWIDGET_SetItemHeight()
   IDECORATOR_SetWidget()
   
===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_GetItemRect()

Description:
   This inline function retrieves the bounding rectangle of the list item at
   a given index. 


Prototype:
   int IWIDGET_GetItemRect(IWidget *po, int nIndex, AEERect *prc);
   
Parameters:
===pre>
   po:      Pointer to the list, pick or grid widget.
   nIndex:  Index of list item to retrieve rectangle for.
   prc:     Pointer to an AEERect structure to be filled on return.
===/pre>   

Return Values:
===pre>
   SUCCESS - If the widget was able to successfully retrieve the bounding rectangle of 
             the requested list item.
   EFAILED - If the item rectangle could not be retrieved. Fails for invalid indexes
             or items that are not scrolled into view.
===/pre>

Comments:
   This inline function and property will only succeed for list items that are 
   visible, including partially visible items.  Visible means that they are scrolled
   to viewable position in the list, not necessarily actually displayed on screen.

Side Effects: 
   None

See Also:
   PROP_ITEMRECT
   PropItemRectDesc


===========================================================================

IWIDGET_GetVarItemSize()

Description:
   This inline function retrieves the default size for the current item from
   an indexer callback function.   
   
Prototype:
   int IWIDGET_GetVarItemSize(IWidget *po, int *pnItemSize);

Parameters:
===pre>
   po:         Pointer to the list or pick widget.
   pnItemSize: Pointer to an integer variable to be filled on return.
===/pre>   

Return Values:
===pre>
   SUCCESS - If the widget was able to successfully retrieve the size of
             the current item.
   EFAILED - If the size of the curren item could not be retrieved.
===/pre>

Comments:
   None

Side Effects: 
   None

See Also:
   PROP_VARITEMSIZE
   LWF_VARIABLEITEMSIZE


===========================================================================

IWIDGET_SetVarItemSize()

Description:
   This inline function sets the size of the current item from an 
   indexer callback function.
   
Prototype:
   int IWIDGET_SetVarItemSize(IWidget *po, int nItemSize);

Parameters:
===pre>
   po:         Pointer to the list, or pick widget.
   nItemSize:  Integer size value to be set.
===/pre>   

Return Values:
===pre>
   SUCCESS - If the widget was able to successfully the set the size of
             the current item.
   EFAILED - If the item size could not be set.
===/pre>

Comments:
   None

Side Effects: 
   None

See Also:
   PROP_VARITEMSIZE
   LWF_VARIABLEITEMSIZE



===================================================================================
*/

#endif /* __AEELISTWIDGET_H__ */
