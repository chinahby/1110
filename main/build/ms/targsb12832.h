#ifndef TARGSBW2_H
#define TARGSBW2_H
/* ========================================================================
FILE: TARGSBW2

Copyright (c) 2001-2010 by QUALCOMM Incorporated.  All Rights Reserved.        
=========================================================================== */

#define T_QSC1100

#if defined (FEATURE_VERSION_ESIA) //|| defined (FEATURE_VERSION_EC99)
#define FLASH_NOR_EFS2_START_BYTE_OFFSET 0x00C00000
#elif defined (FEATURE_VERSION_EC99) 
#define FLASH_NOR_EFS2_START_BYTE_OFFSET 0x00B00000
#elif defined(FEATURE_VERSION_K212_20D)
#define FLASH_NOR_EFS2_START_BYTE_OFFSET 0x00B00000
#else
#define FLASH_NOR_EFS2_START_BYTE_OFFSET 0x00A00000
#endif
#define FLASH_NOR_EFS2_END_BYTE_OFFSET (0x00FC0000-1)
#define FLASH_NOR_EFS2_SECTOR_BYTE_SIZE 0x20000
#define FLASH_BURST_NOR 
#define T_SURF 
#define FEATURE_I2C_CHIP_ENABLE 
//#define FEATURE_EXTENDED_KEYS 
#define ADC_CHANNELS_H "adc_channels_qsc1100.h"
#define FEATURE_SSBI_ONLY 
#define FEATURE_SPEAKER_PHONE 
#define FEATURE_PMIC_SPEAKER 
#define BIO_MB_H "bio_mb1100.h"
#define TLMM_MSM_H "tlmm_1100.h"
#define GPIO_H "gpio_1100.h"
#define LEDS GP1_BASE
#define FEATURE_NV_HAS_MIN_RX_RSSI 
#define FEATURE_KEYPAD_MULTI_KEY 
#define FEATURE_SLEEP_CLK_STABILITY 
#define FEATURE_IBATT 
#define FEATURE_KEYPAD_OWNS_KEYPRESS_ISR 
#define FEATURE_ADC_CALIBRATION 
#define FEATURE_TCXOMGR 
#define FEATURE_PIC_SPURIOUS_WORKAROUND 
#define FEATURE_PDM_BACKLIGHT 
#define FEATURE_2ND_LVL_INTR 
#define FEATURE_PMIC_MPP4_USED_AS_AMUX_INPUT 
#define FEATURE_60X5_DCVS 
#define CACHE_POLICY_WRITE_BACK 
#define INTEL_LCD_CONTROLLER 
#define T_DLOAD 1
#define T_SLEEP 1
#define T_MSM2 1
#define T_MSM2P 1
#define T_MSM23 1
#define T_MSM3 1
#define T_MSM31 1
#define T_MSM33 1
#define T_MSM5000 1
#define T_MSM5105 1
#define T_MSM_5100 1
#define T_MSM6050 
#define T_MSM_6050 
#define T_MSM6100 
#define T_MSM_6100 
#define T_MSM6100B 
#define T_MSM6500 
#define T_MSM6550 
#define T_MSM6550A 
#define T_MSM6550B 
#define T_MSM6700 
#define T_MSM6800 
#define T_MSM6800B 
#define T_QSC60X5 
#define T_QSC1100 
#define FEATURE_SATURN 
#define MSMHWIOREG_H "msmhwioreg.h"
#define MSMHWIOREGP_H "msmhwioreg_port.h"
#define TRAMP_MSM_H "tramp_msm.h"
#define TRAMPI_MSM_H "trampi_msm.h"
#define CLKRGM_H "clkrgm_msm.h"
#define CLKRGMI_H "clkrgmi_msm.h"
#define CLKRGM_MSM_H "clkrgm_msm.h"
#define CLKRGMI_MSM_H "clkrgmi_msm.h"
#define CLK_RGM_MODEM_H "clkrgm_modem.h"
#define CLK_RGMI_MODEM_H "clkrgmi_modem.h"
#define GPIO_INT_MSM_H "gpio_int_msm.h"
#define ADIE_I_H "adie_sbi.h"
#define TLMM_MSM_H "tlmm_1100.h"
#define GPIO_H "gpio_1100.h"
#define BOOTHW_MSM_H "boothw_60x5.h"
#define BOOTHWI_MSM_H "boothwi_60x5.h"
#define CPLD_MSM_H "cpld_1100.h"
#define SBI_MSM_H "sbi_qsc60x5.h"
#define HS_H "hs_mb6550.h"
#define ADIE_HAS_ADC 
#define T_UI_S510X 
#define FEATURE_MSMHWIO 
#define FEATURE_TIMER_TASK 
#define FEATURE_ERR_SERVICES 
#define SCL_MODEM_CODE_BASE 0x08100000
#define SCL_MODEM_TOTAL_SIZE 0x100000
#define SCL_HOSTDL_BASE 0x08050000
#define SCL_DBL_CODE_BASE_ADDR 0x58010000
#define SCL_FSBL_CODE_BASE_ADDR 0x08005000
#define SCL_OSBL_CODE_BASE_ADDR 0x08040000
#define SCL_OSBL_STACK_BASE_ADDR 0x58010000
#define SCL_PAGE_TABLE_BASE_ADDR 0x58018000
#define FEATURE_BOOT_DLOAD_SHARED_SEG 
#define MI_NOR_IMEM_ARMPRG_DLOAD_SHARE_ADDR 0x08050000
#define FEATURE_OSBL_SURF 
#define BOOT_MODE_NOR 
#define T_PSRAM_IS_BURST_TYPE 
#define T_SRAM_SIZE 0x400000
#define SCL_APP_RAM_BASE 0x08000000
#define SCL_FRAME_BUF_BASE +0x0
#define SCL_DYNA_LOAD_APPS_BASE +0x0
#define SCL_ADDR_ZERO_MEM_SIZE T_NOR_FLASH_SIZE
#define SCL_PMEM_DATA_BASE 0x83E0000
#define T_RAM_SIZE T_SRAM_SIZE
#define T_RAM_BASE 0x08000000
#define FEATURE_SHADOW_RAM_HEAP 
#define DO_READ_WITH_MEMMOVE 
#define FEATURE_EFS_SUPPORT_LARGE_NOR 





#endif /* TARGSBW2_H */
