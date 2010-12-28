#ifndef TXCSO_H
#define TXCSO_H

/*===========================================================================
                 
   C D M A   T R A N S M I T   S E R V I C E   O P T I O N S  M O D U L E

DESCRIPTION
  This header file contains definitions for the transmit service options
  module that are used by the txc unit.  This header file is internal to 
  the txc unit.

Copyright (c) 1997 through 2008 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/txcso.h_v   1.5   06 Jun 2002 18:15:42   hkulkarn  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/txcso.h#2 $ $DateTime: 2008/07/18 10:34:36 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV
07/30/04   vlc     Added support for rel C TDSO.
06/05/02   hrk     Added prototype for txc_tdso_sec_traffic().
06/15/01   day     Mainlined FEATURE_IS2000
04/20/01   sr      Merged in from MSM5100 archive
11/13/00   bgc     (merge) Separated FCH and SCH TDSO blocks.
11/10/00   jc      Created after repartition of txc.c

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "cai.h"
#include "txc.h"
#include "tdso.h"

#ifdef FEATURE_IS2000_TDSO
extern tdso_frame_block_type fch_tdso_frame_block;
extern tdso_frame_block_type sch_tdso_frame_block;
#endif // FEATURE_IS2000_TDSO

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION TXC_VOC_TRAFFIC

DESCRIPTION
  Get traffic data (primary or secondary) from the vocoder.

DEPENDENCIES
  The vocoder should be initialized already.

RETURN VALUE
  Returns the address of the vocoder data in a pointer passed to the
  procedure.

  Returns the rate of the vocoder data

SIDE EFFECTS
  None

===========================================================================*/
extern cai_data_rate_type txc_voc_traffic
(
  byte **data_ptr,
    /* Returns pointer to data */
  txc_sig_frame_type signext
    /* Signaling mode for next packet */
);


/*===========================================================================

FUNCTION TX_LOOPBACK_TRAFFIC

DESCRIPTION
  Get loopback data bits (primary only) from the Loopback Service Option
  layer.

DEPENDENCIES
  The Loopback Service Option layer should be initialized already.

RETURN VALUE
  Returns the address of the loopback data bits in a pointer passed to the
  procedure.

  Returns the rate of the loopback data.

SIDE EFFECTS
  None

===========================================================================*/

extern cai_data_rate_type txc_loopback_traffic
(
  byte **data_ptr,
    /* Returns pointer to loopback data bits */
  txc_sig_frame_type signext
    /* Signaling mode for this frame */
);

/*===========================================================================

FUNCTION TXC_MAR_TRAFFIC

DESCRIPTION
  Get traffic data (primary or secondary) from Markov frame error rate tests.

DEPENDENCIES
  The tx.frame_cnt has to be right.

RETURN VALUE
  Returns the address of the FER data in a variable passed to the
  procedure.

  Returns the rate of the FER data.

SIDE EFFECTS
  None

===========================================================================*/

extern cai_data_rate_type txc_mar_traffic
(
  byte **data_ptr
    /* Returns pointer to data */
);

#ifdef FEATURE_IS2000_TDSO
/*===========================================================================

FUNCTION TXC_TDSO_TRAFFIC

DESCRIPTION
  Get traffic data (primary) from TDSO

DEPENDENCIES

RETURN VALUE
  Returns the data rate

SIDE EFFECTS
  None

===========================================================================*/

extern cai_data_rate_type txc_tdso_traffic
(
  byte **data_ptr,
    /* Returns pointer to data */
  txc_sig_frame_type signext
    /* Signaling mode for next packet */
);

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION TXC_TDSO_SEC_TRAFFIC

DESCRIPTION
  Get traffic data (Secondary) from TDSO

DEPENDENCIES
  Currently, TDSO is supported on either FCH or DCCH phyical channels.

RETURN VALUE
  Returns the data rate

SIDE EFFECTS
  None

===========================================================================*/

extern cai_data_rate_type txc_tdso_sec_traffic
(
  byte **data_ptr,
    /* Returns pointer to data */
  boolean msg_on_channel
    /* Signaling mode for next frame */
);
#endif /* FEATURE_IS2000_REL_A_SVD */
#endif /*FEATURE_IS2000_TDSO*/
#endif /*TXCSO_H*/
