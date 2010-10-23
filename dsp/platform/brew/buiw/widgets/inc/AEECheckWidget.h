/*
  ========================================================================

  FILE:  AEECheckWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Check and radiobutton widgets

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEECHECKWIDGET_H__
#define __AEECHECKWIDGET_H__

#include "bid\AEECLSID_CHECKWIDGET.bid"
#include "bid\AEECLSID_RADIOWIDGET.bid"

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Checkbox Widget Class IDs

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   checkbox or radio button widget by creating a new instance of the Check Widget
   class, passing the desired class ID to ISHELL_CreateInstance().  The object that is
   created is, itself, a widget and may utilize those APIs defined for the IWidget
   interface.
   
   The class IDs used to identify the check widget are defined below.

Definition:

Identifier             Definition
----------             -----------------------------------
AEECLSID_CHECKWIDGET:  Identifies the class ID of the checkbox widget
AEECLSID_RADIOWIDGET:  Identifies the class ID of the radio button widget

Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Check Widget Class

Description:
   The check widget is derived from the widget object and comes in two flavors --
   checkboxes and radio buttons.

   A checkbox is used to indicate the on or off state of a given interface element.
   For example, a wireless dating application might use checkboxes to collect personal
   interests from prospective daters, each indicating whether or not the person is
   "into" the given interest, like so:

===pre>
      What types of things do you like to do on a
      date?  Select all that apply:
          [ ] Walks in the moonlight 
          [ ] Dinners by candlelight
          [x] Watch Monty Python movies
===/pre>

   Radio buttons allow the user to select one item from a group of choices.  For
   example, an application that determines whether or not you qualify for passage
   across a an old, decrepit and very scary bridge that crosses a murky ravine
   might, ask a series of multiple choice questions with answers organized as groups
   of radio buttons, like so:

===pre>
      What... is your quest?:
          (o) To seek the Holy Grail
          ( ) To get across this bridge
          ( ) To seek fire

      What... is your favorite color?:
          ( ) Blue
          ( ) Red
          (o) Blue!  No, yellow!  Aaaaaaaaaaaaaa!
===/pre>

   Checkboxes are drawn as a square box at the origin of the widget's bounding rectangle
   The appearance of the checkbox varies depending on the checkbox's value (checked or
   unchecked) and activation state (active or inactive).  
   
   Radio buttons are drawn as a circle at the origin of the widget's bounding rectangle. 
   Like checkboxes, the appearance of a radio button varies depending on the radio button's
   value (selected or unselected) and activation state (active or inactive). 
   
   The graphic images used to represent each of the states associated with a checkbox or
   radio button are stored as resources in the "widgets.mif" file.  One resource is used to
   represent various portions of the widget -- such as the active border, the inactive
   border, and the image used to indicate that the widget has been checked (or set).
   These separate images are stored as display "tiles", each of exactly the same size and
   laid out in the resource one after the other in a horizontal strip, sort of like the
   frames of an animation sequence.  The check and radio widgets overlay these images to
   create the effect of the widget in any possible state -- active or inactive, set or
   unset.  For example, to display an inactive checked checkbox, the widget would first
   display the tile containing the widget's inactive border, then overlay this image with
   the tile that contains the image containing an 'x' or a fancy illustrated checkmark),
   thus creating the appropriate image "on the fly".
   
   The resource and tile positions for each state of the check and radio widgets is as follows: 
   
===pre>
        Res ID      Widget         Tile    Image Represented
        ------      ------         ----    -----------------
          200       Checkbox         0     Checkmark, indicating the checked state
          200       Checkbox         1     Border, indicating the inactive state
          200       Checkbox         2     Border, indicating the active state
          
          201       Radio Button     0     Filled circle, indicating the set state
          201       Radio Button     1     Border, indicating the inactive state
          201       Radio Button     2     Border, indicating the active state
===/pre>

   Checkboxes and radio buttons are attached to value models, which store the widget's
   state (on or off) as a boolean value.  Applications can identify and assign a specific
   value model to each widget at the time it is created, or can choose to instead use a
   default IValueModel implementation managed by the check widget class.

   Radio buttons may additionally be attached to a model that will manage the association
   of a group of radio buttons -- though this model does not manage values.  Instead, the
   model functions as a notifier, sending EVT_MDL_GROUP_ITEMACTIVATE events to each listening
   radio button as the user changes the current selection.  The widgets in a radio button
   group should, therefore, be created to all share the same model.

   To create a checkbox or radio widget, an application would do the following:
===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_CHECKWIDGET to
          create a checkbox widget, or AEECLSID_RADIOWIDGET to create a radio button
          widget.  ISHELL_CreateInstance() will retrieve a reference counted instance
          of the check widget class.
       2. Call IWIDGET_SetModel() to attach a model to the widget, if the application
          wishes to override the default value model assigned to the widget at creation.
       3. Call the appropriate IWIDGET_Setxxxxxx() functions to override any of the
          default visual characteristics of the widget.  For example, if the
          newly created a button is to have a background color other than RGB_WHITE,
          the application would call IWIDGET_SetBGColor(), passing in the desired RGB
          color. 
===/pre>

Supported Events:
   Both checkboxes and radio buttons accept and handle events sent from other objects,
   however, their handling of events varies slightly, as discussed below.
   
   The check widget handles a very small set of events, as described below.

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETFOCUS:     The check widget responds to this event by attempting to set the
                      focus of the widget, as specified by the 'wParam' parameter.  When
                      'wParam' is TRUE, the widget is to be set to the active state,
                      otherwise the widget is inactive.  In each case, the widget's
                      bounding rectangle is invalidated, triggering the widget to be
                      redrawn using its new focus state.

EVT_KEY:              The check widget will respond to presses of AVK_SELECT by attempting
                      to changing the value of the boolean data stored in its model to its
                      opposite sense -- i.e. if the value was TRUE, it is set to FALSE, and
                      if it was FALSE it is set to TRUE.  Changing the value model associated
                      with a check widget will ultimately send a model change notification
                      back to the check widget causing the widget to invalidate its
                      bounding rectangle and, therefore, redraw itself using its new value.
===/pre>   
   
   The radio widget handles a slightly larger set of events than the check widget, as
   described below.

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETFOCUS:     The radio widget responds to this event by attempting to set the
                      focus of the widget, as specified by the 'wParam' parameter.  When
                      'wParam' is TRUE, the widget is to be set to the active state,
                      otherwise the widget is inactive.  In each case, the widget's
                      bounding rectangle is invalidated, triggering the widget to be
                      redrawn using its new focus state.

EVT_KEY:              The radio widget responds to presses of AVK_SELECT only if the radio
                      button's value model currently indicates that the radio button is
                      off (i.e. not set).  Pressing AVK_SELECT then has the effect of
                      setting that radio button by changing the value of the data stored
                      in the button's model to TRUE.  Changing the value model associated
                      with a radio widget will ultimately send a model change notification
                      back to the radio widget causing the widget to invalidate its
                      bounding rectangle and, therefore, redraw itself using its new (set)
                      value.
                      
                      Once set, the radio widget will also notify the other widgets in its
                      radio group of the value change so that they may correspondingly set
                      themselves to FALSE, and redraw themselves in their off state.
                      
                      The radio widget will return TRUE when AVK_SELECT is pressed,
                      regardless of whether or not the widget's value was actually changed
                      (for example, if the radio widget was already set when AVK_SELECT is
                      pressed).

EVT_WDG_SETPROPERTY:  The check widget responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The check widget
                      recognizes those properties defined in the "Properties" section, below.

EVT_WDG_GETPROPERTY:  The check widget responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter.  The check widget
                      recognizes those properties defined in the "Properties" section, below.
===/pre>   

Properties:
Property              Description
--------              ------------------------------------------------------------
PROP_VIEWMODEL:       This property contains a pointer to the view model used to manage a
                      group of radio buttons.  An application would set this property for
                      each widget in a radio group, thereby registering each of these widgets
                      as a listener for selection changes in the radio group.  Whenever one
                      of the widgets in a radio group is set to the ON state, each of the
                      other widgets in that group will be turned OFF.
                             
                      Property Value:  IModel *
                      
Required Model:
   IValueModel
   
Model Data:
===pre>   
   boolean modelData;
===/pre>
   
   The checkbox and radio button widgets rely on a value model that represents its data as
   boolean value reflecting the current set/unset state of the widget.  The model should
   always return this boolean value in response to data queries from a client widget.
   
   A default model is created and assigned to the widget when the widget is created with 
   ISHELL_CreateInstance(). The client can replace the widget's model at any time with 
   the method: IWIDGET_SetModel(), as long as the supplied model interface is of the type 
   that the widget expects.   
   
   The data supplied in the value model must be a pointer to type boolean.
   
   
Comments:
   It is very important to note that the text that usually accompanies a checkbox or radio
   button is NOT part of the checkbox or radio widget.  Applications must separately create
   a static text widget and position that widget alongside the checkbox or radio button in
   order to create the appearance of a button followed by text.  For example, an application
   may choose to create a container, then create a checkbox and static text widget within
   that container to group the objects together.
   
See Also:
   Model Events
   ISHELL_CreateInstance()

*/

#endif /* __AEECHECKWIDGET_H__ */
