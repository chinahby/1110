/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C D M A     M U L T I P L E X I N G    M O D U L E

GENERAL DESCRIPTION
  Contains frame building routines
   
EXTERNALIZED FUNCTIONS
  txc_pri - provides the primary traffic to the Multiplex sublayer
  dependant on what the current primary service option is
  txc_data_pump_build_frame - builds the next frame to be transmitted if 
  data pump has been initialized
  txc_build_frame - builds the next frame to be transmitted
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1991 through 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/txcmux.c_v   1.34   18 Sep 2002 14:32:12   hkulkarn  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/txcmux.c#2 $ $DateTime: 2008/07/18 10:34:36 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV and FEATURE_MAX_PWR_PROTECTION_DEBUG.
                   Reduced debug string sizes and removed empty if statements
                   for ULC effort.
09/19/06   vlc     Fixed lint errors.
06/19/06   vlc     Moved prim_num_bits from txctraffic.c to this file.  It is
                   only used in this file.  
                   Added inclusion of mccdma.h
                   Added support for voice encryption.
                   Moved function txc_pri_num_bits() outside FEATURE_IS2000_REL_A_SVD.
01/10/06   rkc     Change ERR_FATAL to MSG_ERROR when prim_data_ptr should not
                   be NULL.
07/25/05   rkc     Fixed lint errors.
04/29/05   vlc     Added 4GV support under FEATURE_VOC_4GV.
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.                                   
09/28/04   sr      Merged in changes from 6550 2009 build
07/30/04   vlc     Added support for rel C TDSO.
06/18/04   ank     Mainlined FEATURE_GPSONE_DBM.
04/20/04   jrp     Fixed lint warnings.
10/22/03   bn      Fixed compilation warning errors when using RVCT compiler 
09/09/03   dna     Merged porting changes for MSM6500
08/25/03   vlc     Removed shared rate_set from txc_so (txc_so_type) to avoid 
                   confusion between FCH and DCCH rate sets.  Now calculate
                   rate set from RC as it is needed.
04/29/03   vlc     Comment out debug statement occuring every frame.
03/28/03   vlc     Added logic in function txc_rlp_sec_data_rate() to support
                   continuous transmit priority for RLP signalling frames in
                   SVD calls.
                   Used new constant TXC_VOC_DIM_DELAY instead of hard coded
                   value.
02/25/03   jrp     Changed two ERR_FATAL to MSG_ERROR.
12/05/02   vlc     Fixed Markov SO handling in function txc_pri() when
                   FEATURE_IS2000_REL_A_SVD is enabled.
09/17/02   hrk     Before deciding max RLP rate on DCCH, making sure that MS
                   is not in control hold state.
09/04/02   bn,hrk  In Rel.A when FEATURE_IS2000_REL_A_SVD is turned off,
                   prim_dcch_data_ptr and prim_dcch_rate were not initialized.
07/24/02   hrk     Added support for VP2. Mainlined from -r1.28.1.7
06/25/02   HQ      Added FEATURE_CDSMS.
05/15/02   bn      Fixed the incorrect of tdso frame type
04/18/02   az      Added support for new voice service option SMV (SO 56)
04/18/02   lcc     When CAI_SO_NULL is active, set rate to 1/8 even if 
                   txtc.nulls is not true.
03/04/02   bn      Set the DCCH mux category of DTX frame type correctly for RS1 or RS2
02/27/02   bn,hrk  Added SVD support.
02/11/02   hrk     Keeping R-FCH, R-DCCH enabled during preambles, no harm as
                   R-FCH, R-DCCH gain during preamble is set to 0.
10/30/01   bn      Added support for Release A RDCH logging
09/06/01   hrk     Fixed reporting of dim-and-burst frames in frame types logging.
                   Created new function - build_null_frame() to build null 1800/1200
                   primary traffic frame.
08/28/01   lh      Back out SI bits changes for Release A Addendum 2.
08/02/01   jrp     Added support for SO54, SO55.
06/15/01   day     Mainlined FEATURE_IS2000
05/11/01   vlc     Merged in datapump changes for PLT (ks.)
04/20/01   sr      Merged in from MSM5100 archive
04/19/01   tc      Merged with MSM_MUX1X.01.00.01 
           lcc     Corrected some merge errors.
04/18/01   sr      Merged with MSM_MUX1X.01.00.00 
           lcc     Added support for FEATURE_IS2000_P2
03/27/01   vlc     Merged to rev 1.10 of MSM archive.
           lcc     Corrected tranmission of preambles in PCG units so that it won't
                   be repeated for "n" number of frames.
           hrk     Enhancements for Max Power Protection.
           lcc     Merged in support for SO 17 (CAI_SO_VOICE_13K_IS733).
           hrk     Enhancements for Max Power Protection and added debug msgs.
03/22/01   lh      Moved si_adj setting to the beginning of build_blank_frame().
03/14/01   lh      Fixed wrong SI bits settings for Release A.
03/07/01   lh      Support for frame formatting based on p_rev_in_use. (LAC
                   changes for REL A)
03/01/01   tc      Merged MSM_MUX1X.00.00.15.
01/31/01   tc      Merged GPS support.
12/13/00   hrk     Enhancements for Max Power Protection.
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
#include "task.h"
#include "assert.h"
#include "voc.h"
#include "tx.h"
#include "txc.h"
#include "err.h"
#include "bit.h"
#include "log.h"
#include "cai.h"
#include "ran.h"
#include "mar.h"
#include "msg.h"
#include "ts.h"
#include "parm.h"
#include "ulpn.h"
#include "rxc.h"

#include "mux.h"

#include "msm_drv.h"

#ifdef FEATURE_PLT
#error code not present
#endif

#include "ts.h"
#include "txccommon.h"
#include "txcmux.h"
#include "txctraffic.h"
#include "txcaccess.h"
#include "txcso.h"
#include "tdso.h"

#ifdef FEATURE_IS2000_REL_A_SVD
#include "dsrlp.h"
#endif /* FEATURE_IS2000_REL_A_SVD */

#include "muxmdsp.h"
#include "mcctdsodv.h"
#include "mccdma.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Constants for building Traffic Channel Frames */
#define TC_BLANK 168             /* Signaling bits for blank and burst */
#define TC_DIM2   88             /* 1/2 rate dim-and-burst */
#define TC_DIM4  128             /* 1/4 rate dim-and-burst */
#define TC_DIM8  152             /* 1/8 rate dim-and-burst */

word prim_num_bits = 0;          /* Num of bits in FCH primary frame */

#if !defined (FEATURE_DIAG_V7_LOG_IFACE)
  /* Log buffer for accumulated MUX frame log */
  txc_log_mux_buf_type txc_log_mux_buf;
#else
  q_type txc_log_mux1_q;             /* for logging traffic frame statistics */
  word txc_mux1_cnt;  /* Counter for mux1 entries */
  txc_log_mux1_type txc_log_mux1_pool[ TXC_NUM_MUX1_BUFS ];
#endif

#ifdef FEATURE_MAX_PWR_PROTECTION
/*lint -esym(752,txc_max_pwr_limited_dtx_sch) Variable used in some ifdef's*/
extern boolean txc_max_pwr_limited_dtx_sch;
#endif

byte build_null_frame ( void );
word txc_pri_num_bits (cai_data_rate_type);
extern txc_sig_frame_type next_fch_sig_type;

#ifdef FEATURE_IS2000_REL_A_SVD
cai_data_rate_type txc_det_frame_rate (void);
dsrlp_rate_enum_type txc_rlp_sec_data_rate (word, boolean);
#endif // FEATURE_IS2000_REL_A_SVD


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/* <EJECT> */
/*===========================================================================

FUNCTION TXC_LOG_MUX1

DESCRIPTION
  This function logs the Reverse Traffic Channel Mux data.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void txc_log_mux1
(
  byte val  /* value to log */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* increment relevant parameter */
  ( void )parm_inc( val, 1 );

#if !defined (FEATURE_DIAG_V7_LOG_IFACE)
  if (txc_log_mux_buf.count == 0)
  {
    ts_get((unsigned long *)txc_log_mux_buf.entry.hdr.ts);
  }

  /* Fill in log data */
  txc_log_mux_buf.entry.vals[txc_log_mux_buf.count] = val;
  txc_log_mux_buf.count++;
  txc_log_mux_buf.entry.hdr.len += sizeof(txc_log_mux_buf.entry.vals[0]);


  if (txc_log_mux_buf.count == LOG_R_MUX1_CNT)
  {
    TXC_SEND_MUX1_LOG();  /*lint !e717 Ignore while (0) */
  }
#else

  if( txc_mux1_ptr == NULL)
  {

    /* need new item from queue */
    if ( ( txc_mux1_ptr =
              (txc_log_mux1_type *) q_get( &txc_log_mux1_q )) == NULL )
    {
      MSG_MED( "No buf from log_mux1_q", 0, 0, 0 );
    }
    else
    {
      ts_get( (unsigned long *)(txc_mux1_ptr->entry.hdr.ts ));
      /* set timestamp in new buffer */
      txc_mux1_cnt = 0;                       /* reset num entries to 0 */
    }
  }

  if( txc_mux1_ptr != NULL ) {                 /* if we have a buffer */
    /* log value */
    txc_mux1_ptr->entry.vals[ txc_mux1_cnt++ ] = val;   /* log this value */

    if( txc_mux1_cnt == LOG_R_MUX1_CNT )       /* entry is full - log it */
    {
      log_put( &txc_mux1_ptr->desc );          /* put item on log q */
      txc_mux1_ptr = NULL;                     /* need new buffer */
    }
  }
#endif /* !defined (FEATURE_DIAG_V7_LOG_IFACE) */
} /* txc_log_mux1 */


/* <EJECT> */
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
void txc_pri (void)
{
  static byte null_data[3] = {0xFF, 0xFF, 0xFF};
#ifdef TMOBILE
#error code not present
#endif /* TMOBILE */

#ifdef FEATURE_IS2000_REL_A_SVD
  
  /* initialize FCH primary rate and data ptr
  */
  prim_rate     = CAI_BLANK_RATE;
  prim_data_ptr = NULL;

#ifdef FEATURE_IS2000_P2
  /* initialize DCCH primary rate and data ptr
  */
  prim_dcch_rate     = CAI_BLANK_RATE;
  prim_dcch_data_ptr = NULL;
#endif /* FEATURE_IS2000_P2 */
#endif // FEATURE_IS2000_REL_A_SVD

#ifndef FEATURE_IS2000_REL_A
  switch (txc_so.pri)
#else
  switch (txc_so.pri.so)
#endif /* FEATURE_IS2000_REL_A */
  {
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
    case CAI_SO_VOICE_13K_IS733:
    case CAI_SO_VOICE_13K:
#endif
    case CAI_SO_VOICE_IS96A:
    case CAI_SO_VOICE_EVRC:
#ifdef FEATURE_SO_VOICE_SMV
    case CAI_SO_VOICE_SMV:
#endif
#ifdef FEATURE_VOC_4GV
    case CAI_SO_VOICE_4GV_NB: 
    case CAI_SO_VOICE_4GV_WB:
#endif /* FEATURE_VOC_4GV */
    {
      txc_sig_frame_type signext = txtc.frame.sig;

      /* service option is voice - get transmit vocoder packet */

#ifdef FEATURE_IS2000_REL_A_SVD
      //
      // REMOVE ERR_FATAL after debug. But need to prevent this condition.
      //
      if (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_DCCH)
      {
        ERR_FATAL("Voice on DCCH not supported", 0, 0, 0);
      }

#ifdef FEATURE_IS2000_P2
      prim_dcch_data_ptr = NULL;
#endif

      if (txtc.cur_dsch == TXC_REV_DCCH)
      {
        /* signaling on DCCH, voice on FCH: don't DIM/BLANK vocoder
        */
        signext = NORM;
      }
#endif // FEATURE_IS2000_REL_A_SVD

      txc_data_rate = prim_rate = txc_voc_traffic (&prim_data_ptr, signext);

#ifdef FEATURE_IS2000_REL_A_SVD
      if (prim_rate == CAI_BLANK_RATE)
      {
        prim_data_ptr = NULL;
      }

#endif // FEATURE_IS2000_REL_A_SVD
      prim_num_bits = txc_pri_num_bits (prim_rate);

#ifdef TMOBILE
#error code not present
#endif
      break;
    }

    case CAI_SO_LOOPBACK_SO55:
    case CAI_SO_LOOPBACK_13K:
    case CAI_SO_LOOPBACK:

#ifdef FEATURE_IS2000_REL_A_SVD

#ifdef FEATURE_IS2000_P2
      /* SO not supported on DCCH
      */
      prim_dcch_data_ptr = NULL;
#endif

      if (txc_so.sec.so != CAI_SO_NULL)
      {
        /* Loopback SO not supported in SVD configuration
        */
        MSG_ERROR("SO %d not supp in SVD", txc_so.pri.so,0,0);

        /* Code Review: what else to do?
        */
      }

      /* Fill up buffer with Loopback rx packet data bits, return rate */
      if (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_FCH)
      {
        txc_data_rate = prim_rate =
          txc_loopback_traffic( &prim_data_ptr, txtc.frame.sig );
      }
      else
      {
        ERR_FATAL("SO %d not supp on DCCH", txc_so.pri.so,0,0);

        /* Code Review: what else to do?
        */
      }
#else
      txc_data_rate = prim_rate =
        txc_loopback_traffic( &prim_data_ptr, txtc.frame.sig );
#endif

#ifdef TMOBILE
#error code not present
#endif
      break;

    case CAI_SO_MARKOV_SO54:
    case CAI_SO_MARKOV_13K:
    case CAI_SO_MARKOV:
    case CAI_SO_RS1_MARKOV:
#if defined(FEATURE_CDSMS) || defined(FEATURE_UASMS)
    case CAI_SO_SMS:
    case CAI_SO_RS2_SMS:
#endif /* defined(FEATURE_CDSMS) || FEATURE_UASMS */
    case CAI_SO_RS2_MARKOV:
#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif /* FEATURE_MDR_DPT_MARKOV */
    {
      /* fill up the buffer with Markov data, returns rate */
#ifdef FEATURE_IS2000_REL_A_SVD

#ifdef FEATURE_IS2000_P2
      /* SO not supported on DCCH
      */
      prim_dcch_data_ptr = NULL;
#endif

      if (txc_so.sec.so != CAI_SO_NULL)
      {
        /* Markov SO not supported in SVD configuration
        */
        MSG_ERROR("SO %d not supp in SVD", txc_so.pri.so,0,0);

        /* Code Review: what else to do?
        */
      }

      /* Fill up buffer with Markov rx packet data bits, return rate */
      if (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_FCH)
      {
        prim_rate = txc_data_rate = txc_mar_traffic( &prim_data_ptr );
      }
      else
      {
        ERR_FATAL("SO %d not supp on DCCH", txc_so.pri.so,0,0);

        /* Code Review: what else to do?
        */
      }
#else
      {
        prim_rate = txc_data_rate = txc_mar_traffic( &prim_data_ptr );
      }
#endif // FEATURE_IS2000_REL_A_SVD

      break;
    }

    case CAI_SO_NULL:
    {
      /* no valid service option - either send preamble or nulls - assume
         preamble ( all zeros full rate ) is inside packet */
      prim_data_ptr = null_data;
      if (txtc.frame.sig == BLANK)
      {
        txc_data_rate = prim_rate = CAI_BLANK_RATE;
      }
      else
      {
        txc_data_rate = prim_rate = CAI_EIGHTH_RATE;
      }
      break;
    }

#ifdef FEATURE_OTASP_OTAPA
    case CAI_SO_RS1_OTAPA:
    case CAI_SO_RS2_OTAPA:
      break;
#endif /* FEATURE_OTASP_OTAPA */

#ifdef FEATURE_IS2000_TDSO
    case CAI_SO_TDSO:
    case CAI_SO_FULL_TDSO:
    {
      txc_sig_frame_type signext = txtc.frame.sig;

#ifdef FEATURE_IS2000_REL_A_SVD
      if (
          ((txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_FCH) &&
           (txtc.cur_dsch == TXC_REV_DCCH)) ||
          ((txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_DCCH) &&
           (txtc.cur_dsch == TXC_REV_FCH))
         )
      {
        /* signaling and pri on different physical channels
        ** don't DIM/BLANK
        */
        signext = NORM;
      }

      if (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_FCH)
      {
        txc_data_rate = prim_rate =
           txc_tdso_traffic (&prim_data_ptr, signext);
#ifdef FEATURE_IS2000_P2
        prim_dcch_data_ptr = NULL;
#endif /* FEATURE_IS2000_P2 */
      }
#ifdef FEATURE_IS2000_P2
      else if (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_DCCH)
      {
        txc_data_rate = prim_dcch_rate =
           txc_tdso_traffic (&prim_dcch_data_ptr, signext);
        prim_data_ptr = NULL;
      }
#endif /* FEATURE_IS2000_P2 */
#else /*FEATURE_IS2000_REL_A_SVD */
#ifdef FEATURE_IS2000_P2
      if (txc_so.rev_dcch.included)
      {
        txc_data_rate = prim_dcch_rate =
               txc_tdso_traffic (&prim_dcch_data_ptr, signext);
      }
      else
#endif /* FEATURE_IS2000_P2 */
      {
        txc_data_rate = prim_rate = 
               txc_tdso_traffic (&prim_data_ptr, signext);
      }
#endif // FEATURE_IS2000_REL_A_SVD

      break;
    }
#endif // FEATURE_IS2000_TDSO

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

#ifdef FEATURE_IS2000_REL_A_SVD
    case CAI_SO_PPP_PKT_DATA_3G:
    {
      dsrlp_rate_enum_type max_rlp_rate = DSRLP_RATE_BLANK; /* no data */

#ifdef FEATURE_IS2000_P2
      if (txc_so.rev_fch.included)
#endif /* FEATURE_IS2000_P2 */
      {
#ifdef FEATURE_IS2000_REL_A
        if (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_FCH)
#endif /* FEATURE_IS2000_REL_A */
        {
          /* R-FCH assigned and RLP is primary SO on R-FCH
          */
          if (!(txc_tx.active_msg && (txtc.cur_dsch == TXC_REV_FCH)))
          {
            /* no msg pending on FCH, all bandwidth to data
            */
            max_rlp_rate = DSRLP_RATE_1;
          }
        }
      }
      txc_tx_process_fundich_data3g (MCTRANS_PHY_CHAN_FCH,
                                     max_rlp_rate,
                                     DSRLP_PRIMARY_SRVC);

#ifdef FEATURE_IS2000_P2
      max_rlp_rate = DSRLP_RATE_BLANK; /* no data */

      if (txc_so.rev_dcch.included
#ifdef FEATURE_IS2000_REL_A
          && (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_DCCH)
#endif /* FEATURE_IS2000_REL_A */
         )
      {
        /* R-DCCH assigned and RLP is primary SO on R-DCCH
        */
        boolean control_hold = txtc.gating_rate != CAI_GATING_RATE_NONE;

        if (!control_hold &&
            !(txc_tx.active_msg && (txtc.cur_dsch == TXC_REV_DCCH)))
        {
          /* MS not in control hold state and no msg pending on DCCH,
          ** all bandwidth to data
          */
          max_rlp_rate = DSRLP_RATE_1;
        }
      }
      txc_tx_process_fundich_data3g (MCTRANS_PHY_CHAN_DCCH,
                                     max_rlp_rate,
                                     DSRLP_PRIMARY_SRVC);
#endif /* FEATURE_IS2000_P2 */
      break;
    }
#endif /* FEATURE_IS2000_REL_A_SVD */

    default:
    {
      break;
    }
  }
}

/*===========================================================================

FUNCTION TXC_PRI_NUM_BITS

DESCRIPTION
  This function returns the number of bits corresponding to the frame rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
word txc_pri_num_bits
(
 cai_data_rate_type rate
)
{
  /* Called for VOICE SOs, must be on FCH. */
  if (ENC_RC_TO_RATE_SET( txc_so.rev_fch.rc ) == ENC_RATE_9600)
  {
    if (rate == CAI_FULL_RATE)
    {
      return MUX1_FULL_PRI_LEN;
    }
    else if (rate == CAI_HALF_RATE)
    {
      return MUX1_HALF_PRI_LEN;
    }
    else if (rate == CAI_QUARTER_RATE)
    {
      return MUX1_QTR_PRI_LEN;
    }
    else if (rate == CAI_EIGHTH_RATE)
    {
      return MUX1_EIGHTH_PRI_LEN;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    if (rate == CAI_FULL_RATE)
    {
      return MUX2_FULL_PRI_LEN;
    }
    else if (rate == CAI_HALF_RATE)
    {
      return MUX2_HALF_PRI_LEN;
    }
    else if (rate == CAI_QUARTER_RATE)
    {
      return MUX2_QTR_PRI_LEN;
    }
    else if (rate == CAI_EIGHTH_RATE)
    {
      return MUX2_EIGHTH_PRI_LEN;
    }
    else
    {
      return 0;
    }
  }
}

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION TXC_DET_FRAME_RATE

DESCRIPTION
  This function determines the lowest frame rate on the R-FCH which can
  accomodate the primary and secondary traffic data based on the number of
  bits in the primary and secondary traffic stream.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cai_data_rate_type
txc_det_frame_rate (void)
{
  /* Called while processing FCH */
  if (ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_9600)
  {
    if ((prim_num_bits + sec_fch_num_bits) <= MUX1_EIGHTH_PRI_LEN)
    {
      return CAI_EIGHTH_RATE;
    }
    else if ((prim_num_bits + sec_fch_num_bits) <= MUX1_QTR_PRI_LEN)
    {
      return CAI_QUARTER_RATE;
    }
    else if ((prim_num_bits + sec_fch_num_bits) <= MUX1_HALF_PRI_LEN)
    {
      return CAI_HALF_RATE;
    }
    else
      return CAI_FULL_RATE;
  }
  else
  {
    if ((prim_num_bits + sec_fch_num_bits) <= MUX2_EIGHTH_PRI_LEN)
    {
      return CAI_EIGHTH_RATE;
    }
    else if ((prim_num_bits + sec_fch_num_bits) <= MUX2_QTR_PRI_LEN)
    {
      return CAI_QUARTER_RATE;
    }
    else if ((prim_num_bits + sec_fch_num_bits) <= MUX2_HALF_PRI_LEN)
    {
      return CAI_HALF_RATE;
    }
    else
      return CAI_FULL_RATE;
  }
}

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
void txc_sec (void)
{
  /* initialize FCH secondary rate and data ptr
  */
  sec_fch_num_bits = 0;
  sec_fch_data_ptr = NULL;

#ifdef FEATURE_IS2000_P2
  /* initialize DCCH secondary rate and data ptr
  */
  sec_dcch_num_bits = 0;
  sec_dcch_data_ptr = NULL;
#endif /* FEATURE_IS2000_P2 */

  switch (txc_so.sec.so)
  {
#ifdef FEATURE_IS2000_TDSO
    case CAI_SO_TDSO:
    case CAI_SO_FULL_TDSO:
    {
      boolean msg_on_channel = FALSE;

#ifdef FEATURE_IS2000_P2
      if (txc_so.rev_dcch.included &&
               (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_DCCH))
      {
        msg_on_channel = (txc_tx.active_msg && (txtc.cur_dsch == TXC_REV_DCCH));
        /* Ignoring data rate return value */
        (void) txc_tdso_sec_traffic (&sec_dcch_data_ptr, msg_on_channel);
        sec_fch_data_ptr = NULL;
      }
      else if (txc_so.rev_fch.included)
#endif /* FEATURE_IS2000_P2 */
      {
        /* R-FCH assigned and TDSO is secondary SO on R-FCH
        */
        if (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_FCH)
        {
          msg_on_channel = (txc_tx.active_msg && (txtc.cur_dsch == TXC_REV_FCH));
          /* Ignoring data rate return value */
          (void) txc_tdso_sec_traffic (&sec_fch_data_ptr, msg_on_channel);
#ifdef FEATURE_IS2000_P2
          sec_dcch_data_ptr = NULL;
#endif /* FEATURE_IS2000_P2 */
        }
      }
      break;
    }
#endif // FEATURE_IS2000_TDSO

    case CAI_SO_NULL:
    {
      sec_fch_data_ptr  = NULL;

#ifdef FEATURE_IS2000_P2
      sec_dcch_data_ptr = NULL;
#endif

      break;
    }

    case CAI_SO_PPP_PKT_DATA_3G:
    {
      boolean msg_on_channel = FALSE;
      dsrlp_rate_enum_type max_rlp_rate  = DSRLP_RATE_BLANK;

#ifdef FEATURE_IS2000_P2
      if (txc_so.rev_fch.included)
#endif
      {
        if (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_FCH)
        {
          /* R-FCH assigned and RLP is secondary SO on R-FCH
          */
          msg_on_channel = (txc_tx.active_msg && (txtc.cur_dsch == TXC_REV_FCH));
          max_rlp_rate =
              txc_rlp_sec_data_rate (MCTRANS_PHY_CHAN_FCH, msg_on_channel);
        }
      }
      txc_tx_process_fundich_data3g (MCTRANS_PHY_CHAN_FCH,
                                     max_rlp_rate,
                                     DSRLP_SECONDARY_SRVC);

#ifdef FEATURE_IS2000_P2
      max_rlp_rate  = DSRLP_RATE_BLANK;

      if (txc_so.rev_dcch.included &&
          (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_DCCH))
      {
        /* R-DCCH assigned and RLP is secondary SO on R-DCCH
        */
        boolean control_hold = txtc.gating_rate != CAI_GATING_RATE_NONE;

        if (!control_hold)
        {
          /* MS not in control hold state
          */
          msg_on_channel = (txc_tx.active_msg && (txtc.cur_dsch == TXC_REV_DCCH));
          max_rlp_rate =
            txc_rlp_sec_data_rate (MCTRANS_PHY_CHAN_DCCH, msg_on_channel);
        }
      }
      txc_tx_process_fundich_data3g (MCTRANS_PHY_CHAN_DCCH,
                                     max_rlp_rate,
                                     DSRLP_SECONDARY_SRVC);
#endif

      /* The pointers : sec_fch_data_ptr and sec_dcch_data_ptr
      ** are set after call to txc_tx_frame_proc_build_data3g(). 
      ** These pointers are currently set in txc_tx_frame_proc()
      ** but this will be moved to txc_tx_frame_proc_build_data3g().
      */

      break;
    }

    default:
    {
      MSG_FATAL ("Bad SO %d as Sec Traf", txc_so.sec.so, 0, 0);
      break;
    }
  }
}

/*===========================================================================

FUNCTION TXC_RLP_SEC_DATA_RATE

DESCRIPTION
  This function determines the RLP secondary data rate based on the rate of
  primary traffic size and signaling traffic.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsrlp_rate_enum_type
txc_rlp_sec_data_rate
(
  word    phy_channel,
  boolean msg_on_channel
)
{
  dsrlp_rate_enum_type sec_rlp_rate = DSRLP_RATE_BLANK;

  if ((txc_so.pri.so == NULL) ||
      (!(txc_so.pri.phy_channels & phy_channel)))
  {
    /* No primary traffic assigned  OR
    ** primary traffic assigned but not on this physical channel.
    ** Secondary only traffic on this physical channel.
    */
    if (msg_on_channel)
    {
      /* Signaling msg pending and signaling on this channel,
      ** BLANK secondary source
      */
      MSG_HIGH("Sgnling traf. No Sec data", 0, 0, 0);
      sec_rlp_rate = DSRLP_RATE_BLANK;
    }
    else
    {
      /* All bandwidth for data.
      */
      sec_rlp_rate = DSRLP_RATE_1;
    }
  }
  else if (txc_so.pri.phy_channels & phy_channel)
  {
    /* Primary + Secondary traffic type assigned on this physical channel
    */
    if ((msg_on_channel) && (prim_rate != CAI_FULL_RATE))
    {
      /* Signaling msg pending and signaling on this channel,
      ** and primary traffic source returned < FULL rate, send Pri + sig,
      ** BLANK secondary source
      */
      MSG_HIGH("Sgnling traf. No Sec data", 0, 0, 0);
      sec_rlp_rate = DSRLP_RATE_BLANK;
    }
    else if (txc_rlp_sig_priority.delay_fr_cnt > 0) 
    {
      /* Here if Vocoder has been dimmed but dim count is still non-zero.
      ** Do not allow RLP to send frames until dim count is zero so 
      ** continous TX for RLP is guaranteed.
      */
      MSG_HIGH("Awaiting cont TX. No Sec data", 0, 0, 0);
      sec_rlp_rate = DSRLP_RATE_BLANK;
    }
    else
    {
      /* Find out secondary data rate based on primary traffic size.
      */
      switch (prim_rate)
      {
        case CAI_FULL_RATE:
          //MSG_HIGH("Voice FULL rate. No Sec data", 0, 0, 0);
          sec_rlp_rate = DSRLP_RATE_BLANK;
          break;

        case CAI_HALF_RATE:
          sec_rlp_rate = DSRLP_RATE_1_2;
          break;

        case CAI_QUARTER_RATE:
          sec_rlp_rate = DSRLP_RATE_3_4;
          break;

        case CAI_EIGHTH_RATE:
          sec_rlp_rate = DSRLP_RATE_7_8;
          break;

        case CAI_BLANK_RATE:
          /* If primary service SO is voice, should not get here.
          */
          MSG_HIGH("Pri SO %d. No Sig. Got Blank fr",txc_so.pri.so, 0, 0);
          sec_rlp_rate = DSRLP_RATE_1;
          break;
          
        case CAI_NULL_RATE:
          /* Do nothing */
          break;

      }
    }
  }

  return sec_rlp_rate;
}

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
word
txc_tdso_sec_frame_size
(
  word    phy_channel,
  boolean msg_on_channel
)
{
  word tdso_sec_size = 0;
  enc_rate_set_type fundich_rate_set = ENC_RATE_9600;
#ifdef FEATURE_IS2000_P2
  if (phy_channel == MCTRANS_PHY_CHAN_FCH) 
  {
#endif
    fundich_rate_set = ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc);
#ifdef FEATURE_IS2000_P2
  }
  else
  {
    fundich_rate_set = ENC_RC_TO_RATE_SET(txc_so.rev_dcch.rc);
  }
#endif
  

  if (msg_on_channel)
  {
    /* Signaling msg pending and signaling on this channel,
    ** BLANK secondary source
    */
    MSG_HIGH("Sgnling traf. No Sec data", 0, 0, 0);
    tdso_sec_size = 0;
  }
  else if ((txc_so.pri.so == NULL) ||
           (!(txc_so.pri.phy_channels & phy_channel)))
  {
    /* No primary traffic assigned  OR
    ** primary traffic assigned but not on this physical channel.
    ** Secondary only traffic on this physical channel.
    ** All bandwidth for secondary source.
    */
    if (fundich_rate_set == ENC_RATE_9600)
    { 
      tdso_sec_size = TDSO_RS1_SECONDARY_03;
    }
    else
    {
      tdso_sec_size = TDSO_RS2_SECONDARY_03;
    }
  }
  else if (txc_so.pri.phy_channels & phy_channel)
  {
    /* primary traffic assigned on this physical channel
    */
    if (fundich_rate_set == ENC_RATE_9600)
    { 
      if (prim_rate == CAI_FULL_RATE)
      {
        tdso_sec_size = 0;
      }
      else if (prim_rate == CAI_HALF_RATE)
      {
        tdso_sec_size = TDSO_RS1_SECONDARY_00;
      }
      else if (prim_rate == CAI_QUARTER_RATE)
      {
        tdso_sec_size = TDSO_RS1_SECONDARY_01;
      }
      else if (prim_rate == CAI_EIGHTH_RATE)
      {
        tdso_sec_size = TDSO_RS1_SECONDARY_02;
      }
      else if (prim_rate == CAI_BLANK_RATE)
      {
        tdso_sec_size = TDSO_RS1_SECONDARY_03;
      }
      else
      {
        // Remove this later.
        ERR_FATAL ("code err", 0, 0, 0);
      }
    }
    else
    {
      if (prim_rate == CAI_FULL_RATE)
      {
        tdso_sec_size = 0;
      }
      else if (prim_rate == CAI_HALF_RATE)
      {
        tdso_sec_size = TDSO_RS2_SECONDARY_00;
      }
      else if (prim_rate == CAI_QUARTER_RATE)
      {
        tdso_sec_size = TDSO_RS2_SECONDARY_01;
      }
      else if (prim_rate == CAI_EIGHTH_RATE)
      {
        tdso_sec_size = TDSO_RS2_SECONDARY_02;
      }
      else if (prim_rate == CAI_BLANK_RATE)
      {
        tdso_sec_size = TDSO_RS2_SECONDARY_03;
      }
      else
      {
        // Remove this later.
        ERR_FATAL ("code err", 0, 0, 0);
      }
    }
  }

  return tdso_sec_size;
}

#endif // FEATURE_IS2000_REL_A_SVD

/* <EJECT> */
/*===========================================================================

FUNCTION BUILD_HRATE_FRAME

DESCRIPTION
  This function builds a half rate frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void build_hrate_frame( void )
{
  word left;
  int  si_adj = 0;    
          /* SI adjustment to account for the 1 bit difference.
             Default to 0 for P_REV < 7 */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -------------------------------------------------------
  ** Set format bits to indicate 9600 bps, dim-and-bust with
  ** Rate 1/2 primary and signaling traffic frame
  ** ------------------------------------------------------- */
  txtc.frame.data[0] = 0x80;

  b_copy( prim_data_ptr, 0, txtc.frame.data, 4, CAI_HALF_RATE_BITS );

#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
  {
    if (!txc_tx.buf.pos)
    {
      /* Set bit to indicate start-of-message */
      b_packb( 0xFF, txtc.frame.data, 84, 1 );
    }
    else
    {
      /* Clear start-of-message bit */
      b_packb( 0x0, txtc.frame.data, 84, 1 );
    }
  }

  /* Calculate how much of the message is left to send */
  left = txc_tx.buf.len - txc_tx.buf.pos;

  /* --------------------------
  ** Copy from message to frame
  ** -------------------------- */

  if (left >= (TC_DIM2 - 1 - si_adj )) /* Subtract SOM/SI bit */
  {
    b_copy( txc_tx.buf.msg, (word) txc_tx.buf.pos,
            txtc.frame.data, (word) (85 + si_adj), (word) (TC_DIM2 - 1 - si_adj ) );
  }
  else
  {
    b_copy( txc_tx.buf.msg, (word) txc_tx.buf.pos, 
            txtc.frame.data, (word) (85 + si_adj), left );
  }

  /* Move index to next section of message */
  txc_tx.buf.pos += (TC_DIM2 - 1 - si_adj);//lint !e734 Loss of precision (assignment) (31 bits to 16 bits)

} /* build_hrate_frame */

/* <EJECT> */
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
enc_rate_type blank_rate;

byte build_blank_frame ( txc_dsch_type dsch )
{
  int left;
  byte val;
  int r;
  uint8 *frame_ptr;
  int si_adj = 0;   
    /* Length adjustment for SI bits, default to 0 for p_rev < 7 */
  enc_rate_set_type fundich_rate_set = ENC_RATE_9600;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Store FCH signalling type for frame being built */
  next_fch_sig_type     = BLANK;

  /* Initialize val to avoid compiler warning */
  #ifdef FEATURE_IS2000_P2
  if ( dsch == TXC_REV_FCH )
  #endif // FEATURE_IS2000_P2
  {
    val = CAI_MUX1_REV_5_ID;
    frame_ptr = txtc.frame.data;
    fundich_rate_set = ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc);
  }
  #ifdef FEATURE_IS2000_P2
  else
  {
    val = CAI_MUX1_REV_DCCH_5_ID;
    frame_ptr = txtc.dcch_frame.data;
    fundich_rate_set = ENC_RC_TO_RATE_SET(txc_so.rev_dcch.rc);
  }
  #endif // FEATURE_IS2000_P2
  
#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */

  if ( fundich_rate_set == ENC_RATE_9600 )
  {
#ifdef FEATURE_MAX_PWR_PROTECTION_VOICE
#error code not present
#endif /* FEATURE_MAX_PWR_PROTECTION_VOICE */

#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
    {
      /* ----------------------------------------------------------------
      ** Set format bits to indicate blank-and-burst and set SOM bit if
      ** this is the first bit of the message to be sent.
      ** -------------------------------------------------------------- */
  
      frame_ptr[0] = 0xB0 | ( txc_tx.buf.pos ? 0 : 8 );

      /* ----------------------------------------------------------------
      ** Calculate how much of the message is left to send in this frame.
      ** Copy from message to frame and subtract 1 from len for SOM bit.
      ** -------------------------------------------------------------- */
  
      //
      // Remove after DEBUG
      //
      if (txc_tx.buf.len < txc_tx.buf.pos)
      {
        ERR_FATAL ("Sgnling buf in bad state", 0, 0, 0);
      }

      left = MIN (txc_tx.buf.len - txc_tx.buf.pos, TC_BLANK - 1 );
  
      b_copy( txc_tx.buf.msg, (word) txc_tx.buf.pos, frame_ptr, (word) 5, (word) left );

      /* Move index to next section of message */
      txc_tx.buf.pos += TC_BLANK - 1;
    }

    blank_rate = ENC_FULL_RATE;
  }

  else if (fundich_rate_set == ENC_RATE_14400)
  {
#ifdef FEATURE_MAX_PWR_PROTECTION_VOICE
#error code not present
#endif /* FEATURE_MAX_PWR_PROTECTION_VOICE */

    /* ----------------------------------------------------------------
    ** Calculate how much of the message is left to send in this frame.
    ** -------------------------------------------------------------- */
    left = txc_tx.buf.len - txc_tx.buf.pos;

    /*
    ** Look for which rate frame we can squeeze this message into.
    ** If the signalling message is too large for one frame just
    ** use full rate frame for the first frame and catch the rest
    ** on the next frame.
    */
    if ( dsch == TXC_REV_FCH )
    {
      for ( r = ENC_8TH_RATE; r > ENC_FULL_RATE; r-- )
      {

        if ( left < ( mux2_sig_blank_len[r] - si_adj ) )
          break;
      }
      blank_rate = (enc_rate_type) r;
    }
    else
    {
      blank_rate = ENC_FULL_RATE;
    }

#ifdef FEATURE_MAX_PWR_PROTECTION_VOICE
#error code not present
#endif /* FEATURE_MAX_PWR_PROTECTION_VOICE */

    /*
    ** Convert the blank frame rate chosen to Frame Mode value
    ** set erase bit = 0 ( to be filled in by traffic_isr, set MM_bit
    ** and set and shift frame mode bits appropriately
    */
    frame_ptr[0] = (uint8) (0 | MUX2_MM_BIT | mux2_sig_blank_fm [blank_rate]);

#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
    {
      /*
      ** Set SOM bit if this is the first bit of the message to be sent.
      */
    frame_ptr[0] |=
         (uint8) ( txc_tx.buf.pos ? 0 : mux2_sig_blank_som [blank_rate]);

    }

    /*
    ** Calculate length of message left to copy into frame buffer.
    ** Copy it and move index to next section of message.
    */
    left = MIN (left, mux2_sig_blank_len [blank_rate] - si_adj );

    b_copy( txc_tx.buf.msg, (word) txc_tx.buf.pos, frame_ptr,
             mux2_sig_blank_pos [blank_rate] + 1 + (word) si_adj, (word) left );

    txc_tx.buf.pos += mux2_sig_blank_len [blank_rate] - si_adj;//lint !e734 Loss of precision (assignment) (31 bits to 16 bits)

    /* set frame categorization for FCH frame statistics */
    /* For DCCH, it's already set earlier */
    if ( dsch == TXC_REV_FCH )
    {
      val = (byte) MUX2_BLANK [blank_rate];
    }
    else
    {
      val = CAI_MUX2_REV_DCCH_5_ID;
    }
  }

  txtc.rate = blank_rate;
  return ( val );

} /* build_blank_frame */



/* <EJECT> */
/*===========================================================================

FUNCTION BUILD_NORM_FRAME

DESCRIPTION
  This function builds a primary traffic only frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte build_norm_frame ( void )
{
  byte val;

  /* Store FCH signalling type for frame being built */
  next_fch_sig_type = NORM;

  /* Initialize val to avoid compiler warning */
  val = CAI_MUX1_REV_1_ID;

#ifdef FEATURE_MAX_PWR_PROTECTION_VOICE
#error code not present
#endif /* FEATURE_MAX_PWR_PROTECTION_VOICE */

  if ( ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_9600 )
  {
    switch (prim_rate)
    {
      case CAI_FULL_RATE:
        txtc.rate = ENC_FULL_RATE;
        val = CAI_MUX1_REV_1_ID;
        /* Copy vocoder data to our out buffer according to rate */
        b_copy( prim_data_ptr, 0, txtc.frame.data, 1,
                CAI_FULL_RATE_BITS );
        /* Buffer has already been set to 0 which forced
            the mixed mode bit to 0 */
        break;

      case CAI_HALF_RATE:
        txtc.rate = ENC_HALF_RATE;
        val = CAI_MUX1_REV_6_ID;
        /* Copy vocoder data to our out buffer according to rate */
        (void) memcpy( txtc.frame.data, prim_data_ptr,
                        CAI_HALF_RATE_BITS / 8 );
        break;

      case CAI_QUARTER_RATE:
        txtc.rate = ENC_QTR_RATE;
        val = CAI_MUX1_REV_7_ID;
        /* Copy vocoder data to our out buffer according to rate */
        (void) memcpy( txtc.frame.data, prim_data_ptr,
                        CAI_QUARTER_RATE_BITS / 8 );
        break;

      case CAI_EIGHTH_RATE:
        txtc.rate = ENC_8TH_RATE;
        val = CAI_MUX1_REV_8_ID;
        /* Copy vocoder data to our out buffer according to rate */
        (void) memcpy( txtc.frame.data, prim_data_ptr,
                        CAI_EIGHTH_RATE_BITS / 8 );
        break;

      default:                /* Should not happen... */
        if (!txc_tx.tx_blank)
        {
          MSG_LOW( "Bad Tx rate %d", prim_rate, 0, 0 );
        }
        val = build_null_frame ();

        break;
    } /* end switch (prim_rate) */
  }
  else if ( ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_14400 )
  {
    switch (prim_rate)
    {
      case CAI_FULL_RATE:
        txtc.rate = ENC_FULL_RATE;
        val = CAI_MUX2_REV_1_ID;
        /* Copy vocoder data to our out buffer according to rate */
        b_copy( prim_data_ptr, 0,
                txtc.frame.data, MUX2_PRI_POS, MUX2_FULL_PRI_LEN );
        /* Buffer has already been set to 0 which forced
            the mixed mode bit to 0 */
        break;

      case CAI_HALF_RATE:
        txtc.rate = ENC_HALF_RATE;
        val = CAI_MUX2_REV_11_ID;
        /* Copy vocoder data to our out buffer according to rate */
        b_copy( prim_data_ptr, 0,
                txtc.frame.data, MUX2_PRI_POS, MUX2_HALF_PRI_LEN );
        /* Buffer has already been set to 0 which forced
            the mixed mode bit to 0 */
        break;

      case CAI_QUARTER_RATE:
        txtc.rate = ENC_QTR_RATE;
        val = CAI_MUX2_REV_19_ID;
        /* Copy vocoder data to our out buffer according to rate */
        b_copy( prim_data_ptr, 0,
                txtc.frame.data, MUX2_PRI_POS, MUX2_QTR_PRI_LEN );
        /* Buffer has already been set to 0 which forced
            the mixed mode bit to 0 */
        break;

      case CAI_EIGHTH_RATE:
        txtc.rate = ENC_8TH_RATE;
        val = CAI_MUX2_REV_24_ID;
        /* Copy vocoder data to our out buffer according to rate */
        b_copy( prim_data_ptr, 0,
                txtc.frame.data, MUX2_PRI_POS, MUX2_EIGHTH_PRI_LEN );
        /* Buffer has already been set to 0 which forced
            the mixed mode bit to 0 */
        break;

      default:                /* Should not happen... */
        if (!txc_tx.tx_blank)
        {
          MSG_LOW( "Bad Tx rate %d", prim_rate, 0, 0 );
        }

        val = build_null_frame ();
        break;
    } /* end switch (prim_rate) */
  }

  return ( val );
}

#ifdef FEATURE_IS2000_REL_A_SVD

byte build_norm_pri_sec_frame ( void )
{
  byte val;

  /* Initialize val to avoid compiler warning */
  val = CAI_MUX1_REV_1_ID;

  if (ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_9600)
  {
    if (prim_rate == CAI_HALF_RATE)
    {
      txtc.frame.data[0] = 0xC0; // 0x80
      val = CAI_MUX1_REV_11_ID;
    }
    else if (prim_rate == CAI_QUARTER_RATE)
    {
      txtc.frame.data[0] = 0xD0; // 0x90
      val = CAI_MUX1_REV_12_ID;
    }
    else if (prim_rate == CAI_EIGHTH_RATE)
    {
      txtc.frame.data[0] = 0xE0; // 0xA0
      val = CAI_MUX1_REV_13_ID;
    }

    /* A Rate Set 1 frame with Pri+Sec traffic will always be FULL rate
    */
    txtc.rate = ENC_FULL_RATE;

    /* Copy vocoder data to out buffer
    */
    b_copy (prim_data_ptr, 0, txtc.frame.data, 4, prim_num_bits);

    /* Copy Secondary data to out buffer
    */
    b_copy (sec_fch_data_ptr, 0, txtc.frame.data,
            prim_num_bits + 4, sec_fch_num_bits);
  }
  else if (ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_14400)
  {
    switch (prim_rate)
    {
      case CAI_FULL_RATE:
        txtc.rate = ENC_FULL_RATE;
        val = CAI_MUX2_REV_1_ID;

        txtc.frame.data[0] = 0x00;

        /* Copy vocoder data to our out buffer according to rate */
        b_copy( prim_data_ptr, 0,
                txtc.frame.data, MUX2_PRI_POS, MUX2_FULL_PRI_LEN );
        /* Buffer has already been set to 0 which forced
            the mixed mode bit to 0 */
        break;

      case CAI_HALF_RATE:
        txtc.rate = ENC_FULL_RATE;
        val = CAI_MUX2_REV_6_ID; /* Header = "X101 00" */ 

        /* Set Mixed Mode bit and Frame Mode bits in buffer
        ** For Half rate Primary (124) + 138 Secondary traffic bits.
        */
        txtc.frame.data[0] = 0x50;

        /* Copy vocoder data to our out buffer according to rate */
        b_copy( prim_data_ptr, 0, txtc.frame.data, 6, MUX2_HALF_PRI_LEN );

        /* Copy RLP data to out buffer */
        b_copy( sec_fch_data_ptr, 0,
                txtc.frame.data, MUX2_HALF_PRI_LEN + 6, sec_fch_num_bits );
        break;

      case CAI_QUARTER_RATE:
        if (sec_fch_num_bits == 208)
        {
          val = CAI_MUX2_REV_7_ID; /* Header = "X101 01" */

          /* Set Mixed Mode bit and Frame Mode bits in buffer
          ** For Qtr rate Primary (54) + 208 Secondary traffic bits.
          */
          txtc.frame.data[0] = 0x54;
          txtc.rate = ENC_FULL_RATE;

          /* Copy vocoder data to our out buffer according to rate */
          b_copy (prim_data_ptr, 0, txtc.frame.data, 6, MUX2_QTR_PRI_LEN);

          /* Copy RLP data to out buffer
          */
          b_copy (sec_fch_data_ptr, 0,
                  txtc.frame.data, MUX2_QTR_PRI_LEN + 6, sec_fch_num_bits);
        }
        else if (sec_fch_num_bits == 67)
        {
          val = CAI_MUX2_REV_15_ID; /* Header = "X101 1" */

          /* Set Mixed Mode bit and Frame Mode bits in buffer
          ** For Qtr rate Primary (54) + 67 Secondary traffic.
          */
          txtc.frame.data[0] = 0x58;
          txtc.rate = ENC_HALF_RATE;

          /* Copy vocoder data to our out buffer according to rate */
          b_copy (prim_data_ptr, 0, txtc.frame.data, 5, MUX2_QTR_PRI_LEN);

          /* Copy RLP data to out buffer
          */
          b_copy (sec_fch_data_ptr, 0,
                  txtc.frame.data, MUX2_QTR_PRI_LEN + 5, sec_fch_num_bits);
        }
        else if (sec_fch_num_bits == 138)
        {
          val = CAI_MUX2_REV_7_ID; /* Header = "X101 01" */

          /* Set Mixed Mode bit and Frame Mode bits in buffer
          ** For Qtr rate Primary (54) + 208 Secondary traffic bits.
          */
          txtc.frame.data[0] = 0x54;
          txtc.rate = ENC_FULL_RATE;

          /* Copy vocoder data to our out buffer according to rate */
          b_copy (prim_data_ptr, 0, txtc.frame.data, 6, MUX2_QTR_PRI_LEN);

          /* Copy RLP data to out buffer
          */
          b_copy (sec_fch_data_ptr, 0,
                  txtc.frame.data, MUX2_QTR_PRI_LEN + 6, sec_fch_num_bits);
        }
        break;

      case CAI_EIGHTH_RATE:
        if (sec_fch_num_bits == 242)
        {
          val = CAI_MUX2_REV_8_ID; /* Header = "X101 10" */

          /* Set Mixed Mode bit and Frame Mode bits in buffer
          ** For Eighth rate Primary (20) + 242 Secondary traffic bits.
          */
          txtc.frame.data[0] = 0x58;
          txtc.rate = ENC_FULL_RATE;

          /* Copy vocoder data to our out buffer according to rate */
          b_copy (prim_data_ptr, 0, txtc.frame.data, 6, MUX2_EIGHTH_PRI_LEN);

          /* Copy RLP data to out buffer
          */
          b_copy (sec_fch_data_ptr, 0,
                  txtc.frame.data, MUX2_EIGHTH_PRI_LEN + 6, sec_fch_num_bits);
        }
        else if (sec_fch_num_bits == 138)
        {
          val = CAI_MUX2_REV_8_ID; /* Header = "X101 10" */

          /* Set Mixed Mode bit and Frame Mode bits in buffer
          ** For Eighth rate Primary (20) + 101 Secondary traffic bits.
          */
          txtc.frame.data[0] = 0x58;
          txtc.rate = ENC_FULL_RATE;

          /* Copy vocoder data to our out buffer according to rate */
          b_copy (prim_data_ptr, 0, txtc.frame.data, 6, MUX2_EIGHTH_PRI_LEN);

          /* Copy RLP data to out buffer
          */
          b_copy (sec_fch_data_ptr, 0,
                  txtc.frame.data, MUX2_EIGHTH_PRI_LEN + 6, sec_fch_num_bits);
        }
        else if (sec_fch_num_bits == 101)
        {
          val = CAI_MUX2_REV_16_ID; /* Header = "X110 0" */

          /* Set Mixed Mode bit and Frame Mode bits in buffer
          ** For Eighth rate Primary (20) + 101 Secondary traffic bits.
          */
          txtc.frame.data[0] = 0x60;
          txtc.rate = ENC_HALF_RATE;

          /* Copy vocoder data to our out buffer according to rate */
          b_copy (prim_data_ptr, 0, txtc.frame.data, 5, MUX2_EIGHTH_PRI_LEN);

          /* Copy RLP data to out buffer
          */
          b_copy (sec_fch_data_ptr, 0,
                  txtc.frame.data, MUX2_EIGHTH_PRI_LEN + 5, sec_fch_num_bits);
        }
        else if (sec_fch_num_bits == 32)
        {
          val = CAI_MUX2_REV_22_ID; /* Header = "X110" */

          /* Set Mixed Mode bit and Frame Mode bits in buffer
          ** For Eighth rate Primary (20) + 32 Secondary traffic bits.
          */
          txtc.frame.data[0] = 0x60;
          txtc.rate = ENC_QTR_RATE;

          /* Copy vocoder data to our out buffer according to rate */
          b_copy (prim_data_ptr, 0, txtc.frame.data, 4, MUX2_EIGHTH_PRI_LEN);

          /* Copy RLP data to out buffer
          */
          b_copy (sec_fch_data_ptr, 0,
                  txtc.frame.data, MUX2_EIGHTH_PRI_LEN + 4, sec_fch_num_bits);
        }
        break;

      default:                /* Should not happen... */
        if (!txc_tx.tx_blank)
        {
          MSG_LOW( "Bad Tx rate %d", prim_rate, 0, 0 );
        }
        txtc.rate = ENC_8TH_RATE;
        val = CAI_MUX2_REV_24_ID;
        /* put null primary traffic into an 1800 bps frame */
        txtc.frame.data[0] = 0x3F;
        txtc.frame.data[1] = 0xFF;
        txtc.frame.data[2] = 0xFC;
        break;
    } /* end switch (prim_rate) */
  }

  return ( val );
}


byte build_sec_frame ( void )
{
  byte val;

  /* Initialize val to avoid compiler warning */
  val = CAI_MUX1_REV_1_ID;

  if (ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_9600)
  {
    /* A RS1 frame with Sec only traffic will always be FULL rate
    */
    txtc.rate = ENC_FULL_RATE;

    txtc.frame.data[0] = 0xF0;
    val = CAI_MUX1_REV_14_ID;

    /* Copy Secondary data to out buffer
    */
    b_copy (sec_fch_data_ptr, 0, txtc.frame.data, 4, sec_fch_num_bits);
  }
  else if (ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_14400)
  {
    cai_data_rate_type frame_rate; 

    /* secondary only frame, explicitly set prim_num_bits = 0
    */
    prim_num_bits = 0;

    frame_rate = txc_det_frame_rate ();

    switch (frame_rate)
    {
      case CAI_FULL_RATE:
        val = CAI_MUX2_REV_9_ID;

        /* Header = "X101 11"
        */
        txtc.frame.data[0] = 0x5C;

        /* Copy Secondary data to out buffer
        */
        b_copy( sec_fch_data_ptr, 0, txtc.frame.data, 6, sec_fch_num_bits );

        txtc.rate = ENC_FULL_RATE;
        break;

      case CAI_HALF_RATE:
        val = CAI_MUX2_REV_17_ID;

        /* Set MM bit and FM bits in buffer for 121 Secondary traffic bits.
        ** Header = "X110 1XXX"
        */
        txtc.frame.data[0] = 0x68;

        /* Copy Secondary data to out buffer
        */
        b_copy (sec_fch_data_ptr, 0, txtc.frame.data, 5, sec_fch_num_bits);

        txtc.rate = ENC_HALF_RATE;
        break;

      case CAI_QUARTER_RATE:
        val = CAI_MUX2_REV_23_ID;

        /* Set MM bit and FM bits in buffer for 52 Secondary traffic bits.
        ** Header = "X111 XXXX"
        */
        txtc.frame.data[0] = 0x70;

        /* Copy Secondary data to out buffer
        */
        b_copy (sec_fch_data_ptr, 0, txtc.frame.data, 4, sec_fch_num_bits);

        txtc.rate = ENC_QTR_RATE;
        break;

      case CAI_EIGHTH_RATE:
        val = CAI_MUX2_REV_8_ID;

        /* Set MM bit and FM bits in buffer for 20 Secondary traffic bits.
        ** Header = "X1XX XXXX"
        */
        txtc.frame.data[0] = 0x40;

        /* Copy RLP data to out buffer
        */
        b_copy (sec_fch_data_ptr, 0, txtc.frame.data, 2, sec_fch_num_bits);

        txtc.rate = ENC_8TH_RATE;
        break;

      default:                /* Should not happen... */
        ERR_FATAL ("bad fr rate", 0, 0, 0);
        break;
    } /* end switch (prim_rate) */
  }

  return ( val );
}
#endif // FEATURE_IS2000_REL_A_SVD

/* <EJECT> */
/*===========================================================================

FUNCTION BUILD_NULL_FRAME

DESCRIPTION
  This function builds a primary traffic 8th rate null frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte build_null_frame ( void )
{
  byte val;

  if( ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_14400)
  {
    /* put null primary traffic into an 1800 bps frame */
    txtc.frame.data[0] = 0x3F;
    txtc.frame.data[1] = 0xFF;
    txtc.frame.data[2] = 0xFC;
    val = CAI_MUX2_REV_24_ID;
  }
  else
  {
    /* put null primary traffic into an 1200 bps frame */
    txtc.frame.data[0] = 0xFF;
    txtc.frame.data[1] = 0xFF;
    val = CAI_MUX1_REV_8_ID;
  }
  txtc.rate = ENC_8TH_RATE;

  /* Store signalling frame type for frame being built */
  next_fch_sig_type = NORM;

  return val;
}
/* <EJECT> */
/*===========================================================================

FUNCTION BUILD_DIM_FRAME

DESCRIPTION
  This function builds a dim-and-burst frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
enc_rate_type dim_rate, pri_rate;

byte build_dim_frame ( void )
{
  byte val, som;
  word left;
  int r;
  int si_adj = 0;   
          /* Adjustment for SI bit, default to 0 for p_rev < 7 */

  /* Store FCH signalling type for frame being built */
  next_fch_sig_type = DIM;

#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */


  /* Initialize val to avoid compiler warning */
  val = CAI_MUX1_REV_1_ID;

  /* Translate primary traffic rate to an encoder rate type */
  pri_rate = mux_cai_to_enc [prim_rate];

  if ( ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_9600 )
  {
    txtc.rate = ENC_FULL_RATE;       /* Full rate */

#ifdef FEATURE_MAX_PWR_PROTECTION_VOICE
#error code not present
#endif /* FEATURE_MAX_PWR_PROTECTION_VOICE */

    switch (prim_rate)
    {
      case CAI_HALF_RATE:
        build_hrate_frame();
        val = CAI_MUX1_REV_2_ID;
        break;

      case CAI_QUARTER_RATE:
        /* -------------------------------------------------------
        ** Set format bits to indicate 9600 bps, dim-and-bust with
        ** Rate 1/4 primary and signaling traffic frame
        ** ------------------------------------------------------- */
        txtc.frame.data[0] = 0x90;

        b_copy( prim_data_ptr, 0, txtc.frame.data, 4,
                CAI_QUARTER_RATE_BITS );

#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
        {
          if (!txc_tx.buf.pos)
          {
            /* Set bit to indicate start-of-message */
            b_packb( 0xFF, txtc.frame.data, 44, 1 );
          }
          else
          {
            /* Clear start-of-message bit */
            b_packb( 0x0, txtc.frame.data, 44, 1 );
          }
        }

        //
        // Remove after DEBUG
        //
        if (txc_tx.buf.len < txc_tx.buf.pos)
        {
          ERR_FATAL ("Signaling buf in bad state", 0, 0, 0);
        }

        /* Calculate how much of the message is left to send */
        left = txc_tx.buf.len - txc_tx.buf.pos;

        /* --------------------------
        ** Copy from message to frame
        ** -------------------------- */

        if (left >= (TC_DIM4 - 1 - si_adj )) /* Subtract SOM/SI bit */
        {
          b_copy( txc_tx.buf.msg, (word) txc_tx.buf.pos,
                  txtc.frame.data, (word) (45 + si_adj), (word) (TC_DIM4 - 1 - si_adj) );
        }
        else
        {
          b_copy( txc_tx.buf.msg, (word) txc_tx.buf.pos,
                  txtc.frame.data, (word) (45 + si_adj), left );
        }

        /* Move index to next section of message */
        txc_tx.buf.pos += (TC_DIM4 - 1 - si_adj);//lint !e734 Loss of precision (31 bits to 16 bits)

        val = CAI_MUX1_REV_3_ID;
        break;

      case CAI_EIGHTH_RATE:
        /* -------------------------------------------------------
        ** Set format bits to indicate 9600 bps, dim-and-bust with
        ** Rate 1/8 primary and signaling traffic frame
        ** ------------------------------------------------------- */
        txtc.frame.data[0] = 0xA0;

        b_copy( prim_data_ptr, 0, txtc.frame.data, 4,
                CAI_EIGHTH_RATE_BITS );

#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
        {
          if (!txc_tx.buf.pos)
          {
            /* Set bit 21 to indicate start-of-message */
            b_packb( 0xFF, txtc.frame.data, 20, 1 );
          }
          else
          {
            /* Clear start-of-message bit */
            b_packb( 0x0, txtc.frame.data, 20, 1 );
          }
          }

        /* Calculate how much of the message is left to send */
        left = txc_tx.buf.len - txc_tx.buf.pos;

        /* --------------------------
        ** Copy from message to frame
        ** -------------------------- */

        if (left >= (TC_DIM8 - 1 - si_adj )) /* Subtract SOM/SI bit */
        {
          b_copy( txc_tx.buf.msg, (word) txc_tx.buf.pos,
                  txtc.frame.data, (word) (21 + si_adj), (word) (TC_DIM8 - 1 - si_adj) );
        }
        else
        {
          b_copy( txc_tx.buf.msg, (word) txc_tx.buf.pos,
                  txtc.frame.data, (word) (21 + si_adj), left );
        }

        /* Move index to next section of message */
        txc_tx.buf.pos += (TC_DIM8 - 1 - si_adj);//lint !e734 Loss of precision (31 bits to 16 bits)

        val = CAI_MUX1_REV_4_ID;
        break;

      default:
        /* Must be a blank - build a blank and burst */
        if (!txc_tx.tx_blank)
        {
          ERR( "Bad voc rate %d", prim_rate, 0, 0 );
        }
        val = build_blank_frame( TXC_REV_FCH );
        break;
    } /* switch */
  }
  else if ( ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_14400 )
  {
    /* ----------------------------------------------------------------
    ** Calculate how much of the message is left to send in this frame.
    ** -------------------------------------------------------------- */
    left = txc_tx.buf.len - txc_tx.buf.pos;

    /*
    ** Look for which rate frame we can squeeze this message into.
    ** If the signalling message is too large for one frame just
    ** use full rate frame for the first frame and catch the rest
    ** on the next frame.
    */
    for (r = ENC_QTR_RATE; r > ENC_FULL_RATE; r-- )
    {
      if ( left < (mux2_sig_dim_len [r] [pri_rate] - si_adj) )
        break;
    }
    dim_rate = (enc_rate_type) r;

#ifdef FEATURE_MAX_PWR_PROTECTION_VOICE
#error code not present
#endif /* FEATURE_MAX_PWR_PROTECTION_VOICE */

    /*
    ** Convert the blank frame rate chosen to Frame Mode value
    ** set erase bit = 0 ( to be filled in by traffic_isr, set MM_bit
    ** and set and shift frame mode bits appropriately
    */
    txtc.frame.data[0] = (byte)
       (0 | MUX2_MM_BIT | mux2_sig_dim_fm [dim_rate] [pri_rate]);

    /*
    ** Fill the primary traffic into the primary part of the frame.
    */
    b_copy( prim_data_ptr, 0, txtc.frame.data,
            mux2_pri_dim_pos [dim_rate] [pri_rate],
            mux2_pri_dim_len [dim_rate] [pri_rate] );

    /*
    ** Set SOM bit if this is the first bit of the message to be sent.
    */
#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
    {
      som = ( txc_tx.buf.pos ? 0 : 0xFF );
    }
    
    b_packb( som, txtc.frame.data,
             mux2_sig_dim_pos [dim_rate] [pri_rate], (word) (1 + si_adj) );

    /*
    ** Calculate length of message left to copy into frame buffer.
    ** Copy it and move index to next section of message.
    */
    left = (word) MIN (left, mux2_sig_dim_len [dim_rate] [pri_rate] - si_adj );
    
    b_copy( txc_tx.buf.msg, (word) txc_tx.buf.pos, txtc.frame.data,
             (word) (mux2_sig_dim_pos [dim_rate][pri_rate] + 1 + si_adj), left );

    txc_tx.buf.pos += mux2_sig_dim_len [dim_rate][pri_rate] - si_adj;//lint !e734 Loss of precision (assignment) (31 bits to 16 bits)

    txtc.rate = dim_rate;

    /* set frame categorization for frame statistics */
    val = (byte) MUX2_DIM [dim_rate] [pri_rate];
  }
  return ( val );
}

/* <EJECT> */
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

void txc_build_frame
(
  txc_sig_frame_type  signext
)
{
  static    byte    val;          /* value for mux1 logging */
#ifdef FEATURE_IS2000_REL_A
  boolean pri_so_is_voice = (
                            (txc_so.pri.so == CAI_SO_VOICE_IS96A)
                         || (txc_so.pri.so == CAI_SO_VOICE_EVRC)
#ifdef FEATURE_SO_VOICE_SMV
                         || (txc_so.pri.so == CAI_SO_VOICE_SMV)
#endif
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
                         || (txc_so.pri.so == CAI_SO_VOICE_13K_IS733)
                         || (txc_so.pri.so == CAI_SO_VOICE_13K)
#endif
#ifdef FEATURE_VOC_4GV
                         || (txc_so.pri.so == CAI_SO_VOICE_4GV_NB)
                         || (txc_so.pri.so == CAI_SO_VOICE_4GV_WB)
#endif /* FEATURE_VOC_4GV */
                        );
#else
  boolean pri_so_is_voice = (
                            (txc_so.pri == CAI_SO_VOICE_IS96A)
                         || (txc_so.pri == CAI_SO_VOICE_EVRC)
#ifdef FEATURE_SO_VOICE_SMV
                         || (txc_so.pri == CAI_SO_VOICE_SMV)
#endif
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
                         || (txc_so.pri == CAI_SO_VOICE_13K_IS733)
                         || (txc_so.pri == CAI_SO_VOICE_13K)
#endif
#ifdef FEATURE_VOC_4GV
                         || (txc_so.pri == CAI_SO_VOICE_4GV_NB)
                         || (txc_so.pri == CAI_SO_VOICE_4GV_WB)
#endif /* FEATURE_VOC_4GV */
                        );
#endif /* FEATURE_IS2000_REL_A */
  qword sys_time_in_20ms;    /* system time in 20ms units */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (txc_tx.tx_on)
  {

    /* Encrypt voice, if feature is enabled.  If feature is not enabled,
    ** buffer will remain unchanged.
    */
    if (pri_so_is_voice)
    {
      ts_get_20ms_frame_time (sys_time_in_20ms);  /* for encryption */
      mccdma_encrypt_voice_frame (
        sys_time_in_20ms,                 /* system time */
        prim_data_ptr,                    /* pointer to buffer to encrypt */
        0,                                /* buffer offset */
        (uint32) prim_num_bits            /* number of bits to encrypt */
      );
    }

    switch (signext)
    {
      case BLANK:
        if (prim_rate == CAI_BLANK_RATE)
        {
          txtc.dim_delay = TXC_VOC_DIM_DELAY;
          val = build_blank_frame( TXC_REV_FCH );
        }
        else if (prim_rate == CAI_FULL_RATE)
        {
          /* The vocoder didn't give us a blank frame! */

          if (txtc.dim_delay)
          {
            txtc.dim_delay--;
            val = build_norm_frame ();
          }
          else
          {
            if ( pri_so_is_voice )
            {
              ERR( "Vocoder rate err", 0, 0, 0 );
            }
            val = build_blank_frame( TXC_REV_FCH );
          }
        } /* end if (prim_rate == CAI_FULL_RATE) */
        else
        {
#ifdef FEATURE_IS2000_REL_A_SVD
          if (txc_so.pri.so == CAI_SO_NULL)
          {
            /* Secondary only traffic, blank secondary data
            */
            val = build_blank_frame( TXC_REV_FCH );
          }
          else
#endif /* FEATURE_IS2000_REL_A_SVD */
          {
            val = build_dim_frame();
          }
        }

        break;

      case DIM:

        if (txtc.nulls)
        {
          /* Just switched to nulls - do blank-and-burst signaling */
          val = build_blank_frame( TXC_REV_FCH );
        }
        else
        {
          if ((prim_rate == CAI_FULL_RATE) || (prim_rate == CAI_BLANK_RATE))
          {
            /* The vocoder didn't give us a dimmed frame! */

            if (txtc.dim_delay)
            {
              txtc.dim_delay--;
              val = build_norm_frame();
            }
            else
            {
              if (pri_so_is_voice) 
              {
                ERR( "Vocoder rate err", 0, 0, 0 );
              }
              val = build_blank_frame( TXC_REV_FCH );
            }
          }
          else
          {
#ifdef FEATURE_IS2000_REL_A_SVD
            if (txc_so.pri.so == CAI_SO_NULL)
            {
              /* Secondary only traffic, BLANK secondary data
              */
              val = build_blank_frame( TXC_REV_FCH );
            }
            else
#endif /* FEATURE_IS2000_REL_A_SVD */
            {
              txtc.dim_delay = 3;
              val = build_dim_frame ();
            }
          }
        }
        break;

      case NORM:

        if( txtc.preambles )
        {
          val = (ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_14400) ?
                CAI_MUX2_REV_1_ID : CAI_MUX1_REV_1_ID;
          txtc.rate = ENC_FULL_RATE;
          //
          // No FCH/DCCH during RC3/4 preamble
          //
          if(
             #ifdef FEATURE_IS2000_P2
             txc_so.rev_dcch.included || txc_so.rev_fch.included &&
             #endif
             txc_so.rev_fch.rc >= CAI_RC_3 )
          {
            enc_fch_enable( TRUE );
            #ifdef FEATURE_IS2000_P2
            enc_dcch_enable( TRUE );
            #endif
            if( txc_hho_preamble_pcg_units == TRUE )
            {
              ASSERT( txtc.num_preambles <= 16 );

               // We want num_preambles 1's as the pilot gating pattern
              MSG_HIGH ("Start %d-PCG preamble", txtc.num_preambles, 0, 0);
              enc_is95c_tx_patn
              (
                 PREAMBLES_TO_GATING_PATN( txtc.num_preambles )
              );
              enc_gating_mode( ENC_HHO_PREAMBLE_MODE );

              // In RC3/4, preamble can only be 1 frame long in the maximum.  So set
              // num_preambles here to 0 but don't set preambles here to FALSE yet.
              // We need to defer that until the preamble is sent.  After that, we need
              // to disable this pattern and enable FCH.
              txtc.num_preambles = 0;
            }
            else
            {
              txtc.num_preambles--;
            }
          }
          else if ((--txtc.num_preambles) == 0)
          {
            txtc.preambles = FALSE;
          }
        }
        else if( txtc.nulls )
        {
          val = build_null_frame ();
          txc_data_rate = CAI_EIGHTH_RATE;
        }
        else
        {
#ifdef FEATURE_IS2000_REL_A_SVD
          if ((txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_FCH) &&
              (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_FCH))
          {
            /* both primary and secondary traffic type assigned on R-FCH
            */
            if (prim_data_ptr && sec_fch_data_ptr)
            {
              //
              // REMOVE if{...} AFTER DEBUG
              //
              if ((prim_rate == CAI_BLANK_RATE) ||
                  (prim_rate == CAI_NULL_RATE)  ||
                  (sec_fch_num_bits == 0))
              {
                ERR_FATAL("Incorrect Prim/Sec rate", 0, 0, 0);
              }
                  
              /* Pri + Sec traffic this frame
              */
              val = build_norm_pri_sec_frame ();
            }
            else if (prim_data_ptr)
            {
              //
              // REMOVE if{...} AFTER DEBUG
              //
              if ((prim_rate == CAI_BLANK_RATE) ||
                  (prim_rate == CAI_NULL_RATE)  ||
                  (sec_fch_num_bits != 0))
              {
                ERR_FATAL("Incorrect Prim/Sec rate", 0, 0, 0);
              }

              /* Pri only traffic this frame
              */
              val = build_norm_frame ();
            }
            else if (sec_fch_data_ptr)
            {
              //
              // REMOVE if{...} AFTER DEBUG
              //
              if ((prim_rate != CAI_BLANK_RATE) ||
                  (sec_fch_num_bits == 0))
              {
                ERR_FATAL("Incorrect Prim/Sec rate", 0, 0, 0);
              }

              /* Sec only traffic this frame
              */
              val = build_sec_frame ();
            }
            //
            // REMOVE following else{...} AFTER DEBUG
            //
            else
            {
              /* prim_data_ptr and sec_fch_data_ptr is NULL. Why? 
              */ 
              ERR_FATAL ("Pri and Sec ptrs == NULL", 0, 0, 0);
            }
          }
          else if (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_FCH)
          {

            /* No secondary traffic type assigned on R-FCH
            */
            if (prim_data_ptr)
            {
              /* build Primary only frame
              */
              val = build_norm_frame ();
            }
            else
            {
              if ((prim_rate != CAI_BLANK_RATE) && (prim_rate != CAI_NULL_RATE)) 
              {
                //
                // Change ERR_FATAL to MSG_ERROR.
                //
                MSG_ERROR("prim_data_ptr is NULL", 0, 0, 0);
              }
            }
          }
          else if (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_FCH)
          {
            /* No primary traffic type assigned on R-FCH
            */

            if (sec_fch_data_ptr)
            {
              //
              // REMOVE if{...} after DEBUG
              //
              if (sec_fch_num_bits == 0)
              {
                ERR_FATAL("Why is sec_fch_num_bits == 0?", 0, 0, 0);
              }

              /* build Secondary only frame
              */
              val = build_sec_frame ();
            }
            else
            {
              //
              // REMOVE if{...} after DEBUG
              //
              if (sec_fch_num_bits != 0)
              {
                ERR_FATAL("Why is sec_fch_num_bits != 0?", 0, 0, 0);
              }

              /* No Primary service, no Secondary data. Send NULL frame.
              */
              val = build_null_frame ();
              txc_data_rate = CAI_EIGHTH_RATE;
            }
          }
          else
          {
            /* FCH assigned but no Primary, Secondary service on FCH.
            ** Send NULL frame.
            */
            val = build_null_frame ();
            txc_data_rate = CAI_EIGHTH_RATE;
          }
#else
          val = build_norm_frame ();
#endif // FEATURE_IS2000_REL_A_SVD
        }
        break;

      default:
      {
        ERR_FATAL( "Bad sgnling mode %d", signext, 0, 0 );
      }
    } /* switch mode */
  }
  else
  {
    val = build_null_frame ();
    txc_data_rate = CAI_EIGHTH_RATE;
  }

  txc_log_mux1( val );             /* log the MUX1 data */

  mar_log_tx ( val );              /* log Markov stats */
  txtc.rev_mux = val;              /* store for RDCH logging */

  if (txc_tx.tx_blank)   /* one blank frame delay */
  {
    txc_tx.tx_blank = FALSE;
  }
}

