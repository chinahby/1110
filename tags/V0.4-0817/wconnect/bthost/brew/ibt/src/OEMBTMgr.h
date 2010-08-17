#ifndef OEMBTMGR_H
#define OEMBTMGR_H
/*=============================================================================
FILE:         OEMBTMgr.h

SERVICES:     IBTMgr Interfaces

DESCRIPTION:  The OEM implementation of IBTMgr

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEShell.h"
#include "AEEIBTMgr.h"
#include "AEEIBTConnection.h"

/*============================================================================
  Macros and other definitions
============================================================================*/
#define OEMBTMGR_MAX_CONNECTIONS              3
#define OEMBTMGR_MAX_BLOCKED_DEVICES         20

//----------------------------------------------------------------------------
// Event related definitions
#define OEMBTMGR_EV_NONE                      0
#define OEMBTMGR_EV_SET_NAME_DONE             1
#define OEMBTMGR_EV_SET_COD_DONE              2
#define OEMBTMGR_EV_SET_SEC_DONE              3
#define OEMBTMGR_EV_SET_ROLE_DONE             4
#define OEMBTMGR_EV_SET_SW_POL_DONE           5
#define OEMBTMGR_EV_SET_BLOCKED_DEVS_DONE     6
#define OEMBTMGR_EV_SET_SVC_SEC_DONE          7
#define OEMBTMGR_EV_AUTH_CONN_DONE            8
#define OEMBTMGR_EV_PERF_TUNING_DONE          9
#define OEMBTMGR_EV_DISABLE_RADIO_DONE       10
#define OEMBTMGR_EV_ENABLE_RADIO_DONE        11
#define OEMBTMGR_EV_DISABLE_VIS_DONE         12
#define OEMBTMGR_EV_RESTORE_VIS_DONE         13
#define OEMBTMGR_EV_RADIO_OFF                14
#define OEMBTMGR_EV_RADIO_ON                 15
#define OEMBTMGR_EV_VIS_OFF                  16
#define OEMBTMGR_EV_VIS_ON                   17
#define OEMBTMGR_EV_SETTINGS_CHANGED         18
#define OEMBTMGR_EV_AUTH_REQ                 19
#define OEMBTMGR_EV_SET_CONN_SEC_DONE        20
#define OEMBTMGR_EV_SET_CONN_ROLE_DONE       21
#define OEMBTMGR_EV_SET_CONN_SW_POL_DONE     22
#define OEMBTMGR_EV_SET_CONN_LPM_DONE        23
#define OEMBTMGR_EV_CONN_STAT_CHANGED        24
#define OEMBTMGR_EV_MAX                    0xFF
typedef uint32 OEMBTMgrEventType;

//----------------------------------------------------------------------------
// Radio control command
#define OEMBTMGR_SET_RADIO_ON                1
#define OEMBTMGR_SET_RADIO_OFF               2
#define OEMBTMGR_SET_RADIO_UNDISCOVERABLE    3
#define OEMBTMGR_RESTORE_DISCOVERABILITY     4
typedef uint32 OEMBTMgrRadioControlCommand;

/*============================================================================
  Structures
============================================================================*/
typedef struct _OEMBTMgr OEMBTMgr;

typedef struct _OEMBTMgrEvent OEMBTMgrEvent;
struct _OEMBTMgrEvent
{
   OEMBTMgrEvent*       pNext;
   OEMBTMgrEventType    dType;
   BTResult             dResult;
   BDAddress            bdAddr;
};

typedef void (ConnEvCallbackFn) (IBTConnection*, OEMBTMgrEvent*);
typedef void (MgrEvCallbackFn) (IBTMgr*, OEMBTMgrEvent*);

/*============================================================================
  OEM APIs exported to the AEE layer   
============================================================================*/
extern void OEMBTMgr_Release (OEMBTMgr*);
extern int  OEMBTMgr_New (IShell*, OEMBTMgr**);
extern int  OEMBTMgr_RegisterEventCb (OEMBTMgr*, MgrEvCallbackFn*, IBTMgr*);
extern void OEMBTMgr_DeregisterEventCb (OEMBTMgr*, IBTMgr*);
extern int  OEMBTMgr_GetNextEvent (OEMBTMgr*, ISignal*, OEMBTMgrEvent*);
extern void OEMBTMgr_GetLocalSettings (OEMBTMgr*, BTLocalSettings*);
extern int  OEMBTMgr_SetName (OEMBTMgr*, const uint8*);
extern int  OEMBTMgr_SetCoD (OEMBTMgr*, const BTClassOfDevice*);
extern int  OEMBTMgr_SetSecurity (OEMBTMgr*, const BDAddress*, BTSecurityLevel);
extern int  OEMBTMgr_SetPrefConnRole (OEMBTMgr*, const BDAddress*, boolean);
extern int  OEMBTMgr_SetRoleSwitchPolicy (OEMBTMgr*, const BDAddress*,boolean);
extern int  OEMBTMgr_SetSvcSecurity (OEMBTMgr*, 
                                     BTSecurityLevel, 
                                     const BTServiceIdentifier*);
extern int  OEMBTMgr_SetLowPowerConfig (OEMBTMgr*,
                                        const BDAddress*, 
                                        BTLowPowerMode,
                                        uint16, 
                                        uint16);
extern int  OEMBTMgr_SetBlockedDevList (OEMBTMgr*, int, const BDAddress*);
extern int  OEMBTMgr_AuthorizeConn (OEMBTMgr*, 
                                    const BDAddress*,
                                    const BTServiceIdentifier*,
                                    boolean);
extern int  OEMBTMgr_TuneScanPerformance (OEMBTMgr*, BTActVsPwr, BTActVsPwr);
extern int  OEMBTMgr_RadioControl (OEMBTMgr*, OEMBTMgrRadioControlCommand);
extern int  OEMBTMgr_ConnRequestRegister (OEMBTMgr*, ISignal*);
extern int  OEMBTMgr_ConnRequestDeregister (OEMBTMgr*);
extern int  OEMBTMgr_GetConnRequest (OEMBTMgr*, 
                                     BDAddress*, 
                                     BTServiceIdentifier*);
extern int  OEMBTMgr_RegisterConnEv (OEMBTMgr*, 
                                     ConnEvCallbackFn*, 
                                     IBTConnection*,
                                     const BDAddress*);
extern void OEMBTMgr_DeregisterConnEv (OEMBTMgr*, IBTConnection*);
extern int  OEMBTMgr_GetConnStatus (OEMBTMgr*, BTConnectionStatus*, boolean);
extern int  OEMBTMgr_GetRadioState (OEMBTMgr* pMe, BTRadioState* pRadioState);
#endif /* OEMBTMGR_H */

