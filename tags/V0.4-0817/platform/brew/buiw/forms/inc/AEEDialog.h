/*
  ========================================================================

  FILE:  AEEDialog.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Standard dialogs 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEDIALOG_H__
#define __AEEDIALOG_H__

#include "bid/AEECLSID_DIALOGFORM.bid"
#include "bid/AEECLSID_PROGRESSDIALOG.bid"
#include "bid/AEECLSID_INFODIALOG.bid"
#include "bid/AEECLSID_ERRORDIALOG.bid"
#include "bid/AEECLSID_WARNDIALOG.bid"

#define THEME_INFO_BASENAME      "InfoDialog"
#define THEME_ERROR_BASENAME     "ErrorDialog"
#define THEME_WARN_BASENAME      "WarnDialog"

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Dialog Form Class IDs

Description:
   Each object in the forms interface is identified by a unique interface ID.  These
   IDs are used throughout the widget and forms framework to distinguish between the
   various object and interface types, such as forms, widgets and containers.
   An application creates a dialog form by creating a new instance of the Dialog Form
   class, passing the ID of the class to ISHELL_CreateInstance().  The object that is
   created is, itself, a form and may utilize those APIs defined for the IForm interface.
   
   The class IDs used to identify the dialog form are defined below.

Definition:

Identifier                Definition
----------                -----------------------------------
AEECLSID_DIALOGFORM:      Identifies the class ID of the dialog form
AEECLSID_PROGRESSDIALOG:  Identifies the class ID of the progress dialog
AEECLSID_INFODIALOG:      Identifies the class ID of the info message dialog
AEECLSID_ERRORDIALOG:     Identifies the class ID of the error message dialog
AEECLSID_WARNDIALOG:      Identifies the class ID of the warning message dialog
   
Comments:
   None

See Also:
   IForm Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Dialog Class

Description:
   The dialog class is derived from the base popup form interface and is intended to
   provide applications with a simple mechanism for displaying quick messages or progress
   information in a standard and consistent manner.  For example, an application that wishes
   to alert the user of some meaningful event (Mail has arrived!  You just won the Lotto!
   JLo and Ben call off the wedding!) could choose to display an information dialog, which
   could be unique in appearance from an error or warning dialog, based on how each of those
   dialog types are defined in the "forms.mif" theme file.  Likewise, an application could
   display a progress dialog to let the user know that some operation will take time to be
   completed.  For example, when downloading a new ring tone, or attempting to connect to
   a remote server, the application could choose to display an animation of three obnoxious
   pink dancing bears doing the Macarena.
   
   Dialogs are a specialized type of form and, as such, support each of the user interface
   elements commonly found within the context of a form -- a title area, a content area, and
   a pair of softkeys.  The dialog class expands on these common UI elements by providing
   additional widget and form capabilities.  Notably, a dialog may possess it's own background,
   backdrop and title widgets -- all independent of widgets managed by the root form.  The
   title and background widget allows a dialog to be displayed with a title and background that
   is different than that displayed on the form the dialog will overlay.  The backdrop widget
   is used by the dialog to create the visual effect of the dialog floating over the top of
   the display's topmost form.
   
   Though the dialog class is simple, applications may use the dialog title and the root
   form's softkeys to guide quick interactions with the user.  Take, for example, the following
   example of a dialog from an application used to purchase tickets to rock concerts:
   
===pre>   
       +-----------------------------------+
       |         Purchase Tickets          |
       +-----------------------------------+
       |                                   |
       |                                   |
       |       A service charge of         |
       |                                   |
       |          $4,000,000               |
       |                                   |
       |   will be charged to your credit  | 
       |   card.                           |
       |                                   |
       |     Do you approve this charge?   |
       |                                   |
       |                                   |
       +-----------------------------------+
       |     Approve     |     Cancel      |
       +-----------------------------------+
===/pre>
   
   This dialog includes a title, content text and a pair of softkeys.  The application could
   attach an event handler to the dialog to process the user's selection of "Approve" or
   "Cancel" from the softkeys, thereby tying together the form content and actions taken on
   behalf of the user into one nice, neat package.
   
   The base dialog form is automatically sized to fit, centered, within the form's root
   container.  The dialog will be displayed with a transparent backdrop to create the effect
   of the dialog floating above other forms on the display.  Specific subclasses of the base
   dialog may size, position and alter the default characteristics of the base dialog in any
   manner that they so choose.

   The base dialog form is created with the following default characteristics:
   
===pre>
                    Position:  Centered within root container
                Border width:  0
                      Shadow:  1 pixel
            Background color:  RGB_WHITE
            Foreground Color:  RGB_BLACK
   Selected Background Color:  Light grey
  Background image alignment:  Bottom right (if a background image is present)
       Backdrop Border width:  0
   Backdrop Background color:  RGB_WHITE
       Backdrop Transparency:  70 (roughly 30% transparent, 70% opaque)
          Title border width:  1 pixel
          Title border color:  Dark grey
               Title padding:  4 pixels on the left   
===/pre>

   To create a base dialog, an application would do the following:

===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_DIALOGFORM to
          retrieve a reference counted instance of the progress dialog form.
       2. Set the dialog's FID_TITLE property to initialize the text to be displayed in the
          dialog title widget.
       3. Set the dialog's FID_TEXT property to initialize the text to be displayed in the content
          area of the dialog.
       4. If the dialog is to support softkeys, set the dialog's FID_SOFTKEY1 and FID_SOFTKEY2
          properties to initialize the softkey text.
       5. Call IFORM_SetHandler() on the dialog to hook an event handler to process user actions.
===/pre>

   Like popup menus, dialogs are wrapped within a fader widget to provide an effect of the
   dialog gradually emerging onto the display, then gradually fading from the display when
   it is removed.
   
   After a dialog is created, the default characteristics listed above, or any other
   characteristic native to dialogs, popups or forms may be overridden.  For  example, the
   dialog could choose to display its message in white text over an image of the beautiful
   green grass in the outfield at Petco Park.

   Once created, the dialog will remain on screen indefinitely -- or until the user explicitly
   dismisses the dialog by pressing any key mapped to an EVT_FORM_CANCEL event.  An application
   may implicitly remove the dialog by sending an EVT_FORM_CANCEL event to the dialog itself.
   For example, if an application wishes to display a dialog to provide visual feedback while a
   lengthy operation is in progress ("Connecting..."), then automatically dismisses the dialog
   once the operation completes.  Such an application would initiate an EVT_FORM_CANCEL event
   upon completing its time consuming task.  As described below under Supported Events, dialogs
   respond to this event by initiating its fade effect, then automatically remove themselves
   from the form stack.

Supported Events:
   The base dialog processes events in two passes.  In the first pass the dialog will attempt
   to process events itself -- providing a mechanisms for handling dialog cancellation and
   property event handling.  Events not handled during the first pass are then passed on to
   the base popup event handler.
  
   The base dialog form handles the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_FORM_CANCEL:      The dialog is being instructed to remove itself from the form stack
                      and will comply by initiating its fade out transition.  Dialogs fade
                      for a period of 200 ms prior to being popped off the form stack.
                      This time period is not currently configurable.
                      Though EVT_FORM_CANCEL is handled by the base dialog event handler,
                      the event will still be passed down to the popup event handler for
                      further processing.
                      
EVT_WDG_SETPROPERTY:  The dialog form responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The dialog form
                      allows the following properties to be set:
                      
                          FID_ACTIVE     --  Sets the activation state of the dialog.
                          FID_VISIBLE    --  Sets the visible state of the dialog
                          FID_TITLE      --  Sets the text to be displayed in the dialog's
                                             title widget.  This event is then passed to the
                                             base form, which then notifies other objects that
                                             the title widget has changed.
                          FID_THEME      --  Sets the theme file to be used by this dialog,
                                             applying the metrics of that theme to be applied to
                                             the form elements and contents.
                          FID_BACKGROUND --  Sets the image to be displayed in the dialog's
                                             background image.
                          WID_BACKDROP   --  Sets the widget used to render the dialog's backdrop
                                             effect.
                      
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The dialog form responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter, passing the value of
                      the property back to the querying object in a pointer passed in the
                      'dwParam' parameter.  The dialog form allows the following properties
                      to be retrieved: 
                      
                          WID_TITLE     --  Retrieves a pointer to the dialog title widget.
                                            Retrieving this property increments the reference 
                                            count of the title widget.
                          WID_BACKDROP  --  Retrieves a pointer to the widget used by dialog
                                            to render its backdrop effect.  Retrieving this
                                            property increments the reference count of the
                                            backdrop widget.
                      
                      These properties are discussed below in greater detail.
===/pre>
   
Properties:
Property         Description
--------         ------------------------------------------------------------
FID_ACTIVE:      This property contains the activation state of the dialog -- TRUE when the
                 dialog is active, FALSE when it is not.  Setting the activation state of
                 the dialog will trigger the dialog to invalidate itself and be redrawn. 
                             
                     Property Value:  boolean

FID_VISIBLE:     This property contains the visible state of the dialog -- TRUE when the
                 dialog may be all or partially visible, FALSE when it is not. Setting the 
                 visible state of the dialog may trigger an invalidation if the root form 
                 client region has changed.
                             
                     Property Value:  boolean

FID_ROOT:        This property contains a pointer to the root form that manages the dialog.
                 The root form controls the stacking order for a collection of forms and
                 manages where the dialog will be placed relative to other forms.
                             
                     Property Value:  IRootForm *

FID_TITLE:       This property contains a pointer to the text that will be displayed in the
                 dialog's title widget (which is different than the root form's title widget).
                 The text will be displayed by itself if the dialog title widget has been
                 implemented as a static widget, or along with a graphic image if the widget
                 is an image static widget.  Though the value of the property is a pointer to
                 an AECHAR, the property value must be set via a pointer to a FormRes data
                 structure, which identifies the property value as either a resource file
                 reference or as a direct pointer to the string to be displayed by the title
                 widget.
                             
                     Property Value:  AECHAR *

FID_THEME:       This property identifies the name of the theme file to be used by a particular
                 dialog.  Upon setting the theme file, BREW will load and apply the theme to the
                 dialog.
                             
                     Property Value:  char *


FID_BACKGROUND:  This property contains a pointer to the image that will be displayed in the
                 dialog's background widget (which is different than the root form's background
                 widget).  Though the value of the property is a pointer to an IImage, the
                 property value must be set via a pointer to a FormRes data structure identifying
                 the property value as either a resource file reference or as a direct pointer to
                 the IImage to be displayed by the background widget.
                             
                     Property Value:  IImage *

WID_BACKDROP:    This property contains a pointer to the widget used to render the backdrop for
                 the dialog.  The backdrop provides a visual effect for the dialog, which makes
                 it seem as though the dialog has been placed "above" other content on the
                 display.  The WID_BACKDROP property allows applications to access the backdrop
                 widget that provides this visual feedback.  It is important to note that dialogs
                 supply a backdrop widget in _addition_ to the primary widget provided by the base
                 form and accessed through the WID_FORM property.  The backdrop widget is also
                 distinguished from the background widget supplied by the root form and accessed
                 by the WID_BACKGROUND property.
                             
                     Property Value:  IWidget *
                                    
WID_TITLE:       This property contains a pointer to the dialog's title widget --  which is not
                 the same as the root form's title widget.  Generally, an application will choose
                 to implement the title widget as a static, image or image static widget to
                 produce the root form's title banner.  An application could,  however, implement
                 the title widget using any widget of their choosing.
                           
                     Property Value:  IWidget *

WID_FORM:        This property contains a pointer to the dialog's primary widget, which by default
                 is an instance of TitleWidget (AEECLSID_TITLEWIDGET). If this widget is replaced, 
                 it should maintain the properties of a TitleWidget, responding to PROP_TITLEWIDGET 
                 which a dialog uses to set title properties.
                 
                     Property Value:  IWidget *

Comments:
   None
   
See Also:
   ISHELL_CreateInstance()
   IFORM_SetHandler()

===========================================================================

Static Dialog Class

Description:
   Information, error and warning dialogs together are examples of a particular type of dialog
   form -- the static dialog -- which is intended to display a dialog containing a static text
   message.  The three types of static dialogs (info, error and warning) differ from one another
   only in theme, with each type residing at a different base location within the theme file
   (see AEEThemeUtil.h for additional information about themes and theme bases).

   Static dialogs inherit each of the UI elements provided by the base dialog including the
   dialog title, dialog background and backdrop elements.  Additionally, the static dialog
   provides a static widget that will be used to display the text of the information, error
   or warning message appearing in the body of the dialog.
      
   The static dialog form inherits all of the default characteristics of the base dialog form,
   while overriding or adding the following additional default characteristics:
   
===pre>
   Preferred vertical Extent:  70% of the height of the root form's client rectangle
 Preferred horizontal Extent:  80% of the width of the root form's client rectangle
      Message text alignment:  SWF_WRAPTEXT
                               IDF_ALIGN_LEFT
                               IDF_ALIGN_MIDDLE
                               SWF_NOSHORTENTEXT
                Text padding:  4 pixels on the left
                               4 pixels on the right
       Text foreground color:  RGB_BLACK
       Text background color:  RGB_WHITE
           Text border width:  0
===/pre>

   To create a static dialog, an application would do the following:

===pre>
       1. Call ISHELL_CreateInstance() with a class ID identifying the type of dialog to
          be created:
          
               AEECLSID_INFODIALOG   :  To create a dialog that utilizes the info dialog theme
               AEECLSID_ERRORDIALOG  :  To create a dialog that utilizes the error dialog theme
               AEECLSID_WARNDIALOG   :  To create a dialog that utilizes the warning dialog theme
       
          ISHELL_CreateInstance() will retrieve a reference counted instance of the appropriate
          dialog form.
       2. Set the dialog's FID_TEXT property to initialize the text to be displayed in the content
          area of the dialog.
       3. Set the dialog's FID_TITLE property to initialize the text to be displayed in the
          dialog title widget.
       4. If the dialog is to support softkeys, set the dialog's FID_SOFTKEY1 and FID_SOFTKEY2
          properties to initialize the softkey text.
       5. Call IFORM_SetHandler() on the dialog to hook an event handler to process user actions.
===/pre>

Supported Events:
   The static dialog processes events in two passes.  In the first pass the static dialog will
   attempt to process property events itself.  Events not handled during the first pass are then
   passed on to the base dialog event handler.
  
   The static dialog form handles the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The static dialog responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The static dialog form
                      allows the following properties to be set:
                      
                         FID_ACTIVE   --  Sets the activation state of the dialog.
                         FID_VISIBLE  --  Sets the visible state of the dialog
                         FID_TEXT     --  Sets the text to be displayed in the content area
                                          of the dialog.
                      
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The static dialog responds to this event by attempting to retrieve the
                      property identified by the 'wParam' parameter, passing the value of
                      the property back to the querying object in a pointer passed in the
                      'dwParam' parameter.  The static dialog form allows the following
                      properties to be retrieved: 
                      
                          WID_STATIC  --  Retrieves a pointer to the widget used to display the
                                          dialog's info, error or warning text.  Retrieving this
                                          property increments the reference count of the static
                                          widget.
                       
                      These properties are discussed below in greater detail.
===/pre>
   
Properties:
Property     Description
--------     ------------------------------------------------------------
FID_ACTIVE:  This property contains the activation state of the static dialog -- TRUE when
             the dialog is active, FALSE when it is not.  Setting the activation state of
             the static dialog will trigger the dialog to invalidate itself and be redrawn. 
                             
                 Property Value:  boolean

FID_TEXT:    This property is used as a trigger to set the text of the information, error or
             warning message to be displayed within the content area of the dialog, retrieving
             that text from a resource file.  This property does not contain any retrievable
             value and therefore may not be accessed by EVT_WDG_GETPROPERTY.  Rather, the value
             is set by identifying the resource file and resource ID that contains the text to be
             displayed by the dialog's static text object.  This information is passed to the
             event handler in an EVT_WDG_SETPROPERTY event, with 'dwParam' containing a pointer
             to a FormRes data structure.  The FormRes data structure identifies both the resource
             file name and resource ID of the text.  For example, a static dialog may change the
             text of an error message by setting the FID_TEXT property, identifying that the text
             is contained in resource 100 of a file named "error.txt".
                           
                 Property Value:  FormRes *
                                    
WID_STATIC:  This property contains a pointer to the static widget used to display the info,
             error or message text of a static dialog.  It is important to note that the static
             static widget is provided by the static dialog form in _addition_ to the form's
             primary widget accessed by the WID_FORM property.
                             
                 Property Value:  IWidget *

Comments:
   None
   
See Also:
   ISHELL_CreateInstance()
   IFORM_SetHandler()

===========================================================================

Progress Dialog Class

Description:
   The progress dialog provides applications with a simple interface for creating forms that
   track and manage determinant and indeterminant tasks.  For example, while downloading a
   short video clip, an application might want to display a dialog containing a progress bar
   that slowly filled up as more and more of the clip was loaded into the device.  Another
   application that wishes to let the user know that a time consuming task was underway (trying
   to connect to a stubborn server, for example) might simply display a short message such as
   "Connecting...".  Each of these scenarios could manage the interface to the user via a
   progress dialog -- perhaps using one of the root form's softkeys to provide a "Cancel"
   operation.

   Progress dialogs inherit each of the UI elements provided by the static and base dialog
   forms including the dialog title, background and backdrop elements from the base dialog,
   as well as the static widget inherited from the static dialog.  Additionally, the progress
   dialog provides a progress widget that may be used to display the ongoing status the
   operation the dialog is meant to track.
   
   To create a progress dialog, an application would do the following:

===pre>
       1. Call ISHELL_CreateInstance() with a class ID of AEECLSID_PROGRESSDIALOG to
          retrieve a reference counted instance of the progress dialog form.
       2. Set the dialog's FID_TEXT property to initialize the text to be displayed in the content
          area of the dialog.
       3. Set the dialog's FID_TITLE property to initialize the text to be displayed in the
          dialog title widget.
       4. Retrieve and manage the dialog's FID_PROGRESS property to gain access to and establish
          the dialog's progress widget.
       5. If the dialog is to support softkeys, set the dialog's FID_SOFTKEY1 and FID_SOFTKEY2
          properties to initialize the softkey text.
       6. Call IFORM_SetHandler() on the dialog to hook an event handler to process user actions.
===/pre>

Supported Events:
   The progress dialog processes events in two passes.  In the first pass the dialog will attempt
   to process property events itself -- primarily managing interactions with the dialog's progress
   widget.  Events not handled during the first pass are then passed on to the static dialog event
   handler.
  
   The progress dialog form handles the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The progress dialog form responds to this event by attempting to set the
                      property identified by the 'wParam' parameter.  The progress dialog form
                      allows the following properties to be set:
                      
                          FID_PROGRESS  --  Updates the progress value for the progress widget.
                      
                      These properties are discussed below in greater detail.

EVT_WDG_GETPROPERTY:  The progress dialog form responds to this event by attempting to retrieve
                      the property identified by the 'wParam' parameter, passing the value of
                      the property back to the querying object in a pointer passed in the
                      'dwParam' parameter.  The progress dialog form allows the following
                      properties to be retrieved: 
                      
                          WID_PROGRESS  --  Retrieves a pointer to the dialog's progress widget. 
                                            Retrieving this property increments the reference
                                            count of the progress widget.
                       
                      These properties are discussed below in greater detail.
===/pre>
   
Properties:
Property       Description
--------       ------------------------------------------------------------
FID_PROGRESS:  This property is used as a trigger to set the current progress value of a form.
               This property updates the value model associated with a form to the value passed
               in 'dwParam' and does not -- itself -- contain any retrievable value.
                             
                   Property Value:  Depends on the value model used by the progress indicator,
                                    though this value usually represents "percent complete".  For
                                    example, hen the value of this property is 80, the standard
                                    progress widget could be redrawn as 80% filled.
                                    
WID_PROGRESS:  This property contains a pointer to the dialog's progress widget.  It is important
               to note that the progress widget is provided in  _addition_ to the form's primary
               widget accessed by the WID_FORM property.
                             
                   Property Value:  IWidget *

Comments:
   None
   
See Also:
   ISHELL_CreateInstance()
   IFORM_SetHandler()
*/


#endif /* __AEEDIALOG_H__ */
