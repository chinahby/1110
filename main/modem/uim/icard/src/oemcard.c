/*============================================================================
FILE:  OEMCard.c

SERVICES:  OEM functions for supporting BREW CARD Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW ICard interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

      OEMCard_AddRef            Adds a brew reference to the CARD object.
      OEMCard_Release           Removes a brew reference to the CARD object.
      OEMCard_QueryInterface    Returns a new interface, not yet supported.
      OEMCard_GetCardStatus     Gets the card status from OEMCard cache.
      OEMCard_GetPinStatus      Gets the PIN status from OEMCard cache.
      OEMCardGSDIcmdCB          Called by GSDI after a clients PIN request.
      OEMCardGSDIGenericcmdCB   Called by GSDI after a clients non PIN request.
      OEMCard_VerifyPin         Called to verify the PIN.
      OEMCard_ChangePin         Called to change the PIN.
      OEMCard_EnablePin         Called to enable the PIN.
      OEMCard_DisablePin        Called to disable the PIN.
      OEMCard_UnblockPin        Called to unblock the PIN.
      OEMCard_GetCardID         Called to get the SIM ID.
      OEMCard_GetCardApps       Gets the card application from GSDI
      OEMCard_GetCardStatusSlot Gets the card status from appropriate slot from OEMCard cache.
      OEMCard_EnableFDNSlot     Called to enable FDN
      OEMCard_DisableFDNSlot    Called to disable FDN
      OEMCard_GetSimCapabilitySlot  Called to get the capabilities of the card
      OEMCard_ChangeUserSlotPref    Called to change the slot preference for the indicated technology
      OEMCard_GetUserSlotPref       Called to get the user preferred slot
      OEMCard_GetATRSize            Called to get ATR size
      OEMCard_GetATR                Called to get ATR
      OEMCard_ActivatePersoFeatureInd   Called to activate the perso feature indicator
      OEMCard_DeactivatePersoFeatureInd Called to deactivate the perso feature indicator
      OEMCard_GetPersoFeatureInd        Called to get the perso feature indicator
      OEMCard_SetPersoFeatureData       Called to set the perso feature data
      OEMCard_GetPersoStatus            Returns the Perso Status
      OEMCard_GetServiceAvailableInd Called to get if the service is available or not

      OEMCardNotifier_AddRef   Adds a brew reference to the CARD object.
      OEMCardNotifier_Release  Removes a brew reference to the CARD object.
      OEMCardNotifier_SetMask  Called by Notifier interface.

      OEMCardConnection_New              Called to create new ICardConnection Object
      OEMCardConnection_AddRef           Called to add reference to ICardConnection Object
      OEMCardConnection_Release          Called to release the connection object
      OEMCardConnection_QueryInterface   Query interface fo ICardConnection
      OEMCardConnection_Open             Called to open a channel
      OEMCardConnection_Close            Called to close a channel
      OEMCardConnection_SendAPDU         Called to send APDU through a particular channel
      OEMCardConnection_GetAPDURsp       Called to retrieve data response from the send APDU


      INTERNAL FUNCTIONS:
      OEMCardNewCardStatus      Used when OEMCard informs of a new card status
      OEMCardNewPinStatus       Used when OEMCard informs of a new PIN status
      OEMCardGSDIGetPinStatusCB Called by MMGSDI after OEMCards PIN request.
      OEMCardGSDIevtCB          Called by MMGSDI whenever a SIM event occurs
      OEMCardNotifier_MergeMasks  Called by Setmask to merge all masks into one.
      OEMCardNewCardStatus_Slot2   Used when OEMCard informs of a new card status
      OEMCardGSDIGetATRCB          Called by MMGSDI after it processes the ATR request
      OEMCardConnectionGSDIcmdCB   Called by MMGSDI after any channel management
                                   aysnchronous processing
      OEMCardGSDIPersocmdCB        Called by MMGSDI after processing Personalization
                                   request
      OEMCardGSDIPersoevtCB        Called by MMGSDI when any Personalization event occurs
      OEMCardNewPersoStatus        Notifies registered client for new Personalization event
      OEMCard_MapSlot              Map ICard Slot to GSDI Slot
      OEMCard_MapPersoEnum         Map ICard Perso Feature Indicator to GSDI value
      OEMCard_MapGSDIFeatureToCardFeature  Map GSDI Perso Feature Indicator to ICard value
      OEMCard_MapService           Map AEECARD services to MMGSDI services
      OEMCard_MapNextServiceEnum   Map the additional services from AEECARD to MMGSDI if it is
                                   not a one to one mapping
      OEMCard_IsSpecialService     Check if the service requested required any special handling

        Copyright © 1999-2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/modem/uim/icard/rel/07H1_2/src/oemcard.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/09   rn      Added support for accessing EF-CFF - Call forwarding EF.
01/28/09   kk      Passing NULL to gsdi_sim_read for avoiding ERROR_FATAL
11/05/08   kk      Update pfnCancel pointer to NULL in the callback pointer
                   during Cancel callback.
10/20/08   tml     Added debug messages, additional check against valid handler
                   value before proceed with handling confirmations from gsdi
                   or mmgsdi 
10/01/08   nb      Lint fixes
05/30/08   vs      Changed pin status lock state to AEECARD_PIN_NOT_INIT
                   when PIN is not initialized
05/20/08   tml     Fixed compilation warning
03/02/08   nk      Added OMH EF Support
03/17/08   nk      Updated OEMCardConnection_SendAPDU to reset i if gone 
                   through all channels.
12/12/07   sun     Removed early cleanup.
11/13/07   sun     Added OMH feature support
11/13/07   sun     Set the ReqInProgress flag to FALSE regardless of the 
                   state of cached ATR , Reverted OMH
11/08/07   vs      Added OMH feature support
10/26/07   sun     Added new API to get the ATR asynchronously
09/19/07   sun     Added call to gsdi_get_atr when we recieve MMGSDI events
06/18/07   sun     Added Sap Callback to ICARD_MODEL as well
05/30/07   sun     Cancel all Callbacks on Release
05/02/07   sun     Removed handling of GSDI_SIM_INIT_COMPLETED
                   and added it to CARD_INIT_COMPLETED
04/30/07   sun     Fixed Compile Error
04/03/07   sun     Reset the initialized flag on ICard Release
03/24/07   sun     Get the PIN Information on CARD_INIT_COMPLETED if 
                   pin events have not been received already
03/14/07   sun     Moved call to gsdi_get_atr such that ATR will be 
                   available to ICard evenn if SIM_INIT_COMPLETED is not  
                   received.
02/21/07   sun     Handle MMGSDI PIN events and remove GSDI PIN handling
02/13/07   sun     Added Messages for SAP
01/30/07   tml     Send refresh complete if the file is not what ICard
                   interested in
01/18/07   sun     Fixed services type for CBMI
01/11/07   tml     Fix pin status population issue
10/06/06   tml     pin status for enable set to verified only upon a successful
                   verification
10/05/06   tml     pin status sets to VERIFIED upon a successful 
                   verification or unblock and lint
09/12/06   sun     Fixed Warning
08/25/06   jar     Added support for FEATURE_MMGSDI_CARD_ERROR_INFO
08/20/06   tml     Support for NAA App Reset and 3G Session Reset
08/09/06   sun     Assign the object Handler to the Cancel function 
07/26/06   tml     Compilation dependency and warning fixes
07/19/06   sun     Fixed Compile Problems for RPC builds
05/18/06   tml     fixed compilation warnings for non MMGSDI enabled build
05/03/06   sp      Updated MMGSDI pin status
05/01/06   tml     Freed Async Info in case of error condition
04/24/06   jk      Fixed featurization for non-RUIM SC-1X compile issue
04/20/06  nk/tml   back out pin state and fixed compilation warning
04/18/06   sp      Update mmgsdi pin state
04/06/06   sun     Added support for RUIM Lock
04/05/06   tml     Return valid Num Retries if Blocked
03/23/06   tml     Fixed compilation warning
03/11/06   tml     Merge changes from branch
03/08/06   tml     Fix potential out of synch issue with open and close
                   channel and lint fixes
03/07/06   jk      Compilation issue for merge to mainline for SC-1X
02/22/06   tml     Fixed compilation issue with FEATURE_MMGSDI undefined and
                   lint
02/16/06   sp      Support for EF_EHPLMN
02/10/06   tml     Fixed compilation warning
02/07/06   tml     ICard cleanup: potential memory leak, function return status
                   input parameter check
01/04/06   tml     Updated enum name for SAP
11/14/05   sun     Added BT State Check to ValidCardCheck as well
11/08/05   sun     Fixed Cmd Status in MMGSDICMDCb
11/03/05   sun     Added new Card Status for BT-Sap when BT is connected
10/28/05   sun     Fixed Callback for BT-SAP
10/21/05   tml     Fixed potential stuck issue with ICARDConnection when
                   Open Channel failed
10/20/05   sun     Added support for BT Deregisteration
10/18/05   sun     Added support for status events for BT SAP
10/07/05   tml     compilation fix for 1x target
09/15/05   tml     Perm Blocked pin status issue
08/27/05   tml     Compilation fix
08/27/05   tml     Compilation fix
08/26/05   tml     BREW 3.1.4 changes
08/26/05   sun     Added support for BT SAP
08/24/05   tml     Added SIM INIT + File Change Notification Event support
08/07/05   jar     Map GSDI_PERSO_PHONECODE_NOT_INIT to AEECARD_PIN_NOT_INIT
                   Fix to avoid overwriting Phone Code Status Rsp
                   Added support GSDI_PERSO_SET_PHONE_CODE_RSP
08/03/05   tml     Fixed for potential memory issue with Channel Management
07/21/05   tml     SAP support
06/17/05   sun     Fixed GSDI Enums
06/15/05   tml     Linuxify fix
06/09/05   tml     Added PUCT, onchip card and Illegal SIM supports
06/06/05   tml     Added Prop1 feature support
06/06/05   tml     Fixed compilation error
05/27/05   tml     Fixed memory leak
05/27/05   tml     SIM Lock phone code support
05/20/05   sun     Fixed Client Refresh commands.
05/11/05   sst     Lint fixes
05/08/05   tml     Removed bridlization feature
04/06/05   tml     fixed open channel response issue
03/30/05   tml     Fixed potential data abort
03/25/05   tml     Support for IModel in File Change Notification
03/21/05   tml     Returned BadParm if instruction class is invalid
03/17/05   ak      Added CPHS EONS Support
03/16/05   wli     Added 820B EFs
03/06/05   tml     Fixed nCmdStatus in genericcommand callback
02/28/05   sun     Added file mapping - MWIS/MBDN
02/18/05   sun     Fixed Compile Errors
01/31/05   tml     Added IModel support
01/30/05   tml     Java Channel Support changes
01/17/05   tml     Did get pin status when SIM_INIT_COMPLETED is received
                   if there was no SIM_INSERTED event being received prior
01/13/05   tml     Initialized CardStatus_2 to GSDI_NO_SIM_EVENT_2
01/07/04   sun     Fixed Compilation Warnings
12/02/04   tml     Fixed compilation issue for Channel Management
11/24/04   jar     Fixed Compile Error for Missing ")"
11/10/04   tml     Change release to return reference count
11/05/04   tml     Change return value info for ICARDCONNECTION_Release
09/28/04   tml     Fixed compilation issue with non RUIM build
09/01/04   tml     Added read/write support
08/25/04   tml     Channel Management changes
09/01/04   tml     Does not allow pin value beginning with 112 or 999 to be
                   sent to the card for CPHS
08/19/04   tml     Added OEMCard_GetServiceAvailableInd API support
08/05/04   tml     Fixed memory free during power down
07/30/04   tml     Added AEECLSID_QUERYINTERFACE in query interface
07/28/04   tml     Fixed memory free issue in OEMCard_Cancel
07/28/04   tml     Fixed link issue with Brew 3.0 build
07/28/04   ap      Moved OEMCard_GetPersoStatus() outside of F_MMGSDI for
                   compiling non-RUIM builds.
07/26/04   tml     BREW 3.0 compilation fix
07/26/04   tml     Fixed compiler issue
07/21/04   tml     Fixed memory allocation for object creation
07/15/04   tml     QIS recommended changes
06/19/04   tml     Changed SIM/USIM Personalization logic
06/10/04   tml     Fixed compilation issue for 6500
06/10/04   tml     Fixed compilation issue for 6500 BREW 3.0 build
06/10/04   tml     Sent out VERIFIED event when received the corresponding
                   event from MMGSDI
06/07/04   tml     Added SIM personalization support
05/19/04   tml     Fixed compilation error
05/18/04   tml     Memory protection fixes
05/14/04   tml     Channel Management support
03/25/04   ak      Compilation fix for BREW 3.0
03/16/04   tml     Added card status for reinit support for new dual mode
                   separate processing during pin verification
01/24/04   tml     Refresh dual slot support
12/22/03   tml     API change
12/08/03   tml     Postponed sending VALID Card until SIM Capability is returned
11/09/03   tml     Fixed null pointer
11/05/03   tml     Fixed compilation issue
11/04/03   tml     Added Pref Slot support
09/19/03   tml     Change me support for FDN to TRUE
08/20/03   tml     Update GSDI API changes
08/06/03   tml     Dual Slot support
07/18/03   tml     Featurized OEMCard_GetCardApps
07/17/03    AT     Fixed OEMCardVerifyPin to use the correct constant.
                   This fixes the problem with 8 char PINs.
07/16/03   tml     Fixed NewPinStatus notify function
03/31/03   tml     Added OEMCard_GetCardApps()
03/25/03    at     Changed the handling of the Pin Status to always reflect
                   the number of tries remaining.
02/27/03   jar     Added GSDI_REFRESH_SIM_RESET, GSDI_REFRESH_SIM_INIT,
                   and AEECARD_CARD_REINIT handling.
12/03/02   tml     Added GSDI_CARD_ERROR in  getcardstatus()
11/25/02   tml     Fixed compilation error for wcdma/gsm
10/29/02   tml     Add RTRE changes
10/03/02   tml     Add AEECARD_NO_CARD_NV_ONLY status and fix PIN_BLOCKED bug
09/11/02    at     Comments from code review.
08/23/02    at     Initial release.
=============================================================================*/

#include "comdef.h"     // Get DMSS type declarations.
#include "err.h"        // Error functions

#include "OEMHeap.h"
#include "OEMObjectMgr.h"
#include "AEE.h"         // BREW headers.
#include "AEECard.h"     // Header file for CARD interface
#include "AEEShell.h"
#include "AEE_OEM.h"
//#include "AEE_OEMDispatch.h"

#include "OEMFeatures.h"
#include "BREWVersion.h"

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#endif

#ifdef FEATURE_ICARD_IMODEL
#include "SysModelBase_priv.h"
#endif /* FEATURE_ICARD_IMODEL */

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#endif

#ifdef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS
#ifdef FEATURE_BREW_3_0
#include "UIMPRIV_GID.BID"
#endif
#endif

#include "BREWVersion.h"
#ifndef USES_MMI
#include "ui.h"
#if !defined(FEATURE_ICARD_NO_UI_BASE) && defined(FEATURE_UIM)
#include "ui_base.h"
#endif /* FEATURE_ICARD_NO_UI_BASE */
#else
#include "oemui.h"
#endif

#include "mmgsdilib.h"

#ifdef FEATURE_MMGSDI
#include "AEECriticalSection.h"
static AEECriticalSection gOEMCardCriticalSection={0};
#define CARD_ENTER_CRITICAL_SECTION    OEMCriticalSection_Enter(&gOEMCardCriticalSection); 
#define CARD_LEAVE_CRITICAL_SECTION    OEMCriticalSection_Leave(&gOEMCardCriticalSection);
#endif /*FEATURE_MMGSDI*/

//-----------------------------------------------------------------------------
//          Type Declarations
//-----------------------------------------------------------------------------
typedef struct ICardNotifier ICardNotifier;
#ifdef FEATURE_ICARD_IMODEL
typedef struct ICardModel ICardModel;
#endif /* FEATURE_ICARD_IMODEL */

/* The following structure is used to notify clients of a new card
 * status.  */
typedef struct OEMCardNotifier_notify_cb
{
  uint16 mask;
  void  *user_data;
} OEMCardNotifier_NOTIFY_CB;

typedef struct
{
  AEECallback        *pCB;
  void               *pClientData;
  AEEObjectHandle     Handler;
  void               *pObj;
} OEMCard_AsynCBInfo;

/* The following structure defines the interface functions to the
 * ICard interface.
 */
AEEINTERFACE(ICardNotifier)
{
  INHERIT_INotifier(ICardNotifier);
};
/*lint -esym(754, ICardNotifierVtbl::AddRef) */
/*lint -esym(754, ICardNotifierVtbl::Release) */
/*lint -esym(754, ICardNotifierVtbl::SetMask) */

typedef struct {
  uint8                        m_byCardStatus;/* SIM Status */
  uint8                        m_byPersoCardStatus;
  uint8                        m_byPersoCardStatus_2;
  uint8                        m_byCardStatus_2; /* SIM Status */
}ICardCore;


/* The following structure is for the ICard object when
 * it is allocated. */
struct ICard
{
   AEEVTBL(ICard)           *pvt;       /* Vector table of ICard methods. */
   IShell                   *m_pIShell; /* Pointer to the IShell object. */
   uint32                    m_uRefs;   /* Number of references to this obj */
   AEECLSID                  m_cls;     /* Class ID of this object. */
   AEECallback              *m_pcbExit;
   boolean                   m_bRefreshPending;
   uint32                    m_bNumFileRefreshed;
   ICardCore                 m_coreObj;    /* pointer to core object */
#ifdef FEATURE_ICARD_IMODEL
   OEMCardNotifier_NOTIFY_CB m_CardStatusData;
   AEECallback               m_oemCb;
   OEMCardNotifier_NOTIFY_CB m_CardStatusData_slot2;
   AEECallback               m_oemCb_slot2;
   OEMCardNotifier_NOTIFY_CB m_pinStatusData[AEECARD_MAXPIN];
   AEECallback               m_oemPINCb[AEECARD_MAXPIN];
   OEMCardNotifier_NOTIFY_CB m_PersoStatusData;
   AEECallback               m_oemPersoCb;
   OEMCardNotifier_NOTIFY_CB m_SapStatusData;
   AEECallback               m_oemSapCb;
#endif /* FEATURE_ICARD_IMODEL */
   mmgsdi_client_id_type     m_mmgsdi_client_id;
   mmgsdi_bt_bd_address_type m_sap_bd_addr;
};

/* The following structure is for the notifier object when
 * it is allocated. */
struct ICardNotifier
{
   AEEVTBL(ICardNotifier)   *pvt;       /* Vector table for notifier object. */
   IShell                   *m_pIShell; /* Pointer to the Ishell object. */
   uint32                    m_uRefs;    /* Number of Notifier references. */
   AEECLSID                  m_cls;      /* Class ID of this object. */
   uint32                    m_dwNotify; /* Merger of all notify masks. */
   struct ICard             *m_cardobj; /* Pointer to my card object. */
#ifndef FEATURE_ICARD_IMODEL
   OEMCardNotifier_NOTIFY_CB m_CardStatusData;
   AEECallback               m_oemCb;
   OEMCardNotifier_NOTIFY_CB m_CardStatusData_slot2;
   AEECallback               m_oemCb_slot2;
   OEMCardNotifier_NOTIFY_CB m_pinStatusData[AEECARD_MAXPIN];
   AEECallback               m_oemPINCb[AEECARD_MAXPIN];
   OEMCardNotifier_NOTIFY_CB m_PersoStatusData;
   AEECallback               m_oemPersoCb;
   OEMCardNotifier_NOTIFY_CB m_SapStatusData;
   AEECallback               m_oemSapCb;
#endif /* !FEATURE_ICARD_IMODEL */
   AEECallback               *m_pcbExit;
};
/*lint -esym(754, ICardNotifier::m_CardStatusData_slot2) */
/*lint -esym(754, ICardNotifier::m_oemCb_slot2) */


/* The following structure is for the ICardConnection object when
 * it is allocated. */
struct ICardConnection
{
   AEEVTBL(ICardConnection) *pvt;       /* Vector table of ICardConnection methods. */
   IShell                   *m_pIShell; /* Pointer to the IShell object. */
   uint32                    m_uRefs;    /* Number of references to this obj */
   AEECLSID                  m_cls;      /* Class ID of this object. */
   AEECallback              *m_pcbExit;
};

#ifdef FEATURE_ICARD_IMODEL
/* The following structure is for the model object when
 * it is allocated. */
struct ICardModel
{
   SysModelBase    base;
   AEEVTBL(IModel) vtModel;

   struct ICard             *m_cardobj; /* Pointer to my card object. */
   AEECallback              *m_pcbExit;
};
#endif /* FEATURE_ICARD_IMODEL */

#define OEMCARD_APDU_P1_INDEX      0x02
#define OEMCARD_APDU_CLASS_INDEX   0x00
#define OEMCARD_APDU_INST_INDEX    0x01
#define OEMCARD_APDU_LC_INDEX      0x04
#define OEMCARD_APDU_MIN_SELECT_BY_AID_APDU_SIZE 5
#define OEMCARD_APDU_SELECT_BY_AID 0x04

#define OEMCARD_MAX_RSP               20

#ifdef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS

#define OEMCARD_RID_SIZE 5
#define OEMCARD_APDU_SELECT_DATA_START_INDEX 5
uint8 oemcard_etsi_rid[OEMCARD_RID_SIZE] = {0xA0,0x00,0x00,0x00,0x09};
uint8 oemcard_pkcs_15_rid[OEMCARD_RID_SIZE] = {0xA0,0x00,0x00,0x00,0x63};
uint8 oemcard_global_platform_rid[OEMCARD_RID_SIZE] = {0xA0,0x00,0x00,0x01,0x51};

#endif /*FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS*/


//-----------------------------------------------------------------------------
//          Function Prototypes
//-----------------------------------------------------------------------------
/* Interface to the Notifier object. */
static uint32 OEMCardNotifier_AddRef( ICardNotifier *po);
static uint32 OEMCardNotifier_Release(ICardNotifier *po);
#if MIN_BREW_BUILD(3,1,4,7)
static void   OEMCardNotifier_SetMask(ICardNotifier *po, uint32 *pdwMask);
#else
static void   OEMCardNotifier_SetMask(ICardNotifier *po, const uint32 *pdwMask);
#endif // MIN_BREW_BUILD(3,1,4,7)

/* Interface to the Card object. */
static uint32 OEMCard_AddRef(                     ICard *po);
static uint32 OEMCard_Release(                    ICard *po);
static int    OEMCard_QueryInterface(             ICard *po, AEECLSID id, void **ppNew);
static int    OEMCard_GetCardStatus(              ICard *po, uint8 *pStatus);
static int    OEMCard_GetPinStatus(               ICard *po, uint8 pin_id, AEECardPinStatus *pPinStatus);
static int    OEMCard_VerifyPin(                  ICard *po, uint8 pin_id, AEECardPinCode *pPin, AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_UnblockPin(                 ICard *po, uint8 pin_id, AEECardPinCode *pPuk, AEECardPinCode *pPin, AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_ChangePin(                  ICard *po, uint8 pin_id, AEECardPinCode *pPin, AEECardPinCode *pNew_pin, AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_DisablePin(                 ICard *po, uint8 pin_id, AEECardPinCode *pPin, AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_EnablePin(                  ICard *po, uint8 pin_id, AEECardPinCode *pPin, AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_GetCardID(                  ICard *po, char *pID, int *pnLen);
static int    OEMCard_GetCardApps(                ICard *po, AEECardApp *pApp);
static int    OEMCard_GetCardStatusSlot(          ICard *po, uint8 wSlot, uint8 *pStatus);
static int    OEMCard_EnableFDNSlot(              ICard *po, uint8 wSlot, boolean bPresent, AEECardPinCode *pPin, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_DisableFDNSlot(             ICard *po, uint8 wSlot, boolean bPresent, AEECardPinCode *pPin, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_GetSimCapabilitySlot(       ICard *po, uint8 wSlot, AEESimCapabilities *pSimCap);
static int    OEMCard_ChangeUserSlotPref(         ICard *po, uint8 wNewSlot, uint8 wApp, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_GetUserSlotPref(            ICard *po, uint8 wApp, uint8 *wSlot);
static int    OEMCard_GetATRSize(                 ICard *po, uint8 wSlot, int *nSize);
static int    OEMCard_GetATR(                     ICard *po, uint8 wSlot, AEECardATR *pATR);
static int    OEMCard_ActivatePersoFeatureInd(    ICard *po, uint8 wSlot, int nFeatureID, AEECardPinCode *pKey, AEECardDeactivatePersoCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_DeactivatePersoFeatureInd(  ICard *po, uint8 wSlot, int nFeatureID, AEECardPinCode *pKey, AEECardDeactivatePersoCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_GetPersoFeatureInd(         ICard *po, uint8 wSlot, AEECardGetPersoFeatureCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_SetPersoFeatureData(        ICard *po, uint8 wSlot, AEECardPersoFeatureInfo *pFeatureInd, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_GetPersoStatus(             ICard *po, uint8 bySlot, uint8 *pPersoStatus);
static int    OEMCard_GetServiceAvailableInd(     ICard *po, uint8 bySlot, AEECardServiceEnumType dwService, AEECardServiceIndType *pSvcInd);
static int    OEMCard_GetFileAttributes(          ICard *po, uint8 bySlot, AEECardFilePathType sFilePath, AEECardFileAttribStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_ReadRecord(                 ICard *po, uint8 bySlot, AEECardFilePathType sFilePath, int nRecordNum, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_ReadBinary(                 ICard *po, uint8 bySlot, AEECardFilePathType sFilePath, int nStartOffset, int nNumBytes, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_WriteRecord(                ICard *po, uint8 bySlot, AEECardFilePathType sFilePath, int nRecordNum, AEECardDataBufType *pWrittenData, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_WriteBinary(                ICard *po, uint8 bySlot, AEECardFilePathType sFilePath, int nStartOffset, AEECardDataBufType *pWrittenData, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_GetCPHSInfo(                ICard *po, uint8 bySlot, AEECardCPHSStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_GetRespData(                ICard *po, uint32 dwSeqNum, AEECardDataBufType *pData);
static int    OEMCard_RegisterForFCN(             ICard *po,uint32 dwClsID,uint32 dwNumFiles,AEECardFileItemEnumType *pFileList);
static int    OEMCard_EncodePUCT(                 ICard *po, AEEPuctInfo *pInPUCTInfo, uint8 *pOutEncodedBuff);
static int    OEMCard_DecodePUCT(                 ICard *po, uint8 *pInEncodedBuff, AEEPuctInfo *pOutPUCTInfo);
static int    OEMCard_UnblockPersoDCK(            ICard *po, uint8 wSlot, int nFeatureID, AEECardPinCode *pKey, AEECardDeactivatePersoCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_GetPersoDCKNumRetries(      ICard *po, uint8 wSlot, AEECardDCKNumRetries *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_GetPhoneCodeStatus(         ICard *po, uint8 wSlot, AEECardPhoneCodeStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_SetPhoneCode(               ICard *po, AEECardPinCode *pPin, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCard_SAPConnect(                 ICard *po, uint8 wSlot, AEECardSAPMode SapMode, AEECardSAPCmdStatus *pSAPCmdStatus, AEECallback *pUserCB);
static int    OEMCard_SAPDisconnect(              ICard *po, uint8 wSlot, AEECardSAPMode SapMode, AEECardSAPDisconnectMode DisconnectMode, AEECardSAPCmdStatus *pSAPCmdStatus, AEECallback *pUserCB);
static int    OEMCard_SAPDeregister(              ICard *po, uint8 wSlot, AEECardSAPCmdStatus *pSAPCmdStatus, AEECallback *pUserCB);
static int    OEMCard_GetATRAsync(                ICard *po, uint8 wSlot, AEECardATR *pATR, AEECallback *pUserCB);

static int OEMCard_CardStatus(uint8 wSlot, uint8 *pbyCardStatus);
#ifdef FEATURE_MMGSDI
static void   OEMCardNewCardStatus(void);
static void   OEMCardNewPinStatus(uint8 pin_id, AEECardPinStatus *pin_status);
#endif /*FEATURE_MMGSDI */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
static void OEMCardNewCardStatus_Slot2(void);
#endif
#ifdef FEATURE_MMGSDI_PERSONALIZATION
static void OEMCardGSDIPersocmdCB(gsdi_cnf_T *gsdi_info);
static void OEMCardNewPersoStatus(uint8 bySlot);
static int OEMCard_MapGSDIFeatureToCardFeature(gsdi_perso_enum_type nGSDIPersoEnum, int *pCardPersoEnum);
#endif /*FEATURE_MMGSDI_PERSONALIZATION */


int    OEMCardConnection_New(           IShell *pIShell, AEECLSID cls, void **ppif);
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
static uint32 OEMCardConnection_AddRef(        ICardConnection *po);
static uint32 OEMCardConnection_Release(       ICardConnection *po);
static int    OEMCardConnection_QueryInterface(ICardConnection *po, AEECLSID id, void **ppNew);
static int    OEMCardConnection_Open(          ICardConnection *po, uint8 wSlotID, AEECardConnectionOpenStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCardConnection_Close(         ICardConnection *po, AEECardConnectionCloseStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCardConnection_SendAPDU(      ICardConnection *po, AEECardAPDUCmd *pAPDU, uint32 dwClientData, AEECardConnectionCmdStatus *pCmdStatus, AEECallback *pUserCB);
static int    OEMCardConnection_GetAPDURsp(    ICardConnection *po, uint32 dwSeqNum, AEECardAPDUResp *pAPDU);
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

/* The function supported by the notifier object. */
static const AEEVTBL(ICardNotifier) gOEMCardNotifierFuncs = {
   OEMCardNotifier_AddRef,
   OEMCardNotifier_Release,
   OEMCardNotifier_SetMask
};

/* The function supported by this object. */
static const AEEVTBL(ICard) gOEMCardFuncs = {
   OEMCard_AddRef,
   OEMCard_Release,
   OEMCard_QueryInterface,
   OEMCard_GetCardStatus,
   OEMCard_GetPinStatus,
   OEMCard_VerifyPin,
   OEMCard_UnblockPin,
   OEMCard_ChangePin,
   OEMCard_DisablePin,
   OEMCard_EnablePin,
   OEMCard_GetCardID,
   OEMCard_GetCardApps,
   OEMCard_GetCardStatusSlot,
   OEMCard_EnableFDNSlot,
   OEMCard_DisableFDNSlot,
   OEMCard_GetSimCapabilitySlot,
   OEMCard_ChangeUserSlotPref,
   OEMCard_GetUserSlotPref,
   OEMCard_GetATRSize,
   OEMCard_GetATR,
   OEMCard_ActivatePersoFeatureInd,
   OEMCard_DeactivatePersoFeatureInd,
   OEMCard_GetPersoFeatureInd,
   OEMCard_SetPersoFeatureData,
   OEMCard_GetPersoStatus,
   OEMCard_GetServiceAvailableInd,
   OEMCard_GetFileAttributes,
   OEMCard_ReadRecord,
   OEMCard_ReadBinary,
   OEMCard_WriteRecord,
   OEMCard_WriteBinary,
   OEMCard_GetCPHSInfo,
   OEMCard_GetRespData,
   OEMCard_RegisterForFCN,
   OEMCard_EncodePUCT,
   OEMCard_DecodePUCT,
   OEMCard_UnblockPersoDCK,
   OEMCard_GetPersoDCKNumRetries,
   OEMCard_GetPhoneCodeStatus,
   OEMCard_SetPhoneCode,
   OEMCard_SAPConnect,
   OEMCard_SAPDisconnect,
   OEMCard_SAPDeregister,
   OEMCard_GetATRAsync
};

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
/* The function supported by this object. */
static const AEEVTBL(ICardConnection) gOEMCardConnectionFuncs = {
   OEMCardConnection_AddRef,
   OEMCardConnection_Release,
   OEMCardConnection_QueryInterface,
   OEMCardConnection_Open,
   OEMCardConnection_Close,
   OEMCardConnection_SendAPDU,
   OEMCardConnection_GetAPDURsp
};
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

#ifdef FEATURE_MMGSDI
static void OEMCarddoNotify(OEMCardNotifier_NOTIFY_CB *pUser);
#endif /* FEATURE_MMGSDI */

//-----------------------------------------------------------------------------
//          Static variables
//-----------------------------------------------------------------------------
static struct ICard           *ICardobj            = NULL;
static struct ICardNotifier   *ICardNotifier_obj   = NULL;
#ifdef FEATURE_ICARD_IMODEL
static struct ICardModel      *ICardModel_obj      = NULL;
#endif /*FEATURE_ICARD_IMODEL */

/* Multiplier look-up table for DecodePUCT function. */
static const float power_of_ten_table[16] = {
   1,
   (float)0.1,
   (float)0.01,
   (float)0.001,
   (float)0.0001,
   (float)0.00001,
   (float)0.000001,
   (float)0.0000001,  // 10 ^ -7
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000    // 10 ^ 7
};

#ifdef FEATURE_MMGSDI

typedef struct
{
  boolean          bReady;
  AEECardATR       sATR;
  boolean          bReqInProgress;
} OEMCardATR;

static gsdi_sim_events_T            CardStatus = GSDI_NO_EVENT; /* SIM Status */

static AEESimCapabilities           sim_cap; /* sim capabilities. */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
static AEESimCapabilities           sim_cap_slot2; /* sim capabilities for slot 2 */
#endif
static boolean                      gInitialized = FALSE;
//Below are removed until they are necessary.
//static byte                     card_mode;     /* Current mode of the card. */

static AEECardPinStatus             pin1_status;   /* Is PIN1 enabled/etc? */
static AEECardPinStatus             pin2_status;   /* Is PIN2 enabled/etc? */
static AEECardPinStatus             sap_pin_status;  /*Is Sap Pin Enabled etc?*/
static AEECardSapStatus             sap_status;
static OEMCardATR                   ATR_Slot1;

#ifdef FEATURE_MMGSDI_PERSONALIZATION
static gsdi_perso_event_enum_type   PersoCardStatus;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
static gsdi_perso_event_enum_type   PersoCardStatus_2;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
#endif /*FEATURE_MMGSDI_PERSONALIZATION */

/* This structure stores the size and responses */
typedef struct
{
  int        nSize;
  uint8      *sRsp;
  uint32     dwConnObjIndex;
} OEMCardRspDataStruct;

static OEMCardRspDataStruct sCardRspTable[OEMCARD_MAX_RSP];

#endif /*FEATURE_MMGSDI */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
static gsdi_sim_events_T            CardStatus_Slot2 = GSDI_NO_SIM_EVENT_2; /* SIM Status */

static AEECardPinStatus             pin1_slot2_status;   /* Is PIN1 enabled/etc? */
static AEECardPinStatus             pin2_slot2_status;   /* Is PIN2 enabled/etc? */
static OEMCardATR                   ATR_Slot2;
#endif


typedef enum
{
  OEMCARD_CANCEL_REASON_CB_CANCEL,
  OEMCARD_CANCEL_REASON_RELEASE,
  OEMCARD_CANCEL_REASON_ENUM_MAX = 0x7FFFFFFF
}OEMCardCancelReason_Enum;

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
typedef enum
{
  OEMCARDCONNECTION_NONE     = 0,
  OEMCARDCONNECTION_OPENING  = 1,
  OEMCARDCONNECTION_OPENED   = 2,
  OEMCARDCONNECTION_SELECTED = 3,
  OEMCARDCONNECTION_CLOSED   = 4,
  OEMCARDCONNECTION_CHANNELSTATUS_ENUM_MAX = 0x7FFFFFFF
}OEMCardConnection_ChannelStatus_Enum;

typedef struct
{
  OEMCardConnection_ChannelStatus_Enum  bChannelStatus;
  ICardConnection                       *pICardConnection_obj;
  uint32                                dwChannelID;  /* == 0xFFFF if ICardConnection object has been created but
                                           open channel hasn't been called yet */
  uint8                                 wSlotID;
  OEMCardCancelReason_Enum              nRequestCancel;
} OEMCardConnectionMapStruct;

typedef struct
{
  int                                   index;
  AEEObjectHandle                       ObjHandler;
  uint32                                dwClientData;
  boolean                               bSelectAPDU;
  boolean                               bIsRelease;
}OEMCardConnection_client;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
#define OEMCARDCONNECTION_MAX_CHANNEL 8
#else
#define OEMCARDCONNECTION_MAX_CHANNEL 4
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
#define OEMCARDCONNECTION_MAX_RSP     20

static OEMCardConnectionMapStruct sCardChannelMapTable[OEMCARDCONNECTION_MAX_CHANNEL];
static OEMCardRspDataStruct sCardChannelRspTable[OEMCARDCONNECTION_MAX_RSP];

static void OEMCardConnectionGSDIcmdCB(gsdi_cnf_T *gsdi_info);
#endif /*FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

#define OEMCARD_MAX_PIN_RETRIES  3
// #define OEMCARD_MAX_PUK_RETRIES  10



#ifdef FEATURE_MMGSDI
typedef struct sCardFileChange{
  uint32 dwNumFiles;
  uint32 dwClsID;
  uim_items_enum_type pNotifyFileList[AEECARD_MAX_FILE_CHANGE_NOTIFICATIONS];
  struct sCardFileChange *next;
}OEMCardFileChangeNotify;


static OEMCardFileChangeNotify *pCardFileChangeNotify = NULL;
static OEMCardFileChangeNotify sTempFileChangeNotify = { 0 };

typedef struct
{
  AEECardFileItemEnumType nCardFile;
  gsdi_file_enum_type     nGSDIFile;
} OEMCardGSDIFileEnumTableType;

static OEMCardGSDIFileEnumTableType OEMCardGSDIFileEnumTable[] =
{
   /* No Other Special Sequencing here just add new ENUMs in Groups of Ten */
   /* EFs at the MF level */
  {AEECARD_ICCID,                            GSDI_ICCID},                            /* ICCID */
  {AEECARD_ELP,                              GSDI_ELP},                              /* Extended Language Preference */
  {AEECARD_DIR,                              GSDI_DIR},                              /* EF DIR for USIM */
  {AEECARD_ARR,                              GSDI_ARR},                              /* Access rule reference */

   /* EFs in CDMA DF*/
  {AEECARD_CDMA_CC,                          GSDI_CDMA_CC},                          /* Call Count */
  {AEECARD_CDMA_IMSI_M,                      GSDI_CDMA_IMSI_M},                      /* IMSI_M */
  {AEECARD_CDMA_IMSI_T,                      GSDI_CDMA_IMSI_T},                      /* IMSI_T */
  {AEECARD_CDMA_TMSI,                        GSDI_CDMA_TMSI},                        /* TMSI */
  {AEECARD_CDMA_ANALOG_HOME_SID,             GSDI_CDMA_ANALOG_HOME_SID},             /* Analog Home SID */
  {AEECARD_CDMA_ANALOG_OP_PARAMS,            GSDI_CDMA_ANALOG_OP_PARAMS},            /* Analog Operational Parameters */
  {AEECARD_CDMA_ANALOG_LOCN_AND_REGN_IND,    GSDI_CDMA_ANALOG_LOCN_AND_REGN_IND},    /* Analog Locn and Regn Indicators */
  {AEECARD_CDMA_HOME_SID_NID,                GSDI_CDMA_HOME_SID_NID},                /* CDMA Home SID and NID */
  {AEECARD_CDMA_ZONE_BASED_REGN_IND,         GSDI_CDMA_ZONE_BASED_REGN_IND},         /* CDMA Zone Based Regn Indicators */
  {AEECARD_CDMA_SYS_REGN_IND,                GSDI_CDMA_SYS_REGN_IND},                /* CDMA System/Network Regn Indicators */

  {AEECARD_CDMA_DIST_BASED_REGN_IND,         GSDI_CDMA_DIST_BASED_REGN_IND},         /* CDMA Distance Based Regn Indicators */
  {AEECARD_CDMA_ACCOLC,                      GSDI_CDMA_ACCOLC},                      /* Access Overload Class */
  {AEECARD_CDMA_CALL_TERM_MODE_PREF,         GSDI_CDMA_CALL_TERM_MODE_PREF},         /* Call Termination Mode Preferences */
  {AEECARD_CDMA_SCI,                         GSDI_CDMA_SCI},                         /* Suggested Slot Cycle Index */
  {AEECARD_CDMA_ANALOG_CHAN_PREF,            GSDI_CDMA_ANALOG_CHAN_PREF},            /* Analog Channel Preferences */
  {AEECARD_CDMA_PRL,                         GSDI_CDMA_PRL},                         /* Preferred Roaming List */
  {AEECARD_CDMA_RUIM_ID,                     GSDI_CDMA_RUIM_ID},                     /* Removable UIM ID */
  {AEECARD_CDMA_CDMA_SVC_TABLE,              GSDI_CDMA_CDMA_SVC_TABLE},              /* CDMA Service Table */
  {AEECARD_CDMA_SPC,                         GSDI_CDMA_SPC},                         /* Service Programming Code */
  {AEECARD_CDMA_OTAPA_SPC_ENABLE,            GSDI_CDMA_OTAPA_SPC_ENABLE},            /* OTAPA/SPC-Enable */

  {AEECARD_CDMA_NAM_LOCK,                    GSDI_CDMA_NAM_LOCK},                    /* NAM-Lock */
  {AEECARD_CDMA_OTASP_OTAPA_FEATURES,        GSDI_CDMA_OTASP_OTAPA_FEATURES},        /* OTASP-OTAPA Parameters */
  {AEECARD_CDMA_SERVICE_PREF,                GSDI_CDMA_SERVICE_PREF},                /* Service Preferences */
  {AEECARD_CDMA_ESN_ME,                      GSDI_CDMA_ESN_ME},                      /* ESN_ME */
  {AEECARD_CDMA_RUIM_PHASE,                  GSDI_CDMA_RUIM_PHASE},                  /* R-UIM Phase */
  {AEECARD_CDMA_PREF_LANG,                   GSDI_CDMA_PREF_LANG},                   /* Preferred Languages */
  {AEECARD_CDMA_UNASSIGNED_1,                GSDI_CDMA_UNASSIGNED_1},                /* Not assigned yet */
  {AEECARD_CDMA_SMS,                         GSDI_CDMA_SMS},                         /* Short Messages */
  {AEECARD_CDMA_SMS_PARAMS,                  GSDI_CDMA_SMS_PARAMS},                  /* Short Message Service Parameters */
  {AEECARD_CDMA_SMS_STATUS,                  GSDI_CDMA_SMS_STATUS},                  /* SMS Status */

  {AEECARD_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, GSDI_CDMA_SUP_SVCS_FEATURE_CODE_TABLE}, /* Sup Services Feature Code Table */
  {AEECARD_CDMA_UNASSIGNED_2,                GSDI_CDMA_UNASSIGNED_2},                /* Not assigned yet */
  {AEECARD_CDMA_HOME_SVC_PVDR_NAME,          GSDI_CDMA_HOME_SVC_PVDR_NAME},          /* CDMA Home Service Provider Name */
  {AEECARD_CDMA_UIM_ID_USAGE_IND,            GSDI_CDMA_UIM_ID_USAGE_IND},            /* R-UIM ID usage indicator */
  {AEECARD_CDMA_ADM_DATA,                    GSDI_CDMA_ADM_DATA},                    /* CDMA Administrative Data */
  {AEECARD_CDMA_MSISDN,                      GSDI_CDMA_MSISDN},                      /* Mobile Directory Number */
  {AEECARD_CDMA_MAXIMUM_PRL,                 GSDI_CDMA_MAXIMUM_PRL},                 /* Max size of the PRL */
  {AEECARD_CDMA_SPC_STATUS,                  GSDI_CDMA_SPC_STATUS},                  /* SPC Status */
  {AEECARD_CDMA_ECC,                         GSDI_CDMA_ECC},                         /* Emergency Call Codes */
  {AEECARD_CDMA_3GPD_ME3GPDOPC,              GSDI_CDMA_3GPD_ME3GPDOPC},              /* Operational Capabilities */

  {AEECARD_CDMA_3GPD_3GPDOPM,                GSDI_CDMA_3GPD_3GPDOPM},                /* Operational Mode */
  {AEECARD_CDMA_3GPD_SIPCAP,                 GSDI_CDMA_3GPD_SIPCAP},                 /* Simple IP Capabilities */
  {AEECARD_CDMA_3GPD_MIPCAP,                 GSDI_CDMA_3GPD_MIPCAP},                 /* Mobile IP Capabilities */
  {AEECARD_CDMA_3GPD_SIPUPP,                 GSDI_CDMA_3GPD_SIPUPP},                 /* Simple IP User Profile Parameters */
  {AEECARD_CDMA_3GPD_MIPUPP,                 GSDI_CDMA_3GPD_MIPUPP},                 /* Mobile IP User Profile Parameters */
  {AEECARD_CDMA_3GPD_SIPSP,                  GSDI_CDMA_3GPD_SIPSP},                  /* Simple IP Status Parameters */
  {AEECARD_CDMA_3GPD_MIPSP,                  GSDI_CDMA_3GPD_MIPSP},                  /* Mobile IP Status Parameters */
  {AEECARD_CDMA_3GPD_SIPPAPSS,               GSDI_CDMA_3GPD_SIPPAPSS},               /* Simple IP PAP SS Parameters */
  {AEECARD_CDMA_UNASSIGNED_3,                GSDI_CDMA_UNASSIGNED_3},                /* Reserved */
  {AEECARD_CDMA_UNASSIGNED_4,                GSDI_CDMA_UNASSIGNED_4},                /* Reserved */

  {AEECARD_CDMA_PUZL,                        GSDI_CDMA_PUZL},                        /* Preffered User Zone List */
  {AEECARD_CDMA_MAXPUZL,                     GSDI_CDMA_MAXPUZL},                     /* Maximum PUZL */
  {AEECARD_CDMA_MECRP,                       GSDI_CDMA_MECRP},                       /* ME-specific Config REquest Param */
  {AEECARD_CDMA_HRPDCAP,                     GSDI_CDMA_HRPDCAP},                     /* HRPD Access Auth Capability Param */
  {AEECARD_CDMA_HRPDUPP,                     GSDI_CDMA_HRPDUPP},                     /* HRPD Access Auth User Profile Param */
  {AEECARD_CDMA_CSSPR,                       GSDI_CDMA_CSSPR},                       /* CUR_SSPR_P_REV */
  {AEECARD_CDMA_ATC,                         GSDI_CDMA_ATC},                         /* Acceess Terminal Class */
  {AEECARD_CDMA_EPRL,                        GSDI_CDMA_EPRL},                        /* Extended Preffered Roaming List */
  {AEECARD_CDMA_BCSMS_CONFIG,                GSDI_CDMA_BCSMS_CONFIG},                /* Broadcast SMS Configuration */
  {AEECARD_CDMA_BCSMS_PREF,                  GSDI_CDMA_BCSMS_PREF},                  /* Broadcast SMS Preferences */

  {AEECARD_CDMA_BCSMS_TABLE,                 GSDI_CDMA_BCSMS_TABLE},                 /* Broadcast SMS Table */
  {AEECARD_CDMA_BCSMS_PARAMS,                GSDI_CDMA_BCSMS_PARAMS},                /* Broadcast SMS Paramaters */
  {AEECARD_CDMA_MMS_NOTIF,                   GSDI_CDMA_MMS_NOTIF},                   /* MMS Notification */
  {AEECARD_CDMA_MMS_EXT8,                    GSDI_CDMA_MMS_EXT8},                    /* MMS Extension 8 */
  {AEECARD_CDMA_MMS_ICP,                     GSDI_CDMA_MMS_ICP},                     /* MMS Issuer Connectivity Parameters */
  {AEECARD_CDMA_MMS_UP,                      GSDI_CDMA_MMS_UP},                      /* MMS User Preferences */
  {AEECARD_CDMA_SMS_CAP,                     GSDI_CDMA_SMS_CAP},                     /* SMS Capabilities */
  {AEECARD_CDMA_3GPD_IPV6CAP,                GSDI_CDMA_3GPD_IPV6CAP},                /* IPv6 Capabilities */
  {AEECARD_CDMA_3GPD_MIPFLAGS,               GSDI_CDMA_3GPD_MIPFLAGS},               /* Mobile IP Flags */
  {AEECARD_CDMA_3GPD_TCPCONFIG,              GSDI_CDMA_3GPD_TCPCONFIG},              /* TCP Configurations */

  {AEECARD_CDMA_3GPD_DGC,                    GSDI_CDMA_3GPD_DGC},                    /* Data Generic Configurations */
  {AEECARD_CDMA_BROWSER_CP,                  GSDI_CDMA_BROWSER_CP},                  /* Browser Connectivity Parameters */
  {AEECARD_CDMA_BROWSER_BM,                  GSDI_CDMA_BROWSER_BM},                  /* Browser Bookmarks */
  {AEECARD_CDMA_3GPD_SIPUPPEXT,              GSDI_CDMA_3GPD_SIPUPPEXT},              /* Simple IP User Profile Parameters Extension */
  {AEECARD_CDMA_MMS_CONFIG,                  GSDI_CDMA_MMS_CONFIG},                  /* MMS Configuration */
  {AEECARD_CDMA_JAVA_DURL,                   GSDI_CDMA_JAVA_DURL},                   /* Java Download URL */
  {AEECARD_CDMA_3GPD_MIPUPPEXT,              GSDI_CDMA_3GPD_MIPUPPEXT},              /* Mobile IP User Profile Parameters Extension */
  {AEECARD_CDMA_BREW_DLOAD,                  GSDI_CDMA_BREW_DLOAD},                  /* BREW Download */
  {AEECARD_CDMA_BREW_TSID,                   GSDI_CDMA_BREW_TSID},                   /* BREW Teleservice ID */
  {AEECARD_CDMA_BREW_SID,                    GSDI_CDMA_BREW_SID},                    /* BREW Subscriber ID */

  {AEECARD_CDMA_LBS_XCONFIG,                 GSDI_CDMA_LBS_XCONFIG},                 /* LBS XTRA Configuration */
  {AEECARD_CDMA_LBS_XSURL,                   GSDI_CDMA_LBS_XSURL},                   /* LBS XTRA Server URLs */
  {AEECARD_CDMA_LBS_V2CONFIG,                GSDI_CDMA_LBS_V2CONFIG},                /* LBS V2 Configuration */
  {AEECARD_CDMA_LBS_V2PDEADDR,               GSDI_CDMA_LBS_V2PDEADDR},               /* LBS V2 PDE Address */
  {AEECARD_CDMA_LBS_V2MPCADDR,               GSDI_CDMA_LBS_V2MPCADDR},               /* LBS V2 MPC Address */
  {AEECARD_CDMA_BREW_AEP,                    GSDI_CDMA_BREW_AEP},       	         /* BREW Application Exec. Policy */
  {AEECARD_CDMA_MODEL,                       GSDI_CDMA_MODEL},       	             /* Model Info */
  {AEECARD_CDMA_RC,                          GSDI_CDMA_RC},       	                 /* Root Certificates */
  {AEECARD_CDMA_APP_LABELS,                  GSDI_CDMA_APP_LABELS},                  /* Application Labels */
  {AEECARD_CDMA_USER_AGENT_STRING,           GSDI_CDMA_USER_AGENT_STRING},           /* KDDI Agent String */

  {AEECARD_CDMA_GID2,                        GSDI_CDMA_GID2},                        /* KDDI GID2 */
  {AEECARD_CDMA_GID1,                        GSDI_CDMA_GID1},                        /* KDDI GID1 */
  {AEECARD_CDMA_GID_REFERENCES,              GSDI_CDMA_GID_REFERENCES},              /* KDDI GID References */
  {AEECARD_CDMA_IMSI_STATUS,                 GSDI_CDMA_IMSI_STATUS},                 /* KDDI IMSI Status */
  {AEECARD_CDMA_ME_DOWNLOADABLE_DATA,        GSDI_CDMA_ME_DOWNLOADABLE_DATA},        /* KDDI ME Downloadable Data */
  {AEECARD_CDMA_ME_SETTING_DATA,             GSDI_CDMA_ME_SETTING_DATA},             /* KDDI ME Setting Date */
  {AEECARD_CDMA_ME_USER_DATA,                GSDI_CDMA_ME_USER_DATA},                /* KDDI User Data */
  {AEECARD_CDMA_RESERVED2,                   GSDI_CDMA_RESERVED2},                   /* KDDI Reserved */
  {AEECARD_CDMA_RESERVED1,                   GSDI_CDMA_RESERVED1},                   /* KDDI Reserved */
  {AEECARD_CDMA_UIM_SVC_TABLE,               GSDI_CDMA_UIM_SVC_TABLE},               /* KDDI UIM Service Table */

  {AEECARD_CDMA_UIM_FEATURES,                GSDI_CDMA_UIM_FEATURES},                /* KDDI UIM Features */
  {AEECARD_CDMA_UIM_VERSION,                 GSDI_CDMA_UIM_VERSION},                 /* KDDI UIM Version */
  {AEECARD_CDMA_HRPD_HRPDUPP,                GSDI_CDMA_HRPD_HRPDUPP},                /* HRPD User Profile Parameters */
  {AEECARD_CDMA_SF_EUIM_ID,                  GSDI_CDMA_SF_EUIM_ID},                  /* Short Form EUIMID */
  /* EFs in DF GSM */
  {AEECARD_GSM_LP,                           GSDI_GSM_LP},                           /* Language Preference */
  {AEECARD_GSM_IMSI,                         GSDI_GSM_IMSI},                         /* IMSI */
  {AEECARD_GSM_KC,                           GSDI_GSM_KC},                           /* Ciphering Key Kc */
  {AEECARD_GSM_PLMN,                         GSDI_GSM_PLMN},                         /* PLMN selector */
  {AEECARD_GSM_HPLMN,                        GSDI_GSM_HPLMN},                        /* HPLMN search period */
  {AEECARD_GSM_ACM_MAX,                      GSDI_GSM_ACM_MAX},                      /* ACM Maximum value */

  {AEECARD_GSM_SST,                          GSDI_GSM_SST},                          /* SIM Service table */
  {AEECARD_GSM_ACM,                          GSDI_GSM_ACM},                          /* Accumulated call meter */
  {AEECARD_GSM_GID1,                         GSDI_GSM_GID1},                         /* Group Identifier Level 1 */
  {AEECARD_GSM_GID2,                         GSDI_GSM_GID2},                         /* Group Identifier Level 2 */
  {AEECARD_GSM_SPN,                          GSDI_GSM_SPN},                          /* Service Provider Name */
  {AEECARD_GSM_PUCT,                         GSDI_GSM_PUCT},                         /* Price Per Unit and currency table */
  {AEECARD_GSM_CBMI,                         GSDI_GSM_CBMI},                         /* Cell broadcast message identifier selection */
  {AEECARD_GSM_BCCH,                         GSDI_GSM_BCCH},                         /* Broadcast control channels */
  {AEECARD_GSM_ACC,                          GSDI_GSM_ACC},                          /* Access control class */

  {AEECARD_GSM_FPLMN,                        GSDI_GSM_FPLMN},                        /* Forbidden PLMNs */
  {AEECARD_GSM_LOCI,                         GSDI_GSM_LOCI},                         /* Location information */
  {AEECARD_GSM_AD,                           GSDI_GSM_AD},                           /* Administrative data */
  {AEECARD_GSM_PHASE,                        GSDI_GSM_PHASE},                        /* Phase identification */
  {AEECARD_GSM_VGCS,                         GSDI_GSM_VGCS},                         /* Voice Group Call service */
  {AEECARD_GSM_VGCSS,                        GSDI_GSM_VGCSS},                        /* Voice Group Call service status */
  {AEECARD_GSM_VBS,                          GSDI_GSM_VBS},                          /* Voice Broadcast service  */
  {AEECARD_GSM_VBSS,                         GSDI_GSM_VBSS},                         /* Voice Broadcast service status */
  {AEECARD_GSM_EMLPP,                        GSDI_GSM_EMLPP},                        /* Enhanced multi level pre-emption and priority */
  {AEECARD_GSM_AAEM,                         GSDI_GSM_AAEM},                         /* Automatic Answer for eMLPP service */

  {AEECARD_GSM_CBMID,                        GSDI_GSM_CBMID},                        /* Cell Broadcast Message id for data dload */
  {AEECARD_GSM_ECC,                          GSDI_GSM_ECC},                          /* Emergency Call Codes */
  {AEECARD_GSM_CBMIR,                        GSDI_GSM_CBMIR},                        /* Cell Broadcast Message id range selection */
  {AEECARD_GSM_DCK,                          GSDI_GSM_DCK},                          /* De-personalization control keys */
  {AEECARD_GSM_CNL,                          GSDI_GSM_CNL},                          /* Co-operative network list */
  {AEECARD_GSM_NIA,                          GSDI_GSM_NIA},                          /* Network's indication of alerting */
  {AEECARD_GSM_KCGPRS,                       GSDI_GSM_KCGPRS},                       /* GPRS ciphering key */
  {AEECARD_GSM_LOCIGPRS,                     GSDI_GSM_LOCIGPRS},                     /* GPRS location information */
  {AEECARD_GSM_SUME,                         GSDI_GSM_SUME},                         /* Setup Menu elements */
  {AEECARD_GSM_PLMNWACT,                     GSDI_GSM_PLMNWACT},                     /* PLMN Selector with Access technology */
  {AEECARD_GSM_OPLMNWACT,                    GSDI_GSM_OPLMNWACT},                    /* Operator controlled PLMNWACT */

  {AEECARD_GSM_HPLMNACT,                     GSDI_GSM_HPLMNACT},                     /* HPLNMN Access technology */
  {AEECARD_GSM_CPBCCH,                       GSDI_GSM_CPBCCH},                       /* CPBCCH information */
  {AEECARD_GSM_INVSCAN,                      GSDI_GSM_INVSCAN},                      /* Investigation PLMN Scan */
  {AEECARD_GSM_RPLMNAT,                      GSDI_GSM_RPLMNAT},                      /* RPLMN  Last used Access Technology */
  {AEECARD_GSM_VMWI,                         GSDI_GSM_VMWI},                         /* CPHS: Voice Mail Waiting Indicator */
  {AEECARD_GSM_SVC_STR_TBL,                  GSDI_GSM_SVC_STR_TBL},                  /* CPHS: Service String Table */
  {AEECARD_GSM_CFF,                          GSDI_GSM_CFF},                          /* CPHS: Call Forwarding Flag */
  {AEECARD_GSM_ONS,                          GSDI_GSM_ONS},                          /* CPHS: Operator Name String */
  {AEECARD_GSM_CSP,                          GSDI_GSM_CSP},                          /* CPHS: Customer Service Profile */
  {AEECARD_GSM_CPHSI,                        GSDI_GSM_CPHSI},                        /* CPHS: CPHS Information */

  {AEECARD_GSM_MN,                           GSDI_GSM_MN},                           /* CPHS: Mailbox Number */
  {AEECARD_GSM_ORANGE_DFS,                   GSDI_GSM_ORANGE_DFS},                   /* Dynamic Flags Status */
  {AEECARD_GSM_ORANGE_D2FS,                  GSDI_GSM_ORANGE_D2FS},                  /* Dynamic2 Flag Setting */
  {AEECARD_GSM_ORANGE_CSP2,                  GSDI_GSM_ORANGE_CSP2},                  /* Customer Service Profile Line2*/
  /* EFs at SoLSA */
  {AEECARD_GSM_SAI,                          GSDI_GSM_SAI},                          /* SoLSA access indicator */
  {AEECARD_GSM_SLL,                          GSDI_GSM_SLL},                          /* SoLSA LSA list */
  /* EFs at MExE Level */
  {AEECARD_GSM_MEXE_ST,                      GSDI_GSM_MEXE_ST},                      /* MExE Service table */
  {AEECARD_GSM_ORPK,                         GSDI_GSM_ORPK},                         /* Operator Root Public Key */
  {AEECARD_GSM_ARPK,                         GSDI_GSM_ARPK},                         /* Administrator Root Public Key */
  {AEECARD_GSM_TPRPK,                        GSDI_GSM_TPRPK},                        /* Third party Root public key */

  {AEECARD_GSM_MBDN,                         GSDI_GSM_MBDN},                         /* Mailbox Dialing Number*/
  {AEECARD_GSM_EXT6,                         GSDI_GSM_EXT6},                          /* Extension 6*/
  {AEECARD_GSM_MBI,                          GSDI_GSM_MBI},                          /* Mail Box Identifier*/
  {AEECARD_GSM_MWIS,                         GSDI_GSM_MWIS},                         /* Message Waiting Indication Status*/
  {AEECARD_GSM_EXT1,                         GSDI_GSM_EXT1},                         /* Extension 1*/
  {AEECARD_GSM_SPDI,                         GSDI_GSM_SPDI},                         /* Service Provider Display Information*/
  {AEECARD_GSM_CFIS,                         GSDI_GSM_CFIS},                         /* GSM: Call Forwarding Indication status */
  /* image file */
  {AEECARD_GSM_IMG,                          GSDI_GSM_IMG},
  {AEECARD_GSM_IMG1INST1,                    GSDI_GSM_IMG1INST1},
  {AEECARD_GSM_IMG1INST2,                    GSDI_GSM_IMG1INST2},
  
  {AEECARD_GSM_IMG1INST3,                    GSDI_GSM_IMG1INST3},
  {AEECARD_GSM_IMG2INST1,                    GSDI_GSM_IMG2INST1},
  {AEECARD_GSM_IMG2INST2,                    GSDI_GSM_IMG2INST2},
  {AEECARD_GSM_IMG2INST3,                    GSDI_GSM_IMG2INST3},
  {AEECARD_GSM_IMG3INST1,                    GSDI_GSM_IMG1INST1},
  {AEECARD_GSM_IMG3INST2,                    GSDI_GSM_IMG2INST2},
  {AEECARD_GSM_IMG3INST3,                    GSDI_GSM_IMG3INST3},
  /* EFs at the telecom DF in GSM and CDMA */
  {AEECARD_TELECOM_ADN,                      GSDI_TELECOM_ADN},                      /* Abbrev Dialing Numbers */
  {AEECARD_TELECOM_FDN,                      GSDI_TELECOM_FDN},                      /* Fixed dialling numbers */
  {AEECARD_TELECOM_SMS,                      GSDI_TELECOM_SMS},                      /* Short messages */

  {AEECARD_TELECOM_CCP,                      GSDI_TELECOM_CCP},                      /* Capability Configuration Parameters */
  {AEECARD_TELECOM_ECCP,                     GSDI_TELECOM_ECCP},                     /* Extended CCP */
  {AEECARD_TELECOM_MSISDN,                   GSDI_TELECOM_MSISDN},                   /* MSISDN */
  {AEECARD_TELECOM_SMSP,                     GSDI_TELECOM_SMSP},                     /* SMS parameters */
  {AEECARD_TELECOM_SMSS,                     GSDI_TELECOM_SMSS},                     /* SMS Status */
  {AEECARD_TELECOM_LND,                      GSDI_TELECOM_LND},                      /* Last number dialled */
  {AEECARD_TELECOM_SDN,                      GSDI_TELECOM_SDN},                      /* Service Dialling numbers */
  {AEECARD_TELECOM_EXT1,                     GSDI_TELECOM_EXT1},                     /* Extension 1 */
  {AEECARD_TELECOM_EXT2,                     GSDI_TELECOM_EXT2},                     /* Extension 2 */
  {AEECARD_TELECOM_EXT3,                     GSDI_TELECOM_EXT3},                     /* Extension 3 */

  {AEECARD_TELECOM_BDN,                      GSDI_TELECOM_BDN},                      /* Barred Dialing Numbers */
  {AEECARD_TELECOM_EXT4,                     GSDI_TELECOM_EXT4},                     /* Extension 4 */
  {AEECARD_TELECOM_SMSR,                     GSDI_TELECOM_SMSR},                     /* SMS reports */
  {AEECARD_TELECOM_CMI,                      GSDI_TELECOM_CMI},                      /* Comparison Method Information */
  {AEECARD_TELECOM_SUME,                     GSDI_TELECOM_SUME},                     /* Setup Menu elements */
  {AEECARD_TELECOM_ARR,                      GSDI_TELECOM_ARR},                      /* Access Rule reference */
  /* EFs at DF GRAPHICS under DF Telecom */
  {AEECARD_IMAGE_FILE,                       GSDI_IMAGE_FILE},                       /* Image instance data files */
  /* EFs at the DF PHONEBOOK under  DF Telecom */
  {AEECARD_TELECOM_PBR,                      GSDI_TELECOM_PBR},                      /* Phone book reference file */
  {AEECARD_TELECOM_CCP1,                     GSDI_TELECOM_CCP1},                     /* Capability Configuration parameters 1 */
  {AEECARD_TELECOM_UID,                      GSDI_TELECOM_UID},                      /* Unique Identifier */

  {AEECARD_TELECOM_PSC,                      GSDI_TELECOM_PSC},                      /* Phone book synchronization center */
  {AEECARD_TELECOM_CC,                       GSDI_TELECOM_CC},                       /* Change counter */
  {AEECARD_TELECOM_PUID,                     GSDI_TELECOM_PUID},                     /* Previous Unique Identifier */
  /* EFs of USIM ADF */
  {AEECARD_USIM_LI,                          GSDI_USIM_LI},                          /* Language Indication */
  {AEECARD_USIM_IMSI,                        GSDI_USIM_IMSI},                        /* IMSI */
  {AEECARD_USIM_KEYS,                        GSDI_USIM_KEYS},                        /* Ciphering and Integrity keys */
  {AEECARD_USIM_KEYSPS,                      GSDI_USIM_KEYSPS},                      /* C and I keys for packet switched domain */
  {AEECARD_USIM_PLMNWACT,                    GSDI_USIM_PLMNWACT},                    /* User controlled PLMN selector with access tech */
  {AEECARD_USIM_UPLMNSEL,                    GSDI_USIM_UPLMNSEL},                    /* UPLMN selector */
  {AEECARD_USIM_HPLMN,                       GSDI_USIM_HPLMN},                       /* HPLMN search period */

  {AEECARD_USIM_ACM_MAX,                     GSDI_USIM_ACM_MAX},                     /* ACM maximum value */
  {AEECARD_USIM_UST,                         GSDI_USIM_UST},                         /* USIM Service table */
  {AEECARD_USIM_ACM,                         GSDI_USIM_ACM},                         /* Accumulated Call meter */
  {AEECARD_USIM_GID1,                        GSDI_USIM_GID1},                        /* Group Identifier Level  */
  {AEECARD_USIM_GID2,                        GSDI_USIM_GID2},                        /* Group Identifier Level 2 */
  {AEECARD_USIM_SPN,                         GSDI_USIM_SPN},                         /* Service Provider Name */
  {AEECARD_USIM_PUCT,                        GSDI_USIM_PUCT},                        /* Price Per Unit and Currency table */
  {AEECARD_USIM_CBMI,                        GSDI_USIM_CBMI},                        /* Cell Broadcast Message identifier selection */
  {AEECARD_USIM_ACC,                         GSDI_USIM_ACC},                         /* Access control class */

  {AEECARD_USIM_FPLMN,                       GSDI_USIM_FPLMN},                       /* Forbidden PLMNs */
  {AEECARD_USIM_LOCI,                        GSDI_USIM_LOCI},                        /* Location information */
  {AEECARD_USIM_AD,                          GSDI_USIM_AD},                          /* Administrative data */
  {AEECARD_USIM_CBMID,                       GSDI_USIM_CBMID},                       /* Cell Broadcast msg identifier for data download */
  {AEECARD_USIM_ECC,                         GSDI_USIM_ECC},                         /* Emergency call codes */
  {AEECARD_USIM_CBMIR,                       GSDI_USIM_CBMIR},                       /* Cell broadcast msg identifier range selection */
  {AEECARD_USIM_PSLOCI,                      GSDI_USIM_PSLOCI},                      /* Packet switched location information */
  {AEECARD_USIM_FDN,                         GSDI_USIM_FDN},                         /* Fixed dialling numbers */
  {AEECARD_USIM_SMS,                         GSDI_USIM_SMS},                         /* Short messages */
  {AEECARD_USIM_MSISDN,                      GSDI_USIM_MSISDN},                      /* MSISDN */

  {AEECARD_USIM_SMSP,                        GSDI_USIM_SMSP},                        /* SMS parameters */
  {AEECARD_USIM_SMSS,                        GSDI_USIM_SMSS},                        /* SMS Status */
  {AEECARD_USIM_SDN,                         GSDI_USIM_SDN},                         /* Service dialling numbers */
  {AEECARD_USIM_EXT2,                        GSDI_USIM_EXT2},                        /* Extension 2 */
  {AEECARD_USIM_EXT3,                        GSDI_USIM_EXT3},                        /* Extension 3 */
  {AEECARD_USIM_SMSR,                        GSDI_USIM_SMSR},                        /* SMS reports */
  {AEECARD_USIM_ICI,                         GSDI_USIM_ICI},                         /* Incoming call information */
  {AEECARD_USIM_OCI,                         GSDI_USIM_OCI},                         /* Outgoing call information */
  {AEECARD_USIM_ICT,                         GSDI_USIM_ICT},                         /* Incoming call timer */
  {AEECARD_USIM_OCT,                         GSDI_USIM_OCT},                         /* Outgoing call timer */

  {AEECARD_USIM_EXT5,                        GSDI_USIM_EXT5},                        /* Extension 5 */
  {AEECARD_USIM_CCP2,                        GSDI_USIM_CCP2},                        /* Capability Configuration Parameters 2 */
  {AEECARD_USIM_EMLPP,                       GSDI_USIM_EMLPP},                       /* Enhanced Multi Level Precedence and Priority */
  {AEECARD_USIM_AAEM,                        GSDI_USIM_AAEM},                        /* Automatic answer for eMLPP service */
  {AEECARD_USIM_GMSI,                        GSDI_USIM_GMSI},                        /* Group identity */
  {AEECARD_USIM_HIDDENKEY,                   GSDI_USIM_HIDDENKEY},                   /* key for hidden phonebook entries */
  {AEECARD_USIM_BDN,                         GSDI_USIM_BDN},                         /* Barred dialling numbers */
  {AEECARD_USIM_EXT4,                        GSDI_USIM_EXT4},                        /* Extension 4 */
  {AEECARD_USIM_CMI,                         GSDI_USIM_CMI},                         /* Comparison Method information */
  {AEECARD_USIM_EST,                         GSDI_USIM_EST},                         /* Enabled services table */

  {AEECARD_USIM_ACL,                         GSDI_USIM_ACL},                         /* Access Point Name Control List */
  {AEECARD_USIM_DCK,                         GSDI_USIM_DCK},                         /* De-personalization Control Keys */
  {AEECARD_USIM_CNL,                         GSDI_USIM_CNL},                         /* Co-operative network list */
  {AEECARD_USIM_START_HFN,                   GSDI_USIM_START_HFN},                   /* Init values for Hyper-frame number */
  {AEECARD_USIM_THRESHOLD,                   GSDI_USIM_THRESHOLD},                   /* Max value of START */
  {AEECARD_USIM_OPLMNWACT,                   GSDI_USIM_OPLMNWACT},                   /* Operator controlled PLMN sel with access tech */
  {AEECARD_USIM_OPLMNSEL,                    GSDI_USIM_OPLMNSEL},                    /* OPLMN selector */
  {AEECARD_USIM_HPLMNWACT,                   GSDI_USIM_HPLMNWACT},                   /* HPLMN selector with access tech */
  {AEECARD_USIM_ARR,                         GSDI_USIM_ARR},                         /* Access Rule reference */
  {AEECARD_USIM_RPLMNACT,                    GSDI_USIM_RPLMNACT},                    /* RPLMN last used access tech */

  {AEECARD_USIM_NETPAR,                      GSDI_USIM_NETPAR},                      /* Network parameters */
  {AEECARD_USIM_VMWI,                        GSDI_USIM_VMWI},                        /* CPHS: Voice Mail Waiting Indicator */
  {AEECARD_USIM_SVC_STR_TBL,                 GSDI_USIM_SVC_STR_TBL},                 /* CPHS: Service String Table */
  {AEECARD_USIM_CFF,                         GSDI_USIM_CFF},                         /* CPHS: Call Forwarding Flag */
  {AEECARD_USIM_ONS,                         GSDI_USIM_ONS},                         /* CPHS: Operator Name String */
  {AEECARD_USIM_CSP,                         GSDI_USIM_CSP},                         /* CPHS: Customer Service Profile */
  {AEECARD_USIM_CPHSI,                       GSDI_USIM_CPHSI},                       /* CPHS: CPHS Information */
  {AEECARD_USIM_MN,                          GSDI_USIM_MN},                          /* CPHS: Mailbox Number */
  /* EFs of ORANGE DF under USIM ADF */
  {AEECARD_USIM_ORANGE_DFS,                  GSDI_USIM_ORANGE_DFS},                  /* Dynamic Flags Status */
  {AEECARD_USIM_ORANGE_D2FS,                 GSDI_USIM_ORANGE_D2FS},                 /* Dynamic2 Flag Setting */

  {AEECARD_USIM_ORANGE_CSP2,                 GSDI_USIM_ORANGE_CSP2},                 /* Customer Service Profile Line2*/
  /*EFs at DF PHONEBOOK under USIM ADF */
  {AEECARD_USIM_PBR,                         GSDI_USIM_PBR},                         /* Phone book reference file */
  {AEECARD_USIM_CCP1,                        GSDI_USIM_CCP1},                        /* Capability Configuration parameters 1 */
  {AEECARD_USIM_UID,                         GSDI_USIM_UID},                         /* Unique Identifier */
  {AEECARD_USIM_PSC,                         GSDI_USIM_PSC},                         /* Phone book synchronization center */
  {AEECARD_USIM_CC,                          GSDI_USIM_CC},                          /* Change counter */
  {AEECARD_USIM_PUID,                        GSDI_USIM_PUID},                        /* Previous Unique Identifier */
  /*EFs at DF GSM under USIM ADF */
  {AEECARD_USIM_KC,                          GSDI_USIM_KC},                          /* GSM ciphering key Kc */
  {AEECARD_USIM_KCGPRS,                      GSDI_USIM_KCGPRS},                      /* GPRS ciphering key */
  {AEECARD_USIM_CPBCCH,                      GSDI_USIM_CPBCCH},                      /* CPBCCH information */

  {AEECARD_USIM_INVSCAN,                     GSDI_USIM_INVSCAN},                     /* Investigation scan */
  /*EFs at DF MEXe under USIM ADF */
  {AEECARD_USIM_MEXE_ST,                     GSDI_USIM_MEXE_ST},                     /* MExE Service table */
  {AEECARD_USIM_ORPK,                        GSDI_USIM_ORPK},                        /* Operator Root Public Key */
  {AEECARD_USIM_ARPK,                        GSDI_USIM_ARPK},                        /* Administrator Root Public Key */
  {AEECARD_USIM_TPRPK,                       GSDI_USIM_TPRPK},                       /* Third party Root public key */
  {AEECARD_USIM_MBDN,                        GSDI_USIM_MBDN},                        /* Mailbox Dialing Number*/
  {AEECARD_USIM_EXT6,                        GSDI_USIM_EXT6},                        /* Extension 6 */
  {AEECARD_USIM_MBI,                         GSDI_USIM_MBI},                         /* Mailbox Identifier*/
  {AEECARD_USIM_MWIS,                        GSDI_USIM_MWIS},                        /* Message Waiting Indication Status*/
  {AEECARD_USIM_SPDI,                        GSDI_USIM_SPDI},                        /* Service Provider Display Information*/

  {AEECARD_USIM_EHPLMN,                      GSDI_USIM_EHPLMN},                      /* Equivalent HPLMN */
  /* EFs of DCS1800 DF */
  {AEECARD_DCS1800_IMSI,                     GSDI_DCS1800_IMSI},                     /* Imsi */
  {AEECARD_DCS1800_KC,                       GSDI_DCS1800_KC},                       /* Ciphering Key Kc */
  {AEECARD_DCS1800_PLMN,                     GSDI_DCS1800_PLMN},                     /* PLMN selector */
  {AEECARD_DCS1800_SST,                      GSDI_DCS1800_SST},                      /* SIM Service table */
  {AEECARD_DCS1800_BCCH,                     GSDI_DCS1800_BCCH},                     /* Broadcast control channels */
  {AEECARD_DCS1800_ACC,                      GSDI_DCS1800_ACC},                      /* Access control class */
  {AEECARD_DCS1800_FPLMN,                    GSDI_DCS1800_FPLMN},                    /* Forbidden PLMNs */
  {AEECARD_DCS1800_LOCI,                     GSDI_DCS1800_LOCI},                     /* Location information */
  {AEECARD_DCS1800_AD,                       GSDI_DCS1800_AD},                       /* Administrative data */

  /* EFs selected by path at the telecom DF in GSM and CDMA */
  {AEECARD_TELECOM_IAP,                      GSDI_TELECOM_IAP},                      /* Index Administration File */
  {AEECARD_TELECOM_PBC,                      GSDI_TELECOM_PBC},                      /* Phone Book Control */
  {AEECARD_TELECOM_GRP,                      GSDI_TELECOM_GRP},                      /* Grouping File */
  {AEECARD_TELECOM_AAS,                      GSDI_TELECOM_AAS},                      /* Additional Number Alpha String */
  {AEECARD_TELECOM_GAS,                      GSDI_TELECOM_GAS},                      /* Grouping Information Alpha String */
  {AEECARD_TELECOM_ANR,                      GSDI_TELECOM_ANR},                      /* Additional Number */
  {AEECARD_TELECOM_SNE,                      GSDI_TELECOM_SNE},                      /* Secondary Name Entry */
  {AEECARD_TELECOM_EMAIL,                    GSDI_TELECOM_EMAIL},                    /* Email */
  {AEECARD_TELECOM_ANRA,                     GSDI_TELECOM_ANRA},                     /* Additional Number A */
  {AEECARD_TELECOM_ANRB,                     GSDI_TELECOM_ANRB},                     /* Additional Number B */

  {AEECARD_TELECOM_ANR1,                     GSDI_TELECOM_ANR1},                     /* Additional Number C */
  {AEECARD_TELECOM_ANRA1,                    GSDI_TELECOM_ANRA1},                    /* Additional Number A1 */
  {AEECARD_TELECOM_ANRB1,                    GSDI_TELECOM_ANRB1},                    /* Additional Number B1 */
  {AEECARD_TELECOM_ADN1,                     GSDI_TELECOM_ADN1},                     /* Abbreviated Dialing Numbers 1 */
  {AEECARD_TELECOM_PBC1,                     GSDI_TELECOM_PBC1},                     /* Phone Book Control 1 */
  {AEECARD_TELECOM_GRP1,                     GSDI_TELECOM_GRP1},                     /* Grouping File 1 */
  {AEECARD_TELECOM_SNE1,                     GSDI_TELECOM_SNE1},                     /* Secondary Name Entry 1 */
  {AEECARD_TELECOM_UID1,                     GSDI_TELECOM_UID1},                     /* Unique Identifier 1 */
  {AEECARD_TELECOM_EMAIL1,                   GSDI_TELECOM_EMAIL1},                   /* Email 1 */
  {AEECARD_TELECOM_IAP1,                     GSDI_TELECOM_IAP1},                     /* Index Administration File 1 */

  /* EFs selected by path of USIM ADF */
  {AEECARD_USIM_ADN,                         GSDI_USIM_ADN},                         /* Abbreviated Dialing Numbers */
  {AEECARD_USIM_IAP,                         GSDI_USIM_IAP},                         /* Index Administration File */
  {AEECARD_USIM_PBC,                         GSDI_USIM_PBC},                         /* Phone Book Control */
  {AEECARD_USIM_EXT1,                        GSDI_USIM_EXT1},                        /* Extension 1 */
  {AEECARD_USIM_GRP,                         GSDI_USIM_GRP},                         /* Grouping File */
  {AEECARD_USIM_AAS,                         GSDI_USIM_AAS},                         /* Additional Number Alpha String */
  {AEECARD_USIM_AAS1,                        GSDI_USIM_AAS1},                        /* Additional Number Alpha String 1 */
  {AEECARD_USIM_GAS,                         GSDI_USIM_GAS},                         /* Grouping Information Alpha String */
  {AEECARD_USIM_GAS1,                        GSDI_USIM_GAS1},                        /* Grouping Information Alpha String */
  {AEECARD_USIM_ANR,                         GSDI_USIM_ANR},                         /* Additional Number */

  {AEECARD_USIM_SNE,                         GSDI_USIM_SNE},                         /* Secondary Name Entry */
  {AEECARD_USIM_EMAIL,                       GSDI_USIM_EMAIL},                       /* Email */
  {AEECARD_USIM_ANRA,                        GSDI_USIM_ANRA},                        /* Additional Number A */
  {AEECARD_USIM_ANRB,                        GSDI_USIM_ANRB},                        /* Additional Number B */
  {AEECARD_USIM_ANRC,                        GSDI_USIM_ANRC},                        /* Additional Number C */
  {AEECARD_USIM_ANR1,                        GSDI_USIM_ANR1},                        /* Additional Number 1 */
  {AEECARD_USIM_ANRA1,                       GSDI_USIM_ANRA1},                       /* Additional Number A1 */
  {AEECARD_USIM_ANRB1,                       GSDI_USIM_ANRB1},                       /* Additional Number B1 */
  {AEECARD_USIM_ANRC1,                       GSDI_USIM_ANRC1},                       /* Additional Number C1 */
  {AEECARD_USIM_ADN1,                        GSDI_USIM_ADN1},                        /* Abbreviated Dialing Numbers 1 */

  {AEECARD_USIM_PBC1,                        GSDI_USIM_PBC1},                        /* Phone Book Control 1 */
  {AEECARD_USIM_GRP1,                        GSDI_USIM_GRP1},                        /* Grouping File 1 */
  {AEECARD_USIM_SNE1,                        GSDI_USIM_SNE1},                        /* Secondary Name Entry 1 */
  {AEECARD_USIM_UID1,                        GSDI_USIM_UID1},                        /* Unique Identifier 1 */
  {AEECARD_USIM_EMAIL1,                      GSDI_USIM_EMAIL1},                      /* Email 1 */
  {AEECARD_USIM_IAP1,                        GSDI_USIM_IAP1},                        /* Index Administration File 1 */
  /* EFs of WIM DF */
  {AEECARD_WIM_ODF,                          GSDI_WIM_ODF},                          /* WIM ODF */
  {AEECARD_WIM_TOKEN_INFO,                   GSDI_WIM_TOKEN_INFO},                   /* Generic token info and capabilities */
  {AEECARD_WIM_UNUSED_SPACE,                 GSDI_WIM_UNUSED_SPACE},                 /* Unused Space */
  /* EFs of ORANGE DF */
  {AEECARD_ORANGE_DFS,                       GSDI_ORANGE_DFS},                       /* Dynamic Flags Status */

  {AEECARD_ORANGE_D2FS,                      GSDI_ORANGE_D2FS},                      /* Dynamic2 Flag Setting */
  {AEECARD_ORANGE_CSP2,                      GSDI_ORANGE_CSP2},                      /* Customer Service Profile Line2*/
  {AEECARD_ORANGE_PARAMS,                    GSDI_ORANGE_PARAMS},                    /* Welcome Message*/
  {AEECARD_GSM_CINGULAR_ACT_HPLMN,           GSDI_CINGULAR_ACT_HPLMN},                /*Acting HPLMN*/
  {AEECARD_GSM_CINGULAR_SPT_TABLE,           GSDI_CINGULAR_SPT_TABLE},                /*Support Table*/
  {AEECARD_GSM_PNN,                          GSDI_GSM_PNN},                          /* CPHS: PLMN Name */
  {AEECARD_GSM_OPL,                          GSDI_GSM_OPL},                          /* CPHS: Operator List */
  {AEECARD_USIM_PNN,                         GSDI_USIM_PNN},                         /* CPHS: PLMN Network Name */
  {AEECARD_USIM_OPL,                         GSDI_USIM_OPL},                         /* CPHS: Operator List */
  {AEECARD_USIM_CFIS,                        GSDI_USIM_CFIS},                        /* Call Forwarding Indication status */
  
  {AEECARD_NO_SUCH_ITEM,                     GSDI_NO_SUCH_ITEM}                      /* No such item */
};

/*===========================================================================

FUNCTION: OEMCard_CleanUpAsynObject

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
static void OEMCard_CleanUpAsynObject(AEEObjectHandle Handler)
{
  OEMCard_AsynCBInfo *pAsynInfo = NULL;
  
  CARD_ENTER_CRITICAL_SECTION
  pAsynInfo = (OEMCard_AsynCBInfo*)AEEObjectMgr_GetObject(Handler);
  if (pAsynInfo == NULL)
  {
    /* Unregister the handler */
    (void)AEEObjectMgr_Unregister(Handler);
    CARD_LEAVE_CRITICAL_SECTION
    return;
  }

  if (pAsynInfo->pCB != NULL) {
    CALLBACK_Cancel((AEECallback*)pAsynInfo->pCB);
  }

  FREE(pAsynInfo);
  pAsynInfo = NULL;
  /* Unregister the handler */
  (void)AEEObjectMgr_Unregister(Handler);
  CARD_LEAVE_CRITICAL_SECTION
  /* The rest of the clean up is being taken care by OEMCard_Cancel */
} /* OEMCard_CleanUpAsynObject */

/*===========================================================================

FUNCTION: OEMCardSetPinStatus

DESCRIPTION
   Sets the global pin status given the pin id and the Pin data.
   Only applicable to mmgsdi pin data.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - if the pin Status is set successfully
  EFAILED  - otherwise

SIDE EFFECTS
  
===========================================================================*/
int OEMCard_SetPinStatus(
  AEECardPinStatus     *pPinStatus,
  mmgsdi_pin_info_type  pin_data)
{
  if(NULL == pPinStatus)
  {
    MSG_ERROR("_SetPinStatus: Pin Status Pointer is null",0,0,0);
    return EFAILED;
  }

  switch(pin_data.status) 
  {
    case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
      pPinStatus->lock_state = AEECARD_PIN_NOT_INIT;
      break;

    case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
      pPinStatus->lock_state = AEECARD_PIN_ENABLED;
      pPinStatus->tries_remaining = (uint8)pin_data.num_retries;
      break;

    case MMGSDI_PIN_UNBLOCKED:
    case MMGSDI_PIN_ENABLED_VERIFIED:
    case MMGSDI_PIN_CHANGED:
      pPinStatus->lock_state = AEECARD_PIN_VERIFIED;
      pPinStatus->tries_remaining = (uint8)pin_data.num_retries;
      break;

    case MMGSDI_PIN_DISABLED:
      pPinStatus->lock_state = AEECARD_PIN_DISABLED;
      pPinStatus->tries_remaining = (uint8)pin_data.num_retries;
      break;

    case MMGSDI_PIN_BLOCKED:
      pPinStatus->lock_state = AEECARD_PIN_BLOCKED;
      pPinStatus->tries_remaining = (uint8)pin_data.num_unblock_retries;
      break;

    case MMGSDI_PIN_PERM_BLOCKED:
      pPinStatus->lock_state = AEECARD_PIN_PERM_DISABLED;
      pPinStatus->tries_remaining = (uint8)pin_data.num_unblock_retries;
      break;

    default:
      MSG_HIGH("_SetPinStatus: Other PIN status 0x%x, not handled",
               pin_data.status,0,0 );
      break;
  }
  
  return SUCCESS;

}/*OEMCard_SetPinStatus*/


#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================

FUNCTION: OEMCardGSDIGetPinStatusCB

DESCRIPTION
   When OEMCard initializes, it needs to call GSDI Get Pin Status to get
   the latest PIN status.  It then stores this information in the cache.
   Since this is used at initialization, it will also keep track of a mask
   of outstanding PIN requests.  Once we get all the PIN responses, we
   send out the event to the registered clients that we are ready to
   go.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  The mask variable "card_init_waiting" is changed as we receive the PIN
  responses.  The variables pin1_status and pin2_status will be updated
  as those pin status responses are received.
  Also, a NOTIFY event will be generated to the clients once we receive
  the last Pin Status.
===========================================================================*/
static void OEMCardGSDIGetPinStatusCB(gsdi_cnf_T *sim_data)
{
  AEECardPinStatus *pPinStatus; /* Which pin are we working on. */
  uint8 pin_id;

  if (NULL == sim_data) {
    MSG_ERROR("GSDIGetPinStatusCB: Null Cnf Ptr", 0, 0, 0);
    return;
  }
  /* Figure out which pin we got.  Set the masks and a pointer
   * to the structure. */

  if (sim_data->pin_status_cnf.message_header.slot == GSDI_SLOT_1)
  {
    switch (sim_data->pin_status_cnf.pin_id)
    {
      case GSDI_PIN1:
        pPinStatus = &pin1_status;
        pin_id = AEECARD_PIN1;
        break;
      case GSDI_PIN2:
        pPinStatus = &pin2_status;
        pin_id = AEECARD_PIN2;
        break;
      default:
        MSG_ERROR("GSDIGetPinStatusCB: Unsupported PIN 0x%x", 
                  sim_data->pin_status_cnf.pin_id, 0, 0);
        return; /* Other pins are not supported yet. */
    }
  }
  else { /* Slot 2 */
    switch (sim_data->pin_status_cnf.pin_id)
    {
      case GSDI_PIN1:
        pPinStatus = &pin1_slot2_status;
        pin_id = AEECARD_PIN1_SLOT2;
        break;
      case GSDI_PIN2:
        pPinStatus = &pin2_slot2_status;
        pin_id = AEECARD_PIN2_SLOT2;
        break;
      default:
        MSG_ERROR("GSDIGetPinStatusCB: Unsupported PIN 0x%x", 
                  sim_data->pin_status_cnf.pin_id, 0, 0);
        return; /* Other pins are not supported yet. */
    }
  }

  /* If the command succeeded, we can set things accordingly. */
  if ( sim_data->message_header.gsdi_status == GSDI_SUCCESS )
  {
    switch(sim_data->pin_status_cnf.status) {
      case GSDI_PIN_NOT_INITIALISED:
        pPinStatus->lock_state = AEECARD_PIN_UNKNOWN;
        break;
      case GSDI_PIN_DISABLED:
        pPinStatus->lock_state = AEECARD_PIN_DISABLED;
        break;
      case GSDI_PIN_ENABLED:
        /* PIN is required. */
        if(pPinStatus->lock_state != AEECARD_PIN_VERIFIED)
        {
          pPinStatus->lock_state = AEECARD_PIN_ENABLED;
        }
        break;
      case GSDI_PIN_BLOCKED:
        /* Pin requires Unlock Key. */
        pPinStatus->lock_state = AEECARD_PIN_BLOCKED;
        break;
      case GSDI_PIN_PERM_BLOCKED:
        /* Pin cannot be Unblocked. */
        pPinStatus->lock_state = AEECARD_PIN_PERM_DISABLED;
        /* Set it to zero because GSDI returns Puk retries number when PIN is blocked */
        sim_data->pin_status_cnf.num_of_retries = 0;
        break;
      default:
        /* Pin is invalid */
        pPinStatus->lock_state = AEECARD_PIN_NOT_VALID;
        MSG_ERROR("GSDIGetPinStatusCB: Unknown pin_status_cnf.status 0x%x",
                  sim_data->pin_status_cnf.status, 0, 0);
        break;
    }
    /* set number of retries */
    pPinStatus->tries_remaining = sim_data->pin_status_cnf.num_of_retries;
    OEMCardNewPinStatus(pin_id, pPinStatus);
  }
  else
  {
    if (sim_data->pin_status_cnf.message_header.slot == GSDI_SLOT_1)
    {
      CardStatus = GSDI_CARD_ERROR;
    }
    else {
      CardStatus_Slot2 = GSDI_CARD_ERROR_2;
    }
    MSG_ERROR("FAILED GET PIN 0x%x STATUS with reason 0x%x",
              sim_data->pin_status_cnf.pin_id,
              sim_data->message_header.gsdi_status,
              0);
  }
} /* OEMCardGSDIGetPinStatusCB */
#endif/*FEATURE_MMGSDI_DUAL_SLOT*/

/*===========================================================================

FUNCTION: OEMCardGSDISimCapCB

DESCRIPTION
   When OEMCard initializes, it needs to call GSDI Get Sim Capabilities.
   It then stores this information in the cache.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/
static void OEMCardGSDISimCapCB(gsdi_cnf_T *sim_data)
{
  AEESimCapabilities *pSimCap = NULL;

  if (NULL == sim_data) {
    MSG_ERROR("GSDISimCapCB: sim_data Null", 0, 0, 0);
    return;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (sim_data->get_sim_cap_cnf.message_header.slot == GSDI_SLOT_1)
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  {
    pSimCap = &sim_cap;
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else { /* Slot 2 */
    pSimCap = &sim_cap_slot2;
  }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */


  /* If the command succeeded, we can set things accordingly. */
  if (sim_data->get_sim_cap_cnf.message_header.gsdi_status == GSDI_SUCCESS) {

    pSimCap->acl_enabled = sim_data->get_sim_cap_cnf.sim_capabilities.acl_enabled;
    pSimCap->bdn_enabled = sim_data->get_sim_cap_cnf.sim_capabilities.bdn_enabled;
    pSimCap->fdn_enabled = sim_data->get_sim_cap_cnf.sim_capabilities.fdn_enabled;
    pSimCap->imsi_invalidated = sim_data->get_sim_cap_cnf.sim_capabilities.imsi_invalidated;
    pSimCap->unknown_capabilities = sim_data->get_sim_cap_cnf.sim_capabilities.unknown_capabilities;
  }
  else
  {
    MSG_ERROR("FAILED GET SIM Capability with reason 0x%x",
              sim_data->message_header.gsdi_status, 0, 0);
    pSimCap->unknown_capabilities = TRUE;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (sim_data->get_sim_cap_cnf.message_header.slot == GSDI_SLOT_2) {
    OEMCardNewCardStatus_Slot2();
  }
  else
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  {
    OEMCardNewCardStatus();
  }
} /* OEMCardGSDISimCapCB */

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
/*===========================================================================

FUNCTION: OEMCardGSDIGetATRCB

DESCRIPTION
   When OEMCard initializes, it needs to call GSDI Get ATR.
   It then stores this information in the cache.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/
static void OEMCardGSDIGetATRCB(gsdi_cnf_T *sim_data)
{
  OEMCardATR *pAtr              = NULL;
  AEECardATR *pClientAtr        = NULL;
  AEEObjectHandle Handler;
  OEMCard_AsynCBInfo *pAsynInfo = NULL;

  if (!ICardobj) {
    MSG_ERROR("GSDIGetATRCB: ICardobj Null", 0, 0, 0);
    //UNLOCK MUTEX
    return;
  }

  if (NULL == sim_data) {
    MSG_ERROR("GSDIGetATRCB: sim_data Null", 0, 0, 0);
    return;
  }

  CARD_ENTER_CRITICAL_SECTION
  Handler= (AEEObjectHandle)sim_data->message_header.client_ref;
  if(Handler != 0) {
    pAsynInfo = (OEMCard_AsynCBInfo*)AEEObjectMgr_GetObject(Handler);
  }
  CARD_LEAVE_CRITICAL_SECTION

  if (NULL != pAsynInfo) {
    if (NULL == pAsynInfo->pCB) {
      OEMCard_CleanUpAsynObject(Handler);
      MSG_HIGH("OEMCardGSDIGetATRCB pAsynInfo->pCB Null", 0, 0, 0);
      return;
    }
    pClientAtr  = pAsynInfo->pClientData;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT

  if (sim_data->get_atr_cnf.message_header.slot == GSDI_SLOT_1) {
    pAtr = &ATR_Slot1;
  }
  else {
    pAtr = &ATR_Slot2;
  }
#else
  pAtr = &ATR_Slot1;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  
  /*Response REceived, reset the inProgress flag*/
  pAtr->bReqInProgress = FALSE;

  if (sim_data->get_atr_cnf.message_header.gsdi_status == GSDI_SUCCESS) {
    pAtr->bReady = TRUE;
    if (sim_data->get_atr_cnf.data_len > 0) {
      if(pAtr->sATR.pATR != NULL)
      {
         sys_free(pAtr->sATR.pATR);
      }
      pAtr->sATR.pATR = sys_malloc(sim_data->get_atr_cnf.data_len);
      if (pAtr->sATR.pATR == NULL)
      {
        MSG_ERROR("ATR Memory allocation fail", 0, 0, 0);
        return;
      }
      pAtr->sATR.nSize = sim_data->get_atr_cnf.data_len;
      (void)MEMCPY((uint8*)pAtr->sATR.pATR, sim_data->get_atr_cnf.data_buffer,
               pAtr->sATR.nSize);
    }

    if(pClientAtr != NULL)
    {
      pClientAtr->nSize = MIN(sim_data->get_atr_cnf.data_len,pClientAtr->nSize);
      (void)MEMCPY((uint8*)pClientAtr->pATR, sim_data->get_atr_cnf.data_buffer,
               pClientAtr->nSize);
    }
  }
  else
  {
    if(pClientAtr != NULL)
    {
      pClientAtr->nSize = 0;
    }
  }
    /* Validate user callback to ensure its still valid */
  if (pAsynInfo != NULL && pAsynInfo->pCB != NULL && pAsynInfo->pCB->pfnCancel != NULL) {
#if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback(pAsynInfo->pCB,  0);
#else
    AEE_ResumeEx(pAsynInfo->pCB, 0, 0);
#endif
  }
  else {
    MSG_ERROR("GSDIGetATRCB: pAsynInfo or ->pCB or ->pfnCancel NULL", 
              0, 0, 0 );
  }
} /* OEMCardGSDIGetATRCB */
#endif /*FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

/*===========================================================================

FUNCTION: OEMCardGSDIevtCB

DESCRIPTION
  This function is called by GSDI whenever there is a new event.  It basically
  will get the event, save the info in the appropriate status chache's and
  will notify the registered clients of the change.

  The following events are handled:
    GSDI_SIM_INSERTED - This event will trigger OEM Card to send out PIN
                        requests to GSDI.  It then sets the status variable
                        waiting mask to the list of PINs we are requesting.
                        Once the PINs are received, the event will be
                        generated to indicate we have a new card.
    GSDI_SIM_REMOVED -  This event triggers OEM to send out the Notification
                        that the card has been removed.  It clears out all
                        the cached info on this card.
    GSDI_DRIVER_ERROR-  These events indicate an error on the card and will
    GSDI_CARD_ERROR     cause the OEM layer to indicate the card is not usable.
                        These also cause the cached data to be removed.
    GSDI_SIM_INIT_COMPLETED
                        This event is the event indicating the SIM is usable
                        and we can start full service.
    GSDI_REFRESH_SIM_RESET
                        This event is the event indicating the SIM has been
                        reset as a result of a REFRESH Proactive Command with
                        the SIM RESET Command Qualifier.  A Warm RESET is
                        performed on the Card and all Card information must be
                        reset.
    GSDI_REFRESH_SIM_INIT
                        This event is the event indicating the SIM is about
                        to go through SIM Initialization again as a result
                        of a REFRESH Proactive Command with the SIM INIT
                        Command Qualifier.  Procedures after a PIN Verification
                        are executed again.  All PIN Information is unchanged.

    PIN1/PIN2 events:   GSDI_PIN1_BLOCKED GSDI_PIN2_BLOCKED
                        GSDI_PIN1_UNBLOCKED GSDI_PIN2_UNBLOCKED
                        GSDI_PIN1_VERIFIED GSDI_PIN2_VERIFIED
                        GSDI_PIN1_ENABLED GSDI_PIN2_ENABLED
                        GSDI_PIN1_DISABLED GSDI_PIN2_DISABLED
                        GSDI_PIN1_PERM_DISABLED GSDI_PIN2_PERM_DISABLED
                        These events occur when some application causes
                        the PIN status (either PIN1/PIN2) to change.
                        The cached copy of the variable is updated and
                        the notify event is sent.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  The mask variable "card_init_waiting" is changed as we send the PIN
  requests.  The variables pin1_status and pin2_status will be updated
  as the callback occurs.  Notify events are generated to the clients
  to indicate many of the handled events.
===========================================================================*/
static void OEMCardGSDIevtCB(gsdi_sim_events_T gsdi_event)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  gsdi_returns_T     gsdi_status = GSDI_SUCCESS;
  sim_capabilities_T dummy_sim_cap;
  gsdi_pin_status_T  pin_status_dummy;
  byte               num_retries_dummy;
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/
  
  MSG_HIGH("GSDIevtCB: Received gsdi_event 0x%x", gsdi_event, 0, 0);
  switch(gsdi_event) {
    case GSDI_SIM_INSERTED:
    case GSDI_APP_SELECTED:
      CardStatus = gsdi_event;
      OEMCardNewCardStatus();
      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
   case GSDI_SIM_INSERTED_2: /* received sim inserted for slot 2 */
   case GSDI_APP_SELECTED_2: /* received sim inserted for slot 2 */
      CardStatus_Slot2 = gsdi_event;

      /* set cache card mode value*/
      pin1_slot2_status.lock_state = AEECARD_PIN_UNKNOWN;
      pin1_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      pin2_slot2_status.lock_state = AEECARD_PIN_UNKNOWN;
      pin2_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;

      /* set cache pin status */
      gsdi_status = gsdi2_get_pin_status( GSDI_SLOT_2,
                            GSDI_PIN1,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin1 status 0x%x", gsdi_status, 0, 0);
      }
      gsdi_status = gsdi2_get_pin_status(GSDI_SLOT_2,
                            GSDI_PIN2,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin2 status 0x%x", gsdi_status, 0, 0);
      }
      OEMCardNewCardStatus_Slot2();

      break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

    case GSDI_SIM_REMOVED:
    case GSDI_NO_SIM:
#ifdef FEATURE_GSTK
    case GSDI_REFRESH_SIM_RESET:
    case GSDI_REFRESH_APP_RESET:
    case GSDI_REFRESH_3G_SESSION_RESET:
#endif
    case GSDI_CARD_ERR_POLL_ERROR:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET:
    case GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES:
    case GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES:
    case GSDI_CARD_ERR_VOLTAGE_MISMATCH:
    case GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS:
    case GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS:
    case GSDI_CARD_ERR_MAXED_PARITY_ERROR:
    case GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR:
    case GSDI_CARD_ERR_MAXED_OVERRUN_ERROR:
    case GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED:
    case GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION:
    case GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE:
    case GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE:
    case GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE:
    case GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE:
    case GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE:
    case  GSDI_DRIVER_ERROR:
    case  GSDI_CARD_ERROR:
      /* The card was removed. */
      CardStatus = gsdi_event;
      pin1_status.lock_state = AEECARD_PIN_UNKNOWN;
      pin1_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      pin2_status.lock_state = AEECARD_PIN_UNKNOWN;
      pin2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;

      ATR_Slot1.bReady = TRUE;
      ATR_Slot1.sATR.nSize = 0;

      /* This counts as an event to tell the clients. */
      OEMCardNewCardStatus();
      break;

   case GSDI_SIM_ILLEGAL:
      CardStatus = gsdi_event;
      OEMCardNewCardStatus();
      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
   case GSDI_SIM_ILLEGAL_2:
      CardStatus = gsdi_event;
      OEMCardNewCardStatus_Slot2();
      break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

#ifdef FEATURE_GSTK
   case  GSDI_REFRESH_SIM_INIT:
   case  GSDI_REFRESH_SIM_INIT_FCN:
     /* The SIM Initialization procs after a PIN    */
     /* verification are about to be ran            */
     /* No Need to redo PIN Verification...just     */
     /* wait for next event GSDI_SIM_INIT_COMPLETED */
     CardStatus = gsdi_event;

      /* This counts as an event to tell the clients. */
     OEMCardNewCardStatus();
     break;
#endif

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SIM_REMOVED_2:
#ifdef FEATURE_GSTK
    case GSDI_REFRESH_SIM_RESET_2:
    case GSDI_REFRESH_APP_RESET_2:
    case GSDI_REFRESH_3G_SESSION_RESET_2:
#endif /* FEATURE_GSTK */
    case GSDI_CARD_ERR_POLL_ERROR_2:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_2:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET_2:
    case GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES_2:
    case GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES_2:
    case GSDI_CARD_ERR_VOLTAGE_MISMATCH_2:
    case GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS_2:
    case GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_2:
    case GSDI_CARD_ERR_MAXED_PARITY_ERROR_2:
    case GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR_2:
    case GSDI_CARD_ERR_MAXED_OVERRUN_ERROR_2:
    case GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED_2:
    case GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION_2:
    case GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE_2:
    case GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE_2:
    case GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE_2:
    case GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE_2:
    case GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE_2:
    case GSDI_CARD_ERR_POLL_ERROR_2:
    case GSDI_CARD_ERROR_2:
      CardStatus_Slot2 = gsdi_event;

      pin1_slot2_status.lock_state = AEECARD_PIN_UNKNOWN;
      pin1_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      pin2_slot2_status.lock_state = AEECARD_PIN_UNKNOWN;
      pin2_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;

      ATR_Slot2.bReady = TRUE;
      ATR_Slot2.sATR.nSize = 0;

      /* This counts as an event to tell the clients. */
      OEMCardNewCardStatus_Slot2();
      break;

#ifdef FEATURE_GSTK
   case  GSDI_REFRESH_SIM_INIT_2:
   case  GSDI_REFRESH_SIM_INIT_FCN_2:
     /* The SIM Initialization procs after a PIN    */
     /* verification are about to be ran            */
     /* No Need to redo PIN Verification...just     */
     /* wait for next event GSDI_SIM_INIT_COMPLETED */
     CardStatus_Slot2 = gsdi_event;

      /* This counts as an event to tell the clients. */
     OEMCardNewCardStatus_Slot2();
     break;
#endif
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

    case  GSDI_MEMORY_WARNING:
    case  GSDI_NO_SIM_EVENT:
      /* Ignore. */
      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SIM_INIT_COMPLETED_NO_PROV:
      {
        uint8 card_status;

        if ((SUCCESS == OEMCard_CardStatus(AEECARD_SLOT1, &card_status)) &&
            (AEECARD_NOT_READY == card_status))
        {
          gsdi_status = gsdi_get_pin_status(GSDI_PIN1,
                     &pin_status_dummy,
                     &num_retries_dummy,
                     0,
                     OEMCardGSDIGetPinStatusCB);
          if (gsdi_status != GSDI_SUCCESS)
          {
            MSG_ERROR("Unable to queue slot1 get pin1 status 0x%x", gsdi_status, 0, 0);
          }
          gsdi_status = gsdi_get_pin_status(GSDI_PIN2,
                     &pin_status_dummy,
                     &num_retries_dummy,
                     0,
                     OEMCardGSDIGetPinStatusCB);
          if (gsdi_status != GSDI_SUCCESS)
          {
            MSG_ERROR("Unable to queue slot1 get pin2 status 0x%x", gsdi_status, 0, 0);
          }
        }

        CardStatus = gsdi_event;

        /* set cache sim capability info */
        (void)gsdi_get_sim_capabilities(&dummy_sim_cap, 0, OEMCardGSDISimCapCB);

        if (pin1_status.lock_state == AEECARD_PIN_ENABLED)
        {
          pin1_status.lock_state = AEECARD_PIN_VERIFIED;
          gsdi_status = gsdi_get_pin_status(GSDI_PIN1,
                            &pin_status_dummy,
                            &num_retries_dummy,
                            0,
                            OEMCardGSDIGetPinStatusCB);
          if (gsdi_status != GSDI_SUCCESS)
          {
            MSG_ERROR("Unable to queue slot1 get pin1 status 0x%x", gsdi_status, 0, 0);
          }
        }
        break;
      }

    case GSDI_SIM_INIT_COMPLETED_2:
    case GSDI_SIM_INIT_COMPLETED_NO_PROV_2:
      {
        uint8 card_status;

        if ((SUCCESS == OEMCard_CardStatus(AEECARD_SLOT2, &card_status)) &&
            (AEECARD_NOT_READY == card_status))
        {
          gsdi_status = gsdi2_get_pin_status(GSDI_SLOT_2,
                      GSDI_PIN1,
                      NULL,
                      0,
                      OEMCardGSDIGetPinStatusCB);
          if (gsdi_status != GSDI_SUCCESS)
          {
            MSG_ERROR("Unable to queue slot2 get pin1 status 0x%x", gsdi_status, 0, 0);
          }
          gsdi_status = gsdi2_get_pin_status(GSDI_SLOT_2,
                      GSDI_PIN2,
                      NULL,
                      0,
                      OEMCardGSDIGetPinStatusCB);
          if (gsdi_status != GSDI_SUCCESS)
          {
            MSG_ERROR("Unable to queue slot2 get pin2 status 0x%x", gsdi_status, 0, 0);
          }
        }

        CardStatus_Slot2 = gsdi_event;
        /* set cache sim capability info */
        (void)gsdi2_get_sim_capabilities(GSDI_SLOT_2, NULL, 0, OEMCardGSDISimCapCB);

        if (pin1_slot2_status.lock_state == AEECARD_PIN_ENABLED)
        {
          pin1_slot2_status.lock_state = AEECARD_PIN_VERIFIED;
          gsdi_status = gsdi2_get_pin_status(GSDI_SLOT_2,
                            GSDI_PIN1,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
          if (gsdi_status != GSDI_SUCCESS)
          {
            MSG_ERROR("Unable to queue slot2 get pin1 status 0x%x", gsdi_status, 0, 0);
          }
        }
        break;
      }
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/

   
    case GSDI_FDN_ENABLE:
      sim_cap.fdn_enabled = TRUE;
      break;

    case GSDI_FDN_DISABLE:
      sim_cap.fdn_enabled = FALSE;
      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_FDN_ENABLE_2:
      sim_cap_slot2.fdn_enabled = TRUE;
      break;

    case GSDI_FDN_DISABLE_2:
      sim_cap_slot2.fdn_enabled = FALSE;
      break;

     /* set the cache pin status variable to the corresponding states */
    case GSDI_PIN1_BLOCKED_2:
      pin1_slot2_status.lock_state = AEECARD_PIN_BLOCKED;
      pin1_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      /* set cache pin status */
      gsdi_status = gsdi2_get_pin_status( GSDI_SLOT_2,
                            GSDI_PIN1,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin1 status 0x%x", gsdi_status, 0, 0);
      }
      break;

    case GSDI_PIN2_BLOCKED_2:
      pin2_slot2_status.lock_state = AEECARD_PIN_BLOCKED;
      pin2_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      gsdi_status = gsdi2_get_pin_status( GSDI_SLOT_2,
                            GSDI_PIN2,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin2 status 0x%x", gsdi_status, 0, 0);
      }
      break;

    case GSDI_PIN1_UNBLOCKED_2:
      pin1_slot2_status.lock_state = AEECARD_PIN_VERIFIED;
      pin1_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      gsdi_status = gsdi2_get_pin_status( GSDI_SLOT_2,
                            GSDI_PIN1,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin1 status 0x%x", gsdi_status, 0, 0);
      }
      break;

    case GSDI_PIN2_UNBLOCKED_2:
      pin2_slot2_status.lock_state = AEECARD_PIN_VERIFIED;
      pin2_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      gsdi_status = gsdi2_get_pin_status( GSDI_SLOT_2,
                            GSDI_PIN2,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin2 status 0x%x", gsdi_status, 0, 0);
      }
      break;


    case GSDI_PIN1_VERIFIED_2:
      if (pin1_slot2_status.lock_state != AEECARD_PIN_VERIFIED)
      {
        pin1_slot2_status.lock_state = AEECARD_PIN_VERIFIED;
        pin1_slot2_status.tries_remaining = OEMCARD_MAX_PIN_RETRIES;
      }
      gsdi_status = gsdi2_get_pin_status( GSDI_SLOT_2,
                            GSDI_PIN1,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin1 status 0x%x", gsdi_status, 0, 0);
      }
      OEMCardNewPinStatus(AEECARD_PIN1_SLOT2, &pin1_slot2_status);
      break;

    case GSDI_PIN2_VERIFIED_2:
      if (pin2_slot2_status.lock_state != AEECARD_PIN_VERIFIED)
      {
        pin2_slot2_status.lock_state = AEECARD_PIN_VERIFIED;
        pin2_slot2_status.tries_remaining = OEMCARD_MAX_PIN_RETRIES;
      }
      gsdi_status = gsdi2_get_pin_status( GSDI_SLOT_2,
                            GSDI_PIN2,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin1 status 0x%x", gsdi_status, 0, 0);
      }
      OEMCardNewPinStatus(AEECARD_PIN2_SLOT2, &pin2_slot2_status);
      break;

    case GSDI_PIN1_ENABLED_2:
      if ((pin1_slot2_status.lock_state == AEECARD_PIN_DISABLED) ||
          (pin1_slot2_status.lock_state == AEECARD_PIN_VERIFIED)) {
        pin1_slot2_status.lock_state = AEECARD_PIN_VERIFIED;
      }
      else {
        pin1_slot2_status.lock_state = AEECARD_PIN_ENABLED;
      }
      pin1_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      gsdi_status = gsdi2_get_pin_status( GSDI_SLOT_2,
                            GSDI_PIN1,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin1 status 0x%x", gsdi_status, 0, 0);
      }
      break;

    case GSDI_PIN2_ENABLED_2:
      if ((pin2_slot2_status.lock_state == AEECARD_PIN_DISABLED) ||
          (pin2_slot2_status.lock_state == AEECARD_PIN_VERIFIED)) {
        pin2_slot2_status.lock_state = AEECARD_PIN_VERIFIED;
      }
      else {
        pin2_slot2_status.lock_state = AEECARD_PIN_ENABLED;
      }
      pin2_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      gsdi_status = gsdi2_get_pin_status( GSDI_SLOT_2,
                            GSDI_PIN2,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin2 status 0x%x", gsdi_status, 0, 0);
      }
      break;

    case GSDI_PIN1_DISABLED_2:
      pin1_slot2_status.lock_state = AEECARD_PIN_DISABLED;
      pin1_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      gsdi_status = gsdi2_get_pin_status( GSDI_SLOT_2,
                            GSDI_PIN1,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin1 status 0x%x", gsdi_status, 0, 0);
      }
      break;

    case GSDI_PIN2_DISABLED_2:
      pin2_slot2_status.lock_state = AEECARD_PIN_DISABLED;
      pin2_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      gsdi_status = gsdi2_get_pin_status( GSDI_SLOT_2,
                            GSDI_PIN2,
                            NULL,
                            0,
                            OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin2 status 0x%x", gsdi_status, 0, 0);
      }
      break;

    case GSDI_PIN1_PERM_BLOCKED_2:
      pin1_slot2_status.lock_state = AEECARD_PIN_PERM_DISABLED;
      pin1_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      gsdi_status = gsdi2_get_pin_status(GSDI_SLOT_2,
                          GSDI_PIN1,
                          NULL,
                          0,
                          OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin1 status 0x%x", gsdi_status, 0, 0);
      }
      break;

    case GSDI_PIN2_PERM_BLOCKED_2:
      pin2_slot2_status.lock_state = AEECARD_PIN_PERM_DISABLED;
      pin2_slot2_status.tries_remaining = AEECARD_UNKNOWN_TRIES;
      gsdi_status = gsdi2_get_pin_status(GSDI_SLOT_2,
                          GSDI_PIN2,
                          NULL,
                          0,
                          OEMCardGSDIGetPinStatusCB);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MSG_ERROR("Unable to queue slot2 get pin2 status 0x%x", gsdi_status, 0, 0);
      }
      break;

#endif /*FEATURE_MMGSDI_DUAL_SLOT */

    default:
      break;
  }

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
  if (!ATR_Slot1.bReady && GSDI_NO_EVENT != CardStatus &&
      !ATR_Slot1.bReqInProgress)
  {
    (void)gsdi_sim_get_atr(GSDI_SLOT_1, 0, OEMCardGSDIGetATRCB);
     ATR_Slot1.bReqInProgress = TRUE;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (!ATR_Slot2.bReady && GSDI_NO_SIM_EVENT_2 != CardStatus_Slot2  &&
      !ATR_Slot2.bReqInProgress)
  {
    (void)gsdi_sim_get_atr(GSDI_SLOT_2, 0, OEMCardGSDIGetATRCB);
     ATR_Slot2.bReqInProgress = TRUE;
  }
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/

#endif /*FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

} /* OEMCardGSDIevtCB */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================

FUNCTION: OEMCardGSDIPersoevtCB

DESCRIPTION
  This function is called by GSDI whenever there is a new Perso event.
  It basically will get the event, save the info in the appropriate status
  cache's and will notify the registered clients of the change.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  Notify events are generated to the clients to indicate many of the
  handled events.
===========================================================================*/
static void OEMCardGSDIPersoevtCB (
    gsdi_perso_event_enum_type        event,
    gsdi_slot_id_type                 slot,
    boolean                           additional_info_avail,
    gsdi_perso_additional_info_type * info )
{
  MSG_HIGH("GSDIPersoevtCB: Received perso_event 0x%x", event, 0, 0);
  if (slot == GSDI_SLOT_1) {
    PersoCardStatus = event;
    OEMCardNewPersoStatus(AEECARD_SLOT1);
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else if (slot == GSDI_SLOT_2) {
    PersoCardStatus_2 = event;
    OEMCardNewPersoStatus(AEECARD_SLOT2);
  }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
} /* OEMCardGSDIPersoevtCB*/ /*lint !e715 */

#endif /*FEATURE_MMGSDI_PERSONALIZATION*/

#endif /*#ifdef FEATURE_MMGSDI*/

/*==================================================================
   AEE Exit callback
==================================================================*/
static void ICard_AEEExitNotify(ICard * po)
{
  MSG_HIGH("Exit ICard in AEEExitNotify, obj: 0x%x", ICardobj, 0, 0);
  if (ICardobj != NULL) {
    if (ICardobj->m_pcbExit) {
      sys_free(ICardobj->m_pcbExit);
      ICardobj->m_pcbExit = NULL;
    }
    (void)aee_forcereleaseobj((void **)&ICardobj);
    ICardobj = NULL;
  }
} /* ICard_AEEExitNotify */ /*lint !e715 */

static void ICardNotifier_AEEExitNotify(ICardNotifier * po)
{
  MSG_HIGH("Exit ICardNotifier in AEEExitNotify, obj: 0x%x",
           ICardNotifier_obj, 0, 0);
  if (ICardNotifier_obj != NULL) {
    if (ICardNotifier_obj->m_pcbExit) {
      sys_free(ICardNotifier_obj->m_pcbExit);
      ICardNotifier_obj->m_pcbExit = NULL;
    }
    if (NULL != ICardobj) {
       (void)OEMCard_Release(ICardobj);
    }
    (void)aee_forcereleaseobj((void **)&ICardNotifier_obj);
    ICardNotifier_obj = NULL;
  }
} /* ICardNotifier_AEEExitNotify *//*lint !e715 */

#ifdef FEATURE_ICARD_IMODEL
static void ICardModel_AEEExitNotify(ICardModel * po)
{
  MSG_HIGH("Exit ICardModel in AEEExitNotify, obj: 0x%x",
           ICardModel_obj, 0, 0);
  if (ICardModel_obj != NULL) {
    if (ICardModel_obj->m_pcbExit) {
      sys_free(ICardModel_obj->m_pcbExit);
      ICardModel_obj->m_pcbExit = NULL;
    }
    if (NULL != ICardobj) {
       (void)OEMCard_Release(ICardobj);
    }
    SysModelBase_Dtor(&ICardModel_obj->base);
    sys_free(ICardModel_obj);
    ICardModel_obj = NULL;
  }
} /* ICardModel_AEEExitNotify */
#endif /*FEATURE_ICARD_IMODEL */

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
static void ICardConnection_AEEExitNotify(ICardConnection * po)
{
  MSG_HIGH("Exit CardConnection in AEEExitNotify, obj: 0x%x",
           po, 0, 0);
  if (po != NULL) {
    int i = 0;
    /* In case it failed to release, we need the ptr to be NULL
        It may not have released due to channel being open */
    for (i = 0; i< OEMCARDCONNECTION_MAX_CHANNEL; i++ )
    {
      if (po == sCardChannelMapTable[i].pICardConnection_obj)
      {
        if (sCardChannelMapTable[i].pICardConnection_obj->m_pcbExit != NULL) {
          CALLBACK_Cancel(sCardChannelMapTable[i].pICardConnection_obj->m_pcbExit);
          sys_free(sCardChannelMapTable[i].pICardConnection_obj->m_pcbExit);
          sCardChannelMapTable[i].pICardConnection_obj->m_pcbExit = NULL;
        }
        (void)aee_forcereleaseobj((void **)&(sCardChannelMapTable[i].pICardConnection_obj));
        sCardChannelMapTable[i].pICardConnection_obj = NULL;
        sCardChannelMapTable[i].bChannelStatus = OEMCARDCONNECTION_NONE;
        sCardChannelMapTable[i].nRequestCancel = OEMCARD_CANCEL_REASON_ENUM_MAX;
        break;
      }
    }
  }
  return;
} /* ICardConnection_AEEExitNotify */
#endif /*FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION: OEMCardNewSapStatus
   Used when the perso status changes.  This causes a NOTIFY event to be
   sent to applications waiting for it.

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  The card status mask is sent with the notify event.
===========================================================================*/
static void OEMCardNewSapStatus(AEECardSapStatus *sapConnStatus)
{

  if (NULL == ICardobj) {
    MSG_ERROR("NewSapStatus: ICardobj Null", 0, 0, 0);
    return;
  }
  if (
#ifdef FEATURE_ICARD_IMODEL
      (NULL == ICardModel_obj) &&
#endif /* FEATURE_ICARD_IMODEL */
      (NULL == ICardNotifier_obj)) {
    MSG_ERROR("NewSapStatus: ICardNotifier_obj Null 0x%x", 
              ICardNotifier_obj, 0, 0);
#ifdef FEATURE_ICARD_IMODEL
    MSG_ERROR("Or NewSapStatus: ICardModel_obj Null 0x%x", 
              ICardModel_obj, 0, 0);
#endif /* FEATURE_ICARD_IMODEL */
    return;
  }

#ifndef FEATURE_ICARD_IMODEL
  MSG_HIGH("NewSapStatus: ICARD_IMODEL Sending SAP Status - connSt: 0x%x discType: 0x%x", 
           sapConnStatus->connectState, sapConnStatus->disconnectType, 0 );

  ICardNotifier_obj->m_SapStatusData.mask = NMASK_BT_SAP_STATUS;
  ICardNotifier_obj->m_SapStatusData.user_data = sapConnStatus;

  ICardNotifier_obj->m_oemSapCb.pfnNotify = (PFNNOTIFY) OEMCarddoNotify;
  ICardNotifier_obj->m_oemSapCb.pNotifyData = &(ICardNotifier_obj->m_SapStatusData);
#if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback(&(ICardNotifier_obj->m_oemSapCb),  0);
#else
    AEE_ResumeEx(&(ICardNotifier_obj->m_oemSapCb), 0, 0);
#endif

#else /*FEATURE_ICARD_IMODEL*/
  MSG_HIGH("NewSapStatus: ICardNotifier Sending SAP Status - connSt: 0x%x discType: 0x%x", 
           sapConnStatus->connectState, sapConnStatus->disconnectType, 0 );

  ICardobj->m_SapStatusData.mask = NMASK_BT_SAP_STATUS;
  ICardobj->m_SapStatusData.user_data = sapConnStatus;

  ICardobj->m_oemSapCb.pfnNotify = (PFNNOTIFY) OEMCarddoNotify;
  ICardobj->m_oemSapCb.pNotifyData = &(ICardobj->m_SapStatusData);
  #if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback(&(ICardobj->m_oemSapCb),  0);
  #else
    AEE_ResumeEx(&(ICardobj->m_oemSapCb), 0, 0);
  #endif
#endif /* FEATURE_ICARD_IMODEL */
} /* OEMCardNewSapStatus */

/*===========================================================================

FUNCTION: OEMCardGSDIMMGetPinStatus

DESCRIPTION
   Get the PIN information from MMGSDI event

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  The variables pin1_status and pin2_status will be updated
  as those pin status responses are received.
  Also, a NOTIFY event will be generated to the clients once we receive
  the last Pin Status.
===========================================================================*/
static void OEMCardMMGSDIGetPinStatus(
  const mmgsdi_event_data_type *pEvent
)
{
  AEECardPinStatus *pPinStatus  = NULL; /* Which pin are we working on. */
  uint8             pin_id      = 0;

  if(NULL == pEvent) {
    MSG_ERROR("MMGSDIGetPinStatus: Event Pointer is NULL",0,0,0);
    return;
  }

  switch (pEvent->data.pin.pin_info.pin_id)
  {
    case MMGSDI_PIN1:
      pPinStatus = &pin1_status;
      pin_id = AEECARD_PIN1;
      break;
    case MMGSDI_PIN2:
      pPinStatus = &pin2_status;
      pin_id = AEECARD_PIN2;
      break;
    default:
      ERR("Unsupported PIN 0x%x", pEvent->data.pin.pin_info.pin_id, 0, 0);
      return; /* Other pins are not supported yet. */
  }
  if(NULL == pPinStatus) {
    MSG_ERROR("MMGSDIGetPinStatus: Pin Status ptr is NULL",0,0,0);
    return;
  }

  if(OEMCard_SetPinStatus(pPinStatus,pEvent->data.pin.pin_info) == SUCCESS) {
    OEMCardNewPinStatus(pin_id, pPinStatus);
  }
} /* OEMCardGSDIMMGetPinStatus */

/*===========================================================================
FUNCTION: OEMCard_MMGSDIEvtCb

DESCRIPTION
  Callback to MMGSDI Events

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCard_MMGSDIEvtCb(const mmgsdi_event_data_type *event)
{
  AEECardPinStatus *pPinStatus;
  AEECardSapStatus *pSapStatus;
  sim_capabilities_T dummy_sim_cap;

  if (NULL == event)
    return;

  MSG_HIGH("MMGSDIEvtCb - evt: 0x%x", event->evt, 0, 0 );

  if (NULL == ICardobj) {
    MSG_ERROR("_MMGSDIEvtCb: ICardobj NULL",0,0,0);
    return;
  }

  switch(event->evt)
  {
  case MMGSDI_SAP_PIN_EVT:
    pPinStatus = &sap_pin_status;
    if(event->data.sap_pin.status == MMGSDI_PIN_ENABLED_NOT_VERIFIED)
      pPinStatus->lock_state = AEECARD_PIN_ENABLED;
    else if(event->data.sap_pin.status == MMGSDI_PIN_ENABLED_VERIFIED)
      pPinStatus->lock_state = AEECARD_PIN_VERIFIED;
    else
      pPinStatus->lock_state = AEECARD_PIN_INVALID;

    pPinStatus->tries_remaining= 1;
    OEMCardNewPinStatus(AEECARD_SAP_PIN, pPinStatus);
    break;

    case MMGSDI_SAP_CONNECT_EVT:
      pSapStatus = &sap_status;
      switch(event->data.sap_connect.connect_state)
      {
        case MMGSDI_SAP_CONNECTED:
          pSapStatus->connectState = AEECARD_SAP_CONNECTED_STATE;
          break;
        case MMGSDI_SAP_CONNECTING:
          pSapStatus->connectState = AEECARD_SAP_CONNECTING_STATE;
          break;
        case MMGSDI_SAP_CONNECT_FAIL:
          pSapStatus->connectState = AEECARD_SAP_CONNECTION_FAIL_STATE;
          break;
        default:
          MSG_ERROR("Invalid Connection Status Received %x", event->data.sap_connect.connect_state,0,0);
          return;
      }
      OEMCardNewSapStatus(pSapStatus);
      break;
    case MMGSDI_SAP_DISCONNECT_EVT:
      pSapStatus = &sap_status;
      switch(event->data.sap_disconnect.disconnect_state)
      {
        case MMGSDI_SAP_DISCONNECTED:
          pSapStatus->connectState = AEECARD_SAP_DISCONNECTED_STATE;
          break;
        case MMGSDI_SAP_DISCONNECTING:
          pSapStatus->connectState = AEECARD_SAP_DISCONNECTING_STATE;
          break;
        case MMGSDI_SAP_DISCONNECT_FAIL:
          pSapStatus->connectState = AEECARD_SAP_DISCONNECTION_FAIL_STATE;
          break;
        default:
          MSG_ERROR("Invalid Connection Status Received %x", event->data.sap_disconnect.disconnect_state,0,0);
          return;
      }
      switch(event->data.sap_disconnect.sap_disconnect_type)
      {
        case MMGSDIBT_DISCONNECT_GRACE:
          pSapStatus->disconnectType = AEECARD_DISC_GRACE;
          break;
        case MMGSDIBT_DISCONNECT_IMMED:
          pSapStatus->disconnectType = AEECARD_DISC_IMMED;
          break;
        default:
          MSG_ERROR("Invalid disconnect type Received %x", event->data.sap_disconnect.sap_disconnect_type,0,0);
          return;
      }
      OEMCardNewSapStatus(pSapStatus);
      break;

   case MMGSDI_PIN1_EVT:
   case MMGSDI_PIN2_EVT:
      OEMCardMMGSDIGetPinStatus(event);
      break;

   case MMGSDI_CARD_INIT_COMPLETED_EVT:
     if(pin1_status.lock_state == AEECARD_PIN_UNKNOWN)
     {
        (void)OEMCard_SetPinStatus(&pin1_status, event->data.card_init_completed.app_info[0].pin1);
        (void)OEMCard_SetPinStatus(&pin2_status, event->data.card_init_completed.app_info[0].pin2);
     }
     CardStatus = GSDI_SIM_INIT_COMPLETED;

     /* set cache sim capability info */
     (void)gsdi_get_sim_capabilities(&dummy_sim_cap, 0, OEMCardGSDISimCapCB);

     break;
   case MMGSDI_CARD_INSERTED_EVT:
   case MMGSDI_CARD_ERROR_EVT:
   case MMGSDI_CARD_REMOVED_EVT:
   case MMGSDI_TERMINAL_PROFILE_DL_EVT:
   case MMGSDI_REFRESH_EVT:
   case MMGSDI_FDN_EVT:
   case MMGSDI_SWITCH_SLOT_EVT:
   case MMGSDI_ILLEGAL_CARD_EVT:
   case MMGSDI_UNIVERSAL_PIN_EVT:
   case MMGSDI_SESSION_CLOSE_EVT:
   case MMGSDI_SELECT_AID_EVT:
   case MMGSDI_MAX_EVT_ENUM:
     break;
   }

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
  if (!ATR_Slot1.bReady && GSDI_NO_EVENT != CardStatus &&
      !ATR_Slot1.bReqInProgress)
  {
    if(gsdi_sim_get_atr(GSDI_SLOT_1, 0, OEMCardGSDIGetATRCB) == GSDI_SUCCESS){
     ATR_Slot1.bReqInProgress = TRUE;
    }
  }
#endif /*FEATURE_MMGSDI_CHANNEL_MANAGEMENT*/

} /* OEMCard_MMGSDIEvtCb */

/*===========================================================================
FUNCTION: OEMCard_MMGSDIRspCb

DESCRIPTION
  Callback to MMGSDI Response

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCard_MMGSDIRspCb(mmgsdi_return_enum_type status,
                                mmgsdi_cnf_enum_type       cnf,
                                const mmgsdi_cnf_type     *cnf_ptr)
{
} /* OEMCard_MMGSDIRspCb */

/*===========================================================================
FUNCTION: OEMCard_MMGSDIClientIdRegCb

DESCRIPTION
  Callback to MMGSDI Client Registration

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCard_MMGSDIClientIdRegCb
  (mmgsdi_return_enum_type status,
   mmgsdi_cnf_enum_type    cnf,
   const mmgsdi_cnf_type  *cnf_ptr)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  if (MMGSDI_SUCCESS == status && NULL != ICardobj )
  {
    if (NULL == cnf_ptr)
    {
      MSG_ERROR("_MMGSDIClientIdRegCb: Null Cnf Ptr for SUCCESS status", 
                0, 0, 0);
      return;
    }
    if (cnf == MMGSDI_CLIENT_ID_REG_CNF)
    {
      ICardobj->m_mmgsdi_client_id =
         cnf_ptr->client_id_reg_cnf.response_header.client_id;

      mmgsdi_status = mmgsdi_client_evt_reg (
             ICardobj->m_mmgsdi_client_id ,
             OEMCard_MMGSDIEvtCb,
             OEMCard_MMGSDIRspCb,
             0);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("Unable to Register for MMGSDI event notification: 0x%x",
                  mmgsdi_status, 0, 0);
      }
    }
  }
} /* OEMCard_MMGSDIClientIdRegCb */

#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION: OEMCard_New

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
int OEMCard_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
#ifdef FEATURE_MMGSDI
   gsdi_returns_T          gsdi_status = GSDI_SUCCESS;
   mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
#endif /* FEATURE_MMGSDI */
#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
     MSG_ERROR("OEMCard_New: pIShell 0x%x Null or ppif 0x%x Null", 
               pIShell, ppif, 0);
     return EBADPARM;
   }

   if (AEECLSID_CARD != cls) {
     MSG_ERROR("OEMCard_New: cls 0x%x != AEECLSID_CARD", cls, 0, 0);
     return EUNSUPPORTED;
   }

   if (ICardobj == NULL)
   {
#ifdef FEATURE_MMGSDI
      if (!gInitialized) {
         /* call gsdi_init */
         // Note:  Need to make sure that this is called once only....
         // We should move this to tmc.c then.  PBM or FEATURE FLAG
         gsdi_init(TRUE);

         /* Register OEMCardGSDIevtCB with gsdi so that OEMCard can start
         * receiving events from gsdi */
         // We need to make this smarter.  Call GSDI_GET_CARD_STATUS.
         gsdi_status = gsdi_reg_callback(OEMCardGSDIevtCB);
         if (gsdi_status != GSDI_SUCCESS)
         {
           MSG_ERROR("Unable to register GSDI evt cb 0x%x", gsdi_status, 0, 0);
         }
#ifdef FEATURE_MMGSDI_PERSONALIZATION
         gsdi_status = gsdi_perso_register_task(OEMCardGSDIPersoevtCB, 0, OEMCardGSDIPersocmdCB);
         if (gsdi_status != GSDI_SUCCESS)
         {
           MSG_ERROR("Unable to register GSDI Perso evt cb 0x%x", gsdi_status, 0, 0);
           /* todo no way to disregister evt cb at this point */
         }
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
         gInitialized = TRUE;
         mmgsdi_status = mmgsdi_client_id_reg (
            OEMCard_MMGSDIClientIdRegCb,
            10);
         if (mmgsdi_status != MMGSDI_SUCCESS)
         {
           MSG_ERROR("Unable to get MMGSDI client id 0x%x", mmgsdi_status, 0, 0);
           /* todo no way to disregister gsdi evt and perso cb at this point */
         }
#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif
      }
#endif /*#ifdef FEATURE_MMGSDI*/

      // Allocate the object.
      ICardobj = (ICard *)(void*)AEE_NewClassEx((AEEVTBL(IBase) *)(void*)&gOEMCardFuncs, sizeof(ICard), TRUE);

      if (NULL == ICardobj) {
        MSG_ERROR("OEMCard_New: ICardobj Null", 0, 0, 0);
        return ENOMEMORY;
      }

      /* register for AEE Exit event */
      ICardobj->m_pcbExit = sys_malloc(sizeof(AEECallback));
      if (!ICardobj->m_pcbExit)
      {
        *ppif = NULL;
        FREE(ICardobj);
        ICardobj = NULL;
        MSG_ERROR("OEMCard_New: ICardobj register AEEExit event failed", 0, 0, 0);
        return ENOMEMORY;
      }

      CALLBACK_Init(ICardobj->m_pcbExit, (PFNNOTIFY)ICard_AEEExitNotify,
                     ICardobj);
#ifndef FEATURE_BREW_3_0
      AEE_RegisterSystemCallback(ICardobj->m_pcbExit, AEE_SCB_AEE_EXIT,
                                 AEE_RESUME_CB_SYS,0);
#else
      ISHELL_OnExit(pIShell, ICardobj->m_pcbExit);
#endif
      ICardobj->pvt = (ICardVtbl *)&gOEMCardFuncs;
      ICardobj->m_pIShell = pIShell;
      ICardobj->m_cls = cls;
      ICardobj->m_uRefs = 1;
      ICardobj->m_bRefreshPending = FALSE;

      *ppif = ICardobj;
   }
   else
   {
      if (OEMCard_AddRef(ICardobj) != 0)
      {
        *ppif = ICardobj;
      }
      else
      {
        *ppif = NULL;
      }
   }

   return AEE_SUCCESS;
} /* OEMCard_New */

/*===========================================================================

FUNCTION: OEMCard_AddRef

DESCRIPTION
  Indicates that one more object is now pointing to the OEMCard object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static uint32 OEMCard_AddRef(ICard *po)
{
   if (po) {
      return (++po->m_uRefs);
   }
   return 0;
} /* OEMCard_AddRef */

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION: OEMCard_FreeSequenceNumber

DESCRIPTION
  This function frees the Sequence Number for the OEMCard Get Resp Table

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  If data hasn't been retrieved from the index location of the
  table, upon calling this function, the data will be freed

See Also:
   None

===========================================================================*/
static void OEMCard_FreeSequenceNumber(uint32 dwSeqNum)
{
  if (dwSeqNum >= OEMCARD_MAX_RSP) {
    return;
  }

  /* remove the memory allocated */
  if (NULL != sCardRspTable[dwSeqNum].sRsp)
  {
    OEM_Free(sCardRspTable[dwSeqNum].sRsp);
    sCardRspTable[dwSeqNum].sRsp = NULL;
    sCardRspTable[dwSeqNum].nSize = 0;
  }
} /* OEMCard_FreeSequenceNumber */
#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION: OEMCard_Release

DESCRIPTION
  Decreases the number of references to the ICard object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static uint32 OEMCard_Release(ICard *po)
{
#if defined(FEATURE_MMGSDI) || defined (FEATURE_ICARD_IMODEL)
   int i = 0;
#endif /* FEATURE_MMGSDI */

   if (!po) {
     return 0;
   }

   if (--po->m_uRefs != 0) {
     return po->m_uRefs;
   }

   if (NULL == ICardobj) {
     return 0;
   }

   MSG_HIGH("OEMCard_Release Cleanup: ICardobj = 0x%x, exit_cb = 0x%x",
             ICardobj, ICardobj->m_pcbExit, 0);
   if (ICardobj->m_pcbExit) {
     CALLBACK_Cancel(ICardobj->m_pcbExit);
     sys_free(ICardobj->m_pcbExit);
     ICardobj->m_pcbExit = NULL;
   }

   // Ref count is zero.  Release memory

#ifdef FEATURE_MMGSDI
   if(pCardFileChangeNotify)
   {
     OEMCardFileChangeNotify *temp = pCardFileChangeNotify;
     OEMCardFileChangeNotify *next;

     while(temp)
     {
       next = temp->next;
       sys_free(temp);
       temp = next;
     }
   }
   ATR_Slot1.bReqInProgress = FALSE;
   ATR_Slot1.bReady = FALSE;
   if ((ATR_Slot1.sATR.nSize > 0) && (ATR_Slot1.sATR.pATR != NULL))
   {
     sys_free(ATR_Slot1.sATR.pATR);
     ATR_Slot1.sATR.pATR = NULL;
     ATR_Slot1.sATR.nSize = 0;
   }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
   ATR_Slot2.bReady = FALSE;
   ATR_Slot2.bReqInProgress = FALSE;
   if ((ATR_Slot2.sATR.nSize > 0) && (ATR_Slot2.sATR.pATR != NULL))
   {
     sys_free(ATR_Slot2.sATR.pATR);
     ATR_Slot2.sATR.pATR = NULL;
     ATR_Slot2.sATR.nSize = 0;
   }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
   for (i = 0; i< OEMCARD_MAX_RSP; i++)
   {
     OEMCard_FreeSequenceNumber(i);
   }
   gInitialized = FALSE;
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_ICARD_IMODEL
   /*  Cancel callbacks*/

   CALLBACK_Cancel( &po->m_oemCb );
   CALLBACK_Cancel( &po->m_oemCb_slot2 );
   for (i = 0; i < AEECARD_MAXPIN; i++)
   {
     CALLBACK_Cancel( &po->m_oemPINCb[i] );
   }
   CALLBACK_Cancel( &po->m_oemPersoCb );
   CALLBACK_Cancel( &po->m_oemSapCb );

#endif /* FEATURE_ICARD_IMODEL */

   OEM_Free(po);
   ICardobj = NULL;
   return SUCCESS;
} /* OEMCard_Release */

/*===========================================================================

FUNCTION: OEMCard_QueryInterface

DESCRIPTION
  Allows applications to query if ICard supports other interfaces and then
  returns a pointer to that new interface supported via ICard.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_QueryInterface(ICard *po, AEECLSID id, void **ppNew)
{
   int ret_val = AEE_SUCCESS;

   if ((!po) || (!ppNew)) {
     return EBADPARM;
   }

   switch (id) {
     case AEECLSID_QUERYINTERFACE:
     case AEECLSID_CARD:
       if (OEMCard_AddRef(po) > 0)
       {
         *ppNew = po;
       }
       else
       {
         *ppNew = NULL;
       }
       break;
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
     case AEECLSID_CARDCONNECTION:
       ret_val = OEMCardConnection_New(po->m_pIShell,AEECLSID_CARDCONNECTION,ppNew);
       break;
#endif /*FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
     default:
       *ppNew = NULL;
       ret_val = ECLASSNOTSUPPORT;
       break;
   }

   return ret_val;
} /* OEMCard_QueryInterface */

/*===========================================================================

FUNCTION: OEMCard_GetCardStatus

DESCRIPTION
  Looks at the variable CardStatus and pin1_status to determine the status of
  the current SIM.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS if all went well, EBADPARM if there was a parameter error.

SIDE EFFECTS
  *card_status is set to one of the following:
    AEECARD_NO_CARD            No card is inserted.
    AEECARD_INVALID_CARD       Unusable Card is inserted.
    AEECARD_VALID_CARD         The card is now usable.
    AEECARD_NOT_INIT           Temporary condition indicating ICard is
                               still determining the state of the card or
                               GSDI is still initializing.  There will be
                               an event generated at the completion of this
                               process.
    AEECARD_CARD_REINIT        Indicates OEMCard has received notification that the
                               SIM Initialization procedures are about to be executed
                               again.  Must send notify CM that service is not available.
    AEECARD_AVAIL_CARD         Indicates OEMCard has received a notification that the
                               card is presented but no provisioning information will
                               be coming from that card.
===========================================================================*/
static int OEMCard_GetCardStatus(ICard *po, uint8 *card_status)
{
  return OEMCard_GetCardStatusSlot(po, AEECARD_SLOT1, card_status);
} /* OEMCard_GetCardStatus */

/* ===========================================================================
Function: OEMCard_GetCardStatusSlot()

Description:
   This method is obtains the current status of the SIM/USIM/RUIM device
   at the indiated card slot from the UE.

Parameters:
   po: [Input] Pointer to the ICard object
   wSlot: [Input] Indicates which slot the card status is to be retrieved from
   ps: [Input/Output] Pointer to status information to be filled.
  *ps is set to one of the following:
    AEECARD_NO_CARD            No card is inserted.
    AEECARD_INVALID_CARD       Unusable Card is inserted.
    AEECARD_VALID_CARD         The card is now usable.
    AEECARD_NOT_INIT           Temporary condition indicating ICard is
                               still determining the state of the card or
                               GSDI is still initializing.  If the user
                               registered for the notifications, an event
                               will be generated at the completion of this
                               process.
    AEECARD_CARD_REINIT        Indicates OEMCard has received notification that the
                               SIM Initialization procedures are about to be executed
                               again.  Must send notify CM that service is not available.
    AEECARD_AVAIL_CARD         Indicates OEMCard has received a notification that the
                               card is presented but no provisioning information will
                               be coming from that card.
Return Value:

  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter

Comments:
   None

Side Effects:
   None

See Also:
   None
===========================================================================*/
static int OEMCard_GetCardStatusSlot(ICard *po, uint8 wSlot, uint8 *card_status)
{

  if ((!po) || (!card_status)){
    MSG_ERROR("_GetCardStatusSlot: po Null 0x%x or card_status Null 0x%x",
              po, card_status, 0); 
     return EBADPARM;
  }

  return OEMCard_CardStatus(wSlot, card_status);
} /* OEMCard_GetCardStatusSlot */

/* ===========================================================================
Function: OEMCard_CardStatus()

Description:
   This method is obtains the current status of the SIM/USIM/RUIM device
   at the indiated card slot from the UE.

Parameters:
   wSlot: [Input] Indicates which slot the card status is to be retrieved from
   pbyCardStatus: [Input/Output] Pointer to status information to be filled.
  *pbyCardStatus is set to one of the following:
    AEECARD_NO_CARD            No card is inserted.
    AEECARD_INVALID_CARD       Unusable Card is inserted.
    AEECARD_VALID_CARD         The card is now usable.
    AEECARD_NOT_INIT           Temporary condition indicating ICard is
                               still determining the state of the card or
                               GSDI is still initializing.  If the user
                               registered for the notifications, an event
                               will be generated at the completion of this
                               process.
    AEECARD_CARD_REINIT        Indicates OEMCard has received notification that the
                               SIM Initialization procedures are about to be executed
                               again.  Must send notify CM that service is not available.
    AEECARD_AVAIL_CARD         Indicates OEMCard has received a notification that the
                               card is presented but no provisioning information will
                               be coming from that card.
Return Value:

  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter

Comments:
   None

Side Effects:
   None

See Also:
   None
===========================================================================*/
static int OEMCard_CardStatus(uint8 wSlot, uint8 *pbyCardStatus)
{
  if (!pbyCardStatus) {
    MSG_ERROR("_CardStatus: pbyCardStatus Null ",
              0, 0, 0); 
    return EBADPARM;
  }
#ifndef FEATURE_MMGSDI
  *pbyCardStatus = AEECARD_NO_CARD;
#else
  if (wSlot == AEECARD_SLOT1) {

#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)*/


    switch(CardStatus) {
    case GSDI_SIM_INSERTED:
    case GSDI_APP_SELECTED:
      /* When the SIM is inserted, we get this CardStatus.  OEMCard then
      * sends requests to get the PIN code.  If the response has not been
      * yet seen, then we return AEECARD_NOT_INIT.  Also, if the response
      * has been returned as AEECARD_PIN_DISABLED, then we are waiting for
      * the SIM_INIT_COMPLETED before we finish initialization.  */

      switch(pin1_status.lock_state)
      {
        case AEECARD_PIN_UNKNOWN:
        case AEECARD_PIN_DISABLED: /* PIN is not required. */
        case AEECARD_PIN_VERIFIED: /* PIN is required. */
        case AEECARD_PIN_NOT_VALID: /* PIN functionality not supported. */
        case AEECARD_PIN_ENABLED:  /* PIN is enabled. */
        case AEECARD_PIN_BLOCKED: /* PIN is blocked */
          *pbyCardStatus = AEECARD_NOT_INIT;
          break;
        case AEECARD_PIN_PERM_DISABLED: /* Pin cannot be Unblocked. */
        default:
          *pbyCardStatus = AEECARD_INVALID_CARD;
          break;
      }
      break;

    case GSDI_SIM_INIT_COMPLETED:
      /* The SIM has completed its initialization, all is well with the
      * world according to the SIM.
      */
      *pbyCardStatus = AEECARD_VALID_CARD;
      break;

    case GSDI_SIM_REMOVED:
    case GSDI_NO_SIM:
    case GSDI_CARD_ERROR:
#ifdef FEATURE_GSTK
    case GSDI_REFRESH_SIM_RESET:
    case GSDI_REFRESH_APP_RESET:
    case GSDI_REFRESH_3G_SESSION_RESET:
#endif
    case GSDI_CARD_ERR_POLL_ERROR:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET:
    case GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES:
    case GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES:
    case GSDI_CARD_ERR_VOLTAGE_MISMATCH:
    case GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS:
    case GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS:
    case GSDI_CARD_ERR_MAXED_PARITY_ERROR:
    case GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR:
    case GSDI_CARD_ERR_MAXED_OVERRUN_ERROR:
    case GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED:
    case GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION:
    case GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE:
    case GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE:
    case GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE:
    case GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE:
    case GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE:
      /* The SIM has been removed. */
      *pbyCardStatus = AEECARD_NO_CARD;
      break;

#ifdef FEATURE_GSTK
    case GSDI_REFRESH_SIM_INIT:
    case GSDI_REFRESH_SIM_INIT_FCN:
      /* The SIM is reinitializing.  Wait for */
      /* SIM Init Completed                   */
      /* PIN Information is not changed       */
      *pbyCardStatus = AEECARD_CARD_REINIT;
      break;
#endif

    case GSDI_NO_EVENT:
      /* GSDI is not ready */
      *pbyCardStatus = AEECARD_NOT_READY;
      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SIM_INIT_COMPLETED_NO_PROV:
      /* no provisioning info from the card */
      *pbyCardStatus = AEECARD_AVAIL_CARD;
      break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    case GSDI_SIM_ILLEGAL:
       *pbyCardStatus = AEECARD_ILLEGAL_CARD;
       break;

    default:
      /* We encountered some type of error and we are marking the SIM as
      * invalid.
      */
      *pbyCardStatus = AEECARD_INVALID_CARD;
      DBGPRINTF("Unknown status value 0x%x received from get card status()",
          CardStatus);
      break;

    }
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else {
    switch(CardStatus_Slot2) {
    case GSDI_SIM_INSERTED_2:
    case GSDI_APP_SELECTED_2:
      /* When the SIM is inserted, we get this CardStatus.  OEMCard then
      * sends requests to get the PIN code.  If the response has not been
      * yet seen, then we return AEECARD_NOT_INIT.  Also, if the response
      * has been returned as AEECARD_PIN_DISABLED, then we are waiting for
      * the SIM_INIT_COMPLETED before we finish initialization.  */

      switch(pin1_slot2_status.lock_state)
      {
        case AEECARD_PIN_UNKNOWN:
        case AEECARD_PIN_DISABLED: /* PIN is not required. */
        case AEECARD_PIN_VERIFIED: /* PIN is required. */
        case AEECARD_PIN_NOT_VALID: /* PIN functionality not supported. */
        case AEECARD_PIN_ENABLED:  /* PIN is enabled. */
        case AEECARD_PIN_BLOCKED: /* PIN is blocked */
          *pbyCardStatus = AEECARD_NOT_INIT;
          break;
        case AEECARD_PIN_PERM_DISABLED: /* Pin cannot be Unblocked. */
        default:
          *pbyCardStatus = AEECARD_INVALID_CARD;
          break;
      }
      break;

    case GSDI_SIM_INIT_COMPLETED_2:
      /* The SIM has completed its initialization, all is well with the
      * world according to the SIM.
      */
      *pbyCardStatus = AEECARD_VALID_CARD;
      break;

    case GSDI_CARD_ERR_POLL_ERROR_2:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_2:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET_2:
    case GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES_2:
    case GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES_2:
    case GSDI_CARD_ERR_VOLTAGE_MISMATCH_2:
    case GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS_2:
    case GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_2:
    case GSDI_CARD_ERR_MAXED_PARITY_ERROR_2:
    case GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR_2:
    case GSDI_CARD_ERR_MAXED_OVERRUN_ERROR_2:
    case GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED_2:
    case GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION_2:
    case GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE_2:
    case GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE_2:
    case GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE_2:
    case GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE_2:
    case GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE_2:
    case GSDI_CARD_ERR_POLL_ERROR_2:
    case GSDI_SIM_REMOVED_2:
    case GSDI_CARD_ERROR_2:
#ifdef FEATURE_GSTK
    case GSDI_REFRESH_SIM_RESET_2:
    case GSDI_REFRESH_APP_RESET_2:
    case GSDI_REFRESH_3G_SESSION_RESET_2:
#endif
      /* The SIM has been removed. */
      *pbyCardStatus = AEECARD_NO_CARD;
      break;

#ifdef FEATURE_GSTK
    case GSDI_REFRESH_SIM_INIT_2:
    case GSDI_REFRESH_SIM_INIT_FCN_2:
      /* The SIM is reinitializing.  Wait for */
      /* SIM Init Completed                   */
      /* PIN Information is not changed       */
      *pbyCardStatus = AEECARD_CARD_REINIT;
      break;
#endif

    case GSDI_NO_SIM_EVENT_2:
      /* GSDI is not ready */
      *pbyCardStatus = AEECARD_NOT_READY;
      break;

    case GSDI_SIM_INIT_COMPLETED_NO_PROV_2:
      /* no provisioning info from the card */
      *pbyCardStatus = AEECARD_AVAIL_CARD;
      break;

    case GSDI_SIM_ILLEGAL_2:
       *pbyCardStatus = AEECARD_ILLEGAL_CARD;
       break;

    default:
      /* We encountered some type of error and we are marking the SIM as
      * invalid.
      */
      *pbyCardStatus = AEECARD_INVALID_CARD;
      DBGPRINTF("Unknown status value 0x%x received from get card status()",
          CardStatus_Slot2);
      break;
    }
  }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
#endif /*!FEATURE_MMGSDI */
  return SUCCESS;
} /* OEMCard_CardStatus */

/*===========================================================================

FUNCTION: OEMCard_GetPINStatus

DESCRIPTION
  Gets the status of the requested PIN.  The PIN status is stored in OEM
  cache, thus we just get the information out of the local variable.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS if we understand the pin_id.
  EBADPARM if there was an error in an input parameter.

SIDE EFFECTS
  pPINStatus is updated.
===========================================================================*/
static int OEMCard_GetPinStatus(ICard *po, uint8 pin_id,
    AEECardPinStatus *pPINStatus)
{
   if ((!po) || (!pPINStatus)) {
     MSG_ERROR("_GetPinStatus: po Null 0x%x or pPINStatus Null 0x%x",
              po, pPINStatus, 0); 
     return EBADPARM;
   }

#ifdef FEATURE_MMGSDI
   switch (pin_id) {
   case AEECARD_PIN1:
     /* Lock state of this PIN */
     pPINStatus->lock_state      = pin1_status.lock_state;
     /* Number of unlock tries remaining. */
     pPINStatus->tries_remaining = pin1_status.tries_remaining;
     break;
   case AEECARD_PIN2:
     /* Lock state of this PIN */
     pPINStatus->lock_state      = pin2_status.lock_state;
     /* Number of unlock tries remaining. */
     pPINStatus->tries_remaining = pin2_status.tries_remaining;
     break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
   case AEECARD_PIN1_SLOT2:
     /* Lock state of this PIN */
     pPINStatus->lock_state      = pin1_slot2_status.lock_state;
     /* Number of unlock tries remaining. */
     pPINStatus->tries_remaining = pin1_slot2_status.tries_remaining;
     break;
   case AEECARD_PIN2_SLOT2:
     /* Lock state of this PIN */
     pPINStatus->lock_state      = pin2_slot2_status.lock_state;
     /* Number of unlock tries remaining. */
     pPINStatus->tries_remaining = pin2_slot2_status.tries_remaining;
     break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
   default:
     MSG_ERROR("_GetPinStatus: Unhandled PIN id 0x%x",
               pin_id, 0, 0); 
     return EBADPARM;
   }

   return (SUCCESS);
#else /*#ifdef FEATURE_MMGSDI*/
   return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_GetPinStatus */

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION: OEMCard_MapFileType

DESCRIPTION
  This function maps gsdi_file_types_T into AEECardFileType

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_MapFileType(AEECardFileType *pCardFileType, gsdi_file_types_T nGSDIFileType)
{
  if (NULL == pCardFileType) {
    MSG_ERROR("_MapFileType: pCardFileType Null", 0, 0, 0);
    return EFAILED;
  }
  switch(nGSDIFileType) {
  case GSDI_MASTER_FILE:
    *pCardFileType = AEECARD_FTYPE_MASTER_FILE;
    break;
  case GSDI_DEDICATED_FILE:
    *pCardFileType = AEECARD_FTYPE_DEDICATED_FILE;
    break;
  case GSDI_ELEMENTARY_FILE:
    *pCardFileType = AEECARD_FTYPE_ELEMENTARY_FILE;
    break;
  default:
    MSG_ERROR("_MapFileType: nGSDIFileType 0x%x not supported", 
              nGSDIFileType, 0, 0);
    return EFAILED;
  }
  return SUCCESS;
} /* OEMCard_MapFileType */


/*===========================================================================

FUNCTION: OEMCard_MapFileStructure

DESCRIPTION
  This function maps gsdi_file_structures_T into AEECardFileStructureType

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_MapFileStructure(AEECardFileStructureType *pCardFileStruct,
                             gsdi_file_structures_T nGSDIFileStruct)
{
  if (NULL == pCardFileStruct) {
    MSG_ERROR("_MapFileStructure: pCardFileStruct Null", 0, 0, 0);
    return EFAILED;
  }
  switch(nGSDIFileStruct) {
  case GSDI_TRANSPARENT_FILE:
    *pCardFileStruct = AEECARD_FSTRUCTURE_TRANSPARENT;
    break;
  case GSDI_LINEAR_FILE:
    *pCardFileStruct = AEECARD_FSTRUCTURE_LINEAR;
    break;
  case GSDI_CYCLIC_FILE:
    *pCardFileStruct = AEECARD_FSTRUCTURE_CYCLIC;
    break;
  default:
    MSG_ERROR("_MapFileStructure: nGSDIFileStruct 0x%x not supported", 
              nGSDIFileStruct, 0, 0);
    return EFAILED;
  }
  return SUCCESS;
} /* OEMCard_MapFileStructure */

/*===========================================================================
FUNCTION: OEMCard_ParseUSIMProtectionInfo

DESCRIPTION
  This function parses the access conditions for USIM

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_ParseUSIMProtectionInfo (
  AEECardAccessConditionType* pAccessStruct,
  uint8                       byAccessValue)
{
  if (NULL == pAccessStruct) {
    MSG_ERROR("_ParseUSIMProtectionInfo: pAccessStruct Null", 0, 0, 0);
    return EFAILED;
  }
  (void)MEMSET(pAccessStruct, 0x00, sizeof(AEECardAccessConditionType));
  pAccessStruct->dwStructureSize = sizeof(AEECardAccessConditionType);
  switch (byAccessValue) {
  case 1:
    pAccessStruct->nApp1OrGSMPin1 = TRUE;
    break;
  case 2:
    pAccessStruct->nApp2Pin1 = TRUE;
    break;
  case 3:
    pAccessStruct->nApp3Pin1 = TRUE;
    break;
  case 4:
    pAccessStruct->nApp4Pin1 = TRUE;
    break;
  case 5:
    pAccessStruct->nApp5Pin1 = TRUE;
    break;
  case 6:
    pAccessStruct->nApp6Pin1 = TRUE;
    break;
  case 7:
    pAccessStruct->nApp7Pin1 = TRUE;
    break;
  case 8:
    pAccessStruct->nApp8Pin1 = TRUE;
    break;
  case 0x0A:
    pAccessStruct->nADM1 = TRUE;
    break;
  case 0x0B:
    pAccessStruct->nADM2 = TRUE;
    break;
  case 0x0C:
    pAccessStruct->nADM3 = TRUE;
    break;
  case 0x0D:
    pAccessStruct->nADM4 = TRUE;
    break;
  case 0x0E:
    pAccessStruct->nADM5 = TRUE;
    break;
  case 0x11:
    pAccessStruct->nPinUniversal = TRUE;
    break;
  case 0x81:
    pAccessStruct->nApp1OrGSMPin2 = TRUE;
    break;
  case 0x82:
    pAccessStruct->nApp2Pin2 = TRUE;
    break;
  case 0x83:
    pAccessStruct->nApp3Pin2 = TRUE;
    break;
  case 0x84:
    pAccessStruct->nApp4Pin2 = TRUE;
    break;
  case 0x85:
    pAccessStruct->nApp5Pin2 = TRUE;
    break;
  case 0x86:
    pAccessStruct->nApp6Pin2 = TRUE;
    break;
  case 0x87:
    pAccessStruct->nApp7Pin2 = TRUE;
    break;
  case 0x88:
    pAccessStruct->nApp8Pin2 = TRUE;
    break;
  default:
    MSG_ERROR("ParseUSIMProtectionInfo: Inavlid byAccessValue 0x%x", 
              byAccessValue, 0, 0);
    return EFAILED;
  }
  return SUCCESS;
} /* OEMCard_ParseUSIMProtectionInfo */

/*===========================================================================
FUNCTION: OEMCard_ParseSIMProtectionInfo

DESCRIPTION
  This function parses the access conditions for USIM

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_ParseSIMProtectionInfo (
  AEECardAccessConditionType* pAccessStruct,
  uint8                       byAccessValue)
{
  if (NULL == pAccessStruct) {
    MSG_ERROR("_ParseSIMProtectionInfo: pAccessStruct Null", 0, 0, 0);
    return EFAILED;
  }
  (void)MEMSET(pAccessStruct, 0x00, sizeof(AEECardAccessConditionType));
  pAccessStruct->dwStructureSize = sizeof(AEECardAccessConditionType);
  switch (byAccessValue) {
  case 0:
    pAccessStruct->nAlways = TRUE;
    break;
  case 1:
    pAccessStruct->nApp1OrGSMPin1 = TRUE;
    break;
  case 2:
    pAccessStruct->nApp1OrGSMPin2 = TRUE;
    break;
  case 4:
    pAccessStruct->nADM1 = TRUE;
    break;
  case 5:
    pAccessStruct->nADM2 = TRUE;
    break;
  case 6:
    pAccessStruct->nADM3 = TRUE;
    break;
  case 7:
    pAccessStruct->nADM4 = TRUE;
    break;
  case 8:
    pAccessStruct->nADM5 = TRUE;
    break;
  case 9:
    pAccessStruct->nADM6 = TRUE;
    break;
  case 0x0A:
    pAccessStruct->nADM7 = TRUE;
    break;
  case 0x0B:
    pAccessStruct->nADM8 = TRUE;
    break;
  case 0x0C:
    pAccessStruct->nADM9 = TRUE;
    break;
  case 0x0D:
    pAccessStruct->nADM10 = TRUE;
    break;
  case 0x0E:
    pAccessStruct->nADM11 = TRUE;
    break;
  case 0x0F:
    pAccessStruct->nNever = TRUE;
    break;
  default:
    MSG_ERROR("_ParseSIMProtectionInfo: Inavlid byAccessValue 0x%x", 
              byAccessValue, 0, 0);
    return EFAILED;
  }
  return SUCCESS;
} /* OEMCard_ParseSIMProtectionInfo */

/*===========================================================================

FUNCTION: OEMCard_MapFileProtection

DESCRIPTION
  This function maps gsdi_file_protection_T into AEECardFileProtectionType

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_MapFileProtection(
  AEECardFileProtectionType *pCardFileProtection,
  gsdi_file_protection_T     nGSDIFileProtection)
{
  gsdi_card_apps_T  sCardApp;
  int               nReturnStatus;
  if (NULL == pCardFileProtection) {
    MSG_ERROR("_MapFileProtection: pCardFileProtection Null", 0, 0, 0);
    return EFAILED;
  }

  gsdi_get_apps_available( &sCardApp );
  if (!sCardApp.slot1_present) {
    if (sCardApp.apps_enabled &  GSDI_USIM_APP_MASK) {
      nReturnStatus = OEMCard_ParseUSIMProtectionInfo(&(pCardFileProtection->sRead),
                                                        nGSDIFileProtection.read);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }

      nReturnStatus = OEMCard_ParseUSIMProtectionInfo(&(pCardFileProtection->sWrite),
                                                        nGSDIFileProtection.write);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }

      nReturnStatus = OEMCard_ParseUSIMProtectionInfo(&(pCardFileProtection->sIncrease),
                                                        nGSDIFileProtection.increase);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }

      nReturnStatus = OEMCard_ParseUSIMProtectionInfo(&(pCardFileProtection->sInvalidate),
                                                        nGSDIFileProtection.invalidate);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }

      nReturnStatus = OEMCard_ParseUSIMProtectionInfo(&(pCardFileProtection->sRehabilitate),
                                                        nGSDIFileProtection.rehabilitate);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }
    }
    else if ((sCardApp.apps_enabled &  GSDI_GSM_SIM_MASK) ||
             (sCardApp.apps_enabled &  GSDI_RUIM_APP_MASK)) {
      nReturnStatus = OEMCard_ParseSIMProtectionInfo(&(pCardFileProtection->sRead),
                                                        nGSDIFileProtection.read);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }

      nReturnStatus = OEMCard_ParseSIMProtectionInfo(&(pCardFileProtection->sWrite),
                                                        nGSDIFileProtection.write);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }

      nReturnStatus = OEMCard_ParseSIMProtectionInfo(&(pCardFileProtection->sIncrease),
                                                        nGSDIFileProtection.increase);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }

      nReturnStatus = OEMCard_ParseSIMProtectionInfo(&(pCardFileProtection->sInvalidate),
                                                        nGSDIFileProtection.invalidate);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }

      nReturnStatus = OEMCard_ParseSIMProtectionInfo(&(pCardFileProtection->sRehabilitate),
                                                        nGSDIFileProtection.rehabilitate);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }
    }
  }
  return SUCCESS;
} /* OEMCard_MapFileProtection */

#ifdef FEATURE_CPHS
#error code not present
#endif /*FEATURE_CPHS */

/*===========================================================================

FUNCTION: OEMCard_PopulateFileAttribBuf

DESCRIPTION
  This function populate the gsdi_file_attributes_T structure into
  AEECardFileAttribStatus structure

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_PopulateFileAttribBuf(
  AEECardFileAttribStatus *pFileAttrib,
  gsdi_file_attributes_T  *pGSDIFileAttrib)
{
  int nReturnStatus = SUCCESS;
  if ((NULL == pFileAttrib) || (NULL == pGSDIFileAttrib)){
    MSG_ERROR("_PopulateFileAttribBuf: pFileAttrib Null 0x%x or pGSDIFileAttrib Null 0x%x", 
              pFileAttrib, pGSDIFileAttrib, 0);
    return EBADPARM;
  }
  pFileAttrib->nFileSize = pGSDIFileAttrib->file_size;
  (void)MEMCPY(pFileAttrib->byFileID, pGSDIFileAttrib->file_id, AEECARD_FILE_ID_LEN);
  nReturnStatus = OEMCard_MapFileType(&(pFileAttrib->nFileType), pGSDIFileAttrib->file_type);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }
  if (pGSDIFileAttrib->cyclic_increase_allowed) {
    pFileAttrib->bCyclicIncreaseAllowed = 1;
  }
  else {
    pFileAttrib->bCyclicIncreaseAllowed = 0;
  }
  nReturnStatus = OEMCard_MapFileStructure(&(pFileAttrib->nFileStructure), pGSDIFileAttrib->file_structure);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCard_MapFileProtection(&(pFileAttrib->nFileProtection), pGSDIFileAttrib->file_protection);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (pGSDIFileAttrib->file_invalidated) {
    pFileAttrib->bFileInvalidated = 1;
  }
  else {
    pFileAttrib->bFileInvalidated = 0;
  }

  pFileAttrib->nRecordLen = pGSDIFileAttrib->rec_len;
  pFileAttrib->nNumberRecords = pGSDIFileAttrib->num_of_records;
  if (pGSDIFileAttrib->rw_invalidated_deactivated_allowed) {
    pFileAttrib->bReadWriteAllowedWhenInvalidated = 1;
  }
  else {
    pFileAttrib->bReadWriteAllowedWhenInvalidated = 0;
  }
  return SUCCESS;

} /* OEMCard_PopulateFileAttribBuf */

/*===========================================================================

FUNCTION: OEMCardGSDIcmdCB

DESCRIPTION
  This function is called when OEMCard calls GSDI during normal operation.
  This will cause a callback to be issued to the caller.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCardGSDIcmdCB(gsdi_cnf_T *gsdi_info)
{
  AEEObjectHandle       Handler;
  OEMCard_AsynCBInfo   *pAsynInfo;
  gsdi_pin_ids_T        pin_id  = GSDI_MAX_PIN_ID;
  int                   retries = AEECARD_UNKNOWN_TRIES;
  AEECardPinCmdStatus  *pCmdStatus;
  AEECardStatusWord     sSW;
  AEECardPinStatus     *pinStatus = NULL;

  sSW.sw1 = 0x00;
  sSW.sw2 = 0x00;

  if ((NULL == gsdi_info) || (NULL == ICardobj)) {
    MSG_ERROR("GSDIcmdCB: gsdi_info Null 0x%x or ICardobj Null 0x%x", 
              gsdi_info, ICardobj, 0);
    return;
  }

  CARD_ENTER_CRITICAL_SECTION
  Handler = (AEEObjectHandle)gsdi_info->message_header.client_ref;
  pAsynInfo = (OEMCard_AsynCBInfo*)AEEObjectMgr_GetObject(Handler);
  CARD_LEAVE_CRITICAL_SECTION

  if (NULL == pAsynInfo) {
    MSG_ERROR("GSDIcmdCB: pAsynInfo Null", 0, 0, 0);
    return;
  }

  if (pAsynInfo->Handler != Handler) {
    MSG_ERROR("GSDIcmdCB: pAsynInfo->Handler mismatch 0x%x vs 0x%x",
      pAsynInfo->Handler,Handler,0);
    /* do not clean up the content because it could be invalid data, e.g.,
       we have a case where the data reflected all unused address space with
       value not == 0 */
    return;
  }

  if (NULL == pAsynInfo->pCB) {
    OEMCard_CleanUpAsynObject(Handler);
    MSG_ERROR("GSDIcmdCB: pAsynInfo->pCB Null", 0, 0, 0);
    return;
  }

  pCmdStatus  = pAsynInfo->pClientData;
  if (NULL == pCmdStatus) {
    OEMCard_CleanUpAsynObject(Handler);
    MSG_ERROR("GSDIcmdCB: pCmdStatus Null", 0, 0, 0);
    return;
  }

  if (GSDI_SUCCESS == gsdi_info->message_header.gsdi_status) {
    pCmdStatus->nCmdStatus = SUCCESS;
  }
  else {
    pCmdStatus->nCmdStatus = EFAILED;
  }

  switch(gsdi_info->message_header.resp_type)
  {
    case GSDI_VERIFY_PIN_RSP:
      pin_id = gsdi_info->verify_pin_cnf.pin_id;
      retries = gsdi_info->verify_pin_cnf.num_of_retries;
      sSW.sw1 = gsdi_info->verify_pin_cnf.status_words.sw1;
      sSW.sw2 = gsdi_info->verify_pin_cnf.status_words.sw2;
      break;
    case GSDI_CHANGE_PIN_RSP:
      pin_id = gsdi_info->change_pin_cnf.pin_id;
      retries = gsdi_info->change_pin_cnf.num_of_retries;
      sSW.sw1 = gsdi_info->change_pin_cnf.status_words.sw1;
      sSW.sw2 = gsdi_info->change_pin_cnf.status_words.sw2;
      break;
    case GSDI_UNBLOCK_PIN_RSP:
      pin_id = gsdi_info->unblock_pin_cnf.pin_id;
      retries = gsdi_info->unblock_pin_cnf.num_of_retries;
      sSW.sw1 = gsdi_info->unblock_pin_cnf.status_words.sw1;
      sSW.sw2 = gsdi_info->unblock_pin_cnf.status_words.sw2;
      break;
    case GSDI_DISABLE_PIN_RSP:
      pin_id = gsdi_info->disable_pin_cnf.pin_id;
      retries = gsdi_info->disable_pin_cnf.num_of_retries;
      sSW.sw1 = gsdi_info->disable_pin_cnf.status_words.sw1;
      sSW.sw2 = gsdi_info->disable_pin_cnf.status_words.sw2;
      break;
    case GSDI_ENABLE_PIN_RSP:
      pin_id = gsdi_info->enable_pin_cnf.pin_id;
      retries = gsdi_info->enable_pin_cnf.num_of_retries;
      sSW.sw1 = gsdi_info->enable_pin_cnf.status_words.sw1;
      sSW.sw2 = gsdi_info->enable_pin_cnf.status_words.sw2;
      break;
    case GSDI_PERSO_VERIFY_PHONE_CODE_RSP:
    case GSDI_PERSO_CHANGE_PHONE_CODE_RSP:
    case GSDI_PERSO_UNBLOCK_PHONE_CODE_RSP:
    case GSDI_PERSO_SET_PHONE_CODE_RSP:
      break;
    default:
      MSG_ERROR("Unknown command to OEMCardGSDIcmdCB 0x%x",
                gsdi_info->message_header.resp_type, 0, 0);
      OEMCard_CleanUpAsynObject(Handler);
      return;
  }

  switch(gsdi_info->message_header.resp_type)
  {
    case GSDI_PERSO_VERIFY_PHONE_CODE_RSP:
    case GSDI_PERSO_CHANGE_PHONE_CODE_RSP:
    case GSDI_PERSO_UNBLOCK_PHONE_CODE_RSP:
    case GSDI_PERSO_SET_PHONE_CODE_RSP:
      pCmdStatus->nPinID = AEECARD_PHONE_CODE;
      break;
    default:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      if (gsdi_info->message_header.slot == GSDI_SLOT_1)
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
      {
        if (pin_id == GSDI_PIN1)
        {
          pinStatus = &pin1_status;
          pCmdStatus->nPinID = AEECARD_PIN1;
        }
        else if (pin_id == GSDI_PIN2)
        {
          pinStatus = &pin2_status;
          pCmdStatus->nPinID = AEECARD_PIN2;
        }
        else
        {
          MSG_ERROR("Unknown PIN to OEMCardGSDIcmdCB 0x%x", pin_id, 0, 0);
          OEMCard_CleanUpAsynObject(Handler);
          return;
        }
        /* Status word */
        (void)MEMCPY(&pCmdStatus->sStatus, &sSW, sizeof(AEECardStatusWord));
        pinStatus->tries_remaining = (uint8)retries;
      }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      else {
        if (pin_id == GSDI_PIN1)
        {
          pinStatus = &pin1_slot2_status;
          pCmdStatus->nPinID = AEECARD_PIN1_SLOT2;
        }
        else if (pin_id == GSDI_PIN2)
        {
          pinStatus = &pin2_slot2_status;
          pCmdStatus->nPinID = AEECARD_PIN2_SLOT2;
        }
        else
        {
          MSG_ERROR("Unknown PIN to OEMCardGSDIcmdCB 0x%x", pin_id, 0, 0);
          OEMCard_CleanUpAsynObject(Handler);
          return;
        }
        /* Status word */
        (void)MEMCPY(&pCmdStatus->sStatus, &sSW, sizeof(AEECardStatusWord));
        pinStatus->tries_remaining = (uint8)retries;
      }
#endif
      break;
  }

  /* Validate user callback to ensure its still valid */
  if (pAsynInfo->pCB->pfnCancel != NULL) {
#if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback(pAsynInfo->pCB,  0);
#else
    AEE_ResumeEx(pAsynInfo->pCB, 0, 0);
#endif
  }
  else {
    MSG_ERROR("GSDICmdCB: pAsynInfo->pCB->pfnCancel NULL", 0, 0, 0 );
  }
} /* OEMCardGSDIcmdCB */

/*===========================================================================
FUNCTION: OEMCard_FindSequenceNumber

DESCRIPTION
  Find the next available response slot for the generic command response

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_FindSequenceNumber(uint32 *dwSeqNum,
                               int     nSize,
                               uint8  *byData)
{
  uint32 i = 0;

  if (dwSeqNum == NULL)
  {
    return EBADPARM;
  }
  for (i = 0; i< OEMCARD_MAX_RSP; i++) {
    if (sCardRspTable[i].sRsp == NULL) {
      sCardRspTable[i].sRsp = OEM_Malloc(nSize);
      if (sCardRspTable[i].sRsp != NULL)
      {
        (void)MEMCPY(sCardRspTable[i].sRsp,
          byData,
          nSize);
        *dwSeqNum = i;
        sCardRspTable[i].nSize = nSize;
        return SUCCESS;
      }
      else {
        return ENOMEMORY;
      }
    }
  }
  return EFAILED;
} /* OEMCard_FindSequenceNumber */

/*===========================================================================
FUNCTION: OEMCard_MapFromGSDIFileEnum

DESCRIPTION
  Map AEECard File Enum to GSDI File Enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_MapFromGSDIFileEnum(gsdi_file_enum_type sFileEnum,
                               AEECardFileItemEnumType *pCardFileEnum
                               )
{
  int i = 0;
  int nTableSize = ARR_SIZE(OEMCardGSDIFileEnumTable);

  if (NULL == pCardFileEnum) {
    MSG_ERROR("_MapFromGSDIFileEnum: pCardFileEnum Null", 0, 0, 0);
    return EFAILED;
  }

  for ( i = 0; i < nTableSize; i++ ) {
    if (sFileEnum == OEMCardGSDIFileEnumTable[i].nGSDIFile){
      *pCardFileEnum = OEMCardGSDIFileEnumTable[i].nCardFile;
      return SUCCESS;
    }
  }
  return EFAILED;
} /* OEMCard_MapFromGSDIFileEnum */

/*===========================================================================

FUNCTION: OEMCardGSDIGenericcmdCB

DESCRIPTION
  This function is called when OEMCard calls GSDI during normal operation
  for FDN related procedure.
  This will cause a callback to be issued to the caller.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCardGSDIGenericcmdCB(gsdi_cnf_T *gsdi_info)
{
  AEEObjectHandle          Handler;
  OEMCard_AsynCBInfo      *pAsynInfo;
  AEECardGenericCmdStatus *pCmdStatus;
  AEECardFileAttribStatus *pFileAttrStatus;
#ifdef FEATURE_CPHS
#error code not present
#endif /*FEATURE_CPHS */
  
  if ((NULL == gsdi_info) || (NULL == ICardobj)) {
    MSG_ERROR("GSDIGenericcmdCB: gsdi_info is Null:0x%x or ICardobj is Null:0x%x",
              gsdi_info,ICardobj,0);
    return;
  }

  CARD_ENTER_CRITICAL_SECTION
  Handler= (AEEObjectHandle)gsdi_info->message_header.client_ref;
  pAsynInfo = (OEMCard_AsynCBInfo*)AEEObjectMgr_GetObject(Handler);
  CARD_LEAVE_CRITICAL_SECTION

  if (NULL == pAsynInfo) {
    MSG_ERROR("GSDIGenericcmdCB: pAsynInfo is Null",0,0,0);
    return;
  }

  if (pAsynInfo->Handler != Handler) {
    MSG_ERROR("GSDIGenericcmdCB: pAsynInfo->Handler mismatch 0x%x vs 0x%x",
      pAsynInfo->Handler,Handler,0);
    /* do not clean up the content because it could be invalid data, e.g.,
       we have a case where the data reflected all unused address space with
       value not == 0 */
    return;
  }

  if (NULL == pAsynInfo->pCB) {
    MSG_ERROR("GSDIGenericcmdCB: pAsynInfo->pCB is Null",0,0,0);
    OEMCard_CleanUpAsynObject(Handler);
    return;
  }

  pCmdStatus  = pAsynInfo->pClientData;
  pFileAttrStatus  = pAsynInfo->pClientData;
#ifdef FEATURE_CPHS
#error code not present
#endif /*FEATURE_CPHS */
  if (NULL == pCmdStatus) {
    MSG_ERROR("GSDIGenericcmdCB: pCmdStatus is Null",0,0,0);
    OEMCard_CleanUpAsynObject(Handler);
    return;
  }

  if (GSDI_SUCCESS == gsdi_info->message_header.gsdi_status)
  {
    pCmdStatus->nCmdStatus = SUCCESS;
    switch(gsdi_info->message_header.resp_type)
    {
    case GSDI_ENABLE_FDN_RSP:
      pCmdStatus->dwData = 0x00;
      break;
    case GSDI_DISABLE_FDN_RSP:
      pCmdStatus->dwData = 0x00;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SWITCH_SLOT_PREF_RSP:
      if (gsdi_info->switch_slot_pref_cnf.refresh_pending) {
        pCmdStatus->dwData = (uint32)TRUE;
      }
      else{
        pCmdStatus->dwData = (uint32)FALSE;
      }
      break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    case GSDI_GET_FILE_ATTRIBUTES_RSP:
      pFileAttrStatus->dwStructureSize = sizeof(AEECardFileAttribStatus);
      pFileAttrStatus->nCmdStatus = OEMCard_PopulateFileAttribBuf(pFileAttrStatus,
                                             &(gsdi_info->gfa_cnf.gfa_data));
      break;

    case GSDI_SIM_READ_RSP:
      {
        gsdi_file_enum_type gsdiFileEnum;

        pCmdStatus->nCmdStatus = OEMCard_FindSequenceNumber(&(pCmdStatus->dwData),
                                                              gsdi_info->read_cnf.returned_data_len,
                                                              gsdi_info->read_cnf.data);

        if (gsdi_util_map_uim_enum_to_file_enum((uim_items_enum_type)gsdi_info->read_cnf.sim_filename ,&gsdiFileEnum)
          != GSDI_SUCCESS)
        {
          MSG_ERROR("Unable to match file uim enum 0x%x",
            (uim_items_enum_type)gsdi_info->read_cnf.sim_filename, 0, 0);
          pCmdStatus->nCmdStatus = EFAILED;
          OEMCard_FreeSequenceNumber(pCmdStatus->dwData);
          break;
        }
        if (OEMCard_MapFromGSDIFileEnum(gsdiFileEnum,&pCmdStatus->fileName) != SUCCESS)
        {
          MSG_ERROR("Unable to match file gsdi enum 0x%x",
            gsdiFileEnum, 0, 0);
          pCmdStatus->nCmdStatus = EFAILED;
          OEMCard_FreeSequenceNumber(pCmdStatus->dwData);
        }
      }
      break;
    case GSDI_SIM_WRITE_RSP:
      pCmdStatus->dwData = 0;
      pCmdStatus->nCmdStatus = SUCCESS;
      break;
#ifdef FEATURE_CPHS
#error code not present
#endif /*FEATURE_CPHS */
    default:
      MSG_ERROR("GSDIGenericcmdCB: Unsupported command cnf type 0x%x",
                gsdi_info->message_header.resp_type, 0, 0);
      break;
    }
  }
  else {
    pCmdStatus->nCmdStatus = EFAILED;
    pFileAttrStatus->nCmdStatus = EFAILED;
#ifdef FEATURE_CPHS
#error code not present
#endif
  }


#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (gsdi_info->message_header.slot == GSDI_SLOT_1)
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
  {
    if ( gsdi_info->message_header.resp_type == GSDI_GET_FILE_ATTRIBUTES_RSP) {
      pFileAttrStatus->nSlotID = AEECARD_SLOT1;
    }
#ifdef FEATURE_CPHS
#error code not present
#endif /*FEATURE_CPHS */
    else {
      pCmdStatus->nSlotID = AEECARD_SLOT1;
    }
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else {
    if ( gsdi_info->message_header.resp_type == GSDI_GET_FILE_ATTRIBUTES_RSP) {
      pFileAttrStatus->nSlotID = AEECARD_SLOT2;
    }
#ifdef FEATURE_CPHS
#error code not present
#endif /*FEATURE_CPHS */
    else {
      pCmdStatus->nSlotID = AEECARD_SLOT2;
    }
  }
#endif

  /* Validate user callback to ensure its still valid */
  if (pAsynInfo->pCB->pfnCancel != NULL) {
#if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback(pAsynInfo->pCB,  0);
#else
    AEE_ResumeEx(pAsynInfo->pCB, 0, 0);
#endif
  }
  else {
    MSG_ERROR("GSDIGenericcmdCB: pAsynInfo->pCB->pfnCancel NULL", 0, 0, 0 );
  }
} /* OEMCardGSDIGenericcmdCB */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================

FUNCTION: OEMCardGSDIPersocmdCB

DESCRIPTION
  This function is called when OEMCard calls GSDI Personalization function
  This will cause a callback to be issued to the caller.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCardGSDIPersocmdCB(gsdi_cnf_T *gsdi_info)
{
  AEEObjectHandle                   Handler;
  OEMCard_AsynCBInfo               *pAsynInfo;
  AEECardGenericCmdStatus          *pCmdStatus;
  AEECardDeactivatePersoCmdStatus  *pDeactivateCmdStatus;
  AEECardGetPersoFeatureCmdStatus  *pGetFeatureCmdStatus;
  AEECardDCKNumRetries             *pDCKNumRetriesStatus;
#if defined (FEATURE_MMGSDI_PERSONALIZATION_ENGINE) && defined(FEATURE_MMGSDI_PERSONALIZATION_NON_QC)
  AEECardPhoneCodeStatus           *pPhoneCodeStatus;
#endif

  if ((NULL == gsdi_info) || (NULL == ICardobj)) {
    MSG_ERROR("GSDIPersocmdCB: gsdi_info Null: 0x%x or ICardobj Null: 0x%x",
              gsdi_info, ICardobj, 0);
    return;
  }

  MSG_HIGH("GSDIPersocmdCB: resp_type is 0x%x",
            gsdi_info->message_header.resp_type, 0, 0);

  if (gsdi_info->message_header.resp_type == GSDI_PERSO_REG_TASK_RSP) {
    /* ICard registered from Perso Event */
    if (gsdi_info->message_header.gsdi_status != GSDI_SUCCESS) {
      MSG_ERROR("Unable to register Perso Event 0x%x",
           gsdi_info->message_header.gsdi_status, 0, 0);
    }
    return;
  }
  
  CARD_ENTER_CRITICAL_SECTION
  Handler= (AEEObjectHandle)gsdi_info->message_header.client_ref;
  pAsynInfo = (OEMCard_AsynCBInfo*)AEEObjectMgr_GetObject(Handler);
  CARD_LEAVE_CRITICAL_SECTION

  if (NULL == pAsynInfo) {
    MSG_ERROR("GSDIPersocmdCB: pAsynInfo is Null", 0, 0, 0);
    return;
  }

  if (pAsynInfo->Handler != Handler) {
    MSG_ERROR("GSDIPersocmdCB: pAsynInfo->Handler mismatch 0x%x vs 0x%x",
      pAsynInfo->Handler,Handler,0);
    /* do not clean up the content because it could be invalid data, e.g.,
       we have a case where the data reflected all unused address space with
       value not == 0 */
    return;
  }

  if (NULL == pAsynInfo->pCB) {
    OEMCard_CleanUpAsynObject(Handler);
    MSG_ERROR("GSDIPersocmdCB: pAsynInfo->pCB is Null", 0, 0, 0);
    return;
  }

  pCmdStatus  = pAsynInfo->pClientData;
  pDeactivateCmdStatus = pAsynInfo->pClientData;
  pGetFeatureCmdStatus = pAsynInfo->pClientData;
  pDCKNumRetriesStatus = pAsynInfo->pClientData;
#if defined (FEATURE_MMGSDI_PERSONALIZATION_ENGINE) && defined(FEATURE_MMGSDI_PERSONALIZATION_NON_QC)
  pPhoneCodeStatus = pAsynInfo->pClientData;
#endif

  if (NULL == pCmdStatus) {
    OEMCard_CleanUpAsynObject(Handler);
    MSG_ERROR("GSDIPersocmdCB: pCmdStatus is Null", 0, 0, 0);
    return;
  }

  switch(gsdi_info->message_header.resp_type)
  {
  case GSDI_PERSO_ACT_IND_RSP:
    if (GSDI_SUCCESS == gsdi_info->message_header.gsdi_status) {
      pDeactivateCmdStatus->nCmdStatus = SUCCESS;
    }
    else {
      pDeactivateCmdStatus->nCmdStatus = EFAILED;
    }

    if (OEMCard_MapGSDIFeatureToCardFeature(gsdi_info->act_ind_cnf.perso_feature,
                                        &pDeactivateCmdStatus->nPersoFeatureID) != SUCCESS)
    {
      MSG_ERROR("Unable to map GSDI Feature to ICard Feature 0x%x",
        gsdi_info->act_ind_cnf.perso_feature, 0, 0);
      pDeactivateCmdStatus->nCmdStatus = EFAILED;
      break;
    }
    if ((gsdi_info->message_header.gsdi_status == GSDI_SUCCESS) ||
        (gsdi_info->message_header.gsdi_status == GSDI_PERSO_INVALID_CK) ||
        (gsdi_info->message_header.gsdi_status == GSDI_PERSO_CK_BLOCKED)) {
      pDeactivateCmdStatus->byRetries = gsdi_info->act_ind_cnf.num_retries;
    }
    else {
      pDeactivateCmdStatus->byRetries = 0xFF;
    }
    break;
  case GSDI_PERSO_DEACT_IND_RSP:
    if (GSDI_SUCCESS == gsdi_info->message_header.gsdi_status) {
      pDeactivateCmdStatus->nCmdStatus = SUCCESS;
    }
    else {
      pDeactivateCmdStatus->nCmdStatus = EFAILED;
    }

    if (OEMCard_MapGSDIFeatureToCardFeature(gsdi_info->dact_ind_cnf.perso_feature,
                                        &pDeactivateCmdStatus->nPersoFeatureID) != SUCCESS)
    {
      MSG_ERROR("Unable to map GSDI Feature to ICard Feature 0x%x",
        gsdi_info->act_ind_cnf.perso_feature, 0, 0);
      pDeactivateCmdStatus->nCmdStatus = EFAILED;
      break;
    }
    if ((gsdi_info->message_header.gsdi_status == GSDI_SUCCESS) ||
        (gsdi_info->message_header.gsdi_status == GSDI_PERSO_INVALID_CK) ||
        (gsdi_info->message_header.gsdi_status == GSDI_PERSO_CK_BLOCKED)) {
      pDeactivateCmdStatus->byRetries = gsdi_info->act_ind_cnf.num_retries;
    }
    else {
      pDeactivateCmdStatus->byRetries = 0xFF;
    }
    break;
  case GSDI_PERSO_GET_IND_RSP:
    if (GSDI_SUCCESS == gsdi_info->message_header.gsdi_status) {
      pGetFeatureCmdStatus->nCmdStatus = SUCCESS;
#ifdef FEATURE_PERSO_SIM
      pGetFeatureCmdStatus->bCPIndStatus = gsdi_info->get_ind_cnf.cp_ind_status;
      pGetFeatureCmdStatus->bNWIndStatus = gsdi_info->get_ind_cnf.nw_ind_status;
      pGetFeatureCmdStatus->bSIMIndStatus = gsdi_info->get_ind_cnf.sim_ind_status;
      pGetFeatureCmdStatus->bSPIndStatus = gsdi_info->get_ind_cnf.sp_ind_status;
      pGetFeatureCmdStatus->bNSIndStatus = gsdi_info->get_ind_cnf.ns_ind_status;
#endif
#ifdef FEATURE_PERSO_RUIM
      pGetFeatureCmdStatus->bRUIMNW1IndStatus  = gsdi_info->get_ind_cnf.ruim_nw1_ind_status;
      pGetFeatureCmdStatus->bRUIMNW2IndStatus  = gsdi_info->get_ind_cnf.ruim_nw2_ind_status;
      pGetFeatureCmdStatus->bRUIMHRPDIndStatus = gsdi_info->get_ind_cnf.ruim_hrpd_ind_status;
      pGetFeatureCmdStatus->bRUIMSPIndStatus   = gsdi_info->get_ind_cnf.ruim_sp_ind_status;

      pGetFeatureCmdStatus->bRUIMCPIndStatus   = gsdi_info->get_ind_cnf.ruim_cp_ind_status;
      pGetFeatureCmdStatus->bRUIMRUIMIndStatus = gsdi_info->get_ind_cnf.ruim_ruim_ind_status;
#endif
#if defined(FEATURE_MMGSDI_PERSONALIZATION_NON_QC)
      pGetFeatureCmdStatus->bProp1IndStatus = gsdi_info->get_ind_cnf.prop1_ind_status;
#else
      pGetFeatureCmdStatus->bProp1IndStatus = FALSE;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
    }
    else {
      pGetFeatureCmdStatus->nCmdStatus = EFAILED;
    }
    break;
  case GSDI_PERSO_SET_DATA_RSP:
    if (GSDI_SUCCESS == gsdi_info->message_header.gsdi_status) {
      pCmdStatus->nCmdStatus = SUCCESS;
    }
    else {
      pCmdStatus->nCmdStatus = EFAILED;
    }
    break;
  case GSDI_PERSO_UNBLOCK_IND_RSP:
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
    if (GSDI_SUCCESS == gsdi_info->message_header.gsdi_status) {
      pDeactivateCmdStatus->nCmdStatus = SUCCESS;
    }
    else {
      pDeactivateCmdStatus->nCmdStatus = EFAILED;
    }

    pDeactivateCmdStatus->byRetries = 0xFF;

    if (OEMCard_MapGSDIFeatureToCardFeature(gsdi_info->dact_ind_cnf.perso_feature,
                                        &pDeactivateCmdStatus->nPersoFeatureID) != SUCCESS)
    {
      MSG_ERROR("Unable to map GSDI Feature to ICard Feature 0x%x",
                         gsdi_info->act_ind_cnf.perso_feature, 0, 0);
    }
    
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
    break;

  case GSDI_PERSO_GET_DCK_NUM_RETRIES_RSP:
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
    if (GSDI_SUCCESS == gsdi_info->message_header.gsdi_status) {
      pDCKNumRetriesStatus->nCmdStatus = SUCCESS;
#ifdef FEATURE_PERSO_SIM
      pDCKNumRetriesStatus->nNW_NumRetries = gsdi_info->dck_num_retries_cnf.nw_num_retries;
      pDCKNumRetriesStatus->nNS_NumRetries = gsdi_info->dck_num_retries_cnf.ns_num_retries;
      pDCKNumRetriesStatus->nSP_NumRetries = gsdi_info->dck_num_retries_cnf.sp_num_retries;
      pDCKNumRetriesStatus->nCP_NumRetries = gsdi_info->dck_num_retries_cnf.cp_num_retries;
      pDCKNumRetriesStatus->nSIM_NumRetries = gsdi_info->dck_num_retries_cnf.sim_num_retries;
#endif
#ifdef FEATURE_PERSO_RUIM
      pDCKNumRetriesStatus->nRUIMNW1_NumRetries = gsdi_info->dck_num_retries_cnf.ruim_nw1_num_retries;
      pDCKNumRetriesStatus->nRUIMNW2_NumRetries = gsdi_info->dck_num_retries_cnf.ruim_nw2_num_retries;
      pDCKNumRetriesStatus->nRUIMHRPD_NumRetries= gsdi_info->dck_num_retries_cnf.ruim_hrpd_num_retries;
      pDCKNumRetriesStatus->nRUIMSP_NumRetries  = gsdi_info->dck_num_retries_cnf.ruim_sp_num_retries;
      pDCKNumRetriesStatus->nRUIMCP_NumRetries  = gsdi_info->dck_num_retries_cnf.ruim_cp_num_retries;
      pDCKNumRetriesStatus->nRUIMRUIM_NumRetries= gsdi_info->dck_num_retries_cnf.ruim_ruim_num_retries;
#endif
#if defined(FEATURE_MMGSDI_PERSONALIZATION_NON_QC)
      pDCKNumRetriesStatus->nProp1_NumRetries = gsdi_info->dck_num_retries_cnf.prop1_num_retries;
#else
      pDCKNumRetriesStatus->nProp1_NumRetries = 0;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

    }
    else {
      pDCKNumRetriesStatus->nCmdStatus = EFAILED;
    }

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
    break;

  case GSDI_PERSO_GET_PHONE_CODE_STATUS_RSP:
#if defined (FEATURE_MMGSDI_PERSONALIZATION_ENGINE) && defined(FEATURE_MMGSDI_PERSONALIZATION_NON_QC)
    switch (gsdi_info->message_header.gsdi_status)
    {
    case GSDI_SUCCESS:
      pPhoneCodeStatus->nCmdStatus = SUCCESS;
      pPhoneCodeStatus->nNumRetries = gsdi_info->get_phone_code_cnf.num_retries;
      switch(gsdi_info->get_phone_code_cnf.status)
      {
      case GSDI_PERSO_PHONECODE_NOT_INIT:
        pPhoneCodeStatus->nStatus = AEECARD_PIN_NOT_INIT;
        break;
      case GSDI_PERSO_PHONECODE_INIT:
        pPhoneCodeStatus->nStatus = AEECARD_PIN_ENABLED;
        break;
      case GSDI_PERSO_PHONECODE_MAX_ENUM:
        pPhoneCodeStatus->nStatus = AEECARD_PIN_NOT_VALID;
        break;
      }
      break;
    case GSDI_PERSO_PROP2_STATUS:
      pPhoneCodeStatus->nCmdStatus = SUCCESS;
      pPhoneCodeStatus->nNumRetries = 0;
      pPhoneCodeStatus->nStatus = AEECARD_PIN_BLOCKED;
      break;
    default:
      pPhoneCodeStatus->nCmdStatus = EFAILED;
    }

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE && FEATURE_MMGSDI_PERSONALIZATION_NON_QC*/
    break;

  case GSDI_PERSO_SET_PHONE_CODE_RSP:
#if defined (FEATURE_MMGSDI_PERSONALIZATION_ENGINE) && defined(FEATURE_MMGSDI_PERSONALIZATION_NON_QC)
    if (GSDI_SUCCESS == gsdi_info->message_header.gsdi_status) {
      pCmdStatus->nCmdStatus = SUCCESS;
    }
    else {
      pCmdStatus->nCmdStatus = EFAILED;
    }
#endif /* #if defined (FEATURE_MMGSDI_PERSONALIZATION_ENGINE) && defined(FEATURE_MMGSDI_PERSONALIZATION_NON_QC)*/
    break;
  default:
    break;
  }

  if (gsdi_info->message_header.resp_type != GSDI_PERSO_GET_PHONE_CODE_STATUS_RSP )
  {
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if (gsdi_info->message_header.slot == GSDI_SLOT_1)
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    {
      pCmdStatus->nSlotID = AEECARD_SLOT1;
      pDeactivateCmdStatus->nSlotID = AEECARD_SLOT1;
      pGetFeatureCmdStatus->nSlotID = AEECARD_SLOT1;
    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    else
    {
      pCmdStatus->nSlotID = AEECARD_SLOT2;
      pDeactivateCmdStatus->nSlotID = AEECARD_SLOT2;
      pGetFeatureCmdStatus->nSlotID = AEECARD_SLOT2;
    }
#endif
  }
  /* Validate user callback to ensure its still valid */
  if (pAsynInfo->pCB->pfnCancel != NULL) {
#if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback(pAsynInfo->pCB,  0);
#else
    AEE_ResumeEx(pAsynInfo->pCB, 0, 0);
#endif
  }
  else {
    MSG_ERROR("GSDIPersocmdCB: pAsynInfo->pCB->pfnCancel NULL", 0, 0, 0 );
  }
} /* OEMCardGSDIPersocmdCB */
#endif /*FEATURE_MMGSDI_PERSONALIZATION */

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
/*===========================================================================

FUNCTION: OEMCard_OpenCancel

DESCRIPTION
   Cancels the user callback

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCard_OpenCancel(AEECallback *pcb)
{
  OEMCard_AsynCBInfo *pAsyncInfo = NULL; 
  int                 i          = 0;
  AEEObjectHandle     Handler;

  CARD_ENTER_CRITICAL_SECTION

  if (pcb != NULL) {
    Handler= (AEEObjectHandle)pcb->pCancelData;
    pAsyncInfo = (OEMCard_AsynCBInfo*)AEEObjectMgr_GetObject(Handler);
    
    if (pAsyncInfo != NULL) {

      for (i = 0; i< OEMCARDCONNECTION_MAX_CHANNEL; i++ )
      {
        if (pAsyncInfo->pObj == (void*)sCardChannelMapTable[i].pICardConnection_obj)
        {
          if (sCardChannelMapTable[i].bChannelStatus == OEMCARDCONNECTION_OPENING)
          {
            sCardChannelMapTable[i].nRequestCancel = OEMCARD_CANCEL_REASON_CB_CANCEL;
            break;
          }
        }
      }
      (void)AEEObjectMgr_Unregister(pAsyncInfo->Handler);
      if (pAsyncInfo->pCB != NULL) {
        AEECallback *pAsyncCB = (AEECallback*)(pAsyncInfo->pCB);
        pAsyncCB->pCancelData = NULL;
      }
      FREE(pAsyncInfo);
      pAsyncInfo = NULL; /* setting pcb->pCB->pfnCancel = NULL; */
    }
    pcb->pfnCancel = NULL;
  }
  CARD_LEAVE_CRITICAL_SECTION
} /* OEMCard_OpenCancel */
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

/*===========================================================================

FUNCTION: OEMCard_Cancel

DESCRIPTION
   Cancels the user callback

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCard_Cancel(AEECallback *pcb)
{
  OEMCard_AsynCBInfo *pAsyncInfo = NULL;
  AEEObjectHandle     Handler;

  CARD_ENTER_CRITICAL_SECTION
  if (pcb != NULL) {
    Handler= (AEEObjectHandle)pcb->pCancelData;

    pAsyncInfo = (OEMCard_AsynCBInfo*)AEEObjectMgr_GetObject(Handler);
 
    if (pAsyncInfo != NULL) {
      (void)AEEObjectMgr_Unregister(pAsyncInfo->Handler);
      if (pAsyncInfo->pCB != NULL) {
        AEECallback *pAsyncCB = (AEECallback*)(pAsyncInfo->pCB);
        pAsyncCB->pCancelData = NULL;
      }
      FREE(pAsyncInfo);
      pAsyncInfo = NULL; /* setting pcb->pCB->pfnCancel = NULL; */
    }
    pcb->pfnCancel = NULL;
  }
  CARD_LEAVE_CRITICAL_SECTION
} /* OEMCard_Cancel */

#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP) */

/*===========================================================================
FUNCTION: OEMCard_CheckValidCardStatus

DESCRIPTION
  Check for Card Status to determine if it is valid

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_CheckValidCardStatus(gsdi_slot_id_type sGSDISlotID)
{
  uint8 card_status;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (sGSDISlotID == GSDI_SLOT_2) {
    // Check to see if the card is connected.
    if (SUCCESS != OEMCard_CardStatus (AEECARD_SLOT2, &card_status) ||
        ((card_status != AEECARD_VALID_CARD) &&
         (card_status != AEECARD_NOT_INIT)) &&
         (card_status != AEECARD_AVAIL_CARD) &&
         (card_status != AEECARD_CARD_REINIT) &&
#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif
        )
    {
      return EFAILED;
    }
  }
  else
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  {
    // Check to see if the card is connected.
    if (SUCCESS != OEMCard_CardStatus (AEECARD_SLOT1, &card_status))
    {
      return EFAILED;
    }

    if(  (card_status != AEECARD_VALID_CARD)
      && (card_status != AEECARD_NOT_INIT)
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      && (card_status != AEECARD_AVAIL_CARD)
#endif
#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif
      && (card_status != AEECARD_CARD_REINIT))
    {
      return EFAILED;
    }
  }
  return SUCCESS;
} /* OEMCard_CheckValidCardStatus */ /*lint !e715 */

#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* #if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP) */

/*===========================================================================
FUNCTION: OEMCard_MapService

DESCRIPTION
  Map the AEECard service to GSDI service

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_MapService(AEECardServiceEnumType dwService, gsdi_services_type *gsdi_service)
{
  if (NULL == gsdi_service) {
    MSG_ERROR("_MapService: gsdi_service Null", 0, 0, 0);
    return EBADPARM;
  }

  switch(dwService) {
  case AEECARD_CHV1_DISABLE:
    *gsdi_service = GSDI_CHV1_DISABLE;
    break;
  case AEECARD_ADN:
    *gsdi_service = GSDI_ADN;
    break;
  case AEECARD_FDN:
    *gsdi_service = GSDI_FDN;
    break;
  case AEECARD_LND:
    *gsdi_service = GSDI_LND;
    break;
  case AEECARD_BDN:
    *gsdi_service = GSDI_BDN;
    break;
  case AEECARD_LOCAL_PHONEBOOK:
    *gsdi_service = GSDI_LOCAL_PHONEBOOK;
    break;
  case AEECARD_GLOBAL_PHONEBOOK:
    *gsdi_service = GSDI_GLOBAL_PHONEBOOK;
    break;
  case AEECARD_AOC:
    *gsdi_service = GSDI_AOC;
    break;
  case AEECARD_OCI_OCT:
    *gsdi_service = GSDI_OCI_OCT;
    break;
  case AEECARD_ICI_ICT:
    *gsdi_service = GSDI_ICI_ICT;
    break;
  case AEECARD_CCP:
    *gsdi_service = GSDI_CCP;
    break;
  case AEECARD_EXT_CCP:
    *gsdi_service = GSDI_EXT_CCP;
    break;
  case AEECARD_MSISDN:
    *gsdi_service = GSDI_MSISDN;
    break;
  case AEECARD_EXT1:
    *gsdi_service = GSDI_EXT1;
    break;
  case AEECARD_EXT2:
    *gsdi_service = GSDI_EXT2;
    break;
  case AEECARD_EXT3:
    *gsdi_service = GSDI_EXT3;
    break;
  case AEECARD_EXT4:
    *gsdi_service = GSDI_EXT4;
    break;
  case AEECARD_EXT5:
    *gsdi_service = GSDI_EXT5;
    break;
  case AEECARD_EXT8:
    *gsdi_service = GSDI_EXT8;
    break;
  case AEECARD_SDN:
    *gsdi_service = GSDI_SDN;
    break;
  case AEECARD_SMS:
    *gsdi_service = GSDI_SMS;
    break;
  case AEECARD_SMSP:
    *gsdi_service = GSDI_SMSP;
    break;
  case AEECARD_SMSR:
    *gsdi_service = GSDI_SMSR;
    break;
  case AEECARD_GID1:
    *gsdi_service = GSDI_GID1;
    break;
  case AEECARD_GID2:
    *gsdi_service = GSDI_GID2;
    break;
  case AEECARD_SPN:
    *gsdi_service = GSDI_SPN;
    break;
  case AEECARD_VGCS_GID_LIST:
    *gsdi_service = GSDI_VGCS_GID_LIST;
    break;
  case AEECARD_VBS_GID_LIST:
    *gsdi_service = GSDI_VBS_GID_LIST;
    break;
  case AEECARD_ENH_ML_SVC:
    *gsdi_service = GSDI_ENH_ML_SVC;
    break;
  case AEECARD_AA_EMLPP:
    *gsdi_service = GSDI_AA_EMLPP;
    break;
  case AEECARD_DATA_DL_SMSCB:
    *gsdi_service = GSDI_DATA_DL_SMSCB;
    break;
  case AEECARD_DATA_DL_SMSPP:
    *gsdi_service = GSDI_DATA_DL_SMSPP;
    break;
  case AEECARD_MENU_SEL:
    *gsdi_service = GSDI_MENU_SEL;
    break;
  case AEECARD_CALL_CONTROL:
    *gsdi_service = GSDI_CALL_CONTROL;
    break;
  case AEECARD_MOSMS_CONTROL:
    *gsdi_service = GSDI_MOSMS_CONTROL;
    break;
  case AEECARD_PROACTIVE_SIM:
    *gsdi_service = GSDI_PROACTIVE_SIM;
    break;
  case AEECARD_USSD:
    *gsdi_service = GSDI_USSD;
    break;
  case AEECARD_RUN_AT_CMD:
    *gsdi_service = GSDI_RUN_AT_CMD;
    break;
  case AEECARD_GPRS_USIM:
    *gsdi_service = GSDI_GPRS_USIM;
    break;
  case AEECARD_NETWK_ALTERING_MS:
    *gsdi_service = GSDI_NETWK_ALTERING_MS;
    break;
  case AEECARD_CBMID_RANGES:
    *gsdi_service = GSDI_CBMID_RANGES;
    break;
  case AEECARD_CBMI:
    *gsdi_service = GSDI_CBMI;
    break;
  case AEECARD_DEPERSON_KEYS:
    *gsdi_service = GSDI_DEPERSON_KEYS;
    break;
  case AEECARD_COOP_NETWK_LIST:
    *gsdi_service = GSDI_COOP_NETWK_LIST;
    break;
  case AEECARD_GPRS:
    *gsdi_service = GSDI_GPRS;
    break;
  case AEECARD_IMAGE:
    *gsdi_service = GSDI_IMAGE;
    break;
  case AEECARD_SOLSA:
    *gsdi_service = GSDI_SOLSA;
    break;
  case AEECARD_SP_DISP_INFO:
    *gsdi_service = GSDI_SP_DISP_INFO;
    break;
  case AEECARD_GSM_ACCESS_IN_USIM:
    *gsdi_service = GSDI_GSM_ACCESS_IN_USIM;
    break;
  case AEECARD_GSM_SECURITY_CONTEXT:
    *gsdi_service = GSDI_GSM_SECURITY_CONTEXT;
    break;
  case AEECARD_PLMN_SELECTOR:
    *gsdi_service = GSDI_PLMN_SELECTOR;
    break;
  case AEECARD_UPLMN_SEL_WACT:
    *gsdi_service = GSDI_UPLMN_SEL_WACT;
    break;
  case AEECARD_RPLMN_LACT:
    *gsdi_service = GSDI_RPLMN_LACT;
    break;
  case AEECARD_OPLMN_SEL_WACT:
    *gsdi_service = GSDI_OPLMN_SEL_WACT;
    break;
  case AEECARD_HPLMN_WACT:
    *gsdi_service = GSDI_HPLMN_WACT;
    break;
  case AEECARD_OPLMN_LIST:
    *gsdi_service = GSDI_OPLMN_LIST;
    break;
  case AEECARD_PLMN_NTWRK_NAME:
    *gsdi_service = GSDI_PLMN_NTWRK_NAME;
    break;
  case AEECARD_MMS:
    *gsdi_service = GSDI_MMS;
    break;
  case AEECARD_MMS_USR_P:
    *gsdi_service = GSDI_MMS_USR_P;
    break;
  case AEECARD_EST:
    *gsdi_service = GSDI_EST;
    break;
  case AEECARD_ACL:
    *gsdi_service = GSDI_ACL;
    break;
  case AEECARD_CPBCCH:
    *gsdi_service = GSDI_CPBCCH;
    break;
  case AEECARD_INV_SCAN:
    *gsdi_service = GSDI_INV_SCAN;
    break;
  case AEECARD_MEXE:
    *gsdi_service = GSDI_MEXE;
    break;
  case AEECARD_MAILBOX_DIAL:
    *gsdi_service = GSDI_MAILBOX_DIAL;
    break;
  case AEECARD_MSG_WAIT:
    *gsdi_service = GSDI_MSG_WAIT;
    break;
  case AEECARD_CALL_FWD_IND:
    *gsdi_service = GSDI_CALL_FWD_IND;
    break;
  default:
    MSG_ERROR("_MapService: Unsupported dwService 0x%x",
              dwService, 0, 0);
    return EUNSUPPORTED;
  }
  return SUCCESS;
} /* OEMCard_MapService */

/*===========================================================================
FUNCTION: OEMCard_MapNextServiceEnum

DESCRIPTION
  Map the AEECard service to GSDI service if there is any alternate service
  in the GSDI enum list

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_MapNextServiceEnum(AEECardServiceEnumType dwService, gsdi_services_type *gsdi_service)
{
  if (NULL == gsdi_service) {
    MSG_ERROR("_MapNextServiceEnum: gsdi_service Null", 0, 0, 0);
    return EBADPARM;
  }

  switch(dwService) {
  case AEECARD_MOSMS_CONTROL:
    *gsdi_service =  GSDI_MO_SMS_BY_SIM;
    break;
  default:
    MSG_ERROR("_MapNextServiceEnum: Unsupported dwService 0x%x",
              dwService, 0, 0);
    return EUNSUPPORTED;
  }
  return SUCCESS;
} /* OEMCard_MapNextServiceEnum */

/*===========================================================================
FUNCTION: OEMCard_IsSpecialService

DESCRIPTION
  Check if the service requires any special or additional handling

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static boolean OEMCard_IsSpecialService(AEECardServiceEnumType dwService)
{
  switch(dwService) {
  case AEECARD_MOSMS_CONTROL:
    return TRUE;
  default:
    return FALSE;
  }
} /* OEMCard_IsSpecialService */

/*===========================================================================
FUNCTION: OEMCard_MapSlot

DESCRIPTION
  Map AEECARD slot to GSDI Slot enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_MapSlot(uint8 wSlot, gsdi_slot_id_type *gsdi_slot)
{
  if (NULL == gsdi_slot) {
    MSG_ERROR("_MapSlot: gsdi_slot Null", 0, 0, 0);
    return EFAILED;
  }
  switch (wSlot) {
    case AEECARD_SLOT1:
      *gsdi_slot = GSDI_SLOT_1;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case AEECARD_SLOT2:
      *gsdi_slot = GSDI_SLOT_2;
      break;
#endif/*FEATURE_MMGSDI_DUAL_SLOT */
    default:
      *gsdi_slot = GSDI_SLOT_NONE;
      return EBADPARM;
  }
  return SUCCESS;
} /* OEMCard_MapSlot */

#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* #if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP) */

/*===========================================================================
FUNCTION: OEMCard_MapPinEnum

DESCRIPTION
  Map AEECard Pin Enum to GSDI enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_MapPinEnum(uint8 byPinId, gsdi_pin_ids_T *pGSDIPinId,
                       gsdi_slot_id_type *pGSDISlotID)
{
  if ((NULL == pGSDIPinId) || (NULL == pGSDISlotID)) {
    MSG_ERROR("_MapPinEnum: pGSDIPinId Null 0x%x or pGSDISlotID Null 0x%x", 
              pGSDIPinId, pGSDISlotID, 0);
    return EBADPARM;
  }

  /* initialize to slot 2 */
  *pGSDISlotID = GSDI_SLOT_2;

    /* convert AEECARD pin enum to GSDI pin enum */
  switch(byPinId) {
  case AEECARD_PIN1:
    *pGSDISlotID = GSDI_SLOT_1;
    /* fall through */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case AEECARD_PIN1_SLOT2:
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    *pGSDIPinId = GSDI_PIN1;
    break;
  case AEECARD_PIN2:
    *pGSDISlotID = GSDI_SLOT_1;
    /* fall through */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case AEECARD_PIN2_SLOT2:
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    *pGSDIPinId = GSDI_PIN2;
    break;
  default:
    MSG_ERROR("_MapPinEnum: unsupported byPinId 0x%x", byPinId, 0, 0);
    return EFAILED;
  }
  return SUCCESS;
} /* OEMCard_MapPinEnum */

/*===========================================================================
FUNCTION: OEMCard_MapFileEnum

DESCRIPTION
  Map AEECard File Enum to GSDI File Enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_MapFileEnum(AEECardFileItemEnumType sCardFileEnum,
                       gsdi_file_enum_type *pFileEnum)
{
  int i = 0;
  int nTableSize = ARR_SIZE(OEMCardGSDIFileEnumTable);

  if (NULL == pFileEnum) {
    MSG_ERROR("_MapPinEnum: pFileEnum Null", 0, 0, 0);
    return EFAILED;
  }

  for ( i = 0; i < nTableSize; i++ ) {
    if (sCardFileEnum == OEMCardGSDIFileEnumTable[i].nCardFile) {
      *pFileEnum = OEMCardGSDIFileEnumTable[i].nGSDIFile;
      return SUCCESS;
    }
  }
  return EFAILED;
} /* OEMCard_MapFileEnum */

/*===========================================================================
FUNCTION: OEMCard_CheckFilePath

DESCRIPTION
  Check if File Path parameter is valid

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_CheckFilePath(AEECardFilePathType sFilePath)
{
  if (0 == sFilePath.nPathLen) {
    MSG_ERROR("_CheckFilePath: nPathLen 0", 0, 0, 0);
    return EFAILED;
  }
  if (NULL == sFilePath.pPath) {
    return EFAILED;
  }
  return SUCCESS;
} /* OEMCard_CheckFilePath */

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
/*===========================================================================

FUNCTION: OEMCard_GetLinkSysCallBackEx

DESCRIPTION
  Performs the AEE_LinkSysCallback operation.  Set the user passed in data
  in the reserve field

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_GetLinkSysCallBackEx(AEECallback *pUserCB, void *pCmdStatus,
                                        AEEObjectHandle *pHandler,
                                        void            *pObj,
                                        PFNCBCANCEL     pCancelCb)
{
  OEMCard_AsynCBInfo  *pAsynInfo = NULL;
  int                  nRet;

  if (NULL == pHandler) {
    MSG_ERROR("_GetLinkSysCallBackEx: pHandler Null", 0, 0, 0);
    return EFAILED;
  }

  pAsynInfo = MALLOC(sizeof(OEMCard_AsynCBInfo));
  if (NULL == pAsynInfo) {
    MSG_ERROR("_GetLinkSysCallBackEx: pAsynInfo Alloc failed", 0, 0, 0);
    return ENOMEMORY;
  }
  
  CARD_ENTER_CRITICAL_SECTION
  pAsynInfo->pCB = (AEECallback*)AEE_LinkSysCallback(pUserCB);
  if (NULL != pAsynInfo->pCB) {
    pAsynInfo->pClientData = pCmdStatus;
    pAsynInfo->pCB->pfnCancel = pCancelCb;
    pAsynInfo->pObj = pObj;

    nRet = AEEObjectMgr_Register(pAsynInfo, &pAsynInfo->Handler);
    if (SUCCESS != nRet) {
      CALLBACK_Cancel(pAsynInfo->pCB);
      FREEIF(pAsynInfo);
      pAsynInfo = NULL;
      CARD_LEAVE_CRITICAL_SECTION
      MSG_ERROR("_GetLinkSysCallBackEx: AEEObjectMgr_Regstier failed 0x%x", 
                nRet, 0, 0);
      return nRet;
    }
    pAsynInfo->pCB->pCancelData = (void *)pAsynInfo->Handler;
  }
  else {
    FREEIF(pAsynInfo);
    pAsynInfo = NULL;
    CARD_LEAVE_CRITICAL_SECTION
    MSG_ERROR("_GetLinkSysCallBackEx: pAsynInfo->pCB Null; failed to Linksys", 
              0, 0, 0);
    return EFAILED;
  }
  *pHandler = pAsynInfo->Handler;
   CARD_LEAVE_CRITICAL_SECTION
   return SUCCESS;
} /* OEMCard_GetLinkSysCallBackEx */
#endif /*#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT*/

/*===========================================================================

FUNCTION: OEMCard_GetLinkSysCallBack

DESCRIPTION
  Performs the AEE_LinkSysCallback operation.  Set the user passed in data
  in the reserve field

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_GetLinkSysCallBack(AEECallback *pUserCB, void *pCmdStatus, AEEObjectHandle *pHandler)
{
  OEMCard_AsynCBInfo  *pAsynInfo = NULL;
  int                  nRet;

  if (NULL == pHandler) {
    MSG_ERROR("_GetLinkSysCallBack: pHandler Null", 0, 0, 0);
    return EFAILED;
  }

  pAsynInfo = MALLOC(sizeof(OEMCard_AsynCBInfo));
  if (NULL == pAsynInfo) {
    MSG_ERROR("_GetLinkSysCallBack: pAsynInfo Alloc failed", 0, 0, 0);
    return ENOMEMORY;
  }

  CARD_ENTER_CRITICAL_SECTION
  pAsynInfo->pCB = (AEECallback*)AEE_LinkSysCallback(pUserCB);
  if (NULL != pAsynInfo->pCB) {
    pAsynInfo->pClientData = pCmdStatus;
    pAsynInfo->pCB->pfnCancel = (PFNCBCANCEL)OEMCard_Cancel;
    pAsynInfo->pObj = NULL;

    nRet = AEEObjectMgr_Register(pAsynInfo, &pAsynInfo->Handler);
    if (SUCCESS != nRet) {
      CALLBACK_Cancel(pAsynInfo->pCB);
      FREEIF(pAsynInfo);
      pAsynInfo = NULL;
      CARD_LEAVE_CRITICAL_SECTION
      MSG_ERROR("_GetLinkSysCallBack: AEEObjectMgr_Regstier failed 0x%x", 
                nRet, 0, 0);
      return EFAILED;

    }
    pAsynInfo->pCB->pCancelData = (void *)pAsynInfo->Handler;
  }
  else {
    FREEIF(pAsynInfo);
    pAsynInfo = NULL;
    CARD_LEAVE_CRITICAL_SECTION
    MSG_ERROR("_GetLinkSysCallBack: pAsynInfo->pCB Null; failed to Linksys", 
              0, 0, 0);
    return EFAILED;
  }
  *pHandler = pAsynInfo->Handler;
  CARD_LEAVE_CRITICAL_SECTION
  return SUCCESS;
} /* OEMCard_GetLinkSysCallBack */
#endif /*#ifdef FEATURE_MMGSDI*/

/*===========================================================================

FUNCTION: OEMCard_VerifyPin

DESCRIPTION
  Sends the VERIFY PIN request to GSDI with the given PIN.  The user callback
  structure is passed to GSDI and will be returned to the call back function.
  This function then sends the callback to the user.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_VerifyPin(ICard *po, uint8 pin_id, AEECardPinCode *pPin,
                              AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle   Handler = 0;
  char              pin_code[GSDI_MAX_PIN_LEN+1];
  char             *phone_code = NULL;
  gsdi_pin_ids_T    gsdi_pin_id = GSDI_MAX_PIN_ID;
  byte              num_retries;
  int               nReturnStatus = SUCCESS;
  gsdi_slot_id_type gsdi_slot_id = GSDI_SLOT_NONE;
#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* FEATURE_BT && FEATURE_BT_EXTPF_SAP */

  if ((!po) || (!pPin) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  if ((pPin->code_len > 0) && (pPin->code == NULL)) {
    return EBADPARM;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot_id);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
  if (pin_id == AEECARD_PHONE_CODE)
  {
    if (pPin->code_len == 0) {
      return EBADPARM;
    }
    phone_code = MALLOC(pPin->code_len+1);
    if (phone_code == NULL)
    {
      return ENOMEMORY;
    }
    (void)WSTRTOSTR(pPin->code, phone_code, pPin->code_len+1);
  }
  else
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC*/
#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* #if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP) */
  {
    /* Find the PIN */
    nReturnStatus = OEMCard_MapPinEnum(pin_id, &gsdi_pin_id, &gsdi_slot_id);
    if (nReturnStatus != SUCCESS) {
      return nReturnStatus;
    }

    (void)MEMSET(pin_code, 0, sizeof(pin_code));
    if (pPin->code != NULL) {
      (void)WSTRTOSTR(pPin->code, pin_code, sizeof(pin_code));
    }
  }
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    FREEIF(phone_code);
#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* #if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)*/
    phone_code = NULL;
    return nReturnStatus;
  }

  switch(pin_id)
  {
  case AEECARD_PIN1_SLOT2:
  case AEECARD_PIN2_SLOT2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if(gsdi2_verify_pin(GSDI_SLOT_2, gsdi_pin_id, pin_code, NULL, (uint32)Handler,
                        OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
    return SUCCESS;
#else
    OEMCard_CleanUpAsynObject(Handler);
    return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  case AEECARD_PIN1:
  case AEECARD_PIN2:
    if (gsdi_verify_pin(gsdi_pin_id, pin_code, &num_retries, (uint32)Handler,
                        OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
    return SUCCESS;
  case AEECARD_PHONE_CODE:
#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
    if (gsdi_perso_verify_phonecode(pPin->code_len, (uint8*)phone_code, (uint32)Handler,
                       OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      FREEIF(phone_code);
      phone_code = NULL;
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
    FREEIF(phone_code);
    phone_code = NULL;
    return SUCCESS;
#else
    FREEIF(phone_code);
    phone_code = NULL;
    OEMCard_CleanUpAsynObject(Handler);
    return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
  case AEECARD_SAP_PIN:
#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
    return EUNSUPPORTED;
#endif /* #if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP) */
  }
  return EFAILED;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
}  /* OEMCard_VerifyPin */

/*===========================================================================

FUNCTION: OEMCard_ChangePin

DESCRIPTION
  Sends the Change PIN request to GSDI with the given PIN.  The user callback
  structure is passed to GSDI and will be returned to the call back function.
  This function then sends the callback to the user.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_ChangePin(ICard *po, uint8 pin_id, AEECardPinCode *pOld_pin,
                              AEECardPinCode *pNew_pin, AEECardPinCmdStatus *pCmdStatus,
                              AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle   Handler = 0;
  char             *old_phone_code = NULL;
  char             *new_phone_code = NULL;
  char              old_pin_code[GSDI_MAX_PIN_LEN+1];
  char              new_pin_code[GSDI_MAX_PIN_LEN+1];
  gsdi_pin_ids_T    gsdi_pin_id;
  byte              num_retries;
  int               nReturnStatus = SUCCESS;
  gsdi_slot_id_type gsdi_slot_id = GSDI_SLOT_NONE;

  if ((!po) || (!pOld_pin) || (!pNew_pin) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  if ((pOld_pin->code_len > 0) && (pOld_pin->code == NULL)) {
    return EBADPARM;
  }
  if ((pNew_pin->code_len > 0) && (pNew_pin->code == NULL)) {
    return EBADPARM;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot_id);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
  if (pin_id == AEECARD_PHONE_CODE)
  {
    if ((pNew_pin->code_len == 0) || (pOld_pin->code_len == 0)) {
      return EBADPARM;
    }
    new_phone_code = MALLOC(pNew_pin->code_len+1);
    if (new_phone_code == NULL)
    {
      return ENOMEMORY;
    }
    old_phone_code = MALLOC(pOld_pin->code_len+1);
    if (old_phone_code == NULL)
    {
      FREEIF(new_phone_code);
      new_phone_code = NULL;
      return ENOMEMORY;
    }
    (void)WSTRTOSTR(pNew_pin->code, new_phone_code, pNew_pin->code_len+1);
    (void)WSTRTOSTR(pOld_pin->code, old_phone_code, pOld_pin->code_len+1);
  }
  else
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC*/
  {
    /* Find the PIN */
    nReturnStatus = OEMCard_MapPinEnum(pin_id, &gsdi_pin_id, &gsdi_slot_id);
    if (nReturnStatus != SUCCESS) {
      return nReturnStatus;
    }

    (void)MEMSET(old_pin_code, 0, sizeof(old_pin_code));
    (void)MEMSET(new_pin_code, 0, sizeof(new_pin_code));
    if (pOld_pin->code != NULL) {
      (void)WSTRTOSTR(pOld_pin->code, old_pin_code, sizeof(old_pin_code));
    }
    if (pNew_pin->code != NULL) {
      (void)WSTRTOSTR(pNew_pin->code, new_pin_code, sizeof(new_pin_code));
    }
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    FREEIF(old_phone_code);
    FREEIF(new_phone_code);
    old_phone_code = NULL;
    new_phone_code = NULL;
    return nReturnStatus;
  }

  switch(pin_id)
  {
  case AEECARD_PIN1_SLOT2:
  case AEECARD_PIN2_SLOT2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if (gsdi2_change_pin(GSDI_SLOT_2, gsdi_pin_id, old_pin_code, new_pin_code, NULL,
      (uint32)Handler, OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
    return SUCCESS;
#else
    OEMCard_CleanUpAsynObject(Handler);
    return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  case AEECARD_PIN1:
  case AEECARD_PIN2:
    if (gsdi_change_pin(gsdi_pin_id, old_pin_code, new_pin_code, &num_retries,
      (uint32)Handler, OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
    return SUCCESS;
  case AEECARD_PHONE_CODE:
#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
    if (gsdi_perso_change_phonecode(pOld_pin->code_len, (uint8*)old_phone_code, pNew_pin->code_len,
                       (uint8*)new_phone_code, (uint32)Handler,
                       OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      FREEIF(old_phone_code);
      FREEIF(new_phone_code);
      old_phone_code = NULL;
      new_phone_code = NULL;
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
    FREEIF(old_phone_code);
    FREEIF(new_phone_code);
    old_phone_code = NULL;
    new_phone_code = NULL;
    return SUCCESS;
#else
    FREEIF(old_phone_code);
    FREEIF(new_phone_code);
    old_phone_code = NULL;
    new_phone_code = NULL;
    OEMCard_CleanUpAsynObject(Handler);
    return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
  }
  return EFAILED;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_ChangePin */

/*===========================================================================

FUNCTION: OEMCard_EnablePin

DESCRIPTION
  Sends the Enable PIN request to GSDI with the given PIN.  The user callback
  structure is passed to GSDI and will be returned to the call back function.
  This function then sends the callback to the user.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_EnablePin(ICard *po, uint8 pin_id, AEECardPinCode *pPin,
                              AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle   Handler = 0;
  byte              num_retries;
  char              pin_code[GSDI_MAX_PIN_LEN+1];
  gsdi_pin_ids_T    gsdi_pin_id;
  int               nReturnStatus = SUCCESS;
  gsdi_slot_id_type gsdi_slot_id = GSDI_SLOT_NONE;

  if (po == NULL || pPin == NULL || pCmdStatus == NULL || pUserCB == NULL) {
    return EBADPARM;
  }
  if ((pPin->code_len > 0) && (pPin->code == NULL)) {
    return EBADPARM;
  }

  if (pin_id == AEECARD_PHONE_CODE)
    return EBADPARM;

  /* Find the PIN */
  nReturnStatus = OEMCard_MapPinEnum(pin_id, &gsdi_pin_id, &gsdi_slot_id);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot_id);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  (void)MEMSET(pin_code, 0, sizeof(pin_code));
  if (pPin->code != NULL) {
    (void)WSTRTOSTR(pPin->code, pin_code, sizeof(pin_code));
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if ((pin_id == AEECARD_PIN1_SLOT2) || (pin_id == AEECARD_PIN2_SLOT2)) {
    if (gsdi2_enable_pin(GSDI_SLOT_2, gsdi_pin_id, pin_code, NULL,
      (uint32)Handler, OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
  }
  else
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  {
    if (gsdi_enable_pin(gsdi_pin_id, pin_code, &num_retries,
      (uint32)Handler, OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif
} /* OEMCard_EnablePin */

/*===========================================================================

FUNCTION: OEMCard_DisablePin

DESCRIPTION
  Sends the Disable PIN request to GSDI with the given PIN.  The user callback
  structure is passed to GSDI and will be returned to the call back function.
  That function then sends the callback to the user.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_DisablePin(ICard *po, uint8 pin_id, AEECardPinCode *pPin,
                               AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle   Handler = 0;
  byte              num_retries;
  char              pin_code[GSDI_MAX_PIN_LEN+1];
  gsdi_pin_ids_T    gsdi_pin_id;
  int               nReturnStatus = SUCCESS;
  gsdi_slot_id_type gsdi_slot_id = GSDI_SLOT_NONE;

  if (po == NULL || pPin == NULL || pCmdStatus == NULL || pUserCB == NULL) {
    return EBADPARM;
  }
  if ((pPin->code_len > 0) && (pPin->code == NULL)) {
    return EBADPARM;
  }

  if (pin_id == AEECARD_PHONE_CODE)
    return EBADPARM;

  /* Find the PIN */
  nReturnStatus = OEMCard_MapPinEnum(pin_id, &gsdi_pin_id, &gsdi_slot_id);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot_id);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  (void)MEMSET(pin_code, 0, sizeof(pin_code));
  if (pPin->code != NULL) {
    (void)WSTRTOSTR(pPin->code, pin_code, sizeof(pin_code));
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if ((pin_id == AEECARD_PIN1_SLOT2) || (pin_id == AEECARD_PIN2_SLOT2)) {
    if (gsdi2_disable_pin(GSDI_SLOT_2, gsdi_pin_id, pin_code, NULL,
      (uint32)Handler, OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
  }
  else
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  {
    if (gsdi_disable_pin(gsdi_pin_id, pin_code, &num_retries,
      (uint32)Handler, OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_DisablePin */

/*===========================================================================

FUNCTION: OEMCard_UnblockPin

DESCRIPTION
  Sends the Unblock PIN request to GSDI with the given PIN.  The user callback
  structure is passed to GSDI and will be returned to the call back function.
  This function then sends the callback to the user.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_UnblockPin(ICard *po, uint8 pin_id, AEECardPinCode *pPuk,
                               AEECardPinCode *pPin, AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle   Handler = 0;
  char             *master_key = NULL;
  char             *new_phone_code = NULL;
  char              puk_code[GSDI_MAX_PUK_LEN+1];
  char              pin_code[GSDI_MAX_PIN_LEN+1];
  byte              num_retries;
  gsdi_pin_ids_T    gsdi_pin_id;
  int               nReturnStatus = SUCCESS;
  gsdi_slot_id_type gsdi_slot_id = GSDI_SLOT_NONE;

  if ((!po) || (!pPin) || (!pUserCB) || (!pCmdStatus) || (!pPuk)) {
    return EBADPARM;
  }

  if ((pPuk->code_len > 0) && (pPuk->code == NULL)) {
    return EBADPARM;
  }
  if ((pPin->code_len > 0) && (pPin->code == NULL)) {
    return EBADPARM;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot_id);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
  if (pin_id == AEECARD_PHONE_CODE)
  {
    if ((pPin->code_len == 0) || (pPuk->code_len == 0)) {
      return EBADPARM;
    }
    new_phone_code = MALLOC(pPin->code_len+1);
    if (new_phone_code == NULL)
    {
      return ENOMEMORY;
    }
    master_key = MALLOC(pPuk->code_len+1);
    if (master_key == NULL)
    {
      FREEIF(new_phone_code);
      new_phone_code = NULL;
      return ENOMEMORY;
    }
    (void)WSTRTOSTR(pPin->code, new_phone_code, pPin->code_len+1);
    (void)WSTRTOSTR(pPuk->code, master_key, pPuk->code_len+1);
  }
  else
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC*/
  {
    /* Find the PIN */
    nReturnStatus = OEMCard_MapPinEnum(pin_id, &gsdi_pin_id, &gsdi_slot_id);
    if (nReturnStatus != SUCCESS) {
      return nReturnStatus;
    }

    (void)MEMSET(puk_code, 0, sizeof(puk_code));
    if (pPuk->code != NULL) {
      (void)WSTRTOSTR(pPuk->code, puk_code, sizeof(puk_code));
    }
    (void)MEMSET(pin_code, 0, sizeof(pin_code));
    if (pPin->code != NULL) {
      (void)WSTRTOSTR(pPin->code, pin_code, sizeof(pin_code));
    }
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    FREEIF(master_key);
    FREEIF(new_phone_code);
    master_key = NULL;
    new_phone_code = NULL;
    return nReturnStatus;
  }

  switch(pin_id)
  {
  case AEECARD_PIN1_SLOT2:
  case AEECARD_PIN2_SLOT2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if (gsdi2_unblock_pin(GSDI_SLOT_2, gsdi_pin_id, puk_code, pin_code, NULL,
      (uint32)Handler, OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
    return SUCCESS;
#else
    OEMCard_CleanUpAsynObject(Handler);
    return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  case AEECARD_PIN1:
  case AEECARD_PIN2:
    if (gsdi_unblock_pin(gsdi_pin_id, puk_code, pin_code, &num_retries,
       (uint32)Handler, OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
    return SUCCESS;
  case AEECARD_PHONE_CODE:
#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
    if (gsdi_perso_unblock_phonecode(pPuk->code_len, (uint8*)master_key, pPin->code_len,
                       (uint8*)new_phone_code, (uint32)Handler,
                       OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
    {
      FREEIF(master_key);
      FREEIF(new_phone_code);
      master_key = NULL;
      new_phone_code = NULL;
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
    FREEIF(master_key);
    FREEIF(new_phone_code);
    master_key = NULL;
    new_phone_code = NULL;
    return SUCCESS;
#else
    FREEIF(master_key);
    FREEIF(new_phone_code);
    master_key = NULL;
    new_phone_code = NULL;
    OEMCard_CleanUpAsynObject(Handler);
    return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
  }
  return EFAILED;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_UnblockPin */

/*===========================================================================

FUNCTION: OEMCard_GetCardID

DESCRIPTION
  Not yet supported, will get the SIM ID from the SIM.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_GetCardID(ICard * po, char *pId, int *pnLen)
{
  return EUNSUPPORTED;
#if 0
#define CARD_ID_SIZE ((word)(sizeof(((nv_item_type *) 0)-> esn.esn )))

   byte *CARD_buffer;

   if ((!po) || (!pnLen)) {
      return EFAILED;
   }

   // Check to see if the card is connected.
   if (!ICard_IsCardConnected (po)) {
      return EFAILED;
   }

   CARD_buffer = MALLOC(CARD_ID_SIZE);
   if (!CARD_buffer) {
      return ENOMEMORY;
   }

   // Prepare to read the id EF.
   gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
   gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
   gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
   gUimCmd.access_uim.hdr.rpt_function       = OEMCARD_report;

   gUimCmd.access_uim.item      = UIM_CDMA_CARD_ID;
   gUimCmd.access_uim.access    = UIM_READ_F;
   gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
   gUimCmd.access_uim.num_bytes = CARD_ID_SIZE;
   gUimCmd.access_uim.offset    = 1;
   gUimCmd.access_uim.data_ptr  = CARD_buffer;

   // From nvCARD_access():  Access an EF, do not signal any task, use no
   // signal, no done queue, use a callback, always report status.

   // Send the command to the R-UIM:
   OEMCARD_send_uim_cmd_and_wait (&gUimCmd);

   if ((gCallBack.rpt_type != UIM_ACCESS_R)
                                        || (gCallBack.rpt_status != UIM_PASS)) {
      FREE(CARD_buffer);
      return EFAILED;
   }

   if (pId == NULL) {
      *pnLen = gUimCmd.access_uim.num_bytes_rsp;
   } else {
      *pnLen = MIN(*pnLen, gUimCmd.access_uim.num_bytes_rsp);
      (void)MEMCPY(pId, CARD_buffer, *pnLen);
   }

   FREE(CARD_buffer);
   return AEE_SUCCESS;
#endif // 0
} /* OEMCard_GetCardID */ /*lint !e715 */

/*===========================================================================

FUNCTION: OEMCard_GetCardApps

DESCRIPTION
  Query GSDI for the inserted card's applications/DF presented.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS if all went well, EBADPARM if there was a parameter error.
  EUNSUPPORTED is there is no MMGSDI

SIDE EFFECTS
===========================================================================*/
static int OEMCard_GetCardApps(ICard * po, AEECardApp *pCardApp)
{

#ifdef FEATURE_MMGSDI
  gsdi_card_apps_T  sCardApp;
#endif /*FEATURE_MMGSDI */

  if (!po || pCardApp == NULL) {
    return EBADPARM;
  }
#ifdef FEATURE_MMGSDI
  gsdi_get_apps_available( &sCardApp );
  pCardApp->apps1_enabled = sCardApp.apps_enabled;
  pCardApp->slot1_present = sCardApp.slot1_present;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  pCardApp->apps2_enabled = sCardApp.apps2_enabled;
  pCardApp->slot2_present = sCardApp.slot2_present;
#endif
  pCardApp->operational_slot = sCardApp.operational_slot;
  pCardApp->onchip_card_activated = sCardApp.onchip_sim_active;
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI */

} /* OEMCard_GetCardApps */

/*===========================================================================

FUNCTION: OEMCard_EnableFDNSlot

DESCRIPTION
  Send the pin 2 information to GSDI in order to enable the FDN entries

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_EnableFDNSlot(ICard *po, uint8 wSlot, boolean bPresent,
                                 AEECardPinCode *pPin,
                                 AEECardGenericCmdStatus *pCmdStatus,
                                 AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle   Handler = 0;
  gsdi_slot_id_type gsdi_slot;
  char              pin_code[GSDI_MAX_PIN_LEN+1];
  int               nReturnStatus = SUCCESS;

  if ((!po) || ((!pPin) && bPresent) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  /* convert the slot */
  nReturnStatus = OEMCard_MapSlot(wSlot, &gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  (void)MEMSET(pin_code, 0, sizeof(pin_code));
  if (bPresent && (pPin != NULL)) {
    (void)WSTRTOSTR(pPin->code, pin_code, sizeof(pin_code));
  }

  if (gsdi2_enable_fdn(gsdi_slot, bPresent, pin_code, NULL, (uint32)Handler,
      OEMCardGSDIGenericcmdCB) != GSDI_SUCCESS)
  {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_EnableFDNSlot */

/*===========================================================================

FUNCTION: OEMCard_DisableFDNSlot

DESCRIPTION
  Send the pin 2 information to GSDI in order to disable the FDN entries

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_DisableFDNSlot(ICard *po, uint8 wSlot, boolean bPresent,
                                  AEECardPinCode *pPin,
                                  AEECardGenericCmdStatus *pCmdStatus,
                                  AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle   Handler;
  gsdi_slot_id_type gsdi_slot;
  char              pin_code[GSDI_MAX_PIN_LEN+1];
  int               nReturnStatus = SUCCESS;

  if ((!po) || ((!pPin) && bPresent) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  /* convert the slot */
  nReturnStatus = OEMCard_MapSlot(wSlot, &gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  (void)MEMSET(pin_code, 0, sizeof(pin_code));
  if (bPresent && (pPin != NULL)) {
    (void)WSTRTOSTR(pPin->code, pin_code, sizeof(pin_code));
  }
  if (gsdi2_disable_fdn(gsdi_slot, bPresent, pin_code, NULL, (uint32)Handler,
      OEMCardGSDIGenericcmdCB) != GSDI_SUCCESS)
  {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_DisableFDNSlot */

/*===========================================================================

FUNCTION: OEMCard_GetSimCapabilitySlot

DESCRIPTION
  returns the capabilities of the sim

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_GetSimCapabilitySlot(ICard *po, uint8 wSlot, AEESimCapabilities *pSimCap)
{
#ifdef FEATURE_MMGSDI

  if ((!po) || (!pSimCap)) {
    return EBADPARM;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (wSlot == AEECARD_SLOT2) {
    // Check to see if the card is connected.
    if(SUCCESS != OEMCard_CheckValidCardStatus(GSDI_SLOT_2))
    {
      return EFAILED;
    }
    else {
      (void)MEMCPY(pSimCap, &sim_cap_slot2, sizeof(AEESimCapabilities));
      return SUCCESS;
    }

  }
  else
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  {
    // Check to see if the card is connected.
    if(SUCCESS != OEMCard_CheckValidCardStatus(GSDI_SLOT_1))
    {
      return EFAILED;
    }
    else {
      (void)MEMCPY(pSimCap, &sim_cap, sizeof(AEESimCapabilities));
      return SUCCESS;
    }
  }

#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_GetSimCapabilitySlot *//*lint !e715 */

/*===========================================================================

FUNCTION: OEMCard_ChangeUserSlotPref

DESCRIPTION
  Change the preference slot of the indicated technologies

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_ChangeUserSlotPref(ICard *po, uint8 wNewSlot, uint8 wApp,
                                      AEECardGenericCmdStatus *pCmdStatus,
                                      AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  AEEObjectHandle    Handler = 0;
  gsdi_returns_T     gsdi_return_status;
  gsdi_app_enum_type gsdi_app;
  gsdi_slot_id_type  gsdi_slot;
  int                nReturnStatus;

  if ((!po) || (!pUserCB) || (!pCmdStatus)){
    return EBADPARM;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  if (OEMCard_MapSlot(wNewSlot, &gsdi_slot) != SUCCESS) {
    return EFAILED;
  }

  switch (wApp)
  {
  case AEECARD_GSM_APP:
    gsdi_app = GSDI_GSM_APP;
    break;
  case AEECARD_CDMA_APP:
    gsdi_app = GSDI_CDMA_APP;
    break;
  default:
    return EBADPARM;
  }

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  gsdi_return_status = gsdi2_switch_slot_preference(gsdi_slot,
                                                    gsdi_app,
                                                    (uint32)Handler,
                                                    OEMCardGSDIGenericcmdCB);

  if (gsdi_return_status == GSDI_SUCCESS) {
    return SUCCESS;
  }
  else {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI_DUAL_SLOT*/
} /* OEMCard_ChangeUserSlotPref *//*lint !e715 */

/*===========================================================================

FUNCTION: OEMCard_GetUserSlotPref

DESCRIPTION
  Get the preference slot of the indicated technology

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_GetUserSlotPref(ICard *po,  uint8 wApp, uint8 *pwSlot)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  gsdi_app_enum_type gsdi_app;
  gsdi_slot_id_type  gsdi_slot;
  gsdi_returns_T     gsdi_return_status;
#endif

  if ((!po) || (!pwSlot)){
    return EBADPARM;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT

  switch (wApp)
  {
  case AEECARD_GSM_APP:
    gsdi_app = GSDI_GSM_APP;
    break;
  case AEECARD_CDMA_APP:
    gsdi_app = GSDI_CDMA_APP;
    break;
  case AEECARD_USIM_APP:
    gsdi_app = GSDI_USIM_APP;
    break;
  default:
    return EBADPARM;
  }

  gsdi_return_status = gsdi_get_user_preferred_slot(gsdi_app,
                                                    &gsdi_slot);

  if (gsdi_return_status != GSDI_SUCCESS) {
    return EFAILED;
  }
  switch (gsdi_slot) {
    case GSDI_SLOT_1:
      *pwSlot = AEECARD_SLOT1;
      break;
    case GSDI_SLOT_2:
      *pwSlot = AEECARD_SLOT2;
      break;
    case GSDI_SLOT_NONE:
      *pwSlot = AEECARD_SLOT_NONE;
      break;
    default:
      return EFAILED;
  }

  return SUCCESS;
#else
  *pwSlot = AEECARD_SLOT1;
   return SUCCESS;
#endif /*#ifdef FEATURE_MMGSDI_DUAL_SLOT*/
} /* OEMCard_GetUserSlotPref *//*lint !e715 */

/*===========================================================================

FUNCTION: OEMCard_GetATRSize

DESCRIPTION
  returns the size of the SIM's ATR

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid, nSize will indicate whether there
            is any ATR present.  nSize == 0 indicates that there is no
            card present
  EBADPARM -  Bad parameter
  EFAILED - Request made before ICard could cache the ATR
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_GetATRSize(ICard *po, uint8 wSlot, int *nSize)
{
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT

  if ((!po) || (!nSize)) {
    return EBADPARM;
  }

  if (wSlot == AEECARD_SLOT2) {
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if (!ATR_Slot2.bReady) {
        return EFAILED;
    }
    /* ATR is ready */
    *nSize = ATR_Slot2.sATR.nSize;
    return SUCCESS;
#else
    /* Dual Slot is not defined, so return zero */
    *nSize = 0;
    return SUCCESS;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  }
  else

  {
    if (!ATR_Slot1.bReady) {
      return EFAILED;
    }
    /* ATR is ready */
    *nSize = ATR_Slot1.sATR.nSize;
    return SUCCESS;
  }

#else /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
} /* OEMCard_GetATRSize */

/*===========================================================================

FUNCTION: OEMCard_GetATR

DESCRIPTION
  returns the ATR of the sim

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_GetATR(ICard *po, uint8 wSlot, AEECardATR *pATR)
{
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT

  if ((!po) || (!pATR)) {
    return EBADPARM;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (wSlot == AEECARD_SLOT2) {
    if (!ATR_Slot2.bReady) {
      return EFAILED;
    }

    /* ATR is ready */
    if (pATR->pATR == NULL) {
      return EBADPARM;
    }
    if (pATR->nSize > ATR_Slot2.sATR.nSize) {
      (void)MEMCPY(pATR->pATR, ATR_Slot2.sATR.pATR, ATR_Slot2.sATR.nSize);
      pATR->nSize = ATR_Slot2.sATR.nSize;
    }
    else {
      (void)MEMCPY(pATR->pATR, ATR_Slot2.sATR.pATR, pATR->nSize);
    }

    return SUCCESS;
  }
  else
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  {
    if (!ATR_Slot1.bReady) {
      return EFAILED;
    }
    /* ATR is ready */

    if (pATR->pATR == NULL) {
      return EBADPARM;
    }

    if (pATR->nSize > ATR_Slot1.sATR.nSize) {
      (void)MEMCPY(pATR->pATR, ATR_Slot1.sATR.pATR, ATR_Slot1.sATR.nSize);
      pATR->nSize = ATR_Slot1.sATR.nSize;
    }
    else {
      (void)MEMCPY(pATR->pATR, ATR_Slot1.sATR.pATR, pATR->nSize);
    }
    return SUCCESS;
  }
  
#else /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
} /* OEMCard_GetATR *//*lint !e715 */

/*===========================================================================

FUNCTION: OEMCard_GetATRAsync

DESCRIPTION
  returns the ATR of the sim asynchronously

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_GetATRAsync(ICard *po, uint8 wSlot, AEECardATR *pATR, AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT

  AEEObjectHandle Handler;
  int nReturnStatus;

  if ((!po) || (!pATR) || (!pUserCB)) {
    MSG_ERROR("Null pointer - po/pATR/pUserCB",0,0,0);
    return EBADPARM;
  }

  /* ATR is ready */
  if (pATR->pATR == NULL) {
    MSG_ERROR("pATR is NULL",0,0,0);
    return EBADPARM;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (wSlot == AEECARD_SLOT2) {
    if (!ATR_Slot2.bReady) {
       /* Get and populate the callback */
      nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pATR, &Handler);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }
      (void)gsdi_sim_get_atr(GSDI_SLOT_2, (uint32)Handler, OEMCardGSDIGetATRCB);
      return SUCCESS;
    }
   
   if (pATR->nSize > ATR_Slot2.sATR.nSize) {
      (void)MEMCPY(pATR->pATR, ATR_Slot2.sATR.pATR, ATR_Slot2.sATR.nSize);
      pATR->nSize = ATR_Slot2.sATR.nSize;
    }
    else {
      (void)MEMCPY(pATR->pATR, ATR_Slot2.sATR.pATR, pATR->nSize);
    }
  }
  else
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  {
    if (!ATR_Slot1.bReady) {
      nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pATR, &Handler);
      if (nReturnStatus != SUCCESS) {
        return nReturnStatus;
      }
      
      (void)gsdi_sim_get_atr(GSDI_SLOT_1, (uint32)Handler ,OEMCardGSDIGetATRCB);
      return SUCCESS;
    }
    /* ATR is ready */

    if (pATR->nSize > ATR_Slot1.sATR.nSize) {
      (void)MEMCPY(pATR->pATR, ATR_Slot1.sATR.pATR, ATR_Slot1.sATR.nSize);
      pATR->nSize = ATR_Slot1.sATR.nSize;
    }
    else {
      (void)MEMCPY(pATR->pATR, ATR_Slot1.sATR.pATR, pATR->nSize);
    }
  }
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(pUserCB,  0);
#else
  AEE_ResumeEx(pUserCB, 0, 0);
#endif
  return SUCCESS;
  

#else /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
} /* OEMCard_GetATRAsync *//*lint !e715 */

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION: OEMCard_MapFile

DESCRIPTION
  This function is called to map the gsdi_file_enum_type from AEECardFilePathType

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCard_MapFile(AEECardFilePathType sFilePath,
                    gsdi_file_enum_type *gsdi_file_item )
{
  int nReturnStatus = SUCCESS;
  if (NULL == gsdi_file_item) {
    MSG_ERROR("_MapFile: gsdi_file_item null", 0, 0, 0);
    return EFAILED;
  }

  if (sFilePath.nFileIndication == AEECARD_FILE_NAME) {
    nReturnStatus = OEMCard_MapFileEnum(sFilePath.nFileName, gsdi_file_item);
    if (nReturnStatus != SUCCESS) {
      return nReturnStatus;
    }
  }
  else if (sFilePath.nFileIndication == AEECARD_FILE_PATH) {
    nReturnStatus = OEMCard_CheckFilePath(sFilePath);
    if (nReturnStatus != SUCCESS) {
      return nReturnStatus;
    }
    return EUNSUPPORTED;
  }
  else {
    MSG_ERROR("_MapFile: Unknown file IndType 0x%x", 
              sFilePath.nFileIndication, 0, 0);
    return EBADPARM;
  }
  return SUCCESS;
} /* OEMCard_MapFile */
#endif /*FEATURE_MMGSDI */

/*===========================================================================

FUNCTION: OEMCard_GetFileAttributes

Description:
  This method provides the service to get the Attribute information on a
  file or directory on the SIM/USIM

Parameters:
  po        : [Input] Pointer to the ICard object
  bySlot    : [Input] Indicates which slot the GetFileAttributes access is to be
                      retrieved from
  sFilePath : [Input] Indicates the file of interest
  pCmdStatus: [Input/Output] Command response status
  pUserCB   : [Input]Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data read is returned in Asynchronous fashion.  In the pUserCB, ICard
  will populate pCmdStatus with the file attribute information

Side Effects:
   None

See Also:
   None
===========================================================================*/
static int OEMCard_GetFileAttributes( ICard *po,
                                      uint8 bySlot,
                                      AEECardFilePathType sFilePath,
                                      AEECardFileAttribStatus *pCmdStatus,
                                      AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle     Handler;
  gsdi_slot_id_type   gsdi_slot;
  gsdi_file_enum_type gsdi_file_item;
  uim_items_enum_type uim_file_item;
  int                 nReturnStatus = SUCCESS;
  gsdi_file_attributes_T  dummy_attributes;

  if ((!po) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  /* convert the slot */
  nReturnStatus = OEMCard_MapSlot(bySlot, &gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCard_MapFile(sFilePath, &gsdi_file_item);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (gsdi_util_map_file_enum_to_uim_enum(gsdi_file_item, &uim_file_item) != GSDI_SUCCESS) {
    return EFAILED;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (gsdi_get_file_attributes(uim_file_item, &dummy_attributes, (uint32)Handler,
      OEMCardGSDIGenericcmdCB) != GSDI_SUCCESS)
  {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_GetFileAttributes */


/*===========================================================================

FUNCTION: OEMCard_ReadRecord

Description:
  This function provides the Read Record API to the client

Parameters:
  po        : [Input] Pointer to the ICard object
  bySlot    : [Input] Indicates which slot the access is to be
                      retrieved from
  sFilePath : [Input] Indicates the file of interest
  nRecordNum: [Input] The record to be read (nRecordNum has to be greater than 0)
  pCmdStatus: [Input/Output] Command response status
  pUserCB   : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data read is returned in Asynchronous fashion.  In the pUserCB, ICard
  will indicate if the process is Success/Fail.  In case of Success, client
  is required to use ICARD_GetRespData to retrieve the Record Data

Side Effects:
   None

See Also:
   None
===========================================================================*/
static int OEMCard_ReadRecord(ICard *po,
                              uint8 bySlot,
                              AEECardFilePathType sFilePath,
                              int nRecordNum,
                              AEECardGenericCmdStatus *pCmdStatus,
                              AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle     Handler;
  gsdi_slot_id_type   gsdi_slot;
  gsdi_file_enum_type gsdi_file_item;
  uim_items_enum_type uim_file_item;
  int                 nReturnStatus = SUCCESS;
  int                 dummy_rec_num_accessed;

  if ((!po) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  /* convert the slot */
  nReturnStatus = OEMCard_MapSlot(bySlot, &gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (nRecordNum <= 0) {
    MSG_ERROR("Invalid Rec Num 0x%x", nRecordNum, 0, 0);
    return EFAILED;
  }

  nReturnStatus = OEMCard_MapFile(sFilePath, &gsdi_file_item);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (gsdi_util_map_file_enum_to_uim_enum(gsdi_file_item, &uim_file_item) != GSDI_SUCCESS) {
    return EFAILED;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (gsdi_sim_read(uim_file_item, GSDI_INDEX_ABSOLUTE, nRecordNum,
                    NULL, 0, NULL, 0, 0, &dummy_rec_num_accessed, (uint32)Handler,
      OEMCardGSDIGenericcmdCB) != GSDI_SUCCESS)
  {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_ReadRecord */

/*===========================================================================

FUNCTION: OEMCard_ReadBinary

Description:
  This function provides the Read Binary File API to the client

Parameters:
  po        : [Input] Pointer to the ICard object
  bySlot    : [Input] Indicates which slot the access is to be
                      retrieved from
  sFilePath : [Input] Indicates the file of interest
  nStartOffset: [Input] The start offset of the read
  nNumbytes   : [Input] Number of bytes to be read
  pCmdStatus: [Input/Output] Command response status
  pUserCB   : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data read is returned in Asynchronous fashion.  In the pUserCB, ICard
  will indicate if the process is Success/Fail.  In case of Success, client
  is required to use ICARD_GetRespData to retrieve the Record Data

Side Effects:
   None

See Also:
   None
===========================================================================*/
static int OEMCard_ReadBinary(ICard *po,
                       uint8 bySlot,
                       AEECardFilePathType sFilePath,
                       int nStartOffset,
                       int nNumBytes,
                       AEECardGenericCmdStatus *pCmdStatus,
                       AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle     Handler;
  gsdi_slot_id_type   gsdi_slot;
  gsdi_file_enum_type gsdi_file_item;
  uim_items_enum_type uim_file_item;
  int                 nReturnStatus = SUCCESS;
  byte                dummy_data_p;
  int                 dummy_rec_num_accessed;

  if ((!po) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  /* convert the slot */
  nReturnStatus = OEMCard_MapSlot(bySlot, &gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCard_MapFile(sFilePath, &gsdi_file_item);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (gsdi_util_map_file_enum_to_uim_enum(gsdi_file_item, &uim_file_item) != GSDI_SUCCESS) {
    return EFAILED;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }
  if (nNumBytes > 0){
    if (gsdi_sim_read(uim_file_item, GSDI_NO_INDEX, 0,
                      NULL, 0, &dummy_data_p,
                      (uint8) nNumBytes, (uint8)nStartOffset,
                      &dummy_rec_num_accessed, (uint32)Handler,
                      OEMCardGSDIGenericcmdCB) != GSDI_SUCCESS){
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
  }
  else if (nNumBytes == 0){
    if (gsdi_sim_read(uim_file_item, GSDI_NO_INDEX, 0,
                      NULL, 0, NULL,
                      (uint8) nNumBytes, (uint8)nStartOffset,
                      &dummy_rec_num_accessed, (uint32)Handler,
                      OEMCardGSDIGenericcmdCB) != GSDI_SUCCESS){
      OEMCard_CleanUpAsynObject(Handler);
      return EFAILED;
    }
  }
  else{
    MSG_ERROR("Invalid no of bytes requested to read - 0x%x", nNumBytes, 0, 0);
    return EFAILED;
  }/* end if (nNumBytes > 0) */
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_ReadBinary */

/*===========================================================================

FUNCTION: OEMCard_WriteRecord

Description:
  This function provides the Write Record API to the client

Parameters:
  po        : [Input] Pointer to the ICard object
  bySlot    : [Input] Indicates which slot the access is to be
                      retrieved from
  sFilePath : [Input] Indicates the file of interest
  nRecordNum: [Input] The record to be read (nRecordNum has to be greater than 0)
  pWrittenData: [Input] Data to be written to the Card
  pCmdStatus: [Input/Output] Command response status
  pUserCB   : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data written is returned in Asynchronous fashion.  In the pUserCB, ICard
  will indicate if the process is Success/Fail.


Side Effects:
   None

See Also:
   None
===========================================================================*/
static int OEMCard_WriteRecord(ICard *po,
                               uint8 bySlot,
                               AEECardFilePathType sFilePath,
                               int nRecordNum,
                               AEECardDataBufType *pWrittenData,
                               AEECardGenericCmdStatus *pCmdStatus,
                               AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle     Handler;
  gsdi_slot_id_type   gsdi_slot;
  gsdi_file_enum_type gsdi_file_item;
  uim_items_enum_type uim_file_item;
  int                 nReturnStatus = SUCCESS;
  int                 dummy_rec_num_accessed;

  if ((!po) || (!pUserCB) || (!pCmdStatus) || (!pWrittenData)) {
    return EBADPARM;
  }

  if (pWrittenData->DataLen == NULL) {
    return EBADPARM;
  }

  if ((*(pWrittenData->DataLen) > 0) && (pWrittenData->pData == NULL)) {
    return EBADPARM;
  }
  /* convert the slot */
  nReturnStatus = OEMCard_MapSlot(bySlot, &gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (nRecordNum <= 0) {
    MSG_ERROR("Invalid Rec Num 0x%x", nRecordNum, 0, 0);
    return EBADPARM;
  }

  nReturnStatus = OEMCard_MapFile(sFilePath, &gsdi_file_item);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (gsdi_util_map_file_enum_to_uim_enum(gsdi_file_item, &uim_file_item) != GSDI_SUCCESS) {
    return EFAILED;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (gsdi_sim_write(uim_file_item, GSDI_INDEX_ABSOLUTE, nRecordNum,
                     NULL, 0, pWrittenData->pData, *(pWrittenData->DataLen), 0,
                     &dummy_rec_num_accessed,
                     (uint32)Handler,
      OEMCardGSDIGenericcmdCB) != GSDI_SUCCESS)
  {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_WriteRecord */

/*===========================================================================

FUNCTION: OEMCard_WriteBinary

Description:
  This function provides the Write Binary API to the client

Parameters:
  po        : [Input] Pointer to the ICard object
  bySlot    : [Input] Indicates which slot the access is to be
                      retrieved from
  sFilePath : [Input] Indicates the file of interest
  nStartOffset: [Input] The offset location that the data is to be written to
  pWrittenData: [Input] Data to be written to the Card
  pCmdStatus: [Input/Output] Command response status
  pUserCB   : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data written is returned in Asynchronous fashion.  In the pUserCB, ICard
  will indicate if the process is Success/Fail.


Side Effects:
   None

See Also:
   None
===========================================================================*/
static int OEMCard_WriteBinary(ICard *po,
                               uint8 bySlot,
                               AEECardFilePathType sFilePath,
                               int nStartOffset,
                               AEECardDataBufType *pWrittenData,
                               AEECardGenericCmdStatus *pCmdStatus,
                               AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI
  AEEObjectHandle     Handler;
  gsdi_slot_id_type   gsdi_slot;
  gsdi_file_enum_type gsdi_file_item;
  uim_items_enum_type uim_file_item;
  int                 nReturnStatus = SUCCESS;
  int                 dummy_rec_num_accessed;

  if ((!po) || (!pUserCB) || (!pCmdStatus) || (!pWrittenData)) {
    return EBADPARM;
  }

  if (pWrittenData->DataLen == NULL) {
    return EBADPARM;
  }

  if ((*(pWrittenData->DataLen) > 0) && (pWrittenData->pData == NULL)) {
    return EBADPARM;
  }
  /* convert the slot */
  nReturnStatus = OEMCard_MapSlot(bySlot, &gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCard_MapFile(sFilePath, &gsdi_file_item);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (gsdi_util_map_file_enum_to_uim_enum(gsdi_file_item, &uim_file_item) != GSDI_SUCCESS) {
    return EFAILED;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  if (gsdi_sim_write(uim_file_item, GSDI_NO_INDEX, 0,
                     NULL, 0, pWrittenData->pData, *(pWrittenData->DataLen), nStartOffset,
                     &dummy_rec_num_accessed,
                     (uint32)Handler,
      OEMCardGSDIGenericcmdCB) != GSDI_SUCCESS)
  {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_WriteBinary */

 /*===========================================================================

FUNCTION: OEMCard_GetCPHSInfo

Description:
  This function returns the CPHS information to user

Parameters:
  po        : [Input] Pointer to the ICard object
  bySlot    : [Input] Indicates which slot the access is to be
                      retrieved from
  pCmdStatus: [Input/Output] Command response status
  pUserCB   : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data read is returned in Asynchronous fashion.  In the pUserCB, ICard
  will indicate if the process is Success/Fail.


Side Effects:
   None

See Also:
   None
===========================================================================*/
static int OEMCard_GetCPHSInfo(ICard *po,
                          uint8 bySlot,
                          AEECardCPHSStatus *pCmdStatus,
                          AEECallback *pUserCB)
{
#if defined( FEATURE_MMGSDI ) && defined( FEATURE_CPHS )
#error code not present
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI*/
} /* OEMCard_GetCPHSInfo */

/*===========================================================================
FUNCTION: OEMCard_GetRespData

DESCRIPTION
  This function returns the response data to user

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

SIDE EFFECTS
   Data read is returned in Synchronous fashion.  The dwSeqNum is returned in the
  dwData member of the AEECardGenericCmdStatus structure

  if (pData->pDataLen == NULL)
  { function call will return error }
  if (pData->pData == NULL)
  { function call will return size of data available for copying }
  if (pData->pData != NULL)
  { ICard will copy *(pData->pDataLen) amount of data to the pData->pData buffer }
    if ICard has less than *(pData->pDataLen) amount of data, it will copy the
    minumum number of bytes into the pData->pData buffer
    *(pData->pDataLen) upon return of a function will reflect the exact number
    of bytes that are being copied to the application

See Also:
   None

===========================================================================*/
static int OEMCard_GetRespData(ICard *po,
                               uint32 dwSeqNum,
                               AEECardDataBufType *pData)
{
#ifdef FEATURE_MMGSDI
  if ((!po) || (!pData)) {
    return EBADPARM;
  }
  if (dwSeqNum >= OEMCARD_MAX_RSP) {
    return EBADPARM;
  }

  if (pData->DataLen == NULL) {
    return EBADPARM;
  }

  if (pData->pData == NULL) {
    *(pData->DataLen) = sCardRspTable[dwSeqNum].nSize;
    return SUCCESS;
  }
  if (*(pData->DataLen) > sCardRspTable[dwSeqNum].nSize) {
    *(pData->DataLen) = sCardRspTable[dwSeqNum].nSize;
  }

  (void)MEMCPY(pData->pData, sCardRspTable[dwSeqNum].sRsp, *(pData->DataLen));

  /* remove the memory allocated */
  OEMCard_FreeSequenceNumber(dwSeqNum);

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI */
} /* OEMCard_GetRespData */

/*===========================================================================
FUNCTION: OEMCard_EncodePUCT()

Description:
  This function takes PUCT information from AEEPuctInfo structure and encodes
  it into a binary character array of AEECARD_PUCT_FILE_LEN bytes. The
  output encoded array is the data that can be written to the PUCT file.

Prototype:

  int   OEMCard_EncodePUCT(ICard *po,
                         AEEPuctInfo *pInPUCTInfo,
                         uint8 *pOutEncodedBuff);

Parameters:
  po          : [Input] Pointer to the ICard object
  pInPUCTInfo : [Input] Structure containing price-per-unit and currency code.
  pOutEncodedBuff : [Output] Binary encoded form of PUCT file.

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Failed encoding

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================*/
static int OEMCard_EncodePUCT(ICard *po,
                       AEEPuctInfo *pInPUCTInfo,
                       uint8 *pOutEncodedBuff)
{
  int      PPU           = 0;
  int      Exp           = 0;
  boolean  bExpSign      = 1;
  uint32   nDecodeByte4  = 0;
  uint32   nDecodeByte5  = 0;
  float    PricePerUnit  = 0;

  if((po == NULL) || (pInPUCTInfo == NULL) ||
     (pOutEncodedBuff == NULL))
  {
    return EBADPARM;
  }

  (void)MEMSET((void *)pOutEncodedBuff,0x00,AEECARD_PUCT_FILE_LEN);

  // The First 3 bytes indicating currency....Ex: USD
  pOutEncodedBuff[0]  = pInPUCTInfo->currencyCode[0];
  pOutEncodedBuff[1]  = pInPUCTInfo->currencyCode[1];
  pOutEncodedBuff[2]  = pInPUCTInfo->currencyCode[2];

  PricePerUnit  = pInPUCTInfo->ppu;

  // This check is essential to see to it that the value does not exceed 4096 i.e; 2 POWER 12
  if(PricePerUnit > 4095)
  {
    do
    {
      PricePerUnit /= 10;
      ++Exp;
      bExpSign = 0;
      if(Exp > 7)
      {
        return EFAILED;
      }
    }while(PricePerUnit > 4095);
  }
  else
  {
    if((PricePerUnit - FFLOOR(PricePerUnit)) > 0)
    {
      do
      {
        if(PricePerUnit >= 409.5)
        {
          PricePerUnit = FFLOOR(PricePerUnit);
          break;
        }
        else
        {
          PricePerUnit *= 10;
          ++Exp;
          bExpSign = 1;
        }
      } while((PricePerUnit - FFLOOR(PricePerUnit)) > 0);
    }
  }

  // Just to make sure that PricePerUnit(PPU) is an integer...
  PPU = (uint16) FLTTOINT(PricePerUnit);

  // To make sure we use only last 12 bits.
  PPU = 0x0FFF & PPU;

  // Get the last 4 bits of PPU
  // These 4 bits form the "first half" of byte 5
  nDecodeByte5 = (uint8) PPU & 0x0F;

  // After obtaining the 4 bits, remaining bits form byte 4
  nDecodeByte4 = (uint8)(PPU >> 4);

  Exp = (Exp << 5);

  // Check for the Exponential sign
  if(1  ==  bExpSign)
  {
    // Sign should be negative.
    Exp = Exp | 0x10;
  }

  nDecodeByte5  = nDecodeByte5 | Exp;

  // Byte 4 is ready
  pOutEncodedBuff[3]  = (uint8)nDecodeByte4;

  // Byte 5 is ready
  pOutEncodedBuff[4]  = (uint8)nDecodeByte5;

  return SUCCESS;
} /* OEMCard_EncodePUCT */

/*===========================================================================
FUNCTION: OEMCard_DecodePUCT()

Description:
  This function takes a binary character array containing PUCT file data
  and decodes it to an AEEPuctInfo structure. The input array should be
  AEECARD_PUCT_FILE_LEN bytes in length.

Prototype:

  int   OEMCard_DecodePUCT(ICard *po,
                         uint8 *pInEncodedBuff,
                         AEEPuctInfo *pOutPUCTInfo);

Parameters:
  po          : [Input] Pointer to the ICard object
  pInEncodedBuff: [Input] Binary encoded form of PUCT file.
  pOutPUCTInfo : [Output] Structure containing price-per-unit and currency code.

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Failed decoding

Comments:
   None

Side Effects:
   None

See Also:
   None

===========================================================================*/
static int OEMCard_DecodePUCT(ICard *po,
                       uint8 *pInEncodedBuff,
                       AEEPuctInfo *pOutPUCTInfo)
{
  uint16   eppu;
  float    multiplier;

  if((po == NULL) || (pInEncodedBuff == NULL) ||
     (pOutPUCTInfo == NULL))
  {
    return EBADPARM;
  }

  (void)MEMSET((void *)pOutPUCTInfo, 0x00, sizeof(AEEPuctInfo));

  // First 3 bytes is the Currency code, in GSM 7-bit alphabet.
  pOutPUCTInfo->currencyCode[0] = pInEncodedBuff[0];
  pOutPUCTInfo->currencyCode[1] = pInEncodedBuff[1];
  pOutPUCTInfo->currencyCode[2] = pInEncodedBuff[2];

  // Extract out the elementary price per unit (EPPU) from bytes 3 and 4.
  eppu = (uint16) pInEncodedBuff[3];
  eppu = (eppu << 4) | (uint16)(pInEncodedBuff[4] & 0x0F);

  // Determine the multiplier using byte 4.
  if (pInEncodedBuff[4] & 0x10) {
     // Index to negative exponent.
     multiplier = power_of_ten_table[(pInEncodedBuff[4]>>5)];
  }
  else {
     // Index to positive exponent.
     multiplier = power_of_ten_table[8 + (pInEncodedBuff[4]>>5)];
  }

  // Store the price-per-unit.
  pOutPUCTInfo->ppu = (float)eppu * multiplier;

  return SUCCESS;
} /* OEMCard_DecodePUCT */

/* The following structure is used to notify clients of a new card
 * status.  */
/* typedef struct OEMCard_NOTIFIER_notify_cb
 * {
 *   uint8 mask;
 *   void *user_data;
 * } OEMCard_NOTIFIER_NOTIFY_CB;
 */

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION: OEMCardFreeNotifierData
  Frees the Notifier Data

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/
static void OEMCardFreeNotifierData(OEMCardNotifier_NOTIFY_CB *pUser)
{
  if (pUser == NULL)
    return;

  switch(pUser->mask)
  {
    case NMASK_FILE_READ:
      if(pUser->user_data != NULL)
      {
        AEECardFileContents *pFileData = pUser->user_data;
        FREEIF(pFileData->data);
        pFileData->data = NULL;
        FREEIF(pFileData);
        pFileData = NULL;
      }
      break;
    default:
      break;
  }

} /* OEMCardFreeNotifierData */

/*===========================================================================

FUNCTION: OEMCarddoNotify
   Used when the card status changes.  This causes a NOTIFY event to be
   sent to applications waiting for it.

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  The card status mask is sent with the notify event.
===========================================================================*/
static void OEMCarddoNotify(OEMCardNotifier_NOTIFY_CB *pUser)
{
  if (pUser == NULL)
    return;

  if (ICardNotifier_obj) {
    (void)ISHELL_Notify(ICardNotifier_obj->m_pIShell, AEECLSID_CARD_NOTIFIER,
        pUser->mask, pUser->user_data);
  }
#ifdef FEATURE_ICARD_IMODEL
  if (ICardModel_obj) {
    ModelEvent  evt;
    IModel     *pIModel = SYSMODELBASE_TO_IMODEL(&ICardModel_obj->base);

    evt.evCode  = pUser->mask;
    evt.dwParam = (uint32)pUser->user_data;
    evt.pModel  = pIModel;

    IMODEL_Notify(pIModel, &evt);
  }
#endif /* FEATURE_ICARD_IMODEL */
  OEMCardFreeNotifierData(pUser);
} /* OEMCarddoNotify */

/*===========================================================================

FUNCTION: OEMCardNewCardStatus
   Used when the card status changes.  This causes a NOTIFY event to be
   sent to applications waiting for it.

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  The card status mask is sent with the notify event.
===========================================================================*/
static void OEMCardNewCardStatus()
{
  if ((NULL == ICardobj) ||
     (
#ifdef FEATURE_ICARD_IMODEL
      (NULL == ICardModel_obj) &&
#endif /* FEATURE_ICARD_IMODEL */
      ((NULL == ICardNotifier_obj) ||
      ((NMASK_CARD_STATUS & ICardNotifier_obj->m_dwNotify) == 0))))
  {
    MSG_ERROR("GetCardStatus: Null ICardobj - 0x%x or Null ICardNotifier_obj - 0x%x", 
              ICardobj, ICardNotifier_obj, 0);
#ifdef FEATURE_ICARD_IMODEL
    MSG_ERROR("GetCardStatus: or Null ICardModel_obj - 0x%x", 
              ICardModel_obj, 0, 0);
#endif /* FEATURE_ICARD_IMODEL */
    return;
  }

  if (OEMCard_GetCardStatus(ICardobj, &ICardobj->m_coreObj.m_byCardStatus)
    != SUCCESS)
  {
    return;
  }

#ifdef FEATURE_ICARD_IMODEL
  ICardobj->m_CardStatusData.mask = NMASK_CARD_STATUS;
  ICardobj->m_CardStatusData.user_data = &ICardobj->m_coreObj.m_byCardStatus;
  ICardobj->m_oemCb.pfnNotify = (PFNNOTIFY) OEMCarddoNotify;
  ICardobj->m_oemCb.pNotifyData = &(ICardobj->m_CardStatusData);
  MSG_HIGH("ICARD_IMODEL: OEMCardNewCardStatus m_byCardStatus: 0x%x", 
            ICardobj->m_coreObj.m_byCardStatus, 0, 0);
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(ICardobj->m_oemCb),  0);
#else
  AEE_ResumeEx(&(ICardobj->m_oemCb), 0, 0);
#endif
#else
  ICardNotifier_obj->m_CardStatusData.mask = NMASK_CARD_STATUS;
  ICardNotifier_obj->m_CardStatusData.user_data = &ICardobj->m_coreObj.m_byCardStatus;
  ICardNotifier_obj->m_oemCb.pfnNotify = (PFNNOTIFY) OEMCarddoNotify;
  ICardNotifier_obj->m_oemCb.pNotifyData = &(ICardNotifier_obj->m_CardStatusData);
  MSG_HIGH("ICardNotifier: OEMCardNewCardStatus m_byCardStatus: 0x%x", 
            ICardobj->m_coreObj.m_byCardStatus, 0, 0);
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(ICardNotifier_obj->m_oemCb),  0);
#else
  AEE_ResumeEx(&(ICardNotifier_obj->m_oemCb), 0, 0);
#endif
#endif /* FEATURE_ICARD_IMODEL */
} /* OEMCardNewCardStatus */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================

FUNCTION: OEMCardNewCardStatus_Slot2
   Used when the card status changes.  This causes a NOTIFY event to be
   sent to applications waiting for it.

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  The card status mask is sent with the notify event.
===========================================================================*/
void OEMCardNewCardStatus_Slot2()
{
  if ((NULL == ICardobj) ||
      (
#ifdef FEATURE_ICARD_IMODEL
       (NULL == ICardModel_obj) &&
#endif /*FEATURE_ICARD_IMODEL */
      ((NULL == ICardNotifier_obj) ||
      ((NMASK_CARD_SLOT2_STATUS & ICardNotifier_obj->m_dwNotify) == 0))))
  {
    MSG_ERROR("NewCardStatus_Slot2: Null ICardobj - 0x%x or Null ICardNotifier_obj - 0x%x", 
              ICardobj, ICardNotifier_obj, 0);
#ifdef FEATURE_ICARD_IMODEL
    MSG_ERROR("NewCardStatus_Slot2: or Null ICardModel_obj - 0x%x", 
              ICardModel_obj, 0, 0);
#endif /* FEATURE_ICARD_IMODEL */
    return;
  }

  if(OEMCard_GetCardStatusSlot(ICardobj, AEECARD_SLOT2,&ICardobj->m_coreObj.m_byCardStatus_2)
    != SUCCESS)
  {
    return;
  }

#ifdef FEATURE_ICARD_IMODEL
  ICardobj->m_CardStatusData_slot2.mask = NMASK_CARD_SLOT2_STATUS;
  ICardobj->m_CardStatusData_slot2.user_data = &ICardobj->m_coreObj.m_byCardStatus_2;
  ICardobj->m_oemCb_slot2.pfnNotify = (PFNNOTIFY) OEMCarddoNotify;
  ICardobj->m_oemCb_slot2.pNotifyData = &(ICardobj->m_CardStatusData_slot2);
  MSG_HIGH("ICARD_IMODEL: OEMCardNewCardStatus_Slot2 m_CardStatusData_slot2: 0x%x", 
            ICardobj->m_coreObj.m_byCardStatus_2, 0, 0);
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(ICardobj->m_oemCb_slot2),  0);
#else
  AEE_ResumeEx(&(ICardobj->m_oemCb_slot2), 0, 0);
#endif
#else
  ICardNotifier_obj->m_CardStatusData_slot2.mask = NMASK_CARD_SLOT2_STATUS;
  ICardNotifier_obj->m_CardStatusData_slot2.user_data = &ICardobj->m_coreObj.m_byCardStatus_2;
  ICardNotifier_obj->m_oemCb_slot2.pfnNotify = (PFNNOTIFY) OEMCarddoNotify;
  ICardNotifier_obj->m_oemCb_slot2.pNotifyData = &(ICardNotifier_obj->m_CardStatusData_slot2);
  MSG_HIGH("ICardNotifier: OEMCardNewCardStatus_Slot2 m_CardStatusData_slot2: 0x%x", 
            ICardobj->m_coreObj.m_byCardStatus_2, 0, 0);
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(ICardNotifier_obj->m_oemCb_slot2),  0);
#else
  AEE_ResumeEx(&(ICardNotifier_obj->m_oemCb_slot2), 0, 0);
#endif
#endif /* FEATURE_ICARD_IMODEL */
} /* OEMCardNewCardStatus_Slot2 */
#endif


/*===========================================================================

FUNCTION: OEMCardNewPinStatus

DESCRIPTION
The pin status has changed and we need to inform the user of the new
pin status.

DEPENDENCIES
none

RETURN VALUE
none

SIDE EFFECTS
The given pin_status is sent to the user.
===========================================================================*/
static void OEMCardNewPinStatus(uint8 pin_id, AEECardPinStatus *pin_status)
{
  uint32 dwmask;

  if ((ICardobj == NULL) ||
      (
#ifdef FEATURE_ICARD_IMODEL
       (NULL == ICardModel_obj) &&
#endif /* FEATURE_ICARD_IMODEL */
      ((NULL == ICardNotifier_obj) ||
      (((NMASK_PIN1_STATUS & ICardNotifier_obj->m_dwNotify) == 0) &&
      ((NMASK_PIN1_SLOT2_STATUS & ICardNotifier_obj->m_dwNotify) == 0) &&
      ((NMASK_PIN2_STATUS & ICardNotifier_obj->m_dwNotify) == 0) &&
      ((NMASK_PIN2_SLOT2_STATUS & ICardNotifier_obj->m_dwNotify) == 0))))) {
    MSG_ERROR("NewPinStatus: Null ICardobj - 0x%x or Null ICardNotifier_obj - 0x%x", 
              ICardobj, ICardNotifier_obj, 0);
#ifdef FEATURE_ICARD_IMODEL
    MSG_ERROR("NewPinStatus: or Null ICardModel_obj - 0x%x", 
              ICardModel_obj, 0, 0);
#endif /* FEATURE_ICARD_IMODEL */
    return;
  }

  if(NULL == pin_status) {
    MSG_ERROR("NewPinStatus: pin_status pointer is NULL", 0, 0, 0);
    return;
  }

  switch (pin_id)
  {
    case AEECARD_PIN1:
      dwmask = NMASK_PIN1_STATUS;
      break;
    case AEECARD_PIN2:
      dwmask = NMASK_PIN2_STATUS;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case AEECARD_PIN1_SLOT2:
      dwmask = NMASK_PIN1_SLOT2_STATUS;
      break;
    case AEECARD_PIN2_SLOT2:
      dwmask = NMASK_PIN2_SLOT2_STATUS;
      break;
#endif
    case AEECARD_SAP_PIN:
      dwmask = NMASK_BT_PIN1_STATUS;
      break;
    default:
      ERR("PIN id 0x%x not supported", pin_id, 0, 0);
      return;
  }

  /* pin_status_data[0] is mapped to pin 1 status info
     pin_status_data[1] is mapped to pin 2 status info
     pin_status_data[2] is mapped to pin 1 slot 2 status info
     pin_status_data[3] is mapped to pin 2 slot 2 status info
     pin_status_data[5] is mapped to sap pin status info
  */
#ifdef FEATURE_ICARD_IMODEL
  ICardobj->m_pinStatusData[pin_id-1].mask = (uint8)dwmask;
  ICardobj->m_pinStatusData[pin_id-1].user_data = pin_status;
  ICardobj->m_oemPINCb[pin_id-1].pfnNotify = (PFNNOTIFY) OEMCarddoNotify;
  ICardobj->m_oemPINCb[pin_id-1].pNotifyData = &(ICardobj->m_pinStatusData[pin_id-1]);
  MSG_HIGH("ICARD_IMODEL: OEMCardNewPinStatus dwmask: 0x%x", 
            ICardobj->m_pinStatusData[pin_id-1].mask, 0, 0);
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(ICardobj->m_oemPINCb[pin_id-1]),  0);
#else
  AEE_ResumeEx(&(ICardobj->m_oemPINCb[pin_id-1]), 0, 0);
#endif
#else
  ICardNotifier_obj->m_pinStatusData[pin_id-1].mask = (uint16)dwmask;
  ICardNotifier_obj->m_pinStatusData[pin_id-1].user_data = pin_status;
  ICardNotifier_obj->m_oemPINCb[pin_id-1].pfnNotify = (PFNNOTIFY) OEMCarddoNotify;
  ICardNotifier_obj->m_oemPINCb[pin_id-1].pNotifyData = &(ICardNotifier_obj->m_pinStatusData[pin_id-1]);
  MSG_HIGH("ICardNotifier: OEMCardNewPinStatus dwmask: 0x%x", 
            ICardNotifier_obj->m_pinStatusData[pin_id-1].mask, 0, 0);
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(ICardNotifier_obj->m_oemPINCb[pin_id-1]),  0);
#else
  AEE_ResumeEx(&(ICardNotifier_obj->m_oemPINCb[pin_id-1]), 0, 0);
#endif
#endif /* FEATURE_ICARD_IMODEL */
} /* OEMCardNewPinStatus */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================

FUNCTION: OEMCardNewPersoStatus
   Used when the perso status changes.  This causes a NOTIFY event to be
   sent to applications waiting for it.

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  The card status mask is sent with the notify event.
===========================================================================*/
static void OEMCardNewPersoStatus(uint8 bySlot)
{

  if (NULL == ICardobj)
  {
    MSG_ERROR("NewPersoStatus: ICardobj Null", 0, 0, 0);
    return;
  }
  if (
#ifdef FEATURE_ICARD_IMODEL
      (NULL == ICardModel_obj) &&
#endif /* FEATURE_ICARD_IMODEL */
      (NULL == ICardNotifier_obj)) {
    MSG_ERROR("NewPersoStatus: ICardNotifier_obj Null 0x%x", 
              ICardNotifier_obj, 0, 0);
#ifdef FEATURE_ICARD_IMODEL
    MSG_ERROR("NewPersoStatus: Or ICardModel_obj Null 0x%x", 
              ICardModel_obj, 0, 0);
#endif /* FEATURE_ICARD_IMODEL */
    return;
  }
  if (
#ifdef FEATURE_ICARD_IMODEL
       (NULL == ICardModel_obj) &&
#endif /* FEATURE_ICARD_IMODEL */
      (NULL != ICardNotifier_obj)) {
    if ((AEECARD_SLOT1 == bySlot) &&
        ((NMASK_CARD_PERSO_STATUS & ICardNotifier_obj->m_dwNotify) == 0))
    {
      MSG_ERROR("NewPersoStatus: NMASK_CARD_PERSO_STATUS not registered for ICardNotifier_obj", 0, 0, 0);
      return;
    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if ((AEECARD_SLOT2 == bySlot) &&
        ((NMASK_CARD_PERSO_SLOT2_STATUS & ICardNotifier_obj->m_dwNotify) == 0))
    {
      MSG_ERROR("NewPersoStatus: NMASK_CARD_PERSO_SLOT2_STATUS not registered for ICardNotifier_obj", 0, 0, 0);
      return;
    }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
  }

  if (AEECARD_SLOT1 == bySlot) {
    if(OEMCard_GetPersoStatus(ICardobj, bySlot,
      &ICardobj->m_coreObj.m_byPersoCardStatus) != SUCCESS)
    {
      return;
    }
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else if (AEECARD_SLOT2 == bySlot) {
    if(OEMCard_GetPersoStatus(ICardobj, bySlot,
      &ICardobj->m_coreObj.m_byPersoCardStatus_2) != SUCCESS)
    {
      return;
    }
  }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

#ifdef FEATURE_ICARD_IMODEL
  if (AEECARD_SLOT1 == bySlot) {
    ICardobj->m_PersoStatusData.mask = NMASK_CARD_PERSO_STATUS;
    ICardobj->m_PersoStatusData.user_data = &ICardobj->m_coreObj.m_byPersoCardStatus;
    MSG_HIGH("ICARD_IMODEL: OEMCardNewPersoStatus NMASK_CARD_PERSO_STATUS,  perso: 0x%x", 
              ICardobj->m_coreObj.m_byPersoCardStatus, 0, 0);
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else {
    ICardobj->m_PersoStatusData.mask = NMASK_CARD_PERSO_SLOT2_STATUS;
    ICardobj->m_PersoStatusData.user_data = &ICardobj->m_coreObj.m_byPersoCardStatus_2;
    MSG_HIGH("ICARD_IMODEL: OEMCardNewPersoStatus NMASK_CARD_PERSO_SLOT2_STATUS,  perso: 0x%x", 
              ICardobj->m_coreObj.m_byPersoCardStatus_2, 0, 0);
  }
#endif /*#ifdef FEATURE_MMGSDI_DUAL_SLOT*/

  ICardobj->m_oemPersoCb.pfnNotify = (PFNNOTIFY) OEMCarddoNotify;
  ICardobj->m_oemPersoCb.pNotifyData = &(ICardobj->m_PersoStatusData);
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(ICardobj->m_oemPersoCb),  0);
#else
  AEE_ResumeEx(&(ICardobj->m_oemPersoCb), 0, 0);
#endif
#else
  if (AEECARD_SLOT1 == bySlot) {
    ICardNotifier_obj->m_PersoStatusData.mask = NMASK_CARD_PERSO_STATUS;
    ICardNotifier_obj->m_PersoStatusData.user_data = &ICardobj->m_coreObj.m_byPersoCardStatus;
    MSG_HIGH("ICardNotifier: OEMCardNewPersoStatus NMASK_CARD_PERSO_STATUS,  perso: 0x%x", 
              ICardobj->m_coreObj.m_byPersoCardStatus, 0, 0);
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else {
    ICardNotifier_obj->m_PersoStatusData.mask = NMASK_CARD_PERSO_SLOT2_STATUS;
    ICardNotifier_obj->m_PersoStatusData.user_data = &ICardobj->m_coreObj.m_byPersoCardStatus_2;
    MSG_HIGH("ICardNotifier: OEMCardNewPersoStatus NMASK_CARD_PERSO_SLOT2_STATUS,  perso: 0x%x", 
              ICardobj->m_coreObj.m_byPersoCardStatus_2, 0, 0);
  }
#endif /*#ifdef FEATURE_MMGSDI_DUAL_SLOT*/

  ICardNotifier_obj->m_oemPersoCb.pfnNotify = (PFNNOTIFY) OEMCarddoNotify;
  ICardNotifier_obj->m_oemPersoCb.pNotifyData = &(ICardNotifier_obj->m_PersoStatusData);
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(ICardNotifier_obj->m_oemPersoCb),  0);
#else
  AEE_ResumeEx(&(ICardNotifier_obj->m_oemPersoCb), 0, 0);
#endif
#endif /* FEATURE_ICARD_IMODEL */

} /* OEMCardNewPersoStatus*/

#endif /*FEATURE_MMGSDI_PERSONALIZATION */
#endif /* #ifdef FEATURE_MMGSDI */

/*===========================================================================

FUNCTION: OEMCardNotifier_MergeMasks

DESCRIPTION
  When someone asks to be notified of an event.  This function merges
  all the different requests into one variable to see if anyone is
  interested in the masks for the events the ICard Interface.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  The given pin_status is sent to the user.
===========================================================================*/
static uint32 OEMCardNotifier_MergeMasks(const uint32 *pdwMasks)
{
  uint32 dw,dwm;

  dwm=0;
  if (pdwMasks == NULL)
    return dwm;

  while ((dw = *pdwMasks) != 0)
  {
    dwm |= GET_NOTIFIER_MASK(dw);
    pdwMasks++;
  }
  return dwm;
} /* OEMCardNotifier_MergeMasks */

/*===========================================================================

FUNCTION: OEMCardNotifier_New

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
int OEMCardNotifier_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
     MSG_ERROR("OEMCardNotifier_New: pIShell null 0x%x or ppif null 0x%x",
                pIShell, ppif, 0);
     return EBADPARM;
   }

   if (AEECLSID_CARD_NOTIFIER != cls) {
     MSG_ERROR("OEMCardNotifier_New: cls 0x%x != AEECLSID_CARD_NOTIFIER",
                cls, 0, 0);
     return EUNSUPPORTED;
   }

   if (NULL == ICardNotifier_obj) {
     int card_retval;

     // Allocate the object.
     ICardNotifier_obj = (ICardNotifier *)(void*)AEE_NewClassEx((AEEVTBL(IBase) *)(void*)&gOEMCardNotifierFuncs,
                                                      sizeof(ICardNotifier), TRUE);

     if (NULL == ICardNotifier_obj) {
       MSG_ERROR("OEMCardNotifier_New: Cannot get NewClassEx", 0, 0, 0);
       return ENOMEMORY;
     }

     /* register for AEE Exit event */
     ICardNotifier_obj->m_pcbExit = sys_malloc(sizeof(AEECallback));
     if (!ICardNotifier_obj->m_pcbExit)
     {
       *ppif = NULL;
       sys_free(ICardNotifier_obj);
       ICardNotifier_obj = NULL; // JM: In case of next attempt
       MSG_ERROR("OEMCardNotifier_New: Cannot alloc m_pcbExit", 0, 0, 0);
       return ENOMEMORY;
     }

     CALLBACK_Init(ICardNotifier_obj->m_pcbExit, (PFNNOTIFY)ICardNotifier_AEEExitNotify,
                     ICardNotifier_obj);
#ifndef FEATURE_BREW_3_0
     AEE_RegisterSystemCallback(ICardNotifier_obj->m_pcbExit, AEE_SCB_AEE_EXIT,
                                 AEE_RESUME_CB_SYS,0);
#else
     ISHELL_OnExit(pIShell, ICardNotifier_obj->m_pcbExit);
#endif
     ICardNotifier_obj->pvt = (ICardNotifierVtbl *)&gOEMCardNotifierFuncs;
     ICardNotifier_obj->m_pIShell = pIShell;
     ICardNotifier_obj->m_cls = cls;
     ICardNotifier_obj->m_uRefs = 1;

     *ppif = ICardNotifier_obj;
     if ((card_retval = OEMCard_New(pIShell, AEECLSID_CARD,
                 (void **) &ICardNotifier_obj->m_cardobj)) != AEE_SUCCESS)
     {
       (void) OEMCardNotifier_Release(ICardNotifier_obj);
       return card_retval;
     }
   }
   else
   {
     if (OEMCardNotifier_AddRef(ICardNotifier_obj) > 0)
     {
       *ppif = (void *) ICardNotifier_obj;
     }
     else
     {
       *ppif = NULL;
     }
   }

   return AEE_SUCCESS;
} /* OEMCardNotifier_New */

/*===========================================================================

FUNCTION: OEMCardNotifier_AddRef

DESCRIPTION
  Indicates that one more object is now pointing to the OEMCard object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static uint32 OEMCardNotifier_AddRef(ICardNotifier *po)
{
   if (po) {
      return (++po->m_uRefs);
   }
   return 0;
} /* OEMCardNotifier_AddRef */

/*===========================================================================

FUNCTION: OEMCardNotifier_Release

DESCRIPTION
  Decreases the number of references to the ICardNotifier object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static uint32 OEMCardNotifier_Release(ICardNotifier *po)
{

#ifndef FEATURE_ICARD_IMODEL
   int32 i;
#endif

   if (!po) {
      return 0;
   }

   if (ICardNotifier_obj == NULL) {
      return 0;
   }

   (void)OEMCard_Release(ICardNotifier_obj->m_cardobj);
   if (--po->m_uRefs != 0) {
      return po->m_uRefs;
   }

   MSG_HIGH("OEMCardNotifier_Release Cleanup: ICardNotifierobj = 0x%x, exit_cb = 0x%x",
             ICardNotifier_obj, ICardNotifier_obj->m_pcbExit, 0);
   if (ICardNotifier_obj->m_pcbExit) {
     CALLBACK_Cancel(ICardNotifier_obj->m_pcbExit);
     sys_free(ICardNotifier_obj->m_pcbExit);
     ICardNotifier_obj->m_pcbExit = NULL;
   }

#ifndef FEATURE_ICARD_IMODEL
   /*Cancel Callbacks*/
   CALLBACK_Cancel( &po->m_oemCb );
   CALLBACK_Cancel( &po->m_oemCb_slot2 );
   for (i = 0; i < AEECARD_MAXPIN; i++)
   {
     CALLBACK_Cancel( &po->m_oemPINCb[i] );
   }
   CALLBACK_Cancel( &po->m_oemPersoCb );
   CALLBACK_Cancel( &po->m_oemSapCb );
#endif /* !FEATURE_ICARD_IMODEL */

   // Ref count is zero.  Release memory
   OEM_Free(po);
   ICardNotifier_obj = NULL;
   return 0;
} /* OEMCardNotifier_Release */

/*===========================================================================

FUNCTION: OEMCardNotifier_SetMask

DESCRIPTION
  This function is called by Brew when an application does an
  ISHELL_register_notify for the ICardNotifier object.  We merge
  all the masks for all the registrations and save a bitmap of
  the sum of all the events being requested.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  The given pin_status is sent to the user.
===========================================================================*/
#if MIN_BREW_BUILD(3,1,4,7)
static void OEMCardNotifier_SetMask(ICardNotifier *po, uint32 *pdwMask)
#else
static void OEMCardNotifier_SetMask(ICardNotifier *po, const uint32 *pdwMask)
#endif // MIN_BREW_BUILD(3,1,4,7)
{
  if (po == NULL)
    return;

  po->m_dwNotify = OEMCardNotifier_MergeMasks(pdwMask);
} /* OEMCardNotifier_SetMask */

#ifdef FEATURE_ICARD_IMODEL
/*===========================================================================

FUNCTION: OEMCardModel_Dtor

DESCRIPTION
  Deallocates the model object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCardModel_Dtor(SysModelBase *po)
{
   if (!po) {
      return;
   }

   if (ICardModel_obj == NULL) {
      return;
   }

   // Get rid of the model allocated data.
   SysModelBase_Dtor(po);

   if (NULL != ICardobj) {
      (void)OEMCard_Release(ICardobj);
   }

   MSG_HIGH("OEMCardModel_Release Cleanup: ICardModel_obj = 0x%x, exit_cb = 0x%x",
             ICardModel_obj, ICardModel_obj->m_pcbExit, 0);
   if (ICardModel_obj->m_pcbExit) {
     CALLBACK_Cancel(ICardModel_obj->m_pcbExit);
     sys_free(ICardModel_obj->m_pcbExit);
     ICardModel_obj->m_pcbExit = NULL;
   }
   // Ref count is zero.  Release memory
   OEM_Free(ICardModel_obj);
   ICardModel_obj = NULL;
} /* OEMCardModel_Dtor */

/*===========================================================================

FUNCTION: OEMCardModel_New

DESCRIPTION
  Allocates a new object from the heap.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
int OEMCardModel_New(IShell *pIShell, AEECLSID cls, void **ppif)
{

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
     MSG_ERROR("OEMCardModel_New: pIShell 0x%x null or ppif 0x%x null", 
               pIShell, ppif, 0);
      return EBADPARM;
   }

   *ppif = NULL;

#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   return EUNSUPPORTED;
#endif

   if (AEECLSID_CARD_MODEL != cls) {
     MSG_ERROR("OEMCardModel_New: cls 0x%x != AEECLSID_CARD_MODEL", 
               cls, 0, 0);
     return EUNSUPPORTED;
   }

   if (NULL == ICardModel_obj)
   {
     int card_retval;

     // Allocate the object.
     ICardModel_obj = (ICardModel *)sys_malloc(sizeof(ICardModel));

     if (NULL == ICardModel_obj) {
       MSG_ERROR("OEMCardModel_New: Cannot alloc ICardModel_obj", 
                 0, 0, 0);
       return ENOMEMORY;
     }

     (void)MEMSET(ICardModel_obj, 0, sizeof(ICardModel));

     SysModelBase_Ctor(&ICardModel_obj->base,
                       &ICardModel_obj->vtModel,
                       OEMCardModel_Dtor);

     /* register for AEE Exit event */
     ICardModel_obj->m_pcbExit = sys_malloc(sizeof(AEECallback));
     if (!ICardModel_obj->m_pcbExit)
     {
       (void) SysModelBase_Release(SYSMODELBASE_TO_IMODEL(&ICardModel_obj->base));
       MSG_ERROR("OEMCardModel_New: Cannot alloc ICardModel_obj->m_pcbExit", 
                 0, 0, 0);
       return ENOMEMORY;
     }

     CALLBACK_Init(ICardModel_obj->m_pcbExit, (PFNNOTIFY)ICardModel_AEEExitNotify,
                     ICardModel_obj);
#ifndef FEATURE_BREW_3_0
     AEE_RegisterSystemCallback(ICardModel_obj->m_pcbExit, AEE_SCB_AEE_EXIT,
                                 AEE_RESUME_CB_SYS,0);
#else
     ISHELL_OnExit(pIShell, ICardModel_obj->m_pcbExit);
#endif

     if ((card_retval = OEMCard_New(pIShell, AEECLSID_CARD,
                 (void **) &ICardModel_obj->m_cardobj)) != AEE_SUCCESS)
     {
       (void) SysModelBase_Release(SYSMODELBASE_TO_IMODEL(&ICardModel_obj->base));
       return card_retval;
     }
   }
   else
   {
     (void)SysModelBase_AddRef(SYSMODELBASE_TO_IMODEL(&ICardModel_obj->base));
   }

   *ppif = (void *) SYSMODELBASE_TO_IMODEL(&ICardModel_obj->base);

   return AEE_SUCCESS;
} /* OEMCardModel_New */
#endif /* FEATURE_ICARD_IMODEL */

/*===========================================================================

FUNCTION: OEMCardConnection_New

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
int OEMCardConnection_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
   boolean bFound = FALSE;
   int i = 0;
#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
     MSG_ERROR("OEMCardConnection_New: pIShell 0x%x null or ppif 0x%x null", 
               pIShell, ppif, 0);
     return EBADPARM;
   }

   if (AEECLSID_CARDCONNECTION != cls) {
     MSG_ERROR("OEMCardConnection_New: cls 0x%x != AEECLSID_CARDCONNECTION", 
               cls, 0, 0);
     return EUNSUPPORTED;
   }

   for (i = 0; i< OEMCARDCONNECTION_MAX_CHANNEL; i++ )
   {
     if (NULL == sCardChannelMapTable[i].pICardConnection_obj) {
       // Allocate the object.
       sCardChannelMapTable[i].pICardConnection_obj = (ICardConnection *)(void*)AEE_NewClassEx((AEEVTBL(IBase) *)(void*)&gOEMCardConnectionFuncs,
                                                      sizeof(ICardConnection), TRUE);

       if (NULL == sCardChannelMapTable[i].pICardConnection_obj) {
         MSG_ERROR("OEMCardConnection_New: Cannot NewClassEx", 0, 0, 0);
         return ENOMEMORY;
       }

       /* register for AEE Exit event */
       sCardChannelMapTable[i].pICardConnection_obj->m_pcbExit = sys_malloc(sizeof(AEECallback));
       if (!sCardChannelMapTable[i].pICardConnection_obj->m_pcbExit)
       {
          if (ppif) {
            *ppif = NULL;
          }
          FREE(sCardChannelMapTable[i].pICardConnection_obj);
          sCardChannelMapTable[i].pICardConnection_obj = NULL; // JM: just in case
          MSG_ERROR("OEMCardConnection_New: Cannot alloc pICardConnection_obj->m_pcbExit", 
                     0, 0, 0);
          return ENOMEMORY;
       }

       CALLBACK_Init(sCardChannelMapTable[i].pICardConnection_obj->m_pcbExit, (PFNNOTIFY)ICardConnection_AEEExitNotify,
                     sCardChannelMapTable[i].pICardConnection_obj);
#ifndef FEATURE_BREW_3_0
       AEE_RegisterSystemCallback(sCardChannelMapTable[i].pICardConnection_obj->m_pcbExit, AEE_SCB_AEE_EXIT,
                                 AEE_RESUME_CB_SYS,0);
#else
       ISHELL_OnExit(pIShell, sCardChannelMapTable[i].pICardConnection_obj->m_pcbExit);
#endif

       sCardChannelMapTable[i].pICardConnection_obj->pvt = (ICardConnectionVtbl *)&gOEMCardConnectionFuncs;
       sCardChannelMapTable[i].pICardConnection_obj->m_pIShell = pIShell;
       sCardChannelMapTable[i].pICardConnection_obj->m_cls = cls;
       sCardChannelMapTable[i].pICardConnection_obj->m_uRefs = 1;
       sCardChannelMapTable[i].bChannelStatus = OEMCARDCONNECTION_NONE;
       sCardChannelMapTable[i].dwChannelID = 0xFFFF;
       sCardChannelMapTable[i].nRequestCancel = OEMCARD_CANCEL_REASON_ENUM_MAX;

       *ppif = sCardChannelMapTable[i].pICardConnection_obj;
       bFound = TRUE;
       break;
     }
   }
   if (bFound)
   {
     return AEE_SUCCESS;
   }
   else
   {
     return EFAILED;
   }
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT*/
} /* OEMCardConnection_New */

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
/*===========================================================================

FUNCTION: OEMCardConnection_AddRef

DESCRIPTION
  Indicates that one more object is now pointing to the OEMCard object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static uint32 OEMCardConnection_AddRef(ICardConnection *po)
{
   if (po) {
      return (++po->m_uRefs);
   }
   return 0;
} /* OEMCardConnection_AddRef */

/*===========================================================================
FUNCTION: OEMCardConnection_FreeAPDURspIndex

DESCRIPTION
  Free the APDU Rsp index

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static void OEMCardConnection_FreeAPDURspIndex(uint32 index)
{
  if (index >= OEMCARDCONNECTION_MAX_RSP) {
    return;
  }
  if (sCardChannelRspTable[index].sRsp != NULL)
  {
    OEM_Free(sCardChannelRspTable[index].sRsp);
    sCardChannelRspTable[index].sRsp = NULL;
    sCardChannelRspTable[index].nSize = 0;
    sCardChannelRspTable[index].dwConnObjIndex = OEMCARDCONNECTION_MAX_CHANNEL;
  }
} /* OEMCardConnection_FreeAPDURspIndex */

/*===========================================================================

FUNCTION: OEMCardConnection_CleanupConnectionTableInfo

DESCRIPTION
  Clean up the Card Channel Connection Table

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCardConnection_CleanupConnectionTableInfo(int index)
{
  int i = 0;
  sCardChannelMapTable[index].bChannelStatus = OEMCARDCONNECTION_NONE;
  sCardChannelMapTable[index].dwChannelID = 0xFFFF;
  sCardChannelMapTable[index].nRequestCancel = OEMCARD_CANCEL_REASON_ENUM_MAX;
  if (sCardChannelMapTable[index].pICardConnection_obj != NULL) {
    sCardChannelMapTable[index].pICardConnection_obj->m_uRefs = 0;
    if (sCardChannelMapTable[index].pICardConnection_obj->m_pcbExit != NULL) {
      CALLBACK_Cancel(sCardChannelMapTable[index].pICardConnection_obj->m_pcbExit);
      sys_free(sCardChannelMapTable[index].pICardConnection_obj->m_pcbExit);
      sCardChannelMapTable[index].pICardConnection_obj->m_pcbExit = NULL;
    }
    OEM_Free(sCardChannelMapTable[index].pICardConnection_obj);
    sCardChannelMapTable[index].pICardConnection_obj = NULL;
  }
  for (i = 0; i < OEMCARDCONNECTION_MAX_RSP; i++)
  {
    if (sCardChannelRspTable[i].dwConnObjIndex == index)
    {
      OEMCardConnection_FreeAPDURspIndex(i);
    }
  }
} /* OEMCardConnection_CleanupConnectionTableInfo */

/*===========================================================================

FUNCTION: OEMCardConnection_Release

DESCRIPTION
  Remove the pointer from the table, and set the table to not in use.
  If channel is still opened, returned error.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static uint32 OEMCardConnection_Release(ICardConnection *po)
{
  OEMCardConnection_client     *pClientData;
  gsdi_slot_id_type             gsdi_slot = GSDI_SLOT_NONE;
  int                           i = 0;
  int                           nReturnStatus = SUCCESS;

  if (!po) {
    return 0;
  }

  for (i = 0; i< OEMCARDCONNECTION_MAX_CHANNEL; i++ )
  {
    if (po == sCardChannelMapTable[i].pICardConnection_obj)
    {
      if (sCardChannelMapTable[i].pICardConnection_obj == NULL) {
        return 0;
      }
      if (sCardChannelMapTable[i].pICardConnection_obj->m_uRefs > 0) {
        sCardChannelMapTable[i].pICardConnection_obj->m_uRefs--;
        if (sCardChannelMapTable[i].pICardConnection_obj->m_uRefs > 0) {
          /* still has another instance */
          return sCardChannelMapTable[i].pICardConnection_obj->m_uRefs;
        }
      }
      else {
        /* already released */
        return 0;
      }
      if ((sCardChannelMapTable[i].bChannelStatus != OEMCARDCONNECTION_OPENED) &&
          (sCardChannelMapTable[i].bChannelStatus != OEMCARDCONNECTION_SELECTED) &&
          (sCardChannelMapTable[i].bChannelStatus != OEMCARDCONNECTION_OPENING)){
        /* has closed the channel already */
        OEMCardConnection_CleanupConnectionTableInfo(i);
        return 0;
      }

      MSG_HIGH("OEMCardConnection_Release Cleanup: ICardConnectionobj = 0x%x, exit_cb = 0x%x",
                sCardChannelMapTable[i].pICardConnection_obj,
                sCardChannelMapTable[i].pICardConnection_obj->m_pcbExit,
                0);

      if (sCardChannelMapTable[i].bChannelStatus == OEMCARDCONNECTION_OPENING)
      {
        /* No channel opened yet, wait until open confirmation and then close
           before returning from this function */
        sCardChannelMapTable[i].nRequestCancel = OEMCARD_CANCEL_REASON_RELEASE;
      }
      else
      {
        /* convert the slot */
        nReturnStatus = OEMCard_MapSlot(sCardChannelMapTable[i].wSlotID, &gsdi_slot);
        if (nReturnStatus != SUCCESS) {
          return sCardChannelMapTable[i].pICardConnection_obj->m_uRefs;
        }

        /* Found the opened ICardConnection */
        pClientData = MALLOC(sizeof(OEMCardConnection_client));
        if (pClientData == NULL) {
          return sCardChannelMapTable[i].pICardConnection_obj->m_uRefs;
        }

        /* Check if the status of the card is valid */
        nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
        if (nReturnStatus != SUCCESS) {
          FREE(pClientData);
          pClientData = NULL;
          return sCardChannelMapTable[i].pICardConnection_obj->m_uRefs;
        }

        pClientData->bIsRelease = TRUE;
        pClientData->index = i;
        pClientData->dwClientData = 0;
        pClientData->bSelectAPDU = FALSE;

        if (gsdi_sim_close_channel(gsdi_slot, 0, sCardChannelMapTable[i].dwChannelID,
                                  (uint32)pClientData, OEMCardConnectionGSDIcmdCB) != GSDI_SUCCESS)
        {
          FREE( pClientData );
          pClientData = NULL;
          return sCardChannelMapTable[i].pICardConnection_obj->m_uRefs;
        }
        sCardChannelMapTable[i].nRequestCancel = OEMCARD_CANCEL_REASON_RELEASE;
      }
#if !defined(FEATURE_ICARD_NO_UI_BASE) && defined(FEATURE_UIM)
      /* wait till response */
      (void)rex_clr_sigs(&ui_tcb, UI_ICARD_SIG);
      (void)ui_wait(UI_ICARD_SIG);
      (void)rex_clr_sigs(&ui_tcb, UI_ICARD_SIG);
#endif /* FEATURE_ICARD_NO_UI_BASE */
      return sCardChannelMapTable[i].pICardConnection_obj->m_uRefs;
    }
  }
  return 0;
} /* OEMCardConnection_Release */

/*===========================================================================

FUNCTION: OEMCardConnection_QueryInterface

DESCRIPTION
  Allows applications to query if ICardConnection supports other interfaces and then
  returns a pointer to that new interface supported via ICardConnection.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCardConnection_QueryInterface(ICardConnection *po, AEECLSID id, void **ppNew)
{
   int ret_val = AEE_SUCCESS;

   if ((!po) || (!ppNew)) {
      return EBADPARM;
   }

   switch (id) {
     case AEECLSID_CARD:
         (void)OEMCard_AddRef(ICardobj);
         *ppNew = ICardobj;
         break;
     case AEECLSID_QUERYINTERFACE:
     case AEECLSID_CARDCONNECTION:
        (void)OEMCardConnection_AddRef(po);
        *ppNew = po;
        break;
     default:
         *ppNew = NULL;
         ret_val = ECLASSNOTSUPPORT;
         break;
   }

   return ret_val;
} /* OEMCardConnection_QueryInterface */

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION: OEMCard_CleanupConnectionAynchObj

DESCRIPTION
  This function is called to cleanup the Async Object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCard_CleanupConnectionAynchObj(OEMCardConnection_client *pClientData)
{
  if (NULL == pClientData)
    return;

  OEMCard_CleanUpAsynObject(pClientData->ObjHandler);
  FREEIF( pClientData );
  pClientData = NULL;
} /* OEMCard_CleanupConnectionAynchObj */

/*===========================================================================

FUNCTION: OEMCardConnection_FindNextAvailRspSlot

DESCRIPTION
  Find the next available response slot for the SendAPDU command response

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCardConnection_FindNextAvailRspSlot(
  uint32 *dwSeqNum,
  int     nSize,
  uint8  *byData,
  uint32  index)
{
  uint32 i = 0;
  if (NULL == dwSeqNum) {
    MSG_ERROR("_FindNextAvailRspSlot: dwSeqNum Null", 0, 0, 0); 
    return EFAILED;
  }

  if (nSize <= 0) {
    MSG_ERROR("_FindNextAvailRspSlot: nSize %d < 0", nSize, 0, 0); 
    return SUCCESS;
  }

  if (NULL == byData) {
    MSG_ERROR("_FindNextAvailRspSlot: byData Null", 0, 0, 0); 
    return EFAILED;
  }

  for (i = 0; i< OEMCARDCONNECTION_MAX_RSP; i++) {
    if (NULL == sCardChannelRspTable[i].sRsp) {
      sCardChannelRspTable[i].sRsp = OEM_Malloc(nSize);
      if (NULL != sCardChannelRspTable[i].sRsp)
      {
        (void)MEMCPY(sCardChannelRspTable[i].sRsp,
          byData,
          nSize);
        *dwSeqNum = i;
        sCardChannelRspTable[i].nSize = nSize;
        sCardChannelRspTable[i].dwConnObjIndex = index;
        return SUCCESS;
      }
      else {
        MSG_ERROR("_FindNextAvailRspSlot: Alloc for sRsp with size 0x%x failed", 
                  nSize, 0, 0); 
        return ENOMEMORY;
      }
    }
  }
  return EFAILED;
} /* OEMCardConnection_FindNextAvailRspSlot */

/*===========================================================================

FUNCTION: OEMCardConnection_InternalClose

DESCRIPTION
  This function will call gsdi function to close the opened channel when
  either a connection release or a callback cancel has been called on
  a channel that has an open channel.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCardConnection_InternalClose( OEMCardCancelReason_Enum nCancelReason,
                                             int32                    index,
                                             boolean                 *pSetSig)
{
  OEMCardConnection_client     *pClientDataClose = NULL;
  gsdi_slot_id_type             gsdi_slot;

  if (pSetSig == NULL)
    return;

  if (index >= OEMCARDCONNECTION_MAX_CHANNEL)
    return;

  switch (sCardChannelMapTable[index].wSlotID)
  {
  case AEECARD_SLOT1:
    gsdi_slot = GSDI_SLOT_1;
    break;
  case AEECARD_SLOT2:
    gsdi_slot = GSDI_SLOT_2;
    break;
  default:
    MSG_ERROR("Invalid Slot, 0x%x", sCardChannelMapTable[index].wSlotID, 0 , 0);
    return;
  }
  *pSetSig = FALSE;
  switch (nCancelReason)
  {
  case OEMCARD_CANCEL_REASON_RELEASE:
    /* Close the channel */
    MSG_HIGH("Request to Cancel index:0x%x", index, 0, 0);
    pClientDataClose = MALLOC(sizeof(OEMCardConnection_client));
    if (pClientDataClose == NULL) {
      MSG_ERROR("Unable to Alloc clientdata for issuing Close", 0, 0, 0);
      *pSetSig = TRUE;
      return;
    }

    pClientDataClose->bIsRelease = TRUE;
    pClientDataClose->index = index;
    pClientDataClose->dwClientData = 0;
    pClientDataClose->bSelectAPDU = FALSE;

    if (gsdi_sim_close_channel(gsdi_slot, 0,
       (int32)sCardChannelMapTable[index].dwChannelID,
      (uint32)pClientDataClose, OEMCardConnectionGSDIcmdCB) != GSDI_SUCCESS)
    {
      MSG_ERROR("Unable to queue close command", 0, 0, 0);
      FREE( pClientDataClose );
      pClientDataClose = NULL;
      *pSetSig = TRUE;
      return;
    }
    break;
  case OEMCARD_CANCEL_REASON_CB_CANCEL:
    /* Close the channel */
    MSG_HIGH("Request to Cancel index:0x%x", index, 0, 0);
    pClientDataClose = MALLOC(sizeof(OEMCardConnection_client));
    if (pClientDataClose == NULL) {
      MSG_ERROR("Unable to Alloc clientdata for issuing Close", 0, 0, 0);
      return;
    }

    pClientDataClose->bIsRelease = FALSE;
    pClientDataClose->index = index;
    pClientDataClose->dwClientData = 0;
    pClientDataClose->bSelectAPDU = FALSE;

    if (gsdi_sim_close_channel(gsdi_slot, 0,
      (int32)sCardChannelMapTable[index].dwChannelID,
      (uint32)pClientDataClose, OEMCardConnectionGSDIcmdCB) != GSDI_SUCCESS)
    {
      MSG_ERROR("Unable to queue close command", 0, 0, 0);
      FREE( pClientDataClose );
      pClientDataClose = NULL;
      return;
    }
    break;
  case OEMCARD_CANCEL_REASON_ENUM_MAX:
    MSG_HIGH("No cancel reason", 0, 0, 0);
    break;
  }
} /* OEMCardConnection_InternalClose */

/*===========================================================================

FUNCTION: OEMCardConnectionGSDIcmdCB

DESCRIPTION
  This function is called when OEMCardConnection calls GSDI during normal
  operation channel management related procedures.
  This will cause a callback to be issued to the caller.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCardConnectionGSDIcmdCB(gsdi_cnf_T *gsdi_info)
{
  AEEObjectHandle               Handler = 0;
  OEMCard_AsynCBInfo           *pAsynInfo;
  OEMCardConnection_client     *pClientData;
  int                           index;
  AEECardConnectionCmdStatus   *pCmdStatus = NULL;
  AEECardConnectionOpenStatus  *pOpenStatus = NULL;
  AEECardConnectionCloseStatus *pCloseStatus = NULL;
  boolean                       bIsSelect;
  boolean                       bIsRelease;
  int                           nStatus = SUCCESS;
  int                           sw1 = 0;
  int                           sw2 = 0;
  int                           wSlot = AEECARD_SLOT_NONE;
  int                           nSize = 0;
  boolean                       bSetSig = FALSE;

  if (NULL == gsdi_info) {
    MSG_ERROR("ConnectionGSDIcmdCB: gsdi_info NULL", 0, 0, 0); 
    return;
  }

  pClientData = (OEMCardConnection_client*)gsdi_info->message_header.client_ref;
  if (NULL == pClientData) {
    MSG_ERROR("ConnectionGSDIcmdCB: pClientData NULL", 0, 0, 0); 
    return;
  }

  index =  pClientData->index;
  if (index >= OEMCARDCONNECTION_MAX_CHANNEL)
  {
    FREE( pClientData );
    pClientData = NULL;
    MSG_ERROR("ConnectionGSDIcmdCB: Invalid index 0x%x", index, 0, 0); 
    return;
  }

  Handler = pClientData->ObjHandler;
  bIsSelect = pClientData->bSelectAPDU;
  bIsRelease = pClientData->bIsRelease;

  if (!sCardChannelMapTable[index].pICardConnection_obj) {
    FREE( pClientData );
    pClientData = NULL;
    MSG_ERROR("ConnectionGSDIcmdCB: sCardChannelMapTable[0x%x].pICardConnection_obj NULL", 
               index, 0, 0); 
    return;
  }

  if (bIsRelease) {
    if (gsdi_info->message_header.resp_type == GSDI_CLOSE_CHANNEL_RSP) {
      OEMCardConnection_CleanupConnectionTableInfo(index);
    }
    else {
      MSG_ERROR("ConnectionGSDIcmdCB: Release for resp type 0x%x", 
                gsdi_info->message_header.resp_type, 0, 0);
    }
    FREE( pClientData );
    pClientData = NULL;
#if !defined(FEATURE_ICARD_NO_UI_BASE) && defined(FEATURE_UIM)
    (void)rex_set_sigs( &ui_tcb, UI_ICARD_SIG);
#endif /* FEATURE_ICARD_NO_UI_BASE */
    return;
  }

  /* Update state */
  switch(gsdi_info->message_header.resp_type)
  {
  case GSDI_OPEN_CHANNEL_RSP:
    if (gsdi_info->message_header.slot == GSDI_SLOT_1)
    {
      wSlot = AEECARD_SLOT1;
    }
    else {
      wSlot = AEECARD_SLOT2;
    }
    if (GSDI_SUCCESS == gsdi_info->message_header.gsdi_status)
    {
      nStatus = SUCCESS;
      if (gsdi_info->open_channel_cnf.data_len > 2) {
        sw1 = gsdi_info->open_channel_cnf.data_buffer[1];
        sw2 = gsdi_info->open_channel_cnf.data_buffer[2];
      }
      else {
        sw1 = gsdi_info->open_channel_cnf.data_buffer[0];
        sw2 = gsdi_info->open_channel_cnf.data_buffer[1];
      }
      sCardChannelMapTable[index].wSlotID = wSlot;

      /* Check sw */
      if ((sw1 == 0x90) && (sw2 == 0x00)) {
        sCardChannelMapTable[index].bChannelStatus = OEMCARDCONNECTION_OPENED;
        sCardChannelMapTable[index].dwChannelID = gsdi_info->open_channel_cnf.channel_id;

        /* Check if client has request cancel */
        if(sCardChannelMapTable[index].nRequestCancel != OEMCARD_CANCEL_REASON_ENUM_MAX)
        {
          OEMCardConnection_InternalClose(sCardChannelMapTable[index].nRequestCancel,
                                          index,
                                         &bSetSig);
          /* Cancel the Open callback */
          OEMCard_CleanupConnectionAynchObj(pClientData);
          if (bSetSig)
          {
#if !defined(FEATURE_ICARD_NO_UI_BASE) && defined(FEATURE_UIM)
            (void)rex_set_sigs( &ui_tcb, UI_ICARD_SIG);
#endif /* FEATURE_ICARD_NO_UI_BASE */
          }
          return;
        }
      } /* 0x9000 */
      else
      {
        sCardChannelMapTable[index].bChannelStatus = OEMCARDCONNECTION_NONE;
        sCardChannelMapTable[index].dwChannelID = 0xFFFF;
        /* Check if client has request cancel */
        switch (sCardChannelMapTable[index].nRequestCancel)
        {
        case OEMCARD_CANCEL_REASON_ENUM_MAX:
          MSG_HIGH("No cancel reason", 0, 0, 0);
          break;
        case OEMCARD_CANCEL_REASON_CB_CANCEL:
          /* Set signal and do not call the callback */
          MSG_HIGH("Open Failed and has requested to Cancel index:0x%x", index, 0, 0);
          OEMCard_CleanupConnectionAynchObj(pClientData);
          return;
        case OEMCARD_CANCEL_REASON_RELEASE:
          /* Set signal and do not call the callback */
          MSG_HIGH("Open Failed and has requested to Cancel index:0x%x", index, 0, 0);
          OEMCard_CleanupConnectionAynchObj(pClientData);
#if !defined(FEATURE_ICARD_NO_UI_BASE) && defined(FEATURE_UIM)
          (void)rex_set_sigs( &ui_tcb, UI_ICARD_SIG);
#endif /* FEATURE_ICARD_NO_UI_BASE */
          return;
        }
      }
    }
    else
    {
      nStatus = EFAILED;
      sCardChannelMapTable[index].bChannelStatus = OEMCARDCONNECTION_NONE;
      sCardChannelMapTable[index].dwChannelID = 0xFFFF;
      switch (sCardChannelMapTable[index].nRequestCancel)
      {
      case OEMCARD_CANCEL_REASON_ENUM_MAX:
        MSG_HIGH("No cancel reason", 0, 0, 0);
        break;
      case OEMCARD_CANCEL_REASON_CB_CANCEL:
        /* Set signal and do not call the callback */
        MSG_HIGH("Open Failed and has requested to Cancel index:0x%x", index, 0, 0);
        OEMCard_CleanupConnectionAynchObj(pClientData);
        return;
      case OEMCARD_CANCEL_REASON_RELEASE:
        /* Set signal and do not call the callback */
        MSG_HIGH("Open Failed and has requested to Cancel index:0x%x", index, 0, 0);
        OEMCard_CleanupConnectionAynchObj(pClientData);
#if !defined(FEATURE_ICARD_NO_UI_BASE) && defined(FEATURE_UIM)
        (void)rex_set_sigs( &ui_tcb, UI_ICARD_SIG);
#endif /* FEATURE_ICARD_NO_UI_BASE */
        return;
      }
    }
    break;

  case GSDI_CLOSE_CHANNEL_RSP:
    if (GSDI_SUCCESS == gsdi_info->message_header.gsdi_status)
    {
      nStatus = SUCCESS;
      sw1 = gsdi_info->close_channel_cnf.data_buffer[0];
      sw2 = gsdi_info->close_channel_cnf.data_buffer[1];
      /* Check sw */
      if ((sw1 == 0x90) && (sw2 == 0x00)) {
        if (sCardChannelMapTable[index].dwChannelID == gsdi_info->close_channel_cnf.channel_id) {
          sCardChannelMapTable[index].wSlotID = 0;
          sCardChannelMapTable[index].bChannelStatus = OEMCARDCONNECTION_CLOSED;
          sCardChannelMapTable[index].dwChannelID = 0xFFFF ;
        }
        else {
          MSG_ERROR("Unmatch Channel ID %x at index = %x",
                    gsdi_info->close_channel_cnf.channel_id, index, 0);
        }
      }
    }
    else
    {
      nStatus = EFAILED;
    }
    break;

  case GSDI_SEND_CHANNEL_DATA_RSP:

    if (gsdi_info->message_header.gsdi_status == GSDI_SUCCESS) {
      nStatus = SUCCESS;
      /* no matter whether can find a spot in the table or not, check for what
         state the connection need to be in since the info was sent to the card
         and was processed */
      nSize = gsdi_info->send_channel_data_cnf.apdu_len;
      if (bIsSelect) {
        if ((gsdi_info->send_channel_data_cnf.data_buffer[nSize-2] == 0x90) &&
            (gsdi_info->send_channel_data_cnf.data_buffer[nSize-1] == 0x00)) {
          /* select was successful, move to state that allows other commands as well */
          sCardChannelMapTable[index].bChannelStatus = OEMCARDCONNECTION_SELECTED;
        }
        else {
          /* select was not successful, move to open state */
          sCardChannelMapTable[index].bChannelStatus = OEMCARDCONNECTION_OPENED;
        }
      }
    }
    else {
      /* card didn't get to process command */
      nStatus = EFAILED;
      /* card did not get to process select, move to open state */
      if (bIsSelect) {
        sCardChannelMapTable[index].bChannelStatus = OEMCARDCONNECTION_OPENED;
      }
    }
    break;
  default:
    break;

  }

  /* Populate notification data if still available */
  if((sCardChannelMapTable[index].nRequestCancel == OEMCARD_CANCEL_REASON_RELEASE) ||
    (sCardChannelMapTable[index].nRequestCancel == OEMCARD_CANCEL_REASON_CB_CANCEL)) {

    OEMCard_CleanupConnectionAynchObj(pClientData);
    sCardChannelMapTable[index].nRequestCancel = OEMCARD_CANCEL_REASON_ENUM_MAX;
    return;
  }

  CARD_ENTER_CRITICAL_SECTION
  pAsynInfo = (OEMCard_AsynCBInfo*)AEEObjectMgr_GetObject(Handler);
  CARD_LEAVE_CRITICAL_SECTION

  if (NULL == pAsynInfo) {
    OEMCard_CleanupConnectionAynchObj(pClientData);
    MSG_ERROR("ConnectionGSDIcmdCB: pAsynInfo Null", 0, 0, 0); 
    return;
  }

  if (pAsynInfo->Handler != Handler) {
    MSG_ERROR("ConnectionGSDIcmdCB: pAsynInfo->Handler mismatch 0x%x vs 0x%x",
              pAsynInfo->Handler,Handler,0);
    /* do not clean up the content because it could be invalid data, e.g.,
       we have a case where the data reflected all unused address space with
       value not == 0 */
    return;
  }

  if (NULL == pAsynInfo->pCB) {
    OEMCard_CleanupConnectionAynchObj(pClientData);
    MSG_ERROR("ConnectionGSDIcmdCB: pAsynInfo->pCB Null", 0, 0, 0); 
    return;
  }


  pCmdStatus  = pAsynInfo->pClientData;
  pOpenStatus  = pAsynInfo->pClientData;
  pCloseStatus  = pAsynInfo->pClientData;

  if (NULL == pCmdStatus)
  {
    OEMCard_CleanupConnectionAynchObj(pClientData);
    MSG_ERROR("ConnectionGSDIcmdCB: pCmdStatuspCmdStatus Null", 0, 0, 0); 
    return;
  }

  switch(gsdi_info->message_header.resp_type)
  {
  case GSDI_OPEN_CHANNEL_RSP:
    pOpenStatus->pCardConnection = sCardChannelMapTable[index].pICardConnection_obj;
    pOpenStatus->nCmdStatus = nStatus;
    pOpenStatus->wSlotID = wSlot;
    pOpenStatus->sw.sw1 = sw1;
    pOpenStatus->sw.sw2 = sw2;
    break;

  case GSDI_CLOSE_CHANNEL_RSP:
    pCloseStatus->pCardConnection = sCardChannelMapTable[index].pICardConnection_obj;
    pCloseStatus->nCmdStatus = nStatus;
    pCloseStatus->sw.sw1 = sw1;
    pCloseStatus->sw.sw2 = sw2;
    break;

  case GSDI_SEND_CHANNEL_DATA_RSP:
    pCmdStatus->pCardConnection = sCardChannelMapTable[index].pICardConnection_obj;
    pCmdStatus->dwClientData = pClientData->dwClientData;
    pCmdStatus->nSize = nSize;
    pCmdStatus->nCmdStatus = nStatus;
    if (gsdi_info->message_header.gsdi_status == GSDI_SUCCESS) {
      pCmdStatus->nCmdStatus = OEMCardConnection_FindNextAvailRspSlot(&pCmdStatus->dwSeqNum,
            pCmdStatus->nSize, gsdi_info->send_channel_data_cnf.data_buffer, index);
      if (pCmdStatus->nCmdStatus != SUCCESS) {
        /* Not able to get info into the response table */
        pCmdStatus->nSize = 0;
      }
    }
    break;
  default:
    break;
  }

  /* Validate user callback to ensure its still valid */
  if (NULL != pAsynInfo->pCB->pfnCancel) {
#if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback(pAsynInfo->pCB,  0);
#else
    AEE_ResumeEx(pAsynInfo->pCB, 0, 0);
#endif
  }
  else {
    MSG_ERROR("ConnectionGSDIcmdCB: pAsynInfo->pCB->pfnCancel NULL", 0, 0, 0 );
  }

  FREE( pClientData );
  pClientData = NULL;
} /* OEMCardConnectionGSDIcmdCB */
#endif /*#ifdef FEATURE_MMGSDI*/

/*===========================================================================

FUNCTION: OEMCardConnection_Open

DESCRIPTION
  Open a logical channel at the corresponding slot.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCardConnection_Open(ICardConnection *po, uint8 wSlotID,
                                     AEECardConnectionOpenStatus *pCmdStatus,
                                     AEECallback *pUserCB)
{
  boolean                   bFound = FALSE;
  int                       i = 0;
  OEMCardConnection_client *pClientData;
  gsdi_slot_id_type         gsdi_slot = GSDI_SLOT_NONE;
  int                       nReturnStatus = SUCCESS;

  if ((!po) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  /* convert the slot */
  nReturnStatus = OEMCard_MapSlot(wSlotID, &gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  for (i = 0; i< OEMCARDCONNECTION_MAX_CHANNEL; i++ )
  {
    if (po == sCardChannelMapTable[i].pICardConnection_obj)
    {
      if ((sCardChannelMapTable[i].bChannelStatus == OEMCARDCONNECTION_OPENED) ||
          (sCardChannelMapTable[i].bChannelStatus == OEMCARDCONNECTION_OPENING) ||
          (sCardChannelMapTable[i].bChannelStatus == OEMCARDCONNECTION_SELECTED)) {
        /* has already opened the channel */
        return EFAILED;
      }
      bFound = TRUE;
      break;
    }
  }
  if ((!bFound) || (i == OEMCARDCONNECTION_MAX_CHANNEL))
  {
    return EFAILED;
  }

  /* Found an empty index for opening a connection */
  pClientData = MALLOC(sizeof(OEMCardConnection_client));
  if (pClientData == NULL) {
    return ENOMEMORY;
  }

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBackEx(pUserCB,
                                               (void*)pCmdStatus,
                                               &(pClientData->ObjHandler),
                                               (void*)sCardChannelMapTable[i].pICardConnection_obj,
                                               (PFNCBCANCEL)OEMCard_OpenCancel);
  if (nReturnStatus != SUCCESS) {
    FREE( pClientData );
    pClientData = NULL;
    return nReturnStatus;
  }
  else {
    pClientData->bIsRelease = FALSE;
    pClientData->index = i;
    pClientData->dwClientData = 0;
    pClientData->bSelectAPDU = FALSE;
  }

  sCardChannelMapTable[i].bChannelStatus = OEMCARDCONNECTION_OPENING;
  if (gsdi_sim_open_channel(gsdi_slot, NULL, 0, 0, (uint32)pClientData, OEMCardConnectionGSDIcmdCB) != GSDI_SUCCESS)
  {
    OEMCard_CleanupConnectionAynchObj(pClientData);
    return EFAILED;
  }
  return SUCCESS;

} /* OEMCardConnection_Open */

/*===========================================================================
FUNCTION: OEMCardConnection_Close

DESCRIPTION
  Close a logical channel.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCardConnection_Close(ICardConnection *po,
                                     AEECardConnectionCloseStatus *pCmdStatus,
                                     AEECallback *pUserCB)
{
  boolean                   bFound = FALSE;
  int                       i = 0;
  OEMCardConnection_client *pClientData;
  gsdi_slot_id_type         gsdi_slot = GSDI_SLOT_NONE;
  int                       nReturnStatus = SUCCESS;

  /* Null input parameters */
  if ((!po) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  for (i = 0; i< OEMCARDCONNECTION_MAX_CHANNEL; i++ )
  {
    if (po == sCardChannelMapTable[i].pICardConnection_obj)
    {
      if ((sCardChannelMapTable[i].bChannelStatus == OEMCARDCONNECTION_OPENED) ||
          (sCardChannelMapTable[i].bChannelStatus == OEMCARDCONNECTION_OPENING) ||
          (sCardChannelMapTable[i].bChannelStatus == OEMCARDCONNECTION_SELECTED)) {
        /* has open the channel */
        bFound = TRUE;
        break;
      }
      break;
    }
  }
  if ((!bFound) || (i == OEMCARDCONNECTION_MAX_CHANNEL))
  {
    return EFAILED;
  }

  /* convert the slot */
  nReturnStatus = OEMCard_MapSlot(sCardChannelMapTable[i].wSlotID, &gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Found the opened ICardConnection */
  pClientData = MALLOC(sizeof(OEMCardConnection_client));
  if (pClientData == NULL) {
    return ENOMEMORY;
  }

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &(pClientData->ObjHandler));
  if (nReturnStatus != SUCCESS) {
    FREE( pClientData );
    pClientData = NULL;
    return nReturnStatus;
  }
  else {
    pClientData->bIsRelease = FALSE;
    pClientData->index = i;
    pClientData->dwClientData = 0;
    pClientData->bSelectAPDU = FALSE;
  }

  if (gsdi_sim_close_channel(gsdi_slot, 0, sCardChannelMapTable[i].dwChannelID,
                              (uint32)pClientData, OEMCardConnectionGSDIcmdCB) != GSDI_SUCCESS)
  {
    OEMCard_CleanupConnectionAynchObj(pClientData);
    return EFAILED;
  }
  return SUCCESS;
} /* OEMCardConnection_Close */

/*===========================================================================
FUNCTION: OEMCardConnection_SelectFilter

DESCRIPTION
  Filtering based on the select AID command

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCardConnection_SelectFilter(
  ICardConnection *po,
  AEECardAPDUCmd *pAPDU,
  boolean bSelectOnly)
{
  if ((po == NULL) || (pAPDU == NULL)) {
    return EBADPARM;
  }
  if (pAPDU->pAPDUData[OEMCARD_APDU_P1_INDEX] == OEMCARD_APDU_SELECT_BY_AID) {
    /* select by AID */
    if (pAPDU->pAPDUData[OEMCARD_APDU_LC_INDEX] < OEMCARD_APDU_MIN_SELECT_BY_AID_APDU_SIZE) {
      /* Minimum of 5 octet of AID */
      return EBADPARM;
    }
  }
  if (pAPDU->pAPDUData[OEMCARD_APDU_P1_INDEX] != OEMCARD_APDU_SELECT_BY_AID) {
    /* allow select file by ID command in the selected state */
    return bSelectOnly ? ECMDDISALLOWED : SUCCESS;
  }

#ifdef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS
#ifdef FEATURE_BREW_3_0
  if(ISHELL_CheckPrivLevel(po->m_pIShell, AEECLSID_UIMPRIV_GID, TRUE) != TRUE)
#else
  if (ISHELL_CheckPrivLevel(po->m_pIShell, PL_SYSTEM, TRUE) != TRUE)
#endif /* FEATURE_BREW_3_0 */
  {
    if(!MEMCMP(&(pAPDU->pAPDUData[OEMCARD_APDU_SELECT_DATA_START_INDEX]), oemcard_etsi_rid, OEMCARD_RID_SIZE)) {
      /* equals to ETSI RID */
      return ECMDDISALLOWED;
    }
    if(!MEMCMP(&(pAPDU->pAPDUData[OEMCARD_APDU_SELECT_DATA_START_INDEX]), oemcard_pkcs_15_rid, OEMCARD_RID_SIZE)) {
      /* equals to PKCS#15 RID */
      return ECMDDISALLOWED;
    }
    if(!MEMCMP(&(pAPDU->pAPDUData[OEMCARD_APDU_SELECT_DATA_START_INDEX]), oemcard_global_platform_rid, OEMCARD_RID_SIZE)) {
      /* equals to Global Platform RID */
      return ECMDDISALLOWED;
    }
  }
#endif /*#ifdef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS*/
  return SUCCESS;
} /* OEMCardConnection_SelectFilter */

/*===========================================================================
FUNCTION: OEMCardConnection_SendAPDU

DESCRIPTION
  Send command on a logical channel.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCardConnection_SendAPDU(ICardConnection *po, AEECardAPDUCmd *pAPDU,
                         uint32 dwClientData, AEECardConnectionCmdStatus *pCmdStatus,
                         AEECallback *pUserCB)
{
  int                       i = 0;
  OEMCardConnection_client *pClientData;
  gsdi_slot_id_type         gsdi_slot = GSDI_SLOT_NONE;
  int                       nReturnStatus = SUCCESS;
  boolean                   bSelectOnly = FALSE;
#ifdef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS
  boolean                   bSelectAPDU = FALSE;
#endif /*FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS */

  if ((!po) || (!pUserCB) || (!pCmdStatus) || (!pAPDU)) {
    return EBADPARM;
  }

  if (pAPDU->nSize < 4) {
    /* APDU manadatory header is 4 bytes */
    return EBADPARM;
  }

  for (i = 0; i< OEMCARDCONNECTION_MAX_CHANNEL; i++ )
  {
    if (po == sCardChannelMapTable[i].pICardConnection_obj)
    {
#ifdef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS
      if (sCardChannelMapTable[i].bChannelStatus == OEMCARDCONNECTION_OPENED) {
        /* has already opened the channel, allowed for select only */
        bSelectOnly = TRUE;
        break;
      }
      if (sCardChannelMapTable[i].bChannelStatus == OEMCARDCONNECTION_SELECTED) {
        /* allow any commands */
        break;
      }
      /* the channel is either closed or not yet opened */
      return EFAILED;
#else
      break;
#endif /*FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS*/
    }
  }

  /* If we have gone through all the channels then i will be set to MAX_CHANNEL
     which means that we did not find a match to po so error out */
  if (OEMCARDCONNECTION_MAX_CHANNEL == i) 
  {
    MSG_ERROR("ICardConnection Pointer did not match any CardChannel Object",
              0,0,0);
    return EFAILED;
  }

  /* Check class byte */
  switch (pAPDU->pAPDUData[OEMCARD_APDU_CLASS_INDEX]&0xF0) {
    case 0x00:
    case 0x80:
    case 0x90:
    case 0xA0:
      break;
    default:
      MSG_ERROR("Class byte 0x%x, Ch ID 0x%x",
                 pAPDU->pAPDUData[OEMCARD_APDU_CLASS_INDEX],
                 sCardChannelMapTable[i].dwChannelID,
                 0);
      return EBADPARM;
  }

  /* Check instruction type */
  switch (pAPDU->pAPDUData[OEMCARD_APDU_INST_INDEX]) {
  case 0x10: /* Terminal Profile */
  case 0xC2: /* Envelope */
  case 0x14: /* Terminal Response */
  case 0x12: /* Fetch */
  case 0x70: /* Management Channel */
  case 0xC0: /* Get Response */
    return ECMDDISALLOWED;
  case 0xA4:
    /*  pass bSelectOnly to indicate the connection state. */
    nReturnStatus = OEMCardConnection_SelectFilter(po, pAPDU, bSelectOnly);
    if (nReturnStatus != SUCCESS) {
      return nReturnStatus;
    }
#ifdef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS
    bSelectAPDU = TRUE;
#endif /*#ifdef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS*/
    break;
  default:
#ifdef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS
    if (bSelectOnly) {
      return ECMDDISALLOWED;
    }
    else {
      break;
    }
#else
    break;
#endif /*#ifdef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS*/
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* convert the slot */
  nReturnStatus = OEMCard_MapSlot(sCardChannelMapTable[i].wSlotID, &gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Found the opened ICardConnection */
  pClientData = MALLOC(sizeof(OEMCardConnection_client));
  if (pClientData == NULL) {
    return ENOMEMORY;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    FREE( pClientData );
    pClientData = NULL;
    return nReturnStatus;
  }

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &(pClientData->ObjHandler));
  if (nReturnStatus != SUCCESS) {
    FREE( pClientData );
    pClientData = NULL;
    return nReturnStatus;
  }
  else {
    pClientData->bIsRelease = FALSE;
    pClientData->index = i;
    pClientData->dwClientData = dwClientData;
#ifdef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS
    pClientData->bSelectAPDU = bSelectAPDU;
#else
    pClientData->bSelectAPDU = FALSE;
#endif
  }

  /* Mask the lower 2 bits of the instruction Class ID to zero */
  pAPDU->pAPDUData[OEMCARD_APDU_CLASS_INDEX] &= 0xFC;
  if (gsdi_sim_send_channel_data(gsdi_slot,  sCardChannelMapTable[i].dwChannelID,
    pAPDU->nSize, pAPDU->pAPDUData, (uint32)pClientData, OEMCardConnectionGSDIcmdCB) != GSDI_SUCCESS)
  {
    OEMCard_CleanupConnectionAynchObj(pClientData);
    return EFAILED;
  }
  return SUCCESS;
} /* OEMCardConnection_SendAPDU */

/*===========================================================================
FUNCTION: OEMCardConnection_GetAPDURsp

DESCRIPTION
  Get the APDU response.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCardConnection_GetAPDURsp(ICardConnection *po, uint32 dwSeqNum,
                                      AEECardAPDUResp *pAPDU)
{
  if ((!po) || (!pAPDU)) {
    return EBADPARM;
  }
  if (dwSeqNum >= OEMCARDCONNECTION_MAX_RSP) {
    return EBADPARM;
  }

  if (po != sCardChannelMapTable[sCardChannelRspTable[dwSeqNum].dwConnObjIndex].pICardConnection_obj)
  {
    return EBADPARM;
  }

  if (pAPDU->nSize > sCardChannelRspTable[dwSeqNum].nSize) {
    pAPDU->nSize = sCardChannelRspTable[dwSeqNum].nSize;
  }

  (void)MEMCPY(pAPDU->pAPDUData, sCardChannelRspTable[dwSeqNum].sRsp, pAPDU->nSize);

  /* remove the memory allocated */
  OEMCardConnection_FreeAPDURspIndex(dwSeqNum);

  return SUCCESS;
} /* OEMCardConnection_GetAPDURsp */
#endif /*FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================
FUNCTION: OEMCard_MapPersoEnum

DESCRIPTION
  Map AEECard Perso Feature Indicator Enum to GSDI enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_MapPersoEnum(
  int                   nCardPersoEnum, 
  gsdi_perso_enum_type *pGSDIPersoEnum)
{
  if (NULL == pGSDIPersoEnum) {
    MSG_ERROR("_MapPersoEnum: pGSDIPersoEnum Null", 0, 0, 0);
    return EBADPARM;
  }
  switch (nCardPersoEnum) {
#ifdef FEATURE_PERSO_SIM
    case AEECARD_NW_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_NW;
      break;
    case AEECARD_NW_SUBSET_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_NS;
      break;
    case AEECARD_SERVICE_PROVIDER_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_SP;
      break;
    case AEECARD_CORPORATE_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_CP;
      break;
    case AEECARD_SIM_USIM_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_SIM;
      break;

    case AEECARD_PROP1_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_PROP1;
      break;
#endif
#ifdef FEATURE_PERSO_RUIM
    case AEECARD_RUIM_NW1_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_RUIM_NW1;
      break;
    case AEECARD_RUIM_NW2_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_RUIM_NW2;
      break;
    case AEECARD_RUIM_HRPD_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_RUIM_HRPD;
      break;
    case AEECARD_RUIM_SERVICE_PROVIDER_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_RUIM_SP;
      break;
    case AEECARD_RUIM_CORPORATE_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_RUIM_CP;
      break;
    case AEECARD_RUIM_RUIM_PERSO_FEATURE:
      *pGSDIPersoEnum = GSDI_PERSO_RUIM_RUIM;
      break;
#endif
    default:
      MSG_ERROR("_MapPersoEnum: nCardPersoEnum 0x%x not supported", 
                nCardPersoEnum, 0, 0);
      return EBADPARM;
  }
  return SUCCESS;
}/* OEMCard_MapPersoEnum */

/*===========================================================================
FUNCTION: OEMCard_MapGSDIFeatureToCardFeature

DESCRIPTION
  Map GSDI Perso Feature Indicator Enum to AEECARD enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_MapGSDIFeatureToCardFeature(
  gsdi_perso_enum_type nGSDIPersoEnum, 
  int                 *pCardPersoEnum)
{
  if (NULL == pCardPersoEnum) {
    MSG_ERROR("_MapGSDIFeatureToCardFeature: pCardPersoEnum null", 0, 0, 0);
    return EBADPARM;
  }
  switch (nGSDIPersoEnum) {
#ifdef FEATURE_PERSO_SIM
    case GSDI_PERSO_NW:
      *pCardPersoEnum = AEECARD_NW_PERSO_FEATURE;
      break;
    case GSDI_PERSO_NS:
      *pCardPersoEnum = AEECARD_NW_SUBSET_PERSO_FEATURE;
      break;
    case GSDI_PERSO_SP:
      *pCardPersoEnum = AEECARD_SERVICE_PROVIDER_PERSO_FEATURE;
      break;
    case GSDI_PERSO_CP:
      *pCardPersoEnum = AEECARD_CORPORATE_PERSO_FEATURE;
      break;
    case GSDI_PERSO_SIM:
      *pCardPersoEnum = AEECARD_SIM_USIM_PERSO_FEATURE;
      break;
    case GSDI_PERSO_PROP1:
      *pCardPersoEnum = AEECARD_PROP1_PERSO_FEATURE;
      break;
#endif
#ifdef FEATURE_PERSO_RUIM
    case GSDI_PERSO_RUIM_NW1:
      *pCardPersoEnum = AEECARD_RUIM_NW1_PERSO_FEATURE;
      break;
    case GSDI_PERSO_RUIM_NW2:
      *pCardPersoEnum = AEECARD_RUIM_NW2_PERSO_FEATURE;
      break;
    case GSDI_PERSO_RUIM_HRPD:
      *pCardPersoEnum = AEECARD_RUIM_HRPD_PERSO_FEATURE;
      break;
    case GSDI_PERSO_RUIM_SP:
      *pCardPersoEnum = AEECARD_RUIM_SERVICE_PROVIDER_PERSO_FEATURE;
      break;
    case GSDI_PERSO_RUIM_CP:
      *pCardPersoEnum = AEECARD_RUIM_CORPORATE_PERSO_FEATURE;
      break;
    case GSDI_PERSO_RUIM_RUIM:
      *pCardPersoEnum = AEECARD_RUIM_RUIM_PERSO_FEATURE;
      break;
#endif
    default:
      MSG_ERROR("_MapGSDIFeatureToCardFeature: nGSDIPersoEnum 0x%x not supported", 
                nGSDIPersoEnum, 0, 0);
      return EBADPARM;
  }
  return SUCCESS;
} /* OEMCard_MapGSDIFeatureToCardFeature */
#endif /*#ifdef FEATURE_MMGSDI_PERSONALIZATION*/

/*===========================================================================
FUNCTION: OEMCard_ActivatePersoFeatureInd

DESCRIPTION
  Activate the Personalization feature indicator

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_ActivatePersoFeatureInd(ICard *po, uint8 wSlot, int nFeatureID, AEECardPinCode *pKey,
                                           AEECardDeactivatePersoCmdStatus *pCmdStatus, AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  AEEObjectHandle                  Handler;
  int                              nReturnStatus;
  gsdi_perso_control_key_data_type key_info;
  gsdi_returns_T                   gsdi_status;

  if ((po == NULL) || (pKey == NULL) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  (void)MEMSET(&key_info, 0, sizeof(gsdi_perso_control_key_data_type));

  nReturnStatus = OEMCard_MapSlot(wSlot, &key_info.slot );
  if(nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCard_MapPersoEnum(nFeatureID, &(key_info.feature));
  if(nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  key_info.num_bytes = pKey->code_len;
  key_info.control_key_p = (uint8*)MALLOC(key_info.num_bytes+1);
  if (key_info.control_key_p == NULL) {
    OEMCard_CleanUpAsynObject(Handler);
    return ENOMEMORY;
  }

  (void)WSTRTOSTR(pKey->code, (char*)key_info.control_key_p, (key_info.num_bytes+1));

  gsdi_status = gsdi_perso_activate_feature_indicator(
                             &key_info,
                             (uint32)Handler,
                              OEMCardGSDIPersocmdCB);
  if (key_info.control_key_p != NULL) {
    FREE(key_info.control_key_p);
    key_info.control_key_p = NULL;
  }
  if (gsdi_status != GSDI_SUCCESS) {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
} /* OEMCard_ActivatePersoFeatureInd */

/*===========================================================================
FUNCTION: OEMCard_DeactivatePersoFeatureInd

DESCRIPTION
  Deactivate the Personalization feature indicator

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_DeactivatePersoFeatureInd(ICard *po, uint8 wSlot, int nFeatureID, AEECardPinCode *pKey,
                                             AEECardDeactivatePersoCmdStatus *pCmdStatus, AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  AEEObjectHandle                  Handler = 0;
  int                              nReturnStatus;
  gsdi_perso_control_key_data_type key_info;
  gsdi_returns_T                   gsdi_status;

  if ((po == NULL) || (pKey == NULL) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  (void)MEMSET(&key_info, 0, sizeof(gsdi_perso_control_key_data_type));

  nReturnStatus = OEMCard_MapSlot(wSlot, &key_info.slot );
  if(nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCard_MapPersoEnum(nFeatureID, &(key_info.feature));
  if(nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  key_info.num_bytes = pKey->code_len;
  if (key_info.num_bytes > 0 ) {
    key_info.control_key_p = (uint8*)MALLOC(key_info.num_bytes+1);
    if (key_info.control_key_p == NULL) {
      OEMCard_CleanUpAsynObject(Handler);
      return ENOMEMORY;
    }

    (void)WSTRTOSTR(pKey->code, (char*)key_info.control_key_p, (key_info.num_bytes+1));
  }

  gsdi_status = gsdi_perso_deactivate_feature_indicator(
                             &key_info,
                             (uint32)Handler,
                              OEMCardGSDIPersocmdCB);
  if (key_info.control_key_p != NULL) {
    FREE(key_info.control_key_p);
    key_info.control_key_p = NULL;
  }
  if (gsdi_status != GSDI_SUCCESS) {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
} /* OEMCard_DeactivatePersoFeatureInd */

/*===========================================================================
FUNCTION: OEMCard_GetPersoFeatureInd

DESCRIPTION
  Get the Personalization feature indicator

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_GetPersoFeatureInd(ICard *po, uint8 wSlot,
                                      AEECardGetPersoFeatureCmdStatus *pCmdStatus,
                                      AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  AEEObjectHandle   Handler = 0;
  int               nReturnStatus;
  gsdi_returns_T    gsdi_status;
  gsdi_slot_id_type gsdi_slot;

  if ((po == NULL) || (pCmdStatus == NULL) || (!pUserCB)) {
    return EBADPARM;
  }

  nReturnStatus = OEMCard_MapSlot(wSlot, &gsdi_slot);
  if(nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  gsdi_status = gsdi_perso_get_feature_indicators(gsdi_slot, (uint32)Handler, OEMCardGSDIPersocmdCB);
  if (gsdi_status != GSDI_SUCCESS) {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;

#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
} /* OEMCard_GetPersoFeatureInd */

/*===========================================================================
FUNCTION: OEMCard_UnblockPersoDCK

DESCRIPTION
  Unblock the Personalization feature indicator

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_UnblockPersoDCK(ICard *po, uint8 wSlot, int nFeatureID, AEECardPinCode *pKey,
                                           AEECardDeactivatePersoCmdStatus *pCmdStatus, AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
  AEEObjectHandle                  Handler;
  int                              nReturnStatus;
  gsdi_perso_control_key_data_type key_info;
  gsdi_returns_T                   gsdi_status;

  if ((po == NULL) || (pKey == NULL) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  (void)MEMSET(&key_info, 0, sizeof(gsdi_perso_control_key_data_type));

  nReturnStatus = OEMCard_MapSlot(wSlot, &key_info.slot );
  if(nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCard_MapPersoEnum(nFeatureID, &(key_info.feature));
  if(nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  key_info.num_bytes = pKey->code_len;
  key_info.control_key_p = (uint8*)MALLOC(key_info.num_bytes+1);
  if (key_info.control_key_p == NULL) {
    OEMCard_CleanUpAsynObject(Handler);
    return ENOMEMORY;
  }

  (void)WSTRTOSTR(pKey->code, (char*)key_info.control_key_p, (key_info.num_bytes+1));

  gsdi_status = gsdi_perso_unblock_feature_indicator(
                             &key_info,
                             (uint32)Handler,
                              OEMCardGSDIPersocmdCB);
  if (key_info.control_key_p != NULL) {
    FREE(key_info.control_key_p);
    key_info.control_key_p = NULL;
  }
  if (gsdi_status != GSDI_SUCCESS) {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
} /* OEMCard_ActivatePersoFeatureInd */

/*===========================================================================
FUNCTION: OEMCard_GetPersoDCKNumRetries

DESCRIPTION
  Get the number of retries for the DCK

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_GetPersoDCKNumRetries(ICard *po, uint8 wSlot,
                                        AEECardDCKNumRetries *pCmdStatus, AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
  AEEObjectHandle                  Handler;
  int                              nReturnStatus;
  gsdi_slot_id_type                gsdi_slot_id;
  gsdi_returns_T                   gsdi_status;

  if ((po == NULL) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  nReturnStatus = OEMCard_MapSlot(wSlot, &gsdi_slot_id );
  if(nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  gsdi_status = gsdi_perso_get_dck_num_retries(
                             gsdi_slot_id,
                             (uint32)Handler,
                              OEMCardGSDIPersocmdCB);
  if (gsdi_status != GSDI_SUCCESS) {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
} /* OEMCard_GetPersoDCKNumRetries */

/*===========================================================================
FUNCTION: OEMCard_GetPhoneCodeStatus

DESCRIPTION
  Get the number of retries for the DCK

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_GetPhoneCodeStatus(ICard *po, uint8 wSlot,
                                        AEECardPhoneCodeStatus *pCmdStatus, AEECallback *pUserCB)
{
#if defined (FEATURE_MMGSDI_PERSONALIZATION_ENGINE) && defined(FEATURE_MMGSDI_PERSONALIZATION_NON_QC)
  AEEObjectHandle                  Handler;
  int                              nReturnStatus;
  gsdi_slot_id_type                gsdi_slot_id;
  gsdi_returns_T                   gsdi_status;

  if ((po == NULL) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  nReturnStatus = OEMCard_MapSlot(wSlot, &gsdi_slot_id );
  if(nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  gsdi_status = gsdi_perso_get_phonecode_status(
                             gsdi_slot_id,
                             (uint32)Handler,
                              OEMCardGSDIPersocmdCB);
  if (gsdi_status != GSDI_SUCCESS) {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI_PERSONALIZATION_ENGINE && FEATURE_MMGSDI_PERSONALIZATION_NON_QC*/
} /* OEMCard_GetPhoneCodeStatus */

/*===========================================================================

FUNCTION: OEMCard_SetPhoneCode

DESCRIPTION
  Sends the Set Phone code request to GSDI.  The user callback
  structure is passed to GSDI and will be returned to the call back function.
  This function then sends the callback to the user.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_SetPhoneCode(ICard *po, AEECardPinCode *pPin,
                              AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
  AEEObjectHandle   Handler = 0;
  char             *phone_code = NULL;
  int               nReturnStatus = SUCCESS;

  if ((!po) || (!pPin) || (!pUserCB) || (!pCmdStatus)) {
    return EBADPARM;
  }

  if ((pPin->code_len > 0) && (pPin->code == NULL)) {
    return EBADPARM;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(GSDI_SLOT_1);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  phone_code = MALLOC(pPin->code_len+1);
  if (phone_code == NULL)
  {
    return ENOMEMORY;
  }
  (void)WSTRTOSTR(pPin->code, phone_code, pPin->code_len+1);

  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    FREEIF(phone_code);
    phone_code = NULL;
    return nReturnStatus;
  }

  if (gsdi_perso_set_phonecode(pPin->code_len, (uint8*)phone_code, (uint32)Handler,
                       OEMCardGSDIcmdCB ) != GSDI_SUCCESS)
  {
    FREEIF(phone_code);
    phone_code = NULL;
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  FREEIF(phone_code);
  phone_code = NULL;
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC*/
} /* OEMCard_SetPhoneCode */

#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* #if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)*/

/*===========================================================================

FUNCTION: OEMCard_SAPConnect

DESCRIPTION
  Initiate SAP Connect

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_SAPConnect(ICard *po, uint8 wSlot, AEECardSAPMode SapMode,
                              AEECardSAPCmdStatus *pSAPCmdStatus, AEECallback *pUserCB)
{
#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return EUNSUPPORTED;
#endif /*#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)*/
} /* OEMCard_SAPConnect */

/*===========================================================================

FUNCTION: OEMCard_SAPDisconnect

DESCRIPTION
  Initiate SAP Disconnect

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_SAPDisconnect(ICard *po, uint8 wSlot, AEECardSAPMode SapMode,
                              AEECardSAPDisconnectMode DisconnectMode,
                              AEECardSAPCmdStatus *pSAPCmdStatus, AEECallback *pUserCB)
{
#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return EUNSUPPORTED;
#endif /*#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)*/
} /* OEMCard_SAPDisconnect */

/*===========================================================================

FUNCTION: OEMCard_SAPDeregister

DESCRIPTION
  Initiate SAP Deregister

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCard_SAPDeregister(ICard *po, uint8 wSlot, AEECardSAPCmdStatus *pSAPCmdStatus,
                                 AEECallback *pUserCB)
{
#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#else
  return EUNSUPPORTED;
#endif /*#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)*/
} /* OEMCard_SAPDeregister */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
#if defined (FEATURE_PERSO_SIM) || defined (FEATURE_PERSO_RUIM)
/*===========================================================================
FUNCTION: OEMCard_TokenizePersoData

DESCRIPTION
  Find the offset of the each Personalization Data separated by a space

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_TokenizePersoData(uint8* pSource, int nSrcSize, int nBeginOffset, int *nTokenStopOffset)
{
  int i = 0;
  if ((pSource == NULL) || (nTokenStopOffset == NULL))
  {
    return EBADPARM;
  }

  *nTokenStopOffset = nBeginOffset + nSrcSize;

  for (i = 0; i< nSrcSize; i++) {
    if (pSource[i] == 0x20) { /* space */
      *nTokenStopOffset = nBeginOffset + i;
      return SUCCESS;
    }
  }
  return SUCCESS;
} /* OEMCard_TokenizePersoData */

/*===========================================================================
FUNCTION: OEMCard_ParseSIMPersoData

DESCRIPTION
  Parse and Convert user input into BCD format for SIM Personalization

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_ParseSIMPersoData(AECHAR* pSource, uint8* pDes, int nSrcSize, int32 *pReturnSize)
{
  int i = 0;
  uint8 *pTempBuf;
  int nEndOffset;
  int nBeginOffset;

  boolean isOdd = FALSE;
  int nOutBufInitOffset;
  int j = 0;

  if ((pSource == NULL) || (pDes == NULL) || (pReturnSize == NULL))
  {
    return EBADPARM;
  }

  pTempBuf = (uint8*)MALLOC(nSrcSize+1);
  if (pTempBuf == NULL) {
    return ENOMEMORY;
  }
  (void)WSTRTOSTR(pSource, (char*)pTempBuf, (nSrcSize+1));

  *pReturnSize = 0;
  nBeginOffset = 0;
  i = 0;

  while (i< nSrcSize) {
    if (OEMCard_TokenizePersoData(&pTempBuf[i], (nSrcSize-i), i, &nEndOffset) != SUCCESS)
    {
      FREE(pTempBuf);
      pTempBuf = NULL;
      return EBADPARM;
    }
    nOutBufInitOffset = *pReturnSize;
    /* First byte indicates whether it is 2 or 3 digits MCC */
    if ((pTempBuf[nBeginOffset]-0x30) == 2)
    {
      if((nEndOffset - nBeginOffset) > 16) {
        FREE(pTempBuf);
        pTempBuf = NULL;
        return EBADPARM;
      }
      isOdd = TRUE;
    }
    else if ((pTempBuf[nBeginOffset]-0x30) == 3)
    {
      if((nEndOffset - nBeginOffset) > 17) {
        FREE(pTempBuf);
        pTempBuf = NULL;
        return EBADPARM;
      }
      isOdd = FALSE;
    }
    else
    {
      FREE(pTempBuf);
      pTempBuf = NULL;
      return EBADPARM;
    }

    /* Initialize j */
    j = nBeginOffset+1;
    if (isOdd) {
      /* First byte */
      pDes[*pReturnSize] = ((pTempBuf[j+1] - 0x30)<<4) | (pTempBuf[j] - 0x30);
      *pReturnSize = *pReturnSize + 1;
      j+=2;
      /* second byte => 0xFx */
      pDes[*pReturnSize] = 0xF0 | (pTempBuf[j] - 0x30);
      *pReturnSize = *pReturnSize + 1;
      j++;
      /* third type */
      pDes[*pReturnSize] = ((pTempBuf[j+1] - 0x30)<<4) | (pTempBuf[j] - 0x30);
      *pReturnSize = *pReturnSize + 1;
      j+=2;
    }
    else {
      /* First byte */
      pDes[*pReturnSize] = ((pTempBuf[j+1] - 0x30)<<4) | (pTempBuf[j] - 0x30);
      *pReturnSize = *pReturnSize + 1;
      j+=2;
      /* second byte */
      pDes[*pReturnSize] = ((pTempBuf[j+3] - 0x30)<<4) | (pTempBuf[j] - 0x30);
      *pReturnSize = *pReturnSize + 1;
      j++;
      /* third type */
      pDes[*pReturnSize] = ((pTempBuf[j+1] - 0x30)<<4) | (pTempBuf[j] - 0x30);
      *pReturnSize = *pReturnSize + 1;
      j+=3;
    }
    if (((nEndOffset-j)%2) != 0) {
      while (j<nEndOffset-1)
      {
        pDes[*pReturnSize] = ((pTempBuf[j+1] - 0x30)<<4) | (pTempBuf[j] - 0x30);
        *pReturnSize = *pReturnSize + 1;
        j+=2;
      }
      /* last byte */
      pDes[*pReturnSize] = 0xF0 | (pTempBuf[j] - 0x30);
      *pReturnSize = *pReturnSize + 1;
    }
    else {
      while (j<nEndOffset)
      {
        pDes[*pReturnSize] = ((pTempBuf[j+1] - 0x30)<<4) | (pTempBuf[j] - 0x30);
        *pReturnSize = *pReturnSize + 1;
        j+=2;
      }
    }

    /* How many bytes have been populated */
    j = *pReturnSize - nOutBufInitOffset;

    if (j != 8) {
      while(j < 8) {
        pDes[*pReturnSize] = 0xFF;
        j++;
        *pReturnSize = *pReturnSize+1;
      }
    }
    nBeginOffset = nEndOffset+1;
    isOdd = FALSE;
    i = nBeginOffset;
  }

  FREE(pTempBuf);
  pTempBuf = NULL;
  return SUCCESS;
} /* OEMCard_ParseSIMPersoData*/

/*===========================================================================
FUNCTION: OEMCard_ParseNonSIMPersoData

DESCRIPTION
  Parse and Convert user input into BCD format for non SIM Personalization

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_ParseNonSIMPersoData(AECHAR* pSource, uint8* pDes, int nSrcSize, int32 *pReturnSize)
{
  int i = 0;
  uint8 *pTempBuf;
  int nEndOffset;
  int nBeginOffset;
  boolean isOdd = FALSE;

  if ((pSource == NULL) || (pDes == NULL) || (pReturnSize == NULL))
  {
    return EBADPARM;
  }

  pTempBuf = (uint8*)MALLOC(nSrcSize+1);
  if (pTempBuf == NULL) {
    return ENOMEMORY;
  }
  (void)WSTRTOSTR(pSource, (char*)pTempBuf, (nSrcSize+1));

  *pReturnSize = 0;

  nBeginOffset = 0;
  i = 0;

  while (i< nSrcSize) {
    if (OEMCard_TokenizePersoData(&pTempBuf[i], (nSrcSize-i), i, &nEndOffset) != SUCCESS)
    {
      FREE(pTempBuf);
      pTempBuf = NULL;
      return EBADPARM;
    }

    if ((nEndOffset - nBeginOffset) < 5) {
      FREE(pTempBuf);
      pTempBuf = NULL;
      return EBADPARM;
    }
    if (((nEndOffset - nBeginOffset) % 2) != 0) {/* 6 digits MCC and MNC */
      isOdd = TRUE;
    }

    /* First byte */
    pDes[*pReturnSize] = ((pTempBuf[nBeginOffset+1] - 0x30)<<4) | (pTempBuf[nBeginOffset] - 0x30);
    nBeginOffset+=2;
    *pReturnSize = *pReturnSize + 1;

    /* Second byte */
    if (!isOdd) {/* 6 digits MCC and MNC */
      pDes[*pReturnSize] = ((pTempBuf[nBeginOffset+3] - 0x30)<<4) | (pTempBuf[nBeginOffset] - 0x30);
    }
    else {
      /* 5 digits MCC and MNC */
      pDes[*pReturnSize] = 0xF0 | (pTempBuf[nBeginOffset] - 0x30);
    }
    nBeginOffset+=1;
    *pReturnSize = *pReturnSize + 1;

    /* Third byte */
    pDes[*pReturnSize] = ((pTempBuf[nBeginOffset+1] - 0x30)<<4) | (pTempBuf[nBeginOffset] - 0x30);
    nBeginOffset+=2;
    *pReturnSize = *pReturnSize + 1;

    if (!isOdd) {
      nBeginOffset++;
    }

    if (nBeginOffset < nEndOffset) {
      /* Continue */
      /* SP */
      /* Fourth byte */
      pDes[*pReturnSize] = ((pTempBuf[nBeginOffset+1] - 0x30)<<4) | (pTempBuf[nBeginOffset] - 0x30);
      nBeginOffset+=2;
      *pReturnSize = *pReturnSize + 1;

      if (nBeginOffset < nEndOffset) {
        /* Continue */
        /* Corporate */
        /* Fourth byte */
        pDes[*pReturnSize] = ((pTempBuf[nBeginOffset+1] - 0x30)<<4) | (pTempBuf[nBeginOffset] - 0x30);
        nBeginOffset+=2;
        *pReturnSize = *pReturnSize + 1;
      }
    }
    nBeginOffset++;
    isOdd = FALSE;
    i = nBeginOffset;
  }

  FREE(pTempBuf);
  pTempBuf = NULL;
  return SUCCESS;
} /* OEMCard_ParseNonSIMPersoData */
#endif /* #if defined (FEATURE_PERSO_SIM) || defined (FEATURE_PERSO_RUIM) */
#endif /*FEATURE_MMGSDI_PERSONALIZATION*/

/*===========================================================================
FUNCTION: OEMCard_SetPersoFeatureData

DESCRIPTION
  Set the Personalization feature indicator

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
static int OEMCard_SetPersoFeatureData(ICard *po, uint8 wSlot, AEECardPersoFeatureInfo *pFeatureInd,
                                      AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  AEEObjectHandle                   Handler = 0;
  int                               nReturnStatus;
  gsdi_returns_T                    gsdi_status;
  gsdi_perso_set_feature_data_type  gsdi_perso_data;

  if ((po == NULL) || (pCmdStatus == NULL) || (!pUserCB) || (pFeatureInd == NULL)) {
    return EBADPARM;
  }

  if ((pFeatureInd->nSize > 0) && (pFeatureInd->pPersoData == NULL)) {
    return EBADPARM;
  }

  nReturnStatus = OEMCard_MapSlot(wSlot, &gsdi_perso_data.slot);
  if(nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCard_MapPersoEnum(pFeatureInd->nPersoFeatureInd, &(gsdi_perso_data.feature));
  if(nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  /* Get and populate the callback */
  nReturnStatus = OEMCard_GetLinkSysCallBack(pUserCB, (void*)pCmdStatus, &Handler);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  gsdi_perso_data.num_bytes = pFeatureInd->nSize;
  if (gsdi_perso_data.num_bytes > 0) {
    gsdi_perso_data.perso_data_p = MALLOC(gsdi_perso_data.num_bytes);
    if (gsdi_perso_data.perso_data_p == NULL) {
      OEMCard_CleanUpAsynObject(Handler);
      return ENOMEMORY;
    }
    switch(pFeatureInd->nPersoFeatureInd) {
#ifdef FEATURE_PERSO_SIM
    case AEECARD_NW_PERSO_FEATURE:
    case AEECARD_NW_SUBSET_PERSO_FEATURE:
    case AEECARD_SERVICE_PROVIDER_PERSO_FEATURE:
    case AEECARD_CORPORATE_PERSO_FEATURE:
      pCmdStatus->nCmdStatus = OEMCard_ParseNonSIMPersoData(pFeatureInd->pPersoData,
           gsdi_perso_data.perso_data_p, pFeatureInd->nSize, &gsdi_perso_data.num_bytes);
      break;
    case AEECARD_SIM_USIM_PERSO_FEATURE:
      pCmdStatus->nCmdStatus = OEMCard_ParseSIMPersoData(pFeatureInd->pPersoData,
           gsdi_perso_data.perso_data_p, pFeatureInd->nSize, &gsdi_perso_data.num_bytes);
      break;
#endif
#ifdef FEATURE_PERSO_RUIM
    case AEECARD_RUIM_NW1_PERSO_FEATURE:
    case AEECARD_RUIM_NW2_PERSO_FEATURE:
    case AEECARD_RUIM_HRPD_PERSO_FEATURE:
    case AEECARD_RUIM_SERVICE_PROVIDER_PERSO_FEATURE:
    case AEECARD_RUIM_CORPORATE_PERSO_FEATURE:
      pCmdStatus->nCmdStatus = OEMCard_ParseNonSIMPersoData(pFeatureInd->pPersoData,
           gsdi_perso_data.perso_data_p, pFeatureInd->nSize, &gsdi_perso_data.num_bytes);
      break;
    case AEECARD_RUIM_RUIM_PERSO_FEATURE:
      pCmdStatus->nCmdStatus = OEMCard_ParseSIMPersoData(pFeatureInd->pPersoData,
           gsdi_perso_data.perso_data_p, pFeatureInd->nSize, &gsdi_perso_data.num_bytes);
      break;
#endif
    default:
      break;
    }
    if (pCmdStatus->nCmdStatus != SUCCESS) {
      FREE(gsdi_perso_data.perso_data_p);
      gsdi_perso_data.perso_data_p = NULL;
      OEMCard_CleanUpAsynObject(Handler);
      return pCmdStatus->nCmdStatus;
    }
  }

  gsdi_status = gsdi_perso_set_feature_data(&gsdi_perso_data,
                      (uint32)Handler, OEMCardGSDIPersocmdCB);

  if (gsdi_perso_data.perso_data_p != NULL) {
    FREE(gsdi_perso_data.perso_data_p);
    gsdi_perso_data.perso_data_p = NULL;
  }
  if (gsdi_status != GSDI_SUCCESS) {
    OEMCard_CleanUpAsynObject(Handler);
    return EFAILED;
  }
  return SUCCESS;

#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
} /* OEMCard_SetPersoFeatureData */

/* ===========================================================================
Function: OEMCard_GetPersoStatus()

Description:
   This method is obtains the current perso status of the SIM/USIM/RUIM device
   at the indiated card slot from the UE.

Parameters:
   po: [Input] Pointer to the ICard object
   bySlot: [Input] Indicates which slot the card status is to be retrieved from
   pPersoStatus: [Input/Output] Pointer to status information to be filled.
  *pPersoStatus is set to one of the following:
    AEECARD_PERSO_NO_EVENT
    AEECARD_PERSO_NETWORK_FAIL
    AEECARD_PERSO_NETWORK_SUBSET_FAIL
    AEECARD_PERSO_SERVICE_PROVIDER_FAIL
    AEECARD_PERSO_CORPORATE_FAIL
    AEECARD_PERSO_SIM_FAIL
    AEECARD_PERSO_NETWORK_DEACTIVATE
    AEECARD_PERSO_NETWORK_SUBSET_DEACTIVATE
    AEECARD_PERSO_SERVICE_PROVIDER_DEACTIVATE
    AEECARD_PERSO_CORPORATE_DEACTIVATE
    AEECARD_PERSO_SIM_DEACTIVATE
    AEECARD_PERSO_NETWORK_CONTROL_BLOCKED
    AEECARD_PERSO_NETWORK_SUBSET_BLOCKED
    AEECARD_PERSO_SERVICE_PROVIDER_BLOCKED
    AEECARD_PERSO_CORPORATE_BLOCKED
    AEECARD_PERSO_SIM_BLOCKED

Return Value:

  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter

Comments:
   None

Side Effects:
   None

See Also:
   None
===========================================================================*/
static int OEMCard_GetPersoStatus(ICard *po, uint8 bySlot, uint8 *pPersoStatus)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perso_event_enum_type *pGSDIPersoStatus;

  if ((!po) || (!pPersoStatus)) {
     MSG_ERROR("_GetPersoStatus: po Null 0x%x or pPersoStatus Null 0x%x",
               po, pPersoStatus, 0);
     return EBADPARM;
  }

  if (bySlot == AEECARD_SLOT1) {
    pGSDIPersoStatus = &PersoCardStatus;
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else if (bySlot == AEECARD_SLOT2) {
    pGSDIPersoStatus = &PersoCardStatus_2;
  }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
  else {
    MSG_ERROR("_GetPersoStatus: bySlot 0x%x not supported",
               bySlot, 0, 0);
    return EBADPARM;
  }

  switch (*pGSDIPersoStatus)
  {
  case GSDI_PERSO_NO_EVENT:       /* PERSO Event Init Value                      */
    *pPersoStatus = AEECARD_PERSO_NO_EVENT;
    break;
#ifdef FEATURE_PERSO_SIM
  case GSDI_PERSO_NW_FAILURE:     /* NW Personalization Init Failure             */
    *pPersoStatus = AEECARD_PERSO_NETWORK_FAIL;
    break;
  case GSDI_PERSO_NS_FAILURE:     /* NS Personalization Init Failure             */
    *pPersoStatus = AEECARD_PERSO_NETWORK_SUBSET_FAIL;
    break;
  case GSDI_PERSO_SP_FAILURE:     /* PS Personalization Init Failure             */
    *pPersoStatus = AEECARD_PERSO_SERVICE_PROVIDER_FAIL;
    break;
  case GSDI_PERSO_CP_FAILURE:     /* CP Personalization Init Failure             */
    *pPersoStatus = AEECARD_PERSO_CORPORATE_FAIL;
    break;
  case GSDI_PERSO_SIM_FAILURE:    /* SIM/USIM Personalization Deactivated        */
    *pPersoStatus = AEECARD_PERSO_SIM_FAIL;
    break;
  case GSDI_PERSO_NW_DEACTIVATED: /* NW Personalization Deactivated              */
    *pPersoStatus = AEECARD_PERSO_NETWORK_DEACTIVATE;
    break;
  case GSDI_PERSO_NS_DEACTIVATED: /* NS Personalization Deactivated              */
    *pPersoStatus = AEECARD_PERSO_NETWORK_SUBSET_DEACTIVATE;
    break;
  case GSDI_PERSO_SP_DEACTIVATED: /* PS Personalization Deactivated              */
    *pPersoStatus = AEECARD_PERSO_SERVICE_PROVIDER_DEACTIVATE;
    break;
  case GSDI_PERSO_CP_DEACTIVATED: /* CP Personalization Deactivated              */
    *pPersoStatus = AEECARD_PERSO_CORPORATE_DEACTIVATE;
    break;
  case GSDI_PERSO_SIM_DEACTIVATED:/* SIM/USIM Personalization Deactivated        */
    *pPersoStatus = AEECARD_PERSO_SIM_DEACTIVATE;
    break;
  case GSDI_PERSO_NCK_BLOCKED:    /* Network Control Key was Blocked             */
    *pPersoStatus = AEECARD_PERSO_NETWORK_CONTROL_BLOCKED;
    break;
  case GSDI_PERSO_NSK_BLOCKED:    /* Network Subset Control Key was Blocked      */
    *pPersoStatus = AEECARD_PERSO_NETWORK_SUBSET_BLOCKED;
    break;
  case GSDI_PERSO_SPK_BLOCKED:    /* Service Provider Control Key was Blocked    */
    *pPersoStatus = AEECARD_PERSO_SERVICE_PROVIDER_BLOCKED;
    break;
  case GSDI_PERSO_CCK_BLOCKED:    /* Corporate Control Key was Blocked           */
    *pPersoStatus = AEECARD_PERSO_CORPORATE_BLOCKED;
    break;
  case GSDI_PERSO_PPK_BLOCKED:    /* SIM/USIM Control Key was Blocked            */
    *pPersoStatus = AEECARD_PERSO_SIM_BLOCKED;
    break;
#endif /* FEATURE_PERSO_SIM */

#ifdef FEATURE_PERSO_RUIM
  case GSDI_PERSO_RUIM_NW1_FAILURE:     /* NW Personalization Init Failure             */
    *pPersoStatus = AEECARD_PERSO_RUIM_NETWORK1_FAIL;
    break;
  case GSDI_PERSO_RUIM_NW2_FAILURE:     /* NW Personalization Init Failure             */
    *pPersoStatus = AEECARD_PERSO_RUIM_NETWORK2_FAIL;
    break;
  case GSDI_PERSO_RUIM_HRPD_FAILURE:     /* NS Personalization Init Failure             */
    *pPersoStatus = AEECARD_PERSO_RUIM_HRPD_FAIL;
    break;
  case GSDI_PERSO_RUIM_SP_FAILURE:     /* PS Personalization Init Failure             */
    *pPersoStatus = AEECARD_PERSO_RUIM_SERVICE_PROVIDER_FAIL;
    break;
  case GSDI_PERSO_RUIM_CP_FAILURE:     /* CP Personalization Init Failure             */
    *pPersoStatus = AEECARD_PERSO_RUIM_CORPORATE_FAIL;
    break;
  case GSDI_PERSO_RUIM_RUIM_FAILURE:    /* SIM/USIM Personalization Deactivated        */
    *pPersoStatus = AEECARD_PERSO_RUIM_RUIM_FAIL;
    break;
  case GSDI_PERSO_RUIM_NW1_DEACTIVATED: /* NW Personalization Deactivated              */
    *pPersoStatus = AEECARD_PERSO_RUIM_NETWORK1_DEACTIVATE;
    break;
  case GSDI_PERSO_RUIM_NW2_DEACTIVATED: /* NW Personalization Deactivated              */
    *pPersoStatus = AEECARD_PERSO_RUIM_NETWORK2_DEACTIVATE;
    break;
  case GSDI_PERSO_RUIM_HRPD_DEACTIVATED: /* NS Personalization Deactivated              */
    *pPersoStatus = AEECARD_PERSO_RUIM_HRPD_DEACTIVATE;
    break;
  case GSDI_PERSO_RUIM_SP_DEACTIVATED: /* PS Personalization Deactivated              */
    *pPersoStatus = AEECARD_PERSO_RUIM_SERVICE_PROVIDER_DEACTIVATE;
    break;
  case GSDI_PERSO_RUIM_CP_DEACTIVATED: /* CP Personalization Deactivated              */
    *pPersoStatus = AEECARD_PERSO_RUIM_CORPORATE_DEACTIVATE;
    break;
  case GSDI_PERSO_RUIM_RUIM_DEACTIVATED:/* SIM/USIM Personalization Deactivated        */
    *pPersoStatus = AEECARD_PERSO_RUIM_RUIM_DEACTIVATE;
    break;
  case GSDI_PERSO_RUIM_NCK1_BLOCKED:    /* Network Control Key was Blocked             */
    *pPersoStatus = AEECARD_PERSO_RUIM_NETWORK1_CONTROL_BLOCKED;
    break;
  case GSDI_PERSO_RUIM_NCK2_BLOCKED:    /* Network Control Key was Blocked             */
    *pPersoStatus = AEECARD_PERSO_RUIM_NETWORK2_CONTROL_BLOCKED;
    break;
  case GSDI_PERSO_RUIM_HNCK_BLOCKED:    /* Network Subset Control Key was Blocked      */
    *pPersoStatus = AEECARD_PERSO_RUIM_HRPD_BLOCKED;
    break;
  case GSDI_PERSO_RUIM_SPCK_BLOCKED:    /* Service Provider Control Key was Blocked    */
    *pPersoStatus = AEECARD_PERSO_RUIM_SERVICE_PROVIDER_BLOCKED;
    break;
  case GSDI_PERSO_RUIM_CCK_BLOCKED:    /* Corporate Control Key was Blocked           */
    *pPersoStatus = AEECARD_PERSO_RUIM_CORPORATE_BLOCKED;
    break;
  case GSDI_PERSO_RUIM_PCK_BLOCKED:    /* SIM/USIM Control Key was Blocked            */
    *pPersoStatus = AEECARD_PERSO_RUIM_RUIM_BLOCKED;
    break;
#endif /* FEATURE_PERSO_RUIM */

  case GSDI_PERSO_SANITY_ERROR:
    *pPersoStatus = AEECARD_PERSO_FAILED;
    break;
  case GSDI_PERSO_GEN_PROP1:
    *pPersoStatus = AEECARD_PERSO_PROP1_EVENT;
    break;
  case GSDI_PERSO_GEN_PROP2:
    *pPersoStatus = AEECARD_PERSO_PROP2_EVENT;
    break;
  default:
    MSG_ERROR("_GetPersoStatus: *pGSDIPersoStatus 0x%x not supported",
               *pGSDIPersoStatus, 0, 0);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
} /* OEMCard_GetPersoStatus*/

/* ===========================================================================
Function: OEMCard_GetServiceAvailableInd()

Description:
   This method is obtains the service indicaton that the client request.  It
   indicates if the service is available in UMTS/GSM/CDMA

Parameters:
   po: [Input] Pointer to the ICard object
   bySlot: [Input] Indicates which slot the card status is to be retrieved from
   deService: [input] Indicates the service that is being query for
   pSvcInd: [Input/Output] Pointer to result of the service indication.  Caller
                           should only read this data if the return status is
                           SUCCESS

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
   None

Side Effects:
   None

See Also:
   None
===========================================================================*/
static int OEMCard_GetServiceAvailableInd(ICard *po, uint8 bySlot,
                                   AEECardServiceEnumType dwService,
                                   AEECardServiceIndType *pSvcInd)
{
#ifdef FEATURE_MMGSDI
  int                nReturnStatus = SUCCESS;
  gsdi_slot_id_type  gsdi_slot     = GSDI_SLOT_NONE;
  gsdi_svr_rsp_type  gsdi_svr_rsp;
  gsdi_services_type gsdi_service;
  boolean            bIsSpecialService = FALSE;
  gsdi_svr_rsp_type  gsdi_svr_rsp2;
  gsdi_services_type gsdi_service2;

  if ((po == NULL) || (pSvcInd == NULL)) {
    return EBADPARM;
  }

  /* memset the client input to zero */
  (void)MEMSET(pSvcInd, 0x00, sizeof(AEECardServiceIndType));

  /* convert the slot */
  nReturnStatus = OEMCard_MapSlot(bySlot, &gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* Check if the status of the card is valid */
  nReturnStatus = OEMCard_CheckValidCardStatus(gsdi_slot);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

  /* convert the service request */
  nReturnStatus = OEMCard_MapService(dwService, &gsdi_service);
  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  gsdi_svr_rsp = gsdi2_lib_is_service_available(gsdi_slot, gsdi_service);
#else
  gsdi_svr_rsp = gsdi_lib_is_service_available(gsdi_service);
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

  bIsSpecialService = OEMCard_IsSpecialService(dwService);
  if (bIsSpecialService) { /* e.g. MO SMS Control, there are 2 enums in GSDI */
    nReturnStatus = OEMCard_MapNextServiceEnum(dwService, &gsdi_service2);
    if (nReturnStatus == SUCCESS) { /* if failed, go straight to the status/service population */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      gsdi_svr_rsp2 = gsdi2_lib_is_service_available(gsdi_slot, gsdi_service2);
#else
      gsdi_svr_rsp2 = gsdi_lib_is_service_available(gsdi_service2);
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
      if ((gsdi_svr_rsp2.gsdi_status != GSDI_SUCCESS) && (gsdi_svr_rsp.gsdi_status != GSDI_SUCCESS)) {
        /* both failed */
        return EFAILED;
      }
      else {
        if ((gsdi_svr_rsp2.gsdi_status == GSDI_SUCCESS) && (gsdi_svr_rsp.gsdi_status == GSDI_SUCCESS)) {
          gsdi_svr_rsp.svr_bitmap |= gsdi_svr_rsp2.svr_bitmap;
        }
        else {
          if (gsdi_svr_rsp2.gsdi_status == GSDI_SUCCESS) {
            /* replace gsdi_Svr_rsp with gsdi_svr_rsp */
            gsdi_svr_rsp.gsdi_status = gsdi_svr_rsp2.gsdi_status;
            gsdi_svr_rsp.svr_bitmap = gsdi_svr_rsp2.svr_bitmap;
          }
        }
      }
    }
  }

  /* Done with the service check */
  if (gsdi_svr_rsp.gsdi_status == GSDI_SUCCESS) {
    if((gsdi_svr_rsp.svr_bitmap & 0x10) == 0x10) { /* has USIM service */
      pSvcInd->bUMTSSvcOn = TRUE;
    }
    if((gsdi_svr_rsp.svr_bitmap & 0x01) == 0x01) { /* has GSM service */
      pSvcInd->bGSMSvcOn = TRUE;
    }
    if((gsdi_svr_rsp.svr_bitmap & 0x04) == 0x04) { /* has CDMA service */
      pSvcInd->bCDMASvcOn = TRUE;
    }
    return SUCCESS;
  }
  else {
    return EFAILED;
  }

#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
} /* OEMCard_GetServiceAvailableInd */

#ifdef FEATURE_MMGSDI
/* ===========================================================================
Function: OEMCard_ScanDupFilesForRefresh()

Description:
   Scans the file for duplicate files. ICard creates one combined list for all
   clients and passes that list to GSDI.

Parameters:
   pCompleteFileList: [Input/Output] List of files
   totalNumFiles: [Output]  Total Mumber of files in that list

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
   None

Side Effects:
   None

See Also:
   None
===========================================================================*/
static int OEMCard_ScanDupFilesForRefresh
(
  uim_items_enum_type  *pCompleteFileList,
  uint32 *totalNumFiles
)
{
  OEMCardFileChangeNotify *temp = pCardFileChangeNotify;
  uim_items_enum_type  pNewCompleteFileList[AEECARD_MAX_FILE_CHANGE_NOTIFICATIONS] = {UIM_ICCID};
  unsigned int newTotal = 0;

  if (totalNumFiles == NULL)
    return EBADPARM;

  *totalNumFiles = 0;

  //calculate the actual total number of files without dups
  while(temp)
  {
    unsigned int i,j;
    boolean match = FALSE;

    for(i=0;i<temp->dwNumFiles;i++)
    {
      match = FALSE;
      if (pCompleteFileList != NULL)
      {
        for(j=0;j<*totalNumFiles;j++)
        {
          if(temp->pNotifyFileList[i] == pCompleteFileList[j])
          {
            match = TRUE;
            break;
          }
        }
      }
      if(!match)
      {
        if((*totalNumFiles+newTotal+1) > AEECARD_MAX_FILE_CHANGE_NOTIFICATIONS)
        {
          return EFAILED;
        }
        pNewCompleteFileList[*totalNumFiles+newTotal] = temp->pNotifyFileList[i];
        newTotal++;
      }
    }
    *totalNumFiles = newTotal;
    (void)MEMCPY(pCompleteFileList, pNewCompleteFileList, newTotal*sizeof(uim_items_enum_type));
    temp = temp->next;
  }

  return SUCCESS;
} /* OEMCard_ScanDupFilesForRefresh */

/*===========================================================================
Function: OEMCard_GSDIRefreshCb()

Description:
   The Callback is called when a file that has been refreshed is read by ICard

Parameters:
   gsdi_info: [Input] GSDI Response data

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None
===========================================================================*/
static void OEMCard_GSDIRefreshCb(
  gsdi_cnf_T * gsdi_info
)
{
   static OEMCardNotifier_NOTIFY_CB pData;
   gsdi_returns_T                   gsdi_status = GSDI_SUCCESS;

   if(gsdi_info == NULL )
   {
     MSG_ERROR("OEMCard_GSDIRefreshCb: gsdi_info is NULL",0,0,0);
     return;
   }
   if (ICardobj == NULL)
   {
     MSG_ERROR("OEMCard_GSDIRefreshCb: null ICardobj", 0, 0, 0);
     return;
   }

   switch(gsdi_info->message_header.resp_type)
   {
     case GSDI_SIM_READ_RSP:
       // For the Response, gather the data and send a notification to all clients
       if(gsdi_info->message_header.gsdi_status == GSDI_SUCCESS)
       {
         gsdi_read_cnf_T *pReadCnf = &gsdi_info->read_cnf;
         AEECardFileContents *pFileContents = NULL;
         gsdi_file_enum_type gsdiFileEnum;

         ICardobj->m_bNumFileRefreshed++;
         if((ICardobj->m_bRefreshPending) &&
            (ICardobj->m_bNumFileRefreshed == sTempFileChangeNotify.dwNumFiles))
         {
           ICardobj->m_bRefreshPending = FALSE;
           gsdi_status = gsdi_refresh_complete(sTempFileChangeNotify.dwNumFiles,
                                       sTempFileChangeNotify.pNotifyFileList,
                                       TRUE,
                                       0,
                                       OEMCard_GSDIRefreshCb);
           if (gsdi_status != GSDI_SUCCESS)
           {
             MSG_ERROR("Unable to Send refresh complete to GSDI 0x%x", gsdi_status, 0, 0);
           }
         }

         // Convert to AEECARD file type
         gsdi_status = gsdi_util_map_uim_enum_to_file_enum((uim_items_enum_type)pReadCnf->sim_filename ,&gsdiFileEnum);
         if(gsdi_status != GSDI_SUCCESS)
         {
           MSG_ERROR("gsdi_util_map_uim_enum_to_file_enum: fail 0x%x for file 0x%x",
             gsdi_status,(uim_items_enum_type)pReadCnf->sim_filename,0);
           return;
         }

         pFileContents = (AEECardFileContents*)MALLOC(sizeof(AEECardFileContents));

         if(pFileContents == NULL)
         {
           MSG_ERROR("OEMCard_GSDIRefreshCb: pFileContents is NULL",0,0,0);
           return;
         }

         if(OEMCard_MapFromGSDIFileEnum(gsdiFileEnum,&pFileContents->filename) != SUCCESS)
         {
           MSG_ERROR("OEMCard_MapFromGSDIFileEnum fail for file 0x%x",
             gsdiFileEnum,0,0);
           FREE(pFileContents);
           pFileContents = NULL;
           return;
         }

         pFileContents->data  = (uint8*)MALLOC(pReadCnf->returned_data_len);

         // Check for MALLOC
         if(pFileContents->data == NULL)
         {
           FREE(pFileContents);
           pFileContents = NULL;
           MSG_ERROR("OEMCard_GSDIRefreshCb, pFileContents->data is NULL",0,0,0);
           return;
         }
         pFileContents->length  =pReadCnf->returned_data_len;
         (void)MEMCPY((uint8*)pFileContents->data, (uint8*)pReadCnf->data,pReadCnf->returned_data_len );

         MSG_HIGH("_GSDIRefreshCb: Notify NMAKE_FILE_READ", 0, 0, 0);
         pData.mask= NMASK_FILE_READ;
         pData.user_data = pFileContents;
#ifdef FEATURE_ICARD_IMODEL
         ICardobj->m_oemCb.pfnNotify = (PFNNOTIFY)OEMCarddoNotify;
         ICardobj->m_oemCb.pNotifyData = &(pData);
#if MIN_BREW_VERSION(3,0)
         AEE_ResumeCallback(&(ICardobj->m_oemCb),  0);
#else
         AEE_ResumeEx(&(ICardobj->m_oemCb), 0, 0);
#endif
#else
         ICardNotifier_obj->m_oemCb.pfnNotify   =(PFNNOTIFY)OEMCarddoNotify;
         ICardNotifier_obj->m_oemCb.pNotifyData = &(pData);
#if MIN_BREW_VERSION(3,0)
         AEE_ResumeCallback(&(ICardNotifier_obj->m_oemCb),  0);
#else
         AEE_ResumeEx(&(ICardNotifier_obj->m_oemCb), 0, 0);
#endif
#endif /* FEATURE_ICARD_IMODEL */
         /*Do not clean up pfileContents and data, it will be cleaned
           up after the  event is sent to the application*/
       }
       else {
         if(ICardobj->m_bRefreshPending)
         {
           /* Refresh read failed */
           MSG_ERROR("Unable to do SIM Read on the file 0x%x", gsdi_status, 0, 0);
           MSG_ERROR("Reset the m_bRefershPending to False, and notify GSDI the read failure", 0, 0, 0);
           ICardobj->m_bRefreshPending = FALSE;
           ICardobj->m_bNumFileRefreshed = sTempFileChangeNotify.dwNumFiles;
           gsdi_status = gsdi_refresh_complete(sTempFileChangeNotify.dwNumFiles,
                                       sTempFileChangeNotify.pNotifyFileList,
                                       FALSE,
                                       0,
                                       OEMCard_GSDIRefreshCb);
           if (gsdi_status != GSDI_SUCCESS)
           {
             MSG_ERROR("Unable to Send refresh complete to GSDI 0x%x", gsdi_status, 0, 0);
           }
         }
       }
       break;

     default:
       if(gsdi_info->message_header.gsdi_status != GSDI_SUCCESS)
       {
         MSG_HIGH("OEMCard_GSDIRefreshCb Recvd %x",gsdi_info->message_header.gsdi_status,0,0);
         return;
       }
       break;
   }
} /* OEMCard_GSDIRefreshCb */

/*===========================================================================
Function: OEMCardRefreshFile()

Description:
   The Callback is in UI context called when a file gets changed in GSDI

Parameters:
   pFileChangeNotify: [Input] GSDI Response data

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None
===========================================================================*/
static void OEMCardRefreshFile
(
  OEMCardFileChangeNotify *pFileChangeNotify
)
{
  unsigned int i;
  byte                dummy_data_p;
  int                 dummy_rec_num_accessed;
  gsdi_returns_T      gsdi_status = GSDI_SUCCESS;
  boolean             pending_read = FALSE;

  if (pFileChangeNotify == NULL)
    return;

  for(i=0;i<pFileChangeNotify->dwNumFiles;i++)
  {
    // Only care about these files right now.
    switch(pFileChangeNotify->pNotifyFileList[i])
    {
      case UIM_GSM_CSP:
      case UIM_USIM_CSP:
      case UIM_USIM_SPN:
      case UIM_GSM_SPN:
      case UIM_GSM_PNN:
      case UIM_USIM_PNN:
      case UIM_USIM_SPDI:
      case UIM_GSM_SPDI:
        ICardobj->m_bRefreshPending = TRUE;
        (void)MEMCPY(&sTempFileChangeNotify, pFileChangeNotify, sizeof(OEMCardFileChangeNotify));

        gsdi_status = gsdi_sim_read(
          pFileChangeNotify->pNotifyFileList[i], GSDI_NO_INDEX, 0,
          NULL, 0, &dummy_data_p, 0, 0, &dummy_rec_num_accessed, 0,
          OEMCard_GSDIRefreshCb);
        if (gsdi_status != GSDI_SUCCESS)
        {
          MSG_ERROR("Unable to do SIM Read on the file 0x%x", gsdi_status, 0, 0);
          MSG_ERROR("Reset the m_bRefershPending to False, and notify GSDI the read failure", 0, 0, 0);
          ICardobj->m_bRefreshPending = FALSE;
          ICardobj->m_bNumFileRefreshed = sTempFileChangeNotify.dwNumFiles;
          (void) gsdi_refresh_complete(sTempFileChangeNotify.dwNumFiles,
                                       sTempFileChangeNotify.pNotifyFileList,
                                       FALSE,
                                       0,
                                       OEMCard_GSDIRefreshCb);
          return;
        }
        pending_read = TRUE;
        /* Has sent a Read to GSDI, no need to send refresh complete until
           ICard received the response from the READ */
        break;
      default:
       /* Limitation, if clients register file other than the ones listed above, they 
          will not be notified */ 
        break;
    }
  }
  if (pending_read == FALSE)
  {
    /* Come here when we didn't queue any command for the file read */
    (void) gsdi_refresh_complete(pFileChangeNotify->dwNumFiles,
                                 pFileChangeNotify->pNotifyFileList,
                                 TRUE,
                                 0,
                                 OEMCard_GSDIRefreshCb);
  }
} /* OEMCardRefreshFile */

/*===========================================================================
Function: OEMCard_RefreshFileChangeNotificationCb()

Description:
   The Callback is called when a file gets changed in GSDI

Parameters:
   gsdi_info: [Input] GSDI Response data

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None
===========================================================================*/
static void OEMCard_RefreshFileChangeNotificationCb
(
  uim_items_enum_type  *file_list_p,
  uint8                 num_files
)
{
  static OEMCardFileChangeNotify sFileChange;
  if ((NULL == ICardobj) ||
      (NULL == ICardNotifier_obj))
  {
    MSG_ERROR("_RefreshFileChangeNotificationCb: Null Icardobj 0x%x or Null icardnotifier_obj",
              ICardobj, ICardNotifier_obj, 0);
    return;
  }

  sFileChange.dwNumFiles = num_files;
  ICardobj->m_bNumFileRefreshed = 0;
  (void)MEMCPY(sFileChange.pNotifyFileList,file_list_p,num_files *sizeof(uim_items_enum_type));

#ifdef FEATURE_ICARD_IMODEL
  ICardobj->m_oemCb.pfnNotify = (PFNNOTIFY) OEMCardRefreshFile;
  ICardobj->m_oemCb.pNotifyData = &(sFileChange);
  MSG_HIGH("_RefreshFileChangeNotificationCb: ICARD_IMODEL, notify file changed",
            0, 0, 0);
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(ICardobj->m_oemCb),  0);
#else
  AEE_ResumeEx(&(ICardobj->m_oemCb), 0, 0);
#endif
#else
  ICardNotifier_obj->m_oemCb.pfnNotify = (PFNNOTIFY) OEMCardRefreshFile;
  ICardNotifier_obj->m_oemCb.pNotifyData = &(sFileChange);
  MSG_HIGH("_RefreshFileChangeNotificationCb: ICardNotifier, notify file changed",
            0, 0, 0);
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(ICardNotifier_obj->m_oemCb),  0);
#else
  AEE_ResumeEx(&(ICardNotifier_obj->m_oemCb), 0, 0);
#endif
#endif /* FEATURE_ICARD_IMODEL */

} /* OEMCard_RefreshFileChangeNotificationCb */
#endif /* FEATURE_MMGSDI */

/*===========================================================================
Function: OEMCard_RegisterForFCN()

Description:
   Allows clients to register for File Change Notifications.
   All files from all clients are combined into one list and the
   registration is done once with GSDI.

Parameters:
       po: [Input] ICARD object
  dwClsID: [Input] Class ID of the client
dwNumFile: [Input] Total Number of files
pFileList: [Input] List of files

Return Value:
   SUCCESS
   EFAILED

Comments:
   None

Side Effects:
   Client would be aware of any refreshes occuring due to file change

See Also:
   None
===========================================================================*/
static int OEMCard_RegisterForFCN
(
  ICard *po,
  uint32 dwClsID,
  uint32 dwNumFiles,
  AEECardFileItemEnumType *pFileList
)
{
#ifdef FEATURE_MMGSDI

  OEMCardFileChangeNotify *pFileChangeNotify = NULL;
  int                      nReturnStatus = SUCCESS;
  uint32                   totalNumFiles = 0;
  uim_items_enum_type      pCompleteFileList[AEECARD_MAX_FILE_CHANGE_NOTIFICATIONS];
  OEMCardFileChangeNotify *temp = pCardFileChangeNotify;
  uint8                    i=0;

  if(ICardobj == NULL || po == NULL)
    return EBADPARM;

  /*File change notification should not exceed maximum */
  if( dwNumFiles >= AEECARD_MAX_FILE_CHANGE_NOTIFICATIONS
    || dwClsID == 0 )
  {
    return EBADPARM;
  }

  /*If FNumber of files is 0, implies that App is deregistering
    The FileList in that case should be null*/
  if(dwNumFiles != 0 && pFileList == NULL)
  {
    return EBADPARM;
  }

  pFileChangeNotify = (OEMCardFileChangeNotify*)sys_malloc(sizeof(OEMCardFileChangeNotify));
  if (pFileChangeNotify == NULL)
  {
    return ENOMEMORY;
  }
  pFileChangeNotify->dwClsID = dwClsID;
  pFileChangeNotify->dwNumFiles = dwNumFiles;
  pFileChangeNotify->next = NULL;

   /*Convert AEECard File Name to UIM Filename*/
  for(i=0;i<dwNumFiles;i++)
  {
      gsdi_file_enum_type gsdi_file_item;
      nReturnStatus = OEMCard_MapFileEnum(pFileList[i], &gsdi_file_item);
      if (nReturnStatus != SUCCESS) {
        sys_free(pFileChangeNotify);
        pFileChangeNotify = NULL;
        return nReturnStatus;
      }

      if (gsdi_util_map_file_enum_to_uim_enum(gsdi_file_item, &pFileChangeNotify->pNotifyFileList[i]) != GSDI_SUCCESS) {
        sys_free(pFileChangeNotify);
        pFileChangeNotify = NULL;
        return EFAILED;
      }
  }

  /*It may be the head*/
  if(pCardFileChangeNotify == NULL)
  {
    pCardFileChangeNotify = pFileChangeNotify;
  }
  else
  {
    OEMCardFileChangeNotify *prev = pCardFileChangeNotify;
    boolean nodeFound = FALSE;
    while(temp != NULL)
    {
      if(temp->dwClsID == dwClsID)
      {
        if(dwNumFiles == 0)
        {
          // App is no longer interested in File Change, remove the link to this node.
          if(pCardFileChangeNotify== temp)
          {
            pCardFileChangeNotify = temp->next;
          }
          else
          {
            prev->next = temp->next;
          }
          sys_free(temp);
          nodeFound = TRUE;
          break;
        }
        else
        {
          pFileChangeNotify->next = temp->next;
          // just need to update the pointer.
          if(pCardFileChangeNotify== temp)
          {
            pCardFileChangeNotify = pFileChangeNotify;
          }
          else
          {
            prev->next = pFileChangeNotify;
          }
          sys_free(temp);
          nodeFound = TRUE;
          break;
        }
      }
      prev = temp;
      temp = temp->next;
    }
    //Prev cannot be NULL.
    if(!nodeFound)
      prev->next = pFileChangeNotify;
  }

  //Skip the return value, all checks have been done already
  (void)OEMCard_ScanDupFilesForRefresh(pCompleteFileList,&totalNumFiles);

  if(gsdi_register_for_file_change_notifications(
      OEMCard_RefreshFileChangeNotificationCb,
      totalNumFiles,
      pCompleteFileList,
      0,
      OEMCard_GSDIRefreshCb) != GSDI_SUCCESS)
  {
      return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif
} /* OEMCard_RegisterForFCN */

