#ifndef _BTRCRX_H
#define _BTRCRX_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           B L U E T O O T H    R F C O M M    R E C E I V E

DESCRIPTION
  This file contains declarations for the Bluetooth code that processes
  data received for RFCOMM from L2CAP.

Copyright (c) 2000 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btrcrx.h_v   1.2   02 Mar 2001 12:03:06   propach
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btrcrx.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btrcrx.h_v  $
 *
 *    #4         1  Jun 2004            DP
 * Implemented Mode 2 security.
 *
 *    #3         26 Sep 2003            DP
 * Cleaned up.
 * 
 *    Rev 1.2   02 Mar 2001 12:03:06   propach
 * Removed unused init/online/offline routines.
 * 
 *    Rev 1.1   26 Feb 2001 12:39:28   propach
 * Updated RC flow control.  Updated MSC and test cmd/resp.  Improved
 * QXDM message outputs.
 * 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "dsm.h"

#include "bt.h"
#include "btrcmsgi.h"

#if defined (BT_SWDEV_QXDM_LOG_MSG) || defined (BT_SWDEV_QXDM_LOG_PER)
#include "btlogi.h"
#include "log.h"
#endif


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Type Definitions                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*  Parameters passed through most RFCOMM receive routines.  */

struct ssn_struct;
struct dlc_struct;

typedef struct
{
  boolean             error;
  uint8               fh_dlci;            /*  fh_ fields are unpacked    */
  boolean             fh_command;         /*    from the RFCOMM frame    */
  boolean             fh_direction;       /*    header.                  */
  uint8               fh_server_channel;
  uint8               fh_frame_type;
  boolean             fh_poll_final;
  uint16              fh_frame_length;
  bt_rc_fcs_type      fcs_calculated;     /*  Frame Check Sequence       */
  bt_rc_fcs_type      fcs_received;       /*     (FCS) related fields.   */
  uint8               mcch_type;          /*  mcch_ fields are unpacked  */
  boolean             mcch_command;       /*    from the MCC headers.    */
  uint16              mcch_length;
  struct ssn_struct*  ssn_ptr;            /*  Session table entry ptr.   */
  struct dlc_struct*  dlc_ptr;            /*  DLC table entry pointer.   */
  dsm_item_type*      dsm_ptr;            /*  DSM item pointer.          */
} bt_rc_rx_info_type;


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                    External Functions Prototypes                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_rc_rx_process_frame

DESCRIPTION
  Processes a received RFCOMM frame.

===========================================================================*/
extern void bt_rc_rx_process_frame( bt_rc_rx_info_type* rxi_ptr );


/*===========================================================================

FUNCTION
  bt_rc_rx_unpack_frame_hdr_and_fcs

DESCRIPTION
  Unpacks a received RFCOMM frame header and FCS into the passed
  rx_info structure.

RETURN VALUE
  Updates fh_* and fcs_* fields within rx_info

===========================================================================*/
extern void bt_rc_rx_unpack_frame_hdr_and_fcs(
              bt_rc_rx_info_type* rxi_ptr );


/*===========================================================================

FUNCTION
  bt_rc_rx_sabm_service_sec_result

DESCRIPTION
  Processes service security results initiated by the receipt of an
  SABM frame.

===========================================================================*/
extern void bt_rc_rx_sabm_service_sec_result(
              struct ssn_struct*  ssn_ptr,
              struct dlc_struct*  dlc_ptr,
              boolean             secure_enough );

#endif /* FEATURE_BT */

#endif /* _BTRCRX_H */

