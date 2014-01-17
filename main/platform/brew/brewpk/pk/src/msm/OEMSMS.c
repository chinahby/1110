/*===========================================================================
                              OEMSMS.c

  OEM REFERENCE SMS SUPPORT ROUTINES
  
  This file contains a reference implementation of SMS support.
    
        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/*===========================================================================
                      IMPORTED DECLARATIONS
===========================================================================*/
#include "OEMFeatures.h"
#include "OEMHeap.h"
#include "OEMCriticalSection.h"
#include "OEMSMS.h"
#include "OEMSMS_priv.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEESMS.h"
#include "AEETelephone.h"
#include "AEE_OEMSMS.h"


/*===========================================================================
                 PRIVATE CONSTANTS
===========================================================================*/
#define CES_FREE        0
#define CES_PARSING     1
#define CES_READY       2
#define CES_DISPATCHING 3

/*===========================================================================
                 PRIVATE DATA DECLARATIONS
===========================================================================*/
#define RELEASEIF(p) if (p) { IBASE_Release((IBase*)p); p = 0; }

struct _OEMSMSMgr
{
   uint32                        nRefs;
   AEECallback                   *pcbOnMTMessage;
   SMSCacheEntry *               pSMSCache;      // Points to the cache to store incoming messages
};

// The number of entries in the cache for Mobile terminated SMS. 25 value is chosen
// keeping in mind that inbox of native client have limit of 20 messages.
#if defined(FEATURE_VERSION_K212_12832)
#define MAX_SMS_CACHE   5
#else
#define MAX_SMS_CACHE   25
#endif
// Array of OEM defined handlers.  This list contains OEM defined handlers
// which dont need the typical BREW handler pattern BREWHANDLER:Payload
// So for example if Java requires  all //WMA messages without the :, they
// can be added here.
static char * aszOEMDefinedHandler[] = {"WMA"};

/*===========================================================================
                 PRIVATE METHOD DECLARATIONS
===========================================================================*/
static int OEMSMS_Delete( OEMSMSMgr *pme );
static void OEMSMS_CancelListener( AEECallback *pcb );
static void OEMSMS_ReleaseMessage( AEECallback *pcb );

/*===========================================================================
                 PRIVATE DATA DEFINITIONS
===========================================================================*/
static OEMSMSMgr *gpSMS;
static uint32 gMsgId;
/*===========================================================================
                 PUBLIC METHOD DEFINITIONS
===========================================================================*/

/*===========================================================================
Manages singleton instance of gpSMS. Initializes gpSMS for first time and 
increases its reference count subsequently.
===========================================================================*/
int OEMSMS_New( OEMSMSMgr **ppme )
{
   int i;
   SMSCacheEntry *pce;

   *ppme = 0;

   FARF(SMS, ("==>  OEMSMS_New"));

   if (gpSMS) 
   {
      ++gpSMS->nRefs;
   } 
   else 
   {
      gpSMS = (OEMSMSMgr *)sys_malloc( sizeof(OEMSMSMgr) + (sizeof(SMSCacheEntry) * MAX_SMS_CACHE) );
      
      if (!gpSMS) 
      {
         return ENOMEMORY;
      }

      gpSMS->nRefs = 1;
      
      // Initialize the cache
      pce = gpSMS->pSMSCache = (SMSCacheEntry *)(gpSMS + 1);

      // Mark all the cache entries as free
      for(i = 0; i < MAX_SMS_CACHE; i++, pce++) 
      {
         pce->wStatus = CES_FREE;
      }

      OEMSMS_Init();
   }
   
   *ppme = gpSMS;

   return SUCCESS;
}

/*===========================================================================
Inrecases reference count of gpSMS
===========================================================================*/
uint32 OEMSMS_AddRef( OEMSMSMgr *pme )
{
   FARF(SMS, ("==>  OEMSMS_AddRef"));

   return ++pme->nRefs;
}

/*===========================================================================
Decreases reference count of gpSMS. Frees object when reference count reaches
zero
===========================================================================*/
uint32 OEMSMS_Release( OEMSMSMgr *pme )
{
   FARF(SMS, ("==>  OEMSMS_Release"));

   if( --pme->nRefs ) 
      return pme->nRefs;

   OEMSMS_Delete(pme);
   return 0;
}

/*===========================================================================
Called by AEESMS to register callback to be invoked on mobile terminated
messages
===========================================================================*/
void OEMSMS_OnMTMessage( OEMSMSMgr *pme, AEECallback *pcb )
{
   FARF(SMS, ("==>  OEMSMS_OnMTMessage"));

   // Make sure that its not there in any other list
   CALLBACK_Cancel(pcb);

   pcb->pfnCancel   = OEMSMS_CancelListener;
   pcb->pCancelData = (void *)pme;
   pcb->pmc         = 0;

   if( pme->pcbOnMTMessage ) 
   {
      pme->pcbOnMTMessage->pmc = (void *)pcb;
   }
   pcb->pNext           = pme->pcbOnMTMessage;
   pme->pcbOnMTMessage  = pcb;
}

/*===========================================================================
Finds a free cache entry.
===========================================================================*/
SMSCacheEntry *OEMSMSCache_GetEntry( void )
{
   OEMSMSMgr *pme = gpSMS;
   SMSCacheEntry *pce;
   SMSCacheEntry *pcoe = NULL;
   int i;

   FARF(SMS, ("==>  OEMSMSCache_GetEntry"));

   if( !gpSMS ) 
   {
      return NULL;
   }

   // Search for an entry in CES_FREE state. Also scout for oldest entry in
   // CES_DISPATCHING state.

   OEMCriticalSection_Enter(&gSMSCriticalSection);

   for(pce = pme->pSMSCache, i = 0; i < MAX_SMS_CACHE; i++, pce++)
   {
      if(pce->wStatus == CES_FREE)
      {
         pce->wStatus = CES_PARSING;
         break;
      }

      if(pce->wStatus == CES_DISPATCHING)
      {
         if (!pcoe)
         {
            pcoe = pce; // First entry in dispatching state
         }
         else
         {
            // Change pcoe if current pce is pointing to older entry
            if (pce->dwTimeStamp < pcoe->dwTimeStamp)
               pcoe = pce;
         }
      }
   }

   OEMCriticalSection_Leave(&gSMSCriticalSection);

   if (i >= MAX_SMS_CACHE)
   {
      if (!pcoe)
      {
         // Out of luck. This should happen only if more than MAX_SMS_CACHE messages 
         // arrive and we do not get a chance to process them
         FARF(SMS,("SMS (No Entries)"));
         return NULL;
      }
      else
      {
         // Use the oldest SMS cache entry
         pcoe->wStatus = CES_PARSING;
         pce = pcoe;
      }
   }

   MEMSET(pce,0,sizeof(SMSCacheEntry));
   pce->dwTransactionId = gMsgId;
   gMsgId = (gMsgId == 0xffffffff ? 0 : gMsgId + 1);
   return pce;
}

/*===========================================================================
Releases a cache entry into free pool.
===========================================================================*/
void OEMSMSCache_Release( SMSCacheEntry *pce )
{   
   FARF(SMS, ("==>  OEMSMSCache_Release"));
   pce->wStatus = CES_FREE;
}

/*===========================================================================
Called on arrival of MT message to invoke callbacks registered
===========================================================================*/
void OEMSMSCache_Ready( SMSCacheEntry *pce )
{
   FARF(SMS, ("==>  OEMSMSCache_Ready"));  
   
   // Change the status of message to CES_READY
   pce->wStatus = CES_READY;
   if( gpSMS )
   {
      while (gpSMS->pcbOnMTMessage)
      { 
         FARF(SMS, ("Resume Callback"));  
         AEE_SYS_RESUME( gpSMS->pcbOnMTMessage );
      }
   }
}

/*===========================================================================
Checks if OEMSMSMgr isn initialized
===========================================================================*/
boolean OEMSMS_IsInitialized(void)
{   
   FARF(SMS, ("==>  OEMSMS_IsInitialized"));
   return (gpSMS != NULL);
}

/*===========================================================================
Called by AEESMS to get transaction ID , teleservice ID and check 
if it is loopback message
===========================================================================*/
int OEMSMS_GetNewMsgInfo( OEMSMSMgr *pme, uint32 *pdwTransactionId, int * pts, boolean* pIsLoopback )
{
   SMSCacheEntry *   pce;
   int nErr = EFAILED;
   int i;
   
   FARF(SMS, ("==>  OEMSMS_GetNewMsgInfo"));

   if (!pdwTransactionId || !pts)
      return EFAILED;

   OEMCriticalSection_Enter(&gSMSCriticalSection);

   for(pce = pme->pSMSCache, i = 0; i < MAX_SMS_CACHE; i++, pce++)
   {
      if(pce->wStatus == CES_READY)
      {
         *pdwTransactionId = pce->dwTransactionId;
         *pts = pce->ts;
         *pIsLoopback = pce->bIsLoopbackMsg;
         pce->wStatus = CES_DISPATCHING;
         FARF(SMS,("SMS Dispatching"));
         nErr = SUCCESS;
         break;
      }
   }

   OEMCriticalSection_Leave(&gSMSCriticalSection);

   return nErr;
}

/*===========================================================================
Copies payload from pce to pMsgPayload
===========================================================================*/
void OEMSMS_CopyPayloadFromCache(SMSCacheEntry *pce, OEMMsgPayload * pMsgPayload)
{
   FARF(SMS, ("==>  OEMSMS_CopyPayloadFromCache"));

   if (!pce || !pMsgPayload) {
      return;
   }

   if (!pMsgPayload->nBytes || pMsgPayload->nBytes < pce->nText)
   {
      FARF(SMS, ("Message ID = %d, Size= %d", pce->dwTransactionId, pce->nText));
      pMsgPayload->nBytes = pce->nText;
   }
   else
   {
      FARF(SMS, ("Message ID = %d, Encoding = %d, Size = %d", pce->dwTransactionId, pce->nEncoding, pce->nText));
      FARF(SMS, ("Text = %s", pce->pText));
      pMsgPayload->nEncoding = pce->nEncoding;
      pMsgPayload->nBytes = pce->nText;
      pMsgPayload->nPadding = 0;
      MEMCPY(pMsgPayload->data, pce->pText, pce->nText);
   }
}


/*===========================================================================
Copies payload data from pce to pOEMMsgData
===========================================================================*/
void OEMSMS_CopyPayloadDataFromCache(SMSCacheEntry *pce, OEMMsgData * pOEMMsgData)
{
   int nUDHLength = 0;

   FARF(SMS, ("==>  OEMSMS_CopyPayloadDataFromCache"));

   if (!pce || !pOEMMsgData) {
      return;
   }
         
   pOEMMsgData->bIsLoopbackMsg = pce->bIsLoopbackMsg;
   pOEMMsgData->nFromAppletCls = pce->nFromAppletCls;
   pOEMMsgData->bLangPresent = pce->bLangPresent;
   pOEMMsgData->dwLang = pce->dwLang;
   pOEMMsgData->dwPID = pce->dwPID;
   pOEMMsgData->dwMsgClass = pce->dwMsgClass;
   pOEMMsgData->dwTimeStamp = pce->dwTimeStamp;
   pOEMMsgData->dwAbsoluteValidityTimeStamp = pce->dwAbsoluteValidityTimeStamp;
   pOEMMsgData->dwRelativeValidity = pce->dwRelativeValidity;
   pOEMMsgData->dwMsgWaitingType = pce->dwMsgWaitingType;
   pOEMMsgData->dwMsgWaitingKind = pce->dwMsgWaitingKind;
   pOEMMsgData->dwRawDCS = pce->dwRawDCS;
   pOEMMsgData->nNumMsgs = pce->nNumMsgs;
   pOEMMsgData->bPriorityPresent = pce->bPriorityPresent;
   pOEMMsgData->nPriority = pce->nPriority;
   pOEMMsgData->bPrivacyPresent = pce->bPrivacyPresent;
   pOEMMsgData->nPrivacy = pce->nPrivacy;
   pOEMMsgData->nTimeStampZone = pce->nTimeStampZone;
   pOEMMsgData->nAbsoluteValidityTimeStampZone = pce->nAbsoluteValidityTimeStampZone;
   pOEMMsgData->nDischargeTimeZone = pce->nDischargeTimeZone;
   STRLCPY(pOEMMsgData->szCallback, pce->szCallback, sizeof(pOEMMsgData->szCallback));
   STRLCPY(pOEMMsgData->szMOFromMTTo, pce->szFrom, sizeof(pOEMMsgData->szMOFromMTTo));
   pOEMMsgData->dwMOFromMTToType = pce->dwMOFromMTToType;
   pOEMMsgData->dwMOFromMTToPlan = pce->dwMOFromMTToPlan;
   pOEMMsgData->dwCallbackType = pce->dwCallbackType;
   pOEMMsgData->dwCallbackPlan = pce->dwCallbackPlan;
   pOEMMsgData->ts = pce->ts;
   pOEMMsgData->bRequestDLAck = pce->bRequestDLAck;
   pOEMMsgData->bRequestUserAck = pce->bRequestUserAck;
   pOEMMsgData->bRequestReadAck = pce->bRequestReadAck;
   pOEMMsgData->bIsMOSMS = pce->bIsMOSMS;
   pOEMMsgData->bIsDLAck = pce->bIsDLAck;
   pOEMMsgData->bIsUserAck = pce->bIsUserAck;
   pOEMMsgData->bIsReadAck = pce->bIsReadAck;
   pOEMMsgData->bIsCancelRequest = pce->bIsCancelRequest;
   pOEMMsgData->bIsReplyPathPresent = pce->bIsReplyPathPresent;
   pOEMMsgData->bIsStored = pce->bIsStored;
   pOEMMsgData->bIsCompressed = pce->bIsCompressed;
   pOEMMsgData->bHasMoreMessageToSend = pce->bHasMoreMessageToSend;
   pOEMMsgData->st = pce->st;
   pOEMMsgData->dwIndex = pce->dwIndex;

   pOEMMsgData->dwTag = pce->dwTag;
   if (AEESMS_TAG_STATUS_RPT == pOEMMsgData->dwTag)
   {
      pOEMMsgData->dwMsgDischargeTime = pce->dwMsgDischargeTime;
      pOEMMsgData->dwReportStatus = pce->dwReportStatus;
   }

   pOEMMsgData->dwMsgID = pce->dwMsgID;
   pOEMMsgData->bIsSCAddress = pce->bIsSCAddress;
   pOEMMsgData->dwRelativeDeferredValidity = pce->dwRelativeDeferredValidity;
   pOEMMsgData->dwAbsoluteDeferredValidity = pce->dwAbsoluteDeferredValidity;
   pOEMMsgData->dwGWCommand = pce->dwGWCommand;
   pOEMMsgData->dwMessageDisplayMode = pce->dwMessageDisplayMode;

   pOEMMsgData->bIsGWBroadcast = pce->bIsGWBroadcast;
   if (pOEMMsgData->bIsGWBroadcast == TRUE)
   {
      // Special Handling for transferring data from pce to OEMMsgData
      OEMSMS_GetGWCBParms(pce, pOEMMsgData);
   }
   else
   {
      pOEMMsgData->dwSrvID = pce->dwSrvID;
   }

   OEMSMS_GetUDHData(pce, &nUDHLength, NULL);
   if (pOEMMsgData->nUDHLength >= nUDHLength)
   {
      OEMSMS_GetUDHData(pce, &nUDHLength, pOEMMsgData->data);
   }
   pOEMMsgData->nUDHLength = nUDHLength;
      
   if (pOEMMsgData->nRawTPDULength >= pce->nRawTPDULength)
   {
      MEMCPY(pOEMMsgData->data + pOEMMsgData->nUDHLength, 
             pce->aRawTPDUData, 
             pce->nRawTPDULength);
   }
   pOEMMsgData->nRawTPDULength = pce->nRawTPDULength;
      
   if (pOEMMsgData->nAlphaIDLength >= pce->nAlphaIDLength)
   {
      MEMCPY(pOEMMsgData->data + pOEMMsgData->nUDHLength + pOEMMsgData->nRawTPDULength, 
             pce->aAlphaID, 
             pce->nAlphaIDLength);
   }
   pOEMMsgData->nAlphaIDLength = pce->nAlphaIDLength;
}

/*===========================================================================
Called to get message payload
===========================================================================*/
int OEMSMS_GetMsgPayload( OEMSMSMgr *pme, uint32 dwTransactionId, OEMMsgPayload * pMsgPayload )
{
   SMSCacheEntry *   pce;
   int nErr = EFAILED;
   int i;

   FARF(SMS, ("==>  OEMSMS_GetMsgPayload"));

   if (!pMsgPayload)
      return EFAILED;

   OEMCriticalSection_Enter(&gSMSCriticalSection);

   for(pce = pme->pSMSCache, i = 0; i < MAX_SMS_CACHE; i++, pce++)
   {
      if(pce->dwTransactionId == dwTransactionId)
      {
         nErr = SUCCESS;
         break;
      }
   }

   OEMCriticalSection_Leave(&gSMSCriticalSection);

   if (!nErr)
   {
      OEMSMS_CopyPayloadFromCache(pce, pMsgPayload);
   }
   return nErr;
}

/*===========================================================================
Called to get message data
===========================================================================*/
int OEMSMS_GetMsgData(OEMSMSMgr *pme, uint32 dwTransactionId, OEMMsgData * pOEMMsgData)
{
   SMSCacheEntry *   pce;
   int nErr = EFAILED;
   int i;
   
   FARF(SMS, ("==>  OEMSMS_GetMsgData"));

   if (!pOEMMsgData)
      return nErr;

   MEMSET(pOEMMsgData, 0, sizeof(pOEMMsgData));

   OEMCriticalSection_Enter(&gSMSCriticalSection);

   for(pce = pme->pSMSCache, i = 0; i < MAX_SMS_CACHE; i++, pce++){
      if(pce->dwTransactionId == dwTransactionId)
      {
         nErr = SUCCESS;
         break;
      }
   }

   OEMCriticalSection_Leave(&gSMSCriticalSection);

   if (!nErr)
   {
      OEMSMS_CopyPayloadDataFromCache(pce, pOEMMsgData);
   }
   return nErr;
}

/*===========================================================================

===========================================================================*/
void OEMSMS_SendMessage( OEMSMSMgr *pme, OEMMsgData * pMsgData, 
                        OEMMsgPayload * pMsgPayload, uint32 *pdwStatus, AEECallback *pcb )
{
   OEMMsg *hMsg = NULL;
   int  nErr = SUCCESS;
   ITelephone * pITelephone = NULL;
   AEETSSInfo ssi;
   IShell* pIShell = AEE_GetShell();
   FARF(SMS, ("==>  OEMSMS_SendMessage"));

   if ( pIShell == NULL )
   {
      nErr = EFAILED;
      FARF(SMS, ("==>  AEE_GetShell failed"));
      goto Done;
   }

   nErr = ISHELL_CreateInstance(pIShell, AEECLSID_TELEPHONE, (void**)&pITelephone);
   if (SUCCESS != nErr)
      goto Done;

   MEMSET(&ssi, 0, sizeof(ssi));
   nErr = ITELEPHONE_GetServingSystemInfo(pITelephone, &ssi, sizeof(ssi));
   if (SUCCESS != nErr)
      goto Done;

   nErr = OEMSMS_NewMessage( pMsgData, pMsgPayload, ssi.sys_mode, &hMsg );

   if (AEE_SUCCESS == nErr) 
   {   
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = hMsg;
      pcb->pfnCancel   = OEMSMS_ReleaseMessage;
      OEMSMS_SendSMS( hMsg, pcb, pdwStatus );
   }
   
Done:
   RELEASEIF(pITelephone);
   if( SUCCESS != nErr ) 
   {
      if (EBADTASK == nErr)
      {
         AEESMS_SETERROR(*pdwStatus, AEESMS_ERRORTYPE_TEMP, AEESMS_ERROR_NETWORK_PROBLEM);
         AEE_SYS_RESUME( pcb );
      }
      else
      {
         AEESMS_SETERROR(*pdwStatus, AEESMS_ERRORTYPE_TEMP, AEESMS_ERROR_NO_MEMORY);
         AEE_SYS_RESUME( pcb );
      }
   }
}

/*===========================================================================

===========================================================================*/
int OEMSMS_SetClientStatus( OEMSMSMgr *pme, uint32 cs, uint32 mt )
{   
   FARF(SMS, ("==>  OEMSMS_SetClientStatus"));
   return OEMSMS_SetMsgAckStatus(cs, mt);
}

/*===========================================================================
Called to free SMSCacheEntry associated with the given transaction Id
===========================================================================*/
void OEMSMS_FreeMsg( OEMSMSMgr *pme, uint32 dwTransactionId)
{
   SMSCacheEntry *   pce;
   int i;

   FARF(SMS, ("==>  OEMSMS_FreeMsg"));

   OEMCriticalSection_Enter(&gSMSCriticalSection);

   for(pce = pme->pSMSCache, i = 0; i < MAX_SMS_CACHE; i++, pce++)
   {
      if(pce->dwTransactionId == dwTransactionId)
      {
         OEMSMSCache_Release(pce);
         break;
      }
   }

   OEMCriticalSection_Leave(&gSMSCriticalSection);
}

/*===========================================================================
// This function enables OEMs make BREW ignore stuff prepended to // for
// considering message as BREW directed
===========================================================================*/
boolean OEMSMS_IgnorePrefixForBrewDirectedMsg(void) 
{
   FARF(SMS, ("==>  OEMSMS_IgnorePrefixForBrewDirectedMsg"));
   return FALSE;
}

/*===========================================================================
// This function checks provided SMS against OEM configurable list of 
// strings which must be treated as registered BREW handlers. 
===========================================================================*/
boolean OEMSMS_OEMDefinedHandler(const char * pszText, char ** ppszHandler)
{
   boolean bRetVal = FALSE;
   char ** ppszOEMDefinedHandler = (char **)aszOEMDefinedHandler;
   
   FARF(SMS, ("==>  OEMSMS_OEMDefinedHandler"));

   while (ppszOEMDefinedHandler < &aszOEMDefinedHandler[sizeof(aszOEMDefinedHandler)/sizeof(char*)])
   {
      if (0 == STRNICMP(*ppszOEMDefinedHandler, 
                        pszText + STRLEN("//"), 
                        STRLEN(*ppszOEMDefinedHandler))) 
      {
         bRetVal = TRUE;
         if (ppszHandler) {
            *ppszHandler = *ppszOEMDefinedHandler;
         }
         break;
      }
      ppszOEMDefinedHandler++;   
   }
   return bRetVal;
}

/*===========================================================================
                 PRIVATE METHOD DEFINITIONS
===========================================================================*/

/*===========================================================================

===========================================================================*/
static int OEMSMS_Delete( OEMSMSMgr *pme )
{   
   FARF(SMS, ("==>  OEMSMS_Delete"));

   if( pme == gpSMS ) {

      OEMSMS_Cleanup();
      gpSMS = (OEMSMSMgr *)0;
   }

   sys_free( pme );
   return SUCCESS;
}

/*===========================================================================

===========================================================================*/
static void OEMSMS_CancelListener( AEECallback *pcb )
{
   OEMSMSMgr *pme = (OEMSMSMgr *)pcb->pCancelData;
   AEECallback *pPrev = (AEECallback *)pcb->pmc;
   
   FARF(SMS, ("==>  OEMSMS_CancelListener"));

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != OEMSMS_CancelListener)
      return;
   
   if( pPrev ) {
      pPrev->pNext = pcb->pNext;
   }
   else {
      pme->pcbOnMTMessage = pcb->pNext;
   }
   if( pcb->pNext ) {
      pcb->pNext->pmc = (void *)pPrev;
   }

   pcb->pCancelData = 0;
   pcb->pfnCancel   = 0;
}


/*===========================================================================

===========================================================================*/
static void OEMSMS_ReleaseMessage( AEECallback *pcb )
{
   FARF(SMS, ("==>  OEMSMS_ReleaseMessage"));

  //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != OEMSMS_ReleaseMessage)
      return;
   
   OEMSMS_DeleteMessage( (OEMMsg*)pcb->pCancelData );

   pcb->pCancelData = 0;
   pcb->pfnCancel   = 0;
}



/*===========================================================================

  This returns the correct ascii char for a DIGIT_MODE_4_BIT char              

===========================================================================*/
char GetDigit(byte ch)
{
   switch(ch){
      case 0:
      case 10:
         return('0');
      case 11:
         return('*');
      case 12:
         return('#');
      default:
         if(ch >= 1 && ch <= 9){
            return( (char)('0' + ch));
         }
         break;
   }
   return(' ');
}

/*===========================================================================

  Converts a number character to its DTMF value

===========================================================================*/
byte GetDTMF (char ch) 
{
   switch (ch) {
      case '0':
         return 0x0a;
      case '*':
         return 0x0b;
      case '#':
         return 0x0c;
      default:
         if (ch >= '1' && ch <= '9') {
            return(byte)(ch - '0');
         }
   }
   return 0x00;
}

/*===========================================================================

  Converts an ASCII string to a DTMF string

===========================================================================*/
uint32 GetASCII2DTMF (char * pcBuf, byte * pBuf, uint32 dwLen) 
{
   uint32   i,j;
   byte     d;

   for (i = j = 0; i < dwLen; i++) 
   {
      if ((d = GetDTMF(pcBuf[i])) != 0) 
      {  
         // skip if char is not recognized
         pBuf[j] = d;  // change each ASCII digit to DTMF 
         j++;
      }
   }

   return(uint32)j;
}

