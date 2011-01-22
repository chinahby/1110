/*======================================================
FILE:  AEEBTSPPServer.c

SERVICES:  Implementation of IBTSPPServer.

GENERAL DESCRIPTION:
   Implements IBTSPPServer

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTSPPServer.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#ifdef BT_SIMULATION
#include "btmsg.h"
#endif

#include "AEEBase.h"
#include "AEEStdLib.h"
#include "OEMBTSPP.h"
#include "AEEBTSPPServer.h"
#include "AEE_OEMDispatch.h"

/*======================================================================= 
   Defines
=======================================================================*/
#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(x)); (x) = 0; }

struct IBTSPPServer
{
   const IBTSPPServerVtbl*       pvt;
   uint32                        dwRefs;
   IShell*                       pIShell;
   AEECallback                   cbAppClose;
   BTHandle                      hBT; 
};


static uint32  AEEBTSPPSRV_AddRef( IBTSPPServer* po );
static uint32  AEEBTSPPSRV_Release( IBTSPPServer* po );
static int     AEEBTSPPSRV_QueryInterface( IBTSPPServer* po,
                                           AEECLSID clsid,
                                           void **ppo );
static int32   AEEBTSPPSRV_Read( IBTSPPServer* po, char* pcBuf, int32 cbBuf );
static void    AEEBTSPPSRV_Readable( IBTSPPServer* po, AEECallback* pcb );
static int     AEEBTSPPSRV_GetLastError( IBTSPPServer* po );
static int32   AEEBTSPPSRV_Write( IBTSPPServer* pme, const char* pcBuf, int32 cbBuf );
static void    AEEBTSPPSRV_Writeable( IBTSPPServer* pme, AEECallback* pcb );
static int     AEEBTSPPSRV_IOCtl( IBTSPPServer* po, int nOption, uint32 dwVal );
static int     AEEBTSPPSRV_Close( IBTSPPServer* po );
static int     AEEBTSPPSRV_Open( IBTSPPServer* po, const char* szPort );
static int     AEEBTSPPSRV_OnStatChange( IBTSPPServer* po, AEECallback* pRCb );
static int     AEEBTSPPSRV_OnModemStatusChange( IBTSPPServer*  po, 
                                                AEECallback*   pStatusCb);
static int     AEEBTSPPSRV_OnLineError( IBTSPPServer*  po, 
                                        AEECallback*   pLineErrorCb);

static int     AEEBTSPPSRV_Disconnect( IBTSPPServer* po,
                                       BTResult* pR,
                                       AEECallback* pRCb );
static int     AEEBTSPPSRV_GetStatus( IBTSPPServer* po, BTSPPStatus* pStatus);
static int     AEEBTSPPSRV_GetModemStatus(IBTSPPServer* po, BTSPPModemStatus* pStatus);
static int     AEEBTSPPSRV_GetLineError(IBTSPPServer* po, BTSPPLineError* pLineError);


static int     AEEBTSPPSRV_Configure( IBTSPPServer* po,
                                      const BTSPPConfig* pCfg,
                                      BTResult* pR,
                                      AEECallback* pRCb
                                    );

static int     AEEBTSPPSRV_IOCtlEx( IBTSPPServer* po,
                                    BTSPPIOCtlCommand IOctlCmd, 
                                    BTSPPModemStatus* pModemStatus,
                                    AEECallback* pResultCb);

static int     AEEBTSPPSRV_EnableService( IBTSPPServer* po,
                                          BTSecurityLevel secLevel,
                                          uint32  uServerChannel,
                                          boolean  bUnRegSdp,
                                          BTResult* pResult,
                                          AEECallback* pResultCb );

static int     AEEBTSPPSRV_DisableService( IBTSPPServer* po,
                                           BTResult* pResult, 
                                           AEECallback* pResultCb );

static int     AEEBTSPPSRV_OnConnRequest( IBTSPPServer*  po,
                                          AEECallback*   pCb,
                                          BTEventData*   pEv );

static int     AEEBTSPPSRV_Authorize( IBTSPPServer* po,
                                      const BDAddress* pBDAddr, 
                                      boolean bAccept,
                                      BTResult* pResult, 
                                      AEECallback* pResultCb );

void AEEBTSPPSRV_OnAppClose(IBTSPPServer* pData);

static const IBTSPPServerVtbl g_Vtbl_BTSPPServer = {
   AEEBTSPPSRV_AddRef,
   AEEBTSPPSRV_Release,
   AEEBTSPPSRV_QueryInterface,
   AEEBTSPPSRV_Read,
   AEEBTSPPSRV_Readable,
   AEEBTSPPSRV_GetLastError,
   AEEBTSPPSRV_Write,
   AEEBTSPPSRV_Writeable,
   AEEBTSPPSRV_IOCtl,
   AEEBTSPPSRV_Close,
   AEEBTSPPSRV_Open,
   AEEBTSPPSRV_OnStatChange,
   AEEBTSPPSRV_OnModemStatusChange,
   AEEBTSPPSRV_OnLineError,
   AEEBTSPPSRV_Disconnect,
   AEEBTSPPSRV_GetStatus,
   AEEBTSPPSRV_GetModemStatus,
   AEEBTSPPSRV_GetLineError,
   AEEBTSPPSRV_Configure,
   AEEBTSPPSRV_IOCtlEx,
   AEEBTSPPSRV_EnableService,
   AEEBTSPPSRV_DisableService,
   AEEBTSPPSRV_OnConnRequest,
   AEEBTSPPSRV_Authorize
};
/*=========================================================================
   Instantiates a new SPP server object based on the serivename,
   uuid and version. If successful output pointer(ppif) shall have 
   pointer to new SPP server object
===========================================================================*/
int AEEBTSPPServer_New(IShell* pIShell, void **ppif,
                       const uint8* pszServiceName, 
                       const uint16 version,
                       BTServiceClassUUID uuid)
{
   IBTSPPServer *pMe;

   if ( ISHELL_CheckPrivLevel( pIShell,AEEIID_BT_SPP_SERVER, TRUE ) != TRUE )
   {
      return EPRIVLEVEL;
   }

   if ( (NULL == ppif) || (NULL == pszServiceName) )
   {
      return EBADPARM;
   }

   // allocate pMe
   pMe = MALLOCREC( IBTSPPServer );

   if ( NULL  == pMe )
   {
      return ENOMEMORY;
   }

   // create the OEM layer object and initialise it
   if ( SUCCESS != OEMBTSPP_New( &pMe->hBT, pszServiceName,version, 
                                 uuid, TRUE ) )
   {
      FREE( pMe );
      return EFAILED;
   }

   pMe->pvt     = &g_Vtbl_BTSPPServer;
   pMe->pIShell = pIShell;
   pMe->dwRefs  = 0;
   ISHELL_AddRef( pIShell );

   // Initialise the cb that will be called in case the app closes
   CALLBACK_Init( &pMe->cbAppClose, AEEBTSPPSRV_OnAppClose, pMe );
   AEE_LinkSysObject( &pMe->cbAppClose );

   AEEBTSPPSRV_AddRef( pMe );

   *ppif = (void *)pMe;

   return SUCCESS;
}

/*=========================================================================
    Clones a SPP server object based on the stream ID. While a new AEE
    object is created, the underlying OEM object remains the same.
===========================================================================*/
int AEEBTSPPServerClone(IShell* pIShell, void **ppif,
                        int16              streamID)
{
   IBTSPPServer *pMe = NULL;

   if ( ISHELL_CheckPrivLevel( pIShell,AEEIID_BT_SPP_SERVER, TRUE ) != TRUE )
   {
      return EPRIVLEVEL;
   }

   // allocate pMe
   pMe = MALLOCREC( IBTSPPServer );

   if ( NULL  == pMe )
   {
      return ENOMEMORY;
   }

   // Get a handle to the OEM layer object based on stream ID
   if ( SUCCESS != OEMBTSPP_ServClone( &pMe->hBT, streamID ) )
   {
      FREE( pMe );
      return EFAILED;
   }

   pMe->pvt     = &g_Vtbl_BTSPPServer;
   pMe->pIShell = pIShell;
   pMe->dwRefs  = 0;
   ISHELL_AddRef( pIShell );

   // Initialise the cb that will be called in case the app closes
   CALLBACK_Init( &pMe->cbAppClose, AEEBTSPPSRV_OnAppClose, pMe );
   AEE_LinkSysObject( &pMe->cbAppClose );

   AEEBTSPPSRV_AddRef( pMe );

   *ppif = (void *)pMe;

   return SUCCESS;
}

/*=========================================================================
    Does the cleanup job when application using SPP object is closed
===========================================================================*/
void AEEBTSPPSRV_OnAppClose(IBTSPPServer* pme)
{
   if ( NULL != pme )
   {
      CALLBACK_Cancel( &pme->cbAppClose );

      RELEASEIF( pme->pIShell );

      OEMBTSPP_Release( pme->hBT );
      pme->hBT = NULL;

      FREEIF( pme );


   }
}

/*======================================================================= 
   Adds a reference count. Inherited from IPORT interface
=======================================================================*/
static uint32 AEEBTSPPSRV_AddRef( IBTSPPServer* pMe )
{
   // increment ref count
   if ( NULL != pMe )
   {
      return ++pMe->dwRefs;
   }
   return 0;
}

/*======================================================================= 
 Decrements a reference count and if count is zero, (means no application
 using the object)then releases the object.
=======================================================================*/
static uint32 AEEBTSPPSRV_Release( IBTSPPServer* pme )
{
   uint32 dwRefs;

   if ( pme->dwRefs == 0 )
   {
      return pme->dwRefs;
   }

   dwRefs = --pme->dwRefs;

   if ( 0 == dwRefs )
   {
      AEEBTSPPSRV_OnAppClose(pme);
   }

   return dwRefs;
}
/*======================================================================= 
 This function retrieves a pointer to an interface conforming to the 
 definition of the specified class ID. 
=======================================================================*/
static int   AEEBTSPPSRV_QueryInterface( IBTSPPServer* pme,
                                         AEECLSID clsid, void **ppo )
{
   if (( NULL == ppo ) || (NULL == pme ))
   {
      return EBADPARM;
   }

   if ( !AEE_CHECK_PTR("AEEBTSPPSRV_QueryInterface", ppo, sizeof(void*), TRUE))
   {
      return EMEMPTR;
   }

   if ( ( AEECLSID_QUERYINTERFACE == clsid ) ||
        ( AEECLSID_BASE == clsid )           ||
        ( AEECLSID_PORT == clsid)            ||
        ( AEEIID_BTSPP == clsid )            ||
        (AEEIID_BT_SPP_SERVER == clsid ))
   {
      *ppo = (void*)pme;
      IBASE_AddRef( *ppo );
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}

/*======================================================================= 
 Retrieve the received data from remote device.
=======================================================================*/
static int32 AEEBTSPPSRV_Read( IBTSPPServer* po, char* pcBuf, int32 cbBuf )
{
   return OEMBTSPP_Read( po->hBT, pcBuf, cbBuf );

}

/*======================================================================= 
   Inherited from IPORT_Readable().
   Registers a function to be invoked when receive queue has data.
=======================================================================*/
static void AEEBTSPPSRV_Readable( IBTSPPServer* po, AEECallback* pcb )
{
   OEMBTSPP_Readable( po->hBT, pcb ,po->pIShell);
   return;
}

/*======================================================================= 
   Inherited from IPORT_GetLastError().
   This function returns the last error that occurred with Read() or 
   Write().
=======================================================================*/
static int AEEBTSPPSRV_GetLastError( IBTSPPServer* po )
{
   return OEMBTSPP_GetLastError( po->hBT );
}

/*======================================================================= 
  Inherited from IPORT_Write().
  This function sends data to remote device.
=======================================================================*/
static int32 AEEBTSPPSRV_Write( IBTSPPServer* pme,
                                const char* pcBuf,
                                int32 cbBuf)
{
   return OEMBTSPP_Write( pme->hBT, (const char *)pcBuf, cbBuf );
}

/*======================================================================= 
  Inherited from IPORT_Writeable().
  Registers a callback to be invoked when transmit queue can accept more 
  data to be sent out. 
=======================================================================*/
static void AEEBTSPPSRV_Writeable( IBTSPPServer* pme, AEECallback* pcb )
{
   OEMBTSPP_Writable( pme->hBT, pcb, pme->pIShell );
   return;
}

/*======================================================================= 
  Inherited from IPORT_IOCtl().
  This API is currently not supported. Please check IPORT_IOCtlEx()
=======================================================================*/
static int AEEBTSPPSRV_IOCtl( IBTSPPServer* po, int nOption, uint32 dwVal )
{
   return ECLASSNOTSUPPORT;
}

/*======================================================================= 
 Inherited from IPORT_Close().
 This function is not supported.  Instead, DisableService() 
 should be used by server application
=======================================================================*/
static int AEEBTSPPSRV_Close( IBTSPPServer* po )
{
   return ECLASSNOTSUPPORT;
}

/*======================================================================= 
 Inherited from IPORT_Open().
 This function is not supported.  Instead, AEEBTSPPSRV_EnableService() 
 should be used by server application
=======================================================================*/
static int AEEBTSPPSRV_Open( IBTSPPServer* po, const char* szPort )
{
   return ECLASSNOTSUPPORT;
}

/*======================================================================= 
 This API registers a callback to be invoked when there's a change in the
 SPP stream state associated with the SPP object.
=======================================================================*/
static int AEEBTSPPSRV_OnStatChange( IBTSPPServer* po, AEECallback* pRCb )
{
   return OEMBTSPP_OnStatusChange( po->hBT, pRCb,
                                   OEMBTSPP_STATE_CHANGE,
                                   po->pIShell);
}

/*======================================================================= 
 This API registers a callback to be invoked when there's a change in the
 modem status associated with the SPP object.
=======================================================================*/
static int AEEBTSPPSRV_OnModemStatusChange(IBTSPPServer*  po,
                                           AEECallback*   pStatusCb)
{
   return OEMBTSPP_OnStatusChange( po->hBT, pStatusCb,
                                   OEMBTSPP_MODEM_CHANGE, po->pIShell );
}

/*======================================================================= 
 This API registers a callback to be invoked when a line error occurs on the
 SPP stream associated with the SPP object. 
=======================================================================*/
static int AEEBTSPPSRV_OnLineError(IBTSPPServer* po,
                                   AEECallback*  pLineErrorCb)
{
   return OEMBTSPP_OnStatusChange( po->hBT,
                                   pLineErrorCb,
                                   OEMBTSPP_LINE_ERROR,
                                   po->pIShell );
}

/*======================================================================= 
  This API terminates the SPP connection
=======================================================================*/
static int AEEBTSPPSRV_Disconnect( IBTSPPServer* po,
                                   BTResult* pR,
                                   AEECallback* pRCb )
{
   return OEMBTSPP_Disconnect( po->hBT, pR, pRCb );
}

/*======================================================================= 
    This API retrieves the stream's current status.  The streamID field of 
   the status indicates the stream whose status is being retrieved.
=======================================================================*/
static int AEEBTSPPSRV_GetStatus( IBTSPPServer* po, BTSPPStatus* pStatus)
{
   return OEMBTSPP_GetStatus( po->hBT, pStatus );

}

/*======================================================================= 
    This API retrieves the stream's current modem status.  
   The streamID field of the status indicates the stream whose
   modemstatus is being retrieved.
=======================================================================*/
static int AEEBTSPPSRV_GetModemStatus(IBTSPPServer* po,
                                      BTSPPModemStatus* pStatus)
{
   return OEMBTSPP_GetModemStatus( po->hBT, pStatus);

}

/*======================================================================= 
    This API retrieves the stream's current line error .  
   The streamID field of the status indicates the stream
   whose line error is being retrieved.
=======================================================================*/
static int AEEBTSPPSRV_GetLineError(IBTSPPServer* po,
                                    BTSPPLineError* pLineError)
{
   return OEMBTSPP_GetLineError( po->hBT, pLineError);

}

/*======================================================================= 
    This API configures the stream.  Note that the configuration does not 
   actually affect the connection, only the configuration settings are sent 
   to the remote device.
=======================================================================*/
static int AEEBTSPPSRV_Configure( IBTSPPServer* po,
                                  const BTSPPConfig* pConfig,
                                  BTResult* pR,
                                  AEECallback* pRCb)
{
   return OEMBTSPP_Configure( po->hBT, pConfig, pR, pRCb );
}

/*======================================================================= 
    This API controls the stream.
=======================================================================*/
static int AEEBTSPPSRV_IOCtlEx( IBTSPPServer *po, BTSPPIOCtlCommand IOctlCmd, 
                                BTSPPModemStatus* pModemStatus, 
                                AEECallback* pResultCb)
{
   return OEMBTSPP_IOCtlEx( po->hBT, IOctlCmd, pModemStatus, 
                            pResultCb, po->pIShell);

}

/*======================================================================= 
    This method opens up an SPP stream that listens for a connection
   request from an SPP client and advertises SPP service to inquiring
   devices.
=======================================================================*/
static int AEEBTSPPSRV_EnableService( IBTSPPServer*   po,
                                      BTSecurityLevel secLevel,
                                      uint32          uServerChannel,
                                      boolean         bUnRegSdp,
                                      BTResult*       pResult,
                                      AEECallback*    pResultCb )
{
   return OEMBTSPPSRV_EnableService( po->hBT, secLevel,
                                     uServerChannel,bUnRegSdp,
                                     pResult, pResultCb);
}


/*======================================================================= 
    This method closes the SPP stream associated with this SPP server.
   It also cancels the Authorization callback.
=======================================================================*/
static int  AEEBTSPPSRV_DisableService( IBTSPPServer* po, BTResult* pResult, 
                                        AEECallback* pResultCb )
{
   return OEMBTSPPSRV_DisableService( po->hBT, pResult, pResultCb );

}

/*======================================================================= 
   This method allows the server application to be notified when a client
   tries to connect to it. 
=======================================================================*/
static int AEEBTSPPSRV_OnConnRequest( IBTSPPServer*  po,
                                      AEECallback*   pCb,
                                      BTEventData*   pEv )
{
   return OEMBTSPPSRV_OnConnRequest( po->hBT, pCb, pEv );

}

/*======================================================================= 
   This method enables application implementing the SPP server role to either
   accept or reject the connection request from the client whose BD address 
   is given.. 
=======================================================================*/
static int AEEBTSPPSRV_Authorize( IBTSPPServer*     po,
                                  const BDAddress*  pBDAddr,
                                  boolean           bAccept,
                                  BTResult*         pResult,
                                  AEECallback*      pResultCb )
{
   return OEMBTSPPSRV_Authorize(po->hBT,
                                pBDAddr,
                                bAccept,
                                pResult,
                                pResultCb);

}

#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */

