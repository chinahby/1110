/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   C D M A   T R A N S M I T   S E R V I C E   O P T I O N S   M O D U L E

GENERAL DESCRIPTION
  Provides an interface to service option specific functions
   
EXTERNALIZED FUNCTIONS
  txc_voc_traffic - Get traffic data (primary or secondary) from the vocoder
  txc_loopback_traffic -  Get loopback data bits (primary only) from the 
  Loopback Service Option layer.
  txc_mar_traffic - Get traffic data (primary or secondary) from Markov frame 
  error rate tests
  txc_tdso_traffic - Get traffic data (primary) from TDSO

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1991 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/txcso.c_v   1.16   24 Jul 2002 14:49:40   hkulkarn  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/txcso.c#3 $ $DateTime: 2008/07/18 10:34:36 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV. Removed unnecessary variables.
                   Reduced debug string sizes for ULC effort.
07/03/08   vlc     Fixed post-increment issue in function txc_voc_traffic()
                   for FEATURE_JCDMA.
07/25/05   rkc     Fixed lint errors.
09/28/04   sr      Merged in changes from 6550 2009 build
07/30/04   vlc     Added support for rel C TDSO.
07/29/04   jrp     Properly checked return value of voc_frame_cmd.
06/16/04   sr      Merged in changes from 6500 5004 build
04/27/04   vlc     Following changes were done in function txc_voc_traffic(), 
                   ret_val was renamed to rlp_need_tx for clarity sake, F3
                   message was moved to include more information, and if 
                   rlp_need_tx is TRUE, do not increase vocoder bandwidth for 
                   next frame.
04/20/04   jrp     Fixed lint warnings.
08/25/03   vlc     Removed shared rate_set from txc_so (txc_so_type) to avoid 
                   confusion between FCH and DCCH rate sets.  Now calculate
                   rate set from RC as it is needed.
04/29/03   vlc     As an enhancement, do not dim vocoder for RLP signalling
                   frames if DCCH is also mapped for RLP data.
03/28/03   sy/vlc  Added logic to support different transmit schemes for    
                   different types of RLP signalling frames in an SVD call. 
07/24/02   hrk     Added support for VP2. 
                   Introduced new function txc_tdso_sec_traffic().
05/15/02   bn      Initialize the FCH TDSO frame size correctly
04/04/02   hrk     Removed capping MAX_RATE to 1/2 in SVD. Implemented new algo.
03/15/02   hrk     Reducing vocoder MAX RATE if RLP has signalling to send.
02/27/02   hrk     Added SVD support.
06/15/01   day     Mainlined FEATURE_IS2000
                   Delete VOC_FULL_ONLY
06/07/01   kk      Mixed RS support for TDSO interface added.
05/18/01   kk      RS2 TDSO support.
05/11/01   fc      Added support for DCCH STDSO.
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00 
           hrk     Added support for FEATURE_IS2000_P2
03/20/01   ych     Merged JCDMA Features.
11/13/00   bgc     (merge) Seperated the FCH and SCH tdso blocks while
                   packing Reverse TDSO frames. Also fixed the fr_time
                   during FCH tdso frames. 
11/12/00   sr      (merge) Added support for Full TDSO
11/10/00   jc      This file was part of txc.c. See txccommon.c for full 
                   change history

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <memory.h>
#include "comdef.h"
#include "customer.h"
#include "queue.h"
#include "bio.h"
#include "task.h"
#include "tx.h"
#include "txc.h"
#include "err.h"
#include "log.h"
#include "cai.h"
#include "ran.h"
#include "mar.h"
#include "ts.h"
#include "rf.h"
#include "ulpn.h"
#include "rxc.h"
#include "loopback.h"

#include "msm_drv.h"

#ifdef FEATURE_PLT
#error code not present
#endif

#include "ts.h"
#include "mccdma.h"

#ifdef FEATURE_IS2000_TDSO
  #include "tdso.h"
#endif // FEATURE_IS2000_TDSO


#include "txccommon.h"
#include "txcmux.h"
#include "txctraffic.h"
#include "txcaccess.h"
#include "txcso.h"
#include "muxmdsp.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* FULL Rate Frames Only Compile Option */

#ifdef FEATURE_IS2000_TDSO
  tdso_frame_block_type fch_tdso_frame_block;
  tdso_frame_block_type sch_tdso_frame_block;
#endif // FEATURE_IS2000_TDSO



#ifdef FEATURE_JCDMA
/* Voice frame activity test */
LOCAL struct {
   boolean enable;
   word    cnt;
   word    voc_1;
   word    voc_2;
   word    voc_8;
} txc_voc_activity = { FALSE, 0, 0, 0, 0 };
#endif /* FEATURE_JCDMA */

#ifdef FEATURE_IS2000_REL_A_SVD
/* txc_rlp_sig_priority is used to keep track of priorities in sending RLP control 
** frames when RLP is the secondary SO in a SVD call.  The purpose of the
** implementation is to improve voice quality in a SVD call.
*/
txc_rlp_sig_priority_type txc_rlp_sig_priority;
#endif /* FEATURE_IS2000_REL_A_SVD */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/* <EJECT> */
/***************************************************************************/
/*                      TRAFFIC State Procedures                           */
/***************************************************************************/

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
boolean dim_last = FALSE;

cai_data_rate_type txc_voc_traffic
(
  byte **data_ptr,
    /* Returns pointer to data */
  txc_sig_frame_type signext
    /* Signaling mode for next packet */
)
{
  voc_rate_type      max_rate;    /* Maximum rate for next frame */
  voc_rate_type      min_rate;    /* Mimimum rate for next frame */
  voc_rate_type      voc_rate;    /* Current vocoder data rate */
  cai_data_rate_type data_rate;   /* Data rate of current frame */
#ifdef FEATURE_IS2000_REL_A_SVD
  boolean            rlp_need_tx = FALSE;
#endif /* FEATURE_IS2000_REL_A_SVD */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (signext == BLANK)
  {
    max_rate = VOC_0_RATE;  /* Nothing from vocoder, please */
    min_rate = VOC_0_RATE;
    MSG_MED ("Req B&B",0,0,0);
  }
  else
  {
    /* Set up vocoder max and min for next packet */
    if ((signext == DIM) && (!dim_last))
    {
      max_rate = VOC_2_RATE; /* Dim-and-burst --> force half rate */
      min_rate = VOC_8_RATE;
      dim_last = TRUE;
      MSG_MED ("Req D&B",0,0,0);
    }
    else
    {
      dim_last = FALSE;

      max_rate = VOC_1_RATE; /* Full range of rates */

#ifdef FEATURE_IS2000_REL_A_SVD
      /* Check if RLP has signalling to send.
      ** Do this only if there is no L3 signalling to be sent.
      */
      if (
          (txc_so.sec.so == CAI_SO_PPP_PKT_DATA_3G) &&
          (!(txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_DCCH))
         )
      {
        /* dsrlp_any_sig_to_send() returns TRUE if RLP has signalling to send.
        ** Special handling is required when txc_rlp_sig_priority.curr_fr_val 
        ** is:
        ** 
        ** DSRLP_TX_CONTINUOUS:    Initial delay OK then continuous TX
        ** 
        ** This action takes a minimum delay of 3 frames plus the frames 
        ** required to transmit the RLP signalling sequence (1 frame or 
        ** multiple frames, unknown to the MUX layer).
        */                                                                       
        rlp_need_tx = dsrlp_any_sig_to_send (&txc_rlp_sig_priority.curr_fr_val);
        if (rlp_need_tx == TRUE) 
        {
          /* Dim vocoder to send RLP signalling.
          */
          max_rate = VOC_2_RATE; /* Pri-and-Sec frame, force half rate */
          if( txc_rlp_sig_priority.delay_fr_cnt != 0)
          {
            txc_rlp_sig_priority.delay_fr_cnt--;
          }
          else if ( 
                   (txc_rlp_sig_priority.curr_fr_val == DSRLP_TX_CONTINUOUS) &&
                   (txc_rlp_sig_priority.curr_fr_val != txc_rlp_sig_priority.prev_fr_val) 
                  )
          {
            txc_rlp_sig_priority.delay_fr_cnt = TXC_VOC_DIM_DELAY;
          }
          MSG_MED ("RLPSigDimVoc,dly=%d,cur=%d,prv=%d",
                   txc_rlp_sig_priority.delay_fr_cnt,
                   txc_rlp_sig_priority.curr_fr_val,
                   txc_rlp_sig_priority.prev_fr_val
                  );
        }
        else
        {
          txc_rlp_sig_priority.delay_fr_cnt = 0;
          txc_rlp_sig_priority.curr_fr_val = DSRLP_TX_NONE;

        } /* end if (rlp_need_tx == TRUE) */

        /* Set current to previous.  
        **  - In case RLP returns TRUE, if we don't set current to previous,
        **    then we set delay frame count to TXC_VOC_DIM_DELAY 
        **    on the frame which delay frame count is decremented to zero.
        **  - In case RLP returns FALSE, current frame value is reset to NONE so 
        **    previous should be reset to NONE as well.
        */
        txc_rlp_sig_priority.prev_fr_val = txc_rlp_sig_priority.curr_fr_val;

      } /* end if (txc_so.sec.so == CAI_SO_PPP_PKT_DATA_3G) */

#endif /* FEATURE_IS2000_REL_A_SVD */

      min_rate = VOC_8_RATE;

      /* if dipswitch 8 is ON force full rate voice */
      if (BIO_GET_SW( BIO_SW8_M ))
      {
        max_rate = VOC_1_RATE;
        min_rate = VOC_1_RATE;
      }
    }
  }

#ifdef FEATURE_JCDMA
/* We look at the txc_voc_activity and report vocoder rate set
 */ 
  if (txc_voc_activity.enable)
  {
    if (txc_voc_activity.cnt < txc_voc_activity.voc_1)
    {
      voc_rate = VOC_1_RATE;
    }
    else if (txc_voc_activity.cnt < (txc_voc_activity.voc_1 +
                                     txc_voc_activity.voc_2))
    {
      voc_rate = VOC_2_RATE;
    }
    else
    {
      voc_rate = VOC_8_RATE;
    }
    txc_voc_activity.cnt = (++txc_voc_activity.cnt
                            % (txc_voc_activity.voc_1 +
                               txc_voc_activity.voc_2 +
                               txc_voc_activity.voc_8));
    
    max_rate = voc_rate;
    min_rate = voc_rate; 
  }
#endif /* FEATURE_JCDMA */

  /* Get the data from the vocoder */
  voc_rate = voc_tx_cmd( max_rate, min_rate, data_ptr );


  /* We use a neat trick here to see whether we asked for a dim frame
  ** for the next frame but got less than full rate for the current frame
  ** and used up all the signalling bits waiting to be transmitted.
  ** If this is TRUE then we change the order for the next frame with
  ** the vocoder and don't affect voice quality over the next frame.
  **
  ** Do not allow this neat trick if DSRLP also require bandwidth to
  ** send RLP signalling data (for SVD calls.)
  */

#ifdef FEATURE_IS2000_REL_A_SVD
  if (rlp_need_tx == TRUE) 
  {
    MSG_MED ("RLPSigDimVoc,dly=%d,dimrate=%d,vocrate=%d",
             txc_rlp_sig_priority.delay_fr_cnt,
             max_rate,
             voc_rate
            );
  }
  else
#endif /* FEATURE_IS2000_REL_A_SVD */
  {
  if((txtc.frame.sig == DIM) && (voc_rate != VOC_1_RATE) &&
     ((txc_tx.buf.pos + 88) >= txc_tx.buf.len) )
  {
    /* if dipswitch 8 is ON force full rate voice */
    if( BIO_GET_SW( BIO_SW8_M ) ) 
    {
      if( voc_frame_cmd (VOC_1_RATE, VOC_1_RATE) != VOC_DONE_S)
      {
        ERR ("Vocoder not responding",0,0,0);
      }
    }
    else
    {
      MSG_MED ("Req Full Range",0,0,0);
                      /* ask for full range of rates */
      if (voc_frame_cmd(VOC_1_RATE, VOC_8_RATE) != VOC_DONE_S)
      {
        MSG_ERROR("Voc Cmd Fail",0,0,0);
      }
    }
  }
  }


  switch (voc_rate)
  {
    case VOC_8_RATE:
      data_rate = CAI_EIGHTH_RATE;
      break;
    case VOC_4_RATE:
      data_rate = CAI_QUARTER_RATE;
      break;
    case VOC_2_RATE:
      data_rate = CAI_HALF_RATE;
      break;
    case VOC_1_RATE:
      data_rate = CAI_FULL_RATE;
      break;
    default:
      data_rate = CAI_BLANK_RATE;
      break;
  }

  return (data_rate);

} /* txc_voc_traffic */

/* <EJECT> */
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

cai_data_rate_type txc_loopback_traffic
(
  byte **data_ptr,
    /* Returns pointer to loopback data bits */
  txc_sig_frame_type signext
    /* Signaling mode for this frame */
)
{
  cai_data_rate_type max_rate;
    /* Maximum rate for this frame */
  cai_data_rate_type data_rate;
    /* Rate of current frame */
  static cai_frame_type data;
   /* Place to store loopback data bits */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (signext == BLANK)
  {
    /* Nothing from Loopback Service Option layer please */
    max_rate = CAI_BLANK_RATE;
  }
  else if (signext == DIM)
  {
    /* Dim-and-burst --> force half-rate */
    max_rate = CAI_HALF_RATE;
  }
  else
  {
    max_rate = CAI_FULL_RATE;
  }

  /* Get the loopback data bits from the Loopback Service Option layer */
  data_rate = loopback_tx( max_rate, &data ); //lint !e545 Suspicious use of &

  /* Set address of loopback data */
  *data_ptr = data;

  return (data_rate);

} /* txc_loopback_traffic */

/* <EJECT> */
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

cai_data_rate_type txc_mar_traffic
(
  byte **data_ptr
    /* Returns pointer to data */
)
{
  static byte        fer_data [ENC_14400_FULL_RATE_SIZE];
  dword              frame_num;
  byte               markov_rate;
  cai_data_rate_type data_rate;     /* Data rate of current frame */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *data_ptr = fer_data;            /* return address of data */

  /*  Frame seed for Markov, frame number is +1 because we are building the
   *  frame before the tx_traffic_isr has occurred.
   */
  frame_num = qw_lo( txc_tx.frame_cnt );

/* txc_mar_traffic() is called before incrementing the frame count by 1 in
** txc isr. Hence we need to increment it by one for markov data generation.
*/
  frame_num++;

  markov_rate = mar_tx( fer_data, frame_num, FALSE );

  switch (markov_rate)
  {
    case MAR_8_RATE:
      data_rate = CAI_EIGHTH_RATE;
      break;
    case MAR_4_RATE:
      data_rate = CAI_QUARTER_RATE;
      break;
    case MAR_2_RATE:
      data_rate = CAI_HALF_RATE;
      break;
    case MAR_1_RATE:
      data_rate = CAI_FULL_RATE;
      break;
    default:
      data_rate = CAI_BLANK_RATE;
      break;
  }

  return (data_rate);

} /* txc_mar_traffic */

#ifdef FEATURE_IS2000_TDSO
/* <EJECT> */
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

cai_data_rate_type txc_tdso_traffic
(
  byte **data_ptr,
    /* Returns pointer to data */
  txc_sig_frame_type signext
    /* Signaling mode for next packet */
)
{
   cai_data_rate_type data_rate;   /* Data rate of current frame */

   if( signext == DIM )
   {
      // request 1 8TH rate frame from TDSO
      fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_rate = TDSO_RATE_EIGHTH;
      data_rate = CAI_EIGHTH_RATE;
   }
   else if( signext == BLANK )
   {
      fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_rate = TDSO_BLANK;
      data_rate = CAI_BLANK_RATE;
   }
   else
   {
     // TDSO rate defines if double or single size PDU
     // will be used. No matter what the rate set is 
     // TDSO_RATE should be set to 1, which indicates
     // single pdu.

     fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_rate = TDSO_RATE_1;
     data_rate = CAI_FULL_RATE;
   }
   fch_tdso_frame_block.num_frames = 1;
   fch_tdso_frame_block.fdch_fr_mux =
     (tdso_mux_type) (ENC_RC_TO_RATE_SET( txc_so.rev_fch.rc ));
   fch_tdso_frame_block.fr_time = qw_lo( txc_tx.frame_cnt ) + 1;

#ifdef FEATURE_IS2000_P2
   if (txc_so.rev_dcch.included)
   {
#ifdef FEATURE_IS2000_REL_A_SVD
     if (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_DCCH)
#endif /* FEATURE_IS2000_REL_A_SVD */
     {
       fch_tdso_frame_block.fdch_fr_mux =
         (tdso_mux_type) (ENC_RC_TO_RATE_SET( txc_so.rev_dcch.rc ));
       fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_channel = TDSO_DCCH;
     }
   }
   else
#endif /* FEATURE_IS2000_P2 */
   {
#ifdef FEATURE_IS2000_REL_A_SVD
     if (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_FCH)
#endif /* FEATURE_IS2000_REL_A_SVD */
     {
       fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_channel = TDSO_FCH;
     }
   }

   if( fch_tdso_frame_block.fdch_fr_mux == TDSO_RATE_SET_1 )
   {
      fch_tdso_frame_block.frame_type.rev_arr[ 0 ].mux_pdu_type =
         TDSO_MUX_PDU_1;
   }
   else
   {
      fch_tdso_frame_block.frame_type.rev_arr[ 0 ].mux_pdu_type =
         TDSO_MUX_PDU_2;
   }

   fch_tdso_frame_block.frame_type.rev_arr[ 0 ].srvc_type =
      TDSO_PRIMARY_SRVC;

   fch_tdso_frame_block.frame_type.rev_arr[ 0 ].max_frame_size =
      tdso_data_len[ fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_rate ]
                   [ fch_tdso_frame_block.fdch_fr_mux ];

   if ( tdso_build_tx_frames(REV_LINK, &fch_tdso_frame_block ) != TDSO_VALID)
   {
     MSG_ERROR("TDSO Build Failed",0,0,0);
   }
   *data_ptr = fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_item_ptr;

   if(fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_rate == TDSO_BLANK)  
   {
     data_rate = CAI_BLANK_RATE;
     *data_ptr = NULL;
   }

   return (data_rate);
} /* txc_tdso_traffic */

#ifdef FEATURE_IS2000_REL_A_SVD
/* <EJECT> */
/*===========================================================================

FUNCTION TXC_TDSO_SEC_TRAFFIC

DESCRIPTION
  Get traffic data (Secondary) from TDSO

DEPENDENCIES
  Currently TDSO is supported on either R-FCH or R-DCCH.

RETURN VALUE
  Returns the data rate

SIDE EFFECTS
  None

===========================================================================*/

cai_data_rate_type txc_tdso_sec_traffic
(
  byte **data_ptr,
    /* Returns pointer to data */
  boolean msg_on_channel
    /* msg pending on signaling channel*/
)
{
   cai_data_rate_type data_rate;   /* Data rate of current frame */
   word  sec_num_bits = 0;

   sec_fch_num_bits  = 0;
#ifdef FEATURE_IS2000_P2
   sec_dcch_num_bits  = 0;
#endif

   if (msg_on_channel)
   {
      // for SVD always blank S-TDSO data.
      fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_rate = TDSO_BLANK;
      data_rate = CAI_BLANK_RATE;
   }
   else
   {
     // TDSO rate defines if double or single size PDU
     // will be used. No matter what the rate set is 
     // TDSO_RATE should be set to 1, which indicates
     // single pdu.

#ifdef FEATURE_IS2000_REL_A_SVD
     // Even though Secondary traffic will be less than FULL RATE,
     // if a FRAME rate of less than FULL rate is specified,
     // TDSO will always build a FULL RATE frame.
#endif // FEATURE_IS2000_REL_A_SVD

     fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_rate = TDSO_RATE_1;
     data_rate = CAI_FULL_RATE;
   }

   fch_tdso_frame_block.num_frames  = 1;
   fch_tdso_frame_block.fdch_fr_mux = 
     (tdso_mux_type) (ENC_RC_TO_RATE_SET( txc_so.rev_fch.rc ));
   fch_tdso_frame_block.fr_time     = qw_lo( txc_tx.frame_cnt ) + 1;
   fch_tdso_frame_block.frame_type.rev_arr[ 0 ].srvc_type = TDSO_SECONDARY_SRVC;

   if (fch_tdso_frame_block.fdch_fr_mux == TDSO_RATE_SET_1)
   {
     fch_tdso_frame_block.frame_type.rev_arr[ 0 ].mux_pdu_type = TDSO_MUX_PDU_1;
   }
   else
   {
     fch_tdso_frame_block.frame_type.rev_arr[ 0 ].mux_pdu_type = TDSO_MUX_PDU_2;
   }

   // initialize number of bits in sec. traffic
#ifdef FEATURE_IS2000_P2
   if (txc_so.rev_dcch.included &&
       (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_DCCH))
   {
     fch_tdso_frame_block.fdch_fr_mux = 
       (tdso_mux_type) (ENC_RC_TO_RATE_SET( txc_so.rev_dcch.rc ));
     fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_channel = TDSO_DCCH;
     sec_num_bits = txc_tdso_sec_frame_size (MCTRANS_PHY_CHAN_DCCH,
                                             msg_on_channel);
   }
   else if (txc_so.rev_fch.included)
#endif /* FEATURE_IS2000_P2 */
   {
     if (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_FCH)
     {
       fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_channel = TDSO_FCH;
       sec_num_bits = txc_tdso_sec_frame_size (MCTRANS_PHY_CHAN_FCH,
                                               msg_on_channel);
     }
   }

   if (sec_num_bits == 0)
   {
     fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_rate = TDSO_BLANK;
     data_rate = CAI_BLANK_RATE;
   }

   // Even though FRAME RATE is specified as TDSO_RATE_1 (FULL rate),
   // specify the correct FRAME SIZE limit. 
   fch_tdso_frame_block.frame_type.rev_arr[ 0 ].max_frame_size =
      sec_num_bits;

   if (tdso_build_tx_frames(REV_LINK, &fch_tdso_frame_block ) != TDSO_VALID)
   {
     MSG_ERROR("TDSO Build Failed",0,0,0);
   }
   *data_ptr = fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_item_ptr;

   if (fch_tdso_frame_block.frame_type.rev_arr[ 0 ].fr_rate == TDSO_BLANK)  
   {
     data_rate = CAI_BLANK_RATE;
     sec_num_bits = 0;
     *data_ptr = NULL;
   }

#ifdef FEATURE_IS2000_P2
   if (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_DCCH)
   {
     sec_dcch_num_bits = sec_num_bits;
   }
   else if (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_FCH)
#endif // FEATURE_IS2000_P2
   {
     sec_fch_num_bits = sec_num_bits;
   }

   return (data_rate);
} /* txc_tdso_sec_traffic */
#endif // FEATURE_IS2000_REL_A_SVD
#endif // FEATURE_IS2000_TDSO

#ifdef FEATURE_JCDMA
/*===========================================================================

FUNCTION TXC_SET_VOC_ACTIVITY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void txc_set_voc_activity
(
  boolean enable,   /* enable/disable voice activity test */
  word voc_1,       /* # of full rate */
  word voc_2,       /* # of 1/2 rate */
  word voc_8        /* # of 1/8 rate */
)
{
  txc_voc_activity.enable = enable;
  txc_voc_activity.voc_1 = voc_1;
  txc_voc_activity.voc_2 = voc_2;
  txc_voc_activity.voc_8 = voc_8;

  txc_voc_activity.cnt = 0;
}
#endif /* FEATURE_JCDMA */


