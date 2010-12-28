#ifndef _BTRCTX_H
#define _BTRCTX_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          B L U E T O O T H    R F C O M M    T R A N S M I T

DESCRIPTION
  This file contains declarations for the Bluetooth code that transmits
  messages for RFCOMM.

Copyright (c) 2000 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btrctx.h_v   1.4   08 Jun 2001 15:04:52   propach
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btrctx.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btrctx.h_v  $
 * 
 *    Rev 1.4   08 Jun 2001 15:04:52   propach
 * Added RFCOMM link negotiations and modem status lines for SPP certification.
 * 
 *    Rev 1.3   16 Mar 2001 17:28:46   propach
 * Added infrastructure for SPP including initial support of
 * RPN and RLS and reworking PN signaling messages.
 * 
 *    Rev 1.2   02 Mar 2001 12:06:18   propach
 * Removed unused init/online/offline routines.  Added TX NSC response.
 * 
 *    Rev 1.1   26 Feb 2001 12:39:38   propach
 * Updated RC flow control.  Updated MSC and test cmd/resp.  Improved
 * QXDM message outputs.
 * 
 *    Rev 1.0   29 Jan 2001 17:09:44   propach
 * Initial revision.
 * 
 *    Rev 1.9   22 Nov 2000 11:45:26   sramacha
 * 
 *    Rev 1.8   13 Nov 2000 17:48:48   amitj
 * Removed the prototype declarations for bt_rc_sig_tx() and bt_rc_pyld_tx()
 * 
 *    Rev 1.7   07 Nov 2000 15:41:06   amitj
 * Corrected the declarations of the 2 new functions
 * 
 *    Rev 1.6   07 Nov 2000 13:26:50   amitj
 * Declared 2 new functions: bt_rc_sig_tx and bt_rc_pyld_tx
 * 
 *    Rev 1.5   30 Oct 2000 14:46:04   sramacha
 * Added test command and response
 * 
 *    Rev 1.4   05 Sep 2000 17:57:08   propach
 * Added QDSP services changes.
 *
 *    Rev 1.3   17 Aug 2000 13:46:32   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.2   02 Aug 2000 09:41:02   propach
 * Added QBTS support for BT.  Added to API.
 *
 *    Rev 1.1   19 Jul 2000 17:27:04   propach
 * Update BT API and featurization with cleanup.
 *
 *    Rev 1.0   04 Jul 2000 19:22:36   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "bt.h"
#include "btrci.h"
#include "btrcrx.h"
#include "dsm.h"

#if defined (BT_SWDEV_QXDM_LOG_MSG) || defined (BT_SWDEV_QXDM_LOG_PER)
#include "btlogi.h"
#include "log.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                    External Functions Prototypes                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_rc_tx_sabm_frame

DESCRIPTION
  Transmits an SABM frame on a specified DLC.

===========================================================================*/
extern void bt_rc_tx_sabm_frame
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_ua_frame

DESCRIPTION
  Transmits a UA frame on a specified DLC.

===========================================================================*/
extern void bt_rc_tx_ua_frame
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_uih_data_frame

DESCRIPTION
  Transmits a UIH frame with specified user data on a specified session.

===========================================================================*/
extern void bt_rc_tx_uih_data_frame
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr,
  dsm_item_type*       dsm_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_disc_frame

DESCRIPTION
  Transmits a DISC (disconnect) frame on a specified DLC.

===========================================================================*/
extern void bt_rc_tx_disc_frame
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_dm_frame

DESCRIPTION
  Transmits a DM frame on a specified session for a specified DLCI.

===========================================================================*/
extern void bt_rc_tx_dm_frame
(
  bt_rc_session_type*  ssn_ptr,
  uint8                dlci
);


/*===========================================================================

FUNCTION
  bt_rc_tx_nsc_response

DESCRIPTION
  Transmits a Not Supported Command (NSC) response with a specified
  received MCC header type field on a specified session.

===========================================================================*/
extern void bt_rc_tx_nsc_response
(
  bt_rc_session_type*  ssn_ptr,
  uint8                rx_mcc_hdr_type
);


/*===========================================================================

FUNCTION
  bt_rc_tx_msc_command

DESCRIPTION
  Transmits an MSC command message for a specified DLC.

===========================================================================*/
extern void bt_rc_tx_msc_command
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_msc_response

DESCRIPTION
  Transmits a specified MSC response message on a specified DLCI on
  which a MSC command was received.

===========================================================================*/
extern void bt_rc_tx_msc_response
(
  bt_rc_rx_info_type*   rxi_ptr,
  bt_rc_mcc_msce_type*  msce_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_pn_command

DESCRIPTION
  Transmits an PN command message for a specified DLC.

===========================================================================*/
extern void bt_rc_tx_pn_command
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_pn_response

DESCRIPTION
  Transmits a specified PN response message on a specified DLCI on
  which a PN command was received.

===========================================================================*/
extern void bt_rc_tx_pn_response
(
  bt_rc_rx_info_type*  rxi_ptr,
  bt_rc_mcc_pn_type*   pn_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_rpn_command

DESCRIPTION
  Transmits an RPN command message for a specified DLC.

===========================================================================*/
extern void bt_rc_tx_rpn_command
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr,
  boolean              request_only
);


/*===========================================================================

FUNCTION
  bt_rc_tx_rpn_response

DESCRIPTION
  Transmits a specified RPN response message on a specified DLCI on
  which a RPN command was received.

===========================================================================*/
extern void bt_rc_tx_rpn_response
(
  bt_rc_rx_info_type*   rxi_ptr,
  bt_rc_mcc_rpne_type*  rpne_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_rls_command

DESCRIPTION
  Transmits an RLS command message for a specified DLC.

===========================================================================*/
extern void bt_rc_tx_rls_command
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_rls_response

DESCRIPTION
  Transmits a specified RLS response message on a specified DLCI on
  which a RLS command was received.

===========================================================================*/
extern void bt_rc_tx_rls_response
(
  bt_rc_rx_info_type*  rxi_ptr,
  bt_rc_mcc_rls_type*  rls_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_fc_command

DESCRIPTION
  Transmits either an FCon or FCoff command message (as appropriate)
  for a specified session.

===========================================================================*/
extern void bt_rc_tx_fc_command
(
  bt_rc_session_type*  ssn_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_fc_response

DESCRIPTION
  Transmits either an FCon or FCoff command response message (as
  appropriate) for a specified session.

===========================================================================*/
extern void bt_rc_tx_fc_response
(
  bt_rc_session_type*  ssn_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_mcc_test_response
  
DESCRIPTION
  Transmits a Test response for a specified received Test command.

===========================================================================*/
extern void bt_rc_tx_mcc_test_response
(
  bt_rc_rx_info_type*  rxi_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_tx_mcc_test_command

DESCRIPTION
  Transmits a test command of specified format on a specified
  RFCOMM session.

===========================================================================*/
extern void bt_rc_tx_mcc_test_command
(
  bt_rc_session_type*  ssn_ptr,
  uint16               byte_count,
  uint8                start_value,
  uint8                increment
);

#endif /* FEATURE_BT */

#endif /* _BTRCTX_H */

