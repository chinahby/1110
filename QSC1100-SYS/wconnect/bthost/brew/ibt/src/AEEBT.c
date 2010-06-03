/*=============================================================================
FILE:  AEEBT.c

SERVICES:  Implementation of IBT and IBTNotifier

GENERAL DESCRIPTION:
   Implements IBT, IBTNotifier.

===============================================================================
        Copyright (c) 2007-2009 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBT.c#2 $ 
$DateTime: 2009/04/03 17:37:13 $
$Author: c_novinm $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2009-03-31  NM   Custom interface to set/clear bondable
2008-11-25  gb   Replaced usage of FEATURE_IBT_DYNAMIC_BMP_1_0 flag with
                 FEATURE_BMP.
2008-10-06  rs   Adding the feature flag to avoid inclucion of features not 
                 yet a part of BREW IBT release.
=============================================================================*/

#include "AEEStdLib.h"
#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#ifdef BT_SIMULATION
#include "btmsg.h"
#endif

#include "AEEBase.h"
#include "AEEBT.h"
#include "AEEBTMgr.bid"
#include "AEEBTConnection.bid"
#include "AEEBTServiceDiscovery.bid"
#include "AEEBTAudioGateway.bid"
#include "AEEBTAVRCPTarget.bid"
#include "AEEIBTAudioGateway.h"
#include "AEEBTSPPServer.h"
#include "AEEBTSPPClient.h"
#include "AEEBTDevDB.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEEBT.bid"
#include "OEMBT.h"
#include "AEEBTUtils.h"
#include "AEEModTable.h"
#include "BREWVersion.h"
#include "AEEBTA2DP.bid"
#include "AEEBTA2DPNotifier.bid"
#ifdef FEATURE_BT_AVRCP_13
#include "AEEAVRCPTG_Priv_Ext.bid"
#endif /* FEATURE_BT_AVRCP_13 */

#include "AEEOBEXCli.bid"
#include "AEEOBEXSrv.bid"
#include "AEERemoteControl_AVRCP_1_0_Target.bid"

#ifdef FEATURE_BMP
#include "AEEBTBPPSender.bid"
#include "AEEBTPBAPClient.bid"
#include "AEEBTPBAPServer.bid"
#include "AEEBTOPPServer.bid"
#include "AEEBTFTPServer.bid"
#include "AEEBTOPPClient.bid"
#include "AEEBTFTPServer.bid"
#include "AEEBTFTPClient.bid"
#include "AEEBTImagePushInitiator.bid"
#include "AEEBTImagePullInitiator.bid"
#include "AEEBTImagePrintInitiator.bid"
#include "AEEBTImagePushResponder.bid"
#include "AEEBTImagePullResponder.bid"
#endif /* FEATURE_BMP */

// generated using brew class id generator for test hook to set/get bondable 
#define AEEBT_CMD_SET_BONDABLE		0x0108bef5
#define AEEBT_CMD_GET_BONDABLE		0x0108c057

extern int AEEBTAG_New (IShell*, void**);
extern int AEEBTA2DP_New (IShell*, AEECLSID, void**);
extern int AEEBTA2DPNotifier_New (IShell*, AEECLSID, void**);
extern int AEEBTMgr_New (IShell*, void**);
extern int AEEBTConn_New (IShell*, const BDAddress*, void**);
extern int AEEBTSD_New (IShell*, void**);
extern int AEEBTSPPServer_New (IShell*, void**, const uint8*, const uint16,
                               BTServiceClassUUID);
extern int AEEBTSPPServerClone (IShell*, void**, int16);
extern int AEEBTSPPClient_New (IShell*, void**);
extern int AEEBTDEVDB_New (IShell*, void**);
extern int AEEBTAVRCP_New (IShell*, void**);
#ifdef FEATURE_BT_AVRCP_13
extern int AEEBTAVRCPMetadataTarget_New(IShell*, AEECLSID, void**);
#endif /* FEATURE_BT_AVRCP_13 */

#ifdef FEATURE_BMP
extern int AEEBTBPPSender_New(IShell*, void** );
extern int AEEBTPBAPClient_New (IShell*, void**);
extern int AEEBTPBAPServer_New (IShell*, void**);
extern int AEEBTOPPServer_New (IShell*, void**);
extern int AEEBTOPPClient_New (IShell*, void**);
extern int AEEBTFTPServer_New (IBTEnv*, AEECLSID, void**);
extern int AEEBTFTPClient_New (IBTEnv*, AEECLSID, void**);
extern int AEEBTBIPInitiator_New (IBTEnv*, AEECLSID, void**);
extern int AEEBTBIPResponder_New( IBTEnv*  pEnv, AEECLSID clsid, void**   ppif );
#endif /* FEATURE_BMP */

extern int IBT_New( IShell *pIShell, AEECLSID clsid, void **ppf );
extern int IBTNotifier_New( IShell *pIShell, AEECLSID clsid, void **ppf );
extern int AEEOBEXSrv_New(IShell* pIShell, AEECLSID cls,void **ppif);
extern int AEEOBEXCli_New(IShell* pIShell, AEECLSID cls,void **ppif);
extern int AEERemoteControl_New(IShell* piShell, AEECLSID cls, void** ppif);

extern const AEEStaticClass gBTClassList[] =
{
   {AEECLSID_BT,          ASCF_PRIV|ASCF_UPGRADE, 0,  NULL, IBT_New},
   {AEECLSID_BT_NOTIFIER, ASCF_UPGRADE,           0,  NULL, IBTNotifier_New},
   {AEECLSID_OBEXCli,     ASCF_UPGRADE, 0,  NULL, AEEOBEXCli_New},
   {AEECLSID_OBEXSrv,     ASCF_UPGRADE, 0,  NULL, AEEOBEXSrv_New},
   {AEECLSID_RemoteControl_AVRCP_1_0_Target, ASCF_UPGRADE, 0,  NULL, AEERemoteControl_New},
#ifdef FEATURE_BT_AVRCP_13
   {AEECLSID_AEEAVRCPTG_Priv_Ext, ASCF_UPGRADE, 0,  NULL, AEEBTAVRCPMetadataTarget_New},
#endif /* #ifdef FEATURE_BT_AVRCP_13*/
   {0,0,0,NULL,NULL}
};

/*=============================================================================
  IBT Definition
=============================================================================*/

struct IBT
{
   const IBTVtbl*   pvt;
   uint32           uRefs;
   OEMBT*           pOEMBT;
   IShell*          pIShell;
   AEECallback      cbAppClose;
};

static void   AEEBT_OnAppCloseCb( IBT* pMe );
static uint32 AEEBT_AddRef( IBT* po );
static uint32 AEEBT_Release( IBT* po );
static int    AEEBT_QueryInterface( IBT*     po, 
                                    AEECLSID clsid, 
                                    void**   ppo );

static int    AEEBT_Command( IBT*                 po, 
                             int                  cmd,
                             AEECallback*         pCb,
                             const void*          pInBuffer, 
                             int                  inBufSize, 
                             void*                pOutBuffer,
                             int                  outBufSize);

static int    AEEBT_GetObject( IBT*               po, 
                               AEECLSID           iId,
                               const void*        pInData,
                               int                size,
                               void**             ppObj);

static const IBTVtbl gVtblIBT = {
   AEEBT_AddRef,
   AEEBT_Release,
   AEEBT_QueryInterface,
   AEEBT_Command,
   AEEBT_GetObject
};



/*======================================================================= 
    Inherited from IBASE_AddRef(). Adds Reference count to the object
=======================================================================*/
static uint32 AEEBT_AddRef( IBT* pme )
{
   return( ++pme->uRefs );
}

/*======================================================================= 
     Inherited from IBASE_Release(). Decrements reference count and 
    releases the object when reference count becomes zero
=======================================================================*/
static uint32 AEEBT_Release( IBT* pme )
{
   uint32 dwRefs = 0;

   if ( 0 == pme->uRefs )
   {
      return pme->uRefs;
   }

   dwRefs = --pme->uRefs;

   if ( 0 == dwRefs )
   {
      AEEBT_OnAppCloseCb(pme);
   }

   return dwRefs;
}

/*======================================================================= 
    Inherited from IQI_QueryInterface().
    Returns a pointer to an available interface based on the input
    class ID
=======================================================================*/
static int AEEBT_QueryInterface( IBT* pme, AEECLSID clsid, void** ppo )
{

   int    result = ECLASSNOTSUPPORT;

   *ppo = NULL;
   switch ( clsid )
   {
   case AEECLSID_BASE:
   case AEECLSID_QUERYINTERFACE:
   case AEECLSID_BT:
      *ppo = (IBase*)pme;
      IBASE_AddRef( *ppo );
      result = SUCCESS;
      break;
   }

   return result;

}
/*======================================================================= 
   This function either returns local device info or any possible
   data associated  with a BT command
=======================================================================*/
static int AEEBT_Command( IBT*                pme, 
                          int                 cmd,
                          AEECallback*        pCb,
                          const void*         pInBuffer, 
                          int                 inBufSize, 
                          void*               pOutBuffer,
                          int                 outBufSize)
{
   int nErr = EUNSUPPORTED;

   switch ( cmd )
   {
   case AEEBT_CMD_GET_NOTIF_DATA:
      if ( (pInBuffer == NULL) || (pOutBuffer == NULL) )
      {
         nErr = EBADPARM;
      }
      else if ( (inBufSize  != sizeof( BTNotifyInfo )) ||
                (outBufSize != sizeof( BTEventData )) )
      {
         nErr = EVERSIONNOTSUPPORT;
      }
      else
      {
         nErr = OEMBT_GetNotificationData( (BTNotifyInfo*)pInBuffer, 
                                           (BTEventData*) pOutBuffer );
      }
      break;
   case AEEBT_CMD_GET_LOCAL_INFO:
      if ( pOutBuffer == NULL )
      {
         nErr = EBADPARM;
      }
      else if ( outBufSize != sizeof( BTLocalInfo ) )
      {
         nErr = EVERSIONNOTSUPPORT;
      }
      else
      {
         nErr = OEMBT_GetLocalInfo( (BTLocalInfo*) pOutBuffer );
      }
      break;
   case AEEBT_CMD_GET_RADIO_STATE:
      if ( pOutBuffer == NULL )
      {
         nErr = EBADPARM;
      }
      else
      {
         nErr = OEMBT_GetRadioState( (BTRadioState*) pOutBuffer );
      }
      break;
   case AEEBT_CMD_SET_BONDABLE:
      if ( pInBuffer == NULL )
      {
         nErr = EBADPARM;
      }
      else if ( inBufSize != sizeof( boolean ) )
      {
         nErr = EVERSIONNOTSUPPORT;
      }
      else
      {
	     nErr = OEMBT_SetBondable( (*(boolean*) pInBuffer) );
      }
      break;
   case AEEBT_CMD_GET_BONDABLE:
      if ( pOutBuffer == NULL )
      {
         nErr = EBADPARM;
      }
      else if ( outBufSize != sizeof( boolean ) )
      {
         nErr = EVERSIONNOTSUPPORT;
      }
      else
      {
         nErr = OEMBT_GetBondable( (boolean*) pOutBuffer );
      }
      break;
   default:
      break;
   }

   return nErr;
}

/*======================================================================= 
    This function returns Bluetooth interface object based on the 
   interface ID
=======================================================================*/
static int AEEBT_GetObject( IBT*        pme, 
                            AEECLSID    iId,
                            const void* pInData,
                            int         size,
                            void**      ppObj)
{

   int nErr = EUNSUPPORTED;
   *ppObj = NULL;

   switch ( iId )
   {
   case AEEIID_BT_DEV_DB:
      nErr = AEEBTDEVDB_New( pme->pIShell, ppObj );
      break;

   case AEEIID_BT_SPP_SERVER:
      {
         struct Serv
         {
            BTServiceClassUUID uuid;
            const uint8* pszServiceName;
            const uint16 version;
         };
         
         // incase the server is being cloned
         struct ServClone
         {
            int16  streamID;
         };

         if ( size == (sizeof(struct Serv)) )
         {
            const struct Serv* local = pInData;
            nErr = AEEBTSPPServer_New( pme->pIShell, ppObj,
                                       local->pszServiceName,
                                       local->version,
                                       local->uuid );
         }
         else if ( size == (sizeof(struct ServClone)) )
         {
            const struct ServClone* local = pInData;
            
            nErr = AEEBTSPPServerClone( pme->pIShell, ppObj, local->streamID );
         }
         break;
      }
   case AEEIID_BT_SPP_CLIENT:
      nErr = AEEBTSPPClient_New( pme->pIShell, ppObj );
      break;

   case AEECLSID_BTAudioGateway:
      nErr = AEEBTAG_New( pme->pIShell, ppObj );
      break;

   case AEECLSID_BTMgr:
      nErr = AEEBTMgr_New( pme->pIShell, ppObj );
      break;

   case AEECLSID_BTConnection:
      if (size == sizeof(BDAddress))
      {
         nErr = AEEBTConn_New( pme->pIShell, (BDAddress*) pInData, ppObj );
      }
      break;
   case AEECLSID_BTAVRCPTarget:
      nErr = AEEBTAVRCP_New( pme->pIShell, ppObj );
      break;

   case AEECLSID_BTA2DP:
      nErr = AEEBTA2DP_New( pme->pIShell, 0, ppObj );
      break;

   case AEECLSID_BTA2DPNotifier:
      nErr = AEEBTA2DPNotifier_New( pme->pIShell, 0, ppObj );
      break;

   case AEECLSID_BTServiceDiscovery:
      nErr = AEEBTSD_New( pme->pIShell, ppObj );
      break;

#ifdef FEATURE_BMP

   case AEECLSID_BTBPPSender:
      nErr = AEEBTBPPSender_New( pme->pIShell, ppObj );
      break;

   case AEECLSID_BTPBAPClient:
     nErr = AEEBTPBAPClient_New( pme->pIShell, ppObj );
     break; 

   case AEECLSID_BTPBAPServer:
      nErr = AEEBTPBAPServer_New( pme->pIShell, ppObj );
     break; 

   case AEECLSID_BTOPPServer:
      nErr = AEEBTOPPServer_New( pme->pIShell, ppObj );
      break;

   case AEECLSID_BTOPPClient:
      nErr = AEEBTOPPClient_New( pme->pIShell, ppObj );
      break;

   case AEECLSID_BTFTPServer:
   {
#ifdef USE_CS
#error code not present
#else
      nErr = AEEBTFTPServer_New (pme->pIShell, AEECLSID_BTFTPServer, ppObj);
#endif 
   }
      break;
   case AEECLSID_BTFTPClient:
   {
#ifdef USE_CS
#error code not present
#else
      nErr = AEEBTFTPClient_New (pme->pIShell, AEECLSID_BTFTPClient, ppObj);
#endif 
      break;
   }
   
   case AEECLSID_BTImagePushInitiator:
     nErr = AEEBTBIPInitiator_New (pme->pIShell, AEECLSID_BTImagePushInitiator,ppObj );
     break;

  case AEECLSID_BTImagePullInitiator:
     nErr = AEEBTBIPInitiator_New (pme->pIShell, AEECLSID_BTImagePullInitiator,ppObj );
     break;

  case AEECLSID_BTImagePrintInitiator:
	  nErr = AEEBTBIPInitiator_New (pme->pIShell, AEECLSID_BTImagePrintInitiator, ppObj );
	  break;

  case AEECLSID_BTImagePushResponder:
	  nErr = AEEBTBIPResponder_New (pme->pIShell, AEECLSID_BTImagePushResponder,ppObj );
     break;
  
  case AEECLSID_BTImagePullResponder:
	  nErr = AEEBTBIPResponder_New (pme->pIShell, AEECLSID_BTImagePullResponder,ppObj );
     break;

#endif /* FEATURE_BMP */  

   default :
      break;
   }

   return nErr;
}

/*======================================================================= 
    This function is called when either the reference count goes to zero 
   or the application exits without cleaning up.
=======================================================================*/
static void AEEBT_OnAppCloseCb( IBT* pMe )
{
   if ( pMe != NULL )
   {
      CALLBACK_Cancel( &pMe->cbAppClose );
      RELEASEIF( pMe->pIShell );
      OEMBT_Release( pMe->pOEMBT );
      FREEIF( pMe );
   }
}



/*=============================================================================
  IBTNotifier Definition
=============================================================================*/
typedef struct IBTNotifier IBTNotifier;

AEEINTERFACE(IBTNotifier)
{
   INHERIT_INotifier(IBTNotifier);
};

struct IBTNotifier
{
   const IBTNotifierVtbl*        pvt;
   uint32                        uRefs;
   IShell*                       pIShell;
   uint32                        dwNotify;
   OEMBT*                        pOEMBT;
   AEECallback                   cbOnBTEv;
   AEECallback                   cbExit;
};

/*======================================================================= 
=======================================================================*/
static uint32 AEEBTNotifier_AddRef( IBTNotifier* po );
static uint32 AEEBTNotifier_Release( IBTNotifier* po );

#if MIN_BREW_BUILD(3,1,4,7)
static void   AEEBTNotifier_SetMask( IBTNotifier* po, uint32* pdwMask );
#else
static void   AEEBTNotifier_SetMask( IBTNotifier* po, const uint32* pdwMask );
#endif //MIN_BREW_BUILD(3,1,4,7)



static const IBTNotifierVtbl  gVtblIBTNotifier = 
{
   AEEBTNotifier_AddRef,
   AEEBTNotifier_Release,
   AEEBTNotifier_SetMask
};

static IBTNotifier* pgBTNotifier = NULL;

/*======================================================================= 
=======================================================================*/
static void AEEBTNotifier_OnExitCb( IBTNotifier* pme )
{

   AEEBTNotifier_Release(pme);

}

/*======================================================================= 
=======================================================================*/
static void AEEBTNotifier_cbOnBTEv( IBTNotifier* pme )
{

   BTNotifyInfo sNotifyInfo;

   while ( SUCCESS == OEMBTNotifier_GetBTEv( &sNotifyInfo ) )
   {
      int result;

      if ( NMASK_BT & pme->dwNotify )
      {

         DBGPRINTF_LOW( "IBT ISHELL_Notify - ishell=%x ", pme->pIShell);

         if (NULL != pme->pIShell)
         {
            result = ISHELL_Notify( pme->pIShell, AEECLSID_BT_NOTIFIER, 
                                    NMASK_BT | (sNotifyInfo.event << 16 ) , 
                                    (void*)&sNotifyInfo.uTimeStamp );
            if ( result != SUCCESS )
            {
               DBGPRINTF_ERROR("NotifierCb - ev=%x r=%x", sNotifyInfo.event, result);
            }
         }
      }

   }
   // Re-register callback
   OEMBT_OnBTEv( pme->pOEMBT, &pme->cbOnBTEv );

}

/*======================================================================= 
=======================================================================*/
static uint32 AEEBTNotifier_AddRef( IBTNotifier* pme )
{
   return ++pme->uRefs;
}

/*======================================================================= 
=======================================================================*/
static uint32 AEEBTNotifier_Release( IBTNotifier* pme )
{
   uint32 uRefs = 0;

   DBGPRINTF_MED( "IBT NOTIFY release Ref count = 0x%x ", pme->uRefs);

   if (0 == pme->uRefs)
   {
      return pme->uRefs;
   }
   uRefs = --pme->uRefs;

   if ( uRefs == 0 )
   {
      OEMBT_NotifUnReg( pme->pOEMBT );
      pgBTNotifier = NULL;


      CALLBACK_Cancel( &pme->cbExit );
      CALLBACK_Cancel( &pme->cbOnBTEv );
      RELEASEIF( pme->pIShell );
      OEMBT_Release( pme->pOEMBT );
      FREEIF(pme);
   }
   return uRefs;

}

/*======================================================================= 
=======================================================================*/
#if MIN_BREW_BUILD(3,1,4,7)
static void AEEBTNotifier_SetMask( IBTNotifier* pme, uint32* pdwMasks )
#else
static void AEEBTNotifier_SetMask( IBTNotifier* pme, const uint32* pdwMasks )
#endif //MIN_BREW_BUILD(3,1,4,7)
{

   while ( 0 != *pdwMasks )
   {
      pme->dwNotify |= GET_NOTIFIER_MASK( *pdwMasks );
      pdwMasks++;
   }

}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */

/*======================================================================= 
   This function returns OEM BT object to the calling APP 
   if the privilege check passes
=======================================================================*/
int IBT_New( IShell* pIShell, AEECLSID clsid, void** ppif )
{
#if !defined(FEATURE_IBT_DYNAMIC) || !defined(FEATURE_BT)
   return ECLASSNOTSUPPORT;
#else
   IBT *pMe;

   if ( ISHELL_CheckPrivLevel( pIShell,clsid, TRUE ) != TRUE )
   {
      return EPRIVLEVEL;
   }

   if ( NULL == ppif )
   {
      return EBADPARM;
   }
   // allocate pMe
   pMe = MALLOCREC( IBT );

   if ( NULL == pMe )
   {
      return ENOMEMORY;
   }

   // get a ref to the OEM layer object
   if ( SUCCESS != OEMBT_New( &pMe->pOEMBT ) )
   {
      FREE( pMe );
      return EFAILED;
   }

   pMe->pvt      = &gVtblIBT;
   pMe->pIShell  = pIShell;
   pMe->uRefs    = 1;
   ISHELL_AddRef( pIShell );

   CALLBACK_Init( &pMe->cbAppClose, 
                  (PFNNOTIFY)AEEBT_OnAppCloseCb, pMe );
   AEE_LinkSysObject( &pMe->cbAppClose );

   *ppif = (void *)pMe;

   return SUCCESS;
#endif /* #if !defined(FEATURE_IBT_DYNAMIC) || !defined(FEATURE_BT) */
}

/*======================================================================= 
=======================================================================*/
int IBTNotifier_New( IShell* pIShell, AEECLSID clsid, void** ppif )
{
#if !defined(FEATURE_IBT_DYNAMIC) || !defined(FEATURE_BT)
   return ECLASSNOTSUPPORT;
#else
   int nErr = SUCCESS;

   if ( NULL == pgBTNotifier )
   {
      OEMBT *pif;

      if ( SUCCESS == (nErr = OEMBT_New( &pif )) )
      {
         pgBTNotifier = (IBTNotifier *)MALLOC( sizeof(IBTNotifier) );

         if ( NULL != pgBTNotifier )
         {
            pgBTNotifier->pvt      = &gVtblIBTNotifier;
            pgBTNotifier->pOEMBT   = pif;
            pgBTNotifier->uRefs    = 1;
            pgBTNotifier->pIShell  = pIShell;
            ISHELL_AddRef( pIShell );

            OEMBT_NotifReg(pgBTNotifier->pOEMBT);

            CALLBACK_Init( &pgBTNotifier->cbOnBTEv, 
                           (PFNNOTIFY)AEEBTNotifier_cbOnBTEv, pgBTNotifier );
            OEMBT_OnBTEv( pif, &pgBTNotifier->cbOnBTEv );

            CALLBACK_Init( &pgBTNotifier->cbExit, 
                           (PFNNOTIFY)AEEBTNotifier_OnExitCb, pgBTNotifier );
            ISHELL_OnExit( pgBTNotifier->pIShell, &pgBTNotifier->cbExit );
            *ppif = pgBTNotifier;
         }
         else
         {
            OEMBT_Release( pif );
            nErr = ENOMEMORY;
         }
      }
   }
   else
   {
      AEEBTNotifier_AddRef( (IBTNotifier *)pgBTNotifier );
      *ppif = pgBTNotifier;
   }

   DBGPRINTF_MED( "IBT NOTIFY Returning Obj refcount = 0x%x ", 
                  pgBTNotifier->uRefs);
   return nErr;
#endif /* #if !defined(FEATURE_IBT_DYNAMIC) || !defined(FEATURE_BT) */
}
