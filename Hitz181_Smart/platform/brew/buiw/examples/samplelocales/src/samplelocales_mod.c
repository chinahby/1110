/*===========================================================================
  || examplelocales specific xModule stuff
  ===========================================================================*/
#include "AEEStdLib.h"
#include "AEEShell.h"           /* Shell interface definitions */

#include "AEECLSID_LOCALEDEDE.BID"
#include "AEECLSID_LOCALEENGB.BID"


// extern declarations of the NEW routines supported by this module.
// In this sample extension, ENGB and DEDE locales are implemented.
// Their create routines are declared here and called below.
extern int xModule_LocaleENGBCreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp);
extern int xModule_LocaleDEDECreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp);

int xModule_CreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp)
{
   if (idClass == AEECLSID_LOCALEENGB)
      return xModule_LocaleENGBCreateInstance(module, shell, idClass, pp);
   else if (idClass == AEECLSID_LOCALEDEDE)
      return xModule_LocaleDEDECreateInstance(module, shell, idClass, pp);

   return (ECLASSNOTSUPPORT);
}
