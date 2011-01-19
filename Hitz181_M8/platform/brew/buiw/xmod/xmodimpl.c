#include "AEEStdLib.h"
#include "AEEShell.h"           /* Shell interface definitions */
#include "xmod.h"

/*===========================================================================
  || xModule stuff
  ===========================================================================*/
/*
  || types
*/
typedef struct xModule {
   IModule   im; /* inline struct, has vtable */
            
   unsigned  uRef;
   IShell   *piShell;

} xModule;

/*
  || forward decls (vtable parts)
*/
static uint32 xModule_AddRef(IModule *p);
static uint32 xModule_Release(IModule *p);
static void xModule_FreeResources(IModule *p, IHeap *ph, IFileMgr *pfm);

static void xModule_Delete(xModule *me)
{
   ISHELL_Release(me->piShell);
   FREE(me);
}

static void xModule_CtorZ(xModule *me, IShell *piShell, PFNCREATEINSTANCE pfnCreateInstance)
{
   me->im.pvt = (IModuleVtbl *)(me + 1);

   /* Initialize individual entries in the VTBL */
   me->im.pvt->AddRef         = xModule_AddRef;
   me->im.pvt->Release        = xModule_Release;
   me->im.pvt->CreateInstance = pfnCreateInstance;
   me->im.pvt->FreeResources  = xModule_FreeResources;

   me->uRef = 1;
   me->piShell = piShell;
   ISHELL_AddRef(piShell);

}

int xModule_New(IShell *piShell, IModule **pp,
                PFNCREATEINSTANCE pfnCreateInstance)
{
   xModule *me = 0;

   me = MALLOCREC_EX(xModule,sizeof(IModuleVtbl));

   if ((xModule *)0 != me) {
      xModule_CtorZ(me,piShell,pfnCreateInstance);
      *pp = &me->im;
      return SUCCESS;
   } else {
      return ENOMEMORY;
   }

}

static uint32 xModule_AddRef(IModule *p)
{
   xModule *me = (xModule *)p;
   return ++me->uRef;
}

static uint32 xModule_Release(IModule *p)
{
   xModule *me = (xModule *)p;
   uint32 uRef = --me->uRef;

   if (uRef == 0) {
      xModule_Delete(me);
   }
   return uRef;
}

static void xModule_FreeResources(IModule *p, IHeap *ph, IFileMgr *pfm)
{
   (void)p,(void)ph,(void)pfm;
}

