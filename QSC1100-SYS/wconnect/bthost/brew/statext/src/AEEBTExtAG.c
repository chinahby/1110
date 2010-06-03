/*===========================================================================

FILE:      AEEBTExtAG.c

SERVICES:  BlueTooth Audio Gateway QCT Extension

GENERAL DESCRIPTION: AEE BREW interface to the Bluetooth audio gateway.
        Most of the functionality of this module is implemented in SVC mode
        in OEMBTExtAG.c

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtAG.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#ifdef FEATURE_IBT

#include "AEEBTExt_priv.h"
#include "OEMBTExtAG.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtAG VTABLE
static uint32  AEEBTExtAG_AddRef( IBTExtAG* po );
static uint32  AEEBTExtAG_Release( IBTExtAG* po );
static int     AEEBTExtAG_QueryInterface( IBTExtAG* po, AEECLSID class, 
                                          void** ppo );

// Helper functions
static void    AEEBTExtAG_OnExitCb( IBTExtAG* pMe );

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtAG) gpvtIBTExtAG =
{
  AEEBTExtAG_AddRef, 
  AEEBTExtAG_Release,
  AEEBTExtAG_QueryInterface,
  OEMBTExtAG_Config,
  OEMBTExtAG_Enable,
  OEMBTExtAG_Connect,
  OEMBTExtAG_Ring,
  OEMBTExtAG_SetVRCapable,
  OEMBTExtAG_UpdateVRState,
  OEMBTExtAG_SetSpkrGain,
  OEMBTExtAG_SetMicGain,
  OEMBTExtAG_ConnectAudio,
  OEMBTExtAG_DisconnectAudio,
  OEMBTExtAG_Disconnect,
  OEMBTExtAG_Disable,
  OEMBTExtAG_GetDialedString,
  OEMBTExtAG_EnableExternalIO,
  OEMBTExtAG_DisableExternalIO,
  OEMBTExtAG_RetrieveCommand,
  OEMBTExtAG_SendResponse,
  OEMBTExtAG_SetInbandRingCapable,
  OEMBTExtAG_SetBatteryChargeIndicator,
  OEMBTExtAG_SetRSSI
};

static IBTExtAG*     pIBTExtAGobj = NULL;

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtAG_New()

Description:
   This is called from the shell when an app tries to create an instance of a
   class ID associated with this module.

Parameters:
   IShell* pShell:  IShell pointer
   AEECLSID cls:    Class to create
   void** ppof:     pointer to created interface pointer

Return Value:  None

Componts:      None

Side Effects:  None

===========================================================================*/
int AEEBTExtAG_New(IShell* ps, AEECLSID cls, void** ppif)
{
  IBTExtAG* pNew;

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  if ( ! AEE_CHECK_PTR( "AEEBTExtAG_New", ppif, sizeof(void*), TRUE))
  {
    return EMEMPTR;
  }

  if ( AEECLSID_BLUETOOTH_AG != cls )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( pIBTExtAGobj == NULL )
  {
    // allocate the object
    pNew = (IBTExtAG*) AEE_NewClassEx( (IBaseVtbl*)&gpvtIBTExtAG, 
                                       sizeof(IBTExtAG), TRUE );
    if ( pNew == NULL )
    {
      return ENOMEMORY;
    }

    // init me
    pNew->m_pIShell = ps;
    pNew->m_uRefs   = 1;
    pNew->m_cls     = cls;

    // init BT layer
    if( OEMBTExtAG_Init( pNew ) != SUCCESS )
    {
      FREE( pNew );
      return EFAILED;
    }

    CALLBACK_Init( &pNew->m_cbExit, AEEBTExtAG_OnExitCb, pNew );
    ISHELL_OnExit( pNew->m_pIShell, &pNew->m_cbExit );

    pIBTExtAGobj = pNew;
  }
  else
  {
    AEEBTExtAG_AddRef( pIBTExtAGobj );
  }

  *ppif = pIBTExtAGobj;
  
  return SUCCESS;
}

//==========================================================================
//   Helper Funtions
//==========================================================================

static void AEEBTExtAG_OnExitCb( IBTExtAG* pMe )
{
  if ( pIBTExtAGobj != NULL )
  {
    CALLBACK_Cancel( &pIBTExtAGobj->m_cbExit );
    (void)OEMBTExtAG_Release( pIBTExtAGobj );
    FREE( pIBTExtAGobj );
    pIBTExtAGobj = NULL;
  }
}

//==========================================================================
//   IBTExtAG VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtAG_AddRef( IBTExtAG* pMe )
{
  if ( pMe != NULL )
  {
    // increment ref count
    return ++pMe->m_uRefs;
  }

  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtAG_Release( IBTExtAG* pMe )
{
  if ( pMe != NULL )
  {
    // decrement ref count
    if ( pMe->m_uRefs > 0 )
    {
      --pMe->m_uRefs;
    }

    // outstanding references?
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    } 
  
    // Ref count is zero.  clean up
    AEEBTExtAG_OnExitCb( pMe );
  }
  
  return SUCCESS;
}

// IQI (see AEE.h)
static int AEEBTExtAG_QueryInterface(
  IBTExtAG* pMe, 
  AEECLSID  cls, 
  void**    ppo
)
{
  if ( (pMe == NULL) || (ppo == NULL) )
  {
    return EBADPARM;
  }

  if (!AEE_CHECK_PTR("AEEBTExtAG_QueryInterface", ppo, sizeof(void*), TRUE))
  {
    return EMEMPTR;
  }
  
  switch (cls)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_AG:
       *ppo = pMe;
       AEEBTExtAG_AddRef(*ppo);
       return SUCCESS;
    default:
       *ppo = NULL;
       return ECLASSNOTSUPPORT;
  }
}

#endif /* FEATURE_IBT */
