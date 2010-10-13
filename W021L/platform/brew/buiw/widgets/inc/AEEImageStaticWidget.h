/*
  ========================================================================

  FILE:  AEEImageStaticWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Defines a commonly used combination image and
  static

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEIMAGESTATICWIDGET_H__
#define __AEEIMAGESTATICWIDGET_H__

#include "AEEWidget.h"
#include "AEEWProperties.h"
#include "AEEImageWidget.h"
#include "AEEStaticWidget.h"
#include "AEEWModel.h"

#include "bid/AEECLSID_IMAGESTATICWIDGET.bid"


// @@@ to set the PROP_FLAGS flags for the image and static widget, you must
// first obtain the respective widgets with PROP_IMAGESTATIC_IMAGEWIDGET and
// PROP_IMAGESTATIC_STATICWIDGET

// flags for PROP_FLAGS
#define ISWF_LAYOUT_IMAGERIGHT      0x01000000  // layout image to right side of widget
#define ISWF_NOIMAGE                0x02000000  // do not display image
#define ISWF_NOTEXT                 0x04000000  // do not display text

// IValueModel data representation for ImageStaticWidget
typedef struct {
   AECHAR *          pwText;     // widget text 
   IImage *          piImage;    // widget image
} ImageStaticInfo;


static __inline int IWIDGET_SetImageStaticImageWidget(IWidget *po, IWidget *piwImage) {
   return IWIDGET_SetProperty(po, PROP_IMAGESTATIC_IMAGEWIDGET, (uint32)piwImage);
}

static __inline int IWIDGET_SetImageStaticStaticWidget(IWidget *po, IWidget *piwStatic) {
   return IWIDGET_SetProperty(po, PROP_IMAGESTATIC_STATICWIDGET, (uint32)piwStatic);
}

static __inline int IWIDGET_GetImageStaticImageWidget(IWidget *po, IWidget **ppo) {
   return IWIDGET_GetProperty(po, PROP_IMAGESTATIC_IMAGEWIDGET, (uint32 *)ppo);
}

static __inline int IWIDGET_GetImageStaticStaticWidget(IWidget *po, IWidget **ppo) {
   return IWIDGET_GetProperty(po, PROP_IMAGESTATIC_STATICWIDGET, (uint32 *)ppo);
}

static __inline int IWIDGET_SetImageStaticInfo(IWidget *po, ImageStaticInfo *pInfo, PFNVALUEFREE pfnFree) {
   IValueModel *piModel;
   if (po && IWIDGET_GetModel(po, AEEIID_VALUEMODEL, (IModel **)&piModel) == 0) {
      IVALUEMODEL_SetValue(piModel, (void *)pInfo, sizeof(ImageStaticInfo), pfnFree);
      IVALUEMODEL_Release(piModel);
      return SUCCESS;
   }
   return EFAILED;
}

static __inline int IWIDGET_SetImageStaticHasSelectImage(IWidget *po) {
   IWidget* piwImage = NULL;
   int nErr = IWIDGET_GetImageStaticImageWidget(po, &piwImage);
   if (!nErr)
   {
      nErr = IWIDGET_AddFlags(piwImage, IWF_HASSELECTIMAGE);
      IWIDGET_Release(piwImage);
   }
   return nErr;
}

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Image Static Widget Class IDs

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates an
   image static widget by creating a new instance of the Image Static Widget class,
   passing the ID of the class to ISHELL_CreateInstance().  The object that is created
   is, itself, a widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the image static widget is defined below.

Definition:

Identifier                   Definition
----------                   -----------------------------------
AEECLSID_IMAGESTATICWIDGET:  Identifies the class ID of the image static widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

   
=============================================================================

ImageStaticInfo

Description:
   This data type defines the structure of the data an image static widget will expected to
   find in its attached model, to create and render both the static text and image components
   that together form an image static widget.
   
Definition:
   typedef struct {
      AECHAR  *pwText;
      IImage  *piImage;
   } ImageStaticInfo;

Members:
===pre>
   pwText:   Pointer to the AEChar that should be displayed as the text component of the
             image static widget.
   piImage:  Pointer to the IImage that should be displayed as the image component of the
             image static widget.
===/pre>
Comments:
   The image static widget expects the data stored in its value model to be stored as a
   pointer to an ImageStaticInfo data structure.

See Also:
   IImage

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Image Static Widget Class

Description:
   The image static widget is used to display a single interface element that consists
   of both an image and a text label.  For example, a real estate application could
   use the image static widget to display a photo of a home for sale with the price
   and address immediately to the right.
   
   All told, the static image widget is actually three objects in one:
===pre>
       1. A static text widget containing the text that is used to label the image.
       2. An image widget containing the graphic image to be displayed.
       3. A prop container into which the above elements will be placed.
===/pre>
   
   Applications specify the image to be displayed in the widget by calling
   IWIDGET_SetImageStaticInfo(), passing in a pointer to an ImageStaticInfo data
   structure, which contains the text and image to be displayed.  
   
   By default, the layout of the image has the image on the left and the label on the
   right, with the height of the label dictating the overall height of the parent
   container enclosing both objects.  This orientation can be changed, however, with
   the label on the left and the image on the right, by setting the appropriate bit
   in the widget's property flags (ISWF_LAYOUT_IMAGERIGHT bit of PROP_FLAGS).
   
   The label text will be vertically centered in the parent container, immediately to
   the right (or left, if the image/label orientation has been changed) of the image.
   By default, there is a two pixel padding between the image and the start of the label
   text -- a value that could be overridden by calling IWIDGET_SetLeftPadding() on the
   label portion of the image.  An application could change this or any other property
   of the label or image widgets to customize color, alignment or justification.
   
   The widget interface provides APIs to allow applications to access either the image
   or label parts of the image static widget, thereby allowing an application to have
   great control over how the widget is to act and be displayed.  For example, the
   application could retrieve the label portion of the object, then alter the font,
   color or alignment of the text.  Likewise, the image portion could be retrieved and
   manipulated -- for example, to add a transparency effect to the graphic.
      
   To create a static image widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_IMAGESTATICWIDGET to
          retrieve a reference counted instance of the static image widget.
       2. Call IWIDGET_SetImageStaticInfo() to identify the image and text to be displayed by
          the widget.
===/pre>

Supported Events:
   The image static widget processes events in three passes.  In the first pass, events
   are passed on to the widget's container, providing the container the opportunity to
   intercept events that affect the layout of any child objects.  If the event is not
   handled during the first pass, the image static widget will attempt to handle the
   any property events specific to the image static widget.  Finally, events not handled
   during either of the first two passes will be passed to the base widget event handler.
  
   The image static widget handles the following events in the described manner:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The image static widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The image static widget
                      allows the following properties to be set:
                      
                          PROP_SELECTED                 --  Sets the selected state of the image
                                                            static widget to either selected
                                                            (TRUE) or unselected (FALSE).
                          PROP_IMAGESTATIC_IMAGEWIDGET  --  Sets the image widget to be displayed
                                                            by the image static widget.
                          PROP_IMAGESTATIC_STATICWIDGET --  Sets the static text widget to be
                                                            displayed by the image static widget.
                          PROP_FLAGS                    --  Set various image static widget flags.
                      
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The image static widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  These properties are
                      discussed below in greater detail.
                      
                          PROP_IMAGESTATIC_IMAGEWIDGET  --  Retrieves a pointer to the image widget
                                                            to be displayed by the image static widget.
                          PROP_IMAGESTATIC_STATICWIDGET --  Retrieves a pointer to the static text
                                                            widget to be displayed by the image static
                                                            widget.
                      
                      These properties are discussed below in greater detail.

===/pre>
   
Properties:
Property                        Description
--------                        ------------------------------------------------------------
PROP_SELECTED:                  This property contains the selection state of the image static widget.
                                Set this property to TRUE to indicate that the image static widget is
                                in its selected state, FALSE to indicate that the widget is unselected.
                             
                                    Property Value:  boolean

PROP_IMAGESTATIC_IMAGEWIDGET:   This property contains a pointer to the widget that will be displayed
                                the graphic image contained within the image static widget.  

                                    Property Value:  IWidget *

PROP_IMAGESTATIC_STATICWIDGET:  This property contains a pointer to the widget that will be displayed
                                the static text contained within the image static widget. 

                                    Property Value:  IWidget *

PROP_FLAGS:                     This property contains a set of flag masks that dictate
                                the layout and display characteristics for the image static widget.
                                These flags are passed in the 'dwParam' of the event and include:
                              
                                  ISWF_LAYOUT_IMAGERIGHT
                                  ----------------------
                                  When this flag is set the widget will be laid out with its image
                                  displayed to the right of the text string (normally, the image is
                                  placed on the widget's left side, with the text laid out to the
                                  right).
                              
                                  ISWF_NOIMAGE
                                  ----------------------
                                  When this flag is set the image portion of the widget will not be
                                  displayed.  The image widget managed by the image static widget is
                                  still present, but it is, essentially, hidden from view.  When this
                                  flag is cleared, the image will again be displayed.
                              
                                  ISWF_NOTEXT
                                  ----------------------
                                  When this flag is set the static text portion of the widget will not
                                  be displayed.  The static text widget managed by the image static
                                  widget is still present, but it is, essentially, hidden from view.
                                  When this flag is cleared, the static text will again be displayed.
                              
                                    Property Value:  uint32

Required Model:
   IValueModel
   
Model Data:
===pre>   
   ImageStaticInfo *modelData;
===/pre>
   
   The image static widget relies on an value model that represents its data as a pointer
   to an ImageStaticInfo data structure.  The model stores only a pointer to the data
   structure, not a copy of the data itself, and should always return this pointer in
   response to data queries from a client widget.

   A default model is created and assigned to the widget when the widget is created with 
   ISHELL_CreateInstance(). The client can replace the widget's model at any time with 
   the method: IWIDGET_SetModel(), as long as the supplied model interface is of the type 
   that the widget expects.   
   
   The data supplied in the value model must be a pointer to type ImageStaticInfo.
   
Comments:
   None
   
See Also:
   Model Events
   ISHELL_CreateInstance()
   IWIDGET_SetImageStaticInfo()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_GetImageStaticImageWidget()

Description:
   This function retrieves the image widget portion of the image static widget, returning
   this object as a pointer to an IWidget.  Once retrieved, an application may use this
   pointer to manipulate the image -- for example, changing characteristics of the animation.

Prototype:
   int IWIDGET_GetImageStaticImageWidget(IWidget *po, IWidget **ppo)

Parameters:
===pre>
   po:   Pointer to the image static widget object.
   ppo:  Pointer to an IWidget pointer that will be returned by the function, providing
         access to the image widget portion of the parent widget.  This returned widget
         manages the image to be displayed by the image static widget.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully retrieve a pointer to the underlying image
             widget.
   EFAILED - If the image widget could not be retrieved.
===/pre>
   
   A pointer to the image widget is returned in 'ppo'.

Comments:
   None

Side Effects: 
   IWIDGET_GetImageStaticImageWidget() calls IWIDGET_GetProperty() and has the net effect
   of processing an EVT_WDG_GETPROPERTY event.

See Also:
   EVT_WDG_GETPROPERTY
   IWIDGET_GetProperty()

==========================================================================


IWIDGET_GetImageStaticStaticWidget()

Description:
   This function retrieves the static widget portion of the image static widget, returning
   this object as a pointer to an IWidget.  Once retrieved, an application may use this
   pointer to manipulate the text label associated with the widget.  For example, changing
   the color or transparency of the text.

Prototype:
   int IWIDGET_GetImageStaticStaticWidget(IWidget *po, IWidget **ppo)

Parameters:
===pre>
   po:   Pointer to the image static widget object.
   ppo:  Pointer to an IWidget pointer that will be returned by the function, providing
         access to the static widget portion of the parent widget.  This returned widget
         manages the text label to be displayed by the image static widget.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully retrieve a pointer to the underlying static
             text widget.
   EFAILED - If the static text widget could not be retrieved.
===/pre>
   
   A pointer to the static text widget (the label) is returned in 'ppo'.

Comments:
   None

Side Effects: 
   IWIDGET_GetImageStaticImageWidget() calls IWIDGET_GetProperty() and has the net effect
   of processing an EVT_WDG_GETPROPERTY event.

See Also:
   EVT_WDG_GETPROPERTY
   IWIDGET_GetProperty()

==========================================================================

IWIDGET_SetImageStaticFlags()

Description:
   This function set the flags that manage the layout attributes of an image static
   widget.  Normally, an image static widget is displayed with the image on the left
   and the label on the right.  However, this layout can be reversed by changing the
   widget's layout flags.  The image static widget recognizes a small set of layout
   flags, as follows:
===pre>
   IDF_ALIGN_LEFT  -  Layout the widget with the image on the left and the label on
                      the right.
   IDF_ALIGN_RIGHT -  Layout the widget with the label on the left and the image on
                      the right.
===/pre>

Prototype:
   int IWIDGET_SetImageStaticFlags(IWidget *po, uint32 dwFlags)

Parameters:
===pre>
   po:       Pointer to the image static widget object.
   dwFlags:  A 32-bit integer indicating how the image and label should be displayed.
             The image static widget recognizes only a pair of flags, IDF_ALIGN_LEFT
             and IDF_ALIGN_RIGHT, each of which are used to control the left/right
             relationship between the image and label.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully set the layout of the image and label
             widgets as desired.
   EFAILED - If the requested layout could not be set, or if flags other than
             IDF_ALIGN_LEFT or IDF_ALIGN_RIGHT were specified in the call.
===/pre>

Comments:
   None

Side Effects: 
   IWIDGET_SetImageStaticFlags() calls IWIDGET_SetProperty() and has the net effect
   of processing an EVT_WDG_SETPROPERTY event.
   
   Calling IWIDGET_SetImageStaticFlags() will cause the image static widget to be redrawn
   with the specified layout flags.

See Also:
   EVT_WDG_SETPROPERTY
   IWIDGET_SetProperty()

==========================================================================

IWIDGET_SetImageStaticInfo()

Description:
   This function stores a pointer to an ImageStaticInfo data structure in the value model
   associated with an image static widget.

Prototype:
   int IWIDGET_SetImageStaticInfo(IWidget *po, ImageStaticInfo *pInfo, PFNVALUEFREE pfnFree)

Parameters:
===pre>
   po:       Pointer to the image static widget object.
   pInfo:    Pointer to the ImageStaticInfo data structure that contains pointers to the
             image and text that is to be displayed by the widget.  This pointer will be
             stored in the widget's attached value model.
   pfnFree:  Pointer to the callback function that BREW will call to free existing data stored
             in the value model.  If no special function is required to free the model data,
             pass NULL.
===/pre>

Return Value:
===pre>
   SUCCESS - If BREW was able to successfully retrieve the value model and store the pointer
             to the ImageStaticInfo data structure that houses the image and text pointers
             widgets as desired.
   EFAILED - If the value model could not be retrieved or BREW could otherwise not honor
             the request.
===/pre>

Comments:
   None

Side Effects: 
   None

See Also:
   ImageStaticInfo
   PFNVALUEFREE

*/

#endif /* __AEEIMAGESTATICWIDGET_H__ */
