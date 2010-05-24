/*
  ========================================================================

  FILE:  AEEDrawHandler.h
  
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

#ifndef __AEEDRAWHANDLER_H__
#define __AEEDRAWHANDLER_H__

#include "AEE.h"
#include "bid/AEEIID_DRAWHANDLER.BID"


typedef void (*PFNDRAWHANDLER) (void *pCxt, ICanvas *piCanvas, int x, int y);


typedef struct {
   PFNDRAWHANDLER    pfn;
   void *            pCxt;
   PFNFREEHANDLER    pfnFree;
} DrawHandlerDesc;


#define DRAWHANDLERDESC_Init(pdesc,ph,pv,pf) \
   (pdesc)->pfn = (PFNDRAWHANDLER)(ph), (pdesc)->pCxt = (pv), (pdesc)->pfnFree = (PFNFREEHANDLER)(pf)

#define DRAWHANDLERDESC_Call(pdesc,canv,x,y) \
   if ((pdesc)->pfn) (pdesc)->pfn((pdesc)->pCxt,(canv),(x),(y));

#define DRAWHANDLERDESC_Free(pdesc) \
   if ((pdesc)->pfnFree) (pdesc)->pfnFree((pdesc)->pCxt);


#define INHERIT_IDrawHandler(iname) \
   INHERIT_IQueryInterface(iname); \
   void  (*SetDraw)(iname *, DrawHandlerDesc *pDesc)


AEEINTERFACE_DEFINE(IDrawHandler);


#define IDRAWHANDLER_AddRef(p)                    AEEGETPVTBL((p),IDrawHandler)->AddRef(p)
#define IDRAWHANDLER_Release(p)                   AEEGETPVTBL((p),IDrawHandler)->Release(p)
#define IDRAWHANDLER_QueryInterface(p,i,p2)       AEEGETPVTBL((p),IDrawHandler)->QueryInterface(p,i,p2)
#define IDRAWHANDLER_SetDraw(p,pd)                AEEGETPVTBL((p),IDrawHandler)->SetDraw(p,pd) 

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

PFNDRAWHANDLER

Description:
   This type definition specifies the prototype for the function pointer an application 
   may register as the draw handler for a widget.  The draw handler allows an allocation 
   to hook a custom drawing routine into the default draw operation normally invoked for 
   a widget.  For example, an application could choose to enhance the display of a 
   widget by drawing a translucent winking eyeball in the background (ooooo!  ahhhh!).  
   This custom draw handler will be called whenever the widget is asked to draw itself.  
   The handler is NOT intended to override and replace an object's normal draw handler.  
   Rather, it is intended to perform additional drawing and should itself call 
   DRAWHANDLERDESC_Call() to continue the chain of drawing operations.  
   
   The widget framework will call the PFNDRAWHANDLER with the same parameters normally 
   passed to IWIDGET_Draw(), and additionally passes an additional parameter that 
   identifies a pointer to private data owned and managed by the application, which the 
   application provides to the widget at the time the draw handler is registered.  

Definition:
    typedef void (*PFNDRAWHANDLER)(void *pCxt, ICanvas *piCanvas, int x, int y);

Members:
===pre>
   pCxt:      A pointer to private data allocated and managed by the caller that
              has registered the draw handler with the widget.  This data
              pointer is passed into the draw handler, providing the callback with
              access to this private storage.
   piCanvas:  Pointer to the ICanvas interface that will provide drawing services
              to the widget.
   x:         Horizontal position of the left side of the widget, offset from
              the left side of the ICanvas object.
   y:         Vertical position of the top of the widget, offset from the left
              side of the ICanvas object
===/pre>

Comments:
   The draw handler is registered with a widget by initializing the 'pfn' field of a 
   DrawHandlerDesc data structure and calling IDRAWHANDLER_SetDraw() to hook the draw 
   handler into the widget's normal drawing process.  Upon registering the draw handler, 
   IDRAWHANDLER_SetDraw() will fill the DrawHandlerDesc data structure with the draw 
   handler that would have been called had the new handler not been registered.  The 
   PFNDRAWHANDLER function should pass this DrawHandlerDesc pointer to the old draw 
   handler when calling DRAWHANDLERDESC_Call() in order to ensure that normal widget 
   drawing still takes place.  

See Also:
   ICanvas Interface
   DrawHandlerDesc
   IDRAWHANDLER_SetDraw()
   DRAWHANDLERDESC_Call()

=============================================================================

DrawHandlerDesc

Description:
   This data type defines the callback handlers a custom draw handler will use to draw 
   itself and -- when the custom handler is finally released -- free any private storage 
   allocated by the application.

Definition:
   typedef struct {
      PFNDRAWHANDLER    pfn;
      void             *pCxt;
      PFNFREEHANDLER    pfnFree;
   } DrawHandlerDesc;

Members:
===pre>
   pfn:      Pointer to the callback function that BREW will call when drawing
             the widget.  This routine will override the default drawing 
             routine for the widget, allowing an application to apply a custom 
             rendering technique to the widget.
   pCxt:     A pointer to private data allocated and managed by the caller that
             has registered the draw handler with the widget.  This data pointer is 
             passed into the draw handler, as well as the free handler, providing each 
             of these callbacks with access to this private storage.
   pfnFree:  Pointer to the callback function that BREW will call when freeing the 
             memory associated with a draw handler upon releasing the object.
             This gives the draw handler the opportunity to release any private
             "deep" storage that may have been allocated by the caller.
===/pre>
Comments:
   A pointer to a DrawHandlerDesc data structure is passed to IDRAWHANDLER_SetDraw() to
   register the callbacks a widget will use to perform custom drawing and memory
   free routines.

See Also:
   PFNDRAWHANDLER
   PFNFREEHANDLER
   IDRAWHANDLER_SetDraw()

===================================================================================
   MACRO DOCUMENTATION
===================================================================================

DRAWHANDLERDESC_Init()

Description:
   This macro initializes each of the fields of a DrawHandlerDesc data structure.  An
   application implementing a custom draw handler should call DRAWHANDLERDESC_Init() 
   before calling IDRAWHANDLER_SetHandler()..

Prototype:
   DRAWHANDLERDESC_Init(DrawHandlerDesc *pdesc, PFNDRAWHANDLER ph, void *pv, PFNFREEHANDLER pf);

Parameters:
===pre>
   pdesc:  Pointer to a DrawHandlerDesc data structure, into which the macro will
           copy the remaining parameters.
   ph:     Pointer to the callback function that BREW will call when drawing the
           the draw handler.  This routine will override the default drawing routine
           for the widget, allowing an application to apply a custom rendering
           technique to the widget.
   pv:     A pointer to private data allocated and managed by the caller that
           has registered the draw handler with the widget.  This data pointer is passed 
           into the draw handler, as well as the free handler, providing each of these 
           callbacks with access to this private storage.
   pf:     Pointer to the callback function that BREW will call when freeing the
           memory associated with a widget upon releasing the object. This gives the 
           widget the opportunity to release any private "deep" storage that may have been 
           allocated by the caller.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   DrawHandlerDesc
   PFNDRAWHANDLER
   PFNFREEHANDLER

===================================================================================

DRAWHANDLERDESC_Call()

Description:
   This macro issues the drawing callback referenced by the draw handler descriptor
   passed to the function in the 'pdesc' parameter.  An application that overrides a
   widget's default draw routine by supplying a custom draw handler should ALWAYS
   call DRAWHANDLERDESC_Call() within its draw routine, passing in a pointer to the
   draw handler descriptor returned by IDRAWHANDLER_SetDraw() when the current
   draw handler was initially registered.  The application providing a custom draw
   handler consequently has the option of applying its own drawing operations prior
   to, or after, the default draw operation, depending on the needs of that particular
   draw handler.

Prototype:
   DRAWHANDLERDESC_Call(DrawHandlerDesc *pdesc, ICanvas *canv, int x, int y);

Parameters:
===pre>
   pdesc:  Pointer to a DrawHandlerDesc data structure that contains a pointer
           to the draw handler BREW should execute.
   canv:   Pointer to the ICanvas object onto which the draw handler will perform
           its drawing operations
   x:      Horizontal position of the left side of the widget, offset
           from the left side of the ICanvas object.
   y:      Vertical position of the top of the widget, offset from the
           top of the ICanvas object.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   DrawHandlerDesc
   IDRAWHANDLER_SetDraw()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IDrawHandler Interface

Description: 
   An application wishing to hook a custom draw handler into the drawing chain for a 
   widget may QueryInterface for the IDrawHandler interface (AEEIID_DRAWHANDLER) and 
   then register its draw handler by calling IDRAWHANDLER_SetDraw().

   The call to IDRAWHANDLER_SetDraw() swaps the contents of the DrawHandlerDesc data 
   structure, effectively filling the structure with default draw handler (i.e.  the 
   draw handler that would be called if a custom handler had not been attached to the 
   widget).  The application should preserve the contents of this structure and pass it 
   to DRAWHANDLERDESC_Call() within the custom draw handler itself to insure that the 
   drawing chain continues.  The application may choose to perform its drawing either 
   prior to or following the call to DRAWHANDLERDESC_Call().  For example, a custom draw 
   handler that wishes to superimpose a logo over the top of another object (sort of 
   like station branding on cable TV broadcasts), might choose to call 
   DRAWHANDLERDESC_Call() first to allow the default drawing to take place, and then 
   perform it's own custom rendering.  
   
Supported Events:
   The draw handler passes all received events to its child widget.

See Also:
   IQueryInterface


=======================================================================

IDRAWHANDLER_SetDraw()

Description:
   This function allows an application to hook its own custom draw handler into the 
   drawing chain for a widget.  The customized draw handler will then be called instead 
   of the normal default drawing procedure allowing the application to perform whatever 
   rendering magic is appropriate.  For example, an application could choose to install 
   its own custom draw procedure that renders all of its child widgets into an offscreen 
   bitmap, then transformed that bitmap and finally blitted the image onto the display 
   to create the effect of looking at the display from the inside out (neato!).  
   
   The custom draw handler is stored in a DrawHandlerDesc data structure as a
   PFNDRAWHANDLER, along with a pointer to a block of memory allocate and owned by the
   application.  This pointer will be passed into the draw handler each time the draw
   handler is asked to draw itself, providing access to private application storage.
   The DrawHandlerDesc data structure also contains a PFNFREEHANDLER that will be
   called when the widget is released so that the widget may free the private
   memory allocated and passed to the event handler.
   
Prototype:
   void IDRAWHANDLER_SetDraw(IDrawHandler *p, DrawHandlerDesc *pd);

Parameters:
===pre>   
   p:   A pointer to an IDrawHandler interface.
   pd:  Pointer to the data structure that contains the custom draw handler,
        private memory pointer, and the callback used to free the private memory.
===/pre>   
   
Return Value:
   None

Comments:
   Passing NULL for the DrawHandlerDesc will reset the draw handler to use the default 
   draw handler.  
   
   Unless the draw handler is to be responsible for drawing all of the widget, the 
   custom draw handler installed should itself remember to call DRAWHANDLERDESC_Call() 
   so that the drawing chain will continue.  The draw handler may call 
   DRAWHANDLERDESC_Call() at any point -- either before it performs its own drawing, 
   after, or someplace in the middle.  
   
   An application that hooks into both a widget's draw handler and event handler (by 
   calling IHANDLER_SetHandler()), should exercise care in managing the private data 
   released by the PFNFREEHANDLER stored in the DrawHandlerDesc data structure.  Draw 
   handlers and event handlers both manage data structures that contain pointers to 
   private storage, as well as a pointer to a function that will be used to release this 
   storage -- which could be different...  or could be the same!  Each of these 
   PFNFREEHANDLER routines will be called when the widget is released.  
   Therefore, an application that uses the same block of private storage for both its 
   custom event and draw handling procedures should be careful to check the validity of 
   the storage to be freed before actually attempting to release the memory.  
   
Side Effects: 
   None
   
See Also:
   IDrawHandler Interface
   PFNHANDLER
   PFNFREEHANDLER
   DrawHandlerDesc
   DRAWHANDLERDESC_Call()
   IDRAWHANDLER_SetHandler()
   
===========================================================================

*/


#endif /* __AEEDRAWHANDLER_H__ */
