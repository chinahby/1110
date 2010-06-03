#ifndef _BTQSOCNVMPLATFORM_H
#define _BTQSOCNVMPLATFORM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       B L U E T O O T H    B T S    D R I V E R    H E A D E R    F I L E

GENERAL DESCRIPTION
  This module contains the Bluetooth BTS SoC driver definitions specific to 
  AMSS platforms.
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008-2009 QUALCOMM Incorporated. 
All Rights Reserved. 
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/wconnect/bthost/soccfg/rel/00.00.26/src/btqsocnvmplatform.h#3 $
 $DateTime: 2009/02/10 18:33:52 $
 $Author: jnahar $



  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-02-10   jn  Increase number of Rx buffers in BTS from 9 to 11.


*
*   #5          06 Feb 2009           SA  
*   Cleanup & Added Support for FTM_BT feature for WM targets 
*
*   #4          16 Sep 2008           SA
*   Added support for dynamic parsing of released nvm   
*
*   #3          22 May 2008           RH
*   Changed WM flag to UNDER_CE to avoid including "customer.h" (for WM7 compile)  
*
*   #2          20 Mar 2008           RH
*   Added Windows Mobile support using FEATURE_WINCE.  
*
*   #1         31 Jan  2008          BH
*   Created new file for AMSS BTS 4020 (QSoC) support.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include "btqsocnvm_pf_def.h"
#include "btqsocnvm.h"
#include "customer.h"
#include "comdef.h"


/* Defined in AMSS's soc/btsoc.h for SOC targets */
#define BT_SOC_MAX_RX_PKT_SIZE       0x0208  /* 520d */
/* Defined in AMSS's bti.h for SOC targets */
#define BT_MTP_MAX_HC_LEN            341

/*===========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.
===========================================================================*/

/*=========================================================================*/
/*         Macros for BTS Buffer numbers and sizes                         */
/*=========================================================================*/

/* Max HCI buffer length for BTS. Must be 8 bytes larger than what */
#define BT_QSOC_MAX_HC_TX_LEN        (BT_MTP_MAX_HC_LEN + 0x8)

#define BT_QSOC_MAX_HC_TX_LEN_LSB    (BT_QSOC_MAX_HC_TX_LEN & 0xFF) /* LSB */
/* MSB */
#define BT_QSOC_MAX_HC_TX_LEN_MSB    (uint8)((uint16)BT_QSOC_MAX_HC_TX_LEN >> 0x8)

#define BT_QSOC_MAX_HC_RX_LEN        BT_SOC_MAX_RX_PKT_SIZE
#define BT_QSOC_MAX_HC_RX_LEN_LSB    (BT_QSOC_MAX_HC_RX_LEN & 0xFF) /* LSB */
/* MSB */
#define BT_QSOC_MAX_HC_RX_LEN_MSB    (uint8)((uint16)BT_QSOC_MAX_HC_RX_LEN >> 0x8)

#define BT_QSOC_MAX_HC_NUM_RX_BUF    0x0B /* 11d */
#define BT_QSOC_MAX_HC_NUM_TX_BUF    0x0E /* 14d */
#define BT_QSOC_SCO_BUF_SIZE         0x5A /* 90d */

/*==========================================================================

  FUNCTION       btqsocnvmplatform_log_error

  DESCRIPTION    Logs the parser error.

  DEPENDENCIES   None.

  PARAMETERS     btqsocnvm_parser_err_type.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void btqsocnvmplatform_log_error
(
btqsocnvm_parser_err_type parser_err
);

/*==========================================================================

  FUNCTION       memcpy

  DESCRIPTION    extern declaration for standard memcpy function.

  DEPENDENCIES   None.

  PARAMETERS     void *dest, const void *src, unsigned int count

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void * memcpy
(
void *dest,
const void *src,
unsigned int count
);


/*==========================================================================

  FUNCTION       strcmp 

  DESCRIPTION    extern declaration for standard strcmp function.

  DEPENDENCIES   None.

  PARAMETERS     const char *s1, const char* s2.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern int strcmp
(
const char *s1,
const char *s2
);

#endif /* _BTQSOCNVMPLATFORM_H */
