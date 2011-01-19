#ifndef OEMCARDSESSION_H
#define OEMCARDSESSION_H
/*=============================================================================

FILE: OEMCARD_SESSION.h

SERVICES: OEM Functions for Card Session Management Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2004-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //source/qcom/qct/modem/uim/icardsession/rel/07H1_2/inc/OEMCardSession.h#1 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
06/21/08     kk      Added PKCS15 Support
04/08/07     sun     Only do Client Deregistration after closing sessions
01/17/08     nk      Added support for change esn me enum mapping
12/20/07     nk      Fixed Compiler Warning
11/07/07     sk      Added support for NAA Refresh
11/08/07     vs      Featurized header declaration for "SysModelBase_priv.h"
09/27/07     tml     Lint fix
09/18/07     tml     Added MFLO support
06/04/07     nk      Added JCDMA card get info support
05/07/07     sun     Added support for Srv Available and USIM Auth
03/07/07     sun     Added support for Disable with Replacement
02/27/07     nk      Added Registration API
02/27/07     sun     Added Support for Read/Write by Path
07/26/06     tml     lint fixes
06/12/06     SUN     Removed Unnecessary Includes
04/19/04     SUN     Initial Revision
=============================================================================*/

#include "customer.h"
#include "OEMFeatures.h"
#include "BREWVersion.h"

#include "AEE.h"
#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEComdef.h"

#include "AEECardSession.h"

#include "OEMHeap.h"
#include "OEMObjectMgr.h"

#include "OEMFeatures.h"
#include "BREWVersion.h"

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#endif /* FEATURE_BREW_3_0 */

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#include "tmc.h"
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
#endif /* FEATURE_ICARDSESSION_IMODEL */

#include "AEEModel.h"

#ifdef AEE_SIMULATOR

#ifdef MSG_ERROR
   #undef MSG_ERROR
#endif
#ifdef MSG_HIGH
   #undef MSG_HIGH
#endif
#ifdef MSG_MED
   #undef MSG_MED
#endif
#ifdef MSG_LOW
   #undef MSG_LOW
#endif

#endif /* AEE_SIMULATOR */


/* Max number of simultaneous calls */

#if __ARMCC_VERSION >= 120000 // ADS 1.2
#pragma arm section zidata
#endif // __ARMCC_VERSION >= 120000 // ADS 1.2

/* suppress warning for unused parameter. */
#undef PARAM_NOT_REF
#define PARAM_NOT_REF(x) /*lint -esym(715,x) */

/* Adds a node to the head of a doubly linked list */
/* l-value of head should not be NULL */
#define OEMCARDSESSION_ADD_NODE(head, node) \
        { \
          if ((head)) (head)->pPrev = (node); \
          (node)->pPrev = NULL; \
          (node)->pNext = (head); \
          (head) = (node); \
        }

/* Removes a node from the doubly linked list */
/* l-value of head should not be NULL */
#define OEMCARDSESSION_REMOVE_NODE(head, node) \
        { \
          if ((node)->pPrev) \
            (node)->pPrev->pNext = (node)->pNext; \
          else \
            (head) = (node)->pNext; \
          if ((node)->pNext) \
            (node)->pNext->pPrev = (node)->pPrev; \
        }

/* Note, this macro should only be called if mem_ptr is NOT null */
#define OEMCARDSESSION_SYS_FREE(mem_ptr) \
        { \
          mem_free(&tmc_heap, (mem_ptr)); \
          (mem_ptr) = NULL; \
        } 

#define OEMCARDESESSION_DEFAULT_CLIENT_ID    0

/*===========================================================================

                    TYPE DEFINITIONS

===========================================================================*/
typedef enum
{
  OEMCARDSESSION_NONE     = 0,
  OEMCARDSESSION_OPENING  = 1,
  OEMCARDSESSION_OPENED   = 2,
  OEMCARDSESSION_SELECTED = 3,
  OEMCARDSESSION_CLOSED   = 4
}OEMCardSessionChannelStatusEnum;

/*bySlot*/
typedef enum
{
  OEMCARDSESSION_SLOT_NONE,                 
  OEMCARDSESSION_SLOT1,       
  OEMCARDSESSION_SLOT2       
} OEMCardSessionSlotEnum;

#ifdef FEATURE_MMGSDI
typedef struct
{
  uint32                    nCardFile;
  mmgsdi_file_enum_type     nMMGSDIFile;
} OEMCardSessionMMGSDIFileEnumTableType;

extern OEMCardSessionMMGSDIFileEnumTableType OEMCardSessionMMGSDIFileEnumTable[];
#endif /* FEATURE_MMGSDI */

typedef struct AEECardSessionOwner 
{
  void *                      data;
  uint64                      clientID;
  int32                       nRef;
  struct AEECardSessionOwner *pNext;
  struct AEECardSessionOwner *pPrev;
} AEECardSessionOwner;

/* Core Object, instantiated once */
typedef struct ICardSessionCore 
{
  uint32                         m_uRefs;
  AEECardSessionOwner           *m_SessionOwner;
  struct OEMCardSessionEventCBS *m_EventCbs;
} ICardSessionCore;

extern ICardSessionCore     *ICardSessionCoreObj;

typedef struct ICardSessionModel {
#ifdef FEATURE_ICARDSESSION_IMODEL
   SysModelBase    base;
#endif /* FEATURE_ICARDSESSION_IMODEL */
   AEEVTBL(IModel) vtModel;
} ICardSessionModel;

/* ICardSession object, instantiated every time */
struct ICardSession 
{
  union {
    AEEVTBL(ICardSession) *        pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject                   m_so;
#endif
  } u;
  IShell *                         m_pIShell;
  uint32                           m_uRefs;
  AEECLSID                         m_cls;
  ICardSessionCore *               m_coreObj;    /* pointer to core object */
  void *                           m_pac;        /* App context */
  uint64                           m_client_id;
  OEMCardSessionChannelStatusEnum  m_byChannelStatus;
  OEMCardSessionSlotEnum           m_u8Slot;
  ICardSessionModel *              m_modelObj;
};

typedef struct
{
  AEECallback        *pCB;
  void               *pClientData;
  AEEObjectHandle     Handler;
} OEMCardSessionAsynCBInfo;

typedef struct
{
  AEEObjectHandle   ObjHandler;
  ICardSession      *m_pCardSession;
} OEMCardSessionDataType;

/*===========================================================================

               INTERFACE   FUNCTION   DECLARATIONS

===========================================================================*/


/* Interface to the CardSession object. */
int OEMCardSession_New(IShell *pIShell, AEECLSID cls, void **ppif);
uint32 OEMCardSession_AddRef(ICardSession *pMe);
uint32 OEMCardSession_Release(ICardSession *pMe);
int    OEMCardSession_QueryInterface(ICardSession *pMe, AEECLSID clsid, void **ppNew);


/*===========================================================================

               INTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*=============================================================================
FUNCTION: OEMCardSessionCore_FreeOnAEEExit

DESCRIPTION:
  This function is called by ICardSessionNotifier if it exits in response to BREW
  exiting. Hopefully the object reference is zero by now and somebody
  has already freed the core object. Just in case that has not happened, we do
  it here.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionCore_FreeOnAEEExit(
  void *pUser);


#ifdef FEATURE_MMGSDI
/*=============================================================================
FUNCTION: OEMCardSessionCmdCb

DESCRIPTION:
  Callback function for MMGSDI

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionCmdCb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *pCnf);


/*===========================================================================
FUNCTION: OEMCard_MapMMGSDISlot

DESCRIPTION
  Map AEECARDSession slot to MMGSDI Slot enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapMMGSDISlot(
  OEMCardSessionSlotEnum    u8Slot, 
  mmgsdi_slot_id_enum_type *pMMgsdiSlot);


/*===========================================================================
FUNCTION: OEMCardSession_MapFromGSDIFileEnum

DESCRIPTION
  Map AEECardSession File Enum to MMGSDI File Enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFromMMGSDIFileEnum(
  mmgsdi_file_enum_type dwFileEnum,
  uint32                *pCardFileEnum);


/*===========================================================================
FUNCTION: OEMCardSession_MapFileEnum

DESCRIPTION
  Map AEECardSession File Enum to GSDI File Enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFileEnum(
  mmgsdi_file_enum_type *pFileEnum,
  uint32                 dwCardFileEnum);


/*===========================================================================
FUNCTION: OEMCardSession_MapFileAccess

DESCRIPTION
  Map AEECardSession File Access to GSDI File Access

DEPENDENCIES
  none

RETURN VALUE
  EBADPARM - Parameters provided were incorrect
  SUCCESS  - Successfully converted to MMGSDI access

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFileAccess(
  AEECardSessionFilePathType sFilePath,
  mmgsdi_access_type         *pFileAccess);


/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIStatus

DESCRIPTION
  Map MMGSDI status to Brew status

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFromMMGSDIStatus(
  mmgsdi_return_enum_type sStatus);


/*===========================================================================
FUNCTION: OEMCardSession_MapPinID

DESCRIPTION
  Map AEECardSession Pin ID to MMGSDI Pin ID

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapPinID(
  mmgsdi_pin_enum_type *pPinIDEnum,
  uint32                dwPinIDEnum);


/*===========================================================================
FUNCTION: OEMCardSession_MapReplaceOption

DESCRIPTION
  Map AEECardSession Replace Option to MMGSDI Replace Option

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapReplaceOption(
  mmgsdi_pin_replace_enum_type   *pReplaceEnum,
  uint32                         dwReplaceEnum);


/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIPinID

DESCRIPTION
  Map MMGSDI Pin ID to AEECardSession Pin ID

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFromMMGSDIPinID(
  mmgsdi_pin_enum_type sPinIDEnum,
  uint32              *pCardPinIDEnum);


/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIPinStatus

DESCRIPTION
  Map MMGSDI Pin Status to AEECardSession Pin Status

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFromMMGSDIPinStatus(
  mmgsdi_pin_info_type        sPinInfo,
  uint32                     *pCardPinRetries,
  uint32                     *pCardPukRetries,
  uint32                     *pCardPinStatusEnum);


/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIPinOp

DESCRIPTION
  Map MMGSDI Pin Operation to AEECardSession Pin Operation

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFromMMGSDIPinOp(
  mmgsdi_pin_operation_enum_type sPinOpEnum,
  uint32                        *pCardPinOpEnum);


/*===========================================================================

FUNCTION: OEMCardSession_CleanUpAsynObject

DESCRIPTION
  Performs the clean up process for the object manager handler, the content
  of the object.  boolean bCBCancel indicates if the CALLBACK_Cancel should
  be called for the AEECallback * within the OEMCard_AsynCBInfo structure

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
void OEMCardSession_CleanUpAsynObject(
  AEEObjectHandle Handler);


#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIJCDMACardInfo

DESCRIPTION
  Map MMGSDI JCDMA Card Info to AEECardSession JCDMA Card Info

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
int OEMCardSession_MapFromMMGSDIJCDMACardInfo(
  mmgsdi_jcdma_card_info_enum_type nMMGSDIJCDMACardInfoEnum,
  uint32                           *pJCDMACardInfoEnum);


/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIESNMEChangeType

DESCRIPTION
  Map MMGSDI ESN ME Change Type to AEECardSession ME Change ESN values

DEPENDENCIES
  none

RETURN VALUE
  EBADPARM: bad input paramter
  SUCCESS:  successful mapping from mmgsdi value to icardesession card value
  EBADITEM: invalid card type returned by mmgsdi

SIDE EFFECTS
===========================================================================*/
int OEMCardSession_MapFromMMGSDIESNMEChange(
  mmgsdi_esn_me_change_enum_type   nMMGSDIESNMEChangeEnum,
  uint32                           *pESNMEChangeEnum);
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
#endif /* FEATURE_MMGSDI */


/*=============================================================================
FUNCTION: OEMCardSessionCore_New

DESCRIPTION:
  Initiates  the Coreobject. This Coreobject is common acroos all instances
  of ICardSession

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
ICardSessionCore * OEMCardSessionCore_New(
  void); 


#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */


/*---------------------------------------------------------------------------
                           Callback Functions
--------------------------------------------------------------------------- */
/*=============================================================================
FUNCTION: OEMCardSessionCloseAndFreeCb

DESCRIPTION:
  Closes the session and Frees all data associated with it.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionCloseAndFreeCb(
  void *pData);


/*=============================================================================
FUNCTION: OEMCardSession_Free

DESCRIPTION:
  Free the CardSession

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSession_Free(
 ICardSession *pMe,
 boolean session_open);


#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION: OEMCardSessionInternalCb

DESCRIPTION
  This function is called when OEMCardSession calls MMGSDI during release 
  procedure and when there is a need to close the session prior to 
  releasing releasing the ICardSession object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
void OEMCardSessionInternalCb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *pCnf);


/*===========================================================================

FUNCTION: OEMCardSession_GetLinkSysCallBack

DESCRIPTION
  Performs the AEE_LinkSysCallback operation.  Set the user passed in data
  in the reserve field

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
int OEMCardSession_GetLinkSysCallBack(
  AEECallback *pUserCB,
  void *pCmdStatus,
  AEEObjectHandle *pHandler);
#endif /* FEATURE_MMGSDI */


/****************************************************************************
                              ICARDSESSION APIS
****************************************************************************/
/*=============================================================================
FUNCTION: OEMCardSessionCore_Release

DESCRIPTION:
  Release Core CardSession Object

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
uint32 OEMCardSessionCore_Release(
  void);


/*===========================================================================

FUNCTION: OEMCardSession_Open

DESCRIPTION
  Opens a new session given the APPID

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS       - Request successfully Queued
  EBADPARM     -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
===========================================================================*/
int OEMCardSession_Open(
  ICardSession             *pMe,
  AEECardSessionCmdStatus  *pCmdStatus,
  const AEECardSessionData *pAppID,
  AEECallback              *pUserCB);


/*===========================================================================
FUNCTION: OEMCardSession_Close

DESCRIPTION
  Close a logical channel.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_Close(
  ICardSession            *pMe,
  AEECardSessionCmdStatus *pCmdStatus,
  AEECallback             *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_Authenticate

DESCRIPTION
      App can run authenticate algorithms on the card by providing all the
      appropriate data.
      
RETURN VALUE:
   int: SUCCESS if successful

COMMENTS:


SIDE EFFECTS:
===========================================================================*/
int OEMCardSession_Authenticate(
  ICardSession             *pMe,
  const AEECardSessionData *pAuthData,
  AEECardSessionAuthStatus *pAuthStatus,
  AEECallback              *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_ReadTransparent

DESCRIPTION
  Reads a transparent file

RETURN VALUE
  int: SUCCESS if successful

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_ReadTransparent(
  ICardSession               *pMe,
  uint32                      dwFileName,
  uint32                      dwOffset,
  uint32                      dwReqLen,
  AEECardSessionReadTpStatus *pCmdStatus,
  AEECallback                *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_ReadRecord

DESCRIPTION
  Reads a record based file

RETURN VALUE
  int: SUCCESS if successful

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_ReadRecord(
  ICardSession                *pMe,
  uint32                       dwFileName,
  uint32                       dwRecNum,
  uint32                       dwReqLen,
  AEECardSessionReadRecStatus *pCmdStatus,
  AEECallback                 *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_WriteTransparent

DESCRIPTION
  Writes to a trnasparent file

RETURN VALUE
  int: SUCCESS if successful

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_WriteTransparent(
  ICardSession                  *pMe,
  uint32                         dwFileName,
  uint32                         dwOffset,
  const AEECardSessionData      *pWriteData,
  AEECardSessionWriteTpStatus   *pWriteStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_WriteRecord

DESCRIPTION
  Writes to a record based file

RETURN VALUE
  int: SUCCESS if successful

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_WriteRecord(
  ICardSession                     *pMe,
  uint32                            dwFileName,
  uint32                            dwOffset,
  const AEECardSessionData         *pWriteData,
  AEECardSessionWriteRecStatus     *pWriteStatus,
  AEECallback                      *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_GetFileAttr

DESCRIPTION
  This method provides the service to get the Attribute information on a
  file or directory on the SIM/USIM

RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_GetFileAttr(
  ICardSession                  *pMe,
  AEECardSessionFilePathType     sFilePath,
  AEECardSessionFileAttribStatus*pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_GetCardAllPinsInfo

DESCRIPTION
  Get All the Card PIN information relating to the ICardSession object
  
RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_GetCardAllPinsInfo(
  ICardSession                  *pMe, 
  AEECardSessionCardAllPinsInfo *pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_VerifyPin

DESCRIPTION
  Verify the PIN for the particular application accessed by the 
  ICardSession object.
  
RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_VerifyPin(
  ICardSession                  *pMe, 
  uint32                         dwPinID,
  const AEECardSessionData      *pPin, 
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_EnablePin

DESCRIPTION
  Enable the PIN for the particular application accessed by the 
  ICardSession object.
  
RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_EnablePin(
  ICardSession                  *pMe, 
  uint32                         dwPinID,
  const AEECardSessionData      *pPin, 
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_DisablePinExt

DESCRIPTION
  Disable the PIN for the particular application accessed by the
  ICardSession object and provides the user with replacement option

RETURN VALUE
  SUCCESS -   Successfully queued to the services, response will be provided in 
              cb
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, dwPinID, pPin, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_DisablePinExt(
  ICardSession                  *pMe,
  uint32                         dwPinID,
  const AEECardSessionData      *pPin,
  uint32                         dwReplace,
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_DisablePin

DESCRIPTION
  Disable the PIN for the particular application accessed by the 
  ICardSession object.
  
RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_DisablePin(
  ICardSession                  *pMe, 
  uint32                         dwPinID,
  const AEECardSessionData      *pPin, 
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_UnblockPin

DESCRIPTION
  Unblock the PIN for the particular application accessed by the ICardSession object.  
  The New Pin value will also need to be provided in this operation.
  
RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_UnblockPin(
  ICardSession                  *pMe, 
  uint32                         dwPinID,
  const AEECardSessionData      *pPuk, 
  const AEECardSessionData      *pNewPin, 
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_ChangePin

DESCRIPTION
  Change the PIN for the particular application accessed by the ICardSession object.  
  The New Pin value will also need to be provided in this operation.
  
RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_ChangePin(
  ICardSession                  *pMe, 
  uint32                         dwPinID,
  const AEECardSessionData      *pOldPin, 
  const AEECardSessionData      *pNewPin, 
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_SelectSubscription

DESCRIPTION
  The function selects the subscription.  This function is applicable to all 
  technologies.  
  This function should only be called when a technology initialization is 
  required or when a technology switch is required.
  i.e., UMTS App1 -> UMTS App2 is not applicable without a SIM INSERTED Card Event
  i.e., GSM -> CDMA is applicable
  
RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_SelectSubscription(
  ICardSession                  *pMe, 
  const AEECardSessionSubData   *pSub,
  AEECardSessionCmdStatus       *pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_ReadTransparentByPath

DESCRIPTION
  Reads a transparent file ByPath

RETURN VALUE
  int: SUCCESS if successful

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_ReadTransparentByPath(
  ICardSession               *pMe,
  AEECardSessionFilePathType  sFilePath,
  uint32                      dwOffset,
  uint32                      dwReqLen,
  AEECardSessionReadTpStatus *pCmdStatus,
  AEECallback                *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_ReadRecordByPath

DESCRIPTION
  Reads a record based file ByPath

RETURN VALUE
  int: SUCCESS if successful

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_ReadRecordByPath(
  ICardSession                *pMe,
  AEECardSessionFilePathType   sFilePath,
  uint32                       dwRecNum,
  uint32                       dwReqLen,
  AEECardSessionReadRecStatus *pCmdStatus,
  AEECallback                 *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_WriteTransparentByPath

DESCRIPTION
  Writes to a transparent file ByPath

RETURN VALUE
  int: SUCCESS if successful

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_WriteTransparentByPath(
  ICardSession                  *pMe,
  AEECardSessionFilePathType     sFilePath,
  uint32                         dwOffset,
  const AEECardSessionData      *pWriteData,
  AEECardSessionWriteTpStatus   *pWriteStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_WriteRecordByPath

DESCRIPTION
  Writes to a record based file ByPath

RETURN VALUE
  int: SUCCESS if successful

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_WriteRecordByPath(
  ICardSession                     *pMe,
  AEECardSessionFilePathType        sFilePath,
  uint32                            dwOffset,
  const AEECardSessionData         *pWriteData,
  AEECardSessionWriteRecStatus     *pWriteStatus,
  AEECallback                      *pUserCB);


/*=============================================================================
FUNCTION: OEMCardSession_Register

DESCRIPTION:
  API to register for Client ID with MMGSDI.
  Every app will need to do a client registration of its own.

RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCardSession_Register(
  ICardSession            *pMe,
  AEECardSessionCmdStatus *pCmdStatus,
  AEECallback             *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_IsServiceAvailable

DESCRIPTION
  The function finds out if the service is available in the card

RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_IsServiceAvailable(
  ICardSession                  *pMe,
  uint32                         dwSrvType,
  AEECardSessionCmdStatus       *pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_USIMAuthenticate

DESCRIPTION
  The function Sends a USIM Authenticate command to the card

RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_USIMAuthenticate(
  ICardSession                  *pMe,
  const AEECardSessionData      *pAuthData,
  uint32                         dwContextType,
  AEECardSessionUSIMAuthStatus  *pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_JCDMACard_GetInfo

DESCRIPTION
  This function provides card information on color, lock status, and insertion
  change.  This function must be called after sim_init_completed

RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  If card type is invalid then subscription will not be sent and therefore
  the phone will not be able to register to a network.
===========================================================================*/
int OEMCardSession_JCDMACard_GetInfo(
  ICardSession                  *pMe,
  AEECardSessionJCDMACardInfo   *pCmdStatus,
  AEECallback                   *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_MFLOInitActParams

DESCRIPTION
  The function Sends a MFLO Init Act Params

RETURN VALUE
  SUCCESS      - Request successfully Queued
  EBADPARM     - Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_MFLOInitActParams(
  ICardSession                 *pMe, 
  const AEECardSessionData     *pActParams,
  const AEECardSessionOption   *pOption,
  AEECardSessionCmdStatus      *pInitActParamsStatus,
  AEECallback                  *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_MFLOGetSubscriberID

DESCRIPTION
  The function Sends a MFLO Get Subscriber ID

RETURN VALUE
  SUCCESS   - Request successfully Queued
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_MFLOGetSubscriberID(
  ICardSession                          *pMe,
  const AEECardSessionOption            *pOption,
  AEECardSessionMFLOSubIdStatus         *pSubscriberIdStatus,
  AEECallback                           *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_MFLOGetSubscriberID

DESCRIPTION
  The function Sends a MFLO Get Public Key

RETURN VALUE
  SUCCESS   - Request successfully Queued
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_MFLOGetPublicKey(
  ICardSession                          *pMe,
  const AEECardSessionOption            *pOption,
  AEECardSessionMFLOPubKeyStatus        *pPublicKeyStatus,
  AEECallback                           *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_MFLOSignUcast

DESCRIPTION
  The function Sends a MFLO Sign UCast Message

RETURN VALUE
  SUCCESS   - Request successfully Queued
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_MFLOSignUcast(
  ICardSession                      *pMe,
  uint32                             dwMsgType,
  const AEECardSessionData          *pMsg,
  const AEECardSessionOption        *pOption,
  AEECardSessionCmdDataStatus       *pSignUcastStatus,
  AEECallback                       *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_MFLOVerifyUcast

DESCRIPTION
  The function Sends a MFLO Verify UCast Message

RETURN VALUE
  SUCCESS   - Request successfully Queued
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_MFLOVerifyUcast(
  ICardSession                      *pMe,
  uint32                             dwMsgType,
  const AEECardSessionData          *pMsg,
  const AEECardSessionOption        *pOption,
  AEECardSessionCmdStatus           *pVerifyUcastStatus,
  AEECallback                       *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_GetATR

DESCRIPTION
  The function Get ATR of the card

RETURN VALUE
  SUCCESS   - Request successfully Queued
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_GetATR(
  ICardSession                 *pMe,
  const AEECardSessionOption   *pOption,
  AEECardSessionCmdDataStatus  *pGetATRStatus,
  AEECallback                  *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_GetCardReaderStatus

DESCRIPTION
  The function Get Card Reader Status of the card

RETURN VALUE
  SUCCESS   - Request successfully Queued
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_GetCardReaderStatus(
  ICardSession                   *pMe,
  const AEECardSessionOption     *pOption,
  AEECardSessionCardReaderStatus *pGetCardReaderStatus,
  AEECallback                    *pUserCB);


/*===========================================================================

FUNCTION: OEMCardSession_SendAPDU

DESCRIPTION
  The function Sends APDU to the card

RETURN VALUE
  SUCCESS   - Request successfully Queued
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_SendAPDU(
  ICardSession                      *pMe,
  const AEECardSessionData          *pAPDU,
  const AEECardSessionOption        *pOption,
  AEECardSessionCmdDataStatus       *pSendAPDUStatus,
  AEECallback                       *pUserCB);


/*=============================================================================
FUNCTION: OEMCardSession_RegisterForRefresh

DESCRIPTION:  
  This API will allow the ICardSession Client to Register for File
  Change notifications for the current session (current app). 
      
PROTOTYPE:
    int OEMCardSession_RegisterForRefresh(
                              ICardSession                 *pMe,
                              AEECardSessionCmdStatus      *pCmdStatus,
                              AEECardSessionRefreshData     sRefreshData,
                              boolean                       vote_for_init,
                              AEECardSessionOption         *pOption,
                              AEECallback                  *pUserCB)

PARAMETERS:
    *pMe          : [Input] ICardSession interface pointer
    *pCmdStatus   : [Input] Command Status
     sRefreshData : [Input] Refresh Files that the client is interested in
     vote_for_init: [Input] Client vote for init
    *pOption      : [Input] Optional Parameter pointer (currently not in use)
    *pUserCB      : [Input/Output] App Callback   for this command only.       

RETURN VALUE:
  SUCCESS     - Status information is valid
  EBADPARM    - Bad parameter
  EUNSUPPORT  - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  Voting Request will be received by the client and the 
  client will be required to call ICardSession_OkToRefresh

SEE ALSO:
  None
=============================================================================*/
int OEMCardSession_RegisterForRefresh (
   ICardSession                    *pMe,
   AEECardSessionCmdStatus         *pCmdStatus,
   AEECardSessionRefreshData        sRefreshData,
   boolean                          vote_for_init,
   AEECardSessionOption            *pOption,
   AEECallback                     *pUserCB);


/*=============================================================================
FUNCTION: OEMCardSession_DeRegisterForRefresh

DESCRIPTION:  
  This API will allow the ICardSession Client to DeRegister for File
  Change notifications for the current session (current app). 
      
PROTOTYPE:
    int OEMCardSession_DeRegisterForRefresh(
                              ICardSession                 *pMe,
                              AEECardSessionCmdStatus      *pCmdStatus,
                              AEECardSessionRefreshData    sRefreshData,
                              boolean                      vote_for_init,
                              AEECardSessionOption         *pOption,
                              AEECallback                  *pUserCB)

PARAMETERS:
    *Me           : [Input] ICardSession interface pointer
    *pCmdStatus   : [Input] Command Status
     sRefreshData : [Input] Refresh Files that the client is not interested in
     vote_for_init: [Input] client vote for init
    *pOption      : [Input] Optional Parameter pointer (currently not in use)
    *pUserCB      : [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS    - Status information is valid
  EBADPARM   - Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  If the session is closed or the ICardSession instance is destroyed, 
  then IcardSession will internally deregister FCN with MMGSDI.

SIDE EFFECTS:
  None

SEE ALSO:
  None
=============================================================================*/
int OEMCardSession_DeRegisterForRefresh (
   ICardSession                    *pMe,
   AEECardSessionCmdStatus         *pCmdStatus,
   AEECardSessionRefreshData        sRefreshData,
   boolean                          vote_for_init,
   AEECardSessionOption            *pOption,
   AEECallback                     *pUserCB);


/*=============================================================================
FUNCTION: OEMCardSession_RefreshComplete

DESCRIPTION:  
  This API will allow the ICardSession Client to send Refresh Complete status
  to MMGSDI. 
      
PROTOTYPE:
    int OEMCardSession_RefreshComplete(
                               ICardSession                 *pMe,
                               AEECardSessionCmdStatus      *pCmdStatus,
                               boolean                       dwStatus,
                               AEECardSessionOption         *pOption,
                               AEECallback                  *pUserCB)

PARAMETERS:
    *pMe         : [Input] ICardSession interface pointer
    *pCmdStatus  : [Input] Command Status
     dwStatus    : [Input] Status of the files that the client read OR
                           Status of the init procedure , client carried out.
    *pOption     : [Input] Optional Parameter pointer (currently not in use)
    *pUserCB     : [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS    - Status information is valid
  EBADPARM   - Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  None
=============================================================================*/
int OEMCardSession_RefreshComplete (
   ICardSession                    *pMe,
   AEECardSessionCmdStatus         *pCmdStatus,
   boolean                          dwStatus,
   AEECardSessionOption            *pOption,
   AEECallback                     *pUserCB);


/*=============================================================================
FUNCTION: int OEMCardSession_OkToRefresh

DESCRIPTION:  
  With this API the client is REQUIRED to inform ICardSession whenever 
  a refresh event is received with REFRESH_STAGE_WAIT_FOR_OK_TO_INIT or 
  REFRESH_STAGE_WAIT_FOR_OK_TO_FCN. If the client did not register for 
  voting, then this API need not be called.
      
PROTOTYPE:
    int ICARDSESSION_OkToRefresh(
                                  ICardSession                 *pMe,
                                  AEECardSessionCmdStatus      *pCmdStatus,
                                  Boolean                       okToRefresh,
                                  AEECardSessionOption         *pOption,
                                  AEECallback                  *pUserCB)

PARAMETERS:
    *pMe         : [Input] ICardSession interface pointer
    *pCmdStatus  : [Input] Command Status
     okToRefresh : [Input] Is it ok to bring down call stack or terminate app
    *pOption     : [Input] Optional Parameter pointer (currently not in use)
    *pUserCB     : [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS    - Status information is valid
  EBADPARM   - Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  None
=============================================================================*/
int OEMCardSession_OkToRefresh (
   ICardSession             *pMe,
   AEECardSessionCmdStatus  *pCmdStatus,
   boolean                   okToRefresh,
   AEECardSessionOption     *pOption,
   AEECallback              *pUserCB);


/*=============================================================================
FUNCTION: OEMCardSession_ClientIDDereg

DESCRIPTION:
   This function deregisters the client id

DEPENDENCIES:
  None

SIDE EFFECTS:
  
=============================================================================*/
/*lint -save -esym(818,pMe) suppress could be const */
void OEMCardSession_ClientIDDereg(ICardSession *pMe);

/*=============================================================================
FUNCTION: OEMCardSession_CreatePkcs15LookupTable

DESCRIPTION:  
  This API will allow ICARDSESSION client to trigger MMGSDI to create a PKCS15 
  lookup table.
      
PROTOTYPE:
    int OEMCardSession_CreatePkcs15LookupTable(
                                  ICardSession                 *p,
                                  AEECardSessionCmdStatus      *pCmdStatus,
                                  AEECardSessionOption         *pOption,
                                  AEECallback                  *pUserCB)

PARAMETERS:
    *p           : [Input] ICardSession interface pointer
    *pCmdStatus  : [Input] Command Status
    *pOption     : [Input] Optional parameter pointer (currently not in use)
    *pUserCB     : [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS    - Status information is valid
  EBADPARM   - Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  None
=============================================================================*/
int OEMCardSession_CreatePkcs15LookupTable (
   ICardSession             *pMe,
   AEECardSessionCmdStatus  *pCmdStatus,
   AEECardSessionOption     *pOption,
   AEECallback              *pUserCB);


#endif /* OEMCardSession_H */

