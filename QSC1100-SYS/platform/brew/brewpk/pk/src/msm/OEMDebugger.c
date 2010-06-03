/*=====================================================
FILE:  OEMDebugger.c

SERVICES:   Debugger Interface

DESCRIPTION: This file provides defintions for the debugger interfaces made available by
the AEEDebugger.lib to application developers. This is a standard  file that must be included by
all applications using the debugger services of the AEE.

There is 1 interface

IDebugger -

PUBLIC CLASSES AND STATIC FUNCTIONS:
	List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 2003-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "task.h"
#include "OEMHeap.h"
#include "AEEStdLib.h"
#include "AEE.h"
#include "AEEError.h"
#include "AEESource.h"
#include "OEMDebugger.h"
#include "AEEComdef.h"
#include "AEE_OEM.h"

extern byte usr_stack[];

/*===========================================================================

                      MACRO DEFINITIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

// IDebugger
typedef struct _IOEMDebugger
{

   const AEEVTBL(IDEBUGGER)* pvt;
   IShell         *m_pIShell;
   uint32         m_nRefs;
   void*         m_pContext;

}IOEMDebugger;

extern brew_reg_dbg_callback(void*, void*);
extern int g_debugFlag;
extern void dog_kick(void);

static const	VTBL(IDEBUGGER)		gDEBUGGER_Funcs=	{
									OEMDEBUGGER_AddRef,
									OEMDEBUGGER_Release,
									OEMDEBUGGER_QueryInterface,
									OEMDEBUGGER_SetCtx,
									OEMDEBUGGER_GetCtx,
									OEMDEBUGGER_RegCallback,
									OEMDEBUGGER_GetHeapRange,
									OEMDEBUGGER_GetHeapSize,
									OEMDEBUGGER_GetStackRange,
									OEMDEBUGGER_GetStackSize
									};


#define  stack_size   UI_STACK_SIZ

/*===========================================================================
FUNCTION	OEMDEBUGGER_New()

DESCRIPTION
	Allocates and constructs a new IOEMDEBUGGER object.
=======================================================================*/

int OEMDEBUGGER_New(IShell * pIShell,AEECLSID ClsId, void ** ppObj)
{
   int nReturn = ENOMEMORY;
   IOEMDebugger *pMe = (IOEMDebugger*) MALLOC(sizeof(IOEMDebugger));

   if (pMe) {
         pMe->pvt = &gDEBUGGER_Funcs;
         pMe->m_nRefs = 1;

         pMe->m_pIShell = pIShell;

         *ppObj = pMe;
         nReturn = SUCCESS;
      }


   return nReturn;
}


/*===========================================================================
FUNCTION	OEMDEBUGGER_AddRef()

DESCRIPTION
   This function increments the reference count of the IDEBUGGER Interface object.
   This  allows the object to be shared by multiple callers. The object is freed
   when the reference count reaches 0 (zero).

DEPENDENCIES
   none

RETURN VALUE
   The update reference count.
=======================================================================*/

static uint32 OEMDEBUGGER_AddRef(IDEBUGGER* po)
{
   IOEMDebugger* pMe = (IOEMDebugger*)po;
   return(++pMe->m_nRefs);
}

/*===========================================================================
FUNCTION	OEMDEBUGGER_Release()

DESCRIPTION
   This function increments the reference count of the IDEBUGGER Interface object.
   This  allows the object to be shared by multiple callers. The object is freed
   when the reference count reaches 0 (zero).

DEPENDENCIES
   none

RETURN VALUE
   The update reference count.
=======================================================================*/

static uint32 OEMDEBUGGER_Release(IDEBUGGER* po)
{
   IOEMDebugger* pMe = (IOEMDebugger*)po;

   if(pMe->m_nRefs){
      if(--pMe->m_nRefs)
      return(pMe->m_nRefs);
	}

   // Free object
   FREEIF(pMe);
   g_debugFlag = 0;
   return(0);
}

/*===========================================================================

Function:  OEMDEBUGGER_QueryInterface()

Description:
   This is the QueryInterface function of the IDebugger Extension.

Parameters:
   IDebugger *pif
   AEECLSID id
   ppo

Return Value:
   Error message

Comments:
   None

Side Effects:
   None

==============================================================================*/

static int OEMDEBUGGER_QueryInterface(IDEBUGGER *pif, AEECLSID id, void **ppo)
{
   if (id == AEECLSID_DEBUGGER)
   {
      *ppo = pif;
      IQI_AddRef(*ppo);
      return SUCCESS;
   }
   else
   {
      *ppo = 0;
      return ECLASSNOTSUPPORT;
   }
}


/*===========================================================================
FUNCTION	OEMDEBUGGER_SetCtx

DESCRIPTION
	Set the BREW Debugger Context

DEPENDENCIES
   none

RETURN VALUE
	void
=======================================================================*/

static void	OEMDEBUGGER_SetCtx(IDEBUGGER* po, void* pbCtx)
{
   IOEMDebugger* pMe = (IOEMDebugger*)po;
   pMe->m_pContext = pbCtx;
}


/*===========================================================================
FUNCTION	OEMDEBUGGER_GetCtx

DESCRIPTION
	Get the BREW Context

DEPENDENCIES
   none

RETURN VALUE
	void
=======================================================================*/

static void*	OEMDEBUGGER_GetCtx(IDEBUGGER* po)
{
   return (void*)((IOEMDebugger*)po)->m_pContext;
}

/*===========================================================================
FUNCTION	OEMDEBUGGER_RegCallback

DESCRIPTION
   Register a debugger callback

DEPENDENCIES
   none

RETURN VALUE
   SUCCESS/EFAILED
=======================================================================*/

static int OEMDEBUGGER_RegCallback(IDEBUGGER* po, void* callbackFn)
{
   if(!callbackFn)
      return EFAILED;

    brew_reg_dbg_callback(callbackFn, (void*)((IOEMDebugger*)po)->m_pContext);

   return SUCCESS;
}


/*===========================================================================
FUNCTION	OEMDEBUGGER_GetHeapRange

DESCRIPTION
   gets the starting address of the heap

DEPENDENCIES
   none

RETURN VALUE
   starting address of the heap allocation
=======================================================================*/

static uint32 OEMDEBUGGER_GetHeapRange(IDEBUGGER* po)
{

   void* ppHeapBytes;
   uint32 puHeapBytesLen;

   OEM_GetHeapInitBytes(&ppHeapBytes,&puHeapBytesLen);

   return ((uint32)(ppHeapBytes));
}

/*===========================================================================
FUNCTION	OEMDEBUGGER_GetHeapSize

DESCRIPTION
   gets the size of the heap

DEPENDENCIES
   none

RETURN VALUE
   size of the heap allocation
=======================================================================*/

static uint32 OEMDEBUGGER_GetHeapSize(IDEBUGGER* po)
{

   void* ppHeapBytes;
   uint32 puHeapBytesLen;

   OEM_GetHeapInitBytes(&ppHeapBytes,&puHeapBytesLen);

   return (puHeapBytesLen);
}

/*===========================================================================
FUNCTION	OEMDEBUGGER_GetStackRange

DESCRIPTION
   get the starting address of the stack allocation

DEPENDENCIES
   none

RETURN VALUE
   Starting address of stack allocation
=======================================================================*/

static uint32 OEMDEBUGGER_GetStackRange(IDEBUGGER* po)
{
   return ((uint32) &ui_stack);
}

/*===========================================================================
FUNCTION	OEMDEBUGGER_GetStackSize

DESCRIPTION
   get the size of the stack allocation

DEPENDENCIES
   none

RETURN VALUE
   Size of stack allocation
=======================================================================*/

static uint32 OEMDEBUGGER_GetStackSize(IDEBUGGER* po)
{
  return ( (uint32) stack_size);
}

/*===========================================================================

FUNCTION OEMDEBUGGER_DogKick

DESCRIPTION
  This procedure resets the watchdog timer circuit.  Resets the circuit so that
  we have another N milliseconds before the circuit will reset the system.
  Arms auto-kick.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void OEMDEBUGGER_DogKick(IDEBUGGER* po)
{
	dog_kick();
}

