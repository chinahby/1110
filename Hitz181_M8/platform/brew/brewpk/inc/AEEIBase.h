#ifndef AEEIBASE_H
#define AEEIBASE_H /* #ifndef AEEIBASE_H */
/*======================================================

Copyright © 2005-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/
#include "AEEStdDef.h"
#include "AEEInterface.h"

#define AEEIID_IBase    0x01036a7b

#define INHERIT_IBase(iname) \
   uint32 (*AddRef)(iname*); \
   uint32 (*Release)(iname*)

AEEINTERFACE_DEFINE(IBase);

static __inline uint32 IBase_AddRef(IBase *me)
{
   return AEEGETPVTBL(me,IBase)->AddRef(me);
}

static __inline uint32 IBase_Release(IBase *me)
{
   return AEEGETPVTBL(me,IBase)->Release(me);
}

/*
=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================

IBASE Interface

Description:
   IBASE (IBase) is the base level object class from which extensible
   API object classes should be derived.  It supplies a standard
   mechanism to accomplish object extensibility while maintaining
   binary compatibility.

=======================================================================

IBase_AddRef()

Description:

   This method increments the reference count on an object.  This
   allows the object to be shared by multiple callers.  The object 
   can be trusted to remain valid until a matching call to Release()
   is made.  AddRef() should be called whenever a reference to an object 
   is copied (e.g., when written to the output parameter to 
   Base())

Prototype:

   uint32 IBase_AddRef(IBase * po);

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
   IBase_Release()
=======================================================================
IBase_Release()

Description:

   This function decrements the reference count of an object.  This
   function should be called whenever a reference to an object 
   is destroyed (e.g. set to 0).

Prototype:

   uint32 IBase_Release(IBase * po);

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
   IBase_AddRef()

=======================================================================
*/
#endif /* #ifndef AEEIBASE_H */
