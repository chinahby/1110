/*===========================================================================

                               S M S M T . c


GENERAL DESCRIPTION
   This file contains functions used to process Mobile Terminated messages.

EXTERNALIZED FUNCTIONS
   GSM1xSMSMT_ProcessMTMsg

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "GSM1xSMSApp.h"
#include "GSM1xSMSMT.h"
#include "GSM1xSMSUtil.h"
#include "AEEGSMSMS.h"
#include "gsm1xsms_res.h"


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#define IEI_SPECIAL_SMS_MESSAGE_INDICATION 0x01 // GSM 3.40, 9.2.3.24

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

/*===========================================================================

                    GLOBAL DATA

===========================================================================*/

/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/
static void GSM1xSMSMT_CreateTitle(SMSApp * pMe, SMSMessageStruct * pMsg,
                              boolean voicemail);

static boolean GSM1xHandleSMSDeliver
(
   SMSApp *pMe, 
   GSMSMSMsg *pGsm, 
   SMSMessageStruct *pInMsg
);

static int GSM1xStoreSMSDeliverMsg
(
   SMSApp *pMe, 
   GSMSMSMsg *pGsm, 
   boolean bIsVoicemail);

static int GSM1xParseVMMessage
(
   SMSApp *pMe, 
   GSMSMSMsg *pGsm, 
   uint8 *pCount, 
   boolean *pStore, 
   uint8 *pFCS
);

/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: GSM1xSMSMT_MTMsgHandler

DESCRIPTION:
   Takes the data from an incoming message and converts to GSM1xSMSAppMessageStruct

PROTOTYPE:
   void GSM1xSMSMT_MTMsgHandler(SMSApp *pMe,GSMSMSRawMsg *rawMsg) 
				

PARAMETERS:
   pMe [in]: Pointer to the SMS applet structure

   rawMsg [in]: Pointer to the incoming raw message
   
RETURN VALUE:
   Returns TRUE if successful. FALSE otherwise.

COMMENTS:
 
SIDE EFFECTS:
   None
=============================================================================*/
boolean GSM1xSMSMT_MTMsgHandler(SMSApp * pMe, const GSMSMSRawMsg *pRawMsg)
{
   GSMSMSMsg     gsm;
   byte          failureCause = 0xFF; // Unspecified Error
   GSMSMSMsg     drpt;
   SMSMessageStruct *pInMsg;

   pInMsg = &pMe->m_msIn;

   if (IGSMSMS_DecodeMessage(pMe->m_pIGSMSMS, pRawMsg, &gsm) != SUCCESS)
   {
      // See if this was a bad SMS_DELIVER
      if (gsm.msgType == GSMSMS_MSG_SMS_DELIVER)
      {
         // NAK with invalid parameter
         IGSMSMS_CreateDefaultMessage(pMe->m_pIGSMSMS, 
                                      GSMSMS_MSG_SMS_DELIVER_REPORT, 
                                      &drpt);

         drpt.msg.SMSDeliverReport.FCS_present = TRUE;
         drpt.msg.SMSDeliverReport.FCS = failureCause;
         IGSMSMS_SendSMSDeliverReport(pMe->m_pIGSMSMS, 
                                      &drpt.msg.SMSDeliverReport);

         DBGPRINTF("GSM1x_Test: SMS_DeliverReport Msg sent");
      }

      return FALSE;
   }

   // Keep the message incase we need it later
   MEMCPY(&pMe->m_decodedMsg, &gsm, sizeof(gsm));

   pInMsg->m_scAddr.addr[0] = '\0';

   switch (gsm.msgType)
   {
   case GSMSMS_MSG_SMS_DELIVER:
      {
         GSMSMSDeliverType  *pSMSDeliver = &gsm.msg.SMSDeliver;
         boolean rv;

         GSM1xSMSApp_DecodeAddress(&pSMSDeliver->OA, 
                              pInMsg->m_szFrom, 
                              sizeof(pInMsg->m_szFrom));

         IGSMSMS_DecodeUserData(pMe->m_pIGSMSMS, 
                                &gsm, pInMsg->m_szMessage, 
                                sizeof(pInMsg->m_szMessage));

         pInMsg->m_eEncoding = gsm.msg.SMSDeliver.DCS.msgCoding;
         pInMsg->m_dwTimeStamp = GSM1xSMSUtil_GSMTime2Secs(&gsm.msg.SMSDeliver.SCTS);
         pInMsg->m_eClass = pSMSDeliver->DCS.cg.generalData.msgClass;

         // Save SC address if RP set
         if (gsm.msg.SMSDeliver.RP == TRUE)
         {
            MEMCPY(&pInMsg->m_scAddr, 
                   &gsm.msg.SMSDeliver.SCAddr, 
                   sizeof(pInMsg->m_scAddr));
         }

         rv = GSM1xHandleSMSDeliver(pMe, &gsm, pInMsg);

         // Update annunciators if a new message arrived
         GSM1xSMSApp_UpdateMboxCounters(pMe);
         return rv;
      }
      break;

   case GSMSMS_MSG_SMS_STATUS_REPORT:
      {
         GSMSMSStatusReportType *pSMSStatusReport = &gsm.msg.SMSStatusReport;
         uint16 index;
         int rv;

         GSM1xSMSApp_DecodeAddress(&pSMSStatusReport->RA, 
                              pInMsg->m_szFrom, 
                              sizeof(pInMsg->m_szFrom));

         rv = IGSMSMS_DecodeUserData(pMe->m_pIGSMSMS, 
                                &gsm, 
                                pInMsg->m_szMessage, 
                                sizeof(pInMsg->m_szMessage));

         if (rv != AEE_GSMSMS_SUCCESS)
         {
            return FALSE;
         }

         DBGPRINTF("GSM1x_Test: SMS_StatusReport Msg Received");

         // If this is a reply to an SMS_Command message, ignore it
         if (pSMSStatusReport->SRQ == TRUE)
         {
            return FALSE;
         }

         pInMsg->m_dwTimeStamp = 
            GSM1xSMSUtil_GSMTime2Secs(&pSMSStatusReport->SCTS);

         // Look through SMS entries in NVRAM to see if there is a match
         if (GSM1xSMSMessageStore_MatchSMS(pMe, 
                                      pSMSStatusReport->MR, 
                                      &pSMSStatusReport->RA, 
                                      &index, 
                                      GSMSMS_STORE_NVRAM) != TRUE)
         {
            // We do not currently support Status report messages for the SIM
            // so no need to check SIM entries for a match
            return FALSE;
         }
         else
         {
            // Store the StatusReport
            if (IGSMSMS_StoreStatusReport(pMe->m_pIGSMSMS, index, &gsm, GSMSMS_STORE_NVRAM) != AEE_GSMSMS_SUCCESS)
            {
               DBGPRINTF("IGSMSMS_StoreStatusReport failed");
               return FALSE;
            }
         }

         // Now create a title for this message 
         GSM1xSMSMT_CreateTitle(pMe, pInMsg, FALSE);
         return TRUE;
      }
      break;

   default:
      break;
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: GSM1xSMSMT_CreateTitle

DESCRIPTION:
   Creates the title of the message. The title is the text shown on the menu
   item for a message. The appropriate title is placed in the title field

PROTOTYPE:
   static void GSM1xSMSMT_CreateTitle(SMSMessageStruct * pInMsg);

PARAMETERS:
   pInMsg [in/out]: Pointer to SMS message structure

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
static void GSM1xSMSMT_CreateTitle(SMSApp * pMe, SMSMessageStruct * pInMsg,
                              boolean voicemail)
{
    if (!pInMsg)
    {
        return;
    }

    //Handle Voice Mails
    if (voicemail)
    {
      AECHAR szBuf[MAX_TITLE_LEN];

        (void)ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                                   IDS_VOICEMAIL_TITLE_FMT,
                                   szBuf, sizeof(szBuf));
        WSPRINTF(pInMsg->m_szTitle, sizeof(pInMsg->m_szTitle),
                 szBuf, pInMsg->m_dwNumMsgs);
        return;
    }

    // Let's extract the title from the "From" field (if there is any) 
    // using the helper function.
    GSM1xSMSUtil_ExtractTitle(pMe,SMS_MB_INBOX,pInMsg);
}

/*=============================================================================
FUNCTION: GSM1xParseVMMessage

DESCRIPTION:
   Checks for Voicemail messages: UHDI, DCS and PID

PROTOTYPE:
    static int GSM1xParseVMMessage(SMSApp *pMe, GSMSMSMsg *pGsm, uint8 *pCount, boolean *pStore, uint8 *pFCS)

PARAMETERS:
   pInMsg [in/out]: Pointer to SMS message structure

RETURN VALUE:
   -1 if there was an error in the message
    0 if the message wasn't a voicemail message
    1 if the message was a voicemail message

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
static int GSM1xParseVMMessage
(
   SMSApp *pMe, 
   GSMSMSMsg *pGsm, 
   uint8 *pCount, 
   boolean *pStore, 
   uint8 *pFCS
)
{
   byte udhlen=0, len=0, idx=0;
   GSMSMSDeliverType *pSMSDeliver;
   byte *header;
   SMSMessageStruct *pInMsg;

   pSMSDeliver= &pGsm->msg.SMSDeliver;
   header = &pSMSDeliver->UD[0];
   pInMsg = &pMe->m_msIn;

   udhlen = header[idx]; // UDH length

   // Check for voicemail
   if (pSMSDeliver->UDHI)
   {
      boolean found = FALSE;

      while ((idx < udhlen) && (!found))
      {
         // Switch on IEI
         switch (header[++idx])
         {
         case IEI_SPECIAL_SMS_MESSAGE_INDICATION:

            len = header[++idx];

            // If Voicemail indication
            if ((header[idx+1] & 0x7F) == 0)
            {
               // Get the number of waiting messages
               *pCount = header[idx+2];
            
               // Always store the VM message, regardless of 
               // the indicaton as that is how we keep track
               // if (header[idx+1] & 0x80)
               // {
               //    *pStore = TRUE;
               // }
               *pStore = TRUE;
               return 1;
            }
            idx += len;
            break;

         default:
            // Skip the unknown IEI
            len = header[++idx];
            idx += len;
            break;
         }
      }
   }

   // Check the DCS
   if (pSMSDeliver->DCS.codingGroup == GSMSMS_DCS_CG_MESSAGE_WAITING)
   {
      // If message is Voicemail
      if (pSMSDeliver->DCS.cg.messageWaiting.indicationType == 
             GSMSMS_MSG_WAITING_VOICEMAIL)
      {
         if (pSMSDeliver->DCS.cg.messageWaiting.setIndication == TRUE)
         {
            *pCount = 1;
         }
         else
         {
            *pCount = 0;
         }

         // Always store the VM message, regardless of 
         // the indicaton as that is how we keep track
         *pStore = TRUE;

         return 1;
      }
      else 
      {
         *pFCS = 0x90;
         *pStore = FALSE;
         *pCount = 0;

         return (-1);
      }
   }
   // Otherwise check the PID
   else if (pSMSDeliver->PID == 0x5F) // Return Call Message (GSM 3.40, 9.2.3.9)
   {
      *pStore = TRUE;
      *pCount = 1;

      return 1;
   }
   return 0;
    
}

/*=============================================================================
FUNCTION: GSM1xHandleSMSDeliver

DESCRIPTION:
   Handles the three cases returned from GSM1xParseVMMessage

PROTOTYPE:
   static boolean GSM1xHandleSMSDeliver(SMSApp *pMe, 
                                 GSMSMSMsg *pGsm, 
                                 SMSMessageStruct *pInMsg)

PARAMETERS:
   pMe [in]: Pointer to the SMSApp structure.

RETURN VALUE:
   FALSE if message was handled
   TRUE if additional handling is required

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
static boolean GSM1xHandleSMSDeliver
(
   SMSApp *pMe, 
   GSMSMSMsg *pGsm, 
   SMSMessageStruct *pInMsg
)
{
   uint8     fcs=0;
   uint8     count=0;
   boolean   bStore=FALSE;
   GSMSMSMsg drpt;

   switch (GSM1xParseVMMessage(pMe, pGsm, &count, &bStore, &fcs))
   {
   case -1:
      // Message has an invalid format

      // NAK with unsupported DCS (we only support voicemail)
      IGSMSMS_CreateDefaultMessage(pMe->m_pIGSMSMS, 
                                   GSMSMS_MSG_SMS_DELIVER_REPORT, 
                                   &drpt);

      drpt.msg.SMSDeliverReport.FCS_present = TRUE;
      drpt.msg.SMSDeliverReport.FCS = fcs; 
      IGSMSMS_SendSMSDeliverReport(pMe->m_pIGSMSMS, 
                                   &drpt.msg.SMSDeliverReport);

      DBGPRINTF("GSM1x_Test: SMS_DeliverReport Msg sent");

      return FALSE;

   case 0:
      // Message is not a Voicemail message

      // See if the message should be stored
      if (GSM1xStoreSMSDeliverMsg(pMe, pGsm, FALSE) != AEE_GSMSMS_SUCCESS)
      {
         // Message was NAKed
         return FALSE;
      }

      DBGPRINTF("GSM1x_Test: SMS_Deliver Msg Received");

      // Send ACK
      IGSMSMS_CreateDefaultMessage(pMe->m_pIGSMSMS, 
                                   GSMSMS_MSG_SMS_DELIVER_REPORT,
                                   &drpt);
      IGSMSMS_SendSMSDeliverReport(pMe->m_pIGSMSMS, 
                                   &drpt.msg.SMSDeliverReport);

      DBGPRINTF("GSM1x_Test: SMS_DeliverReport Msg sent");

      if (pGsm->msg.SMSDeliver.DCS.cg.generalData.msgClass == GSMSMS_MSG_CLASS_2)
      {
         pMe->m_eMBoxType = SMS_MB_SIM;  
      }
      else
      {
         pMe->m_eMBoxType = SMS_MB_INBOX;  
      }

      return TRUE;
     
   case 1:
      // Message was a voicemail
      GSM1xSMSMessageStore_RestorePrefs(pMe);
      pMe->m_cfg.m_wNumVMMsgs = count;

      // Store change in case power goes off
      GSM1xSMSMessageStore_StorePrefs(pMe);

      if (bStore)
      {
         (void)GSM1xStoreSMSDeliverMsg(pMe, pGsm, TRUE);
      }

      DBGPRINTF("GSM1x_Test: SMS_Deliver (VM) Msg Received");

      // Send ACK
      IGSMSMS_CreateDefaultMessage(pMe->m_pIGSMSMS, 
                                   GSMSMS_MSG_SMS_DELIVER_REPORT,
                                   &drpt);
      IGSMSMS_SendSMSDeliverReport(pMe->m_pIGSMSMS, 
                                  &drpt.msg.SMSDeliverReport);

      DBGPRINTF("GSM1x_Test: SMS_DeliverReport Msg sent");

      pMe->m_eMBoxType = SMS_MB_VOICEMAIL;  

      // Now create a title for this message 
      GSM1xSMSMT_CreateTitle(pMe, pInMsg, FALSE);
      return TRUE;

   default:
      // We should never get here
      ASSERT_NOT_REACHABLE;
      break;
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: GSM1xStoreSMSDeliverMsg

DESCRIPTION:
   Implements the GSM required storing of messages

PROTOTYPE:
   static int GSM1xStoreSMSDeliverMsg(SMSApp *pMe, 
                                 GSMSMSMsg *pGsm, 
                                 boolean bIsVoicemail)

PARAMETERS:
   pMe [in]: Pointer to the SMSApp structure.
   pGsm [in]: Pointer to the decoded message to handle
   bIsVoicemail [in]: Indicates whether the message is a voicemail message

RETURN VALUE:
   FALSE if message was handled
   TRUE if additional handling is required

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
static int GSM1xStoreSMSDeliverMsg(SMSApp *pMe, GSMSMSMsg *pGsm, boolean bIsVoicemail)
{
   int storeErr = AEE_GSMSMS_SUCCESS;
   uint16 index;

   GSMSMSDeliverType *pSMSDeliver = &pGsm->msg.SMSDeliver;

   if (bIsVoicemail)
   {
      uint16 index = 0;

      // Only the most recent voicemail message is stored
      storeErr = IGSMSMS_StoreMessage(pMe->m_pIGSMSMS, pGsm,
                                         GSMSMS_STORE_NVRAM_VM, &index);
      pMe->m_bNeedVMRefresh = TRUE;
   }
   else if (pSMSDeliver->DCS.codingGroup == GSMSMS_DCS_CG_GENERAL_DATA)
   {
      // Store to SIM if Class 2
      index = 0xFFFF;
      if (pSMSDeliver->DCS.cg.generalData.msgClass == GSMSMS_MSG_CLASS_2)
      {
         if (pMe->m_bSSTDisabled)
         {
            storeErr = AEE_GSMSMS_EFAILED;
         }
         else
         {
            storeErr = IGSMSMS_StoreMessage(pMe->m_pIGSMSMS, pGsm, 
                                         GSMSMS_STORE_SIM, &index);

            pMe->m_wCurrMsgID = index;
            pMe->m_bNeedSIMRefresh = TRUE;
         }
      }
      // Store to NVRAM if class class 1
      else if (pSMSDeliver->DCS.cg.generalData.msgClass == GSMSMS_MSG_CLASS_1)
      {
         index = 0xFFFF;
         storeErr = IGSMSMS_StoreMessage(pMe->m_pIGSMSMS, pGsm, 
                                         GSMSMS_STORE_NVRAM, &index);

         pMe->m_wCurrMsgID = index;
         pMe->m_bNeedNVRAMRefresh = TRUE;
      }
   }

   if (bIsVoicemail == FALSE) 
   { 
      if (storeErr != AEE_GSMSMS_SUCCESS)
      {
         byte error = 0xD2; // Error in MS
         GSMSMSMsg     drpt;

         if (pSMSDeliver->DCS.cg.generalData.msgClass == GSMSMS_MSG_CLASS_2)
         {
            uint16 i;
            int rv;
            GSMSMSStatusType status;

            if (pMe->m_bSSTDisabled)
            {
               error = 0xD1; // No storage capability in SIM
            }
            else if (storeErr == AEE_GSMSMS_ESTORE_FULL)
            {

                DBGPRINTF("GSM1x_Test:SM saved on SIM failed Cause SIM Full");

               // See if NV is full
               error = 0xD0; // USIM Storage full

               for (i=0; i< pMe->m_wMaxNVMsgs; i++)
               {
                  rv = IGSMSMS_GetMessageStatus(pMe->m_pIGSMSMS, i, &status, GSMSMS_STORE_NVRAM);
                  if (rv == AEE_GSMSMS_SUCCESS)
                  {
                     if (status == GSMSMS_STATUS_NONE)
                     {
                        break;
                     }
                  }
               }
               if (i >= pMe->m_wMaxNVMsgs)
               {
                  error = 0xD3; // Memory Capacity Exceeded
               }
            }
         }
         else if (storeErr == AEE_GSMSMS_ESTORE_FULL)
         {
            uint16 i;
            int rv;
            GSMSMSStatusType status;

            DBGPRINTF("GSM1x_Test:SM saved on NVRAM failed Cause NVRAM Full");

            // See if SIM is full
            error = 0xD2; // Error in MS

            for (i=0; i< pMe->m_wMaxSIMMsgs; i++)
            {
               rv = IGSMSMS_GetMessageStatus(pMe->m_pIGSMSMS, i, &status, GSMSMS_STORE_SIM);
               if (rv == AEE_GSMSMS_SUCCESS)
               {
                  if (status == GSMSMS_STATUS_NONE)
                  {
                    break;
                  }
               }
            }
            if (i >= pMe->m_wMaxSIMMsgs)
            {
               error = 0xD3; // Memory Capacity Exceeded
            }
         }
         else if (storeErr == AEE_GSMSMS_ESST)
         {
            error = 0x91; // Message class not supported
         }

         // NAK with invalid parameter
         if (IGSMSMS_CreateDefaultMessage(pMe->m_pIGSMSMS, 
                                             GSMSMS_MSG_SMS_DELIVER_REPORT, 
                                             &drpt) == AEE_GSMSMS_SUCCESS)
         {
            drpt.msg.SMSDeliverReport.FCS_present = TRUE;
            drpt.msg.SMSDeliverReport.FCS = error; 

            if (error == 0xD3)
            {
               (void)IGSMSMS_SetMemoryCapExceededFlag(pMe->m_pIGSMSMS, 0xFE);
            }

            IGSMSMS_SendSMSDeliverReport(pMe->m_pIGSMSMS, 
                                         &drpt.msg.SMSDeliverReport);

            DBGPRINTF("GSM1x_Test: SMS_DeliverReport Msg sent");
         }
      }
      else
      {
         if (pSMSDeliver->DCS.cg.generalData.msgClass == GSMSMS_MSG_CLASS_2)
         {
            DBGPRINTF("GSM1x_Test:SM saved on SIM ok");
         }
         else
         {
            DBGPRINTF("GSM1x_Test:SM saved on NVRAM ok");
         }
      }
   }
   return storeErr;
}
