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

$Header: //depot/asic/qsc60x0/apps/StaticExtensions/OEM/Inc/OEMCardSession.h#3 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
01/22/08     sun     Added ICardSession_Register API
10/02/07     vs      Featurized header decleration for "SysModelBase_priv.h"
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

#include "BREWVersion.h"

#include "AEE_OEMDispatch.h"
#ifdef WIN32
#undef FEATURE_MMGSDI
#endif
#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#include "tmc.h"
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

/* ICardSession object, instantiated every time */
struct ICardSession 
{
  union {
    AEEVTBL(ICardSession) *        pvt;
  } u;
  IShell *                         m_pIShell;
  uint32                           m_uRefs;
  AEECLSID                         m_cls;
  ICardSessionCore *               m_coreObj;    /* pointer to core object */
  void *                           m_pac;        /* App context */
  uint64                           m_client_id;
  OEMCardSessionChannelStatusEnum  m_byChannelStatus;
  OEMCardSessionSlotEnum           m_u8Slot;
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
 ICardSession *pMe);


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
  none

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
  none

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
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
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
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
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
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
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
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
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

FUNCTION: OEMCardSession_DisablePin

DESCRIPTION
  Disable the PIN for the particular application accessed by the 
  ICardSession object.
  
RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
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
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
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
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
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
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
int OEMCardSession_SelectSubscription(
  ICardSession                  *pMe, 
  const AEECardSessionSubData   *pSub,
  AEECardSessionCmdStatus       *pCmdStatus,
  AEECallback                   *pUserCB);


/*=============================================================================
FUNCTION: OEMCardSession_Register

DESCRIPTION:
  API to register for Client ID with MMGSDI.
  Every app will need to do a client registration of its own.

RETURN VALUE
  SUCCESS - Rquest sussessfully Queued
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCardSession_Register(
  ICardSession            *pMe,
  AEECardSessionCmdStatus *pCmdStatus,
  AEECallback             *pUserCB);

#endif /* OEMCardSession_H */
