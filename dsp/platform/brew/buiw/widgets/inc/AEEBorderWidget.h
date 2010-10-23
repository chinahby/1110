/*
  ========================================================================

  FILE:  AEEBorderWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: A general border decorator. I understand a number
  of background and border properties as well as a dropshadow 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEBORDERWIDGET_H__
#define __AEEBORDERWIDGET_H__

#include "bid\AEECLSID_BORDERWIDGET.bid"

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Border Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   border widget by creating a new instance of the Border Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the border widget is defined below.

Definition:

Identifier              Definition
----------              -----------------------------------
AEECLSID_BORDERWIDGET:  Identifies the class ID of the border widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Border Widget Class

Description:
   
   The border widget is derived from the decorator object and is used to draw borders 
   and backgrounds for widgets.  For example, a border widget may simply draw the 
   background content of a widget as an opaque white rectangle, or could draw a drop 
   shadow effect or a transparent frame around a widget's content rectangle -- or, it 
   could do both, drawing the background content and a frame.  

   The border widget is described by several visible components - a border, a background 
   fill color, a background image and a shadow.  

   Border and background colors (including gradients) are specified by type RGBAVAL, 
   which contains values for the red, green and blue color channels, as well as an alpha 
   channel value.  The alpha channel value specifies the 'opacity' of the combined color 
   channels in the range of 0-255, with 0 meaning fully transparent (no color visible), 
   and 255 meaning fully opaque.  This is a departure from the traditional method of 
   specifying colors in BREW, which did not use the alpha channel value.  Because the 
   alpha channel is now significant in widgets, it is important to use the macro 
   MAKE_RGBA() when constructing RGBA values for border and background colors instead of 
   using MAKE_RGB().  Similarly, use RGBA_NONE, RGBA_BLACK and RGBA_WHITE instead of 
   RGB_NONE, RGB_BLACK and RGB_WHITE when using predefined colors.  
   
   Because opacity is specified by the alpha channel value present in the RGBAVAL used 
   for colors, the properties used to specify transparency such as 
   PROP_BORDERTRANPARENCY, PROP_BGTRANSPARENCY, and PROP_SHADOWTRANSPARENCY are no 
   longer recommended, although they are still supported for backward compatibility.  If 
   used, they will override the alpha channel values for color properties that have been 
   previously set, but will not override color properties that are set afterwards.
   
   Note that the alpha channel value (opacity) is the inverse of the transparency value 
   set by the transparency properties, as shown by this formula: 
      
      AlphaValue = (255 - TransparencyValue).
   
   
===pre>
      Border
      ------
      The portion of the widget that frames the widget's content rectangle.  For example,
      the border of a list widget would be the frame that outlines the list contents.  A
      widget's border may be defined by color, transparency, width and style -- each of which
      may be controlled by setting various widget properties.
     
      Color          The color of a border widget's frame depends on whether or not the widget is
                     selected, unselected, active or inactive.  For example, a border widget could
                     display itself in light gray when the widget is inactive, or bright green when
                     it is selected and active.  Border colors are established by setting a given
                     widget property, specifying the color the widget should use when drawing its
                     border under the current selection and activation states of the widget.  The
                     default color, when no widget border color properties have been set, is to
                     display the widget's border using RGBA_BLACK.
                     
                     The widget properties that determine border color include:
                     
                         PROP_BORDERCOLOR
                         PROP_SELECTED_BORDERCOLOR
                         PROP_ACTIVE_BORDERCOLOR
                         PROP_INACTIVE_BORDERCOLOR
                         PROP_SACTIVE_BORDERCOLOR
                         PROP_SINACTIVE_BORDERCOLOR

                     Note that various properties override others, and the order that a border widget's
                     color properties is set is very important.  For example, if an object goes to the
                     trouble of setting the PROP_SACTIVE_BORDERCOLOR and PROP_SINACTIVE_BORDERCOLOR
                     properties, but then sets the PROP_BORDERCOLOR, the PROP_BORDERCOLOR color will
                     override the previously established colors because PROP_BORDERCOLOR sets the border
                     color for ALL state combinations of selection and activation.

      Transparency   The transparency of a border widget is controlled by the 
                     PROP_BORDERTRANSPARENCY widget property and determines the degree of 
                     transparency for the border.  A value of 0 would indicate that the widget 
                     border should be drawn as fully opaque, with greater values up to 255 making 
                     the border increasingly transparent.  
                     
                     Note: This property is deprecated because the border transparency is now 
                     specified by the inverse of the alpha channel component in the RGBAVAL used for 
                     the PROP_xxx_BORDERCOLOR properties.  
                     
      Width          The thickness of a border widget's frame is determined by the widget's current
                     activation state.  For example, the frame of a list box might be two pixels wide
                     when the control is active; one pixel wide when it is inactive.  By varying the
                     thickness of the border, an active widget is immediately distinguished from those
                     that are inactive.  Several widget properties are used to determine border width,
                     with the defaults being a 2 pixel border when the widget is in its active state
                     and 1 pixel when in its inactive state.  Note that the width of the active state 
                     should be greater than or equal to the inactive state because the preferred extent
                     is calculated using the active state's width.  
                     
                     The widget properties that determine border width include:

                         PROP_BORDERWIDTH
                         PROP_ACTIVE_BORDERWIDTH
                         PROP_INACTIVE_BORDERWIDTH

                     Note that various properties override others, and the order that a border widget's
                     width properties is set is very important.  For example, if an object sets the active
                     width with PROP_ACTIVE_BORDERWIDTH, a subsequent setting of PROP_BORDERWIDTH would
                     change the previous value stored for the widget's active state.

      Radius         The radius of the rounded corners of a BORDERSTYLE_ROUNDED style border is 
                     controlled by the PROP_BORDERRADIUS widget property.  This value is constrained
                     to between 0 and half of the smaller of the border's height and width.  The
                     value of this property determines the look of the rounded border and specifies
                     just how round the corners will be.  Depending on your application, values from 
                     3 to 10 are likely adequate.
                     
      Style          Border widgets may be displayed in a variety of styles, as indicated by the widget's
                     PROP_BORDERSTYLE property.  Currently, the border widget supports these styles:
                     
                     BORDERSTYLE_NORMAL   - The widget border will be displayed as a standard rectangle
                     BORDERSTYLE_BEVELED  - The widget border will be displayed with beveled corners, inset
                                            from the widget's bounding rectangle.
                     BORDERSTYLE_ROUNDED  - The widget border will be displayed with rounded corners, inset
                                            from the widget's bounding rectangle.  The radius of the 
                                            corners can be set with IWIDGET_SetBorderRadius().

      Background
      ----------
      The portion of the widget that serves as the background for the widget.  The background is the canvas
      upon which other elements will be drawn.  For example, the text of a checkbox or the icons in a grid
      list would be drawn by a widget on top of the border widget's background.  The background of a border
      widget may possess attributes of color and transparency -- each of which may be controlled by setting
      various widget properties.

      Color          The background color of a border widget depends on whether or not the widget is
                     selected, unselected, active or inactive.  For example, the background may be
                     bright yellow when the widget is active, but a muted yellow when the widget is
                     inactive.  Background colors are established by setting a given widget property,
                     specifying the color the widget should use when filling in its background --
                     depending, of course, on the current selection and activation states of the widget. 
                     The default background color when no widget background color properties have been set,
                     is to display the widget's background using RGBA_NONE.
                     
                     The widget properties that determine background color include:
                     
                         PROP_BGCOLOR
                         PROP_SELECTED_BGCOLOR
                         PROP_ACTIVE_BGCOLOR
                         PROP_INACTIVE_BGCOLOR
                         PROP_SACTIVE_BGCOLOR
                         PROP_SINACTIVE_BGCOLOR

                     Note that various properties override others, and the order that a border widget's
                     color properties is set is very important.  For example, if an object has set the
                     active background color by way of PROP_ACTIVE_BGCOLOR, then subsequently sets the
                     PROP_BGCOLOR, the previously set active color will be overridden by the new value.

      Gradient       In addition to background colors, border widgets support gradients, 
                     where an initial color gradually changes into another color.  The gradient 
                     color used for drawing depends on whether the widget is selected, 
                     unselected, and active or inactive.  A widgets background and gradient color properties 
                     are used together, with the background color being the start color, and the background 
                     gradient color as the end color.

                     The property PROP_GRADIENT_STYLE determines which gradient is used.  
                     Possible gradient types which can be specified include:

                        GRADIENT_STYLE_VERT (vertical, top to bottom)                            
                        GRADIENT_STYLE_HORZ (horizontal, left to right)
                        GRADIENT_STYLE_CENTERVERT (vertical, center to outer)
                        GRADIENT_STYLE_CENTERHORZ (horizontal, center to outer
                        GRADIENT_STYLE_NONE (no gradient - default)

                     If the default gradient style is used (GRADIENT_STYLE_NONE), the current widget background color will be used for the entire
                     background area of the widget, and no gradient will be used.
                     
                     If the gradient style is set to a value other than GRADIENT_STYLE_NONE, two sets of color properties are
                     used to determine the gradient; various BGCOLOR values (see Color section above) and GRADIENT values, which include:

                        PROP_GRADIENT           
                        PROP_SELECTED_GRADIENT  
                        PROP_ACTIVE_GRADIENT    
                        PROP_INACTIVE_GRADIENT  
                        PROP_SACTIVE_GRADIENT   
                        PROP_SINACTIVE_GRADIENT 
                                          
                     For example, if PROP_GRADIENT_STYLE is set to GRADIENT_STYLE_HORZ, and then 
                     PROP_ACTIVE_BGCOLOR and PROP_ACTIVE_GRADIENT are set for the same widget, with:

                        PROP_ACTIVE_BGCOLOR = MAKE_RGBA(0,0,255,255);  // Blue, fully opaque

                     and:

                        PROP_ACTIVE_GRADIENT = MAKE_RGBA(0,0,255,0);    // Blue, fully transparent

                     the resulting gradient will be an opaque blue background gradually fading to 
                     fully transparent. Note the use of the MAKE_RGBA macro, which can take a 4th parameter
                     to specify the alpha channel, or opacity value.  Like the other three parameters, the 4th has the range
                     of a single byte, with 0 being completely transparent and 255 being completely opaque.

      Transparency   The background of a widget can be made either opaque or semitransparent by setting
                     the widget's PROP_BGTRANSPARENCY property.  A value of 0 would indicate that the
                     widget will have a fully opaque background.  As values increase -- up to 255 -- the
                     background of the widget becomes increasingly transparent.  This property has 
                     no effect on the transparency characteristics for gradient backgrounds.

                     Note: This property is deprecated because the background transparency is now 
                     specified by the inverse of the alpha channel component in the RGBAVAL used for 
                     the PROP_xxx_BGCOLOR properties.


      Image          The background of a widget may contain an image. 
                     
                     By default, the background image is drawn on top of the background fill color, unless 
                     the BGIF_IMAGEUNDER flag is specified with the PROP_BGIMAGE_FLAGS property, in which case the 
                     background fill color will be drawn over the image.  

                     If the image is smaller than the area of the background, the image may be tiled to 
                     fill all of the available space.  Tiling is enabled with the BGIF_TILED flag of the
                     PROP_BGIMAGE_FLAGS property.  See the section below titled 'Background Image Tiling'
                     for more information on image tiling.

                     
      Shadow
      ------
      A shadow is a visual enhancement to a widget that creates the illusion that the widget lays above
      other objects on the display.  For example, a list or a button may possess a drop shadow to give
      the user interface a 3D appearance.   The shadow of a border widget may possess attributes of color,
      transparency and position -- each of which may be controlled by setting various widget properties.

      Color          The color of a border widget's shadow is always one uniform color.  Unlike borders
                     and backgrounds, the shadow color may not vary depending on the widget's state.
                     The color of a shadow is determined by the PROP_SHADOWCOLOR property, and defaults
                     to RGBA_BLACK when PROP_SHADOWCOLOR has not been explicitly set. 

      Transparency   The shadow of a widget can be made either opaque or semitransparent by setting
                     the widget's PROP_SHADOWTRANSPARENCY property.  A value of 0 would indicate that the
                     widget will have a fully opaque shadow.  As values increase -- up to 255 -- the
                     background of the widget becomes increasingly transparent and <gasp!> shadowy.

                     Note: This property is deprecated because the shadow transparency is now 
                     specified by the inverse of the alpha channel component in the RGBAVAL used for 
                     the PROP_SHADOWCOLOR properties.
                     
      Position       The placement of a widget's shadow is determined by an [x,y] offset from the widget's
                     origin, effectively shifting a rectangle of the widget's exact dimensions immediately
                     beneath the widget itself.  To illustrate, a shadow specified with an offset of [1,1]
                     would place the shadow one pixel to the right and below the widget itself, thereby
                     creating the effect of a drop shadow with the light source in the upper left-hand
                     corner of the display.  Similarly, an offset of [1,-1] would place the shadow one pixel
                     to the right and one pixel above the widget itself -- the light source effectively
                     moving to the lower left of the image.  The location of the widget shadow is controlled
                     by several widget properties, as follows:
===/pre>

   To create a border widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_BORDERWIDGET to
          retrieve a reference counted instance of the border widget.
       2. Call the appropriate IWIDGET_Setxxxxxx() functions to initialize any special visual
          characteristics of the border widget such as border color, width, or transparency.
          For example, if the newly created a border widget is to be 4 pixels thick, the
          application would call IWIDGET_SetBorderWidth(), passing 4 in the 'nBorderWidth'
          parameter. 
===/pre>

Supported Events:
   The border widget processes events in two passes.  In the first pass the border widget
   will attempt to process any event specifically related to the border itself.  These events
   include those that will effect the border, background or shadow, as described above.  If
   the event is handled during this first pass, the border widget will invalidate itself and
   be redrawn, using any updated property values.  The second pass passes unhandled events to
   the parent decorator.
  
   The border widget handles the following events in the described manner:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETFOCUS:     The border widget responds to this event by attempting to set the
                      focus of the widget, as specified by the 'wParam' parameter.  When
                      'wParam' is TRUE, the widget is to be set to the active state,
                      otherwise the widget is inactive.
                      
                      The border widget will return FALSE after handling EVT_WDG_SETFOCUS,
                      thereby allowing other objects to also handle the event.

EVT_WDG_SETPROPERTY:  The border widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The border widget
                      recognizes the properties defined in the "Properties" section, below.

EVT_WDG_GETPROPERTY:  The border widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  The border widget
                      recognizes the properties defined in the "Properties" section, below.

===/pre>   

Properties:


Property                     Description
--------                     ------------------------------------------------------------
PROP_APPLYWPROPS:            This property will lookup and apply the provided named set of widget
                             properties, using the provided IRESFILE interface.

                             This property is best used through the IWIDGET_SetProperties() macro,
                             which constructs the variable argument list and WResProDesc structure.

                             Property Value: WResPropDesc pointer

PROP_BORDERCOLOR:            This property contains the color the widget will use to draw its
                             border.  Setting PROP_BORDERCOLOR will set the border color for all
                             combinations of widget selection and activation, as indicated by the
                             following table:
                             
                                                                      Border color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         PROP_BORDERCOLOR
                               Unselected         Active           PROP_BORDERCOLOR
                               Selected           Inactive         PROP_BORDERCOLOR
                               Selected           Active           PROP_BORDERCOLOR
                             
                             Though setting the PROP_BORDERCOLOR will effect _all_ combinations
                             of selection and activation, getting the value of the property will
                             return the border color that will be applied when drawing the widget
                             in its active, unselected state.
                             
                             Property Value:  RGBAVAL

PROP_SELECTED_BORDERCOLOR:   This property contains the color the widget should use to draw its
                             border when the widget is selected, regardless of whether or not the
                             widget is active or inactive, as indicated by the following table:
                          
                                                                      Border color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         PROP_SELECTED_BORDERCOLOR
                               Selected           Active           PROP_SELECTED_BORDERCOLOR
                             
                             Though setting the PROP_SELECTED_BORDERCOLOR will effect both the
                             active and inactive states of a selected widget, getting the value
                             of the property will return the border color that will be applied
                             when drawing the widget in its selected, active state.
                             
                             Property Value:  RGBAVAL

PROP_ACTIVE_BORDERCOLOR:     This property contains the color the widget should use to draw its
                             border when the widget is active, as indicated by the following table:
                          
                                                                      Border color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           PROP_ACTIVE_BORDERCOLOR
                               Selected           Inactive         Unchanged
                               Selected           Active           Unchanged*
                             
                             * This property will be 'inherited' by the Active/Selected state if
                             the PROP_SACTIVE_BORDERCOLOR property is never set. 
                             
                             Property Value:  RGBAVAL

PROP_INACTIVE_BORDERCOLOR:   This property contains the color the widget should use to draw its
                             border when the widget is inactive, regardless of whether or not the
                             widget is selected or unselected, as indicated by the following table:
                          
                                                                      Border color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         PROP_INACTIVE_BORDERCOLOR
                               Unselected         Active           Unchanged
                               Selected           Inactive         Unchanged*
                               Selected           Active           Unchanged
                             
                             * This property will be 'inherited' by the Inactive/Selected state if
                             the PROP_SINACTIVE_BORDERCOLOR property is never set. 
                             
                             Property Value:  RGBAVAL

PROP_SACTIVE_BORDERCOLOR:    This property contains the color the widget should use to draw its
                             border when the widget is selected and active -- but neither
                             unselected or inactive, as indicated by the following table:
                          
                                                                      Border color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         Unchanged
                               Selected           Active           PROP_SACTIVE_BORDERCOLOR
                             
                             Getting the value of the PROP_SACTIVE_BORDERCOLOR property will
                             return the border color that will be applied when drawing the widget
                             in its active, selected state.
                             
                             Property Value:  RGBAVAL
                             
PROP_SINACTIVE_BORDERCOLOR:  This property contains the color the widget should  use to draw its
                             border when the widget is selected and inactive -- but neither
                             unselected or active, as indicated by the following table:
                          
                                                                      Border color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         PROP_SINACTIVE_BORDERCOLOR
                               Selected           Active           Unchanged
                             
                             Getting the value of the PROP_SINACTIVE_BORDERCOLOR property will
                             return the border color that will be applied when drawing the widget
                             in its inactive, selected state.
                             
                             Property Value:  RGBAVAL
                             
PROP_BORDERTRANSPARENCY:     This property contains a value -- ranging from 0 to 255 -- that
                             specifies the degree of transparency to be applied to the border
                             when it is drawn on the display.  A value of 0 would indicate that
                             the widget border should be drawn as fully opaque, with greater
                             values up to 255 making the border increasingly transparent.
                             
                             Property Value:  uint8  (0 to 255)
                             
                             This property overrides the transparency values of colors already set 
                             by PROP_xxx_BORDERCOLOR properties.  
                             
PROP_BORDERWIDTH:            This property contains the width, in pixels, of the widget's border,
                             regardless of whether or not the widget is active or inactive, as
                             indicated by the following table:

                               Active State     Border width
                               ------------     ------------
                               Inactive         PROP_BORDERWIDTH
                               Active           PROP_BORDERWIDTH
                             
                             Getting the value of the PROP_BORDERWIDTH property will return the
                             border width that will be applied when drawing the widget in its
                             active state.  By default, the active and inactive border widths are 
                             2 and 1, respectively.  Note that the active border width should be
                             greater than or equal to the inactive border width.  Otherwise, the
                             preferred extent of the widget might be calculated incorrectly when
                             the border is inactive.  
                             
                             Property Value:  uint8
                             
PROP_ACTIVE_BORDERWIDTH:     This property contains the width, in pixels, of the widget's border
                             when the widget is active, as indicated by the following table:

                               Active State     Border width
                               ------------     ------------
                               Inactive         unchanged
                               Active           PROP_ACTIVE_BORDERWIDTH
                             
                             Getting the value of the PROP_ACTIVE_BORDERWIDTH property will return
                             the border width that will be applied when drawing the widget in its
                             active state.  The active border width should be greater than or equal
                             to the inactive border width.  
                             
                             Property Value:  uint8
                             
PROP_INACTIVE_BORDERWIDTH:   This property contains the width, in pixels, of the widget's border
                             when the widget is inactive, as indicated by the following table:

                               Active State     Border width
                               ------------     ------------
                               Inactive         PROP_INACTIVE_BORDERWIDTH
                               Active           unchanged
                             
                             Getting the value of the PROP_INACTIVE_BORDERWIDTH property will return
                             the border width that will be applied when drawing the widget in its
                             inactive state.  The inactive border width should be lesser than or equal
                             to the active border width.  
                             
                             Property Value:  uint8


PROP_BORDERRADIUS:           This property contains the radius, in pixels, of the rounded corners of 
                             a border when its style is set to BORDERSTYPE_ROUNDED.  The larger the 
                             radius, the more rounded the corners of the border will be.

                             Property Value:  uint8

                             
PROP_BORDERSTYLE:            This property specifies the style that the widget should apply when
                             drawing its border.  Currently, the border widget supports these
                             styles:
                     
                               BORDERSTYLE_NORMAL  - The widget border will be displayed as a
                                                     standard rectangle
                               BORDERSTYLE_BEVELED - The widget border will be displayed with
                                                      beveled corners, inset from the widget's
                                                      bounding rectangle.
                               BORDERSTYLE_ROUNDED - The widget border will be displayed with 
                                                      rounded corners, inset from the widget's 
                                                      bounding rectangle.  

PROP_BGCOLOR:                This property contains the color the widget will use to draw its
                             background.  Setting PROP_BGCOLOR will set the background color for
                             all combinations of widget selection and activation, as indicated by
                             the following table:
                             
                                                                   Background color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         PROP_BGCOLOR
                               Unselected         Active           PROP_BGCOLOR
                               Selected           Inactive         PROP_BGCOLOR
                               Selected           Active           PROP_BGCOLOR
                             
                             Though setting the PROP_BGCOLOR will effect _all_ combinations of
                             selection and activation, getting the value of the property will
                             return the background color that will be applied when drawing the
                             widget in its active, unselected state.
                             
                             Property Value:  RGBAVAL

PROP_SELECTED_BGCOLOR:       This property contains the color the widget should use to draw its
                             background when the widget is selected, regardless of whether or not
                             the widget is active or inactive, as indicated by the following table:
                          
                                                                   Background color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         PROP_SELECTED_BGCOLOR
                               Selected           Active           PROP_SELECTED_BGCOLOR
                             
                             Though setting the PROP_SELECTED_BGCOLOR will effect both the active
                             and inactive states of a selected widget, getting the value of the
                             property will return the background color that will be applied when
                             drawing the widget in its selected, active state.
                             
                             Property Value:  RGBAVAL

PROP_ACTIVE_BGCOLOR:         This property contains the color the widget should use to draw its
                             background when the widget is active, as indicated by the following 
                             table:
                                                                   Background color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           PROP_ACTIVE_BGCOLOR
                               Selected           Inactive         Unchanged
                               Selected           Active           Unchanged*
                             
                             * This property will be 'inherited' by the Active/Selected state if
                             the PROP_SACTIVE_BGCOLOR property is never set. 
                             
                             Property Value:  RGBAVAL

PROP_INACTIVE_BGCOLOR:       This property contains the color the widget should use to draw its
                             background when the widget is inactive, as indicated by the following
                             table:
                          
                                                                   Background color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         PROP_INACTIVE_BGCOLOR
                               Unselected         Active           Unchanged
                               Selected           Inactive         Unchanged*
                               Selected           Active           Unchanged
                             
                             * This property will be 'inherited' by the Inactive/Selected state if
                             the PROP_SINACTIVE_BGCOLOR property is never set. 
                             
                             Property Value:  RGBAVAL

PROP_SACTIVE_BGCOLOR:        This property contains the color the widget should use to draw its
                             background when the widget is selected and active -- but neither
                             unselected or inactive, as indicated by the following table:
                          
                                                                   Background color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         Unchanged
                               Selected           Active           PROP_SACTIVE_BGCOLOR
                             
                             Getting the value of the PROP_SACTIVE_BGCOLOR property will return
                             the background color that will be applied when drawing the widget
                             in its active, selected state.
                             
                             Property Value:  RGBAVAL
                             
PROP_SINACTIVE_BGCOLOR:      This property contains the color the widget should  use to draw its
                             background when the widget is selected and inactive -- but neither
                             unselected or active, as indicated by the following table:
                          
                                                                   Background color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         PROP_SINACTIVE_BGCOLOR
                               Selected           Active           Unchanged
                             
                             Getting the value of the PROP_SINACTIVE_BGCOLOR property will return
                             the background color that will be applied when drawing the widget
                             in its inactive, selected state.
                             
                             Property Value:  RGBAVAL

PROP_BGTRANSPARENCY:         This property contains a value -- ranging from 0 to 255 -- that
                             specifies the degree of transparency to be applied to the background
                             when it is drawn on the display.  A value of 0 would indicate that
                             the widget's background should be drawn as fully opaque, with greater
                             values up to 255 making the background increasingly transparent.
                             
                             Property Value:  uint8  (0 to 255)

                             This property overrides the transparency values of colors already set 
                             by PROP_xxx_BGCOLOR properties.  

PROP_GRADIENT:               This property sets the end RGBA color values used for a 
                             gradient fill background in all 4 widget states, as indicated by the
                             following table:
                                                                   Background gradient when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         PROP_GRADIENT
                               Unselected         Active           PROP_GRADIENT
                               Selected           Inactive         PROP_GRADIENT
                               Selected           Active           PROP_GRADIENT
                                                      
                             The 'dwParam' parameter is an RGBAVAL 
                             for the ending color of the gradient.  

                             Property Value:  RGBAVAL

PROP_SELECTED_GRADIENT:      This property sets the end RGBA color values used for a 
                             gradient fill background in both selected widget states, as indicated by the
                             following table:
                                                                   Background gradient when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         PROP_SELECTED_GRADIENT
                               Selected           Active           PROP_SELECTED_GRADIENT
                                                      
                             The 'dwParam' parameter is an RGBAVAL 
                             for the ending color of the gradient.  

                             Property Value:  RGBAVAL

PROP_INACTIVE_GRADIENT:      This property sets the end RGBA color values used for a       
                             gradient fill background when the widget is inactive and unselected, as 
                             indicated by the following table:                                                        
                                                                   Background gradient when             
                               Selection State    Active State     the widget is displayed           
                               ---------------    ------------     -----------------------           
                               Unselected         Inactive         PROP_INACTIVE_GRADIENT
                               Unselected         Active           Unchanged*
                               Selected           Inactive         Unchanged*                         
                               Selected           Active           Unchanged*
                                                                                                     
                             * This property will be 'inherited' if it was not previously set.                        
                                                                                                     
                             The 'dwParam' parameter is an RGBAVAL 
                             for the ending color of the gradient.  

                             Property Value:  RGBAVAL
  
PROP_ACTIVE_GRADIENT:        This property sets the end RGBA color values used for a 
                             gradient fill background when the widget is active and unselected, as 
                             indicated by the following table:
                                                                   Background gradient when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           PROP_ACTIVE_GRADIENT
                               Selected           Inactive         Unchanged*
                               Selected           Active           Unchanged*
                             
                             * This property will be 'inherited' if it was not previously set.                             
                             
                             The 'dwParam' parameter is an RGBAVAL 
                             for the ending color of the gradient.  

                             Property Value:  RGBAVAL
                             
PROP_SINACTIVE_GRADIENT:     This property sets the end RGBA color values used for a     
                             gradient fill background when the widget is inactive and selected, as 
                             indicated by the following table:                                                  
                                                                   Background gradient when           
                               Selection State    Active State     the widget is displayed         
                               ---------------    ------------     -----------------------         
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         PROP_SINACTIVE_GRADIENT
                               Selected           Active           Unchanged*                       
                                                                                                   
                             * This property will be 'inherited' if it was not previously set.
  
                             The 'dwParam' parameter is an RGBAVAL 
                             for the ending color of the gradient.  

                             Property Value:  RGBAVAL

PROP_SACTIVE_GRADIENT:       This property sets the end RGBA color values used for a     
                             gradient fill background when the widget is active and selected, as 
                             indicated by the following table:
                                                                   Background gradient when
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         Unchanged
                               Unselected         Active           Unchanged
                               Selected           Inactive         Unchanged
                               Selected           Active           PROP_SACTIVE_GRADIENT
                                                                                                   
                             The 'dwParam' parameter is an RGBAVAL 
                             for the ending color of the gradient.  

                             Property Value:  RGBAVAL
                             
PROP_GRADIENT_STYLE:         This property gets/sets the gradient fill style. The fill 
                             style value is passed in the 'dwParam' parameter and may
                             be one of the following values:

                             GRADIENT_STYLE_VERT
                             -------------------
                             This fill style will draw a smoothly changing gradient 
                             starting from the top of the widget's rectangle (start color) 
                             and ending at the bottom of the rectangle (end color). 
                              
                             Note:
                             In the figures below, a '1' character represents  the 
                             start color, and the highest valued  integer character 
                             ('9' or '5') represents the end color. The character 
                             values in the middle range represent a proportional blend 
                             of the two endpoint colors.
                              
                             +----------+
                             |1111111111|<---start color (1)
                             |2222222222|
                             |3333333333|
                             |4444444444|
                             |5555555555|
                             |6666666666|
                             |7777777777|
                             |8888888888|
                             |9999999999|<---end color (9)
                             +----------+
                              
                              
                             GRADIENT_STYLE_HORZ
                             -------------------
                             This fill style will draw a smoothly changing gradient 
                             starting from the left side of the widget's rectangle 
                             (start color) and ending at the right side of the rectangle (end color). 

                              +------------start color (1)
                              |
                              |       +----end color (9)
                              |       |
                              V       V
                             +---------+
                             |123456789|
                             |123456789|
                             |123456789|
                             |123456789|
                             |123456789|
                             |123456789|
                             |123456789|
                             |123456789|
                             +---------+
                              
                              
                             GRADIENT_STYLE_CENTERVERT
                             -------------------------
                             This fill style value will draw a smoothly changing gradient
                             starting from the top of the widget's rectangle (start color) 
                             and ending at the middle of the rectangle (end color), then
                             draw another smoothly changing gradient pattern from the center
                             (end color) to the bottom of the rectangle (start color).
                              
                             +----------+
                             |1111111111|<---start color (1)
                             |2222222222|
                             |3333333333|
                             |4444444444|
                             |5555555555|<---end color (5)
                             |4444444444|
                             |3333333333|
                             |2222222222|
                             |1111111111|<---start color (1)
                             +----------+

                              
                             GRADIENT_STYLE_CENTERHORZ
                             -------------------------
                             This fill style value will draw a smoothly changing gradient
                             starting from the left side of the widget's rectangle 
                             (start color) and ending at the center of the rectangle (end color),
                             then will draw another smoothly changing gradient pattern from 
                             the center (end color) to the right side of the rectangle 
                             (start color).
                              
                              +------------start color (1)
                              |
                              |   +--------end color (5)
                              |   |   
                              |   |   +----start color (1)
                              |   |   |
                              V   V   V
                             +---------+
                             |123454321|
                             |123454321|
                             |123454321|
                             |123454321|
                             |123454321|
                             |123454321|
                             |123454321|
                             |123454321|
                             +---------+

PROP_SHADOWCOLOR:            This property contains the color the widget should use to draw its
                             shadow, and also specifies it's degree of opacity in the 
                             alpha-channel component.
                             
                             Property Value:  RGBAVAL

PROP_SHADOWTRANSPARENCY:     This property contains a value -- ranging from 0 to 255 -- that
                             specifies the degree of transparency to be applied to a widget's
                             shadow when it is drawn on the display.  A value of 0 would indicate
                             that the shadow should be drawn as fully opaque, with greater values
                             up to 255 making the background increasingly transparent.

                             This property overrides the transparency values of the color set by 
                             PROP_SHADOWCOLOR.
                             
                             Property Value:  uint8  (0 to 255)

PROP_SHADOWOFFSET:           This property contains the offset of the widget's shadow, assuming that
                             both the 'x' and 'y' are equal, creating the effect of a light source
                             coming from either the upper left of the widget (for example, [2,2] which
                             would create a two pixel shadow) or the lower right of the widget (for
                             example, [-4,-4] which would create a 4 pixel shadow).
                             
                             Property Value:  uint8

PROP_SHADOWOFFSETX:          This property contains the 'x' offset of the widget's shadow.  A positive
                             'x' value has the effect of placing the shadow to the right of the
                             widget's bounding rectangle, while a negative value places the shadow to
                             the left.
                             
                             Property Value:  uint8

PROP_SHADOWOFFSETY:          This property contains the 'y' offset of the widget's shadow.  A positive
                             'y' value has the effect of placing the shadow below the widget's bounding
                             rectangle, while a negative value places the shadow above.
                             
                             Property Value:  uint8



PROP_BGIMAGE:                This property contains the background image of the widget. The
                             current background image may be set or retrieved with this property.
                             Passing NULL in a set operation removes the background image. The 
                             background image is drawn over the background fill color by default, 
                             or may be drawn under a partially transparent background color if 
                             the BGIF_IMAGEUNDER flags is set. 

                             Property Value: IImage*


PROP_BGIMAGE_FLAGS:          This property contains a set of flag masks that dictate how the
                             background image will behave.  These flags are passed in the 'dwParam' 
                             of the event. The following flags are defined for the background image:
                              
                             BGIF_HASSELECTIMAGE
                             -------------------
                             When this flag is set the background image consists of a pair of images 
                             of precisely the same size.  The first image will be displayed by the 
                             widget when the widget is in its unselected state -- the second image 
                             is displayed when the image is selected.  By default, this flag is off, 
                             indicating that the image contains only a single item to be displayed, 
                             regardless of the current selection state.
                                    
                             BGIF_TILED
                             ----------
                             This flag indicates that the background image should be tiled inside the 
                             widget's extent.  The image will fill the full extent of the widget.  If 
                             the image does not does not proportionately fit inside the extent, the 
                             image will be clipped to fit.  

                             BGIF_IMAGEUNDER
                             ---------------
                             This flag indicates that the background image will be drawn underneath
                             the background fill color, if any. If this flag is set and a 
                             background fill color is also set, it should have some degree of 
                             transparency in the alpha channel otherwise the background image will
                             not be visible. By default this flag is off, indicating that the 
                             background image is drawn over the background fill color. 

                             IDF_ALIGN_LEFT
                             ---------------
                             This flag indicates that the background image will be drawn so that
                             the left edge of the image will be aligned to the inner left edge of 
                             the widget. 
                             
                             IDF_ALIGN_RIGHT
                             ---------------
                             This flag indicates that the background image will be drawn so that
                             the right edge of the image will be aligned to the inner right edge of 
                             the widget. 
                             
                             IDF_ALIGN_CENTER
                             ----------------
                             This flag indicates that the background image will be drawn so that
                             image is horizontally centered within the widget.
                             
                             IDF_ALIGN_TOP
                             ----------------
                             This flag indicates that the background image will be drawn so that
                             the top edge of the image will be aligned to the inner top edge of 
                             the widget. 
                             
                             IDF_ALIGN_BOTTOM
                             ----------------
                             This flag indicates that the background image will be drawn so that
                             the bottom edge of the image will be aligned to the inner bottom edge of 
                             the widget. 
                             
                             IDF_ALIGN_MIDDLE
                             ----------------
                             This flag indicates that the background image will be drawn so that
                             image is vertically centered within the widget.
         

                               Property Value:  uint32


PROP_BGIMAGE_PARM:           This property is used to pass IImage parameters to the background 
                             image by using IImage_SetParm(). The property value is a pointer to an 
                             ImageParm data structure, which contains an IPARM value and two generic 
                             arguments. See AEEImage.h for usage and meanings of the IPARM values.

                               Property Value: ImageParm*


PROP_BGIMAGE_FRAME:         This property is used to set the currently displayed frame in a multiple
                            frame image. Frames are numbered from zero to 'number-of-frames' minus
                            one.  Setting this property has the side effect of stopping image 
                            animation.

                               Property Value: int


PROP_BGIMAGE_ANIMATE:       This property is used to start or stop the animation of a background
                            image.  

                               Property Value: boolean


PROP_BGIMAGE_TILED_BORDER:  This property is used to enable 'border tiling' and to set the dimensions 
                            of the left, top, right and bottom borders.  Border tiling is described
                            below.

                               Property Value: BGImageTiledBorder*

PROP_BGIMAGE_ANIMATE_FLAGS: This property holds the flags related to background image animation.

                            AF_ENABLE_EVT_STEP
                            ------------------
                            This flag allows the user to enable StepEvent to be sent through the
                            widget's ViewModel.  When background image animates, StepEvent will only
                            be passed to model listeners if this flag is set to true.  
                       
                               Property Value:  uint32

Background image tiling:
   Tiling of the background image is supported by providing the ability to copy (tile) a source 
   image multiple times across a larger region. Tiling can be simple multiple copies of the 
   source, or it can be a bordered tile pattern.
   
   The following diagram shows a source image that consists of a 3x3 grid of colored squares.

   
   +---+---+---+
   | 1 | 2 | 3 |
   +---+---+---+
   | 4 | 5 | 6 |
   +---+---+---+
   | 7 | 8 | 9 |
   +---+---+---+



   If the tile source image were to simply fill a widget background, the result would be
   the following:


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
   

   
   In addition to simple tiling, border tiling is supported, which can preserve the 
   border of a source image. This tiling method is specified with the PROP_BGIMAGE_TILED_BORDER
   property, which passes a BGImageTiledBorder data structure. This structure specifies the
   width or height in pixels of each border tile. To reset border tiling back to simple
   tiling, set the PROP_BGIMAGE_TILED_BORDER property passing a BGImageTiledBorder data 
   structure with all data members set to zero. 

   The usage of the BGImageTiledBorder members are described by the following diagram:

              left   right
             |---|   |---|
           
           - +---+---+---+
       top | | 1 | 2 | 3 |
           - +---+---+---+
             | 4 | 5 | 6 |
           - +---+---+---+
    bottom | | 7 | 8 | 9 |
           - +---+---+---+


   If the border attributes are used on this example such that the top, left, right and 
   bottom border widths all fit the internal squares, then the region is tiled as follows:


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
   

   
   The cells marked with a asterisk (*) might be clipped in order to fill the area.

   Image tiling also supports a selected image.  Use the selected image just as you would 
   in an non-tiled Image Widget.  



Required Model:
   None
   
Comments:
   None
   
See Also:
   Model Events
   StepEvent
   ISHELL_CreateInstance()
*/

#endif /* __AEEBORDERWIDGET_H__ */
