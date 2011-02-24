#include "AEEDebugger.h"

static uint32	OEMDEBUGGER_AddRef(IDEBUGGER* po);
static uint32	OEMDEBUGGER_Release(IDEBUGGER* po);
static int    	OEMDEBUGGER_QueryInterface (IDEBUGGER *pMe, AEECLSID id, void **ppo);
static void		OEMDEBUGGER_SetCtx(IDEBUGGER* po, void* pbCtx);
static void*   	OEMDEBUGGER_GetCtx(IDEBUGGER* po);
static int 		OEMDEBUGGER_RegCallback(IDEBUGGER* po, void* callbackFn);
static uint32   OEMDEBUGGER_GetHeapRange(IDEBUGGER* po);
static uint32   OEMDEBUGGER_GetHeapSize(IDEBUGGER* po);
static uint32   OEMDEBUGGER_GetStackRange(IDEBUGGER* po);
static uint32   OEMDEBUGGER_GetStackSize(IDEBUGGER* po);
		void 	OEMDEBUGGER_DogKick(IDEBUGGER* po);

/*===========================================================================
OEMDEBUGGER_AddRef()

Description:
	   This function increments the reference count of the IDEBUGGER Interface object.
	   This  allows the object to be shared by multiple callers. The object is freed
   	   when the reference count reaches 0 (zero).

Prototype:
   static uint32 OEMDEBUGGER_AddRef(IDEBUGGER* po)

Parameters:
	Pointer to the IDebugger interface

Returns:
   The updated reference count.

Comments
	Ideally there will be one interface active at a given time

Side Effects:
   None

See Also:
	None
===========================================================================
=======================================================================

OEMDEBUGGER_Release()

Description:

   This function increments the reference count of the IDEBUGGER Interface object.
   This  allows the object to be shared by multiple callers. The object is freed
   when the reference count reaches 0 (zero).

Prototype:

static uint32 OEMDEBUGGER_Release(IDEBUGGER* po)

Parameters:
	Pointer to the IDebugger interface

Returns:
   The update reference count.

Comments
	None

Side Effects:
   None

See Also:
	None

===========================================================================

OEMDEBUGGER_QueryInterface()

Description:
   This is the QueryInterface function of the IDebugger Extension.

Prototype:

static uint32 OEMDEBUGGER_QueryInterface(IDEBUGGER *pMe, AEECLSID id, void **ppo)

Parameters:
	Pointer to the IDebugger interface
	Class Id.
	Returned object.

Returns:

Comments
	None

Side Effects:
   None

See Also:
	None
==============================================================================

===========================================================================
OEMDEBUGGER_SetCtx()

Description:
	This function is used to set the address of the brew debugger context and
	used to store the address of the debugger context inside BREW and when the
	app returns from the undef exception, the IDEBUGGER_GetCtx() is used to
	retrieve the address.

Prototype:
	static void	OEMDEBUGGER_SetCtx(IDEBUGGER* po, void* pbCtx)

Parameters:
	po - Pointer to the IDebugger interface
	pbCtx - Pointer to the brew debugger context (used for storing the address)

Returns:
   void

Comments
	None

Side Effects:
   None

See Also:
	None
=======================================================================
=======================================================================
OEMDEBUGGER_GetCtx()

Description:
	When the control is returned to the stub, the stub needs to know the address at
	which the debugging context was stored. This is done by making a call to the
	IDebugger interface which returns the address of the pointer pointing to the
	context.

Prototype:
   static void*	OEMDEBUGGER_GetCtx(IDEBUGGER* po)

Parameters:
	po = Pointer to the IDEBUGGER interface

Returns:
   A void pointer to the address where the context is stored. The app then
   typecasts the pointer to the debug context structure.

Comments
	None

Side Effects:
   None

See Also:
	None
=======================================================================
=======================================================================
OEMDEBUGGER_RegCallback()

Description:
	This function is used by the app to register a callback function when
	a breakpoint is hit.The IDegubber interface then stores the pointer to this
	function in the debugger lib and when the bkpt is hit and the function
	brew_dbg_undef_return is invoked after returning from the exception,
	it in turn calls this function in the stub and the control is now with the stub.

Prototype:
   static int OEMDEBUGGER_RegCallback(IDEBUGGER* po, void* callbackFn)

Parameters:
	po - pointer to the IDEBUGGER interface
	callbackFn - pointer to a callback function from the stub

Returns:
   EFAILED/SUCCESS

Comments
	None

Side Effects:
   None

See Also:
	None
=======================================================================
=======================================================================
OEMDEBUGGER_GetHeapRange

Description:
   This function gets the starting address of the heap.


Prototype:
	uint32   OEMDEBUGGER_GetHeapRange(IDEBUGGER* po);

Parameters:
	po - pointer to the IDEBUGGER interface

Returns:
   starting address of the heap allocation

Comments
	None

Side Effects:
 	None

See Also:
	None
=======================================================================
=======================================================================
OEMDEBUGGER_GetHeapSize

Description:
   It gets the size of the heap

Prototype:
	uint32 OEMDEBUGGER_GetHeapSize(IDEBUGGER* po)

Parameters:
	po - pointer to the IDEBUGGER interface

Returns:
	size of the heap allocation

Comments
	None

Side Effects:
	None

See Also:
	None
=======================================================================
=======================================================================
OEMDEBUGGER_GetStackRange

Description:
   It gets the starting address of the stack allocation

Prototype:
	uint32 OEMDEBUGGER_GetStackRange(IDEBUGGER* po)

Parameters:
	po - pointer to the IDEBUGGER interface

Returns:
	size of the heap allocation

Comments:
	None

Side Effects:
	None

See Also:
	None
=======================================================================
=======================================================================
OEMDEBUGGER_GetStackSize

Description:
   It gets the size of the stack allocation

Prototype:
	uint32 OEMDEBUGGER_GetStackSize(IDEBUGGER* po)

Parameters:
	po - pointer to the IDEBUGGER interface

Returns:
	size of the stack

Comments:
	None

Side Effects:
	None

See Also:
	None
=======================================================================
=======================================================================
OEMDEBUGGER_DogKick

Description:
  This procedure resets the watchdog timer circuit.  Resets the circuit so that
  we have another N milliseconds before the circuit will reset the system.
  Arms auto-kick.

Prototype:
	void OEMDEBUGGER_DogKick(IDEBUGGER* po)

Parameters:
	po - pointer to the IDEBUGGER interface

Returns:
	size of the stack

Comments:
	None

Side Effects:
	None

See Also:
	None
===========================================================================*/

