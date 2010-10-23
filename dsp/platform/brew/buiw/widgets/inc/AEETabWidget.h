/*
  ========================================================================

  FILE:  AEETabWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Tabbed decorator widget

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEETABWIDGET_H__
#define __AEETABWIDGET_H__

#include "AEEDecorator.h"

#include "bid\AEECLSID_TABWIDGET.bid"


#define TABWF_SHOWBORDER      1        // draw a border around the contained widget
#define TABWF_WRAP            2        // active tab wraps when moving past first or last item
#define TABWF_HASSELECTIMAGE  4        // tab images are composed of two tiles of equal
                                       //   size, the first displayed when the tab is in the 
                                       //   inactive (unfocused) state, and the other displayed 
                                       //   while active
#define TABWF_FILLWIDTH       8        // pad tabs so that all available width is filled
#define TABWF_SHOWTEXTALWAYS  16       // unselected tabs show text

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Tab Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   tab widget by creating a new instance of the Tab Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the tab widget is defined below.

Definition:

Identifier           Definition
----------           -----------------------------------
AEECLSID_TABWIDGET:  Identifies the class ID of the tab widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Tab Widget Class

Description:
   The tab widget is a decorator used to simulate the appearance of multiple tabs
   across an area of content.  For example, an address book application might want
   to display the information about each contact entry using a series of tabs -- one
   providing home contact information, another work data, and a third containing a
   fancy photo of the person.  The diagram below illustrates a typical tab widget:
   
===pre>
                                                                        
        +-----------------+ +-----------------+ +-----------------+
        |     Tab One     | |     Tab Two     | |    Tab Three    | <---- Tab Widget
        |                 | +-----------------+ +-----------------+
      +-+                 +-------------------------------------------+
      |                                                               |
      |                                                               |
      |                                                               | <--- Wrapped
      |                      Content for Tab One                      |      Content
      |                                                               | 
      |                                                               |
      |                                                               |
      +---------------------------------------------------------------+
      
                  A typical tab widget with attached content
===/pre>  

   In the diagram above, the tab widget rests above the content that will change as
   the user switches from tab to tab.  The tab widget itself consists of _only_ the
   tabs -- not the content displayed when a particular tab is selected.  Therefore,
   the tab widget, would look like:
   
===pre>
        +-----------------+ +-----------------+ +-----------------+
        |     Tab One     | |     Tab Two     | |    Tab Three    |
        |                 | +-----------------+ +-----------------+
      --+                 +--------------------------------------------
      
                          Tab widget all by itself
===/pre>  
   
   Note that the tab widget includes space for a "gutter" between the first tab and
   the widget's left border, and extends horizontally to the right border.  The
   selected tab is displayed "opened" as if it and the underlying content are layered
   above each of the remaining tabs.  The non-selected tabs are displayed within boxes that
   rest above the lower horizontal extent of the widget border.  Tabs may also be
   displayed with left or right scroll indicators, for those occasions where
   collection of all tabs is wider than the container into which the tabs are to be
   drawn.

   The tab widget renders each tab using a single wrapped widget that the tab widget
   creates and manages itself, internally.  By default, this rendering widget is an image
   static widget which therefore provides the ability for an application to display a
   label and an optional icon on each tab.  This is especially useful when several tabs
   are present, or if the extent of the constraining container is such that the label
   text won't fit, but an image will suffice to communicate the tab's contents.

   The data to be displayed on the label of each tab is, by default, stored in a vector
   model and it is up to the application using the tab widget to initialize the data in
   the model to contain the information to be displayed on each tab label.

   On a more macro level, applications typically wrap the tab widget around a content
   widget that becomes the destination for the content specific to each tab.  For example,
   the tab widget could be wrapped around a card container that would display the content
   of each selected tab.  Continuing the example introduced earlier, when the "Home" tab
   of the address book is selected, the content widget should display information pertaining
   to the a contact's home address and phone number.  Likewise, the "Work" tab would display
   information about how to contact that person when he or she is at work.  While the tab
   widget does not manage the content to be displayed _within_ each tab, it is responsible
   for displaying what is _on_ each tab -- i.e. the labels or icons displayed, as above, for
   tab one, two or three.
   
   To create a tab widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_TABWIDGET to retrieve a
         reference counted instance of the tab widget.
       2. Create the content widget that will be used to render the content of each tab
       3. Call IDECORATOR_SetWidget() to wrap the tab widget around the content widget
       4. Store the text and icon for each tab label in an ImageStaticInfo data structure
          and add this item to the tab widget's vector model.
===/pre>

   The tab widget, by default, will be created with the following characteristics:

===pre>
                  Tab Height:  20 pixels
                        Font:  AEECLSID_FONTSYSBOLD
                Border width:  1 pixel
                 Top Padding:  2 pixels
              Bottom Padding:  1 pixel
                Left Padding:  2 pixels
               Right Padding:  2 pixels
===/pre>

Supported Events:
   The tab widget processes events in two passes.  In the first pass the tab widget will
   attempt to handle events itself.  Events not handled during this first pass are then
   handed off to the widget's border.  The tab widget will accept the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_KEY:              The tab widget will respond to presses of AVK_LEFT and AVK_RIGHT by
                      attempting to change the focus to the tab on the left or right of the
                      current selected tab, scrolling the tabs as necessary to bring the new
                      selection into view, or wrapping around if the TABWF_WRAP flag is set.
                      For example, in a tab widget containing 458 tabs (and, really, we don't
                      advise creating 458 tabs), let's say the tab's horizontal extent allows
                      three tabs to be displayed and the current focus is on tab number 214.
                      Scroll indicators would be visible to the left and right of the tab
                      widget to indicate that additional tabs are hidden to the left and right.
                      Repeated pressing of AVK_LEFT will move the focus to the left to tab 213,
                      212, 211 and so forth, all the while scrolling tabs into view.  When the
                      focus arrives at tab number 1, the next press of AVK_LEFT will scroll
                      the tab at the far right (tab 458) into view if TABWF_WRAP is set.
                      Changing the tab focus will cause BREW to send an EVT_MDL_TAB_CHANGE
                      to any listening objects (such as the wrapped content widget) and trigger
                      the tab widget to invalidate its bounding rectangle and, therefore, be
                      redrawn.

EVT_WDG_SETFOCUS:     The tab widget responds to this event by returning TRUE, thereby 
                      accepting the focus.  Since the tab widget is a decorator, it also 
                      attempts to set focus to its child widget.
                      
EVT_WDG_SETPROPERTY:  The tab widget responds to this event by attempting to set the property
                      identified by the 'wParam' parameter.  The tab widget allows the following
                       properties to be set:
                      
                          PROP_VIEWMODEL  --  Sets the view model to be used by the widget.
                          PROP_FLAGS      --  Set various flags that effect how the tabs will
                                              behave and be displayed.
                          PROP_INDEXER    --  Sets the indexer descriptor to be used to
                                              iterate through each of the tabs in the widget.
      
EVT_WDG_GETPROPERTY:  The tab widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  These properties are
                      discussed below in greater detail.
                      
                          PROP_VIEWMODEL  --  Gets the view model attached to the tab widget.
                                              Retrieving this property increments the reference 
                                              count of the view model.
===/pre>   

Properties:
Property         Description
--------         ------------------------------------------------------------
PROP_VIEWMODEL:  This property contains a pointer to the view model that manages the current
                 selection within the tab widget, notifying attached objects as necessary so
                 that they might be updated to reflect the current tab selection.
                             
                     Property Value:  IModel *

                 
                 Note on view model usage: When a child widget is set on the tab widget 
                 decorator with IDECORATOR_SetWidget(), a model listener is added to the 
                 view model of the child widget, and the tab widget's view model is passed 
                 to the child with IWIDGET_SetViewModel(). If the child widget is "tab widget
                 aware", that is, if it listens for the EVT_MDL_TAB_CHANGE event, it will
                 have the effect of keeping the tab index of the tab widget and child widget
                 synchronized. Currently, the card container is the only widget that listens 
                 for EVT_MDL_TAB_CHANGE.
                 
                     
                     
PROP_FLAGS:      This property contains a set of flags that control how the tab widget will be
                 displayed and how it will respond to user interaction.  In addition to those
                 flags that will be handled by the widget's underlying border, the tab widget
                 defines the following widget specific flags:
                              
                   TABWF_SHOWBORDER
                   ----------------
                   When this flag is set the tab widget will be displayed by an enclosed
                   border.  The border is drawn using the current border properties for
                   color, line thickness, etc...  Note, however, that if the border color
                   has previously been set to RGB_NONE, clearing TABWF_SHOWBORDER will NOT
                   actually show the border.
                              
                   TABWF_WRAP
                   ----------
                   When this flag is set, tab selection will wrap from the first tab to the
                   last, or the last to the first in response to key presses at either end of
                   the row of tabs.  For example, when the user has moved all the way to the
                   left of a row of tabs, then attempts to move one more tab to the left, the
                   tab at the very end of the row will scroll into view -- though this behavior
                   will only have effect when the widget contains more tabs than can be
                   displayed within the horizontal boundaries of the widget.  Likewise, when
                   moving to the far right of the tab row, the next tab to be displayed would
                   be the tab at the very beginning of the row.  When this flag is not set,
                   any attempt to move the tab selection beyond the left or right tab will
                   stop without wrapping.
                              
                   TABWF_HASSELECTIMAGE
                   --------------------
                   When this flag is set, the image static widget used to render each tab
                   label, actually consists of a pair of images of precisely the same size. 
                   The first image will be displayed on the tab label when that tab is in
                   its unselected state -- the second image is displayed when the tab is
                   selected.  By default, this flag is OFF, indicating that the image static
                   owned by the tab widget contains only a single item to be displayed,
                   regardless of the current tab selection state.
                              
                                     
                   TABWF_FILLWIDTH
                   ---------------
                   When this flag is set the tabs will be drawn such that all available
                   space will be filled.  Normally, tabs are sized equally to accommodate the
                   widest label, and the tabs will all be displayed at this width regardless
                   of how many tabs there are to be displayed and how much horizontal space
                   has been granted to the widget, as illustrated below:


                           +-----------------+ +-----------------+
                           |     Tab One     | |     Tab Two     |
                           |                 | +-----------------+
                         --+                 +--------------------------------------------

                   Above, the two tabs are displayed at equal size, with the remaining space
                   empty.  With TABWF_FILLWIDTH set, the entire vertical extent of the widget
                   will be consumed by the tabs, with the selected tab growing to assume the
                   extra space, like so:
                     
                           +---------------------------------------+ +-----------------+
                           |                Tab One                | |     Tab Two     |
                           |                                       | +-----------------+
                         --+                                       +----------------------

                   By setting TABWF_FILLWIDTH, the selected tab is better able to display both
                   the label text and image.

                   TABWF_SHOWTEXTALWAYS
                   --------------------
                   When this flag is set the tab text will always be shown. By default,
                   all unselected tabs are sized to show an icon only. This flag forces the
                   unselected tabs to always be drawn full size and to display its text.
                     
                     Property Value:  uint32


PROP_INDEXER:    This property contains a pointer to an indexer descriptor that will be used
                 by the widget to iterate through the tabs managed by the tab widget.  The
                 indexer descriptor contains a pointer to a callback function that allows
                 clients of the tab widget to provide their own mechanism for iterating over
                 the individual tabs.

                     Property Value:  IndexerDesc *


PROP_FOCUSINDEX: This property selects or retrieves the currently active tab by index. Tab 
                 indexes are numbered from '0' to 'n' from left to right. In other words, if 
                 there are three tabs installed, the left-most tab is number '0', and the 
                 last or rightmost tab if number '2'. 
                 
                     Property Value: uint32


PROP_TABHEIGHT:  This property sets or retrieves a value used to set the tab height.  It
                 cannot be set to anything less than what is required to display the label.
                           
                     Property Value: int


PROP_TABSPACING_HORZ:   This property sets the amount of blank space between tabs, in pixels.
                           
                           Property Value: int
                           

PROP_TABSPACING_VERT:   This property sets the amount of blank space underneath tabs, in pixels.

                           Property Value: int
                           

PROP_ACTIVE_TABWIDTH:   This property sets or retrieves a value used to set the width of the
                        active tab.  By default, the item widget for the active tab is sized 
                        to its preferred extent.  By setting this property to a value greater
                        than zero, all active tabs will be displayed at this fixed width.

                           Property Value: int


PROP_INACTIVE_TABWIDTH: This property sets or retrieves a value used to set the width of the
                        inactive tab.  By default, the item widget for the intactive tab is 
                        sized to its preferred extent.  By setting this property to a value 
                        greater than zero, all inactive tabs will be displayed at this fixed 
                        width.
                           
                           Property Value: int

Required Model:
   IListModel
   
Model Data:
   Like the ListWidget, the TabWidget uses an item widget for drawing it's items. The
   item widget for the TabWidget is an ImageStaticWidget (AEECLSID_IMAGESTATICWIDGET). 
   So the model data that will be stored in the IListModel must be a pointer to an
   ImageStaticInfo data structure (defined in AEEImageStaticWidget.h). 

   
Comments:
   It is very important to note that the content that is displayed to reflect the current
   tab selection is NOT part of the tab widget itself.  The tab widget itself is ONLY that
   portion of the display that manages the tabs; not the content.  Applications must separately
   create an appropriate widget to dislpay content (such as the card container), then attach
   the tab widget as a decorator of this widget by calling IDECORATOR_SetWidget(), with the view
   model being identical for both the tab and the content it intended to display. 
 
See Also:
   Model Events
   ModelEvent
   ISHELL_CreateInstance()
   IDECORATOR_SetWidget()
   ImageStaticInfo
   AEEImageStaticWidget.h

*/
#endif   //__AEETABWIDGET_H__



