#ifndef OEMBTAVRCPTGEXTDEF_H
#define OEMBTAVRCPTGEXTDEF_H
/*=============================================================================
FILE:  OEMBTAVRCPTGExt.h

SERVICES:  Exports functions/ data types that are invoked by the 
           AEE layer 

GENERAL DESCRIPTION:
      


===============================================================================
        Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEAVRCPTG_priv_ext_def.h"
#include "AEEAVRCPTG_Priv_Ext.h"
#include "btcomdef.h"

//=============================================================================
//  Type definitions
//============================================================================= 

/* For BT events handled by the AVRCP interface */
/* AEE layer must copy data from the pointer into local memory */
typedef void (*OEMBTAVRCPTG_ExtCmdCb) (void *pUser, AEEBTAVRCPTgExtCmdInfo *pCmdInfo);

typedef struct OEMBTAVRCPTGExt OEMBTAVRCPTGExt;


/* =================================================================================

                             AVRCP TG EXT OEM function Calls          

====================================================================================*/
int OEMBTAVRCPTGExt_Init(OEMBTAVRCPTGExt **pMeExt, 
                         void* cbData, 
                         OEMBTAVRCPTG_ExtCmdCb aeeEvtCb);

int OEMBTAVRCPTGExt_Release(OEMBTAVRCPTGExt *pMeExt);


int OEMBTAVRCPTGExt_SendEventIdReply(OEMBTAVRCPTGExt            *pMeExt,
                                     uint8                      txn,
                                     uint32                     eventCount,
                                     AEEBTAVRCPTgMediaEventMask *eventIds);



int OEMBTAVRCPTGExt_SendCompIdReply( OEMBTAVRCPTGExt *pMeExt,
                                     uint8            txn,
                                     uint32            compCount,
                                     uint32           *comp );


int OEMBTAVRCPTGExt_ListAttrIdReply(OEMBTAVRCPTGExt             *pMeExt,
                                    uint8                       txn,
                                    uint32                      attrCount,
                                    AEEBTAVRCPTgPlayerAppAttrs  *attrIds);


int OEMBTAVRCPTGExt_ListAttrValReply(OEMBTAVRCPTGExt             *pMeExt,
                                     uint8                       txn,
                                     AEEBTAVRCPTgPlayerAppAttrs  attrId,
                                     uint32                       attrValCount,
                                     uint32                      *attrValue);

int OEMBTAVRCPTGExt_GetAttrValReply(OEMBTAVRCPTGExt             *pMeExt,
                                    uint8                       txn,
                                    uint32                       attrCount,
                                    uint32                      *attrIds);

int OEMBTAVRCPTGExt_GetAttrSettingTxtReply(OEMBTAVRCPTGExt            *pMeExt,
                                           uint8                      txn,
                                           uint32                     attrCount,
                                           AEEBTAVRCPTgItemTxt       *attrIdTxt);

int OEMBTAVRCPTGExt_GetAttrSettingValTxtReply(OEMBTAVRCPTGExt            *pMeExt,
                                              uint8                      txn,
                                              uint32                     valCount,
                                              AEEBTAVRCPTgItemTxt       *attrValTxt);

int OEMBTAVRCPTGExt_SendPlayStatusReply(OEMBTAVRCPTGExt            *pMeExt,
                                        uint8                      txn,
                                        uint32                     songLen,
                                        uint32                     songPos,
                                        AEEBTAVRCPTgPlayStatus     playStatus);

int OEMBTAVRCPTGExt_SendGetElementAttrReply(OEMBTAVRCPTGExt            *pMeExt,
                                            uint8                       txn,
                                            uint32                      mediaAttrCount,
                                            AEEBTAVRCPTgItemTxt        *mediaAttr);

int OEMBTAVRCPTGExt_SendRegNotifReply(OEMBTAVRCPTGExt            *pMeExt,
                                      uint8                       txn,
                                      AEEBTAVRCPTgMediaEventMask  eventId,
                                      AEEBTAVRCPTgPlaybackInfo    *playInfo,
                                      AEEBTAVRCPTgContBattStatus   battStat,
                                      AEEBTAVRCPTgSystemStatus     sysStat,
                                      boolean                      isFinal,
                                      uint32                       *attrs,
                                      uint32                       attrCount);

int OEMBTAVRCPTGExt_SendGroupNavReply(OEMBTAVRCPTGExt            *pMeExt,
                                      uint8                      txn,
                                      uint32                     vendorUniqueId,
                                      boolean                    buttonPress,
                                      AEEBTAVRCPTgCmdResponseStatus respCode);

int OEMBTAVRCPTGExt_SendSimpleMetaReply(OEMBTAVRCPTGExt                *pMeExt,
                                        uint8                           txn,
                                        AEEBTAVRCPTgPduId               pduId, 
                                        AEEBTAVRCPTgCmdResponseStatus   status );

#endif /* OEMBTAVRCPTGEXTDEF_H */

