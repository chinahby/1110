#ifndef AEEDEBUGGER_H
#define AEEDEBUGGER_H
/*======================================================
FILE:  AEEDEBIGGER.h

SERVICES:  AEE DEBUGGER

IDEBUGGER

CLASS IDs:
	AEECLSID_DEBUGGER

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEComdef.h"

#define AEECLSID_DEBUGGER		   0x0101611a

//************************************************************************************************
//
// IDEBUGGER Interface
//
//************************************************************************************************

// IDEBUGGER Object
typedef struct _IDEBUGGER			IDEBUGGER;


AEEINTERFACE(IDEBUGGER)
{
   INHERIT_IQueryInterface(IDEBUGGER);

   void	    (*SetCtx)(IDEBUGGER* po, void* pbCtx);
   void*    (*GetCtx)(IDEBUGGER* po);
   int      (*RegCallback)(IDEBUGGER* po, void* callbackFn);
   uint32   (*GetHeapRange)(IDEBUGGER* po);
   uint32   (*GetHeapSize)(IDEBUGGER* po);
   uint32   (*GetStackRange)(IDEBUGGER* po);
   uint32   (*GetStackSize)(IDEBUGGER* po);
  };

#define IDEBUGGER_AddRef(p)                    AEEGETPVTBL(p,IDEBUGGER)->AddRef(p)
#define IDEBUGGER_Release(p)				   AEEGETPVTBL(p,IDEBUGGER)->Release(p)
#define IDEBUGGER_QueryInterface(p, i, o)	   AEEGETPVTBL((p),IDEBUGGER)->QueryInterface((p),(i),(o))
#define IDEBUGGER_SetCtx(p, pbCtx)            AEEGETPVTBL(p,IDEBUGGER)->SetCtx(p, pbCtx)
#define IDEBUGGER_GetCtx(p)                   AEEGETPVTBL(p,IDEBUGGER)->GetCtx(p)
#define IDEBUGGER_RegCallback(p,callbackFn)   AEEGETPVTBL(p,IDEBUGGER)->RegCallback(p,callbackFn)
#define IDEBUGGER_GetHeapRange(p)            AEEGETPVTBL(p,IDEBUGGER)->GetHeapRange(p)
#define IDEBUGGER_GetHeapSize(p)            AEEGETPVTBL(p,IDEBUGGER)->GetHeapSize(p)
#define IDEBUGGER_GetStackRange(p)            AEEGETPVTBL(p,IDEBUGGER)->GetStackRange(p)
#define IDEBUGGER_GetStackSize(p)            AEEGETPVTBL(p,IDEBUGGER)->GetStackSize(p)


#endif /* AEEDEBUGGER_H */


/*
=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================


=======================================================================
=======================================================================
=======================================================================

Interface Name: IDEBUGGER

Description:

   This interface provides hooks for the debugging stub for storing the
   debugging context, registering a callback function to be invoked when
   breakpoint is hit, and for accessing the Heap and the Stack ranges.

   Typical use involves instantiating an IDEBUGGER


Functions in this interface:                     ~
   IDEBUGGER_AddRef()                             ~
   IDEBUGGER_Release()                             ~
   IDEBUGGER_QueryInterface()                       ~
   IDEBUGGER_GetCtx()                                ~
   IDEBUGGER_SetCtx()                                 ~
   IDEBUGGER_RegisterCallback()                        ~
   IDEBUGGER_GetHeapRange()                             ~
   IDEBUGGER_GetHeapSize()                               ~
   IDEBUGGER_GetStackRange()                              ~
   IDEBUGGER_GetStackSize()
=======================================================================

IDEBUGGER_AddRef()

Description:
	To update the reference count for the IDebugger interface
Prototype:
   static uint32 IDEBUGGER_AddRef(IDEBUGGER* po)

Parameters:
	Pointer to the IDebugger interface

Returns:
   The updated reference count.

Comments:
	Ideally there will be one interface active at a given time

Side Effects:
   None

See Also:
	None

=======================================================================

=======================================================================

IDEBUGGER_Release()

Description:

  Used for releasing the interface
Prototype:

static uint32 IDEBUGGER_Release(IDEBUGGER* po)
Parameters:
	Pointer to the IDebugger interface

Returns:
   The update reference count.

Comments:
	None

Side Effects:
   None

See Also:
	None

======================================================================
======================================================================
IDEBUGGER_QueryInterface()
This function is inherited from IQI_QueryInterface().
=======================================================================
=======================================================================

IDEBUGGER_SetCtx()

Description:
	This function is used to set the address of the brew debugger context and used to store the address
	of the debugger context inside BREW and when the app returns from the undef exception,
	the IDEBUGGER_GetCtx() is used to retrieve the address.

Prototype:
	static void	IDEBUGGER_SetCtx(IDEBUGGER* po, void* pbCtx)

Parameters:

	po - Pointer to the IDebugger interface
	pbCtx - Pointer to the brew debugger context (used for storing the address)
Returns:
   void

Comments:
	None

Side Effects:
   None

See Also:
	None

=======================================================================
=======================================================================

IDEBUGGER_GetCtx()

Description:
	When the control is returned to the stub, the stub needs to know the address at
	which the debugging context was stored. This is done by making a call to the
	IDebugger interface which returns the address of the pointer pointing to the
	context.
Prototype:
   static void*	IDEBUGGER_GetCtx(IDEBUGGER* po)

Parameters:
	po = Pointer to the IDEBUGGER interface

Returns:
   A void pointer to the address where the context is stored. The app then
   typecasts the pointer to the debug context structure.

Comments:
	None

Side Effects:
   None

See Also:
	None

=======================================================================
=======================================================================

IDEBUGGER_RegisterCallback()

Description:
	This function is used by the app to register a callback function when
	a breakpoint is hit.The IDegubber interface then stores the pointer to this
	function in the debugger lib and when the bkpt is hit and the function
	brew_dbg_undef_return is invoked after returning from the exception,
	it in turn calls this function in the stub and the control is now with the stub.
Prototype:
   static int IDEBUGGER_RegCallback(IDEBUGGER* po, void* callbackFn)

Parameters:
	po - pointer to the IDEBUGGER interface
	callbackFn - pointer to a callback function from the stub

Returns:
   EFAILED/SUCCESS

Comments:
	None

Side Effects:
   None

See Also:
	pk/src/brew_debugger.c - Function - brew_dbg_undef_return

=======================================================================
=======================================================================

IDEBUGGER_GetHeapRange()

Description:
	Used to determine the starting range for heap allocation
Prototype:
   static uint32 IDEBUGGER_GetHeapRange(IDEBUGGER* po)

Parameters:
	po - pointer to the IDEBUGGER interface

Returns:
	The starting address for heap allocation
Comments:
	None

Side Effects:
   None

See Also:
	None

=======================================================================
=======================================================================
=======================================================================

IDEBUGGER_GetHeapSize()

Description:
	Used to determine the size for heap allocation
Prototype:
	static uint32 IDEBUGGER_GetHeapSize(IDEBUGGER* po)

Parameters:
	po - pointer to the IDEBUGGER interface

Returns:
	The size of heap allocation
Comments:
	None

Side Effects:
   None

See Also:
	None

=======================================================================
=======================================================================

IDEBUGGER_GetStackRange()

Description:
	Used to determine the starting range for stack allocation
Prototype:
   static uint32 IDEBUGGER_GetStackRange(IDEBUGGER* po)

Parameters:
	po - pointer to the IDEBUGGER interface

Returns:
	The starting address for stack allocation
Comments:
	None

Side Effects:
   None

See Also:
	None

=======================================================================
=======================================================================

IDEBUGGER_GetStackSize()

Description:
	Used to determine the size for stack allocation
Prototype:
   static uint32 IDEBUGGER_GetStackSize(IDEBUGGER* po)

Parameters:
	po - pointer to the IDEBUGGER interface

Returns:
	The size for stack allocation
Comments:
	None

Side Effects:
   None

See Also:
	None

=======================================================================
=======================================================================

IDEBUGGER_DogKick()

Description:
		This procedure resets the watchdog timer circuit.  Resets the
		circuit so that we have another N milliseconds before the circuit
		will reset the system. Arms auto-kick.
Prototype:
		static void IDEBUGGER_DogKick(IDEBUGGER* po)


Parameters:
        po - pointer to the IDEBUGGER interface

Returns:
        None

Comments:
        None

Side Effects:
   		None

See Also:
        None
===========================================================================
*/

