/*
  ========================================================================

  FILE: AEECameraFrameModel.h
  
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
#ifndef __AEECAMERAFRAMEMODEL_H__
#define __AEECAMERAFRAMEMODEL_H__

#include "AEEFrameModel.h"
#include "AEECamera.h"
#include "AEECLSID_CAMERAFRAMEMODEL.BID"
#include "AEEIID_CAMERAFRAMEMODEL.BID"

/////////////////////////////////////////////////////////////////////////////
// ICameraFrameModel interface


#define INHERIT_ICameraFrameModel(iname) \
   INHERIT_IFrameModel(iname); \
   int   (*SetICamera)        (iname *po, ICamera *piCamera); \
   void  (*GetICamera)        (iname *po, ICamera **ppo); \
   void  (*RegisterNotify)    (iname *po, PFNCAMERANOTIFY pfnNotify, void * pUser)


AEEINTERFACE_DEFINE(ICameraFrameModel);


#define ICAMERAFRAMEMODEL_AddRef(p)                AEEGETPVTBL((p),ICameraFrameModel)->AddRef(p)
#define ICAMERAFRAMEMODEL_Release(p)               AEEGETPVTBL((p),ICameraFrameModel)->Release(p)
#define ICAMERAFRAMEMODEL_QueryInterface(p,i,o)    AEEGETPVTBL((p),ICameraFrameModel)->QueryInterface(p,i,o)
#define ICAMERAFRAMEMODEL_AddListener(p,pl)        AEEGETPVTBL((p),ICameraFrameModel)->AddListener(p,pl)
#define ICAMERAFRAMEMODEL_Notify(p,pe)             AEEGETPVTBL((p),ICameraFrameModel)->Notify(p,pe)
#define ICAMERAFRAMEMODEL_GetFrame(p,ppo)          AEEGETPVTBL((p),ICameraFrameModel)->GetFrame(p,ppo)
#define ICAMERAFRAMEMODEL_GetSize(p,x,y)           AEEGETPVTBL((p),ICameraFrameModel)->GetSize(p,x,y)
#define ICAMERAFRAMEMODEL_SetSize(p,x,y)           AEEGETPVTBL((p),ICameraFrameModel)->SetSize(p,x,y)
#define ICAMERAFRAMEMODEL_GetOffset(p,x,y)         AEEGETPVTBL((p),ICameraFrameModel)->GetOffset(p,x,y)
#define ICAMERAFRAMEMODEL_SetOffset(p,x,y)         AEEGETPVTBL((p),ICameraFrameModel)->SetOffset(p,x,y)
#define ICAMERAFRAMEMODEL_SetICamera(p,pm)         AEEGETPVTBL((p),ICameraFrameModel)->SetICamera(p,pm)
#define ICAMERAFRAMEMODEL_GetICamera(p,pp)         AEEGETPVTBL((p),ICameraFrameModel)->GetICamera(p,pp)
#define ICAMERAFRAMEMODEL_RegisterNotify(p,pfn,pu) AEEGETPVTBL((p),ICameraFrameModel)->RegisterNotify(p,pfn,pu)




/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================
Camera Frame Model Interface ID

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the model framework to indicate the model that a given object
   will use to describe the data associated with that object.  

   The model interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as the base interface for all model interfaces.

   The camera frame model recognizes the following interface ID.

Definition:
===pre>
Identifier                Definition
----------                -------------------------------------------
AEEIID_CAMERAFRAMEMODEL:  Identifies the camera frame model interface
===/pre>
   
Comments:
   None

See Also:
   IFrameModel Interface
   IWIDGET_GetModel()
   IWIDGET_SetModel()

===================================================================================
   INTERFACE DOCUMENTATION
===================================================================================

Interface Name:   ICameraFrameModel

Description: 
   
   ICameraFrameModel is derived from IFrameModel and provides image frames from
   a camera.  The camera image is managed by an ICamera object and wrapped
   by the camera frame model.  Frame changes are sent to listeners, usually
   frame widgets.

   To create a camera frame model, an application would do the following:
===pre>
      1.	Create an instance of the frame widget using AEECLSID_FRAMEWIDGET.
      2.	Create an instance of the ICamera object using AEECLSID_CAMERA.
      3.	Set the camera size and display size using ICAMERA_SetSize and ICAMERA_SetDisplaySize.
      4.	Create an instance of the CameraFrameModel using AEECLSID_CAMERAFRAMEMODEL.
      5.	Set the ICamera instance on the CameraFrameModel using ICAMERAFRAMEMODEL_SetICamera.
      6.	Set the model on the frame widget using IWIDGET_SetModel.
      7.	Start camera using ICAMERA_Preview.
===/pre>

The camera frame model generates the following events:
===pre>
Event                   Description
-----                   ------------------------------------------------------------
EVT_FRAMEMDL_FRAME:     The current frame has changed.         
===/pre> 

Comments:
   None
   
See Also:
   ICamera Interface
   IModel Interface
   IFrameModel Interface
   IFrameWidget Interface

==============================================================================

ICAMERAFRAMEMODEL_AddRef()

Description:

   This function increments the reference count of the ICameraFrameModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 ICAMERAFRAMEMODEL_AddRef(ICameraFrameModel *p);

Parameters:
===pre>
   p:  Pointer to the ICameraFrameModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the ICameraFrameModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   ICAMERAFRAMEMODEL_Release()
   
=======================================================================

ICAMERAFRAMEMODEL_Release()

Description:

   This function decrements the reference count of the ICameraFrameModel interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 ICAMERAFRAMEMODEL_Release(ICameraFrameModel *p);

Parameters:
===pre>
   p:  Pointer to the ICameraFrameModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the ICameraFrameModel interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   ICAMERAFRAMEMODEL_AddRef()

=======================================================================

ICAMERAFRAMEMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int ICAMERAFRAMEMODEL_QueryInterface(ICameraFrameModel *p, AEECLSID c, void **d);

Parameters:
===pre>
   p:  Pointer to the ICameraFrameModel interface object.
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

ICAMERAFRAMEMODEL_GetFrame()

Description:
   This function returns the current frame as an IBitmap* ready for
   rendering to the display.

Prototype:
   IBitmap *ICAMERAFRAMEMODEL_GetFrame(ICameraFrameModel *p, IBitmap **ppib);

Parameters:
===pre>   
   p:          a pointer to the ICameraFrameModel object
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

ICAMERAFRAMEMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   text model.  The listener includes a callback function that BREW will call whenever the model has
   changed state, thereby notifying the owner of the listener that data has changed.

Prototype:
   int ICAMERAFRAMEMODEL_AddListener(ICameraFrameModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the ICameraFrameModel interface object.
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

ICAMERAFRAMEMODEL_GetSize()

Description:
   This function retrieves the size of the frame.  This size may not be the
   same as the widget extent used to render the frame.  The results of this
   function is also dependent on the underlying media object represented by the
   model.

Prototype:
   void ICAMERAFRAMEMODEL_GetSize(ICameraFrameModel *p, int *pcx, int *pcy); 

Parameters:
===pre>
   p:    Pointer to the ICameraFrameModel interface object.
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
   ICAMERAFRAMEMODEL_SetSize

===========================================================================

ICAMERAFRAMEMODEL_SetSize()

Description:
   This function sets the size of the frame.  This size may not be the
   same as the widget extent used to render the frame.  The effect of this
   function is also dependent on the underlying media object represented by the
   model.

Prototype:
   void ICAMERAFRAMEMODEL_SetSize(ICameraFrameModel *p, int cx, int cy); 

Parameters:
===pre>
   p:    Pointer to the ICameraFrameModel interface object.
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
   ICAMERAFRAMEMODEL_GetSize

===========================================================================

ICAMERAFRAMEMODEL_GetOffset()

Description:
   This function retrieves the offset of the frame.  The results of this
   function is dependent on the underlying media object represented by the
   model.

Prototype:
   void ICAMERAFRAMEMODEL_GetOffset(ICameraFrameModel *p, int *px, int *py); 

Parameters:
===pre>
   p:    Pointer to the ICameraFrameModel interface object.
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
   ICAMERAFRAMEMODEL_SetOffset

===========================================================================

ICAMERAFRAMEMODEL_SetOffset()

Description:
   This function sets the offset of the frame.  The effect of this
   function is dependent on the underlying media object represented by the
   model.

Prototype:
   void ICAMERAFRAMEMODEL_SetOffset(ICameraFrameModel *p, int x, int y); 

Parameters:
===pre>
   p:    Pointer to the ICameraFrameModel interface object.
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
   ICAMERAFRAMEMODEL_GetOffset

===========================================================================

ICAMERAFRAMEMODEL_SetICamera()

Description:
   This function sets the ICamera object associated with this model.

Prototype:
   int ICAMERAFRAMEMODEL_SetICamera(ICameraFrameModel *p, ICamera *piCamera); 

Parameters:
===pre>
   p:          Pointer to the ICameraFrameModel interface object.
   piCamera:   Pointer to ICamera object.
===/pre>
   
Return Value:
   SUCCESS or ICamera error code (See AEEMedia.h).

Comments:  
   None

Side Effects: 
   None

See Also:
   ICAMERAFRAMEMODEL_GetICamera
   ICamera Interface

===========================================================================

ICAMERAFRAMEMODEL_GetICamera()

Description:
   This function retrieves the ICamera object associated with this model.

Prototype:
   void ICAMERAFRAMEMODEL_GetICamera(ICameraFrameModel *p, ICamera **ppo); 

Parameters:
===pre>
   p:          Pointer to the ICameraFrameModel interface object.
   ppo:        Set to the ICamera object upon return.
===/pre>
   
Return Value:
   None

Comments:  
   This function ADDREF's the ICamera object.  The caller is responsible
   for releasing the object.

Side Effects: 
   None

See Also:
   ICAMERAFRAMEMODEL_SetICamera
   ICamera Interface

===========================================================================

ICAMERAFRAMEMODEL_RegisterNotify()

Description:
   This function registers a callback function for notification events
   generated by the ICamera object associated with this model.  See
   AEECamera.h for information on the events that are sent.

Prototype:
   void ICAMERAFRAMEMODEL_RegisterNotify(ICameraFrameModel *p, PFNCAMERANOTIFY pfnNotify, void * pUser); 

Parameters:
===pre>
   p:             Pointer to the ICameraFrameModel interface object.
   pfnNotify:     Pointer to callback function.
   pUser:         Pointer to user defined data passed to the callback.
===/pre>
   
Return Value:
   None

Comments:  
   None

Side Effects: 
   None

See Also:
   PFNCAMERANOTIFY

===========================================================================
*/



#endif //__AEECAMERAFRAMEMODEL_H__

