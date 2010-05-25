#ifndef SIORSREG_H
#define SIORSREG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      S I O   M S M   U A R T   R E G I S T E R    D E S C R I P T I O N

                   SIO Register Description for RS-232 UARTs

DESCRIPTION

  This file contains the SIO UART register descriptions.

Copyright (c) 2000-2002, 2008 by QUALCOMM Incorporated.  
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/siorsreg.h_v   1.10   29 Oct 2002 16:20:26   chenc  $
  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siorsreg.h#1 $ $DateTime: 2009/03/02 15:50:48 $ $Author: cmihalik $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/08   jmm     Added UART2DM
02/01/08   rh      Unwrap the definition for MSMU_MR1_RFR_LVL_HI_MASK
07/31/06   sr      Add support for 3000kbps
05/18/06   sr      Add support for 921.6kbps
05/10/06   rh      Adding a new flow control type SIO_CTSRFR_AUTO_FCTL
10/29/04   cc      Fixed the MSMU_MR2_2SB bit mask. 
11/24/03   cc      Added FEATURE_JCDMA_SIO and OR it with FEATURE_JCDMA_DS. 
                   Fixed bit rate setting for 76800.                      
04/29/02   cc      Renamed FEATURE_NEW_UART_REG_STRUCT to
                   MSMHW_UART_MREG_13BIT_NREG_15BIT_DREG_13BIT
03/15/02   cc      Merge in Cheetah changes. 
02/06/02   cc      Defined MSMU_SIM_SEL_MASK. 
01/14/02   cc      Defined MSMU_MR1_RFR_LVL_HI_MASK, MSMU_IPR_STALE_LSB
                   and MSMU_IPR_STALE_MSB.
12/19/01   thh     Added support for 26MHz TCXO under BSP_TCXO_26P0_MHZ.
                   Updated copyright date.
12/12/01   JC      Added 13.0 MHz TCXO support for GSM and FEATURE_BSP so
                   that s/w doesn't have dependency on T_MSM33 (legacy).
06/18/01   dwp     Add support for FEATURE_UART_RATE1.
03/23/01   dwp     More T-53/JCDMA mods for FEATURE_UART_TCXO_CLK_FREQ.
01/26/01   ych     Merged T-53/JCDMA features for SK.
08/22/00   dsb     Added BSP support for TCXO.
06/15/00   jwh     Featurized MSMU_MR1_RFR_LVL_MASK.
06/05/00   jwh     Added support for FEATURE_TCXO1920.
03/09/00   jwh     Created file from siors232.c register descriptions

===========================================================================*/

#include "bsp.h"
#include "clk.h"

/*---------------------------------------------------------------------------
 Macro to write to MSM UART registers. 
---------------------------------------------------------------------------*/

#define SIO_NOP 

#define MSMU_OUT( reg, val)     \
   (void)outpw( (reg), (val));   \
   SIO_NOP

#define MSMUW_OUT( reg, val)   \
   (void)outpw( (reg), (val)); \
   SIO_NOP

#define MSMUDW_OUT( reg, val)    \
   (void)outpdw( (reg), (val) ); \
   SIO_NOP

/*===========================================================================
 Register Descriptions
===========================================================================*/

/* Mode Register 1 */

#define MSMU_MR1_RFRC         0x80      /* Ready-for-receiving Control      */
#define MSMU_MR1_CTSC         0x40      /* Clear-to-send Control            */

#define MSMU_MR1_RFR_LVL_MASK 0x3f      /* Mask to use when writing to lower
                                          six bits of MSMU_MR1 register    */
#define MSMU_MR1_RFR_LVL_HI_MASK 0x700  /* Mask to use when writing to bit
                                          10:8 of the MSMU_MR1 register    */     

/* Mode Register 2 */

#define MSMU_MR2_LOOPBACK 0x80         /* Channel Mode                     */
#define MSMU_MR2_ERRMODE  0x40         /* Error Mode                       */
#define MSMU_MR2_5BPC     0x00         /* 5 Bits per character             */
#define MSMU_MR2_6BPC     0x10         /* 6 Bits per character             */
#define MSMU_MR2_7BPC     0x20         /* 7 Bits per character             */
#define MSMU_MR2_8BPC     0x30         /* 8 Bits per character             */
#define MSMU_MR2_1SB      0x04         /* 1 Stop Bit                       */
#define MSMU_MR2_2SB      0x0C         /* 2 Stop Bits                      */
#define MSMU_MR2_OPAR     0x01         /* Odd Parity                       */
#define MSMU_MR2_EPAR     0x02         /* Even Parity                      */
#define MSMU_MR2_SPAR     0x03         /* Space Parity                     */

/* Clock Select Register */

#ifdef FEATURE_UART_TCXO_CLK_FREQ

/* If the UART gets TCXO instead of TCXO/4 then of all of the codes give   */
/* baud rates that are four times faster because we're leaving the M/N     */
/* counter alone for the (upper) standard bauds.                                   */

#define MSMU_CSR_75_BPS    0x00        /* 75 bits per second               */
#define MSMU_CSR_150_BPS   0x00        /* 150 bits per second              */
#define MSMU_CSR_300_BPS   0x00        /* 300 bits per second              */
#define MSMU_CSR_600_BPS   0x11        /* 600 bits per second              */
#define MSMU_CSR_1200_BPS  0x22        /* 1200 bits per second             */
#define MSMU_CSR_2400_BPS  0x33        /* 2400 bits per second             */
#define MSMU_CSR_4800_BPS  0x44        /* 4800 bits per second             */
#define MSMU_CSR_9600_BPS  0x55        /* 9600 bits per second             */
#define MSMU_CSR_144K_BPS  0x66        /* 14.4K bits per second            */
#define MSMU_CSR_192K_BPS  0x77        /* 19.2K bits per second            */
#define MSMU_CSR_288K_BPS  0x88        /* 28.8K bits per second            */
#define MSMU_CSR_384K_BPS  0x99        /* 38.4K bits per second            */
#define MSMU_CSR_576K_BPS  0xAA        /* 57.6K bits per second            */
#define MSMU_CSR_1152K_BPS 0xCC        /* 115.2K bits per second           */
#define MSMU_CSR_2304K_BPS 0xEE        /* 230.4K bits per second           */
#define MSMU_CSR_4608K_BPS 0xFF        /* 460.8K bits per second           */
#define MSMU_CSR_768K_BPS  0xBB        /* 76.8K bits per second            */
#define MSMU_CSR_9216K_BPS   0xFF        /* 921.6K bits per second         */
#define MSMU_CSR_2900K_BPS   0xFF        /* 2.9M bits per second           */
#define MSMU_CSR_3200K_BPS   0xFF        /* 3.2M bits per second           */
#define MSMU_CSR_36864K_BPS  0xFF        /* 3686.4K bits per second        */
#define MSMU_CSR_4000K_BPS   0xFF        /* 4 Mbits per second             */

#else   /* not #ifdef FEATURE_UART_TCXO_CLK_FREQ */

#define MSMU_CSR_75_BPS    0x00        /* 75 bits per second               */
#define MSMU_CSR_150_BPS   0x11        /* 150 bits per second              */
#define MSMU_CSR_300_BPS   0x22        /* 300 bits per second              */
#define MSMU_CSR_600_BPS   0x33        /* 600 bits per second              */
#define MSMU_CSR_1200_BPS  0x44        /* 1200 bits per second             */
#define MSMU_CSR_2400_BPS  0x55        /* 2400 bits per second             */
#define MSMU_CSR_3600_BPS  0x66        /* 3600 bits per second             */
#define MSMU_CSR_4800_BPS  0x77        /* 4800 bits per second             */
#define MSMU_CSR_7200_BPS  0x88        /* 7200 bits per second             */
#define MSMU_CSR_9600_BPS  0x99        /* 9600 bits per second             */
#define MSMU_CSR_144K_BPS  0xAA        /* 14.4K bits per second            */
#define MSMU_CSR_192K_BPS  0xBB        /* 19.2K bits per second            */
#define MSMU_CSR_288K_BPS  0xCC        /* 28.8K bits per second            */
#define MSMU_CSR_384K_BPS  0xDD        /* 38.4K bits per second            */
#define MSMU_CSR_576K_BPS  0xEE        /* 57.6K bits per second            */
#define MSMU_CSR_1152K_BPS 0xFF        /* 115.2K bits per second           */
#define MSMU_CSR_768K_BPS  0xFF        /* 76.8K bits per second            */
#define MSMU_CSR_9216K_BPS   0xFF        /* 921.6K bits per second         */
#define MSMU_CSR_2900K_BPS   0xFF        /* 2.9M bits per second           */
#define MSMU_CSR_3200K_BPS   0xFF        /* 3.2M bits per second           */
#define MSMU_CSR_36864K_BPS  0xFF        /* 3686.4K bits per second        */
#define MSMU_CSR_4000K_BPS   0xFF        /* 4 Mbits per second             */

#endif  /* #ifdef FEATURE_UART_TCXO_CLK_FREQ */

#define MSMU_CSR_TX_ONLY_MASK  0x0F    /* Mask to change only TX CSR       */
#define MSMU_CSR_RX_ONLY_MASK  0xF0    /* Mask to change only RX CSR       */

/* Status Register */

#define MSMU_SR_RXHUNT    0x80         /* Hunt Character Received          */
#define MSMU_SR_RXBREAK   0x40         /* Break Received                   */
#define MSMU_SR_PF_ERR    0x20         /* Parity of Framing Error          */
#define MSMU_SR_OVR_ERR   0x10         /* Overrun Error                    */
#define MSMU_SR_TXEMT     0x08         /* Transmitter Empty                */
#define MSMU_SR_TXRDY     0x04         /* Transmitter Ready                */
#define MSMU_SR_RXFULL    0x02         /* Receiver Full                    */
#define MSMU_SR_RXRDY     0x01         /* Receiver Ready                   */

/* Command Register */

#define MSMU_CR_RESET_RX      0x10     /* Reset Receiver                   */
#define MSMU_CR_RESET_TX      0x20     /* Reset Transmitter                */
#define MSMU_CR_RESET_ERR     0x30     /* Reset Error Status               */
#define MSMU_CR_RESET_BRK_INT 0x40     /* Reset Break Change Interrupt     */
#define MSMU_CR_STA_BRK       0x50     /* Start Break                      */
#define MSMU_CR_STO_BRK       0x60     /* Stop Break                       */
#define MSMU_CR_CLR_DCTS      0x70     /* Clear CTS Change (delta)         */
#define MSMU_CR_SAMP_MODE     0x90     /* Sample Data Mode                 */
#define MSMU_CR_TEST_PARITY   0xA0     /* Test Parity                      */
#define MSMU_CR_TEST_FRAME    0xB0     /* Test Frame                       */
#define MSMU_CR_RESET_SAMPLE  0xC0     /* Reset Sample Data Mode           */
#define MSMU_CR_SET_RFR       0xD0     /* Set RFR                          */
#define MSMU_CR_RESET_RFR     0xE0     /* Reset RFR                        */
#define MSMU_CR_DIS_TX        0x08     /* Disable Transmitter              */
#define MSMU_CR_ENA_TX        0x04     /* Enable Transmitter               */
#define MSMU_CR_DIS_RX        0x02     /* Disable Receiver                 */
#define MSMU_CR_ENA_RX        0x01     /* Enable Receiver                  */
#if defined(FEATURE_USE_UART1DM) || defined(FEATURE_USE_UART2DM)
#define MSMU_CR_RESET_STALE   0x80    /* Enable CR protection             */
#define MSMU_CR_ENA_CR_PROT   0x100    /* Enable CR protection             */
#define MSMU_CR_DIS_CR_PROT   0x200    /* Disable CR protection            */
#define MSMU_CR_RESET_TX_RDY  0x300    /* Clears TX_READY interrupt        */
#define MSMU_CR_FORCE_STALE   0x400    /* SW force stale                   */
#define MSMU_CR_ENA_STALE_EVT 0x500    /* Enables Stale Event              */
#define MSMU_CR_DIS_STALE_EVT 0x600    /* Disables Stale Event             */
#endif

/* Interrupt Programming Register */

#define MSMU_IPR_NEW_SAMPLE_MODE  0x40  /* New Sample Mode method          */
#define MSMU_IPR_NEW_RXSTALE      0x20  /* New RXSTALE Interrupt method    */
#define MSMU_IPR_STALE_LSB        0x1f  /* STALE_TIMEOUT_LSB               */
#define MSMU_IPR_STALE_MSB        0x380 /* STALE_TIMEOUT_MSB               */

/* Interrupt Mask Register */

#if defined(FEATURE_USE_UART1DM) || defined(FEATURE_USE_UART2DM)
#define MSMU_IMR_TX_READY    0x80      /* TX Ready                         */
#endif
#define MSMU_IMR_CUR_CTS     0x40      /* Current CTS                      */
#define MSMU_IMR_DELTA_CTS   0x20      /* Delta CTS                        */
#define MSMU_IMR_RXLEV       0x10      /* RX Level exceeded                */
#define MSMU_IMR_RXSTALE     0x08      /* Stale RX character occurred      */
#define MSMU_IMR_RXBREAK     0x04      /* RX Break occurred                */
#define MSMU_IMR_RXHUNT      0x02      /* RX Hunt character received       */
#define MSMU_IMR_TXLEV       0x01      /* TX Level or below met            */

/* Interrupt Status Register */

#if defined(FEATURE_USE_UART1DM) || defined(FEATURE_USE_UART2DM)
#define MSMU_ISR_TX_READY    0x80      /* TX Ready                         */
#endif
#define MSMU_ISR_CUR_CTS     0x40      /* Current CTS                      */
#define MSMU_ISR_DELTA_CTS   0x20      /* Delta CTS                        */
#define MSMU_ISR_RXLEV       0x10      /* RX Level met or exceeded         */
#define MSMU_ISR_RXSTALE     0x08      /* Stale RX character occurred      */
#define MSMU_ISR_RXBREAK     0x04      /* RX Break occurred                */
#define MSMU_ISR_RXHUNT      0x02      /* RX Hunt character received       */
#define MSMU_ISR_TXLEV       0x01      /* TX Level or below met            */

#if defined(FEATURE_USE_UART1DM) || defined(FEATURE_USE_UART2DM)

#define MSMU_DMEN_RXTX_DM_DIS 0x00
#define MSMU_DMEN_RXTX_DM_ENA 0x03
#define MSMU_DMEN_RX_DM_DIS   0x01

/* Equal division of TX and RX fifo */
#define MSMU_BADR_DEFAULT     0x40

#endif

/* For M/N counter registers */
#ifdef BSP_OSCILLATOR_IN
#if (BSP_OSCILLATOR_IN == BSP_TCXO_19P2_MHZ) /* TCXO = 19.20 */

  #define MSMU_MVR_DEFAULT     0x0180   /* Default value of M register */
  #define MSMU_NVR_DEFAULT     0xFD97   /* Default value of N register */
  #define MSMU_DVR_DEFAULT     0x01F4   /* Default value of D register */

  #define MSMU_MVR_DEFAULT_76800     0x100   /* Default value of M register         */
  #define MSMU_NVR_DEFAULT_76800     0xF917   /* Default value of N register         */
  #define MSMU_DVR_DEFAULT_76800     0x1f4   /* Default value of D register         */

  #ifdef T_CLKREGIM
  #if (T_CLKREGIM == 7500)
    /* Values of MND for UART source freq of 196.6091Mhz */
    #define MSMU_MVR_DEFAULT_UARTDM  0x0480   /* Default value of M register */
    #define MSMU_NVR_DEFAULT_UARTDM  0x8c7f   /* Default value of N register */
    #define MSMU_DVR_DEFAULT_UARTDM  0x3c00   /* Default value of D register */

    #define MSMU_MVR_921600   0x3
    #define MSMU_NVR_921600   0x7FDA
    #define MSMU_DVR_921600   0x14

    #define MSMU_MVR_2900000  0x2D5
    #define MSMU_NVR_2900000  0x76D4
    #define MSMU_DVR_2900000  0x600

    #define MSMU_MVR_3200000  0x28B
    #define MSMU_NVR_3200000  0x78C6
    #define MSMU_DVR_3200000  0x4E2
    
    #define MSMU_MVR_3686400  0x12B
    #define MSMU_NVR_3686400  0x7D42
    #define MSMU_DVR_3686400  0x1F4

    #define MSMU_MVR_4000000  0x28B
    #define MSMU_NVR_4000000  0x7ABA
    #define MSMU_DVR_4000000  0x3E8
  #endif

  #if (T_CLKREGIM == 7200)
    /* Values of MND for UART source freq of 245.7600Mhz */
    #define MSMU_MVR_DEFAULT_UARTDM  0x0240   /* Default value of M register */
    #define MSMU_NVR_DEFAULT_UARTDM  0xB73f   /* Default value of N register */
    #define MSMU_DVR_DEFAULT_UARTDM  0x2580   /* Default value of D register */

    #define MSMU_MVR_921600   0x480
    #define MSMU_NVR_921600   0xB97f
    #define MSMU_DVR_921600   0x2580

    #define MSMU_MVR_3200000  0x500
    #define MSMU_NVR_3200000  0xECFF
    #define MSMU_DVR_3200000  0xC00
    
    #define MSMU_MVR_3686400  0x900
    #define MSMU_NVR_3686400  0xE37F
    #define MSMU_DVR_3686400  0x12C0

    #define MSMU_MVR_4000000  0x1900
    #define MSMU_NVR_4000000  0xB8FF
    #define MSMU_DVR_4000000  0x3000
  #endif
  #endif /* Is T_CLKREGIM defined? */

#elif (BSP_OSCILLATOR_IN == BSP_TCXO_19P68_MHZ)
  #define MSMU_MVR_DEFAULT     0x180    /* Default value of M register         */
  #define MSMU_NVR_DEFAULT     0xFD7B   /* Default value of N register         */
  #define MSMU_DVR_DEFAULT     0x201    /* Default value of D register         */

  #define MSMU_MVR_DEFAULT_76800     0x100    /* Default value of M register         */
  #define MSMU_NVR_DEFAULT_76800     0xFCFB   /* Default value of N register         */
  #define MSMU_DVR_DEFAULT_76800     0x201    /* Default value of D register         */

/* Use of 19.68 values for 19.8 has been sufficient thus far, and has
   neglibible error, less then 5% */

#elif (BSP_OSCILLATOR_IN == BSP_TCXO_19P8_MHZ)
  #define MSMU_MVR_DEFAULT     0x180    /* Default value of M register         */
  #define MSMU_NVR_DEFAULT     0xFD7B   /* Default value of N register         */
  #define MSMU_DVR_DEFAULT     0x201    /* Default value of D register         */

  #define MSMU_MVR_DEFAULT_76800     0x100    /* Default value of M register         */
  #define MSMU_NVR_DEFAULT_76800     0xFCFB   /* Default value of N register         */
  #define MSMU_DVR_DEFAULT_76800     0x201    /* Default value of D register         */

#elif (BSP_OSCILLATOR_IN == BSP_TCXO_13P0_MHZ)
#define MSMU_MVR_DEFAULT     0x0480   /* Default value of M register         */
#define MSMU_NVR_DEFAULT     0xE4C2   /* Default value of N register         */
#define MSMU_DVR_DEFAULT     0x0900   /* Default value of D register         */

#elif (BSP_OSCILLATOR_IN == BSP_TCXO_26P0_MHZ)

#define MSMU_MVR_DEFAULT     0x0480   /* Default value of M register         */
#define MSMU_NVR_DEFAULT     0xE4C2   /* Default value of N register         */
#define MSMU_DVR_DEFAULT     0x0900   /* Default value of D register         */

#else
#error Unsupported TCXO value
#endif  /* TCXO = 19.2 */

#else /* not T_MSM33 */
  #error "Old clock definition, not supported anymore. Use BSP_OSCILLATOR_IN"
#endif /* T_MSM33 */

#define MSMU_MVR_DISABLE     0x0000   /* Value to write to M register when
                                       disabling clock                     */
#define MSMU_NVR_DISABLE     0xffff   /* Value to write to N register when
                                       disabling clock                     */
#define MSMU_DVR_DISABLE     0x0000   /* Value to write to D register when
                                       disabling clock                     */
#define MSMU_MNDR_DISABLE_MASK HWIO_RMSK(UART_MNDREG)
                                    /* Mask to use when
                                       writing to MND register to disable
                                       UART clock                          */
/* For UART IRDA Register */

#define MSMU_IRDA_DISABLE    0x00   /* Disable UART IRDA transceiver       */

/* For UART SIM_CFG register */
#define MSMU_SIM_SEL_MASK    0x01   /* SIM select mask                     */

/* General values in enabling/disabling active high bits in registers      */

#ifdef T_MSM31
#define MSMU_ENA             0xffff /* Value to be used in enabling.
                                       Should be used with masks.          */
#define MSMU_DIS             0x0000 /* Value to be used in disabling.
                                       Should be used with masks.          */
#else
#define MSMU_ENA             0xff   /* Value to be used in enabling.
                                       Should be used with masks.          */
#define MSMU_DIS             0x00   /* Value to be used in disabling.
                                       Should be used with masks.          */
#endif /* T_MSM31 */


#endif /* SIORSREG_H */

