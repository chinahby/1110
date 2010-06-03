/*
  ========================================================================

  FILE:  AEERootForm.h
  
  SERVICES: 

  GENERAL DESCRIPTION: RootForm for all forms. Sorta like a window manager

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEROOTFORM_H__
#define __AEEROOTFORM_H__

#include "AEEForm.h"
#include "AEEResFile.h"
#include "AEEWProperties.h"

#define AEEIID_ROOTFORM    	0x01013604

#include "bid/AEECLSID_ROOTFORM.bid"

#define FORM_URL_SCHEME          "form:"

#define FORM_NONE                (IForm*)0
#define FORM_DEFAULT             (IForm*)0
#define FORM_LAST                (IForm*)1
#define FORM_FIRST               (IForm*)2
#define FORM_NEXT                (IForm*)3
#define FORM_PREV                (IForm*)4
#define FORM_ALL                 (IForm*)5

#define INHERIT_IRootForm(iname) \
   INHERIT_IForm(iname); \
   int    (*InsertForm)      (iname *, IForm *pf, IForm *pfBefore); \
   int    (*RemoveForm)      (iname *, IForm *pf); \
   IForm *(*GetForm)         (iname *, IForm *pwRef, boolean bNext, boolean bWrap);\
   int    (*ResolveForm)     (iname *, char const *szFormUrl, IForm **ppiForm); \
   void   (*GetClientRect)   (iname *, IXYContainer **ppContainer, AEERect *rc)

AEEINTERFACE(IRootForm) {
   INHERIT_IRootForm(IRootForm);
};

#define IROOTFORM_AddRef(p)                    AEEGETPVTBL(p,IRootForm)->AddRef(p)
#define IROOTFORM_Release(p)                   AEEGETPVTBL(p,IRootForm)->Release(p)
#define IROOTFORM_QueryInterface(p,c,d)        AEEGETPVTBL(p,IRootForm)->QueryInterface(p,c,d)
#define IROOTFORM_HandleEvent(p,e,w,d)         AEEGETPVTBL(p,IRootForm)->HandleEvent(p,e,w,d)
#define IROOTFORM_InsertForm(p,f,pr)           AEEGETPVTBL(p,IRootForm)->InsertForm(p,f,pr)
#define IROOTFORM_RemoveForm(p,f)              AEEGETPVTBL(p,IRootForm)->RemoveForm(p,f)
#define IROOTFORM_GetClientRect(p,c,r)         AEEGETPVTBL(p,IRootForm)->GetClientRect(p,c,r)

//
// BEWARE:  IROOTFORM_GetForm() does not add a reference to the returned IForm for the caller.
//
#define IROOTFORM_GetForm(p,pr,n,w)            AEEGETPVTBL(p,IRootForm)->GetForm(p,pr,n,w)
#define IROOTFORM_ResolveForm(p,u,o)           AEEGETPVTBL(p,IRootForm)->ResolveForm(p,u,o)

#define IROOTFORM_PopForm(p)                   IROOTFORM_RemoveForm(p,FORM_LAST)
#define IROOTFORM_PushForm(p,f)                IROOTFORM_InsertForm(p,f,FORM_LAST)

static __inline IForm *IROOTFORM_TO_IFORM(IRootForm *me) {
   return (IForm *)(void*)me;
}

static __inline boolean IROOTFORM_IsFormOnStack(IRootForm *po, IForm *piForm) {
   return (boolean)(piForm && IROOTFORM_GetForm(po, piForm, TRUE, TRUE) != NULL);
}

static __inline IForm *IROOTFORM_GetTopForm(IRootForm *po) {
   return IROOTFORM_GetForm(po, NULL, FALSE, FALSE);
}

static __inline boolean IROOTFORM_RemoveFormIfTop(IRootForm *po, IForm *piForm) {
   IForm *piTopForm;
   if ((piTopForm = IROOTFORM_GetTopForm(po)) != NULL &&
       piTopForm == piForm && IROOTFORM_RemoveForm(po, piForm) == SUCCESS) {
         return TRUE;
   }
   return FALSE;
}

static __inline void IFORM_PopSelf(IForm *piForm)
{
   IRootForm *piRoot = 0;
   if (piForm 
         && SUCCESS == IFORM_GetRootForm(piForm, &piRoot) 
         && piRoot) {

      (void)IROOTFORM_RemoveFormIfTop(piRoot, piForm);
      IROOTFORM_Release(piRoot);
   }
}

static __inline void IROOTFORM_Activate(IRootForm *po) {
   (void)IFORM_SetProperty(IROOTFORM_TO_IFORM(po), FID_ACTIVE, TRUE);
}
static __inline void IROOTFORM_Deactivate(IRootForm *po) {
   (void)IFORM_SetProperty(IROOTFORM_TO_IFORM(po), FID_ACTIVE, FALSE);
}

static __inline int IROOTFORM_GetThemeFile(IRootForm *po, IResFile **ppo) {
   return IFORM_GetProperty(IROOTFORM_TO_IFORM(po), FID_THEME_FILE, (uint32)ppo);
}

static __inline int IROOTFORM_GetThemeFileName(IRootForm *po, const char **name) {
   return IFORM_GetProperty(IROOTFORM_TO_IFORM(po), FID_THEME_FNAME, (uint32)name);
}
static __inline int IROOTFORM_SetThemeFileName(IRootForm *po, const char *name) {
   return IFORM_SetProperty(IROOTFORM_TO_IFORM(po), FID_THEME_FNAME, (uint32)name);
}

static __inline int IROOTFORM_SetDisplay(IRootForm *po, IDisplay *piDisplay) {
   return IFORM_SetProperty(IROOTFORM_TO_IFORM(po), FID_DISPLAY, (uint32)piDisplay);
}

static __inline int IROOTFORM_ApplyWPropsV(IRootForm *po, IWidget *piWidget, VaListPtrType args) {
   WPropDesc desc;
   desc.piWidget = piWidget;
   desc.args = args;
   return IFORM_SetProperty(IROOTFORM_TO_IFORM(po), FID_WPROPS, (uint32)&desc);
}

static __inline int IROOTFORM_ApplyWProps(IRootForm *po, IWidget *piWidget,  ...) {
   int result;
   va_list args;
   va_start(args, piWidget);
   result = IROOTFORM_ApplyWPropsV(po, piWidget, VaListPtr_From_va_list(&args));
   va_end(args);
   return result;
}


/////////////////////////////////////////////////////////////////////////////

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Root Form Interface and Class IDs

Description:
   Each object in the forms interface is identified by a unique interface ID.
   These IDs are used throughout the forms and widgets framework to distinguish
   between the various object and interface types, such as forms, widgets and
   containers.

   The root form interface supports both an interface and the creation of class
   object instances.  The root form interface or a new instance of the root form
   class object may be created by calling ISHELL_CreateInstance() and passing in
   the desired class id of the object.
   
   The root form interface recognizes the following set of class and interface IDs.

Definition:

Identifier         Definition
----------         -----------------------------------
AEEIID_ROOTFORM:   Identifies the root form interface
AEECLSID_ROOTFORM: Identifies the class ID for the root form
   
Comments:
   None

See Also:
   IRootForm Interface
   ISHELL_CreateInstance()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IRootForm Interface

Description:
   IRootForm is the conceptual entity that lends structure and organization to a group of
   forms.  It is the interface that defines the visual components common to a collection
   of forms, such as the title, background and softkeys -- while also managing the order
   and relationship each of its children have with one another, i.e. which form is at the
   top of the display stack, which is next when that form is removed, which is next after
   that, and so forth.
   
   The root form is responsible for providing the base structure upon which an application's
   forms will be built, providing the standard interface components that all child forms will
   leverage, including the background, title and softkey widgets.  These interface elements
   are owned by the root form, with each child form providing the content to be displayed.
   For example, an application for coffee junkies may provide several different forms -- one
   for browsing pictures of exotic beans, one that displays help for caffeine addiction, and
   one for locating your online buddies who can help you score some Indonesian Sumatra.  As
   the user moves from screen to screen the background, title and softkeys could change to
   reflect the current application context.  When looking a the bean list, the title may
   read "Beans" with a lovely semi transparent picture of a sexy bean displayed in the
   background.  Move to the caffeine page and the background becomes a skull and crossbones
   with "Caffeine" in the title, and so forth.  Each of these screens is a separate form, and
   though the content of the title, background and softkeys may differ from form to form, these
   forms share a single instance of the widgets used to present each visual element.  The forms
   provide the content, while the root form provides the structure.
   
   That said, without content, a root form is not really a form at all!  It is a skeleton
   upon which real forms will be built.  In addition to the base visual characteristics of
   background, title and softkeys, the root form is also responsible for providing organization
   to its child forms.  Conceptually, the root form is similar to a window manager in a
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
   the display order of all the frames that have been created, with the form on the top of the
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
  
   The following APIs are provided by the root form interface to support an object reference
   counting mechanism allowing form objects to manage their own memory instances.
===pre>
       IROOTFORM_AddRef()
       IROOTFORM_Release()
===/pre>
      
   The following API is provided to query an object for another API contract from the object
   in question (in this case, the root form interface)
===pre>
       IROOTFORM_QueryInterface()
===/pre>
      
   The following API is provided to support event handling to the root form object:
===pre>
       IROOTFORM_HandleEvent()
===/pre>

   The following APIs are provided to manage activation and deactivation of forms managed by the
   root form:
===pre>
       IROOTFORM_Activate()
       IROOTFORM_Deactivate()
===/pre>
      
   The following APIs provide the ability to manage and manipulate the current form stack:
===pre>
       IROOTFORM_InsertForm()
       IROOTFORM_RemoveForm()
       IROOTFORM_GetForm()
       IROOTFORM_PopForm()
       IROOTFORM_PushForm()
       IROOTFORM_IsFormOnStack()
       IROOTFORM_GetTopForm()
       IROOTFORM_RemoveFormIfTop()
       IFORM_PopSelf()
===/pre>

   The following API is provided to resolve form based URL schemes and create an instance of the
   class ID that will provide the handler for this scheme:
===pre>
       IROOTFORM_ResolveForm()
===/pre>

   The following API is provided to retrieve the client rectangle exposed by the root form:
===pre>
       IROOTFORM_GetClientRect()
===/pre>

Supported Events:
   The root form processes events in three passes.  In the first pass the event will be passed
   to the root form's form widget -- the root container for every form managed by the root form.
   This pass gives the common user interface elements such as the title, background and softkey
   widgets an opportunity to process the event.  If the event is not processed by the root
   container, the root form will attempt to process any property events specifically related to
   the root form.  For example, the root form is responsible for returning the title widget when
   asked to retrieve the WID_TITLE property.  These properties also include those that manage
   form activation and theme usage.  Events not handled during the first or second pass will then
   be handed off to the top form in the root form's form stack.
  
   The events accepted by the root form interface include:

===pre>
Event                 Description
-----                 ------------------------------------------------------------
EVT_WDG_SETPROPERTY:  The root form handler responds to this event by attempting to set the property
                      identified by the 'wParam' parameter.  The following properties may be set by
                      the root form event handler:
                      
                          FID_ACTIVE     --  Sets the activation state of the topmost form and (on
                                             activation) invalidates the root container.
                          FID_THEME_BASENAME --  Sets the theme base for the root form and sends a form
                                             event to listening objects to notify them that form
                                             elements should be re-rendered using the new theme.
                          FID_THEME      --  Requests a form to apply its current theme. The RootForm
                                             will request all forms on the form stack to apply the 
                                             current theme.
                          FID_THEME_FNAME --  Sets the filename used for applying themes. The file name
                                             is used by the currently installed theme file (IResFile). 
                          FID_BACKGROUND --  Sets the background image to be used by forms managed
                                             by the root form.
                          WID_BACKGROUND --  Sets the background widget to be used by forms managed
                                             by the root form, releasing any existing background
                                             widget and replacing it with the widget pointed to by
                                             'dwParam'.
                          WID_TITLE      --  Sets the title widget to be used by forms managed by
                                             the root form, releasing any existing title widget and
                                             replacing it with the widget pointed to by 'dwParam'.
                          WID_SOFTKEYS   --  Sets the softkey widget to be used by forms managed by
                                             the root form, releasing any existing softkey widget and
                                             replacing it with the widget pointed to by 'dwParam'.
                          FID_DISPLAY    --  Sets the display to be used for the RootForm and all the
                                             forms and widgets it manages. The background, title and 
                                             softkey widget are resized horizontally and vertically to
                                             fit the given display size.

EVT_WDG_GETPROPERTY:    The root form handler responds to this event by attempting to retrieve the
                        property identified by the 'wParam' parameter.  The following properties may
                        be retrieved by the root form event handler:
                      
                            FID_THEME_FNAME --  Retrieves the name of the theme file used by the root form.
                                                The string is not copied and should not be freed by the caller.
                            FID_THEME_BASENAME --  Retrieves the theme base used by the root form.
                            FID_THEME_FILE --  Retrieves the current theme file from the RootForm. 
                                               Note that this property can only be retrieved at this time.
                            FID_SHELL      --  Retrieves a reference to the IShell object used by the root
                                               form, incrementing the reference count of the IShell object.
                            WID_TITLE      --  Retrieves a pointer to the root form's title widget, incrementing 
                                               the reference count of the title widget.
                            WID_SOFTKEYS   --  Retrieves a pointer to the root form's softkey widget, incrementing 
                                               the reference count of the softkey widget.
                            WID_BACKGROUND --  Retrieves a pointer to the root form's background widget, incrementing 
                                               the reference count of the background widget.
                            WID_FORM       --  Retrieves the primary widget associated with the root
                                               form, incrementing the reference count of the widget.  Typically 
                                               (though not necessarily), this is the root form's root container.
                            WID_CONTAINER  --  Retrieves the root container, incrementing 
                                               the reference count of the root container.
===/pre>

Properties:
Property            Description
--------            ------------------------------------------------------------
WID_FORM:           This property contains a pointer to the root form's primary widget, which -- in most
                    cases -- will be the root container for all forms managed by the root form.  An
                    application could, however, choose to wrap the root container in another widget
                    (for example, in a border widget) and the WID_FORM property will point to this
                    parent widget.
                             
                        Property Value:  IWidget *
                                    
WID_CONTAINER:      This property contains a pointer to the root form's root container -- the container
                    that serves as the parent for each of the base UI elements provided by the root
                    form (background, title and softkey widgets).
                             
                        Property Value:  IContainer *
                                    
WID_TITLE:          This property contains a pointer to the root form's title widget.  Generally, an
                    application will choose to implement the title widget as a static, image or image
                    static widget to produce the root form's title banner.  An application could,
                    however, implement the title widget using any widget of their choosing.

                    Note: If set, it should be done prior to setting FID_TITLE on
                    the active form as this value will not be copied over to the new widget.                    
                              
                        Property Value:  IWidget *
                                    
WID_SOFTKEYS:       This property contains a pointer to the root form's softkey widget.  Though the
                    root form's softkey widget is commonly implemented as an softkey widget, an
                    application could implement the root form softkey widget using any widget of their
                    choosing.

                    Note: If set, it should be done prior to setting FID_SOFTKEY1 or FID_SOFTKEY2 on
                    the active form as these values will not be copied over to the new widget.

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

FID_ACTIVE:         This property contains the activation state of the root form -- TRUE when the root
                    form is active, FALSE when it is not.  Setting the activation state of the root
                    form will trigger a like event to be sent to the topmost form -- providing an
                    opportunity for the form at the top of the form stack to activate or deactivate its
                    contents.  On activation, the roof form will further invalidate the root container,
                    causing all the child objects to be redrawn.
                             
                        Property Value:  boolean

FID_THEME:          This property has applies the current theme, as indicated by the current themefile, 
                    to the RootForm and all forms on the form stack. This will also trigger an update to 
                    the UI elements managed by the RootForm, such as the title, softkeys and background.
                    This property can only be set. 

                        Property Value: None

FID_DISPLAY:        This property specifies the display to be used for drawing the RootForm, the forms it 
                    manages and all the widgets they contain.
                    This property can only be set.
                             
                        Property Value:  IDisplay *


FID_THEME_FNAME:    This property identifies the name of the theme file to be used by the root form.
                    Setting FID_THEME_FNAME property will set the theme file to be used by the root 
                    form and each of the forms on the form stack and will also trigger an update of 
                    the root form's common UI elements, which will be redrawn using the new theme metrics.  
                             
                        Property Value:  char *

FID_THEME_BASENAME: This property specifies a string that identifies the base location within a theme file 
                    for the RootForm to locate properties. Most, if not all UI elements managed by the RootForm 
                    are themed through the currently activated Form, but there may be UI elements that are globally 
                    themed by the RootForm.  Setting this property will fire a formmodel event notifying 
                    listening objects that the basename of the form has changed.
                             
                        Property Value:  char *

FID_THEME_FILE:     This property retrieves the currently used themefile from the RootForm. The object referred to 
                    is an IResFile interface to reflect the named and numbered resources available for theming. 
                    Note that this form property is currently only retrievable and can not be set.
                             
                        Property Value:  IResFile *

FID_SHELL:          This property contains a reference to the IShell object used by the root form.  The
                    property returns a pointer to the IShell instance in the 'dwParam' parameter passed
                    to the root form with the EVT_WDG_GETPROPERTY event.
                              
                    Forms do not support attempts to set the FID_SHELL property.

                        Property Value:  IShell *

FID_BACKGROUND:     This property contains a pointer to a FormRes data structure which will contain either
                    a reference to a resource containing the background image, or it will contain a pointer
                    to where the IImage is currently loaded into memory.  The root form does not support
                    the ability to retrieve the FID_BACKGROUND property.
                             
                        Property Value:  FormRes *


===========================================================================

IROOTFORM_AddRef()

Description:

   This function increments the reference count of the IRootForm interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IROOTFORM_AddRef(IRootForm *p);

Parameters:
===pre>
   p:  Pointer to the IRootForm interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IRootForm interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IROOTFORM_Release()
   
=======================================================================

IROOTFORM_Release()

Description:

   This function decrements the reference count of the IRootForm interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IROOTFORM_Release(IRootForm *p);

Parameters:
===pre>
   p:  Pointer to the IRootForm interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IRootForm interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IROOTFORM_AddRef()

=======================================================================

IROOTFORM_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IROOTFORM_QueryInterface(IRootForm *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IRootForm interface object.
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

IROOTFORM_HandleEvent()

Description:
   This function receives and handles events passed to the root form from its owner.  The root
   form processes events in three passes.  In the first pass the event will be passed to the
   root form's form widget -- the root container for every form managed by the root form.  This
   pass gives the common user interface elements such as the title, background and softkey
   widgets an opportunity to process the event.  If the event is not processed by the root
   container, the root form will attempt to process any property events specifically related to
   the root form.  For example, the root form is responsible for returning the title widget when
   asked to retrieve the WID_TITLE property.  These properties also include those that manage
   form activation and theme usage.  Events not handled during the first or second pass will then
   be handed off to the top form in the root form's form stack.
   
Prototype:
   boolean IROOTFORM_HandleEvent(IRootForm *p, AEEEvent e, uint16 w, uint32 d);

Parameters:
===pre>   
   p:  A pointer to an IRootForm object.
   e:  The event code being sent to the IRootForm object.
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
    IRootForm Interface
    Widget Events

===========================================================================

IROOTFORM_InsertForm()

Description:
   This function allows an application to insert a form at a specific position within the
   root form's form stack.  Most applications can sufficiently manage the layering of forms
   by using the more convenient IROOTFORM_PopForm() and IROOTFORM_PushForm() functions, to
   place and remove forms from the form stack.  Applications that wish to exercise greater
   control over the ordering of forms on the form stack can used IROOTFORM_Insert() to
   place forms at very specific locations.  For example, when displaying a popup, an
   application could place a new form just below the popup by calling IROOTFORM_InsertForm(),
   thereby maintaining the effect of the popup floating above the primary display content.
   Ordinarily, the new form would have been placed above the popup.
   
   The root form will be updated if the inserted form is visible, either at the top of the
   stack or at a position that is above the topmost visible form -- again, for example, when
   the new form is inserted behind a popup.
   
Prototype:
   int IROOTFORM_InsertForm(IRootForm *p, IForm *f, IForm *pr);

Parameters:
===pre>   
   p:   A pointer to an IRootForm object.
   f:   Pointer to the form to be inserted into the form stack.
   pr:  Pointer to a position within the form stack where the new form will be inserted.  The
        new form will be inserted immediately before the form pointed to by 'pr'.  The
        insertion location may be passed as a constant that identifies a relative position
        within the form stack, as follows:
            FORM_DEFAULT  -  Allow BREW to place the inserted form at its default location.
                             When inserting a form, this places the new form at the top of
                             the form stack.
            FORM_LAST     -  The new form should be inserted at the last form on the form
                             stack -- i.e. this places the form at the top of the form stack
                             as the most recent form.
            FORM_FIRST    -  The new form should be inserted at the first form on the form
                             stack -- i.e. this places the form at the bottom of the form stack
                             as the oldest form.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The form was successfully inserted into the form stack.
   EBADPARM  --  The form pointer passed in 'pr' either does not exist, or refers to a form
                 that is not in the root form's form stack.
   ENOMEMORY --  The form stack could not grow to accomodate a new form.
===/pre>   

Comments:  
   A form should not be inserted if it already exists in the form stack.

Side Effects: 
   The theme used by the root form will automatically be applied to the form being inserted into
   the stack.
   
See Also:
    IRootForm Interface
    IForm Interface
    IROOTFORM_PopForm()
    IROOTFORM_PushForm()
    IROOTFORM_RemoveForm()

===========================================================================

IROOTFORM_RemoveForm()

Description:
   This function allows an application to remove a form at a specific position within the
   root form's form stack.  Most applications can sufficiently manage the layering of forms
   by using the more convenient IROOTFORM_PopForm() and IROOTFORM_PushForm() functions, to
   place and remove forms from the form stack.  Applications that wish to exercise greater
   control over the ordering of forms on the form stack can used IROOTFORM_RemoveForm() to
   remove a form from any position within the form stack.  For example, an application that
   queues up several transient forms may choose to remove them based on the passage of time,
   regardless of whether or not that form is at the top of the form stack.
   
   When removing a form, the root form will be updated if the removed form had previously
   been visible -- i.e. the form was at the top of the stack or was the topmost visible form.
   
Prototype:
   int IROOTFORM_RemoveForm(IRootForm *p, IForm *f);

Parameters:
===pre>   
   po:  A pointer to an IRootForm object.
   f:   Pointer to the form to be removed from the form stack.  The form to be removed may be
        passed as a constant that identifies a relative position within the form stack, as
        follows:
            FORM_DEFAULT  -  Remove the default form, which BREW interprets to be the form at
                             the top of the form stack.
            FORM_LAST     -  Remove the last form on the form stack -- i.e. the form at the
                             top of the form stack.
            FORM_FIRST    -  Remove the first form on the form stack -- i.e. the form at the
                             bottom of the form stack.
            FORM_ALL      -  Remove all of the forms on the form stack.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The form was successfully removed from the form stack.
   EBADPARM  --  The form pointer passed in 'p' either does not exist, or refers to a form
                 that is not in the root form's form stack.
===/pre>   

Comments:  
   None

Side Effects: 
   None
   
See Also:
    IRootForm Interface
    IForm Interface
    IROOTFORM_PopForm()
    IROOTFORM_PushForm()
    IROOTFORM_InsertForm()

===========================================================================

IROOTFORM_GetClientRect()

Description:
   This function retrieves a pointer to the root form's container and the client rectangle.
   The client rectangle defines the area available to clients of the root form for
   creating and managing their own content.  Generally, this area is the extent of the
   root form's container, minus the area reserved for the title and softkey widgets.
   The function can be used to selectively retrieve the root container or the client
   rectangle, by passing NULL for either the IXYContainer or AEERect pointers.
   
Prototype:
   void IROOTFORM_GetClientRect(IRootForm *p, IXYContainer **c, AEERect *r);

Parameters:
===pre>   
   p:  A pointer to an IRootForm object.
   c:  Pointer to an IXYContainer pointer, into which the function will return a pointer
       to the root container of the root form.  When 'c' is NULL the root container will
       not be retrieved.
   r:  Pointer to an AEERect, into which the function will return the rectangle occupied
       by the root form's client area.  When 'r' is NULL the client rectangle will not
       be retrieved.
===/pre>   
   
Return Value:
   A pointer to the root form's container is returned in 'c', while the client rectangle
   is returned in 'r'.  

Comments:  
   None

Side Effects: 
   Retrieving the root container will increment the reference count of the root container
   object.
   
See Also:
    IRootForm Interface
    IXYContainer Interface

===========================================================================

IROOTFORM_GetForm()

Description:
   This function provides facilities that allow an application to retrieve forms stored in
   the root form's form stack.  The function may be instructed to return the form at the
   beginning of the form stack, or from some position relative to another form in the form
   stack -- for example, returning the form that appears before or after another form in
   the stack.  An application can easily iterate over all of the forms in the form stack
   by repeatedly calling IROOTFORM_GetForm() and passing in the form pointer returned from
   the previous call.

Prototype:
   IForm *IROOTFORM_GetForm(IRootForm *p, IForm *pr, boolean n, boolean w);

Parameters:
===pre>   
   p:   A pointer to an IRootForm object.
   pr:  Pointer to a form in the form stack that will be a reference point for the form
        to be retrieved.  For example, if 'pr' points to the second form on the stack and
        the application is requesting the "next" form on the stack, the function will
        return a pointer to the third form.
        When 'pr' is NULL, the function will return either the first form on the stack
        (i.e. the bottom form) or the last form on the stack (i.e. the top form), depending
        on the value of 'n'.  When 'pr' is NULL and 'n' is TRUE, IROOTFORM_GetForm() will
        return the first form.  When 'pr' is NULL and 'n' is FALSE, IROOTFORM_GetForm()
        will return the last form.  
   n:   Specifies whether the function should retrieve the next form on the stack, or the
        previous form on the stack -- always relative to the form pointer passed in 'pr'.
        When 'n' is TRUE the function will retrieve the next form on the stack (i.e. the
        form closer to the top of the stack).  When 'n' is FALSE the function will retrieve
        the previous form on the stack (i.e. the form closer to the bottom of the stack).
   w:   Specifies whether or not the function should wrap when attempting to retrieve the
        next form from the top of the stack, or the previous form from the bottom of the
        stack.  When 'w' is TRUE, the function will wrap at either end of the stack,
        otherwise the function will return NULL indicating that there are no more forms to
        be retrieved.
===/pre>   
   
Return Value:
   A pointer to a form in the form stack, or NULL when there is no form to be returned. 
   
Comments:  
   Though IROOTFORM_GetForm() function returns a pointer to a form, the function does not
   add a reference to the returned IForm for the caller.

   An application may iterate over the entire form stack -- from bottom to top -- with the
   following code:
   
===pre>   
       IForm *pForm = NULL;
       
       while (pForm = IROOTFORM_GetForm (pRootForm, pForm, TRUE, FALSE)) {
          //
          // Do fancy stuff with the form pointer returned in pForm
          //
       }
       // Drops out of the loop once IROOTFORM_GetForm() returns NULL
===/pre>   
   
   To iterate over all the forms on the form stack from top to bottom, the application's
   code would look like the following:
   
===pre>   
       IForm *pForm = NULL;
       
       while (pForm = IROOTFORM_GetForm (pRootForm, pForm, FALSE, FALSE)) {
          //
          // Do fancy stuff with the form pointer returned in pForm
          //
       }
       // Drops out of the loop once IROOTFORM_GetForm() returns NULL
===/pre>   
   
Side Effects: 
   None
   
See Also:
    IRootForm Interface
    IForm Interface

===========================================================================

IROOTFORM_ResolveForm()

Description:
   This function creates a form based on a "form:" URL, returning a pointer to the newly
   create form referenced by the URL.  The URL should identify itself as a "form:" URL
   scheme, including a path that identifies a handler of type AEEIID_FORM.
   
Prototype:
   int IROOTFORM_ResolveForm(IRootForm *po, char const *u, IForm **o);

Parameters:
===pre>   
   p:  A pointer to an IRootForm object.
   u:  Pointer to the URL to be resolved.  The URL should be formatted as a NULL terminated
       string, defining a "form:" URL scheme and a path that identifies an AEEIID_FORM
       handler.
   o:  Pointer to an IForm pointer, into which the function will return a pointer to the form
       created by the function, or NULL if the function fails.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS              --  The form was successfully removed from the form stack.
   EBADPARM             --  The function was called with either a NULL URL scheme, or without
                            defining an IForm pointer into which the function would be
                            returning the created form.
   ESCHEMENOTSUPPORTED  --  The URL passed to the function is ill-formed, either lacking the
                            proper scheme identifier (which must be "form:") or a proper
                            path to an AEEIID_FORM handler.
   ENOMEMORY            --  Insufficient memory to resolve the URL.
===/pre>   

Comments:  
   None

Side Effects: 
   None
   
See Also:
    IRootForm Interface

===========================================================================

IROOTFORM_PopForm()

Description:
   This function pops the form at the top of the form stack, effectively removing that form
   from the stack.  The root form will be updated and redrawn when the topmost form is popped
   off of the form stack.

Prototype:
   int IROOTFORM_PopForm(IRootForm *po);

Parameters:
===pre>   
   p:  A pointer to an IRootForm object.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The form was successfully popped off of the form stack.
   EBADPARM  --  The form stack was empty, so there were not any forms to be popped off of
                 the stack.
===/pre>   

Comments:  
   None

Side Effects: 
   None
   
See Also:
    IRootForm Interface
    IROOTFORM_PushForm()

===========================================================================

IROOTFORM_PushForm()

Description:
   This function pushes a form onto the top of the form stack, making that form the topmost
   form and -- therefore -- the form that will appear on the display.  Pushing a form onto
   the stack will deactivate the form that is currently frontmost on the display (e.g. the
   form that was previously at the top of the stack) and will make the new form the activate
   form.
   
Prototype:
   int IROOTFORM_PushForm(IRootForm *p, IForm *f);

Parameters:
===pre>   
   p:  A pointer to an IRootForm object.
   f:  Pointer to the form to be pushed into the form stack.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS   --  The form was successfully pushed onto the form stack.
   ENOMEMORY --  The form stack could not grow to accomodate a new form.
===/pre>   

Comments:  
   A form should not be pushed if it already exists in the form stack.

Side Effects: 
   The theme used by the root form will automatically be applied to the form being pushed onto
   the stack.
   
See Also:
    IRootForm Interface
    IROOTFORM_PopForm()

==========================================================================

IROOTFORM_TO_IFORM()

Description:
   This function provides type safe casting of an IRootForm pointer to an IForm pointer and
   should be used when passing an IRootForm pointer to functions that require a pointer to
   an IForm.  This is safer than a simple cast, since the compiler will verify the
   pointer type.

Prototype:
   IForm *IROOTFORM_TO_IFORM(IRootForm *me)

Parameters:
===pre>
   me:  Pointer to an IRootForm interface object.
===/pre>

Return Value:
   Returns 'me' cast to an IForm pointer.

Comments:  
   None

Side Effects: 
   None

See Also:
   IRootForm Interface
   IForm Interface

==========================================================================

IROOTFORM_IsFormOnStack()

Description:
   This function checks to see whether or not a given form appears in the form stack managed
   by the root form, returning TRUE when the form is present, FALSE when it is not.

Prototype:
   boolean IROOTFORM_IsFormOnStack(IRootForm *po, IForm *piForm)

Parameters:
===pre>
   po:      Pointer to an IRootForm interface object.
   piForm:  Pointer to the form to look for in the root form's form stack.
===/pre>

Return Value:
===pre>   
   TRUE  --  The form 'piForm' was found in the form stack managed by the root form 'po'.
   FALSE --  The form was not found in the form stack.
===/pre>   

Comments:  
   None

Side Effects: 
   None

See Also:
   IRootForm Interface
   IForm Interface

==========================================================================

IROOTFORM_GetTopForm()

Description:
   This function retrieves a pointer to the form at the top of the root form's form stack.
   Generally, this is a pointer to the form that is currently frontmost on the display.
   Retrieving a pointer to the topmost form on the form stack does not alter the current
   structure of the stack.

Prototype:
   IForm *IROOTFORM_GetTopForm(IRootForm *po)

Parameters:
===pre>
   po:  Pointer to an IRootForm interface object.
===/pre>

Return Value:
   A pointer to the form that is at the top of the form stack, or NULL when there is no form
   to be returned.

Comments:  
   Though IROOTFORM_GetTopForm() function returns a pointer to a form, the function does not
   add a reference to the returned IForm for the caller.

Side Effects: 
   None

See Also:
   IRootForm Interface
   IForm Interface

==========================================================================

IROOTFORM_RemoveFormIfTop()

Description:
   This function will remove a specified form from the form stack, if that form is currently
   at the top of the stack.

Prototype:
   boolean IROOTFORM_RemoveFormIfTop(IRootForm *po, IForm *piForm)

Parameters:
===pre>
   po:      Pointer to an IRootForm interface object.
   piForm:  Pointer to the form to be removed from the form stack -- but only if that form is
            also the form at the top of the stack.
===/pre>

Return Value:
===pre>   
   TRUE  --  The specified form was at the top of the form stack and was successfully removed
             from the stack.
   FALSE --  The specified form was not removed from the form stack.
===/pre>   

Comments:  
   None

Side Effects: 
   None

See Also:
   IRootForm Interface
   IForm Interface
   IROOTFORM_RemoveForm()

==========================================================================

IFORM_PopSelf()

Description:
   This function allows a form to remove itself from the top of the root form's form
   stack -- a task that normally is the responsibility of the root form itself.  The
   form will be removed from the form stack only if it is currently the topmost form
   on the stack, otherwise, the function returns without affecting the form stack at
   all.

Prototype:
   void IFORM_PopSelf(IForm *piForm)

Parameters:
===pre>
   piForm:  Pointer to the form to be popped off of the form stack -- but only if that
            form is also the form at the top of the stack.
===/pre>

Return Value:
===pre>   
   TRUE  --  The specified form was at the top of the form stack and was successfully removed
             from the stack.
   FALSE --  The specified form was not removed from the form stack.
===/pre>   

Comments:  
   An application may call IFORM_PopSelf(), rather than IROOTFORM_RemoveForm(),
   IROOTFORM_PopForm(), or IROOTFORM_RemoveFormIfTop() -- all of which require a
   pointer to the root form that manages the form stack that includes the form to
   be removed.

Side Effects: 
   None

See Also:
   IForm Interface
   IROOTFORM_PopForm()
   IROOTFORM_RemoveForm()
   IROOTFORM_RemoveFormIfTop()

===========================================================================

IROOTFORM_Activate()

Description:
   This function will request that the root form activate itself, which in turn will cause the
   form at the top of the root form's form stack to be activated.  For example, a suspended
   application might choose to activate its root form in response to an EVT_APP_RESUME event.
   Calling IROOTFORM_Activate() will allow the topmost form to activate its controls and
   widgets when the application has regained control of the device display.  Activating the
   root form sets a domino of activations to the topmost form, it's root container, and each
   of the child objects provided by the form.
   
Prototype:
   void IROOTFORM_Activate(IRootForm *po); 

Parameters:
===pre>
   po:  Pointer to the IRootForm interface object that has become active.
===/pre>

Return Value:
   None

Comments:  
   None

Side Effects: 
   Calling IROOTFORM_Activate() effectively sets the FID_ACTIVE property on the root form,
   passing TRUE as the current activation state.

See Also:
   IROOTFORM_Deactivate()

===========================================================================

IROOTFORM_Deactivate()

Description:
   This function will request that the root form deactivate itself, which in turn will cause
   the form at the top of the root form's form stack to be deactivated.  For example, when a
   running application is suspended, the application might choose to deactivate its root form
   in response to an EVT_APP_SUSPEND event.  Calling IROOTFORM_Deactivate() will allow the
   topmost form to deactivate its controls and widgets when the application has given up control
   of the device display. Deactivating the root form sets a domino of deactivations to the
   topmost form, it's root container, and each of the child objects provided by the form.

Prototype:
   void IROOTFORM_Deactivate(IRootForm *po); 

Parameters:
===pre>
   po:  Pointer to the IRootForm interface object that has been deactivated.
===/pre>

Return Value:
   None

Comments:  
   None

Side Effects: 
   Calling IROOTFORM_Deactivate() effectively sets the FID_ACTIVE property on the root form,
   passing FALSE as the current activation state.

See Also:
   IROOTFORM_Activate()

===========================================================================

IROOTFORM_GetThemeFile()

Description:
   This function will retrieve the currently used theme file from the RootForm. The 
   returned theme file can be used to retrieve numbered or named resources from the 
   current themefile.

   The common use of this function is in reaction to the FID_THEME property to load named
   resources from the theme file such as images that are needed from the theme. 

Prototype:
   int IROOTFORM_GetThemeFile(IRootForm *po, IResFile **ppo); 

Parameters:
===pre>
   po:  Pointer to the IRootForm interface object that has been deactivated.
   ppo: Pointer to the destination for the IResFile object handle
===/pre>

Return Value:
   SUCCESS, themefile retrieved
   EFAILED, themefile not set in RootForm or unable to return 

Comments:  
   None

Side Effects: 
   None

See Also:
   IROOTFORM_SetThemeFileName()
   IRootForm_ApplyWProps()
   IResFile Interface

===========================================================================

IROOTFORM_GetThemeFileName()

Description:
   This function will retrieve the currently used theme filename from the RootForm. 
   Ownership of the  returned character pointer is maintained by the RootForm.

Prototype:
   int IROOTFORM_GetThemeFileName(IRootForm *po, const char **name); 

Parameters:
===pre>
   po:  Pointer to the IRootForm interface object that has been deactivated.
   name: Pointer to the destination for the string pointer
===/pre>

Return Value:
   SUCCESS, theme filename retrieved
   EFAILED, theme filename not set in RootForm or unable to return 

Comments:  
   None

Side Effects: 
   None

See Also:
   IROOTFORM_SetThemeFileName()
   IResFile Interface

===========================================================================

IROOTFORM_SetThemeFileName()

Description:
   This function set the current theme used by the RootForm to the specified filename. 
   The specified theme filename is maintained in the RootForm and all forms on the formstack 
   are re-themed with the content in the specified theme file.

Prototype:
   int IROOTFORM_SetThemeFileName(IRootForm *po, const char *name); 

Parameters:
===pre>
   po:  Pointer to the IRootForm interface object that has been deactivated.
   name: Pointer to the theme filename string
===/pre>

Return Value:
   SUCCESS, theme filename retrieved
   EFAILED, theme filename not settable in RootForm or unable to set or duplicate 

Comments:  
   None

Side Effects: 
   None

See Also:
   IROOTFORM_GetThemeFileName()
   IResFile Interface

===========================================================================

IROOTFORM_SetDisplay()

Description: 
   This function sets the display to be used for drawing the RootForm,
   its widgets and all the forms and widgets it manages. The
   background, title and softkeys are resized and positioned to fit
   the given display size. Note that the height of the softkeys and
   title is not affected by this.

Prototype:
   int IROOTFORM_SetDisplay(IRootForm *po, IDisplay *piDisplay); 

Parameters:
===pre>
   po:  Pointer to the IRootForm interface object that has been deactivated.
   piDisplay: Pointer to the display to set drawing to. 
===/pre>

Return Value:
   SUCCESS, display set
   EFAILED, display could not be set  

Comments:  
   None

Side Effects: 
   Resizes the background, softkey and title widgets to not fall outside the 
   bounds of the given display

See Also:
   None

===========================================================================

IROOTFORM_ApplyWProps()

Description:
   This utility function applies a named set of widget properties a specified widget using 
   the current theme file. Usually the properties for a widget specific to a form is  
   prefixed by the basename of the form. 

   An example of applying properties to a widget:

===pre>
   const char *pBaseName;
   if (SUCCESS == IFORM_GetThemeBaseName(me->piForm, &pBaseName)) {
      IROOTFORM_ApplyWProps(me->piRoot, me->piTitle, pBaseName, "Title", "Properties");
   }
===/pre>

Prototype:
   int IROOTFORM_ApplyWProps(IRootForm *po, IWidget *piw, ...); 

Parameters:
===pre>
   po:  Pointer to the IRootForm interface object that has been deactivated.
   piw: Pointer to the widget to apply properties to
===/pre>

Return Value:
   SUCCESS, properties found and applied to widget
   EFAILED, invalid params or properties not found

Comments:  
   This function is a wrapper around IROOTFORM_ApplyWPropsV which takes varargs. This function 
   in turn sets the FID_WPROPS form property.

   Several forms have default theme properties that will be applied if they exist in the current theme file 
   and appropriate widgets are assigned to the form. 
   Each of these entries will be appended to the current theme base name.

   Dialogs (AEEDialog.h): 
===pre>
      "Dialog.Background.Image" (Background image widget)
      "Dialog.Background.Properties" (Background widget properties)
      "Dialog.Properties" (Main widget properties)
      "Dialog.Title.Properties" (Title widget properties)
      "Dialog.Content.Properties" (Static dialog content properties)
      "Dialog.ProgressBar.Properties" (Progress dialog progress bar properties)
===/pre>

   Lists (AEEListForm.h):
===pre>
      "List.Scrollbar.Properties" (Scrollbar widget properties)
      "List.Properties" (List widget properties)
      "List.Item.Properties" (List item widget properties)
===/pre>

   Popups (AEEPopup.h):
===pre>
      "Backdrop.Properties" (Backdrop widget properties)
      "List.Scrollbar.Properties" (Scrollbar widget properties)
      "List.Properties" (List widget properties)
      "List.Item.Properties" (List item widget properties)
===/pre>

   Default Forms (AEEForm.h, AEERootForm.h):
===pre>
      "Background.Image" (Default form image widget)
      "Properties" (Default form properties)
      "Title.Properties" (Title widget properties)
      "Softkeys.Properties" (Softkey widget properties)
      "Background.Properties" (Background widget properties)
===/pre>
   

Side Effects: 
   None

See Also:
   IROOTFORM_GetThemeFile()
   IResFile Interface

*/

#endif /* __AEEROOTFORM_H__ */
