#ifndef RXCPAGING_H
#define RXCPAGING_H

/*===========================================================================

                 C D M A    P A G I N G    M O D U L E

DESCRIPTION
  This header file contains definitions for the paging module that are 
  used by the rxc unit.  This header file is internal to the rxc unit.

Copyright (c) 1997 through 2005 by QUALCOMM, Inc.  All Rights Reserved.

===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxcpaging.h_v   1.3   28 Feb 2002 20:55:46   ldefauw  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcpaging.h#1 $ $DateTime: 2007/11/15 15:12:49 $ $Author: glenns $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/28/02   lad     Removed frame_num from rxc_pc_init().
04/20/01   sr      Merged in from MSM5100 archive
11/10/00   jc      Created after repartition of rxc.c

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* number of Paging Channel messages with good CRCs */
extern dword rxc_pc_crc_good;

/* number of Paging Channel messages with bad CRCs */
extern dword rxc_pc_crc_bad;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION RXC_PC_ISR

DESCRIPTION
  This function is the interrupt handler for the Decoder interrupt used
  when the Receive task is processing the Paging Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_pc_isr( void );

/*===========================================================================

FUNCTION RXC_PC

DESCRIPTION
  Paging Channel state processing.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  Next state to be processed.

===========================================================================*/
extern word rxc_pc ( void );

/*===========================================================================

FUNCTION RXC_PC_INIT

DESCRIPTION
  This funtion initializes the Paging Channel processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_pc_init
(
  word curr_state,  /* Current RXC state machine state */
  word pc_rate      /* Rate of Paging Channel */
);

#endif /* RXCPAGING_H */
