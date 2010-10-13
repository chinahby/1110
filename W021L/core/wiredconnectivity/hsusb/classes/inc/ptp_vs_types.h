/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef __PTP_VS_TYPES_H__
#define __PTP_VS_TYPES_H__

/*
 *   To add a PTP extension, perform the following:
 *   1. Add your xxx_types.h header file
 *   2. Add your relevant VS_EXT range macros
 */

#ifndef CONFIG_PROTO_PTP_VS_EXT

/* There are no extensions defined */

#define PTP_VS_EXT_OP_GROUP             PTP_VS_EXT_OP_DUMMY
#define PTP_VS_EXT_RESP_GROUP           PTP_VS_EXT_RESP_DUMMY
#define PTP_VS_EXT_OBJ_FORMAT_GROUP     PTP_VS_EXT_OBJ_FORMAT_DUMMY
#define PTP_VS_EXT_EVENT_GROUP          PTP_VS_EXT_EVENT_DUMMY
#define PTP_VS_EXT_DEV_PROP_GROUP       PTP_VS_EXT_DEV_PROP_DUMMY
#define PTP_VS_EXT_FMODE_GROUP          PTP_VS_EXT_FMODE_DUMMY
#define PTP_VS_EXT_PROTECT_GROUP        PTP_VS_EXT_PROTECT_DUMMY
#define PTP_VS_EXT_ASSOC_GROUP          PTP_VS_EXT_ASSOC_DUMMY
#define PTP_VS_EXT_STOR_GROUP           PTP_VS_EXT_STOR_DUMMY
#define PTP_VS_EXT_FS_GROUP             PTP_VS_EXT_FS_DUMMY
#define PTP_VS_EXT_ACCESS_GROUP         PTP_VS_EXT_ACCESS_DUMMY


#else

/* Extensions are available */
#include <mtp_vs_ext.h>

/*
 *   Each of the following GROUP mappings maps VendorSpecific
 *   extensions to PTP.
 *   To add your extension, add your extended RANGES to each
 *   relevant group.
 */

#define PTP_VS_EXT_OP_GROUP             \
    PTP_VS1_OP_RANGE

#define PTP_VS_EXT_RESP_GROUP           \
    PTP_VS1_RESP_RANGE

#define PTP_VS_EXT_OBJ_FORMAT_GROUP     \
    PTP_VS1_OBJ_FORMAT_RANGE

#define PTP_VS_EXT_EVENT_GROUP          \
    PTP_VS1_EVENT_RANGE

#define PTP_VS_EXT_DEV_PROP_GROUP       \
    PTP_VS1_DEV_PROP_RANGE

#define PTP_VS_EXT_FMODE_GROUP          \
    PTP_VS1_FMODE_RANGE

#define PTP_VS_EXT_PROTECT_GROUP        \
    PTP_VS1_PROTECT_RANGE

#define PTP_VS_EXT_ASSOC_GROUP          \
    PTP_VS1_ASSOC_RANGE

#define PTP_VS_EXT_STOR_GROUP           \
    PTP_VS1_STOR_RANGE

#define PTP_VS_EXT_FS_GROUP             \
    PTP_VS1_FS_RANGE

#define PTP_VS_EXT_ACCESS_GROUP         \
    PTP_VS1_ACCESS_RANGE

#endif

#endif /*__PTP_VS_TYPES_H__*/

