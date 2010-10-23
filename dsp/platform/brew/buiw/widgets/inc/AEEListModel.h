/*
  ========================================================================

  FILE:  AEEListModel.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I define the abstract list interface. Derived
  interfaces, such as IAraryModel and IVectorModel offer usable
  interfaces and concrete implementations

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEELISTMODEL_H__
#define __AEELISTMODEL_H__

#include "AEEWModel.h"

#include "bid\AEEIID_LISTMODEL.bid"

// Event that the listmodel sends out on changes
typedef struct ListModelEvent {
   ModelEvent  base;
   int         pos;      // position at which change occurred
   int         oldSize;  // old size of list
   int         newSize;  // new size of list
} ListModelEvent;

#define LISTMODELEVENT_Init(pe,c,p,o,n) { (pe)->base.evCode = c; (pe)->pos = p; (pe)->oldSize = o; (pe)->newSize = n; }

// safe upcast
static __inline ModelEvent *LISTMODELEVENT_TO_MODELEVENT(ListModelEvent *p) { 
   return (ModelEvent *)(void*)p;
}

/////////////////////////////////////////////////////////////////////////////
// IListModel interface

typedef struct IListModel IListModel;
#define INHERIT_IListModel(iname) \
   INHERIT_IModel(iname);\
   int (*Size)                 (iname *po); \
   int (*GetAt)                (iname *po, uint32 nIndex, void **ppoItem)

AEEINTERFACE(IListModel) {
   INHERIT_IListModel(IListModel);
};

#define ILISTMODEL_AddRef(p)             AEEGETPVTBL((p),IListModel)->AddRef(p)
#define ILISTMODEL_Release(p)            AEEGETPVTBL((p),IListModel)->Release(p)
#define ILISTMODEL_QueryInterface(p,c,d) AEEGETPVTBL((p),IListModel)->QueryInterface(p,c,d)
#define ILISTMODEL_AddListener(p,pl)     AEEGETPVTBL((p),IListModel)->AddListener(p,pl)
#define ILISTMODEL_Notify(p,pe)          AEEGETPVTBL((p),IListModel)->Notify(p,pe)
#define ILISTMODEL_Size(p)               AEEGETPVTBL((p),IListModel)->Size(p)
#define ILISTMODEL_GetAt(p,n,o)          AEEGETPVTBL((p),IListModel)->GetAt(p,n,o)

// Safe upcast
static __inline IModel *ILISTMODEL_TO_IMODEL(IListModel *p) { 
   return (IModel *)(void*)p; 
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
List Model Interface ID

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the model framework to indicate the model that a given object
   will use to describe the data associated with that object.  

   The list model interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as a base interface for other interfaces (e.g. array model and vector
   model).

   The list model recognizes the following interface ID.

Definition:

Identifier         Definition
----------         -----------------------------------
AEEIID_LISTMODEL:  Identifies the list model interface
   
Comments:
   None

See Also:
   IListModel Interface
   IWIDGET_GetModel()
   IWIDGET_SetModel()
   
=============================================================================

ListModelEvent

Description:
   This data type extends the base model event to include fields used by models that
   organize data as a list (or a collection).  For example, a widget that allows the user
   to page through a list of full screen photographs might choose to model the organization
   of the data as a list.  As items are added and removed from the list, the model would
   send events to listening objects in a ListModelEvent, identifying the specific event code
   in the 'base' ModelEvent, while information pertaining to the structure of the list
   would be stored in the remaining fields of the ListModelEvent data structure.  Events sent
   to objects registered with the list model describe the list in purely structural terms --
   changes to the number of items, and where a particular change has occurred.  The contents
   of the items stored in a list are managed separately, depending on the unique needs of
   the information to be modeled.
   
Definition:
   typedef struct ListModelEvent {
      ModelEvent  base;
      int         pos;
      int         oldSize;
      int         newSize;
   } ListModelEvent;

Members:
===pre>
   base:     The base model event, which identifies the event code and provides the object
             with the listener it had previously registered with the model framework.
   pos:      Indicates a relative position within the list, as defined by the  specific
             event.  For example, when inserting items into a list model, this field
             would indicate the position where the new items have been added.
   oldSize:  Indicates the number of items stored in the list prior to the change in the
             list value.
   newSize:  Indicates the number if items now stored in the list.  The ListModelEvent
             provides both the old and new list sizes so that the object receiving the
             event may adjust its display, or otherwise respond in a manner appropriate to
             that object.
===/pre>
Comments:
   The 'oldSize' and 'newSize' fields are one based values indicating "number of items",
   with 0 indicating that the list is empty.  The 'pos' field is zero based, with 0
   indicating the first item in the list, 1 the second, and so on...

   The following model events will be passed to listening objects in a ListModelEvent
   data structure:
===pre>
       EVT_MDL_LIST_CHANGE
===/pre>

See Also:
   ModelEvent

===================================================================================
   MACRO DOCUMENTATION
===================================================================================

LISTMODELEVENT_Init()

Description:
   This function serves to initialize the fields of a ListModelEvent data structure.
   It initializes the event code field of the base model event, and each of the fields
   unique to the ListModelEvent.

Prototype:
   LISTMODELEVENT_Init(ListModelEvent *pe, uint32 c, int p, int o, int n)

Parameters:
===pre>
   pe:  Pointer to a list model event.  
   c:   Event code that identifies the type of event.
   p:   Indicates a relative position within the list whose value depends on the type
        of change being communicated by the event.  In general, this is the index of
        the item effected by the change.  For example, when an new item is inserted into
        the list, this field identifies the index where the new item was added.  Similarly,
        when an item is deleted, this field identifies the index where the deleted item
        formerly resided.
   o:   Indicates the number of items stored in the list prior to a change to the list.
   n:   Indicates the number of items stored in the list after the change.
===/pre>

Return Value:
   None

Comments:
   None
   
Side Effects: 
  None

See Also:
  ListModelEvent

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IListModel Interface

Description:
   IListModel is inherited from the IModel interface and is used as a base class for models that
   manage data organized in lists or collections.  The IListModel interface is not intended to be
   used directly, instead serving as a base from which other more sophisticated models will be
   derived.  Two examples of models that derive from IListModel are the array and vector models.
   
   As a minimal base class, it is important to note that the IListModel interface models only the
   structure of a simple list -- the size and current position of the list -- not the data actually
   stored in each item of the list.  
   
   IListModel provides object reference counting mechanisms that allow objects to manage their own
   memory instances.  In addition, the interface provides APIs for registering a listener with a
   list model, as well as interfaces for manipulating the structural information stored in the model.
   
=======================================================================

ILISTMODEL_AddRef()

Description:

   This function increments the reference count of the IListModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 ILISTMODEL_AddRef(IListModel *p);

Parameters:
===pre>
   p:  Pointer to the IListModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IListModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   ILISTMODEL_Release()
   
=======================================================================

ILISTMODEL_Release()

Description:

   This function decrements the reference count of the IListModel interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 ILISTMODEL_Release(IListModel *p);

Parameters:
===pre>
   p:  Pointer to the IListModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IListModel interface object.  If the
   returned reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   ILISTMODEL_AddRef()

=======================================================================

ILISTMODEL_QueryInterface()

Description: 
   This function queries an interface for an alternate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int ILISTMODEL_QueryInterface(IListModel *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IListModel interface object.
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

ILISTMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   list model.  The listener includes a callback function that BREW will call whenever the model has
   changed state, thereby notifying the owner of the listener that data has changed.  For example,
   consider a model that derives from IListModel that manages a list of on-line buddies, fetched
   occasionally from a server.  The widget that registers itself as a listener for this model would
   be responsible for displaying the list of buddies, while the list's size, position and other
   data attributes would be managed by the derived list model (the actual data to be displayed would
   likely be managed by a text or value model).  The "buddy widget" would register as a listener for
   the derived list model by calling the AddListener routine for the derived model.  As buddies come
   on-line or go off-line, the model would send out notifications to any registered object with updated
   information about changed information in the model.  The buddy widget, for example, as a listener of
   the derived list model, might then update its display to reflect the current list of on-line buddies.

Prototype:
   int ILISTMODEL_AddListener(IListModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the IListModel interface object.
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

ILISTMODEL_Size()

Description:
   This function retrieves the size of a given list model.  ILISTMODEL_Size() is supplied
   as a stub and is intended to be over ridden by access methods in more sophisticated
   models that derive from IListModel.

Prototype:
   int ILISTMODEL_Size(IListModel *po); 

Parameters:
===pre>
   po:  Pointer to the IListModel interface object.
===/pre>

Return Value:
   The current number of items stored in the list model.

Comments:
   The base class for IListModel always returns a list size of 0 (zero).

Side Effects: 
   None

See Also:
   IListModel Interface

   
===========================================================================

ILISTMODEL_GetAt()

Description:
   This function retrieves a specified item from of a given list model.  ILISTMODEL_GetAt()
   is supplied as a stub and is intended to be over ridden by access methods in more
   sophisticated models that derive from IListModel.

Prototype:
   int ILISTMODEL_GetAt(IListModel *po, uint32 nIndex, void **ppo); 

Parameters:
===pre>
   po:      Pointer to the IListModel interface object.
   nIndex:  Indicates the zero-based index of the list item.  The first item in the list is
            indexed by 0, the second is indexed by 1, and so on...
   ppo:     Pointer to a pointer returned by ILISTMODEL_GetAt() that references the item stored
            at the designated index position within the model.
===/pre>

Return Value:
===pre>
   SUCCESS   - The model was able to successfully return a pointer to the requested item.
   EBADPARM  - The requested index is not within the range of items stored in the model.
===/pre>

Comments:
   The reference implementation always returns EBADPARM.

Side Effects: 
   None

See Also:
   IListModel Interface

===================================================================================

LISTMODELEVENT_TO_MODELEVENT()

Description:
   This function provides type safe casting from ListModelEvent pointers to ModelEvent
   pointers and should be used when passing a ListModelEvent pointer to functions that
   require a pointer to a ModelEvent.  This is safer than a simple cast, since the
   compiler will verify the pointer type prior to the actual cast.

Prototype:
   ModelEvent *LISTMODELEVENT_TO_MODELEVENT(ListModelEvent *p);

Parameters:
===pre>
   p:  Pointer to a ListModelEvent data structure.
===/pre>

Return Value:
   Returns 'p' cast to a ModelEvent pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   ModelEvent
   ListModelEvent

==========================================================================

ILISTMODEL_TO_IMODEL()

Description:
   This function provides type safe casting of an IListModel pointer to an IModel pointer
   and should be used when passing an IListModel pointer to functions that require a
   pointer to an IModel.  This is safer than a simple cast, since the compiler will verify
   the pointer type prior to the actual cast.

Prototype:
   IModel *ILISTMODEL_TO_IMODEL(IListModel *p)

Parameters:
===pre>
   p:  Pointer to an IListModel interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IModel pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IModel Interface
   IListModel Interface
*/

#endif /* __AEELISTMODEL_H__ */
