#ifndef OEMCARDSESSIONNOTIFIER_H
#define OEMCARDSESSIONNOTIFIER_H
/*=============================================================================

FILE: OEMCARDSESSIONNOTIFIER.h

SERVICES: OEM Functions for Card Session Management Notifier Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //depot/asic/qsc60x0/apps/StaticExtensions/OEM/Inc/OEMCardSessionNotifier.h#2 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
10/02/07     vs      Featurized header decleration for "SysModelBase_priv.h"
10/25/06     tml     Added initial support for Universal PIN replacement 
                     support
08/25/06     tml     Added Refresh Event Notification support 
07/24/06     tml     Initial Revision
=============================================================================*/

#include "customer.h"
#include "OEMFeatures.h"
#include "BREWVersion.h"

#include "AEE.h"
#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEComdef.h"

#include "AEECardSessionNotifier.h"
#include "OEMCardSession.h"

#include "OEMHeap.h"
#include "OEMObjectMgr.h"

#include "OEMFeatures.h"
#include "BREWVersion.h"
#include "AEE_OEMDispatch.h"

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI */

#ifndef AEE_STATIC
#define AEE_STATIC
#endif

#include "AEEModel.h"
#include "AEE_OEM.h"
#include "OEMOS.h"
#include "AEEStdLib.h"
#include "OEMHeap.h"


#include "OEMObjectMgr.h"
#ifdef FEATURE_ICARDSESSION_IMODEL
#include "SysModelBase_priv.h"
#endif /* FEATURE_ICARDSESSION_IMODEL */

/*===========================================================================

                    TYPE DEFINITIONS

===========================================================================*/
typedef struct ICardSessionNotifier ICardSessionNotifier;

AEEINTERFACE(ICardSessionNotifier)
{
  INHERIT_INotifier(ICardSessionNotifier);
};


#ifdef FEATURE_MMGSDI
typedef struct
{
  mmgsdi_events_enum_type  sMMGSDIEvtEnum;
  uint32                   dwCardSessionEvtEnum;
} OEMCardSessionNotifierCardEventTableType;

extern OEMCardSessionNotifierCardEventTableType OEMCardSessionNotifierCardEventTable[];
#endif /* FEATURE_MMGSDI */


/* OEMCardSessionNotifier Event Callback object.  It is used to transfer
** data from the DMSS context to the BREW context */
typedef struct OEMCardSessionEventCBS
{
  AEECallback                     cb;
#ifdef FEATURE_MMGSDI
  mmgsdi_event_data_type         *pData;
#endif /* FEATURE_MMGSDI */
  int32                           nRef;
  struct OEMCardSessionEventCBS  *pNext;
  struct OEMCardSessionEventCBS  *pPrev;
} OEMCardSessionEventCBS;


/* ICardSessionNotifier object, instantiated once */
struct ICardSessionNotifier 
{
   AEEVTBL(ICardSessionNotifier) *   pvt;
   IShell *                          m_pIShell;
   uint32                            m_uRefs;
   AEECLSID                          m_cls;
   AEECallback                       m_cbExit;
   struct ICardSessionCore *         m_coreObj;   /* pointer to core object */
   uint32                            m_dwNotify;  /* Notification bits */
   OEMCardSessionSlotEnum            m_u8Slot;
};

/*===========================================================================

               INTERFACE   FUNCTION   DECLARATIONS

===========================================================================*/

/* Interface to the CardSessionNotifier object. */
int OEMCardSessionNotifier_New(IShell *pIShell, AEECLSID cls, void **ppif);
uint32 OEMCardSessionNotifier_AddRef(ICardSessionNotifier *pMe);
uint32 OEMCardSessionNotifier_Release(ICardSessionNotifier *pMe);
//TODO really need this for 6550?
#if MIN_BREW_BUILD(3,1,4,7)
void   OEMCardSessionNotifier_SetMask(ICardSessionNotifier *pMe, uint32 *pdwMask);
#else
void   OEMCardSessionNotifier_SetMask(ICardSessionNotifier *pMe, const uint32 *pdwMask);
#endif /* MIN_BREW_BUILD(3,1,4,7) */


/*===========================================================================

               INTERNAL FUNCTION DECLARATIONS

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
  const uint32 *pdwMasks);


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
  void *pUser);


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
  uint32                dwMask, 
  void                       *pNotifyData,
  const ICardSessionNotifier *pNotifierobj,
  AEECLSID              sClsId);


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
  const mmgsdi_event_data_type *pEvent);


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
  uint32                 *pCardEvtEnum );


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
  AEECardSessionCardInsertedData           *pCardInsertedData);


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
  AEECardSessionAppSelectedData            *pCardAppSelectedData);


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
  const AEECardSessionCardErrorData   *pCardErrorData);


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
  AEECardSessionCardReadyData                    *pCardReadyData);


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
  AEECardSessionCardRefreshData      *pCardRefreshData);


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
  uint32                     *pCardPinEventEnum);


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
  uint32                          *pCardPinReplacementEnum);


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
  uint32              *pCardTech);


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
  AECHAR                *pCardLabel);


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
  uint8                 *pCardAid);


/*===========================================================================
FUNCTION: OEMCardSessionNotifier_CopyFromMMGSDIAidStruct

DESCRIPTION
  Copy MMGSDI Aid Structure to the corresponding data structure in AEECardSession

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
  AECHAR          *pLabel );


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
  uint32                        *pCardRefreshMode );


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
  uint32                         *pCardRefreshStage );


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
  AEECLSID                 *pClsID);


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
  mmgsdi_event_data_type       *pOutEvent);


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
  uint32 dwDataSize);


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
  AEECardSessionCardInsertedData   *pCardInsertedData);


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
  AEECardSessionAppSelectedData   *pCardAppSelectedData);


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
  AEECardSessionCardReadyData   *pCardReadyData);


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
  AEECardSessionCardErrorData   *pCardErrorData);


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
  AEECardSessionCardRefreshData   *pCardRefreshData);


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
  AEECardSessionCardEventData *pCardEvent);


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
  OEMCardSessionEventCBS *pcb);
#endif /* FEATURE_MMGSDI */
#endif /* OEMCARDSESSIONNOTIFIER_H */
