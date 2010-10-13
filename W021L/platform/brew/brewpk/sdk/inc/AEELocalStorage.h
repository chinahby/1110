#ifndef AEELOCALSTORAGE_H
#define AEELOCALSTORAGE_H
/*======================================================

FILE:      AEELocalStorage.h

SERVICES:  ILocalStorage

DESCRIPTION:

    ILocalStorage provides an API to store an opaque 
     void pointer keyed by an AEECLSID.

=======================================================

      Copyright © 2005-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEQueryInterface.h"

#define AEEIID_LOCALSTORAGE 0x0102fa3e

typedef struct LSEntry {
   uint32 reserved[4];
} LSEntry;


#define INHERIT_ILocalStorage(iname) \
   INHERIT_IQueryInterface(iname); \
   void  (*Lock)(iname *p); \
   void  (*Unlock)(iname *p); \
   int   (*Get)(iname *p, uint32 dwKey, LSEntry **ppe); \
   int   (*Add)(iname *p, uint32 dwKey, LSEntry *pe); \
   void  (*Remove)(iname *p, LSEntry *pe)

// declare the actual LocalStorage interface
AEEINTERFACE_DEFINE(ILocalStorage);

static __inline uint32 ILOCALSTORAGE_AddRef(ILocalStorage* me)
{
   return AEEGETPVTBL(me,ILocalStorage)->AddRef(me);
}

static __inline uint32 ILOCALSTORAGE_Release(ILocalStorage* me)
{
   return AEEGETPVTBL(me,ILocalStorage)->Release(me);
}

static __inline int ILOCALSTORAGE_QueryInterface(ILocalStorage* me, 
                                                 AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,ILocalStorage)->QueryInterface(me, cls, ppo);
}

static __inline void ILOCALSTORAGE_Lock(ILocalStorage* me)
{
   AEEGETPVTBL(me,ILocalStorage)->Lock(me);
}

static __inline void ILOCALSTORAGE_Unlock(ILocalStorage* me)
{
   AEEGETPVTBL(me,ILocalStorage)->Unlock(me);
}

static __inline int ILOCALSTORAGE_Get(ILocalStorage* me, uint32 dwKey, 
                                      LSEntry** ppe)
{
   return AEEGETPVTBL(me,ILocalStorage)->Get(me, dwKey, ppe);
}

static __inline int ILOCALSTORAGE_Add(ILocalStorage* me, uint32 dwKey, 
                                      LSEntry* pe)
{
   return AEEGETPVTBL(me,ILocalStorage)->Add(me, dwKey, pe);
}

static __inline void ILOCALSTORAGE_Remove(ILocalStorage* me, LSEntry* pe)
{
   AEEGETPVTBL(me,ILocalStorage)->Remove(me, pe);
}

/*
==============================================================================
DATA STRUCTURES DOCUMENTATION
==============================================================================
LSEntry

Description:
   Opaque data space reserved for use by storage implementation.

Definition:

typedef struct LSEntry {
   uint32 reserved[4];
} LSEntry;

Members:
  reserved           - reserved

Comments:
  None

See Also:
  None

==============================================================================
*/

/*
=============================================================================
INTERFACES DOCUMENTATION
=============================================================================

ILocalStorage Interface

  ILocalStorage is an interface for generic storage. This interface enables
store and retrieve of opaque data buffers to an object.

  Users of this interface must reserve LSEntry space in each opaque data 
buffer to be stored in the storage. LSEntry space is used by implementation of
ILocalStorage to track the data pointers.

  ILocalStorage interface tracks saved entries by given key and address of      
LSEntry. When an entry is retrieved by the key corresponding address of LSEntry
is returned. Users can access beginning of their data buffer by subtracting
offset to LSEntry in data buffer.

  A simple use model is to declare LSEntry at the beginning of the data buffer.
Which simplifies data retrieval from address of LSEntry by casting to original 
data type.

===pre>
  eg : struct my_ls_data_type{
         LSEntry lse;
         ... ;  // other members of data buffer.
       };
===/pre>

  Users of the ILocalStorage object must identify appropriate key that prevents
collision with other users of the object. A recommended approach is to use an   
address from code space as a key. An address that is local to the consuming     
code as key will prevent collision with others. This could be an address of a
function.  Collision is not considered an error but only the last added is
returned during a call to get.  If the last added is removed then the second to
last is returned during a call to get, and so forth.

===pre>
eg : 

   static int save_to_ls(struct my_ls_data_type *pdata)
   {
      ...
      nErr = ILOCALSTORAGE_Add(piLocalStorage, (uint32)save_to_ls, 
                               &pdata->lse);
      ...
   }
===/pre>

=============================================================================

ILOCALSTORAGE_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ILOCALSTORAGE_Release()

=============================================================================

ILOCALSTORAGE_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ILOCALSTORAGE_AddRef()

=============================================================================

ILOCALSTORAGE_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

See Also:
    None

=============================================================================

ILOCALSTORAGE_Lock()

Description: 
   Lock the object for an exclusive access by current thread. Caller might 
block in this request until lock is available.

Prototype:
   void ILOCALSTORAGE_Lock(ILocalStorage *picd);

Parameters:
   picd : pointer to the ILocalStorage interface

Return value:
   None

Comments:  
   None.

See Also:
   ILOCALSTORAGE_Unlock()

=============================================================================

ILOCALSTORAGE_Unlock()

Description: 
   Unlock the object.

Prototype:
   void ILOCALSTORAGE_Lock(ILocalStorage *picd);

Parameters:
   picd : pointer to the ILocalStorage interface

Return value:
   None

Comments:  
   None.

See Also:
   ILOCALSTORAGE_Lock()

=============================================================================

ILOCALSTORAGE_Get()

Description: 
   Find and return the last added data pointer associated to the key that has
   not been removed.

Prototype:
   int ILOCALSTORAGE_Get(ILocalStorage *picd, uint32 dwKey, LSEntry** ppe);

Parameters:
   picd  : pointer to the ILocalStorage interface
   dwKey : a key against which the entry was previously added.
   ppe   : [out] pointer to be filled with the pointer to entry.

Return value:
   SUCCESS if data is found (has been set)
   ENOSUCH if data can't be found (has not been set, or has been removed)

Comments:  
   None

See Also:
   ILOCALSTORAGE_Add()
   ILOCALSTORAGE_Remove()

=============================================================================

ILOCALSTORAGE_Add()

Description: 
   Store a data pointer for the specified key value.  Multiple data pointers
   may be stored using the same key.  When adding multiple data pointes using
   the same key the data pointers stack.  Thus, when calling ILOCALSTORAGE_Get()
   the last data pointer added that has not been removed is returned.

Prototype:
   int ILOCALSTORAGE_Add(ILocalStorage *picd, uint32 dwKey, LSEntry* pe)

Parameters:
   picd  : pointer to the ILocalStorage interface
   dwKey : key value.
   pe    : pointer to local storage entry

Return value:
   SUCCESS if data is set
   EALREADY if key already exists.

Comments:
   
  Use an address from the code that uses the data buffer as the key.            

See Also:
   ILOCALSTORAGE_Get()
   ILOCALSTORAGE_Remove()

=============================================================================

ILOCALSTORAGE_Remove()

Description: 
   Remove a data pointer for the specified key.

Prototype:
   void ILOCALSTORAGE_Remove(ILocalStorage *picd, LSEntry* pe)

Parameters:
   picd  : pointer to the ILocalStorage interface
   pe    : pointer to local storage entry

Return value:
   None

Comments:  
   None.

See Also:
   ILOCALSTORAGE_Get()
   ILOCALSTORAGE_Set()

=============================================================================
*/

#endif /* #ifndef AEELOCALSTORAGE_H */

