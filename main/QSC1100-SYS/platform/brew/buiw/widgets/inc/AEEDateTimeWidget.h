/*
  ========================================================================

  FILE:  AEEDateTimeWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEEDATETIMEWIDGET_H__
#define __AEEDATETIMEWIDGET_H__

#include "AEEWProperties.h"
#include "AEELocale.h"

#include "bid/AEECLSID_DATEWIDGET.bid"
#include "bid/AEECLSID_TIMEWIDGET.bid"
#include "bid/AEECLSID_JULIANDATEWIDGET.bid"
#include "bid/AEECLSID_JULIANTIMEWIDGET.BID"

#define PROP_JULIANVALUEMODEL          PROP_SPECIAL   // set to TRUE if the widget stores its date and time
                                                      // data as a JulianType

#define DTWF_UPDATE_ONFIELDCHANGE      1  // update the underlying value model on every field change 
#define DTWF_USEUPDOWNKEYS             2  // use the up/down arrow keys
#define DTWF_SHOWUPDOWNARROWS          4  // show up/down indicator arrows (only valid if DTWF_USEUPDOWNKEYS set)
#define DTWF_NOFIELDWRAP               8  // Do not wrap to/from first/last field in focus

static __inline int IWIDGET_SetLocale(IWidget *po, ILocale *piLocale) {
   return IWIDGET_SetProperty(po, PROP_LOCALE, (uint32)piLocale);
}

static __inline int IWIDGET_SetFormatString(IWidget *po, const AECHAR *pwFormat) {
   return IWIDGET_SetProperty(po, PROP_FORMATSTRING, (uint32)pwFormat);
}

static __inline int IWIDGET_SetDelimiterString(IWidget *po, const AECHAR *pwDelim) {
   return IWIDGET_SetProperty(po, PROP_DELIMITER, (uint32)pwDelim);
}

static __inline int IWIDGET_SetFieldPad(IWidget *po, uint8 nPad) {
   return IWIDGET_SetProperty(po, PROP_FIELDPAD, (uint32)nPad);
}

static __inline int IWIDGET_SetArrowPad(IWidget *po, uint8 nPad) {
   return IWIDGET_SetProperty(po, PROP_ARROWPAD, (uint32)nPad);
}

static __inline int IWIDGET_GetFieldPad(IWidget *po, uint8 *pnPad) {
   return IWIDGET_GetProperty(po, PROP_FIELDPAD, (uint32*)pnPad);
}

static __inline int IWIDGET_GetArrowPad(IWidget *po, uint8 *pnPad) {
   return IWIDGET_GetProperty(po, PROP_ARROWPAD, (uint32*)pnPad);
}

static __inline int IWIDGET_GetJulianTypeValueModel(IWidget *po, boolean *bIsJulianType) {
   return IWIDGET_GetProperty(po, PROP_JULIANVALUEMODEL, (uint32*)bIsJulianType);
}

static __inline int IWIDGET_SetJulianDateTime(IWidget *po, JulianType *pJulian) {

   IValueModel *pivm = 0;
   boolean bIsJulianTypeValueModel;
   JulianType *pjt;

   if (SUCCESS != IWIDGET_GetModel(po, AEEIID_VALUEMODEL, (IModel **)&pivm)) {
      return EFAILED;
   }

   if (pivm) {
      if ((SUCCESS == IWIDGET_GetJulianTypeValueModel(po, &bIsJulianTypeValueModel))
         && bIsJulianTypeValueModel) {
            pjt = (JulianType *)MALLOC(sizeof(JulianType));
            if (pjt) {
               *pjt = *pJulian;
               IVALUEMODEL_SetValue(pivm, (void*)pjt, sizeof(JulianType), WidgetAEEFree);
            } else {
               IVALUEMODEL_Release(pivm);
               return ENOMEMORY;
            }
      } else {
         IVALUEMODEL_SetValue(pivm, (void*)JULIANTOSECONDS(pJulian), 0, NULL);
      }
      IVALUEMODEL_Release(pivm);
   }
   return SUCCESS;
}

static __inline int IWIDGET_GetJulianDateTime(IWidget *po, JulianType *pJulian) {

   IValueModel *pivm = 0;
   boolean bIsJulianTypeValueModel;

   if (SUCCESS != IWIDGET_GetModel(po, AEEIID_VALUEMODEL, (IModel **)&pivm)) {
      return EFAILED;
   }

   if (pivm) {
      if ((SUCCESS == IWIDGET_GetJulianTypeValueModel(po, &bIsJulianTypeValueModel))
         && bIsJulianTypeValueModel) {
         *pJulian = *((JulianType *)IVALUEMODEL_GetValue(pivm, NULL));
      } else {
         uint32 secs = (uint32)IVALUEMODEL_GetValue(pivm, NULL);
         GETJULIANDATE(secs, pJulian);
      }
      IVALUEMODEL_Release(pivm);
   }
   return SUCCESS;
}

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Date Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   date widget by creating a new instance of the Date Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  An application creates a time widget by 
   creating a new instance of the Time Widget class, passing the ID of the class to 
   ISHELL_CreateInstance().  
   
   The object that is created is itself a widget and may utilize those APIs defined 
   for the IWidget interface.
   
   The class ID used to identify the date widget is defined below.

Definition:

Identifier                                                        Definition
----------                                                            -----------------------------------
AEECLSID_JULIANDATEWIDGET:          Identifies the class ID of the julian date widget
AEECLSID_JULIANTIMEWIDGET:          Identifies the class ID of the julian time widget
AEECLSID_DATEWIDGET:                        Identifies the class ID of the date widget (DEPRECATED)
AEECLSID_TIMEWIDGET:                        Identifies the class ID of the time widget (DEPRECATED)

Comments:
   The date widget (AEECLSID_DATEWIDGET) and time widget (AEECLSID_TIMEWIDGET)
   are deprecated and have been replaced by their julian counterparts.

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================
Date and Time Widget Classes
 
   This document describes two similar widgets -- a date widget and a time 
   widget -- each of which is used to display and set a date/time variable.  
   These widgets share common behaviors, such as how they handle events or 
   interact with their attached model, but differ in what fields they 
   contain and what data they expose from their model. 
   
   Features common to both the date and time widgets:
   ===pre>
      - Automatic field verification and date/time validation.
      - Field order is configurable
      - Fields specified by a format pattern string or ILocale object
      - Field delimiter specified by a string or ILocale object
      - Value model updating and notification performed in two different 
        configurable modes, on field change or on focus change.
      - Fields auto-incremented and decremented with the up/down arrow keys.
  ===/pre>    
   For a descriptions of each widget, follow links:
   
===pre>
	Julian Date Widget Class

	Julian Time Widget Class

	Date Widget Class (DEPRECATED)

	Time Widget Class (DEPRECATED)
===/pre>
===h2>
View Model Notifications:
===/h2>

   The date and time widgets will send event notifications through their view model for
   the following widget events:

===pre>   
Event                   Description
-----                   -----------------------------------------------------------
EVT_MDL_FIELDCHANGE:    The date and time widget send this notification when the current 
                        field selection has been changed by user navigation.  The 
                        'dwParam' value of the ModelEvent struct contains a composite 
                        value.  The high order word contains the zero-based index of the 
                        new field.  The low order word contains a boolean signifying the 
                        direction of movement.  
                        
                        HIWORD(dwParam) (dwParam >> 16)  --  (uint16) new field index
                        LOWORD(dwParam) (dwParam & 0xFF) --  (boolean) direction of movement
                                                             TRUE means forward (right) movement
                                                             FALSE means backward (left) movement
                        
                        For example, when moving from field 1 to field 2, the widget will 
                        send an EVT_MDL_FIELDCHANGE notifation with the high order word of 
                        the dwParam member set to 2, and the low order word of the dwParam 
                        member set to 1 (TRUE).  

===/pre>
===h2>   
Supported Events:
===/h2>
   The date and time widgets handle the following events:

===pre>
Event                   Description
-----                   ------------------------------------------------------------
EVT_WDG_SETPROPERTY:    The date and time widgets responds to this event by attempting to set the
                        property identified by the 'wParam' parameter.  The date and time widgets
                        allow the following properties to be set:
                        
                           PROP_FONT                     --  Sets the font to be used when displaying the
                                                             widget field text.
                           PROP_FONT_CLASS               --  Sets the font to be used when displaying
                                                             widget field text
                           PROP_FONT_OUTLINEWIDTH        --  Sets the font's outline width.  Must first
                                                             use PROP_FONT to set the font to a font
                                                             object that understands this property, no
                                                             effect otherwise.
                           PROP_FONT_OUTLINECOLOR        --  Sets the font's outline color.  Must first
                                                             use PROP_FONT to set the font to a font
                                                             object that understands this property, no
                                                             effect otherwise.
                           PROP_LOCALE                   --  Set the ILocale object that the widget will 
                                                             use to retrieve date/time display and entry 
                                                             characteristics.  Note that this property will 
                                                             override any previous settings of 
                                                             PROP_FORMATSTRING and PROP_DELIMITER.
                           PROP_FORMATSTRING             --  Set a format string that the widget will
                                                             use to determine date/time display and
                                                             entry characteristics. Note that this property
                                                             will override any previous settings of PROP_LOCALE.
                           PROP_DELIMITER                --  Set a delimiter string that the widget will
                                                             display between fields.  Note that this property
                                                             will override any previous settings of PROP_LOCALE.
                           PROP_FIELDPAD                 --  Set the amount of padding in pixels that will be
                                                             displayed around each side of the bounding 
                                                             rectangle of the  field text. 
                           PROP_ARROWPAD                 --  Set the amount of padding that will be displayed
                                                             between the field selection rectangle and the up and 
                                                             down arrows, when visible.
                           PROP_FLAGS                    --  Set various flags that effect the behavior
                                                             of the date or time widget.
                           PROP_FOCUSINDEX               --  Sets which field is focused and highlited in the date/
                                                             time widget.  This is zero-based, so pass 0 to 
                                                             IWIDGET_SetFocusIndex() to select the first field.
                           PROP_FGCOLOR                  --  Sets the text color to be used when the
                                                             widget is selected, unselected, active or
                                                             inactive. 
                           PROP_SACTIVE_FGCOLOR          --  Sets the color of text when the widget is
                                                             active.  
                           PROP_TEXT_SELECTED_FGCOLOR    --  Sets the text color for the selected text
                           PROP_TEXT_SACTIVE_FGCOLOR     --  Sets the text color for the selected text when the 
                                                             widget is active
                           PROP_TEXT_SELECTED_BGCOLOR    --  Sets the background text color for the selected text
                           PROP_TEXT_SACTIVE_BGCOLOR     --  Sets the background text color for the selected text  
                                                             when the widget is active
                           PROP_BGCOLOR                  --  Sets the background color to be used when the
                                                             widget is selected, unselected, active or 
													                      inactive.
                           PROP_SACTIVE_BGCOLOR          --  Sets the color of the selection when the widget
                                                             is active.
                                                                     
                        These properties are discussed below in greater detail.

                        
EVT_WDG_GETPROPERTY:    The date and time widgets respond to this event by attempting to retrieve the
                        property identified by the 'wParam' parameter.  The date and time widgets
                        allow the following properties to be retrieved:
                      
                           PROP_FONT                     --  Retrieves the font used to display the text.
                           PROP_FONT_OUTLINEWIDTH        --  Retrieves the current outline width of the
                                                             display text if the font supports this
                                                             property.
                           PROP_FONT_OUTLINECOLOR        --  Retrieves the current outline color of the
                                                             display text if the font supports this
                                                             property.
                           PROP_FIELDPAD                 --  Retreive the amount of padding in pixels that will be
                                                             displayed around each side of the bounding 
                                                             rectangle of the  field text.  When the field is 
                                                             selected, the selection highlight will encompass 
                                                             the text bounding rectangle plus the field pad.
                           PROP_ARROWPAD                 --  Retrieve the amount of padding that will be displayed
                                                             between the top and bottom of the field selection
                                                             rectangle and the up and down arrows, when visible.
                           PROP_FLAGS                    --  Retrieves the flags that effect the behavior
                                                             of the widget.
                           PROP_SACTIVE_FGCOLOR          --  Gets the active selected text color.
                           PROP_FOCUSINDEX               --  Retrieves which field is selected in the date/
                                                             time widget.  This is zero-based, so when the first
                                                             field is selected, IWIDGET_GetFocusIndex() will 
                                                             return 0.
                           PROP_JULIANVALUEMODEL         --  Used to determine if the widget is storing its date
                                                             data in its value model as a JulianType versus a long.
                                                             The Date/Time Widgets ill always return FALSE, the
                                                             Julian Date and Julian Time widgets will return TRUE.
                           PROP_FGCOLOR                  --  Retrieves the text color used when the widget is selected,
                                                             unselected, active or inactive.  
                           PROP_SACTIVE_FGCOLOR          --  Retrieves the color of selected text when the widget is
                                                             active.
                           PROP_TEXT_SELECTED_FGCOLOR    --  Retrieves the color of selected text color
                           PROP_TEXT_SACTIVE_FGCOLOR     --  Retrieves the color of text for the selected text when the 
                                                             widget is active
                           PROP_TEXT_SELECTED_BGCOLOR    --  Retrieves the background color of selected text color
                           PROP_TEXT_SACTIVE_BGCOLOR     --  Retrieves the background color of text for the selected text 
                                                             when the widget is active
                           PROP_BGCOLOR                  --  Retrieves the background color used when the widget is selected, 
                                                             unselected, active or inactive.
                           PROP_SACTIVE_BGCOLOR          --  Retrieves the color of the seleciton used when the widget
                                                             is active.
                                                      
   
                        These properties are discussed below in greater detail.
===/pre>
                                             
Properties:

Property                Description
--------                ------------------------------------------------------------
PROP_FONT:              This property contains a pointer to the IFont object that will be used to render
                        the text displayed within the date or time widget.
                                
                           Property Value:  IFont *
                             
PROP_FONT_CLASS:        This property sets the class ID to create an IFont object that will be used to render
                        the text displayed within the date or time widget.It is a shortcut for PROP_FONT 
                        as it saves having to create an IFont object and then passing it in.
                        
                           Property Value: AEECLSID

PROP_FONT_OUTLINEWIDTH: This property sets the width of the outline to be applied when rendering
                        text within the static widget.  Setting this property only effects the
                        text if an IFont object (such as an IHFont)  that understands this 
                        property was set earlier via the PROP_FONT property.

                           Property Value: int

PROP_FONT_OUTLINECOLOR: This property sets the color of the outline to be applied when rendering
                        text within the static widget.  Setting this property only effects the
                        text if an IFont object (such as an IHFont)  that understands this 
                        property was set earlier via the PROP_FONT property.

                           Property Value: RGBAVAL
                             
PROP_LOCALE:            This property sets the ILocale object that the widget will use to retrieve date/time 
                        display and entry characteristics such as date or time pattern string, field delimiter, 
                        and AM/PM designators. Note that this property will override any previous settings of 
                        PROP_FORMATSTRING and PROP_DELIMITER.  The ILocale items that are used by the date and
                        time widget are shown below. For more information on ILocale, see AEELocale.h
                         
                           ILocale parm items used by Time widget
                           --------------------------------------
                           ILOCALEPARM_DATETIME_SHORTTIMEPATTERN
                           ILOCALEPARM_DATETIME_TIMESEPARATOR
                           ILOCALEPARM_DATETIME_AMDESIGNATOR
                           ILOCALEPARM_DATETIME_PMDESIGNATOR
                            
                           ILocale parm items used by Date widget
                           --------------------------------------
                           ILOCALEPARM_DATETIME_SHORTDATEPATTERN
                           ILOCALEPARM_DATETIME_DATESEPARATOR

                           
                           Property Value: ILocale *
                           

PROP_FORMATSTRING       This property sets a format string that the widget will use to determine 
                        date/time field order and field characteristics.  Note that this property
                        will override any previous settings of PROP_LOCALE.  The format string is
                        a NULL-terminated AECHAR string compatible with the short date and time 
                        patterns returned from ILocale (ILOCALEPARM_DATETIME_SHORTDATEPATTERN and 
                        ILOCALEPARM_DATETIME_SHORTTIMEPATTERN). The strings are composed of patterns 
                        that specify desired entry fields in the widget, shown below.  All other 
                        characters in the string are ignored.
                        
                           Property Value: AECHAR *
                        
                        
                        Allowable format patterns for date widget
                        -----------------------------------------
                        
                        d | dd | ddd | dddd     day, (two digits)
                        M | MM | MMM | MMMM     month (two digits)
                        y | yy | yyy            year (two digits)
                        Y | yyyy                year (four digits)

                        
                        Example date format string:
                           
                           const AECHAR wszFormat[] = {'M','d','Y',0};  
                           // fields displayed are: month (2 digits)
                           //                       day (2 Digits)
                           //                       year (4 digits)


                        
                        Allowable format patterns for time widget
                        -----------------------------------------
                        
                        h | hh                  hour, (1-12, two digits)
                        H | HH                  hour, (1-24, two digits)
                        m | mm                  minute, (two digits)
                        s | ss                  seconds, (two digits)
                        t                       AM/PM designator (single char)
                        T | tt                  AM/PM designator (two chars) 

                          
                        Example time format string:
                           
                           const AECHAR wszFormat[] = {'h','m','s','T',0};  
                           // fields displayed are: hours (12 hour time)
                           //                       minutes
                           //                       seconds
                           //                       AM/PM (2 chars)
                          

                          
PROP_DELIMITER          This property sets a delimiter string that the widget will display between fields.  
                        The delimiter string is a NULL-terminated AECHAR string. Note that this property 
                        will override any previous settings of PROP_LOCALE.
                        
                           Property Value: AECHAR *
                           
                           
PROP_FIELDPAD           This property sets the amount of padding, in pixels, that will be displayed around 
                        each side of the bounding rectangle of the field text. When the field is selected, 
                        the selection highlight will encompass the text bounding rectangle plus the field pad.
                          
                           Property Value: uint8
                           

PROP_ARROWPAD           This property sets the amount of padding, in pixels, that will be displayed
                        between the top and bottom of the field selection rectangle and the Up.Down
                        arrows, when visible.
                        
                           Property Value: uint8


PROP_FLAGS              This property contains a set of flags that effect the behavior of the date and 
                        time widget.

                        DTWF_UPDATE_ONFIELDCHANGE
                        -------------------------
                        When this flag is set, the widget will update and notify through 
                        it's attached value model whenever the field selection is moved from 
                        a dirty field.  (A dirty field is one whose value has been changed).  
                        Without this flag set, the default behavior is to only update and 
                        notify through the value model when focus is removed from the widget 
                        and one or more fields are dirty.  
                        
                        
                        DTWF_USEUPDOWNKEYS
                        ------------------
                        When this flag is set, the widget will accept and handle Up/Down 
                        (AVK_UP/AVK_DOWN) keys by incrementing or decrementing the current 
                        field value.  Without this flag set, the default behavior is to 
                        ignore Up/Down keys.  
                        
                        
                        DTWF_SHOWUPDOWNARROWS
                        ---------------------
                        When this flag is set in conjunction with DTWF_USEUPDOWNKEYS flag, 
                        the widget will display Up/Down indicator arrows above and below 
                        each field to indicate that the field may be incremented or 
                        decremented.  If this flag is used without DTWF_USEUPDOWNKEYS, it 
                        will be ignored.  

                        DTWF_NOFIELDWRAP
                        -----------------
                        When this flag is set, the widget will not wrap from first field to
                        the last field when left key (AVK_LEFT) is pressed, or from 
                        last field to first field when right key (AVK_RIGHT) is pressed. 
                        If this flag is not set, then corresponding keys will  be handled
                        by the widget and widget will wrap from/to first/last field on 
                        AVK_LEFT/AVK_RIGHT.


PROP_IMAGESTRIP:        This property contains the bitmap the widget uses when drawing the
                        Up/Down indicator arrows above and below each field that indicate 
                        that the field may be incremented or decremented.  The bitmap is
                        made up of a horizontal image strip of the up arrow image followed
                        by the down arrow.  
                        
                        Setting PROP_IMAGESTRIP  to NULL will cause the Date/Time Widget to 
                        revert to using the default images for the up and down indicators.

                        The bitmap is AddRef'ed interally and released when no longer
                        needed by the widget.
                             
                             Property Value:  IBitmap

PROP_FOCUSINDEX:        This property contains which field in the date/time widget is
                        currently selected.  This value is zero-based, so index 0 indicates
                        the first field in the widget.  Use IWIDGET_SetFocusIndex() and
                        IWIDGET_GetFocusIndex() to programatically select fields.
                             
                             Property Value:  int

PROP_JULIANVALUEMODEL:  Get Only.  This property contains information about how the widget 
                        stores its date data in its internal value model.  This will be 
                        TRUE if the widget is a Julian Date or Julian Time widget, as these
                        store their dates as JulianTypes internally.  Regular date and time
                        widgets will report this property as being FALSE.

                              Property Value: boolean

PROP_FGCOLOR:           This property contains the color the date and time widget will use to 
                        draw its text.  Setting PROP_FGCOLOR will set the text color for all
                        combinations of widget selection and activation, as indicated by the
                        following table:
                             
                                                                       Text color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         PROP_FGCOLOR
                               Unselected         Active           PROP_FGCOLOR
                               Selected           Inactive         PROP_FGCOLOR
                               Selected           Active           PROP_FGCOLOR
                             
                        Though setting the PROP_FGCOLOR will effect _all_ combinations of
                        selection and activation, getting the value of the property will return
                        the text color that will be applied when drawing the widget in its active,
                        unselected state.

                        Note, setting PROP_FGCOLOR automatically sets the color to be used for
                        selected text to RGB_WHITE.  This allows applications to set the text
                        color to RGB_BLACK without making selected text invisible.  If your app
                        needs to draw selected text in a custom color, set PROP_SACTIVE_FGCOLOR
                        after setting PROP_FGCOLOR.
                             
                             Property Value:  RGBVal

PROP_SACTIVE_FGCOLOR:   This property contains the color the date and time widget will use to 
                        draw the text within the widget when it is active.
                             
                             Property Value:  RGBVal

PROP_TEXT_SELECTED_FGCOLOR
                        This property contains the color the date and time widget will use to 
                        draw selected text within the widget.

                             Property Value:  RGBVal

PROP_TEXT_SACTIVE_FGCOLOR
                        This property contains the color the date and time widget will use to 
                        draw the selected text when the widget is active.

                             Property Value:  RGBVal
                             
PROP_TEXT_SELECTED_BGCOLOR
                        This property contains the background color the date and time widget will 
                        use to draw selected text within the widget.

                             Property Value:  RGBVal

PROP_TEXT_SACTIVE_BGCOLOR
                        This property contains the background color the date and time widget will
                        use to draw the selected text when the widget is active.

                             Property Value:  RGBVal

PROP_BGCOLOR            This property contains the color the date and time widget will use as 
                        its background.  Setting PROP_BGCOLOR will set the background color for 
                        all combinations of widget selection and activation, as indicated by the
                        following table:
                             
                                                                    Background color when 
                               Selection State    Active State     the widget is displayed
                               ---------------    ------------     -----------------------
                               Unselected         Inactive         PROP_BGCOLOR
                               Unselected         Active           PROP_BGCOLOR
                               Selected           Inactive         PROP_BGCOLOR
                               Selected           Active           PROP_BGCOLOR
                             
                        Though setting the PROP_BGCOLOR will effect _all_ combinations of
                        selection and activation, getting the value of the property will return
                        the background color that will be applied when drawing the widget in its 
                        active, unselected state.
                             
                             Property Value:  RGBVal

PROP_SACTIVE_BGCOLOR:   This property contains the color the date and time widget will use to 
                        draw highliting for selected text.  Text selection is drawn only when 
                        the widget is selected and active.
                             
                             Property Value:  RGBVal
                           
Required Model:

   IValueModel 
 
Model Data:

   Julian Date Widget, Julian Time Widget

   JulianType  julian;  // julian date structure representing any date

   The julian date and julian time widgets differ from their counterparts in that
   they rely on a value model that contains a julian date structure (rather than a
   uint32) representing the date and time.  
   
   A default model is created and assigned to the widget when the widget is created with 
   ISHELL_CreateInstance().  The client can replace the widget's model at any time with 
   the method: IWIDGET_SetModel(), as long as the supplied model interface is of the type 
   that the widget expects.  

  The Julian Date and Julian Time widgets both perform validation of the 
   JulianType date structure set in their model to ensure a valid date is stored
   and displayed.  If an invalid date is passed in, the widgets will change the
   offending fields to make the date valid. This keeps their behavior in line with 
   their non-julian counterparts.
   
   The data supplied in the value model must be of type JulianType.

   Date Widget, Time Widget      (DEPRECATED)

   uint32  seconds;  // number of seconds since phone reference time
   
   The date and time widgets rely on a value model that contains an unsigned 32-bit 
   integer representing the date/time value in seconds.  This value represents the number 
   of seconds since Jan 6 1980, the phone reference time.  Use the library function 
   GETJULIANDATE() to convert this value to a more usable JulianType data structure.  Use 
   the library function JULIANTOSECONDS() to convert a JulianType date/time to a seconds 
   value compatible with this widget's value mode.  
   
   When using JULIANTOSECONDS() to create a uint32 'seconds' value for use in a 
   time widget, make sure that the date fields of the JulianType contain a valid date.  
   Specifically, the wYear field should contain a value in the range of 1980-2115, the 
   wMonth field should contain a value in the range of 1-12, and the wDay field should 
   contain a value in the range of 1-31.  The wWeekDay field is ignored when computing 
   seconds with JULIANTOSECONDS().  
   
   A default model is created and assigned to the widget when the widget is created with 
   ISHELL_CreateInstance().  The client can replace the widget's model at any time with 
   the method: IWIDGET_SetModel(), as long as the supplied model interface is of the type 
   that the widget expects.  
   
   The data supplied in the value model must be of type uint32.

   NOTE: The date and time widgets are deprecated and should no longer be used.
===/pre>

Comments:
   None
   
See Also:
   Model Events
   ISHELL_CreateInstance()
   IWIDGET_SetFlags()
   JulianType


==============================================================
Julian Date Widget Class

Description: 
   The julian date widget is a user interface element that provides
   formatted date entry for the widget framework.  Date entry is 
   accomplished by typing numeric characters into fields representing 
   month, day (of month), and year.  The order of the fields and the 
   separator glyph displayed between fields is configurable.  On 
   creation, the julian date widget will create and use the current 
   system ILocale object, which provides language and locality specific 
   date formatting information.  
   
   A typical julian date widget might be configured to present a date in 
   the following format: "dd/mm/yyyy", meaning two digits for day, two 
   digits for month, and four digits for year, and a forward slash 
   between fields.  In this case the widget would look like this: 
===pre>

   +-----------------------------------------------------+
   |                                                     |
   |  +-------+       +-------+       +---------------+  |
   |  |       |     * |       |     * |               |  |
   |  | Day   |    *  | Month |    *  |   Year        |  |
   |  | Field |   *   | Field |   *   |   Field       |  |
   |  |       |  *    |       |  *    |               |  |
   |  |       | *     |       | *     |               |  |
   |  +-------+       +-------+       +---------------+  |
   |  (2 digits)      (2 digits)       (2 or 4 digits)   |
   |                                                     |
   +-----------------------------------------------------+
   
===/pre>
=======================================================
Julian Time Widget Class

Description: 
   The julian time widget is a user interface element that provides 
   formatted time entry for the widget framework.  Time entry is 
   accomplished by typing characters into fields representing hours, 
   minutes, seconds and meridiem (AM/PM).  The order of the fields and 
   the separator glyph displayed between fields is configurable.  On 
   creation, the time widget will create and use an ILocale object, 
   which provides language and locality specific time formatting 
   information.  
    
   A typical julian time widget might be configured to present a time in 
   the following format: "hh:mm:ss tt", meaning two digits for hour, 
   two digits for minutes,  two digits for seconds, and 2 digits for 
   meridiem (AM/PM), and a colon ":" between fields. In this case the 
   widget would look like this: 
===pre>
   +---------------------------------------------------------+
   |                                                         |
   |  +--------+     +--------+     +--------+   +--------+  |
   |  |        |     |        |     |        |   |        |  |
   |  | Hour   |  *  | Minute |  *  | Second |   |Meridiem|  |
   |  | Field  |     | Field  |     | Field  |   | Field  |  |
   |  |        |  *  |        |  *  |        |   |        |  |
   |  |        |     |        |     |        |   |        |  |
   |  +--------+     +--------+     +--------+   +--------+  |
   |  (2 digits)     (2 digits)     (2 digits)   (1 or 2 digits)
   |                                                         |
   +---------------------------------------------------------+
   
===/pre>

   To create a julian date or julian time widget, an application would do 
   the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of either:
                - AEECLSID_JULIANDATEWIDGET to create a date widget
                - AEECLSID_JULIANTIMEWIDGET to create a time widget
          
          ISHELL_CreateInstance() will return a reference counted instance of 
          the desired date or time widget.
       2. Call IWIDGET_SetJulianDateTime() to specify a date/time.  Note,
          both the julian date and julian time widgets will default to
          display the current date or time.
       3. Call IWIDGET_SetFlags() to specify widget specific behavior.
       4. Call IWIDGET_GetPreferredExtent() and IWIDGET_SetExtent() to size 
          the widget.
===/pre>

=====================================================================
Date Widget Class

Description: 
   The date widget class has been deprecated and replaced with the
   julian date widget class.  The julian date widget stores its date data 
   in a JuliaType date structure rather than a uint32 as the old date 
   widget class did.  While the old date widget is limited to displaying
   dates between 1980 and 2115, the julian date widget can display any
   arbitrary date that is represented by BREW's JulianType data type.
   Always use the new julian date widget class.

========================================================================
Time Widget Class

Description: 
   The time widget class has been deprecated and replaced with the
   julian time widget class.  The old time widget is incompatible with
   the new julian date widget described above and should no longer be
   used.  Instead, use the julian time widget class in your code.  This
   will ensure comaptibility with any julian date wigdet you may use
   in current or future code and allow the sharing of model data 
   between the two.

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_SetLocale

Description: 
   A convenience inline function for setting PROP_LOCALE with IWIDGET_SetProperty().  
   Sets the ILocale object that the widget will use to initialize itself with 

Prototype:
   int IWIDGET_SetLocale(IWidget *po, ILocale *piLocale);

Parameters:
===pre>
   po:         [in] Pointer to the IWidget object
   piLocale:   [in] The ILocale object to be queried by widget.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget handled the PROP_LOCALE.property
   EFAILED - Otherwise.
===/pre>

Comments:
   Since setting an ILocale object may cause the date/time widget to display a 
   completely different field presentation, it is recommended that 
   IWIDGET_GetPreferredExtent() and IWIDGET_SetExtent() be called on the widget 
   after this function call.  
   
Side Effects:
   Setting an ILocale object may cause the date/time widget to initialize itself
   with an entirely new set of fields and field delimiters. 

See Also:
   PROP_LOCALE
   IWidget
      

===========================================================================

IWIDGET_SetFormatString

Description:
   A convenience inline function for setting PROP_FORMATSTRING with IWIDGET_SetProperty().
   Sets the format string

Prototype:
   int IWIDGET_SetFormatString(IWidget *po, const AECHAR *pwFormat);

Parameters:
===pre>
   po:         [in] Pointer to the IWidget object
   pwFormat:   [in] Pointer to an AECHAR format string
===/pre>

Return Value:
===pre>
   SUCCESS - The widget handled the PROP_FORMATSTRING.property
   EFAILED - Otherwise.
===/pre>

Comments:
   Since setting a format string may cause the date/time widget to display a 
   completely different field presentation, it is recommended that 
   IWIDGET_GetPreferredExtent() and IWIDGET_SetExtent() be called on the widget 
   after setting this property.
   
Side Effects:
   Setting a format string may cause the date/time widget to initialize itself
   with an entirely new set of fields. 

See Also:
   PROP_FORMATSTRING
   IWidget
   
      

===========================================================================

IWIDGET_SetDelimiterString

Description:
   A convenience inline function for setting PROP_DELIMITER with IWIDGET_SetProperty().
   Sets the delimiter string.

Prototype:
   int IWIDGET_SetDelimiterString(IWidget *po, const AECHAR *pwDelim);

Parameters:
===pre>
   po:         [in] Pointer to the IWidget object
   pwDelim:    [in] Pointer to an AECHAR delimiter string.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget handled the PROP_DELIMITER property
   EFAILED - Otherwise.
===/pre>

Comments: 
   Since changing a delimiter string may cause the date/time widget to change 
   it's preferred size, it is recommended that IWIDGET_GetPreferredExtent() and 
   IWIDGET_SetExtent() be called on the widget after setting this property.  
   
Side Effects:
   The preferred extent may change if the delimiter string is changed.

See Also:
   PROP_DELIMITER
   IWidget
   
      

===========================================================================

IWIDGET_SetFieldPad

Description:
   A convenience inline function for setting PROP_FIELDPAD with IWIDGET_SetProperty().
   Sets the field padding value.

Prototype:
   int IWIDGET_SetFieldPad(IWidget *po, uint8 nPad);

Parameters:
===pre>
   po:         [in] Pointer to the IWidget object
   nPad;       [in] Field pad value.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget handled the PROP_FIELDPAD property
   EFAILED - Otherwise.
===/pre>

Comments:
   None
   
Side Effects:
   None

See Also:
   PROP_FIELDPAD
   IWidget


===========================================================================

IWIDGET_SetArrowPad

Description:
   A convenience inline function for setting PROP_ARROWPAD with IWIDGET_SetProperty().  
   Sets the arrow padding value.  

Prototype:
   int IWIDGET_SetArrowPad(IWidget *po, uint8 nPad);

Parameters:
===pre>
   po:         [in] Pointer to the IWidget object
   nPad        [in] Arrow pad value.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget handled the PROP_ARROWPAD property
   EFAILED - Otherwise.
===/pre>

Comments:
   None
   
Side Effects:
   None

See Also:
   PROP_ARROWPAD
   IWidget


===========================================================================

IWIDGET_GetFieldPad

Description:
   A convenience inline function for getting PROP_FIELDPAD with IWIDGET_GetProperty().  
   Gets the field padding value.  

Prototype:
   int IWIDGET_GetFieldPad(IWidget *po, uint8 *pnPad);

Parameters:
===pre>
   po:         [in]  Pointer to the IWidget object
   pnPad       [out] Address of destination variable which will receive field pad value.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget handled the PROP_FIELDPAD property
   EFAILED - Otherwise.
===/pre>

Comments:
   None
   
Side Effects:
   None

See Also
   PROP_FIELDPAD
   IWidget
   

===========================================================================

IWIDGET_GetArrowPad

Description:
   A convenience inline function for getting PROP_ARROWPAD with IWIDGET_GetProperty().  
   Gets the arrow padding value.  

Prototype:
   int IWIDGET_GetArrowPad(IWidget *po, uint8 *pnPad);

Parameters:
===pre>
   po:         [in]  Pointer to the IWidget object
   pnPad       [out] Address of destination variable which will receive arrow pad value.
===/pre>

Return Value:
===pre>
   SUCCESS - The widget handled the PROP_ARROWPAD property
   EFAILED - Otherwise.
===/pre>

Comments:
   None
   
Side Effects:
   None

See Also:
   PROP_ARROWPAD
   IWidget
   

===========================================================================

IWIDGET_SetJulianDateTime

Description:
   Sets the date/time value of the widget from a JulianType data structure.
   
   Natively, the value model of the date or time widget accepts an unsigned
   32-bit value representing date/time in seconds (from phone reference time).
   This function converts the JulianType date/time value to an unsigned 32-bit
   value and sets it in the value model of the widget.

   The value model of the julian date or julian time widget accepts a
   JulianType date structure.  This function makes a copy of the JulianType
   struct passed to it and sets it in the vlaue model.  The date and time
   specified in the JulianType structure is validated and adjusted if needed, 
   ensuring the julian date and time widgets display valid data.

Prototype:
   int IWIDGET_SetJulianDateTime(IWidget *po, JulianType *pJulian);

Parameters:
===pre>
   po:         [in] Pointer to the IWidget object
   pJulian:    [in] Pointer to a filled JulianType data structure
===/pre>

Return Value:
===pre>
   SUCCESS - The date/time value was set in the value model of the widget.
   EFAILED - The Set operation was not successful
===/pre>

Comments:
   None
   
Side Effects:
   None

See Also:
   IWIDGET_GetModel()
   IVALUEMODEL_SetValue()
   JULIANTOSECONDS()
   IWidget
   IValueModel
   JulianType

===========================================================================

IWIDGET_GetJulianDateTime
   Gets the date/time value of the widget from a JulianType data structure. 
   
   Natively, the value model of the date or time widget holds an unsigned
   32-bit value representing date/time in seconds (from phone reference time).
   This function converts the unsigned 32-bit integer value from the value model
   and converts it to a JulianType date/time value. 

   The value model of the julian date or julian time widget accepts a
   JulianType date structure.  The date stored within the value model is
   returned as is by this function.

Description:

Prototype:
   int IWIDGET_GetJulianDateTime(IWidget *po, JulianType *pJulian);

Parameters:
===pre>
   po:         [in]   Pointer to the IWidget object
   pJulian     [out]  Address of JulianType data structure that will receive the date/time value.
===/pre>

Return Value:
===pre>
   SUCCESS - The date/time value was received from the value model of the widget.
   EFAILED - The Get operation was not successful
===/pre>

Comments:
   None
   
Side Effects:
   None.

See Also:
   IWIDGET_GetModel()
   IVALUEMODEL_GetValue()
   GETJULIANDATE()
   IWidget
   IValueModel
   JulianType
      
===========================================================================
*/

#endif   //__AEEDATETIMEWIDGET_H__

