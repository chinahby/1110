#ifndef RXC_H
#define RXC_H
/*===========================================================================

                R X C D M A   S U B T A S K    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for the main
  task to interface with the RXC subtask.

Copyright (c) 1990 through 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxc.h_v   1.29   29 Oct 2002 10:54:06   baon  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/rxc.h#1 $ $DateTime: 2007/11/15 15:12:49 $ $Author: glenns $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/17/07   vlc     Moved rxc_get_chan_tc_demod_config_param outside
                   FEATURE_IS2000_REL_A_CC.
01/09/07   vlc     Moved rxc_chan_demod_config_param_type outside 
                   FEATURE_IS2000_REL_A_CC.  This type is used by SRCH 
                   outside the REL_A feature.
06/09/06   trc     Remove T_MSM6700 featurization
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
04/13/05   bn/fq   Moved all the common defines and structures to rxcmc.h
12/07/04   ejv     Added rxc_tc_chan_id_type, rxc_sch_info_type,
                   rxc_tc_chan_info_type, and prototype for
                   rxc_get_chan_tc_demod_config_param.
11/09/05   vlc     Added support for Rel D Fast Call Set Up Bypass 2 Good Frame
                   feature.
09/28/04   sr      Merged in changes from 6550 2009 build
08/09/04   bn/kwo  Added new interfaces to expose chan mod setup parameters
05/26/04   vlc     Added RXC_FOR_PDCH to rxc_dsch_type.
05/07/04   vlc     Implemented new RLP interface for EV-DV.  The new interface
                   replaces the current interface for 1x channels as well.
                   Also, modified RLP logging extensively to work with new
                   interface.
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
03/03/04   sr      Changes for DV plus included changes to MSMSHARED tip
01/13/04   jrp     Moved descramble bypass variables in RXC_TCI_F within
                   FEATURE_IS2000_REL_C_DV.
11/04/03   bn      Defined FCCCH deint task in first slot and BCCH deint task in second slot
10/09/03   bn      Corrected the FCCCH data type in rxc_fccch_info_type
04/18/03   bn      Removed JCDMA Features in FER so that it can apply to DIAG
03/20/03   bn,sr   Added NCCCH_STATE to support simultaneously monitor BCCH and FCCCH
02/10/03   fc      Added page_match in rxc_wake_type.
11/19/02   hxw     Added rxc_gps_cmd_type support for gpsOne.
10/29/02   bn      Added new fields in the rxc_chan_config_cmd_type and chan_info
                   to support BC BCCH
08/13/02   sr      Removed rxc_idle_chan_config(). New SRCH-RXC interface for
                   Fast RAHO replaces this.
07/11/02   bn      Modified function prototype to support new QIB logic
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up
                   (feature FEATURE_IS2000_REL_A_VP2.)
04/19/02   jrp     Removed fch_incl from rxc_get_setpt_cmd_type.
04/01/02   lad     Added members needed for BCCH/FCCCH frame info logging.
03/27/02   ems     Modified sizes of ser and energy fields, featurized with
                   MSMHW_EXTENDED_ENERGY_MATRIX.
02/27/02   lad     Merged IS2000 Rel A Common Channels:
                   Added rxc_idle_chan_config().
                   Added rxc_idle_activate().
                   Assigned members of rxc_chan_type to the values of
                   caix_chan_type to eliminate need for translation.  Updated
                   rxc_idle_cmd_type to contain chan_id.  Added 'length' and
                   'frame' fields to rxc_dec_type.
                   Added the complete BCCH/FCCCH cmd interfaces & unused code.
02/25/02   bn      Added support for  MUX SVD signaling
01/21/02   bgc     Merged data changes for disabling MDR.
01/03/02   bgc     Added SCH featurization to turn off for MSM6000.
12/10/01   hrk     Renamed RXC_FULL_FRAME to RXC_GOOD_FRAME.
11/13/01   bn      Added the enhance DCCH DTX detection and channel supervision logics
08/22/01   bn      Support DCCH in FTM mode
06/15/01   snn     Added extern for new functions for DCCH UI display.
06/15/01   day     Mainlined FEATURE_FFPC, FEATURE_IS95B, FEATURE_RXC_ACTION_TIME_PROCESSING,
                   FEATURE_IS2000
                   Deleted FEATURE_FFPC_OLD, FEATURE_SPECIAL_MDR
06/11/01   fc      Removed #ifdef for rxc_pc_fade_type.
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
06/01/01   snn     Modified the rxc_get_sch_info to return SCH RC. Added
                   prototype for rxc_get_fch_rate_set()
05/11/01   jq      Added RXC_NO_FRAME.
05/03/01   lcc     Changed rxc_returned_type to include "included" field for
                   rev_pwr_cntl_delay.  Also made some minor restructuring.
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2
03/20/01   ych     Merged JCDMA Features
03/08/01   va      Added rxc_get_setpt_cmd_type data structure.
03/07/01   va      ASdded p_rev_in_use in the TCI_F and RETURNED_F interface.
02/01/01   tc      Updated RXC_RIF_HHO_OR_CFS_F to RXC_RIFHHO_CFS_GPS_F.
01/29/01   tc      Merged GPS support.
11/29/00   jq      Moved defination of rxc_wake_type.slot out of BROADCAST_SMS.
10/04/00   jq      Added FTM FCH and SCH loopback feature
09/06/00   bgc     Added FEATURE_FACTORY_TESTMODE.
07/19/00   hrk     Moved member rev_pwr_cntl_delay in rxc_tci_cmd_type out of
                   FEATURE_FFPC.
07/17/00   va      Added support for rev pwr cntl delay.
07/14/00   rm      Added suuport for multiframe interleaving.
06/05/00   fc      Added rxc_olr_cmd_type.
06/02/00   lad     Changed order of rxc_frame_type enum for logging.
05/31/00   fc      Added command RXC_OLR_F.
05/26/00   jq      Added support for the new rxc design to support BCSMS
05/18/00   et      added support for dtx in sch
04/24/00   hrk     Added new transaction processing support.
04/12/00   hrk     Added prototype for rxc_send_rlp_frames_log().
04/10/00   dgy     Removed FEATURE_IS2000_QPCH.
04/07/00   va      Added RXC_RF_TUNE_F (HHO fix from PP)
03/27/00   sr      Changed the type of scc_energy[] from word to int2
03/24/00   bgc     Added enable_skip_tci in tci_cmd_type for PLT.
03/24/00   na      Added prototype for rxc_init_sch_mux().
03/16/00   na      Included prototypes for rxc_get_sch_info().
02/02/00   hrk     Merged with version GV40401 rxc.h from ASW archive:
           dna     Added timeout and is_hho as payload to the
                   RXC_RIF_HHO_OR_CFS_F command.
           aaj     Added FEATURE_DETECT_SUP_MUTING for MDR sup muting detection
01/31/00   rm      Clean up TEMP_HACKS_FOR_TIGER.
01/24/00   rm      Added in support for ACTION TIME processing.
12/17/99   va      Added transaction_num to rxc_pwr_ctrl_cmd_type,
                   Removed return_from_hho from rxc_returned_type interface.
12/13/99   hrk     Introduced new RXC-FFPC interface. Older implementation
                   of FFPC is featurized to FEATURE_FFPC_OLD
12/01/99   hrk     SOme FFPC parameters expanded to quadword range.
11/22/99   va      Expanded the following MC-RXC interfaces for the IS2000
                   support:
                   RXC_TCI_F
                   RXC_SO_F
                   RXC_RETURNED_F
                   RXC_HO_F (new)
                   RXC_PWR_CTRL_F (new)
                   RXC_FPC_SUBCHAN_GAIN_F (new)
                   The old RXC_HO_F has been replaced by RXC_HHO_BEGIN_F.
11/09/99   rm      Added decode type as part of RXC_SCH command to distinguish
                   between Convolutional / Turbo coding.
11/04/99   lcc     Corrected FFPC featurization in txc_tci_cmd_type.
                   Merged from GD4021:
           kmp     Mainlained the FEATURE_ENHANCED_STANDBY_II feature by
                   removing the #ifdef around the rxc_load_pc_isr() prototype.
           ry      Added extern declaration for rxc_flag_good_frame()
10/27/99   ajn     Added QPCH support
10/21/99   rm      Added new command interface RXC_SCCH_F to support MDR
                   for MSM5000.
10/07/99   lcc     Merged from PLT 2.0:
           jcw     Added support for F3 message control for PLT demod
           hrk     Added support for Outer Loop on F-FCH
           rm      Added new Interface for SCH.
09/17/99   lcc     Merged in new interface definitions for IS2000 and MSM5000
                   support from PLT archive:
                   1. Fixed problem in tci struct when building for MSM3000
                   2. Removed T_MSM5000PLT.
                   3. Made changes in TCI command to add RC parameter for
                      IS2000. Also  changes dec_tc_type to dec_fch_type.
08/26/99   aaj     Merged Panther Branch 3.1 to Mainline
08/25/99   aaj     Added new definition for sup channel SER threshold for RS2
08/09/99   ry      Combined FEATURE_IS95B_HARD_HO and FEATURE_IS95B_CFS into
                   FEATURE_IS95B_MAHHO
07/09/99   ry      Added RXC_RIF_HHO_OR_CFS_F and RXC_RETURNED_F
06/17/99   kmp     Merged in the one change listed below.
06/07/99   ks      Added rxc_pc_fade_type enum for various fade types.
04/06/99   aaj     Removed functions prototypes for RXDSP clock changes.
                   Added function prototype for rxc_voc_xchg()
02/26/99   ejv     Added prototype for rxc_load_pc_isr, part of FEATURE_
                   ENHANCED_STANDBY_II.
02/03/99   aaj     Added protoype declarations for RXDSP clk change routines
01/15/99   aaj     Merged MDR branch to Panther mainline
11/20/98   aaj     Added support for Medium (High) data rate sup channels
11/03/98   abh     Changes for frame no. correction after frame offset change.
09/17/98   bgc     Added rxc_set_frame_offset().
08/31/98   pms     Added Tan's Pascal calling convention code for ROM savings
09/06/96   gb      Added dec_date rate determination field to dec buffer type.
04/09/96   rdh     Added funct to return whether or not we're in Paging slot.
03/06/96   jca     Added RXC_PWR_REP_DELAY_F to rxc_cmd_name_type.
12/12/95   jca     Changed (there is no) rx_cmd_q to rxc_cmd_q.
08/10/95   gb      Added New Markov Service Options and made all service
                     options CAI_SO_*.
07/26/95   rah     Added rxc_reset_voc_init call.
07/13/95   gb      Changed mux option initialization.
06/14/95   gb      Added support for Rate set 2 service options.
08/09/94   jca     Added support for IS96/IS96A dual-capability vocoder.
03/24/94   gb      Added called interface to set isr.
06/24/92   jai     First Revision
10/10/90   rrd     created file

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "qw.h"
#include "queue.h"
#include "dec.h"
#include "deint.h"
#include "cmd.h"

#include "cai.h"
#include "caix.h"
#include "mctrans.h"
#include "rxcmc.h"

#if  defined (FEATURE_IS95B_MDR)
#include "mdrrlp.h"
#endif

#if !defined (FEATURE_IS95B_MDR) && defined (FEATURE_DS_RLP3)
#include "rlp.h"
#endif

#include "mux1xdiag.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
extern boolean rxc_sup_dec_int_enable;
extern boolean rxc_turbo_dec_enabled;

#ifdef FEATURE_IS2000_REL_A_CC_SMMC
#define  RXC_FCCCH_DEINT_TASK_SLOT_NUMBER     0x01  /* FCCCH deint task in the first 5 ms*/
#define  RXC_BCCH_DEINT_TASK_SLOT_NUMBER      0x02  /* BCCH deint task in the second 5 ms*/
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

/* RXC Traffic channel identifiers */
typedef enum
{
  RXC_F_FCH,
  RXC_F_SCH,
  RXC_F_DCCH,
  RXC_F_PDCCH0,
  RXC_F_PDCCH1,
  RXC_F_RCCH,
  RXC_F_ACKCH,
  RXC_F_GCH0,
  RXC_F_GCH1
} rxc_tc_chan_id_type;

// Definitions for physical channels carrying signalling
typedef enum
{
  RXC_FOR_FCH  = 0x1,
  RXC_FOR_DCCH = 0x2,
  RXC_FOR_PDCH = 0x3

} rxc_dsch_type;

typedef enum {
  RXC_GOOD_FRAME    = 0,
  RXC_DTX_FRAME     = 1,
  RXC_ERASURE_FRAME = 2,
  RXC_NO_FRAME      = 3
} rxc_frame_type;



#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
/* Indicates whether DCCH channel is good or bad quality  */
typedef enum
{
  RXC_GOOD_CHANNEL,
  RXC_POOR_CHANNEL
}
rxc_dcch_quality_type;
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION   */



/* -----------------------------------------------------
** This represents the state of the mobile while it
** it is monitoring the Paging channel for Fade reports.
** ----------------------------------------------------- */
typedef enum
{
  CDMA_IDLE_FADE,
  CDMA_ACCESS_FADE
} rxc_pc_fade_type;


#ifdef FEATURE_GPSONE_OVERRIDE_RDA
#error code not present
#endif



#ifdef FEATURE_BROADCAST_SMS
#define RXC_BC_PEEK_BOUNDRY 7
/* the number of frames left before RXC stop peeking into a BC SMS slot for the
 * beginning of a Data Burst Message */
#endif /* FEATURE_BROADCAST_SMS */



typedef struct
{
     byte sch_id;
     byte sch_init_setpt;
     byte sch_FER;
     byte sch_min_setpt;
     byte sch_max_setpt;
}  rxc_fpc_sch_params_type;

typedef struct
{
    byte for_sch_id;
    qword start_time;
     byte for_sch_duration;
    boolean for_sch_fer_rep;
    byte for_sch_rate; //This will eventually be an enum
}  rxc_sch_params_type;

typedef struct
{
     rxc_hdr_type hdr;
     byte transaction_num;
     byte num_for_schs_assigned;
     rxc_fpc_sch_params_type fwd_sch_params[2];  //Real size is same as above
     boolean fpc_incl;
     byte fpc_mode;
     boolean fpc_olpc_sch_master_incl;
     byte fpc_sec_chan;
     byte num_schs;
     rxc_sch_params_type  sch_params[2]; //Real size is as above
     boolean  fch_sch_setpt_thresh_incl;
     byte  fch_sch_setpt_thresh;
}  rxc_escam_cmd_type;



/* Supplemental Channel (SCH) Config Info */
typedef struct {
  cai_radio_config_type rc;
  cai_sch_rate_type     sch_rate;       // 1X, 2X, 4X, 8X, 16X
  cai_sch_coding_type   code_type;      // 0 - convolutional, 1 - turbo
} rxc_sch_info_type;



/* A union of all TC channels */
typedef struct {
  rxc_tc_chan_id_type chan_id; /* Type of channel */

  union {
    rxc_sch_info_type sch;
 } config;
} rxc_tc_chan_info_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Decoder Signalling Data buffer types */

/* Sync Channel buffer type */
typedef byte rxc_dec_sc_type [ DEC_96_BITS / 8 ];

/* Paging Channel buffer type */
typedef struct
{
  dec_pc_type bits;
  qword frame;
} rxc_dec_pc_type;

/* ------------------------------------------------------------------------------------ */

#define RXC_TOTAL_TC (DEC_192_BITS + DEC_96_BITS + DEC_48_BITS + DEC_24_BITS)

/* ------------------------------------------------------------------------------------ */
#if defined (FEATURE_IS95B_MDR) || defined (FEATURE_DS_RLP3)

#define RXC_SCC_ERASURE_THRESHOLD       77      /* Same as for Fundamental channel
                                                   full rate data. see function
                                                   rxc_9600_dec_rate_sel() for details */

#define RXC_SCC_ERASURE_THRESHOLD_RS2   50

/* Due to better data rate the RS2 generally is less prone to Symbol errors. The
** other factor is that the chainback length (number of symbols viterbi decoder
** traces back) is 96 for RS2 whereas it is 64 for RS1 in >= MSM2.3. Hence
** RS2 SER threshold can be lower than RS1.
*/

typedef struct {
    byte num_sup;                          /* number of supplementals */
    byte scc_crc;                          /* CRC status for sup channels */
    uint16 scc_ser[DEC_MAX_SCC_NUM];         /* SER for supplemental channels */
    int32  scc_energy[DEC_MAX_SCC_NUM];     /* Energy for supplemental channels */
    dword bits[(DEC_SCC_SIZE_14400/2) * DEC_MAX_SCC_NUM ]; /* dword buffer for
                                                           ** SCC channel data.
                                                           ** DEC_SCC_SIZE is
                                                           ** in word16 units */
  } rxc_scc_buf_type;

/* Traffic Channel buffer type */
typedef struct
{
  dec_fch_type bits;
  qword frame;
  rxc_scc_buf_type scc;
} rxc_dec_tc_type;

#else /* defined (FEATURE_IS95B_MDR) */

/* Traffic Channel buffer type */
typedef struct
{
  dec_fch_type bits;
  qword frame;
} rxc_dec_tc_type;

#endif /* defined (FEATURE_IS95B_MDR) */

#ifdef FEATURE_IS2000_REL_A_CC
//#define RXC_MAX_FRAME_SIZE (744/8) /* Currently, BCCH is largest supported */
/* this is not true for the case CCCH_4x...it could be up to 96 bytes */
#define RXC_MAX_FRAME_SIZE (96) /* Currently, BCCH is largest supported */
#endif

/* ------------------------------------------------------------------------------------ */

/* Type to be placed on rxc_dec_q */
typedef struct
{
  q_link_type  link;     /* queue link */
  word         state;    /* State of RXC state machine when bits were
                                received */
  cai_data_rate_type dec_rate;      /* rate determination for F-FCH this frame  */
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  cai_data_rate_type dcch_dec_rate; /* rate determination for F-DCCH this frame */
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
  rxc_dcch_quality_type  dcch_chan_quality;
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION   */

#ifdef FEATURE_IS2000_REL_A_CC
  //TODO: Determine how to represent frame time for new idle channels that support < 20ms frames.
  //  Eventually, it would be nice for all of RXC to use the same field instead of having a field
  //  for frame time in every struct.
  //  Current suggestion: use relative PCG time represented as a uint16, stored as a uint32
  //  (to avoid shift instructions to mask out rollover bits during arithmetic operations).
  //  pcg_time >= 2^16 indicates rollover.
  //  This is quick to get from CDMA timestamp (qw_lo (cdma_time) >> 16).
  //  RXC only needs relative frame times for most of its processing (I think).

  unsigned int length; /* Length of following buffer */ //TODO: Review.
  qword  frame;    //FIXME: Only BCCH uses this field.  FCCCH will some day
                   //need  5ms granulatrity, so 20ms frame times is not
                   //sufficient.
#endif

  union
  {
    rxc_dec_sc_type sc_buf;  /* Bits received on Sync Channel */
    rxc_dec_pc_type pc_buf;  /* Bits received on Paging Channel */
    rxc_dec_tc_type tc_buf;  /* Bits received on Traffic Channel */
#ifdef FEATURE_IS2000_REL_A_CC
    byte buf[RXC_MAX_FRAME_SIZE]; /* Generic buffer */
#endif
  } buf;
} rxc_dec_type;

/*-------------------------------------------------------------------------*/
/* Type for sending storing symbol error rates */
typedef struct
{
  dword frame_cnt;
  dword ser;
} rxc_sc_ser_type;

typedef rxc_sc_ser_type rxc_pc_ser_type;

typedef struct
{
  dword frame_cnt;  /* Total frame count */
  dword ser1;   /* Sum of all 192 bit frame symbol error counts */
  dword ser2;   /* Sum of all 96 bit frame symbol error counts */
  dword ser4;   /* Sum of all 48 bit frame symbol error counts */
  dword ser8;   /* Sum of all 24 bit frame symbol error counts */
} rxc_ser_type;

typedef struct
{
  uint8   spr_rate;     /* spreading rate in chip*/
  uint8   rnd_factor;   /* rounding factor 2 or 3 */
  uint8   rep_factor;   /* repeat factor: 1, 2 or 4 */
} rxc_chan_demod_config_param_type;

#ifdef FEATURE_IS2000_REL_A_CC
extern const rxc_chan_demod_config_param_type
  rxc_bcch_demod_config_param_tab[CAI_CODE_RATE_COUNT][CAI_BCCH_RATE_COUNT];

extern const rxc_chan_demod_config_param_type
  rxc_fccch_demod_config_param_tab[CAI_CODE_RATE_COUNT][CAI_FCCCH_DATA_RATE_INVALID];

extern boolean rxc_get_chan_demod_config_param
(
  /* pointer to channel info (typically from a chan config msg) */
  rxc_idle_chan_info_type          *chan_info,

  /* pointer to struct to return info in */
  rxc_chan_demod_config_param_type *demod_params
);
#endif /* FEATURE_IS2000_REL_A_CC */

extern boolean rxc_get_chan_tc_demod_config_param
(
  /* pointer to channel info (typically from a chan config msg) */
  rxc_tc_chan_info_type            *chan_info,

  /* pointer to struct to return info in */
  rxc_chan_demod_config_param_type *demod_params
);

extern word rxc_rpt_voc_rate(void);
  /* report current vocoder frame rate */

#ifdef FEATURE_JCDMA_MONITOR
extern dec_rate_set_type rxc_get_rate_set( void );
  /* report current rate set in use */
#endif /* FEATURE_JCDMA_MONITOR */

extern void rxc_ser_reset( void );
  /* reset ser statistics */

extern void rxc_ser_report ( rxc_ser_type * );
  /* report on the collected ser statistics */



extern void rxc_clear_isr (void);
  /* set the DEC_INT isr to NULL for now */

extern void rxc_set_pc_isr (void);
  /* setup the DEC_INT isr as the paging isr */



extern void rxc_voc_xchg (void);
  /* Writes the decoded over the air voice bits to the vocoder */

#if  defined (FEATURE_IS95B_MDR) || defined (FEATURE_DS_RLP3)

extern rlp_rate_enum_type rxc_scc_rate_sel_rlp( rxc_dec_type *buf_ptr, byte sup_index );
  /* Returns RLP Variable. Returns if the sup channel is erasure or full rate frame */

#ifdef FEATURE_DETECT_SUP_MUTING
extern void rxc_sup_mute_init( void );
  /* initialize data indicator for each supplemental to 1 */

extern rlp_rate_enum_type rxc_sup_mute(rlp_rate_enum_type sup_status, byte si );
 /* This function executes a algorithm to detect muting of sup channels */
#endif /* FEATURE_DETECT_SUP_MUTING */

#endif /* FEATURE_IS95B_MDR */

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION RXC_IDLE_ACTIVATE

DESCRIPTION
  This externally-called function instructs RXC to activate idle state
  processing.  This is intended for slotted mode operation in which the
  idle state needs to be activated very quickly and, therefore, cannot wait
  for RXC to process a task command to do so.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_idle_activate ( void );

#else
/*===========================================================================

FUNCTION RXC_LOAD_PC_ISR

DESCRIPTION
  This externally-called function installs the PC ISR and controls
  whether or not the decoder should have its state metrics
  cleared until the first frame in the slot.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_load_pc_isr ( boolean blank_frame );
#endif /* FEATURE_IS2000_REL_A_CC */


#ifdef FEATURE_IS95B_MAHHO
/* <EJECT> */
/*===========================================================================

FUNCTION RXC_FLAG_GOOD_FRAME

DESCRIPTION
  This function is called by SRCH during a Return-If-Fail HHO, after
  verifying none of the HHO failure conditions is met.  This is to
  inform RXC that it's okay to start processing good frame.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RXC will start processing good frame

===========================================================================*/
extern void rxc_flag_good_frame( void );
#endif /* FEATURE_IS95B_MAHHO */

/*========================================================================

FUNCTION RXC_POWER_COMB_CONFIG

DESCRIPTION This function configures the power control puncturing position
            and PUNCT_POS_SEL based on Radio Configuration

DEPENDENCIES Power control and puncturing must be enabled before.

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_power_comb_config
(
  deint_rc_type rc
);

/*========================================================================

FUNCTION RXC_SET_MAC_RND

DESCRIPTION This function sets the MAC Engine Rounding factors based on the
             spreading Rate.

DEPENDENCIES MAC Engine should be enabled.

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_set_mac_rnd
(
  deint_rc_type rc,
  dec_sch_rate_type sch_rate,
  dec_coding_type code_type

);
#ifdef FEATURE_DS_IS2000
/*========================================================================

FUNCTION RXC_GET_SCH_INFO

DESCRIPTION This function returns current configuration information for
            the SCHs. This is used by the UI in the data display.

            PARAMETERS:
            sch_rc       - gives the Radio configuration of the R-SCH (used
                           only if RS2 is defined )
            sch_rate_ptr - points to the current SCH rate.
            double_ptr   - is TRUE when double sized PDUs are being used
                           is FALSE when single sized PDUs are being used

DEPENDENCIES None

RETURN VALUE TRUE:  If SCH are assigned.
             FALSE: If no SCH are assigned.

SIDE EFFECTS None

=========================================================================*/
boolean rxc_get_sch_info
(
  dec_rc_type*       sch_rc,         /* ptr to SCH RC */
  dec_sch_rate_type* sch_rate_ptr,
  boolean* double_ptr
);


/*========================================================================

FUNCTION RXC_GET_FCH_RATE_SET

DESCRIPTION This function returns current Rate set used by FCH.
            This is used by the UI in the data display.

            PARAMETERS:
            None

DEPENDENCIES None

RETURN VALUE

SIDE EFFECTS None

=========================================================================*/
dec_rate_set_type rxc_get_fch_rate_set(void );

#ifdef FEATURE_IS2000_P2
/*========================================================================

FUNCTION RXC_GET_DCCH_RATE_SET

DESCRIPTION This function returns current Rate set used by DCCH.
            This is used by the UI in the data display.

            PARAMETERS:
            None

DEPENDENCIES None

RETURN VALUE
            Rates set of the DCCH.

SIDE EFFECTS None

=========================================================================*/
extern dec_rate_set_type rxc_get_dcch_rate_set(void );

/*===========================================================================

FUNCTION RXC_DCCH_IS_ON

DESCRIPTION
  Returns if DCCH is ON.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If DCCH is On
  FALSE: If DCCH is Off

SIDE EFFECTS
  None
===========================================================================*/
extern boolean rxc_dcch_is_on(void );

#endif /* FEATURE_IS2000_P2*/
/*========================================================================

FUNCTION RXC_INIT_SCH_MUX

DESCRIPTION This function initializes the array that will be used to hold
            the SCH Mux frames for RLP

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxc_init_sch_mux( void );

#endif  /* FEATURE_DS_IS2000 */

/*========================================================================

FUNCTION RXC_SERVICE_TRANS_CMD

DESCRIPTION
   This function services a group of transactions at action time

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxc_service_trans_cmd( rxc_cmd_msg_type *cmd_ptr );

#ifdef FEATURE_IS2000_F_SCH
/*========================================================================
FUNCTION RXC_FWD_SCH_TRANS_CMD

DESCRIPTION
   The fwd. SCH transaction callback

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None
========================================================================*/
void rxc_fwd_sch_trans_cmd( mctrans_action_packet_type *fwd_sch_packet );

#endif // FEATURE_IS2000_F_SCH


/*========================================================================

FUNCTION RXC_DTX_DET_SCH

DESCRIPTION This function determines if the current frame is a DTX frame,
an erasure frame, or a full frame.

DEPENDENCIES None

RETURN VALUE enum, either RXC_GOOD_FRAME, RXC_DTX_FRAME, or RXC_ERASURE_FRAME


SIDE EFFECTS None

=========================================================================*/
rxc_frame_type rxc_dtx_det_sch(dec_rc_type rc, boolean crc_pass);



/*===========================================================================
FUNCTION      RXC_GET_AVERAGED_FER

DESCRIPTION   This function gets erasure frames for poor FER alarm.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
extern void rxc_get_averaged_fer
(
  mux1xdiag_fer_s_type *fer                          /* pointer to store FER data */
);


#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
/*========================================================================

FUNCTION RXCRDA_DCCH_INIT

DESCRIPTION This function returns the initial threshole frame quality value.

DEPENDENCIES Shoud be called after the receiving of the ECAM

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxcrda_dcch_init( void );

/*========================================================================

FUNCTION RXC_DCCH_QUALITY_DET

DESCRIPTION This function returns the quality of the channel to assist in
channel setup and maintenance.

DEPENDENCIES None

RETURN VALUE enum rxc_dcch_quality_type: RXC_GOOD_CHANNEL, RXC_POOR_CHANNEL

SIDE EFFECTS None

=========================================================================*/
rxc_dcch_quality_type rxc_dcch_quality_det(rxc_frame_type  rda_decision );

#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION   */

#ifdef FEATURE_UNUSED_MUX1X_CODE
#error code not present
#endif /* FEATURE_UNUSED_MUX1X_CODE */

#endif /* RXC_H */

