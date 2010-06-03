/*
  ========================================================================

  FILE:  AEEFrameWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Widget for displaying image frames from sources like
  a camera or video.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEFRAMEWIDGET_H__
#define __AEEFRAMEWIDGET_H__


#include "AEECLSID_FRAMEWIDGET.BID"


/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Frame Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   frame widget by creating a new instance of the Frame Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the frame widget is defined below.

Definition:

Identifier            Definition
----------            -----------------------------------
AEECLSID_FRMAEWIDGET: Identifies the class ID of the frame widget
   
Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Frame Widget Class

Description:
   The frame widget is a user interface element for displaying frames from
   a media source such as video or camera.  This media source is contained in
   a frame model which sends events to the widget indicating the frame
   has changed.  When receiving the frame event, the widget requests
   the new frame from the model and displays it within the widget's extent.
   
   To create a frame widget, an application would do the following:
===pre>
       1. Create an instance of the frame widget using AEECLSID_FRAMEWIDGET.
       2. Configure the widget and set the extent using IWIDGET_SetExtent.
       3. Create an instance of IFrameModel and call IWIDGET_SetModel on the frame
          widget.
===/pre>

   The frame widget listens to the following model events:

===pre>
Event                   Description
-----                   ------------------------------------------------------------
EVT_FRAMEMDL_FRAME:     The current frame has changed.  Calls IFRAMEMODEL_GetFrame
                        and redraws the widget.         
===/pre>   

Supported Events:
   The frame widgets supports the following events:

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
PROP_BGCOLOR:         This property contains the background color that is drawn when
                      there is no frame image to display or the frame image is smaller
                      than the widget's extent.

                      Property Value:  RGBVal

PROP_EX:              FrameWidget supports PROP_EX for the following ClsId(s)

                      Property Id: PROPEX_DIRECT 
                      Property Value: int.
                      
                      By setting this extended property to TRUE, the widget's regular
                      draw routine is bypassed and the underlying interface(like IMedia) 
                      will be allowed to draw directly to the screen. Note that
                      the applications will have to disable this, or pause the 
                      media,when they want some other widget to draw on top of 
                      this FrameWidget.
                      
                      Before setting this property, 
                      - FrameWidget must have been inserted in Widget tree with a RootContainer.
                      - FrameModel must have been initialized with correct size by calling
                        IFRAMEMODEL_SetSize()
                      

                      Example:
                      int nDirect = DMF_ENABLE;
                      IFRAMEWIDGET_SetPropertyEx(piWidget, PROPEX_DIRECT, 
                              SIZEOF(int), (void*) &nDirect);

                     
                      If there is a layout change such that the position of the framewidget
                      changes, then this property should be set again.


Required Model:
   IFrameModel Interface
   
Comments:
   None
   
See Also:
   IFrameModel Interface
   IDirectMode Interface

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

*/

#endif      /* __AEEFRAMEWIDGET_H__ */

