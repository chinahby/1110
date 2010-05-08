/*===========================================================================
FILE: OEMGSM1xSig.c

SERVICES: GSM1x Signaling interface.

GENERAL DESCRIPTION
   This file contains functions used to provide GSM1x Signaling services

PUBLIC CLASSES AND STATIC FUNCTIONS:
   AEEGSM1xSig
   AEEGSM1xSigNotifier

INITIALIZATION AND SEQUENCING REQUIREMENTS
   IGSM1xControl must be initialized before IGSM1xSig can be used.
   IGSM1xSig_Init must be called from AEE_Init.

        Copyright © 2001-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Then include OEM headers.*/
#include "OEMFeatures.h"

/* The whole file depends on the following include.*/                         
#if defined(FEATURE_GSM1x)

#include "OEMGSM1xMessage.h"

#include "OEMGSM1xSIG_SVC.h"

/* Then include AEE headers.*/
#include "AEE.h"
#include "AEEShell.h"
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
extern AEEGSM1xSig_Status OEMGSM1xControl_GetStatus(void);


/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/

/*lint -e754 */
struct IGSM1xSigNotifier
{
   AEEVTBL(IGSM1xSigNotifier) *pvt;

   IShell                  * m_pIShell;
   uint32                    m_dwNotify; 
   boolean                   m_bInitialized;

};


struct IGSM1xSig
{
   AEEVTBL(IGSM1xSig) *pvt;

   IShell                  * m_pIShell;

   boolean                   m_bInitialized;
};


/*===========================================================================

                      PUBLIC METHOD DECLARATIONS

These functions are called from BREW applets
===========================================================================*/

static uint32 OEMGSM1xSigNotifier_AddRef (IGSM1xSigNotifier * po);
static uint32 OEMGSM1xSigNotifier_Release(IGSM1xSigNotifier * po);
static void   OEMGSM1xSigNotifier_SetMask(IGSM1xSigNotifier * po, uint32 * pdwMask);

static uint32 OEMGSM1xSig_AddRef        (IGSM1xSig * po);
static uint32 OEMGSM1xSig_Release       (IGSM1xSig * po);
static int OEMGSM1xSig_QueryInterface   (IGSM1xSig * po, 
                                         AEECLSID clsid, void **ppNew);

AEEGSM1xSig_Status OEMGSM1xSig_GetStatus(IGSM1xSig *po);

int OEMGSM1xSig_SendSignalingMessage    (IGSM1xSig *po, 
                                         const AEEGSM1xSig_SignalingMessageType* pMsg);

int OEMGSM1xSig_SendSignalingReject     (IGSM1xSig *po, 
                                         const AEEGSM1xSig_RejectMessageType* pMsg);


/*===========================================================================

                    Protected Functions

These functions can be called from static BREW classes and DMSS/OEM code.
===========================================================================*/
void OEMGSM1xSigNotifier_Init(IShell * ps);
int  OEMGSM1xSigNotifier_New (IShell * ps, AEECLSID cls,void **ppif);

void OEMGSM1xSig_Init(IShell * ps);
int  OEMGSM1xSig_New (IShell * ps, AEECLSID cls,void **ppif);

void OEMGSM1xSig_ProcessSigCacheNotification(void);


/*===========================================================================

                    Private Functions

These functions are local to this module
===========================================================================*/

static void OEMGSM1xSig_processSigNotifyCache(void *dummy);

static int OEMGSM1xSig_sendSigMessageNotify(const AEEGSM1xSig_NotifyMessageType* sigMessage);

static boolean OEMGSM1xSig_verifyRejectMessage(const AEEGSM1xSig_RejectMessageType* pMsg);
static boolean OEMGSM1xSig_verifySignalingMessage(const AEEGSM1xSig_SignalingMessageType* pMsg);


/*===========================================================================

                     LOCAL/STATIC DATA
===========================================================================*/

static VTBL(IGSM1xSigNotifier) gOEMGSM1xSigNotifierFuncs = {
    OEMGSM1xSigNotifier_AddRef,
    OEMGSM1xSigNotifier_Release,
    OEMGSM1xSigNotifier_SetMask
};

static IGSM1xSigNotifier sGSM1xSigNotifier = {
                                               &gOEMGSM1xSigNotifierFuncs,  // Virtual table
                                               0,                           // m_pIShell
                                               0,                           // m_dwNotify
                                               FALSE                        // m_bInitialized
                                             };

static VTBL(IGSM1xSig) gOEMGSM1xSigFuncs = {
    OEMGSM1xSig_AddRef,
    OEMGSM1xSig_Release,
    OEMGSM1xSig_QueryInterface,

    OEMGSM1xSig_GetStatus,

    OEMGSM1xSig_SendSignalingMessage,
    OEMGSM1xSig_SendSignalingReject
};


static IGSM1xSig sGSM1xSig = {  /*lint -save -e785 */
                               &gOEMGSM1xSigFuncs,     // Virtual table
                               0,                      // m_pIShell
                               FALSE                   // m_bInitialized                                 
                             };
                              /*lint -restore */
                              // Rest of the fields are initialized later.

static AEECallback gNewSigMessageCB;


static AEEGSM1xSig_NotifyMessageType gGSM1xSigMessageTemp;

/*===========================================================================

                     BREW Methods

===========================================================================*/

/*=============================================================================
FUNCTION: OEMGSM1xSigNotifier_Init                           EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to initialize IGSM1xSigNotifier. This method will 
   initialize the singleton instance of IGSM1xSigNotifier by calling the
   OEMGSM1xSigNotifier_New method.

PARAMETERS:
   ps      : Pointer to the IShell object.
   
RETURN VALUE:
   None.
   
COMMENTS:
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
void OEMGSM1xSigNotifier_Init(IShell * ps)
{
   IGSM1xSigNotifier * pTmpGSM1xSigNotifier;

   CALLBACK_Init(&gNewSigMessageCB, 
                 OEMGSM1xSig_processSigNotifyCache,
                 NULL);

   if (OEMGSM1xSigNotifier_New(ps, AEECLSID_IGSM1XSIGNOTIFIER, (void *)&pTmpGSM1xSigNotifier) != SUCCESS)
   {
      FARF(ERROR,("Unable to init IGSM1xSigNotifier."));
   }
}


/*=============================================================================
FUNCTION: OEMGSM1xSigNotifier_New                            EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to create a new instance of IGSM1xSigNotifier. 
   IGSM1xSigNotifier is a singleton which is statically allocated. This function 
   call returns a pointer to the singleton. On the first call we initialize 
   the singleton.

PARAMETERS:
   ps      : Pointer to the IShell object.
   cls     : Class id of the class to be created.
   ppif    : pointer to the new class created.
   
RETURN VALUE:
   SUCCESS            -  Instance created successfully.
   ECLASSNOTSUPPORT   -  New called with the wrong class id.
   
COMMENTS:
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
int OEMGSM1xSigNotifier_New(IShell * ps, AEECLSID cls,void **ppif)
{
   *ppif = 0;

   if (cls != AEECLSID_IGSM1XSIGNOTIFIER) 
   {
      return ECLASSNOTSUPPORT;
   }

   if (sGSM1xSigNotifier.m_bInitialized)
   {
     *ppif = &sGSM1xSigNotifier;
      return SUCCESS;
   }

   // If we are here, we need to initialize GSM1xSigNotifier object
   //
   sGSM1xSigNotifier.m_pIShell = ps;
   ISHELL_AddRef(ps);

   *ppif = &sGSM1xSigNotifier;

   sGSM1xSigNotifier.m_bInitialized = TRUE;
   return SUCCESS;
}


/*=============================================================================
FUNCTION: OEMGSM1xSigNotifier_AddRef                         EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to increment the reference counter for an IGSM1xSigNotifier 
   instance. Since GSM1xSigNotifier is a singleton, we have implemented a dummy
   routine that always returns 1.

PARAMETERS:
   po      : Pointer to the IGSM1xSigNotifier object
   
RETURN VALUE:
   1
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static uint32 OEMGSM1xSigNotifier_AddRef(IGSM1xSigNotifier * pme)
{
   PARAM_NOT_REF(pme)
   return 1;
}


/*=============================================================================
FUNCTION: OEMGSM1xSigNotifier_Release                           EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to decrement the reference counter for an IGSM1xSigNotifier 
   instance. Since GSM1xSigNotifier is a singleton, we have implemented a dummy
   routine that always returns 0.

PARAMETERS:
   po      : Pointer to the IGSM1xSigNotifier object
   
RETURN VALUE:
   0
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static uint32 OEMGSM1xSigNotifier_Release(IGSM1xSigNotifier * pme)
{
   PARAM_NOT_REF(pme)
   return 0;
}


/*=============================================================================
FUNCTION: OEMGSM1xSigNotifier_SetMask                           EXTERNAL FUNCTION

DESCRIPTION:
   This method may is normally called from BREW to provide a list of masks 
   applications have registered for. 

PARAMETERS:
   po      : Pointer to the IGSM1xSigNotifier object
   pdwMask : pointer to the array containing masks.
   
RETURN VALUE:
   None.
   
COMMENTS:
   We could chose to do nothing here; however we use this to do some optimization.
   Before calling IShellNotify we verify if anybody is registered for the event we
   are about to generate. To do this m_dwNotify stores the OR of all masks 
   applications have registered for.
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static void OEMGSM1xSigNotifier_SetMask(IGSM1xSigNotifier * pme, uint32 * pdwMask)
{  
   uint32   dw, dwm;

   dwm = 0;
   while ((dw = *pdwMask) != 0)
   {
      dwm |= GET_NOTIFIER_MASK(dw);
      pdwMask++;
   }

   pme->m_dwNotify = dwm;
}

/*=============================================================================
FUNCTION: OEMGSM1xSig_Init                                  EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to initialize IGSM1xSigNotifier. This method will 
   initialize the singleton instance of IGSM1xSigNotifier by calling the
   OEMGSM1xSig_New method.

PARAMETERS:
   ps      : Pointer to the IShell object.
   
RETURN VALUE:
   None.
   
COMMENTS:
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
void OEMGSM1xSig_Init(IShell * ps)
{
   IGSM1xSig * pTmpGSM1xSig;

   OEMGSM1xSig_SVC_Init();

   if (OEMGSM1xSig_New(ps, AEECLSID_IGSM1XSIG, (void *)&pTmpGSM1xSig) != SUCCESS)
   {
       FARF(ERROR,("Unable to init IGSM1xSig."));
   }
}


/*=============================================================================
FUNCTION: OEMGSM1xSig_New                              EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to create a new instance of IGSM1xSig. IGSM1xSig is 
   a singleton which is statically allocated. This function call returns a 
   pointer to the singleton. On the first call we initialize the singleton.

PARAMETERS:
   ps      : Pointer to the IShell object.
   cls     : Class id of the class to be created.
   ppif    : pointer to the new class created.
   
RETURN VALUE:
   SUCCESS            -  Instance created successfully.
   ECLASSNOTSUPPORT   -  New called with the wrong class id.
   
COMMENTS:
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
int OEMGSM1xSig_New(IShell * ps, AEECLSID cls,void **ppif)
{
   *ppif = 0;

   if (cls != AEECLSID_IGSM1XSIG) 
   {
      return ECLASSNOTSUPPORT;
   }

   if (sGSM1xSig.m_bInitialized)
   {
     *ppif = &sGSM1xSig;
      return SUCCESS;
   }

   // If we are here, we need to initialize GSM1xSig object
   //
   sGSM1xSig.m_pIShell = ps;
   ISHELL_AddRef(ps);

   *ppif = &sGSM1xSig;

   sGSM1xSig.m_bInitialized = TRUE;
   return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMGSM1xSig_AddRef                                EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to increment the reference counter for an IGSM1xSig
   instance. Since GSM1xSig is a singleton, we have implemented a dummy
   routine that always returns 1.

PARAMETERS:
   po      : Pointer to the IGSM1xSig object
   
RETURN VALUE:
   1
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static uint32 OEMGSM1xSig_AddRef(IGSM1xSig * pme)
{
   PARAM_NOT_REF(pme)
   return 1;
}

/*=============================================================================
FUNCTION: OEMGSM1xSig_Release                               EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to decrement the reference counter for an IGSM1xSig
   instance. Since GSM1xSig is a singleton, we have implemented a dummy
   routine that always returns 0.

PARAMETERS:
   po      : Pointer to the IGSM1xSig object
   
RETURN VALUE:
   0
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static uint32 OEMGSM1xSig_Release(IGSM1xSig * pme)
{
   PARAM_NOT_REF(pme)
   return 0;
}


/*=============================================================================
FUNCTION: OEMGSM1xSig_QueryInterface                        EXTERNAL FUNCTION

DESCRIPTION:
   This method may be used to request an alternative interface to the IGSM1xSig. 
   Currently no alternative interface is defined.

PARAMETERS:
   po    : Pointer to the IGSM1xSig object.
   clsid : Class id of the interface requested. 
   ppNew : returns pointer to the new interface requested.
           NULL if the requested interface is  not supported.
   
RETURN VALUE:
   SUCCESS           -  ppNew contains a pointer to the requested interface.
   ECLASSNOTSUPPORT  -  requested interface is not supported.
   
COMMENTS:
   None.
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static int OEMGSM1xSig_QueryInterface(IGSM1xSig * pme, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEECLSID_IGSM1XSIG || clsid == AEECLSID_QUERYINTERFACE)
   {
      *ppNew = (void *)pme;
      (void)OEMGSM1xSig_AddRef(pme);
      return SUCCESS;
   }

   ppNew=0;
   return ECLASSNOTSUPPORT;
}

/*===========================================================================

                     IGSM1xSig Methods   

===========================================================================*/

/*=============================================================================
FUNCTION: OEMGSM1xSig_sendSigMessageNotify                   INTERNAL FUNCTION

DESCRIPTION:
   This function is used to send a protocol type notify event to BREW applets.
   This function should be invoked in BREW context. 

PARAMETERS:
   sigMessage: pointer to the signaling message to be send along with the notify.
   
RETURN VALUE:
   ECLASSNOTSUPPORT  - GSM1xSigNotifier not initialized.
   SUCCESS           - Event send out.
   
COMMENTS:
   None.
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static int OEMGSM1xSig_sendSigMessageNotify(const AEEGSM1xSig_NotifyMessageType* sigMessage)
{
   byte protocolType;
   uint32 mask;

   if (! sGSM1xSigNotifier.m_bInitialized)
   {
      return ECLASSNOTSUPPORT;
   }

   // If no one is registered for this event notification, return immediately.
   //
   if (! (sGSM1xSigNotifier.m_dwNotify & NMASK_GSM1xSIG_PROTOCOL_TYPE)) 
   {
      FARF(DEBUG,("GSM1x:Sig event no one registered."));
      return SUCCESS;
   }

   if (sigMessage->messageType == GSM1xSIG_SIGNALING) 
   {
      protocolType = sigMessage->msg.signalMessage.ProtocolType;
      FARF(DEBUG,("GSM1x:Sending Sig event proto(%d)",protocolType));
   } 
   else        /// GSM1xSIG_REJECT
   {
      protocolType = sigMessage->msg.rejectMessage.ProtocolTypeRejected;
      FARF(DEBUG,("GSM1x:Sending reject proto(%d)",protocolType));
   }
   mask = (protocolType << 16) |  NMASK_GSM1xSIG_PROTOCOL_TYPE;

   (void)ISHELL_Notify(sGSM1xSigNotifier.m_pIShell,
                       AEECLSID_IGSM1XSIGNOTIFIER,
                       mask,
                       (void *)sigMessage
                      );
   return SUCCESS;
}


/*=============================================================================
FUNCTION: OEMGSM1xSig_GetStatus                              EXTERNAL FUNCTION

DESCRIPTION:
   This method is used to retrieve the current GSM1x status on the 
   phone. This method lets the caller know if GSM1x capability is
   enabled or disabled on the mobile.

PARAMETERS:
   po: Pointer to the IGSM1xSig object
   
RETURN VALUE:
   GSM1xSIG_ACTIVE   - GSM1x capability enabled on the mobile
   GSM1xSIG_INACTIVE - GSM1x capability disabled on the mobile
   
COMMENTS:
   None.
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
AEEGSM1xSig_Status OEMGSM1xSig_GetStatus(IGSM1xSig *pme)   
{
   PARAM_NOT_REF(pme)
   return OEMGSM1xControl_GetStatus();
}


/*=============================================================================
FUNCTION: OEMGSM1xSig_processSigNotifyCache                  INTERNAL FUNCTION

DESCRIPTION:
Called after a context switch into the BREW context. Constructs a notification 
message and sends it to the registered clients.
        
PARAMETERS:
    dummy [in] : not used

RETURN VALUE:
  SUCCESS or EFAILED

SIDE EFFECTS:
  None
=============================================================================*/
static void OEMGSM1xSig_processSigNotifyCache(void *dummy)
{
   boolean                         messageFound;

   PARAM_NOT_REF(data);

   // Process the Cache entries for Supp. Services
   for (;;) 
   {
      messageFound = OEMGSM1xSig_SVC_GetSIGMessage(&gGSM1xSigMessageTemp);

      if (!messageFound)
         break;

      (void)OEMGSM1xSig_sendSigMessageNotify(&gGSM1xSigMessageTemp);
   }
}

/*=============================================================================
FUNCTION: OEMGSM1xSig_verifyRejectMessage                   INTERNAL FUNCTION

DESCRIPTION:
   This is a helper function called from SendSignalingReject to verify the 
   reject message. It will verify that the outgoing reject message is 
   not rejecting a protocol/message that is protected e.g. RIL3, authentication.

Parameters:
   pMsg :  pointer to the reject message to be send out.
   
Return Value:
   FALSE  -  Verification failed. This message should not be send out.
   TRUE   -  Verified. It is ok to send this message.
   
Comments:
   None.
   
Side Effects:
   None.
   
See Also:
=============================================================================*/
static boolean OEMGSM1xSig_verifyRejectMessage(const AEEGSM1xSig_RejectMessageType* pMsg)
{
   // Apps shouldn't be rejecting Config and Signal protocol
   //
   if ((pMsg->ProtocolTypeRejected == GSM1xSIG_PTYPE_SIGNAL) ||
       (pMsg->ProtocolTypeRejected == GSM1xSIG_PTYPE_CONFIG))
   {
      FARF(DEBUG,("GSM1x: Not allowed to reject Sig/Config."));
      return FALSE;
   }

   // If it not RIL3 or Ext RIL3, we don't care what apps do
   //
   if ((pMsg->ProtocolTypeRejected != GSM1xSIG_PTYPE_RIL3) &&
       (pMsg->ProtocolTypeRejected != GSM1xSIG_PTYPE_EXT_RIL3))
   {
      return TRUE;
   }

   // Length = 0 implies we don't support RIL3 protocol, we can't allow that
   if (pMsg->RejectCauseLength == 0) 
   {
      FARF(DEBUG,("GSM1x: Not allowed to reject RIL3 proto"));
      return FALSE;
   }

   // Dummy block to create local variables.
   {
      uint8 rejectByte = pMsg->RejectCause[0];
      uint8 rejectDisc = rejectByte | 0x0f;         // rejected discriminator 
      uint8 rejectType = rejectByte >> 4;

      if (rejectType == GSM1xSIG_RIL3_REJECT_FORMAT) 
      {
         // Apps are not allowed to reject RIL3 format
         FARF(DEBUG,("GSM1x: Not allowed to reject RIL3 format"));
         return FALSE;
      }

      if (rejectType == GSM1xSIG_RIL3_REJECT_DISCRIMINATOR) 
      {
         if (rejectDisc == GSM1xSIG_MOB_MGMT_NON_GPRS_DISCRIMINATOR) 
         {
            // Apps are not allowed to reject RIL3 
            FARF(DEBUG,("GSM1x: Can't reject MOB_MGMT_NON_GPRS."));
            return FALSE;
         }
      }

      if (rejectType == GSM1xSIG_RIL3_REJECT_MSG_TYPE)
      {
         uint8 rejectedMsgType = pMsg->RejectCause[1];

         if (pMsg->RejectCauseLength != 2) 
         {
            FARF(DEBUG,("GSM1x: Error in reject message"));
            return FALSE;
         }

         if (rejectedMsgType == GSM1xSIG_AUTH_REQUEST ||
             rejectedMsgType == GSM1xSIG_AUTH_RESPONSE)
         {
            FARF(DEBUG,("GSM1x: Can't reject auth request"));
            return FALSE;
         }
      }
   }
   return TRUE;
}
/*=============================================================================
FUNCTION: OEMGSM1xSig_verifySignalingMessage                INTERNAL FUNCTION

DESCRIPTION:
   This is a helper function called from SendSignalingMessage to verify outgoing
   signaling message. It will verify that the outgoing signaling message does 
   not contain any authentication related message.

Parameters:
   pMsg :  pointer to the signaling message to be send out.
   
Return Value:
   FALSE  -  Verification failed. This message should not be send out.
   TRUE   -  Verified. It is ok to send this message.
   
Comments:
   None.
   
Side Effects:
   None.
   
See Also:
=============================================================================*/
static boolean OEMGSM1xSig_verifySignalingMessage(const AEEGSM1xSig_SignalingMessageType* pMsg )
{

   if (pMsg->ProtocolType == GSM1xSIG_PTYPE_RIL3) 
   {
      uint8 ril3Header   = pMsg->ProtocolData[0];
      uint8 messageType  = pMsg->ProtocolData[1];
      uint8 protocolDisc = ril3Header & 0x0f;

      if (protocolDisc == GSM1xSIG_MOB_MGMT_NON_GPRS_DISCRIMINATOR) 
      {
         if (messageType == GSM1xSIG_AUTH_REQUEST ||
             messageType == GSM1xSIG_AUTH_RESPONSE)
         {
            FARF(DEBUG,("GSM1x: Trying to send auth. Rejected"));
            return FALSE;
         }
      }
   }
   else if (pMsg->ProtocolType == GSM1xSIG_PTYPE_EXT_RIL3)
   {
      uint8 cursor=0;

      while (cursor < pMsg->ProtocolDataLen)
      {
         uint8 type = pMsg->ProtocolData[cursor];
         uint8 len  = pMsg->ProtocolData[cursor+1];

         if (type == GSM1xSIG_RIL3_EXT_GSM_TYPE) break; 
         cursor = cursor + len + 2;
      }
      if (cursor >= pMsg->ProtocolDataLen) 
      {
         // Message doesn't have GSM_Data field present. Reject this
         return FALSE;
      }
      else
      {
         uint8 ril3Header   = pMsg->ProtocolData[cursor+2];
         uint8 messageType  = pMsg->ProtocolData[cursor+3];
         uint8 protocolDisc = ril3Header & 0x0f;

         if (protocolDisc == GSM1xSIG_MOB_MGMT_NON_GPRS_DISCRIMINATOR) 
         {
            if (messageType == GSM1xSIG_AUTH_REQUEST ||
                messageType == GSM1xSIG_AUTH_RESPONSE)
            {
               FARF(DEBUG,("GSM1x: Trying to send auth. Rejected"));
               return FALSE;
            }
         }
      }
   }

   return TRUE;
}

/*=============================================================================
FUNCTION: OEMGSM1xSig_SendSignalingMessage                   INTERNAL FUNCTION

DESCRIPTION:
   This method is used to send a GSM1x signaling message (except for
   GSM1x Authentication Req/Rsp Message) to the network.
   Use OEMGSM1xSig_sendSignalingSMS routine to bypass check for authentication
   messages.

Parameters:
   po: Pointer to the IGSM1xSig object
   pMsg: Pointer to the AEEGSM1xSig_SignalingMessageType struct containing the
         GSM1x signaling msg details 
   
Return Value:
   SUCCESS         - GSM1x Signaling Message queued up.
   EBADPARM        - Value in pMsg is invalid (ex. attempting to send Auth msg)
   EFAILED         - General Failure in sending out the signaling message
   EITEMBUSY       - No more buffers left for sending messages.
   EGSM1x_INACTIVE - Phone is not in IGSM1x mode
   EBADCLASS:      - IPhone is not initialized.
   
Comments:
   None.
   
Side Effects:
   None.
   
See Also:
=============================================================================*/
int OEMGSM1xSig_SendSignalingMessage 
(
   IGSM1xSig *po, 
   const AEEGSM1xSig_SignalingMessageType* pMsg
)
{
   PARAM_NOT_REF(po)
   if (pMsg == NULL)                 return EBADPARM;
   if (pMsg->ProtocolRevision > 15)  return EBADPARM;
   if (pMsg->ProtocolType > 15)      return EBADPARM;

   if (!OEMGSM1xSig_verifySignalingMessage(pMsg))
   {
      FARF(DEBUG,("GSM1x: VerifySignalingMessage failed"));
      return EBADPARM;
   }

   return OEMGSM1xSig_SVC_SendSignalingSMS(pMsg);
}


/*=============================================================================
FUNCTION: OEMGSM1xSig_SendSignalingReject                   INTERNAL FUNCTION

DESCRIPTION:
   This method is used to send a GSM1x signaling reject message to
   the network.

Parameters:
   po: Pointer to the IGSM1xSig object
   pMsg: Pointer to the AEEGSM1xSig_RejectMessageType struct containing the
         GSM1x signaling reject msg
   
Return Value:
   SUCCESS         - GSM1x Reject Message queued up.
   EBADPARM        - Value in pMsg is invalid (ex. attempting to send Auth msg)
   EFAILED         - General Failure in sending out the signaling message
   EITEMBUSY       - No more buffers left for sending messages.
   EGSM1x_INACTIVE - Phone is not in IGSM1x mode
   EBADCLASS:      - IPhone is not initialized.

Comments:
   None.
   
Side Effects:
   None.
   
See Also:
=============================================================================*/
int OEMGSM1xSig_SendSignalingReject
(
   IGSM1xSig *po, 
   const AEEGSM1xSig_RejectMessageType* pMsg
)
{
   PARAM_NOT_REF(po)
   AEEGSM1xSig_SignalingMessageType tmpMsg;

   if (pMsg == NULL)                    return EBADPARM;
   if (pMsg->ProtocolTypeRejected > 15) return EBADPARM;

   // Verify that apps are not doing something they shouldn't be.
   //
   if (!OEMGSM1xSig_verifyRejectMessage(pMsg)) return EBADPARM;


   tmpMsg.ProtocolData[0] = GSM1xSIG_CONFIG_REJECT; // message type = config reject
   tmpMsg.ProtocolData[1] = (GSM1xSIG_CURRENT_PROTOCOL_REVISION << 4)
                             | GSM1xSIG_MIN_PROTOCOL_REVISION ;
   tmpMsg.ProtocolData[2] = (pMsg->ProtocolTypeRejected << 4);       /*lint !e734 */
   tmpMsg.ProtocolData[3] = pMsg->RejectCauseLength;
   MEMCPY(&(tmpMsg.ProtocolData[4]),
          pMsg->RejectCause,
          pMsg->RejectCauseLength);

   tmpMsg.ProtocolDataLen  = pMsg->RejectCauseLength + 4;
   tmpMsg.ProtocolRevision = GSM1xSIG_CURRENT_PROTOCOL_REVISION;
   tmpMsg.ProtocolType     = GSM1xSIG_PTYPE_CONFIG;

   FARF(DEBUG,("GSM1x: Submitting reject message"));
   return OEMGSM1xSig_SVC_SendSignalingSMS(&tmpMsg);
}


/*=============================================================================
FUNCTION: OEMGSM1xSig_ProcessSigCacheNotification

DESCRIPTION:
   Notify that new MT messages are available in the cache.
        
PARAMETERS:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
=============================================================================*/
void OEMGSM1xSig_ProcessSigCacheNotification(void)
{
   AEE_ResumeCallback(&gNewSigMessageCB, 0);
}


#endif /*FEATURE_GSM1x*/

