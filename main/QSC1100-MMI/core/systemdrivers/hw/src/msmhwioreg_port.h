#ifndef MSMHWIOREG_PORT_H
#define MSMHWIOREG_PORT_H

/*===========================================================================

           MSM6500 REGISTER PORTING HEADER FILE

DESCRIPTION
  This file contains hand ported registers for MSM6550

Copyright (c) 2004 - 2005   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*==========================================================================

    PLEASE DO NOT USE THESE REGISTERS IN THIS FILE WITHOUT ANY REVIEW 
	APPROPRIATE SOFTWARE INTERFACE DOCUMENT AS THESE REGISTERS ARE NOT
	DIRECTLY AUTOGENATED FROM THE CHIP ADDRESS FILES.

    INITIALLY, IT IS AVAILABLE FOR GETTING QSC1100 500 BUILD COMPILED
	WITHOUT ERRORS. 

	THE GOAL IS TO REMOVE THESE REGISTERS FROM THIS FILE ASAP!!!

    IF YOU GO THROUGH THIS FILE, AND SEE ERRO COMMENTS FROM COMPILER ADDED
	ABOVE REGISTERS, PLEASE DO NOT USE THEM!!!

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/msm6050redefs.h_v   1.6   20 Mar 2002 17:58:20   dlee  $
$Header: //depot/asic/qsc1100/drivers/hw/msmhwioreg_port.h#20 $ $DateTime: 2008/04/18 13:24:00 $ $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ps      Removed port registers used by SECBOOT
06/28/07   hvo     Cleaned up, rearranged registers into particular sections
05/16/07   hvo     Added registers for QSC1100 port, no one should use these
                   added registers
01/30/07   cr      Add MICRO_TO_MDSP_IRQ port.
01/18/07   cr      Add second LED port intended for use by BIO LED output.
12/13/06   cr      Add uart_dm block as copy/paste from msmhwioreg.h to support
                   uart_dm name difference. Fix uart2 block bit fields.
10/18/05   rp      Removed obsolete defines. Added  defines for
                   internal RAM
07/23/05   wd      L4 Merge.
05/19/05   wd      Support for L4 linex.
04/20/05   ejv     Removed HDR_SLEEP defines.
04/18/05   bt      Changed access to register SAMPSRV_n_LATEST_RTC.
04/11/05   rp      Used the same tramp table defines used in 7500 for
                   making unified tramp design between 7500 and 6800 
03/09/05   rp      Removed obsolete defines
08/18/04   ap      Added defines for updated USB core.
06/22/04   jkl     Removed obsolete definitions.
05/27/04   ap      Added HWIO_USB_ENDPOINT_READY_CLR_ADDR support.
04/19/04   AP      Added definitions for 6550.
===========================================================================*/

/* This is where all the hand edit pieces of msmhwioreg.h belong.
 *
 *
*/
/* ..\..\drivers\clkregim\clkrgm_msm.c, hvo */
#ifdef T_SURF    
  
  /* Add an encapsulation for SURF FPGA register(s) required by USBOTG */
  #define HWIO_HAPPY_LED_ADDR           EBI2_CS0_BASE
  #define HWIO_HAPPY_LED_RMSK           0xFFFF
  #define HWIO_HAPPY_LED_SHFT           0
  #define HWIO_HAPPY_LED_IN                                    \
          in_word(HWIO_HAPPY_LED_ADDR)
  #define HWIO_HAPPY_LED_INM(m)                                \
          in_dword_masked(HWIO_HAPPY_LED_ADDR,m)
  #define HWIO_HAPPY_LED_OUT(v)                                \
          out_word(HWIO_HAPPY_LED_ADDR,v)
  #define HWIO_HAPPY_LED_OUTM(m,v)                             \
      out_word_masked(HWIO_HAPPY_LED_ADDR,m,v,HWIO_HAPPY_LED_shadow)

#endif /* T_SURF */

/* They are used by qdspcmd.c, hvo */
#define HWIO_CODEC_CTL_OUTM(m,v)              HWIO_AUX_CODEC_CTL_OUTM(m,v)
#define HWIO_CODEC_CTL_ADSP_CODEC_CTL_EN_BMSK HWIO_AUX_CODEC_CTL_ADSP_CODEC_CTL_EN_BMSK
#define HWIO_CODEC_CTL_ONES_POLARITY_BMSK     HWIO_AUX_CODEC_CTL_ONES_POLARITY_BMSK 

/* I think this register no longer exist in QSC1100, IRQ registers handle it, hvo */
#define HWIO_UART_INT_ENABLE_ADDR                                         0x28000000
#define HWIO_UART_INT_ENABLE_RMSK                                                0xf
#define HWIO_UART_INT_ENABLE_SHFT                                                  0
#define HWIO_UART_INT_ENABLE_IN                                           \
        in_dword_masked(HWIO_UART_INT_ENABLE_ADDR, HWIO_UART_INT_ENABLE_RMSK)
#define HWIO_UART_INT_ENABLE_INM(m)                                       \
        in_dword_masked(HWIO_UART_INT_ENABLE_ADDR, m)
#define HWIO_UART_INT_ENABLE_OUT(v)                                       \
        out_dword(HWIO_UART_INT_ENABLE_ADDR,v)
#define HWIO_UART_INT_ENABLE_OUTM(m,v)                                    \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_INT_ENABLE_ADDR,m,v,HWIO_UART_INT_ENABLE_IN); \
		HWIO_INTFREE()
#define HWIO_UART_INT_ENABLE_UART2_DP_RX_DATA_INT_BMSK                           0x8
#define HWIO_UART_INT_ENABLE_UART2_DP_RX_DATA_INT_SHFT                           0x3


/* These UART registers no longer exist in QSC1100, IRQ handles them */
#define HWIO_UART_INT_POL_REG_ADDR                                        0x28000000
#define HWIO_UART_INT_POL_REG_RMSK                                               0x7
#define HWIO_UART_INT_POL_REG_SHFT                                                 0
#define HWIO_UART_INT_POL_REG_IN                                          \
        in_dword_masked(HWIO_UART_INT_POL_REG_ADDR, HWIO_UART_INT_POL_REG_RMSK)
#define HWIO_UART_INT_POL_REG_INM(m)                                      \
        in_dword_masked(HWIO_UART_INT_POL_REG_ADDR, m)
#define HWIO_UART_INT_POL_REG_OUT(v)                                      \
        out_dword(HWIO_UART_INT_POL_REG_ADDR,v)
#define HWIO_UART_INT_POL_REG_OUTM(m,v)                                   \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_INT_POL_REG_ADDR,m,v,HWIO_UART_INT_POL_REG_IN); \
		HWIO_INTFREE()
#define HWIO_UART_INT_POL_REG_UART3_DP_RX_DATA_INT_BMSK                          0x4
#define HWIO_UART_INT_POL_REG_UART3_DP_RX_DATA_INT_SHFT                          0x2
#define HWIO_UART_INT_POL_REG_UART2_DP_RX_DATA_INT_BMSK                          0x2
#define HWIO_UART_INT_POL_REG_UART2_DP_RX_DATA_INT_SHFT                          0x1

#ifdef FEATURE_SC2X_HAS_UART1
#define HWIO_UART_INT_POL_REG_UART1_DP_RX_DATA_INT_SHFT                            0
#endif


#define HWIO_UART_INT_DET_REG_ADDR                                        0x28000000
#define HWIO_UART_INT_DET_REG_RMSK                                               0xf
#define HWIO_UART_INT_DET_REG_SHFT                                                 0
#define HWIO_UART_INT_DET_REG_IN                                          \
        in_dword_masked(HWIO_UART_INT_DET_REG_ADDR, HWIO_UART_INT_DET_REG_RMSK)
#define HWIO_UART_INT_DET_REG_INM(m)                                      \
        in_dword_masked(HWIO_UART_INT_DET_REG_ADDR, m)
#define HWIO_UART_INT_DET_REG_OUT(v)                                      \
        out_dword(HWIO_UART_INT_DET_REG_ADDR,v)
#define HWIO_UART_INT_DET_REG_OUTM(m,v)                                   \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_INT_DET_REG_ADDR,m,v,HWIO_UART_INT_DET_REG_IN); \
		HWIO_INTFREE()
#define HWIO_UART_INT_DET_REG_UART2_DP_RX_DATA_INT_BMSK                          0x8
#define HWIO_UART_INT_DET_REG_UART2_DP_RX_DATA_INT_SHFT                          0x3

#ifdef FEATURE_SC2X_HAS_UART1
#define HWIO_UART_INT_DET_REG_UART1_DP_RX_DATA_INT_SHFT                          0x2
#endif
#define HWIO_UART_INT_DET_REG_UART2_INT_BMSK                                     0x2
#define HWIO_UART_INT_DET_REG_UART2_INT_SHFT                                     0x1

#ifdef FEATURE_SC2X_HAS_UART1
#define HWIO_UART_INT_DET_REG_UART1_INT_SHFT                                       0
#endif

#define HWIO_UART_INT_CLR_REG_ADDR                                        0x28000000
#define HWIO_UART_INT_CLR_REG_RMSK                                               0xf
#define HWIO_UART_INT_CLR_REG_SHFT                                                 0
#define HWIO_UART_INT_CLR_REG_OUT(v)                                      \
        out_dword(HWIO_UART_INT_CLR_REG_ADDR,v)
#define HWIO_UART_INT_CLR_REG_OUTM(m,v)                                   \
        out_dword_masked(HWIO_UART_INT_CLR_REG_ADDR,m,v,HWIO_UART_INT_CLR_REG_shadow)
#define HWIO_UART_INT_CLR_REG_UART2_DP_RX_DATA_INT_BMSK                          0x8
#define HWIO_UART_INT_CLR_REG_UART2_DP_RX_DATA_INT_SHFT                          0x3

#define HWIO_UART_INT_DET_REG_UART2_INT_BMSK                                     0x2
#define HWIO_UART_INT_STATUS_UART2_INT_BMSK                                      0x2
#define HWIO_UART_INT_CLR_REG_UART2_INT_BMSK                                     0x2



/* end p1r9 to p2r0 */

          /* Since chip address file does not provide bit field definitions
           * for IRQ_0, I provide them here. This bit order is copied from 

              Morpheus Modem Core (SC2x)
              Hardware Description Document
              80-V7864-29A Rev A
              August 9, 2006

           */
/* Being used by gpio_int_msm module, hvo */
#define GP1_BASE                                                           EBI2_GP1_BASE

   /* SC2X port. Contains ported and non-ported registers/bits. Parts that
   * are ported should be conditionally compiled under FEATURE_SC2X_SW_PORT
   * only. Parts that are not yet ported should be found under
   * FEATURE_SC2X_SW_*_NOT_PORT. 
   *
   * If a portion of the hardware interface is ported in the corresponding 
   * software subsystem, then remove those registers/bits from the 
   * FEATURE_SC2X_SW_*_NOT_PORT part of this file. 
   *
   * If a portion of the hardware interface is ported in this file, then
   * move that portion out of the NOT_PORT conditionally compilation so
   * that it is only conditionally compiled under FEATURE_SC2X_SW_PORT.
   */
#ifdef FEATURE_SC2X_SW_PORT


#define EBI2_GP1_BASE EBI2_CS0_BASE
#define EBI2_GP2_BASE EBI2_CS2_BASE

  /* The following RTC_INT_* is a straightforward port; RTC_INT_*2 went away 
   * and bits previously in those registers are now in RTC_INT_*. This is 
   * just a straightfoward name change from INT_*[1,2] to INT_*.
   * So, it is reasonable, and I hope safe, to do that part of
   * the HW interface port here.
   */
#define HWIO_RTC_INT_CLR1_ADDR                        \
        HWIO_RTC_INT_CLEAR_ADDR
#define HWIO_RTC_INT_CLR1_OUT(v)                      \
        out_dword(HWIO_RTC_INT_CLEAR_ADDR,v)
#define HWIO_RTC_INT_STATUS1_ADDR                     \
        HWIO_RTC_INT_STATUS_ADDR
#define HWIO_RTC_INT_CLR1_RTC_CMP1_BMSK               \
        HWIO_RTC_INT_CLEAR_RTC_CMP1_BMSK
#define HWIO_RTC_INT_STATUS1_RTC_CMP1_BMSK            \
        HWIO_RTC_INT_STATUS_RTC_CMP1_BMSK
#define HWIO_RTC_INT_CLR1_RTC_CMP2_BMSK               \
        HWIO_RTC_INT_CLEAR_RTC_CMP2_BMSK
#define HWIO_RTC_INT_STATUS1_RTC_CMP2_BMSK            \
        HWIO_RTC_INT_STATUS_RTC_CMP2_BMSK
#define HWIO_RTC_INT_CLR1_DEM_GP_CMP_BMSK             \
        HWIO_RTC_INT_CLEAR_DEM_GP_CMP_BMSK
#define HWIO_RTC_INT_STATUS1_DEM_GP_CMP_BMSK          \
        HWIO_RTC_INT_STATUS_DEM_GP_CMP_BMSK
#define HWIO_RTC_INT_CLR2_PN_ROLLOVER_1X_BMSK         \
        HWIO_RTC_INT_CLEAR_PN_ROLLOVER_1X_BMSK
#define HWIO_RTC_INT_STATUS2_PN_ROLLOVER_1X_BMSK      \
        HWIO_RTC_INT_STATUS_PN_ROLLOVER_1X_BMSK
#define HWIO_RTC_INT_CLR2_PN_ROLLOVER_HDR_BMSK        \
        HWIO_RTC_INT_CLEAR_PN_ROLLOVER_HDR_BMSK
#define HWIO_RTC_INT_STATUS2_PN_ROLLOVER_HDR_BMSK     \
        HWIO_RTC_INT_STATUS_PN_ROLLOVER_HDR_BMSK
#define HWIO_RTC_INT_CLR2_ADDR                        \
        HWIO_RTC_INT_CLR1_ADDR
#define HWIO_RTC_INT_CLR2_OUT(v)                      \
        HWIO_RTC_INT_CLR1_OUT(v)
#define HWIO_RTC_INT_STATUS2_ADDR                     \
        HWIO_RTC_INT_STATUS1_ADDR
#define HWIO_RTC_INT_CLR2_ADDR                        \
        HWIO_RTC_INT_CLR1_ADDR
#define HWIO_RTC_INT_STATUS2_ADDR                     \
        HWIO_RTC_INT_STATUS1_ADDR
#define HWIO_RTC_INT_STATUS2_IN                       \
        HWIO_RTC_INT_STATUS_IN

#ifdef FEATURE_SC2X_HAS_UART1
#define HWIO_UART1_DM_FROMMN_CLK_NS_CLK_SRC_SEL_BMSK                             0xf
#endif
#ifdef FEATURE_SC2X_HAS_UART1
#define HWIO_UART1_DM_FROMMN_CLK_NS_CLK_SRC_SEL_TCXO_FVAL                          0
#endif
#ifdef FEATURE_SC2X_HAS_UART1
#define HWIO_UART1_DM_FROMMN_CLK_NS_CLK_SRC_SEL_SHFT                               0
#endif

// vphan: ring.c needs HWIO_RINGER_MN_A_NDIV_OUT
#define HWIO_RINGER_MN_A_NDIV_ADDR                                        0x28000000
#define HWIO_RINGER_MN_A_NDIV_OUT(v)                                      \
        out_dword(HWIO_RINGER_MN_A_NDIV_ADDR,v)

// vphan: ring.c needs HWIO_RINGER_MN_A_DUTY_OUT
#define HWIO_RINGER_MN_A_DUTY_ADDR                                        0x28000000
#define HWIO_RINGER_MN_A_DUTY_OUT(v)                                      \
        out_dword(HWIO_RINGER_MN_A_DUTY_ADDR,v)

// vphan: boothw_target.c MPM_CLK_SLEEPOSC
#define HWIO_MPM_CLK_SLEEPOSC_ADDR                                        0x28000000
#define HWIO_MPM_CLK_SLEEPOSC_RMSK                                              0x3f
#define HWIO_MPM_CLK_SLEEPOSC_SHFT                                                 0
#define HWIO_MPM_CLK_SLEEPOSC_IN                                          \
        in_dword_masked(HWIO_MPM_CLK_SLEEPOSC_ADDR, HWIO_MPM_CLK_SLEEPOSC_RMSK)
#define HWIO_MPM_CLK_SLEEPOSC_INM(m)                                      \
        in_dword_masked(HWIO_MPM_CLK_SLEEPOSC_ADDR, m)
#define HWIO_MPM_CLK_SLEEPOSC_OUT(v)                                      \
        out_dword(HWIO_MPM_CLK_SLEEPOSC_ADDR,v)
#define HWIO_MPM_CLK_SLEEPOSC_OUTM(m,v)                                   \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_MPM_CLK_SLEEPOSC_ADDR,m,v,HWIO_MPM_CLK_SLEEPOSC_IN); \
		HWIO_INTFREE()
#define HWIO_MPM_CLK_SLEEPOSC_SLEEP_OSC_GAIN_BMSK                               0x30
#define HWIO_MPM_CLK_SLEEPOSC_SLEEP_OSC_GAIN_SHFT                                0x4
#define HWIO_MPM_CLK_SLEEPOSC_SLEEP_OSC_GAIN_MINIMUM_GAIN_FVAL                     0
#define HWIO_MPM_CLK_SLEEPOSC_SLEEP_OSC_GAIN_MAXIMUM_GAIN_FVAL                   0x3
#define HWIO_MPM_CLK_SLEEPOSC_SLEEP_OSC_RF_BYPASS_BMSK                           0x8
#define HWIO_MPM_CLK_SLEEPOSC_SLEEP_OSC_RF_BYPASS_SHFT                           0x3
#define HWIO_MPM_CLK_SLEEPOSC_SLEEP_OSC_RD_BYPASS_BMSK                           0x4
#define HWIO_MPM_CLK_SLEEPOSC_SLEEP_OSC_RD_BYPASS_SHFT                           0x2
#define HWIO_MPM_CLK_SLEEPOSC_SLEEP_XTAL_EN_BMSK                                 0x2
#define HWIO_MPM_CLK_SLEEPOSC_SLEEP_XTAL_EN_SHFT                                 0x1
#define HWIO_MPM_CLK_SLEEPOSC_WRITE_DISABLE_STATUS_BMSK                          0x1
#define HWIO_MPM_CLK_SLEEPOSC_WRITE_DISABLE_STATUS_SHFT                            0

// vphan: ftm_im2_lib.c need MSM_CLK_HALTA
#define HWIO_MSM_CLK_HALTA_ADDR                                           0x28000000
#define HWIO_MSM_CLK_HALTA_RMSK                                           0x1fffffff
#define HWIO_MSM_CLK_HALTA_SHFT                                                    0
#define HWIO_MSM_CLK_HALTA_IN                                             \
        in_dword_masked(HWIO_MSM_CLK_HALTA_ADDR, HWIO_MSM_CLK_HALTA_RMSK)
#define HWIO_MSM_CLK_HALTA_INM(m)                                         \
        in_dword_masked(HWIO_MSM_CLK_HALTA_ADDR, m)
#define HWIO_MSM_CLK_HALTA_OUT(v)                                         \
        out_dword(HWIO_MSM_CLK_HALTA_ADDR,v)
#define HWIO_MSM_CLK_HALTA_OUTM(m,v)                                      \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_MSM_CLK_HALTA_ADDR,m,v,HWIO_MSM_CLK_HALTA_IN); \
		HWIO_INTFREE()

// vphan: hwi.c needs MSM_CLK_HALTA_ADSP_HM_CLK_HALT_BMSK
#define HWIO_MSM_CLK_HALTA_ADSP_HM_CLK_HALT_BMSK                                 0x1
#define HWIO_MSM_CLK_HALTA_ADSP_HM_CLK_HALT_SHFT                                   0

// vphan: rf_mm_cdma1x.c needs PA_ON_CTL_BYPASS
/* They are used by enc.c, hvo */
#ifndef T_QSC6055
  #define HWIO_SYSTEM_MODE_ADDR                                         (CHIP_BASE+0x708)
  #define HWIO_PA_ON_STATUS_ADDR                                        (CHIP_BASE+0x720)
#else
  #define HWIO_PA_ON_STATUS_ADDR                                        HWIO_MODEM_PA_ON_STATUS_ADDR
#endif
// rp  tramp defines that differs from 6275.
#define PA_ON_CTL               MODEM_PA_ON_CTL
#define PA_ON_STATUS            MODEM_PA_ON_STATUS
#define HWIO_MODEM_PA_ON_CTL_BYPASS_TX_PUNCT_BMSK                          HWIO_MODEM_PA_ON_CTL_BYPASS_TX_VALID_BMSK 
#define HWIO_MODEM_PA_ON_CTL_BYPASS_TX_PUNCT_SHFT                          HWIO_MODEM_PA_ON_CTL_BYPASS_TX_VALID_SHFT
#define HWIO_MODEM_PA_ON_CTL_BYPASS_PA_PUNCT_RAW_BMSK                      HWIO_MODEM_PA_ON_CTL_BYPASS_PA_VALID_BMSK
#define HWIO_MODEM_PA_ON_CTL_BYPASS_PA_PUNCT_RAW_SHFT                      HWIO_MODEM_PA_ON_CTL_BYPASS_PA_VALID_SHFT
#define HWIO_MODEM_PA_ON_CTL_BYPASS_PA_SYNTH_LOCK_RAW_BMSK                 HWIO_MODEM_PA_ON_CTL_BYPASS_PA_SYNTH_LOCK_BMSK
#define HWIO_MODEM_PA_ON_CTL_BYPASS_PA_SYNTH_LOCK_RAW_SHFT                 HWIO_MODEM_PA_ON_CTL_BYPASS_PA_SYNTH_LOCK_SHFT


// vphan: rflib_adc_samp_freq.c need
// Bit fields being removed from CAF released 1.27
#define HWIO_RXF_SRCc_CTL_NOTCH_EN_BMSK                                    0x2000000
#define HWIO_RXF_SRCc_CTL_NOTCH_EN_SHFT                                         0x19

#define HWIO_RXF_SRCc_CTL_NOTCH2_EN_BMSK                                   0x4000000
#define HWIO_RXF_SRCc_CTL_NOTCH2_EN_SHFT                                        0x1a

/* UARTs registers section please DO NOT USE THEM, they are added
   for compiling the first QSC1100 build 
 */ 

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_UART2_MR1_ADDR                                                0x28000000
#define HWIO_UART2_MR1_PHYS                                                0x28000000
#else
#define HWIO_UART2_MR1_ADDR                                                0x28000000
#endif
#define HWIO_UART2_MR1_RMSK                                                   0x3ffff
#define HWIO_UART2_MR1_SHFT                                                         0
#define HWIO_UART2_MR1_IN                                                  \
        in_dword_masked(HWIO_UART2_MR1_ADDR, HWIO_UART2_MR1_RMSK)
#define HWIO_UART2_MR1_INM(m)                                              \
        in_dword_masked(HWIO_UART2_MR1_ADDR, m)
#define HWIO_UART2_MR1_OUT(v)                                              \
        out_dword(HWIO_UART2_MR1_ADDR,v)
#define HWIO_UART2_MR1_OUTM(m,v)                                           \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART2_MR1_ADDR,m,v,HWIO_UART2_MR1_IN); \
		HWIO_INTFREE()
#define HWIO_UART2_MR1_AUTO_RFR_LEVEL1_BMSK                                   0x3ff00
#define HWIO_UART2_MR1_AUTO_RFR_LEVEL1_SHFT                                       0x8
#define HWIO_UART2_MR1_RX_RDY_CTL_BMSK                                           0x80
#define HWIO_UART2_MR1_RX_RDY_CTL_SHFT                                            0x7
#define HWIO_UART2_MR1_CTS_CTL_BMSK                                              0x40
#define HWIO_UART2_MR1_CTS_CTL_SHFT                                               0x6
#define HWIO_UART2_MR1_AUTO_RFR_LEVEL0_BMSK                                      0x3f
#define HWIO_UART2_MR1_AUTO_RFR_LEVEL0_SHFT                                         0

#define HWIO_UART2_MR2_ADDR                                                0x28000000
#define HWIO_UART2_MR2_RMSK                                                      0xff
#define HWIO_UART2_MR2_SHFT                                                         0
#define HWIO_UART2_MR2_IN                                                  \
        in_dword_masked(HWIO_UART2_MR2_ADDR, HWIO_UART2_MR2_RMSK)
#define HWIO_UART2_MR2_INM(m)                                              \
        in_dword_masked(HWIO_UART2_MR2_ADDR, m)
#define HWIO_UART2_MR2_OUT(v)                                              \
        out_dword(HWIO_UART2_MR2_ADDR,v)
#define HWIO_UART2_MR2_OUTM(m,v)                                           \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART2_MR2_ADDR,m,v,HWIO_UART2_MR2_IN); \
		HWIO_INTFREE()
#define HWIO_UART2_MR2_LOOPBACK_BMSK                                             0x80
#define HWIO_UART2_MR2_LOOPBACK_SHFT                                              0x7
#define HWIO_UART2_MR2_ERROR_MODE_BMSK                                           0x40
#define HWIO_UART2_MR2_ERROR_MODE_SHFT                                            0x6
#define HWIO_UART2_MR2_BITS_PER_CHAR_BMSK                                        0x30
#define HWIO_UART2_MR2_BITS_PER_CHAR_SHFT                                         0x4
#define HWIO_UART2_MR2_BITS_PER_CHAR_5_BITS_FVAL                                    0
#define HWIO_UART2_MR2_BITS_PER_CHAR_6_BITS_FVAL                                  0x1
#define HWIO_UART2_MR2_BITS_PER_CHAR_7_BITS_FVAL                                  0x2
#define HWIO_UART2_MR2_BITS_PER_CHAR_8_BITS_FVAL                                  0x3
#define HWIO_UART2_MR2_STOP_BIT_LEN_BMSK                                          0xc
#define HWIO_UART2_MR2_STOP_BIT_LEN_SHFT                                          0x2
#define HWIO_UART2_MR2_PARITY_MODE_BMSK                                           0x3
#define HWIO_UART2_MR2_PARITY_MODE_SHFT                                             0

#define HWIO_UART2_CSR_ADDR                                                0x28000000
#define HWIO_UART2_CSR_RMSK                                                      0xff
#define HWIO_UART2_CSR_SHFT                                                         0
#define HWIO_UART2_CSR_OUT(v)                                              \
        out_dword(HWIO_UART2_CSR_ADDR,v)
#define HWIO_UART2_CSR_OUTM(m,v)                                           \
        out_dword_masked(HWIO_UART2_CSR_ADDR,m,v,HWIO_UART_CSR_shadow)
#define HWIO_UART2_CSR_UART_RX_CLK_SEL_BMSK                                      0xf0
#define HWIO_UART2_CSR_UART_RX_CLK_SEL_SHFT                                       0x4
#define HWIO_UART2_CSR_UART_TX_CLK_SEL_BMSK                                       0xf
#define HWIO_UART2_CSR_UART_TX_CLK_SEL_SHFT                                         0

// vphan: ap_uart.c needs UART2_TF
#define HWIO_UART2_TF_ADDR                                                 0x28000000
#define HWIO_UART2_TF_RMSK                                                       0xff
#define HWIO_UART2_TF_SHFT                                                          0
#define HWIO_UART2_TF_OUT(v)                                               \
        out_dword(HWIO_UART2_TF_ADDR,v)
#define HWIO_UART2_TF_OUTM(m,v)                                            \
        out_dword_masked(HWIO_UART2_TF_ADDR,m,v,HWIO_UART_TF_shadow)
#define HWIO_UART2_TF_DATA_BMSK                                                  0xff
#define HWIO_UART2_TF_DATA_SHFT                                                     0

// vphan: ap_uart.c needs UART2_CR
#define HWIO_UART2_CR_ADDR                                                 0x28000000
#define HWIO_UART2_CR_RMSK                                                      0x1ff
#define HWIO_UART2_CR_SHFT                                                          0
#define HWIO_UART2_CR_OUT(v)                                               \
        out_dword(HWIO_UART2_CR_ADDR,v)
#define HWIO_UART2_CR_OUTM(m,v)                                            \
        out_dword_masked(HWIO_UART2_CR_ADDR,m,v,HWIO_UART_CR_shadow)
#define HWIO_UART2_CR_CHANNEL_COMMAND_BMSK                                      0x1f0
#define HWIO_UART2_CR_CHANNEL_COMMAND_SHFT                                        0x4
#define HWIO_UART2_CR_UART_TX_DISABLE_BMSK                                        0x8
#define HWIO_UART2_CR_UART_TX_DISABLE_SHFT                                        0x3
#define HWIO_UART2_CR_UART_TX_EN_BMSK                                             0x4
#define HWIO_UART2_CR_UART_TX_EN_SHFT                                             0x2
#define HWIO_UART2_CR_UART_RX_DISABLE_BMSK                                        0x2
#define HWIO_UART2_CR_UART_RX_DISABLE_SHFT                                        0x1
#define HWIO_UART2_CR_UART_RX_EN_BMSK                                             0x1
#define HWIO_UART2_CR_UART_RX_EN_SHFT                                               0

#define HWIO_UART2_IMR_ADDR                                                0x28000000
#define HWIO_UART2_IMR_RMSK                                                     0x1ff
#define HWIO_UART2_IMR_SHFT                                                         0
#define HWIO_UART2_IMR_OUT(v)                                              \
        out_dword(HWIO_UART2_IMR_ADDR,v)
#define HWIO_UART2_IMR_OUTM(m,v)                                           \
        out_dword_masked(HWIO_UART2_IMR_ADDR,m,v,HWIO_UART_IMR_shadow)
#define HWIO_UART2_IMR_TX_DONE_BMSK                                             0x100
#define HWIO_UART2_IMR_TX_DONE_SHFT                                               0x8
#define HWIO_UART2_IMR_TX_ERROR_BMSK                                             0x80
#define HWIO_UART2_IMR_TX_ERROR_SHFT                                              0x7
#define HWIO_UART2_IMR_CURRENT_CTS_BMSK                                          0x40
#define HWIO_UART2_IMR_CURRENT_CTS_SHFT                                           0x6
#define HWIO_UART2_IMR_DELTA_CTS_BMSK                                            0x20
#define HWIO_UART2_IMR_DELTA_CTS_SHFT                                             0x5
#define HWIO_UART2_IMR_RXLEV_BMSK                                                0x10
#define HWIO_UART2_IMR_RXLEV_SHFT                                                 0x4
#define HWIO_UART2_IMR_RXSTALE_BMSK                                               0x8
#define HWIO_UART2_IMR_RXSTALE_SHFT                                               0x3
#define HWIO_UART2_IMR_RXBREAK_BMSK                                               0x4
#define HWIO_UART2_IMR_RXBREAK_SHFT                                               0x2
#define HWIO_UART2_IMR_RXHUNT_BMSK                                                0x2
#define HWIO_UART2_IMR_RXHUNT_SHFT                                                0x1
#define HWIO_UART2_IMR_TXLEV_BMSK                                                 0x1
#define HWIO_UART2_IMR_TXLEV_SHFT                                                   0

// vphan: uimdrv.c needs HWIO_UART2_IPR_OUT
#define HWIO_UART2_IPR_ADDR                                                0x28000000
#define HWIO_UART2_IPR_RMSK                                                   0x3ffff
#define HWIO_UART2_IPR_SHFT                                                         0
#define HWIO_UART2_IPR_IN                                                  \
        in_dword_masked(HWIO_UART2_IPR_ADDR, HWIO_UART2_IPR_RMSK)
#define HWIO_UART2_IPR_INM(m)                                              \
        in_dword_masked(HWIO_UART2_IPR_ADDR, m)
#define HWIO_UART2_IPR_OUT(v)                                              \
        out_dword(HWIO_UART2_IPR_ADDR,v)
#define HWIO_UART2_IPR_OUTM(m,v)                                           \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART2_IPR_ADDR,m,v,HWIO_UART2_IPR_IN); \
		HWIO_INTFREE()
#define HWIO_UART2_IPR_STALE_TIMEOUT_MSB_BMSK                                 0x3ff80
#define HWIO_UART2_IPR_STALE_TIMEOUT_MSB_SHFT                                     0x7
#define HWIO_UART2_IPR_SAMPLE_DATA_BMSK                                          0x40
#define HWIO_UART2_IPR_SAMPLE_DATA_SHFT                                           0x6
#define HWIO_UART2_IPR_RXSTALE_LAST_BMSK                                         0x20
#define HWIO_UART2_IPR_RXSTALE_LAST_SHFT                                          0x5
#define HWIO_UART2_IPR_STALE_TIMEOUT_LSB_BMSK                                    0x1f
#define HWIO_UART2_IPR_STALE_TIMEOUT_LSB_SHFT                                       0

// vphan: uimdrv.c needs HWIO_UART2_TFWR_TFW_BMSK
#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_UART2_TFWR_ADDR                                               0x28000000
#define HWIO_UART2_TFWR_PHYS                                               0x28000000
#else
#define HWIO_UART2_TFWR_ADDR                                               0x28000000
#endif
#define HWIO_UART2_TFWR_RMSK                                                  0x3ffff
#define HWIO_UART2_TFWR_SHFT                                                        0
#define HWIO_UART2_TFWR_IN                                                 \
        in_dword_masked(HWIO_UART2_TFWR_ADDR, HWIO_UART2_TFWR_RMSK)
#define HWIO_UART2_TFWR_INM(m)                                             \
        in_dword_masked(HWIO_UART2_TFWR_ADDR, m)
#define HWIO_UART2_TFWR_OUT(v)                                             \
        out_dword(HWIO_UART2_TFWR_ADDR,v)
#define HWIO_UART2_TFWR_OUTM(m,v)                                          \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART2_TFWR_ADDR,m,v,HWIO_UART2_TFWR_IN); \
		HWIO_INTFREE()
#define HWIO_UART2_TFWR_TFW_BMSK                                              0x3ffff
#define HWIO_UART2_TFWR_TFW_SHFT                                                    0

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_UART2_RFWR_ADDR                                               0x28000000
#define HWIO_UART2_RFWR_PHYS                                               0x28000000
#else
#define HWIO_UART2_RFWR_ADDR                                               0x28000000
#endif
#define HWIO_UART2_RFWR_RMSK                                                  0x3ffff
#define HWIO_UART2_RFWR_SHFT                                                        0
#define HWIO_UART2_RFWR_IN                                                 \
        in_dword_masked(HWIO_UART2_RFWR_ADDR, HWIO_UART2_RFWR_RMSK)
#define HWIO_UART2_RFWR_INM(m)                                             \
        in_dword_masked(HWIO_UART2_RFWR_ADDR, m)
#define HWIO_UART2_RFWR_OUT(v)                                             \
        out_dword(HWIO_UART2_RFWR_ADDR,v)
#define HWIO_UART2_RFWR_OUTM(m,v)                                          \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART2_RFWR_ADDR,m,v,HWIO_UART2_RFWR_IN); \
		HWIO_INTFREE()
#define HWIO_UART2_RFWR_RFW_BMSK                                              0x3ffff
#define HWIO_UART2_RFWR_RFW_SHFT                                                    0

// vphan: uimdrv.c needs HWIO_UART2_MREG_ADDR
#define HWIO_UART2_MREG_ADDR                                               0x28000000
#define HWIO_UART2_MREG_RMSK                                                     0xff
#define HWIO_UART2_MREG_SHFT                                                        0
#define HWIO_UART2_MREG_IN                                                 \
        in_dword_masked(HWIO_UART2_MREG_ADDR, HWIO_UART2_MREG_RMSK)
#define HWIO_UART2_MREG_INM(m)                                             \
        in_dword_masked(HWIO_UART2_MREG_ADDR, m)
#define HWIO_UART2_MREG_OUT(v)                                             \
        out_dword(HWIO_UART2_MREG_ADDR,v)
#define HWIO_UART2_MREG_OUTM(m,v)                                          \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART2_MREG_ADDR,m,v,HWIO_UART2_MREG_IN); \
		HWIO_INTFREE()
#define HWIO_UART2_MREG_DATA_BMSK                                                0xff
#define HWIO_UART2_MREG_DATA_SHFT                                                   0

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_UART2_NREG_ADDR                                               0x28000000
#define HWIO_UART2_NREG_PHYS                                               0x28000000
#else
#define HWIO_UART2_NREG_ADDR                                               0x28000000
#endif
#define HWIO_UART2_NREG_RMSK                                                     0xff
#define HWIO_UART2_NREG_SHFT                                                        0
#define HWIO_UART2_NREG_IN                                                 \
        in_dword_masked(HWIO_UART2_NREG_ADDR, HWIO_UART2_NREG_RMSK)
#define HWIO_UART2_NREG_INM(m)                                             \
        in_dword_masked(HWIO_UART2_NREG_ADDR, m)
#define HWIO_UART2_NREG_OUT(v)                                             \
        out_dword(HWIO_UART2_NREG_ADDR,v)
#define HWIO_UART2_NREG_OUTM(m,v)                                          \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART2_NREG_ADDR,m,v,HWIO_UART2_NREG_IN); \
		HWIO_INTFREE()
#define HWIO_UART2_NREG_DATA_BMSK                                                0xff
#define HWIO_UART2_NREG_DATA_SHFT                                                   0

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_UART2_DREG_ADDR                                               0x28000000
#define HWIO_UART2_DREG_PHYS                                               0x28000000
#else
#define HWIO_UART2_DREG_ADDR                                               0x28000000
#endif
#define HWIO_UART2_DREG_RMSK                                                     0xff
#define HWIO_UART2_DREG_SHFT                                                        0
#define HWIO_UART2_DREG_IN                                                 \
        in_dword_masked(HWIO_UART2_DREG_ADDR, HWIO_UART2_DREG_RMSK)
#define HWIO_UART2_DREG_INM(m)                                             \
        in_dword_masked(HWIO_UART2_DREG_ADDR, m)
#define HWIO_UART2_DREG_OUT(v)                                             \
        out_dword(HWIO_UART2_DREG_ADDR,v)
#define HWIO_UART2_DREG_OUTM(m,v)                                          \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART2_DREG_ADDR,m,v,HWIO_UART2_DREG_IN); \
		HWIO_INTFREE()
#define HWIO_UART2_DREG_DATA_BMSK                                                0xff
#define HWIO_UART2_DREG_DATA_SHFT                                                   0

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_UART2_MNDREG_ADDR                                             0x28000000
#define HWIO_UART2_MNDREG_PHYS                                             0x28000000
#else
#define HWIO_UART2_MNDREG_ADDR                                             0x28000000
#endif
#define HWIO_UART2_MNDREG_RMSK                                                   0x3f
#define HWIO_UART2_MNDREG_SHFT                                                      0
#define HWIO_UART2_MNDREG_IN                                               \
        in_dword_masked(HWIO_UART2_MNDREG_ADDR, HWIO_UART2_MNDREG_RMSK)
#define HWIO_UART2_MNDREG_INM(m)                                           \
        in_dword_masked(HWIO_UART2_MNDREG_ADDR, m)
#define HWIO_UART2_MNDREG_OUT(v)                                           \
        out_dword(HWIO_UART2_MNDREG_ADDR,v)
#define HWIO_UART2_MNDREG_OUTM(m,v)                                        \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART2_MNDREG_ADDR,m,v,HWIO_UART2_MNDREG_IN); \
		HWIO_INTFREE()
#define HWIO_UART2_MNDREG_MREG_LSB_BMSK                                          0x20
#define HWIO_UART2_MNDREG_MREG_LSB_SHFT                                           0x5
#define HWIO_UART2_MNDREG_NREG_LSB_BMSK                                          0x1c
#define HWIO_UART2_MNDREG_NREG_LSB_SHFT                                           0x2
#define HWIO_UART2_MNDREG_DREG_LSB_BMSK                                           0x3
#define HWIO_UART2_MNDREG_DREG_LSB_SHFT                                             0

// vphan: ap_uart.c needs UART2_IRDA
#define HWIO_UART2_IRDA_ADDR                                               0x28000000
#define HWIO_UART2_IRDA_RMSK                                                      0xf
#define HWIO_UART2_IRDA_SHFT                                                        0
#define HWIO_UART2_IRDA_OUT(v)                                             \
        out_dword(HWIO_UART2_IRDA_ADDR,v)
#define HWIO_UART2_IRDA_OUTM(m,v)                                          \
        out_dword_masked(HWIO_UART2_IRDA_ADDR,m,v,HWIO_UART_IRDA_shadow)
#define HWIO_UART2_IRDA_IRDA_LOOPBACK_BMSK                                        0x8
#define HWIO_UART2_IRDA_IRDA_LOOPBACK_SHFT                                        0x3
#define HWIO_UART2_IRDA_INVERT_IRDA_TX_BMSK                                       0x4
#define HWIO_UART2_IRDA_INVERT_IRDA_TX_SHFT                                       0x2
#define HWIO_UART2_IRDA_INVERT_IRDA_RX_BMSK                                       0x2
#define HWIO_UART2_IRDA_INVERT_IRDA_RX_SHFT                                       0x1
#define HWIO_UART2_IRDA_IRDA_EN_BMSK                                              0x1
#define HWIO_UART2_IRDA_IRDA_EN_SHFT                                                0

#define HWIO_UART2_SIM_CFG_ADDR                                            0x28000000
#define HWIO_UART2_SIM_CFG_RMSK                                               0x3ffff
#define HWIO_UART2_SIM_CFG_SHFT                                                     0
#define HWIO_UART2_SIM_CFG_OUT(v)                                          \
        out_dword(HWIO_UART2_SIM_CFG_ADDR,v)
#define HWIO_UART2_SIM_CFG_OUTM(m,v)                                       \
        out_dword_masked(HWIO_UART2_SIM_CFG_ADDR,m,v,HWIO_UART_SIM_CFG_shadow)
#define HWIO_UART2_SIM_CFG_UIM_TX_MODE_BMSK                                   0x20000
#define HWIO_UART2_SIM_CFG_UIM_TX_MODE_SHFT                                      0x11
#define HWIO_UART2_SIM_CFG_UIM_RX_MODE_BMSK                                   0x10000
#define HWIO_UART2_SIM_CFG_UIM_RX_MODE_SHFT                                      0x10
#define HWIO_UART2_SIM_CFG_SIM_STOP_BIT_LEN_BMSK                               0xff00
#define HWIO_UART2_SIM_CFG_SIM_STOP_BIT_LEN_SHFT                                  0x8
#define HWIO_UART2_SIM_CFG_SIM_STOP_BIT_LEN_1_BIT_TIMES_FVAL                      0x1
#define HWIO_UART2_SIM_CFG_SIM_STOP_BIT_LEN_2_BIT_TIMES_FVAL                      0x2
#define HWIO_UART2_SIM_CFG_SIM_CLK_ON_BMSK                                       0x80
#define HWIO_UART2_SIM_CFG_SIM_CLK_ON_SHFT                                        0x7
#define HWIO_UART2_SIM_CFG_SIM_CLK_TD8_SEL_BMSK                                  0x40
#define HWIO_UART2_SIM_CFG_SIM_CLK_TD8_SEL_SHFT                                   0x6
#define HWIO_UART2_SIM_CFG_SIM_CLK_STOP_HIGH_BMSK                                0x20
#define HWIO_UART2_SIM_CFG_SIM_CLK_STOP_HIGH_SHFT                                 0x5
#define HWIO_UART2_SIM_CFG_SIM_CLK_SEL_BMSK                                      0x10
#define HWIO_UART2_SIM_CFG_SIM_CLK_SEL_SHFT                                       0x4
#define HWIO_UART2_SIM_CFG_MASK_RX_BMSK                                           0x8
#define HWIO_UART2_SIM_CFG_MASK_RX_SHFT                                           0x3
#define HWIO_UART2_SIM_CFG_SWAP_D_BMSK                                            0x4
#define HWIO_UART2_SIM_CFG_SWAP_D_SHFT                                            0x2
#define HWIO_UART2_SIM_CFG_INV_D_BMSK                                             0x2
#define HWIO_UART2_SIM_CFG_INV_D_SHFT                                             0x1
#define HWIO_UART2_SIM_CFG_SIM_SEL_BMSK                                           0x1
#define HWIO_UART2_SIM_CFG_SIM_SEL_SHFT                                             0

// vphan - ap_uart.c needed UART2_SR
// Sub-Section 15.2.2.2: Read registers
#define HWIO_UART2_SR_ADDR                                                 0x28000000
#define HWIO_UART2_SR_RMSK                                                       0xff
#define HWIO_UART2_SR_SHFT                                                          0
#define HWIO_UART2_SR_IN                                                   \
        in_dword_masked(HWIO_UART2_SR_ADDR, HWIO_UART2_SR_RMSK)
#define HWIO_UART2_SR_INM(m)                                               \
        in_dword_masked(HWIO_UART2_SR_ADDR, m)
#define HWIO_UART2_SR_HUNT_CHAR_BMSK                                             0x80
#define HWIO_UART2_SR_HUNT_CHAR_SHFT                                              0x7
#define HWIO_UART2_SR_RX_BREAK_BMSK                                              0x40
#define HWIO_UART2_SR_RX_BREAK_SHFT                                               0x6
#define HWIO_UART2_SR_PAR_FRAME_ERR_BMSK                                         0x20
#define HWIO_UART2_SR_PAR_FRAME_ERR_SHFT                                          0x5
#define HWIO_UART2_SR_UART_OVERRUN_BMSK                                          0x10
#define HWIO_UART2_SR_UART_OVERRUN_SHFT                                           0x4
#define HWIO_UART2_SR_TXEMT_BMSK                                                  0x8
#define HWIO_UART2_SR_TXEMT_SHFT                                                  0x3
#define HWIO_UART2_SR_TXRDY_BMSK                                                  0x4
#define HWIO_UART2_SR_TXRDY_SHFT                                                  0x2
#define HWIO_UART2_SR_RXFULL_BMSK                                                 0x2
#define HWIO_UART2_SR_RXFULL_SHFT                                                 0x1
#define HWIO_UART2_SR_RXRDY_BMSK                                                  0x1
#define HWIO_UART2_SR_RXRDY_SHFT                                                    0

#define HWIO_UART2_RF_ADDR                                                 0x28000000
#define HWIO_UART2_RF_RMSK                                                       0xff
#define HWIO_UART2_RF_SHFT                                                          0
#define HWIO_UART2_RF_IN                                                   \
        in_dword_masked(HWIO_UART2_RF_ADDR, HWIO_UART2_RF_RMSK)
#define HWIO_UART2_RF_INM(m)                                               \
        in_dword_masked(HWIO_UART2_RF_ADDR, m)
#define HWIO_UART2_RF_DATA_BMSK                                                  0xff
#define HWIO_UART2_RF_DATA_SHFT                                                     0

#define HWIO_UART2_MISR_ADDR                                               0x28000000
#define HWIO_UART2_MISR_RMSK                                                    0x1ff
#define HWIO_UART2_MISR_SHFT                                                        0
#define HWIO_UART2_MISR_IN                                                 \
        in_dword_masked(HWIO_UART2_MISR_ADDR, HWIO_UART2_MISR_RMSK)
#define HWIO_UART2_MISR_INM(m)                                             \
        in_dword_masked(HWIO_UART2_MISR_ADDR, m)
#define HWIO_UART2_MISR_UART_MISR_BMSK                                          0x1ff
#define HWIO_UART2_MISR_UART_MISR_SHFT                                              0

#define HWIO_UART2_ISR_ADDR                                                0x28000000
#define HWIO_UART2_ISR_RMSK                                                     0x1ff
#define HWIO_UART2_ISR_SHFT                                                         0
#define HWIO_UART2_ISR_IN                                                  \
        in_dword_masked(HWIO_UART2_ISR_ADDR, HWIO_UART2_ISR_RMSK)
#define HWIO_UART2_ISR_INM(m)                                              \
        in_dword_masked(HWIO_UART2_ISR_ADDR, m)
#define HWIO_UART2_ISR_TX_DONE_BMSK                                             0x100
#define HWIO_UART2_ISR_TX_DONE_SHFT                                               0x8
#define HWIO_UART2_ISR_TX_ERROR_BMSK                                             0x80
#define HWIO_UART2_ISR_TX_ERROR_SHFT                                              0x7
#define HWIO_UART2_ISR_CURRENT_CTS_BMSK                                          0x40
#define HWIO_UART2_ISR_CURRENT_CTS_SHFT                                           0x6
#define HWIO_UART2_ISR_DELTA_CTS_BMSK                                            0x20
#define HWIO_UART2_ISR_DELTA_CTS_SHFT                                             0x5
#define HWIO_UART2_ISR_RXLEV_BMSK                                                0x10
#define HWIO_UART2_ISR_RXLEV_SHFT                                                 0x4
#define HWIO_UART2_ISR_RXSTALE_BMSK                                               0x8
#define HWIO_UART2_ISR_RXSTALE_SHFT                                               0x3
#define HWIO_UART2_ISR_RXBREAK_BMSK                                               0x4
#define HWIO_UART2_ISR_RXBREAK_SHFT                                               0x2
#define HWIO_UART2_ISR_RXHUNT_BMSK                                                0x2
#define HWIO_UART2_ISR_RXHUNT_SHFT                                                0x1
#define HWIO_UART2_ISR_TXLEV_BMSK                                                 0x1
#define HWIO_UART2_ISR_TXLEV_SHFT                                                   0

// vphan: dloaduart.c need UART1DM
#define HWIO_UART1DM_MR2_RMSK                                                   0xff
#define HWIO_UART1DM_MR2_SHFT                                                      0
#define HWIO_UART1DM_MR2_IN                                               \
        in_dword_masked(HWIO_UART_DM_MR2_ADDR, HWIO_UART_DM_MR2_RMSK)
#define HWIO_UART1DM_MR2_INM(m)                                           \
        in_dword_masked(HWIO_UART_DM_MR2_ADDR, m)
#define HWIO_UART1DM_MR2_OUT(v)                                           \
        out_dword(HWIO_UART_DM_MR2_ADDR,v)
#define HWIO_UART1DM_MR2_OUTM(m,v)                                        \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_DM_MR2_ADDR,m,v,HWIO_UART_DM_MR2_IN); \
		HWIO_INTFREE()
#define HWIO_UART1DM_MR2_LOOPBACK_BMSK                                          0x80
#define HWIO_UART1DM_MR2_LOOPBACK_SHFT                                           0x7
#define HWIO_UART1DM_MR2_ERROR_MODE_BMSK                                        0x40
#define HWIO_UART1DM_MR2_ERROR_MODE_SHFT                                         0x6
#define HWIO_UART1DM_MR2_BITS_PER_CHAR_BMSK                                     0x30
#define HWIO_UART1DM_MR2_BITS_PER_CHAR_SHFT                                      0x4
#define HWIO_UART1DM_MR2_BITS_PER_CHAR_5_BITS_FVAL                                 0
#define HWIO_UART1DM_MR2_BITS_PER_CHAR_6_BITS_FVAL                               0x1
#define HWIO_UART1DM_MR2_BITS_PER_CHAR_7_BITS_FVAL                               0x2
#define HWIO_UART1DM_MR2_BITS_PER_CHAR_8_BITS_FVAL                               0x3
#define HWIO_UART1DM_MR2_STOP_BIT_LEN_BMSK                                       0xc
#define HWIO_UART1DM_MR2_STOP_BIT_LEN_SHFT                                       0x2
#define HWIO_UART1DM_MR2_STOP_BIT_LEN_0_563_FVAL                                   0
#define HWIO_UART1DM_MR2_STOP_BIT_LEN_1_000_BIT_TIME_FVAL                        0x1
#define HWIO_UART1DM_MR2_STOP_BIT_LEN_1_563_FVAL                                 0x2
#define HWIO_UART1DM_MR2_STOP_BIT_LEN_2_000_BIT_TIMES_FVAL                       0x3
#define HWIO_UART1DM_MR2_PARITY_MODE_BMSK                                        0x3
#define HWIO_UART1DM_MR2_PARITY_MODE_SHFT                                          0
#define HWIO_UART1DM_MR2_PARITY_MODE_NO_PARITY_FVAL                                0
#define HWIO_UART1DM_MR2_PARITY_MODE_ODD_PARITY_FVAL                             0x1
#define HWIO_UART1DM_MR2_PARITY_MODE_EVEN_PARITY_FVAL                            0x2
#define HWIO_UART1DM_MR2_PARITY_MODE_SPACE_PARITY_FVAL                           0x3

#define HWIO_UART1DM_CR_RMSK                                                   0xfff
#define HWIO_UART1DM_CR_SHFT                                                       0
#define HWIO_UART1DM_CR_OUT(v)                                            \
        out_dword(HWIO_UART_DM_CR_ADDR,v)
#define HWIO_UART1DM_CR_OUTM(m,v)                                         \
        out_dword_masked(HWIO_UART_DM_CR_ADDR,m,v,HWIO_UART_DM_CR_shadow)
#define HWIO_UART1DM_CR_CHANNEL_COMMAND_MSB_BMSK                               0x800
#define HWIO_UART1DM_CR_CHANNEL_COMMAND_MSB_SHFT                                 0xb
#define HWIO_UART1DM_CR_GENERAL_COMMAND_BMSK                                   0x700
#define HWIO_UART1DM_CR_GENERAL_COMMAND_SHFT                                     0x8
#define HWIO_UART1DM_CR_CHANNEL_COMMAND_LSB_BMSK                                0xf0
#define HWIO_UART1DM_CR_CHANNEL_COMMAND_LSB_SHFT                                 0x4
#define HWIO_UART1DM_CR_CHANNEL_COMMAND_BMSK    HWIO_UART1DM_CR_CHANNEL_COMMAND_LSB_BMSK
#define HWIO_UART1DM_CR_CHANNEL_COMMAND_SHFT    HWIO_UART1DM_CR_CHANNEL_COMMAND_LSB_SHFT
#define HWIO_UART1DM_CR_UART_TX_DISABLE_BMSK                                     0x8
#define HWIO_UART1DM_CR_UART_TX_DISABLE_SHFT                                     0x3
#define HWIO_UART1DM_CR_UART_TX_EN_BMSK                                          0x4
#define HWIO_UART1DM_CR_UART_TX_EN_SHFT                                          0x2
#define HWIO_UART1DM_CR_UART_RX_DISABLE_BMSK                                     0x2
#define HWIO_UART1DM_CR_UART_RX_DISABLE_SHFT                                     0x1
#define HWIO_UART1DM_CR_UART_RX_EN_BMSK                                          0x1
#define HWIO_UART1DM_CR_UART_RX_EN_SHFT                                            0

// Sub-Section 15.2.2.2: Read registers
#define HWIO_UART1DM_SR_RMSK                                                    0xff
#define HWIO_UART1DM_SR_SHFT                                                       0
#define HWIO_UART1DM_SR_IN                                                \
        in_dword_masked(HWIO_UART_DM_SR_ADDR, HWIO_UART_DM_SR_RMSK)
#define HWIO_UART1DM_SR_INM(m)                                            \
        in_dword_masked(HWIO_UART_DM_SR_ADDR, m)
#define HWIO_UART1DM_SR_HUNT_CHAR_BMSK                                          0x80
#define HWIO_UART1DM_SR_HUNT_CHAR_SHFT                                           0x7
#define HWIO_UART1DM_SR_RX_BREAK_BMSK                                           0x40
#define HWIO_UART1DM_SR_RX_BREAK_SHFT                                            0x6
#define HWIO_UART1DM_SR_PAR_FRAME_ERR_BMSK                                      0x20
#define HWIO_UART1DM_SR_PAR_FRAME_ERR_SHFT                                       0x5
#define HWIO_UART1DM_SR_UART_OVERRUN_BMSK                                       0x10
#define HWIO_UART1DM_SR_UART_OVERRUN_SHFT                                        0x4
#define HWIO_UART1DM_SR_TXEMT_BMSK                                               0x8
#define HWIO_UART1DM_SR_TXEMT_SHFT                                               0x3
#define HWIO_UART1DM_SR_TXRDY_BMSK                                               0x4
#define HWIO_UART1DM_SR_TXRDY_SHFT                                               0x2
#define HWIO_UART1DM_SR_RXFULL_BMSK                                              0x2
#define HWIO_UART1DM_SR_RXFULL_SHFT                                              0x1
#define HWIO_UART1DM_SR_RXRDY_BMSK                                               0x1
#define HWIO_UART1DM_SR_RXRDY_SHFT                                                 0

#endif /* FEATURE_SC2X_SW_PORT */

#define HWIO_UART_SR_RXRDY_BMSK                                                  0x1

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_UART_INT_STATUS_ADDR                                         0x28000000
#define HWIO_UART_INT_STATUS_PHYS                                         0x28000000
#else
#define HWIO_UART_INT_STATUS_ADDR                                         0x28000000
#endif
#define HWIO_UART_INT_STATUS_RMSK                                                0xf
#define HWIO_UART_INT_STATUS_SHFT                                                  0
#define HWIO_UART_INT_STATUS_IN                                           \
        in_dword_masked(HWIO_UART_INT_STATUS_ADDR, HWIO_UART_INT_STATUS_RMSK)
#define HWIO_UART_INT_STATUS_INM(m)                                       \
        in_dword_masked(HWIO_UART_INT_STATUS_ADDR, m)
#define HWIO_UART_INT_STATUS_UART2_DP_RX_DATA_INT_BMSK                           0x8
#define HWIO_UART_INT_STATUS_UART2_DP_RX_DATA_INT_SHFT                           0x3

#define HWIO_UART_INT_STATUS_UART1_DP_RX_DATA_INT_BMSK HWIO_UART_INT_STATUS_UART2_DP_RX_DATA_INT_BMSK 
#define HWIO_UART_INT_CLR_REG_UART1_DP_RX_DATA_INT_BMSK HWIO_UART_INT_CLR_REG_UART2_DP_RX_DATA_INT_BMSK 
#define HWIO_UART_INT_POL_REG_UART1_DP_RX_DATA_INT_BMSK HWIO_UART_INT_POL_REG_UART2_DP_RX_DATA_INT_BMSK 
#define HWIO_UART_INT_ENABLE_UART1_INT_BMSK HWIO_UART_INT_ENABLE_UART2_INT_BMSK 
#define HWIO_UART_INT_ENABLE_UART1_DP_RX_DATA_INT_BMSK HWIO_UART_INT_ENABLE_UART2_DP_RX_DATA_INT_BMSK 
#define HWIO_UART_INT_CLR_REG_UART1_INT_BMSK HWIO_UART_INT_CLR_REG_UART2_INT_BMSK 
#define HWIO_UART_INT_STATUS_UART1_INT_BMSK HWIO_UART_INT_STATUS_UART2_INT_BMSK 
#define HWIO_UART_INT_DET_REG_UART1_INT_BMSK HWIO_UART_INT_DET_REG_UART2_INT_BMSK 
#define HWIO_UART_INT_DET_REG_UART1_DP_RX_DATA_INT_BMSK HWIO_UART_INT_DET_REG_UART2_DP_RX_DATA_INT_BMSK 

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_UART_ISR_ADDR                                                0x28000000
#define HWIO_UART_ISR_PHYS                                                0x28000000
#else
#define HWIO_UART_ISR_ADDR                                                0x28000000
#endif
#define HWIO_UART_ISR_RMSK                                                     0x1ff
#define HWIO_UART_ISR_SHFT                                                         0
#define HWIO_UART_ISR_IN                                                  \
        in_dword_masked(HWIO_UART_ISR_ADDR, HWIO_UART_ISR_RMSK)
#define HWIO_UART_ISR_INM(m)                                              \
        in_dword_masked(HWIO_UART_ISR_ADDR, m)
#define HWIO_UART_ISR_TX_DONE_BMSK                                             0x100
#define HWIO_UART_ISR_TX_DONE_SHFT                                               0x8
#define HWIO_UART_ISR_TX_ERROR_BMSK                                             0x80
#define HWIO_UART_ISR_TX_ERROR_SHFT                                              0x7
#define HWIO_UART_ISR_CURRENT_CTS_BMSK                                          0x40
#define HWIO_UART_ISR_CURRENT_CTS_SHFT                                           0x6
#define HWIO_UART_ISR_DELTA_CTS_BMSK                                            0x20
#define HWIO_UART_ISR_DELTA_CTS_SHFT                                             0x5
#define HWIO_UART_ISR_RXLEV_BMSK                                                0x10
#define HWIO_UART_ISR_RXLEV_SHFT                                                 0x4
#define HWIO_UART_ISR_RXSTALE_BMSK                                               0x8
#define HWIO_UART_ISR_RXSTALE_SHFT                                               0x3
#define HWIO_UART_ISR_RXBREAK_BMSK                                               0x4
#define HWIO_UART_ISR_RXBREAK_SHFT                                               0x2
#define HWIO_UART_ISR_RXHUNT_BMSK                                                0x2
#define HWIO_UART_ISR_RXHUNT_SHFT                                                0x1
#define HWIO_UART_ISR_TXLEV_BMSK                                                 0x1
#define HWIO_UART_ISR_TXLEV_SHFT                                                   0

/*
------------------------------------------------------------------------
OBJECT SXNEAL/ring.o
"..\..\services\avs\ring.c", line 272: Warning: C2207W: inventing 'extern int HWIO_RINGER_MN_A_MDIV_OUT();'
"..\..\services\avs\ring.c", line 272: Warning: C2207W: inventing 'extern int HWIO_RINGER_MN_B_MDIV_OUT();'
"..\..\services\avs\ring.c", line 384: Warning: C2207W: inventing 'extern int HWIO_RINGER_MN_A_DUTY_OUT();'
"..\..\services\avs\ring.c", line 385: Warning: C2207W: inventing 'extern int HWIO_RINGER_MN_A_NDIV_OUT();'
"..\..\services\avs\ring.c", line 388: Warning: C2207W: inventing 'extern int HWIO_RINGER_MN_B_DUTY_OUT();'
"..\..\services\avs\ring.c", line 389: Warning: C2207W: inventing 'extern int HWIO_RINGER_MN_B_NDIV_OUT();'
"..\..\services\avs\ring.c", line 392: Error: C2456E: undeclared name, inventing 'extern int HWIO_RINGER_MN_A_MDIV_RINGER_MN_A_EN_BMSK'
"..\..\services\avs\ring.c", line 393: Error: C2456E: undeclared name, inventing 'extern int HWIO_RINGER_MN_B_MDIV_RINGER_MN_B_EN_BMSK'
*/

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_RINGER_MN_A_MDIV_ADDR                                        0x28000000
#define HWIO_RINGER_MN_A_MDIV_PHYS                                        0x28000000
#else
#define HWIO_RINGER_MN_A_MDIV_ADDR                                        0x28000000
#endif
#define HWIO_RINGER_MN_A_MDIV_RMSK                                              0x7f
#define HWIO_RINGER_MN_A_MDIV_SHFT                                                 0
#define HWIO_RINGER_MN_A_MDIV_OUT(v)                                      \
        out_dword(HWIO_RINGER_MN_A_MDIV_ADDR,v)
#define HWIO_RINGER_MN_A_MDIV_OUTM(m,v)                                   \
        out_dword_masked(HWIO_RINGER_MN_A_MDIV_ADDR,m,v,HWIO_RINGER_MN_A_MDIV_shadow)
#define HWIO_RINGER_MN_A_MDIV_RINGER_MN_A_EN_BMSK                               0x40
#define HWIO_RINGER_MN_A_MDIV_RINGER_MN_A_EN_SHFT                                0x6
#define HWIO_RINGER_MN_A_MDIV_RINGER_MN_A_MDIV_BMSK                             0x3f
#define HWIO_RINGER_MN_A_MDIV_RINGER_MN_A_MDIV_SHFT                                0

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_RINGER_MN_B_DUTY_ADDR                                        0x28000000
#define HWIO_RINGER_MN_B_DUTY_PHYS                                        0x28000000
#else
#define HWIO_RINGER_MN_B_DUTY_ADDR                                        0x28000000
#endif
#define HWIO_RINGER_MN_B_DUTY_RMSK                                             0xfff
#define HWIO_RINGER_MN_B_DUTY_SHFT                                                 0
#define HWIO_RINGER_MN_B_DUTY_OUT(v)                                      \
        out_dword(HWIO_RINGER_MN_B_DUTY_ADDR,v)
#define HWIO_RINGER_MN_B_DUTY_OUTM(m,v)                                   \
        out_dword_masked(HWIO_RINGER_MN_B_DUTY_ADDR,m,v,HWIO_RINGER_MN_B_DUTY_shadow)
#define HWIO_RINGER_MN_B_DUTY_D_VALUE_BMSK                                     0xfff
#define HWIO_RINGER_MN_B_DUTY_D_VALUE_SHFT                                         0
#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_RINGER_MN_B_NDIV_ADDR                                        0x28000000
#define HWIO_RINGER_MN_B_NDIV_PHYS                                        0x28000000
#else
#define HWIO_RINGER_MN_B_NDIV_ADDR                                        0x28000000
#endif
#define HWIO_RINGER_MN_B_NDIV_RMSK                                            0x1fff
#define HWIO_RINGER_MN_B_NDIV_SHFT                                                 0
#define HWIO_RINGER_MN_B_NDIV_OUT(v)                                      \
        out_dword(HWIO_RINGER_MN_B_NDIV_ADDR,v)
#define HWIO_RINGER_MN_B_NDIV_OUTM(m,v)                                   \
        out_dword_masked(HWIO_RINGER_MN_B_NDIV_ADDR,m,v,HWIO_RINGER_MN_B_NDIV_shadow)
#define HWIO_RINGER_MN_B_NDIV_N_VALUE_BMSK                                    0x1fff
#define HWIO_RINGER_MN_B_NDIV_N_VALUE_SHFT                                         0

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_RINGER_MN_B_MDIV_ADDR                                        0x28000000
#define HWIO_RINGER_MN_B_MDIV_PHYS                                        0x28000000
#else
#define HWIO_RINGER_MN_B_MDIV_ADDR                                        0x28000000
#endif
#define HWIO_RINGER_MN_B_MDIV_RMSK                                              0x7f
#define HWIO_RINGER_MN_B_MDIV_SHFT                                                 0
#define HWIO_RINGER_MN_B_MDIV_OUT(v)                                      \
        out_dword(HWIO_RINGER_MN_B_MDIV_ADDR,v)
#define HWIO_RINGER_MN_B_MDIV_OUTM(m,v)                                   \
        out_dword_masked(HWIO_RINGER_MN_B_MDIV_ADDR,m,v,HWIO_RINGER_MN_B_MDIV_shadow)
#define HWIO_RINGER_MN_B_MDIV_RINGER_MN_B_EN_BMSK                               0x40
#define HWIO_RINGER_MN_B_MDIV_RINGER_MN_B_EN_SHFT                                0x6
#define HWIO_RINGER_MN_B_MDIV_RINGER_MN_B_MDIV_BMSK                             0x3f
#define HWIO_RINGER_MN_B_MDIV_RINGER_MN_B_MDIV_SHFT                                0


/*
Error: L6218E: Undefined symbol HWIO_MPM_PMIC_SBI_SEL_OUTM (referred from pm.o).

#ifdef T_RUMI_SC2X
     Configure the RUMI to talk to the PMIC using 3 wire SBI0 
     HWIO_OUTM( EMU_TX_GPIO_WR, 0xE8000000,0xE0000000 ); //RF TXCO, TX_SBI, Saturn BBRX, 3 wire SBI
#else
     Target needs MPM configuration set to 0 before SSBI1 will communicate to EPIC/
     HWIO_OUTM( MPM_PMIC_SBI_SEL, 0xF, 0);
#endif T_RUMI_SC2X

HWIO_MPM_PMIC_SBI_SEL_OUTM (referred from boothw_target.o)
  Target needs MPM configuration set to 0 before SSBI1 will communicate to EPIC 
  HWIO_OUTM( MPM_PMIC_SBI_SEL, 0xF, 0);

*/

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_MPM_PMIC_SBI_SEL_ADDR                                        0x28000000
#define HWIO_MPM_PMIC_SBI_SEL_PHYS                                        0x28000000
#else
#define HWIO_MPM_PMIC_SBI_SEL_ADDR                                        0x28000000
#endif
#define HWIO_MPM_PMIC_SBI_SEL_RMSK                                               0xf
#define HWIO_MPM_PMIC_SBI_SEL_SHFT                                                 0
#define HWIO_MPM_PMIC_SBI_SEL_IN                                          \
        in_dword_masked(HWIO_MPM_PMIC_SBI_SEL_ADDR, HWIO_MPM_PMIC_SBI_SEL_RMSK)
#define HWIO_MPM_PMIC_SBI_SEL_INM(m)                                      \
        in_dword_masked(HWIO_MPM_PMIC_SBI_SEL_ADDR, m)
#define HWIO_MPM_PMIC_SBI_SEL_OUT(v)                                      \
        out_dword(HWIO_MPM_PMIC_SBI_SEL_ADDR,v)
#define HWIO_MPM_PMIC_SBI_SEL_OUTM(m,v)                                   \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_MPM_PMIC_SBI_SEL_ADDR,m,v,HWIO_MPM_PMIC_SBI_SEL_IN); \
		HWIO_INTFREE()

/* EFUSE registers section please DO NOT USE THEM, they are added
   for compiling the first QSC1100 build 
 */
 
/*
OBJECT SXNEAL/efuse.o
"..\..\drivers\efuse\efuse.c", line 121: Error: C2456E: undeclared name, inventing 'extern int HWIO_EF_CONFIG_MSB_JTAG_EN_N_BMSK'
"..\..\drivers\efuse\efuse.c", line 122: Error: C2456E: undeclared name, inventing 'extern int HWIO_EF_CONFIG_MSB_JTAG_EN_N_SHFT'
"..\..\drivers\efuse\efuse.c", line 177: Error: C2456E: undeclared name, inventing 'extern int HWIO_EF_CONFIG_LSB_MEM_REDUN_AND_CHIP_ID_DSBL_BMSK'
"..\..\drivers\efuse\efuse.c", line 178: Error: C2456E: undeclared name, inventing 'extern int HWIO_EF_CONFIG_LSB_MEM_REDUN_AND_CHIP_ID_DSBL_SHFT'
"..\..\drivers\efuse\efuse.c", line 121: Serious error: C2432E: non-constant initialiser
"..\..\drivers\efuse\efuse.c", line 122: Serious error: C2432E: non-constant initialiser
"..\..\drivers\efuse\efuse.c", line 177: Serious error: C2432E: non-constant initialiser
"..\..\drivers\efuse\efuse.c", line 178: Serious error: C2432E: non-constant initialiser
..\..\drivers\efuse\efuse.c: 0 warnings, 4 errors, 4 serious errors
make: *** [SXNEAL/efuse.o] Error 1
Finished build: 826747   Duration: 7:33 (m:s)   Cluster availability: 100%
*/
#define HWIO_EF_CONFIG_MSB_OEM_HW_ID_AUTH_ENABLE_BMSK                     0x40000000
#define HWIO_EF_CONFIG_MSB_OEM_HW_ID_AUTH_ENABLE_SHFT                           0x1e
#define HWIO_EF_CONFIG_MSB_JTAG_EN_N_BMSK                                      0x800
#define HWIO_EF_CONFIG_MSB_JTAG_EN_N_SHFT                                        0xb
#define HWIO_EF_CONFIG_LSB_MEM_REDUN_AND_CHIP_ID_DSBL_BMSK                       0x2
#define HWIO_EF_CONFIG_LSB_MEM_REDUN_AND_CHIP_ID_DSBL_SHFT                       0x1

/* Registers being used by Boot Codes, PLEASE REVIEW BEFORE USING THEM */

/* boot_error_handler uses these registers, hvo */
#define HWIO_BOOT_MODE_STATUS_ADDR HWIO_BOOT_FROM_ROM_STATUS_ADDR
/* "..\..\drivers\boot\bootmem.c", line 166: Error: C2456E: undeclared name, inventing 'extern int HWIO_BOOT_MODE_STATUS_IN'
*/
#define HWIO_BOOT_MODE_STATUS_IN HWIO_BOOT_FROM_ROM_STATUS_IN

/* They are used by boot_dload.c, hvo */
#define HWIO_INTERNAL_RAM_SEC_BOOT_ADDR                             IMEM_BASE
#define HWIO_INTERNAL_RAM_NONSEC_BOOT_ADDR                          IMEM_BASE

//----------------------------------------------------------------------------
// HDLC (0x98000000 - 0x9FFFFFFC)
//----------------------------------------------------------------------------
#ifdef FEATURE_MEMORY_VIRTUAL
#define HDLC_BASE                                                        0xfc600000
#define HDLC_BASE_PHYS                                                   0x98000000
#else
#define HDLC_BASE                                                        0x98000000
#endif

//----------------------------------------------------------------------------
// CRYPTO (0xA0000000 - 0xA7FFFFFC)
//----------------------------------------------------------------------------
#ifdef FEATURE_MEMORY_VIRTUAL
#define CRYPTO_BASE                                                      0xfc700000
#define CRYPTO_BASE_PHYS                                                 0xa0000000
#else
#define CRYPTO_BASE                                                      0xa0000000
#endif

/*
OBJECT SXNAAL/SBNAAL/bootmem.o
"..\..\drivers\boot\bootmem.c", line 167: Error: C2456E: undeclared name, inventing 'extern int HWIO_BOOT_MODE_STATUS_BOOT_MODE_BMSK'
*/
#define HWIO_BOOT_MODE_STATUS_BOOT_MODE_BMSK  0x0

// ..\..\drivers\boot\boothw.c, hvo
        /* 6800b 5.0 GPn_CFG port to EBI*_CSn */
#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_GPn_CFG0_ADDR(n) HWIO_EBI1_CSn_CFG0_ADDR(n)
#define HWIO_GPn_CFG0_ADDR_PHYS(n) HWIO_EBI1_CSn_CFG0_ADDR_PHYS(n)
#else
#define HWIO_GPn_CFG0_ADDR(n) HWIO_EBI1_CSn_CFG0_ADDR(n)
#endif

#define HWIO_GPn_CFG0_CS_SETUP_SHFT  0x0

#define HWIO_GPn_CFG0_RMSK HWIO_EBI1_CSn_CFG0_RMSK
#define HWIO_GPn_CFG0_SHFT HWIO_EBI1_CSn_CFG0_SHFT
#define HWIO_GPn_CFG0_INI(n) HWIO_EBI1_CSn_CFG0_INI(n)
#define HWIO_GPn_CFG0_INMI(n,mask) HWIO_EBI1_CSn_CFG0_INI(n,mask)
#define HWIO_GPn_CFG0_OUTI(n,v) HWIO_EBI1_CSn_CFG0_OUTI(n,v)
#define HWIO_GPn_CFG0_OUTMI(n,mask,v) HWIO_EBI1_CSn_CFG0_OUTI(n,mask,v)
#define HWIO_GPn_CFG0_RECOVERY_BMSK HWIO_EBI1_CSn_CFG0_RECOVERY_BMSK
#define HWIO_GPn_CFG0_RECOVERY_SHFT HWIO_EBI1_CSn_CFG0_RECOVERY_SHFT
#define HWIO_GPn_CFG0_HOLD_WR_BMSK HWIO_EBI1_CSn_CFG0_HOLD_WR_BMSK
#define HWIO_GPn_CFG0_HOLD_WR_SHFT HWIO_EBI1_CSn_CFG0_HOLD_WR_SHFT
#define HWIO_GPn_CFG0_HOLD_RD_BMSK HWIO_EBI1_CSn_CFG1_HOLD_RD_BMSK
#define HWIO_GPn_CFG0_HOLD_RD_SHFT HWIO_EBI1_CSn_CFG1_HOLD_RD_SHFT
#define HWIO_GPn_CFG0_WAIT_WR_BMSK HWIO_EBI1_CSn_CFG0_WAIT_WR_BMSK
#define HWIO_GPn_CFG0_WAIT_WR_SHFT HWIO_EBI1_CSn_CFG0_WAIT_WR_SHFT
#define HWIO_GPn_CFG0_WAIT_RD_BMSK HWIO_EBI1_CSn_CFG0_WAIT_RD_BMSK
#define HWIO_GPn_CFG0_WAIT_RD_SHFT HWIO_EBI1_CSn_CFG0_WAIT_RD_SHFT

/* 
------------------------------------------------------------------------
OBJECT SXNAAL/siors232.o
"..\..\drivers\sio\siors232.c", line 1039: Error: C2456E: undeclared name, inventing 'extern int HWIO_UART_SR_ADDR'
..\..\drivers\sio\siors232.c: 0 warnings, 1 error, 0 serious errors
make: *** [SXNAAL/siors232.o] Error 1
*/
#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_UART_SR_ADDR                                                 0x28000000
#define HWIO_UART_SR_PHYS                                                 0x28000000
#else
#define HWIO_UART_SR_ADDR                                                 0x28000000
#endif
#define HWIO_UART_SR_RMSK                                                       0xff
#define HWIO_UART_SR_SHFT                                                          0
#define HWIO_UART_SR_IN                                                   \
        in_dword_masked(HWIO_UART_SR_ADDR, HWIO_UART_SR_RMSK)
#define HWIO_UART_SR_INM(m)                                               \
        in_dword_masked(HWIO_UART_SR_ADDR, m)
#define HWIO_UART_SR_HUNT_CHAR_BMSK                                             0x80
#define HWIO_UART_SR_HUNT_CHAR_SHFT                                              0x7
#define HWIO_UART_SR_RX_BREAK_BMSK                                              0x40
#define HWIO_UART_SR_RX_BREAK_SHFT                                               0x6
#define HWIO_UART_SR_PAR_FRAME_ERR_BMSK                                         0x20
#define HWIO_UART_SR_PAR_FRAME_ERR_SHFT                                          0x5
#define HWIO_UART_SR_UART_OVERRUN_BMSK                                          0x10
#define HWIO_UART_SR_UART_OVERRUN_SHFT                                           0x4
#define HWIO_UART_SR_TXEMT_BMSK                                                  0x8
#define HWIO_UART_SR_TXEMT_SHFT                                                  0x3
#define HWIO_UART_SR_TXRDY_BMSK                                                  0x4
#define HWIO_UART_SR_TXRDY_SHFT                                                  0x2
#define HWIO_UART_SR_RXFULL_BMSK                                                 0x2
#define HWIO_UART_SR_RXFULL_SHFT                                                 0x1
#define HWIO_UART_SR_RXRDY_BMSK                                                  0x1
#define HWIO_UART_SR_RXRDY_SHFT   

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TF_ADDR                                                 0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TF_PHYS                                                 0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TF_ADDR                                                 0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TF_RMSK                                                       0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TF_SHFT                                                          0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TF_OUT(v)                                               \
        out_dword(HWIO_UART_TF_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TF_OUTM(m,v)                                            \
        out_dword_masked(HWIO_UART_TF_ADDR,m,v,HWIO_UART_TF_shadow)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TF_DATA_BMSK                                                  0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TF_DATA_SHFT                                                     0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_ADDR                                                 0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_PHYS                                                 0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_ADDR                                                 0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_RMSK                                                      0x1ff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_SHFT                                                          0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_OUT(v)                                               \
        out_dword(HWIO_UART_CR_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_OUTM(m,v)                                            \
        out_dword_masked(HWIO_UART_CR_ADDR,m,v,HWIO_UART_CR_shadow)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_CHANNEL_COMMAND_BMSK                                      0x1f0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_CHANNEL_COMMAND_SHFT                                        0x4
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_UART_TX_DISABLE_BMSK                                        0x8
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_UART_TX_DISABLE_SHFT                                        0x3
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_UART_TX_EN_BMSK                                             0x4
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_UART_TX_EN_SHFT                                             0x2
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_UART_RX_DISABLE_BMSK                                        0x2
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_UART_RX_DISABLE_SHFT                                        0x1
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_UART_RX_EN_BMSK                                             0x1
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CR_UART_RX_EN_SHFT                                               0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_ADDR                                                0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_PHYS                                                0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_ADDR                                                0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_RMSK                                                   0x3ffff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_SHFT                                                         0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_IN                                                  \
        in_dword_masked(HWIO_UART_MR1_ADDR, HWIO_UART_MR1_RMSK)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_INM(m)                                              \
        in_dword_masked(HWIO_UART_MR1_ADDR, m)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_OUT(v)                                              \
        out_dword(HWIO_UART_MR1_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_OUTM(m,v)                                           \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_MR1_ADDR,m,v,HWIO_UART_MR1_IN); \
		HWIO_INTFREE()
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_AUTO_RFR_LEVEL1_BMSK                                   0x3ff00
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_AUTO_RFR_LEVEL1_SHFT                                       0x8
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_RX_RDY_CTL_BMSK                                           0x80
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_RX_RDY_CTL_SHFT                                            0x7
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_CTS_CTL_BMSK                                              0x40
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_CTS_CTL_SHFT                                               0x6
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_AUTO_RFR_LEVEL0_BMSK                                      0x3f
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR1_AUTO_RFR_LEVEL0_SHFT                                         0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_ADDR                                                0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_PHYS                                                0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_ADDR                                                0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_RMSK                                                      0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_SHFT                                                         0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_IN                                                  \
        in_dword_masked(HWIO_UART_MR2_ADDR, HWIO_UART_MR2_RMSK)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_INM(m)                                              \
        in_dword_masked(HWIO_UART_MR2_ADDR, m)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_OUT(v)                                              \
        out_dword(HWIO_UART_MR2_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_OUTM(m,v)                                           \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_MR2_ADDR,m,v,HWIO_UART_MR2_IN); \
		HWIO_INTFREE()
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_LOOPBACK_BMSK                                             0x80
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_LOOPBACK_SHFT                                              0x7
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_ERROR_MODE_BMSK                                           0x40
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_ERROR_MODE_SHFT                                            0x6
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_BITS_PER_CHAR_BMSK                                        0x30
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_BITS_PER_CHAR_SHFT                                         0x4
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_BITS_PER_CHAR_5_BITS_FVAL                                    0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_BITS_PER_CHAR_6_BITS_FVAL                                  0x1
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_BITS_PER_CHAR_7_BITS_FVAL                                  0x2
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_BITS_PER_CHAR_8_BITS_FVAL                                  0x3
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_STOP_BIT_LEN_BMSK                                          0xc
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_STOP_BIT_LEN_SHFT                                          0x2
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_STOP_BIT_LEN_0_563_FVAL                                      0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_STOP_BIT_LEN_1_000_BIT_TIME_FVAL                           0x1
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_STOP_BIT_LEN_1_563_FVAL                                    0x2
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_STOP_BIT_LEN_2_000_BIT_TIMES_FVAL                          0x3
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_PARITY_MODE_BMSK                                           0x3
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_PARITY_MODE_SHFT                                             0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_PARITY_MODE_NO_PARITY_FVAL                                   0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_PARITY_MODE_ODD_PARITY_FVAL                                0x1
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_PARITY_MODE_EVEN_PARITY_FVAL                               0x2
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MR2_PARITY_MODE_SPACE_PARITY_FVAL                              0x3
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_ADDR                                               0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_PHYS                                               0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_ADDR                                               0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_RMSK                                                      0xf
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_SHFT                                                        0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_OUT(v)                                             \
        out_dword(HWIO_UART_IRDA_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_OUTM(m,v)                                          \
        out_dword_masked(HWIO_UART_IRDA_ADDR,m,v,HWIO_UART_IRDA_shadow)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_IRDA_LOOPBACK_BMSK                                        0x8
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_IRDA_LOOPBACK_SHFT                                        0x3
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_INVERT_IRDA_TX_BMSK                                       0x4
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_INVERT_IRDA_TX_SHFT                                       0x2
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_INVERT_IRDA_RX_BMSK                                       0x2
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_INVERT_IRDA_RX_SHFT                                       0x1
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_IRDA_EN_BMSK                                              0x1
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IRDA_IRDA_EN_SHFT                                                0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_ADDR                                                0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_PHYS                                                0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_ADDR                                                0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_RMSK                                                     0x1ff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_SHFT                                                         0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_OUT(v)                                              \
        out_dword(HWIO_UART_IMR_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_OUTM(m,v)                                           \
        out_dword_masked(HWIO_UART_IMR_ADDR,m,v,HWIO_UART_IMR_shadow)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_TX_DONE_BMSK                                             0x100
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_TX_DONE_SHFT                                               0x8
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_TX_ERROR_BMSK                                             0x80
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_TX_ERROR_SHFT                                              0x7
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_CURRENT_CTS_BMSK                                          0x40
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_CURRENT_CTS_SHFT                                           0x6
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_DELTA_CTS_BMSK                                            0x20
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_DELTA_CTS_SHFT                                             0x5
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_RXLEV_BMSK                                                0x10
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_RXLEV_SHFT                                                 0x4
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_RXSTALE_BMSK                                               0x8
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_RXSTALE_SHFT                                               0x3
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_RXBREAK_BMSK                                               0x4
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_RXBREAK_SHFT                                               0x2
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_RXHUNT_BMSK                                                0x2
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_RXHUNT_SHFT                                                0x1
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_TXLEV_BMSK                                                 0x1
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IMR_TXLEV_SHFT                                                   0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_ADDR                                                0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_PHYS                                                0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_ADDR                                                0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_RMSK                                                   0x3ffff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_SHFT                                                         0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_IN                                                  \
        in_dword_masked(HWIO_UART_IPR_ADDR, HWIO_UART_IPR_RMSK)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_INM(m)                                              \
        in_dword_masked(HWIO_UART_IPR_ADDR, m)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_OUT(v)                                              \
        out_dword(HWIO_UART_IPR_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_OUTM(m,v)                                           \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_IPR_ADDR,m,v,HWIO_UART_IPR_IN); \
		HWIO_INTFREE()
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_STALE_TIMEOUT_MSB_BMSK                                 0x3ff80
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_STALE_TIMEOUT_MSB_SHFT                                     0x7
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_SAMPLE_DATA_BMSK                                          0x40
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_SAMPLE_DATA_SHFT                                           0x6
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_RXSTALE_LAST_BMSK                                         0x20
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_RXSTALE_LAST_SHFT                                          0x5
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_STALE_TIMEOUT_LSB_BMSK                                    0x1f
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_IPR_STALE_TIMEOUT_LSB_SHFT                                       0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MREG_ADDR                                               0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MREG_PHYS                                               0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MREG_ADDR                                               0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MREG_RMSK                                                     0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MREG_SHFT                                                        0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MREG_IN                                                 \
        in_dword_masked(HWIO_UART_MREG_ADDR, HWIO_UART_MREG_RMSK)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MREG_INM(m)                                             \
        in_dword_masked(HWIO_UART_MREG_ADDR, m)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MREG_OUT(v)                                             \
        out_dword(HWIO_UART_MREG_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MREG_OUTM(m,v)                                          \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_MREG_ADDR,m,v,HWIO_UART_MREG_IN); \
		HWIO_INTFREE()
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MREG_DATA_BMSK                                                0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MREG_DATA_SHFT                                                   0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_NREG_ADDR                                               0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_NREG_PHYS                                               0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_NREG_ADDR                                               0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_NREG_RMSK                                                     0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_NREG_SHFT                                                        0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_NREG_IN                                                 \
        in_dword_masked(HWIO_UART_NREG_ADDR, HWIO_UART_NREG_RMSK)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_NREG_INM(m)                                             \
        in_dword_masked(HWIO_UART_NREG_ADDR, m)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_NREG_OUT(v)                                             \
        out_dword(HWIO_UART_NREG_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_NREG_OUTM(m,v)                                          \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_NREG_ADDR,m,v,HWIO_UART_NREG_IN); \
		HWIO_INTFREE()
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_NREG_DATA_BMSK                                                0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_NREG_DATA_SHFT                                                   0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_DREG_ADDR                                               0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_DREG_PHYS                                               0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_DREG_ADDR                                               0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_DREG_RMSK                                                     0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_DREG_SHFT                                                        0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_DREG_IN                                                 \
        in_dword_masked(HWIO_UART_DREG_ADDR, HWIO_UART_DREG_RMSK)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_DREG_INM(m)                                             \
        in_dword_masked(HWIO_UART_DREG_ADDR, m)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_DREG_OUT(v)                                             \
        out_dword(HWIO_UART_DREG_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_DREG_OUTM(m,v)                                          \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_DREG_ADDR,m,v,HWIO_UART_DREG_IN); \
		HWIO_INTFREE()
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_DREG_DATA_BMSK                                                0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_DREG_DATA_SHFT                                                   0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_UART_RF_ADDR                                                 0x28000000
#define HWIO_UART_RF_PHYS                                                 0x28000000
#else
#define HWIO_UART_RF_ADDR                                                 0x28000000
#endif
#define HWIO_UART_RF_RMSK                                                       0xff
#define HWIO_UART_RF_SHFT                                                          0
#define HWIO_UART_RF_IN                                                   \
        in_dword_masked(HWIO_UART_RF_ADDR, HWIO_UART_RF_RMSK)
#define HWIO_UART_RF_INM(m)                                               \
        in_dword_masked(HWIO_UART_RF_ADDR, m)
#define HWIO_UART_RF_DATA_BMSK                                                  0xff
#define HWIO_UART_RF_DATA_SHFT   

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_RFWR_ADDR                                               0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_RFWR_PHYS                                               0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_RFWR_ADDR                                               0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_RFWR_RMSK                                                  0x3ffff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_RFWR_SHFT                                                        0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_RFWR_IN                                                 \
        in_dword_masked(HWIO_UART_RFWR_ADDR, HWIO_UART_RFWR_RMSK)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_RFWR_INM(m)                                             \
        in_dword_masked(HWIO_UART_RFWR_ADDR, m)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_RFWR_OUT(v)                                             \
        out_dword(HWIO_UART_RFWR_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_RFWR_OUTM(m,v)                                          \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_RFWR_ADDR,m,v,HWIO_UART_RFWR_IN); \
		HWIO_INTFREE()
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_RFWR_RFW_BMSK                                              0x3ffff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_RFWR_RFW_SHFT                                                    0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_HCR_ADDR                                                0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_HCR_PHYS                                                0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_HCR_ADDR                                                0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_HCR_RMSK                                                      0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_HCR_SHFT                                                         0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_HCR_IN                                                  \
        in_dword_masked(HWIO_UART_HCR_ADDR, HWIO_UART_HCR_RMSK)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_HCR_INM(m)                                              \
        in_dword_masked(HWIO_UART_HCR_ADDR, m)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_HCR_OUT(v)                                              \
        out_dword(HWIO_UART_HCR_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_HCR_OUTM(m,v)                                           \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_HCR_ADDR,m,v,HWIO_UART_HCR_IN); \
		HWIO_INTFREE()
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_HCR_DATA_BMSK                                                 0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_HCR_DATA_SHFT                                                    0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CSR_ADDR                                                0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CSR_PHYS                                                0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CSR_ADDR                                                0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CSR_RMSK                                                      0xff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CSR_SHFT                                                         0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CSR_OUT(v)                                              \
        out_dword(HWIO_UART_CSR_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CSR_OUTM(m,v)                                           \
        out_dword_masked(HWIO_UART_CSR_ADDR,m,v,HWIO_UART_CSR_shadow)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CSR_UART_RX_CLK_SEL_BMSK                                      0xf0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CSR_UART_RX_CLK_SEL_SHFT                                       0x4
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CSR_UART_TX_CLK_SEL_BMSK                                       0xf
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_CSR_UART_TX_CLK_SEL_SHFT                                         0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TFWR_ADDR                                               0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TFWR_PHYS                                               0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TFWR_ADDR                                               0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TFWR_RMSK                                                  0x3ffff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TFWR_SHFT                                                        0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TFWR_IN                                                 \
        in_dword_masked(HWIO_UART_TFWR_ADDR, HWIO_UART_TFWR_RMSK)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TFWR_INM(m)                                             \
        in_dword_masked(HWIO_UART_TFWR_ADDR, m)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TFWR_OUT(v)                                             \
        out_dword(HWIO_UART_TFWR_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TFWR_OUTM(m,v)                                          \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_TFWR_ADDR,m,v,HWIO_UART_TFWR_IN); \
		HWIO_INTFREE()
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TFWR_TFW_BMSK                                              0x3ffff
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_TFWR_TFW_SHFT                                                    0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_UART_MISR_ADDR                                               0x28000000
#define HWIO_UART_MISR_PHYS                                               0x28000000
#else
#define HWIO_UART_MISR_ADDR                                               0x28000000
#endif
#define HWIO_UART_MISR_RMSK                                                    0x1ff
#define HWIO_UART_MISR_SHFT                                                        0
#define HWIO_UART_MISR_IN                                                 \
        in_dword_masked(HWIO_UART_MISR_ADDR, HWIO_UART_MISR_RMSK)
#define HWIO_UART_MISR_INM(m)                                             \
        in_dword_masked(HWIO_UART_MISR_ADDR, m)
#define HWIO_UART_MISR_UART_MISR_BMSK                                          0x1ff
#define HWIO_UART_MISR_UART_MISR_SHFT                                              0

#ifdef FEATURE_MEMORY_VIRTUAL
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_ADDR                                             0x28000000
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_PHYS                                             0x28000000
#endif
#else
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_ADDR                                             0x28000000
#endif
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_RMSK                                                   0x3f
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_SHFT                                                      0
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_IN                                               \
        in_dword_masked(HWIO_UART_MNDREG_ADDR, HWIO_UART_MNDREG_RMSK)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_INM(m)                                           \
        in_dword_masked(HWIO_UART_MNDREG_ADDR, m)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_OUT(v)                                           \
        out_dword(HWIO_UART_MNDREG_ADDR,v)
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_OUTM(m,v)                                        \
		HWIO_INTLOCK(); \
	out_dword_masked_ns(HWIO_UART_MNDREG_ADDR,m,v,HWIO_UART_MNDREG_IN); \
		HWIO_INTFREE()
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_MREG_LSB_BMSK                                          0x20
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_MREG_LSB_SHFT                                           0x5
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_NREG_LSB_BMSK                                          0x1c
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_NREG_LSB_SHFT                                           0x2
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_DREG_LSB_BMSK                                           0x3
#endif
#if !defined(FEATURE_SECOND_UART)
#define HWIO_UART_MNDREG_DREG_LSB_SHFT                                             0
#endif

#ifdef FEATURE_MEMORY_VIRTUAL
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_ADDR(c)                      (0xf9003688+0x100*(c))
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_PHYS(c)                 (0x90003688+0x100*(c))
#else
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_ADDR(c)                      (0x90003688+0x100*(c))
#endif
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_RMSK                               0xffffffff
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_SHFT                                        0
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_OUTI(c,v) \
        out_dword(HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_ADDR(c),v)
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_OUTMI(c,mask,v) \
        out_dword_masked(HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_ADDR(c),mask,v,HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_shadow[c])
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_PWR_HYST_BMSK                      0xe0000000
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_PWR_HYST_SHFT                            0x1d
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_NOTCH_PWR_REF2_BMSK                0x1ff80000
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_NOTCH_PWR_REF2_SHFT                      0x13
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_NOTCH_PWR_REF1_BMSK                   0x7fe00
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_NOTCH_PWR_REF1_SHFT                       0x9
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_NOTCH_NBIT2_BMSK                        0x1c0
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_NOTCH_NBIT2_SHFT                          0x6
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_NOTCH_NBIT1_BMSK                         0x38
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_NOTCH_NBIT1_SHFT                          0x3
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_NOTCH_NBIT0_BMSK                          0x7
#define HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_NOTCH_NBIT0_SHFT                            0

/* OBJECT libs_xo/rfmsm6500/ftm_sample_capture_lib.o
"..\..\services\ftm\lib\ftm_sample_capture_lib.c", line 220: Error: C2456E: undeclared name, inventing 'extern int HWIO_GPS_RTC_CNT_IN'
..\..\services\ftm\lib\ftm_sample_capture_lib.c: 0 warnings, 1 error, 0 serious errors
make[1]: *** [libs_xo/rfmsm6500/ftm_sample_capture_lib.o] Error 1
make: *** [libs] Error 2
*/

#define HWIO_RXF_GPF_COEF_ADDR                                            0x28000000
#define HWIO_RXF_GPF_COEF_RMSK                                              0x1fffff
#define HWIO_RXF_GPF_COEF_SHFT                                                     0
#define HWIO_RXF_GPF_COEF_OUT(v)                                          \
        out_dword(HWIO_RXF_GPF_COEF_ADDR,v)
#define HWIO_RXF_GPF_COEF_OUTM(m,v)                                       \
        out_dword_masked(HWIO_RXF_GPF_COEF_ADDR,m,v,HWIO_RXF_GPF_COEF_shadow)
#define HWIO_RXF_GPF_COEF_COMPLEX_BMSK                                      0x1f0000
#define HWIO_RXF_GPF_COEF_COMPLEX_SHFT                                          0x10
#define HWIO_RXF_GPF_COEF_DROOP_BMSK                                          0xf800
#define HWIO_RXF_GPF_COEF_DROOP_SHFT                                             0xb
#define HWIO_RXF_GPF_COEF_GAIN_SEL_BMSK                                        0x700
#define HWIO_RXF_GPF_COEF_GAIN_SEL_SHFT                                          0x8
#define HWIO_RXF_GPF_COEF_GAIN_SEL_NO_LEFT_SHIFT_OR_RIGHT_SHIFT_FVAL               0
#define HWIO_RXF_GPF_COEF_GAIN_SEL_LEFT_SHIFT_BY_1_BIT_FVAL                      0x1
#define HWIO_RXF_GPF_COEF_GAIN_SEL_LEFT_SHIFT_BY_2_BITS_FVAL                     0x2
#define HWIO_RXF_GPF_COEF_GAIN_SEL_LEFT_SHIFT_BY_3_BITS_FVAL                     0x3
#define HWIO_RXF_GPF_COEF_GAIN_SEL_RIGHT_SHIFT_BY_1_BIT_FVAL                     0x5
#define HWIO_RXF_GPF_COEF_GAIN_SEL_RIGHT_SHIFT_BY_2_BITS_FVAL                    0x6
#define HWIO_RXF_GPF_COEF_GAIN_SEL_RIGHT_SHIFT_BY_3_BITS_FVAL                    0x7
#define HWIO_RXF_GPF_COEF_GREY_MAP_BMSK                                         0xff
#define HWIO_RXF_GPF_COEF_GREY_MAP_SHFT                                            0

#define HWIO_RXF_GPF_COEF_NOTCH0_ADDR                                     0x28000000
#define HWIO_RXF_GPF_COEF_NOTCH0_RMSK                                     0xffffffff
#define HWIO_RXF_GPF_COEF_NOTCH0_SHFT                                              0
#define HWIO_RXF_GPF_COEF_NOTCH0_OUT(v)                                   \
        out_dword(HWIO_RXF_GPF_COEF_NOTCH0_ADDR,v)
#define HWIO_RXF_GPF_COEF_NOTCH0_OUTM(m,v)                                \
        out_dword_masked(HWIO_RXF_GPF_COEF_NOTCH0_ADDR,m,v,HWIO_RXF_GPF_COEF_NOTCH0_shadow)
#define HWIO_RXF_GPF_COEF_NOTCH0_ROT_I_H_BMSK                             0xff000000
#define HWIO_RXF_GPF_COEF_NOTCH0_ROT_I_H_SHFT                                   0x18
#define HWIO_RXF_GPF_COEF_NOTCH0_ROT_I_L_BMSK                               0xff0000
#define HWIO_RXF_GPF_COEF_NOTCH0_ROT_I_L_SHFT                                   0x10
#define HWIO_RXF_GPF_COEF_NOTCH0_ROT_R_H_BMSK                                 0xff00
#define HWIO_RXF_GPF_COEF_NOTCH0_ROT_R_H_SHFT                                    0x8
#define HWIO_RXF_GPF_COEF_NOTCH0_ROT_R_L_BMSK                                   0xff
#define HWIO_RXF_GPF_COEF_NOTCH0_ROT_R_L_SHFT                                      0

#define HWIO_RXF_GPF_COEF_NOTCH1_ADDR                                     0x28000000
#define HWIO_RXF_GPF_COEF_NOTCH1_RMSK                                     0xffffffff
#define HWIO_RXF_GPF_COEF_NOTCH1_SHFT                                              0
#define HWIO_RXF_GPF_COEF_NOTCH1_OUT(v)                                   \
        out_dword(HWIO_RXF_GPF_COEF_NOTCH1_ADDR,v)
#define HWIO_RXF_GPF_COEF_NOTCH1_OUTM(m,v)                                \
        out_dword_masked(HWIO_RXF_GPF_COEF_NOTCH1_ADDR,m,v,HWIO_RXF_GPF_COEF_NOTCH1_shadow)
#define HWIO_RXF_GPF_COEF_NOTCH1_ROT_I_H_BMSK                             0xff000000
#define HWIO_RXF_GPF_COEF_NOTCH1_ROT_I_H_SHFT                                   0x18
#define HWIO_RXF_GPF_COEF_NOTCH1_ROT_I_L_BMSK                               0xff0000
#define HWIO_RXF_GPF_COEF_NOTCH1_ROT_I_L_SHFT                                   0x10
#define HWIO_RXF_GPF_COEF_NOTCH1_ROT_R_H_BMSK                                 0xff00
#define HWIO_RXF_GPF_COEF_NOTCH1_ROT_R_H_SHFT                                    0x8
#define HWIO_RXF_GPF_COEF_NOTCH1_ROT_R_L_BMSK                                   0xff
#define HWIO_RXF_GPF_COEF_NOTCH1_ROT_R_L_SHFT                                      0

#endif /* MSMHWIOREG_PORT_H */
