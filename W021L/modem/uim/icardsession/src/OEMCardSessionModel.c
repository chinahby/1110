/*=============================================================================

FILE: OEMCARDSESSIONMODEL.c

SERVICES: OEM Functions for Card Session Notifier Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2007-2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //source/qcom/qct/modem/uim/icardsession/rel/07H1_2/src/OEMCardSessionModel.c#2 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     --------------------------------------------------------- 
05/13/09     kk      Fixed Compiler Warning
10/01/08     nb      Lint fixes
02/22/08     kk      initialization of output ptr when 
                     FEATURE_ICARDSESSION_IMODEL not defined
01/16/08     nk      Fixed Compiler Warning
01/04/08     nk      Fixed Compiler Error
12/20/07     nk      Fixed Compiler Warning
12/14/07     sk      Fixed warning
12/12/07     sun     Fixed mapping of file enums
12/04/07     sk      Fixed compilation error
12/03/07     sk      Fixed compilation warning
08/24/07     sk      Initial Revision
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "comdef.h"     // Get DMSS type declarations.
#include "err.h"        // Error functions
#include "OEMCardSessionModel.h"
#include "OEMCardSession.h"
#include "AEE.h"
#include "AEEStdLib.h" 
#include "intconv.h"

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI */

/*===========================================================================

                            INTERFACE FUNCTION TABLE

===========================================================================*/

#ifdef FEATURE_MMGSDI
OEMCardSessionModelCardEventTableType OEMCardSessionModelCardEventTable[] = 
{
  {MMGSDI_CARD_INSERTED_EVT,       AEECARDSESSION_MODEL_CARD_INSERTED},
  {MMGSDI_CARD_ERROR_EVT,          AEECARDSESSION_MODEL_CARD_ERROR},
  {MMGSDI_SELECT_AID_EVT,          AEECARDSESSION_MODEL_APP_SELECTED},
  {MMGSDI_CARD_INIT_COMPLETED_EVT, AEECARDSESSION_MODEL_CARD_READY},
  {MMGSDI_REFRESH_EVT,             AEECARDSESSION_MODEL_CARD_REFRESH},
  {MMGSDI_SESSION_CLOSE_EVT,       AEECARDSESSION_MODEL_SESSION_CLOSED} 
};

/*===========================================================================

                            FUNCTION PROTOTYPES

===========================================================================*/
#ifdef FEATURE_ICARDSESSION_IMODEL
static void OEMCardSessionModel_PinEventCB(void *pData);
static void OEMCardSessionModel_CardEventCB(void *pData);
static uint32 OEMCardSessionModel_AddRef (ICardSessionModel *pMe);
#endif /* FEATURE_ICARDSESSION_IMODEL */
#endif /* FEATURE_MMGSDI */

/*=============================================================================
FUNCTION: OEMCardSessionModel_New

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
/*lint -e{715} suppress "pMe, ppNew not referenced" */
/*lint -save -esym(818,pMe,ppNew) suppress could be const */
int OEMCardSessionModel_New(ICardSession *pMe, void **ppNew)
{
#ifdef FEATURE_ICARDSESSION_IMODEL
  AEECardSessionOwner     *pCardSessionOwner = NULL;
  mmgsdi_return_enum_type  mmgsdi_status     = MMGSDI_SUCCESS;

  MSG_HIGH("OEMCardSessionModel_New", 0, 0, 0);

  /* Step 1: Parameter check */
  if (NULL == ppNew) {
    MSG_ERROR("ppNew ptr NULL", 0, 0, 0);
    return EFAILED;
  }
  if (NULL == pMe) {
    MSG_ERROR("pMe ptr NULL", 0, 0, 0);
    return EFAILED;
  }

  /* step 2: check if model object for the specific ICardSession instance
     actually exists */
  *ppNew = NULL;
  if (NULL != pMe->m_modelObj) {
    IModel *pModel = SYSMODELBASE_TO_IMODEL(&pMe->m_modelObj->base);
    (void) OEMCardSessionModel_AddRef(pMe->m_modelObj);
    *ppNew = pModel;
    return SUCCESS;
  }
  /* If the object doesn't already exist, allocate it now. */
  pMe->m_modelObj = (ICardSessionModel *)sys_malloc(sizeof(ICardSessionModel));

  if (NULL == pMe->m_modelObj) {
    MSG_ERROR("NewClassEx failed", 0, 0, 0);
    return ENOMEMORY;
  }

  (void)MEMSET(pMe->m_modelObj, 0, sizeof(ICardSessionModel));

  SysModelBase_Ctor(&pMe->m_modelObj->base, 
                    &pMe->m_modelObj->vtModel,
                    NULL);


  *ppNew = (void*)(SYSMODELBASE_TO_IMODEL(&pMe->m_modelObj->base));

  /* step 4: Register for MMGSDI events */
  mmgsdi_status = mmgsdi_client_evt_reg (pMe->m_client_id,
                                         OEMCardSessionModel_MMGSDIEvtCb,
                                         OEMCardSessionCmdCb,
                                         0);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    MSG_ERROR("Unable to register for Event",0,0,0);
    /* Release the model object ptr */
    SysModelBase_Dtor(&pMe->m_modelObj->base);
    (void) OEMCardSessionModel_Release(pMe->m_modelObj);
    return EFAILED;
  }

  /* Step 5: Add the model object ptr and session information to the linked list */
  pCardSessionOwner = (AEECardSessionOwner *)mem_malloc(&tmc_heap,
                       sizeof(AEECardSessionOwner));

  if (NULL == pCardSessionOwner) {
    MSG_ERROR("Card Session node could not be malloced",0,0,0);
    /* Client Id dereg will release the model obj pointer */
    OEMCardSession_ClientIDDereg(pMe);     
    return EFAILED;
  }
  (void)MEMSET(pCardSessionOwner, 0, sizeof(AEECardSessionOwner));
  pCardSessionOwner->clientID = pMe->m_client_id;
  pCardSessionOwner->data     = (void*)pMe->m_modelObj;
  OEMCARDSESSION_ADD_NODE(pMe->m_coreObj->m_SessionOwner,
                          pCardSessionOwner);
  return SUCCESS;
#else
  *ppNew = NULL;
  MSG_ERROR("FEATURE_ICARDSESSION_IMODEL not supported",0,0,0);
  return EUNSUPPORTED;
#endif /* FEATURE_ICARDSESSION_IMODEL */
}/* OEMCardSessionModel_New */

/* The function needs to be featurized because its use is featurized.  If the 
   feature is not turned on then there is a warning indicating that the 
   function is declared but not used */
#ifdef FEATURE_ICARDSESSION_IMODEL
/*=============================================================================
FUNCTION: OEMCardSessionModel_AddRef

DESCRIPTION:
  Indicates that one or more object is now pointing to ICardSessionModel
  object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
/*lint -e{715} suppress "pMe not referenced" */
/*lint -save -esym(818,pMe) suppress could be const */
static uint32 OEMCardSessionModel_AddRef(ICardSessionModel *pMe)
{
  MSG_HIGH("OEMCardSessionModel_AddRef", 0, 0, 0);
  if (NULL != pMe) {
    return 1;
  }
  return 0;
}/* OEMCardSessionModel_AddRef */
#endif /* FEATURE_ICARDSESSION_IMODEL */

/*=============================================================================
FUNCTION: OEMCardSessionModel_Release

DESCRIPTION:
  Decreases the number of references to the ICardSessionModel object.  If 
  the last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
/*lint -e{715} suppress "pMe not referenced" */
/*lint -save -esym(818,pMe) suppress could be const */
uint32 OEMCardSessionModel_Release(
  ICardSessionModel *pMe)
{
#ifdef FEATURE_ICARDSESSION_IMODEL
  IModel     *pModel = NULL;

  MSG_HIGH("OEMCardSessionModel_Release", 0, 0, 0);

  if (NULL == pMe)
    return (0);
  else {
    pModel = SYSMODELBASE_TO_IMODEL(&pMe->base);
    return(IMODEL_Release(pModel));
  }
#else
  MSG_ERROR("FEATURE_ICARDSESSION_IMODEL not supported",0,0,0);
  return EUNSUPPORTED;
#endif /* FEATURE_ICARDSESSION_IMODEL */
}/* OEMCardSessionModel_Release */

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_ICARDSESSION_IMODEL
/*=============================================================================
FUNCTION: OEMCardSessionModel_AllocEventCBS

DESCRIPTION:
  This function allocate the Event CBs

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
OEMCardSessionEventCBS * OEMCardSessionModel_AllocEventCBS(
  uint32 dwDataSize)
{
  OEMCardSessionEventCBS *pcb = NULL;

  MSG_HIGH("OEMCardSessionModel_AllocEventCBS size 0x%x", dwDataSize, 0, 0);

  if (NULL == ICardSessionCoreObj) {
    MSG_ERROR("ICardSessionCoreObj ptr NULL", 0, 0, 0);
    return NULL;
  }

  pcb = (OEMCardSessionEventCBS *)mem_malloc(&tmc_heap, 
                                             sizeof(OEMCardSessionEventCBS));
  if (NULL == pcb) {
    MSG_ERROR(
      "OEMCardSession Failed to allocate memory for OEMCardSessionEventCBS", 
      0, 0, 0);
    return NULL;
  }

  (void)memset(pcb, 0, sizeof(OEMCardSessionEventCBS));
  
  if (dwDataSize) {
    pcb->pData = mem_malloc(&tmc_heap, dwDataSize);
    if (NULL == pcb->pData) {
      MSG_ERROR(
        "OEMCardSession Failed to allocate memory for pcb->data freeing for pcb = 0x%X",
        pcb, 0, 0);
      OEMCARDSESSION_SYS_FREE(pcb);
      return NULL;
    }
    (void) memset(pcb->pData, 0, dwDataSize);
  }

  pcb->nRef = 1;

  OEMCARDSESSION_ADD_NODE(ICardSessionCoreObj->m_EventCbs, pcb);

  return pcb;
}/* OEMCardSessionModel_AllocEventCBS */

/*=============================================================================
FUNCTION: OEMCardSessionModel_MMGSDIEvtCb

DESCRIPTION:
   This function is the event callback that MMGSDI will call when there is
   event notification
  
DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionModel_MMGSDIEvtCb(
  const mmgsdi_event_data_type *pEvent)
{
  OEMCardSessionEventCBS   *pEventCb       = NULL;
  PFNNOTIFY                 pBrewFn        = NULL;  
  uint32                    dwSize         = 0;
  int32                     nReturnStatus  = SUCCESS;

  MSG_HIGH("OEMCardSessionModel_MMGSDIEvtCb", 0, 0, 0);

  if ( NULL == pEvent ) {
    MSG_ERROR("Null Event Ptr", 0, 0, 0);
    return;
  }
  
  dwSize = sizeof(mmgsdi_event_data_type);
  switch (pEvent->evt)
  {
  case MMGSDI_CARD_INSERTED_EVT:
    pBrewFn = (PFNNOTIFY)OEMCardSessionModel_CardEventCB;
    break;
  case MMGSDI_CARD_ERROR_EVT:
    pBrewFn = (PFNNOTIFY)OEMCardSessionModel_CardEventCB;
    break;
  case MMGSDI_CARD_INIT_COMPLETED_EVT:
    pBrewFn = (PFNNOTIFY)OEMCardSessionModel_CardEventCB;
    break;
  case MMGSDI_SELECT_AID_EVT:
    pBrewFn = (PFNNOTIFY)OEMCardSessionModel_CardEventCB;
    break;
  case MMGSDI_REFRESH_EVT:
    pBrewFn = (PFNNOTIFY)OEMCardSessionModel_CardEventCB;
    break;
  case MMGSDI_PIN1_EVT:
  case MMGSDI_PIN2_EVT:
  case MMGSDI_UNIVERSAL_PIN_EVT:
    pBrewFn = (PFNNOTIFY)OEMCardSessionModel_PinEventCB;
    break;
  case MMGSDI_SESSION_CLOSE_EVT:
    pBrewFn = (PFNNOTIFY)OEMCardSessionModel_CardEventCB;
    break;
  case MMGSDI_CARD_REMOVED_EVT:
  case MMGSDI_TERMINAL_PROFILE_DL_EVT:
  case MMGSDI_FDN_EVT:
  case MMGSDI_SWITCH_SLOT_EVT:
  case MMGSDI_ILLEGAL_CARD_EVT:
  case MMGSDI_SAP_CONNECT_EVT:
  case MMGSDI_SAP_DISCONNECT_EVT:
  case MMGSDI_SAP_PIN_EVT:
  case MMGSDI_MAX_EVT_ENUM:
    MSG_ERROR("Evt Not Handled 0x%x", pEvent->evt, 0, 0);
    return;
  }

  pEventCb = OEMCardSessionModel_AllocEventCBS(dwSize);
  if (NULL == pEventCb)
    return;

  (void)MEMSET(pEventCb->pData, 0x00, dwSize); 
  nReturnStatus = OEMCardSessionNotifier_CopyEventInfo(pEvent, 
                                                       pEventCb->pData);

  if (SUCCESS != nReturnStatus) {
    OEMCardSessionModel_FreeEventCBS(pEventCb);
    return;
  }
  
  CALLBACK_Init(&(pEventCb->cb), pBrewFn, pEventCb);

  AEE_ResumeCallback(&(pEventCb->cb), 0);
}/* OEMCardSessionModel_MMGSDIEvtCb */


/*=============================================================================
FUNCTION: OEMCardSessionModel_CardEventCB

DESCRIPTION:
  This function builds the Card Event and send it to the client

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCardSessionModel_CardEventCB(
  void *pData)
{
  mmgsdi_event_data_type          *pMMGSDIEventData = NULL;
  AEECardSessionCardEventData     *pEventData       = NULL;
  int                              nReturnStatus    = SUCCESS;
  ModelEvent                       *pEvent          = NULL;
  ICardSessionModel                *pModelObj       = NULL;
  IModel                           *pModel          = NULL;   

  MSG_HIGH("OEMCardSessionModel_CardEventCB", 0, 0, 0);

  if (NULL == pData) {
    MSG_ERROR("pData NULL ptr", 0, 0, 0);
    return;
  }

  pMMGSDIEventData = ((OEMCardSessionEventCBS *)pData)->pData;
  
  if (NULL == pMMGSDIEventData) {
    MSG_ERROR("pMMGSDIEventData NULL ptr", 0, 0, 0);
    return;
  }

  if (NULL == (pEventData = MALLOC(sizeof(AEECardSessionCardEventData)))) {
    MSG_ERROR("OEMCardSession Failed to allocate EventData for CardEvent", 
              0, 0, 0);
    OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    return;
  }
  
  nReturnStatus = OEMCardSessionModel_MapFromMMGSDICardEvent(
                    pMMGSDIEventData->evt,
                    &pEventData->dwCardEvent);
             
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("OEMCardSession Failed to map the MMGSDI card evt 0x%x", 
      pMMGSDIEventData->evt, 0, 0);
    OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    OEM_Free(pEventData);
    return;
  }

  if (NULL == (pEvent = MALLOC(sizeof(ModelEvent)))) {
    MSG_ERROR("OEMCardSession Failed to allocate EventData for CardEvent", 
              0, 0, 0);
    OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    OEM_Free(pEventData);
    return;
  }

  pEvent->evCode = pEventData->dwCardEvent;

  MSG_HIGH("AEE CARD EVENT 0x%x", pEventData->dwCardEvent, 0, 0);

  switch(pEventData->dwCardEvent) {
    case AEECARDSESSION_MODEL_CARD_INSERTED:
      nReturnStatus = OEMCardSessionModel_CopyCardInsertedData(
                        &pMMGSDIEventData->data.card_inserted,
                        &pEventData->uEventData.sCardInserted);
      break;

    case AEECARDSESSION_MODEL_APP_SELECTED:
      nReturnStatus = OEMCardSessionModel_CopyAppSelectedData(
                        &pMMGSDIEventData->data.select_aid,
                        &pEventData->uEventData.sAppSelected);
      break;

    case AEECARDSESSION_MODEL_CARD_ERROR:
      nReturnStatus = OEMCardSessionModel_CopyCardErrorData(
                        &pMMGSDIEventData->data.card_error,
                        &pEventData->uEventData.sCardError);
      break;

    case AEECARDSESSION_MODEL_CARD_READY:
      nReturnStatus = OEMCardSessionModel_CopyCardReadyData(
                        &pMMGSDIEventData->data.card_init_completed,
                        &pEventData->uEventData.sCardReady);
      break;

    case AEECARDSESSION_MODEL_CARD_REFRESH:
      nReturnStatus = OEMCardSessionModel_CopyCardRefreshData(
                        &pMMGSDIEventData->data.refresh,
                        &pEventData->uEventData.sCardRefresh);
      break;

    case AEECARDSESSION_MODEL_SESSION_CLOSED:
      break;

    default:
      MSG_ERROR("OEMCardSession Unknown dwCardEvent 0x%x", 
        pEventData->dwCardEvent, 0, 0);
      OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData);
      OEM_Free(pEventData);
      OEM_Free(pEvent);
      return;
  }

  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("OEMCardSession Failed to copy data for Card Evt 0x%x", 
      pEventData->dwCardEvent, 0, 0);
    OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    OEM_Free(pEventData);
    OEM_Free(pEvent);
    return;
  }

  pModelObj = OEMCardSessionModel_GetModelObjFromClientId(pMMGSDIEventData->client_id);
  if (NULL == pModelObj) {
    MSG_ERROR("Null Model Obj", 0, 0, 0);
    OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    OEMCardSessionModel_FreeCardEvent(pEventData);
    FREE(pEventData);
    FREE(pEvent);
    return;
  }

  pEvent->dwParam = (uint32)pEventData;
  pModel = SYSMODELBASE_TO_IMODEL(&pModelObj->base);
  IMODEL_Notify(pModel, pEvent);

  if(pEventData->dwCardEvent == AEECARDSESSION_MODEL_SESSION_CLOSED) {
    /* Remove the model object from the linked list */
    (void) OEMCardSessionModel_RemoveModelObjNode(pMMGSDIEventData->client_id);
  }

  FREE(pEvent);
  OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData);
  OEMCardSessionModel_FreeCardEvent(pEventData);
  FREE(pEventData);
}/* OEMCardSessionModel_CardEventCB */


/*=============================================================================
FUNCTION: OEMCardSessionModel_MapFromMMGSDICardEvent

DESCRIPTION:
  This function Maps the MMGSDI event to the Card Card Event

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCardSessionModel_MapFromMMGSDICardEvent(
  mmgsdi_events_enum_type sEvtEnum,
  uint32                 *pCardEvtEnum )
{
  uint32 i = 0;

  if (NULL == pCardEvtEnum) {
    MSG_ERROR("MMGSDI Evt 0x%x Null CardEvt Ptr", sEvtEnum, 0, 0);
    return EBADPARM;
  }

  for (i=0; i<ARR_SIZE(OEMCardSessionModelCardEventTable); i++) {
    if (sEvtEnum == OEMCardSessionModelCardEventTable[i].sMMGSDIEvtEnum) {
      *pCardEvtEnum = 
        OEMCardSessionModelCardEventTable[i].dwCardSessionEvtEnum;
      MSG_HIGH("Mapped MMGSDI evt 0x%x to AEECardSessionModel evt 0x%x",
               sEvtEnum, *pCardEvtEnum, 0);
      return SUCCESS;
    }
  }
  MSG_ERROR("MMGSDI Evt 0x%x not match for CardEvent", sEvtEnum, 0, 0);
  return EFAILED;
}/* OEMCardSessionModel_MapFromMMGSDICardEvent */


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyCardInsertedData

DESCRIPTION
  Map MMGSDI Card Inserted Data to AEECardSessionModel Card Inserted Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyCardInsertedData(
  const mmgsdi_card_inserted_evt_info_type *pInsertedData,
  AEECardSessionCardInsertedData           *pCardInsertedData) 
{
  int                    nReturnStatus = SUCCESS;
  uint32                 i             = 0;
  uint32                 dwTotalApp    = 0;

  if ((NULL == pCardInsertedData) || (NULL == pInsertedData)){
    MSG_ERROR(" NULL ptr", 0, 0, 0);
    return EBADPARM;
  }

  (void)MEMSET(pCardInsertedData, 
               0x00, 
               sizeof(AEECardSessionCardInsertedData));

  dwTotalApp = pInsertedData->num_aids_avail;

  if (pInsertedData->num_aids_avail > AEECARDSESSION_MAX_APPS) {
    MSG_HIGH("OEMCardSession resets max num Aids exceed AEECardSession limit 0x%x", 
      pInsertedData->num_aids_avail, 0, 0);
    dwTotalApp = AEECARDSESSION_MAX_APPS;
  }

  pCardInsertedData->dwNumApp = dwTotalApp;

  MSG_HIGH("Inserted Evt: Num App 0x%x", pCardInsertedData->dwNumApp, 0, 0);

  for (i = 0; i < dwTotalApp; i++) {
      nReturnStatus = OEMCardSessionModel_CopyFromMMGSDIAidStruct(
      pInsertedData->aid_info[i],
      &pCardInsertedData->aAppIDList[i].dwTech,
      &pCardInsertedData->aAppIDList[i].nAidLen,
      pCardInsertedData->aAppIDList[i].aAid,
      pCardInsertedData->aAppIDList[i].aLabel );                                                     
    if (SUCCESS != nReturnStatus) {
      MSG_ERROR("OEMCardSession Failed to map the MMGSDI card AID struct 0x%x", 
                 pInsertedData->aid_info[i].app_type, 0, 0);
      OEMCardSessionModel_CleanupCardInsertedEvt(pCardInsertedData);
      return nReturnStatus;
    }   
  }
  
  return nReturnStatus;
}


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyFromMMGSDIAidStruct

DESCRIPTION
  Copy MMGSDI Aid Structure to the corresponding data structure in 
  AEECardSession

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyFromMMGSDIAidStruct (
  mmgsdi_aid_type  sAID,
  uint32          *pCardTech,
  int32           *pCardAidLen,
  uint8           *pAid,
  AECHAR          *pLabel )
{
  int nReturnStatus = SUCCESS;

  MSG_HIGH("OEMCardSessionNotifier_CopyFromMMGSDIAidStruct", 0, 0, 0);

  if ((NULL == pCardTech) || (NULL == pCardAidLen) ||
      (NULL == pAid) || (NULL == pLabel)) {
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EFAILED;
  }

  /* App type can be none, no need to check return status */
  (void)OEMCardSessionNotifier_MapFromMMGSDIApps(
    sAID.app_type,
    pCardTech);

  nReturnStatus = OEMCardSessionModel_CopyFromMMGSDIAid(
    sAID,
    pCardAidLen,
    pAid);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }   

  nReturnStatus = OEMCardSessionModel_CopyFromMMGSDILabel(
    sAID,
    pLabel);
  if (SUCCESS != nReturnStatus) {
    MSG_HIGH("OEMCardSessionModel_CopyFromMMGSDILabel() failed 0x%x", nReturnStatus, 0, 0);
  }  
  return SUCCESS;
}/* OEMCardSessionModel_CopyFromMMGSDIAidStruct */


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyFromMMGSDIAid

DESCRIPTION
  Copy MMGSDI Aid to AEECardSession Aid 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyFromMMGSDIAid(
  mmgsdi_aid_type        sAid,
  int32                 *pCardAidLen,
  uint8                 *pCardAid)
{
  MSG_HIGH("OEMCardSessionModel_CopyFromMMGSDIAid", 0, 0, 0);

  if ((NULL == pCardAidLen) || (NULL == pCardAid)) {
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EFAILED;
  }

  if (sAid.aid.data_len > AEECARDSESSION_MAX_APP_ID_LEN) {
    MSG_ERROR("Invalid AID Len 0x%x", sAid.aid.data_len, 0, 0);
    return EFAILED;
  }

  (void)MEMSET(pCardAid, 0x00, AEECARDSESSION_MAX_APP_ID_LEN);
  
  *pCardAidLen = sAid.aid.data_len;
  MSG_HIGH("AID Len 0x%x", *pCardAidLen, 0, 0);
  
  (void)MEMCPY(pCardAid, sAid.aid.data_ptr, 
      int32touint32(MIN(sAid.aid.data_len, AEECARDSESSION_MAX_APP_ID_LEN)));  
  return SUCCESS;
}/* OEMCardSessionModel_CopyFromMMGSDIAid */


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyFromMMGSDILabel

DESCRIPTION
  Copy MMGSDI Label to AEECardSession Label 
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyFromMMGSDILabel(
  mmgsdi_aid_type        sAid,
  AECHAR                *pCardLabel)
{
  int32  dwMaxLabelLen = AEECARDSESSION_MAX_APP_LABEL_LEN * sizeof(AECHAR);

  if (NULL == pCardLabel) {
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EFAILED;
  }

  if (sAid.label.data_len > (AEECARDSESSION_MAX_APP_LABEL_LEN - 1)) {
    MSG_ERROR("Invalid Label Len 0x%x", sAid.label.data_len, 0, 0);
    return EFAILED;
  }

  /* This ensure NULL terminated label */
  (void)MEMSET(pCardLabel, 
               0x00, 
               sizeof(AECHAR)*(AEECARDSESSION_MAX_APP_LABEL_LEN));

  MSG_HIGH("Label Len 0x%x", sAid.label.data_len, 0, 0);

  (void)STRTOWSTR((char*)sAid.label.data_ptr, pCardLabel, 
                  MIN(sAid.label.data_len, dwMaxLabelLen));  
  return SUCCESS;
}/* OEMCardSessionModel_CopyFromMMGSDILabel */


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyAppSelectedData

DESCRIPTION
  Map MMGSDI App Selected Data to AEECardSessionNotifier App Selected Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyAppSelectedData(
  const mmgsdi_select_aid_evt_info_type    *pAppSelectedData,
  AEECardSessionAppSelectedData            *pCardAppSelectedData) 
{
  int nReturnStatus = SUCCESS;

  if (NULL == pAppSelectedData){
    MSG_ERROR("pAppSelectedData NULL ptr ",0,0,0);
    return EBADPARM;
  }

  if (NULL == pCardAppSelectedData){
    MSG_ERROR("pAppSelectedData or pCardAppSelectedData NULL ptr ",0,0,0);
    return EBADPARM;
  }

  pCardAppSelectedData->nChannelID = pAppSelectedData->channel_id;

  MSG_HIGH("Selected App Evt: Channel ID 0x%x", 
           pCardAppSelectedData->nChannelID, 0, 0);

  nReturnStatus = OEMCardSessionModel_CopyFromMMGSDIAidStruct(
      pAppSelectedData->app_data,
      &pCardAppSelectedData->aAppID.dwTech,
      &pCardAppSelectedData->aAppID.nAidLen,
      pCardAppSelectedData->aAppID.aAid,
      pCardAppSelectedData->aAppID.aLabel );
  
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("OEMCardSession Failed to map the MMGSDI app aid struct 0x%x", 
      pAppSelectedData->app_data.app_type, 0, 0);
    OEMCardSessionModel_CleanupAppSelectedEvt(pCardAppSelectedData);
    return nReturnStatus;
  }
  return nReturnStatus;
}/* OEMCardSessionModel_CopyAppSelectedData */


/*=============================================================================
FUNCTION: OEMCardSessionModel_CleanupAppSelectedEvt

DESCRIPTION:
  This function clean up the Selected App event data:
  it will set the information to initial values for the members and free
  data pointer as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionModel_CleanupAppSelectedEvt(
  AEECardSessionAppSelectedData   *pCardAppSelectedData)
{
  MSG_HIGH("OEMCardSessionNotifier_CleanupAppSelectedEvt", 0, 0, 0);

  if (NULL == pCardAppSelectedData) {
    MSG_ERROR("pCardAppSelectedData ptr NULL", 0, 0, 0);
    return;
  }

  (void)MEMSET(&pCardAppSelectedData->aAppID, 
               0x00, 
               sizeof(AEECardSessionAppData));
  pCardAppSelectedData->nChannelID       = 0;
}


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyCardErrorData

DESCRIPTION
  Map MMGSDI Card Error Data to AEECardSessionModel Card Error Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyCardErrorData(
  const mmgsdi_card_err_evt_info_type *pErrorData,
  const AEECardSessionCardErrorData   *pCardErrorData) 
{
  /* pCardErrorData const until more data is needed to be populated */
  int nReturnStatus = SUCCESS;

  if ((NULL == pErrorData) || (NULL == pCardErrorData)){
    MSG_ERROR("NULL ptr ", 0, 0, 0);
    return EBADPARM;
  }
  return nReturnStatus;
}/* OEMCardSessionModel_CopyCardErrorData */


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyCardReadyData

DESCRIPTION
  Map MMGSDI Card Ready Data to AEECardSessionModel Card Ready Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyCardReadyData(
  const mmgsdi_card_init_completed_evt_info_type *pReadyData,
  AEECardSessionCardReadyData                    *pCardReadyData) 
{
  uint32 i             = 0;
  int    nReturnStatus = SUCCESS;
  uint32 dwTotalApp    = 0;

  if ((NULL == pReadyData) || (NULL == pCardReadyData)){
    MSG_ERROR("NULL ptr ", 0, 0, 0);
    return EBADPARM;
  }

  dwTotalApp = pReadyData->num_avail_apps;

  if (pReadyData->num_avail_apps > AEECARDSESSION_MAX_APPS) {
    MSG_HIGH(
      "OEMCardSession resets max num Aids exceed AEECardSession limit 0x%x", 
      pReadyData->num_avail_apps, 0, 0);
    dwTotalApp = AEECARDSESSION_MAX_APPS;
  }

  pCardReadyData->dwNumApp = dwTotalApp;

  MSG_HIGH("Ready Evt: Num App 0x%x", dwTotalApp, 0, 0);
  
  for (i = 0; i < dwTotalApp; i++) {
    nReturnStatus = OEMCardSessionModel_CopyFromMMGSDIAidStruct(
      pReadyData->app_info[i].app_data,
      &pCardReadyData->aAppInfoList[i].dwTech,
      &pCardReadyData->aAppInfoList[i].nAidLen,
      pCardReadyData->aAppInfoList[i].aAid,
      pCardReadyData->aAppInfoList[i].aLabel );

    if (SUCCESS != nReturnStatus) {
      MSG_ERROR("OEMCardSession Failed to map the MMGSDI Card AID 0x%x ", 
        nReturnStatus, 0, 0);
      OEMCardSessionModel_CleanupCardReadyEvt(pCardReadyData);
      return nReturnStatus;
    } 

    /* PIN1 */
    MSG_HIGH("Ready Evt: PIN1", 0, 0, 0);

    nReturnStatus = OEMCardSession_MapFromMMGSDIPinStatus(
      pReadyData->app_info[i].pin1,
      &pCardReadyData->aAppInfoList[i].dwPin1NumRetries,
      &pCardReadyData->aAppInfoList[i].dwPuk1NumRetries,
      &pCardReadyData->aAppInfoList[i].dwPin1Status);
    if (SUCCESS != nReturnStatus) {
      MSG_ERROR("OEMCardSession Failed to map the MMGSDI pin1 status 0x%x ", 
                 pReadyData->app_info[i].pin1.status, 0, 0);
      OEMCardSessionModel_CleanupCardReadyEvt(pCardReadyData);
      return nReturnStatus;
    }  

    /* PIN2 */
    MSG_HIGH("Ready Evt: PIN2", 0, 0, 0);

    nReturnStatus = OEMCardSession_MapFromMMGSDIPinStatus(
      pReadyData->app_info[i].pin2,
      &pCardReadyData->aAppInfoList[i].dwPin2NumRetries,
      &pCardReadyData->aAppInfoList[i].dwPuk2NumRetries,
      &pCardReadyData->aAppInfoList[i].dwPin2Status);
    if (SUCCESS != nReturnStatus) {
      MSG_ERROR(
        "OEMCardSession Failed to map the MMGSDI pin2 status 0x%x ", 
        pReadyData->app_info[i].pin2.status, 0, 0);
      OEMCardSessionModel_CleanupCardReadyEvt(pCardReadyData);
      return nReturnStatus;
    }  
  
    /* Universal PIN */
    MSG_HIGH("Ready Evt: Univ PIN", 0, 0, 0);

    nReturnStatus = OEMCardSession_MapFromMMGSDIPinStatus(
      pReadyData->app_info[i].universal_pin,
      &pCardReadyData->aAppInfoList[i].dwPinUniversalNumRetries,
      &pCardReadyData->aAppInfoList[i].dwPukUniversalNumRetries,
      &pCardReadyData->aAppInfoList[i].dwPinUniversalStatus);
    if (SUCCESS != nReturnStatus) {
      MSG_ERROR(
        "OEMCardSession Failed to map the MMGSDI pin universal status 0x%x ", 
        pReadyData->app_info[i].universal_pin.status, 0, 0);
      OEMCardSessionModel_CleanupCardReadyEvt(pCardReadyData);
      return nReturnStatus;
    }  
  }
  return nReturnStatus;
}


/*=============================================================================
FUNCTION: OEMCardSessionModel_CleanupCardReadyEvt

DESCRIPTION:
  This function clean up the Card Ready event data:
  it will set the information to initial values for the members and free
  data pointer as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionModel_CleanupCardReadyEvt(
  AEECardSessionCardReadyData   *pCardReadyData)
{
  int i = 0;
   
  MSG_HIGH("OEMCardSessionModel_CleanupCardReadyEvt", 0, 0, 0);
  
  if (NULL == pCardReadyData) {
    MSG_ERROR("NULL ptr ", 0, 0, 0);
    return;
  }

  for (i = 0; i < AEECARDSESSION_MAX_APPS; i++) {
    (void)MEMSET(&pCardReadyData->aAppInfoList[i], 
                 0x00, 
                 sizeof(AEECardSessionAppInfo));
  }
  pCardReadyData->dwNumApp  = 0;
}/* OEMCardSessionModel_CleanupCardReadyEvt */


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyCardRefreshData

DESCRIPTION
  Map MMGSDI Refresh Data to AEECardSessionNotifier Card Refresh Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyCardRefreshData(
  const mmgsdi_refresh_evt_info_type *pRefreshData,
  AEECardSessionCardRefreshData      *pCardRefreshData) 
{
  int nReturnStatus = SUCCESS;

  MSG_HIGH("OEMCardSessionModel_CopyCardRefreshData", 0, 0, 0);

  if ((NULL == pRefreshData) || (NULL == pCardRefreshData)){
    MSG_ERROR("Null Input Parm: mmgsdi_p 0x%x, card_p 0x%x", 
      pRefreshData, pCardRefreshData, 0);
    return EBADPARM;
  }

  nReturnStatus = OEMCardSessionModel_CopyFromMMGSDIRefreshMode(
    pRefreshData->mode,
    &pCardRefreshData->dwRefreshMode);
  
  if (SUCCESS != nReturnStatus) {
    OEMCardSessionModel_CleanupCardRefreshEvt(pCardRefreshData);
    return nReturnStatus;
  } 

  nReturnStatus = OEMCardSessionModel_CopyFromMMGSDIRefreshStage(
    pRefreshData->stage,
    &pCardRefreshData->dwRefreshStage);
  
  if (SUCCESS != nReturnStatus) {
    OEMCardSessionModel_CleanupCardRefreshEvt(pCardRefreshData);
    return nReturnStatus;
  } 

  nReturnStatus = OEMCardSessionModel_CopyFromMMGSDIAidStruct(
      pRefreshData->aid,
      &pCardRefreshData->aAppID.dwTech,
      &pCardRefreshData->aAppID.nAidLen,
      pCardRefreshData->aAppID.aAid,
      pCardRefreshData->aAppID.aLabel );

  if (SUCCESS != nReturnStatus) {
    OEMCardSessionModel_CleanupCardRefreshEvt(pCardRefreshData);
    return nReturnStatus;
  } 

  nReturnStatus =  OEMCardSessionModel_CopyFromMMGSDIRefreshFileList(
    pRefreshData->refresh_files,
    &pCardRefreshData->dwFileList);

  if (SUCCESS != nReturnStatus) {
    OEMCardSessionModel_CleanupCardRefreshEvt(pCardRefreshData);
    return nReturnStatus;
  } 
  return nReturnStatus;
}/* OEMCardSessionModel_CopyCardRefreshData */


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyFromMMGSDIRefreshMode

DESCRIPTION
  Copy MMGSDI Refresh Mode to the corresponding AEECardSessionNotifier
  Refresh Mode

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyFromMMGSDIRefreshMode (
  mmgsdi_refresh_mode_enum_type  sMMGSDIRefreshMode,
  uint32                        *pCardRefreshMode )
{
  MSG_HIGH("OEMCardSessionModel_CopyFromMMGSDIRefreshMode", 0, 0, 0);

  if (NULL == pCardRefreshMode){
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EFAILED;
  }

  switch(sMMGSDIRefreshMode)
  {
    case MMGSDI_REFRESH_RESET:
      *pCardRefreshMode = AEECARDSESSION_MODEL_REFRESH_RESET;
      break;
    case MMGSDI_REFRESH_NAA_INIT:
      *pCardRefreshMode = AEECARDSESSION_MODEL_REFRESH_NAA_INIT;
      break;
    case MMGSDI_REFRESH_NAA_INIT_FCN:
      *pCardRefreshMode = AEECARDSESSION_MODEL_REFRESH_NAA_INIT_FCN;
      break;
    case MMGSDI_REFRESH_NAA_FCN:
      *pCardRefreshMode = AEECARDSESSION_MODEL_REFRESH_FCN;
      break;
    case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
      *pCardRefreshMode = AEECARDSESSION_MODEL_REFRESH_NAA_INIT_FULL_FCN;
      break;
    case MMGSDI_REFRESH_NAA_APP_RESET:
      *pCardRefreshMode = AEECARDSESSION_MODEL_REFRESH_APP_RESET;
      break;
    case MMGSDI_REFRESH_3G_SESSION_RESET:
      *pCardRefreshMode = AEECARDSESSION_MODEL_REFRESH_3G_SESSION_RESET;
      break;
    default:
      MSG_ERROR("Unhandled Refresh Mode: 0x%x", sMMGSDIRefreshMode, 0, 0);
      return EFAILED;
  }

  MSG_HIGH("Refresh mode MMGSDI: 0x%x, Card: 0x%x", 
    sMMGSDIRefreshMode, *pCardRefreshMode, 0);

  return SUCCESS;
}/* OEMCardSessionModel_CopyFromMMGSDIRefreshMode */


/*=============================================================================
FUNCTION: OEMCardSessionModel_CleanupCardRefreshEvt

DESCRIPTION:
  This function clean up the Card Refresh event data:
  it will set the information to initial values for the members and free
  data pointer as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionModel_CleanupCardRefreshEvt(
  AEECardSessionCardRefreshData   *pCardRefreshData)
{
  MSG_HIGH("OEMCardSessionModel_CleanupCardRefreshEvt", 0, 0, 0);
  
  if (NULL == pCardRefreshData){
    MSG_ERROR("NULL ptr ", 0, 0, 0);
    return;
  }

  (void)MEMSET(&pCardRefreshData->aAppID, 
         0x00, 
         sizeof(AEECardSessionAppData));
  (void)MEMSET(&pCardRefreshData->dwFileList,
               0x00,
               sizeof(AEECardSessionRefreshData));

  pCardRefreshData->dwRefreshMode = 0;
  pCardRefreshData->dwRefreshStage = 0;
}/* OEMCardSessionModel_CleanupCardRefreshEvt */


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyFromMMGSDIRefreshFileList

DESCRIPTION
  Copy MMGSDI Refresh File List to the corresponding AEECardSessionModel
  Refresh File list

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyFromMMGSDIRefreshFileList
(
  mmgsdi_refresh_file_list_type sMMGSDIFileList,
  AEECardSessionRefreshData    *pCardRefreshFileList
)
{
  uint32 i = 0;

  MSG_HIGH("OEMCardSessionModel_CopyFromMMGSDIRefreshFileList", 0, 0, 0);

  if (NULL == pCardRefreshFileList){
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EFAILED;
  }

  pCardRefreshFileList->nNumOfFiles = sMMGSDIFileList.num_files;
  if(pCardRefreshFileList->nNumOfFiles > 0)
  {
    pCardRefreshFileList->pFileList = 
      MALLOC(sizeof(uint32)*pCardRefreshFileList->nNumOfFiles);
    if(pCardRefreshFileList->pFileList == NULL)
      return EFAILED;

    for(i=0;i<pCardRefreshFileList->nNumOfFiles;i++)
    {
      if( SUCCESS != OEMCardSession_MapFromMMGSDIFileEnum(sMMGSDIFileList.file_list_ptr[i],
                                           &pCardRefreshFileList->pFileList[i]))
      {
        MSG_ERROR("Unable to match mmgsdi File 0x%x",
                   sMMGSDIFileList.file_list_ptr[i], 0, 0);
      }
    }
  }
  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyFromMMGSDIRefreshStage

DESCRIPTION
  Copy MMGSDI Refresh Stage to the corresponding AEECardSessionNotifier
  Refresh Stage

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyFromMMGSDIRefreshStage (
  mmgsdi_refresh_stage_enum_type  sMMGSDIRefreshStage,
  uint32                         *pCardRefreshStage )
{
  MSG_HIGH("OEMCardSessionModel_CopyFromMMGSDIRefreshStage", 0, 0, 0);

  if (NULL == pCardRefreshStage){
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EFAILED;
  }
    
  switch(sMMGSDIRefreshStage)
  {
    case MMGSDI_REFRESH_STAGE_START:
      *pCardRefreshStage = AEECARDSESSION_MODEL_REFRESH_STAGE_START;
      break;
    case MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED:
      *pCardRefreshStage = AEECARDSESSION_MODEL_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED;
      break;
    case MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED:
      *pCardRefreshStage = AEECARDSESSION_MODEL_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED;
      break;
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT:
      *pCardRefreshStage = AEECARDSESSION_MODEL_REFRESH_STAGE_OK_TO_INIT;
      break;
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN:
      *pCardRefreshStage = AEECARDSESSION_MODEL_REFRESH_STAGE_OK_TO_FCN;
      break;
    case MMGSDI_REFRESH_STAGE_END_SUCCESS:
      *pCardRefreshStage = AEECARDSESSION_MODEL_REFRESH_STAGE_END_SUCCESS;
      break;
    case MMGSDI_REFRESH_STAGE_END_FAILED:
      *pCardRefreshStage = AEECARDSESSION_MODEL_REFRESH_STAGE_END_FAILED;
      break;
    case MMGSDI_REFRESH_STAGE_ENUM_MAX:
      MSG_ERROR("Invalid Refresh stage: MMGSDI_REFRESH_STAGE_ENUM_MAX", 0, 0, 0);
      return EFAILED;
  }
  MSG_HIGH("Refresh Stage MMGSDI: 0x%x, Card: 0x%x", 
    sMMGSDIRefreshStage, *pCardRefreshStage, 0);
  return SUCCESS;
}/* OEMCardSessionModel_CopyFromMMGSDIRefreshStage */


/*=============================================================================
FUNCTION: OEMCardSessionModel_FreeEventCBS

DESCRIPTION:
  This function free the Event CBs

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionModel_FreeEventCBS(
  OEMCardSessionEventCBS *pcb)
{

  MSG_HIGH("OEMCardSessionNotifier_FreeEventCBS", 0, 0, 0);

  if (NULL == pcb) {
    MSG_ERROR("NULL ptr ", 0, 0, 0);
    return;
  }

  if (pcb->nRef <= 0) {
    MSG_ERROR("pcb->nRef < =0 ", 0, 0, 0);
    return;
  }

  if (--pcb->nRef > 0) {
    MSG_ERROR("--pcb->nRef > 0 ", 0, 0, 0);
    return;
  }

  CALLBACK_Cancel(&pcb->cb);

  if (ICardSessionCoreObj) 
    OEMCARDSESSION_REMOVE_NODE(ICardSessionCoreObj->m_EventCbs, pcb);

  if (pcb->pData)
  {
    OEMCARDSESSION_SYS_FREE(pcb->pData);
  }

  OEMCARDSESSION_SYS_FREE(pcb);
}/* OEMCardSessionModel_FreeEventCBS */


/*=============================================================================
FUNCTION: OEMCardSessionModel_FreeCardEvent

DESCRIPTION:
  This function free Card Event Data as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionModel_FreeCardEvent(
  AEECardSessionCardEventData *pCardEvent)
{
  MSG_HIGH("OEMCardSessionModel_FreeCardEvent", 0, 0, 0);

  if (NULL == pCardEvent) {
    MSG_ERROR("pCardEvent NULL ptr ", 0, 0, 0);
    return;
  }
  
  switch(pCardEvent->dwCardEvent) {
    case AEECARDSESSION_MODEL_CARD_INSERTED:
      OEMCardSessionModel_CleanupCardInsertedEvt(
        &pCardEvent->uEventData.sCardInserted);
      break;

    case AEECARDSESSION_MODEL_APP_SELECTED:
      OEMCardSessionModel_CleanupAppSelectedEvt(
        &pCardEvent->uEventData.sAppSelected);
      break;

    case AEECARDSESSION_MODEL_CARD_ERROR:
      OEMCardSessionModel_CleanupCardErrorEvt(
        &pCardEvent->uEventData.sCardError);
      break;

    case AEECARDSESSION_MODEL_CARD_READY:
      OEMCardSessionModel_CleanupCardReadyEvt(
        &pCardEvent->uEventData.sCardReady);
      break;

    case AEECARDSESSION_MODEL_CARD_REFRESH:
      OEMCardSessionModel_CleanupCardRefreshEvt(
        &pCardEvent->uEventData.sCardRefresh);
      break;

    default:
      MSG_ERROR("OEMCardSession Unknown dwCardEvent 0x%x", 
        pCardEvent->dwCardEvent, 0, 0);
      break;
  }
}/* OEMCardSessionModel_FreeCardEvent */


/*=============================================================================
FUNCTION: OEMCardSessionModel_CleanupCardInsertedEvt

DESCRIPTION:
  This function clean up the Card Inserted event data:
  it will set the information to initial values for the members and free
  data pointer as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionModel_CleanupCardInsertedEvt(
  AEECardSessionCardInsertedData   *pCardInsertedData)
{
  int i = 0;

  MSG_HIGH("OEMCardSessionModel_CleanupCardInsertedEvt", 0, 0, 0);

  if (NULL == pCardInsertedData){
    MSG_ERROR("pCardInsertedData NULL ptr ", 0, 0, 0);
    return;
  }

  for (i = 0; i < AEECARDSESSION_MAX_APPS; i++) {
    (void)MEMSET(&pCardInsertedData->aAppIDList[i], 
                 0x00, 
                 sizeof(AEECardSessionAppData));
  }
  pCardInsertedData->dwNumApp  = 0;
}/* OEMCardSessionModel_CleanupCardInsertedEvt */


/*=============================================================================
FUNCTION: OEMCardSessionModel_CleanupCardErrorEvt

DESCRIPTION:
  This function clean up the Card Error event data:
  it will set the information to initial values for the members and free
  data pointer as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionModel_CleanupCardErrorEvt(
  AEECardSessionCardErrorData   *pCardErrorData)
{
  int i = 0;

  MSG_HIGH("OEMCardSessionModel_CleanupCardErrorEvt", 0, 0, 0);

  if (NULL == pCardErrorData){
    MSG_ERROR("pCardErrorData NULL ptr ", 0, 0, 0);
    return;
  }

  for (i = 0; i < AEECARDSESSION_MAX_APPS; i++) {
    for (i = 0; i < AEECARDSESSION_MAX_APPS; i++) {
      (void)MEMSET(&pCardErrorData->aAppIDList[i], 
                   0x00, 
                   sizeof(AEECardSessionAppData));
    }
  }
  pCardErrorData->dwNumApp  = 0;
}/* OEMCardSessionModel_CleanupCardErrorEvt */


/*=============================================================================
FUNCTION: OEMCardSessionModel_RemoveModelObjNode

DESCRIPTION:
  This function returns the model object associated with a specfic client id 
  from the list.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCardSessionModel_RemoveModelObjNode(uint64 client_id)
{
  AEECardSessionOwner     *pCardSessionOwnerCurr = NULL;
  AEECardSessionOwner     *pCardSessionOwnerPrev = NULL;

  pCardSessionOwnerCurr = ICardSessionCoreObj->m_SessionOwner;

  while (pCardSessionOwnerCurr) 
  {
    pCardSessionOwnerPrev = pCardSessionOwnerCurr;
    pCardSessionOwnerCurr = pCardSessionOwnerCurr->pNext;

    if (pCardSessionOwnerPrev->clientID == client_id) 
    {
      OEMCARDSESSION_REMOVE_NODE(ICardSessionCoreObj->m_SessionOwner,
                                 pCardSessionOwnerPrev);
      return SUCCESS;
    }
  }/* while (pCardSessionOwnerCurr) */
  MSG_ERROR("OEMCardSessionModel_RemoveModelObjNode() failed",0,0,0);
  return EFAILED;
}


/*=============================================================================
FUNCTION: OEMCardSessionModel_GetModelObjFromClientId

DESCRIPTION:
  This function returns the model object associated with a specfic client id 
  from the list.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
ICardSessionModel *  OEMCardSessionModel_GetModelObjFromClientId(uint64 client_id)
{
  AEECardSessionOwner     *pCardSessionOwnerCurr = NULL;
  AEECardSessionOwner     *pCardSessionOwnerPrev = NULL;

  pCardSessionOwnerCurr = ICardSessionCoreObj->m_SessionOwner;

  while (pCardSessionOwnerCurr) 
  {
    pCardSessionOwnerPrev = pCardSessionOwnerCurr;
    pCardSessionOwnerCurr = pCardSessionOwnerCurr->pNext;

    if (pCardSessionOwnerPrev->clientID == client_id) 
    {
      MSG_HIGH("Client ID 0x%x", client_id, 0, 0);
      return((ICardSessionModel *)pCardSessionOwnerPrev->data);
    }
  }/* while (pCardSessionOwnerCurr) */
  return (NULL);
}


/*===========================================================================
FUNCTION: OEMCardSessionModel_MapFromMMGSDIPinEvent

DESCRIPTION
  Map MMGSDI Pin Status to AEECardSession Pin Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_MapFromMMGSDIPinEvent(
  mmgsdi_pin_status_enum_type sPinStatusEnum,
  uint32                     *pCardPinEventEnum) 
{
  MSG_HIGH("OEMCardSessionModel_MapFromMMGSDIPinEvent MMGSDI: 0x%x",
           sPinStatusEnum, 0, 0);

  if (NULL == pCardPinEventEnum) {
    MSG_ERROR("pCardPinEventEnum NULL ptr ", 0, 0, 0);
    return EBADPARM;
  }

  *pCardPinEventEnum = AEECARDSESSION_PIN_NOT_INITIALIZED;

  switch (sPinStatusEnum)
  {
    case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
      *pCardPinEventEnum = AEECARDSESSION_MODEL_PIN_ENABLED_NOT_VERIFIED;
      break;
    case MMGSDI_PIN_ENABLED_VERIFIED:
      *pCardPinEventEnum = AEECARDSESSION_MODEL_PIN_ENABLED_VERIFIED;
      break;
    case MMGSDI_PIN_CHANGED:
      *pCardPinEventEnum = AEECARDSESSION_MODEL_PIN_CHANGED;
      break;
    case MMGSDI_PIN_UNBLOCKED:
      *pCardPinEventEnum = AEECARDSESSION_MODEL_PIN_UNBLOCKED;
      break;
    case MMGSDI_PIN_DISABLED:
      *pCardPinEventEnum = AEECARDSESSION_MODEL_PIN_DISABLED;
      break;
    case MMGSDI_PIN_BLOCKED:
      *pCardPinEventEnum = AEECARDSESSION_MODEL_PIN_BLOCKED;
      break;
    case MMGSDI_PIN_PERM_BLOCKED:
      *pCardPinEventEnum = AEECARDSESSION_MODEL_PIN_PERM_BLOCKED;
      break;
  case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
      MSG_ERROR("MMGSDI_PIN_STATUS_NOT_INITIALIZED", 0, 0, 0);
      return EFAILED;
  }
  
  MSG_HIGH("Card Pin Evt 0x%x", *pCardPinEventEnum, 0, 0);

  return SUCCESS;
}

/*===========================================================================
FUNCTION: OEMCardSessionModel_MapFromMMGSDIPinReplacement

DESCRIPTION
  Map MMGSDI Pin replacement to AEECardSession Pin Replacement enumeration

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_MapFromMMGSDIPinReplacement(
  mmgsdi_pin_replace_enum_type     sPinReplacementEnum,
  uint32                          *pCardPinReplacementEnum) 
{
  MSG_HIGH("OEMCardSessionModel_MapFromMMGSDIPinReplacement MMGSDI: 0x%x",
           sPinReplacementEnum, 0, 0);

  if (NULL == pCardPinReplacementEnum) {
    MSG_ERROR("Null pCardPinReplacementEnum ptr", 0, 0, 0);
    return EBADPARM;
  }

  *pCardPinReplacementEnum = AEECARDSESSION_PIN_NOT_INITIALIZED;

  switch (sPinReplacementEnum)
  {
    case MMGSDI_PIN_REPLACED_BY_UNIVERSAL:
      *pCardPinReplacementEnum = AEECARDSESSION_MODEL_PIN_REPLACED_BY_UNIVERSAL;
      break;
    case MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL:
      *pCardPinReplacementEnum = AEECARDSESSION_MODEL_PIN_NOT_REPLACED_BY_UNIVERSAL;
      break;
    default:
      MSG_ERROR("Unknown MMGSDI PIN Replacement 0x%x", sPinReplacementEnum, 0, 0);
      return EFAILED;
  }
  
  MSG_HIGH("Card Pin Replacement 0x%x", *pCardPinReplacementEnum, 0, 0);

  return SUCCESS;
}


/*=============================================================================
FUNCTION: OEMCardSessionModel_PinEventCB

DESCRIPTION:
  This function builds the Card Event and send it to the client

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCardSessionModel_PinEventCB(
  void *pData)
{
  mmgsdi_event_data_type          *pMMGSDIEventData = NULL;
  AEECardSessionPinEventData      *pEventData       = NULL;
  int                              nReturnStatus    = SUCCESS;
  uint32                           i                = 0;
  IModel                           *pModel          = NULL;
  ICardSessionModel                *pModelObj       = NULL;
  ModelEvent                       *pEvent           = NULL;

  MSG_HIGH("OEMCardSessionModel_PinEventCB", 0, 0, 0);

  if (NULL == pData){
    MSG_ERROR("pData NULL ptr ", 0, 0, 0);
    return;
  }

  pMMGSDIEventData = ((OEMCardSessionEventCBS *)pData)->pData;
  
  if (NULL == pMMGSDIEventData){
    MSG_ERROR("pMMGSDIEventData NULL ptr ", 0, 0, 0);
    return;
  }

  if (NULL == (pEventData = MALLOC(sizeof(AEECardSessionPinEventData)))) {
    MSG_ERROR("OEMCardSession Failed to allocate EventData for PinEvent",
              0, 0, 0);
    OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData); 
    return;
  }

  if (NULL == (pEvent = MALLOC(sizeof(ModelEvent)))) {
    MSG_ERROR("OEMCardSession Failed to allocate EventData for CardEvent", 
              0, 0, 0);
    OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    OEM_Free(pEventData);
    return;
  }
  
  nReturnStatus = OEMCardSession_MapFromMMGSDIPinID(
    pMMGSDIEventData->data.pin.pin_info.pin_id,
    &pEventData->dwPinID);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("OEMCardSession Failed to map the MMGSDI pin id 0x%x ", 
      pMMGSDIEventData->data.pin.pin_info.pin_id, 0, 0);
    OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData); 
    OEM_Free(pEventData);
    OEM_Free(pEvent);
    return;
  }

  nReturnStatus = OEMCardSessionModel_MapFromMMGSDIPinEvent(
                    pMMGSDIEventData->data.pin.pin_info.status,
                    &pEventData->dwPinEvent);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("OEMCardSession Failed to map the MMGSDI pin status 0x%x ", 
      pMMGSDIEventData->data.pin.pin_info.status, 0, 0);
    OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData); 
    OEM_Free(pEventData);
    OEM_Free(pEvent);
    return;
  }

  pEventData->dwNumPinRetries = 
    pMMGSDIEventData->data.pin.pin_info.num_retries;
  pEventData->dwNumPukRetries = 
    pMMGSDIEventData->data.pin.pin_info.num_unblock_retries;
  
  MSG_HIGH("PIN Event: PIN Num Retries 0x%x, Unblock Num Retries, 0x%x",
           pEventData->dwNumPinRetries, pEventData->dwNumPukRetries, 0);

  nReturnStatus = OEMCardSessionModel_MapFromMMGSDIPinReplacement(
                    pMMGSDIEventData->data.pin.pin_info.pin_replacement,
                    &pEventData->dwPinReplacement);
  if (SUCCESS != nReturnStatus) {
    OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData); 
    OEM_Free(pEventData);
    OEM_Free(pEvent);
    return;
  }

  /* App Data */
  pEventData->dwNumApp = pMMGSDIEventData->data.pin.num_aids;
  for (i = 0; i < pEventData->dwNumApp; i++) {
    nReturnStatus = OEMCardSessionModel_CopyFromMMGSDIAidStruct(
                      pMMGSDIEventData->data.pin.aid_type[i],
                      &pEventData->aAppIDList[i].dwTech,
                      &pEventData->aAppIDList[i].nAidLen,
                      pEventData->aAppIDList[i].aAid,
                      pEventData->aAppIDList[i].aLabel);
    if (SUCCESS != nReturnStatus) {
      MSG_ERROR("OEMCardSession Failed to copy the MMGSDI aid struct 0x%x ", 
                 nReturnStatus, 0, 0);
      OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData); 
      OEM_Free(pEventData);
      OEM_Free(pEvent);
      return;
    }    
  }

  pModelObj = OEMCardSessionModel_GetModelObjFromClientId(pMMGSDIEventData->client_id);
  if (NULL == pModelObj) {
    MSG_ERROR("Null Model Obj for Cls", 0, 0, 0);
    OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    OEM_Free(pEventData);
    OEM_Free(pEvent);
    return;
  }
  pEvent->dwParam = (uint32)pEventData;
  pModel = SYSMODELBASE_TO_IMODEL(&pModelObj->base);
  IMODEL_Notify(pModel, pEvent);
  OEM_Free(pEvent);
  OEMCardSessionModel_FreeEventCBS((OEMCardSessionEventCBS*)pData);
  OEM_Free(pEventData);
}
#endif /* FEATURE_ICARDSESSION_IMODEL */
#endif /* FEATURE_MMGSDI */
