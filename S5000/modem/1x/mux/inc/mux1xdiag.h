#ifndef MUX1XDIAG_H
#define MUX1XDIAG_H
/*==========================================================================

                      Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of DIAG packets must be PACKED.
  
  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2003 through 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/mux1xdiag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/03   bn      Added support for retrieving paging statistic counters via DIAG
05/22/03   vlc     Added extern prototype for mux1xdiag_init().
04/15/03   bn      Created new file for DIAG MUX1X sub-system access.
===========================================================================*/

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"


/* define sub-commands for MUX1X sub-system */
#define MUX1X_DIAG_FER_F                     0  /* FER sub-command */
#define MUX1X_DIAG_PAGING_STAT_F             1  /* Paging statistic sub-command */
#define MUX1X_DIAG_DV_INFO_LOG_MASK_UPDATE_F 2  /* DV_INFO log mask update 
                                                 *  sub-command */
/* end of define sub-commands for MUX1X sub-system */

/* Data structure for Frame Error Rate
*/
typedef struct 
{

  uint16    meas_frames;
    /* Indicate # of measured frames depended on PWR_REP_FRAMES */

  uint16    err_frames;
    /* Indicate # of error frames during measured frames */

} mux1xdiag_fer_s_type;

/************************************************************************** 
    DIAG REQUEST/RESPONSE PACKET DEFINITIONS FOR MUX1X SUB-SYSTEM
**************************************************************************/

/* MUX-FER diag request/response packet definition
 */

DIAGPKT_SUBSYS_REQ_DEFINE(MUX1X, MUX1X_DIAG_FER_F)
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(MUX1X, MUX1X_DIAG_FER_F)
  uint16          meas_frames;
    /* Indicate # of measured frames depended on PWR_REP_FRAMES */

  uint16          err_frames;
    /* Indicate # of error frames during measured frames */
DIAGPKT_RSP_END


/* MUX-PAGING Statistic diag request/response packet definition
 */

DIAGPKT_SUBSYS_REQ_DEFINE(MUX1X, MUX1X_DIAG_PAGING_STAT_F)
  byte   counters_bitmask;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(MUX1X, MUX1X_DIAG_PAGING_STAT_F)
  uint16          gpm_counter;
    /* GPM statistic counter */

  uint16          spm_counter;
    /* SPM statistic counter */

  uint16          rssi_negative_counter;
    /* RSSI negative statistic counter */

DIAGPKT_RSP_END

/* MUX-DV_INFO log mask update request/response packet definition
 */

DIAGPKT_SUBSYS_REQ_DEFINE(MUX1X, MUX1X_DIAG_DV_INFO_LOG_MASK_UPDATE_F)
  uint16 log_mask;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(MUX1X, MUX1X_DIAG_DV_INFO_LOG_MASK_UPDATE_F)
DIAGPKT_RSP_END


extern void mux1xdiag_init (void);

#endif /* MUX1XDIAG_H */
