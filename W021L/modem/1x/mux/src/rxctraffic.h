#ifndef RXCTRAFFIC_H
#define RXCTRAFFIC_H
/*===========================================================================

            C D M A    R E C E I V E   T R A F F I C    M O D U L E
             
DESCRIPTION
  This header file contains definitions for the receive traffic module 
  that are used by the rxc unit.  This header file is internal to the rxc 
  unit.

Copyright (c) 1997 through 2006 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxctraffic.h_v   1.26   15 Oct 2002 18:30:56   donb  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxctraffic.h#3 $ $DateTime: 2008/07/28 09:31:07 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/08   jtm     Updated rxc_configure_ffpc_channel() prototype.
07/16/08   jtm     Added prototype for rxc_configure_ffpc_channel() and defines
                   for spreading rate and rounding factor array sizes.
06/26/06   vlc     Added extern for rxc_sent_valid_rpt.
12/07/04   ejv     Added extern for rxc_spr_rate_tab and rxc_rnd_factor_tab.
06/16/04   sr      Merged in changes from 6500 5004 build
06/14/04   vlc     Added log_pdch_rlp_bits to rxc_rlp_logging_chan_info_type. 
05/07/04   vlc     Implemented new RLP interface for EV-DV.  The new interface 
                   replaces the current interface for 1x channels as well.
                   Also, modified RLP logging extensively to work with new 
                   interface.
04/27/04   vlc     Added function rxc_check_for_sch_update() to fix rate change 
                   issue for bursts with 1 frame gap.
04/20/04   jrp     Fixed lint warnings.
05/29/03   vlc     For SVD, created fade_check_fch() and fade_check_dcch() to  
                   replace fade_check().                                       
12/12/02   ph      New argument to include channel frame received on rxc_tc_parse()
11/19/02   hxw     Added externed rxc_gps_state;
10/15/02   dlb     Added new power control logging to combine power control
                   and frame type logging.
08/08/02   vlc     Fixed compiler error when FEATURE_IS95B_MAHHO is turned off.
07/16/02   bgc     Featurized sch externs inside FEATURE_IS2000_F_SCH.
07/11/02   bn      Added rxc_set_tx_in_traffic_on function prototype 
06/04/02   vlc     Fixed minor typo in rxc_rda_log_frame_info() prototype.
06/04/02   bn      Modified the FDCH and RDA logging function parameters for VP2
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up 
                   (feature FEATURE_IS2000_REL_A_VP2.)
02/27/02   lad     Merged IS2000 Release A Common Chanels:
                   Moved TCI code to rxc_tci_init()
12/20/01   bn      Corrected the function name for logging RDA
12/10/01   hrk     Renamed rxc_erasure_flags_type to rxc_eib_flags_type and
                   renamed rxc_erasure_flags to rxc_eib_flags.
11/20/01   bn      Added RDA logging logic
11/13/01   bn      Added the enhance DCCH DTX detection and channel supervision logics
10/30/01   bn      Added support for Release A FDCH logging
09/12/01   bgc     Exported SCH decoder status for use in rxcrda module.
08/31/01   bn      Included the rxc_set_dcch_loopback() function prototype
08/28/01   lh      Back out SI bits changes for Release A Addendum 2.
08/22/01   bn      Support DCCH in FTM mode
06/15/01   day     Mainlined FEATURE_IS2000
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00 
           lcc     Added support for FEATURE_IS2000_P2.
03/27/01   vlc     Merged MSM_MUX1X.00.00.17.
           lcc     Changed rxc_stored_sch_packet to use type mctrans_rxc_f_sch_action_type.
03/07/01   va      MOved rxc_tc_var_type into this header file as this is needed by
                   rxcpaging module also.
02/01/01   tc      Updated to rxc_rifhho_cfs_gps_in_progress.
01/30/01   tc      Merged GPS support.
01/12/01   tc      Merged MSM_MUX1X.00.00.06.
11/23/00   tc      Fixed MSM5100 PLT rxc_set_fch_loopback, rxc_set_sch_loopback.
11/13/00   bgc     Added accessor functions for FTM loopback flags.
11/10/00   jc      Created after repartition of rxc.c

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "log.h"
#include "rxc.h"
#include "rxtx.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#if defined (FEATURE_DIAG_V7_LOG_IFACE)
  /* Type to log MAR data */
  typedef struct
  {
    log_desc_type desc;           /* log descriptor */
    log_mar_type  entry;          /* mar entry      */
  } rxc_log_mar_type;

  /* Structure for logging vocoder packets */
  typedef struct {
    log_desc_type desc;
    log_voc_type  voc;
  } rxc_log_voc_type;

#endif  

#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
  /* Indicator for a Return-If-Fail HHO or CFS                            */
  typedef struct {
    boolean previous;
    boolean current;
  } rxc_eib_flags_type;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

/* States of Traffic Channel parsing state machine */
typedef enum
{
  TC_HUNT_STATE,
  TC_MSG_STATE,
  TC_FLUSH_STATE
}rxc_tc_state_type;


typedef struct
{
  rxc_tc_state_type  state;
    /* State of Traffic Channel state machine */
  word  msg_inx;   /* index into message buffer */
#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#else
  word  msg_len;   /* length of message in bits */
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
  rxtx_rx_msg_type *msg_ptr;
    /* pointer to buffer to place received message for Layer 2 task */
#ifdef FEATURE_FACTORY_TESTMODE
  boolean fch_loopback;
#ifdef FEATURE_IS2000_P2
  /* Support FEATURE_FTM_DCCH */
  boolean dcch_loopback;
  /* End of FEATURE_FTM_DCCH */
#endif
  boolean sch_loopback;
#endif
#ifdef FEATURE_IS2000_REL_A
  byte p_rev_in_use; /* p_rev in use needed for frame unpacking */
  byte temp_p_rev_in_use; /* Store p_rev_in_use temporarily just before
                           the "action" frame arrives */
  boolean in_p_rev_transition; /* Indicates if p_rev is about to change */
  qword p_rev_action_frame; /* Action time of frame when p_rev changes */
#endif
} rxc_tc_var_type;

/* TC state varaiables */

extern rxc_tc_var_type rxc_tc_var;



extern byte *rxc_vbits;        /* Command buffer */


extern boolean rxc_erase;
  /* indicator of whether to treat blank and burst as erasures */


/* Decision history for FFPC logging */
extern word rxc_fpc_history;

#ifdef FEATURE_LOG_PC_INFO_2
/* Is FFPC logging for SCH active? */
extern boolean rxc_log_ffpc_sch_data;
#endif /* FEATURE_LOG_PC_INFO_2 */

#if defined(FEATURE_RLP_LOGGING)
/* Contains various channel information used in RLP 3 payload logging.
*/
typedef struct
{
  word log_fch_rlp_bits;   /* Num rlp bits on FCH */
  word log_dcch_rlp_bits;  /* Num rlp bits on DCCH */
  word log_pdch_rlp_bits;  /* Num rlp bits on PDCH (encapsulated PDUs only ) */
  boolean log_turbo_sch_erasure; /* Turbo SCH reports erasure frame */
  boolean first_entry;     /* This entry is first entry in the 20ms frame */

} rxc_rlp_logging_chan_info_type;
extern rxc_rlp_logging_chan_info_type rxc_rlp_logging_chan_info;

#endif /* FEATURE_RLP_LOGGING */


#if !defined (FEATURE_DIAG_V7_LOG_IFACE)
  extern log_mar_type rxc_log_mar_buf; /* Log buffer for accumulated Markov frame log */
#else
  extern q_type rxc_log_mar_q;  /* For logging MAR data */

  #ifdef TMOBILE
#error code not present
  #endif

  /*-------------------------------------------------------------------------*/
  /* Buffers for Logging Queues                                              */
  /*-------------------------------------------------------------------------*/

  /* buffers for rxc_log_mar_q */
  #define                           RXC_NUM_MAR_BUFS 5
  extern rxc_log_mar_type rxc_log_mar_pool[ RXC_NUM_MAR_BUFS ];
 
  /* Number of mar entries currently in Markov Frame log buffer */
  extern byte rxc_mar_cnt;

  /* pointer to buffer to buffer to log Markov data in */
  extern rxc_log_mar_type *rxc_mar_buf_ptr;
  
#endif /* defined(FEATURE_DIAG_V7_LOG_IFACE) */



extern boolean rxc_fade; /* indicator of whether fade processingis in
                            progress */
extern word rxc_ho_cnt;  /* number of frames before handoff processing */
extern byte rxc_voc_rate;      /* Current vocoder frame rate */ 
extern word rxc_lost_voc_pkts; /* Number of lost logged vocoder packets */
extern boolean rxc_sent_valid_rpt;  /* indicates if 2-good-frame report
                                       (RXC_VALID_R) has been sent */

/*-------------------------------------------------------------------------*/
/* Variables for Hard Handoff with Return-on-Failure                       */
/*-------------------------------------------------------------------------*/
#if defined (FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
  extern word rxc_lost_frames;
  /* Lost frame count during a Return-If-Fail HHO                         */
  extern word rxc_rif_hho_timeout_frames;
  /* Equivalent of the Return-If-Fail hard handoff timer                  */
  extern boolean rxc_rifhho_cfs_gps_in_progress;
  #ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
  #endif
  extern rxc_eib_flags_type rxc_eib_flags;
  
  extern boolean rxc_use_stored_eibs;
#endif /* (FEATURE_IS95B_MAHHO) || (FEATURE_GPS_MODE) */

#ifdef FEATURE_DS_IS2000
  #define SCH_MAX_FWD_PDU 8                     /* Max # of F-SCH MuxPDUs*/
  extern dsm_item_type * rxc_dsm_arr[ SCH_MAX_FWD_PDU]; /* Arr of SCH RLP frames*/
  #define RXC_DSM_ARR_NOT_ALLOC 0xFF               /* Const for invalid arr*/
  extern byte rxc_dsm_arr_cnt;                  /* Current index in arr */
#endif

/*-------------------------------------------------------------------------*/
/*  Supplemental Channel variable                                          */
/*-------------------------------------------------------------------------*/
extern dec_sch_cfg_type rxc_sch;

extern boolean rxc_voc_config_done;

#ifdef FEATURE_IS2000_F_SCH
extern rxc_frame_type rxc_sch_frame_status;
/* Export decoder status for rxcrda module */
extern dec_sch_status_type rxc_sch_status;
#endif /* FEATURE_IS2000_F_SCH */

extern mctrans_rxc_f_sch_action_type rxc_stored_sch_packet;

/* Lookup tables for spreading rate and rounding factor */
#define NUM_SPR_RATE_RC     3
#define NUM_RND_FACT_RC     3
#define NUM_SPR_RATES       6
#define NUM_RND_FACT        6

extern uint8 const rxc_spr_rate_tab[NUM_SPR_RATE_RC][NUM_SPR_RATES];
extern uint8 const rxc_rnd_factor_tab[NUM_RND_FACT_RC][NUM_RND_FACT];

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/*-------------------------------------------------------------------------*/
/*  Simultaneous Voice and Data variable                                   */
/*-------------------------------------------------------------------------*/
/* Used in simultaneous F-FCH and F-DCCH channel set up.  Indicates which
** channel handles FFPC.
*/
extern cai_fpc_pri_chan_type ffpc_channel;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifndef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION RXC_FRAME_TYPES_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated forward frame types log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A forward frame types log may be sent.

===========================================================================*/
extern void rxc_frame_types_send_log(void);
#else
/*===========================================================================

FUNCTION RXC_FDCH_FRAME_INFO_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated FDCH frame information log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A FDCH frame information log may be sent.

===========================================================================*/
void rxc_fdch_frame_info_send_log(void);
#endif /* FEATURE_IS2000_REL_A */

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
extern void rxc_rda_frame_info_send_log(void);

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
extern void rxc_rda_log_frame_info (uint8 fch_dec_rate, uint8 dcch_dec_rate);
#else
extern void rxc_rda_log_frame_info (uint8 dec_rate);
#endif /* FEATURE_IS2000_REL_A_SVD and FEATURE_IS2000_P2 */


/*===========================================================================

FUNCTION RXC_TCI

DESCRIPTION
  Traffic Channel Initialization processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Next state to be processed.

===========================================================================*/
extern word rxc_tci ( void );

/*===========================================================================

FUNCTION RXC_TC

DESCRIPTION
  Traffic Channel processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Next state to be processed.

===========================================================================*/
extern word rxc_tc( void );

/*===========================================================================

FUNCTION RXC_LOG_MAR

DESCRIPTION
  This function logs the Forward Traffic Channel Markov test data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_log_mar(byte mar,byte ber);

/*===========================================================================

FUNCTION RXC_TC_ISR

DESCRIPTION
  This function is the interrupt handler for the Decoder interrupt used
  when the Receive task is processing the Traffic Channel.
  Due to timing requirements imposed by the 13k decoder and the erasure
  indicator bit turnaround time we need to first process the decoded frame
  and decide what rate it is before building the final tx frame at this
  point in time.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_tc_isr( void );

/*===========================================================================

FUNCTION PWR_MSR_CHECK

DESCRIPTION
  This procedure processes the updated rxc_pwr structure to determine if a
  power measurement report should be sent to MC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A power measurement report may be sent to MC.

===========================================================================*/
extern void pwr_msr_check( void );

#ifdef FEATURE_IS2000_REL_A
/*=========================================================================
FUNCTION RXC_FDCH_LOG_FRAME_INFO

DESCRIPTION
  This function populates FDCH frame information logs.

DEPENDENCIE
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
void rxc_fdch_log_frame_info (byte fch_mux, byte dcch_mux ); /* mux type for FCH/DCCH logging */
#else
void rxc_fdch_log_frame_info (byte mux ); /* mux type for FCH/DCCH logging */
#endif /* FEATURE_IS2000_P2 and FEATURE_IS2000_REL_A_SVD */
#else
/*=========================================================================
FUNCTION RXC_LOG_FRAME_TYPE

DESCRIPTION
  This function populates frame type logs.

DEPENDENCIE
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
  extern void rxc_log_frame_type 
  (
    int16 frame_rate,                /* FCH  Frame rate */
    log_fch_type_enum_type fch_type, /* FCH  Frame type */
    log_sch_rate_enum_type sch0_type /* SCH0 Frame type */
    #ifdef FEATURE_IS2000_P2
    , cai_chind_type ch_ind     /* indicate which channels are active */
    , byte mux           /* mux type for DCCH logging */
    #endif /* FEATURE_IS2000_P2 */
  );

#endif /* FEATURE_IS2000_REL_A */
/*===========================================================================

FUNCTION RXC_TC_PARSE

DESCRIPTION
  This function collects the signaling bits of the Traffic Channel.
  When it determines that an entire Traffic Channel message has been
  received and the message CRC checks, it will place the message on
  a queue for the Layer 2 task and signal the Layer 2 task.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean indicator of whether message data was good.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean rxc_tc_parse
  (
    qword         frame_num,
    byte          frame[],    /* Address of 1st byte of frame. */
    word          frame_len,  /* Number of bits in frame */
    rxc_dsch_type frame_chan  /* Channel frame received on */
  );


/*===========================================================================

FUNCTION RXC_TCI_INIT

DESCRIPTION
  This funtion initializes the Traffic Channel initialization state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_IS2000_REL_A_CC
extern rxc_state_type rxc_tci_init
( 
  rxc_state_type curr_state,
  rxc_cmd_msg_type *cmd_ptr
);
#else
extern void rxc_tci_init(  word curr_state );
#endif /* FEATURE_IS2000_REL_A_CC */


#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION RXCTRAFFIC_FRAME_BND_INIT

DESCRIPTION
  This funtion initializes the Traffic Channel processing state at the 
  frame boundary.  Note this is called in ISR context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxctraffic_frame_bnd_init (void);
#endif /* FEATURE_IS2000_REL_A_CC */


#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION FADE_CHECK_FCH

DESCRIPTION
  This procedure processes the frame rate decision and updates the fade
  good and bad frame counts accordingly to determine if a fade report
  should be sent to MC.  This procedure is called when the primary power
  control channel is FCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A fade report may be sent to MC.

===========================================================================*/
extern void fade_check_fch
(
  byte mux
);

/*===========================================================================

FUNCTION FADE_CHECK_DCCH

DESCRIPTION
  This procedure processes the frame rate decision and updates the fade
  good and bad frame counts accordingly to determine if a fade report
  should be sent to MC.  This procedure is called when the primary power
  control channel is DCCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A fade report may be sent to MC.

===========================================================================*/
extern void fade_check_dcch
(
#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
  rxc_dcch_quality_type  dcch_chan_quality
#else
  byte mux
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION */
);

#else /* else of FEATURE_IS2000_REL_A_SVD */
/*===========================================================================

FUNCTION FADE_CHECK

DESCRIPTION
  This procedure processes the frame rate decision and updates the fade
  good and bad frame counts accordingly to determine if a fade report
  should be sent to MC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A fade report may be sent to MC.

===========================================================================*/
extern void fade_check
(
  byte mux
    /* category of received frame */
#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  
       ,rxc_dcch_quality_type  dcch_chan_quality
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  */
);
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_FACTORY_TESTMODE
/*===========================================================================

FUNCTION RXC_FCH_LOOPBACK_QUERY

DESCRIPTION
  This procedure returns whether FCH loopback is enabled for Factory 
  Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean, true if loobpack is on.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean rxc_fch_loopback_query( void );

#ifdef FEATURE_IS2000_P2
/* Support FEATURE_FTM_DCCH */
/*===========================================================================

FUNCTION RXC_DCCH_LOOPBACK_QUERY

DESCRIPTION
  This procedure returns whether DCCH loopback is enabled for Factory 
  Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean, true if loobpack is on.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean rxc_dcch_loopback_query( void );
/* End of FEATURE_FTM_DCCH */
#endif

/*===========================================================================

FUNCTION RXC_SCH_LOOPBACK_QUERY

DESCRIPTION
  This procedure returns whether SCH loopback is enabled for Factory 
  Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean, true if loobpack is on.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean rxc_sch_loopback_query( void );

/*===========================================================================

FUNCTION RXC_SET_FCH_LOOPBACK

DESCRIPTION
  This procedure sets FCH loopback for Factory Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Loopback mode is set in rxctraffic.

===========================================================================*/
extern void rxc_set_fch_loopback( boolean );

/*===========================================================================

FUNCTION RXC_SET_DCCH_LOOPBACK

DESCRIPTION
  This procedure sets DCCH loopback for Factory Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Loopback mode is set in rxctraffic.

===========================================================================*/
extern void rxc_set_dcch_loopback( boolean );

/*===========================================================================

FUNCTION RXC_SET_SCH_LOOPBACK

DESCRIPTION
  This procedure sets SCH loopback for Factory Test Mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Loopback mode is set in rxctraffic.

===========================================================================*/
extern void rxc_set_sch_loopback( boolean );

#endif

/*=============================================================================
FUNCTION RXC_SET_TX_IN_TRAFFIC_ON

DESCRIPTION
  This function is called to set/reset the rxc_tx_in_traffic_on flag if TX is turned 
  on/off

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rxc_set_tx_in_traffic_on(boolean tx_status);

/*===========================================================================

FUNCTION rxc_check_for_sch_update

DESCRIPTION
  This function checks for SCH updates in case of 1 frame gap.  This function
  is called AFTER the current frame has been demuxed and is ready for RLP, 
  and BEFORE RLP is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_check_for_sch_update (void);

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/*===========================================================================

FUNCTION RXC_CONFIGURE_FFPC_CHANNEL

DESCRIPTION
  Used in simultaneous F-FCH and F-DCCH channel set up.  Indicates 
  which channel handles FFPC. Also resets the data valid flag and 
  counters for FCH and DCCH in VP2
  
DEPENDENCIES
  

RETURN VALUE
  Current Radio Configuration

SIDE EFFECTS
  None
===========================================================================*/
cai_radio_config_type rxc_configure_ffpc_channel 
(
  rxc_cmd_msg_type *cmd_ptr
);
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

#endif /* RXCTRAFFIC_H */

