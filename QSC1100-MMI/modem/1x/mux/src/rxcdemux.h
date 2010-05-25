#ifndef RXCDEMUX_H
#define RXCDEMUX_H
/*===========================================================================

           C D M A    D E -M U L T I P L E X I N G    M O D U L E

DESCRIPTION
  This header file contains definitions for the receive de-multiplexing module 
  that are used by the rxc unit.  This header file is internal to the rxc 
  unit.

Copyright (c) 1997 through 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxcdemux.h_v   1.2   20 Apr 2001 11:47:36   sanjayr  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcdemux.h#2 $ $DateTime: 2007/11/15 15:23:12 $ $Author: glenns $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/07   vlc     Support release 0 build with both original RLP API and new 
                   FL DSDV RLP API.
07/30/04   vlc     Changed curr_ch to rxc_curr_ch.
06/01/04   vlc     Added externs to allow F-PDCH code to call existing functions 
                   for encapsulated MuxPDU5 processing.
04/18/03   bn      Removed JCDMA Features in FER so that it can apply to DIAG
04/20/01   sr      Merged in from MSM5100 archive
03/20/01   ych     Merged JCDMA Features.
11/10/00   jc      Created after repartition of rxc.c

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef FEATURE_PRE_DV_RLP_INTERFACE
#include "dsdvrlp_rx.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

extern dsdvrlp_fwd_channel_enum_type rxc_curr_ch; /* FCH, DCCH, or PDCH is being demuxed */
extern dsrlp_mux_pdu_enum_type rxc_mux_pdu; /* MUX PDU type (1, 2...) */
#endif  /* !FEATURE_PRE_DV_RLP_INTERFACE */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION RXC_TC_DATA

DESCRIPTION
  This procedure processes data received from the decoder while the Receive
  task is processing the Traffic Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_tc_data( void );

/*===========================================================================
FUNCTION      RXC_INIT_AVERAGED_FER

DESCRIPTION   This function initializes erasure frames for poor FER alarm.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
extern void  rxc_init_averaged_fer( void );

/*===========================================================================
FUNCTION      RXC_PRI_DATA

DESCRIPTION   Services primary traffic stream for the FCH, DCCH and PDCH for 
              Data 3G SO. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
extern void rxc_pri_data
(
  byte bits[],                  /* Address of first byte of primary data   */
  word bit_pos,                 /* Position of first bit of data in buffer */
  word bit_len,                 /* Number of bits of data in buffer        */
  word mux,                     /* CAI MUX1 frame statistic                */
  rxc_dec_type *buf_ptr         /* Ptr to buffer which holds decoder data  */
);

/*===========================================================================
FUNCTION      RXC_SEC_DATA

DESCRIPTION   Services secondary traffic stream for the FCH, DCCH and PDCH for 
              Data 3G SO. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
extern void rxc_sec_data
(
  byte bits[],                  /* Address of first byte of data           */
  word bit_pos,                 /* Position of first bit of data in buffer */
  word bit_len,                 /* Number of bits of data in buffer        */
  word mux,                     /* CAI MUX frame statistic                 */
  rxc_dec_type *buf_ptr         /* Ptr to buffer which holds decoder data  */
);

/*===========================================================================

FUNCTION rxc_tc_data_get_rlp_list

DESCRIPTION
  This function gets 1 linked list element from RLP client to store   
  information of 1 PDU payload.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_tc_data_get_rlp_list (void);

/*===========================================================================

FUNCTION rxc_tc_data_give_rlp_list

DESCRIPTION
  This function gives 1 PDU worth of channel data to RLP client.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void rxc_tc_data_give_rlp_list (void);

#endif /* RXCDEMUX_H */
