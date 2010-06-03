/*
  ========================================================================

  FILE:  AEEProgressWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I indicate progress from 0 to 100% 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEPROGRESSWIDGET_H__
#define __AEEPROGRESSWIDGET_H__

#include "bid/AEECLSID_PROGRESSWIDGET.bid"

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Progress Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   progress widget by creating a new instance of the Progress Widget class, passing the
   ID of the class to ISHELL_CreateInstance().  The object that is created is, itself,
   a widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the progress widget is defined below.

Definition:

Identifier                Definition
----------                -----------------------------------
AEECLSID_PROGRESSWIDGET:  Identifies the class ID of the progress widget
   
Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Progress Widget Class

Description:
   The progress widget is used to display a simple progress indicator on the display
   that typically illustrate the "percentage complete" of a finite operation.  For
   example, an operation such as downloading a 120K graphic image might choose to
   display a progress indicator during the download to show the user how much of
   the download has completed, and how much remains.
   
   The progress widget is drawn as a rectangle, with some percentage of that rectangle
   filled, as indicated by the attached value model.  For example, an application might
   define the progress widget to be 8 pixels high by 80 pixels wide.  If the attached
   value model indicates that the tracked operation is 25% complete, the progress
   widget will draw an 8 x 80 pixel border, filled up to the 20 pixel mark (25% of the
   total width of 80 pixels).

   The progress widget is created with the following default characteristics:
   
===pre>
       Foreground color:  light gray
                Padding:  1
===/pre>

   To create a progress widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_PROGRESSWIDGET to
          retrieve a reference counted instance of the cursor widget.
       2. Set any properties the application may wish to use to enhance the progress
          widget's appearance (border color, fill color, etc).
===/pre>

Supported Events:
   The progress widget processes events in two passes.  In the first pass the progress
   widget will pass all events to the widget's border, and return TRUE if the border accepts and
   processes the event.  If the event is not handled, the progress widget will attempt to
   process the event itself, handling the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The progress widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The progress widget
                      recognizes those properties defined in the "Properties" section, below.

EVT_WDG_GETPROPERTY:  The progress widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  The progress widget
                      recognizes those properties defined in the "Properties" section, below.
===/pre>   

Properties:
Property              Description
--------              ------------------------------------------------------------
PROP_FGCOLOR:         This property contains the color that the progress widget will use to
                      draw the filled portion of its progress rectangle.  When setting this
                      property, the progress widget will invalidate itself so that the widget
                      is redrawn using the new color.

                      Property Value:  RGBVal

PROP_ANIMATE_FLAGS:   This property holds the flags related to progress widget's animation when the
                      model holds the value -1 i.e. the progress widget is displaying an infinitely-
                      looping progress bar. 

                      AF_ENABLE_EVT_STEP
                      ------------------
                      This flag allows the user to enable StepEvent to be sent through the
                      widget's ViewModel.  When progress widget animates, StepEvent will only
                      be passed to model listeners if this flag is set to true.
                    
                      Property Value:  uint32

Required Model:
   IValueModel
   
Model Data:
===pre>   
   int  modelData;  (should range from 0 to 100)
===/pre>
   
   The progress widget relies on a value model that represents its data as an integer
   ranging from 0 to 100 to indicate the "percent complete" of the operation being
   tracked.

   A special "indeterminate-progress" mode is supported. To trigger this mode, set the
   value model data to a negative value.  The progress widget will begin displaying an 
   animated progress bar repeatedly moving from left to right within the display area.
   The animation will continue until a positive value is set in the model.

   A default model is created and assigned to the widget when the widget is created with 
   ISHELL_CreateInstance(). The client can replace the widget's model at any time with 
   the method: IWIDGET_SetModel(), as long as the supplied model interface is of the type 
   that the widget expects.   
   
   The data supplied in the value model must be a pointer to type 'int'.


Comments:
   None

See Also:
   Model Events
   StepEvent
   ISHELL_CreateInstance()
*/

#endif /* __AEEPROGRESSWIDGET_H__ */
