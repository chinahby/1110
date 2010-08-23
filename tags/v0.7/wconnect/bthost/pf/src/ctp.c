#ifdef FEATURE_BT_EXTPF_CTP
/**
 * @file  
 * Cordless Telephony Profile connection functions
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/


#define __OI_MODULE__ OI_MODULE_CTP

#include "oi_stddefs.h"
#include "oi_bt_assigned_nos.h"
#include "oi_memmgr.h"
#include "oi_std_utils.h"
#include "oi_debug.h"
#include "oi_assert.h"

#include "oi_tcs.h"
#include "oi_sdpdb.h"
#include "oi_ctp.h"
#include "oi_tcs.h"

OI_STATUS OI_CTP_Connect(OI_BD_ADDR *addr)
{
    OI_DBGTRACE(("Trace: OI_CTP_Connect %:\n", addr));
    return OI_TCS_Connect(addr, OI_PSM_TCS_CORDLESS);
}

OI_STATUS OI_CTP_ConnectIntercom(OI_BD_ADDR *addr) 
{
    OI_DBGTRACE(("Trace: OI_CTP_ConnectIntercom %:\n", addr));
    return OI_TCS_Connect(addr, OI_PSM_TCS);
}
#endif /* FEATURE_BT_EXTPF_CTP */
