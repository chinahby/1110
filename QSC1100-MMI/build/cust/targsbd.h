#ifndef TARGSBD_H
#define TARGSBD_H
/* ========================================================================
FILE: TARGSBD

Copyright (c) 2001-2008 by QUALCOMM Incorporated.  All Rights Reserved.        
=========================================================================== */

#define T_MSM60X0A

#define T_SURF 
#define FEATURE_RDEVMAP_DIAG_DEFAULT_TO_USB 
#define T_MB_SURF_6100 
#define T_MB_SURF_6050 
#define FEATURE_NV_HAS_MIN_RX_RSSI 
#define BIO_MB_H "bio_mb6100.h"
#define BSP_RF_H "bsp_rf_qsc60x0.h"
#define ZIF_MIN zrf6000_lib
#define ZIF_LIB zif_zrf6000
#define ADC_CHANNELS_H "adc_channels_surf60xx.h"
#define LEDS 0x20100000
#define FEATURE_KEYPAD_MULTI_KEY 
#define FEATURE_ETM 
#define FEATURE_MSMHWIO 
#define FEATURE_PLL_196P608_CPLL_146P7648 
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
#define T_MSM6100B 
#define T_MSM6100C 
#define T_MSM6100D 
#define T_MSM_6100 
#define T_QSC60X0 
#define T_QSC60X0_TEMP 
#define BOOTHW_MSM_H "boothw_60x0.h"
#define TRAMP_MSM_H "tramp_60x0.h"
#define TRAMPI_MSM_H "trampi_60x0.h"
#define CLKRGM_H "clkrgm_msm_60x0.h"
#define CLKRGM_MSM_H "clkrgm_msm_60x0.h"
#define CLKRGMI_MSM_H "clkrgmi_msm_60x0.h"
#define CLK_RGM_MODEM_H "clkrgm_modem.h"
#define CLK_RGMI_MODEM_H "clkrgmi_modem.h"
#define TLMM_MSM_H "tlmm_60x0.h"
#define GPIO_INT_MSM_H "gpio_int_60x0.h"
#define CPLD_MSM_H "cpld_6100.h"
#define ADIE_I_H "adie_sbi.h"
#define SBI_MSM_H "sbi_qsc60x0.h"
#define T_RF_ZRF6000HT 
#define T_UI_S510X 
#define HS_H "hs_mb6100.h"
#define FEATURE_MEMHEAP_MT 
#define FEATURE_MDSP_USES_MODULES 
#define FEATURE_CDMA1X 
#define FEATURE_MULTIMODE_RFAPI 
#define FEATURE_DIAG_FS_ACCESS_VALIDATION 
#define FEATURE_BREW 

#define FEATURE_OEMUI_TASK

#define FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS 
#define ADIE_HAS_ADC 
#define T_PMIC_PANORAMIX 
#define FEATURE_SPLIT_SCAN_LINES 
#define FEATURE_KEYPAD_OWNS_KEYPRESS_ISR 
#define FEATURE_MSM_LIBRARY 
#define FEATURE_SPEAKER_PHONE 
#define FEATURE_PMIC_SPEAKER 
#define FEATURE_ADC_CALIBRATION 
#define FEATURE_BATTERY_CHARGER 
#define FEATURE_VBATDET_CALIBRATION 
#define FEATURE_BATTERY_CHARGER_SINGLE_CHARGING_PATH 
#define FEATURE_BATTERY_CHARGER_NO_CHG_Q_THERMISTOR 
#define CHG_TARGET_H "chg_pm6650_config.h"
#define FEATURE_HEADSET_SWITCH 
#define FTM_HAS_CTON 
#define FTM_HAS_QFUSE 
#define FEATURE_QFUSE_PROGRAMMING 
//#define FEATURE_DISP_176X220
#define FEATURE_DISP_128X128 
#define FEATURE_BITMAP 
#define BOOT_MODE_NOR 
#define T_NOR_FLASH_SIZE 0x01000000
#define T_NOR_FLASH_IS_BURST_TYPE 
#define T_SRAM_SIZE 0x00800000
#define T_PSRAM_IS_BURST_TYPE 
#define DUAL_BUS 
#define SCL_APP_RAM_BASE 0x10000000
#define SCL_FRAME_BUF_BASE +0x0
#define SCL_DYNA_LOAD_APPS_BASE +0x0
#define SCL_ADDR_ZERO_MEM_SIZE T_NOR_FLASH_SIZE
#define T_RAM_SIZE T_SRAM_SIZE
#define T_RAM_BASE 0x10000000

//Newwind 20080910 add for support OEM Specail Target at platform level(Such as include some FFA feature to T_SURF)
#define FEATURE_OEM_SPECIAL_6020_TARGET
#define FEATURE_SCREEN_CAPTURE

#define FLASH_DEVICE_BASE 0x00000000
#define MAIN_APP_ADDRESS 0x20000

#define FEATRUE_AUTO_SET_NEED_NV_VALUE
#define FEATURE_BACKUP_CRITICAL_NV_TO_FLASH

#define FEATURE_SUPPORT_MASS_STORAGE_CDROM


#ifndef WIN32
#include "custpmic3.h"
#endif

#ifdef FEATURE_ENABLE_CHARGING_GAUGING
   #undef FEATURE_ENABLE_CHARGING_GAUGING
#endif
#ifdef FEATURE_LOW_BATT_DETECTION
   #undef FEATURE_LOW_BATT_DETECTION
#endif
#ifdef FEATURE_MANAGE_GPIOS_FOR_SLEEP
   #undef FEATURE_MANAGE_GPIOS_FOR_SLEEP
#endif
#ifdef ADC_TARGET_H
   #undef ADC_TARGET_H
#endif
#ifdef ADC_USES_ADIE
   #undef ADC_USES_ADIE
#endif
#ifdef FEATURE_LCD_18BPP_EMULATION
   #undef FEATURE_LCD_18BPP_EMULATION
#endif
#ifdef FEATURE_LCD_18BPP_EMULATION
   #undef FEATURE_LCD_18BPP_EMULATION
#endif


#endif /* TARGSBD_H */
