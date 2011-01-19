#ifndef BBRX_6500_H
#define BBRX_6500_H

#include "adie.h"

/*===========================================================================


                    ANALOG BASEBAND RECEIVER HEADER FILE

DESCRIPTION
  This file contains declarations and definitions to support interaction
  with the analog die of a stacked digital/analog die MSM.

Copyright (c) 2001, 2002, 2003, 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/bbrx_1x.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/06   hkk     Finally, the reference voltage is changed to 0.6V. 
09/07/06   gfr     Added GSM set_mode arrays definitions.
08/14/06   hkk     GPR register value changed to low current setting
07/06/06   hkk     VREF values for GPS changed
05/22/06   hkk     Common Adie driver for 1x and UMTS 
05/04/06   hkk     BBRx sleep/wakeup - additional changes
04/17/06   hkk     Added changes for BBRX sleep/wakeup for current saving with Saturn
03/17/05   hkk     Created file.

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "bbrx.h"


/****************************************************************************/
/*                          SATURN    REGISTERS                                    */
/****************************************************************************/

#define BBRX_GPR_A1          0x03
#define BBRX_GPR_A2          0x04
#define BBRX_DDO_A1          0x05
#define BBRX_DDO_A2          0x06
#define BBRX_DDO_A3          0x07
#define BBRX_DDI_A1          0x08 
#define BBRX_CH1_EN1         0x09 
#define BBRX_CH1_EN2         0x0A 
#define BBRX_CH1_SW1	     0x0B		
#define BBRX_CH1_SW2         0x0C
#define BBRX_CH1_C1          0x0D
#define BBRX_CH1_C2          0x0E 
#define BBRX_CH1_VREF        0x0F
#define BBRX_CH1_GMC         0x10   
#define BBRX_CH1_GMC_ADJ     0x11 
#define BBRX_CH1_I1          0x12 
#define BBRX_CH1_I2          0x13 
#define BBRX_CH1_IBUF1       0x14 
#define BBRX_CH1_IBUF2       0x15 
#define BBRX_CH1_RSTR        0x16 
#define BBRX_CH1_TMUX        0x17 
#define BBRX_CH1_COMP        0x18 
#define BBRX_CH0_EN1         0x19 
#define BBRX_CH0_EN2         0x1A 
#define BBRX_CH0_SW1         0x1B 
#define BBRX_CH0_SW2         0x1C 
#define BBRX_CH0_C1          0x1D 
#define BBRX_CH0_C2          0x1E     
#define BBRX_CH0_VREF        0x1F
#define BBRX_CH0_I1          0x20
#define BBRX_CH0_I2          0x21
#define BBRX_CH0_IBUF1       0x22
#define BBRX_CH0_IBUF2       0x23
#define BBRX_CH0_RSTR        0x24
#define BBRX_CH0_TMUX        0x25
#define BBRX_CH0_COMP        0x26
#define BBRX_CHO_R1          0x27
#define BBRX_CHO_R2          0x28
#define BBRX_CH1_R1          0x29
#define BBRX_CH1_R2          0x2A
#define BBRX_RESV_EN         0x2B
#define BBRX_RESV_1			 0x2C
#define BBRX_RESV_2 		 0x2D
#define BBRX_RESV_3			 0x2E
#define BBRX_RESV_4	    	 0x2F

#define BBRX_UNDEFINED_R    0xff

/****************************************************************************/
/*                          1x   VALUES                                    */
/****************************************************************************/

#ifndef FEATURE_QUASAR_RF

#define BBRX_GPR_A1_V         0x00
#define BBRX_GPR_A2_V         0x00 
#define BBRX_DDO_A1_V         0x00 
#define BBRX_DDO_A2_V         0x00 
#define BBRX_DDO_A3_V         0x00 
#define BBRX_DDI_A1_V         0x00 
#define BBRX_1x_CH1_EN1_V     0x0d 
#define BBRX_1x_CH1_EN2_V     0x18 
#define BBRX_1x_CH1_SW1_V     0x41 
#define BBRX_1x_CH1_SW2_V     0x00 
#define BBRX_1x_CH1_C1_V      0x00 
#define BBRX_1x_CH1_C2_V      0x30 
#define BBRX_1x_CH1_VREF_V    0x1B
#define BBRX_1x_CH1_GMC_V     0x00 
#define BBRX_1x_CH1_GMC_ADJ_V 0x00 
#define BBRX_1x_CH1_I1_V      0x55 
#define BBRX_1x_CH1_I2_V      0x15 
#define BBRX_1x_CH1_IBUF1_V   0x00 
#define BBRX_1x_CH1_IBUF2_V   0x00 
#define BBRX_1x_CH1_RSTR_V    0x00 
#define BBRX_1x_CH1_TMUX_V    0x00 
#define BBRX_1x_CH1_COMP_V    0xb4 

#define BBRX_1x_CH0_EN1_V     0x0d
#define BBRX_1x_CH0_EN2_V     0x18 
#define BBRX_1x_CH0_SW1_V     0x41 
#define BBRX_1x_CH0_SW2_V     0x00 
#define BBRX_1x_CH0_C1_V      0x00 
#define BBRX_1x_CH0_C2_V      0x30 
#define BBRX_1x_CH0_VREF_V    0x1B 
#define BBRX_1x_CH0_I1_V      0x55 
#define BBRX_1x_CH0_I2_V      0x15 
#define BBRX_1x_CH0_IBUF1_V   0x00
#define BBRX_1x_CH0_IBUF2_V   0x00 
#define BBRX_1x_CH0_RSTR_V    0x00 
#define BBRX_1x_CH0_TMUX_V    0x00 
#define BBRX_1x_CH0_COMP_V    0xb4 
#define BBRX_1x_CHO_R1_V      0x00 
#define BBRX_1x_CHO_R2_V      0x00 
#define BBRX_1x_CH1_R1_V      0x00 
#define BBRX_1x_CH1_R2_V      0x00 
#define BBRX_1x_RESV_EN_V     0x00 
#define BBRX_1x_RESV_1_V      0x00 
#define BBRX_1x_RESV_2_V      0x00 
#define BBRX_1x_RESV_3_V      0x00 
#define BBRX_1x_RESV_4_V      0x00 
			


/****************************************************************************/
/*                          GPS   VALUES                                    */
/****************************************************************************/



#define BBRX_GPS_GPR_A1_V     0x00
#define BBRX_GPR_A2_V         0x00 
#define BBRX_DDO_A1_V         0x00 
#define BBRX_DDO_A2_V         0x00 
#define BBRX_DDO_A3_V         0x00 
#define BBRX_DDI_A1_V         0x00 
#define BBRX_GPS_CH1_EN1_V     0x0d 
#define BBRX_GPS_CH1_EN2_V     0x18 
#define BBRX_GPS_CH1_SW1_V     0x41 
#define BBRX_GPS_CH1_SW2_V     0x00 
#define BBRX_GPS_CH1_C1_V      0x00 
#define BBRX_GPS_CH1_C2_V      0x30 
#define BBRX_GPS_CH1_VREF_V    0x1b
#define BBRX_GPS_CH1_GMC_V     0x00 
#define BBRX_GPS_CH1_GMC_ADJ_V 0x00 
#define BBRX_GPS_CH1_I1_V      0x55 
#define BBRX_GPS_CH1_I2_V      0x15 
#define BBRX_GPS_CH1_IBUF1_V   0x00 
#define BBRX_GPS_CH1_IBUF2_V   0x00 
#define BBRX_GPS_CH1_RSTR_V    0x00 
#define BBRX_GPS_CH1_TMUX_V    0x00 
#define BBRX_GPS_CH1_COMP_V    0xb4 
#define BBRX_GPS_CH0_EN1_V     0x0d 
#define BBRX_GPS_CH0_EN2_V     0x18 
#define BBRX_GPS_CH0_SW1_V     0x41 
#define BBRX_GPS_CH0_SW2_V     0x00 
#define BBRX_GPS_CH0_C1_V      0x00 
#define BBRX_GPS_CH0_C2_V      0x30 
#define BBRX_GPS_CH0_VREF_V    0x1b 
#define BBRX_GPS_CH0_I1_V      0x55 
#define BBRX_GPS_CH0_I2_V      0x15 
#define BBRX_GPS_CH0_IBUF1_V   0x00
#define BBRX_GPS_CH0_IBUF2_V   0x00 
#define BBRX_GPS_CH0_RSTR_V    0x00 
#define BBRX_GPS_CH0_TMUX_V    0x00 
#define BBRX_GPS_CH0_COMP_V    0xb4 
#define BBRX_GPS_CHO_R1_V      0x00 
#define BBRX_GPS_CHO_R2_V      0x00 
#define BBRX_GPS_CH1_R1_V      0x00 
#define BBRX_GPS_CH1_R2_V      0x00 
#define BBRX_GPS_RESV_EN_V     0x00 
#define BBRX_GPS_RESV_1_V      0x00 
#define BBRX_GPS_RESV_2_V      0x00 
#define BBRX_GPS_RESV_3_V      0x00 
#define BBRX_GPS_RESV_4_V      0x00 
			



#ifdef FEATURE_DIFFERENTIAL_RX_MOD_CLK
#define BBRX_CH0_CK_V       0x60
#else
#define BBRX_CH0_CK_V       0x20
#endif

#define BBRX_CH0_GMC_V      0x40
#define BBRX_CH0_GMC_ADJ_V  0x50
#define BBRX_CH0_OTAI_V     0x55
#define BBRX_CH0_OTAI2_V    0x01
#define BBRX_CH0_RSTR_V     0x04
#define BBRX_CH0_TMUX_V     0x00
#define BBRX_CH0_R1_V       0x84
#define BBRX_CH0_R2_V       0x00

#ifdef FEATURE_CLKREGIM_ADIE_CH1_CLK_GSM
#error code not present
#else
#define BBRX_CH1_PD1_WCDMA_V           0x1F
#endif

#define BBRX_CH1_PD1_V      0x1E
#define BBRX_CH1_PD2_V      0x18
#define BBRX_CH1_SW_V       0x09
#define BBRX_CH1_C_V        0x38
#define BBRX_CH1_C2_V       0x12

#ifdef FEATURE_DIFFERENTIAL_RX_MOD_CLK
#define BBRX_CH1_CK_V       0x60
#else
#define BBRX_CH1_CK_V       0x20
#endif

#define BBRX_CH1_GMC_V      0x40
#define BBRX_CH1_GMC_ADJ_V  0x50
#define BBRX_CH1_OTAI_V     0x55
#define BBRX_CH1_OTAI2_V    0x01
#define BBRX_CH1_RSTR_V     0x04
#define BBRX_CH1_TMUX_V     0x00
#define BBRX_CH1_R1_V       0x84
#define BBRX_CH1_R2_V       0x00
#define BBRX_UNDEFINED_V    0xff
#define BBRX_CH0_PD1_GPS_V     0x02
#define BBRX_CH0_PD2_GPS_V     0x00
#define BBRX_CH0_SW_GPS_V      0x07
#define BBRX_CH0_C_GPS_V       0x28
#define BBRX_CH0_C2_GPS_V      0x13
/* CH0_CK_GPS: 0x20 if select Single Ended in ANALOG_CLKX_SEL */
/*             0x60 if selected Diff clk   in ANALOG_CLKX_SEL */
#define BBRX_CH0_CK_GPS_V      0x20
#define BBRX_CH0_DC_GPS_V      0x00
#define BBRX_CH0_GMC_GPS_V     0x49
#define BBRX_CH0_GMC_ADJ_GPS_V 0x50
#define BBRX_CH0_OTAI_GPS_V    0x55
#define BBRX_CH0_OTAI2_GPS_V   0x01
#define BBRX_CH0_RSTR_GPS_V    0x04
#define BBRX_CH0_TMUX_GPS_V    0x00
#define BBRX_CH0_R1_GPS_V      0x88
#define BBRX_CH0_R2_GPS_V      0x00
#define BBRX_CH1_PD1_GPS_V     0x02
#define BBRX_CH1_PD2_GPS_V     0x00
#define BBRX_CH1_SW_GPS_V      0x07
#define BBRX_CH1_C_GPS_V       0x28
#define BBRX_CH1_C2_GPS_V      0x13
#define BBRX_CH1_CK_GPS_V      0x20
#define BBRX_CH1_DC_GPS_V      0x00
#define BBRX_CH1_GMC_GPS_V     0x49
#define BBRX_CH1_GMC_ADJ_GPS_V 0x50
#define BBRX_CH1_OTAI_GPS_V    0x55
#define BBRX_CH1_OTAI2_GPS_V   0x01
#define BBRX_CH1_RSTR_GPS_V    0x04
#define BBRX_CH1_TMUX_GPS_V    0x00
#define BBRX_CH1_R1_GPS_V      0x88
#define BBRX_CH1_R2_GPS_V      0x00
/* BBRX Settings for GPS End */

#else /* Quasar version */
#error code not present
#endif  /* FEATURE_QUASAR_RF */


#ifdef FEATURE_GSM
#error code not present
#endif


/* -------------------------------------------------------------------------

                            EXPORTED DATA TYPES

---------------------------------------------------------------------------- */

/*
*/
/*===========================================================================

FUNCTION BBRX_1x_INIT                                      EXTERNALIZED FUNCTION

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void bbrx_1x_init( void );



/*===========================================================================

FUNCTION      BBRX_1x_SET_MODE                             EXTERNALIZED FUNCTION

DESCRIPTION

  This function sets the registers to the appropriate value when 
  switching to different systems

PARAMETERS

  adie_system_mode is an enumerated value that denotes the desired 
  system to switch to.

DEPENDENCIES

  sbi_init() must have been called. (for sbi communication targets)
  adie_init() must have been called.


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean bbrx_1x_set_mode (bbrx_rf_path_type path, bbrx_mode_type mode);

#endif /* BBRX_6500_H */

