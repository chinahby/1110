/*===========================================================================

FILE: OEMResArbiter.c

SERVICES: OEM Resource Arbiter reference implementation.

DESCRIPTION
   This file is a reference implementation of the AEEResArbiter
	class.  It is currently defined to be a static module, but may easily
	be moved to a downloadable module.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#ifndef AEE_STATIC
#include "xmod.h"
#endif
#include "AEEResArbiter.h"
#include "AEETopVisibleCtl.h"

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/

//
// Resource Arbiter object (IResArbiter)
//
OBJECT(CResArbiter)
{
   const AEEVTBL(IResArbiter)  *pvt;

   // private object data;
   long           m_nRefs;                   // reference count
   IShell    *    m_pIShell;                 // IShell interface ptr
   IModule   *    m_pIModule;                // pointer to IModule
};

static int           OEMResArbiter_NewEx(IShell * pIShell, AEECLSID cls,IModule *pIModule, void ** ppobj);
static uint32        OEMResArbiter_AddRef(IResArbiter * po);
static uint32        OEMResArbiter_AddRef(IResArbiter * po);
static uint32        OEMResArbiter_Release(IResArbiter * po);
static int           OEMResArbiter_QueryInterface(IResArbiter * po, AEECLSID cls, void **ppo);
static int   			OEMResArbiter_ConfirmAcquire(IResArbiter * po, AEECLSID clsReq,
								AEEResCtlInfo * ownerInfo, AEEResCtlInfo * requesterInfo);

#ifndef AEE_STATIC
/*===========================================================================

FUNCTION: xModule_CreateInstance

DESCRIPTION
        This function is invoked while the app is being loaded. All Modules must provide this
        function. Ensure to retain the same name and parameters for this function.
        In here, the module must verify the ClassID and then invoke the xxx_New() function.

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to InitAppData(). This is done as follows:
   InitAppData() is called to initialize AppletData instance. It is app developers
   responsibility to fill-in app data initialization code of InitAppData(). App developer
   is also responsible to release memory allocated for data contained in AppletData -- this can be
   done in FreeAppData().

PROTOTYPE:
        int xModule_CreateInstance(AEECLSID clsID, IShell* pIShell, IModule* pIModule,
                                                                                  void** ppObj)

PARAMETERS:
        clsID: [in]: Specifies the ClassID of the class which is being loaded

        pIShell: [in]: Contains pointer to the IShell interface.

        pIModule: pin]: Contains pointer to the IModule interface to the current module to which
        this class belongs

        ppObj: [out]: On return, *ppObj must point to a valid object structure. Allocation
        of memory for this structure and initializing the base data members is done by xxx_New().

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS: If xxx_New() invocation was successful
  EFAILED: If errors occurred in AEEApplet_New().
  If this function returns FALSE, the object is not created.

SIDE EFFECTS
  none
===========================================================================*/
int xModule_CreateInstance(IModule *pIModule, IShell * pIShell, AEECLSID ClsId, void ** ppObj)
{
   *ppObj = NULL;

   // Verify the class id and instantiate the class
   switch(ClsId)
   {
	case AEECLSID_RESARBITER:
      return OEMResArbiter_NewEx(pIShell,ClsId,pIModule,ppObj);

   }


   return EFAILED;
}
#endif

/*===========================================================================

EXTERNAL - Called from OEM static table.  Supports creation of AEEResArbiter.

===========================================================================*/

int OEMResArbiter_New(IShell * pIShell, AEECLSID cls, void ** ppObj)
{
	return OEMResArbiter_NewEx(pIShell, cls, 0, ppObj);
}

/*===========================================================================

Internal - bumps up reference count to module, if externally loaded.

===========================================================================*/

int OEMResArbiter_NewEx(IShell * pIShell, AEECLSID cls, IModule *pIModule, void ** ppObj)
{
   CResArbiter *     pMe = NULL;
   VTBL(IResArbiter) *pifn;

  // allocate memory for object

   if (!pIShell || !ppObj)
      return(EBADPARM);

   *ppObj = NULL;

   if((pMe = (CResArbiter *)MALLOC(sizeof(CResArbiter)+sizeof(*pifn))) == NULL)
      return(ENOMEMORY);

   pifn = (VTBL(IResArbiter) *)((byte *)pMe + sizeof(CResArbiter));

   pifn->AddRef         = OEMResArbiter_AddRef;
   pifn->Release        = OEMResArbiter_Release;
   pifn->QueryInterface = OEMResArbiter_QueryInterface;
   pifn->ConfirmAcquire = OEMResArbiter_ConfirmAcquire;

   // initialize the vtable
   INIT_VTBL(pMe, IBase, *pifn);
   IBASE_AddRef((IBase *)pMe);

   pMe->m_pIShell  = pIShell;
   pMe->m_pIModule = pIModule;
   IBASE_AddRef((IBase *)pMe->m_pIShell);
   if (pIModule)
      IMODULE_AddRef(pIModule);

   *ppObj = pMe;

   return(SUCCESS);
}

/*=====================================================================

Public Method - Increments the reference count on the class object.

======================================================================*/
uint32 OEMResArbiter_AddRef(IResArbiter * po)
{
   return(++((CResArbiter*)po)->m_nRefs);
}

/*=====================================================================

Public Method - Decrements the reference count on the class object.
If the value reaches 0, it frees the object and all associated data.

======================================================================*/
static uint32 OEMResArbiter_Release(IResArbiter * po)
{
   CResArbiter * pMe = (CResArbiter*)po;

   if(pMe->m_nRefs){
      if (--pMe->m_nRefs)
         return(pMe->m_nRefs);

      // Release IShell & IDisplay
		if (pMe->m_pIModule)
			IBASE_Release((IBase *)pMe->m_pIModule);
		IBASE_Release((IBase *)pMe->m_pIShell);

      // Free Object
      FREE(pMe);
   }
   return(0);
}

/*=====================================================================

Public Method - return valid interfaces.  Note this increments the
   reference count.

======================================================================*/
int OEMResArbiter_QueryInterface(IResArbiter * po, AEECLSID cls, void **ppo)
{
   if (   (cls == AEECLSID_RESARBITER)
       || (cls == AEECLSID_QUERYINTERFACE)){
      *ppo = po;
      IRESARBITER_AddRef(po);
      return(SUCCESS);
   }
   *ppo = 0;
   return(ECLASSNOTSUPPORT);
}

/*=======================================================================
Function:

Description:

Prototype:
   int OEMResarbiter_ConfirmAcquire(IShell * po, AEECLSID clsReq, 
                                    AEEResCtlInfo * pOwner, AEEResCtlInfo * pRequestor

Parameters:
   po [in]:          Pointer to the IResArbiter object
   clsReq [in]:      Class id of resource in contention
   pOwner [in]:      Arbitration information about current owner
   pRequestor[in]:   Arbitration information about Requstor

Return Value:
   SUCCESS:    Data type is detected and MIME is returned
   EITEMBUSY:  Resource is in use and cannot be acquired.

Comments: None
Side Effects: None
See Also: None
========================================================================*/

int OEMResArbiter_ConfirmAcquire(IResArbiter * po, AEECLSID clsReq,
		AEEResCtlInfo * pOwner, AEEResCtlInfo * pRequestor)
{
   CResArbiter * pMe = (CResArbiter*)po;

   int status = EITEMBUSY;
   int i;

   //
   // first check class list to see if owner will allow it
   // 
   switch (pOwner->nClsCount)
   {
   case -1: // allow anyone to acquire resource
      status = SUCCESS;
      break;

   case 0: // allow no one to acquire resource
      status = EITEMBUSY;
      break;

   default: // check access (relinquish) list
      for (i=0;i<pOwner->nClsCount;i++)
      {
         uint32 privId = pOwner->pClsList[i];
         if (privId < QVERSION)
         {
            // is reason acceptable?
            if (privId == pRequestor->dwReason)
            {
               status = SUCCESS;
               break;
            }
         }
         else
         { 
            // does requestor class id match or has group privilege?
            if (ISHELL_CheckPrivLevel(pMe->m_pIShell,privId,TRUE))
            {
               status = SUCCESS;
               break;
            }
         }
      }
      break;
   }
   // At this point, an OEM may choose to accept the access list permission
   // checks and/or add additional decision algorithms such as examining 
   // current reason or allowing specific requestor clsid's reguardless 
   // of the owner's access list, etc.

   // by default, if the current owner indicates it's busy (with dialogs or ?) 
   // and the resource is TopVisible, don't release resource.
   // BREW sets the dwReason to RESCTL_REASON_BUSY if current application
   // responds to EVT_BUSY.
   if (pOwner->dwReason == RESCTL_REASON_BUSY && clsReq == AEECLSID_TOPVISIBLECTL)
      status = EITEMBUSY;

   // If the resource is TopVisible the OEM may examine start arguments
/************************************
   if (clsReq == AEECLSID_TOPVISIBLECTL) {
      char *pszArgs;
      int argLen;
      if (SUCCESS == ITOPVISIBLECTL_GetArgs((ITopVisibleCtl *)pRequestor->pCtl, NULL, &argLen)) {
         pszArgs = MALLOC(argLen);
         if(pszArgs) {
            ITOPVISIBLECTL_GetArgs((ITopVisibleCtl *)pRequestor->pCtl, pszArgs, &argLen);
            //use the args
            FREE(pszArgs);
         }
      }
   }
*************************************/
   return (status);
}
