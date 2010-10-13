#ifndef _AEEHTMLVIEWER_H
#define _AEEHTMLVIEWER_H
/*======================================================
FILE:  AEEHTMLViewer.h

SERVICES:  AEE HTML Interfaces

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/
#include "AEEWeb.h"

typedef struct IHtmlViewer IHtmlViewer;

// IHtmlViewer property flags
#define HVP_SCROLLBAR         0x00000001
#define HVP_INLINE_EDIT       0x00000002
#define HVP_INVALIDATION      0x00000004     // expose invalidation behaviour
#define HVP_LOADALLIMAGES     0x00000008     // images will be loaded even if width and height
                                             // are not specified or if images are large
#define HVP_TRANSPARENT       0x00000010

enum {
   HVN_NONE = 0         // not used
  ,HVN_DONE             // done loading base document (objects/images may still be pending)
                        // (nDone = SUCCESS or ERROR)
  ,HVN_JUMP             // user selected link
  ,HVN_SUBMIT           // user submitted form
  ,HVN_FOCUS            // focus changed
  ,HVN_REDRAW_SCREEN    // htmlviewer drew outside of bounding rect, applet must redraw its screen
  ,HVN_FULLSCREEN_EDIT  // htmlviewer edit field going into fullscreen mode
  ,HVN_INVALIDATE       // htmlviewer needs redraw
  ,HVN_PAGEDONE         // first visible page finished loading including objects/images
  ,HVN_CONTENTDONE      // document finished loaded including objects/images
  ,HVN_ERASEBKGND       // erase background
};
#ifdef WIN32
typedef unsigned HVNotifyCode;
#else
typedef int8 HVNotifyCode;
#endif


//  HtmlViewer Option IDs:
//
// [Here we use the WEBOPT convention associating option ID ranges with value types (see AEEWeb.h).]

#define HVOPT_N_INDENT    (WEBOPT_32BIT+0x01001)     // basic indent
#define HVOPT_N_VSPACE    (WEBOPT_32BIT+0x01002)     // basic paragraph spacing
#define HVOPT_N_SBWIDTH   (WEBOPT_32BIT+0x01003)     // scrollbar width
#define HVOPT_N_ERROR     (WEBOPT_32BIT+0x01004)     // HVERR_... flags           (get only)
#define HVOPT_RGB_TEXT    (WEBOPT_32BIT+0x01101)     // default text color
#define HVOPT_RGB_LINK    (WEBOPT_32BIT+0x01102)     // link text color
#define HVOPT_SZ_TYPE     (WEBOPT_SZ+0x01001)        // MIME type and/or charset  (set only)
#define HVOPT_SZ_URL      (WEBOPT_SZ+0x01002)        // base URL                  (set only)
#define HVOPT_PTR_IWEB    (WEBOPT_INTERFACE+0x01003) // IHtmlViewer uses IWeb interface to obtain sub-objects

typedef void *HVOpt;             // all types of values can fit in a 'void *'


// Types of focus items
enum {
   HVFOCUS_NONE = 0              // no visible item
  ,HVFOCUS_LINK                  // <A HREF=...>
  ,HVFOCUS_EDIT                  // <INPUT TYPE={TEXT|PASSWORD}> or <TEXTAREA>
  ,HVFOCUS_CHECKBOX              // <INPUT TYPE=CHECKBOX> : state = 1/0 for checked/unchecked
  ,HVFOCUS_RADIO                 // <INPUT TYPE=RADIO> : state = 1/0 for selected/unselected
  ,HVFOCUS_SUBMIT                // <INPUT TYPE=SUBMIT>
  ,HVFOCUS_RESET                 // <INPUT TYPE=RESET>
  ,HVFOCUS_LIST                  // <SELECT>
};
#ifdef WIN32
typedef unsigned HVFocusType;
#else
typedef int8 HVFocusType;
#endif



// HViewJump:  contains data for hypertext jumps and form submissions
//
typedef struct {
   int            posElem;     // FORM or A item that triggered submit
   const char *   pszURL;      // URL ("URI")
   const char *   pszMethod;   // HVN_SUBMIT:  "GET", "POST", etc.
   const char *   pszData;     // HVN_SUBMIT:  NULL for "GET"; form data otherwise.
} HViewJump;


typedef struct {
   int            posElem;     // -1 when no focus
   HVFocusType    nType;       // HVFOCUS_... type
   int            nState;      // state, dependent upon type.
} HViewFocus;


// HViewNotify:  notification data
//
typedef struct {
   HVNotifyCode   code;        // notification code
   IHtmlViewer *  phv;         // identifies IHtmlViewer that sent the notification

   union {
      HViewJump   jump;        // HVN_JUMP
      HViewJump   submit;      // HVN_SUBMIT
      int         nDone;       // HVN_DONE
      HViewFocus  focus;       // HVN_FOCUS
   } u;
} HViewNotify;


typedef void (*PFNHVIEWNOTIFY)   (void * pvUser, HViewNotify * pHViewNotify);

//
AEEINTERFACE(IHtmlViewer) {

   INHERIT_IControl(IHtmlViewer);

   int      (*SetOption)      (IHtmlViewer * p, int32 nID, HVOpt pVal);
   int      (*GetOption)      (IHtmlViewer * p, int32 nID, HVOpt *ppVal);
   void     (*LoadStream)     (IHtmlViewer * p, IAStream * ps);
   void     (*LoadSource)     (IHtmlViewer * p, ISource * ps);
   void     (*SetNotifyFn)    (IHtmlViewer * p, PFNHVIEWNOTIFY pfnNotify, void * pvUser);
   void     (*SetData)        (IHtmlViewer * p, const char * psz, int cbLen);
   boolean  (*IsBusy)         (IHtmlViewer * p);
   void     (*Stop)           (IHtmlViewer * p);
   int32    (*FindElem)       (IHtmlViewer * p, const char *pszElem, int32 nIndex);
   int32    (*GetElemText)    (IHtmlViewer * p, int32 nElem, char *pchBuf, int32 cbBuf);
   int32    (*GetAttrValue)   (IHtmlViewer * p, int32 nElem, const char *pszAttr,  char *pchBuf, int32 cbBuf);
   void     (*SetViewState)   (IHtmlViewer * p, PCSTR pcszState);
   int      (*GetViewState)   (IHtmlViewer * p, char * pszBuffer, int cbBuffer);
};



#define IHTMLVIEWER_AddRef(p)                AEEGETPVTBL((p),IHtmlViewer)->AddRef(p)
#define IHTMLVIEWER_Release(p)               AEEGETPVTBL((p),IHtmlViewer)->Release(p)

#define IHTMLVIEWER_HandleEvent(p,e,w,d)     AEEGETPVTBL((p),IHtmlViewer)->HandleEvent((p),(e),(w),(d))
#define IHTMLVIEWER_Redraw(p)                AEEGETPVTBL((p),IHtmlViewer)->Redraw(p)
#define IHTMLVIEWER_SetActive(p,a)           AEEGETPVTBL((p),IHtmlViewer)->SetActive((p),(a))
#define IHTMLVIEWER_IsActive(p)              AEEGETPVTBL((p),IHtmlViewer)->IsActive(p)
#define IHTMLVIEWER_SetRect(p,r)             AEEGETPVTBL((p),IHtmlViewer)->SetRect((p),(r))
#define IHTMLVIEWER_GetRect(p,r)             AEEGETPVTBL((p),IHtmlViewer)->GetRect((p),(r))
#define IHTMLVIEWER_SetProperties(p,n)       AEEGETPVTBL((p),IHtmlViewer)->SetProperties((p),(n))
#define IHTMLVIEWER_GetProperties(p)         AEEGETPVTBL((p),IHtmlViewer)->GetProperties(p)
#define IHTMLVIEWER_Reset(p)                 AEEGETPVTBL((p),IHtmlViewer)->Reset(p)

#define IHTMLVIEWER_SetOption(p,n,v)         AEEGETPVTBL((p),IHtmlViewer)->SetOption((p),(n),(v))
#define IHTMLVIEWER_GetOption(p,n,pv)        AEEGETPVTBL((p),IHtmlViewer)->GetOption((p),(n),(pv))
#define IHTMLVIEWER_LoadStream(p,ps)         AEEGETPVTBL((p),IHtmlViewer)->LoadStream((p),(ps))
#define IHTMLVIEWER_LoadSource(p,ps)         AEEGETPVTBL((p),IHtmlViewer)->LoadSource((p),(ps))
#define IHTMLVIEWER_SetNotifyFn(p,pf,pv)     AEEGETPVTBL((p),IHtmlViewer)->SetNotifyFn((p),(pf),(pv))
#define IHTMLVIEWER_SetData(p,pb,l)          AEEGETPVTBL((p),IHtmlViewer)->SetData((p),(pb),(l))
#define IHTMLVIEWER_IsBusy(p)                AEEGETPVTBL((p),IHtmlViewer)->IsBusy(p)
#define IHTMLVIEWER_Stop(p)                  AEEGETPVTBL((p),IHtmlViewer)->Stop(p)
#define IHTMLVIEWER_FindElem(p,p2,n)         AEEGETPVTBL((p),IHtmlViewer)->FindElem(p,p2,n)
#define IHTMLVIEWER_GetElemText(p,n,pb,cb)   AEEGETPVTBL((p),IHtmlViewer)->GetElemText(p,n,pb,cb)
#define IHTMLVIEWER_GetAttrValue(p,n,s,pb,cb) AEEGETPVTBL((p),IHtmlViewer)->GetAttrValue(p,n,s,pb,cb)
#define IHTMLVIEWER_SetViewState(p,s)        AEEGETPVTBL((p),IHtmlViewer)->SetViewState((p),(s))
#define IHTMLVIEWER_GetViewState(p,psz,cb)   AEEGETPVTBL((p),IHtmlViewer)->GetViewState((p),(psz),(cb))

#define IHTMLVIEWER_ParseBuffer(p,pb)        AEEGETPVTBL((p),IHtmlViewer)->SetData((p),(pb),-1)
#define IHTMLVIEWER_Draw(p)                  AEEGETPVTBL((p),IHtmlViewer)->Redraw(p)


// Shorthand:

#define IHTMLVIEWER_SetType(p,psz)           IHTMLVIEWER_SetOption(p, HVOPT_SZ_TYPE, (HVOpt)(psz))
#define IHTMLVIEWER_SetURL(p,psz)            IHTMLVIEWER_SetOption(p, HVOPT_SZ_URL, (HVOpt)(psz))
#define IHTMLVIEWER_SetIndent(p,n)           IHTMLVIEWER_SetOption(p, HVOPT_N_INDENT, (HVOpt)(n))
#define IHTMLVIEWER_SetVSpace(p,n)           IHTMLVIEWER_SetOption(p, HVOPT_N_VSPACE, (HVOpt)(n))
#define IHTMLVIEWER_SetSBWidth(p,n)          IHTMLVIEWER_SetOption(p, HVOPT_N_SBWIDTH, (HVOpt)(n))
#define IHTMLVIEWER_SetTextColor(p,rgb)      IHTMLVIEWER_SetOption(p, HVOPT_RGB_TEXT, (HVOpt)(rgb))
#define IHTMLVIEWER_SetLinkColor(p,rgb)      IHTMLVIEWER_SetOption(p, HVOPT_RGB_LINK, (HVOpt)(rgb))
#define IHTMLVIEWER_SetIWeb(p,piw)           IHTMLVIEWER_SetOption(p, HVOPT_PTR_IWEB, (HVOpt)(piw))

#define IHTMLVIEWER_GetError(p,pn)           IHTMLVIEWER_GetOption(p, HVOPT_N_ERROR, (HVOpt)(pn))


// GetElemText / GetAttrValue return values:
//
#define IHTMLVIEWER_NOTFOUND       -1
#define IHTMLVIEWER_BADELEMENT     -2
#define IHTMLVIEWER_BADATTR        -3
#define IHTMLVIEWER_NOVALUE        -4



/*=====================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

IHtmlViewer Interface

The IHtmlViewer interface allows the application to control an HTML viewer 
object.  BREW provides an HTML viewer that supports a subset of HTML 
version 3.2.  The viewer is not an application, but an object to be 
invoked by a host application.  It processes an HTML document, displays 
its visual representation on the screen, and allows a user to interact 
with the document: scrolling, taking links, and entering data into form 
fields.  

The HTML viewer does not involve itself with network access in any way.  
User actions that follow a link or submit a form results in notifications 
being sent to the host application.  

The HTML viewer in the current version of BREW has been designed to enable 
developers to add a degree of flexibility to application interfaces and to 
simplify application development.  The feature set is focused on enabling 
flexible integration with applications.  Its HTML formatting features and 
its ability to deal with malformed content are limited.  HTML presented to 
the viewer should be properly constructed, with start and end tags for 
nonempty elements and no mismatched end tags.  As a result, HTML that is 
tested with the particular application is advised, and it is not intended 
to support for arbitrary content (as obtained from various web sites).  


Notable HTML features supported include:

- Hypertext links 

- Simple HTML FORM support: text input, lists, submit buttons, check 
boxes, and radio buttons 

- inline images or plug-ins 

- Horizontal alignments: left, center, right 

- Bold text (displayed using the AEE_FONT_BOLD font) 

- Large text (displayed using the AEE_FONT_LARGE font) 

- Text colors 


Notable HTML features that are not supported in the current version of BREW include:

- Java applets 

- FRAMESET 

- Scripting 

- Background colors 

- Cascading Style Sheets 

- Unicode character references 


The HTML viewer can be created using the AEECLSID_HTML ClassID.

The following header file is required:~
AEEHTMLViewer.h

=============================================================================


IHTMLVIEWER_AddRef()


Description:
   This function is inherited from IBASE_AddRef(). 
   
Version:
   Introduced BREW Client 1.1

See Also:
   IHTMLVIEWER_Release()


=============================================================================


IHTMLVIEWER_FindElem()


Description:
   
   This function finds an element, given the element name and the number of 
   the occurrence.  

Prototype:

   int32 IHTMLVIEWER_FindElem(
      IHtmlViewer *  pIHtmlViewer, 
      const char *   pszElem, 
      int            nIndex
   );


Parameters:

   pIHtmlViewer: Pointer to the IHtmlViewer Interface object.  
   
   pszElem: Name of the element to find, which is a zero-terminated ASCII string (case 
      insensitive).  
   
   nIndexd: Which occurrence (0 => first element, 1=> second element, and so on).  
 

Return Value:

   A non-negative number corresponding to the location of the element within the document,
   if found
   
   IHTMLVIEWER_NOTFOUND, if pszElem was not found.
   
   IHTMLVIEWER_BADELEMENT, if pszElem was not a recognized element name. Only recognized
   elements can be inspected


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   None


=============================================================================


IHTMLVIEWER_GetAttrValue()


Description:
   
   This function obtains the value of an attribute of an element.  


Prototype:

   int32 IHTMLVIEWER_GetAttrValue(
      IHtmlViewer *  pIHTMLViewer,
      int32          nElemPos,
      const char *   pszAttr,
      char *         pchBuf,
      int32          cbBuf
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   nElemPos: Location of the element in the document
   
   pszAttr: Name of attribute, zero-terminated ASCII string (case insensitive)
   
   pchBuf: Pointer to the start of a buffer to hold the result (which is a 
      zero-terminated string) 
         
   cbBuf: Size of the buffer, in bytes
 

Return Value:

   IHTMLVIEWER_NOTFOUND, attribute is not found
   
   IHTMLVIEWER_BADELEMENT, nElemPos was invalid.
   
   IHTMLVIEWER_BADATTR, pszAttr is not a recognized attribute name. Only recognized
   attributes are available for query.
   
   IHTMLVIEWER_NOVALUE, attribute was specified, but no value was assigned. 
   
   Non-negative number corresponding to the length of the string copied into pchBuf.
   
   A value larger than cbBuf may be returned, but only cbBuf bytes are written to the
   buffer in that case.


Comments:
   
   None

Version:

   Introduced BREW Client 1.1

See Also:

   None


=============================================================================


IHTMLVIEWER_GetElemText()


Description:

   This function obtains the text content of an element.  Contained elements 
   are ignored, and separate sections of text content (including that of 
   sub-elements) are concatenated.  


Prototype:

   int32 IHTMLVIEWER_GetElemText(
      IHtmlViewer *  pIHTMLViewer,
      int32          nElemPos,
      char *         pchBuf,
      int32          cbBuf
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   nElemPos: The location of the element in the document.  This can be obtained from 
      IHTMLVIEWER_FindElem().  
   
   pchBuf: Pointer to the start of a buffer to hold the result (which is a 
      zero-terminated string) 
   
   cbBuf: Size of the buffer, in bytes
   

Return Value:

   IHTMLVIEWER_BADELEMENT, if fails.  This occurs only when the nElemPos 
   value is invalid.  
   
   Length of the resulting string, if successful. 
   
   0(zero), if an empty element, or an element that contains no text is 
   present.  
   
   A value larger than cbBuf may be returned, but only cbBuf bytes are 
   written to the buffer in that case.  

Version:
   Introduced BREW Client 1.1


See Also:
   
   IHTMLVIEWER_FindElem()
   IHTMLVIEWER_GetAttrValue()


=============================================================================


IHTMLVIEWER_GetError()


Description:

   This function is a shorthand function for 
   IHTMLVIEWER_SetOption(HVOPT_N_ERROR).  It gets the error code value.  


Prototype:
   
   int IHTMLVIEWER_GetError(
      IHtmlViewer *pIHtmlViewer, 
      int *pnError
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   pnError: Pointer to an int that recieves the error value.
 

Return Value:

   Returns an IHtmlViewer specific error code.


Comments:
   
   This function is actually a macro that calls IHTMLVIEWER_GetOption().

Version:
   Introduced BREW Client 1.1


See Also:
   
   IHTMLVIEWER_GetOption()


=============================================================================


IHTMLVIEWER_GetOption()


Description:

   This function retrieves one of a number of values that describe the state 
   of the viewer.  Note that this does not retrieve the values that have been 
   set through IHTMLVIEWER_SetOption().  


Prototype:
   
   int IHTMLVIEWER_GetOption(
      IHtmlViewer *  pIHTMLViewer, 
      int32          nID, 
      HVOpt *        ppVal
   );

Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   nID: ID of the value being retrieved.
   
   ppVal: Address of variable or memory location to hold the result.
 

Return Value:

   SUCCESS, if option was retrieved; * ppVal holds the result.
   
   EUNSUPPORTED, if option ID is not supported; * ppVal remains unmodified.
   

Comments:
   
   Callers do not call this function directly; there are macros defined for 
   each individual option (see IHTMLVIEWER_SetOption()).  

Version:
   Introduced BREW Client 1.1


See Also:
   
   IHTMLVIEWER_SetOption()
   IHTMLVIEWER_GetError()


=============================================================================


IHTMLVIEWER_GetProperties()

Description:

   This function returns the control-specific properties or flags.


Prototype:
   
   uint32 IHTMLVIEWER_GetProperties(
      IHTMLViewer * pIHTMLViewer
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
 

Return Value:
   
   32-bit properties for the control.


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   IHTMLVIEWER_SetProperties()


=============================================================================


IHTMLVIEWER_GetRect()


Description:

   This function fills a pointer to an input AEERect structure with the 
   active screen coordinates for the control.  This is particularly useful 
   after a control is created to determine its optimal/default size and 
   position.  


Prototype:

   void IHTMLVIEWER_GetRect(
      IHTMLViewer *  pIHTMLViewer, 
      AEERect *      prc
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   prc: Rectangle to be filled with the coordinates of the control.
 

Return Value:
   
   None


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   AEERect
   IHTMLVIEWER_SetRect()


=============================================================================


IHTMLVIEWER_GetViewState()


Description:

   This function returns a string that represents the current viewer state.  
   This string may be passed to IHTMLVIEWER_SetViewState() to restore the 
   viewer state.  


Prototype:

   int IHtmlViewer_GetViewState(
      IHtmlViewer *  p, 
      char *         pszBuffer, 
      int            cbBuffer
   );

Parameters:

   p: Pointer to the IHtmlViewer object

   pszBuffer: Pointer to start of a buffer to hold the result (which will be a 
      zero-terminated string).  
   
   cbBuffer: size of the buffer, in bytes
 

Return Value:

   The length of the result string including zero terminator.
   
   NOTE: if either pszBuffer==NULL or cbBuffer==0, the result string will not 
   be written but the required buffer size will be returned.  


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   IHTMLVIEWER_SetViewState()


=============================================================================


IHTMLVIEWER_HandleEvent()


Description:

   This function provides the main event processing for a control.  It is 
   called when any event is passed to the control.  Events mainly include 
   keypress events.  This function must be implemented by all controls.  


Prototype:

   boolean IHTMLVIEWER_HandleEvent(
      IHTMLViewer *  pIHTMLViewer,
      AEEEvent       evt, 
      uint16         wp,
      uint32         dwp
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   evt: Event code.
   
   wp: 16-bit event data.
   
   dwp: 32-bit event data.
 

Return Value:

   TRUE, if the event was processed by the control.
   
   FALSE, if otherwise.


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   None


=============================================================================


IHTMLVIEWER_IsActive()


Description:

   This function returns the active or focus state of the control.


Prototype:

   boolean IHTMLVIEWER_IsActive(
      IHTMLViewer * pIHTMLViewer
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
 

Return Value:

   TRUE, if the control is active.
   
   FALSE, if otherwise.


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   None


=============================================================================


IHTMLVIEWER_IsBusy()


Description:

   This function returns TRUE if the viewer is in the process of loading HTML 
   from an IAStream or ISource.  


Prototype:
   
   boolean IHTMLVIEWER_IsBusy(
      IHtmlViewer * pIHTMLViewer
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
 

Return Value:
   
   TRUE, if the viewer is busy.
   
   FALSE, if not.


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   None


=============================================================================


IHTMLVIEWER_LoadSource()


Description:

   This function loads the contents of an ISource into the document, parsing 
   the data as HTML.  Any preexisting contents are deleted.  Loading 
   continues asynchronously in the background, and generally does not 
   complete during the call to this function.  When the end of the ISource is 
   reached, the documents contents are displayed.  The caller can check 
   whether the viewer is busy loading by calling IHTMLVIEWER_IsBusy(), and it 
   can be notified of completion by setting a notification callback and 
   watching for the HVN_DONE notification.  The character encoding of the 
   source stream is assumed to be an 8-bit encoding compatible with the 
   native encoding of the device (ISOLATIN 1, Shift-JIS, or KSC5601).  


Prototype:

   void IHTMLVIEWER_LoadSource(
      IHtmlViewer *  pIHTMLViewer, 
      ISource *      ps
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   ps: Pointer to the ISource object
 

Return Value:
   
   None


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   None


=============================================================================


IHTMLVIEWER_LoadStream()


Description:

   This function loads the contents of an IAStream into the document, parsing 
   the data as HTML.  Any preexisting contents are deleted.  Loading 
   continues asynchronously in the background, and generally does not 
   complete during the call to this function.  When the end of the IAStream 
   is reached, the documents contents are displayed.  The caller can check 
   whether the viewer is busy loading by calling IHTMLVIEWER_IsBusy(), and it 
   can be notified of completion by setting a notification callback and 
   watching for the HVN_DONE notification.  The character encoding of the 
   source stream is assumed to be an 8-bit encoding compatible with the 
   native encoding of the device (ISOLATIN 1, Shift-JIS, or KSC5601).  


Prototype:

   void IHTMLVIEWER_LoadStream(
      IHtmlViewer *  pIHTMLViewer, 
      IAStream *     ps
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   ps: Pointer to the IAStream object
 

Return Value:
   
   None


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   None


=============================================================================


IHTMLVIEWER_ParseBuffer()


Description:

   This function loads the viewer with the contents of a zero-terminated 
   string.  This is equivalent to IHTMLVIEWER_SetData(), except that the HTML 
   is provided in a zero-terminated string instead of in a length-limited 
   buffer.  See IHTMLVIEWER_SetData() for more details.  


Prototype:

   void IHTMLVIEWER_ParseBuffer(
      IHtmlViewer *  pIHTMLViewer, 
      const char *   psz
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   psz: Pointer to the zero-terminated string containing HTML
 

Return Value:
   
   None


Comments:
   
   This function is actually a macro that calls IHTMLVIEWER_SetData() with 
   cbLen set to -1.  

Version:
   Introduced BREW Client 1.1


See Also:
   
   IHTMLVIEWER_SetData()


=============================================================================


IHTMLVIEWER_Redraw()


Description:

   This function redraws the entire HTML viewer control.  This is used to 
   refresh the viewer's screen area after part or all of it has been 
   overdrawn by some other entity.  The application does not normally have to 
   call this function to ensure that the screen has been updated after 
   changes have been made to the content or options of the viewer.  The 
   viewer takes care of minimizing unnecessary redrawing while ensuring that 
   changes are properly reflected on the screen.  


Prototype:

   boolean IHTMLVIEWER_Redraw(
      IHTMLViewer * pIHTMLViewer
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
 

Return Value:

   TRUE, if the event was processed by the control.
   
   FALSE, if otherwise.


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   None


=============================================================================


IHTMLVIEWER_Release()


Description:
	This function is inherited from IBASE_Release(). 
	
See Also:
   IHTMLVIEWER_AddRef()


=============================================================================


IHTMLVIEWER_Reset()


Description:

   This function instructs the control to reset (free or delete) its contents 
   and to immediately leave active/focus mode.  This is useful in freeing all 
   underlying memory in text or menu controls or removing all menu items in a 
   single call.  


Prototype:

   void IHTMLVIEWER_Reset(
      IHTMLViewer * pIHTMLViewer
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
 

Return Value:
   
   None


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   IHTMLVIEWER_SetActive()


=============================================================================


IHTMLVIEWER_SetActive()


Description:

   This function instructs the control to enter or leave focus or selected 
   mode.  The concept of focus is left somewhat to the control.  In the case 
   of menus, focus indicates that the menu is active.  In terms of text 
   controls it means the control is active and in edit mode.  This call 
   usually results in the underlying control redrawing its contents.  It is 
   important to know that controls still have their IHTMLVIEWER_HandleEvent() 
   function called even when they are inactive.  This allows them to process 
   special events such as scrolling multiline text controls.  


Prototype:

   void IHTMLVIEWER_SetActive(
      IHTMLViewer *  pIHTMLViewer, 
      boolean        bActive
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   bActive: Specifies whether to activate (TRUE) or deactivate (FALSE) the control.  
 


Return Value:
   
   None


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   None


=============================================================================


IHTMLVIEWER_SetData()


Description:

   This function provides a buffer of HTML to the viewer.  All of the buffer 
   is parsed into the viewer synchronously.  Any preexisting contents are 
   discarded.  On return, the viewer retains no pointers into the buffer.  
   The character encoding of the source stream is assumed to be an 8-bit 
   encoding compatible with the native encoding of the device (ISOLATIN 1, 
   Shift-JIS, or KSC5601).  


Prototype:

   void IHTMLVIEWER_SetData(
      IHtmlViewer *  pIHTMLViewer,
      const char *   psz,
      int            cbLen
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   psz: Pointer to the start of buffer (not necessarily zero-terminated)
   
   cbLen: Size of buffer, or -1.  -1 indicates that psz points to a zero-terminated 
      string and all of that string is to be loaded into the viewer.  
 

Return Value:
   
   None


Comments:
   
   None

Version:
   Introduced BREW Client 1.1


See Also:
   
   IHTMLVIEWER_ParseBuffer()


=============================================================================


IHTMLVIEWER_SetIndent()


Description:
   
   This function is a shorthand function for IHTMLVIEWER_SetOption 
   (HVOPT_N_INDENT).  Sets the basic indent amount.  


Prototype:

   int IHTMLVIEWER_SetIndent(
      IHtmlViewer * p, 
      int nIndent
   );


Parameters:

   p: Pointer to the IHtmlViewer Interface.
   
   nIndent: Default indentation, in pixels.
 

Return Value:

   SUCCESS or one of the standard BREW error codes.


Comments:
   
   This function is actually a macro that calls IHTMLVIEWER_SetOption().

Version:
   Introduced BREW Client 1.1


See Also:
   
   IHTMLVIEWER_SetOption().


=============================================================================


IHTMLVIEWER_SetLinkColor()


Description:
   
   This function is a Shorthand function for IHTMLVIEWER_SetOption 
   (HVOPT_RGB_LINK).  Sets the default text color.  


Prototype:

   void IHTMLVIEWER_SetLinkColor(
      IHtmlViewer *  pIHTMLViewer, 
      RGBVAL         rgbLink
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   rgbLink: Color for links
 

Return Value:

   SUCCESS or one of the standard BREW error codes.


Comments:

   None

Version:
   Introduced BREW Client 1.1


See Also:

   RGBVAL
   IHTMLVIEWER_SetOption().


=============================================================================


IHTMLVIEWER_SetNotifyFn()


Description:

   This function specifies the information in a notification callback, which 
   consists of a function pointer and a void context pointer to be passed to 
   the function when it is called.  The notification callback is used to 
   notify the owner of events, usually resulting from user actions.  There is 
   never more than one notify callback active at any time.  Setting a 
   callback overrides any previous setting.  Passing NULL for pfnNotify 
   disables notification callbacks entirely.  


Prototype:

   void IHTMLVIEWER_SetNotifyFn(
      IHtmlViewer *  pIHTMLViewer,
      PFNHVIEWNOTIFY pfnNotify,
      void *         pvUser
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   pfnNotify: Pointer to the function to be called with notifications, or NULL if 
      notifications are to be canceled.  
   
   pvUser: 32-bit value to be passed to pfnNotify when/if it is called.
 

Return Value:
   
   None


Comments

   None

Version:
   Introduced BREW Client 1.1


See Also:

   PFNHVIEWNOTIFY


=============================================================================


IHTMLVIEWER_SetOption()


Description:

   This function sets one of a number of options that control the behavior of 
   the viewer.  Callers typically do not call this function directly; there 
   are macros defined for each individual option.  


Prototype:

   int IHTMLVIEWER_SetOption(
      IHtmlViewer * pIHTMLViewer,
      int32 nID,
      HVOpt pVal
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   nID: ID of the option being set.
   
   pVal: Value to assign to the option.
 

Return Value:

   SUCCESS, if option was assigned.
   
   EUNSUPPORTED, if option ID is not supported.


Comments:

   None

Version:
   Introduced BREW Client 1.1


See Also:

   IHTMLVIEWER_SetType()
   IHTMLVIEWER_SetURL()
   IHTMLVIEWER_SetIndent()
   IHTMLVIEWER_SetVSpace()
   IHTMLVIEWER_SetSBWidth()
   IHTMLVIEWER_SetTextColor()
   IHTMLVIEWER_SetLinkColor()
   IHTMLVIEWER_GetOption()


=============================================================================


IHTMLVIEWER_SetProperties()


Description:

   This function sets control-specific properties or flags.  Although some 
   properties are defined across controls (CP_FRAME, and so forth), most 
   controls expose a range of properties to allow more specific control over 
   the object.  
   
   HVP_SCROLLBAR, when set, causes the viewer to display a scrollbar when the 
   document contents do not fit in the rectangle allocated to the control.  
   If the document contents do fit, a scrollbar is not visible, but space for 
   the scrollbar is still reserved.  
   
   HVP_INLINE_EDIT causes text input fields 
   to be editable in place in the document when they get focus.  Otherwise, 
   the inline representation is read-only, and the user has to press SELECT 
   to edit the contents.  

   HVP_TRANSPARENT can be used to display the viewer over any background.  In
   this case, the viewer sends the HVN_ERASEBKGND notification so that the
   app can re-paint the background before it renders the HTML.


Prototype:

   void IHTMLVIEWER_SetProperties(
      IHTMLViewer *  pIHTMLViewer, 
      uint32         dwProps
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   dwProps: 32-bit set of flags/properties.
 

Return Value:

   None


Comments:

   None

Version:
   Introduced BREW Client 1.1


Side Effects:

   May redraw the control


See Also:

   IHTMLVIEWER_GetProperties()
   IHTMLVIEWER_SetNotifyFn()


=============================================================================


IHTMLVIEWER_SetRect()


Description:

   This function sets the active screen coordinates of the control.  It may 
   result in the control redrawing its contents.  


Prototype:

   void IHTMLVIEWER_SetRect(
      IHTMLViewer *     pIHTMLViewer, 
      const AEERect *   prc
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   prc: The bounding rectangle for the control.
   

Return Value:

   None


Comments:

   None


Side Effects:

   May redraw the control

Version:
   Introduced BREW Client 1.1


See Also:

   AEERect
   IHTMLVIEWER_GetRect()


=============================================================================


IHTMLVIEWER_SetSBWidth()


Description:

   This function is a shorthand function for IHTMLVIEWER_SetOption 
   (HVOPT_N_SBWIDTH).  It sets the width of the vertical scroll bar.  


Prototype:

   int IHTMLVIEWER_SetSBWidth(
      IHtmlViewer *  pIHTMLViewer, 
      int            nSBWidth
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   nSBWidth: Basic paragraph spacing.
 

Return Value:

   SUCCESS or one of the standard BREW error codes.


Comments:

   None

Version:
   Introduced BREW Client 1.1


See Also:

   None


=============================================================================


IHTMLVIEWER_SetTextColor()


Description:

   This function is a shorthand function for IHTMLVIEWER_SetOption 
   (HVOPT_RGB_TEXT).  It sets the default text color.  


Prototype:

   int IHTMLVIEWER_SetTextColor(
      IHtmlViewer *  pIHTMLViewer, 
      RGBVAL         rgb
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   rgbText: Color for text.
 

Return Value:

   Always returns SUCCESS.


Comments:

   None

Version:
   Introduced BREW Client 1.1


See Also:

   None


=============================================================================


IHTMLVIEWER_SetType()


Description:

   This function is a shorthand function for IHTMLVIEWER_SetOption 
   (HVOPT_SZ_TYPE).  it sets the MIME type and/or character set for the 
   viewer.  At present, the MIME type, if specified, other than 'text/html' 
   is an error.  The charset, if specified, other than one of the supported 
   charsets is an error.  


Prototype:

   int IHtmlViewer_SetType(
      IHtmlViewer *pIHTMLViewer, 
      char *pszType
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   pszType: Pointer to the MIME type string with the following format-

         "text/html; charset=iso-8859-1" or "charset=iso-8859-1"

      NOTE - The MIME type string follows the semantics of the 
      Content-type field described in RFC2046.  
 


Return Value:

   SUCCESS or one of the standard BREW error codes.


Comments:

   None

Version:
   Introduced BREW Client 1.1


See Also:

   IHTMLVIEWER_SetOption()
   HVOPT_SZ_TYPE


=============================================================================


IHTMLVIEWER_SetURL()


Description:

   This function specifies the URL address of the document that is to be 
   loaded into the viewer.  This should be called before HTML is loaded into 
   a document.  


Prototype:

   int IHTMLVIEWER_SetURL(
      IHtmlViewer *  pIHTMLViewer, 
      const char *   pszURL
   )


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   pszURL: Base URL string.
 

Return Value:

   SUCCESS or one of the standard BREW error codes.


Comments:

   None

Version:
   Introduced BREW Client 1.1


See Also:

   None


=============================================================================


IHTMLVIEWER_SetViewState()


Description:

   This function restores the HTML viewer state (such as scroll position) to 
   a state represented by a string returned by IHTMLVIEWER_GetViewState().  


Prototype:

   void IHTMLVIEWER_SetViewState(
      IHtmlViewer * pIHtmlViewer, 
      PCSTR pcszState
   );


Parameters:

   pIHtmlViewer: Pointer to the IHtmlViewer Interface object.
   
   pcszState: String returned by IHTMLVIEWER_GetViewState() which represents a previous 
      view state.  
 

Comments:

   None

Version:
   Introduced BREW Client 1.1


See Also:

   IHTMLVIEWER_GetViewState()


=============================================================================


IHTMLVIEWER_SetVSpace()


Description:

   This function is a shorthand function for IHTMLVIEWER_SetOption 
   (HVOPT_N_VSPACE).it sets the vertical paragraph spacing.  


Prototype:

   int IHTMLVIEWER_SetVSpace(
      IHtmlViewer * pIHTMLViewer, 
      int nVSpace
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   nVSpace: Basic paragraph spacing, in pixels.
 

Return Value:

   SUCCESS or one of the standard BREW error codes.


Comments:

   None

Version:
   Introduced BREW Client 1.1


See Also:

   None


=============================================================================


IHTMLVIEWER_SetIWeb()


Description:

   This function is a shorthand function for IHTMLVIEWER_SetOption 
   (HVOPT_PTR_IWEB).  It sets the IWeb interface with which the viewer will 
   use to obtain sub-objects.  


Prototype:

   int IHTMLVIEWER_SetIWeb(
      IHtmlViewer *p, 
      IWeb * piWeb
   );


Parameters:

   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
   
   piWeb: IWeb interface pointer
 

Return Value:

   SUCCESS or one of the standard BREW error codes.


Comments:

   None

Version:
   Introduced BREW Client 1.1


See Also:

   None


=============================================================================


IHTMLVIEWER_Stop()


Description:

   This function causes the viewer to stop loading HTML from an IAStream or 
   ISource, if the viewer is busy.  If this stops loading of the document, an 
   HVN_DONE notification is triggered.  


Prototype:

   void IHTMLVIEWER_Stop(
      IHtmlViewer * pIHTMLViewer
   );


Parameters:


   pIHTMLViewer: Pointer to the IHtmlViewer Interface object.
 

Return Value:

   None


Comments:

   None

Version:
   Introduced BREW Client 1.1


See Also:

   None


*/ 
/*====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================

HtmlViewer Property Flags

Description:
   
   Property flags enable specific behavior of the HTML viewer object. Property
   flags may be used individually or in combination.

   HVP_SCROLLBAR           HTML viewer displays a vertical scrollbar
   
   HVP_INLINE_EDIT         Enables inline text controls in forms
   
   HVP_INVALIDATION        Enables HVN_INVALIDATE notification
   
   HVP_LOADALLIMAGES       Loads images that don't specify width and height,
                           or if images are larger than the screen
                           By default, only images with WIDTH and HEIGHT 
                           attributes are loaded.

Members:
   None

Comments:
   None

See Also:
   None

=============================================================================


HVNotifyCode


Description:
   
   Enum type contained in HViewNotify structure passed to PFNHVIEWNOTIFY 
   callback function.  Indicates type of notification.  


Definition:
   
   typedef enum HVNotifyCode {
      HVN_NONE = 0,
      HVN_DONE,
      HVN_JUMP,
      HVN_SUBMIT,
      HVN_FOCUS,
      HVN_REDRAW_SCREEN,
      HVN_FULLSCREEN_EDIT,
      HVN_INVALIDATE,
      HVN_PAGEDONE,
      HVN_CONTENTDONE
   } HVNotifyCode;


Members:
   
   HVN_NONE                unused value
   
   HVN_DONE                done loading (nDone = SUCCESS or ERROR)
   
   HVN_JUMP                user selected link (Use 'jump' member of HViewNotify struct)
   
   HVN_SUBMIT              user submitted form (Use 'submit' member of HViewNotify struct)
   
   HVN_FOCUS               focus changed (Use 'focus' member of HViewNotify struct)
   
   HVN_REDRAW_SCREEN       htmlviewer drew outside of bounding rect, applet must redraw its screen
   
   HVN_FULLSCREEN_EDIT     htmlviewer edit field going into fullscreen mode
   
   HVN_INVALIDATE          htmlviewer needs redraw
   
   HVN_PAGEDONE            first visible page finished loading including images
                           This notification will be sent synchronously to IHTMLVIEWER_SetData()
                           (i.e.,before it returns), if there are no images in the
                           first visible page, otherwise, it will be sent asynchronously,
                           when the images have been completely loaded.
   
   HVN_CONTENTDONE         document finished loaded including images
                           This notification will be sent synchronously to IHTMLVIEWER_SetData()
                           (i.e.,before it returns), if there are no images in the
                           document, otherwise, it will be sent asynchronously, when 
                           the images have been completely loaded.

Comments:
   None

See Also:
   None

===========================================================================


PFNHVIEWNOTIFY

Description:
   
   This is the type for the HTML viewer's notification callback functions. Notification
callbacks are called by the HTML viewer to notify the owner of changes of state,
user-generated events, and other occurrences.

Prototype:
   
   typedef void (*PFNHVIEWNOTIFY)(
      void *         pvUser, 
      HViewNotify *  pHViewNotify
   );

Parameters:  
   pvUser
      User supplied callback context.

   pHViewNotify
      Pointer to HViewNotify structure containing notification data.

Return Value:
   None

Comments:
   The actual application-defined callback function is passed to the 
   IHtmlViewer object with the SetNotifyFn() function.  

See Also:
   IHTMLVIEWER_SetNotifyFn()
   HVNotifyCode

===========================================================================

HVFocusType

Description:  
   Code to identify the type of focus change resulting in an HVN_FOCUS 
   notification.  

Definition:

    typedef enum {
       HVFOCUS_NONE = 0
       HVFOCUS_LINK,
       HVFOCUS_EDIT,
       HVFOCUS_CHECKBOX,
       HVFOCUS_RADIO,
       HVFOCUS_SUBMIT,
       HVFOCUS_RESET,
       HVFOCUS_LIST
    } HVFocusType;


Members:
===pre>
   
   HVFOCUS_NONE         no visible item
   
   HVFOCUS_LINK         <A HREF=...>
   
   HVFOCUS_EDIT         <INPUT TYPE={TEXT|PASSWORD}> or <TEXTAREA>
   
   HVFOCUS_CHECKBOX     <INPUT TYPE=CHECKBOX> : state = 1/0 for checked/unchecked
   
   HVFOCUS_RADIO        <INPUT TYPE=RADIO> : state = 1/0 for selected/unselected
   
   HVFOCUS_SUBMIT       <INPUT TYPE=SUBMIT>
   
   HVFOCUS_RESET        <INPUT TYPE=RESET>
   
   HVFOCUS_LIST         <SELECT>

===/pre>

===========================================================================

HViewJump:

Description:
   
   Data structure containing information for hypertext jumps and form 
   submissions.  Used for HVN_JUMP and HVN_SUBMIT notifications 

Definition:
   
   typedef struct {
      int            posElem;
      const char *   pszURL;
      const char *   pszMethod;
      const char *   pszData;
   } HViewJump;


Members:
   
   posElem
      position of <FORM> or <A> item that triggered submit

   pszURL
      HVN_JUMP: URL (URI) of hypertext link

   pszMethod
      HVN_SUBMIT: "GET", "POST", etc.

   pszData
      HVN_SUBMIT: NULL for "GET"; form data otherwise.

Comments:
    None

See Also:
    HViewNotify

===========================================================================

HViewFocus

Description:
   
   This data structure contains focus change information and is used for HVN_FOCUS
   notifications.

Definition:
   
   typedef struct {
      int            posElem;
      HVFocusType    nType;
      int            nState;
   } HViewFocus;

Members:
   
   posElem
      position of focus item, -1 when no focus item

   nType
      identifies type of object obtaining focus

   nState
      state of check box or radio control, only used for HVFOCUS_CHECKBOX and 
      HVFOCUS_RADIO focus types 

Comments:
    None

See Also:
    HVFocusType

===========================================================================

HViewNotify

Description:
   
   This data structure containing notification information and is passed by reference
to application-defined notification callback function.

Definition:
   
   typedef struct {
      HVNotifyCode   code;
      IHtmlViewer *  phv;
      union {
         HViewJump   jump;
         HViewJump   submit;
         int         nDone;
         HViewFocus  focus;
      } u;
   } HViewNotify;


Members:

   code
      notification code

   phv
      identifies IHtmlViewer that sent the notification

   u
      union w/ the following members

         jump        used for HVN_JUMP
   
         submit      used for HVN_SUBMIT
   
         nDone       used for HVN_DONE
   
         focus       used for HVN_FOCUS

Comments:
    None

See Also:
    HVNotifyCode
    HViewFocus
    IHTMLVIEWER_SetNotifyFn()

===========================================================================

HtmlViewer Option IDs:

Description:
   
   Constants used to get and set IHtmlViewer operating options.  Option 
   definitions are grouped into types to help identify the type of argument 
   (cast to type HVOpt in func prototype).  

Definition:
   
   Type 'integer' options:
      HVOPT_N_INDENT:      basic indent
      HVOPT_N_VSPACE:      basic paragraph spacing
      HVOPT_N_SBWIDTH:     scrollbar width
      HVOPT_N_ERROR:       HVERR_... flags           (get only)

   Type 'RGBVAL' options
      HVOPT_RGB_TEXT:      default text color
      HVOPT_RGB_LINK:      link text color

   Type 'char*' options
      HVOPT_SZ_TYPE:       MIME type and/or charset  (set only)
      HVOPT_SZ_URL:        base URL                  (set only)

   Type '{interface}*' options
      HVOPT_PTR_IWEB:      IHtmlViewer uses IWeb interface to obtain sub-objects


Comments:
   
   The above option ID constants are used in the 'nID' argument of the 
   SetOption() and GetOption() functions.  


===========================================================================

HVOpt

Description:
   
   Type of value passed to IHTMLVIEWER_GetOption() and  IHTMLVIEWER_SetOption() is actually a void 
   pointer.  

Definition:
   
   typedef void *HVOpt;

Comments:
   
 See Also:
IWeb Interface for  WEBOPT_*  types



===========================================================================

===========================================================================*/


#endif  //_AEEHTMLVIEWER_H

