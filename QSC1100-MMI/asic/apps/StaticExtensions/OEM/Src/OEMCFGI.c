/*===========================================================================
                              OEMCFGI.c

  OEM CONFIG CONTROL ROUTINES
  
  This file contains a reference implementation of a OEM Config
  control interfaces.
    
        Copyright ?1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

/*=============================================================================

$Header: //depot/asic/qsc60x0/apps/StaticExtensions/OEM/Src/OEMCFGI.c#6 $
$DateTime: 2007/04/12 10:27:41 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/28/06   ypmw    Fixed LCD Display for SC1X
07/17/06   JD      removed CFGI_SMS_CHANNEL_SELECTION and MAX_MSG_PAYLOAD
03/02/05   JLG     Add CFGI_SUBSCRIBERID support.
04/19/04   PA      Added CFGI_LAST_RX_DATA_COUNT, CFGI_LAST_TX_DATA_COUNT,
                   CFGI_TOTAL_RX_DATA_COUNT and CFGI_TOTAL_TX_DATA_COUNT
04/16/04   SUN     Fixed Compile Error
04/14/04   SUN     Added new CFGI item CFGI_VR & CFGI_TTY
03/31/04   dyc     Added 2nd chain RxAGC support for MSM6500.
                   Use int32 for CFGI_DEBUG_RX_AGC_IN_DBM item.
03/30/04   SUN     Check for return values
03/08/04   PMD     Added Support for WMS Application Mode and seperate 
                   Voicemails for CDMA and GSM / WCDMA (GW)
03/04/04   dyc     Update for field debug support
02/20/04   sun     Fixed Compile Warning
02/14/04   PMD     Added SMS Retry Period Config Item
02/12/04   RAM     Added new CFGI items for CDMA/HDR diversity control.
02/12/04   SUN     Fixed Compile Errors
02/10/04   RAM     Read the AMR item on power-up.
02/10/04   SUN     Added support for CFGI_AMR
01/28/04   RAM     Added new CFGI item for SpeakerPhone.
01/26/04   PMD     Added SMS Segmentation Message Reference
01/26/04   PMD     Added SMS Segmentation Settings
12/09/03    lz     Added new CFGI item for BREW Download information
11/26/03   SUN     Added new CFGI item for CUG
11/23/03   SUN     Added new CFGI items for devices
11/06/03   AK/PMD  Changes for Brew 3.0
11/05/03   RA      Added PA,MSM,PMIC,CHARGER,BATTERY thermistor readings
11/04/03   PMD     Added CDMA and GW Default Template Settings for SMS
10/31/03   PMD     Added IConfig Items for SMS Cell Broadcast Settings
                   Added IConfig Items for SMS Alert Settings
10/17/03   kar     Fixed link errors
10/15/03   SUN     Fixed Compile error
08/20/03   ADM     Added code to retrieve call times information 
08/19/03   SUN     Added Support for IMEI
08/05/03   PMD     Added Mode preference (CDMA / GW) NV Item for SMS to be 
                   accessed using IConfig
06/02/03   PMD     Usage of NV items for retrieving / storing Voice Mail Number

=============================================================================*/
#include "customer.h"
#include "OEMFeatures.h"
#include "BREWVersion.h"
#include "AEEComdef.h"
#include "OEMConfig.h"
#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "AEEDownload.h"
#if MIN_BREW_VERSION(3,0)
#include "AEE_OEMDispatch.h"
#endif

#include "OEMCFGI.h"
#include "OEMNV.h"
#include "OEMFS.h"
#ifndef WIN32
#include "snd.h"
#include "hs.h"
#endif
#ifndef FEATURE_OEMUI_TASK
#include "ui.h"
#endif
#ifndef WIN32
#include "mobile.h"
#include "adc.h"
#include "rfi.h"
#include "therm.h"
#endif


#ifdef T_MSM6500
//#error code not present
#endif
#ifdef FEATURE_NPT
#error code not present
#endif /* FEATURE_NPT */

#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#ifndef WIN32
#include "db.h"
#endif
#endif

#include "cm.h"

#if defined( FEATURE_APP_MANAGER)
#include "OEMSVC.h"
#endif//#if defined( FEATURE_APP_MANAGER)

#include "Wms.h"
#include "Appscommon.h"
#include "AEECallList.h" 

#include "AEEAlarm.h"
#ifdef FEATURE_KEYGUARD
#include "OEMKeyguard.h"
#endif
#include "AEECM.h"
#ifdef FEATURE_SUPPORT_BT_APP
#ifndef WIN32
#include "bt_ui_int.h"
#endif
#endif
#ifdef FEATURE_TOUCHPAD
#include "touchpad.h"
#endif

/*===========================================================================

                     DEFINES AND CONSTANTS

===========================================================================*/
////
// Version number of the OEMConfigListType structure.  This value
// must be incremented whenever an existing field in changed or deleted
// in OEMConfigListType.  It does not need to be incremented when a new
// field is added to the end of OEMConfigListType.
//                                  
#define OEMCONFIGLIST_VERSION ( (uint16) 0x000F )
#ifndef WIN32
extern NOTICE_TYPE ver_date[];
extern NOTICE_TYPE ver_time[];
#else
#include "oem_common_nv.h"
#include "oemhelperfunctype.h"
extern NOTICE_TYPE ver_date[];
extern NOTICE_TYPE ver_time[];
#endif

////
// The EFS file that stores the OEM configuration.
#if MIN_BREW_VERSION(3,0)
#ifndef WIN32
#define OEMCONFIGLIST_FILE (AEEFS_SYS_DIR"priv/oemconfig.dat")
#else
#define OEMCONFIGLIST_FILE ("priv/oemconfig.dat")
#endif
#else
#define OEMCONFIGLIST_FILE ("brew/oemconfig.dat")
#endif

#define CDMAMIN 1

////
// Used to define an entry in a OEM Config Item table
#define CFGTABLEITEM(cfgi,size)  \
         {cfgi, OEMPriv_GetItem_##cfgi, OEMPriv_SetItem_##cfgi, (size)}

////
// Used to define a read-only entry in a OEM Config Item table
#define CFGTABLEITEM_READONLY(cfgi,size)  \
         {cfgi, OEMPriv_GetItem_##cfgi, OEMPriv_SetItem_ReadOnly, (size)}
         
////
// Used to define an empty entry in a OEM Config Item table
#define CFGTABLEITEM_EMPTY(cfgi) {cfgi, NULL, NULL, -1}


/* Closed user group index */
typedef PACKED struct
{
   boolean                         present;
   byte                            msb;
   byte                            lsb;
} OEMConfigIndex;

/* Forward CUG info */
typedef PACKED struct 
{
   boolean                         present;
   OEMConfigIndex                  cug_index;
   boolean                         suppress_pref_cug;
   boolean                         suppress_outgoing_access;
} OEMConfigCugInfo;
#ifdef FEATRUE_KEY_PAD_CTL
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
    #define KEY_PDA_CTL_FROM_TIME 57600000
    #define KEY_PDA_CTL_TO_TIME      28800000
#else
    #define KEY_PDA_CTL_FROM_TIME 59400000
    #define KEY_PDA_CTL_TO_TIME      32400000
#endif
#endif

#if defined(FEATURE_PEKTEST)
// 为方便 PEK 测试，PEK 测试配置放在最前面
#define DEFAULT_BREW_CARRIER_ID     27
#define DEFAULT_BREW_PLATFORM_ID    600
#define DEFAULT_BREW_USERNAME       "card"
#define DEFAULT_BREW_PASSWORD       "card"
#define DEFAULT_BREW_SERVER         "oemdemo.qualcomm.com"
#define DEFAULT_BREW_DOWNLOAD_FLG   (DIF_TEST_ALLOWED | DIF_SID_VALIDATE_ALL | DIF_RUIM_DEL_OVERRIDE | DIF_MIN_FOR_SID)
#define DEFAULT_BREW_APOLICY        APOLICY_SID
#define DEFAULT_BREW_PPOLICY        PPOLICY_BREW_OR_CARRIER

#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
// 中国电信缺省的brew设置 bkey -- 0 平台号暂用别的厂商的，确定后需更改
// 要求 BAM 从2.0.5升级至2.1,根据规范 PPP 连接按互联星空参数进行
#define DEFAULT_BREW_CARRIER_ID     53
#define DEFAULT_BREW_PLATFORM_ID    30012
#define DEFAULT_BREW_USERNAME       "ctwap@mycdma.cn"
#define DEFAULT_BREW_PASSWORD       "vnet.mobi"
#define DEFAULT_BREW_SERVER         "brew.vnet.mobi"
#define DEFAULT_BREW_DOWNLOAD_FLG   (DIF_TEST_ALLOWED | DIF_SID_VALIDATE_ALL | DIF_RUIM_DEL_OVERRIDE | DIF_AUTO_UPGRADE | DIF_MIN_FOR_SID)
#define DEFAULT_BREW_APOLICY        APOLICY_SID
#define DEFAULT_BREW_PPOLICY        PPOLICY_BREW_OR_CARRIER

#elif defined(FEATURE_CARRIER_TAIWAN_APBW)
// 亚太宽频缺省的brew设置 bkey -- 0 
#define DEFAULT_BREW_CARRIER_ID     97
#define DEFAULT_BREW_PLATFORM_ID    600
#define DEFAULT_BREW_USERNAME       ""
#define DEFAULT_BREW_PASSWORD       ""
#define DEFAULT_BREW_SERVER         "ads.qma.com.tw"
#define DEFAULT_BREW_DOWNLOAD_FLG   (DIF_TEST_ALLOWED | DIF_SID_VALIDATE_ALL | DIF_RUIM_DEL_OVERRIDE | DIF_AUTO_UPGRADE | DIF_MIN_FOR_SID)
#define DEFAULT_BREW_APOLICY        APOLICY_SID
#define DEFAULT_BREW_PPOLICY        PPOLICY_BREW_OR_CARRIER

#else

// 缺省的brew设置，pek测试用
#define DEFAULT_BREW_CARRIER_ID     27
#define DEFAULT_BREW_PLATFORM_ID    600
#define DEFAULT_BREW_USERNAME       "card"
#define DEFAULT_BREW_PASSWORD       "card"
#define DEFAULT_BREW_SERVER         "oemdemo.qualcomm.com"
#define DEFAULT_BREW_DOWNLOAD_FLG   (DIF_TEST_ALLOWED | DIF_SID_VALIDATE_ALL | DIF_RUIM_DEL_OVERRIDE | DIF_MIN_FOR_SID)
#define DEFAULT_BREW_APOLICY        APOLICY_SID
#define DEFAULT_BREW_PPOLICY        PPOLICY_BREW_OR_CARRIER
#endif

#define OEMCONFIG_UPDATED           1

////
// Configuration items managed by the OEM layer.
//
// NOTE: Whenever a change is made to an existing memory ofthis structure, 
//       OEMCONFIGLIST_VERSION must be incremented.   It does not need to be
//       incremented after adding a new item to the end of the structure.
//  
typedef PACKED struct
{
   byte          alert_type;                   // CFGI_ALERT_TYPE
   byte          silence_all;                  // CFGI_SILENCEALL
   byte          headset_ringer;               // CFGI_HEADSET_RNG
   byte          missed_call_alert;            // CFGI_MISSED_CALL_ALERT
   byte          roaming_alert;                // CFGI_ROAMING_ALERT
   byte          internet_call_alert;          // CFGI_INTERNET_ALERT
   dword         internet_call_time;           // CFGI_BROWSER_CALL_TIMER
   byte          sms_service_option;           // CFGI_SMS_SERVICE_OPTION
   byte          sms_is41_workaround;          // CFGI_SMS_IS41_WORKAROUND
   byte          sms_mo_encoding;              // CFGI_SMS_MO_ENCODING
   byte          sms_mode;                     // CFGI_SMS_MODE
   byte          sms_app;                      // CFGI_SMS_APP
   byte          sms_cb_auto_delete;           // CFGI_SMS_CB_AUTO_DELETE        
   byte          sms_cb_silent_alert;          // CFGI_SMS_CB_SILENT_ALERT       
   byte          sms_cb_lang_filter;           // CFGI_SMS_CB_LANG_FILTER 
   byte          sms_alert_display;            // CFGI_SMS_ALERT_DISPLAY
   byte          sms_alert_sound;              // CFGI_SMS_ALERT_SOUND
   byte          sms_alert_volume;             // CFGI_SMS_ALERT_VOLUME
   byte          sms_cdma_default_template;    // CFGI_SMS_CDMA_DEFAULT_TEMPLATE
   byte          sms_gw_default_template;      // CFGI_SMS_GW_DEFAULT_TEMPLATE
   byte          sms_segment_setting;          // CFGI_SMS_SEGMENT_SETTING
   word          sms_segment_ref_num;          // CFGI_SMS_SEGMENT_REF_NUM
   word          sms_retry_period;             // CFGI_SMS_RETRY_PERIOD

   // CFGI_VOICEMAIL_NUMBER
   AECHAR        voicemail_number[OEMNV_VOICEMAIL_MAXLEN];

   // CFGI_VOICEMAIL_NUMBER_CDMA
   AECHAR        voicemail_number_cdma[OEMNV_VOICEMAIL_MAXLEN];

   // CFGI_VOICEMAIL_NUMBER_GW
   AECHAR        voicemail_number_gw[OEMNV_VOICEMAIL_MAXLEN];

   dword         recent_mt_call_time;          // CFGI_RECENT_MT_CALL_TIMER
   dword         recent_mo_call_time;          // CFGI_RECENT_MO_TIMER

   byte          minute_alert;                 // CFGI_MINUTE_ALERT
   byte          any_key_answer;               // CFGI_ANYKEY_ANSWER
   byte          headset_autoanswer;           // CFGI_HEADSET_AUTOANSWER
   uint32        device_mask;                  // CFGI_HEADSET|CFGI_STEREO_HEADSET|CFGI_HFK|
                                               // CFGI_USB_HFK|CFGI_SDAC| CFGI_EXT_PWR | CFGI_SPEAKERPHONE

   uint32        shake_mask;   //CFGI_SHAKE_MUSIC_CHECK|CFGI_SHAKE_FM_RADIO_CHECK|CFGI_SHAKE_WALLPAPER_CHECK|
                              //CFGI_SHAKE_SNOOZE_ALARM_CHECK|CFGI_SHAKE_VIDEO_CHECK|CFGI_PEDOMETER_CHECK
   
   OEMConfigCugInfo  cug;                         // CFGI_CUG  
    boolean           vr;                         // CFGI_VR

   uint16        phone_password;               // CFGI_PHONE_PASSWORD     
   //boolean     phone_password_check;         // CFGI_PHONE_PASSWORD_CHECK  //type = boolean
   byte          restrict_outgoing;            // CFGI_RESTRICT_OUTGOING     //type = byte
   byte          restrict_incoming;            // CFGI_RESTRICT_INCOMING     //type = byte
   //情景模式
   byte          p_profile_cur_number;                           // CFGI_PROFILE_CUR_NUMBER
   byte          p_alert_type[PROFILENUMBER];                    // CFGI_PROFILE_ALERT_TYPE
   byte          p_bt_sms_ringer[PROFILENUMBER];                 // CFGI_PROFILE_SMS_RINGER  
   ringID        p_call_ringer[PROFILENUMBER];                   // CFGI_PROFILE_CALL_RINGER      
   ringID        p_alarm_ringer[PROFILENUMBER];                  // CFGI_PROFILE_ALARM_RINGER
   ringID        p_sms_ringer_id[PROFILENUMBER];                 // CFGI_PROFILE_SMS_RINGER_ID   
   uint16        p_start_music[PROFILENUMBER];                   // CFGI_PROFILE_STARTUP_MUSIC
   uint16        p_shutdown_music[PROFILENUMBER];                // CFGI_PROFILE_SHUTDOWN_MUSIC
   byte          p_poweronoff_alert[PROFILENUMBER];              // CFGI_PROFILE_POWERONOFF_ALERT   
   byte          p_missed_call_alert[PROFILENUMBER];             // CFGI_PROFILE_MISSED_CALL_ALERT
   //byte          p_headset_autoanswer[PROFILENUMBER];          // CFGI_PROFILE_HEADSET_AUTOANSWER
   byte          p_keysnd_type[PROFILENUMBER];                   // CFGI_PROFILE_KEYSND_TYPE 
   byte          p_keytone_length[PROFILENUMBER];                // CFGI_PROFILE_KEYTONE_LENGTH      
   byte          p_ringer_level[PROFILENUMBER];                  // CFGI_PROFILE_RINGER_VOL 
   byte          p_handset_ear_level[PROFILENUMBER];             // CFGI_PROFILE_EAR_VOL 
   byte          p_beep_level[PROFILENUMBER];                    // CFGI_PROFILE_BEEP_VOL  
   //短信铃声(0-表示无)
   byte          bt_sms_ringer;                //CFGI_SMS_RINGER           // type=byte 
   //闹钟铃声
   uint16        alarm_ringer;                 //CFGI_ALARM_RINGER   
   //短信铃声
   uint16        sms_ringer_id;                //CFGI_SMS_RINGER_ID    
   //多彩按键音
   byte          keysnd_type;                  //CFGI_KEYSND_TYPE   
   //屏保时间
   byte          p_screensaver_time;           // CFGI_SCREENSAVER_TIME 
   //桌面墙纸
   char          s_wallpaper[AEE_MAX_FILE_NAME/*FILESPECLEN*/];     //CFGI_WALLPAPER
#ifdef FEATURE_ANIMATION_POWERUPDOWN
   //开机动画
   char          s_startupani[AEE_MAX_FILE_NAME/*FILESPECLEN*/];    //CFGI_STARTUP_ANIMATION 
   //关机动画
   char          s_poweroffani[AEE_MAX_FILE_NAME/*FILESPECLEN*/];   //CFGI_POWEROFF_ANIMATION    
#endif
   //背光亮度级别      
   byte          backlight_level;              //CFGI_BACKLIGHT_LEVEL    

   //keyToneLength key_tone_length;            // CFGI_KEYTONE_LENGTH
#ifdef FEATURE_SET_AUTOKEYGUARD
   byte          autokey_guard;                // CFGI_AUTOKEYGUARD
#endif
   //byte        keypad_light_time;            //CFGI_KEYPAD_LIGHT_TIME
   boolean       b_phonebook_lock;             //CFGI_PHONEBOOK_LOCK_CHECK
   boolean       b_recentcall_lock;            //CFGI_RECENTCALL_LOCK_CHECK
   boolean       b_sms_lock;                   //CFGI_SMS_LOCK_CHECK
   boolean       b_calendar_lock;              //CFGI_CALENDAR_LOCK_CHECK
   boolean       b_key_lock;                    //CFGI_KEY_LOCK_CHECK
   boolean       lock_ruim;  
   AEEConfigSIDNIDPairType lock_mccmnc[OEMNV_LOCK_MCCMNC_ARRSIZE];  
   byte          desktop_theme;                //CFGI_DESKTOP_THEME
#ifdef FEATURE_SCREEN_SAVE
   char          s_screensave_type[AEE_MAX_FILE_NAME/*FILESPECLEN*/];//CFGI_SCREENSAVE_TYPE 
#endif
   byte          wms_priority;                  //CFGI_WMS_PRIORITY
   byte          wms_sendmode;                  //CFGI_WMS_SENDMODE
   byte          wms_storetype;                 //CFGI_WMS_STORETYPE
   boolean       wms_delivereports;             //CFGI_WMS_DELIVERYREPORTS
#ifdef FEATURE_AUTOREPLACE
   boolean       wms_autoreplace;               //CFGI_WMS_AUTOREPLACE
#endif
   byte          wms_validityperiod;            //CFGI_WMS_VALIDITYPERIOD
   wms_memory_store_e_type    wms_memstore;     //CFGI_WMS_MEMSTORE
   boolean       wms_resvdmsgalert_timeout;     //CFGI_WMS_RESVDMSGALERT_TIMEIOUT
   boolean       callbacknum_switch;            //CFGI_WMS_CALLBACKNUMSWITCH
   AECHAR        callback_number[WMS_ADDRESS_MAX]; //CFGI_CALLBACKNUM
   byte          wms_mochannel;                 //CFGI_WMS_MO_CHANNEL_SELECT
#ifdef FEATRUE_AUTO_POWER
   Auto_Power_Cfg m_auto_poweron;               //CFGI_AUTO_POWER_ON
   Auto_Power_Cfg m_auto_poweroff;              //CFGI_AUTO_POWER_OFF
#endif 

   PowerDown_Alarm_Cfg powerdown_alarm;         //CFGI_POWERDOWN_ALARM

#ifdef FEATRUE_SET_IP_NUMBER
   IP_Number_Cfg m_ip_number;                   //CFGI_IP_NUMBER
#endif
#ifdef FEATURE_LED_CONTROL
   byte          m_led_control;                 //CFGI_LED_CONTROL
#endif
#ifdef FEATRUE_KEY_PAD_CTL
   Key_pad_Cfg   m_key_pad_ctl;                 //CFGI_KEY_PAD_CTL
#endif
   

   byte        fmRadio_volume;                                  //CFGI_FMRADIO_VOLUME
   sChanInfo   fmRadio_chan_info[MAX_FMRADIO_STORED_CHANNEL];   //CFGI_FMRADIO_CHAN_INFO
   byte        fmRadio_chan_total;                              //CFGI_FMRADIO_CHAN_TOTAL
   EmergencyNum_Table    emerg_table;                 //CFGI_EMERGENCYNUM_TABLE
   char     BUSY_ENABLE[FEATURE_CODE_MAX_LENTH];      //CFGI_CALLFORWARD_BUSY_ENABLE
   char     BUSY_DISABLE[FEATURE_CODE_MAX_LENTH];     //CFGI_CALLFORWARD_BUSY_DISABLE
   char     NOANSWER_ENABLE[FEATURE_CODE_MAX_LENTH];  //CFGI_CALLFORWARD_NOANSWER_ENABLE
   char     NOANSWER_DISABLE[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_NOANSWER_DISABLE
   char     NOCONNECT_ENABLE[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_NOCONNECT_ENABLE
   char     NOCONNECT_DISABLE[FEATURE_CODE_MAX_LENTH];//CFGI_CALLFORWARD_NOCONNECT_DISABLE
   char     ANYWAY_ENABLE[FEATURE_CODE_MAX_LENTH];    //CFGI_CALLFORWARD_ANYWAY_ENABLE
   char     ANYWAY_DISABLE[FEATURE_CODE_MAX_LENTH];   //CFGI_CALLFORWARD_ANYWAY_DISABLE
   char     DISABLE_ALL[FEATURE_CODE_MAX_LENTH];      //CFGI_CALLFORWARD_DISABLE_ALL
   char     WAIT_ENABLE[FEATURE_CODE_MAX_LENTH];      //CFGI_CALLFORWARD_WAIT_ENABLE
   char     WAIT_DISABLE[FEATURE_CODE_MAX_LENTH];     //CFGI_CALLFORWARD_WAIT_DISABLE
   char     VOICEMAIL_ENABLE[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_VOICEMAIL_ENABLE
   ServiceProvider List_SP[OEMNV_MAX_SERVICEPROVIDER_NUMBER]; //CFGI_SERVICE_PROVIDER
   #ifdef FATRUE_LOCK_IMSI_MCCMNC
   SetImsi List[OEMNV_MAX_SERVICEPROVIDER_NUMBER];//CFGI_IMSI_SETMCC
   #endif
   //输入法设置   
   byte     input_mode;                                //CFGI_INPUTMODE   
   boolean m_fm_background;                            /*CFGI_FM_BACKGROUND*/
#ifdef FEATURE_RANDOM_MENU_COLOR
   uint32 m_nmenubgcolor;                       //CFGI_MENU_BGCOLOR
   byte m_nrandommenu;                      //CFGI_RANDOM_MENU
#endif
#ifdef FEATRUE_SET_IP_NUMBER
   boolean m_b_ip_pound;                               /*CFGI_IP_POUND*/
#endif
#ifdef FEATURE_SHORT_CODE_NAM_COUNT
   byte short_code_nam_count;//CFGI_SHORT_CODE_NAM_COUNT
#endif 
   boolean   alarm_flag;                               //CFGI_ALARM_FLAG
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
   char     BUSY_VOICEMAIL[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_BUSY_VOICEMAIL,
   char     NOANSWER_VOICEMAIL[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_NOANSWER_VOICEMAIL,
   char     NOCONNECT_VOICEMAIL[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_NOCONNECT_VOICEMAIL,
   char     ANYWAY_VOICEMAIL[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_ANYWAY_VOICEMAIL,
   char     WAIT_TEMP_DISABLE[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_WAIT_TEMP_DISABLE,
   char     DND_ENABLE[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_DND_ENABLE,
   char     DND_DISABLE[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_DND_DISABLE,
   char     CNIR_ENABLE[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_CNIR_ENABLE,
   char     CNIR_DISABLE[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_CNIR_DISABLE,
#endif
#ifdef FEATURE_SPORTS_APP   
   uint16 sport_flag;                                    //CFGI_SPORT_FLAG
#endif
#ifdef FEATURE_MENU_STYLE
  byte menu_style;              /*CFGI_MENU_STYLE*/
 #endif
   byte camera_enviroment; /*CFGI_CAMERA_ENVIROMENT*/
   byte camera_quality;    /*CFGI_CAMERA_QUALITY*/
   byte camera_size;       /*CFGI_CAMERA_SIZE*/
   byte camera_tone;       /*CFGI_CAMERA_TONE*/
   byte camera_banding;    /*CFGI_CAMERA_BANDING*/
   byte camera_storage;    /*CFGI_CAMERA_STORAGE*/
   byte camera_brightness; /*CFGI_CAMERA_BRIGHTNESS*/
   byte video_enviroment;  /*CFGI_VIDEO_ENVIROMENT*/ 
   boolean camera_icon;    /*CFGI_CAMERA_ICON_HIDE*/
   boolean video_icon;     /*CFGI_VIDEO_ICON_HIDE*/
   byte camera_frame;      /*CFGI_CAMERA_FRAME*/
   byte camera_color;      /*CFGI_CAMERA_COLOR*/
   byte video_color;       /*CFGI_VIDEO_COLOR*/
   byte video_brightness;  /*CFGI_CAMERA_BRIGHTNESS*/

#ifdef FEATURE_PLANEMODE
   byte planeMode;         /*CFGI_PLANEMODE*/
#endif

#ifdef FEATURE_DS_SIP_MULTIPLE_PROFILE
   byte brew_username[126];                          /* CFGI_BREW_USERNAME       */
   byte brew_password[32];                           /* CFGI_BREW_PASSWORD       */
#else
#ifdef WIN32
   byte brew_username[126];                          /* CFGI_BREW_USERNAME       */
   byte brew_password[32];  
#else
   byte brew_username[NV_MAX_PAP_USER_ID_LENGTH];    /* CFGI_BREW_USERNAME       */
   byte brew_password[NV_MAX_PAP_PASSWORD_LENGTH];   /* CFGI_BREW_PASSWORD       */
#endif   
#endif

#ifdef FEATURE_USB_FUNCTION_SELECT
   OEMUSBFUNCTION usb_function;
#endif //FEATURE_USB_FUNCTION_SELECT

    boolean missed_call_icon;                            /* CFGI_MISSED_CALL_ICON    */
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
   uint32 m_ndefaulerend;                       //CFGI_DEFAULT_REND
   byte m_nrendstate;                      //CFGI_REND_STATE
#endif
#ifdef FEATURE_TOUCHPAD
   pen_cal_type m_pencal_data;
#endif
} OEMConfigListType;


  /* International Mobile Equipment Identity */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#else
#define OEMCFG_IMEI_SIZE 10
#endif


////
// Configuration items managed by the NV subsystem.
typedef struct
{
  ///////////////////////////////////////////////////////////////////////
  // Sound
  //////////////////////////////////////////////////////////////////////
#ifdef FEATURE_SMART_SOUND
  boolean  smart_sound;                /*  CFGI_SMART_SOUND */
#endif
  byte  ringer_level;                  /* CFGI_RINGER_VOL */
  byte  handset_ear_level;             /* CFGI_EAR_VOL */
  byte  beep_level;                    /* CFGI_BEEP_VOL */
  byte  ringer_type;                   /* CFGI_RINGER_TYPE */
  byte  key_sound;                     /* CFGI_KEYBEEP_SOUND */


  ////////////////////////////////////////////////////////////////
  // Display
  ////////////////////////////////////////////////////////////////
  byte contrast;                       /* CFGI_CONTRAST_LVL */
  byte backlight;                      /* CFGI_BACK_LIGHT */
  nv_language_enum_type language;      /* CFGI_LANGUAGE_SELECTION */
  AECHAR banner[NV_MAX_LTRS+1];        /* CFGI_BANNER */
  byte menu_format;                    /* CFGI_MENU_TYPE */

  ////////////////////////////////////////////////////////////////
  // Security
  ////////////////////////////////////////////////////////////////
  byte          lockcode[NV_LOCK_CODE_SIZE];  // CFGI_LOCKCODE

  ////////////////////////////////////////////////////////////////
  // ETC
  ////////////////////////////////////////////////////////////////
  dword   all_call_time;               /* CFGI_ALL_CALL_TIMER */
  byte    backlight_hfk;               /* CFGI_EXTPWR_BK_LIGHT */
  byte    svc_alert;                   /* CFGI_SVC_ALERT */
  boolean amr;                         /*CFGI_AMR*/
  boolean cdma_rx_diversity_ctrl;      /*CFGI_CDMA_RX_DIVERSITY_CTL*/
  boolean hdr_rx_diversity_ctrl;       /*HDR_CDMA_RX_DIVERSITY_CTL*/
  OEMTTY tty;
  uint32  display_duration;            /* CFGI_LCD_TIMER */

  ////////////////////////////////////////////////////////////////
  // Data Call Counters
  ////////////////////////////////////////////////////////////////
  uint32 last_rx_data_count;           /* CFGI_LAST_RX_DATA_COUNT */
  uint32 last_tx_data_count;           /* CFGI_LAST_TX_DATA_COUNT */
  uint32 total_rx_data_count;          /* CFGI_TOTAL_RX_DATA_COUNT */
  uint32 total_tx_data_count;          /* CFGI_TOTAL_TX_DATA_COUNT */
  byte set_time_format;                /* CFGI_TIME_FORMAT*/
#ifdef FEATURE_TIME_DATA_SETTING  
  byte set_date_format;                /* CFGI_DATE_FORMAT*/
#endif
  byte lock;                           /*CFGI_PHONE_PASSWORD_CHECK*/
  nv_auto_redial_type auto_redial;     /*CFGI_AUTO_REDIAL*/
  byte voice_priv;                     /*CFGI_VOICE_PRIVACY*/
  keyToneLength key_tone_length;       /* CFGI_KEYTONE_LENGTH*/
  byte brew_server[NV_BREW_SERVER_SIZ];             /* CFGI_BREW_SERVER         */
  dword brew_carrier_id;                            /* CFGI_BREW_CARRIER_ID     */
  byte brew_bkey[NV_BREW_BKEY_SIZ];                 /* CFGI_BREW_BKEY           */
  dword brew_auth_policy;                           /* CFGI_BREW_AUTH_POLICY    */
  dword brew_privacy_policy;                        /* CFGI_BREW_PRIVACY_POLICY */
  byte brew_subscriber_id[NV_BREW_SID_SIZ];         /* CFGI_BREW_SUBSCRIBER_ID  */
  dword brew_platform_id;                           /* CFGI_BREW_PLATFORM_ID  */
  word brew_download_flags;                         /* Flgs                     */
} NVConfigListType;

////
// GetConfig/SetConfig function pointer.  Unlike the 
// OEM_GetConfig()/OEM_SetConfig() functions, this function does
// not have an item parameter nor a size parameter.
//
// The item parameter is not required because there is a 1-1 mapping
// between config item and config function.  
//
// The size parameter is not required because the size check is performed
// before calling this function (and variable sized config items are 
// not supported).
// 
typedef int (*PFNCFGITEMOP)(void *pBuff);

////
// OEM Config Item Table entry
typedef struct _ConfigItemTableEntry {
   AEEConfigItem item;  // the item id
   PFNCFGITEMOP  get;   // GetConfig function
   PFNCFGITEMOP  set;   // SetConfig function
   int32         size;  // data size of this item (note that variable sized 
                        //                         items are not supported)
} ConfigItemTableEntry;


////
// OEM Config Table information
typedef struct _ConfigTableInfo {
   ConfigItemTableEntry const *tbl;  // pointer to the table
   uint32                      size; // number of elements in the table
} ConfigTableInfo;


typedef struct {
   char *num_buf;
   byte  num_len;
} EmergencyNumber;

static boolean cfgi_lcd;   //variable that will turn on or off the lcd

/*===========================================================================

                     STATIC FUNCTION PROTOTYPES

===========================================================================*/


// Number-classification callback functions
static void OEMPriv_WriteOEMConfigList(void);
static int OEMPriv_ReadOEMConfigList(void);


// OEM Config Item Get/Set functions
static int OEMPriv_GetItem_CFGI_SILENCEALL(void *pBuff);
static int OEMPriv_SetItem_CFGI_SILENCEALL(void *pBuff);
static int OEMPriv_GetItem_CFGI_RECENT_MT_CALL_TIMER(void *pBuff);
static int OEMPriv_SetItem_CFGI_RECENT_MT_CALL_TIMER(void *pBuff);
static int OEMPriv_GetItem_CFGI_RECENT_MO_CALL_TIMER(void *pBuff);
static int OEMPriv_SetItem_CFGI_RECENT_MO_CALL_TIMER(void *pBuff);
static int OEMPriv_GetItem_CFGI_ALL_CALL_TIMER(void *pBuff);
static int OEMPriv_SetItem_CFGI_ALL_CALL_TIMER(void *pBuff);
static int OEMPriv_GetItem_CFGI_BROWSER_CALL_TIMER(void *pBuff);
static int OEMPriv_SetItem_CFGI_BROWSER_CALL_TIMER(void *pBuff);
static int OEMPriv_GetItem_CFGI_EXTPWR_BK_LIGHT(void *pBuff);
static int OEMPriv_SetItem_CFGI_EXTPWR_BK_LIGHT(void *pBuff);
static int OEMPriv_GetItem_CFGI_HEADSET_RNG(void *pBuff);
static int OEMPriv_SetItem_CFGI_HEADSET_RNG(void *pBuff);
static int OEMPriv_GetItem_CFGI_ANYKEY_ANSWER(void *pBuff);
static int OEMPriv_SetItem_CFGI_ANYKEY_ANSWER(void *pBuff);
static int OEMPriv_GetItem_CFGI_HEADSET_AUTOANSWER(void *pBuff);
static int OEMPriv_SetItem_CFGI_HEADSET_AUTOANSWER(void *pBuff);
static int OEMPriv_GetItem_CFGI_MISSED_CALL_ALERT(void *pBuff);
static int OEMPriv_SetItem_CFGI_MISSED_CALL_ALERT(void *pBuff);
static int OEMPriv_GetItem_CFGI_ROAMING_ALERT(void *pBuff);
static int OEMPriv_SetItem_CFGI_ROAMING_ALERT(void *pBuff);
static int OEMPriv_GetItem_CFGI_SVC_ALERT(void *pBuff);
static int OEMPriv_SetItem_CFGI_SVC_ALERT(void *pBuff);
static int OEMPriv_GetItem_CFGI_INTERNET_ALERT(void *pBuff);
static int OEMPriv_SetItem_CFGI_INTERNET_ALERT(void *pBuff);
static int OEMPriv_GetItem_CFGI_MINUTE_ALERT(void *pBuff);
static int OEMPriv_SetItem_CFGI_MINUTE_ALERT(void *pBuff);
static int OEMPriv_GetItem_CFGI_RINGER_VOL(void *pBuff);
static int OEMPriv_SetItem_CFGI_RINGER_VOL(void *pBuff);
static int OEMPriv_GetItem_CFGI_EAR_VOL(void *pBuff);
static int OEMPriv_SetItem_CFGI_EAR_VOL(void *pBuff);
static int OEMPriv_GetItem_CFGI_BEEP_VOL(void *pBuff);
static int OEMPriv_SetItem_CFGI_BEEP_VOL(void *pBuff);
static int OEMPriv_GetItem_CFGI_ALERT_TYPE(void *pBuff);
static int OEMPriv_SetItem_CFGI_ALERT_TYPE(void *pBuff);
#ifdef FEATURE_SMART_SOUND
static int OEMPriv_GetItem_CFGI_SMART_SOUND(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMART_SOUND(void *pBuff);
#endif
static int OEMPriv_GetItem_CFGI_KEYBEEP_SOUND(void *pBuff);
static int OEMPriv_SetItem_CFGI_KEYBEEP_SOUND(void *pBuff);
static int OEMPriv_GetItem_CFGI_RINGER_TYPE(void *pBuff);
static int OEMPriv_SetItem_CFGI_RINGER_TYPE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CONTRAST_LVL(void *pBuff);
static int OEMPriv_SetItem_CFGI_CONTRAST_LVL(void *pBuff);
static int OEMPriv_GetItem_CFGI_BACK_LIGHT(void *pBuff);
static int OEMPriv_SetItem_CFGI_BACK_LIGHT(void *pBuff);
static int OEMPriv_GetItem_CFGI_LANGUAGE_SELECTION(void *pBuff);
static int OEMPriv_SetItem_CFGI_LANGUAGE_SELECTION(void *pBuff);
static int OEMPriv_GetItem_CFGI_MENU_FORMAT(void *pBuff);
static int OEMPriv_SetItem_CFGI_MENU_FORMAT(void *pBuff);
static int OEMPriv_GetItem_CFGI_TIME_FORMAT(void *pBuff);
static int OEMPriv_SetItem_CFGI_TIME_FORMAT(void *pBuff);
#ifdef FEATURE_TIME_DATA_SETTING
static int OEMPriv_GetItem_CFGI_DATE_FORMAT(void *pBuff);
static int OEMPriv_SetItem_CFGI_DATE_FORMAT(void *pBuff);
#endif
static int OEMPriv_GetItem_CFGI_IMEI(void *pBuff);
static int OEMPriv_SetItem_CFGI_IMEI(void *pBuff);
static int OEMPriv_GetItem_CFGI_CUG(void *pBuff);
static int OEMPriv_SetItem_CFGI_CUG(void *pBuff);
static int OEMPriv_GetItem_CFGI_VR(void *pBuff);
static int OEMPriv_SetItem_CFGI_VR(void *pBuff);
static int OEMPriv_GetItem_CFGI_AMR(void *pBuff);
static int OEMPriv_SetItem_CFGI_AMR(void *pBuff);
static int OEMPriv_GetItem_CFGI_TTY(void *pBuff);
static int OEMPriv_SetItem_CFGI_TTY(void *pBuff);
static int OEMPriv_GetItem_CFGI_LAST_RX_DATA_COUNT(void *pBuff);
static int OEMPriv_SetItem_CFGI_LAST_RX_DATA_COUNT(void *pBuff);
static int OEMPriv_GetItem_CFGI_LAST_TX_DATA_COUNT(void *pBuff);
static int OEMPriv_SetItem_CFGI_LAST_TX_DATA_COUNT(void *pBuff);
static int OEMPriv_GetItem_CFGI_TOTAL_RX_DATA_COUNT(void *pBuff);
static int OEMPriv_SetItem_CFGI_TOTAL_RX_DATA_COUNT(void *pBuff);
static int OEMPriv_GetItem_CFGI_TOTAL_TX_DATA_COUNT(void *pBuff);
static int OEMPriv_SetItem_CFGI_TOTAL_TX_DATA_COUNT(void *pBuff);
static int OEMPriv_GetItem_CFGI_CDMA_RX_DIVERSITY_CTL(void *pBuff);
static int OEMPriv_SetItem_CFGI_CDMA_RX_DIVERSITY_CTL(void *pBuff);
static int OEMPriv_GetItem_CFGI_HDR_RX_DIVERSITY_CTL(void *pBuff);
static int OEMPriv_SetItem_CFGI_HDR_RX_DIVERSITY_CTL(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_SERVICE_OPTION(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_SERVICE_OPTION(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_TIMESTAMP(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_TIMESTAMP(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_IS41_WORKAROUND(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_IS41_WORKAROUND(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_MO_ENCODING(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_MO_ENCODING(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_MODE(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_MODE(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_APP(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_APP(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_CB_AUTO_DELETE(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_CB_AUTO_DELETE(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_CB_SILENT_ALERT(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_CB_SILENT_ALERT(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_CB_LANG_FILTER(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_CB_LANG_FILTER(void *pBuff);

static int OEMPriv_GetItem_CFGI_SMS_ALERT_DISPLAY(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_ALERT_DISPLAY(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_ALERT_SOUND(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_ALERT_SOUND(void *pBuff);

static int OEMPriv_GetItem_CFGI_SMS_CDMA_DEFAULT_TEMPLATE(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_CDMA_DEFAULT_TEMPLATE(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_GW_DEFAULT_TEMPLATE(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_GW_DEFAULT_TEMPLATE(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_SEGMENT_SETTING(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_SEGMENT_SETTING(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_SEGMENT_REF_NUM(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_SEGMENT_REF_NUM(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_RETRY_PERIOD(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_RETRY_PERIOD(void *pBuff);

static int OEMPriv_GetItem_CFGI_SECCODE(void *pBuff);
static int OEMPriv_SetItem_CFGI_SECCODE(void *pBuff);
#ifdef FEATURE_ENABLE_OTKSL
static int OEMPriv_GetItem_CFGI_OTKSLCODE(void *pBuff);
static int OEMPriv_SetItem_CFGI_OTKSLCODE(void *pBuff);
#endif /* FEATURE_ENABLE_OTKSL */
static int OEMPriv_GetItem_CFGI_ESN(void *pBuff);
static int OEMPriv_GetItem_CFGI_RFCAL_VERSION(void *pBuff);
static int OEMPriv_GetItem_CFGI_RFCAL_DATE(void *pBuff);
static int OEMPriv_GetItem_CFGI_SLOTINDEX(void *pBuff);
static int OEMPriv_SetItem_CFGI_SLOTINDEX(void *pBuff);
static int OEMPriv_GetItem_CFGI_HOME_SIDNID_LIST(void *pBuff);
static int OEMPriv_SetItem_CFGI_HOME_SIDNID_LIST(void *pBuff);
static int OEMPriv_GetItem_CFGI_LOCK_SIDNID_LIST(void *pBuff);
static int OEMPriv_SetItem_CFGI_LOCK_SIDNID_LIST(void *pBuff);
static int OEMPriv_GetItem_CFGI_IMSI_MCC(void *pBuff);
static int OEMPriv_SetItem_CFGI_IMSI_MCC(void *pBuff);
static int OEMPriv_GetItem_CFGI_IMSI_11_12(void *pBuff);
static int OEMPriv_SetItem_CFGI_IMSI_11_12(void *pBuff);
#ifdef FATRUE_LOCK_IMSI_MCCMNC
static int OEMPriv_GetItem_CFGI_IMSI_SETMCC(void *pBuff);
static int OEMPriv_SetItem_CFGI_IMSI_SETMCC(void *pBuff);
#endif
static int OEMPriv_GetItem_CFGI_IMSI_S(void *pBuff);
static int OEMPriv_SetItem_CFGI_IMSI_S(void *pBuff);
static int OEMPriv_GetItem_CFGI_PRL_ENABLED(void *pBuff);
static int OEMPriv_SetItem_CFGI_PRL_ENABLED(void *pBuff);
static int OEMPriv_GetItem_CFGI_AOC(void *pBuff);
static int OEMPriv_SetItem_CFGI_AOC(void *pBuff);
static int OEMPriv_GetItem_CFGI_HOME_SID_REG(void *pBuff);
static int OEMPriv_SetItem_CFGI_HOME_SID_REG(void *pBuff);
static int OEMPriv_GetItem_CFGI_FORN_SID_REG(void *pBuff);
static int OEMPriv_SetItem_CFGI_FORN_SID_REG(void *pBuff);
static int OEMPriv_GetItem_CFGI_FORN_NID_REG(void *pBuff);
static int OEMPriv_SetItem_CFGI_FORN_NID_REG(void *pBuff);
#ifdef FEATURE_ACP
static int OEMPriv_GetItem_CFGI_MODE_PREF(void *pBuff);
static int OEMPriv_SetItem_CFGI_MODE_PREF(void *pBuff);
static int OEMPriv_GetItem_CFGI_AMPS_HOME_SID(void *pBuff);
static int OEMPriv_SetItem_CFGI_AMPS_HOME_SID(void *pBuff);
static int OEMPriv_GetItem_CFGI_AMPS_FIRSTCHP(void *pBuff);
static int OEMPriv_SetItem_CFGI_AMPS_FIRSTCHP(void *pBuff);
static int OEMPriv_GetItem_CFGI_AMPS_REG_TYPE(void *pBuff);
static int OEMPriv_SetItem_CFGI_AMPS_REG_TYPE(void *pBuff);
#endif /* FEATURE_ACP */
static int OEMPriv_GetItem_CFGI_DATA_QNC_ENABLED(void *pBuff);
static int OEMPriv_SetItem_CFGI_DATA_QNC_ENABLED(void *pBuff);
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
static int OEMPriv_GetItem_CFGI_RTRE_CONFIGURATION(void *pBuff);
static int OEMPriv_SetItem_CFGI_RTRE_CONFIGURATION(void *pBuff);
#endif

#ifdef FEATURE_FDN
static int OEMPriv_GetItem_CFGI_FDN_ENABLED(void *pBuff);
static int OEMPriv_SetItem_CFGI_FDN_ENABLED(void *pBuff);
#endif /* FEATURE_FDN */
static int OEMPriv_GetItem_CFGI_HEADSET(void *pBuff);
static int OEMPriv_SetItem_CFGI_HEADSET(void *pBuff);
static int OEMPriv_GetItem_CFGI_STEREO_HEADSET(void *pBuff);
static int OEMPriv_SetItem_CFGI_STEREO_HEADSET(void *pBuff);
static int OEMPriv_GetItem_CFGI_HFK(void *pBuff);
static int OEMPriv_SetItem_CFGI_HFK(void *pBuff);
static int OEMPriv_GetItem_CFGI_USB_HFK(void *pBuff);
static int OEMPriv_SetItem_CFGI_USB_HFK(void *pBuff);
static int OEMPriv_GetItem_CFGI_SDAC(void *pBuff);
static int OEMPriv_SetItem_CFGI_SDAC(void *pBuff);
static int OEMPriv_GetItem_CFGI_EXT_PWR(void *pBuff);
static int OEMPriv_SetItem_CFGI_EXT_PWR(void *pBuff);
static int OEMPriv_GetItem_CFGI_SPEAKERPHONE(void *pBuff);
static int OEMPriv_SetItem_CFGI_SPEAKERPHONE(void *pBuff);

static int OEMPriv_GetItem_CFGI_SHAKE_MUSIC_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_SHAKE_MUSIC_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_SHAKE_FM_RADIO_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_SHAKE_FM_RADIO_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_SHAKE_WALLPAPER_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_SHAKE_WALLPAPER_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_SHAKE_SNOOZE_ALARM_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_SHAKE_SNOOZE_ALARM_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_SHAKE_VIDEO_CHECK (void *pBuff);
static int OEMPriv_SetItem_CFGI_SHAKE_VIDEO_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_PEDOMETER_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_PEDOMETER_CHECK(void *pBuff);
static int OEMPriv_GetItem_CFGI_EMERGENCYNUM_TABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_EMERGENCYNUM_TABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_BUSY_ENABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_BUSY_ENABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_BUSY_DISABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_BUSY_DISABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOANSWER_ENABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOANSWER_ENABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOANSWER_DISABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOANSWER_DISABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOCONNECT_ENABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOCONNECT_ENABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOCONNECT_DISABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOCONNECT_DISABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_ANYWAY_ENABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_ANYWAY_ENABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_ANYWAY_DISABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_ANYWAY_DISABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_DISABLE_ALL(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_DISABLE_ALL(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_WAIT_ENABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_WAIT_ENABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_WAIT_DISABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_WAIT_DISABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_VOICEMAIL_ENABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_VOICEMAIL_ENABLE(void *pBuff);
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
static int OEMPriv_GetItem_CFGI_CALLFORWARD_BUSY_VOICEMAIL(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_BUSY_VOICEMAIL(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOANSWER_VOICEMAIL(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOANSWER_VOICEMAIL(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOCONNECT_VOICEMAIL(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOCONNECT_VOICEMAIL(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_ANYWAY_VOICEMAIL(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_ANYWAY_VOICEMAIL(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_WAIT_TEMP_DISABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_WAIT_TEMP_DISABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_DND_ENABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_DND_ENABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_DND_DISABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_DND_DISABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_CNIR_ENABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_CNIR_ENABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_CNIR_DISABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_CNIR_DISABLE(void *pBuff);
#endif
static int OEMPriv_GetItem_CFGI_SERVICE_PROVIDER(void *pBuff);
static int OEMPriv_SetItem_CFGI_SERVICE_PROVIDER(void *pBuff);
static int OEMPriv_SetItem_CFGI_INPUTMODE(void *pBuff);
static int OEMPriv_GetItem_CFGI_INPUTMODE(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEVICE(void *pBuff);
static int OEMPriv_GetItem_CFGI_GSENSOR(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_CHANNEL(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_PILOT_PN(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_TX_AGC(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_TX_AGC_IN_DBM(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_RX_AGC(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_RX_AGC_IN_DBM(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_SID(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_NID(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_TX_ADJUST(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_HDET(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_VBATT(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_ERRLOG(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_LNA_STATE(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_PA_STATE(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_RATCHET_STATE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CDMA_DYNAMIC_RANGE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CDMA_MIN_RX_RSSI(void *pBuff);
#ifdef FEATURE_FACTORY_TESTMODE
static int OEMPriv_GetItem_CFGI_FACTORY_TEST_MODE(void *pBuff);
#endif /*FEATURE_FACTORY_TESTMODE*/
static int OEMPriv_GetItem_CFGI_DEBUG_THERM(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_BAND(void *pBuff);

// Generic item is read-only function
static int OEMPriv_SetItem_ReadOnly(void *pBuff);

#ifdef T_MSM6500
//#error code not present
#endif 

static int OEMPriv_GetItem_CFGI_LCD(void *pBuff);
static int OEMPriv_SetItem_CFGI_LCD(void *pBuff);
static int OEMPriv_GetItem_CFGI_LCD_TIMER(void *pBuff);
static int OEMPriv_SetItem_CFGI_LCD_TIMER(void *pBuff);
static int OEMPriv_GetItem_CFGI_PHONE_PASSWORD(void *pBuff);
static int OEMPriv_SetItem_CFGI_PHONE_PASSWORD(void *pBuff);
static int OEMPriv_GetItem_CFGI_PHONE_PASSWORD_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_PHONE_PASSWORD_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_RESTRICT_OUTGOING(void *pBuff);
static int OEMPriv_GetItem_CFGI_RESTRICT_OUTGOING(void *pBuff);
static int OEMPriv_SetItem_CFGI_RESTRICT_INCOMING(void *pBuff);
static int OEMPriv_GetItem_CFGI_RESTRICT_INCOMING(void *pBuff);
//情景模式
static int OEMPriv_GetItem_CFGI_PROFILE_CUR_NUMBER(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_CUR_NUMBER(void *pBuff);
   
static int OEMPriv_GetItem_CFGI_PROFILE_ALERT_TYPE(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_ALERT_TYPE(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_SMS_RINGER(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_SMS_RINGER(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_CALL_RINGER(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_CALL_RINGER(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_ALARM_RINGER(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_ALARM_RINGER(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_SMS_RINGER_ID(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_SMS_RINGER_ID(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_STARTUP_MUSIC(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_STARTUP_MUSIC(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_SHUTDOWN_MUSIC(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_SHUTDOWN_MUSIC(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_POWERONOFF_ALERT(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_POWERONOFF_ALERT(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_MISSED_CALL_ALERT(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_MISSED_CALL_ALERT(void *pBuff);
   
//static int OEMPriv_GetItem_CFGI_PROFILE_HEADSET_AUTOANSWER(void *pBuff);
//static int OEMPriv_SetItem_CFGI_PROFILE_HEADSET_AUTOANSWER(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_KEYSND_TYPE(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_KEYSND_TYPE(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_KEYTONE_LENGTH(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_KEYTONE_LENGTH(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_RINGER_VOL(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_RINGER_VOL(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_EAR_VOL(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_EAR_VOL(void *pBuff);

static int OEMPriv_GetItem_CFGI_PROFILE_BEEP_VOL(void *pBuff);
static int OEMPriv_SetItem_CFGI_PROFILE_BEEP_VOL(void *pBuff);
//短信铃声(0-表示无)
static int OEMPriv_SetItem_CFGI_SMS_RINGER(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_RINGER(void *pBuff);
//闹钟铃声
static int OEMPriv_GetItem_CFGI_ALARM_RINGER(void *pBuff);
static int OEMPriv_SetItem_CFGI_ALARM_RINGER(void *pBuff);

//短信铃声
static int OEMPriv_GetItem_CFGI_SMS_RINGER_ID(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_RINGER_ID(void *pBuff);
static int OEMPriv_GetItem_CFGI_HEADSET_PRESENT(void *pBuff);
//多彩按键音
static int OEMPriv_GetItem_CFGI_KEYSND_TYPE(void *pBuff);
static int OEMPriv_SetItem_CFGI_KEYSND_TYPE(void *pBuff);
//语音加密
static int OEMPriv_GetItem_CFGI_VOICE_PRIVACY(void *pBuff);
static int OEMPriv_SetItem_CFGI_VOICE_PRIVACY(void *pBuff);
static int OEMPriv_GetItem_CFGI_SCREENSAVER_TIME(void *pBuff);
static int OEMPriv_SetItem_CFGI_SCREENSAVER_TIME(void *pBuff);
//背光亮度级别
static int OEMPriv_GetItem_CFGI_BACKLIGHT_LEVEL(void *pBuff);
static int OEMPriv_SetItem_CFGI_BACKLIGHT_LEVEL(void *pBuff);
#ifdef FEATURE_SET_AUTOKEYGUARD
static int OEMPriv_GetItem_CFGI_AUTOKEYGUARD(void *pBuff);
static int OEMPriv_SetItem_CFGI_AUTOKEYGUARD(void *pBuff);
#endif
static int OEMPriv_GetItem_CFGI_KEYTONE_LENGTH(void *pBuff);
static int OEMPriv_SetItem_CFGI_KEYTONE_LENGTH(void *pBuff);

//static int OEMPriv_GetItem_CFGI_KEYPAD_LIGHT_TIME(void *pBuff);
//static int OEMPriv_SetItem_CFGI_KEYPAD_LIGHT_TIME(void *pBuff);

static int OEMPriv_GetItem_CFGI_PHONEBOOK_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_PHONEBOOK_LOCK_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_RECENTCALL_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_RECENTCALL_LOCK_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_SMS_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_LOCK_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_CALENDAR_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALENDAR_LOCK_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_KEY_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_KEY_LOCK_CHECK(void *pBuff);
static int OEMPriv_GetItem_CFGI_LOCK_RUIM(void *pBuff);
static int OEMPriv_SetItem_CFGI_LOCK_RUIM(void *pBuff);

static int OEMPriv_GetItem_CFGI_LOCK_MCCMNC_LIST(void *pBuff);
static int OEMPriv_SetItem_CFGI_LOCK_MCCMNC_LIST(void *pBuff);

static int OEMPriv_GetItem_CFGI_DESKTOP_THEME(void *pBuff);
static int OEMPriv_SetItem_CFGI_DESKTOP_THEME(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_PRIORITY(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_PRIORITY(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_SENDMODE(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_SENDMODE(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_STORETYPE(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_STORETYPE(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_DELIVERYREPORTS(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_DELIVERYREPORTS(void *pBuff);

#ifdef FEATURE_AUTOREPLACE
static int OEMPriv_GetItem_CFGI_WMS_AUTOREPLACE(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_AUTOREPLACE(void *pBuff);
#endif

static int OEMPriv_GetItem_CFGI_WMS_VALIDITYPERIOD(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_VALIDITYPERIOD(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_MEMSTORE(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_MEMSTORE(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_RESVDMSGALERT_TIMEIOUT(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_RESVDMSGALERT_TIMEIOUT(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_CALLBACKNUMSWITCH(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_CALLBACKNUMSWITCH(void *pBuff);

static int OEMPriv_GetItem_CFGI_CALLBACKNUM(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLBACKNUM(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_MO_CHANNEL_SELECT(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_MO_CHANNEL_SELECT(void *pBuff);

#ifdef FEATRUE_AUTO_POWER
static int OEMPriv_GetItem_CFGI_AUTO_POWER_ON(void *pBuff);
static int OEMPriv_SetItem_CFGI_AUTO_POWER_ON(void *pBuff);

static int OEMPriv_GetItem_CFGI_AUTO_POWER_OFF(void *pBuff);
static int OEMPriv_SetItem_CFGI_AUTO_POWER_OFF(void *pBuff);
#endif

static int OEMPriv_GetItem_CFGI_POWERDOWN_ALARM(void *pBuff);
static int OEMPriv_SetItem_CFGI_POWERDOWN_ALARM(void *pBuff);

#ifdef FEATRUE_SET_IP_NUMBER
static int OEMPriv_GetItem_CFGI_IP_NUMBER(void *pBuff);
static int OEMPriv_SetItem_CFGI_IP_NUMBER(void *pBuff);
static int OEMPriv_GetItem_CFGI_IP_POUND(void *pBuff);
static int OEMPriv_SetItem_CFGI_IP_POUND(void *pBuff);
#endif

#ifdef FEATURE_SHORT_CODE_NAM_COUNT
static int OEMPriv_GetItem_CFGI_SHORT_CODE_NAM_COUNT(void *pBuff);
static int OEMPriv_SetItem_CFGI_SHORT_CODE_NAM_COUNT(void *pBuff);
#endif //#ifdef FEATURE_SHORT_CODE_NAM_COUNT

#ifdef FEATURE_LED_CONTROL
static int OEMPriv_GetItem_CFGI_LED_CONTROL(void *pBuff);
static int OEMPriv_SetItem_CFGI_LED_CONTROL(void *pBuff);
#endif

#ifdef FEATRUE_KEY_PAD_CTL
static int OEMPriv_GetItem_CFGI_KEY_PAD_CTL(void *pBuff);
static int OEMPriv_SetItem_CFGI_KEY_PAD_CTL(void *pBuff);
#endif

static int OEMPriv_GetItem_CFGI_AUTO_REDIAL(void *pBuff);
static int OEMPriv_SetItem_CFGI_AUTO_REDIAL(void *pBuff);

static int OEMPriv_GetItem_CFGI_FMRADIO_VOLUME(void *pBuff);
static int OEMPriv_SetItem_CFGI_FMRADIO_VOLUME(void *pBuff);

static int OEMPriv_GetItem_CFGI_FMRADIO_CHAN_INFO(void *pBuff);
static int OEMPriv_SetItem_CFGI_FMRADIO_CHAN_INFO(void *pBuff);

static int OEMPriv_GetItem_CFGI_FMRADIO_CHAN_TOTAL(void *pBuff);
static int OEMPriv_SetItem_CFGI_FMRADIO_CHAN_TOTAL(void *pBuff);
static int OEMPriv_GetItem_CFGI_KEYGUARD_ENABLED(void *pBuff);
static int OEMPriv_SetItem_CFGI_KEYGUARD_ENABLED(void *pBuff);

static int OEMPriv_GetItem_CFGI_FM_BACKGROUND(void *pBuff) ;
static int OEMPriv_SetItem_CFGI_FM_BACKGROUND(void *pBuff) ;
#ifdef FEATURE_RANDOM_MENU_COLOR
static int OEMPriv_GetItem_CFGI_MENU_BGCOLOR(void *pBuff);

static int OEMPriv_SetItem_CFGI_MENU_BGCOLOR(void *pBuff);
static int OEMPriv_GetItem_CFGI_RANDOM_MENU(void *pBuff);

static int OEMPriv_SetItem_CFGI_RANDOM_MENU(void *pBuff);
#endif

#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add 
static int OEMPriv_GetItem_CFGI_DEFAULT_REND(void *pBuff);
static int OEMPriv_SetItem_CFGI_DEFAULT_REND(void *pBuff);
static int OEMPriv_GetItem_CFGI_REND_STATE(void *pBuff);
static int OEMPriv_SetItem_CFGI_REND_STATE(void *pBuff);
#endif//FEATURE_RANDOM_MENU_REND

#ifdef FEATURE_TOUCHPAD
static int OEMPriv_GetItem_CFGI_PEN_CAL(void *pBuff);
static int OEMPriv_SetItem_CFGI_PEN_CAL(void *pBuff);
#endif//FEATURE_TOUCHPAD

static AEECMPrivacyPref OEM_PrivacyPrefToAEECM(cm_privacy_pref_e_type pref) ;
static cm_privacy_pref_e_type OEM_PrivacyPrefToDMSS(AEECMPrivacyPref pref) ;
static int OEMPriv_GetItem_CFGI_DEBUG_ECIO(void *pBuff);
static int OEMPriv_GetItem_CFGI_ALARM_FLAG(void *pBuff) ;
static int OEMPriv_SetItem_CFGI_ALARM_FLAG(void *pBuff) ;

#ifdef FEATURE_SPORTS_APP
static int OEMPriv_GetItem_CFGI_SPORT_FLAG(void *pBuff) ;
static int OEMPriv_SetItem_CFGI_SPORT_FLAG(void *pBuff) ;
#endif


#ifdef FEATURE_MENU_STYLE
static int OEMPriv_GetItem_CFGI_MENU_STYLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_MENU_STYLE(void *pBuff);
#endif

static int OEMPriv_GetItem_CFGI_CAMERA_ENVIROMENT(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_ENVIROMENT(void *pBuff);

static int OEMPriv_GetItem_CFGI_CAMERA_QUALITY(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_QUALITY(void *pBuff);

static int OEMPriv_GetItem_CFGI_CAMERA_SIZE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_SIZE(void *pBuff);

static int OEMPriv_GetItem_CFGI_CAMERA_TONE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_TONE(void *pBuff);

static int OEMPriv_GetItem_CFGI_CAMERA_BANDING(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_BANDING(void *pBuff);

static int OEMPriv_GetItem_CFGI_CAMERA_STORAGE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_STORAGE(void *pBuff);

static int OEMPriv_GetItem_CFGI_CAMERA_BRIGHTNESS(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_BRIGHTNESS(void *pBuff);

static int OEMPriv_GetItem_CFGI_VIDEO_ENVIROMENT(void *pBuff);
static int OEMPriv_SetItem_CFGI_VIDEO_ENVIROMENT(void *pBuff);

static int OEMPriv_GetItem_CFGI_CAMERA_ICON(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_ICON(void *pBuff);

static int OEMPriv_GetItem_CFGI_VIDEO_ICON(void *pBuff);
static int OEMPriv_SetItem_CFGI_VIDEO_ICON(void *pBuff);

static int OEMPriv_GetItem_CFGI_CAMERA_FRAME(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_FRAME(void *pBuff);

static int OEMPriv_GetItem_CFGI_CAMERA_COLOR(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_COLOR(void *pBuff);

static int OEMPriv_GetItem_CFGI_VIDEO_COLOR(void *pBuff);
static int OEMPriv_SetItem_CFGI_VIDEO_COLOR(void *pBuff);
static int OEMPriv_GetItem_CFGI_VIDEO_BRIGHTNESS(void *pBuff);
static int OEMPriv_SetItem_CFGI_VIDEO_BRIGHTNESS(void *pBuff);
static void OEM_InitDefaultBREWParameters(void);
static void OEM_InitBREWParameters(void);
static void OEM_StoreDownloadInfo(AEEDownloadInfo * pdi);

#ifdef FEATURE_PLANEMODE
static int OEMPriv_GetItem_CFGI_PLANEMODE(void *pBuff);
static int OEMPriv_SetItem_CFGI_PLANEMODE(void *pBuff);
#endif

#ifdef FEATURE_USB_FUNCTION_SELECT
static int OEMPriv_GetItem_CFGI_USB_FUNCTION(void *pBuff);
static int OEMPriv_SetItem_CFGI_USB_FUNCTION(void *pBuff);
#endif //FEATURE_USB_FUNCTION_SELECT

static int OEMPriv_GetItem_CFGI_MISSED_CALL_ICON(void *pBuff);
static int OEMPriv_SetItem_CFGI_MISSED_CALL_ICON(void *pBuff);
/*===========================================================================

                     STATIC/LOCAL DATA

===========================================================================*/

static AEECallback   gCBWriteOEMConfig;   

////
// Cache of OEM configuration items (initialized with defaults)
//
// NOTE: Any changes made to these values should be mirrored in 
//       OEM_RestoreFactorySettings()
static OEMConfigListType oemi_cache = {
#ifdef  FEATURE_CARRIER_THAILAND_CAT     
   OEMNV_ALERTTYPE_VIBANDRINGER,               // CFGI_ALERT_TYPE
#else
   OEMNV_ALERTTYPE_RINGER,                         // CFGI_ALERT_TYPE
#endif
   OEMNV_SA_NORMAL,                                // CFGI_SILENCEALL
   OEMNV_HEADSET_RNG_ON,                           // CFGI_HEADSET_RNG
   OEMNV_ALERT_ENABLE,                             // CFGI_MISSED_CALL_ALERT
   OEMNV_ALERT_ENABLE,                             // CFGI_ROAMING_ALERT
   OEMNV_ALERT_ENABLE,                             // CFGI_INTERNET_ALERT
   0,                                              // CFGI_BROWSER_CALL_TIMER
   OEMNV_SMS_SO_SO6,			                         // CFGI_SMS_SERVICE_OPTION
   OEMNV_SMS_EMAIL_ADDRESSING_IS41,                // CFGI_SMS_IS41_WORKAROUND  
#if defined(FEATURE_CARRIER_ANGOLA_MOVICEL) || defined(FEATURE_CARRIER_MAROC_WANA) || defined (FEATURE_CARRIER_ISRAEL_PELEPHONE)
   OEMNV_SMS_MO_ENCODING_OCTET,
#else   
   OEMNV_SMS_MO_ENCODING_7BIT,                     // CFGI_SMS_MO_ENCODING
#endif
   OEMNV_SMS_MODE_CDMA,                            // CFGI_SMS_MODE
   OEMNV_SMS_APP_TEST,                             // CFGI_SMS_APP
   OEMNV_SMS_CB_DISABLE_AUTO_DELETE,               // CFGI_SMS_CB_AUTO_DELETE
   OEMNV_SMS_CB_DISABLE_SILENT_ALERT,              // CFGI_SMS_CB_SILENT_ALERT
   OEMNV_SMS_CB_DISABLE_LANG_FILTER,               // CFGI_SMS_CB_LANG_FILTER
   OEMNV_SMS_ALERT_DISPLAY_ENABLE,                 // CFGI_SMS_ALERT_DISPLAY
   OEMNV_SMS_ALERT_SOUND_ENABLE,                   // CFGI_SMS_ALERT_SOUND
   OEMNV_SMS_ALERT_VOLUME_2,                       // CFGI_SMS_ALERT_VOLUME
   0,                                              // CFGI_SMS_CDMA_DEFAULT_TEMPLATE 
   0,                                              // CFGI_SMS_GW_DEFAULT_TEMPLATE
   0,                                              // CFGI_SMS_SEGMENT_SETTING
   0,                                              // CFGI_SMS_SEGMENT_REF_NUM
   0,                                              // CFGI_SMS_RETRY_PERIOD 
   {0,},                                           // CFGI_VOICEMAIL_NUMBER
   {0,},                                           // CFGI_VOICEMAIL_NUMBER_CDMA
   {0,},                                           // CFGI_VOICEMAIL_NUMBER_GW
   0,                                              // CFGI_RECENT_MT_CALL_TIMER
   0,                                              // CFGI_RECENT_MO_TIMER
   OEMNV_ALERT_DISABLE,                            // CFGI_MINUTE_ALERT
   OEM_AUTO_ANSWER_MODE,                           // CFGI_ANYKEY_ANSWER
   OEMNV_HEADSET_AUTOANSWER_OFF,                   // CFGI_HEADSET_AUTOANSWER
   //   OEMNV_TIMEFORM_AMPM,                       // CFGI_TIME_FORMAT   
   //#ifdef FEATURE_TIME_DATA_SETTING
   //   OEMNV_DATEFORM_YMD,                        //CFGI_DATE_FORMAT
   //#endif
   0,                                              // CFGI_DEVICE
    0,                                              //CFGI_GSENSOR
   {0,},                                           // CFGI_CUG
   FALSE,                                          // CFGI_VR
   
   OEMNV_PHONE_PASSWORD,                            // CFGI_PHONE_PASSWORD,        //type = uint16
   //OEMNV_PHONE_PASSWORD_CHECK,                    // CFGI_PHONE_PASSWORD_CHECK,  //type = boolean
   0,                                               // CFGI_RESTRICT_OUTGOING,     //type = byte
   0,                                               // CFGI_RESTRICT_INCOMING,     //type = byte
   //情景模式      
   OEMNV_PROFILE_NORMALMODE,                        // CFGI_PROFILE_CUR_NUMBER
   OEMNV_ALERT_TYPE_INIT,                           // CFGI_PROFILE_ALERT_TYPE 
   OEMNV_SMS_RINGER_INIT,                           // CFGI_PROFILE_SMS_RINGER 
   OEMNV_CALL_RINGER_INIT,                          // CFGI_PROFILE_CALL_RINGER 
   OEMNV_ALARM_RINGER_INIT,                         // CFGI_PROFILE_ALARM_RINGER 
   OEMNV_SMS_RINGER_ID_INIT,                        // CFGI_PROFILE_SMS_RINGER_ID    
   OEMNV_STARTUP_MUSIC_INIT,                        // CFGI_PROFILE_STARTUP_MUSIC 
   OEMNV_SHUTDOWN_MUSIC_INIT,                       // CFGI_PROFILE_SHUTDOWN_MUSIC 
   OEMNV_POWERONOFF_ALERT_INIT,                     // CFGI_PROFILE_POWERONOFF_ALERT   
   OEMNV_MISSED_CALL_ALERT_INIT,                    // CFGI_PROFILE_MISSED_CALL_ALERT 
   //OEMNV_HEADSET_AUTOANSWER_INIT,                 // CFGI_PROFILE_HEADSET_AUTOANSWER 
   OEMNV_KEYSND_TYPE_INIT,                          // CFGI_PROFILE_KEYSND_TYPE 
   OEMNV_KEYTONE_LENGTH_INIT,                       // CFGI_PROFILE_KEYTONE_LENGTH     
   OEMNV_RINGER_VOL_INIT,                           // CFGI_PROFILE_RINGER_VOL 
   OEMNV_EAR_VOL_INIT ,                             // CFGI_PROFILE_EAR_VOL 
   OEMNV_BEEP_VOL_INIT,                             // CFGI_PROFILE_BEEP_VOL   
   //短信铃声
   OEMNV_SMS_RING,                                  //CFGI_SMS_RINGER 
   //闹钟铃声   
   OEMNV_ALARM_RINGER,                              //CFGI_ALARM_RINGER     
   //短信铃声
   OEMNV_SMS_RINGER_ID ,                            //CFGI_SMS_RINGER_ID
   //多彩按键音
   OEMNV_KEYSND_DEFAULT,                            //CFGI_KEYSND_TYPE
   //屏保时间
   OEMNV_SCREENSAVER_TIME_2MIN,                     // CFGI_SCREENSAVER_TIME
   //桌面墙纸
   {OEMNV_WALLPAPER},                               //CFGI_WALLPAPER   
#ifdef FEATURE_ANIMATION_POWERUPDOWN
   //开机动画
   {OEMNV_STARTUPANI},                              //CFGI_STARTUP_ANIMATION
   
   //关机动画   
   {OEMNV_POWEROFFANI},                             //CFGI_POWEROFF_ANIMATION 
#endif
   //背光亮度级别
   OEMNV_BACKLIGHT_LEVEL,                           //CFGI_BACKLIGHT_LEVEL  
   
//#ifdef FEATURE_KEYTONE_LONG
//   OEMNV_KEYTONE_LONG,                            // CFGI_KEYTONE_LENGTH
//#else
//   OEMNV_KEYTONE_NORMAL,                          // CFGI_KEYTONE_LENGTH
//#endif /* FEATURE_KEYTONE_LONG */
#ifdef FEATURE_SET_AUTOKEYGUARD
   OEMNV_AKG_OFF,                                   // CFGI_AUTOKEYGUARD
#endif
   //OEMNV_BL_10S,                                  //CFGI_KEYPAD_LIGHT_TIME
   FALSE,                                           //CFGI_PHONEBOOK_LOCK_CHECK
   FALSE,                                           //CFGI_RECENTCALL_LOCK_CHECK
   FALSE,                                           //CFGI_SMS_LOCK_CHECK
   FALSE,                                           //CFGI_CALENDAR_LOCK_CHECK
   FALSE,                                           //CFGI_KEY_LOCK_CHECK
   OEMNV_LOCK_RUIM,                                 // CFGI_LOCK_RUIM,   //type = boolean
   {0,},
#ifdef FEATURE_CARRIER_THAILAND_HUTCH   
   DESKTOP_THEME_DEEP_BLUE,
#else
   DESKTOP_THEME_ORANGE, 
#endif //FEATURE_CARRIER_THAILAND_HUTCH
#ifdef FEATURE_SCREEN_SAVE
   OEMNV_SCREENSAVE_TYPE,                           //CFGI_SCREENSAVE_TYPE
#endif
   WMS_PRIORITY_NORMAL,                             //CFGI_WMS_PRIORITY
   SENDOPT_SAVEANDSEND,                             //CFGI_WMS_SENDMODE
   WMS_MEMORY_STORE_NV_CDMA,                        //CFGI_WMS_STORETYPE
   FALSE                                            //CFGI_WMS_DELIVERYREPORTS
#ifdef FEATURE_AUTOREPLACE
   ,FALSE                                           //CFGI_WMS_AUTOREPLACE
#endif
   ,OEMNV_SMS_VALIDITYPERIOD_MAX                    //CFGI_WMS_VALIDITYPERIOD
#if defined(FEATURE_CARRIER_THAILAND_HUTCH)
   ,WMS_MEMORY_STORE_RUIM                       
 #else
   ,WMS_MEMORY_STORE_NV_CDMA                        //CFGI_WMS_MEMSTORE
 #endif //defined FEATURE_CARRIER_THAILAND_HUTCH
   ,TRUE                                           //CFGI_WMS_RESVDMSGALERT_TIMEIOUT
   ,FALSE                                           //CFGI_WMS_CALLBACKNUMSWITCH
   ,{0,}                                            //CFGI_CALLBACKNUM
   ,WMS_MO_ONLY_TRAFFIC                             //CFGI_WMS_MO_CHANNEL_SELECT
#ifdef FEATRUE_AUTO_POWER
   ,{FALSE,0}                                       //CFGI_AUTO_POWER_ON
   ,{FALSE,0}                                       //CFGI_AUTO_POWER_OFF
#endif
   ,{FALSE,0}
#ifdef FEATRUE_SET_IP_NUMBER
   ,{0,0,{"0"}}                                     //CFGI_IP_NUMBER
#endif
#ifdef FEATURE_LED_CONTROL
   , 0xF                                            //CFGI_LED_CONTROL
#endif
#ifdef FEATRUE_KEY_PAD_CTL
   ,{TRUE,KEY_PDA_CTL_FROM_TIME,KEY_PDA_CTL_TO_TIME}
#endif
   

   , (MAX_FMRADIO_VOLUME + 1) / 2                   //CFGI_FMRADIO_VOLUME
   , {0}                                            //CFGI_FMRADIO_CHAN_INFO
   , 0                                              //CFGI_FMRADIO_CHAN_TOTAL
   ,OEMNV_EMERGENCYNUM_TABLE_NUM                                            //CFGI_EMERGENCYNUM_TABLE
   ,{OEMNV_CALLFORWARD_BUSY_ENABLE}                                         //CFGI_CALLFORWARD_BUSY_ENABLE
   ,{OEMNV_CALLFORWARD_BUSY_DISABLE}                                        //CFGI_CALLFORWARD_BUSY_DISABLE
   ,{OEMNV_CALLFORWARD_NOANSWER_ENABLE}                                     //CFGI_CALLFORWARD_NOANSWER_ENABLE
   ,{OEMNV_CALLFORWARD_NOANSWER_DISABLE}                                    //CFGI_CALLFORWARD_NOANSWER_DISABLE
   ,{OEMNV_CALLFORWARD_NOCONNECT_ENABLE}                                    //CFGI_CALLFORWARD_NOCONNECT_ENABLE
   ,{OEMNV_CALLFORWARD_NOCONNECT_DISABLE}                                   //CFGI_CALLFORWARD_NOCONNECT_DISABLE
   ,{OEMNV_CALLFORWARD_ANYWAY_ENABLE}                                       //CFGI_CALLFORWARD_ANYWAY_ENABLE
   ,{OEMNV_CALLFORWARD_ANYWAY_DISABLE}                                      //CFGI_CALLFORWARD_ANYWAY_DISABLE
   ,{OEMNV_CALLFORWARD_DISABLE_ALL}                                         //CFGI_CALLFORWARD_DISABLE_ALL
   ,{OEMNV_CALLFORWARD_WAIT_ENABLE}                                         //CFGI_CALLFORWARD_WAIT_ENABLE
   ,{OEMNV_CALLFORWARD_WAIT_DISABLE}                                        //CFGI_CALLFORWARD_WAIT_DISABLE
   ,{OEMNV_CALLFORWARD_VOICEMAIL_ENABLE}                                    //CFGI_CALLFORWARD_VOICEMAIL_ENABLE
   ,OEMNV_SERVICE_PROVIDER                                                  //CFGI_SERVICE_PROVIDER   
#ifdef FATRUE_LOCK_IMSI_MCCMNC
    ,{0}                                           //CFGI_IMSI_SETMCC
#endif
   ,OEMNV_INPUTMODE_DEFAULT                        //CFGI_INPUTMODE     
   ,FALSE                                          //CFGI_FM_BACKGROUND
#ifdef FEATURE_RANDOM_MENU_COLOR
   ,APPSCOMMON_BG_COLOR  //CFGI_MENU_BGCOLOR
   ,0  //CFGI_RANDOM_MENU
#endif
#ifdef FEATRUE_SET_IP_NUMBER
   ,TRUE                                           //CFGI_IP_POUND
#endif
#ifdef FEATURE_SHORT_CODE_NAM_COUNT
   ,OEMNV_SHORT_CODE_NAM_COUNT          //CFGI_SHORT_CODE_NAM_COUNT
#endif //#ifdef FEATURE_SHORT_CODE_NAM_COUNT
   ,FALSE                                   //CFGI_ALARM_FLAG
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
   ,{""}//CFGI_CALLFORWARD_BUSY_VOICEMAIL,
   ,{""}//CFGI_CALLFORWARD_NOANSWER_VOICEMAIL,
   ,{""}//CFGI_CALLFORWARD_NOCONNECT_VOICEMAIL,
   ,{""}//CFGI_CALLFORWARD_ANYWAY_VOICEMAIL,
   ,{""}//CFGI_CALLFORWARD_WAIT_TEMP_DISABLE,
   ,{""}//CFGI_CALLFORWARD_DND_ENABLE,
   ,{""}//CFGI_CALLFORWARD_DND_DISABLE,
   ,{""}//CFGI_CALLFORWARD_CNIR_ENABLE,
   ,{""}//CFGI_CALLFORWARD_CNIR_DISABLE,
#endif
   	#ifdef FEATURE_SPORTS_APP
	,0                                      //CFGI_SPORT_FLAG
	#endif
   ,OEMNV_MENU_STYLE_ICON

   ,OEMNV_CAMERA_ENVIR_AUTO         /*CFGI_CAMERA_ENVIROMENT*/
   ,OEMNV_CAMERA_QUALITY_HIGH       /*CFGI_CAMERA_QUALITY*/
   ,OEMNV_CAMERA_SIZE_176_220         /*CFGI_CAMERA_SIZE*/ 	//   ,OEMNV_CAMERA_SIZE_640_480         /*CFGI_CAMERA_SIZE*/
#ifdef FEATURE_CARRIER_CHINA_TELCOM
   ,OEMNV_CAMERA_SHUTTER_TONE_SHUTTER1/*CFGI_CAMERA_TONE*/
#else   
   ,OEMNV_CAMERA_SHUTTER_TONE_ENABLE        /*CFGI_CAMERA_TONE*/
#endif   
   ,OEMNV_CAMERA_BANDING_50HZ      /*CFGI_CAMERA_BANDING*/
   ,OEMNV_CAMERA_STORAGE_MEMORY_CARD/*CFGI_VIDEO_STORAGE*/ 
   ,OEMNV_CAMERA_BRIGHTNESS_LEVEL3  /*CFGI_CAMERA_BRIGHTNESS*/
   ,OEMNV_CAMERA_ENVIR_AUTO          /*CFGI_VIDEO_ENVIROMENT*/
   ,FALSE                           /*CFGI_CAMERA_ICON*/
   ,FALSE                           /*CFGI_VIDEO_ICON*/
   ,OEMNV_CAMERA_FRAME_0            /*CFGI_CAMERA_FRAME*/
   ,OEMNV_CAMERA_COLOR_NORMAL       /*CFGI_CAMERA_COLOR*/
   ,OEMNV_CAMERA_COLOR_NORMAL       /*CFGI_VIDEO_COLOR*/
   ,OEMNV_CAMERA_BRIGHTNESS_LEVEL3  /*CFGI_VIDEO_BRIGHTNESS*/
#ifdef FEATURE_PLANEMODE
    ,OEMNV_PLANEMODE_OFF/*CFGI_PLANEMODE*/
#endif
   ,{DEFAULT_BREW_USERNAME}
   ,{DEFAULT_BREW_PASSWORD}

#ifdef FEATURE_USB_FUNCTION_SELECT
   ,OEMNV_USB_FUNCTION_DEFAULT
#endif //FEATURE_USB_FUNCTION_SELECT
   ,FALSE
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
   ,0  //APPSCOMMON_DEFAULT_REND  //CFGI_DEFAULT_REND
   ,2  //CFGI_REND_STATE 界面效果转换状态，0 不加效果,1 选定效果,2 随机效果   
#endif
#ifdef FEATURE_TOUCHPAD
   ,{-1,-1,-1,-1}
#endif//FEATURE_TOUCHPAD

};

////
// Cache of NV configuration items (defaults are supplied by OEMNV.c)
static NVConfigListType nvi_cache;

////
// Config table for the custom config items.  
//
// IMPORTANT: 
// The config items in this table MUST be in numeric sequence.  In 
// other words, the expression 
//
//       customItemTable[n].item == customItemTable[n-1].item + 1 
//
// must hold for all n = 1...ARR_SIZE(customItemTable)-1
//
static ConfigItemTableEntry const customItemTable[] =
{
   //////////////////////////////////////////////////////////////////////
   // Setting Menu
   //////////////////////////////////////////////////////////////////////
   CFGTABLEITEM(CFGI_SILENCEALL, sizeof(byte)),
#ifdef FEATURE_SET_AUTOKEYGUARD
   //CFGTABLEITEM_EMPTY(CFGI_AUTOKEYGUARD),
   CFGTABLEITEM(CFGI_AUTOKEYGUARD, sizeof(byte)),
#endif
   CFGTABLEITEM(CFGI_RECENT_MT_CALL_TIMER, sizeof(dword)),
   CFGTABLEITEM(CFGI_RECENT_MO_CALL_TIMER, sizeof(dword)),
   CFGTABLEITEM(CFGI_ALL_CALL_TIMER, sizeof(dword)),
   CFGTABLEITEM(CFGI_BROWSER_CALL_TIMER, sizeof(dword)),
   CFGTABLEITEM(CFGI_EXTPWR_BK_LIGHT, sizeof(byte)),
   CFGTABLEITEM(CFGI_HEADSET_RNG, sizeof(byte)),
   CFGTABLEITEM(CFGI_ANYKEY_ANSWER, sizeof(byte)),
   CFGTABLEITEM(CFGI_HEADSET_AUTOANSWER, sizeof(byte)),
   CFGTABLEITEM(CFGI_MISSED_CALL_ALERT, sizeof(byte)),
   CFGTABLEITEM(CFGI_ROAMING_ALERT, sizeof(byte)),
   CFGTABLEITEM(CFGI_SVC_ALERT, sizeof(byte)),
   CFGTABLEITEM(CFGI_INTERNET_ALERT, sizeof(byte)),
   CFGTABLEITEM(CFGI_MINUTE_ALERT, sizeof(byte)),
   CFGTABLEITEM(CFGI_VOICE_PRIVACY,sizeof(byte)),
#ifdef FEATRUE_AUTO_POWER   
   CFGTABLEITEM(CFGI_AUTO_POWER_ON, sizeof(Auto_Power_Cfg)),       //type = Auto_Power_Cfg
   CFGTABLEITEM(CFGI_AUTO_POWER_OFF, sizeof(Auto_Power_Cfg)) ,      //type = Auto_Power_Cfg
#endif

   CFGTABLEITEM(CFGI_POWERDOWN_ALARM, sizeof(PowerDown_Alarm_Cfg)), 
   CFGTABLEITEM(CFGI_ALARM_FLAG, sizeof(boolean)),
#ifdef FEATURE_SPORTS_APP
	CFGTABLEITEM(CFGI_SPORT_FLAG,sizeof(uint16)),
#endif

#ifdef FEATRUE_SET_IP_NUMBER
   CFGTABLEITEM(CFGI_IP_NUMBER, sizeof(IP_Number_Cfg)),
#endif
   CFGTABLEITEM(CFGI_AUTO_REDIAL, sizeof(nv_auto_redial_type)),
   ////////////////////////////////////////////////////////////////
   // Sound Menu
   ////////////////////////////////////////////////////////////////

   CFGTABLEITEM(CFGI_RINGER_VOL, sizeof(byte)),
   CFGTABLEITEM(CFGI_EAR_VOL, sizeof(byte)),
   CFGTABLEITEM(CFGI_BEEP_VOL, sizeof(byte)),
   CFGTABLEITEM(CFGI_ALERT_TYPE, sizeof(byte)),
#ifdef FEATURE_SMART_SOUND
   CFGTABLEITEM(CFGI_SMART_SOUND, sizeof(boolean)),
#endif
   
   CFGTABLEITEM(CFGI_KEYBEEP_SOUND, sizeof(byte)),
   //CFGTABLEITEM_EMPTY(CFGI_KEYTONE_LENGTH),
   CFGTABLEITEM(CFGI_KEYTONE_LENGTH, sizeof(keyToneLength)),
   CFGTABLEITEM(CFGI_RINGER_TYPE, sizeof(byte)),
   
      //短信铃声(0-表示无)
   CFGTABLEITEM(CFGI_SMS_RINGER, sizeof(byte)) ,             // type=byte    
   //闹钟铃声,
   CFGTABLEITEM(CFGI_ALARM_RINGER, sizeof(uint16))  ,        
   
   //短信铃声
   CFGTABLEITEM(CFGI_SMS_RINGER_ID, sizeof(uint16)) ,
   //情景模式         
   //情景模式类型
   CFGTABLEITEM(CFGI_PROFILE_CUR_NUMBER, sizeof(oemi_cache.p_profile_cur_number)),
    
   //来电提示方式     
   CFGTABLEITEM(CFGI_PROFILE_ALERT_TYPE, sizeof(oemi_cache.p_alert_type))  ,
   
   //短信提示方式
   CFGTABLEITEM(CFGI_PROFILE_SMS_RINGER, sizeof(oemi_cache.p_bt_sms_ringer)) ,

   //来电铃声
   CFGTABLEITEM(CFGI_PROFILE_CALL_RINGER, sizeof(oemi_cache.p_call_ringer))  ,  
  
   //闹钟铃声
   CFGTABLEITEM(CFGI_PROFILE_ALARM_RINGER, sizeof(oemi_cache.p_alarm_ringer)) ,
   
   //短信铃声
   CFGTABLEITEM(CFGI_PROFILE_SMS_RINGER_ID, sizeof(oemi_cache.p_sms_ringer_id)) ,    
 
   //开机音乐
   CFGTABLEITEM(CFGI_PROFILE_STARTUP_MUSIC, sizeof(oemi_cache.p_start_music)) ,
   
   //关机音乐
   CFGTABLEITEM(CFGI_PROFILE_SHUTDOWN_MUSIC, sizeof(oemi_cache.p_shutdown_music)) ,

   //开关机铃声提示
   CFGTABLEITEM(CFGI_PROFILE_POWERONOFF_ALERT, sizeof(oemi_cache.p_poweronoff_alert)) ,

   //未接来电提醒
   CFGTABLEITEM(CFGI_PROFILE_MISSED_CALL_ALERT, sizeof(oemi_cache.p_missed_call_alert)) ,
       
   //自动接听
   //CFGTABLEITEM(CFGI_PROFILE_HEADSET_AUTOANSWER, sizeof(oemi_cache.p_headset_autoanswer)) ,
   
   //多彩按键音
   CFGTABLEITEM(CFGI_PROFILE_KEYSND_TYPE, sizeof(oemi_cache.p_keysnd_type)) ,
   
   //按键音长度
   CFGTABLEITEM(CFGI_PROFILE_KEYTONE_LENGTH, sizeof(oemi_cache.p_keytone_length)) , 
 
   //铃声音量
   CFGTABLEITEM(CFGI_PROFILE_RINGER_VOL, sizeof(oemi_cache.p_ringer_level)) ,
   
   //耳机音量
   CFGTABLEITEM(CFGI_PROFILE_EAR_VOL, sizeof(oemi_cache.p_handset_ear_level)) ,
   
   //键盘音量
   CFGTABLEITEM(CFGI_PROFILE_BEEP_VOL, sizeof(oemi_cache.p_beep_level)) ,
   //多彩按键音,
   
   CFGTABLEITEM(CFGI_KEYSND_TYPE, sizeof(byte)) ,
   CFGTABLEITEM(CFGI_IMEI, OEMCFG_IMEI_SIZE),
   CFGTABLEITEM(CFGI_CUG, sizeof(OEMConfigCugInfo)),
   CFGTABLEITEM(CFGI_VR, sizeof(boolean)),
   CFGTABLEITEM(CFGI_AMR, sizeof(boolean)),
   CFGTABLEITEM(CFGI_TTY, sizeof(OEMTTY)),
   CFGTABLEITEM(CFGI_LCD, sizeof(uint8)),
   CFGTABLEITEM(CFGI_LCD_TIMER, sizeof(uint32)),
   ////////////////////////////////////////////////////////////////
   // Data Call Counters
   ////////////////////////////////////////////////////////////////
   CFGTABLEITEM(CFGI_LAST_RX_DATA_COUNT, sizeof(uint32)),
   CFGTABLEITEM(CFGI_LAST_TX_DATA_COUNT, sizeof(uint32)),
   CFGTABLEITEM(CFGI_TOTAL_RX_DATA_COUNT, sizeof(uint32)),
   CFGTABLEITEM(CFGI_TOTAL_TX_DATA_COUNT, sizeof(uint32)),

   CFGTABLEITEM(CFGI_CDMA_RX_DIVERSITY_CTL, sizeof(boolean)),
   CFGTABLEITEM(CFGI_HDR_RX_DIVERSITY_CTL, sizeof(boolean)),

   ////////////////////////////////////////////////////////////////
   // Phone Number
   ////////////////////////////////////////////////////////////////
   CFGTABLEITEM_EMPTY(CFGI_PHONE_NUMBER),

   ////////////////////////////////////////////////////////////////
   // Display Menu
   ////////////////////////////////////////////////////////////////

   CFGTABLEITEM(CFGI_CONTRAST_LVL, sizeof(byte)),
   CFGTABLEITEM(CFGI_BACK_LIGHT, sizeof(byte)),

   CFGTABLEITEM(CFGI_LANGUAGE_SELECTION, sizeof(nv_language_enum_type)),
   CFGTABLEITEM_EMPTY(CFGI_BANNER),
   CFGTABLEITEM(CFGI_MENU_FORMAT, sizeof(byte)),
   CFGTABLEITEM(CFGI_TIME_FORMAT, sizeof(byte)),
#ifdef FEATURE_TIME_DATA_SETTING
   CFGTABLEITEM(CFGI_DATE_FORMAT, sizeof(byte)),
#endif
   //屏保时间
   CFGTABLEITEM(CFGI_SCREENSAVER_TIME, sizeof(oemi_cache.p_screensaver_time)) ,
   //桌面墙纸
   CFGTABLEITEM_EMPTY(CFGI_WALLPAPER) ,
#ifdef FEATURE_ANIMATION_POWERUPDOWN
    //开机动画
   CFGTABLEITEM_EMPTY(CFGI_STARTUP_ANIMATION),     
   //关机动画
   CFGTABLEITEM_EMPTY(CFGI_POWEROFF_ANIMATION),   
#endif
   //背光亮度级别 
   CFGTABLEITEM(CFGI_BACKLIGHT_LEVEL, sizeof(byte)) ,
   //CFGTABLEITEM(CFGI_KEYPAD_LIGHT_TIME, sizeof(byte)) ,
   CFGTABLEITEM(CFGI_DESKTOP_THEME, sizeof(byte)) ,
#ifdef FEATURE_SCREEN_SAVE
   CFGTABLEITEM_EMPTY(CFGI_SCREENSAVE_TYPE) ,
#endif
#ifdef FEATURE_LED_CONTROL  
   CFGTABLEITEM(CFGI_LED_CONTROL, sizeof(byte)),
#endif
#ifdef FEATRUE_KEY_PAD_CTL  
   CFGTABLEITEM(CFGI_KEY_PAD_CTL, sizeof(Key_pad_Cfg)),
#endif


   ////////////////////////////////////////////////////////////////
   // SMS Applet
   ////////////////////////////////////////////////////////////////

   CFGTABLEITEM(CFGI_SMS_SERVICE_OPTION, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_TIMESTAMP, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_IS41_WORKAROUND, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_MO_ENCODING, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_MODE, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_APP, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_CB_AUTO_DELETE, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_CB_SILENT_ALERT, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_CB_LANG_FILTER, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_ALERT_DISPLAY, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_ALERT_SOUND, sizeof(byte)),
   CFGTABLEITEM_EMPTY(CFGI_SMS_ALERT_VOLUME),
   CFGTABLEITEM(CFGI_SMS_CDMA_DEFAULT_TEMPLATE, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_GW_DEFAULT_TEMPLATE, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_SEGMENT_SETTING, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_SEGMENT_REF_NUM, sizeof(word)),
   CFGTABLEITEM(CFGI_SMS_RETRY_PERIOD, sizeof(word)),
   CFGTABLEITEM_EMPTY(CFGI_VOICEMAIL_NUMBER),
   CFGTABLEITEM_EMPTY(CFGI_VOICEMAIL_NUMBER_CDMA),
   CFGTABLEITEM_EMPTY(CFGI_VOICEMAIL_NUMBER_GW),
   CFGTABLEITEM(CFGI_WMS_PRIORITY, sizeof(byte)),
   CFGTABLEITEM(CFGI_WMS_SENDMODE, sizeof(byte)),
   CFGTABLEITEM(CFGI_WMS_STORETYPE, sizeof(byte)),
   CFGTABLEITEM(CFGI_WMS_DELIVERYREPORTS, sizeof(boolean)),
#ifdef FEATURE_AUTOREPLACE
   CFGTABLEITEM(CFGI_WMS_AUTOREPLACE, sizeof(boolean)),
#endif   
   CFGTABLEITEM(CFGI_WMS_VALIDITYPERIOD, sizeof(byte)),
   CFGTABLEITEM(CFGI_WMS_MEMSTORE, sizeof(wms_memory_store_e_type)),
   CFGTABLEITEM(CFGI_WMS_RESVDMSGALERT_TIMEIOUT, sizeof(boolean)),
   CFGTABLEITEM(CFGI_WMS_CALLBACKNUMSWITCH, sizeof(boolean)),
   CFGTABLEITEM(CFGI_CALLBACKNUM, sizeof(AECHAR)*WMS_ADDRESS_MAX),
   CFGTABLEITEM(CFGI_WMS_MO_CHANNEL_SELECT, sizeof(byte)),
   
   ////////////////////////////////////////////////////////////////
   // RecentCalls App
   ////////////////////////////////////////////////////////////////
   
   CFGTABLEITEM_EMPTY(CFGI_LASTCALL_TIMER),
   CFGTABLEITEM_EMPTY(CFGI_AIR_TIMER),
   CFGTABLEITEM_EMPTY(CFGI_ROAM_TIMER),
   CFGTABLEITEM_EMPTY(CFGI_LIFE_TIMER),

   ////////////////////////////////////////////////////////////////
   // Service Programming 
   ////////////////////////////////////////////////////////////////

   CFGTABLEITEM(CFGI_SECCODE,OEMNV_SECCODE_LENGTH * sizeof(AECHAR)),
#ifdef FEATURE_ENABLE_OTKSL
   CFGTABLEITEM(CFGI_OTKSLCODE,OEMNV_SECCODE_LENGTH * sizeof(AECHAR)),
#endif
   CFGTABLEITEM_READONLY(CFGI_ESN,sizeof(uint32)),
   CFGTABLEITEM(CFGI_SLOTINDEX,sizeof(uint8)),
   CFGTABLEITEM_EMPTY(CFGI_BUILD_VERSION),
   CFGTABLEITEM_READONLY(CFGI_RFCAL_VERSION, sizeof(AECHAR) * 
                                             OEMNV_RFCALVER_LEN),
   CFGTABLEITEM_READONLY(CFGI_RFCAL_DATE, sizeof(dword)),
   CFGTABLEITEM(CFGI_HOME_SIDNID_LIST,sizeof(AEEConfigSIDNIDPairType) * 
                                      OEMNV_HOME_SIDNID_ARRSIZE),
   CFGTABLEITEM(CFGI_LOCK_SIDNID_LIST,sizeof(AEEConfigSIDNIDPairType) * 
                                      OEMNV_LOCK_SIDNID_ARRSIZE),
   CFGTABLEITEM(CFGI_IMSI_MCC,sizeof(uint16)),
   CFGTABLEITEM(CFGI_IMSI_11_12,sizeof(uint16)),
#ifdef FATRUE_LOCK_IMSI_MCCMNC
   CFGTABLEITEM(CFGI_IMSI_SETMCC,sizeof(SetImsi) * OEMNV_MAX_SERVICEPROVIDER_NUMBER),
#endif
   CFGTABLEITEM(CFGI_IMSI_S,sizeof(AECHAR) * OEMNV_IMSI_S_LENGTH),
   CFGTABLEITEM(CFGI_PRL_ENABLED,sizeof(boolean)),
   CFGTABLEITEM_EMPTY(CFGI_PRI_CH_A),
   CFGTABLEITEM_EMPTY(CFGI_PRI_CH_B),
   CFGTABLEITEM_EMPTY(CFGI_SEC_CH_A),
   CFGTABLEITEM_EMPTY(CFGI_SEC_CH_B),
   CFGTABLEITEM(CFGI_AOC,sizeof(uint8)),
   CFGTABLEITEM(CFGI_HOME_SID_REG,sizeof(boolean)),
   CFGTABLEITEM(CFGI_FORN_SID_REG,sizeof(boolean)),
   CFGTABLEITEM(CFGI_FORN_NID_REG,sizeof(boolean)),
#ifdef FEATURE_ACP
   CFGTABLEITEM(CFGI_MODE_PREF,sizeof(byte)),
   CFGTABLEITEM(CFGI_AMPS_HOME_SID,sizeof(uint16)),
   CFGTABLEITEM(CFGI_AMPS_FIRSTCHP,sizeof(uint16)),
   CFGTABLEITEM(CFGI_AMPS_REG_TYPE,sizeof(byte)),
#endif /* FEATURE_ACP */
   CFGTABLEITEM(CFGI_DATA_QNC_ENABLED,sizeof(boolean)),
   CFGTABLEITEM_EMPTY(CFGI_DATA_DIALSTRING),

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
   CFGTABLEITEM(CFGI_RTRE_CONFIGURATION,sizeof(nv_rtre_configuration_type)),
#endif 

   ////////////////////////////////////////////////////////////////
   // Security Menu
   ////////////////////////////////////////////////////////////////
   CFGTABLEITEM_EMPTY(CFGI_LOCKCODE),
   CFGTABLEITEM(CFGI_PHONE_PASSWORD, sizeof(uint16)) ,       //type = uint16
   CFGTABLEITEM(CFGI_PHONE_PASSWORD_CHECK,  sizeof(byte)),//type = boolean
   CFGTABLEITEM(CFGI_RESTRICT_OUTGOING, sizeof(byte)) ,      //type = byte
   CFGTABLEITEM(CFGI_RESTRICT_INCOMING, sizeof(byte)),       //type = byte
   CFGTABLEITEM(CFGI_PHONEBOOK_LOCK_CHECK, sizeof(boolean)), //type = boolena
   CFGTABLEITEM(CFGI_RECENTCALL_LOCK_CHECK, sizeof(boolean)),//type = boolena
   CFGTABLEITEM(CFGI_SMS_LOCK_CHECK, sizeof(boolean)),       //type = boolena
   CFGTABLEITEM(CFGI_CALENDAR_LOCK_CHECK, sizeof(boolean)),  //type = boolean
   CFGTABLEITEM(CFGI_KEY_LOCK_CHECK, sizeof(boolean)),       //type = boolena
   CFGTABLEITEM(CFGI_LOCK_RUIM,  sizeof(boolean)),            //type = boolean
   CFGTABLEITEM(CFGI_LOCK_MCCMNC_LIST,  sizeof(oemi_cache.lock_mccmnc)), 
#ifdef FEATURE_FDN
   
   CFGTABLEITEM(CFGI_FDN_ENABLED,sizeof(boolean)),
#endif /* FEATURE_FDN */

   CFGTABLEITEM(CFGI_HEADSET,sizeof(boolean)),
   CFGTABLEITEM(CFGI_STEREO_HEADSET,sizeof(boolean)),
   CFGTABLEITEM(CFGI_HFK,sizeof(boolean)),
   CFGTABLEITEM(CFGI_USB_HFK,sizeof(boolean)),
   CFGTABLEITEM(CFGI_SDAC,sizeof(boolean)),
   CFGTABLEITEM(CFGI_EXT_PWR,sizeof(boolean)),
   CFGTABLEITEM(CFGI_SPEAKERPHONE,sizeof(boolean)),
   CFGTABLEITEM(CFGI_SHAKE_MUSIC_CHECK,  sizeof(boolean)),             //type = boolean
   CFGTABLEITEM(CFGI_SHAKE_FM_RADIO_CHECK,  sizeof(boolean)),          //type = boolean
   CFGTABLEITEM(CFGI_SHAKE_WALLPAPER_CHECK,  sizeof(boolean)),         //type = boolean
   CFGTABLEITEM(CFGI_SHAKE_SNOOZE_ALARM_CHECK,  sizeof(boolean)),      //type = boolean
   CFGTABLEITEM(CFGI_SHAKE_VIDEO_CHECK,  sizeof(boolean)),             //type = boolean
   CFGTABLEITEM(CFGI_PEDOMETER_CHECK,  sizeof(boolean)),             //type = boolean
   
   CFGTABLEITEM(CFGI_FMRADIO_VOLUME, sizeof(byte)),
   CFGTABLEITEM(CFGI_FMRADIO_CHAN_INFO, sizeof(sChanInfo) * MAX_FMRADIO_STORED_CHANNEL),
   CFGTABLEITEM(CFGI_FMRADIO_CHAN_TOTAL, sizeof(byte)),
   CFGTABLEITEM(CFGI_EMERGENCYNUM_TABLE,sizeof(EmergencyNum_Table)),
   CFGTABLEITEM(CFGI_CALLFORWARD_BUSY_ENABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_BUSY_DISABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_NOANSWER_ENABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_NOANSWER_DISABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_NOCONNECT_ENABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_NOCONNECT_DISABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_ANYWAY_ENABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_ANYWAY_DISABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_DISABLE_ALL,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_WAIT_ENABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_WAIT_DISABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_VOICEMAIL_ENABLE,FEATURE_CODE_MAX_LENTH),
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
   CFGTABLEITEM(CFGI_CALLFORWARD_BUSY_VOICEMAIL,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_NOANSWER_VOICEMAIL,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_NOCONNECT_VOICEMAIL,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_ANYWAY_VOICEMAIL,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_WAIT_TEMP_DISABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_DND_ENABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_DND_DISABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_CNIR_ENABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_CNIR_DISABLE,FEATURE_CODE_MAX_LENTH),
#endif
   CFGTABLEITEM(CFGI_SERVICE_PROVIDER,sizeof(ServiceProvider)*OEMNV_MAX_SERVICEPROVIDER_NUMBER),
   CFGTABLEITEM(CFGI_INPUTMODE, sizeof(byte))
#ifdef FEATRUE_SET_IP_NUMBER
   ,CFGTABLEITEM(CFGI_IP_POUND, sizeof(boolean))
#endif
#ifdef FEATURE_SHORT_CODE_NAM_COUNT
   ,CFGTABLEITEM(CFGI_SHORT_CODE_NAM_COUNT,   sizeof(byte))  //byte
#endif //#ifdef FEATURE_SHORT_CODE_NAM_COUNT

#ifdef FEATURE_MENU_STYLE
   ,CFGTABLEITEM(CFGI_MENU_STYLE, sizeof(byte))
#endif

   ,CFGTABLEITEM_EMPTY(CFGI_BREW_USERNAME)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_PASSWORD)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_SERVER)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_CARRIER_ID)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_BKEY)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_AUTH_POLICY)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_PRIVACY_POLICY)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_SUBSCRIBER_ID)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_PLATFORM_ID)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_TESTOPT)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_USEAKEY)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_AUTOUPGRADE_FLG)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_USEMINFORSID_FLG)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_PREPAY_FLG)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_NOAUTOACK_FLG)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_SIDENCODE_FLG)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_SIDVALIDATAALL_FLG)
   ,CFGTABLEITEM_EMPTY(CFGI_BREW_IDS_RUIMDELETE_FLG)

   ,CFGTABLEITEM(CFGI_CAMERA_ENVIROMENT, sizeof(byte))
   ,CFGTABLEITEM(CFGI_CAMERA_QUALITY, sizeof(byte))
   ,CFGTABLEITEM(CFGI_CAMERA_SIZE, sizeof(byte))
   ,CFGTABLEITEM(CFGI_CAMERA_TONE, sizeof(byte))
   ,CFGTABLEITEM(CFGI_CAMERA_BANDING, sizeof(byte))
   ,CFGTABLEITEM(CFGI_CAMERA_STORAGE, sizeof(byte))
   ,CFGTABLEITEM(CFGI_CAMERA_BRIGHTNESS, sizeof(byte))
   ,CFGTABLEITEM(CFGI_VIDEO_ENVIROMENT, sizeof(byte))
   ,CFGTABLEITEM(CFGI_CAMERA_ICON, sizeof(boolean))
   ,CFGTABLEITEM(CFGI_VIDEO_ICON, sizeof(boolean))
   ,CFGTABLEITEM(CFGI_CAMERA_FRAME, sizeof(byte))
   ,CFGTABLEITEM(CFGI_CAMERA_COLOR, sizeof(byte))
   ,CFGTABLEITEM(CFGI_VIDEO_COLOR, sizeof(byte))
   ,CFGTABLEITEM(CFGI_VIDEO_BRIGHTNESS, sizeof(byte))
#ifdef FEATURE_PLANEMODE
,CFGTABLEITEM(CFGI_PLANEMODE, sizeof(byte))
#endif

#ifdef FEATURE_USB_FUNCTION_SELECT
,CFGTABLEITEM(CFGI_USB_FUNCTION, sizeof(OEMUSBFUNCTION))
#endif //FEATURE_USB_FUNCTION_SELECT

,CFGTABLEITEM(CFGI_MISSED_CALL_ICON, sizeof(boolean))

};

////
// Config table for the custom config state items.  
//
// IMPORTANT: 
// The config items in this table MUST be in numeric sequence.  In 
// other words, the expression 
//
//       customStateItemTable[n].item == customStateItemTable[n-1].item + 1 
//
// must hold for all n = 1...ARR_SIZE(customStateItemTable)-1
//
static ConfigItemTableEntry const customStateItemTable[] =
{
   CFGTABLEITEM_READONLY(CFGI_DEVICE,sizeof(uint32)),
   CFGTABLEITEM_READONLY(CFGI_GSENSOR,sizeof(uint32)),

   // Debug Menu state items 
   CFGTABLEITEM_READONLY(CFGI_DEBUG_CHANNEL,sizeof(uint16)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_PILOT_PN,sizeof(uint16)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_TX_AGC,sizeof(uint16)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_TX_AGC_IN_DBM,sizeof(int16)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_RX_AGC,sizeof(signed char)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_RX_AGC_IN_DBM,sizeof(int32)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_ECIO,sizeof(byte)),             
   CFGTABLEITEM_READONLY(CFGI_DEBUG_SID,sizeof(uint16)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_NID,sizeof(uint16)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_TX_ADJUST,sizeof(signed char)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_HDET,sizeof(byte)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_VBATT,sizeof(byte)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_ERRLOG,sizeof(OEMErrLogType)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_LNA_STATE,sizeof(byte)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_PA_STATE,sizeof(byte)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_RATCHET_STATE,sizeof(byte)),

#ifdef T_MSM6500
//#error code not present
#endif 

   // Items required for power calculation in Debug Menu
   CFGTABLEITEM_READONLY(CFGI_CDMA_DYNAMIC_RANGE,sizeof(uint16)),
   CFGTABLEITEM_READONLY(CFGI_CDMA_MIN_RX_RSSI,sizeof(uint16)),

#ifdef FEATURE_FACTORY_TESTMODE
   CFGTABLEITEM_READONLY(CFGI_FACTORY_TEST_MODE,sizeof(boolean)),
#endif /*FEATURE_FACTORY_TESTMODE*/
   CFGTABLEITEM_READONLY(CFGI_DEBUG_THERM,sizeof(word)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_BAND,sizeof(byte)),

   CFGTABLEITEM(CFGI_KEYGUARD_ENABLED,sizeof(boolean)),
   CFGTABLEITEM_READONLY(CFGI_HEADSET_PRESENT,sizeof(boolean)),
   CFGTABLEITEM(CFGI_FM_BACKGROUND,sizeof(boolean)),
#ifdef FEATURE_RANDOM_MENU_COLOR
   CFGTABLEITEM(CFGI_MENU_BGCOLOR,sizeof(uint32)),
   CFGTABLEITEM(CFGI_RANDOM_MENU,sizeof(byte)),
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add
   CFGTABLEITEM(CFGI_DEFAULT_REND,sizeof(uint32)),
   CFGTABLEITEM(CFGI_REND_STATE,sizeof(byte)),
#endif//FEATURE_RANDOM_MENU_REND
#ifdef FEATURE_TOUCHPAD
   CFGTABLEITEM(CFGI_PEN_CAL,sizeof(pen_cal_type))
#endif//FEATURE_TOUCHPAD

};

////
//  The table containing all the config tables.
static ConfigTableInfo const cfgTable[] =
{
   { customItemTable,      ARR_SIZE(customItemTable)      },
   { customStateItemTable, ARR_SIZE(customStateItemTable) }
};

/* Cache initialized ? */
static boolean cache_initialized = FALSE;


boolean   OEM_GetCachedLCDSetting()
{
  boolean returnVal;

  if(OEMPriv_GetItem_CFGI_LCD(&returnVal) == SUCCESS)
    return returnVal;
  else
    return TRUE;
}


/*===========================================================================

FUNCTION OEM_IsHeadsetPresent

DESCRIPTION
  Query if headset is currently detected

DEPENDENCIES


RETURN VALUE
  TRUE - if headset is detected
  FALSE - if headset is not detected

SIDE EFFECTS
  None

===========================================================================*/
boolean OEM_IsHeadsetPresent (void)
{
#ifndef WIN32
   return(HS_HEADSET_ON());
#else
	return FALSE;
#endif

}

#ifdef FEATURE_FACTORY_TESTMODE
/*=============================================================================
FUNCTION:  OEM_IsFactoryTestMode

DESCRIPTION: Returns TRUE if the phone is in 'Factory Test Mode'.

PARAMETERS:
   None

RETURN VALUE:
   boolean

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEM_IsFactoryTestMode(void)
{
   return FALSE;
}
#endif /*FEATURE_FACTORY_TESTMODE*/


/*===========================================================================
FUNCTION: OEM_RestoreFactorySetting

DESCRIPTION


RETURN VALUE

DEPENDENCIES

===========================================================================*/
void OEM_RestoreFactorySetting( void )
{
   nv_item_type nvi;

   // Reset the OEM Configuration list
   //
   // NOTE: Any changes to these values should be mirrored in the 
   //       initialization of the oemi_cache variable.
#ifdef FEATURE_KEYGUARD	 
    oemi_cache.b_key_lock       =  FALSE; 
#endif
#ifdef  FEATURE_CARRIER_THAILAND_CAT     
   oemi_cache.alert_type          = OEMNV_ALERTTYPE_VIBANDRINGER;
#else 
   oemi_cache.alert_type          = OEMNV_ALERTTYPE_RINGER;
#endif
   oemi_cache.silence_all         = OEMNV_SA_NORMAL;
   oemi_cache.headset_ringer      = OEMNV_HEADSET_RNG_ON;
   oemi_cache.missed_call_alert   = OEMNV_ALERT_ENABLE;
   oemi_cache.internet_call_alert = OEMNV_ALERT_ENABLE;
   oemi_cache.internet_call_time  = 0;
#ifdef FEATURE_CARRIER_THAILAND_HUTCH   
   oemi_cache.desktop_theme       = DESKTOP_THEME_DEEP_BLUE;
#else
   oemi_cache.desktop_theme       = DESKTOP_THEME_ORANGE;   
#endif //FEATURE_CARRIER_THAILAND_HUTCH
#ifdef FEATRUE_AUTO_POWER
   oemi_cache.m_auto_poweron.bStateOn  = FALSE;
   oemi_cache.m_auto_poweroff.bStateOn = FALSE;
   oemi_cache.m_auto_poweron.dwWATime  = 0;
   oemi_cache.m_auto_poweroff.dwWATime = 0;
#endif
   oemi_cache.powerdown_alarm.bStateOn  = FALSE;
   oemi_cache.powerdown_alarm.alarm_id = 0;
   oemi_cache.powerdown_alarm.dwWATime  = 0;
   oemi_cache.alarm_flag = FALSE;
#ifdef FEATURE_LED_CONTROL
   oemi_cache.m_led_control            = 0xF;
#endif
#ifdef FEATRUE_KEY_PAD_CTL
   oemi_cache.m_key_pad_ctl.bStateOn  = TRUE; 
   oemi_cache.m_key_pad_ctl.from_Time = KEY_PDA_CTL_FROM_TIME;
   oemi_cache.m_key_pad_ctl.to_Time   = KEY_PDA_CTL_TO_TIME;
#endif
#ifdef FEATRUE_SET_IP_NUMBER
   //oemi_cache.m_ip_number.bStateOn     = FALSE;
   //oemi_cache.m_ip_number.ip_count      = 0; 
   //MEMSET((void *)oemi_cache.m_ip_number.Number,0 ,sizeof(oemi_cache.m_ip_number.Number));
   MEMSET((void *)&oemi_cache.m_ip_number, 0 ,sizeof(IP_Number_Cfg));
   oemi_cache.m_b_ip_pound = TRUE;
#endif

   //oemi_cache.keypad_light_time      =OEMNV_BL_10S;
   oemi_cache.b_phonebook_lock         = FALSE;
   oemi_cache.b_recentcall_lock        = FALSE;
   oemi_cache.b_sms_lock               = FALSE;
#if defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
    nvi_cache.key_tone_length = OEMNV_KEYTONE_LONG;
    nvi.cont_key_dtmf = TRUE;
    (void) OEMNV_Put( NV_CONT_KEY_DTMF_I, &nvi);
#else //FEATURE_CARRIER_VENEZUELA_MOVILNET
    nvi_cache.key_tone_length = OEMNV_KEYTONE_NORMAL;
#ifndef WIN32
    nvi.cont_key_dtmf = FALSE;
    (void) OEMNV_Put( NV_CONT_KEY_DTMF_I, &nvi);
#endif
#endif

//#ifdef FEATURE_KEYTONE_LONG
//    nvi_cache.key_tone_length          = OEMNV_KEYTONE_LONG;
//    nvi.cont_key_dtmf = TRUE;
//#else
//    nvi_cache.key_tone_length          = OEMNV_KEYTONE_NORMAL;
//    nvi.cont_key_dtmf = FALSE;
//#endif /* FEATURE_KEYTONE_LONG */
//    OEMNV_Put( NV_CONT_KEY_DTMF_I, &nvi );
#ifdef FEATURE_SET_AUTOKEYGUARD
   oemi_cache.autokey_guard            = OEMNV_AKG_OFF;
#endif
   oemi_cache.roaming_alert            = OEMNV_ALERT_ENABLE;

   oemi_cache.minute_alert             = OEMNV_ALERT_DISABLE;
   oemi_cache.any_key_answer           = OEM_AUTO_ANSWER_MODE;
   oemi_cache.headset_autoanswer       = OEMNV_HEADSET_AUTOANSWER_OFF;
   oemi_cache.phone_password         = OEMNV_PHONE_PASSWORD;
   //oemi_cache.phone_password_check   = OEMNV_PHONE_PASSWORD_CHECK;  //恢复出厂设置不恢复为不检测手机密码
   //短信铃声(0-表示无)
   oemi_cache.bt_sms_ringer = OEMNV_SMS_RING;

   //星期闹钟,
   //oemi_cache.ui16_weekalarm         = OEMNV_WEEKALARM;//not use
   //自动关机,
   //oemi_cache.ui16_autopower         = OEMNV_AUTOPOWER;//not use
   //情景模式,
   //oemi_cache.bt_scene               = OEMNV_NORMALMODE;//not use
#ifdef FEATURE_ANIMATION_POWERUPDOWN
   //开机动画
   MEMCPY((void*)oemi_cache.s_startupani,OEMNV_STARTUPANI, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
   
   //关机动画
   MEMCPY((void*)oemi_cache.s_poweroffani,OEMNV_POWEROFFANI, AEE_MAX_FILE_NAME/*FILESPECLEN*/); 
#endif
   //桌面墙纸
   MEMCPY((void*)oemi_cache.s_wallpaper,OEMNV_WALLPAPER, AEE_MAX_FILE_NAME/*FILESPECLEN*/); 
#ifdef FEATURE_SCREEN_SAVE
   //屏保类型
   MEMCPY((void*)oemi_cache.s_screensave_type,OEMNV_SCREENSAVE_TYPE, AEE_MAX_FILE_NAME/*FILESPECLEN*/); 
#endif
   oemi_cache.wms_priority        = WMS_PRIORITY_NORMAL; 
   oemi_cache.wms_storetype       = WMS_MEMORY_STORE_NV_CDMA; 
   oemi_cache.wms_delivereports   = FALSE; 
   oemi_cache.wms_validityperiod   = OEMNV_SMS_VALIDITYPERIOD_MAX; // 无限制
#if defined(FEATURE_CARRIER_THAILAND_HUTCH)
   oemi_cache.wms_memstore         = WMS_MEMORY_STORE_RUIM; // 存于UIM
#else
   oemi_cache.wms_memstore         = WMS_MEMORY_STORE_NV_CDMA; // 存于话机
#endif //defined FEATURE_CARRIER_THAILAND_HUTCH
   oemi_cache.wms_resvdmsgalert_timeout = FALSE; // 不发送
   oemi_cache.callbacknum_switch = FALSE;
   oemi_cache.callback_number[0] = 0;
   oemi_cache.wms_mochannel = WMS_MO_ONLY_TRAFFIC;
#ifndef WIN32
   wms_msg_setmochannel(oemi_cache.wms_mochannel);
#endif
   oemi_cache.sms_service_option  = OEMNV_SMS_SO_SO6;
   oemi_cache.sms_is41_workaround = OEMNV_SMS_EMAIL_ADDRESSING_IS41;
#if defined(FEATURE_CARRIER_ANGOLA_MOVICEL) || defined(FEATURE_CARRIER_MAROC_WANA) || defined (FEATURE_CARRIER_ISRAEL_PELEPHONE)
   oemi_cache.sms_mo_encoding     = OEMNV_SMS_MO_ENCODING_OCTET;
#else
   oemi_cache.sms_mo_encoding     = OEMNV_SMS_MO_ENCODING_7BIT;
#endif   
   oemi_cache.sms_mode            = OEMNV_SMS_MODE_CDMA;
   oemi_cache.sms_app             = OEMNV_SMS_APP_TEST;
   oemi_cache.sms_cb_auto_delete  = OEMNV_SMS_CB_DISABLE_AUTO_DELETE;
   oemi_cache.sms_cb_silent_alert = OEMNV_SMS_CB_DISABLE_SILENT_ALERT;
   oemi_cache.sms_cb_lang_filter  = OEMNV_SMS_CB_DISABLE_LANG_FILTER;
   oemi_cache.sms_alert_display   = OEMNV_SMS_ALERT_DISPLAY_ENABLE;
   oemi_cache.sms_alert_sound     = OEMNV_SMS_ALERT_SOUND_ENABLE;
   oemi_cache.sms_alert_volume    = OEMNV_SMS_ALERT_VOLUME_2;
   oemi_cache.sms_cdma_default_template = 0;
   oemi_cache.sms_gw_default_template   = 0;  
   oemi_cache.sms_segment_setting       = 0;
   oemi_cache.sms_segment_ref_num       = 0;
   oemi_cache.sms_retry_period          = 0;
   /*
   MEMSET((void *) oemi_cache.voicemail_number, 
          0, 
          sizeof(oemi_cache.voicemail_number));
   */
   {
    AECHAR pBuff[OEMNV_VOICEMAIL_MAXLEN]={0};
    
    (void)STRTOWSTR(OEMNV_VOICEMAIL_NUMBER, pBuff, sizeof(AECHAR)*OEMNV_VOICEMAIL_MAXLEN);    
#ifdef FEATURE_WMS_APP
#ifndef WIN32
    nvi.sms_vm_number.num_digits = WSTRLEN(pBuff);
    WSTRTOSTR(pBuff, (char *)nvi.sms_vm_number.digits, nvi.sms_vm_number.num_digits+1);
#endif
    OEMNV_Put(NV_SMS_VM_NUMBER_I, &nvi);
#else
    WSTRNCOPYN((void *) oemi_cache.voicemail_number,
                sizeof(oemi_cache.voicemail_number)/sizeof(AECHAR),
                (AECHAR*) pBuff,
                -1);
#endif
   }     
   MEMSET((void *) oemi_cache.voicemail_number_cdma, 
          0, 
          sizeof(oemi_cache.voicemail_number_cdma));
   MEMSET((void *) oemi_cache.voicemail_number_gw, 
          0, 
          sizeof(oemi_cache.voicemail_number_gw));
   //   oemi_cache.time_format         = OEMNV_TIMEFORM_AMPM;
   //#ifdef FEATURE_TIME_DATA_SETTING
   //   oemi_cache.date_format         = OEMNV_DATEFORM_YMD;
   //#endif
   //菜单风格  
   //oemi_cache.menu_style = OEMNV_MENUSTYLE; //not use
   //闹钟铃声
   oemi_cache.alarm_ringer    = OEMNV_ALARM_RINGER;  
   
   //短信铃声
   oemi_cache.sms_ringer_id   = OEMNV_SMS_RINGER_ID;  
   //背光亮度级别
   oemi_cache.backlight_level = OEMNV_BACKLIGHT_LEVEL;   
   //pm_lcd_bright_level      = OEMNV_BACKLIGHT_LEVEL;
   //pm_kbd_bright_level      = OEMNV_BACKLIGHT_LEVEL;
   //情景模式类型
   oemi_cache.p_profile_cur_number = OEMNV_PROFILE_NORMALMODE; 

   //来电提示方式 
  {
       byte restore_alert_type[PROFILENUMBER] = OEMNV_ALERT_TYPE_INIT;
       MEMCPY((void *) oemi_cache.p_alert_type,(const void * )restore_alert_type,sizeof(restore_alert_type));
  }
  //短信提示方式
  {  
       byte restore_bt_sms_ringer[PROFILENUMBER] = OEMNV_SMS_RINGER_INIT;
       MEMCPY((void *) oemi_cache.p_bt_sms_ringer,(const void * )restore_bt_sms_ringer,sizeof(restore_bt_sms_ringer));
  }   
   //来电铃声
  {   
       ringID restore_call_ringer[PROFILENUMBER] = OEMNV_CALL_RINGER_INIT;
       MEMCPY((void *) oemi_cache.p_call_ringer,(const void * )restore_call_ringer,sizeof(restore_call_ringer));
  }   
   //闹钟铃声
  {   
       ringID restore_alarm_ringer[PROFILENUMBER] = OEMNV_ALARM_RINGER_INIT;
       MEMCPY((void *) oemi_cache.p_alarm_ringer,(const void * )restore_alarm_ringer,sizeof(restore_alarm_ringer));
  }   
   //短信铃声
  { 
       ringID restore_sms_ringer_id[PROFILENUMBER] = OEMNV_SMS_RINGER_ID_INIT;
       MEMCPY((void *) oemi_cache.p_sms_ringer_id,(const void * )restore_sms_ringer_id,sizeof(restore_sms_ringer_id));
  } 
    
   //开机音乐
  {
       uint16 restore_start_music[PROFILENUMBER] = OEMNV_STARTUP_MUSIC_INIT;
       MEMCPY((void *) oemi_cache.p_start_music,(const void * )restore_start_music,sizeof(restore_start_music));
  }
   //关机音乐
  {
       uint16 restore_shutdown_music[PROFILENUMBER] = OEMNV_SHUTDOWN_MUSIC_INIT;
       MEMCPY((void *) oemi_cache.p_shutdown_music,(const void * )restore_shutdown_music,sizeof(restore_shutdown_music));  
  }
    
   //开关机铃声提示
  { 
       byte restore_poweronoff_alert[PROFILENUMBER] = OEMNV_POWERONOFF_ALERT_INIT;
       MEMCPY((void *) oemi_cache.p_poweronoff_alert,(const void * )restore_poweronoff_alert,sizeof(restore_poweronoff_alert));
  }
   
   //未接来电提醒
  { 
       byte restore_missed_call_alert[PROFILENUMBER] = OEMNV_MISSED_CALL_ALERT_INIT;
       MEMCPY((void *) oemi_cache.p_missed_call_alert,(const void * )restore_missed_call_alert,sizeof(restore_missed_call_alert));
  }
  //自动接听
  //{
  //     byte restore_headset_autoanswer[PROFILENUMBER] = OEMNV_HEADSET_AUTOANSWER_INIT;
  //     MEMCPY((void *) oemi_cache.p_headset_autoanswer,(const void * )restore_headset_autoanswer,sizeof(restore_headset_autoanswer));
  //}
   //多彩按键音
  { 
       byte restore_keysnd_type[PROFILENUMBER] = OEMNV_KEYSND_TYPE_INIT;
       MEMCPY((void *) oemi_cache.p_keysnd_type,(const void * )restore_keysnd_type,sizeof(restore_keysnd_type));
  }

   //按键音长度
  { 
       byte restore_keytone_length[PROFILENUMBER] = OEMNV_KEYTONE_LENGTH_INIT;
       MEMCPY((void *) oemi_cache.p_keytone_length,(const void * )restore_keytone_length,sizeof(restore_keytone_length));
  }    

   //铃声音量
  {
       byte restore_ringer_level[PROFILENUMBER] = OEMNV_RINGER_VOL_INIT;
       MEMCPY((void *) oemi_cache.p_ringer_level,(const void * )restore_ringer_level,sizeof(restore_ringer_level));
  }
   //耳机音量
  {
       byte restore_handset_ear_level[PROFILENUMBER] = OEMNV_EAR_VOL_INIT;
       MEMCPY((void *) oemi_cache.p_handset_ear_level,(const void * )restore_handset_ear_level,sizeof(restore_handset_ear_level));   
  }
   //键盘音量
  {
       byte restore_beep_level[PROFILENUMBER] = OEMNV_BEEP_VOL_INIT;
       MEMCPY((void *) oemi_cache.p_beep_level,(const void * )restore_beep_level,sizeof(restore_beep_level));       
  }
   //屏保时间
   oemi_cache.p_screensaver_time=0; 
   
   //屏保动画
   //MEMCPY((void*)oemi_cache.s_screen_animation,OEMNV_SCREENPANI, AEE_MAX_FILE_NAME/*FILESPECLEN*/);     
   MEMSET((void *)&oemi_cache.cug, 0, sizeof(OEMConfigCugInfo));

   oemi_cache.vr = 0;

    oemi_cache.restrict_incoming = 0;
    oemi_cache.restrict_outgoing = 0;

   oemi_cache.fmRadio_volume = (MAX_FMRADIO_VOLUME + 1)/2;                                  
   MEMSET((void *)&oemi_cache.fmRadio_chan_info, 0, sizeof(sChanInfo) * MAX_FMRADIO_STORED_CHANNEL);
   oemi_cache.fmRadio_chan_total = 0;
/*
   //guoys add begin 20080723 for oem.ini
   MEMCPY((void*)oemi_cache.BUSY_ENABLE,OEMNV_BUSY_ENABLE,FEATURE_CODE_MAX_LENTH); 
   MEMCPY((void*)oemi_cache.BUSY_DISABLE,OEMNV_BUSY_DISABLE,FEATURE_CODE_MAX_LENTH); 
   MEMCPY((void*)oemi_cache.NOANSWER_ENABLE,OEMNV_NOANSWER_ENABLE,FEATURE_CODE_MAX_LENTH); 
   MEMCPY((void*)oemi_cache.NOANSWER_DISABLE,OEMNV_NOANSWER_DISABLE,FEATURE_CODE_MAX_LENTH); 
   MEMCPY((void*)oemi_cache.NOCONNECT_ENABLE,OEMNV_NOCONNECT_ENABLE,FEATURE_CODE_MAX_LENTH); 
   MEMCPY((void*)oemi_cache.NOCONNECT_DISABLE,OEMNV_NOCONNECT_DISABLE,FEATURE_CODE_MAX_LENTH); 
   MEMCPY((void*)oemi_cache.ANYWAY_ENABLE,OEMNV_ANYWAY_ENABLE,FEATURE_CODE_MAX_LENTH); 
   MEMCPY((void*)oemi_cache.ANYWAY_DISABLE,OEMNV_ANYWAY_DISABLE,FEATURE_CODE_MAX_LENTH); 
   MEMCPY((void*)oemi_cache.DISABLE_ALL,OEMNV_DISABLE_ALL,FEATURE_CODE_MAX_LENTH); 
   MEMCPY((void*)oemi_cache.WAIT_ENABLE,OEMNV_WAIT_ENABLE,FEATURE_CODE_MAX_LENTH); 
   MEMCPY((void*)oemi_cache.WAIT_DISABLE,OEMNV_WAIT_DISABLE,FEATURE_CODE_MAX_LENTH); 
   MEMCPY((void*)oemi_cache.VOICEMAIL_ENABLE,OEMNV_VOICEMAIL_ENABLE,FEATURE_CODE_MAX_LENTH); 

*/
   oemi_cache.input_mode=OEMNV_INPUTMODE_DEFAULT;

   oemi_cache.menu_style= OEMNV_MENU_STYLE_ICON;

   oemi_cache.camera_enviroment = OEMNV_CAMERA_ENVIR_AUTO;
   oemi_cache.camera_quality = OEMNV_CAMERA_QUALITY_HIGH;
   oemi_cache.camera_size = OEMNV_CAMERA_SIZE_176_220;//   oemi_cache.camera_size = OEMNV_CAMERA_SIZE_640_480;
#ifdef FEATURE_CARRIER_CHINA_TELCOM
   oemi_cache.camera_tone = OEMNV_CAMERA_SHUTTER_TONE_SHUTTER1;
#else   
   oemi_cache.camera_tone = OEMNV_CAMERA_SHUTTER_TONE_ENABLE;
#endif
   oemi_cache.camera_banding = OEMNV_CAMERA_BANDING_50HZ;
   oemi_cache.camera_storage = OEMNV_CAMERA_STORAGE_MEMORY_CARD;
   oemi_cache.camera_brightness = OEMNV_CAMERA_BRIGHTNESS_LEVEL3;
   oemi_cache.video_enviroment = OEMNV_CAMERA_ENVIR_AUTO;
   oemi_cache.camera_icon = FALSE;
   oemi_cache.video_icon = FALSE;
   oemi_cache.camera_frame = OEMNV_CAMERA_FRAME_0;
   oemi_cache.camera_color = OEMNV_CAMERA_COLOR_NORMAL;
   oemi_cache.video_color = OEMNV_CAMERA_COLOR_NORMAL;
   oemi_cache.video_brightness = OEMNV_CAMERA_BRIGHTNESS_LEVEL3;

#ifdef FEATURE_PLANEMODE
   oemi_cache.planeMode = OEMNV_PLANEMODE_OFF;
#endif

   oemi_cache.brew_username[0] = 0;
   oemi_cache.brew_password[0] = 0;
   (void)STRCPY((char *)oemi_cache.brew_username, DEFAULT_BREW_USERNAME);
   (void)STRCPY((char *)oemi_cache.brew_password, DEFAULT_BREW_PASSWORD);
   
#ifdef FEATURE_USB_FUNCTION_SELECT
   oemi_cache.usb_function = OEMNV_USB_FUNCTION_DEFAULT;
#endif //FEATURE_USB_FUNCTION_SELECT


   OEMPriv_WriteOEMConfigList();

   // Backlight with external pwr
#ifndef WIN32
   nvi.back_light_hfk = OEMNV_EXTPWR_BL_ON;
   (void) OEMNV_Put( NV_BACK_LIGHT_HFK_I, &nvi );
   nvi_cache.backlight_hfk = OEMNV_EXTPWR_BL_ON;
   
   // LCD display duration
   nvi.display_duration = 0;
   (void) OEMNV_Put( NV_DISPLAY_DURATION_I, &nvi);
   nvi_cache.display_duration = nvi.display_duration;
#if defined(FEATURE_CARRIER_THAILAND_CAT)
   // CFGI_RINGER_VOL:
   nvi.ringer_level = OEMNV_VOLUME_MID;
   (void) OEMNV_Put( NV_RINGER_LVL_I, &nvi );
   nvi_cache.ringer_level = OEMNV_VOLUME_ESCALATING;

   // CFGI_EAR_VOL:
   nvi.ear_level = OEMNV_VOLUME_MID;
   (void) OEMNV_Put( NV_EAR_LVL_I, &nvi );
   nvi_cache.handset_ear_level = OEMNV_VOLUME_ESCALATING;

   // CFGI_BEEP_VOL:
   nvi.beep_level = OEMNV_VOLUME_LOW;
   (void) OEMNV_Put( NV_BEEP_LVL_I, &nvi );
   nvi_cache.beep_level = OEMNV_VOLUME_LOW;
#else
   // CFGI_RINGER_VOL:
   nvi.ringer_level = OEMNV_VOLUME_MID;
   (void) OEMNV_Put( NV_RINGER_LVL_I, &nvi );
   nvi_cache.ringer_level = OEMNV_VOLUME_ESCALATING;

   // CFGI_EAR_VOL:
   nvi.ear_level = OEMNV_VOLUME_MID;
   (void) OEMNV_Put( NV_EAR_LVL_I, &nvi );
   nvi_cache.handset_ear_level = OEMNV_VOLUME_ESCALATING;

   // CFGI_BEEP_VOL:
   nvi.beep_level = OEMNV_VOLUME_MID;
   (void) OEMNV_Put( NV_BEEP_LVL_I, &nvi );
   nvi_cache.beep_level = OEMNV_VOLUME_MID;
#endif //defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT

#ifdef FEATURE_SMART_SOUND
   // CFGI_SMART_SOUND:
   nvi.auto_volume_enabled = OEMNV_SMARTSOUND_DISABLE;
   (void) OEMNV_Put( NV_AUTO_VOLUME_ENABLED_I, &nvi );
   nvi_cache.smart_sound = OEMNV_SMARTSOUND_DISABLE;
   OEMSound_SmartSoundEnable(FALSE);
#endif
   // CFGI_SVC_ALERT:
   nvi.svc_area_alert = OEMNV_ALERT_ENABLE;
   (void) OEMNV_Put( NV_SVC_AREA_ALERT_I, &nvi );
   nvi_cache.svc_alert = OEMNV_ALERT_ENABLE;

    //CFGI_LANGUAGE_SELECTION
    if(nvi_cache.language != (nv_language_enum_type) OEMNV_LANGUAGE_DEFULT)
    {
#if defined(FEATURE_PEKTEST)
        // PEK 测试版软件将语言改为英语
        nvi.language_selection = NV_LANGUAGE_ENGLISH;
        nvi_cache.language = NV_LANGUAGE_ENGLISH;
#else
        nvi.language_selection = (nv_language_enum_type) OEMNV_LANGUAGE_DEFULT;
        nvi_cache.language = (nv_language_enum_type) OEMNV_LANGUAGE_DEFULT;
#endif

        (void) OEMNV_Put( NV_LANGUAGE_SELECTION_I, &nvi );
        (void) AEE_IssueSystemCallback(AEE_SCB_DEVICE_INFO_CHANGED);
    }
   //CFGI_RTRE_CONFIGURATION
   nvi.rtre_config = OEMNV_RTRT_DEFULT_CONFIG;
   (void) OEMNV_Put( NV_RTRE_CONFIG_I, &nvi );

   // CFGI_RINGER_TYPE:
   nvi.ringer_type = (byte) SND_ALERT;
   (void) OEMNV_Put( NV_RINGER_TYPE_I, &nvi );
   nvi_cache.ringer_type = (byte) SND_ALERT;

   // CFGI_KEYBEEP_SOUND:
   nvi.key_sound = (nv_key_sound_enum_type) OEMNV_KEYBEEP_TONE;
   (void) OEMNV_Put( NV_KEY_SOUND_I, &nvi );
   nvi_cache.key_sound = OEMNV_KEYBEEP_TONE;

   // LCD contrast
   nvi.lcd = OEMNV_CONTRAST_LEVEL_4;
   (void) OEMNV_Put( NV_LCD_I, &nvi );
   nvi_cache.contrast = OEMNV_CONTRAST_LEVEL_4;
   //OEMDisp_SetContrast( nvi_cache.contrast );
   
   // Menu Format
   nvi.menu_format = (nv_menu_format_enum_type) OEMNV_MENUFORM_LARGE;
   (void) OEMNV_Put( NV_MENU_FORMAT_I, &nvi );
   nvi_cache.menu_format = (byte) OEMNV_MENUFORM_LARGE;

#if defined(FEATURE_CARRIER_THAILAND_CAT)
   nvi.back_light = OEMNV_BL_30S;
   (void) OEMNV_Put( NV_BACK_LIGHT_I, &nvi );
   nvi_cache.backlight = OEMNV_BL_30S;
#else
   // NV_BACK_LIGHT_I
   //nvi.back_light = OEMNV_BL_10S;
   nvi.back_light = OEMNV_BL_10S;
   (void) OEMNV_Put( NV_BACK_LIGHT_I, &nvi );
   //nvi_cache.backlight = OEMNV_BL_10S;
   nvi_cache.backlight = OEMNV_BL_10S;
#endif //defined FEATURE_CARRIER_THAILAND_CAT

   // Data Call Counters
   nvi.last_rx_data_count = 0;
   (void) OEMNV_Put( NV_LAST_RX_DATA_COUNT_I, &nvi);
   nvi_cache.last_rx_data_count = 0;
    
   nvi.last_tx_data_count = 0;
   (void) OEMNV_Put( NV_LAST_TX_DATA_COUNT_I, &nvi);
   nvi_cache.last_tx_data_count = 0;
    
   nvi.last_rx_data_count = 0;
   (void) OEMNV_Put( NV_TOTAL_RX_DATA_COUNT_I, &nvi);
   nvi_cache.total_rx_data_count = 0;
    
   nvi.total_tx_data_count = 0;
   (void) OEMNV_Put( NV_TOTAL_TX_DATA_COUNT_I, &nvi);
   nvi_cache.total_tx_data_count = 0;  
   
   nvi.set_time_format = NV_SET_TIME_FORMAT_24_HOUR;
   (void) OEMNV_Put( NV_SET_TIME_FORMAT_I, &nvi);
   nvi_cache.set_time_format = (byte)NV_SET_TIME_FORMAT_24_HOUR;
   
#ifdef FEATURE_TIME_DATA_SETTING
    nvi.set_date_format = NV_SET_DATE_FORMAT_DD_MM_YYYY;
    (void) OEMNV_Put( NV_SET_DATE_FORMAT_I, &nvi);
    nvi_cache.set_date_format = (byte)NV_SET_DATE_FORMAT_DD_MM_YYYY;
#endif 
   //CFGI_PHONE_PASSWORD_CHECK
   nvi.lock = 0;
   (void) OEMNV_Put( NV_LOCK_I, &nvi);
   nvi_cache.lock = 0;

   //CFGI_AUTO_REDIAL
   nvi.auto_redial.enable = FALSE;
   nvi.auto_redial.rings    = 10;
   (void) OEMNV_Put( NV_AUTO_REDIAL_I, &nvi);   
   nvi_cache.auto_redial.enable = FALSE;
   nvi_cache.auto_redial.rings    = 10;

#ifdef FEATURE_SID_LOCK
    {
        db_items_value_type sid_lock;
        
        sid_lock.b_sid_lock = 1;
        db_put(DB_SID_LOCK, &sid_lock);
        nvi.enabled_sid_lock.nam = cm_get_curr_nam();
        nvi.enabled_sid_lock.b_sid_lock = 1;
        (void) OEMNV_Put(NV_SID_LOCK_I, &nvi);
    }
#endif //FEATURE_SID_LOCK
   
   //CFGI_VOICE_PRIVACY
   nvi.voice_priv = CM_PRIVACY_PREF_STANDARD;
   (void) OEMNV_Put( NV_VOICE_PRIV_I, &nvi);
   nvi_cache.voice_priv = CM_PRIVACY_PREF_STANDARD;
#endif//#ifndef win32   
   OEM_InitDefaultBREWParameters();
#ifdef FEATURE_SUPPORT_BT_APP
#ifndef WIN32
   bt_ui_restore_set(); 
#endif
#endif
   OEMFS_Remove( ALARM_EFS_FILE);
   {
       extern void ClockApps_ClearAll(void);
       DBGPRINTF( ";restorefactory, remove alarm file success");
#ifndef WIN32//wlh 临时修改
       ClockApps_ClearAll();
#endif//WIN32
   }
#ifndef WIN32//wlh 临时修改
   {
	   extern void StopWatch_ClearData();
	   StopWatch_ClearData();
   }
#endif//WIN32
    {
        IShell *piShell = AEE_GetShell();
        if(piShell)
            ISHELL_PostURL(piShell,"brw_app:ResetSetting");
    }	
	
   //oemi_cache.input_mode=OEMNV_INPUTMODE_DEFAULT;
}



/*=============================================================================
FUNCTION: OEMPriv_MCC_TO_DEC

DESCRIPTION:  Converts an MCC type number to a normal decimal number

PARAMETERS:
   mcc: 

RETURN VALUE:
   uint16: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint16 OEMPriv_MCC_TO_DEC(uint16 mcc)
{
   static const char mintab[] = {
      '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
   };
   char   txt[4];

   txt[0] = mintab[ mcc/100 ];
   mcc %= 100;
   txt[1] = mintab[ mcc/10 ];
   txt[2] = mintab[ mcc%10 ];
   txt[3] = 0;

   return (uint16) ATOI(txt);
}


/*=============================================================================
FUNCTION: OEMPriv_MIN2_TO_DEC

DESCRIPTION:  Converts an MIN2 type number to a normal decimal number

PARAMETERS:
   mcc: 

RETURN VALUE:
   uint16: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint16 OEMPriv_MIN2_TO_DEC(uint16 min2)
{
   static const char mintab[] = {
      '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
   };
   char   txt[4];

   txt[0] = mintab[ min2/100 ];
   min2 %= 100;
   txt[1] = mintab[ min2/10 ];
   txt[2] = mintab[ min2%10 ];
   txt[3] = 0;

   return (uint16) ATOI(txt);
}


/*=============================================================================
FUNCTION: OEMPriv_MIN1_TO_STR

DESCRIPTION: Convert a MIN1 type number to ASCII

PARAMETERS:
   min1: MIN1 to convert
   *txt: string buffer with enough room for eight characters

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMPriv_MIN1_TO_STR(uint32  min1,
                                char   *txt)
{
   static const char mintab[] = {
      '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
   };
   word temp;

   if (min1 == 0) {
      MEMSET(txt, '0', 7);
   } else {
      temp = (word) (min1>>14);
      *txt++ = mintab[temp/100];
      temp %= 100;
      *txt++ = mintab[temp/10];
      *txt++ = mintab[temp%10];

      min1 &= 0x3FFFL;                  /* get bottom 14 bits */
      /* next digit is top 4 bits */
      temp = (word) (( min1 >> 10 ) & 0xF );
      *txt++ = (char) ( ( ( temp == 10 ) ? 0 : temp ) + '0' );
      temp = (word) ( min1 & 0x3FF );   /* get bottom 10 bits */
      *txt++ = mintab[ temp/100 ];
      temp %= 100;
      *txt++ = mintab[ temp/10 ];
      *txt++ = mintab[ temp%10 ];
      *txt = 0;
   }


}
/*=============================================================================

=============================================================================*/

static dword g_me_esn=0;

dword OEM_GetMEESN(void)
{
    return g_me_esn;
}

void OEM_SetMEESN(dword esn)
{
    g_me_esn = esn;
}

/*=============================================================================
FUNCTION:  OEM_InitPreference
 
DESCRIPTION:
   Initializes the OEM configuration information from NV
 
PARAMETERS:
   None
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:         

=============================================================================*/
void OEM_InitPreference(void)
{
#ifdef FEATURE_SMART_SOUND
  boolean        smart_data;
#endif
  nv_item_type   nvi;

  // Read the OEM Configuration List
  int   nRet = 0;
  //OEMPriv_ReadOEMConfigList();
  nRet = OEMPriv_ReadOEMConfigList();
  if (OEMCONFIG_UPDATED == nRet)
  {
     OEM_InitDefaultBREWParameters();
#if defined(FEATURE_PEKTEST)
     // PEK 测试版软件将语言改为英语
     nvi.language_selection = NV_LANGUAGE_ENGLISH;
     nvi_cache.language = NV_LANGUAGE_ENGLISH;
     (void) OEMNV_Put(NV_LANGUAGE_SELECTION_I, &nvi );
    
     (void) AEE_IssueSystemCallback(AEE_SCB_DEVICE_INFO_CHANGED);
#endif     
  }
  else
  {
     OEM_InitBREWParameters();
  }
#ifndef WIN32  
  wms_msg_setmochannel(oemi_cache.wms_mochannel);
#endif

  //
  // Load the NV Configuration List 
  //

  //////////////////////////////////////////////////////////////////////
  // Setting Menu
  //////////////////////////////////////////////////////////////////////
#ifndef WIN32
  (void) OEMNV_Get( NV_LIFE_TIMER_G_I, &nvi );
  nvi_cache.all_call_time = nvi.life_timer_g.time;

  (void) OEMNV_Get( NV_BACK_LIGHT_HFK_I, &nvi );
  nvi_cache.backlight_hfk = nvi.back_light_hfk;

  (void) OEMNV_Get(NV_SVC_AREA_ALERT_I, &nvi);
  nvi_cache.svc_alert = nvi.svc_area_alert ? 1 : 0;

  //////////////////////////////////////////////////////////////////////
  // Sound Menu
  //////////////////////////////////////////////////////////////////////
  // VOLUMES
  /* Ringer volume level                             */
  (void) OEMNV_Get( NV_RINGER_LVL_I, &nvi );
  nvi_cache.ringer_level = nvi.ringer_level;

  /* Handset ear piece volume level                  */
  (void) OEMNV_Get( NV_EAR_LVL_I, &nvi );
  nvi_cache.handset_ear_level = nvi.ear_level;

  /* Key beep volume level                           */
  (void) OEMNV_Get( NV_BEEP_LVL_I, &nvi );
  nvi_cache.beep_level = nvi.beep_level;

#ifdef FEATURE_SMART_SOUND
  (void) OEMNV_Get( NV_AUTO_VOLUME_ENABLED_I, &nvi );
  nvi_cache.smart_sound = nvi.auto_volume_enabled;
#endif

  (void) OEMNV_Get( NV_RINGER_TYPE_I, &nvi );
  nvi_cache.ringer_type = nvi.ringer_type;

  (void) OEMNV_Get( NV_KEY_SOUND_I, &nvi );
  nvi_cache.key_sound = (byte) nvi.key_sound;


  ////////////////////////////////////////////////////////////////
  // Display Menu
  ////////////////////////////////////////////////////////////////
  (void) OEMNV_Get( NV_LCD_I, &nvi );
  //Need to convert device value into an index 
  //pContrastEntry = OEMDisp_GetContrastEntry(OEMDISP_CONTRAST_NONE, nvi.lcd);
  nvi_cache.contrast = (byte)nvi.lcd;
  
  (void) OEMNV_Get( NV_BACK_LIGHT_I, &nvi );
  nvi_cache.backlight = nvi.back_light;

  (void) OEMNV_Get( NV_DISPLAY_DURATION_I, &nvi );
  nvi_cache.display_duration = nvi.display_duration;

  (void) OEMNV_Get( NV_LANGUAGE_SELECTION_I, &nvi );
  nvi_cache.language = nvi.language_selection;

  (void) OEMNV_Get( NV_BANNER_I, &nvi );
  {
     /* Convert the nv banner data from UTF-8 to Unicode */
     int bannerLen=0;

     while ( (bannerLen < (int) sizeof(nvi.banner.letters)) &&
             (nvi.banner.letters[bannerLen]) ) {
        ++bannerLen;
     }
     memset(nvi_cache.banner, 0, sizeof(nvi_cache.banner));
     (void) UTF8TOWSTR((byte *) nvi.banner.letters, 
                       bannerLen, 
                       nvi_cache.banner,
                       sizeof(nvi_cache.banner) - sizeof(AECHAR));
  }

  (void) OEMNV_Get( NV_MENU_FORMAT_I, &nvi );
  nvi_cache.menu_format = (byte)nvi.menu_format;

  (void) OEMNV_Get( NV_LOCK_CODE_I, &nvi );
  memcpy(nvi_cache.lockcode, 
         (byte *) nvi.lock_code.digits, 
         sizeof(nvi_cache.lockcode));

#ifdef FEATURE_SMART_SOUND
  // Tell the sound logic to turn on or off smartsound.
  (void) OEM_GetConfig(CFGI_SMART_SOUND, &smart_data, sizeof(boolean));
  OEMSound_SmartSoundEnable(smart_data);
#endif

     
#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#else
  nvi_cache.amr  = TRUE;
#endif

  //Diversity control for CDMA
#ifdef FEATURE_CDMA_RX_DIVERSITY
  (void) OEMNV_Get( NV_CDMA_RX_DIVERSITY_CTRL_I, &nvi );
  nvi_cache.cdma_rx_diversity_ctrl = nvi.cdma_rx_diversity_ctrl;
#else
  nvi_cache.cdma_rx_diversity_ctrl = FALSE;
#endif

  //Diversity control for HDR
#ifdef FEATURE_HDR
#error code not present
#else
  nvi_cache.hdr_rx_diversity_ctrl = FALSE;
#endif

  // TTY
  (void) OEMNV_Get( NV_TTY_I, &nvi );
  nvi_cache.tty  = nvi.tty;

  // Data Call Counters
  (void) OEMNV_Get( NV_LAST_RX_DATA_COUNT_I, &nvi);
  nvi_cache.last_rx_data_count = nvi.last_rx_data_count;
  
  (void) OEMNV_Get( NV_LAST_TX_DATA_COUNT_I, &nvi);
  nvi_cache.last_tx_data_count = nvi.last_tx_data_count;
  
  (void) OEMNV_Get( NV_TOTAL_RX_DATA_COUNT_I, &nvi);
  nvi_cache.total_rx_data_count = nvi.last_rx_data_count;
  
  (void) OEMNV_Get( NV_TOTAL_TX_DATA_COUNT_I, &nvi);
  nvi_cache.total_tx_data_count = nvi.total_tx_data_count;
  (void) OEMNV_Get( NV_SET_TIME_FORMAT_I,  &nvi);
  nvi_cache.set_time_format = (byte)nvi.set_time_format;
  
#ifdef FEATURE_TIME_DATA_SETTING
  (void) OEMNV_Get( NV_SET_DATE_FORMAT_I,  &nvi);
  nvi_cache.set_date_format = (byte)nvi.set_date_format;
#endif

   //CFGI_AUTO_REDIAL
   (void) OEMNV_Get( NV_AUTO_REDIAL_I,  &nvi);
   nvi_cache.auto_redial.enable = nvi.auto_redial.enable;
   if(nvi.auto_redial.rings == 0)
   {
       nvi.auto_redial.rings = 10;
   }
   nvi_cache.auto_redial.rings    = nvi.auto_redial.rings;

   //CFGI_VOICE_PRIVACY
   (void) OEMNV_Get( NV_VOICE_PRIV_I,  &nvi);
   nvi_cache.voice_priv = nvi.voice_priv;
   
   //CFGI_PHONE_PASSWORD_CHECK
   (void) OEMNV_Get( NV_LOCK_I, &nvi);
   nvi_cache.lock = nvi.lock;

    //CFGI_KEYTONE_LENGTH
    (void) OEMNV_Get( NV_CONT_KEY_DTMF_I, &nvi);
    if(nvi.cont_key_dtmf)
    {
        nvi_cache.key_tone_length = OEMNV_KEYTONE_LONG;
    }
    else
    {
        nvi_cache.key_tone_length = OEMNV_KEYTONE_NORMAL;
    }
#endif//#ifndef WIN32
    cfgi_lcd = TRUE;

    cache_initialized = TRUE;
  
}


/*==================================================================
Function: OEM_GetCachedConfig

Description: Retrieves cached configuration information from the OEM

Prototype:
   int  OEM_GetCachedConfig(AEEConfigItem i, void * pBuff, int nSize)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
int OEM_GetCachedConfig(AEEConfigItem i, void * pBuff, int nSize)
{
   uint32 j;

   if (!cache_initialized)
     OEM_InitPreference();

   // First check if the item can be found in any of the config 
   // tables before resorting to the ugly switch() statement below...
   for (j = 0; j < ARR_SIZE(cfgTable); j++) {
      // Is the item in the range of this table?
      if ( (i >= cfgTable[j].tbl[0].item) &&
           (i <= cfgTable[j].tbl[cfgTable[j].size-1].item) ) {

         int idx = (int)i - (int) cfgTable[j].tbl[0].item;

         // Ensure the config item table is valid
         //ASSERT(i == (int) cfgTable[j].tbl[idx].item);

         if (NULL == cfgTable[j].tbl[idx].get) {
            // Exit the for() loop immediately if we found the
            // entry and it had a NULL function. 
            break;
         }

         // Perform the size check here so the item functions can
         // assume the size is ok.
         if (nSize != cfgTable[j].tbl[idx].size) {
            return EBADPARM;
         }

         return cfgTable[j].tbl[idx].get(pBuff);
      }
   }
   
   // Didn't find the config item in the tables, maybe it's in this switch...
   switch (i) {

   //////////////////////////////////////////////////////////////////////
   // Security Menu
   //////////////////////////////////////////////////////////////////////

   case CFGI_LOCKCODE:
      //check to see if we have enough buffer size
      if (!pBuff || (nSize != (NV_LOCK_CODE_SIZE+1))) { 
         return EBADPARM;
      }

      MEMCPY((byte *) pBuff, 
             (byte *) nvi_cache.lockcode, 
             NV_LOCK_CODE_SIZE);

      // Ensure the string is NULL terminated
      ((char *)pBuff)[NV_LOCK_CODE_SIZE] = '\0';
      return(0);

   ////////////////////////////////////////////////////////////////
   // Phone Number
   ////////////////////////////////////////////////////////////////
   case CFGI_PHONE_NUMBER:
      {
         nv_item_type nvi;
         char         digits[OEMNV_PHONENUMBER_MAXLEN];

         if (nSize < (int) sizeof(AECHAR) * OEMNV_PHONENUMBER_MAXLEN) {
            return EFAILED;
         }
#ifndef WIN32
         nvi.mob_dir_number.nam = (byte) CM_NAM_1;

         if (NV_DONE_S != OEMNV_Get(NV_DIR_NUMBER_PCS_I, &nvi)) {
           return EFAILED;
         }
         if (nSize <= nvi.mob_dir_number.n_digits*sizeof(AECHAR)) {
            return EBADPARM;
         }

         i = 0;
         for (;;) {
            if (i >= MIN((int)sizeof(digits)-1,
                          nvi.mob_dir_number.n_digits)) {
               digits[i] = '\0';
               break;
            }
          
            if (nvi.mob_dir_number.digitn[i] <= 9) {
               digits[i] = '0' + (char) nvi.mob_dir_number.digitn[i];
            } else if (0x0A == nvi.mob_dir_number.digitn[i]) {
               digits[i] = '0';
            } else {
               // Bad value!
               digits[i] = '\0';
               break;
            }
            i++;
         }
#endif
         STR_TO_WSTR(digits,
                     (AECHAR *) pBuff,
                     nSize);
         return SUCCESS;
      }


   ////////////////////////////////////////////////////////////////
   // Display Menu
   ////////////////////////////////////////////////////////////////
   case CFGI_BANNER:
      WSTRNCOPYN(pBuff,
                 nSize / (int) sizeof(AECHAR),
                 nvi_cache.banner,
                 (int) (sizeof(nvi_cache.banner) / sizeof(AECHAR)) );
      return 0;

   ////////////////////////////////////////////////////////////////
   // SMS Applet
   ////////////////////////////////////////////////////////////////

   case CFGI_SMS_ALERT_VOLUME:
   {
#ifdef FEATURE_WMS_APP
#ifndef WIN32
     nv_item_type nvi;
     
     if (OEMNV_Get(NV_ALERTS_LVL_I, &nvi) != NV_DONE_S) 
     {
       return EFAILED;
     }
     MEMCPY((byte *)pBuff, (byte *)&nvi.alerts_lvl, 1);
#endif
#else
     MEMCPY((byte *)pBuff, (byte *)&oemi_cache.sms_alert_volume, 1);
#endif /* FEATURE_WMS_APP */
     return SUCCESS;
   }

   case CFGI_VOICEMAIL_NUMBER:
   {
     nv_item_type nvi;
#ifndef WIN32     
     if (OEMNV_Get(NV_SMS_VM_NUMBER_I, &nvi) != NV_DONE_S) 
     {
       return EFAILED;
     }
     nvi.sms_vm_number.digits[nvi.sms_vm_number.num_digits] = 0;
     STRTOWSTR((char *)nvi.sms_vm_number.digits, pBuff, (nvi.sms_vm_number.num_digits+1) * sizeof(AECHAR));
#endif
     if (WSTRLEN(pBuff) == 0)
     {
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
        (void)STRTOWSTR("*9", pBuff, nSize);
#endif     
     }
     
     return SUCCESS;
   }

   case CFGI_VOICEMAIL_NUMBER_CDMA:
   {
     WSTRNCOPYN(pBuff,
                nSize / (int) sizeof(AECHAR),
                (void *) oemi_cache.voicemail_number_cdma,
                (int) (sizeof(oemi_cache.voicemail_number_cdma) / sizeof(AECHAR)));
     return SUCCESS;
   }

   case CFGI_VOICEMAIL_NUMBER_GW:
   {
     WSTRNCOPYN(pBuff,
                nSize / (int) sizeof(AECHAR),
                (void *) oemi_cache.voicemail_number_gw,
                (int) (sizeof(oemi_cache.voicemail_number_gw) / sizeof(AECHAR)));
     return SUCCESS;
   }

   ////////////////////////////////////////////////////////////////
   // RecentCalls App
   ////////////////////////////////////////////////////////////////
   
   // time of the last call
   case CFGI_LASTCALL_TIMER:
   {
     nv_item_type nvItem;
#ifndef WIN32
     if(NV_DONE_S == OEMNV_Get(NV_CALL_TIMER_G_I, &nvItem))
     {
       *(dword*)pBuff = (nvItem.call_timer_g.time);
       return SUCCESS;
     }
     else
       return EFAILED;
#else
	 return SUCCESS;
#endif
   } 

   // total local (non-roaming) air time used
   case CFGI_AIR_TIMER:   
   {
      dword time = 0;
#ifndef WIN32      
      for(i=0;i < NV_MAX_NAMS; i++)
      {
        nv_item_type nvItem;
      
        nvItem.air_timer.nam = (byte) i;
        if(NV_DONE_S == OEMNV_Get(NV_AIR_TIMER_I, &nvItem))
        {
          time += (nvItem.air_timer.time);
        }
        else
          return EFAILED;
      }
       *(dword*)pBuff = time;
#endif
       return SUCCESS;
   } 

   // returns the total roaming time used.
   case CFGI_ROAM_TIMER:
   {
      dword time = 0;
#ifndef WIN32     
      for(i=0;i < NV_MAX_NAMS; i++)
      {
        nv_item_type nvItem;
        
        nvItem.roam_timer.nam = (byte) i;
        if(NV_DONE_S == OEMNV_Get(NV_ROAM_TIMER_I, &nvItem))
        {
          time += (nvItem.roam_timer.time);
        }
        else
          return EFAILED;
      }
       *(dword*)pBuff = time;
#endif
       return SUCCESS;
    }

   // returns the total air time used.
   case CFGI_LIFE_TIMER:
   {
      nv_item_type nvItem;
#ifndef WIN32
      if(NV_DONE_S == OEMNV_Get(NV_LIFE_TIMER_G_I, &nvItem))
      {
        *(dword*)pBuff = (nvItem.life_timer_g.time);
        return SUCCESS;
      }
      else
        return EFAILED;
#else
	 return SUCCESS;
#endif
   } // Recent Calls App

   ////////////////////////////////////////////////////////////////
   // Service Programming 
   ////////////////////////////////////////////////////////////////

   case CFGI_BUILD_VERSION:
#ifndef WIN32
      STR_TO_WSTR((char *)ver_modelversion,
                  (AECHAR *) pBuff,
                  nSize);
#endif
      return SUCCESS;

   case CFGI_PRI_CH_A:
   case CFGI_PRI_CH_B:
      {
         nv_item_type nvi;
#ifndef WIN32
         if (nSize != (int) sizeof(uint16)) {
            return EBADPARM;
         }

         nvi.pcdmach.nam = (byte) CM_NAM_1;
         if (NV_DONE_S != OEMNV_Get(NV_PCDMACH_I, &nvi)) {
            return EFAILED;
         }

         if (CFGI_PRI_CH_A == i) {
            *(uint16 *) pBuff = nvi.pcdmach.channel_a;
         } else {
            *(uint16 *) pBuff = nvi.pcdmach.channel_b;
         }
#endif
         return SUCCESS;
      }

   case CFGI_SEC_CH_A:
   case CFGI_SEC_CH_B:
      {
#ifndef WIN32
         nv_item_type nvi;

         if (nSize != (int) sizeof(uint16)) {
            return EBADPARM;
         }

         nvi.scdmach.nam = (byte) CM_NAM_1;
         if (NV_DONE_S != OEMNV_Get(NV_SCDMACH_I, &nvi)) {
            return EFAILED;
         }

         if (CFGI_SEC_CH_A == i) {
            *(uint16 *) pBuff = nvi.scdmach.channel_a;
         } else {
            *(uint16 *) pBuff = nvi.scdmach.channel_b;
         }
#endif
         return SUCCESS;
      }

   case CFGI_DATA_DIALSTRING:
      {
#ifndef WIN32
         char         code[OEMNV_DATA_DIALSTRING_MAXLEN];
         nv_item_type nvi;

         if (nSize > ((int) sizeof(AECHAR) * OEMNV_DATA_DIALSTRING_MAXLEN)) {
            return EBADPARM;
         }

         if (NV_DONE_S != OEMNV_Get(NV_DATA_PKT_ORIG_STR_I, &nvi)) {
            return EFAILED;
         }

         MEMSET(code, '\0', sizeof(code));
         MEMCPY(code,
                (void *) nvi.data_pkt_orig_str.digits, 
                nvi.data_pkt_orig_str.num_digits);

         STR_TO_WSTR(code,
                     (AECHAR *) pBuff,
                     nSize);
#endif
         return SUCCESS;
      }

   case CFGI_BUSY_CURSOR_OFFSET:
      {
         AEERect *rc;

         if (nSize != (int) sizeof(*rc)) {
            return EBADPARM;
         }

         // Position the busy icon far off the bottom
         // of the display so it will never be visible
         rc = (AEERect *) pBuff;
         rc->x = 0;
         rc->y = 10000;
         
         return SUCCESS;
      }

   case CFGI_DOWNLOAD:
	  {
#ifndef WIN32
         AEEDownloadInfo * pdi = (AEEDownloadInfo *)pBuff;

         if (!pBuff || nSize != sizeof(AEEDownloadInfo))
         {
            return(EBADPARM);
         }

         MEMSET((byte *) pdi, 0, nSize);

         pdi->dwCarrierID  = nvi_cache.brew_carrier_id;
         pdi->dwPlatformID = nvi_cache.brew_platform_id;
         pdi->nAuth        = nvi_cache.brew_auth_policy;
         pdi->wFlags       = nvi_cache.brew_download_flags;
         pdi->nPolicy      = nvi_cache.brew_privacy_policy;
         STRCPY((char*)pdi->szServer, (char*)nvi_cache.brew_server);
         STRCPY((char*)pdi->bBKey, (char*)nvi_cache.brew_bkey);
#endif        
         return(SUCCESS);
	  }   
	  
    //
    // OEM IDs, specific to the OEM
    // 
    case CFGI_OEM_IDS_LEN:
    case CFGI_OEM_IDS:
        {
            // An initial entry of 0 is used to signify that OEM IDs are not to
            // be used. Change the value(s) to include the OEM IDs this build
            // should accept as valid OEM IDs.
            static const uint32 adwOEMIDs[] = {0,};
            
            // There is no need to change the following assignment. nOEMIDsSize
            // will be updated appropriately if adwOEMIDs does not start with 0
            int nOEMIDsSize = 0;
            
            // If the first entry in the array is 0, nOEMIDsSize should remain 0
            if ((sizeof(adwOEMIDs) != 0) && (0 != adwOEMIDs[0])) 
            {
                nOEMIDsSize = sizeof(adwOEMIDs);
            }
            
            if (CFGI_OEM_IDS_LEN == i) 
            {
                // Caller is requesting the size of the OEM IDs array
                if (sizeof(int) != nSize) 
                {
                    return EBADPARM;
                }
                else 
                {
                    // Assign the size to the out param (pBuff)
                    *((int*)pBuff) = nOEMIDsSize;
                }
            }
            else 
            {
                // Caller is requesting the data in the OEM IDs array
                if (nSize != nOEMIDsSize) 
                {
                    return EBADPARM;
                }
                else
                {
                    // Copy the data into the caller supplied buffer
                    MEMMOVE(pBuff, adwOEMIDs, nSize);
                }
            }
        }
        return SUCCESS;
	  
#if defined( FEATURE_APP_MANAGER)
    case CFGI_DOWNLOAD_BUFFER:
    {
         if (nSize != (int)sizeof(uint32)) 
         {
            return EBADPARM;
         }
        
         *(uint32*)pBuff = 1024 * 8;
         return SUCCESS;
    }
    case CFGI_HTTP_BUFFER:
    {
         if(!pBuff || nSize != (int)sizeof(dword))
         {
              return(EBADPARM);
         }
     
         *((uint32*)pBuff) = 1024 * 8;
         return SUCCESS;
    }
    case CFGI_NET_CONNTIMEOUT:
    {
    	 if(!pBuff || nSize != (int)sizeof(dword))
    	 {
            return(EBADPARM);
    	 }
    	 
         *((uint32*)pBuff) = 30000;
         return SUCCESS;
    }   		
#endif //#if defined( FEATURE_APP_MANAGER)
	  
      // 不支持该项，以 MIN 代替 SID
      // 以 MIN 代替 SID
      case CFGI_SUBSCRIBERID:
      {
         int nLen;
         AEEMobileInfo info;
         
         GetMobileInfoEx(&info);
         nLen = STRLEN(info.szMobileID);
             
         if (!pBuff || nSize <= nLen)
         {
            return EBADPARM;
         }
         
         STRLCPY((char *)pBuff, info.szMobileID, nSize);
         return AEE_SUCCESS;
      }
      
      case CFGI_SUBSCRIBERID_LEN:
      {
         int nLen;
         AEEMobileInfo info;
         
         if (!pBuff || nSize != sizeof(int32))
         {
            return EBADPARM;
         }
         
         GetMobileInfoEx(&info);
         nLen = STRLEN(info.szMobileID);
         *((int32 *)pBuff) = nLen+1;

         return AEE_SUCCESS;
      }
#ifdef FEATURE_ANIMATION_POWERUPDOWN
    //开机动画
    case CFGI_STARTUP_ANIMATION:
      MEMCPY((void *)pBuff, (void *)oemi_cache.s_startupani, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      return AEE_SUCCESS;
      
    //关机动画  
    case CFGI_POWEROFF_ANIMATION:
      MEMCPY((void *)pBuff, (void *)oemi_cache.s_poweroffani, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      return AEE_SUCCESS;
#endif

    //桌面墙纸  
    case CFGI_WALLPAPER:
      MEMCPY((void *)pBuff, (void *)oemi_cache.s_wallpaper, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      return AEE_SUCCESS;
      
#ifdef FEATURE_SCREEN_SAVE
    //屏幕保护类型
   case CFGI_SCREENSAVE_TYPE:
      MEMCPY((void *)pBuff, (void *)oemi_cache.s_screensave_type, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      return AEE_SUCCESS;
#endif

   case CFGI_MOBILEINFO:
      if (nSize != sizeof(AEEMobileInfo))
      {
         return(EBADPARM);
      }
      GetMobileInfoEx((AEEMobileInfo *) pBuff);
        
      return SUCCESS;

    case CFGI_BREW_USERNAME:
        {
            if (!pBuff || nSize <= sizeof(AECHAR)*STRLEN((char *)oemi_cache.brew_username))
            {
                return EBADPARM;
            }
        
            (void)STRTOWSTR((char *)oemi_cache.brew_username, (AECHAR *)pBuff, nSize);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_PASSWORD:
        {
            if (!pBuff || nSize <= sizeof(AECHAR)*STRLEN((char *)oemi_cache.brew_password))
            {
                return EBADPARM;
            }
        
            (void)STRTOWSTR((char *)oemi_cache.brew_password, (AECHAR *)pBuff, nSize);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_SERVER:
        {
            if (!pBuff || nSize <= sizeof(AECHAR)*STRLEN((char *)nvi_cache.brew_server))
            {
                return EBADPARM;
            }
        
            (void)STRTOWSTR((char *)nvi_cache.brew_server, (AECHAR *)pBuff, nSize);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_CARRIER_ID:
        {
            if (!pBuff || nSize != sizeof(dword))
            {
                return EBADPARM;
            }
        
            *(dword *)pBuff = nvi_cache.brew_carrier_id;
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_BKEY:
        {
            if ((!pBuff) || nSize <= sizeof(AECHAR)*STRLEN((char *)nvi_cache.brew_bkey))
            {
                return EBADPARM;
            }
            
            (void)STRTOWSTR((char *)nvi_cache.brew_bkey, (AECHAR *)pBuff, nSize);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_AUTH_POLICY:
        {
            if (!pBuff || nSize != sizeof(uint8))
            {
                return EBADPARM;
            }
        
            *(uint8 *)pBuff = (uint8)nvi_cache.brew_auth_policy;
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_PRIVACY_POLICY:
        {
            if (!pBuff || nSize != sizeof(uint8))
            {
                return EBADPARM;
            }
        
            *(uint8 *)pBuff = (uint8)nvi_cache.brew_privacy_policy;
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_SUBSCRIBER_ID:
        {
            int nLen;
            AEEMobileInfo info;
             
            //GetMobileInfo(&info);
            GetMobileInfoEx(&info);
            nLen = STRLEN(info.szMobileID);
             
            if (!pBuff || nSize < sizeof(AECHAR)*(1+nLen))
            {
                return EBADPARM;
            }
             
            (void)STRTOWSTR(info.szMobileID, (AECHAR *)pBuff, nSize);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_PLATFORM_ID:
        {
            if (!pBuff || nSize != sizeof(dword))
            {
                return EBADPARM;
            }
        
            *(dword *)pBuff = nvi_cache.brew_platform_id;
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_TESTOPT:              // DIF_TEST_ALLOWED  
        {
            if ((!pBuff) || nSize != sizeof(boolean))
            {
                return EBADPARM;
            }
            
            *(boolean *)pBuff = ((nvi_cache.brew_download_flags & DIF_TEST_ALLOWED) ? TRUE : FALSE);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_USEAKEY:              // DIF_USE_A_KEY
        {
            if ((!pBuff) || nSize != sizeof(boolean))
            {
                return EBADPARM;
            }
            
            *(boolean *)pBuff = ((nvi_cache.brew_download_flags & DIF_USE_A_KEY) ? TRUE : FALSE);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_AUTOUPGRADE_FLG:      // DIF_AUTO_UPGRADE 
        {
            if ((!pBuff) || nSize != sizeof(boolean))
            {
                return EBADPARM;
            }
            
            *(boolean *)pBuff = ((nvi_cache.brew_download_flags & DIF_AUTO_UPGRADE) ? TRUE : FALSE);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_USEMINFORSID_FLG:     // DIF_MIN_FOR_SID
        {
            if ((!pBuff) || nSize != sizeof(boolean))
            {
                return EBADPARM;
            }
            
            *(boolean *)pBuff = ((nvi_cache.brew_download_flags & DIF_MIN_FOR_SID) ? TRUE : FALSE);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_PREPAY_FLG:           // DIF_PREPAY
        {
            if ((!pBuff) || nSize != sizeof(boolean))
            {
                return EBADPARM;
            }
            
            *(boolean *)pBuff = ((nvi_cache.brew_download_flags & DIF_PREPAY) ? TRUE : FALSE);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_NOAUTOACK_FLG:        // DIF_NO_AUTO_ACK
        {
            if ((!pBuff) || nSize != sizeof(boolean))
            {
                return EBADPARM;
            }
            
            *(boolean *)pBuff = ((nvi_cache.brew_download_flags & DIF_NO_AUTO_ACK) ? TRUE : FALSE);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_SIDENCODE_FLG:        // DIF_SID_ENCODE
        {
            if ((!pBuff) || nSize != sizeof(boolean))
            {
                return EBADPARM;
            }
            
            *(boolean *)pBuff = ((nvi_cache.brew_download_flags & DIF_SID_ENCODE) ? TRUE : FALSE);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_SIDVALIDATAALL_FLG:   // DIF_SID_VALIDATE_ALL
        {
            if ((!pBuff) || nSize != sizeof(boolean))
            {
                return EBADPARM;
            }
            
            *(boolean *)pBuff = ((nvi_cache.brew_download_flags & DIF_SID_VALIDATE_ALL) ? TRUE : FALSE);
        }
        return AEE_SUCCESS;
        
    case CFGI_BREW_IDS_RUIMDELETE_FLG:   // DIF_RUIM_DEL_OVERRIDE
        {
            if ((!pBuff) || nSize != sizeof(boolean))
            {
                return EBADPARM;
            }
            
            *(boolean *)pBuff = ((nvi_cache.brew_download_flags & DIF_RUIM_DEL_OVERRIDE) ? TRUE : FALSE);
        }
        return AEE_SUCCESS;
    
    case CFGI_OEMAUTH_CHALLENGE_CAP:
        if (nSize < sizeof(boolean))
        {
            return EBADPARM;
        }
        /* 
        if (IsRunAsUIMVersion())
        {
            *(boolean *)pBuff = TRUE;
        }
        else
        */
        {
            *(boolean *)pBuff = FALSE;
        }
        return AEE_SUCCESS;
   
   default:
      return(EUNSUPPORTED);

   }
}


/*==================================================================
Function: OEM_SetCachedConfig

Description: Sets configuration information from the OEM

Prototype:
   int  OEM_SetCachedConfig(AEEConfigItem i, void * pBuff, int nSize)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
int OEM_SetCachedConfig(AEEConfigItem i, void * pBuff, int nSize)
{
   nv_item_type nvi;
   uint32       j;

   if (!cache_initialized)
     OEM_InitPreference();

   // First check if the item can be found in any of the config 
   // tables before resorting to the ugly switch() statement below...
   for (j = 0; j < ARR_SIZE(cfgTable); j++) {
      // Is the item in the range of this table?
      if ( (i >= cfgTable[j].tbl[0].item) &&
           (i <= cfgTable[j].tbl[cfgTable[j].size-1].item) ) {

         int idx = (int)i - (int) cfgTable[j].tbl[0].item;

         // Ensure the config item table is valid
         //ASSERT(i == (int) cfgTable[j].tbl[idx].item);

         if (NULL == cfgTable[j].tbl[idx].get) {
            // Exit the for() loop immediately if we found the
            // entry and it had a NULL function. 
            break;
         }

         // Perform the size check here so the item functions can
         // assume the size is ok.
         if (nSize != cfgTable[j].tbl[idx].size) {
            ERR("SetConfig(): invalid parm size", 0, 0, 0);
            return EBADPARM;
         }

         return cfgTable[j].tbl[idx].set(pBuff);
      }
   }
   
   // Didn't find the config item in the tables, maybe it's in this switch...
   switch(i) {
   //////////////////////////////////////////////////////////////////////
   // Security Menu
   //////////////////////////////////////////////////////////////////////

   case CFGI_LOCKCODE:
      if (nSize != NV_LOCK_CODE_SIZE+1) {
         return EBADPARM;
      }

      if( MEMCMP((byte *) pBuff, 
                 (byte *) nvi_cache.lockcode, 
                 NV_LOCK_CODE_SIZE) ) {

         MEMCPY((byte *)nvi_cache.lockcode, 
                (byte *) pBuff, 
                NV_LOCK_CODE_SIZE);
#ifndef WIN32
         MEMCPY((byte *)nvi.lock_code.digits,
                (byte *) pBuff, 
                NV_LOCK_CODE_SIZE);

         (void) OEMNV_Put( NV_LOCK_CODE_I, &nvi );
#endif
      }
      return SUCCESS;


   ////////////////////////////////////////////////////////////////
   // Phone Number
   ////////////////////////////////////////////////////////////////
   case CFGI_PHONE_NUMBER:
      {
#ifndef WIN32
         int32    len;
         AECHAR  *wPtr = pBuff;

         len = WSTRLEN(wPtr);

         if (len > NV_DIR_NUMB_PCS_SIZ) {
            return EBADPARM;
         }

         nvi.mob_dir_number.nam = (byte) CM_NAM_1;
         nvi.mob_dir_number.n_digits = (byte) len;
 
         for (j = 0; j < (uint32) len; j++) {
 
            if ('0' == wPtr[j]) {
               nvi.mob_dir_number.digitn[j] = 0x0A;
            } else if ( (wPtr[j] >= '1') || (wPtr[j] <= '9')) {
               nvi.mob_dir_number.digitn[j] = (byte) wPtr[j] - '0';
            } else {
               return EBADPARM;
            }
         }
 
         (void) OEMNV_Put( NV_DIR_NUMBER_PCS_I, &nvi );
#endif 
         return SUCCESS;
      }

   //////////////////////////////////////////////////////////////////////
   // Display Menu
   //////////////////////////////////////////////////////////////////////
   case CFGI_BANNER:
      if(!pBuff) return EFAILED;
#ifndef WIN32
      WSTRNCOPYN(nvi_cache.banner, 
                 sizeof(nvi_cache.banner)/sizeof(AECHAR),
                 (AECHAR*) pBuff,
                 -1);

      /* Convert the nv banner data from Unicode to UTF-8 */
      memset((void *)nvi.banner.letters, 0, sizeof(nvi.banner.letters));
      (void) WSTRTOUTF8((AECHAR*) pBuff, 
                        WSTRLEN((AECHAR*) pBuff),
                        (byte *)nvi.banner.letters, 
                        sizeof(nvi.banner.letters));
 

      (void) OEMNV_Put( NV_BANNER_I, &nvi );
#endif
      return 0;

   ////////////////////////////////////////////////////////////////
   // SMS Applet
   ////////////////////////////////////////////////////////////////
   case CFGI_SMS_ALERT_VOLUME:
   {
     if(!pBuff) 
       return EFAILED;
#ifndef WIN32
#ifdef FEATURE_WMS_APP
    MEMCPY((byte*)&nvi.alerts_lvl, (byte*)pBuff, 1);
    
    if (OEMNV_Put(NV_ALERTS_LVL_I, &nvi) != NV_DONE_S)
    {
      return EFAILED;
    }
#else
    MEMCPY((byte*)&oemi_cache.sms_alert_volume, (byte*)pBuff, 1);
    OEMPriv_WriteOEMConfigList();
#endif
#endif
    return SUCCESS;
   }

   case CFGI_VOICEMAIL_NUMBER:
   {
     if(!pBuff) 
       return EFAILED;
#ifndef WIN32    
    nvi.sms_vm_number.num_digits = WSTRLEN(pBuff);
    WSTRTOSTR(pBuff, (char *)nvi.sms_vm_number.digits, nvi.sms_vm_number.num_digits+1);

    if (OEMNV_Put(NV_SMS_VM_NUMBER_I, &nvi) != NV_DONE_S)
    {
      return EFAILED;
    }
#endif
      return SUCCESS;
   }

   case CFGI_VOICEMAIL_NUMBER_CDMA:
   {
     if(!pBuff) 
       return EFAILED;
     WSTRNCOPYN((void *) oemi_cache.voicemail_number_cdma,
                sizeof(oemi_cache.voicemail_number_cdma)/sizeof(AECHAR),
                (AECHAR*) pBuff, -1);
     OEMPriv_WriteOEMConfigList();
     return SUCCESS;
   }

   case CFGI_VOICEMAIL_NUMBER_GW:
   {
     if(!pBuff) 
       return EFAILED;
     WSTRNCOPYN((void *) oemi_cache.voicemail_number_gw,
                sizeof(oemi_cache.voicemail_number_gw)/sizeof(AECHAR),
                (AECHAR*) pBuff, -1);
     OEMPriv_WriteOEMConfigList();
     return SUCCESS;
   }

   ////////////////////////////////////////////////////////////////
   // RecentCalls App
   ////////////////////////////////////////////////////////////////
   case CFGI_LASTCALL_TIMER:
     {
      if ((!pBuff) || (nSize != (int)sizeof(dword)))
        return EBADPARM;
#ifndef WIN32      
      nvi.call_timer_g.time = *(dword*) pBuff;

      if(OEMNV_Put(CFGI_LASTCALL_TIMER,&nvi)!= NV_DONE_S)
        return EFAILED;
#endif
      return SUCCESS;
     }

   case CFGI_AIR_TIMER:
     {
      if ((!pBuff) || (nSize != (int)sizeof(dword)))
        return EBADPARM;
#ifndef WIN32      
      nvi.air_timer.time = *(dword*) pBuff;

      if(OEMNV_Put(CFGI_AIR_TIMER,&nvi)!= NV_DONE_S)
        return EFAILED;
#endif
      return SUCCESS;
     }
   
   case CFGI_ROAM_TIMER:
     {
      if ((!pBuff) || (nSize != (int)sizeof(dword)))
        return EBADPARM;
#ifndef WIN32      
      nvi.roam_timer.time = *(dword*) pBuff;

      if(OEMNV_Put(CFGI_ROAM_TIMER,&nvi)!= NV_DONE_S)
        return EFAILED;
#endif
      return SUCCESS;
     }

   case CFGI_LIFE_TIMER:
     {
      if ((!pBuff) || (nSize != (int)sizeof(dword)))
        return EBADPARM;
#ifndef WIN32      
      nvi.life_timer_g.time = *(dword*) pBuff;

      if(OEMNV_Put(CFGI_LIFE_TIMER,&nvi)!= NV_DONE_S)
        return EFAILED;
#endif
      return SUCCESS;
     }
   ////////////////////////////////////////////////////////////////
   // Service Programming 
   ////////////////////////////////////////////////////////////////
   case CFGI_BUILD_VERSION:
      // This item is read-only
      return EBADPARM;

   case CFGI_PRI_CH_A:
   case CFGI_PRI_CH_B:
      {
         if (nSize != (int) sizeof(uint16)) {
            return EBADPARM;
         }
#ifndef WIN32
         // Get current values
         nvi.pcdmach.nam = (byte) CM_NAM_1;
         if (NV_DONE_S != OEMNV_Get(NV_PCDMACH_I, &nvi)) {
            return EFAILED;
         }

         // Change either A or B
         nvi.pcdmach.nam = (byte) CM_NAM_1;
         if (CFGI_PRI_CH_A == i) {
            nvi.pcdmach.channel_a = *(uint16 *) pBuff;
         } else {
            nvi.pcdmach.channel_b = *(uint16 *) pBuff;
         }

         // Put it back
         if (NV_DONE_S != OEMNV_Put(NV_PCDMACH_I, &nvi)) {
            return EFAILED;
         }
#endif
         return SUCCESS;
      }

   case CFGI_SEC_CH_A:
   case CFGI_SEC_CH_B:
      {
         if (nSize != (int) sizeof(uint16)) {
            return EBADPARM;
         }
#ifndef WIN32
         // Get current values
         nvi.scdmach.nam = (byte) CM_NAM_1;
         if (NV_DONE_S != OEMNV_Get(NV_SCDMACH_I, &nvi)) {
            return EFAILED;
         }

         // Change either A or B
         nvi.scdmach.nam = (byte) CM_NAM_1;
         if (CFGI_SEC_CH_A == i) {
            nvi.scdmach.channel_a = *(uint16 *) pBuff;
         } else {
            nvi.scdmach.channel_b = *(uint16 *) pBuff;
         }

         // Put it back
         if (NV_DONE_S != OEMNV_Put(NV_SCDMACH_I, &nvi)) {
            return EFAILED;
         }
#endif
         return SUCCESS;
      }

   case CFGI_DATA_DIALSTRING:
      {
         char code[OEMNV_DATA_DIALSTRING_MAXLEN];

         if (nSize > ((int) sizeof(AECHAR) * OEMNV_DATA_DIALSTRING_MAXLEN)) {
            return EBADPARM;
         }

         WSTR_TO_STR((AECHAR *) pBuff,
                     code,
                     sizeof(code));
#ifndef WIN32
         // Length includes the NULL character
         nvi.data_pkt_orig_str.num_digits = (uint8) STRLEN(code) + 1;

         MEMCPY((void *) nvi.data_pkt_orig_str.digits, 
                code,
                sizeof(nvi.data_pkt_orig_str.digits));

         if (NV_DONE_S != OEMNV_Put(NV_DATA_PKT_ORIG_STR_I, &nvi)) {
            return EFAILED;
         }
#endif
         return SUCCESS;
      }
 /*
   case CFGI_DATA_NETWORK:
      // This config item was added in BREW v2.0 but we don't support
      // it yet...
      ERR("CFGI_DATA_NETWORK not implemented", 0, 0, 0);
      return EUNSUPPORTED;
*/
   case CFGI_DOWNLOAD:
	 {
	    AEEDownloadInfo * pdi = (AEEDownloadInfo *)pBuff;

        if (!pBuff || nSize != sizeof(AEEDownloadInfo))
        {
            return(EBADPARM);
        }
        OEM_StoreDownloadInfo(pdi);

        return SUCCESS;
	 }

    // 不支持该项，以 MIN 代替 SID
    /*
   case CFGI_SUBSCRIBERID:
      {
         if (!pBuff || nSize > NV_BREW_SID_SIZ)
         {
            return EBADPARM;
         }

         return SetSubscriberID((byte *)pBuff, nSize);
      }
    */

#ifdef FEATURE_ANIMATION_POWERUPDOWN
   //开机动画
   case CFGI_STARTUP_ANIMATION:
      if(!pBuff) return EFAILED;
      MEMSET((void *)oemi_cache.s_startupani,'\0', AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      MEMCPY((void *)oemi_cache.s_startupani, (void *)pBuff, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      OEMPriv_WriteOEMConfigList();
      return AEE_SUCCESS;
      
   //关机动画  
   case CFGI_POWEROFF_ANIMATION:
      if(!pBuff) return EFAILED;
      MEMSET((void *)oemi_cache.s_poweroffani,'\0', AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      MEMCPY((void *)oemi_cache.s_poweroffani, (void *)pBuff, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      OEMPriv_WriteOEMConfigList();
      return AEE_SUCCESS;
#endif

   //桌面墙纸 
   case CFGI_WALLPAPER:
      if(!pBuff) return EFAILED;
      MEMSET((void *)oemi_cache.s_wallpaper,'\0', AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      MEMCPY((void *)oemi_cache.s_wallpaper, (void *)pBuff, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      OEMPriv_WriteOEMConfigList();
      return AEE_SUCCESS;
      
#ifdef FEATURE_SCREEN_SAVE
   //屏幕保护类型
   case CFGI_SCREENSAVE_TYPE:
      if(!pBuff) return EFAILED;
      MEMSET((void *)oemi_cache.s_screensave_type,'\0', AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      MEMCPY((void *)oemi_cache.s_screensave_type, (void *)pBuff, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      OEMPriv_WriteOEMConfigList();
      return AEE_SUCCESS;
#endif
         
   //
   // DNS Addresses - Used by the HTTP layer, etc.
   //
   case CFGI_DNS_IP1:
   case CFGI_DNS_IP2:
#ifndef WIN32
      {
         nv_items_enum_type item = (i==CFGI_DNS_IP1 ? NV_PRIMARY_DNS_I : NV_SECONDARY_DNS_I);
         if ((!pBuff) || nSize != sizeof(uint32))
         {
            return EBADPARM;
         }
         
         nvi.primary_dns = NTOHL(*((uint32 *)pBuff));
         
         if (OEMNV_Put(item, &nvi) != NV_DONE_S)
         {
            DBGPRINTF(";put %d failed", item);
            return EFAILED;
         }
      }
#endif
      return AEE_SUCCESS;
   
   case CFGI_BREW_USERNAME:
      {
         if ((!pBuff) || WSTRLEN((AECHAR *)pBuff) > BREW_USERNAME_LEN)
         {
            return EFAILED;
         }

         (void)WSTRTOSTR((AECHAR *)pBuff, (char*)oemi_cache.brew_username, sizeof(oemi_cache.brew_username));
         OEMPriv_WriteOEMConfigList();
      }
      return AEE_SUCCESS;
      
   case CFGI_BREW_PASSWORD:
      {
         if ((!pBuff) || WSTRLEN((AECHAR *)pBuff) > BREW_PASSWORD_LEN)
         {
            return EFAILED;
         }
         oemi_cache.brew_password[0] = 0;
         (void)WSTRTOSTR((AECHAR *)pBuff, (char*)oemi_cache.brew_password, sizeof(oemi_cache.brew_password));
         OEMPriv_WriteOEMConfigList();
      }
      return AEE_SUCCESS;
    
   case CFGI_BREW_SERVER:
      {
         if ((!pBuff) || WSTRLEN((AECHAR *)pBuff) >= DL_MAX_SERVER)
         {
            return EFAILED;
         }
#ifndef WIN32        
         (void)WSTRTOSTR((AECHAR *)pBuff, (char*)nvi.brew_server, sizeof(nvi.brew_server));
         if (OEMNV_Put(NV_BREW_SERVER_I, &nvi) != NV_DONE_S)
         {
            DBGPRINTF(";put NVI_BREW_SERVER_I failed");
            return EFAILED;
         }
         (void)STRCPY((char *)nvi_cache.brew_server, (char *)nvi.brew_server);
#endif
      }
      return AEE_SUCCESS;
    
   case CFGI_BREW_CARRIER_ID:
      {
         if ((!pBuff) || nSize != sizeof(dword))
         {
            return EFAILED;
         }
#ifndef WIN32         
         nvi.brew_carrier_id = *((dword *)pBuff);
         
         if (OEMNV_Put(NV_BREW_CARRIER_ID_I, &nvi) != NV_DONE_S)
         {
            DBGPRINTF(";put NVI_BREW_CARRIER_ID_I failed");
            return EFAILED;
         }
         nvi_cache.brew_carrier_id = nvi.brew_carrier_id;
#endif
      }
      return AEE_SUCCESS;
    
   case CFGI_BREW_BKEY:
      {
         int k;
         AECHAR *pWStr = (AECHAR *)pBuff;
         
         if ((!pWStr) || WSTRLEN(pWStr) >= DL_BKEY_SIZE)
         {
            return EFAILED;
         }
#ifndef WIN32         
         for (k=0; k<WSTRLEN(pWStr); k++)
         {
            nvi.brew_bkey[k] = (byte)pWStr[k];
         }
         nvi.brew_bkey[k] = 0;
         
         if (OEMNV_Put(NV_BREW_BKEY_I, &nvi) != NV_DONE_S)
         {
            DBGPRINTF(";put NVI_BREW_BKEY_I failed");
            return EFAILED;
         }
         (void)STRCPY((char *)nvi_cache.brew_bkey, (char *)nvi.brew_bkey);
#endif
      }
      return AEE_SUCCESS;
    
   case CFGI_BREW_AUTH_POLICY:
      {
         if ((!pBuff) || nSize != sizeof(uint8))
         {
            return EFAILED;
         }
#ifndef WIN32         
         nvi.brew_auth_policy = *(uint8 *)pBuff;
         
         if (OEMNV_Put(NV_BREW_AUTH_POLICY_I, &nvi) != NV_DONE_S)
         {
            DBGPRINTF(";put NVI_BREW_AUTH_POLICY_I failed");
            return EFAILED;
         }
         nvi_cache.brew_auth_policy = nvi.brew_auth_policy;
#endif
      }
      return AEE_SUCCESS;
    
   case CFGI_BREW_PRIVACY_POLICY:
      {
         if ((!pBuff) || nSize != sizeof(uint8))
         {
            return EFAILED;
         }
#ifndef WIN32        
         nvi.brew_privacy_policy = *(uint8 *)pBuff;
         
         if (OEMNV_Put(NV_BREW_PRIVACY_POLICY_I, &nvi) != NV_DONE_S)
         {
            DBGPRINTF(";put NVI_BREW_PRIVACY_POLICY_I failed");
            return EFAILED;
         }
         nvi_cache.brew_privacy_policy = nvi.brew_privacy_policy;
#endif
      }
      return AEE_SUCCESS;
    
   /*
   case CFGI_BREW_SUBSCRIBER_ID:
      {
         int k;
         AECHAR *pWStr = (AECHAR *)pBuff;
         
         if ((!pWStr) || WSTRLEN(pWStr) >= NV_BREW_SID_SIZ)
         {
            return EFAILED;
         }
         
         for (k=0; k<WSTRLEN(pWStr); k++)
         {
            nvi.brew_subscriber_id[k] = (byte)pWStr[k];
         }
         nvi.brew_subscriber_id[k] = 0;
         
         if (OEMNV_Put(NV_BREW_SUBSCRIBER_ID_I, &nvi) != NV_DONE_S)
         {
            DBGPRINTF(";put CFGI_BREW_SUBSCRIBER_ID failed");
            return EFAILED;
         }
         (void)STRCPY((char *)nvi_cache.brew_subscriber_id, (char *)nvi.brew_subscriber_id);
      }
      return AEE_SUCCESS;
   */
      
   case CFGI_BREW_PLATFORM_ID:
      {
         if ((!pBuff) || nSize != sizeof(dword))
         {
            return EFAILED;
         }
#ifndef WIN32         
         nvi.brew_platform_id = *(dword *)pBuff;
         
         if (OEMNV_Put(NV_BREW_PLATFORM_ID_I, &nvi) != NV_DONE_S)
         {
            DBGPRINTF(";put NV_BREW_PLATFORM_ID_I failed");
            return EFAILED;
         }
         nvi_cache.brew_platform_id = nvi.brew_platform_id;
#endif
      }
      return AEE_SUCCESS;
      
   case CFGI_BREW_TESTOPT:              // DIF_TEST_ALLOWED  
   case CFGI_BREW_USEAKEY:              // DIF_USE_A_KEY
   case CFGI_BREW_AUTOUPGRADE_FLG:      // DIF_AUTO_UPGRADE 
   case CFGI_BREW_USEMINFORSID_FLG:     // DIF_MIN_FOR_SID
   case CFGI_BREW_PREPAY_FLG:           // DIF_PREPAY
   case CFGI_BREW_NOAUTOACK_FLG:        // DIF_NO_AUTO_ACK
   case CFGI_BREW_SIDENCODE_FLG:        // DIF_SID_ENCODE
   case CFGI_BREW_SIDVALIDATAALL_FLG:   // DIF_SID_VALIDATE_ALL
   case CFGI_BREW_IDS_RUIMDELETE_FLG:   // DIF_RUIM_DEL_OVERRIDE
      {
         if ((!pBuff) || nSize != sizeof(boolean))
         {
            return EFAILED;
         }
#ifndef WIN32         
         if (OEMNV_Get(NV_BREW_DOWNLOAD_FLAGS_I, &nvi) != NV_DONE_S)
         {
            DBGPRINTF(";put NV_BREW_DOWNLOAD_FLAGS_I failed");
            nvi.brew_download_flags = 0;
         }
         switch (i)
         {
            case CFGI_BREW_TESTOPT:             // DIF_TEST_ALLOWED  
                if (*(boolean *)pBuff)
                {
                    nvi.brew_download_flags |= DIF_TEST_ALLOWED;
                }
                else
                {
                    nvi.brew_download_flags &= ~DIF_TEST_ALLOWED;
                }
                break;
                
            case CFGI_BREW_USEAKEY:              // DIF_USE_A_KEY
                if (*(boolean *)pBuff)
                {
                    nvi.brew_download_flags |= DIF_USE_A_KEY;
                }
                else
                {
                    nvi.brew_download_flags &= ~DIF_USE_A_KEY;
                }
                break;
            
            case CFGI_BREW_AUTOUPGRADE_FLG:      // DIF_AUTO_UPGRADE 
                if (*(boolean *)pBuff)
                {
                    nvi.brew_download_flags |= DIF_AUTO_UPGRADE;
                }
                else
                {
                    nvi.brew_download_flags &= ~DIF_AUTO_UPGRADE;
                }
                break;
                
            case CFGI_BREW_USEMINFORSID_FLG:     // DIF_MIN_FOR_SID
                if (*(boolean *)pBuff)
                {
                    nvi.brew_download_flags |= DIF_MIN_FOR_SID;
                }
                else
                {
                    nvi.brew_download_flags &= ~DIF_MIN_FOR_SID;
                }
                break;
                
            case CFGI_BREW_PREPAY_FLG:           // DIF_PREPAY
                if (*(boolean *)pBuff)
                {
                    nvi.brew_download_flags |= DIF_PREPAY;
                }
                else
                {
                    nvi.brew_download_flags &= ~DIF_PREPAY;
                }
                break;
                
            case CFGI_BREW_NOAUTOACK_FLG:        // DIF_NO_AUTO_ACK
                if (*(boolean *)pBuff)
                {
                    nvi.brew_download_flags |= DIF_NO_AUTO_ACK;
                }
                else
                {
                    nvi.brew_download_flags &= ~DIF_NO_AUTO_ACK;
                }
                break;
                
            case CFGI_BREW_SIDENCODE_FLG:        // DIF_SID_ENCODE
                if (*(boolean *)pBuff)
                {
                    nvi.brew_download_flags |= DIF_SID_ENCODE;
                }
                else
                {
                    nvi.brew_download_flags &= ~DIF_SID_ENCODE;
                }
                break;
                
            case CFGI_BREW_SIDVALIDATAALL_FLG:   // DIF_SID_VALIDATE_ALL
                if (*(boolean *)pBuff)
                {
                    nvi.brew_download_flags |= DIF_SID_VALIDATE_ALL;
                }
                else
                {
                    nvi.brew_download_flags &= ~DIF_SID_VALIDATE_ALL;
                }
                break;
                
            case CFGI_BREW_IDS_RUIMDELETE_FLG:   // DIF_RUIM_DEL_OVERRIDE
                if (*(boolean *)pBuff)
                {
                    nvi.brew_download_flags |= DIF_RUIM_DEL_OVERRIDE;
                }
                else
                {
                    nvi.brew_download_flags &= ~DIF_RUIM_DEL_OVERRIDE;
                }
                break;
                
            default:
                break;
         }

         if (OEMNV_Put(NV_BREW_DOWNLOAD_FLAGS_I, &nvi) != NV_DONE_S)
         {
            DBGPRINTF(";put NVI_BREW_DOWNLOAD_FLAGS_I failed");
            return EFAILED;
         }
         nvi_cache.brew_download_flags = nvi.brew_download_flags;
#endif
      }
      return AEE_SUCCESS;
   
   default:
      return(EUNSUPPORTED);
   }
}

/*==================================================================
Function: OEM_GetPlatformID

Description: Retrieves Platform ID from OEM (required by 
             OEM_GetDeviceInfo.

Prototype:
   void  OEM_SetCachedConfig(uint32 *pPlatformID)

Parameter(s):

Return Value:  EBADPARM or SUCCESS
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
int OEM_GetPlatformID(uint32 *pPlatformID)
{
   if(!pPlatformID)
      return(EBADPARM);

   *pPlatformID = nvi_cache.brew_platform_id;
   return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMPriv_WriteOEMConfigListCB

DESCRIPTION: Callback that actually writes the OEM config list to EFS

PARAMETERS:
   *pUnused: Unused 

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMPriv_WriteOEMConfigListCB(void *pUnused)
{
   void   *fp;
   //uint16  version = OEMCONFIGLIST_VERSION;
   char version[30]={0};
   
#if MIN_BREW_VERSION(3,0)
   int nErr = EFAILED;
   nErr = OEMFS_Open(OEMCONFIGLIST_FILE, _OFM_READWRITE, &fp);
  if (nErr != SUCCESS) {
      nErr = OEMFS_Open(OEMCONFIGLIST_FILE, _OFM_CREATE, &fp);
      if (nErr != SUCCESS) {
#ifndef WIN32//wlh 
         ERR("Unable to create OEM config file", 0, 0, 0);
#endif//WIN32
         return;
    }
   }
   (void)STRCPY(version, ver_date);
   (void)STRCAT(version, ver_time);
   (void) OEMFS_Seek(fp, _SEEK_START, 0, &nErr);
   (void) OEMFS_Write(fp, &version, sizeof(version), &nErr);
   (void) OEMFS_Write(fp, (void *) &oemi_cache, sizeof(oemi_cache), &nErr);
#else
   fp = OEMFS_Open(OEMCONFIGLIST_FILE, _OFM_READWRITE);
   if (NULL == fp) {
      fp = OEMFS_Open(OEMCONFIGLIST_FILE, _OFM_CREATE);
      if (NULL == fp) {
         ERR("Unable to create OEM config file", 0, 0, 0);
         return;
      }
   }
   (void)STRCPY(version, ver_date);
   (void)STRCAT(version, ver_time);
   (void) OEMFS_Seek(fp, _SEEK_START, 0);
   (void) OEMFS_Write(fp, &version, sizeof(version));
   (void) OEMFS_Write(fp, (void *) &oemi_cache, sizeof(oemi_cache));
#endif
   (void) OEMFS_Close(fp);
}

/*=============================================================================
FUNCTION: OEMPriv_WriteOEMConfigList

DESCRIPTION: Saves the OEM configuration list to EFS

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMPriv_WriteOEMConfigList(void)
{
   static boolean cbInitialized = FALSE;

   if (!cbInitialized) {
      CALLBACK_Init(&gCBWriteOEMConfig, 
                    OEMPriv_WriteOEMConfigListCB,
                    NULL);
      cbInitialized = TRUE;
   }

   // Queue an AEE Callback instead of immediately writing the config
   // data out to EFS.  This prevents unnecessary EFS writes if the caller
   // is making numerious sequential calls to ICONFIG_SetItem()
#if MIN_BREW_VERSION(3,0)
   AEE_APP_RESUME(&gCBWriteOEMConfig, NULL);
#else
   AEE_ResumeCallback(&gCBWriteOEMConfig,
                      AEE_RESUME_CB_SYS|AEE_RESUME_CB_PRIO);
#endif
}


/*=============================================================================
FUNCTION: OEMPriv_ReadOEMConfigList

DESCRIPTION: Reads the OEM configuration list from EFS

PARAMETERS:
   None

RETURN VALUE:
   int

COMMENTS:
   增加返回值。OEMCONFIG_UPDATED 表示配置版本有升级。
   其余情况为 0

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMPriv_ReadOEMConfigList(void)
{
   void   *fp;
   int  nRet = 0;
   //uint16  version;
   char version[30]={0};
   char szCompileDT[30]={0};
   
#if MIN_BREW_VERSION(3,0)
  int nErr = EFAILED;

   //nErr = OEMFS_Open(OEMCONFIGLIST_FILE, _OFM_READ, &fp);
   nErr = OEMFS_Open(OEMCONFIGLIST_FILE, _OFM_READWRITE, &fp);
   if (nErr != SUCCESS) {
      // Config file does not exist yet.  No big deal, just
      // use the defaults
      nRet = OEMCONFIG_UPDATED;
      return nRet;
   }

   (void) OEMFS_Seek(fp, _SEEK_START, 0, &nErr);
   (void) OEMFS_Read(fp, &version, sizeof(version), &nErr);
   /*
   if (version != OEMCONFIGLIST_VERSION) {
      // Different version.  Ignore it and use the defaults
   (void) OEMFS_Close(fp);
      return;
   }
   (void) OEMFS_Read(fp, (void *) &oemi_cache, sizeof(oemi_cache), &nErr);
   */
   version[29]=0;
   (void)STRCPY(szCompileDT, ver_date);
   (void)STRCAT(szCompileDT, ver_time);
   if (STRCMP(version, szCompileDT)!=0)
   { // 版本不配, 重建 OEM 配置
      (void) OEMFS_Truncate(fp,0);
      (void) OEMFS_Seek(fp, _SEEK_START, 0, &nErr);
      (void) OEMFS_Write(fp, szCompileDT, sizeof(szCompileDT), &nErr);
      (void) OEMFS_Write(fp, (void *) &oemi_cache, sizeof(oemi_cache), &nErr);
      nRet = OEMCONFIG_UPDATED;
#ifdef FEATURE_LOAD_DEFAULT_SETTING      
      if (OEMFS_Test (AEEFS_ROOT_DIR OEM_BAKFILE) == SUCCESS)
      {
           OEMFS_Rename(AEEFS_ROOT_DIR OEM_BAKFILE,AEEFS_ROOT_DIR OEM_INIFILE);
      }
#endif //#ifdef FEATURE_LOAD_DEFAULT_SETTING      
   }
   else
   {
      (void) OEMFS_Read(fp, (void *) &oemi_cache, sizeof(oemi_cache), &nErr);
   }
   
#else
   //fp = OEMFS_Open(OEMCONFIGLIST_FILE, _OFM_READ);
   fp = OEMFS_Open(OEMCONFIGLIST_FILE, _OFM_READWRITE);
   if (NULL == fp) {
      // Config file does not exist yet.  No big deal, just
      // use the defaults
      nRet = OEMCONFIG_UPDATED;
      return nRet;
   }

   (void) OEMFS_Seek(fp, _SEEK_START, 0);
   (void) OEMFS_Read(fp, &version, sizeof(version));
   /*
   if (version != OEMCONFIGLIST_VERSION) {
      // Different version.  Ignore it and use the defaults
      (void) OEMFS_Close(fp);
      return;
   }
   (void) OEMFS_Read(fp, (void *) &oemi_cache, sizeof(oemi_cache));
   */
   version[29]=0;
   (void)STRCPY(szCompileDT, ver_date);
   (void)STRCAT(szCompileDT, ver_time);
   if (STRCMP(version, szCompileDT)!=0)
   { // 版本不配, 重建 OEM 配置
      (void) OEMFS_Truncate(fp,0);
      (void) OEMFS_Seek(fp, _SEEK_START, 0);
      (void) OEMFS_Write(fp, szCompileDT, sizeof(szCompileDT));
      (void) OEMFS_Write(fp, (void *) &oemi_cache, sizeof(oemi_cache));
      nRet = OEMCONFIG_UPDATED;
#ifdef FEATURE_LOAD_DEFAULT_SETTING      
      if (OEMFS_Test (AEEFS_ROOT_DIR OEM_BAKFILE) == SUCCESS)
      {
           OEMFS_Rename(AEEFS_ROOT_DIR OEM_BAKFILE,AEEFS_ROOT_DIR OEM_INIFILE);
      }
#endif //#ifdef FEATURE_LOAD_DEFAULT_SETTING      
   }
   else
   {
      (void) OEMFS_Read(fp, (void *) &oemi_cache, sizeof(oemi_cache));
   }
#endif
   (void) OEMFS_Close(fp);

   //
   // Verify the config file data is valid
   //
#if (defined FEATURE_CARRIER_THAILAND_CAT) || (defined FEATURE_CARRIER_THAILAND_HUTCH)
   if (oemi_cache.alert_type > OEMNV_ALERTTYPE_VIBANDRINGER) {
      oemi_cache.alert_type = OEMNV_ALERTTYPE_RINGER;
   }
#else // FEATURE_CARRIER_THAILAND_CAT  // kai.yao modify  2009.03.17   m91 need  vib and ringer
   if (oemi_cache.alert_type > OEMNV_ALERTTYPE_VIBANDRINGER) {
      oemi_cache.alert_type = OEMNV_ALERTTYPE_RINGER;
   }
#endif
   
   if (oemi_cache.silence_all > OEMNV_SA_LITONLY) {
      oemi_cache.silence_all = OEMNV_SA_NORMAL;
   }

   if (oemi_cache.sms_service_option > OEMNV_SMS_SO_SO14) {
      oemi_cache.sms_service_option = OEMNV_SMS_SO_SO6;
   }

   if (oemi_cache.sms_is41_workaround > OEMNV_SMS_EMAIL_ADDRESSING_STANDARD) { 
      oemi_cache.sms_is41_workaround = OEMNV_SMS_EMAIL_ADDRESSING_IS41;
   }

   /*
   if (oemi_cache.sms_mo_encoding > OEMNV_SMS_MO_ENCODING_UNICODE) { 
      oemi_cache.sms_mo_encoding = OEMNV_SMS_MO_ENCODING_7BIT;
   }
   */
   switch (oemi_cache.sms_mo_encoding)
   {
      case OEMNV_SMS_MO_ENCODING_7BIT:
#if defined(FEATURE_CARRIER_ANGOLA_MOVICEL) || defined(FEATURE_CARRIER_MAROC_WANA) || defined (FEATURE_CARRIER_ISRAEL_PELEPHONE)
      case OEMNV_SMS_MO_ENCODING_OCTET:
#else        
      case OEMNV_SMS_MO_ENCODING_UNICODE:
#endif        
          break;
          
      default:
          oemi_cache.sms_mo_encoding = OEMNV_SMS_MO_ENCODING_7BIT;
          break;
   }

   if (oemi_cache.sms_mode > OEMNV_SMS_MODE_GW) { 
      oemi_cache.sms_mode = OEMNV_SMS_MODE_CDMA;
   }

   if (oemi_cache.sms_app > OEMNV_SMS_APP_TEST) { 
      oemi_cache.sms_mode = OEMNV_SMS_APP_TEST;
   }

   if (oemi_cache.sms_cb_auto_delete > OEMNV_SMS_CB_ENABLE_AUTO_DELETE) { 
      oemi_cache.sms_cb_auto_delete = OEMNV_SMS_CB_DISABLE_AUTO_DELETE;
   }

   if (oemi_cache.sms_cb_silent_alert > OEMNV_SMS_CB_ENABLE_SILENT_ALERT) { 
      oemi_cache.sms_cb_silent_alert = OEMNV_SMS_CB_DISABLE_SILENT_ALERT;
   }

   if (oemi_cache.sms_cb_lang_filter > OEMNV_SMS_CB_ENABLE_LANG_FILTER) { 
      oemi_cache.sms_cb_lang_filter = OEMNV_SMS_CB_DISABLE_LANG_FILTER;
   }

   if (oemi_cache.sms_alert_display > OEMNV_SMS_ALERT_DISPLAY_ENABLE) {
      oemi_cache.sms_alert_display = OEMNV_SMS_ALERT_DISPLAY_ENABLE;
   }

   if (oemi_cache.sms_alert_sound > OEMNV_SMS_ALERT_SOUND_ENABLE) {
      oemi_cache.sms_alert_sound = OEMNV_SMS_ALERT_SOUND_ENABLE;
   }

   if (oemi_cache.sms_alert_volume > OEMNV_SMS_ALERT_VOLUME_4) {
      oemi_cache.sms_alert_volume = OEMNV_SMS_ALERT_VOLUME_2;
   }
   //if (oemi_cache.time_format > OEMNV_TIMEFORM_24HR) {
   //   oemi_cache.time_format = OEMNV_TIMEFORM_AMPM;
   //}
   
   //#ifdef FEATURE_TIME_DATA_SETTING
   //   if (oemi_cache.date_format > OEMNV_DATEFORM_YMD) {
   //      oemi_cache.date_format = OEMNV_DATEFORM_YMD;
   //   }
   //#endif
   oemi_cache.device_mask = 0;

   MEMSET((void *)&oemi_cache.cug, 0, sizeof(OEMConfigCugInfo));
	
   return nRet;
}


/*===========================================================================

                     CONFIG ITEM GET/SET FUNCTION DEFINITIONS

===========================================================================*/


/*=============================================================================
FUNCTION: OEMPriv_GetItem_CFGI_SILENCEALL

DESCRIPTION:  Gets the current value of the CFGI_SILENCEALL config item.

PARAMETERS:
   *pBuff: Pointer to a buffer to store the config value

RETURN VALUE:
   int:  SUCCESS if successful, otherwise a BREW error code.

COMMENTS:
   Note that pBuff may be assumed to point to a correctly sized buffer for
   this config item.  OEM_GetConfig() peforms a size check before invoking
   this function.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMPriv_GetItem_CFGI_SILENCEALL(void *pBuff) 
{
   *(byte *) pBuff = oemi_cache.silence_all;
   return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMPriv_SetItem_CFGI_SILENCEALL

DESCRIPTION:  Sets the CFGI_SILENCEALL config item.

PARAMETERS:
   *pBuff: Pointer to the new config value

RETURN VALUE:
   int:  SUCCESS if successful, otherwise a BREW error code.

COMMENTS:
   Note that pBuff may be assumed to point to a correctly sized buffer for
   this config item.  OEM_SetConfig() peforms a size check before invoking
   this function.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMPriv_SetItem_CFGI_SILENCEALL(void *pBuff) 
{
   if (oemi_cache.silence_all != *(byte *)pBuff) {
      oemi_cache.silence_all = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_RECENT_MT_CALL_TIMER(void *pBuff) 
{
   *(dword *) pBuff = oemi_cache.recent_mt_call_time;
   return SUCCESS;
}


static int OEMPriv_SetItem_CFGI_RECENT_MT_CALL_TIMER(void *pBuff) 
{
   if (oemi_cache.recent_mt_call_time != *(dword *)pBuff) {
      oemi_cache.recent_mt_call_time = *(dword *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_RECENT_MO_CALL_TIMER(void *pBuff) 
{
   *(dword *) pBuff = oemi_cache.recent_mo_call_time;
   return SUCCESS;
}


static int OEMPriv_SetItem_CFGI_RECENT_MO_CALL_TIMER(void *pBuff) 
{
   if (oemi_cache.recent_mo_call_time != *(dword *)pBuff) {
      oemi_cache.recent_mo_call_time = *(dword *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_ALL_CALL_TIMER(void *pBuff) 
{
   *(dword *) pBuff = nvi_cache.all_call_time;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_ALL_CALL_TIMER(void *pBuff) 
{
#ifndef WIN32
   nv_item_type nvi;

   if (nvi_cache.all_call_time != *((dword *)pBuff)) {
      nvi_cache.all_call_time = *((dword *)pBuff);
      nvi.life_timer_g.time = *((dword *)pBuff);
      (void) OEMNV_Put( NV_LIFE_TIMER_G_I, &nvi);
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_BROWSER_CALL_TIMER(void *pBuff) 
{
   *(dword *) pBuff = oemi_cache.internet_call_time;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_BROWSER_CALL_TIMER(void *pBuff) 
{
   if (oemi_cache.internet_call_time != *(dword *)pBuff) {
      oemi_cache.internet_call_time = *((dword *)pBuff);
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_EXTPWR_BK_LIGHT(void *pBuff) 
{
   *(byte *) pBuff = nvi_cache.backlight_hfk;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_EXTPWR_BK_LIGHT(void *pBuff) 
{
#ifndef WIN32
   nv_item_type nvi;

   if (nvi_cache.backlight_hfk != *((byte *)pBuff)) {
      nvi_cache.backlight_hfk = *((byte *)pBuff);
      nvi.back_light_hfk = *((byte *)pBuff);
      (void) OEMNV_Put( NV_BACK_LIGHT_HFK_I, &nvi );
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_HEADSET_RNG(void *pBuff) 
{
   *(byte *)pBuff = oemi_cache.headset_ringer;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_HEADSET_RNG(void *pBuff) 
{
   if (oemi_cache.headset_ringer != *(byte *)pBuff) {
      oemi_cache.headset_ringer = *((byte *)pBuff);
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_ANYKEY_ANSWER(void *pBuff) 
{
   *(boolean *)pBuff = oemi_cache.any_key_answer;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_ANYKEY_ANSWER(void *pBuff) 
{
   if (oemi_cache.any_key_answer != *(byte *)pBuff) {
      oemi_cache.any_key_answer = *((byte *)pBuff);
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_HEADSET_AUTOANSWER(void *pBuff) 
{
   *(byte *)pBuff = oemi_cache.headset_autoanswer;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_HEADSET_AUTOANSWER(void *pBuff) 
{
   if (oemi_cache.headset_autoanswer != *(byte *)pBuff) {
      oemi_cache.headset_autoanswer = *((byte *)pBuff);
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_MISSED_CALL_ALERT(void *pBuff) 
{
   *(byte *)pBuff = oemi_cache.missed_call_alert;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MISSED_CALL_ALERT(void *pBuff) 
{
   if (oemi_cache.missed_call_alert != *(byte *)pBuff) {
      oemi_cache.missed_call_alert = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_ROAMING_ALERT(void *pBuff) 
{
   *(byte *)pBuff = oemi_cache.roaming_alert;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_ROAMING_ALERT(void *pBuff) 
{
   if (oemi_cache.roaming_alert != *(byte *)pBuff) {
      oemi_cache.roaming_alert = *((byte *)pBuff);
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SVC_ALERT(void *pBuff) 
{
   *((byte *)pBuff) = nvi_cache.svc_alert;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SVC_ALERT(void *pBuff) 
{
#ifndef WIN32
   byte b = *((byte *)pBuff) ? 1: 0;
   nv_item_type nvi;

   if (nvi_cache.svc_alert != b) {
      nvi_cache.svc_alert = b;
      nvi.svc_area_alert = b;
      (void) OEMNV_Put( NV_SVC_AREA_ALERT_I, &nvi );
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_INTERNET_ALERT(void *pBuff) 
{
   *(byte *)pBuff = oemi_cache.internet_call_alert;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_INTERNET_ALERT(void *pBuff) 
{
   if (oemi_cache.internet_call_alert != *(byte *)pBuff) {
      oemi_cache.internet_call_alert = *((byte *)pBuff);
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_MINUTE_ALERT(void *pBuff) 
{
   *(boolean *) pBuff = oemi_cache.minute_alert;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MINUTE_ALERT(void *pBuff) 
{
   if (oemi_cache.minute_alert != *(boolean *)pBuff) {
      oemi_cache.minute_alert = *(boolean *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}



static int OEMPriv_GetItem_CFGI_RINGER_VOL(void *pBuff) 
{
   *(byte *) pBuff = (byte)nvi_cache.ringer_level;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_RINGER_VOL(void *pBuff) 
{
#ifndef WIN32
   nv_item_type nvi;

   if (nvi_cache.ringer_level != *((byte *)pBuff)) {
      nvi_cache.ringer_level = *((byte *)pBuff);
      nvi.ringer_level = *((byte *)pBuff);
      (void) OEMNV_Put( NV_RINGER_LVL_I, &nvi);
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_EAR_VOL(void *pBuff) 
{
   *(byte *) pBuff = (byte)nvi_cache.handset_ear_level;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_EAR_VOL(void *pBuff) 
{
#ifndef WIN32
   nv_item_type nvi;

   if (nvi_cache.handset_ear_level != *((byte *)pBuff)) {
      nvi_cache.handset_ear_level = *((byte *)pBuff);
      nvi.ear_level = *((byte *)pBuff);
      (void) OEMNV_Put( NV_EAR_LVL_I, &nvi);
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_BEEP_VOL(void *pBuff) 
{
   *(byte *) pBuff = (byte)nvi_cache.beep_level;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_BEEP_VOL(void *pBuff) 
{
#ifndef WIN32
   nv_item_type nvi;

   if (nvi_cache.beep_level != *((byte *)pBuff)) {
      nvi_cache.beep_level = *((byte *)pBuff);
      nvi.beep_level = *((byte *)pBuff);
      (void) OEMNV_Put( NV_BEEP_LVL_I, &nvi);
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_ALERT_TYPE(void *pBuff) 
{
   *(byte *)pBuff = oemi_cache.alert_type;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_ALERT_TYPE(void *pBuff) 
{
   if (oemi_cache.alert_type != *((byte *)pBuff)) {
      oemi_cache.alert_type = *((byte *)pBuff);
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

#ifdef FEATURE_SMART_SOUND
static int OEMPriv_GetItem_CFGI_SMART_SOUND(void *pBuff) 
{
   *(boolean *) pBuff = nvi_cache.smart_sound;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMART_SOUND(void *pBuff) 
{
   nv_item_type nvi;

   if (nvi_cache.smart_sound != *((boolean *)pBuff)) {
      nvi_cache.smart_sound = *((boolean *)pBuff);
      nvi.auto_volume_enabled = *((boolean *)pBuff);
      (void) OEMNV_Put( NV_AUTO_VOLUME_ENABLED_I, &nvi);
   }

   return SUCCESS;
}
#endif

static int OEMPriv_GetItem_CFGI_KEYBEEP_SOUND(void *pBuff) 
{
   *(byte *) pBuff = nvi_cache.key_sound;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_KEYBEEP_SOUND(void *pBuff) 
{
#ifndef WIN32
   nv_item_type nvi;

   if (nvi_cache.key_sound != *((byte *)pBuff)) {
      nvi_cache.key_sound = *((byte *)pBuff);
      nvi.key_sound = (nv_key_sound_enum_type) *((byte *)pBuff);
      (void) OEMNV_Put( NV_KEY_SOUND_I, &nvi);
   }
#endif
   return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_RINGER_TYPE(void *pBuff) 
{
    // To do - ringer type
    *(byte *) pBuff = nvi_cache.ringer_type;
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_RINGER_TYPE(void *pBuff) 
{
#ifndef WIN32
   nv_item_type nvi;

   if (nvi_cache.ringer_type != *((byte *)pBuff)) {
      nvi_cache.ringer_type = *((byte *)pBuff);
      nvi.ringer_type = *((byte *)pBuff);
      (void) OEMNV_Put( NV_RINGER_TYPE_I, &nvi);
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_IMEI(void *pBuff) 
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#else
   ((AECHAR *) pBuff)[0] = (AECHAR) 0;
   return EUNSUPPORTED;
#endif
}

static int OEMPriv_SetItem_CFGI_IMEI(void *pBuff) 
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#else
   return EUNSUPPORTED;
#endif
}

static int OEMPriv_GetItem_CFGI_CUG(void *pBuff) 
{
   MEMCPY(pBuff, (void*) &oemi_cache.cug, sizeof(OEMConfigCugInfo));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CUG(void *pBuff) 
{
   MEMCPY((void*) &oemi_cache.cug, pBuff, sizeof(OEMConfigCugInfo));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_VR(void *pBuff) 
{
  *(boolean *) pBuff = oemi_cache.vr;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_VR(void *pBuff) 
{
  if (oemi_cache.vr != *(boolean *)pBuff) {
      oemi_cache.vr  = *(boolean *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;						   
}
static int OEMPriv_GetItem_CFGI_AMR(void *pBuff) 
{
  *(boolean *) pBuff = nvi_cache.amr;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_AMR(void *pBuff) 
{
#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif

  if (nvi_cache.amr != *((boolean *)pBuff)) {
      nvi_cache.amr = *((boolean *)pBuff);
#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif
  }
   return SUCCESS;								   
}

static int OEMPriv_GetItem_CFGI_TTY(void *pBuff) 
{
  *(OEMTTY *) pBuff = nvi_cache.tty;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_TTY(void *pBuff) 
{
#ifndef WIN32
  nv_item_type nvi;

  if (nvi_cache.tty != *((OEMTTY *)pBuff)) {
      nvi_cache.tty = *((OEMTTY *)pBuff);
     (void) OEMNV_Put( NV_TTY_I, &nvi);
  }
#endif
  return SUCCESS;								   
}


static int OEMPriv_GetItem_CFGI_CDMA_RX_DIVERSITY_CTL(void *pBuff)
{
#ifdef FEATURE_CDMA_RX_DIVERSITY
   *(boolean *) pBuff = nvi_cache.cdma_rx_diversity_ctrl;
   return SUCCESS;
#else
  return EUNSUPPORTED;
#endif
}

static int OEMPriv_SetItem_CFGI_CDMA_RX_DIVERSITY_CTL(void *pBuff)
{
#ifdef FEATURE_CDMA_RX_DIVERSITY
  nv_item_type nvi;
  if (nvi_cache.cdma_rx_diversity_ctrl != *((boolean *)pBuff)) {
      nvi_cache.cdma_rx_diversity_ctrl = *((boolean *)pBuff);
      nvi.cdma_rx_diversity_ctrl = *((boolean *)pBuff);
      (void) OEMNV_Put( NV_CDMA_RX_DIVERSITY_CTRL_I, &nvi);
  }
   return SUCCESS;
#else
  return EUNSUPPORTED;
#endif
}

static int OEMPriv_GetItem_CFGI_HDR_RX_DIVERSITY_CTL(void *pBuff)
{
#ifdef FEATURE_HDR
#error code not present
#else
   return EUNSUPPORTED;
#endif
}

static int OEMPriv_SetItem_CFGI_HDR_RX_DIVERSITY_CTL(void *pBuff)
{
#ifdef FEATURE_HDR
#error code not present
#else
  return EUNSUPPORTED;
#endif
}

static int OEMPriv_GetItem_CFGI_LAST_RX_DATA_COUNT(void *pBuff)
{
  *(uint32 *) pBuff = nvi_cache.last_rx_data_count;
  return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_LAST_RX_DATA_COUNT(void *pBuff)
{
#ifndef WIN32
  nv_item_type nvi;
  if (nvi_cache.last_rx_data_count != *((uint32 *)pBuff)) {
    nvi_cache.last_rx_data_count = *((uint32 *)pBuff);
    nvi.last_rx_data_count = *((uint32 *)pBuff);
    (void) OEMNV_Put(NV_LAST_RX_DATA_COUNT_I, &nvi);
  }
#endif
  return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_LAST_TX_DATA_COUNT(void *pBuff)
{
  *(uint32 *) pBuff = nvi_cache.last_tx_data_count;
  return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_LAST_TX_DATA_COUNT(void *pBuff)
{
#ifndef WIN32
  nv_item_type nvi;
  if (nvi_cache.last_tx_data_count != *((uint32 *)pBuff)) {
    nvi_cache.last_tx_data_count = *((uint32 *)pBuff);
    nvi.last_tx_data_count = *((uint32 *)pBuff);
    (void) OEMNV_Put(NV_LAST_TX_DATA_COUNT_I, &nvi);
  }
#endif
  return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_TOTAL_RX_DATA_COUNT(void *pBuff)
{
  *(uint32 *) pBuff = nvi_cache.total_rx_data_count;
  return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_TOTAL_RX_DATA_COUNT(void *pBuff)
{
#ifndef WIN32
  nv_item_type nvi;
  if (nvi_cache.total_rx_data_count != *((uint32 *)pBuff)) {
    nvi_cache.total_rx_data_count = *((uint32 *)pBuff);
    nvi.total_rx_data_count = *((uint32 *)pBuff);
    (void) OEMNV_Put(NV_TOTAL_RX_DATA_COUNT_I, &nvi);
  }
#endif
  return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_TOTAL_TX_DATA_COUNT(void *pBuff)
{
  *(uint32 *) pBuff = nvi_cache.total_tx_data_count;
  return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_TOTAL_TX_DATA_COUNT(void *pBuff)
{
#ifndef WIN32
  nv_item_type nvi;
  if (nvi_cache.total_tx_data_count != *((uint32 *)pBuff)) {
    nvi_cache.total_tx_data_count = *((uint32 *)pBuff);
    nvi.total_tx_data_count = *((uint32 *)pBuff);
    (void) OEMNV_Put(NV_TOTAL_TX_DATA_COUNT_I, &nvi);
  }
#endif
  return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_CONTRAST_LVL(void *pBuff) 
{
   *(byte *) pBuff = nvi_cache.contrast;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CONTRAST_LVL(void *pBuff) 
{
#ifndef WIN32
   nv_item_type nvi;

   if (nvi_cache.contrast != *((byte *)pBuff)) {
      nvi_cache.contrast = *((byte *)pBuff);
      nvi.lcd = *((byte *)pBuff);
      (void) OEMNV_Put( NV_LCD_I, &nvi );
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_BACK_LIGHT(void *pBuff)
{
   *(byte *) pBuff = nvi_cache.backlight;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_BACK_LIGHT(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if (nvi_cache.backlight != *((byte *)pBuff)) {
      nvi_cache.backlight = *((byte *)pBuff);
      nvi.back_light = *((byte *)pBuff);
      (void) OEMNV_Put( NV_BACK_LIGHT_I, &nvi );
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_LANGUAGE_SELECTION(void *pBuff)
{
   *(nv_language_enum_type *) pBuff = nvi_cache.language;
   return SUCCESS;
}


static int OEMPriv_SetItem_CFGI_LANGUAGE_SELECTION(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if( nvi_cache.language != *((nv_language_enum_type *)pBuff )) {
      nvi_cache.language = *((nv_language_enum_type *)pBuff );
      nvi.language_selection = *((nv_language_enum_type *)pBuff);
      (void) OEMNV_Put( NV_LANGUAGE_SELECTION_I, &nvi );

      // NOTE!
      // 
      // After setting the new language, the caller must call
      // ISHELL_CloseApplet(ps, TRUE) to exit all applets and return to 
      // the Idle applet as the fonts may be different for the new language.
      // Currently only the Idle applet keeps track of the language
      // and takes appropriate action when it is resumed.
      //
     (void) AEE_IssueSystemCallback(AEE_SCB_DEVICE_INFO_CHANGED);
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MENU_FORMAT(void *pBuff)
{
   *(byte *) pBuff = nvi_cache.menu_format;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MENU_FORMAT(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if (nvi_cache.menu_format != *((byte *)pBuff)) {
      nvi_cache.menu_format = *((byte *)pBuff);
      nvi.menu_format = (nv_menu_format_enum_type) *((byte *)pBuff);
      (void) OEMNV_Put( NV_MENU_FORMAT_I, &nvi );
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_TIME_FORMAT(void *pBuff)
{
   *(byte *) pBuff = nvi_cache.set_time_format;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_TIME_FORMAT(void *pBuff)
{
#ifndef WIN32
    nv_item_type nvi;

   if (nvi_cache.set_time_format != *((byte *)pBuff)) {
      nvi_cache.set_time_format = *((byte *)pBuff);
      nvi.set_time_format = *((nv_set_time_format_enum_type *)pBuff);
      (void) OEMNV_Put( NV_SET_TIME_FORMAT_I,  &nvi);
   }
#endif
   return SUCCESS;
   
   //if (oemi_cache.time_format != *(boolean *)pBuff) {
   //   oemi_cache.time_format = *(boolean *)pBuff;
   //   OEMPriv_WriteOEMConfigList();
   //}

   //return SUCCESS;
}

#ifdef FEATURE_TIME_DATA_SETTING
static int OEMPriv_GetItem_CFGI_DATE_FORMAT(void *pBuff)
{
   *(byte *) pBuff = nvi_cache.set_date_format;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_DATE_FORMAT(void *pBuff)
{
#ifndef WIN32
    nv_item_type nvi;

   if (nvi_cache.set_date_format != *((byte *)pBuff)) {
      nvi_cache.set_date_format = *((byte *)pBuff);
      nvi.set_date_format = *((nv_set_date_format_enum_type *)pBuff);
      (void) OEMNV_Put( NV_SET_DATE_FORMAT_I, &nvi);
   }
#endif
   return SUCCESS;
   //if (oemi_cache.date_format != *(byte *)pBuff) {
   //   oemi_cache.date_format = *(byte *)pBuff;
   //   OEMPriv_WriteOEMConfigList();
   //}

   //return SUCCESS;
}
#endif

static int OEMPriv_GetItem_CFGI_HEADSET(void *pBuff)
{
   if(oemi_cache.device_mask & OEMNV_HEADSET_MASK)
     *(boolean *) pBuff = TRUE;
   else{
     *(boolean *) pBuff = FALSE;
   }

   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_HEADSET(void *pBuff)
{
   if(*(boolean *) pBuff == TRUE){
     oemi_cache.device_mask |= OEMNV_HEADSET_MASK;
   }
   else{
     oemi_cache.device_mask &= ~OEMNV_HEADSET_MASK;
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_STEREO_HEADSET(void *pBuff)
{
   if(oemi_cache.device_mask & OEMNV_STEREO_HEADSET_MASK)
     *(boolean *) pBuff = TRUE;
   else{
    *(boolean *) pBuff= FALSE;
   }

   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_STEREO_HEADSET(void *pBuff)
{
   if(*(boolean *) pBuff == TRUE){
     oemi_cache.device_mask |= OEMNV_STEREO_HEADSET_MASK;
   }
   else{
     oemi_cache.device_mask &= ~OEMNV_STEREO_HEADSET_MASK;
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_HFK(void *pBuff)
{
   if(oemi_cache.device_mask & OEMNV_ANALOG_HFK_MASK)
     *(boolean *)  pBuff = TRUE;
   else{
     *(boolean *)  pBuff = FALSE;
   }

   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_HFK(void *pBuff)
{
   if(*(boolean *) pBuff == TRUE){
     oemi_cache.device_mask |= OEMNV_ANALOG_HFK_MASK;
   }
   else{
     oemi_cache.device_mask &= ~OEMNV_ANALOG_HFK_MASK;
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_USB_HFK(void *pBuff)
{
   if(oemi_cache.device_mask & OEMNV_USB_HFK_MASK)
     *(boolean *) pBuff = TRUE;
   else{
     *(boolean *) pBuff = FALSE;
   }

   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_USB_HFK(void *pBuff)
{
   if(*(boolean *) pBuff == TRUE){
     oemi_cache.device_mask |= OEMNV_USB_HFK_MASK;
   }
   else{
     oemi_cache.device_mask &= ~OEMNV_USB_HFK_MASK;
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SDAC(void *pBuff)
{
   if(oemi_cache.device_mask & OEMNV_SDAC_MASK)
     *(boolean *) pBuff = TRUE;
   else{
     *(boolean *) pBuff = FALSE;
   }

   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SDAC(void *pBuff)
{
   if(*(boolean *) pBuff == TRUE){
     oemi_cache.device_mask |= OEMNV_SDAC_MASK;
   }
   else{
     oemi_cache.device_mask &= ~OEMNV_SDAC_MASK;
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_EXT_PWR(void *pBuff)
{
   if(oemi_cache.device_mask & OEMNV_EXT_PWR_MASK)
     *(boolean *) pBuff = TRUE;
   else{
     *(boolean *) pBuff = FALSE;
   }

   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_EXT_PWR(void *pBuff)
{
   if(*(boolean *) pBuff == TRUE){
     oemi_cache.device_mask |= OEMNV_EXT_PWR_MASK;
   }
   else{
     oemi_cache.device_mask &= ~OEMNV_EXT_PWR_MASK;
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SPEAKERPHONE(void *pBuff)
{
   if(oemi_cache.device_mask & OEMNV_SPEAKERPHONE_MASK)
     *(boolean *) pBuff = TRUE;
   else{
     *(boolean *) pBuff = FALSE;
   }

   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SPEAKERPHONE(void *pBuff)
{
   if(*(boolean *) pBuff == TRUE){
     oemi_cache.device_mask |= OEMNV_SPEAKERPHONE_MASK;
   }
   else{
     oemi_cache.device_mask &= ~OEMNV_SPEAKERPHONE_MASK;
   }

   return SUCCESS;
}
  
static int OEMPriv_GetItem_CFGI_DEVICE(void *pBuff)
{
   *(uint32 *) pBuff = oemi_cache.device_mask;
   return SUCCESS;
}
//摇一摇换音乐
static int OEMPriv_GetItem_CFGI_SHAKE_MUSIC_CHECK(void *pBuff)
{
    if(oemi_cache.shake_mask & OEMNV_SHAKE_MUSIC_MASK)
      *(boolean *) pBuff = TRUE;
    else{
      *(boolean *) pBuff = FALSE;
    }
    
    return SUCCESS;


}
static int OEMPriv_SetItem_CFGI_SHAKE_MUSIC_CHECK(void *pBuff)
{
    if(*(boolean *) pBuff == TRUE){
      oemi_cache.shake_mask |= OEMNV_SHAKE_MUSIC_MASK;
    }
    else{
      oemi_cache.shake_mask &= ~OEMNV_SHAKE_MUSIC_MASK;
    }
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;

}
//摇一摇换收音机  
static int OEMPriv_GetItem_CFGI_SHAKE_FM_RADIO_CHECK(void *pBuff)
{
    if(oemi_cache.shake_mask & OEMNV_SHAKE_FM_RADIO_MASK)
      *(boolean *) pBuff = TRUE;
    else{
      *(boolean *) pBuff = FALSE;
    }
    
    return SUCCESS;


}
static int OEMPriv_SetItem_CFGI_SHAKE_FM_RADIO_CHECK(void *pBuff)
{
    if(*(boolean *) pBuff == TRUE){
      oemi_cache.shake_mask |= OEMNV_SHAKE_FM_RADIO_MASK;
    }
    else{
      oemi_cache.shake_mask &= ~OEMNV_SHAKE_FM_RADIO_MASK;
    }
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;


}
//摇一摇换图片  
static int OEMPriv_GetItem_CFGI_SHAKE_WALLPAPER_CHECK(void *pBuff)
{
    if(oemi_cache.shake_mask & OEMNV_SHAKE_WALLPAPER_MASK)
      *(boolean *) pBuff = TRUE;
    else{
      *(boolean *) pBuff = FALSE;
    }
    
    return SUCCESS;

}
static int OEMPriv_SetItem_CFGI_SHAKE_WALLPAPER_CHECK(void *pBuff)
{
    if(*(boolean *) pBuff == TRUE){
      oemi_cache.shake_mask |= OEMNV_SHAKE_WALLPAPER_MASK;
    }
    else{
      oemi_cache.shake_mask &= ~OEMNV_SHAKE_WALLPAPER_MASK;
    }
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;


}
//摇一摇换闹钟
static int OEMPriv_GetItem_CFGI_SHAKE_SNOOZE_ALARM_CHECK(void *pBuff)
{
    if(oemi_cache.shake_mask & OEMNV_SHAKE_SNOOZE_ALARM_MASK)
      *(boolean *) pBuff = TRUE;
    else{
      *(boolean *) pBuff = FALSE;
    }
    
    return SUCCESS;


}
static int OEMPriv_SetItem_CFGI_SHAKE_SNOOZE_ALARM_CHECK(void *pBuff)
{
    if(*(boolean *) pBuff == TRUE){
      oemi_cache.shake_mask |= OEMNV_SHAKE_SNOOZE_ALARM_MASK;
    }
    else{
      oemi_cache.shake_mask &= ~OEMNV_SHAKE_SNOOZE_ALARM_MASK;
    }
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;


}

//摇一摇换视频
static int OEMPriv_GetItem_CFGI_SHAKE_VIDEO_CHECK (void *pBuff)
{
    if(oemi_cache.shake_mask & OEMNV_SHAKE_VIDEO_MASK)
      *(boolean *) pBuff = TRUE;
    else{
      *(boolean *) pBuff = FALSE;
    }
    
    return SUCCESS;


}
static int OEMPriv_SetItem_CFGI_SHAKE_VIDEO_CHECK(void *pBuff)
{
   if(*(boolean *) pBuff == TRUE){
     oemi_cache.shake_mask |= OEMNV_SHAKE_VIDEO_MASK;
   }
   else{
     oemi_cache.shake_mask &= ~OEMNV_SHAKE_VIDEO_MASK;
   }
   OEMPriv_WriteOEMConfigList(); 
   return SUCCESS; 

}

//计步器
static int OEMPriv_GetItem_CFGI_PEDOMETER_CHECK (void *pBuff)
{
    if(oemi_cache.shake_mask & OEMNV_PEDOMETER_MASK)
      *(boolean *) pBuff = TRUE;
    else{
      *(boolean *) pBuff = FALSE;
    }
    
    return SUCCESS;


}
static int OEMPriv_SetItem_CFGI_PEDOMETER_CHECK(void *pBuff)
{
   if(*(boolean *) pBuff == TRUE){
     oemi_cache.shake_mask |= OEMNV_PEDOMETER_MASK;
   }
   else{
     oemi_cache.shake_mask &= ~OEMNV_PEDOMETER_MASK;
   }
   OEMPriv_WriteOEMConfigList();
   return SUCCESS; 
}

static int OEMPriv_GetItem_CFGI_GSENSOR(void *pBuff)
{
   *(uint32 *) pBuff = oemi_cache.shake_mask;
   return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_SMS_SERVICE_OPTION(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_service_option;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_SERVICE_OPTION(void *pBuff)
{
   if (oemi_cache.sms_service_option != *(byte *)pBuff) {
      oemi_cache.sms_service_option = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_TIMESTAMP(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if (NV_DONE_S != OEMNV_Get(NV_SMS_UTC_I, &nvi)) {
      return EFAILED;
   }

   if (nvi.sms_utc) {
      *(byte *) pBuff = OEMNV_SMS_TIMESTAMP_ADJUST;
   } else {
      *(byte *) pBuff = OEMNV_SMS_TIMESTAMP_ASRECEIVED;
   }
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_TIMESTAMP(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if (OEMNV_SMS_TIMESTAMP_ADJUST == *(byte *) pBuff) {
      nvi.sms_utc = 1;
   } else {
      nvi.sms_utc = 0;
   }

   if (NV_DONE_S != OEMNV_Put(NV_SMS_UTC_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_IS41_WORKAROUND(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_is41_workaround;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_IS41_WORKAROUND(void *pBuff)
{
   if (oemi_cache.sms_is41_workaround != *(byte *)pBuff) {
      oemi_cache.sms_is41_workaround = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_MO_ENCODING(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_mo_encoding;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_MO_ENCODING(void *pBuff)
{
   if (oemi_cache.sms_mo_encoding != *(byte *)pBuff) {
      oemi_cache.sms_mo_encoding = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_MODE(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_mode;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_MODE(void *pBuff)
{
   if (oemi_cache.sms_mode != *(byte *)pBuff) {
      oemi_cache.sms_mode = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_APP(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_app;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_APP(void *pBuff)
{
   if (oemi_cache.sms_app != *(byte *)pBuff) {
      oemi_cache.sms_app = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_CB_AUTO_DELETE(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_cb_auto_delete;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_CB_AUTO_DELETE(void *pBuff)
{
   if (oemi_cache.sms_cb_auto_delete != *(byte *)pBuff) {
      oemi_cache.sms_cb_auto_delete = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_CB_SILENT_ALERT(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_cb_silent_alert;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_CB_SILENT_ALERT(void *pBuff)
{
   if (oemi_cache.sms_cb_silent_alert != *(byte *)pBuff) {
      oemi_cache.sms_cb_silent_alert = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_CB_LANG_FILTER(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_cb_lang_filter;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_CB_LANG_FILTER(void *pBuff)
{
   if (oemi_cache.sms_cb_lang_filter != *(byte *)pBuff) {
      oemi_cache.sms_cb_lang_filter = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_ALERT_DISPLAY(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_alert_display;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_ALERT_DISPLAY(void *pBuff)
{
   if (oemi_cache.sms_alert_display != *(byte *)pBuff) {
      oemi_cache.sms_alert_display = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_ALERT_SOUND(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_alert_sound;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_ALERT_SOUND(void *pBuff)
{
   if (oemi_cache.sms_alert_sound != *(byte *)pBuff) {
      oemi_cache.sms_alert_sound = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_CDMA_DEFAULT_TEMPLATE(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_cdma_default_template;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_CDMA_DEFAULT_TEMPLATE(void *pBuff)
{
   if (oemi_cache.sms_cdma_default_template != *(byte *)pBuff) {
      oemi_cache.sms_cdma_default_template = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_GW_DEFAULT_TEMPLATE(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_gw_default_template;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_GW_DEFAULT_TEMPLATE(void *pBuff)
{
   if (oemi_cache.sms_gw_default_template != *(byte *)pBuff) {
      oemi_cache.sms_gw_default_template = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_SEGMENT_SETTING(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_segment_setting;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_SEGMENT_SETTING(void *pBuff)
{
   if (oemi_cache.sms_segment_setting != *(byte *)pBuff) {
      oemi_cache.sms_segment_setting = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_SEGMENT_REF_NUM(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_segment_ref_num;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_SEGMENT_REF_NUM(void *pBuff)
{
   if (oemi_cache.sms_segment_ref_num != *(byte *)pBuff) {
      oemi_cache.sms_segment_ref_num = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_RETRY_PERIOD(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.sms_retry_period;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_RETRY_PERIOD(void *pBuff)
{
   if (oemi_cache.sms_retry_period != *(byte *)pBuff) {
      oemi_cache.sms_retry_period = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SECCODE(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if (NV_DONE_S != OEMNV_Get(NV_SEC_CODE_I, &nvi)) {
      return EFAILED;
   }
   STR_TO_WSTR((char *)nvi.sec_code.digits,
               (AECHAR *) pBuff,
               OEMNV_SECCODE_LENGTH * sizeof(AECHAR));
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SECCODE(void *pBuff)
{
#ifndef WIN32
   char code[OEMNV_SECCODE_LENGTH];
   nv_item_type nvi;

   WSTR_TO_STR((AECHAR *) pBuff,
               code,
               sizeof(code));

   if (OEMNV_SECCODE_LENGTH-1 != STRLEN(code)) {
      return EBADPARM;
   }

   MEMCPY((void *) nvi.sec_code.digits, 
          code,
          sizeof(nvi.sec_code.digits));

   if (NV_DONE_S != OEMNV_Put(NV_SEC_CODE_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}

#ifdef FEATURE_ENABLE_OTKSL
static int OEMPriv_GetItem_CFGI_OTKSLCODE(void *pBuff)
{
   nv_item_type nvi;

   if (NV_DONE_S != OEMNV_Get(NV_OTKSL_I, &nvi)) {
      return EFAILED;
   }
   STR_TO_WSTR((char *)nvi.otksl.digits,
               (AECHAR *) pBuff,
               OEMNV_SECCODE_LENGTH * sizeof(AECHAR));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_OTKSLCODE(void *pBuff)
{
   char code[OEMNV_SECCODE_LENGTH];
   nv_item_type nvi;

   WSTR_TO_STR((AECHAR *) pBuff,
               code,
               sizeof(code));

   if (OEMNV_SECCODE_LENGTH-1 != STRLEN(code)) {
      return EBADPARM;
   }

   MEMCPY((void *) nvi.otksl.digits, 
          code,
          sizeof(nvi.otksl.digits));

   if (NV_DONE_S != OEMNV_Put(NV_OTKSL_I, &nvi)) {
      return EFAILED;
   }
   return SUCCESS;
}

#endif /* FEATURE_ENABLE_OTKSL */

static int OEMPriv_GetItem_CFGI_ESN(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if (NV_DONE_S != OEMNV_Get(NV_ESN_I, &nvi)) {
      return EFAILED;
   }
   *(uint32 *) pBuff = nvi.esn.esn;
#endif
   return SUCCESS;
}



static int OEMPriv_GetItem_CFGI_RFCAL_VERSION(void *pBuff)
{
#ifdef FEATURE_RF_ZIF
#ifndef WIN32
   nv_item_type nvi;
   int i;

   if (NV_DONE_S != OEMNV_Get(NV_RF_CAL_VER_I, &nvi)) {
      return EFAILED;
   }

   for (i = 0; i < (OEMNV_RFCALVER_LEN - 1); i++) {
      ((AECHAR *) pBuff)[i] = (AECHAR) (nvi.rf_cal_ver[i] + '0');
   }

   ((AECHAR *) pBuff)[OEMNV_RFCALVER_LEN - 1] = (AECHAR) 0;
#endif
   return SUCCESS;
#else
#error code not present
#endif

}




static int OEMPriv_GetItem_CFGI_RFCAL_DATE(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if (NV_DONE_S != OEMNV_Get(NV_RF_CAL_DATE_I, &nvi)) {
      return EFAILED;
   }

#ifdef FEATURE_RF_ZIF
   *(dword *) pBuff = nvi.rf_cal_date;
   return SUCCESS;
#else
#error code not present
#endif
#else
	return SUCCESS;
#endif
}



static int OEMPriv_GetItem_CFGI_SLOTINDEX(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if (NV_DONE_S != OEMNV_Get(NV_SLOT_CYCLE_INDEX_I, &nvi)) {
      return EFAILED;
   }
   *(uint8 *) pBuff = nvi.slot_cycle_index;
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SLOTINDEX(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.slot_cycle_index = *(uint8 *) pBuff;

   if (NV_DONE_S != OEMNV_Put(NV_SLOT_CYCLE_INDEX_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_HOME_SIDNID_LIST(void *pBuff)
{
#ifndef WIN32
   nv_item_type             nvi;
   int                      j;
   AEEConfigSIDNIDPairType *p;

   nvi.home_sid_nid.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_HOME_SID_NID_I, &nvi)) {
      return EFAILED;
   }

   MEMSET(pBuff, 
          0, 
          sizeof(AEEConfigSIDNIDPairType) * OEMNV_HOME_SIDNID_ARRSIZE);


   for (j = 0, p = (AEEConfigSIDNIDPairType *) pBuff;
        j < NV_MAX_HOME_SID_NID; 
        j++, p++) {
      p->sid = nvi.home_sid_nid.pair[j].sid;
      p->nid = nvi.home_sid_nid.pair[j].nid;
   }
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_HOME_SIDNID_LIST(void *pBuff)
{
#ifndef WIN32
   nv_item_type             nvi;
   int                      j;
   AEEConfigSIDNIDPairType *p;

   nvi.home_sid_nid.nam = (byte) CM_NAM_1;

   for (j = 0, p = (AEEConfigSIDNIDPairType *) pBuff;
        j < NV_MAX_HOME_SID_NID; 
        j++, p++) {
      nvi.home_sid_nid.pair[j].sid = p->sid;
      nvi.home_sid_nid.pair[j].nid = p->nid;
   }

   if (NV_DONE_S != OEMNV_Put(NV_HOME_SID_NID_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_LOCK_SIDNID_LIST(void *pBuff)
{
#ifndef WIN32
   nv_item_type             nvi;
   int                      j;
   AEEConfigSIDNIDPairType *p;

   nvi.sid_nid_lock.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_SID_NID_LOCK_I, &nvi)) {
      return EFAILED;
   }

   MEMSET(pBuff, 
          0, 
          sizeof(AEEConfigSIDNIDPairType) * OEMNV_LOCK_SIDNID_ARRSIZE);

   for (j = 0, p = (AEEConfigSIDNIDPairType *) pBuff;
        j < NV_MAX_SID_NID_LOCK; 
        j++, p++) {
      p->sid = nvi.sid_nid_lock.pair[j].sid;
      p->nid = nvi.sid_nid_lock.pair[j].nid;
   }
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_LOCK_SIDNID_LIST(void *pBuff)
{
#ifndef WIN32
   nv_item_type             nvi;
   int                      j;
   AEEConfigSIDNIDPairType *p;

   nvi.sid_nid_lock.nam = (byte) CM_NAM_1;

   for (j = 0, p = (AEEConfigSIDNIDPairType *) pBuff;
        j < NV_MAX_SID_NID_LOCK; 
        j++, p++) {
      nvi.sid_nid_lock.pair[j].sid = p->sid;
      nvi.sid_nid_lock.pair[j].nid = p->nid;
   }

   if (NV_DONE_S != OEMNV_Put(NV_SID_NID_LOCK_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_IMSI_MCC(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.imsi_mcc.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_IMSI_MCC_I, &nvi)) {
      return EFAILED;
   }

   *(uint16 *) pBuff = OEMPriv_MCC_TO_DEC(nvi.imsi_mcc.imsi_mcc);
#endif
   return SUCCESS;
}


static int OEMPriv_SetItem_CFGI_IMSI_MCC(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;
   uint16       enc_mcc,
                user_mcc,
                digit;

   ERR("TODO", 0, 0, 0);

   user_mcc = *(uint16 *) pBuff;

   /* Encode MCC as per IS-95A 6.3.1.3 */
   digit = (user_mcc / 100) % 10;
   if (0 == digit) {
      digit = 10;
   }
   enc_mcc = digit;

   digit = (user_mcc / 10) % 10;
   if (0 == digit) {
      digit = 10;
   }
   enc_mcc = (uint16) (enc_mcc * 10) + digit;

   digit = user_mcc % 10;
   if (0 == digit) {
      digit = 10;
   }
   enc_mcc = (uint16) (enc_mcc * 10) + digit;

   enc_mcc -= 111;

   nvi.imsi_mcc.nam = (byte) CM_NAM_1;
   nvi.imsi_mcc.imsi_mcc = enc_mcc;
   if (NV_DONE_S != OEMNV_Put(NV_IMSI_MCC_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_IMSI_11_12(void *pBuff)
{
#ifndef WIN32
   nv_item_type      nvi;
   uint16            mnc;
   uint16            digit;

   nvi.imsi_11_12.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_IMSI_11_12_I, &nvi)) {
      return EFAILED;
   }

   /* The saved MNC value is encoded as IS-95A */
   nvi.imsi_11_12.imsi_11_12 %= 100;

   digit = (nvi.imsi_11_12.imsi_11_12 / 10) + 1;
   mnc   = 10 * (digit % 10);

   digit = (nvi.imsi_11_12.imsi_11_12 % 10) + 1;
   mnc  += (digit % 10);

   *(uint16 *) pBuff = mnc;
#endif
   return SUCCESS;
}


static int OEMPriv_SetItem_CFGI_IMSI_11_12(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;
   uint16       user,
                enc,
                digit;

   /* Encode IMSI_11_12 as per IS-95A 6.3.1.2 */
   user = *(uint16 *) pBuff;

   digit = (user / 10) % 10;
   if (0 == digit) {
      digit = 10;
   }
   enc = digit;

   digit = user % 10;
   if (0 == digit) {
      digit = 10;
   }
   enc = (uint16) (enc * 10) + digit;

   enc -= 11;
   enc &= 0x00FF;

   nvi.imsi_11_12.nam = (byte) CM_NAM_1;
   nvi.imsi_11_12.imsi_11_12 = (uint8) enc;

   if (NV_DONE_S != OEMNV_Put(NV_IMSI_11_12_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_IMSI_S(void *pBuff)
{
#ifndef WIN32
   char         imsi_s[11];
   nv_item_type nvi;
   uint16       min2;

   nvi.min2.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_MIN2_I, &nvi)) {
      return EFAILED;
   }

   min2 = OEMPriv_MIN2_TO_DEC(nvi.min2.min2[CDMAMIN]);

   imsi_s[0] = '0' + (char) (min2 / 100);
   imsi_s[1] = '0' + (char) ( (min2 / 10) % 10);
   imsi_s[2] = '0' + (char) (min2 % 10);

   nvi.min1.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_MIN1_I, &nvi)) {
      return EFAILED;
   }

   OEMPriv_MIN1_TO_STR(nvi.min1.min1[CDMAMIN], &imsi_s[3]);

   STR_TO_WSTR(imsi_s, 
               (AECHAR *) pBuff, 
               sizeof(AECHAR) * OEMNV_IMSI_S_LENGTH);
#endif
   return SUCCESS;
}


static int OEMPriv_SetItem_CFGI_IMSI_S(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;
   AECHAR      *imsi;
   uint16       min2;
   uint32       min1;
   uint16       digit;
   int          i;

   // Validate the input string
   imsi = (AECHAR *) pBuff;
   if (WSTRLEN(imsi) != 10) {
      return EFAILED;
   }

   while (*imsi) {
      if ( (*imsi < (AECHAR) '0') ||
           (*imsi > (AECHAR) '9') ) {
         return EFAILED;
      }
      imsi++;
   }

   //
   // Encode the first three digits (IS-95A 6.3.1.1)
   //
   imsi = (AECHAR *) pBuff;
   min2 = 0;
   for (i = 0; i < 3; i++) {
      digit = *imsi - (AECHAR) '0';
      if (0 == digit) {
         digit = 10;
      }
      imsi++;

      min2 = (uint16) (min2 * 10) + digit;
   }

   min2 -= 111;

   //
   // Encode the last seven digits (IS-95A 6.3.1.1)
   //

   // Encode the second three digits into the ten most
   // significant bits (of the 24-bit number)...
   min1 = 0;
   for (i = 0; i < 3; i++) {
      digit = *imsi - (AECHAR) '0';
      if (0 == digit) {
         digit = 10;
      }
      imsi++;

      min1 = (uint32) (min1 * 10) + digit;
   }

   min1 -= 111;

   min1 <<= 14;
   min1 &= 0x00FFC000;

   // The fourth last digit is mapping as BCD into four bits
   digit = *imsi - (AECHAR) '0';
   if (0 == digit) {
      digit = 10;
   }
   imsi++;

   min1 = min1 | (0x00003C00 & (digit << 10));

   // Encode the last three digits into the ten least significant bits
   {
      uint32 tmp = 0;

      for (i = 0; i < 3; i++) {
         digit = *imsi - (AECHAR) '0';
         if (0 == digit) {
            digit = 10;
         }
         imsi++;

         tmp = (uint32) (tmp * 10) + digit;
      }

      tmp -= 111;
      tmp &= 0x000003FF;

      min1 |= tmp;
   }

   //
   // Write the encoded values out to NV...

   #ifdef FEATURE_NPT
#error code not present
   #endif /* FEATURE_NPT */
   
   nvi.min2.nam = (byte) CM_NAM_1;
   nvi.min2.min2[CDMAMIN] = min2;
   if (NV_DONE_S != OEMNV_Put(NV_MIN2_I, &nvi)) {
      return EFAILED;
   }

   nvi.min1.nam = (byte) CM_NAM_1;
   nvi.min1.min1[CDMAMIN] = min1;

   if (NV_DONE_S != OEMNV_Put(NV_MIN1_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}

#ifdef FATRUE_LOCK_IMSI_MCCMNC
static int OEMPriv_GetItem_CFGI_IMSI_SETMCC(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.List, sizeof(SetImsi) * OEMNV_MAX_SERVICEPROVIDER_NUMBER);
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_IMSI_SETMCC(void *pBuff)
{
    memcpy((void *)oemi_cache.List,(void *)pBuff,sizeof(SetImsi) * OEMNV_MAX_SERVICEPROVIDER_NUMBER);
   OEMPriv_WriteOEMConfigList();  
   return SUCCESS;
}
#endif
static int OEMPriv_GetItem_CFGI_PRL_ENABLED(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.prl_enabled.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_PRL_ENABLED_I, &nvi)) {
      return EFAILED;
   }

   *(boolean *) pBuff = nvi.prl_enabled.enabled;
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PRL_ENABLED(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.prl_enabled.nam = (byte) CM_NAM_1;
   nvi.prl_enabled.enabled = *(boolean *) pBuff;

   if (NV_DONE_S != OEMNV_Put(NV_PRL_ENABLED_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_AOC(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.accolc.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_ACCOLC_I, &nvi)) {
      return EFAILED;
   }

   *(uint8 *) pBuff = nvi.accolc.ACCOLCpClass[CDMAMIN];
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_AOC(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.accolc.nam = (byte) CM_NAM_1;
   nvi.accolc.ACCOLCpClass[CDMAMIN] = *(uint8 *) pBuff;
   if (NV_DONE_S != OEMNV_Put(NV_ACCOLC_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_HOME_SID_REG(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.mob_term_home.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_MOB_TERM_HOME_I, &nvi)) {
      return EFAILED;
   }

   *(boolean *) pBuff = nvi.mob_term_home.enabled[CDMAMIN] ? TRUE:FALSE;
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_HOME_SID_REG(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.mob_term_home.nam = (byte) CM_NAM_1; 
   nvi.mob_term_home.enabled[CDMAMIN] = *(boolean *) pBuff;

   if (NV_DONE_S != OEMNV_Put(NV_MOB_TERM_HOME_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_FORN_SID_REG(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.mob_term_for_sid.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_MOB_TERM_FOR_SID_I, &nvi)) {
      return EFAILED;
   }

   *(boolean *) pBuff = nvi.mob_term_for_sid.enabled[CDMAMIN]?TRUE:FALSE;
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_FORN_SID_REG(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.mob_term_for_sid.nam = (byte) CM_NAM_1; 
   nvi.mob_term_for_sid.enabled[CDMAMIN] = *(boolean *) pBuff;

   if (NV_DONE_S != OEMNV_Put(NV_MOB_TERM_FOR_SID_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_FORN_NID_REG(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.mob_term_for_nid.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_MOB_TERM_FOR_NID_I, &nvi)) {
      return EFAILED;
   }

   *(boolean *) pBuff = nvi.mob_term_for_nid.enabled[CDMAMIN]?TRUE:FALSE;
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_FORN_NID_REG(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.mob_term_for_nid.nam = (byte) CM_NAM_1;
   nvi.mob_term_for_nid.enabled[CDMAMIN] = *(boolean *) pBuff;

   if (NV_DONE_S != OEMNV_Put(NV_MOB_TERM_FOR_NID_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}

#ifdef FEATURE_ACP
static int OEMPriv_GetItem_CFGI_MODE_PREF(void *pBuff)
{
   nv_item_type nvi;

   nvi.pref_mode.nam  = (byte) CM_NAM_1;

   if (NV_DONE_S != OEMNV_Get(NV_PREF_MODE_I, &nvi)) {
      return EFAILED;
   }

   switch (nvi.pref_mode.mode) {
   case NV_MODE_DIGITAL_ONLY:
      *(byte *) pBuff = OEMNV_MODE_DIGITAL_ONLY;
      break;
   case NV_MODE_ANALOG_ONLY:
      *(byte *) pBuff = OEMNV_MODE_ANALOG_ONLY;
      break;
   case NV_MODE_AUTOMATIC:
   default:
      *(byte *) pBuff = OEMNV_MODE_AUTOMATIC;
      break;
   }
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MODE_PREF(void *pBuff)
{
   nv_item_type nvi;

   nvi.pref_mode.nam  = (byte) CM_NAM_1;
   switch (*(byte *) pBuff) {
   case OEMNV_MODE_DIGITAL_ONLY:
      nvi.pref_mode.mode = NV_MODE_DIGITAL_ONLY;
      break;
   case OEMNV_MODE_ANALOG_ONLY:
      nvi.pref_mode.mode = NV_MODE_ANALOG_ONLY;
      break;
   case OEMNV_MODE_AUTOMATIC:
   default:
      nvi.pref_mode.mode = NV_MODE_AUTOMATIC;
      break;
   }
  
   // Note that CM is not notified of the new mode preference
   // until the phone restarts...
   if (NV_DONE_S != OEMNV_Put(NV_PREF_MODE_I, &nvi)) {
      return EFAILED;
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_AMPS_HOME_SID(void *pBuff)
{
   nv_item_type nvi;

   nvi.analog_home_sid.nam  = (byte) CM_NAM_1;

   if (NV_DONE_S != OEMNV_Get(NV_ANALOG_HOME_SID_I, &nvi)) {
      return EFAILED;
   }

   *(uint16 *) pBuff = nvi.analog_home_sid.sid;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_AMPS_HOME_SID(void *pBuff)
{
   nv_item_type nvi;

   nvi.analog_home_sid.nam = (byte) CM_NAM_1;
   nvi.analog_home_sid.sid = *(uint16 *) pBuff;

   if (NV_DONE_S != OEMNV_Put(NV_ANALOG_HOME_SID_I, &nvi)) {
      return EFAILED;
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_AMPS_FIRSTCHP(void *pBuff)
{
   nv_item_type nvi;

   nvi.analog_firstchp.nam  = (byte) CM_NAM_1;

   if (NV_DONE_S != OEMNV_Get(NV_ANALOG_FIRSTCHP_I, &nvi)) {
      return EFAILED;
   }

   *(uint16 *) pBuff = nvi.analog_firstchp.channel;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_AMPS_FIRSTCHP(void *pBuff)
{
   nv_item_type nvi;

   nvi.analog_firstchp.nam = (byte) CM_NAM_1;
   nvi.analog_firstchp.channel = *(uint16 *) pBuff;

   if (NV_DONE_S != OEMNV_Put(NV_ANALOG_FIRSTCHP_I, &nvi)) {
      return EFAILED;
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_AMPS_REG_TYPE(void *pBuff)
{
   nv_item_type nvi;

   nvi.analog_reg.nam  = (byte) CM_NAM_1;

   if (NV_DONE_S != OEMNV_Get(NV_ANALOG_REG_I, &nvi)) {
      return EFAILED;
   }

   switch (nvi.analog_reg.kind) {
   case NV_WHEREABOUTS_KNOWN:
      *(byte *) pBuff = OEMNV_AMPSREG_WHEREABOUTS_KNOWN;
      break;
   case NV_WHEREABOUTS_UNKNOWN:
      *(byte *) pBuff = OEMNV_AMPSREG_WHEREABOUTS_UNKNOWN;
      break;
   case NV_DISABLED:
   default:
      *(byte *) pBuff = OEMNV_AMPSREG_DISABLED;
      break;
   }
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_AMPS_REG_TYPE(void *pBuff)
{
   nv_item_type nvi;

   nvi.analog_reg.nam = (byte) CM_NAM_1;
   switch (*(byte *) pBuff) {
   case OEMNV_AMPSREG_WHEREABOUTS_KNOWN:
      nvi.analog_reg.kind = NV_WHEREABOUTS_KNOWN; 
      break;
   case OEMNV_AMPSREG_WHEREABOUTS_UNKNOWN:
      nvi.analog_reg.kind = NV_WHEREABOUTS_UNKNOWN; 
      break;
   case OEMNV_AMPSREG_DISABLED:
   default:
      nvi.analog_reg.kind = NV_DISABLED; 
      break;
   }

   if (NV_DONE_S != OEMNV_Put(NV_ANALOG_REG_I, &nvi)) {
      return EFAILED;
   }

   return SUCCESS;
}

#endif /* FEATURE_ACP */

static int OEMPriv_GetItem_CFGI_DATA_QNC_ENABLED(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if (NV_DONE_S != OEMNV_Get(NV_DATA_QNC_ENABLED_I, &nvi)) {
      return EFAILED;
   }

   *(boolean *) pBuff = nvi.data_qnc_enabled;
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_DATA_QNC_ENABLED(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.data_qnc_enabled = *(boolean *)pBuff;

   if (NV_DONE_S != OEMNV_Put(NV_DATA_QNC_ENABLED_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
static int OEMPriv_GetItem_CFGI_RTRE_CONFIGURATION(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if (NV_DONE_S != OEMNV_Get(NV_RTRE_CONFIG_I, &nvi)) {
      // Default to R-UIM only.
      *(nv_rtre_configuration_type *) pBuff = NV_RTRE_CONFIG_RUIM_ONLY;
      return SUCCESS;
   }

   *(nv_rtre_configuration_type *) pBuff = nvi.rtre_config;
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_RTRE_CONFIGURATION(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   nvi.rtre_config = *(nv_rtre_configuration_type *) pBuff;

   if (NV_DONE_S != OEMNV_Put(NV_RTRE_CONFIG_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}
#endif 
#ifdef FEATURE_FDN

static int OEMPriv_GetItem_CFGI_FDN_ENABLED(void *pBuff)
{
   *(boolean *)pBuff = OEM_IsFDNEnabled();
   return SUCCESS;
}


static int OEMPriv_SetItem_CFGI_FDN_ENABLED(void *pBuff)
{
   if (OEM_EnableFDN(*(boolean *) pBuff)) {
      return SUCCESS;
   }

   return EUNSUPPORTED;
}

#endif /* FEATURE_FDN */


static int OEMPriv_GetItem_CFGI_DEBUG_CHANNEL(void *pBuff)
{
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#ifndef WIN32
   db_items_value_type dbi;

   db_get(DB_CHANNEL, &dbi); 
   *(uint16 *)pBuff = dbi.channel;
#endif
   return SUCCESS;
#else
   return EUNSUPPORTED;
#endif
}


static int OEMPriv_GetItem_CFGI_DEBUG_PILOT_PN(void *pBuff)
{
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#ifndef WIN32
   db_items_value_type dbi;

   db_get(DB_PILOT_PN_OFF, &dbi); 
   *(uint16 *)pBuff = dbi.pilot_pn_off;
#endif
   return SUCCESS;
#else
   return EUNSUPPORTED;
#endif
}


static int OEMPriv_GetItem_CFGI_DEBUG_TX_AGC(void *pBuff)
{
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#ifndef WIN32
   *(uint16 *)pBuff = (uint16) rf_get_cdma_tx_agc();
#endif
   return SUCCESS;
#else
   *(byte *)pBuff = (byte) 0;
   return EUNSUPPORTED;
#endif
}

static int OEMPriv_GetItem_CFGI_DEBUG_TX_AGC_IN_DBM(void *pBuff)
{
#if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)) && defined(FEATURE_RF_ZIF) 
   int32 rxagc;
   int32  txadj;
#if defined (T_MSM6500)
//#error code not present
#else
#ifndef WIN32

   rxagc = rf_get_cdma_rx_agc_in_dBm();
   txadj = (int32)srch_get_tx_adj();
#endif
  *(int32 *)pBuff =  -73 - rxagc + txadj;


#endif /* T_MSM6500 */
   return SUCCESS;
#else
   *(byte *)pBuff = (byte) 0;
   return EUNSUPPORTED;
#endif
}


static int OEMPriv_GetItem_CFGI_DEBUG_RX_AGC(void *pBuff)
{
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#ifndef WIN32
   *(signed char *)pBuff = rf_get_cdma_rx_agc();
#endif
   return SUCCESS;
#else
   *(byte *)pBuff = (byte) 0;
   return EUNSUPPORTED;
#endif
}

static int OEMPriv_GetItem_CFGI_DEBUG_RX_AGC_IN_DBM(void *pBuff)
{
#if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)) && defined(FEATURE_RF_ZIF)
  #if defined (T_MSM6500)
//#error code not present
#else
#ifndef WIN32
    byte aa = srch_get_rx_agc();
    *(int32 *)pBuff = ( (-6325 + ((int1)aa * 33) )/10 );
#endif
    //*(int32 *)pBuff = rf_get_cdma_rx_agc_in_dBm();
  #endif /* T_MSM6500 */
   return SUCCESS;
#else
   *(byte *)pBuff = (byte) 0;
   return EUNSUPPORTED;
#endif
}
static int OEMPriv_GetItem_CFGI_DEBUG_ECIO(void *pBuff)
{
#if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900))&& defined(FEATURE_RF_ZIF)  
#ifndef WIN32
 db_items_value_type dbi;

  db_get(DB_CDMA_ECIO, &dbi); 
    *(byte*)pBuff =  dbi.cdma_ecio;
#ifdef FEATURE_OEM_DEBUG
  MSG_ERROR("cdma_ecio = %d,%d", *(byte*)pBuff,dbi.cdma_ecio,0);
#endif
#endif
   return SUCCESS;
#else
   *(byte *)pBuff = (byte) 0;
   return EUNSUPPORTED;
#endif
}
static int OEMPriv_GetItem_CFGI_DEBUG_SID(void *pBuff)
{
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#ifndef WIN32
   db_items_value_type dbi;

   db_get(DB_SID, &dbi); 
   *(uint16 *)pBuff = dbi.sid;
#endif
   return SUCCESS;
#else
   return EUNSUPPORTED;
#endif
}


static int OEMPriv_GetItem_CFGI_DEBUG_NID(void *pBuff)
{
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#ifndef WIN32
   db_items_value_type dbi;

   db_get(DB_NID, &dbi); 
   *(uint16 *)pBuff = dbi.nid;
#endif
   return SUCCESS;
#else
   return EUNSUPPORTED;
#endif
}


static int OEMPriv_GetItem_CFGI_DEBUG_TX_ADJUST(void *pBuff)
{
#ifdef FEATURE_RF_ZIF
#ifndef WIN32
   *(signed char *)pBuff = rf_get_tx_gain_adj();
#endif
   return SUCCESS;
#else
#error code not present
#endif
}

static int OEMPriv_GetItem_CFGI_DEBUG_HDET(void *pBuff)
{
#ifdef FEATURE_RF_ZIF
#ifndef WIN32
   *(byte *)pBuff = (byte) rf_hdet_data; /* adc_read(ADC_HDET_CELL); */
#endif  
   return SUCCESS;
#else
#error code not present
#endif
}


static int OEMPriv_GetItem_CFGI_DEBUG_VBATT(void *pBuff)
{
#ifdef FEATURE_RF_ZIF
#ifndef WIN32
   *(byte *)pBuff = (byte) adc_read(ADC_VBATT);
#endif
   return SUCCESS;
#else
#error code not present
#endif
}


static int OEMPriv_GetItem_CFGI_DEBUG_ERRLOG(void *pBuff)
{
#ifndef WIN32
   OEMErrLogType *p_errlog = (OEMErrLogType *)pBuff;
   nv_err_log_type nv_log;
   int i;

   MEMSET ((void *) &nv_log, 0, sizeof (nv_err_log_type));

   if (p_errlog->err_num < NV_MAX_ERR_LOG) {
      (void) err_get_log(p_errlog->err_num, &nv_log);
      if (nv_log.err_count) {
         p_errlog->err_count = nv_log.err_count;
         p_errlog->line_num = nv_log.line_num;
         p_errlog->fatal = nv_log.fatal;
         for (i = 0; i < NV_ERR_FILE_NAME_LEN; i++) {
            p_errlog->file_name[i] = nv_log.file_name[i];
         }
         return SUCCESS;
      }
   }
   return EFAILED;
#else
	return SUCCESS;
#endif
}


static int OEMPriv_GetItem_CFGI_DEBUG_LNA_STATE(void *pBuff)
{
#if defined(FEATURE_RF_ZIF) && defined(T_MSM6500)
//#error code not present
	return SUCCESS;
#elif defined(FEATURE_RF_ZIF)
#ifndef WIN32
   *(byte *)pBuff = (byte) rf_get_lna_gain_state();
#endif
    return SUCCESS;
#else
   *(byte *)pBuff = 0;
   return EUNSUPPORTED;

#endif
}



static int OEMPriv_GetItem_CFGI_DEBUG_PA_STATE(void *pBuff)
{
#ifdef FEATURE_RF_ZIF
#ifndef WIN32
   *(byte *)pBuff = (byte) rf_get_pa_state();
#endif
   return SUCCESS;
#else
#error code not present
#endif
}

static int OEMPriv_GetItem_CFGI_DEBUG_RATCHET_STATE(void *pBuff)
{
#ifdef FEATURE_RF_ZIF
#ifndef WIN32
   *(byte *)pBuff = (byte) rf_get_ratchet_state();
#endif
   return SUCCESS;
#else
#error code not present
#endif
}

static int OEMPriv_GetItem_CFGI_CDMA_DYNAMIC_RANGE(void *pBuff)
{
#ifdef FEATURE_RF_ZIF
#ifndef WIN32
   nv_item_type nvi;

   if (NV_DONE_S != OEMNV_Get(NV_CDMA_DYNAMIC_RANGE_I, &nvi)) {
      return EFAILED;
   }
   *(short *)pBuff = nvi.cdma_dynamic_range;
#endif
   return SUCCESS;
#else
#error code not present
#endif
}



static int OEMPriv_GetItem_CFGI_CDMA_MIN_RX_RSSI(void *pBuff)
{
#ifdef FEATURE_RF_ZIF
#ifndef WIN32
   nv_item_type nvi;

   if (NV_DONE_S != OEMNV_Get(NV_CDMA_MIN_RX_RSSI_I, &nvi)) {
      return EFAILED;
   }
   
   *(short *)pBuff = nvi.cdma_min_rx_rssi;
#endif
   return SUCCESS;
#else
#error code not present
#endif
}



#ifdef FEATURE_FACTORY_TESTMODE
static int OEMPriv_GetItem_CFGI_FACTORY_TEST_MODE(void *pBuff)
{
   *(boolean *) pBuff = OEM_IsFactoryTestMode();
   return SUCCESS;
}

#endif /*FEATURE_FACTORY_TESTMODE*/

static int OEMPriv_SetItem_ReadOnly(void *pBuff)
{
   // This item is read-only
   return EBADPARM;
}

#ifdef T_MSM6500
//#error code not present
#endif 


static int OEMPriv_GetItem_CFGI_DEBUG_THERM(void *pBuff)
{
#ifdef FEATURE_RF_ZIF
#ifndef WIN32
   *(word *)pBuff = (byte) therm_read();
#endif
   *(word *)pBuff = (*(word *)pBuff) << 8;
#if defined(RF_HAS_HDET_BYPASS) 
#ifndef WIN32
   *(word *)pBuff |= (((byte) adc_read(ADC_XO_THERM)) & 0xFF);
#endif
#else   
   *(word *)pBuff |= (((byte) adc_read(ADC_THERM)) & 0xFF);
#endif /* FEATURE_CS08 */
   return SUCCESS;
#else
#error code not present
#endif
}

static int OEMPriv_GetItem_CFGI_DEBUG_BAND(void *pBuff)
{
#ifdef FEATURE_RF_ZIF
#ifndef WIN32
  db_items_value_type db_value;

  db_get(DB_RF_MODE, &db_value);

  switch (db_value.rf_mode)
  {
  case DB_RF_ANALOG:
    *(byte *)pBuff = CFGI_DEBUG_BAND_AMPS;
    break;
  case DB_RF_CDMA:
    *(byte *)pBuff = CFGI_DEBUG_BAND_CDMA;
    break;
  case DB_RF_PCN:
    *(byte *)pBuff = CFGI_DEBUG_BAND_PCS;
    break;
  case DB_RF_SLEEP:
    *(byte *)pBuff = CFGI_DEBUG_BAND_SLEEP;
    break;
  case DB_RF_GPS:
    *(byte *)pBuff = CFGI_DEBUG_BAND_GPS;
    break;
  default:
    *(byte *)pBuff = CFGI_DEBUG_BAND_LAST;
    break;
  }
#endif
  return SUCCESS;
#else
#error code not present
#endif
}

static int OEMPriv_SetItem_CFGI_LCD(void *pBuff)
{
  cfgi_lcd = *((boolean *)pBuff);
  return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_LCD(void *pBuff)
{
  *((boolean *)pBuff)= cfgi_lcd;
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_LCD_TIMER(void *pBuff)
{
  *((uint32 *)pBuff)= nvi_cache.display_duration;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_LCD_TIMER(void *pBuff)
{
#ifndef WIN32
  nv_item_type nvi;

  if (nvi_cache.display_duration != *((uint32 *)pBuff)) {
      nvi_cache.display_duration = *((uint32 *)pBuff);
      nvi.display_duration = nvi_cache.display_duration;
     (void) OEMNV_Put( NV_DISPLAY_DURATION_I, &nvi);
  }
#endif
  return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_PHONE_PASSWORD(void *pBuff)
{
   *(uint16 *) pBuff = oemi_cache.phone_password;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PHONE_PASSWORD(void *pBuff)
{
   if (oemi_cache.phone_password != *(uint16 *)pBuff) {
      oemi_cache.phone_password = *(uint16 *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_PHONE_PASSWORD_CHECK(void *pBuff)
{
   *(byte *) pBuff = nvi_cache.lock;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PHONE_PASSWORD_CHECK(void *pBuff)
{
#ifndef WIN32
    nv_item_type nvi;

   if (nvi_cache.lock != *((byte *)pBuff)) {
      nvi_cache.lock = *((byte *)pBuff);
      nvi.lock = *((byte *)pBuff);
      (void) OEMNV_Put( NV_LOCK_I, &nvi);
   }
#endif
   return SUCCESS;
   //if (oemi_cache.phone_password_check != *(byte *)pBuff) {
   //   oemi_cache.phone_password_check = *(byte *)pBuff;
   //   OEMPriv_WriteOEMConfigList();
   //}
   //return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_RESTRICT_OUTGOING(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.restrict_outgoing;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_RESTRICT_OUTGOING(void *pBuff)
{
   if (oemi_cache.restrict_outgoing != *(byte *)pBuff) {
      oemi_cache.restrict_outgoing = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_RESTRICT_INCOMING(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.restrict_incoming;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_RESTRICT_INCOMING(void *pBuff)
{
//   *(byte *) pBuff = oemi_cache.restrict_incoming;
   if (oemi_cache.restrict_incoming != *(byte *)pBuff) {
      oemi_cache.restrict_incoming = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}
//情景模式
//情景模式类型
static int OEMPriv_GetItem_CFGI_PROFILE_CUR_NUMBER(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.p_profile_cur_number;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_CUR_NUMBER(void *pBuff)
{
   if (oemi_cache.p_profile_cur_number != *(byte *)pBuff)
   {
      oemi_cache.p_profile_cur_number = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

//来电提示方式
static int OEMPriv_GetItem_CFGI_PROFILE_ALERT_TYPE(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_alert_type,sizeof(oemi_cache.p_alert_type));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_ALERT_TYPE(void *pBuff)
{
   memcpy((void *)oemi_cache.p_alert_type,(void *)pBuff,sizeof(oemi_cache.p_alert_type));
   OEMPriv_WriteOEMConfigList();  
   return SUCCESS;
}

//短信提示方式
static int OEMPriv_GetItem_CFGI_PROFILE_SMS_RINGER(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_bt_sms_ringer,sizeof(oemi_cache.p_bt_sms_ringer));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_SMS_RINGER(void *pBuff)
{
   memcpy((void *)oemi_cache.p_bt_sms_ringer,(void *)pBuff,sizeof(oemi_cache.p_bt_sms_ringer));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}

//来电铃声
static int OEMPriv_GetItem_CFGI_PROFILE_CALL_RINGER(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_call_ringer,sizeof(oemi_cache.p_call_ringer));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_CALL_RINGER(void *pBuff)
{
	memcpy((void *)oemi_cache.p_call_ringer,(void *)pBuff,sizeof(oemi_cache.p_call_ringer));
	OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}

//闹钟铃声
static int OEMPriv_GetItem_CFGI_PROFILE_ALARM_RINGER(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_alarm_ringer,sizeof(oemi_cache.p_alarm_ringer));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_ALARM_RINGER(void *pBuff)
{
	memcpy((void *)oemi_cache.p_alarm_ringer,(void *)pBuff,sizeof(oemi_cache.p_alarm_ringer));
	OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}

//短信铃声
static int OEMPriv_GetItem_CFGI_PROFILE_SMS_RINGER_ID(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_sms_ringer_id,sizeof(oemi_cache.p_sms_ringer_id));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_SMS_RINGER_ID(void *pBuff)
{
   memcpy((void *)oemi_cache.p_sms_ringer_id,(void *)pBuff,sizeof(oemi_cache.p_sms_ringer_id));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}
   
//开机音乐
static int OEMPriv_GetItem_CFGI_PROFILE_STARTUP_MUSIC(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_start_music,sizeof(oemi_cache.p_start_music));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_STARTUP_MUSIC(void *pBuff)
{
   memcpy((void *)oemi_cache.p_start_music,(void *)pBuff,sizeof(oemi_cache.p_start_music));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}

//关机音乐
static int OEMPriv_GetItem_CFGI_PROFILE_SHUTDOWN_MUSIC(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_shutdown_music,sizeof(oemi_cache.p_shutdown_music));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_SHUTDOWN_MUSIC(void *pBuff)
{
   memcpy((void *)oemi_cache.p_shutdown_music,(void *)pBuff,sizeof(oemi_cache.p_shutdown_music));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}

//开关机铃声提示
static int OEMPriv_GetItem_CFGI_PROFILE_POWERONOFF_ALERT(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_poweronoff_alert,sizeof(oemi_cache.p_poweronoff_alert));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_POWERONOFF_ALERT(void *pBuff)
{
   memcpy((void *)oemi_cache.p_poweronoff_alert,(void *)pBuff,sizeof(oemi_cache.p_poweronoff_alert));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}

//未接来电提醒
static int OEMPriv_GetItem_CFGI_PROFILE_MISSED_CALL_ALERT(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_missed_call_alert,sizeof(oemi_cache.p_missed_call_alert));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_MISSED_CALL_ALERT(void *pBuff)
{
   memcpy((void *)oemi_cache.p_missed_call_alert,(void *)pBuff,sizeof(oemi_cache.p_missed_call_alert));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}

//多彩按键音
static int OEMPriv_GetItem_CFGI_PROFILE_KEYSND_TYPE(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_keysnd_type,sizeof(oemi_cache.p_keysnd_type));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_KEYSND_TYPE(void *pBuff)
{
   memcpy((void *)oemi_cache.p_keysnd_type,(void *)pBuff,sizeof(oemi_cache.p_keysnd_type));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}

//按键音长度
static int OEMPriv_GetItem_CFGI_PROFILE_KEYTONE_LENGTH(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_keytone_length,sizeof(oemi_cache.p_keytone_length));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_KEYTONE_LENGTH(void *pBuff)
{
   memcpy((void *)oemi_cache.p_keytone_length,(void *)pBuff,sizeof(oemi_cache.p_keytone_length));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}
 
//铃声音量
static int OEMPriv_GetItem_CFGI_PROFILE_RINGER_VOL(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_ringer_level,sizeof(oemi_cache.p_ringer_level));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_RINGER_VOL(void *pBuff)
{
   memcpy((void *)oemi_cache.p_ringer_level,(void *)pBuff,sizeof(oemi_cache.p_ringer_level));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}

//耳机音量
static int OEMPriv_GetItem_CFGI_PROFILE_EAR_VOL(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_handset_ear_level,sizeof(oemi_cache.p_handset_ear_level));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_EAR_VOL(void *pBuff)
{
   memcpy((void *)oemi_cache.p_handset_ear_level,(void *)pBuff,sizeof(oemi_cache.p_handset_ear_level));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
}

//键盘音量
static int OEMPriv_GetItem_CFGI_PROFILE_BEEP_VOL(void *pBuff)
{
   memcpy((void *)pBuff,(void *)oemi_cache.p_beep_level,sizeof(oemi_cache.p_beep_level));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PROFILE_BEEP_VOL(void *pBuff)
{
   memcpy((void *)oemi_cache.p_beep_level,(void *)pBuff,sizeof(oemi_cache.p_beep_level));
   OEMPriv_WriteOEMConfigList();
   return SUCCESS;
} 
//短信铃声(0-表示无),星期闹钟,自动关机
static int OEMPriv_GetItem_CFGI_SMS_RINGER(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.bt_sms_ringer;
   return SUCCESS;
}

//闹钟铃声
static int OEMPriv_GetItem_CFGI_ALARM_RINGER(void *pBuff)
{
   *(uint16 *) pBuff = oemi_cache.alarm_ringer;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_ALARM_RINGER(void *pBuff)
{
   if (oemi_cache.alarm_ringer != *(uint16 *)pBuff)
   {
      oemi_cache.alarm_ringer = *(uint16 *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}
//短信铃声
static int OEMPriv_GetItem_CFGI_SMS_RINGER_ID(void *pBuff)
{
   *(uint16 *) pBuff = oemi_cache.sms_ringer_id;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_RINGER_ID(void *pBuff)
{
   if (oemi_cache.sms_ringer_id != *(uint16 *)pBuff)
   {
      oemi_cache.sms_ringer_id = *(uint16 *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_SMS_RINGER(void *pBuff)
{
   if (oemi_cache.bt_sms_ringer != *(byte *)pBuff) {
      oemi_cache.bt_sms_ringer = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}  
static int OEMPriv_GetItem_CFGI_HEADSET_PRESENT(void *pBuff)
{
   *(boolean *) pBuff = OEM_IsHeadsetPresent();
   return SUCCESS;
}
//多彩按键音
static int OEMPriv_GetItem_CFGI_KEYSND_TYPE(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.keysnd_type;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_KEYSND_TYPE(void *pBuff)
{
   if (oemi_cache.keysnd_type != *(byte *)pBuff)
   {
      oemi_cache.keysnd_type = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_VOICE_PRIVACY(void *pBuff)
{
   //nv_item_type nvi;

   //if (NV_DONE_S != OEMNV_Get(NV_VOICE_PRIV_I, &nvi)) {
   //   return EFAILED;
   //}
   *(byte *) pBuff = OEM_PrivacyPrefToAEECM(nvi_cache.voice_priv);
   return SUCCESS;
   
   //nv_item_type nvi;

   //if (NV_DONE_S != OEMNV_Get(NV_VOICE_PRIV_I, &nvi)) {
   //   return EFAILED;
   //}

   //*(byte *) pBuff = nvi.voice_priv;
   //return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_VOICE_PRIVACY(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;
   byte voice_priv = OEM_PrivacyPrefToDMSS(*((byte *)pBuff));
   if (nvi_cache.voice_priv != voice_priv) 
   {
      nvi_cache.voice_priv = voice_priv;
      nvi.voice_priv = voice_priv;
      (void) OEMNV_Put( NV_VOICE_PRIV_I, &nvi);
   }
#endif
   return SUCCESS;
   
   //nv_item_type nvi;

   //nvi.voice_priv = *(byte *) pBuff;

   //if (NV_DONE_S != OEMNV_Put(NV_VOICE_PRIV_I, &nvi)) {
   //   return EFAILED;
   //}
#if 0
/*
 * TODO:  For FEATURE_CM_MULTIMODE, we must have the call id for this to
 *        work.  That being said, the only "right" way to get a call id
 *        is to add this function onto IPhone which has the call id.
 *        For now though, we will ASS/U/ME that the only one to allocate
 *        a call id is IPhone which means it will have id 0.
 */
   if (*(byte *)pBuff == 0) {
#ifdef FEATURE_CM_MULTIMODE
      (void)cm_mm_call_cmd_privacy_pref(
#else
      (void)cm_call_cmd_privacy_pref(
#endif
                                     NULL, NULL, 0,
#ifdef FEATURE_CM_MULTIMODE
                                                   0,
#endif
                                                      CM_PRIVACY_PREF_STANDARD);
   }
   else if (*(byte *)pBuff == 1) {
#ifdef FEATURE_CM_MULTIMODE
      (void)cm_mm_call_cmd_privacy_pref(
#else
      (void)cm_call_cmd_privacy_pref(
#endif
                                     NULL, NULL, 0,
#ifdef FEATURE_CM_MULTIMODE
                                                   0,
#endif
                                                      CM_PRIVACY_PREF_ENHANCED);
   }
   else {
      return EFAILED;
   }
#endif
   return SUCCESS;
}
//屏保时间
static int OEMPriv_GetItem_CFGI_SCREENSAVER_TIME(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.p_screensaver_time;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SCREENSAVER_TIME(void *pBuff)
{
   if (oemi_cache.p_screensaver_time != *(byte *)pBuff)
   {
      oemi_cache.p_screensaver_time = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}
//背光亮度级别
static int OEMPriv_GetItem_CFGI_BACKLIGHT_LEVEL(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.backlight_level;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_BACKLIGHT_LEVEL(void *pBuff)
{
   if (oemi_cache.backlight_level != *(byte *)pBuff)
    {
        oemi_cache.backlight_level = *(byte *)pBuff;
        //pm_lcd_bright_level = oemi_cache.backlight_level;
        //pm_kbd_bright_level = oemi_cache.backlight_level;
        OEMPriv_WriteOEMConfigList();
    }
   return SUCCESS;
}
#ifdef FEATURE_SET_AUTOKEYGUARD
static int OEMPriv_GetItem_CFGI_AUTOKEYGUARD(void *pBuff) 
{
   *(byte *) pBuff = oemi_cache.autokey_guard;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_AUTOKEYGUARD(void *pBuff) 
{
   if (oemi_cache.autokey_guard != *(byte *)pBuff) {
      oemi_cache.autokey_guard = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}
#endif
static int OEMPriv_GetItem_CFGI_KEYTONE_LENGTH(void *pBuff) 
{
   *(keyToneLength *) pBuff = nvi_cache.key_tone_length;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_KEYTONE_LENGTH(void *pBuff) 
{
#ifndef WIN32
    nv_item_type nvi;   
    if (nvi_cache.key_tone_length != *((keyToneLength *)pBuff)) 
    {
        nvi_cache.key_tone_length = *((keyToneLength *)pBuff);
        nvi.cont_key_dtmf = *((boolean *)pBuff);
        (void) OEMNV_Put( NV_CONT_KEY_DTMF_I, &nvi);
    }
#endif
    return SUCCESS;
}

//static int OEMPriv_GetItem_CFGI_KEYPAD_LIGHT_TIME(void *pBuff) 
//{
//   *(byte *) pBuff = oemi_cache.keypad_light_time;
//   return SUCCESS;
//}

//static int OEMPriv_SetItem_CFGI_KEYPAD_LIGHT_TIME(void *pBuff) 
//{
//   if (oemi_cache.keypad_light_time != *(byte *)pBuff) {
//      oemi_cache.keypad_light_time = *(byte *)pBuff;
//      OEMPriv_WriteOEMConfigList();
//   }
//
//   return SUCCESS;
//}

static int OEMPriv_GetItem_CFGI_PHONEBOOK_LOCK_CHECK(void *pBuff) 
{
   *(byte *) pBuff = oemi_cache.b_phonebook_lock;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PHONEBOOK_LOCK_CHECK(void *pBuff) 
{
   if (oemi_cache.b_phonebook_lock != *(byte *)pBuff) {
      oemi_cache.b_phonebook_lock = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_RECENTCALL_LOCK_CHECK(void *pBuff) 
{
   *(byte *) pBuff = oemi_cache.b_recentcall_lock;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_RECENTCALL_LOCK_CHECK(void *pBuff) 
{
   if (oemi_cache.b_recentcall_lock != *(byte *)pBuff) {
      oemi_cache.b_recentcall_lock = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_LOCK_CHECK(void *pBuff) 
{
   *(byte *) pBuff = oemi_cache.b_sms_lock;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_LOCK_CHECK(void *pBuff) 
{
   if (oemi_cache.b_sms_lock != *(byte *)pBuff) {
      oemi_cache.b_sms_lock = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CALENDAR_LOCK_CHECK(void *pBuff) 
{
   *(byte *) pBuff = oemi_cache.b_calendar_lock;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CALENDAR_LOCK_CHECK(void *pBuff) 
{
   if (oemi_cache.b_calendar_lock != *(byte *)pBuff) {
      oemi_cache.b_calendar_lock = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_KEY_LOCK_CHECK(void *pBuff) 
{
   *(byte *) pBuff = oemi_cache.b_key_lock;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_KEY_LOCK_CHECK(void *pBuff) 
{
   if (oemi_cache.b_key_lock != *(byte *)pBuff) {
      oemi_cache.b_key_lock = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_LOCK_RUIM(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.lock_ruim;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_LOCK_RUIM(void *pBuff)
{
   if (oemi_cache.lock_ruim != *(byte *)pBuff) {
      oemi_cache.lock_ruim = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_LOCK_MCCMNC_LIST(void *pBuff)
{

   memcpy((void *)pBuff,(void *)oemi_cache.lock_mccmnc,sizeof(oemi_cache.lock_mccmnc));
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_LOCK_MCCMNC_LIST(void *pBuff)
{
   memcpy((void *)oemi_cache.lock_mccmnc,(void *)pBuff,sizeof(oemi_cache.lock_mccmnc));
   OEMPriv_WriteOEMConfigList();  
   return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_DESKTOP_THEME(void *pBuff) 
{
   *(byte *) pBuff = oemi_cache.desktop_theme;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_DESKTOP_THEME(void *pBuff) 
{
   if (oemi_cache.desktop_theme != *(byte *)pBuff) {
      oemi_cache.desktop_theme = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_WMS_PRIORITY(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.wms_priority;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_WMS_PRIORITY(void *pBuff)
{
   if (oemi_cache.wms_priority != *(byte *)pBuff) {
      oemi_cache.wms_priority = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_WMS_SENDMODE(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.wms_sendmode;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_WMS_SENDMODE(void *pBuff)
{
   if (oemi_cache.wms_sendmode != *(byte *)pBuff) {
      oemi_cache.wms_sendmode = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_WMS_STORETYPE(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.wms_storetype;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_WMS_STORETYPE(void *pBuff)
{
   if (oemi_cache.wms_storetype != *(byte *)pBuff) {
      oemi_cache.wms_storetype = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_WMS_DELIVERYREPORTS(void *pBuff)
{
   *(boolean *) pBuff = oemi_cache.wms_delivereports;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_WMS_DELIVERYREPORTS(void *pBuff)
{
   if (oemi_cache.wms_delivereports != *(boolean *)pBuff) {
      oemi_cache.wms_delivereports = *(boolean *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

#ifdef FEATURE_AUTOREPLACE
static int OEMPriv_GetItem_CFGI_WMS_AUTOREPLACE(void *pBuff)
{
   *(boolean *) pBuff = oemi_cache.wms_autoreplace;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_WMS_AUTOREPLACE(void *pBuff)
{
   if (oemi_cache.wms_autoreplace != *(boolean *)pBuff) {
      oemi_cache.wms_autoreplace = *(boolean *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}
#endif /* FEATURE_AUTOREPLACE */

static int OEMPriv_GetItem_CFGI_WMS_VALIDITYPERIOD(void *pBuff)
{
    if (NULL == pBuff)
    {
        return EFAILED;
    }
    *(byte *) pBuff = oemi_cache.wms_validityperiod;
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_WMS_VALIDITYPERIOD(void *pBuff)
{
    if (NULL == pBuff)
    {
        return EFAILED;
    }
    
    if (oemi_cache.wms_validityperiod != *(byte *)pBuff) 
    {
        oemi_cache.wms_validityperiod = *(byte *)pBuff;
        OEMPriv_WriteOEMConfigList();
    }
    
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_WMS_MEMSTORE(void *pBuff)
{
    if (NULL == pBuff)
    {
        return EFAILED;
    }
    *(wms_memory_store_e_type *) pBuff = oemi_cache.wms_memstore;
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_WMS_MEMSTORE(void *pBuff)
{
    if (NULL == pBuff)
    {
        return EFAILED;
    }
    
    if (oemi_cache.wms_memstore != *(wms_memory_store_e_type *)pBuff) 
    {
        oemi_cache.wms_memstore = *(wms_memory_store_e_type *)pBuff;
        OEMPriv_WriteOEMConfigList();
    }
    
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_WMS_RESVDMSGALERT_TIMEIOUT(void *pBuff)
{
    if (NULL == pBuff)
    {
        return EFAILED;
    }
    *(boolean *) pBuff = oemi_cache.wms_resvdmsgalert_timeout;
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_WMS_RESVDMSGALERT_TIMEIOUT(void *pBuff)
{
    if (NULL == pBuff)
    {
        return EFAILED;
    }
    
    if (oemi_cache.wms_resvdmsgalert_timeout != *(boolean *)pBuff) 
    {
        oemi_cache.wms_resvdmsgalert_timeout = *(boolean *)pBuff;
        OEMPriv_WriteOEMConfigList();
    }
    
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_WMS_CALLBACKNUMSWITCH(void *pBuff)
{
    if (NULL == pBuff)
    {
        return EFAILED;
    }
    *(boolean *) pBuff = oemi_cache.callbacknum_switch;
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_WMS_CALLBACKNUMSWITCH(void *pBuff)
{
    if (NULL == pBuff)
    {
        return EFAILED;
    }
    
    if (oemi_cache.callbacknum_switch != *(boolean *)pBuff) 
    {
        oemi_cache.callbacknum_switch = *(boolean *)pBuff;
        OEMPriv_WriteOEMConfigList();
    }
    
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CALLBACKNUM(void *pBuff)
{
    int i;
    AECHAR *pwstrTep = (AECHAR *)pBuff;
    
    if (NULL == pwstrTep)
    {
        return EFAILED;
    }
    
    for (i=0; (i<WMS_ADDRESS_MAX-1) && oemi_cache.callback_number[i] != 0; i++)
    {
        pwstrTep[i] = oemi_cache.callback_number[i];
    }
    pwstrTep[i] = 0;
    
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CALLBACKNUM(void *pBuff)
{
    int i, nLen;
    AECHAR *pwstrTep = (AECHAR *)pBuff;
    
    if (NULL == pwstrTep)
    {
        return EFAILED;
    }
    
    nLen = WSTRLEN(pwstrTep);
    nLen = (nLen <= (WMS_ADDRESS_MAX-1) ? nLen : (WMS_ADDRESS_MAX-1));
    for (i=0; i<nLen; i++)
    {
        oemi_cache.callback_number[i] = pwstrTep[i];
    }
    oemi_cache.callback_number[i] = 0;
    
    OEMPriv_WriteOEMConfigList();
    
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_WMS_MO_CHANNEL_SELECT(void *pBuff)
{
    if (NULL == pBuff)
    {
        return EFAILED;
    }
    *(byte *) pBuff = oemi_cache.wms_mochannel;
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_WMS_MO_CHANNEL_SELECT(void *pBuff)
{
    if (NULL == pBuff)
    {
        return EFAILED;
    }
    
    if (oemi_cache.wms_mochannel != *(byte *)pBuff) 
    {
        oemi_cache.wms_mochannel = *(byte *)pBuff;
        OEMPriv_WriteOEMConfigList();
    }
    
    return SUCCESS;
}

#ifdef FEATRUE_AUTO_POWER
static int OEMPriv_GetItem_CFGI_AUTO_POWER_ON(void *pBuff) 
{
   MEMCPY(pBuff, (void*) &oemi_cache.m_auto_poweron, sizeof(Auto_Power_Cfg));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_AUTO_POWER_ON(void *pBuff) 
{
    MEMCPY((void*) &oemi_cache.m_auto_poweron, pBuff, sizeof(Auto_Power_Cfg));
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_AUTO_POWER_OFF(void *pBuff) 
{
   MEMCPY(pBuff, (void*) &oemi_cache.m_auto_poweroff, sizeof(Auto_Power_Cfg));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_AUTO_POWER_OFF(void *pBuff) 
{
    MEMCPY((void*) &oemi_cache.m_auto_poweroff, pBuff, sizeof(Auto_Power_Cfg));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
#endif
static int OEMPriv_GetItem_CFGI_POWERDOWN_ALARM(void *pBuff) 
{
   MEMCPY(pBuff, (void*) &oemi_cache.powerdown_alarm, sizeof(PowerDown_Alarm_Cfg));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_POWERDOWN_ALARM(void *pBuff) 
{
    MEMCPY((void*) &oemi_cache.powerdown_alarm, pBuff, sizeof(PowerDown_Alarm_Cfg));
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
#ifdef FEATRUE_SET_IP_NUMBER
static int OEMPriv_GetItem_CFGI_IP_NUMBER(void *pBuff) 
{
   
   MEMCPY(pBuff, (void*) &oemi_cache.m_ip_number, sizeof(IP_Number_Cfg));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_IP_NUMBER(void *pBuff) 
{
    MEMCPY((void*) &oemi_cache.m_ip_number, pBuff, sizeof(IP_Number_Cfg));
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_IP_POUND(void *pBuff) 
{
   
   *(boolean *) pBuff = oemi_cache.m_b_ip_pound;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_IP_POUND(void *pBuff) 
{
    if (oemi_cache.m_b_ip_pound != *(boolean *)pBuff) {
      oemi_cache.m_b_ip_pound = *(boolean *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}
#endif

#ifdef FEATURE_SHORT_CODE_NAM_COUNT
static int OEMPriv_GetItem_CFGI_SHORT_CODE_NAM_COUNT(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.short_code_nam_count;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SHORT_CODE_NAM_COUNT(void *pBuff)
{
   if (oemi_cache.short_code_nam_count != *(byte *)pBuff) {
      oemi_cache.short_code_nam_count = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}
#endif //#ifdef FEATURE_SHORT_CODE_NAM_COUNT

#ifdef FEATURE_LED_CONTROL
static int OEMPriv_GetItem_CFGI_LED_CONTROL(void *pBuff) 
{
   *(byte *) pBuff = oemi_cache.m_led_control;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_LED_CONTROL(void *pBuff) 
{
   if (oemi_cache.m_led_control != *(byte *)pBuff) {
      oemi_cache.m_led_control = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}
#endif

#ifdef FEATRUE_KEY_PAD_CTL
static int OEMPriv_GetItem_CFGI_KEY_PAD_CTL(void *pBuff) 
{
   MEMCPY(pBuff, (void*) &oemi_cache.m_key_pad_ctl, sizeof(Key_pad_Cfg));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_KEY_PAD_CTL(void *pBuff) 
{
    MEMCPY((void*) &oemi_cache.m_key_pad_ctl, pBuff, sizeof(Key_pad_Cfg));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
#endif
static int OEMPriv_GetItem_CFGI_AUTO_REDIAL(void *pBuff)
{
   nv_auto_redial_type * point = (nv_auto_redial_type *) pBuff;
   point->enable = nvi_cache.auto_redial.enable;
   point->rings = nvi_cache.auto_redial.rings;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_AUTO_REDIAL(void *pBuff)
{
#ifndef WIN32
    nv_item_type nvi;
    nv_auto_redial_type * point = (nv_auto_redial_type *) pBuff;
    if((nvi_cache.auto_redial.enable != point->enable)
        || (nvi_cache.auto_redial.rings != point->rings))
    {
        nvi_cache.auto_redial.enable = point->enable;
        nvi_cache.auto_redial.rings   = point->rings;
        nvi.auto_redial.enable = point->enable;
        nvi.auto_redial.rings   = point->rings;
        (void) OEMNV_Put( NV_AUTO_REDIAL_I, &nvi);
    }
#endif
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_EMERGENCYNUM_TABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.emerg_table, sizeof(EmergencyNum_Table));
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_EMERGENCYNUM_TABLE(void *pBuff)
{
    MEMCPY((void*) &oemi_cache.emerg_table, pBuff, sizeof(EmergencyNum_Table));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CALLFORWARD_BUSY_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.BUSY_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_BUSY_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.BUSY_ENABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.BUSY_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_BUSY_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.BUSY_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_BUSY_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.BUSY_DISABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.BUSY_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOANSWER_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.NOANSWER_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOANSWER_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.NOANSWER_ENABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.NOANSWER_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOANSWER_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.NOANSWER_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOANSWER_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.NOANSWER_DISABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.NOANSWER_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOCONNECT_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.NOCONNECT_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOCONNECT_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.NOCONNECT_ENABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.NOCONNECT_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOCONNECT_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.NOCONNECT_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOCONNECT_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.NOCONNECT_DISABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.NOCONNECT_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_ANYWAY_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.ANYWAY_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_ANYWAY_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.ANYWAY_ENABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.ANYWAY_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_ANYWAY_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.ANYWAY_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_ANYWAY_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.ANYWAY_DISABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.ANYWAY_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_DISABLE_ALL(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.DISABLE_ALL, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_DISABLE_ALL(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.DISABLE_ALL) != 0)
    {
        STRLCPY((char*) &oemi_cache.DISABLE_ALL, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_WAIT_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.WAIT_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_WAIT_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.WAIT_ENABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.WAIT_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_WAIT_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.WAIT_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_WAIT_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.WAIT_DISABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.WAIT_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_VOICEMAIL_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.VOICEMAIL_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_VOICEMAIL_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.VOICEMAIL_ENABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.VOICEMAIL_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SERVICE_PROVIDER(void *pBuff)
{

   memcpy((void *)pBuff,(void *)oemi_cache.List_SP,sizeof(ServiceProvider)*OEMNV_MAX_SERVICEPROVIDER_NUMBER);
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_SERVICE_PROVIDER(void *pBuff)
{
   memcpy((void *)oemi_cache.List_SP,(void *)pBuff,sizeof(ServiceProvider)*OEMNV_MAX_SERVICEPROVIDER_NUMBER);
   OEMPriv_WriteOEMConfigList();  
   return SUCCESS;
}

//输入法设置
static int OEMPriv_GetItem_CFGI_INPUTMODE(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.input_mode;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_INPUTMODE(void *pBuff)
{
   if (oemi_cache.input_mode != *(byte *)pBuff) {
      oemi_cache.input_mode = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

boolean OEM_IsEmergency_Number(char *pNumber,int len)
{
    boolean is_emergency = FALSE;
    int i =0;
    //DBGPRINTF("%s OEM_IsEmergency_Number",pNumber);
    if(oemi_cache.emerg_table.emert_size == 0)
    {
        //DBGPRINTF("emerg_table size is 0");
        return FALSE;
    }
    
    if(pNumber)
    {
        for (i = 0; i < oemi_cache.emerg_table.emert_size; i++)
        {
            //DBGPRINTF("%s %d OEM_IsEmergency_Number1",oemi_cache.emerg_table.emerg_num[i].num_buf,oemi_cache.emerg_table.emerg_num[i].num_len);
            if(len == oemi_cache.emerg_table.emerg_num[i].num_len)
            {
                if(STRNCMP(pNumber,oemi_cache.emerg_table.emerg_num[i].num_buf,oemi_cache.emerg_table.emerg_num[i].num_len) == 0)
                {
                    is_emergency = TRUE;
                    break;
                }
            }
        }
    }
    return is_emergency;
}

static int OEMPriv_GetItem_CFGI_FMRADIO_VOLUME(void *pBuff) 
{
   *(byte *) pBuff = oemi_cache.fmRadio_volume;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_FMRADIO_VOLUME(void *pBuff) 
{
   if (oemi_cache.fmRadio_volume != *(byte *)pBuff) {
      oemi_cache.fmRadio_volume = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_FMRADIO_CHAN_INFO(void *pBuff) 
{
   
   MEMCPY(pBuff, (void*) &oemi_cache.fmRadio_chan_info, sizeof(sChanInfo) * MAX_FMRADIO_STORED_CHANNEL);
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_FMRADIO_CHAN_INFO(void *pBuff)
{
    MEMCPY((void*) &oemi_cache.fmRadio_chan_info, pBuff, sizeof(sChanInfo) * MAX_FMRADIO_STORED_CHANNEL);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_FMRADIO_CHAN_TOTAL(void *pBuff) 
{
   *(byte *) pBuff = oemi_cache.fmRadio_chan_total;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_FMRADIO_CHAN_TOTAL(void *pBuff)
{
   if (oemi_cache.fmRadio_chan_total != *(byte *)pBuff) {
      oemi_cache.fmRadio_chan_total = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

/*==============================================================================
函数：
       GetRepeatRawNumber
       
说明：
       所有保存于recent call中的记录，前两位都用于保存当前的号码重复条数。
       得到raw　number.用于把连续重复的号码记录为一条记录。

参数：
        [in]number,包含'重复数'的号码记录
        [out]rawnumber，不包含'重复数'的号码
        
      
返回值：
       无。

备注：:

==============================================================================*/
int GetRepeatRawNumber(AECHAR * number,AECHAR * rawnumber)
{
    AECHAR wszCount[3];
    char   szCount[3];
    int    nCount = 0;

    (void)WSTRNCOPYN ( wszCount, 3, number, 2 );
    
    // get nCount
    nCount = ATOI( WSTRTOSTR( wszCount, szCount, CALLHISTORY_MAXDIGITS ) );
    (void)WSTRNCOPYN ( rawnumber, CALLHISTORY_MAXDIGITS, number+2, -1 );
    
    if ( nCount > 0 && nCount <= 99 )
        return nCount;      
    else
        return 0;
}
static int OEMPriv_GetItem_CFGI_KEYGUARD_ENABLED(void *pBuff)
{
#ifdef FEATURE_KEYGUARD
   *(boolean *) pBuff = OEMKeyguard_IsEnabled(); 
#else
   *(boolean *) pBuff = FALSE;
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_KEYGUARD_ENABLED(void *pBuff)
{
#ifdef FEATURE_KEYGUARD
   OEMKeyguard_SetState(*(boolean *) pBuff);
#endif
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_FM_BACKGROUND(void *pBuff) 
{
   MEMCPY(pBuff, (void*) &oemi_cache.m_fm_background, sizeof(boolean));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_FM_BACKGROUND(void *pBuff) 
{
    MEMCPY((void*) &oemi_cache.m_fm_background, pBuff, sizeof(boolean));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}

#ifdef FEATURE_RANDOM_MENU_COLOR
static int OEMPriv_GetItem_CFGI_MENU_BGCOLOR(void *pBuff) 
{
   MEMCPY(pBuff, (void*) &oemi_cache.m_nmenubgcolor, sizeof(uint32));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MENU_BGCOLOR(void *pBuff) 
{
    MEMCPY((void*) &oemi_cache.m_nmenubgcolor, pBuff, sizeof(uint32));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_RANDOM_MENU(void *pBuff)
{
   MEMCPY(pBuff, (void*) &oemi_cache.m_nrandommenu, sizeof(byte));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_RANDOM_MENU(void *pBuff)
{
    MEMCPY((void*) &oemi_cache.m_nrandommenu, pBuff, sizeof(byte));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add
static int OEMPriv_GetItem_CFGI_DEFAULT_REND(void *pBuff) 
{
	MEMCPY(pBuff, (void*) &oemi_cache.m_ndefaulerend, sizeof(uint32));
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_DEFAULT_REND(void *pBuff) 
{
    MEMCPY((void*) &oemi_cache.m_ndefaulerend, pBuff, sizeof(uint32));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_REND_STATE(void *pBuff)
{
	MEMCPY(pBuff, (void*) &oemi_cache.m_nrendstate, sizeof(byte));
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_REND_STATE(void *pBuff)
{
    MEMCPY((void*) &oemi_cache.m_nrendstate, pBuff, sizeof(byte));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
#endif//FEATURE_RANDOM_MENU_REND
#ifdef FEATURE_TOUCHPAD
static int OEMPriv_GetItem_CFGI_PEN_CAL(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.m_pencal_data, sizeof(pen_cal_type));
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PEN_CAL(void *pBuff)
{
    MEMCPY((void*) &oemi_cache.m_pencal_data, pBuff, sizeof(pen_cal_type));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
#endif//FEATURE_TOUCHPAD
static AEECMPrivacyPref OEM_PrivacyPrefToAEECM(cm_privacy_pref_e_type pref) 
{
  switch(pref) {
    case CM_PRIVACY_PREF_STANDARD:
      return AEECM_PRIVACY_PREF_STANDARD;

    case CM_PRIVACY_PREF_ENHANCED:
      return AEECM_PRIVACY_PREF_ENHANCED;

    default:
      return AEECM_PRIVACY_PREF_NONE;
  }
}
static cm_privacy_pref_e_type OEM_PrivacyPrefToDMSS(AEECMPrivacyPref pref) 
{
  switch(pref) {
    case AEECM_PRIVACY_PREF_STANDARD:
      return CM_PRIVACY_PREF_STANDARD;

    case AEECM_PRIVACY_PREF_ENHANCED:
      return CM_PRIVACY_PREF_ENHANCED;

    default:
      return CM_PRIVACY_PREF_NONE;
  }
}



static int OEMPriv_GetItem_CFGI_ALARM_FLAG(void *pBuff)
{
   MEMCPY(pBuff, (void*) &oemi_cache.alarm_flag, sizeof(boolean));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_ALARM_FLAG(void *pBuff)
{
    MEMCPY((void*) &oemi_cache.alarm_flag, pBuff, sizeof(boolean));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}

#ifdef FEATURE_SPORTS_APP
static int OEMPriv_GetItem_CFGI_SPORT_FLAG(void *pBuff)
{
   MEMCPY(pBuff, (void*) &oemi_cache.sport_flag, sizeof(uint16));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SPORT_FLAG(void *pBuff)
{
    MEMCPY((void*) &oemi_cache.sport_flag, pBuff, sizeof(uint16));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
#endif

static int OEMPriv_GetItem_CFGI_MENU_STYLE(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.menu_style;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MENU_STYLE(void *pBuff)
{
   if (oemi_cache.menu_style != *(byte *)pBuff) {
      oemi_cache.menu_style = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CAMERA_ENVIROMENT(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.camera_enviroment;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CAMERA_ENVIROMENT(void *pBuff)
{
   if (oemi_cache.camera_enviroment != *(byte *)pBuff) {
      oemi_cache.camera_enviroment = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CAMERA_QUALITY(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.camera_quality;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CAMERA_QUALITY(void *pBuff)
{
   if (oemi_cache.camera_quality != *(byte *)pBuff) {
      oemi_cache.camera_quality = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CAMERA_SIZE(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.camera_size;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CAMERA_SIZE(void *pBuff)
{
   if (oemi_cache.camera_size != *(byte *)pBuff) {
      oemi_cache.camera_size = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CAMERA_TONE(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.camera_tone;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CAMERA_TONE(void *pBuff)
{
   if (oemi_cache.camera_tone != *(byte *)pBuff) {
      oemi_cache.camera_tone = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CAMERA_BANDING(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.camera_banding;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CAMERA_BANDING(void *pBuff)
{
   if (oemi_cache.camera_banding != *(byte *)pBuff) {
      oemi_cache.camera_banding = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CAMERA_STORAGE(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.camera_storage;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CAMERA_STORAGE(void *pBuff)
{
   if (oemi_cache.camera_storage != *(byte *)pBuff) {
      oemi_cache.camera_storage = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CAMERA_BRIGHTNESS(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.camera_brightness;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CAMERA_BRIGHTNESS(void *pBuff)
{
   if (oemi_cache.camera_brightness != *(byte *)pBuff) {
      oemi_cache.camera_brightness = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_VIDEO_ENVIROMENT(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.video_enviroment;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_VIDEO_ENVIROMENT(void *pBuff)
{
   if (oemi_cache.video_enviroment != *(byte *)pBuff) {
      oemi_cache.video_enviroment = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CAMERA_ICON(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.camera_icon;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CAMERA_ICON(void *pBuff)
{
   if (oemi_cache.camera_icon != *(byte *)pBuff) {
      oemi_cache.camera_icon = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_VIDEO_ICON(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.video_icon;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_VIDEO_ICON(void *pBuff)
{
   if (oemi_cache.video_icon != *(byte *)pBuff) {
      oemi_cache.video_icon = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CAMERA_FRAME(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.camera_frame;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CAMERA_FRAME(void *pBuff)
{
   if (oemi_cache.camera_frame != *(byte *)pBuff) {
      oemi_cache.camera_frame = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_CAMERA_COLOR(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.camera_color;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CAMERA_COLOR(void *pBuff)
{
   if (oemi_cache.camera_color != *(byte *)pBuff) {
      oemi_cache.camera_color = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_VIDEO_COLOR(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.video_color;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_VIDEO_COLOR(void *pBuff)
{
   if (oemi_cache.video_color != *(byte *)pBuff) {
      oemi_cache.video_color = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_VIDEO_BRIGHTNESS(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.video_brightness;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_VIDEO_BRIGHTNESS(void *pBuff)
{
   if (oemi_cache.video_brightness != *(byte *)pBuff) {
      oemi_cache.video_brightness = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

#ifdef FEATURE_PLANEMODE
static int OEMPriv_GetItem_CFGI_PLANEMODE(void *pBuff)
{
    *(byte *) pBuff = oemi_cache.planeMode;
    
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_PLANEMODE(void *pBuff)
{
    if (oemi_cache.planeMode != *(byte *)pBuff) {
       oemi_cache.planeMode = *(byte *)pBuff;
       OEMPriv_WriteOEMConfigList();
    }
    
    return SUCCESS;
}
#endif

#ifdef FEATURE_USB_FUNCTION_SELECT
static int OEMPriv_GetItem_CFGI_USB_FUNCTION(void *pBuff) 
{
   MEMCPY(pBuff, (void*) &oemi_cache.usb_function, sizeof(OEMUSBFUNCTION));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_USB_FUNCTION(void *pBuff) 
{
    MEMCPY((void*) &oemi_cache.usb_function, pBuff, sizeof(OEMUSBFUNCTION));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}   
#endif //FEATURE_USB_FUNCTION_SELECT

static int OEMPriv_GetItem_CFGI_MISSED_CALL_ICON(void *pBuff)
{
   *(boolean *) pBuff = oemi_cache.missed_call_icon;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MISSED_CALL_ICON(void *pBuff)
{
   if (oemi_cache.missed_call_icon != *(boolean *)pBuff) {
      oemi_cache.missed_call_icon = *(boolean *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
static int OEMPriv_GetItem_CFGI_CALLFORWARD_BUSY_VOICEMAIL(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.BUSY_VOICEMAIL, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_BUSY_VOICEMAIL(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.BUSY_VOICEMAIL) != 0)
    {
        STRLCPY((char*) &oemi_cache.BUSY_VOICEMAIL, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOANSWER_VOICEMAIL(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.NOANSWER_VOICEMAIL, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOANSWER_VOICEMAIL(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.NOANSWER_VOICEMAIL) != 0)
    {
        STRLCPY((char*) &oemi_cache.NOANSWER_VOICEMAIL, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOCONNECT_VOICEMAIL(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.NOCONNECT_VOICEMAIL, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOCONNECT_VOICEMAIL(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.NOCONNECT_VOICEMAIL) != 0)
    {
        STRLCPY((char*) &oemi_cache.NOCONNECT_VOICEMAIL, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_ANYWAY_VOICEMAIL(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.ANYWAY_VOICEMAIL, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_ANYWAY_VOICEMAIL(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.ANYWAY_VOICEMAIL) != 0)
    {
        STRLCPY((char*) &oemi_cache.ANYWAY_VOICEMAIL, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_WAIT_TEMP_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.WAIT_TEMP_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_WAIT_TEMP_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.WAIT_TEMP_DISABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.WAIT_TEMP_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_DND_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.DND_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_DND_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.DND_ENABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.DND_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_DND_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.DND_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_DND_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.DND_DISABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.DND_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_CNIR_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.CNIR_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_CNIR_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.CNIR_ENABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.CNIR_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_CNIR_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.CNIR_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_CNIR_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.CNIR_DISABLE) != 0)
    {
        STRLCPY((char*) &oemi_cache.CNIR_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
#endif
/*==============================================================================
函数:
    OEM_InitDefaultBREWParameters

说明:
    函数用于对 BREW Configuration Parameters 作初始缺省设置。不同运营商的要求可能
    不一样，新增运营商支持时需相应修改本函数。函数默认一组设置。

参数:
    None

返回值:
    None

备注:
    函数会对 AEEDownloadInfo 产生影响。
     
==============================================================================*/
static void OEM_InitDefaultBREWParameters(void)
{
#ifndef WIN32
    nv_item_type nvi;
    
    // BREW DNS IP1
    // BREW DNS IP2
    nvi.primary_dns = 0;
    (void)OEMNV_Put(NV_PRIMARY_DNS_I, &nvi);
    (void)OEMNV_Put(NV_SECONDARY_DNS_I, &nvi);
    
    // BREW ADS IP/URL
    (void)MEMSET((uint8*)nvi.brew_server, 0, sizeof(nvi.brew_server));
    (void)MEMSET((uint8*)nvi_cache.brew_server, 0, sizeof(nvi_cache.brew_server));
    (void)STRCPY((char *)nvi.brew_server, DEFAULT_BREW_SERVER);
    (void)STRCPY((char *)nvi_cache.brew_server, (char *)nvi.brew_server);
    (void)OEMNV_Put(NV_BREW_SERVER_I, &nvi);
    
    // Carrier ID
    nvi.brew_carrier_id = DEFAULT_BREW_CARRIER_ID;
    nvi_cache.brew_carrier_id = nvi.brew_carrier_id;
    (void)OEMNV_Put(NV_BREW_CARRIER_ID_I, &nvi);
    
    // Bkey
    (void)MEMSET((uint8*)nvi.brew_bkey, 0, sizeof(nvi.brew_bkey));
    (void)MEMSET((uint8*)nvi_cache.brew_bkey, 0, sizeof(nvi_cache.brew_bkey));
    (void)OEMNV_Put(NV_BREW_BKEY_I, &nvi);
    
    // Authorization Policy
    nvi.brew_auth_policy = DEFAULT_BREW_APOLICY;
    nvi_cache.brew_auth_policy = nvi.brew_auth_policy;
    (void)OEMNV_Put(NV_BREW_AUTH_POLICY_I, &nvi);   
     
    // Privacy Policy
    nvi.brew_privacy_policy = DEFAULT_BREW_PPOLICY;
    nvi_cache.brew_privacy_policy = nvi.brew_privacy_policy;
    (void)OEMNV_Put(NV_BREW_PRIVACY_POLICY_I, &nvi);
    
    // Subscriber Id (SID)
    (void)MEMSET((uint8*)nvi.brew_subscriber_id, 0, sizeof(nvi.brew_subscriber_id));
    (void)MEMSET((uint8*)nvi_cache.brew_subscriber_id, 0, sizeof(nvi_cache.brew_subscriber_id));
    (void)OEMNV_Put(NV_BREW_SUBSCRIBER_ID_I, &nvi);
    
    // Platform ID
    nvi.brew_platform_id = DEFAULT_BREW_PLATFORM_ID;
    nvi_cache.brew_platform_id = nvi.brew_platform_id;
    (void)OEMNV_Put(NV_BREW_PLATFORM_ID_I, &nvi);
    
    // Download Flags 
    nvi.brew_download_flags = DEFAULT_BREW_DOWNLOAD_FLG;
    nvi_cache.brew_download_flags = nvi.brew_download_flags;
    (void)OEMNV_Put(NV_BREW_DOWNLOAD_FLAGS_I, &nvi);
#endif
}

/*==============================================================================
函数:
    OEM_InitBREWParameters

说明:
    函数用于对 nvi_cache 中 BREW Configuration Parameters 初始化。

参数:
    None

返回值:
    None

备注:
    
     
==============================================================================*/
static void OEM_InitBREWParameters(void)
{
#ifndef WIN32
    nv_item_type nvi;
    uint32       i;
    
    // BREW ADS IP/URL
    nvi_cache.brew_server[0] = 0;
    nvi.brew_server[0] = 0;
    (void)OEMNV_Get(NV_BREW_SERVER_I, &nvi);
    (void)STRCPY((char *)nvi_cache.brew_server, (char *)nvi.brew_server);
    
    // Carrier ID
    nvi.brew_carrier_id = 0;
    (void)OEMNV_Get(NV_BREW_CARRIER_ID_I, &nvi);
    nvi_cache.brew_carrier_id = nvi.brew_carrier_id;
    
    // Bkey
    for (i=0; i<NV_BREW_BKEY_SIZ; i++)
    {
        nvi.brew_bkey[i] = 0;
    }
    (void)OEMNV_Get(NV_BREW_BKEY_I, &nvi);
    for (i=0; i<NV_BREW_BKEY_SIZ; i++)
    {
        nvi_cache.brew_bkey[i] = nvi.brew_bkey[i];
    }
    
    // Authorization Policy
    nvi.brew_auth_policy = APOLICY_NONE;
    (void)OEMNV_Get(NV_BREW_AUTH_POLICY_I, &nvi);
    nvi_cache.brew_auth_policy = nvi.brew_auth_policy;
     
    // Privacy Policy
    nvi.brew_privacy_policy = PPOLICY_BREW;
    (void)OEMNV_Get(NV_BREW_PRIVACY_POLICY_I, &nvi);
    nvi_cache.brew_privacy_policy = nvi.brew_privacy_policy;
    
    // Subscriber Id (SID)
    for (i=0; i<NV_BREW_SID_SIZ; i++)
    {
        nvi.brew_subscriber_id[i] = 0;
    }
    (void)OEMNV_Get(NV_BREW_SUBSCRIBER_ID_I, &nvi);
    for (i=0; i<NV_BREW_SID_SIZ; i++)
    {
        nvi_cache.brew_subscriber_id[i] = nvi.brew_subscriber_id[i];
    }
    
    // Platform ID
    nvi.brew_platform_id = 0;
    (void)OEMNV_Get(NV_BREW_PLATFORM_ID_I, &nvi);
    nvi_cache.brew_platform_id = nvi.brew_platform_id;
    
    // Download Flags (SID)
    nvi.brew_download_flags = 0;
    (void)OEMNV_Get(NV_BREW_DOWNLOAD_FLAGS_I, &nvi);
    nvi_cache.brew_download_flags = nvi.brew_download_flags;
#endif
} /* OEM_InitBREWParameters */

/*==============================================================================
函数:
    OEM_StoreDownloadInfo

说明:
    函数用于对 BREW Configuration Parameters 作初始缺省设置。不同运营商的要求可能
    不一样，新增运营商支持时需相应修改本函数。函数默认一组设置。

参数:
    None

返回值:
    None

备注:
    函数会对 AEEDownloadInfo 产生影响。
     
==============================================================================*/
static void OEM_StoreDownloadInfo(AEEDownloadInfo * pdi)
{
#ifndef WIN32
    nv_item_type nvi;
    uint32       i;
    
    if (NULL == pdi)
    {
        return;
    }
    
    // BREW ADS IP/URL
    nvi.brew_server[0] = 0;
    nvi_cache.brew_server[0] = 0;
    (void)STRCPY((char *)nvi.brew_server, pdi->szServer);
    (void)STRCPY((char *)nvi_cache.brew_server, pdi->szServer);
    (void)OEMNV_Put(NV_BREW_SERVER_I, &nvi);
    
    // Carrier ID
    nvi.brew_carrier_id = pdi->dwCarrierID;
    nvi_cache.brew_carrier_id = pdi->dwCarrierID;
    (void)OEMNV_Put(NV_BREW_CARRIER_ID_I, &nvi);
    
    // Bkey
    for (i=0; i<NV_BREW_BKEY_SIZ; i++)
    {
        if (i<DL_BKEY_SIZE)
        {
            nvi.brew_bkey[i] = pdi->bBKey[i];
            nvi_cache.brew_bkey[i] = pdi->bBKey[i];
        }
        else
        {
            nvi.brew_bkey[i] = 0;
            nvi_cache.brew_bkey[i] = 0;
        }
    }
    (void)OEMNV_Put(NV_BREW_BKEY_I, &nvi);
    
    // Authorization Policy
    nvi.brew_auth_policy = pdi->nAuth;
    nvi_cache.brew_auth_policy = pdi->nAuth;
    (void)OEMNV_Put(NV_BREW_AUTH_POLICY_I, &nvi);   
     
    // Privacy Policy
    nvi.brew_privacy_policy = pdi->nPolicy;
    nvi_cache.brew_privacy_policy = pdi->nPolicy;
    (void)OEMNV_Put(NV_BREW_PRIVACY_POLICY_I, &nvi);
        
    // Platform ID
    nvi.brew_platform_id = pdi->dwPlatformID;
    nvi_cache.brew_platform_id = pdi->dwPlatformID;
    (void)OEMNV_Put(NV_BREW_PLATFORM_ID_I, &nvi);
    
    // Download Flags (SID)
    nvi.brew_download_flags = pdi->wFlags;
    nvi_cache.brew_download_flags = pdi->wFlags;
    (void)OEMNV_Put(NV_BREW_DOWNLOAD_FLAGS_I, &nvi);
#endif
} /* OEM_StoreDownloadInfo */

//static int SetSubscriberID(byte *pBuff, int nSize)
//{
//   nv_item_type nvi; //buffer to read NV
//   int i; 
//   int numBytesToCopy; 
//			  
//   if (nSize > NV_BREW_SID_SIZ)
//   {
//      numBytesToCopy = NV_BREW_SID_SIZ; 
//   }
//   else 
//   {
//      numBytesToCopy = nSize; 
//   }
//   
//   // Copy subscriber ID
//   for (i = 0; i < numBytesToCopy; i++)
//   {
//      nvi.brew_subscriber_id[i] = pBuff[i];
//      nvi_cache.brew_subscriber_id[i] = pBuff[i];
//   }
//
//   // Pad the rest with the null char 
//   for (; i < NV_BREW_SID_SIZ; i++)
//   {
//      nvi.brew_subscriber_id[i] = '\0'; 
//      nvi_cache.brew_subscriber_id[i] = '\0'; 
//   }
//   
//   return OEMNV_Put(NV_BREW_SUBSCRIBER_ID_I, &nvi);
//}

/*==============================================================================
函数:
    OEM_SetBAM_ADSAccount

说明:
    函数用于将 BREW App Manager 的 MobileShop 账号信息设置到 NV 。

参数:
    None

返回值:
    None

备注:
    NV 的相关配置只起临时作用。不同应用的账号信息可能不一样。使用时需自行配置。
    本函数仅供 BREW App Manager 使用。
==============================================================================*/
void OEM_SetBAM_ADSAccount(void)
{
#if !defined WIN32
    nv_item_type nvi;
    
    // 账号
#ifdef FEATURE_DS_SIP_MULTIPLE_PROFILE
    nvi.ds_sip_nai_info.index = 0;
    (void)OEMNV_Put(NV_DS_SIP_ACTIVE_PROFILE_INDEX_I, &nvi);

    (void)STRCPY((char *)nvi.ds_sip_nai_info.nai, (char *)oemi_cache.brew_username);
    nvi.ds_sip_nai_info.nai_length = STRLEN((char *)oemi_cache.brew_username);
    (void)OEMNV_Put(NV_DS_SIP_NAI_INFO_I, &nvi);
#else /* FEATURE_DS_SIP_MULTIPLE_PROFILE */
    (void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)oemi_cache.brew_username);
    nvi.pap_user_id.user_id_len = STRLEN((char *)oemi_cache.brew_username);
    (void)OEMNV_Put(NV_PPP_USER_ID_I, &nvi);
#endif /* FEATURE_DS_SIP_MULTIPLE_PROFILE */

    // 账号密码
#ifdef FEATURE_DS_SIP_MULTIPLE_PROFILE
    nvi.ds_sip_nai_info.index = 0;
    
    (void)STRCPY((char *)nvi.ds_sip_ppp_ss_info.ss, (char *)oemi_cache.brew_password);
    nvi.ds_sip_ppp_ss_info.ss_length = STRLEN((char *)oemi_cache.brew_password);

    (void)OEMNV_Put(NV_DS_SIP_PPP_SS_INFO_I, &nvi);
#else /* FEATURE_DS_SIP_MULTIPLE_PROFILE */
    (void)STRCPY((char *)nvi.pap_password.password, (char *)oemi_cache.brew_password);
    nvi.pap_password.password_len = STRLEN((char *)oemi_cache.brew_password);
    
    (void)OEMNV_Put(NV_PPP_PASSWORD_I, &nvi);
#endif /* FEATURE_DS_SIP_MULTIPLE_PROFILE */
#endif
} /* OEM_SetBAM_ADSAccount */


