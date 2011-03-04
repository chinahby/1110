/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  T L M M     F O R     Q S C 1 1 0 0

GENERAL DESCRIPTION
  This module provides routines to modify the TLMM after the boothw module
  has initialized it to a known startup configuration.

EXTERNALIZED FUNCTIONS
    tlmm_init()
    Initialize TLMM registers.
    tlmm_write ()
    Modify TLMM registers.


Copyright (c) 2001 - 2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tlmm/tlmm_1100.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/09   hvo     Featurized SDCC, Camera as low tier does not support them
12/13/07   hvo     Fixed compiler errors
12/03/07   hvo     Added I2C supported on GSBI2
07/02/07   vph     Ported for QSC1100 from QSC60X5
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "comdef.h"
#include "boothw.h"
#include BOOTHWI_MSM_H
#include "tlmm.h"
#include "err.h"

#include "msm_help.h"

#define DEFAULT_EBI2_CS0_CFG1 0x83030020

void tlmm_gsbi_wrap_proto_init(void);

/* Start with FFA platform */
tlmm_hw_platform_type  tlmm_platform = TLMM_FFA;

/*===========================================================================

  FUNCTION tlmm_detect_platform()

  DESCRIPTION
    This function will read one of the FPGA registers, such as Daughter
    Card ID.

    This function reads one the FPGA registers such as Daughter Card ID. 
    For example, if read's result is one of values indicates RICO/LIGER/TRESTLES
    then it has detected a SURF. If read's result is nothing, then it is FFA.

  PARAMETERS
    None.

  DEPENDENCIES
    Boot loader need to call this function.
    SURF always has FPGA, and FFA never has FPGA
    FFA shall not lockup when issue a read to the FPGA address range
    SURF shall not change the FPGA address and the offset of Daughter Card
    ID address and its identification value

  RETURN VALUE
    FFA or SURF.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL tlmm_hw_platform_type tlmm_detect_platform(void)
{
  byte temp = 0;

  /* Default setting for EBI2_CS0 as LCD.  In order to read FPGA version,
   * we need to switch EBI2_CS0 to FGPA, and restore it backs once we are done
   */
  /* Temporary store EBI2_CS0_CFG1 setting for switching back later */
  dword temp_ebi2_cfg = HWIO_INI(EBI2_CSn_CFG1, 0);

  /* Enable FPGA for reading */
  HWIO_OUTI(EBI2_CSn_CFG1, 0, DEFAULT_EBI2_CS0_CFG1);
  gpio_out(LCD_FLAG,1);

  /* Read FPGA daughter card version */
  temp = inp(TLMM_SURF_FPGA_BASE_ADDR);

  /* Restore EBI2_CS0_CFG1 to LCD */
  HWIO_OUTI(EBI2_CSn_CFG1, 0, temp_ebi2_cfg);
  gpio_out(LCD_FLAG,0);

  if( (temp == TLMM_QSC11X0_RICO_DC) || 
      (temp == TLMM_QSC11X0_LIGER_DC) ||
      (temp == TLMM_QSC11X0_TRESTLES_DC) ) 
  {
    return TLMM_SURF;
  }
  else
  {
    return TLMM_FFA;
  }
}


/*===========================================================================

  FUNCTION tlmm_init()

  DESCRIPTION
    Initializes the TLMM shadow registers.  This is a duplicate of the code
    that is executed in boothw.c. Since boothw has no RAM during
    initialization, duplication here allows a ram shadow of the TLMM
    registers and allows tlmm_write() operations later.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
===========================================================================*/
void tlmm_init()
{
  uint32 fmask;
  uint32 value;

  /* Initialize shadow registers to match settings set above */
  gpio_tlmm_shadow();

#ifndef BUILD_BOOT_CHAIN
  tlmm_gsbi_wrap_proto_init();
#endif

  tlmm_platform = tlmm_detect_platform();

#ifdef FEATURE_DRV_SDCC
  /* Set drive strength of 10mA for SDCC */  
  fmask = HWIO_FMSK (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_24) |
              HWIO_FMSK (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_25) | 
              HWIO_FMSK (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_26) |
              HWIO_FMSK (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_27) |
              HWIO_FMSK (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_28) |
              HWIO_FMSK (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_29);

  value = (HWIO_FVAL (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_24, 6_MA) <<
           HWIO_SHFT (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_24)) |  
          (HWIO_FVAL (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_25, 6_MA) <<
           HWIO_SHFT (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_25)) |  
          (HWIO_FVAL (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_26, 6_MA) <<
           HWIO_SHFT (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_26)) |  
          (HWIO_FVAL (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_27, 6_MA) <<
           HWIO_SHFT (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_27)) |  
          (HWIO_FVAL (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_28, 6_MA) <<
           HWIO_SHFT (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_28)) |  
          (HWIO_FVAL (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_29, 6_MA) <<
           HWIO_SHFT (GPIO_PAD_HDRIVE_MSEL_3, HDRIVE_GPIO_29));

  HWIO_OUTM (GPIO_PAD_HDRIVE_MSEL_3, fmask, value);
#endif
}

/*
  This is the default configuration for the GPIOs, given a
  particular feature spec.  This table will be used at bootup
  to configure the GPIO pins before DMSS starts.
*/

const GPIO_SignalType  TLMM_GPIO_CONFIG[GPIO_NUM_GPIOS] =
{
  GPIO_0_SIGNAL,   GPIO_1_SIGNAL,   GPIO_2_SIGNAL,   GPIO_3_SIGNAL,   GPIO_4_SIGNAL,
  GPIO_5_SIGNAL,   GPIO_6_SIGNAL,   GPIO_7_SIGNAL,   GPIO_8_SIGNAL,   GPIO_9_SIGNAL,
  GPIO_10_SIGNAL,  GPIO_11_SIGNAL,  GPIO_12_SIGNAL,  GPIO_13_SIGNAL,  GPIO_14_SIGNAL,
  GPIO_15_SIGNAL,  GPIO_16_SIGNAL,  GPIO_17_SIGNAL,  GPIO_18_SIGNAL,  GPIO_19_SIGNAL,
  GPIO_20_SIGNAL,  GPIO_21_SIGNAL,  GPIO_22_SIGNAL,  GPIO_23_SIGNAL,  GPIO_24_SIGNAL,
  GPIO_25_SIGNAL,  GPIO_26_SIGNAL,  GPIO_27_SIGNAL,  GPIO_28_SIGNAL,  GPIO_29_SIGNAL,
  GPIO_30_SIGNAL,  GPIO_31_SIGNAL,  GPIO_32_SIGNAL,  GPIO_33_SIGNAL,  GPIO_34_SIGNAL,
  GPIO_35_SIGNAL,  GPIO_36_SIGNAL,  GPIO_37_SIGNAL,  GPIO_38_SIGNAL,  GPIO_39_SIGNAL,
  GPIO_40_SIGNAL,  GPIO_41_SIGNAL,  GPIO_42_SIGNAL,  GPIO_43_SIGNAL,  GPIO_44_SIGNAL,
  GPIO_45_SIGNAL,  GPIO_46_SIGNAL,  GPIO_47_SIGNAL,  GPIO_48_SIGNAL,  GPIO_49_SIGNAL,
  GPIO_50_SIGNAL,  GPIO_51_SIGNAL,  GPIO_52_SIGNAL,  GPIO_53_SIGNAL,  GPIO_54_SIGNAL,
  GPIO_55_SIGNAL,  GPIO_56_SIGNAL,  GPIO_57_SIGNAL,  GPIO_58_SIGNAL,  GPIO_59_SIGNAL,
  GPIO_60_SIGNAL,  GPIO_61_SIGNAL,  GPIO_62_SIGNAL
};

#ifndef BUILD_BOOT_CHAIN
/*===========================================================================

  FUNCTION tlmm_gsbi_wrap_proto_init()

  DESCRIPTION
     Initialize the GSBI_WRAP SERIAL INTERFACE PROTOCOL

  PARAMETERS
    None.

  DEPENDENCIES
    Need to enable clock.
    RUIM must use GSBI0 because of different voltage requirement
  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
===========================================================================*/
void tlmm_gsbi_wrap_proto_init(void)
{
  /* There are three separate GSBI controller wrapped inside the GSBI_WRAP.
   * Each GSBI can support one of the serial protocols : I2C, UART, or SPI.
   * The following code won't configure all possible combination serial
   * protocol usage. 
   * Config1: GSBI0=RUIM, GSBI1=UART,      GSBI2=AUDIO  
   * Config2: GSBI0=RUIM, GSBI1=BLUETOOTH, GSBI2=AUDIO  
   * Config3: GSBI0=UART, GSBI1=BLUETOOTH, GSBI2=AUDIO  
   * Config4: GSBI0=RUIM, GSBI1=UART,      GSBI2=I2C
   * Config5: GSBI0=RUIM, GSBI1=SPI,       GSBI2=I2C
   */
#if defined(FEATURE_GSBI0_RUIM) && !defined(FEATURE_GSBI0_UART) \
                                && !defined(FEATURE_GSBI0_SPI) \
                                && !defined(FEATURE_GSBI0_AUDIO) \
                                && !defined(FEATURE_GSBI0_I2C)
  HWIO_OUT(GSBI0_GSBI_CTRL_REG,
           HWIO_FVAL(GSBI0_GSBI_CTRL_REG,PROTOCOL_CODE, R_UIM)
              << HWIO_SHFT(GSBI0_GSBI_CTRL_REG,PROTOCOL_CODE));

#elif defined(FEATURE_GSBI0_UART) && !defined(FEATURE_GSBI0_RUIM) \
                                  && !defined(FEATURE_GSBI0_AUDIO) \
                                  && !defined(FEATURE_GSBI0_I2C) \
                                  && !defined(FEATURE_GSBI0_SPI) 
  HWIO_OUT(GSBI0_GSBI_CTRL_REG,
		   HWIO_FVAL(GSBI0_GSBI_CTRL_REG,PROTOCOL_CODE,UART)
			  << HWIO_SHFT(GSBI0_GSBI_CTRL_REG,PROTOCOL_CODE));
#else
  // IDLE
  HWIO_OUT(GSBI0_GSBI_CTRL_REG,~HWIO_RMSK(GSBI0_GSBI_CTRL_REG));
#endif

#if defined(FEATURE_GSBI1_UART) && !defined(FEATURE_GSBI1_SPI) \
                                 && !defined(FEATURE_GSBI1_AUDIO)\
                                 && !defined(FEATURE_GSBI1_I2C) \
								 && !defined(FEATURE_GSBI1_RUIM)
  HWIO_OUT(GSBI1_GSBI_CTRL_REG,
      HWIO_FVAL(GSBI1_GSBI_CTRL_REG,PROTOCOL_CODE,UART)
        << HWIO_SHFT(GSBI1_GSBI_CTRL_REG,PROTOCOL_CODE) );
#elif defined(FEATURE_GSBI1_SPI) && !defined(FEATURE_GSBI1_UART) \
                                 && !defined(FEATURE_GSBI1_AUDIO) \
                                 && !defined(FEATURE_GSBI1_I2C) \
                                 && !defined(FEATURE_GSBI1_RUIM)
  HWIO_OUT(GSBI1_GSBI_CTRL_REG,
      HWIO_FVAL(GSBI1_GSBI_CTRL_REG,PROTOCOL_CODE,SPI)
        << HWIO_SHFT(GSBI1_GSBI_CTRL_REG,PROTOCOL_CODE));
  // need to concern about extra chip select for spi
#else
  /* IDLE */
  HWIO_OUT(GSBI1_GSBI_CTRL_REG,~HWIO_RMSK(GSBI1_GSBI_CTRL_REG));
#endif

/* The use of each protocol is predefined */
#if defined(FEATURE_GSBI2_AUDIO) && !defined(FEATURE_GSBI2_UART) \
                                 && !defined(FEATURE_GSBI2_SPI) \
								 && !defined(FEATURE_GSBI2_I2C) \
                                 && !defined(FEATURE_GSBI2_RUIM) 
  HWIO_OUT(GSBI2_GSBI_CTRL_REG,
      HWIO_FVAL(GSBI2_GSBI_CTRL_REG,PROTOCOL_CODE,AUDIO)
        << HWIO_SHFT(GSBI2_GSBI_CTRL_REG,PROTOCOL_CODE));
#elif defined(FEATURE_GSBI2_I2C) && !defined(FEATURE_GSBI2_AUDIO) \
                                 && !defined(FEATURE_GSBI2_UART) \
								 && !defined(FEATURE_GSBI2_SPI) \
                                 && !defined(FEATURE_GSBI2_RUIM) 
  HWIO_OUT(GSBI2_GSBI_CTRL_REG,
	  HWIO_FVAL(GSBI2_GSBI_CTRL_REG,PROTOCOL_CODE,I2CC)
		<< HWIO_SHFT(GSBI2_GSBI_CTRL_REG,PROTOCOL_CODE));
#else
  /* IDLE */
  HWIO_OUT(GSBI2_GSBI_CTRL_REG,~HWIO_RMSK(GSBI2_GSBI_CTRL_REG));
#endif
}
#endif
