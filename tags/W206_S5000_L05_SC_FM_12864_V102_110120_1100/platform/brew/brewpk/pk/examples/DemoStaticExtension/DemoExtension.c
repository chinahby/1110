/*=============================================================================

FILE:      DemoExtension.c

SERVICES:  Sample extension implementation

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

   IDemoExtension

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEShell.h"
#include "AEEStdLib.h"
#include "IDemoExtension.h"
#include "DemoExtension.bid"

//=============================================================================
//   Macro definitions
//=============================================================================

#define RELEASEIF(p) do { if (p) { IQI_Release((IQueryInterface*)(p)); p = 0; } } while (0)


//=============================================================================
//   Type definitions
//=============================================================================

typedef struct DemoExtension DemoExtension;
AEEINTERFACE(DemoExtension) {
   INHERIT_IDemoExtension(DemoExtension);
};

struct DemoExtension {
   const DemoExtensionVtbl *pvt;
   uint32   uRef;
   IShell * piShell;
   // additional private members go here
};


//=============================================================================
//
//
//=============================================================================
static void DemoExtension_Delete(DemoExtension *me)
{
   RELEASEIF(me->piShell);
   FREE(me);
}

//=============================================================================
//
//
//=============================================================================
static int DemoExtension_DrawHello(DemoExtension *me, AEEPoint ptXYOffset)
{
   AECHAR      szbuff[] = {'H','E','L','L','O','\0'};
   IDisplay *  piDisplay = NULL; // Display interface pointer

   if (SUCCESS != ISHELL_CreateInstance(me->piShell, AEECLSID_DISPLAY,
                                        (void **)&piDisplay)) {
      return EFAILED;
   }

   IDISPLAY_DrawText(piDisplay, AEE_FONT_BOLD, szbuff, -1,
                     ptXYOffset.x, ptXYOffset.y, NULL, 0);

   IDISPLAY_Release(piDisplay);
   return SUCCESS;
}

//=============================================================================
//
//
//=============================================================================
static uint32 DemoExtension_AddRef(DemoExtension *me)
{
   return ++me->uRef;
}

//=============================================================================
//
//
//=============================================================================
static uint32 DemoExtension_Release(DemoExtension *me)
{
   uint32 uRef = --me->uRef;

   if (0 == uRef) {
      DemoExtension_Delete(me);
   }

   return uRef;

}

//=============================================================================
//
//
//=============================================================================
static int DemoExtension_QueryInterface(DemoExtension *me, AEECLSID cls, void **ppo)
{
   switch (cls) {
   case AEECLSID_QUERYINTERFACE:  // IQueryInterface interface ID
   case AEEIID_DEMOEXTENSION:     // IDemoExtension interface ID (must be changed)
   case AEECLSID_DEMOEXTENSION:   // DemoExtension class ID (must be changed)
      *ppo = me;
      DemoExtension_AddRef(me);
      return SUCCESS;

   default:
      return ECLASSNOTSUPPORT;
   }
}

//=============================================================================
//
//
//=============================================================================
static void DemoExtension_CtorZ(DemoExtension *me, IShell *piShell)
{
   {
      static const DemoExtensionVtbl vt = {
         DemoExtension_AddRef,
         DemoExtension_Release,
         DemoExtension_QueryInterface,
         DemoExtension_DrawHello,
      };
      me->pvt = &vt;
   }

   me->uRef = 1;

   me->piShell = piShell;
   ISHELL_AddRef(piShell);
}

/*=============================================================================

Function:  DemoExtension_New()

Description:
   This function is called from AEEClsCreateInstance. This helps 
   with construction of the class and initialization of its members.   

Prototype:
   int DemoExtension_New(IShell *piShell, AEECLSID cls, void **ppif)

Parameters:
   piShell: Pointer to IShell interface
   cls : ClassID passed in 
   ppif: [out]: On return, *ppif contains a valid pointer to the
                IDemoExtension interface 

Return Value:
   AEE_SUCCESS: If successful
   Error code: If failed

Comments:  None

Side Effects: None

=============================================================================*/
int DemoExtension_New(IShell *piShell, AEECLSID cls, void **ppif)
{
   int            nErr = SUCCESS;
   DemoExtension *me = 0;

   me = (DemoExtension *)MALLOC(sizeof(DemoExtension));
   if ((DemoExtension *)0 == me) {
      nErr = ENOMEMORY;
      goto bail;
   }

   DemoExtension_CtorZ(me, piShell);

 bail:
   *ppif = (void*)me;

   return nErr;
}


#include "AEEModTable.h"

// Create an AEEStaticClass array that lists the information about the interface 
const AEEStaticClass gDemoExtensionClasses[] = {
   { AEECLSID_DEMOEXTENSION, ASCF_UPGRADE, 0, 0, DemoExtension_New },
   0,
};

// Define this if you are intending to build this as a static extension DLL.
// Otherwise this implementation file can be statically built and linked in
// the target build.
#ifdef _BUILD_AS_STATIC_EXTENSION_
/*===========================================================================

Function:  GetStaticClassEntries()

Description:
   This function is required to be exposed if this interface will be 
   exposed as a static extension DLL. BREW Simulator loads all the DLLs
   that expose this function as a Static Extension DLL.

Prototype:
      const AEEStaticClass* GetStaticClassEntries(void)

Parameters: None  

Return Value:
   return an array of AEEStaticClass

Comments:  None

Side Effects: None

==============================================================================*/
BREWSTATICEXTENSION_API const AEEStaticClass* GetStaticClassEntries(void)
{
   return gDemoExtensionClasses;
}
#endif // _BUILD_AS_STATIC_EXTENSION_

