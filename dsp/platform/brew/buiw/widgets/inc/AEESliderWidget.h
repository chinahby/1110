/*
  ========================================================================

  FILE:  AEESliderWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEESLIDERWIDGET_H__
#define __AEESLIDERWIDGET_H__

#include "AEEWProperties.h"


#include "bid\AEECLSID_SLIDERWIDGET.bid"


static __inline int IWIDGET_GetSliderMaxPos(IWidget *po, uint16 *pnValue) {
   int nTemp = 0;
   int nErr = IWIDGET_GetPropInt(po, PROP_SLIDER_MAXPOS, &nTemp);
   *pnValue = (uint16)nTemp;
   return nErr;
}
static __inline int IWIDGET_SetSliderMaxPos(IWidget *po, uint16 pos) {
   return IWIDGET_SetProperty(po, PROP_SLIDER_MAXPOS, (uint32)pos);
}

static __inline int IWIDGET_GetSliderHandleBorderColor(IWidget *po, RGBVAL *prgb) {
   return IWIDGET_GetPropRGB(po, PROP_SLIDER_HANDLEBORDERCOLOR, prgb);
}
static __inline int IWIDGET_SetSliderHandleBorderColor(IWidget *po, RGBVAL rgb) {
   return IWIDGET_SetProperty(po, PROP_SLIDER_HANDLEBORDERCOLOR, (uint32)rgb);
}

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Slider Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   slider widget by creating a new instance of the Slider Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the slider widget is defined below.

Definition:

Identifier              Definition
----------              -----------------------------------
AEECLSID_SLIDERWIDGET:  Identifies the class ID of the slider widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Slider Widget Class

Description:
   The Slider widget object is a user-interface element that provides a means
   to control a setting over a predetermined range. Sliders are best used
   for settings that have continuous variable values, such as display 
   brightness or contrast. 
   
   Sliders can be positioned horizontally or vertically. Horizontal layout 
   is recommended. 

   Sliders do not include text labels. Use a static text widget to create 
   labels for a slider.

   Slider widgets use a value model to supply the value for the slider setting.

   Slider appearance and color values.

===pre>                               
                                            ------ Slider "handle"
                                           |
                                           V
                               +-+-+-+-+-+-+               
                               |.|.|.|.|.|.|               
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
   |*|*|*|*|*|*|*|*|*|*|*|*|*|*|.|*|*|*|*|.|*|*|*|*|*|*|*|*|*|*|*|*|*|*| 
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
   |*|b|b|b|b|b|b|b|b|b|b|b|b|b|.|*|*|*|*|.|b|b|b|b|b|b|b|b|b|b|b|b|b|*| 
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
   |*|*|*|*|*|*|*|*|*|*|*|*|*|*|.|*|*|*|*|.|*|*|*|*|*|*|*|*|*|*|*|*|*|*| 
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
                               |.|.|.|.|.|.|               
                               +-+-+-+-+-+-+               
===/pre>
                           
   
   To create a slider widget, an application would do the following:
===pre>
      1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_SLIDERWIDGET 
         to create an instance of the slider widget.
      2. Set properties to control the widget's appearance or function.
      3. Set the extent of the widget.
      4. Place the widget in a container.
===/pre>

Supported Events:
   The slider widget handles the following set of events:
   
===pre>
Event                   Description   
-----                   -----------------------------------------------------
EVT_WDG_SETFOCUS        The slider widget responds to this event by displaying
                        itself in the new focus state and by returning TRUE to
                        indicate that it accepts focus.
                     
EVT_KEY                 The slider widget handles this event when the 'wParam'
                        parameter is one of {AVK_LEFT, AVK_RIGHT} for horizontal
                        orientation and when one of {AVK_UP, AVK_DOWN} for 
                        vertical orientation. These combinations increment or
                        decrement the position of the slider and its underlying
                        data model value, and return TRUE to indicate that the
                        event was handled.

EVT_WDG_SETPROPERTY     This event allows various properties to be set on the
                        slider widget. The properties handled are described below.
                        
EVT_WDG_GETPROPERTY     This event allows various properties to be retrieved from
                        the slider widget. The properties handled are described 
                        below.
===/pre>
                        
Properties:
Property                Description
--------                ------------------------------------------------------------
PROP_MAXPOS             This property gets/sets the value of the maximum position 
                        of the slider handle. The minimum position value is always zero
                        and visually occurs when the slider handle is in the left-most
                        position (for horizontal orientation) or bottom-most position 
                        (for vertical orientation), of the slider range. Conversely, 
                        the maximum position value is set by PROP_MAXPOS and visually
                        occurs when the handle is in the right-most or top-most 
                        position of the slider range.  

PROP_SLIDER_HANDLEBORDERCOLOR
                        This property gets/sets the RGB value of the border area around
                        the slider handle. In figure (1), this color is represented
                        by the '.' character. The other slider colors are determined by 
                        standard border properties.

PROP_LAYOUTSTYLE        This property gets/sets the current layout style of the slider
                        widget. The layout style value passed in the 'dwParam' 
                        parameter may be one of two values: LAYOUT_VERT for vertical 
                        orientation or LAYOUT_HORZ for horizontal orientation. 
                        Vertical orientation means that the slider handle moves 
                        vertically with the minimum value being at the bottom of the 
                        slider range and the maximum value being at the top of the 
                        range. Horizontal orientation means that the slider handle 
                        moves horizontally with the minimum value being at the left 
                        side of the slider range and the maximum value being at the 
                        right side of the range.
                        
PROP_INACTIVE_BORDERCOLOR
PROP_ACTIVE_BORDERCOLOR
PROP_SINACTIVE_BORDERCOLOR
PROP_SACTIVE_BORDERCOLOR
PROP_BORDERCOLOR        These properties determine the color of the border of the 
                        slider range and the handle fill area. This color is 
                        represented by the '*' character in figure (1). There are
                        different properties and colors for each of the four 
                        possible selected/active states. PROP_BORDERCOLOR is a shortcut
                        property for setting all four state values to the same
                        color.
                        

PROP_INACTIVE_BGCOLOR
PROP_ACTIVE_BGCOLOR
PROP_SINACTIVE_BGCOLOR
PROP_SACTIVE_BGCOLOR
PROP_BGCOLOR            These properties determine the color of the inside of the 
                        slider range. This color is represented by the 'b' character 
                        in figure (1). There are different properties and colors for 
                        each of the four possible selected/active states. PROP_BGCOLOR 
                        is a shortcut property for setting all four state values to the 
                        same color.
                        
Required Model:
   IValueModel
   

Model Data:
===pre>   
   uint32  modelData;
===/pre>
   
   The slider widget relies on a value model that presents its data as an 
   unsigned integer in the range of zero to 'MaxPos', which is the 
   maximum position value and is set by the property: PROP_MAXPOS. 
   
   For example, if 'MaxPos' is set to the value (4), there are five possible
   values and corresponding positions for the slider handle: (0,1,2,3,4).
   Continuing with the above example, if the modelData is set to '1', the
   slider handle will be positioned offset from the left side at one-fifth 
   of the length of the slider: 
   
===pre>   
      +-+
   ===| |===========
      +-+
   0   1   2   3   4
===/pre>
   
   
Comments:
   None
   

See Also:
   Model Events
   ISHELL_CreateInstance()      


===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_GetSliderMaxPos()

Description:
   This function retrieves the maximum position value for the slider. 
   The maximum position value determines the number of steps the slider 
   handle may move through and the maximum value of the integer model data.

Prototype:
   int IWIDGET_GetSliderMaxPos(IWidget *po, uint16 *pnValue);

Parameters:
===pre>
   po:       A pointer to the IWidget object (slider widget).
   pnValue:  Pointer to an integer variable that will receive the 
             value of the slider widget's maximum position.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to successfully retrieve the maximum 
             position value.
   EFAILED - The widget was unable to retrieve the maximum position value.
===/pre>

Comments:
   None.

Side Effects:
   None.

See Also:
   PROP_MAXPOS
   IWIDGET_SetSliderMaxPos()


===========================================================================

IWIDGET_SetSliderMaxPos()

Description:
   This function sets the maximum position value for the slider widget. 
   The maximum position value determines the number of steps the slider 
   handle may move through and the maximum value of the integer model data.

Prototype:
   int IWIDGET_SetSliderMaxPos(IWidget *po, uint16 pos);

Parameters:
===pre>
   po:       A pointer to the IWidget object (slider widget).
   pnValue:  Value to set as the slider widget's maximum position.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to successfully set the maximum position value.
   EFAILED - The widget was unable to set the maximum position value.
===/pre>

Comments:
   None.

Side Effects:
   If the maximum position is greater than the current value of the position
   as store in the model data, the model data is limited to the new maximum
   position value.

See Also:
   PROP_MAXPOS
   IWIDGET_GetSliderMaxPos()


===========================================================================

IWIDGET_GetSliderHandleBorderColor()

Description:
   This function retrieves the RGB value of the slider handle's border.

Prototype:
   int IWIDGET_GetSliderHandleBorderColor(IWidget *po, RGBVAL *prgb);

Parameters:
===pre>
   po:    Pointer to the IWidget object (slider widget).
   prgb:  Pointer to an RGBVAL variable to receive the handle border color.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to successfully retrieve the handle border color.
   EFAILED - The widget was unable to retrieve the handle border color.
===/pre>

Comments:
   None.

Side Effects:
   None.

See Also:
   PROP_HANDLEBORDERCOLOR:
   IWIDGET_SetSliderHandleBorderColor()


===========================================================================

IWIDGET_SetSliderHandleBorderColor()

Description:
   This function sets the RGB value of the slider handle's border.

Prototype:
   int IWIDGET_SetSliderHandleBorderColor(IWidget *po, RGBVAL rgb);

Parameters:
===pre>
   po:    Pointer to the IWidget object (slider widget).
   prgb:  Color value to set as the handle border color.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget was able to successfully set the handle border color.
   EFAILED - The widget was unable to set the handle border color.
===/pre>

Comments:
   None.
   
Side Effects:
   None.

See Also:
   PROP_HANDLEBORDERCOLOR:
   IWIDGET_GetSliderHandleBorderColor()


===========================================================================
*/


#endif /*__AEESLIDERWIDGET_H__*/




