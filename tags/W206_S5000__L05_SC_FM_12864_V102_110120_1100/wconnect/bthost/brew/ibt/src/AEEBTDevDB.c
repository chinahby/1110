/*============================================================================
FILE:  AEEBTDevDB.c

SERVICES:  Implementation of IBTDevDB interface

GENERAL DESCRIPTION:
   Implements IBTDevDB

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTDevDB.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTDevDB.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
$Author: deepikas $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2008-10-02  rs   Removing the privilege checks for devdb instantiation.
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#ifdef BT_SIMULATION
#include "btmsg.h"
#endif

#include "AEEBase.h"
#include "AEEStdLib.h"
#include "AEEBT.BID"
#include "AEEBTDevDB.h"
#include "OEMBTDevDB.h"
#include "AEE_OEMDispatch.h"

/*======================================================================= 
   Defines
=======================================================================*/
#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(x)); (x) = 0; }

struct IBTDevDB
{
   const IBTDevDBVtbl            *pvt;
   uint32                        dwRefs;
   OEMBTDEV_ITERATOR             iterator;
   IShell                        *pIShell;
   AEECallback                   cbAppClose;
   BTHandle                      hBT;

};


static void    AEEBTDevDB_OnAppCloseCb(IBTDevDB* pMe);
static uint32  AEEBTDevDB_AddRef( IBTDevDB* po );
static uint32  AEEBTDevDB_Release( IBTDevDB* po );
static int     AEEBTDevDB_QueryInterface( IBTDevDB* po,
                                          AEECLSID clsid,
                                          void **ppo );

static BTResult AEEBTDevDB_Add( IBTDevDB* po,
                                const BTDeviceInfo* pDeviceInfo);

static BTResult AEEBTDevDB_Read( IBTDevDB* po,
                                 BTDBEntry* pEntry);

static BTResult AEEBTDevDB_Update( IBTDevDB* po,
                                   BTModDevField fields2Update,
                                   const BTDBEntry* pEntry
                                 );

static BTResult AEEBTDevDB_EnumInit( IBTDevDB* po,
                                     const BTDevDBEnumerator* pEnumerator);

static BTResult AEEBTDevDB_EnumNext( IBTDevDB* po,
                                     BTDBEntry* pEntry);

static int AEEBTDevDB_Bond( IBTDevDB* po,
                            const BDAddress* pBDAddr,
                            const uint8* pszPin,
                            boolean bReplying,
                            BTResult* pResult,
                            AEECallback* pResultCb);

//==========================================================================
//   Static data
//==========================================================================
static const IBTDevDBVtbl g_Vtbl_IBTDevDB = {
   AEEBTDevDB_AddRef,
   AEEBTDevDB_Release,
   AEEBTDevDB_QueryInterface,
   AEEBTDevDB_Add,
   AEEBTDevDB_Read,
   AEEBTDevDB_Update,
   AEEBTDevDB_EnumInit,
   AEEBTDevDB_EnumNext,
   AEEBTDevDB_Bond
};

int AEEBTDEVDB_New(IShell* pIShell, void **ppif )
{
   IBTDevDB *pMe;

   if ( NULL == ppif )
   {
      return EBADPARM;
   }
   // allocate pMe
   pMe = MALLOCREC( IBTDevDB );

   if ( NULL  == pMe )
   {
      return ENOMEMORY;
   }

   // get a ref to the OEM layer object
   if ( SUCCESS != OEMBTDEVDB_New( &pMe->hBT ) )
   {
      FREE( pMe );
      return EFAILED;
   }

   pMe->pvt     = &g_Vtbl_IBTDevDB;
   pMe->pIShell = pIShell;
   pMe->dwRefs  = 1;

   pMe->iterator.index = DEV_DB_INVALID_INDX;
   MEMSET( (void*)(&pMe->iterator.devEnum), 0, sizeof( BTDevDBEnumerator ) );

   ISHELL_AddRef( pIShell );

   // Initialise the cb that will be called in case the app closes
   CALLBACK_Init( &pMe->cbAppClose, AEEBTDevDB_OnAppCloseCb, pMe );
   AEE_LinkSysObject( &pMe->cbAppClose );

   *ppif = (void *)pMe;

   return SUCCESS;
}

static uint32  AEEBTDevDB_AddRef( IBTDevDB* pMe )
{
   // increment ref count
   if ( pMe != NULL )
   {
      return ++pMe->dwRefs;
   }
   return 0;
}

void AEEBTDevDB_OnAppCloseCb(IBTDevDB* pMe)
{
   // called when either the ref count goes to zero 
   // or the application exits without cleaning up.
   if ( pMe != NULL )
   {
      CALLBACK_Cancel( &pMe->cbAppClose );
      RELEASEIF( pMe->pIShell );
      OEMBTDevDB_Release( pMe->hBT );
      FREEIF( pMe );
   }
}

static uint32  AEEBTDevDB_Release( IBTDevDB* pMe )
{
   uint32 dwRefs = 0;

   if ( pMe->dwRefs == 0 )
   {
      return dwRefs;
   }

   dwRefs = --pMe->dwRefs;

   if ( 0 == dwRefs )
   {
      AEEBTDevDB_OnAppCloseCb(pMe);
   }

   return dwRefs;
}

static int AEEBTDevDB_QueryInterface( IBTDevDB* pme,
                                      AEECLSID clsid,
                                      void **ppo )
{
   if (( NULL == ppo ) || (NULL == pme ))
   {
      return EBADPARM;
   }

   if ( !AEE_CHECK_PTR("AEEBTDevDB_QueryInterface", ppo, sizeof(void*), TRUE) )
   {
      return EMEMPTR;
   }

   if ( ( AEECLSID_QUERYINTERFACE == clsid ) ||
        ( AEECLSID_BASE == clsid )           ||
        ( AEEIID_BT_DEV_DB == clsid ))
   {
      *ppo = (IBase*)pme;
      IBASE_AddRef( *ppo );
      return SUCCESS;
   }
   return ECLASSNOTSUPPORT;
}

static BTResult AEEBTDevDB_Add( IBTDevDB* po,
                                const BTDeviceInfo* pDeviceInfo )
{
   if ( ISHELL_CheckPrivLevel( po->pIShell, 
                               AEEPRIVID_BT_DEV_WRITE, TRUE ) == FALSE )
   {
      return EPRIVLEVEL;
   }

   return OEMBTDevDB_Add( po->hBT, pDeviceInfo,po->pIShell );
}

static BTResult AEEBTDevDB_Read( IBTDevDB* po,
                                 BTDBEntry* pEntry)
{
   return OEMBTDevDB_Read( po->hBT, pEntry );
}

static BTResult AEEBTDevDB_Update( IBTDevDB* po,
                                   BTModDevField fields2Update,
                                   const BTDBEntry* pEntry
                                 )
{
   if ( ISHELL_CheckPrivLevel (po->pIShell, 
                               AEEPRIVID_BT_DEV_WRITE, TRUE ) != TRUE )
   {
      return EPRIVLEVEL;
   }

   return OEMBTDevDB_Update( po->hBT, fields2Update, pEntry, po->pIShell);
}

static BTResult AEEBTDevDB_EnumInit( IBTDevDB* po,
                                     const BTDevDBEnumerator* pEnumerator)
{
   if ( ( NULL == pEnumerator ) ||
        ( NULL == po ) ||
        ( !AEE_CHECK_PTR( "AEEBTDevDB_EnumInit", pEnumerator, 
                          sizeof(BTDevDBEnumerator), TRUE)))
   {
      return AEEBT_RESULT_BAD_PARAM;
   }

   po->iterator.index = 0;
   po->iterator.devEnum = *pEnumerator;

   return AEEBT_RESULT_SUCCESS;
}

static BTResult AEEBTDevDB_EnumNext( IBTDevDB* po,
                                     BTDBEntry* pEntry)
{
   return OEMBTDevDB_EnumNext( po->hBT, pEntry, &po->iterator );

}

static int AEEBTDevDB_Bond( IBTDevDB* po,
                            const BDAddress* pBDAddr,
                            const uint8* pszPin,
                            boolean bReplying,
                            BTResult* pResult,
                            AEECallback* pResultCb)
{
   return OEMBTDevDB_Bond( po->hBT,
                           pBDAddr,
                           pszPin,
                           bReplying,
                           pResult,
                           pResultCb );
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */

