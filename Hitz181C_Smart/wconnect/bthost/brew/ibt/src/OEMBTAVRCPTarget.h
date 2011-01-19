#ifndef OEMBTAVRCPTARGET_H
#define OEMBTAVRCPTARGET_H

#include "AEEBTDef.h"
#include "AEEIBTAVRCPTarget.h"
#include "AEEIRemoteControl_Target.h"

typedef enum {
  OEMBT_AVRCP_RESPONSE_NOT_IMPLEMENTED = 0x8,
  OEMBT_AVRCP_RESPONSE_ACCEPTED        = 0x9,
  OEMBT_AVRCP_RESPONSE_REJECTED        = 0xa,
  OEMBT_AVRCP_RESPONSE_IN_TRANSITION   = 0xb,
  OEMBT_AVRCP_RESPONSE_IMPLEMENTED     = 0xc,
  OEMBT_AVRCP_RESPONSE_STABLE          = 0xc,
  OEMBT_AVRCP_RESPONSE_CHANGED         = 0xd
} OEMBTAVRCPRspType;

/* See AVRCP 1.0, section 4.7.5 to see which commands are
 * required to be supported by each category */
#define OEMBT_AVRCP_CATEGORY1 0x01 /* Player / Recorder */
#define OEMBT_AVRCP_CATEGORY2 0x02 /* Monitor / Amplifier */
#define OEMBT_AVRCP_CATEGORY3 0x04 /* Tuner */
#define OEMBT_AVRCP_CATEGORY4 0x08 /* Menu */
typedef uint8 OEMBTAVRCPCatType;


/* Type declaration for OEM structure */
struct OEMAVRCPTarget;
typedef struct OEMAVRCPTarget OEMAVRCPTarget;

/* For BT events handled by the AVRCP interface */
/* AEE layer must copy data from the pointer into local memory */
typedef void OEMBTAVRCP_EvCb(void *pUser, AEEBTAVRCPEventType *pEvent);

/* For commands send by remote CT, handled by the remote control interface */
/* AEE layer must copy data from the pointer into local memory */
/* AEE layer CB will immediately reply with result code to be sent to CT */
typedef OEMBTAVRCPRspType OEMBTAVRCP_CmdCb(void *pUser,
                                           AEERemoteControl_CommandType *pCmd );
typedef OEMBTAVRCPRspType OEMBTAVRCP_VendorCmdCb(void *pUser,
                                                 AEERemoteControl_VendorCommandType *pCmd );


extern AEEResult OEMBTAVRCP_EventCbDeregister (OEMAVRCPTarget* pMe);
extern AEEResult OEMBTAVRCP_EventCbRegister(OEMAVRCPTarget* pMe, OEMBTAVRCP_EvCb*, void* pUser);
extern AEEResult OEMBTAVRCP_Release (OEMAVRCPTarget* pMe);
extern AEEResult OEMBTAVRCP_New (IShell* pIShell, /*out*/OEMAVRCPTarget** pMe);
extern AEEResult OEMBTAVRCP_Enable( OEMAVRCPTarget* pMe );
extern AEEResult OEMBTAVRCP_Connect( OEMAVRCPTarget* pMe, const BDAddress* pBDAddr);
extern AEEResult OEMBTAVRCP_Disconnect( OEMAVRCPTarget* pMe, const BDAddress* pBDAddr);
extern AEEResult OEMBTAVRCP_Disable( OEMAVRCPTarget* pMe );

extern AEEResult OEMBTAVRCP_CmdCbDeregister (OEMAVRCPTarget* pMe);
extern AEEResult OEMBTAVRCP_CmdCbRegister(OEMAVRCPTarget* pMe, OEMBTAVRCP_CmdCb*, void* pUser);
extern AEEResult OEMBTAVRCP_VendorCmdCbDeregister (OEMAVRCPTarget* pMe);
extern AEEResult OEMBTAVRCP_VendorCmdCbRegister(OEMAVRCPTarget* pMe, OEMBTAVRCP_VendorCmdCb*, void* pUser);
extern AEEResult OEMBTAVRCP_SetCategory(OEMAVRCPTarget *pMe, OEMBTAVRCPCatType Category );

#endif /*  OEMBTAVRCPTARGET_H */
