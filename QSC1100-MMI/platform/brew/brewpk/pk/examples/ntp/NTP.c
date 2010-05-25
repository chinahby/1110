/*============================================================================

FILE:  AEENTP.c

SERVICES: Provides NTP related services.


==============================================================================

Copyright © 1999-2004 QUALCOMM Incorporated 
All Rights Reserved.
QUALCOMM Proprietary


============================================================================*/

#include "AEEStdLib.h"
#include "AEENTP.h"		// Applet-specific header that contains class ID
#include "AEENet.h"

#define FARF_NTP               0

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 

#define EPOCHDIFF 2524953600UL

#define NTP_PACKET_SIZE 48

#define NTP_MODE_SHIFT            0
#define NTP_MODE_MASK             0x07
#define NTP_MODE_RESERVED         0
#define NTP_MODE_SYM_ACTIVE       1
#define NTP_MODE_SYM_PASSIVE      2
#define NTP_MODE_CLIENT           3
#define NTP_MODE_SERVER           4
#define NTP_MODE_BROADCAST        5
#define NTP_MODE_RESERVED_CONTROL 6
#define NTP_MODE_RESERVED_PRIVATE 7

#define NTP_VERSION_SHIFT    3
#define NTP_VERSION_MASK     0x38

#define NTP_LI_SHIFT         6
#define NTP_LI_MASK          0xC0
#define NTP_LI_NO_WARNING    0
#define NTP_LI_61_SEC_MINUTE 1
#define NTP_LI_59_SEC_MINUTE 2
#define NTP_LI_ALARM         3

#define NTP_PACKET_HEADER      0
#define NTP_PACKET_STRATUM     1
#define NTP_PACKET_POLL        2
#define NTP_PACKET_PRECISION   3
#define NTP_PACKET_ROOT_DELAY  4
#define NTP_PACKET_ROOT_DISP   8
#define NTP_PACKET_REF_ID      12
#define NTP_PACKET_REF_TS_SEC  16
#define NTP_PACKET_REF_TS_MS   20
#define NTP_PACKET_ORIG_TS_SEC 24
#define NTP_PACKET_ORIG_TS_MS  28
#define NTP_PACKET_RCV_TS_SEC  32
#define NTP_PACKET_RCV_TS_MS   36
#define NTP_PACKET_TRNS_TS_SEC 40
#define NTP_PACKET_TRNS_TS_MS  44



struct _INTP
{
   const AEEVTBL(INTP) *pvt;// Function Table

   AEEVTBL(INTP) pvt_storage;
   IShell*           m_pShell;     // Pointer to the shell
#ifdef DYNAMIC_APP
   IModule *	        m_pModule;	 // IModule interface pointer
#endif
   uint32            m_dwRefs;     // Reference count for this object
   INetMgr *         m_pNetMgr;    // Pointer to Net Manager Object
   ISocket *         m_pSocket;    // Pointer to Socket Object
   AEEDNSResult      m_DNSResult;  // Pointer to the DNS Result structure
   AEECallback       m_cbDNS;      // Callback function for the GetHostByName Call
   INAddr            m_NTP_Server; // Address of the NTP Server
   char *            m_pszServer;  // The server name
   INPort            m_nPort;      // The NTP Port Number (Should be 123)
   char              m_nVersion;   // Version Number of the NTP Client (1-4)
   AEENTPResult *    m_pResult;    // Pointer to the stucture to return the status in
   AEECallback  *    m_pcbResult;  // Callback Function to report status to
   char              m_NTPBuffer[NTP_PACKET_SIZE];  // Where to store the NTP Packet
   boolean           m_bCanceled;  //Has the last Operation been canceled

};

int NTP_New(IShell *pShell, AEECLSID ClsId, void **ppObj);
static uint32 NTP_AddRef(INTP *pMe);
static uint32 NTP_Release(INTP *pMe);
static int NTP_QueryInterface(INTP *pif, AEECLSID id, void **ppo);
static int NTP_GetTime(INTP * pINTP, char * pszServer, uint32 nPort, char nNTPVersion, uint32 nTimeout, AEENTPResult * pres, AEECallback *pcb);


//Helper Functions
static void BindSocket(INTP * pMe);
static void NTP_QueryDone(INTP * pMe, int32 nReason, uint32 dwSeconds, int32 dwOffset, char nStratum);
static void DNS_LookupDone(void *p);
static void SendNTPRequest(INTP * pMe);
static void ReceiveNTPResponse(INTP * pMe);
static void CancelOperationTO(INTP * pMe);
static void CancelOperationCB(AEECallback * pcb);

#ifndef DYNAMIC_APP
static const VTBL(INTP) gNTPFuncs = 
{
   NTP_AddRef,
      NTP_Release,
      NTP_QueryInterface,
      NTP_GetTime  
};
#endif

#ifdef DYNAMIC_APP
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMod, void **ppObj)
{
   INTP *pMe;
   int nErr = NTP_New(pIShell, ClsId, ppObj);
   if(nErr == SUCCESS)
   {
      pMe = (INTP *) *ppObj;
      pMe->m_pModule   = pMod;
      IMODULE_AddRef(pMod);
   }
   return nErr;
}
#endif

/*====================================================================
CONSTRUCTOR / DESTRUCTORS
====================================================================*/
int NTP_New(IShell *pShell, AEECLSID ClsId, void **ppObj)
{
   INTP *pNew;
   int nErr;

   // Make sure its us
   if (ClsId != AEECLSID_NTP)
      return ECLASSNOTSUPPORT;

   // Get the object
   pNew = MALLOCREC(INTP);
   if (!pNew)
   {
      FARF(NTP,("Error MALLOC FAILED"));
      return ENOMEMORY;
   }
   // Okay, this is kinda a waste, but until BREW learns how to store
   // global data, this is what we have to do.  I can't figure out any way
   // for the compiler to give me a pointer directly into the code
#ifdef DYNAMIC_APP
   pNew->pvt_storage.AddRef = NTP_AddRef;
   pNew->pvt_storage.Release = NTP_Release;
   pNew->pvt_storage.QueryInterface = NTP_QueryInterface;
   pNew->pvt_storage.GetTime = NTP_GetTime;

   pNew->pvt = &pNew->pvt_storage;
#else
   pNew->pvt = &gNTPFuncs;
#endif

   // Now initialize it
   pNew->m_pShell = pShell;
   ISHELL_AddRef(pNew->m_pShell);

   pNew->m_dwRefs = 1;


   nErr = ISHELL_CreateInstance(pNew->m_pShell, AEECLSID_NET, (void **) &pNew->m_pNetMgr);

   if(nErr != SUCCESS)
   {
      FARF(NTP,("Creating NetMgr Failed %d", nErr));
      ISHELL_Release(pNew->m_pShell);
      FREE(pNew);
      return nErr;
   }

   *ppObj = pNew;
   return SUCCESS;
}

static void NTP_Delete(INTP *pMe)
{
   // Delete everything

   // Cancel DNS Callback
   CALLBACK_Cancel(&pMe->m_cbDNS);

   //Cancel Pending Timeout Timer
   ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY) CancelOperationTO, (void *) pMe);

   //Release Structures
   if( NULL != pMe->m_pSocket )
   {
      ISOCKET_Release(pMe->m_pSocket);
      pMe->m_pSocket = NULL;
   }
   if( NULL != pMe->m_pNetMgr )
   {
      INETMGR_Release(pMe->m_pNetMgr);
      pMe->m_pNetMgr = NULL;
   }
   if (NULL != pMe->m_pShell)
   {
      ISHELL_Release(pMe->m_pShell);
      pMe->m_pShell = NULL;
   }

#ifdef DYNAMIC_APP
   if(NULL != pMe->m_pModule)
   {
      IMODULE_Release(pMe->m_pModule);
      pMe->m_pModule = NULL;
   }
#endif

   FREE(pMe);
}

/*=====================================================================
INTERFACE FUNCTIONS
=====================================================================*/
static uint32 NTP_AddRef(INTP *po)
{
   INTP *pme = (INTP*)po;
   return ++pme->m_dwRefs;
}

static uint32 NTP_Release(INTP *po)
{
   INTP *pme = (INTP*)po;
   uint32 dwRefs;

   dwRefs = --pme->m_dwRefs;
   if (dwRefs == 0)
      NTP_Delete(pme);
   return (dwRefs);
}

static int NTP_QueryInterface(INTP *po, AEECLSID id,
                              void **ppo)
{
   switch (id)
   {
   case AEECLSID_QUERYINTERFACE:
   case AEECLSID_NTP:
      *ppo = (void*)po;
      IQI_AddRef(*ppo);
      return SUCCESS;
   }

   /* Default case: We don't know what they mean by their classid */
   return ECLASSNOTSUPPORT;
} 

static int NTP_GetTime(INTP * pMe, char * pszServer, uint32 nPort, char nNTPVersion, uint32 nTimeout, AEENTPResult * pres, AEECallback *pcb)
{
   if(!pszServer || !pres || !pcb)
      return EBADPARM;

   pMe->m_pszServer = pszServer;
   pMe->m_nPort     = (INPort) nPort;
   pMe->m_nVersion  = nNTPVersion;
   pMe->m_pResult   = pres;
   pMe->m_pcbResult = pcb;

   pMe->m_bCanceled = FALSE;

   //Set Timeout Time
   if(nTimeout)
   {
      ISHELL_SetTimer(pMe->m_pShell, nTimeout, (PFNNOTIFY) CancelOperationTO, (void *) pMe);
   }
   pcb->pfnCancel = (PFNCBCANCEL) CancelOperationCB;
   pcb->pCancelData = (void *) pMe;


      //Open The Socket
   pMe->m_pSocket = INETMGR_OpenSocket(pMe->m_pNetMgr, AEE_SOCK_DGRAM);
   if(!pMe->m_pSocket) 
   {
      FARF(NTP,("OpenSocket Failed"));
      return EFAILED;
   }

   //Bind the socket
   BindSocket(pMe);

   return SUCCESS;
}

/*=====================================================================
NON-INTERFACE FUNCTIONS
=====================================================================*/
static void NTP_QueryDone(INTP * pMe, int32 nReason, uint32 dwSeconds, int32 dwOffset, char nStratum)
{
   ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY) CancelOperationTO, (void *) pMe);
   pMe->m_pResult->nResult   = nReason;
   pMe->m_pResult->dwSeconds = dwSeconds;
   pMe->m_pResult->dwOffset = dwOffset;
   pMe->m_pResult->nStratum  = nStratum;
   pMe->m_pcbResult->pfnCancel = NULL;
   pMe->m_pcbResult->pCancelData = NULL;
   if( NULL != pMe->m_pSocket )
   {
      ISOCKET_Release(pMe->m_pSocket);
      pMe->m_pSocket = NULL;
   }
   ISHELL_Resume(pMe->m_pShell, pMe->m_pcbResult);
   return;
}

static void CancelOperationTO(INTP * pMe)
{
   pMe->m_bCanceled = TRUE;
   FARF(NTP,("Canceling NTP Query - TIMEOUT"));
   NTP_QueryDone(pMe, AEE_NET_ETIMEDOUT, 0, 0, 0);
}

static void CancelOperationCB(AEECallback * pcb)
{
   INTP *pMe = pcb->pCancelData;
   pMe->m_bCanceled = TRUE;
   ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY) CancelOperationTO, (void *) pMe);
   FARF(NTP,("Canceling NTP Query - CALLBACK CANCELED"));
   if( NULL != pMe->m_pSocket )
   {
      ISOCKET_Release(pMe->m_pSocket);
      pMe->m_pSocket = NULL;
   }
}

static void BindSocket(INTP * pMe)
{
   int nErr = 0;

   if(pMe->m_bCanceled)
      return;

   //Bind the Socket
   nErr = ISOCKET_Bind(pMe->m_pSocket, AEE_INADDR_ANY, pMe->m_nPort);
   if(nErr)
   {
      if(nErr == AEE_NET_WOULDBLOCK)
      {
         //Call Bind Again
         ISOCKET_Writeable(pMe->m_pSocket,(PFNNOTIFY) BindSocket, (void *) pMe);
         return;
      }
      else
      {
         FARF(NTP,("Bind Socket Failed %d", nErr));
         NTP_QueryDone(pMe, nErr, 0, 0, 0);
         return;
      }

   }
   //Socket Bound
   //Send NTP Request

   CALLBACK_Init(&pMe->m_cbDNS,DNS_LookupDone,pMe);
   INETMGR_GetHostByName(pMe->m_pNetMgr, &(pMe->m_DNSResult),pMe->m_pszServer, &(pMe->m_cbDNS)); 

   return;

}


static void DNS_LookupDone(void *p)
{
   int nErr;
   INTP * pMe = (INTP *)p;
   if(pMe->m_bCanceled)
      return;

   nErr = pMe->m_DNSResult.nResult;

   if (nErr > AEEDNSMAXADDRS) {
      FARF(NTP,("** DNS Lookup Failed for %s: Error %d", pMe->m_pszServer, nErr));
      NTP_QueryDone(pMe, nErr, 0, 0, 0);
      return;
   }


   pMe->m_NTP_Server = pMe->m_DNSResult.addrs[0];

   SendNTPRequest(pMe);
}


static void SendNTPRequest(INTP * pMe)
{
   int32 nErr = 0;
 
   uint32 dwSecs;
   uint32 * pdwSecs = (uint32 *) &(pMe->m_NTPBuffer[NTP_PACKET_TRNS_TS_SEC]);

   if(pMe->m_bCanceled)
      return;


   MEMSET(&(pMe->m_NTPBuffer), 0, NTP_PACKET_SIZE);

   pMe->m_NTPBuffer[NTP_PACKET_HEADER] = (char) ((NTP_LI_NO_WARNING << NTP_LI_SHIFT) | (pMe->m_nVersion << NTP_VERSION_SHIFT) | (NTP_MODE_CLIENT << NTP_MODE_SHIFT));
   
   dwSecs = GETTIMESECONDS() - LOCALTIMEOFFSET(NULL) + EPOCHDIFF;

   *pdwSecs = HTONL(dwSecs);

   nErr = ISOCKET_SendTo(pMe->m_pSocket, (void *) &(pMe->m_NTPBuffer), NTP_PACKET_SIZE, 0, pMe->m_NTP_Server, HTONS(pMe->m_nPort)); 

   if(nErr == AEE_NET_WOULDBLOCK)
   {
      ISOCKET_Writeable(pMe->m_pSocket,(PFNNOTIFY) SendNTPRequest, (void *) pMe);
      return;
   }
   else if(nErr == AEE_NET_ERROR)
   {
      FARF(NTP,("Net Send Failure"));
      NTP_QueryDone(pMe, nErr, 0, 0, 0);
      return;
   }

   ReceiveNTPResponse(pMe);
   return;
}


static void ReceiveNTPResponse(INTP * pMe)
{
   int32 nErr = 0;
   uint32 NTPSecs;
   uint32   dwOrigSecs;
   uint32   dwRcvSecs;
   uint32   dwTrnsSecs;
   uint32   dwDestSecs;
   int32    dwOffset;
   uint32   *pdwSecs;
   char     nStratum;
   char     nHeader;
   char     nLI;
   char     nVersion;
   char     nMode;

   if(pMe->m_bCanceled)
      return;

   MEMSET(&(pMe->m_NTPBuffer), 0, NTP_PACKET_SIZE);

   nErr = ISOCKET_RecvFrom(pMe->m_pSocket, (void *) &(pMe->m_NTPBuffer), NTP_PACKET_SIZE, 0, NULL, NULL); 

   if(nErr == AEE_NET_WOULDBLOCK)
   {
      ISOCKET_Readable(pMe->m_pSocket,(PFNNOTIFY) ReceiveNTPResponse, (void *) pMe);
      return;
   }
   else if(nErr == AEE_NET_ERROR)
   {
      FARF(NTP,("Net Receive Failure"));
      NTP_QueryDone(pMe, nErr, 0, 0, 0);
      return;
   }

   dwDestSecs = GETTIMESECONDS() - LOCALTIMEOFFSET(NULL) + EPOCHDIFF;

   pdwSecs  = (uint32 *) &(pMe->m_NTPBuffer[NTP_PACKET_ORIG_TS_SEC]);
   dwOrigSecs = NTOHL(*pdwSecs);
   pdwSecs  = (uint32 *) &(pMe->m_NTPBuffer[NTP_PACKET_RCV_TS_SEC]);
   dwRcvSecs  = NTOHL(*pdwSecs);
   pdwSecs  = (uint32 *) &(pMe->m_NTPBuffer[NTP_PACKET_TRNS_TS_SEC]);
   dwTrnsSecs = NTOHL(*pdwSecs);

   dwOffset = (((int32)dwRcvSecs - (int32)dwOrigSecs) + ((int32)dwTrnsSecs - (int32)dwDestSecs)) / 2;

   NTPSecs  = dwDestSecs + dwOffset;

   nHeader  = pMe->m_NTPBuffer[NTP_PACKET_HEADER];
   nStratum = pMe->m_NTPBuffer[NTP_PACKET_STRATUM];

   nLI      = (nHeader & (NTP_LI_MASK))      >> NTP_LI_SHIFT;
   nVersion = (nHeader & (NTP_VERSION_MASK)) >> NTP_VERSION_SHIFT;
   nMode    = (nHeader & (NTP_MODE_MASK))    >> NTP_MODE_SHIFT;

   FARF(NTP,("NTP PACKET RECIEVED"));
   FARF(NTP,("==================="));
   FARF(NTP,("NTP Time      : %u", NTPSecs));
   FARF(NTP,("NTP Orig Time : %u", dwOrigSecs));
   FARF(NTP,("NTP Rcv  Time : %u", dwRcvSecs));
   FARF(NTP,("NTP Trns Time : %u", dwTrnsSecs));
   FARF(NTP,("NTP Dest Time : %u", dwDestSecs));
   FARF(NTP,("NTP Offset    : %d", dwOffset));
   FARF(NTP,("NTP Stratum  : %u", nStratum));
   nErr = SUCCESS;
   switch(nLI)
   {
   case NTP_LI_NO_WARNING:
      {
         FARF(NTP,("NTP Leap Ind: No Warning (%u)", nLI));
         break;
      }
   case NTP_LI_61_SEC_MINUTE:
      {
         FARF(NTP,("NTP Leap Ind: 61 Second Minute (%u)", nLI));
         break;
      }
   case NTP_LI_59_SEC_MINUTE:
      {
         FARF(NTP,("NTP Leap Ind: 59 Second Minute (%u)", nLI));
         break;
      }
   case NTP_LI_ALARM:
      {
         FARF(NTP,("NTP Leap Ind: Alarm Condition (%u)", nLI));
         nErr = EFAILED;
         break;
      }
   default:
      {
         FARF(NTP,("NTP Leap Ind: Unknown Value (%u)", nLI));
         nErr = EFAILED;
         break;
      }
   }
   if(nVersion != pMe->m_nVersion)
      nErr = EFAILED;

   FARF(NTP,("NTP Version  : %u", nVersion));

   switch(nMode)
   {
   case NTP_MODE_RESERVED:
      {
         FARF(NTP,("NTP Mode     : Reserved (%u)", nMode));
         nErr = EFAILED;
         break;
      }
   case NTP_MODE_SYM_ACTIVE:
      {
         FARF(NTP,("NTP Mode     : Symmetric Active (%u)", nMode));
         nErr = EFAILED;
         break;
      }
   case NTP_MODE_SYM_PASSIVE:
      {
         FARF(NTP,("NTP Mode     : Symmetric Passive (%u)", nMode));
         nErr = EFAILED;
         break;
      }
   case NTP_MODE_CLIENT:
      {
         FARF(NTP,("NTP Mode     : Client (%u)", nMode));
         nErr = EFAILED;
         break;
      }
   case NTP_MODE_SERVER:
      {
         FARF(NTP,("NTP Mode     : Server (%u)", nMode));
         break;
      }
   case NTP_MODE_BROADCAST:
      {
         FARF(NTP,("NTP Mode     : Broadcast (%u)", nMode));
         nErr = EFAILED;
         break;
      }
   case NTP_MODE_RESERVED_CONTROL:
      {
         FARF(NTP,("NTP Mode     : Reserved Control (%u)", nMode));
         nErr = EFAILED;
         break;
      }
   case NTP_MODE_RESERVED_PRIVATE:
      {
         FARF(NTP,("NTP Mode     : Reserved Private (%u)", nMode));
         nErr = EFAILED;
         break;
      }
   default:
      {
         FARF(NTP,("NTP Mode     : Unknown (%u)", nMode));
         nErr = EFAILED;
         break;
      }
   }

   FARF(NTP,("==================="));

   NTP_QueryDone(pMe, nErr, NTPSecs, dwOffset, nStratum);

   return;
} 


