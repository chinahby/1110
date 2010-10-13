#ifndef AEEIITEMSTORE_H
#define AEEIITEMSTORE_H

/*=============================================================================

FILE:         AEEIItemStore.h

SERVICES:     IItemStore interface

DESCRIPTION:  This file defines the IItemStore interface that allows BREW 
              applications to get, set and be notified of changes to 
              string based items.  The item and its possible values are defined 
              by the classid that implements the IItemStore interface.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEIQI.h"
#include "AEEISignal.h"


/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/


/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/


//*****************************************************************************
//
// IItemStore Interface
//
//*****************************************************************************

#define AEEIID_IItemStore    0x01060f39  

#define INHERIT_IItemStore(iname) \
   INHERIT_IQI(iname); \
   int (*Get)(iname * po, char *pszItemBuf, int nItemLen, int *pnItemLenReq); \
   int (*Set)(iname * po, const char *pItem);\
   int (*OnChange)(iname * po, ISignal *piSignal)

AEEINTERFACE_DEFINE(IItemStore);


//////////////////////
// IItemStore methods
//////////////////////

static __inline uint32 IItemStore_AddRef(IItemStore *me)
{
   return AEEGETPVTBL(me,IItemStore)->AddRef(me);
}

static __inline uint32 IItemStore_Release(IItemStore *me)
{
   return AEEGETPVTBL(me,IItemStore)->Release(me);
}

static __inline int IItemStore_QueryInterface(IItemStore *me, 
                                             AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IItemStore)->QueryInterface(me, cls, ppo);
}

static __inline int IItemStore_Get(IItemStore *me, 
                                   char *pszItemBuf, 
                                   int nItemLen,
                                   int *pnItemLenReq)
{
   return AEEGETPVTBL(me,IItemStore)->Get(me, pszItemBuf, nItemLen, pnItemLenReq);
}

static __inline int IItemStore_Set(IItemStore *me, const char *pszItem)
{
   return AEEGETPVTBL(me,IItemStore)->Set(me, pszItem);
}


static __inline int IItemStore_OnChange(IItemStore *me, ISignal *piSignal) 
{
   return AEEGETPVTBL(me,IItemStore)->OnChange(me, piSignal);
}

/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================


=============================================================================
*/

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

IItemStore Interface

Description:
   IItemStore interface that allows BREW applications to get, set and be 
   notified of changes to string based items. The item and its possible 
   values is defined by the classid that implements the IItemStore interface.

   The following header file is required:~
   AEEIItemStore.h

Version:
   Introduced BREW Client 3.1.5SP01

=============================================================================

IItemStore_AddRef()

Description:
   This function is inherited from IQI_AddRef(). 

See Also:
   IItemStore_Release()

=============================================================================

IItemStore_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface(). 

See Also:
   IQI_QueryInterface()

=============================================================================

IItemStore_Release()

Description:
   This function is inherited from IQI_Release(). 

See Also:
   IItemStore_AddRef()

=============================================================================

IItemStore_Get

Description:  
   This method gets the value of the specified item.

Prototype:
   int IItemStore_Get(IItemStore * po, char *pszItemBuf, int nItemLen, int *pnItemLenReq)

Parameters:
   po             [in]  Pointer to IItemStore interface
   pszItemBuf     [out] Pointer to buffer to which item is copied.
   pnItemLen      [in]  The size of pszItemBuf in bytes.
   pnItemLenReq   [out] Pointer to the full length of the item

Return Value:
   AEE_SUCCESS on success,
   Other non-success error codes if no data is copied

Comments:
   [For The Caller]
   
   If no errors prevail, the length of the item is returned in *pnItemLenReq,
   and as much of its value is copied into pszItemBuf as will fit.
   AEE_SUCCESS is returned.

   If there is some error, then an error code is returned, no data is copied,
   and *pnItemLenReq is not filled in. 
   
   If the buffer is too small to hold the full item string, what will fit into
   the buffer is always copied. The partial copy will always completely fill 
   the given buffer. The length of the full item string is returned in
   *pnItemLenReq (if non-null). AEE_SUCCESS is returned.  Because there is 
   no error code indicating a partial copy was performed it is sometimes 
   important to check the that the required amount of data was copied.

   For some items, the length of the data is entirely fixed and
   well known. Its length is clearly documented. In this case the caller
   can often avoid dynamic allocation of the buffer and pass NULL for 
   pnItemLenReq. Note that if a buffer that is too small is passed, partial
   copying still occurs. In some cases the caller may actually desire to 
   only retrieve a partial result. 

   For other items the length of the data varies. The caller may
   pass a fixed buffer if they know the maximum length or if they only need
   a known fixed first part of the item string, but generally dynamic 
   allocation of the buffer is used because the full item string is required
   and the maximum size is not fixed.

   One possible calling implementation is to first call 
   IItemStore_Get(), passing a zero length buffer to have the full
   length of the item string returned in *pnItemLenReq. Second, allocate a buffer
   of that size and call IItemStore_Get() again to have the full
   item copied. A safer and better implementation calls IItemStore_Get() 
   in a loop, realloc'ing the buffer to be larger until the size of the buffer 
   passed is equal to or greater then the size of the actual item value 
   returned.  Using the loop handles the case where the size of the string 
   might vary from one call to the other because the underlying data is varying. 
   
   The length of the data copied is always MIN(nItemLen, *pnItemLenReq) 
   whether a full or partial copy was performed and whether the size is
   fixed or varies. The length copied is the smaller of the buffer size 
   passed and the full length of the item. 

   If nItemLen is less than or equal to zero, then no data is copied and 
   pszItemBuf is ignored and may be NULL. If nItemLen is not 0, copying 
   is attempted and pszItemBuf must point to a buffer of at least the given
   length.  No checking that pszItemBuf is NULL is done. This is up to the
   caller.

   The size of the full item is always returned in *pnItemLenReq even if
   nItemLen is 0 and no copying is done. 

   The data being returned is always a NUL terminated string, and pszItemBuf 
   points to a buffer believed to be large enough to hold the string including the
   NUL. The length returned in pnItemLenReq is the length of the string plus the
   NUL. It is NOT the length of the string. Some implementations may return
   a partial result if the buffer is too small, but they shall always add a
   NUL terminator if they are described as returning NUL terminated strings.

   [For the implementor]
   The basic implementation rules can be stated in a simple and straight
   forward way. If the data is available and no error conditions prevail
   AEE_SUCCESS is returned, the full length of the data is returned in
   *pnItemLenReq, and as much of the data that will fit in the buffer provided
   by the caller is copied. If the data is not available, then an error code
   is returned, no length is returned and no copying is done.

   When copying the data, either the buffer passed in must be filled completely,
   or if the buffer is larger than the item string, the full item string must be
   copied. If this rule is not followed, the caller can't know the amount of
   data copied. Following this rule may lead to situations where seemingly
   meaningless amounts of data is copied, but the rule must still be followed.

   The exact length of the data must always be returned in *pnItemLenReq, 
   when pnItem is not NULL. In the case where the length of the data has to
   be calculated and the buffer given is too small to hold the data, it may be
   convenient to return an easy-to-calculate length and that is slightly larger
   than the data. It may also be convenient to return incompletely formatted or
   computed data as the partial result. This is explicitly not allowed so as
   not to presume a particular way of calling this method and to be entirely
   consistent with the returned item data and its length.

   It is not permitted to return an error such as BUFFERTOOSMALL and fill in
   *pnItemLenReq. If the buffer is too small, partial data shall be copied,
   the correct length filled in and AEE_SUCCESS returned. It is up to the 
   caller to figure out they got partial data. The general rule is that 
   if an error is returned, no copying is done and *pnItemLenReq shall not be set.
   Conversely, if AEE_SUCCESS is returned the length shall be returned correctly
   and as much of the item string as fits in the buffer shall be copied.

   There is no need to ever check pszItemBuf for NULL, only to not do any
   copying when nItemLen is zero. It is OK if the call to this function
   crashes when NULL is passed for pszItemBuf.

   It is legal for pnItemLenReq to be NULL.  In this case the length of the 
   item is not returned, nor is any error ever returned. Negative values
   shall never be returned for the length of the item data. It is also
   not an error if the caller passed in a zero length buffer. If nItemLen
   is negative, the buffer is considered zero length and no error returned.

   These rules lead to a few situations that are, or may seem, awkward for 
   the implementor. They are, however, constructed to balance complexity and
   consistency over a number of use cases for both the implementor and
   the caller and must be followed to conform to this interface.  

   It is allowed that the length of the data varies from one call to the next
   as the underlying data being returned may be varying. The length returned
   however should always be the length of the actual data. If the data 
   being returned varies continuously, this interface may not be suitable. This
   interface should not be used for any sort of a "read" interface. 

   The documentation for each classid that implements this interface should describe 
   the possible values for the item. 

   Implementations that return NUL terminated strings must always include
   a terminating NUL even if a partial value is copied because the buffer
   was too small. 
   
   A good way to copy data into the buffer regardless of data type, given the data 
   is in pVal and the length is nValLen is:~
===pre>
        memmove(pszItemBuf, pVal, MIN(nValLen, nItemLen));
===/pre>

Side Effects:
   None

See Also:
   IItemStore_Set()

===============================================================================
   
IItemStore_Set

Description:  
   This method sets the value of the item to the specified string.  

Prototype:
   int IItemStore_Set(IItemStore * po, const char *pszItem)

Parameters:
   po       [in]  Pointer to IItemStore interface
   pszItem  [in]  Item Value

Return Value:
   AEE_SUCCESS: if the data is copied
   AEE_ENOSUCH: if the value of the item is not known
   AEE_EREADONLY:  can get item string, but not set it
   AEE_ENOMEMORY:  can't allocate heap memory to store item
   AEE_EUNSUPPORTED: if Set is not supported for this object

   Other return values indicating an error, depending on the implementation or item

Comments:
   The standard behavior is to copy the data pointed to by pItem. The caller can 
   assume the data has been copied and doesn't have to manage the lifetime of the 
   data any further.

   Exceptions to this rule are allowed if clearly documented.
   
   If the object is not a settable item, this function returns the error 
   status AEE_EUNSUPPORTED.

Side Effects:
   None

See Also:
   IItemStore_Get()
   IItemStore_OnChange()

===============================================================================
   
IItemStore_OnChange

Description:
   This function registers a signal which is set whenever the items changes.

Prototype:
   int IItemStore_OnChange(IItemStore * po, ISignal *piSignal)

Parameters:
   po       [in] Pointer to IItemStore interface
   piSignal [in] Pointer to signal which will be set when the item changes

Return Value:
   AEE_SUCCESS: if the data is copied
   Other return values indicating an error, depending on the implementation or item

Comments:
   None

Side Effects:
   None

See Also:
   IItemStore_Set()

===============================================================================
*/

#endif /* #ifndef AEEIITEMSTORE_H */
