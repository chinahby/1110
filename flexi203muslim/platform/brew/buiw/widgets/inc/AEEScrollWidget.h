/*
  ========================================================================

  FILE:  AEEScrollWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: We define scrollbar like widgets. A
  scrollindicator indicates whether there's more to scroll while the
  scrollbar is the more traditional way of indicating relative
  positioning. 

  A scroll widget can we wrapped around an existing widget to decorate
  the child with scroll visualization. A scroll widget can also be
  used separately and a PROP_VIEWMODEL set on it to set the model
  for it to use to visualize scroll position

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEESCROLLWIDGET_H__
#define __AEESCROLLWIDGET_H__

#include "AEEWidget.h"
#include "AEEWProperties.h"

#include "bid/AEECLSID_SCROLLBARWIDGET.bid"
#include "bid/AEECLSID_SCROLLINDICATORWIDGET.bid"

// ScrollbarWidget flags
#define SBWF_NOAUTOHIDE       1  // don't auto-hide the scrollbar when scrolling not possible
#define SBWF_ARROWS           2  // show arrows on scrollbars
#define SBWF_FOCUSTRACK       4  // scrollbar follows the focussed item

static __inline int IWIDGET_GetScrollbarRect(IWidget *po, AEERect *prc) {
   return IWIDGET_GetProperty(po, PROP_SCROLLBARRECT, (uint32*)prc);
}
static __inline int IWIDGET_GetVScrollbarRect(IWidget *po, AEERect *prc) {
   return IWIDGET_GetProperty(po, PROP_VSCROLLBARRECT, (uint32*)prc);
}
static __inline int IWIDGET_GetHScrollbarRect(IWidget *po, AEERect *prc) {
   return IWIDGET_GetProperty(po, PROP_HSCROLLBARRECT, (uint32*)prc);
}

static __inline int IWIDGET_GetScrollHandleRect(IWidget *po, AEERect *prc) {
   return IWIDGET_GetProperty(po, PROP_SCROLLHANDLERECT, (uint32*)prc);
}
static __inline int IWIDGET_GetVScrollHandleRect(IWidget *po, AEERect *prc) {
   return IWIDGET_GetProperty(po, PROP_VSCROLLHANDLERECT, (uint32*)prc);
}
static __inline int IWIDGET_GetHScrollHandleRect(IWidget *po, AEERect *prc) {
   return IWIDGET_GetProperty(po, PROP_HSCROLLHANDLERECT, (uint32*)prc);
}

static __inline int IWIDGET_GetScrollFillRect(IWidget *po, AEERect *prc) {
   return IWIDGET_GetProperty(po, PROP_SCROLLFILLRECT, (uint32*)prc);
}
static __inline int IWIDGET_GetVScrollFillRect(IWidget *po, AEERect *prc) {
   return IWIDGET_GetProperty(po, PROP_VSCROLLFILLRECT, (uint32*)prc);
}
static __inline int IWIDGET_GetHScrollFillRect(IWidget *po, AEERect *prc) {
   return IWIDGET_GetProperty(po, PROP_HSCROLLFILLRECT, (uint32*)prc);
}


/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Scrollbar Widget Class IDs

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   scrollbar or scrollbar indicator widget by creating a new instance of the Scroll
   Widget class, passing the desired class ID to ISHELL_CreateInstance().  The object
   that is created is, itself, a widget and may utilize those APIs defined for the
   IWidget interface.
   
   The class IDs used to identify the scroll widget are defined below.

Definition:

Identifier                       Definition
----------                       -----------------------------------
AEECLSID_SCROLLBARWIDGET:        Identifies the class ID of the scrollbar widget
AEECLSID_SCROLLINDICATORWIDGET:  Identifies the class ID of the scroll indicator widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Scroll Widget Class

Description:
   The scroll widget is derived from the decorator object and is typically wrapped around
   a content based widget to provide visual indications of position and proportion while
   the user navigates over the content.  For example, a scroll widget might be wrapped
   around a long series of text -- far more than could be displayed on a single page.  The
   scroll widget will indicate where the viewable text falls, relative to the entire body
   of text -- i.e. is the user viewing the text near the beginning, near the end, or
   somewhere in the middle.  The scroll widget may also indicate how much text remains to
   be viewed -- i.e. does the displayed text represent the majority of all the text, or is
   it just a small sliver of a much larger tome?
   
   The scroll widget class actually provides a pair of separate widgets for providing
   visual decoration of scrollable contents: the scrollbar, and the scroll indicator.

   To wrap a scrollbar widget around a content widget, an application would do the
   following:
===pre>
       1. Create the content widget that will be wrapped by a scrollbar.
       2. Call ISHELL_CreateInstance() with a class ID of AEECLSID_SCROLLBARWIDGET to
          retrieve a reference counted instance of the scrollbar widget.
       3. Call IDECORATOR_SetWidget() to wrap the scrollbar around the content widget.
       
===/pre>
   
===h3>
Scrollbar Widget
===/h3>
   The scrollbar widget is a decorator that provides visual indication of the position and
   proportion of the content the user is viewing on the display.  It can be aligned vertically
   or horizontally (or both) and is displayed in a fashion similar to scrollbars used in
   desktop GUIs.  

   Visually, the scrollbar consists of several parts, as illustrated by the diagram below
===pre>
              Up Arrow ---->  / \
                              +-+
                              | |
                              | |
        Fill Rectangle ---->  | |
                              | |
                              +-+
                Handle ---->  | |
                              +-+
                              | |
                              | |
                              | |
        Fill Rectangle ---->  | |
                              | |
                              +-+
            Down Arrow ---->  \ /
===/pre>

   On either end of the scrollbar is an arrow, indicating to the user how the contents may be
   scrolled -- up or down for a vertical scrollbar, left or right for a scrollbar aligned
   horizontally.  The rectangle that connects the arrows is the fill rectangle -- essentially,
   the background for the scrollbar.  Within the scrollbar is a "handle" that provides visual
   indication of the current position of the viewable portion of the content, as well as the
   proportion of the total content that is currently displayed.  For example, when the handle
   is at the top of a vertical scrollbar, the top portion of the content will be displayed.
   Likewise, when the handle is in the middle of the scrollbar, the user will be viewing the
   middle of the content.  The handle itself indicates how much overall content is currently
   in view.  When a small percentage of content is in view (say, one screen's worth of text 
   "War & Peace"), the handle will be proportionally small.  Conversely, when nearly all of
   the content is in view, the handle will be very large.  When all of the content is in view,
   the entire scrollbar is hidden, with the area formerly occupied by the scrollbar available
   to the widget being scrolled (i.e. the wrapped content widget).

   Many visual attributes of the scrollbar widget can be customized by setting various
   properties, as described in greater detail in the event and property sections below.  For
   example, to make the fill color used in the scrollbar a nice, lovely aquamarine, an
   application would call IWIDGET_SetProperty(), passing in a pointer to the scroll widget,
   PROP_SCROLLFILLCOLOR as the property to be set, and the RGB value for aquamarine.
   
   If no special properties are set, the scrollbar widget will have the following default
   appearance.

===pre>
                Scroll style:  Vertical
                Border color:  RGB_NONE (since the border width is zero)
         Active border width:  0 pixels
       Inactive border width:  0 pixel
         Active scroll color:  RGB_BLACK
       Inactive scroll color:  RGB_BLACK
         Active handle color:  RGB_WHITE
       Inactive handle color:  RGB_WHITE
                  Fill color:  Medium Gray (128, 128, 128)
                Handle Width:  3
                     Padding:  2
===/pre>
       
===h3>
Scroll Indicator Widget
===/h3>
   The scroll indicator widget is a decorator that provides visual indication that some viewable
   content may be scrolled.  The easiest way to think of a scroll indicator, is to imagine a
   scrollbar without all the juicy filling in the middle, leaving you with a pair of arrows
   like the up and down arrows in the figures below.
   
===pre>
              Up Arrow ---->  / \
                              +-+ 
            Down Arrow ---->  \ /
            
            
                      / \    <---- Up Arrow
            +---------+-+----------+
            |                      |
            |                      |
            |                      |
            |        Content       |
            |                      |
            |                      |
            |                      |
            +---------+-+----------+
                      \ /    <---- Down Arrow
===/pre>
   
   In the top diagram, the scroll indicator is drawn as a pair of up and down arrows, squished
   together, indicating that the content can be scrolled either up or down.  A widget such as
   this might be useful displayed beside an image that illustrates a gage (such as a volume
   control in a sound application).  The example on the bottom places the up and down arrows
   above and below some scrollable content -- maybe a map, or fabric swatches that the user can
   browse through.  Like the scrollbar widget, the scroll indicator widget may be aligned
   vertically, horizontally... or both.

   The scroll indicator widget maintains the following default characteristics, any of which
   may be overridden by changing the appropriate widget property.

===pre>
                Scroll style:  Vertical
                Border color:  RGB_NONE (since the border width is zero)
         Active border width:  0 pixels
       Inactive border width:  0 pixel
      Active indicator color:  RGB_WHITE
    Inactive indicator color:  RGB_NONE
                     Padding:  1
===/pre>

Resources:
   The graphic images used to represent the scroll arrows associated with a scrollbar are
   stored as resources in the "widgets.mif" file.  One resource is used to store both arrows
   of set -- i.e. the up and down arrows used at the ends of a vertical scrollbar are stored
   in the same resource as individual display "tiles".  The tiles are exactly the same size
   and are laid out in the resource one after the other in a horizontal strip, sort of like
   the frames of an animation sequence.  
   
   The resource and tile positions used to display the arrows of a scrollbar are defined as
   follows: 
   
===pre>
        Res ID      Tile    Image Represented
        ------      ----    -----------------
          203         0     Up arrow of the vertical scrollbar
          203         1     Down arrow of the vertical scrollbar

          204         0     Left arrow of the horizontal scrollbar
          204         1     Right arrow of the horizontal scrollbar
===/pre>

Supported Events:
   The scrollbar and scroll indicator widgets process events in three passes.  In the
   first pass the widget will attempt to process any property events related to the widget
   border.  If the event is handled during this first pass, the widget will invalidate
   itself and be redrawn, using any updated border property values.  In the second pass,
   the scroll widget will attempt to handle events that may be directly intended for the
   scroll widget itself.  Finally, events not handled during either of the first two passes
   will be passed to the parent decorator.

   It is important to note that the user does not interact directly with the scrollbar or
   scroll indicator widget -- neither widget can accept focus and the user does not "click on"
   the scroll arrows or otherwise manipulate the widgets in any way (as commonly experienced
   when interacting with scrolling controls in a desktop GUI).  Instead, the user scrolls
   concealed content into view by directly interacting with the content itself, generally by
   pressing the device navigation keys.  From the standpoint of the widget framework, these key
   events will be detected by the content widget itself, which then sends any listening objects
   an EVT_MDL_SCROLL_CHANGE ScrollEvent.  The scrollbar and scroll indicator widgets listen for
   such events and will respond to the event notification by invalidating themselves, thereby
   causing the widgets to be redrawn, reflecting the new scroll position.
  
   The scrollbar widget handles the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The scrollbar widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The scrollbar widget
                      allows the following properties to be set:
                      
                          PROP_SCROLLSTYLE                --  Sets the layout of the scrollbar to
                                                              vertical, horizontal or both.
                          PROP_SCROLLPAD                  --  Sets the distance between the scrollbar
                                                              and the widget it wraps.
                          PROP_SCROLLHANDLEWIDTH          --  Sets the width of the scrollbar handle.
                          PROP_ACTIVE_SCROLLCOLOR         --  Sets the border color to be used when
                                                              the scrollbar is active.
                          PROP_INACTIVE_SCROLLCOLOR       --  Sets the border color to be used when
                                                              the scrollbar is inactive.
                          PROP_SCROLLFILLCOLOR            --  Sets the fill color for the scrollbar.
                          PROP_SCROLLHANDLECOLOR          --  Sets the color of the scrollbar handle,
                                                              regardless of state.
                          PROP_ACTIVE_SCROLLHANDLECOLOR   --  Sets the handle color to be used when
                                                              scrollbar is active.
                          PROP_INACTIVE_SCROLLHANDLECOLOR --  Sets the handle color to be used when
                                                              scrollbar is inactive.
                          PROP_VIEWMODEL                  --  Sets the view model to be used by the
                                                              widget.
                          PROP_FLAGS                      --  Sets flags that effect how the scrollbar 
                                                              will behave.
                                                              
                      
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The scrollbar widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  These properties are
                      discussed below in greater detail.
                      
                          PROP_SCROLLSTYLE                --  Gets the layout for the scrollbar.
                          PROP_SCROLLPAD                  --  Gets the distance between the scrollbar
                                                              and the widget it wraps.
                          PROP_SCROLLHANDLEWIDTH          --  Gets the width of the scrollbar handle.
                          PROP_ACTIVE_SCROLLCOLOR         --  Gets the border color that will be used
                                                              when the scrollbar is active.
                          PROP_INACTIVE_SCROLLCOLOR       --  Gets the border color that will be used
                                                              when the scrollbar is inactive.
                          PROP_SCROLLFILLCOLOR            --  Gets the fill color that will be used
                                                              to draw the scrollbar.
                          PROP_SCROLLHANDLECOLOR          --  Gets the color that will be used to
                                                              draw the scrollbar handle.
                          PROP_ACTIVE_SCROLLHANDLECOLOR   --  Gets the color that will be used to
                                                              draw the scrollbar handle when the
                                                              scrollbar is active.
                          PROP_INACTIVE_SCROLLHANDLECOLOR --  Gets the color that will be used to
                                                              draw the scrollbar handle when the
                                                              scrollbar is inactive.
                          PROP_FLAGS                      --  Gets the flags that dictate how the
                                                              scrollbar will behave
===/pre>
    
  
   The scroll indicator widget handles the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The scroll indicator widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The scroll indicator, lacking
                      either a handle or a fill region, supports a smaller set of properties than
                      the scrollbar.  the scroll indicator widget allows the following properties to
                      be set:
                      
                          PROP_SCROLLSTYLE                --  Sets the layout of the scroll indicator
                                                              to vertical, horizontal or both.
                          PROP_SCROLLPAD                  --  Sets the distance between the scrollbar
                                                              and the widget it wraps.
                          PROP_SCROLLCOLOR                --  Sets the color of the scroll indicator,
                                                              regardless of state.
                          PROP_ACTIVE_SCROLLCOLOR         --  Sets the scroll indicator color to be
                                                              used when the scroll indicator is active.
                          PROP_INACTIVE_SCROLLCOLOR       --  Sets the scroll indicator color to be
                                                              used when the scroll indicator is inactive.
                          PROP_VIEWMODEL                  --  Sets the view model to be used by the
                                                              widget.
                                                              
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The scrollbar indicator responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  These properties are
                      discussed below in greater detail.
                      
                          PROP_SCROLLSTYLE                --  Gets the layout for the scroll indicator.
                          PROP_SCROLLPAD                  --  Gets the distance between the scroll
                                                              indicator and the widget it wraps.
                          PROP_ACTIVE_SCROLLCOLOR         --  Gets the color that will be used to draw
                                                              the scroll indicator when it is active.
                          PROP_INACTIVE_SCROLLCOLOR       --  Gets the color that will be used to draw
                                                              the scroll indicator when it is inactive.
                                                              
===/pre>

                      These properties are discussed below in greater detail.
    
Properties:
Property                          Description
--------                          ------------------------------------------------------------
PROP_SCROLLSTYLE:                 This property identifies the layout of the scrollbar or scroll
                                  indicator, as follows:
                               
                                  SCROLL_VERT
                                  -----------
                                  When PROP_SCROLLSTYLE is set to SCROLL_VERT, the widget will be laid
                                  out vertically.  For the scrollbar widget, this usually means that
                                  the widget will be laid out to the right of the content widget that
                                  it wraps, indicating that the contents may be scrolled up or down.
                                  A scroll indicator would also be laid out vertically, either beside
                                  the content it wraps, or "around" the content widget.  In the latter
                                  case, the up arrow would be placed above the content widget and 
                                  the down arrow would be placed below.
                               
                                  SCROLL_HORZ
                                  -----------
                                  When PROP_SCROLLSTYLE is set to SCROLL_HORZ, the widget will be laid
                                  out horizontally.  For the scrollbar widget, this usually means that
                                  the widget will be laid out below the content widget that it wraps,
                                  indicating that the contents may be scrolled left or right.  A scroll
                                  indicator would also be laid out horizontally, either beside the
                                  content it wraps, or "around" the content widget.  In the latter case,
                                  the left arrow would be placed to the left of the content widget and
                                  the right arrow would be placed to the right.
                               
                                  SCROLL_BOTH
                                  -----------
                                  When PROP_SCROLLSTYLE is set to SCROLL_BOTH, both a vertical and
                                  horizontal scrollbar or scroll indicator will be wrapped around the
                                  content widget, laid out as described above for SCROLL_VERT and
                                  SCROLL_HORZ.

PROP_SCROLLPAD:                   This property contains an integer value that species the distance,
                                  in pixels, between the scrollbar or scroll indicator and the widget
                                  being decorated.  For example, if a vertical scrollbar has been
                                  wrapped around a viewport widget, the PROP_SCROLLPAD property will
                                  indicate where the scrollbar will be drawn relative to the wrapped
                                  widget's bounding rectangle.  So, if PROP_SCROLLPAD is set to 2, the
                                  scrollbar will be drawn 2 pixels to the right of the viewport's right
                                  edge.
                             
                                    Property Value:  int
                      
PROP_SCROLLHANDLEWIDTH:           This property contains an integer value that species the width,
                                  in pixels, of the handle region of a vertical scrollbar, or the
                                  height of the handle region if the PROP_SCROLLSTYLE indicates that
                                  the scrollbar is horizontal.  The width of the scroll handle is
                                  actually measured as the number of horizontal (or vertical) pixels
                                  _inside_ the handle, exclusive of the handle's border and ultimately
                                  determines the width of the entire scrollbar.  The scrollbar width
                                  will be calculated as the handle width, plus the thickness of the
                                  scrollbar and handle borders -- which are always 1 pixel thick.
                                  The diagram below illustrates how the handle width determines the
                                  scrollbar width.  Here, the handle width is three pixels, again
                                  measuring just the number of pixels _inside_ the border of the
                                  handle region.  The left and right edges of the handle add one
                                  pixel apiece, as does the scrollbar border:
                                  
                                    Total scrollbar width = handle width + 4 pixels = 7 pixels.
                                  
                                    ------------------------ Border of the scrollbar
                                   |           |
                                   V           V
                                  +-+-+-+-+-+-+-+
                                  |*| | | | | |*|
                                  +-+-+-+-+-+-+-+
                                  |*| | | | | |*|
                                  +-+-+-+-+-+-+-+
                                  |*| | | | | |*|
                                  +-+-+-+-+-+-+-+
                                  |*|*|*|*|*|*|*|
                                  +-+-+-+-+-+-+-+
                                  |*|*| | | |*|*|
                                  +-+-+-+-+-+-+-+
                                  |*|*| | | |*|*|
                                  +-+-+-+-+-+-+-+
                                  |*|*| | | |*|*|
                                  +-+-+-+-+-+-+-+
                                  |*|*| | | |*|*|
                                  +-+-+-+-+-+-+-+
                                  |*|*| | | |*|*|
                                  +-+-+-+-+-+-+-+
                                  |*|*| | | |*|*|
                                  +-+-+-+-+-+-+-+
                                  |*|*|*|*|*|*|*|
                                  +-+-+-+-+-+-+-+
                                  |*| | | | | |*|
                                  +-+-+-+-+-+-+-+
                                  |*| | | | | |*|
                                  +-+-+-+-+-+-+-+
                                  |*| | | | | |*|
                                  +-+-+-+-+-+-+-+
                                       \   /
                                        \ /
                                         V__________________ Handle width set to 3 pixels
                                  
                                  This property is applicable to the scrollbar widget, but NOT
                                  the scroll indicator widget.
                                      
                                    Property Value:  int
                      
PROP_SCROLLCOLOR:                 For the scrollbar widget, this property contains the border color,
                                  while it contains the indicator color for the scroll indicator
                                  widget.  In both cases, the property is used as a shortcut to
                                  set the color to be used to draw the widget in both its active and
                                  inactive states.  When retrieving this property, the widget will
                                  return the current active color (PROP_ACTIVE_SCROLLCOLOR), which
                                  may actually be different than the inactive color.
                                         
                                    Property Value:  RGBVal
                      
PROP_ACTIVE_SCROLLCOLOR:          For the scrollbar widget, this property contains the color the
                                  widget should use to draw its border when the widget is active.
                                  For the scroll indicator widget, this property contains the color
                                  the widget should use to draw itself when the widget is active.
                                         
                                    Property Value:  RGBVal
                      
PROP_INACTIVE_SCROLLCOLOR:        For the scrollbar widget, this property contains the color the
                                  widget should use to draw its border when the widget is inactive.
                                  For the scroll indicator widget, this property contains the color
                                  the widget should use to draw itself when the widget is inactive.
                                         
                                    Property Value:  RGBVal
                      
PROP_SCROLLFILLCOLOR:             This property contains the color the scrollbar widget should use
                                  as the fill color when drawing the content of its fill rectangle.
                                         
                                    Property Value:  RGBVal
                      
PROP_SCROLLHANDLECOLOR:           This property is used as a shortcut to set the color to be used
                                  to draw the handle of a scrollbar widget in both its active and
                                  inactive states.  When retrieving this property, the scrollbar
                                  widget will return the current active color used to draw the
                                  scrollbar handle (PROP_ACTIVE_SCROLLHANDLECOLOR), which may
                                  actually be different than the inactive color.
                                         
                                    Property Value:  RGBVal
                      
PROP_ACTIVE_SCROLLHANDLECOLOR:    This property contains the color to be used to draw a scrollbar
                                  widget when the widget is active.
                                         
                                    Property Value:  RGBVal
                      
PROP_INACTIVE_SCROLLHANDLECOLOR:  This property contains the color to be used to draw a scrollbar
                                  widget when the widget is inactive.
                                         
                                    Property Value:  RGBVal
                      
PROP_VIEWMODEL:                   This property contains the view model that will be used by the
                                  scrollbar or scroll indicator widget to effect the visual scroll
                                  position.  Scroll widgets that are used separate from a wrapped
                                  content widget must set the model from which it will obtain its
                                  values.
                                         
                                    Property Value:  IModel *



PROP_SCROLLBARENDPAD              This property contains the amount of padding added to both
                                  ends of the scrollbars when they are displayed. When arrows
                                  are displayed (see flag SBWF_ARROWS), the actual pad value 
                                  will be the greater of PROP_SCROLLBARENDPAD and the length
                                  of the scrollbar arrows. In other words, the pad will be
                                  such that the scrollbar rectangles will never overlap the 
                                  scrollbar arrow glyphs.


PROP_FLAGS:                       This property contains a set of flags that determine the behavior
                                  of the scrollbar.
                                  
                                  SBWF_NOAUTOHIDE
                                  ---------------
                                  When this flag is set the scrollbar will always be visible, 
                                  even when the scroll range is such that scrolling is not 
                                  possible. By default, the scrollbar auto-hides when scrolling
                                  is not possible, This flag overrides that behavior
                                  
                                  
                                  SBWF_ARROWS
                                  -----------
                                  When this flag is set the scrollbar will be drawn with
                                  arrows on either end of the scroll rectangle. By default, 
                                  the scrollbar is not displayed with arrows.


                                  SBWF_FOCUSTRACK
                                  ---------------
                                  When this flag is set the scrollbar will listen for 
                                  EVT_MDL_FOCUS_CHANGE events and track the current focus 
                                  index instead the scroll position sent with the
                                  EVT_MDL_SCROLL_CHANGE event. For example, if the scrollbar
                                  with this flag set is installed on a list widget, the 
                                  scrollbar handle will move every time the selected list 
                                  item is changed by the user.
                                             


PROP_SCROLLBARRECT                These properties are used to retrieve the bounding rectangle 
PROP_VSCROLLBARRECT               of the scrollbar including the arrows, border and padding.  
PROP_HSCROLLBARRECT               PROP_SCROLLBARRECT is used to retrieve the scrollbar 
                                  rectangle in cases where PROP_SCROLLSTYLE is either SCROLL_VERT 
                                  or SCROLL_HORZ. In the case where PROP_SCROLLSTYLE is set to 
                                  SCROLL_BOTH, use PROP_VSCROLLBARRECT and PROP_HSCROLLBARRECT 
                                  to retrieve the scrollbar rectangles for the vertical and 
                                  horizontal scrollbars, respectively. 


PROP_SCROLLHANDLERECT             These properties are used to retrieve the rectangle of the 
PROP_VSCROLLHANDLERECT            scrollbar 'handle'. PROP_SCROLLHANDLERECT is used to retrieve 
PROP_HSCROLLHANDLERECT            the scrollbar handle rectangle in cases where 
                                  PROP_SCROLLSTYLE is either SCROLL_VERT or SCROLL_HORZ. In the 
                                  case where PROP_SCROLLSTYLE is set to SCROLL_BOTH, use 
                                  PROP_VSCROLLHANDLERECT and PROP_HSCROLLHANDLERECT to retrieve 
                                  the scrollbar rectangles for the vertical and horizontal 
                                  scrollbars, respectively. 


PROP_SCROLLFILLRECT               These properties are used to retrieve the rectangle of the 
PROP_VSCROLLFILLRECT              scrollbar fill area, which is the area inside the scrollbar 
PROP_HSCROLLFILLRECT              border area that contains the handle. PROP_SCROLLFILLRECT 
                                  is used to retrieve the scrollbar fill rectangle in cases 
                                  where PROP_SCROLLSTYLE is either SCROLL_VERT or SCROLL_HORZ. 
                                  In the case where PROP_SCROLLSTYLE is set to SCROLL_BOTH, 
                                  use PROP_VSCROLLFILLRECT and PROP_HSCROLLFILLRECT to 
                                  retrieve the scrollbar fill rectangles for the vertical 
                                  and horizontal scrollbars, respectively. 


Required Model:
   IModel (dummy)
   
Model Data:
   The scroll widget takes a dummy scroll model, primarily as an event driving mechanism for
   tracking and handling position changes to the scrollable content that a scroll widget
   wraps.  As these changes are detected, the wrapped widget will create the scroll model on
   demand and generate the scroll event and notification that will drive the scroll widget to
   update itself to reflect the user's movements through the scrollable content.
   
Comments:
   None
   
See Also:
   Model Events
   ScrollEvent
   ISHELL_CreateInstance()
   IWIDGET_GetViewModel()
   IWIDGET_SetViewModel()
   IDECORATOR_SetWidget()
   IWIDGET_SetProperty()
   

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_GetScrollbarRect()
IWIDGET_GetVScrollbarRect()
IWIDGET_GetHScrollbarRect()

Description:
   These functions retrieve the bounding rectangle of the scrollbar including
   the arrows, border and padding.  IWIDGET_GetScrollbarRect() is used to
   retrieve the scrollbar rectangle in cases where PROP_SCROLLSTYLE is 
   either SCROLL_VERT or SCROLL_HORZ. In the case where PROP_SCROLLSTYLE is
   set to SCROLL_BOTH, use IWIDGET_GetVScrollbarRect and IWIDGET_GetHScrollbarRect
   to retrieve the scrollbar rectangles for the vertical and horizontal scrollbars,
   respectively. 

Prototype:
   int IWIDGET_GetScrollbarRect(IWidget *po, AEERect *prc)
   int IWIDGET_GetVScrollbarRect(IWidget *po, AEERect *prc)
   int IWIDGET_GetHScrollbarRect(IWidget *po, AEERect *prc)

Parameters:
===pre>
   po:            Pointer to IWidget object (scrollbar widget)
   prc:           Pointer to an AEERect data structure.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to successfully retrieve the scrollbar rectangle.
   EFAILED - The widget was unable to retrieve the scrollbar rectangle.
===/pre>

Comments:
   IWIDGET_GetScrollbarRect will fail when called on a scrollbar widget with the
   layout style set to SCROLL_BOTH. Conversely, IWIDGET_GetVScrollbarRect and 
   IWIDGET_GetHScrollbarRect will fail when called on a scrollbar widget with the
   layout style not equal to SCROLL_BOTH.
   
Side Effects:
   None.

See Also
   PROP_SCROLLBARRECT
   PROP_VSCROLLBARRECT
   PROP_HSCROLLBARRECT
   

===========================================================================

IWIDGET_GetScrollHandleRect()
IWIDGET_GetVScrollHandleRect()
IWIDGET_GetHScrollHandleRect()

Description:
   These functions retrieve the rectangle of the scrollbar 'handle'.  
   IWIDGET_GetScrollHandleRect() is used to retrieve the scrollbar handle rectangle 
   in cases where PROP_SCROLLSTYLE is either SCROLL_VERT or SCROLL_HORZ. In the 
   case where PROP_SCROLLSTYLE is set to SCROLL_BOTH, use IWIDGET_GetVScrollbarRect 
   and IWIDGET_GetHScrollbarRect to retrieve the scrollbar rectangles for the 
   vertical and horizontal scrollbars, respectively. 

Prototype:
   int IWIDGET_GetScrollHandleRect(IWidget *po, AEERect *prc)
   int IWIDGET_GetVScrollHandleRect(IWidget *po, AEERect *prc)
   int IWIDGET_GetHScrollHandleRect(IWidget *po, AEERect *prc)

Parameters:
===pre>
   po:            Pointer to IWidget object (scrollbar widget)
   prc:           Pointer to an AEERect data structure.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to successfully retrieve the scrollbar handle rectangle.
   EFAILED - The widget was unable to retrieve the scrollbar handle rectangle.
===/pre>

Comments:
   IWIDGET_GetScrollHandleRect will fail when called on a scrollbar widget with the
   layout style set to SCROLL_BOTH. Conversely, IWIDGET_GetVScrollHandleRect and 
   IWIDGET_GetHScrollHandleRect will fail when called on a scrollbar widget with the
   layout style not equal to SCROLL_BOTH.

Side Effects:
   None.

See Also
   PROP_SCROLLHANDLERECT
   PROP_VSCROLLHANDLERECT
   PROP_HSCROLLHANDLERECT

===========================================================================

IWIDGET_GetScrollFillRect()
IWIDGET_GetVScrollFillRect()
IWIDGET_GetHScrollFillRect()

Description:
   These functions retrieve the rectangle of the scrollbar fill area, which is
   the area inside the scrollbar border area that contains the handle. 
   IWIDGET_GetScrollFillRect() is used to retrieve the scrollbar fill rectangle 
   in cases where PROP_SCROLLSTYLE is either SCROLL_VERT or SCROLL_HORZ. In the 
   case where PROP_SCROLLSTYLE is set to SCROLL_BOTH, use IWIDGET_GetVScrollFillRect
   and IWIDGET_GetHScrollFillRect to retrieve the scrollbar rectangles for the 
   vertical and horizontal scrollbars, respectively. 

Prototype:
   int IWIDGET_GetScrollFillRect(IWidget *po, AEERect *prc)
   int IWIDGET_GetVScrollFillRect(IWidget *po, AEERect *prc)
   int IWIDGET_GetHScrollFillRect(IWidget *po, AEERect *prc)

Parameters:
===pre>
   po:            Pointer to IWidget object (scrollbar widget)
   prc:           Pointer to an AEERect data structure.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to successfully retrieve the scrollbar handle rectangle.
   EFAILED - The widget was unable to retrieve the scrollbar handle rectangle.
===/pre>

Comments:
   IWIDGET_GetScrollFillRect will fail when called on a scrollbar widget with the
   layout style set to SCROLL_BOTH. Conversely, IWIDGET_GetVScrollFillRect and 
   IWIDGET_GetHScrollFillRect will fail when called on a scrollbar widget with the
   layout style not equal to SCROLL_BOTH.

Side Effects:
   None.

See Also
   PROP_SCROLLFILLRECT
   PROP_VSCROLLFILLRECT
   PROP_HSCROLLFILLRECT


===========================================================================
 */

#endif /* __AEESCROLLWIDGET_H__ */
