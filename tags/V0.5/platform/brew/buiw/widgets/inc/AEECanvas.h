/*
  ========================================================================

  FILE:  AEECanvas.h
  
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
#ifndef __AEECANVAS_H__
#define __AEECANVAS_H__


#include "AEE.h"
#include "AEEBitmap.h"


#include "bid/AEEIID_CANVAS.bid"
#include "bid/AEECLSID_CANVAS.bid"

/////////////////////////////////////////////////////////////////////////////
// ICanvas interface


typedef struct ICanvas ICanvas;

#define INHERIT_ICanvas(iname) \
   INHERIT_IQueryInterface(iname);\
   int   (*GetBitmap)      (iname *, IBitmap **ppo);\
   int   (*SetBitmap)      (iname *, IBitmap *pib);\
   int   (*GetClipRect)    (iname *, AEERect *prc);\
   int   (*SetClipRect)    (iname *, const AEERect *prc)


AEEINTERFACE(ICanvas) {
   INHERIT_ICanvas(ICanvas);
};

#define ICANVAS_AddRef(p)                 AEEGETPVTBL(p,ICanvas)->AddRef(p)
#define ICANVAS_Release(p)                AEEGETPVTBL(p,ICanvas)->Release(p)
#define ICANVAS_QueryInterface(p,i,p2)    AEEGETPVTBL(p,ICanvas)->QueryInterface(p,i,p2)
#define ICANVAS_GetBitmap(p,ppo)          AEEGETPVTBL(p,ICanvas)->GetBitmap(p,ppo)
#define ICANVAS_SetBitmap(p,pib)          AEEGETPVTBL(p,ICanvas)->SetBitmap(p,pib)
#define ICANVAS_SetClipRect(p,prc)        AEEGETPVTBL(p,ICanvas)->SetClipRect(p,prc)
#define ICANVAS_GetClipRect(p,prc)        AEEGETPVTBL(p,ICanvas)->GetClipRect(p,prc)


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

Canvas Interface and Class IDs

Description:
   An ICanvas is both an interface and an object instance that provides 
   an ICanvas interface. Both are identified by unique 32-bit ID values.
   
Definition:

Identifier        Definition
----------        -----------------------------------
AEEIID_CANVAS:    Interface ID for the ICanvas interface
AEECLSID_CANVAS:  Class ID for an instance of an ICanvas object

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

ICanvas Interface

Description:
   The ICanvas interface provides a drawing surface for widgets and other components
   to draw upon. Typically, one ICanvas instance is created by the application or
   application framework and passed to the IRootContainer.  The IRootContainer in
   turn passes it to each widget at draw time.
   
   The following APIs are provided to support an object reference counting mechanism for
   objects to manage their own memory instances.

===pre>
       ICANVAS_AddRef()
       ICANVAS_Release()
===/pre>
      
   The following API is provided to support run-time interface polymorphism.
===pre>
       ICANVAS_QueryInterface
===/pre>

   The following APIs are provided to retrieve or set the destination bitmap for the
   draw operation. Drawing is achieved by using native IBitmap interface methods.
===pre>
       ICANVAS_GetBitmap()
       ICANVAS_SetBitmap()
===/pre>

   The following APIs are provided to retrieve or set the clipping rectangle.
===pre>
       ICANVAS_GetClipRect()
       ICANVAS_SetClipRect()
===/pre>

Required header files:
===pre>
   AEE.h
   AEEBitmap.h
===/pre>
 
===========================================================================

ICANVAS_AddRef()

Description:

   This function increments the reference count of the ICanvas interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 ICANVAS_AddRef(ICanvas *p);

Parameters:
===pre>
   p:  Pointer to the ICanvas interface object
===/pre>

Return Value:
   Returns the incremented reference count for the ICanvas interface object.

Comments:
   None

Side Effects:
   None

See Also:
   ICANVAS_Release()
   
=======================================================================

ICANVAS_Release()

Description:

   This function decrements the reference count of the ICanvas interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 ICANVAS_Release(ICanvas *p);

Parameters:
===pre>
   p:  Pointer to the ICanvas interface object
===/pre>

Return Value:
   Returns the decremented reference count for the ICanvas interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   ICANVAS_AddRef()

=======================================================================

ICANVAS_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int ICANVAS_QueryInterface(ICanvas *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the ICanvas interface object.
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

ICANVAS_GetBitmap()

Description:
   This function receives the destination bitmap from the ICanvas object.

Prototype:
   int ICANVAS_GetBitmap(ICanvas *p, IBitmap **ppo);

Parameters:
===pre>   
   p:    A pointer to an ICanvas object.
   ppo:  Memory location where the returned IBitmap* will be stored.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS - The function was able to successfully return a destination bitmap.
   EFAILED - The destination bitmap was not available.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    IBitmap


===========================================================================

ICANVAS_SetBitmap()

Description:
   This function assigns a destination bitmap to the ICanvas object.

Prototype:
   int ICANVAS_SetBitmap(ICanvas *p, IBitmap *piBitmap);

Parameters:
===pre>   
   p:    A pointer to an ICanvas object.
   ppo:  A pointer to an IBitmap interface object, which the ICanvas
         object will use as its destination bitmap.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS - The function was able to successfully set a new destination bitmap.
   EFAILED - The destination bitmap was not successfully set.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    IBitmap



===========================================================================

ICANVAS_GetClipRect()

Description:
   This function retrieves the current clipping rectangle from the ICanvas object.

Prototype:
   int ICANVAS_GetClipRect(ICanvas *p, AEERect *prcClip);

Parameters:
===pre>   
   p:    A pointer to an ICanvas object.
   ppo:  A pointer to an AEERect data structure that will be filled
         with the current clipping rectangle.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS - The function successfully returned the clipping rectangle
   EFAILED - The clipping rectangle was not retrieved.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    

===========================================================================

ICANVAS_SetClipRect()

Description:
   This function sets a new clipping rectangle into the ICanvas object.

Prototype:
   int ICANVAS_SetClipRect(ICanvas *p, AEERect *prcClip);

Parameters:
===pre>   
   p:    A pointer to an ICanvas object.
   ppo:  A pointer to an AEERect data structure that is initialized
         to the value of the new clipping rectangle
===/pre>   
   
Return Value:
===pre>   
   SUCCESS - The function successfully set the clipping rectangle
   EFAILED - The clipping rectangle was not set.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
   None
   
*/

#endif   //__AEECANVAS_H__

