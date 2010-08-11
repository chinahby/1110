/*
  ========================================================================

  FILE:  AEEHtmlWidget.h
  
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
#ifndef __AEEHTMLWIDGET_H__
#define __AEEHTMLWIDGET_H__

#include "AEECLSID_HTMLWIDGET.BID"

#include "AEEWidget.h"
#include "AEEWProperties.h"
#include "AEEWeb.h"
#include "AEEDocModel.h"
#include "AEEHtmlViewModel.h"


// IHtmlViewer property flags
#define HWF_LOADALLIMAGES     0x00000001     // images will be loaded even if width and height
                                             // are not specified or if images are large

//#define HWF_INLINE_EDIT       0x00000002
//#define HWF_INVALIDATION      0x00000004     // expose invalidation behaviour


// HTML widget properties
#define PROP_HTMLW_URL				(PROP_SPECIAL + 0)  // base URL for link jumps and submit
#define PROP_HTMLW_IWEB				(PROP_SPECIAL + 1)  // IWeb interface widget uses to obtain sub-objects
#define PROP_HTMLW_INDENT			(PROP_SPECIAL + 2)  // basic indent
#define PROP_HTMLW_VSPACE			(PROP_SPECIAL + 3)  // basic paragraph spacing
#define PROP_HTMLW_BUFFERSIZE		(PROP_SPECIAL + 4)  // set/get size of internal buffer
#define PROP_HTMLW_FOCUSPOS			(PROP_SPECIAL + 5)  // set/get current focus position
#define PROP_HTMLW_SCROLLPOS		(PROP_SPECIAL + 6)  // set/get current scroll position
#define PROP_HTMLW_FONT_NORMAL		(PROP_SPECIAL + 7)  // the font for normal text
#define PROP_HTMLW_FONT_BOLD		(PROP_SPECIAL + 8)  // the font for bold text
#define PROP_HTMLW_FONT_LARGE		(PROP_SPECIAL + 9)  // the font for large text
#define PROP_HTMLW_LINK_COLOR		(PROP_SPECIAL + 10)	// active link foreground color
#define PROP_HTMLW_DOC_BGCOLOR   (PROP_SPECIAL + 11) // set html document background color
#define PROP_HTMLW_SCROLLMLLINK  (PROP_SPECIAL + 12) // scroll multi line link
#define PROP_HTMLW_SELECTPOS		(PROP_SPECIAL + 13) // set select position (this does not mean focus)

#define HTMLW_SCROLL_TOP      0
#define HTMLW_SCROLL_END      -1


static __inline int IWIDGET_SetURL(IWidget *po, const char *pszURL) {
   return IWIDGET_SetProperty(po, PROP_HTMLW_URL, (uint32)pszURL);
}

static __inline int IWIDGET_SetIWeb(IWidget *po, IWeb *piWeb) {
   return IWIDGET_SetProperty(po, PROP_HTMLW_IWEB, (uint32)piWeb);
}

static __inline int IWIDGET_SetIndent(IWidget *po, int nIndent) {
   return IWIDGET_SetProperty(po, PROP_HTMLW_INDENT, (uint32)nIndent);
}

static __inline int IWIDGET_GetIndent(IWidget *po, int *pnIndent) {
   return IWIDGET_GetPropInt(po, PROP_HTMLW_INDENT, pnIndent);
}

static __inline int IWIDGET_SetVSpace(IWidget *po, int nVSpace) {
   return IWIDGET_SetProperty(po, PROP_HTMLW_VSPACE, (uint32)nVSpace);
}

static __inline int IWIDGET_GetVSpace(IWidget *po, int *pnVSpace) {
   return IWIDGET_GetPropInt(po, PROP_HTMLW_VSPACE, pnVSpace);
}

static __inline int IWIDGET_SetBufferSize(IWidget *po, uint32 dwSize) {
   return IWIDGET_SetProperty(po, PROP_HTMLW_BUFFERSIZE, dwSize);
}

static __inline int IWIDGET_GetBufferSize(IWidget *po, uint32 *pdwSize) {
   return IWIDGET_GetProperty(po, PROP_HTMLW_BUFFERSIZE, pdwSize);
}

static __inline int IWIDGET_SetFocusPos(IWidget *po, int32 nPos) {
   return IWIDGET_SetProperty(po, PROP_HTMLW_FOCUSPOS, (uint32)nPos);
}

static __inline int IWIDGET_GetFocusPos(IWidget *po, int32 *pnPos) {
   return IWIDGET_GetProperty(po, PROP_HTMLW_FOCUSPOS, (uint32*)pnPos);
}


static __inline int IWIDGET_SetScrollPos(IWidget *po, int32 nPos) {
   return IWIDGET_SetProperty(po, PROP_HTMLW_SCROLLPOS, (uint32)nPos);
}

static __inline int IWIDGET_GetScrollPos(IWidget *po, int32 *pnPos) {
   return IWIDGET_GetProperty(po, PROP_HTMLW_SCROLLPOS, (uint32*)pnPos);
}

static __inline int IWIDGET_SetFontNormal(IWidget *po, IFont *piFont) {
   return IWIDGET_SetProperty(po, PROP_HTMLW_FONT_NORMAL, (uint32)piFont);
}

static __inline int IWIDGET_SetFontBold(IWidget *po, IFont *piFont) {
   return IWIDGET_SetProperty(po, PROP_HTMLW_FONT_BOLD, (uint32)piFont);
}

static __inline int IWIDGET_SetFontLarge(IWidget *po, IFont *piFont) {
   return IWIDGET_SetProperty(po, PROP_HTMLW_FONT_LARGE, (uint32)piFont);
}
/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
HTML Widget Class ID

Description:
   Each class and interface within the widget interface is identified by a unique ID.
   These IDs are used throughout the framework to distinguish between various types of
   user interface objects, such as containers and widgets.  An application creates an
   html widget by creating a new instance of the HTML Widget class, passing the ID
   of the class to ISHELL_CreateInstance().  The object that is created is, itself, a
   widget and may utilize those APIs defined for the IWidget interface.
   
   The class ID used to identify the HTML widget is defined below.

Definition:

Identifier            Definition
----------            -----------------------------------
AEECLSID_WIDGET:      Identifies the class ID of the softkey widget
   
Comments:
   None

See Also:
   IWidget Interface
   ISHELL_CreateInstance()

===========================================================================
 CLASS DOCUMENTATION
===========================================================================

HTML Widget Class

Description:
   The HTML widget is a user interface element that provides HTML display facilities to the
   widget framework.  It supports a subset of HTML version 3.2.   
   
     
   Its HTML formatting features and  its ability to deal with malformed content are limited.
   HTML presented to the widget should be properly constructed, with start and end tags for 
   nonempty elements and no mismatched end tags.  As a result, HTML that is 
   tested with the particular application is advised, and it is not intended 
   to support for arbitrary content (as obtained from various web sites).  


   Notable HTML features supported include:

   - Hypertext links 

   - Simple HTML FORM support: text input, lists, submit buttons, check 
   boxes, and radio buttons 

   - Inline images or plug-ins 

   - Horizontal alignments: left, center, right 

   - Bold text (displayed using the AEE_FONT_BOLD font) 

   - Large text (displayed using the AEE_FONT_LARGE font) 

   - Text colors 
  
   - Html document background colors when specified in hex format

   - Link colors using body tag


   Notable HTML features that are not supported in the current version of BREW include:

   - Java applets 

   - FRAMESET 

   - Scripting 

   - Cascading Style Sheets 

   - Unicode character references 

HTML Widget Properties:

   - PROP_HTMLW_URL: base URL for link jumps and submit

   - PROP_HTMLW_IWEB: IWeb interface widget uses to obtain sub-objects

   - PROP_HTMLW_INDENT: basic indent

   - PROP_HTMLW_VSPACE: basic paragraph spacing

   - PROP_HTMLW_BUFFERSIZE: size of internal buffer

   - PROP_HTMLW_FOCUSPOS: current focus position

   - PROP_HTMLW_SCROLLPOS: current scroll position

   - PROP_HTMLW_LINK_COLOR: Get/Set active link foreground color

   - PROP_HTMLW_DOC_BGCOLOR: Get/Set html document background color

   - PROP_HTMLW_SCROLLMLLINK: When set to FALSE, multiline links are jumped as if 
                              they are single line link. When TRUE, each line in 
                              the link is scrolled one line at a time

===========================================================================
 INTERFACE DOCUMENTATION
===========================================================================

IWidget Interface

Description:
   Several IWidget APIs are provided by HTMLWidget in addition to the native
   APIs provided by Widgets.


=============================================================================

IWIDGET_SetURL()


Description:

   This function specifies the URL address of the document that is to be 
   loaded into the viewer.  This should be called before HTML is loaded into 
   a document.  


Prototype:
   int IWIDGET_SetURL(IWidget *po, const char *pszURL);

Parameters:
===pre>
   po:      Pointer to the IWidget Interface Object
   pszURL:  Base URL string.
===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to set the requested property.
   EFAILED - If the property could not be set.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None


=============================================================================

IWIDGET_SetIWeb()


Description:
   This function sets the IWeb interface with which the viewer will 
   use to obtain sub-objects

Prototype:
   int IWIDGET_SetIWeb(IWidget *po, IWeb *piWeb);

Parameters:
===pre>
   po:      Pointer to the IWidget Interface Object
   piWeb:   IWeb interface pointer

===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to set the requested property.
   EFAILED - If the property could not be set.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None


=============================================================================

IWIDGET_SetIndent()


Description:
   This function sets the basic indentation of the HTML Widget object

Prototype:
   int IWIDGET_SetIndent(IWidget *po, int nIndent);

Parameters:
===pre>
   po:      Pointer to the IWidget Interface Object
   nIndent: Integer amount of indentation

===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to set the requested property.
   EFAILED - If the property could not be set.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None


=============================================================================

IWIDGET_GetIndent()


Description:
   This function gets the current value of the indentation property of the 
   HTML Widget object

Prototype:
   IWIDGET_GetIndent(IWidget *po, int *pnIndent);

Parameters:
===pre>
   po:       Pointer to the IWidget Interface Object
   pnIndent: Pointer to an integer where the retrieved indentation value will
             be stored

===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None


=============================================================================

IWIDGET_SetVSpace()


Description:
   This function sets the basic paragraph spacing of the HTML Widget object

Prototype:
   int IWIDGET_SetVSpace(IWidget *po, int nVSpace);

Parameters:
===pre>
   po:      Pointer to the IWidget Interface Object
   nVSpace: Integer amount of paragraph spacing

===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to set the requested property.
   EFAILED - If the property could not be set.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None


=============================================================================

IWIDGET_GetVSpace()


Description:
   This function gets the current value of the paragraph spacing property of
   the HTML Widget object

Prototype:
   int IWIDGET_GetVSpace(IWidget *po, int *pnVSpace);

Parameters:
===pre>
   po:       Pointer to the IWidget Interface Object
   pnVSpace: Pointer to an integer where the retrieved paragraph spacing
             value will be stored

===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None


=============================================================================

IWIDGET_SetBufferSize()


Description:
   This function sets the internal buffer size of the HTML Widget object

Prototype:
   int IWIDGET_SetBufferSize(IWidget *po, uint32 dwSize);

Parameters:
===pre>
   po:      Pointer to the IWidget Interface Object
   dwSize:  New value for internal buffer size

===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to set the requested property.
   EFAILED - If the property could not be set.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None


=============================================================================

IWIDGET_GetBufferSize()


Description:
   This function gets the current value of the internal buffer size property 
   of the HTML Widget object

Prototype:
   IWIDGET_GetBufferSize(IWidget *po, uint32 *pdwSize);

Parameters:
===pre>
   po:       Pointer to the IWidget Interface Object
   pdwSize:  Pointer to an integer where the retrieved internal buffer size
             value will be stored

===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None


=============================================================================

IWIDGET_SetFocusPos()


Description:
   This function sets the focus position of the HTML Widget object

Prototype:
   int IWIDGET_SetFocusPos(IWidget *po, int32 nPos);

Parameters:
===pre>
   po:      Pointer to the IWidget Interface Object
   nPos:    New value for the focus position

===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to set the requested property.
   EFAILED - If the property could not be set.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None


=============================================================================

IWIDGET_GetFocusPos()


Description:
   This function gets the current value of the focus position property 
   of the HTML Widget object

Prototype:
   int IWIDGET_GetFocusPos(IWidget *po, int32 *pnPos);

Parameters:
===pre>
   po:       Pointer to the IWidget Interface Object
   pnPos:    Pointer to an integer where the retrieved focus position
             value will be stored

===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None


=============================================================================

IWIDGET_SetScrollPos()


Description:
   This function sets the scroll position of the HTML Widget object

Prototype:
   int IWIDGET_SetScrollPos(IWidget *po, int32 nPos);

Parameters:
===pre>
   po:      Pointer to the IWidget Interface Object
   nPos:    New value for the scroll position

===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to set the requested property.
   EFAILED - If the property could not be set.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None


=============================================================================

IWIDGET_GetScrollPos()


Description:
   This function gets the current value of the scroll position property 
   of the HTML Widget object

Prototype:
   int IWIDGET_GetScrollPos(IWidget *po, int32 *pnPos);

Parameters:
===pre>
   po:       Pointer to the IWidget Interface Object
   pnPos:    Pointer to an integer where the retrieved scroll position
             value will be stored

===/pre>

Return Value:
===pre>
   SUCCESS - If the widget is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>

Comments:
   None

Side Effects:
   None


See Also:
   None

==========================================================================
*/

#endif //__AEEHTMLWIDGET_H__

