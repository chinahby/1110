#ifndef OEMCFGI_H
#define OEMCFGI_H
/*===========================================================================

              O E M   C F G I

DESCRIPTION
  Definitions to support access to OEM configuration items.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   This file should not be included directly.  It is included by
   OEMConfig.h, which is included by AEEConfig.h


Copyright (c) 2001-2006     by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc60x0/apps/StaticExtensions/OEM/Inc/OEMCFGI.h#3 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/28/06   ypmw    Fixed LCD Display for SC1X
07/17/06   JD      Removed two config items, implemented through AEEConfig
04/15/04   PA      Added CFGI_LAST_RX_DATA_COUNT, CFGI_LAST_TX_DATA_COUNT,
                   CFGI_TOTAL_RX_DATA_COUNT and CFGI_TOTAL_TX_DATA_COUNT
04/14/04   SUN     Added new CFGI item CFGI_VR & CFGI_TTY
03/08/04   PMD     Added Support for WMS Application Mode and seperate
                   Voicemails for CDMA and GSM / WCDMA (GW)
02/14/04   PMD     Added SMS Retry Period Config Item
02/10/04   SUN     Added new CFGI item for CDMA/HDR diversity control
02/10/04   SUN     Added support for CFGI_AMR
01/28/04   RAM     Added new CFGI item for SpeakerPhone.
01/26/04   PMD     Added SMS Segmentation Message Reference
01/26/04   PMD     Added SMS Segmentation Settings
11/26/03   SUN     Added new CFGI item for CUG
11/23/03   SUN     Added new CFGI items for devices
11/05/03   RA      Added PA,MSM,PMIC,CHARGER,BATTERY thermistor readings
11/04/03   PMD     Added CDMA and GW Default Template Settings for SMS
10/31/03   PMD     Added IConfig Items for SMS Cell Broadcast Settings
                   Added IConfig Items for SMS Alert Settings
08/20/03   ADM     Added CFGI_LASTCALL_TIMER, CFGI_AIR_TIMER,CFGI_ROAM_TIMER.
                   CFGI_LIFE_TIMER to retrieve time information for the
                   RecentCalls app
08/19/03   SUN     Added Support for IMEI
08/05/03   PMD     Added Mode preference (CDMA / GW) NV Item for SMS to be
                   accessed using IConfig
09/16/02   sg      Added a number of CFGI items for Debug Applet
09/13/02   mjv     Added Dynamic R-UIM support
08/29/02   sg      Added CFGI_TIME_FORMAT
08/27/02   mjv     Added CFGI_FDN_ENABLED, CFGI_RECENT_MT_CALL_TIMER,
                   CFGI_RECENT_MO_CALL_TIMER, CFGI_MINUTE_ALERT,
                   CFGI_ANYKEY_ANSWER, and CFGI_HEADSET_AUTOANSWER.
                   Removed CFGI_LAST_CALL_TIMER.
08/08/02   sg      Featurized Smart Sound
07/30/02   mjv     Added CFGI_VOICEMAIL_NUMBER
07/19/02   mjv     Added CFGI_KEYGUARD_ENABLED
07/19/02   mjv     Added CFGI_OTKSLCODE
07/02/02   mjv     Added CFGI_IMSI_11_12, CFGI_IMSI_S, CFGI_MODE_PREF,
                   CFGI_AMPS_HOME_SID, CFGI_AMPS_FIRSTCHP, CFGI_AMPS_REG_TYPE,
                   CFGI_DATA_QNC_ENABLED, CFGI_DATA_DIALSTRING,
                   CFGI_SMS_IS41_WORKAROUND and CFGI_SMS_MO_ENCODING.
                   Renamed CFGI_COUNTRY_CODE to CFGI_IMSI_MCC.
                   Removed CFGI_MODEL_ID.
06/14/02   mjv     Added new CFGI items for the Service Programming applet,
                   added better documentation for existing items.
06/04/02   lnr     Fixed defects 384 & 402. Make the maximum SMS message
                   payload and the SMS MO Channel selection as configurable
                   items.
06/02/02   mjv     Removed a number of CFGI items that are not used.
                   Featurized CFGI_LOCKCODE with FEATURE_UIM.
05/29/02   mjv     Added CFGI_DEBUG_* config items
02/01/02   mjv     Removed CFGI_LAST_CALL.
12/05/01   jcb     Created file. Moved OEM AEEConfigItem's here.

===========================================================================*/

#include "BREWversion.h"

#if MIN_BREW_VERSIONEx(3,1,2)
#ifndef AEECONFIG_H
#include "AEEConfig.h"
//#error OEMCFGI.h must not be included directly, include AEEConfig.h instead
#endif
#else
#if !defined( OEMCONFIG_H) && !defined( AEE_SIMULATOR)
#error OEMCFGI.h must not be included directly, include OEMConfig.h instead
#endif
#endif

#include "OEMFeatures.h"
#if !defined( AEE_SIMULATOR)
#include "customer.h"
#endif

//-----------------------------------------------------------------------------
//  Includes
//-----------------------------------------------------------------------------

#include "oemnvint.h"  // include the config values

#ifdef CUST_OEM_NV_H
#include CUST_OEM_NV_H
#else
#ifndef WIN32
#error You must define CUST_OEM_NV_H in min file and contain corresponding header file in your build directory
#endif
#endif

#define BREW_USERNAME_LEN 32
#define BREW_PASSWORD_LEN 32


typedef byte keyToneLength;
#define MAX_EMERGENCY_NUM          10
#define MAX_EMERGENCY_NUM_LEN      10
#define FEATURE_CODE_MAX_LENTH     10
typedef PACKED struct// _Emergency_Number
{
   char  num_buf[MAX_EMERGENCY_NUM_LEN];
   int  num_len;
} Emergency_Number;

typedef PACKED struct //_EmergencyNum_Table
{
    Emergency_Number  emerg_num[MAX_EMERGENCY_NUM];
    int emert_size;
} EmergencyNum_Table;

// 信息发送模式
typedef enum Send_OPT_e_Type
{
    SENDOPT_NONE,           // 内部使用
    SENDOPT_SEND,           // 只发送
    SENDOPT_SAVE,           // 只保存
    SENDOPT_SAVEANDSEND     // 发送并保存
} Send_OPT_e_Type;

//-----------------------------------------------------------------------------
//  Types
//-----------------------------------------------------------------------------

enum /* OEM AEEConfigItem Values */
{
   //////////////////////////////////////////////////////////////////////
   // Setting Menu
   //////////////////////////////////////////////////////////////////////

   CFGI_SILENCEALL = CFGI_FIRST_OEM, // type=byte
#ifdef FEATURE_SET_AUTOKEYGUARD
   CFGI_AUTOKEYGUARD,                // type=byte
#endif
   CFGI_RECENT_MT_CALL_TIMER,        // type=uint32
   CFGI_RECENT_MO_CALL_TIMER,        // type=uint32
   CFGI_ALL_CALL_TIMER,              // type=uint32
   CFGI_BROWSER_CALL_TIMER,          // type=uint32
   CFGI_EXTPWR_BK_LIGHT,             // type=byte
   CFGI_HEADSET_RNG,                 // type=byte
   CFGI_ANYKEY_ANSWER,               // type=boolean
   CFGI_HEADSET_AUTOANSWER,          // type=byte
   CFGI_MISSED_CALL_ALERT,           // type=byte
   CFGI_ROAMING_ALERT,               // type=byte
   CFGI_SVC_ALERT,                   // type=byte
   CFGI_INTERNET_ALERT,              // type=byte
   CFGI_MINUTE_ALERT,                // type=byte
   CFGI_VOICE_PRIVACY,               // type=byte
#ifdef FEATRUE_AUTO_POWER
   CFGI_AUTO_POWER_ON,               //type = Auto_Power_Cfg
   CFGI_AUTO_POWER_OFF,              //type = Auto_Power_Cfg
#endif

   CFGI_POWERDOWN_ALARM,             //type =PowerDown_Alarm_Cfg
   CFGI_ALARM_FLAG,                         //type = boolean
#ifdef FEATURE_SPORTS_APP

	CFGI_SPORT_FLAG,
#endif	

#ifdef FEATRUE_SET_IP_NUMBER
   CFGI_IP_NUMBER,                   //type = IP_Number_Cfg
#endif
   CFGI_AUTO_REDIAL,                 //type = nv_auto_redial_type
   ////////////////////////////////////////////////////////////////
   // Sound Menu
   ////////////////////////////////////////////////////////////////

   CFGI_RINGER_VOL,                  // type=byte
   CFGI_EAR_VOL,                     // type=byte
   CFGI_BEEP_VOL,                    // type=byte
   CFGI_ALERT_TYPE,                  // type=byte
#ifdef FEATURE_SMART_SOUND
   CFGI_SMART_SOUND,                 // type=boolean
#endif
   CFGI_KEYBEEP_SOUND,               // type=byte
   CFGI_KEYTONE_LENGTH,              // type=keyToneLength

   // CFGI_RINGER_TYPE should only used by OEMRinger.c
   // The valid values are:
   // {SND_ALERT,SND_EFS_RINGER_1,SND_RING_[123456789],SND_ALERT_[12345]}
   CFGI_RINGER_TYPE,                // type=byte,
   CFGI_SMS_RINGER,                 // type=byte    短信铃声(0-表示无)
   CFGI_ALARM_RINGER,               //type = byte 闹钟铃声
   CFGI_SMS_RINGER_ID,              //type = byte 短信铃声
   CFGI_PROFILE_CUR_NUMBER,         //情景模式类型
   CFGI_PROFILE_ALERT_TYPE,         //来电提示
   CFGI_PROFILE_SMS_RINGER,         //短信提示
   CFGI_PROFILE_CALL_RINGER,        //来电铃声选择  
   CFGI_PROFILE_ALARM_RINGER,       //闹钟铃声选择
   CFGI_PROFILE_SMS_RINGER_ID,      //SMS音乐   
   CFGI_PROFILE_STARTUP_MUSIC,      //开机音乐
   CFGI_PROFILE_SHUTDOWN_MUSIC,     //关机音乐
   CFGI_PROFILE_POWERONOFF_ALERT,   //开关机音乐提示
   CFGI_PROFILE_MISSED_CALL_ALERT,  //短信铃声选择
   //CFGI_PROFILE_HEADSET_AUTOANSWER, //自动接听
   CFGI_PROFILE_KEYSND_TYPE,        //多彩按键音
   CFGI_PROFILE_KEYTONE_LENGTH,     //按键音长度  
   CFGI_PROFILE_RINGER_VOL,         //铃声音量
   CFGI_PROFILE_EAR_VOL,            //耳机音量
   CFGI_PROFILE_BEEP_VOL,           //键盘音量
   CFGI_KEYSND_TYPE,                //多彩按键音  
   // IMEI
   CFGI_IMEI,                       // type = oemnvi_imei_type
   CFGI_CUG,                        // type = OEMNVCugInfo
   CFGI_VR,                         // type = boolean

    //AMR
    CFGI_AMR,                       // type=boolean
    CFGI_TTY,                       // type = OEMTTY
    CFGI_LCD,                       // boolean, false = always on
    CFGI_LCD_TIMER,                 // display_duration type uint8

    ////////////////////////////////////////////////////////////////
    // Data Call Counters
    ////////////////////////////////////////////////////////////////

    CFGI_LAST_RX_DATA_COUNT,        // type=uint32
    CFGI_LAST_TX_DATA_COUNT,        // type=uint32
    CFGI_TOTAL_RX_DATA_COUNT,       // type=uint32
    CFGI_TOTAL_TX_DATA_COUNT,       // type=uint32

    //CDMA Rx Diversity Control
    CFGI_CDMA_RX_DIVERSITY_CTL,

    //HDR Rx Diversity Control
    CFGI_HDR_RX_DIVERSITY_CTL,

   ////////////////////////////////////////////////////////////////
   // Phone Number
   ////////////////////////////////////////////////////////////////

   CFGI_PHONE_NUMBER,                // type=AECHAR *, len <= OEMNV_PHONENUMBER_MAXLEN

   ////////////////////////////////////////////////////////////////
   // Display Menu
   ////////////////////////////////////////////////////////////////

   CFGI_CONTRAST_LVL,                // type=byte
   CFGI_BACK_LIGHT,                  // type=byte
   CFGI_LANGUAGE_SELECTION,          // type=byte
   CFGI_BANNER,                      // type=AECHAR *, size <= OEMNV_BANNER_MAXLEN
   CFGI_MENU_FORMAT,                 // type=byte
   CFGI_TIME_FORMAT,                 // type=byte
#ifdef FEATURE_TIME_DATA_SETTING
   CFGI_DATE_FORMAT,                 //type = byte
#endif 
   CFGI_SCREENSAVER_TIME,            //屏保时间
   CFGI_WALLPAPER,                   // 桌面墙纸
#ifdef FEATURE_ANIMATION_POWERUPDOWN
   CFGI_STARTUP_ANIMATION,           // 开机动画
   CFGI_POWEROFF_ANIMATION,          // 关机动画
#endif
   CFGI_BACKLIGHT_LEVEL,             // 背光亮度级别选择
   CFGI_DESKTOP_THEME,               // type=byte
#ifdef FEATURE_SCREEN_SAVE
   CFGI_SCREENSAVE_TYPE,             // 屏幕保护类型
#endif
#ifdef FEATURE_LED_CONTROL
   CFGI_LED_CONTROL,                 //type=byte
#endif
#ifdef FEATRUE_KEY_PAD_CTL
   CFGI_KEY_PAD_CTL,                 //type = Key_pad_Cfg
#endif
   ////////////////////////////////////////////////////////////////
   // SMS Applet
   ////////////////////////////////////////////////////////////////

   CFGI_SMS_SERVICE_OPTION,          // type=byte
   CFGI_SMS_TIMESTAMP,               // type=byte
   CFGI_SMS_IS41_WORKAROUND,         // type=byte
   CFGI_SMS_MO_ENCODING,             // type=byte
   CFGI_SMS_MODE,                    // type=byte
   CFGI_SMS_APP,                     // type=byte
   CFGI_SMS_CB_AUTO_DELETE,          // type=byte
   CFGI_SMS_CB_SILENT_ALERT,         // type=byte
   CFGI_SMS_CB_LANG_FILTER,          // type=byte
   CFGI_SMS_ALERT_DISPLAY,           // type=byte
   CFGI_SMS_ALERT_SOUND,             // type=byte
   CFGI_SMS_ALERT_VOLUME,            // type=byte
   CFGI_SMS_CDMA_DEFAULT_TEMPLATE,   // type=byte
   CFGI_SMS_GW_DEFAULT_TEMPLATE,     // type=byte
   CFGI_SMS_SEGMENT_SETTING,         // type=byte
   CFGI_SMS_SEGMENT_REF_NUM,         // type=uint16
   CFGI_SMS_RETRY_PERIOD,            // type=uint16
   CFGI_VOICEMAIL_NUMBER,            // type=AECHAR *,
                                     // size <= OEMNV_VOICEMAIL_MAXLEN
   CFGI_VOICEMAIL_NUMBER_CDMA,       // type=AECHAR *,
                                     // size <= OEMNV_VOICEMAIL_MAXLEN
   CFGI_VOICEMAIL_NUMBER_GW,         // type=AECHAR *,
                                     // size <= OEMNV_VOICEMAIL_MAXLEN
   CFGI_WMS_PRIORITY,                // type=byte
   CFGI_WMS_SENDMODE,                // type=byte
   CFGI_WMS_STORETYPE,               // type=byte
   CFGI_WMS_DELIVERYREPORTS,         // type= boolean
#ifdef FEATURE_AUTOREPLACE
   CFGI_WMS_AUTOREPLACE,             // type= boolean
#endif   
   CFGI_WMS_VALIDITYPERIOD,          // type=byte
   CFGI_WMS_MEMSTORE,                // type=wms_memory_store_e_type
   CFGI_WMS_RESVDMSGALERT_TIMEIOUT,  // type= boolean
   CFGI_WMS_CALLBACKNUMSWITCH,       // type= boolean
   CFGI_CALLBACKNUM,                 // type= AECHAR *
   CFGI_WMS_MO_CHANNEL_SELECT,       // type= byte
   
   /////////////////////////////////////////////////////////////////
   // Recent Calls App
   /////////////////////////////////////////////////////////////////

   CFGI_LASTCALL_TIMER,              // Last Call Time in seconds (type = uint32)
   CFGI_AIR_TIMER,                   // Total home air time in seconds (type = uint32)
   CFGI_ROAM_TIMER,                  // Total roaming time in seconds (type = uint32)
   CFGI_LIFE_TIMER,                  // Total time in seconds (type = uint32)

   ////////////////////////////////////////////////////////////////
   // Service Programming
   ////////////////////////////////////////////////////////////////

   CFGI_SECCODE,          // type=AECHAR *, len == OEMNV_SECCODE_LENGTH
#ifdef FEATURE_ENABLE_OTKSL
   CFGI_OTKSLCODE,        // type=AECHAR *, len == OEMNV_SECCODE_LENGTH
#endif
   CFGI_ESN,              // type=uint32 (read-only)
   CFGI_SLOTINDEX,        // type=byte
   CFGI_BUILD_VERSION,    // type=AECHAR * (ver_dir variable from mobile.h)
                          // read-only
   CFGI_RFCAL_VERSION,    // type=AECHAR *, len==OEMNV_RFCALVER_LEN(read-only)
   CFGI_RFCAL_DATE,       // type=dword (read-only)
   CFGI_HOME_SIDNID_LIST, // type=array of AEEConfigSIDNIDPairTypes
                          //      with OEMNV_HOME_SIDNID_SIZE
                          //      elements.
   CFGI_LOCK_SIDNID_LIST, // type=array of AEEConfigSIDNIDPairTypes
                          //      with OEMNV_LOCK_SIDNID_ARRSIZE
                          //      elements.
   CFGI_IMSI_MCC,         // type=uint16 (MCC encoded as decimal)   (read-only)
   CFGI_IMSI_11_12,       // type=uint16 (MNC encoded as decimal)   (read-only)
   #ifdef FATRUE_LOCK_IMSI_MCCMNC
   CFGI_IMSI_SETMCC,   // tyoe= SetImsi
   #endif
   CFGI_IMSI_S,           // type=AECHAR *, len==OEMNV_IMSI_S_LENGTH(read-only)
   CFGI_PRL_ENABLED,      // type=boolean
   CFGI_PRI_CH_A,         // type=uint16
   CFGI_PRI_CH_B,         // type=uint16
   CFGI_SEC_CH_A,         // type=uint16
   CFGI_SEC_CH_B,         // type=uint16
   CFGI_AOC,              // type=byte   (read-only)
   CFGI_HOME_SID_REG,     // type=boolean
   CFGI_FORN_SID_REG,     // type=boolean
   CFGI_FORN_NID_REG,     // type=boolean

#ifdef FEATURE_ACP
   CFGI_MODE_PREF,        // type=uint16

   CFGI_AMPS_HOME_SID,    // type=uint16
   CFGI_AMPS_FIRSTCHP,    // type=uint16

   CFGI_AMPS_REG_TYPE,    // type=uint16
#endif /* FEATURE_ACP */

   CFGI_DATA_QNC_ENABLED, // type=boolean
   CFGI_DATA_DIALSTRING,  // type=AECHAR *, len <= OEMNV_DATA_DIALSTRING_MAXLEN

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
   CFGI_RTRE_CONFIGURATION,   // nv_rtre_configuration_type
#endif
 
   ////////////////////////////////////////////////////////////////
   // Security Menu
   ////////////////////////////////////////////////////////////////

   CFGI_LOCKCODE,              // type=AECHAR *, len <= OEMNV_LOCKCODE_MAXLEN
   CFGI_PHONE_PASSWORD,        //type = uint16
   CFGI_PHONE_PASSWORD_CHECK,  //type = boolean
   CFGI_RESTRICT_OUTGOING,     //type = byte
   CFGI_RESTRICT_INCOMING,     //type = byte
   CFGI_PHONEBOOK_LOCK_CHECK,  //type = boolean
   CFGI_RECENTCALL_LOCK_CHECK, //type = boolean
   CFGI_SMS_LOCK_CHECK,        //type = boolean
   CFGI_CALENDAR_LOCK_CHECK,   //type = boolean
   CFGI_KEY_LOCK_CHECK,      //type = boolean
   CFGI_LOCK_RUIM,   //type: boolean 
   CFGI_LOCK_MCCMNC_LIST,

#ifdef FEATURE_FDN
   CFGI_FDN_ENABLED,           // type=boolean
#endif /* FEATURE_FDN */

   CFGI_HEADSET,               // type=boolean
   CFGI_STEREO_HEADSET,        // type=boolean
   CFGI_HFK,                   // type=boolean
   CFGI_USB_HFK,               // type=boolean
   CFGI_SDAC,                  // type=boolean
   CFGI_EXT_PWR,               // type=boolean
   CFGI_SPEAKERPHONE,          // type=boolean
   
   CFGI_SHAKE_MUSIC_CHECK,            //type = boolean,摇一摇换音乐               
   CFGI_SHAKE_FM_RADIO_CHECK,         //type = boolean,摇一摇换收音机    
   CFGI_SHAKE_WALLPAPER_CHECK,        //type = boolean,摇一摇换图片   
   CFGI_SHAKE_SNOOZE_ALARM_CHECK,     //type = boolean,摇一摇换闹钟
   CFGI_SHAKE_VIDEO_CHECK,            //type = boolean,摇一摇换视频
   CFGI_PEDOMETER_CHECK,              //type = boolean,计步器 
   CFGI_FMRADIO_VOLUME,
   CFGI_FMRADIO_CHAN_INFO,
   CFGI_FMRADIO_CHAN_TOTAL,
   CFGI_EMERGENCYNUM_TABLE,    //tpye = EmergencyNum_Table
   CFGI_CALLFORWARD_BUSY_ENABLE,
   CFGI_CALLFORWARD_BUSY_DISABLE,
   CFGI_CALLFORWARD_NOANSWER_ENABLE,
   CFGI_CALLFORWARD_NOANSWER_DISABLE,
   CFGI_CALLFORWARD_NOCONNECT_ENABLE,
   CFGI_CALLFORWARD_NOCONNECT_DISABLE,
   CFGI_CALLFORWARD_ANYWAY_ENABLE,
   CFGI_CALLFORWARD_ANYWAY_DISABLE,
   CFGI_CALLFORWARD_DISABLE_ALL,
   CFGI_CALLFORWARD_WAIT_ENABLE,
   CFGI_CALLFORWARD_WAIT_DISABLE,
   CFGI_CALLFORWARD_VOICEMAIL_ENABLE,
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
   CFGI_CALLFORWARD_BUSY_VOICEMAIL,
   CFGI_CALLFORWARD_NOANSWER_VOICEMAIL,
   CFGI_CALLFORWARD_NOCONNECT_VOICEMAIL,
   CFGI_CALLFORWARD_ANYWAY_VOICEMAIL,
   CFGI_CALLFORWARD_WAIT_TEMP_DISABLE,
   CFGI_CALLFORWARD_DND_ENABLE,
   CFGI_CALLFORWARD_DND_DISABLE,
   CFGI_CALLFORWARD_CNIR_ENABLE,
   CFGI_CALLFORWARD_CNIR_DISABLE,
#endif
   CFGI_SERVICE_PROVIDER,
   CFGI_INPUTMODE,

#ifdef FEATRUE_SET_IP_NUMBER
   CFGI_IP_POUND,
#endif
#ifdef FEATURE_SHORT_CODE_NAM_COUNT
      CFGI_SHORT_CODE_NAM_COUNT,
#endif //#ifdef FEATURE_SHORT_CODE_NAM_COUNT

#ifdef FEATURE_MENU_STYLE
   CFGI_MENU_STYLE,                  //type=byte
#endif

   // BREW Configuration Parameters
   CFGI_BREW_USERNAME,
   CFGI_BREW_PASSWORD,
   CFGI_BREW_SERVER,
   CFGI_BREW_CARRIER_ID,
   CFGI_BREW_BKEY,
   CFGI_BREW_AUTH_POLICY,
   CFGI_BREW_PRIVACY_POLICY,
   CFGI_BREW_SUBSCRIBER_ID,
   CFGI_BREW_PLATFORM_ID,
   CFGI_BREW_TESTOPT,               // DIF_TEST_ALLOWED
   CFGI_BREW_USEAKEY,               // DIF_USE_A_KEY
   CFGI_BREW_AUTOUPGRADE_FLG,       // DIF_AUTO_UPGRADE 
   CFGI_BREW_USEMINFORSID_FLG,      // DIF_MIN_FOR_SID
   CFGI_BREW_PREPAY_FLG,            // DIF_PREPAY
   CFGI_BREW_NOAUTOACK_FLG,         // DIF_NO_AUTO_ACK
   CFGI_BREW_SIDENCODE_FLG,         // DIF_SID_ENCODE
   CFGI_BREW_SIDVALIDATAALL_FLG,    // DIF_SID_VALIDATE_ALL
   CFGI_BREW_IDS_RUIMDELETE_FLG,    // DIF_RUIM_DEL_OVERRIDE

////////////////////////////////////////////////////////////////
// CameraApp
////////////////////////////////////////////////////////////////
   CFGI_CAMERA_ENVIROMENT,      //type = byte
   CFGI_CAMERA_QUALITY,         //type = byte
   CFGI_CAMERA_SIZE,            //type = byte
   CFGI_CAMERA_TONE,            //type = byte
   CFGI_CAMERA_BANDING,         //type = byte
   CFGI_CAMERA_STORAGE,         //type = byte
   CFGI_CAMERA_BRIGHTNESS,      //type = byte
   CFGI_VIDEO_ENVIROMENT,       //type = byte
   CFGI_CAMERA_ICON,            //type = boolean
   CFGI_VIDEO_ICON,             //type = boolean
   CFGI_CAMERA_FRAME,           //type = byte
   CFGI_CAMERA_COLOR,           //type = byte
   CFGI_VIDEO_COLOR,            //type = byte
   CFGI_VIDEO_BRIGHTNESS,       //type = byte

#ifdef FEATURE_PLANEMODE
         CFGI_PLANEMODE,                 //type = byte
#endif

#ifdef FEATURE_USB_FUNCTION_SELECT
   CFGI_USB_FUNCTION,
#endif //FEATURE_USB_FUNCTION_SELECT


   CFGI_MISSED_CALL_ICON,


   ////////////////////////////////////////////////////////////////
   //
   // State Items
   //
   // The following CFGI items are used by the UI to request information
   // that is currently not available through any existing BREW interfaces.
   //
   // Unless otherwise indicated, state items are read-only.
   //
   // The caller must not assume that these items are actually
   // supported by the OEM layer.  If return value from ICONFIG_GetItem()
   // is not AEE_SUCCESS, the Applet should take appropriate action
   // like using default value instead.
   //
   ////////////////////////////////////////////////////////////////
   CFGI_FIRST_STATE_ITEM = 0x8000,   // Move the 'state items' far away
                                     // from the normal config items

   CFGI_DEVICE, //type = uint32
   CFGI_GSENSOR,                      //type = uint32 
   // Debug Menu state items
   CFGI_DEBUG_CHANNEL,    // type=uint16
   CFGI_DEBUG_PILOT_PN,   // type=uint16
   CFGI_DEBUG_TX_AGC,     // type=uint16
   CFGI_DEBUG_TX_AGC_IN_DBM,// type=int16
   CFGI_DEBUG_RX_AGC,     // type=byte
   CFGI_DEBUG_RX_AGC_IN_DBM,// type=int16
   CFGI_DEBUG_ECIO,
   CFGI_DEBUG_SID,        // type=uint16
   CFGI_DEBUG_NID,        // type=uint16
   CFGI_DEBUG_TX_ADJUST,  // type=byte
   CFGI_DEBUG_HDET,       // type=byte
   CFGI_DEBUG_VBATT,      // type=byte
   CFGI_DEBUG_ERRLOG,     // type=OEMErrLOgType
   CFGI_DEBUG_LNA_STATE,  // type=byte
   CFGI_DEBUG_PA_STATE,   // type=byte
   CFGI_DEBUG_RATCHET_STATE,// type=byte

#ifdef T_MSM6500
//#error code not present
#endif

   // Items required for power calculation in Debug Menu
   CFGI_CDMA_DYNAMIC_RANGE, // type=unit16
   CFGI_CDMA_MIN_RX_RSSI,   // type=uint16

#ifdef FEATURE_FACTORY_TESTMODE
   // TRUE if the handset is in 'factory test mode'
   CFGI_FACTORY_TEST_MODE,// type=boolean
#endif /*FEATURE_FACTORY_TESTMODE*/
   CFGI_DEBUG_THERM,      // type=uint16
   CFGI_DEBUG_BAND,       // type=byte
   //
   // TRUE if the keyguard is currently enabled.
   //
   // This item is not read-only. The keyguard can be enabled/disabled by
   // setting this item to TRUE/FALSE.
   //
   CFGI_KEYGUARD_ENABLED, // type=boolean
   CFGI_HEADSET_PRESENT,  // type=boolean
   CFGI_FM_BACKGROUND,    // type=boolean   
#ifdef FEATURE_RANDOM_MENU_COLOR
   /*Menu background RGB value*/
   CFGI_MENU_BGCOLOR,  // type=RGBVAL   
   /*Menu background color state, 0 for off, 1 for a specific color, 2 for random color*/
   CFGI_RANDOM_MENU,  // type=byte   
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for REND
   /*Menu default REND*/
   CFGI_DEFAULT_REND,  // 默认界面转换效果
   /*Menu default color REND, 0 for off, 1 for a specific REND, 2 for random REND*/
   CFGI_REND_STATE,  // 界面效果转换状态，0 不加效果,1 选定效果,2 随机效果   
#endif
#ifdef FEATURE_TOUCHPAD
   CFGI_PEN_CAL,     // 笔校准参数
#endif
   CFGI_LAST_OEM
};

enum
{
  CFGI_DEBUG_BAND_AMPS,
  CFGI_DEBUG_BAND_CDMA,
  CFGI_DEBUG_BAND_PCS,
  CFGI_DEBUG_BAND_GPS,
  CFGI_DEBUG_BAND_SLEEP,
  CFGI_DEBUG_BAND_LAST
};
/*==============================================================================
函数：
       GetRepeatRawNumber
       
说明：
       得到raw　number.用于把连续重复的号码记录为一条记录。

参数：
      
返回值：
       无。

备注：:

==============================================================================*/
int GetRepeatRawNumber(AECHAR * number,AECHAR * rawnumber);

void OEM_RestoreFactorySetting( void );

boolean OEM_IsEmergency_Number(char *pNumber,int len);
extern dword OEM_GetMEESN(void);
extern void OEM_SetMEESN(dword esn);
#endif // !OEMCFGI_H


