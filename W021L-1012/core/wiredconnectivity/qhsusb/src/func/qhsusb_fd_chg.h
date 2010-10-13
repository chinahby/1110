#ifndef QHSUSB_FD_CHG_H
#define QHSUSB_FD_CHG_H
/*=======================================================================*//**
  Functional driver for dead battery charger function

  @file         qhsusb_fd_chg.h
 
  @brief        Skeleton documentation example
 
  @details      There is no standard class for download. 
                Instead, vendor-specific interface with 
                2 bulk EP (1-IN, 1-OUT) get used.
 
               Copyright (c) 2008 QUALCOMM Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
*//*========================================================================*/
/* $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/src/func/qhsusb_fd_chg.h#3 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_urb.h"
#include "qhsusb_ch9.h" /* for USB descriptor types */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Init
 * 
 * Should be called prior to any other functions
 * 
 * @param rx_callback
 *               URB completion callback,
 *               will be called for every successfully received URB
 *  
 * @return device, NULL if error
 */
struct qhsusb_dcd_dsc_device* qhsusb_fd_chg_init(void);

/**
 * Terminates connection.
 * After this function, interface is not valid
 * and its functions can't be used.
 */
void qhsusb_fd_chg_shutdown(void);

#endif /* #ifndef QHSUSB_FD_CHG_H */

