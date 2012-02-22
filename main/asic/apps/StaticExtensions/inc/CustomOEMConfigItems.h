#ifndef CUSTOMOEMCONFIGITEMS_H
#define CUSTOMOEMCONFIGITEMS_H
#ifdef FEATURE_ANALOG_TV
#include "AEETv.h"
#endif
/*=============================================================================

FILE: CustomOEMConfigItems.h

SERVICES:  Non-standard IConfig items 

GENERAL DESCRIPTION:
   This file defines the IConfig items that are used by the UI that are 
   implemented at the OEM layer and are not currently supported by the BREW SDK.

   The caller must not assume that these items are actually supported by the 
   OEM layer.  If the return value from ICONFIG_GetItem() is not SUCCESS, the 
   caller should take appropriate action, such as using default values instead. 
   
   NOTE THAT THIS FILE MUST BE MAINTAINED WITH IDENTICAL CONTENT TO THE
   VERSION IN THE uiOne HDK TREE.
 
PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2004-2007 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/inc/CustomOEMConfigItems.h#8 $
  $DateTime: 2008/12/30 02:21:57 $
  $Author: krishnac $
  $Change: 812537 $

=============================================================================*/

#include "OEMFeatures.h"
#if !defined( AEE_SIMULATOR)
#include "customer.h"
#endif

#include "AEEConfig.h"

#define CUSTOMOEMCONFIG_BASE 0x9000

#ifndef CUST_EDITION
////
// Default Backlight setting.
//
// Type: byte, 0: never on, 1-254: secs before going off, 255: always on
// 
#define CFGI_BACK_LIGHT         (CUSTOMOEMCONFIG_BASE + 0)

////
#ifndef FEATURE_BMP
// Service alert (enable/disable)
//
// Type: byte, Valid values defined by ALERT_xxx
#define CFGI_SVC_ALERT         (CUSTOMOEMCONFIG_BASE + 1)
#endif

////
// Ext Pwr Backlight (on/off)
//
// Type: byte, Valid values defined by EXTPWRBKLT_xxx
#define CFGI_EXTPWR_BK_LIGHT        (CUSTOMOEMCONFIG_BASE + 2)

#ifndef FEATURE_BMP
////
// Voice privacy (standard/enhanced)
//
// Type: byte, Valid values defined by VOICEPRIVACY_xxx
#define CFGI_VOICEPRIVACY     (CUSTOMOEMCONFIG_BASE + 3)

#define VOICEPRIVACY_STD      0
#define VOICEPRIVACY_ENH      1


////
// RTRE Configuration (CDMA R-UIM only)
// 
// Type: byte, Valid values defined by RTRE_CONFIG_xxx
#define CFGI_RTRE_CONFIG       (CUSTOMOEMCONFIG_BASE + 4)

#define RTRE_CONFIG_RUIMONLY             0
#define RTRE_CONFIG_NVONLY               1
#define RTRE_CONFIG_RUIMORDROPBACK       2
#define RTRE_CONFIG_SIMACCESS            3
#define RTRE_CONFIG_DISABLED             255
#endif

/////
// UnitTest Activation status
//
// Type:  boolean.  TRUE if UnitTest is currently active
#define CFGI_UNITTEST_ACTIVE   (CUSTOMOEMCONFIG_BASE + 5)

#ifndef FEATURE_BMP
////
// Radio Technology supported by the phone
//
// Type: byte, Valid values defined by RADIO_TECH_xxx
#define CFGI_RADIO_TECH        (CUSTOMOEMCONFIG_BASE + 6)

#define RADIO_TECH_CDMA                  1
#define RADIO_TECH_GSM                   2
#define RADIO_TECH_WCDMA                 4
#endif

////
// Contrast level
//
// Type: byte.  Valid values are defined by CONTRAST_LVL_xxx
#define CFGI_CONTRAST_LVL     (CUSTOMOEMCONFIG_BASE + 7)
 
#define CONTRAST_LVL_MIN      0
#define CONTRAST_LVL_MAX      7

#ifndef FEATURE_BMP
////
// Factory Test Mode
//
// Type: boolean.  TRUE if the device is in factory test mode
// Read-only
#define CFGI_FACTORY_TEST_MODE         (CUSTOMOEMCONFIG_BASE + 8)

////
// Firmware Version
//
// Type: AECHAR string Read-only
#define CFGI_FIRMWARE_VERSION  (CUSTOMOEMCONFIG_BASE + 9)

#define FIRMWAREVERSION_MAX_LEN  32

//Auto Answer
//
//Type: Boolean. True if auto answer is on.
//Corresponds to NV_AUTO_ANSWER_I
#define CFGI_AUTO_ANSWER  (CUSTOMOEMCONFIG_BASE + 10)


//TTY mod

//Type: byte. 0 - Full, 1- Talk, 2 - Hear, 3 - Off
#define CFGI_TTY  (CUSTOMOEMCONFIG_BASE + 11)

#define TTY_FULL 0
#define TTY_TALK 1
#define TTY_HEAR 2
#define TTY_OFF  3
#endif

//Leap Seconds since start of System Time
//
//Type: uint8
//Corresponds to NV_DB_LP_SEC_I
#define CFGI_LP_SEC  (CUSTOMOEMCONFIG_BASE + 12)


//Offset local time from System Time, i.e. timezone offset
//
//Type: uint8
//Corresponds to NV_DB_LTM_OFF_I 
#define CFGI_LTM_OFF  (CUSTOMOEMCONFIG_BASE + 13)

//Daylight savings time indicator
//
//Type: boolean
//Corresponds to NV_DB_DAYLT_I  
#define CFGI_DAYLT  (CUSTOMOEMCONFIG_BASE + 14)

#endif //#ifndef CUST_EDITION

////
// If this value is TRUE, the messaging daemons will not run.  This item 
// can be used to prevent the messaging daemons from conflicting with 
// another messaging client on the device.
//
// Type:  boolean
//
// NOTE: This item is is shared with CustomConfigItems.h, as it must be 
// accessed by the OEM layer.  However it is still implemented in 
// the CustomConfigItems module.
#ifndef CFGI_MSG_DISABLE_DAEMONS
#define CFGI_MSG_DISABLE_DAEMONS (0xA000 + 63)
#else
#if CFGI_MSG_DISABLE_DAEMONS != (0xA000 + 63)
#error CFGI_MSG_DISABLE_DAEMONS value incorrect
#endif 
#endif

#ifdef CUST_EDITION

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
#ifdef FEATURE_ANALOG_TV
#define MAXLEN 32
typedef struct
{
	AECHAR name[MAXLEN];      //频道名，如“中央一台”之类的,名字最多为32个字符
	AECHAR channel[MAXLEN];  //频道号，如"1、2"频道之类的 [MAXLEN]
}TvBookmark;

typedef struct
{
	int32	BrightnessStep; /*设置亮度0~15*/
    int32   ContrastStep; /*设置对比度0~15*/
    int32   DefinitionStep; /*设置清晰度0~63*/
    int32   SaturationStep; /*设置色彩饱和度0~255*/
    int32   SoundStep;/*声音*/
    int ChannelCountAble;//上次搜到的有信号的台总数
    int CurrentChannel;//上一次退出之前看的那个台索引
    TvBookmark   Bookmark[MAXLEN];//最多可以保存32个书签
    int   Bookmarktotal;       //当前存储频道总数
    TLG_REGION_CODE region; //当前设置的区域
}CFG_TvSetting;
#endif

#ifdef FEATURE_USES_MMS
/* MMS setting */
typedef struct _OEMConfigMMsInfo_
{
   boolean                         	bUseProxy;
   char                  			hProxyHost[128];
   int                         		iProxyPort;
}CFG_MMsInfo;
#endif


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

enum{
    CFGI_BACK_LIGHT = CUSTOMOEMCONFIG_BASE,
    CFGI_SVC_ALERT,
    CFGI_EXTPWR_BK_LIGHT,
    CFGI_VOICEPRIVACY,               // type=byte
    CFGI_CONTRAST_LVL,
    CFGI_FACTORY_TEST_MODE,
    CFGI_TTY,
#ifdef FEATRUE_AUTO_POWER
    CFGI_AUTO_POWER_ON,               //type = Auto_Power_Cfg
    CFGI_AUTO_POWER_OFF,              //type = Auto_Power_Cfg
#endif

    CFGI_POWERDOWN_ALARM,             //type =PowerDown_Alarm_Cfg
    CFGI_ALARM_FLAG,                  //type = boolean
#ifdef FEATURE_SPORTS_APP
    CFGI_SPORT_FLAG,
#endif	

#ifdef FEATRUE_SET_IP_NUMBER
    CFGI_IP_NUMBER,                   //type = IP_Number_Cfg
#endif
    CFGI_AUTO_REDIAL,                 //type = nv_auto_redial_type
    CFGI_SMS_RINGER,                 // type=byte    ?ìD?á?éù(0-±íê??T)
    CFGI_ALARM_RINGER,               //type = byte ???óá?éù
    CFGI_SMS_RINGER_ID,              //type = byte ?ìD?á?éù
    CFGI_PROFILE_CUR_NUMBER,         //?é?°?￡ê?ààDí
    CFGI_PROFILE_ALERT_TYPE,         //à′μ?ìáê?
    CFGI_PROFILE_SMS_RINGER,         //?ìD?ìáê?
    CFGI_PROFILE_CALL_RINGER,        //à′μ?á?éù????  
    CFGI_PROFILE_ALARM_RINGER,       //???óá?éù????
    CFGI_PROFILE_SMS_RINGER_ID,      //SMSò?à?   
    CFGI_PROFILE_STARTUP_MUSIC,      //?a?úò?à?
    CFGI_PROFILE_SHUTDOWN_MUSIC,     //1??úò?à?
    CFGI_PROFILE_POWERONOFF_ALERT,   //?a1??úò?à?ìáê?
    CFGI_PROFILE_MISSED_CALL_ALERT,  //?ìD?á?éù????
    CFGI_PROFILE_KEYSND_TYPE,        //?à2ê°′?üò?
    CFGI_PROFILE_KEYTONE_LENGTH,     //°′?üò?3¤?è  
    CFGI_PROFILE_RINGER_VOL,         //á?éùò?á?
    CFGI_PROFILE_EAR_VOL,            //?ú?úò?á?
    CFGI_PROFILE_BEEP_VOL,           //?ü?ìò?á?
    CFGI_KEYSND_TYPE,                //?à2ê°′?üò?  
#ifdef FEATURE_TIME_DATA_SETTING
    CFGI_DATE_FORMAT,                 //type = byte
#endif 
    CFGI_SCREENSAVER_TIME,            //?á±￡ê±??
    CFGI_WALLPAPER,                   // ×à??????
#ifdef FEATURE_ANIMATION_POWERUPDOWN
    CFGI_STARTUP_ANIMATION,           // ?a?ú?ˉ?-
    CFGI_POWEROFF_ANIMATION,          // 1??ú?ˉ?-
#endif
    CFGI_BACKLIGHT_LEVEL,             // ±31aáá?è??±e????
    CFGI_DESKTOP_THEME,               // type=byte
#ifdef FEATURE_SCREEN_SAVE
    CFGI_SCREENSAVE_TYPE,             // ?á??±￡?¤ààDí
#endif
#ifdef FEATURE_LED_CONTROL
    CFGI_LED_CONTROL,                 //type=byte
#endif
#ifdef FEATRUE_KEY_PAD_CTL
    CFGI_KEY_PAD_CTL,                 //type = Key_pad_Cfg
#endif
#ifdef FEATURE_USES_MMS
	CFGI_MMS_SETTING,				  //MMS Setting
#endif

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
    CFGI_SHAKE_MUSIC_CHECK,            //type = boolean,ò?ò?ò???ò?à?               
    CFGI_SHAKE_FM_RADIO_CHECK,         //type = boolean,ò?ò?ò???ê?ò??ú    
    CFGI_SHAKE_WALLPAPER_CHECK,        //type = boolean,ò?ò?ò???í???   
    CFGI_SHAKE_SNOOZE_ALARM_CHECK,     //type = boolean,ò?ò?ò??????ó
    CFGI_SHAKE_VIDEO_CHECK,            //type = boolean,ò?ò?ò???êó?μ
    CFGI_PEDOMETER_CHECK,              //type = boolean,??2??÷ 
    CFGI_FMRADIO_VOLUME,
    CFGI_FMRADIO_CHAN_INFO,
    CFGI_FMRADIO_CHAN_TOTAL,
    CFGI_EMERGENCYNUM_TABLE,    //tpye = EmergencyNum_Table
    CFGI_CALLFORWARD_BUSY_ENABLE,
    CFGI_CALLFORWARD_BUSY_DISABLE,
    CFGI_CALLFORWARD_NOANSWER_ENABLE,
    CFGI_CALLFORWARD_NOANSWER_DISABLE,
    CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE,
    CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE,
    CFGI_CALLFORWARD_UNREACHABLE_ENABLE,
    CFGI_CALLFORWARD_UNREACHABLE_DISABLE,
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

    ////////////////////////////////////////////////////////////////
    // CameraApp
    ////////////////////////////////////////////////////////////////
#ifdef FEATURE_CAMERA_MULTI_SENSOR
   CFGI_CAMERA_MULTI,
   CFGT_CAMERA_FLASH,
#endif
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
    CFGI_MISSED_CALL_ICON,
    CFGI_GSENSOR,                      //type = uint32 
    CFGI_HEADSET_PRESENT,  // type=boolean
    CFGI_FM_BACKGROUND,    // type=boolean   
#ifdef FEATURE_RANDOM_MENU_COLOR
    /*Menu background RGB value*/
    CFGI_MENU_BGCOLOR,  // type=RGBVAL   
    /*Menu background color state, 0 for off, 1 for a specific color, 2 for random color*/
    CFGI_RANDOM_MENU,  // type=byte   
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for REND
    CFGI_REND_STATE,
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
    CFGI_PEN_CAL,     // ±êD￡×?2?êy
#endif
    CFGI_BREWSET_USENAME,
    CFGI_BREWSET_PASSWORD,
    CFGI_LANGUAGE_MOD,
#ifdef FEATURE_ANALOG_TV
    CFGI_TV_OR_CAMERA,
    CFGI_TV_SETCHANNL,
#endif
    //Add By zzg 2010_11_22
#ifdef FEATURE_APP_BLUETOOTH
	CFGI_BT_STATUS,
#endif
    CFGI_FLSHLITHG_STATUS,
    CFGI_WMSWRITD_END_STATUS,
    CFGI_FM_PLAY_MODE,
    CFGI_IDLE_DATETIME_MODE,  //add yangdecai 2011-04-13
    CFGI_CAMERA_PHOTO_MODE,   //add pyuangui 20110516
#if defined (FEATURE_VERSION_W208S)
	CFGI_OTKSL_TIMES,		  //Add By zzg 2012_01_18
#endif
	//Add End
	
#ifdef FEATURE_LONG_NETLOCK
	CFGI_NET_LOCK_FLAGS,
#endif
#ifdef FEATURE_USES_MMS    
    CFGI_MMSIMAGE,
    CFGI_MMSSOUND,
    CFGI_MMSVIDEO,
    CFGI_MMSDRAFTDATA_INFO,
    CFGI_MMS_DRAFTCOUNT,      
    CFGI_MMSOUTDATA_INFO,
    CFGI_MMSINDATA_INFO,
    CFGI_MMS_OUTCOUNT,
    CFGI_MMS_INCOUNT,  
    CFGI_WMS_MMSNOTIFY,
    CFGI_WMS_MMSDELIVERYREPORT,
    CFGI_WMS_READREPLY,
    CFGI_WMS_REPORTALLOWED,
    CFGI_WMS_SENDERVISIBILITY,
    CFGI_MMS_SERVER_ADDRESS,
    CFGI_MMS_USER_NAME,
    CFGI_MMS_PASSWORD,
    CFGI_MMS_PROXY,
    CFGI_MMS_PORT,
#endif  
#ifdef FEATURE_VERSION_W208S
    CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
    CFGI_SMS_RESTRICT_RECEIVE_INFO,    
#endif    
    CFGI_NET_LOCK_ENABLED,
    CFGI_NET_LOCK_MNC,
    CFGI_NET_LOCK_MCC,
    CFGI_NET_LOCK_SID,  
    CFGI_CUST_END
};

#endif

#endif /* CUSTOMOEMCONFIGITEMS_H */
