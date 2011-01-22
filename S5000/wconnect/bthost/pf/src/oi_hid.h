/**
@file
 * This file provides the API for specifying parameters for a HID connection.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#ifndef _OI_HID_H
#define _OI_HID_H

#include "oi_l2cap.h"
#include "oi_l2cap_qos.h"

/** \addtogroup HID HID APIs */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _OI_HID_CONNECTION_PARAMS OI_HID_CONNECTION_PARAMS;

/** This structure is used to send L2CAP connection and quality of service configuration 
 * parameters for the control and interrupt channels of a HID connection. Any of these may 
 * be set to NULL to indicate that default values should be used. Quality of service
 * guarantees are not currently supported in BLUEmagic 3.0 software but will be in a future
 * release of BLUEmagic software.
 */
struct _OI_HID_CONNECTION_PARAMS {
#ifndef OI_CODE
    OI_L2CAP_CONNECT_PARAMS  *controlParams;      /**< control channel connection parameters */
#else
#error code not present
#endif

    OI_L2CAP_FLOWSPEC const *controlQos;               /**< control channel quality of service parameters */

#ifndef OI_CODE
    OI_L2CAP_CONNECT_PARAMS *interruptParams;    /**< interrupt channel connection parameters */
#else
#error code not present
#endif

    OI_L2CAP_FLOWSPEC const *interruptQos;             /**< interrupt channel quality of service parameters */
};

#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* _OI_HID_H */
