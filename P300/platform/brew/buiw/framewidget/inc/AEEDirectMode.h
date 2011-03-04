/*
  ========================================================================

  FILE: AEEDirectMode.h
  
  SERVICES: 
      API to Set/Get DirectMode. This interface should be implemented by
      FrameModel to support direct mode. This interface is used by 
      FrameWidget.

  GENERAL DESCRIPTION:  
      DirectMode is used to allow certain interfaces to draw directly
      to the screen and bypass the widgets framework. This is useful
      for cases where certain software/hardware needs to draw by itself
      and not on demand (as is the case with IWIDGET_Draw).
      
   
  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEEDIRECTMODE_H__
#define __AEEDIRECTMODE_H__


#include "AEE.h"

//Interface Id for IDirectMode interface
#define AEEIID_DIRECTMODE   	0x0103fa30

//Direct Mode flags 
#define DMF_DISABLE               0       // Default - do not use direct mode
#define DMF_ENABLE            (1<<0)      // Enable direct mode.


/////////////////////////////////////////////////////////////////////////////
// IDirectMode interface

#define INHERIT_IDirectMode(iname) \
   INHERIT_IQueryInterface(iname); \
   int (*SetDirectMode)   (iname *po, int nDirect, AEERect *prc);\
   int (*GetDirectMode)   (iname *po, int *pnDirect, AEERect *prc)

AEEINTERFACE_DEFINE(IDirectMode);

static __inline int IDIRECTMODE_SetDirectMode(IDirectMode *p,int nDirect, AEERect *prc) {
   return AEEGETPVTBL((p),IDirectMode)->SetDirectMode(p,nDirect,prc);
}

static __inline int IDIRECTMODE_GetDirectMode(IDirectMode *p,int* pnDirect, AEERect *prc) {
   return AEEGETPVTBL((p),IDirectMode)->GetDirectMode(p,pnDirect,prc);
}

/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================
 DirectMode Interface ID

Description:
   Each interface is identified by a unique interface ID.  
   The DirectMode inteface recognizes the following interface ID.

Definition:
===pre>
Identifier                Definition
----------                -------------------------------------------
AEEIID_DIRECTMODE:        Identifies the IDirectMode interface
===/pre>
   
Comments:
   None

See Also:
   IFrameModel Interface

===================================================================================
   INTERFACE DOCUMENTATION
===================================================================================

Interface Name:  IDirectMode

Description: 
      IDirectMode is derived from IQueryInterface. It is expected to be implemented
      by FrameModel which supports DirectMode. Whenever the application tries to set 
      or get direct mode property (PROPEX_DIRECT) on the FrameWidget, FrameWidget 
      communicates to FrameModel, aboutthis property, using IDirectMode. 
      FrameWidget queries for this interface ( AEEIID_DIRECTMODE) and if it is 
      sucessful, it uses the DirectMode API to set/get the directmode. 


==============================================================================

IDIRECTMODE_AddRef()

Description:

   This function increments the reference count of the IDirectMode interface object, 
   allowing the interface to be shared by multiple callers.  The object is freed when 
   the reference count reaches 0 (zero). 

Prototype:

   uint32 IDIRECTMODE_AddRef(IDirectMode *p);

Parameters:
===pre>
   p:  Pointer to the IDirectMode interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IDirectMode interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IDIRECTMODE_Release()
   
=======================================================================

IDIRECTMODE_Release()

Description:

   This function decrements the reference count of the IDirectMode interface object.  
   The object is freed from memory and is no longer valid once its reference count 
   reaches 0 (zero).

Prototype:

   uint32 IDIRECTMODE_Release(IDirectMode *p);

Parameters:
===pre>
   p:  Pointer to the IDierctMode interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IDirectMode interface object.  
   If the returned reference count is 0 (zero), the object has been freed and will 
   no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IDIRECTMODE_AddRef()

=======================================================================

IDIRECTMODE_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IDIRECTMODE_QueryInterface(IDirectMode *p, AEECLSID c, void **d);

Parameters:
===pre>
   p:  Pointer to the IDirectMode interface object.
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
   
==========================================================================

IDIRECTMODE_SetDirectMode()

Description:
   This function sets the direct mode for the underlying object it is using.


Prototype:
   int *IDIRECTMODE_SetDirectMode(IDirectMode *p, int nDirect, AEERect *prc);

Parameters:
===pre>   
   p:          a pointer to the IDirectMode object
   nDirect:    Flags used for direct mode. Supported flags are
                DMF_DISABLE : Disable direct mode.
                DMF_ENABLE  : Enable direct mode.
   prc:        Pointer to AEERect. This specifies the rectangle to be used
               for drawing directly to the screen. IDirectMode implementation
               will pass this rectangle to the underlying interface (like IMedia)


===/pre>   
   
Return Value:
===pre>
   SUCCESS     if successful
   EFAILED     if unable to set the requested mode on underlying interface.
===/pre>

Comments:  
   None

Side Effects: 
   If the direct mode is enabled, then the drawing on the specified rectangle on
   the screen is no longer controlled by the widget's framework. If there are
   any other layout related changes, which result in external invalidation of that
   rectangle, the application should disable direct mode for these changes to work
   correctly.

   
See Also:
   IDIRECTMODE_GetDirectMode

===========================================================================

IDIRECTMODE_GetDirectMode()

Description:
   

Prototype:
   int *IDIRECTMODE_GetDirectMode(IDirectMode *p, int* pnDirect, AEERect *prc);

Parameters:
===pre>
   p:          a pointer to the IDirectMode object
   pnDirect:   pointer to an integer. If this pointer is not NULL, then the 
               current DirectMode flags will be returned.
   prc:        pointer to AEERect where the rectangle used for directmode drawing
               will be returned if this pointer is not NULL. This parameter has no 
               meaning if the direct mode is disabled.

===/pre>
   
Return Value:
===pre>
 SUCCESS : If able to get the required parameters for direct mode
 EFAILED:  Otherwise.

===/pre>

Comments:  
   None

Side Effects: 
   None

See Also:
   IDIRECTMODE_SetDirectMode

===========================================================================
*/

#endif // __AEEDIRECTMODE_H__