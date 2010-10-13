/*
FILE:  OEMSTK.c

SERVICES:  OEM functions for supporting BREW ISTK Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW ISTK interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

        Copyright © 1999-2006, 2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================
            EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/istk/rel/07H1_2/src/AEESTK.c#3 $
$DateTime: 2009/07/10 12:58:45 $
$Author: xiaominz $
$Change: 964241 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/09/09   xz      Fix compile error when call OEMSTK_GetNextCmd()
06/30/09   xz      Send error TR when ISTK fails to dispatch command
09/05/08   tms     Add IModel support
06/12/06   sk      Enhancement to AEESTK_SendCommandResponse
02/16/06   sk      Added Language Selection event support
07/22/05   sk      Added Idle Screen support
05/24/05   sk      Linuxify fixes
05/11/05   sst     Lint fixes
03/05/05   sk      Added support for Browser Termination
                   evt dl and added checks for NULL pointers.
01/07/04   sun     Fixed Compilation Warnings
=============================================================================*/


#include "err.h"        // Error functions

#include "OEMHeap.h"
#include "OEMFeatures.h"
#include "OEMSTK.h"

#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEE.h"
#include "AEEComdef.h"     // Get DMSS type declarations.
#include "AEESTK.h"

#include "STK.BID"

#ifdef FEATURE_AEESTK_IMODEL_SUPPORT
#include "AEEModel.h"
#include "AEECLSID_MODEL.bid"
#endif /* FEATURE_AEESTK_IMODEL_SUPPORT */

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#endif 
#include "BREWVersion.h"

struct ISTK
{
  AEEVTBL(ISTK)             *pvt; 
  AEECallback                m_cbSysObject;
  void                      *m_pAContext;
  IShell                    *m_pIShell;
  AEECLSID                   m_cls; /*lint -esym(754,ISTK::m_cls) */
  uint32                     m_uRefs;           /* Number of references to this obj */
  OEMSTK                    *m_pSTK;
#ifdef FEATURE_AEESTK_IMODEL_SUPPORT
  IModel                    *m_pModel;    // IModel to hold listeners
  AEECallback                m_cbModel;   // Callback called by OEMSTK when GSTK provides a command
  AEESTKNotifierData         m_modelData; // Model notification data
#endif /* FEATURE_AEESTK_IMODEL_SUPPORT */
};

/* ISTKNotifier object, Singleton */
struct ISTKNotifier 
{
  AEEVTBL(ISTKNotifier)   *pvt; /*lint -esym(754,ISTKNotifier::pvt) */
  IShell                  *m_pIShell;
  uint32                   m_uRefs;  
  AEECLSID                 m_cls;    
  uint32                   m_dwNotify;  /* Notification bits */
  AEECallback              cb;      /* Callback called by OEMSTK when GSTK provides a command */
  OEMSTK                  *m_pSTK;     /* OEMSTK ptr */
  AEECallback             *m_pcbExit;
};


#ifndef FEATURE_BREW_3_0
#define AEE_ENTER_SYSTEM_CONTEXT()  AEE_SetAppContext(NULL)
#define AEE_LEAVE_SYSTEM_CONTEXT(p) (void)AEE_SetAppContext(p)
#else
#define AEE_ENTER_SYSTEM_CONTEXT()  AEE_EnterAppContext(NULL)
#define AEE_LEAVE_SYSTEM_CONTEXT(p) AEE_LeaveAppContext(p)
#endif

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

/* Interface to the STK Notifier object. */
static uint32 AEESTKNotifier_AddRef(ISTKNotifier *po);
static uint32 AEESTKNotifier_Release(ISTKNotifier *po);
#if MIN_BREW_BUILD(3,1,4,7)
static void   AEESTKNotifier_SetMask(ISTKNotifier *po, uint32 *pdwMask);
#else
static void   AEESTKNotifier_SetMask(ISTKNotifier *po, const uint32 *pdwMask);
#endif // MIN_BREW_BUILD(3,1,4,7)
static uint32 AEESTK_MergeMasks(const uint32 * pdwMasks);

static uint32 AEESTK_AddRef(ISTK *pMe);

static uint32 AEESTK_Release(ISTK *pMe);


static int AEESTK_QueryInterface(
              ISTK     *pMe,
              AEECLSID id,
              void     **ppNew);

  
static int AEESTK_SendCommandResponse
                (
                  ISTK                      *pISTK,
                  AEESTKProactiveCmdHeader  cmdHdr,
                  AEESTKResult              result,                       
                  AEESTKAdditionalInfo      *additionalResult,                        
                  void                      *extra_param,
                  uint8                      extra_param_length
                );

static int AEESTK_SendCommandResponseEx
                (
                  ISTK                      *pISTK,
                  AEESTKProactiveCmdHeader  cmdHdr,
                  AEESTKResult              result,                       
                  AEESTKAdditionalInfo      *additionalResult,                        
                  void                      *extra_param
                );

static int AEESTK_SendEnvelopeEvtDownloadCommand
                (
                  ISTK                      *pISTK, 
                  uint16                    *pwTransactionID,
                  uint32                    dwSize,
                  AEESTKEventDLInd          *evtList
                );

static int AEESTK_SendEnvelopeMenuSelCommand
                (
                  ISTK                        *pISTK,
                  uint16                    *pwTransactionID,
                  byte                      nItemChosen,                          
                  boolean                   bHelp
                 );


static int AEESTK_UserActivity(ISTK *pISTK);
static int AEESTK_IdleScreen(ISTK *pISTK);
static int AEESTK_LangSelection(ISTK *pISTK, 
                                AEESTKLangSelectionResponse *sLangSelection);

static int AEESTK_ChannelStatusChange(ISTK *pISTK, AEESTKGetChStatusResponse sChStatus);

static int AEESTK_DataAvailable(ISTK *pISTK, AEESTKDataAvailNotify sDataAvail);

static int AEESTK_BrowserTermination(ISTK *pISTK, AEESTKBrowserTermEnumType sBrowserTerm);

static void AEESTK_OnCardCmd(ISTKNotifier *pMe);

#ifdef FEATURE_AEESTK_IMODEL_SUPPORT
static void AEESTK_OnCardCmdModel( ISTK *pMe );
#endif /* FEATURE_AEESTK_IMODEL_SUPPORT */

int AEESTK_GetCmdData
                (
                  ISTK                      *pISTK, 
                  uint32                    dwTransactionID, 
                  void                      *pCmdBuf, 
                  uint32                    *pBufLen
                );


static const AEEVTBL(ISTK) gISTKFuncs = {
  AEESTK_AddRef,
  AEESTK_Release,
  AEESTK_QueryInterface,
  AEESTK_GetCmdData,
  AEESTK_SendCommandResponse, 
  AEESTK_SendCommandResponseEx,
  AEESTK_SendEnvelopeEvtDownloadCommand,
  AEESTK_SendEnvelopeMenuSelCommand,
  AEESTK_UserActivity,
  AEESTK_ChannelStatusChange,
  AEESTK_DataAvailable,
  AEESTK_BrowserTermination,
  AEESTK_IdleScreen,
  AEESTK_LangSelection
};

/* The functions supported by STK Notifier object. */
static const AEEVTBL(ISTKNotifier) gISTKNotifierFuncs = {
   AEESTKNotifier_AddRef,
   AEESTKNotifier_Release,
   AEESTKNotifier_SetMask
};
/***********************************************************************
 * D A T A    O B J E C T S
 ***********************************************************************/
static struct ISTKNotifier *ISTKNotifierObj = NULL;

/*=============================================================================
FUNCTION: AEESTKNotifier_FreeSTKNotifier

DESCRIPTION:  
  Function to clean up ISTKNotifier members

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void AEESTKNotifier_FreeSTKNotifier(ISTKNotifier *pMe)
{
  if (!pMe)
    return;

  if (pMe->m_pSTK)
  {
    (void)OEMSTK_Release(pMe->m_pSTK);
    pMe->m_pSTK = NULL;
  }

  if (pMe->m_pIShell) {
    (void)ISHELL_Release(pMe->m_pIShell);
    pMe->m_pIShell = NULL;
  }

  CALLBACK_Cancel(&pMe->cb);
  
  if (pMe->m_pcbExit) {
    CALLBACK_Cancel(pMe->m_pcbExit);
    sys_free(pMe->m_pcbExit);
    pMe->m_pcbExit = NULL;
  }

  sys_free(pMe);
  ISTKNotifierObj = NULL;
}

/*=============================================================================
FUNCTION: AEESTKNotifier_ExitNotify

DESCRIPTION:  
  Clean up callback function for ISTKNotifier 

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void AEESTKNotifier_ExitNotify(ISTKNotifier * po)
{
  MSG_HIGH("Exit ISTKNotifier in AEEExitNotify, obj: 0x%x", 
           ISTKNotifierObj, 0, 0);
  if (ISTKNotifierObj != NULL) {
    AEESTKNotifier_FreeSTKNotifier(ISTKNotifierObj);
  }
  return;
} /*lint !e715 */


/*=============================================================================
FUNCTION: ISTKNotifier_New

DESCRIPTION:  
  Allocates a new ISTKNotifier object and initializes it or adds reference to
  an existing object.
  Allocates a ISTK object if it was not already present. 

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int AEESTKNotifier_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
  void *pac = NULL;

  /* Check parameters. */
  if ((!pIShell) || (!ppif)) {
    return EBADPARM;
  }

  if (cls != AEECLSID_STKNOTIFIER) {
    return EUNSUPPORTED;
  }

  /* If the object doesn't already exist, allocate it now. */
  if (ISTKNotifierObj == NULL) {
    /* Allocate the object. */

    ISTKNotifierObj = (ISTKNotifier*)(void*)AEE_OEM_NEWCLASSEX((IBaseVtbl*)(void*)&gISTKNotifierFuncs,
                                                     sizeof(ISTKNotifier), TRUE); 
        
    if (ISTKNotifierObj == NULL) {
      *ppif = NULL;
      return ENOMEMORY;
    }

    /* Initialize the values. */
    ISTKNotifierObj->m_pIShell = pIShell;
    (void)ISHELL_AddRef(pIShell);
    ISTKNotifierObj->m_cls = cls;
    ISTKNotifierObj->m_uRefs = 1;
    ISTKNotifierObj->m_dwNotify = 0;

    //Create a new OEMSTK instance.
    (void)OEMSTK_New(pIShell, &ISTKNotifierObj->m_pSTK);

    //Initialize the callback.
    CALLBACK_Init(&ISTKNotifierObj->cb, (PFNNOTIFY) AEESTK_OnCardCmd, ISTKNotifierObj);
    //Provide OEMSTK with the callback.
    OEMSTK_OnCardCmd(ISTKNotifierObj->m_pSTK, &ISTKNotifierObj->cb);

    if(!ISTKNotifierObj->m_pSTK) {
      (void) AEESTKNotifier_Release(ISTKNotifierObj); 
      ISTKNotifierObj = NULL;
      *ppif = NULL;
      return EFAILED;
    }

    /* register for AEE Exit event */
    ISTKNotifierObj->m_pcbExit = sys_malloc(sizeof(AEECallback));
    if (!ISTKNotifierObj->m_pcbExit)
    {
      (void) AEESTKNotifier_Release(ISTKNotifierObj); 
      ISTKNotifierObj = NULL;
      *ppif = NULL;
      return ENOMEMORY;
    }

    pac = AEE_ENTER_SYSTEM_CONTEXT();
    CALLBACK_Init(ISTKNotifierObj->m_pcbExit, (PFNNOTIFY)AEESTKNotifier_ExitNotify,
                    ISTKNotifierObj);
#ifndef FEATURE_BREW_3_0
    AEE_RegisterSystemCallback(ISTKNotifierObj->m_pcbExit, AEE_SCB_AEE_EXIT,
                                AEE_RESUME_CB_SYS,0);
#else
    ISHELL_OnExit(pIShell, ISTKNotifierObj->m_pcbExit);
#endif
    AEE_LEAVE_SYSTEM_CONTEXT(pac);
  }
  else
  {
    (void) AEESTKNotifier_AddRef(ISTKNotifierObj);
  }

  *ppif = ISTKNotifierObj;

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: AEESTKNotifier_AddRef

DESCRIPTION:  
  Indicates that one or more object is now pointing to ISTKNotifier object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 AEESTKNotifier_AddRef(ISTKNotifier *pMe)
{
  if (pMe) {
    return (++pMe->m_uRefs);
  } 
  return 0;
}

/*=============================================================================
FUNCTION: AEESTKNotifier_Release

DESCRIPTION:  
  Decreases the number of references to the ISTKNotifier object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 AEESTKNotifier_Release(ISTKNotifier *pMe)
{
   if (!pMe)
      return (0);

   if (pMe->m_uRefs > 0) {
     if(--pMe->m_uRefs) return pMe->m_uRefs;
   }

   AEESTKNotifier_FreeSTKNotifier(pMe);
   
   return (0);
}

/*=============================================================================
FUNCTION: AEESTKNotifier_SetMask

DESCRIPTION:  
  This function is called by Brew when an application does an
  ISHELL_RegisterNotify for the ISTK_NOTIFIER object.  We merge
  all the masks for all the registrations and save a bitmap of
  the sum of all the events being requested.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
#if MIN_BREW_BUILD(3,1,4,7)
static void AEESTKNotifier_SetMask(ISTKNotifier *pMe, uint32 *pdwMask)
#else
static void AEESTKNotifier_SetMask(ISTKNotifier *pMe, const uint32 *pdwMask)
#endif // MIN_BREW_BUILD(3,1,4,7)
{
  if (!pMe || !pdwMask)
    return;

  pMe->m_dwNotify = AEESTK_MergeMasks(pdwMask);  
}

/*=============================================================================
FUNCTION: ISTK_MergeMasks

DESCRIPTION:  


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 AEESTK_MergeMasks(const uint32 * pdwMasks)
{
   uint32   dw,dwm;

   dwm = 0;
   while((dw = *pdwMasks) != 0){
      dwm |= ((uint16)(dw & 0x0000FFFF));
      pdwMasks++;
   }
   return dwm;
}


/*=============================================================================
FUNCTION: AEESTK_FreeSTK

DESCRIPTION:  
  Free ISTK members

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void AEESTK_FreeSTK(ISTK *pMe)
{
  if (pMe == NULL) {
    return;
  }

#ifdef FEATURE_AEESTK_IMODEL_SUPPORT
  CALLBACK_Cancel(&pMe->m_cbModel);

  if (NULL != pMe->m_pModel) {
     IMODEL_Release(pMe->m_pModel);
  }
#endif /* FEATURE_AEESTK_IMODEL_SUPPORT */
  
  CALLBACK_Cancel(&pMe->m_cbSysObject);
  if (pMe->m_pIShell) {
    (void)ISHELL_Release(pMe->m_pIShell);
    pMe->m_pIShell = NULL;
  }

  if (pMe->m_pSTK) {
    (void)OEMSTK_Release(pMe->m_pSTK);
    pMe->m_pSTK  = NULL;
  }
  FREE(pMe);
  pMe = NULL;
}


/*=============================================================================
FUNCTION: AEESTK_SysObjectCB

DESCRIPTION:  
  Registered callback that will be called when the application goes away.  This
  will trigger freeing of the ISTK

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void AEESTK_SysObjectCB(void *pUsr)
{
  ISTK *pMe = (ISTK*)pUsr;

  if (pMe == NULL) {
    return;
  }
  AEESTK_FreeSTK((ISTK*)pUsr);
}



/*=============================================================================
FUNCTION: AEESTK_OnCardCmd

DESCRIPTION:  
  This is the call back OEMSTK calls on receiving a command from card
  This is called in AEE context. At this point, all it knows is there 
  is a command. It will get the TransationID and command_type (mask) 
  from OEMSTK then it will notify the registered clients(applications).
  Application will do AEESTK_GetCommandBuf based on mask and 
  the command_id provided. 

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void AEESTK_OnCardCmd(ISTKNotifier *pMe)
{
  uint32 commandType; 
  uint8  cmdNum;
  int nReturnStatus;
  AEESTKNotifierData notify_data;
  AEESTKAdditionalInfo  additional_info;
    
  if(!pMe)
  { 
    MSG_ERROR("NULL pMe pointer", 0, 0, 0);
    return;
  }    
  // Here commandType will specify the mask to be sent.
  // dwTransationID specifies the index in the OEMSTK cache where to fetch it.
  //CALLBACK_Cancel(&pMe->cb);
  OEMSTK_GetNextCmd(pMe->m_pSTK,
                    &commandType,
                    &cmdNum,
                    &notify_data.dwTransactionID);
  
  (void)OEMSTK_GetBufSize(pMe->m_pSTK, notify_data.dwTransactionID, &notify_data.dwCmdDataSize);

  // TODO Will not have access to notify_data... don't pass pointer... 
  // and can free after this function since ISHELL_Notify is synchronous
  nReturnStatus = ISHELL_Notify(pMe->m_pIShell, AEECLSID_STKNOTIFIER, 
    (commandType), (void*) (&notify_data));
  if(nReturnStatus != SUCCESS)
  {
    MSG_ERROR("Failed ISHELL_Notify : %d", nReturnStatus, 0 ,0 );
    additional_info.nLength = 0;
    if (SUCCESS != OEMSTK_SendCommandResponse(
                     pMe->m_pSTK,
                     (AEESTKProactiveCmdType)(notify_data.dwCmdDataSize & 0x0000FFFF),
                     cmdNum,
                     AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                     &additional_info,                               
                     NULL,
                     0)
       )
    {
      MSG_ERROR("Failed to send error term rsp!", 0, 0, 0);
    }
  }
}


#ifdef FEATURE_AEESTK_IMODEL_SUPPORT
/*=============================================================================
FUNCTION: AEESTK_OnCardCmdModel

DESCRIPTION:  
  This is the call back OEMSTK calls on receiving a command from card
  when an IModel notification is used.

  This is called in AEE context. At this point, all it knows is there 
  is a command. It will get the TransationID and command_type (mask) 
  from OEMSTK then it will notify the registered clients(applications).
  Application will do AEESTK_GetCommandBuf based on mask and 
  the command_id provided. 

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void AEESTK_OnCardCmdModel( ISTK *pMe )
{
  ModelEvent event;
  uint32 commandType = 0;
  uint8  cmdNum;

  if (NULL == pMe) { 
    MSG_ERROR("NULL pMe pointer", 0, 0, 0);
    return;
  }
  
  pMe->m_modelData.dwTransactionID = 0;
  pMe->m_modelData.dwCmdDataSize = 0;

  // Here commandType will specify the mask to be sent.
  // dwTransationID specifies the index in the OEMSTK cache where to fetch it.
  OEMSTK_GetNextCmd( pMe->m_pSTK,
                     &commandType,
                     &cmdNum,
                     &(pMe->m_modelData.dwTransactionID) );

  (void)OEMSTK_GetBufSize( pMe->m_pSTK,
                           pMe->m_modelData.dwTransactionID,
                           &(pMe->m_modelData.dwCmdDataSize) );

  /* 
     Set up notify event.  The data pointer sent refers to the
     model data structure held in pMe.  This is done to avoid
     an invalid pointer dereference when this function goes out
     of scope.
  */
  event.evCode = EVT_MDL_GENERIC_CHANGE;           // Simple generic change
  event.pModel = pMe->m_pModel;                    // The model issuing the notify
  event.dwParam = (uint32)(void *)&(pMe->m_modelData); // Data passed to the listeners

  IMODEL_Notify( pMe->m_pModel, &event );
}
#endif /* FEATURE_AEESTK_IMODEL_SUPPORT */

/*=============================================================================
FUNCTION: AEESTK_New

DESCRIPTION:  
  Creation of ISTK object and it's member

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int AEESTK_New( IShell *pIShell, 
                AEECLSID uCls, 
                void **ppif )
{
   int     nStatus  = SUCCESS;
   OEMSTK *pOEMSTK  = NULL;
   ISTK   *pMe      = NULL;

   if (ppif == NULL) {
     return EFAILED;
   }
   if ((nStatus = OEMSTK_New(pIShell, &pOEMSTK)) == SUCCESS)
   {
      pMe = (ISTK*)MALLOC(sizeof(ISTK));

      *ppif = pMe;

      if (pMe) 
      {
         pMe->pvt      = (AEEVTBL(ISTK)*) &gISTKFuncs;
         pMe->m_pSTK   = pOEMSTK;
         pMe->m_uRefs   = 1;
         pMe->m_pIShell  = pIShell;
         (void)ISHELL_AddRef(pIShell);
         CALLBACK_Init(&pMe->m_cbSysObject, AEESTK_SysObjectCB, (void*)pMe);
         AEE_LinkSysObject(&pMe->m_cbSysObject);
         pMe->m_pAContext = AEE_GetAppContext();
      }
      else 
      {                       
         (void)OEMSTK_Release(pOEMSTK);
         nStatus = ENOMEMORY;
      }
   }
   else {
     *ppif = NULL;
   }
   return nStatus;
} /*lint !e715 */

/*=============================================================================
FUNCTION: AEESTK_AddRef

DESCRIPTION:  
  Add reference to the existing ISTK object

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 AEESTK_AddRef(ISTK *pMe)
{
  if (pMe) {
    return (++pMe->m_uRefs);
  } 
  return 0;
}

/*=============================================================================
FUNCTION: AEESTK_Release

DESCRIPTION:  
  Release the ISTK object and it's member

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 AEESTK_Release(ISTK *pMe)
{
   if (!pMe)
      return (0);

   if (pMe->m_uRefs > 0) {
     if(--pMe->m_uRefs) return pMe->m_uRefs;
   }

   AEESTK_FreeSTK(pMe);
   return (0);
}

/*=============================================================================
FUNCTION: AEESTK_QueryInterface

DESCRIPTION:  
  Query interface for the ISTK object

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int AEESTK_QueryInterface(
              ISTK     *pMe,
              AEECLSID id,
              void     **ppo)
{
  if ((ppo == NULL) || (pMe == NULL)) {
    return EBADPARM;
  }
  if ((id == AEECLSID_STK) || (id == AEECLSID_QUERYINTERFACE))
  {
     *ppo = pMe;
     (void)IQI_AddRef(*ppo);
     return SUCCESS;
  } 
#ifdef FEATURE_AEESTK_IMODEL_SUPPORT
  else if (AEEIID_MODEL == id)
  {
      int result = SUCCESS;

      if (NULL == pMe->m_pModel) {
         // Get an IModel interface to hold our listeners
         result = ISHELL_CreateInstance( pMe->m_pIShell,
                                         AEECLSID_MODEL,
                                         (void**) &pMe->m_pModel );
         if (SUCCESS != result) {
            MSG_ERROR("Failed to CI(Model)", 0, 0, 0);
            return result;
         }
      
         // Initialize the model callback
         CALLBACK_Init( &pMe->m_cbModel, (PFNNOTIFY) AEESTK_OnCardCmdModel, pMe );

         // Provide OEMSTK layer with the model callback.
         // Requesting an IModel interface silently disables the callback
         // set up for event notifications through AEESTKNotifier
         // (see AEESTKNotifier_New).  In other words, only use ONE
         // notification mechanism, not both.
         OEMSTK_OnCardCmd(pMe->m_pSTK, &pMe->m_cbModel);
      }

      *ppo = pMe->m_pModel;
      (void)IMODEL_AddRef(*ppo);
      
      return result;
  }
#endif /* FEATURE_AEESTK_IMODEL_SUPPORT */
  else 
  {
     *ppo = NULL;
     return ECLASSNOTSUPPORT;
  }
}
/*=============================================================================
FUNCTION: AEESTK_GetCmdData

DESCRIPTION:  
  Function to return the size of the command data as well as the command data

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int AEESTK_GetCmdData
                (
                  ISTK                      *pISTK, 
                  uint32                    dwTransactionID, 
                  void                      *pCmdBuf, 
                  uint32                    *pBufLen
                )
{
  if((!pISTK) || (!pBufLen))
    return EBADPARM;
  if(pCmdBuf == NULL) //User is interested in the length required.
    return OEMSTK_GetBufSize(pISTK->m_pSTK, dwTransactionID, pBufLen);
  else
    return OEMSTK_GetCachedEntry(pISTK->m_pSTK, dwTransactionID, pCmdBuf, *pBufLen);
}

/*=============================================================================
FUNCTION: AEESTK_SendCommandResponse

DESCRIPTION:  
  Function to send Terminal response to the card

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int AEESTK_SendCommandResponse
                (
                  ISTK                      *pISTK,
                  AEESTKProactiveCmdHeader  cmdHdr,
                  AEESTKResult              result,                       
                  AEESTKAdditionalInfo      *additionalResult,                        
                  void                      *extra_param,
                  uint8             extra_param_length
                )
{
  if((!pISTK) || (!additionalResult))
    return EFAILED;
  return OEMSTK_SendCommandResponse(pISTK->m_pSTK, cmdHdr.nCommandID, cmdHdr.nCommandNumber,
                              result, additionalResult, extra_param, extra_param_length);
}


/*=============================================================================
FUNCTION: AEESTK_SendCommandResponseEx

DESCRIPTION:  
  Function to send Terminal response to the card. This is an extension to
  AEESTK_SendCommandResponse and takes in new response structures that has  
  information for appropriate processing and packing before sending TR to the card.
  A Client should use this new API for Get Input command response. For all
  other commands, the old API AEESTK_SendCommandResponse() can be used.
  
INPUT
  pISTK              - [Input] pointer to OEMSTK object
  cmdHdr             - [Input] command header details
  result             - [Input] result value                      
  additionalResult   - [Input] additional info to result                        
  extra_param        - [Input] extra parameters depending on proactive command
  extra_param_length - [Input] extra parameters length field.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int AEESTK_SendCommandResponseEx
                (
                  ISTK                      *pISTK,
                  AEESTKProactiveCmdHeader  cmdHdr,
                  AEESTKResult              result,                       
                  AEESTKAdditionalInfo      *additionalResult,                        
                  void                      *extra_param
                )
{
  if((!pISTK) || (!additionalResult))
    return EFAILED;

  /* extra_param may be NULL depending on the proactive command */

  return OEMSTK_SendCommandResponseEx(pISTK->m_pSTK, cmdHdr.nCommandID,
                                       cmdHdr.nCommandNumber,result, additionalResult, 
                                       extra_param);
}

/*=============================================================================
FUNCTION: AEESTK_SendEnvelopeEvtDownloadCommand

DESCRIPTION:  
  Function to allow application to send event download command

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int AEESTK_SendEnvelopeEvtDownloadCommand
                (
                  ISTK                      *pISTK, 
                  uint16                    *pwTransactionID,
                  uint32                    dwSize,
                  AEESTKEventDLInd          *evtList
                )
{
  if((!pISTK) || (!pwTransactionID) || (!evtList))
    return EFAILED;
  return OEMSTK_SendEnvelopeEvtDownloadCommand(pISTK->m_pSTK, pwTransactionID, dwSize, evtList);
}

/*=============================================================================
FUNCTION: AEESTK_SendEnvelopeMenuSelCommand

DESCRIPTION:  
  Function to allow application to send a menu selection envelope command

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int AEESTK_SendEnvelopeMenuSelCommand
                (
                  ISTK                        *pISTK,
                  uint16                    *pwTransactionID,
                  byte                      nItemChosen,                          
                  boolean                   bHelp
                 )
{
  if((!pISTK) || (!pwTransactionID))
    return EFAILED;
  return OEMSTK_SendEnvelopeMenuSelCommand(pISTK->m_pSTK, pwTransactionID, nItemChosen, bHelp);
}

/*=============================================================================
FUNCTION: AEESTK_BrowserTermination

DESCRIPTION:  
  Indicates to ISTK when there is a browser termination.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int AEESTK_BrowserTermination(ISTK *pISTK, AEESTKBrowserTermEnumType sBrowserTerm)
{
  if(!pISTK)
    return EFAILED;
  return OEMSTK_BrowserTermination(pISTK->m_pSTK, sBrowserTerm);
}

/*=============================================================================
FUNCTION: AEESTK_UserActivity

DESCRIPTION:  
  Indicates to ISTK when there is a user activity, e.g., key events

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int AEESTK_UserActivity(ISTK *pISTK)
{
  if(!pISTK)
    return EFAILED;
  return OEMSTK_UserActivity(pISTK->m_pSTK);
}

/*=============================================================================
FUNCTION: AEESTK_IdleScreen

DESCRIPTION:
  Indicates to ISTK when there is a idle screen.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int AEESTK_IdleScreen(ISTK *pISTK)
{
  if(!pISTK)
    return EFAILED;
  return OEMSTK_IdleScreen(pISTK->m_pSTK);
}

/*=============================================================================
FUNCTION: AEESTK_LangSelection

DESCRIPTION:
  Indicates to ISTK when there is a language selection done by the user.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int AEESTK_LangSelection(
    ISTK *pISTK, 
    AEESTKLangSelectionResponse *sLangSelection
)
{
  if(!pISTK)
    return EFAILED;
  return OEMSTK_LangSelection(pISTK->m_pSTK, sLangSelection);
}

/*=============================================================================
FUNCTION: AEESTK_ChannelStatusChange

DESCRIPTION:  
  Indicates to ISTK when there is a channel status change.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int AEESTK_ChannelStatusChange(ISTK *pISTK, AEESTKGetChStatusResponse sChStatus)
{
  if(!pISTK)
    return EFAILED;
  return OEMSTK_ChannelStatusChange(pISTK->m_pSTK, sChStatus);
}

/*=============================================================================
FUNCTION: AEESTK_DataAvailable

DESCRIPTION:  
  Indicates to ISTK when there is data available after a successful receive data

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int AEESTK_DataAvailable(ISTK *pISTK, AEESTKDataAvailNotify sDataAvail)
{
  if(!pISTK)
    return EFAILED;
  return OEMSTK_DataAvailable(pISTK->m_pSTK, sDataAvail);
}
