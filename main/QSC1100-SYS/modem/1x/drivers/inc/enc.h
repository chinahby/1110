#ifndef ENC_H
#define ENC_H
/*===========================================================================

      E N C O D E R   ( I N T E R L E A V E R )    H E A D E R    F I L E

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM CDMA Interleaver / Encoder chip set

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 1992 through 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/inc/enc.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/09   vlc     Added function enc_hw_reset() which resets encoder HW block.
07/16/08   jtm     Removed unused functions as part of the ULC memory reduction
                   effort.
05/21/08   vlc     Added changes for 7800 modem to mainline.
04/21/05   vlc     Main lined code in current targets, cleand out obsolete targets,
                   cleaned out obsolete features.
10/12/04   ljl     Updated encoder channel for R-SCH.
06/18/04   bn      Renamed the encoder channel enum type
03/05/04   bn      Checked in encoder driver changes for MSM6700
01/06/03   bn      Added new function to handle encoder's writing for MUXPDU5
03/14/02   ems     Added enci.h include
02/22/02   tc      Supported FEATURE_IS2000_REL_A_CC.
02/11/02   sr      Removed enc_version()
02/08/02   tc      Supported Reverse Enhance Access Channel.
02/06/02   ems     Merged in changes for MSM6050.  Featurized enc_voc_ctl and 
                   enc_codec_clk_ctl.  Note MOD_CLK_CTL is test use only.
                   Removed ENC_CHIPX8_CLK_DISABLE ENC_CHIPX8_CLK_ENABLE. 
11/08/01   hrk     Added function prototype for enc_pcbit_test_mask().
07/12/01   vlc     Changed FEATURE_RC4_SCH to FEATURE_DS_IS2000_RS2 in order
                   to eliminate FEATURE_RC4_SCH.
05/27/01   sr/snn  Modified ENC_SCH_DATA_OUT to take pdu size as a parameter.
05/03/01   tc      Supported reverse link DCCH. 
04/26/01   vlc     Added Reverse Link SCH RC4 support. 
04/09/01   sr      Rounded up size of SCH encoder data to nearest 32 bits
03/23/01   sr      Added support for 32X
02/28/01   tn      Added support for FEATURE_ENC_BTF_IN_NV.
02/14/01   tn      Added enc_set_rx_polarity() and enc_set_tx_polarity().
                   Added new definitions for Tx/Rx spectral polarity.
11/10/00   tc      Defined feature T_MSM_5100
11/09/00   tc      Added MSM5100 modulator driver support
10/22/00   rmd     Clean up code and merged with MSM5000 code.
09/07/00   jcw     Changed include to generic msm.h instead of msm50reg.h
07/14/00   sr      Added enc_gating_pattern_type enum defintion
06/22/00   sr      Added boolean generate_crc to enc_sch_data_out_cfg()
05/08/00   sr      Added support for MSM5000B
                   Added enc_sch_data_out_cfg()
                   Added macro ENC_SCH_DATA_OUT()
                   Changed ENC_RDWR_M to include bit for SCH
04/25/00   sr      Added function prototype for enc_pcbit_test_data()
04/11/00   sr      Merged in rel 0.3 changes:
                      Added function descriptions as part of code cleanup 
                      for release 0.3
                   Removed unused prototype for enc_dcch_gain()
11/19/99   sr      Changes to support Supplemental Channels
10/07/99   lcc     Merged in from PLT 2.0.
09/23/00   lcc     Merged in MSM5000 support from PLT archive.
12/28/94   jah     Added support for 14400 rate set.
10/20/94   jah     Changed enc_pn_rand_delay() to return previous delay.
10/11/94   jah     Added enc_pn_rand_delay()
03/07/94   jah     Improve some function comments.
05/11/93   jah     Added enc_chipx8_clk_enable/disable.
08/31/92   jah     Added enc_init_pa() to configure PA control for PCN/not
07/31/92   jah     Added enc_acp_pa_ctl() to control the PA for Analog calls.
07/28/92   jah     Added enc_voc_ctl() and enc_codec_clk_ctl()
01/30/92   jah     Ported module from brassboard enc.h

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "qw.h"         /* definition of qword                  */
#include "rex.h"        /* definition of REX data types         */
#include "target.h"     /* Target specific definitions          */
#include "misc.h"
#include "msm.h"
#include "enci.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Encoder channel enum type */
typedef enum 
{
  ENC_CHAN_1,
  ENC_CHAN_2,
  ENC_CHAN_3,
  ENC_CHAN_RPDCH,
  ENC_TOTAL_CHAN
} enc_chan_number_type;

/* Channel states for the Encoder
*/
typedef enum {
  ENC_SLEEP_MODE,       /* Sleep/Analog mode                    */
  ENC_CDMA_MODE,        /* CDMA Start mode                      */
  ENC_SC_MODE,          /* Sync Channel mode                    */
  ENC_PC_MODE,          /* Paging/Access Channel mode           */
  ENC_TC_MODE           /* Traffic Channel/Acquisition mode     */
} enc_mode_type;

/* Data encoding rates for enc_data_out()
*/
typedef enum {
  ENC_FULL_RATE = 0x00, /* Encode "full" rate frames    */
  ENC_HALF_RATE = 0x01, /* Encode "half" rate frames    */
  ENC_QTR_RATE  = 0x02, /* Encode "quarter" rate frames */
  ENC_8TH_RATE  = 0x03  /* Encode "eighth" rate frames  */
} enc_rate_type;

/* Data encoding rate sets for enc_data_out()
*/
typedef enum {
  ENC_RATE_9600  = 0x00, /* Select 9600 bps rate set   */
  ENC_RATE_14400 = 0x01  /* Select 14400 bps rate set  */
} enc_rate_set_type;

/* Encoder Code rates
*/
typedef enum 
{
  ENC_CODE_1_BY_2_RATE = 0x00,
  ENC_CODE_1_BY_3_RATE = 0x01,
  ENC_CODE_1_BY_4_RATE = 0x02
} enc_code_rate_type;

/* Data encoding rates for enc_sch_data_out()
*/
typedef enum 
{
  ENC_SCH_8TH_RATE     = 0x00, /* RC3:    1.5 Kbps, RC4:    1.8 Kbps    */
  ENC_SCH_QTR_RATE     = 0x01, /* RC3:    2.7 Kbps, RC4:    3.6 Kbps    */
  ENC_SCH_HALF_RATE    = 0x02, /* RC3:    4.8 Kbps, RC4:    7.2 Kbps    */
  ENC_SCH_FULL_RATE    = 0x03, /* RC3:    9.6 Kbps, RC4:   14.4 Kbps    */
  ENC_SCH_2X_RATE      = 0x04, /* RC3:   19.2 Kbps, RC4:   28.8 Kbps    */
  ENC_SCH_4X_RATE      = 0x05, /* RC3:   38.4 Kbps, RC4:   57.6 Kbps    */
  ENC_SCH_8X_RATE      = 0x06, /* RC3:   76.8 Kbps, RC4:  115.2 Kbps    */
  ENC_SCH_16X_RATE     = 0x07, /* RC3:  153.5 Kbps, RC4:  230.4 Kbps    */
#ifdef FEATURE_32X_SCH_DATA_RATE                                       
  ENC_SCH_32X_RATE     = 0x08, /* RC3:  307.2 Kbps, RC4:  not supported */
#endif /* FEATURE_32X_SCH_DATA_RATE */ 
  ENC_NUM_SCH_RATES
} enc_sch_rate_type;

/* TX_EARLY_FRAME_ADV periods
*/
typedef enum 
{
  ENC_TX_EARLY_FRAME_DISABLE = 0x00,
  ENC_TX_EARLY_FRAME_5_MS    = 0x01,
  ENC_TX_EARLY_FRAME_10_MS   = 0x02,
  ENC_TX_EARLY_FRAME_20_MS   = 0x03
} enc_tx_early_frame_period_type;

/* FCH mode
*/
typedef enum {
  ENC_IS95_A_MODE = 0x0000,
  ENC_IS95_C_MODE = 0x0001
} enc_is95_mode_type;

/* Radio Configuration
*/
typedef enum 
{
  ENC_FCH_RC_1 = 1, /* Defined to be same as CAI_RC_?.  Don't change */ 
  ENC_FCH_RC_2 = 2,
  ENC_FCH_RC_3 = 3,
  ENC_FCH_RC_4 = 4,
  ENC_NUM_FCH_RCS = 4
} enc_fch_radio_config_type;

typedef enum 
{
  ENC_SCH_RC_3 = 3,
  ENC_SCH_RC_4 = 4,
  ENC_NUM_SCH_RCS = 2
} enc_sch_radio_config_type;

typedef enum 
{
  ENC_NO_GATING_MODE = 0x0,
  ENC_HHO_PREAMBLE_MODE = 0x1,
  ENC_FCH_8TH_RATE_GATING_MODE = 0x2,
  ENC_PILOT_GATING_MODE = 0x4
} enc_gating_mode_type;

typedef enum
{
  ENC_NO_GATING_PATTERN = 0xFFFF,
  ENC_HALF_RATE_GATING_PATTERN = 0xAAAA,
  ENC_QTR_RATE_GATING_PATTERN = 0x8888,
  ENC_FCH_8TH_RATE_GATING_PATTERN = 0xCCCC // Pilot duty cycle is 50%
} enc_gating_pattern_type;

/* Size (in bytes) of full rate packet packet at 9600 and 14400 bps
*/
#define ENC_9600_FULL_RATE_SIZE   24
#define ENC_14400_FULL_RATE_SIZE  36


/* 
    There are 3072 bits in 20 ms frame for data rate = 153.6K;
   6144 bits for rate = 307.2k as specified in the standard.
   3072/8 = 384  6144/8=768.
*/
#define ENC_SCH_RC3_16X_SIZE  384

#ifdef FEATURE_32X_SCH_DATA_RATE
#define ENC_SCH_RC3_32X_SIZE  768
#endif /* FEATURE_32X_SCH_DATA_RATE */ 

#ifdef FEATURE_DS_IS2000_RS2
#define ENC_SCH_RC4_16X_SIZE  576
#endif /* FEATURE_DS_IS2000_RS2 */

/* Masks for looking at circuit status returned by enc_status()
*/
typedef enum {
  ENC_STATUS_WRITE_ERROR_M = 0x0015,    /* Encoder write error */
  ENC_STATUS_ENCODER_BUSY_M = 0x002A,    /* Encoder busy */
  ENC_STATUS_POSITION_ERROR_M = 0x0040,  /* Start encoding at the same time */
  ENC_STATUS_TX_FR_INT_SLOT_M = 0x0180,  /* 5 ms frame slot */
  ENC_STATUS_TX_FR_INT_SLOT_POS = 0x07  /* interrupt slot bit position */
} enc_status_mask_type;

typedef enum {
  ENC_STATUS_INTERRUPT_SLOT_0  = 0x00,  /* 5 ms slot bin 0 */
  ENC_STATUS_INTERRUPT_SLOT_1  = 0x01,  /* 5 ms slot bin 1 */
  ENC_STATUS_INTERRUPT_SLOT_2  = 0x02,  /* 5 ms slot bin 2 */
  ENC_STATUS_INTERRUPT_SLOT_3  = 0x03  /* 5 ms slot bin 3 */
} enc_status_interrupt_slot_type;

#ifndef T_MSM7800
/* Definitions to specify RX spectral polarity */
typedef enum {
  ENC_RX_SPECTRAL_INV  = ENC_RX_INV_V,
  ENC_RX_SPECTRAL_NORM = ENC_RX_NORM_V
} enc_rx_polarity_type;
#endif /* !T_MSM7800 */

/* Definitions to specify TX spectral polarity */
typedef enum {
  ENC_TX_SPECTRAL_INV  = ENC_TX_INV_V,
  ENC_TX_SPECTRAL_NORM = ENC_TX_NORM_V
} enc_tx_polarity_type;

/*--------------------------------------------------------------------------
                            SOME MACROS
--------------------------------------------------------------------------*/

/* convert the reverse link RC specifier to a rateset */
#define ENC_RC_TO_RATE_SET(x) ((enc_rate_set_type)((x-1)%2))

/* Send the data to the encoder */
#define ENC_SCH_DATA_OUT( data_ptr, data_size, double_sized ) \
enc_write_data(ENC_CHAN_3,data_ptr,data_size,FALSE);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION ENC_INIT

DESCRIPTION
  Initialize the circuit to Idle.

DEPENDENCIES
  If enc_set_btf() is not called first, a default value will be used here.

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the circuit to Idle, disables the transmit path, the PA, and
  puts the circuit in a state where it is drawing minimal power.  The CHIPX8
  clock is enabled though.  Undoes any effects from enc_pn_rand_delay().

===========================================================================*/
extern void enc_init( void );


/*===========================================================================

FUNCTION ENC_SET_BTF

DESCRIPTION
  Load the back-to-the-future, and set the back-to-the-future value loaded
  by enc_init().  Note that this value is biased +25, so to set a value of
  100 into the counter, the parameter must be 125.

  Takes effect at the next SYNC80M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Undoes any effects from enc_pn_rand_delay().

===========================================================================*/
extern void enc_set_btf
(
  dword btf_chipx8_delay        /* CHIPX8 delay for SYNC80M, biased +25 */
);


/*===========================================================================

FUNCTION ENC_SET_FRAME_OFF

DESCRIPTION
  Specify Frame stagger offset for forward/reverse traffic frames
  This is the offset from the system frame in power control groups.
  This offset has no effect while in Sync Channel mode.  The value
  is reset to 0 offset by enc_init().

  Takes effect at the next SYNC80M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_set_frame_off
(
  byte fwd_off,         /* Forward channel frame stagger */
  byte rev_off          /* Reverse channel frame stagger */
);


/*===========================================================================

FUNCTION ENC_SET_MODE

DESCRIPTION
  Sets the operating mode of the driver: sleep, cdma, sync, paging, traffic.

DEPENDENCIES
  CDMA mode  - must not be entered before 20ms after enc_init().
  Sleep mode - must not be entered before 20ms after enc_tx_enable(FALSE).

RETURN VALUE
  None

SIDE EFFECTS
  Changes the operating mode of the circuit.
  Sleep mode - masks the transmit path, turns off the PA, and disables
               the CHIPX8 clocks to the circuit.  PN state is lost.
  CDMA mode  - masks the transmit path, turns ON the PA, and enables the
               CHIPX8 clocks to the circuit.  If previously in sleep mode,
               the circuit will not re-synchronize until the next SYNC80M.
  SC mode    - initialize the circuit for the Sync Channel
  PC mode    - initialize the circuit for the Paging Channel
  TC mode    - initialize the circuit for the Traffic Channel

===========================================================================*/
extern void enc_set_mode
(
  enc_mode_type channel_mode    /* Mode in which the circuit is to run */
);


/*===========================================================================

FUNCTION ENC_TX_ENABLE

DESCRIPTION
  Enable/disable the transmit path by masking and unmasking it.
  The TX_PUNCT change takes effect on the next transmit frame boundary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_tx_enable
(
  boolean enable_tx     /* Enable/disable (mask/unmask) the transmitter */
);


/*===========================================================================

FUNCTION ENC_KILL_PA

DESCRIPTION
  Kill the PA, a fatal error has occurred.  Turn off the PA and
  keep it off until we reset the system.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets enc_pa_dead to FALSE, which disallows re-enabling the PA.

===========================================================================*/
void enc_kill_pa( void );


/*===========================================================================

FUNCTION ENC_INT_TIMING_OFFSET

DESCRIPTION
  Specify the delay after the Transmit Frame Boundary when the Transmit Frame
  Interrupt will activate.  This takes effect immediately.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_int_timing_offset
(
  byte bin_mask,                /* 5ms bins mask */
  byte power_ctl_groups,        /* Offset in power ctl groups */
  byte walsh_symbols            /* Offset in Walsh symbols    */
);


/*===========================================================================

FUNCTION ENC_USER_PN_STATE

DESCRIPTION
  Set the User PN long code state.  The User PN state takes effect at the next
  80 ms Transmit Frame Boundary (which proceeds SYNC80M by the count in the
  Back-to-the-Future counter).  Loading 0's into the user PN state inhibits
  spreading from the User PN Generator.  This operation is non-atomic.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_user_pn_state
(
  qword user_pn_state   /* User PN state to load into User PN Generator */
);


/*===========================================================================

FUNCTION ENC_USER_PN_MASK

DESCRIPTION
  Set the User PN long code mask.  The User PN mask takes effect at the next
  20 ms Transmit Frame Boundary.  Loading 0's into the user PN mask inhibits
  spreading from the User PN Generator.  This operation is non-atomic.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_user_pn_mask
(
  qword user_pn_mask    /* User PN mask to load into User PN Generator */
);


/*===========================================================================

FUNCTION ENC_STMR_CMD

DESCRIPTION
  Load the User PN long code state and long code mask.
  TX_LC_STATE_LD   bit 3:2
     00   No operation
     01   Load on the next Tx 80 ms boundary
  TX_MASK_LD      bit 1:0
     00   No operation
     01   Load immediately
     10   Load on the next Tx 20 ms boundary  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_stmr_cmd
(
  byte stmr_cmd        /* load command */
);


/*===========================================================================

FUNCTION ENC_IQ_PN_STATE

DESCRIPTION
  Set the I & Q PN states into the I & Q PN Generator.  Load 0's to inhibit
  spreading from the I & Q PN Generator.  This takes effect at the next
  80 ms Transmit Frame Boundary (which proceeds SYNC80M by the count in the
  Back-to-the-Future counter).  This operation is non-atomic.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_iq_pn_state
(
  word i_pn_state,      /* I PN state */
  word q_pn_state       /* Q PN state */
);


/*===========================================================================

FUNCTION ENC_BOZO_MODE

DESCRIPTION
  Set the "bozo mode" bit, so that the User PN long code state is reloaded
  on each 80 ms Transmit Frame boundary.  Useful when debugging time sync
  problems between the Cel and the mobile.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_bozo_mode
(
  boolean enable_bozo_mode      /* Whether to enable/disable "bozo" mode */
);


/*===========================================================================

FUNCTION ENC_FCH_RC

DESCRIPTION
  Set the reverse fundamental channel radio configuration

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_fch_rc
(
   enc_fch_radio_config_type fch_rc
);



/*===========================================================================

FUNCTION ENC_DCCH_RC

DESCRIPTION
  Set the reverse DCCH radio configuration

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_dcch_rc
(
   enc_fch_radio_config_type dcch_rc
);

/*===========================================================================

FUNCTION ENC_SCH_RC

DESCRIPTION
  Set the reverse supplemental channel radio configuration

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_sch_rc
(
   enc_sch_radio_config_type sch_rc
);


/*===========================================================================

FUNCTION ENC_RATE_SET

DESCRIPTION
  Set the rate set to use for encoding data on the Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_rate_set
(
  enc_rate_set_type rate_set    /* Rate set to use for encoding data */
);


/*===========================================================================

FUNCTION ENC_FCH_DATA_OUT

DESCRIPTION
  Send data to the encoder, generate a CRC (or not), and encode it at
  a specified rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_fch_data_out
(
  byte *data_ptr,               /* Buffer to send to Encoder                 */
  boolean generate_crc,         /* Have the Encoder generate a CRC, or not   */
  enc_rate_type rate        /* Rate to encode the data as                */
);


/*===========================================================================

FUNCTION ENC_STATUS

DESCRIPTION
  Read the status of the encoder circuit.

DEPENDENCIES
  None

RETURN VALUE
  Circuit status which can be masked with enc_status_mask_type.

SIDE EFFECTS
  Reading status latches the status in the encoder, clearing error status.

===========================================================================*/
extern word enc_status( void );


/*===========================================================================

FUNCTION ENC_INTERRUPT_SLOT

DESCRIPTION
  Read the 5 ms frame slot of the encoder interrupt.

DEPENDENCIES
  None

RETURN VALUE
  The 5 ms slot.

SIDE EFFECTS
  None.

===========================================================================*/
extern enc_status_interrupt_slot_type enc_interrupt_slot( void );


/*===========================================================================

FUNCTION ENC_TX_CLK_ENABLE

DESCRIPTION
  Disable the Reverse Link transmit path clocks to save power while not using
  the Reverse Link (e.g. listening to the Paging Channel, but not transmitting
  on the Access Channel).  The Reverse Link state is undefined when transmit
  clocks are re-enabled, the transmit path should be re-initialized into Paging
  or Traffic channel mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Disables the transmit half of the circuit, putting it into sleep mode.

===========================================================================*/
extern void enc_tx_clk_enable
(
  boolean enable_tx_clks        /* Enable/disable the CHIPX8 clock on Tx path */
);


/*===========================================================================

FUNCTION ENC_TX_IF_LOOPBACK_INIT

DESCRIPTION
  Initialize the circuit to IF loopback mode.  Once this is done, packets
  can be transmitted by calling enc_tx_if_loopback().  The packets will
  loop-back from TX to RX and be received.  To go back to normal call modes,
  call enc_set_mode().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Puts the circuit in IF loopback

===========================================================================*/
extern void enc_tx_if_loopback_init( void );


/*===========================================================================

FUNCTION ENC_ACP_PA_CTL

DESCRIPTION
  Turn on/off PA_ON and forces TX_PUNCT high and low, this effectively
  controls the PA for Analog Call Processing.

  Note: this does not override enc_kill_pa().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets enc_pa_dead to FALSE, which disallows re-enabling the PA.

===========================================================================*/
extern void enc_acp_pa_ctl
(
  boolean enable
    /* Enable/Set (disable/clear) the PA_ON and TX_PUNCT outputs */
);


/*===========================================================================

FUNCTION ENC_INIT_PA

DESCRIPTION
  Initialize the PA to off, do this appropriate to which type of PA control
  hardware is available.  With the PCN-applique, PA_ON is never to be turned
  on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_ENC_BTF_IN_NV
extern void enc_init_pa
(
  boolean pcn,
    /* True if the PCN applique is attached, False if not */
  uint32 delay
    /* BTF delay value in chipx8 */
);
#else
extern void enc_init_pa
(
  boolean pcn
    /* True if the PCN applique is attached, False if not */
);
#endif


/*===========================================================================

FUNCTION ENC_PN_RAND_DELAY

DESCRIPTION
  Delay the transmit time by a specified number of chips.  This value is
  loaded internally by the Interleaver at the next SYNC80M, and takes effect
  at the successive internal 80 millisecond boundary (the BTF roll-over).

  For CDMA transmit PN randomization, call this routine with the number of
  chips of delay desired.  Call it again with 0 to return to non-delayed
  transmit frame timing.

DEPENDENCIES
  None

RETURN VALUE
  Number of chips from the previous call to enc_pn_rand_delay.

SIDE EFFECTS
  None

===========================================================================*/
extern word enc_pn_rand_delay
(
  word chips        /* Chip delay for transmit frame timing */
);


/*===========================================================================

FUNCTION ENC_SCH_DATA_OUT

DESCRIPTION
  Send data to the encoder, generate a CRC (or not), and encode it at
  a specified rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_sch_data_out
(
  byte *data_ptr,                /* Buffer to send to Encoder               */
  boolean generate_crc,          /* Have the Encoder generate a CRC, or not */
  enc_sch_rate_type rate,        /* Rate to encode the data as              */
  boolean turbo_code,            /* TRUE= Turbo, FALSE=Convolutional        */
  word ltu_size                  /* LTU block size with CRC in bits         */
);



#ifdef FEATURE_IS2000_REL_A_CC
//enc_reach_on
/*===========================================================================

FUNCTION ENC_REACH_DATA_OUT

DESCRIPTION
  Send data to the encoder, generate a CRC (or not), and encode it at
  a specified rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_reach_data_out
(
  byte *data_ptr,                /* Buffer to send to Encoder               */
  boolean generate_crc,          /* Have the Encoder generate a CRC, or not */
  enc_sch_rate_type rate,        /* Rate to encode the data as              */
  boolean turbo_code,            /* TRUE= Turbo, FALSE=Convolutional        */
  word ltu_size,                 /* LTU block size with CRC in bits         */
  word frame_size                /* Frame size                              */
);
//enc_reach_on
#endif


/*===========================================================================

FUNCTION ENC_SCH_LTU_SIZE

DESCRIPTION
  Set supplemental channel LTU size. 32 <= ltu_size < 512 bits

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_sch_ltu_size
(
  word sch_ltu_size        /* 32 <= ltu_size < 512 bits */
);


/*===========================================================================

FUNCTION ENC_PCH_GAIN

DESCRIPTION
  Set reverse Pilot Channel gain

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_pch_gain
(
  word pch_gain        /* 8:0 of MOD_PCH_GAIN register is rev. pilot ch gain */
);


/*===========================================================================

FUNCTION ENC_SCH_GAIN

DESCRIPTION
  Set reverse Supplemental Channel gain

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_sch_gain
(
  word sch_gain        /* 8:0 of MOD_SCH_GAIN register is rev. sch gain */
);


/*===========================================================================

FUNCTION ENC_FCH_GAIN

DESCRIPTION
  Set reverse Fundamental Channel gain

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_fch_gain
(
  word fch_gain        /* 8:0 of MOD_FCH_GAIN register is rev. fch gain */
);


/*===========================================================================

FUNCTION ENC_IS95C_ENABLE

DESCRIPTION
  Turns the IS95C bit of FCH_CTL register on/off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_is95c_enable
(
  boolean is95c
);


/*===========================================================================

FUNCTION ENC_FCH_ENABLE

DESCRIPTION
  Turns the FCH_ENABLE bit of FCH_CTL register on/off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_fch_enable
(
   boolean enable_fch
);


/*===========================================================================

FUNCTION ENC_SCH_ENABLE

DESCRIPTION
  Turns the SCH_ENABLE bit of SCH_CTL register on/off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_sch_enable
(
   boolean enable_sch
);


/*===========================================================================

FUNCTION ENC_PCH_ENABLE

DESCRIPTION
   Turns the PCH_ENABLE bit of MISC_CTL register on/off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_pch_enable
(
   boolean enable_pch
);

/*===========================================================================

FUNCTION ENC_PCBIT_TEST_MASK

DESCRIPTION
   Sets the mask in MOD_PCBIT_TEST_MASK register. When a bit in this register
   is set to 1 the corresponding bit of MOD_PCBIT_TEST_DATA register is sent
   as power control bit on the reverse pilot channel.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_pcbit_test_mask
(
  word pcbit_test_mask
);


/*===========================================================================

FUNCTION ENC_PCBIT_TEST_DATA

DESCRIPTION
   Sets the contents of MOD_PCBIT_TEST_DATA

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_pcbit_test_data
(
   word pcbit_test_data
);

#ifdef FEATURE_IS2000_R_SCH

/*===========================================================================

FUNCTION ENC_SCH_MUXPDU5_DATA_OUT

DESCRIPTION
  Send data to the encoder for MUXPDU5, since for MUXPDU5, the size of the PDU can be
  odd, so this function can handle byte writing into the encoder RAM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_sch_muxpdu5_data_out
(
  byte *data_ptr,                /* Buffer to send to Encoder               */
  word total_numberofbyte        /* total number of sending byte */
);

/*===========================================================================

FUNCTION ENC_SCH_DATA_OUT_CFG

DESCRIPTION
   Program the MOD_SCH_CTL register

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_sch_data_out_cfg
(
  enc_sch_rate_type rate,        /* Rate to encode the data as              */
  boolean turbo_code,            /* TRUE= Turbo, FALSE=Convolutional        */
  word ltu_size,                 /* LTU block size with CRC in bits         */
  boolean generate_crc           /* Have the Encoder generate a CRC, or not */
);
#endif /* FEATURE_IS2000_R_SCH */


/*===========================================================================

FUNCTION ENC_IS95C_TX_PATN

DESCRIPTION
   Program the IS95C_TX_PATN register

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_is95c_tx_patn( word pattern );

/*===========================================================================

FUNCTION ENC_GATING_MODE

DESCRIPTION
   Program the MOD_MISC_CTL register with one of 3 mutually exclusive pilot
   gating modes: PILOT_GATING_MODE, FCH_8TH_RATE_GATING_MODE or 
   HHO_PREAMBLE_MODE, NO_GATING_MODE.

DEPENDENCIES
   The gating pattern must be specified using enc_is95c_tx_patn();
   
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_gating_mode( enc_gating_mode_type gating_mode );

/*===========================================================================

FUNCTION ENC_DCCH_ENABLE

DESCRIPTION
  Turns the DCCH_ENABLE bit of MOD_MISC_CTL register on/off

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_dcch_enable( boolean dcch_en );

/*===========================================================================

FUNCTION ENC_DCCH_DATA_OUT

DESCRIPTION
  Send DCCH data to the encoder, generate a CRC (or not), and encode it at
  a specified rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_dcch_data_out
(
  byte *data_ptr,               /* Buffer to send to Encoder               */
  boolean generate_crc,         /* Have the Encoder generate a CRC, or not */
  enc_rate_type rate            /* Rate to encode the data as              */
);

/*===========================================================================

FUNCTION ENC_DCCH_GAIN

DESCRIPTION
  Set DCCH gain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_dcch_gain( word dcch_gain );

/*===========================================================================

FUNCTION ENC_SET_TX_POLARITY

DESCRIPTION
  Set spectral polarity for TX.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_set_tx_polarity( enc_tx_polarity_type tx_polarity );

#ifndef T_MSM7800
/*===========================================================================

FUNCTION ENC_SET_RX_POLARITY

DESCRIPTION
  Set spectral polarity for RX.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_set_rx_polarity( enc_rx_polarity_type rx_polarity );
#endif /* !T_MSM7800 */

/*===========================================================================
FUNCTION enc_write_data

DESCRIPTION
  This function is used to write data into the encoder for a given channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.

===========================================================================*/
void enc_write_data 
           (  
             enc_chan_number_type  chan_num,     /* which channel will be written to */
             void                  *src_data_ptr, /* pointer points to source data, 16-bit-align */
             uint16                total_num_of_byte, /* Total number of byte */
             boolean               last_write /* indicated if this is the last write for a given channel */
           );
/*===========================================================================

FUNCTION enc_write_data_done

DESCRIPTION
  This function is used to indicate the HW writing to ENCODER is done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.

===========================================================================*/
void enc_write_data_done (void);

/*===========================================================================

FUNCTION ENC_INT_TIMING_OFFSET_PCG

DESCRIPTION
  This function enables the Encoder Interrupt at the specified PCG in a 20 ms 
  frame.  Input values are 0 to 15.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_int_timing_offset_pcg 
(
  word enc_pcg
);

/*===========================================================================

FUNCTION enc_hw_reset

DESCRIPTION
  This function resets the encoder HW block.  This step is necessary to avoid
  failed 1x access attempts between EVDO and 1X transition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.

===========================================================================*/
extern void enc_hw_reset (void);
#endif  /* ENC_H */

