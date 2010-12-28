#ifndef AEEIParameters1_H
#define AEEIParameters1_H
/*==============================================================================

FILE:        AEEIParameters1.h

SERVICES:    IParameters1

DESCRIPTION: This defines a standard interface for getting and setting parameters
             on an object. Interfaces that involve configuration parameters
             can inherit from this. Objects that involve configuration
             parameters can provide them via this interface. This interface is
             simpler and easier to use than IxOpts. If used very carefully and
             in a limited way, it is suitable for remoting/marshalling across
             protection and processor boundaries.
             
             This interface replaces IParametersRO and IParameters.
          

===============================================================================
        Copyright © 2006-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "brew/AEEIQI.h"

#define AEEIID_IParameters1 (0x0104047f)

#define INHERIT_IParameters1(iname) \
   INHERIT_IQI(iname); \
   int (*GetParam)(iname* me, int nId, void *pParamBuf, int nParamBufLen, int *pnParamLen); \
   int (*SetParam)(iname* me, int nId, const void *pParam, int nParamLen)

AEEINTERFACE_DEFINE(IParameters1);


static __inline uint32 IParameters1_AddRef(IParameters1 *me)
{
   return AEEGETPVTBL(me,IParameters1)->AddRef(me);
}

static __inline uint32 IParameters1_Release(IParameters1 *me)
{
   return AEEGETPVTBL(me,IParameters1)->Release(me);
}

static __inline int IParameters1_QueryInterface(IParameters1 *me,
                                               AEEIID iid, void** ppo)
{
   return AEEGETPVTBL(me,IParameters1)->QueryInterface(me, iid, ppo);
}

static __inline int IParameters1_GetParam(IParameters1 *me,
                                         int nId, 
                                         void *pParamBuf,
                                         int nParamBufLen,
                                         int *pnParamLen)
{
   return AEEGETPVTBL(me,IParameters1)->GetParam(me, nId, pParamBuf, nParamBufLen, pnParamLen);
}

static __inline int IParameters1_SetParam(IParameters1 *me,
                                         int nId, const void *pParam,
                                         int nParamLen)
{
   return AEEGETPVTBL(me,IParameters1)->SetParam(me, nId, pParam, nParamLen);
}


/*
===============================================================================
 INTERFACES DOCUMENTATION
===============================================================================
IParameters1 Interface

This defines an interface for accessing and setting a small number of 
small configuration parameters or data items.  It is an abstract interface 
intended primarily to be inherited by other interfaces needing this 
functionality.  It is preferable to use this interface rather than 
inventing a new one, for general interface consistency and because there 
are some utility classes that are useful with this way of handling 
parameters.  

This interface is good for simple, single valued, and untyped parameters.
Consider using the IxOpts interface when a richer interface is needed. With 
IxOpts, parameters can have multiple values, they have data types and 
collections can be aggregated.

This interface replaces IParametersRO and IParameters. This interface is 
better because it is less vulnerable to mistakes that result in buffer 
overruns, it uses int rather than unsigned, and because the usage conventions
are clearly documented.

This interface deliberately uses int for the lengths rather than unsigned,
short or other. Shorts as function parameters are always promoted to int in C.
Unsigned can lead to pointer arithmetic overflow easier than int and is generally 
more trouble. Unsigned is discontinuous in underflow in a disastrous way. It is
also better to stay with one type rather than mixing signed and unsigned.

This interface is less vulnerable to buffer overruns because the pointer and
length passed are very explicit and next to each other. IParameters had the
length passed in and out with a pointer to an integer (an [in/out] C 
parameter); it was easy to forget to set the length, and there was
typically no error flagged when it was forgotten. When it was forgotten
the length in was what was in the uninitialized stack variable, typically a
number larger than the size of the buffer, thus a buffer overrun 
vulnerability was introduced.


===============================================================================

IParameters1_AddRef()

Description:
   This function is inherited from IQI_AddRef().

===============================================================================

IParameters1_Release()

Description:
   This function is inherited from IQI_Release().

===============================================================================

IParameters1_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

===============================================================================

IParameters1_GetParam()

Description:
   This method copies parameter's value to the caller's buffer

Prototype:
   IParameters1_GetParam(IParameters1   *me,
                         int             nId, 
                         void           *pParamBuf,
                         int             nParamBufLen,
                         int            *pnParamLen)

Parameters:
   me:          [in]  The interface pointer
   nId:         [in]  ID of the particular parameter to get
   pParamBuf:   [out] Pointer to buffer to which parameter is copied
   nParamBufLen [in]  The length of pParamBuf
   pnParamLen:  [out] Pointer to the full length of the parameter data

Return value:
   AEE_SUCCESS: if data is copied
   AEE_ENOSUCH: if the parameter ID is not known~
   Other non-success error codes if no data is copied

Comments:
   [For The Caller]
   
   The basic rules can be stated in a simple and straight forward way. If no
   errors prevail, the length of the parameter is returned in *pnParamLen,
   and as much of its value is copied into pParamBuf as will fit.
   AEE_SUCCESS is returned.

   If there is some error, then an error code is returned, no data is copied,
   and *pnParamLen is not filled in. 
   
   If the buffer is too small to hold the full parameter, what will fit into
   the buffer is always copied. The partial copy will always completely fill 
   the given buffer. The length of the full parameter is returned in
   *pnParamLen (if non-null). AEE_SUCCESS is returned.  Because there is 
   no error code indicating a partial copy was performed it is sometimes 
   important to check the that the required amount of data was copied.

   For some parameter IDs, the length of the data is entirely fixed and
   well known. Its length is clearly documented. In this case the caller
   can often avoid dynamic allocation of the buffer and pass NULL for 
   pnParamLen. Note that if a buffer that is too small is passed, partial
   copying still occurs. In some cases the caller may actually desire to 
   only retrieve a partial result. 

   For other parameter IDs the length of the data varies. The caller may
   pass a fixed buffer if they know the maximum length or if they only need
   a known fixed first part of the parameter, but generally dynamic 
   allocation of the buffer is used because the full parameter is required
   and the maximum size is not fixed.

   One possible calling implementation is to first call 
   IParameters1_GetParam(), passing a zero length buffer to have the full
   length of the parameter returned in *pnParamLen. Second, allocate a buffer
   of that size and call IParameters1_GetParam() again to have the full
   parameter copied. A safer and better implementation calls 
   IParameters1_GetParam() in a loop, realloc'ing the buffer to be larger 
   until the size of the buffer passed is equal to or greater then the size
   of the actual parameter value returned.  Using the loop handles the case
   where the size of the parameter might vary from one call to the other
   because the underlying data is varying. 
   
   The length of the data copied is always MIN(nParamBufLen, *pnParamLen) 
   whether a full or partial copy was performed and whether the size is
   fixed or varies. The length copied is the smaller of the buffer size 
   passed and the full length of the parameter. 

   For some clearly documented parameter IDs, it is safe to ignore the return
   value and/or the returned length. If the documentation indicates that
   AEE_SUCCESS is always returned, the return value can be ignored.  If the
   documentation indicates a fixed amount of data is always returned or the
   caller only needs partial results, the returned length can be ignored.

   If nParamBufLen is less than or equal to zero, then no data is copied and 
   pParamBuf is ignored and may be NULL. If nParamBufLen is not 0, copying 
   is attempted and pParamBuf must point to a buffer of at least the given
   length.  No checking that pParamBuf is NULL is done. This is up to the
   caller.

   The size of the full parameter is always returned in *pnParamLen even if
   nParamBufLen is 0 and no copying is done. 

   If type of the parameter data is a simple integer, then this method is
   usually called with pParamBuf pointing to an integer variable. nParamBufLen
   is usually sizeof(int) in this case.

   If the data being returned is a structure then pParamBuf points to a
   structure into which the data is copied. nParamBufLen should be at 
   least the size of the structure in bytes.

   If the data being returned is a NUL terminated string, then pParamBuf points
   to a buffer believed to be large enough to hold the string including the
   NUL. The length returned in pnParamLen is the length of the string plus the
   NUL. It is NOT the length of the string. Some implementations may return
   a partial result if the buffer is too small, but they shall always add a
   NUL terminator if they are described as returning NUL terminated strings.

   In some cases the data being returned is a pointer to a an internal
   buffer rather than the data itself. This is allowed but discouraged. 
   It is not allowed if the interface is to be remoted or marshalled.
   When this is done, the length in and out is the length of the pointer, not
   the data. This is consistent with the behavior described above because the
   pointer is the data that is copied. If the length is to be provided to the
   caller in this situation, it should be with a different parameter ID. The 
   implementation must say what the lifetime of the pointer returned is.

   If the data being returned is a variable length binary buffer, then 
   pParamBuf points to the buffer into which it should be copied. The
   lengths are in bytes.

   Some implementations may return a number of fixed size records such as
   an array of structures. The length of the buffer passed in and of the
   parameter returned is always in bytes. The size of the records are
   documented for the particular parameter ID.

   [For the implementor]
   The basic implementation rules can be stated in a simple and straight
   forward way. If the data is available and no error conditions prevail
   AEE_SUCCESS is returned, the full length of the data is returned in
   *pnParamLen, and as much of the data that will fit in the buffer provided
   by the caller is copied. If the data is not available, then an error code
   is returned, no length is returned and no copying is done.

   When copying the data, either the buffer passed in must be filled completely,
   or if the buffer is larger than the parameter, the full parameter must be
   copied. If this rule is not followed, the caller can't know the amount of
   data copied. Following this rule may lead to situations where seemingly
   meaningless amounts of data is copied, such as 3 bytes of a 4 byte integer, 
   but the rule must still be followed.

   The exact length of the data must always be returned in *pnParamLen, 
   when pnParam is not NULL. In the case where the length of the data has to
   be calculated and the buffer given is too small to hold the data, it may be
   convenient to return an easy-to-calculate length and that is slightly larger
   than the data. It may also be convenient to return incompletely formatted or
   computed data as the partial result. This is explicitly not allowed so as
   not to presume a particular way of calling this method and to be entirely
   consistent with the returned parameter data and its length.

   It is not permitted to return an error such as BUFFERTOOSMALL and fill in
   *pnParamLen. If the buffer is too small, partial data shall be copied,
   the correct length filled in and AEE_SUCCESS returned. It is up to the 
   caller to figure out they got partial data. The general rule is that 
   if an error is returned, no copying is done and *pnParamLen shall not be set.
   Conversely, if AEE_SUCCESS is returned the length shall be returned correctly
   and as much of the parameter as fits in the buffer shall be copied.

   There is no need to ever check pParamBuf for NULL, only to not do any
   copying when nParamBufLen is zero. It is OK if the call to this function
   crashes when NULL is passed for pParamBuf.

   It is legal for pnParamLen to be NULL.  In this case the length of the 
   parameter is not returned, nor is any error ever returned. Negative values
   shall never be returned for the length of the parameter data. It is also
   not an error if the caller passed in a zero length buffer. If nParamBufLen
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

   The documentation for each parameter ID should say what the type
   of the data is. On request for an invalid parameter AEE_NOSUCH should
   be returned. AEE_BADPARAM is generally never returned.

   If the data for a particular parameter is always available, for example
   if it is a fixed constant, then the documentation may say that AEE_SUCCESS
   is always returned to allow the caller to ignore the return value.

   If the length of the data for a particular parameter ID is entirely
   fixed and never changes, the implementor should clearly document this 
   to save the caller from having to do dynamic allocation and length checking.   

   Implementations that return NUL terminated strings must always include
   a terminating NUL even if a partial value is copied because the buffer
   was too small.

   If the parameter is an aligned data type such as an int, it should not be
   presumed that the caller will pass an aligned pointer. The implementation
   should not do a cast and assignment to copy the data. A good way to 
   copy data into the buffer regardless of data type, given the data is in pVal
   and the length is nValLen is:~
===pre>
        memmove(pParamBuf, pVal, MIN(nValLen, nParamBufLen));
===/pre>

   While this interface has been designed with remoting and marshalling in mind,
   the rules given so far are insufficient. Certain data types, pointers in 
   particular, cannot be used if the use of this interface is to be remotable.
   While pointers are clearly the main problem, other data types can also
   pose a problem. Structures must be used with care as they are often 
   compiler dependent and the host and service may be have been compiled 
   differently. No rules are given here that will guarantee remotability. It
   is up to the implementor using this interface to use it in a way that 
   is remotable. 
   
See Also:
   IParameters1_SetParam

===============================================================================

IParameters1_SetParam()

Description:
   This method sets a parameter value

Prototype:
   IParameters1_SetParam(IParameters1 *me,
                        int nId, const void *pParam,
                        int nParamLen)

Parameters:
   me: The interface pointer
   nId :       [in] the parameter to set
   pParam :    [in] points to data / buffer to set
   nParamLen : [in] size of the data pointed to by pParam

Return value:
   AEE_SUCCESS: if the data is copied
   AEE_ENOSUCH: if the parameter ID is not known
   AEE_EREADONLY:  can get parameter, but not set it
   AEE_ENOMEMORY:  can't allocate heap memory to store param
   AEE_EUNSUPPORTED: if SetParam is not supported for this object
   
   Other return values per implementation or parameter type

Comments:
   The standard behavior is to copy the data pointed to by pParam. The number
   of bytes given in nParamLen is copied. The caller can assume the data
   has been copied and doesn't have to manage the lifetime of the data any
   further.

   Exceptions to this rule are allowed if clearly documented.

   One standard exception is the passing of a ref counted object. pParam points
   to the object. uParamLen is the length of the pointer to the object. When
   values of this type are set, the SetParam implementation is responsible for 
   increasing the reference count by calling AddRef on the object.

   Another exception is to copy only the pointer and not the data. Here the caller
   must manage the lifetime of the pointer explicitly. In this case the length
   in uParamLen is the size of the data pointed to.  This must be clearly
   documented.
   
   If the object does not have any settable parameters, this function
   returns the error status AEE_UNSUPPORTED.

See Also:
   IParameters1_GetParam()
===============================================================================

*/

#endif /* #ifndef AEEIParameters1_H */
