#ifdef FEATURE_BT_EXTPF_CTP
/** 
@file  
helper functions for TCS protocol
*/

/** \addtogroup Telephony Telephony APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef _TCS_HELPER_H
#define _TCS_HELPER_H

#include "oi_stddefs.h"

OI_STATUS OI_Tcs_StartTimer(TCS_CALL_INFO *call, OI_UINT timerType);
OI_STATUS OI_Tcs_CancelTimer(TCS_CALL_INFO *call);
OI_STATUS OI_Tcs_CopyNumber(OI_TCS_NUMBER *dest, OI_TCS_NUMBER *src);
OI_STATUS OI_Tcs_SetupCallInfo(TCS_CALL_INFO *call, OI_TCS_CONNECT_INFO *info);
OI_STATUS OI_Tcs_ReleaseCallInfo(TCS_CALL_INFO *call);

#endif

/**@}*/
#endif /* FEATURE_BT_EXTPF_CTP */
