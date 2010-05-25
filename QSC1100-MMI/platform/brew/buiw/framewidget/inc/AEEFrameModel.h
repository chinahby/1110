/*
  ========================================================================

  FILE: AEEFrameModel.h
  
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
#ifndef __AEEFRAMEMODEL_H__
#define __AEEFRAMEMODEL_H__

#include "AEEWModel.h"
#include "AEEWidget.h"
#include "AEEIID_FRAMEMODEL.BID"

/////////////////////////////////////////////////////////////////////////////
// Events from models

#define EVT_FRAMEMDL_BASE           (EVT_MDL_WIDGET_START+0x200)
#define EVT_FRAMEMDL_FRAME          (EVT_FRAMEMDL_BASE)
#define EVT_FRAMEMDL_MEDIACHANGED   (EVT_FRAMEMDL_BASE+0x1)
#define EVT_FRAMEMDL_DONE           (EVT_FRAMEMDL_BASE+0x2)

/////////////////////////////////////////////////////////////////////////////
// IFrameModel interface


#define INHERIT_IFrameModel(iname) \
   INHERIT_IModel(iname); \
   int   (*GetFrame)    (iname *po, IBitmap **ppo); \
   void  (*SetSize)     (iname *po, int cx, int cy); \
   void  (*GetSize)     (iname *po, int *pcx, int *pcy); \
   void  (*SetOffset)   (iname *po, int x, int y); \
   void  (*GetOffset)   (iname *po, int *px, int *py)


AEEINTERFACE_DEFINE(IFrameModel);


#define IFRAMEMODEL_AddRef(p)                   AEEGETPVTBL((p),IFrameModel)->AddRef(p)
#define IFRAMEMODEL_Release(p)                  AEEGETPVTBL((p),IFrameModel)->Release(p)
#define IFRAMEMODEL_QueryInterface(p,i,o)       AEEGETPVTBL((p),IFrameModel)->QueryInterface(p,i,o)
#define IFRAMEMODEL_AddListener(p,pl)           AEEGETPVTBL((p),IFrameModel)->AddListener(p,pl)
#define IFRAMEMODEL_Notify(p,pe)                AEEGETPVTBL((p),IFrameModel)->Notify(p,pe)
#define IFRAMEMODEL_GetFrame(p,ppo)             AEEGETPVTBL((p),IFrameModel)->GetFrame(p,ppo)
#define IFRAMEMODEL_GetSize(p,x,y)              AEEGETPVTBL((p),IFrameModel)->GetSize(p,x,y)
#define IFRAMEMODEL_SetSize(p,x,y)              AEEGETPVTBL((p),IFrameModel)->SetSize(p,x,y)
#define IFRAMEMODEL_GetOffset(p,x,y)            AEEGETPVTBL((p),IFrameModel)->GetOffset(p,x,y)
#define IFRAMEMODEL_SetOffset(p,x,y)            AEEGETPVTBL((p),IFrameModel)->SetOffset(p,x,y)



/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================


===================================================================================
   INTERFACE DOCUMENTATION
===================================================================================

Interface Name: IFrameModel

Description: 

   IFrameModel is an abstract base class representing a media source such as
   video or a camera.  The IFrameModel notifies it's listener through IMODEL_Notify 
   whenever a new frame is available, and the listener can then retrieve the 
   current frame and display it. A frame widget is typically used with the
   frame model.

==============================================================================

IFRAMEMODEL_AddRef()

Description:

   This function increments the reference count of the IFrameModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IFRAMEMODEL_AddRef(IFrameModel *p);

Parameters:
===pre>
   p:  Pointer to the IFrameModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IFrameModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IFRAMEMODEL_Release()
   
=======================================================================

IFRAMEMODEL_Release()

Description:

   This function decrements the reference count of the IFrameModel interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IFRAMEMODEL_Release(IFrameModel *p);

Parameters:
===pre>
   p:  Pointer to the IFrameModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IFrameModel interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IFRAMEMODEL_AddRef()

=======================================================================

IFRAMEMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IFRAMEMODEL_QueryInterface(IFrameModel *p, AEECLSID c, void **d);

Parameters:
===pre>
   p:  Pointer to the IFrameModel interface object.
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

IFRAMEMODEL_GetFrame()

Description:
   This function returns the current frame as an IBitmap* ready for
   rendering to the display.

Prototype:
   int IBitmap *IFRAMEMODEL_GetFrame(IFrameModel *p, IBitmap **ppib);

Parameters:
===pre>   
   p:          a pointer to the IFrameModel object
   ppib:       memory location to fill with addref'ed bitmap pointer 
===/pre>   
   
Return Value:
===pre>
   SUCCESS     if successful
   EBADSTATE   if frame bitmap is not available
===/pre>

Comments:  
   None

Side Effects: 
   None
   
See Also:
   None

===========================================================================

IFRAMEMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   text model.  The listener includes a callback function that BREW will call whenever the model has
   changed state, thereby notifying the owner of the listener that data has changed.

Prototype:
   int IFRAMEMODEL_AddListener(IFrameModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the IFrameModel interface object.
   pl:  Pointer to the ModelListener that will be attached to the object.  The ModelListener will
        contain the PFNLISTENER callback function BREW will call to notify the registering object
        of changes to the value model.
===/pre>
   
Return Value:
===pre>
   SUCCESS          - Listener successfully added
   EBADPARM         - Invalid argument
   ENOMEMORY        - Insufficient memory
===/pre>

Comments:  
   None

Side Effects: 
   None

See Also:
   ModelListener
   PFNLISTENER

===========================================================================

IFRAMEMODEL_GetSize()

Description:
   This function retrieves the size of the frame.  This size may not be the
   same as the widget extent used to render the frame.  The results of this
   function is also dependent on the underlying media object represented by the
   model.

Prototype:
   void IFRAMEMODEL_GetSize(IFrameModel *p, int *pcx, int *pcy); 

Parameters:
===pre>
   p:    Pointer to the IFrameModel interface object.
   pcx:  Pointer to an integer to receive the width of the frame.
   pcy:  Pointer to an integer to receive the height of the frame.
===/pre>
   
Return Value:
   None

Comments:  
   None

Side Effects: 
   None

See Also:
   IFRAMEMODEL_SetSize

===========================================================================

IFRAMEMODEL_SetSize()

Description:
   This function sets the size of the frame.  This size may not be the
   same as the widget extent used to render the frame.  The effect of this
   function is also dependent on the underlying media object represented by the
   model.

Prototype:
   void IFRAMEMODEL_SetSize(IFrameModel *p, int cx, int cy); 

Parameters:
===pre>
   p:    Pointer to the IFrameModel interface object.
   cx:   Width of the frame.
   cy:   Height of the frame.
===/pre>
   
Return Value:
   None

Comments:  
   None

Side Effects: 
   None

See Also:
   IFRAMEMODEL_GetSize

===========================================================================

IFRAMEMODEL_GetOffset()

Description:
   This function retrieves the offset of the frame.  The results of this
   function is dependent on the underlying media object represented by the
   model.

Prototype:
   void IFRAMEMODEL_GetOffset(IFrameModel *p, int *px, int *py); 

Parameters:
===pre>
   p:    Pointer to the IFrameModel interface object.
   px:   Pointer to an integer to receive the horizontal offset of the frame.
   py:   Pointer to an integer to receive the vertical offset of the frame.
===/pre>
   
Return Value:
   None

Comments:  
   None

Side Effects: 
   None

See Also:
   IFRAMEMODEL_SetOffset

===========================================================================

IFRAMEMODEL_SetOffset()

Description:
   This function sets the offset of the frame.  The effect of this
   function is dependent on the underlying media object represented by the
   model.

Prototype:
   void IFRAMEMODEL_SetOffset(IFrameModel *p, int x, int y); 

Parameters:
===pre>
   p:    Pointer to the IFrameModel interface object.
   x:    Horizontal offset of the frame.
   y:    Vertical offset of the frame.
===/pre>
   
Return Value:
   None

Comments:  
   None

Side Effects: 
   None

See Also:
   IFRAMEMODEL_GetOffset

===========================================================================
*/


#endif //__AEEFRAMEMODEL_H__

