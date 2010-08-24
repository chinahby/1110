/*
  ========================================================================

  FILE:  AEEDisplayCanvas.h
  
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
#ifndef __AEEDISPLAYCANVAS_H__
#define __AEEDISPLAYCANVAS_H__

#include "AEECanvas.h"
#include "AEEDisp.h"


/////////////////////////////////////////////////////////////////////////////
// IDisplayCanvas interface


#include "bid/AEEIID_DISPLAYCANVAS.bid"
#include "bid/AEECLSID_DISPLAYCANVAS.bid"


typedef struct IDisplayCanvas IDisplayCanvas;

#define INHERIT_IDisplayCanvas(iname) \
   INHERIT_ICanvas(iname);\
   int   (*GetDisplay)    (iname *, IDisplay **ppo);\
   int   (*SetDisplay)    (iname *, IDisplay *piDisplay)
   

AEEINTERFACE(IDisplayCanvas) {
   INHERIT_IDisplayCanvas(IDisplayCanvas);
};


#define IDISPLAYCANVAS_AddRef(p)                 AEEGETPVTBL(p,IDisplayCanvas)->AddRef(p)
#define IDISPLAYCANVAS_Release(p)                AEEGETPVTBL(p,IDisplayCanvas)->Release(p)
#define IDISPLAYCANVAS_QueryInterface(p,i,p2)    AEEGETPVTBL(p,IDisplayCanvas)->QueryInterface(p,i,p2)
#define IDISPLAYCANVAS_GetBitmap(p,ppo)          AEEGETPVTBL(p,IDisplayCanvas)->GetBitmap(p,ppo)
#define IDISPLAYCANVAS_SetBitmap(p,pib)          AEEGETPVTBL(p,IDisplayCanvas)->SetBitmap(p,pib)
#define IDISPLAYCANVAS_SetClipRect(p,prc)        AEEGETPVTBL(p,IDisplayCanvas)->SetClipRect(p,prc)
#define IDISPLAYCANVAS_GetClipRect(p,prc)        AEEGETPVTBL(p,IDisplayCanvas)->GetClipRect(p,prc)
#define IDISPLAYCANVAS_GetDisplay(p,ppo)         AEEGETPVTBL(p,IDisplayCanvas)->GetDisplay(p,ppo)
#define IDISPLAYCANVAS_SetDisplay(p,pid)         AEEGETPVTBL(p,IDisplayCanvas)->SetDisplay(p,pid)



static __inline int ICANVAS_GetDisplay(ICanvas *piCanvas, IDisplay **ppo) {
   IDisplayCanvas *pidc = 0;
   int nErr = ICANVAS_QueryInterface(piCanvas, AEEIID_DISPLAYCANVAS, (void**)&pidc);
   if (!nErr) {
      nErr = IDISPLAYCANVAS_GetDisplay(pidc, ppo);
   }
   if (pidc) {
      IDISPLAYCANVAS_Release(pidc);
   }
   return nErr;
}


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Display Canvas Interface and Class IDs

Description:
   An IDisplayCanvas is both an interface and an object instance that 
   provides an IDisplayCanvas interface. Both are identified by unique 
   32-bit ID values.

Definition:

Identifier              Definition
----------              -----------------------------------
AEEIID_DISPLAYCANVAS    Interface ID for the ICanvas interface         
AEECLSID_DISPLAYCANVAS  Class ID for an instance of an ICanvas object  


===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IDisplayCanvas Interface

Description:
   The IDisplayCanvas interface provides an ICanvas interface for widgets and 
   other components to draw upon. It has additional methods to accept and adapt 
   an IDisplay interface to function as an ICanvas, and to obtain an IDisplay
   from an IDisplayCanvas.
   
   The following APIs are provided to support an object reference counting mechanism for
   objects to manage their own memory instances.
===pre>
       IDISPLAYCANVAS_AddRef()
       IDISPLAYCANVAS_Release()
===/pre>
      
   The following API is provided to support run-time interface polymorphism.
===pre>
       IDISPLAYCANVAS_QueryInterface

===/pre>

   The following APIs are provided to retrieve or set the destination bitmap for the
   draw operation. Drawing is achieved by using native IBitmap interface methods.
===pre>
       IDISPLAYCANVAS_GetBitmap()
       IDISPLAYCANVAS_SetBitmap()
===/pre>

   The following APIs are provided to retrieve or set the clipping rectangle.
===pre>
       IDISPLAYCANVAS_GetClipRect()
       IDISPLAYCANVAS_SetClipRect()
===/pre>


   The following APIs are provided to retrieve or set the IDisplay interface.
===pre>
      IDISPLAYCANVAS_GetDisplay()
      IDISPLAYCANVAS_SetDisplay()
===/pre>


Required header files:
===pre>
   AEE.h
   AEEBitmap.h
===/pre>


===========================================================================

IDISPLAYCANVAS_GetDisplay()

Description:
   This function receives the wrapped IDisplay interface from the 
   IDisplayCanvas object.

Prototype:
   int IDISPLAYCANVAS_GetDisplay(IDisplayCanvas *p, IDisplay **ppo);

Parameters:
===pre>   
   p:    A pointer to an ICanvas object.
   ppo:  Memory location where the returned IDisplay pointer will 
         be stored.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS - The function was able to successfully return an IDisplay pointer.
   EFAILED - The IDisplay pointer was not available.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    IDisplay Interface


===========================================================================

IDISPLAYCANVAS_SetDisplay()

Description:
   This function assigns an IDisplay object to the IDisplayCanvas object.

Prototype:
   int IDISPLAYCANVAS_SetDisplay(IDisplayCanvas *p, IDisplay *piDisplay);

Parameters:
===pre>   
   p:    A pointer to an IDisplayCanvas object.
   ppo:  A pointer to an IDisplay object, which the IDisplayCanvas object 
         will use as its destination bitmap and clipping rectangle.
===/pre>   
   
Return Value:
===pre>   
   SUCCESS - The function was able to successfully set the IDispay object.
   EFAILED - The IDisplay object was not successfully set.
===/pre>   

Comments:
   None

Side Effects: 
   None
   
See Also:
    IDisplay Interface


===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

Interface Reference:  ICanvas

===========================================================================

ICANVAS_GetDisplay()

Description:
   This is an inline function that gets an IDisplay object from an ICanvas
   object, providing the ICanvas object also supports the IDisplayCanvas
   interface. This function first performs a QueryInterface for an 
   IDisplayCanvas interface, then if successful, defers to 
   IDISPLAYCANVAS_GetDisplay() to obtain an IDisplay.

Prototype:
   int ICANVAS_GetDisplay(ICanvas *piCanvas, IDisplay **ppo);
   
Parameters:
===pre>   
   p:   Pointer to an ICanvas object.
   pe:  Memory location where the returned IDisplay pointer will 
        be stored.
        
===/pre> 

Return Value:
===pre>   
   SUCCESS - The function was able to successfully return an IDisplay pointer.
   EFAILED - The IDisplay pointer was not available.
===/pre>   
   
Comments:
   None

Side Effects: 
   None

See Also:
   IDisplay Interface        


==========================================================================
*/



#endif   //__AEEDISPLAYCANVAS_H__

