#include "AEEStdLib.h"
#include "AEEShell.h"           /* Shell interface definitions */

#include "xmod.h"

#ifndef AEE_STATIC

/* Allow the AEEMod_Load function to be renamed through a
   preprocessor directive */
#ifndef AEEMOD_LOAD
#define AEEMOD_LOAD AEEMod_Load
#endif

/* Allow the AEEMOD_CREATEINSTANCE function to te renamed through a
 * preprocessor directive */
#ifndef AEEMOD_CREATEINSTANCE
#define AEEMOD_CREATEINSTANCE xModule_CreateInstance  
#endif

#endif   // AEE_STATIC


/* Prototype the createinstance function */
int AEEMOD_CREATEINSTANCE(IModule *p, IShell *piShell, 
                          AEECLSID idClass, void **pp);


#if defined(AEE_SIMULATOR) && !defined(AEE_STATIC)
AEEHelperFuncs *g_pvtAEEStdLibEntry;
__declspec(dllexport) /* AEEMod_Load has to be exported from a DLL */
#endif /* #ifdef AEE_SIMULATOR */

/*
   ||
   || this bit of code has to be FIRST in a module (might as well be this one)
   ||
*/
int AEEMOD_LOAD(IShell *piShell, void *ph, IModule **ppMod)
{
   /*	
        ||  IMPORTANT NOTE: g_dwAEEStdLibEntry global variable is defined 
        ||     for SDK ONLY!
        || 
        ||  This variable should NOT BE (1) overwritten and/or (2) USED 
        ||    DIRECTLY by BREW SDK users. It is used as an entry point to 
        ||    AEEStdLib.
        || 
        ||  DO NOT REMOVE next three lines.
   */
#if defined(AEE_SIMULATOR) && !defined(AEE_STATIC)
   g_pvtAEEStdLibEntry = (AEEHelperFuncs *)ph;
#endif

   return xModule_New(piShell,ppMod,AEEMOD_CREATEINSTANCE);
}

