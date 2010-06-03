/*============================================================================

FILE:  oemidleapp.c

SERVICES:  
        Implementation for the idle application's shim services. This is also
        the recommended autostart application.

GENERAL DESCRIPTION:
        Implements a simple app framework to occupy an applet slot in the BREW
        applet stack when the native ui idle applet is running.

REVISION HISTORY: 

============================================================================

               Copyright © 2004 QUALCOMM Incorporated
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/
#include "uistate.h"

#include "shimapp.h"

#include "AEEStdLib.h"
#ifdef AEE_STATIC // {
#include "shimmodgen.h"
#endif // }AEE_STATIC

OBJECT(CIdleApp){
   AEEApplet            a;
};


#ifdef AEE_STATIC // {
AEEMod      gmIdleMod;
CIdleApp    gaIdleApp;

static int     IdleMod_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
int            IdleMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif // } AEE_STATIC

static int     IdleApp_InitAppData(IApplet * pIApplet);
static void    IdleApp_FreeAppletData(IApplet * pIApplet);
static boolean IdleApp_HandleEvent(IApplet * pIApplet, AEEEvent eCode, uint16 wParam, uint32 dwParam);

#ifdef AEE_STATIC // {
int IdleMod_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   if( !AEEStaticMod_NewNoAlloc(&gmIdleMod, ps, pHelpers,(PFNMODCREATEINST)IdleMod_CreateInstance,(PFNFREEMODDATA)NULL) ){
      *pMod = (IModule *)&gmIdleMod;
      return AEE_SUCCESS;
   }
   return EFAILED;
}
static int IdleMod_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#else // } AEE_STATIC {
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule * po, void ** ppObj)
#endif // } !AEE_STATIC
{
   int nRet = EFAILED;

   *ppObj = (IApplet *)0;
   switch( ClsId ){
   case AEECLSID_IDLEAPP:
      {
         nRet = ENOMEMORY;
#ifdef AEE_STATIC // {
         if( AEEStaticApplet_New((AEEApplet *)&gaIdleApp, ClsId, pIShell,po,(AEEHANDLER)IdleApp_HandleEvent,(PFNFREEAPPDATA)IdleApp_FreeAppletData) ){
            *ppObj = (IApplet *)&gaIdleApp;
#else // } AEE_STATIC {
         if( AEEApplet_New(sizeof(CIdleApp), ClsId, pIShell,po,(IApplet**)ppObj,(AEEHANDLER)IdleApp_HandleEvent,(PFNFREEAPPDATA)IdleApp_FreeAppletData) ){
#endif // } !AEE_STATIC
            if( (nRet = IdleApp_InitAppData((IApplet *)*ppObj)) != SUCCESS ){
               DBGPRINTF("*SHIM* Init Fails: %d", nRet);
               IdleApp_FreeAppletData((IApplet *)*ppObj);
            }
         }
      }
      break;
   default:
      nRet = ECLASSNOTSUPPORT;
      break;
   }
   return nRet;
}

static int IdleApp_InitAppData(IApplet * pIApplet)
{
   CIdleApp *  pme = (CIdleApp *)pIApplet;
   // TODO: Add applicable initialisation code

   return SUCCESS;
}
static void IdleApp_FreeAppletData(IApplet * pIApplet)
{
   // TODO: Add applicable cleanup code
   //CIdleApp *  pme = (CIdleApp *)pIApplet;

}
static boolean IdleApp_HandleEvent(IApplet * pIApplet, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   CIdleApp *  pme = (CIdleApp *)pIApplet;

   switch( eCode ){
   case EVT_APP_START:
      return TRUE;
   case EVT_APP_SUSPEND:
      return TRUE;
   case EVT_APP_STOP:
      return TRUE;
   case EVT_APP_RESUME:
      // TODO: Decide if this logic fits into your native applet design.
      // consider the case where a state transition moves from main menu application
      // to an application such as calculator. At that point the menu will be suspended
      // and when the calculator ends the menu will be resumed. Does the event need to be
      // sent, or will the handler call satisfy it alone. This case says the handler call
      // will satisfy it. You may need to trigger a native UI event outside of the dispatcher
      // to satisfy this condition.
      return TRUE;
   case EVT_KEY:
      if( wParam == AVK_CLR ){
         return TRUE; // User is not to interfere!
      }
      break;
   case EVT_CLOSE_SELF:
      {
         ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
      }
      return TRUE;
   case EVT_LAUNCH_APP:
      {
         ISHELL_StartApplet(pme->a.m_pIShell, (AEECLSID)dwParam);
      }
      return TRUE;
   default:
      break;
   }
   return FALSE;
}