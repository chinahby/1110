#ifndef RXCCOMMON_H
#define RXCCOMMON_H
/*===========================================================================

             C D M A    R E C E I V E   C O M M O N   M O D U L E

DESCRIPTION
  This header file contains definitions for the receive common module that
  are used by the rxc unit.  This header file is internal to the rxc unit.

Copyright (c) 1997 through 2008 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxccommon.h_v   1.13   27 Aug 2002 09:20:18   vlechan  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/rxccommon.h#3 $ $DateTime: 2008/07/28 09:31:07 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/08   jtm     Updated RXC_UPR_BNDS_CHK to RXC_UPPER_BOUNDS_CHECK.
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV and obsolete prototypes. 
                   Added RXC_UPR_BNDS_CHK.
05/03/07   cab     Moved to new sleep voting interface
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.                                   
04/20/04   jrp     Fixed lint warnings.
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
03/03/04   sr      Changes for DV
08/08/03   vlc     Use dec_dcch_status_type instead of dec_fch_status_type for
                   rxc_dcch_status.
07/18/03   vlc     Release A changes:  Changed FCH and DCCH
                   channel set up to better indicate when decoder data is
                   available for the appropriate channel(s) for the RXC layer.
06/16/03   bn      Fixed the "out of buffer dec_free_q" problem.
05/05/03   bn      Added new frame skipping field into rxc_slot_type to handle SMMC
08/27/01   vlc     Changed power measurement report process for VP2.
08/08/02   vlc     Fixed compiler error when FEATURE_IS95B_MAHHO is turned off.
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up 
                   (feature FEATURE_IS2000_REL_A_VP2.)
02/27/02   lad     Added rxc_clear_decoder_queue().
02/25/02   bn      Added support for  MUX SVD signaling 
10/30/01   bn      Added support for Release A FDCH logging
10/17/01   lad     Added inclusion of atmr.h and mccrx.h.
08/15/01   sr      Put back prototype for rxc_init_action_item(); it is called
                   in rxcpaging.c
08/02/01   lcc     Removed function prototypes for functions used only in
                   traffic channel.
06/15/01   day     Mainlined FEATURE_IS95B, FEATURE_IS2000
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2
01/29/01   bgc     Moved rxc_scr_trans_cmd() to rxctraffic.c
11/10/00   jc      Created after repartition of rxc.c

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "tdso.h"
#include "dec.h"
#include "rex.h"
#include "atmr.h"
#include "mccrx.h"
#include "rxc.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Command block for Interrupt handlers */
typedef struct
{
  q_link_type      link;   /* Queue links */
  byte cmd;                /* Command to be processed */
  word sci_mask;           /* current sci_mask */
  word idle_pgslot;        /* assigned paging slot */
}rxc_isr_cmd_type;

/*-------------------------------------------------------------------------*/
/*  Forward Traffic Channel Power Control Parameters                       */
/*-------------------------------------------------------------------------*/

typedef struct
{
  boolean threshold_on;
    /* Threshold report mode indicator */
  boolean periodic_on;
    /* Periodic report mode indicator */
  word delay_frames;
    /* Number of frames to delay after a Power Measurement message is
       sent before recording */
  word pwr_thresh;
    /* Threshold for bad frames before Power Measurement message is sent */
  word pwr_frames;
    /* Maximum number of frames before a Power Measurement message is sent */
  byte num_bad_frames;
    /* Number of category 9 and 10 frames received */
  word num_frames;
    /* Total number of frames received */
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  byte dcch_num_bad_frames;
    /* For VP2 only, num_bad_frames applies to FCH and dcch_num_bad_frames 
    ** applies to DCCH.  This is done for both "DCCH only" and "FCH & DCCH"
    ** scenarios.
    */
  word dcch_num_frames;
    /* For VP2 only, num_frames applies to FCH and dcch_num_frames 
    ** applies to DCCH.  This is done for both "DCCH only" and "FCH & DCCH"
    ** scenarios.
    */
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

  word num_delay_frames;
    /* Number of frames delayed */

  word sch_tot_frames[2]; /* for SCH0 and SCH1 */
  word sch_bad_frames[2]; /* for SCH0 and SCH1 */
  boolean sch_fer_rep[2]; /* for SCH0 and SCH1 */

}  rxc_pwr_type;


/*-------------------------------------------------------------------------*/
/* Data structures and variables for RXC ACTION TIME PROCESSING            */
/*-------------------------------------------------------------------------*/

typedef struct rxc_action_list_item_struct
{
  rxc_cmd_msg_type action_cmd;
  qword action_time;
  struct rxc_action_list_item_struct *prev_ptr;
  struct rxc_action_list_item_struct *next_ptr;
} rxc_action_list_item_type;

#ifdef FEATURE_IS95B_MDR
  /*-------------------------------------------------------------------------*/
  /*  Supplemental Code Channel (IS95B SCCH )variable                        */
  /*-------------------------------------------------------------------------*/

    typedef struct
  {
     /* Number of Supplemental Code Channels */
  byte rate_sel[7]; /* store the rate for logging */
     byte num_sup;
  } rxc_scch_type;
#endif

/*-------------------------------------------------------------------------*/
/* Service Option variables                                                */
/*-------------------------------------------------------------------------*/

typedef struct
{
  dec_rate_set_type rate_set;           /* rate set in use */
  struct
  {
    #ifdef FEATURE_IS2000_P2
    boolean included;               /* Indicates if channel is included */
    #ifdef FEATURE_IS2000_REL_A_SVD
    boolean data_valid;             /* Indicates when data is valid */
    word data_valid_countdown;      /* Counts down when data is valid */
    #endif
    #endif
    cai_radio_config_type rc;       /* RC of channel */
  }
  #ifdef FEATURE_IS2000_P2
  for_fch, for_dcch;
  #else
  for_fch;
  #endif
  boolean high_rc;         /* Indicate if RC 3 or above */

  word pri;                /* Primary service option */
  word sec;                /* Secondary service option */
#ifdef FEATURE_PLT
#error code not present
#endif
#ifdef FEATURE_FACTORY_TESTMODE
  boolean enable_skip_tci;   /* disables check for good frames in TCI */
#endif
  byte transaction_num;  /* transaction number for action times */

#ifdef FEATURE_IS2000_REL_A /* copy the SCH MUX OPTION from high layer to the mux for FDCH logging */
  word for_sch_mux_option;
#endif /* FEATURE_IS2000_REL_A */
} rxc_so_type;

/* function prototype for action time callback function */
typedef void (*rxc_atmr_func_type)(void);

/* function prototype for transaction callback function */
typedef void (*transaction_processor_type) (mctrans_action_packet_type *);

/*-------------------------------------------------------------------------*/
/* Variables for counting slots                                            */
/*-------------------------------------------------------------------------*/
typedef struct
{
  boolean active;     /* indicator of whether we are checking slots */
  boolean valid_msg;  /* indicator of whether a valid msg was received */
  word slot_frames;   /* number of frames remaining in the 2 slot period */
  word slot_now;      /* current slot */
  byte slot_mask;     /* the slot mask indicating the type of slot(s) RXC is about to enter */
  byte valid_msg_mask; /* the type of slots that valid msg reporting is for */
#ifdef FEATURE_BROADCAST_SMS
  word bcpg_slot_frames; /* number of frames remaining in the broadcast page slots */
  word bcsms_slot_frames; /* number of frames remaining in the broadcast SMS slots */
  boolean bc_started; /* the scheduled BC has started in the first slot */
#endif /* FEATURE_BROADCAST_SMS */
  word slot;          /* assigned paging slot modulo sci_mask */
  word idle_pgslot;   /* Hashed paging channel slot */
  word sci_mask;      /* Mask to get position-in-slot from pgslot */
  boolean after_slot_boundary;
  boolean skipping;   /* Decoder interrupts skipped until 20ms after slot */
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
  boolean skipping_bcch; /* for SMMS, we need different flag for BCCH, mainly 2sd BCCH */
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
  boolean page_match; /* Enable page matching for first frame after slot */
  boolean blank;      /* Enable blank for first *int* (not frame) aft slot */
  byte frame_skip_cnt;/* Count used to know when to jump out of skipping state */
} rxc_slot_type;


#define RXC_TIMER_NOT_SET -1

/* Table to convert forward link RC to rate set */
/* Note: RC starts from 1, zeroth entry is a filler.  All RCs entries defined */
/* here, but Tiger 2 only supports 1 - 5. */
extern const dec_rate_set_type fwd_rc_to_rate_set[10];


extern rex_timer_type  rxc_fade_timer;
  /* Timer for amount of time since mobile last detected a valid SAT while
     communicating with the base station over the voice channel */

extern rex_timer_type rxc_aho_fade_timer;
  /* This timer is used for T72M timer monitoring */

/* Queues */

extern q_type          rxc_dec_q;
  /* Queue for signaling data packets received from the decoder */

extern q_type          rxc_dec_free_q;
  /* Queue for free buffers to be placed, when filled,
     on the rxc_dec_q queue */

extern q_type          rxc_isr_q;
  /* Queue for command packets to be processed by the Receive interrupt
     handler */

extern q_type          rxc_isr_free_q;
/* Queue for free buffers to be placed, when filled,
     on the rxc_isr_q queue */

extern byte rxc_frame_offset;
  /* Stores the current frame offset of the forward link traffic
     channel */

extern byte rxc_frame_offset_countdown;
  /* Used as a counter to determine of rxc_dec_frame needs to be recomputed */


extern dec_fch_status_type rxc_fch_status;

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/* Used in simultaneous F-FCH and F-DCCH channel set up.
*/
extern dec_dcch_status_type rxc_dcch_status;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

extern byte  rxc_zero_count; /* Indicator of whether SER statistics should be set to
                        zero - anything other than 0 = reset */

extern int rxc_time_cnt;


/*-------------------------------------------------------------------------*/
/* Interrupt Service Routine variables */
/*-------------------------------------------------------------------------*/

/* Frame number */
extern qword  rxc_dec_frame;

extern rxc_isr_cmd_type *rxc_isr_cmd_buf_ptr;

/* logging AGC values */
extern byte log_rx_agc;
extern byte log_tx_pwr;
extern byte log_tx_gain_adj;

/*  Forward Traffic Channel Power Control Parameters  */

extern rxc_pwr_type rxc_pwr;

/* Service Option variables */

extern rxc_so_type rxc_so;

#ifdef FEATURE_IS2000_REL_A_SVD
#define RXC_SR_ID_MAX   8
typedef struct 
{
   word   traffic_type;         /* 0 for primary, 1 for secondary */
   word   so;                   /* service option */
} rxc_srid_ref_type ;

extern rxc_srid_ref_type  rxc_srid_ref[RXC_SR_ID_MAX];
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS95B_MDR
/*-------------------------------------------------------------------------*/
/*  Supplemental Code Channel (IS95B SCCH )variable                        */
/*-------------------------------------------------------------------------*/
   extern rxc_scch_type rxc_scch;
#endif  /* FEATURE_IS95B_MDR */

#ifdef FEATURE_PLT
#error code not present
#endif  /* FEATURE_PLT */

/*-------------------------------------------------------------------------*/
/* Variables for counting slots                                            */
/*-------------------------------------------------------------------------*/
extern rxc_slot_type rxc_slot;

extern word rxc_log_msg_drop;     /* number of messages dropped due to no
                                 available buffer */

#if defined(FEATURE_IS95B_MAHHO) || defined (FEATURE_GPS_MODE)
  /* Indicator to ignore good frame while doing a Return-If-Fail HHO      */
  extern boolean rxc_ignore_good_frame;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

extern q_type rxc_action_item_free_q;

extern atmr_handle_type rxc_atmr_handle;

#ifdef FEATURE_IS2000_TDSO
extern tdso_frame_block_type rxc_tdso_frame_block;
#endif // FEATURE_IS2000_TDSO

#define RXC_UPPER_BOUNDS_CHECK(index, array_size) \
          (((index) < (array_size)) ? (index):((array_size)-1))

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION RXC_EXIT

DESCRIPTION
  This function performs processing necessary when leaving the RX CDMA
  subtask

DEPENDENCIES
  none.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_exit( void );

/*========================================================================

FUNCTION RXC_CONFIG_DEMOD

DESCRIPTION This function configures the demodulator and combiner based on
            Radio Configuration

DEPENDENCIES ??

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_config_demod( deint_rc_type rc );

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/*========================================================================

FUNCTION RXC_CONFIG_DEMOD_CH1

DESCRIPTION This function configures the demodulator and combiner for
            Channel 1 based on Radio Configuration.

DEPENDENCIES ??

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_config_demod_ch1 ( deint_rc_type rc );

/*========================================================================

FUNCTION RXC_SET_DCCH_MODE

DESCRIPTION This function handles the process of setting up DCCH.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_set_dcch_mode
( 
  deint_rc_type dcch_rc,
  boolean power_control_channel 
);

/*========================================================================

FUNCTION RXC_SET_FCH_MODE

DESCRIPTION This function handles the process of setting up FCH.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_set_fch_mode
( 
  deint_rc_type fch_rc,
  boolean power_control_channel 
);

#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

/*===========================================================================

FUNCTION RXC_MC_RPT

DESCRIPTION
  This function generates a report to the Main Control task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_mc_rpt(mccrx_rpt_type *rpt_ptr);


/*===========================================================================

FUNCTION RXC_ISR_CMD

DESCRIPTION
  This procedure posts a command for the interrupt handler, continues to
  kick the watchdog while waiting for a response and returns the response.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_isr_cmd( rxc_isr_cmd_type *cmd_ptr );

/*===========================================================================

FUNCTION RXC_FLAG_ERASURE

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_IS95B_MAHHO
  extern void rxc_flag_erasure( boolean erasure_flag );
#endif /* FEATURE_IS95B_MAHHO */

/*===========================================================================

FUNCTION SEND_PWR_RPT

DESCRIPTION
  This function sends a power report to the Main Control task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void send_pwr_rpt( void );

/*===========================================================================

FUNCTION RXC_WAIT

DESCRIPTION
  This function continues to reset the Watchdog signal and timer while
  waiting for any signal in a specified signal mask to be set. If this
  procedure detects that any signal in the specified signal mask is set,
  it will return the entire mask of signals set in the task'stcb.

DEPENDENCIES
  None.

RETURN VALUE
  Mask of signals returned by the rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
extern rex_sigs_type rxc_wait( rex_sigs_type wait_sigs );

/*========================================================================

FUNCTION RXC_CLEANUP_SCH_MUX

DESCRIPTION This function cleansup the array that is used to hold
            the SCH MUX frames

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
#ifdef FEATURE_DS_IS2000
  extern void rxc_cleanup_sch_mux( void );
#endif

/*===========================================================================
FUNCTION RXC_RESET_FRAME_COUNT

DESCRIPTION
  This function resets the frame counters of valid message reporting

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Some RXC global counters will be changed.

===========================================================================*/
extern void rxc_reset_frame_count( void );

/*==========================================================================
FUNCTION RXC_SC_ser_ACCUM

DESCRIPTION
  This function accumulates the status information for SYNC Channel.
  The status information includes CRC/Quality, Energy Metrics and SER

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void rxc_sc_ser_accum( void );

/*===========================================================================

FUNCTION RXC_CLEAR_DECODER_QUEUE

DESCRIPTION
  This function clears the RXC decoder queue by placing all items on the
  decoder free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_clear_decoder_queue (void);

/*===========================================================================

FUNCTION RXC_CLEAR_ISR_CMD_QUEUE

DESCRIPTION
  This function clears the RXC ISR COMMAND queue by placing all items on the
  ISR COMMAND free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_clear_isr_cmd_queue (void);

/*===========================================================================

FUNCTION RXC_VOTE_OK_TO_SLEEP

DESCRIPTION
  Indicates to sleep subsystem that MUX is voting ok to sleep (do tcxo 
  shutdown)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

void rxc_vote_ok_to_sleep( void );

/*===========================================================================

FUNCTION RXC_VOTE_NOT_OK_TO_SLEEP

DESCRIPTION
  Indicates to sleep subsystem that MUX is voting not ok to sleep (do not do
  tcxo shutdown)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

void rxc_vote_not_ok_to_sleep( void );

#endif /* RXCCOMMON_H */

