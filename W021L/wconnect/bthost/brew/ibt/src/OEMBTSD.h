#ifndef OEMBTSD_H
#define OEMBTSD_H
/*=============================================================================
FILE:         OEMBTSD.h

SERVICES:     IBTServiceDiscovery Interfaces

DESCRIPTION:  The OEM implementation of IBTServiceDiscovery

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBTSD.h#1 $ 
$DateTime: 2009/01/07 18:14:54 $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2008-10-13  pn   Changed OEMBTSDEvent to have BT events processed in BREW task.

=============================================================================*/
#include "AEEShell.h"
#include "AEEIBTServiceDiscovery.h"

#define OEMBTSD_MAX_DISCOVERED_DEV          30
#define OEMBTSD_MAX_SERVICE_DB_ENTRIES      12

//----------------------------------------------------------------------------
// Event related definitions
#define OEMBTSD_EV_NONE                      0
#define OEMBTSD_EV_DEV_DISCOVERY_STARTED     1
#define OEMBTSD_EV_DEV_DISCOVERY_COMPLETED   2
#define OEMBTSD_EV_DEV_DISCOVERY_RESPONSE    3
#define OEMBTSD_EV_SVC_DISCOVERY_STARTED     4
#define OEMBTSD_EV_SVC_DISCOVERY_COMPLETED   5
#define OEMBTSD_EV_NAME_REQ_STARTED          6
#define OEMBTSD_EV_NAME_REQ_COMPLETED        7
#define OEMBTSD_EV_SVC_REG_DONE              8
#define OEMBTSD_EV_SVC_DEREG_DONE            9
#define OEMBTSD_EV_SET_DISC_MODE_DONE       10
#define OEMBTSD_EV_MAX                    0xFF

typedef uint32 OEMBTSDEventType;

/*============================================================================
  Structures
============================================================================*/
typedef struct _OEMBTSD OEMBTSD;

typedef struct _OEMBTSDEvent OEMBTSDEvent;
struct _OEMBTSDEvent
{
   boolean                 bInUse;
   OEMBTSDEventType        dType;
   BTResult                dResult;
   BTDeviceInfo            devInfo;
   BTSvcDiscoverableMode   dMode;
   char*                   pszXML;  // freed by AEE layer
   uint32                  uVal;    // generic info
   AEECallback             cb;
};

typedef void (SDEvCallbackFn) (IBTServiceDiscovery*, OEMBTSDEvent*);

/*============================================================================
  OEM APIs exported to the AEE layer   
============================================================================*/
extern void OEMBTSD_Release (OEMBTSD*, uint32*, int);
extern int  OEMBTSD_New (IShell*, OEMBTSD**);
extern void OEMBTSD_DeregisterEvCb (OEMBTSD*, IBTServiceDiscovery*);
extern int  OEMBTSD_RegisterEvCb (OEMBTSD*, SDEvCallbackFn*, 
                                  IBTServiceDiscovery*);
extern int  OEMBTSD_StartDevDiscovery (OEMBTSD*, BTClassOfService, int, int);
extern int  OEMBTSD_StopDevDiscovery (OEMBTSD*);
extern int  OEMBTSD_GetName (OEMBTSD*, const BDAddress*);
extern int  OEMBTSD_GetNameCancel (OEMBTSD*, const BDAddress*);
extern int  OEMBTSD_GetLocalDBRecCount (OEMBTSD*);
extern int  OEMBTSD_GetLocalDBSummary (OEMBTSD*, BTServiceRecordSummary*, int);
extern int  OEMBTSD_ReadRec (OEMBTSD*, uint32, char**);
extern int  OEMBTSD_WriteRec (OEMBTSD*, uint32, const char*);
extern int  OEMBTSD_GetDiscoverableMode (OEMBTSD*, BTSvcDiscoverableMode*);
extern int  OEMBTSD_SetDiscoverableMode (OEMBTSD*, BTSvcDiscoverableMode);
extern int  OEMBTSD_StartSvcDiscovery (OEMBTSD*, const BDAddress*,
                                       const BTSvcSearchPattern*);
extern int  OEMBTSD_StopSvcDiscovery (OEMBTSD*, const BDAddress*);
extern int  OEMBTSD_RegisterService (OEMBTSD*, const char*, boolean);
extern int  OEMBTSD_DeregisterService (OEMBTSD*, uint32);

#endif /* OEMBTSD_H */

