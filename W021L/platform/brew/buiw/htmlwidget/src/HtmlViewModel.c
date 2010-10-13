/*
  ========================================================================

  FILE:  HtmlViewModel.c
  
  SERVICES: 

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "HtmlViewModel.h"
#include "HtmlWidget.h"
#include "ModelBase.h"

#define MAKE_ME   HtmlViewModel *me = (HtmlViewModel*)po

/////////////////////////////////////////////////////////////////////////////
// IHtmlViewModel implementation

typedef struct HtmlViewModel {
   ModelBase      base;       // model base struct
   IShell *       piShell;    // for IShell_Resume
   // pointer to HtmlWidget object
   HtmlWidget *   phw;        // pointer to (private) HtmlWidget object
} HtmlViewModel;


/////////////////////////////////////////////////////////////////////////////
// IHtmlViewModel methods


static boolean HtmlViewModel_IsBusy(IHtmlViewModel *po)
{
   MAKE_ME;
   return (boolean)(me->phw->isbusy);
}

static int32 HtmlViewModel_FindElem(IHtmlViewModel *po, const char *pszElem, int32 nIndex)
{
   MAKE_ME;
   return Dox_FindElem(&me->phw->dox, pszElem, nIndex);
}

static int32 HtmlViewModel_GetElemText(IHtmlViewModel *po, int32 nElem, char *pchBuf, int32 cbBuf)
{
   MAKE_ME;
   return Dox_GetElemText(&me->phw->dox, nElem, pchBuf, cbBuf);
}

static int32 HtmlViewModel_GetAttrValue(IHtmlViewModel *po, int32 nElem, const char *pszAttr,  char *pchBuf, int32 cbBuf)
{
   MAKE_ME;	
   return Dox_GetAttrString(&me->phw->dox, nElem, pszAttr, pchBuf, cbBuf);   
}

static void HtmlViewModel_RestoreState(IHtmlViewModel *po, const char *pszState)
{
   MAKE_ME;
   Dox_SetFormElemState(&me->phw->dox, pszState);
   return;
}

static int HtmlViewModel_SaveState(IHtmlViewModel *po, char *pszBuffer, int cbBuffer)
{
   MAKE_ME;
   return Dox_GetFormElemState(&me->phw->dox, pszBuffer, cbBuffer);
}


/////////////////////////////////////////////////////////////////////////////
// IHtmlViewModel creation and IBase methods

/*
:: HtmlViewModel_Dtor
*/
static void HtmlViewModel_Dtor(HtmlViewModel *me)
{
   RELEASEIF(me->piShell);
}


/*
:: HtmlViewModel_Release
*/
static uint32 HtmlViewModel_Release(IHtmlViewModel *po)
{
   MAKE_ME;

   if (me->base.nRefs == 1) {
      HtmlViewModel_Dtor(me);
   }
   return ModelBase_Release((IModel*)me);
}

/*
:: HtmlViewModel_QueryInterface
*/
static int HtmlViewModel_QueryInterface(IHtmlViewModel *po, AEECLSID id, void **ppo)
{
   if (SUCCESS == ModelBase_QueryInterface((IModel*)po, id, ppo)) {
      return SUCCESS;
   }

   if (id == AEEIID_HTMLVIEWMODEL) {
      *ppo = po;
      IHTMLVIEWMODEL_AddRef(po);
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}


/*
:: HtmlViewModel_Ctor
*/
static void HtmlViewModel_Ctor(HtmlViewModel *me, AEEVTBL(IHtmlViewModel) *pvt, IShell *piShell, 
                               IModule *piModule, HtmlWidget *phw)
{
   ModelBase_Ctor(&me->base, (AEEVTBL(IModel)*)pvt, piModule);

   // override vtable methods
   pvt->Release         = HtmlViewModel_Release;
   pvt->QueryInterface  = HtmlViewModel_QueryInterface;
   
   // initialize IHtmlViewModel method(s)
#define VTINIT(name)   pvt->name = HtmlViewModel_##name
   VTINIT( IsBusy );
   VTINIT( FindElem );
   VTINIT( GetElemText );
   VTINIT( GetAttrValue );
   VTINIT( RestoreState );
   VTINIT( SaveState );

   me->piShell = piShell;
   ADDREFIF(piShell);

   me->phw = phw;    // no AddRef
}


/*
:: HtmlViewModel_New
*/
int HtmlViewModel_New(IHtmlViewModel **ppo, IShell *piShell, IModule *piModule, 
                      HtmlWidget *phw)
{
   HtmlViewModel *me = MALLOCREC_VTBL(HtmlViewModel, IHtmlViewModel);
   
   if (!me) {
      *ppo = 0;
      return ENOMEMORY;
   }

   HtmlViewModel_Ctor(me, GETVTBL(me,IHtmlViewModel), piShell, piModule, phw);
   *ppo = (IHtmlViewModel*)me;

   return SUCCESS;
}





