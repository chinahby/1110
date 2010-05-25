/*===========================================================================

                     S M S M e s s a g e S t o r e . c
                

GENERAL DESCRIPTION
   This file contains functions used to store and retrieve SMS messages.
 
EXTERNALIZED FUNCTIONS
   GSM1xSMSMessageStore_ResetMessageStruct
   GSM1xSMSMessageStore_GetNextMessage
   GSM1xSMSMessageStore_GetMessageByID
   GSM1xSMSMessageStore_EraseEntireMBox
   GSM1xSMSMessageStore_EraseMessageFromMBox
   GSM1xSMSMessageStore_EraseOldMessagesFromMBox
   GSM1xSMSMessageStore_GetAddresseeAtIndex
   GSM1xSMSMessageStore_StorePrefs
   GSM1xSMSMessageStore_RestorePrefs

INITIALIZATION AND SEQUENCING REQUIREMENTS
   Database services must be available

Copyright (c) 2001,2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "GSM1xSMSApp.h"
#include "GSM1xSMSMessageStore.h"
#include "GSM1xSMSUtil.h"
#include "AEEGSMSMS.h" 

/*===========================================================================

                    LOCAL/STATIC DATA 

===========================================================================*/
static boolean ConvertGSMToInternal
(
   SMSApp *pMe,
   GSMSMSRawMsg *pRawMsg,
   SMSMessageStruct *internalMsg
);


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/
static void eMBoxToMask(SMSMBoxType mbox, uint16 *mask, GSMSMSStorageType *store);
static boolean AddressMatch(const GSMSMSAddress *pA, const GSMSMSAddress *pB);

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_ResetMessageStruct
  
DESCRIPTION:
   Reset the fields of the message struct to default values

PROTOTYPE:
   void GSM1xSMSMessageStore_ResetMessageStruct(SMSMessageStruct *pMsg);

PARAMETERS:
   pMsg [in]: Pointer to messagestruct to reset.

RETURN VALUE:
   None.

SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSMessageStore_ResetMessageStruct(SMSMessageStruct * pMsg) {
   if (pMsg) {
      MEMSET(pMsg->m_szFrom, 0, MAX_FROM_CHARS * sizeof(AECHAR));
      MEMSET(pMsg->m_szTo, 0, MAX_TO_CHARS * sizeof(AECHAR));
      MEMSET(pMsg->m_szTitle, 0, MAX_TITLE_LEN * sizeof(AECHAR));
      MEMSET(pMsg->m_szMessage, 0, MAX_MSG_LEN * sizeof(AECHAR));
      pMsg->m_dwTimeStamp = 0;
      pMsg->m_dwNumMsgs = 0;
      pMsg->m_dwMBox = SMS_MB_NONE;
      pMsg->m_eEncoding = GSMSMS_ENCODING_UCS2;
      pMsg->m_bNew = FALSE;
   }
}

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_GetNextMessage
  
DESCRIPTION:
   Fills in the szNextMsg struct with the next message in the message store.

PROTOTYPE:
   uint16 GSM1xSMSMessageStore_GetNextMessage(SMSApp * pMe,
                                         uint16 *pwID,
                                         SMSMessageStruct *pNextMsg,
                                         boolean bSetRead);

PARAMETERS:
    pMe [in]: Pointer to the SMS applet structure.

   pwID [in,out]: Pointer to ID of the last message, filled with ID of message
   that is returned.

   pNextMsg [out]: Struct into which to place message data.
   
   bSetRead [in]: Whether or not to set the message as having been read

RETURN VALUE:
    TRUE if message returned is valid. FALSE otherwise.

COMMENTS:
    Getting a message from the database doesn't necessarily mean
    the caller wants the message to display its contents, but perhaps to
    iterate through the messages to find a particular type. Therefore, if
    the message is being retreived for this purpose, we shouldn't mark
    the message as having been read.

SIDE EFFECTS:
   Message is marked read if bSetRead is TRUE and the message has never been
   read before.

   The bSetRead flag is currently ignored when reading from the SIM box
=============================================================================*/
boolean GSM1xSMSMessageStore_GetNextMessage(SMSApp *pMe,
                                       uint16 *pwID,
                                       SMSMessageStruct *pNextMsg,
                                       boolean bSetRead) 
{
   uint16  i = 0;
   uint16  numMsgs;
   uint16  wID = *pwID;
   GSMSMSRawMsg rawMsg;
   GSMSMSStorageType store;
   int errType;

   switch(pMe->m_eMBoxType)
   {
      case SMS_MB_INBOX:
         if (pMe->m_wNumInMsgs == 0)
         {
            return FALSE;
         }
         store = GSMSMS_STORE_NVRAM;
         break;
      case SMS_MB_OUTBOX:
         if (pMe->m_wNumOutMsgs == 0)
         {
            return FALSE;
         }
         store = GSMSMS_STORE_NVRAM;
         break;
      case SMS_MB_SIM:
         if (pMe->m_wNumSIMMsgs == 0)
         {
            return FALSE;
         }
         store = GSMSMS_STORE_SIM;
         break;
      default:
         // Not a recognized mailbox for this operation
         ASSERT_NOT_REACHABLE;
         return FALSE;
   }

   errType = IGSMSMS_GetStoreSize(pMe->m_pIGSMSMS, store, &numMsgs);
   if (errType != AEE_GSMSMS_SUCCESS)
   {
      DBGPRINTF("IGSMSMS_GetStoreSize failed");
      return FALSE;
   }

   if (wID == SMS_MESSAGE_STORE_INDEX_NONE)
   {
      wID = 0;
   }
   else if (wID >= numMsgs)
   {
      return FALSE;
   }
   else
   {
     wID++;
   }

   for(i=wID; i<numMsgs; i++)
   {
      errType = IGSMSMS_GetMessage(pMe->m_pIGSMSMS, i, &rawMsg, store);
      if (errType != AEE_GSMSMS_SUCCESS)
      {
         DBGPRINTF("IGSMSMS_GetMessage failed");
         return FALSE;
      }
      // If this is the next outbox/SIM outbox message
      if ((pMe->m_eMBoxType == SMS_MB_INBOX || 
           pMe->m_eMBoxType == SMS_MB_SIM) &&
          (rawMsg.msg.sim.status >= GSMSMS_STATUS_MT_READ &&
           rawMsg.msg.sim.status <= GSMSMS_STATUS_MT_NOT_READ))
      {
         if (ConvertGSMToInternal(pMe, &rawMsg, pNextMsg) == FALSE)
         {
             DBGPRINTF("ConvertGSMToInternal failed");
             return FALSE;
         }
         *pwID = i;
         return TRUE;
      }
      // If this is the next inbox/sim inbox message
      else if ((pMe->m_eMBoxType == SMS_MB_OUTBOX || pMe->m_eMBoxType == SMS_MB_SIM) &&
          (rawMsg.msg.sim.status >= GSMSMS_STATUS_MO_SENT &&
           rawMsg.msg.sim.status <= GSMSMS_STATUS_MO_SENT_ST_STORED))
      {
         if (ConvertGSMToInternal(pMe, &rawMsg, pNextMsg) == FALSE)
         {
             DBGPRINTF("ConvertGSMToInternal failed");
             return FALSE;
         }
         *pwID = i;
         return TRUE;
      }
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_GetMessageByID
  
DESCRIPTION:
   Fills in the msMsg struct with the data for the message matching wID.

PROTOTYPE:
   uint16 GSM1xSMSMessageStore_GetMessageByID(SMSApp * pMe,
                SMSMBoxType eMBoxType,
                                         uint16 wID,
                                         SMSMessageStruct *pMsg,
                boolean bSetRead);

PARAMETERS:
   pMe [in] : SMS applet data structure

   eMBoxType [in]: Which mailbox to check.

   wID [in]: ID of the message to return.

   pMsg [out]: Struct into which to place message data.
   
   bSetRead [in]: Whether or not to set the message as having been read

RETURN VALUE:
   TRUE if pMsg contains a valid message.  FALSE otherwise.

SIDE EFFECTS:
   Message is marked read if bSetRead is TRUE and the message has never been
   read before.

=============================================================================*/
boolean GSM1xSMSMessageStore_GetMessageByID(SMSApp * pMe,
                                       SMSMBoxType eMBoxType,
                                       uint16 wID,
                                       SMSMessageStruct *pMsg,
                                       boolean bSetRead) {
   
   GSMSMSRawMsg      simMsg;
   GSMSMSStorageType readFrom;

   if (!pMe || !pMsg) {
      return FALSE;
   }

   if (pMe->m_eMBoxType != eMBoxType)
   {
      ASSERT_NOT_REACHABLE;
   }
   
   switch (eMBoxType)
   {
   case SMS_MB_INBOX:
   case SMS_MB_OUTBOX:
      readFrom = GSMSMS_STORE_NVRAM;
      break;
   case SMS_MB_VOICEMAIL:
      readFrom = GSMSMS_STORE_NVRAM_VM;
      break;
   case SMS_MB_SIM:
      readFrom = GSMSMS_STORE_SIM;
      break;
   default:
      return FALSE;
   }

   pMsg->m_dwMBox = eMBoxType;

   if (IGSMSMS_GetMessage(pMe->m_pIGSMSMS, wID, &simMsg, readFrom) != SUCCESS)
   {
      return FALSE;
   }
   if (bSetRead)
   {
      if (simMsg.msg.sim.status == GSMSMS_STATUS_MT_NOT_READ)
      {
          if (IGSMSMS_SetMessageStatus(pMe->m_pIGSMSMS, wID, GSMSMS_STATUS_MT_READ, readFrom) != SUCCESS)
          {
             return FALSE;
          }

          switch (eMBoxType)
          {
          case SMS_MB_INBOX:
          case SMS_MB_OUTBOX:
             pMe->m_bNeedNVRAMRefresh = TRUE;
             break;
          case SMS_MB_VOICEMAIL:
             pMe->m_bNeedVMRefresh = TRUE;
             break;
          case SMS_MB_SIM:
             pMe->m_bNeedSIMRefresh = TRUE;
             break;
          default:
             break;
          }

          // Update annunciator in case last new message was read
          GSM1xSMSApp_UpdateMboxCounters(pMe);
      }
   }

   // Fill in pMsg
   if (ConvertGSMToInternal(pMe, &simMsg, pMsg) == FALSE)
   {
       return FALSE;
   }

   return TRUE;
}

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_EraseEntireMBox

DESCRIPTION:
   Erases all of the messages in the specified mailbox.

PROTOTYPE:
   boolean GSM1xSMSMessageStore_EraseEntireMBox(SMSApp * pMe, 
                  SMSMBoxType eMBoxType);

PARAMETERS:
   pMe [in]: Pointer to the SMS applet structure; 
    
   eMBoxType [in]: Which mailbox to completely erase.

RETURN VALUE:
   TRUE if successful.  FALSE otherwise.

SIDE EFFECTS:
   Will reset the message count upon success
=============================================================================*/
boolean GSM1xSMSMessageStore_EraseEntireMBox(SMSApp * pMe, SMSMBoxType eMBoxType) 
{

   GSMSMSStorageType store;
   uint16 mask;
   int rv;

   if (!pMe) {
      return FALSE;
   }

   switch (eMBoxType) {
      case SMS_MB_VOICEMAIL:
      case SMS_MB_INBOX:
      case SMS_MB_OUTBOX:
      case SMS_MB_SIM:

         eMBoxToMask(eMBoxType, &mask, &store);
         
         rv = IGSMSMS_DeleteAllMessages(pMe->m_pIGSMSMS, mask, store);
         if (rv == AEE_GSMSMS_SUCCESS)
         {
            switch (eMBoxType) {
               case SMS_MB_VOICEMAIL:
                  pMe->m_bNeedVMRefresh = TRUE;
                  break;
               case SMS_MB_INBOX:
               case SMS_MB_OUTBOX:
                  pMe->m_bNeedNVRAMRefresh = TRUE;
                  break;
               case SMS_MB_SIM:
                  pMe->m_bNeedSIMRefresh = TRUE;
                  break;
               default:
                  break;
            }

            // Update annunciator
            GSM1xSMSApp_UpdateMboxCounters(pMe);

            GSM1xSMSApp_CheckIfSMMARequired(pMe, FALSE);
            return TRUE;
         }
         break;

      default:
         break;
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_EraseMessageFromMBox
  
DESCRIPTION:
   Erases the indicated message from the specified mailbox.

PROTOTYPE:
   boolean GSM1xSMSMessageStore_EraseMessageFromMBox(SMSApp *pMe,
                  SMSMBoxType eMBoxType, 
                  uint16 wID);

PARAMETERS:
    pMe [in]: Pointer the SMS applet structure

   eMBoxType [in]: Indicates from which mailbox to erase the message.

   wID [in]: ID of the message to erase.

RETURN VALUE:
   TRUE if successful.  FALSE otherwise.

SIDE EFFECTS:
   Will decrement the message count upon success.
=============================================================================*/
boolean GSM1xSMSMessageStore_EraseMessageFromMBox(SMSApp * pMe, 
                                  SMSMBoxType eMBoxType,
                                  uint16 wID) 
{
   int rv;
   uint16 mask;
   GSMSMSStorageType store;

   if (pMe == NULL) 
   {
      return FALSE;
   }
   
   eMBoxToMask(eMBoxType, &mask, &store);
         
   rv = IGSMSMS_DeleteMessage(pMe->m_pIGSMSMS, wID, store);
   
   if (rv == AEE_GSMSMS_SUCCESS)
   {
      switch (eMBoxType) {
         case SMS_MB_VOICEMAIL:
            pMe->m_bNeedVMRefresh = TRUE;
            break;
         case SMS_MB_INBOX:
         case SMS_MB_OUTBOX:
            pMe->m_bNeedNVRAMRefresh = TRUE;
            break;
         case SMS_MB_SIM:
            pMe->m_bNeedSIMRefresh = TRUE;
            break;
         default:
            break;
      }

      // Update annunciator
      GSM1xSMSApp_UpdateMboxCounters(pMe);

      GSM1xSMSApp_CheckIfSMMARequired(pMe, FALSE);
      return TRUE;
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_GetAddresseeAtIndex
  
DESCRIPTION:
   Fills in szBuf with the data for addressee for the message
   wMsgID in mailbox eMBoxType.

PROTOTYPE:
   void GSM1xSMSMessageStore_GetAddresseeAtIndex(SMSApp * pMe,
                                     SMSMBoxType eMBoxType,
                                        uint16 wMsgID,
                                  AECHAR *szBuf,
                                  uint16 wBufSize)

PARAMETERS:
    pMe [in]: Pointer the applet data structure.

   eMBoxType [in]: Which mailbox to check.

   wMsgID [in]: ID of the message.

   pszBuf [out]: Addressee data.

   wBufSize [in]: Size of pszBuf.                           f

RETURN VALUE:
   None

COMMENTS:
   None

SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSMessageStore_GetAddresseeAtIndex(SMSApp *pMe,
                                         SMSMBoxType eMBoxType,
                                         uint16 wMsgID,
                                         AECHAR *pszBuf,
                                         uint16 wBufSize) 
{
   GSMSMSRawMsg      rawMsg;
   GSMSMSMsg         gsmMsg;
   GSMSMSStorageType fromStore;
   int               errType;

   if (eMBoxType == SMS_MB_OUTBOX)
   {
	   DBGPRINTF("Error in adressee function.  Shouldn't be here while in Outbox");
	   return;
   }

   else if (eMBoxType == SMS_MB_INBOX)
   {
	  MEMCPY(pszBuf, pMe->m_msIn.m_szFrom, wBufSize);
	  return;
   }
   else
   {
      fromStore = GSMSMS_STORE_SIM;
   

	//Note: this function should not be fetching the message from the 
    //store.  There should be a flag in the pMe struct that facilitates
    //this operation.  That would be a good optimization.

	errType = IGSMSMS_GetMessage(pMe->m_pIGSMSMS, wMsgID, &rawMsg, fromStore);
	if(errType != AEE_GSMSMS_SUCCESS)
	{
      DBGPRINTF("IGSMSMS_GetMessage failed");
      return;
	}

	errType = IGSMSMS_DecodeMessage(pMe->m_pIGSMSMS, &rawMsg, &gsmMsg);
	if(errType != AEE_GSMSMS_SUCCESS)
	{
      DBGPRINTF("IGSMSMS_DecodeMessage failed");
      return;
	}

	switch (gsmMsg.msgType)
	{
	case GSMSMS_MSG_SMS_SUBMIT:
      STRTOWSTR("Me", pszBuf, wBufSize);
      return;
	case GSMSMS_MSG_SMS_DELIVER:
	  MEMCPY(pszBuf, pMe->m_msIn.m_szFrom, wBufSize);
      return;
	default:
      break;
	}


	// Message not valid, send back empty string
	pszBuf[0] = (AECHAR)'\0';
   }
}

static void eMBoxToMask(SMSMBoxType mbox, uint16 *mask, GSMSMSStorageType *store)
{
   switch(mbox)
   {
   case SMS_MB_VOICEMAIL:
      *mask = GSMSMS_SIM_MT;
      *store = GSMSMS_STORE_NVRAM_VM;
      break;
   case SMS_MB_OUTBOX:
      *mask = GSMSMS_SIM_MO;
      *store = GSMSMS_STORE_NVRAM;
      break;
   case SMS_MB_INBOX:
      *mask = GSMSMS_SIM_MT;
      *store = GSMSMS_STORE_NVRAM;
      break;
   case SMS_MB_SIM:
      *mask = GSMSMS_SIM_ALL;
      *store = GSMSMS_STORE_SIM;
      break;
   default:
      break;
   }
}


/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_StorePrefs
  
DESCRIPTION:
   Save the current preferences to the preferences file.

PROTOTYPE:

PROTOTYPE:
   void GSM1xSMSMessageStore_StorePrefs(SMSApp *pMe);

PARAMETERS:
   pMe [in]: Pointer to the AEEApplet structure.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSMessageStore_StorePrefs(SMSApp *pMe) {

   if (!pMe) {
      return;
   }

   (void)ISHELL_SetPrefs(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS, SMS_CFG_VERSION, 
                    &pMe->m_cfg, sizeof(GSM1xSMSAppConfig));

}


/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_RestorePrefs
  
DESCRIPTION:
   Restore the current preferences from the preferences file.

PROTOTYPE:

PROTOTYPE:
   void GSM1xSMSMessageStore_RestorePrefs(SMSApp *pMe);

PARAMETERS:
   pMe [in]: Pointer to the AEEApplet structure.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSMessageStore_RestorePrefs(SMSApp *pMe) {

   if (!pMe) {
      return;
   }

   (void)ISHELL_GetPrefs(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS, SMS_CFG_VERSION,
                    &pMe->m_cfg, sizeof(GSM1xSMSAppConfig));
}

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_EraseOldMessagesFromInbox

DESCRIPTION:
    Erases all old messages (messages which are already read by the user)
    from the Inbox.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.

RETURN VALUE:
    None

COMMENTS:
    None
    
SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
void GSM1xSMSMessageStore_EraseOldMessagesFromInbox(SMSApp *pMe)
{
   uint16   numMsgs = 0;
   uint16   i;
   int errType;
   GSMSMSStatusType status;

   if(!pMe)
   {
      return;
   }

   errType = IGSMSMS_GetStoreSize(pMe->m_pIGSMSMS, GSMSMS_STORE_NVRAM, &numMsgs);
 
   if (errType != AEE_GSMSMS_SUCCESS)
   {
      DBGPRINTF("IGSMSMS_GetStoreSize failed");
      return;
   }

   if(numMsgs == 0)
   {
      // No messages to erase. 
      return;
   }

   pMe->m_wNumInMsgs = 0;
   for(i=0; i<= numMsgs;i++)
   {
      errType = IGSMSMS_GetMessageStatus(pMe->m_pIGSMSMS, i, &status, GSMSMS_STORE_NVRAM);
      if(errType == AEE_GSMSMS_SUCCESS)
      {
         if (status == GSMSMS_STATUS_MT_READ)
         {
            // Message is already read. Delete it now.
            (void)IGSMSMS_DeleteMessage(pMe->m_pIGSMSMS, i, GSMSMS_STORE_NVRAM);
         }
         else if (status == GSMSMS_STATUS_MT_NOT_READ)
         {
            // Update the m_wNumInMsgs with the number of messages in the Inbox.
            pMe->m_wNumInMsgs++;
         }
      }
   }
}

/*=============================================================================
FUNCTION:  ConvertGSMToInternal
 
DESCRIPTION: Converts the message format on the SIM to the internal 
             format used by the Applet
 
PARAMETERS:
   *simMsg:      [in]  Pointer to the SMS data retrieved from the SIM
   *internalMsg: [out] Pointer to the SMS data formatted for the applet
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   None

SEE ALSO:

=============================================================================*/
static boolean ConvertGSMToInternal(SMSApp *pMe, GSMSMSRawMsg *pRawMsg, SMSMessageStruct *internalMsg)
{
   GSMSMSMsg gsm;
   byte failureCause;

   if ((failureCause = IGSMSMS_DecodeMessage(pMe->m_pIGSMSMS, pRawMsg, &gsm)) != SUCCESS)
   {
      DBGPRINTF("Decode of SIM Msg failed (%d)", failureCause);
      return FALSE;
   }

   internalMsg->m_scAddr.addr[0] = '\0';

   if (gsm.msgType == GSMSMS_MSG_SMS_SUBMIT)
   {
       if (pMe->m_eMBoxType == SMS_MB_SIM)
       {
          internalMsg->m_dwMBox = SMS_MB_SIM;
       }
       else
       {
          internalMsg->m_dwMBox = SMS_MB_OUTBOX;
       }
 
       internalMsg->m_eEncoding = gsm.msg.SMSSubmit.DCS.msgCoding;

       // How do I get the time for SMS_SUBMIT Messages?
       internalMsg->m_dwTimeStamp = 0;

       // copy the destination address
       GSM1xSMSApp_DecodeAddress(&gsm.msg.SMSSubmit.DA, internalMsg->m_szTo, sizeof(internalMsg->m_szTo));
   }
   else
   {
       if (pMe->m_eMBoxType ==  SMS_MB_SIM)
       {
          internalMsg->m_dwMBox = SMS_MB_SIM;
       }
       else
       {
          internalMsg->m_dwMBox = SMS_MB_INBOX;
       }
       internalMsg->m_eEncoding = gsm.msg.SMSDeliver.DCS.msgCoding;
       internalMsg->m_dwTimeStamp = 
          GSM1xSMSUtil_GSMTime2Secs(&gsm.msg.SMSDeliver.SCTS);

       // copy the sender address
       GSM1xSMSApp_DecodeAddress(&gsm.msg.SMSDeliver.OA, internalMsg->m_szFrom, sizeof(internalMsg->m_szFrom));

       // if RP bit set, save SC address
       if (gsm.msg.SMSDeliver.RP == TRUE)
       {
          internalMsg->m_scAddr = gsm.msg.SMSDeliver.SCAddr;
       }
   }

   // Decode the UD 
   IGSMSMS_DecodeUserData(pMe->m_pIGSMSMS, &gsm, internalMsg->m_szMessage, sizeof(internalMsg->m_szMessage));

   // create the title
   WSTRNCOPYN(internalMsg->m_szTitle, 
      sizeof(internalMsg->m_szTitle)/sizeof(AECHAR),
      internalMsg->m_szMessage,
      (sizeof(internalMsg->m_szMessage)/sizeof(AECHAR))-1);

   internalMsg->m_dwNumMsgs = 0;
   internalMsg->m_eClass = gsm.msg.SMSDeliver.DCS.cg.generalData.msgClass;

   // Applies to both MO and MT messages
   if (pRawMsg->format == GSMSMS_RAW_FORMAT_SIM && ( // <- this part should be true in this fn
       (pRawMsg->msg.sim.status == GSMSMS_STATUS_MT_NOT_READ) ||
       (pRawMsg->msg.sim.status == GSMSMS_STATUS_MO_NOT_SENT) ) )
   {
      internalMsg->m_bNew = TRUE;
   }  
   else
   {
      internalMsg->m_bNew = FALSE;
   }

   return TRUE;
}

/*=============================================================================
FUNCTION:  GSM1xSMSMessageStore_MatchSMS
 
DESCRIPTION: Finds a matching SMS message for an incoming status report message
 
PARAMETERS:
   *pMe:      [in]  Pointer to the SMSApp instance
   MR:        [in]  Message reference to match
   *pIndex    [out] Pointer to index to set with matching SMS
   store:     [in]  Storage to search (i.e. NVRAM, SIM)
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   None

SEE ALSO:

=============================================================================*/
boolean GSM1xSMSMessageStore_MatchSMS
(
   SMSApp *pMe, 
   uint8 MR, 
   const GSMSMSAddress *pRA, 
   uint16 *pIndex, 
   GSMSMSStorageType store
)
{
    uint16 i;
    uint16 numEntries;
    GSMSMSRawMsg gsmRaw;
    GSMSMSMsg gsmMsg;
    
    if (IGSMSMS_GetStoreSize(pMe->m_pIGSMSMS, store, &numEntries) != AEE_GSMSMS_SUCCESS)
    {
       return FALSE;
    }

    for (i=0; i<numEntries; i++)
    {
        if (IGSMSMS_GetMessage(pMe->m_pIGSMSMS, i, &gsmRaw, store) != AEE_GSMSMS_SUCCESS)
        {
           continue;
        }

        if (gsmRaw.msg.sim.status == GSMSMS_STATUS_MO_SENT_ST_NOT_RECEIVED)
        {
           continue;
        }

        if (IGSMSMS_DecodeMessage(pMe->m_pIGSMSMS, &gsmRaw, &gsmMsg) != AEE_GSMSMS_SUCCESS)
        {
           DBGPRINTF("IGSMSMS_DecodeMessage failed");
           continue;
        }

        // Check the TP->MR and RA
        if (gsmMsg.msgType != GSMSMS_MSG_SMS_SUBMIT || 
            gsmMsg.msg.SMSSubmit.MR != MR || 
            AddressMatch(&gsmMsg.msg.SMSSubmit.DA, pRA) == FALSE)
        {
           continue;
        }

        // We have a match!
        *pIndex = i;
        return TRUE;
        
    }
    return FALSE;
}

static boolean AddressMatch(const GSMSMSAddress *pA, const GSMSMSAddress *pB)
{
   int i, len = STRLEN((char *)pA->addr);

   if (pA->npi != pB->npi || pA->ton != pB->ton)
   {
      return FALSE;
   }
   for (i=0; i< len; i++)
   {
      if (pA->addr[i] != pB->addr[i])
      {
         return FALSE;
      }
   }
   if ((int)STRLEN((char *)pB->addr) == len)
   {
      return TRUE;
   }
   return FALSE;
}

