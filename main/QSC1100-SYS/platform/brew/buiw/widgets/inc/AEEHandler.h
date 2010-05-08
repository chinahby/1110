/*
  ========================================================================

  FILE:  AEEHandler.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Handler interface for widgets and forms

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEEHANDLER_H__
#define __AEEHANDLER_H__

#include "AEE.h"


typedef boolean (*PFNHANDLER)     (void *pCxt, AEEEvent evt, uint16 wParam, uint32 dwParam);
typedef void    (*PFNFREEHANDLER) (void *pCxt);


typedef struct {
   PFNHANDLER        pfn;
   void *            pCxt;
   PFNFREEHANDLER    pfnFree;
} HandlerDesc;


#define HANDLERDESC_Init(pdesc,ph,pv,pf) \
   (pdesc)->pfn = (PFNHANDLER)(ph), (pdesc)->pCxt = (pv), (pdesc)->pfnFree = (PFNFREEHANDLER)(pf)


#define HANDLERDESC_Call(pdesc,e,w,dw) \
   ( ((pdesc)->pfn) ? (pdesc)->pfn((pdesc)->pCxt,(e),(w),(dw)) : FALSE )


#define HANDLERDESC_Free(pdesc) \
   if ((pdesc)->pfnFree) (pdesc)->pfnFree((pdesc)->pCxt)



/////////////////////////////////////////////////////////////////////////////
// IHandler interface

#include "bid/AEEIID_HANDLER.bid"

typedef struct IHandler IHandler;

#define INHERIT_IHandler(iname) \
   INHERIT_IQueryInterface(iname);\
   boolean  (*HandleEvent)    (iname *, AEEEvent evt, uint16 wParam, uint32 dwParam);\
   void     (*SetHandler)     (iname *, HandlerDesc *pDesc)

#define IHANDLER_HandleEvent(p,e,w,d)  AEEGETPVTBL((p),IHandler)->HandleEvent(p,e,w,d)
#define IHANDLER_SetHandler(p,pd)      AEEGETPVTBL((p),IHandler)->SetHandler(p,pd)


AEEINTERFACE(IHandler) {
   INHERIT_IHandler(IHandler);
};


/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Handler Interface ID

Description:
   Each class and interface within the widget framework is identified by a unique
   ID.  These IDs are used throughout the framework to distinguish between various
   types of objects, such as containers and widgets.

   The handler interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as the base interface for all handlers.  For example, the handler
   interface is used by the container, decorator, draw decorator and widget
   interfaces to register various callback functions with a specific interface.

   The handler interface is identified by the interface ID below.

Definition:

Identifier       Definition
----------       -----------------------------------
AEEIID_HANDLER:  Identifies the handler interface

Comments:
   None

See Also:
   IHandler Interface
   IContainer Interface
   IDecorator Interface
   IDrawDecorator Interface
   IWidget Interface

=======================================================================

PFNHANDLER

Description:
   This type definition specifies the prototype for the function pointer an application
   may register as a custom event handler for an object.  The application may hook a custom
   event handling routine into the normal event handling chain, providing the opportunity to
   either override or alter the normal event handling mechanism for a given object.  For
   example, an object wishing to handle the pressing of a specific key in a non-standard
   way (every press of the '*' key would, for example, cause the application to play the
   sound of a duck quacking) could register a custom event handler that would trap for key
   events and act accordingly when that specific key is pressed.  The custom event handler
   would perform its action and could choose to end normal event handling at that point by
   returning TRUE (or FALSE, if the action taken merited further event processing).  Or,
   the custom event handler could pass the event onto the next event handler in the chain
   by calling HANDLERDESC_Call().
   
   The widgets framework will call the PFNHANDLER with the same parameters normally passed
   into a BREW event handler, and additionally passes an extra parameter that identifies a
   pointer to private data owned and managed by the application.  The custom event handler
   may use this pointer to access private data that has been created by the object
   implementing the handler interface.

Definition:
    typedef boolean (*PFNHANDLER)(void *pCxt, AEEEvent evt, uint16 wParam, uint32 dwParam);

Members:
===pre>
   pCxt:     A pointer to private data allocated and managed by the caller that
             has registered the event handler.  This data pointer is passed into
             the event handler, providing the callback with access to this private
             storage.
   evt:      The event code being sent into the event handler.
   wParam:   16-bit event-specific parameter.
   dwParam:  32-bit event-specific parameter.
===/pre>
   
Return Value:
===pre>
   TRUE  --  The custom event handler was able to handle the event.
   FALSE --  The event was not handled by the custom event handler.
===/pre>

Comments:
    The custom event handler is registered by an object implementing the handler interface
    by initializing the 'pfn' field of a HandlerDesc data structure and calling that object's
    implementation of the IHANDLER_SetHandler() function.  This will hook the custom event
    handler into that object's normal event handling process.  Upon registering the event
    handler, IHANDLER_SetHandler() will return a pointer to a HandlerDesc data structure that
    contains a pointer to the event handler that would have been called had the new handler
    not been registered.  The PFNHANDLER function should pass this HandlerDesc pointer to the
    old event handler by calling HANDLERDESC_Call() in order to ensure that normal event
    processing will still take place. 

See Also:
   ICanvas Interface
   IHANDLER_SetHandler()
   HANDLERDESC_Call()
   HandlerDesc

=======================================================================

PFNFREEHANDLER

Description:
   This type definition specifies the prototype for the function pointer an application
   may register for the purpose of releasing private storage allocated for use specifically
   by a custom event handler.  The private data utilized by an object implementing the
   handler interface is considered to be opaque, and could point to an arbitrarily complex
   data structure with its own unique storage requirements.  The PFNFREEHANDLER callback
   function allows a handler to release "deep storage.  This memory free handler will be
   called upon releasing the object implementing the handler interface.  The pointer passed
   into the PFNFREEHANDLER callback may be freed in whatever manner is appropriate for the
   needs of the storage block.  For example, then this pointer accesses a data structure
   containing "deep storage", the PFNFREEHANDLER would likely free each of the memory blocks
   pointed to from fields within this complex structure.

Definition:
    typedef void (*PFNFREEHANDLER)(void *pCxt);

Members:
===pre>
   pCxt:  A pointer to private data allocated and managed by the caller that
          has registered a custom handler.  This data pointer will be passed
          into the PFNFREEHANDLER to allow the callback to free any complex
          memory allocations associated with the storage block.
===/pre>

Comments:
    The PFNFREEHANDLER is registered by an object implementing the handler interface by
    initializing the 'pfnFree' field of a HandlerDesc data structure and calling that object's
    implementation of the IHANDLER_SetHandler() function.  This will hook the PFNFREEHANDLER
    into that object's object release process.  Upon registering the PFNFREEHANDLER, 
    IHANDLER_SetHandler() will return a pointer to a HandlerDesc data structure that
    contains a pointer to the memory free that would have been called had the new handler
    not been registered.  The PFNFREEHANDLER function should pass this HandlerDesc pointer to
    the old memory free handler by calling HANDLERDESC_Free() in order to ensure that normal
    memory disposal will still take place.  
    
    When the installed free function is called by the IHandler, it must NOT call back into the 
    object through the context pointer (pCxt). For example, in a free function installed for 
    a form, you must not call an IForm function with the form pointer, because the form is in 
    the process of being destroyed and is in an indeterminate state.  The free function should 
    only use the pointer to access the installed handler's own members that need to be 
    cleaned up.


See Also:
   IHandler Interface
   IHANDLER_SetHandler()
   HANDLERDESC_Free()
   HandlerDesc

=============================================================================

HandlerDesc

Description:
   This data type defines the callback handlers a object implementing the handler interface
   will use to handle events and -- when the object is finally released -- free any private
   storage allocated by the object.

Definition:
   typedef struct {
      PFNHANDLER        pfn;
      void             *pCxt;
      PFNFREEHANDLER    pfnFree;
   } HandlerDesc;

Members:
===pre>
   pfn:      Pointer to the callback function that BREW will call when handling
             events sent to this object.  This routine will override the default 
             event handling routine, allowing an application to perform custom
             event handling for the object.
   pCxt:     A pointer to private data allocated and managed by the caller that
             has registered the handler.  This data pointer is passed into the
             event handler, as well as the free handler, providing each of these
             callbacks with access to this private storage.
   pfnFree:  Pointer to the callback function that BREW will call when freeing the
             private memory associated with the handler.  This gives the object
             the opportunity to release any "deep storage" that may have been
             allocated by the object.
===/pre>
Comments:
   A pointer to a HandlerDesc data structure is passed to IHANDLER_SetHandler() to
   register the callbacks an object implementing the handler interface will use to
   perform custom event handling and memory free routines.

See Also:
   PFNHANDLER
   PFNFREEHANDLER
   IHANDLER_SetHandler()

===================================================================================
   MACRO DOCUMENTATION
===================================================================================

HANDLERDESC_Init()

Description:
   This macro initializes each of the fields of a HandlerDesc data structure.  An
   application implementing the handler interface that wishes to provide its own event
   handling and memory free callback routines should call HANDLERDESC_Init() to initialize
   both the event handler, the storage used by the event handler, and the callback
   routine that will be responsible for freeing the memory used by the private storage.

Prototype:
   HANDLERDESC_Init(HandlerDesc *pdesc, PFNHANDLER ph, void *pv, PFNFREEHANDLER pf);

Parameters:
===pre>
   pdesc:  Pointer to a HandlerDesc data structure, into which the macro will
           copy the remaining parameters.
   ph:     Pointer to the callback function that BREW will call when processing
           events for this object.  This routine will override the default event
           handler for the object, allowing an application to handle various
           events in a custom manner.
   pv:     A pointer to private data allocated and managed by the caller that
           has registered the handler.  This data pointer is passed into the
           custom event handler, as well as the free handler, providing each of
           these callbacks with access to this private storage.
   pf:     Pointer to the callback function that BREW will call when freeing the
           memory associated with a handler upon release of the object implementing
           the handler interface.  This gives the object the opportunity to release
           any private "deep storage" that may have been allocated by the object.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   HandlerDesc
   PFNHANDLER
   PFNFREEHANDLER

===================================================================================

HANDLERDESC_Call()

Description: 
   
   This macro calls the event handler function contained in the referenced 
   HandlerDesc data structure.  An application that overrides an object's 
   default event handler by supplying its own custom event handler should 
   ALWAYS call HANDLERDESC_Call() within the custom event handler, passing in 
   a pointer to the handler descriptor returned by IHANDLER_SetHandler() when 
   the custom event handler was initially registered.  The application 
   providing a custom event handler consequently has the option of applying 
   its own event filtering prior to, or after, the default event handler, 
   depending on the needs of that particular object.  Any hooked event handler 
   may then, therefore, be used for pre or post event handling.  

Prototype:
   HANDLERDESC_Call(HandlerDesc *pdesc, AEEEvent e, uint16 w, uint32 dw);

Parameters:
===pre>
   pdesc:  Pointer to a HandlerDesc data structure that contains a pointer
           to the event handler BREW should execute.
   e:      The event code being sent into the event handler.
   w:      16-bit event-specific parameter.
   dw:     32-bit event-specific parameter.
===/pre>

Return Value:
===pre>
   TRUE  --  The custom event handler was able to handle the event.
   FALSE --  The event was not handled by the custom event handler.
===/pre>

Comments:
   None

Side Effects: 
   None

See Also:
   HandlerDesc
   IHANDLER_SetHandler()


===================================================================================

HANDLERDESC_Free()

Description: 
   
   This macro calls the free function contained in the referenced 
   HandlerDesc data structure.  An application that intalls a free function
   when overriding the default event handler should call HANDLERDESC_Free() at 
   the end of that free function to ensure that the previous handler's allocated 
   context memory is cleaned up.

Prototype:
   HANDLERDESC_Free(HandlerDesc *pdesc);

Parameters:
===pre>
   pdesc:  Pointer to a HandlerDesc data structure that contains a pointer
           to the free function to call.
===/pre>

Return Value:
   None
   
Comments:
   When the installed free function is called by the IHandler, it must NOT call 
   back into the object through the context pointer (pCxt). For example, in a free 
   function installed for a form, you must not call an IForm function with the 
   form pointer, because the form is in the process of being destroyed and is in 
   an indeterminate state.  The free function should only use the pointer to access 
   the installed handler's own members that need to be cleaned up.

Side Effects: 
   None

See Also:
   HandlerDesc
   IHANDLER_SetHandler()
   PFNFREEHANDLER


===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IHandler Interface

Description:
   IHandler is an abstract interface meant to be a base from which more complex
   object implementations are based.  Implementations of the handler interface
   include the decorator interface, the draw decorator interface, and the widget
   interface.  A handler is, essentially, a mechanism that provides customized
   event handling to another object.  An application could, for example, provide
   a customized event handler to a container object for the purpose of handling
   certain events in very specific ways -- for example, handling (or even ignoring)
   attempts to set various widget properties in a special manner.
   
   Objects implementing the handler interface must provide their own implementation
   of each function defined by the interface.
   
Supported Events:
   The base handler interface may process any BREW event, as required by the object
   implementing the handler interface.

See Also:
   IDecorator Interface
   IDrawDecorator Interface
   IWidget Interface
    
=======================================================================

IHANDLER_HandleEvent()

Description:
   This function receives and handles events passed to an object implementing the
   handler interface from its owner.  Objects implementing the handler interface
   must provide their own implementation of IHANDLER_HandleEvent().  For example,
   the decorator widget supplies its own event handler, IDECORATOR_HandleEvent(),
   which passes all received events directly to the child widget wrapped by the
   decorator object.

Prototype:
   boolean IHANDLER_HandleEvent(IHandler *p, AEEEvent e, uint16 w, uint32 d);

Parameters:
===pre>   
   p:  A pointer to an IHandler object.
   e:  The event code being sent to the IHandler object.
   w:  16-bit event-specific parameter.
   d:  32-bit event-specific parameter.
===/pre>   
   
Return Value:
===pre>
   TRUE  --  If the event was handled by the object.
   FALSE --  If the event was not handled.
===/pre>

Comments:
   None

Side Effects: 
   None
   
See Also:
    Widget Events
    IHandler Interface
    IDECORATOR_HandleEvent()
   
   
======================================================================

IHANDLER_SetHandler()

Description: 
   This function allows an application to subclass an object by installing a 
   new event handler.  IHANDLER_SetHandler() is passed a pointer to a handler 
   descriptor which contains a pointer to the new event handler, an optional 
   context pointer, and an optional callback pointer used to free private 
   memory associated with the object.  On return, the handler descriptor will 
   be filled with the previous event handler, context pointer and free 
   callback (usually NULL).  
   
   The new event handler must pass unhandled events to the previous event 
   handler by calling the previous event handler stored in the handler 
   descriptor using the macro HANDLERDESC_Call().  

   Applications can 'chain' multiple event handlers on an object by calling 
   IHANDLER_SetHandler() multiple times with different HandlerDesc data 
   structures.  

   To uninstall a handler from the chain, call IHANDLER_SetHandler() a second 
   time with the same HandlerDesc data structure.  This will swap the new and 
   previous event handlers back to the state they were before the handler was 
   installed.  If multiple handlers are installed, they must be uninstalled in 
   the reverse order from which they were installed.  The following example 
   may help to illustrate.  (Note that chaining multiple handlers in this way 
   is not likely to be useful, it is only shown here to demonstrate the 
   mechanics of multiple handlers.) 

===pre>   
   HandlerDesc hdNew1, hdNew2, hdNew3;

   // install 3 handlers
   IFORM_SetHandler(po, &hdNew1);
   IFORM_SetHandler(po, &hdNew2);
   IFORM_SetHandler(po, &hdNew3);
   
   // later, undo the above handler chaining
   IFORM_SetHandler(po, &hdNew3);
   IFORM_SetHandler(po, &hdNew2);
   IFORM_SetHandler(po, &hdNew1);
===/pre>   
   
Prototype:
   void IHANDLER_SetHandler(IHandler *p, HandlerDesc *pd);

Parameters:
===pre>   
   p:    A pointer to an IHandler object.
   phd:  Pointer to the data structure that contains the event handler, 
         context pointer and a callback used to free private memory.
===/pre>   

Return Value: 
   IHANDLER_SetHandler() fills the HandlerDesc data structure pointed to by 
   'pd' with the previous event handler, context pointer and free callback.  
   The application should save the contents of the HandlerDesc data structure 
   and pass unhandled events to the previous event handler and context 
   pointer. The macro HANDLERDESC_Call() may be used as a convenience for this
   call.

Comments:
   Applications may reset the object's event handler, context pointer and free 
   pointer by passing in NULL for the 'pd' parameter.  
  
   The new event handler must pass unhandled events to the previous event 
   handler so that all installed handlers have a chance to handle the event.  

Side Effects: 
   None
   
See Also:
   IHandler Interface
   HandlerDesc
   HANDLERDESC_Call() 

*/

#endif   //__AEEHANDLER_H__
