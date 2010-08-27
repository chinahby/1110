/*======================================================
FILE:       CustomModel.c

SERVICES:   Custom Model class that does nothing 

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "CustomModel_priv.h"
#include "CustomModelBase.h"
#include "samplewidgets_util.h"

/////////////////////////////////////////////////////////////////////
// CustomModel structure

typedef struct CustomModel 
{
   ModelBase     base;
} CustomModel;


/////////////////////////////////////////////////////////////////////
// Function prototypes

static int CustomModel_QueryInterface(ICustomModel *po, AEECLSID id, void **ppo);
static uint32 CustomModel_Release(ICustomModel *po);
static void CustomModel_CustomFunc(ICustomModel *po);

static void CustomModel_Ctor(CustomModel *me, AEEVTBL(ICustomModel) *pvt, IModule *piModule);
static void CustomModel_Dtor(CustomModel *me);


/////////////////////////////////////////////////////////////////////
// CustomModel implementation

//--------------------------------------------------------------------
//    CustomModel implementation
//--------------------------------------------------------------------

/* -----------------------------------------------------------------------
    CustomModel_QueryInterface 
   --- */
static int CustomModel_QueryInterface(ICustomModel *po, AEECLSID id, void **ppo)
{
   if (id == AEEIID_CUSTOMMODEL) {
      *ppo = po;
      ICUSTOMMODEL_AddRef(po);
      return SUCCESS;
   }
   return ModelBase_QueryInterface((IModel*)po, id, ppo);
}


/* -----------------------------------------------------------------------
    CustomModel_Release 
   --- */
static uint32 CustomModel_Release(ICustomModel *po)
{
   CustomModel *me = (CustomModel *)po;

   if (me->base.nRefs == 1) {
      CustomModel_Dtor(me);
   }

   return ModelBase_Release(ICUSTOMMODEL_TO_IMODEL(po));
}


/* -----------------------------------------------------------------------
    CustomModel_CustomFunc 
   --- */
static void CustomModel_CustomFunc(ICustomModel *po)
{
   // place your custom code in member functions like this one.
}


/* -----------------------------------------------------------------------
    CustomWidget_Ctor 
   --- */
static void CustomModel_Ctor(CustomModel *me, AEEVTBL(ICustomModel) *pvt, IModule *piModule)
{
   ModelBase_Ctor(&me->base, (AEEVTBL(IModel) *)pvt, piModule);

   pvt->Release         = CustomModel_Release;
   pvt->QueryInterface  = CustomModel_QueryInterface;
   pvt->CustomFunc      = CustomModel_CustomFunc;
}


/* -----------------------------------------------------------------------
    CustomWidget_Dtor
   --- */
static void CustomModel_Dtor(CustomModel *me)
{
   // release internal members, if we have any ...

   return;
}


/* -----------------------------------------------------------------------
    CustomModel_New  
   --- */
int CustomModel_New(ICustomModel **ppo, IShell *piShell, IModule *piModule)
{
   CustomModel *me = MALLOCREC_VTBL(CustomModel, ICustomModel);
   
   if (me) {
      CustomModel_Ctor(me, GETVTBL(me,ICustomModel), piModule);
      *ppo = (ICustomModel*)me;
      return SUCCESS;
   }
   
   return ENOMEMORY;
}

