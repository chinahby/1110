#ifndef OEMCARDSESSIONMODEL_H
#define OEMCARDSESSIONMODEL_H
/*=============================================================================

FILE: OEMCARDSESSIONMODEL.h

SERVICES: OEM Functions for Card Session Management Notifier Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //source/qcom/qct/modem/uim/icardsession/rel/07H1_2/inc/OEMCardSessionModel.h#1 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     --------------------------------------------------------- 
12/14/07     sk      Added newline character at EOF to remove warning
12/04/07     sk      Fixed compilation warning
08/24/07     sk      Initial Revision
=============================================================================*/

#include "customer.h"
#include "OEMFeatures.h"
#include "BREWVersion.h"

#include "AEE.h"
#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEComdef.h"

#include "AEECardSessionModel.h"
#include "AEECardSessionNotifier.h"
#include "OEMCardSession.h"
#include "OEMCardSessionNotifier.h"

#include "OEMHeap.h"
#include "OEMObjectMgr.h"

#include "OEMFeatures.h"
#include "BREWVersion.h"

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#endif /* FEATURE_BREW_3_0 */

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI */

#ifndef AEE_STATIC
#define AEE_STATIC
#endif

#include "err.h"
#include "AEE_OEM.h"
#include "OEMOS.h"
#include "AEEStdLib.h"
#include "OEMHeap.h"

#include "OEMObjectMgr.h"
#ifdef FEATURE_ICARDSESSION_IMODEL
#include "SysModelBase_priv.h"
#endif /*FEATURE_ICARDSESSION_IMODEL */
#include "AEEModel.h"


/*===========================================================================

                    TYPE DEFINITIONS

===========================================================================*/
typedef struct
{
#ifdef FEATURE_MMGSDI
  mmgsdi_events_enum_type  sMMGSDIEvtEnum;
#endif
  uint32                   dwCardSessionEvtEnum;
} OEMCardSessionModelCardEventTableType;


extern OEMCardSessionModelCardEventTableType OEMCardSessionModelCardEventTable[];

/* Interface to the CardSessionModel object. */
int OEMCardSessionModel_New (ICardSession *pIShell,   void **ppif);
uint32 OEMCardSessionModel_Release (ICardSessionModel *pMe);

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
OEMCardSessionEventCBS * OEMCardSessionModel_AllocEventCBS(uint32 dwDataSize);


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
  OEMCardSessionEventCBS *pcb);


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
  AEECardSessionAppSelectedData   *pCardAppSelectedData);


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
  AEECardSessionCardReadyData   *pCardReadyData);


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
  AEECardSessionCardRefreshData   *pCardRefreshData);


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
  AEECardSessionCardEventData *pCardEvent);


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
  AEECardSessionCardErrorData   *pCardErrorData);


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
  AEECardSessionCardInsertedData   *pCardInsertedData);

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
ICardSessionModel * OEMCardSessionModel_GetModelObjFromClientId(
    uint64 client_id);


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
int OEMCardSessionModel_RemoveModelObjNode(uint64 client_id);


#ifdef FEATURE_MMGSDI
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
void OEMCardSessionModel_MMGSDIEvtCb(const mmgsdi_event_data_type *pEvent);


/*=============================================================================
FUNCTION: OEMCardSessionModel_CopyEventInfo

DESCRIPTION:
   This function copies the event info
  
DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCardSessionModel_CopyEventInfo(
  const mmgsdi_event_data_type *pInEvent,
  mmgsdi_event_data_type       *pOutEvent);


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
  uint32                 *pCardEvtEnum );


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyCardInsertedData

DESCRIPTION
  Map MMGSDI Card Inserted Data to AEECardSessionNotifier Card Inserted Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyCardInsertedData(
  const mmgsdi_card_inserted_evt_info_type *pInsertedData,
  AEECardSessionCardInsertedData           *pCardInsertedData);


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyFromMMGSDIAidStruct

DESCRIPTION
  Copy MMGSDI Aid Structure to the corresponding data structure in AEECardSession

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
  AECHAR          *pLabel );


/*===========================================================================
FUNCTION: OEMCardSessionModel_MapFromMMGSDIApps

DESCRIPTION
  Map MMGSDI Apps to AEECardSession Tech bitmask

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_MapFromMMGSDIApps(
  mmgsdi_app_enum_type sApp,
  uint32              *pCardTech);


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
  uint8                 *pCardAid);


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
  AECHAR                *pCardLabel);


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
  AEECardSessionAppSelectedData            *pCardAppSelectedData);


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyCardErrorData

DESCRIPTION
  Map MMGSDI Card Error Data to AEECardSessionNotifier Card Error Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyCardErrorData(
  const mmgsdi_card_err_evt_info_type *pErrorData,
  const AEECardSessionCardErrorData   *pCardErrorData);


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
  AEECardSessionCardReadyData                    *pCardReadyData);


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyCardRefreshData

DESCRIPTION
  Map MMGSDI Refresh Data to AEECardSessionModel Card Refresh Event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyCardRefreshData(
  const mmgsdi_refresh_evt_info_type *pRefreshData,
  AEECardSessionCardRefreshData      *pCardRefreshData);


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
  uint32                        *pCardRefreshMode );


/*===========================================================================
FUNCTION: OEMCardSessionModel_CopyFromMMGSDIRefreshStage

DESCRIPTION
  Copy MMGSDI Refresh Stage to the corresponding AEECardSessionModel
  Refresh Stage

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSessionModel_CopyFromMMGSDIRefreshStage (
  mmgsdi_refresh_stage_enum_type  sMMGSDIRefreshStage,
  uint32                         *pCardRefreshStage );


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
int OEMCardSessionModel_CopyFromMMGSDIRefreshFileList(
  mmgsdi_refresh_file_list_type sMMGSDIFileList,
  AEECardSessionRefreshData    *pCardRefreshFileList);
#endif /* FEATURE_MMGSDI */
#endif /* FEATURE_ICARDSESSION_IMODEL */

#endif /*OEMCARDSESSIONMODEL_H*/

