#ifndef OEMBTUTILS_H
#define OEMBTUTILS_H
/*=============================================================================
FILE:         OEMBTUtils.h

SERVICES:     IBT common utilities

DESCRIPTION:  This header file contains utility functions used by all IBT 
              interfaces.

===============================================================================
           Copyright (c) 2007-2008 QUALCOMM Incorporated.
                All Rights Reserved.
           Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBTUtils.h#1 $ 
$DateTime: 2009/01/07 18:14:54 $
=============================================================================*/
#include "AEEBTDef.h"
#include "bt.h"
#ifndef BT_SIMULATION
#include "snddev.h"
#endif

extern void OEMBT_BDAddrToStr(const BDAddress*, char*);
extern int OEMBT_StrToBDAddr (const char*, BDAddress*);
extern int OEMBT_CmdStatus2AEEResult (bt_cmd_status_type);
extern BTResult OEMBT_CmdStatus2Result (bt_cmd_status_type);
extern bt_service_class_enum_type OEMBT_SvcClsBrew2Core (BTClassOfService);
extern bt_cmd_status_type OEMBT_AEEResult2ObexStat(AEEResult result);
extern AEEResult OEMBT_SetServiceSecurity(bt_app_id_type,
                                          BTSecurityLevel,
                                          bt_sd_uuid_type);

#endif /* OEMBTUTILS_H */
