/*=================================================================================
FILE:          YCbCrFactory.c

SERVICES:      IYCbCrFactory interface

DESCRIPTION:   This module implements the IYCbCrFactory class that can be used
               to create IYCbCr object.

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=================================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.


 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 10/15/07  KV     IYCbCr interface should be released by the caller
 3/17/06   RI     Initial design of the YCbCr Factory
============================================================================*/

#include "IYCbCrFactory.h"
#include "IYCbCr.h"
#include "YCbCr_priv.h"
#include "AEE.h"
#include "AEE_OEM.h"
/************************************************************************
 ** Type Definitions
 ************************************************************************/

struct IYCBCRFactory
{
  /* lint -e754 */
  AEEVTBL(IYCBCRFactory)      *pvt;
  /* lint +e754 */
  uint32           m_uRefs;
  IShell           *m_pIShell;
  AEECLSID         m_cls;
  IYCbCr           *m_pIYCbCr;
};


/************************************************************************
 ** Function Declarations
 ************************************************************************/

static uint32 YCbCrFactory_AddRef(IYCBCRFactory *pMe);
static uint32 YCbCrFactory_Release(IYCBCRFactory *pMe);
static int YCbCrFactory_QueryInterface
(
  IYCBCRFactory  *pMe,
  AEECLSID  id,
  void  **ppNew
);

static int YCbCrFactory_Create
(
  IYCBCRFactory  *pMe,
  uint32  uScheme,
  uint32  cx,
  uint32  cy,
  IYCbCr  **ppIYCbCr
);

static const AEEVTBL(IYCBCRFactory) gIYCbCrFactoryFuncs =
{
  YCbCrFactory_AddRef,
  YCbCrFactory_Release,
  YCbCrFactory_QueryInterface,
  YCbCrFactory_Create
};

/************************************************************************
 ** Interface functions
 ************************************************************************/

/*=============================================================================
FUNCTION: IYCbCrFactory_New

DESCRIPTION:
  Create a IYCBCRFactory object and initialize the members.

DEPENDENCIES:
  None.

SIDE EFFECTS:
  None.

=============================================================================*/
int IYCbCrFactory_New(IShell* pIShell, AEECLSID cls, void **ppif)
{

  IYCBCRFactory  *IYCBCRFactoryObj;

  /* Allocate the object.*/
  if (!ppif)
    return EFAILED;

  IYCBCRFactoryObj = (IYCBCRFactory *)(void*)AEE_NewClassEx((IBaseVtbl*)
                      (void*)&gIYCbCrFactoryFuncs, sizeof(IYCBCRFactory), TRUE);

  if (!IYCBCRFactoryObj)
  {
    *ppif = NULL;
    return ENOMEMORY;
  }

  /* Initialize the values. */
  IYCBCRFactoryObj->m_pIShell = pIShell;
  (void)ISHELL_AddRef(pIShell);
  IYCBCRFactoryObj->m_cls = cls;
  IYCBCRFactoryObj->m_uRefs = 1;

  *ppif = IYCBCRFactoryObj;

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: YCBCRFACTORY_CREATE

DESCRIPTION:
  Initiate the IYCbCr object information in the IYCBCRFactory object.

DEPENDENCIES:
  None.

SIDE EFFECTS:
  None.

=============================================================================*/
static int YCbCrFactory_Create(IYCBCRFactory *pMe, uint32 uScheme, uint32 cx,
                               uint32 cy, IYCbCr **ppIYCbCr)
{
  int retVal;

  if(!pMe || !ppIYCbCr)
    return EBADPARM;

  retVal = YCbCr_New(uScheme, cx, cy, ppIYCbCr);

  if (retVal == SUCCESS)
  {
    if(*ppIYCbCr)
    {
      pMe->m_pIYCbCr = *ppIYCbCr;
    }
    else
    {
      retVal = ENOMEMORY;
    }
  }
  return retVal;
}


/*=============================================================================
FUNCTION: YCBCRFACTORY_ADDREF

DESCRIPTION:
  Add reference to the existing IYCBCRFactory object

DEPENDENCIES:
  None

SIDE EFFECTS:
  None.

=============================================================================*/
static uint32 YCbCrFactory_AddRef(IYCBCRFactory *pMe)
{
  if (pMe)
  {
    return (++pMe->m_uRefs);
  }
  return 0;
}


/*=============================================================================
FUNCTION: YCBCRFACTORY_RELEASE

DESCRIPTION:
  Release the IYCBCRFactory object and its members.

DEPENDENCIES:
  None.

SIDE EFFECTS:
  None.

=============================================================================*/
static uint32 YCbCrFactory_Release(IYCBCRFactory *pMe)
{
  if (!pMe)
    return 0;

  if (pMe->m_uRefs > 0)
  {
    if(--pMe->m_uRefs)
    {
      return pMe->m_uRefs;
    }
  }

  if (pMe->m_pIYCbCr)
  {
    //IYCbCr interface should be released by the caller
    pMe->m_pIYCbCr = NULL;
  }
  if (pMe->m_pIShell)
  {
    (void)ISHELL_Release(pMe->m_pIShell);
    pMe->m_pIShell = NULL;
  }

  FREE((void *)pMe); /*lint !e740 !e611 */

  return 0;
}

/*=============================================================================
FUNCTION: YCBCRFACTORY_QUERYINTERFACE

DESCRIPTION:
  Query interface for the IYCBCRFactory object

DEPENDENCIES:
  None.

SIDE EFFECTS:
  None.

=============================================================================*/
static int YCbCrFactory_QueryInterface(IYCBCRFactory *pMe, AEECLSID clsid,
                                       void **ppo)
{
  if (!ppo || !pMe)
    return EBADPARM;

  switch (clsid)
  {
  case AEECLSID_QUERYINTERFACE:
  //lint -fallthrough
  case AEECLSID_YCBCR:
    *ppo = pMe;
    (void)YCbCrFactory_AddRef(pMe);
    break;
  default:
    *ppo = NULL;
    break;
  }

  return (*ppo ? SUCCESS : ECLASSNOTSUPPORT);
}
