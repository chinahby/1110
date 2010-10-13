/*
  ========================================================================

  FILE:  AEEBitmapWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: BitmapWidget can display an IBitmap

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEEBITMAPWIDGET_H__
#define __AEEBITMAPWIDGET_H__

#include "AEEWidget.h"
#include "AEEWModel.h"

#include "bid/AEECLSID_BITMAPWIDGET.bid"


static __inline int IWIDGET_SetBitmap(IWidget *p, IBitmap *pib) {
   IInterfaceModel *piim;
   int nErr = IWIDGET_GetModel(p, AEEIID_INTERFACEMODEL, (IModel**)&piim);
   if (!nErr) {
      IINTERFACEMODEL_SetIPtr(piim, (IBase*)(void*)pib, AEECLSID_BITMAP);
      IINTERFACEMODEL_Release(piim);
   }
   return nErr;
}

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Bitmap Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   bitmap widget by creating a new instance of the Bitmap Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the bitmap widget is defined below.

Definition:

Identifier              Definition
----------              -----------------------------------
AEECLSID_BITMAPWIDGET:  Identifies the class ID of the bitmap widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Bitmap Widget Class

Description:
   The bitmap widget is used to display a bitmap image on the display.  For example,
   a photo of your on-line buddy could be rendered as a bitmap widget.  Likewise, an
   icon displayed in a list or an image displayed as part of a checkbox or radio
   button could all be managed by bitmap widgets.

   Applications specify the bitmap to be displayed in the widget by calling
   IWIDGET_SetBitmap(), passing in a pointer to the IBitmap image to be displayed.  The
   image itself will be displayed with its origin at the top-left corner of the widget's
   destination rectangle, cropped to the width and height of the widget's extent.  The
   image is neither stretched, shrunk nor scaled to fit into the destination
   rectangle.  The alignment of the bitmap image within the widget's destination 
   rectangle can be altered by the following flags:

===pre>
       Alignment Mask     Results
       --------------     -------
       IDF_ALIGN_LEFT     The image is displayed relative to the left coordinate of the
                          widget's border rectangle.  If the width of the image extends
                          beyond the border's right coordinate, it will be cropped on
                          the right side.
       IDF_ALIGN_RIGHT    The image is displayed relative to the right coordinate of the
                          widget's border rectangle.  If the width of the image extends
                          beyond the border's left coordinate, it will be cropped on the
                          left side.
       IDF_ALIGN_CENTER   The image is displayed horizontally centered in the widget's
                          border rectangle.  If the width of the image is wider than the
                          border rectangle, it will be cropped on both the left and right
                          sides.
       IDF_ALIGN_TOP      The image is displayed relative to the top coordinate of the
                          widget's border rectangle.  If the height of the image extends
                          below the border's bottom coordinate, it will be cropped on
                          the bottom side.
       IDF_ALIGN_BOTTOM   The image is displayed relative to the bottom coordinate of the
                          widget's border rectangle.  If the height of the image extends
                          above the border's top coordinate, it will be cropped on the
                          top side.
       IDF_ALIGN_MIDDLE   The image is displayed vertically centered in the widget's
                          border rectangle.  If the height of the image is larger than the
                          height of the border rectangle, the image will be cropped on both
                          the top and bottom sides.
===/pre>
   
   By default, the bitmap is aligned with the flags IDF_ALIGN_LEFT and IDF_ALIGN_TOP.  

   To create a bitmap widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_BITMAPWIDGET to
          retrieve a reference counted instance of the bitmap widget.
       2. Call IWIDGET_SetBitmap() to identify the IBitmap image to be displayed by
          the widget.
       3. If the bitmap is to be displayed with transparency, call IWIDGET_SetTranspColor()
          passing in the RGB value of the transparency color.
===/pre>

Supported Events:
   The bitmap widget handles a very small set of events, as described below.

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The bitmap widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The bitmap widget
                      allows the following properties to be set:
                      
                          PROP_TRANSP_COLOR   --  Sets the transparency color to be used
                                                  when transferring the bitmap image to
                                                  the screen.
                      
                      The bitmap widget will return FALSE after setting the specified
                      property, thereby allowing other objects in the hierarchy to also
                      process the event.
===/pre>   

Properties:
Property             Description
--------             ------------------------------------------------------------
PROP_TRANSP_COLOR:   This property determines how the bitmap will be transferred to the
                     display.  Set PROP_TRANSP_COLOR to RGB_NONE to copy the bitmap without
                     any transparency (i.e. every pixel in the source bitmapped will be
                     copied to the display), otherwise the bitmapped will be transferred
                     with transparency.

Required Model:
   IInterfaceModel
   
Model Data:
===pre>   
   IBitmap *modelData;
===/pre>
   
   The bitmap widget relies on an interface model that represents its data as a pointer
   to an IBitmap data structure.  The model stores only a pointer to the bitmap not a
   copy of the bitmap itself, and should always return this pointer in response to data
   queries from a client widget.
   
   A default model is created and assigned to the widget when the widget is created with 
   ISHELL_CreateInstance(). The client can replace the widget's model at any time with 
   the method: IWIDGET_SetModel(), as long as the supplied model interface is of the type 
   that the widget expects.   
   
Comments:
   None
   
See Also:
   Model Events
   ISHELL_CreateInstance()
   IWIDGET_SetBitmap()
   IINTERFACEMODEL_SetIPtr()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_SetBitmap()

Description:
   This function stores a pointer to a bitmap in the model used by a bitmap widget.
   Neither the widget nor the model make a copy of the data -- i.e. the data continues
   to be owned and should be managed by the object that calls IWIDGET_SetBitmap().

Prototype:
   int IWIDGET_SetBitmap(IWidget *p, IBitmap *pib);

Parameters:
===pre>   
   p:    A pointer to an IWidget object.
   pib:  Pointer to the bitmap image to be displayed by the bitmap widget.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS - The widget was able to successfully store the pointer to the bitmap
             to be displayed
   EFAILED - The attempt to set the bitmap has failed.
===/pre>   

Comments:
   The bitmap widget uses the IInterfaceModel interface to store the pointer passed to
   IWIDGET_SetBitmap.

Side Effects: 
   Setting the bitmap of a bitmap widget by calling IWIDGET_SetBitmap() will cause BREW
   to send an EVT_MDL_VALUE ModelEvent to any object that has registered as a listener
   with the IInterfaceModel.  Generally, this means that the bitmap widget will receive
   notification that its contents have changes and will redraw itself using the newly
   stored bitmap.
   
See Also:
    EVT_MDL_VALUE
    IInterfaceModel
    IWidget
    IBitmap

*/


#endif /* __AEEBITMAPWIDGET_H__ */
