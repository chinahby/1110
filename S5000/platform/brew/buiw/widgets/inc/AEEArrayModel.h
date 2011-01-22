/*
  ========================================================================

  FILE:  AEEArrayModel.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I define an array based listmodel that offers
  open access to its contents. Correspondingly, no memory management
  is done (!)

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/


#ifndef __AEEARRAYMODEL_H__
#define __AEEARRAYMODEL_H__

#include "AEEListModel.h"

#include "bid/AEEIID_ARRAYMODEL.bid"

#include "bid/AEECLSID_ARRAYMODEL.bid"

/////////////////////////////////////////////////////////////////
// IArrayModel interface

typedef struct IArrayModel IArrayModel;
#define INHERIT_IArrayModel(iname) \
   INHERIT_IListModel(iname); \
   void  (*SetItems) (iname *po, void *pItems, uint32 nNumItems, uint16 nItemSize)


AEEINTERFACE(IArrayModel) {
   INHERIT_IArrayModel(IArrayModel);
};

#define IARRAYMODEL_AddRef(p)                AEEGETPVTBL((p),IArrayModel)->AddRef(p)
#define IARRAYMODEL_Release(p)               AEEGETPVTBL((p),IArrayModel)->Release(p)
#define IARRAYMODEL_QueryInterface(p,c,d)    AEEGETPVTBL((p),IArrayModel)->QueryInterface(p,c,d)
#define IARRAYMODEL_AddListener(p,pl)        AEEGETPVTBL((p),IArrayModel)->AddListener(p,pl)
#define IARRAYMODEL_Size(p)                  AEEGETPVTBL((p),IArrayModel)->Size(p)
#define IARRAYMODEL_GetAt(p,i,ppo)           AEEGETPVTBL((p),IArrayModel)->GetAt(p,i,ppo)
#define IARRAYMODEL_SetItems(p,pi,n,s)       AEEGETPVTBL((p),IArrayModel)->SetItems(p,pi,n,s)

// Safe upcasts
static __inline IListModel *IARRAYMODEL_TO_ILISTMODEL(IArrayModel *p) { 
   return (IListModel *)(void*)p; 
}

static __inline IModel *IARRAYMODEL_TO_IMODEL(IArrayModel *p) { 
   return (IModel *)(void*)p; 
}


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Array Model Interface and Class IDs

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the model framework to indicate the model that a given object
   will use to describe the data associated with that object.
   
   The array model supports both an interface and the creation of class object
   instances.  The array model interface or a new instance of the array model
   class object may be created by calling ISHELL_CreateInstance() and passing
   in the desired class id of the object.  The object that is created is,
   itself, a model and may utilize those APIs defined for either the model or
   array model interfaces.

   The array model recognizes the following set of class and interface IDs.

Definition:

Identifier            Definition
----------            -----------------------------------
AEEIID_ARRAYMODEL:    Identifies the array model interface
AEECLSID_ARRAYMODEL:  Identifies the class ID of the array model

Comments:
   None

See Also:
   IArrayModel Interface
   ISHELL_CreateInstance()
   IWIDGET_GetModel()
   IWIDGET_SetModel()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IArrayModel Interface

Description:
   IArrayModel is derived from the IListModel interface and is used to model a collection of
   objects, all of an equal fixed size.  The data of an array model is stored as one contiguous
   block of memory, each item being stored one after the other in equal sized blocks, like so:
   
===pre>
         +__________________________+
         |                          |  Location 0
         |          Item 0          |
         |                          |
         |--------------------------|   
         |                          |  Location (1 x item_size)
         |          Item 1          |
         |                          |
         |--------------------------|
         |                          |  Location (2 x item_size)
         |          Item 2          |
         |                          |
         |--------------------------|
                      :
                      :
         |--------------------------|
         |                          |  Location ((numItems - 1) x item_size)
         |        Item n - 1        |
         |                          |
         +--------------------------+
===/pre>
   
   The data stored within each item of the array model is -- from the standpoint of the array
   model -- opaque.  It's just a block of data that happens to be the same size as every other
   blob of data stored within the collection.  The array model, when queried to retrieve a
   particular item, merely returns a pointer to the location within the array where the requested
   item is stored.
   
   The array model operates on whole array structures and lacks facilities for adding, inserting,
   deleting or replacing individual items.  Changes require that the entire existing array structure
   be replaced with a new array.  The array model is best suited for small lists of items, or lists
   that will not be changing.  For more complex data collections the vector model may be used.
   
   The IArrayModel interface provides APIs for manipulating the data stored in the array model,
   as well as routines for managing reference counting of the interface object, and functions that
   allow widgets and UI controls to register for a listener with the array model.
   
=======================================================================

IARRAYMODEL_AddRef()

Description:

   This function increments the reference count of the IArrayModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IARRAYMODEL_AddRef(IArrayModel *p);

Parameters:
===pre>
   p:  Pointer to the IArrayModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IArrayModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IARRAYMODEL_Release()
   
=======================================================================

IARRAYMODEL_Release()

Description:

   This function decrements the reference count of the IArrayModel interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IARRAYMODEL_Release(IArrayModel *p);

Parameters:
===pre>
   p:  Pointer to the IArrayModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IArrayModel interface object.  If the
   returned reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IARRAYMODEL_AddRef()

=======================================================================

IARRAYMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IARRAYMODEL_QueryInterface(IArrayModel *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IArrayModel interface object.
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

IARRAYMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   array model.  The listener includes a callback function that BREW will call whenever the model has
   changed state, thereby notifying the owner of the listener that data has changed.  For example,
   consider an array model that manages the storage of SMS messages.  Each entry in the array will
   be the same size (perhaps including information about the sender, time of arrival, and a pointer
   to the contents of the message).  An "SMS widget" could register as a listener by calling
   IARRAYMODEL_AddListener().  As new SMS messages arrive and are stored into the array model, BREW will
   send out notifications to all registered objects, providing them the opportunity to update their
   display or react in some other manner.  The SMS widget, for example, as a listener of the array model,
   might update an indicator showing the number of pending SMS messages.

Prototype:
   int IARRAYMODEL_AddListener(IArrayModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the IArrayModel interface object.
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
   ILISTMODEL_AddListener()
   
===========================================================================

IARRAYMODEL_Size()

Description:
   This function retrieves the size of a given array model, returned as the number of items stored
   in the model.

Prototype:
   int IARRAYMODEL_Size(IArrayModel *po); 

Parameters:
===pre>
   po:  Pointer to the IArrayModel interface object.
===/pre>

Return Value:
   The current number of items stored in the array model.

Comments:
   Note that the 'size' returned by IARRAYMODEL_Size is actually the number of items stored in
   the array model, not the size in bytes of either the array or a single item. 

Side Effects: 
   None

See Also:
   IArrayModel Interface

===========================================================================

IARRAYMODEL_GetAt()

Description:
   This function retrieves a pointer to a specific item stored in an array model.  Callers
   provide to IARRAYMODEL_GetAt() the zero-based index of the item to be retrieved, as well
   as a pointer to a pointer, into which IARRAYMODEL_GetAt() will return a reference to the
   requested array item.

Prototype:
   int IARRAYMODEL_GetAt(IArrayModel *po, uint32 nIndex, void **ppoItem); 

Parameters:
===pre>
   po:       Pointer to the IArrayModel interface object.
   nIndex:   Indicates the zero-based index of the array item.  The first item in the array
             is indexed by 0, the second is indexed by 1, and so on...
   ppoItem:  Pointer to a pointer returned by IARRAYMODEL_GetAt() that references the item stored
             at the designated index position within the model.
===/pre>

Return Value:
===pre>
   SUCCESS   - The model was able to successfully return a pointer to the requested item.
   EBADPARM  - The requested index is not within the range of items stored in the model.
===/pre>

Comments:
   The pointer returned in 'ppoItem' is undefined when IARRAYMODEL_GetAt() returns EBADPARM.
   The fixed size of each item stored in the array model must already be known to the caller.

Side Effects: 
   None

See Also:
   IArrayModel Interface
   IARRAYMODEL_SetItems()

===========================================================================

IARRAYMODEL_SetItems()

Description:
   This function sets an array model to point to a new set of items.  The caller is expected
   to manage the memory occupied by the referenced items, with the array model only having
   visibility to the number of items, the fixed size of each item, and a count of the number
   of items stored in the array.

Prototype:
   void IARRAYMODEL_SetItems(IArrayModel *po, void *pItems, uint32 nItems, uint16 nItemSize); 

Parameters:
===pre>
   po:         Pointer to the IArrayModel interface object.
   pItems:     Pointer to the items to be represented by the array model, or NULL if there are
               no items stored in the array.  The memory occupied by the array of item pointers
               is managed by the array model, while the memory occupied by the items themselves
               would be managed by a customized memory freeing callback.
   nItems:     Indicates the number if items stored in the array, or zero if the array is empty.
   nItemSize:  The size, in bytes, of each array item.  Each item in an array model occupies a
               fixed and equal amount of memory.
===/pre>

Return Value:
   None

Comments:
   IARRAYMODEL_SetItems() operates on a full array of items; it cannot be used to set the contents
   of individual array items.

Side Effects: 
   Changing the items stored in an array model by calling IARRAYMODEL_SetItems() will cause BREW to
   send an EVT_MDL_LIST_CHANGE ListModelEvent to any object that has registered as a listener
   with the model.
   
   The fields of the ListModelEvent will contain the following information:
===pre>
       pos:      0 
       oldSize:  The number of items in the array model before calling IARRAYMODEL_SetItems().
       newSize:  The number of items in the array model after calling IARRAYMODEL_SetItems().
===/pre>

See Also:
   Model Events
   ListModelEvent
   IArrayModel Interface
   IARRAYMODEL_GetAt()

==========================================================================

IARRAYMODEL_TO_ILISTMODEL()

Description:
   This function provides type safe casting of an IArrayModel pointer to an IListModel pointer
   and should be used when passing an IArrayModel pointer to functions that require a pointer
   to an IListModel.  This is safer than a simple cast, since the compiler will verify the
   pointer type.

Prototype:
   IListModel *IARRAYMODEL_TO_ILISTMODEL(IArrayModel *p)

Parameters:
===pre>
   p:  Pointer to an IArrayModel interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IListModel pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IListModel Interface
   IArrayModel Interface

==========================================================================

IARRAYMODEL_TO_IMODEL()

Description:
   This function provides type safe casting of an IArrayModel pointer to an IModel pointer
   and should be used when passing an IArrayModel pointer to functions that require a
   pointer to an IModel.  This is safer than a simple cast, since the compiler will verify
   the pointer type prior to the actual cast.

Prototype:
   IModel *IARRAYMODEL_TO_IMODEL(IArrayModel *p)

Parameters:
===pre>
   p:  Pointer to an IArrayModel interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IModel pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IModel Interface
   IArrayModel Interface
 
*/

#endif /* __AEEARRAYMODEL_H__ */
