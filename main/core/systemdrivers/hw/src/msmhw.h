#ifndef MSMHW_H
#define MSMHW_H
/*===========================================================================

                     MSM6100 Hardware Support 

DESCRIPTION
  This file contains definitions that specify which MSM H/W features are 
  supported on each MSM.

Copyright (c) 2000, 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/msm6050hw.h_v   1.6   07 Feb 2002 14:46:02   donb  $
$Header: //depot/asic/qsc1100/drivers/hw/msmhw.h#3 $ $DateTime: 2009/02/17 05:36:58 $ $Author: rrevanur $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/09   hvo     Featurized the Modem Turbo Decode, only available for mid
                   and high tiers
11/27/02   cr      Added MSMHW_EXTENDED_LTU_LEN feature to handle dynamic    
                   LTU length in function deint_load_sch_pkt_params().       
07/23/02   dle     Added MSMHW_LCD_BUS_SIZED.
02/07/02   dlb     Added MSMHW_DECODER_DYNAMIC_CLOCK_GATING.
01/09/02   dbc     Added MSMHW_NOT_USES_CAGC_HIGH_LOW flag. MSM6050 does not
                   have CAGC HIGH_LOW dependency for any CAGC registers.
11/28/01   dlb     Added MSMHW_EXTENDED_ENERGY_MATRIX.
09/13/01   dsb     Created from msm5100hw.h

===========================================================================*/
#include "customer.h"

#define MSMHW_EXTENDED_LTU_LEN
        
/* ------------ MSM6050 specific ------------ */        

/* 6050 does not use CAGC HIGH/LOW */                                        
#define MSMHW_NOT_USES_CAGC_HIGH_LOW

/* 6050 has a 32-bit internal bus and can support 32 bit wide registers */                      
#define MSMHW_32BIT_MSM_REGISTERS
                                                
/* New Hardware revision number register */
#define MSMHW_EXTENDED_HW_REVISION_NUMBER

/* Internal Stereo DAC M/N counters */
#define MSMHW_INTERNAL_STEREO_DAC

/* Extended Energy Matrix fields in the decoder output buffer */
#define MSMHW_EXTENDED_ENERGY_MATRIX

/* Decoder Dynamic Clock Gating */
#ifdef T_RUMI_CLK_BLOCK
#error code not present
#endif
                

                                                
/* ------------ MSM5100 specific ------------ */
  
/* 5100 contains an option allowing the MSM to latch interrupts by
   edge or level */          
#define MSMHW_EDGE_LEVEL_INT_LATCH_OPTION                              
                            
/* 5100 has turbo coding */
#ifdef T_QSC1110
#ifdef FEATURE_CLKREGIM_2X_MODE
#define MSMHW_TURBO_CODING
#endif
#endif
/* QDSP2 needs the interrupt cleared before the ISR is invoked */
#define MSMHW_ADSP_NEEDS_INT_CLEARED_B4_ISR

                                                  
/* ------------ MSM510x family ------------ */

/* Primary UART has larger FIFOs */
#ifndef FEATURE_SC2X_SWAP_UART_DM_WITH_UART2
#define MSMHW_UART1_512BYTE_FIFOS
#endif

#define MSMHW_UART_EDGE_DRIVEN_INTERRUPT
/* USB FIFO */
#define MSMHW_USB_256BYTE_FIFOS

/* GPIO_INT0-4 have been moved out of the primary level, and interrupts rearranged */
#define MSMHW_GROUP_INT_FOR_ALL_GPIO

/* Define if you need Q-offset spectral inversion. */                                 
#undef MSMHW_Q_RX_SPECTRAL_INVERSION

/* Do not define this feature in order for MSM5100 to pull in
   MSM3300 defines for QDSP2 command interface.
 */
#undef MSMHW_CONCURRENT_VBAP_AMPS

#define MSMHW_LCD_PORT_BUS_SIZED

#ifdef T_MSM6300
#error code not present
#endif /* T_MSM6300 */

/* For camera */
#define MSMHW_CAMIF_V2

#endif /* MSMHW_H */

