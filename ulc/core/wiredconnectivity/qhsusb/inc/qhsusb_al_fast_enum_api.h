#ifndef _QHSUSB_FAST_ENUM_API_H_
#define _QHSUSB_FAST_ENUM_API_H_

/*=======================================================================*//**
 * @file        qhsusb_al_fast_enum_api.h
 * @author:     stzahi  
 * @date        23-Sept-2008
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) AL (Adaptation-Layer) for Fast-Enum exported API.
 *
 * @details     This header file contains the API to be used by the boot for charging via USB.
  *
 * @note        
 *
 *              Copyright (c) 2008 QUALCOMM Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/

// ===========================================================================
//
//                           EDIT HISTORY FOR FILE
//  This section contains comments describing changes made to the module.
//  Notice that changes are listed in reverse chronological order.
//
//  $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/inc/qhsusb_al_fast_enum_api.h#2 $$DateTime: 2008/10/07 05:29:56 $$Author: amirs $
//
// when       who     what, where, why
// --------   ---     ----------------------------------------------------------
// 09/23/08   stzahi  Creation
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"     // basic types as byte word uint32 etc

//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------

void hsu_al_fast_enum_init(void);
void hsu_al_fast_enum_shutdown(void);
boolean hsu_al_fast_enum_poll(void);

void qhsusb_al_fast_enum_delay_usec(uint32 microseconds);


#endif // _QHSUSB_FAST_ENUM_API_H_
