#ifndef RXCSYNC_H
#define RXCSYNC_H
/*===========================================================================
                 
                 C D M A    S Y N C    M O D U L E

DESCRIPTION
  This header file contains definitions for the sync module that 
  are used by the rxc unit.  This header file is internal to the rxc unit.

Copyright (c) 1997 through 2005 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxcsync.h_v   1.1   20 Apr 2001 11:47:38   sanjayr  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcsync.h#1 $ $DateTime: 2007/11/15 15:12:49 $ $Author: glenns $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/04   jrp     Fixed lint warnings.
04/20/01   sr      Merged in from MSM5100 archive
11/10/00   jc      Created after repartition of rxc.c

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "dec5000.h"
#include "rxc.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* DEBUG CRC statistics */
extern dword rxc_sc_crc_good;
  /* number of Sync Channel messages with good CRCs */
extern dword rxc_sc_crc_bad;
  /* number of Sync Channel messages with bad CRCs */

extern dec_sc_status_type rxc_sc_status; /* Status(CRC + QUALITY), ser, energy */
extern rxc_sc_ser_type rxc_sc_ser_total;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION RXC_SC_ISR

DESCRIPTION
  This function is the interrupt handler for the Decoder interrupt used
  when the Receive task is processing the Sync Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_sc_isr ( void );

/*===========================================================================

FUNCTION RXC_SC_INIT

DESCRIPTION
  This funtion initializes the Sync channel processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_sc_init(word curr_state);

/*===========================================================================

FUNCTION RXC_SC

DESCRIPTION
  Sync channel processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Next state to be processed.

===========================================================================*/
extern word rxc_sc ( void );

#endif /* RXCSYNC_H */
