#ifndef CUSTDIAG_H
#define CUSTDIAG_H
/*===========================================================================

DESCRIPTION
  Configuration for DIAG.

  Copyright (c) 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/build/cust/custdiag.h#1 $ $DateTime: 2008/03/06 14:38:01 $ $Author: milanp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/02   cah     Added GSM features.
05/01/02   jct     Created
===========================================================================*/

/*---------------------------------------------------------------------------
                             Diagnostics
---------------------------------------------------------------------------*/

/* Diag originated test calls, with UI command
*/
#define FEATURE_DIAG_ORIG_CALL

/* Backward compatibility feature for Diag to allow the new Diag
 * to work with old tools using the old protocol. If defined, 
 * the Diag serial interface is in backward-compatible mode. 
 * If undefined, DIAG runs in "streaming" mode on the serial link.
*/
#undef FEATURE_DIAG_NON_STREAMING

/* Define to avoid breaking the log mask
*/
#define FEATURE_DIAG_COUPLED_ARCHITECTURE

/* Old DIAG kept packet definitions in service iface files.
*/
#define FEATURE_DIAG_PACKET_COUPLING

/* Old DIAG exposed its header for log packets.
*/
#define FEATURE_LOG_EXPOSED_HEADER

/* MC packets need a cache of NV info for status, etc. */
#define FEATURE_DIAG_CACHE

/* Enable diagnv
*/
#define FEATURE_DIAG_NV

/* Enable diag sio coupling
*/
#define FEATURE_DIAG_DS_SIO_COUPLING

/* Diag should check validity of address when doing memory ops.
*/
#define FEATURE_DIAG_HW_ADDR_CHECK

/* Manually register dispatch tables. */
#define FEATURE_DIAG_MANUAL_DISPATCH_TABLE

/* Expose old function in msg.h b/c a client violated the API. */
#define FEATURE_MSG_IFACE_VIOLATION

#endif /* CUSTDIAG_H */
