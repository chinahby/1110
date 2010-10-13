#ifndef QHSUSB_FD_HDLC_H
#define QHSUSB_FD_HDLC_H
/*=======================================================================*//**
  @file qhsusb_fd_hdlc.h

  Functional driver for download function

  @file         qhsusb_fd_hdlc.h
 
  @brief        Skeleton documentation example
 
  @details      There is no standard class for download. 
                Instead, vendor-specific interface with 
                2 bulk EP (1-IN, 1-OUT) get used.
 
               Copyright (c) 2008 QUALCOMM Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
*//*========================================================================*/
/* $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/src/func/qhsusb_fd_hdlc.h#9 $ */

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

#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
/**
 * Should be set prior to call to \a qhsusb_fd_hdlc_init
 * 
 * Defines flavor for USB interface used:
 * 
 * @param use_cdc_acm
 *               - 1 - use CDC/ACM class
 *               - 0 - use "obex" like vendor specific class
 */
void qhsusb_hdlc_use_cdcacm(int use_cdc_acm);
/**
 * Query whether CDC/ACM used
 * 
 * @return 
 */
int qhsusb_hdlc_get_use_cdcacm(void);
#endif

/**
 * Init
 * 
 * @note Should be called prior to any other functions
 * @param rx_callback
 *                   URB completion callback,
 *                   will be called for every successfully received URB
 * @param str_product
 *                   Product name string
 * 
 * @return device software descriptor, NULL if error
 */
struct qhsusb_dcd_dsc_device* qhsusb_fd_hdlc_init(void (*rx_callback)(struct qhsusb_urb* urb),
                                                  uint8* str_product);

/**
 * Terminates connection.
 * After this function, interface is not valid
 * and its functions can't be used.
 */
void qhsusb_fd_hdlc_shutdown(void);

/**
 * schedule byte for Tx
 * 
 * @attention May block till USB finish data transfer.
 * 
 */
void qhsusb_fd_hdlc_tx_byte(unsigned char chr);

/**
 * Query whether USB is busy doing Tx.
 * 
 * @return - 0 : no transfer in progress
 *         - any other value : USB is not finished transfers
 */
int qhsusb_fd_hdlc_is_tx_active(void);

/**
 * Indicate Rx URB processing by upper layers is finished
 * and URB may be re-used.
 */
void qhsusb_fd_hdlc_rx_completed(struct qhsusb_urb* urb);
#endif /* #ifndef QHSUSB_FD_HDLC_H */

