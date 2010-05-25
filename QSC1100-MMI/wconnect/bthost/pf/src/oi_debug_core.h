#ifndef _OI_DEBUG_CORE_H
#define _OI_DEBUG_CORE_H
/** @file   
 *
 * Debug functions used by BM3 core modules, i.e. functions not needed or wanted
 * by BHAPI Client.  Core functions are isolated to prevent unnecessary 
 * dependencies in the BHAPI client.
 *
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_bt_spec.h"

/** \addtogroup Debugging Debugging APIs */
/**@{*/

/**
 * Utility functions.
 */

OI_CHAR *OI_LinkKeyTypeText(OI_BT_LINK_KEY_TYPE keyType);

OI_CHAR *OI_LinkTypeText(OI_UINT8 linkType);  /**< ACL or SCO? */

OI_CHAR *OI_LinkModeText(OI_UINT8 mode);  /**< active/sniff/park/hold? */

OI_CHAR *OI_HciDataTypeText(OI_UINT8 hciDataType) ; /**< h4 types */

/*****************************************************************************/
#endif /* _OI_DEBUG_CORE_H */

/**@}*/
