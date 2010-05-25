/*
  ========================================================================

  FILE:  AEEForm.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Basic form definition

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEFORM_H__
#define __AEEFORM_H__

#include "AEE.h"
#include "AEEWidget.h"
#include "AEEHandler.h"
#include "AEEDecorator.h"
#include "AEEXYContainer.h"

#include "bid/AEEIID_FORM.bid"

#include "bid/AEECLSID_FORM.bid"

                                 
#define PROP_FORM          0x5000

#define WID_FORM           (PROP_FORM + 0)
#define WID_TITLE          (PROP_FORM + 1)
#define WID_SOFTKEYS       (PROP_FORM + 2)
#define WID_BACKGROUND     (PROP_FORM + 3)
#define WID_DECORATOR      (PROP_FORM + 4)
#define WID_PROGRESS       (PROP_FORM + 5)
#define WID_LIST           (PROP_FORM + 6)
#define WID_LISTITEM       (PROP_FORM + 7)
#define WID_CONTAINER      (PROP_FORM + 8)
#define WID_BACKDROP       (PROP_FORM + 9)
#define WID_STATIC         (PROP_FORM + 10)
#define WID_SCROLLBAR      (PROP_FORM + 11)

#define FID_ACTIVE         (PROP_FORM + 100)
#define FID_ROOT           (PROP_FORM + 101)
#define FID_THEME          (PROP_FORM + 102)
#define FID_THEME_BASENAME (PROP_FORM + 103)
#define FID_THEME_FNAME    (PROP_FORM + 104) 
#define FID_THEME_FILE     (PROP_FORM + 105) 
#define FID_TITLE          (PROP_FORM + 106)
#define FID_TITLEIMAGE     (PROP_FORM + 107)
#define FID_BACKGROUND     (PROP_FORM + 108)
#define FID_PREFRECT       (PROP_FORM + 109)

#define FID_TEXT           (PROP_FORM + 110)
#define FID_PROGRESS       (PROP_FORM + 111)
#define FID_LISTMODEL      (PROP_FORM + 112)
#define FID_ARRAYMODEL     (PROP_FORM + 113)
#define FID_LISTINDEX      (PROP_FORM + 114)
#define FID_VIEWMODEL      (PROP_FORM + 115)
#define FID_FORMMODEL      (PROP_FORM + 116)
#define FID_SHELL          (PROP_FORM + 117)
#define FID_MENUMODEL      (PROP_FORM + 118)

#define FID_ISPOPUP        (PROP_FORM + 120)
#define FID_DISPLAY        (PROP_FORM + 121)
#define FID_FADEMS         (PROP_FORM + 122)
#define FID_VISIBLE        (PROP_FORM + 123)

#define FID_WPROPS         (PROP_FORM + 150)

#define FID_SOFTKEY1       (PROP_FORM + 200)
#define FID_SOFTKEY2       (PROP_FORM + 201)

#define FID_SELECTHANDLER  (PROP_FORM + 302)
#define FID_CANCELKEY      (PROP_FORM + 303)

#define PROP_FORM_OEM      0x6000

#define PROP_FORM_USER     0x8000


// Forms events
#define EVT_FORM_POP       0x900
#define EVT_FORM_CANCEL    0x901


typedef struct IForm       IForm;
typedef struct IRootForm   IRootForm;

/////////////////////////////////////////////////////////////////
// Form interface

typedef void    (*PFNSELECT)      (void *pv, int idOption);

typedef struct SelectHandler {
   PFNSELECT   pfn;  // select callback function
   void *      pv;   // select context pointer
} SelectHandler;


#define INHERIT_IForm(iname) \
   INHERIT_IHandler(iname)

AEEINTERFACE(IForm) {
   INHERIT_IForm(IForm);
};

#define IFORM_AddRef(p)                   AEEGETPVTBL(p,IForm)->AddRef(p)
#define IFORM_Release(p)                  AEEGETPVTBL(p,IForm)->Release(p)
#define IFORM_QueryInterface(p,c,d)       AEEGETPVTBL(p,IForm)->QueryInterface(p,c,d)
#define IFORM_HandleEvent(p,e,w,d)        AEEGETPVTBL(p,IForm)->HandleEvent(p,e,w,d)
#define IFORM_SetHandler(p,pd)            AEEGETPVTBL(p,IForm)->SetHandler(p,pd)

static __inline int IFORM_GetProperty(IForm *po, uint16 propId, uint32 pVal) {
   return ((0 != IFORM_HandleEvent(po, EVT_WDG_GETPROPERTY, propId, (uint32)pVal)) ? SUCCESS : EFAILED);
}
static __inline int IFORM_SetProperty(IForm *po, uint16 propId, uint32 val) {
   return ((0 != IFORM_HandleEvent(po, EVT_WDG_SETPROPERTY, propId, val)) ? SUCCESS : EFAILED);
}

static __inline void IFORM_Activate(IForm *po) {
   IFORM_SetProperty(po, FID_ACTIVE, TRUE);
}
static __inline void IFORM_Deactivate(IForm *po) {
   IFORM_SetProperty(po, FID_ACTIVE, FALSE);
}

static __inline int IFORM_IsVisible(IForm *po, boolean *bVisible) {
   return IFORM_GetProperty(po, FID_VISIBLE, (uint32)bVisible);
}

static __inline int IFORM_GetWidget(IForm *po, uint16 wid, IWidget **ppo) {
   return IFORM_HandleEvent(po, EVT_WDG_GETPROPERTY, wid, (uint32)ppo) ? 0 : ECLASSNOTSUPPORT;
}
static __inline int IFORM_SetWidget(IForm *po, uint16 wid, IWidget *ppo) {
   return IFORM_HandleEvent(po, EVT_WDG_SETPROPERTY, wid, (uint32)ppo) ?  SUCCESS : ECLASSNOTSUPPORT;
}

static __inline int IFORM_GetRootForm(IForm *po, IRootForm **ppo) {
   return IFORM_GetProperty(po, FID_ROOT, (uint32)ppo);
}

static __inline int IFORM_GetShell(IForm *po, IShell **ppo) {
   return IFORM_GetProperty(po, FID_SHELL, (uint32)ppo);
}

#define IFORM_GetTitleWidget(p,w)         IFORM_GetWidget(p, WID_TITLE, w)
#define IFORM_GetSoftkeyWidget(p,w)       IFORM_GetWidget(p, WID_SOFTKEYS, w)
#define IFORM_GetBGWidget(p,w)            IFORM_GetWidget(p, WID_BACKGROUND, w)
#define IFORM_GetProgressWidget(p,w)      IFORM_GetWidget(p, WID_PROGRESS, w)

static __inline int IFORM_GetDecoratorWidget(IForm *po, IDecorator **ppo) {
   return IFORM_GetWidget(po, WID_DECORATOR, (IWidget **)ppo);
}

typedef struct FormRes {
   uint16   id;
   void *   pdata;
} FormRes;


static __inline int IFORM_SetRes(IForm *po, uint16 fid, const char *resFile, uint16 id) {
   FormRes fr;
   fr.id    = id;
   fr.pdata = (void*)resFile;
   return IFORM_SetProperty(po, fid, (uint32)&fr);
}

static __inline int IFORM_SetPtr(IForm *po, uint16 fid, void *ptr) {
   FormRes fr;
   fr.id = 0;
   fr.pdata = ptr;
   return IFORM_SetProperty(po, fid, (uint32)&fr);
}

static __inline int IFORM_GetTextPtr(IForm *po, uint16 fid, AECHAR **ppo) {
   return IFORM_GetProperty(po, fid, (uint32)ppo);
}
static __inline int IFORM_SetText(IForm *po, uint16 fid, const AECHAR *txt) {
   return IFORM_SetPtr(po, fid, (void *)txt);
}

#define IFORM_SetResText(p,f,r,i) IFORM_SetRes(p,f,r,i)
#define IFORM_SetTitle(p,t)       IFORM_SetText(p,FID_TITLE, t)
#define IFORM_SetSoftkey(p,i,t)   IFORM_SetText(p,i,t)

static __inline int IFORM_GetBGImage(IForm *po, IImage **ppi) {
   return IFORM_GetProperty(po, FID_BACKGROUND, (uint32)ppi);
}
static __inline int IFORM_SetBGImage(IForm *po, IImage *piImage) {
   return IFORM_SetPtr(po, FID_BACKGROUND, (void *)piImage);
}
static __inline int IFORM_SetResBGImage(IForm *po, const char *resFile, uint16 resid) {
   return IFORM_SetRes(po, FID_BACKGROUND, resFile, resid);
}


static __inline int IFORM_GetTitleImage(IForm *po, IImage **ppi) {
   return IFORM_GetProperty(po, FID_TITLEIMAGE, (uint32)ppi);
}
static __inline int IFORM_SetTitleImage(IForm *po, IImage *piImage) {
   return IFORM_SetPtr(po, FID_TITLEIMAGE, (void *)piImage);
}
static __inline int IFORM_SetResTitleImage(IForm *po, const char *resfile, uint16 id) {
   return IFORM_SetRes(po, FID_TITLEIMAGE, resfile, id);
}

static __inline boolean IFORM_GetIsPopup(IForm *po) {
   boolean bIsPopup = 0;
   IFORM_GetProperty(po, FID_ISPOPUP, (uint32)&bIsPopup);
   return bIsPopup;
}

static __inline int IFORM_ApplyTheme(IForm *po) {
   return IFORM_SetProperty(po, FID_THEME, (uint32)NULL);
}

static __inline int IFORM_SetProgress(IForm *po, int8 percent) {
   return IFORM_SetProperty(po, FID_PROGRESS, percent);
}

static __inline boolean IFORM_IsActive(IForm *po) {
   boolean result = FALSE;
   IFORM_GetProperty(po, FID_ACTIVE, (uint32)&result);
   return result;
}

static __inline int IFORM_SetSoftkeys(IForm *po, const char *pszResFile, 
                                      uint16 idSoftkey1, uint16 idSoftkey2) {
   int nErr = SUCCESS;
   if (idSoftkey1) {
      nErr |= IFORM_SetRes(po, FID_SOFTKEY1, pszResFile, idSoftkey1);
   } else {
      nErr |= IFORM_SetText(po, FID_SOFTKEY1, NULL);
   }
   if (idSoftkey2) {
      nErr |= IFORM_SetRes(po, FID_SOFTKEY2, pszResFile, idSoftkey2);
   } else {
      nErr |= IFORM_SetText(po, FID_SOFTKEY2, NULL);
   }
   return nErr;
}

static __inline int IFORM_SetSelectHandler(IForm *po, PFNSELECT pfnHandler, void *pv) {
   SelectHandler sh;
   sh.pfn   = pfnHandler;
   sh.pv    = pv;
   return IFORM_SetProperty(po, FID_SELECTHANDLER, (uint32)&sh);
}

static __inline int IFORM_SetCancelKey(IForm *po, uint16 wKey) {
   return IFORM_SetProperty(po, FID_CANCELKEY, (uint32)wKey);
}

static __inline int IFORM_GetCancelKey(IForm *po, uint16 *pwKey) {
   return IFORM_GetProperty(po, FID_CANCELKEY, (uint32)pwKey);
}

static __inline int IFORM_SetThemeBaseName(IForm *po, char *name) {
   return IFORM_SetProperty(po, FID_THEME_BASENAME, (uint32)name);
}

static __inline int IFORM_GetThemeBaseName(IForm *po, char **pname) {
   return IFORM_GetProperty(po, FID_THEME_BASENAME, (uint32)pname);
}


/////////////////////////////////////////////////////////////////
// Form model events

#define EVT_MDL_FORM_CHANGE      0x00002000

// sent from FormModel (IModel)
typedef struct FormEvent {
   ModelEvent  base;
   uint32      dwItemMask;
   IForm*      piForm;
} FormEvent;


// item mask bits
#define FORMITEM_BACKGROUND      1 // Background of form has changed
#define FORMITEM_TITLE           2 // Title content of form has changed
#define FORMITEM_SOFTKEY         4 // Softkey info of form has changed
#define FORMITEM_WIDGET          8 // Primary widget in form has changed
#define FORMITEM_THEME_BASENAME 16 // Theme info of form has changed
#define FORMITEM_STACK          32 // RootForm form stack has changed
#define FORMITEM_CLIENT         64 // RootForm client area has changed
#define FORMITEM_ALL            ((uint32)-1)

static __inline int IFORM_GetFormModel(IForm *po, IModel **ppo) {
   return IFORM_GetProperty(po, FID_FORMMODEL, (uint32)ppo);
}

/////////////////////////////////////////////////////////////////////////////

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Form Interface and Class IDs

Description:
   Each object in the forms interface is identified by a unique interface ID.
   These IDs are used throughout the forms and widgets framework to distinguish
   between the various object and interface types, such as forms, widgets and
   containers.

   The base form interface supports both an interface and the creation of class
   object instances.  The base form interface or a new instance of the base form
   class object may be created by calling ISHELL_CreateInstance() and passing in
   the desired class id of the object.
   
   The base form interface recognizes the following set of class and interface IDs.

Definition:

Identifier      Definition
----------      -----------------------------------
AEEIID_FORM:    Identifies the base form interface
AEECLSID_FORM:  Identifies the class ID for the base form
   
Comments:
   None

See Also:
   IForm Interface
   ISHELL_CreateInstance()


=============================================================================

Form Events

Description:
   The forms framework introduces a small set of BREW events that serve as a notification
   mechanisms for removing forms from the form stack.  These form specific events will be
   sent to objects in addition to the normal set of BREW interface events such as key
   press events and other UI related events.  A form's event handler should be prepared to
   handle these events in whatever manner is appropriate for the behavior of that object.
   For example, the EVT_WDG_GETPROPERTY event may be sent to a form to retrieve various
   attributes or characteristics that apply to forms.  To illustrate, a form that contains
   a display banner of some sort would likely respond to an EVT_WDG_GETPROPERTY event
   specifying WID_TITLE as the requested property type, by returning the widget responsible
   for displaying the form's display banner.

   The table below lists the AEE event codes supported by the forms framework, identifying
   the data structure that will be sent with the event.

Definition:
AEE Event Code    Description
--------------    ------------------------------------------------------
EVT_FORM_POP:     The base form event handler responds to this event by instructing the root
                  form to remove this form from the form stack.

EVT_FORM_CANCEL:  The base form event handler responds to this event by sending an EVT_FORM_POP
                  event back through the form event handling hierarchy.  This gives hooked
                  event handlers an opportunity to handle the event, whereas the event would
                  eventually make it back to the base form event handler where the root form
                  would remove this form from the form stack.                  

Members:
   None
   
Comments:
   In addition to the forms specific events cited above, the forms framework supports many of
   the events defined to support widgets (i.e. EVT_WDG_SETPROPERTY, EVT_WDG_GETPROPERTY, etc).

See Also:
   None
   
=============================================================================

Form Model Events

Description:
   The form interface defines a very small set of model events, used by the form interface
   to notify listening objects of changes to the form.  For example, an object that has
   registered as a listener with the form model will receive a EVT_MDL_FORM_CHANGE if any of
   several characteristics of the form has changed -- the title, the title image, background
   color, softkey text, etc...  Form model events are sent to listening objects in a FormEvent
   data structure

   The table below lists the AEE event codes supported by the form model, describing the
   circumstances that generate each event and identifying the data structure that will be sent
   with the notification.

Definition:
AEE Event Code        Description
--------------        ------------------------------------------------------
EVT_MDL_FORM_CHANGE:  The data associated with a form model has changed.  Generally, this means
                      that one or more components of the form has changed such as the title,
                      background or softkeys, as identified by the FormEvent data structure that
                      accompanies this event.  The object receiving this event will generally
                      invalidate itself so that it will be redrawn with the next screen update.  

Members:
   None
   
Comments:
   None

See Also:
   FormEvent
   ModelEvent

=============================================================================

FormEvent

Description:
   This data type extends the base model event to include fields used by models that
   provide data specific to form changes.  For example, an object that is interested
   in changes to a form's title image could register as a client of the form model,
   which monitors changes to the components managed by a particular form.  If the
   form's title image changes, the model would sent events to listening objects in
   a FormEvent, identifying the specific event code (EVT_MDL_FORM_CHANGE) in the
   'base' ModelEvent, while information specific to the event will be stored in the
   remaining fields of the FormEvent data structure.  In this example, the event would
   specify that the FORMITEM_TITLE has changed.

Definition:
   typedef struct FormEvent {
      ModelEvent  base;
      uint32      dwItemMask;
      IForm*      piForm;
   } FormEvent;

Members:
===pre>
   base:        The base model event, which identifies the event code and provides
                the object with the listener it had previously registered with the
                model framework.
   dwItemMask:  Identifies the item (or items) that are affected by the event.  For
                example, when the event code indicates a EVT_MDL_FORM_CHANGE event,
                'dwItemMask' identifies all those form components that have experienced
                a change.  The form components are identified by a set of mutually
                exclusive bits, as follows:
                    FORMITEM_BACKGROUND     :  Form background
                    FORMITEM_TITLE          :  Form title content (either the text, image
                                               or both)
                    FORMITEM_SOFTKEY        :  Form softkey content
                    FORMITEM_WIDGET         :  Form primary widget
                    FORMITEM_THEME_BASENAME :  Theme information utilized by the form
                    FORMITEM_STACK          :  Root form stack
                    FORMITEM_CLIENT         :  Root form client area
                    FORMITEM_ALL            :  All components of the form
   piForm:     Pointer to the form being identified by the event.
===/pre>
   
Comments:
   The following form model events will be passed to listening objects in a FormEvent
   data structure:
===pre>
       EVT_MDL_FORM_CHANGE
===/pre>

See Also:
   Model Events
   ModelEvent

=============================================================================

FormRes

Description:
   This data type defines a descriptor for form data stored in a BREW resource file,
   identifying where the information can be found by file name and resource ID.  This
   information can then easily be stored as a form property to identify the value of
   text, bitmaps and other pieces of data displayed by a form.  For example, the image
   to be displayed in the background of a form could be stored in a resource file, with
   the file name and the ID of the appropriate bitmap resource stored together in a
   FormRes data structure.  A pointer to this structure, in turn, would be stored in
   the FID_BACKGROUND property of the form.

Definition:
   typedef struct FormRes {
      uint16  id;
      void    *pdata;
   } FormRes;

Members:
===pre>
   id:     Identifies the resource ID within the 'pdata' resource file where the
           particular data type can be found.  When 'id' is zero, 'pdata' points to
           the data itself, rather than to a resource file name -- i.e. the data is
           already in memory and is NOT in a resource file.
   pdata:  Pointer to the name of the resource file (encoded as a char * rather than
           an AECHAR) where the data described by 'FormRes' can be found.  However,
           when 'id' is zero, 'pdata' points to the data itself, rather than to the
           name of a resource file.  For example, when FormRes is being used to
           identify a text string, and 'id' is zero, 'pdata' would point to the AECHAR
           that contained the text string.  If 'id' is any other value, 'pdata' would
           contain the name of the resource file containing the string.
===/pre>
   
Comments:
   None

See Also:
   IFORM_SetRes()
   IFORM_SetResText()
   IFORM_SetResBGImage()
   IFORM_SetResTitleImage()

=============================================================================

SelectHandler

Description:
   This data type defines a single selection handler object, containing both a callback
   function and a pointer to private data that will be passed into the callback routine
   each time the selection handler is invoked.  Specialized forms such as the list form
   and popup menu form support the notion of item selection -- list items, in the case
   of the list form, menu items in the case of the popup menu form.  Applications that
   create forms based on these types of specialized forms may hook into the item selection
   events to perform any processing unique to the needs of that form.  For example, a form
   that wishes to perform its own processing in response to a menu item selection could
   register a selection handler that will be called each time the user makes a menu
   selection.

Definition:
   typedef struct SelectHandler {
     PFNSELECT  pfn;
     void *     pv;
  } SelectHandler;

Members:
===pre>
   pfn:  The callback function that the forms framework will call when an item selection
         takes place.
   pv:   Pointer to the private data defined by and returned to the form registering the
         selection handler.  The data pointer will be passed to the selection handler each
         time the 'pfn' routine is called.
   
Comments:
   None

See Also:
   PFNSELECT
   IFORM_SetSelectHandler()

=============================================================================

PFNSELECT

Description:
   This type definition specifies the prototype for the function pointer used as the callback
   function for selection events on a form.  For example, a form that provides a standard list
   mechanism (such as the list form) might identify its own selection callback so that it may
   process the selection in some special way, unique to that form.

Definition:
    typedef void (*PFNSELECT)(void *pv, int idOption);

Members:
===pre>
    pv:        A pointer to private data that is created and owned by the form receiving the
               callback.
    idOption:  An identifier used to disambiguate items in the object that contains the items to be
               selected.  The identifier will vary from form type to form type.  For example, a
               list form would pass the item number of the item that has been selected, while a
               popup menu may pass the menu ID of the particular menu item that has been selected.
               More complex forms may choose to pass in data from an attached model to identify the
               selected item.
===/pre>

Comments:
   Forms typically will provide a pointer to their own object instance in the 'pv' field of the
   SelectHandler data structure when registering their selection handler.  This pointer will then
   be passed into the selection handler in the corresponding 'pv' parameter of the prototype.

   To register a selection callback routine an application would call IFORM_SetSelectHandler(),
   passing in a pointer to its PFNSELECT function and a pointer to the private data it wishes
   to access each time the function is called (basically, the same fields that appear in a
   SelectHandler data structure).
      
See Also:
    SelectHandler
    IFORM_SetSelectHandler()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IForm Interface

Description:
   IForm is a user interface element that allows applications to easily build screens that
   share a common look and feel and abide by a standard appearance theme.  By utilizing forms
   and themes, the visual appearance of an application can very easily be modified to support
   a common user experience across multiple applications.  For example, by using forms and
   themes, all the static widgets of an application can share the same background color or
   level of transparency.  Likewise, each screen in the application can maintain a common
   visual structure -- title area, content, plus a pair of softkeys.
   
   A single form defines structure, organization and behavior for a single screen.  A list of
   contacts in an address book application, a screen laid out like a questionnaire, or even a
   popup menu providing simple commands can all be thought of as separate forms.  In fact,
   BREW provides default form implementations for lists, dialogs and popups, while providing
   the capability for applications to derive their own form implementations.
   
   The base form consists of the following visual characteristics:

===pre>   
       +-----------------------------------+
       |               Title               |  <--------  Title area of the form
       +-----------------------------------+
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       |                                   |
       |                                   |  <--------  Background area of the form
       |                                   |             (occupies the entire extent
       |                                   |              of the display, behind the
       |                                   |              title and softkeys)
       |                                   |
       |                                   |
       +-----------------------------------+
       |    Softkey 1    |    Softkey 2    |  <--------  Softkey area of the form
       +-----------------------------------+
===/pre>

   Both the title and softkey areas of the form are optional.  If either is missing, the client
   may claim that area for its own use.  The title area of the form, when present,
   may contain a static text string, an image, or both an image and text.  The form may display a
   pair of softkeys, each generally corresponding to hardware keys placed immediately below the
   display on the target device.  The background provides a backdrop for the content that will be
   placed onto the form.  This backdrop might be as simple as a subtle shading of color, or it
   could be an elaborate graphic image containing winking dancing girls -- whatever is most
   appropriate for the content and spirit of the application (it is doubtful that an online
   "places of worship locator" would use the winking dancing girls).  The background area covers
   the entire extent of the form's root container and will contain a color or background image
   over which all other form elements will be displayed.  
   
   In addition to its visual characteristics, a form also maintains information about its
   model, its collection of contained widgets, the theme used to render those widgets, and
   a pointer back to the root form that owns this form.  These relationships can be further
   illustrated, as follows:

===pre>   


    +-----------------+                           +-----------------+
    |      Model      | <----+             +----> |      Theme      |
    +-----------------+      |             |      +-----------------+
                             |             |
                             |             |
                        +----|-------------|----+
                        |    |             |    |
                        |    +             +    |
                        |                       |
                        |                       |
                        |         Form          |
                        |                       |
                        |                       |
                        |    +             +    |
                        |    |             |    |
                        +----|-------------|----+
                             |             |
                             |             |
    +-----------------+      |             |      +-----------------+
    |    Root Form    | <----+             +----> |     Widget      |
    +-----------------+                           +-----------------+
===/pre>

   The form's model acts as a conduit through which the form is able to notify registered
   objects of EVT_MDL_FORM_CHANGE events.  The form contains a primary widget, which itself
   is generally a container that will wrap the collection of widgets (text, buttons, images,
   checkboxes, etc) to be presented on the form.  The theme identifies an appearance to be
   applied to each widget contained within the form.  The root form points back to the object
   that manages the layering of the current stack of forms.
   
   An application may choose to employ one form, or it could define multiple forms that the
   user navigates as they use the application.  In either case, whether there is one form or
   many, BREW will always maintain the concept of a "root form" -- which is not really a form
   itself, rather, it is the root entity which organizes all of the forms currently defined
   by an application.  Conceptually, the root form is similar to a window manager in a
   traditional desktop GUI, handling form activation, managing form layering and arbitrating
   events to objects contained within the form.
   
   The root form itself is the structure upon which the forms it manages will be built, as
   illustrated by the diagram below:

===pre>
                                                   Root  Form      
                                            +----------------------+
                                            |                      |
      +------+            Vector <--------------+  Model           |
      |      |             Model            |                      |
      |      |       +---------------+      |                      |
      |      |       |               |      |      Widgets +-------------------+
   +------+  | <--------+  Form 1    |      |                      |           |
   |      |  |       |               |      |                      |           |
   |      |--+       +---------------+      |   + Root Container   |           |
   |      |          |               |      |   |                  |           |
   |      | <-----------+  Form 2    |      +---|------------------+           |
   |      |          |               |          |                              |
   +------+          +---------------+          |                              |
    Forms            |               |          |                              |
                     |       :       |          |                              |
                                                |                              |
                                                |                              |
                                                |                              V
                                                V                  +------------------------+     
                                       +-----------------+         |                        | 
                                       |                 | <- - - - - +  Title widget       |
                                       +-----------------+         |                        |
                                       |                 |         +------------------------+
                                       |                 |         |                        |
                                       |                 | <- - - - - +  Background widget  |
                                       |                 |         |                        |
                                       |                 |         +------------------------+
                                       +-----------------+         |                        |
                                       |        |        | <- - - - - +  Softkey widget     |
                                       +-----------------+         |                        |
                                                                   +------------------------+
===/pre>

   The root form relies on a vector model to manage the "form stack".  The form stack represents
   the display order of all the forms that have been created, with the form on the top of the
   stack being in the foreground of the display.  Once that form is popped off the stack, the next
   form on the stack bubbles up to the top and takes over the display.  The root form can manipulate
   its attached vector model however it chooses, removing forms from within the stack, or creating
   new forms at a particular place on the stack.

   The form stack should never have duplicate forms in it, as properties associated with each form
   are maintaned by the root form, and can differ depending on whether the form is active 
   (on top of the stack), visible (behind a dialog or popup), or completely hidden.  
   
   The vector model that supports the root form stores pointers to those forms that the root form
   creates.  In addition to attaching itself to a vector model, the root form defines the container
   into which the form elements (title, background and softkeys) will be placed.  As a form is
   popped to top of the stack, the root form retrieves the form pointer, updates the title, softkey,
   and background widgets, then populates the root container with the contents of the form.
  
   The following APIs are provided by the form interface to support an object reference counting
   mechanism allowing form objects to manage their own memory instances.
===pre>
       IFORM_AddRef()
       IFORM_Release()
===/pre>
      
   The following API is provided to query an object for another API contract from the object
   in question (in this case, the form interface)
===pre>
       IFORM_QueryInterface()
===/pre>
      
   The following APIs are provided to support event handling to the form object:
===pre>
       IFORM_HandleEvent()
       IFORM_SetHandler()
       IFORM_SetSelectHandler()
===/pre>
      
   The following APIs are provided to set and retrieve various properties of a form object:
===pre>
       IFORM_SetProperty()
       IFORM_GetProperty()
===/pre>
      
   The following APIs are provided to manage activation and deactivation of form objects:
===pre>
       IFORM_Activate()
       IFORM_Deactivate()
       IFORM_IsActive()
===/pre>
      
   The following APIs are provided to set and retrieve the widget that will act as the parent
   for all other widget's to be contained in a form:
===pre>
       IFORM_SetWidget()
       IFORM_GetWidget()
===/pre>
      
   The following API is provided to retrieve the root form that manages a particular form object:
===pre>
       IFORM_GetRootForm()
===/pre>
      
   The following API is provided to retrieve an instance of the BREW shell that is associated with
   a particular form object:
===pre>
       IFORM_GetShell()
===/pre>
      
   The following APIs are provided to retrieve the title, softkey, background, progress and
   decorator widgets associated with a particular form:
===pre>
       IFORM_GetTitleWidget()
       IFORM_GetSoftkeyWidget()
       IFORM_GetBGWidget()
       IFORM_GetProgressWidget()
       IFORM_GetDecoratorWidget()
===/pre>
      
   The following APIs are provided to set the contents of the standard user interface elements
   (title, softkey, background, etc) associated with a form -- either directly, or by taking
   the value from a particular resource file:
===pre>
       IFORM_SetRes()
       IFORM_SetPtr()
       IFORM_SetText()
       IFORM_SetResText()
       IFORM_SetTitle()
       IFORM_SetSoftkey()
       IFORM_SetBGImage()
       IFORM_SetResBGImage()
       IFORM_SetTitleImage()
       IFORM_SetResTitleImage()
       IFORM_SetProgress()
       IFORM_SetSoftkeys()
===/pre>
      
   The following APIs are provided to retrieve the contents of the standard user interface elements
   (title, softkey, background, etc) associated with a form:
===pre>
       IFORM_GetTextPtr()
       IFORM_GetBGImage()
       IFORM_GetTitleImage()
       IFORM_GetIsPopup()
===/pre>
      
   The following APIs are provided to set and retrieve information about the theme that will be used
   by the form to customize the appearance of the objects it will display:
===pre>
       IFORM_SetTheme()
       IFORM_SetThemeBase()
       IFORM_GetThemeBase()
===/pre>
      
   The following APIs are provided to set and retrieve information about the models associated with
   a form:
===pre>
       IFORM_GetFormModel()
===/pre>

Supported Events:
   The base form event handler is responsible for accepting and processing various events that
   identify key presses, manage the form stack and access various form properties.
   
   The events handled by the form interface include:

===pre>
Event                   Description
-----                   ------------------------------------------------------------
EVT_FORM_POP:           The base form event handler responds to this event by instructing the root
                        form to remove this form from the form stack.

EVT_FORM_CANCEL:        The base form event handler responds to this event by sending an EVT_FORM_POP
                        event back through the form event handling hierarchy.  This gives hooked
                        event handlers an opportunity to handle the event, whereas the event would
                        eventually make it back to the base form event handler where the root form
                        would remove this form from the form stack.

EVT_KEY:                The base form event handler responds to the EVT_KEY if the key is AVK_CLR
                        or the key specified by the FID_CANCELKEY property. It sendings an 
                        EVT_FORM_CANCEL event back through the form event handling hierarchy.  

EVT_WDG_SETPROPERTY:    The base form handler responds to this event by attempting to set the
                        property identified by the 'wParam' parameter.  The following properties may
                        be set by the base form event handler:
                      
                            WID_FORM       --  Sets the primary widget associated with the form and
                                               sends a form event to listening objects to notify
                                               them that the primary widget has changed.
                            FID_ROOT       --  Sets the root form that will control this form.
                            FID_ACTIVE     --  Sets the activation state of the form.
                            FID_VISIBLE    --  Sets the visible state of the form.
                            FID_THEME_BASENAME --  Sets the theme basename for this form. Setting this 
                                               property will cause an event to be sent out notifying
                                               listening objects that the basename of the form has 
                                               changed.
                            FID_THEME      --  Requests a form to apply the current theme.
                            FID_BACKGROUND --  Sets the background image for the form and sends a form
                                               event to listening objects to notify them that the
                                               background has changed.
                            FID_TITLE      --  Sets the title text of the form and sends a form event
                                               to listening objects to notify them that the title
                                               content has changed.
                            FID_TITLEIMAGE --  Sets the title image of the form and sends a form event
                                               to listening objects to notify them that the title
                                               content has changed.
                            FID_SOFTKEY1   --  Sets the text of the form's left softkey and sends a form
                                               event to listening objects to notify them that the softkey
                                               content has changed.
                            FID_SOFTKEY2   --  Sets the text of the form's right softkey and sends a form
                                               event to listening objects to notify them that the softkey
                                               content has changed.                                               

EVT_WDG_GETPROPERTY:    The base form handler responds to this event by attempting to retrieve the
                        property identified by the 'wParam' parameter.  The following properties may
                        be retrieved by the base form event handler:
                      
                            WID_FORM       --  Retrieves the primary widget associated with the form, incrementing 
                                               the reference count of the widget.
                            FID_SHELL      --  Retrieves a reference to the IShell object used by the form, 
                                               incrementing the reference count of the IShell object.
                            FID_ROOT       --  Retrieves the root form that will control this form, incrementing 
                                               the reference count of the root form.
                            FID_ACTIVE     --  Retrieves the activation state of the form.
                            FID_VISIBLE    --  Retrieves the visible state of the form
                            FID_THEME_BASE --  Retrieves the theme base used by this form.
                            FID_BACKGROUND --  Retrieves the image this form will use to display its
                                               background, incrementing the reference count of the image.
                            FID_PREFRECT   --  Retrieves the preferred screen rectangle of the form widget. 
                                               Used during activation or showing of the form (FID_ACTIVE, FID_VISIBLE) 
                                               to position and size the widget.
                            FID_ISPOPUP    --  Retrieves TRUE if the form is implemented as a popup, FALSE
                                               if it is not.
                            FID_TITLE      --  Retrieves the title text of the form.
                            FID_TITLEIMAGE --  Retrieves the title image of the form, incrementing 
                                               the reference count of the image.
                            FID_SOFTKEY1   --  Retrieves the text of the form's left softkey.
                            FID_SOFTKEY2   --  Retrieves the text of the form's right softkey.
                            FID_FORMMODEL  --  Retrieves a reference to the form model attached to the form, 
                                               incrementing the reference count of the model.
===/pre>

Properties:
Property            Description
--------            ------------------------------------------------------------
WID_FORM:           This property contains a pointer to the form's primary widget, which serves as
                    the container object for all other widgets contained within the form.
                             
                        Property Value:  IWidget *
                                    
WID_TITLE:          This property contains a pointer to the root form's title widget.  Generally, an
                    application will choose to implement the title widget as a static, image or image
                    static widget to produce the root form's title banner.  An application could,
                    however, implement the title widget using any widget of their choosing.
                              
                        Property Value:  IWidget *
                                    
WID_SOFTKEYS:       This property contains a pointer to the root form's softkey widget.  Though the
                    root form's softkey widget is commonly implemented as a softkey  widget, an
                    application could implement the root form softkey widget using any widget of their
                    choosing.
                             
                        Property Value:  IWidget *
                                    
WID_BACKGROUND:     This property contains a pointer to the root form's background widget.  Generally,
                    an application will choose to implement the background widget as an image widget,
                    which allows the application to place a graphic image into the form's background,
                    or -- since the image widget understands border properties -- create any visual
                    effect inherent in borders (color, shading, transparency, etc) without any image
                    present at all.  Though the root form's background widget is commonly implemented
                    as an image widget, an application could implement the root form background widget
                    using any widget of their choosing.
                             
                        Property Value:  IWidget *
                                    
WID_DECORATOR:      This property contains a pointer to a form's decorator widget for those specialized
                    forms that have derived from the base form.  For example, the list form defines a
                    list widget (which derives from the decorator widget) to manage list data in an
                    expected and consistent manner.  It is important to note that the decorator widget
                    is provided in _addition_ to the form's primary widget accessed by the WID_FORM
                    property.  It is also important to emphasize that the interpretation of the
                    WID_DECORATOR property is at the discretion of a particular form implementation.
                    For example, a specialized form that supports the ability to browse through a
                    list of photographs, displaying a thumbnail of the current selection in an image
                    viewer, might choose to return a pointer to the image viewer in response to queries
                    to get the WID_DECORATOR property, while the list widget used to browse the names
                    of the images might be returned by the WID_LIST property.  The list is just a list,
                    while the image viewer decorates that list.  This distinction is important in light
                    of the prior example in which a pointer to a list widget is returned as the
                    WID_DECORATOR property.
                             
                        Property Value:  IWidget *
                                    
WID_PROGRESS:       This property contains a pointer to a form's progress widget for those specialized
                    forms that have derived from the base form.  For example, the progress dialog is a
                    specialized form that derives from dialogs, popups and -- eventually -- back to
                    forms.  The progress dialog defines a progress widget -- typically to popup a quick
                    dialog to illustrate progress for some time consuming operation.  The WID_PROGRESS
                    property allows applications to access the progress widget that provides this
                    visual feedback.  It is important to note that the progress widget is provided in
                    _addition_ to the form's primary widget accessed by the WID_FORM property.
                             
                        Property Value:  IWidget *
                                    
WID_LIST:           This property contains a pointer to a form's list widget for those specialized
                    forms that have derived from the base form.  For example, the popup menu, which
                    derives from a form presents its menu items using a list widget.  This list
                    widget is stored in the WID_LIST property.  It is important to note that the list
                    widget is provided in _addition_ to the form's primary widget accessed by the
                    WID_FORM property.  Likewise, it is also important to emphasize that the
                    interpretation of the WID_LIST property is at the discretion of a particular form
                    implementation.  For example, a specialized form that supports the ability to
                    browse through a list of photographs, displaying a thumbnail of the current
                    selection in an image viewer, might respond to queries for the WID_LIST property
                    by returning a pointer to the list widget used to browse the names of the photos,
                    while a pointer to the image viewer widget might be returned when asked for
                    the WID_DECORATOR property.  The list is just a list, while the image viewer
                    decorates that list.
                             
                        Property Value:  IWidget *
                                    
WID_LISTITEM:       This property contains a pointer to the widget used to render each item contained
                    in a form's list.  This is, effectively, the widget wrapped within the form's
                    WID_LIST (or WID__DECORATOR, in the case of list forms) property.
                             
                        Property Value:  IWidget *

WID_CONTAINER:      This property contains a pointer to the form's root container, which serves as the
                    container object for all other objects contained within the root form.  This will
                    include the title, background and softkey widgets, as will as the primary widget
                    and all widgets it may contain.
                             
                        Property Value:  IContainer *

WID_BACKDROP:       This property contains a pointer to the widget used to render a form's backdrop
                    image.  Generally, the backdrop is used by those specialized forms that have derived
                    from the base form, such as popups or dialogs. The "backdrop" is distinguished from
                    the "background" as being the visual element that appears to lay behind and beneath
                    the form appearing in the foreground -- again, using a popup dialog that seems to
                    float above the backgro0und display.  The backdrop would appear, in this case, as a
                    subtle wash or shadow effect to emphasize that the dialog or popup is "above" the
                    rest of the display.  The WID_BACKDROP property allows applications to access the
                    backdrop widget that provides this visual feedback.  It is important to note that the
                    backdrop widget is provided in _addition_ to the form's primary widget accessed by the
                    WID_FORM property, as well as the root form's background widget accessed by the
                    WID_BACKGROUND property.
                             
                        Property Value:  IWidget *
                                    
WID_STATIC:         This property contains a pointer to a form's static widget for those specialized
                    forms that have derived from the base form.  For example, the static dialog is a
                    specialized form that derives from dialogs, popups and -- eventually -- back to
                    forms.  The static dialog defines a static widget that displays text messages in
                    a popup dialog.  The WID_STATIC property allows applications to access the static
                    widget that displays the text for the dialog.  It is important to note that the
                    static widget is provided by the specialized form in _addition_ to the form's
                    primary widget accessed by the WID_FORM property.
                             
                        Property Value:  IWidget *

FID_ACTIVE:         This property contains the activation state of the form -- TRUE when the form is
                    active, FALSE when it is not.  Setting the activation state of the form will trigger
                    the form to invalidate itself and be redrawn.  Setting the activation state of the
                    root form will cause only the topmost form on the form stack to be activated or
                    deactivated, and -- on activation -- will further invalidate the root container,
                    causing all the child objects to be redrawn. On activation FID_PREFRECT is requested
                    of the form to get the preferred screen rectangle of the form widget.
                             
                        Property Value:  boolean

FID_VISIBLE:        This property contains the visible state of the dialog -- TRUE when the
                    form may be all or partially visible, FALSE when it is not. Setting the 
                    visible state of the form may trigger an invalidation if the root form 
                    client region has changed.

                    See Also: FORMITEM_CLIENT
                             
                        Property Value:  boolean

FID_ROOT:           This property contains a pointer to the root form that manages a form.  The root
                    form controls the stacking order for a collection of forms.  Requesting the
                    FID_ROOT property of the root form itself will yield undefined results and return
                    FALSE to the requesting client.
                             
                        Property Value:  IRootForm *

FID_THEME:          This property requests a form to apply the current theme to itself. This includes 
                    loading the background image to be used by the form. The base form only supports 
                    setting this property. 
                             
                        Property Value:  None

FID_THEME_BASENAME: This property specifies a string that identifies the base location within the theme file,
                    from which BREW should obtain and apply theme data when rendering the objects in this
                    form.  All named properties and resources used by a form while applying the theme will be 
                    loaded relative to this basename. Setting this property will fire a formmodel event notifying 
                    listening objects that the basename of the form has changed.
                             
                        Property Value:  char *

FID_TITLE:          This property contains a pointer to the text that will be displayed in a form's title
                    area.  The text will be displayed by itself if the title widget is implemented as a
                    static widget, or along with a graphic image if the title widget is an image static
                    widget.  Though the value of the property is a pointer to an AECHAR, the property value
                    must be set via a pointer to a FormRes data structure which identifies the property
                    value as either a resource file reference or as a direct pointer to the
                    string to be displayed by the title widget.
                             
                        Property Value:  AECHAR *

FID_TITLEIMAGE:     This property contains a pointer to the image that will be displayed in a form's
                    title area.  The image will be displayed by itself if the title widget is implemented
                    as an image widget, or along with text if the title widget is an image static widget.
                    Though the value of the property is a pointer to an IImage, the property value must
                    be set via a pointer to a FormRes data structure which identifies the property value
                    as either a resource file reference or as a direct pointer to the image to be
                    displayed by the title widget.
                             
                        Property Value:  IImage *

FID_BACKGROUND:     This property contains a pointer to the image that will be displayed in the background
                    content area of a form or the root form.  Though the value of the property is a pointer
                    to an IImage, the property value must be set via a pointer to a FormRes data structure
                    which identifies the property value as either a resource file reference or as a direct
                    pointer to the IImage to be displayed by the background widget.
                             
                        Property Value:  IImage *

FID_PREFRECT:       This property retrieves the preferred position and size of the form widget. 
                    It is requested during activation or showing of the form (FID_ACTIVE, FID_VISIBLE) to allow the form to indicate 
                    the form widget rectangle (size and position). 

                        Property Value:  AEERect *

FID_TEXT:           This property is used as a trigger to set the text of a form's static text object,
                    from a text resource or string.  This property does not contain any retrievable
                    value and therefore may not be accessed by EVT_WDG_GETPROPERTY.  The text value
                    is passed to a form's event handler in an EVT_WDG_SETPROPERTY event, with 'dwParam' 
                    containing a pointer to a FormRes data structure which identifies a text resource
                    or points to a character string.  For example, a static dialog may change the
                    text of an error message by setting the FID_TEXT property, identifying that the text
                    is contained in resource 100 of a file named "error.txt".
                             
                        Property Value:  FormRes *

FID_PROGRESS:       This property is used as a trigger to set the current progress value of a form.  This
                    property updates the value model associated with a form (in particular, with the
                    progress dialog form) and does not contain any retrievable value.  Therefore, the
                    FID_PROGRESS property.  The updated value of the form's progress widget is passed to
                    a form's event handler in an EVT_WDG_SETPROPERTY event, with 'dwParam' containing
                    the updated progress value.
                             
                        Property Value:  Depends on the value model used by the progress indicator,
                                         though this value usually represents "percent complete".  For
                                         example, hen the value of this property is 80, the standard
                                         progress widget could be redrawn as 80% filled.

FID_LISTMODEL:      This property contains a pointer to the list model that may be attached to a form's
                    list widget, returning that pointer in an IModel pointer passed in the 'dwParam'
                    parameter passed to the form with the EVT_WDG_GETPROPERTY event.

                        Property Value:  IModel *

FID_ARRAYMODEL:     This property contains a pointer to the array model that may be attached to a form's
                    list widget, returning that pointer in an IModel pointer passed in the 'dwParam'
                    parameter passed to the form with the EVT_WDG_GETPROPERTY event.

                        Property Value:  IModel *

FID_LISTINDEX:      This property contains the zero-based index of the list item that possesses the
                    current focus in a list form's list widget.  When setting this property the index
                    is passed in the 'dwParam' of the event and -- if the list is active -- will cause
                    the list to be redrawn with the focused item scrolled into view.

                        Property Value:  int

FID_VIEWMODEL:      This property contains a pointer to the view model that may be attached to an object
                    within a form -- typically, the list widget on a list or popup form.  The property
                    returns that pointer in an IModel pointer passed in the 'dwParam' parameter passed
                    to the form in the EVT_WDG_GETPROPERTY event.
                              
                    Forms do not support attempts to set the FID_VIEWMODEL property.

                        Property Value:  IModel *

FID_FORMMODEL:      This property contains a pointer to the form model that may be attached to a form.
                    The property returns that pointer in an IModel pointer passed in the 'dwParam'
                    parameter passed to the form in the EVT_WDG_GETPROPERTY event.
                              
                    Forms do not support attempts to set the FID_FORMMODEL property.

                        Property Value:  IModel *

FID_SHELL:          This property contains a reference to the IShell object used by the form.  The
                    property returns a pointer to the IShell instance in the 'dwParam' parameter
                    passed to the form with the EVT_WDG_GETPROPERTY event.
                              
                    Forms do not support attempts to set the FID_SHELL property.

                        Property Value:  IShell *

FID_MENUMODEL:      This property contains a pointer to the menu model that may be attached to an object
                    within a form -- typically, the list widget that forms the basis for a popup menu
                    form.  The property returns that pointer in an IModel pointer passed in the 'dwParam'
                    parameter passed to the form in the EVT_WDG_GETPROPERTY event.
                             
                    Forms do not support attempts to set the FID_MENUMODEL property.

                        Property Value:  IModel *

FID_ISPOPUP:        This property indicates whether or not a given form possesses the characteristics of
                    a popup, returning TRUE when the form is a popup, FALSE when it is not.
                               
                    Forms do not support attempts to set the FID_ISPOPUP property (a given form either
                    is, or is not, a popup from the point it is created onward).
                             
                        Property Value:  boolean

FID_SOFTKEY1:       This property contains a pointer to the text that will be displayed in softkey one
                    (usually, the left softkey), in the form's softkey area.  Though the value of the
                    property is a pointer to an AECHAR, the property value must be set via a pointer to
                    a FormRes data structure which identifies the property value as either a resource
                    file reference or as a direct pointer to the string to be displayed by the softkey
                    widget.
                             
                        Property Value:  AECHAR *

FID_SOFTKEY2:       This property contains a pointer to the text that will be displayed in softkey two
                    (usually, the right softkey), in the form's softkey area.  Though the value of the
                    property is a pointer to an AECHAR, the property value must be set via a pointer to
                    a FormRes data structure which identifies the property value as either a resource
                    file reference or as a direct pointer to the string to be displayed by the softkey
                    widget.
                             
                        Property Value:  AECHAR *

FID_SELECTHANDLER:  This property contains a pointer to the select handler used by a form.  Forms
                    wishing to hook into the base form's selection handler would provide their own
                    callback function that the forms framework would call whenever a selection
                    event takes place.  For example, a list form could provide its own selection
                    handler that would be called each time a new list item is selected.  The
                    selection handler is stored in a SelectHandler data structure as a PFNSELECT,
                    along with a pointer to context data owned by the widget.  The FID_SELECTHANDLER
                    property contains a pointer to this data structure, and can be set but not
                    retrieved -- i.e. this property is used as a mechanism for registering a form's
                    selection callback and does not contain data that would be used by a form
                    instance.
                             
                        Property Value:  SelectHandler *

FID_CANCELKEY:      This property identifies the key that will perform the cancel operation for a
                    particular form -- i.e. the virtual key code that will generate the EVT_FORM_CANCEL
                    event.
                             
                        Property Value:  uint16

FID_DISPLAY:         Sets the display to be used for the RootForm and all the forms and widgets it manages. 
                     The background, title and softkey widget are resized horizontally and vertically to
                     fit the given display size.

                        Property Value:  IDisplay *

FID_FADEMS:          Sets the fade time for certain forms to a given value in milliseconds. Dialog backdrops, 
                     for example, utilize this property to fade out on cancellation of the dialog. 

                        Property Value:  uint32

===========================================================================

IFORM_AddRef()

Description:

   This function increments the reference count of the IForm interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IFORM_AddRef(IForm *p);

Parameters:
===pre>
   p:  Pointer to the IForm interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IForm interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IFORM_Release()
   
=======================================================================

IFORM_Release()

Description:

   This function decrements the reference count of the IForm interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IFORM_Release(IForm *p);

Parameters:
===pre>
   p:  Pointer to the IForm interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IForm interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IFORM_AddRef()

=======================================================================

IFORM_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IFORM_QueryInterface(IForm *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IForm interface object.
   c:  A globally unique id that identifies the interface to be queried.
   d:  Pointer to a pointer returned by the function to reference the
       interface requested by the 'c' parameter.  If the queried item is not
       available, this pointer is returned as NULL.
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

=======================================================================

IFORM_HandleEvent()

Description:
   This function receives and handles events passed to the form from its owner.

Prototype:
   boolean IFORM_HandleEventHandleEvent(IForm *p, AEEEvent e, uint16 w, uint32 d);

Parameters:
===pre>   
   p:  A pointer to an IForm object.
   e:  The event code being sent to the IForm object.
   w:  16-bit event-specific parameter.
   d:  32-bit event-specific parameter.
===/pre>   
   
Return Value:
===pre>
   TRUE  - If the event was handled by the object.
   FALSE - If the event was not handled.
===/pre>

Comments:  
   None

Side Effects: 
   None
   
See Also:
    Form Events
    Widget Events
    IForm

===========================================================================

IFORM_SetHandler()

Description:
   This function allows a form to hook it's own event handler into the normal event
   handling mechanism.  For example, a specialized form such as the list form or popup
   menu could insert its own event handler that will be called by the base form object
   each time an event is processed.
   
   The event handler is stored in a HandlerDesc data structure as a PFNHANDLER, along with
   a pointer to a block of memory allocated and owned by the widget.  This pointer will be
   passed into the widget's event handler with each event to be processed.  The  HandlerDesc
   data structure also contains a PFNFREEHANDLER that will be called when the widget is
   released so that the widget may free the private memory allocated and passed to the event
   handler.
   
   IFORM_SetHandler() associates a given HandlerDesc data structure with a particular form.

Prototype:
   void IFORM_SetHandler(IForm *p, HandlerDesc *pd);

Parameters:
===pre>   
   p:   A pointer to an IForm object.
   pd:  Pointer to the data structure that contains the event handler, private memory
         pointer, and the callback used to free the private memory.
===/pre>   
   
Return Value:
   None

Comments:  
   Passing NULL for the HandlerDesc will reset the form's event handler to use the base
   form's default event handler -- without private memory or the need to free privately
   allocated storage.
   
   The handler installed by a form to process events should itself remember to call
   HANDLERDESC_Call() so that forms that themselves derive from this form may also
   to hook into the form event handler.

Side Effects: 
   None
   
See Also:
   IForm
   PFNHANDLER
   PFNFREEHANDLER
   HandlerDesc
   
==========================================================================

IFORM_GetProperty()

Description:
   This function retrieves a specific property value for the given form.  Forms may
   possess any number of form properties, as defined in AEEForm.h, and may additionally
   support widget properties defined in AEEWProperties.h.  Form properties include
   visual characteristics of the form such as the current activation state, theme,
   and background image.  The property value is returned by IFORM_GetProperty() in a
   pointer passed into the function in the 'pVal' parameter, with the function
   returning SUCCESS or EFAILED depending on whether or not the requested property was
   able to be retrieved from the form.

Prototype:
   int IFORM_GetProperty(IForm *po, uint16 propId, uint32 *pVal); 

Parameters:
===pre>
   po:      Pointer to the IForm interface object.
   propId:  Identifies the property to be retrieved.
   pVal:    A pointer to a 32-bit unsigned integer into which the form will copy the
            value of the requested property.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>
   
   The value of the requested property is returned in 'pVal'.

Comments:  
   Calling IFORM_GetProperty() has the same effect as processing an EVT_WDG_GETPROPERTY
   event.

Side Effects: 
   None

See Also:
   IForm Interface
   IFORM_SetProperty()
   Widget Events

==========================================================================

IFORM_SetProperty()

Description:
   This function sets the value of a specific property for the given form.  Forms may
   possess any number of form properties, as defined in AEEForm.h, and may additionally
   support widget properties defined in AEEWProperties.h.  Form properties include
   visual characteristics of the form such as the current activation state, theme,
   and background image.

Prototype:
   int IFORM_SetProperty(IForm *p, uint16 propId, uint32 val); 

Parameters:
===pre>
   p:       Pointer to the IForm interface object.
   propId:  Identifies the property whose value is to be set.
   val:     The new value for the widget's or form's 'propID' property, expressed as a 32-bit
            unsigned integer.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to set the requested property.
   EFAILED - If the property could not be set.
===/pre>

Comments:  
   Calling IFORM_SetProperty() has the same effect as processing an EVT_WDG_SETPROPERTY
   event.
   
   Not all properties set a stored value in the specified form.  Several form properties
   (FID_TEXT, FID_PROGRESS, etc) are used as triggers to invoke a specific form behavior
   and do NOT store any retrievable value in the indicated property.

Side Effects: 
   None

See Also:
   IForm Interface
   IFORM_GetProperty()

===========================================================================

IFORM_Activate()

Description:
   This function will request that a form activate itself -- for example, when that
   form is brought to the top of the form stack and becomes the visible form on the
   display.  The form should respond in whatever manner is appropriate for its specific
   needs.  For example, a form that defines a container widget (which, in turn, holds
   text, images and other controls) could give the focus to the container and trigger
   a trickle down effect of activation to all the child widgets on the form.

   During activation FID_PREFRECT is requested from the form to allow the form to 
   indicate the preferred rectangle (position and size) in screen co-ordinates of 
   the form widget.
   

Prototype:
   void IFORM_Activate(IForm *po); 

Parameters:
===pre>
   po:  Pointer to the IForm interface object that has become active.
===/pre>

Return Value:
   None

Comments:  
   None

Side Effects: 
   Calling IFORM_Activate() effectively sets the FID_ACTIVE property on the form,
   passing TRUE as the current activation state.

See Also:
   IFORM_Deactivate()

===========================================================================

IFORM_Deactivate()

Description:
   This function will request that a form deactivate itself -- for example, when that
   form is no longer on the top of the form stack and is no longer the visible form on
   the display.  The form should respond in whatever manner is appropriate for its specific
   needs.  For example, a form that defines a container widget (which, in turn, holds
   text, images and other controls) could deactivate this container and trigger a trickle
   down effect of deactivation to all the child widgets on the form.

Prototype:
   void IFORM_Deactivate(IForm *po); 

Parameters:
===pre>
   po:  Pointer to the IForm interface object that has been deactivated.
===/pre>

Return Value:
   None

Comments:  
   None

Side Effects: 
   Calling IFORM_Deactivate() effectively sets the FID_ACTIVE property on the form,
   passing FALSE as the current activation state.

See Also:
   IFORM_Activate()

===========================================================================

IFORM_GetWidget()

Description:
   This function will retrieve a pointer to a particular child widget contained within
   a form.  For example, an application could call IFORM_GetWidget(), passing in
   WID_SOFTKEYS to retrieve a pointer to the form's softkey widget. 

Prototype:
   int IFORM_GetWidget(IForm *po, uint16 wid, IWidget **ppo); 

Parameters:
===pre>
   po:   Pointer to the IForm interface object.
   wid:  Widget ID specifying the widget to be retrieved.
   ppo:  Pointer to an IWidget pointer, into which the function will return a pointer
         to the requested widget.
===/pre>

Return Value:
===pre>
   0                - If the form was able to successfully retrieve the requested widget
   ECLASSNOTSUPPORT - If the requested widget was not recognized by the form.
===/pre>

Comments:  
   The pointer passed in the 'ppo' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.
   
   IFORM_GetWidget() is the base "widget getter" for a form.  Applications are encouraged
   to use the more specific IFORM_GetxxxxxxWidget() functions when retrieving a form's
   widgets.

Side Effects: 
   Calling IFORM_GetWidget() effectively queries the form to retrieve the requested
   widget as a property.

See Also:
   IForm Interface
   IFORM_GetTitleWidget()
   IFORM_GetSoftkeyWidget()
   IFORM_GetBGWidget()
   IFORM_GetProgressWidget()
   IFORM_GetDecoratorWidget()
   IFORM_SetWidget()
   IWidget Interface

===========================================================================

IFORM_SetWidget()

Description:
   This function will attach a widget to a form as one of the form's primary, common
   widgets -- title, soktkey, etc...  For example, an application that wishes to
   change a form's title widget could call IFORM_SetWidget() specifying WID_TITLE as
   the form widget ID.  Attempts to set a widget that is not supported by a particular
   form will result in no assignment and ECLASSNOTSUPPORT being returned by the function.
   For example, attempting to set FID_SOFTKEY1 on a form that is not designed to
   provide softkeys would result in a failure. 

Prototype:
   int IFORM_SetWidget(IForm *po, uint16 wid, IWidget *ppo); 

Parameters:
===pre>
   po:   Pointer to the IForm interface object.
   wid:  Widget ID specifying the widget to be retrieved.
   ppo:  Pointer to the IWidget that will function as the 'wid' widget on this form.
===/pre>

Return Value:
===pre>
   SUCCESS          - If the form was able to successfully set the widget.
   ECLASSNOTSUPPORT - If the passed widget is not supported by the form.
===/pre>

Comments:  
   None

Side Effects: 
   Calling IFORM_SetWidget() effectively sets the specified 'wid' property on the form.

See Also:
   IForm Interface
   IFORM_GetWidget()
   IWidget Interface

===========================================================================

IFORM_GetRootForm()

Description:
   This function will retrieve a pointer to the root form that manages a form.  The root
   form controls the stacking order for a collection of forms.

Prototype:
   int IFORM_GetRootForm(IForm *po, IRootForm **ppo); 

Parameters:
===pre>
   po:   Pointer to the IForm interface object.
   ppo:  Pointer to an IRootForm pointer, into which the function will return a pointer
         to the root form.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully retrieve the root form.
   EFAILED - If the root form could not be retrieved.
===/pre>
   
   A pointer to the root form is returned in 'ppo'.

Comments:  
   Calling IFORM_GetRootForm() on the root form will yield undefined results and return
   EFAILED to the requesting client.
   
   The pointer passed in the 'ppo' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   Calling IFORM_GetRootForm() effectively queries the form to retrieve the FID_ROOT
   property.

See Also:
   IForm Interface
   IRootForm Interface

===========================================================================

IFORM_GetShell()

Description:
   This function will retrieve a reference to the to the IShell object used by a form.

Prototype:
   int IFORM_GetRootForm(IForm *po, IShell **ppo); 

Parameters:
===pre>
   po:   Pointer to the IForm interface object.
   ppo:  Pointer to an IShell pointer, into which the function will return a pointer
         to the shell.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully retrieve a reference to the shell.
   EFAILED - If the shell reference could not be retrieved.
===/pre>
   
   A pointer to the shell used by the form is returned in 'ppo'.

Comments:  
   The pointer passed in the 'ppo' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   Calling IFORM_GetShell() effectively queries the form to retrieve the FID_SHELL
   property.

See Also:
   IForm Interface
   IShell Interface

===================================================================================

IFORM_GetTitleWidget()

Description:
   This function will retrieve a pointer to the form's title widget.

Prototype:
   int IFORM_GetTitleWidget(IForm *p, IWidget **w); 

Parameters:
===pre>
   p:  Pointer to the IForm interface object.
   w:  Pointer to an IWidget pointer, into which the function will return a pointer
       to the form's title widget.
===/pre>

Return Value:
===pre>
   0                - If the form was able to successfully retrieve the title widget.
   ECLASSNOTSUPPORT - If the form does not support the use of a title widget.
===/pre>

   A pointer to the form's title widget is returned in 'w'.

Comments:  
   The pointer passed in the 'w' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   Calling IFORM_GetTitleWidget() effectively queries the form to retrieve the WID_TITLE
   property.

See Also:
   IForm Interface
   IWidget Interface

===========================================================================

IFORM_GetSoftkeyWidget()

Description:
   This function will retrieve a pointer to the form's softkey widget.

Prototype:
   int IFORM_GetSoftkeyWidget(IForm *p, IWidget **w); 

Parameters:
===pre>
   p:  Pointer to the IForm interface object.
   w:  Pointer to an IWidget pointer, into which the function will return a pointer
       to the form's softkey widget.
===/pre>

Return Value:
===pre>
   0                - If the form was able to successfully retrieve the softkey widget.
   ECLASSNOTSUPPORT - If the form does not support the use of a softkey widget.

===/pre>

   A pointer to the form's softkey widget is returned in 'w'.

Comments:  
   The pointer passed in the 'w' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   Calling IFORM_GetSoftkeyWidget() effectively queries the form to retrieve the
   WID_SOFTKEYS property.

See Also:
   IForm Interface
   IWidget Interface

===========================================================================

IFORM_GetBGWidget()

Description:
   This function will retrieve a pointer to the form's background widget.

Prototype:
   int IFORM_GetBGWidget(IForm *p, IWidget **w); 

Parameters:
===pre>
   p:  Pointer to the IForm interface object.
   w:  Pointer to an IWidget pointer, into which the function will return a pointer
       to the form's background widget.
===/pre>

Return Value:
===pre>
   0                - If the form was able to successfully retrieve the background
                      widget.
   ECLASSNOTSUPPORT - If the form does not support the use of a background widget.

===/pre>

   A pointer to the form's background widget is returned in 'w'.

Comments:  
   The pointer passed in the 'w' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   Calling IFORM_GetSoftkeyWidget() effectively queries the form to retrieve the
   WID_SOFTKEYS property.

See Also:
   IForm Interface
   IWidget Interface

===========================================================================

IFORM_GetProgressWidget()

Description:
   This function will retrieve a pointer to the form's progress widget for those specialized
   forms that have derived from the base form.  For example, the progress dialog is a
   specialized form that derives from dialogs, popups and -- eventually -- back to forms.
   The progress dialog defines a progress widget -- typically to popup a quick dialog to
   illustrate progress for some time consuming operation.  Calling IFORM_GetProgressWidget()
   allows an application to access the progress widget that provides this visual feedback.
   It is important to note that the progress widget is provided in _addition_ to the form's
   primary widget accessed by the WID_FORM property.  Requesting to retrieve the progress
   widget from a form that is not intended to support a progress indicator will fail with
   ECLASSNOTSUPPORT.

Prototype:
   int IFORM_GetProgressWidget(IForm *p, IWidget **w); 

Parameters:
===pre>
   p:  Pointer to the IForm interface object.
   w:  Pointer to an IWidget pointer, into which the function will return a pointer
       to the form's progress widget.
===/pre>

Return Value:
===pre>
   0                - If the form was able to successfully retrieve the progress
                      widget.
   ECLASSNOTSUPPORT - If the form does not support the use of a progress widget.
===/pre>

   A pointer to the form's progress widget is returned in 'w'.

Comments:  
   The pointer passed in the 'w' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   Calling IFORM_GetProgressWidget() effectively queries the form to retrieve the
   WID_PROGRESS property.

See Also:
   IForm Interface
   IWidget Interface

===========================================================================

IFORM_GetDecoratorWidget()

Description:
   This function will retrieve a pointer to the form's decorator widget for those specialized
   forms that have derived from the base form.  For example, the list form defines a list widget
   (which derives from the decorator widget) to manage list data in an expected and consistent
   manner.  It is important to note that the decorator widget is provided in _addition_ to the
   form's primary widget accessed by the WID_FORM property.  Calling IFORM_GetDecoratorWidget()
   allows an application to access this decorator widget.
   
Prototype:
   int IFORM_GetDecoratorWidget(IForm *po, IDecorator **ppo); 

Parameters:
===pre>
   po:   Pointer to the IForm interface object.
   ppo:  Pointer to an IDecorator pointer, into which the function will return a pointer
         to the form's decorator widget.
===/pre>

Return Value:
===pre>
   0                - If the form was able to successfully retrieve the decorator
                      widget.
   ECLASSNOTSUPPORT - If the form does not support the use of a dedicated decorator widget.

===/pre>

   A pointer to the form's decorator widget is returned in 'ppo'.

Comments:  
   The pointer passed in the 'ppo' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   Calling IFORM_GetDecoratorWidget() effectively queries the form to retrieve the
   WID_DECORATOR property.

See Also:
   IForm Interface
   IDecorator Interface

===========================================================================

IFORM_SetRes()

Description:
   This function will set the value of a form property, identifying that value as a resource
   stored in a BREW resource file.  The form framework will load the specified resource and
   assign the data to the appropriate form property.  For example, a picture of a wind-up
   strawberry ice cream cone could be stored as a bitmap resource in a particular file.  An
   application could then set the background of a form to be this lovely little walking ice
   cream delight by calling IFORM_SetRes(), identifying the image's file name and resource
   ID as the source for setting the FID_BACKGROUND form property.  The form framework will
   comply by loading the image, then notifying all listening objects that the background has
   changed.
   
Prototype:
   int IFORM_SetRes(IForm *po, uint16 fid, const char *resFile, uint16 id); 

Parameters:
===pre>
   po:       Pointer to the IForm interface object.
   fid:      Identifies the form property to be set by the contents of the specified resource.
   resFile:  Identifies the name of the resource file that contains the resource that contains
             the property data.
   id:       Identifies the resource that contains the data to be stored in the form's 'fid'
             property.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to set the specified form property.  Note, however, that this
             does NOT mean that BREW was able to successfully retrieve the resource from the
             resource file.  Applications should be certain that 'resfile' is a valid BREW
             resource file and that resource 'id' actually exists.
   EFAILED - If the specified form property could not be set.
===/pre>

Comments:  
   Applications should exercise care in calling IFORM_SetRes(), making sure that the specified
   resource file and resource actually exist.  Moreover, IFORM_SetRes() is not intended to be
   a generalized form of setting form properties from data contained in a resource file.  Only
   the following form properties may be set from resource data:
===pre>
       FID_BACKGROUND
       FID_TITLE
       FID_TITLEIMAGE
       FID_SOFTKEY1
       FID_SOFTKEY2
===/pre>
   
   Attempting to set properties not listed above will yield unexpected (and fairly ugly) results.

Side Effects: 
   Calling IFORM_SetRes() will send an EVT_WDG_SETPROPERTY event to the form, requesting that
   the 'fid' property be set.

See Also:
   IForm Interface
   Widget Events

===========================================================================

IFORM_SetPtr()

Description:
   This function will set the value of a pointer-based form property -- i.e. those properties
   that contain a pointer as its value, such as the background image or title text.
   
Prototype:
   int IFORM_SetPtr(IForm *po, uint16 fid, void *ptr); 

Parameters:
===pre>
   po:   Pointer to the IForm interface object.
   fid:  Identifies the form property to be set.
   ptr:  Pointer to the data that will be referenced by the 'fid' property.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to set the specified form property.
   EFAILED - If the specified form property could not be set.
===/pre>

Comments:  
   Only the following form properties may be set using IFORM_SetPtr():
===pre>
       FID_BACKGROUND
       FID_TITLE
       FID_TITLEIMAGE
       FID_SOFTKEY1
       FID_SOFTKEY2
===/pre>
   
   Attempting to set properties not listed above will yield unexpected (and fairly ugly) results.

Side Effects: 
   Calling IFORM_SetPtr() will send an EVT_WDG_SETPROPERTY event to the form, requesting that
   the 'fid' property be set.

See Also:
   IForm Interface
   Widget Events

===========================================================================

IFORM_GetTextPtr()

Description:
   This function will retrieve a pointer to the text stored in a form property.  For example,
   an application could retrieve the text of the form title by calling IFORM_GetTextPtr() and
   specifying FID_TITLE as the property to be retrieved.
   
Prototype:
   int IFORM_GetTextPtr(IForm *po, uint16 fid, AECHAR **ppo); 

Parameters:
===pre>
   po:   Pointer to the IForm interface object.
   fid:  Identifies the form property that contains the text to be retrieved.
   ppo:  Pointer to an AECHAR pointer, into which the function will return a pointer
         to the property text.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully retrieve the specified form property.
   EFAILED - If the specified form property could not be retrieved.
===/pre>

Comments:  
   The pointer passed in the 'ppo' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   Calling IFORM_GetTextPtr() effectively queries the form to retrieve the 'fid' property.

See Also:
   IForm Interface

===========================================================================

IFORM_SetText()

Description:
   This function will set the value of a text-based form property -- i.e. those properties
   that contain a pointer to an AECHAR  string as its value, such as the title or softkey
   properties.
   
Prototype:
   int IFORM_SetText(IForm *po, uint16 fid, AECHAR *txt); 

Parameters:
===pre>
   po:   Pointer to the IForm interface object.
   fid:  Identifies the form property to be set.
   ptr:  Pointer to the text that will be referenced by the 'fid' property.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to set the specified form property.
   EFAILED - If the specified form property could not be set.
===/pre>

Comments:  
   Only the following form properties should be set using IFORM_SetText():
===pre>
       FID_TITLE
       FID_SOFTKEY1
       FID_SOFTKEY2
===/pre>
   
   Attempting to set properties not listed above will yield unexpected results.

Side Effects: 
   Calling IFORM_SetText() will send an EVT_WDG_SETPROPERTY event to the form, requesting that
   the 'fid' property be set.

See Also:
   IForm Interface
   Widget Events

===========================================================================

IFORM_SetResText()

Description:
   This function will set the value of a text-based form property, identifying the text as a
   resource stored in a BREW resource file.  The form framework will load the specified text
   resource and assign a pointer to this text as the appropriate form property.  For example,
   the text of a form's softkey widgets could be stored as a text resource in a particular file.
   An application could then set the text for either softkey one or softkey two by calling
   IFORM_SetResText(), identifying the resource file name and ID of the text resource that would
   be used to set the FID_SOFTKEY1 or FID_SOFTKEY2 form properties.  The form framework will
   comply by loading the text, assigning a pointer to this text as the form property, then
   notifying all listening objects that the softkey text has changed.
   
Prototype:
   int IFORM_SetResText(IForm *p, uint16 f, const char *r, uint16 i); 

Parameters:
===pre>
   p:  Pointer to the IForm interface object.
   f:  Identifies the form property to be set by the contents of the specified resource.
   r:  Identifies the name of the resource file that contains the resource that contains
       the property text.
   i:  Identifies the resource that contains the text to be stored in the form's 'f'
       property.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to set the specified form property.  Note, however, that this
             does NOT mean that BREW was able to successfully retrieve the resource from the
             resource file.  Applications should be certain that 'resfile' is a valid BREW
             resource file and that resource 'id' actually exists.
   EFAILED - If the specified form property could not be set.
===/pre>

Comments:  
   Applications should exercise care in calling IFORM_SetResText(), making sure that the specified
   resource file and resource actually exist.  Moreover, IFORM_SetResText() is not intended to be
   a generalized form of setting form properties from data contained in a resource file.  Only
   the following form properties may be set from text-based resource data:
===pre>
       FID_TITLE
       FID_SOFTKEY1
       FID_SOFTKEY2
===/pre>
   
   Attempting to set properties not listed above will yield unexpected results.

Side Effects: 
   Calling IFORM_SetResText() will send an EVT_WDG_SETPROPERTY event to the form, requesting that
   the 'f' property be set.

See Also:
   IForm Interface
   Widget Events

===========================================================================

IFORM_SetTitle()

Description:
   This function will set the title text of a form, storing the title as a pointer to an
   AECHAR in the form's FID_TITLE property.
   
Prototype:
   int IFORM_SetTitle(IForm *p, AECHAR *t); 

Parameters:
===pre>
   p:  Pointer to the IForm interface object.
   t:  Pointer to the text to appear as the form's title.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to set the title text
   EFAILED - If the title text could not be set.
===/pre>

Comments:  
   Applications are encouraged to use IFORM_SetTitle() as a shorthand method of setting the title text.

Side Effects: 
   Calling IFORM_SetTitle() will send an EVT_WDG_SETPROPERTY event to the form, requesting that
   the FID_TITLE property be set.

See Also:
   IForm Interface
   Widget Events

===========================================================================

IFORM_SetSoftkey()

Description:
   This function will set the text appearing on either of a form's softkey widgets, storing that text
   as a pointer to an AECHAR in the FID_SOFTKEY1 or FID_SOFTKEY2 property.
   
Prototype:
   int IFORM_SetSoftkey(IForm *p, uint16 i, AECHAR *t); 

Parameters:
===pre>
   p:  Pointer to the IForm interface object.
   i:  Identifies the form property to be set.  Pass FID_SOFTKEY1 to set the text of softkey one, or
       FID_SOFTKEY2 to set the text of softkey two.
   t:  Pointer to the text to appear on the specified softkey.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to set the softkey text
   EFAILED - If the softkey text could not be set.
===/pre>

Comments:  
   Applications are encouraged to use IFORM_SetSoftkey() as a shorthand method of setting the
   softkey text.

Side Effects: 
   Calling IFORM_SetSoftkey() will send an EVT_WDG_SETPROPERTY event to the form, requesting that
   the FID_SOFTKEY1 or FID_SOFTKEY2 property be set.

See Also:
   IForm Interface
   Widget Events

===========================================================================

IFORM_GetBGImage()

Description:
   This function will retrieve a pointer to the image displayed in the background area of a
   form.  This is, essentially, the image displayed by the form's background widget.  For
   example, a form that uses a semi-opaque photo of Shaquille O'Neal as the background image
   for all of its forms could retrieve Shaq's smiling mug by calling IFORM_GetBGImage().
   
Prototype:
   int IFORM_GetBGImage(IForm *po, IImage **ppi); 

Parameters:
===pre>
   po:   Pointer to the IForm interface object.
   ppi:  Pointer to an IImage pointer, into which the function will return a pointer
         to the background image.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully retrieve the background image.
   EFAILED - If the background image could not be retrieved.
===/pre>

Comments:  
   The pointer passed in the 'ppi' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   Calling IFORM_GetBGImage() effectively queries the form to retrieve the FID_BACKGROUND
   property.

See Also:
   IForm Interface

===========================================================================

IFORM_SetBGImage()

Description:
   This function will set the background image to be displayed on a form, storing the background
   as a pointer to an IImage in the form's FID_BACKGROUND property.
   
Prototype:
   int IFORM_SetBGImage(IForm *po, IImage *piImage); 

Parameters:
===pre>
   po:       Pointer to the IForm interface object.
   piImage:  Pointer to the image to be displayed as the form's background.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully set the background image.
   EFAILED - If the background image could not be set.
===/pre>

Comments:  
   Applications are encouraged to use IFORM_SetBGImage() as a shorthand method of setting the 
   form's background.

Side Effects: 
   Calling IFORM_SetBGImage() will send an EVT_WDG_SETPROPERTY event to the form, requesting that
   the FID_BACKGROUND property be set.

See Also:
   IForm Interface
   Widget Events

===========================================================================

IFORM_SetResBGImage()

Description:
   This function will set the background image to be displayed on a form, identifying the image
   as a resource stored in a BREW resource file, and storing a pointer to this image in the form's
   FID_BACKGROUND property.  The form framework will load the specified image resource and assign
   a pointer to this image as the background property on the form.  For example, an application
   that wishes to display a picture of a ecstatically happy cupie doll looking at a giant ice
   cream cone in the background widget of a form could store this picture as an image resource in
   a particular resource file.  The application could then set the background of a form to be this
   rather strange image by calling IFORM_SetResBGImage(), identifying the resource file name and
   ID of the image to be assigned to the form's FID_BACKGROUND property.  The form framework will
   comply by loading the image, assigning a pointer to this image as the form's background property,
   then notifying all listening objects that the background has changed.
   
Prototype:
   int IFORM_SetResBGImage(IForm *po, const char *resFile, uint16 resid); 

Parameters:
===pre>
   po:       Pointer to the IForm interface object.
   resFile:  Identifies the name of the resource file that contains the resource that contains
             the background image.
   resid:    Identifies the resource that contains the image to be displayed in the form's
             background widget.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to set the background image.  Note, however, that this
             does NOT mean that BREW was able to successfully retrieve the image from the
             resource file.  Applications should be certain that 'resfile' is a valid BREW
             resource file and that resource 'resid' actually exists.
   EFAILED - If the background could not be set.
===/pre>

Comments:  
   Applications should exercise care in calling IFORM_SetResBGImage(), making sure that the
   specified resource file and resource actually exist.
   
Side Effects: 
   Calling IFORM_SetResBGImage() will send an EVT_WDG_SETPROPERTY event to the form, requesting
   that the FID_BACKGROUND property be set.

See Also:
   IForm Interface
   Widget Events

===========================================================================

IFORM_GetTitleImage()

Description:
   This function will retrieve a pointer to the image displayed in the title area of a form.
   This is, essentially, the image displayed by the form's title widget.  For example, a form
   that uses a fancy corporate logo in the title area of a form could retrieve a pointer to
   this logo by calling IFORM_GetTitleImage().
   
Prototype:
   int IFORM_GetTitleImage(IForm *po, IImage **ppi); 

Parameters:
===pre>
   po:   Pointer to the IForm interface object.
   ppi:  Pointer to an IImage pointer, into which the function will return a pointer
         to the title image.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully retrieve the title image.
   EFAILED - If the title image could not be retrieved.
===/pre>

Comments:  
   The pointer passed in the 'ppi' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   Calling IFORM_GetTitleImage() effectively queries the form to retrieve the FID_TITLEIMAGE
   property.

See Also:
   IForm Interface

===========================================================================

IFORM_SetTitleImage()

Description:
   This function will set the title image to be displayed on a form, storing the image as
   a pointer to an IImage in the form's FID_TITLEIMAGE property.
   
Prototype:
   int IFORM_SetTitleImage(IForm *po, IImage *piImage); 

Parameters:
===pre>
   po:       Pointer to the IForm interface object.
   piImage:  Pointer to the image to be displayed in the form's title area.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully set the title image.
   EFAILED - If the title image could not be set.
===/pre>

Comments:  
   Applications are encouraged to use IFORM_SetTitleImage() as a shorthand method of setting
   the form's title image.

Side Effects: 
   Calling IFORM_SetTitleImage() will send an EVT_WDG_SETPROPERTY event to the form, requesting
   that the FID_TITLEIMAGE property be set.

See Also:
   IForm Interface
   Widget Events

===========================================================================

IFORM_SetResTitleImage()

Description:
   This function will set the image to be displayed in the title area of a form, identifying the
   image as a resource stored in a BREW resource file, and storing a pointer to this image in the
   form's FID_TITLEIMAGE property.  The form framework will load the specified image resource and
   assign a pointer to this image as the title image property on the form.  For example, an
   application that wishes to display a picture of a sleek sports car in the title area of a form
   could store this picture as an image resource in a particular resource file.  The application
   could then set the title image of a form to be this image by calling IFORM_SetResTitleImage(),
   identifying the resource file name and ID of the image to be assigned to the form's FID_TITLEIMAGE
   property.  The form framework will comply by loading the image, assigning a pointer to this image
   as the form's title image property, then notifying all listening objects that the title area has
   changed.
   
Prototype:
   int IFORM_SetResTitleImage(IForm *po, const char *resFile, uint16 resid); 

Parameters:
===pre>
   po:       Pointer to the IForm interface object.
   resFile:  Identifies the name of the resource file that contains the resource that contains
             the title image.
   resid:    Identifies the resource that contains the image to be displayed in the form's
             title widget.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to set the title image.  Note, however, that this does NOT
             mean that BREW was able to successfully retrieve the image from the resource file. 
             Applications should be certain that 'resfile' is a valid BREW resource file and
             that resource 'resid' actually exists.
   EFAILED - If the title image could not be set.
===/pre>

Comments:  
   Applications should exercise care in calling IFORM_SetResTitleImage(), making sure that the
   specified resource file and resource actually exist.
   
Side Effects: 
   Calling IFORM_SetResTitleImage() will send an EVT_WDG_SETPROPERTY event to the form, requesting
   that the FID_TITLEIMAGE property be set.

See Also:
   IForm Interface
   Widget Events

===========================================================================

IFORM_GetIsPopup()

Description:
   This function will retrieve the popup state of a form, returning TRUE if the form
   functions as a popup, FALSE if it does not.
      
Prototype:
   boolean IFORM_GetIsPopup(IForm *po); 

Parameters:
===pre>
   po:    Pointer to the IForm interface object.
===/pre>

Return Value:
===pre>
   TRUE  - If the form is a popup.
   FALSE - If the form is not a popup.
===/pre>

Comments:  
   None

Side Effects: 
   Calling IFORM_GetIsPopup() effectively queries the form to retrieve the FID_ISPOPUP
   property.

See Also:
   IForm Interface

===========================================================================

IFORM_ApplyTheme()

Description:

   This function instructs the form framework to apply the current theme to
   the contents of a given form.  Upon setting the theme file the base
   full-screen form implementation will load the theme background image for a
   full screen form.

Prototype:
   int IFORM_ApplyTheme(IForm *po); 

Parameters:
===pre>
   po:         Pointer to the IForm interface object.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully apply the theme.
===/pre>

Comments:  
   None

Side Effects: 
   Calling IFORM_ApplyTheme() will send an EVT_WDG_SETPROPERTY event to the form, requesting
   that the FID_THEME property be set.
   
See Also:
   IForm Interface
   IRootForm Interface
   FormEvent
   Widget Events

===========================================================================

IFORM_SetProgress()

Description:
   This function updates a form's progress widget, providing that widget with a new value
   reflective of the current progress of some application defined operation -- dependent
   on how a given form has chosen to implement "progress".  The progress dialog, for example,
   tracks progress as "percent complete" and displays a progress widget that reflects this
   percentage as a filled rectangle -- the more percent complete, the more the rectangle is
   filled.  Other forms may choose to reflect progress differently.  For example, a form that
   wishes to show non-deterministic progress may wish to show some perpetual animation not
   meant to indicate when this animation might complete.
   
Prototype:
   int IFORM_SetProgress(IForm *po, uint8 percent); 

Parameters:
===pre>
   po:       Pointer to the IForm interface object.
   percent:  Specifies the "percent complete" of the operation for which the form is
             indicating progress.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form contains a progress widget and is able to accept progress update
             events.
   EFAILED - If the form does not contain a progress widget or is not able to accept progress
             update events.
===/pre>

Comments:  
   None

Side Effects: 
   Calling IFORM_SetProgress() will send an EVT_WDG_SETPROPERTY event to the form with the
   FID_PROGRESS property being used as a mechanism to trigger the form to update its
   progress widget.  Though the FID_PROGRESS property is being "set", this property does
   not actually contain any value.

See Also:
   IForm Interface
   FormEvent
   Widget Events

===========================================================================

IFORM_IsActive()

Description:
   This function will retrieve the activation state of a form, returning TRUE if the form
   is currently active, FALSE if it is not.  Only the form at the top of the root form's
   form stack is considered to be active; all other forms on the form stack are inactive.
      
Prototype:
   boolean IFORM_IsActive(IForm *po); 

Parameters:
===pre>
   po:    Pointer to the IForm interface object.
===/pre>

Return Value:
===pre>
   TRUE  - If the form is active.
   FALSE - If the form is not currently active.
===/pre>

Comments:  
   None

Side Effects: 
   Calling IFORM_IsActive() effectively queries the form to retrieve the FID_ACTIVE
   property.

See Also:
   IForm Interface

===========================================================================

IFORM_SetSoftkeys()

Description:
   This function sets or clears the text to be displayed in each of the two softkey widgets
   that appear on a form, setting the text (when present) from strings appearing in a BREW
   resource file.
   
Prototype:
   int IFORM_SetSoftkeys(IForm *po, const char *pszResFile, uint16 idSoftkey1, uint16 idSoftkey2); 

Parameters:
===pre>
   po:          Pointer to the IForm interface object.
   pszResFile:  Identifies the name of the resource file that contains the resource that
                contains softkey strings.
   idSoftkey1:  Identifies the resource ID of the string to be displayed in softkey one, or
                zero to clear the text appearing in softkey one.
   idSoftkey2:  Identifies the resource ID of the string to be displayed in softkey two, or
                zero to clear the text appearing in softkey two.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to set the softkey text for both softkeys.  Note, however, that
             this does NOT mean that BREW was able to successfully retrieve the text for both
             softkeys from the resource file.  Applications should be certain that 'pszResFile'
             is a valid BREW resource file and that the 'idSoftkey1' and 'idSoftkey2' resources
             actually exists.
   EFAILED - If the softkey text could not be set.
===/pre>

Comments:  
   Applications should exercise care in calling IFORM_SetSoftkeys(), making sure that the
   specified resource file and resources actually exist.
   
Side Effects: 
   Calling IFORM_SetSoftkeys() will send a pair of EVT_WDG_SETPROPERTY events to the form, the first
   requesting that the FID_SOFTKEY1 property be set, while the second sets FID_SOFTKEY2.

See Also:
   IForm Interface
   Widget Events


===========================================================================

IFORM_SetSelectHandler()

Description:
   This function will attach a selection handler to a form, identifying a callback
   routine that will be called each time a selection takes place, as well as a pointer
   to private data that will be provided to the callback each time the function is
   issued.  For example, let's say a specialized form has been created that contains
   a list allowing the user to scroll through pictures of famous people.  Each time a
   famous face is selected we want the device to play a recorded message from that
   celebrity ("Hi, I'm Marilyn, thank you for choosing BREW!").  The application could
   register a selection handler that would be called each time an item in the list is
   selected.  The callback could then play the appropriate audio message based on the
   item being selected.
   
Prototype:
   int IFORM_SetSelectHandler(IForm *po, PFNSELECT pfnHandler, void *pv); 

Parameters:
===pre>
   po:          Pointer to the IForm interface object.
   pfnHandler:  The callback function that the forms framework will call when an item
                selection takes place.
   pv:          Pointer to the private data defined by and returned to the form registering
                the selection handler.  The data pointer will be passed to the selection
                handler each time the 'pfnHandler' routine is called.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully register the selection handler.
   EFAILED - If the selection handler could not be registered.
===/pre>

Comments:  
   None

Side Effects: 
   Calling IFORM_SetSelectHandler() will send an EVT_WDG_SETPROPERTY event to the form, requesting
   that the FID_SELECTHANDLER property be set.

See Also:
   IForm Interface
   Widget Events

===========================================================================

IFORM_SetCancelKey()

Description:
   This function identifies the key that will perform the cancel operation for a form --
   i.e. the virtual key code that, when pressed, generates an EVT_FORM_CANCEL event.  This
   allows an application to more clearly indicate how they would like their user interface
   to behave with regards to moving forms off of the form stack.
    
Prototype:
   int IFORM_SetCancelKey(IForm *po, uint16 wKey); 

Parameters:
===pre>
   po:    Pointer to the IForm interface object.
   wKey:  Specifies the virtual key code the form will treat as the cancel key.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully set the cancel key.
   EFAILED - If the cancel key could not be set.
===/pre>

Comments:  
   None

Side Effects: 
   Calling IFORM_SetCancelKey() will send an EVT_WDG_SETPROPERTY event to the form, requesting
   that the FID_CANCELKEY property be set.

See Also:
   IForm Interface
   Form Events
   Widget Events

===========================================================================

IFORM_GetCancelKey()

Description:
   This function will retrieve the virtual key code that the form recognizes as the
   cancel key -- i.e. the key that, when pressed, will generate an EVT_FORM_CANCEL
   event, returning that key in the 'pwKey' passed into the function.
      
Prototype:
   int IFORM_GetCancelKey(IForm *po, uint16 *pwKey); 

Parameters:
===pre>
   po:     Pointer to the IForm interface object.
   pwKey:  Pointer to a 16-bit integer into which the virtual key code of the
           form's cancel key will be copied.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully retrieve the cancel key.
   EFAILED - If the cancel key could not be retrieved.
===/pre>

Comments:  
   None

Side Effects: 
   Calling IFORM_GetCancelKey() effectively queries the form to retrieve the FID_CANCELKEY
   property.

See Also:
   IForm Interface
   Form Events

===========================================================================

IFORM_SetThemeBaseName()

Description:

   This function sets the theme basename for the form, identifying the base
   name within the current theme file from which a form will apply theme data.

   A single theme file may contain many sets of theme data, each set rooted at
   a basename. The standard forms will look for theme entries under sub-names
   of the basename to locate properties and other resources required to apply
   the theme. 

   By changing the theme basename, a form is indicating where in the theme
   file the form framework should root its searches for theme data to be
   applied to this specific form.

   Changing the theme basename will trigger a FORMITEM_THEME_BASENAME form
   event to be sent to listening objects to notify them that the theme
   basename has changed.

   The RootForm is amongst those objects listening and will update at least
   the title, background and softkey properties based on the new theme
   basename.

   
Prototype:
   int IFORM_SetThemeBaseName(IForm *po, const char *baseName); 

Parameters:
===pre>
   po:         Pointer to the IForm interface object.
   baseName:   Specifies the section within the current theme file where the theme
               data for this form can be retrieved.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully set the theme basename to be applied by the form.
   EFAILED - If the theme basename could not be set.
===/pre>

Comments:  
   Several forms have default theme base names that will be used if the application does not override them.

   Default Forms (AEEForm.h, AEERootForm.h):
===pre>
      "System" (All forms)
      "Root" (Root forms only)
===/pre>
  
   Dialogs (AEEDialog.h): 
===pre>
      "System.ProgressDialog" (Progress dialogs only)
===/pre>

   Popups (AEEPopup.h):
===pre>
      "System.PopupMenu" (Popup menus only)
===/pre>


Side Effects: 
   Calling IFORM_SetThemeBaseName() will send an EVT_WDG_SETPROPERTY event to the form, requesting
   that the FID_THEME_BASENAME property be set.

   Successfully applying the form theme basename will cause a
   FORMITEM_THEME_BASENAME form event to be sent to any listening objects.
   
See Also:
   IForm Interface
   IFORM_ApplyThem()
   FormEvent

===========================================================================

IFORM_GetThemeBaseName()

Description:

   This function will retrieve the theme basename used by the current form.
   The theme basename identifies the root within the current theme file from
   which a form will apply theme data. 

   Note that the form retains ownership of the returned name. 
      
Prototype:
   int IFORM_GetThemeBaseName(IForm *po, const char **name); 

Parameters:
===pre>
   po:          Pointer to the IForm interface object.
   name:        Pointer to the destination for the string pointer
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully retrieve the form theme basename
   EFAILED - If the form theme basename could not be retrieved.
===/pre>

Comments:  
   None

Side Effects: 
   Calling IFORM_GetThemeBase() effectively queries the form to retrieve the FID_THEME_BASE
   property.

See Also:
   IForm Interface

===========================================================================

IFORM_GetFormModel()

Description:
   This function will retrieve a pointer to the model interface used by the form.
   
Prototype:
   int IFORM_GetFormModel(IForm *po, IModel **ppo); 

Parameters:
===pre>
   po:   Pointer to the IForm interface object.
   ppo:  Pointer to an IModel pointer, into which the function will return a pointer
         to the model interface attached to the form.
===/pre>

Return Value:
===pre>
   SUCCESS - If the form is able to successfully retrieve a reference to the model
             interface.
   EFAILED - If the form's model interface could not be retrieved.
===/pre>

Comments:  
   The pointer passed in the 'ppo' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   Calling IFORM_GetFormModel() effectively queries the form to retrieve the FID_FORMMODEL
   property.

See Also:
   IForm Interface
   IModel Interface

*/

#endif /* __AEEFORM_H__ */
