
/*===========================================================================

FILE:      AEEOBEX.c

SERVICES:  OBEX BREW extension

GENERAL DESCRIPTION: BREW AEE interface to the OBEX BREW extension .
        Most of the interface is implemented directly by the OEM layer.


PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEOBEX.c#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
  $Change: 815296 $

                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/26/08   pn     Removed relative path from inclusion of AEECLSID_MODEL.bid.
03/08/07   MH     Added disconnect response API
11/28/06   MH     Removed unused OEMOBEX_ClientInitData function
8/25/06    bh     compile errors fixed
7/28/06    bh     modified for obex client authentication
11/13/04   sivap  initial checkin. unit tested for IOBEXServer API
=============================================================================*/


#include "OEMFeatures.h"

#ifdef FEATURE_IOBEX

#include "AEEOBEX.h"
#include "AEEModel.h"
#include "AEEOBEX_priv.h"
#include "OEMOBEX.h"
#include "AEE_OEMHeap.h"
#include "AEE_OEM.h"

#include "AEEStdLib.h"

//#include "AEECLSID_MODEL.bid"
#ifdef FEATURE_BREW_3_0

#include "AEE_OEMDispatch.h"

#endif


//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================


//==========================================================================
//   Function prototypes
//==========================================================================

// IOBEX VTABLE
static uint32 AEEOBEX_AddRef( IOBEX* po );
static uint32 AEEOBEX_Release( IOBEX* po );

int AEEOBEX_ServerInitialize (IOBEX *pMe, uint32 obexHandle);
//==========================================================================
//   Static data
//==========================================================================



static const VTBL(IOBEXServer) gpvtIOBEXServer =
{
    AEEOBEX_AddRef,
    AEEOBEX_Release,
    AEEOBEX_QueryInterface,
    OEMOBEX_AuthResponse,
    OEMOBEX_GetSupportedTransports,
    OEMOBEX_ServerRegister,
    OEMOBEX_ServerDeregister,
    AEEOBEX_ServerInitialize,
    OEMOBEX_ServerAcceptConnection,
    OEMOBEX_ServerConfirmSetPath,
    OEMOBEX_ServerPutResponse,
    OEMOBEX_ServerGetResponse,
    OEMOBEX_ServerForceDisconnect,
    OEMOBEX_ServerAuthenticate,
    OEMOBEX_ServerDisconnectResponse
};

static const VTBL(IOBEXClient) gpvtIOBEXClient =
{
    AEEOBEX_AddRef,
    AEEOBEX_Release,
    AEEOBEX_QueryInterface,
    OEMOBEX_AuthResponse,
    OEMOBEX_GetSupportedTransports,
    OEMOBEX_ClientConnect,
    OEMOBEX_ClientDisconnect,
    OEMOBEX_ClientPut,
    OEMOBEX_ClientGet,
    OEMOBEX_ClientSetPath,
    OEMOBEX_ClientAbortOperation,
    OEMOBEX_ClientAuthenticate
};

/*===========================================================================

Function:  AEEOBEXServer_New()

Description:
This is called from the shell when an app tries to create an instance of a
class ID associated with this module.

Parameters:
IShell* pShell:  IShell pointer
AEECLSID cls:    Class to create
void** ppof:     pointer to created interface pointer

Return Value:  SUCCESS if object created and initialized
ENOMEMORY if unable to create object
EFAILED if unable to initialize object
ECLASSNOTSUPPORT if wrong class ID
EBADPARM if input pointers are NULL

Componts:      None

Side Effects:  None

===========================================================================*/
int AEEOBEXServer_New(IShell* ps, AEECLSID cls, void** ppif)
{
    IOBEX* pMe;

    if ( cls != AEECLSID_OBEXSERVER )
    {
        return ECLASSNOTSUPPORT;
    }

    if ( (ps == NULL) || (ppif == NULL) )
    {
        return EBADPARM;
    }

    *ppif = NULL;


    // allocate pMe  -- dont need this to be in system memory
    //Note that the corresponding OEM object however will need to be allocated
    //in system memory
    pMe = (IOBEX*) AEE_OEM_NEWCLASSEX( (IBaseVtbl*)&gpvtIOBEXServer,
                                 sizeof(IOBEX), FALSE);
    if ( pMe == NULL )
    {
        return ENOMEMORY;
    }

    pMe->m_pIShell = ps;

    //OEM Layer is intialized when app calls IOBEXServer_Register() or
    //via IOBEXServer_Initialize() (in the evt callback)

    // init me
  pMe->m_uRefs = 1;
    pMe->u.pvtS = &gpvtIOBEXServer;
    pMe->m_pIModel = NULL; //IModel will be created when the object is initialized


    *ppif = pMe;

    return SUCCESS;
}

int AEEOBEX_ServerInitialize (IOBEX *pMe, uint32 obexHandle)
{
  int nRetVal;
    if (obexHandle == 0)
    {
      return EFAILED; //this is illegal
    }
    else
    {
    //set the object's handle field using the given handle
    pMe->m_hOBEX = obexHandle;
    //then initalize the OEM Object with the new pMe

    if (OEMOBEX_Initialize(pMe) != SUCCESS)
    {
      return EFAILED;
    }

    //create the IModel instance that will be used during notifications
        //this model instance is removed when the app calls IOBEXServer_Release()
        nRetVal = ISHELL_CreateInstance(pMe->m_pIShell,AEECLSID_MODEL,
                                   (void **)&pMe->m_pIModel);
    if((nRetVal != SUCCESS) || (pMe->m_pIModel == NULL))
        {
      DBGPRINTF_ERROR("Failed to create an instance of IModel. cleaning up self");
            FREE(pMe);
            return EFAILED;
        }

    }
  return SUCCESS;
}

int AEEOBEXClient_New(IShell* ps, AEECLSID cls, void** ppif)
{
    IOBEX* pMe;
  int nRetVal;

    if ( cls != AEECLSID_OBEXCLIENT )
    {
        return ECLASSNOTSUPPORT;
    }

    if ( (ps == NULL) || (ppif == NULL) )
    {
        return EBADPARM;
    }

    *ppif = NULL;


    // allocate pMe -- in application memory
    pMe = (IOBEX*) AEE_OEM_NEWCLASS( (IBaseVtbl*)&gpvtIOBEXClient,
                                 sizeof(IOBEX));
    if ( pMe == NULL )
    {

        return ENOMEMORY;
    }

    pMe->m_pIShell = ps;

    nRetVal = ISHELL_CreateInstance(pMe->m_pIShell,AEECLSID_MODEL,
                               (void **)&pMe->m_pIModel);
  if((nRetVal != SUCCESS) || (pMe->m_pIModel == NULL))

    {
        FREE(pMe);
        return EFAILED;
    }
    // init OEM layer

    if (OEMOBEX_Initialize(pMe) != SUCCESS)
    {
        IMODEL_Release(pMe->m_pIModel);
        FREE( pMe );
        return EFAILED;
    }

    // init me
    //pMe->pvt = &gpvtIOBEX;
  pMe->m_uRefs = 1;
    pMe->u.pvtC = &gpvtIOBEXClient;



    *ppif = pMe;

    return SUCCESS;
}

//==========================================================================
//   IOBEX VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEOBEX_AddRef( IOBEX* pMe )
{
    // increment ref count
    if ( pMe != NULL )
    {
        return ++pMe->m_uRefs;
    }
    return 0;
}


 int AEEOBEX_QueryInterface(
                                 IOBEX* pMe,
                                 AEECLSID class,
                                 void** ppo
                                 )
{
    // pointer validation
    if (!AEE_CHECK_PTR("AEEOBEX_QueryInterface", ppo, sizeof(void*), TRUE))
    {
        return EMEMPTR;
    }

    switch (class)
    {
        case AEECLSID_BASE:
        case AEECLSID_QUERYINTERFACE:
        case AEECLSID_OBEXSERVER:
        case AEECLSID_OBEXCLIENT:
            *ppo = pMe;
            AEEOBEX_AddRef(*ppo);
            return SUCCESS;
        case AEEIID_MODEL:
            *ppo = pMe->m_pIModel;
            IMODEL_AddRef(*ppo);
      return SUCCESS;
            //AEEOBEX_AddRef(*ppo);
        default:
            *ppo = NULL;
            return ECLASSNOTSUPPORT;
    }
}

// IBase (see AEE.h)
static uint32 AEEOBEX_Release( IOBEX* pMe )
{
    // decrement ref count
    if ( pMe != NULL )
    {
        --pMe->m_uRefs;
        if ( pMe->m_uRefs > 0 )
        {
            return pMe->m_uRefs;
        }


        //if m_uRefs == 0, then lets release the corresponding OEM object
        if (OEMOBEX_Release(pMe) != SUCCESS)
        {
            // Uh oh. Release didn't work.
            ++pMe->m_uRefs;
            return EFAILED;
        }
        if (pMe->m_pIModel)
        {
            IMODEL_Release(pMe->m_pIModel);
            pMe->m_pIModel = NULL;
        }

        //if we are here, m_uRefs is == 0
        FREE(pMe);
    }

    return SUCCESS;
}


#endif /* FEATURE_IOBEX */
