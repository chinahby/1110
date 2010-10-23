/*
  ========================================================================

  FILE:  AEEVectorModel.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I define a model that is an automatically
  growing array of pointers.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEVECTORMODEL_H__
#define __AEEVECTORMODEL_H__


#include "AEEListModel.h"

#include "bid\AEEIID_VECTORMODEL.bid"

#include "bid\AEECLSID_VECTORMODEL.bid"

#define VECTOR_LAST              (uint32) 0xFFFFFFFF

/////////////////////////////////////////////////////////////////
// IVectorModel interface

typedef struct IVectorModel IVectorModel;
#define INHERIT_IVectorModel(iname) \
   INHERIT_IListModel(iname); \
   int         (*ReplaceAt)      (iname *po, uint32 nIndex, void *pvItem); \
   int         (*InsertAt)       (iname *po, uint32 nIndex, void *pvItem); \
   int         (*DeleteAt)       (iname *po, uint32 nIndex); \
   void        (*DeleteAll)      (iname *po); \
   int         (*EnsureCapacity) (iname *po, uint32 nRequired, uint32 nGrowBy); \
   PFNNOTIFY   (*SetPfnFree)     (iname *po, PFNNOTIFY pfnFree)


AEEINTERFACE(IVectorModel) {
   INHERIT_IVectorModel(IVectorModel);
};


#define IVECTORMODEL_AddRef(p)             AEEGETPVTBL((p),IVectorModel)->AddRef(p)
#define IVECTORMODEL_Release(p)            AEEGETPVTBL((p),IVectorModel)->Release(p)
#define IVECTORMODEL_QueryInterface(p,c,d) AEEGETPVTBL((p),IVectorModel)->QueryInterface(p,c,d)
#define IVECTORMODEL_AddListener(p,pl)     AEEGETPVTBL((p),IVectorModel)->AddListener(p,pl)
#define IVECTORMODEL_Notify(p,pe)          AEEGETPVTBL((p),IVectorModel)->Notify(p,pe)
#define IVECTORMODEL_Size(p)               AEEGETPVTBL((p),IVectorModel)->Size(p)
#define IVECTORMODEL_GetAt(p,i,ppo)        AEEGETPVTBL((p),IVectorModel)->GetAt(p,i,ppo)
#define IVECTORMODEL_ReplaceAt(p,i,pv)     AEEGETPVTBL((p),IVectorModel)->ReplaceAt(p,i,pv)
#define IVECTORMODEL_InsertAt(p,i,pv)      AEEGETPVTBL((p),IVectorModel)->InsertAt(p,i,pv)       
#define IVECTORMODEL_DeleteAt(p,i)         AEEGETPVTBL((p),IVectorModel)->DeleteAt(p,i)          
#define IVECTORMODEL_DeleteAll(p)          AEEGETPVTBL((p),IVectorModel)->DeleteAll(p)         
#define IVECTORMODEL_EnsureCapacity(p,r,g) AEEGETPVTBL((p),IVectorModel)->EnsureCapacity(p,r,g)
#define IVECTORMODEL_SetPfnFree(p,pfn)     AEEGETPVTBL((p),IVectorModel)->SetPfnFree(p,pfn)

#define IVECTORMODEL_Add(p,pv)             IVECTORMODEL_InsertAt(p,VECTOR_LAST,pv)

// special value for IVECTORMODEL_SetPfnFree, indicates that stored
// ptrs are interface ptrs and should be Released
#define VF_RELEASE   ((PFNNOTIFY)-1)

// Safe upcasts
static __inline IListModel *IVECTORMODEL_TO_ILISTMODEL(IVectorModel *p) { 
   return (IListModel *)(void*)p; 
}

static __inline IModel     *IVECTORMODEL_TO_IMODEL(IVectorModel *p) { 
   return (IModel *)(void*)p; 
}


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Vector Model Interface ID

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the model framework to indicate the model that a given object
   will use to describe the data associated with that object.
   
   The vector model supports both an interface and the creation of class object
   instances.  The vector model interface or a new instance of the vector model
   class object may be created by calling ISHELL_CreateInstance() and passing
   in the desired class id of the object.  The object that is created is,
   itself, a model and may utilize those APIs defined for either the model or
   vector model interfaces.

   The vector model recognizes the following set of class and interface IDs.

Definition:

Identifier             Definition
----------             -----------------------------------
AEEIID_VECTORMODEL:    Identifies the vector model interface
AEECLSID_VECTORMODEL:  Identifies the class ID of the vector model
   
Comments:
   None

See Also:
   IVectorModel Interface
   ISHELL_CreateInstance()
   IWIDGET_GetModel()
   IWIDGET_SetModel()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IVectorModel Interface

Description:
   IVectorModel is derived from the IListModel interface and is used to model a collection of
   objects that may be of some variable size.  Vector models benefit from APIs that provide the
   ability to add, insert, delete and replace items stored in the model and allow each item to
   vary in size and structure.  Each item in a vector model is referenced by a pointer,  with all
   the item pointers collected together in one contiguous block of memory.  Basically, the vector
   model is stored as an array of pointers, like so:
   
===pre>
                    __________________________ 
           Item 0  |                          |
                   |          void *      +---------> Points to the contents of item 0
                   |                          |
                   |--------------------------|   
           Item 1  |                          |
                   |          void *      +---------> Points to the contents of item 1
                   |                          |
                   |--------------------------|
           Item 2  |                          |
                   |          void *      +---------> Points to the contents of item 2
                   |                          |
                   |--------------------------|
                                :
                                :
                   |--------------------------|
        Item n - 1 |                          | 
                   |          void *      +---------> Points to the contents of item n - 1
                   |                          |
                    --------------------------
===/pre>
   
   The block of item pointers is actually accessed by the vector model through a second pointer.
   
   The contents of each item in the vector could be of a different size and structure, however this
   distinction is opaque to the vector model itself, which instead sets its focus on managing the
   pointers to each item.  From the standpoint of the vector model, the items themselves are simply
   blobs of data.  The vector model, when queried to retrieve a particular item, returns a pointer to
   the location where that item's contents are stored.
   
   The IVectorModel interface provides APIs for manipulating the data stored in the vector model,
   as well as routines for managing reference counting of the interface object, and functions that
   allow widgets and UI controls to register for a listener with the array model.
   
=======================================================================

IVECTORMODEL_AddRef()

Description:

   This function increments the reference count of the IVectorModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IVECTORMODEL_AddRef(IVectorModel *p);

Parameters:
===pre>
   p:  Pointer to the IVectorModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IVectorModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IVECTORMODEL_Release()
   
=======================================================================

IVECTORMODEL_Release()

Description:

   This function decrements the reference count of the IVectorModel interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IVECTORMODEL_Release(IVectorModel *p);

Parameters:
===pre>
   p:  Pointer to the IVectorModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IVectorModel interface object.  If the
   returned reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IVectorMODEL_AddRef()

=======================================================================

IVECTORMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IVECTORMODEL_QueryInterface(IVectorModel *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IVectorModel interface object.
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

IVECTORMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   vector model.  The listener includes a callback function that BREW will call whenever the model
   has changed state, thereby notifying the owner of the listener that data has changed.  For example,
   a collection of MP3 songs might use the vector model to store information about the title, time,
   genre and the actual encoded data for each stored file -- each, as a variable length item within
   the model.  An "MP3 browser widget" could register as a listener for this model by calling
   IVECTORMODEL_AddListener().  As new MP3 files are acquired and stored into the vector model, BREW will
   send out notifications to all registered objects, providing them the opportunity to update their
   display or react in some other manner.  The MP3 browser widget, for example, as a listener of the
   vector model, might update its display to show the title and length of new songs.

Prototype:
   int IVECTORMODEL_AddListener(IVectorModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the IVectorModel interface object.
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
   
==========================================================================

IVECTORMODEL_Size()

Description:
   This function retrieves the size of a given vector model, returned as the number of items stored
   in the model.

Prototype:
   int IVECTORMODEL_Size(IVectorModel *po); 

Parameters:
===pre>
   po:  Pointer to the IVectorModel interface object.
===/pre>

Return Value:
   The current number of items stored in the vector model.

Comments:
   Note that the 'size' returned by IVECTORMODEL_Size() is actually the number of items stored in
   the vector model, not the size in bytes of a single item or the entire model. 

Side Effects: 
   None

See Also:
   IVectorModel Interface

===========================================================================

IVECTORMODEL_GetAt()

Description:
   This function retrieves a pointer to a specific item stored in a vector model.  Callers
   provide to IVECTORMODEL_GetAt() the zero-based index of the item to be retrieved, as well
   as a pointer to a pointer, into which IVECTORMODEL_GetAt() will return a reference to the
   requested item.

Prototype:
   int IVECTORMODEL_GetAt(IVectorModel *po, uint32 nIndex, void **ppoItem); 

Parameters:
===pre>
   po:       Pointer to the IVectorModel interface object.
   nIndex:   Indicates the zero-based index of the item to be retrieved.  The first item in
             the model is indexed by 0, the second is indexed by 1, and so on...
   ppoItem:  Pointer to a pointer returned by IVECTORMODEL_GetAt() that references the item stored
             at the designated index position within the model.
===/pre>

Return Value:
===pre>
   SUCCESS   - The model was able to successfully return a pointer to the requested item.
   EBADPARM  - The requested index is not within the range of items stored in the model.
===/pre>

Comments:
   The pointer returned in 'ppoItem' will be NULL when IVECTORMODEL_GetAt() returns EBADPARM.

Side Effects: 
   None

See Also:
   IVectorModel Interface

===========================================================================

IVECTORMODEL_ReplaceAt()

Description:
   This function replaces the item stored at a specified index location within the model, with a
   different item.  Callers provide to IVECTORMODEL_ReplaceAt() the zero-based index of the item
   to be replaced, as well as a pointer to the replacement item.  IVECTORMODEL_ReplaceAt() merely
   replaces the original item pointer with a new item pointer and does not actually replace the
   original item contents.  However, the memory occupied by the original item _will_  be released
   by the vector model's PFNNOTIFY callback function, if such a callback has been established by a
   prior call to IVECTORMODEL_SetPfnFree().
   
   The figures below illustrates the state of the information stored in a vector model before and
   after calling IVECTORMODEL_ReplaceAt().  Because items in the vector model are zero-based, the
   caller would specify an index that is one less than the item to be removed when calling
   IVECTORMODEL_ReplaceAt().  For example, to replace the third item in the model, the caller would
   specify '2' as the index of the item to be replaced.  In the figure, the item to be removed is
   stored at location index-1 in the array of item content pointers, and points to the contents of
   the original item.
   
===pre>
         Vector Model before calling IVECTORMODEL_ReplaceAt()
         
                                :
                                :
                   |--------------------------|   
  Item (index - 2) |                          |
                   |          void *      +---------> Points to the contents of item (index - 2)
                   |                          |
                   |--------------------------|
                   |                          |
  Item (index - 1) |      old void *      +--------->  --------------------------
                   |                          |       |                          |
                   |--------------------------|       |      Contents of the     |
                                :                     |       original item      |
                                :                     |                          |
                                                      |                          |
                                                       --------------------------
===/pre>

   After calling IVECTORMODEL_ReplaceAt(), the memory occupied by the original index-1 item will be
   released, and the original index-1 pointer will have been replaced by a new pointer, now pointing
   to the contents of the new item referenced by index-1. 
   
===pre>
         Vector Model after calling IVECTORMODEL_ReplaceAt()
         
                                :
                                :
                   |--------------------------|   
  Item (index - 2) |                          |
                   |          void *      +---------> Points to the contents of item (index - 2)
                   |                          |
                   |--------------------------|
                   |                          |
  Item (index - 1) |      new void *      +-------     --------------------------
                   |                          |  |    |                          |
                   |--------------------------|  |    |        RELEASED!!!       |
                                :                |    |      Contents of the     |
                                :                |    |       original item      |
                                                 |    |                          |
                                                 |     --------------------------
                                                 |     
                                                  -->  --------------------------
                                                      |                          |
                                                      |                          |   
                                                      |      Contents of the     |
                                                      |          new item        |
                                                      |                          |
                                                       --------------------------
===/pre>

Prototype:
   int IVECTORMODEL_ReplaceAt(IVectorModel *po, uint32 nIndex, void *pvNew); 

Parameters:
===pre>
   po:      Pointer to the IVectorModel interface object.
   nIndex:  Indicates the zero-based index of the item to be replaced.  The first item in
            the model is indexed by 0, the second is indexed by 1, and so on...
   pvNew:   Pointer to the contents of the new item that is replacing the item indexed by
            nIndex.
===/pre>

Return Value:
===pre>
   SUCCESS   - The model was able to successfully replace the desired item.
   EBADPARM  - The requested index is not within the range of items stored in the model.
===/pre>

Comments:
   The vector model may define a custom free memory routine to release the memory consumed by
   complex items.  The custom free memory routine is registered with the vector model by
   calling IVECTORMODEL_SetPfnFree().

Side Effects: 
   Replacing an item stored in a vector model by calling IVECTORMODEL_ReplaceAt() will cause BREW
   to send an EVT_MDL_LIST_CHANGE ListModelEvent to any object that has registered as a listener
   with the model.

   The fields of the ListModelEvent will contain the following information:
===pre>
       pos:      The index of the vector item that has been replace. 
       oldSize:  The number of items in the vector model.
       newSize:  The number of items in the vector model.
===/pre>

See Also:
   Model Events
   ListModelEvent
   IVectorModel Interface
   IVECTORMODEL_SetPfnFree()

===========================================================================

IVECTORMODEL_Add()

Description:
   This function adds a new item to a vector model, adding the item at the end of the current
   list of stored items.  The item being added to the model is referenced by a pointer to
   the item's contents, with the pointer (rather than the contents) actually being stored
   in the vector model.

Prototype:
   int IVECTORMODEL_Add(IVectorModel *po, void *pvItem);

Parameters:
===pre>
   po:      Pointer to the IVectorModel interface object.
   pvItem:  Pointer to the contents of the item being added to the vector model.
===/pre>
   
Return Value:
===pre>
   SUCCESS    - The item was successfully added to the vector model.
   ENOMEMORY  - The vector model was unable to allocate enough memory to accommodate the new item.
===/pre>

Comments:
   IVECTORMODEL_Add() will check to see if there is sufficient allocated space already available
   in the vector model and use that space to store the new item.  If space is not available,
   IVECTORMODEL_Add() will attempt to allocate storage for the new item, and any additional items
   as previously declared in a call to IVECTORMODEL_EnsureCapacity().

Side Effects: 
   Adding an item to a vector model by calling IVECTORMODEL_Add() will cause BREW to send
   an EVT_MDL_LIST_CHANGE ListModelEvent to any object that has registered as a listener with
   the model.

   The fields of the ListModelEvent will contain the following information:
===pre>
       pos:      The position in the vector model where the new item has been added.  When
                 adding at the end of the vector model this value will be equal to the number
                 of items in the model prior to calling IVECTORMODEL_Add().
       oldSize:  The number of items in the vector model before calling IVECTORMODEL_Add().
       newSize:  The number of items in the vector model after calling IVECTORMODEL_Add().
===/pre>
   
See Also:
   Model Events
   ListModelEvent
   IVectorModel Interface
   IVECTORMODEL_EnsureCapacity()

===========================================================================

IVECTORMODEL_InsertAt()

Description:
   This function inserts a new item to a vector model, adding the new item before the item
   specified in the function call.  The item being added to the model is referenced by a pointer
   to the item's contents, with the pointer (rather that the contents) actually being stored
   in the vector model.

Prototype:
   int IVECTORMODEL_InsertAt(IVectorModel *po, uint32 nIndex, void *pvItem);

Parameters:
===pre>
   po:      Pointer to the IVectorModel interface object.
   nIndex:  Indicates the zero-based index of the item before which the new item will be added.
            The first item in the model is indexed by 0, the second is indexed by 1, and so on.
            So, if IVECTORMODEL_InsertAt() is called with nIndex equal to 2, the new item will be
            inserted at index 2, with items 2, 3, 4, etc all shifting one position higher. 
            If IVECTORMODEL_InsertAt is called with VECTOR_LAST (-1) the new item is isserted at 
            the end of the vector
   pvItem:  Pointer to the contents of the item being added to the vector model.
===/pre>
   
Return Value:
===pre>
   SUCCESS    - The item was successfully added to the vector model.
   ENOMEMORY  - The vector model was unable to allocate enough memory to accommodate the new item.
===/pre>

Comments:
   The vector model inserts items by first growing the array of item pointers stored in the model by
   one item, moving the existing items from 'nIndex' to the end of the list in order to make room for
   the new item, then placing the new pointer ('pvItem') at 'nIndex' in the array.
 
   IVECTORMODEL_InsertAt() will check to see if there is sufficient allocated space already available
   in the vector model and use that space to store the new item.  If space is not available,
   IVECTORMODEL_InsertAt() will attempt to allocate storage for the new item, and any additional items
   as previously declared in a call to IVECTORMODEL_EnsureCapacity().
  
   Passing -1 (MAXINT) for 'nIndex' will add the new item to the end of the vector model.

Side Effects: 
   Inserting an item in a vector model by calling IVECTORMODEL_InsertAt() will cause BREW to send
   an EVT_MDL_LIST_CHANGE ListModelEvent to any object that has registered as a listener with
   the model.

   The fields of the ListModelEvent will contain the following information:
===pre>
       pos:      The index of the item before which the new item was inserted.
       oldSize:  The number of items in the vector model before calling IVECTORMODEL_InsertAt().
       newSize:  The number of items in the vector model after calling IVECTORMODEL_InsertAt().
===/pre>
   
See Also:
   Model Events
   ListModelEvent
   IVectorModel Interface
   IVECTORMODEL_EnsureCapacity()

===========================================================================

IVECTORMODEL_DeleteAt()

Description:
   This function deletes an existing item from a vector model, releasing the memory referenced by the
   item pointer and removing that pointer from the model's array of item pointers.  

Prototype:
   int IVECTORMODEL_DeleteAt(IVectorModel *po, uint32 nIndex);

Parameters:
===pre>
   po:      Pointer to the IVectorModel interface object.
   nIndex:  Indicates the zero-based index of the item to be removed from the vector model.  The first
            item in the model is indexed by 0, the second is indexed by 1, and so on.  So, if
            IVECTORMODEL_DeleteAt is called with nIndex equal to 2, item 2 will be removed from the
            vector model, with items 3, 4, 5, etc all shifting one position lower.
===/pre>
   
Return Value:
===pre>
   SUCCESS   - The model was able to successfully delete the desired item.
   EBADPARM  - The requested index is not within the range of items stored in the model.
===/pre>

Comments:
   Following a successful call to IVECTORMODEL_DeleteAt(), the number of items in the model is reduced
   by one.  The array of item pointers in the vector model is also reduced by one pointer, with the
   'nIndex' pointer being removed and all pointers above being moved down by one position.

   The vector model may define a custom free memory routine to release the memory consumed by
   complex items.  The custom free memory routine is registered with the vector model by
   calling IVECTORMODEL_SetPfnFree().

Side Effects: 
   Deleting an item in a vector model by calling IVECTORMODEL_DeleteAt() will cause BREW to send
   an EVT_MDL_LIST_CHANGE ListModelEvent to any object that has registered as a listener with
   the model.

   The fields of the ListModelEvent will contain the following information:
===pre>
       pos:      The index of the item that has been deleted from the vector model
       oldSize:  The number of items in the vector model before calling IVECTORMODEL_DeleteAt().
       newSize:  The number of items in the vector model after calling IVECTORMODEL_DeleteAt().
===/pre>

See Also:
   Model Events
   ListModelEvent
   IVectorModel Interface

===========================================================================

IVECTORMODEL_DeleteAll()

Description:
   This function deletes all of the items stored in a vector model, releasing the memory referenced
   by each of the item pointers, removing all of the pointers from the model's array of item pointers,
   and setting the size of the vector model to 0 (zero).  

Prototype:
   void IVECTORMODEL_DeleteAll(IVectorModel *po);

Parameters:
===pre>
   po:  Pointer to the IVectorModel interface object.
===/pre>
   
Return Value:
   None

Comments:
   The vector model may define a custom free memory routine to release the memory consumed by
   complex items.  The custom free memory routine is registered with the vector model by
   calling IVECTORMODEL_SetPfnFree() and will be called for each item stored in the vector model
   when issuing IVECTORMODEL_DeleteAll().

Side Effects: 
   Deleting the items in a vector model by calling IVECTORMODEL_DeleteAll() will cause BREW to
   send an EVT_MDL_LIST_CHANGE ListModelEvent to any object that has registered as a listener with
   the model.

   The fields of the ListModelEvent will contain the following information:
===pre>
       pos:      0
       oldSize:  The number of items in the vector model before calling IVECTORMODEL_DeleteAll().
       newSize:  0 (since there are now no more items in the vector model)
===/pre>

See Also:
   Model Events
   ListModelEvent
   IVectorModel Interface
   IVECTORMODEL_SetPfnFree()

===========================================================================

IVECTORMODEL_EnsureCapacity()

Description:
   This function allows callers to preflight the memory that will be needed by a vector model.
   For example, if an implementation knows ahead of time that it will require, as a minimum,
   some established number of data items, it may allocate the storage for these items in one
   operation and know immediately whether or not there is sufficient memory to support the 
   entire collection of data.  This is more efficient than calling IVECTORMODEL_Add() for every
   item in the collection, only to encounter a failure well into the allocation of items. 
   
   Two sizing parameters are passed to IVECTORMODEL_EnsureCapacity() -- one that specifies the
   minimum number of items that will be needed by the vector model, and one that specifies the
   number of items that the model should grow when adding items beyond the minimum required size
   of the model.  To illustrate, consider a vector model that currently contains 8 items.  If
   IVECTORMODEL_EnsureCapacit()y is called specifying a required minimum of 20 items, with a growth
   factor of 5 items, the following will happen:
===pre>
        - The vector model will immediately grow to 20 items, adding space for 20 - 8 = 12 items.
        - New items added or inserted into the vector model will consume the newly allocated (but
          unoccupied) space.
        - At the point at which an added item would cause the model to grow beyond the current
          allocated space of 20 items, space for 5 additional items will be allocated by the vector
           model.
===/pre>   

Prototype:
   void IVECTORMODEL_EnsureCapacity(IVectorModel *po, uint32 nRequired, uint32 nGrowBy);

Parameters:
===pre>
   po:         Pointer to the IVectorModel interface object.
   nRequired:  The minimum number of items that will be required by the vector model.  The vector
               model will immediately grow to contain space for exactly this number of items (or
               fail, if additional memory could not be allocated).
   nGrowBy:    Indicates the number of additional items that should be allocated when adding or
               inserting items beyond the currently allocated number of items.
===/pre>
   
Return Value:
===pre>
   SUCCESS   - The vector model was able to size itself to be at least nRequired items.
   ENOMEMORY - The vector model was unable to allocate enough memory to accommodate the number of
               required items.
===/pre>

Comments:
   IVECTORMODEL_EnsureCapacity() only ensures that there is enough space in the vector model for
   the item pointers; it does not ensure that there is enough memory available for the memory
   required by the individual items.  Applications may also wish to preflight the memory that would
   be required for the individual item contents, if they wish to make absolutely sure that memory
   is available to accommodate a large allocation of vector items.
   
   The size of the vector model will change only if the currently allocated number of items is
   less than the number of required items.  When nRequired is less than the current size of the
   vector model, the model does _not_ shrink to the specified size.

Side Effects: 
   None

See Also:
   IVectorModel Interface
   IVECTORMODEL_Add()
   IVECTORMODEL_InsertAt()

===========================================================================

IVECTORMODEL_SetPfnFree()

Description:
   This function specifies the callback function to be used by the vector model when releasing
   memory occupied by items stored in the vector model.  The vector model maintains an array of
   pointers to the contents of each item, with the items defining their individual structure and
   storage requirements.  The callback function allows a vector model to release "deep storage" --
   custom memory allocated and occupied by the fields of the items stored in a vector model.
   
   The figure below illustrates complex items in a vector model that possess "deep storage".  A
   pointer to the contents of item x is stored in the vector model's array of item pointers.  The
   item itself then includes a pointer to additional storage allocated and used by item x.  To
   release this storage, the vector model would provide a PFNNOTIFY callback that would free the
   contents of item x, as well as the custom memory pointed to from within item x.
   
===pre>
                                                      +--------------------------+
                                                      |                          |
                                :                     |        Contents of       |
                                :                     |           item x         |
                   |--------------------------|       |                          |       +---------+
       Item x      |                          |       |    Pointer to custom +---------> |         |
                   |          void *      +---------> |          storage         |       |         |
                   |                          |       +--------------------------+       +---------+
                   |--------------------------|
                   |                          |
     Item (x + 1)  |          void *      +---------> +--------------------------+
                   |                          |       |                          |
                   |--------------------------|       |        Contents of       |
                                :                     |        item (x + 1)      |
                                :                     |                          |
                                                      |                          |
                                                      +--------------------------+
===/pre>

Prototype:
   PFNNOTIFY IVECTORMODEL_SetPfnFree(IVectorModel *po, PFNNOTIFY pfnFree);

Parameters:
===pre>
   po:       Pointer to the IVectorModel interface object.
   pfnFree:  Pointer to the callback function that BREW will call when freeing the memory
             associated with an item stored in the vector model.
===/pre>
   
Return Value:
   IVECTORMODEL_SetPfnFree() returns a pointer to the vector model's old PFNNOTIFY callback function.
   A vector model may therefore manage multiple PFNNOTIFY callbacks, each capable of managing
   different item contents.

Comments:
   Pass VF_RELEASE in 'pfnFree' to indicate that the item pointers stored in the vector model are
   actually interface pointers.  When this is the case, the vector model will release the interfaces
   by calling IBASE_Release().

Side Effects: 
   None

See Also:
   IVectorModel Interface
   PFNNOTIFY

==========================================================================

IVECTORMODEL_TO_ILISTMODEL()

Description:
   This function provides type safe casting of an IVectorModel pointer to an IListModel pointer
   and should be used when passing an IVectorModel pointer to functions that require a pointer
   to an IListModel.  This is safer than a simple cast, since the compiler will verify the
   pointer type.

Prototype:
   IListModel *IVECTORMODEL_TO_ILISTMODEL(IVectorModel *p)

Parameters:
===pre>
   p:  Pointer to an IVectorModel interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IListModel pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IListModel Interface
   IVectorModel Interface

==========================================================================

IVECTORMODEL_TO_IMODEL()

Description:
   This function provides type safe casting of an IVectorModel pointer to an IModel pointer
   and should be used when passing an IVectorModel pointer to functions that require a
   pointer to an IModel.  This is safer than a simple cast, since the compiler will verify
   the pointer type prior to the actual cast.

Prototype:
   IModel *IVECTORMODEL_TO_IMODEL(IVectorModel *p)

Parameters:
===pre>
   p:  Pointer to an IVectorModel interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IModel pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IModel Interface
   IVectorModel Interface

*/

#endif /* __AEEVECTORMODEL_H__ */
