/*
  ========================================================================

  FILE:  AEEModel.h
  
  SERVICES: Defines the basic model interface

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEEMODEL_H__
#define __AEEMODEL_H__

#include "AEE.h"

////////////////////////////////////////////////////////////////////////////
// Model Interface ID

#include "bid\AEEIID_MODEL.bid"

/////////////////////////////////////////////////////////////////////////////
// Events from models

#define EVT_MDL_GENERIC_CHANGE      0x0            // Generic change to IModel

// QCOM-Reserved Range
#define EVT_MDL_GENERIC_START       EVT_MDL_GENERIC_CHANGE
#define EVT_MDL_GENERIC_END         (EVT_MDL_GENERIC_START+ 0xFFF)    // 0x00000FFF

// BUIT Widget Range
#define EVT_MDL_WIDGET_START        0x00001000
#define EVT_MDL_WIDGET_END          (EVT_MDL_WIDGET_START + 0xFFF)    // 0x00001FFF

// BUIT Form Range
#define EVT_MDL_FORM_START          0x00002000
#define EVT_MDL_FORM_END            (EVT_MDL_FORM_START + 0xFFF)      // 0x00002FFF

// BREW Internal Usage Range
#define EVT_MDL_BREW_START          0x00010000 
#define EVT_MDL_BREW_END            (EVT_MDL_BREW_START + 0x000EFFFF) // 0x000FFFFF

// OEM-User Range
#define EVT_MDL_OEM_START           0x00100000
#define EVT_MDL_OEM_END             (EVT_MDL_OEM_START + 0x2FFF)      // 0x00102FFF

// Developer-User Range
#define EVT_MDL_DEV_START           0x00103000
#define EVT_MDL_DEV_END             (EVT_MDL_DEV_START + 0x2FFF)      // 0x00105FFF

// BREW Internal Usage Sub-Ranges
#define EVT_MDL_FMRADIO_START       EVT_MDL_BREW_START                // 0x00010000
#define EVT_MDL_FMRADIO_END         (EVT_MDL_FMRADIO_START + 0xFF)    // 0x000100FF


/////////////////////////////////////////////////////////////////////////////
// ModelEvent and ModelListener data structures

typedef struct IModel IModel;
typedef struct ModelListener ModelListener;

typedef struct ModelEvent {
   uint32         evCode;
   IModel *       pModel;
   uint32         dwParam;    // event specific data (pos, mode, etc.)
} ModelEvent;


typedef void (*PFNLISTENER) (void *pUserData, ModelEvent *pEvent);
typedef void (*PFNLISTENERCANCEL) (ModelListener *pl);

struct ModelListener {
   ModelListener *pNext;
   ModelListener *pPrev;

   PFNLISTENER pfnListener;
   void *pListenerData;

   PFNLISTENERCANCEL pfnCancel;
   void *pCancelData;
};

#define LISTENER_Init(pcb,pcf,pcd) { (pcb)->pfnListener = (PFNLISTENER)(pcf);\
                                     (pcb)->pListenerData = (pcd); }

#define LISTENER_IsRegistered(pcb) (0 != (pcb)->pfnCancel)
#define LISTENER_Cancel(pcb) if (LISTENER_IsRegistered(pcb)) (pcb)->pfnCancel(pcb)


/////////////////////////////////////////////////////////////////////////////
// IModel interface


#define INHERIT_IModel(iname) \
   INHERIT_IQueryInterface(iname); \
   int  (*AddListener)      (iname *p, ModelListener *pListener); \
   void (*Notify)           (iname *p, ModelEvent *pe)

AEEINTERFACE(IModel) {
   INHERIT_IModel(IModel);
};

#define IMODEL_AddRef(p)             AEEGETPVTBL((p),IModel)->AddRef(p)
#define IMODEL_Release(p)            AEEGETPVTBL((p),IModel)->Release(p)
#define IMODEL_QueryInterface(p,c,d) AEEGETPVTBL((p),IModel)->QueryInterface(p,c,d)
#define IMODEL_AddListener(p,pl)     AEEGETPVTBL((p),IModel)->AddListener(p,pl)
#define IMODEL_Notify(p,pe)          AEEGETPVTBL((p),IModel)->Notify(p,pe)

static __inline int IMODEL_AddListenerEx(IModel *p, ModelListener *pml, PFNLISTENER pfn, void *pv)
{
   pml->pfnListener = pfn;
   pml->pListenerData = pv;
   return IMODEL_AddListener(p,pml);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Model Interface ID

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the model framework to indicate the model that a given object
   will use to describe the data associated with that object.  

   The model interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as the base interface for all model interfaces.

   The model interface recognizes the following interface ID.

Definition:

Identifier     Definition
----------     -----------------------------------
AEEIID_MODEL:  Identifies the base model interface

Comments:
   None

See Also:
   IModel Interface
   IWIDGET_GetModel()
   IWIDGET_SetModel()
   
=============================================================================
ModelEvent

Description:
   This data type specifies the structure that is common to every event sent from the
   model interface.  The data structure provides fields to identify the specific model
   event and the originating model, as well as an event dependent field whose
   value will be interpreted differently, depending on the model event identified by
   the 'evCode' parameter.  .
   
Definition:
   typedef struct ModelEvent {
      uint32   evCode;
      IModel   *pModel;
      uint32   dwParam;
   } ModelEvent;

Members:
===pre>
   evCode:   Event code that identifies the type of event.
   pModel:   Pointer to the model that sent the event.   
   dwParam:  An event dependent field that will vary in meaning depending
              on the event identified in the 'evCode' field.
===/pre>

Comments:

See Also:
   Model Events
   ModelListener
   IMODEL_AddListener()
   
=============================================================================
PFNLISTENER

Description:
   This type definition specifies the prototype for the function pointer used as the callback
   function for model listeners.  The callback will be issued by the model to notify the
   registering object of various events involving the model (value changes, etc).

Definition:
    typedef void (*PFNLISTENER)(void *pUserData, ModelEvent *pEvent);

Members:
===pre>
    pUserData:  A pointer to data defined by and returned to the object that has registered
                the model listener.
    pEvent:     The model event being passed to the listener.
===/pre>

Comments:
   None

See Also:
    Model Events
    IMODEL_AddListener()
    IMODEL_AddListenerEx()

=======================================================================
PFNLISTENERCANCEL

Description:
   This type definition specifies the prototype for the function pointer used to cancel
   a previously issued callback for a model listener.

Definition:
    typedef void (*PFNLISTENERCANCEL)(ModelListener *pl);

Members:
===pre>
    pl:  A pointer to the model listener that issued the callback to be cancelled.
===/pre>

Comments:
    The PFNLISTENERCANCEL callback is an element of the ModelListener data type.

See Also:
    ModelListener
    PFNLISTENER

=======================================================================
ModelListener

Description:
   This structure defines a single listener registered with a model, containing
   pointers to the model's data and callback functions.  The model framework
   maintains a linked list of model listeners, each defined by the ModelListener
   data structure.  When model events occur -- for example, when the underlying 
   data value of the model changes -- BREW traverses the list of model listeners 
   notifying each registered listener of the model event.

Definition:
   typedef struct ModelListener {
      ModelListener      *pNext;
      ModelListener      *pPrev;
      PFNLISTENER        pfnListener;
      void               *pListenerData;
      PFNLISTENERCANCEL  pfnCancel;
      void               *pCancelData;
   };

Members:
===pre>
   pNext:          Reserved and used by model
   pPrev:          Reserved and used by model
   pfnListener:    The callback function that BREW will call when notifying registered
                   listeners of model events.  The function is defined by the interested
                   object (a widget, for example) and passed to the model when registering
                   the listener with the model.
   pListenerData:  Pointer to the private data defined by and returned to the object that
                   has registered the model listener.  The data pointer will be returned to
                   the object in the pfnListener callback.
   pfnCancel:      The callback function that BREW will call when canceling a previously issued
                   model listener callback.
   pfnCancelData:  Reserved and used by model. 
===/pre>

Comments:
   Objects register model listeners by calling IMODEL_AddListener.  BREW maintains a linked list
   of model listeners, each represented by a ModelListener data structure.

See Also:
   Model Events
   IMODEL_AddListener()
   PFNLISTENER
   PFNLISTENERCANCEL

===================================================================================
   MACRO DOCUMENTATION
===================================================================================
LISTENER_Init()

Description:
   Widgets and other user interface objects call LISTENER_Init() to initialize the fields
   of a ModelListener data structure prior to registering that ModelListener with a
   particular model.  LISTENER_Init() initializes only the listener callback and private
   data fields of the model listener.  The remaining fields of the ModelListener are
   either reserved for private use by BREW, or may be initialized separately as detailed
   in the description of the ModelListener data structure.  

Prototype:
   LISTENER_Init(ModelListener *pcb, PFNLISTENER pcf, void *pcd)

Parameters:
===pre>
   pcb:  Pointer to the model listener being initialized.
   pcf:  Pointer to the callback function that BREW will call to notify the listening
         object of events involving the model.
   pcd:  Pointer to the private data defined by the object that will be returned to
         the object in the listener callback function.
===/pre>

Return Value:
   None

Comments:
   An object should initialize the listener by calling LISTENER_Init() prior to 
   registering the listener with the model by calling IMODEL_AddListener() or
   IMODEL_AddListernerEx().

Side Effects: 
   None

See Also:
   ModelListener
   IMODEL_AddListener()
   IMODEL_AddListenerEx()
   PFNLISTENER

===================================================================================

LISTENER_IsRegistered()

Description:
   This function checks to see whether or not a given model listener is currently registered
   with the model.

Prototype:
   boolean LISTENER_IsRegistered(ModelListener *pcb)

Parameters:
===pre>
   pcb:  Pointer to a model listener.
===/pre>

Return Value:
   TRUE, if the listener is currently registered with the listener.
   FALSE, if the listener is not currently registered.

Comments:
   A ModelListener is considered registered if the cancel callback function pointer
   is not NULL.
   
   This function does not verify whether or not the passed ModelListener
   pointer is valid.  Applications should verify that the ModelListener pointer
   points to a valid ModelListener before calling LISTENER_IsRegistered().
   
Side Effects: 
  None

See Also:
  ModelListener

===================================================================================

LISTENER_Cancel()

Description:
   This function cancels a previously issued model listener callback.  By calling
   LISTENER_Cancel(), BREW will call the PFNLISTENERCANCEL callback function registered
   with the model listener. 

Prototype:
   LISTENER_Cancel(ModelListener *pcb)

Parameters:
===pre>
   pcb:  Pointer to a model listener.
===/pre>

Return Value:
   None

Comments:
   BREW will verify that the listener is registered and that the cancel callback is
   valid before calling the callback function.  The cancel callback is stored as a
   PFNLISTENERCANCEL in the ModelListener data structure.
   
Side Effects: 
  None

See Also:
  ModelListener
  PFNLISTENERCANCEL
   
===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IModel Interface

Description:
   IModel is the interface for a model in the Model-View-Controller triad.  A model 
   consists of state data along with a notification mechanism to inform its client
   of state changes. 
   
   The model interface provides object reference counting mechanisms that allow objects
   to manage their own memory instances.  In addition, the interface provides APIs for
   registering a listener with a model, as well as interfaces for manipulating the data
   represented by a model.

Required header files:
===pre>
   AEE.h
   AEEStdLib.h
===/pre>

=======================================================================

IMODEL_AddRef()

Description:

   This function increments the reference count of the IModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IMODEL_AddRef(IModel *p);

Parameters:
===pre>
   p:  Pointer to the IModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IMODEL_Release()
   
=======================================================================

IMODEL_Release()

Description:

   This function decrements the reference count of the IModel interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IMODEL_Release(IModel *p);

Parameters:
===pre>
   p:  Pointer to the IModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IModel interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IMODEL_AddRef()
   
=======================================================================

IMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IMODEL_QueryInterface(IModel *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IModel interface object.
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

IMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   model.  The listener includes a callback function that BREW will call whenever the model has
   changed state, thereby notifying the owner of the listener that data has changed.  To illustrate,
   consider a widget that displays the current charge of battery.  The widget itself is responsible
   for displaying battery charge, while the value of the charge would be managed by a model that
   receives updates from the device as battery strength changes.  The battery widget would register
   as a listener for the battery model by calling IMODEL_AddListener().  As the model learns of changes
   to the battery charge, the model would send out notifications to any registered object -- in this
   case, to the battery widget, which could then redraw itself to accurately portray battery strength. 

Prototype:
   int IMODEL_AddListener(IModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the IModel interface object.
   pl:  Pointer to the ModelListener that will be attached to the object.  The ModelListener will
        contain the PFNLISTENER callback function BREW will call to notify the registering object
        of changes to the model.
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

==========================================================================

IMODEL_Notify()

Description:
   This routine is used to notify all objects registered with a model of a 
   change in its state.  It is typically called internally from within a 
   widget and not directly from applications.
   
   Objects wishing to know when a model's state changes can register for 
   notifications by calling IMODEL_AddListener().  When a model's state 
   changes, IMODEL_Notify() is called with the proper event to notify the 
   listening objects of the new state.  IMODEL_Notify() iterates through the 
   list of listeners registered with the model via IMODEL_AddListener() and 
   fires off each of their notification callbacks with the specified event.  
   
   Consider the DateWidget as an example.  The DateWidget uses a model to
   store a date value and implements a date control that displays that 
   value in a human-readable form with controls allowing it to be changed.  
   When a field in the DateWidget is changed by user input, the widget
   immediately updates the date value stored in its model.  It then calls 
   IMODEL_Notify() to notify all listening objects that the actual date
   value displayed by the widget has changed.  Applications can call 
   IWIDGET_GetModel() to gain access to the DateWidget's model and call 
   IMODEL_AddListener() to register for notifications from it.  After doing 
   this, they will be notified each time the displayed date is changed.

Prototype:
   void IMODEL_Notify(IModel *p, ModelEvent *pe); 

Parameters:
===pre>
   p:    Pointer to the IModel interface object.
   pe:   Pointer to the model event that will be sent to the ModelListeners
         registered with the model.

===/pre>
  
Return Value:
   None

Comments:
   This routine is called by widgets internally to notify objects of
   a change in their model's state.  It is not typically called
   from application code.

   When writing your own widget, be sure to allow control to throttle
   calls to IMODEL_Notify() if your data changes rapidly.  The DateWidget, 
   for example, may be set up to notify listeners of a change in its value 
   only when the widget loses focus, rather than after each field change.  
   This limits the number of notification events the model sends.  Without
   this feature, users of the date widget would be bombarded with a
   potentially large number of notifications, possibly impacting 
   performance.

Side Effects: 
   All PFNLISTENER callbacks within the ModelListeners registered with 
   the model via IMODEL_AddListener() will be called.

See Also:
   Model Events
   ModelListener
   IMODEL_AddListener()
   
==========================================================================

IMODEL_AddListenerEx()

Description:
   This macro is used as a shortcut by combining the functionality of LISTENER_Init and 
   IMODEL_AddListener(), allowing callers to initialize the fields of a ModelListener, 
   and register the listener with the model, all in a single API.  

Prototype:
   int IMODEL_AddListenerEx(IModel *p, ModelListener *pml, PFNLISTENER pfn, void *pv); 

Parameters:
===pre>
   p:    Pointer to the IModel interface object.
   pml:  Pointer to the ModelListener that will be attached to the object.  The ModelListener will
         contain the PFNLISTENER callback function BREW will call to notify the registering object
         of changes to the model.
   pfn:  Pointer to the callback function that BREW will call to notify the listening object of
         events involving the model.
   pv:   Pointer to the private data defined by the object that will be returned to the object in
         the listener callback function.
===/pre>
  
Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   ModelListener
   PFNLISTENER
   IMODEL_AddListener()

*/

#endif /* __AEEMODEL_H__ */
