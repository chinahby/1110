/*
  ========================================================================

  FILE:  AEEHtmlViewModel.h
  
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
#ifndef __AEEHTMLVIEWMODEL_H__
#define __AEEHTMLVIEWMODEL_H__

#include "AEEWModel.h"

#include "AEEIID_HTMLVIEWMODEL.BID"

// model event codes base
#define EVT_HVM_BASE    0x00001210


// GetElemText / GetAttrValue return values:
#define HVM_NOTFOUND       -1
#define HVM_BADELEMENT     -2
#define HVM_BADATTR        -3
#define HVM_NOVALUE        -4



/////////////////////////////////////////////////////////////////////////////
// IHtmlViewModel events

#define EVT_HVM_DONE          (EVT_HVM_BASE + 0) // done loading base document (objects/images may still be pending)
#define EVT_HVM_PAGEDONE      (EVT_HVM_BASE + 1) // first visible page finished loading including objects/images
#define EVT_HVM_CONTENTDONE   (EVT_HVM_BASE + 2) // document finished loaded including objects/images
#define EVT_HVM_JUMP          (EVT_HVM_BASE + 3) // user selected link
#define EVT_HVM_SUBMIT        (EVT_HVM_BASE + 4) // user submitted form
#define EVT_HVM_FOCUS         (EVT_HVM_BASE + 5) // focus changed

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

typedef unsigned HVFocusType;


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


// HtmlViewEvent 
typedef struct HtmlViewEvent {
   ModelEvent     base;
   union {
      HViewJump   jump;        // EVT_HVM_JUMP
      HViewJump   submit;      // EVT_HVM_SUBMIT
      int         nDone;       // EVT_HVM_DONE
      HViewFocus  focus;       // EVT_HVM_FOCUS
   } u;

} HtmlViewEvent;



/////////////////////////////////////////////////////////////////////////////
// IHtmlViewModel interface

typedef struct IHtmlViewModel IHtmlViewModel;


#define INHERIT_IHtmlViewModel(iname) \
   INHERIT_IModel(iname); \
   boolean  (*IsBusy)      (iname *po);\
   int32    (*FindElem)    (iname *po, const char *pszElem, int32 nIndex);\
   int32    (*GetElemText) (iname *po, int32 nElem, char *pchBuf, int32 cbBuf);\
   int32    (*GetAttrValue)(iname *po, int32 nElem, const char *pszAttr,  char *pchBuf, int32 cbBuf);\
   void     (*RestoreState)(iname *po, const char *pszState);\
   int      (*SaveState)   (iname *po, char *pszBuffer, int cbBuffer)


AEEINTERFACE(IHtmlViewModel) {
   INHERIT_IHtmlViewModel(IHtmlViewModel);
};


#define IHTMLVIEWMODEL_AddRef(p)              AEEGETPVTBL((p),IHtmlViewModel)->AddRef(p)
#define IHTMLVIEWMODEL_Release(p)             AEEGETPVTBL((p),IHtmlViewModel)->Release(p)
#define IHTMLVIEWMODEL_QueryInterface(p,c,d)  AEEGETPVTBL((p),IHtmlViewModel)->QueryInterface(p,c,d)
#define IHTMLVIEWMODEL_AddListener(p,pl)      AEEGETPVTBL((p),IHtmlViewModel)->AddListener(p,pl)
#define IHTMLVIEWMODEL_Notify(p,pe)           AEEGETPVTBL((p),IHtmlViewModel)->Notify(p,pe)
#define IHTMLVIEWMODEL_IsBusy(p)              AEEGETPVTBL((p),IHtmlViewModel)->IsBusy(p)
#define IHTMLVIEWMODEL_FindElem(p,psz,n)      AEEGETPVTBL((p),IHtmlViewModel)->FindElem(p,psz,n)
#define IHTMLVIEWMODEL_GetElemText(p,n,pb,cb) AEEGETPVTBL((p),IHtmlViewModel)->GetElemText(p,n,pb,cb) 
#define IHTMLVIEWMODEL_GetAttrValue(p,n,psz,pb,cb) AEEGETPVTBL((p),IHtmlViewModel)->GetAttrValue(p,n,psz,pb,cb)
#define IHTMLVIEWMODEL_RestoreState(p,psz)    AEEGETPVTBL((p),IHtmlViewModel)->RestoreState(p,psz)
#define IHTMLVIEWMODEL_SaveState(p,pb,cb)     AEEGETPVTBL((p),IHtmlViewModel)->SaveState(p,pb,cb)


// safe upcast
static __inline IModel *IHTMLVIEWMODEL_TO_IMODEL(IHtmlViewModel *p) { 
   return (IModel *)p; 
}



static __inline void IHTMLVIEWMODEL_NotifyJump(IHtmlViewModel *me, int posElem, const char *pszURL) {
   HtmlViewEvent ev;
   ZEROAT(&ev);
   ev.base.evCode    = EVT_HVM_JUMP;
   ev.u.jump.posElem = posElem;
   ev.u.jump.pszURL  = pszURL; 
   IHTMLVIEWMODEL_Notify(me, (ModelEvent*)&ev);
}

static __inline void IHTMLVIEWMODEL_NotifySubmit(IHtmlViewModel *me, int posElem, 
                                                 const char *pszURL, const char *pszMethod,
                                                 const char *pszData) {
   HtmlViewEvent ev;
   ZEROAT(&ev);
   ev.base.evCode        = EVT_HVM_SUBMIT;
   ev.u.submit.posElem   = posElem;
   ev.u.submit.pszURL    = pszURL; 
   ev.u.submit.pszMethod = pszMethod; 
   ev.u.submit.pszData   = pszData; 
   IHTMLVIEWMODEL_Notify(me, (ModelEvent*)&ev);
}

static __inline void IHTMLVIEWMODEL_NotifyFocus(IHtmlViewModel *me, int posElem, 
                                                HVFocusType nType, int nState) {
   HtmlViewEvent ev;
   ZEROAT(&ev);
   ev.base.evCode       = EVT_HVM_FOCUS;
   ev.u.focus.posElem   = posElem;
   ev.u.focus.nType     = nType;
   ev.u.focus.nState    = nState;
   IHTMLVIEWMODEL_Notify(me, (ModelEvent*)&ev);
}


static __inline void IHTMLVIEWMODEL_NotifyEvent(IHtmlViewModel *me, int code) {
   HtmlViewEvent ev;
   ZEROAT(&ev);
   ev.base.evCode = code;
   IHTMLVIEWMODEL_Notify(me, (ModelEvent*)&ev);
}


static __inline void IHTMLVIEWMODEL_NotifyScrollV(IHtmlViewModel *me, uint16 range,
                                                  uint16 visible, uint16 position) {
   ScrollEvent ev;
   ZEROAT(&ev);
   ev.base.evCode = EVT_MDL_SCROLL_CHANGE;
   ev.bVertical   = 1;
   ev.range       = range;
   ev.visible     = visible;
   ev.position    = position;
   IHTMLVIEWMODEL_Notify(me, (ModelEvent*)&ev);
}

static __inline void IHTMLVIEWMODEL_NotifyDone(IHtmlViewModel *me, int nCode) {
   HtmlViewEvent ev;
   ZEROAT(&ev);
   ev.base.evCode    = EVT_HVM_DONE;
   ev.u.nDone         = nCode;
   IHTMLVIEWMODEL_Notify(me, (ModelEvent*)&ev);
}

#define IHTMLVIEWMODEL_NotifyScroll   IHTMLVIEWMODEL_NotifyScrollV



/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
IHtmlViewModel Interface ID

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the widget framework to indicate the model that a given object
   will use to describe the data associated with that object.  

   The model interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as the base interface for all model interfaces.

   The HtmlViewModel is identified by the following interface ID.

Definition:

Identifier              Definition
----------              -----------------------------------
AEEIID_HTMLVIEWMODEL    Identifies the view model interface
   
Comments:
   None

See Also:
   IHtmlViewModel Interface
   IWIDGET_GetModel()


IHtmlViewModel Events: (@@@ details TBD)
---------------------

EVT_HVM_DONE          - done loading base document (objects/images may still be pending)

EVT_HVM_PAGEDONE      - first visible page finished loading including objects/images

EVT_HVM_CONTENTDONE   - document finished loaded including objects/images

EVT_HVM_JUMP          - user selected link

EVT_HVM_SUBMIT        - user submitted form

EVT_HVM_FOCUS         - focus has changed



===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IHtmlViewModel Interface

Description:
   IHtmlViewModel is inherited from the IModel interface and is used to input data into
   a document viewer such as the HTML widget. This model only contains applet
   side methods (for inputting data), the widget to model interface is undefined 
   and uses private methods for data access.
   
=======================================================================

IHTMLVIEWMODEL_AddRef()

Description:

   This function increments the reference count of the IHtmlViewModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IHTMLVIEWMODEL_AddRef(IHtmlViewModel *p);

Parameters:
===pre>
   p:  Pointer to the IHtmlViewModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IHtmlViewModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IHTMLVIEWMODEL_Release()
   
=======================================================================
IHTMLVIEWMODEL_Release()

Description:

   This function decrements the reference count of the IHtmlViewModel interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IHTMLVIEWMODEL_Release(IHtmlViewModel *p);

Parameters:
===pre>
   p:  Pointer to the IHtmlViewModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IHtmlViewModel interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IHTMLVIEWMODEL_AddRef()

=======================================================================

IHTMLVIEWMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IHTMLVIEWMODEL_QueryInterface(IHtmlViewModel *p, AEECLSID c, void **d);

Parameters:
===pre>
   p:  [in]  Pointer to the IHtmlViewModel interface object.
   c:  [in]  A globally unique id that identifies the interface to be queried.
   d:  [out] Address of a memory location that will receive the requrested 
             interface pointer. If the queried item is not available, this 
             pointer is returned as NULL.
===/pre>

Return Value:
===pre>
   SUCCESS          - The requested interface was found and has been returned
                      in the 'd' parameter.
   ECLASSNOTSUPPORT - The requested interface is not supported.
===/pre>

Comments:  
   The pointer passed in the 'd' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper
   level of indirection is used in calling the function.

Side Effects: 
   Upon successful retrieval of an interface pointer, that interface is returned with
   its reference count incremented.  The caller is responsible for releasing this object
   at some point in the future.

See Also:
   IQI_QueryInterface()
   
==========================================================================

IHTMLVIEWMODEL_AddListener()

Description:
   This function allows client objects to attach a listener to the given model.  The 
   listener includes a callback function that will be called whenever the model has 
   changed state.  

Prototype:
   int IHTMLVIEWMODEL_AddListener(IHtmlViewModel *p, ModelListener *pl); 

Parameters:
===pre>
   p:   Pointer to the IHtmlViewModel interface object.
   pl:  Pointer to the ModelListener that will be attached to the object.  The ModelListener will
        contain the PFNLISTENER callback function that will be called for IHtmlViewModel events.
===/pre>
   
Return Value:
===pre>
   SUCCESS          - Listener successfully added
   EBADPARM         - Invalid argument
   ENOMEMORY        - Insufficient memory
===/pre>

Comments:  
   There is no guarantee that the listeners will be called in order.

Side Effects: 
   None

See Also:
   ModelListener
   PFNLISTENER

==========================================================================

IHTMLVIEWMODEL_IsBusy

Description:
   This function returns a boolean indicating whether the viewer is busy
   loading a document.

Prototype:
   boolean IHTMLVIEWMODEL_IsBusy(IHtmlViewModel *po);

Parameters:
===pre>
   p:   Pointer to the IHtmlViewModel interface object.
===/pre>
   
Return Value:
===pre>
   type: boolean 
   
   TRUE     Viewer is busy loading a document
   FALSE    Viewer is not busy
===/pre>

Comments:  
   None

Side Effects: 
   None

See Also:

==========================================================================

IHTMLVIEWMODEL_FindElem

Description:
   This function finds an element, given the element name and the number of 
   the occurrence.  

Prototype:
   int32 IHTMLVIEWMODEL_FindElem(IHtmlViewModel *po, const char *pszElem, int32 nIndex);

Parameters:
===pre>
   p:       Pointer to the IHtmlViewModel interface object.
   pszElem: Name of the element to find, which is a zero-terminated ASCII string (case 
            insensitive).  
   nIndex:  Which occurrence to find (0 => first element, 1=> second element, and so on).            
===/pre>
   
Return Value:
===pre>
   A non-negative number corresponding to the location of the element within the document,
   if found
   
   HVM_NOTFOUND, if pszElem was not found.
   
   HVM_BADELEMENT, if pszElem was not a recognized element name. Only recognized
   elements can be inspected
===/pre>

Comments:  
   None

Side Effects: 
   None

See Also:

==========================================================================

IHTMLVIEWMODEL_GetElemText

Description:
   This function obtains the text content of an element.  Contained elements 
   are ignored, and separate sections of text content (including that of 
   sub-elements) are concatenated.  

Prototype:
   int32 IHTMLVIEWMODEL_GetElemText(IHtmlViewModel *po, int32 nElem, char *pchBuf, int32 cbBuf);

Parameters:
===pre>
   p:       Pointer to the IHtmlViewModel interface object.
   nElem    The location of the element in the document.  This can be obtained from 
            IHTMLVIEWMODEL_FindElem().  
   pchBuf   Pointer to the start of a buffer to hold the result (which is a 
            zero-terminated string) 
   cbBuf    Size of the buffer, in bytes
===/pre>
   
Return Value:
===pre>
   HVM_BADELEMENT, if failure.  This occurs only when the nElemPos value is invalid.  
   
   Length of the resulting string, if successful. 
   
   0(zero), if an empty element, or an element that contains no text is present.  
   
   A value larger than cbBuf may be returned, but only cbBuf bytes are written to 
   the buffer in that case.  
===/pre>

Comments:  
   None

Side Effects: 
   None

See Also:
   IHTMLVIEWMODEL_FindElem

==========================================================================

IHTMLVIEWMODEL_GetAttrValue

Description:
   This function obtains the value of an attribute of an element.  

Prototype:
   int32 IHTMLVIEWMODEL_GetAttrValue(IHtmlViewModel *po, int32 nElem, const char *pszAttr, char *pchBuf, int32 cbBuf);

Parameters:
===pre>
   p:       Pointer to the IHtmlViewModel interface object.
   nElem:   Location of the element in the document
   pszAttr: Name of attribute, zero-terminated ASCII string (case insensitive)
   pchBuf:  Pointer to the start of a buffer to hold the result (which is a 
            zero-terminated string) 
   cbBuf:   Size of the buffer, in bytes
===/pre>
   
Return Value:
===pre>
   HVM_NOTFOUND, attribute is not found
   
   HVM_BADELEMENT, nElemPos was invalid.
   
   HVM_BADATTR, pszAttr is not a recognized attribute name. Only recognized
   attributes are available for query.
   
   HVM_NOVALUE, attribute was specified, but no value was assigned. 
   
   Non-negative number corresponding to the length of the string copied into pchBuf.
   
   A value larger than cbBuf may be returned, but only cbBuf bytes are written to the
   buffer in that case.
===/pre>

Comments:  
   None

Side Effects: 
   None

See Also:
   IHTMLVIEWMODEL_FindElem

==========================================================================

IHTMLVIEWMODEL_RestoreState

Description:
   This function restores the HTML viewer state (such as scroll position) to 
   a state represented by a string returned by IHTMLVIEWMODEL_SaveState()

Prototype:
   void IHTMLVIEWMODEL_RestoreState(IHtmlViewModel *po, const char *pszState);

Parameters:
===pre>
   p:          Pointer to the IHtmlViewModel interface object.
   pszState:   String returned by IHTMLVIEWMODEL_SaveState() which represents a previous 
               view state.  
===/pre>
   
Return Value:
   None

Comments:  
   None

Side Effects: 
   None

See Also:
   IHTMLVIEWMODEL_SaveState

==========================================================================

IHTMLVIEWMODEL_SaveState

Description:
   This function returns a string that represents the current viewer state.  
   This string may be passed to IHTMLVIEWMODEL_RestoreState() to restore the 
   viewer state.  

Prototype:
   int IHTMLVIEWMODEL_SaveState(IHtmlViewModel *po, char *pszBuffer, int cbBuffer);

Parameters:
===pre>
   p:          Pointer to the IHtmlViewModel interface object.
   pszBuffer   Pointer to start of a buffer to hold the result (which will be a 
               zero-terminated string).  
   cbBuffer    size of the buffer, in bytes
===/pre>
   
Return Value:
===pre>
   The length of the result string including zero terminator.
   
   NOTE: if either pszBuffer==NULL or cbBuffer==0, the result string will not 
   be written but the required buffer size will be returned.  
===/pre>

Comments: 
   None 

Side Effects: 
   None

See Also:

==========================================================================
*/


#endif   //__AEEHTMLVIEWMODEL_H__

