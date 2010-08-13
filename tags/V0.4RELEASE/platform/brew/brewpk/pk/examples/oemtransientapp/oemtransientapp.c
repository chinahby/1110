/*============================================================================

FILE:  oemtransientapp.c

SERVICES:  
        Transient applet to shim each native UI application.

GENERAL DESCRIPTION:
        Allows maintenance of one applet stack on the system by BREW.
        Exports an applet per shimmed applet with the exception of the idle
        application. The idle application has it's own implementation file.

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

OBJECT(CTransApp){
   AEEApplet            a;

   // My class ID, so I can give app specific behaviors based on class
   AEECLSID             clsMe;
};

OBJECT(UIEventFnMap)
{
   AEECLSID             clsApp;
   PFNUIEVENTHANDLER    pfnEvtHandler;
};
// TODO: Example only, Add your apps and event handlers to this table
static const UIEventFnMap   gmapUIState[] = 
                  {{AEECLSID_IDLEAPP,   uistate_idle},
                   {AEECLSID_CALCAPP,    uistate_calculator},
                   {AEECLSID_MAINMENU,   uistate_mmenu},
                  };


#ifdef AEE_STATIC // {
AEEMod      gmTransMod;
CTransApp   gaTransApp[NUMBER_UI_APPLETS-1]; // minus 1 because Idle applet has it's own applet

static int     TransMod_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
int            TransMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);

#endif // } AEE_STATIC

static int     TransApp_InitAppData(IApplet * pIApplet);
static void    TransApp_FreeAppletData(IApplet * pIApplet);
static boolean TransApp_HandleEvent(IApplet * pIApplet, AEEEvent eCode, uint16 wParam, uint32 dwParam);

#ifdef AEE_STATIC // {
int TransMod_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   if( !AEEStaticMod_NewNoAlloc(&gmTransMod, ps, pHelpers,(PFNMODCREATEINST)TransMod_CreateInstance,(PFNFREEMODDATA)NULL) ){
      *pMod = (IModule *)&gmTransMod;
      return AEE_SUCCESS;
   }
   return EFAILED;
}
static int TransMod_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#else // } AEE_STATIC {
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule * po, void ** ppObj)
#endif // } !AEE_STATIC
{
   int nRet = EFAILED;

   *ppObj = (IApplet *)0;
   // +1 due to idle app's position
   if( (ClsId >= AEECLSID_FIRSTSHIM+1 && ClsId <= AEECLSID_LASTSHIM) ){
      CTransApp *    pme = NULL;
      
      nRet = ENOMEMORY;
#ifdef AEE_STATIC // {
      // minus1 due to idle app being first oem
      if( AEEStaticApplet_New((AEEApplet *)&gaTransApp[ClsId-(AEECLSID_FIRSTSHIM-1)], ClsId, pIShell,po,(AEEHANDLER)TransApp_HandleEvent,(PFNFREEAPPDATA)TransApp_FreeAppletData) ){
         *ppObj = (IApplet *)&gaTransApp[ClsId-(AEECLSID_FIRSTSHIM-1)];
#else // } AEE_STATIC {
      if( AEEApplet_New(sizeof(CTransApp), ClsId, pIShell,po,(IApplet**)ppObj,(AEEHANDLER)TransApp_HandleEvent,(PFNFREEAPPDATA)TransApp_FreeAppletData) ){
#endif // } !AEE_STATIC
         pme = (CTransApp *)*ppObj;
         pme->clsMe = ClsId;
         if( (nRet = TransApp_InitAppData((IApplet *)*ppObj)) != SUCCESS ){
            TransApp_FreeAppletData((IApplet *)*ppObj);
         }
      }
   }else{
      nRet = ECLASSNOTSUPPORT;
   }
   return nRet;
}

static int TransApp_InitAppData(IApplet * pIApplet)
{
   CTransApp *  pme = (CTransApp *)pIApplet;
   // TODO: Add applicable initialisation code

   return SUCCESS;
}
static void TransApp_FreeAppletData(IApplet * pIApplet)
{
   // TODO: Add applicable cleanup code
//   CTransApp *  pme = (CTransApp *)pIApplet; 

}
static boolean TransApp_HandleEvent(IApplet * pIApplet, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   CTransApp *  pme = (CTransApp *)pIApplet;

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
      // allows the shimmed application to control the CLR key
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
PFNUIEVENTHANDLER CShim_GetEventHandler(AEECLSID clsApp)
{
   UIEventFnMap * pMap;
   int            i,
                  nEntries = sizeof(gmapUIState) / sizeof(gmapUIState);

   for( i = 0, pMap = (UIEventFnMap *)gmapUIState; i < nEntries; pMap++,i++)
   {
      if( pMap->clsApp == clsApp ){
         return pMap->pfnEvtHandler;
      }
   }
   return NULL;
}