/*======================================================
FILE:  AEEBTSPPClient.c

SERVICES:  Implementation of  IBTSPPClient

GENERAL DESCRIPTION:
   Implements IBTSPPClient

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTSPPClient.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#ifdef BT_SIMULATION
#include "btmsg.h"
#endif

#include "AEEBase.h"
#include "AEEStdLib.h"
#include "AEEBTSPPClient.h"
#include "OEMBTSPP.h"
#include "AEE_OEMDispatch.h"

/*======================================================================= 
   Defines
=======================================================================*/
#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(x)); (x) = 0; }

struct IBTSPPClient
{
   const IBTSPPClientVtbl*  pvt;
   uint32                   dwRefs;
   IShell*                  pIShell;
   AEECallback              cbAppClose;
   BTHandle                 hBT; 
};


// Client APIs
static uint32  AEEBTSPPCLI_AddRef( IBTSPPClient* po );
static uint32  AEEBTSPPCLI_Release( IBTSPPClient* po );
static int     AEEBTSPPCLI_QueryInterface( IBTSPPClient* po, AEECLSID clsid, void **ppo );
static int32   AEEBTSPPCLI_Read( IBTSPPClient* po, char* pcBuf, int32 cbBuf );
static void    AEEBTSPPCLI_Readable( IBTSPPClient* po, AEECallback* pcb );
static int     AEEBTSPPCLI_GetLastError( IBTSPPClient* po );
static int32   AEEBTSPPCLI_Write( IBTSPPClient* pme, const char* pcBuf, int32 cbBuf );
static void    AEEBTSPPCLI_Writeable( IBTSPPClient* pme, AEECallback* pcb );
static int     AEEBTSPPCLI_IOCtl( IBTSPPClient* po, int nOption, uint32 dwVal );
static int     AEEBTSPPCLI_Close( IBTSPPClient* po );
static int     AEEBTSPPCLI_Open( IBTSPPClient* po, const char* szPort );
static int     AEEBTSPPCLI_OnStatChange( IBTSPPClient* po, AEECallback* pRCb );
static int     AEEBTSPPCLI_OnModemStatusChange( IBTSPPClient*  po, 
                                                AEECallback*   pStatusCb);
static int     AEEBTSPPCLI_OnLineError( IBTSPPClient*  po, 
                                        AEECallback*   pLineErrorCb);

static int     AEEBTSPPCLI_Disconnect( IBTSPPClient* po, BTResult* pR, AEECallback* pRCb );
static int     AEEBTSPPCLI_GetStatus( IBTSPPClient* po, BTSPPStatus* pS );
static int     AEEBTSPPCLI_GetModemStatus(IBTSPPClient* po, BTSPPModemStatus* pStatus);
static int     AEEBTSPPCLI_GetLineError(IBTSPPClient* po, BTSPPLineError* pLineError);

static int     AEEBTSPPCLI_Configure( IBTSPPClient* po, const BTSPPConfig* pCfg,
                                      BTResult* pR, AEECallback* pRCb);

static int     AEEBTSPPCLI_IOCtlEx( IBTSPPClient* po, 
                                    BTSPPIOCtlCommand IOctlCmd,
                                    BTSPPModemStatus* pModemStatus, 
                                    AEECallback* pResultCb);

static int     AEEBTSPPCLI_FindServersInit( IBTSPPClient* po,
                                            uint32        searchDuration,
                                            uint32        maxServers);

static int     AEEBTSPPCLI_FindServersNext( IBTSPPClient*      po, 
                                            uint32*            pSize,
                                            AEECallback*       pCb);

static int     AEEBTSPPCLI_GetServerInfo( IBTSPPClient*      po, 
                                          BTSPPDeviceInfo*   pServerInfo);

static int     AEEBTSPPCLI_Connect( IBTSPPClient*      po,
                                    const BDAddress*   pBDAddr,
                                    uint32             uServerChannel,
                                    BTResult*          pResult,
                                    AEECallback*       pResultCb);

static int    AEEBTSPPCLI_CancelServerSearch(IBTSPPClient*      po,
                                             AEECallback*  pCb );

void AEEBTSPPCLI_OnAppClose(IBTSPPClient* pData);


static const IBTSPPClientVtbl g_Vtbl_BTSPPClient = {
   AEEBTSPPCLI_AddRef,
   AEEBTSPPCLI_Release,
   AEEBTSPPCLI_QueryInterface,
   AEEBTSPPCLI_Read,
   AEEBTSPPCLI_Readable,
   AEEBTSPPCLI_GetLastError,
   AEEBTSPPCLI_Write,
   AEEBTSPPCLI_Writeable,
   AEEBTSPPCLI_IOCtl,
   AEEBTSPPCLI_Close,
   AEEBTSPPCLI_Open,
   AEEBTSPPCLI_OnStatChange,
   AEEBTSPPCLI_OnModemStatusChange,
   AEEBTSPPCLI_OnLineError,
   AEEBTSPPCLI_Disconnect,
   AEEBTSPPCLI_GetStatus,
   AEEBTSPPCLI_GetModemStatus,
   AEEBTSPPCLI_GetLineError,
   AEEBTSPPCLI_Configure,
   AEEBTSPPCLI_IOCtlEx,
   AEEBTSPPCLI_FindServersInit,
   AEEBTSPPCLI_FindServersNext,
   AEEBTSPPCLI_GetServerInfo,
   AEEBTSPPCLI_Connect,
   AEEBTSPPCLI_CancelServerSearch
};

// Client APIs
/*=========================================================================
   Instantiates a new SPP clinet object  If successful output 
   pointer(ppif) shall have pointer to new SPP client object
===========================================================================*/
int AEEBTSPPClient_New(IShell* pIShell, void **ppif )
{
   IBTSPPClient *pMe;

   if ( ISHELL_CheckPrivLevel( pIShell,AEEIID_BT_SPP_CLIENT, TRUE ) != TRUE )
   {
      return EPRIVLEVEL;
   }

   if ( NULL == ppif )
   {
      return EBADPARM;
   }

   // allocate pMe
   pMe = MALLOCREC( IBTSPPClient );

   if ( NULL  == pMe )
   {
      return ENOMEMORY;
   }

   // create the OEM layer object and initialise it
   if ( SUCCESS !=  OEMBTSPP_New( &pMe->hBT, NULL, 0, 0,FALSE ))
   {
      FREE( pMe );
      return EFAILED;
   }

   pMe->pvt = &g_Vtbl_BTSPPClient;
   pMe->pIShell = pIShell;
   pMe->dwRefs  = 0;
   ISHELL_AddRef( pIShell );

   // Initialise the cb that will be called in case the app closes
   CALLBACK_Init( &pMe->cbAppClose, AEEBTSPPCLI_OnAppClose, pMe );
   AEE_LinkSysObject( &pMe->cbAppClose );

   AEEBTSPPCLI_AddRef( pMe );

   *ppif = (void *)pMe;

   return SUCCESS;
}

/*=========================================================================
    Does the cleanup job when application using SPP object is closed
===========================================================================*/
void AEEBTSPPCLI_OnAppClose(IBTSPPClient* pme)
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
static uint32 AEEBTSPPCLI_AddRef( IBTSPPClient* pMe )
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
static uint32 AEEBTSPPCLI_Release( IBTSPPClient* pme )
{
   uint32 dwRefs;

   if ( 0 == pme->dwRefs )
   {
      return pme->dwRefs;
   }

   dwRefs = --pme->dwRefs;

   if ( 0 == dwRefs )
   {
      AEEBTSPPCLI_OnAppClose(pme);
   }

   return dwRefs;
}


/*======================================================================= 
 This function retrieves a pointer to an interface conforming to the 
 definition of the specified class ID.
=======================================================================*/
static int AEEBTSPPCLI_QueryInterface( IBTSPPClient* pme,
                                       AEECLSID clsid,
                                       void **ppo )
{
   if (( NULL == ppo ) || (NULL == pme ))
   {
      return EBADPARM;
   }

   if ( !AEE_CHECK_PTR("AEEBTSPPCLI_QueryInterface",
                       ppo, sizeof(void*), TRUE) )
   {
      return EMEMPTR;
   }

   if ( ( AEECLSID_QUERYINTERFACE == clsid ) ||
        ( AEECLSID_BASE == clsid )           ||
        ( AEECLSID_PORT == clsid )           ||
        ( AEEIID_BTSPP == clsid )            ||  
        (AEEIID_BT_SPP_CLIENT == clsid))
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
static int32 AEEBTSPPCLI_Read( IBTSPPClient* po, char* pcBuf, int32 cbBuf )
{
   return OEMBTSPP_Read( po->hBT, pcBuf, cbBuf );

}

/*======================================================================= 
   Inherited from IPORT_Readable().
   Registers a function to be invoked when receive queue has data.
=======================================================================*/
static void AEEBTSPPCLI_Readable( IBTSPPClient* po, AEECallback* pcb )
{
   OEMBTSPP_Readable( po->hBT, pcb, po->pIShell);
   return;
}

/*======================================================================= 
   Inherited from IPORT_GetLastError().
   This function returns the last error that occurred with Read() or 
   Write().
=======================================================================*/
static int AEEBTSPPCLI_GetLastError( IBTSPPClient* po )
{
   return OEMBTSPP_GetLastError( po->hBT );
}

/*======================================================================= 
  Inherited from IPORT_Write().
  This function sends data to remote device.
=======================================================================*/
static int32 AEEBTSPPCLI_Write( IBTSPPClient* pme, const char* pcBuf, int32 cbBuf )
{
   return OEMBTSPP_Write( pme->hBT, (const char *)pcBuf, cbBuf );
}

/*======================================================================= 
  Inherited from IPORT_Writeable().
  Registers a callback to be invoked when transmit queue can accept more 
  data to be sent out. 
=======================================================================*/
static void  AEEBTSPPCLI_Writeable( IBTSPPClient* pme, AEECallback* pcb )
{
   OEMBTSPP_Writable( pme->hBT, pcb, pme->pIShell );
   return;
}

/*======================================================================= 
  Inherited from IPORT_IOCtl().
  This API is currently not supported. Please check IPORT_IOCtlEx()
=======================================================================*/
static int  AEEBTSPPCLI_IOCtl( IBTSPPClient* po, int nOption, uint32 dwVal )
{
   return ECLASSNOTSUPPORT;
}

/*======================================================================= 
 Inherited from IPORT_Close().
 This function is not supported.  Instead, Disconnect() 
 should be used.
=======================================================================*/
static int AEEBTSPPCLI_Close( IBTSPPClient* po )
{
   return ECLASSNOTSUPPORT;
}

/*======================================================================= 
 Inherited from IPORT_Open().
 This function is not supported.  Instead, AEESPPCLI_Connect() 
 should be used
=======================================================================*/
static int AEEBTSPPCLI_Open( IBTSPPClient* po, const char* szPort )
{
   return ECLASSNOTSUPPORT;
}

/*======================================================================= 
 This API registers a callback to be invoked when there's a change in the
 SPP stream state associated with the SPP object.
=======================================================================*/
static int AEEBTSPPCLI_OnStatChange( IBTSPPClient* po, AEECallback* pRCb )
{
   return OEMBTSPP_OnStatusChange( po->hBT,
                                   pRCb,
                                   OEMBTSPP_STATE_CHANGE,
                                   po->pIShell );
}

/*======================================================================= 
 This API registers a callback to be invoked when there's a change in the
 modem status associated with the SPP object.
=======================================================================*/
static int AEEBTSPPCLI_OnModemStatusChange(IBTSPPClient*  po,
                                           AEECallback*   pCb)
{
   return OEMBTSPP_OnStatusChange( po->hBT,
                                   pCb,
                                   OEMBTSPP_MODEM_CHANGE,
                                   po->pIShell );
}

/*======================================================================= 
 This API registers a callback to be invoked when a line error occurs on the
 SPP stream associated with the SPP object. 
=======================================================================*/
static int AEEBTSPPCLI_OnLineError(IBTSPPClient*  po, AEECallback*   pCb)
{
   return OEMBTSPP_OnStatusChange( po->hBT,
                                   pCb,
                                   OEMBTSPP_LINE_ERROR,
                                   po->pIShell );
}

/*======================================================================= 
  This API terminates the SPP connection
=======================================================================*/
static int AEEBTSPPCLI_Disconnect( IBTSPPClient* po,
                                   BTResult* pR,
                                   AEECallback* pRCb )
{
   return OEMBTSPP_Disconnect( po->hBT, pR, pRCb );
}

/*======================================================================= 
    This API retrieves the stream's current status.  The streamID field of 
   the status indicates the stream whose status is being retrieved.
=======================================================================*/
static int AEEBTSPPCLI_GetStatus( IBTSPPClient* po, BTSPPStatus* pStatus)
{
   return OEMBTSPP_GetStatus( po->hBT, pStatus );

}

/*======================================================================= 
    This API retrieves the stream's current modem status.  
   The streamID field of the status indicates the stream whose
   modemstatus is being retrieved.
=======================================================================*/
static int AEEBTSPPCLI_GetModemStatus(IBTSPPClient* po,
                                      BTSPPModemStatus* pStatus)
{
   return OEMBTSPP_GetModemStatus( po->hBT, pStatus);

}

/*======================================================================= 
    This API retrieves the stream's current line error .  
   The streamID field of the status indicates the stream
   whose line error is being retrieved.
=======================================================================*/
static int AEEBTSPPCLI_GetLineError(IBTSPPClient* po,
                                    BTSPPLineError* pLineError)
{
   return OEMBTSPP_GetLineError( po->hBT, pLineError);

}

/*======================================================================= 
    This API configures the stream.  Note that the configuration does not 
   actually affect the connection, only the configuration settings are sent 
   to the remote device.
=======================================================================*/
static int AEEBTSPPCLI_Configure( IBTSPPClient* po, const BTSPPConfig* pCfg,
                                  BTResult* pR, AEECallback* pRCb)
{
   return OEMBTSPP_Configure( po->hBT, pCfg, pR, pRCb );
}

/*======================================================================= 
    This API controls the stream.
=======================================================================*/
static int AEEBTSPPCLI_IOCtlEx( IBTSPPClient* po,
                                BTSPPIOCtlCommand IOctlCmd, 
                                BTSPPModemStatus* pModemStatus,
                                AEECallback* pResultCb)
{
   return OEMBTSPP_IOCtlEx( po->hBT,
                            IOctlCmd,
                            pModemStatus,
                            pResultCb,
                            po->pIShell );

}

/*======================================================================= 
  This function enables applications implementing the SPP client role to
  search for all SPP servers in the vicinity.
=======================================================================*/
static int AEEBTSPPCLI_FindServersInit( IBTSPPClient* po,
                                        uint32        dwSSearchDuration,
                                        uint32        dwMaxServers)
{
   return OEMBTSPPCLI_FindServersInit(po->hBT, dwSSearchDuration, dwMaxServers);

}

/*======================================================================= 
   After AEEBTSPPCLI_FindServersInit() returns SUCCESS, this function should
   be called to register a callback to be invoked when a server is found.
   The callback will be invoked when the next server is found.
=======================================================================*/
static int AEEBTSPPCLI_FindServersNext( IBTSPPClient*      po, 
                                        uint32*            pSize,
                                        AEECallback*       pCb)
{
   return OEMBTSPPCLI_FindServersNext(po->hBT, pSize, pCb, po->pIShell);

}

/*======================================================================= 
   After the callback registered via IBTSPPCLI_FindServersNext() is invoked,
   the application should allocate enough memory to save the server
   information.
=======================================================================*/
static int AEEBTSPPCLI_GetServerInfo( IBTSPPClient*      po, 
                                      BTSPPDeviceInfo*   pServerInfo)
{
   return OEMBTSPPCLI_GetServerInfo(po->hBT, pServerInfo);

}

/*======================================================================= 
   This method enables application implementing the SPP client role to
   connect to the SPP server whose BD address is given.
=======================================================================*/
static int AEEBTSPPCLI_Connect( IBTSPPClient*      po,
                                const BDAddress*   pBDAddr,
                                uint32             uServerChannel,
                                BTResult*          pResult,
                                AEECallback*       pResultCb)
{
   return OEMBTSPPCLI_Connect(po->hBT,
                              pBDAddr,
                              uServerChannel,
                              pResult,
                              pResultCb);
}

/*======================================================================= 
   This function cancels a previously initiated server search.
=======================================================================*/
static int AEEBTSPPCLI_CancelServerSearch( IBTSPPClient*  po,
                                           AEECallback*  pCb)
{
   return OEMBTSPPCLI_CancelServerSearch(po->hBT, pCb);
}

#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */

