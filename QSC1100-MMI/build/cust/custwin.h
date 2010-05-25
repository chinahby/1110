#ifndef CUSTQ1_H
#define CUSTQ1_H
/* ========================================================================
FILE: CUSTQ1

Copyright (c) 2001-2008 by QUALCOMM Incorporated.  All Rights Reserved.        
=========================================================================== */

#ifndef TARGSBD_H
   #include "targsbd.h"
#endif

#define FEATURE_RECORDER
#define FEATURE_HAS_CARD0
#define FEATURE_CALL_RECORDER

#if defined( FEATURE_RECORDER)
#define FEATURE_QCP
#define FEATURE_SOUND_DEVICE_HEADSET_SPEAKER
#endif

//#define FEATURE_MSG_OFF 
//#define FEATURE_MSG_LOW_MEMORY_USAGE
//#define FEATURE_NSRW_FLASH 
#define CUST_MOB_MODEL 237
#define FEATURE_I2C 
#define FEATURE_IS2000_REL_A_UNDEFINED 
#define FEATURE_MULTIMODE_ARCH 
#define FEATURE_BUILD_USB

#ifndef WIN32
#define FEATURE_WMS_APP 
#endif//WIN32

#define FEATURE_MDSP_TASK 
#define FEATURE_DBM 
#define FEATURE_ERR_LCD 
#define FEATURE_HYBRID_HDR 
#define FEATURE_IS2000_SLOTTED_TIMER 
#define FEATURE_IS2000_REV_FCH_GATING 
#define FEATURE_AUTO_VOICE_CALL_DIALER 
#define FEATURE_SD20 
#define FEATURE_FTM_MC 
#define FEATURE_TSG_EXT 
#define FEATURE_USE_TIME_VU 
#define EFS_PAGE_SIZE 512
#define FEATURE_QDSP_SET_IMAGE_HW_RESOURCES 
#define FEATURE_VOC_PACKET_INTERFACE 
#define FEATURE_IPL_EFX_COMPACT 
#define FEATURE_MEID_SUPPORT 
#define FEATURE_ADC_MULTI_RESOLUTION 
#define FEATURE_VOC_4GV 
#define FEATURE_TIMER_TASK 
#define DO_READ_WITH_MEMMOVE 
#define FEATURE_FS_DIAG 
#define FEATURE_XO 
#define T_QSC6020 
#define FEATURE_ZZ2 
#define FEATURE_IPL_LITE 
#define FEATURE_QCT_JPEG_DECODER 
#define FEATURE_JPEG_DECODER 
//remove for ROM Space saving.
/*
#define FEATURE_AAC 
#define FEATURE_ENHANCED_AAC_PLUS 
#define FEATURE_AAC_PLUS 
#define FEATURE_AUDFMT_EQUALIZER 
#define FEATURE_MP4_DECODER 
*/

#define FEATURE_APP_MANAGER 
#define FEATURE_STATIC_RAPTOR 
#define RAPTOR_UMTS_DDA 
#define CLKRGM_MCLK_IS_96MHZ 
#define FLASH_NOR_EFS2_START_BYTE_OFFSET 0x00F00000
#define FLASH_NOR_EFS2_END_BYTE_OFFSET (0x01000000-4*32*1024 -1)
#define FLASH_NOR_EFS2_SECTOR_BYTE_SIZE 0x00020000
#define FEATURE_DYNAMIC_OEM_HEAP 
#define FEATURE_ENABLE_SLEEP 
//#define FEATURE_NO_WMS_APP
#define FTM_HAS_AUDIO 
#define FEATURE_UI_STRING_MAPS_ACTIVE 
#define CM_STRING_MAPS_ACTIVE 
#define FEATURE_NO_13K_VOICE_SUPPORT 
#define FEATURE_RUNTIME_DEVMAP 
//remove for ROM Space saving.
//#define FEATURE_EFS_FTL 
#define FEATURE_AUDIO_CONFIGURATION_6020_STANDARD 
#define FEATURE_RUIM 
#define FEATURE_OMH_SMS 
#define FEATURE_UIM1 
#define FEATURE_AUTH 
#define FEATURE_AUTH_DIGITS 
//delete for supporting our UTK #define FEATURE_GSTK 
#define FEATURE_UIM_TOOLKIT 
//#define FEATURE_APP_CATAPP 
//#define FEATURE_APP_CAT 
#define FEATURE_DH_EXP 
#define FEATURE_UIM_PMIC_ON_UIM1 
#define FEATURE_BUILD_CDMA_CELL 
#define FEATURE_RF_ZIF 
#define FEATURE_AUX_SBI 
#define FEATURE_MDSP_TASK 
#define FEATURE_EFS 
#define FEATURE_IS95B_MDR 
#define FEATURE_DS 
#define FEATURE_ONCRPC_SIO 
#define FEATURE_SEC_SSL_DIAG_EVENTS 
#define FEATURE_SECTEST_DIAG_EVENTS 
#define FEATURE_SEC_SSL_LOGGING 
#define FEATURE_EFS2 
#define FEATURE_EFS2_DELAYED_TRUNC 
//#define FEATURE_TOUCHPAD
#ifdef FEATURE_SEC_DRM
   #undef FEATURE_SEC_DRM
#endif

#include "custsec.h"
#include "custdiag.h"
#include "custrex.h"
#include "custsurf.h"
#include "custsio.h"
#include "custcmx.h"
#include "custavs.h"
#include "custnvm.h"
#include "custrf.h"
#include "custefs.h"
#include "custcm.h"
#include "custbmp.h"
#include "custui.h"
#include "custwms.h"
#include "custdmss.h"
#include "custsrch.h"
#include "custrela.h"
#include "custcdma.h"
#include "custmp4.h"
#include "custjpeg.h"
#include "custruim.h"
#include "custdata.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
   #undef FEATURE_APP_TEST_AUTOMATION
#endif
#ifdef FEATURE_TRIMODE_ITEMS
   #undef FEATURE_TRIMODE_ITEMS
#endif
#ifdef FEATURE_SPLIT_SCAN_LINES
   #undef FEATURE_SPLIT_SCAN_LINES
#endif
#ifdef FEATURE_SYSTEM_ARBITRATION
   #undef FEATURE_SYSTEM_ARBITRATION
#endif
#ifdef FEATURE_IS2000_RELA
   #undef FEATURE_IS2000_RELA
#endif
#ifdef FEATURE_AUTOBAUD
   #undef FEATURE_AUTOBAUD
#endif
#ifdef RF_ZIF_LIBRARY_INTERWORK
   #undef RF_ZIF_LIBRARY_INTERWORK
#endif
#ifdef FEATURE_MOBIMON_TEST_AUTOMATION
   #undef FEATURE_MOBIMON_TEST_AUTOMATION
#endif
#ifdef FEATURE_ADIE_HAS_MM_ADC
   #undef FEATURE_ADIE_HAS_MM_ADC
#endif
#ifdef FEATURE_GUNIMON_TEST_AUTOMATION
   #undef FEATURE_GUNIMON_TEST_AUTOMATION
#endif
#ifdef FEATURE_MEDIAPLAYER_TEST_AUTOMATION
   #undef FEATURE_MEDIAPLAYER_TEST_AUTOMATION
#endif
#ifdef FEATURE_PNG_ENCODER
   #undef FEATURE_PNG_ENCODER
#endif
#ifdef FEATURE_JPEG_ENCODER
   #undef FEATURE_JPEG_ENCODER
#endif
#ifdef FEATURE_SECOND_UART
   #undef FEATURE_SECOND_UART
#endif

//support spcieal target at model level.
#define FEATURE_Q1_SPECIAL_TARGET
#define OEM_STATIC_HEAP_SIZE 0x00200000

#define RF_HAS_HDET_BYPASS

#ifdef FEATURE_BACKUP_CRITICAL_NV_TO_FLASH
//Define the backup area parameters
//If we locate nv backup to code area, we need to lock interrupt and disable mmu data catch during flash operations
#ifdef MAP_BACKUP_TO_CODE_AREA
#define CRITICAL_NV_BACKUP_ADDR             0x18000
#define CRITICAL_NV_BACKUP_SIZE             0x08000
#else
#define CRITICAL_NV_BACKUP_ADDR             (FLASH_NOR_EFS2_END_BYTE_OFFSET + 1)
#define CRITICAL_NV_BACKUP_SIZE             (T_NOR_FLASH_SIZE - CRITICAL_NV_BACKUP_ADDR)
#endif
#endif//FEATURE_BACKUP_CRITICAL_NV_TO_FLASH

//#ifndef WIN32//wlh 临时修改
#define FEATRUE_SUPPORT_G_SENSOR
//#endif//WIN32
#define FEATURE_SUPPORT_BT_BCM

#ifndef WIN32
#define FEATURE_SUPPORT_VC0848
#endif//WIN32
#define FEATURE_SUPPORT_TORCH

//#define FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK

#define FEATURE_ONEMSG_USE_ONEFILE
//#define FEATURE_CDSMS_CACHE_USELIST

#define FEATURE_COLOR_DISPLAY

#define FIX_LINKITEM_INITBUG
//wlh 注意这里
//#undef AEE_SIMULATOR

#define FEATURE_UTK
#define FEATURE_FM_RADIO

#define FEATURE_CARRIER_CHINA_TELCOM
#define FEATURE_LANG_ENGLISH
#define FEATURE_LANG_CHINESE
//#define FEATURE_CHARGER_OVER_VOLTAGE_CONTROL
#define FEATURE_APP_NUMBERMANAGER
#define FEATURE_SPN_FROM_BSMCCMNC

#ifdef FEATURE_STEREO_DAC
   #undef FEATURE_STEREO_DAC
#endif

#ifdef FEATURE_APP_MPEG4
   #undef FEATURE_APP_MPEG4
#endif

#ifdef FEATURE_BREW_IMOVIEFILE
   #undef FEATURE_BREW_IMOVIEFILE
#endif

#ifdef FEATURE_EFS_FTL
#undef FEATURE_EFS_FTL
#endif

#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
#undef FEATURE_MMGSDI_TEST_AUTOMATION
#endif

#ifdef FEATURE_MMGSDI_PERSONALIZATION
#undef FEATURE_MMGSDI_PERSONALIZATION
#endif

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#undef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#endif

#ifdef FEATURE_PERSO_RUIM
#undef FEATURE_PERSO_RUIM
#endif

#endif /* CUSTSBD_H */
