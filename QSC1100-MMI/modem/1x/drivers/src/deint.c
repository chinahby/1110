/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  M O B I L E   C H A N N E L  D E I N T E R L E A V E R    S E R V I C E S

GENERAL DESCRIPTION
  Library of routines to initialize, and operate on, the Qualcomm CDMA
 Channel Deinterleaver chip.

Copyright (c) 1999 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/src/deint.c#2 $
$Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/src/deint.c#2 $ $DateTime: 2009/03/06 13:36:02 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/09   jtm     Cleaned up compiler warnings
03/03/09   jtm     Lint fixes.
01/16/09   vlc     Moved Turbo decoder functions under MSMHW_TURBO_CODING.
10/23/08   jtm     Removed T_QUASAR
08/12/08   jtm     Added DEINT_UPPER_BOUNDS_CHECK for bounds checking.
07/16/08   jtm     Eliminated obsolete code and reduced debug string sizes to save
                   ROM for the ULC effort.
07/01/08   vlc     Merged code changes from old tip archive.
04/10/08   trc     Switch clock mgmt strategy via FEATURE_SRCH_CLKREGIM_RM
03/31/08   mca     Changed FEATURE_CLKREGIM_RM to FEATURE_SRCH_CLKREGIM_RM
03/05/08   vlc     User clock resource manager to manage RX clocks 
                   (FEATURE_CLKREGIM_RM.)
11/22/06   vlc     Fixed RVCT compiler warnings.
09/19/06   vlc     Fixed lint errors.
06/05/06   bn/vlc  In function deint_allocate_ram_nccch(), swap RAM allocation
                   for channel 2 to fix issue with SCH in traffic state.
03/31/03   bn      Make function deint_allocate_CH0_RAM becomes public
01/03/05   rkc     In deint_set_sc_mode(), allocated channel 0 RAM to
                   fix problem when switching between 1x only/hybrid modes.
01/03/05   rkc     Change deint_allocate_ram_nccch() to use L-M deinterleaver RAM
                   instead of N-P RAM since N-P RAM is used by HDR in simultaneous
                   1/x/HDR mode.  Also turn on L0 and M0 extended RAM since original
                   768 symbols cannot handle max 3072 frame size.
01/03/05   rkc     Change DINT_TASK_LIST's start address for decoder output buffer
                   for FCCCH data since FCCCH moved to short buffer offset.
04/21/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
03/30/05   bn      Removed obsoleted debug message
01/14/05   jyw     Added the macros for DINT_CH3_CFG
12/17/04   jyw     Added the initialization to DINT_CH3_CFG
11/30/04   vlc     Added debug F3 message to show Conv SCH parameters.
06/10/04   sr      Fixed shifts for TD_PARAMS_LO to match the changes in MSM6700
                   Turned on clocks to DEINT & DEC fro QUASAR
05/06/04   sr      Moved out DV driver code to separate file
05/05/04   sr      Fixed typo in PDCH #defines & defined DEVO_REV_D_HW
04/13/04   sr      Added DINT PDCCH configuration parameters and modified
                   deint_set_pdcch_mode() to program these into DINT.
                   Replaced num_pdcch with hw_test_mode input parameter to
                   deint_set_pdcch_mode().
09/03/03   dna     Ported for MSM6500
08/08/03   vlc     In function deint_create_dcch_task_list(), changed output 
                   buffer address of DCCH task to decouple FCH/DCCH rate sets.
07/10/03   vlc     Added DEC_SCH_DUMMY_FULL_OB_ADDR.  
                   This address is used for Turbo SCH dummy task and is beyong 
                   any critical decoder output buffer adresses.                           
                   Got rid of unused parameter for local Rel A function
                   deint_create_fch_task_list().
                   Added initialization for curr_deint_ch_active at start of
                   traffic channel.
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
03/07/03   vlc     Changes are for Release A.  Created function
                   deint_load_dcch_pkt_params() and use deint packet F for 
                   DCCH rather than deint packet A so FCH and DCCH can
                   co-exist with different RC.
02/25/03   vlc     Changes are for MSM6100, Release A:
                   Added logic to solve Turbo finite burst assignment issues
                   brought on by moving Turbo dummy SCH interrupt to 3rd bin.
                   Added new functions deint_atmr_isr2 () and 
                   deint_atmr_isr2_init ().  These functions support the use 
                   of a second Action Timer ISR which is set to go off at the 
                   4th bin in a frame.
                   In function deint_delete_supplemental_task_list(), now zero
                   out 3rd bin task list for Turbo SCH.
01/21/03   vlc     Put Turbo SCH dummy task on 3rd bin code under release A
                   feature.
11/27/02   cr      Moved MSMHW_EXTENDED_LTU_LEN msm6100hw.h
11/26/02   sr/vlc  Moved Turbo SCH dummy task to 3rd bin for MSM6100.        
11/25/02   vlc     Added MSMHW_EXTENDED_LTU_LEN feature to handle dynamic    
                   LTU length in function deint_load_sch_pkt_params().       
10/22/02   vlc     Added RS1 Turbo 32X parameters.  Changed 2 parameters of RS2
                   Turbo 16X.  Removed code to prohibit RS1 Turbo 32X config which
                   no longer applied.
08/26/02   sr      Added deint_bcch_seq_repetition()
07/16/02   dlb2    Resolved register differences between MSM5100 and MSM6050
                   for Rel A code.
07/17/02   vlc     Changed code (including the addition of various masks) to turn
                   off FCH interrupt when DCCH is dynamically added.
06/27/02   dlb     Added workaround for MSM6050 DEC1 problem.
06/21/02   vlc     Switched order of parameter list of 
                   deint_create_fch_task_list().
06/12/02   vlc     Removed unused functions deint_delete_fch_task_list()
                   and deint_delete_dcch_task_list(). 
                   Changed DEINT_MAX_SEQUENCING_CMD to 3 for VP2 support.
06/04/02   vlc     Added functions deint_delete_fch_dcch_task_list(),
                   deint_delete_fch_task_list(), and deint_delete_dcch_task_list().
                   Changed code to allow DCCH and FCH channel set up while in
                   traffic state with no active channels.
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
                   Implemented comments from VP2 code review.
05/16/02   dlb     Added support for modified LTU_LEN fields for MSM6050.
                   Featurized with MSMHW_EXTENDED_LTU_LEN flag.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up 
                   (feature FEATURE_IS2000_REL_A_VP2.)
03/15/02   vlc     No longer program deinterleaver to shift 1X SCH header bits
                   for FEATURE_IS2000_REL_A_SVD.
03/05/02   sr      Initialized deint task list with "zero" tasks in all the
                   4 5ms bins in deint_init() and removed creation of "zero"
                   tasks in other 5ms bins in deint_create_fch_task_list() 
                   as this could wipe out already programmed and active SCH task
02/25/02   sr      Merged in Release A common channels from the branch
01/23/02   dlb     Resolved register differences between MSM5100 and MSM6050
11/16/01   vlc     Modified deint_init () to call dec_clk_select ().
10/04/01   sr      Merged in fix for turbo decoder sequencing from MSM5000.
                   This solves turbo code overlapping burst problems.
09/25/01   lad     Moved internal constants and macros from deint.h, made 
                   tables 'static const' to save RAM, and made a few cosmetic
                   changes.
08/13/01   sr      Merged in from MSM5000 archive:
                   In deint_delete_supplemental_task_list, added removal of
                   DEINT_SCH_READ_PARAMS commands to make sure that any pending
                   SCH tasks are cleaned up.
08/10/01   lh      Added 32X support.
07/04/01   sr      Added support for mixed rate sets by defining independent 
                   deinterleaver packet E parameters for 1X SCH task instead of 
                   reusing packet A parameters loaded for FCH full rate task.
06/04/01   tc      Initialized HW_TD_INTLV_LEN_X2/X4/X8 to clear the bit.
02/20/01   sr      Added support for 32X
01/05/01   tc      Fixed the decoder clock source select to TCXO.
12/21/00   tc      Fixed the forward link turbo code.
12/13/00   vlc     Changed TD_MIN_LLR_WH to TD_MIN_LLR_THRESH_WH for MSM5100.
                   This change is necessary when MSMHW_TURBO_CODING is enabled.
11/14/00   tc      Added MSM5100 Decoder clock source select support.
10/24/00   rmd     Merge with MSM5000 code. Did some code clean up.
10/23/00   rmd     Included rxc.h so that we get access to the variables 
                   rxc_turbo_dec_enable and rxc_sup_dec_int_enable.
10/19/00   rmd     In deint_init(), removed call to clk_regime_enable(CLK_RGM_DEC_M).
                   This call is not necessary, this clk regime is already on.    
09/18/00   rm      enable clockregim after selecting clock source
09/08/00   jcw     Added include of clkregim.h to support call to clk_regime_enable
09/06/00   djd/et  Added ifdef MSMHW_TURBO_CODING around turbo coding software.
09/06/00   rm      Added clock regime changes for MSM5105.
09/06/00   rm      Enable Decoder clock regime in deint_init.
09/05/00   rm      Added sequencing support for Turbo decoder.
09/03/00   rm      Made changes to fix SCH sequencing of read/write parameters.
08/04/00   rm      Added support to sequence read parameters for convolutionally
                   encoded SCH frames. This is required for SCH Burst assigments. 
07/13/00   rm      Fixed the case where FCH data was being overwriiten if
                   SCH was turbo encoded.
07/12/00   rm      Added PLT support for 40/80ms MULTIFRAME INTERLEAVING
                   for suuplemental channels.
07/11/00   rm      Added PLT support for 5ms FCH frames. 
05/10/00   rm      Fixes to support 1X SCH.
04/05/00   va      Fixed bug in sending DEINT_SCCH_READ_PARAMS command.
04/15/00   rm      Created a new dummy task in the second 5ms slot when
                   Turbo Decdoing (SCH) is enabled. Turbo Decoder now
                   doesnot generate interrupts. This dummy task in the
                   second 5ms slot will generate a interrupt and this
                   should be associated with SCH interrpt processing for 
                   Turbo Decoded frames.
03/23/00   bgc     Changed yamamoto threshholds for RC1 and RC2 to 
                   [1, 1, 3, 3] for [full, half, quarter, eighth].  These
                   are the same thresholds as MSM3000 for RC1 and RC2
01/28/00   rm      Added in support for 1X SCH (RC3, RC4 & RC5).
01/24/00   rm      Added in support to sequence read/write parameters. The
                   write parameters are programmed one frame ahead of the read
                   parameters. This will allow us clean transiton if SCH rates
                   change or a change in RC, without loss of a frame.
11/15/99   rm      Added in support for RC5 SCH (convolutional codes).
11/09/99   rm      Added in support for RC3,RC4 & RC5 SCH (TURBO DECODER).
10/20/99   rm      Added in a new function to delete supplemental channel
                   task from deinterleaver task ram.
10/17/99   rm      Added in support for IS95B Supplemental Code Channel.
09/24/99   rm      Changes the Deinterleaver/Decoder clock to TCXO.
09/24/99   rm      Added in support for SCH (RC3 & RC4).
08/11/99   rm      Added support for RC4 and RC5 for F-FCH.
07/31/99   rm      Created file. Interfaces to support channel deinterleaver.

============================================================================*/
#include "msm_drv.h"   
#include "assert.h"
#include "deint.h"
#include "dec.h"
#include "msm.h"
#include "msg.h"
#include "err.h"
#include "queue.h"
#include "rxc.h"

#include "msmhwio.h"
#ifdef FEATURE_SRCH_CLKREGIM_RM
#include "clkregim.h"
#endif /* FEATURE_SRCH_CLKREGIM_RM */


/*=======================================================================
   
   Macros to resolve register name differences between platforms

========================================================================*/
  #define DINT_CH0_CFG_MASK                      HWIO_RMSK(DINT_CHw_CFG)
  #define DINT_CH1_CFG_MASK                      HWIO_RMSK(DINT_CHw_CFG)
  #define DINT_CH2_CFG_MASK                      HWIO_RMSK(DINT_CHw_CFG)
  #define DINT_CH0_CFG__SYNC_CHAN_80M_BND_MASK   HWIO_FMSK(DINT_CHw_CFG,SYNC_CHAN_80M_BND)
  #define DINT_CH0_CFG__NUM_SUB_CHANS_MASK       HWIO_FMSK(DINT_CHw_CFG,NUM_SUB_CHANS)
  #define DINT_CH1_CFG__NUM_SUB_CHANS_MASK       HWIO_FMSK(DINT_CHw_CFG,NUM_SUB_CHANS)
  #define DINT_CH2_CFG__NUM_SUB_CHANS_MASK       HWIO_FMSK(DINT_CHw_CFG,NUM_SUB_CHANS)
  #define DINT_CH0_CFG__NEXT_20M_BND_MASK        HWIO_FMSK(DINT_CHw_CFG,NEXT_20M_BND)
  #define DINT_CH1_CFG__NEXT_20M_BND_MASK        HWIO_FMSK(DINT_CHw_CFG,NEXT_20M_BND)
  #define DINT_CH2_CFG__NEXT_20M_BND_MASK        HWIO_FMSK(DINT_CHw_CFG,NEXT_20M_BND)

  #define SET_TST_SEL_DINT(val)        HWIO_OUT  (TST_SEL_DINT, val)
  #define SET_DINT_CH_CFG(ch,mask,val) HWIO_OUTMI (DINT_CHw_CFG,ch,mask,val) 
  #define SET_DINT_PKT_OFFSET(val)     HWIO_OUT  (DINT_PKT_OFFSET, val)
  #define SET_DINT_TASK_OFFSET(val)    HWIO_OUT  (DINT_TASK_OFFSET, val)

/* for simultaneous SCH(ch2) + CCCH(ch1) + BCCH(ch3) + QPCH (ch0) when
   broadcasting across the whole system by using shared FSCH among idle
   MS, in MSM6700 DINT channel 3 is added */
  #define DINT_CH3_CFG__NEXT_20M_BND_MASK        HWIO_FMSK(DINT_CHw_CFG,NEXT_20M_BND)
/*=======================================================================
   
   Constants describing settings for deinterleaver registers.

========================================================================*/
         

/* Packet Values for  Packet A, B, C, D, E, and F */
#define DINT_PKT_A_V (0x00 << 0x03)
#define DINT_PKT_B_V (0x01 << 0x03)
#define DINT_PKT_C_V (0x02 << 0x03)
#define DINT_PKT_D_V (0x03 << 0x03)
#define DINT_PKT_E_V (0x04 << 0x03)
#define DINT_PKT_F_V (0x05 << 0x03)

/* since we could have both FCCCH and BCCH on at a time, we can't share
new packet is fined for FCCCH*/
#define DINT_PKT_G_V (0x06 << 0x03)



#define DINT_TASK_LIST_PACKET_A_M  (0x00 << 0x04) 
#define DINT_TASK_LIST_PACKET_A_V  (0x00 << 0x04) 
#define DINT_TASK_LIST_PACKET_B_M  (0x01 << 0x04) 
#define DINT_TASK_LIST_PACKET_B_V  (0x01 << 0x04) 
#define DINT_TASK_LIST_PACKET_C_M  (0x02 << 0x04) 
#define DINT_TASK_LIST_PACKET_C_V  (0x02 << 0x04) 
#define DINT_TASK_LIST_PACKET_D_M  (0x03 << 0x04) 
#define DINT_TASK_LIST_PACKET_D_V  (0x03 << 0x04) 
#define DINT_TASK_LIST_PACKET_E_M  (0x04 << 0x04) 
#define DINT_TASK_LIST_PACKET_E_V  (0x04 << 0x04) 
#define DINT_TASK_LIST_PACKET_F_M  (0x05 << 0x04) 
#define DINT_TASK_LIST_PACKET_F_V  (0x05 << 0x04) 

/* since we could have both FCCCH and BCCH on at a time, we can't share
new packet is fined for FCCCH*/
#define DINT_TASK_LIST_PACKET_G_M  (0x06 << 0x04) 
#define DINT_TASK_LIST_PACKET_G_V  (0x06 << 0x04) 



/* Offsets for packet parameters */
#define DINT_PKT_INTLV_CFG_LO_V 0x00
#define DINT_PKT_INTLV_CFG_HI_V 0x01
#define DINT_PKT_BLK_SIZE_LO_V 0x02
#define DINT_PKT_BLK_SIZE_HI_V 0x03
#define DINT_PKT_PUNCT_LO_V 0x04
#define DINT_PKT_PUNCT_HI_V 0x05
#define DINT_PKT_PL_CRC_LO_V 0x06
#define DINT_PKT_PL_CRC_HI_V 0x07


/* Timing trigger settings */
#define DINT_SYNC_CHAN_80M_BND_V 0x40
#define DINT_NEXT_20M_BND_V 0x20

/* RAM allocation settings */
#define DINT_LRAM_CHAN_ALLOC_MASK 0xC0
#define DINT_MRAM_CHAN_ALLOC_MASK 0x30
#define DINT_NRAM_CHAN_ALLOC_MASK 0x0C
#define DINT_PRAM_CHAN_ALLOC_MASK 0x03


/* Settings to assignment deinterleaver RAMs to channels */
#define DINT_CH0_RAM_L_V  0x00
#define DINT_CH0_RAM_M_V  0x00
#define DINT_CH0_RAM_N_V  0x00
#define DINT_CH0_RAM_P_V  0x00

#define DINT_CH1_RAM_L_V  0x40
#define DINT_CH1_RAM_M_V  0x10
#define DINT_CH1_RAM_N_V  0x04
#define DINT_CH1_RAM_P_V  0x01

#define DINT_CH2_RAM_L_V  0x80
#define DINT_CH2_RAM_M_V  0x20
#define DINT_CH2_RAM_N_V  0x08
#define DINT_CH2_RAM_P_V  0x02

#define DINT_L0_RAM_EXTEND_ON (0x1 << HWIO_SHFT(DINT_CFG,L0_RAM_EXTEND))
/* Enable DINT_CFG's L0_RAM_EXTEND when F-CCCH, F-BCCH and F-SCH cannot fit in original RAM size */
#define DINT_M0_RAM_EXTEND_ON (0x1 << HWIO_SHFT(DINT_CFG,M0_RAM_EXTEND))
/* Enable DINT_CFG's M0_RAM_EXTEND when F-CCCH, F-BCCH and F-SCH cannot fit in original RAM size */

/* Task list settings */
#define DINT_TASK_LIST_ACTIVE_TASK_M  (0x01 << 15)
#define DINT_TASK_LIST_ACTIVE_TASK_V  (0x01 << 15)
#define DINT_TASK_LIST_SUB_CHANS_EN_V (0x01 << 14)
#define DINT_TASK_LIST_SYNC_CHAN_EN_V (0x01 << 13)
#define DINT_TASK_LIST_DEC_INTR_EN_M  (0x01  << 12)
#define DINT_TASK_LIST_DEC_INTR_EN_V  (0x01  << 12)
#define DINT_TASK_LIST_DEC_INTR_DIS_V (0x00  << 12)
#define DINT_TASK_LIST_CH0_M       (0x00 << 10)
#define DINT_TASK_LIST_CH0_V       (0x00 << 10)
#define DINT_TASK_LIST_CH1_M       (0x01 << 10)
#define DINT_TASK_LIST_CH1_V       (0x01 << 10)
#define DINT_TASK_LIST_CH2_V       (0x02 << 10)
#define DINT_TASK_LIST_FRM_HYP_20MS_M (0x02 << 8)
#define DINT_TASK_LIST_FRM_HYP_20MS_V (0x02 << 8)
#define DINT_TASK_LIST_FRM_HYP_5MS_V (0x00 << 8)

#define DINT_FIRST_5MS_SLOT_ADDR 0x0000
#define DINT_SECOND_5MS_SLOT_ADDR 0x0020
#define DINT_THIRD_5MS_SLOT_ADDR 0x0040
#define DINT_FOURTH_5MS_SLOT_ADDR 0x0060

#ifdef FEATURE_IS2000_REL_A_CC
/* IS2000 sequence repetition (used in BCCH) */
typedef enum {
  DEINT_SEQ_REP_1X = 0,
  DEINT_SEQ_REP_2X = 1,
  DEINT_SEQ_REP_4X = 2,
  DEINT_SEQ_REP_COUNT
} deint_seq_rep_type;

/* The argument is deint_seq_rep_type */
#define DEINT_CH_CFG_SEQ_REP(xx_seq_rep) (xx_seq_rep << 3)
#endif

/*=========================================================================
    DEINTERLEAVER PACKET PARAMETERS MACROS
==========================================================================*/

typedef struct
{
  word pkt_intlv_cfg_lo;
  /* row_width | chainback_depth | ym_thresh | smt_scalar */
  word pkt_intlv_cfg_hi;                   
  /* intlv_cols | intlv_rows */
  word pkt_blk_size_lo;                    
  /* Block size */
  word pkt_blk_size_hi;    
  /* punct_length | symbol_repeat | code_rate */
  word pkt_punct_lo;                      
  /* lo_punct_pattern | rdcnt_en_sel | num_ltu_frm */
  word pkt_punct_hi;  
  /* punct_pattern_hi */
  word pkt_pl_crc_lo;                      
  /* Physical Layer CRC Polynomial */
  word pkt_pl_crc_hi;                  
  /* LTU CRC Polynomial */

} deint_pkt_params_type;

typedef struct
{
  deint_pkt_params_type fch_full_rate_param;
  deint_pkt_params_type fch_half_rate_param;
  deint_pkt_params_type fch_quarter_rate_param;
  deint_pkt_params_type fch_eighth_rate_param;

} deint_fch_pkt_cfg_type;

typedef struct
{
  deint_pkt_params_type sc_param;
} deint_sc_pkt_cfg_type;

typedef struct
{
  deint_pkt_params_type pc_params[PRAT_COUNT];
} deint_pc_pkt_cfg_type;

#ifdef FEATURE_IS2000_REL_A_CC
typedef struct 
{
  deint_pkt_params_type bcch_params[CAI_CODE_RATE_COUNT];
} deint_bcch_pkt_cfg_type;
#endif

typedef deint_pkt_params_type deint_sch_pkt_cfg_type;
#ifdef FEATURE_IS2000_REL_A_CC
typedef deint_pkt_params_type deint_ccch_pkt_cfg_type;
#endif



#ifdef MSMHW_TURBO_CODING
/*=============================================================
   DEINTRELEAVER & TURBO DECODER Params for Turbo Codes
================================================================*/
typedef struct
{
  word td_block_size;
  /* Deinterleaver TD block size. Indicates the block size, N
     for de-interleaving. Programmed value should be N-1. */
  word td_intlv_cfg_lo;
  /* bits 13:15 indicate Row_width for the DeInterleaver(m=logM) 
     bits 0:12   reserved */
  word td_intlv_cfg_hi;
  /* bits 8:15 indicate number of columns for deinterleaver.
     bits 0:7 indicate number of rows (M = 2 ^ (m-1) )  */
  word td_min_llr;
  /* Indicates the Minimum Log-Likelihood ratio used for
     early termination check */
  word td_punct_lo;
  /* Punct_Pattern | Punct_Length | TD_DEC_INT_EN | Code_Rate */
  word td_punct_hi;
  /* bits 0:15 indicate the 16_upper bits of the puncturing pattern */
  word td_intlv_size_lo;
  /* bits 0:14 indicate the Turbo decoder interleaver length 
     bit 15 indicates if Early Termination is enabled or not */
  word td_intlv_size_hi;
  /* bits 0:3 reserved 
     bits 4:9 indicate Minimum number of iterations to be run before
     checking for early termination word td_intlv_size_hi;
     bits 10:15 indicate Maximum number of iterations to be run */
  word td_params_lo;
  /* Indicate the length of Sliding Window, Beta Scalinf factor */
  word td_params_hi;
  /* Indicate the minimum number of CRC checks (pass) and the number
    of windows for the Turbo Decoder */
} deint_td_params_type;  
#endif /* MSMHW_TURBO_CODING */
   

/*=======================================================================

   Data Structures for Deinterleaver/Decoder Sequencing Commands to
   Program Read Parameters.

========================================================================*/
typedef enum
{
  DEINT_FCH_READ_PARAMS,
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  DEINT_DCCH_READ_PARAMS,
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
  DEINT_SCCH_READ_PARAMS,
  DEINT_SCH_READ_PARAMS
}  deint_sequencing_cmd_name_type;


typedef struct
{
  q_link_type link;   /* Queue Link */
  q_type *done_q_ptr;
  deint_sequencing_cmd_name_type command;
}  deint_hdr_type;

typedef struct
{
  deint_hdr_type hdr;
  deint_rc_type fch_rc;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  boolean fch_pc;     
    /* Indicates if this is the power control chn */
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
}  deint_fch_read_params_cmd_type;

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
typedef struct
{
  deint_hdr_type hdr;
  deint_rc_type dcch_rc;
  boolean dcch_pc;    
    /* Indicates if this is the power control chn */
}  deint_dcch_read_params_cmd_type;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

typedef struct
{
  deint_hdr_type hdr;
  deint_rc_type sch_rc;
  dec_coding_type dec_coding;
  dec_sch_rate_type  dec_sch_rate;
#ifdef FEATURE_IS2000_REL_A //32X
  uint8 num_ltu;
  uint16 ltu_len;
  dec_mux_pdu_type_type pdu_type;
#endif /* FEATURE_IS2000_REL_A */
}  deint_sch_read_params_cmd_type;

typedef struct
{
  deint_hdr_type hdr;
  deint_rc_type scch_rc;
  byte num_sup;
}  deint_scch_read_params_cmd_type;


/* Union of all the sequencing commands */
typedef union
{
  deint_hdr_type   hdr;
  deint_fch_read_params_cmd_type fch_cmd;
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  deint_dcch_read_params_cmd_type dcch_cmd;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
  deint_scch_read_params_cmd_type scch_cmd;
  deint_sch_read_params_cmd_type sch_cmd;
}  deint_sequencing_cmd_type;


#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/*=============================================================
   DEINTRELEAVER values to support Simultaneous FCH and DCCH.
================================================================*/
/* Used to decide whether or not Decoder should be reset.  This is used to 
** avoid resetting the decoder multiple times while the mobile is in traffic
** state.  
*/
typedef enum
{
  DEINT_SYNC_STATE,
  DEINT_PAGING_IDLE_STATE,
  DEINT_TRAFFIC_STATE

}  deint_phone_state_type;

/* Used to determine how to create the FCH/DCCH task lists.  
*/
typedef enum
{
  DEINT_CH_INACTIVE,
  DEINT_FCH_ACTIVE,
  DEINT_DCCH_ACTIVE,
  DEINT_FCH_DCCH_ACTIVE

}  deint_fundicated_ch_active_type;

#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

/*=========================================================================

    DEINTERLEAVER PACKET PARAMETERS FOR SYNC CHANNEL

==========================================================================*/

#define DINT_PKT_SC_ROW_WIDTH_V (0x04 << 13)
#define DINT_PKT_SC_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_SC_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SC_SMT_SCALAR_V (0xb5)
#define DINT_PKT_SC_INTLV_COLS_V (0x01 << 8)
#define DINT_PKT_SC_INTLV_ROWS_V (0x80 << 0)
#define DINT_PKT_SC_BLOCK_SIZE_V 0x0180
#define DINT_PKT_SC_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SC_SYMBOL_REPEAT_V (0x01 << 2)
#define DINT_PKT_SC_CODE_RATE_V (0x00)
#define DINT_PKT_SC_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SC_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_SC_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SC_PL_CRC_V (0x0000)
#define DINT_PKT_SC_LTU_CRC_V (0x0000)
#define DINT_PKT_SC_NUM_LTU_FRM_V 0x00


#ifdef FEATURE_IS2000_REL_A_CC
/*=========================================================================

    DEINTERLEAVER PACKET PARAMETERS FOR BCCH

==========================================================================*/

/* PKT_INTLV_CFG_HI */
#define DINT_PKT_BCCH_ROW_WIDTH_V (0x03 << 13) /* m=6 */
#define DINT_PKT_BCCH_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_BCCH_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_BCCH_SMT_SCALAR_V (0xff)

/* PKT_INTLV_CFG_LO */
#define DINT_PKT_BCCH_INTLV_COLS_V (0x18 << 8)
#define DINT_PKT_BCCH_INTLV_ROWS_V (0x40 << 0)

/* PKT_BLK_SIZE_LO */
#define DINT_PKT_BCCH_BLOCK_SIZE_V (0x0600)

/* PKT_BLK_SIZE_HI */
#define DINT_PKT_BCCH_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_BCCH_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_BCCH_CODE_RATE_HALF_V (0x00)
#define DINT_PKT_BCCH_CODE_RATE_QTR_V (0x02)

/* PKT_PUNCT_LO */
#define DINT_PKT_BCCH_LO_PUNCT_PATTERN_V (0xff << 8) /* no puncturing */
#define DINT_PKT_BCCH_RDCNT_EN_SEL_V (0x1 << 4)
#define DINT_PKT_BCCH_NUM_LTU_FRM_V 0x00

/* PKT_PUNCT_HI */
#define DINT_PKT_BCCH_HI_PUNCT_PATTERN_V (0xffff)

/* PKT_PL_CRC_LO */
#define DINT_PKT_BCCH_PL_CRC_V (0xc867)

/* PKT_PL_CRC_HI */
#define DINT_PKT_BCCH_LTU_CRC_V (0x0000)

/*=========================================================================

    DEINTERLEAVER PACKET PARAMETERS FOR CCCH

==========================================================================*/

        /* CCCH Packet Parameters. 20ms frame, R=1/4, Data Rate = 9600 bps */
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_INTLV_COLS_V (0x0c << 8)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_BLOCK_SIZE_V 0x0300
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_PL_CRC_V (0xf130)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_LTU_CRC_V (0x0000)
#define DINT_PKT_CCCH_CODE_1BY4_1X_RATE_NUM_LTU_FRM_V 0x00

        /* CCCH Packet Parameters. 20ms frame, R=1/4, Data Rate = 19200 bps */
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_INTLV_COLS_V (0x18 << 8)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_INTLV_ROWS_V (0x80 << 0)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_BLOCK_SIZE_V 0x0600
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_LTU_CRC_V (0x0000)
#define DINT_PKT_CCCH_CODE_1BY4_2X_RATE_NUM_LTU_FRM_V 0x00

        /* CCCH Packet Parameters. 20ms frame, R=1/4, Data Rate = 38400 bps */
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_INTLV_COLS_V (0x30 << 8)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_BLOCK_SIZE_V 0x0c00
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_LTU_CRC_V (0x0000)
#define DINT_PKT_CCCH_CODE_1BY4_4X_RATE_NUM_LTU_FRM_V 0x00

        /* CCCH Packet Parameters. 20ms frame, R=1/2, Data Rate = 9600 bps */
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_INTLV_COLS_V (0x06 << 8)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_BLOCK_SIZE_V 0x0180
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_PL_CRC_V (0xf130)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_LTU_CRC_V (0x0000)
#define DINT_PKT_CCCH_CODE_1BY2_1X_RATE_NUM_LTU_FRM_V 0x00

        /* CCCH Packet Parameters. 20ms frame, R=1/2, Data Rate = 19200 bps */
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_INTLV_COLS_V (0x0c << 8)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_BLOCK_SIZE_V 0x0300
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_RDCNT_EN_SEL_V (0x01 << 4)
/* #define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_RDCNT_EN_SEL_V (0x00 << 4) */
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_LTU_CRC_V (0x0000)
#define DINT_PKT_CCCH_CODE_1BY2_2X_RATE_NUM_LTU_FRM_V 0x00

        /* CCCH Packet Parameters. 20ms frame, R=1/2, Data Rate = 38400 bps */
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_INTLV_COLS_V (0x18 << 8)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_BLOCK_SIZE_V 0x0600
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_LTU_CRC_V (0x0000)
#define DINT_PKT_CCCH_CODE_1BY2_4X_RATE_NUM_LTU_FRM_V 0x00

#endif /* FEATURE_IS2000_REL_A_CC */


/*========================================================================

       DEINTERLEAVER PACKET PARAMETERS FOR FCH FOR RC1 -RC5

==========================================================================*/

#define DINT_PKT_FCH_RC1_ALL_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_FCH_RC1_ALL_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_FCH_RC1_FULL_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_FCH_RC1_HALF_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_FCH_RC1_QUARTER_RATE_YM_THRESH_V (0x03 << 8)
#define DINT_PKT_FCH_RC1_EIGHTH_RATE_YM_THRESH_V (0x03 << 8)
#define DINT_PKT_FCH_RC1_FULL_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_FCH_RC1_HALF_RATE_SMT_SCALAR_V (0xb5)
#define DINT_PKT_FCH_RC1_QUARTER_RATE_SMT_SCALAR_V (0x80)
#define DINT_PKT_FCH_RC1_EIGHTH_RATE_SMT_SCALAR_V (0x5a)
#define DINT_PKT_FCH_RC1_ALL_RATE_INTLV_COLS_V (0x06 << 8)
#define DINT_PKT_FCH_RC1_ALL_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_FCH_RC1_ALL_RATE_BLOCK_SIZE_V 0x0180
#define DINT_PKT_FCH_RC1_ALL_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_FCH_RC1_FULL_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_FCH_RC1_HALF_RATE_SYMBOL_REPEAT_V (0x01 << 2)
#define DINT_PKT_FCH_RC1_QUARTER_RATE_SYMBOL_REPEAT_V (0x02 << 2)
#define DINT_PKT_FCH_RC1_EIGHTH_RATE_SYMBOL_REPEAT_V (0x03 << 2)
#define DINT_PKT_FCH_RC1_ALL_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_FCH_RC1_ALL_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_FCH_RC1_ALL_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_FCH_RC1_ALL_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_FCH_RC1_FULL_RATE_PL_CRC_V (0xf130)
#define DINT_PKT_FCH_RC1_HALF_RATE_PL_CRC_V (0x9b00)
#define DINT_PKT_FCH_RC1_QUARTER_RATE_PL_CRC_V (0x0000)
#define DINT_PKT_FCH_RC1_EIGHTH_RATE_PL_CRC_V (0x0000)
#define DINT_PKT_FCH_RC1_ALL_RATE_LTU_CRC_V (0x0000)
#define DINT_PKT_FCH_RC1_ALL_RATE_NUM_LTU_FRM_V 0x00


#define DINT_PKT_FCH_RC2_ALL_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_FCH_RC2_ALL_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_FCH_RC2_FULL_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_FCH_RC2_HALF_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_FCH_RC2_QUARTER_RATE_YM_THRESH_V (0x03 << 8)
#define DINT_PKT_FCH_RC2_EIGHTH_RATE_YM_THRESH_V (0x03 << 8)
#define DINT_PKT_FCH_RC2_FULL_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_FCH_RC2_HALF_RATE_SMT_SCALAR_V (0xb5)
#define DINT_PKT_FCH_RC2_QUARTER_RATE_SMT_SCALAR_V (0x80)
#define DINT_PKT_FCH_RC2_EIGHTH_RATE_SMT_SCALAR_V (0x5a)
#define DINT_PKT_FCH_RC2_ALL_RATE_INTLV_COLS_V (0x06 << 8)
#define DINT_PKT_FCH_RC2_ALL_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_FCH_RC2_ALL_RATE_BLOCK_SIZE_V 0x0180
#define DINT_PKT_FCH_RC2_ALL_RATE_PUNCT_LENGTH_V (0x05 << 4)
#define DINT_PKT_FCH_RC2_FULL_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_FCH_RC2_HALF_RATE_SYMBOL_REPEAT_V (0x01 << 2)
#define DINT_PKT_FCH_RC2_QUARTER_RATE_SYMBOL_REPEAT_V (0x02 << 2)
#define DINT_PKT_FCH_RC2_EIGHTH_RATE_SYMBOL_REPEAT_V (0x03 << 2)
#define DINT_PKT_FCH_RC2_ALL_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_FCH_RC2_ALL_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_FCH_RC2_ALL_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_FCH_RC2_ALL_RATE_HI_PUNCT_PATTERN_V (0xd7ff)
#define DINT_PKT_FCH_RC2_FULL_RATE_PL_CRC_V (0xf130)
#define DINT_PKT_FCH_RC2_HALF_RATE_PL_CRC_V (0xf640)
#define DINT_PKT_FCH_RC2_QUARTER_RATE_PL_CRC_V (0x9b00)
#define DINT_PKT_FCH_RC2_EIGHTH_RATE_PL_CRC_V (0x1c00)
#define DINT_PKT_FCH_RC2_ALL_RATE_LTU_CRC_V (0x0000)
#define DINT_PKT_FCH_RC2_ALL_RATE_NUM_LTU_FRM_V 0x00


#define DINT_PKT_FCH_RC3_ALL_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_FCH_RC3_ALL_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_FCH_RC3_ALL_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_FCH_RC3_FULL_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_FCH_RC3_HALF_RATE_SMT_SCALAR_V (0xb5)
#define DINT_PKT_FCH_RC3_QUARTER_RATE_SMT_SCALAR_V (0x80)
#define DINT_PKT_FCH_RC3_EIGHTH_RATE_SMT_SCALAR_V (0x5a)
#define DINT_PKT_FCH_RC3_ALL_RATE_INTLV_COLS_V (0x0c << 8)
#define DINT_PKT_FCH_RC3_ALL_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_FCH_RC3_ALL_RATE_BLOCK_SIZE_V 0x0300
#define DINT_PKT_FCH_RC3_FULL_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_FCH_RC3_HALF_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_FCH_RC3_QUARTER_RATE_PUNCT_LENGTH_V (0x08 << 4)
#define DINT_PKT_FCH_RC3_EIGHTH_RATE_PUNCT_LENGTH_V (0x04 << 4)
#define DINT_PKT_FCH_RC3_FULL_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_FCH_RC3_HALF_RATE_SYMBOL_REPEAT_V (0x01 << 2)
#define DINT_PKT_FCH_RC3_QUARTER_RATE_SYMBOL_REPEAT_V (0x02 << 2)
#define DINT_PKT_FCH_RC3_EIGHTH_RATE_SYMBOL_REPEAT_V (0x03 << 2)
#define DINT_PKT_FCH_RC3_ALL_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_FCH_RC3_ALL_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_FCH_RC3_FULL_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_FCH_RC3_HALF_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_FCH_RC3_QUARTER_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_FCH_RC3_EIGHTH_RATE_RDCNT_EN_SEL_V (0x01 << 4)
#define DINT_PKT_FCH_RC3_FULL_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_FCH_RC3_HALF_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_FCH_RC3_QUARTER_RATE_HI_PUNCT_PATTERN_V (0xff7f)
#define DINT_PKT_FCH_RC3_EIGHTH_RATE_HI_PUNCT_PATTERN_V (0xf7ff)
#define DINT_PKT_FCH_RC3_FULL_RATE_PL_CRC_V (0xf130)
#define DINT_PKT_FCH_RC3_HALF_RATE_PL_CRC_V (0x9b00)
#define DINT_PKT_FCH_RC3_QUARTER_RATE_PL_CRC_V (0x9c00)
#define DINT_PKT_FCH_RC3_EIGHTH_RATE_PL_CRC_V (0x9c00)
#define DINT_PKT_FCH_RC3_ALL_RATE_LTU_CRC_V (0x0000)
#define DINT_PKT_FCH_RC3_ALL_RATE_NUM_LTU_FRM_V 0x00

#define DINT_PKT_FCH_RC4_ALL_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_FCH_RC4_ALL_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_FCH_RC4_ALL_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_FCH_RC4_FULL_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_FCH_RC4_HALF_RATE_SMT_SCALAR_V (0xb5)
#define DINT_PKT_FCH_RC4_QUARTER_RATE_SMT_SCALAR_V (0x80)
#define DINT_PKT_FCH_RC4_EIGHTH_RATE_SMT_SCALAR_V (0x5a)
#define DINT_PKT_FCH_RC4_ALL_RATE_INTLV_COLS_V (0x06 << 8)
#define DINT_PKT_FCH_RC4_ALL_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_FCH_RC4_ALL_RATE_BLOCK_SIZE_V 0x0180
#define DINT_PKT_FCH_RC4_FULL_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_FCH_RC4_HALF_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_FCH_RC4_QUARTER_RATE_PUNCT_LENGTH_V (0x08 << 4)
#define DINT_PKT_FCH_RC4_EIGHTH_RATE_PUNCT_LENGTH_V (0x04 << 4)
#define DINT_PKT_FCH_RC4_FULL_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_FCH_RC4_HALF_RATE_SYMBOL_REPEAT_V (0x01 << 2)
#define DINT_PKT_FCH_RC4_QUARTER_RATE_SYMBOL_REPEAT_V (0x02 << 2)
#define DINT_PKT_FCH_RC4_EIGHTH_RATE_SYMBOL_REPEAT_V (0x03 << 2)
#define DINT_PKT_FCH_RC4_ALL_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_FCH_RC4_ALL_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_FCH_RC4_ALL_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_FCH_RC4_FULL_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_FCH_RC4_HALF_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_FCH_RC4_QUARTER_RATE_HI_PUNCT_PATTERN_V (0xff7f)
#define DINT_PKT_FCH_RC4_EIGHTH_RATE_HI_PUNCT_PATTERN_V (0xf7ff)
#define DINT_PKT_FCH_RC4_FULL_RATE_PL_CRC_V (0xf130)
#define DINT_PKT_FCH_RC4_HALF_RATE_PL_CRC_V (0x9b00)
#define DINT_PKT_FCH_RC4_QUARTER_RATE_PL_CRC_V (0x9c00)
#define DINT_PKT_FCH_RC4_EIGHTH_RATE_PL_CRC_V (0x9c00)
#define DINT_PKT_FCH_RC4_ALL_RATE_LTU_CRC_V (0x0000)
#define DINT_PKT_FCH_RC4_ALL_RATE_NUM_LTU_FRM_V 0x00


#define DINT_PKT_FCH_RC5_ALL_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_FCH_RC5_ALL_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_FCH_RC5_ALL_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_FCH_RC5_FULL_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_FCH_RC5_HALF_RATE_SMT_SCALAR_V (0xb5)
#define DINT_PKT_FCH_RC5_QUARTER_RATE_SMT_SCALAR_V (0x80)
#define DINT_PKT_FCH_RC5_EIGHTH_RATE_SMT_SCALAR_V (0x5a)
#define DINT_PKT_FCH_RC5_ALL_RATE_INTLV_COLS_V (0x0c << 8)
#define DINT_PKT_FCH_RC5_ALL_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_FCH_RC5_ALL_RATE_BLOCK_SIZE_V 0x0300
#define DINT_PKT_FCH_RC5_ALL_RATE_PUNCT_LENGTH_V (0x0b << 4)
#define DINT_PKT_FCH_RC5_FULL_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_FCH_RC5_HALF_RATE_SYMBOL_REPEAT_V (0x01 << 2)
#define DINT_PKT_FCH_RC5_QUARTER_RATE_SYMBOL_REPEAT_V (0x02 << 2)
#define DINT_PKT_FCH_RC5_EIGHTH_RATE_SYMBOL_REPEAT_V (0x03 << 2)
#define DINT_PKT_FCH_RC5_ALL_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_FCH_RC5_ALL_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_FCH_RC5_FULL_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_FCH_RC5_HALF_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_FCH_RC5_QUARTER_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_FCH_RC5_EIGHTH_RATE_RDCNT_EN_SEL_V (0x01 << 4)
#define DINT_PKT_FCH_RC5_ALL_RATE_HI_PUNCT_PATTERN_V (0xd9bf)
#define DINT_PKT_FCH_RC5_FULL_RATE_PL_CRC_V (0xf130)
#define DINT_PKT_FCH_RC5_HALF_RATE_PL_CRC_V (0xf640)
#define DINT_PKT_FCH_RC5_QUARTER_RATE_PL_CRC_V (0x9b00)
#define DINT_PKT_FCH_RC5_EIGHTH_RATE_PL_CRC_V (0x9c00)
#define DINT_PKT_FCH_RC5_ALL_RATE_LTU_CRC_V (0x0000)
#define DINT_PKT_FCH_RC5_ALL_RATE_NUM_LTU_FRM_V 0x00

#define DINT_PKT_SCH_RC3_1X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_SCH_RC3_1X_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_SCH_RC3_1X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC3_1X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC3_1X_RATE_INTLV_COLS_V (0x0c << 8)
#define DINT_PKT_SCH_RC3_1X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_SCH_RC3_1X_RATE_BLOCK_SIZE_V 0x0300
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC3_1X_RATE_LTU_LEN_HI_V (0x00 << 9)
  #define DINT_PKT_SCH_RC3_1X_RATE_LTU_LEN_LO_V (0x00 << 6)
#else
  #define DINT_PKT_SCH_RC3_1X_RATE_LTU_LEN_V (0x00 << 9)
#endif
#define DINT_PKT_SCH_RC3_1X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SCH_RC3_1X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC3_1X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_SCH_RC3_1X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC3_1X_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_SCH_RC3_1X_RATE_NUM_LTU_FRM_V 0x00
#define DINT_PKT_SCH_RC3_1X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SCH_RC3_1X_RATE_PL_CRC_V (0xf130)
#define DINT_PKT_SCH_RC3_1X_RATE_LTU_CRC_V (0x0000)

#define DINT_PKT_SCH_RC3_2X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_SCH_RC3_2X_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_SCH_RC3_2X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC3_2X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC3_2X_RATE_INTLV_COLS_V (0x18 << 8)
#define DINT_PKT_SCH_RC3_2X_RATE_INTLV_ROWS_V (0x80 << 0)
#define DINT_PKT_SCH_RC3_2X_RATE_BLOCK_SIZE_V 0x0600
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC3_2X_RATE_LTU_LEN_HI_V (0x00 << 9)
  #define DINT_PKT_SCH_RC3_2X_RATE_LTU_LEN_LO_V (0x00 << 6)
#else
  #define DINT_PKT_SCH_RC3_2X_RATE_LTU_LEN_V (0x00 << 9)
#endif
#define DINT_PKT_SCH_RC3_2X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SCH_RC3_2X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC3_2X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_SCH_RC3_2X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC3_2X_RATE_RDCNT_EN_SEL_V (0x01 << 4)
#define DINT_PKT_SCH_RC3_2X_RATE_NUM_LTU_FRM_V 0x00
#define DINT_PKT_SCH_RC3_2X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SCH_RC3_2X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC3_2X_RATE_LTU_CRC_V (0xc867)

#define DINT_PKT_SCH_RC3_4X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_SCH_RC3_4X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_SCH_RC3_4X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC3_4X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC3_4X_RATE_INTLV_COLS_V (0x30 << 8)
#define DINT_PKT_SCH_RC3_4X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_SCH_RC3_4X_RATE_BLOCK_SIZE_V 0x0c00
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC3_4X_RATE_LTU_LEN_HI_V (0x0b << 9)
  #define DINT_PKT_SCH_RC3_4X_RATE_LTU_LEN_LO_V (0x02 << 6)
#else
  #define DINT_PKT_SCH_RC3_4X_RATE_LTU_LEN_V (0x2e << 9)
#endif
#define DINT_PKT_SCH_RC3_4X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SCH_RC3_4X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC3_4X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_SCH_RC3_4X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC3_4X_RATE_RDCNT_EN_SEL_V (0x01 << 4)
#define DINT_PKT_SCH_RC3_4X_RATE_NUM_LTU_FRM_V 0x02
#define DINT_PKT_SCH_RC3_4X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SCH_RC3_4X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC3_4X_RATE_LTU_CRC_V (0xc867)

#define DINT_PKT_SCH_RC3_8X_RATE_ROW_WIDTH_V (0x04 << 13)
#define DINT_PKT_SCH_RC3_8X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_SCH_RC3_8X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC3_8X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC3_8X_RATE_INTLV_COLS_V (0x30 << 8)
#define DINT_PKT_SCH_RC3_8X_RATE_INTLV_ROWS_V (0x80 << 0)
#define DINT_PKT_SCH_RC3_8X_RATE_BLOCK_SIZE_V 0x1800
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC3_8X_RATE_LTU_LEN_HI_V (0x0b << 9)
  #define DINT_PKT_SCH_RC3_8X_RATE_LTU_LEN_LO_V (0x02 << 6)
#else
  #define DINT_PKT_SCH_RC3_8X_RATE_LTU_LEN_V (0x2e << 9)
#endif
#define DINT_PKT_SCH_RC3_8X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SCH_RC3_8X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC3_8X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_SCH_RC3_8X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC3_8X_RATE_RDCNT_EN_SEL_V (0x01 << 4)
#define DINT_PKT_SCH_RC3_8X_RATE_NUM_LTU_FRM_V 0x04
#define DINT_PKT_SCH_RC3_8X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SCH_RC3_8X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC3_8X_RATE_LTU_CRC_V (0xc867)

#define DINT_PKT_SCH_RC3_16X_RATE_ROW_WIDTH_V (0x04 << 13)
#define DINT_PKT_SCH_RC3_16X_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_SCH_RC3_16X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC3_16X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC3_16X_RATE_INTLV_COLS_V (0x60 << 8)
#define DINT_PKT_SCH_RC3_16X_RATE_INTLV_ROWS_V (0x80 << 0)
#define DINT_PKT_SCH_RC3_16X_RATE_BLOCK_SIZE_V 0x3000
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC3_16X_RATE_LTU_LEN_HI_V (0x0b << 9)
  #define DINT_PKT_SCH_RC3_16X_RATE_LTU_LEN_LO_V (0x02 << 6)
#else
  #define DINT_PKT_SCH_RC3_16X_RATE_LTU_LEN_V (0x2e << 9)
#endif
#define DINT_PKT_SCH_RC3_16X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SCH_RC3_16X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC3_16X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_SCH_RC3_16X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC3_16X_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_SCH_RC3_16X_RATE_NUM_LTU_FRM_V 0x08
#define DINT_PKT_SCH_RC3_16X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SCH_RC3_16X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC3_16X_RATE_LTU_CRC_V (0xc867)

#define DINT_PKT_SCH_RC4_1X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_SCH_RC4_1X_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_SCH_RC4_1X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC4_1X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC4_1X_RATE_INTLV_COLS_V (0x06 << 8)
#define DINT_PKT_SCH_RC4_1X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_SCH_RC4_1X_RATE_BLOCK_SIZE_V 0x0180
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC4_1X_RATE_LTU_LEN_HI_V (0x00 << 9)
  #define DINT_PKT_SCH_RC4_1X_RATE_LTU_LEN_LO_V (0x00 << 6)
#else
  #define DINT_PKT_SCH_RC4_1X_RATE_LTU_LEN_V (0x00 << 9)
#endif
#define DINT_PKT_SCH_RC4_1X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SCH_RC4_1X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC4_1X_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_SCH_RC4_1X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC4_1X_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_SCH_RC4_1X_RATE_NUM_LTU_FRM_V 0x00
#define DINT_PKT_SCH_RC4_1X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SCH_RC4_1X_RATE_PL_CRC_V (0xf130)
#define DINT_PKT_SCH_RC4_1X_RATE_LTU_CRC_V (0x0000)

#define DINT_PKT_SCH_RC4_2X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_SCH_RC4_2X_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_SCH_RC4_2X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC4_2X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC4_2X_RATE_INTLV_COLS_V (0x0c << 8)
#define DINT_PKT_SCH_RC4_2X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_SCH_RC4_2X_RATE_BLOCK_SIZE_V 0x0300
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC4_2X_RATE_LTU_LEN_HI_V (0x00 << 9)
  #define DINT_PKT_SCH_RC4_2X_RATE_LTU_LEN_LO_V (0x00 << 6)
#else
  #define DINT_PKT_SCH_RC4_2X_RATE_LTU_LEN_V (0x00 << 9)
#endif
#define DINT_PKT_SCH_RC4_2X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SCH_RC4_2X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC4_2X_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_SCH_RC4_2X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC4_2X_RATE_RDCNT_EN_SEL_V (0x01 << 4)
#define DINT_PKT_SCH_RC4_2X_RATE_NUM_LTU_FRM_V 0x00
#define DINT_PKT_SCH_RC4_2X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SCH_RC4_2X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC4_2X_RATE_LTU_CRC_V (0xc867)

#define DINT_PKT_SCH_RC4_4X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_SCH_RC4_4X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_SCH_RC4_4X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC4_4X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC4_4X_RATE_INTLV_COLS_V (0x18 << 8)
#define DINT_PKT_SCH_RC4_4X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_SCH_RC4_4X_RATE_BLOCK_SIZE_V 0x0600
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC4_4X_RATE_LTU_LEN_HI_V (0x0b << 9)
  #define DINT_PKT_SCH_RC4_4X_RATE_LTU_LEN_LO_V (0x02 << 6)
#else
  #define DINT_PKT_SCH_RC4_4X_RATE_LTU_LEN_V (0x2e << 9)
#endif
#define DINT_PKT_SCH_RC4_4X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SCH_RC4_4X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC4_4X_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_SCH_RC4_4X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC4_4X_RATE_RDCNT_EN_SEL_V (0x01 << 4)
#define DINT_PKT_SCH_RC4_4X_RATE_NUM_LTU_FRM_V 0x02
#define DINT_PKT_SCH_RC4_4X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SCH_RC4_4X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC4_4X_RATE_LTU_CRC_V (0xc867)

#define DINT_PKT_SCH_RC4_8X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_SCH_RC4_8X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_SCH_RC4_8X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC4_8X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC4_8X_RATE_INTLV_COLS_V (0x30 << 8)
#define DINT_PKT_SCH_RC4_8X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_SCH_RC4_8X_RATE_BLOCK_SIZE_V 0x0c00
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC4_8X_RATE_LTU_LEN_HI_V (0x0b << 9)
  #define DINT_PKT_SCH_RC4_8X_RATE_LTU_LEN_LO_V (0x02 << 6)
#else
  #define DINT_PKT_SCH_RC4_8X_RATE_LTU_LEN_V (0x2e << 9)
#endif
#define DINT_PKT_SCH_RC4_8X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SCH_RC4_8X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC4_8X_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_SCH_RC4_8X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC4_8X_RATE_RDCNT_EN_SEL_V (0x01 << 4)
#define DINT_PKT_SCH_RC4_8X_RATE_NUM_LTU_FRM_V 0x04
#define DINT_PKT_SCH_RC4_8X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SCH_RC4_8X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC4_8X_RATE_LTU_CRC_V (0xc867)

#define DINT_PKT_SCH_RC4_16X_RATE_ROW_WIDTH_V (0x04 << 13)
#define DINT_PKT_SCH_RC4_16X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_SCH_RC4_16X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC4_16X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC4_16X_RATE_INTLV_COLS_V (0x30 << 8)
#define DINT_PKT_SCH_RC4_16X_RATE_INTLV_ROWS_V (0x80 << 0)
#define DINT_PKT_SCH_RC4_16X_RATE_BLOCK_SIZE_V 0x1800
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC4_16X_RATE_LTU_LEN_HI_V (0x0b << 9)
  #define DINT_PKT_SCH_RC4_16X_RATE_LTU_LEN_LO_V (0x02 << 6)
#else
  #define DINT_PKT_SCH_RC4_16X_RATE_LTU_LEN_V (0x2e << 9)
#endif
#define DINT_PKT_SCH_RC4_16X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SCH_RC4_16X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC4_16X_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_SCH_RC4_16X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC4_16X_RATE_RDCNT_EN_SEL_V (0x01 << 4)
#define DINT_PKT_SCH_RC4_16X_RATE_NUM_LTU_FRM_V 0x08
#define DINT_PKT_SCH_RC4_16X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SCH_RC4_16X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC4_16X_RATE_LTU_CRC_V (0xc867)

#ifdef FEATURE_32X_SCH_DATA_RATE
#define DINT_PKT_SCH_RC4_32X_RATE_ROW_WIDTH_V (0x04 << 13)
#define DINT_PKT_SCH_RC4_32X_RATE_CHAINBACK_DEPTH_V (0x00 << 12)
#define DINT_PKT_SCH_RC4_32X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC4_32X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC4_32X_RATE_INTLV_COLS_V (0x60 << 8) 
#define DINT_PKT_SCH_RC4_32X_RATE_INTLV_ROWS_V (0x80 << 0) 
#define DINT_PKT_SCH_RC4_32X_RATE_BLOCK_SIZE_V 0x3000
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC4_32X_RATE_LTU_LEN_HI_V (0x00 << 9)
  #define DINT_PKT_SCH_RC4_32X_RATE_LTU_LEN_LO_V (0x00 << 6)
#else
  #define DINT_PKT_SCH_RC4_32X_RATE_LTU_LEN_V (0x0 << 9)   
#endif
#define DINT_PKT_SCH_RC4_32X_RATE_PUNCT_LENGTH_V (0x00 << 4)
#define DINT_PKT_SCH_RC4_32X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC4_32X_RATE_CODE_RATE_V (0x00)
#define DINT_PKT_SCH_RC4_32X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC4_32X_RATE_RDCNT_EN_SEL_V (0x0 << 4)
#define DINT_PKT_SCH_RC4_32X_RATE_NUM_LTU_FRM_V 0x0
#define DINT_PKT_SCH_RC4_32X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_PKT_SCH_RC4_32X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC4_32X_RATE_LTU_CRC_V (0xc867)
#endif /* FEATURE_32X_SCH_DATA_RATE */ 

#define DINT_PKT_SCH_RC5_1X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_SCH_RC5_1X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_SCH_RC5_1X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC5_1X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC5_1X_RATE_INTLV_COLS_V (0x0c << 8)
#define DINT_PKT_SCH_RC5_1X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_SCH_RC5_1X_RATE_BLOCK_SIZE_V 0x0300
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC5_1X_RATE_LTU_LEN_HI_V (0x00 << 9)
  #define DINT_PKT_SCH_RC5_1X_RATE_LTU_LEN_LO_V (0x00 << 6)
#else
  #define DINT_PKT_SCH_RC5_1X_RATE_LTU_LEN_V (0x00 << 9) 
#endif
#define DINT_PKT_SCH_RC5_1X_RATE_PUNCT_LENGTH_V (0x0B << 4)
#define DINT_PKT_SCH_RC5_1X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC5_1X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_SCH_RC5_1X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC5_1X_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_SCH_RC5_1X_RATE_NUM_LTU_FRM_V 0x00 
#define DINT_PKT_SCH_RC5_1X_RATE_HI_PUNCT_PATTERN_V (0xd9bf)
#define DINT_PKT_SCH_RC5_1X_RATE_PL_CRC_V (0xf130)
#define DINT_PKT_SCH_RC5_1X_RATE_LTU_CRC_V (0x0000)

#define DINT_PKT_SCH_RC5_2X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_SCH_RC5_2X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_SCH_RC5_2X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC5_2X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC5_2X_RATE_INTLV_COLS_V (0x18 << 8)
#define DINT_PKT_SCH_RC5_2X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_SCH_RC5_2X_RATE_BLOCK_SIZE_V 0x0600
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC5_2X_RATE_LTU_LEN_HI_V (0x00 << 9)
  #define DINT_PKT_SCH_RC5_2X_RATE_LTU_LEN_LO_V (0x00 << 6)
#else
  #define DINT_PKT_SCH_RC5_2X_RATE_LTU_LEN_V (0x00 << 9) 
#endif
#define DINT_PKT_SCH_RC5_2X_RATE_PUNCT_LENGTH_V (0x0B << 4)
#define DINT_PKT_SCH_RC5_2X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC5_2X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_SCH_RC5_2X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC5_2X_RATE_RDCNT_EN_SEL_V (0x01 << 4)
#define DINT_PKT_SCH_RC5_2X_RATE_NUM_LTU_FRM_V 0x00 
#define DINT_PKT_SCH_RC5_2X_RATE_HI_PUNCT_PATTERN_V (0xd9bf)
#define DINT_PKT_SCH_RC5_2X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC5_2X_RATE_LTU_CRC_V (0xc867)

#define DINT_PKT_SCH_RC5_4X_RATE_ROW_WIDTH_V (0x03 << 13)
#define DINT_PKT_SCH_RC5_4X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_SCH_RC5_4X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC5_4X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC5_4X_RATE_INTLV_COLS_V (0x30 << 8)
#define DINT_PKT_SCH_RC5_4X_RATE_INTLV_ROWS_V (0x40 << 0)
#define DINT_PKT_SCH_RC5_4X_RATE_BLOCK_SIZE_V 0x0c00
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC5_4X_RATE_LTU_LEN_HI_V (0x11 << 9)
  #define DINT_PKT_SCH_RC5_4X_RATE_LTU_LEN_LO_V (0x02 << 6)
#else
  #define DINT_PKT_SCH_RC5_4X_RATE_LTU_LEN_V (0x46 << 9) 
#endif
#define DINT_PKT_SCH_RC5_4X_RATE_PUNCT_LENGTH_V (0x0B << 4)
#define DINT_PKT_SCH_RC5_4X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC5_4X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_SCH_RC5_4X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC5_4X_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_SCH_RC5_4X_RATE_NUM_LTU_FRM_V 0x02 
#define DINT_PKT_SCH_RC5_4X_RATE_HI_PUNCT_PATTERN_V (0xd9bf)
#define DINT_PKT_SCH_RC5_4X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC5_4X_RATE_LTU_CRC_V (0xc867)

#define DINT_PKT_SCH_RC5_8X_RATE_ROW_WIDTH_V (0x04 << 13)
#define DINT_PKT_SCH_RC5_8X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_SCH_RC5_8X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC5_8X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC5_8X_RATE_INTLV_COLS_V (0x30 << 8)
#define DINT_PKT_SCH_RC5_8X_RATE_INTLV_ROWS_V (0x80 << 0)
#define DINT_PKT_SCH_RC5_8X_RATE_BLOCK_SIZE_V 0x1800
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC5_8X_RATE_LTU_LEN_HI_V (0x11 << 9)
  #define DINT_PKT_SCH_RC5_8X_RATE_LTU_LEN_LO_V (0x02 << 6)
#else
  #define DINT_PKT_SCH_RC5_8X_RATE_LTU_LEN_V (0x46 << 9)
#endif
#define DINT_PKT_SCH_RC5_8X_RATE_PUNCT_LENGTH_V (0x0B << 4)
#define DINT_PKT_SCH_RC5_8X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC5_8X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_SCH_RC5_8X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC5_8X_RATE_RDCNT_EN_SEL_V (0x01 << 4)
#define DINT_PKT_SCH_RC5_8X_RATE_NUM_LTU_FRM_V 0x04 
#define DINT_PKT_SCH_RC5_8X_RATE_HI_PUNCT_PATTERN_V (0xd9bf)
#define DINT_PKT_SCH_RC5_8X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC5_8X_RATE_LTU_CRC_V (0xc867)

#define DINT_PKT_SCH_RC5_16X_RATE_ROW_WIDTH_V (0x04 << 13)
#define DINT_PKT_SCH_RC5_16X_RATE_CHAINBACK_DEPTH_V (0x01 << 12)
#define DINT_PKT_SCH_RC5_16X_RATE_YM_THRESH_V (0x01 << 8)
#define DINT_PKT_SCH_RC5_16X_RATE_SMT_SCALAR_V (0xff)
#define DINT_PKT_SCH_RC5_16X_RATE_INTLV_COLS_V (0x60 << 8)
#define DINT_PKT_SCH_RC5_16X_RATE_INTLV_ROWS_V (0x80 << 0)
#define DINT_PKT_SCH_RC5_16X_RATE_BLOCK_SIZE_V 0x3000
#ifdef MSMHW_EXTENDED_LTU_LEN
  #define DINT_PKT_SCH_RC5_16X_RATE_LTU_LEN_HI_V (0x11 << 9)
  #define DINT_PKT_SCH_RC5_16X_RATE_LTU_LEN_LO_V (0x02 << 6)
#else
  #define DINT_PKT_SCH_RC5_16X_RATE_LTU_LEN_V (0x46 << 9)
#endif
#define DINT_PKT_SCH_RC5_16X_RATE_PUNCT_LENGTH_V (0x0B << 4)
#define DINT_PKT_SCH_RC5_16X_RATE_SYMBOL_REPEAT_V (0x00 << 2)
#define DINT_PKT_SCH_RC5_16X_RATE_CODE_RATE_V (0x02)
#define DINT_PKT_SCH_RC5_16X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_PKT_SCH_RC5_16X_RATE_RDCNT_EN_SEL_V (0x00 << 4)
#define DINT_PKT_SCH_RC5_16X_RATE_NUM_LTU_FRM_V 0x08
#define DINT_PKT_SCH_RC5_16X_RATE_HI_PUNCT_PATTERN_V (0xd9bf)
#define DINT_PKT_SCH_RC5_16X_RATE_PL_CRC_V (0xc867)
#define DINT_PKT_SCH_RC5_16X_RATE_LTU_CRC_V (0xc867)

#ifdef FEATURE_IS2000_REL_A //32X
    #define DINT_LTU_SIZE_IN_BYTE_M  (0x7f << 9)
  #ifdef MSMHW_EXTENDED_LTU_LEN
    #define DINT_LTU_SIZE_MAX (0x7f)
    #define DINT_NUM_LTU_MAX (0x0f)
    #define DINT_LTU_SIZE_IN_BYTE_LO_M  (0x07 << 5)
  #else
    #define DINT_LTU_SIZE_IN_BITS_M  (0x07 << 5)
  #endif /* MSMHW_EXTENDED_LTU_LEN */
    #define DINT_LTU_PER_FRAME_M     (0xf)
#endif /* FEATURE_IS2000_REL_A */
    

#ifdef MSMHW_TURBO_CODING

/*==================================================================
   TURBO DECODER  PARAMTER CONSTANT
===================================================================*/

#define DINT_TD_EARLY_TERM_EN (0x01 << 15)
/* Enables Early Termination Checking for the Turbo Decoder */
#define DINT_TD_EARLY_TERM_DIS (0x01 << 15)
/* Disables Early Termination Checking for the Turbo Decoder */
#define DINT_TD_MAX_ITER_NUM (0x09 << 10)
/* Maximum number of iterations for all rates = 10 */
#define DINT_TD_MIN_ITER_NUM (0x00 << 4)
/* Minimum of 1 iteration for all rates */
#define DINT_TD_MIN_NUM_CRC_PASS (0x01 << 10)
/* Minimum number of consective CRC passes = 2*/
#define DINT_TD_DEC_INT_EN (0x01 << 2)
/* Enable Turbo Decoder Interrupt after the frame is decoded */
#define DINT_TD_DEC_INT_DIS (0x00 << 2)
/* Do Not Enable Turbo Decoder Interrupt after the frame is decoded */

#define DINT_TD_HYP_X1_ON (0x1 << HWIO_SHFT(TD_INTLV_SIZE_LO,TD_HYP_X1_ON))
/* Enable X1 hypothesis for Turbo decoding during variable rate operation */

#define DINT_TD_SCH_RC3_2X_RATE_BLOCK_SIZE_V (0x5ff)
#define DINT_TD_SCH_RC3_2X_RATE_ROW_WIDTH_V  (0x03 << 13)
#define DINT_TD_SCH_RC3_2X_RATE_INTLV_COLS_V (0x17 << 8)
#define DINT_TD_SCH_RC3_2X_RATE_INTLV_ROWS_V (0x3f)
#define DINT_TD_SCH_RC3_2X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC3_2X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC3_2X_RATE_PUNCT_LENGTH_V (0x17 << 3)
#define DINT_TD_SCH_RC3_2X_RATE_CODE_RATE_V (0x02)
#define DINT_TD_SCH_RC3_2X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_TD_SCH_RC3_2X_RATE_TD_INTLV_LEN_V (0x179)
#define DINT_TD_SCH_RC3_2X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC3_2X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC3_2X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC3_2X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC3_2X_RATE_NUM_WINDOWS_V (0x0a)


#define DINT_TD_SCH_RC3_4X_RATE_BLOCK_SIZE_V (0xbff)
#define DINT_TD_SCH_RC3_4X_RATE_ROW_WIDTH_V  (0x03 << 13)
#define DINT_TD_SCH_RC3_4X_RATE_INTLV_COLS_V (0x2f << 8)
#define DINT_TD_SCH_RC3_4X_RATE_INTLV_ROWS_V (0x3f)
#define DINT_TD_SCH_RC3_4X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC3_4X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC3_4X_RATE_PUNCT_LENGTH_V (0x17 << 3)
#define DINT_TD_SCH_RC3_4X_RATE_CODE_RATE_V (0x02)
#define DINT_TD_SCH_RC3_4X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_TD_SCH_RC3_4X_RATE_TD_INTLV_LEN_V (0x2f9)
#define DINT_TD_SCH_RC3_4X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC3_4X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC3_4X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC3_4X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC3_4X_RATE_NUM_WINDOWS_V (0x16)


#define DINT_TD_SCH_RC3_8X_RATE_BLOCK_SIZE_V (0x17ff)
#define DINT_TD_SCH_RC3_8X_RATE_ROW_WIDTH_V  (0x04 << 13)
#define DINT_TD_SCH_RC3_8X_RATE_INTLV_COLS_V (0x2f << 8)
#define DINT_TD_SCH_RC3_8X_RATE_INTLV_ROWS_V (0x7f)
#define DINT_TD_SCH_RC3_8X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC3_8X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC3_8X_RATE_PUNCT_LENGTH_V (0x17 << 3)
#define DINT_TD_SCH_RC3_8X_RATE_CODE_RATE_V (0x02)
#define DINT_TD_SCH_RC3_8X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_TD_SCH_RC3_8X_RATE_TD_INTLV_LEN_V (0x5f9)
#define DINT_TD_SCH_RC3_8X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC3_8X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC3_8X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC3_8X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC3_8X_RATE_NUM_WINDOWS_V (0x2e)


#define DINT_TD_SCH_RC3_16X_RATE_BLOCK_SIZE_V (0x2fff)
#define DINT_TD_SCH_RC3_16X_RATE_ROW_WIDTH_V  (0x04 << 13)
#define DINT_TD_SCH_RC3_16X_RATE_INTLV_COLS_V (0x5f << 8)
#define DINT_TD_SCH_RC3_16X_RATE_INTLV_ROWS_V (0x7f)
#define DINT_TD_SCH_RC3_16X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC3_16X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC3_16X_RATE_PUNCT_LENGTH_V (0x17 << 3)
#define DINT_TD_SCH_RC3_16X_RATE_CODE_RATE_V (0x02)
#define DINT_TD_SCH_RC3_16X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_TD_SCH_RC3_16X_RATE_TD_INTLV_LEN_V (0xbf9)
#define DINT_TD_SCH_RC3_16X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC3_16X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC3_16X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC3_16X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC3_16X_RATE_NUM_WINDOWS_V (0x5e)



#define DINT_TD_SCH_RC4_2X_RATE_BLOCK_SIZE_V (0x2ff)
#define DINT_TD_SCH_RC4_2X_RATE_ROW_WIDTH_V  (0x03 << 13)
#define DINT_TD_SCH_RC4_2X_RATE_INTLV_COLS_V (0x0b << 8)
#define DINT_TD_SCH_RC4_2X_RATE_INTLV_ROWS_V (0x3f)
#define DINT_TD_SCH_RC4_2X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC4_2X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC4_2X_RATE_PUNCT_LENGTH_V (0x17 << 3)
#define DINT_TD_SCH_RC4_2X_RATE_CODE_RATE_V (0x00)
#define DINT_TD_SCH_RC4_2X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_TD_SCH_RC4_2X_RATE_TD_INTLV_LEN_V (0x179)
#define DINT_TD_SCH_RC4_2X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC4_2X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC4_2X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC4_2X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC4_2X_RATE_NUM_WINDOWS_V (0x0a)


#define DINT_TD_SCH_RC4_4X_RATE_BLOCK_SIZE_V (0x5ff)
#define DINT_TD_SCH_RC4_4X_RATE_ROW_WIDTH_V  (0x03 << 13)
#define DINT_TD_SCH_RC4_4X_RATE_INTLV_COLS_V (0x17 << 8)
#define DINT_TD_SCH_RC4_4X_RATE_INTLV_ROWS_V (0x3f)
#define DINT_TD_SCH_RC4_4X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC4_4X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC4_4X_RATE_PUNCT_LENGTH_V (0x17 << 3)
#define DINT_TD_SCH_RC4_4X_RATE_CODE_RATE_V (0x00)
#define DINT_TD_SCH_RC4_4X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_TD_SCH_RC4_4X_RATE_TD_INTLV_LEN_V (0x2f9)
#define DINT_TD_SCH_RC4_4X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC4_4X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC4_4X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC4_4X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC4_4X_RATE_NUM_WINDOWS_V (0x16)


#define DINT_TD_SCH_RC4_8X_RATE_BLOCK_SIZE_V (0xbff)
#define DINT_TD_SCH_RC4_8X_RATE_ROW_WIDTH_V  (0x03 << 13)
#define DINT_TD_SCH_RC4_8X_RATE_INTLV_COLS_V (0x2f << 8)
#define DINT_TD_SCH_RC4_8X_RATE_INTLV_ROWS_V (0x3f)
#define DINT_TD_SCH_RC4_8X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC4_8X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC4_8X_RATE_PUNCT_LENGTH_V (0x17 << 3)
#define DINT_TD_SCH_RC4_8X_RATE_CODE_RATE_V (0x00)
#define DINT_TD_SCH_RC4_8X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_TD_SCH_RC4_8X_RATE_TD_INTLV_LEN_V (0x5f9)
#define DINT_TD_SCH_RC4_8X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC4_8X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC4_8X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC4_8X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC4_8X_RATE_NUM_WINDOWS_V (0x2e)


#define DINT_TD_SCH_RC4_16X_RATE_BLOCK_SIZE_V (0x17ff)
#define DINT_TD_SCH_RC4_16X_RATE_ROW_WIDTH_V  (0x04 << 13)
#define DINT_TD_SCH_RC4_16X_RATE_INTLV_COLS_V (0x2f << 8)
#define DINT_TD_SCH_RC4_16X_RATE_INTLV_ROWS_V (0x7f)
#define DINT_TD_SCH_RC4_16X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC4_16X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC4_16X_RATE_PUNCT_LENGTH_V (0x17 << 3)
#define DINT_TD_SCH_RC4_16X_RATE_CODE_RATE_V (0x00)
#define DINT_TD_SCH_RC4_16X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_TD_SCH_RC4_16X_RATE_TD_INTLV_LEN_V (0xbf9)
#define DINT_TD_SCH_RC4_16X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC4_16X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC4_16X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC4_16X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC4_16X_RATE_NUM_WINDOWS_V (0x5e)


#ifdef FEATURE_32X_SCH_DATA_RATE
#define DINT_TD_SCH_RC4_32X_RATE_BLOCK_SIZE_V (0x2fff)
#define DINT_TD_SCH_RC4_32X_RATE_ROW_WIDTH_V  (0x04 << 13)
#define DINT_TD_SCH_RC4_32X_RATE_INTLV_COLS_V (0x5f << 8)
#define DINT_TD_SCH_RC4_32X_RATE_INTLV_ROWS_V (0x7f)
#define DINT_TD_SCH_RC4_32X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC4_32X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC4_32X_RATE_PUNCT_LENGTH_V (0x17 << 3)
#define DINT_TD_SCH_RC4_32X_RATE_CODE_RATE_V (0x00)
#define DINT_TD_SCH_RC4_32X_RATE_HI_PUNCT_PATTERN_V (0xffff)
#define DINT_TD_SCH_RC4_32X_RATE_TD_INTLV_LEN_V (0x17f9)
#define DINT_TD_SCH_RC4_32X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC4_32X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC4_32X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC4_32X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC4_32X_RATE_NUM_WINDOWS_V (0xbe)
#endif /* FEATURE_32X_SCH_DATA_RATE */


#define DINT_TD_SCH_RC5_2X_RATE_BLOCK_SIZE_V (0x5ff)
#define DINT_TD_SCH_RC5_2X_RATE_ROW_WIDTH_V  (0x03 << 13)
#define DINT_TD_SCH_RC5_2X_RATE_INTLV_COLS_V (0x17 << 8)
#define DINT_TD_SCH_RC5_2X_RATE_INTLV_ROWS_V (0x3f)
#define DINT_TD_SCH_RC5_2X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC5_2X_RATE_LO_PUNCT_PATTERN_V (0x00 << 8)
#define DINT_TD_SCH_RC5_2X_RATE_PUNCT_LENGTH_V (0x0b << 3)
#define DINT_TD_SCH_RC5_2X_RATE_CODE_RATE_V (0x02)
#define DINT_TD_SCH_RC5_2X_RATE_HI_PUNCT_PATTERN_V (0xdda0)
#define DINT_TD_SCH_RC5_2X_RATE_TD_INTLV_LEN_V (0x239)
#define DINT_TD_SCH_RC5_2X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC5_2X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC5_2X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC5_2X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC5_2X_RATE_NUM_WINDOWS_V (0x10)


#define DINT_TD_SCH_RC5_4X_RATE_BLOCK_SIZE_V (0xbff)
#define DINT_TD_SCH_RC5_4X_RATE_ROW_WIDTH_V  (0x03 << 13)
#define DINT_TD_SCH_RC5_4X_RATE_INTLV_COLS_V (0x2f << 8)
#define DINT_TD_SCH_RC5_4X_RATE_INTLV_ROWS_V (0x3f)
#define DINT_TD_SCH_RC5_4X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC5_4X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC5_4X_RATE_PUNCT_LENGTH_V (0x0b << 3)
#define DINT_TD_SCH_RC5_4X_RATE_CODE_RATE_V (0x02)
#define DINT_TD_SCH_RC5_4X_RATE_HI_PUNCT_PATTERN_V (0xddaf)
#define DINT_TD_SCH_RC5_4X_RATE_TD_INTLV_LEN_V (0x479)
#define DINT_TD_SCH_RC5_4X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC5_4X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC5_4X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC5_4X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC5_4X_RATE_NUM_WINDOWS_V (0x22)


#define DINT_TD_SCH_RC5_8X_RATE_BLOCK_SIZE_V (0x17ff)
#define DINT_TD_SCH_RC5_8X_RATE_ROW_WIDTH_V  (0x04 << 13)
#define DINT_TD_SCH_RC5_8X_RATE_INTLV_COLS_V (0x2f << 8)
#define DINT_TD_SCH_RC5_8X_RATE_INTLV_ROWS_V (0x7f)
#define DINT_TD_SCH_RC5_8X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC5_8X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC5_8X_RATE_PUNCT_LENGTH_V (0x0b << 3)
#define DINT_TD_SCH_RC5_8X_RATE_CODE_RATE_V (0x02)
#define DINT_TD_SCH_RC5_8X_RATE_HI_PUNCT_PATTERN_V (0xddaf)
#define DINT_TD_SCH_RC5_8X_RATE_TD_INTLV_LEN_V (0x8f9)
#define DINT_TD_SCH_RC5_8X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC5_8X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC5_8X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC5_8X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC5_8X_RATE_NUM_WINDOWS_V (0x46)


#define DINT_TD_SCH_RC5_16X_RATE_BLOCK_SIZE_V (0x2fff)
#define DINT_TD_SCH_RC5_16X_RATE_ROW_WIDTH_V  (0x04 << 13)
#define DINT_TD_SCH_RC5_16X_RATE_INTLV_COLS_V (0x5f << 8)
#define DINT_TD_SCH_RC5_16X_RATE_INTLV_ROWS_V (0x7f)
#define DINT_TD_SCH_RC5_16X_RATE_MIN_LLR_V    (0x00)
#define DINT_TD_SCH_RC5_16X_RATE_LO_PUNCT_PATTERN_V (0xff << 8)
#define DINT_TD_SCH_RC5_16X_RATE_PUNCT_LENGTH_V (0x0b << 3)
#define DINT_TD_SCH_RC5_16X_RATE_CODE_RATE_V (0x02)
#define DINT_TD_SCH_RC5_16X_RATE_HI_PUNCT_PATTERN_V (0xddaf)
#define DINT_TD_SCH_RC5_16X_RATE_TD_INTLV_LEN_V (0x11f9)
#define DINT_TD_SCH_RC5_16X_RATE__FRAME_20M_RATE_HYP_V (0x00 << 16)
#define DINT_TD_SCH_RC5_16X_RATE__SLIDE_WIN_LEN_V (0x1f << 10)
#define DINT_TD_SCH_RC5_16X_RATE__PARTIAL_WIN_LEN_V (0x1c << 4)
#define DINT_TD_SCH_RC5_16X_RATE_BETA_V (0x04)
#define DINT_TD_SCH_RC5_16X_RATE_NUM_WINDOWS_V (0x8e)

#endif /* MSMHW_TURBO_CODING */

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)  
#define DEINT_MAX_SEQUENCING_CMD  3   /* FCH + DCCH + SCH */
#else
#define DEINT_MAX_SEQUENCING_CMD  2   /* FCH + SCH or FCH + SCCH */
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

/*=======================================================================

   Data Structures for Deinterleaver/Decoder Sequencing Commands to
   Program Read Parameters.

========================================================================*/
static q_type deint_sequencing_cmd_q;
static q_type deint_sequencing_free_q;
static deint_sequencing_cmd_type 
  deint_sequencing_cmd_buf[DEINT_MAX_SEQUENCING_CMD];

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/* Avoid resetting decoder while in traffic state.
*/
static deint_phone_state_type curr_deint_state;

/* Indicate activity of fundicated channels FCH and DCCH.
*/
static deint_fundicated_ch_active_type curr_deint_ch_active = DEINT_CH_INACTIVE;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_REL_A
/* Changes to Turbo SCH set up, including the new struct sch_setup, were 
** made to avoid the first Turbo SCH ISR which will occur 5 ms after 
** Turbo Dec parameters are loaded resulting in reading previous SCH frame 
** data.  This issue was discovered during Turbo finite burst testing.
**
** The fix is to move the creation of the Turbo SCH dummy task to 
** the fourth bin of the same frame as the action time which the Turbo Dec 
** parameters is loaded.  This move guarantees that the first dummy 
** interrupt will not happen in the frame which the ISR is set up 
** (1 frame too soon).
**
** Since finite bursts may involve conversion between Turbo and Conv
** coding, the setting of the coding type is also moved.  This move
** ensures smooth transitioning between all types of finite burst 
** assignments (bursts with gaps, back to back, overlapping) which may 
** involve coding conversion as well.
*/
typedef struct
{
  boolean this_frame;
  boolean turbo_dec_int;
  boolean sup_dec_int;

} deint_sch_setup_type;

static deint_sch_setup_type sch_setup;

#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

  DEINTERLEAVER PACKET CONFIGURATION PARAMETERS FOR SYNC CHANNEL

============================================================================*/

static const deint_sc_pkt_cfg_type deint_sc_pkt_params = {

     DINT_PKT_SC_ROW_WIDTH_V | DINT_PKT_SC_CHAINBACK_DEPTH_V | 
     DINT_PKT_SC_YM_THRESH_V | DINT_PKT_SC_SMT_SCALAR_V,
     
     DINT_PKT_SC_INTLV_COLS_V | DINT_PKT_SC_INTLV_ROWS_V, 
     DINT_PKT_SC_BLOCK_SIZE_V, 
     
     DINT_PKT_SC_PUNCT_LENGTH_V |
     DINT_PKT_SC_SYMBOL_REPEAT_V | DINT_PKT_SC_CODE_RATE_V, 
     
     DINT_PKT_SC_LO_PUNCT_PATTERN_V | DINT_PKT_SC_RDCNT_EN_SEL_V |
     DINT_PKT_SC_NUM_LTU_FRM_V, 
     
     DINT_PKT_SC_HI_PUNCT_PATTERN_V, 
     
     DINT_PKT_SC_PL_CRC_V, 
     
     DINT_PKT_SC_LTU_CRC_V
  
};

/*===========================================================================

  DEINTERLEAVER PACKET CONFIGURATION PARAMETERS FOR IS95 PAGING CHANNEL

============================================================================*/

static const deint_pc_pkt_cfg_type deint_pc_pkt_params = {
{
  /* PRAT_9600 */
  //TODO: Replace magic numbers with constants
  {
    0x61ff,
    0x0640,
    0x0180,
    0x0000,
    0xff00,
    0xffff,
    0x0000,
    0x0000
  },
  
  /* PRAT_4800 */
  {
    0x61b5,
    0x0640,
    0x0180,
    0x0004,
    0xff00,
    0xffff,
    0x0000,
    0x0000 
  }
}
};

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

  DEINTERLEAVER PACKET CONFIGURATION PARAMETERS FOR BCCH

============================================================================*/

static const deint_bcch_pkt_cfg_type deint_bcch_pkt_params = {
{
  {
    /* PKT_INTLV_CFG_HI */
    DINT_PKT_BCCH_ROW_WIDTH_V | DINT_PKT_BCCH_CHAINBACK_DEPTH_V |
    DINT_PKT_BCCH_YM_THRESH_V | DINT_PKT_BCCH_SMT_SCALAR_V,
  
    /* PKT_INTLV_CFG_LO */
    DINT_PKT_BCCH_INTLV_COLS_V*2 | /* Twice that for code rate 1/2 */
    DINT_PKT_BCCH_INTLV_ROWS_V,
  
    /* PKT_BLK_SIZE_LO */
    DINT_PKT_BCCH_BLOCK_SIZE_V*2, /* Twice that for code rate 1/2 */
  
    /* PKT_BLK_SIZE_HI */
    DINT_PKT_BCCH_PUNCT_LENGTH_V | DINT_PKT_BCCH_SYMBOL_REPEAT_V |
    DINT_PKT_BCCH_CODE_RATE_QTR_V,
  
    /* PKT_PUNCT_LO */
    DINT_PKT_BCCH_LO_PUNCT_PATTERN_V | DINT_PKT_BCCH_RDCNT_EN_SEL_V |
    DINT_PKT_BCCH_NUM_LTU_FRM_V,
  
    /* PKT_PUNCT_HI */
    DINT_PKT_BCCH_HI_PUNCT_PATTERN_V,
  
    /* PKT_PL_CRC_LO */
    DINT_PKT_BCCH_PL_CRC_V,
  
    /* PKT_PL_CRC_HI */
    DINT_PKT_BCCH_LTU_CRC_V
  },
  {
    /* PKT_INTLV_CFG_HI */
    DINT_PKT_BCCH_ROW_WIDTH_V | DINT_PKT_BCCH_CHAINBACK_DEPTH_V |
    DINT_PKT_BCCH_YM_THRESH_V | DINT_PKT_BCCH_SMT_SCALAR_V,
  
    /* PKT_INTLV_CFG_LO */
    DINT_PKT_BCCH_INTLV_COLS_V |
    DINT_PKT_BCCH_INTLV_ROWS_V,
  
    /* PKT_BLK_SIZE_LO */
    DINT_PKT_BCCH_BLOCK_SIZE_V,
  
    /* PKT_BLK_SIZE_HI */
    DINT_PKT_BCCH_PUNCT_LENGTH_V | DINT_PKT_BCCH_SYMBOL_REPEAT_V |
    DINT_PKT_BCCH_CODE_RATE_HALF_V,
  
    /* PKT_PUNCT_LO */
    DINT_PKT_BCCH_LO_PUNCT_PATTERN_V | DINT_PKT_BCCH_RDCNT_EN_SEL_V |
    DINT_PKT_BCCH_NUM_LTU_FRM_V,
  
    /* PKT_PUNCT_HI */
    DINT_PKT_BCCH_HI_PUNCT_PATTERN_V,
  
    /* PKT_PL_CRC_LO */
    DINT_PKT_BCCH_PL_CRC_V,
  
    /* PKT_PL_CRC_HI */
    DINT_PKT_BCCH_LTU_CRC_V
  }
}
};

deint_ccch_pkt_cfg_type deint_ccch_packet_params[][DEC_NUM_CCCH_RATES] = {

  /* R=1/4 */
  {
    {
      (DINT_PKT_CCCH_CODE_1BY4_1X_RATE_ROW_WIDTH_V |
      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_CHAINBACK_DEPTH_V |
      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_YM_THRESH_V |
      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_SMT_SCALAR_V),

      (DINT_PKT_CCCH_CODE_1BY4_1X_RATE_INTLV_COLS_V |
      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_INTLV_ROWS_V),

      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_BLOCK_SIZE_V,

      (DINT_PKT_CCCH_CODE_1BY4_1X_RATE_PUNCT_LENGTH_V |
      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_SYMBOL_REPEAT_V |
      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_CODE_RATE_V),

      (DINT_PKT_CCCH_CODE_1BY4_1X_RATE_LO_PUNCT_PATTERN_V |
      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_RDCNT_EN_SEL_V |
      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_NUM_LTU_FRM_V),

      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_HI_PUNCT_PATTERN_V,

      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_PL_CRC_V,

      DINT_PKT_CCCH_CODE_1BY4_1X_RATE_LTU_CRC_V
    },
    {
      (DINT_PKT_CCCH_CODE_1BY4_2X_RATE_ROW_WIDTH_V |
      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_CHAINBACK_DEPTH_V |
      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_YM_THRESH_V |
      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_SMT_SCALAR_V),

      (DINT_PKT_CCCH_CODE_1BY4_2X_RATE_INTLV_COLS_V |
      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_INTLV_ROWS_V),

      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_BLOCK_SIZE_V,

      (DINT_PKT_CCCH_CODE_1BY4_2X_RATE_PUNCT_LENGTH_V |
      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_SYMBOL_REPEAT_V |
      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_CODE_RATE_V),

      (DINT_PKT_CCCH_CODE_1BY4_2X_RATE_LO_PUNCT_PATTERN_V |
      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_RDCNT_EN_SEL_V |
      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_NUM_LTU_FRM_V),

      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_HI_PUNCT_PATTERN_V,

      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_PL_CRC_V,

      DINT_PKT_CCCH_CODE_1BY4_2X_RATE_LTU_CRC_V
    },
    {
      (DINT_PKT_CCCH_CODE_1BY4_4X_RATE_ROW_WIDTH_V |
      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_CHAINBACK_DEPTH_V |
      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_YM_THRESH_V |
      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_SMT_SCALAR_V),

      (DINT_PKT_CCCH_CODE_1BY4_4X_RATE_INTLV_COLS_V |
      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_INTLV_ROWS_V),

      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_BLOCK_SIZE_V,

      (DINT_PKT_CCCH_CODE_1BY4_4X_RATE_PUNCT_LENGTH_V |
      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_SYMBOL_REPEAT_V |
      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_CODE_RATE_V),

      (DINT_PKT_CCCH_CODE_1BY4_4X_RATE_LO_PUNCT_PATTERN_V |
      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_RDCNT_EN_SEL_V |
      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_NUM_LTU_FRM_V),

      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_HI_PUNCT_PATTERN_V,

      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_PL_CRC_V,

      DINT_PKT_CCCH_CODE_1BY4_4X_RATE_LTU_CRC_V
    }
  },

  /* R=1/2 */
  {
    {
      (DINT_PKT_CCCH_CODE_1BY2_1X_RATE_ROW_WIDTH_V |
      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_CHAINBACK_DEPTH_V |
      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_YM_THRESH_V |
      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_SMT_SCALAR_V),

      (DINT_PKT_CCCH_CODE_1BY2_1X_RATE_INTLV_COLS_V |
      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_INTLV_ROWS_V),

      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_BLOCK_SIZE_V,

      (DINT_PKT_CCCH_CODE_1BY2_1X_RATE_PUNCT_LENGTH_V |
      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_SYMBOL_REPEAT_V |
      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_CODE_RATE_V),

      (DINT_PKT_CCCH_CODE_1BY2_1X_RATE_LO_PUNCT_PATTERN_V |
      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_RDCNT_EN_SEL_V |
      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_NUM_LTU_FRM_V),

      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_HI_PUNCT_PATTERN_V,

      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_PL_CRC_V,

      DINT_PKT_CCCH_CODE_1BY2_1X_RATE_LTU_CRC_V
    },
    {
      (DINT_PKT_CCCH_CODE_1BY2_2X_RATE_ROW_WIDTH_V |
      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_CHAINBACK_DEPTH_V |
      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_YM_THRESH_V |
      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_SMT_SCALAR_V),

      (DINT_PKT_CCCH_CODE_1BY2_2X_RATE_INTLV_COLS_V |
      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_INTLV_ROWS_V),

      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_BLOCK_SIZE_V,

      (DINT_PKT_CCCH_CODE_1BY2_2X_RATE_PUNCT_LENGTH_V |
      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_SYMBOL_REPEAT_V |
      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_CODE_RATE_V),

      (DINT_PKT_CCCH_CODE_1BY2_2X_RATE_LO_PUNCT_PATTERN_V |
      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_RDCNT_EN_SEL_V |
      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_NUM_LTU_FRM_V),

      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_HI_PUNCT_PATTERN_V,

      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_PL_CRC_V,

      DINT_PKT_CCCH_CODE_1BY2_2X_RATE_LTU_CRC_V
    },
    {
      (DINT_PKT_CCCH_CODE_1BY2_4X_RATE_ROW_WIDTH_V |
      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_CHAINBACK_DEPTH_V |
      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_YM_THRESH_V |
      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_SMT_SCALAR_V),

      (DINT_PKT_CCCH_CODE_1BY2_4X_RATE_INTLV_COLS_V |
      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_INTLV_ROWS_V),

      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_BLOCK_SIZE_V,

      (DINT_PKT_CCCH_CODE_1BY2_4X_RATE_PUNCT_LENGTH_V |
      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_SYMBOL_REPEAT_V |
      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_CODE_RATE_V),

      (DINT_PKT_CCCH_CODE_1BY2_4X_RATE_LO_PUNCT_PATTERN_V |
      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_RDCNT_EN_SEL_V |
      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_NUM_LTU_FRM_V),

      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_HI_PUNCT_PATTERN_V,

      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_PL_CRC_V,

      DINT_PKT_CCCH_CODE_1BY2_4X_RATE_LTU_CRC_V
    }
  }
};
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

  DEINTERLEAVER PACKET CONFIGURATION PARAMETERS FOR FCH FOR RADIO 
  CONFIGURATION 1-5.

============================================================================*/

static const deint_fch_pkt_cfg_type deint_fch_packet_params[] = {
{

  /************ RC1 Full Rate Packet Parameters *******/

  {

    DINT_PKT_FCH_RC1_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC1_FULL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC1_FULL_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC1_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC1_ALL_RATE_BLOCK_SIZE_V, 
       
    DINT_PKT_FCH_RC1_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC1_FULL_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC1_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC1_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC1_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC1_ALL_RATE_HI_PUNCT_PATTERN_V, 
       
    DINT_PKT_FCH_RC1_FULL_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC1_ALL_RATE_LTU_CRC_V
  },

   /********* RC1 HALF Rate Packet Parameters **********/

  {

    DINT_PKT_FCH_RC1_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC1_HALF_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC1_HALF_RATE_SMT_SCALAR_V, 
   
    DINT_PKT_FCH_RC1_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC1_ALL_RATE_BLOCK_SIZE_V, 
    
    DINT_PKT_FCH_RC1_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC1_HALF_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC1_ALL_RATE_CODE_RATE_V, 
   
    DINT_PKT_FCH_RC1_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC1_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC1_ALL_RATE_HI_PUNCT_PATTERN_V, 
       
    DINT_PKT_FCH_RC1_HALF_RATE_PL_CRC_V, 
   
    DINT_PKT_FCH_RC1_ALL_RATE_LTU_CRC_V

  },

  /******** RC1 QUARTER Rate Packet Parameters ***********/

  { 
    
    DINT_PKT_FCH_RC1_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC1_QUARTER_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC1_QUARTER_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC1_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC1_ALL_RATE_BLOCK_SIZE_V,

    DINT_PKT_FCH_RC1_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC1_QUARTER_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC1_ALL_RATE_CODE_RATE_V, 
    
    DINT_PKT_FCH_RC1_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC1_ALL_RATE_NUM_LTU_FRM_V, 
    
    DINT_PKT_FCH_RC1_ALL_RATE_HI_PUNCT_PATTERN_V, 
   
    DINT_PKT_FCH_RC1_QUARTER_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC1_ALL_RATE_LTU_CRC_V

  },

  /********* RC1 EIGHTH Rate Packet Parameters ****************/
 
  {

    DINT_PKT_FCH_RC1_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_CHAINBACK_DEPTH_V |
    DINT_PKT_FCH_RC1_EIGHTH_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC1_EIGHTH_RATE_SMT_SCALAR_V, 
    
    DINT_PKT_FCH_RC1_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_INTLV_ROWS_V,
  
    DINT_PKT_FCH_RC1_ALL_RATE_BLOCK_SIZE_V, 
   
    DINT_PKT_FCH_RC1_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC1_EIGHTH_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC1_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC1_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC1_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC1_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC1_ALL_RATE_HI_PUNCT_PATTERN_V, 

    DINT_PKT_FCH_RC1_EIGHTH_RATE_PL_CRC_V, 
  
    DINT_PKT_FCH_RC1_ALL_RATE_LTU_CRC_V

  },

},

/***********************RC2********************************/

{

  /***************** RC2 Full Rate Packet Parameters *******/

  { 

    DINT_PKT_FCH_RC2_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC2_FULL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC2_FULL_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC2_ALL_RATE_BLOCK_SIZE_V, 
    
    DINT_PKT_FCH_RC2_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC2_FULL_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC2_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC2_ALL_RATE_NUM_LTU_FRM_V, 
    
    DINT_PKT_FCH_RC2_ALL_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_FCH_RC2_FULL_RATE_PL_CRC_V, 
    
    DINT_PKT_FCH_RC2_ALL_RATE_LTU_CRC_V

},

  /*************** RC2 HALF Rate Packet Parameters **********/

  { 
   
    DINT_PKT_FCH_RC2_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC2_HALF_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC2_HALF_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_INTLV_ROWS_V,
    
    DINT_PKT_FCH_RC2_ALL_RATE_BLOCK_SIZE_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC2_HALF_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC2_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC2_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_HI_PUNCT_PATTERN_V, 
       
    DINT_PKT_FCH_RC2_HALF_RATE_PL_CRC_V, 
    
    DINT_PKT_FCH_RC2_ALL_RATE_LTU_CRC_V

  },

  /************ RC2 QUARTER Rate Packet Parameters ******************/

  { 
   
    DINT_PKT_FCH_RC2_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC2_QUARTER_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC2_QUARTER_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_INTLV_ROWS_V,
    
    DINT_PKT_FCH_RC2_ALL_RATE_BLOCK_SIZE_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC2_QUARTER_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC2_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC2_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_HI_PUNCT_PATTERN_V, 
       
    DINT_PKT_FCH_RC2_QUARTER_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC2_ALL_RATE_LTU_CRC_V

  },

  /************* RC2 EIGHTH Rate Packet Parameters *********************/

  {

    DINT_PKT_FCH_RC2_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC2_EIGHTH_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC2_EIGHTH_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC2_ALL_RATE_BLOCK_SIZE_V, 

    DINT_PKT_FCH_RC2_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC2_EIGHTH_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC2_ALL_RATE_CODE_RATE_V, 

    DINT_PKT_FCH_RC2_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC2_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC2_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC2_ALL_RATE_HI_PUNCT_PATTERN_V, 

    DINT_PKT_FCH_RC2_EIGHTH_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC2_ALL_RATE_LTU_CRC_V

  },

},

/*****************************RC3************************************/
{


  /*********************RC3 FULL Rate Packet Parameters *************/

  { 

    DINT_PKT_FCH_RC3_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC3_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC3_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC3_FULL_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC3_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC3_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC3_ALL_RATE_BLOCK_SIZE_V, 
       
    DINT_PKT_FCH_RC3_FULL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC3_FULL_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC3_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC3_ALL_RATE_LO_PUNCT_PATTERN_V |
    DINT_PKT_FCH_RC3_FULL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC3_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC3_FULL_RATE_HI_PUNCT_PATTERN_V,
        
    DINT_PKT_FCH_RC3_FULL_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC3_ALL_RATE_LTU_CRC_V

  },

  /****************** RC3 HALF Rate Packet Parameters ************/

  { 

    DINT_PKT_FCH_RC3_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC3_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC3_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC3_HALF_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC3_ALL_RATE_INTLV_COLS_V |
    DINT_PKT_FCH_RC3_ALL_RATE_INTLV_ROWS_V,
    
    DINT_PKT_FCH_RC3_ALL_RATE_BLOCK_SIZE_V,
        
    DINT_PKT_FCH_RC3_HALF_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC3_HALF_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC3_ALL_RATE_CODE_RATE_V, 
    
    DINT_PKT_FCH_RC3_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC3_HALF_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC3_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC3_HALF_RATE_HI_PUNCT_PATTERN_V,
        
    DINT_PKT_FCH_RC3_HALF_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC3_ALL_RATE_LTU_CRC_V

  },

  /*************** RC3 QUARTER Rate Packet Parameters ***************/

  { 
     
    DINT_PKT_FCH_RC3_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC3_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC3_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC3_QUARTER_RATE_SMT_SCALAR_V, 
    
    DINT_PKT_FCH_RC3_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC3_ALL_RATE_INTLV_ROWS_V,
    
    DINT_PKT_FCH_RC3_ALL_RATE_BLOCK_SIZE_V,

    DINT_PKT_FCH_RC3_QUARTER_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC3_QUARTER_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC3_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC3_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC3_QUARTER_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC3_ALL_RATE_NUM_LTU_FRM_V,
        
    DINT_PKT_FCH_RC3_QUARTER_RATE_HI_PUNCT_PATTERN_V,

    DINT_PKT_FCH_RC3_QUARTER_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC3_ALL_RATE_LTU_CRC_V

  },

  /************* RC3 EIGHTH Rate Packet Parameters *******************/

  { 
    
    DINT_PKT_FCH_RC3_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC3_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC3_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC3_EIGHTH_RATE_SMT_SCALAR_V,
        
    DINT_PKT_FCH_RC3_ALL_RATE_INTLV_COLS_V |
    DINT_PKT_FCH_RC3_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC3_ALL_RATE_BLOCK_SIZE_V, 
    
    DINT_PKT_FCH_RC3_EIGHTH_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC3_EIGHTH_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC3_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC3_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC3_EIGHTH_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC3_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC3_EIGHTH_RATE_HI_PUNCT_PATTERN_V,

    DINT_PKT_FCH_RC3_EIGHTH_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC3_ALL_RATE_LTU_CRC_V

  },

},

/************************RC4************************************/

{


  /*****************RC4 FULL Rate Packet Parameters *************/

  { 

    DINT_PKT_FCH_RC4_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC4_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC4_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC4_FULL_RATE_SMT_SCALAR_V,
    
    DINT_PKT_FCH_RC4_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC4_ALL_RATE_INTLV_ROWS_V,
    
    DINT_PKT_FCH_RC4_ALL_RATE_BLOCK_SIZE_V,

    DINT_PKT_FCH_RC4_FULL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC4_FULL_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC4_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC4_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC4_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC4_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC4_FULL_RATE_HI_PUNCT_PATTERN_V,
        
    DINT_PKT_FCH_RC4_FULL_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC4_ALL_RATE_LTU_CRC_V

  },

  /**************** RC4 HALF Rate Packet Parameters *************/

  { 
     
    DINT_PKT_FCH_RC4_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC4_ALL_RATE_CHAINBACK_DEPTH_V |
    DINT_PKT_FCH_RC4_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC4_HALF_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC4_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC4_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC4_ALL_RATE_BLOCK_SIZE_V,
        
    DINT_PKT_FCH_RC4_HALF_RATE_PUNCT_LENGTH_V |
    DINT_PKT_FCH_RC4_HALF_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC4_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC4_ALL_RATE_LO_PUNCT_PATTERN_V |
    DINT_PKT_FCH_RC4_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC4_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC4_HALF_RATE_HI_PUNCT_PATTERN_V, 
       
    DINT_PKT_FCH_RC4_HALF_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC4_ALL_RATE_LTU_CRC_V

  },

  /********** RC4 QUARTER Rate Packet Parameters ***************/

  { 
     
    DINT_PKT_FCH_RC4_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC4_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC4_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC4_QUARTER_RATE_SMT_SCALAR_V,
        
    DINT_PKT_FCH_RC4_ALL_RATE_INTLV_COLS_V |
    DINT_PKT_FCH_RC4_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC4_ALL_RATE_BLOCK_SIZE_V,
    
    DINT_PKT_FCH_RC4_QUARTER_RATE_PUNCT_LENGTH_V |
    DINT_PKT_FCH_RC4_QUARTER_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC4_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC4_ALL_RATE_LO_PUNCT_PATTERN_V |
    DINT_PKT_FCH_RC4_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC4_ALL_RATE_NUM_LTU_FRM_V,                         
       
    DINT_PKT_FCH_RC4_QUARTER_RATE_HI_PUNCT_PATTERN_V, 
       
    DINT_PKT_FCH_RC4_QUARTER_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC4_ALL_RATE_LTU_CRC_V

  },

  /************ RC4 EIGHTH Rate Packet Parameters ******************/
 
  { 
     
    DINT_PKT_FCH_RC4_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC4_ALL_RATE_CHAINBACK_DEPTH_V |
    DINT_PKT_FCH_RC4_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC4_EIGHTH_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC4_ALL_RATE_INTLV_COLS_V |
    DINT_PKT_FCH_RC4_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC4_ALL_RATE_BLOCK_SIZE_V,
        
    DINT_PKT_FCH_RC4_EIGHTH_RATE_PUNCT_LENGTH_V |
    DINT_PKT_FCH_RC4_EIGHTH_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC4_ALL_RATE_CODE_RATE_V,
        
    DINT_PKT_FCH_RC4_ALL_RATE_LO_PUNCT_PATTERN_V |
    DINT_PKT_FCH_RC4_ALL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC4_ALL_RATE_NUM_LTU_FRM_V,
        
    DINT_PKT_FCH_RC4_EIGHTH_RATE_HI_PUNCT_PATTERN_V, 

    DINT_PKT_FCH_RC4_EIGHTH_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC4_ALL_RATE_LTU_CRC_V

  },

},

/******************RC5*****************************************/
{


  /**************RC5 FULL Rate Packet Parameters *************/

  { 

    DINT_PKT_FCH_RC5_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC5_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC5_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC5_FULL_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC5_ALL_RATE_INTLV_ROWS_V,
    
    DINT_PKT_FCH_RC5_ALL_RATE_BLOCK_SIZE_V,
        
    DINT_PKT_FCH_RC5_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC5_FULL_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC5_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC5_FULL_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC5_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_HI_PUNCT_PATTERN_V, 
       
    DINT_PKT_FCH_RC5_FULL_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC5_ALL_RATE_LTU_CRC_V

  },

  /************** RC5 HALF Rate Packet Parameters *****************/

  { 
     
    DINT_PKT_FCH_RC5_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC5_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC5_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC5_HALF_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC5_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC5_ALL_RATE_BLOCK_SIZE_V, 
    
    DINT_PKT_FCH_RC5_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC5_HALF_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC5_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC5_HALF_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC5_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_HI_PUNCT_PATTERN_V,

    DINT_PKT_FCH_RC5_HALF_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC5_ALL_RATE_LTU_CRC_V

  },

  /************* RC5 QUARTER Rate Packet Parameters **************/

  {

    DINT_PKT_FCH_RC5_ALL_RATE_ROW_WIDTH_V | 
    DINT_PKT_FCH_RC5_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC5_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC5_QUARTER_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC5_ALL_RATE_INTLV_ROWS_V,

    DINT_PKT_FCH_RC5_ALL_RATE_BLOCK_SIZE_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC5_QUARTER_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC5_ALL_RATE_CODE_RATE_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_FCH_RC5_QUARTER_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC5_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_HI_PUNCT_PATTERN_V,
        
    DINT_PKT_FCH_RC5_QUARTER_RATE_PL_CRC_V, 

    DINT_PKT_FCH_RC5_ALL_RATE_LTU_CRC_V

  },

  /************* RC5 EIGHTH Rate Packet Parameters ************************/

  { 
     
    DINT_PKT_FCH_RC5_ALL_RATE_ROW_WIDTH_V |
    DINT_PKT_FCH_RC5_ALL_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_FCH_RC5_ALL_RATE_YM_THRESH_V |
    DINT_PKT_FCH_RC5_EIGHTH_RATE_SMT_SCALAR_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_INTLV_COLS_V | 
    DINT_PKT_FCH_RC5_ALL_RATE_INTLV_ROWS_V,
    DINT_PKT_FCH_RC5_ALL_RATE_BLOCK_SIZE_V,
        
    DINT_PKT_FCH_RC5_ALL_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_FCH_RC5_EIGHTH_RATE_SYMBOL_REPEAT_V |
    DINT_PKT_FCH_RC5_ALL_RATE_CODE_RATE_V,
        
    DINT_PKT_FCH_RC5_ALL_RATE_LO_PUNCT_PATTERN_V |
    DINT_PKT_FCH_RC5_EIGHTH_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_FCH_RC5_ALL_RATE_NUM_LTU_FRM_V, 
       
    DINT_PKT_FCH_RC5_ALL_RATE_HI_PUNCT_PATTERN_V, 
       
    DINT_PKT_FCH_RC5_EIGHTH_RATE_PL_CRC_V, 
    
    DINT_PKT_FCH_RC5_ALL_RATE_LTU_CRC_V

  },

}

};

/*============================================================================

  DEINTERLEAVER PACKET CONFIGURATION PARAMETERS FOR IS95C Supplemental Channels

==============================================================================*/
                                            
static const deint_sch_pkt_cfg_type 
  deint_sch_pkt_params[NUM_SCH_RC][NUM_SCH_RATES] = {

{
   /********** RC3 1X SCH Packet Parameters ***********/

  {

    DINT_PKT_SCH_RC3_1X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC3_1X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC3_1X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC3_1X_RATE_SMT_SCALAR_V,

    DINT_PKT_SCH_RC3_1X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC3_1X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC3_1X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC3_1X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC3_1X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC3_1X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC3_1X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC3_1X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC3_1X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC3_1X_RATE_LO_PUNCT_PATTERN_V | 
    //DINT_PKT_SCH_RC3_1X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC3_1X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC3_1X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC3_1X_RATE_PL_CRC_V, 
    
    DINT_PKT_SCH_RC3_1X_RATE_LTU_CRC_V
  },

   /********** RC3 2X SCH Packet Parameters ***********/

  {

    DINT_PKT_SCH_RC3_2X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC3_2X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC3_2X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC3_2X_RATE_SMT_SCALAR_V,

    DINT_PKT_SCH_RC3_2X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC3_2X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC3_2X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC3_2X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC3_2X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC3_2X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC3_2X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC3_2X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC3_2X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC3_2X_RATE_LO_PUNCT_PATTERN_V | 
    //DINT_PKT_SCH_RC3_2X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC3_2X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC3_2X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC3_2X_RATE_PL_CRC_V, 
    
    DINT_PKT_SCH_RC3_2X_RATE_LTU_CRC_V
  },

   /********** RC3 4X SCH Packet Parameters ***********/

  { 
    DINT_PKT_SCH_RC3_4X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC3_4X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC3_4X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC3_4X_RATE_SMT_SCALAR_V,
     
    DINT_PKT_SCH_RC3_4X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC3_4X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC3_4X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC3_4X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC3_4X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC3_4X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC3_4X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC3_4X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC3_4X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC3_4X_RATE_LO_PUNCT_PATTERN_V |
    DINT_PKT_SCH_RC3_4X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC3_4X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC3_4X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC3_4X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC3_4X_RATE_LTU_CRC_V

  },

   /********** RC3 8X SCH Packet Parameters ***********/

  {
  
    DINT_PKT_SCH_RC3_8X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC3_8X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC3_8X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC3_8X_RATE_SMT_SCALAR_V,

    DINT_PKT_SCH_RC3_8X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC3_8X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC3_8X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC3_8X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC3_8X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC3_8X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC3_8X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC3_8X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC3_8X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC3_8X_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_SCH_RC3_8X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC3_8X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC3_8X_RATE_HI_PUNCT_PATTERN_V, 
       
    DINT_PKT_SCH_RC3_8X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC3_8X_RATE_LTU_CRC_V

  },

   /********** RC3 16X SCH Packet Parameters ***********/

  {

    DINT_PKT_SCH_RC3_16X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC3_16X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC3_16X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC3_16X_RATE_SMT_SCALAR_V,

    DINT_PKT_SCH_RC3_16X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC3_16X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC3_16X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC3_16X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC3_16X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC3_16X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC3_16X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC3_16X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC3_16X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC3_16X_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_SCH_RC3_16X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC3_16X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC3_16X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC3_16X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC3_16X_RATE_LTU_CRC_V

  }
},

{


   /********** RC4 1X SCH Packet Parameters ***********/

  {

    DINT_PKT_SCH_RC4_1X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC4_1X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC4_1X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC4_1X_RATE_SMT_SCALAR_V,

    DINT_PKT_SCH_RC4_1X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC4_1X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC4_1X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_1X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC4_1X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC4_1X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC4_1X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC4_1X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_1X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC4_1X_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_SCH_RC4_1X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC4_1X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC4_1X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC4_1X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC4_1X_RATE_LTU_CRC_V
  },

   /********** RC4 2X SCH Packet Parameters ***********/

  {

    DINT_PKT_SCH_RC4_2X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC4_2X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC4_2X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC4_2X_RATE_SMT_SCALAR_V,

    DINT_PKT_SCH_RC4_2X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC4_2X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC4_2X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_2X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC4_2X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC4_2X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC4_2X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC4_2X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_2X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC4_2X_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_SCH_RC4_2X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC4_2X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC4_2X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC4_2X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC4_2X_RATE_LTU_CRC_V
  },

   /********** RC4 4X SCH Packet Parameters ***********/

  { 
    DINT_PKT_SCH_RC4_4X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC4_4X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC4_4X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC4_4X_RATE_SMT_SCALAR_V,
     
    DINT_PKT_SCH_RC4_4X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC4_4X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC4_4X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_4X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC4_4X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC4_4X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC4_4X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC4_4X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_4X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC4_4X_RATE_LO_PUNCT_PATTERN_V |
    DINT_PKT_SCH_RC4_4X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC4_4X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC4_4X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC4_4X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC4_4X_RATE_LTU_CRC_V

  },

   /********** RC4 8X SCH Packet Parameters ***********/

  {
  
    DINT_PKT_SCH_RC4_8X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC4_8X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC4_8X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC4_8X_RATE_SMT_SCALAR_V,
    DINT_PKT_SCH_RC4_8X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC4_8X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC4_8X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_8X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC4_8X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC4_8X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC4_8X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC4_8X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_8X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC4_8X_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_SCH_RC4_8X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC4_8X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC4_8X_RATE_HI_PUNCT_PATTERN_V, 
       
    DINT_PKT_SCH_RC4_8X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC4_8X_RATE_LTU_CRC_V

  },

   /********** RC4 16X SCH Packet Parameters ***********/

  {

    DINT_PKT_SCH_RC4_16X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC4_16X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC4_16X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC4_16X_RATE_SMT_SCALAR_V,

    DINT_PKT_SCH_RC4_16X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC4_16X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC4_16X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_16X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC4_16X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC4_16X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC4_16X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC4_16X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_16X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC4_16X_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_SCH_RC4_16X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC4_16X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC4_16X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC4_16X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC4_16X_RATE_LTU_CRC_V

},

#ifdef FEATURE_32X_SCH_DATA_RATE
     /********** RC4 32X SCH Packet Parameters ***********/
  {

    DINT_PKT_SCH_RC4_32X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC4_32X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC4_32X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC4_32X_RATE_SMT_SCALAR_V,

    DINT_PKT_SCH_RC4_32X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC4_32X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC4_32X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_32X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC4_32X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC4_32X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC4_32X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC4_32X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC4_32X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC4_32X_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_SCH_RC4_32X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC4_32X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC4_32X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC4_32X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC4_32X_RATE_LTU_CRC_V
  },
#endif /* FEATURE_32X_SCH_DATA_RATE */
},

{

   /********** RC5 1X SCH Packet Parameters ***********/

  {

    DINT_PKT_SCH_RC5_1X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC5_1X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC5_1X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC5_1X_RATE_SMT_SCALAR_V,

    DINT_PKT_SCH_RC5_1X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC5_1X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC5_1X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC5_1X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC5_1X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC5_1X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC5_1X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC5_1X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC5_1X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC5_1X_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_SCH_RC5_1X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC5_1X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC5_1X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC5_1X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC5_1X_RATE_LTU_CRC_V
  },

   /********** RC5 2X SCH Packet Parameters ***********/

  {

    DINT_PKT_SCH_RC5_2X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC5_2X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC5_2X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC5_2X_RATE_SMT_SCALAR_V,

    DINT_PKT_SCH_RC5_2X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC5_2X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC5_2X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC5_2X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC5_2X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC5_2X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC5_2X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC5_2X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC5_2X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC5_2X_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_SCH_RC5_2X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC5_2X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC5_2X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC5_2X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC5_2X_RATE_LTU_CRC_V
  },

   /********** RC5 4X SCH Packet Parameters ***********/

  { 
    DINT_PKT_SCH_RC5_4X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC5_4X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC5_4X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC5_4X_RATE_SMT_SCALAR_V,
     
    DINT_PKT_SCH_RC5_4X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC5_4X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC5_4X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC5_4X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC5_4X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC5_4X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC5_4X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC5_4X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC5_4X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC5_4X_RATE_LO_PUNCT_PATTERN_V |
    DINT_PKT_SCH_RC5_4X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC5_4X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC5_4X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC5_4X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC5_4X_RATE_LTU_CRC_V

  },

   /********** RC5 8X SCH Packet Parameters ***********/

  {
  
    DINT_PKT_SCH_RC5_8X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC5_8X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC5_8X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC5_8X_RATE_SMT_SCALAR_V,
    DINT_PKT_SCH_RC5_8X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC5_8X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC5_8X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC5_8X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC5_8X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC5_8X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC5_8X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC5_8X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC5_8X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC5_8X_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_SCH_RC5_8X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC5_8X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC5_8X_RATE_HI_PUNCT_PATTERN_V, 
       
    DINT_PKT_SCH_RC5_8X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC5_8X_RATE_LTU_CRC_V

  },

   /********** RC5 16X SCH Packet Parameters ***********/

  {

    DINT_PKT_SCH_RC5_16X_RATE_ROW_WIDTH_V | 
    DINT_PKT_SCH_RC5_16X_RATE_CHAINBACK_DEPTH_V | 
    DINT_PKT_SCH_RC5_16X_RATE_YM_THRESH_V | 
    DINT_PKT_SCH_RC5_16X_RATE_SMT_SCALAR_V,

    DINT_PKT_SCH_RC5_16X_RATE_INTLV_COLS_V | 
    DINT_PKT_SCH_RC5_16X_RATE_INTLV_ROWS_V,
  
    DINT_PKT_SCH_RC5_16X_RATE_BLOCK_SIZE_V, 

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC5_16X_RATE_LTU_LEN_HI_V | 
#else
    DINT_PKT_SCH_RC5_16X_RATE_LTU_LEN_V | 
#endif
    DINT_PKT_SCH_RC5_16X_RATE_PUNCT_LENGTH_V | 
    DINT_PKT_SCH_RC5_16X_RATE_SYMBOL_REPEAT_V | 
    DINT_PKT_SCH_RC5_16X_RATE_CODE_RATE_V,

#ifdef MSMHW_EXTENDED_LTU_LEN
    DINT_PKT_SCH_RC5_16X_RATE_LTU_LEN_LO_V | 
#endif
    DINT_PKT_SCH_RC5_16X_RATE_LO_PUNCT_PATTERN_V | 
    DINT_PKT_SCH_RC5_16X_RATE_RDCNT_EN_SEL_V |
    DINT_PKT_SCH_RC5_16X_RATE_NUM_LTU_FRM_V, 

    DINT_PKT_SCH_RC5_16X_RATE_HI_PUNCT_PATTERN_V, 
    
    DINT_PKT_SCH_RC5_16X_RATE_PL_CRC_V, 
  
    DINT_PKT_SCH_RC5_16X_RATE_LTU_CRC_V

  }
}

};


#ifdef MSMHW_TURBO_CODING

/*=====================================================
  TURBO DECODER PARAMETERS for RC3-RC5 Supplemental 
  Channels (IS95C-SCH) 
=======================================================*/

static const deint_td_params_type 
  deint_td_sch_params[NUM_SCH_RC][NUM_TD_SCH_RATES] = {

  {
     /**** RC3 2X Deinterleaver/Turbo Decoder Parameters ****/

     {  
       DINT_TD_SCH_RC3_2X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC3_2X_RATE_ROW_WIDTH_V, 
       DINT_TD_SCH_RC3_2X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC3_2X_RATE_INTLV_ROWS_V, 
       DINT_TD_SCH_RC3_2X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC3_2X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC3_2X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC3_2X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC3_2X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC3_2X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC3_2X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC3_2X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC3_2X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC3_2X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC3_2X_RATE_NUM_WINDOWS_V,
     },

     /**** RC3 4X Deinterleaver/Turbo Decoder Parameters *****/

     {
       DINT_TD_SCH_RC3_4X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC3_4X_RATE_ROW_WIDTH_V,
       DINT_TD_SCH_RC3_4X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC3_4X_RATE_INTLV_ROWS_V,
       DINT_TD_SCH_RC3_4X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC3_4X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC3_4X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC3_4X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC3_4X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC3_4X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC3_4X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC3_4X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC3_4X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC3_4X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC3_4X_RATE_NUM_WINDOWS_V,
     },
    
     /***** RC3 8X Deinterleaver/Turbo Decoder Parameters *****/

     {
       DINT_TD_SCH_RC3_8X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC3_8X_RATE_ROW_WIDTH_V,
       DINT_TD_SCH_RC3_8X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC3_8X_RATE_INTLV_ROWS_V,
       DINT_TD_SCH_RC3_8X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC3_8X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC3_8X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC3_8X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC3_8X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC3_8X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC3_8X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC3_8X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC3_8X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC3_8X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC3_8X_RATE_NUM_WINDOWS_V,
     },

     /***** RC3 16X Deinterleaver/Turbo Decoder Parameters *****/

     {
       DINT_TD_SCH_RC3_16X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC3_16X_RATE_ROW_WIDTH_V,
       DINT_TD_SCH_RC3_16X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC3_16X_RATE_INTLV_ROWS_V,
       DINT_TD_SCH_RC3_16X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC3_16X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC3_16X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC3_16X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC3_16X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC3_16X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC3_16X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC3_16X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC3_16X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC3_16X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC3_16X_RATE_NUM_WINDOWS_V,
     }
  },
  
  {
     /**** RC4 2X Deinterleaver/Turbo Decoder Parameters ****/

     {  
       DINT_TD_SCH_RC4_2X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC4_2X_RATE_ROW_WIDTH_V, 
       DINT_TD_SCH_RC4_2X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC4_2X_RATE_INTLV_ROWS_V, 
       DINT_TD_SCH_RC4_2X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC4_2X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC4_2X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC4_2X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC4_2X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC4_2X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC4_2X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC4_2X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC4_2X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC4_2X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC4_2X_RATE_NUM_WINDOWS_V,
     },

     /**** RC4 4X Deinterleaver/Turbo Decoder Parameters *****/

     {
       DINT_TD_SCH_RC4_4X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC4_4X_RATE_ROW_WIDTH_V,
       DINT_TD_SCH_RC4_4X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC4_4X_RATE_INTLV_ROWS_V,
       DINT_TD_SCH_RC4_4X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC4_4X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC4_4X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC4_4X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC4_4X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC4_4X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC4_4X_RATE__FRAME_20M_RATE_HYP_V | 
       DINT_TD_SCH_RC4_4X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC4_4X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC4_4X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC4_4X_RATE_NUM_WINDOWS_V,
     },
    
     /***** RC4 8X Deinterleaver/Turbo Decoder Parameters *****/

     {
       DINT_TD_SCH_RC4_8X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC4_8X_RATE_ROW_WIDTH_V,
       DINT_TD_SCH_RC4_8X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC4_8X_RATE_INTLV_ROWS_V,
       DINT_TD_SCH_RC4_8X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC4_8X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC4_8X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC4_8X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC4_8X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC4_8X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC4_8X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC4_8X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC4_8X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC4_8X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC4_8X_RATE_NUM_WINDOWS_V,
     },

     /***** RC4 16X Deinterleaver/Turbo Decoder Parameters *****/

     {
       DINT_TD_SCH_RC4_16X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC4_16X_RATE_ROW_WIDTH_V,
       DINT_TD_SCH_RC4_16X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC4_16X_RATE_INTLV_ROWS_V,
       DINT_TD_SCH_RC4_16X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC4_16X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC4_16X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC4_16X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC4_16X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC4_16X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC4_16X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC4_16X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC4_16X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC4_16X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC4_16X_RATE_NUM_WINDOWS_V,
     },
  
#ifdef FEATURE_32X_SCH_DATA_RATE
     /***** RC4 32X Deinterleaver/Turbo Decoder Parameters *****/

     {
       DINT_TD_SCH_RC4_32X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC4_32X_RATE_ROW_WIDTH_V,
       DINT_TD_SCH_RC4_32X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC4_32X_RATE_INTLV_ROWS_V,
       DINT_TD_SCH_RC4_32X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC4_32X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC4_32X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC4_32X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC4_32X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC4_32X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC4_32X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC4_32X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC4_32X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC4_32X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC4_32X_RATE_NUM_WINDOWS_V,
     },
#endif /* FEATURE_32X_SCH_DATA_RATE */
  },

      
  {
     /**** RC5 2X Deinterleaver/Turbo Decoder Parameters ****/

     {  
       DINT_TD_SCH_RC5_2X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC5_2X_RATE_ROW_WIDTH_V, 
       DINT_TD_SCH_RC5_2X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC5_2X_RATE_INTLV_ROWS_V, 
       DINT_TD_SCH_RC5_2X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC5_2X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC5_2X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC5_2X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC5_2X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC5_2X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC5_2X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC5_2X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC5_2X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC5_2X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC5_2X_RATE_NUM_WINDOWS_V,
     },

     /**** RC5 4X Deinterleaver/Turbo Decoder Parameters *****/

     {
       DINT_TD_SCH_RC5_4X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC5_4X_RATE_ROW_WIDTH_V,
       DINT_TD_SCH_RC5_4X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC5_4X_RATE_INTLV_ROWS_V,
       DINT_TD_SCH_RC5_4X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC5_4X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC5_4X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC5_4X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC5_4X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC5_4X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC5_4X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC5_4X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC5_4X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC5_4X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC5_4X_RATE_NUM_WINDOWS_V,
     },
    
     /***** RC5 8X Deinterleaver/Turbo Decoder Parameters *****/

     {
       DINT_TD_SCH_RC5_8X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC5_8X_RATE_ROW_WIDTH_V,
       DINT_TD_SCH_RC5_8X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC5_8X_RATE_INTLV_ROWS_V,
       DINT_TD_SCH_RC5_8X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC5_8X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC5_8X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC5_8X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC5_8X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC5_8X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC5_8X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC5_8X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC5_8X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC5_8X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC5_8X_RATE_NUM_WINDOWS_V,
     },

     /***** RC5 16X Deinterleaver/Turbo Decoder Parameters *****/

     {
       DINT_TD_SCH_RC5_16X_RATE_BLOCK_SIZE_V,
       DINT_TD_SCH_RC5_16X_RATE_ROW_WIDTH_V,
       DINT_TD_SCH_RC5_16X_RATE_INTLV_COLS_V |
       DINT_TD_SCH_RC5_16X_RATE_INTLV_ROWS_V,
       DINT_TD_SCH_RC5_16X_RATE_MIN_LLR_V,
       DINT_TD_SCH_RC5_16X_RATE_LO_PUNCT_PATTERN_V |
       DINT_TD_SCH_RC5_16X_RATE_PUNCT_LENGTH_V |
       DINT_TD_SCH_RC5_16X_RATE_CODE_RATE_V,
       DINT_TD_SCH_RC5_16X_RATE_HI_PUNCT_PATTERN_V,
       DINT_TD_EARLY_TERM_EN |
       DINT_TD_HYP_X1_ON |
       DINT_TD_SCH_RC5_16X_RATE_TD_INTLV_LEN_V,
       DINT_TD_MAX_ITER_NUM |
       DINT_TD_MIN_ITER_NUM,
       DINT_TD_SCH_RC5_16X_RATE__FRAME_20M_RATE_HYP_V |
       DINT_TD_SCH_RC5_16X_RATE__SLIDE_WIN_LEN_V |
       DINT_TD_SCH_RC5_16X_RATE__PARTIAL_WIN_LEN_V |
       DINT_TD_SCH_RC5_16X_RATE_BETA_V,
       DINT_TD_MIN_NUM_CRC_PASS |
       DINT_TD_SCH_RC5_16X_RATE_NUM_WINDOWS_V,
     }
  }
}; 
 
#endif /* MSMHW_TURBO_CODING */

/*--------------------------------------------------------------------------
 * Private Function Prototypes
 *--------------------------------------------------------------------------*/
static void deint_load_sc_pkt_params (void);
static void deint_create_sc_task_list (void);

static void deint_load_pc_pkt_params (deint_pc_rate_type pc_rate);
static void deint_create_pc_task_list (void);

#ifdef FEATURE_IS2000_REL_A_CC
void deint_allocate_ram_nccch (void);
static void deint_load_bcch_pkt_params (cai_code_rate_type code_rate);
static void deint_create_bcch_task_list (deint_ch_id_type ch_id, byte ncc_actchan_mask);
#endif

static boolean deint_load_fch_pkt_params (deint_rc_type fch_rc);

#ifndef FEATURE_IS2000_REL_A_SVD
static void deint_create_fch_task_list (
  deint_rc_type fch_rc,
  boolean int_enable
);
#else
static void deint_create_fch_task_list (
  deint_rc_type fch_rc, 
  boolean       fch_pc
);

static void deint_create_dcch_task_list
(
  deint_rc_type dcch_rc, 
  boolean dcch_pc
);

static void deint_load_dcch_pkt_params (deint_rc_type dcch_rc);
#endif /* !FEATURE_IS2000_REL_A_SVD */

/* Currently, we do not support mixed rates on SCCH.  Use FCH pkt_params. */
static void deint_create_scch_task_list (deint_rc_type scch_rc);

static void deint_load_sch_pkt_params (
#ifdef FEATURE_IS2000_REL_A //32X
  uint8   num_ltu,          
  uint16  ltu_len,          
#endif /* FEATURE_IS2000_REL_A */
  deint_rc_type sch_rc, 
  dec_sch_rate_type sch_rate
);
static void deint_create_sch_task_list (
#ifdef FEATURE_IS2000_REL_A
  dec_mux_pdu_type_type pdu_type,
#endif
  deint_rc_type sch_rc,
  dec_sch_rate_type dec_sch_rate
);


static void deint_allocate_ch1_ram_scch (void);
static void deint_allocate_ch2_ram_sch (dec_coding_type dec_coding);

#ifdef MSMHW_TURBO_CODING
static void deint_create_dummy_sch_task_list (void);
void deint_load_td_deint_params
(
  deint_rc_type sch_rc,
  dec_sch_rate_type dec_sch_rate
);

void deint_load_td_dec_params
(
  deint_rc_type sch_rc,
  dec_sch_rate_type dec_sch_rate
);
#endif /* MSMHW_TURBO_CODING */

/*============================================================================

FUNCTION DEINT_INIT

DESCRIPTION 
  Enables the DEC clock regime and selects CHIPX8 clock. 
  Calls dec_init to initialize the SVD Polynomials. 

DEPENDENCIES
  DEC Clock regime should have been enabled.

RETURN VALUE 
  None

SIDE EFFECTS
  None
=============================================================================*/
void deint_init
(
  void
)
{ 
  byte i;

  (void) q_init(&deint_sequencing_cmd_q);
  (void) q_init(&deint_sequencing_free_q);
  for(i=0; i< DEINT_MAX_SEQUENCING_CMD; i++)
  {
    deint_sequencing_cmd_buf[i].hdr.done_q_ptr = &deint_sequencing_free_q;
    q_put(&deint_sequencing_free_q,
          q_link(&deint_sequencing_cmd_buf[i], &deint_sequencing_cmd_buf[i].
                                                hdr.link));
   }

#ifdef FEATURE_SRCH_CLKREGIM_RM
  /* Enable Decoder clocks */
  dec_svd_clk_enable();
  dec_td_clk_enable();
#endif /* FEATURE_SRCH_CLKREGIM_RM */

#ifndef FEATURE_SRCH_CLKREGIM_RM
   /* Default decoder clk is CHIPX8 to reduce power consumption in Idle mode */
  dec_clk_select (DEC_CLK_CHIPX8);
#endif /* !FEATURE_SRCH_CLKREGIM_RM */

  SET_TST_SEL_DINT (0x0000);
#ifdef FEATURE_DEINT_SIMULATION
#error code not present
#endif 

  /* Initialize the task list */
  SET_DINT_TASK_OFFSET (DINT_FIRST_5MS_SLOT_ADDR);
  HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);

  SET_DINT_TASK_OFFSET (DINT_SECOND_5MS_SLOT_ADDR);
  HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);

  SET_DINT_TASK_OFFSET (DINT_THIRD_5MS_SLOT_ADDR);
  HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);

  SET_DINT_TASK_OFFSET (DINT_FOURTH_5MS_SLOT_ADDR);
  HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);

  dec_init();
  deint_allocate_CH0_RAM();

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  /* Move this code from function deint_allocate_CH0_RAM to here.  This
  ** code is necessary because in default case, RAM M, N, P will be 
  ** allocated for CH0.  So DSP will output symbols to CH0, which goes 
  ** to all RAM L, M, N, P and take up extra hardware cycles.
  **
  ** It's safest to execute this code once in the initialization state
  ** since VP2 allows FCH and DCCH to be assigned to either CH0 or CH1.
  */
  HWIO_OUTM(DINT_CFG, DINT_MRAM_CHAN_ALLOC_MASK, DINT_CH1_RAM_M_V); 
  HWIO_OUTM(DINT_CFG, DINT_NRAM_CHAN_ALLOC_MASK, DINT_CH1_RAM_N_V);
  HWIO_OUTM(DINT_CFG, DINT_PRAM_CHAN_ALLOC_MASK, DINT_CH1_RAM_P_V);
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */  

#ifdef FEATURE_SRCH_CLKREGIM_RM
  /* Disable Decoder clocks */
  dec_td_clk_disable();
  dec_svd_clk_disable();
#endif /* FEATURE_SRCH_CLKREGIM_RM */
}

/*===========================================================================

FUNCTION DEINT_ALLOCATE_CH0_RAM

DESCRIPTION 
  F-FCH (or F-DCCH, depending on which channel is used for power control), 
  SYNC and Paging channel symbols are received on Channel 0. 
  This funcntion programs the RAM usage for channel 0.
  We allocate (static) Channel 0 to RAM L. 
  Size of RAM L is 768*6 bits. So RAM L can hold symbols for SYNC, PAGING and 
  F-FCH (or F-DCCH) Radio Config 1-5.
  NOTE**** This is a private function, it should be called only from deint_init()

DEPENDENCIES
  None 

RETURN VALUE
  None


SIDE EFFECTS
   The values programmes in the DINT_CFG resgister effect the deinterleaver 
   write control.
=============================================================================*/
void deint_allocate_CH0_RAM
(
  void
)
{

  /*  Program the deinterleaver memory management parameters */
  /* Select RAM L */
  HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,L_UPPER_RAM_SEL), 0x0000);
  HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,L_ODD_RAM_PAGE_SEL), 0x0000);

  /* Disable extended RAM */
  HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,L0_RAM_EXTEND), 0x0000);
  HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,M0_RAM_EXTEND), 0x0000);

  /* Allocate channel 0 to RAM L */
  HWIO_OUTM(DINT_CFG, DINT_LRAM_CHAN_ALLOC_MASK, DINT_CH0_RAM_L_V);

#ifndef FEATURE_IS2000_REL_A_SVD
  /* Since no strobe will be generated for CH1, allocate RAM's M, N and 
  ** P to CH1. 
  **
  ** Move this code to function deint_init for simultaneous voice and
  ** data support (see deint_init for explanation.)
  */
  HWIO_OUTM(DINT_CFG, DINT_MRAM_CHAN_ALLOC_MASK, DINT_CH1_RAM_M_V); 
  HWIO_OUTM(DINT_CFG, DINT_NRAM_CHAN_ALLOC_MASK, DINT_CH1_RAM_N_V);
  HWIO_OUTM(DINT_CFG, DINT_PRAM_CHAN_ALLOC_MASK, DINT_CH1_RAM_P_V);
#endif /* !FEATURE_IS2000_REL_A_SVD */
}


/*===========================================================================

FUNCTION DEINT_ALLOCATE_CH1_RAM_SCCH

DESCRIPTION
 SCCH symbols are received on Channel 1 from the DEMOD DSP. This function 
 programs the RAM usage for channel 1 for SCCH.
 CH1 is allocated to Ram N and RAM P.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The values programmes in the DINT_CFG resgister effect the deinterleaver
  write control.
============================================================================*/
static void deint_allocate_ch1_ram_scch
(
  void 
)
{
  /*  Program the deinterleaver memory management parameters */
  /* For SCCH RAM N and P are combined in the following way
        N (lower addr) + P (upper addr).      */

  HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,N_UPPER_RAM_SEL), 0x0000);
  HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,P_UPPER_RAM_SEL),
            HWIO_FMSK(DINT_CFG,P_UPPER_RAM_SEL));

  /* Allocate CH1 to RAM N and RAM P */
  HWIO_OUTM(DINT_CFG, DINT_NRAM_CHAN_ALLOC_MASK, DINT_CH1_RAM_N_V);
  HWIO_OUTM(DINT_CFG, DINT_PRAM_CHAN_ALLOC_MASK, DINT_CH1_RAM_P_V);


  /*  Since no strobe will be generated for CH2, allocate RAM's M to CH2. */

  HWIO_OUTM(DINT_CFG, DINT_MRAM_CHAN_ALLOC_MASK, DINT_CH2_RAM_M_V);

}


#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/*===========================================================================

FUNCTION DEINT_ALLOCATE_CH1_RAM

DESCRIPTION
  This function programs the RAM usage for channel 1 for either F-FCH or 
  F-DCCH.  Channel 1 is allocated to Ram M for F-FCH or F_DCCH.  In a
  simultaneous voice and data call where both channel 0 and channel 1 may be
  set up at the same time, the Driver software:
  
     - Assigns the channel which handles power control to Channel 0.
     - Assigns the other channel to Channel 1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The values programmes in the DINT_CFG resgister effect the deinterleaver
  write control.
============================================================================*/

void deint_allocate_ch1_ram
(
  void 
)
{
  /* Program the deinterleaver memory management parameters */
  /* Select RAM M */
  HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,M_UPPER_RAM_SEL), 0x0000);
  HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,M_ODD_RAM_PAGE_SEL), 0x0000);

  /* Disable extended RAM */
  HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,L0_RAM_EXTEND), 0x0000);
  HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,M0_RAM_EXTEND), 0x0000);

  /* Allocate channel 1 to RAM M */
  HWIO_OUTM(DINT_CFG, DINT_MRAM_CHAN_ALLOC_MASK, DINT_CH1_RAM_M_V);

} /* end deint_allocate_ch1_ram () */
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */


/*===========================================================================

FUNCTION DEINT_ALLOCATE_CH2_RAM_SCH

DESCRIPTION 
 SCH symbols are received on Channel 2. This function programs the RAM usage 
 for channel 2 for SCH. CH2 is allocated to Ram N and RAM P. 


DEPENDENCIES 
  None

RETURN VALUE
  None

SIDE EFFECTS
  The values programmes in the DINT_CFG resgister effect the deinterleaver 
  write control.
============================================================================*/
static void deint_allocate_ch2_ram_sch
(
  dec_coding_type dec_coding
  /* Indicates if the code type is convolutional or turbo */
)
{
   /*  Program the deinterleaver memory management parameters */
  if(dec_coding == DEC_CONVOLUTIONAL)
  {
    /* For Conv SCH RAM N and P are combined in the following way
     * N (lower addr) + P (upper addr). */

    HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,N_UPPER_RAM_SEL), 0x0000);
    HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,P_UPPER_RAM_SEL), 
              HWIO_FMSK(DINT_CFG,P_UPPER_RAM_SEL));
  }
#ifdef MSMHW_TURBO_CODING
  else 
  {
    /* Decoding Type = TURBO */
    HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,N_ODD_RAM_PAGE_SEL), 0x0000);
    HWIO_OUTM(DINT_CFG, HWIO_FMSK(DINT_CFG,P_ODD_RAM_PAGE_SEL),
                           HWIO_FMSK(DINT_CFG,P_ODD_RAM_PAGE_SEL)); 
  }
#endif /* MSMHW_TURBO_CODING */

  /* Allocate CH2 to RAM N and RAM P */
  HWIO_OUTM(DINT_CFG, DINT_NRAM_CHAN_ALLOC_MASK, DINT_CH2_RAM_N_V);
  HWIO_OUTM(DINT_CFG, DINT_PRAM_CHAN_ALLOC_MASK, DINT_CH2_RAM_P_V);


#ifndef FEATURE_IS2000_REL_A_SVD
  /* Since no strobe will be generated for CH1, allocate RAM's M to CH1.
  **
  ** Move this code to function deint_init for simultaneous voice and
  ** data support (see deint_init for explanation.)
  */
  HWIO_OUTM(DINT_CFG, DINT_MRAM_CHAN_ALLOC_MASK, DINT_CH1_RAM_M_V); 
#endif /* !FEATURE_IS2000_REL_A_SVD */

    
}

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION DEINT_ALLOCATE_RAM_NCCCH

DESCRIPTION 
 Map the deint RAM for both channel 1 and 2.  

DEPENDENCIES 
  None

RETURN VALUE
  None

SIDE EFFECTS
  The values programmes in the DINT_CFG resgister effect the deinterleaver 
  write control.
============================================================================*/
void deint_allocate_ram_nccch (void)
{

  /* Use unsigned int instead of 'word' to avoid shifting to clear upper 
   * bits of register.  Cast to 'word' upon completion. */
  unsigned int mask = 0; 
  unsigned int val = 0;

  /* For both BCCH/FCCCH, we set L RAM to CH1 and M RAM to CH2.
   * L0 and M0 RAM extend turned on.  */  

  mask |= HWIO_FMSK(DINT_CFG,L_UPPER_RAM_SEL) |
          HWIO_FMSK(DINT_CFG,L_ODD_RAM_PAGE_SEL) |
          HWIO_FMSK(DINT_CFG,M_UPPER_RAM_SEL) |
          HWIO_FMSK(DINT_CFG,M_ODD_RAM_PAGE_SEL) |
          HWIO_FMSK(DINT_CFG,N_UPPER_RAM_SEL) |
          HWIO_FMSK(DINT_CFG,N_ODD_RAM_PAGE_SEL) |
          HWIO_FMSK(DINT_CFG,P_UPPER_RAM_SEL) |
          HWIO_FMSK(DINT_CFG,P_ODD_RAM_PAGE_SEL) |
          HWIO_FMSK(DINT_CFG,L0_RAM_EXTEND) |
          HWIO_FMSK(DINT_CFG,M0_RAM_EXTEND) |
#ifdef FEATURE_BCCH_CH0_WORKAROUND
          DINT_LRAM_CHAN_ALLOC_MASK |
#endif
          DINT_LRAM_CHAN_ALLOC_MASK |
          DINT_MRAM_CHAN_ALLOC_MASK |
          DINT_NRAM_CHAN_ALLOC_MASK |
          DINT_PRAM_CHAN_ALLOC_MASK;

  val |= 
#ifdef FEATURE_BCCH_CH0_WORKAROUND
         DINT_CH1_RAM_L_V | 
         DINT_CH0_RAM_N_V | 
         DINT_CH0_RAM_P_V;
#else
         DINT_L0_RAM_EXTEND_ON |
         DINT_M0_RAM_EXTEND_ON |
         DINT_CH1_RAM_M_V |
         DINT_CH2_RAM_L_V;
#endif
  HWIO_OUTM(DINT_CFG, mask, val);
  MSG_LOW("Deint alloc RAM nccch: mask 0x%x, val:0x%x", mask, val, 0);
}
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION DEINT_SET_SC_MODE

DESCRIPTION
    This function calls dec_sc_init() which puts the decoder in continous mode 
    and resets the SVD.  This function loads the deinterleaver packet parameters
    for SYNC Channel. 

DEPENDENCIES
    deint_init() should have been called

RETURN VALUE
  None

SIDE EFFECTS
   The deinterleaver packet parameters for SYNC Channel are loaded in PACKET
   RAM.
=============================================================================*/
void deint_set_sc_mode
(
  void
)
{

  dec_sc_init();

  deint_allocate_CH0_RAM();
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  /* Avoid resetting decoder while in traffic state.
  */
  curr_deint_state = DEINT_SYNC_STATE;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

  SET_DINT_CH_CFG (0, DINT_CH0_CFG__SYNC_CHAN_80M_BND_MASK
                      | DINT_CH0_CFG__NEXT_20M_BND_MASK,
            DINT_SYNC_CHAN_80M_BND_V);

  SET_DINT_CH_CFG (0, DINT_CH0_CFG__NUM_SUB_CHANS_MASK, 0x0000);

  HWIO_OUT(DINT_OTD_CFG, 0x0000);   /* NO OTD */
   
  SET_DINT_CH_CFG (1, DINT_CH1_CFG__NEXT_20M_BND_MASK, 0x00);
  SET_DINT_CH_CFG (2, DINT_CH2_CFG__NEXT_20M_BND_MASK, 0x00);

  /* The DEINT_CFG_CH3 init is added since MSM6700 
     if not init, then the 26.666 ms DEc interrupt is observed */ 
  SET_DINT_CH_CFG (3, DINT_CH3_CFG__NEXT_20M_BND_MASK, 0x00);

  deint_load_sc_pkt_params();
  deint_create_sc_task_list();

}

/*==========================================================================
FUNCTION DEINT_SET_PC_MODE

DESCRIPTION
  This function calls dec_pc_init() which puts the decoder in continous
  mode and resets the SVD. This function loads the deinterleaver packet 
  parameters for PAGING Channel.

DEPENDENCIES
 deint_init() should have been called. 

RETURN VALUE
  None

SIDE EFFECTS
   The deinterleaver packet parameters for PAGING Channel are loaded in PACKET
   RAM. 
=============================================================================*/
void deint_set_pc_mode
(
  deint_pc_rate_type pc_rate
  /* Indicates the Paging Channel Rate : 9600bps / 4800bps */
)
{
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  /* Avoid resetting decoder while in traffic state.
  */
  curr_deint_state = DEINT_PAGING_IDLE_STATE;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

  dec_pc_init((dec_pc_rate_type)pc_rate);
  
  /* Clear The SYNC_CHAN_80_M_BND for Paging Channel */ 
  SET_DINT_CH_CFG (0, DINT_CH0_CFG__NEXT_20M_BND_MASK | 
                      DINT_CH0_CFG__SYNC_CHAN_80M_BND_MASK, 
             DINT_NEXT_20M_BND_V);

  SET_DINT_CH_CFG (0, DINT_CH0_CFG__NUM_SUB_CHANS_MASK, 0x0000);

  HWIO_OUT(DINT_OTD_CFG, 0x0000);   /* NO OTD  1 BANK INTERLEAVING */
   
  deint_load_pc_pkt_params(pc_rate);
  deint_create_pc_task_list();
}

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION DEINT_CONFIG_20MS_FRAME_BOUNDARY

DESCRIPTION
    This function specifies frame boundary timing for the deinterleaver/
    decoder.  If frame_boundary == TRUE, the next sync20 will denote a frame
    boundary for the specified channel.

DEPENDENCIES
    None.

RETURN VALUE
  None

SIDE EFFECTS
   None.
   
=============================================================================*/
boolean 
deint_config_20ms_frame_boundary 
(
  deint_ch_id_type ch_id, 
  boolean frame_boundary
)
{
  unsigned int val = (frame_boundary) ? DINT_NEXT_20M_BND_V : 0;

  switch (ch_id) {
    case DEINT_CH0:
      SET_DINT_CH_CFG (0, DINT_CH0_CFG__NEXT_20M_BND_MASK, val);
      break;
    case DEINT_CH1:
      SET_DINT_CH_CFG (1, DINT_CH1_CFG__NEXT_20M_BND_MASK, val);
      break;
    case DEINT_CH2:
      SET_DINT_CH_CFG (2, DINT_CH2_CFG__NEXT_20M_BND_MASK, val);
      break;
    default:
      return FALSE;
  }

  return TRUE;
}

/*==========================================================================
FUNCTION DEINT_SET_BCCH_MODE

DESCRIPTION
  This function puts the decoder in packet mode and resets the SVD. 
  This function loads the deinterleaver packet parameters for the 
  BCCH.

DEPENDENCIES
  deint_init() should have been called. 

RETURN VALUE
  None

SIDE EFFECTS
   None.
   
=============================================================================*/
boolean deint_set_bcch_mode 
(
  cai_bcch_rate_type data_rate,
  cai_code_rate_type code_rate,
  deint_ch_id_type   chan_id,
  byte               ncc_actchan_mask
)
{

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  /* Avoid resetting decoder while in traffic state.
  */
  curr_deint_state = DEINT_PAGING_IDLE_STATE;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

  /* Deallocate CH0 */
  SET_DINT_CH_CFG (0, 0xFFFF, 0);
   
  deint_bcch_seq_repetition(CAI_BCCH_19200_RATE,chan_id);
  /* The OTD_SEL is adversely named.  It really stands for 2-bank
   * interleaving, which is used by all IS2000 channels.*/
  HWIO_OUTM(DINT_OTD_CFG, HWIO_FMSK(DINT_OTD_CFG,OTD_SEL),
            HWIO_FMSK(DINT_OTD_CFG,OTD_SEL));   
  
  deint_load_bcch_pkt_params (code_rate);
  deint_create_bcch_task_list (chan_id,ncc_actchan_mask);
 

  return TRUE;
}

/*==========================================================================
FUNCTION  DEINT_BCCH_SEQ_REPETITION

DESCRIPTION
  This function sets DEINT_CH2_CFG:SEQ_REP field based on the BCCH data rate.
  19200 -> sequence repetition factor of 1 (no repetition)
   9600 -> sequence repetition factor of 2
   4800 -> sequence repetition factor of 4

DEPENDENCIES
  deint_init() should have been called. 

RETURN VALUE
  None

SIDE EFFECTS
   None.
   
=============================================================================*/
void deint_bcch_seq_repetition(cai_bcch_rate_type data_rate, deint_ch_id_type chan_id)
{
  deint_seq_rep_type seq_rep = DEINT_SEQ_REP_COUNT;

   /* Settings for DINT_CH2_CFG register */
  switch (data_rate) {
    case CAI_BCCH_19200_RATE:
      seq_rep = DEINT_SEQ_REP_1X;
      break;
    case CAI_BCCH_9600_RATE:
      seq_rep = DEINT_SEQ_REP_2X;
      break;
    case CAI_BCCH_4800_RATE:
      seq_rep = DEINT_SEQ_REP_4X;
      break;
    default:
      MSG_LOW("Bad fr len", 0, 0, 0);
      return;
  }

  if(chan_id == DEINT_CH1)
  {
    SET_DINT_CH_CFG (1, DINT_CH1_CFG_MASK, DEINT_CH_CFG_SEQ_REP(seq_rep));
  }
  else if(chan_id == DEINT_CH2)
  {
    SET_DINT_CH_CFG (2, DINT_CH2_CFG_MASK, DEINT_CH_CFG_SEQ_REP(seq_rep));
  }
  else
  {
    SET_DINT_CH_CFG (0, DINT_CH0_CFG_MASK, DEINT_CH_CFG_SEQ_REP(seq_rep));
  }

}
#endif /* FEATURE_IS2000_REL_A_CC */

/*==========================================================================
FUNCTION DEINT_SET_FCH_MODE

DESCRIPTION
    This function calls dec_tc_init() which puts the decoder in packet
    mode and resets the SVD. This function loads the deinterleaver packet
    parameters for FCH of a given Radio Configuration.

DPENEDENCIES
    deint_init should have been called.

RETURN VALUE
  None

SIDE EFFECTS
   

=============================================================================*/
#ifndef FEATURE_IS2000_REL_A_SVD
void deint_set_fch_mode
(
  deint_rc_type fch_rc
)
{
  deint_sequencing_cmd_type *cmd_ptr;  /* Sequencing Command Ptr */

   dec_tc_init();

   deint_allocate_CH0_RAM ();

   SET_DINT_CH_CFG (0, DINT_CH0_CFG__NEXT_20M_BND_MASK, 
             DINT_NEXT_20M_BND_V);

   SET_DINT_CH_CFG (0, DINT_CH0_CFG__NUM_SUB_CHANS_MASK, 0x0000);

   if(fch_rc >= DEINT_RC3)
   {
      HWIO_OUTM(DINT_OTD_CFG, HWIO_FMSK(DINT_OTD_CFG,OTD_SEL),
                 HWIO_FMSK(DINT_OTD_CFG,OTD_SEL));   
      /* IS95C  2 Bank deinterleaving; NO OTD */
   }
   else
   {
      HWIO_OUTM(DINT_OTD_CFG,HWIO_FMSK(DINT_OTD_CFG,OTD_SEL), 0x0000);   
      /* IS95A 1 Bank deinterleaving ; NO OTD */
      
   }
  if((cmd_ptr = (deint_sequencing_cmd_type *)
                q_get(&deint_sequencing_free_q)) == NULL)
  {
    MSG_ERROR("0 Bufs on seq_free_q", 0, 0, 0);
    /* Just incase if this happens, program the read parameters here */
    deint_load_fch_pkt_params(fch_rc);
    deint_create_fch_task_list(fch_rc, TRUE);
  }
  else
  {
     cmd_ptr->hdr.command = DEINT_FCH_READ_PARAMS;
     cmd_ptr->fch_cmd.fch_rc = fch_rc;
     q_put(&deint_sequencing_cmd_q, &cmd_ptr->hdr.link);
  }

}

#else
void deint_set_fch_mode
(
  deint_rc_type fch_rc,
  boolean power_control_channel
)
{
  deint_sequencing_cmd_type *cmd_ptr;  /* Sequencing Command Ptr */

  if (curr_deint_state != DEINT_TRAFFIC_STATE)
  {
    /* Only perform these init operations at the start of traffic state.
    */
    dec_tc_init();
    curr_deint_state = DEINT_TRAFFIC_STATE;
    curr_deint_ch_active = DEINT_CH_INACTIVE;
  }

  /* Update configuration of fundicated channels FCH and DCCH.
  */
  if (curr_deint_ch_active == DEINT_CH_INACTIVE)
  {
    curr_deint_ch_active = DEINT_FCH_ACTIVE;
    MSG_HIGH ("Set_fch_mode,new=FCH,fch_rc=%d",fch_rc,0,0);
  }
  else if (curr_deint_ch_active == DEINT_DCCH_ACTIVE)
  {
    curr_deint_ch_active = DEINT_FCH_DCCH_ACTIVE;
    MSG_HIGH ("Set_fch_mode,new=FCH_DCCH,fch_rc=%d",fch_rc,0,0);
  }
  else if (curr_deint_ch_active == DEINT_FCH_ACTIVE)
  {
    MSG_HIGH ("Set_fch_mode called in FCH mode,fch_rc=%d",fch_rc,0,0);
  }
  else if (curr_deint_ch_active == DEINT_FCH_DCCH_ACTIVE)
  {
    MSG_HIGH ("Set_fch_mode called in FCH_DCCH mode,fch_rc=%d",fch_rc,0,0);
  }
  else
  {
    ERR ("Bad val curr_deint_ch_act: %d", curr_deint_ch_active,0,0);
  }

  /* Must use channel 0 for power control channel. */
  if (power_control_channel)
  {
    deint_allocate_CH0_RAM ();

    SET_DINT_CH_CFG (0, DINT_CH0_CFG__NEXT_20M_BND_MASK, 
                     DINT_NEXT_20M_BND_V);
   
    SET_DINT_CH_CFG (0, DINT_CH0_CFG__NUM_SUB_CHANS_MASK, 0x0000);

  }
  else
  {
    deint_allocate_ch1_ram (); 

    SET_DINT_CH_CFG (1, DINT_CH1_CFG__NEXT_20M_BND_MASK,
                     DINT_NEXT_20M_BND_V);

    SET_DINT_CH_CFG (1, DINT_CH1_CFG__NUM_SUB_CHANS_MASK, 0x0000);
  
  } /* end if (power_control_channel) */

  if(fch_rc >= DEINT_RC3)
  {
    HWIO_OUTM(DINT_OTD_CFG, HWIO_FMSK(DINT_OTD_CFG,OTD_SEL),
              HWIO_FMSK(DINT_OTD_CFG,OTD_SEL));   
    /* IS95C  2 Bank deinterleaving; NO OTD */
  }
  else
  {
    HWIO_OUTM(DINT_OTD_CFG,HWIO_FMSK(DINT_OTD_CFG,OTD_SEL), 0x0000);   
    /* IS95A 1 Bank deinterleaving ; NO OTD */
  }

  if((cmd_ptr = (deint_sequencing_cmd_type *)
                q_get(&deint_sequencing_free_q)) == NULL)
  {
    MSG_ERROR("0 Bufs on seq_free_q", 0, 0, 0);
    

    /* Just incase if this happens, program the read parameters here */
    (void) deint_load_fch_pkt_params(fch_rc);
    deint_create_fch_task_list (fch_rc, 
                                power_control_channel
                               );
  }
  else
  {
    cmd_ptr->hdr.command = DEINT_FCH_READ_PARAMS;
    cmd_ptr->fch_cmd.fch_rc = fch_rc;
    cmd_ptr->fch_cmd.fch_pc = power_control_channel;
    q_put(&deint_sequencing_cmd_q, &cmd_ptr->hdr.link);
  }
} /* end deint_set_fch_mode */

/*==========================================================================
FUNCTION DEINT_SET_DCCH_MODE

DESCRIPTION
  This function calls dec_tc_init() which puts the decoder in packet
  mode and resets the SVD (Serial Viterbi decoder.) This function loads the 
  deinterleaver packet parameters for DCCH of a given Radio Configuration.

DEPENDENCIES
  deint_init should have been called.

RETURN VALUE
  None

SIDE EFFECTS
  Reset decoders.
   

=============================================================================*/

void deint_set_dcch_mode
(
  deint_rc_type dcch_rc,
  boolean power_control_channel
)
{
  deint_sequencing_cmd_type *cmd_ptr;  /* Sequencing Command Ptr */

  if (curr_deint_state != DEINT_TRAFFIC_STATE)
  {
    /* Only perform these init operations at the start of traffic state.
    */
    dec_tc_init();
    curr_deint_state = DEINT_TRAFFIC_STATE;
    curr_deint_ch_active = DEINT_CH_INACTIVE;
  }
  
  /* Update configuration of fundicated channels FCH and DCCH.
  */
  if (curr_deint_ch_active == DEINT_CH_INACTIVE)
  {
    curr_deint_ch_active = DEINT_DCCH_ACTIVE;
    MSG_HIGH ("Set_dcch_mode,new=DCCH,dcch_rc=%d",dcch_rc,0,0);
  }
  else if (curr_deint_ch_active == DEINT_FCH_ACTIVE)
  {
    curr_deint_ch_active = DEINT_FCH_DCCH_ACTIVE;
    MSG_HIGH ("Set_dcch_mode,new=FCH_DCCH,dcch_rc=%d",dcch_rc,0,0);
  }
  else if (curr_deint_ch_active == DEINT_DCCH_ACTIVE)
  {
    MSG_HIGH ("Set_dcch_mode called in DCCH mode,dcch_rc=%d",dcch_rc,0,0);
  }
  else if (curr_deint_ch_active == DEINT_FCH_DCCH_ACTIVE)
  {
    MSG_HIGH ("Set_dcch_mode called in FCH_DCCH mode,dcch_rc=%d",dcch_rc,0,0);
  }
  else
  {
    ERR ("Bad val deint_ch_act: %d", curr_deint_ch_active,0,0);
  }

  /* Must use channel 0 for power control channel. */
  if (power_control_channel)
  {
    deint_allocate_CH0_RAM ();

    SET_DINT_CH_CFG (0, DINT_CH0_CFG__NEXT_20M_BND_MASK, DINT_NEXT_20M_BND_V);
   
    SET_DINT_CH_CFG (0, DINT_CH0_CFG__NUM_SUB_CHANS_MASK, 0x0000);
   
  }
  else
  {
    deint_allocate_ch1_ram (); 

    SET_DINT_CH_CFG (1, DINT_CH1_CFG__NEXT_20M_BND_MASK,
                     DINT_CH1_CFG__NEXT_20M_BND_MASK);

    SET_DINT_CH_CFG (1, DINT_CH1_CFG__NUM_SUB_CHANS_MASK, 0x0000);

  } /* end if (power_control_channel) */

  if(dcch_rc >= DEINT_RC3)
  {
    /* IS95C  2 Bank deinterleaving; NO OTD */
    HWIO_OUTM (DINT_OTD_CFG, HWIO_FMSK(DINT_OTD_CFG,OTD_SEL),
              HWIO_FMSK(DINT_OTD_CFG,OTD_SEL));   
  }
  else
  {
    /* IS95A 1 Bank deinterleaving ; NO OTD */
    HWIO_OUTM (DINT_OTD_CFG,HWIO_FMSK(DINT_OTD_CFG,OTD_SEL), 0x0000);   
  }

  /* FOR DCCH decoding, use deint_load_dcch_pkt_params()) to load packet
  ** parameters (need to use a different packet to co-exist with FCH.)
  */
  if((cmd_ptr = (deint_sequencing_cmd_type *)
                q_get(&deint_sequencing_free_q)) == NULL)
  {
    MSG_ERROR("0 Bufs on seq_free_q", 0, 0, 0);

    /* Just incase if this happens, program the read parameters here */
    deint_load_dcch_pkt_params(dcch_rc);
    deint_create_dcch_task_list(dcch_rc, power_control_channel);
  }
  else
  {
    cmd_ptr->hdr.command = DEINT_DCCH_READ_PARAMS;
    cmd_ptr->dcch_cmd.dcch_rc = dcch_rc;
    cmd_ptr->dcch_cmd.dcch_pc = power_control_channel;
    q_put(&deint_sequencing_cmd_q, &cmd_ptr->hdr.link);
  }
} /* end deint_set_dcch_mode () */
#endif /* !FEATURE_IS2000_REL_A_SVD */
    

/*==========================================================================
FUNCTION DEINT_SET_SCH_MODE

DESCRIPTION
    This function will initialize the deinterleaver for Supplemental Channels.
    This function loads the deinterleaver packet
    parameters for SCH of a given Radio Configuration.

DPENEDENCIES
    deint_init should have been called.

RETURN VALUE
  None

SIDE EFFECTS
   

=============================================================================*/
void deint_set_sch_mode
(

#ifdef FEATURE_IS2000_REL_A //32X
  uint8   num_ltu,          
  uint16  ltu_len,          
  dec_mux_pdu_type_type pdu_type,
#endif /* FEATURE_IS2000_REL_A */

  deint_rc_type sch_rc, 
  dec_coding_type dec_coding, 
  dec_sch_rate_type dec_sch_rate
#ifdef FEATURE_PLT 
#error code not present
#endif  /* FEATURE_PLT */ 
)
{
  deint_sequencing_cmd_type *cmd_ptr;  /* Sequencing Command Ptr */ 
  deint_allocate_ch2_ram_sch(dec_coding);   // Configure CH2 to use RAM N & P.

#ifdef FEATURE_MULTIFRAME_INTERLEAVING
#error code not present
#else

     SET_DINT_CH_CFG (2, DINT_CH2_CFG__NEXT_20M_BND_MASK, 
                      DINT_CH2_CFG__NEXT_20M_BND_MASK);
#endif  /* FEATURE_MULTIFRAME_INTERLEAVING */    

   SET_DINT_CH_CFG (2, DINT_CH2_CFG__NUM_SUB_CHANS_MASK,0x0000);

   if(dec_coding == DEC_CONVOLUTIONAL)
   {
     HWIO_OUTM(DINT_OTD_CFG,HWIO_FMSK(DINT_OTD_CFG,CH2_CODE_TYPE), 0x0000);   

     if((cmd_ptr = (deint_sequencing_cmd_type *)
              q_get(&deint_sequencing_free_q)) == NULL)
     {
       MSG_ERROR("0 Bufs on seq_free_q", 0, 0, 0);
       /* Just incase if this happens, program the read parameters here */
     if(dec_sch_rate != DEC_SCH_RATE_1X)
     {
       /* Load new packet parameters only if SCH Rate is not 1X.
          Packet parameters for 1X SCH are the same as for full
          rate FCH (Packet A). */

#ifdef FEATURE_IS2000_REL_A //32X
       deint_load_sch_pkt_params(num_ltu, ltu_len, sch_rc, dec_sch_rate);
#else
       deint_load_sch_pkt_params(sch_rc, dec_sch_rate);
#endif /* FEATURE_IS2000_REL_A */
     }
#ifdef FEATURE_IS2000_REL_A //32X
     deint_create_sch_task_list(pdu_type, sch_rc, dec_sch_rate);
#else
     deint_create_sch_task_list(sch_rc, dec_sch_rate);
#endif /* FEATURE_IS2000_REL_A */
     }
     else
     {
       cmd_ptr->hdr.command = DEINT_SCH_READ_PARAMS;
       cmd_ptr->sch_cmd.sch_rc = sch_rc;
       cmd_ptr->sch_cmd.dec_sch_rate = dec_sch_rate;
       cmd_ptr->sch_cmd.dec_coding = DEC_CONVOLUTIONAL;
#ifdef FEATURE_IS2000_REL_A //32X
       cmd_ptr->sch_cmd.ltu_len = ltu_len;
       cmd_ptr->sch_cmd.num_ltu = num_ltu;
       cmd_ptr->sch_cmd.pdu_type = pdu_type;
#endif /* FEATURE_IS2000_REL_A */
       q_put(&deint_sequencing_cmd_q, &cmd_ptr->hdr.link);
     }
   }
#ifdef MSMHW_TURBO_CODING
   else
   {

     /* Turbo Coding */
     HWIO_OUTM(DINT_OTD_CFG, HWIO_FMSK(DINT_OTD_CFG,CH2_CODE_TYPE),
      HWIO_FMSK(DINT_OTD_CFG,CH2_CODE_TYPE));   

      /* Turbo Decoding */
        dec_sch_rate = (dec_sch_rate_type)(dec_sch_rate - DEC_SCH_RATE_2X);
        deint_load_td_deint_params(sch_rc, dec_sch_rate);

        /* We have told TD not to generate a intterupt after SCH frame is
           decoded, so we create a dummy task in the second 5ms slot that
           will generate an interrupt */    
       if((cmd_ptr = (deint_sequencing_cmd_type *)
            q_get(&deint_sequencing_free_q)) == NULL)
       {
         MSG_ERROR("0 Bufs on seq_free_q", 0, 0, 0);
         deint_load_td_dec_params(sch_rc, dec_sch_rate);
         deint_create_dummy_sch_task_list();
       }
       else
       {
         cmd_ptr->hdr.command = DEINT_SCH_READ_PARAMS;
         cmd_ptr->sch_cmd.sch_rc = sch_rc;
         cmd_ptr->sch_cmd.dec_sch_rate = dec_sch_rate;
         cmd_ptr->sch_cmd.dec_coding = DEC_TURBO;
         q_put(&deint_sequencing_cmd_q, &cmd_ptr->hdr.link);
       }
       
   }  
#endif /* MSMHW_TURBO_CODING */
}

/*==========================================================================
FUNCTION DEINT_SET_SCCH_MODE

DESCRIPTION
    This function will initialize the deinterleaver for Supplemental Channels.
    This function loads the deinterleaver packet
    parameters for SCCH of a given Radio Configuration.

DPENEDENCIES
    deint_init should have been called.

RETURN VALUE
  None

SIDE EFFECTS


=============================================================================*/

void deint_set_scch_mode
(
  deint_rc_type scch_rc, 
  byte num_sup
)
{
  deint_sequencing_cmd_type *cmd_ptr;
   deint_allocate_ch1_ram_scch();   // Configure CH1 to use RAM N & P.

   SET_DINT_CH_CFG (1, DINT_CH1_CFG__NEXT_20M_BND_MASK,
                    DINT_CH1_CFG__NEXT_20M_BND_MASK);

   SET_DINT_CH_CFG (1, DINT_CH1_CFG__NUM_SUB_CHANS_MASK, (num_sup - 1));

   HWIO_OUTM(DINT_OTD_CFG,HWIO_FMSK(DINT_OTD_CFG,CH2_CODE_TYPE), 0x0000);   
  /* FOR SCCH decoding, the packet parameters that are programmed in PACKET
     A for F_FCH can be used for SCCH */

  if((cmd_ptr = (deint_sequencing_cmd_type *)
               q_get(&deint_sequencing_free_q)) == NULL)
  {
    MSG_ERROR("0 Bufs on seq_free_q", 0, 0, 0);

   /* Just incase if this happens, program the read parameters here */
    deint_create_scch_task_list(scch_rc);
  }
  else
  {
    cmd_ptr->hdr.command = DEINT_SCCH_READ_PARAMS;
    cmd_ptr->scch_cmd.scch_rc = scch_rc;
    q_put(&deint_sequencing_cmd_q, &cmd_ptr->hdr.link);
  }
}

/*==========================================================================
FUNCTION DEINT_LOAD_PKT_PARAMS

DESCRIPTION
  This function loads deinterleaver packet parameters.

DEPENDENCIES
   This routine assumes DINT_PKT_OFFSET_WH has been set to assure the
   correct packet RAM is being written.

RETURN VALUE
  None

SIDE EFFECTS
  None
 
==========================================================================*/
static void 
deint_load_pkt_params (const deint_pkt_params_type *pkt_params)
{
  /* Note:::: write to DINT_PKT_CFG register causes the address pinter in 
   * CFG_RAM to autoincrement by 16 bits at a time
   * So each packet configuration will be selected automatically */
  
  /* PKT_INTLV_CFG_LO */
  HWIO_OUT( DINT_PKT_CFG, pkt_params->pkt_intlv_cfg_lo);
  
  /* PKT_INTLV_PKT_HI */
  HWIO_OUT( DINT_PKT_CFG, pkt_params->pkt_intlv_cfg_hi);
  
  /* PKT_BLK_SIZE_LO */
  HWIO_OUT( DINT_PKT_CFG, pkt_params->pkt_blk_size_lo);
  
  /* PKT_BLK_SIZE_HI */
  HWIO_OUT( DINT_PKT_CFG, pkt_params->pkt_blk_size_hi);
  
  /* PKT_PUNCT_LO  */
  HWIO_OUT( DINT_PKT_CFG, pkt_params->pkt_punct_lo);
  
  /* PKT_PUNCT_HI */
  HWIO_OUT( DINT_PKT_CFG, pkt_params->pkt_punct_hi);
  
  /* PKT_PL_CRC_LO */
  HWIO_OUT( DINT_PKT_CFG, pkt_params->pkt_pl_crc_lo);
  
  /* PKT_PL_CRC_HI */
  HWIO_OUT( DINT_PKT_CFG, pkt_params->pkt_pl_crc_hi);
  
  MSG_LOW("pkt params (cfg)   0x%x 0x%x", pkt_params->pkt_intlv_cfg_lo, pkt_params->pkt_intlv_cfg_hi, 0);
  MSG_LOW("pkt params (blk)   0x%x 0x%x", pkt_params->pkt_blk_size_lo, pkt_params->pkt_blk_size_hi, 0);
  MSG_LOW("pkt params (punct) 0x%x 0x%x", pkt_params->pkt_punct_lo, pkt_params->pkt_punct_hi, 0);
  MSG_LOW("pkt params (crc)   0x%x 0x%x", pkt_params->pkt_pl_crc_lo, pkt_params->pkt_pl_crc_hi, 0);
}

/*==========================================================================
FUNCTION DEINT_LOAD_SC_PKT_PARAMS

DESCRIPTION
  This function loads the deinterleaver packet parameters for SYNC Channel.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
==========================================================================*/
static void deint_load_sc_pkt_params
(
  void
)
{
   /* Select Packet A to program parameters for SYNC Channel Message */

   SET_DINT_PKT_OFFSET (DINT_PKT_A_V | DINT_PKT_INTLV_CFG_LO_V);

  /* Note:::: write to DINT_PKT_CFG register causes the address pinter in 
     CFG_RAM to autoincrement by 16 bits at a time
    So each packet configuration will be selected automatically */

   /* Program for Packet RAM A */
   deint_load_pkt_params (&deint_sc_pkt_params.sc_param);
}
   
/*===========================================================================
FUNCTION DEINT_LOAD_PC_PKT_PARAMS

DESCRIPTION
  This routine loads packet parameters used to configure CH0 for IS95 PCH
  operation.
   
DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
static void deint_load_pc_pkt_params
(
  deint_pc_rate_type pc_rate
)
{
  ASSERT (pc_rate < PRAT_COUNT);
  
  SET_DINT_PKT_OFFSET (DINT_PKT_A_V | DINT_PKT_INTLV_CFG_LO_V);
  
  deint_load_pkt_params(&deint_pc_pkt_params.pc_params[pc_rate]);

}   

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================
FUNCTION DEINT_LOAD_BCCH_PKT_PARAMS

DESCRIPTION
  This routine loads packet parameters used to configure CH2 for BCCH
  operation.
   
DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
static void 
deint_load_bcch_pkt_params (cai_code_rate_type code_rate)
{
  SET_DINT_PKT_OFFSET (DINT_PKT_A_V | DINT_PKT_INTLV_CFG_LO_V);

  deint_load_pkt_params (&deint_bcch_pkt_params.bcch_params[
                         DEINT_UPPER_BOUNDS_CHECK(code_rate,CAI_CODE_RATE_COUNT)]);
}   
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================
FUNCTION DEINT_LOAD_FCH_PKT_PARAMS

DESCRIPTION
  This function programs 4 packets corresponding to each RC for FCH. 
  The four packets correspond to FULL RATE, HALF RATE,QUARTER RATE & 8TH 
  HYPOTHESIS.
  The values programmed into the registers take effect at the next SYNC20.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None
===========================================================================*/
static boolean deint_load_fch_pkt_params
(
  deint_rc_type fch_rc
)
{
  

  if(fch_rc > DEINT_RC5)
  {
    return FALSE;
  }
   

  /* Note:::: write to DINT_PKT_CFG register causes the address pointer in 
   CFG_RAM to autoincrement by 16 bits at a time
   So each packet configuration will be selected automatically */

  /* Select Packet A to program parameters for FULL Rate Hypothesis */

  SET_DINT_PKT_OFFSET (DINT_PKT_A_V | DINT_PKT_INTLV_CFG_LO_V);

 /**********************PROGRAMMING FULL RATE HYPOTHESIS *****************/

  deint_load_pkt_params (&deint_fch_packet_params[DEINT_FCH_RC_INDEX(fch_rc)].
            fch_full_rate_param);
 
 /**********************PROGRAMMING HALF RATE HYPOTHESIS **********************/

  /* Select Packet B to program parameters for HALF Rate Hypothesis */

  // SET_DINT_PKT_OFFSET (DINT_PKT_B_V | DINT_PKT_INTLV_CFG_LO_V);
         
  deint_load_pkt_params (&deint_fch_packet_params[DEINT_FCH_RC_INDEX(fch_rc)].
            fch_half_rate_param);

  /**********************PROGRAMMING QUARTER RATE HYPOTHESIS *********************/

  /* Select Packet C to program parameters for QUARTER Rate Hypothesis */

  // SET_DINT_PKT_OFFSET (DINT_PKT_C_V | DINT_PKT_INTLV_CFG_LO_V);

  deint_load_pkt_params (&deint_fch_packet_params[DEINT_FCH_RC_INDEX(fch_rc)].
             fch_quarter_rate_param);

   /********************PROGRAMMING EIGHTH RATE HYPOTHESIS *****************/

  /* Select Packet D to program parameters for EIGHTH Rate Hypothesis */

  // SET_DINT_PKT_OFFSET (DINT_PKT_D_V | DINT_PKT_INTLV_CFG_LO_V);
         
  deint_load_pkt_params (&deint_fch_packet_params[DEINT_FCH_RC_INDEX(fch_rc)].
            fch_eighth_rate_param);


#ifdef FEATURE_PLT 
#error code not present
#endif /* FEATURE_PLT  */

  return TRUE;
}

#ifdef MSMHW_TURBO_CODING
/*==========================================================================
FUNCTION DEINT_LOAD_TD_DEINT_PARAMS

DESCRIPTION
  This function loads the deinterleaver packet parameters for Turbo Encoded
  Supplemental Channel.
  The values programmed into the registers take effect at the next SYNC20.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void deint_load_td_deint_params
(
  deint_rc_type sch_rc,
  dec_sch_rate_type dec_sch_rate
)
{
  MSG_HIGH("LOADING TD DEINT,rc=%d,r_off=%d", sch_rc,dec_sch_rate,0);
  /* Program The Write Paramters for TD. These Parameters affect the
     deinterleaving operation for Turbo SCH Frames */
  HWIO_OUT(TD_INTLV_CFG_LO,
       deint_td_sch_params[DEINT_SCH_RC_INDEX(sch_rc)]
           [DEINT_UPPER_BOUNDS_CHECK(dec_sch_rate, NUM_TD_SCH_RATES)].
       td_intlv_cfg_lo);
  HWIO_OUT(TD_INTLV_CFG_HI,
       deint_td_sch_params[DEINT_SCH_RC_INDEX(sch_rc)]
           [DEINT_UPPER_BOUNDS_CHECK(dec_sch_rate, NUM_TD_SCH_RATES)].
       td_intlv_cfg_hi);
}

/*==========================================================================
FUNCTION DEINT_LOAD_TD_DEC_PARAMS

DESCRIPTION
  This function loads the decoder packet parameters for Turbo Encoded
  Supplemental Channel.
  The values programmed into the registers take effect at the next SYNC20.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void deint_load_td_dec_params
(
  deint_rc_type sch_rc,
  dec_sch_rate_type dec_sch_rate
)
{
  MSG_HIGH("LOADING TD DEC,rc=%d,r_off=%d", sch_rc,dec_sch_rate,0);
  HWIO_OUT(TD_BLOCK_SIZE,
          deint_td_sch_params[DEINT_SCH_RC_INDEX(sch_rc)]
           [DEINT_UPPER_BOUNDS_CHECK(dec_sch_rate, NUM_TD_SCH_RATES)].
          td_block_size);
  HWIO_OUT(TD_MIN_LLR_THRESH, 
              deint_td_sch_params[DEINT_SCH_RC_INDEX(sch_rc)]
           [DEINT_UPPER_BOUNDS_CHECK(dec_sch_rate, NUM_TD_SCH_RATES)].
              td_min_llr);
  HWIO_OUT(TD_PUNCT_LO,
           deint_td_sch_params[DEINT_SCH_RC_INDEX(sch_rc)]
           [DEINT_UPPER_BOUNDS_CHECK(dec_sch_rate, NUM_TD_SCH_RATES)].
           td_punct_lo);
  HWIO_OUT(TD_PUNCT_HI,
           deint_td_sch_params[DEINT_SCH_RC_INDEX(sch_rc)]
           [DEINT_UPPER_BOUNDS_CHECK(dec_sch_rate, NUM_TD_SCH_RATES)].
           td_punct_hi);
  HWIO_OUT(TD_INTLV_SIZE_LO,
           deint_td_sch_params[DEINT_SCH_RC_INDEX(sch_rc)]
           [DEINT_UPPER_BOUNDS_CHECK(dec_sch_rate, NUM_TD_SCH_RATES)].
           td_intlv_size_lo);
  HWIO_OUT(TD_INTLV_SIZE_HI,
           deint_td_sch_params[DEINT_SCH_RC_INDEX(sch_rc)]
           [DEINT_UPPER_BOUNDS_CHECK(dec_sch_rate, NUM_TD_SCH_RATES)].
           td_intlv_size_hi);
  HWIO_OUT(TD_PARAMS_LO,
           deint_td_sch_params[DEINT_SCH_RC_INDEX(sch_rc)]
           [DEINT_UPPER_BOUNDS_CHECK(dec_sch_rate, NUM_TD_SCH_RATES)].
           td_params_lo);
  HWIO_OUT(TD_PARAMS_HI,
           deint_td_sch_params[DEINT_SCH_RC_INDEX(sch_rc)]
           [DEINT_UPPER_BOUNDS_CHECK(dec_sch_rate, NUM_TD_SCH_RATES)].
           td_params_hi);

  HWIO_OUT(TD_INTLV_LEN_X2, 0x0000);
  HWIO_OUT(TD_INTLV_LEN_X4, 0x0000);
  HWIO_OUT(TD_INTLV_LEN_X8, 0x0000);
}
#endif /* MSMHW_TURBO_CODING */

/*==========================================================================
FUNCTION DEINT_LOAD_SCH_PKT_PARAMS

DESCRIPTION
  This function loads the deinterleaver packet parameters for Supplemental 
  Channel. The values programmed into the registers take effect at the next 
  SYNC20.
 
DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
==========================================================================*/
static void deint_load_sch_pkt_params
(
#ifdef FEATURE_IS2000_REL_A //32X
  uint8   num_ltu,          
  uint16  ltu_len,          
#endif /* FEATURE_IS2000_REL_A */
  deint_rc_type sch_rc, 
  dec_sch_rate_type sch_rate
)
{
   
#ifdef FEATURE_IS2000_REL_A //32X
  word pkt_blk_size_hi;    
  /* punct_length | symbol_repeat | code_rate */
  word pkt_punct_lo;                      
  /* lo_punct_pattern | rdcnt_en_sel | num_ltu_frm */
#ifdef MSMHW_EXTENDED_LTU_LEN
  word ltu_len_in_bytes;
  word ltu_len_in_bytes_hi_value;
  word ltu_len_in_bytes_lo_value;
#endif /* MSMHW_EXTENDED_LTU_LEN */
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_32X_SCH_DATA_RATE
   if(sch_rate > DEC_SCH_RATE_32X)
#else
   if(sch_rate > DEC_SCH_RATE_16X)
#endif
   {
      MSG_FATAL("Bad SCH data rate", 0, 0, 0);
   } 
   
   MSG_HIGH("SCH Conv DEINT,rc=%d,r_off=%d", sch_rc,sch_rate,0);
   /* Select Packet E to program parameters for convolutionally encoded SCH. 
      Note Packets A - D are used for FCH. */
   
   SET_DINT_PKT_OFFSET (DINT_PKT_E_V | DINT_PKT_INTLV_CFG_LO_V);

   /* Note:::: write to DINT_PKT_CFG register causes the address pointer in 
    CFG_RAM to autoincrement by 16 bits at a time
     So each packet configuration will be selected automatically */
   

     /* Program PKT_INTLV_CFG_LO for Packet E  */
    HWIO_OUT( DINT_PKT_CFG, 
            deint_sch_pkt_params[DEINT_SCH_RC_INDEX(sch_rc)][sch_rate].
              pkt_intlv_cfg_lo);

     /* Program PKT_INTLV_PKT_HI for Packet E */
    HWIO_OUT( DINT_PKT_CFG,
            deint_sch_pkt_params[DEINT_SCH_RC_INDEX(sch_rc)][sch_rate].
              pkt_intlv_cfg_hi);

     /* Program PKT_BLK_SIZE_LO for Packet E */
    HWIO_OUT( DINT_PKT_CFG,
            deint_sch_pkt_params[DEINT_SCH_RC_INDEX(sch_rc)][sch_rate].
              pkt_blk_size_lo);
    
    /****
    When 32X is supported, the upper layer would pass the LTU size, and
    number of LTU per frame to physical layer.  In this case we would use 
    the LTU size and # of LTU per frame.
    ****/

     /* Prgram PKT_BLK_SIZE_HI for packet E */
     /* Bit 15:9 of pkt_blk_size_hi is the LTU length in byte */    
#ifdef FEATURE_IS2000_REL_A //32X

    pkt_blk_size_hi = deint_sch_pkt_params[DEINT_SCH_RC_INDEX(sch_rc)][sch_rate].
                    pkt_blk_size_hi;

#ifdef MSMHW_EXTENDED_LTU_LEN
    /* Extended LTU Length HW change (from MSM6050) only accomodate LTU 
    ** length in bytes.
    */
    ltu_len_in_bytes = ltu_len >> 3; 
    if (ltu_len_in_bytes > DINT_LTU_SIZE_MAX) 
    {
      ERR ("LTU len (bytes) > max, %d", ltu_len_in_bytes,0,0);
    }
    if (num_ltu > DINT_NUM_LTU_MAX) 
    {
      ERR ("Num LTU per fr > max, %d", num_ltu,0,0);
    }

    /* High value is most significant 5 bits of ltu_len_in_bytes */
    ltu_len_in_bytes_hi_value = ((ltu_len_in_bytes >> 2) << 9);

    /* Low value is least significant 2 bits of ltu_len_in_bytes */
    ltu_len_in_bytes_lo_value = (ltu_len_in_bytes & 3) << 6;

    /* With this operation, bits 14 and 15 are always zeros */
    pkt_blk_size_hi = (pkt_blk_size_hi & ~DINT_LTU_SIZE_IN_BYTE_M) |
                      ltu_len_in_bytes_hi_value;
#else
      // zero out bits 15:9 of pkt_blk_size_hi and then put in the ltu_size
      // in byte. 
    pkt_blk_size_hi = (pkt_blk_size_hi  & ~DINT_LTU_SIZE_IN_BYTE_M) |
                      ((ltu_len << 6)  &  DINT_LTU_SIZE_IN_BYTE_M);
#endif /* MSMHW_EXTENDED_LTU_LEN */
    //MSG_MED ("pkt_blk_size_hi=%d", pkt_blk_size_hi,0,0);
    HWIO_OUT( DINT_PKT_CFG, pkt_blk_size_hi);
    
#else
    
    HWIO_OUT( DINT_PKT_CFG, 
              deint_sch_pkt_params[DEINT_SCH_RC_INDEX(sch_rc)][sch_rate].
              pkt_blk_size_hi);
    
#endif /* FEATURE_IS2000_REL_A */

     /* Program PKT_PUNCT_LO for Packet E  */
#ifdef FEATURE_IS2000_REL_A //32X
    pkt_punct_lo = deint_sch_pkt_params[DEINT_SCH_RC_INDEX(sch_rc)][sch_rate].
                   pkt_punct_lo;
#ifdef MSMHW_EXTENDED_LTU_LEN
    // bits 6 & 7 is the least significant 2 bits of the ltu length in bytes.
    // bits 5 is unused.
    // bits 3:0 is number of LTU per frame.
    pkt_punct_lo = (
                    (pkt_punct_lo  & ~DINT_LTU_SIZE_IN_BYTE_LO_M  //7:5
                                   & ~DINT_LTU_PER_FRAME_M)     | //3:0   
                    ltu_len_in_bytes_lo_value                   | 
                    num_ltu
                   );
#else
    // bits 7:5 is the fractional potion of LTU size.
    // bits 3:0 is number of LTU per frame.
    pkt_punct_lo = (pkt_punct_lo    & ~DINT_LTU_SIZE_IN_BITS_M   //7:5
                                    & ~DINT_LTU_PER_FRAME_M)    |    
                   ((ltu_len << 5)  &  DINT_LTU_SIZE_IN_BITS_M) | //7:5
                   (num_ltu         &  DINT_LTU_PER_FRAME_M);      //3:0
#endif /* MSMHW_EXTENDED_LTU_LEN */
    //MSG_MED ("pkt_punct_lo=%d", pkt_punct_lo,0,0);
    HWIO_OUT( DINT_PKT_CFG, pkt_punct_lo);
    
#else

    HWIO_OUT( DINT_PKT_CFG, 
           deint_sch_pkt_params[DEINT_SCH_RC_INDEX(sch_rc)][sch_rate].
           pkt_punct_lo);
#endif /* FEATURE_IS2000_REL_A */

    /* Program PKT_PUNCT_HI for Packet E */
     HWIO_OUT( DINT_PKT_CFG, 
           deint_sch_pkt_params[DEINT_SCH_RC_INDEX(sch_rc)][sch_rate].
            pkt_punct_hi);

     /* Program PKT_PL_CRC_LO for Packet E */
     HWIO_OUT( DINT_PKT_CFG, 
            deint_sch_pkt_params[DEINT_SCH_RC_INDEX(sch_rc)][sch_rate].
            pkt_pl_crc_lo);

     /* Program PKT_PL_CRC_HI for Packet E */
     HWIO_OUT( DINT_PKT_CFG, 
          deint_sch_pkt_params[DEINT_SCH_RC_INDEX(sch_rc)][sch_rate].
          pkt_pl_crc_hi);

   
 
}


/*==========================================================================
FUNCTION DEINT_CREATE_SC_TASK_LIST()

DESCRIPTION
  Create Task1 corresponding to SYNC CHANNEL HYPOTHESIS. Packet A
  contains the configuration  parameters for SYNC CHANNEL HYPOTHESIS.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void deint_create_sc_task_list
(
  void
)
{
   
   SET_DINT_TASK_OFFSET (DINT_FIRST_5MS_SLOT_ADDR);
   
   /* Create Task1 corresponding to SYNC CHANNEL HYPOTHESIS. Packet A 
      contains the configuration  parameters for SYNC CHANNEL HYPOTHESIS,
      so we select Packet A. 
      Note First write is always to TASK_LIST_LO and the next write is 
      to TASK_LIST_HI */
   
   HWIO_OUT(DINT_TASK_LIST, DINT_TASK_LIST_ACTIVE_TASK_V | 
            DINT_TASK_LIST_CH0_V | DINT_TASK_LIST_FRM_HYP_20MS_V |
            DINT_TASK_LIST_DEC_INTR_EN_V | DINT_TASK_LIST_SYNC_CHAN_EN_V | 
            DINT_TASK_LIST_PACKET_A_V);
         
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   
   /* We now need to create a zero task to dicate to the task arbitartor 
      that there is no other task in this 5ms slot. */
   
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);


   /* We also need to create ZERO tasks for the 2,3,4  5ms slot */

   SET_DINT_TASK_OFFSET (DINT_SECOND_5MS_SLOT_ADDR);
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);

   SET_DINT_TASK_OFFSET (DINT_THIRD_5MS_SLOT_ADDR);
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);

   SET_DINT_TASK_OFFSET (DINT_FOURTH_5MS_SLOT_ADDR);
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);

}

/*===========================================================================
FUNCTION DEINT_CREATE_PC_TASK_LIST()

DESCRIPTION
  This function creates a task for decoding paging channel message in the 
  first 5ms slot.

DEPENDENCIES
  deint_load_pc_pkt_params should have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
static void deint_create_pc_task_list
(
  void
)
{

   SET_DINT_TASK_OFFSET (DINT_FIRST_5MS_SLOT_ADDR);

   /* Create Task1 corresponding to PAGING CHANNEL HYPOTHESIS. Packet A
      contains the configuration  parameters for PAGING CHANNEL HYPOTHESIS,
      Note First write is always to TASK_LIST_LO and the next write is
      to TASK_LIST_HI */

   HWIO_OUT(DINT_TASK_LIST, DINT_TASK_LIST_ACTIVE_TASK_V |
            DINT_TASK_LIST_CH0_V | DINT_TASK_LIST_FRM_HYP_20MS_V |
            DINT_TASK_LIST_DEC_INTR_EN_V | DINT_TASK_LIST_PACKET_A_V);

   HWIO_OUT(DINT_TASK_LIST, 0x0000);  /* OB_ADDR = 0x0000 */
   /* We now need to create a zero task to indicate to the task arbitartor
      that there is no other task in this 5ms slot. */

   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);


   /* We also need to create ZERO tasks for the 2,3,4  5ms slot */

   SET_DINT_TASK_OFFSET (DINT_SECOND_5MS_SLOT_ADDR);
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);

   SET_DINT_TASK_OFFSET (DINT_THIRD_5MS_SLOT_ADDR);
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);

   SET_DINT_TASK_OFFSET (DINT_FOURTH_5MS_SLOT_ADDR);
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);

}


#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================
FUNCTION DEINT_CREATE_BCCH_TASK_LIST()

DESCRIPTION
  This function creates a task for decoding a BCCH message in the 
  second 5ms task slot.

DEPENDENCIES
  deint_load_bcch_pkt_params should have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/

static void 
deint_create_bcch_task_list (deint_ch_id_type ch_id, byte  ncc_actchan_mask)
{
  unsigned int task_entry = 0;

  if (ncc_actchan_mask == 0)
  {
    /* only clear deinterleaver task in the first slot if there is no active FCCCH*/
  SET_DINT_TASK_OFFSET (DINT_FIRST_5MS_SLOT_ADDR);
    HWIO_OUT(DINT_TASK_LIST, 0x0000);
    HWIO_OUT(DINT_TASK_LIST, 0x0000);
  }

  /* create BCCH task list in the second 5ms slot */
  SET_DINT_TASK_OFFSET (DINT_SECOND_5MS_SLOT_ADDR);

 /* Create Task1 corresponding to BCCH hypothesis. 
  * Packet A contains the configuration  parameters for BCCH.
  * Note First write is always to TASK_LIST_LO and the next write is
  * to TASK_LIST_HI */
  
    task_entry = DINT_TASK_LIST_ACTIVE_TASK_V | 
    DINT_TASK_LIST_DEC_INTR_EN_V |   
    DINT_TASK_LIST_FRM_HYP_20MS_V | /* 20ms or larger */
    DINT_TASK_LIST_PACKET_A_V;
  
  switch (ch_id) {
    case DEINT_CH0:
      task_entry |= DINT_TASK_LIST_CH0_V;
      break;
    case DEINT_CH1:
      task_entry |= DINT_TASK_LIST_CH1_V;
      break;
    case DEINT_CH2:
      task_entry |= DINT_TASK_LIST_CH2_V;
      break;
    default:
      MSG_LOW("ch_id bad %d", ch_id, 0, 0);
  }

  HWIO_OUT(DINT_TASK_LIST, task_entry);
      
  MSG_LOW("Task list write: 0x%x", task_entry, 0, 0);

  HWIO_OUT(DINT_TASK_LIST, 0x0000);  /* OB_ADDR = 0x0000 */
  
   
  /* We now need to create a zero task to indicate to the task arbitartor
     that there is no other task in this 5ms slot. */

  /* Create a zero task to terminate the list. */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);   
  HWIO_OUT(DINT_TASK_LIST, 0x0000);

  /* We also need to create zero tasks for the 3,4  5ms slot */

  SET_DINT_TASK_OFFSET (DINT_THIRD_5MS_SLOT_ADDR);
  HWIO_OUT(DINT_TASK_LIST, 0x0000);
  HWIO_OUT(DINT_TASK_LIST, 0x0000);

  SET_DINT_TASK_OFFSET (DINT_FOURTH_5MS_SLOT_ADDR);
  HWIO_OUT(DINT_TASK_LIST, 0x0000);
  HWIO_OUT(DINT_TASK_LIST, 0x0000);
}
#endif /* FEATURE_IS2000_REL_A_CC */

/*=========================================================================
FUNCTION DEINT_CREATE_FCH_TASK_LIST

DESCRIPTION
  This function creates 4 tasks in the first 5ms slot of task RAM. The 4 tasks
  correspond to FULL RATE, HALF RATE, QUARTER RATE and 8TH RATE Hypothesis. 
  Decoder Interrupt is enabled after the last task is completed.

DEPENDENCIES
  deint_load_fch_pkt_params should have been called.

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
#ifndef FEATURE_IS2000_REL_A_SVD

static void deint_create_fch_task_list
(
  deint_rc_type fch_rc, 
  boolean int_enable
)
{
   SET_DINT_TASK_OFFSET (0x0000);
   
   /* Create Task1 corresponding to FULL RATE HYPOTHESIS. Packet A contains
      the configuration parameters for FULL Rate HYPOTHESIS, 
      so we select Packet A */
   /* Note First write is always to TASK_LIST_LO and the next 
      write is to TASK_LIST_HI */

   HWIO_OUT(DINT_TASK_LIST, 
            DINT_TASK_LIST_ACTIVE_TASK_V | 
            DINT_TASK_LIST_CH0_V | DINT_TASK_LIST_FRM_HYP_20MS_V
            | DINT_TASK_LIST_PACKET_A_V);

   HWIO_OUT(DINT_TASK_LIST, 
           ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(fch_rc)].
             dec_ob_addr_full_rate) / 2));   

   /* At this point the task ram address has incremented to 0x02, 
      so it now points to the second task of the first 5ms slot.
      Create Task 2 corresponding to the HALF RATE HYPOTHESIS. 
      Packet B contains the configuration parameters for HALF RATE 
      HYPOTHESIS */

   HWIO_OUT(DINT_TASK_LIST,
             DINT_TASK_LIST_ACTIVE_TASK_V | DINT_TASK_LIST_CH0_V |
              DINT_TASK_LIST_FRM_HYP_20MS_V | DINT_TASK_LIST_PACKET_B_V);

   HWIO_OUT(DINT_TASK_LIST, 
           ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(fch_rc)].
              dec_ob_addr_half_rate) / 2));

   /* At this point the task ram address has incremented to 0x04, 
      so it now points to the third task of the first 5ms slot.
      Create Task 3 corresponding to the QUARTER RATE HYPOTHESIS. 
      Packet C contains the configuration parameters for QUARTER
      RATEHYPOTHESIS */
   HWIO_OUT(DINT_TASK_LIST, 
            DINT_TASK_LIST_ACTIVE_TASK_V | DINT_TASK_LIST_CH0_V | 
            DINT_TASK_LIST_FRM_HYP_20MS_V  | DINT_TASK_LIST_PACKET_C_V);
   HWIO_OUT(DINT_TASK_LIST, 
           ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(fch_rc)].
              dec_ob_addr_qtr_rate) / 2));
   
   /* At this point the task ram address has incremented to 0x06, 
      so it now points to the fourth task of the first 5ms slot.
      Create Task 4 corresponding to the EIGHTH RATE HYPOTHESIS. 
      Packet D contains the configuration parameters for EIGHTH
      RATE HYPOTHESIS */
#ifdef FEATURE_FCH_5MS_FRAME
#error code not present
#else
  if(int_enable) {
   HWIO_OUT(DINT_TASK_LIST, 
            DINT_TASK_LIST_ACTIVE_TASK_V | DINT_TASK_LIST_CH0_V | 
            DINT_TASK_LIST_FRM_HYP_20MS_V |
            DINT_TASK_LIST_DEC_INTR_EN_V | DINT_TASK_LIST_PACKET_D_V);
   }
   else
   {
   HWIO_OUT(DINT_TASK_LIST, 
            DINT_TASK_LIST_ACTIVE_TASK_V | DINT_TASK_LIST_CH0_V | 
            DINT_TASK_LIST_FRM_HYP_20MS_V |
            DINT_TASK_LIST_PACKET_D_V);
   }
#endif /* FEATURE_FCH_5MS_FRAME */

   HWIO_OUT(DINT_TASK_LIST, 
           ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(fch_rc)].
              dec_ob_addr_8th_rate) / 2));

#ifdef FEATURE_PLT  
#error code not present
#endif  /* FEATURE_PLT */ 

   /* We now need to create a zero task to dicate to the task arbitartor that 
      there is no other task in this 5ms slot */
   
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);


#ifdef FEATURE_PLT  
#error code not present
#endif  /* FEATURE_PLT  */

#ifdef FEATURE_PLT 
#error code not present
#endif  /* FEATURE_PLT */

#ifdef FEATURE_PLT 
#error code not present
#endif  /* FEATURE_PLT */ 
}

#else /* of !FEATURE_IS2000_REL_A_SVD */

static void deint_create_fch_task_list
(
  deint_rc_type fch_rc, 
  boolean       fch_pc
)
{
  word channel_select = DINT_TASK_LIST_CH0_V;   
  word channel_select_mask = DINT_TASK_LIST_CH0_M;   
    /* Temp value to hold channel selection       */

  if ((curr_deint_ch_active != DEINT_FCH_ACTIVE) &&
      (curr_deint_ch_active != DEINT_FCH_DCCH_ACTIVE)
     )
  {
    ERR ("Bad config create_fch_task_list()",0,0,0);
    return;
  }

  /* Select either channel 0 or channel 1 for F-DCCH depending on the Power
  ** Control indicator.  If Power Control indicator is TRUE, set F-DCCH on
  ** channel 0, otherwise, set it on channel 1.
  */
  MSG_HIGH("FCH PowerCnt=%d", fch_pc,0,0);
  if (fch_pc)
  {
    channel_select_mask = DINT_TASK_LIST_CH0_M;
    channel_select = DINT_TASK_LIST_CH0_V;
  }
  else
  {
    channel_select_mask = DINT_TASK_LIST_CH1_M;
    channel_select = DINT_TASK_LIST_CH1_V;
  }

  SET_DINT_TASK_OFFSET (0x0000);
   
  /* Create Task1 corresponding to FULL RATE HYPOTHESIS. Packet A contains
  ** the configuration parameters for FULL Rate HYPOTHESIS, 
  ** so we select Packet A.
  **
  ** Note First write is always to TASK_LIST_LO and the next 
  ** write is to TASK_LIST_HI.
  */
  HWIO_OUT(DINT_TASK_LIST, 
           DINT_TASK_LIST_ACTIVE_TASK_V | 
           channel_select | DINT_TASK_LIST_FRM_HYP_20MS_V | 
           DINT_TASK_LIST_PACKET_A_V);

  HWIO_OUT(DINT_TASK_LIST,
           ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(fch_rc)].
             dec_ob_addr_full_rate) / 2));   

  /* At this point the task ram address has incremented to 0x02, 
     so it now points to the second task of the first 5ms slot.
     Create Task 2 corresponding to the HALF RATE HYPOTHESIS. 
     Packet B contains the configuration parameters for HALF RATE 
     HYPOTHESIS */
  HWIO_OUT(DINT_TASK_LIST,
           DINT_TASK_LIST_ACTIVE_TASK_V | 
           channel_select | DINT_TASK_LIST_FRM_HYP_20MS_V | 
           DINT_TASK_LIST_PACKET_B_V);

  HWIO_OUT(DINT_TASK_LIST, 
           ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(fch_rc)].
             dec_ob_addr_half_rate) / 2));

  /* At this point the task ram address has incremented to 0x04, 
     so it now points to the third task of the first 5ms slot.
     Create Task 3 corresponding to the QUARTER RATE HYPOTHESIS. 
     Packet C contains the configuration parameters for QUARTER
     RATEHYPOTHESIS */
  HWIO_OUT(DINT_TASK_LIST, 
           DINT_TASK_LIST_ACTIVE_TASK_V | 
           channel_select | DINT_TASK_LIST_FRM_HYP_20MS_V | 
           DINT_TASK_LIST_PACKET_C_V);
  HWIO_OUT(DINT_TASK_LIST, 
          ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(fch_rc)].
            dec_ob_addr_qtr_rate) / 2));
  
  /* At this point the task ram address has incremented to 0x06, 
     so it now points to the fourth task of the first 5ms slot.
     Create Task 4 corresponding to the EIGHTH RATE HYPOTHESIS. 
     Packet D contains the configuration parameters for EIGHTH
     RATE HYPOTHESIS */
  if (curr_deint_ch_active == DEINT_FCH_ACTIVE)
  {
    /* If just FCH is active, enable interrupt and create a zero task
    ** (zero task is the task list terminator.)  
    ** Important change:  the macro used to set register DINT_TASK_LIST_WH
    ** has been changed from MSM_OUTH to MSM_OUTHM.  This change is necessary 
    ** because (only) the interrupt enable field of this register may be 
    ** changed later on if DCCH is added.
    */
    HWIO_OUTM (DINT_TASK_LIST, 
               (DINT_TASK_LIST_ACTIVE_TASK_M | 
                channel_select_mask | DINT_TASK_LIST_FRM_HYP_20MS_M |
                DINT_TASK_LIST_DEC_INTR_EN_M | DINT_TASK_LIST_PACKET_D_M),
               (DINT_TASK_LIST_ACTIVE_TASK_V | 
                channel_select | DINT_TASK_LIST_FRM_HYP_20MS_V |
                DINT_TASK_LIST_DEC_INTR_EN_V | DINT_TASK_LIST_PACKET_D_V)
              );
    HWIO_OUT(DINT_TASK_LIST, 
            ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(fch_rc)].
              dec_ob_addr_8th_rate) / 2));

    HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
    HWIO_OUT(DINT_TASK_LIST, 0x0000);
  }
  else if (curr_deint_ch_active == DEINT_FCH_DCCH_ACTIVE)
  {
    /* Do not enable interrupt and do not create a zero task.  DCCH
    ** task enables interrupt and uses the next slot.
    */
    HWIO_OUT(DINT_TASK_LIST, 
             DINT_TASK_LIST_ACTIVE_TASK_V | 
             channel_select | DINT_TASK_LIST_FRM_HYP_20MS_V | 
             DINT_TASK_LIST_PACKET_D_V);
    HWIO_OUT(DINT_TASK_LIST, 
            ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(fch_rc)].
              dec_ob_addr_8th_rate) / 2));
  }
}

/*===========================================================================
FUNCTION DEINT_LOAD_DCCH_PKT_PARAMS

DESCRIPTION
  This function programs 1 FULL rate packet corresponding to the RC for DCCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None
===========================================================================*/
static void deint_load_dcch_pkt_params
(
  deint_rc_type dcch_rc
)
{
  
  /* Select Packet F to program parameters for FULL Rate Hypothesis */
  SET_DINT_PKT_OFFSET (DINT_PKT_F_V | DINT_PKT_INTLV_CFG_LO_V);

 /**********************PROGRAMMING FULL RATE HYPOTHESIS *****************/

  deint_load_pkt_params (&deint_fch_packet_params[DEINT_FCH_RC_INDEX(dcch_rc)].
            fch_full_rate_param);
 
} /* deint_load_dcch_pkt_params */

/*=========================================================================
FUNCTION DEINT_CREATE_DCCH_TASK_LIST

DESCRIPTION
  This function creates a task to decode DCCH in the 5th entry of the first
  5ms slot of task RAM.  The first 4 entries are reserved for FCH channel
  Full rate, Half rate, Quarter rate and Eighth rate tasks.  If FCH channel
  is not set up, this function creates 4 dummy tasks in place of the real
  FCH tasks.  Decoder interrupts are not enabled for any dummy tasks.
  Decoder interrupt is enabled after the real DCCH task is completed.

DEPENDENCIES
  deint_load_fch_pkt_params should have been called.

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
void deint_create_dcch_task_list
(
  deint_rc_type dcch_rc, 
  boolean       dcch_pc

)
{

  word channel_select = 0;   /* Temp value to hold channel selection       */

  if ((curr_deint_ch_active != DEINT_DCCH_ACTIVE) &&
      (curr_deint_ch_active != DEINT_FCH_DCCH_ACTIVE)
     )
  {
    ERR ("Bad config create_dcch_task_list()",0,0,0);
    return;
  }

  /* Select either channel 0 or channel 1 for F-DCCH depending on the Power
  ** Control indicator.  If Power Control indicator is TRUE, set F-DCCH on
  ** channel 0, otherwise, set it on channel 1.
  */
  MSG_HIGH("DCCH PowerCnt=%d", dcch_pc,0,0);
  if (dcch_pc)
  {
    channel_select = DINT_TASK_LIST_CH0_V;
  }
  else
  {
    channel_select = DINT_TASK_LIST_CH1_V;
  }

  if (curr_deint_ch_active == DEINT_DCCH_ACTIVE)
  {
    /* If just DCCH is active, create 4 dummy tasks with no interrupts.
    ** Continue with real DCCH task with interrupt and create a zero task
    ** (zero task is the task list terminator.)
    */
    SET_DINT_TASK_OFFSET (0x0000);
     
    /* 4 dummy tasks similar to FCH configuration.
    **
    ** Note First write is always to TASK_LIST_LO and the next 
    ** write is to TASK_LIST_HI.
    */
    HWIO_OUT(DINT_TASK_LIST,                // Full rate
             DINT_TASK_LIST_ACTIVE_TASK_V | 
             channel_select | DINT_TASK_LIST_FRM_HYP_20MS_V |
             DINT_TASK_LIST_PACKET_A_V);
    HWIO_OUT(DINT_TASK_LIST,
             ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(dcch_rc)].
               dec_ob_addr_full_rate) / 2));   
   
    HWIO_OUT(DINT_TASK_LIST,                // Half rate
             DINT_TASK_LIST_ACTIVE_TASK_V | 
             channel_select | DINT_TASK_LIST_FRM_HYP_20MS_V | 
             DINT_TASK_LIST_PACKET_B_V);
    HWIO_OUT(DINT_TASK_LIST, 
             ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(dcch_rc)].
               dec_ob_addr_half_rate) / 2));

    HWIO_OUT(DINT_TASK_LIST,                // Quarter rate
             DINT_TASK_LIST_ACTIVE_TASK_V | 
             channel_select | DINT_TASK_LIST_FRM_HYP_20MS_V | 
             DINT_TASK_LIST_PACKET_C_V);
    HWIO_OUT(DINT_TASK_LIST, 
             ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(dcch_rc)].
               dec_ob_addr_qtr_rate) / 2));
    
    HWIO_OUT(DINT_TASK_LIST,                // Eigth rate
             DINT_TASK_LIST_ACTIVE_TASK_V | 
             channel_select | DINT_TASK_LIST_FRM_HYP_20MS_V |
             DINT_TASK_LIST_PACKET_D_V);
    HWIO_OUT(DINT_TASK_LIST, 
             ((dec_fch_ob_params[DEINT_FCH_RC_INDEX(dcch_rc)].
               dec_ob_addr_8th_rate) / 2));

  }
  else if (curr_deint_ch_active == DEINT_FCH_DCCH_ACTIVE)
  {
    /* If both DCCH and FCH are active, make sure to turn off the FCH
    ** interrupt.  The FCH interrupt is in the third slot of the first 5ms bin.
    */
    SET_DINT_TASK_OFFSET (0x0006);
    HWIO_OUTM (DINT_TASK_LIST, 
               (DINT_TASK_LIST_DEC_INTR_EN_M),
               (DINT_TASK_LIST_DEC_INTR_DIS_V));

    /* Now move the task offset to get ready for DCCH task creation.
    */
    SET_DINT_TASK_OFFSET (0x0008);

  }

  /* Create DCCH Task corresponding to FULL RATE HYPOTHESIS. Packet F contains
  ** the configuration parameters for DCCH FULL Rate HYPOTHESIS, 
  ** so we select Packet F.  Note that we unconditionally turn on the DCCH
  ** interrupt here because DCCH is the last task in the task list.
  */
  HWIO_OUT(DINT_TASK_LIST, 
         DINT_TASK_LIST_ACTIVE_TASK_V | 
         channel_select | DINT_TASK_LIST_FRM_HYP_20MS_V |
         DINT_TASK_LIST_DEC_INTR_EN_V | DINT_TASK_LIST_PACKET_F_V);

  HWIO_OUT(DINT_TASK_LIST, (DEC_DCCH_FULL_OB_ADDR / 2));

  /* We now need to create a zero task to dicate to the task arbitartor that 
  ** there is no other task in this 5ms slot.
  */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);   /* Create a ZERO Task */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);

} /* end deint_create_dcch_task_list () */
#endif /* !FEATURE_IS2000_REL_A_SVD */


/*=========================================================================
FUNCTION DEINT_CREATE_SCCH_TASK_LIST

DESCRIPTION
  This function creates a task to decode SCCH in the second 5ms slot of task 
  RAM.
  Decoder Interrupt is enabled after the  task is completed.

DEPENDENCIES
  deint_load_scch_pkt_params should have been called.

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
static void deint_create_scch_task_list
(
  deint_rc_type scch_rc
)
{

     /* Note First write is always to TASK_LIST_LO and the next
      write is to TASK_LIST_HI */
  byte bit_shift;
  bit_shift = 0;
  
  if(scch_rc == DEINT_RC1)
  {
   /* The start of payload occurs at bit 1, since bit 0 is MM bit */
   /* NO   Layer 2 messages on SCCH channels */

    bit_shift = 1; 
  }
  else if(scch_rc == DEINT_RC2)
  {
   /* The start of payload occurs at bit 2, since bit 0 is EIB &
     bit 1 is MM bit */
   /* NO   Layer 2 messages on SCCH channels */
     
    bit_shift = 2;
  }
  else
  {
    MSG_ERROR("Bad SCCH RC %d", scch_rc, 0, 0);
  } 
 
  SET_DINT_TASK_OFFSET (DINT_SECOND_5MS_SLOT_ADDR);
  HWIO_OUT(DINT_TASK_LIST,
           DINT_TASK_LIST_ACTIVE_TASK_V |
           DINT_TASK_LIST_CH1_V | DINT_TASK_LIST_FRM_HYP_20MS_V |
           DINT_TASK_LIST_DEC_INTR_EN_V | DINT_TASK_LIST_PACKET_A_V |
           HWIO_FMSK(DINT_TASK_LIST,PAGE0_SUB_CHANS) |
           bit_shift);

  HWIO_OUT(DINT_TASK_LIST, DEC_SCH_RAM_BASE);


  /* We now need to create a zero task to dicate to the task arbitartor that
  ** there is no other task in this 5ms slot 
  */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);
}

/*=========================================================================
FUNCTION DEINT_CREATE_SCH_TASK_LIST

DESCRIPTION
  This function creates a task to decode SCH in the second 5ms slot of task RAM. 
    Decoder Interrupt is enabled after the  task is completed.

DEPENDENCIES
  deint_load_sch_pkt_params should have been called.

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
static void deint_create_sch_task_list
(
#ifdef FEATURE_IS2000_REL_A
  dec_mux_pdu_type_type pdu_type,
#endif
  deint_rc_type sch_rc,
  dec_sch_rate_type dec_sch_rate   /* SCH Rate */
)
{
  byte bit_shift=0;
#ifdef FEATURE_IS2000_REL_A
  if( pdu_type != DEC_MUX_PDU_TYPE_5 )
  {
#endif
#ifdef FEATURE_IS2000_REL_A_SVD
    /* Note that bit_shift is only used in case of 1X.
    **
    ** Do not shift out the MM bit if secondary traffic stream is 
    ** supported.  In this case, RLP data can be carried in the primary
    ** or secondary traffic stream.  The MM bit is the only bit which 
    ** indicates which stream carries RLP data.
    **
    ** So for rate set 2, also leave in the EIB bit to simplify the driver
    ** code.  Both EIB and MM bits will be discarded by the MUX layer before
    ** data is given to RLP.
    **
    ** This change also affects the decoder read SCH functions (data & status) 
    ** and the processing of rx frames in the rxdemux layer.
    */
    bit_shift=0;
#else 
    if((sch_rc == DEINT_RC3) || (sch_rc == DEINT_RC4)) 
    {
      bit_shift=1;
    }
    else
    {
      bit_shift=2;
    }
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_REL_A
  }
#endif
  /* Note First write is always to TASK_LIST_LO and the next 
  ** write is to TASK_LIST_HI 
  */
  SET_DINT_TASK_OFFSET (DINT_SECOND_5MS_SLOT_ADDR);
  if(dec_sch_rate == DEC_SCH_RATE_1X)
  {
    HWIO_OUT(DINT_TASK_LIST,
             DINT_TASK_LIST_ACTIVE_TASK_V |
             DINT_TASK_LIST_CH2_V | DINT_TASK_LIST_FRM_HYP_20MS_V |
             DINT_TASK_LIST_DEC_INTR_EN_V | DINT_TASK_LIST_PACKET_E_V | 
             bit_shift);
  }
  else
  {
    HWIO_OUT(DINT_TASK_LIST, 
             DINT_TASK_LIST_ACTIVE_TASK_V | 
             DINT_TASK_LIST_CH2_V | DINT_TASK_LIST_FRM_HYP_20MS_V |
             DINT_TASK_LIST_DEC_INTR_EN_V | DINT_TASK_LIST_PACKET_E_V);
  }

  HWIO_OUT(DINT_TASK_LIST, DEC_SCH_RAM_BASE);
             
  /* We now need to create a zero task to dicate to the task arbitartor that 
  ** there is no other task in this 5ms slot 
  */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);
}

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)  
/*=========================================================================
FUNCTION deint_delete_fch_dcch_task_list

DESCRIPTION
    This function deletes the FCH and DCCH tasks from the deinterleaver task 
    RAM.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void deint_delete_fch_dcch_task_list 
(
  void
)
{
  /* Remove any FCH or DCCH task pending in deint_sequencing_cmd_q.
  */
  if ( q_cnt( &deint_sequencing_cmd_q ) != 0 )
  {
    /* Get pointer to first item in Q.
    */
    deint_sequencing_cmd_type  *cur_ptr = q_check( &deint_sequencing_cmd_q );
    deint_sequencing_cmd_type  *next_ptr;   /* Points to next link in Q */
    do
    {
      next_ptr = q_next( &deint_sequencing_cmd_q, &cur_ptr->hdr.link );
      if (( cur_ptr->hdr.command == DEINT_DCCH_READ_PARAMS ) ||
          ( cur_ptr->hdr.command == DEINT_FCH_READ_PARAMS ) 
         )
      {
        MSG_HIGH("Removing remnant FCH or DCCH cmd",0,0,0);

        /* Found pending FCH or DCCH task, remove from sequencing queue.
        */
        q_delete( &deint_sequencing_cmd_q, &cur_ptr->hdr.link );
        q_put(cur_ptr->hdr.done_q_ptr, &cur_ptr->hdr.link);
      }
    } while ( (cur_ptr = next_ptr) != NULL );
  }

  /* Select first 5ms bin */
  SET_DINT_TASK_OFFSET (0x0000);

  /* We now need to create a zero task to indicate to the task arbitartor that 
  ** there is no other task in this 5ms bin.
  */
  HWIO_OUT(DINT_TASK_LIST, 0x0000);   
  HWIO_OUT(DINT_TASK_LIST, 0x0000);

  /* Indicate that there's current no FCH or DCCH task active.
  */
  curr_deint_ch_active = DEINT_CH_INACTIVE;

} /* end deint_delete_fch_dcch_task_list */

#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

/*=========================================================================
FUNCTION DEINT_DELETE_SUPPLEMENTAL_TASK_LIST

DESCRIPTION
    This function deletes the sch/scch task from the deinterleaver task ram.
    The SCh/SCCH task is the first task of the second 5ms slot.

DEPENDENCIES
  deint_load_sch_pkt_params should have been called.

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
void deint_delete_supplemental_task_list
(
  void
)
{

  // First any SCH task pending in deint_sequencing_cmd_q

  if ( q_cnt( &deint_sequencing_cmd_q ) != 0 )
  {
    // Get pointer to first item in Q
    deint_sequencing_cmd_type  *cur_ptr = q_check( &deint_sequencing_cmd_q );
    deint_sequencing_cmd_type  *next_ptr;   /* Points to next link in Q */
    do
    {
      next_ptr = q_next( &deint_sequencing_cmd_q, &cur_ptr->hdr.link );
      if ( cur_ptr->hdr.command == DEINT_SCH_READ_PARAMS )
      {
        MSG_HIGH("Removing remnant SCH cmd",0,0,0);
        // Found pending SCH task, remove from sequencing queue
        q_delete( &deint_sequencing_cmd_q, &cur_ptr->hdr.link );
        q_put(cur_ptr->hdr.done_q_ptr, &cur_ptr->hdr.link);
      }
    } while ( (cur_ptr = next_ptr) != NULL );
  }

  /* Delete Convolutional task entry in the 2nd bin */
  SET_DINT_TASK_OFFSET (DINT_SECOND_5MS_SLOT_ADDR);
  HWIO_OUT(DINT_TASK_LIST, 0x0000);  
  HWIO_OUT(DINT_TASK_LIST, 0x0000);

#ifdef FEATURE_IS2000_REL_A
  /* Delete Turbo task entry in the 3rd bin */
  SET_DINT_TASK_OFFSET (DINT_THIRD_5MS_SLOT_ADDR);
  HWIO_OUT(DINT_TASK_LIST, 0x0000);  
  HWIO_OUT(DINT_TASK_LIST, 0x0000);

  /* Reset SCH setup struct */
  deint_atmr_isr2_init();
#endif /* FEATURE_IS2000_REL_A */

}

#ifdef MSMHW_TURBO_CODING
/*=========================================================================
FUNCTION DEINT_CREATE_DUMMY_SCH_TASK_LIST

DESCRIPTION
    This function creates a dummy task in the second 5ms slot. 
    This function should be called only when turbo sch is enabled.

DEPENDENCIES
 

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
static void deint_create_dummy_sch_task_list
(
  void
)
{

     /* Note First write is always to TASK_LIST_LO and the next
      write is to TASK_LIST_HI */

#ifdef FEATURE_IS2000_REL_A
   SET_DINT_TASK_OFFSET (DINT_THIRD_5MS_SLOT_ADDR);
#else
   SET_DINT_TASK_OFFSET (DINT_SECOND_5MS_SLOT_ADDR);
#endif /* EATURE_IS2000_REL_A */
   
  /* We now need to create a dummy task that will generate a interrupt.
     This interrupt will be used to as th SCH interrupt when turbo decoder
     is operational */


   HWIO_OUT(DINT_TASK_LIST, 
            DINT_TASK_LIST_ACTIVE_TASK_V | DINT_TASK_LIST_CH0_V | 
            DINT_TASK_LIST_FRM_HYP_20MS_V |
            DINT_TASK_LIST_DEC_INTR_EN_V | DINT_TASK_LIST_PACKET_A_V);
   HWIO_OUT(DINT_TASK_LIST, DEC_SCH_DUMMY_FULL_OB_ADDR);

   /* We now need to create a zero task to dicate to the task arbitartor that 
      there is no other task in this 5ms slot */
   
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);

}
#endif /* MSMHW_TURBO_CODING */

/*=========================================================================
FUNCTION DEINT_PROCESS_SEQUENCING_CMD

DESCRIPTION
  This function checks the deint cmd_q to see if there are any pending
  commands to program the read parameters corresponding to FCH/SCH/SCCH.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
void deint_process_sequencing_cmd
(
  void
)
{
  deint_sequencing_cmd_type *cmd_ptr;

  INTLOCK();

  while((cmd_ptr = (deint_sequencing_cmd_type *)
           q_get(&deint_sequencing_cmd_q)) != NULL)
  {
    switch(cmd_ptr->hdr.command)
    {
       case DEINT_FCH_READ_PARAMS:
       {
         MSG_HIGH("New FCH Deint tsk, RC= %d", cmd_ptr->fch_cmd.fch_rc,0,0);
         (void) deint_load_fch_pkt_params(cmd_ptr->fch_cmd.fch_rc);
         deint_create_fch_task_list(
                                    cmd_ptr->fch_cmd.fch_rc, 
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
                                    cmd_ptr->fch_cmd.fch_pc
#else
                                    TRUE
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
                                   );
         break;
       } 

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
       case DEINT_DCCH_READ_PARAMS:
       {
         MSG_HIGH ("New DCCH Deint tsk, RC= %d", cmd_ptr->dcch_cmd.dcch_rc,0,0);
         deint_load_dcch_pkt_params (cmd_ptr->dcch_cmd.dcch_rc);
         deint_create_dcch_task_list (cmd_ptr->dcch_cmd.dcch_rc, 
                                      cmd_ptr->dcch_cmd.dcch_pc
                                     );
         break;
       } 
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

       case DEINT_SCCH_READ_PARAMS:
       {
         /* We currently do not support mixed rate SCCH.  Therefore, the packet
          * paramters are the same as FCH, so there is no need to reload them. */
         deint_create_scch_task_list(cmd_ptr->scch_cmd.scch_rc);
         break;
       }
       case DEINT_SCH_READ_PARAMS:
       {
         if(cmd_ptr->sch_cmd.dec_coding == DEC_CONVOLUTIONAL)
         {
           /* Sequencing is currently done for Convolutionally encoded frames */
           deint_load_sch_pkt_params(
#ifdef FEATURE_IS2000_REL_A
                                     cmd_ptr->sch_cmd.num_ltu,
                                     cmd_ptr->sch_cmd.ltu_len,
#endif /* FEATURE_IS2000_REL_A */
                                     cmd_ptr->sch_cmd.sch_rc, 
                                     cmd_ptr->sch_cmd.dec_sch_rate);
#ifdef FEATURE_IS2000_REL_A
           /* Delay changing SCH booleans to the 4th bin of this frame. */
           sch_setup.this_frame = TRUE;
           sch_setup.turbo_dec_int = FALSE;
           sch_setup.sup_dec_int = TRUE;
#else
           rxc_sup_dec_int_enable=TRUE;
#if defined(MSMHW_TURBO_CODING)
           rxc_turbo_dec_enabled=FALSE;
#endif /*MSMHW_TURBO_CODING*/
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_A
           deint_create_sch_task_list(cmd_ptr->sch_cmd.pdu_type, cmd_ptr->sch_cmd.sch_rc,
                                      cmd_ptr->sch_cmd.dec_sch_rate);
#else
           deint_create_sch_task_list(cmd_ptr->sch_cmd.sch_rc,
                                      cmd_ptr->sch_cmd.dec_sch_rate);
#endif
         }
#if defined(MSMHW_TURBO_CODING)
         else
         {
           deint_load_td_dec_params(cmd_ptr->sch_cmd.sch_rc,
                                    cmd_ptr->sch_cmd.dec_sch_rate);
#ifdef FEATURE_IS2000_REL_A
           /* Delay changing SCH booleans to the 4th bin of this frame.
           ** Delay creating dummy SCH task to the 4th bin of this frame. 
           */
           sch_setup.this_frame = TRUE;
           sch_setup.turbo_dec_int = TRUE;
           sch_setup.sup_dec_int = TRUE;
#else
           /* TURBO CODE */
           deint_create_dummy_sch_task_list();
           rxc_sup_dec_int_enable=TRUE;
           rxc_turbo_dec_enabled=TRUE;
#endif /* FEATURE_IS2000_REL_A */
         }
#endif /* MSMHW_TURBO_CODING */
         break;
       }
       default:
       {
         MSG_ERROR("Bad DEINT SEQ CMD", 0, 0, 0);
         break;
       }
    }  /* switch */
    q_put(cmd_ptr->hdr.done_q_ptr, &cmd_ptr->hdr.link);
  }
  INTFREE();
}


#ifdef FEATURE_IS2000_REL_A_CC

/*=============================================================================
=============================================================================*/
void deint_load_ccch_pkt_params
( 
  deint_code_rate_type code_rate, 
  dec_ccch_data_rate_type data_rate 
)
{
  
  /* Select Packet G to program parameters */

  SET_DINT_PKT_OFFSET (DINT_PKT_G_V | DINT_PKT_INTLV_CFG_LO_V);

  /* Note:::: write to DINT_PKT_CFG register causes the address pinter in 
     CFG_RAM to autoincrement by 16 bits at a time
      So each packet configuration will be selected automatically */

  /* Program PKT_INTLV_CFG_LO for Packet G  */
  HWIO_OUT( DINT_PKT_CFG, 
           deint_ccch_packet_params[code_rate][data_rate].pkt_intlv_cfg_lo);

  /* Program PKT_INTLV_PKT_HI for Packet G */
  HWIO_OUT( DINT_PKT_CFG,
           deint_ccch_packet_params[code_rate][data_rate].pkt_intlv_cfg_hi);

  /* Program PKT_BLK_SIZE_LO for Packet  G */
  HWIO_OUT( DINT_PKT_CFG,
           deint_ccch_packet_params[code_rate][data_rate].pkt_blk_size_lo);

  /* Prgram PKT_BLK_SIZE_HI for packet G */
  HWIO_OUT( DINT_PKT_CFG,
           deint_ccch_packet_params[code_rate][data_rate].pkt_blk_size_hi);

  /* Program PKT_PUNCT_LO for Packet G  */
  HWIO_OUT( DINT_PKT_CFG, 
           deint_ccch_packet_params[code_rate][data_rate].pkt_punct_lo);

  /* Program PKT_PUNCT_HI for Packet G */
  HWIO_OUT( DINT_PKT_CFG,
           deint_ccch_packet_params[code_rate][data_rate].pkt_punct_hi);
   
  /* Program PKT_PL_CRC_LO for Packet G */
  HWIO_OUT( DINT_PKT_CFG,
           deint_ccch_packet_params[code_rate][data_rate].pkt_pl_crc_lo);

  /* Program PKT_PL_CRC_HI for Packet G */
  HWIO_OUT( DINT_PKT_CFG,
             deint_ccch_packet_params[code_rate][data_rate].pkt_pl_crc_hi);

}


/*===========================================================================
FUNCTION DEINT_CREATE_CCCH_TASK_LIST()

DESCRIPTION
  This function creates a task for decoding a FCCCH message in the 
  first 5ms task slot.

DEPENDENCIES
  deint_load_ccch_pkt_params should have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
void deint_create_ccch_task_list(deint_ch_id_type chan_id, byte ncc_actchan_mask)
{

   /* create FCCCH task list in the first 5ms slot */
   
   SET_DINT_TASK_OFFSET (DINT_FIRST_5MS_SLOT_ADDR);
   
   /* Packet A contains the configuration parameters for the <code_rate> and
      <data_rate> combination so we select Packet A */
   /* Note First write is always to TASK_LIST_LO and the next 
      write is to TASK_LIST_HI */

   if (chan_id == DEINT_CH1)
   {
     HWIO_OUT(DINT_TASK_LIST, 
              DINT_TASK_LIST_ACTIVE_TASK_V | 
              DINT_TASK_LIST_CH1_V | DINT_TASK_LIST_FRM_HYP_20MS_V |
              DINT_TASK_LIST_DEC_INTR_EN_V | DINT_TASK_LIST_PACKET_G_V);
   }
   else
   {
   
     HWIO_OUT(DINT_TASK_LIST, 
            DINT_TASK_LIST_ACTIVE_TASK_V | 
            DINT_TASK_LIST_CH2_V | DINT_TASK_LIST_FRM_HYP_20MS_V |
            DINT_TASK_LIST_DEC_INTR_EN_V | DINT_TASK_LIST_PACKET_G_V);
   }

   
   HWIO_OUT(DINT_TASK_LIST, DEC_FCCCH_RAM_BASE );

   /* add a NULL task at the end */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   
   HWIO_OUT(DINT_TASK_LIST, 0x0000);

   if (ncc_actchan_mask == 0)
   {
     /* only clear slot 2 if there is no BCCH active */ 
     SET_DINT_TASK_OFFSET (DINT_SECOND_5MS_SLOT_ADDR);
     HWIO_OUT(DINT_TASK_LIST, 0x0000);   
     HWIO_OUT(DINT_TASK_LIST, 0x0000);
   }
  

    /* We also need to create ZERO tasks for the 3,4  5ms slot */

   SET_DINT_TASK_OFFSET (DINT_THIRD_5MS_SLOT_ADDR);
   
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task; end third 5ms */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);


   SET_DINT_TASK_OFFSET (DINT_FOURTH_5MS_SLOT_ADDR);
   
   HWIO_OUT(DINT_TASK_LIST, 0x0000);   // Create a ZERO Task; end 4th 5ms */
   HWIO_OUT(DINT_TASK_LIST, 0x0000);

}

/*==========================================================================
FUNCTION DEINT_SET_CCCH_MODE

DESCRIPTION
  This function puts the decoder in packet mode and resets the SVD. 
  This function loads the deinterleaver packet parameters for the 
  FCCCH.

DEPENDENCIES
  deint_init() should have been called. 

RETURN VALUE
  None

SIDE EFFECTS
   None.
   
=============================================================================*/
void deint_set_ccch_mode
(
  deint_code_rate_type ccch_code_rate,       /* R=1/4 or R=1/2 */
  dec_ccch_data_rate_type ccch_data_rate,   /* 9600, 19200 or 38400 */
  deint_ch_id_type chan_id,
  byte ncc_actchan_mask
)
{
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  /* Avoid resetting decoder while in traffic state.
  */
  curr_deint_state = DEINT_PAGING_IDLE_STATE;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
  if (chan_id == DEINT_CH1)
  {
    SET_DINT_CH_CFG (1, DINT_CH1_CFG__NEXT_20M_BND_MASK, 
                   DINT_CH1_CFG__NEXT_20M_BND_MASK);

    // IS-95-B SCCH's
    SET_DINT_CH_CFG (1, DINT_CH1_CFG__NUM_SUB_CHANS_MASK, 0x0000);

    // CCCH is always convolutional codingthere is no CH1_CODE_TYPE
    // i guess it shoould be convolutional, so we could use CH2_CODE_TYPE
    HWIO_OUTM(DINT_OTD_CFG,HWIO_FMSK(DINT_OTD_CFG,CH2_CODE_TYPE), 0x0000);   
  }
  else
  {
    SET_DINT_CH_CFG (2, DINT_CH2_CFG__NEXT_20M_BND_MASK, 
                   DINT_CH2_CFG__NEXT_20M_BND_MASK);

    // IS-95-B SCCH's
    SET_DINT_CH_CFG (2, DINT_CH2_CFG__NUM_SUB_CHANS_MASK, 0x0000);

    // CCCH is always convolutional coding
    HWIO_OUTM(DINT_OTD_CFG,HWIO_FMSK(DINT_OTD_CFG,CH2_CODE_TYPE), 0x0000);   
  }

   /* BCCH ought to have cleared the SYNC_CHAN_80_M_BND for Paging Channel 
    * Do it here JIC ??? */ 
  SET_DINT_CH_CFG (0, DINT_CH0_CFG__NEXT_20M_BND_MASK | 
                      DINT_CH0_CFG__SYNC_CHAN_80M_BND_MASK, 
                      DINT_NEXT_20M_BND_V);

  // IS-95-B SCCH's
  SET_DINT_CH_CFG (0, DINT_CH0_CFG__NUM_SUB_CHANS_MASK, 0x0000);

    /* 2 bank interleaving for IS2000 */
  HWIO_OUTM(DINT_OTD_CFG, HWIO_FMSK(DINT_OTD_CFG,OTD_SEL),
            HWIO_FMSK(DINT_OTD_CFG,OTD_SEL));   
   
  deint_load_ccch_pkt_params(ccch_code_rate, ccch_data_rate);
  deint_create_ccch_task_list(chan_id, ncc_actchan_mask);
}
#endif /* FEATURE_IS2000_REL_A_CC */

#ifdef FEATURE_DEINT_RAM_LOGGING
#error code not present
#endif /* FEATURE_DEINT_RAM_LOGGING */


#ifdef FEATURE_IS2000_REL_A
/*=========================================================================
FUNCTION deint_atmr_isr2_init

DESCRIPTION
  This function initializes variables used in deint_atmr_isr2().
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void deint_atmr_isr2_init (void)
{
  (void) memset (&sch_setup, 0, sizeof (sch_setup));
}

/*=========================================================================
FUNCTION deint_atmr_isr2

DESCRIPTION
  This function is invoked once a frame, at the 4th bin.  This function 
  support changes for 6100 release A Turbo SCH.  These changes are the result 
  of moving the Turbo SCH dummy task to the 3rd bin to give the Decoder more 
  time to decode 32X turbo data rate. 

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void deint_atmr_isr2 (void)
{
  if (sch_setup.this_frame) 
  {
    /* Update SCH booleans from save values.  Since this ISR happens on the
    ** 4th bin of a frame, the Traffic ISR has already happened for this frame
    ** thus the changes done here won't affect SCH data read operation for 
    ** this frame.
    */
    rxc_turbo_dec_enabled = sch_setup.turbo_dec_int;
    rxc_sup_dec_int_enable = sch_setup.sup_dec_int;

    if (rxc_turbo_dec_enabled == TRUE)
    {
      /* Here for Turbo SCH */

      /* Create Turbo dummy task entry */
      deint_create_dummy_sch_task_list();

      /* Delete Convolutional task entry in the 2nd bin, if any */
      SET_DINT_TASK_OFFSET (DINT_SECOND_5MS_SLOT_ADDR);
      HWIO_OUT(DINT_TASK_LIST, 0x0000);  
      HWIO_OUT(DINT_TASK_LIST, 0x0000);
    }
    else
    {
      /* Here for Convolutional SCH */

      /* Delete Turbo task entry in the 3rd bin, if any */
      SET_DINT_TASK_OFFSET (DINT_THIRD_5MS_SLOT_ADDR);
      HWIO_OUT(DINT_TASK_LIST, 0x0000);  
      HWIO_OUT(DINT_TASK_LIST, 0x0000);
    }
  }
 
  /* Reset SCH setup struct every frame */
  deint_atmr_isr2_init();

}
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================
FUNCTION DEINT_DELETE_NCCCH_TASK_LIST()

DESCRIPTION
  Depend on the input slot mask, this funtion deletes the corresponding
  deinterleaver task.

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
void deint_delete_nccch_task_list(byte slot_mask)
{
  if (slot_mask & 0x01)
  {
    SET_DINT_TASK_OFFSET (DINT_FIRST_5MS_SLOT_ADDR);
      /* Create a ZERO Task; end 1st 5ms */
  }
  else /*if (slot_mask & 0x02)*/
  {
    SET_DINT_TASK_OFFSET (DINT_SECOND_5MS_SLOT_ADDR);
      /* Create a ZERO Task; end 2nd 5ms */
  } 

  HWIO_OUT(DINT_TASK_LIST, 0x0000);   
  HWIO_OUT(DINT_TASK_LIST, 0x0000);  
}
#endif /* FEATURE_IS2000_REL_A_CC */
