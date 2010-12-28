/*===========================================================================
FILE: OEMGSM1xSig_SVC.c

SERVICES: GSM1x Signaling interface.

GENERAL DESCRIPTION
   This file contains functions used by GSM1xMessage to access the SIG cache

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS
   BREW must be initialized.

        Copyright © 2003-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Include DMSS first*/
#include "uim.h"

/* Then include OEM headers.*/
#include "OEMFeatures.h"

/* The whole file depends on the following include.*/                         
#if defined(FEATURE_GSM1x)

#include "OEMGSM1xSIG_SVC.h"

/* Then include AEE headers.*/
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "AEE_OEMDispatch.h"
#include "AEEGSM1xSig.h"

/*===========================================================================

                      CONSTANTS and ENUMs

===========================================================================*/

#define FARF_ERROR    0
#define FARF_HIGH     0
#define FARF_DEBUG    0

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */


#define FARF(x, p) /*lint -save -e506 -e774*/ \
                   if (1 == FARF_##x) DBGPRINTF p \
                   /*lint -restore*/

//#define ASSERT_FARF(x) if (!(x)) { FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); }

// This should only be used for function parameters, not for local variables
#define PARAM_NOT_REF(param)         /*lint -esym(715,param) */


typedef enum {

    CES_FREE,
    CES_READY

} SigNotifyCacheStatusEnum;


#define MAX_SIG_NOTIFY_CACHE_ENTRIES        3

// Protocol revisions maybe used in future versions. For the moment we 
// assume that all messages will be exchanged with protocol revision 0.
//
#define GSM1xSIG_CURRENT_PROTOCOL_REVISION  0
#define GSM1xSIG_MIN_PROTOCOL_REVISION      0


// For protocol type CONFIG, this message type indicates reject
//
#define GSM1xSIG_CONFIG_REJECT 0x01         
                                            

// Protocol discriminator for authentication
//
#define GSM1xSIG_MOB_MGMT_NON_GPRS_DISCRIMINATOR  5    
                                                       

// Message types used for RIL3 type MOB_MGMT_NON_GPRS_DISCRIMINATOR
//
#define GSM1xSIG_AUTH_REQUEST    0x12
#define GSM1xSIG_AUTH_RESPONSE   0x14


/*===========================================================================

                      EXTERNAL FUNCTIONS

===========================================================================*/
// Need this routine from OEMGSM1xControl.c file
//
extern AEEGSM1xSig_Status OEMGSM1xControl_SVC_GetStatus(void);

extern void OEMGSM1xSig_ProcessSigCacheNotification(void);

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/

typedef struct
{
    SigNotifyCacheStatusEnum          wStatus;
    AEEGSM1xSig_NotifyMessageType     userData;

} GSM1xSigNotifyCacheEntry;

/*===========================================================================

                      PUBLIC METHOD DECLARATIONS

===========================================================================*/

/*===========================================================================

                      Protected Functions

These functions can be called from static BREW classes and DMSS/OEM code.
===========================================================================*/

// The following function is called from UIM task to report when the GSM
// algorithm completes. This is submitted as callback function in uim_cmd
void OEMGSM1xSig_SVC_HandleRUIMCallback( uim_rpt_type *uim_rpt );


/*===========================================================================
                       
                      Private Functions

===========================================================================*/

static void OEMGSM1xSig_SVC_processUIMRsp(void);

static void OEMGSM1xSig_SVC_addToSigNotifyCache(const AEEGSM1xSig_NotifyMessageType* userData);

static int OEMGSM1xSig_SVC_processConfigMessage(const uint8 *bearerData, int length);

static int OEMGSM1xSig_SVC_processRIL3(AEEGSM1xSig_SignalingMessageType *signalMessage);
static int OEMGSM1xSig_SVC_processExtRIL3(AEEGSM1xSig_SignalingMessageType *signalMessage);

static boolean OEMGSM1xSig_SVC_submitAuthRequest(uint8 *rand);


/*===========================================================================

                     LOCAL/STATIC DATA
===========================================================================*/

static boolean                   gbInitialized;

// When a new signaling message comes in, it is placed in m_SigNotifyCache
// and OEMGSM1xSig_ProcessSigCacheNotification is called.
//
static GSM1xSigNotifyCacheEntry  gSigNotifyCache[MAX_SIG_NOTIFY_CACHE_ENTRIES];

// New Callback to handle UIM respnose
static AEECallback               gUIMRspCB;

static boolean GSM1xAuthProcessingRandFlag; // Ensures that there is only one
                                            // AuthRequest being processed by UIM

static boolean GSM1xAuthRIL3ExtFlag;        // true if current AuthRequest being 
                                            // processed came as an extended RIL3 
                                            // message.

static boolean GSM1xAuthRefNumberPresent;   // true if we received an AuthRefNumber 
                                            // in AuthRequest being processed.

static uint32 GSM1xAuthRefNumber;           // Stores AuthRefNumber for AuthRequest 
                                            // currently being processed. Valid 
                                            // only if GSM1xAuthRefNumberPresent is true

static uint8 GSM1xAuthRand[16];             // Stores RAND currently being processed

static uim_rpt_type GSM1xUIMRpt;


/*===========================================================================

                     BREW Methods

===========================================================================*/

/*=============================================================================
FUNCTION: OEMGSM1xSig_SVC_Init                            EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to initialize IGSM1xSigNotifier. This method will 
   initialize the singleton instance of IGSM1xSigNotifier by calling the
   OEMGSM1xSig_SVCNotifier_New method.

PARAMETERS:
   None.
   
RETURN VALUE:
   None.
   
COMMENTS:
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
void OEMGSM1xSig_SVC_Init(void)
{
   uint16 i;

   // Initializing UIMCallBack
   CALLBACK_Init(&gUIMRspCB, 
                 (PFNNOTIFY)OEMGSM1xSig_SVC_processUIMRsp,
                 NULL);

   for (i = 0; i < MAX_SIG_NOTIFY_CACHE_ENTRIES; i++)
   {
      gSigNotifyCache[i].wStatus = CES_FREE;
   }

   gbInitialized = TRUE;
}


/*=============================================================================
FUNCTION: OEMGSM1xSig_SVC_GetSIGMessage 

DESCRIPTION:
     Get a SIG message from the cache.
        
PARAMETERS:
  pMsg : pointer to a buffer that will receive the extracted message 

RETURN VALUE:
  TRUE if a message was extracted, FALSE if there are no more SIG message

SIDE EFFECTS:
    None
=============================================================================*/
boolean OEMGSM1xSig_SVC_GetSIGMessage(AEEGSM1xSig_NotifyMessageType* pMsg)
{
   uint16                     i;
   GSM1xSigNotifyCacheEntry * pcse;

   if ( (!gbInitialized) || (pMsg == NULL))
   {
      return FALSE;
   }

   for (pcse = gSigNotifyCache, i = 0; i < MAX_SIG_NOTIFY_CACHE_ENTRIES; i++, pcse++)
   {
      if (pcse->wStatus == CES_READY) 
      {
         break;
      }
   }

   if (i < MAX_SIG_NOTIFY_CACHE_ENTRIES)
   {
       COPY_TO_USER((void *)pMsg, &(pcse->userData), sizeof(pcse->userData));

       pcse->wStatus = CES_FREE;
       return TRUE;
   }
   else
   {
       return FALSE;
   }
}


/*=============================================================================
FUNCTION: OEMGSM1xSig_SVC_addToSigNotifyCache                 INTERNAL FUNCTION

DESCRIPTION:
This function puts the request into the SigNotifyCache and switches context to brew. 
In brew context OEMGSM1x_processSigNotifyCache will be invoked. 

PARAMETERS:
    userData [in] : Signaling message to be delivered

RETURN VALUE:
  None.

SIDE EFFECTS:
  Context switched to Brew context.
=============================================================================*/
static void OEMGSM1xSig_SVC_addToSigNotifyCache(
    const AEEGSM1xSig_NotifyMessageType* userData
    )
{
   uint16                     i;
   GSM1xSigNotifyCacheEntry * pcse;

   for (pcse = gSigNotifyCache, i = 0; i < MAX_SIG_NOTIFY_CACHE_ENTRIES; i++, pcse++)
   {
      if (pcse->wStatus == CES_FREE) 
      {
         break;
      }
   }

   if (i >= MAX_SIG_NOTIFY_CACHE_ENTRIES) 
   {
      FARF(HIGH,("Sig: No Cache available"));
      return;
   }

   MEMSET(pcse, 0, sizeof(GSM1xSigNotifyCacheEntry));
   MEMCPY(&(pcse->userData), userData, sizeof(AEEGSM1xSig_NotifyMessageType));
   pcse->wStatus = CES_READY;

   OEMGSM1xSig_ProcessSigCacheNotification();
}


/*=============================================================================
FUNCTION: OEMGSM1xSig_SVC_SendSignalingSMS                     

DESCRIPTION:
     Sends a IS-637 message with a tunnelled GSM1x message. Formats the IS-637 
     client message structure (including bearer data) and bit-packs the user
     data.
        
PARAMETERS:
    * pSigMsg   [in] : signaling message to be send as SMS

RETURN VALUE:
  EGSM1x_INACTIVE - Phone is not in IGSM1x mode
  EBADCLASS       - IPhone not initialized.
  EITEMBUSY       - No more buffers left for oustanding MO SMS.
  SUCCESS         - MO SMS was queued up.

SIDE EFFECTS:
    None
=============================================================================*/
int OEMGSM1xSig_SVC_SendSignalingSMS (const AEEGSM1xSig_SignalingMessageType* pSigMsg)
{
   byte payload[GSM1xSIG_SIGNALING_MAX+1];
   byte protocolRevision;
   byte protocolType;
   byte protocolDataLen;

   // We don't send out any signaling messages while GSM1x is inactive.
   //
   if (OEMGSM1xControl_SVC_GetStatus() == GSM1xSIG_INACTIVE)
   {
      return EGSM1x_INACTIVE;
   }

   COPY_FROM_USER(&protocolRevision, &pSigMsg->ProtocolRevision, sizeof(protocolRevision));
   COPY_FROM_USER(&protocolType, &pSigMsg->ProtocolType, sizeof(protocolType));
   COPY_FROM_USER(&protocolDataLen, &pSigMsg->ProtocolDataLen, sizeof(protocolDataLen));

   payload[0] = protocolType | ( protocolRevision << 4); /*lint !e734 */
   COPY_FROM_USER(&(payload[1]), pSigMsg->ProtocolData, protocolDataLen);

   FARF(DEBUG,("GSM1x: Submitting signaling message"));

   return (OEMGSM1xMessage_SendSIGMessage( protocolDataLen+1,
                                           payload,
                                           FALSE));         // StartTransaction flag
}


/*=============================================================================
FUNCTION: OEMGSM1xSig_SVC_ProcessMessage                         EXTERNAL FUNCTION

DESCRIPTION:
     Process an incoming GSM1x message. This is the entry point from IPhone. (WMS task)
        
PARAMETERS:

RETURN VALUE:
  none

SIDE EFFECTS:
  none 
=============================================================================*/
void OEMGSM1xSig_SVC_ProcessMessage (const byte *bearerData, const uint16 length)
{
   byte protocolByte;
   byte protocolType, protocolRev;
   AEEGSM1xSig_NotifyMessageType notifyData;

   if ((! gbInitialized) ||
       (OEMGSM1xControl_SVC_GetStatus() == GSM1xSIG_INACTIVE))
   {
      return;
   }

   if (length < 1)  
   {
      FARF(ERROR,("No protocol byte in the payload."));
      return;
   }

   protocolByte = bearerData[0];
   protocolType = protocolByte & 0x0f;
   protocolRev  = protocolByte >> 4;

   FARF(DEBUG,("GSM1x:Handling sig proto(%d)", protocolType));
   if (protocolType == GSM1xSIG_PTYPE_CONFIG) 
   {
      // If Config message contains reject message, they 
      // have to be handled differently
      if (OEMGSM1xSig_SVC_processConfigMessage(bearerData,length) == SUCCESS)
      {
         return;
      }
   }

   // We are not dealing with a reject message.
   //
   notifyData.messageType = GSM1xSIG_SIGNALING;          
   notifyData.msg.signalMessage.ProtocolType     = protocolType;
   notifyData.msg.signalMessage.ProtocolRevision = protocolRev;
   notifyData.msg.signalMessage.ProtocolDataLen  = length - 1;  /*lint !e734 */

   MEMCPY(notifyData.msg.signalMessage.ProtocolData,
          &(bearerData[1]),
          length - 1);                                          /*lint !e732 */

   // Filter out any authentication messages.
   //
   if (protocolType == GSM1xSIG_PTYPE_RIL3) 
   {
      if (OEMGSM1xSig_SVC_processRIL3(
             &(notifyData.msg.signalMessage)) == SUCCESS)
      {
         return; 
      }
   }
   else if (protocolType == GSM1xSIG_PTYPE_EXT_RIL3) 
   {
      if (OEMGSM1xSig_SVC_processExtRIL3(
             &(notifyData.msg.signalMessage)) == SUCCESS)
      {
         return; 
      }
   }

   FARF(DEBUG,("GSM1x: Queuing up a signaling message"));
   // If we reached here, we are dealing with a signaling message
   // that does not have an authentication payload. Queue up the 
   // message to be send to registered clients.
   //
   OEMGSM1xSig_SVC_addToSigNotifyCache(&notifyData);
}


/*=============================================================================
FUNCTION: OEMGSM1xSig_SVC_processConfigMessage                INTERNAL FUNCTION

DESCRIPTION:
   This is a helper function called from ProcessMessage to handle reject messages.
   If the incomding SMS contains a reject message, this function will process 
   the message and return SUCCESS. The function will return SUCCESS even if 
   it detects an error; SUCCESS indicates that this function has done all 
   the processing needed for the message.

Parameters:
   bearerData   : Bearer data payload from an IS-637 message
   length       : Length of the bearerData
   
Return Value:
   EFAILED    - Message not processed.
   SUCCESS    - Message processed
   
Comments:
   None.
   
Side Effects:
   None.
   
See Also:
=============================================================================*/
static int OEMGSM1xSig_SVC_processConfigMessage (const uint8 *bearerData, int length)
{
   AEEGSM1xSig_NotifyMessageType notifyData;

   // bearerData[0]  = Protocol byte

   uint8 msgType            = bearerData[1];
   uint8 pRevByte           = bearerData[2];         // not used currently
   uint8 rejectedProtocol   = (bearerData[3]) >> 4;      
   uint8 rejectLength       = bearerData[4];      
   const uint8 *rejectCause = &(bearerData[5]);

   if (length < 2)  
   {
      FARF(ERROR,("No message type specified in config message."));
      return SUCCESS;
   }

   if (msgType != GSM1xSIG_CONFIG_REJECT) 
   {
      // This is not a reject message. Let's pass it onto the BREW applet
      return EFAILED;
   }

   // This is a reject message so we are either going to consume it or 
   // forward it to BREW. In either case we will be returning SUCCESS

   if (length < 5)
   {
      FARF(ERROR,("Reject message too short: payload less than 5 bytes."));
      return SUCCESS;
   }

   if (rejectedProtocol == 0)
   {
       // We have the wrong PREV. No behaviour defined.
       //
       FARF(ERROR,("PREV rejected by n/w. protocol revision byte= %d", pRevByte));
       return SUCCESS;
   }

   if ((rejectedProtocol == GSM1xSIG_PTYPE_RIL3) ||
       (rejectedProtocol == GSM1xSIG_PTYPE_EXT_RIL3))
   {
      uint8 rejectByte = bearerData[5];
      uint8 rejectDisc = rejectByte | 0x0f;         // rejected discriminator 
      uint8 rejectType = rejectByte >> 4;

      if (rejectLength == 0) 
      {
         // Network doesn't support this protocol type. No behaviour defined.
         //
         FARF(ERROR,("N/w doesn't support protocol = %d", rejectedProtocol));
         return SUCCESS;
      }

      if (rejectType == GSM1xSIG_RIL3_REJECT_FORMAT) 
      {
         FARF(ERROR,("N/w dosn't support protocol %d .",rejectType));
         return SUCCESS;
      }
      
      if (rejectType == GSM1xSIG_RIL3_REJECT_DISCRIMINATOR) 
      {
         if (rejectDisc == GSM1xSIG_MOB_MGMT_NON_GPRS_DISCRIMINATOR) 
         {
            FARF(ERROR,("N/w dosn't support discriminator %d .",rejectDisc));
            return SUCCESS;
         }
      }

      if (rejectType == GSM1xSIG_RIL3_REJECT_MSG_TYPE) 
      {
         uint8 rejectedMsgType = bearerData[6];
         if (rejectLength != 2) 
         {
            FARF(ERROR,("Unexpected reject cause length %d. Expecting 2.",rejectLength));
            return SUCCESS;
         }

         if (rejectedMsgType == GSM1xSIG_AUTH_RESPONSE) 
         {
            FARF(ERROR,("N/w rejected our authentication response."));
            return SUCCESS;
         }
      }
   }

   // If we are here, GSM1xSig doesn't have to handle this reject message.
   // We will pass on the reject message to any BREW app that's interested.
   //

   notifyData.messageType = GSM1xSIG_REJECT;
                                                     
   notifyData.msg.rejectMessage.ProtocolTypeRejected = rejectedProtocol;
   notifyData.msg.rejectMessage.RejectCauseLength    = rejectLength;
   MEMCPY(notifyData.msg.rejectMessage.RejectCause,
          rejectCause, 
          rejectLength);

   FARF(DEBUG,("GSM1x: Queuing up a reject message"));

   OEMGSM1xSig_SVC_addToSigNotifyCache(&notifyData);
   return SUCCESS;
}


/*===========================================================================
FUNCTION OEMGSM1xSig_SVC_submitAuthRequest                  INTERNAL FUNCTION

DESCRIPTION
  Submits an UIM command to compute Kc and SRES using GSM algorithm.
  This helper function is called from OEMGSM1xSig_SVC_processRIL3  or
  OEMGSM1xSig_SVC_processExtRIL3.

PARAMETERS:
   rand: Pointer to the 16 byte rand value.

RETURN VALUE
   TRUE   -  UIM command to compute Kc submitted successfully.
   FALSE  -  unable to submit UIM command.

SIDE EFFECTS
  None

===========================================================================*/
static boolean OEMGSM1xSig_SVC_submitAuthRequest(uint8 *randChallenge)
{
   uim_cmd_type *uim_cmd_ptr;

   FARF(HIGH,("Processing GSM1x auth request"));

   if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
   {
      uim_cmd_ptr->run_gsm.hdr.cmd_hdr.task_ptr = NULL;
      uim_cmd_ptr->run_gsm.hdr.cmd_hdr.sigs = 0;
      uim_cmd_ptr->run_gsm.hdr.cmd_hdr.done_q_ptr = &uim_free_q;

      uim_cmd_ptr->run_gsm.hdr.command = UIM_RUN_GSM_ALGO_F;
      uim_cmd_ptr->run_gsm.hdr.options = UIM_OPTION_ALWAYS_RPT;
      uim_cmd_ptr->run_gsm.hdr.protocol = UIM_GSM;
      uim_cmd_ptr->run_gsm.hdr.slot = UIM_SLOT_1;
      uim_cmd_ptr->run_gsm.hdr.rpt_function = OEMGSM1xSig_SVC_HandleRUIMCallback;
      MEMCPY(uim_cmd_ptr->run_gsm.rand,randChallenge,16); 

      FARF(DEBUG,("GSM1x: Submitted auth request"));
      // Send the command to the UIM server. 
      uim_cmd ( uim_cmd_ptr );
      return TRUE;
   }
   else // No UIM command buffers on the free queue. 
   {
      return FALSE;
   }
}


/*===========================================================================
FUNCTION OEMGSM1xSig_SVC_processRIL3                        INTERNAL FUNCTION

DESCRIPTION
  This function is invoked whenever there is an incoming GSM1x signaling 
  message with RIL3 protocol. 
  If the message contains an authenticaition request, it is handled here.

DEPENDENCIES
  None

PARAMETERS:
   signalMessage :  pointer to the signaling message received

RETURN VALUE
  TRUE  -  Signaling message was handled/consumed. 
  FALSE -  Signaling message not handled. It should be passed to registered
           BREW application.

SIDE EFFECTS
  Sets GSM1xAuthRefNumber and GSM1xAuthRand[] global variables

===========================================================================*/
static int OEMGSM1xSig_SVC_processRIL3(AEEGSM1xSig_SignalingMessageType *signalMessage)
{
   uint8 *bearerData = signalMessage->ProtocolData;
   //uint8 keySequenceNumber;
   uint8 ril3Header  = bearerData[0];     
   uint8 messageType = bearerData[1];
   uint8 protocolDiscriminator = ril3Header & 0x0f;

   if (protocolDiscriminator != GSM1xSIG_MOB_MGMT_NON_GPRS_DISCRIMINATOR) 
   {
      return EFAILED;
   }

   if (messageType == GSM1xSIG_AUTH_REQUEST) 
   {
      if (GSM1xAuthProcessingRandFlag) 
      {
          // If we have multiple RAND number being processed in UIM queue
          // we won't be able to correlate the returned SRES and Kc with the
          // appropriate RAND and reference number. So we allow only one 
          // request to be submitted; any messages received while a request 
          // is being processed is ignored.
          return SUCCESS;
      }
      GSM1xAuthProcessingRandFlag = TRUE;
      GSM1xAuthRIL3ExtFlag = FALSE; 
      //keySequenceNumber = bearerData[2] | 0x0f;    // GSM specific field. Not used for signaling
      MEMCPY(GSM1xAuthRand,&(bearerData[3]),sizeof(GSM1xAuthRand));

      if (!OEMGSM1xSig_SVC_submitAuthRequest(GSM1xAuthRand))
      {
         FARF(ERROR,( "No free buffers on auth_free_q"));
         GSM1xAuthProcessingRandFlag=FALSE; 
      }
      return SUCCESS;
   }

   // If we reached here, message was not handled. Let's pass it to BREW apps
   return EFAILED;
}


/*===========================================================================
FUNCTION OEMGSM1xSig_SVC_processExtRIL3                     INTERNAL FUNCTION

DESCRIPTION
  This function is invoked whenever there is an incoming GSM1x signaling 
  message with extendedRIL3 protocol. 
  If the message contains an authenticaition request, it is handled here.

DEPENDENCIES
  None

PARAMETERS:
   signalMessage :  pointer to the signaling message received

RETURN VALUE
  TRUE  -  Signaling message was handled/consumed. 
  FALSE -  Signaling message not handled. It should be passed to registered
           BREW application.

SIDE EFFECTS
  Sets GSM1xAuthRefNumber and GSM1xAuthRand[] global variables

===========================================================================*/
static int OEMGSM1xSig_SVC_processExtRIL3(AEEGSM1xSig_SignalingMessageType *signalMessage)
{
   uint32 tempAuthRefNumber;          
   boolean tempAuthRefNumberPresent=FALSE;

   uint8* bearerData = signalMessage->ProtocolData;
   uint8 messageType;
   uint8 *ril3Payload = NULL;
   uint8 cursor;

   FARF(HIGH,("Processing GSM1x RIL3 extended format message"));
   
   // Parse the TLV (Type,Length,Value) triplets
   cursor=0;
   while (cursor < signalMessage->ProtocolDataLen) 
   {
      uint8 type;
      uint8 len;
      type = bearerData[cursor];
      len  = bearerData[cursor+1];
      switch (type) {
         case GSM1xSIG_RIL3_EXT_GSM_TYPE:
            {
               uint8 ril3Header  = bearerData[cursor+2];     
               uint8 protocolDiscriminator = ril3Header & 0x0f;

               if (protocolDiscriminator != GSM1xSIG_MOB_MGMT_NON_GPRS_DISCRIMINATOR) 
               {
                  return EFAILED;
               }
               ril3Payload = &(bearerData[cursor+2]);
               break;
            }

         case GSM1xSIG_RIL3_EXT_REF_NUM_TYPE:
            // ASSERT(len==4);
            tempAuthRefNumberPresent=TRUE;
            MEMCPY(&tempAuthRefNumber,&(bearerData[cursor+2]),sizeof(tempAuthRefNumber)); 
            break;

         default:
            // Currently there are only 2 possibilities listed above. In future
            // more may be added. We ignore them since they don't affect 
            // authentciation
            break;
      };
      cursor = cursor + 2 + len;
   }

   if (ril3Payload == NULL)
   {
      FARF(ERROR,("Not able to find a GSM_DATA type in extended RIL3 message"));
      return SUCCESS;               // We will discard the message
   }


   // Handle the RIL3 payload now
   // 
   messageType = ril3Payload[1];
   if (messageType == GSM1xSIG_AUTH_REQUEST) 
   {
      //uint8 keySequenceNumber;

      if (GSM1xAuthProcessingRandFlag) 
      {
          // If we have multiple RAND number being processed in UIM queue
          // we won't be able to correlate the returned SRES and Kc with the
          // appropriate RAND and reference number. So we allow only one 
          // request to be submitted; any messages received while a request 
          // is being processed is ignored.

          // We want the request to be ignored, so we return SUCCESS
          return SUCCESS;
      }
      GSM1xAuthProcessingRandFlag = TRUE;
      GSM1xAuthRefNumber = tempAuthRefNumber;   /*lint !e644 */
                                                // GSM1xAuthRefNumber is only used if 
                                                // GSM1xAuthRefNumberPresent is true.  
      GSM1xAuthRefNumberPresent = tempAuthRefNumberPresent;


      GSM1xAuthRIL3ExtFlag = TRUE; 
      //keySequenceNumber = bearerData[2] | 0x0f;  // GSM specific field.  Not used for signaling

      MEMCPY(GSM1xAuthRand,&(ril3Payload[3]),sizeof(GSM1xAuthRand));

      if (!OEMGSM1xSig_SVC_submitAuthRequest(GSM1xAuthRand))
      {
         FARF(ERROR,( "No free buffers on auth_free_q"));
         GSM1xAuthProcessingRandFlag = FALSE;
      }
      return SUCCESS;
   }

   return EFAILED;
}


/*===========================================================================
FUNCTION OEMGSM1xSig_SVC_HandleRUIMCallback                 EXTERNAL FUNCTION

DESCRIPTION
   During handling of authentication request, a UIM command is submitted. When 
   the UIM command completes this callback function is invoked.

DEPENDENCIES
  None

PARAMETERS:
   uim_rpt :  pointer to the uim report

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void OEMGSM1xSig_SVC_HandleRUIMCallback(uim_rpt_type *uim_rpt)
{
   FARF(DEBUG,("Handling the RUIM callback"));

   if (uim_rpt == NULL)
   {
       FARF(ERROR,("Bad uim_rpt pointer!"));
       return;
   }

   if (uim_rpt->rpt_type != UIM_RUN_GSM_ALGO_R) 
      return;

   MEMCPY(&GSM1xUIMRpt, uim_rpt, sizeof(*uim_rpt));

   AEE_SYS_RESUME(&gUIMRspCB);
}


/*===========================================================================
FUNCTION OEMGSM1xSig_SVC_processUIMRsp                    INTERNAL FUNCTION

DESCRIPTION
   process the Auth UIM response.

DEPENDENCIES
  None

PARAMETERS:
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void OEMGSM1xSig_SVC_processUIMRsp(void)
{
   uim_rpt_type *uim_rpt;

   FARF(HIGH,("Continuing the RUIM callback in BREW Context"));

   uim_rpt = &GSM1xUIMRpt;

   if ((! gbInitialized) ||
       (OEMGSM1xControl_SVC_GetStatus() == GSM1xSIG_INACTIVE))
   {
      FARF(DEBUG,("GSM1x: Mode become inactive ??"));
      GSM1xAuthProcessingRandFlag=FALSE; 
      return;
   }

   if (uim_rpt->rpt_status != UIM_PASS)
   {
      FARF(ERROR,("UIM_RUN_GSM_ALGO_R failed! sw1:%x - sw2:%x", 
                uim_rpt->sw1, uim_rpt->sw2));
   }
   else                                // run gsm algorithm was successful
   {
      AEEGSM1xSig_SignalingMessageType tmpMsg;
      uint8 *buffer = tmpMsg.ProtocolData;
      uint8 *sres;

      if (uim_rpt->rpt.run_gsm.data_length != 12)
      {
         FARF(ERROR,("UIM_RUN_GSM_ALGO_R failed! Response too short (%d)",
                   uim_rpt->rpt.run_gsm.data_length));
      }
      sres = uim_rpt->rpt.run_gsm.data;

      tmpMsg.ProtocolRevision = GSM1xSIG_CURRENT_PROTOCOL_REVISION;

      if (GSM1xAuthRIL3ExtFlag)                // Send an extended  RIL3 message
      {  
         byte msgLength;
   
         tmpMsg.ProtocolType = GSM1xSIG_PTYPE_EXT_RIL3;

         // Compose the RIL3 payload TLV
         buffer[0] = GSM1xSIG_RIL3_EXT_GSM_TYPE;  // Type 
         buffer[1] = 6;                           // Length
   
         buffer[2] = GSM1xSIG_MOB_MGMT_NON_GPRS_DISCRIMINATOR;   // TID is 0
         buffer[3] = GSM1xSIG_AUTH_RESPONSE;
         MEMCPY(&(buffer[4]),sres,4*sizeof(uint8)); 
         msgLength = 8;  

         if (GSM1xAuthRefNumberPresent) 
         {
            // Compose the Authentication Reference Number payload TLV
            buffer[8] = GSM1xSIG_RIL3_EXT_REF_NUM_TYPE;    // Type 
            buffer[9] = sizeof(GSM1xAuthRefNumber);        // Length
            MEMCPY(&(buffer[10]),&GSM1xAuthRefNumber,sizeof(GSM1xAuthRefNumber)); 
            msgLength = 14;
         }

         tmpMsg.ProtocolDataLen = msgLength;
      }
      else                                         // Send a normal RIL3 message 
      {  
         tmpMsg.ProtocolType = GSM1xSIG_PTYPE_RIL3;

         buffer[0] = GSM1xSIG_MOB_MGMT_NON_GPRS_DISCRIMINATOR;   // TID is 0
         buffer[1] = GSM1xSIG_AUTH_RESPONSE;
         MEMCPY(&(buffer[2]),sres,4*sizeof(uint8)); 

         tmpMsg.ProtocolDataLen = 6;
      }

      // TODO: trigger ciphering block

      // Can't do anything with the return value. If it fails, let it fail.
      //
      (void)OEMGSM1xSig_SVC_SendSignalingSMS(&tmpMsg);
   }

   GSM1xAuthProcessingRandFlag=FALSE; 
}


#endif /*FEATURE_GSM1x*/

