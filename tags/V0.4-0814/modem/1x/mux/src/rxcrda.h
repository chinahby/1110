#ifndef RXCRDA_H
#define RXCRDA_H

/*===========================================================================

       C D M A    R A T E    D E T E R M I N A T I O N   M O D U L E
        
DESCRIPTION
  This header file contains definitions for the rate determination module 
  that are used by the rxc unit.  This header file is internal to the rxc 
  unit.

Copyright (c) 1997 through 2005 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxcrda.h_v   1.7   31 May 2002 18:35:22   lchan  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcrda.h#1 $ $DateTime: 2007/11/15 15:12:49 $ $Author: glenns $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/07   vlc     Fixed compiler errors when REL A is turned off.
04/20/04   jrp     Fixed lint warnings.
05/31/02   lcc     Added rxcrda_update_gating_rate to inform RDA of gating
                   rate when in control hold.
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
                   Implemented comments from VP2 code review.
05/13/02   vlc     Added rxc_dtx_det_dcch() for DTX detection on DCCH
                   (feature FEATURE_IS2000_REL_A_VP2.)
09/12/01   bgc     Renamed filter for SCH and added filter function for DCCH.
06/15/01   day     Delete FEATURE_SPECIAL_MDR
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00 
           bgc     Added rxc_dtx_det_dcch() for DTX detection on DCCH.
11/10/00   jc      Created after repartition of rxc.c

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "rlp.h"
#include "dec5000.h"
#include "cai.h"
#include "rxc.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Indicates whether supplemental frame is full rate or erasure */
typedef enum
{
  RXC_SUP_ERASURE = 0,
  RXC_SUP_FULL
}rxc_tc_sup_rate_type;

#ifdef FEATURE_DETECT_SUP_MUTING
/*
 This feature is used to detect muting of supplemental channels by the
 basestation during a MDR call. IS-95B is lenient enough to allow the
 basestation to not to de-allocate the unused supplemental channels while
 in a MDR call. Hence, typically a basestation may allocate supplementals to
 the mobile when the MDR call is initiated but, if there is no forward
 link data to send, then it may just stop transmitting on these channels
 without de-allocating them.  Thus saving some signalling bandwidth for ch
 allocation in case those are required in near future. This results in mobile
 demodulating un-coded walsh channels which can result in random CRC passes.
 We term this not-transmitting-on-allocated-traffic-channels as "muting".

 This feature runs an IIR estimator for the quality of supplemental channels
 to detect an abrupt good frame which follows a long sequence of bad frames.
 It also uses Yamamato quality metric to finally make a decision that this sup
 is not really a good frame but is an accidental full rate CRC pass.
*/

  /* Threshold for the engery of frame quality estimator */
  #define DATA_IND_THRESHOLD 15

  extern byte cur_di[DEC_MAX_SCC_NUM];  /* current data indicator value */
  extern byte past_di[DEC_MAX_SCC_NUM]; /* past data indicator value */
  extern rlp_rate_enum_type past_sup_status[DEC_MAX_SCC_NUM]; /* past CRC values */
#endif /* FEATURE_DETECT_SUP_MUTING*/

#ifdef FEATURE_RDA_CORRECTION
  extern int8  rxcrda_sch_dtx_fix_counter;
  extern int8  rxcrda_dcch_dtx_fix_counter;
#endif /* FEATURE_RDA_CORRECTION */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION RXC_SCC_RATE_SEL

DESCRIPTION
  Determine the rate of the specified SCC. It is either FULL rate or ERASURE.
  This function returns the rate type rxc_tc_sup_rate_type

DEPENDENCIES
  None.

RETURN VALUE
  DEC_RATE_ALL : If the supplemental channel is an erasure.
  DEC_RATE_FULL : If the supplemental channel is full rate.

SIDE EFFECTS
  None.

===========================================================================*/
#if  defined (FEATURE_IS95B_MDR)
  extern rxc_tc_sup_rate_type rxc_scc_rate_sel ( rxc_dec_type *buf_ptr, byte sup_index );
  /* Decide if the sup channel is erasure or full rate frame */
#endif /* FEATURE_IS95B_MDR */

/*===========================================================================

FUNCTION RXC_DEC_RATE_SEL

DESCRIPTION
  This function calls the appropriate rate procedure to
  process the decoder data received from the Traffic Channel
  and determine the data rate depending on the current rate set.

DEPENDENCIES
  None.

RETURN VALUE
  Decoder rate.

SIDE EFFECTS
  None.

===========================================================================*/
extern word rxc_dec_rate_sel (void);

/*===========================================================================

FUNCTION RXC_MAR_RATE_SEL

DESCRIPTION
  This function determines the proper rate to input to the vocoder based
  on the frame CAI category ( mux1 category ).

DEPENDENCIES
  None.

RETURN VALUE
  Vocoder frame rate.

SIDE EFFECTS
  None.

===========================================================================*/
extern byte rxc_mar_rate_sel(word mux1 );

/*===========================================================================

FUNCTION RXC_VOC_RATE_SEL

DESCRIPTION
  This function determines the proper rate to input to the vocoder based
  on the frame CAI category ( mux1 category ).

DEPENDENCIES
  None.

RETURN VALUE
  Vocoder frame rate.

SIDE EFFECTS
  None.

===========================================================================*/
extern byte rxc_voc_rate_sel( word mux1 );


/*===========================================================================

FUNCTION RXCRDA_SCH_CORRECTION

DESCRIPTION
  If a DTX frame was incorrectly detected as ERASURE, this function tries to
  re-classify it as DTX.  It is a filter to encourage runs of DTX and correct
  sporadic erasures.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

#ifdef FEATURE_RDA_CORRECTION
rxc_frame_type rxc_sch_dtx_filter
(
 rxc_frame_type frame_status
);
#endif /* FEATURE_RDA_CORRECTION */

/*===========================================================================

FUNCTION RXCRDA_DCCH_CORRECTION

DESCRIPTION
  If a DTX frame was incorrectly detected as ERASURE, this function tries to
  re-classify it as DTX.  It is a filter to encourage runs of DTX and fix
  sporadic erasures that should have been classified as DTX.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

#ifdef FEATURE_RDA_CORRECTION
rxc_frame_type rxc_dcch_dtx_filter
(
 rxc_frame_type frame_status
);
#endif /* FEATURE_RDA_CORRECTION */

#ifdef FEATURE_IS2000_P2
/*========================================================================

FUNCTION RXC_DTX_DET_DCCH

DESCRIPTION This function determines if the current frame is a DTX frame,
an erasure frame, or a full frame in the dedicated control channel.

DEPENDENCIES None

RETURN VALUE enum, either RXC_GOOD_FRAME, RXC_DTX_FRAME, or RXC_ERASURE_FRAME


SIDE EFFECTS None

=========================================================================*/
extern word rxc_dtx_det_dcch(dec_rc_type rc);
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_P2
/*========================================================================

FUNCTION RXCRDA_UPDATE_GATING_RATE

DESCRIPTION This function is used to update the pc_acc_scaler variable which is
            used to scale the pc_acc in DCCH channel quality and DTX detection.
            This update is actually done in 2 steps.  The new value is first copied
            into a temporary storage (pending_pc_acc_scaler).  The function argument
            also contains the system time when the new value should become effective.
            The actual update is done in rxc_dtx_det_dcch, when the specified time
            arrives.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxcrda_update_gating_rate
(
  cai_pilot_gating_rate_type gating_rate
);
#endif // FEATURE_IS2000_P2

#endif /* RXCRDA_H */
