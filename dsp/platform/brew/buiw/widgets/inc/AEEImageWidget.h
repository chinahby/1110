/*
  ========================================================================

  FILE:  AEEImageWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Defines a widget that displays an image

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEIMAGEWIDGET_H__
#define __AEEIMAGEWIDGET_H__

#include "AEEImage.h"
#include "AEEWidget.h"
#include "AEEWProperties.h"

#include "bid\AEECLSID_IMAGEWIDGET.bid"

// Define image interface for pre-2.1 SDKs
#ifndef AEEIID_IMAGE 
#define AEEIID_IMAGE  0x01013110
#endif

// Define IPARM_SCALE for pre-3.0 BREW builds
#ifndef IPARM_SCALE
#define IPARM_SCALE     12       // from BREW's 3.0 AEEImage.h
#endif

#define IMAGEPARM_Init(d,t,p1,p2) \
   (d)->parm = t, (d)->arg1 = (p1), (d)->arg2 = (p2)
   
   

// ImageWidget Flags
#define IWF_HASSELECTIMAGE       0x10000000  // image is composed of two tiles of equal
                                             // size, the first displayed in the unselected
                                             // state, and the other displayed in the 
                                             // selected state. (Set by PROP_SELECTED)
#define IWF_TILED                0x01000000  // tiled image mode



static __inline int IWIDGET_SetImage(IWidget *p, IImage *piImage) {
   IInterfaceModel *piim;
   int result;
   result = IWIDGET_GetModel(p, AEEIID_INTERFACEMODEL, (IModel**)&piim);
   if (result == SUCCESS) {
      IINTERFACEMODEL_SetIPtr(piim, (IBase*)piImage, AEEIID_IMAGE);
      IINTERFACEMODEL_Release(piim);
   }
   return result;
}

static __inline int IWIDGET_SetImageParm(IWidget *po, ImageParm *p) {
   return IWIDGET_SetProperty(po, PROP_IMAGE_PARM, (uint32)p);
}

static __inline int IWIDGET_SetImageOffset(IWidget *po, uint32 x, uint32 y) {
   ImageParm parm;
   IMAGEPARM_Init(&parm, IPARM_OFFSET, x, y);
   return IWIDGET_SetImageParm(po, &parm);
}

static __inline int IWIDGET_SetImageSize(IWidget *po, uint32 dx, uint32 dy) {
   ImageParm parm;
   IMAGEPARM_Init(&parm, IPARM_SIZE, dx, dy);
   return IWIDGET_SetImageParm(po, &parm);
}

static __inline int IWIDGET_SetNFrames(IWidget *po, int nFrames) {
   ImageParm parm;
   IMAGEPARM_Init(&parm, IPARM_NFRAMES, nFrames, 0);
   return IWIDGET_SetImageParm(po, &parm);
}

static __inline int IWIDGET_GetNFrames(IWidget *po, int *pnNFrames) {
   return IWIDGET_GetProperty(po, PROP_IMAGE_NFRAMES, (uint32 *)pnNFrames);
}

static __inline int IWIDGET_SetImageFrame(IWidget *po, int nFrame) {
   return IWIDGET_SetProperty(po, PROP_IMAGE_FRAME, (uint32)nFrame);
}

static __inline int IWIDGET_GetImageFrame(IWidget *po, int *pnFrame) {
   return IWIDGET_GetProperty(po, PROP_IMAGE_FRAME, (uint32 *)pnFrame);
}

static __inline int IWIDGET_SetRate(IWidget *po, uint16 nRate) {
   ImageParm parm;
   IMAGEPARM_Init(&parm, IPARM_RATE, nRate, 0);
   return IWIDGET_SetImageParm(po, &parm);
}

static __inline int IWIDGET_SetROP(IWidget *po, uint32 rop) {
   ImageParm parm;
   IMAGEPARM_Init(&parm, IPARM_ROP, rop, 0);
   return IWIDGET_SetImageParm(po, &parm);
}

static __inline int IWIDGET_SetScale(IWidget *po, uint32 dx, uint32 dy) {
   ImageParm parm;
   IMAGEPARM_Init(&parm, IPARM_SCALE, dx, dy);
   return IWIDGET_SetImageParm(po, &parm);
}

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Image Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates an
   image widget by creating a new instance of the Image Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the image widget is defined below.

Definition:

Interface ID           Definition
------------           -----------------------------------
AEECLSID_IMAGEWIDGET:  Identifies the class ID of the image widget
AEEIID_IMAGE:          Identifies the image widget interface ID for versions of BREW
                       prior to 2.1.
   
Comments:
   AEEIID_IMAGE should be used to create instances of the image widget class on
   versions of BREW prior to 2.1.

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

=============================================================================

ImageParm

Description:
   This data type defines a data structure used to access the individual attributes
   of a bitmap image.  These attributes indicate how the bitmap will be displayed or
   how it will behave.  For example, the size and position of an image are each
   separately represented by image attributes, as are the rate of animation and the
   destination display.
   
   ImageParam is, essentially, the basis for a parameter block interface for setting
   or retrieving each attribute.  The data structure itself defines a single image
   attribute -- identifying the attribute being accessed and up to two arguments that
   together define the value for that parameter.  To illustrate, the image size is
   identified by the IPARM_SIZE parameter, with the horizontal width and vertical
   height being the arguments of that parameter.  Likewise, to specify how the bitmap
   is to be transferred to the display, the data structure would identify IPARM_ROP
   as the attribute parameter, with the raster operation to be performed passed as
   the lone argument.

   Each image parameters is discussed in detail in AEEImage.h.

Definition:
   typedef struct {
      int  parm;
      int  arg1;
      int  arg2;
   } ImageParm;

Members:
===pre>
   parm:  Identifies the image parameter to be set or retrieved.  The meaning of 'arg1'
          and 'arg2' depends on the parameter being accessed.  See AEEImage.h for the
          complete list of image parameters.
   arg1:  Identifies the first parameter argument.  The meaning of this argument is
          dependent on the attribute defined in the 'parm' field.  For example, when
          'parm' is set to IPARM_SIZE, 'arg1' contains the horizontal width of the image.
          However, when 'parm' is set to IPARM_OFFSET, 'arg1' contains the horizontal
          position where the image is to be displayed within its graphics port.
   arg2:  Identifies the second parameter argument.  The meaning of this argument is
          dependent on the attribute defined in the 'parm' field.  For example, when
          'parm' is set to IPARM_SIZE, 'arg2' contains the vertical height of the image.
          However, when 'parm' is set to IPARM_OFFSET, 'arg2' contains the vertical
          position where the image is to be displayed within its graphics port.
===/pre>

Comments:
   Though ImageParm is primarily used to set the value of a given image attribute, it
   can also be used to retrieve various values.  For example, the AEERasterOp associated
   with an image may be retrieved by setting 'parm' to IPARM_GETROP.  When the widget
   receives an EVT_WDG_SETPROPERTY event identifying PROP_IMAGE_PARM as the property to
   be set, the image widget will actually interpret this event as a request to retrieve
   the underlying image's AEERasterOp, and will return a pointer to the AEERasterOp, as
   well as a result code, in the 'arg1' and 'arg2' fields of the ImageParm data structure
   passed in with the event.
   
   See AEEImage.h for a complete discussion of the parameters and value arguments that
   define each image attribute.

See Also:
   AEEImage.h

===================================================================================
   MACRO DOCUMENTATION
===================================================================================
IMAGEPARM_Init()

Description:
   This macro initializes each of the fields of an ImageParm data structure.

Prototype:
   IMAGEPARM_Init(ImageParm *d, int t, int p1, int p2)

Parameters:
===pre>
   d:   Pointer to the ImageParm data structure to be initialized
   t:   Identifies the image parameter to be accessed by the data structure being
        initialized.
   p1:  Identifies the first argument associated with the chosen image parameter.
   p2:  Identifies the second argument associated with the chosen image parameter.
===/pre>

Return Value:
   None

Comments:
   See AEEImage.h for a complete discussion of the parameters and value arguments that
   define each image attribute.

Side Effects: 
   None

See Also:
   ImageParm

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Image Widget Class

Description:
   The image widget is used to create an object in the widget framework that
   contains an IImage bitmap or animated bitmap graphic image.  For example, the
   image of a map showing the directions to a fancy downtown club could be
   displayed using an image widget.  An application could also choose to use an
   image widget to provide a background graphic for a form or dialog.
   
   The graphic image displayed by an image widget is provided by the model attached
   to the widget when it is created.  Therefore, as the data in the model changes,
   the image represented by the image widget will be updated on the display.
   
   An application may specify the image to be displayed by the image widget by calling
   IWIDGET_SetImage(), passing in a pointer to the IImage image to be displayed.
   The image itself will be displayed within the rectangle that defines the widget's
   border, positioned by default with its origin aligned to the upper left corner of
   the border, clipped to the widget's extent.  The position of the image can be altered
   by changing the setting the PROP_FLAGS property, specifying horizontal and vertical
   alignments, as follows:

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
   
   The image is always displayed at its full size, cropped to the clip rectangle of the
   destination port.  The size, position, animation rate and many other attributes of the
   image may be further customized by calling IWIDGET_SetImageParm(), passing in an ImageParm
   date structure that identifies the attribute to be set, along with the attribute's
   associate value.  See AEEImage.h for a discussion of each of the image parameters that
   can be set.

   The image widget assumes the IImage to be displayed has been fully loaded and decoded.
   Some image formats such as png, jpg and gif require the image data to be decompressed
   before being displayed.  When loading these images via BREW's ISHELL_LoadResImage() or
   related routines, it is the application's responsibility to use IIMAGE_Notify() to
   wait for the image decoding to complete before passing the IImage to the image widget 
   for display.  If the application fails to do this, screen updates immediately following
   an image change may not happen as expected.

   Image tiling is also supported by providing the ability to copy (tile) a source image
   multiple times across a larger region. Tiling can be simple multiple copies of the source,
   or it can be a bordered tile pattern.
   
   The following diagram shows a source image that consists of a 3x3 grid of colored squares.
===pre>
   
   +---+---+---+
   | 1 | 2 | 3 |
   +---+---+---+
   | 4 | 5 | 6 |
   +---+---+---+
   | 7 | 8 | 9 |
   +---+---+---+

===/pre>

   If the tile source image were to simply fill an Image Widget, the result would be
   the following:
===pre>

   +---+---+---+---+---+---+
   | 1 | 2 | 3 | 1 | 2 | 3 |
   +---+---+---+---+---+---+
   | 4 | 5 | 6 | 4 | 5 | 6 |
   +---+---+---+---+---+---+
   | 7 | 8 | 9 | 7 | 8 | 9 |
   +---+---+---+---+---+---+
   | 1 | 2 | 3 | 1 | 2 | 3 |
   +---+---+---+---+---+---+
   | 4 | 5 | 6 | 4 | 5 | 6 |
   +---+---+---+---+---+---+
   | 7 | 8 | 9 | 7 | 8 | 9 |
   +---+---+---+---+---+---+
   
===/pre>
   
   In addition to simple tiling, the tile element can preserve the border of a source image.
   Such tiles are specified with the border width attributes bdt, bdr, bdb and bdl. In the 
   example of simple tiling above, the values of bdt, bdb, bdr, and bdl would all be zero.
   By default, the values of bdt, bdb, bdr, and bdl are zero, which essentially means simple
   tiling is the default behavior of Image Widget when tiling is enabled and these values have
   not been explicitly set.  If the border attributes are used on this example such that the
   top, left, right and bottom border widths all fit the internal squares, then the region
   is tiled as follows:
===pre>

   +---+---+---+---+---+---+
   | 1 | 2 | 2 | 2 | 2*| 3 |
   +---+---+---+---+---+---+
   | 4 | 5 | 5 | 5 | 5*| 6 |
   +---+---+---+---+---+---+
   | 4 | 5 | 5 | 5 | 5*| 6 |
   +---+---+---+---+---+---+
   | 4 | 5 | 5 | 5 | 5*| 6 |
   +---+---+---+---+---+---+
   | 4*| 5*| 5*| 5*| 5*| 6*|
   +---+---+---+---+---+---+
   | 7 | 8 | 8 | 8 | 8*| 9 |
   +---+---+---+---+---+---+
   
===/pre>
   
   The cells marked with a asterisk (*) might be clipped in order to fill the area.
   The usage of the border attributes is defined by the following diagram:
===pre>
          bdl     bdr
         |---|   |---|

       - +---+---+---+
   bdt | | 1 | 2 | 3 |
       - +---+---+---+
         | 4 | 5 | 6 |
       - +---+---+---+
   bdb | | 7 | 8 | 9 |
       - +---+---+---+
===/pre>
   In order to set the value of bdt, bdb, bdl, or bdr, use IWIDGET_SetProperty() with 
   one of the following prop flags and the pixel distance(uint16):
===pre>
   PROP_TILED_BDT
   PROP_TILED_BDB
   PROP_TILED_BDL
   PROP_TILED_BDR
===/pre> 

   Image tiling also supports a selected image.  Use the selected image just as you would 
   in an non-tiled Image Widget.  

   The widget interface provides APIs that allow applications to directly set many of the
   attributes associated with an image, as well as interfaces to start or stop animating
   the image.
   
   The image widget is created with the following default characteristics:
===pre>
           Border width:  0
       Background color:  RGB_NONE
                Padding:  0
           Transparency:  0 (fully opaque)
===/pre>
      
   To create an image widget, an application would do the following:

===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_IMAGEWIDGET to
          retrieve a reference counted instance of the image widget.
       2. Call IWIDGET_SetImage()  to identify the image to be displayed by the widget.
       3. Call IWIDGET_SetFlags() if the application wishes to override the default
          alignment of the image within its border.
===/pre>

Supported Events:
   The image widget processes events in three passes.  In the first pass the image widget
   will attempt to process any property events related to the widget's border.  If the
   event is handled during this first pass, the image widget will invalidate itself and
   be redrawn, using any updated border property values.  The second pass attempts to
   handle events that may be directly intended for the image widget, including those events
   that set image specific properties and the selection state of the widget.  Finally,
   events not handled during either of the first two passes will be passed to the parent
   decorator.
  
   The image widget handles the following events in the described manner:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The image widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The image widget
                      allows the following properties to be set:
                      
                          PROP_SELECTED       --  If the widget supports selection (as
                                                  indicated by the IWF_HASSELECTIMAGE
                                                  flag) this property will toggle the
                                                  current selection state on screen.
                          PROP_IMAGE_PARM     --  Retrieve the current image's AEERasterOp.
                          PROP_IMAGE_ANIMATE  --  Sets the current animation state of the
                                                  image being displayed.
                          PROP_ANIMATE        --  Same as PROP_IMAGE_ANIMATE
                          PROP_IMAGE_FRAME    --  Sets which frame of the image contained
                                                  within the Image Widget is drawn.
                          PROP_FLAGS          --  Set various image widget flags.
                          PROP_ANIMATE_FLAGS  --  Set various animation-specific flags.
                      
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The image widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  These properties are
                      discussed below in greater detail.
                      
                          PROP_FLAGS          --  Retrieves various image widget flags.
                          PROP_IMAGE_ANIMATE  --  Retrieves the animation state of the image
                                                  being displayed
                          PROP_ANIMATE        --  Same as PROP_IMAGE_ANIMATE
                          PROP_IMAGE_FRAME    --  Retrieves which frame of the image 
                                                  contained in the Image Widget is drawn.
                          PROP_IMAGE_NFRAMES  --  Restrieves the number of frames in
                                                  the image contained in the Image Widget.
                          PROP_ANIMATE_FLAGS  --  Retrieves the animation flags.
===/pre>
===pre>
Properties:
Property             Description
--------             ------------------------------------------------------------
PROP_SELECTED:       Though this property contains the selection state of the image widget, the
                     actual toggling of the property value is handled by the widget border, with
                     the widget border's event handler returning FALSE to allow child objects to
                     also handle the event.  The image widget will respond to requests to set this
                     property by togging the image display from its selected to unselected states
                     (or vice versa), but only if the image widget supports selection (as indicated
                     by the IWF_HASSELECTIMAGE flag).
                             
                         Property Value:  boolean (handled by border widget)

PROP_IMAGE_PARM:     This property contains a pointer to an ImageParm data structure, which
                     provides access to a single attribute of the image wrapped within the image
                     widget.  Setting this property effectively queries the image to interact with
                     one of the image attributes -- which could result in either setting that
                     attribute, or, actually retrieving the value of the attribute that parameter
                     represents.  For example, when the 'parm' field is set to IPARM_OFFSET, the
                     image wrapped by the image widget, would set its offset to the horizontal and
                     vertical coordinates identified by the 'arg1' and 'arg2' fields.  See
                     AEEImage.h for a discussion of each of the image parameters that can be
                     accessed when setting the  PROP_IMAGE_PARM property.  Note that some viewers 
                     may not support all IPARM parameters.  The image widget only responds to 
                     requests to set this property.
                             
                         Property Value:  ImageParm *

PROP_IMAGE_ANIMATE:  This property contains a boolean flag indicating whether or not animation of
                     the image widget should be started or stopped.  Image Widget responds to both
                     set and get of this property.
                             
                         Property Value:  boolean

PROP_TILED_BDT:      This property defines the border distance top (bdt) for a tiled Image Widget.
                     See the class description for usage of this property in context.

                         Property Value:  uint16

PROP_TILED_BDB:      This property defines the border distance bottom (bdb) for a tiled Image Widget.
                     See the class description for usage of this property in context.

                         Property Value:  uint16

PROP_TILED_BDL:      This property defines the border distance left (bdt) for a tiled Image Widget.
                     See the class description for usage of this property in context.

                         Property Value:  uint16

PROP_TILED_BDR:      This property defines the border distance right (bdr) for a tiled Image Widget.
                     See the class description for usage of this property in context.

                         Property Value:  uint16

PROP_IMAGE_FRAME:    This property specifies which frame of the contained IImage the Image Widget
                     will draw..

                         Property Value:  int


PROP_FLAGS:          This property contains a set of flag masks that dictate how an
                     image widget will behave.  These flags are passed in the 'dwParam' of the
                     event.  The following flags are identified for the image widget:
                              
                       IWF_HASSELECTIMAGE
                       ------------------
                       When this flag is set the image displayed by the image widget actually
                       consists of a pair of images of precisely the same size.  The first
                       image will be displayed by the image widget when the widget is in its
                       unselected state -- the second image is displayed when the image is
                       selected.  By default, this flag is OFF, indicating that the image
                       contains only a single item to be displayed, regardless of the current
                       selection state.
                              
                         Property Value:  uint32

                       IWF_TILED
                       ---------
                       This flag indicates that the image displayed by image widget should
                       be tiled inside the widget's extent.  The image will fill the full
                       extent of the widget.  If the image does not does not proportionately 
                       fit inside the extent, the image will be clipped to fit.  For more
                       information about the layout of basic tiled mode, see the Image
                       Widget class description. 

                         Property Value:  uint32

PROP_ANIMATE_FLAGS:  This property holds the flags related to image widget's animation.

                        AF_ENABLE_EVT_STEP
                        ------------------
                        This flag allows the user to enable StepEvent to be sent through the
                        widget's ViewModel.  When image widget animates, StepEvent will only
                        be passed to model listeners if this flag is set to true.
                       
                         Property Value:  uint32

===/pre>

Required Model:
   IInterfaceModel (default)
   
Model Data:
===pre>   
   IImage *modelData;
===/pre>
   
   The image widget relies on an interface model that represents its data as a pointer
   to an IImage data structure.  The model stores only a pointer to the image not a
   copy of the image itself, and should always return this pointer in response to data
   queries from a client widget.

   The IImage pointer passed as the model data must point to an image that has been
   fully loaded and decoded.  It is the application's responsibility to wait for
   image formats such as png, jpg and gif to complete decoding before passing them
   to the image widget.  See the BREW documentation on IImage and IIMAGE_Notify()
   for details.
   
   The image widget may be created with a specific interface model identified, rather
   than the default interface model assigned by the widget framework.  Clients that
   identify their own model interface to be attached to the image widget must abide by
   the data conventions described above, returning a pointer to an IImage when queried
   for the model data. 
   
Comments:
   None
   
See Also:
   Model Events
   ISHELL_CreateInstance()
   IWIDGET_SetImage()
   IWIDGET_SetFlags()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_SetImage()

Description:
   This function will set the image to be displayed within an image widget.  
   The pointer passed in 'piImage' will be stored in the interface model 
   associated with the widget and it's reference count will be incremented.

   Note:  If you intend to scale the image, be sure to pass the unscaled 
   image to IWIDGET_SetImage() then call IWIDGET_SetScale() to perform the 
   scale.  Scaling the image via IIMAGE_SetParm() before (or after) passing 
   it to this routine will cause undesirable behavior, particularly if the 
   image is scaled beyond the extent of the image widget.

   Note:  The IImage passed to IWIDGET_SetImage() must have been 
   completely loaded and decoded.  It is the application's responsibility
   to wait for image formats such as png, jpg and gif (and any media type 
   that requires decoding) to completely decode before passing them to the 
   image widget.  For  example, if an app uses ISHELL_LoadResImage() to 
   load a jpg image from a resource file, it must do an IIMAGE_Notify() to 
   wait for the image to completely load before passing the resulting 
   pointer to IWIDGET_SetImage().  For more details on IIMAGE_Notify, see 
   the BREW documentation on the IImage interface.

Prototype:
   int IWIDGET_SetImage(IWidget *p, IImage *piImage)

Parameters:
===pre>
   p:        Pointer to the image widget object.
   piImage:  Pointer to an IImage that contains the image to be displayed by the
             image widget.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully store 'piImage' in the model associated
             with the image widget.
   EFAILED - If the image could not be set or if the image widget utilizes a model
             other than the interface model.
===/pre>

Comments:
   None 

Side Effects: 
   Calling IWIDGET_SetImage() will result in the widget redrawing itself with the new
   image.

See Also:
   IWIDGET_SetScale()
   BREW IImage Interface

==========================================================================

IWIDGET_SetImageOffset()

Description:
   This function is used to set the offset into the image.

Prototype:
   int IWIDGET_SetImageOffset(IWidget *po, uint32 x, uint32 y)

Parameters:
===pre>
   po:     Pointer to the IWidget object.
   x:      x offset.
   y:      y offset.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the offset.
   EFAILED - If the offset could not be set.
===/pre>

Comments:
   None  

Side Effects: 
   IWIDGET_SetImageOffset() calls IWIDGET_SetImageParm() to access the IPARM_OFFSET
   image attribute.  This has the net effect of processing an EVT_WDG_SETPROPERTY
   event through the widget hierarchy.

See Also:
   EVT_WDG_SETPROPERTY
   IWidget
   IWIDGET_SetImageParm()

===================================================================================

IWIDGET_SetImageParm()

Description:
   This function provides an interface to setting or retrieving many of the individual
   attributes of a widget's underlying image.  For example, IWIDGET_SetImageParm() could be
   used to establish the position of an image within its enclosing border, or used to
   retrieved the AEERasterOp identifying how the pixel data of the image will behave.
   Image attributes are managed through a parameter block interface, with the parameter
   block identifying the attribute to be accessed, as well as the values associated with
   that attribute.  Again, to illustrate, the attribute of image size is identified by
   the constant IPARM_SIZE, as well as a pair of values that specify the horizontal
   width and vertical height of the image.  These items are collected in the ImageParm
   data structure, with this structure then passed to IWIDGET_SetImageParm() to set (or get)
   a given attribute.

Prototype:
   int IWIDGET_SetImageParm(IWidget *po, ImageParm *p)

Parameters:
===pre>
   po:  Pointer to the image widget object.
   p:   Pointer to an ImageParm data structure identifying the image attribute to be set
        (or retrieved when the 'parm' field is IPARM_GETROP).  For those 'parm' values that 
        set image attributes, this data structure will also contain, as arguments, the value 
        for the associate attribute.  These argument fields serve as pointers to returned 
        values when the 'parm' value indicates a request to retrieve attribute values from 
        the image.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set or retrieve the requested image attribute.
   EFAILED - If the requested image attribute could not be set or retrieved.
===/pre>

Comments:
   The behavior of IWIDGET_SetImageParm() depends on the image attribute identified in the 'parm'
   field of the parameter block passed to the function in the 'p' parameter, as presented in
   the following table.  Image Widget only supports a subset of all IImage IPARM parameters.  
   Note that some viewers might not support all IPARM parameters listed below.  

===pre>
      'parm' field       Behavior of IWIDGET_SetImageParm()
      ------------       -----------------------------
      IPARM_SIZE         This parameter will set the width and height of the image bitmap
                         to the values defined in the 'arg1' and 'arg2' fields.
                         
                             arg1  -  Contains the width of the image
                             arg2  -  Contains the height of the image
                         
                         Note, however, that changing the bitmap dimensions does not scale
                         the image to fit the requested size.  Rather, the size passed in
                         'arg1' and 'arg2' define a section of the overall bitmap to be used
                         in image operations, effectively providing a mechanism for cropping
                         a bitmap's contents.  For example, a single bitmap image of 64 x 64
                         pixels could actually be the concatenation of four separate 32 x 32
                         pixel images, each occupying a quadrant of the overall image.  An
                         application could identify a portion of the larger image by setting
                         the IPARM_SIZE of the image to 32 x 32 to indicate that only a portion
                         of the larger bitmap is to be used for image operations.  Normally,
                         the image size is set to the full dimensions of the bitmap.
                             
      IPARM_OFFSET       This parameter will set the horizontal and vertical offset of the
                         image's origin within the stored bitmap, to the values defined in
                         the 'arg1' and 'arg2' fields.
                         
                             arg1  -  Contains the horizontal coordinate for the origin
                             arg2  -  Contains the vertical coordinate for the origin
                         
                         Carrying forward the example presented above, consider a 64 x 64
                         pixel bitmap containing four separate 32 x 32 images.  Any of the
                         four smaller sub-images could be accessed by setting the IPARM_SIZE
                         attribute to 32 x 32, and the IPARM_OFFSET parameter to different
                         points of origin:
                           -  The upper left image would leave the IPARM_OFFSET at (0, 0)
                           -  The upper right image would set the IPARM_OFFSET to (32, 0)
                           -  The lower left image would set the IPARM_OFFSET to (0, 32)
                           -  The lower right image would set the IPARM_OFFSET to (32, 32)
                             
      IPARM_CXFRAME      This parameter will set the width, in pixels, of each frame in an
                         animated bitmap.  For example, an animated bitmap that stores all
                         of its frames in a single 512 x 80 pixel bitmap might indicate a
                         frame width of 64 pixels -- meaning that each frame of the animation
                         is 64 pixels wide, by the bitmap height of 80 pixels.  All told,
                         this would specify 8 frames of animation (512 pixels divided by 64
                         pixels).  The frame size would be specified in the 'arg1' field,
                         with the 'arg2' field being ignored:
                         
                             arg1  -  Contains the width of each animation frame
                             arg2  -  Ignored
                             
      IPARM_ROP          This parameter specifies the raster operation that will be applied
                         when transferring the bitmap to the display.
                         
                             arg1  -  Contains a constant identifying how each pixel of the
                                      image should be logically combined with those already
                                      appearing on the display.  See the description of
                                      IPARM_ROP in AEEImage.h for a full description of the
                                      available transfer modes and constants.
                             arg2  -  Ignored
                             
      IPARM_NFRAMES      This parameter will set the number of frames that together comprise
                         the frames of an animated bitmap.  For example, an animated bitmap
                         that stores all of its frames in a single 512 x 80 pixel bitmap might
                         indicate that the animation sequence consists of 8 frames -- meaning
                         that each frame of the animation will be 64 pixels wide (512 pixels
                         divided by 8 frames equals 64 pixels per frame).  The number of frames
                         would be specified in the 'arg1' field with the 'arg2' field being
                         ignored:
                         
                             arg1  -  Contains the number of animation frames contained in the
                                      bitmap.
                             arg2  -  Ignored
                             
                         Note that when the number of animation frames is specified explicitly,
                         the size of each frame (represented by IPARM_CXFRAME) is calculated.
                             
      IPARM_RATE         This parameter will set the rate of animation -- i.e. the time, in
                         milliseconds, to be observed between each frame of the animation.  The
                         rate is specified in the 'arg1' field and the 'arg2' field is ignored:
                         
                             arg1  -  Contains the number of milliseconds that should be
                                      observed between frames while displaying the frames of an
                                      animated bitmap.
                             arg2  -  Ignored
                             
      IPARM_OFFSCREEN    This parameter is not currently supported.
                         
                             arg1  -  Ignored
                             arg2  -  Ignored
                             
      IPARM_REDRAW       This parameter will set the private redraw callback for a given image.
                         BREW will issue this callback whenever an update is needed, thereby
                         providing the image widget the opportunity to perform any special
                         processing needed when an update occurs.
                         
                             arg1  -  Contains a pointer to the redraw callback defined as a
                                      PFNNOTIFY function.
                             arg2  -  Contains a pointer to the image widget itself.
                             
      IPARM_GETROP       This parameter is used to retrieve the raster operation constant that
                         will be applied to the pixels of the bitmap when it is transferred to
                         the display.  The arguments identified in the data structure are
                         actually used as destinations for the data returned by the call to
                         IWIDGET_SetImageParm().
                         
                             arg1  -  Contains a pointer to an AEERasterOp into which the
                                      current transfer mode will be copied.
                             arg2  -  Contains a pointer to an integer into which the
                                      results of fetching the raster op will be copied.
                                      If NULL is passed for 'arg2', no results are returned.

      IPARM_SCALE        This parameter will scale the width and height of the image bitmap
                         to the values defined in the 'arg1' and 'arg2' fields.
                         
                             arg1  -  Contains the scaled width of the image
                             arg2  -  Contains the scaled height of the image
                         
                         Note, image scaling is only supported on BREW versions 3.0 and newer.  

===/pre>
   

Side Effects: 
   IWIDGET_SetImageParm() calls IWIDGET_SetProperty() to access a specific image attribute.  This
   has the net effect of processing an EVT_WDG_SETPROPERTY event through the widget hierarchy.

See Also:
   EVT_WDG_SETPROPERTY
   IWidget
   IWIDGET_SetProperty()
   IBitmap
   AEERasterOp
   PFNNOTIFY

==========================================================================

IWIDGET_SetImageSize()

Description:
   This function is used to set the (cropped) size of the image.

Prototype:
   int IWIDGET_SetImageSize(IWidget *po, uint32 dx, uint32 dy)

Parameters:
===pre>
   po:     Pointer to the IWidget object.
   dx:     Width to crop.
   dy:     Height to crop.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the size.
   EFAILED - If the size could not be set.
===/pre>

Comments:
   Set dx and dy to zero to restore image to full size (not cropped).  

Side Effects: 
   IWIDGET_SetImageSize() calls IWIDGET_SetImageParm() to access the IPARM_SIZE image attribute.
   This has the net effect of processing an EVT_WDG_SETPROPERTY event through the widget
   hierarchy.

See Also:
   EVT_WDG_SETPROPERTY
   IWidget
   IWIDGET_SetImageParm()

==========================================================================

IWIDGET_SetNFrames()

Description:
   This function is used to set the number of frames contained in the animation sequence
   of an animated bitmap.  For example, an animated bitmap that stores all of its frames
   in a single 512 x 80 pixel bitmap might indicate that the animation sequence consists
   of 8 frames -- meaning that each frame of the animation will be 64 pixels wide (512
   pixels divided by 8 frames equals 64 pixels per frame).

Prototype:
   int IWIDGET_SetNFrames(IWidget *po, int nFrames)

Parameters:
===pre>
   po:       Pointer to the IWidget object.
   nFrames:  Specifies the number of frames contained in the animation sequence.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the number of animation frames
   EFAILED - If the number of frames could not be set.
===/pre>

Comments:
   None  

Side Effects: 
   Changing the number of animation frames will recalculate the frame size.  For example,
   if a 1024 x 48 bitmap is originally set to contain 16 frames of animation, each frame
   would be 64 pixels wide (1024 pixels divided by 16 frames).  If IWIDGET_SetNFrames()
   is then called with 'nFrames' set to 32, the width of each frame changes proportionally
   from 64 pixels to 32 pixels (1024 pixels divided by 32 frames).
   
   IWIDGET_SetNFrames() calls IWIDGET_SetImageParm() to access the IPARM_NFRAMES image attribute.
   This has the net effect of processing an EVT_WDG_SETPROPERTY event through the widget
   hierarchy.

See Also:
   EVT_WDG_SETPROPERTY
   IWidget
   IWIDGET_SetImageParm()

==========================================================================

IWIDGET_GetNFrames()

Description:
   This function is used to retrieve the total number of frames of the
   image contained and drawn by the ImageWidget. 

Prototype:
   int IWIDGET_GetNFrames(IWidget *po, int *pnNFrames)

Parameters:
===pre>
   po:         Pointer to the IWidget object.
   pnNFrames:  On SUCCESS, the total number of frames in the image
===/pre>

Return Value:
===pre>
   SUCCESS -   If this call was able to successfully get the frame count
   EFAILED -   If the frame count could not be retrieved.
===/pre>

Comments:
   None  

Side Effects: 
   None

See Also:
   EVT_WDG_GETPROPERTY
   IWidget
   IWIDGET_GetProperty()
   IWIDGET_SetNFrames()

==========================================================================

IWIDGET_SetImageFrame()

Description:
   This function is use to set which frame of a multiple-framed image
   to render when the ImageWidget draws itself.

Prototype:
   int IWIDGET_SetImageFrame(IWidget *po, int nFrame)

Parameters:
===pre>
   po:       Pointer to the IWidget object.
   nFrame:   Specifies which frame in the animation sequence to render
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the frame
   EFAILED - If the was out of range and frame could not be set.
===/pre>

Comments:
   None  

Side Effects: 
   If successful, the next time the Image Widget renders its image, the
   frame set in this call will be rendered.

See Also:
   EVT_WDG_SETPROPERTY
   EVT_WDG_GETPROPERTY
   IWidget
   IWIDGET_SetProperty()
   IWIDGET_GetProperty()

==========================================================================

IWIDGET_GetImageFrame()

Description:
   This function is used to get which frame of a multiple-framed image
   will be rendered when the ImageWidget draws itself.

Prototype:
   int IWIDGET_GetImageFrame(IWidget *po, int *pnFrame) 

Parameters:
===pre>
   po:       Pointer to the IWidget object.
   pnFrame:  On SUCCESS, the current image frame
===/pre>

Return Value:
===pre>
   SUCCESS - If this call was able to successfully get the frame
   EFAILED - If the current frame could not be retrieved.
===/pre>

Comments:
   None  

Side Effects: 
   None

See Also:
   EVT_WDG_GETPROPERTY
   IWidget
   IWIDGET_GetProperty()
   IWIDGET_SetImageFrame()

==========================================================================

IWIDGET_SetRate()

Description:
   This function is used to set the rate of animation for the frames stored within an
   image widget's bitmap, thereby controlling the speed of the animation.  When an
   animation sequence is displayed, frames are copied to the display one after the
   other, with the next frame in the sequence displayed after this specified time has
   passed.  So, for example, with the animation rate set to 150 milliseconds, the second
   frame will be displayed 150 milliseconds after the first, the third 150 milliseconds
   after the second, and so forth.

Prototype:
   int IWIDGET_SetRate(IWidget *po, uint16 nRate)

Parameters:
===pre>
   po:     Pointer to the IWidget object.
   nRate:  Specifies the number of milliseconds to wait between animation frames.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the animation rate.
   EFAILED - If the animation rate could not be set.
===/pre>

Comments:
   The animation rate defaults to 150 milliseconds.  Setting this rate to values less
   than the default will result in faster animation, while higher rates result in
   slower animation.  

Side Effects: 
   IWIDGET_SetRate() calls IWIDGET_SetImageParm() to access the IPARM_RATE image attribute.
   This has the net effect of processing an EVT_WDG_SETPROPERTY event through the widget
   hierarchy.

See Also:
   EVT_WDG_SETPROPERTY
   IWidget
   IWIDGET_SetImageParm()

==========================================================================

IWIDGET_SetROP()

Description:
   This function is used to specify the raster operation for an image widget's bitmap.
   The raster operation specifies how the pixels of a bitmap will be transformed when
   they are transferred to the display.  For example, a raster operation of AEE_RO_COPY
   would copy the pixels of the source bitmap directly to the screen, regardless of the
   pixels in the destination graphics port.  More complex behaviors can be performed by
   specifying various raster operations, each of which identifies the logical relation
   between the source image and its destination.  Illustrating, again, a raster
   operation of AEE_RO_OR would logically 'or' each pixel of the source bitmap with the
   corresponding pixel of the destination.
   
   See the description of IPARM_ROP in AEEImage.h for a full description of the available
   transfer modes and constants that can be used as raster ops.
 
Prototype:
   int IWIDGET_SetROP(IWidget *po, uint32 rop)

Parameters:
===pre>
   po:   Pointer to the IWidget object.
   rop:  Identifies the raster operation to be applied when transferring the pixels of
         the image widget's bitmap to those in the destination graphics port.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the raster operation.
   EFAILED - If the raster operation could not be set.
===/pre>

Comments:
   None

Side Effects: 
   IWIDGET_SetROP() calls IWIDGET_SetImageParm() to access the IPARM_ROP image attribute.
   This has the net effect of processing an EVT_WDG_SETPROPERTY event through the widget
   hierarchy.

See Also:
   EVT_WDG_SETPROPERTY
   IWidget
   IWIDGET_SetImageParm()

==========================================================================

IWIDGET_SetScale()

Description:
   This function is used to set the scaled size of the image.  Once an
   image has been set using IWIDGET_SetImage(), this routine scales the
   image to the horizontal and vertical dimensions specified.

   This function is the only supported way to scale images in the image
   widget!  Pass the unscaled image to IWIDGET_SetImage() first, then
   scale it using this routine.  Failing to do so (using IIMAGE_SetParm(),
   for example) will break image alignment and cause other undesirable
   behavior.

   Image scaling is supported on BREW 3.0 and newer only.
 
Prototype:
   IWIDGET_SetScale(IWidget *po, uint32 dx, uint32 dy)

Parameters:
===pre>
   po:   Pointer to the IWidget object.
   dx:   Scaled width of the image
   dy:   Scaled height of the image
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully scale the image.
   EFAILED - If the image could not be scaled.
===/pre>

Comments:
   IWIDGET_SetScale() relies on BREW's bilinear scaling algorithm to
   perform the image scale and is only available when running on
   BREW 3.0 or greater.

Side Effects: 
   IWIDGET_SetScale() calls IWIDGET_SetImageParm() to access the 
   IPARM_SCALE image attribute. This has the net effect of processing 
   an EVT_WDG_SETPROPERTY event through the widget hierarchy.

See Also:
   EVT_WDG_SETPROPERTY
   IWidget
   IWIDGET_SetImageParm()

==========================================================================

IWIDGET_Animate()

Description:
   This function is used to start or stop an image widget's animation sequence, as
   specified by the 'bAnim' parameter.  If the image contains a sequence of frames,
   they will be displayed, one after another, in sequential order, with the current
   animation rate observed between frames.
   
Prototype:
   int IWIDGET_Animate(IWidget *po, boolean bAnim)

Parameters:
===pre>
   po:   Pointer to the IWidget object.
   rop:  Specifies whether or not the widget should start or stop its animation
         sequence.  Pass TRUE to start the animation, FALSE to stop.
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget was able to set the property, PROP_ANIMATE, to the
             value of 'bAnim'.
   EFAILED - If the request was not handled by the widget framework.
===/pre>

Comments:
   The animation sequence cannot be paused.  That is, each time IWIDGET_Animate() is
   called the frame sequence resets itself to the first frame in the animation
   sequence.

Side Effects: 
   IWIDGET_Animate() calls IWIDGET_SetProperty() to set the PROP_ANIMATE
   property.  This has the net effect of processing an EVT_WDG_SETPROPERTY event
   through the widget hierarchy.
   
   Calling IWIDGET_Animate() causes the image widget to be redrawn.

See Also:
   EVT_WDG_SETPROPERTY
   PROP_ANIMATE
   IWidget
   IWIDGET_SetProperty()

*/
#endif /* __AEEIMAGEWIDGET_H__ */
