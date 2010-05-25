/*
  ========================================================================

  FILE:  AEEGridContainer.h
  
  SERVICES: 

  GENERAL DESCRIPTION: A grid container uses a grid to order its
  children. It provides default focus movement behavior to allow
  focus to move around the grid in the obvious way. 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEGRIDCONTAINER_H__
#define __AEEGRIDCONTAINER_H__

#include "AEE.h"
#include "AEEWidget.h"
#include "AEEContainer.h"

#include "bid/AEECLSID_GRIDCONTAINER.bid"
#include "bid/AEEIID_GRIDCONTAINER.bid"

typedef struct IGridContainer IGridContainer;

#define CELL_FIXED           (1 << 0)
#define CELL_KEEP_EXTENT     (1 << 1)
#define CELL_SIZE_TO_FIT     (1 << 2)
#define CELL_PROPORTIONAL    (1 << 3)

typedef struct {
   uint16 iFlag;              // cell type e.g. CELL_FIXED
   uint16 iValue;             // value (required for CELL_FIXED and CELL_PROPORTIONAL)
   uint16 iPaddingBefore;     // padding within the cell before the widget
   uint16 iPaddingAfter;      // padding within the cell after the widget
} GridDescriptor;

#define INHERIT_IGridContainer(iname) \
   INHERIT_IContainer(iname);\
   void  (*GetGridDescriptors) (iname *, GridDescriptor *pRows, uint16 *iRows, GridDescriptor *pCols, uint16 *iCols);\
   int   (*SetGridDescriptors) (iname *, GridDescriptor *pRows, uint16 *iRows, GridDescriptor *pCols, uint16 *iCols);\
   int   (*GetVisibility)      (iname *, IWidget *piw, boolean *pbVisible);\
   int   (*SetVisibility)      (iname *, IWidget *piw, IWidget *piwBefore, const boolean *pbVisible)   

AEEINTERFACE(IGridContainer) {
   INHERIT_IGridContainer(IGridContainer);
};

#define IGRIDCONTAINER_AddRef(p)                         AEEGETPVTBL(p,IGridContainer)->AddRef(p)
#define IGRIDCONTAINER_Release(p)                        AEEGETPVTBL(p,IGridContainer)->Release(p)
#define IGRIDCONTAINER_QueryInterface(p,i,p2)            AEEGETPVTBL(p,IGridContainer)->QueryInterface(p,i,p2)
#define IGRIDCONTAINER_Invalidate(p,pw,prc,f)            AEEGETPVTBL(p,IGridContainer)->Invalidate(p,pw,prc,f)
#define IGRIDCONTAINER_Locate(p,pw,ppr,prc)              AEEGETPVTBL(p,IGridContainer)->Locate(p,pw,ppr,prc)
#define IGRIDCONTAINER_Remove(p,pw)                      AEEGETPVTBL(p,IGridContainer)->Remove(p,pw)
#define IGRIDCONTAINER_GetWidget(p,pw,d,w)               AEEGETPVTBL(p,IGridContainer)->GetWidget(p,pw,d,w) 
#define IGRIDCONTAINER_GetGridDescriptors(p,pr,ir,pc,ic) AEEGETPVTBL(p,IGridContainer)->GetGridDescriptors(p,pr,ir,pc,ic)
#define IGRIDCONTAINER_SetGridDescriptors(p,pr,ir,pc,ic) AEEGETPVTBL(p,IGridContainer)->SetGridDescriptors(p,pr,ir,pc,ic)
#define IGRIDCONTAINER_GetVisibility(p,pw,pb)            AEEGETPVTBL(p,IGridContainer)->GetVisibility(p,pw,pb)
#define IGRIDCONTAINER_SetVisibility(p,pw,piwb,pb)       AEEGETPVTBL(p,IGridContainer)->SetVisibility(p,pw,piwb,pb)

static __inline  int IGRIDCONTAINER_Insert(IGridContainer *po, IWidget *pw, IWidget *pb, const boolean *bVisible) {
   return AEEGETPVTBL(po,IGridContainer)->Insert(po,pw,pb,(const void *)bVisible);
}

static __inline IContainer *IGRIDCONTAINER_TO_ICONTAINER(IGridContainer *me) {
   return (IContainer *)(void*)me;
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Grid Container Interface and Class IDs

Description:
   Each class and interface within the widget framework is identified by a unique
   ID.  These IDs are used throughout the framework to distinguish between various
   types of objects, such as containers and widgets.

   The grid container supports both an interface and the creation of class
   object instances.  The grid container interface or a new instance of the
   grid container class object may be created by calling ISHELL_CreateInstance()
   and passing in the desired class id of the object.  The object that is created is,
   itself, a container and may utilize those APIs defined for either the container or
   grid container interfaces.

   The grid container recognizes the following set of class and interface IDs.

Definition:

Identifier                     Definition
----------                     -----------------------------------
AEECLSID_GRIDCONTAINER:        Identifies the class ID of the grid container
AEEIID_GRIDCONTAINER:          Identifies the interface ID of the grid container

Comments:
   None

See Also:
   IGridContainer Interface
   ISHELL_CreateInstance()

=============================================================================
Cell types

Description:
   Each cell knows how it should be laid out.  Cell type flags will determine the
   behavior of the cell's layout in either the x or y direction.  For example, we
   can specify a flag to indicate that the cell should have a fixed size in the 
   y-axis of 5 pixels.  That cell will never draw taller than 5 pixels high.  Since
   properties are set on a row and column basis, every cell in a particular row or
   column will have the same cell type.  Consider the following example, which
   shows the cell type for both x and y direction.  The cells are labeled with
   numbers for reference below.

===pre>
                           CELL_KEEP_EXTENT            CELL_PROPORTIONAL
                     +--------------------------+--------------------------+
   CELL_FIXED        | [1] X: CELL_KEEP_EXTENT  | [2] X: CELL_PROPORTIONAL |
                     |     Y: CELL_FIXED        |     Y: CELL_FIXED        | 
                     +--------------------------+--------------------------+
   CELL_PROPORTIONAL | [3] X: CELL_KEEP_EXTENT  | [4] X: CELL_PROPORTIONAL |
                     |     Y: CELL_PROPORTIONAL |     Y: CELL_PROPORTIONAL | 
                     +--------------------------+--------------------------+

  Cell [1] - will use its x extent to determine the width of the cell.  The
             height of the cell will be a fixed value (determined by the 
             iValue parameter of that row's GridDescriptor)
  Cell [2] - will use a proportional width (again the value will be determined
             by the iValue member of that row's GridDescriptor).  The height
             will be a fixed value, the same as widget 1's height.  
  Cell [3] - will use the extent of the widget to determine its width, like
             cell 1.  The greater of widget 1 and 3's x-extent will be used.
             The height of widget 3 is proportional and the proportion is
             determined again by the iValue of column's GridDescriptor.
  Cell [4] - will use the proportional value the same as cell 2 for its
             x size.  The height will be the same as cell 3, since they
             share the same GridDescriptor.
===/pre>

  Grid container is after all a grid.  The example above shows that the 
  GridDescriptor for a row or column applies to the whole row/column.  Otherwise, it 
  would not be a grid!  Below is a detailed description of each cell type:

===pre>
Definition                 Description
--------------             ------------------------------------------------------- 
CELL_FIXED:                The cells in this row or column will have a fixed
                           width or height, respectively.  The fixed value is
                           determined by the iValue member of the GridDescriptor
                           which describes the row or column.  The value is the
                           exact number of pixels.

CELL_KEEP_EXTENT:          The cells in this row or column will use the largest
                           widget's width or height, respectively, to determine
                           the size of the cells in the row or column.  This iValue
                           member of GridDescriptor is ignored when this flag is
                           set.

CELL_SIZE_TO_FIT:          This indicates the cell should use the widget's
                           preferred extent to size the cell.  

CELL_PROPORTIONAL:         This indicates the cells should use a proportional width
                           or height.  Proportional behavior works very much like
                           Prop Container's proportions.  The iValue member of 
                           GridDescriptor must be set to the proportional value for a
                           row/column.  Proportions only make sense in the context
                           of the remaining free space left in the row or column.  Cells
                           with CELL_KEEP_EXTENT or CELL_FIXED will not deviate in their
                           widths and heights.  The proportional value indicates what 
                           proportion of the remaining free space in the container should
                           the row/column occupy.  In the example above, if cell #3's 
                           iValue was 1 and cell #4's value was 2, then we expect cell #4
                           to be twice as wide as cell #3.  We calculate this value by
                           summing the proportions of each cell (its iValue) to determine
                           the denominator.  In this case, the denominator would be 3.  We then
                           divide the proportion of the cell by the denominator to determine
                           how much of the grid container's extent it will consume.  
                           Cell #3's x extent will be 1/3 the width of grid container while
                           Cell #4's x extent will be 2/3's the width of grid container's
                           x extent.  For a more detailed explanation of proportions, please
                           refer to prop container.
===/pre>

Members:
   None
   
Comments:
   None

See Also:
   GridDescriptor
   IPropContainer

=============================================================================

GridDescriptor

Description:
   A grid container is, as the name suggests, a grid.  In order to tell grid
   container the behavior of each of its rows and columns, we use
   GridDescriptors.  This same struct is used to describe both rows and
   columns.  

Definition:
   typedef struct {
      uint16 iFlag;              
      uint16 iValue;             
      uint16 iPaddingBefore;     
      uint16 iPaddingAfter;      
   } GridDescriptor;

Members:
===pre>
   iFlag:            This determines the layout of the cells within the 
                     row/column described.  iFlag should be set to a
                     Cell type e.g. CELL_KEEP_EXTENT.

   iValue:           This will be set to a value that a Cell type requires.
                     Currently, the only Cell types that require values are
                     CELL_PROPORTIONAL and CELL_FIXED.  For more detail,
                     refer to the Cell type explanation above.

   iPaddingBefore:   This is the amount of padding the cells within the
                     row/column should have before the widget's extent begins.  

   iPaddingAfter:    This is the amount of padding, measured in pixels,
                     the cells within the described row/column should have
                     after the widget extent.  
===/pre>

Comments:
   None

See Also:
   Cell types

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IGridContainer Interface

Description:
   The grid container is derived from the base container object and is used
   to manage widgets.  As the name suggests, grid container lays widgets out
   in a grid format.  Each column and row has a configurable properties which
   can be set to change the appearance of grid container.  This is done by
   setting the values of the GridDescriptors.  
   
   Let's start with an example of a grid container in action.  Assume you wish
   to create an application that lets users browse action figures from Star
   Wars Episode III: Revenge of the Sith.  We'll use a grid container to hold
   images of the figures, each contained in their own Image Widget.  Assume
   each Image Widget has both an unselected image and a selected image.  We 
   will switch to the selected image when the image widget has focus.       
===pre>
                             Grid Container
   +----------------------------------------------------------------+
   | +------------------+ +------------------+ +------------------+ |
   | | Mace Windu       | | Anakin Skywalker | | Queen Amidala    | |
   | |                  | |                  | |                  | | 
   | | with lightsaber  | | with lightsaber  | | with bizarre food| | 
   | | and detachable   | | to hack Windu's  | | craving action.  | |
   | | hand plus whistle| | hand off and 200 | | She's expecting  | |
   | | to call for alien| | Stormtroopers to | | Luke and Leia    | |       
   | | Jedi help        | | do his fighting  | | Skywalker soon.  | |   
   | +------------------+ +------------------+ +------------------+ |
   |                                                                |
   | +------------------+ +------------------+ +------------------+ |
   | | Random Lame      | | Bobo Fett        | | Jek Porkins      | |
   | | Alien Jedi       | |                  | |                  | | 
   | |                  | | with pubescent   | | "No, I'm alright,| | 
   | | wearing a red    | | voice, Jango     | | I'm alright...   | |
   | | shirt with a     | | Fett's empty     | | oh I'm really    | |
   | | bullseye painted | | helmet, and the  | | alright."        | |
   | | on it            | | Fett family curse| | (Flashback humor)| |
   | +------------------+ +------------------+ +------------------+ |
   +----------------------------------------------------------------+

===/pre>
   
   The grid container will lay the contained widgets out in a grid as shown above.  Hopefully,
   the vivid descriptions above helped you picture the figures in all their splendor. 
   To set up a grid container like the one above, we need to perform the following steps:
===pre>
   1. ISHELL_CreateInstance(piMyShell, AEECLSID_GRIDCONTAINER, &picMyGridContainer)
   2. IGRIDCONTAINER_QueryInterface(picMyGridContainer, AEEIID_WIDGET, &piwMyGridContainer)
   3. Using the widget interface to grid container, set the extent to the desired
      size you want the container to be.  
   4. Insert the grid container into the root container (so it draws to the screen)
      using IROOTCONTAINER_Insert(picMyRoot, piwMyGridContainer, WIDGET_ZNORMAL, &myWidgetPos)
   5. Insert six Image Widgets containing the images of our Star Wars figures
   6. Create an array of GridDescriptors that describe the properties of each row and column.
      We would then create an array of GridDescriptors (one for each row, so 3 in this case).
      Each GridDescriptor would be set to the following values:
         myGridDescriptor.iFlag = CELL_SIZE_TO_FIT;
         myGridDescriptor.iValue = 0;  //ignored for this flag (see Cell types)
         myGridDescriptor.iPaddingBefore = 2;
         myGridDescriptor.iPaddingAfter = 2;
      We are building a pretty simple grid container, so each row descriptor will be
      the same.  
   7. Call IGRIDCONTAINER_SetGridDescriptors(picMyGridContainer, pDesc, &iSize, pDesc, &iSize)
      where pDesc is our array of length iSize containing initialized GridDescriptors to
      describe each row and column.  We are reusing the same pDesc array and iSize value
      because we want our rows and columns to be identical in size and behavior. 
===/pre>
   
   Grid container knows which widget inside the container has focus.  By default the upper
   leftmost widget has focus.  To explicitly set focus to a different widget, say for 
   example the last widget, call:
===pre>
   IWIDGET_MoveFocus(me->piwMyGridContainer, (IWidget*)WIDGET_FOCUS_LAST);
===/pre>
   
   In order to support advanced focus changing behavior, override grid container's
   event handler using IWIDGET_SetHandler.

Supported Events:
   The grid container processes events in two passes.  The grid container first passes all events
   to the base container, which will handle events relating to the container's focus.  Events not
   handled during this first pass will then be handled by the grid container itself.
   
   The grid container will accept the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_GETPROPERTY:  The grid container responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  The grid container allows
                      the following properties to be retrieved, returning the value of the
                      property in the pointer pointed to by 'dwParam':

                          PROP_ROWS        --  Gets the number of rows

                          PROP_COLS        --  Gets the number of columns

                      These properties are discussed below in greater detail.
===/pre>   

Properties:
Property           Description
--------           ------------------------------------------------------------

PROP_ROWS:          This is a "get only" property and will return the number of rows in 
                    the grid container.  In order to set the number of rows, see
                    IGRIDCONTAINER_SetGridDesriptors().
                      
                      Property Value: uint32

PROP_COLS:          This is a "get only" property and will return the number of columns in 
                    the grid container.  In order to set the number of columns, see
                    IGRIDCONTAINER_SetGridDescriptors()
                      
                      Property Value: uint32  

Required Model:
   None 
   
Model Data:
   None

Comments:
   None
   
See Also:
   IContainer Interface
   IWIDGET_MoveFocus   

===========================================================================

IGRIDCONTAINER_AddRef()

Description:

   This function increments the reference count of the IGridContainer interface object,
   allowing the interface to be shared by multiple callers.  The object is freed when the
   reference count reaches 0 (zero).

Prototype:

   uint32 IGRIDCONTAINER_AddRef(IGridContainer *p);

Parameters:
===pre>
   p:  Pointer to the IGridContainer interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IGridContainer interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IGRIDCONTAINER_Release()

=======================================================================

IGRIDCONTAINER_Release()

Description:

   This function decrements the reference count of the IGridContainer interface object.
   The object is freed from memory and is no longer valid once its reference count reaches 0
   (zero).

Prototype:

   uint32 IGRIDCONTAINER_Release(IGridContainer *p);

Parameters:
===pre>
   p:  Pointer to the IGridContainer interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IGridContainer interface object.  If
   the returned reference count is 0 (zero), the object has been freed and will no longer be
   valid.

Comments:
   None

Side Effects:
   None

See Also:
   IGRIDCONTAINER_AddRef()
=======================================================================

IGRIDCONTAINER_QueryInterface()

Description: 
   This function queries an interface for an alternate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IGRIDCONTAINER_QueryInterface(IGridContainer *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IGridContainer interface object.
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
   The pointer passed in the 'd' parameter uses double-indirection i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the Grider
   level of indirection is used in calling the function.

Side Effects: 
   Upon successful retrieval of an interface pointer, that interface is returned with
   its reference count incremented.  The caller is responsible for releasing this object
   at some point in the future.

See Also:
   IQI_QueryInterface()

=======================================================================

IGRIDCONTAINER_Invalidate()

Description: 
   This function will invalidate all or part of a widget within a grid container.
   The application identifies the container and widget, and may also specify a rectangle
   within that widget in order to invalidate a portion of the widget.  By default, the
   entire extent of the widget will be invalidated.
   
Prototype:

   void IGRIDCONTAINER_Invalidate(IGridContainer *p, IWidget *pw, const AEERect *prc, uint32 f)

Parameters:
===pre>
   p:    Pointer to the IGridContainer interface object that serves as the parent to the
         widget to invalidate.
   pw:   Pointer to the IWidget object to be invalidated, or a constant that implies a particular
         widget within the grid container.  The constants used to identify a relative position
         for the widget to be invalidated, are defined as follows:
                              
             WIDGET_ZTOPMOST     -  Requests that the widget at the top of the container's
                                    widget stack should be invalidated.
             WIDGET_ZBOTTOMMOST  -  Requests that the widget at the bottom of the container's
                                    widget stack should be invalidated.
             WIDGET_ZNORMAL      -  Requests that the widget placed at the container's most
                                    normal layout location be invalidated.  The grid
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
   Calling IGRIDCONTAINER_Invalidate() will trigger the invalidation to travel up through
   container's hierarchy, first to its parent, then to its parent, etc. so that the
   invalidation rectangle may be applied to each object in the chain.
   
   Callers should set the ICIF_EXTENT flag of the 'f' parameter when calling the function
   following a change to the widget's extent.  

Side Effects: 
   Calling IGRIDCONTAINER_Invalidate() with the ICIF_EXTENT flag on will cause the layout
   of the objects within the grid container to change, using the widget's new extent.

See Also:
   IGridContainer Interface
   IWidget Interface
   ICONTAINER_Invalidate()
   IWIDGET_Invalidate()

=======================================================================

IGRIDCONTAINER_Locate()

Description: 
   This function will find the location of a particular widget, returning that widget's
   rectangle relative to the widget's root container.  The function will also, optionally,
   return a pointer to the root container.

Prototype:

   int IGRIDCONTAINER_Locate(IGridContainer *p, IWidget *pw, IContainer **ppr, AEERect *prc)

Parameters:
===pre>
   p:    Pointer to the IGridContainer interface object that serves as the parent to the widget
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
   The rectangle referenced by 'prc' MUST be initialized prior to calling IGRIDCONTAINER_Locate(),
   otherwise the results will be undefined.

   The pointer passed in the 'ppr' parameter uses double-indirection i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   IGridContainer Interface
   IWidget Interface
   ICONTAINER_Locate()

=======================================================================

IGRIDCONTAINER_Insert()

Description:
   This function allows an application to insert a widget into the grid container's
   widget stack -- at the top of the stack, at the bottom, or some place in between.  By
   default, widgets will be inserted at the top of the stack.  However, an application may
   choose instead to insert widgets anywhere within the container's widget stack.  For
   example, consider an application that wishes to keep one widget always at the top of the
   container's widget stack so that the layout of this widget is always based on the location
   of the topmost visible widget.  As new widgets are added to the screen, the application
   wants to make sure that the fixed widget remains on the top of the stack, so each time a
   new widget is added it will be inserted before the fixed widget, passing a pointer to the
   fixed widget in the function's 'pb' parameter.
   
Prototype:
   int IGRIDCONTAINER_Insert(IGridContainer *p, IWidget *pw, IWidget *pb, const boolean *bVisible);

Parameters:
===pre>   
   p:          A pointer to an IGridContainer object.
   pw:         Pointer to the widget to be inserted into the grid container's widget stack.
   pb:         Indicates where in the grid container's widget stack that the new widget
               should be inserted.  This value may be expressed as either a pointer to a 
               widget already in the widget stack, in which case the new widget will be
               inserted immediately before the widget pointed to by 'pb' -- or as  a
               constant that identifies a relative position within the widget stack, as
               follows:
                  WIDGET_ZNORMAL     -  Allow BREW to place the inserted widget at the
                                        default location.  For a grid container,
                                        this places the new widget at the top of container's
                                        widget stack.
                  WIDGET_ZTOPMOST    -  The new widget should be inserted as the topmost
                                        widget in the container's widget stack.
                  WIDGET_ZBOTTOMMOST -  The new widget should be inserted at the bottommost
                                       widget in the container's widget stack.
   bVisible:   This parameter should point to a valid boolean in memory set to TRUE, if pw
               should be visible, or FALSE, if pw should not be visible.  If this parameter
               is set to NULL, pw will be inserted with visibility set to TRUE.  
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully inserted into the grid container's widget stack.
   EBADPARM  --  The widget pointer passed in 'pb' either does not exist, or refers to a widget
                 that is not in the grid container's widget stack.
   ENOMEMORY --  The widget stack could not grow to accommodate a new widget.
===/pre>   

Comments:
   None

Side Effects: 
   Calling IGRIDCONTAINER_Insert() will cause the grid container to adjust the object
   layout to include the newly added widget.
   
See Also:
   IGridContainer Interface
   IWidget Interface
   IGRIDCONTAINER_Remove()
   
===========================================================================

IGRIDCONTAINER_Remove()

Description:
   This function allows an application to remove a widget from the grid container's
   widget stack.  The widget to be removed may be specified explicitly, by its widget pointer,
   or implicitly by a constant that identifies the widget's relative position within the
   container's widget stack.
   
Prototype:
   int IGRIDCONTAINER_Remove(IGridContainer *p, IWidget *pw);

Parameters:
===pre>   
   p:   A pointer to an IGridContainer object.
   pw:  Identifies the widget that should be removed from the grid container's widget
        stack. This widget may be expressed explicitly as a widget pointer, or implicitly as 
        a constant that identifies a relative position within the widget stack, as follows:
             WIDGET_ZNORMAL      - Remove the default widget, which BREW interprets to be
                                   the widget at the top of the widget stack.
             WIDGET_ZTOPMOST     - Remove the widget at the top of the widget stack.
             WIDGET_ZBOTTOMMOST  - Remove the widget at the bottom of the widget stack.

===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The widget was successfully removed from the grid container's widget
                 stack.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a widget
                 that is not in the grid container's widget stack.
===/pre>   

Comments:
   None

Side Effects: 
   Calling IGRIDCONTAINER_Remove() will cause the grid container to do its layout again.  The
   removed widget will not leave a space, but rather the remaining widgets will move closer
   to the top left of grid container to fill the removed widget's position.  It will not in any
   case leave an empty cell where the removed widget was.    

See Also:
    IGridContainer Interface
    IWidget Interface
    IGRIDCONTAINER_Insert()

=============================================================================

IGRIDCONTAINER_GetWidget()

Description:
   This function provides facilities that allow an application to retrieve the widgets stored
   in a grid container's widget stack.  The function may be instructed to return the widget
   from an explicit location, such as the top or bottom of the stack, or from some position relative
   to another widget in the widget stack -- for example, returning the widget that appears before
   or after another widget in the stack.  An application can easily iterate over all of the widgets
   in the grid container by repeatedly calling IGRIDCONTAINER_GetWidget() and passing in the widget
   pointer returned from the previous call.
   
Prototype:
   IWidget *IGRIDCONTAINER_GetWidget(IGridContainer *p, IWidget *pw, boolean d, boolean w);

Parameters:
===pre>   
   p:   A pointer to an IGridContainer object.
   pw:  Identifies the widget within the grid container's widget stack that will serve as a
        reference point for the widget to be retrieved.  This widget may be expressed explicitly as
        a widget pointer, or implicitly as a constant that identifies a relative position within
        the widget stack, as follows:
            WIDGET_ZNORMAL      - Use the widget at the grid container's default location as
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
        previous widget from the top or bottom of the grid container's widget stack.  When
        'w' is TRUE, the function will wrap at either end of the stack, otherwise the function
        will return NULL indicating that there are no more widgets to be retrieved.
===/pre>   
   
Return Value:
   A pointer to a widget in the grid container's widget stack, or NULL when there is no
   widget to be returned. 

Comments:
   Though the IGRIDCONTAINER_GetWidget() function returns a pointer to a widget, the
   function does not add a reference to the returned IWidget for the caller.

   An application may iterate over the entire widget stack -- from bottom to top -- with the
   following code:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = IGRIDCONTAINER_GetWidget (pGridContainer, pWidget, TRUE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once IGRIDCONTAINER_GetWidget() returns NULL
===/pre>   
   
   To iterate over all the widgets on the widget stack from top to bottom, the application's
   code would look like the following:
   
===pre>   
       IWidget *pWidget = NULL;
       
       while (pWidget = IGRIDCONTAINER_GetWidget (pGridContainer, pWidget, FALSE, FALSE)) {
          //
          // Do fancy stuff with the widget pointer returned in pWidget
          //
       }
       // Drops out of the loop once IGRIDCONTAINER_GetWidget() returns NULL
===/pre>   

Side Effects: 
  None

See Also:
   IGridContainer Interface
   IWidget Interface

===========================================================================

IGRIDCONTAINER_GetGridDescriptors()

Description:
   This function will fetch the GridDescriptors or the number of rows and columns 
   for a grid container depending on how it is called (see below).

Prototype:
   void IGRIDCONTAINER_GetGridDescriptors(IGridContainer *po, GridDescriptor *pRows, uint16 *iRows,
                                          GridDescriptor *pCols, uint16 *iCols);

Parameters:
===pre>   
   p:      A pointer to an IGridContainer object.
   pRows:  This parameter can be either NULL or an array of GridDescriptors with length iRows,
           one for each row.  When this parameter is NULL, iRows becomes an "out" parameter, and it will
           be set to the number of rows in the grid container.
   iRows:  If pRows is NULL, this parameter will be set to the current number of rows.  If pRows is 
           not NULL i.e. contains an empty array of GridDescriptors, then this parameter should 
           specify the length of the array.
   pCols:  This parameter can be either NULL or an array of GridDescriptors with length iCols,
           one for each column.  When this parameter is NULL, iCols becomes an "out" parameter, and it will
           be set to the number of columns in the grid container.
   iCols:  If pCols is NULL, this parameter will be set to the current number of columns.  If pCols is 
           not NULL i.e. contains an empty array of GridDescriptors, then this parameter should 
           specify the length of the array.   
===/pre>   
   
Return Value:
   None

Comments:
   Getting the number of rows and columns can be accomplished two ways: (1) use IWIDGET_GetProperty()
   with either PROP_ROWS or PROP_COLS, or (2) call IGRIDCONTAINER_GetGridDescriptors().  Here's an
   example of how you could use IGRIDCONTAINER_GetGridDescriptors() to properly get the GridDescriptors
   from a grid container:
===pre>
   int FetchGridDescriptors(IGridContainer *picMyGrid) 
   {
      uint16 iRows, iCols;
      GridDescriptor *pRows, *pCols;
      IGRIDCONTAINER_GetGridDescriptors(picMyGrid, NULL, &iRows, NULL, &iCols);
      pRows = (GridDescriptor*)MALLOC(sizeof(GridDescriptor) * iRows);
      pCols = (GridDescriptor*)MALLOC(sizeof(GridDescriptor) * iCols);
      if (!pRows || !pCols)         //check for good malloc
         return EFAILED;

      IGRIDCONTAINER_GetGridDescriptors(picMyGrid, pRows, &iRows, pCols, &iCols);
      // Now, do something useful with the information 
      return SUCCESS;
   }
===/pre>

Side Effects: 
  None

See Also:
   IGridContainer Interface
   IGRIDCONTAINER_SetGridDescriptors()
   GridDescriptor
   
===========================================================================

IGRIDCONTAINER_SetGridDescriptors()

Description:
   In order to properly use a grid container, two GridDescriptors need to be set, one
   for rows and one for columns.  In order to set the GridDescriptors, create two arrays
   of GridDescriptors and initialize their data.  Each row/column will be described by
   a GridDescriptor within the array.  The zeroth element will correspond to the topmost row or
   leftmost column depending on whether you are describing a row or column.  The array
   describing the rows (pRows) should be of length iRows.  Similarly, the array describing 
   columns (pCols) should be of length iCols.  NULL may be passed in if you do not wish to
   set that descriptor e.g.:
===pre>
   IGRIDCONTAINER_SetGridDescriptors(picMyGrid, NULL, NULL, pCols, &iCols);
===/pre>
   In this case, only the column GridDescriptors will be set.  A layout will still be performed
   on the child widgets.  Also note that grid container makes a copy of the GridDescriptors 
   passed in, so it is not necessary to keep them in memory after this call. 

Prototype:
   int IGRIDCONTAINER_SetGridDescriptors(IGridContainer *po, GridDescriptor *pRows, uint16 *iRows,
                                         GridDescriptor *pCols, uint16 *iCols);

Parameters:
===pre>   
   p:      A pointer to an IGridContainer object.
   pRows:  An array of GridDescriptors of length iRows describing the rows of the grid container. 
   iRows:  The number of rows the grid container has as well as the length of pRows
   pCols:  An array of GridDescriptors of length iCols describing the columns of the grid container
   iCols:  The number of columns the grid container has as well as the length of pCols   
===/pre>   
   
Return Value:
===pre>
   SUCCESS:    Either the row or column grid descriptors were set properly.  This also indicates
               grid container performed a layout on the contained widgets.

   EFAILED:    Neither the row nor column grid descriptors were set because grid container
               could not allocate new grid descriptors or the grid descriptors were identical to the
               existing ones.  This return code indicates that a layout was not performed.
   
   ENOMEMORY:  This is a serious issue.  Grid container will set the number of rows/columns to zero 
               in order to prevent a segmentation fault.
===/pre>

Comments:
   Do not attempt to change the number of rows or columns a grid container has by simply calling this
   function with iRows/iCols and NULL GridDescriptor parameters (pRows and pCols).  If it were that
   straightforward, IWIDGET_SetProperty() would work for PROP_ROWS and PROP_COLS, which it does not.
   Grid container needs a valid array of GridDescriptors which corresponds to the new row/column size
   every time this function is called.
   
Side Effects: 
  None

See Also:
   IGridContainer Interface
   IGRIDCONTAINER_GetGridDescriptors()
   GridDescriptor

===========================================================================

IGRIDCONTAINER_SetVisibility()

Description:
   This function will change the visibility attributes of a given widget, and
   will optionally relocate that widget to a different location in the grid container's
   widget stack.  The widget to receive the new visibility may be specified explicitly by
   its widget pointer, or implicitly by a constant that identifies the widget's relative
   position within the container's widget stack.  The 'pb' parameter controls whether or
   not the widget's location in the stack will move.  Passing the constant WIDGET_ZNORMAL
   leaves the widget where it currently stands, without altering the order of the stack.
   Passing WIDGET_ZTOPMOST or WIDGET_ZBOTTOMMOST will move the widget to the top or
   bottom of the stack, while passing an explicit widget pointer will move the target
   widget to the stack location before the widget passed in 'pb'.
   
Prototype:
   int IGRIDCONTAINER_SetVisibility(IGridContainer *p, IWidget *pw, IWidget *pb, const boolean *pv);

Parameters:
===pre>   
   p:     A pointer to an IGridContainer object.
   pw:    Identifies the widget in the grid container's widget stack that will be the
          target of the visibility change and (optionally) relocation.  This widget may
          be expressed explicitly as a widget pointer, or implicitly as  a constant that
          identifies a relative position within the widget stack, as follows:
               WIDGET_ZNORMAL      - The target widget is the default widget, which BREW
                                     interprets to be the widget at the top of the widget
                                     stack.
               WIDGET_ZTOPMOST     - The target widget is the widget at the top of the
                                     widget stack.
               WIDGET_ZBOTTOMMOST  - The target widget is the widget at the bottom of the
                                     widget stack.
   pb:     Identifies a widget in the  grid container's widget stack that will serve as
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
   
   pv:   Pointer to a boolean, containing the new visibility value for the target widget.

===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The visibility attribute of the widget were successfully changed.
   EBADPARM  --  The widget pointers passed in 'pw' and/or 'pb' either do not exist, or
                 refer to widgets that are not in the grid container's widget stack.
===/pre>   

Comments:
   If IGRIDCONTAINER_SetVisibility() is being called to change the location of the widget within the
   grid container's widget stack without changing the visibility attributes of the widget,
   the caller MUST still pass in a pointer to the correct visibility attributes in the 'pv'
   parameter.  In such cases, the application should first call IGRIDCONTAINER_GetVisibility() to
   retrieve the current visibility attributes, then call IGRIDCONTAINER_SetVisibility() passing
   back the results received from IGRIDCONTAINER_GetVisibility().

Side Effects: 
   The cell size is still determined as if the widget were being drawn to the screen.  Setting a 
   widget to not visible simply means it does not draw.

See Also:
    IGridContainer Interface
    IWidget Interface
    IGRIDCONTAINER_GetVisibility()

===========================================================================

IGRIDCONTAINER_GetVisibility()

Description:
   This function retrieves a widget's visibility as a copy in the parameter
   'pv'. 

Prototype:
   int IGRIDCONTAINER_GetVisibility(IGridContainer *p, IWidget *pw, boolean *bv);

Parameters:
===pre>   
   p:     A pointer to an IGridContainer object.
   pw:    Identifies the widget in the grid container's widget stack whose visibility
          attribute should be retrieved.  This widget may be expressed explicitly as
          a widget pointer, or implicitly as  a constant that identifies a relative
          position within the widget stack, as follows:
               WIDGET_ZNORMAL      - Retrieve the visibility attribute for the default
                                     widget, which BREW interprets to be the widget at
                                     the top of the widget stack.
               WIDGET_ZTOPMOST     - Retrieve the visibility attribute for the widget
                                     at the top of the  widget stack.
               WIDGET_ZBOTTOMMOST  - Retrieve the visibility attribute for the widget
                                     at the bottom of the  widget stack.
   pv:    Pointer to a boolean which will be set with a copy of the current visibility
          attribute of the widget.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The visibility attribute of the widget were successfully retrieved.
   EBADPARM  --  The widget pointer passed in 'pw' either does not exist, or refers to a
                 widget that is not in the grid container's widget stack.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    IGridContainer Interface
    IWidget Interface
    IGRIDCONTAINER_SetVisibility()

===========================================================================

IGRIDCONTAINER_TO_ICONTAINER()

Description:
   This function provides type safe casting of an IGridContainer pointer to an IContainer
   pointer and should be used when passing an IGridContainer pointer to functions that
   require a pointer to an IContainer.  This is safer than a simple cast, since the compiler
   will verify the pointer type.

Prototype:
   IContainer *IGRIDCONTAINER_TO_ICONTAINER(IGridContainer *me)

Parameters:
===pre>
   me:  Pointer to an IGridContainer interface object.
===/pre>

Return Value:
   Returns 'me' cast to an IGridContainer pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IContainer Interface
   IGridContainer Interface
*/

#endif /* __AEEGRIDCONTAINER_H__ */
