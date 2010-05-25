/*=============================================================================

FILE: OEMCARDSESSIONNOTIFIER.c

SERVICES: OEM Functions for Card Session Notifier Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //depot/asic/qsc60x0/apps/StaticExtensions/oem/Src/OEMCardSessionNotifier.c#2 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
10/30/07     jk       Featurized file under FEATURE_ICARDSESSION so that it 
                      can be included in min file even in older baselines without
                      MMGSDI Architecture
10/02/07     vs      Featurized header decleration for "SysModelBase_priv.h"
10/25/06     tml     Added initial support for Universal PIN replacement 
                     support and lint
08/25/06     tml     Added Refresh Event Notification support 
07/24/06     tml     Initial Revision
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_ICARDSESSION
#include "OEMCardSessionNotifier.h"
#include "OEMCardSession.h"

#ifdef FEATURE_ICARDSESSION_IMODEL
#include "SysModelBase_priv.h"
#endif /* FEATURE_ICARDSESSION_IMODEL */

#include "AEEModel.h"
#ifdef WIN32
#undef FEATURE_MMGSDI
#ifndef MSG_ERROR
#define MSG_ERROR DBGPRINTF
#endif
#ifndef MSG_HIGH
#define MSG_HIGH DBGPRINTF
#endif
#endif
#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#include "intconv.h"
#endif

#include "AEECARDSESSION_NOTIFIER.BID"
#include "AEECARDSESSION_NOTIFIER_SLOT2.BID"

/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

static ICardSessionNotifier *ICardSessionNotifierObj_Slot1  = NULL;
static ICardSessionNotifier *ICardSessionNotifierObj_Slot2  = NULL;

/*===========================================================================

                            INTERFACE FUNCTION TABLE

===========================================================================*/
/* The functions supported by CardSession Notifier object. */
static const AEEVTBL(ICardSessionNotifier) gOEMCardSessionNotifierFuncs = {
   OEMCardSessionNotifier_AddRef,
   OEMCardSessionNotifier_Release,
   OEMCardSessionNotifier_SetMask
};


#ifdef FEATURE_MMGSDI
OEMCardSessionNotifierCardEventTableType OEMCardSessionNotifierCardEventTable[] = 
{
  {MMGSDI_CARD_INSERTED_EVT,       AEECARDSESSION_NOTIFIER_CARD_INSERTED},
  {MMGSDI_CARD_ERROR_EVT,          AEECARDSESSION_NOTIFIER_CARD_ERROR},
  {MMGSDI_SELECT_AID_EVT,          AEECARDSESSION_NOTIFIER_APP_SELECTED},
  {MMGSDI_CARD_INIT_COMPLETED_EVT, AEECARDSESSION_NOTIFIER_CARD_READY},
  {MMGSDI_REFRESH_EVT,             AEECARDSESSION_NOTIFIER_CARD_REFRESH}
};
#endif /* FEATURE_MMGSDI */
/*===========================================================================

                           EVENT HANDLERS

===========================================================================*/


/*===========================================================================

                  ICARDSESSIONNOTIFIER FUNCTION DEFINITIONS

===========================================================================*/
/*=============================================================================
FUNCTION: OEMCardSessionNotifier_New

DESCRIPTION:
  Allocates a new ICardSessionNotifier object and initializes it or 
  adds reference to an existing object.
  Allocates a Notifier object if it was not already present.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCardSessionNotifier_New(
  IShell *pIShell, 
  AEECLSID cls, 
  void **ppif)
{
  ICardSessionNotifier   *pNew         = NULL;
  ICardSessionNotifier  **pNotifierObj = NULL;
  OEMCardSessionSlotEnum  u8Slot       = OEMCARDSESSION_SLOT_NONE;    
#ifdef AEE_SIMULATOR
  /* Do not create an object is this is being run on the SDK. */
  *ppif = NULL;
  return EUNSUPPORTED;
#endif

  PACONTEXT pacLast;

  /* Check parameters. */
  if ((NULL == pIShell) || (NULL == ppif)) {
    MSG_ERROR("Null Input Param 0x%x, 0x%x", pIShell, ppif, 0);
    return EBADPARM;
  }

  switch (cls)
  {
    case AEECLSID_CARDSESSION_NOTIFIER:
      pNotifierObj = &ICardSessionNotifierObj_Slot1;
      u8Slot       = OEMCARDSESSION_SLOT1;
      break;
    case AEECLSID_CARDSESSION_NOTIFIER_SLOT2:
      pNotifierObj = &ICardSessionNotifierObj_Slot2;
      u8Slot       = OEMCARDSESSION_SLOT2;
      break;
    default:
      MSG_ERROR("Bad cls 0x%x", cls, 0, 0);
      return EUNSUPPORTED;
  }
  
  /* If the object doesn't already exist, allocate it now. */
  if (NULL == *pNotifierObj) {
    /* Allocate the object. */

    pNew = (ICardSessionNotifier*)(void*)(AEE_NewClassEx(
            (IBaseVtbl*)&gOEMCardSessionNotifierFuncs,
            sizeof(ICardSessionNotifier), TRUE));

    if (NULL == pNew) {
      MSG_ERROR("NewClassEx failed", 0, 0, 0);
      return ENOMEMORY;
    }

    /* Initialize the values. */
    pNew->m_pIShell  = pIShell;
    pNew->m_cls      = cls;
    pNew->m_uRefs    = 1;
    pNew->m_dwNotify = 0;
    pNew->m_u8Slot   = u8Slot;

    /* Create core object */
    pNew->m_coreObj = OEMCardSessionCore_New();

    if (NULL == pNew->m_coreObj) {
      MSG_ERROR("Null CoreObj", 0, 0, 0);
      (void) OEMCardSessionNotifier_Release(pNew);
      return EFAILED;
    }

    /* Exit callback needs to be registered in the system callback */
    pacLast = AEE_EnterAppContext(NULL);
    // tingmui check to see if cls is really being used as input param in FreeOnAEEEXit!!!!!!
    CALLBACK_Init(&pNew->m_cbExit, OEMCardSessionNotifier_FreeOnAEEExit, (void*)cls);
    ISHELL_OnExit(pNew->m_pIShell, &pNew->m_cbExit);
    AEE_LeaveAppContext(pacLast);

    *pNotifierObj = pNew;
  }
  else {
    (void) OEMCardSessionNotifier_AddRef(*pNotifierObj);
  }

  *ppif = *pNotifierObj;

  MSG_HIGH("OEMCardSessionNotifier_New 0x%x", *ppif, 0, 0);

  return AEE_SUCCESS;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_AddRef

DESCRIPTION:
  Indicates that one or more object is now pointing to ICardSessionNotifier 
  object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
uint32 OEMCardSessionNotifier_AddRef(
  ICardSessionNotifier *pMe)
{
  MSG_HIGH("OEMCardSessionNotifier_AddRef", 0, 0, 0);
  if (NULL != pMe) {
    return (++pMe->m_uRefs);
  }
  return 0;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_Release

DESCRIPTION:
  Decreases the number of references to the ICardSessionNotifier object.  If 
  the last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
uint32 OEMCardSessionNotifier_Release(
  ICardSessionNotifier *pMe)
{
  MSG_HIGH("OEMCardSessionNotifier_Release", 0, 0, 0);
  if (NULL == pMe)
    return (0);

  if (pMe->m_uRefs == 0)
    return 0;

  if (--pMe->m_uRefs)
    return pMe->m_uRefs;

  /* Ref count is zero.  Release memory */
  if (pMe->m_coreObj) {
    (void) OEMCardSessionCore_Release();
    pMe->m_coreObj = NULL;
  }

  CALLBACK_Cancel(&pMe->m_cbExit);

  FREE(pMe);

  switch(pMe->m_u8Slot)
  {
    case OEMCARDSESSION_SLOT1:
      ICardSessionNotifierObj_Slot1 = NULL;
      break;
    case OEMCARDSESSION_SLOT2:
      ICardSessionNotifierObj_Slot2 = NULL;
      break;
    default:
      MSG_ERROR("Unknown Slot for Notifier 0x%x", pMe->m_u8Slot, 0, 0);
      break;
  }
  return (0);
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_SetMask

DESCRIPTION:
  This function is called by Brew when an application does an
  ISHELL_RegisterNotify for the ICardSessionNotifier object.  We merge
  all the masks for all the registrations and save a bitmap of
  the sum of all the events being requested.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionNotifier_SetMask(
  ICardSessionNotifier *pMe, 
#if MIN_BREW_BUILD(3,1,4,7)
/*lint -save -esym(818,pdwMask) suppress could be const */
  uint32               *pdwMask)
#else
  const uint32         *pdwMask)
#endif /* MIN_BREW_BUILD(3,1,4,7) */
{
  MSG_HIGH("OEMCardSessionNotifier_SetMask", 0, 0, 0);
  if ((NULL == pMe) || (NULL == pdwMask))
    return;

  pMe->m_dwNotify = OEMCardSessionNotifier_MergeMasks(pdwMask);
}

#if MIN_BREW_BUILD(3,1,4,7)
/*lint -save +esym(818,pdwMask) suppress could be const */
#endif /* MIN_BREW_BUILD(3,1,4,7) */


/*---------------------------------------------------------------------------
                           Callback Functions
--------------------------------------------------------------------------- */
#ifdef FEATURE_MMGSDI
/*=============================================================================
FUNCTION: OEMCardSessionNotifier_CardEventCB

DESCRIPTION:
  This function builds the Card Event and send it to the client

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCardSessionNotifier_CardEventCB(
  void *pData)
{
  mmgsdi_event_data_type          *pMMGSDIEventData = NULL;
  AEECardSessionCardEventData     *pEventData       = NULL;
  int                              nReturnStatus    = SUCCESS;
  mmgsdi_slot_id_enum_type         sMMGSDISlot      = MMGSDI_MAX_SLOT_ID_ENUM;
  AEECLSID                         sClsID;
  ICardSessionNotifier            *pNotifierObj     = NULL;

  MSG_HIGH("OEMCardSessionNotifier_CardEventCB", 0, 0, 0);

  if (NULL == pData)
    return;

  pMMGSDIEventData = ((OEMCardSessionEventCBS *)pData)->pData;
  
  if (NULL == pMMGSDIEventData)
    return;

  if (NULL == (pEventData = MALLOC(sizeof(AEECardSessionCardEventData)))) {
    MSG_ERROR("OEMCardSession Failed to allocate EventData for CardEvent", 
              0, 0, 0);
    OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    return;
  }
  
  nReturnStatus = OEMCardSessionNotifier_MapFromMMGSDICardEvent(
                    pMMGSDIEventData->evt,
                    &pEventData->dwCardEvent);
             
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("OEMCardSession Failed to map the MMGSDI card evt 0x%x", 
      pMMGSDIEventData->evt, 0, 0);
    OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    OEM_Free(pEventData);
    return;
  }

  switch(pEventData->dwCardEvent) {
    case AEECARDSESSION_NOTIFIER_CARD_INSERTED:
      MSG_HIGH("AEECARDSESSION_NOTIFIER_CARD_INSERTED", 0, 0, 0);
      sMMGSDISlot = pMMGSDIEventData->data.card_inserted.slot;
      nReturnStatus = OEMCardSessionNotifier_CopyCardInsertedData(
                        &pMMGSDIEventData->data.card_inserted,
                        &pEventData->uEventData.sCardInserted);
      break;

    case AEECARDSESSION_NOTIFIER_APP_SELECTED:
      MSG_HIGH("AEECARDSESSION_NOTIFIER_APP_SELECTED", 0, 0, 0);
      sMMGSDISlot = pMMGSDIEventData->data.select_aid.slot;
      nReturnStatus = OEMCardSessionNotifier_CopyAppSelectedData(
                        &pMMGSDIEventData->data.select_aid,
                        &pEventData->uEventData.sAppSelected);
      break;

    case AEECARDSESSION_NOTIFIER_CARD_ERROR:
      MSG_HIGH("AEECARDSESSION_NOTIFIER_CARD_ERROR", 0, 0, 0);
      sMMGSDISlot = pMMGSDIEventData->data.card_error.slot;
      nReturnStatus = OEMCardSessionNotifier_CopyCardErrorData(
                        &pMMGSDIEventData->data.card_error,
                        &pEventData->uEventData.sCardError);
      break;

    case AEECARDSESSION_NOTIFIER_CARD_READY:
      MSG_HIGH("AEECARDSESSION_NOTIFIER_CARD_READY", 0, 0, 0);
      sMMGSDISlot = pMMGSDIEventData->data.card_init_completed.slot;
      nReturnStatus = OEMCardSessionNotifier_CopyCardReadyData(
                        &pMMGSDIEventData->data.card_init_completed,
                        &pEventData->uEventData.sCardReady);
      break;

    case AEECARDSESSION_NOTIFIER_CARD_REFRESH:
      MSG_HIGH("AEECARDSESSION_NOTIFIER_CARD_REFRESH", 0, 0, 0);
      sMMGSDISlot = pMMGSDIEventData->data.refresh.slot;
      nReturnStatus = OEMCardSessionNotifier_CopyCardRefreshData(
                        &pMMGSDIEventData->data.refresh,
                        &pEventData->uEventData.sCardRefresh);
      break;

    default:
      MSG_ERROR("OEMCardSession Unknown dwCardEvent 0x%x", 
        pEventData->dwCardEvent, 0, 0);
      OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData);
      OEM_Free(pEventData);
      return;
  }

  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("OEMCardSession Failed to copy data for Card Evt 0x%x", 
      pEventData->dwCardEvent, 0, 0);
    OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    OEM_Free(pEventData);
    return;
  }

  pNotifierObj = OEMCardSessionNotifier_MapMMGSDISlotToNotifyObj(sMMGSDISlot, 
                                                                 &sClsID);
  if (NULL == pNotifierObj) {
    MSG_ERROR("Null Notifier Obj for Cls 0x%x", sClsID, 0, 0);
    OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    OEMCardSessionNotifier_FreeCardEvent(pEventData);
    OEM_Free(pEventData);
    return;
  }

  OEMCardSessionNotifier_NotifyApps(NMASK_CARDSESSION_CARD_EVENT, 
                                   (void*)pEventData,
                                   pNotifierObj,
                                   sClsID);

  OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData);
  OEMCardSessionNotifier_FreeCardEvent(pEventData);
  OEM_Free(pEventData);
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_PinEventCB

DESCRIPTION:
  This function builds the Card Event and send it to the client

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCardSessionNotifier_PinEventCB(
  void *pData)
{
  mmgsdi_event_data_type          *pMMGSDIEventData = NULL;
  AEECardSessionPinEventData      *pEventData       = NULL;
  int                              nReturnStatus    = SUCCESS;
  uint32                           i                = 0;
  AEECLSID                         sClsID;  
  ICardSessionNotifier            *pNotifierObj     = NULL;

  MSG_HIGH("OEMCardSessionNotifier_PinEventCB", 0, 0, 0);

  if (NULL == pData)
    return;

  pMMGSDIEventData = ((OEMCardSessionEventCBS *)pData)->pData;
  
  if (NULL == pMMGSDIEventData)
    return;

  pNotifierObj = OEMCardSessionNotifier_MapMMGSDISlotToNotifyObj(
                    pMMGSDIEventData->data.pin.slot, &sClsID);
  if (NULL == pNotifierObj) {
    MSG_ERROR("Null Notifier Obj for Cls 0x%x", sClsID, 0, 0);
    OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData);
    return;
  }

  if (NULL == (pEventData = MALLOC(sizeof(AEECardSessionPinEventData)))) {
    MSG_ERROR("OEMCardSession Failed to allocate EventData for PinEvent",
              0, 0, 0);
    OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData); 
    return;
  }
  
  nReturnStatus = OEMCardSession_MapFromMMGSDIPinID(
    pMMGSDIEventData->data.pin.pin_info.pin_id,
    &pEventData->dwPinID);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("OEMCardSession Failed to map the MMGSDI pin id 0x%x ", 
      pMMGSDIEventData->data.pin.pin_info.pin_id, 0, 0);
    OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData); 
    OEM_Free(pEventData);
    return;
  }

  nReturnStatus = OEMCardSessionNotifier_MapFromMMGSDIPinEvent(
                    pMMGSDIEventData->data.pin.pin_info.status,
                    &pEventData->dwPinEvent);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("OEMCardSession Failed to map the MMGSDI pin status 0x%x ", 
      pMMGSDIEventData->data.pin.pin_info.status, 0, 0);
    OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData); 
    OEM_Free(pEventData);
    return;
  }

  pEventData->dwNumPinRetries = 
    pMMGSDIEventData->data.pin.pin_info.num_retries;
  pEventData->dwNumPukRetries = 
    pMMGSDIEventData->data.pin.pin_info.num_unblock_retries;
  
  MSG_HIGH("PIN Event: PIN Num Retries 0x%x, Unblock Num Retries, 0x%x",
           pEventData->dwNumPinRetries, pEventData->dwNumPukRetries, 0);

  nReturnStatus = OEMCardSessionNotifier_MapFromMMGSDIPinReplacement(
                    pMMGSDIEventData->data.pin.pin_info.pin_replacement,
                    &pEventData->dwPinReplacement);
  if (SUCCESS != nReturnStatus) {
    OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData); 
    OEM_Free(pEventData);
    return;
  }

  /* App Data */
  pEventData->dwNumApp = pMMGSDIEventData->data.pin.num_aids;
  for (i = 0; i < pEventData->dwNumApp; i++) {
    nReturnStatus = OEMCardSessionNotifier_CopyFromMMGSDIAidStruct(
                      pMMGSDIEventData->data.pin.aid_type[i],
                      &pEventData->aAppIDList[i].dwTech,
                      &pEventData->aAppIDList[i].nAidLen,
                      pEventData->aAppIDList[i].aAid,
                      pEventData->aAppIDList[i].aLabel);
    if (SUCCESS != nReturnStatus) {
      MSG_ERROR("OEMCardSession Failed to copy the MMGSDI aid struct 0x%x ", 
                 nReturnStatus, 0, 0);
      OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData); 
      OEM_Free(pEventData);
      return;
    }    
  }
  
  OEMCardSessionNotifier_NotifyApps(NMASK_CARDSESSION_PIN_EVENT, 
                                   (void*)pEventData,
                                   pNotifierObj,
                                   sClsID);

  OEMCardSessionNotifier_FreeEventCBS((OEMCardSessionEventCBS*)pData);
  OEM_Free(pEventData);
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_MMGSDIEvtCb

DESCRIPTION:
   This function is the event callback that MMGSDI will call when there is
   event notification
  
DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionNotifier_MMGSDIEvtCb(
  const mmgsdi_event_data_type *pEvent)
{
  OEMCardSessionEventCBS   *pEventCb       = NULL;
  PFNNOTIFY                 pBrewFn        = NULL;  
  uint32                    dwSize         = 0;
  int32                     nReturnStatus  = SUCCESS;
  ICardSessionNotifier     *pNotifierObj   = NULL;
  AEECLSID                  sClsID;
  mmgsdi_slot_id_enum_type  sMMGSDISlot    = MMGSDI_MAX_SLOT_ID_ENUM;

  MSG_HIGH("OEMCardSessionNotifier_MMGSDIEvtCb", 0, 0, 0);

  if ( NULL == pEvent ) {
    MSG_ERROR("Null Event Ptr", 0, 0, 0);
    return;
  }
  
  dwSize = sizeof(pEvent->evt);
  switch (pEvent->evt)
  {
  case MMGSDI_CARD_INSERTED_EVT:
    sMMGSDISlot = pEvent->data.card_inserted.slot;
    dwSize += sizeof(mmgsdi_card_inserted_evt_info_type);
    pBrewFn = (PFNNOTIFY)OEMCardSessionNotifier_CardEventCB;
    break;
  case MMGSDI_CARD_ERROR_EVT:
    sMMGSDISlot = pEvent->data.card_error.slot;
    dwSize += sizeof(mmgsdi_card_err_evt_info_type);
    pBrewFn = (PFNNOTIFY)OEMCardSessionNotifier_CardEventCB;
    break;
  case MMGSDI_CARD_INIT_COMPLETED_EVT:
    sMMGSDISlot = pEvent->data.card_init_completed.slot;
    dwSize += sizeof(mmgsdi_card_init_completed_evt_info_type);
    pBrewFn = (PFNNOTIFY)OEMCardSessionNotifier_CardEventCB;
    break;
  case MMGSDI_SELECT_AID_EVT:
    sMMGSDISlot = pEvent->data.select_aid.slot;
    dwSize += sizeof(mmgsdi_select_aid_evt_info_type);
    pBrewFn = (PFNNOTIFY)OEMCardSessionNotifier_CardEventCB;
    break;
  case MMGSDI_REFRESH_EVT:
    sMMGSDISlot = pEvent->data.refresh.slot;
    dwSize += sizeof(mmgsdi_refresh_evt_info_type);
    pBrewFn = (PFNNOTIFY)OEMCardSessionNotifier_CardEventCB;
    break;
  case MMGSDI_PIN1_EVT:
  case MMGSDI_PIN2_EVT:
  case MMGSDI_UNIVERSAL_PIN_EVT:
    sMMGSDISlot = pEvent->data.pin.slot;
    dwSize += sizeof(mmgsdi_pin_evt_info_type);
    pBrewFn = (PFNNOTIFY)OEMCardSessionNotifier_PinEventCB;
    break;
  case MMGSDI_CARD_REMOVED_EVT:
  case MMGSDI_TERMINAL_PROFILE_DL_EVT:
  case MMGSDI_FDN_EVT:
  case MMGSDI_SWITCH_SLOT_EVT:
  case MMGSDI_ILLEGAL_CARD_EVT:
  case MMGSDI_SAP_CONNECT_EVT:
  case MMGSDI_SAP_DISCONNECT_EVT:
  case MMGSDI_SAP_PIN_EVT:
  case MMGSDI_SESSION_CLOSE_EVT:
  case MMGSDI_MAX_EVT_ENUM:
    MSG_ERROR("Evt Not Handled 0x%x", pEvent->evt, 0, 0);
    return;
  }

  pNotifierObj = OEMCardSessionNotifier_MapMMGSDISlotToNotifyObj(sMMGSDISlot, 
                                                                 &sClsID);
  if (NULL == pNotifierObj) {
    MSG_ERROR("Null Core Obj Ptr, No one to notify", 0, 0, 0);
    return;
  }

  pEventCb = OEMCardSessionNotifier_AllocEventCBS(dwSize);
  if (NULL == pEventCb)
    return;

  (void)MEMSET(pEventCb->pData, 0x00, dwSize); 
  nReturnStatus = OEMCardSessionNotifier_CopyEventInfo(pEvent, 
                                                       pEventCb->pData);

  if (SUCCESS != nReturnStatus) {
    OEMCardSessionNotifier_FreeEventCBS(pEventCb);
    return;
  }
  
  CALLBACK_Init(&(pEventCb->cb), pBrewFn, pEventCb);

  AEE_ResumeCallback(&(pEventCb->cb), 0);

  return;
}
#endif /* FEATURE_MMGSDI */

  
/*===========================================================================

                    ICardSessionNotifier Utility functions

===========================================================================*/
/*===========================================================================

FUNCTION: OEMCardSessionNotifier_MergeMasks

DESCRIPTION
  When someone asks to be notified of an event.  This function merges
  all the different requests into one variable to see if anyone is
  interested in the masks for the events the ICardSession Interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 OEMCardSessionNotifier_MergeMasks(
  const uint32 *pdwMasks)
{
  uint32 dw,dwm;

  MSG_HIGH("OEMCardSessionNotifier_MergeMasks", 0, 0, 0);
  dwm=0;
  while ((dw = *pdwMasks) != 0)
  {
    dwm |= GET_NOTIFIER_MASK(dw);
    pdwMasks++;
  }
  return dwm;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_FreeOnAEEExit

DESCRIPTION:
  This function is called in response to BREW exiting. Hopefully the notifier
  object reference is zero by now and somebody has already freed it.  Just in
  case that has not happened, we do it here.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionNotifier_FreeOnAEEExit(
  void *pUser)
{
  AEECLSID              sClsID       = (AEECLSID)pUser;
  ICardSessionNotifier *pNotifierObj = NULL;

  MSG_HIGH("OEMCardSessionNotifier_FreeOnAEEExit 0x%x", pUser, 0, 0);
  
  switch (sClsID)
  {
    case AEECLSID_CARDSESSION_NOTIFIER:
      pNotifierObj = ICardSessionNotifierObj_Slot1;
      break;
    case AEECLSID_CARDSESSION_NOTIFIER_SLOT2:
      pNotifierObj = ICardSessionNotifierObj_Slot2;
      break;
    default:
      MSG_ERROR("Bad cls 0x%x", sClsID, 0, 0);
      return;
  }
  if (pNotifierObj) {
    if (pNotifierObj->m_coreObj) {
      OEMCardSessionCore_FreeOnAEEExit(pNotifierObj->m_coreObj);
      pNotifierObj->m_coreObj = NULL;
    }

    FREE(pNotifierObj);
    pNotifierObj = NULL;
  }
}


#ifdef FEATURE_MMGSDI
/*=============================================================================
FUNCTION: OEMCardSessionNotifier_NotifyApps

DESCRIPTION:
  All events are notified to the application through this function.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionNotifier_NotifyApps(
  uint32                      dwMask, 
  void                       *pNotifyData,
  const ICardSessionNotifier *pNotifierObj,
  AEECLSID                    sClsID)
{
  if (NULL == pNotifyData)
    return;

  if (NULL != pNotifierObj) {
    MSG_HIGH("Sending Mask 0x%X to App", dwMask, 0, 0);
    (void) ISHELL_Notify(pNotifierObj->m_pIShell, sClsID, dwMask, pNotifyData);
  }

  return;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_MapFromMMGSDICardEvent

DESCRIPTION:
  This function Maps the MMGSDI event to the Card Card Event

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCardSessionNotifier_MapFromMMGSDICardEvent(
  mmgsdi_events_enum_type sEvtEnum,
  uint32                 *pCardEvtEnum )
{
  uint32 i = 0;

  if (NULL == pCardEvtEnum) {
    MSG_ERROR("MMGSDI Evt 0x%x Null CardEvt Ptr", sEvtEnum, 0, 0);
    return EBADPARM;
  }

  for (i=0; i<ARR_SIZE(OEMCardSessionNotifierCardEventTable); i++) {
    if (sEvtEnum == OEMCardSessionNotifierCardEventTable[i].sMMGSDIEvtEnum) {
      *pCardEvtEnum = 
        OEMCardSessionNotifierCardEventTable[i].dwCardSessionEvtEnum;
      MSG_HIGH("Mapped MMGSDI evt 0x%x to AEECardSessionNotifier evt 0x%x",
               sEvtEnum, *pCardEvtEnum, 0);
      return SUCCESS;
    }
  }
  
  MSG_ERROR("MMGSDI Evt 0x%x not match for CardEvent", sEvtEnum, 0, 0);
  return EFAILED;
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_CopyCardInsertedData

DESCRIPTION
  Map MMGSDI Card Inserted Data to AEECardSessionNotifier Card Inserted Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_CopyCardInsertedData(
  const mmgsdi_card_inserted_evt_info_type *pInsertedData,
  AEECardSessionCardInsertedData           *pCardInsertedData) 
{
  int                    nReturnStatus = SUCCESS;
  uint32                 i             = 0;
  uint32                 dwTotalApp    = 0;

  if ((NULL == pCardInsertedData) || (NULL == pInsertedData)){
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
    nReturnStatus = OEMCardSessionNotifier_CopyFromMMGSDIAidStruct(
      pInsertedData->aid_info[i],
      &pCardInsertedData->aAppIDList[i].dwTech,
      &pCardInsertedData->aAppIDList[i].nAidLen,
      pCardInsertedData->aAppIDList[i].aAid,
      pCardInsertedData->aAppIDList[i].aLabel );                                                     
    if (SUCCESS != nReturnStatus) {
      MSG_ERROR("OEMCardSession Failed to map the MMGSDI card AID struct 0x%x", 
                 pInsertedData->aid_info[i].app_type, 0, 0);
      OEMCardSessionNotifier_CleanupCardInsertedEvt(pCardInsertedData);
      return nReturnStatus;
    }   
  }
  
  return nReturnStatus;
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_CopyAppSelectedData

DESCRIPTION
  Map MMGSDI App Selected Data to AEECardSessionNotifier App Selected Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_CopyAppSelectedData(
  const mmgsdi_select_aid_evt_info_type    *pAppSelectedData,
  AEECardSessionAppSelectedData            *pCardAppSelectedData) 
{
  int nReturnStatus = SUCCESS;

  if ((NULL == pAppSelectedData) || (NULL == pCardAppSelectedData)){
    return EBADPARM;
  }

  pCardAppSelectedData->nChannelID = pAppSelectedData->channel_id;

  MSG_HIGH("Selected App Evt: Channel ID 0x%x", 
           pCardAppSelectedData->nChannelID, 0, 0);

  nReturnStatus = OEMCardSessionNotifier_CopyFromMMGSDIAidStruct(
      pAppSelectedData->app_data,
      &pCardAppSelectedData->aAppID.dwTech,
      &pCardAppSelectedData->aAppID.nAidLen,
      pCardAppSelectedData->aAppID.aAid,
      pCardAppSelectedData->aAppID.aLabel );
  
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("OEMCardSession Failed to map the MMGSDI app aid struct 0x%x", 
      pAppSelectedData->app_data.app_type, 0, 0);
    OEMCardSessionNotifier_CleanupAppSelectedEvt(pCardAppSelectedData);
    return nReturnStatus;
  }
  
  return nReturnStatus;
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_CopyCardErrorData

DESCRIPTION
  Map MMGSDI Card Error Data to AEECardSessionNotifier Card Error Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_CopyCardErrorData(
  const mmgsdi_card_err_evt_info_type *pErrorData,
  const AEECardSessionCardErrorData   *pCardErrorData) 
{
  /* pCardErrorData const until more data is needed to be populated */
  int nReturnStatus = SUCCESS;

  if ((NULL == pErrorData) || (NULL == pCardErrorData)){
    return EBADPARM;
  }

  return nReturnStatus;
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_CopyCardReadyData

DESCRIPTION
  Map MMGSDI Card Ready Data to AEECardSessionNotifier Card Ready Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_CopyCardReadyData(
  const mmgsdi_card_init_completed_evt_info_type *pReadyData,
  AEECardSessionCardReadyData                    *pCardReadyData) 
{
  uint32 i             = 0;
  int    nReturnStatus = SUCCESS;
  uint32 dwTotalApp    = 0;

  if ((NULL == pReadyData) || (NULL == pCardReadyData)){
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
    nReturnStatus = OEMCardSessionNotifier_CopyFromMMGSDIAidStruct(
      pReadyData->app_info[i].app_data,
      &pCardReadyData->aAppInfoList[i].dwTech,
      &pCardReadyData->aAppInfoList[i].nAidLen,
      pCardReadyData->aAppInfoList[i].aAid,
      pCardReadyData->aAppInfoList[i].aLabel );

    if (SUCCESS != nReturnStatus) {
      MSG_ERROR("OEMCardSession Failed to map the MMGSDI Card AID 0x%x ", 
        nReturnStatus, 0, 0);
      OEMCardSessionNotifier_CleanupCardReadyEvt(pCardReadyData);
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
      OEMCardSessionNotifier_CleanupCardReadyEvt(pCardReadyData);
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
      OEMCardSessionNotifier_CleanupCardReadyEvt(pCardReadyData);
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
      OEMCardSessionNotifier_CleanupCardReadyEvt(pCardReadyData);
      return nReturnStatus;
    }  
  }
  return nReturnStatus;
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_CopyCardRefreshData

DESCRIPTION
  Map MMGSDI Refresh Data to AEECardSessionNotifier Card Refresh Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_CopyCardRefreshData(
  const mmgsdi_refresh_evt_info_type *pRefreshData,
  AEECardSessionCardRefreshData      *pCardRefreshData) 
{
  int nReturnStatus = SUCCESS;

  MSG_HIGH("OEMCardSessionNotifier_CopyCardRefreshData", 0, 0, 0);

  if ((NULL == pRefreshData) || (NULL == pCardRefreshData)){
    MSG_ERROR("Null Input Parm: mmgsdi_p 0x%x, card_p 0x%x", 
      pRefreshData, pCardRefreshData, 0);
    return EBADPARM;
  }

  nReturnStatus = OEMCardSessionNotifier_CopyFromMMGSDIRefreshMode(
    pRefreshData->mode,
    &pCardRefreshData->dwRefreshMode);
  
  if (SUCCESS != nReturnStatus) {
    OEMCardSessionNotifier_CleanupCardRefreshEvt(pCardRefreshData);
    return nReturnStatus;
  } 

  nReturnStatus = OEMCardSessionNotifier_CopyFromMMGSDIRefreshStage(
    pRefreshData->stage,
    &pCardRefreshData->dwRefreshStage);
  
  if (SUCCESS != nReturnStatus) {
    OEMCardSessionNotifier_CleanupCardRefreshEvt(pCardRefreshData);
    return nReturnStatus;
  } 

  if (pCardRefreshData->dwRefreshMode == AEECARDSESSION_NOTIFIER_REFRESH_RESET ) {
    /* No need to copy AID for Reset */
    return nReturnStatus;
  }
  nReturnStatus = OEMCardSessionNotifier_CopyFromMMGSDIAidStruct(
      pRefreshData->aid,
      &pCardRefreshData->aAppID.dwTech,
      &pCardRefreshData->aAppID.nAidLen,
      pCardRefreshData->aAppID.aAid,
      pCardRefreshData->aAppID.aLabel );

  if (SUCCESS != nReturnStatus) {
    OEMCardSessionNotifier_CleanupCardRefreshEvt(pCardRefreshData);
    return nReturnStatus;
  } 

  return nReturnStatus;
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_MapFromMMGSDIPinEvent

DESCRIPTION
  Map MMGSDI Pin Status to AEECardSession Pin Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_MapFromMMGSDIPinEvent(
  mmgsdi_pin_status_enum_type sPinStatusEnum,
  uint32                     *pCardPinEventEnum) 
{
  MSG_HIGH("OEMCardSessionNotifier_MapFromMMGSDIPinEvent MMGSDI: 0x%x",
           sPinStatusEnum, 0, 0);

  if (NULL == pCardPinEventEnum) {
    return EBADPARM;
  }

  *pCardPinEventEnum = AEECARDSESSION_PIN_NOT_INITIALIZED;

  switch (sPinStatusEnum)
  {
    case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
      *pCardPinEventEnum = AEECARDSESSION_NOTIFIER_PIN_ENABLED_NOT_VERIFIED;
      break;
    case MMGSDI_PIN_ENABLED_VERIFIED:
      *pCardPinEventEnum = AEECARDSESSION_NOTIFIER_PIN_ENABLED_VERIFIED;
      break;
    case MMGSDI_PIN_CHANGED:
      *pCardPinEventEnum = AEECARDSESSION_NOTIFIER_PIN_CHANGED;
      break;
    case MMGSDI_PIN_UNBLOCKED:
      *pCardPinEventEnum = AEECARDSESSION_NOTIFIER_PIN_UNBLOCKED;
      break;
    case MMGSDI_PIN_DISABLED:
      *pCardPinEventEnum = AEECARDSESSION_NOTIFIER_PIN_DISABLED;
      break;
    case MMGSDI_PIN_BLOCKED:
      *pCardPinEventEnum = AEECARDSESSION_NOTIFIER_PIN_BLOCKED;
      break;
    case MMGSDI_PIN_PERM_BLOCKED:
      *pCardPinEventEnum = AEECARDSESSION_NOTIFIER_PIN_PERM_BLOCKED;
      break;
    case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
      return EFAILED;
  }
  
  MSG_HIGH("Card Pin Evt 0x%x", *pCardPinEventEnum, 0, 0);

  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_MapFromMMGSDIPinReplacement

DESCRIPTION
  Map MMGSDI Pin replacement to AEECardSession Pin Replacement enumeration

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_MapFromMMGSDIPinReplacement(
  mmgsdi_pin_replace_enum_type     sPinReplacementEnum,
  uint32                          *pCardPinReplacementEnum) 
{
  MSG_HIGH("OEMCardSessionNotifier_MapFromMMGSDIPinReplacement MMGSDI: 0x%x",
           sPinReplacementEnum, 0, 0);

  if (NULL == pCardPinReplacementEnum) {
    MSG_ERROR("Null pCardPinReplacementEnum ptr", 0, 0, 0);
    return EBADPARM;
  }

  *pCardPinReplacementEnum = AEECARDSESSION_PIN_NOT_INITIALIZED;

  switch (sPinReplacementEnum)
  {
    case MMGSDI_PIN_REPLACE_NOT_APPLICABLE:
      *pCardPinReplacementEnum = AEECARDSESSION_NOTIFIER_PIN_REPLACE_NOT_APPLICABLE;
      break;
    case MMGSDI_PIN_REPLACED_BY_UNIVERSAL:
      *pCardPinReplacementEnum = AEECARDSESSION_NOTIFIER_PIN_REPLACED_BY_UNIVERSAL;
      break;
    case MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL:
      *pCardPinReplacementEnum = AEECARDSESSION_NOTIFIER_PIN_NOT_REPLACED_BY_UNIVERSAL;
      break;
    default:
      MSG_ERROR("Unknown MMGSDI PIN Replacement 0x%x", sPinReplacementEnum, 0, 0);
      return EFAILED;
  }
  
  MSG_HIGH("Card Pin Replacement 0x%x", *pCardPinReplacementEnum, 0, 0);

  return SUCCESS;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_CopyEventInfo

DESCRIPTION:
   This function copies the event info
  
DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCardSessionNotifier_CopyEventInfo(
  const mmgsdi_event_data_type *pInEvent,
  mmgsdi_event_data_type       *pOutEvent)
{
  MSG_HIGH("OEMCardSessionNotifier_CopyEventInfo", 0, 0, 0);

  if ((NULL == pInEvent) || (NULL == pOutEvent))
    return EBADPARM;

  pOutEvent->evt = pInEvent->evt;

  switch (pInEvent->evt)
  {
  case MMGSDI_CARD_INSERTED_EVT:
    (void) MEMCPY(&pOutEvent->data.card_inserted, 
                  &pInEvent->data.card_inserted, 
                  sizeof(mmgsdi_card_inserted_evt_info_type));
    break;
  case MMGSDI_CARD_INIT_COMPLETED_EVT:
    (void) MEMCPY(&pOutEvent->data.card_init_completed, 
                  &pInEvent->data.card_init_completed, 
                  sizeof(mmgsdi_card_init_completed_evt_info_type));
    break;
  case MMGSDI_SELECT_AID_EVT:
    (void) MEMCPY(&pOutEvent->data.select_aid, 
                  &pInEvent->data.select_aid, 
                  sizeof(mmgsdi_select_aid_evt_info_type));
    break;
  case MMGSDI_REFRESH_EVT:
    (void) MEMCPY(&pOutEvent->data.refresh, 
                  &pInEvent->data.refresh, 
                  sizeof(mmgsdi_refresh_evt_info_type));
    break;
  case MMGSDI_PIN1_EVT:
  case MMGSDI_PIN2_EVT:
  case MMGSDI_UNIVERSAL_PIN_EVT:
    (void) MEMCPY(&pOutEvent->data.pin, &pInEvent->data.pin, 
      sizeof(mmgsdi_pin_evt_info_type));
    break;
  case MMGSDI_CARD_ERROR_EVT:
  case MMGSDI_CARD_REMOVED_EVT:
  case MMGSDI_TERMINAL_PROFILE_DL_EVT:
  case MMGSDI_FDN_EVT:
  case MMGSDI_SWITCH_SLOT_EVT:
  case MMGSDI_ILLEGAL_CARD_EVT:
  case MMGSDI_SAP_CONNECT_EVT:
  case MMGSDI_SAP_DISCONNECT_EVT:
  case MMGSDI_SAP_PIN_EVT:
  case MMGSDI_SESSION_CLOSE_EVT:
  case MMGSDI_MAX_EVT_ENUM:
    MSG_ERROR("Evt Not Handled 0x%x", pInEvent->evt, 0, 0);
    return EFAILED;
  } 

  return SUCCESS;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_MapMMGSDISlotToNotifyObj

DESCRIPTION:
   This function maps the MMGSDI Slot to Notify Obj and Cls ID
  
DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
ICardSessionNotifier* OEMCardSessionNotifier_MapMMGSDISlotToNotifyObj (
  mmgsdi_slot_id_enum_type  sMMGSDISlot,
  AEECLSID                 *pClsID)
{
  MSG_HIGH("OEMCardSessionNotifier_MapMMGSDISlotToNotifyObj MMGSDI Slot 0x%x", 
           sMMGSDISlot, 0, 0);

  if (NULL == pClsID) {
    MSG_ERROR("Null Input ClsID Ptr", 0, 0, 0);
    return NULL;
  }

  switch (sMMGSDISlot)
  {
    case MMGSDI_SLOT_1:
      *pClsID        = AEECLSID_CARDSESSION_NOTIFIER;
      return ICardSessionNotifierObj_Slot1;
      
    case MMGSDI_SLOT_2:
      *pClsID        = AEECLSID_CARDSESSION_NOTIFIER_SLOT2;
      return ICardSessionNotifierObj_Slot2;

    default:
      MSG_ERROR("Invalid MMGSDI Slot 0x%x", sMMGSDISlot, 0, 0);
      return NULL;
  }
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_MapFromMMGSDIApps

DESCRIPTION
  Map MMGSDI Apps to AEECardSession Tech bitmask

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_MapFromMMGSDIApps(
  mmgsdi_app_enum_type sApp,
  uint32              *pCardTech)
{
  MSG_HIGH("OEMCardSessionNotifier_MapFromMMGSDIApps MMGSDI App 0x%x", sApp, 0, 0);

  if (NULL == pCardTech)
    return EBADPARM;

  switch (sApp) {
    case MMGSDI_APP_SIM:
      *pCardTech |= AEECARDSESSION_SUB_SIM;
      MSG_HIGH("AEECARDSESSION_SUB_SIM new App Mask: 0x%x", 
               *pCardTech, 0, 0);
      break;
    case MMGSDI_APP_USIM:
      *pCardTech |= AEECARDSESSION_SUB_UICC;
      MSG_HIGH("AEECARDSESSION_SUB_UICC new App Mask: 0x%x", 
               *pCardTech, 0, 0);
      break;
    case MMGSDI_APP_RUIM:
      *pCardTech |= AEECARDSESSION_SUB_RUIM;
      MSG_HIGH("AEECARDSESSION_SUB_RUIM new App Mask: 0x%x", 
               *pCardTech, 0, 0);
      break;
    case MMGSDI_APP_UNKNOWN:
      MSG_HIGH("Receive an unknown App type, No Addition to App Mask: 0x%x", 
                *pCardTech, 0, 0);
      break;
    case MMGSDI_APP_NONE:
      MSG_ERROR("Receive none app App type", 0, 0, 0);
      return EFAILED;
  }  
  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_CopyFromMMGSDIAid

DESCRIPTION
  Copy MMGSDI Aid to AEECardSession Aid 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_CopyFromMMGSDIAid(
  mmgsdi_aid_type        sAid,
  int32                 *pCardAidLen,
  uint8                 *pCardAid)
{
  MSG_HIGH("OEMCardSessoin_CopyFromMMGSDIAid", 0, 0, 0);

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
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_CopyFromMMGSDILabel

DESCRIPTION
  Copy MMGSDI Label to AEECardSession Label 
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_CopyFromMMGSDILabel(
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
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_CopyFromMMGSDIAidStruct

DESCRIPTION
  Copy MMGSDI Aid Structure to the corresponding data structure in 
  AEECardSession

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_CopyFromMMGSDIAidStruct (
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

  nReturnStatus = OEMCardSessionNotifier_MapFromMMGSDIApps(
    sAID.app_type,
    pCardTech);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSessionNotifier_CopyFromMMGSDIAid(
    sAID,
    pCardAidLen,
    pAid);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }   

  nReturnStatus = OEMCardSessionNotifier_CopyFromMMGSDILabel(
    sAID,
    pLabel);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }  
  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_CopyFromMMGSDIRefreshMode

DESCRIPTION
  Copy MMGSDI Refresh Mode to the corresponding AEECardSessionNotifier
  Refresh Mode

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_CopyFromMMGSDIRefreshMode (
  mmgsdi_refresh_mode_enum_type  sMMGSDIRefreshMode,
  uint32                        *pCardRefreshMode )
{
  MSG_HIGH("OEMCardSessionNotifier_CopyFromMMGSDIRefreshMode", 0, 0, 0);

  if (NULL == pCardRefreshMode){
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EFAILED;
  }

  switch(sMMGSDIRefreshMode)
  {
    case MMGSDI_REFRESH_RESET:
      *pCardRefreshMode = AEECARDSESSION_NOTIFIER_REFRESH_RESET;
      break;
    case MMGSDI_REFRESH_SIM_INIT:
      *pCardRefreshMode = AEECARDSESSION_NOTIFIER_REFRESH_SIM_INIT;
      break;
    case MMGSDI_REFRESH_SIM_INIT_FCN:
      *pCardRefreshMode = AEECARDSESSION_NOTIFIER_REFRESH_SIM_INIT_FCN;
      break;
    case MMGSDI_REFRESH_FCN:
      *pCardRefreshMode = AEECARDSESSION_NOTIFIER_REFRESH_FCN;
      break;
    case MMGSDI_REFRESH_SIM_INIT_FULL_FCN:
      *pCardRefreshMode = AEECARDSESSION_NOTIFIER_REFRESH_SIM_INIT_FULL_FCN;
      break;
    case MMGSDI_REFRESH_APP_RESET:
      *pCardRefreshMode = AEECARDSESSION_NOTIFIER_REFRESH_APP_RESET;
      break;
    case MMGSDI_REFRESH_3G_SESSION_RESET:
      *pCardRefreshMode = AEECARDSESSION_NOTIFIER_REFRESH_3G_SESSION_RESET;
      break;
    case MMGSDI_REFRESH_MODE_ENUM_MAX:
      MSG_ERROR("Invalid Refresh Mode: MMGSDI_REFRESH_MODE_ENUM_MAX", 0, 0, 0);
      return EFAILED;
  }

  MSG_HIGH("Refresh mode MMGSDI: 0x%x, Card: 0x%x", 
    sMMGSDIRefreshMode, *pCardRefreshMode, 0);

  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_CopyFromMMGSDIRefreshStage

DESCRIPTION
  Copy MMGSDI Refresh Stage to the corresponding AEECardSessionNotifier
  Refresh Stage

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionNotifier_CopyFromMMGSDIRefreshStage (
  mmgsdi_refresh_stage_enum_type  sMMGSDIRefreshStage,
  uint32                         *pCardRefreshStage )
{
  MSG_HIGH("OEMCardSessionNotifier_CopyFromMMGSDIRefreshStage", 0, 0, 0);

  if (NULL == pCardRefreshStage){
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EFAILED;
  }
    
  switch(sMMGSDIRefreshStage)
  {
    case MMGSDI_REFRESH_STAGE_START:
      *pCardRefreshStage = AEECARDSESSION_NOTIFIER_REFRESH_STAGE_START;
      break;
    case MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED:
      *pCardRefreshStage = AEECARDSESSION_NOTIFIER_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED;
      break;
    case MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED:
      *pCardRefreshStage = AEECARDSESSION_NOTIFIER_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED;
      break;
    case MMGSDI_REFRESH_STAGE_END_SUCCESS:
      *pCardRefreshStage = AEECARDSESSION_NOTIFIER_REFRESH_STAGE_END_SUCCESS;
      break;
    case MMGSDI_REFRESH_STAGE_END_FAILED:
      *pCardRefreshStage = AEECARDSESSION_NOTIFIER_REFRESH_STAGE_END_FAILED;
      break;
    case MMGSDI_REFRESH_STAGE_ENUM_MAX:
      MSG_ERROR("Invalid Refresh stage: MMGSDI_REFRESH_STAGE_ENUM_MAX", 0, 0, 0);
      return EFAILED;
  }

  MSG_HIGH("Refresh Stage MMGSDI: 0x%x, Card: 0x%x", 
    sMMGSDIRefreshStage, *pCardRefreshStage, 0);

  return SUCCESS;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_AllocEventCBS

DESCRIPTION:
  This function allocate the Event CBs

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
OEMCardSessionEventCBS * OEMCardSessionNotifier_AllocEventCBS(
  uint32 dwDataSize)
{
  OEMCardSessionEventCBS *pcb = NULL;

  MSG_HIGH("OEMCardSessionNotifier_AllocEventCBS size 0x%x", dwDataSize, 0, 0);

  if (NULL == ICardSessionCoreObj)
    return NULL;

  pcb = (OEMCardSessionEventCBS *)mem_malloc(&tmc_heap, 
                                             sizeof(OEMCardSessionEventCBS));
  if (NULL == pcb) 
  {
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
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_CleanupCardInsertedEvt

DESCRIPTION:
  This function clean up the Card Inserted event data:
  it will set the information to initial values for the members and free
  data pointer as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionNotifier_CleanupCardInsertedEvt(
  AEECardSessionCardInsertedData   *pCardInsertedData)
{
  int i = 0;

  MSG_HIGH("OEMCardSessionNotifier_CleanupCardInsertedEvt", 0, 0, 0);

  if (NULL == pCardInsertedData)
    return;

  for (i = 0; i < AEECARDSESSION_MAX_APPS; i++) {
    (void)MEMSET(&pCardInsertedData->aAppIDList[i], 
                 0x00, 
                 sizeof(AEECardSessionAppData));
  }
  pCardInsertedData->dwNumApp  = 0;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_CleanupAppSelectedEvt

DESCRIPTION:
  This function clean up the Selected App event data:
  it will set the information to initial values for the members and free
  data pointer as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionNotifier_CleanupAppSelectedEvt(
  AEECardSessionAppSelectedData   *pCardAppSelectedData)
{
  MSG_HIGH("OEMCardSessionNotifier_CleanupAppSelectedEvt", 0, 0, 0);

  if (NULL == pCardAppSelectedData)
    return;

  (void)MEMSET(&pCardAppSelectedData->aAppID, 
               0x00, 
               sizeof(AEECardSessionAppData));
  pCardAppSelectedData->nChannelID       = 0;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_CleanupCardReadyEvt

DESCRIPTION:
  This function clean up the Card Ready event data:
  it will set the information to initial values for the members and free
  data pointer as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionNotifier_CleanupCardReadyEvt(
  AEECardSessionCardReadyData   *pCardReadyData)
{
  int i = 0;
   
  MSG_HIGH("OEMCardSessionNotifier_CleanupCardReadyEvt", 0, 0, 0);
  
  if (NULL == pCardReadyData)
    return;

  for (i = 0; i < AEECARDSESSION_MAX_APPS; i++) {
    (void)MEMSET(&pCardReadyData->aAppInfoList[i], 
                 0x00, 
                 sizeof(AEECardSessionAppInfo));
  }
  pCardReadyData->dwNumApp  = 0;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_CleanupCardErrorEvt

DESCRIPTION:
  This function clean up the Card Error event data:
  it will set the information to initial values for the members and free
  data pointer as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionNotifier_CleanupCardErrorEvt(
  AEECardSessionCardErrorData   *pCardErrorData)
{
  int i = 0;

  MSG_HIGH("OEMCardSessionNotifier_CleanupCardErrorEvt", 0, 0, 0);

  if (NULL == pCardErrorData)
    return;

  for (i = 0; i < AEECARDSESSION_MAX_APPS; i++) {
    for (i = 0; i < AEECARDSESSION_MAX_APPS; i++) {
      (void)MEMSET(&pCardErrorData->aAppIDList[i], 
                   0x00, 
                   sizeof(AEECardSessionAppData));
    }
  }
  pCardErrorData->dwNumApp  = 0;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_CleanupCardRefreshEvt

DESCRIPTION:
  This function clean up the Card Refresh event data:
  it will set the information to initial values for the members and free
  data pointer as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionNotifier_CleanupCardRefreshEvt(
  AEECardSessionCardRefreshData   *pCardRefreshData)
{
  MSG_HIGH("OEMCardSessionNotifier_CleanupCardRefreshEvt", 0, 0, 0);
  
  if (NULL == pCardRefreshData)
    return;

  (void)MEMSET(&pCardRefreshData->aAppID, 
         0x00, 
         sizeof(AEECardSessionAppData));

  pCardRefreshData->dwRefreshMode = 0;
  pCardRefreshData->dwRefreshStage = 0;
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_FreeCardEvent

DESCRIPTION:
  This function free Card Event Data as needed

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionNotifier_FreeCardEvent(
  AEECardSessionCardEventData *pCardEvent)
{
  MSG_HIGH("OEMCardSessionNotifier_FreeCardEvent", 0, 0, 0);

  if (NULL == pCardEvent) {
    return;
  }
  
  switch(pCardEvent->dwCardEvent) {
    case AEECARDSESSION_NOTIFIER_CARD_INSERTED:
      OEMCardSessionNotifier_CleanupCardInsertedEvt(
        &pCardEvent->uEventData.sCardInserted);
      break;

    case AEECARDSESSION_NOTIFIER_APP_SELECTED:
      OEMCardSessionNotifier_CleanupAppSelectedEvt(
        &pCardEvent->uEventData.sAppSelected);
      break;

    case AEECARDSESSION_NOTIFIER_CARD_ERROR:
      OEMCardSessionNotifier_CleanupCardErrorEvt(
        &pCardEvent->uEventData.sCardError);
      break;

    case AEECARDSESSION_NOTIFIER_CARD_READY:
      OEMCardSessionNotifier_CleanupCardReadyEvt(
        &pCardEvent->uEventData.sCardReady);
      break;

    case AEECARDSESSION_NOTIFIER_CARD_REFRESH:
      OEMCardSessionNotifier_CleanupCardRefreshEvt(
        &pCardEvent->uEventData.sCardRefresh);
      break;

    default:
      MSG_ERROR("OEMCardSession Unknown dwCardEvent 0x%x", 
        pCardEvent->dwCardEvent, 0, 0);
      break;
  }
}


/*=============================================================================
FUNCTION: OEMCardSessionNotifier_FreeEventCBS

DESCRIPTION:
  This function free the Event CBs

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionNotifier_FreeEventCBS(
  OEMCardSessionEventCBS *pcb)
{

  MSG_HIGH("OEMCardSessionNotifier_FreeEventCBS", 0, 0, 0);

  if (NULL == pcb)
    return;

  if (pcb->nRef <= 0) {
    return;
  }

  if (--pcb->nRef > 0) {
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
}
#endif /* FEATURE_MMGSDI */
#endif /* FEATURE_ICARDSESSION */
