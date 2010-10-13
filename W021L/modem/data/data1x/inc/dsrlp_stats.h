#ifndef DSRLP_STATS_H
#define DSRLP_STATS_H
/*===========================================================================

                D S R L P  S T A T S     H E A D E R    F I L E

DESCRIPTION
  
  This header file contains all the extern definitions related to RLP
  logging. Any callers of these functions need to include this header file. 
 
EXTERNALIZED FUNCTIONS
 
  dsrlp_clear_rlp_statistics()
    Clears the current values of the RLP statistics on command 
    from the off-target diagnostic tool.
  
  dsrlp_log_rlp_statistics()
    Logs the current values of the RLP statistics to the off-target 
    diagnostic tool.
  
  dsrlp_log_rx_init_frm_buf()
    Inits the log_rx_rlp_frm_buf data structure.

  dsrlp_log_tx_init_frm_buf()
    Inits the dsrlp_log_tx_frm_buf data structure.

  dsrlp_log_rx_init_frame()
    Initializes the next frame packet, as well as the pointer to it,
    in the RLP Rx frames log buffer.

  dsrlp_log_tx_init_frame()
    Initializes the next frame packet, as well as the pointer to it,
    in the RLP Tx frames log buffer.

  dsrlp_log_rx_send_frames()
    Send the accumulated RLP Rx partial log.

  dsrlp_log_tx_send_frames()
    Send the accumulated RLP Tx partial log.

  
Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/data/1x/1xrlp/main/lite/inc/dsrlp_stats.h#1 $ $DateTime: 2008/04/11 07:35:12 $ $Author: nsivakum $

when         who   what, where, why
----------   ---   ----------------------------------------------------------
05/07/04     atp   Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#if defined( FEATURE_RLP_LOGGING )
#include "customer.h"
#include "diagpkt.h"

#include "dsrlp.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define DSRLP_SCH_PAYLOAD_LOG_LEN 5

#define DSRLP_LOG_RX_FRAMES_OFFSET \
  (dsrlp_log_rx_rlp_frm_buf.hdr.len - FPOS(log_rlp_frames_type, var_len_buf))

#define DSRLP_LOG_TX_FRAMES_OFFSET \
  (dsrlp_log_tx_rlp_frm_buf.hdr.len - FPOS(log_rlp_frames_type, var_len_buf))

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Buffer for RLP frames logging 
---------------------------------------------------------------------------*/
extern log_rlp_frames_type dsrlp_log_rx_rlp_frm_buf;
extern log_rlp_frames_type dsrlp_log_tx_rlp_frm_buf;

//vlechan 
//extern boolean             dsrlp_log_rx_sch0_erasure;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSRLP_CLEAR_RLP_STATISTICS

DESCRIPTION
  Clears the current values of the RLP statistics on command from the
  off-target diagnostic tool.

  Input Parameters:
    *req_ptr: Pointer to DIAG request packet
    pkt_len: Length of DIAG request packet

DEPENDENCIES
  Logging mask, dsrlp_log_stats_interval 

RETURN VALUE
  Pointer to DIAG response packet

SIDE EFFECTS
  None
===========================================================================*/
extern diag_rsp_type * dsrlp_clear_rlp_statistics
(
  DIAG_RLP_STAT_RESET_F_req_type *req_ptr,
  word pkt_len
);

/*===========================================================================

FUNCTION DSRLP_LOG_RLP_STATISTICS

DESCRIPTION
  Logs the current values of the RLP statistics to the off-target diagnostic
  tool.  If we can't allocate space for the log packet on the DIAG ring
  buffer, we'll abort and try again next time.

  Input Parameters:
    sr_id: The Service ID for the particular service. Primary is usually 1
           and secondary is usually 2.
    callTerminated: True if we're logging at call termination, False if
           we're logging periodically during the call.

DEPENDENCIES
  Logging mask, dsrlp_log_stats_interval 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dsrlp_log_rlp_statistics
( 
  byte sr_id_index, 
  boolean callTerminated 
);

/*===========================================================================

FUNCTION DSRLP_LOG_RX_INIT_FRM_BUF

DESCRIPTION
  Initializes the dsrlp_log_rx_frm_buf structure 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dsrlp_log_rx_init_frm_buf (void);

/*===========================================================================

FUNCTION DSRLP_LOG_TX_INIT_FRM_BUF

DESCRIPTION
  Initializes the dsrlp_log_tx_frm_buf structure

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dsrlp_log_tx_init_frm_buf (void);

/*===========================================================================

FUNCTION DSRLP_LOG_RX_INIT_FRAME

DESCRIPTION
  This function initializes the next frame packet, as well as the
  pointer to it, in the RLP Rx frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  An RLP Rx partial frames log may be sent.

===========================================================================*/
extern log_rlp_pkt_type * dsrlp_log_rx_init_frame (void);

/*===========================================================================

FUNCTION DSRLP_LOG_TX_INIT_FRAME

DESCRIPTION
  This function initializes the next frame packet, as well as the
  pointer to it, in the RLP Tx frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  An RLP Tx partial frames log may be sent.

===========================================================================*/
extern log_rlp_pkt_type * dsrlp_log_tx_init_frame (void);

/*===========================================================================

FUNCTION DSRLP_LOG_RX_SEND_FRAMES

DESCRIPTION
  Terminate and send the accumulated RLP Rx partial frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  An RLP Rx partial frames log may be sent.

===========================================================================*/
extern void dsrlp_log_rx_send_frames (void);

/*===========================================================================

FUNCTION DSRLP_LOG_TX_SEND_FRAMES

DESCRIPTION
  Terminate and send the accumulated RLP Tx partial frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  An RLP Tx partial frames log may be sent.

===========================================================================*/
extern void dsrlp_log_tx_send_frames (void);


#endif /* FEATURE_RLP_LOGGING */
#endif /* DSRLP_STATS_H */
