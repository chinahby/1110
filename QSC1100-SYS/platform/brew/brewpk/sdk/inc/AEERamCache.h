
/*===========================================================================

FILE: AEERamCache.h
 
SERVICES:  
    
   IRamCache
 
DESCRIPTION: 

   A simple, heap-based cache

PUBLIC CLASSES:
   
   IRamCache


INITIALIZATION AND SEQUENCING REQUIREMENTS:  
   
   Not Applicable


Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
#ifndef AEERAMCACHE_H
#define AEERAMCACHE_H /* #ifndef AEERAMCACHE_H */

#include "AEE.h"

/* ======================================================================= 
   Type Declarations
   ======================================================================= */

/*=====================================================================
  Function declarations and inline function definitions
  ===================================================================== */

#define IRAMCACHE_FLUSHALL     ((const char *)0)
#define IRAMCACHE_FLUSHEXPIRED ((const char *)1)

/* belongs in AEERecordStore.h, eventually */
#define INHERIT_IRecordStore(iname)\
   INHERIT_IQueryInterface(iname);\
   int (*Add)(iname *po, \
              const char *pKey, int nKeyLen,\
              const char *pVal, int nValLen);\
   int (*Remove)(iname *po, const char *pKey, int nKeyLen);\
   int (*Find)(iname *po, \
               const char *pKey, int nKeyLen,\
               char *pVal, int *pnValLen)

typedef struct IRecordStore IRecordStore;

AEEINTERFACE(IRecordStore) {
    INHERIT_IRecordStore(IRecordStore);  
};

#define IRECORDSTORE_AddRef(p)             AEEGETPVTBL((p),IRecordStore)->AddRef((p))
#define IRECORDSTORE_Release(p)            AEEGETPVTBL((p),IRecordStore)->Release((p))
#define IRECORDSTORE_QueryInterface(p,i,o) AEEGETPVTBL((p),IRecordStore)->QueryInterface((p),(i),(o))
#define IRECORDSTORE_Add(p,k,kl,v,vl)      AEEGETPVTBL((p),IRecordStore)->Add((p),(k),(kl),(v),(vl))
#define IRECORDSTORE_Remove(p,k,kl)        AEEGETPVTBL((p),IRecordStore)->Remove((p),(k),(kl))
#define IRECORDSTORE_Find(p,k,kl,v,vl)     AEEGETPVTBL((p),IRecordStore)->Find((p),(k),(kl),(v),(vl))


/* belongs in AEECache.h, eventually */
#define INHERIT_ICache(iname)\
   INHERIT_IRecordStore(iname);\
   uint32 (*SetTTL)(iname *, uint32 ulTTL)

typedef struct IRamCache IRamCache;

AEEINTERFACE(IRamCache) {
   INHERIT_ICache(IRamCache);
   int (*SetMaxSize)(IRamCache *pirc, int nSize);
   int (*SetMaxEntries)(IRamCache *pirc, int nNum);
};


#define IRAMCACHE_AddRef(p)             AEEGETPVTBL((p),IRamCache)->AddRef((p))
#define IRAMCACHE_Release(p)            AEEGETPVTBL((p),IRamCache)->Release((p))
#define IRAMCACHE_QueryInterface(p,i,o) AEEGETPVTBL((p),IRamCache)->QueryInterface((p),(i),(o))
#define IRAMCACHE_Add(p,k,kl,v,vl)      AEEGETPVTBL((p),IRamCache)->Add((p),(k),(kl),(v),(vl))
#define IRAMCACHE_Remove(p,k,kl)        AEEGETPVTBL((p),IRamCache)->Remove((p),(k),(kl))
#define IRAMCACHE_Find(p,k,kl,v,vl)     AEEGETPVTBL((p),IRamCache)->Find((p),(k),(kl),(v),(vl))
#define IRAMCACHE_SetTTL(p,t)           AEEGETPVTBL((p),IRamCache)->SetTTL((p),(t))
#define IRAMCACHE_SetMaxSize(p,s)       AEEGETPVTBL((p),IRamCache)->SetMaxSize((p),(s))
#define IRAMCACHE_SetMaxEntries(p,n)    AEEGETPVTBL((p),IRamCache)->SetMaxEntries((p),(n))
#define IRAMCACHE_FlushAll(p)           AEEGETPVTBL((p),IRamCache)->Remove((p),IRAMCACHE_FLUSHALL,-1)
#define IRAMCACHE_FlushExpired(p)       AEEGETPVTBL((p),IRamCache)->Remove((p),IRAMCACHE_FLUSHEXPIRED,-1)

/* =======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

IRecordStore Interface

Description: 
   IRecordStore is an interface to access a simple key-value store
   of records. There is no implementation of IRecordStore. It is 
   basically an abstract base class from which implementations 
   inherit.
  
   Records consist of a key-value pair, each of which can be a binary 
   blob.  Keys must be unique as they are used to locate the records.

   The class that actually implements the record store defines storage
   semantics such as limits on the amount of data that may be stored,
   the life time of the storage and other.

The following header file is required:

   AEERamCache.h

See Also:
   IRamCache Interface

======================================================================= 

IRECORDSTORE_AddRef()

Description:
	This function is inherited from IBASE_AddRef().

======================================================================= 
IRECORDSTORE_Release()

Description:
	This function is inherited from IBASE_Release().
	
======================================================================= 

IRECORDSTORE_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface().
	
======================================================================= 

IRECORDSTORE_Add()

Description: 
      This function adds a record to the store.

Prototype:

   int  IRECORDSTORE_Add(IRecordStore *pIRecordStore, 
                         const char *pKey, int nKeyLen,
                         const char *pVal, int nValLen);

Parameters:

   pIRecordStore:  pointer to the IRecordStore interface

   pKey:  the key data

   nKeyLen:  the length of the key data

   pVal:  the value data

   nValLen:  the length of the value data

Return Value:  
   SUCCESS: if the record is added to the store~
   ENOMEMORY: if the record could not be created to be added

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.1

See Also:
   None

======================================================================= 

IRECORDSTORE_Remove()

Description: 
      This function removes a record from the store

Prototype:

    int  IRECORDSTORE_Remove(IRecordStore *pIRecordStore, 
                             const char *pKey, int nKeyLen);

Parameters:

   pIRecordStore:  pointer to the IRecordStore interface

   pKey:  the key data

   nKeyLen:  the length of the key data

Return Value:  
   SUCCESS: if the record was found and removed~
   EFAILED: if the record is not in the store
    
Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.1

See Also:
   None

======================================================================= 

IRECORDSTORE_Find()

Description: 
      This function finds a record in the cache

Prototype:

   int  IRECORDSTORE_Find(IRecordStore *pIRecordStore, 
                          const char *pKey, int nKeyLen,
                          char *pVal, int *pnValLen)

Parameters:

   pIRecordStore:  pointer to the IRecordStore interface

   pKey:  the key data

   nKeyLen:  the length of the key data

   pVal:  [out] the value data

   pnValLen: [in/out] size of pVal, if pVal is NULL, pnValLen is
              ignored as input, on output, holds the size of the data
              in the store (if any)

Return Value:  
   SUCCESS: if the record was found~
   EFAILED: if the record is not in the store
    
Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.1

See Also:
   None

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

IRamCache Interface

Description:  
   IRamCache provides a size-limited, item-limited LRU cache with TTL, 
   where records are stored on the heap.  The stock BREW implementation of
   IRamCache is a singly-linked list.

   Records consist of a key-value pair, each of which can be a binary 
   blob.  Keys and values are copied.

   IRamCache inherits from IRecordStore.

The following header file is required:

   AEERamCache.h

See Also:
   IRecordStore Interface

======================================================================= 

IRAMCACHE_AddRef()

Description:
	This function is  inherited from IBASE_AddRef().

======================================================================= 
IRAMCACHE_Release()

Description:
	This function is  inherited from IBASE_Release().

======================================================================= 

IRAMCACHE_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface().

======================================================================= 



IRAMCACHE_Add()

Description: 
      This function adds a record to the cache.

Prototype:

    int  IRAMCACHE_Add(IRamCache *pIRamCache, 
                       const char *pKey, int nKeyLen,
                       const char *pVal, int nValLen);

Parameters:

   pIRamCache: the IRamCache interface pointer

   pKey:  the key data

   nKeyLen:  the length of the key data

   pVal:  the value data

   nValLen:  the length of the value data

Return Value:  
   SUCCESS: if the record is added to the cache~
   EFAILED: if the add would violate the constraints of the cache 
            (exceeds maximum size or if maximum entries is 0)~
   ENOMEMORY: if the record could not be created to be added
    
Comments:
   The contents of pKey and pVal are copied by the IRamCache, and may
   be discarded after the call.

   If the cache's TTL is set to zero, Add() will "succeed", but the 
    record will be immediately discarded.
    
   This function is inhereted from IRecoredStore_Add().

Side Effects:
  Expired records are purged~
  The added record is first in the LRU list

Version:
   Introduced BREW Client 2.0

See Also:
   IRECORDSTORE_Add()


======================================================================= 

IRAMCACHE_Remove()

Description: 
      This function removes a record from the cache.

Prototype:

    int  IRAMCACHE_Remove(IRamCache *pIRamCache, 
                          onst char *pKey, int nKeyLen);

Parameters:

   pIRamCache: the IRamCache interface pointer

   pKey:  the key data

   nKeyLen:  the length of the key data

Return Value:  
   SUCCESS: if the record was found and removed~
   EFAILED: if the record is not in the cache
    
Comments:
   This function is inhereted from IRecoredStore.

Side Effects:
   expired records are purged

Version:
   Introduced BREW Client 2.0

See Also:
   IRECORDSTORE_Remove()

======================================================================= 

IRAMCACHE_Find()

Description: 
      This function finds a record in the cache

Prototype:

   int  IRAMCACHE_Find(IRamCache *pIRamCache, const char *pKey, int nKeyLen,
                       char *pVal, int *pnValLen)

Parameters:

   pIRamCache: the IRamCache interface pointer

   pKey: the key data

   nKeyLen: the length of the key data

   pVal: [out] the value data

   pnValLen: [in/out] size of pVal, if pVal is NULL, pnValLen is
               ignored as input, on output, holds the size of the data
               in the cache (if any)

Return Value:  
   SUCCESS: if the record was found~
   EFAILED: if the record is not in the cache
    
Comments:
   This function is inhereted from IRecoredStore.
   
   Find() for an expired record will fail.  Find() non-expired record will put 
      the record in MRU position.

Side Effects:
   expired records are purged

Version:
   Introduced BREW Client 2.0

See Also:
   IRECORDSTORE_Find()

======================================================================= 

IRAMCACHE_SetTTL()

Description: 
     This function sets the TTL for records in the cache, in milliseconds, 
    applies only to subsequently Add()ed records

Prototype:

    uint32 IRAMCACHE_SetTTL(IRamCache *pIRamCache, uint32 ulTTL)

Parameters:

   pIRamCache: the IRamCache interface pointer

   ulTTL:  the new TTL, in milliseconds

Return Value:  
   the old TTL value
    
Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

======================================================================= 

IRAMCACHE_SetMaxSize()

Description: 
      This function sets the maximum size (in bytes) that the RamCache may consume 
    to hold keys and values

Prototype:

    int IRAMCACHE_SetMaxSize(IRamCache *pIRamCache, int nSize);

Parameters:

   pIRamCache: the IRamCache interface pointer

   nSize:  the new size, in bytes

Return Value:  
   the old max size
    
Comments:
   the overhead of storing the keys and values is not included in this 
    number, this number restricts only key and value bytes are counted

Side Effects:
   the cache adjusts to its new constraint, LRU entries and expired 
    entries are removed from the cache

Version:
   Introduced BREW Client 2.0

See Also:
   None

======================================================================= 

IRAMCACHE_SetMaxEntries()

Description: 
   This function sets the maximum number of entries allowed in the cache

Prototype:
   
    int IRAMCACHE_SetMaxEntries(IRamCache *pIRamCache, int nNum);

Parameters:

   pIRamCache: the IRamCache interface pointer

   nNum:  the new entries limit

Return Value:  
   the old enties limit
    
Comments:
   None

Side Effects:
   the cache adjusts to its new constraint, LRU entries and expired 
    entries are removed from the cache

Version:
   Introduced BREW Client 2.0

See Also:
   None

======================================================================= 

IRAMCACHE_FlushAll()

Description: 
      This function empties the cache, all records are removed

Prototype:
   
    int IRAMCACHE_FlushAll(IRamCache *pIRamCache);

Parameters:

   pIRamCache: the IRamCache interface pointer


Return Value:  
   SUCCESS: if task is successful
    
Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

======================================================================= 

IRAMCACHE_FlushExpired()

Description: 
   This function runs expire on the cache. All expired records are removed,
     freeing up the memory used to hold them.

Prototype:
   
    int IRAMCACHE_FlushExpired(IRamCache *pIRamCache);

Parameters:

   pIRamCache: the IRamCache interface pointer

Return Value: 
   SUCCESS: if task is successful
    
Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

   ======================================================================= */


#endif /* #ifndef AEERAMCACHE_H */
