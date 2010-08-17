/*===========================================================================
                              OEMUASMSStorage.c

  OEM REFERENCE SMS SUPPORT ROUTINES
  
  This file contains a reference implementation of SMS support.
  This file interfaces with UASMS services of DMSS.
    
        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#include "OEMFeatures.h"

#ifdef FEATURE_UASMS

#ifdef FEATURE_RUIM_SMS
#include "uiuint.h"
#include "uiusmsl.h"
#endif // FEATURE_RUIM_SMS

#include "OEMSMS_priv.h"
#include "OEMSMSStorage.h"
#include "AEE.h"
#include "AEEStdLib.h"
#include "AEE_OEMDispatch.h"
#include "AEESMS.h"
#include "AEETAPI.h"
#include "AEE_OEMSMS.h"


#ifdef FEATURE_RUIM_SMS
typedef struct 
{
   boolean bStorageInitialized;
   uint8 ruimSMSCount;
   ui_sms_nv_list_type ruimSMSMsgList[MAX_NV_MSGS];
   smsi_msg_type smsi_msg;
   nv_item_type nvi;
   PFNNOTIFY pfnNotify;
   OEMStorageUpdateData sUpdateData;
} OEMUasmsStorage;

static OEMUasmsStorage gsStorage;

// Teleservice Mapping Between WMS and OEM layer SMS teleservice defines
typedef struct
{
   int                        OEM_TsId;
   smsi_teleservice_type      DMSS_TsId;
} TsIdMap;

static const TsIdMap   gsTsIdMap[] = 
                  {{SMS_TELESERVICE_CPT_95,              SMSI_CPT_95},
                  {SMS_TELESERVICE_CMT_95,               SMSI_CMT_95},
                  {SMS_TELESERVICE_VMN_95,               SMSI_VMN_95},
                  {SMS_TELESERVICE_IS91_PAGE,            SMSI_CMT_91_CLI},
                  {SMS_TELESERVICE_IS91_VOICE_MAIL,      SMSI_CMT_91_VOICE_MAIL},
                  {SMS_TELESERVICE_IS91_SHORT_MESSAGE,   SMSI_CMT_91_SHORT_MSG},
                  {SMS_TELESERVICE_MWI,                  SMSI_AWI_95}};

//Tag Mapping between DMSS and AEE layer tags
typedef struct
{
   int                                 AEE_Tag;
   smsi_msg_tag_type                   OEM_Tag;
} TagMap;

static const TagMap   gsTagMap[] =  
                  {{AEESMS_TAG_NONE,                     SMSI_INVALID},
                  {AEESMS_TAG_MT_READ,                   SMSI_MT_READ},
                  {AEESMS_TAG_MT_NOT_READ,               SMSI_MT_NOT_READ},
                  {AEESMS_TAG_MO_SENT,                   SMSI_MO_SENT},
                  {AEESMS_TAG_MO_NOT_SENT,               SMSI_MO_NOT_SENT},
                  {AEESMS_TAG_MO_SENT_ST_NOT_RECEIVED,   SMSI_INVALID},
                  {AEESMS_TAG_MO_SENT_ST_NOT_STORED,     SMSI_INVALID},
                  {AEESMS_TAG_MO_SENT_ST_STORED,         SMSI_INVALID},
                  {AEESMS_TAG_MO_TEMPLATE,               SMSI_MO_TEMPLATE},
                  {AEESMS_TAG_STATUS_RPT,                SMSI_INVALID}};

static void SMSIMSGToOEMMsg(OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, smsi_msg_type * psmsimsg);
static void OEMMsgToSMSIMSG(OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, smsi_msg_type * psmsimsg);
static int ClearCard(uint32 nIndex);

static uint32 Xlate_TS(smsi_teleservice_type ts);
static uint32 Xlate_Tag(smsi_msg_tag_type tag);

static smsi_teleservice_type Xlate_2_NativeTS(uint32 ts);
static smsi_msg_tag_type Xlate_2_NativeTag(int tag);

/*===========================================================================

   Initializes storage if not initialized. Returns TRUE if storage is initialized.
   Returns FALSE otherwise.

===========================================================================*/
boolean OEMSMS_IsStorageInitialized(AEESMSStorageType st)
{
   FARF(SMS, ("OEMSMS_IsStorageInitialized"));

   if (st != AEESMS_CARD_RUIM)
      return FALSE;

   if (gsStorage.bStorageInitialized == FALSE)
   {   
      MEMSET((void*)&gsStorage.nvi, 0, sizeof(gsStorage.nvi));

      // The maximum number of SMS messages that the 'UI' supports. 
      // (needed by nvruimsms_buildlist())
      gsStorage.nvi.sms.data[sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS] = MAX_NV_MSGS;
      gsStorage.nvi.sms.address = 0;
      
      ui_get_nv(NV_SMS_I, &gsStorage.nvi);   
      
      if (gsStorage.nvi.sms.data[sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS+4] == 0xdb) 
      {
         gsStorage.ruimSMSCount = gsStorage.nvi.sms.data[sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS];

         // Should never happen, but be paranoid
         if (gsStorage.ruimSMSCount > MAX_NV_MSGS) {
            gsStorage.ruimSMSCount = MAX_NV_MSGS;
         }

         // Index 0 is reserved for roaming list
         if (gsStorage.ruimSMSCount > 0)
            gsStorage.ruimSMSCount--;

         MEMCPY((void*)gsStorage.ruimSMSMsgList,(void*)&gsStorage.nvi.sms.data[sizeof(ui_sms_nv_list_type)], gsStorage.ruimSMSCount*sizeof(ui_sms_nv_list_type));
         gsStorage.bStorageInitialized = TRUE;
      } 
      else 
      {
         // Error
         gsStorage.ruimSMSCount    = 0;
      }
   }
   return gsStorage.bStorageInitialized;
}

/*===========================================================================

   Returns storage status

===========================================================================*/  
void OEMSMS_GetStorageStatus( AEESMSStorageType st, AEESMSTag mt, AEESMSStorageStatus * pss, uint32 * pnRv, AEECallback *pcb )
{
   int i, nErr = SUCCESS;
   OEMMsgData sMsgData;
   uint32 dwStatus;

   FARF(SMS, ("OEMSMS_GetStorageStatus"));

   if (st != AEESMS_CARD_RUIM)
   {   
      FARF(SMS, ("%d Storage Type not supported", st));
      nErr = EUNSUPPORTED;
      goto Done;
   }

   if (gsStorage.bStorageInitialized == FALSE) 
   {
      nErr = EFAILED;
      goto Done;
   }

   MEMSET(&sMsgData, 0, sizeof(OEMMsgData));
   MEMSET(pss, 0, sizeof(AEESMSStorageStatus));

   for (i = 0; i < gsStorage.ruimSMSCount; i++)
   {
      if (gsStorage.ruimSMSMsgList[i].list_cat == UI_SMS_AVAIL)
         pss->nFreeSlots++;

      OEMSMS_ReadMsg(st, i, &sMsgData, NULL, &dwStatus, NULL);

      if ((dwStatus == SUCCESS) && (mt == sMsgData.dwTag))
         pss->nUsedTagSlots++;
   }
   pss->nMaxSlots = gsStorage.ruimSMSCount;
   pss->nSize = sizeof(AEESMSStorageStatus);

Done:
   *pnRv = nErr;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================

   Stores a message on the card

===========================================================================*/  
void OEMSMS_StoreMsg( AEESMSStorageType st, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 * pnIndex, uint32 *pnRv, AEECallback *pcb )
{
   int nErr = SUCCESS, nIndex;

   FARF(SMS, ("OEMSMS_StoreMsg"));

   if (st != AEESMS_CARD_RUIM)
   {   
      FARF(SMS, ("%d Storage Type not supported", st));
      nErr = EUNSUPPORTED;
      goto Done;
   }
   
   if (gsStorage.bStorageInitialized == FALSE) 
   {
      nErr = EFAILED;
      goto Done;
   }

   for (nIndex = 0; nIndex < gsStorage.ruimSMSCount; nIndex++)
   {
      if (gsStorage.ruimSMSMsgList[nIndex].list_cat == UI_SMS_AVAIL)
         break;
   }

   if (nIndex >= gsStorage.ruimSMSCount)
   {
      nErr = EFAILED;
      goto Done;
   }
   
   MEMSET((void*)&gsStorage.nvi, 0, sizeof(gsStorage.nvi));
   MEMSET((void*)&gsStorage.smsi_msg, 0, sizeof(gsStorage.smsi_msg));

   OEMMsgToSMSIMSG(pMsgData, pMsgPayload, &gsStorage.smsi_msg);
   uasmsu_nv_data(TRUE, (void*)&gsStorage.nvi.sms, &gsStorage.smsi_msg, FALSE); 
   gsStorage.nvi.sms.address = (byte)(nIndex + 1);

   if (ui_put_nv(NV_SMS_I, &gsStorage.nvi) == NV_DONE_S) 
   {
      // Mark this message as in-use
      gsStorage.ruimSMSMsgList[nIndex].list_cat = UI_SMS_TXT_OLD;
      *pnIndex = nIndex;
   }
   else
      nErr = EFAILED;

Done:
   if (SUCCESS == nErr)
   {
      OEMStorageUpdateData * pUpdateData = NULL;

      if (gsStorage.pfnNotify)
      {
         pUpdateData = (OEMStorageUpdateData *)&gsStorage.sUpdateData;
         pUpdateData->nEvent = EVT_MDL_SMSSTORAGE_STORE;
         pUpdateData->sEventData.st = st;
         pUpdateData->sEventData.nIndex = nIndex;
         pUpdateData->sEventData.mt = pMsgData->dwTag;
         gsStorage.pfnNotify(pUpdateData);
      }
   }

   *pnRv = nErr;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================

   Reads a message from the card

===========================================================================*/  
void OEMSMS_ReadMsg( AEESMSStorageType st, uint32 nIndex, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 * pnRv, AEECallback *pcb )
{
   int nErr = SUCCESS;

   FARF(SMS, ("OEMSMS_ReadMsg"));

   if (st != AEESMS_CARD_RUIM)
   {   
      FARF(SMS, ("%d Storage Type not supported", st));
      nErr = EUNSUPPORTED;
      goto Done;
   }

   if (gsStorage.bStorageInitialized == FALSE)
   {
      nErr = EFAILED;
      goto Done ;
   }

   if (nIndex >= gsStorage.ruimSMSCount) 
   {
      nErr = EBADPARM;
      goto Done ;
   }

   MEMSET((void*)&gsStorage.nvi, 0, sizeof(gsStorage.nvi));
   gsStorage.nvi.sms.address = (byte)(nIndex + 1);

   if (ui_get_nv(NV_SMS_I, &gsStorage.nvi) == NV_DONE_S)
   {
      MEMSET((void*)&gsStorage.smsi_msg, 0, sizeof(gsStorage.smsi_msg));

      // Convert from NV packed format to OTA format
      uasmsu_nv_data(FALSE, (void*)&gsStorage.nvi.sms, &gsStorage.smsi_msg, FALSE); 

      // Convert to SMS applet format
      SMSIMSGToOEMMsg(pMsgData, pMsgPayload, &gsStorage.smsi_msg);
   }
   else
      nErr = EFAILED;

Done:
   *pnRv = nErr;

   if (pcb)
      AEE_SYS_RESUME(pcb);
}

/*===========================================================================

   Removes a message from the card

===========================================================================*/  
void OEMSMS_RemoveMsg( AEESMSStorageType st, AEESMSTag mt, uint32 nIndex, uint32 * pnRv, AEECallback *pcb )
{
   FARF(SMS, ("OEMSMS_RemoveMsg"));

   if (st != AEESMS_CARD_RUIM)
   {   
      FARF(SMS, ("%d Storage Type not supported", st));
      *pnRv = EUNSUPPORTED;
      goto Done;
   }

   if (nIndex == 0xffffffff)
   {
      int i;

      if (mt == AEESMS_TAG_NONE)
      {
         // Remove all messages
         for (i = 0; i < gsStorage.ruimSMSCount; i++)
         {
            if ((*pnRv = ClearCard(i)) != SUCCESS)
               break;
         }
      }
      else
      {
         // Remove messages of specified type 
         OEMMsgData sMsgData;
         uint32 dwStatus;

         MEMSET(&sMsgData, 0, sizeof(OEMMsgData));

         for (i = 0; i < gsStorage.ruimSMSCount; i++)
         {
            OEMSMS_ReadMsg(st, i, &sMsgData, NULL, &dwStatus, NULL);

            if ((dwStatus != SUCCESS) || (mt != sMsgData.dwTag))
               continue;

            if ((*pnRv = ClearCard(i)) != SUCCESS)
               break;
         }
      }
   }
   else
   {
      *pnRv = ClearCard(nIndex);
   }

Done:
   if (SUCCESS == *pnRv)
   {
      OEMStorageUpdateData * pUpdateData = NULL;

      if (gsStorage.pfnNotify)
      {
         pUpdateData = (OEMStorageUpdateData *)&gsStorage.sUpdateData;
      
         if (0xffffffff == nIndex) {
            pUpdateData->nEvent = (AEESMS_TAG_NONE == mt ? EVT_MDL_SMSSTORAGE_DELETE_ALL : EVT_MDL_SMSSTORAGE_DELETE_TAG);
         }
         else {
            pUpdateData->nEvent = EVT_MDL_SMSSTORAGE_DELETE;
         }

         pUpdateData->sEventData.st = st;
         pUpdateData->sEventData.nIndex = nIndex;
         pUpdateData->sEventData.mt = mt;
         gsStorage.pfnNotify(pUpdateData);
      }
   }

   AEE_SYS_RESUME(pcb);
}

/*===========================================================================

   Updates a message on the card

===========================================================================*/  
void OEMSMS_UpdateMsg( AEESMSStorageType st, uint32 nIndex, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 *pnRv, AEECallback *pcb )
{
   int nErr = SUCCESS;

   FARF(SMS, ("OEMSMS_UpdateMsg"));

   if (st != AEESMS_CARD_RUIM)
   {   
      FARF(SMS, ("%d Storage Type not supported", st));
      nErr = EUNSUPPORTED;
      goto Done;
   }
   
   if (gsStorage.bStorageInitialized == FALSE) 
   {
      nErr = EFAILED;
      goto Done;
   }

   if (nIndex >= gsStorage.ruimSMSCount)
   {
      nErr = EBADPARM;
      goto Done;
   }

   if (gsStorage.ruimSMSMsgList[nIndex].list_cat == UI_SMS_AVAIL)
   {
      nErr = EBADPARM;
      goto Done;
   }
   
   MEMSET((void*)&gsStorage.nvi, 0, sizeof(gsStorage.nvi));
   MEMSET((void*)&gsStorage.smsi_msg, 0, sizeof(gsStorage.smsi_msg));

   OEMMsgToSMSIMSG(pMsgData, pMsgPayload, &gsStorage.smsi_msg);
   uasmsu_nv_data(TRUE, (void*)&gsStorage.nvi.sms, &gsStorage.smsi_msg, FALSE); 
   gsStorage.nvi.sms.address = (byte)(nIndex + 1);

   if (ui_put_nv(NV_SMS_I, &gsStorage.nvi) == NV_DONE_S) 
   {
      // Mark this message as in-use
      gsStorage.ruimSMSMsgList[nIndex].list_cat = UI_SMS_TXT_OLD;
   }
   else
      nErr = EFAILED;

Done:
   if (SUCCESS == nErr)
   {
      OEMStorageUpdateData * pUpdateData = NULL;

      if (gsStorage.pfnNotify)
      {
         pUpdateData = (OEMStorageUpdateData *)&gsStorage.sUpdateData;
         pUpdateData->nEvent = EVT_MDL_SMSSTORAGE_UPDATE;
         pUpdateData->sEventData.st = st;
         pUpdateData->sEventData.nIndex = nIndex;
         pUpdateData->sEventData.mt = pMsgData->dwTag;
         gsStorage.pfnNotify(pUpdateData);
      }
   }

   *pnRv = nErr;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================

   Sets auto storage status for the messages of the specified class

===========================================================================*/
void OEMSMS_AutoStoreMsg(AEESMSClass mc, AEESMSStorageType st, uint32 *pnRv, AEECallback *pcb)
{   
   FARF(SMS, ("OEMSMS_AutoStoreMsg"));
   *pnRv = EFAILED;
   AEE_SYS_RESUME(pcb);
}
/*===========================================================================

   Clears card for specified index

===========================================================================*/  
static int ClearCard(uint32 nIndex)
{
   int nErr = SUCCESS;

   FARF(SMS, ("ClearCard"));

   if (gsStorage.bStorageInitialized == FALSE) 
   {
      nErr = EFAILED;
      goto Done;
   }

   if (nIndex >= gsStorage.ruimSMSCount) 
   {
      nErr = EBADPARM;
      goto Done;
   }

   if (gsStorage.ruimSMSMsgList[nIndex].list_cat == UI_SMS_AVAIL) 
   {
      // This message is already empty.  We're done!
      goto Done;
   }

   MEMSET((void*)&gsStorage.nvi, 0, sizeof(gsStorage.nvi));
   gsStorage.nvi.sms.address = (byte)(nIndex + 1);

   ui_free_nv(NV_SMS_I, &gsStorage.nvi);

   if (ui_free_nv(NV_SMS_I, &gsStorage.nvi) == NV_DONE_S)
   {
      // Mark this message as available.
      gsStorage.ruimSMSMsgList[nIndex].list_cat = UI_SMS_AVAIL;
   }
   else
      nErr = EFAILED;

Done:
   return nErr;
}

/*=============================================================================

  Converts DMSS SMS struct to OEMMsgData and OEMMsgPayload

=============================================================================*/
static void SMSIMSGToOEMMsg(OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, smsi_msg_type * psmsimsg)
{
   char * pBuffer = NULL;
   int nSize;

   FARF(SMS, ("SMSIMSGToOEMMsg"));

   if (pMsgData)
   {
      //copy the call back number
      if (psmsimsg->sms_param_m & SMSI_CALL_BACK) 
      {
         pBuffer = pMsgData->szCallback;
         nSize = sizeof(pMsgData->szCallback);

         if (psmsimsg->msg_data.call_back.num_type == UASMS_NUMBER_INTERNATIONAL)
         {
            *pBuffer = DIALER_PLUS_CHAR;
            pBuffer++;
            nSize--;
         }
         
         STRNCPY(pBuffer, (char*)psmsimsg->msg_data.call_back.chari, nSize);
      } 
      
      if ((psmsimsg->sms_param_m & SMSI_DEST_ADDR) ||
          (psmsimsg->sms_param_m & SMSI_ORIG_ADDR))
      {
         pBuffer = pMsgData->szMOFromMTTo;
         nSize = sizeof(pMsgData->szMOFromMTTo);

         if (psmsimsg->msg_data.address.num_type == UASMS_NUMBER_INTERNATIONAL)
         {
            *pBuffer = DIALER_PLUS_CHAR;
            pBuffer++;
            nSize--;
         }
         
         STRNCPY(pBuffer, (char*)psmsimsg->msg_data.address.chari, nSize);
         pMsgData->bIsMOSMS = (psmsimsg->sms_param_m & SMSI_DEST_ADDR ? TRUE : FALSE);
      }
      
      if (psmsimsg->sms_param_m & SMSI_MC_TIME_STAMP)
      {      
         uint8           tmp;
         clk_julian_type jt;

         MEMSET(&jt, 0, sizeof(jt));

         tmp = BCD2INT(psmsimsg->msg_data.mc_time.yr);
   
         // If the year is above 95 it we assume it is in 20 th century(1900+ year
         // otherwise we assume it is in 21 century(2000+year)
         jt.year = (tmp <= 95) ? (YEAR_2000 + tmp) : (YEAR_1900 + tmp);
         jt.month = BCD2INT(psmsimsg->msg_data.mc_time.mon);
         jt.day = BCD2INT(psmsimsg->msg_data.mc_time.day);
         jt.hour = BCD2INT(psmsimsg->msg_data.mc_time.hrs);
         jt.minute = BCD2INT(psmsimsg->msg_data.mc_time.min);
         jt.second = BCD2INT(psmsimsg->msg_data.mc_time.sec);

         pMsgData->dwTimeStamp = clk_julian_to_secs(&jt);
      }
      
      if (psmsimsg->sms_param_m & SMSI_PRIORITY) 
      {
         pMsgData->nPriority = psmsimsg->msg_data.priority;
      }

      if (psmsimsg->sms_param_m & SMSI_PRIVACY)
      {
         pMsgData->nPrivacy = psmsimsg->msg_data.privacy;   
      }
   
      if (psmsimsg->sms_param_m & SMSI_LANGUAGE) 
      {
         pMsgData->dwLang = psmsimsg->msg_data.lang;
      }

      if (psmsimsg->sms_param_m & SMSI_REPLY_OPTION)
      {
         pMsgData->bRequestDLAck = psmsimsg->msg_data.reply_opt.dak_req;
         pMsgData->bRequestUserAck = psmsimsg->msg_data.reply_opt.user_ack_req;
      }

      if (psmsimsg->sms_param_m & SMSI_NUM_OF_MSGS)
      {   
         pMsgData->nNumMsgs = 
         (psmsimsg->msg_data.num_of_msgs[0]-0x30)*10 + (psmsimsg->msg_data.num_of_msgs[1]-0x30);
      }

      pMsgData->ts = Xlate_TS(psmsimsg->tele_srv);

      pMsgData->dwTag = Xlate_Tag(psmsimsg->tag);

      if (psmsimsg->sms_param_m & SMSI_MSG_ID)
      {   
         pMsgData->dwMsgID = psmsimsg->sms_msg_id.message_id;
         if (UASMS_BD_TYPE_USER_ACK == psmsimsg->sms_msg_id.message_type)
         {
            pMsgData->bIsUserAck = TRUE;
         }
      }
   }

   if (pMsgPayload)
   {
      if (psmsimsg->sms_param_m & SMSI_USER_DATA)
      {
         pMsgPayload->nEncoding = Xlate_Encoding(psmsimsg->msg_data.user_data.msg_encoding);
         if ((AEESMS_ENC_IA5 == pMsgPayload->nEncoding) ||
            (AEESMS_ENC_ASCII == pMsgPayload->nEncoding))
         {
            // Change encoding as lower layers have done unpacking
            pMsgPayload->nEncoding = AEESMS_ENC_LATIN;
         }
         pMsgPayload->nBytes = psmsimsg->msg_data.user_data.num_fields;    
         MEMCPY((void*)pMsgPayload->data, (void*)psmsimsg->msg_data.user_data.chari, (uint32)psmsimsg->msg_data.user_data.num_fields);
      }
   }
}

/*=============================================================================

  Converts OEMMsgData and OEMMsgPayload to DMSS SMS struct

=============================================================================*/
static void OEMMsgToSMSIMSG(OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, smsi_msg_type * psmsimsg)
{
   FARF(SMS, ("OEMMsgToSMSIMSG"));

   if (pMsgData || pMsgPayload)  
      psmsimsg->sms_param_m |= SMSI_BEARER_DATA;

   if (pMsgData)
   {
      int nLength;

      // Strore Callback
      nLength = STRLEN(pMsgData->szCallback);

      if (nLength > 0) 
      {
         psmsimsg->sms_param_m |= SMSI_CALL_BACK;

         // Check for international numbers
         if (pMsgData->szCallback[0] == DIALER_PLUS_CHAR) 
         {
            STRNCPY((char*)psmsimsg->msg_data.call_back.chari, (char*)(pMsgData->szCallback + 1), sizeof(psmsimsg->msg_data.call_back.chari) - 1);
            psmsimsg->msg_data.call_back.num_type = UASMS_NUMBER_INTERNATIONAL;
            nLength--;
         } 
         else 
         {
            STRNCPY((char*)psmsimsg->msg_data.call_back.chari, (char*)pMsgData->szCallback, sizeof(psmsimsg->msg_data.call_back.chari) - 1);
            psmsimsg->msg_data.call_back.num_type = UASMS_NUMBER_UNKNOWN;
         }
         psmsimsg->msg_data.call_back.num_fields = (uint8) MIN(nLength, (int)sizeof(psmsimsg->msg_data.call_back.chari));
      } 
      
      // Store Destination
      nLength = STRLEN(pMsgData->szMOFromMTTo);

      if (nLength > 0) 
      {
         psmsimsg->sms_param_m |= (pMsgData->bIsMOSMS ? SMSI_DEST_ADDR : SMSI_ORIG_ADDR);

         // Check for international numbers
         if (pMsgData->szMOFromMTTo[0] == DIALER_PLUS_CHAR) 
         {
            STRNCPY((char*)psmsimsg->msg_data.address.chari, (char*)(pMsgData->szMOFromMTTo + 1), sizeof(psmsimsg->msg_data.address.chari) - 1);
            psmsimsg->msg_data.address.num_type = UASMS_NUMBER_INTERNATIONAL;
            nLength--;
         } 
         else 
         {
            STRNCPY((char*)psmsimsg->msg_data.address.chari, (char*)pMsgData->szMOFromMTTo, sizeof(psmsimsg->msg_data.address.chari) - 1);
            psmsimsg->msg_data.address.num_type = UASMS_NUMBER_UNKNOWN;
         }
         psmsimsg->msg_data.address.num_fields = (uint8) MIN(nLength, (int)sizeof(psmsimsg->msg_data.address.chari));
      } 

      // Store time stamp only for MT messages
      if (!pMsgData->bIsMOSMS && pMsgData->dwTimeStamp) 
      {
         clk_julian_type jt;

         psmsimsg->sms_param_m |= SMSI_MC_TIME_STAMP;

         clk_secs_to_julian(pMsgData->dwTimeStamp, &jt);

         if (jt.year <= 1995) 
         {
            psmsimsg->msg_data.mc_time.yr = INT2BCD((uint16)(jt.year - 2000));
         } 
         else 
         {
            psmsimsg->msg_data.mc_time.yr = INT2BCD((uint16)(jt.year - 1900));
         }
         psmsimsg->msg_data.mc_time.mon = INT2BCD(jt.month);
         psmsimsg->msg_data.mc_time.day = INT2BCD(jt.day);
         psmsimsg->msg_data.mc_time.hrs = INT2BCD(jt.hour);
         psmsimsg->msg_data.mc_time.min = INT2BCD(jt.minute);
         psmsimsg->msg_data.mc_time.sec = INT2BCD(jt.second);
      }

      psmsimsg->sms_param_m |= SMSI_PRIORITY;
      psmsimsg->msg_data.priority = (smsi_indicator_type) pMsgData->nPriority;

      psmsimsg->sms_param_m |= SMSI_PRIVACY;
      psmsimsg->msg_data.privacy = (smsi_indicator_type) pMsgData->nPrivacy;

      psmsimsg->sms_param_m |= SMSI_LANGUAGE;
      psmsimsg->msg_data.lang = (smsi_indicator_type) pMsgData->dwLang;

      psmsimsg->sms_param_m |= SMSI_REPLY_OPTION;
      psmsimsg->msg_data.reply_opt.dak_req = (byte) pMsgData->bRequestDLAck;
      psmsimsg->msg_data.reply_opt.user_ack_req = (byte) pMsgData->bRequestUserAck;

      psmsimsg->tele_srv = (smsi_teleservice_type) Xlate_2_NativeTS(pMsgData->ts);

      psmsimsg->tag = (smsi_msg_tag_type) Xlate_2_NativeTag(pMsgData->dwTag);

      if (!pMsgData->bIsMOSMS && pMsgData->nNumMsgs)
      {
         psmsimsg->sms_param_m |= SMSI_NUM_OF_MSGS;
         psmsimsg->msg_data.num_of_msgs[0] = (pMsgData->nNumMsgs/10) + 0x30;
         psmsimsg->msg_data.num_of_msgs[1] = (pMsgData->nNumMsgs%10) + 0x30;
      }
      
      if (pMsgData->dwMsgID)
      {
         psmsimsg->sms_param_m |= SMSI_MSG_ID;
         psmsimsg->sms_msg_id.message_id = (word) pMsgData->dwMsgID;
         if (pMsgData->bIsUserAck)
         {
            psmsimsg->sms_msg_id.message_type = (byte) UASMS_BD_TYPE_USER_ACK;
         }
      }
   }

   if (pMsgPayload)
   {
      psmsimsg->sms_param_m |= SMSI_USER_DATA;
      psmsimsg->msg_data.user_data.msg_encoding = (byte) Xlate_2_NativeEncoding(pMsgPayload->nEncoding);
      if ((AEESMS_ENC_IA5 == pMsgPayload->nEncoding) ||
          (AEESMS_ENC_ASCII == pMsgPayload->nEncoding))
      {
         char * pText = AEE_GetDecodedTextString(pMsgPayload->data, pMsgPayload->nBytes, pMsgPayload->nEncoding, NULL, NULL);
         // Unpack as its packed by lower layers
         if (NULL != pText)
         {
            psmsimsg->msg_data.user_data.data_len = (byte) MIN((pMsgPayload->nBytes*8 - pMsgPayload->nPadding)/7, sizeof(psmsimsg->msg_data.user_data.chari));
            MEMCPY((void*)psmsimsg->msg_data.user_data.chari, (void*)pText, (uint32)psmsimsg->msg_data.user_data.data_len);
            FREE(pText);
         }
      }
      else
      {
         psmsimsg->msg_data.user_data.data_len = (byte) MIN(pMsgPayload->nBytes, sizeof(psmsimsg->msg_data.user_data.chari));
         MEMCPY((void*)psmsimsg->msg_data.user_data.chari, (void*)pMsgPayload->data, (uint32)psmsimsg->msg_data.user_data.data_len);
      }
      psmsimsg->msg_data.user_data.num_fields = psmsimsg->msg_data.user_data.data_len;
   }
}

/*=============================================================================

  Initializes message list for enumeration

=============================================================================*/
int OEMSMS_GetEnumMsgIDs(AEESMSStorageType st, AEESMSTag mt, uint32 * pdwIndex, uint32 * pdwSize )
{
   OEMMsgData sMsgData; 
   uint32 dwStatus;
   int nErr = SUCCESS, i = 0, nCount = 0;
   FARF(SMS, ("OEMSMS_GetEnumMsgIDs"));

   if (!pdwSize || (st != AEESMS_CARD_RUIM))
   {
      return EBADPARM;
   }
   
   if (AEESMS_TAG_NONE == mt)
   {
      // Tag none means don't care here so count all messages
      for (i = 0; i < gsStorage.ruimSMSCount; i++)
      {         
         if (gsStorage.ruimSMSMsgList[i].list_cat == UI_SMS_TXT_OLD)
         {
            nCount++;

            if (*pdwSize >= nCount*sizeof(uint32))
            {
               *pdwIndex++ = i;
            }
         }
      }
   }
   else
   {
      // Count messages of specific tag type
      for (i = 0; i < gsStorage.ruimSMSCount; i++)
      {
         if (gsStorage.ruimSMSMsgList[i].list_cat == UI_SMS_TXT_OLD)
         {
            MEMSET(&sMsgData, 0, sizeof(sMsgData));
            OEMSMS_ReadMsg(st, i, &sMsgData, NULL, &dwStatus, NULL);

            if ((gsStorage.ruimSMSMsgList[i].list_cat == UI_SMS_TXT_OLD) &&
                (dwStatus == SUCCESS) &&
                (sMsgData.dwTag == mt))
            {
               nCount++;

               if (*pdwSize >= nCount*sizeof(uint32))
               {
                  *pdwIndex++ = i;
               }
            }
         }
      }
   }

   *pdwSize = nCount*sizeof(uint32);
   
   return nErr;
}

/*===========================================================================

  Translates DMSS Teleservice ID to OEM Teleservice ID

===========================================================================*/
static uint32 Xlate_TS(smsi_teleservice_type ts)
{
   TsIdMap* pe;
   int i,nEntries = sizeof(gsTsIdMap) / sizeof(TsIdMap);

   for(i = 0, pe = (TsIdMap *)gsTsIdMap; i < nEntries; pe++,i++)
   {
      if(pe->DMSS_TsId == ts)
         return(pe->OEM_TsId);
   }

   // Assume DMSS uses standard TS IDs so that we dont need a mapping table and 
   // we can return the same TS ID
   return(ts); 
}

/*===========================================================================

  Translates OEM Teleservice ID to DMSS Teleservice ID

===========================================================================*/
static smsi_teleservice_type Xlate_2_NativeTS(uint32 ts)
{
   TsIdMap* pe;
   int i,nEntries = sizeof(gsTsIdMap) / sizeof(TsIdMap);

   for(i = 0, pe = (TsIdMap *)gsTsIdMap; i < nEntries; pe++,i++)
   {
      if(pe->OEM_TsId == ts)
         return(pe->DMSS_TsId);
   }

   // Assume DMSS uses standard TS IDs so that we dont need a mapping table and 
   // we can return the same TS ID
   return(ts); 
}

/*===========================================================================

  Translates to AEE tag

===========================================================================*/
static uint32 Xlate_Tag(smsi_msg_tag_type tag)
{
   int i;

   for (i = 0; i < sizeof(gsTagMap)/sizeof(TagMap); i++)
   {
      if (gsTagMap[i].OEM_Tag == tag)
         return gsTagMap[i].AEE_Tag;
   }
   return AEESMS_TAG_NONE;
}

/*===========================================================================

  Translates to native tag

===========================================================================*/
static smsi_msg_tag_type Xlate_2_NativeTag(int tag)
{
   int i;

   for (i = 0; i < sizeof(gsTagMap)/sizeof(TagMap); i++)
   {
      if (gsTagMap[i].AEE_Tag == tag)
         return gsTagMap[i].OEM_Tag;
   }
   return SMSI_INVALID;
}

/*===========================================================================
Called by ISMSStorage to register callback to be invoked on update in storage
status
===========================================================================*/
void OEMSMS_OnStorageUpdate(PFNNOTIFY pfnNotify)
{
   FARF(SMS, ("==>  OEMSMS_OnStorageUpdate"));
   gsStorage.pfnNotify = pfnNotify;
}

#else /*FEATURE_RUIM_SMS*/
boolean OEMSMS_IsStorageInitialized(AEESMSStorageType st)
{   
   FARF(SMS, ("OEMSMS_IsStorageInitialized - EUNSUPPORTED"));
   return TRUE;
}

void OEMSMS_GetStorageStatus( AEESMSStorageType st, AEESMSTag mt, AEESMSStorageStatus * pss, uint32 * pnRv, AEECallback *pcb )
{   
   FARF(SMS, ("OEMSMS_GetStorageStatus - EUNSUPPORTED"));
   *pnRv = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

void OEMSMS_StoreMsg( AEESMSStorageType st, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 * pnIndex, uint32 *pnRv, AEECallback *pcb )
{   
   FARF(SMS, ("OEMSMS_StoreMsg - EUNSUPPORTED"));
   *pnRv = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

void OEMSMS_ReadMsg( AEESMSStorageType st, uint32 nIndex, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 * pnRv, AEECallback *pcb )
{   
   FARF(SMS, ("OEMSMS_ReadMsg - EUNSUPPORTED"));
   *pnRv = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

void OEMSMS_RemoveMsg( AEESMSStorageType st, AEESMSTag mt, uint32 nIndex, uint32 * pnRv, AEECallback *pcb )
{   
   FARF(SMS, ("OEMSMS_RemoveMsg - EUNSUPPORTED"));
   *pnRv = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

int OEMSMS_GetEnumMsgIDs(AEESMSStorageType st, AEESMSTag mt, uint32 * pdwIndex, uint32 * pdwSize )
{   
   FARF(SMS, ("OEMSMS_GetEnumMsgIDs - EUNSUPPORTED"));
   return EUNSUPPORTED;
}

void OEMSMS_UpdateMsg( AEESMSStorageType st, uint32 nIndex, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 *pnRv, AEECallback *pcb )
{   
   FARF(SMS, ("OEMSMS_UpdateMsg - EUNSUPPORTED"));
   *pnRv = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

void OEMSMS_AutoStoreMsg(AEESMSClass mc, AEESMSStorageType st, uint32 *pnRv, AEECallback *pcb)
{   
   FARF(SMS, ("OEMSMS_AutoStoreMsg - EUNSUPPORTED"));
   *pnRv = EUNSUPPORTED;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================
Never called for !FEATURE_RUIM_SMS, but here for linkage
===========================================================================*/
void OEMSMS_OnStorageUpdate(PFNNOTIFY pfnNotify)
{
   FARF(SMS, ("==>  OEMSMS_OnStorageUpdate"));
}
#endif /*FEATURE_RUIM_SMS*/

#endif /*FEATURE_UASMS*/

