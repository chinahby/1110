#ifndef ENCI_H
#define ENCI_H
/*===========================================================================

      E N C O D E R   I N T E R N A L   H E A D E R    F I L E

DESCRIPTION
   All the declarations and definitions necessary for the Encoder driver to
   interact with the QUALCOMM CDMA Interleaver / Encoder chip set

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 1992 through 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/inc/enci.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/09   jtm     Removed T_POSEIDON2 since this will only be supported 
                   in 1h09 branch
02/03/09   jtm     Added support for T_POSEIDON2
05/21/08   vlc     Added changes for 7800 modem to mainline.
10/16/06   trc     Add MSM7600 support
04/21/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
03/14/05   vlc     Merged in BTF value changes for MSM7500.  (dlb')
03/05/04   bn      Checked in encoder driver changes for MSM6700
09/03/03   dna     Ported for MSM6500
02/06/02   ems     Merged in changes for MSM6050.  Added register mappings for 
                   MSM6050 compatability
05/18/01   kk      RS2 TDSO support. (LTU size increased to 560)
04/26/01   vlc     Added ENC_SCH_RC_INX macro.
01/10/01   tc      Added ENC_INIT_BTF value for QRF3300
11/10/00   tc      Added MSM5100 modulator driver support
10/22/00   rmd     Cleaned up code, merged with MSM5000 code and moved bit definitions
                   to msm5XXXbits.h.
09/07/00   jcw     Added masks for VERY_EARLY_FRAME values
07/14/00   sr      Removed ENC_NO_GATING_V. This is replaced by 
                   ENC_NO_GATING_PATTERN in enc.h
05/18/00   sr      Changed ENC_NO_GATING_V to 0xFFFF from 0xFF
05/08/00   sr      Added support for MSM5000B
02/08/00   bgc     Added FEATURE_PLT_RF to control the BTF.
11/19/99   sr      Changes to support Supplemental Channels
11/04/99   lcc     Changed BTF values to the optimized value of 0xc80.
10/07/99   lcc     Merged from PLT 2.0.
09/23/99   lcc     Merged in MSM5000and PLT support from PLT archive.
11/05/98   dlb     Added support for MSM3000.
08/14/98   aks     Adjusted PA warmup time for QCP-1960 to 156uSec.
07/13/98   aks     Adjusted the PA warmup time for 5GP.  QCP-860 is 150uSec
                   and QCP-1960 is 299uSec.
06/30/98   aks     Shifted BTF for QCP-1960 by 0.6 uSec.
03/19/98   aks     Configured for Odie (T_O).
02/05/98   kss     Adjusted ENC_INIT_BTF_* for Module.
12/10/97   aks     Adjusted Q-800 BTF value, brought closer to 0.0us.
12/03/97   mk      Removed ENC_TEST_POINT_SEL, defined in deci.h.
11/24/97   aks     Adjusted TGP BTF value for cellular band.  Brought MSM22
                   closer to zero so that MSM23 does not exceed standards.
06/17/97   dhh     Added target T_Q for Q Phone.
06/16/97   aks     Added CRC disable.  Added ENC_TEST_POINT_SEL.  Changes are
                   related to MSM2.3.
03/14/97   jjn     Configured for Module (T_MD)
01/23/97   jah     BTF looks good, changed TGP PA_WARMUP to 200us.
11/02/96   jah     Configured for TGP (T_T), BTF still needs tuning.
02/07/96   jah     Added ENC_INIT_BTF_* for QCP-1000
01/10/96   jah     Changed ENC_INIT_BTF_PCS for QCP-1900 to 4,224 CHIPX8's
12/22/95   jah     Added ENC_INIT_BTF_PCS, for PCS BTF configuration.
10/15/95   jah     Updated Gemini PA_WARMUP to 638us to track GDAC change.
10/10/95   fkm     Minor Macro Changes to save ROM.
09/28/95   jah     Set Gemini PA_WARMUP to 600us to track GDAC change.
08/25/95   jah     Set Gemini & Beta-2 BTF values per measurement
08/24/95   jah     Set Gemini & ISS-2 PA_WARMUP to 100us
03/20/95   jah     Added support for 14400 rate set and MSM2p encoder mods.
06/28/94   jah     Added 1st guess PA_WARMUP values for ISS-2 and CSS-1
06/28/94   jah     Added PA_WARMUP values for Gemini
05/31/94   jah     Added PA_WARMUP values for ISS1.
05/05/93   jah     Added ENC_MASK_DATA_M
05/03/93   jah     Added ENC_MASK_DATA for T_MSM.
04/26/93   jah     Added changes for Beta II mobile.
04/17/93   jah     Set the mobile PA_WARMUP to 150us after Clarence/Todd/Matt's
                   measurements.  Portable set to 100us, to only allow for
                   tolerances spec'd by Mike/Ana.
03/09/93   jah     Changed ENC_PA_WARMUP_V and ENC_PCN_PA_WARMUP_V for the
                   portable/mobile/PCN RAS PA warm-up capability.
07/27/92   jah     Made enc_image[] private
01/30/92   jah     Ported module from brassboard enc.h

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "processor.h"  /* Definition of chip selects           */
#include "target.h"     /* Target specific definitions          */
#include "msm.h"        /* Register alias for MSM3000.          */


/*===========================================================================

                        MSM6050 Compatability

===========================================================================*/
#define DCCH_CRC_POLY_WH                     MOD_CH1_CRC_POLY_WH
#define DCCH_CRC_POLY_WH_MASK                MOD_CH1_CRC_POLY_WH_MASK  
#define FCH_CRC_POLY_WH                      MOD_CH2_CRC_POLY_WH
#define FCH_CRC_POLY_WH_MASK                 MOD_CH2_CRC_POLY_WH_MASK
#define SCH_CRC_POLY_WH                      MOD_CH3_CRC_POLY_WH
#define SCH_CRC_POLY_WH_MASK                 MOD_CH3_CRC_POLY_WH_MASK

#define MOD_DCCH_PUNC_PATTERN_0_WH           MOD_CH1_PUNCT_PATN_0_WH
#define MOD_DCCH_PUNC_PATTERN_0_WH_MASK      MOD_CH1_PUNCT_PATN_0_WH_MASK
#define MOD_FCH_PUNC_PATTERN_0_WH            MOD_CH2_PUNCT_PATN_0_WH
#define MOD_FCH_PUNC_PATTERN_0_WH_MASK       MOD_CH2_PUNCT_PATN_0_WH_MASK  
#define MOD_SCH_PUNC_PATTERN_0_WH            MOD_CH3_PUNCT_PATN_0_WH
#define MOD_SCH_PUNC_PATTERN_0_WH_MASK       MOD_CH3_PUNCT_PATN_0_WH_MASK
  
#define MOD_DCCH_PUNC_PATTERN_1_WH           MOD_CH1_PUNCT_PATN_1_WB   
#define MOD_DCCH_PUNC_PATTERN_1_WH_MASK      MOD_CH1_PUNCT_PATN_1_WB_MASK
#define MOD_FCH_PUNC_PATTERN_1_WH            MOD_CH2_PUNCT_PATN_1_WH   
#define MOD_FCH_PUNC_PATTERN_1_WH_MASK       MOD_CH2_PUNCT_PATN_1_WH_MASK
#define MOD_SCH_PUNC_PATTERN_1_WH            MOD_CH3_PUNCT_PATN_1_WH   
#define MOD_SCH_PUNC_PATTERN_1_WH_MASK       MOD_CH3_PUNCT_PATN_1_WH_MASK

#define MOD_FCH_ENC_CTL_WB                   MOD_CH2_ENC_CTL_1_WB
#define MOD_FCH_CTL_WH                       MOD_CH2_ENC_CTL_0_WH
#define MOD_FCH_CTL_WH_MASK                  MOD_CH2_ENC_CTL_0_WH_MASK
#define MOD_SCH_CTL_WH                       MOD_CH3_ENC_CTL_0_WH
#define MOD_SCH_CTL_WH_MASK                  MOD_CH3_ENC_CTL_0_WH_MASK

#define PA_ON_CTL_WB__PA_ON_WRITE_MASK       PA_ON_CTL_WB__PA_ON_EN_MASK
#define HW_MOD_ROTATOR_MAP                   MOD_ROTATOR_MAP_WB
#define HW_TX_VERY_EARLY_FRAME_CTL           TX_VERY_EARLY_FRAME_CTL_WH
#define HW_TX_2_EARLY_PCG_CTL                TX_2_EARLY_PCG_CTL_WH
#define HW_WSYM_STATE                        MOD_WSYM_STATE_WB
#define HW_MOD_CLK_CTL                       MOD_CLK_CTL_WH
#define HW_MOD_TEST_CTL                      MOD_TEST_CTL_WH
#define ENC_ADDR0                            MSM3_DUMMY
#define ENC_ADDR1                            MSM3_DUMMY
#define ENC_RAM_CTL                          MSM3_DUMMY
#define DMOD_SCAN_CTL_1                      MSM3_DUMMY

#define MOD_PCBIT_TEST_DATA_WH               MOD_PCH_PCBIT_DATA_WH
#define MOD_PCBIT_TEST_MASK_WH               MOD_PCH_PCBIT_MASK_WH
#define HW_MOD_STMR_CMD                      MOD_STMR_CMD_WB
#define WSYM_STATE_WB                        MOD_WSYM_STATE_WB
#define MOD_WALSHCOVER_SEL_WH                MOD_WCOVER_SEL_WH
#define MOD_STMR_MODIFIER_1_WH               MOD_STMR_MODIFIER_1_WB
 
#define MOD_DCCH_CTL_WH                      MOD_CH1_ENC_CTL_WH
#define MOD_DCCH_ENC_DATA_WH                 MOD_CH1_ENC_DATA_WH
#define MOD_DCCH_TIMING_CTL_WB               MOD_CH1_TIMING_CTL_WB  

#define MOD_FCH_ENC_DATA_WH                  MOD_CH2_ENC_DATA_WH
#define MOD_FCH_TIMING_CTL_WB                MOD_CH2_TIMING_CTL_WB  
#define MOD_FCH_CTL_WH__FCH_EN_MASK          MOD_CH2_ENC_CTL_0_WH__FCH_EN_MASK         
#define MOD_FCH_CTL_WH__FCH_CODE_RATE_MASK   MOD_CH2_ENC_CTL_0_WH__FCH_CODE_RATE_MASK  
#define MOD_FCH_CTL_WH__FCH_ENC_RATE_MASK    MOD_CH2_ENC_CTL_0_WH__FCH_ENC_RATE_MASK
#define MOD_FCH_CTL_WH__IS_95_C_MASK         MOD_CH2_ENC_CTL_0_WH__IS_95_C_MASK

#define MOD_DCCH_CTL_WH__DCCH_CRC_LEN_MASK   MOD_CH1_ENC_CTL_WH__CH1_CRC_LEN_MASK
#define MOD_DCCH_CTL_WH__DCCH_CODE_RATE_MASK MOD_CH1_ENC_CTL_WH__CH1_CODE_RATE_MASK
#define MOD_DCCH_CTL_WH__DCCH_RADIO_CFG_MASK MOD_CH1_ENC_CTL_WH__CH1_RADIO_CFG_MASK
#define MOD_DCCH_CTL_WH__DCCH_ENC_RATE_MASK  MOD_CH1_ENC_CTL_WH__CH1_ENC_RATE_MASK
#define MOD_DCCH_CTL_WH__DCCH_FRAME_SIZE_MASK MOD_CH1_ENC_CTL_WH__CH1_FRAME_SIZE_MASK
#define MOD_SCH_CTL_WH__TURBO_ENCODE_MASK    MOD_CH3_ENC_CTL_0_WH__TURBO_ENCODE_MASK
#define MOD_SCH_CTL_WH__SCH_LTU_EN_MASK      MOD_CH3_ENC_CTL_0_WH__SCH_LTU_EN_MASK 
#define MOD_SCH_CTL_WH__SCH_CODE_RATE_MASK   MOD_CH3_ENC_CTL_0_WH__SCH_CODE_RATE_MASK
#define MOD_SCH_CTL_WH__SCH_CRC_LENGTH_MASK  MOD_CH3_ENC_CTL_0_WH__SCH_CRC_LENGTH_MASK   
#define MOD_SCH_CTL_WH__SCH_EN_MASK          MOD_CH3_ENC_CTL_0_WH__SCH_EN_MASK
#define MOD_SCH_TIMING_CTL_WB                MOD_CH3_TIMING_CTL_WB
#define MOD_SCH_ENC_CTL_WH                   MOD_CH3_ENC_CTL_1_WH
#define MOD_SCH_ENC_DATA_WH                  HWIO_ADDR( MOD_CH3_ENC_DATA )

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* CHIPX8 delay for SYNC80M, biased +25, set by calling enc_set_btf()
** Value for the back-to-the-future counter for Init and CDMA mode
*/
#ifdef T_MSM7500
#error code not present
#else
#if defined(FEATURE_PLT) && !defined(FEATURE_PLT_RF)
#error code not present
#elif defined (FEATURE_QRF3300)
  #define ENC_INIT_BTF     ((dword)0x00000C95L)
  #define ENC_INIT_BTF_PCS ((dword)0x00000C94L)
#else  
  #define ENC_INIT_BTF     ((dword)0x00000C80L)
  #define ENC_INIT_BTF_PCS ((dword)0x00000C80L)
#endif
#endif /* T_MSM7500 */

/* Initial Transmit Frame interrupt is offset from the frame boundary by
** this offset in power control groups and walsh symbols
*/
#define ENC_INIT_PCG_T_OFF      15      /* Power control group time offset */
#define ENC_INIT_WS_T_OFF       0       /* Walsh symbol time offset        */

/* Variables for handling the time it takes to boot the emulator
*/
#define ENC_CHK_BOOT            5       /* Check if emulator booted every N ticks    */

#define ENC_BOOT_TOUT   (3000 / ENC_CHK_BOOT)   /* Timeout for emulator boot */

/* Initial I & Q PN state (set MSB of 15 bit register)
*/
#define ENC_PN_INIT_STATE       0x4000

/*---------------------------------------------------------------------------
                          Write Registers
----------------------------------------------------------------------------*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

  /* Warmup delay for the chip, time turned on before data is transmitted */

/* Time the PA needs to "warm up" before transmitting a power control
** groups worth of bits.  The equation is:
**
** PA_WARMUP_V = (153.6 * DELAY) + 64
**
** for a "warm up" time of (.15 + 10%) .15 ms = 0t87 = 0x57
**                                     .10 ms = 0t80 = 0x50
*/
#define ENC_PA_WARMUP_V         0xA2    /* .638 ms */
#define ENC_TX_WARMUP_V         0xA2
#define ENC_PCN_PA_WARMUP_V     0xA2    /* .638 ms */

#define ENC_LTU_MIN                 32
#define ENC_LTU_MAX                 1144

#define ENC_NUM_FRAC_RATES          4



#define ENC_FCH_RATE_POS      2
#define ENC_SCH_RATE_POS      1
#define ENC_RATE_SET_POS      4 
#define ENC_FCH_CODE_RATE_POS 5
#define ENC_FCH_CRCLEN_POS    8
#define ENC_SCH_CODE_RATE_POS 6
#define ENC_SCH_CRCLEN_POS    8
#define ENC_SCH_GAIN_POS      8
#define ENC_GATING_MODE_POS   10

#define ENC_FCH_ENCODE_TIME_POS       0      
#define ENC_FCH_PCG_COUNT_POS         2
#define ENC_FCH_RADIO_CONFIG_POS      2
#define ENC_FCH_FRAME_SIZE_POS        0
#define ENC_SCH_ENCODE_TIME_POS       0      
#define ENC_SCH_PCG_COUNT_POS         2
#define ENC_SCH_RATE_SET_POS          5 
#define ENC_SCH_SELECT_POS            0
#define ENC_SCH_FRAME_SIZE_POS        2
#define ENC_SCH_VARIABLE_RATE_POS     4
#define ENC_SCH_INTLV_SIZE_POS        5
#define ENC_SCH_LOW_REPEAT_POS        9
#define ENC_SCH_MULTIFRAME_POS        12
#define ENC_SCH_WALSHCOVER_POS        14

  /* Rel C DV hardware allows encoder interrupt to happen every PCG in a 20ms
  ** frame.  Concept of 5ms bin is replaced by specifying the PCG, ranging
  ** in value from 0 to 15.  Need these defines to be backwards compatible
  ** with non Rel C code.
  */
#define ENC_INT_BIN_MASK_VALUE_BIN_0 0x000f      /* 20 ms bin 0    */
#define ENC_INT_BIN_MASK_VALUE_BIN_1 0x00f0      /* 20 ms bin 1    */
#define ENC_INT_BIN_MASK_VALUE_BIN_2 0x0f00      /* 20 ms bin 2    */
#define ENC_INT_BIN_MASK_VALUE_BIN_3 0xf000      /* 20 ms bin 3    */

#define ENC_INT_BIN_0 0x0001                     /* 20 ms bin 0    */
#define ENC_INT_BIN_1 0x0002                     /* 20 ms bin 1    */
#define ENC_INT_BIN_2 0x0004                     /* 20 ms bin 2    */
#define ENC_INT_BIN_3 0x0008                     /* 20 ms bin 3    */


#if defined(T_MSM7600)
#error code not present
#endif  /* T_MSM7600 */

/*===========================================================================

                      TYPE DECLARATIONS

===========================================================================*/
typedef struct
{
   byte length;
   word poly;
} enc_crc_info_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*-------------------------------------------------------------------------*/

/*===========================================================================

                      MACRO FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

MACRO ENC_OUT

DESCRIPTION
  This macro outputs a specified value to a specified port. All 8 bits of the
  port are written (i.e., the port is not buffered by software).

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()
  val   Value to write the Encoder register

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_OUT( io, val )  MSM_OUT( io, val )


/*===========================================================================

MACRO ENC_OUTM

DESCRIPTION
  This macro outputs a specified value, qualified by a specified mask, to
  a specified port address. Only the bits corresponding to the mask are
  actually affected; other bits retain their previous values. To do this,
  an image is maintained of the previous value written to the port which
  is then combined with the new value.

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()
  mask  Mask of bits in Encoder register to write
  val   Value to write the Encoder register

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define ENC_OUTM(io, mask, val)  MSM_OUTM(io, mask, val)

//

/*===========================================================================

MACRO ENC_OUTH

DESCRIPTION
  This macro outputs a specified value to a specified port. All 16 bits of the
  port are written (i.e., the port is not buffered by software).

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()
  val   Value to write the Encoder register

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_OUTH( io, val )  MSM_OUTH( io, val )


/*===========================================================================

MACRO ENC_OUTHM

DESCRIPTION
  This macro outputs a specified value, qualified by a specified mask, to
  a specified port address. Only the bits corresponding to the mask are
  actually affected; other bits retain their previous values. To do this,
  an image is maintained of the previous value written to the port which
  is then combined with the new value.

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()
  mask  Mask of bits in Encoder register to write
  val   Value to write the Encoder register

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define ENC_OUTHM(io, mask, val)  MSM_OUTHM(io, mask, val)


/*===========================================================================

MACRO ENC_IN

DESCRIPTION
  This macro reads and returns the value from a specified port. No mask is
  applied to the read value.

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  byte read from the Encoder register.

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_IN( io )  MSM_IN( io )


/*===========================================================================

MACRO ENC_INM

DESCRIPTION
  This macro reads the value from a specified I/O port address and then
  applies a specified mask to the value. Bits not corresponding to the
  mask are returned as zero.

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()
  mask  Mask of bits in Encoder register to read

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  byte read from the Encoder register.

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_INM( io, mask )  MSM_INM( io, mask )


/*===========================================================================

MACRO ENC_FCH_RC_INX

DESCRIPTION
   This converts the 1-based enum to a 0 based index for array
   subscripting

PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_FCH_RC_INX( rc )  ( ( rc ) - 1 )


/*===========================================================================

MACRO ENC_FCH_RATE_INX

DESCRIPTION
   This converts the old defintions of fractional rates to what is
   understood by MSM5000.

PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_FCH_RATE_INX( rate )  ( 3 - ( rate ) )


/*===========================================================================

MACRO ENC_SCH_RC_INX

DESCRIPTION
   This converts the 3-based enum to a 0 based index for array
   subscripting

PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_SCH_RC_INX( rc )  ( ( rc ) - 3 )

#endif  /* ENCI_H */

