#ifndef AEEIBTMGR_H
#define AEEIBTMGR_H
/*=============================================================================
FILE:         AEEIBTMgr.h

SERVICES:     IBTMgr Interfaces

DESCRIPTION:  IBTMgr is the interface to the underlying Bluetooth driver.
              This interface provides methods to control the Bluetooth radio,
              hence it is reserved for applications with system privilege.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEBTDef.h"
#include "AEEISignal.h"

/*=============================================================================
 Interface ID
=============================================================================*/

#define AEEIID_IBTMgr    	0x0105397c

/*=============================================================================
 BTMgr Operation types
=============================================================================*/
#define AEEBTMGR_OP_NONE                     0
#define AEEBTMGR_OP_SET_NAME                 1
#define AEEBTMGR_OP_SET_COD                  2
#define AEEBTMGR_OP_SET_SEC                  3
#define AEEBTMGR_OP_SET_ROLE                 4
#define AEEBTMGR_OP_SET_ROLE_SW_POLICY       5
#define AEEBTMGR_OP_SET_BLOCKED_DEV_LIST     6
#define AEEBTMGR_OP_SET_SERVICE_SEC          7
#define AEEBTMGR_OP_AUTH_CONN                8
#define AEEBTMGR_OP_TUNE_PERF                9
#define AEEBTMGR_OP_DISABLE_RADIO           10
#define AEEBTMGR_OP_ENABLE_RADIO            11
#define AEEBTMGR_OP_DISABLE_VISIBILITY      12
#define AEEBTMGR_OP_RESTORE_VISIBILITY      13

typedef uint32 BTMgrOperationType;

/*=============================================================================
 BTMgr Event types
=============================================================================*/
#define AEEBTMGR_EV_NONE                           0
#define AEEBTMGR_EV_SET_NAME_SUCCESS               1
#define AEEBTMGR_EV_SET_NAME_FAILED                2
#define AEEBTMGR_EV_SET_COD_SUCCESS                3
#define AEEBTMGR_EV_SET_COD_FAILED                 4
#define AEEBTMGR_EV_SET_SEC_SUCCESS                5
#define AEEBTMGR_EV_SET_SEC_FAILED                 6
#define AEEBTMGR_EV_SET_ROLE_SUCCESS              10
#define AEEBTMGR_EV_SET_ROLE_FAILED               11
#define AEEBTMGR_EV_SET_SW_POL_SUCCESS            12
#define AEEBTMGR_EV_SET_SW_POL_FAILED             13
#define AEEBTMGR_EV_SET_BLOCKED_DEVS_SUCCESS      14
#define AEEBTMGR_EV_SET_BLOCKED_DEVS_FAILED       15
#define AEEBTMGR_EV_SET_SVC_SEC_SUCCESS           16
#define AEEBTMGR_EV_SET_SVC_SEC_FAILED            17
#define AEEBTMGR_EV_AUTH_CONN_SUCCESS             18
#define AEEBTMGR_EV_AUTH_CONN_FAILED              19
#define AEEBTMGR_EV_PERF_TUNING_SUCCESS           20
#define AEEBTMGR_EV_PERF_TUNING_FAILED            21
#define AEEBTMGR_EV_DISABLE_RADIO_SUCCESS         22
#define AEEBTMGR_EV_DISABLE_RADIO_FAILED          23
#define AEEBTMGR_EV_ENABLE_RADIO_SUCCESS          24
#define AEEBTMGR_EV_ENABLE_RADIO_FAILED           25
#define AEEBTMGR_EV_DISABLE_VISIBILITY_SUCCESS    26
#define AEEBTMGR_EV_DISABLE_VISIBILITY_FAILED     27
#define AEEBTMGR_EV_RESTORE_VISIBILITY_SUCCESS    28
#define AEEBTMGR_EV_RESTORE_VISIBILITY_FAILED     29
#define AEEBTMGR_EV_RADIO_OFF                     30
#define AEEBTMGR_EV_RADIO_ON                      31
#define AEEBTMGR_EV_VISIBILITY_OFF                32
#define AEEBTMGR_EV_VISIBILITY_ON                 33
#define AEEBTMGR_EV_SETTINGS_CHANGED              34
#define AEEBTMGR_EV_MAX                       0xFFFF

typedef uint32 BTMgrEventType;

/*=============================================================================
 Local Settings Bitmap
=============================================================================*/
#define AEEBT_LOCAL_NAME_B                0x0001
#define AEEBT_LOCAL_COD_B                 0x0002
#define AEEBT_LOCAL_PREF_ROLE_B           0x0004
#define AEEBT_LOCAL_ROLE_SW_POLICY_B      0x0008
#define AEEBT_LOCAL_SEC_B                 0x0010
#define AEEBT_LOCAL_SERVICE_SEC_B         0x0020
#define AEEBT_LOCAL_ISCAN_PERF_B          0x0040
#define AEEBT_LOCAL_PSCAN_PERF_B          0x0080

typedef uint32  BTLocalSettingBitmap;

/*=============================================================================
 Average Connection Time (ACT) versus Power
=============================================================================*/
#define AEEBT_AVP_AUTOMATIC                  0
#define AEEBT_AVP_SMALLEST_ACT_MOST_POWER    1
#define AEEBT_AVP_LARGER_ACT_LESS_POWER      2
#define AEEBT_AVP_LARGEST_ACT_LEAST_POWER    3

typedef uint32  BTActVsPwr;

/*=============================================================================
 Service Security Identifier
=============================================================================*/
#define AEEBT_SEC_BY_SERVICE_CLASS           1
#define AEEBT_SEC_BY_SERVER_CHANNEL          2
#define AEEBT_SEC_BY_L2CAP_PSM               3
#define AEEBT_SEC_BY_PROTOCOL                4

typedef uint32 BTSvcSecIDMethod;

/*=============================================================================
 Structures
=============================================================================*/
typedef struct 
{
   BDAddress            bdAddr;
   uint8                u8Name[ AEEBT_MAX_LEN_DEVICE_NAME ];
   BTLMPFeatures        lmpFeatures;
   BTLMPVersion         lmpVersion;
   BTClassOfDevice      CoD;
   BTSecurityLevel      dSecLevel;
   boolean              bRoleSwitchAllowed;
   boolean              bPrefMasterRole;
   BTActVsPwr           dPgScanParam;
   BTActVsPwr           dInqScanParam;
}  BTLocalSettings;

typedef struct
{
   BTSvcSecIDMethod     dIDMethod;     // which of the following field is valid
   int                  nNumItems;     // number of services/protocols
   BTProtocol           dProtocol[5];  // RFCOMM, L2CAP, ...
   BTServiceClassUUID   dSvcCls[5];    // Service Class ID list
   uint32               uSvrChnl[5];   // server channel numbers
   uint32               uPSM[5];       // L2CAP PSM numbers

}  BTServiceIdentifier;

/*=============================================================================
                         IBTMgr Interface Definition
=============================================================================*/
#define INHERIT_IBTMgr(iname)  \
   INHERIT_IQI(iname);        \
   int (*OnEventNotify)      (iname*                      po,               \
                              ISignal*                    piSignal);        \
   int (*GetEvent)           (iname*                      po,               \
                              BTMgrEventType*             pdEventID,        \
                              BTResult*                   pdResult);        \
   int (*GetRadioState)      (iname*                      po,               \
                              BTRadioState*               pdRadioState);    \
   int (*GetLocalSettings)   (iname*                      po,               \
                              BTLocalSettingBitmap*       pdWhatsValid,     \
                              BTLocalSettings*            pSettings);       \
   int (*SetName)            (iname*                      po,               \
                              const uint8*                pszName);         \
   int (*SetCoD)             (iname*                      po,               \
                              const BTClassOfDevice*      pCoD);            \
   int (*SetSecurity)        (iname*                      po,               \
                              BTSecurityLevel             dSecLevel);       \
   int (*SetPrefConnRole)    (iname*                      po,               \
                              boolean                     bMaster);         \
   int (*SetRoleSwitchPolicy)(iname*                      po,               \
                              boolean                     bAllow);          \
   int (*SetBlockedDevList)  (iname*                      po,               \
                              int                         nNumDevs,         \
                              const BDAddress*            pAddrList);       \
   int (*SetSvcSecurity)     (iname*                      po,               \
                              BTSecurityLevel             dSecLevel,        \
                              const BTServiceIdentifier*  pSvcIDfier);      \
   int (*OnConnRequest)      (iname*                      po,               \
                              ISignal*                    piSignal);        \
   int (*GetConnRequest)     (iname*                      po,               \
                              BDAddress*                  pBDAddr,          \
                              BTServiceIdentifier*        pdSvcIDfier);     \
   int (*AuthorizeConnReq)   (iname*                      po,               \
                              const BDAddress*            pBDAddr,          \
                              const BTServiceIdentifier*  psSvcIDfier,      \
                              boolean                     bAuthorize);      \
   int (*TuneScanPerformance)(iname*                      po,               \
                              BTActVsPwr                  pageScanParam,    \
                              BTActVsPwr                  inquiryScanParam);\
   int (*DisableRadio)       (iname*                      po);              \
   int (*EnableRadio)        (iname*                      po);              \
   int (*DisableVisibility)  (iname*                      po);              \
   int (*RestoreVisibility)  (iname*                      po)

AEEINTERFACE_DEFINE(IBTMgr);

static __inline uint32 IBTMgr_AddRef (IBTMgr* p)
{
   return AEEGETPVTBL((p),IBTMgr)->AddRef((p));
}

static __inline uint32 IBTMgr_Release (IBTMgr* p)
{
   return AEEGETPVTBL((p),IBTMgr)->Release((p));
}

static __inline int IBTMgr_QueryInterface (IBTMgr* p,
                                           AEECLSID clsid,
                                           void **ppo)
{
   return AEEGETPVTBL((p),IBTMgr)->QueryInterface((p),(clsid),(ppo));
}

static __inline int IBTMgr_OnEventNotify (IBTMgr* p, ISignal* piSignal)
{
   return AEEGETPVTBL((p),IBTMgr)->OnEventNotify((p),(piSignal));
}

static __inline int IBTMgr_GetEvent (IBTMgr* p,
                                     BTMgrEventType* pdEvent,
                                     BTResult* pdResult)
{
   return AEEGETPVTBL((p),IBTMgr)->GetEvent((p),(pdEvent),(pdResult));
}

static __inline int IBTMgr_GetRadioState (IBTMgr* p, BTRadioState* pRadioState)
{
   return AEEGETPVTBL((p),IBTMgr)->GetRadioState((p),(pRadioState));
}

static __inline int IBTMgr_GetLocalSettings (IBTMgr* p, 
                                             BTLocalSettingBitmap*  pdValid,
                                             BTLocalSettings*       pS)
{
   return AEEGETPVTBL((p),IBTMgr)->GetLocalSettings((p),(pdValid),(pS));
}

static __inline int IBTMgr_SetName (IBTMgr* p, const uint8* pu8Name)
{
   return AEEGETPVTBL((p),IBTMgr)->SetName((p),(pu8Name));
}

static __inline int IBTMgr_SetCoD (IBTMgr* p, const BTClassOfDevice* pCoD)
{
   return AEEGETPVTBL((p),IBTMgr)->SetCoD((p),(pCoD));
}

static __inline int IBTMgr_SetSecurity (IBTMgr* p, BTSecurityLevel dSec)
{
   return AEEGETPVTBL((p),IBTMgr)->SetSecurity((p),(dSec));
}

static __inline int IBTMgr_SetPrefConnRole (IBTMgr* p, boolean bM)
{
   return AEEGETPVTBL((p),IBTMgr)->SetPrefConnRole((p),(bM));
}

static __inline int IBTMgr_SetRoleSwitchPolicy (IBTMgr* p,
                                                boolean bA)
{
   return AEEGETPVTBL((p),IBTMgr)->SetRoleSwitchPolicy((p),(bA));
}

static __inline int IBTMgr_SetSvcSecurity (IBTMgr* p, 
                                           BTSecurityLevel dSec,
                                           const BTServiceIdentifier* pSvcID)
{
   return AEEGETPVTBL((p),IBTMgr)->SetSvcSecurity((p),(dSec),(pSvcID));
}

static __inline int IBTMgr_SetBlockedDevList (IBTMgr* p,
                                              int nNumDevs,
                                              const BDAddress* pList)
{
  return AEEGETPVTBL((p),IBTMgr)->SetBlockedDevList((p),(nNumDevs),(pList));
}

static __inline int IBTMgr_OnConnRequest (IBTMgr* p, ISignal* piSignal)
{
   return AEEGETPVTBL((p),IBTMgr)->OnConnRequest((p),(piSignal));
}

static __inline int IBTMgr_GetConnRequest (IBTMgr* p, 
                                           BDAddress* pA,
                                           BTServiceIdentifier* pS)
{
   return AEEGETPVTBL((p),IBTMgr)->GetConnRequest((p),(pA),(pS));
}

static __inline int IBTMgr_AuthorizeConnReq (IBTMgr* p,
                                             const BDAddress* pA,
                                             const BTServiceIdentifier* psSvcID,
                                             boolean bA)
{
   return AEEGETPVTBL((p),IBTMgr)->AuthorizeConnReq((p),(pA),(psSvcID),(bA));
}

static __inline int IBTMgr_TuneScanPerformance (IBTMgr* p,
                                                BTActVsPwr pg,
                                                BTActVsPwr i)
{
   return AEEGETPVTBL((p),IBTMgr)->TuneScanPerformance((p),(pg),(i));
}

static __inline int IBTMgr_DisableRadio (IBTMgr* p)
{
   return AEEGETPVTBL((p),IBTMgr)->DisableRadio((p));
}

static __inline int IBTMgr_EnableRadio (IBTMgr* p)
{
   return AEEGETPVTBL((p),IBTMgr)->EnableRadio((p));
}

static __inline int IBTMgr_DisableVisibility (IBTMgr* p)
{
   return AEEGETPVTBL((p),IBTMgr)->DisableVisibility((p));
}

static __inline int IBTMgr_RestoreVisibility (IBTMgr* p)
{
   return AEEGETPVTBL((p),IBTMgr)->RestoreVisibility((p));
}

/*=============================================================================
DATA STRUCTURE DOCUMENTATION
===============================================================================
BTMgrEventType

Description:
   This data type specifies the events that could occur on an IBTMgr object.

   Applications should never receive AEEBTMGR_EV_NONE and AEEBTMGR_EV_MAX.

Definition:
   #define AEEBTMGR_EV_NONE                           0
   #define AEEBTMGR_EV_SET_NAME_SUCCESS               1
   #define AEEBTMGR_EV_SET_NAME_FAILED                2
   #define AEEBTMGR_EV_SET_COD_SUCCESS                3
   #define AEEBTMGR_EV_SET_COD_FAILED                 4
   #define AEEBTMGR_EV_SET_SEC_SUCCESS                5
   #define AEEBTMGR_EV_SET_SEC_FAILED                 6
   #define AEEBTMGR_EV_SET_ROLE_SUCCESS              10
   #define AEEBTMGR_EV_SET_ROLE_FAILED               11
   #define AEEBTMGR_EV_SET_SW_POL_SUCCESS            12
   #define AEEBTMGR_EV_SET_SW_POL_FAILED             13
   #define AEEBTMGR_EV_SET_BLOCKED_DEVS_SUCCESS      14
   #define AEEBTMGR_EV_SET_BLOCKED_DEVS_FAILED       15
   #define AEEBTMGR_EV_SET_SVC_SEC_SUCCESS           16
   #define AEEBTMGR_EV_SET_SVC_SEC_FAILED            17
   #define AEEBTMGR_EV_AUTH_CONN_SUCCESS             18
   #define AEEBTMGR_EV_AUTH_CONN_FAILED              19
   #define AEEBTMGR_EV_PERF_TUNING_SUCCESS           20
   #define AEEBTMGR_EV_PERF_TUNING_FAILED            21
   #define AEEBTMGR_EV_DISABLE_RADIO_SUCCESS         22
   #define AEEBTMGR_EV_DISABLE_RADIO_FAILED          23
   #define AEEBTMGR_EV_ENABLE_RADIO_SUCCESS          24
   #define AEEBTMGR_EV_ENABLE_RADIO_FAILED           25
   #define AEEBTMGR_EV_DISABLE_VISIBILITY_SUCCESS    26
   #define AEEBTMGR_EV_DISABLE_VISIBILITY_FAILED     27
   #define AEEBTMGR_EV_RESTORE_VISIBILITY_SUCCESS    28
   #define AEEBTMGR_EV_RESTORE_VISIBILITY_FAILED     29
   #define AEEBTMGR_EV_RADIO_OFF                     30
   #define AEEBTMGR_EV_RADIO_ON                      31
   #define AEEBTMGR_EV_VISIBILITY_OFF                32
   #define AEEBTMGR_EV_VISIBILITY_ON                 33
   #define AEEBTMGR_EV_SETTINGS_CHANGED              34
   #define AEEBTMGR_EV_MAX                       0xFFFF
   typedef uint32 BTMgrEventType;

===============================================================================
BTLocalSettingBitmap

Description:
   This data type defines the bitmap of local device settings that can change.
   
Definition:
   #define AEEBT_LOCAL_NAME_B                0x0001
   #define AEEBT_LOCAL_COD_B                 0x0002
   #define AEEBT_LOCAL_PREF_ROLE_B           0x0004
   #define AEEBT_LOCAL_ROLE_SW_POLICY_B      0x0008
   #define AEEBT_LOCAL_SEC_B                 0x0010
   #define AEEBT_LOCAL_SERVICE_SEC_B         0x0020
   #define AEEBT_LOCAL_ISCAN_PERF_B          0x0040
   #define AEEBT_LOCAL_PSCAN_PERF_B          0x0080
   
   typedef uint32  BTLocalSettingBitmap;

===============================================================================
BTActVsPwr

Description:
   This data type defines radio scan performance in terms of average connection 
   time versus power consumption.
   
Definition:
   #define AEEBT_AVP_AUTOMATIC                0
   #define AEEBT_AVP_SMALLEST_ACT_MOST_POWER  1
   #define AEEBT_AVP_LARGER_ACT_LESS_POWER    2
   #define AEEBT_AVP_LARGEST_ACT_LEAST_POWER  3
   typedef uint32  BTActVsPwr;

===============================================================================
BTSvcSecIDMethod

Description:
   This data type defines the methods for identifying a service.
   
Definition:
   #define AEEBTSEC_BY_SERVICE_CLASS        1
   #define AEEBTSEC_BY_SERVER_CHANNEL       2
   #define AEEBTSEC_BY_L2CAP_PSM            3
   #define AEEBTSEC_BY_PROTOCOL             4
   typedef uint32 BTSvcSecIDMethod;

===============================================================================
BTLocalSettings

Description:
   This data type defines the local device settings.
   
Definition:
   typedef struct 
   {
      BTClassOfDevice      sCoD;
      BTSecurityLevel      dSecLevel;
      boolean              bRoleSwitchAllowed;
      boolean              bPrefMasterRole;
      BTActVsPwr           dPgScanParam;
      BTActVsPwr           dInqScanParam;
      uint8                u8Name[ AEEBT_MAX_LEN_DEVICE_NAME ];
      int                  nNameLen;
   
   }  BTLocalSettings;

Members:
   sCoD                 : class of device
   dSecLevel            : overall security level
   bRoleSwitchAllowed   : should incoming role switch request be allowed?
   bPrefMasterRole      : prefers to switch to master role in connections
                            initiated by remote device?
   dPgScanParam         : page scan perfomance parameter
   dInqScanParam        : inquiry scan performance parameter
   u8Name               : Bluetooth name of local device
   nNameLen             : length of local name

===============================================================================
BTServiceIdentifier

Description:
   This data type defines the service identifier used in service security 
   control.
   
Definition:

   typedef struct
   {
      BTSvcSecIDMethod     dIDMethod;     // which of the following field is valid
      int                  nNumItems;     // number of services/protocols
      BTProtocol           dProtocol[5];  // RFCOMM, L2CAP, ...
      BTServiceClassUUID   dSvcCls[5];    // Service Class ID list
      uint32               uSvrChnl[5];   // server channel numbers
      uint32               uPSM[5];       // L2CAP PSM numbers
   
   }  BTServiceIdentifier;

Members:
   dIDMethod            : which of the following field is valid
   nNumItems            : number of valid items in list
   dProtocol            : list of protocols (RFCOMM, L2CAP, ...)
   dSvcCls              : list of Service Class UUID
   uSvrChnl             : list of server channel numbers
   uPSM                 : list of L2CAP PSM numbers

=============================================================================*/

/*=============================================================================
INTERFACE DOCUMENTATION
===============================================================================
IBTMgr Interface

Description:
   This interface provides methods to control the underlying Bluetooth driver.
   User of this interface must have system privilege.

===H2>
   Usage example:
===/H2>

   ===== To create an IBTMgr object, see instruction in class header file

   ===== To register Event signal
   // first, create pMe->piMgrEvSignal, then register:
   IBTMgr_OnEventNotify (pMe->piBTMgr, pMe->piMgrEvSignal);


   ===== To retrieve event:
   BTResult dResult;
   while (IBTMgr_GetEvent (pMe->piBTMgr, &dEvent, &dResult) == AEE_SUCCESS)
   {
      switch (dEvent)
      {
         // handle events
      }
   }
   ISignalCtl_Enable (pMe->piMgrEvSignalCtl); // re-enable signal


   ===== To retrieve the current state of local device radio:
   IBTMgr_GetRadioState (pMe->piBTMgr, &pdRadioState);


   ===== To retrieve the current settings of local device:
   BTLocalSettingBitmap    dWhatsValid;
   BTLocalSettings         localSetting;
   IBTMgr_GetLocalSettings (pMe->piBTMgr, &dWhatsValid, &localSetting);


   ===== To set Bluetooth name for local device:
   uint8 myBTName[] = "RedTooth";
   IBTMgr_SetName (pMe->piBTMgr, myBTName);


   ===== To set class of device for your phone:
   BTClassOfDevice myCoD = {
     AEEBT_COS_TELEPHONY, AEEBT_DC_PHONE, AEEBT_SDC_PHONE_CELLULAR};
   IBTMgr_SetCoD (pMe->piBTMgr, myCoD);


   ===== To set prefer connection role for local device to MASTER:
   IBTMgr_SetPrefConnRole (pMe->piBTMgr, TRUE);


   ===== To allow role switch request from remote devices:
   IBTMgr_SetRoleSwitchPolicy (pMe->piBTMgr, TRUE);


   ===== To set minimum security level in all future connections 
   ===== regardless of services:
   IBTMgr_SetSecurity (pMe->piBTMgr, AEEBT_SEC_AUTHENTICATE);


   ===== To supply underlying Bluetooth driver with a list of devices
   ===== whose connection requests should be automatically rejected:
   IBTMgr_SetBlockedDevList (pMe->piBTMgr, nNumBadGuys, &badGuysAddrList);


   ===== To register for connection request:
   // first, create pMe->piMgrConnSignal, then register:
   IBTMgr_OnConnRequest (pMe->piBTMgr, pMe->piMgrConnSignal);


   ===== To protect local device when providing Dial-Up Networking
   ===== and Serial Port Profile services:
   BTServiceIdentifier  svc2protect;
   svc2protect.dIDMethod  = AEEBT_SEC_BY_SERVICE_CLASS;
   svc2protect.nNumItems  = 2;
   svc2protect.dSvcCls[0] = AEEBT_SCU_DIALUP_NETWORKING;
   svc2protect.dSvcCls[1] = AEEBT_SCU_SERIAL_PORT;
   IBTMgr_SetSvcSecurity (pMe->piBTMgr, AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT,
                          &svc2protect);


   ===== To get connection request information:
   boolean bAccept = FALSE;
   BTServiceIdentifier  requestedSvc;
   IBTMgr_GetConnRequest (pMe->piBTMgr, &remoteDev, &requestedSvc);
   if (MyBlockedDeviceCheck (remoteDev) == FALSE)
   {
      // show user "<remote name> is connecting to use <service>. Allow?"
      // set bAccept to user's response
   }
   IBTMgr_AuthorizeConnReq (pMe->piBTMgr, &remoteDev, &requestedSvc, bAccept);
   ISignalCtl_Enable (pMe->piMgrConnSignalCtl); // re-enable signal


   ===== To tune scan performance:
   IBTMgr_TuneScanPerformance (
     pMe->piBTMgr, 
     AEEBT_AVP_SMALLEST_ACT_MOST_POWER, // faster connection
     AEEBT_AVP_AUTOMATIC); // let Bluetooth driver decide on inquiry scan perf

===============================================================================
IBTMgr_AddRef

Description:
   Inherited from IQI_AddRef().

See Also:
   IBTMgr_Release()
   
===============================================================================
IBTMgr_Release

Description:
   Inherited from IQI_Release().

See Also:
   IBTMgr_AddRef()
   
===============================================================================
IBTMgr_QueryInterface

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IBTMgr-derived class

Prototype:
   int IBTMgr_QueryInterface (IBTMgr*  po, 
                              AEECLSID   clsReq, 
                              void**     ppo);

Parameters:
   po [i]     : the object
   clsReq [i] : Class ID of the interface to query.
   ppo [o]    : Pointer to the interface.

Return Value:
   AEE_SUCCESS: on success, 
   Otherwise: an error code.

Comments:
   If ppo is back a NULL pointer, the queried interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.

===============================================================================
IBTMgr_OnEventNotify()
   
Description:
   This method registers a signal to be triggered when the object has one 
   or more events for application.

Prototype:
   int IBTMgr_OnEventNotify (IBTMgr*    po,
                             ISignal*   piSignal);

Parameters:
   po [in]         : the object
   piSig [in]      : signal associated with the callback function

Return values:
   AEE_SUCCESS     : signal has been scheduled
   AEE_EBADPARM    : input pointers are invalid

See Also:
   ISignal

===============================================================================
IBTMgr_GetEvent()
   
Description:
   This method retrieves BTMgr events.  This API should be called when the 
   signal registered via IBTMgr_OnEventNotify() is triggered.

Prototype:
   int IBTMgr_GetEvent(IBTMgr*          po,
                       BTMgrEventType*  pEventID,
                       BTResult*        pdResult); 

Parameters:
   po [i]          : the object
   pEventID [o]    : one of the event IDs
   pdResult [o]    : only valid for event representing failure operation

Return values:
   AEE_SUCCESS     : event data has been successfully retrieved
   AEE_EBADPARM    : input parameters are invalid
   AEE_ENOMORE     : no event to retrieve

See Also:
   ISignal

===============================================================================
IBTMgr_GetRadioState()
   
Description:
   This method retrieves the local Bluetooth device's current radio state.

Prototype:
   int IBTMgr_GetRadioState (IBTMgr*        po,
                             BTRadioState*  pdRadioState); 

Parameters:
   po [i]          : the object
   pdRadioState [o]: local radio state

Return values:
   AEE_SUCCESS     : radio state has been successfully retrieved
   AEE_EBADPARM    : input parameters are invalid

See Also:
   ISignal

===============================================================================
IBTMgr_GetLocalSettings()
   
Description:
   This method retrieves the local Bluetooth device's current settings.

Prototype:
   int IBTMgr_GetLocalSettings (IBTMgr*                po,
                                BTLocalSettingBitmap*  pdWhatsValid,
                                BTLocalSettings*       pSettings); 

Parameters:
   po [i]          : the object
   pdWhatsValid [o]: bitmask of the valid fields
   pSettings [o]   : current local setttings

Return values:
   AEE_SUCCESS     : current local settings has been successfully retrieved
   AEE_EBADPARM    : input pointers are invalid

See Also:
   ISignal

===============================================================================
IBTMgr_SetName()
   
Description:
   This method sets the Bluetooth name of local device.

   Successful operation would generate AEEBTMGR_EV_SET_NAME_SUCCESS
   and may cause AEEBTMGR_EV_SETTINGS_CHANGED to be generated.

Prototype:
   int IBTMgr_SetName(IBTMgr*       po, 
                      const uint8*  pu8Name);

Parameters:
   po [i]          : the object
   pu8Name [i]     : local device name in UTF-8

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EBADPARM    : input parameters are invalid
   AEE_EALREADY    : the previous SetName operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

===============================================================================
IBTMgr_SetCoD()
   
Description:
   This method sets the Bluetooth Class of Device (CoD) for the local device.

   Successful operation would generate AEEBTMGR_EV_SET_COD_SUCCESS
   and may cause AEEBTMGR_EV_SETTINGS_CHANGED to be generated.
   
Prototype:
   int IBTMgr_SetCoD (IBTMgr*                po,
                      const BTClassOfDevice* pCoD);

Parameters:
   po [in]         : the object
   pCoD [in]       : the Class ofDevice

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EBADPARM    : input parameters are invalid
   AEE_EALREADY    : the previous SetCoD operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

See Also:
   BTClassOfDevice

===============================================================================
IBTMgr_TuneScanPerformance()
   
Description:
   This method sets the performance parameters used in page scanning and
   inquiry scanning.  The shorter the connection time (or faster connection), 
   the more the power would be consumed.  New settings will not take effect 
   until the local radio is power cycled which can be achieved via 
   IBTMgr_DisableRadio() and IBTMgr_EnableRadio().

   Successful operation would generate AEEBTMGR_EV_PERF_TUNING_SUCCESS
   and may cause AEEBTMGR_EV_SETTINGS_CHANGED to be generated.
   
Prototype:
   int IBTMgr_TuneScanPerformance (IBTMgr*       po,
                                   BTActVsPwr    dPgScanParam,
                                   BTActVsPwr    dInqScanParam);

Parameters:
   po [i]             : the object
   dPgScanParam [in]  : performance parameter for page scanning
   dInqScanParam [in] : performance parameter for inquiry scanning

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EBADPARM    : input parameters are invalid
   AEE_EALREADY    : the previous TuneScanPerformance operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

===============================================================================
IBTMgr_SetPrefConnRole()
   
Description:
   By Bluetooth definition, the device initiating a connection is the master
   of that connection.  Either device can try to switch to its preferred
   role, which may or may not succeed.

   This method sets the preferred connection role for the local device in
   future connections initiated by remote devices.

   If Master is the preferred role, local device will attempt a role switch.
   If Slave is preferred, local device will remain the slave of the connection.

   Successful operation would generate AEEBTMGR_EV_SET_ROLE_SUCCESS
   and may cause AEEBTMGR_EV_SETTINGS_CHANGED to be generated.

Prototype:
   int IBTMgr_SetPrefConnRole (IBTMgr*  po,
                               boolean  bMaster);

Parameters:
   po [in]         : the object
   bMaster [in]    : TRUE if local device prefers to be master in connection;
                     FALSE if it prefers to be slave

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EALREADY    : the previous SetPrefConnRole operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

===============================================================================
IBTMgr_SetSecurity()
   
Description:
   This method sets the minimum level of security that should be applied in
   all future connections regardless of services.  

   Successful operation would generate AEEBTMGR_EV_SET_SEC_SUCCESS
   and may cause AEEBTMGR_EV_SETTINGS_CHANGED to be generated.

Prototype:
   int IBTMgr_SetSecurity (IBTMgr*  po, BTSecurityLevel dSec);

Parameters:
   po [in]         : the object
   dSec [in]       : the minimum security level to be used on all connections

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EALREADY    : the previous SetSecurity operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

===============================================================================
IBTMgr_SetRoleSwitchPolicy()
   
Description:
   This method sets the role switch policy.

   If role switch is allowed, role switch request by remote device will 
   be granted.

   Successful operation would generate AEEBTMGR_EV_SET_SW_POL_SUCCESS
   and may cause AEEBTMGR_EV_SETTINGS_CHANGED to be generated.

Prototype:
   int IBTMgr_SetRoleSwitchPolicy (IBTMgr*  po,
                                   boolean  bAllow);

Parameters:
   po [in]         : the object
   bAllow [in]     : TRUE if role switch should be allowed; FALSE if disallowed

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EALREADY    : the previous SetRoleSwitchPolicy operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

===============================================================================
IBTMgr_SetBlockedDevList()
   
Description:
   This method provides the underlying Bluetooth driver with a list of
   BD addresses of devices to be blocked.  Incoming connection requests
   from these devices will always be rejected.  This list is not retained
   by the Bluetooth driver across power cycles.

   Successful operation would generate AEEBTMGR_EV_SET_BLOCKED_DEVS_SUCCESS
   and may cause AEEBTMGR_EV_SETTINGS_CHANGED to be generated.

Prototype:
   int IBTMgr_SetBlockedDevList (IBTMgr*            po,
                                 int                nNumDevs,
                                 const BDAddress*   pAddrList);

Parameters:
   po [i]          : the object
   nNumDevs [i]    : number of address in list; 0 to remove existing list
   pAddrList [i]   : addresses of devices to be blocked

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EBADPARM    : input parameters are invalid
   AEE_EALREADY    : the previous SetBlockedDevList operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

===============================================================================
IBTMgr_SetSvcSecurity()
   
Description:
   This method sets the minimum security level for a service or group of
   services.

   The service of interest can be specified in several ways: by its service
   class UUID (i.e. AEEBT_SCU_SERIAL_PORT), by the RFCOMM server channel number,
   or by L2CAP PSM number.  If security is to be enforced on all services over
   a particular protocol, the protocol UUID should be specified.

   To remove the security setting done by a previous IBTMgr_SetSvcSecurity() 
   on the same service, the application needs to call this API with 
   dSecLevel==AEEBT_SEC_NONE.

   If the application wishes to be notified when a remote device tries to
   connect to use the specified service(s), it should register a signal via
   IBTMgr_OnConnRequest().  If it does not, connection will be accepted 
   automatically.  Whether connection request comes to the application or 
   accepted automatically, dSecLevel will be applied to the connection.
   
   Successful operation would generate AEEBTMGR_EV_SET_SVC_SEC_SUCCESS.

Prototype:
   int IBTMgr_SetSvcSecurity (IBTMgr*                    po,
                              BTSecurityLevel            dSecLevel,
                              const BTServiceIdentifier* pSvcID);

Parameters:
   po [i]          : the object
   dSecLevel [i]   : security level
   pSvcID [i]      : service identifier

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EBADPARM    : input pointers are invalid
   AEE_EALREADY    : the previous SetSvcSecurity operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress
   AEE_EUNSUPPORTED: service ID method not supported

See Also:
   BTServiceSecurity
   IBTMgr_AuthorizeConnReq()
   IBTMgr_OnConnRequest()

===============================================================================
IBTMgr_OnConnRequest()

Description:
   This method allows application to register to be notified when a remote 
   device tries to bring up a connection to use a service set to be protected
   via IBTMgr_SetSvcSecurity().

   If application does not call this API, incoming connections will undergo
   security check and, if passed, get accepted automatically.
   
Prototype:
   int IBTMgr_OnConnRequest (IBTMgr*     po,
                             ISignal*    piSignal);

Parameters:
   po [i]          : the object
   piSignal [i]    : signal associated with function to be invoked when 
                       user authorization is required

Return values:
   AEE_SUCCESS     : signal has been scheduled
   AEE_EBADPARM    : input pointers are invalid

See Also:
   IBTMgr_SetSvcSecurity()
   IBTMgr_AuthorizeConnReq()

===============================================================================
IBTMgr_GetConnRequest()
   
Description:
   When the signal registered via IBTMgr_OnConnRequest() is triggered, this 
   method should be call to retrieve the request information.  Application
   should then call IBTMgr_AuthorizeConnReq() to response to the request.

Prototype:
   int IBTMgr_GetConnRequest (IBTMgr*               po,
                              BDAddress*            pAddr,
                              BTServiceIdentifier*  pdSvcIDfier);

Parameters:
   po [i]          : the object
   pAddr [o]       : address of requesting device
   pdSvcIDfier [o] : requested service

Return values:
   AEE_SUCCESS     : request information successfully retrieved
   AEE_EBADPARM    : input parameters are invalid
   AEE_ENOSUCH     : no existing connection request

See Also:
   IBTMgr_SetSvcSecurity()
   IBTMgr_OnConnRequest()
   IBTMgr_AuthorizeConnReq()

===============================================================================
IBTMgr_AuthorizeConnReq()
   
Description:
   This method accepts or rejects connection request from the given remote
   device which is attempting to use a service on the given protocol.
   
Prototype:
   int IBTMgr_AuthorizeConnReq (IBTMgr*                     po,
                                const BDAddress*            pBDAddr,
                                const BTServiceIdentifier*  psSvcIDfier,
                                boolean                     bAuthorize);

   Successful operation would generate AEEBTMGR_EV_AUTH_CONN_SUCCESS.
   
Parameters:
   po [i]          : the object
   pBDAddr [i]     : address of requesting device
   psSvcIDfier [i] : requested service
   bAuthorize [i]  : TRUE to accept the request; FALSE to reject

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EBADPARM    : input parameters are invalid
   AEE_EALREADY    : the previous AuthorizeConnReq operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

===============================================================================
IBTMgr_DisableRadio()
   
Description:
   This method causes Bluetooth radio to be powered off.  As a result, existing 
   connections will be torn down and pending operations will be abandoned.
   Depending on the underlying Bluetooth driver implementation, application 
   may or may not receive any indication of disconnection or of failed/abandoned
   operations.

   Successful operation would generate AEEBTMGR_EV_DISABLE_RADIO_SUCCESS.
   If radio gets powered off, AEEBTMGR_EV_RADIO_OFF would get generated.
   
Prototype:
   int IBTMgr_DisableRadio (IBTMgr*  po);

Parameters:
   po [i]          : the object

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EALREADY    : the previous DisableRadio operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

See Also:
   IBTMgr_EnableRadio()

===============================================================================
IBTMgr_EnableRadio()
   
Description:
   This method causes Bluetooth radio to be powered on.  Depending on the
   underlying Bluetooth driver implementation, the local device may or may not
   resume its previous settings.

   Successful operation would generate AEEBTMGR_EV_ENABLE_RADIO_SUCCESS.
   If radio gets powered on, AEEBTMGR_EV_RADIO_ON would get generated.

Prototype:
   int IBTMgr_EnableRadio (IBTMgr*  po);

Parameters:
   po [i]          : the object

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EALREADY    : the previous EnableRadio operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

See Also:
   IBTMgr_DisableRadio()

===============================================================================
IBTMgr_DisableVisibility()
   
Description:
   When one or more Bluetooth service are enabled, the Bluetooth radio on 
   local device maybe set to be detectable by remote devices.  This method 
   causes Bluetooth radio to be undetectable by remote devices.

   Successful operation would generate AEEBTMGR_EV_DISABLE_VISIBILITY_SUCCESS.
   If radio becomes undetectable, AEEBTMGR_EV_VISIBILITY_OFF would get 
   generated.
   
Prototype:
   int IBTMgr_DisableVisibility (IBTMgr*  po);

Parameters:
   po [i]          : the object

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EALREADY    : the previous DisableVisibility operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

See Also:
   IBTMgr_RestoreVisibility()

===============================================================================
IBTMgr_RestoreVisibility()
   
Description:
   This method returns the local device's Bluetooth radio to its settings
   that was previously overriden by IBTMgr_DisableVisibility().

   Successful operation would generate AEEBTMGR_EV_RESTORE_VISIBILITY_SUCCESS.
   If radio becomes detectable, AEEBTMGR_EV_VISIBILITY_ON would get generated.
   
Prototype:
   int IBTMgr_RestoreVisibility (IBTMgr*  po);

Parameters:
   po [i]          : the object

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EALREADY    : the previous RestoreVisibility operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

See Also:
   IBTMgr_DisableVisibility()

==========================================================================*/

#endif // AEEIBTMGR_H

