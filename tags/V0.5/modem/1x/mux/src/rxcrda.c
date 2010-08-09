/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        C D M A    R A T E    D E T E R M I N A T I O N   M O D U L E

GENERAL DESCRIPTION
  Contains rate determination algorithims
  
EXTERNALIZED FUNCTIONS
  rxc_scc_rate_sel - Determine the rate of the specified SCC
  rxc_dec_rate_sel - calls the appropriate rate procedure to
  process the decoder data received from the Traffic Channel
  rxc_mar_rate_sel - determines the proper rate to input to the vocoder based
  on the frame CAI category
  rxc_voc_rate_sel - determines the proper rate to input to the vocoder based
  on the frame CAI category
  rxc_dcch_dtx_filter - if a DTX frame was incorrectly detected as ERASURE, 
  this function tries to re-classify it as DTX.
  rxc_sch_dtx_filter - if a DTX frame was incorrectly detected as ERASURE, 
  this function tries to re-classify it as DTX.
                 
 Copyright (c) 1990 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxcrda.c_v   1.43   14 Oct 2002 11:14:14   vlechan  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcrda.c#2 $ $DateTime: 2008/07/18 10:34:36 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Reduced debug string sizes for ULC effort.
03/06/07   vlc     Fixed compiler errors when REL A is turned off.
09/19/06   vlc     Fixed lint errors.
07/25/05   rkc     Fixed lint errors.
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.                                   
01/26/05   vlc     Removed obsolete ds.h inclusion.
04/20/04   jrp     Fixed lint warnings.
04/12/04   bn      Included muxmsm.h
04/12/04   bn      Used the macro from muxmsm.h to get sch_nt and wtd_pilot
04/09/04   bn      Replaced the old SCH DTX logic by the new DTX logic for 6500 or above
02/20/04   bn      Incorporated the feature GPS_OVERRIDE into DCCH DTX detection logic
11/24/03   vlc     Fixed more compilation warnings for RVCT compiler.
10/22/03   sr      Fixed compilation warning errors when using RVCT compiler 
10/14/03   ejv     Added MSM rev check around iqacc setting for 6500.
10/06/03   vlc     Needed to update DCCH RDA logging info in VP2 to take into
                   account change in rxc_dcch_status.
09/30/02   bn      Fixed the incorrect DCCH RDA logging info in VP2
09/11/03   dna     Put FEATURE_MSM6500_IQACC_INIT back in.
09/05/03   vlc     Changed rxc_dcch_status when feature MSMHW_EXTENDED_ENERGY_MATRIX is
                   turned off as well.
07/17/03   bn      Added new DTX logics to handle 32x,RC4 and 16x, RC5. Also optimized
                   the DTX logic, removed the 16x, RC3 energy saturation workaround
08/08/03   vlc     Use dec_dcch_status_type instead of dec_fch_status_type for
                   rxc_dcch_status.
07/23/03   ejv     Added temporary feature FEATURE_MSM6500_IQACC_INIT.
02/18/03   sr      Applied iqacc=0 check to declare DTX in rxc_dtx_det_sch() for 
                   Turbo also 
02/14/03   bn      deleted the merger error of scaling the pc_acc twice when DTX
11/19/02   hxw     Modified rxc_dec_rate_sel(), rxc_dtx_det_sch(), and rxc_scc_rate_sel()
                   to declare erasures when in GPS visit.
10/14/02   vlc     Added #include ts.h to eliminate "inventing extern" warnings.
09/11/02   bn      During the FSCH transaction, the FSCH RC will be set one frame early so
                   at the RDA logging logic, introduce on frame delay for FSCH to synch
                   Used the Correct DCCH data structure in the DTX logic when extended
                   energy matrix defined
08/06/02   cr      Added MSM6100 changes.
09/03/02   bn      Corrected NT_IO register names for both SCH and FCH/DCCH in MSM6050
08/27/02   sr      Initialized temp32bit to 0
08/21/02   hrk     If iqacc is 0, frame is declared DTX in rxc_dtx_det_sch().
08/09/02   bn      Fixed over floating number in channel quanlity computation
07/25/02   bn      Fixed compilation error where R-SCH 32x feature turns off
07/24/02   bn      Not to output the NT_IO zero debug message in PLT mode and
                   send a error fatal message when having invalid RC in DCCH DTX
07/11/02   bn      Added logics to support new QIB algorithm
07/10/02   dlb2    Use RDA_FRAME_INFO_2 log format for MSMHW_EXTENDED_ENERGY_MATRIX
                   feature.
07/10/02   bn      Added DTX logics to support 32X, RC4 and 16X, RC5
06/04/02   bn      Modified the RDA logging function parameters for VP2
05/31/02   lcc/bn  Added scaling of pc_acc for DCCH when pilot is gated.
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
                   Implemented comments from VP2 code review.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up 
                   (feature FEATURE_IS2000_REL_A_VP2.)
03/27/02   ems     Modified sizes of ser and energy fields, featurized with
                   MSMHW_EXTENDED_ENERGY_MATRIX.
03/26/02   bgc     Aligned rxc_rda_log_buffer pointer.
02/04/02   bn      For PLT, By pass the logic of handle nt_io = 0 in channel quality 
01/21/02   bgc     Merged data changes for disabling MDR.
01/09/02   bn      Added the frame offset into RDA log and fixed the RDA incorrect reading
                   of PACC/Frame energy for DCCH.
12/20/01   bn      Corrected the function name for logging RDA
12/10/01   hrk     Renamed RXC_FULL_FRAME to RXC_GOOD_FRAME.
12/04/01   bn      Fixed the complilation errors when undefine FEATURE_DS
11/27/01   bn      Included the quality threshold and channel quality in the RDA packet
                   and indicate DTX when SCH fingers out of lock
11/20/01   bn      Added RDA logging logic
11/20/01   bn      Fixed the case when DCCH sub-chan-gain-lin = 0, set it to 45
                   return DTX frame status when frame energy = 0
11/15/01   bn      Fixed the log of reading fch_nt_io for Non-Puma, and corrected
                   messed up of feature enhance dcch dtx   
11/14/01   bn      Indicate bad channel when fch_nt_io = 0, and added code to 
                   read fch_nt_io for non-puma
11/14/01   bn      Fixed division by zero when frame energy is zero
11/13/01   bn      Added the enhance DCCH DTX detection and channel supervision logics
11/12/01   sr      Merged 1.9.1 back to the trunk
           sr      Moved rxcrda_dtx_msg_enable outside FEATURE_IS2000_P2
           bgc     Updated SCH DTX to work for MSM5105.
           bgc     Added improved SCH DTX detection for convolutional
                   coded SCH.  
           sr      Fixed FCH rate determination algorithm for RC5
09/17/01   vlc     Updated ERR_FATAL to use correct parameter for forward link 
                   rc.
09/12/01   bgc     New DCCH DTX detection algorithm was added and the filter
                   was generalized to apply to SCH or DCCH.
08/30/01   bgc     Added special cases for frames of all 1's (STDSO) in the 
                   FCH RDA for RC4.  Added CRC check to DCCH DTX detection
                   to reduce RDA errors on good frames.
06/15/01   day     Delete FEATURE_SPECIAL_MDR, FEATURE_MDR_FER_F3MSG
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00 
           lcc     Added support for FEATURE_IS2000_P2
           bgc     Added rxc_dtx_det_dcch() for DTX detection to
                   rxc_dec_rate_sel().
11/09/00   bgc     Added special RDA for EVRC.
11/10/00   jc      This file was part of rxc.c. See rxccommon.c for full 
                   change history

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <memory.h>
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "assert.h"
#include "err.h"
#include "log.h"
#include "voc.h"
#include "rxc.h"
#include "mccdma.h"
#include "mccrx.h"
#include "cai.h"
#include "ts.h"

#include "deint.h"
#include "msm_drv.h"
#include "atmr.h"
#include "ffpc.h"

#if  defined (FEATURE_IS95B_MDR)
  #include "mccscm.h"             /* supplemental channel processing routines */
#endif /* FEATURE_IS95B_MDR  */

#ifdef FEATURE_DS
  #if  defined (FEATURE_IS95B_MDR)
    #include "mdrrlp.h"              /* New MDR RLP routines */
  #else
    #include "rlp.h"                 /* RLP routines */
  #endif /* FEATURE_IS95B_MDR */
#endif /* FEATURE_DS */

#ifdef TMOBILE
#error code not present
#endif

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

#ifdef FEATURE_FACTORY_TESTMODE
  /* need some txc buffers to do loopback */
  #include "txc.h"
#endif

#include "rxccommon.h"
#include "rxctraffic.h"
#include "rxcsync.h"
#include "rxcrda.h"
#include "rxcdemux.h"
#include "rxcpaging.h"
#include "mux_logtypes.h"
#include "muxmsm.h"
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
*                        FEATURE_DETECT_SUP_MUTING
*-------------------------------------------------------------------------*/
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
  byte cur_di[DEC_MAX_SCC_NUM];  /* current data indicator value */
  byte past_di[DEC_MAX_SCC_NUM]; /* past data indicator value */
  rlp_rate_enum_type past_sup_status[DEC_MAX_SCC_NUM]; /* past CRC values */
#endif /* FEATURE_DETECT_SUP_MUTING*/

/* debug flag to output RDA related debug message */
boolean rxcrda_dtx_msg_enable = 0;
#ifdef FEATURE_RDA_CORRECTION
  int8  rxcrda_sch_dtx_fix_counter;
  int8  rxcrda_dcch_dtx_fix_counter;

  rxc_frame_type rxcrda_dtx_filter(rxc_frame_type frame_status, int8 *counter);
#endif /* FEATURE_RDA_CORRECTION */


const byte rxcrda_dcch_ser_dtx_thresholds[6] =
{ 0,  /* No such thing as RC 0 */
  0,  /* RC1, no DCCH */
  0,  /* RC2, no DCCH */
  236, /* RC3 */
  100, /* RC4 */
  188  /* RC5 */
};

const byte rxcrda_dcch_ser_erasure_thresholds[6] =
{ 0,  /* No such thing as RC 0 */
  0,  /* RC1, no DCCH */
  0,  /* RC2, no DCCH */
  195, /* RC3 */
  50,  /* RC4 */
  150  /* RC5 */
};

const word rxcrda_convert_to_rxc_rates[6] =
{
  RXC_ERASURE_FRAME,  /*CAI_BLANK_RATE*/
  0,                  /*CAI_EIGHTH_RATE */
  0,                  /*CAI_QUARTER_RATE */
  0,                  /*CAI_HALF_RATE*/
  RXC_GOOD_FRAME,     /*CAI_FULL_RATE*/
  RXC_DTX_FRAME       /*CAI_NULL_RATE*/
};

const word rxcrda_convert_to_cai_rates[3] =
{
  CAI_FULL_RATE, //RXC_GOOD_FRAME
  CAI_NULL_RATE, //RXC_DTX_FRAME
  CAI_BLANK_RATE //RXC_ERASURE_FRAME
};

#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  
static int32 rxcrda_dcch_quality_thresh;
static rxc_dcch_quality_type rxc_dcch_frame_quality = RXC_POOR_CHANNEL;
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION */
static int rxcrda_norm_eng[4] = {0};

#ifdef FEATURE_IS2000_P2
/*
** scaler for the pc_acc in dcch channel quality and dtx detection to adjust for
** pilot gating. If there is no gating then the value of pc_acc_scaler
** should be 1
*/
static byte   pc_acc_scaler = 1;
/* The following are the new pc_acc scaler to use when going into/out of control hold
** and the time it supposed to be effective.  Note pending_pc_acc_scaler_time contains
** the system frame time. */
static byte   pending_pc_acc_scaler = 1;
static qword  pending_pc_acc_scaler_time;
#endif // FEATURE_IS2000_P2

/* RDA logging variables */
static uint32 raw_rda_log_buffer[ RDA_LOG_BUFFER_SIZE/4 +1 ] = {0};
static log_puma_rda_information_type * rxc_rda_log_buffer = 
  (log_puma_rda_information_type *) raw_rda_log_buffer;//lint !e740 Unusual pointer cast
static rda_sub_record_header_type *rxc_rda_sub_record_hdptr = NULL;
static uint16 rxc_rda_log_buff_offset = 0;
static byte rxc_rda_log_sch_rc = 0;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION RXC_RDA_FRAME_INFO_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated RDA frame information log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A RDA frame information log may be sent.

===========================================================================*/
void rxc_rda_frame_info_send_log(void)
{
  if (rxc_rda_log_buffer->hdr.len > 0)
  {
    /* Will send log if submitted. */
    (void) log_submit((log_type *) rxc_rda_log_buffer);

    /* Flag to init start of log */
    rxc_rda_log_buffer->hdr.len = 0;
  }

} /* rxc_rda_frame_info_send_log */

/*===========================================================================

FUNCTION RXC_RDA_LOG_FRAME_INFO_INIT

DESCRIPTION
  This function initializes the RAD frame info log subrecord, as well as the
  pointer to it and the entry for this frame, in the frame types log buffer.
  If necessary, the log is sent and a new one is generated.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A RDA frame info. log may be sent.

===========================================================================*/
uint8  * rxc_rda_log_frame_info_init(void)
{
  channels_info_type  rda_chan_set_mask;
  chan_rc_type        rda_chan_rc_mask;
  uint8 * rda_sub_record_ptr = NULL;

  /* If logging is enabled. */
  if (log_status(LOG_RDA_FRAME_INFO_2_C))
  {
    /* If we reached accumulation threshold, send it. 
    the threshold = the buffer size - the maximum of byte per record in the 
    worst case analysic */
    if (rxc_rda_log_buffer->hdr.len >= RDA_LOG_BUFFER_THRESHOLD)
    {
      rxc_rda_frame_info_send_log();
    }
    /* check if there is a need to re-initialize */
    if (rxc_rda_log_buffer->hdr.len == 0)
    {
      rxc_rda_log_buffer->hdr.len = sizeof(log_hdr_type);
      rxc_rda_log_buffer->hdr.code = LOG_RDA_FRAME_INFO_2_C;

      /* First frame, log time stamp. */
      ts_get((unsigned long *) rxc_rda_log_buffer->hdr.ts);

      /*  Fill in beginning of record   */
      rxc_rda_log_buffer->srheader.num_sub_records = 0;

      /* initialize all the control variables */
      rxc_rda_sub_record_hdptr = &(rxc_rda_log_buffer->srheader); 
      rxc_rda_log_buff_offset = 0;  
    }
    else
    {
      /* If a new sub-record is needed, i.e. the channel set or RC has changed,
      ** check for the condition here.
      */
      /* clear the RDA channel set and RC masks */
      rda_chan_set_mask.mask = 0;
      rda_chan_rc_mask.mask = 0;

      /* set the frame offset */
      rda_chan_rc_mask.chan_rc.frame_offset = rxc_frame_offset;
      /* set the masks based on the active channels and RC */
#ifdef FEATURE_IS2000_P2
      if (rxc_so.for_fch.included)
#endif 
      {
        rda_chan_set_mask.individual_channels.fch = TRUE;
        rda_chan_rc_mask.chan_rc.fch_rc = rxc_so.for_fch.rc;
      }
#ifdef FEATURE_IS2000_P2
      if (rxc_so.for_dcch.included)
      {
        rda_chan_set_mask.individual_channels.dcch = TRUE;
        rda_chan_rc_mask.chan_rc.dcch_rc = rxc_so.for_dcch.rc;
      }
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_SCH
      if (rxc_sch_frame_status!=RXC_NO_FRAME)
      {
        rda_chan_set_mask.individual_channels.sch0 = TRUE;
        rda_chan_rc_mask.chan_rc.sch_rc = rxc_rda_log_sch_rc;
      }
#endif /* FEATURE_IS2000_SCH */
    
      if((rda_chan_set_mask.mask != rxc_rda_sub_record_hdptr->assigned_channels.mask) 
         || (rda_chan_rc_mask.mask != rxc_rda_sub_record_hdptr->rc.mask))
      {
        /* We no longer have a valid sub-record, re-initialize the header pointer */
        rxc_rda_sub_record_hdptr = (rda_sub_record_header_type *) 
                   &rxc_rda_sub_record_hdptr->reference_buf[rxc_rda_log_buff_offset]; 
        rxc_rda_sub_record_hdptr->num_sub_records = 0;
        rxc_rda_log_buff_offset = 0;
      }

    }
    /* Need to init a new subrecord */
    if (rxc_rda_sub_record_hdptr != NULL)
    {
      if (rxc_rda_sub_record_hdptr->num_sub_records == 0)
      {
        /* reset the channel mask */
        rxc_rda_sub_record_hdptr->assigned_channels.mask = 0;

        /* reset the channels RC */
        rxc_rda_sub_record_hdptr->rc.mask = 0;

        /********Initialize the channel assignment and RC for each channels ************/

        /* set the frame offset in the header sub-record */
        rxc_rda_sub_record_hdptr->rc.chan_rc.frame_offset = rxc_frame_offset;

        /* check for FCH */
#ifdef FEATURE_IS2000_P2
        if (rxc_so.for_fch.included)
#endif
        {
          rxc_rda_sub_record_hdptr->assigned_channels.individual_channels.fch = TRUE;
          rxc_rda_sub_record_hdptr->rc.chan_rc.fch_rc = rxc_so.for_fch.rc;
        }
#ifdef FEATURE_IS2000_P2
        /* check for DCCH */
        if (rxc_so.for_dcch.included)
        {
          rxc_rda_sub_record_hdptr->assigned_channels.individual_channels.dcch = TRUE;
          rxc_rda_sub_record_hdptr->rc.chan_rc.dcch_rc = rxc_so.for_dcch.rc;
        }
#endif /* FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_SCH
        /* check for SCH */
        if (rxc_sch_frame_status!=RXC_NO_FRAME)
        {
          /* set the channel bit mask */
          rxc_rda_sub_record_hdptr->assigned_channels.individual_channels.sch0 = TRUE;
        
          /* SCH RC */
          rxc_rda_sub_record_hdptr->rc.chan_rc.sch_rc = rxc_rda_log_sch_rc;

        }
#endif /* FEATURE_IS2000_SCH */

        /* Update packet length */
        rxc_rda_log_buffer->hdr.len += FPOS(rda_sub_record_header_type,reference_buf);//lint !e734 FPOS
      }
      rda_sub_record_ptr = &rxc_rda_sub_record_hdptr->reference_buf[rxc_rda_log_buff_offset];
    }
  }
  else
  {
    rxc_rda_log_buffer->hdr.len = 0; /* Flag that this record has stopped */
  }
  return rda_sub_record_ptr;
}

/*=========================================================================
FUNCTION RXC_RDA_LOG_FRAME_INFO

DESCRIPTION
  This function populates RDA frame information logs.

DEPENDENCIE
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
void rxc_rda_log_frame_info (uint8 fch_dec_rate, uint8 dcch_dec_rate)
#else 
void rxc_rda_log_frame_info (uint8 dec_rate)
#endif /* FEATURE_IS2000_REL_A_SVD and FEATURE_IS2000_P2 */
{
  uint8 *rda_sub_record_ptr;
  uint8 total_byte = 0;
  uint8 index = 0;
  fch_rda_sub_record_type  *fch_rda_sub_record;
#ifdef FEATURE_IS2000_P2
  dcch_rda_sub_record_type *dcch_rda_sub_record;
#endif 
#ifdef FEATURE_IS2000_SCH
  sch_rda_sub_record_type  *sch_rda_sub_record;
#endif

  rda_sub_record_ptr = rxc_rda_log_frame_info_init();
  if (rda_sub_record_ptr != NULL)
  {
    /* include FCH RDA information if FCH channel is active */
    if (rxc_rda_sub_record_hdptr->assigned_channels.individual_channels.fch)
    {
      /* fill in the RDA data for FULL, Haft, Quater and Eight frame */
      for (index = 0; index < 4; index++)
      {
        fch_rda_sub_record = (fch_rda_sub_record_type *) (rda_sub_record_ptr + total_byte);
       
        fch_rda_sub_record->norm_ren_energy = rxcrda_norm_eng[index];//lint !e732 Loss of sign
        fch_rda_sub_record->ser = rxc_fch_status.ser[index];
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
        fch_rda_sub_record->decision = fch_dec_rate;
#else
        fch_rda_sub_record->decision = dec_rate;
#endif /* FEATURE_IS2000_REL_A_SVD and FEATURE_IS2000_P2 */
        fch_rda_sub_record->status = rxc_fch_status.status[index];
        total_byte += sizeof(fch_rda_sub_record_type);
      }

      /* increasing the number of sub-record to by 4 */
      rxc_rda_sub_record_hdptr->num_sub_records += 4; 

    }
#ifdef FEATURE_IS2000_P2
    if (rxc_rda_sub_record_hdptr->assigned_channels.individual_channels.dcch)
    {
      /* fill in the DCCH RDA data for FULL frame */
      /* note that, the data for DCCH is FCH with Full frame */
      dcch_rda_sub_record = (dcch_rda_sub_record_type *) (rda_sub_record_ptr + total_byte);

#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
      dcch_rda_sub_record->quality_threshold = rxcrda_dcch_quality_thresh;//lint !e732 Loss of sign
      dcch_rda_sub_record->quastat.quality_decision = rxc_dcch_frame_quality;
#else
      dcch_rda_sub_record->quality_threshold = 0; 
      dcch_rda_sub_record->quastat.quality_decision = 0;
#endif /* end of #ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION */
      
      dcch_rda_sub_record->pc_acc = DEMOD_GET_PC_ACC();
      dcch_rda_sub_record->fch_nt_io  = DEMOD_GET_FCH_NT_IO();

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
      dcch_rda_sub_record->decision = dcch_dec_rate;
      dcch_rda_sub_record->ser = rxc_dcch_status.ser;
      dcch_rda_sub_record->quastat.status = rxc_dcch_status.status;
      dcch_rda_sub_record->norm_ren_energy = rxc_dcch_status.energy;//lint !e732 Loss of sign
#else
      dcch_rda_sub_record->decision = dec_rate;
      dcch_rda_sub_record->ser = rxc_fch_status.ser[0];
      dcch_rda_sub_record->quastat.status = rxc_fch_status.status[0];
      dcch_rda_sub_record->norm_ren_energy = rxc_fch_status.energy[DEC_RATE_FULL-1];
#endif /* FEATURE_IS2000_P2 and FEATURE_IS2000_REL_A_SVD */
      dcch_rda_sub_record->quastat.reserve = 0;
      total_byte += (sizeof(dcch_rda_sub_record_type));
      /* increasing the number of sub-record to by 1 */
      rxc_rda_sub_record_hdptr->num_sub_records += 1; 
    }
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_SCH
    if (rxc_rda_sub_record_hdptr->assigned_channels.individual_channels.sch0)
    {
      /* fill in the SCH RDA data for FULL frame */
      sch_rda_sub_record = (sch_rda_sub_record_type *) (rda_sub_record_ptr + total_byte);
      sch_rda_sub_record->iq_acc = DEMOD_GET_SCH_IQ_ACC();
      sch_rda_sub_record->sch_nt_io  = DEMOD_GET_SCH_NT_IO();
      if (rxc_sch.code_type == DEC_CONVOLUTIONAL) /* convolutional code */
      {
        sch_rda_sub_record->norm_ren_energy 
                                = rxc_sch_status.conv_sch_status.energy;//lint !e732 Loss of sign
        sch_rda_sub_record->code_info.dev_sch_conv.ser 
                                = rxc_sch_status.conv_sch_status.ser;
        sch_rda_sub_record->code_info.dev_sch_conv.ltu_crc 
                                = rxc_sch_status.conv_sch_status.ltu_crc;
        sch_rda_sub_record->code_info.dev_sch_conv.status 
                                = rxc_sch_status.conv_sch_status.status;
        sch_rda_sub_record->rate = rxc_sch.sch_rate;
        sch_rda_sub_record->decision = rxc_sch_frame_status;
        sch_rda_sub_record->code_type = rxc_sch.code_type;
      }
#ifdef MSMHW_TURBO_CODING
      else /* turbo code */
      {
        sch_rda_sub_record->norm_ren_energy 
                                     = rxc_sch_status.turbo_sch_status.energy;//lint !e732 Loss of sign
        sch_rda_sub_record->code_info.dev_sch_turbo.num_iteration 
                                     = rxc_sch_status.turbo_sch_status.num_iteration;
        sch_rda_sub_record->code_info.dev_sch_turbo.min_llr 
                                     = rxc_sch_status.turbo_sch_status.min_llr;
        sch_rda_sub_record->code_info.dev_sch_turbo.crc_pass 
                                     = rxc_sch_status.turbo_sch_status.crc_pass;
        sch_rda_sub_record->rate = rxc_sch.sch_rate;
        sch_rda_sub_record->decision = rxc_sch_frame_status;
        sch_rda_sub_record->code_type = rxc_sch.code_type;

      }
#endif /* MSMHW_TURBO_CODING */

      total_byte += (sizeof(sch_rda_sub_record_type));
      /* increasing the number of sub-record to by 1 */
      rxc_rda_sub_record_hdptr->num_sub_records += 1; 
    }
#endif /* FEATURE_IS2000_SCH */
    /* update the record length and the sub-record offset */
    rxc_rda_log_buffer->hdr.len += total_byte;
    rxc_rda_log_buff_offset += total_byte;
  }
  /* since the RC value of SCH will be set up one frame early in burst data, need to
  introduce a one frame delay in the SCH RDA log info */
  rxc_rda_log_sch_rc = rxc_sch.rc;
}

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

byte rxc_voc_rate_sel
(
  word mux1
    /* mux category for frame */
)
{

  byte ret_rate;
    /* vocoder rate to return */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* determine vocoder rate based on frame catetory */
  switch( mux1 )
  {

    case CAI_MUX1_FOR_1_ID:
    case CAI_MUX2_FOR_1_ID:
    #ifdef FEATURE_IS2000_P2
    case CAI_MUX1_FOR_DCCH_1_ID:
    case CAI_MUX2_FOR_DCCH_1_ID:
    #endif // FEATURE_IS2000_P2
    {
      /* full rate */
      ret_rate = VOC_1_RATE;
      break;
    }

    case CAI_MUX2_FOR_2_ID:
    case CAI_MUX2_FOR_6_ID:
    case CAI_MUX2_FOR_11_ID:
    #ifdef FEATURE_IS2000_P2
    case CAI_MUX2_FOR_DCCH_2_ID:
    case CAI_MUX2_FOR_DCCH_6_ID:
    #endif // FEATURE_IS2000_P2

    case CAI_MUX1_FOR_2_ID:
    case CAI_MUX1_FOR_6_ID:
    case CAI_MUX1_FOR_11_ID:
    #ifdef FEATURE_IS2000_P2
    case CAI_MUX1_FOR_DCCH_2_ID:
    case CAI_MUX1_FOR_DCCH_11_ID:
    #endif // FEATURE_IS2000_P2
    {
      /* half rate - dim and burst frame or half rate frame */
      ret_rate = VOC_2_RATE;
      break;
    }

    case CAI_MUX2_FOR_3_ID:
    case CAI_MUX2_FOR_7_ID:
    case CAI_MUX2_FOR_12_ID:
    case CAI_MUX2_FOR_15_ID:
    case CAI_MUX2_FOR_19_ID:
    #ifdef FEATURE_IS2000_P2
    case CAI_MUX2_FOR_DCCH_3_ID:
    case CAI_MUX2_FOR_DCCH_7_ID:
    #endif // FEATURE_IS2000_P2

    case CAI_MUX1_FOR_3_ID:
    case CAI_MUX1_FOR_7_ID:
    case CAI_MUX1_FOR_12_ID:
    #ifdef FEATURE_IS2000_P2
    case CAI_MUX1_FOR_DCCH_3_ID:
    case CAI_MUX1_FOR_DCCH_12_ID:
    #endif // FEATURE_IS2000_P2
    {
      /* quarter rate frame */
      ret_rate = VOC_4_RATE;
      break;
    }

    case CAI_MUX2_FOR_4_ID:
    case CAI_MUX2_FOR_8_ID:
    case CAI_MUX2_FOR_10_ID:
    case CAI_MUX2_FOR_13_ID:
    case CAI_MUX2_FOR_16_ID:
    case CAI_MUX2_FOR_18_ID:
    case CAI_MUX2_FOR_20_ID:
    case CAI_MUX2_FOR_22_ID:
    case CAI_MUX2_FOR_24_ID:
    #ifdef FEATURE_IS2000_P2
    case CAI_MUX2_FOR_DCCH_4_ID:
    case CAI_MUX2_FOR_DCCH_8_ID:
    case CAI_MUX2_FOR_DCCH_10_ID:
    #endif // FEATURE_IS2000_P2

    case CAI_MUX1_FOR_4_ID:
    case CAI_MUX1_FOR_8_ID:
    case CAI_MUX1_FOR_13_ID:
    #ifdef FEATURE_IS2000_P2
    case CAI_MUX1_FOR_DCCH_4_ID:
    case CAI_MUX1_FOR_DCCH_13_ID:
    #endif // FEATURE_IS2000_P2
    {
      /* eigth rate frame */
      ret_rate = VOC_8_RATE;
      break;
    }

    case CAI_MUX2_FOR_5_ID:
    case CAI_MUX2_FOR_9_ID:
    case CAI_MUX2_FOR_14_ID:
    case CAI_MUX2_FOR_17_ID:
    case CAI_MUX2_FOR_21_ID:
    case CAI_MUX2_FOR_23_ID:
    case CAI_MUX2_FOR_25_ID:
    #ifdef FEATURE_IS2000_P2
    case CAI_MUX2_FOR_DCCH_5_ID:
    case CAI_MUX2_FOR_DCCH_9_ID:
    case CAI_MUX2_FOR_DCCH_27_ID:
    #endif // FEATURE_IS2000_P2

    case CAI_MUX1_FOR_5_ID:
    case CAI_MUX1_FOR_14_ID:
    #ifdef FEATURE_IS2000_P2
    case CAI_MUX1_FOR_DCCH_5_ID:
    case CAI_MUX1_FOR_DCCH_14_ID:
    case CAI_MUX1_FOR_DCCH_15_ID:
    #endif // FEATURE_IS2000_P2
    {
      /* blank rate frame - blank and burst frame */
      /* If we are to treat blank and burst as erasure tell vocoder erasure */
      ret_rate = ( ( rxc_erase ) ? VOC_ERASURE : VOC_0_RATE );
      break;
    }

    case CAI_MUX2_FOR_26_ID:
    case CAI_MUX1_FOR_10_ID:
    #ifdef FEATURE_IS2000_P2
    case CAI_MUX2_FOR_DCCH_26_ID:
    case CAI_MUX1_FOR_DCCH_10_ID:
    #endif // FEATURE_IS2000_P2
    {
      /* Erasure frame - unable to determine rate of frame */
      ret_rate = VOC_ERASURE;
      break;
    }

    case CAI_MUX1_FOR_9_ID:
    {
      /* Full likely - full rate frame with probable bit error */
      ret_rate = VOC_ERR_RATE;
      break;
    }

    default:
    {
      /* invalid mux option */
      MSG_HIGH("Bad MUX cat %d", mux1,0,0);
      ret_rate = VOC_0_RATE;
      break;
    }
  } /* switch */

  return( ret_rate );
}/* rxc_voc_rate_sel */


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

byte rxc_mar_rate_sel
(
  word mux1
    /* mux category for frame */
)
{

  byte ret_rate;
    /* vocoder rate to return */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* determine vocoder rate based on frame catetory */
  switch( mux1 )
  {

    case CAI_MUX1_FOR_1_ID:
    case CAI_MUX2_FOR_1_ID:
    {
      /* full rate */
      ret_rate = VOC_1_RATE;
      break;
    }

    case CAI_MUX1_FOR_6_ID:
    case CAI_MUX2_FOR_11_ID:
    {
      /* half rate frame */
      ret_rate = VOC_2_RATE;
      break;
    }

    case CAI_MUX1_FOR_7_ID:
    case CAI_MUX2_FOR_19_ID:
    {
      /* quarter rate frame */
      ret_rate = VOC_4_RATE;
      break;
    }

    case CAI_MUX1_FOR_8_ID:
    case CAI_MUX2_FOR_24_ID:
    {
      /* eigth rate frame */
      ret_rate = VOC_8_RATE;
      break;
    }

    case CAI_MUX1_FOR_2_ID:
    case CAI_MUX1_FOR_3_ID:
    case CAI_MUX1_FOR_4_ID:
    case CAI_MUX1_FOR_5_ID:
    case CAI_MUX1_FOR_11_ID:
    case CAI_MUX1_FOR_12_ID:
    case CAI_MUX1_FOR_13_ID:
    case CAI_MUX1_FOR_14_ID:

    case CAI_MUX2_FOR_2_ID:
    case CAI_MUX2_FOR_3_ID:
    case CAI_MUX2_FOR_4_ID:
    case CAI_MUX2_FOR_5_ID:
    case CAI_MUX2_FOR_6_ID:
    case CAI_MUX2_FOR_7_ID:
    case CAI_MUX2_FOR_8_ID:
    case CAI_MUX2_FOR_9_ID:
    case CAI_MUX2_FOR_10_ID:
    case CAI_MUX2_FOR_12_ID:
    case CAI_MUX2_FOR_13_ID:
    case CAI_MUX2_FOR_14_ID:
    case CAI_MUX2_FOR_15_ID:
    case CAI_MUX2_FOR_16_ID:
    case CAI_MUX2_FOR_17_ID:
    case CAI_MUX2_FOR_18_ID:
    case CAI_MUX2_FOR_20_ID:
    case CAI_MUX2_FOR_21_ID:
    case CAI_MUX2_FOR_22_ID:
    case CAI_MUX2_FOR_23_ID:
    case CAI_MUX2_FOR_25_ID:
    {
      /* blank rate frame - blank and burst frame */
      /* If we are to treat blank and burst as erasure tell vocoder erasure */
      ret_rate = ( ( rxc_erase ) ? VOC_ERASURE : VOC_0_RATE );
      break;
    }

    case CAI_MUX1_FOR_10_ID:
    case CAI_MUX2_FOR_26_ID:
    {
      /* Erasure frame - unable to determine rate of frame */
      ret_rate = VOC_ERASURE;
      break;
    }

    case CAI_MUX1_FOR_9_ID:
    {
      /* Full likely - full rate frame with probable bit error */
      ret_rate = VOC_ERR_RATE;
      break;
    }

    default:
    {
      /* invalid mux option */
      MSG_HIGH("Bad MUX cat %d", mux1,0,0);
      ret_rate = VOC_0_RATE;
      break;
    }
  } /* switch */

  return( ret_rate );
}/* rxc_mar_rate_sel */


/*==========================================================================
FUNCTION RXC_FCH_RDA

DESCRIPTION
  This is the new rate determination algorithm for RC3, RC3 and RC5.

DEPENDENCIES
 None

RETURN VALUE
 Decoded Rate of the frame (FULL/HALF/QUARTER/EIGHTH/ERASURE)

SIDE EFFECTS
 None

===========================================================================*/
word rxc_fch_rda(deint_rc_type rc)
{
  word dec_rate;

#define FULL 0
#define HALF 1
#define QUARTER 2
#define EIGHTH 3

  /* Energy metrics for full, half, quarter and eighth */
  int *eng;

  /* CRC for full, half, quarter and eighth */
  byte crc[4];
  /* Total number of rates that pass CRC */
  byte crcN;

  /* Yamamoto quality bits for full, half, quarter, and eighth */
  byte yam[4];

  /* Energy * CRC for full, half, quarter, and eighth */
  int eng_crc[4];

  /* Indexes for the various arrays */
  byte max_eng_rate, max_eng_crc_rate;

  /* high_8th_eng_flag holds the results of a test for the special case
     where only one crc passes and 8th rate has the highest energy. */
  byte high_8th_eng_flag = 0;

  dec_rate = CAI_BLANK_RATE;

  /* eng[FULL] is the energy metric estimate for the full rate frame */
  eng = rxcrda_norm_eng;
  eng[FULL] = rxc_fch_status.energy[DEC_RATE_FULL - 1];
  /* eng[HALF] = energy metric estimate for the half rate frame */
  /* output buffer * sqrt(2). approximtion sqrt(2) = 181/2^7. */
  eng[HALF] = (((rxc_fch_status.energy[DEC_RATE_HALF - 1]) * 181) / 128 ); /* >>7 */
  /* eng[2] = energy metric estimate for the qtr rate frame from decoder */
  /* sqrt(2)^2. */
  eng[QUARTER] = ((rxc_fch_status.energy[DEC_RATE_QTR - 1]) * 2);
  /* eng[EIGHTH] = energy metric estimate for the 8th rate frame from the */
  /* decoder output buffer *  sqrt(2)^3. approximation sqrt(2)^3 = 181/2^6 */
  eng[EIGHTH] = (((rxc_fch_status.energy[DEC_RATE_8TH - 1]) * 181) / 64 ); /* >>6 */

  /* Set CRC's for full, half, quarter and eighth rate */
  crc[FULL] = rxc_fch_status.status[DEC_RATE_FULL - 1] & 0x01;
  crc[HALF] = rxc_fch_status.status[DEC_RATE_HALF - 1] & 0x01;
  crc[QUARTER] = rxc_fch_status.status[DEC_RATE_QTR - 1] & 0x01;
  crc[EIGHTH] = rxc_fch_status.status[DEC_RATE_8TH - 1] & 0x01;

  /* Set count of how many rates that pass CRC */
  crcN = crc[FULL] + crc[HALF] + crc[QUARTER] + crc[EIGHTH];

  /* Set Yamamoto bits for just quarter and eighth (full and half
     are not used.) */
  // yam[FULL] = (rxc_fch_status.status[DEC_RATE_FULL - 1] & 0x02) >> 1;
  // yam[HALF] = (rxc_fch_status.status[DEC_RATE_HALF - 1] & 0x02) >> 1;
  yam[QUARTER] = (rxc_fch_status.status[DEC_RATE_QTR - 1] & 0x02) >> 1;
  yam[EIGHTH] = (rxc_fch_status.status[DEC_RATE_8TH - 1] & 0x02) >> 1;

  /* Set eng_crc[] array */
  eng_crc[FULL] = eng[FULL] * crc[FULL];
  eng_crc[HALF] = eng[HALF] * crc[HALF];
  eng_crc[QUARTER] = eng[QUARTER] * crc[QUARTER];
  eng_crc[EIGHTH] = eng[EIGHTH] * crc[EIGHTH];

  /* Check to see if none of the CRC indicator for any rate is good.
     If yes, declare the frame as an erasure */

  if (crcN == 0)
  {
    dec_rate = CAI_BLANK_RATE;
    return dec_rate;
  }

  /* Find the maximum value in eng[] without loop overhead */
  max_eng_rate = FULL;
  if (eng[HALF] > eng[FULL])
  {
    max_eng_rate = HALF;
  }
  if (eng[QUARTER] > eng[max_eng_rate])
  {
    max_eng_rate = QUARTER;
  }
  if (eng[EIGHTH] > eng[max_eng_rate])
  {
    max_eng_rate = EIGHTH;
  }

  /* Find the maximum value in eng_crc[] without loop overhead */
  max_eng_crc_rate = FULL;
  if (eng_crc[HALF] > eng_crc[FULL])
  {
    max_eng_crc_rate = HALF;
  }
  if (eng_crc[QUARTER] > eng_crc[max_eng_crc_rate])
  {
    max_eng_crc_rate = QUARTER;
  }
  if (eng_crc[EIGHTH] > eng_crc[max_eng_crc_rate])
  {
    max_eng_crc_rate = EIGHTH;
  }

  if (eng_crc[max_eng_crc_rate] <= 0)
  {
    dec_rate = CAI_BLANK_RATE;
    return dec_rate;
  }

  /* Store the results for the special test case */
  if ((crcN == 1) && (max_eng_rate == EIGHTH))
  {
    high_8th_eng_flag = 1;
  }

  switch (rc)
  {
    case DEINT_RC3:
    {
      if(crc[FULL] && (rxc_fch_status.ser[FULL] < 160) &&
         (rxc_fch_status.ser[FULL] < rxc_fch_status.ser[HALF]) &&
         (rxc_fch_status.ser[FULL] < rxc_fch_status.ser[QUARTER]) &&
         (rxc_fch_status.ser[FULL] < rxc_fch_status.ser[EIGHTH]))
      {
        dec_rate = CAI_FULL_RATE;
      }
      else
      {
        switch (max_eng_crc_rate)
        {
          case FULL:
          {
            if (((eng[FULL] - eng[HALF] > -430) &&
                 (eng[FULL] - eng[QUARTER] > -500) &&
                 (eng[FULL] - eng[EIGHTH] > -505)) || high_8th_eng_flag)
            {
              dec_rate = CAI_FULL_RATE;
            }

            break;
          }

          case HALF:
          {
            if (((eng[HALF] - eng[FULL] > 250) &&
                 (eng[HALF] - eng[QUARTER] > 30) &&
                 (eng[HALF] - eng[EIGHTH] > -105)) || high_8th_eng_flag)
            {
              dec_rate = CAI_HALF_RATE;
            }

            break;
          }

          case QUARTER:
          {
            if ((eng[QUARTER] - eng[FULL] > 170) &&
                (eng[QUARTER] - eng[HALF] > 100) &&
                (eng[QUARTER] - eng[EIGHTH] > -115) && (yam[QUARTER] == 1))
            {
              dec_rate = CAI_QUARTER_RATE;
            }

            break;
          }

          case EIGHTH:
          {
            if ((eng[EIGHTH] - eng[FULL] > 25) &&
                (eng[EIGHTH] - eng[HALF] > -40) &&
                (eng[EIGHTH] - eng[QUARTER] > 0) && (yam[EIGHTH] == 1))
            {
              dec_rate = CAI_EIGHTH_RATE;
            }

            break;
          }

          default:
            MSG_ERROR("Bad CRC rate %d",max_eng_crc_rate,0,0);
            break;

        }
      }
      break;
    }

    case DEINT_RC4:
    {
      if(crc[FULL] && (rxc_fch_status.ser[FULL] < 50) &&
         (rxc_fch_status.ser[FULL] < rxc_fch_status.ser[HALF]) &&
         (rxc_fch_status.ser[FULL] < rxc_fch_status.ser[QUARTER]) &&
         (rxc_fch_status.ser[FULL] < rxc_fch_status.ser[EIGHTH]))
      {
        dec_rate = CAI_FULL_RATE;
      }
      else if( crc[FULL] && !crc[HALF] &&
              ( (eng[FULL] < eng[HALF]) && (eng[FULL] < eng[QUARTER]) &&
                (eng[FULL] < eng[EIGHTH]) ) &&
              rxc_fch_status.ser[FULL] < 80 )
      {
        dec_rate = CAI_FULL_RATE;
      }
      else
      {
        switch (max_eng_crc_rate)
        {
          case FULL:
          {
            if (((eng[FULL] - eng[HALF] > -180) &&
                 (eng[FULL] - eng[QUARTER] > -300) &&
                 (eng[FULL] - eng[EIGHTH] > -280)) || high_8th_eng_flag)
            {
              dec_rate = CAI_FULL_RATE;
            }

            break;
          }

          case HALF:
          {
            if (((eng[HALF] - eng[FULL] > 60) &&
                 (eng[HALF] - eng[QUARTER] > -105) &&
                 (eng[HALF] - eng[EIGHTH] > -230)) || high_8th_eng_flag)
            {
              dec_rate = CAI_HALF_RATE;
            }

            break;
          }

          case QUARTER:
          {
            if ((eng[QUARTER] - eng[FULL] > 185) &&
                (eng[QUARTER] - eng[HALF] > 110) &&
                (eng[QUARTER] - eng[EIGHTH] > -150) && (yam[QUARTER] == 1))
            {
              dec_rate = CAI_QUARTER_RATE;
            }

            break;
          }

          case EIGHTH:
          {
            if ((eng[EIGHTH] - eng[FULL] > 30) &&
                (eng[EIGHTH] - eng[HALF] > 15) &&
                (eng[EIGHTH] - eng[QUARTER] > -25) && (yam[EIGHTH] == 1))
            {
              dec_rate = CAI_EIGHTH_RATE;
            }

            break;
          }

          default:
            MSG_ERROR("Bad CRC rate %d",max_eng_crc_rate,0,0);
            break;

        }
      }
      break;
    }

    case DEINT_RC5:
    {
      if(crc[FULL] && (rxc_fch_status.ser[FULL] < 150) &&
         (rxc_fch_status.ser[FULL] < rxc_fch_status.ser[HALF]) &&
         (rxc_fch_status.ser[FULL] < rxc_fch_status.ser[QUARTER]) &&
         (rxc_fch_status.ser[FULL] < rxc_fch_status.ser[EIGHTH]))
      {
        dec_rate = CAI_FULL_RATE;
      }
      else
      {
      switch (max_eng_crc_rate)
      {
        case FULL:
        {
          if (((eng[FULL] - eng[HALF] > 30) &&
               (eng[FULL] - eng[QUARTER] > -35) &&
               (eng[FULL] - eng[EIGHTH] > -20)) || high_8th_eng_flag)
          {
            dec_rate = CAI_FULL_RATE;
          }

          break;
        }

        case HALF:
        {
          if (((eng[HALF] - eng[FULL] > 100) &&
               (eng[HALF] - eng[QUARTER] > 0) &&
               (eng[HALF] - eng[EIGHTH] > -50)) || high_8th_eng_flag)
          {
            dec_rate = CAI_HALF_RATE;
          }

          break;
        }

        case QUARTER:
        {
          if ((eng[QUARTER] - eng[FULL] > 145) &&
              (eng[QUARTER] - eng[HALF] > 105) &&
              (eng[QUARTER] - eng[EIGHTH] > 10))
          {
            dec_rate = CAI_QUARTER_RATE;
          }

          break;
        }

        case EIGHTH:
        {
          if ((eng[EIGHTH] - eng[FULL] > -10) &&
              (eng[EIGHTH] - eng[HALF] > -175) &&
              (eng[EIGHTH] - eng[QUARTER] > -120) && (yam[EIGHTH] == 1))
          {
            dec_rate = CAI_EIGHTH_RATE;
          }

          break;
        }

          default:
            MSG_ERROR("Bad CRC rate %d",max_eng_crc_rate,0,0);
            break;

        } // switch
      }
      break;
    }


    default:
    {
      ERR_FATAL ("Bad Radio Config %d", rc, 0, 0);
    }

  }

  return dec_rate;
}

#ifndef MSM5000_IRAM_FWD /* Moved to internal RAM */
/*===========================================================================

FUNCTION RXC_9600_DEC_RATE_SEL

DESCRIPTION
  This function processes the decoder data received from the Traffic Channel
  and determines the data rate in RATE SET 1.

DEPENDENCIES
  None.

RETURN VALUE
  Decoder rate.

SIDE EFFECTS
  None.

===========================================================================*/

word rxc_9600_dec_rate_sel ()
{
  const word SER2_THRESH = 60;
  /* Threshold for determining half rate */
  const word SER4_THRESH = 60;
  /* Threshold for determining quarter rate */
  const word SER8_THRESH = 64;
  /* Threshold for determing eighth rate */
  const word DSER_THRESH = 10;

   /* result of the rate decision algorithm */
   word dec_rate;

/*--------------------------------------------------------------------*/
  if ( rxc_fch_status.status[DEC_RATE_FULL - 1] & DEC_CRC1_M)
  {
    if ( rxc_fch_status.status[DEC_RATE_HALF - 1] & DEC_CRC2_M )
    {
      if ( rxc_fch_status.ser[DEC_RATE_FULL - 1] <= rxc_fch_status.ser[DEC_RATE_HALF - 1] + 15 )
      {
        /* full rate */
        dec_rate = VOC_1_RATE;
      }
      else
      {
        /* half rate */
        dec_rate = VOC_2_RATE;
      }
    }
    else
    {
      if ( rxc_fch_status.ser[DEC_RATE_FULL - 1] <= 77 )
      {
        /* full rate */
        dec_rate = VOC_1_RATE;
      }
      else
      {
        /* erasure  */
        dec_rate = VOC_0_RATE;
      }
    }
  }
  else if ( (rxc_fch_status.status[DEC_RATE_HALF - 1] & DEC_CRC2_M) && (rxc_fch_status.ser[DEC_RATE_HALF - 1] <=SER2_THRESH) )
  {
    if( !(rxc_fch_status.status[DEC_RATE_QTR - 1] & DEC_GOOD4_M ) &&
        !(rxc_fch_status.status[DEC_RATE_8TH - 1] & DEC_GOOD8_M ) )
    {
      /* half rate */
      dec_rate = VOC_2_RATE;
    }
    else if (rxc_fch_status.status[DEC_RATE_8TH - 1] & DEC_GOOD8_M )
    {
      if( rxc_fch_status.ser[DEC_RATE_HALF - 1] <= rxc_fch_status.ser[DEC_RATE_8TH - 1] + DSER_THRESH )
      {
        /* half rate */
        dec_rate = VOC_2_RATE;
      }
      else
      {
        /* eighth rate */
        dec_rate = VOC_8_RATE;
      }
    }
    else if( rxc_fch_status.ser[DEC_RATE_HALF - 1] <= rxc_fch_status.ser[DEC_RATE_QTR - 1] + DSER_THRESH )
    {
      /* half rate */
      dec_rate = VOC_2_RATE;
    }
    else
    {
      /* fourth rate */
      dec_rate = VOC_4_RATE;
    }

  }
  else if ( (rxc_fch_status.status[DEC_RATE_QTR - 1] & DEC_GOOD4_M) && (rxc_fch_status.status[DEC_RATE_8TH - 1] &DEC_GOOD8_M) )
  {
    if ( (rxc_fch_status.ser[DEC_RATE_8TH - 1] < rxc_fch_status.ser[DEC_RATE_QTR - 1] ) && (rxc_fch_status.ser[DEC_RATE_8TH - 1] <=SER8_THRESH ) )
    {
      dec_rate = VOC_8_RATE;
    }
    else if ( (rxc_fch_status.ser[DEC_RATE_QTR - 1] < rxc_fch_status.ser[DEC_RATE_8TH - 1] ) && (rxc_fch_status.ser[DEC_RATE_QTR - 1] <=SER4_THRESH ) )
    {
      dec_rate = VOC_4_RATE;
    }
    else
    {
      /* Erasure */
      dec_rate = VOC_0_RATE;
    }
  }

  else if ( (rxc_fch_status.status[DEC_RATE_QTR - 1] & DEC_GOOD4_M) && (rxc_fch_status.ser[DEC_RATE_QTR - 1] <=SER4_THRESH) )
  {
    dec_rate = VOC_4_RATE;
  }

  else if ( (rxc_fch_status.status[DEC_RATE_8TH - 1] & DEC_GOOD8_M) && (rxc_fch_status.ser[DEC_RATE_8TH - 1] <=SER8_THRESH) )
  {
    dec_rate = VOC_8_RATE;
  }

  else if ( rxc_fch_status.ser[DEC_RATE_FULL - 1] <= 71 )
  {
    /* Drop the full rate probable bit error as an erasure */
    dec_rate = VOC_0_RATE;
  }

  else
  {
    /* Erasure */
    dec_rate = VOC_0_RATE;
  }
  return (dec_rate);
}
#endif /* MSM5000_IRAM_FWD */


/*===========================================================================

FUNCTION RXC_14400_DEC_RATE_SEL

DESCRIPTION
  This function processes the decoder data received from the Traffic Channel
  and determines the data rate in RATE SET 2.

DEPENDENCIES
  None.

RETURN VALUE
  Decoder rate.

SIDE EFFECTS
  None.

===========================================================================*/

/* PASSING CRC mask */
#define RXC_CRC 0x0F

/*
** Absolute thresholds for individual rates with 1 CRC checking
** with extra threshold for eight rate bad Yamamoto.
*/
#define CRC8_GOOD_THRESH0 120
#define CRC8_GOOD_THRESH1 120
#define CRC4_GOOD_THRESH 96
#define CRC2_GOOD_THRESH 94
#define CRC1_GOOD_THRESH 86
/*
** Absolute thresholds for individual rates with 2 CRCs checking.
*/
#define CRC8_MAX_THRESH 80
#define CRC4_MAX_THRESH 68
#define CRC2_MAX_THRESH 68
#define CRC1_MAX_THRESH 24


word rxc_14400_dec_rate_sel (void)
{
  /* result of the rate decision algorithm (initialized to erasure) */
  word dec_rate = CAI_BLANK_RATE;

/*--------------------------------------------------------------------*/
  byte crc_status = 0x00;
  byte temp = 0x00;
  byte i;
  for(i=0; i<=3; i++)
  {
    temp  = (rxc_fch_status.status[i] & 0x01);
    crc_status |= temp << (3 - i);
  }


  switch (crc_status & RXC_CRC)
  {
    case 1:
      /* Only the eighth rate CRC checks so first test if the
      ** Yamamoto metric for eighth rate passes.
      */
      if (rxc_fch_status.status[DEC_RATE_8TH - 1] & DEC_GOOD8_M)
      {
        if (rxc_fch_status.ser[DEC_RATE_8TH - 1] < CRC8_GOOD_THRESH1 )
        {
            dec_rate = CAI_EIGHTH_RATE;
        }
      }
      else
      {
        if (rxc_fch_status.ser[DEC_RATE_8TH - 1] < CRC8_GOOD_THRESH0 )
        {
            dec_rate = CAI_EIGHTH_RATE;
        }
      }
      break;

    case 2:
      /* Only the quarter rate CRC checks */
        if (rxc_fch_status.ser[DEC_RATE_QTR - 1] < CRC4_GOOD_THRESH )
        {
            dec_rate = CAI_QUARTER_RATE;
        }
        break;

    case 4:
      /* Only the half rate CRC checks */
      if (rxc_fch_status.ser[DEC_RATE_HALF - 1] < CRC2_GOOD_THRESH )
      {
          dec_rate = CAI_HALF_RATE;
      }
      break;

    case 8:
      /* Only the Full rate CRC checks */
      if (rxc_fch_status.ser[DEC_RATE_FULL - 1] < CRC1_GOOD_THRESH )
      {
          dec_rate = CAI_FULL_RATE;
      }
      break;

    case 3:
      /* Both the quarter and eighth rate CRCs check */
      if (rxc_fch_status.ser[DEC_RATE_QTR - 1] < rxc_fch_status.ser[DEC_RATE_8TH - 1] )
      {
        if ( rxc_fch_status.ser[DEC_RATE_QTR - 1] < CRC4_MAX_THRESH )
          dec_rate = CAI_QUARTER_RATE;
      }
      else
      {
        if ( rxc_fch_status.ser[DEC_RATE_8TH - 1] < CRC8_MAX_THRESH )
          dec_rate = CAI_EIGHTH_RATE;
      }
      break;

    case 5:
      /* Both the half and eighth rate CRCs check */
      if (rxc_fch_status.ser[DEC_RATE_HALF - 1] < rxc_fch_status.ser[DEC_RATE_8TH - 1] )
      {
        if ( rxc_fch_status.ser[DEC_RATE_HALF - 1] < CRC2_MAX_THRESH )
          dec_rate = CAI_HALF_RATE;
      }
      else
      {
        if ( rxc_fch_status.ser[DEC_RATE_8TH - 1] < CRC8_MAX_THRESH )
          dec_rate = CAI_EIGHTH_RATE;
      }
      break;

    case 6:
      /* Both the quarter and half rate CRCs check */
      if (rxc_fch_status.ser[DEC_RATE_QTR - 1] < rxc_fch_status.ser[DEC_RATE_HALF - 1] )
      {
        if ( rxc_fch_status.ser[DEC_RATE_QTR - 1] < CRC4_MAX_THRESH )
          dec_rate = CAI_QUARTER_RATE;
      }
      else
      {
        if ( rxc_fch_status.ser[DEC_RATE_HALF - 1] < CRC2_MAX_THRESH )
          dec_rate = CAI_HALF_RATE;
      }
      break;

    case 9:
      /* Both the full and eighth rate CRCs check */
      if (rxc_fch_status.ser[DEC_RATE_FULL - 1] < rxc_fch_status.ser[DEC_RATE_8TH - 1] )
      {
        if ( rxc_fch_status.ser[DEC_RATE_FULL - 1] < CRC1_MAX_THRESH )
          dec_rate = CAI_FULL_RATE;
      }
      else
      {
        if ( rxc_fch_status.ser[DEC_RATE_8TH - 1] < CRC8_MAX_THRESH )
          dec_rate = CAI_EIGHTH_RATE;
      }
      break;

    case 10:
      /* Both the quarter and full rate CRCs check */
      if (rxc_fch_status.ser[DEC_RATE_QTR - 1] < rxc_fch_status.ser[DEC_RATE_FULL - 1] )
      {
        if ( rxc_fch_status.ser[DEC_RATE_QTR - 1] < CRC4_MAX_THRESH )
          dec_rate = CAI_QUARTER_RATE;
      }
      else
      {
        if ( rxc_fch_status.ser[DEC_RATE_FULL - 1] < CRC1_MAX_THRESH )
          dec_rate = CAI_FULL_RATE;
      }
      break;

    case 12:
      /* Both the full and half rate CRCs check */
      if (rxc_fch_status.ser[DEC_RATE_FULL - 1] < rxc_fch_status.ser[DEC_RATE_HALF - 1] )
      {
        if ( rxc_fch_status.ser[DEC_RATE_FULL - 1] < CRC1_MAX_THRESH )
          dec_rate = CAI_FULL_RATE;
      }
      else
      {
        if ( rxc_fch_status.ser[DEC_RATE_HALF] < CRC2_MAX_THRESH )
          dec_rate = CAI_HALF_RATE;
      }
      break;

    case 0:
      break;

    default:
      MSG_ERROR("Bad CRC status %d", (crc_status & RXC_CRC),0,0);
      break;
  }
  /* return the determined rate or an erasure */
  return (dec_rate);

} /* rxc_14400_dec_rate_sel */

#ifdef FEATURE_EVRC_RDA
/*===========================================================================

FUNCTION RXC_EVRC_DEC_RATE_SEL

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Decoder rate.

SIDE EFFECTS
  None.

===========================================================================*/
word rxc_evrc_dec_rate_sel ( void )
{

  const word SER2_THRESH = 60;
  /* Threshold for determining half rate */
  const word SER4_THRESH = 60;
  /* Threshold for determining quarter rate */
  const word SER8_THRESH = 64;
  /* Threshold for determing eighth rate */
  const word DSER_THRESH = 10;

  /* result of the rate decision algorithm */
  word dec_rate;

/*--------------------------------------------------------------------*/

  /* Force changes for EVRC service option */

  /* bit pattern is q1 q2 q4 q8 c1 c2 c4 c8 */

 rxc_fch_status.status[DEC_RATE_QTR - 1] = (rxc_fch_status.status[DEC_RATE_QTR - 1] & (~DEC_GOOD4_M));
  rxc_fch_status.ser[DEC_RATE_QTR - 1] = 200;

  if ( rxc_fch_status.status[DEC_RATE_FULL - 1] & DEC_CRC1_M )
  {
    if ( rxc_fch_status.status[DEC_RATE_HALF - 1] & DEC_CRC2_M )
    {
      if ( rxc_fch_status.ser[DEC_RATE_FULL - 1] <= rxc_fch_status.ser[DEC_RATE_HALF - 1] + 15 )
      {
        /* full rate */
        dec_rate = VOC_1_RATE;

      }
      else
      {
        if ( (rxc_fch_status.status[DEC_RATE_8TH-1] & DEC_GOOD8_M) && (rxc_fch_status.ser[DEC_RATE_8TH-1] + 12 < rxc_fch_status.ser[DEC_RATE_HALF - 1] ) )
        {
          /* Eigth rate */
          dec_rate = VOC_8_RATE;
        }
        else
        {
          /* half rate */
          dec_rate = VOC_2_RATE;
        }
      }
    }/* if CRC2 passes */
    else
    {
      /* CRC2 fails */

      if ( rxc_fch_status.ser[DEC_RATE_FULL - 1] <= 77 )
      {

        if ( (rxc_fch_status.status[DEC_RATE_8TH-1] & DEC_GOOD8_M   ) &&
             (rxc_fch_status.ser[DEC_RATE_8TH-1] + 23 < rxc_fch_status.ser[DEC_RATE_FULL - 1] ) )
        {
          /* Eigth rate */
          dec_rate = VOC_8_RATE;
        }
        else
        {
          /* full rate */
          dec_rate = VOC_1_RATE;
        }

      }/* if ser1 < 77 */
      else
      {
        /* erasure  */
        dec_rate = VOC_0_RATE;

      }/* ser1 > 77 */

    }/* else CRC2 fails */

  }/* if CRC1 passes */
  else if ( (rxc_fch_status.status[DEC_RATE_HALF - 1] & DEC_CRC2_M) && (rxc_fch_status.ser[DEC_RATE_HALF - 1] <=SER2_THRESH) )
  {
    /* if CRC1 failed and CRC2 passes and ser2 < 60 */

    if( !(rxc_fch_status.status[DEC_RATE_QTR - 1] & DEC_GOOD4_M ) &&
        !(rxc_fch_status.status[DEC_RATE_8TH - 1] & DEC_GOOD8_M ) )
    {
      /* half rate */
      dec_rate = VOC_2_RATE;
    }
    else if (rxc_fch_status.status[DEC_RATE_8TH - 1] & DEC_GOOD8_M )
    {
      if( rxc_fch_status.ser[DEC_RATE_HALF - 1] <= rxc_fch_status.ser[DEC_RATE_8TH - 1] + DSER_THRESH )
      {
        /* half rate */
        dec_rate = VOC_2_RATE;
      }
      else
      {
        /* eighth rate */
        dec_rate = VOC_8_RATE;
      }
    }
    else if( rxc_fch_status.ser[DEC_RATE_HALF - 1] <= rxc_fch_status.ser[DEC_RATE_QTR - 1] + DSER_THRESH )
    {
      /* half rate */
      dec_rate = VOC_2_RATE;
    }
    else
    {
      /* fourth rate */
      dec_rate = VOC_4_RATE;
    }

  }
  else if ( (rxc_fch_status.status[DEC_RATE_QTR - 1] & DEC_GOOD4_M) && (rxc_fch_status.status[DEC_RATE_8TH - 1] &DEC_GOOD8_M) )
  {
    /* branch killed */

    if ( (rxc_fch_status.ser[DEC_RATE_8TH - 1] < rxc_fch_status.ser[DEC_RATE_QTR - 1] ) && (rxc_fch_status.ser[DEC_RATE_8TH - 1] <=SER8_THRESH ) )
    {
      dec_rate = VOC_8_RATE;
    }
    else if ( (rxc_fch_status.ser[DEC_RATE_QTR - 1] < rxc_fch_status.ser[DEC_RATE_8TH - 1] ) && (rxc_fch_status.ser[DEC_RATE_QTR - 1] <=SER4_THRESH ) )
    {
      dec_rate = VOC_4_RATE;
    }
    else
    {
      /* Erasure */
      dec_rate = VOC_0_RATE;
    }
  }

  else if ( (rxc_fch_status.status[DEC_RATE_QTR - 1] & DEC_GOOD4_M) && (rxc_fch_status.ser[DEC_RATE_QTR - 1] <=SER4_THRESH) )
  {
    /* branch killed */

    dec_rate = VOC_4_RATE;
  }

  else if ( (rxc_fch_status.status[DEC_RATE_8TH - 1] & DEC_GOOD8_M) && (rxc_fch_status.ser[DEC_RATE_8TH - 1] <=SER8_THRESH) )
  {
    dec_rate = VOC_8_RATE;
  }

  else if ( rxc_fch_status.ser[DEC_RATE_FULL - 1] <= 71 )
  {
    /* Drop the full rate probable bit error as an erasure */
    dec_rate = VOC_0_RATE;
  }

  else
  {
    /* Erasure */
    dec_rate = VOC_0_RATE;
  }
  return(dec_rate);

}/* rxc_evrc_dec_rate_sel() */
#endif /* FEATURE_EVRC_RDA */

#ifndef MSM5000_IRAM_FWD /* Moved to internal RAM */
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

word rxc_dec_rate_sel (void)
{
  word dec_rate = RXC_ERASURE_FRAME;
#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif

#if defined (FEATURE_IS2000_P2) && !defined (FEATURE_IS2000_REL_A_SVD) 
  // Use RC of DCCH or FCH based on what is included.  We don't support both here
  switch ( rxc_so.for_dcch.included?rxc_so.for_dcch.rc:rxc_so.for_fch.rc )
  #else
  switch ( (deint_rc_type) rxc_so.for_fch.rc )
#endif /* FEATURE_IS2000_P2 && !FEATURE_IS2000_REL_A_SVD */
  {
    case DEINT_RC1:
    {
#ifdef FEATURE_EVRC_RDA
      if ( rxc_so.pri == CAI_SO_VOICE_EVRC )
      {
        dec_rate = rxc_evrc_dec_rate_sel ();
      }
      else
#endif /* FEATURE_EVRC_RDA */
      {
        dec_rate =  rxc_9600_dec_rate_sel ();
      }
      break;
    }
    case DEINT_RC2:
    {
      dec_rate = rxc_14400_dec_rate_sel ();
      break;
    }

    case DEINT_RC3:
    case DEINT_RC4:
    case DEINT_RC5:
    {
#if defined (FEATURE_IS2000_P2) && !defined (FEATURE_IS2000_REL_A_SVD) 
      // Note: only one of FCH/DCCH supported here
      if ( rxc_so.for_dcch.included )
      {
        dec_rate = rxc_dtx_det_dcch( (dec_rc_type) rxc_so.for_dcch.rc );
      }
      else
#endif /* FEATURE_IS2000_P2 && !FEATURE_IS2000_REL_A_SVD */
      {
        dec_rate = rxc_fch_rda ( (deint_rc_type) rxc_so.for_fch.rc );
      }
      break;
    }

    default:
    {
#if defined (FEATURE_IS2000_P2) && !defined (FEATURE_IS2000_REL_A_SVD) 
      ERR_FATAL("Bad Radio Config %d", 
                rxc_so.for_dcch.included?rxc_so.for_dcch.rc:rxc_so.for_fch.rc, 0,0);
#else
      ERR_FATAL("Bad Radio Config %d", 
                rxc_so.for_fch.rc, 0,0);
#endif /* FEATURE_IS2000_P2 && !FEATURE_IS2000_REL_A_SVD */
      return 0;  //lint !e527 Unreachable -- Added just to satify compiler warning
    }
  }
  return dec_rate;
}
#endif /* MSM5000_IRAM_FWD */



#if defined (FEATURE_IS95B_MDR) || defined (FEATURE_DS_RLP3)
/* For MDR RLP */
/*===========================================================================

FUNCTION RXC_SCC_RATE_SEL_RLP

DESCRIPTION
  Determine the rate of the specified SCC. It is either FULL rate or ERASURE.
  This function returns the rate type rlp_rate_enum_type.


DEPENDENCIES
  None.

RETURN VALUE
  RLP_ERASURE : If the supplemental channel is an erasure.
  RLP_FULL_RATE : If the supplemental channel is full rate.

SIDE EFFECTS
  None.

===========================================================================*/

rlp_rate_enum_type rxc_scc_rate_sel_rlp
(
  rxc_dec_type *buf_ptr,
  byte sup_index
)
{
  rlp_rate_enum_type  frame_rate;   /* this variable is added to avoid some stupid T32 problem */
    /* The lauterbach emulator T32 can not display the source listing properly
       if there are more than one return statements in a function. To avoid that
       problem we use a temp variable */

    frame_rate = RLP_ERASURE; /* Erasure because CRC does not match or SER is too high */

    if( buf_ptr->buf.tc_buf.scc.scc_crc & (0x1 << sup_index) )
    {
     if( rxc_so.rate_set == DEC_RATE_9600 )
     {
        /* CRC matches for that sup channel */
        if( buf_ptr->buf.tc_buf.scc.scc_ser[sup_index] <= RXC_SCC_ERASURE_THRESHOLD )
        {
            /* CRC matches and the SER is below the threshold, hence it is full rate */
            frame_rate = RLP_FULL_RATE;
        }

     } else if( rxc_so.rate_set == DEC_RATE_14400 )
     {
        /* CRC matches for that sup channel */
        if( buf_ptr->buf.tc_buf.scc.scc_ser[sup_index] <= RXC_SCC_ERASURE_THRESHOLD_RS2 )
        {
            /* CRC matches and the SER is below the threshold, hence it is full rate */
            frame_rate = RLP_FULL_RATE;
        }
     } /* if rxc_so.rate_set == DEC_RATE_14400 */

    }/* if CRC passes for SCC */

    return(frame_rate);
}/* rxc_scc_rate_sel_rlp */
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

rxc_tc_sup_rate_type rxc_scc_rate_sel
(
  rxc_dec_type *buf_ptr,
  byte sup_index
)
{
  rxc_tc_sup_rate_type temp;   /* this variable is added to avoid some stupid T32 problem */
  temp = RXC_SUP_ERASURE; /* Erasure because CRC does not match or SER is too high */

#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif
  if ( buf_ptr->buf.tc_buf.scc.scc_crc & (0x1 << sup_index) )
  {
   if ( rxc_so.rate_set == DEC_RATE_9600 )
   {
     /* CRC matches for that sup channel */
     if( buf_ptr->buf.tc_buf.scc.scc_ser[sup_index] <= RXC_SCC_ERASURE_THRESHOLD )
     {
       /* CRC matches and the SER is below the threshold, hence it is full rate */
       temp = RXC_SUP_FULL;
     }
   }
   else if( rxc_so.rate_set == DEC_RATE_14400 )
   {
     /* CRC matches for that sup channel */
     if( buf_ptr->buf.tc_buf.scc.scc_ser[sup_index] <= RXC_SCC_ERASURE_THRESHOLD_RS2 )
     {
       /* CRC matches and the SER is below the threshold, hence it is full rate */
       temp = RXC_SUP_FULL;
     }
   } /* if rxc_so.rate_set == DEC_RATE_14400 */

  }/* if CRC passes for a supplemental channel */

  return temp;
}/* rxc_scc_rate_sel */


#ifdef FEATURE_DETECT_SUP_MUTING
/*===========================================================================

FUNCTION RXC_SUP_MUTE_INIT

DESCRIPTION
  This function initializes the supplemental channel muting detection
  algorithm.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_sup_mute_init( void )
{
int i;

  /* initialize data indicator for each supplemental to 1 */
  for( i = 0; i < DEC_MAX_SCC_NUM; i++ )
  {
    past_di[i] = 1;
    past_sup_status[i] = RLP_ERASURE;
  }
}/* rxc_sup_mute_init() */
#endif /*FEATURE_DETACT_SUP_MUTING*/
#endif /* FEATURE_IS95B_MDR */

#ifdef FEATURE_IS2000_P2
/*========================================================================

FUNCTION RXC_DTX_DET_DCCH

DESCRIPTION This function determines if the current frame is a DTX frame,
an erasure frame, or a full frame in the dedicated control channel.

DEPENDENCIES None

RETURN VALUE enum, either RXC_GOOD_FRAME, RXC_DTX_FRAME, or RXC_ERASURE_FRAME


SIDE EFFECTS None

=========================================================================*/
/* Engineering control variable for debug printing */ 
word rxc_dtx_det_dcch(dec_rc_type rc)
#ifdef FEATURE_ENHANCE_DCCH_DTX_DETECTION
{
  word rate=CAI_BLANK_RATE;
  int32 norm_pc_acc;
  int2 pc_acc = (int2) DEMOD_GET_PC_ACC();
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  uint16 ser = rxc_dcch_status.ser;
  int32  energy = rxc_dcch_status.energy;
#else
  uint16 ser = rxc_fch_status.ser[DEC_RATE_FULL-1];
  int32  energy = rxc_fch_status.energy[DEC_RATE_FULL-1];
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

  int2  debug_values = 0; // for debug only
  int2 crc_value = 0;     // for debug only

  /* Use 128 for fixed point multiplier */
  byte subchan_gain = ffpc_get_subchan_gain_lin();

#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif
  /* The subchan_gain_lin could be zero which means equal power */
  if(subchan_gain == 0)
  {
    subchan_gain = 45;
  }

  /* See if a new pc_acc scaler is pending.  If so, update at the specified time. */
  if ( pending_pc_acc_scaler != pc_acc_scaler )
  {
    qword cur_frame_time;
    
    ts_get_20ms_frame_offset_time( cur_frame_time );
    if ( qw_cmp(pending_pc_acc_scaler_time, cur_frame_time) <= 0 )
    {
      /* If we are at or pass the time to update the pc_acc_scaler, do it now */
      pc_acc_scaler = pending_pc_acc_scaler;
    }
  }

  /* Begin of debug code */
  debug_values = 128 * subchan_gain / 45; 
  if (energy == 0)
  {
    MSG_HIGH ("Fr Energy is 0 for DCCH!", 0, 0, 0);
    /* Frame Energy should never equal to ZERO. 
    who know! It might! when the MS in the handoff situation
    and searcher out of lock, then return DTX
    */
    return CAI_NULL_RATE;
  }
  /* end of debug code */

  /* if pilot is gated, then scale the pc_acc by the gating factor */
  pc_acc = (pc_acc * pc_acc_scaler);//lint !e734 Loss of precision (assignment) (23 bits to 15 bits)

  norm_pc_acc = (128 * pc_acc * 45) / subchan_gain;
 
  /* If CRC passes */
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  if (rxc_dcch_status.status & 0x01)
#else
  if (rxc_fch_status.status[DEC_RATE_FULL-1] & 0x01)
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
  {
    rate = CAI_FULL_RATE;
    crc_value = 1;
  }
  else
  {
    switch (rc)
    {
      case DEC_RC3:
      {
        if (ser < (520 * norm_pc_acc) / (128 * energy) - 380 )
        {
          rate = CAI_NULL_RATE;
        }
        else
        {
          rate = CAI_BLANK_RATE;
        }
        break;
      }

      case DEC_RC4:
      {
        if (ser < (116 * norm_pc_acc) / (128 * energy) - 138 )
        {
          rate = CAI_NULL_RATE;
        }
        else
        {
          rate = CAI_BLANK_RATE;
        }
        break;
      }

      case DEC_RC5:
      {
        if ( (16 * norm_pc_acc) / (128 * energy) > 19 )
        {
          rate = CAI_NULL_RATE;
        }
        else
        {
          rate = CAI_BLANK_RATE;
        }
        break;
      }
      default:
      {
        rate = CAI_BLANK_RATE;
      }
    }
  } 

  if(rxcrda_dtx_msg_enable ) 
  {
    switch( (cai_data_rate_type) rate)
    {
      case CAI_FULL_RATE:
        MSG_HIGH("DCCH FULL pcacc %d ser %d e %d",
             pc_acc,
             ser,
             energy);
        MSG_HIGH("DCCH-SC gain %d, CRC %d",debug_values, crc_value,0 );
        break;
      case CAI_BLANK_RATE:
        MSG_HIGH("DCCH ERAS pcacc %d ser %d e %d",
             pc_acc,
             ser, 
             energy);
        MSG_HIGH("DCCH-SC gain %d, CRC %d",debug_values, crc_value,0 );
        break;
      case CAI_NULL_RATE:
        MSG_HIGH("DCCH DTX pcacc %d ser %d e %d",
             pc_acc,
                 ser, 
                 energy);
        MSG_HIGH("DCCH-SC gain %d, CRC %d",debug_values, crc_value,0 );
        break;

      case CAI_EIGHTH_RATE:
      case CAI_QUARTER_RATE:
      case CAI_HALF_RATE:
        /* Do nothing */
        break;
    }
  }
  return rate;
}
#else
{
  word rate=CAI_BLANK_RATE;
  word corrected_rate;
  byte use_rate = 0;

  /* If CRC passes */
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
// Take care of VP2 here also.
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
  if (rxc_fch_status.status[DEC_RATE_FULL-1] & 0x01)
  {
    rate = CAI_FULL_RATE;
  }
  /* If SER > SER_dtx */
  else if (rxc_fch_status.ser[DEC_RATE_FULL-1] > 
           rxcrda_dcch_ser_dtx_thresholds[rc])
  {
    rate = CAI_NULL_RATE;
  }
  else if (rxc_fch_status.ser[DEC_RATE_FULL-1] < 
           rxcrda_dcch_ser_erasure_thresholds[rc])
  {
    rate = CAI_BLANK_RATE;
  }
  else
  {
    rate = CAI_BLANK_RATE;  /* Pass erasure to the filter */
    use_rate = 0;           /* And use the filters response */
  }

  /* Pass decision into the filter, and use the response if use_rate */
  corrected_rate = rxc_dcch_dtx_filter(rxcrda_convert_to_rxc_rates[rate]);
  if(use_rate)
  {
    rate = rxcrda_convert_to_cai_rates[corrected_rate];
  }

  if(rxcrda_dtx_msg_enable ) 
  {
    switch(rate)
    {
      case CAI_FULL_RATE:
        MSG_HIGH("DCCH FULL crc %d ser %d e %d",
             rxc_fch_status.status[DEC_RATE_FULL-1]& 0x01,
             rxc_fch_status.ser[DEC_RATE_FULL-1], 
             rxc_fch_status.energy[DEC_RATE_FULL-1]);
        break;
      case CAI_BLANK_RATE:
        MSG_HIGH("DCCH ERAS crc %d ser %d e %d",
             rxc_fch_status.status[DEC_RATE_FULL-1]& 0x01,
             rxc_fch_status.ser[DEC_RATE_FULL-1], 
             rxc_fch_status.energy[DEC_RATE_FULL-1]);
        break;
      case CAI_NULL_RATE:
        MSG_HIGH("DCCH DTX crc %d ser %d e %d",
             rxc_fch_status.status[DEC_RATE_FULL-1]& 0x01,
             rxc_fch_status.ser[DEC_RATE_FULL-1], 
             rxc_fch_status.energy[DEC_RATE_FULL-1]);
        break;
    }
  }
  return rate;
}
#endif /* End of FEATURE_ENHANCE_DCCH_DTX_DETECTION */
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_RDA_CORRECTION
/*===========================================================================

FUNCTION RXC_SCH_DTX_FILTER

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
rxc_frame_type
rxc_sch_dtx_filter
(
 rxc_frame_type frame_status
)
{
  rxc_frame_type ret_status;

  ret_status = rxcrda_dtx_filter( frame_status, &rxcrda_sch_dtx_fix_counter);
  return ret_status;
}

/*===========================================================================

FUNCTION RXC_DCCH_DTX_FILTER

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
rxc_frame_type
rxc_dcch_dtx_filter
(
 rxc_frame_type frame_status
)
{
  rxc_frame_type ret_status;

  ret_status = rxcrda_dtx_filter( frame_status, &rxcrda_dcch_dtx_fix_counter);
  return ret_status;
}
/*===========================================================================

FUNCTION RXCRDA_DTX_FILTER

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
/* Engineering control variables for the filter */
byte rxcrda_dtx_filter_upper_bound = 3;
byte rxcrda_dtx_filter_enable = 0;

rxc_frame_type
rxcrda_dtx_filter
(
 rxc_frame_type frame_status,
 int8 *counter
)
{
  if (rxcrda_dtx_filter_enable == 0)
  {
    return frame_status;
  }

  if (frame_status == RXC_DTX_FRAME)
  {
    (*counter)++;
    if ((*counter) > rxcrda_dtx_filter_upper_bound)
    {
      (*counter) = (int8) rxcrda_dtx_filter_upper_bound;
    }
  }
  else if (frame_status == RXC_ERASURE_FRAME)
  {
    (*counter)--;
    if ((*counter) < -1)
    {
      (*counter) = -1;
    }
  }
  else if (frame_status == RXC_GOOD_FRAME)
  {
    (*counter) -= 3;
    if ((*counter) < -1)
    {
      (*counter) = -1;
    }
  }

  if (frame_status == RXC_ERASURE_FRAME)
  {
    if ((*counter) <= 0)
    {
      return RXC_ERASURE_FRAME;
    }
    else
    {
      return RXC_DTX_FRAME;
    }
  }
  else
  {
    return frame_status;
  }
}
#endif /* FEATURE_RDA_CORRECTION */

/*========================================================================

FUNCTION RXC_DTX_DET_SCH

DESCRIPTION This function determines if the current frame is a DTX frame,
an erasure frame, or a full frame.

DEPENDENCIES None

RETURN VALUE enum, either RXC_GOOD_FRAME, RXC_DTX_FRAME, or RXC_ERASURE_FRAME


SIDE EFFECTS None

=========================================================================*/
#ifdef FEATURE_IS2000_SCH
boolean rxcrda_sch_dtx_disable = 0;

#define MAX_SQRT_INT32 46340 /* =floor(sqrt(2^31-1)), max number which can be squared to give a 32b signed value */
#define DEFAULT_Q      9     /* default Q precision for some constants */
rxc_frame_type rxc_dtx_det_sch(dec_rc_type rc, boolean crc_pass)
{
  /*lint -save -e703 -e704 Shift left or right of signed quantity (Too dangerous to fix for now) */
  /* configuration dependent variables */
  int16 shift_NtIo;  /* shift factor for NtIo in threshold compare */
  int16 pi_codeR;     /* pi * coding rate (DEFAULT_Q = Q9) */
  int16 pi_EbNt;      /* pi * Eb/Nt at Shannon limit (Q9) */
  
  /* frame accumulations from mDSP */
  int16 IQAcc     = (int16) DEMOD_GET_SCH_IQ_ACC();
  int16 RtNtIo    = (int16) MUXMSM_GET_SCH_RT_NT_IO();
  int16 RtEcpIo   = (int16) MUXMSM_GET_SCH_WTD_PILOT();
  
  /* FPC set point from outer loop */
  static int16 fpc_setpt = 256;  /* applied with one frame latency to accommodate delay
                                  * in asserting new set point after frame decode (i.e. 
                                  * decoding+SW delays)
                                  */


  /* algorithm variables */
  rxc_frame_type frame; 
  int32          schEcpNtInst;     /* instantaneous pilot to noise ratio for the SCH */
  int32          schEcpNtInstLim;  /* limited instantaneous pilot to noise ratio for the SCH */
  static int32   schEcpNtFilt = 1; /* filtered pilot to noise ratio for the SCH */
  int32          schEcpNtFiltLim;  /* limited filtered pilot to noise ratio for the SCH */
  
  int16          metricL_Q;        /* cumulative q-precision of left side metrics */
  int16          metricR_Q;        /* cumulative q-precision of right side metrics */
  int32          metricL;          /* metric for left side of comparison */
  int32          metricR;          /* metric for right side of comparison */
  
  int32          IQAccRtNtIo;      /* scaled scared ratio of IQAcc/rtNtIo (debug value) */
  int32          IQAccRtNtIo_Q;    /* Q-precision of scaled scared ratio of IQAcc/rtNtIo (debug value) */
  static int32   IQAccRtNtIo_ref   = 0; /* stored value of this result */
  static int16   IQAccRtNtIo_Q_ref = 0; /* stored value of this result */

  uint8          ltu_crc;    /* CRC check results for LTUs with convolutional coding */
                             /* LTU masks are 0x03, 0xf, 0xff, 0xff for rates 4x, 8x, 16x, and 32x */
  
#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif

  if (IQAcc == 0)
  {
    /* if IQAcc==0, fingers are out of lock or SCH is shut down */
    frame = RXC_DTX_FRAME;
  }
  else if (rxcrda_sch_dtx_disable)
  {
    frame = crc_pass ? RXC_GOOD_FRAME : RXC_ERASURE_FRAME;
  } 
  else
  {
    /* perform DTX detection algorithm */
    
    /* ----- compute pilot to noise ratio  ----- */
    schEcpNtInst    = ((int32)RtEcpIo<<8) / (int32) (RtNtIo+1); /* +1 avoids div-by-zero */
    schEcpNtInst    = schEcpNtInst > MAX_SQRT_INT32 ? MAX_SQRT_INT32 : schEcpNtInst;
                      /* saturate to max integer (MAX_SQRT_INT32) which allows squaring to 32b signed value */
    schEcpNtInst   *= schEcpNtInst;
    
    /* ----- prepare scaling ----- */
    /* Want to scale NtIo by 72/(Num Chips per QPSK symbol).
     * This is done with a 6x of rtNtIo (effectively 36x of NtIo), 
     * and then a shift after squaring.
     */
    switch( rc )
    {
      case DEC_RC3: 
        /* rate=1/4 */
        pi_EbNt    = 167;  /* pi*Eb/Nt with DEFAULT_Q = 9 */
        pi_codeR   = 402;  /* pi*rate with DEFAULT_Q = 9  */
        shift_NtIo = 5;    /* 36/(1<<shift_NtIo)=72/64, for 64 chip (1x rate) symbol */
        break;
        
      case DEC_RC4: 
        /* rate=1/2 */
        pi_EbNt    = 402;  /* pi*Eb/Nt with DEFAULT_Q = 9  */
        pi_codeR   = 804;  /* pi*rate with DEFAULT_Q = 9 */
        shift_NtIo = 6;    /* 36/(1<<shift_NtIo)=72/128, for 128 chip (1x rate) symbol */
        break;
        
      case DEC_RC5: 
        /* rate=3/8 */
        pi_EbNt    = 274; /* pi*Eb/Nt with DEFAULT_Q = 9  */
        pi_codeR   = 603; /* pi*rate with DEFAULT_Q = 9  */
        shift_NtIo = 5;   /* 36/(1<<shift_NtIo)=72/64, for 62 chip (1x rate) symbol */
        break;
        
      default:
        MSG_ERROR("Bad RC for SCH DTX, RC enum=%d", rc, 0, 0);
        pi_EbNt    = 0;
        pi_codeR   = 0;
        shift_NtIo = 0;
        break;
    }
    /* modify shift_NtIo to scale for actual data rate */
    switch ( rxc_sch.sch_rate )
    {
      case DEC_SCH_RATE_1X:
        shift_NtIo -= 0;
        ltu_crc     = 0x00;
        break;
        
      case DEC_SCH_RATE_2X:
        shift_NtIo -= 1; 
        ltu_crc     = 0x00;
        break;
        
      case DEC_SCH_RATE_4X:
        shift_NtIo -= 2; 
        ltu_crc = (rxc_sch.code_type == DEC_TURBO) ? 0 : (0x03 & rxc_sch_status.conv_sch_status.ltu_crc);
        break;
        
      case DEC_SCH_RATE_8X:
        shift_NtIo -= 3; 
        ltu_crc = (rxc_sch.code_type == DEC_TURBO) ? 0 : (0x0f & rxc_sch_status.conv_sch_status.ltu_crc);
        break;
        
      case DEC_SCH_RATE_16X:
        shift_NtIo -= 4; 
        ltu_crc = (rxc_sch.code_type == DEC_TURBO) ? 0 : (0xff & rxc_sch_status.conv_sch_status.ltu_crc);
        break;
        
#ifdef FEATURE_32X_SCH_DATA_RATE
      case DEC_SCH_RATE_32X:
        shift_NtIo -= 5; 
        ltu_crc = (rxc_sch.code_type == DEC_TURBO) ? 0 : (0xff & rxc_sch_status.conv_sch_status.ltu_crc);
        break;
#endif /* FEATURE_32X_SCH_DATA_RATE */
        
      default:
        ltu_crc = 0;
        MSG_ERROR("Bad rate for SCH DTX, rate enum=%d.", rxc_sch.sch_rate, 0, 0);
        break;
    }
    
    /* ----- prepare reference (filtered) and instant Ecp/Nt values ----- */
    /* shift both down to acceptable range. Only the ratio matters, so they must be shifted same amount */
    schEcpNtInstLim = schEcpNtInst >> 1;
    schEcpNtInstLim = schEcpNtInstLim > schEcpNtFilt ? schEcpNtFilt : schEcpNtInstLim;
    schEcpNtFiltLim = schEcpNtFilt;
    while ((schEcpNtInstLim > MAX_SQRT_INT32) || (schEcpNtFiltLim > MAX_SQRT_INT32)) 
    {
      schEcpNtFiltLim >>= 4;
      schEcpNtInstLim >>= 4;
    }
    
    /* ----- compute left side of comparison ----- */
    /* to maintain dynamic range, each step shifts right to saturate to max integer (MAX_SQRT_INT32) 
     * which allows squaring to 32b signed value. Shifts are counted to undo later.
     */
    
    /* compute IQAcc to scaled noise ratio and raise to 4th power */
    metricL   = ((int32)IQAcc << 16) / (6 * RtNtIo +1);
    metricL_Q = 16; /* initial Q factor of this computation, +16 from <<16 */
    while (metricL > MAX_SQRT_INT32) {metricL >>= 4; metricL_Q -= 4; }
    
    /* square the metric: (IQAcc/(6*rtNtIo))^2 */
    metricL   *= metricL;
    metricL_Q += metricL_Q;
    while (metricL > MAX_SQRT_INT32) {metricL >>= 6; metricL_Q -= 6; }
    
    /* adjust by residual (pwr of 2) NtIo scaling: (chips-per-symb)/72 * (IQAcc/rtNtIo)^2 */
    metricL_Q    -= shift_NtIo;
    IQAccRtNtIo   = metricL;
    IQAccRtNtIo_Q = metricL_Q;
    
    /* square the metric: ( (chips-per-symb)/72 * (IQAcc/rtNtIo)^2 )^2 */
    metricL   *= metricL;
    metricL_Q += metricL_Q;
    while (metricL > MAX_SQRT_INT32) {metricL >>= 6; metricL_Q -= 6; }
    
    /* apply filtered Ecp/Nt weighting */
    metricL *= schEcpNtFiltLim;
    
    /* ----- compute right side of comparison ----- */
    /* dynamic range maintained as for left side. */
    
    /* compute reference "expected" metric, from setpoint (FPC enabled) or last frame (FPC disabled) */
    if (ffpc_sch_fpc_enabled())
    {
      /* use current set point to determine the "expected" IQ_ACC/rtNtIo ratio */
      /* compute intermediate threshold in Q17: fpcsetpt (Q8) * pi_codeR(Q9) */
      metricR    = (int32)fpc_setpt * pi_codeR;
      metricR_Q  = 17;
      metricR   -= 1<<17;
      while (metricR > MAX_SQRT_INT32) {metricR >>= 6; metricR_Q -= 6; }
    }
    else
    {
      /* use previous (IQ_ACC/rtNtIo)^2 ratio to determine "expected" IQ_ACC/rtNtIo ratio */
      /* also ensure that this is not less than expected at Shannon (given by pi_EbNt) */
      metricR   = IQAccRtNtIo_ref;
      metricR_Q = IQAccRtNtIo_Q_ref;
      
      if (DEFAULT_Q > IQAccRtNtIo_Q_ref)
      {
        if (pi_EbNt >> (DEFAULT_Q-IQAccRtNtIo_Q_ref) >= metricR) //lint !e702 Shift right of signed quantity
        {
          metricR   = pi_EbNt;
          metricR_Q = DEFAULT_Q;
        }
      }
      else
      {
        if (pi_EbNt >= metricR >> (DEFAULT_Q-IQAccRtNtIo_Q_ref))
        {
          metricR   = pi_EbNt;
          metricR_Q = DEFAULT_Q;
        }
      }
      metricR -= 1<<metricR_Q;
    }
      
    /* apply Ecp/Nt scaling */
    metricR *= schEcpNtInstLim;
    metricR += schEcpNtFiltLim << metricR_Q;
    
    /* ----- equalize Q factors on both sides of the comparison ----- */
    if (metricR_Q > metricL_Q)
    {
      metricR   >>= (metricR_Q - metricL_Q);
      metricR_Q   = metricL_Q; 
    }
    if (metricL_Q > metricR_Q)
    {
      metricL   >>= (metricL_Q - metricR_Q);
      metricL_Q   = metricR_Q; 
    }
    
    /* ----- perform comparison and make final decision ----- */
    /* note: must use >= to ensure that metricL=metricR=0 yields erasure */
    frame = (metricL >= metricR) ? RXC_ERASURE_FRAME : RXC_DTX_FRAME;
    /* use LTUs (conv. code) to override decision to erasure if some pass */
    frame = (ltu_crc) ? RXC_ERASURE_FRAME : frame;
    /* override the decision if CRC passes */
    frame = (crc_pass) ? RXC_GOOD_FRAME : frame;

    if(rxcrda_dtx_msg_enable ) 
    {
      MSG_HIGH ("--DTX: IQAcc %-10d, RtNtIo %-10d, RtEcpIo %-10d",
              IQAcc, RtNtIo, RtEcpIo);
      MSG_HIGH ("  DTX: setpt %-10d, EcpNtF %-10d, EcpNtI  %-10d",
              fpc_setpt, schEcpNtFilt, schEcpNtInst);
      MSG_HIGH ("  DTX: IQNt  %-10d, IQNtQ  %-10d, metL    %-10d",
              IQAccRtNtIo, IQAccRtNtIo_Q, metricL);
      MSG_HIGH ("  DTX: metR  %-10d, metRQ  %-10d, crc/dec %-02x",
              metricR, metricR_Q, (crc_pass<<4) + frame);
    }
    
    /* ----- update Ecp/Nt filter ----- */
    if (frame != RXC_DTX_FRAME)
    {
      /* IIR filter with gain coeff=1/2. */
      schEcpNtFilt += (schEcpNtInst - schEcpNtFilt)>>1; 
            
      /* Update reference IQ_ACC/rtNtIo ratio */
      IQAccRtNtIo_ref   = IQAccRtNtIo;
      IQAccRtNtIo_Q_ref = (int16) IQAccRtNtIo_Q;
    }
  }
  
  /* grab set point to use for next frame */
  fpc_setpt = ffpc_get_sch_setpt_linQ8();  
  
  return frame;
  /*lint -restore */
}
#endif /* FEATURE_IS2000_SCH */

#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  
/*========================================================================

FUNCTION RXCRDA_DCCH_INIT

DESCRIPTION This function returns the initial threshole frame quality value.

DEPENDENCIES Shoud be called after the receiving of the ECAM 

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxcrda_dcch_init( void )
{
  int32  temp;
  byte fpc_dcch_min_pcb_setpt = 0;

  /* compute the PCB min set point */
  fpc_dcch_min_pcb_setpt = ffpc_get_dcch_min_setpt_db() + (2 * ffpc_get_subchan_gain_db());//lint !e734 Loss of precision (assignment) (9 bits to 8 bits)
  
  if (fpc_dcch_min_pcb_setpt > 132)
  {
    temp = (fpc_setpt_db_lin_133_255[fpc_dcch_min_pcb_setpt - 132] + 40);
    temp = temp * temp;
    rxcrda_dcch_quality_thresh = (256 * temp) / 49;
  }
  else
  {
    temp = (fpc_setpt_db_lin_0_132[fpc_dcch_min_pcb_setpt] + 46);
    temp = temp * temp;
    rxcrda_dcch_quality_thresh = (256 * temp) / 2025;
  }
}

/*========================================================================

FUNCTION RXC_DCCH_QUALITY_DET

DESCRIPTION This function returns the quality of the channel to assist in
channel setup and maintenance.

DEPENDENCIES None

RETURN VALUE enum rxc_dcch_quality_type: RXC_GOOD_CHANNEL, RXC_POOR_CHANNEL

SIDE EFFECTS None

=========================================================================*/
rxc_dcch_quality_type rxc_dcch_quality_det(rxc_frame_type  rda_decision )
{
  uint32 temp32bit = 0;

  int2 pc_acc = (int2) DEMOD_GET_PC_ACC();
  int2 fch_nt_io = DEMOD_GET_FCH_NT_IO();
  
  /* Check if fch_nt_io is zero( when out of lock in handoff), 
     then return RXC_POOR_CHANNEL 
  */
  if (fch_nt_io == 0)
  {
#ifdef FEATURE_PLT
#error code not present
#else
    MSG_HIGH("fch_nt_io val is 0",0,0,0);
    rxc_dcch_frame_quality = RXC_POOR_CHANNEL;
#endif /* FEATURE_PLT */
    return rxc_dcch_frame_quality;
  }
  /* if pilot is gated, then scale the pc_acc by the gating factor */
  pc_acc = (pc_acc * pc_acc_scaler);//lint !e734 Loss of precision (assignment) (23 bits to 15 bits)

  /* check if the CRC is good */
  if (rda_decision == RXC_GOOD_FRAME)
  {
    rxc_dcch_frame_quality = RXC_GOOD_CHANNEL;
    rxcrda_dcch_quality_thresh = ffpc_get_dcch_hw_setpt();
  }
  else if (rda_decision == RXC_ERASURE_FRAME)
  {
    rxc_dcch_frame_quality = RXC_POOR_CHANNEL;
  }
  else if (rda_decision == RXC_DTX_FRAME)
  {
    if ((rxc_so.for_dcch.rc == CAI_RC_3) || (rxc_so.for_dcch.rc == CAI_RC_4))
    {
      temp32bit = ((uint32)(((pc_acc * pc_acc)/9600) * 48114))/((uint2)fch_nt_io);
    }
    else if (rxc_so.for_dcch.rc == CAI_RC_5)
    {
      temp32bit = ((uint32)(((pc_acc * pc_acc)/14400) * 48114))/((uint2)fch_nt_io);
    }
    else  /* Invalid RC */
    {
      ERR_FATAL ("Bad DCCH Radio Config %d", rxc_so.for_dcch.rc, 0, 0);
    }

    if (temp32bit > (uint32)rxcrda_dcch_quality_thresh )
    {
      rxc_dcch_frame_quality = RXC_GOOD_CHANNEL;
    }
    else
    {
       rxc_dcch_frame_quality = RXC_POOR_CHANNEL;
    }
  }
  if(rxcrda_dtx_msg_enable ) 
  {
    switch(rxc_dcch_frame_quality)
    {
      case RXC_POOR_CHANNEL:
        MSG_HIGH("DCCH Bad %d %d %d", rxcrda_dcch_quality_thresh, pc_acc,
                 fch_nt_io);
        break;
      case RXC_GOOD_CHANNEL:
        MSG_HIGH("DCCH OK %d %d %d", rxcrda_dcch_quality_thresh, pc_acc,
                 fch_nt_io);
        break;
    }
  }
  return rxc_dcch_frame_quality;
}
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION   */

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
)
{
  /* First compute the time it should be effective (2 frames later) and save it */
  ts_get_20ms_frame_offset_time( pending_pc_acc_scaler_time );
  qw_inc( pending_pc_acc_scaler_time, 2L );

  /* Now store the scaling value */
  /* Note: gating_rate_none is same as gating_rate_one */
  if ( gating_rate == CAI_GATING_RATE_NONE )
  {
    gating_rate = CAI_GATING_RATE_ONE;
  }
  /* Gating rate comes in 0, 1, or 2.  Need to convert to scaler value we want. */
  pending_pc_acc_scaler = 1 << gating_rate;
}
#endif // FEATURE_IS2000_P2

