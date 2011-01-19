#ifndef AEEIQI_H
#define AEEIQI_H /* #ifndef AEEIQI_H */
/*======================================================

Copyright © 2004-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/
#include "AEEStdDef.h"
#include "AEEInterface.h"

#define AEEIID_IQI 0x01000001

#define INHERIT_IQI(iname) \
   uint32 (*AddRef)(iname*);\
   uint32 (*Release)(iname*);\
   int    (*QueryInterface)(iname *, AEEIID, void **)

AEEINTERFACE_DEFINE(IQI);

static __inline uint32 IQI_AddRef(IQI *me)
{
   return AEEGETPVTBL(me,IQI)->AddRef(me);
}

static __inline uint32 IQI_Release(IQI *me)
{
   return AEEGETPVTBL(me,IQI)->Release(me);
}

static __inline int IQI_QueryInterface(IQI *me, AEEIID iid, void** ppo)
{
   return AEEGETPVTBL(me,IQI)->QueryInterface(me, iid, ppo);
}

#define IQI_RELEASEIF(p) do { if (p) { IQI* __releaseif__piqi = (IQI*)(p); (p) = 0; IQI_Release(__releaseif__piqi); } } while (0)

/*
=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================

IQI Interface

Description:
   IQI is the base level object class from which extensible
   API object classes should be derived.  It supplies a standard
   mechanism to accomplish object extensibility while maintaining
   binary compatibility.

=======================================================================

IQI_AddRef()

Description:

   This method increments the reference count on an object.  This
   allows the object to be shared by multiple callers.  The object 
   can be trusted to remain valid until a matching call to Release()
   is made.  AddRef() should be called whenever a reference to an object 
   is copied (e.g., when written to the output parameter to 
   QueryInterface())

Prototype:

   uint32 IQI_AddRef(IQI * po);

Parameters:
   po: Pointer to the object

Return Value:

   Returns the incremented reference count for the object.  A valid
   object returns a positive reference count.

Comments:
   None

Side Effects:
   None

See Also:
   IQI_Release()
=======================================================================
IQI_Release()

Description:

   This function decrements the reference count of an object.  This
   function should be called whenever a reference to an object 
   is destroyed (e.g. set to 0).

Prototype:

   uint32 IQI_Release(IQI * po);

Parameters:

   po: Pointer to the object

Return Value:

   Returns the decremented reference count for the object.

Comments:
   None

Side Effects:
   The object may be deleted during this call.  In any case, the 
    caller should assume that the object _has_ been freed.

See Also:
   IQI_AddRef()

=======================================================================
IQI_QueryInterface()

Description:
   This method asks an object for another API contract from the object in
   question.

Prototype:

   int IQI_QueryInterface(IQI *po, AEEIID idReq, void **ppo)

Parameters:
   po: Pointer to the object
   idReq:  Requested interface ID exposed by the object
   ppo: (out) Returned object.  Filled by this method

Return Value:
   AEE_SUCCESS - Interface found
   AEE_ENOMEMORY - Insufficient memory
   AEE_ECLASSNOTSUPPORT - Requested interface is unsupported
   Another AEEError.h, if appropriate

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
IQI_RELEASEIF()

Description:
   This is a helper macro for those conditionally releasing interfaces
     derived from IQI.

===pre>
Definition:
   IQI_RELEASEIF(p) do { if (p) { IQI_Release((IQI*)(p)); p = 0; } } while (0)

===/pre>
Parameters:
  p: the object to release, it *must* implement IQI_Release(), as this macro 
      does an unsafe cast

Evaluation Value:
   None

Comments:
   None

Side Effects:
   p is set to 0

See Also:
   IQI_Release()

=======================================================================

*/
#endif /* #ifndef AEEIQI_H */
