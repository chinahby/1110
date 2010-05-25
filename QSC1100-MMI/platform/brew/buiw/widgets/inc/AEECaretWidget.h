/*
  ========================================================================

  FILE:  AEECaretWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I display a caret inside other widgets such as a
  text widget.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEECARETWIDGET_H__
#define __AEECARETWIDGET_H__

#include "AEEWProperties.h"


#include "bid/AEECLSID_CARETWIDGET.bid"

typedef struct {
   int nMin;
   int msTimeOn;
   int msFadeTime;
   int nFadeSteps;
} CaretPulse;


static __inline int IWIDGET_SetCaretPulse(IWidget *po, CaretPulse *pcp) {
   return IWIDGET_SetProperty(po, PROP_CARETPULSE, (uint32)pcp);
}


/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Caret Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates a
   caret widget by creating a new instance of the Caret Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the caret widget is defined below.

Definition:

Identifier            Definition
----------            -----------------------------------
AEECLSID_CARETWIDGET: Identifies the class ID of the caret widget
   
Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

Caret Widget Class

Description:
   The caret widget is used to display a text marker inside other widgets to
   indicate where the next character will be inserted.  It is most commonly
   used by the text widget, for instance.  The caret widget exposes properties
   for setting its color and pulse rate (i.e. blinking caret).  An OEM may
   further customize the appearance of all carets using the AEECLSID_CARETWIDGET
   class ID.  Therefore, any widget that wishes to contain a caret widget should
   first try to instantiate it using a call to ISHELL_CreateInstance() with the
   class ID of AEECLSID_CARETWIDGET.  If that fails, then create the default caret
   with CaretWidget_New().

Events:
   The caret widget supports the following events:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  Set property identified by one of the following 'wParam' parameters:
                      
                         PROP_FGCOLOR     --  Sets the color of the caret.
                         PROP_BGCOLOR     --  Sets the background color (used for pulsing)
                         PROP_CARETPULSE  --  Sets the pulse properties.  wParam is pointer
                                              to a CaretPulse struct (see Properties below).
===/pre>
              
Properties:
   The caret widget supports the following properties:

===pre>
Property              Description
--------              ------------------------------------------------------------
PROP_FGCOLOR:         This property contains the color of the caret widget.

                          Property Value:  RGBVal

PROP_BGCOLOR:         This property contains the color of the background, typically
                      the same background color of the containing widget.  It is
                      the color the caret transitions to when pulsing.

                          Property Value:  RGBVal

PROP_CARETPULSE:      This property contains the pulse attributes as defined by the
                      following struct.

                          struct CaretPulse {
                              int nMin;         //  Frequency of pulse
                              int msTimeOn;     //  Time caret is visible (ms)
                              int msFadeTime;   //  Time to transition to non-visible (ms)
                              int nFadeSteps;   //  Number of transitions
                          };        
===/pre>     

See Also:
   ISHELL_CreateInstance()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  IWidget

===========================================================================

IWIDGET_SetCaretPulse

Description:
   Sets the pulse attributes of a caret widget.
   
Prototype:
   int IWIDGET_SetCaretPulse(IWidget *po, CaretPulse *pcp);
   
Parameters:
===pre>   
   po:        Pointer to the IWidget object.
   pcp:       Pointer to a CaretPulse struct.
===/pre>

Return Value:
===pre>   
   SUCCESS - If the caret widget is able to successfully set the CaretPulse.
   EFAILED - If the CaretPulse could not be set.
===/pre>   
   
Comments:
   See PROP_CARETPULSE for definition of the CaretPulse struct.

Side Effects: 
   None

See Also: 
   IWidget
*/

#endif      /* __AEECARETWIDGET_H__ */