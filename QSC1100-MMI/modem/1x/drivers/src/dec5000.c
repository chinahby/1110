 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         M O B I L E   D E C O D E R   ( S V D )    S E R V I C E S

GENERAL DESCRIPTION
  Library of routines to initialize, and operate on, the Qualcomm CDMA
  Serial Viterbi Decoder Chip.

EXTERNALIZED FUNCTIONS
 dec_init
   Initialize the circuit to Idle. Load the polynomials for  1/2, 1/3,
   1/4 code rate.
 dec_version
   Return the hardware version number for this circuit.
 dec_read_data
   Read data from the decoder output buffer.
 dec_sc_init
   Initialize the circuit for decoding the Sync Channel.
 dec_pc_init
   Initialize the circuit for decoding the Paging Channel.
 dec_tc_init
   Initialize the circuit for decoding the Traffic Channel.
 dec_read_sc_status
   Read the frame status and symbol error rates of sync channel  from the 
   Decoder.
 dec_read_pc_status
   Read the frame status and symbol error rates of Paging channel  from the 
   Decoder.
 dec_read_fch_status
   Read the frame status and symbol error rates of Fundamental channel  from the 
   Decoder Output Buffer.
 dec_read_scch_status
   Read the frame status and symbol error rates of Suplemental CODE Channel 
  (SCCH, IS95B) from the Decoder Output Buffer.
 dec_read_sch_status
   Read the frame status and symbol error rates of Suplemental Channel 
  (SCH, IS95C) from the Decoder Output Buffer.
 dec_read_sc_data
   Read data from the Decoder as Sync Channel data.
 dec_read_pc_data
   Read data from the Decoder as Paging Channel data.
 dec_read_fch_data
   Read data from the Decoder as Fundamental Traffic Channel data.
 dec_read_sch_data
   Read data from the Decoder as Supplemental Traffic Channel data 
  (SCH, IS95C).
 dec_read_scch_data
   Read data from the Decoder as Supplemental Traffic Channel data 
  (SCCH, IS95B).
 dec_clk_select
   Select between TCXO or Chipx8 for decoder clock.  

Copyright (c) 1999 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/src/dec5000.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/09   jtm     Cleaned up compiler warnings
02/23/09   jtm     Removed T_POSEIDON2 since this will only be supported 
                   in 1h09 branch
02/03/09   jtm     Added support for T_POSEIDON2
01/28/09   vlc     Moved Turbo decoder setting under MSMHW_TURBO_CODING.
11/12/08   vlc     Added changes specific to MSM7800 turbo decoder.
10/01/08   vlc/jtm Changed dec_ccch_init() to avoid stomping on HDR traffic in
                   simultaneous 1X/HDR mode. Removed T_QUASAR
07/16/08   jtm     Removed CCCH_DEBUG. Reduced debug string sizes to reduce ROM
                   for ULC effort.
07/01/08   vlc     Merged code changes from old tip archive.
04/10/08   trc     Switch clock mgmt strategy via FEATURE_SRCH_CLKREGIM_RM
03/31/08   mca     Changed FEATURE_CLKREGIM_RM to FEATURE_SRCH_CLKREGIM_RM
03/14/08   trc     Only reset VD & TD after turning ON clocks, not prior to
                   turning OFF clocks
03/05/08   vlc     User clock resource manager to manage RX clocks 
                   (FEATURE_CLKREGIM_RM.)
09/11/07   vlc     Added shift_bits_size in function dec_read_sch_data().
08/09/07   vlc     Modified the rel 0 branch of functions dec_read_sch_data()
                   and dec_read_sch_status() to fix 1x F-SCH high CRC failure.
12/21/06   trc     MSM7600 support, part II
10/17/06   trc     Add MSM7600 support
09/19/06   vlc     Fixed lint errors.
04/19/06   trc     Correct several enum types in dec_dtx_thresh_init()
04/19/06   vlc     Fixed compiler warning.
                   In function dec_read_mux_pdu5(), adjust DSM item size 
                   accordingly when pointer to data buffer is moved.
04/12/06   bn      Add code in dec_tc_init to re-initialize dec 1x in case HDR messed it up
01/03/05   rkc     Move FCCCH channel data from Long Buffer to an offset in the 
                   Short Buffer RAM in Decoder Output Buffer.  Offset since BCCH
                   also uses short buffer; move to short buffer since HDR uses
                   long buffer affecting simultaneous 1x/HDR.
08/01/05   grl      Turned off only viterbi decoder clk gating during PC.
04/21/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
02/28/05   bn/vlc  Modified function dec_read_mux_pdu5(), changed words_read
                   to words_need in memcpy statement.
02/28/05   bn/vlc  Merged code changes to fix MUX PDU5 decoder read function to
                   6700 archive.  Same code changes for earlier targets were done
                   in Mux layer.  The code was moved to Dec layer for 6700.
02/17/05   bn      Please featurize the use of clk_regime_sel_clk_src() for the decoder clock 
                   to not be called for T_MSM6700
01/26/05   vlc     Removed obsolete ds.h inclusion.
11/30/04   vlc     In function dec_read_mux_pdu5_sch_data(), start reading SCH
                   data from location 0 of decoder output buffer (long buffer.)
11/17/04   jyw     Removed the 0x1fff covering for the 1st scch status word
08/23/04   vlc     Switched source and dest ptrs for memcpy operations in
                   functions dec_read_mux_pdu5() and dec_read_sch_data().
07/30/04   jyw     Featurized the debuging msg for ccch
07/27/04   jyw     Fixed the status distance in dec_read_ccch_status
06/24/04   sr      Fixed TD CRC mask in dec_read_sch_status()
05/06/04   sr      Moved out DV driver code to separate file
09/10/03   dnn     Changed FEATURE_TC_FRAMES_INTEGRATION to
                   FEATURE_MSM6500_TC_FRAMES_INTEGRATION.
09/10/03   dnn     Added FEATURE_TC_FRAMES_INTEGRATION and its functionalities. 
09/03/03   dna     Ported for MSM6500
08/08/03   vlc     Added changes to move output buffer address of DCCH task to 
                   decouple FCH/DCCH rate sets.
10/22/02   vlc     In function dec_read_sch_status(), added SVD code under    
                   feature MSMHW_EXTENDED_ENERGY_MATRIX.
07/23/02   dlb2    Add EXTENDED_ENERGY_MATRIX support to CDMA2000 Rel. A code.
07/19/02   dlb2    Turn off DEM_MAC clock to save power except when in traffic.
06/27/02   dlb2    Add workaround for MSM6050 DEC1 problem.  Use new macros
                   defined in msm_drv.h to turn on/off dynamic clock gating.
06/17/02    rm     Support IS95B SCH for MSMHW_EXTENDED_ENERGY_MATRIX.
06/07/02   dlb2    Always use mask macros to write to VD_MODE_WB.
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/21/02   sr      Changed MSM_OUT to MSM_OUTM while programming VD_MODE reg
                   in dec_tc_init()
05/21/02   vlc     Removed rate from parameter list of dec_read_dcch_data ().
                   Removed SER scaling code from dec_read_dcch_status ().
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up 
                   (feature FEATURE_IS2000_REL_A_VP2.)
03/27/02   ems     Featurized dec_read_scch_data for MSMHW_EXTENDED_ENERGY_MATRIX
03/15/02   vlc     No longer adjust for bit shift in decoder read sch 
                   data/status functions (for FEATURE_IS2000_REL_A_SVD.)
02/25/02   sr      Merged in Release A common channels from the branch
02/07/02   dlb2    Added Decoder Dynamic Clock Gating feature for MSM6050.
01/23/02   dlb2    Added extended energy matrix feature for MSM6050.
11/16/01   vlc     Added function dec_clk_select ().  
09/12/01   bgc     Added control variables for dec_dtx_thresh_init().
08/15/01   sr      Fixed REL_A featurization
08/10/01   lh      Added 32X support.
08/07/01   hrk     Added DTX thresholds for RC5 turbo codes.
08/07/01   hrk     Removed parameter "code_type" from dec_dtx_thresh_init()
                   declaration.
06/01/01   hrk,snn Added support for DTX detection on RS2 SCH
05/25/01   sr      Changed OB address offset for RC5 1X in
                   dec_read_sch_data() from 2 words to 1 word
02/20/01   sr      Added support for 32X
02/06/01   sr      Changed OB address offset for RC5 1X in 
                   dec_read_sch_status() from 2 words to 1 word
01/10/01   tc      Defined FEATURE_PLT for dec_read_sch_data().
12/04/00   tc      Added decoder initailization delay.
10/24/00   rmd     Mergued with MSM5000 code.
09/13/00   bgc     Added DTX threshholds for RC 4 and made turbo threshholds
                   the same as convolutional in dec_dtx_thresh_init().
09/06/00   djd/et  Added ifdef MSMHW_TURBO_CODING for turbo coding software.
09/06/00   rm      Added New function dec_read_sch_data for FTM.
08/25/00   rc      Added support for handling NULL returns form
                   dsm_new_buffer().
08/11/00   rmd     replaced DEC_OB_DATA_WH with DEC_OB_DATA_RH, Hld shows 
                   this register to be write only but it is a read only register
07/12/00   rm      Added support for 5ms frames.
06/08/00   sr      Added #ifndef MSM5000_IRAM_FWD around:
                   dec_read_fch_data
                   dec_read_fch_status
                   dec_read_sch_data
                   dec_read_sch_status
05/22/00   bgc     Changed SCH 1X threshold to 225.
05/18/00   et      added function dec_dtx_thresh_init
05/11/00   rm      Added fixes for 1X SCH.
03/23/00   bgc     Scaled SER for compatibility to MSM3000 and changed 
                   energies to signed values.
01/28/00   rm      Added in support for 1X SCH.
01/26/00   rm      Modified the dec_read_sch_data fumction to read
                   each mux pdu into the buffer pointed by the dsm_item.
11/09/99   rm      Added in support to read Turbo decoder stats.
10/21/99   rm      Added in support for SCCH
09/24/99   rm      Added in support for SCH (RC3 & RC4).
08/11/99   rm      Misc. Changes after first phase of PLT testing.
07/31/99   rm      Created file. Interfaces to support Serial Viterbi Decoder

============================================================================*/

#include "dec.h"
#include "comdef.h"
#include "msm_drv.h"   
#include "msm.h"
#include "misc.h"
#include "err.h"
#include "msg.h"
#include "dsm.h"
#include "rxc.h"
#include "deint.h"
#include "clkregim.h"

#include "muxmdsp.h"
#include "bit.h"

#ifdef FEATURE_PLT 
#error code not present
#endif  /* FEATURE_PLT */
LOCAL word dec_pc_size;   /* Paging Channel Data rate size */

/*=============================================================================

The following table stores the offset address  in the Decoder Output Buffer
and decoded bits rate for FULL, HALF, QTR, 8TH rate hypothesis for FCH (RC1-RC5)

=============================================================================*/


const dec_fch_ob_params_type dec_fch_ob_params[] = {
{ 
 DEC_FCH_FULL_OB_ADDR_RC1, DEC_FCH_HALF_OB_ADDR_RC1, DEC_FCH_QTR_OB_ADDR_RC1,
 DEC_FCH_8TH_OB_ADDR_RC1, 
 DEC_FCH_SIZ_FULL_RC1, DEC_FCH_SIZ_HALF_RC1, 
 DEC_FCH_SIZ_QTR_RC1, DEC_FCH_SIZ_8TH_RC1  
},
{ 
 DEC_FCH_FULL_OB_ADDR_RC2, DEC_FCH_HALF_OB_ADDR_RC2, DEC_FCH_QTR_OB_ADDR_RC2,
 DEC_FCH_8TH_OB_ADDR_RC2, 
 DEC_FCH_SIZ_FULL_RC2, DEC_FCH_SIZ_HALF_RC2,
 DEC_FCH_SIZ_QTR_RC2, DEC_FCH_SIZ_8TH_RC2
},
{
 DEC_FCH_FULL_OB_ADDR_RC3, DEC_FCH_HALF_OB_ADDR_RC3, DEC_FCH_QTR_OB_ADDR_RC3,
 DEC_FCH_8TH_OB_ADDR_RC3, 
 DEC_FCH_SIZ_FULL_RC3, DEC_FCH_SIZ_HALF_RC3,
 DEC_FCH_SIZ_QTR_RC3, DEC_FCH_SIZ_8TH_RC3
},
{
 DEC_FCH_FULL_OB_ADDR_RC4, DEC_FCH_HALF_OB_ADDR_RC4, DEC_FCH_QTR_OB_ADDR_RC4,
 DEC_FCH_8TH_OB_ADDR_RC4, 
 DEC_FCH_SIZ_FULL_RC4, DEC_FCH_SIZ_HALF_RC4,
 DEC_FCH_SIZ_QTR_RC4, DEC_FCH_SIZ_8TH_RC4
},
{
 DEC_FCH_FULL_OB_ADDR_RC5, DEC_FCH_HALF_OB_ADDR_RC5, DEC_FCH_QTR_OB_ADDR_RC5,
 DEC_FCH_8TH_OB_ADDR_RC5, 
 DEC_FCH_SIZ_FULL_RC5, DEC_FCH_SIZ_HALF_RC5,
 DEC_FCH_SIZ_QTR_RC5, DEC_FCH_SIZ_8TH_RC5
}
};

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
const dec_dcch_ob_params_type dec_dcch_ob_params[] = 
{
  DEC_DCCH_SIZ_FULL_RC3, 
  DEC_DCCH_SIZ_FULL_RC4, 
  DEC_DCCH_SIZ_FULL_RC5 
};
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

const dec_sch_ob_params_type dec_sch_ob_params[NUM_SCH_RC][NUM_SCH_RATES] =  {


 /********SCH RC3 SIZE for 2X, 4X, 8X, 16X, 32X *********/
  { 
    DEC_SCH_SIZ_1X_RC3,
    DEC_SCH_SIZ_2X_RC3,
    DEC_SCH_SIZ_4X_RC3,
    DEC_SCH_SIZ_8X_RC3,
    DEC_SCH_SIZ_16X_RC3
#ifdef FEATURE_32X_SCH_DATA_RATE
    ,0
#endif /* FEATURE_32X_SCH_DATA_RATE */ 
  },

 /********SCH RC4 SIZE for 2X, 4X, 8X, 16X, 32X *********/
  {
    DEC_SCH_SIZ_1X_RC4,
    DEC_SCH_SIZ_2X_RC4,
    DEC_SCH_SIZ_4X_RC4,
    DEC_SCH_SIZ_8X_RC4,
    DEC_SCH_SIZ_16X_RC4
#ifdef FEATURE_32X_SCH_DATA_RATE
    ,DEC_SCH_SIZ_32X_RC4
#endif /* FEATURE_32X_SCH_DATA_RATE */ 
  }, 
 /********SCH RC5 SIZE for 2X, 4X, 8X, 16X, 32X *********/
  {
    DEC_SCH_SIZ_1X_RC5,
    DEC_SCH_SIZ_2X_RC5,
    DEC_SCH_SIZ_4X_RC5,
    DEC_SCH_SIZ_8X_RC5,
    DEC_SCH_SIZ_16X_RC5
#ifdef FEATURE_32X_SCH_DATA_RATE
    ,0
#endif /* FEATURE_32X_SCH_DATA_RATE */ 
  } 
}; 
     
/* Local Function Prototypes */
LOCAL void dec_read_ch_status_sb
(
  word  buffer_offset,
  byte *pstatus,
  word *pser,
  int4 *penergy
);

LOCAL void dec_read_ch_status_lb
(
  word  buffer_offset,
  byte *pstatus,
  word *pser,
  int4 *penergy,
  word *pltu_crc
);

#ifdef FEATURE_32X_SCH_DATA_RATE
static uint32 dec_temp_sdu_data_array[DEC_SCH_SIZ_32X_RC4 / 4];
#else
static uint32 dec_temp_sdu_data_array[DEC_SCH_SIZ_16X_RC4 / 4];
#endif /* FEATURE_32X_SCH_DATA_RATE */
static uint16  *dec_temp_sdu_data_ptr;


/*==========================================================================

FUNCTION DEC_INIT

DESCRIPTION
 Initialize the circuit to Idle. Load the polynomials for  1/2, 1/3,
 1/4 code rate.

DEPENDENCIES
  deint_init() calls dec_init()

RETURN VALUE
 None

SIDE EFFECTS
 None
      
============================================================================*/

void dec_init
(
  void
)
{
   /* Turn dynamic clock gating off before writing to the registers below */
   DEC_DYNAMIC_CLK_OFF();
   CLKRGM_DEC1_START();

#ifdef T_MSM7600
#error code not present
#else /* if !T_MSM7600 */
   HWIO_OUT(VD_TESTCON,0x00);
   HWIO_OUT(VD_POLY2IJ, 0x1c75);
   HWIO_OUT(VD_POLY3IJ, 0x2cb7);
   HWIO_OUT(VD_POLY3K, 0x64);
   HWIO_OUT(VD_POLY4IJ, 0x2e7a);
   HWIO_OUT(VD_POLY4KL, 0x0ea5);
   HWIO_OUT(VD_RESET, 0x00);
#endif /* !T_MSM7600 */

#ifdef MSMHW_TURBO_CODING
   HWIO_OUT(TD_RESET, 0x00);
#endif /* MSMHW_TURBO_CODING */

   CLKRGM_DEC1_END();
}

/*===========================================================================

FUNCTION DEC_VERSION

DESCRIPTION
  Return the hardware version number for this circuit.

DEPENDENCIES
  None

RETURN VALUE
  Returns the version number of the circuit

SIDE EFFECTS
  None

===========================================================================*/
byte dec_version
(
  void
)
{
  return 0x00;
}

/*=========================================================================*/
boolean
dec_read_data
(
  unsigned int outbuf_offset,
  void *buf_ptr,
  unsigned int num_bytes  /* Number of bytes to write to buf_ptr, in bytes */
)
{

  if( outbuf_offset < DEC_SCH_RAM_BASE )
  {
    /* Read from Short Buffer (QS): 16 bit wide */
    DECOB_SHORT_BUFFER_ADDR_SET( outbuf_offset );
    insh_rep ((void *) HWIO_ADDR(DECOB_SHORT_BUFFER_DATA), buf_ptr, (num_bytes + 1) / 2);
  }
  else
  {
    /* 
    ** Read from Long Buffer (QS): 32 bit wide; outbuf_offset is only used 
    ** to choose between Short and Long Buffer. For reading the offset always
    ** starts at 0. So if oubuf_offset is 160, which is typical, 
    ** outbuf_offset - DEC_SCH_RAM_BASE = 0, and you would start reading from
    ** the beginning of the Long Buffer, but the subtraction allows you to 
    ** start reading in the middle of the Long Buffer if you want.
    */
    DECOB_LONG_BUFFER_ADDR_SET( outbuf_offset - DEC_SCH_RAM_BASE );
    insw_rep ((void *) HWIO_ADDR(DECOB_LONG_BUFFER_DATA), buf_ptr, (word) ((num_bytes + 1) / 4));
  }

  return TRUE;
}


/*===========================================================================

FUNCTION DEC_SC_INIT

DESCRIPTION
  Initialize the circuit for decoding the Sync Channel.

DEPENDENCIES
  Dec_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  The decoder is put in continous mode.

===========================================================================*/

void dec_sc_init
(
  void
)
{
   /* Turn dynamic clock gating off before writing to the registers below */
   DEC_DYNAMIC_CLK_OFF();
   CLKRGM_DEC1_START();

   /* Put SVD in continous mode.*/
   /* For simulation purpose only initialize state 0 to 0 and saturate all other state metrics.For the real system,
    it doesn't matter since any initialization you do is likely wrong, since the sync channel is always present
    and you're always picking it up part way through a message. */

   HWIO_OUTM(VD_MODE,
            HWIO_FMSK(VD_MODE,PACKET) | VD_MODE_WB__INTMODE_MASK,
            VD_CONTINUOUS_V | VD_INITMODE_NO_INIT_V);

#ifdef T_MSM7600
#error code not present
#else /* if !T_MSM7600 */
   HWIO_OUT(VD_RESET, 0x00);
#endif /* !T_MSM7600 */

   CLKRGM_DEC1_END();

#ifdef MSMHW_MANAGED_DEM_MAC_CLK
   /* Turn off DEM_MAC clock during sync to save power */
   HWIO_OUTM(MISC_CLK_CTL,
             MISC_CLK_CTL_WW__DEM_MAC_CLK_SLEEP_MASK,
             MISC_CLK_CTL_WW__DEM_MAC_CLK_SLEEP_ENA);
#endif
}

/*===========================================================================

FUNCTION DEC_PC_INIT

DESCRIPTION
  Initialize the circuit for decoding the Paging Channel.

DEPENDENCIES
  Dec_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  The decoder is put in continous mode.

===========================================================================*/
void dec_pc_init
(
  dec_pc_rate_type pc_rate /* Indicates the Paging Cgannel rate */
)
{
  switch (pc_rate) {
    case DEC_PRAT_9600:
      dec_pc_size = DEC_192_BITS / 16;
      break;
    case DEC_PRAT_4800:
      dec_pc_size = DEC_96_BITS / 16;
      break;
    default:
      ERR_FATAL("Bad Pg Ch Rate", 0,0,0);

   }

   /* Turn dynamic clock gating off before writing to the registers below */
   DEC_DYNAMIC_CLK_SVD_OFF();
   CLKRGM_DEC1_START();

     /* Put SVD in continous mode.*/
   /* For simulation purpose only initialize state 0 to 0 and saturate all other state metrics */
   HWIO_OUTM(VD_MODE,
            HWIO_FMSK(VD_MODE,PACKET) | VD_MODE_WB__INTMODE_MASK,
            VD_CONTINUOUS_V | VD_INITMODE_NO_INIT_V);

#ifdef T_MSM7600
#error code not present
#else /* if !T_MSM7600 */
   HWIO_OUT(VD_RESET, 0x00);
#endif /* !T_MSM7600 */


   CLKRGM_DEC1_END();
   /* Turn dynamic clock gating on for the paging channel */
   DEC_DYNAMIC_CLK_SVD_ON();

#ifdef MSMHW_MANAGED_DEM_MAC_CLK
   /* Turn off DEM_MAC clock during paging to save power */
   HWIO_OUTM(MISC_CLK_CTL,
             MISC_CLK_CTL_WW__DEM_MAC_CLK_SLEEP_MASK,
             MISC_CLK_CTL_WW__DEM_MAC_CLK_SLEEP_ENA);
#endif
}

/*===========================================================================

FUNCTION DEC_TC_INIT

DESCRIPTION
  Initialize the circuit for decoding the Traffic Channel.

DEPENDENCIES
  Dec_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_MSM6500_TC_FRAMES_INTEGRATION
word org_frame_offset; /* Used in rxctraffic.c rxc_tc_isr() */
#endif 
void dec_tc_init
(
  void
)
{
#ifdef FEATURE_MSM6500_TC_FRAMES_INTEGRATION
     /* Save the frame offset so we have it if we need to re-program 
      * DEM1X_FRAME_OFFSET in the rxc_tc_isr()
      */
     org_frame_offset = HWIO_IN(DEM1X_FRAME_OFFSET);
#endif 
#ifdef MSMHW_MANAGED_DEM_MAC_CLK
   /* Turn on DEM_MAC clock for traffic */
     HWIO_OUTM(MISC_CLK_CTL,
               MISC_CLK_CTL_WW__DEM_MAC_CLK_SLEEP_MASK,
               MISC_CLK_CTL_WW__DEM_MAC_CLK_SLEEP_DIS);
#endif

     /* Turn dynamic clock gating off before writing to the registers below */
     DEC_DYNAMIC_CLK_OFF();
     CLKRGM_DEC1_START();

    /* PUT SVD In Packet Mode */
     HWIO_OUTM(VD_MODE,
              HWIO_FMSK(VD_MODE,PACKET) | VD_MODE_WB__INTMODE_MASK,
              VD_PACKET_V | VD_INITMODE_SATURATE_STATE_V);

#ifdef T_MSM7600
#error code not present
#else /* if !T_MSM7600 */
   HWIO_OUT(VD_RESET, 0x00);
#endif /* !T_MSM7600 */

#ifdef MSMHW_TURBO_CODING
     HWIO_OUT(TD_RESET, 0x00);
#endif /* MSMHW_TURBO_CODING */

#if defined(T_MSM7600)
#error code not present
#else /* if !T_MSM7600 */
     /* symbol should be sent to 1x-deint */
     HWIO_FL_SYM_BUFF_MODE_OUT( 0x0 );
#endif /* !T_MSM7600 */

     /* access the decode by using micro interface for 1x */
     HWIO_OUT(DECOB_BURST_IF_ACCESS, 0x0);
     /* Allow the micro to control the decoder for 1x mode */

#ifdef MSMHW_TURBO_CODING
#ifdef T_MSM7800
#error code not present
#else
     HWIO_OUT(TD_MODE_CTL,0x0);
#endif /* T_MSM7800 */
#endif /* MSMHW_TURBO_CODING */

     CLKRGM_DEC1_END();
     /* Turn dynamic clock gating on for the traffic channel */
     DEC_DYNAMIC_CLK_ON();
}

/*===========================================================================

FUNCTION DEC_READ_SC_DATA

DESCRIPTION
  Read data from the Decoder as Sync Channel data.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dec_read_sc_data
(
  dec_sc_type   buf             /* Buffer for Sync Channel data */
)
{
  DECOB_SHORT_BUFFER_ADDR_SET( 0x0000 );  
  insh_rep( (void *) HWIO_ADDR(DECOB_SHORT_BUFFER_DATA), buf, (sizeof(dec_sc_type) / 2));

} /* end of dec_read_sc_data */

/*===========================================================================

FUNCTION DEC_READ_PC_DATA

DESCRIPTION
  Read data from the Decoder Output Buffer as Paging Channel data.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dec_read_pc_data
(
  dec_pc_type buf
)
{
  DECOB_SHORT_BUFFER_ADDR_SET( 0x0000 );
  insh_rep( (void *) HWIO_ADDR(DECOB_SHORT_BUFFER_DATA), buf, dec_pc_size);

}
 
/*===========================================================================

FUNCTION DEC_READ_FCH_DATA

DESCRIPTION
  Read data from the Decoder as Fundamental(Traffic) Channel data.  Note that the data
  for the specified rate is loaded from the beginning of the buffer.  Any
  space at the end of the buffer, which was not requested to be filled,
  is indeterminant.

DEPENDENCIES
  Dec_tc_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/              

void dec_read_fch_data
(
  dec_fch_type      *buf_ptr,    /* Buffer for Traffic Channel data      */
  dec_rc_type       rc,
  dec_fch_rate_type fch_rate     /* Rate decode buffer to read           */
)
{
  if((fch_rate == DEC_RATE_ALL) || (fch_rate == DEC_RATE_FULL))
  {
   /* Read Full Rate Data first */ 
    DECOB_SHORT_BUFFER_ADDR_SET( (dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].
             dec_ob_addr_full_rate) / 2);
    insh_rep( (void *) HWIO_ADDR(DECOB_SHORT_BUFFER_DATA), buf_ptr->buf.bits1, 
               (dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].
                dec_full_rate_siz) / 2);
  }
   
   /* Read Half Rate Data */ 
  if((fch_rate == DEC_RATE_ALL) || (fch_rate == DEC_RATE_HALF))
  {
    DECOB_SHORT_BUFFER_ADDR_SET( (dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].
             dec_ob_addr_half_rate) / 2);
    insh_rep( (void *) HWIO_ADDR(DECOB_SHORT_BUFFER_DATA), buf_ptr->buf.bits2,
            (dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].dec_half_rate_siz) / 2);
  }
   
   /* Read Quarter Rate Data */ 
  if((fch_rate == DEC_RATE_ALL) || (fch_rate == DEC_RATE_QTR))
  {
    DECOB_SHORT_BUFFER_ADDR_SET( (dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].
            dec_ob_addr_qtr_rate) / 2);
    insh_rep( (void *) HWIO_ADDR(DECOB_SHORT_BUFFER_DATA), buf_ptr->buf.bits4,
            (dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].dec_qtr_rate_siz) / 2);
  } 
   /* Read Eighth Rate Data */
   
  if((fch_rate == DEC_RATE_ALL) || (fch_rate == DEC_RATE_8TH))
  {
     
    DECOB_SHORT_BUFFER_ADDR_SET( (dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].
             dec_ob_addr_8th_rate) / 2);
    insh_rep( (void *) HWIO_ADDR(DECOB_SHORT_BUFFER_DATA), buf_ptr->buf.bits8,
            (dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].
               dec_8th_rate_siz) / 2 );
  }     
}

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/*===========================================================================

FUNCTION DEC_READ_DCCH_DATA

DESCRIPTION
  Read data from the Decoder as DCCH data.  

DEPENDENCIES
  Dec_tc_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/              

void dec_read_dcch_data
(
  dec_fch_type      *buf_ptr,    /* Buffer for Traffic Channel data      */
  dec_rc_type       rc           /* Radio configuration                    */                                    
)
{
  /* Read Full Rate Data */ 
  DECOB_SHORT_BUFFER_ADDR_SET( (DEC_DCCH_FULL_OB_ADDR / 2));
  insh_rep 
    (
     (void *) HWIO_ADDR(DECOB_SHORT_BUFFER_DATA), 
     buf_ptr->buf.bits1dcch,
     ((dec_dcch_ob_params[DEC_DCCH_RC_INDEX(rc)]) / 2)
    );

} /* end dec_read_dcch_data */
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_PLT
#error code not present
#else

/*===========================================================================

FUNCTION DEC_READ_SCH_DATA

DESCRIPTION
  Read data from the Decoder as Supplemental(Traffic) Channel data (IS95C SCH).
  This function reads each mux pdu (single/double) into the buffer pointed
  to by the dsm_item.
DEPENDENCIES
  Dec_tc_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dec_read_sch_data
(
  dec_sch_cfg_type *sch_rec_ptr,  /* Pointer to rxc_sch record pointer */
  dsm_item_type **dsm_item_ptr,   /* array of dsm_item pointers */
  byte *mux_pdu_count             /* return the number of MUX PDU read */
)

{

  word dec_sch_ob_addr; 
  /* dec_sch_ob_addr points to the address of the sch data and status in
     the decoder output buffer */
  byte double_mux_pdu_size=0;
  /* Indicates the size of a double sized mux pdu of a given RC */
  byte dsm_item_index=0;
  byte cnt;
  /* For 1X rate, the shifted bits need to be accounted for */
  byte shift_bits_size = 0;
  word double_mux_pdu_cnt; 
  uint16  *pdu3_data_ptr;

  /* Indicates the number of double sized mux pdu. If the mux pdu type
     is single then the total number of mux pdu's is 2 times of double_mux_pdu */

  /* dec_sch_size indicates the number of bytes for the RC and the given 
     SCR Rate */
  word dec_sch_size =  dec_sch_ob_params[DEC_SCH_RC_INDEX(sch_rec_ptr->rc)]
                                         [sch_rec_ptr->sch_rate]; 

  if((sch_rec_ptr->rc == DEC_RC3) || (sch_rec_ptr->rc == DEC_RC4))
  {
    double_mux_pdu_size = 44;  /* 352 bits (8bits header + 344 bits payload) */
  }
  else if(sch_rec_ptr->rc == DEC_RC5)
  {
     double_mux_pdu_size = 68;
  }
  else
  {
    ERR_FATAL("Bad Radio Config %d", sch_rec_ptr->rc,0,0);
  }

  dec_sch_ob_addr =  0; /* In MSM6700 Long Buffer Read always starts at 0 */

#ifndef FEATURE_IS2000_REL_A_SVD
  /* Special case for non-Rel A. If SCH Rate = 1X, the shifted bits are stored 
  ** in a separate 32bit word. 
  */
  if(sch_rec_ptr->sch_rate == DEC_SCH_RATE_1X)
  {
    shift_bits_size = 4;
  }
#endif /* !FEATURE_IS2000_REL_A_SVD */

  /* for MSM6700 or above chip, read the whole SDU into a temp array
  we could use the burst-8 interface for faster reading */
  DECOB_LONG_BUFFER_ADDR_SET(dec_sch_ob_addr);
  DECOB_LONG_BUFFER_DATA_GET(dec_temp_sdu_data_array,(dec_sch_size + shift_bits_size + 3)/4);
  dec_temp_sdu_data_ptr = (uint16 *) dec_temp_sdu_data_array;
  
  /* Special case if SCH Rate = 1X */
  if(sch_rec_ptr->sch_rate == DEC_SCH_RATE_1X)
  {
    if (dsm_item_ptr[dsm_item_index] != NULL)
    {
      if((sch_rec_ptr->rc == DEC_RC3) || (sch_rec_ptr->rc == DEC_RC4) 
        || (sch_rec_ptr->rc == DEC_RC5))
      {
#ifdef FEATURE_IS2000_REL_A_SVD
        /* Do not adjust the output buffer address since no shifting is
        ** done to preserve the MM bit.
        **
        ** This change also affects the deinterleaver programming and the 
        ** processing of rx frames in the rxdemux layer.
        */
        pdu3_data_ptr = dec_temp_sdu_data_ptr;
#else 
        /* For RC3 & RC4 there is MM bit. For RC5 there is MM bit and EIB bit, 
        ** so we shift by 2 bits.  In either case the shifted data is offset by
        ** 1 32 bit word in the output buffer.  Since pdu3_data_ptr is word 
        ** aligned, add 2 words (32 bits) to the OB address.
        */
        pdu3_data_ptr = dec_temp_sdu_data_ptr + 2;
#endif /* FEATURE_IS2000_REL_A_SVD */
        
        dsm_item_ptr[dsm_item_index]->used = double_mux_pdu_size/2; /* bytes */
        /* copy single side PDU 3, size = double_mux_pdu_size /2 */
        memcpy(dsm_item_ptr[dsm_item_index++]->data_ptr, pdu3_data_ptr, double_mux_pdu_size/2);
        pdu3_data_ptr = pdu3_data_ptr + double_mux_pdu_size/4;
      }
      *mux_pdu_count=1;
    }
    else
    {
      ERR("RXC has too few DSM items!!!",0,0,0);
      *mux_pdu_count=0;
    }
    return;    
  }

    
  double_mux_pdu_cnt = dec_sch_size / double_mux_pdu_size;
 
  for(cnt=0; cnt < double_mux_pdu_cnt; cnt++)
  {
    if (dsm_item_ptr[dsm_item_index] != NULL)
    {
      pdu3_data_ptr = dec_temp_sdu_data_ptr + dec_sch_ob_addr;
      if(sch_rec_ptr->mux_pdu_double)
      {
        /* Double Sized MUX PDU  Option */
        dsm_item_ptr[dsm_item_index]->used = double_mux_pdu_size;
        /* copy double-sided PDU 3, size = double_mux_pdu_size*/
        memcpy(dsm_item_ptr[dsm_item_index++]->data_ptr, pdu3_data_ptr, double_mux_pdu_size);
      }
      else
      {
        /* Single sized MUX PDU Option */
        /* Read 2 single mux pdu */
           
        dsm_item_ptr[dsm_item_index]->used = (double_mux_pdu_size / 2);
        /* copy the first single-sided PDU 3, size = double_mux_pdu_size / 2*/
        memcpy(dsm_item_ptr[dsm_item_index++]->data_ptr, pdu3_data_ptr, double_mux_pdu_size/2);

        pdu3_data_ptr = pdu3_data_ptr + double_mux_pdu_size / 4;
       
        dsm_item_ptr[dsm_item_index]->used = (double_mux_pdu_size / 2);
        /* copy the second single-sided PDU 3, size = double_mux_pdu_size / 2*/
        memcpy(dsm_item_ptr[dsm_item_index++]->data_ptr, pdu3_data_ptr, double_mux_pdu_size/2);

      }
    
      dec_sch_ob_addr += (double_mux_pdu_size / 2);
      if(sch_rec_ptr->code_type == DEC_CONVOLUTIONAL)
      {
         /* Convolutional codes have LTU (16 bits of CRC) */
         /* Skip over 1 word (1word = 16 bits) */
         dec_sch_ob_addr += 1;
      }
        /* dec_sch_ob_addr now points to the next mux pdu */
    }
    else
    {
      ERR("RXC - too few DSM items",0,0,0);
      break; /* get out of this loop */
    }
  } /* for */
  *mux_pdu_count = dsm_item_index; /* Total number of mux pdu's */
}   
#endif /* FEATURE_PLT */


/*===========================================================================

FUNCTION DEC_READ_SCCH_DATA

DESCRIPTION
  Read data from the Decoder as Supplemental(Traffic) Channel data 
 (IS95B SCCH). This function also reads the status information
  (CRC, SER and Energy Metrics) for each SCCH.

DEPENDENCIES
  Dec_tc_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dec_read_scch_data
(
  dec_rc_type       rc,        /* RADIO CONFIGURATION */
  dword             *buf_ptr,  /* Buffer for Traffic Channel data      */
  uint16            *scch_ser,
  int32             *scch_energy,
  byte              *scch_crc, 
  byte              num_sup    /* Number of SCCH */  
)
{ 
  
  byte crc=0x00;
  word dec_scch_ob_addr; 
  /* dec_scch_ob_addr is the pointer to the decoder output buffer */
  byte scch_payload;  /* In 32 bit units */
  byte sup_index;
  dword temp_status;

  scch_payload = 0;

  dec_scch_ob_addr = 0 + 1; /* In MSM6700 Long Buffer Read always starts at 0 */ 
  /* we have inremented dec_scch_ob_addr by 1 because of bit shifting of MM/
   EIB bit done by the hw */
  if(rc == DEC_RC1)
  { 
    scch_payload = DEC_SCCH_RC1_PAYLOAD_SIZE; 
  /* Payload for RC1 is (192 - 12 - 8 - 1) bits = 171 bits = ~ 11 half words*/
  }
  else if(rc == DEC_RC2)
  {  
    scch_payload = DEC_SCCH_RC2_PAYLOAD_SIZE;    
  /* Payload for RC2 is (288 - 12 - 8 - 2) bits = 266 bits = ~ 17 half words*/
  }
  else
  {
     MSG_ERROR("Bad SCCH RC %d", rc, 0,0);
  }

  /* Convert payload size to word32 units for MSM6700 */
  scch_payload = (scch_payload + 1) >> 1;

  for(sup_index=0; sup_index < num_sup; sup_index++)
  {
    DECOB_LONG_BUFFER_ADDR_SET( dec_scch_ob_addr );  
    insw_rep((void *) HWIO_ADDR(DECOB_LONG_BUFFER_DATA), buf_ptr, scch_payload);
    buf_ptr+=scch_payload;
    dec_scch_ob_addr = (dec_scch_ob_addr + scch_payload);
    /* At this time dec_scch_ob_addr points to the first word of status info
       for Sup channel (sup_index) */
    
    DECOB_LONG_BUFFER_ADDR_SET( dec_scch_ob_addr); 

    /* Read first status word */
    temp_status = in_dword((dword *) HWIO_ADDR(DECOB_LONG_BUFFER_DATA)) ;

    scch_ser[sup_index] = (word) ((temp_status & 0x1FFF0000) >> 16);
    scch_energy[sup_index] = (int4) (temp_status & 0x0000FFFF);

    /* Read second status word */
    temp_status = in_dword(( dword *) HWIO_ADDR(DECOB_LONG_BUFFER_DATA));
     
    scch_energy[sup_index] |= (int4) (temp_status & 0x000F0000);
    if( ((temp_status & 0x00400000) >> 22) )
    {
      crc = (1 << sup_index);
      *scch_crc = *scch_crc | crc;
    }
    /* Incement the ponter to OB addess because we have read 2 words
    of SCCH status for each channel */
    dec_scch_ob_addr = dec_scch_ob_addr + 2;

    dec_scch_ob_addr++; /* and once more to skip over bit shift word of next 
                         ** SCCH channel */
    
  }
   
}

/*=========================================================================
FUNCTION DEC_READ_SC_STATUS

DESCRIPTION
  This function reads the Status (SER, Energy Metrics) for Sync Channel frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

void dec_read_sc_status
(
  dec_sc_status_type *sc_status 
  /* Pointer the buffer where sync channel decoding status will be stored */
)
{
  dec_read_ch_status_sb (DEC_SC_BUF_SIZ / 2,
                      &sc_status->status,
                      &sc_status->ser,
                      &sc_status->energy);
}
 
/*=========================================================================
FUNCTION DEC_READ_PC_STATUS

DESCRIPTION
  This function reads the Status (SER, Energy Metrics) for Paging Channel frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

void dec_read_pc_status
(
  dec_pc_status_type *pc_status
  /* Pointer the buffer where paging channel decoding status will be stored */
) 
{
  dec_read_ch_status_sb (dec_pc_size,
                      &pc_status->status,
                      &pc_status->ser,
                      &pc_status->energy);

}

/*=========================================================================
FUNCTION DEC_READ_FCH_STATUS

DESCRIPTION
  This function reads the Status (CRC, SER, Energy Metrics) for 
  Fundamental Channel frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

void dec_read_fch_status
(
  dec_rc_type rc, 
  /* Indicates the Radio Configuration for FCH */
  dec_fch_status_type *fch_status
  /* Pointer the buffer where FCH decoding status will be stored */
)

{
#ifdef FEATURE_PLT 
#error code not present
#endif  /* FEATURE_PLT */

  int count;    

  /* To calculate the offset to the status information for each buffer, take */
  /* the starting address of the next buffer, divide by 2 to convert a byte  */
  /* offset to a word offset, then subtract the size of the status           */
  /* information.                                                            */

  /* Read FCH Status corresponding to FULL RATE HYPOTHESIS */
  count = DEC_RATE_FULL - 1;   
  dec_read_ch_status_sb (((dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].
                      dec_ob_addr_half_rate - DEC_FCH_STATUS_SIZE) / 2),
                      &(fch_status->status[count]),
                      &(fch_status->ser   [count]),
                      &(fch_status->energy[count]));

  /* Read FCH Status corresponding to HALF RATE HYPOTHESIS */
  count++;
  dec_read_ch_status_sb (((dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].
                      dec_ob_addr_qtr_rate - DEC_FCH_STATUS_SIZE) / 2),
                      &(fch_status->status[count]),
                      &(fch_status->ser   [count]),
                      &(fch_status->energy[count]));
  
  /* Read FCH Status corresponding to QTR RATE HYPOTHESIS */
  count++;
  dec_read_ch_status_sb (((dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].
                      dec_ob_addr_8th_rate - DEC_FCH_STATUS_SIZE) / 2),
                      &(fch_status->status[count]),
                      &(fch_status->ser   [count]),
                      &(fch_status->energy[count]));
  
  /* Read FCH Status corresponding to 8TH RATE HYPOTHESIS */
  count++;
  dec_read_ch_status_sb ((((dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].
                      dec_ob_addr_8th_rate) / 2 ) +
                      (dec_fch_ob_params[DEC_FCH_RC_INDEX(rc)].dec_8th_rate_siz / 2)),
                      &(fch_status->status[count]),
                      &(fch_status->ser   [count]),
                      &(fch_status->energy[count]));

   /* Scale the ser in software for MSM5000 (this was done in hardware on
      the MSM3000) */
   fch_status->ser[DEC_RATE_HALF - 1] *= 2;
   fch_status->ser[DEC_RATE_QTR - 1] *= 4;
   fch_status->ser[DEC_RATE_8TH - 1] *= 8;
   
#ifdef FEATURE_PLT 
#error code not present
#endif  /* FEATURE_PLT */ 
}

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/*=========================================================================
FUNCTION DEC_READ_DCCH_STATUS

DESCRIPTION
  This function reads the Status (CRC, SER, Energy Metrics) for 
  DCCH frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

void dec_read_dcch_status
(
  dec_rc_type rc, 
  /* Indicates the Radio Configuration for DCCH */
  dec_dcch_status_type *dcch_status
  /* Pointer to the struct where DCCH decoding status will be stored */
)

{
  dec_read_ch_status_sb 
    (
     ((DEC_DCCH_FULL_OB_ADDR + (dec_dcch_ob_params[DEC_DCCH_RC_INDEX(rc)])) / 2),
     &dcch_status->status,
     &dcch_status->ser,
     &dcch_status->energy
    );
  
} /* end dec_read_dcch_status */
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

/*=========================================================================
FUNCTION DEC_READ_SCH_STATUS

DESCRIPTION
  This function reads the Status (SER, Energy Metrics) for SCH Channel frames

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

void dec_read_sch_status
(
#ifdef FEATURE_IS2000_REL_A // 32X
  dec_mux_pdu_type_type pdu_type,
#endif
  dec_rc_type sch_rc, 
   /* Indicates the RC for SCH */
  dec_sch_rate_type sch_rate, 
  /* Indicates the Rate Type (2X/4X/8X/16X for SCH */
  dec_sch_status_type *sch_status, 
  dec_coding_type code_type
  /* Indicates the Code type (Convolutional or TURBO */
)

{
  word buffer_offset = 0;
  
  if
  ( 
    ( sch_rate == DEC_SCH_RATE_1X) 
#ifdef FEATURE_IS2000_REL_A // 32X
    && ( pdu_type != DEC_MUX_PDU_TYPE_5 ) 
#endif
  )
  {
    if((sch_rc == DEC_RC3) || (sch_rc == DEC_RC4) || (sch_rc == DEC_RC5))
    {
#ifdef FEATURE_IS2000_REL_A_SVD
      /* Do not adjust the output buffer address since no shifting is
      ** done to preserve the MM bit.
      **
      ** This change also affects the deinterleaver programming and the 
      ** processing of rx frames in the rxdemux layer.
      */
      buffer_offset = ((dec_sch_ob_params[DEC_SCH_RC_INDEX(sch_rc)][sch_rate]
                      / 4) + 0); /* In MSM6700 Long Buffer Read always starts 
                                    at 0 and it is 32 bit wide */  
     
#else 
      buffer_offset = 
        (dec_sch_ob_params[DEC_SCH_RC_INDEX(sch_rc)][sch_rate] + 4) / 4;
                                 /* In MSM6700 Long Buffer Read always 
                                    starts at 0 and it is 32 bit wide */ 
      
#endif /* FEATURE_IS2000_REL_A_SVD */
      
    }
    else
    {
      ERR_FATAL("Bad Radio Config %d", sch_rc,0,0);
    }
  }
  else
  {

    buffer_offset = ((dec_sch_ob_params[DEC_SCH_RC_INDEX(sch_rc)][sch_rate]
                      / 4) + 0); /* In MSM6700 Long Buffer Read always starts 
                                    at 0 and it is 32 bit wide */
  }
       
  if(code_type == DEC_CONVOLUTIONAL)
  {
    dec_read_ch_status_lb (buffer_offset, 
                        &sch_status->conv_sch_status.status,
                        &sch_status->conv_sch_status.ser,
                        &sch_status->conv_sch_status.energy,
                        &sch_status->conv_sch_status.ltu_crc);
  }

  else 
  {
#ifdef MSMHW_TURBO_CODING
    dword temp_status;
#endif

    DECOB_LONG_BUFFER_ADDR_SET( buffer_offset );

#ifdef MSMHW_TURBO_CODING
    /* Code Type = TURBO */ 
    /* Read the Turbo Decoder status */
    temp_status = in_dword((dword*) HWIO_ADDR(DECOB_LONG_BUFFER_DATA));
#ifdef T_MSM7800
#error code not present
#else
    sch_status->turbo_sch_status.energy = (int4) (temp_status & 0x0007ffff);
    if( temp_status & 0x00080000 )   /* Crc = bit 19 */
    {
      sch_status->turbo_sch_status.crc_pass = TRUE;
    }
    else
    {
      sch_status->turbo_sch_status.crc_pass = FALSE;
    }

    temp_status = in_dword((dword*) HWIO_ADDR(DECOB_LONG_BUFFER_DATA));
    sch_status->turbo_sch_status.min_llr = temp_status & 0x000007ff;
    sch_status->turbo_sch_status.num_iteration = 
                                               (temp_status & 0x0001f800) >> 11;
#endif /* T_MSM7800 */
#endif /* MSMHW_TURBO_CODING */
  }                  
}

/*=========================================================================
FUNCTION DEC_DTX_THRESH_INIT

DESCRIPTION
  This function initializes the dtx threshold value depending on the RC and
  the code type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Writes into the MSM register DEFAULT_IQ_ACC_WH
=============================================================================*/
/* Engineering control variables that allow tweaks for testing */
/*lint -e552 called from rxctraffic.c */
word dec_dtx_thresh;
byte dec_dtx_debug=0;

void dec_dtx_thresh_init(dec_rc_type rc, dec_coding_type code_type,
                         dec_sch_rate_type rate)
{
  if(dec_dtx_debug == 0)
  {
    switch(rc)
    {
      case DEC_RC3:
      {
        switch(rate)
        {
          case DEC_SCH_RATE_1X:
          {
            dec_dtx_thresh = 225;
            break;
          }
          case DEC_SCH_RATE_2X:
          {
            dec_dtx_thresh = 464;
            break;
          }
          case DEC_SCH_RATE_4X:
          {
            dec_dtx_thresh = 903;
            break;
          }
          case DEC_SCH_RATE_8X:
          {
            dec_dtx_thresh = 1804;
            break;
          }
          case DEC_SCH_RATE_16X:
          {
            dec_dtx_thresh = 3569;
            break;
          }
        }
        break;
      }
      case DEC_RC4:
      {
        switch(rate)
        {
          case DEC_SCH_RATE_1X:
          {
            dec_dtx_thresh = 124;
            break;
          }
          case DEC_SCH_RATE_2X:
          {
            dec_dtx_thresh = 236;
            break;
          }
          case DEC_SCH_RATE_4X:
          {
            dec_dtx_thresh = 461;
            break;
          }
          case DEC_SCH_RATE_8X:
          {
            dec_dtx_thresh = 906;
            break;
          }
          case DEC_SCH_RATE_16X:
          {
            dec_dtx_thresh = 1769;
            break;
          }
#ifdef FEATURE_32X_SCH_DATA_RATE
          case DEC_SCH_RATE_32X:
          {
            dec_dtx_thresh = 1769*2;
            break;
          }
#endif /* FEATURE_32X_SCH_DATA_RATE */ 
        }
        break;
      }
      case DEC_RC5:
      {
        switch (rate)
        {
          case DEC_SCH_RATE_1X:
          {
            dec_dtx_thresh = 243;
            break;
          }
          case DEC_SCH_RATE_2X:
          {
            dec_dtx_thresh = 461;
            break;
          }
          case DEC_SCH_RATE_4X:
          {
            dec_dtx_thresh = 893;
            break;
          }
          case DEC_SCH_RATE_8X:
          {
            dec_dtx_thresh = 1805;
            break;
          }
        } /* switch */
        break;
      }
      default:
      {
        ERR_FATAL( "Bad RC %d", rc, 0, 0  );
      }
    } /* switch rc */
  } /* if (dec_dtx_debug) */
}

#ifdef FEATURE_SRCH_CLKREGIM_RM
/*============================================================================

FUNCTION DEC_SVD_CLK_ENABLE

DESCRIPTION 
  Enable clocks for DEC and SVD blocks for CDMA operations.

DEPENDENCIES
  DEC clock regimes should have been initialized.

RETURN VALUE 
  None

SIDE EFFECTS
  Enable SVD decoder clocks
=============================================================================*/
void dec_svd_clk_enable (void)
{
  /* Check if SVD clock is already on */
  boolean clk_already_on = clk_regime_is_on(CLK_RGM_SVD_M);

  /* Enable decoder clocks. */
  MSG_HIGH ("Enable Decoder and SVD clocks (reset=%d)",!clk_already_on,0,0);
  clk_regime_resource_enable (CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_CDMA_DEC);
  clk_regime_resource_enable (CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_CDMA_VD);

  /* Reset the decoder if its clock was off previously */
  if (!clk_already_on)
  {
#if defined(T_MSM7600)
#error code not present
#else /* if !T_MSM7600 */
    HWIO_OUT(VD_RESET, 0x00);
#endif /* !T_MSM7600 */
  }

} /* end dec_svd_clk_enable() */

/*============================================================================

FUNCTION DEC_SVD_CLK_DISABLE

DESCRIPTION 
  Disable clocks for DEC and SVD blocks when not in use.

DEPENDENCIES
  DEC clock regimes should have been initialized.

RETURN VALUE 
  None

SIDE EFFECTS
  Disable SVD decoder clocks
=============================================================================*/
void dec_svd_clk_disable (void)
{
  /* Disable decoder clocks. */
  MSG_HIGH ("Disable Decoder and SVD clocks",0,0,0);
  clk_regime_resource_disable (CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_CDMA_VD);
  clk_regime_resource_disable (CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_CDMA_DEC);

} /* end dec_svd_clk_disable() */

/*============================================================================

FUNCTION DEC_TD_CLK_ENABLE

DESCRIPTION 
  Enable clocks for Turbo Decoder block for CDMA operations.

DEPENDENCIES
  TURBO DEC clock regimes should have been initialized.

RETURN VALUE 
  None

SIDE EFFECTS
  Enable turbo decoder clocks
=============================================================================*/
void dec_td_clk_enable (void)
{
  /* Check if TD clock is already on */
  boolean clk_already_on = clk_regime_is_on(CLK_RGM_TURBO_DEC_M);

  /* Enable decoder clocks. */
  MSG_HIGH ("Enable Turbo Decoder clocks (reset=%d)",!clk_already_on,0,0);
  clk_regime_resource_enable (CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_CDMA_TD);

  /* Reset the decoder if its clock was off previously */
  if (!clk_already_on)
  {
#ifdef MSMHW_TURBO_CODING
    HWIO_OUT (TD_RESET, 0x00);
#endif /* MSMHW_TURBO_CODING */
  }

} /* end dec_td_clk_enable() */

/*============================================================================

FUNCTION DEC_TD_CLK_DISABLE

DESCRIPTION 
  Disable clocks for Turbo Decoder block when not in use.

DEPENDENCIES
  TURBO DEC clock regimes should have been initialized.

RETURN VALUE 
  None

SIDE EFFECTS
  Disable turbo decoder clocks
=============================================================================*/
void dec_td_clk_disable (void)
{
  /* Disable decoder clocks. */
  MSG_HIGH ("Disable Turbo Decoder clocks",0,0,0);
  clk_regime_resource_disable (CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_CDMA_TD);

} /* end dec_td_clk_disable() */

#else /* !FEATURE_SRCH_CLKREGIM_RM */

/*============================================================================

FUNCTION DEC_CLK_SELECT

DESCRIPTION 
  This function selects the appropriate decoder clock for Traffic state (TCXO)
  or Idle state (Chipx8).  This function is implemented to reduce power 
  consumption in Idle state.
  Both Turbo and Viterbi decoders are reset after clock switching.

DEPENDENCIES
  DEC Clock regime should have been enabled.

RETURN VALUE 
  None

SIDE EFFECTS
  Switches decoder clock
=============================================================================*/
void dec_clk_select
(
  dec_clk_select_type new_clk_select
)
{ 
  static dec_clk_select_type current_clk_select = DEC_CLK_NONE;

  /* Do not not reset the decoders without change in clock select. */
  if (current_clk_select != new_clk_select)
  {
    INTLOCK ();
    current_clk_select = new_clk_select;

    /* Default is Chipx8, unless TCXO is selected. */
    if (current_clk_select == DEC_CLK_TCXO)
    {
      /* Disable decoder clocks. */
      clk_regime_disable( CLK_RGM_SVD_M       );
      clk_regime_disable( CLK_RGM_TURBO_DEC_M );
      clk_regime_disable( CLK_RGM_DEC_M       );

      /* Enable decoder clocks. */
      clk_regime_enable( CLK_RGM_DEC_M       );
      clk_regime_enable( CLK_RGM_TURBO_DEC_M );
      clk_regime_enable( CLK_RGM_SVD_M       );

      MSG_HIGH ("Dec clk set to TCXO",0,0,0);
    }
    else 
    {
      /* Disable decoder clocks. */
      clk_regime_disable( CLK_RGM_SVD_M       );
      clk_regime_disable( CLK_RGM_TURBO_DEC_M );
      clk_regime_disable( CLK_RGM_DEC_M       );

      /* Enable decoder clocks. */
      clk_regime_enable( CLK_RGM_DEC_M       );
      clk_regime_enable( CLK_RGM_TURBO_DEC_M );
      clk_regime_enable( CLK_RGM_SVD_M       );

      MSG_HIGH ("Dec clk set to CHIPX8",0,0,0);
    }

    DEC_DYNAMIC_CLK_SAVE();
    DEC_DYNAMIC_CLK_OFF();

    /* Need to reset the decoders after switching clock. */
#if defined(T_MSM7600)
#error code not present
#else /* if !T_MSM7600 */
    HWIO_OUT(VD_RESET, 0x00);
#endif /* !T_MSM7600 */

#ifdef MSMHW_TURBO_CODING
    HWIO_OUT (TD_RESET, 0x00);
#endif /* MSMHW_TURBO_CODING */

    DEC_DYNAMIC_CLK_RESTORE();

    INTFREE ();

  }
}
#endif /* !FEATURE_SRCH_CLKREGIM_RM */


/*=========================================================================
FUNCTION DEC_READ_CH_STATUS_SB

DESCRIPTION
  This function reads in the status of a channel from the Short Buffer
  
  The status for the Sync Channel, Paging Channel & Fundicated Channel 
  all have the same format
  
  This function receives an offset into the output buffer of the status
  information, and pointers to variables in which to store the information.
  
  The format of the status information in the output buffer is as follows:
  
  +-------------------------------------------------------------------------+
  | 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 |  7 |  6 | 5 |  4 | 3 | 2 | 1 |  0 |
  +-------------------------------------------------------------------------+
  | BC | VI | UN |                       SER(12:0)                          |
  +-------------------------------------------------------------------------+
  |                                 Energy (15:0)                           |
  +-------------------------------------------------------------------------+
  |                  Unused             | QU | CP | Unused | Energy (19:15) |
  +-------------------------------------------------------------------------+
  
  Where:
  
      BC = Bad Count
      VI = Violation
      UN = Unused
      QU = Quality
      CP = CRC Pass
  
  All resulting values are stored in the lowest order bits except for the
  status result, which is stored as follows:

  +---------------------------------------+
  |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
  +---------------------------------------+
  | BC | VI |      Unused       | QU | CP |
  +---------------------------------------+
  
  The format of this result is for backward compatibility with an earlier
  version of this module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
LOCAL void dec_read_ch_status_sb
(
  word  buffer_offset,
  byte *pstatus,
  word *pser,
  int4 *penergy
)
{
  word temp_status;

  /* Set the start address for reading */
  DECOB_SHORT_BUFFER_ADDR_SET( buffer_offset); 

  /* Read the first word */
  temp_status = inpw((word *) HWIO_ADDR(DECOB_SHORT_BUFFER_DATA));
  if (pser)
  {
    *pser= (temp_status & 0x1FFF);
  }
  if (pstatus)
  {
    *pstatus = (byte) ((temp_status & 0xC000) >> 8);
  }
    
  /* Read the second word */
  temp_status = inpw((word *) HWIO_ADDR(DECOB_SHORT_BUFFER_DATA));
  if (penergy)
  {
    *penergy = (int4) temp_status;
  }
  
  /* Read the third word */
  temp_status = inpw((word *) HWIO_ADDR(DECOB_SHORT_BUFFER_DATA));
  if (pstatus)
  {
    *pstatus |= (byte) ((temp_status & 0x00C0) >> 6);
  }
  if (penergy)
  {
    *penergy |= ((int4)(temp_status & 0x000F) << 16);
  }

} /* dec_read_ch_status_sb */
/*=========================================================================
FUNCTION DEC_READ_CH_STATUS_LB

DESCRIPTION
  This function reads in the status of a channel from the Long Buffer
  
  SCH & FCCCH data is read from the Long Buffer.

  This function receives an offset into the output buffer of the status
  information, and pointers to variables in which to store the information.
  
  The format of the status information in the output buffer is shown in 
  Table 11-5 in the Devo Core HDD
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
LOCAL void dec_read_ch_status_lb
(
  word  buffer_offset,
  byte *pstatus,
  word *pser,
  int4 *penergy,
  word *pltu_crc
)
{
  dword temp_status;

  /* Set the start address for reading */
  DECOB_LONG_BUFFER_ADDR_SET( buffer_offset); 

  /* Read the first word */
  temp_status = in_dword((dword *) HWIO_ADDR(DECOB_LONG_BUFFER_DATA));
  if (pser)
  {
    *pser= (word) ((temp_status & 0x1FFF0000) >> 16);
  }

  if (penergy)
  {
    *penergy = (int4) (temp_status & 0x0000FFFF);
  }

  /* Read the second word */
  temp_status = in_dword((dword *) HWIO_ADDR(DECOB_LONG_BUFFER_DATA));
  if (pstatus)
  {
    *pstatus = (byte) ((temp_status & 0x00C00000) >> 22);
  }
  
  if (penergy)
  {
    *penergy |= (int4) (temp_status & 0x000F0000);
  }

  if (pltu_crc != NULL)
  {
    *pltu_crc = (word) temp_status & 0x000000FF;
  }

} /* dec_read_ch_status_lb */

#ifdef FEATURE_IS2000_REL_A_CC
/*=========================================================================*/
boolean
dec_read_bcch_status (dec_frame_status_type *status)
{
  word temp_status;
  boolean success = FALSE;

  if (status) {
    success = TRUE;

    dec_read_ch_status_sb (DEC_BCCH_SIZ/2,
                        (byte*)&temp_status,
                        &status->sec,
                        &status->energy);

    /* Check status bits */
    status->crc_passed  = (temp_status & 0x01) ? TRUE : FALSE;
    status->quality_ind = (temp_status & 0x02) ? TRUE : FALSE;
  }

  return success;
}

/*=============================================================================
 * ==========================================================================*/
void dec_ccch_init()
{
   /* Turn dynamic clock gating off before writing to the registers below */
   DEC_DYNAMIC_CLK_SVD_OFF();
   CLKRGM_DEC1_START();

/* PUT SVD In Packet Mode */
   HWIO_OUTM(VD_MODE,
             HWIO_FMSK(VD_MODE,PACKET) | VD_MODE_WB__INTMODE_MASK,
             VD_PACKET_V | VD_INITMODE_SATURATE_STATE_V);

#ifdef T_MSM7600
#error code not present
#else /* if !T_MSM7600 */
   HWIO_OUT(VD_RESET, 0x00);
#endif /* !T_MSM7600 */

   CLKRGM_DEC1_END();
   /* Turn dynamic clock gating on for the paging channel */
   DEC_DYNAMIC_CLK_SVD_ON();

#ifdef MSMHW_MANAGED_DEM_MAC_CLK
   /* Turn off DEM_MAC clock during paging to save power */
   HWIO_OUTM(MISC_CLK_CTL,
             MISC_CLK_CTL_WW__DEM_MAC_CLK_SLEEP_MASK,
             MISC_CLK_CTL_WW__DEM_MAC_CLK_SLEEP_ENA);
#endif
}


/*=============================================================================
 * ==========================================================================*/
boolean dec_read_ccch_status
(
  dec_frame_status_type *status,
  dec_ccch_data_rate_type ccch_rate
)
{
  word temp_status;
  boolean success = FALSE;

  if (status) {
    success = TRUE;

    dec_read_ch_status_sb (DEC_FCCCH_RAM_BASE + ((DEC_CCCH_1X_SIZ*(0x1<<ccch_rate))/2),
                        (byte*)&temp_status,
                        &status->sec,
                        &status->energy);

    /* Check status bits */
    status->crc_passed  = (temp_status & 0x01) ? TRUE : FALSE;
    status->quality_ind = (temp_status & 0x02) ? TRUE : FALSE;

  }

  return success;
}


/*=============================================================================
 * ==========================================================================*/
void dec_read_ccch_data
(
  dec_ccch_buf_type ccch_buf,
  dec_ccch_data_rate_type ccch_rate
)
{
  /* Pass in DEC_SCH_RAM_BASE to distinguish between Short & Long Buffer */
  (void) dec_read_data 
           (DEC_FCCCH_RAM_BASE, ccch_buf, DEC_CCCH_1X_SIZ*(0x1<<ccch_rate));
}
#endif /* FEATURE_IS2000_REL_A_CC */

#ifdef FEATURE_IS2000_REL_A /* 32X */
/*===========================================================================

FUNCTION DEC_READ_MUX_PDU5

DESCRIPTION
  Read one MUX PDU5 data from the Decoder. This function parses the MUX PDU5
  header and chain the dsm items if necessary.
DEPENDENCIES

RETURN VALUE
  TRUE if a MuxPDU5 has been read.
  FALSE if otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean dec_read_mux_pdu5
(
  word* start_pos,             /* Start position of the data  */
  dsm_item_type *dsm_item_ptr, /* array of dsm_item pointers */
  word max_pos                 /* on or after this point is garbage */
)
{
  word dec_sch_ob_addr;
  /* dec_sch_ob_addr points to the address of the sch data and status in
   the decoder output buffer */
  byte offset;          /* offset when start address is odd. */
  byte junk_at_end=0;   /* Junk bytes at the end of the reading window */
  word pos;             /* current position */
  byte sr_id;           /* sr_id field */
  byte length_ind;      /* length indicator */
  word length = 0;      /* length of the pay load */
  int  num_words;
  boolean pdu_found = FALSE;  /* return value */
  word words_read;      /* # of words read from the decoder */
  word words_need;      /* # of words need to read */
  dsm_item_type *tail = NULL;
                        /* keeps track of the last dsm item */
  uint16  *pdu5_data_ptr;



  dec_sch_ob_addr =  DEC_SCH_RAM_BASE;
  pos = *start_pos;     /* Set position pointer to the start position */


  if (dsm_item_ptr != NULL)
  {
    while( (pos + 1) < max_pos ) /* each PDU, including PDU header, must has more than 1 byte*/
    {
      /*
      ** Notice that data are read out from decoder in half word (16 bits).
      ** If the start position is odd, we will have to read one byte ahead.
      */

      dec_sch_ob_addr = DEC_SCH_RAM_BASE + pos/2;

      /*
      ** Offset indicates where the real data starts.
      ** If offset is 0, data starts from the first byte.
      ** If offset is 1, the first byte is junk.
      */
      offset = pos%2;

      /*
      ** Because the consumer of the data does not free the dsm item all
      ** the time, e.g., TDSO. So it is possible that the data_ptr is an
      ** odd number.
      ** Base on the assumption that only this function can offset that
      ** pointer by one, which makes it odd, we deduct by one if we see
      ** an odd address. Otherwise, a data abort can happen.
      ** A clean solution would be the consumer always free the dsm item
      ** so the data_ptr is reset automatically.
      */

      if (((dword)dsm_item_ptr->data_ptr)%2 != 0)
      {
        /* Print debug message */
        /* If this message is seen, it means that DSM item is not 
        ** properly freed.
        */
        ERR("Odd data_ptr", 0, 0, 0);

        dsm_item_ptr->data_ptr --;
      }


      /* Read the first 4 bytes, it should contain the MuxPDU header */
      pdu5_data_ptr = dec_temp_sdu_data_ptr + dec_sch_ob_addr - DEC_SCH_RAM_BASE;
      memcpy( dsm_item_ptr->data_ptr, pdu5_data_ptr, 4); /* copy 4 bytes */
      pdu5_data_ptr = pdu5_data_ptr + 2;

      /* extract the sr_id */
      sr_id = (dsm_item_ptr->data_ptr[offset] & 0xe0) >> 5;
      length_ind = (dsm_item_ptr->data_ptr[offset] & 0x0c) >> 2;

      if ( sr_id == 0 || sr_id == 7 || length_ind == 3 )
      {
        /*
        ** If SRID field is '000' or '111', treat this mux PUD as
        ** fill pdu and skip it till the end of this data block.
        ** Notice that '000' is reserved,
        ** if BS sends it, we skip as if it is a fill PDU.
        */

        pos = max_pos;
        length = 0;
      }
      else /* Not a fill PDU */
      {
        if (length_ind == 0)
        {
          /*
          ** Treat these cases as no length feild. '11' is
          ** also reserved. We treat this case as no length.
          ** According to the MAC layer, when the length field is 0, 
          ** the whole SDU( excluding the CRC, encoder tail bits) is
          ** 1 PDU. So the length of PDU is surely bigger than 2 bytes
          */

          length = max_pos - pos;  /* length of the PDU to read */

        }
        else
        {
          /* extract the length field */
          length = b_unpackw(dsm_item_ptr->data_ptr,
                             offset*8 + 6,
                             length_ind*8);

          /* payload is 2+length*8 bits, plus length field (1 or 2 bytes)
             plus 6 bits. Total length of MuxPDU in bytes can be obtained
             from the following formula */

          /* in this case, the length of PDU is also bigger than 2 bytes */
          length += length_ind + 1;  /* 1 byte header + length field */
        }

        /*
        ** Calculate how many 16 bit word needs to be read. Or
        ** the window in words that covers the data needs to
        ** be read.
        ** We could read two bytes of garbage in the worst case,
        ** with one byte at each end to round the PDU to enven
        ** byte boundary.
        ** Notice that the equation is not simplified for ease of reading.
        ** I will leave the optimizer to reduce it.
        */

        num_words = (pos+length)/2 + (pos+length)%2 - pos/2;

        /*
        ** We've already calculated garbage at the beginning of the
        ** reading window, and stored that in offset.
        ** Now we need to calculate how many bytes of garbage at the
        ** end of the reading window.
        ** the answer can be only 0, or 1.
        ** The following formula says: if the data ends on even word
        ** boundary, there should be no garbage in the end. If the
        ** data ends on odd address, there has to be one byte of
        ** garbage.
        */

        junk_at_end = (pos+length)%2;

        /* starting position of next MuxPDU */
        pos += length;

        /* Need to check if this MuxPDU crosses data block boundary */
        if ((pos <= max_pos) && (num_words > 1)) /* since the length is bigger than 2 bytes */
        {
          /* This MuxPDU is not across the boundary */

          /*
          ** Read data into the first dsm item. We never read more
          ** data than a DSM item can hold
          */
          words_read = MIN(dsm_item_ptr->size/2, num_words);

          if (words_read >= 2)
          {

            /* Since we've pulled out two words, so we need to read 2 less */
            memcpy((dsm_item_ptr->data_ptr +4), pdu5_data_ptr, (words_read-2)*2);
            pdu5_data_ptr = pdu5_data_ptr + words_read-2;
          }
          else
          {
            MSG_FATAL("DSM Bad s %x,l %d,p %d",dsm_item_ptr->size,length,pos);        
          }

          /* Bytes read in this DSM item including garbage */

          dsm_item_ptr->used = words_read*2;

          dsm_item_ptr->app_field = TRUE;    /* Indicates a good PDU */

          /* Head of the dsm item chain is the same as tail now */
          tail = dsm_item_ptr;

          while( words_read < num_words )
          {
            /* Still words to read, we need to allocate new dsm items. */

            if( (tail->pkt_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL ))
                != NULL )
            {
              /* Now we have a new tail */
              tail = tail->pkt_ptr;

              /* Calculate how many words we need to read */
              words_need = MIN(tail->size/2, num_words-words_read);

              if (((dword)tail->data_ptr) % 2 != 0)
              {
                /* This also should not happen */
                ERR("Odd data_ptr!", 0, 0, 0);

              /* Adjust data pointer if it is odd */
                tail->data_ptr --;
              }

              /* Continue to read */
              memcpy(tail->data_ptr, pdu5_data_ptr, 2*words_need);
              pdu5_data_ptr = pdu5_data_ptr + words_need;

              /* Set used field inlcuding garbage */
              tail->used = words_need*2;

              /* Adjust number of words read */
              words_read += words_need;

              /* Indicates that this dsm item is good */
              tail->app_field = TRUE;


            }
            else
            {
              MSG_ERROR("No DSM", 0, 0, 0);
              /* No dsm item available */

              break;
            }
          } /* while */

          /* If the words read out from decoder is less then the
          ** number of words expected, there has to be an unexpected
          ** exit from the loop. Clean up is done here.
          */

          if( words_read < num_words )
          {
            /* abnormal exit from the while loop */

            /* Free the chain from the 2nd dsm_item
               because the first item is always there */
            dsm_free_packet( &dsm_item_ptr->pkt_ptr );

            /* Reset fields in the first item */
            dsm_item_ptr->used = 0;
            dsm_item_ptr->app_field = 0;
            dsm_item_ptr->pkt_ptr = NULL;

          }
          else
          {
            /*
            ** Adjust the offset ONLY when the whole PDU is read, to
            ** prevent buffer leak.
            */
            dsm_item_ptr->data_ptr += offset;

            /* When the data_ptr is moved, needs to account for it
            ** by adjusting the size, otherwise data may be written
            ** across buffer boundary.
            */
            dsm_item_ptr->size -= offset;

            /*
            ** Adjust the used field of the first and the last dsm items
            ** to exclude garbage. Notice that in the case of only one
            ** dsm item, head and tail are pointing to the same dsm item.
            */

            dsm_item_ptr->used -= offset;
            tail->used -= junk_at_end;

            pdu_found = TRUE;
          }

          /* We are either found one MUXPDU5 and adjusted pos,
             time to jump out, and return */
          break;

        }
        else
        {
          /* SDU data can't be trusted, discard the whole SDU */
          pos = max_pos;
        }
      } /* not Fill PDU */
    } /* while */
  }
  else
  {
    MSG_ERROR("no DSM", 0, 0, 0);
  }

  *start_pos = pos;  /* return the current position */
  return pdu_found;  /* return status */

}
/*===========================================================================

FUNCTION DEC_READ_MUX_PDU5_SCH_DATA

DESCRIPTION
  Read data from the Decoder as Supplemental(Traffic) Channel data (IS95C SCH).
  This function reads MUX_PDU5 into the buffer pointed
  to by the dsm_item.
DEPENDENCIES
  Dec_tc_init() should have already been called.
  This function should not be called if a frame is bad and there is no LTU
  ( 1X, 2X and turbo case ).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define DEC_SCH_MAX_FWD_PDU  8
void dec_read_mux_pdu5_sch_data
(
  dec_sch_cfg_type *sch_rec_ptr,  /* Pointer to rxc_sch record pointer */
  dsm_item_type **dsm_item_ptr, /* array of dsm_item pointers */
  byte *mux_pdu_count,      /* return the number of MUX PDU read */
  word ltu_crc,            /* LTU crc from the decoder status */
  byte *good_ltu           /* return value of number of good ltus */
)

{
  byte dsm_item_index=0;
  byte cnt;

  /* dec_sch_size indicates the number of bytes for the RC and the given
     SCR Rate */
  word dec_sch_size =  dec_sch_ob_params[DEC_SCH_RC_INDEX(sch_rec_ptr->rc)]
                                         [sch_rec_ptr->sch_rate];
  word sdu_start_pos = 0;
    /* Byte offset of the start point for the next MuxPDU */
  word curr_pos;
    /* Keep the current position of parsed data block */
  byte pdu_cnt = 0;
    /* Keep track of the number of MuxPDU read */
  word mask;
  byte good_ltu_cnt = 0;
    /* Counter for good LTUs */

  /*
     Oh , MuxPDU 5!
     Here is the assumption we have:
     1. There is already 8 dsm items allocated. If a MuxPDU 5 cannot be
        contained within one dsm item, additional dsm items will be
        allocated and chained.
  */

  curr_pos = sdu_start_pos;  /* Initialize current position */

  /* Calculate how many information bits in the SDU */
  if (sch_rec_ptr->sch_rate == DEC_SCH_RATE_1X)
  {
    /* For 1X case, there are 12 bits frame CRC and 8 bits tail */
    dec_sch_size = ((dec_sch_size*8) - (DEC_SCH_SIZ_1X_CRC + DEC_SCH_SIZ_TAIL ))/ 8;

  }
  else
  {
    /* For 2X and up, there are 16 bits frame CRC and 8 bits tail */
    dec_sch_size =  ((dec_sch_size*8) - (DEC_SCH_SIZ_2X_CRC + DEC_SCH_SIZ_TAIL ))/ 8;

  }

  /* for MSM6700 or above chip, read the whole SDU into a temp array
  we could use the burst-8 interface for faster reading */
  DECOB_LONG_BUFFER_ADDR_SET(0);
  DECOB_LONG_BUFFER_DATA_GET(dec_temp_sdu_data_array,(dec_sch_size + 3)/4);
  dec_temp_sdu_data_ptr = (uint16 *) dec_temp_sdu_data_array;


  if( sch_rec_ptr->num_ltu == 0 )
  {
    /*
       This covers the following cases:
       + Convolutional - 1X & 2X ( No LTU )
       + Turbo - 2X and up (No LTU)
    */

    for (cnt=0; cnt< DEC_SCH_MAX_FWD_PDU; cnt++)
    {
      /* Keep reading till max number of MuxPDU allowed by standard */
      if( dec_read_mux_pdu5(&curr_pos,
                        dsm_item_ptr[dsm_item_index],
                        dec_sch_size)
        )
      {
        /*
        ** Function returns TRUE, there might still be MUXPDU 5 in the
        ** data block. Move to the nex dsm item pointer and read again.
        */
        dsm_item_index++;
      }
      else
      {
        /* There is no more MuxPDU in this SDU */
        break;
      }

    }
  }
  else /* This covers LTU != 0 cases */
  {
    /*
    ** If LTU# !=0, MuxPDU header cannot be interpret unless LTU CRC passes
    */

    /* Initialize the mask for crc checking */
    mask = 0x80 >> (8 - sch_rec_ptr->num_ltu);

    for (cnt=0;
         cnt<sch_rec_ptr->num_ltu && dsm_item_index < DEC_SCH_MAX_FWD_PDU;
         cnt++)
    {
      /* Find out whether this LTU is good or bad. Skip bad ones */
      if (mask & ltu_crc)
      {
        /* Good LTU, keep going */
        good_ltu_cnt++;

        /* Set up the start point for this LTU */
        sdu_start_pos =  sch_rec_ptr->ltu_len/8 * cnt;
        curr_pos = sdu_start_pos;


        for (pdu_cnt=0;
             pdu_cnt < DEC_SCH_MAX_FWD_PDU && dsm_item_index < DEC_SCH_MAX_FWD_PDU;
             pdu_cnt++ )
        {
          /* Notice that there are two bytes of CRC at the end of each
          ** LTU. They should not be read out from decoder. So deduct
          ** 2 from the limit setting.
          */

          if (dec_read_mux_pdu5(&curr_pos,
                            dsm_item_ptr[dsm_item_index],
                            sdu_start_pos + sch_rec_ptr->ltu_len/8 - 2)
              )
          {
            /*
            ** There is a MuxPDU 5 found within this LTU, adjust the pointer
            ** and try to find the next one.
            */

            dsm_item_index++;

          }
          else
          {
            /*
            ** There is no more MuxPDU 5 in this LTU. Jump out for the next LTU
            */

            break;
          }
        } /* for */
      } /* if (mask & ltu_crc) */

      mask >>=1;  /* Shift the CRC mask */

    }   /* for (cnt=0; cnt<sch_rec_ptr->num_ltu; cnt++) */
  } /* if( sch_rec_ptr->num_ltu == 0 ) */

  *good_ltu = good_ltu_cnt;        /* Good LTU if convolutional and has LTUs */
  *mux_pdu_count = dsm_item_index; /* Total number of mux pdu's */
}
#endif /* FEATURE_IS2000_REL_A */
