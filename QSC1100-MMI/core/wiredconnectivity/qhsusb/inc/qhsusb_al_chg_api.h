#ifndef _QUSB_CHG_API_H_
#define _QUSB_CHG_API_H_
/*=======================================================================*//**
 * @file        qhsusb_al_chg_api.h
 * @author:     stzahi  
 * @date        23-Sept-2008
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) AL (Adaptation-Layer) for Charging exported API.
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
//  $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/inc/qhsusb_al_chg_api.h#1 $$DateTime: 2008/09/23 08:37:48 $$Author: stzahi $
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
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------
typedef enum
{
  HSU_BOOT_POLL_RESULT_ERROR = -1,
  HSU_BOOT_POLL_RESULT_NO_NEW_EVENT,           /* No change in USB state resulted by polling */
  HSU_BOOT_POLL_RESULT_DISCONNECTED,           /* USB cable disconnected */
  HSU_BOOT_POLL_RESULT_CONNECTED_TO_WALL_CHG,  /* USB cable connected to wall charger */
  HSU_BOOT_POLL_RESULT_CONNECTED_TO_HOST_PC,   /* USB cable connected to a PC */
  HSU_BOOT_POLL_RESULT_NOT_CONNECTED,          /* USB cable is not connected */
  HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED, /* Device is resumed, but not enumerated yet. 
                                                  When resuming *after* enumeration, the 
                                                  ENUMERATED event will be raised */
  HSU_BOOT_POLL_RESULT_ENUMERATED,             /* Device is enumerated successfully */
  HSU_BOOT_POLL_RESULT_SUSPENDED,              /* Device is suspended */
  HSU_BOOT_POLL_RESULT_ENUMERATED_100MA,       /* Device is enumerated successfully with 100ma configuration */
  HSU_BOOT_POLL_RESULT_MAX
} qhsusb_boot_poll_result;

//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------

// ===========================================================================
/**
 * @function    hsu_boot_start_stack
 * 
 * @brief       start the hs-usb stack
 *
 * @details
 *          This function initialize the HS-USB stack.
 * 
 * @param   None
 * 
 * @return  None.
 * 
 */
// ===========================================================================
boolean hsu_boot_start_stack(void);

// ===========================================================================
/**
 * @function    hsu_boot_stop_stack
 * 
 * @brief       Stop the hs-usb stack
 *
 * @details
 *          This function stops the HS-USB stack.
 * 
 * @param   None
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void hsu_boot_stop_stack(void);

/**
 * Analyze status of USB device, detect changes.
 * Return appropriate event.
 * 
 * @note Wall charger detection done before this code called
 * 
 * 3 parameters analyzed:
 * 
 * - (a) attachment state
 * - (c) configuration state
 * - (s) suspend state
 * 
 * old state remembered. Current state compared with new one.
 * Depending on transaction the following events detected.
 * 
 * States named a,c,s for attached, configured, suspend.
 * 
 * - xxx -> 0xx : disconnect HSU_BOOT_POLL_RESULT_DISCONNECTED 
 * - xxx -> 1x1 : suspend    HSU_BOOT_POLL_RESULT_SUSPENDED 
 * - xxx -> 110 : configured HSU_BOOT_POLL_RESULT_ENUMERATED 
 * - 0xx -> 100 : connect    HSU_BOOT_POLL_RESULT_CONNECTED_TO_HOST_PC
 * - 1xx -> 100 : resume     HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED
 *  
 * @return Event
 */
qhsusb_boot_poll_result hsu_boot_poll_stack(void);

// ===========================================================================
/**
 * @function    hsu_boot_start_stack
 * 
 * @brief       start the hs-usb stack
 *
 * @details
 *          This function initialize the HS-USB stack.
 * 
 * @param   None
 * 
 * @return  None.
 * 
 */
// ===========================================================================
boolean hsu_boot_start_stack(void);

// ===========================================================================
/**
 * @function    hsu_boot_disconnect_from_host
 * 
 * @brief       Disconnect the USB device from host
 *
 * @details
 *          This function stops the HS-USB stack.
 *
 * @note    On targets where PHY is on PMIC this function just returns false 
 *          since this operation need to be done using PMIC API and not this API.
 * 
 * @param   None
 * 
 * @return  None.
 * 
 */
// ===========================================================================
boolean hsu_boot_disconnect_from_host(void);

#endif // _QUSB_CHG_API_H_
