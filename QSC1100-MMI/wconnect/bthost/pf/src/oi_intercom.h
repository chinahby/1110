#ifdef FEATURE_BT_EXTPF_CTP
#ifndef _OI_INTERCOM_H
#define _OI_INTERCOM_H
/**
 * @file  
 *
 * This file provides the interface for the Intercom Profile. This profile
 * provides a layer of functionality on top of the Telephony Control protocol
 * Specification - Binary (TCS Binary).
 */

/** \addtogroup Telephony Telephony APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_common.h"
#include "oi_status.h"
#include "oi_tcs.h"

/**
 * This function only registers a service record for the Intercom Profile.
 * This function should not be used directly by applications.
 *
 * @param strings  the name to register
 */
OI_STATUS OI_Intercom_RegisterServiceRecord(const OI_SDP_STRINGS *strings);

/**
 * Deregister the service record.
 */
OI_STATUS OI_Intercom_DeregisterServiceRecord(void);

#endif /* _OI_INTERCOM_H */

/**@}*/
#endif /* FEATURE_BT_EXTPF_CTP */
