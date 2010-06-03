#ifndef AEEIMODEL_H
#define AEEIMODEL_H

/*=====================================================================
FILE: AEEIModel.h

SERVICES: IModel interface

DESCRIPTION: This file defines and documents IModel interface

Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================================*/

#include "AEEIQI.h"

#define AEEIID_IModel             0x0101593a

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


// IModel interface
#define INHERIT_IModel(iname) \
   INHERIT_IQI(iname); \
   int  (*AddListener)      (iname *p, ModelListener *pListener); \
   void (*Notify)           (iname *p, ModelEvent *pe)

AEEINTERFACE(IModel) {
   INHERIT_IModel(IModel);
};

static __inline uint32 IModel_AddRef(IModel *p)
{
   return AEEGETPVTBL(p, IModel)->AddRef(p);
}
static __inline uint32 IModel_Release(IModel *p)
{
   return AEEGETPVTBL(p, IModel)->Release(p);
}
static __inline int IModel_QueryInterface(IModel *p, AEEIID c, void **d)
{
   return AEEGETPVTBL(p, IModel)->QueryInterface(p,c,d);
}
static __inline int IModel_AddListener(IModel *p, ModelListener *pl)
{
   return AEEGETPVTBL(p, IModel)->AddListener(p,pl);
}
static __inline void IModel_Notify(IModel *p, ModelEvent *pe)
{
   AEEGETPVTBL(p, IModel)->Notify(p,pe);
}

static __inline int IModel_AddListenerEx(IModel *p, ModelListener *pml, PFNLISTENER pfn, void *pv)
{
   pml->pfnListener = pfn;
   pml->pListenerData = pv;
   return IModel_AddListener(p,pml);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Model Interface IDs

Description:
   Each model is identified by a unique interface ID.  

Interface ID            Definition
------------            -----------------------------------
AEEIID_IModel:           Identifies the base model interface

   
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
      uint32         evCode;
      IModel        *pModel;
      uint32         dwParam;
   } ModelEvent;

Members:
===pre>
   evCode:          Event code that identifies the type of event.
   pModel:          Pointer to the model that sent the event.   
   dwParam:         An event dependent field that will vary in meaning depending
                    on the event identified in the 'evCode' field.
===/pre>

Comments:

See Also:
   Model Events
   ModelListener
   IModel_AddListener()
   

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
    IModel_AddListener()
    IModel_AddListenerEx()

=======================================================================
PFNLISTENERCANCEL

Description:
   This type definition specifies the prototype for the function pointer used to cancel
   a previously isued callback for a model listener.

Definition:
    typedef void (*PFNLISTENERCANCEL)(ModelListener *pl);

Members:
===pre>
    pl:  A pointer to the model listener that issued the callback to be cancelled.
===/pre>

Comments:
    The PFNLISTENERCANCEL callback is an element of the ModelListener data type.

See Also:
    PFNLISTENER
    ModelListener

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
   pfnCancel:      The callback function that BREW will call when cancelng a previously issued
                   model listener callback.
   pfnCancelData:  Reserved and used by model. 
===/pre>

Comments:
   Objects register model listeners by calling IModel_AddListener.  BREW maintains a linked list
   of model listeners, each represented by a ModelListener data structure.

See Also:
   Model Events
   PFNLISTENER
   PFNLISTENERCANCEL
   IModel_AddListener()

===================================================================================
   MACRO DOCUMENTATION
===================================================================================
LISTENER_Init()

Description:
   Widgets and other user interface obects call LISTENER_Init() to initialize the fields
   of a ModelListener data structure prior to registering that ModelListener with a
   particular model.  LISTENER_Init() initializes only the listerner callback and private
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
   registering the listener with the model by calling IModel_AddListener() or
   IMODEL_AddListernerEx().

Side Effects: 
   None

See Also:
   ModelListener
   PFNLISTENER
   IModel_AddListener()
   IModel_AddListenerEx()

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
   TRUE: if the listener is currently registered with the listener.
   FALSE: if the listener is not currently registered.

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
   consists of state data along with a notification mechanism to inform it's client
   of state changes. 
   
   The model interface provides object reference counting mechanisms that allow objects
   to manage their own memory instances.  In addition, the interface provides APIs for
   registering a listener with a model, as well as interfaces for manipulating the data
   represented by a model.

Required header files:
   AEE.h
   AEEStdLib.h

=======================================================================

IModel_AddRef()

Description:

   This function increments the reference count of the IModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IModel_AddRef(IModel *p);

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
   IModel_Release()
   
=======================================================================

IModel_Release()

Description:

   This function decrements the reference count of the IModel interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IModel_Release(IModel *p);

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
   IModel_AddRef()
   
=======================================================================

IModel_QueryInterface()

Description: 
   This function asks an object for another API contract from the object in question.

Prototype:

   int IModel_QueryInterface(IModel *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IModel interface object.
   c:  Requested class ID exposed by the object
   d:  Pointer to the returned IModel interface object.
===/pre>

Return Value:
   AEE_SUCCESS:           Interface found
   AEE_ENOMEMORY:         Insufficient memory
   AEE_ECLASSNOTSUPPORT:  Requested interface is unsupported

Comments:  
   The IModel pointer in 'd' is set to the new interface (with refcount positive), or NULL if the
   ClassID is not supported by the object.  'p' must _not_ be set to NULL.

Side Effects: 
   None

See Also:
   IQI_QueryInterface()
   
==========================================================================

IModel_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   model.  The listener includes a callback function that BREW will call whenever the model has
   changed state, thereby notifying the owner of the listener that data has changed.  To illustrate,
   consider a widget that displays the current charge of battery.  The widget itself is responsible
   for displaying battery charge, while the value of the charge would be managed by a model that
   receives updates from the device as battery strength changes.  The battery widget would register
   as a listener for the battery model by calling IModel_AddListener().  As the model learns of changes
   to the battery charge, the model would send out notifications to any registered object -- in this
   case, to the battery widget, which could then redraw itself to accurately portray battery strength. 

Prototype:
   int IModel_AddListener(IModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the IModel interface object.
   pl:  Pointer to the ModelListener that will be attched to the object.  The ModelListener will
        contain the PFNLISTENER callback function BREW will call to notify the registering object
        of changes to the model.
===/pre>
   
Return Value:
   AEE_SUCCESS:           Listener added successfully
   AEE_EBADPARM:          Function parameter was invalid

Comments:  
   None

Side Effects: 
   None

See Also:
   ModelListener
   PFNLISTENER

==========================================================================

IModel_AddListenerEx()

Description:
   This macro is used as a shortcut by combining the functionality of LISTENER_Init and 
   IModel_AddListener(), allowing callers to initialize the fields of a ModelListener, 
   and register the listener with the model, all in a single API.  

Prototype:
   int IModel_AddListenerEx(IModel *p, ModelListener *pml, PFNLISTENER pfn, void *pv); 

Parameters:
===pre>
   p:    Pointer to the IModel interface object.
   pml:  Pointer to the ModelListener that will be attched to the object.  The ModelListener will
         contain the PFNLISTENER callback function BREW will call to notify the registering object
         of changes to the model.
   pfn:  Pointer to the callback function that BREW will call to notify the listening object of
         events involving the model.
   pv:   Pointer to the private data defined by the object that will be returned to the object in
         the listener callback function.
===/pre>
  
Return Value:
   AEE_SUCCESS:           Listener added successfully
   AEE_EBADPARM           Function parameter was invalid

Comments:  
   None

Side Effects: 
   None

See Also:
   ModelListener
   PFNLISTENER
   IModel_AddListener()


==========================================================================

IModel_Notify()

Description:
   This function is used to send a notify event to all listeners that are currently
   installed on the model. The notification will be sent to the function specified
   by the 'pfnListener' member of the ModelListener data structure passed to 
   IModel_AddListener().

Prototype:
   void IModel_Notify(IModel *p, ModelEvent *pe);
   
Parameters:
===pre>
   p:    Pointer to the IModel interface object.
   pe:   Pointer to a ModelEvent structure containing the event code and
         event-specific data.
   
===/pre>
  
Return Value:
   None

Comments:  
   This function is typically used only by Model implementors, but may in some
   cases be used when application code needs to force a notification to a model
   listener. 

Side Effects: 
   None

See Also:
   ModelListener
   PFNLISTENER
   IModel_AddListener()

=====================================================================*/

#endif // AEEIMODEL_H
