/*
  ========================================================================

  FILE:  AEEViewportWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I decorate a widget that can be larger than me
  and offer a viewport onto that widget. I allow panning over the
  widget using the movement keys

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEVIEWPORTWIDGET_H__
#define __AEEVIEWPORTWIDGET_H__


#include "bid\AEECLSID_VIEWPORTWIDGET.bid"


// ViewportWidget flags
#define VWF_SIZETOFIT       1  // Resize contained widget to fill viewport if necessary


/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Viewport Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   viewport widget by creating a new instance of the Viewport Widget class, passing the
   ID of the class to ISHELL_CreateInstance().  The object that is created is, itself,
   a widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the viewport widget is defined below.

Definition:

Identifier                Definition
----------                -----------------------------------
AEECLSID_VIEWPORTWIDGET:  Identifies the class ID of the viewport widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()


===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Viewport Widget Class

Description:
   The viewport widget is a decorator that provides a visual view into some larger content
   area, sort of like a window that floats above the content area, limiting the current
   field of view, but with the capability of moving over the content to bring hidden
   content into view.  To illustrate, consider the following graphic image stored in a
   very large bitmap of, say 1024 x 1024 pixels:

===pre>   
     +------------------------------------------------------------------------+
     |                                                                        |
     |                                                                        |
     |                                                                        |
     |                             ..:::..                                    |
     |                           .!!!UWWX!.    ..!!!!!!.                      |
     |                          !!!W$$$$$B!. .!!!U$$$$W!!.                    |
     |                        :!!!MR##$$$R!!!!!!9$$$$$$E!!                    |
     |                       :!!!!!!!!!!!!!!!!!!!!!!!T!!!!:                   |
     |                      :!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                   |
     |                    .!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!:                  |
     |                   .!!!!!XU$$NU!!!!!!!!!!!!!!!!!!!!!!!.                 |
     |                 .!!!!!t$$$$$$$$W!!!!XW$$$$$$W!!!!!!!!!.                |
     |               .!!!:'   #$$$$$$$$$$$$$$$$$$$$$$X!!!!!!!!.               |
     |             .!!!!'         ?$$$$$$$$$$$$$$$$$$$W!!!!!!!!!.             |
     |           .!!!!.         :!!!??#"   #$$$$$#.#$$$R!!!!!!!!!.            |
     |             !!!!.     .!!!!!!!!:    .....!!!!!!!!!.. !!!!!!!.          |
     |              !!!!.  :!!!!!!!!!' .!!!!!!!!!!!!!!!!!!!! !!!!!'           |
     |               !!!!:!!!!!XW!!' :!!!!!!!!!!!!!!!!!!!! !!!!!'             |
     |                !!!!!!!W$R!! .!!!!!!!!!!!!W$?!!!!!! !!!!'               |
     |                !!!!!X$$%!! :!!!!!!!!!!!X$$!!!!!! !!!!!'                |
     |               !!!!!W$$?!! !!!!!!!!!!!!@$R!!!!! :!!!!!'                 |
     |             .!!!!!@$$!!! !!!!!!!!!!!!$$R!!!!! !!!!!!                   |
     |            .!!!!!$$$?!! !!!!!!!!!!!!$$R!!!!! !!!!!!                    |
     |           .!!!!!@$$%!! !!!!!!!!!!!!$$$!!!!! !!!!!!'                    |
     |           !!!!!9$$R!!'.!!!!!!!!!!!8$$!!!!! !!!!!!'                     |
     |          .!!!!X$$$!!! !!!!!!!!!!!t$$%!!!! .!!!!!:                      |
     |          !!!!!9$$R!!! !!!!!!!!!!X$$$!!!!'.!!!!!!                       |
     |         .!!!!!$$$!!!!!!!!!!!!!!!$$$%!!!! !!!!!!'                       |
     |         !!!!!!$$E!!!!!!!!!!!!!!9$$$!!!! .!!!!!:                        |
     |         !!!!!!!M!!!!!!!!!!!!!!t$$$!!!!! !!!!!:                         |
     |         !!!!!!!!!!!!!!!!!!!!!t$$$R!!!! !!!!!:                          |
     |         '!!!!!!!!!!!!!!!!!!!t$$$$!!!!! !!!!'                           |
     |          :!!!!!!!!!!!!!!!!!!$$$$!!!!! !!''                             |
     |           :!!!!!!!!!!!!!!!!!!$$!!!!!i                                  |
     |            '!!!!!!!!!!!!!!!!!!!!!!!'                                   |
     |             '!!!!!!!!!!!!!!!!!!!!;'                                    |
     |               '!!!!!!!!!!!!!!!!:'                                      |
     |                 '!!!!!!!!!!!!i'                                        |
     |                     ''''''                                             |
     |                                                                        |
     |                                                                        |
     +------------------------------------------------------------------------+
===/pre>   

    This image is likely much to large to show on the display all at once, so the
    widget intending to display this image could attach itself to a viewport
    decorator, which would then allow a portion of the widget to be viewed, like so:

===pre>   
     +------------------------------------------------------------------------+
     |                                                                        |
     |                                                                        |
     |                                                                        |
     |                             ..:::..                                    |
     |                           .!!!UWWX!.    ..!!!!!!.                      |
     |                          !!!W$$$$$B!. .!!!U$$$$W!!.                    |
     |                        :!!!MR##$$$R!!!!!!9$$$$$$E!!                    |
     |                       :!!!!!!!!!!!!!!!!!!!!!!!T!!!!:                   |
     |                      :!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                   |
     |                    .!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!:                  |
     |                   .!!!!!XU$$NU!!!!!!!!!!!!!!!!!!!!!!!.                 |
     |                 .!!!!!t$$$$$$$$W!!!!XW$$$$$$W!!!!!!!!!.                |
     |               .!!!:'   #$$$$$$$$$$$$$$$$$$$$$$X!!!!!!!!.               |
     |             .!!!!'         ?$$$$$$$$$$$$$$$$$$$W!!!!!!!!!.             |
     |           .!!!!.         :!!!??#"   #$$$$$#.#$$$R!!!!!!!!!.            |
     |             !!!!.     .!!!!!!!!:    .....!!!!!!!!!.. !!!!!!!.          |
     |              !!!!.  :!!!!!!!!!' .!!!!!!!!!!!!!!!!!!!! !!!!!'           |
     |               !!!!:!!!!!XW!!' :!!!!!!!!!!!!!!!!!!!! !!!!!'             |
     |                !!!!!!!W$R!! .!!!!!!!!!!!!W$?!!!!!! !!!!'               |
     |                !!!!!X$$%!! :!!!!!!!!!!!X$$!!!!!! !!!!!'                |
     |               !!!!!W$$?!! !!!!!!!!!!!!@$R!!!!! :!!!!!'                 |
     |             .!!!!!@$$!!! !!!!!!!!!!!!$$R!!!!! !!!!!!                   |
     |            .!!!!!$$$?!! !!!!!!!!!!!!$$R!!!!! !!!!!!                    |
     |           .!!!!!@$$%!! !!!!!!+--------------------------+              |
     |           !!!!!9$$R!!'.!!!!!!|!!!!8$$!!!!! !!!!!!'      |              |
     |          .!!!!X$$$!!! !!!!!!!|!!!t$$%!!!! .!!!!!:       |              |
     |          !!!!!9$$R!!! !!!!!!!|!!X$$$!!!!'.!!!!!!        |              |
     |         .!!!!!$$$!!!!!!!!!!!!|!!$$$%!!!! !!!!!!'        |              |
     |         !!!!!!$$E!!!!!!!!!!!!|!9$$$!!!! .!!!!!:         |              |
     |         !!!!!!!M!!!!!!!!!!!!!|t$$$!!!!! !!!!!:          |              |
     |         !!!!!!!!!!!!!!!!!!!!!|$$$R!!!! !!!!!:           |              |
     |         '!!!!!!!!!!!!!!!!!!!t|$$$!!!!! !!!!'            |              |
     |          :!!!!!!!!!!!!!!!!!!$|$$!!!!! !!''              |              |
     |           :!!!!!!!!!!!!!!!!!!|$!!!!!i                   |              |
     |            '!!!!!!!!!!!!!!!!!|!!!!!'                    |              |
     |             '!!!!!!!!!!!!!!!!|!!!;'                     |<-----+       |
     |               '!!!!!!!!!!!!!!|!:'                       |      |       |
     |                 '!!!!!!!!!!!!|'                         |      |       |
     |                     ''''''   |                          |      |       |
     |                              +--------------------------+      |       |
     |                                                                |       |
     +----------------------------------------------------------------|-------+
                                                                      |
                                                                      |
                                           Viewport Widget -----------+
                                         (displays only this
                                           portion of image)
===/pre>   
    
   Though the viewport widget overlays a much larger body of content, only the portion
   that lies within the viewport's extent would be transferred to the screen.  Continuing
   the example above, the user would see only the following portion of the total image
   on the display:
   
===pre>
     +--------------------------+
     |!!!!8$$!!!!! !!!!!!'      |
     |!!!t$$%!!!! .!!!!!:       |
     |!!X$$$!!!!'.!!!!!!        |
     |!!$$$%!!!! !!!!!!'        |
     |!9$$$!!!! .!!!!!:         |
     |t$$$!!!!! !!!!!:          |
     |$$$R!!!! !!!!!:           |
     |$$$!!!!! !!!!'            |
     |$$!!!!! !!''              |
     |$!!!!!i                   |
     |!!!!!'                    |
     |!!!;'                     |
     |!:'                       |
     |'                         |
     |                          |
     +--------------------------+
===/pre>
   
   Viewports are usually adorned with scrollbars or scroll indicators which would allow
   the user to -- conceptually -- scroll more (or less) of the tongue into view.  So,
   for example, as the user pressed the RIGHT navigation key, the viewport would
   slide to the right to reveal that content that is hidden to the right of the viewport.

   
   The viewport widget is created with the following default characteristics:
===pre>
           Border width:  0 pixels  (i.e. the border is hidden)
           Border color:  RGB_NONE
       Background color:  RGB_NONE
                 Origin:  (0, 0)
       Scroll Increment:  1 pixel
===/pre>
      
   To create an image widget, an application would do the following:

===pre>
       1. Create the content widget that will use the viewport as a decorator.
       2. Call ISHELL_CreateInstance() with a class ID of AEECLSID_VIEWPORTWIDGET to
          retrieve a reference counted instance of the viewport widget.
       3. Call IDECORATOR_SetWidget() to attach the viewport as a decorator of the
          widget providing the content to be viewed.
===/pre>

Supported Events:
   Like most decorators, the viewport widget processes events in three passes.  In the
   first pass the viewport widget will attempt to process any property events related to
   the widget's border.  If the event is handled during this first pass, the image widget
   will invalidate itself and be redrawn, using any updated border property values.  During
   the second pass, the viewport widget will attempt the handle events itself, including
   focus and key events, and  those event that set viewport specific properties.  Finally,
   events not handled during either of the first two passes will be passed to the decorator
   event handler.
   
   The events accepted by the viewport widget include:

===pre>
Event                   Description
-----                   ------------------------------------------------------------
EVT_WDG_SETFOCUS:       The viewport widget does nothing specific to handle this event,
                        but -- because the viewport's border will have returned FALSE after
                        handling the focus event, the viewport widget will respond by
                        returning TRUE. The child is also notified of the focus event, in case
                        it wants to deal with some of the events the viewport does not deal 
                        with.

EVT_KEY:                The viewport widget responds to the following EVT_KEY events:
                            AVK_UP
                            AVK_DOWN
                            AVK_LEFT
                            AVK_RIGHT
                        Each of these navigation events will attempt to move the viewport
                        in the corresponding direction, effectively scrolling the content
                        of the widget that the viewport decorates.  The degree to which the
                        content will scroll is determined by the PROP_INCREMENT property.
                        For example, if PROP_INCREMENT is set to 1, the viewport would 1
                        pixel in the direction identified by the event.  Moving the viewport
                        may alter the viewport's origin (represented by the PROP_OFFSETX
                        and PROP_OFFSETY properties) and -- if the origin has moved -- will
                        cause the viewport to invalidate itself and be redrawn.  Changing
                        the origin will also cause the viewport widget to send a scroll
                        event to any listening object (for example, an attached scrollbar
                        or scroll indicator).  The scroll event will specify an 'evCode'
                        of EVT_MDL_SCROLL_CHANGE and provide to the listening objects
                        information about the new position of the viewport.

EVT_WDG_SETPROPERTY:    The viewport widget responds to this event by attempting to set the
                        property identified by the 'wParam' parameter.  The viewport widget
                        allows the following properties to be set:
                      
                            PROP_INCREMENT  --  Sets the scroll increment of the viewport.
                            PROP_SCROLLREQ  --  Requests the viewport to send a scroll
                                                event to all listeners.
                            PROP_OFFSETX    --  Sets the horizontal offset of the viewport.
                            PROP_OFFSETY    --  Sets the vertical offset of the viewport.
                            PROP_FLAGS      --  Sets the viewport flags.

EVT_WDG_GETPROPERTY:    The viewport widget responds to this event by attempting to retrieve the
                        property identified by the 'wParam' parameter.  The viewport widget
                        allows the following properties to be retrieved:
                      
                            PROP_INCREMENT  --  Retrieves the scroll increment of the viewport.
                            PROP_OFFSETX    --  Retrieves the horizontal offset of the viewport.
                            PROP_OFFSETY    --  Retrieves the vertical offset of the viewport.
                            PROP_FLAGS      --  Retrieves the viewport flags.
                            PROP_VIEWMODEL  --  Gets the view model attached to the viewport
                                                widget.  Retrieving this property increments the 
                                                reference count of the view model.
===/pre>

Properties:
Property             Description
--------             ------------------------------------------------------------
PROP_INCREMENT:      This property contains the scroll increment of the viewport widget
                     The scroll increment is the number of pixels the viewport origin will 
                     move in one direction, in response to a scroll event.  For example, 
                     when the  property is set to 1, subsequent scrolling via EVT_KEY 
                     events will move the viewport 1 pixel in the direction specified by 
                     the key event. Typically, content that contains a graphic image such 
                     as a photograph or a map would use very small scroll increments of, 
                     perhaps, 1 pixel.  Text content, on the other hand, may set the scroll 
                     increment to the height of one line so that each scroll event moves 
                     the content one line at a time.
                 
                     PROP_INCREMENT applies the same value to scroll movement in both the 
                     horizontal and vertical direction (i.e. there are not separate values
                     for horizontal and vertical scroll changes).
                             
                         Property Value:  int
                                    
PROP_OFFSETX:        This property contains the horizontal offset of the viewport's origin.  
                     This value will be passed to listening objects in scroll events 
                     triggered by the viewport widget.  The default value for PROP_OFFSETX 
                     is 0 (i.e. the viewport is set overlaying the left edge of the widget 
                     it decorates.
                             
                         Property Value:  int
                                    
PROP_OFFSETX:        This property contains the vertical offset of the viewport's origin.  
                     This value will be passed to listening objects in scroll events 
                     triggered by the viewport widget.  The default value for PROP_OFFSETY 
                     is 0 (i.e. the viewport is set overlaying the top edge of the widget 
                     it decorates.
                             
                         Property Value:  int
                                    
PROP_VIEWMODEL:      This property contains a pointer to the view model that may be attached 
                     to the viewport widget, returning that pointer in an IModel pointer 
                     passed in the 'dwParam' parameter passed to the widget with 
                     EVT_WDG_GETPROPERTY.
                              
                     The viewport widget does not support attempts to set the PROP_VIEWMODEL 
                     property.
                             
                         Property Value:  IModel *
                     
PROP_SCROLLREQ:      This property does not possess any value.  It is used as a trigger to 
                     broadcast a scroll event to any object listening to the viewport's view 
                     model.  This provides an opportunity to listening objects to learn the 
                     current position of the viewport.
                              
                     The viewport widget does not support attempts to get the PROP_SCROLLREQ 
                     property.
                             
                         Property Value:  None

PROP_LAYOUTSTYLE:    This property contains the layout style for the viewport, which 
                     determines whether the viewport scrolls vertically, horizontally
                     or both. 
                     
                        Property Value:  LAYOUT_VERT - viewport scrolls vertically
                                         LAYOUT_HORZ - viewport scrolls horizontally
                                         LAYOUT_BOTH - viewport scrolls both vertically and horizontally
            
PROP_FLAGS:          This property contains a set of flags that determine the behavior of the viewport.
                                  
                     VWF_SIZETOFIT
                     -------------
                     When this flag is set, the viewport widget will resize the contained widget 
                     such that it fills the entire viewport extent.  This is only valid when the
                     contained widget extent is smaller than the viewport extent.

Required Model:
   IViewModel 

Model Data:
   Though the viewport model requires a view model, this model does not define any data used by
   either the viewport or the widgets the viewport decorates.  The view model is simply used as
   a conduit for sending scroll events to listening objects.
   
   The model used by the viewport widget is created by the widget itself as a default implementation.
   This model may be replaced by any client that wishes to provide their own model.  However, clients
   that provide their own view model interface to the viewport widget must abide by the event passing
   conventions described above, notifying listening objects of a EVT_MDL_SCROLL_CHANGE scroll event.
   
Comments:
   None
   
See Also:
   Model Events
   IWidget Interface
   ISHELL_CreateInstance()
   IDECORATOR_SetWidget()
                 





*/
#endif /* __AEEVIEWPORTWIDGET_H__ */
