#ifdef FEATURE_BT_EXTPF_CTP
/**
 * @file  
 * Cordless Telephony Profile terminal role
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
#include "oi_init_flags.h"
#include "oi_devmgr.h"

#include "oi_tcs.h"
#include "oi_intercom.h"
#include "oi_sdpdb.h"
#include "oi_ctp.h"
#include "oi_ctp.h"

/**
 * This function handles registration common to both CTP TL and
 * Intercom.
 */
static OI_STATUS Register(const OI_SDP_STRINGS *strings)
{
    OI_STATUS result;

    //Initialize intercom
    result = OI_Intercom_RegisterServiceRecord(strings);
    if (!OI_SUCCESS(result)) {
        OI_TCS_Deregister();
        return result;
    }

    OI_DEVMGR_RegisterServiceClass(OI_SERVICE_CLASS_TELEPHONY);

    OI_INIT_FLAG_PUT_FLAG(TRUE, CTP);

    return OI_OK;
}

/**
 * This function initializes a Cordless Telephony Profile terminal.
 * 
 * @param strings    the name to advertise in the service record
 * @param callbacks  the callbacks to use
 */
OI_STATUS OI_CTP_TL_Register(const OI_SDP_STRINGS *strings,
                             const OI_TCS_CALLBACKS *callbacks)
{
    OI_STATUS result;

    if (OI_INIT_FLAG_VALUE(CTP)) {
        return OI_STATUS_ALREADY_INITIALIZED;
    }
    
    /* Register with TCS. We need to do this before we initialize Intercom so we get the correct role. */
    result = OI_TCS_Register(OI_TCS_ROLE_TERMINAL, FALSE, callbacks);
    if (!OI_SUCCESS(result)) {
        return result;
    }

    return Register(strings);
}

/**
 * This function initializes a Intercom Profile terminal.
 * 
 * @param strings    the name to advertise in the service record
 * @param callbacks  the callbacks to use
 */
OI_STATUS OI_CTP_TL_RegisterIntercom(const OI_SDP_STRINGS *strings,
                                     const OI_TCS_CALLBACKS *callbacks)
{
    OI_STATUS result;

    if (OI_INIT_FLAG_VALUE(CTP)) {
        return OI_STATUS_ALREADY_INITIALIZED;
    }
    
    /* Register with TCS. We need to do this before we initialize Intercom so we get the correct role. */
    result = OI_TCS_Register(OI_TCS_ROLE_TERMINAL, TRUE, callbacks);
    if (!OI_SUCCESS(result)) {
        return result;
    }

    return Register(strings);
}

OI_STATUS OI_CTP_TL_Deregister(void)
{
    OI_STATUS result;

    if (!OI_INIT_FLAG_VALUE(CTP)) {
        return OI_STATUS_NOT_INITIALIZED;
    }

    result = OI_TCS_Deregister();
    if (!OI_SUCCESS(result)) {
        return result;
    }
    OI_DEVMGR_DeregisterServiceClass(OI_SERVICE_CLASS_TELEPHONY);
    OI_Intercom_DeregisterServiceRecord();

    OI_INIT_FLAG_PUT_FLAG(FALSE, CTP);

    return OI_OK;
}
#endif /* FEATURE_BT_EXTPF_CTP */
