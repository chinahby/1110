/*
  ========================================================================

  FILE: AEEMediaFrameModel.h
  
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
#ifndef __AEEMEDIAFRAMEMODEL_H__
#define __AEEMEDIAFRAMEMODEL_H__

#include "AEEFrameModel.h"
#include "AEEMedia.h"
#include "AEECLSID_MEDIAFRAMEMODEL.BID"
#include "AEEIID_MEDIAFRAMEMODEL.BID"

/////////////////////////////////////////////////////////////////////////////
// IMediaFrameModel interface


#define INHERIT_IMediaFrameModel(iname) \
   INHERIT_IFrameModel(iname); \
   int   (*SetIMedia)      (iname *po, IMedia *piMedia); \
   void  (*GetIMedia)      (iname *po, IMedia **ppo); \
   void  (*RegisterNotify) (iname *po, PFNMEDIANOTIFY pfnNotify, void * pUser)


AEEINTERFACE_DEFINE(IMediaFrameModel);


#define IMEDIAFRAMEMODEL_AddRef(p)                 AEEGETPVTBL((p),IMediaFrameModel)->AddRef(p)
#define IMEDIAFRAMEMODEL_Release(p)                AEEGETPVTBL((p),IMediaFrameModel)->Release(p)
#define IMEDIAFRAMEMODEL_QueryInterface(p,i,o)     AEEGETPVTBL((p),IMediaFrameModel)->QueryInterface(p,i,o)
#define IMEDIAFRAMEMODEL_AddListener(p,pl)         AEEGETPVTBL((p),IMediaFrameModel)->AddListener(p,pl)
#define IMEDIAFRAMEMODEL_Notify(p,pe)              AEEGETPVTBL((p),IMediaFrameModel)->Notify(p,pe)
#define IMEDIAFRAMEMODEL_GetFrame(p,ppo)           AEEGETPVTBL((p),IMediaFrameModel)->GetFrame(p,ppo)
#define IMEDIAFRAMEMODEL_GetSize(p,x,y)            AEEGETPVTBL((p),IMediaFrameModel)->GetSize(p,x,y)
#define IMEDIAFRAMEMODEL_SetSize(p,x,y)            AEEGETPVTBL((p),IMediaFrameModel)->SetSize(p,x,y)
#define IMEDIAFRAMEMODEL_GetOffset(p,x,y)          AEEGETPVTBL((p),IMediaFrameModel)->GetOffset(p,x,y)
#define IMEDIAFRAMEMODEL_SetOffset(p,x,y)          AEEGETPVTBL((p),IMediaFrameModel)->SetOffset(p,x,y)
#define IMEDIAFRAMEMODEL_SetIMedia(p,pm)           AEEGETPVTBL((p),IMediaFrameModel)->SetIMedia(p,pm)
#define IMEDIAFRAMEMODEL_GetIMedia(p,pp)           AEEGETPVTBL((p),IMediaFrameModel)->GetIMedia(p,pp)
#define IMEDIAFRAMEMODEL_RegisterNotify(p,pfn,pu)  AEEGETPVTBL((p),IMediaFrameModel)->RegisterNotify(p,pfn,pu)


/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================
Media Frame Model Interface ID

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the model framework to indicate the model that a given object
   will use to describe the data associated with that object.  

   The model interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as the base interface for all model interfaces.

   The media frame model recognizes the following interface ID.

Definition:
===pre>
Identifier                Definition
----------                -------------------------------------------
AEEIID_MEDIAFRAMEMODEL:   Identifies the media frame model interface
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

Interface Name:  IMediaFrameModel

Description: 
   
   IMediaFrameModel is derived from IFrameModel and provides image frames from
   a video.  The video is managed by an IMedia object and wrapped
   by the media frame model.  Frame changes are sent to listeners, usually
   frame widgets.

   To create a media frame model, an application would do the following:
===pre>
      1.	Create an instance of the FrameWidget using AEECLSID_FRAMEWIDGET.
      2.	Create an instance of the IMediaUtil object using AEECLSID_MEDIAUTIL.
      3.	Create an instance of the IMedia interface and load the video file using IMEDIAUTIL_CreateMedia.
      4.	Create an instance of the MediaFrameModel using AEECLSID_MEDIAFRAMEMODEL.
      5.	Set the IMedia instance on the MediaFrameModel using IMEDIAFRAMEMODEL_SetIMedia.
      6.	Set the model on the frame widget using IWIDGET_SetModel.
      7. Start video using IMEDIA_Play.
===/pre>

The media frame model generates the following events:
===pre>
Event                        Description
-----                        ---------------------------------------------------
EVT_FRAMEMDL_FRAME:          The current frame has changed.
EVT_FRAMEMDL_MEDIACHANGED:   The IMedia object has changed. 
===/pre> 

Comments:
   The IMedia object must load the video file before calling IMEDIAFRAMEMODEL_SetIMedia.
   Also the video file must reside in a folder called "media" located in the app directory.
   
See Also:
   IMedia Interface
   IModel Interface
   IFrameModel Interface
   IFrameWidget Interface

==============================================================================

IMEDIAFRAMEMODEL_AddRef()

Description:

   This function increments the reference count of the IMediaFrameModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IMEDIAFRAMEMODEL_AddRef(IMediaFrameModel *p);

Parameters:
===pre>
   p:  Pointer to the IMediaFrameModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IMediaFrameModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IMEDIAFRAMEMODEL_Release()
   
=======================================================================

IMEDIAFRAMEMODEL_Release()

Description:

   This function decrements the reference count of the IMediaFrameModel interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IMEDIAFRAMEMODEL_Release(IMediaFrameModel *p);

Parameters:
===pre>
   p:  Pointer to the IMediaFrameModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IMediaFrameModel interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IMEDIAFRAMEMODEL_AddRef()

=======================================================================

IMEDIAFRAMEMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IMEDIAFRAMEMODEL_QueryInterface(IMediaFrameModel *p, AEECLSID c, void **d);

Parameters:
===pre>
   p:  Pointer to the IMediaFrameModel interface object.
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

IMEDIAFRAMEMODEL_GetFrame()

Description:
   This function returns the current frame as an IBitmap* ready for
   rendering to the display.

Prototype:
   IBitmap *IMEDIAFRAMEMODEL_GetFrame(IMediaFrameModel *p, IBitmap **ppib);

Parameters:
===pre>   
   p:          a pointer to the IMediaFrameModel object
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

IMEDIAFRAMEMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   text model.  The listener includes a callback function that BREW will call whenever the model has
   changed state, thereby notifying the owner of the listener that data has changed.

Prototype:
   int IMEDIAFRAMEMODEL_AddListener(IMediaFrameModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the IMediaFrameModel interface object.
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

IMEDIAFRAMEMODEL_GetSize()

Description:
   This function retrieves the size of the frame.  This size may not be the
   same as the widget extent used to render the frame.  The results of this
   function is also dependent on the underlying media object represented by the
   model.

Prototype:
   void IMEDIAFRAMEMODEL_GetSize(IMediaFrameModel *p, int *pcx, int *pcy); 

Parameters:
===pre>
   p:    Pointer to the IMediaFrameModel interface object.
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
   IMEDIAFRAMEMODEL_SetSize

===========================================================================

IMEDIAFRAMEMODEL_SetSize()

Description:
   This function sets the size of the frame.  This size may not be the
   same as the widget extent used to render the frame.  The effect of this
   function is also dependent on the underlying media object represented by the
   model.

Prototype:
   void IMEDIAFRAMEMODEL_SetSize(IMediaFrameModel *p, int cx, int cy); 

Parameters:
===pre>
   p:    Pointer to the IMediaFrameModel interface object.
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
   IMEDIAFRAMEMODEL_GetSize

===========================================================================

IMEDIAFRAMEMODEL_GetOffset()

Description:
   This function retrieves the offset of the frame.  The results of this
   function is dependent on the underlying media object represented by the
   model.

Prototype:
   void IMEDIAFRAMEMODEL_GetOffset(IMediaFrameModel *p, int *px, int *py); 

Parameters:
===pre>
   p:    Pointer to the IMediaFrameModel interface object.
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
   IMEDIAFRAMEMODEL_SetOffset

===========================================================================

IMEDIAFRAMEMODEL_SetOffset()

Description:
   This function sets the offset of the frame.  The effect of this
   function is dependent on the underlying media object represented by the
   model.

Prototype:
   void IMEDIAFRAMEMODEL_SetOffset(IMediaFrameModel *p, int x, int y); 

Parameters:
===pre>
   p:    Pointer to the IMediaFrameModel interface object.
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
   IMEDIAFRAMEMODEL_GetOffset

===========================================================================

IMEDIAFRAMEMODEL_SetIMedia()

Description:
   This function sets the IMedia object associated with this model.

Prototype:
   int IMEDIAFRAMEMODEL_SetIMedia(IMediaFrameModel *p, IMedia *piMedia); 

Parameters:
===pre>
   p:          Pointer to the IMediaFrameModel interface object.
   piMedia:    Pointer to IMedia object.
===/pre>
   
Return Value:
   SUCCESS or IMedia error code (See AEEMedia.h).

Comments:  
   None

Side Effects: 
   None

See Also:
   IMEDIAFRAMEMODEL_GetIMedia
   IMedia Interface

===========================================================================

IMEDIAFRAMEMODEL_GetIMedia()

Description:
   This function retrieves the IMedia object associated with this model.

Prototype:
   void IMEDIAFRAMEMODEL_GetIMedia(IMediaFrameModel *p, IMedia **ppo); 

Parameters:
===pre>
   p:          Pointer to the IMediaFrameModel interface object.
   ppo:        Set to the IMedia object upon return.
===/pre>
   
Return Value:
   None

Comments:  
   This function ADDREF's the IMedia object.  The caller is responsible
   for releasing the object.

Side Effects: 
   None

See Also:
   IMEDIAFRAMEMODEL_SetIMedia
   IMedia Interface

===========================================================================

IMEDIAFRAMEMODEL_RegisterNotify()

Description:
   This function registers a callback function for notification events
   generated by the IMedia object associated with this model.  See
   AEEMedia.h for information on the events that are sent.

Prototype:
   void IMEDIAFRAMEMODEL_RegisterNotify(IMediaFrameModel *p, PFNMEDIANOTIFY pfnNotify, void * pUser); 

Parameters:
===pre>
   p:             Pointer to the IMediaFrameModel interface object.
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
   PFNMEDIANOTIFY

===========================================================================
*/



#endif //__AEEMEDIAFRAMEMODEL_H__

