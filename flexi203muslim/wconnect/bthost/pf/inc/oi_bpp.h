#ifndef _OI_BPP_H
#define _OI_BPP_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/inc/oi_bpp.h#1 $ 

$Log: $
*
*    #3       18 Oct 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
===========================================================================*/
/**
 * @file  
 *
 * This file provides the interface for a Basic Printing Profile appliation.
 */

/** \addtogroup BPP BPP APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#include "oi_common.h"
#include "oi_obexspec.h"
#include "oi_obexsrv.h"

#define OI_BPP_VERSION 0x0100

typedef OI_UINT32 OI_BPP_JOB_ID;

typedef enum {
    BPP_TARGET_DPS = 1, /**< Direct printing */
    BPP_TARGET_PBR,     /**< Print by reference */
    BPP_TARGET_RUI,     /**< Reflected user interface */
    BPP_TARGET_STS,     /**< Status */
    REF_TARGET_OBJ      /**< Referenced objects */
} OI_BPP_TARGET;

typedef enum {
    OI_BPP_REF_SIMPLE = 1,
    OI_BPP_REF_XML,
    OI_BPP_REF_LIST
} OI_BPP_REF_TYPE;

/**@}*/

#endif /* _OI_BPP_H */
