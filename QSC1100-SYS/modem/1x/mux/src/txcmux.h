#ifndef TXCMUX_H
#define TXCMUX_H

/*===========================================================================
                 
             C D M A     M U L T I P L E X I N G    M O D U L E

DESCRIPTION
  This header file contains definitions for the multiplexing module that 
  are used by the txc unit.  This header file is internal to the txc unit.

Copyright (c) 1997 through 2005 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/txcmux.h_v   1.5   24 Jul 2002 17:05:10   hkulkarn  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/txcmux.h#1 $ $DateTime: 2007/11/15 15:12:49 $ $Author: glenns $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/04   jrp     Fixed lint warnings.
07/24/02   hrk     Added prototype for txc_tdso_sec_frame_size().
06/17/02   hrk     Changed prototype for txc_build_frame().
06/12/02   hrk     Added prototype for txc_sec().
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00 
           lcc     changed prototype of build_blank_frame () to support DCCH.
01/11/01   tc      Merged MSM_MUX1X.00.00.06.
11/23/00   tc      Fixed txc_data_pump_build_frame for MSM5100 PLT.
12/07/00   bgc     Fixed typo for FEATURE_PLT.
11/10/00   jc      Created after repartition of txc.c

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "log.h"
#include "queue.h"
#include "txctraffic.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#if !defined (FEATURE_DIAG_V7_LOG_IFACE)
/* Log buffer for accumulated MUX frame log */
typedef PACKED struct 
{
  log_r_mux1_type       entry;
  byte                  count;
}txc_log_mux_buf_type; 

extern txc_log_mux_buf_type txc_log_mux_buf; /* Log buffer for accumulated Markov frame log */

#define TXC_SEND_MUX1_LOG()                                     \
  do {                                                          \
    (void) log_submit((log_type *) &txc_log_mux_buf);                  \
                                                                \
    /* Re-initialize counters */                                \
    txc_log_mux_buf.count = 0;                                  \
    txc_log_mux_buf.entry.hdr.len = FSIZ(log_r_mux1_type, hdr); \
  } while (0)

#else

extern q_type txc_log_mux1_q;             /* for logging traffic frame statistics */
/* buffers for txc_log_mux1_q */
#define                            TXC_NUM_MUX1_BUFS 2
extern txc_log_mux1_type txc_log_mux1_pool[ TXC_NUM_MUX1_BUFS ];
#endif /*FEATURE_DIAG_V7_LOG_IFACE*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION TXC_PRI

DESCRIPTION
  This function provides the primary traffic to the Multiplex sublayer
  dependant on what the current primary service option is set to.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void txc_pri (void);

#ifdef FEATURE_PLT
#error code not present
#endif

/*===========================================================================

FUNCTION TXC_BUILD_FRAME

DESCRIPTION
  This function builds the next frame to be transmitted.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void txc_build_frame
(
  txc_sig_frame_type  signext
);

/*===========================================================================

FUNCTION BUILD_BLANK_FRAME

DESCRIPTION
  This function builds a blank-and-burst signaling frame.  Used both for FCH and
  DCCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte build_blank_frame ( txc_dsch_type dsch );

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION TXC_SEC

DESCRIPTION
  This function provides the secondary traffic to the Multiplex sublayer
  dependant on what the current secondary service option is set to.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void txc_sec (void);

/*===========================================================================

FUNCTION TXC_TDSO_SEC_FRAME_SIZE

DESCRIPTION
  This function determines the size of the TDSO data to be built
  when TDSO is assigned as Secondary traffic stream.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern word txc_tdso_sec_frame_size
(
  word    phy_channel,
  boolean msg_on_channel
);
#endif /* FEATURE_IS2000_REL_A_SVD */

#endif /*TXCMUX_H*/

