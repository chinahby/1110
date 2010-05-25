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

$Header: //source/qcom/qct/modem/uim/icardsession/rel/07H1_2/src/OEMCardSessionNotifier.c#1 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
12/03/07     sk      Fixed compilation warning
11/07/07     sk      Added support for NAA Refresh
11/08/07     vs      Featurized header declaration for "SysModelBase_priv.h"
03/07/07     sun     Added support for Disable with Replacement
10/25/06     tml     Added initial support for Universal PIN replacement 
                     support and lint
08/25/06     tml     Added Refresh Event Notification support 
07/24/06     tml     Initial Revision
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "comdef.h"     // Get DMSS type declarations.
#include "err.h"        // Error functions
#include "OEMCardSessionNotifier.h"
#include "OEMCardSession.h"

#ifdef FEATURE_ICARDSESSION_IMODEL
#include "SysModelBase_priv.h"
#endif /* FEATURE_ICARDSESSION_IMODEL */
#include "AEEModel.h"

#include "mmgsdilib.h"
#include "intconv.h"


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
  pOutEvent->client_id = pInEvent->client_id;

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

#endif /* FEATURE_MMGSDI */
