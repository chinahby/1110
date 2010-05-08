#ifndef __SDCC_ERRNO_H
#define __SDCC_ERRNO_H
/******************************************************************************
 * File: sdcc_common_api.h
 *
 * Services: 
 *    Public API header for SDCC error code
 *
 * Description:
 *    This file contains the SDCC error code.
 *
 * Copyright (c) 2008 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_errno.h#1 $
  $DateTime: 2008/05/13 20:54:50 $ $Author: shaehnic $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
01/21/08     sc      Initial version

=============================================================================*/

/* enumeration values for errors that are used in SDCC */
typedef enum
{
   SDCC_NO_ERROR,
   SDCC_ERR_UNKNOWN,
   SDCC_ERR_CMD_TIMEOUT,
   SDCC_ERR_DATA_TIMEOUT,
   SDCC_ERR_CMD_CRC_FAIL,              
   SDCC_ERR_DATA_CRC_FAIL,
   SDCC_ERR_CMD_SENT,
   SDCC_ERR_PROG_DONE,
   SDCC_ERR_CARD_READY,
   SDCC_ERR_INVALID_TX_STATE,
   SDCC_ERR_SET_BLKSZ,
   SDCC_ERR_SDIO_R5_RESP,
   SDCC_ERR_DMA,
   SDCC_ERR_READ_FIFO,
   SDCC_ERR_WRITE_FIFO,
   SDCC_ERR_ERASE,
   SDCC_ERR_SDIO,
   SDCC_ERR_SDIO_READ,
   SDCC_ERR_SDIO_WRITE,
   SDCC_ERR_INVALID_PARAM,
   SDCC_ERR_CARD_UNDETECTED,
   SDCC_ERR_FEATURE_UNSUPPORTED,
   SDCC_ERR_SECURE_COMMAND_IN_PROGRESS,
   SDCC_ERR_READ_SEC_CMD_NOT_ALLOWED,
   SDCC_ERR_ABORT_READ_SEC_CMD,
   SDCC_ERR_CARD_INIT = 215
} SDCC_STATUS;

#endif /* __SDCC_ERRNO_H */

